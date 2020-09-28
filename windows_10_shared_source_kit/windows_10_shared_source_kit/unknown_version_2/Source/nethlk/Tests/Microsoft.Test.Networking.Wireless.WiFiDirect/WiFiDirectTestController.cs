///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.RemoteControl;
using Windows.Devices.WiFiDirect.Services;
using Windows.Networking;
using Windows.Networking.Sockets;
using System.Threading.Tasks;
using System.Globalization;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    //
    // This is the main interface through which the Wi-Fi Direct Tests will control the Wi-Fi Direct stack.
    // It is built on the Microsoft.Test.Networking.Wireless.WiFiDirect common class, which directly interacts with the internal WFD APIs exposed by WlanApi.dll
    // This class can be used either directly to control the WFD stack on the local machine, or as a proxy object through the RemoteControl framework to control a remote machine
    //
    internal class WiFiDirectTestController : IDisposable
    {
        bool disposed;
        bool hasBeenResetOnce; // Used for performing one-time test cleanup, where we're not sure of the state when the test program was started

        TestContext testContext;
        bool isRemoteListener;

        // Interface for calling the Wi-Fi Direct APIs.  Holds the handle to the WFD APIs
        // This will be uninitialized if this WiFiDirectTestController object represents a remote instance
        WifiDirect wifiDirect;
        bool subscribedToWfdEvents;

        // Wrapper around Wi-Fi Direct Services APIs. Manages all publications, discoveries, sessions, and sockets
        // This will be uninitialized if this WiFiDirectTestController object represents a remote instance
        WiFiDirectServicesManager wfdServicesMgr;

        // RemoteControl client and the remote instance of WiFiDirectTestController
        // This will be uninitialized if this WiFiDirectTestController object represents a local instance
        RemoteCommandClient remoteCommandClient;
        dynamic remoteInstance;

        // Interface for calling the wlan APIs.
        Wlan wlan;

        // Information on the currently active session.
        Object sessionHandleLock = new Object();
        WFDSessionSafeHandle currentWfdSessionHandle;
        DOT11_MAC_ADDRESS peerDeviceAddress;
        Guid sessionInterface;

        // Flag to indicate the next group request should be accepted.
        bool acceptGroupRequestFlag;
        DOT11_MAC_ADDRESS groupRequestExpectedPeer;
        byte acceptGoIntent;
        DOT11_WPS_CONFIG_METHOD acceptConfigMethod;

        // Auto GO Status
        bool isAutoGoRunning;
        WFD_GROUP_ID autoGoGroupId = new WFD_GROUP_ID();
        string defaultGoProfile;
        bool mustCloseLegacySessionOnCleanup;
        DOT11_MAC_ADDRESS legacySessionPeer;

        TestIeManager ieManager;

        WiFiDirectDataPathTester dataPathTester;

        // Last used profile and last paired device address, used for reconnect.
        string lastUsedProfile = "";
        DOT11_MAC_ADDRESS lastPairedDevice;

        bool acceptConnectRequestFlag;
        bool mustRemoveProfile = false;

        // Constructor.  Initializes the Wi-Fi Direct APIs
        public WiFiDirectTestController(TestContext context, bool remoteListener = false)
        {
            // The WifiDirect object's implementation uses the logging framework in Microsoft.Test.Networking.Tracing.
            // Here we provide an event handler to this framework so we can direct that output to the TAEF log.
            Microsoft.Test.Networking.Tracing.TraceProvider.TraceEvent += TraceProvider_TraceEvent;

            testContext = context;
            isRemoteListener = remoteListener;

            wlan = new Wlan();

            WiFiDirectTestLogger.Log("Initializing Wi-Fi Direct API");
            wifiDirect = new WifiDirect();

            wifiDirect.WfdPeerStateDisconnected += OnPeerStateDisconnected;
            wifiDirect.WfdGroupRequestRecieved += OnGroupRequestReceived;
            wifiDirect.WfdConnectionRequestRecieved += OnConnectionRequestReceived;
            subscribedToWfdEvents = true;

            wfdServicesMgr = new WiFiDirectServicesManager();

            ieManager = new TestIeManager();

            dataPathTester = new WiFiDirectDataPathTester();
        }

        ~WiFiDirectTestController()
        {
            if (!disposed)
            {
                Dispose();
            }
        }

        public void Dispose()
        {
            disposed = true;

            Reset();

            if (dataPathTester != null)
            {
                dataPathTester.Dispose();
            }

            try
            {
                if (subscribedToWfdEvents)
                {
                    wifiDirect.WfdPeerStateDisconnected -= OnPeerStateDisconnected;
                    wifiDirect.WfdGroupRequestRecieved -= OnGroupRequestReceived;
                    wifiDirect.WfdConnectionRequestRecieved -= OnConnectionRequestReceived;
                    subscribedToWfdEvents = false;
                }

                lock(sessionHandleLock)
                {
                    if (currentWfdSessionHandle != null)
                    {
                        currentWfdSessionHandle.Dispose();
                    }
                }

                if (wifiDirect != null)
                {
                    wifiDirect.Dispose();
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception disposing WFD state. {0}", e);
            }

            try
            {
                if (wfdServicesMgr != null)
                {
                    wfdServicesMgr.Dispose();
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception disposing WFDS state. {0}", e);
            }

            try
            {
                if (wlan != null)
                {
                    wlan.Dispose();
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception disposing WLAN state. {0}", e);
            }

            if (remoteInstance != null)
            {
                remoteInstance.Dispose();
            }

            if (remoteCommandClient != null)
            {
                remoteCommandClient.Disconnect();
            }

            GC.SuppressFinalize(this);
        }

        // Private constructor used when creating a remote instance.
        private WiFiDirectTestController(RemoteCommandClient remoteCommandClient, dynamic remoteInstance)
        {
            this.remoteCommandClient = remoteCommandClient;
            this.remoteInstance = remoteInstance;
            this.testContext = null;
        }

        // Factory method for creating test controller for a remote device using RemoteControl
        public static WiFiDirectTestController CreateFromRemoteCommandClient(string remoteCommandServer, ushort remotePort)
        {
            // List of custom types being passed as parameters to remote methods
            Type[] serializationTypes = new Type[]
            {
                typeof(DOT11_MAC_ADDRESS),
                typeof(String),
                typeof(WFDSvcWrapperHandle),
                typeof(WiFiDirectServiceStatus),
                typeof(List<WiFiDirectServiceConfigurationMethod>),
                typeof(List<WFDSvcWrapperHandle>),
                typeof(DOT11_WPS_CONFIG_METHOD),
                typeof(List<ServiceAdvertiserInfo>),
                typeof(List<String>)
            };

            // List of custom types being returned from remote methods.
            Type[] returnTypes = new Type[]
            {
                typeof(DOT11_MAC_ADDRESS),
                typeof(WFDSvcWrapperHandle),
                typeof(ServiceAdvertiserInfo),
                typeof(DiscoveredServiceInfo),
                typeof(ProvisioningInfoWrapper),
                typeof(ServiceSessionInfo),
                typeof(List<WFDSvcWrapperHandle>),
                typeof(DOT11_WPS_CONFIG_METHOD)
            };

            WiFiDirectTestLogger.Log("Connecting to test server {0} on port {1}", remoteCommandServer, remotePort);

            Logger.SetAdditionalLogger(WEX.Logging.Interop.Log.Comment);
            RemoteCommandClient client = new RemoteCommandClient(serializationTypes, returnTypes);
            client.Connect(remoteCommandServer, remotePort);

            WiFiDirectTestLogger.Log("Connection to test server established.  Initializing remote test controller.", remoteCommandServer, remotePort);

            dynamic remoteInstance = RemoteCommandClient.GetRemoteInstance(client, typeof(WiFiDirectTestController));

            WiFiDirectTestLogger.Log("Remote test controller initialized successfully.", remoteCommandServer, remotePort);

            return new WiFiDirectTestController(client, remoteInstance);
        }

        // Deletes all Wlan Profiles from the machine, and disconnects from any networks.
        void DeleteAllWlanProfilesAndDisconnect()
        {
            List<WlanInterface> wlanInterfaceList = wlan.EnumWlanInterfaces();
            foreach(WlanInterface wlanInterface in wlanInterfaceList)
            {
                List<WlanProfileInfo> wlanProfileList = wlan.GetProfileList(wlanInterface.Id);
                foreach(WlanProfileInfo wlanProfileInfo in wlanProfileList)
                {
                    WiFiDirectTestLogger.Log("Deleting Wlan Profile \"{0}\" from interface {1}", wlanProfileInfo.ProfileName, wlanInterface.Id);
                    wlan.DeleteProfile(wlanInterface.Id, wlanProfileInfo.ProfileName);
                }

                if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    WiFiDirectTestLogger.Log("Disconnecting interface {0} from wireless network.", wlanInterface.Id);
                    wlan.Disconnect(wlanInterface.Id, new TimeSpan(0,0,5));
                }
            }
        }

        // Reset the device, disabling listen state, stopping GOs, etc.
        // Generally run in TestSetup for each test case, and finally during test class cleanup.
        // Returns true if the device is in a good state.
        public bool Reset()
        {
            if (remoteInstance != null)
            {
                return remoteInstance.Reset();
            }

            try
            {
                acceptGroupRequestFlag = false;
                acceptConnectRequestFlag = false;

                if (mustRemoveProfile)
                {
                    wifiDirect.WfdRemovePeristedProfile(lastPairedDevice, lastUsedProfile);
                }

                ConfigureListenState(false);

                dataPathTester.Reset();

                lock(sessionHandleLock)
                {
                    if (currentWfdSessionHandle != null)
                    {
                        currentWfdSessionHandle.Dispose();
                        currentWfdSessionHandle = null;
                    }
                }

                DeleteAllWlanProfilesAndDisconnect();

                wfdServicesMgr.ResetState();

                wifiDirect.WfdClearApplicationIe();
                ieManager.SetNewIeGuid(Guid.NewGuid()); // causes the IE to change on each test.

                // Shut off the auto-GO.  Here, we don't want to assume the test started the auto GO and the related members in this test controller object are accurate.
                // Instead, we need to handle the case where the GO may have been running when the test started
                if (!hasBeenResetOnce)
                {
                    WfdGlobalSessionState globalSessionState = wifiDirect.QueryGlobalSessionState();
                    foreach(WfdSessionState sessionState in globalSessionState.Sessions)
                    {
                        if (sessionState.Role == WFD_ROLE_TYPE.WFD_ROLE_TYPE_GROUP_OWNER && 
                            sessionState.AssociatedDevices.Length == 0)
                        {
                            wifiDirect.StopAutonomousGO(sessionState.GroupID);
                        }
                        else
                        {
                            WiFiDirectTestLogger.Error("Unable to setup test controller.  There is an active WFD session which is not owned by this process.");
                            return false;
                        }
                    }
                }
                else if (isAutoGoRunning)
                {
                    if (mustCloseLegacySessionOnCleanup)
                    {
                        wifiDirect.WfdCloseLegacySession(legacySessionPeer, defaultGoProfile);
                    }

                    wifiDirect.StopAutonomousGO(autoGoGroupId);
                    isAutoGoRunning = false;
                    defaultGoProfile = null;
                }

                hasBeenResetOnce = true;
                return true;
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception during test controller reset. {0}", e);
            }

            return false;
        }

        // Setup the device for the test
        // Returns true if the device is in a good state.
        public bool Setup()
        {
            bool result = false;

            if (remoteInstance != null)
            {
                return remoteInstance.Setup();
            }

            // Cleanup old state
            result = Reset();

            if (result & (testContext != null))
            {
                // Determine if we need to do the AP connection. This is based on parameters for the test
                if (testContext.Properties.Contains("EnableConcurrency"))
                {
                    string concurrencySetting = testContext.Properties["EnableConcurrency"] as string;

                    if ((concurrencySetting.Equals("both", StringComparison.OrdinalIgnoreCase)) ||      // Both sides
                        (concurrencySetting.Equals(isRemoteListener ? "sut" : "dut", StringComparison.OrdinalIgnoreCase)))  // This side only
                    {
                        // Concurrency is enabled. Get SSID/Password
                        WiFiDirectTestLogger.Log("Infrastructure concurrency enabled for Wi-Fi Direct testing. Setting up connection to AP");

                        try
                        {
                            string WPA2SSID = testContext.Properties["WPA2_PSK_AES_SSID"] as string;
                            string WPA2Password = testContext.Properties["WPA2_PSK_Password"] as string;

                            this.ConnectToWPA2AP(WPA2SSID, WPA2Password);
                        }
                        catch (Exception e)
                        {
                            WiFiDirectTestLogger.Error("Caught exception during connection to AP. {0}", e);
                            result = false;
                        }
                    }
                }
            }

            return result;
        }

        public void Log(string toLog)
        {
            if (remoteInstance != null)
            {
                remoteInstance.Log(toLog);
                return;
            }

            WiFiDirectTestLogger.Log(toLog);
        }

        public void StartGroup(string group)
        {
            if (remoteInstance != null)
            {
                remoteInstance.StartGroup(group);
                return;
            }

            WiFiDirectTestLogger.StartGroup(group);
        }

        public void EndGroup(string group)
        {
            if (remoteInstance != null)
            {
                remoteInstance.EndGroup(group);
                return;
            }

            WiFiDirectTestLogger.EndGroup(group);
        }

        // Machine name property
        public string MachineName
        {
            get
            {
                if (remoteInstance != null)
                {
                    return remoteInstance.MachineName;
                }

                return InteropUtilities.MachineName;
            }
        }

        // Device Address Property
        public DOT11_MAC_ADDRESS DeviceAddress
        {
            get
            {
                if (remoteInstance != null)
                {
                    return remoteInstance.DeviceAddress;
                }

                WfdCurrentDeviceState currentDeviceState = wifiDirect.QueryWFDCurrentDeviceState();

                return currentDeviceState.DeviceAddress;
            }
        }

        public Guid CurrentIEGuid
        {
            get
            {
                if (remoteInstance != null)
                {
                    return remoteInstance.CurrentIEGuid;
                }

                return ieManager.Guid;
            }
        }

        public Guid DataPathTestGuid
        {
            get
            {
                if (remoteInstance != null)
                {
                    return remoteInstance.DataPathTestGuid;
                }

                return dataPathTester.Guid;
            }
        }

        public string passKeyInternal;
        public string PassKey
        {
            get
            {
                if (remoteInstance != null)
                {
                    return remoteInstance.PassKey;
                }

                if (passKeyInternal == null)
                {
                    GenerateAndStoreNewPassKey();
                }

                return passKeyInternal;
            }

            set
            {
                if (remoteInstance != null)
                {
                    remoteInstance.PassKey = value;
                    return;
                }

                if (value == null ||
                    value.Length != 8 ||
                    Regex.IsMatch(value, @"[^0-9]"))
                {
                    throw new FormatException("Eight digit passkey expected.");
                }

                passKeyInternal = value;
            }
        }

        public string GenerateAndStoreNewPassKey()
        {
            // Generate random number between zero and 99999999
            Random random = new Random();
            int randomNumber = random.Next(100000000);
            string newPassKey = String.Format(CultureInfo.InvariantCulture, "{0:D8}", randomNumber);
            PassKey = newPassKey;
            return newPassKey;
        }

        public WfdGlobalSessionState QueryGlobalSessionState()
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("QueryGlobalSessionState method is not supported on a remote instance");
            }

            return wifiDirect.QueryGlobalSessionState();
        }

        public void AcceptNextGroupRequest(DOT11_MAC_ADDRESS groupRequestExpectedPeer, byte goIntent, DOT11_WPS_CONFIG_METHOD configMethod)
        {
            if (remoteInstance != null)
            {
                remoteInstance.AcceptNextGroupRequest(groupRequestExpectedPeer, goIntent, configMethod);
                return;
            }

            acceptGroupRequestFlag = true;
            this.groupRequestExpectedPeer = groupRequestExpectedPeer;
            acceptGoIntent = goIntent;
            acceptConfigMethod = configMethod;

            WiFiDirectTestLogger.Log("Expecting incoming group request from {0}", groupRequestExpectedPeer);
        }

        public void AcceptNextConnectRequest()
        {
            if (remoteInstance != null)
            {
                remoteInstance.AcceptNextConnectRequest();
                return;
            }

            acceptConnectRequestFlag = true;

            wifiDirect.WfdAddPersistedProfile(lastPairedDevice, lastUsedProfile);
            mustRemoveProfile = true;
        }

        // Check the current listen state, and change it if necessary.
        public void ConfigureListenState(bool enabled)
        {
            if (remoteInstance != null)
            {
                remoteInstance.ConfigureListenState(enabled);
                return;
            }

            bool currentStatus = wifiDirect.GetDiscoverableState();
            if (currentStatus != enabled)
            {
                WiFiDirectTestLogger.Log("{0} Listen State", enabled ? "Enabling" : "Disabling");
                wifiDirect.SetDiscoverableState(enabled);
            }
        }

        // Flush the visible device list
        public void FlushVisibleDeviceList()
        {
            if (remoteInstance != null)
            {
                remoteInstance.FlushVisibleDeviceList();
                return;
            }

            WiFiDirectTestLogger.Log("Flushing visible device list");
            wifiDirect.WfdFlushVisibleDeviceList();
        }

        // Discover devices
        public List<WfdDeviceDescriptor> DiscoverDevices(uint searchTimeoutMs, WFD_DISCOVER_TYPE discoverType)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("DiscoverDevices not supported on a remote instance.");
            }

            return wifiDirect.WfdDiscoverDevices(searchTimeoutMs, discoverType);
        }

        public WfdDeviceDescriptor PerformTargetedDiscovery(
            DOT11_MAC_ADDRESS targetDeviceAddress,
            DOT11_WFD_DISCOVER_DEVICE_FILTER_BITMASK deviceFilterBitmask,
            uint searchTimeoutMs,
            WFD_DISCOVER_TYPE discoverType)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("PerformTargettedDiscovery not supported on a remote instance.");
            }

            return wifiDirect.WfdPerformTargetedDiscovery(targetDeviceAddress, deviceFilterBitmask, searchTimeoutMs, discoverType);
        }

        public void PairWithDevice(
            DOT11_MAC_ADDRESS targetDeviceAddress,
            WFD_PAIR_WITH_DEVICE_PREFERENCE pairingPreference,
            byte goIntent,
            DOT11_WPS_CONFIG_METHOD configMethod,
            bool isPersistent)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("PairWithDevice not supported on a remote instance.");
            }

            lock(sessionHandleLock)
            {
                if (currentWfdSessionHandle != null)
                {
                    throw new Exception("WFDTestController cannot pair with a new device while it has an active WFD session");
                }

                peerDeviceAddress = targetDeviceAddress;
                WiFiDirectTestLogger.Log(
                    "Pairing with {0}, WPS Config Method: {1}, WPS PIN: {2}",
                    targetDeviceAddress, 
                    ConfigMethodToString(configMethod),
                    configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON ? PassKey : "None"
                    );
                currentWfdSessionHandle = wifiDirect.PairWithDevice(
                    targetDeviceAddress,
                    pairingPreference,
                    goIntent,
                    configMethod,
                    configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON ? PassKey : null,
                    isPersistent,
                    out sessionInterface,
                    out lastUsedProfile
                    );
                lastPairedDevice = targetDeviceAddress;
            }
        }

        public void OpenSessionToLastPairedDevice()
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("OpenSessionToLastPairedDevice not supported on a remote instance.");
            }

            lock(sessionHandleLock)
            {
                if (currentWfdSessionHandle != null)
                {
                    throw new Exception("WFDTestController cannot pair with a new device while it has an active WFD session");
                }

                wifiDirect.WfdAddPersistedProfile(lastPairedDevice, lastUsedProfile);
                mustRemoveProfile = true;

                WiFiDirectTestLogger.Log("Opening session with {0}", lastPairedDevice);
                currentWfdSessionHandle = wifiDirect.WfdOpenSession(lastPairedDevice, lastUsedProfile, out sessionInterface);
            }
        }

        public void CloseSession()
        {
            if (remoteInstance != null)
            {
                remoteInstance.CloseSession();
                return;
            }

            lock(sessionHandleLock)
            {
                WiFiDirectTestLogger.Log("Closing WFD session.");

                // The handle may already have been cleaned up by the disconnect notification.  Handle this internally without failing.
                if (currentWfdSessionHandle == null)
                {
                    return;
                }

                currentWfdSessionHandle.Dispose();
                currentWfdSessionHandle = null;
            }

        }

        public void StartAutonomousGo()
        {
            if (remoteInstance != null)
            {
                remoteInstance.StartAutonomousGo();
                return;
            }

            if (isAutoGoRunning)
            {
                return;
            }

            WiFiDirectTestLogger.Log("Starting autonomous GO");

            autoGoGroupId = wifiDirect.StartAutonomousGO();
            isAutoGoRunning = true;
        }

        public void StopAutonomousGo()
        {
            if (remoteInstance != null)
            {
                remoteInstance.StopAutonomousGo();
                return;
            }

            if (!isAutoGoRunning)
            {
                return;
            }

            WiFiDirectTestLogger.Log("Stopping autonomous GO");

            wifiDirect.StopAutonomousGO(autoGoGroupId);
            isAutoGoRunning = false;
        }

        #region WiFi-Direct Services
        public string GenerateUniqueServiceName(
            string prefix = "com.msft.kit"
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GenerateUniqueServiceName(
                    prefix
                    );
            }

            return wfdServicesMgr.GenerateUniqueServiceName(
                DeviceAddress,
                prefix
                );
        }

        public WFDSvcWrapperHandle PublishService(
            string serviceName,
            bool autoAccept,
            bool preferGO,
            List<WiFiDirectServiceConfigurationMethod> configMethods,
            WiFiDirectServiceStatus status,
            uint customStatus,
            string serviceInfo,
            string deferredServiceInfo,
            List<String> prefixList
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.PublishService(
                    serviceName,
                    autoAccept,
                    preferGO,
                    configMethods,
                    status,
                    customStatus,
                    serviceInfo,
                    deferredServiceInfo,
                    prefixList
                    );
            }

            WiFiDirectTestLogger.Log("Publishing Service:");
            WFDSvcWrapperHandle handle = wfdServicesMgr.PublishService(
                serviceName,
                autoAccept,
                preferGO,
                configMethods,
                status,
                customStatus,
                serviceInfo,
                deferredServiceInfo,
                prefixList
                );

            WiFiDirectTestLogger.Log("Waiting to make sure service has successfully started...");
            wfdServicesMgr.GetAdvertiser(handle).WaitForServiceStarted();
            WiFiDirectTestLogger.Log("Service is ready!");

            return handle;
        }

        public ServiceAdvertiserInfo GetAdvertiserInfo(WFDSvcWrapperHandle handle)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetAdvertiserInfo(
                    handle
                    );
            }

            WiFiDirectTestLogger.Log("Extracting data from WiFiDirectServiceAdvertiser");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(handle);
            ServiceAdvertiserInfo advertiserInfo = new ServiceAdvertiserInfo(advertiser.Advertiser);
            advertiserInfo.ServiceAddress = DeviceAddress;

            return advertiserInfo;
        }

        public void UnpublishService(WFDSvcWrapperHandle handle, bool remove = false)
        {
            if (remoteInstance != null)
            {
                remoteInstance.UnpublishService(
                    handle,
                    remove
                    );
                return;
            }

            WiFiDirectTestLogger.Log(
                "Unpublishing{0} WiFiDirectServiceAdvertiser",
                (remove) ? " and removing" : ""
                );

            WiFiDirectTestLogger.Log("Getting Service to Unpublish: (Advertiser={0})", handle);

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(handle);

            wfdServicesMgr.UnPublishService(advertiser, remove);
        }

        public List<WFDSvcWrapperHandle> DiscoverServices(string serviceName, bool fExpectResults = true, string serviceInfo = "")
        {
            if (remoteInstance != null)
            {
                return remoteInstance.DiscoverServices(
                    serviceName,
                    fExpectResults,
                    serviceInfo
                    );
            }

            WiFiDirectTestLogger.Log("Discovering Service(s):");

            // Run async method and wait for results
            AsyncHandleListResult discoveryResults = wfdServicesMgr.DiscoverServicesAsync(serviceName, fExpectResults, serviceInfo).Result;

            // Can't throw in async, so we check for errors here
            if (!discoveryResults.Success)
            {
                if (discoveryResults.Error != null)
                {
                    throw discoveryResults.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in DiscoverServicesAsync");
                }
            }

            WiFiDirectTestLogger.Log("Done Discovering, got {0} services", discoveryResults.Handles.Count);

            return discoveryResults.Handles;
        }

        public List<WFDSvcWrapperHandle> WatcherDiscoverServices(string serviceName, List<ServiceAdvertiserInfo> expected, string serviceInfo = "")
        {
            if (remoteInstance != null)
            {
                return remoteInstance.WatcherDiscoverServices(
                    serviceName,
                    expected,
                    serviceInfo
                    );
            }

            WiFiDirectTestLogger.Log("Discovering Service(s) using watcher:");

            wfdServicesMgr.ResetWatcher();
            foreach (var service in expected)
            {
                wfdServicesMgr.AddExpectedServiceToWatcher(service);
            }

            return wfdServicesMgr.WatchServices(serviceName, serviceInfo);
        }

        public DiscoveredServiceInfo GetDiscoveredServiceInfo(WFDSvcWrapperHandle handle)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetDiscoveredServiceInfo(
                    handle
                    );
            }

            WiFiDirectTestLogger.Log("Extracting data from DeviceInformation");

            DiscoveredService discovered = wfdServicesMgr.GetDiscoveredService(handle);

            return new DiscoveredServiceInfo(discovered.DeviceInfo);
        }

        public void DeclineNextServiceConnectionRequest(WFDSvcWrapperHandle advertiserHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.DeclineNextServiceConnectionRequest(
                    advertiserHandle
                    );
                return;
            }

            WiFiDirectTestLogger.Log("Setting Advertiser to DECLINE next connection request");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(advertiserHandle);

            advertiser.DeclineNextSessionRequest = true;
        }

        public void ExpectNextServiceConnectionRequestFailure(WFDSvcWrapperHandle advertiserHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.ExpectNextServiceConnectionRequestFailure(
                    advertiserHandle
                    );
                return;
            }

            WiFiDirectTestLogger.Log("Setting Advertiser to expect next connection request to fail");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(advertiserHandle);

            advertiser.ExpectSessionRequestFailure = true;
        }

        public void SetServiceOptions(
            WFDSvcWrapperHandle discoveredHandle,
            bool preferGO,
            string sessionInfo = ""
            )
        {
            if (remoteInstance != null)
            {
                remoteInstance.SetServiceOptions(
                    discoveredHandle,
                    preferGO,
                    sessionInfo
                    );

                return;
            }

            WiFiDirectTestLogger.Log("Setting Service Options (Discovery Handle={0})", discoveredHandle);

            DiscoveredService discovered = wfdServicesMgr.GetDiscoveredService(discoveredHandle);

            discovered.SetServiceOptions(preferGO, sessionInfo);
        }

        public ProvisioningInfoWrapper GetServiceProvisioningInfo(
            WFDSvcWrapperHandle discoveredHandle,
            WiFiDirectServiceConfigurationMethod configMethod
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetServiceProvisioningInfo(
                    discoveredHandle,
                    configMethod
                    );
            }

            WiFiDirectTestLogger.Log("Getting Provisioning Info (Discovery Handle={0})", discoveredHandle);

            DiscoveredService discovered = wfdServicesMgr.GetDiscoveredService(discoveredHandle);

            // Run async method and wait for results
            AsyncProvisioningInfoResult provisioningResults = wfdServicesMgr.GetProvisioningInfoAsync(discovered, configMethod).Result;

            // Can't throw in async, so we check for errors here
            if (!provisioningResults.Success)
            {
                if (provisioningResults.Error != null)
                {
                    throw provisioningResults.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in GetProvisioningInfoAsync");
                }
            }

            WiFiDirectTestLogger.Log(
                "Done Getting Provisioning Info (Discovery Handle={0}, GroupFormationNeeded={1}, ConfigMethod={2})",
                discoveredHandle,
                provisioningResults.ProvisioningInfo.IsGroupFormationNeeded.ToString(),
                provisioningResults.ProvisioningInfo.SelectedConfigMethod.ToString()
                );

            return provisioningResults.ProvisioningInfo;
        }

        public WFDSvcWrapperHandle ConnectService(
            WFDSvcWrapperHandle discoveredHandle
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.ConnectService(
                    discoveredHandle
                    );
            }

            WiFiDirectTestLogger.Log("Connecting to service (Discovery Handle={0})", discoveredHandle);

            DiscoveredService discovered = wfdServicesMgr.GetDiscoveredService(discoveredHandle);

            // Run async method and wait for results
            AsyncHandleResult connectResults = wfdServicesMgr.ConnectAsync(discovered).Result;

            // Can't throw in async, so we check for errors here
            if (!connectResults.Success)
            {
                if (connectResults.Error != null)
                {
                    throw connectResults.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in ConnectAsync");
                }
            }

            return connectResults.Handle;
        }

        public string GetServiceDisplayedPin()
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetServiceDisplayedPin();
            }

            wfdServicesMgr.WaitForPinDisplay();
            return wfdServicesMgr.RetrievePin();
        }

        public void EnterServicesPin(string pin)
        {
            if (remoteInstance != null)
            {
                remoteInstance.EnterServicesPin(pin);
                return;
            }

            wfdServicesMgr.EnterPin(pin);
        }

        public ServiceSessionInfo GetServiceSessionInfo(WFDSvcWrapperHandle sessionHandle)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetServiceSessionInfo(
                    sessionHandle
                    );
            }

            WiFiDirectTestLogger.Log("Extracting data from WiFiDirectServiceSession");

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);
            ServiceSessionInfo sessionInfo = new ServiceSessionInfo(session.Session);
            sessionInfo.DeferredSessionInfo = session.DeferredSessionInfo;

            return sessionInfo;
        }

        public WFDSvcWrapperHandle GetAdvertiserLastSession(
            WFDSvcWrapperHandle advertiserHandle,
            bool hasPin
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetAdvertiserLastSession(
                    advertiserHandle,
                    hasPin
                    );
            }

            WiFiDirectTestLogger.Log("Getting the last connected session from the advertiser");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(advertiserHandle);

            WFDSvcWrapperHandle sessionHandle = wfdServicesMgr.GetLastAdvertiserSession(advertiser, hasPin && advertiser.Advertiser.AutoAcceptSession);

            return sessionHandle;
        }

        public void CheckAdvertiserLastSessionDeclined(WFDSvcWrapperHandle advertiserHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.CheckAdvertiserLastSessionDeclined(advertiserHandle);
                return;
            }

            WiFiDirectTestLogger.Log("Checking that advertiser declined last request");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(advertiserHandle);

            if (!advertiser.DidDeclineLastSessionRequest())
            {
                throw new Exception("Session was NOT declined by advertiser!");
            }
        }

        public void CheckAdvertiserLastSessionFailed(WFDSvcWrapperHandle advertiserHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.CheckAdvertiserLastSessionFailed(advertiserHandle);
                return;
            }

            WiFiDirectTestLogger.Log("Checking that advertiser failed to connect to last request");

            ServiceAdvertiser advertiser = wfdServicesMgr.GetAdvertiser(advertiserHandle);

            advertiser.WaitForSessionRequested(130000);

            if (!advertiser.DidFailLastSessionRequest())
            {
                throw new Exception("Advertiser did not fail to connect!");
            }
        }

        public void CheckServiceSessionDeferred(WFDSvcWrapperHandle sessionHandle, bool fExpected)
        {
            if (remoteInstance != null)
            {
                remoteInstance.CheckServiceSessionDeferred(
                    sessionHandle,
                    fExpected
                    );
                return;
            }

            WiFiDirectTestLogger.Log("Getting WiFiDirectServiceSession and verifying session was deferred if expected");

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            if (fExpected && !session.WasDeferred)
            {
                throw new Exception("Session was NOT deferred");
            }
            else if (!fExpected && session.WasDeferred)
            {
                throw new Exception("Session was deferred (NOT EXPECTED)");
            }
        }

        public int GetServiceSessionCount()
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetServiceSessionCount();
            }

            int count = wfdServicesMgr.GetSessionCount();
            WiFiDirectTestLogger.Log("Device has {0} sessions", count);

            return count;
        }

        public WFDSvcWrapperHandle AddServiceStreamSocket(WFDSvcWrapperHandle sessionHandle, UInt16 port)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.AddServiceStreamSocket(
                    sessionHandle,
                    port
                    );
            }

            WiFiDirectTestLogger.Log("Adding stream socket to session (Session Handle={0})", sessionHandle);

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            // Run async method and wait for results
            AsyncHandleResult socketResult = wfdServicesMgr.AddStreamSocketAsync(session, port).Result;

            // Can't throw in async, so we check for errors here
            if (!socketResult.Success)
            {
                if (socketResult.Error != null)
                {
                    throw socketResult.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in AddStreamSocketAsync");
                }
            }

            return socketResult.Handle;
        }

        public WFDSvcWrapperHandle AddServiceDatagramSocket(WFDSvcWrapperHandle sessionHandle, UInt16 port)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.AddServiceDatagramSocket(
                    sessionHandle,
                    port
                    );
            }

            WiFiDirectTestLogger.Log("Adding datagram socket to session (Session Handle={0})", sessionHandle);

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            // Run async method and wait for results
            AsyncHandleResult socketResult = wfdServicesMgr.AddDatagramSocketAsync(session, port).Result;

            // Can't throw in async, so we check for errors here
            if (!socketResult.Success)
            {
                if (socketResult.Error != null)
                {
                    throw socketResult.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in AddDatagramSocketAsync");
                }
            }

            return socketResult.Handle;
        }

        public WFDSvcWrapperHandle GetServiceRemoteSocketAdded(WFDSvcWrapperHandle sessionHandle)
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetServiceRemoteSocketAdded(
                    sessionHandle
                    );
            }

            WiFiDirectTestLogger.Log("Getting the last connected socket for receiving side");

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            WFDSvcWrapperHandle socketHandle = wfdServicesMgr.GetLastRemoteSocketAdded(session);

            return socketHandle;
        }

        public void SendServiceSocketMessage(
            WFDSvcWrapperHandle sessionHandle,
            WFDSvcWrapperHandle socketHandle,
            string message
            )
        {
            if (remoteInstance != null)
            {
                remoteInstance.SendServiceSocketMessage(
                    sessionHandle,
                    socketHandle,
                    message
                    );
                return;
            }

            WiFiDirectTestLogger.Log(
                "Sending message over socket (Session Handle={0} Socket Handle={1})",
                sessionHandle,
                socketHandle
                );

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);
            SocketWrapper socket = session.GetSocket(socketHandle);

            // Run async method and wait for results
            AsyncResult result = wfdServicesMgr.SendMessageAsync(socket, message).Result;

            // Can't throw in async, so we check for errors here
            if (!result.Success)
            {
                if (result.Error != null)
                {
                    throw result.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in SendMessageAsync");
                }
            }
        }

        public string ReceiveServiceSocketMessage(
            WFDSvcWrapperHandle sessionHandle,
            WFDSvcWrapperHandle socketHandle
            )
        {
            if (remoteInstance != null)
            {
                return remoteInstance.ReceiveServiceSocketMessage(
                    sessionHandle,
                    socketHandle
                    );
            }

            WiFiDirectTestLogger.Log(
                "Expecting to receive message over socket (Session Handle={0} Socket Handle={1})",
                sessionHandle,
                socketHandle
                );

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);
            SocketWrapper socket = session.GetSocket(socketHandle);

            // Run async method and wait for results
            AsyncStringResult result = wfdServicesMgr.ReceiveMessageAsync(socket).Result;

            // Can't throw in async, so we check for errors here
            if (!result.Success)
            {
                if (result.Error != null)
                {
                    throw result.Error;
                }
                else
                {
                    throw new Exception("Unknown failure in ReceiveMessageAsync");
                }
            }

            return result.String;
        }

        public void DisconnectServiceSession(WFDSvcWrapperHandle sessionHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.DisconnectServiceSession(
                    sessionHandle
                    );
                return;
            }

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            WiFiDirectTestLogger.Log("Disconnecting session (Session={0} ServiceName={1})", sessionHandle, session.Session.ServiceName);

            session.CloseSession();

            WiFiDirectTestLogger.Log("Disconnect session done (Session={0})", sessionHandle);
        }

        public void WaitForDisconnectServiceSession(WFDSvcWrapperHandle sessionHandle)
        {
            if (remoteInstance != null)
            {
                remoteInstance.WaitForDisconnectServiceSession(
                    sessionHandle
                    );
                return;
            }

            ServiceSession session = wfdServicesMgr.GetSession(sessionHandle);

            WiFiDirectTestLogger.Log("Waiting for Session Disconnect(Session={0} ServiceName={1})", sessionHandle, session.Session.ServiceName);

            session.WaitForSessionClosed(32000);

            WiFiDirectTestLogger.Log("Session Disconnected (Session={0})", sessionHandle);
        }

        public void CheckServicesManagerErrors()
        {
            if (remoteInstance != null)
            {
                remoteInstance.CheckServicesManagerErrors();
                return;
            }

            if (wfdServicesMgr.HasError())
            {
                throw wfdServicesMgr.GetError();
            }
        }
        #endregion

        #region DAF
        public void TryRemoveDafAssociation(DOT11_MAC_ADDRESS address)
        {
            if (remoteInstance != null)
            {
                remoteInstance.TryRemoveDafAssociation(
                    address
                    );
                return;
            }

            try
            {
                string aepId = String.Format(CultureInfo.InvariantCulture, "WiFiDirect#{0}", address.ToString());

                WiFiDirectTestLogger.Log("Removing DAF pairing to {0} if it exists (AEP ID: {1})", address, aepId);

                DafWrapper.RemoveAssociation(aepId);
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("RemoveAssociation threw an exception, ignoring: {0}", ex.Message);
            }
        }
        #endregion

        public void WaitForAllWFDSessionClose(bool fWaitForGOStop = false)
        {
            if (remoteInstance != null)
            {
                remoteInstance.WaitForAllWFDSessionClose(fWaitForGOStop);
                return;
            }

            WiFiDirectTestLogger.Log("Wait for all WFD Session to close (Wait for GO stop {0})", fWaitForGOStop);
            bool sessionClosed = true;

            for (int i = 0; i < 30; ++i)
            {
                sessionClosed = true;
                WfdGlobalSessionState globalState = wifiDirect.QueryGlobalSessionState();

                foreach (var session in globalState.Sessions)
                {
                    // If this is a client, then the session is still connected
                    // If this is a GO, check the associated devices count
                    // If we need to wait for the GO to stop, then anything here means we are not done
                    WiFiDirectTestLogger.Log("FOUND WFD Session (interface: {0}, associated devices: {1})", session.InterfaceAddress, session.AssociatedDevices.Length);
                    if (session.Role == WFD_ROLE_TYPE.WFD_ROLE_TYPE_CLIENT ||
                        session.AssociatedDevices.Length > 0 ||
                        fWaitForGOStop)
                    {
                        sessionClosed = false;
                    }
                }

                if (sessionClosed)
                {
                    WiFiDirectTestLogger.Log("WFD Session is CLOSED");
                    break;
                }

                WiFiDirectTestLogger.Log("Waiting for session to close (iteration {0})", i);
                Task.Delay(1000).Wait();
            }

            if (!sessionClosed)
            {
                throw new Exception("WFD Session still OPEN, not expected!");
            }
        }

        public void SetTestIe()
        {
            if (remoteInstance != null)
            {
                remoteInstance.SetTestIe();
                return;
            }

            WiFiDirectTestLogger.Log("Enabling Test IE containing GUID {0}", ieManager.Guid);
            wifiDirect.WfdSetApplicationIe(ieManager.IeBytes);
        }

        public void StartStreamSocketListener(string port)
        {
            if (remoteInstance != null)
            {
                remoteInstance.StartStreamSocketListener(port);
                return;
            }

            lock (sessionHandleLock)
            {
                if (currentWfdSessionHandle == null)
                {
                    throw new InvalidOperationException("There is no session connected.");
                }

                string localAddress, remoteAddress;
                wifiDirect.WfdGetIpv4AddressesForSession(currentWfdSessionHandle, out localAddress, out remoteAddress);

                dataPathTester.StartConnectionListener(localAddress, remoteAddress, port);
            }
        }

        public void WaitForSocketConnectionReceived()
        {
            if (remoteInstance != null)
            {
                remoteInstance.WaitForSocketConnectionReceived();
                return;
            }

            dataPathTester.WaitForConnectionReceivedCallback();
        }

        public void ConnectStreamSocketAndSendGuid(string port)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("ConnectStreamSocket not supported on a remote instance.");
            }

            lock (sessionHandleLock)
            {
                if (currentWfdSessionHandle == null)
                {
                    throw new InvalidOperationException("There is no session connected.");
                }

                string localAddress, remoteAddress;
                wifiDirect.WfdGetIpv4AddressesForSession(currentWfdSessionHandle, out localAddress, out remoteAddress);

                dataPathTester.ConnectAndSendGuid(localAddress, remoteAddress, port);
            }
        }

        public void ResetDataPathTester()
        {
            if (remoteInstance != null)
            {
                remoteInstance.ResetDataPathTester();
                return;
            }

            dataPathTester.Reset();
        }

        public string GetDefaultGoProfile()
        {
            if (remoteInstance != null)
            {
                return remoteInstance.GetDefaultGoProfile();
            }

            if (defaultGoProfile != null)
            {
                return defaultGoProfile;
            }

            defaultGoProfile = wifiDirect.WfdGetDefaultGoProfile();
            return defaultGoProfile;
        }

        public void ParseGoProfile(string goProfile, out string ssid, out string passphrase)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("ParseGoProfile not supported on remote instance");
            }

            wifiDirect.WfdParseGoProfile(goProfile, out ssid, out passphrase);
        }

        public void PrepareForLegacyConnectionAttempt(DOT11_MAC_ADDRESS peerDeviceAddress)
        {
            if (remoteInstance != null)
            {
                remoteInstance.PrepareForLegacyConnectionAttempt(peerDeviceAddress);
                return;
            }

            string localGoProfile = GetDefaultGoProfile();
            wifiDirect.WfdOpenLegacySession(peerDeviceAddress, localGoProfile);
            mustCloseLegacySessionOnCleanup = true;
            legacySessionPeer = peerDeviceAddress;
        }

        public void LegacyConnectToGo(string profileXML)
        {
            if (remoteInstance != null)
            {
                throw new NotSupportedException("(Legacy)Connect not supported on remote instance");
            }

            List<WlanInterface> wlanInterfaceList = wlan.EnumWlanInterfaces();
            if (wlanInterfaceList.Count == 0)
            {
                throw new Exception("No available inteface for connection");
            }

            WlanInterface wlanInterface = wlanInterfaceList[0];

            wlan.CreateProfile(wlanInterface.Id, profileXML);
            List<WlanProfileInfo> profiles = wlan.GetProfileList(wlanInterface.Id);
            if (profiles.Count == 0)
            {
                throw new Exception("No profiles returned for interface.");
            }

            TimeSpan timeout = new TimeSpan(0, 0, 0, 10, 0);
            wlan.ProfileConnect(wlanInterface.Id, profiles[0].ProfileName, timeout);
        }

        public void ConnectToWPA2AP(string wPA2SSID, string wPA2Password)
        {
            SSIDConfig ssidConfig = new SSIDConfig();
            ssidConfig.SSIDs.Add(new SSIDClass(wPA2SSID, String.Empty));
            MSMElement msmElement = new MSMElement();
            msmElement.Security.SharedKey.KeyType = KeyType.PassPhrase;
            msmElement.Security.SharedKey.KeyMaterial = wPA2Password;

            WlanProfile wlanProfile = new WlanProfile(wPA2SSID, ssidConfig, ConnectionType.ESS, ConnectionMode.Manual, false, msmElement);

            // Connect to the AP using this profile
            LegacyConnectToGo(wlanProfile.ToXml());
        }


        // Handler for WFD Peer Disconnection Events
        void OnPeerStateDisconnected(object sender, WfdPeerStateChangedEventArgs args)
        {
            try
            {
                lock (sessionHandleLock)
                {
                    if (currentWfdSessionHandle == null ||
                        (peerDeviceAddress != args.DeviceDescriptor.PeerMacAddress) ||
                        sessionInterface != args.InterfaceId)
                    {
                        return;
                    }

                    WiFiDirectTestLogger.Log("Received Disconnect Notification for Device {0}", args.DeviceDescriptor.PeerMacAddress);

                    currentWfdSessionHandle.Dispose();
                    currentWfdSessionHandle = null;
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught Exception while handling peer state disconnected notification:  {0}", e);
            }
        }

        void OnGroupRequestReceived(object sender, WfdGroupRequestNotificationData data)
        {
            try
            {
                lock (sessionHandleLock)
                {
                    if (!acceptGroupRequestFlag ||
                        (groupRequestExpectedPeer != data.DeviceAddress))
                    {
                        return;
                    }

                    if (currentWfdSessionHandle != null)
                    {
                        WiFiDirectTestLogger.Log("Discarding incoming group request from {0} because there is already a connected WFD session.", data.DeviceAddress);
                        return;
                    }

                    // TODO: Tighten up this check.  Config Methods aren't currently marshalling properly inside the group request notification data union.
                    //if (data.RequestType == WFD_GROUP_REQUEST_TYPE.wfd_group_request_type_group_negotiation ||
                    //    data.RequestType == WFD_GROUP_REQUEST_TYPE.wfd_group_request_type_provision_discovery)
                    //{
                    //    if (data.RequestedConfigMethod != acceptConfigMethod)
                    //    {
                    //        WiFiDirectTestLogger.Log(
                    //            "Discarding incoming group request from {0} because expected and requested config methods did not match. Expected {1} (0x{2:X}), Requested {3} (0x{4:X})", 
                    //            data.DeviceAddress,
                    //            ConfigMethodToString(acceptConfigMethod),
                    //            acceptConfigMethod,
                    //            ConfigMethodToString(data.RequestedConfigMethod),
                    //            data.RequestedConfigMethod
                    //            );
                    //        return;
                    //    }
                    //}
                    if (data.RequestType == WFD_GROUP_REQUEST_TYPE.wfd_group_request_type_invitation)
                    {
                        if ( (data.SupportedConfigMethods & acceptConfigMethod) == 0)
                        {
                            WiFiDirectTestLogger.Log(
                                "Discarding incoming group request from {0} because device does not support expected config method. Expected {1} (0x{2:X}), Supported {3} (0x{4:X})",
                                data.DeviceAddress,
                                ConfigMethodToString(acceptConfigMethod),
                                acceptConfigMethod,
                                ConfigMethodToString(data.SupportedConfigMethods),
                                data.SupportedConfigMethods
                                );
                            return;
                        }
                    }

                    WiFiDirectTestLogger.Log(
                        "Accepting incoming group request from {0}.  WPS Config Method: {1}, WPS PIN: {2}", 
                        data.DeviceAddress, 
                        ConfigMethodToString(acceptConfigMethod), 
                        acceptConfigMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON ? PassKey : "None");

                    currentWfdSessionHandle = wifiDirect.AcceptGroupRequestAndOpenSession(
                        data.RequestHandle,
                        acceptGoIntent,
                        acceptConfigMethod,
                        acceptConfigMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON ? PassKey : null,
                        out sessionInterface,
                        out lastUsedProfile
                        );
                    peerDeviceAddress = data.DeviceAddress;
                    lastPairedDevice = data.DeviceAddress;
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught Exception while handling group request notification:  {0}", e);
            }
        }

        void OnConnectionRequestReceived(object sender, WfdConnectionRequestEventArgs data)
        {
            try
            {
                lock(sessionHandleLock)
                {
                    if (!acceptConnectRequestFlag ||
                        (lastPairedDevice != data.PeerDeviceAddress))
                    {
                        return;
                    }

                    if (currentWfdSessionHandle != null)
                    {
                        WiFiDirectTestLogger.Log("Discarding incoming connection request from {0} because there is already a connected WFD session.", data.PeerDeviceAddress);
                        return;
                    }

                    WiFiDirectTestLogger.Log("Accepting incoming connection request from {0}.", data.PeerDeviceAddress);
                    currentWfdSessionHandle = wifiDirect.WfdAcceptConnectionRequest(data.RequestHandle, lastUsedProfile, out sessionInterface);
                }
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught Exception while handling connection request notification:  {0}", e);
            }
        }

        private string ConfigMethodToString(DOT11_WPS_CONFIG_METHOD configMethod)
        {
            string configMethodString = "<Unrecognized Config Method>";

            switch(configMethod)
            {
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY:
                    configMethodString = "Display";
                    break;
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON:
                    configMethodString = "PushButton";
                    break;
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD:
                    configMethodString = "Keypad";
                    break;
            }

            return configMethodString;
        }

        // The WifiDirect object's implementation uses the logging framework in Microsoft.Test.Networking.Tracing.
        // This event handler directs that output to the TAEF log.
        private void TraceProvider_TraceEvent(Microsoft.Test.Networking.Tracing.ProviderTraceLevel level, string traceLine)
        {
            WiFiDirectTestLogger.LogFromWrapper(traceLine);
        }
    }
}
