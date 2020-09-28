///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.RemoteControl;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    public class WFDSvcWrapperHandle
    {
        private static int next = 1;

        public WFDSvcWrapperHandle()
        {
            this.Value = next++;
        }

        public int Value
        {
            get;
            // set needs to be public for serialization to work
            set;
        }

        public override string ToString()
        {
            return Value.ToString(CultureInfo.InvariantCulture);
        }

        public override int GetHashCode()
        {
            // Provide a hashcode that minimizes collisions and spreads out over many buckets
            // This will allow efficient Dictionary lookups
            // Multiply value (which just increments linearly) by a large prime
            unchecked
            {
                return Value * 486187739;
            }
        }

        /// <summary>
        /// When using WFDSvcWrapperHandle in dictionary, need custom comparer
        /// </summary>
        public class EqualityComparer : IEqualityComparer<WFDSvcWrapperHandle>
        {
            public bool Equals(WFDSvcWrapperHandle lhs, WFDSvcWrapperHandle rhs)
            {
                if (lhs == null && rhs == null)
                {
                    return true;
                }
                else if (lhs == null || rhs == null)
                {
                    return false;
                }
                return lhs.Value == rhs.Value;
            }

            public int GetHashCode(WFDSvcWrapperHandle h)
            {
                return (h == null) ? 0 : h.GetHashCode();
            }
        }
    }

    /// <summary>
    /// Result of asynchronous operation
    /// 
    /// Includes any error information from execution
    /// </summary>
    internal class AsyncResult
    {
        internal AsyncResult(
            bool success,
            Exception error = null
            )
        {
            Success = success;
            Error = error;
        }

        public bool Success { get; private set; }
        public Exception Error { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a handle
    /// </summary>
    internal class AsyncHandleResult : AsyncResult
    {
        internal AsyncHandleResult(
            WFDSvcWrapperHandle handle,
            bool success,
            Exception error = null
            ) : base(success, error)
        {
            Handle = handle;
        }

        public WFDSvcWrapperHandle Handle { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a list of handles
    /// </summary>
    internal class AsyncHandleListResult : AsyncResult
    {
        internal AsyncHandleListResult(
            List<WFDSvcWrapperHandle> handles,
            bool success,
            Exception error = null
            )
            : base(success, error)
        {
            Handles = handles;
        }

        public List<WFDSvcWrapperHandle> Handles { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a WiFiDirectServiceProvisioningInfo
    /// </summary>
    internal class AsyncProvisioningInfoResult : AsyncResult
    {
        internal AsyncProvisioningInfoResult(
            ProvisioningInfoWrapper info,
            bool success,
            Exception error = null
            )
            : base(success, error)
        {
            ProvisioningInfo = info;
        }

        public ProvisioningInfoWrapper ProvisioningInfo { get; private set; }
    }

    internal class AsyncStringResult : AsyncResult
    {
        internal AsyncStringResult(
            string str,
            bool success,
            Exception error = null
            ) : base(success, error)
        {
            this.String = str;
        }

        public string String { get; private set; }
    }

    /// <summary>
    /// Data from Windows.Devices.WiFiDirect.Services.WiFiDirectServiceAdvertiser
    /// which can easily be serialized to pass over remote control
    /// </summary>
    public class ServiceAdvertiserInfo
    {
        public ServiceAdvertiserInfo()
        {
            PreferredConfigMethods = new List<WiFiDirectServiceConfigurationMethod>();
        }

        /// <summary>
        /// Build the info from the actual object
        /// </summary>
        /// <param name="advertiser"></param>
        internal ServiceAdvertiserInfo(WiFiDirectServiceAdvertiser advertiser)
        {
            PreferGO = advertiser.PreferGroupOwnerMode;
            PreferredConfigMethods = new List<WiFiDirectServiceConfigurationMethod>(advertiser.PreferredConfigurationMethods);
            ServiceName = advertiser.ServiceName;
            ServiceStatus = advertiser.ServiceStatus;

            if (advertiser.ServiceInfo != null && advertiser.ServiceInfo.Length > 0)
            {
                using (DataReader serviceInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(advertiser.ServiceInfo))
                {
                    ServiceInfo = serviceInfoDataReader.ReadString(advertiser.ServiceInfo.Length);
                }
            }
            else
            {
                ServiceInfo = "";
            }

            if (advertiser.DeferredSessionInfo != null && advertiser.DeferredSessionInfo.Length > 0)
            {
                using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(advertiser.DeferredSessionInfo))
                {
                    DeferredSessionInfo = sessionInfoDataReader.ReadString(advertiser.DeferredSessionInfo.Length);
                }
            }
            else
            {
                DeferredSessionInfo = "";
            }
        }

        public bool PreferGO { get; set; }
        public List<WiFiDirectServiceConfigurationMethod> PreferredConfigMethods { get; set; }
        public string ServiceName { get; set; }
        public WiFiDirectServiceStatus ServiceStatus { get; set; }
        // Note: this is set by the controller, don't have the MAC in the WiFiDirectServiceAdvertiser
        public DOT11_MAC_ADDRESS ServiceAddress { get; set; }
        public string ServiceInfo { get; set; }
        public string DeferredSessionInfo { get; set; }
    }

    /// <summary>
    /// Wraps Windows.Devices.WiFiDirect.Services.WiFiDirectServiceAdvertiser
    /// </summary>
    internal class ServiceAdvertiser : IDisposable
    {
        private WiFiDirectServiceAdvertiser advertiser;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;
        
        private bool declineNextRequest = false;
        private bool expectSessionRequestFail = false;

        private CancellationTokenSource connectCts;

        private AutoResetEvent serviceStartedEvent = new AutoResetEvent(false);
        private AutoResetEvent autoAcceptConnectedEvent = new AutoResetEvent(false);
        private AutoResetEvent sessionRequestedEvent = new AutoResetEvent(false);

        // Keep track of sessions connected via events so we can directly access them
        private WFDSvcWrapperHandle lastConnectedSession = null;
        private bool declinedLastRequest = false;
        private bool failedLastRequest = false;

        internal ServiceAdvertiser(
            WiFiDirectServiceAdvertiser advertiser,
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.advertiser = advertiser;
            this.sessionManager = sessionManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
            
            this.advertiser.AdvertisementStatusChanged += OnAdvertisementStatusChanged;
            this.advertiser.AutoAcceptSessionConnected += OnAutoAcceptSessionConnected;
            this.advertiser.SessionRequested += OnSessionRequested;

            WiFiDirectTestLogger.Log("Created ServiceAdvertiser wrapper (Advertiser={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public WiFiDirectServiceAdvertiser Advertiser
        {
            get { return advertiser; }
        }

        public WFDSvcWrapperHandle LastConnectedSession
        {
            get { return lastConnectedSession; }
        }

        public bool DeclineNextSessionRequest
        {
            set { declineNextRequest = value; }
        }

        public bool ExpectSessionRequestFailure
        {
            set { expectSessionRequestFail = value; }
        }

        public bool DidDeclineLastSessionRequest()
        {
            return declinedLastRequest;
        }

        public bool DidFailLastSessionRequest()
        {
            return failedLastRequest;
        }
        #endregion

        #region Events
        private async void OnSessionRequested(WiFiDirectServiceAdvertiser advertiser, WiFiDirectServiceSessionRequestedEventArgs args)
        {
            try
            {
                ThrowIfDisposed();
                sessionRequestedEvent.Reset();

                WiFiDirectTestLogger.Log("Session Requested (Advertiser={0})", handle);

                WiFiDirectServiceSession session = null;

                string sessionInfo = "";
                if (args.GetSessionRequest().SessionInfo != null && args.GetSessionRequest().SessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(args.GetSessionRequest().SessionInfo))
                    {
                        sessionInfo = sessionInfoDataReader.ReadString(args.GetSessionRequest().SessionInfo.Length);
                        WiFiDirectTestLogger.Log("Received Session Info: {1} (Discovery={0})", handle, WiFiDirectTestUtilities.GetTruncatedString(sessionInfo));
                    }
                }

                if (declineNextRequest)
                {
                    declineNextRequest = false;
                    WiFiDirectTestLogger.Log("Declining incoming request (Advertiser={0})", handle);

                    lastConnectedSession = null;
                    declinedLastRequest = true;
                    args.GetSessionRequest().Dispose();
                }
                else
                {
                    WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync... (Advertiser={0})", handle);

                    // Need to allow cancellation in case seeker fails and test is trying to continue to next test case
                    connectCts = new CancellationTokenSource();

                    if (args.GetSessionRequest().ProvisioningInfo.IsGroupFormationNeeded &&
                        args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod != WiFiDirectServiceConfigurationMethod.Default)
                    {
                        if (args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                        {
                            // Generate a PIN, set event
                            this.pinManager.GeneratePin();
                        }
                        else
                        {
                            // Wait on PIN entry event, get displayed PIN
                            this.pinManager.WaitForPinEntry();
                        }

                        string pin = this.pinManager.RetrievePin();

                        WiFiDirectTestLogger.Log(
                            "Provisioning is required, config method={1}, using pin={2} (Advertiser={0})",
                            handle,
                            args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod,
                            pin
                            );

                        session = await advertiser.ConnectAsync(args.GetSessionRequest().DeviceInformation, pin).AsTask(connectCts.Token);
                    }
                    else
                    {
                        session = await advertiser.ConnectAsync(args.GetSessionRequest().DeviceInformation).AsTask(connectCts.Token);
                    }
                    WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync DONE (Advertiser={0})", handle);

                    connectCts = null;

                    // Handle case where object is disposed during connect
                    ThrowIfDisposed();

                    ServiceSession sessionWrapper = new ServiceSession(session, sessionManager, errorHandler, deferredSessionInfo: sessionInfo);
                    sessionManager.AddSession(sessionWrapper);

                    WiFiDirectTestLogger.Log("Added session to list (Advertiser={0}, Session={1})", handle, sessionWrapper.Handle);

                    lastConnectedSession = sessionWrapper.Handle;
                    declinedLastRequest = false;
                }
            }
            catch (TaskCanceledException)
            {
                WiFiDirectTestLogger.Log("Connect was canceled by user, failure should have already happened so swallowing this error (Advertiser={0})", handle);
            }
            catch (Exception ex)
            {
                failedLastRequest = true;
                if (!expectSessionRequestFail)
                {
                    WiFiDirectTestLogger.Error("Unexpected exception in OnSessionRequested: {1} (Advertiser={0})", handle, ex.Message);
                    errorHandler.SetError(ex);
                }
                else
                {
                    WiFiDirectTestLogger.Log("Got expected session request failure (Advertiser={0})", handle);
                }
            }
            finally
            {
                if (!disposed)
                {
                    sessionRequestedEvent.Set();
                }
            }
        }

        private void OnAutoAcceptSessionConnected(WiFiDirectServiceAdvertiser sender, WiFiDirectServiceAutoAcceptSessionConnectedEventArgs args)
        {
            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log("Auto-Accept Session Connected (Advertiser={0})", handle);

                string sessionInfo = "";
                if (args.SessionInfo != null && args.SessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(args.SessionInfo))
                    {
                        sessionInfo = sessionInfoDataReader.ReadString(args.SessionInfo.Length);
                        WiFiDirectTestLogger.Log("Received Session Info: {1} (Discovery={0})", handle, WiFiDirectTestUtilities.GetTruncatedString(sessionInfo));
                    }
                }

                ServiceSession sessionWrapper = new ServiceSession(args.Session, sessionManager, errorHandler, deferredSessionInfo: sessionInfo);
                sessionManager.AddSession(sessionWrapper);

                WiFiDirectTestLogger.Log("Added session to list (Advertiser={0}, Session={1})", handle, sessionWrapper.Handle);

                lastConnectedSession = sessionWrapper.Handle;
                autoAcceptConnectedEvent.Set();
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnAutoAcceptSessionConnected: {1} (Advertiser={0})", handle, ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnAdvertisementStatusChanged(WiFiDirectServiceAdvertiser sender, object o)
        {
            try
            {
                ThrowIfDisposed();

                WiFiDirectServiceAdvertisementStatus status = advertiser.AdvertisementStatus;

                WiFiDirectTestLogger.Log(
                    "Advertisement Status Changed to {1} (Advertiser={0})",
                    handle,
                    status.ToString()
                    );

                if (status == WiFiDirectServiceAdvertisementStatus.Started)
                {
                    serviceStartedEvent.Set();
                }
                else if (status == WiFiDirectServiceAdvertisementStatus.Aborted)
                {
                    WiFiDirectTestLogger.Error(
                        "Advertisement aborted, error: {1} (Advertiser={0})",
                        handle,
                        advertiser.ServiceError
                        );
                    errorHandler.SetError(new Exception("Advertiser aborted unexpectedly"));
                }

            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Unexpected exception in OnAdvertisementStatusChanged: {1} (Advertiser={0})", handle, ex.Message);
            }
        }
        #endregion

        #region Synchronization
        public void WaitForServiceStarted(int msTimeout = 1000)
        {
            ThrowIfDisposed();
            if (!serviceStartedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for service to start");
            }
        }

        public void WaitForAutoAcceptSessionConnected(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!autoAcceptConnectedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for auto accept session to connect");
            }
        }

        public void WaitForSessionRequested(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!sessionRequestedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for session request");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~ServiceAdvertiser()
        {
            Dispose(false);
        }
        
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }
            
            if (disposing)
            {
                // Stop advertising
                // If for some reason the advertiser is stopping between the time that we check
                // the AdvertisementStatus and when we call Stop, this will throw
                try
                {
                    if (advertiser.AdvertisementStatus == WiFiDirectServiceAdvertisementStatus.Started)
                    {
                        advertiser.Stop();
                    }
                }
                catch (Exception)
                {
                    // Stop can throw if it is already stopped or stopping, ignore
                }

                // Remove event handlers
                advertiser.AdvertisementStatusChanged -= OnAdvertisementStatusChanged;
                advertiser.AutoAcceptSessionConnected -= OnAutoAcceptSessionConnected;
                advertiser.SessionRequested -= OnSessionRequested;
                
                // Cancel ongoing connect, if it is not complete
                if (connectCts != null)
                {
                    WiFiDirectTestLogger.Log("Attempt to cancel connection attempt (Advertiser={0})", handle);
                    connectCts.Cancel();
                }

                serviceStartedEvent.Dispose();
                autoAcceptConnectedEvent.Dispose();
                sessionRequestedEvent.Dispose();
            }
            
            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("ServiceAdvertiser");
            }
        }
        #endregion
    }

    class WiFiDirectServicesWatcher : IDisposable
    {
        private DeviceWatcher watcher = null;

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesDiscoveryManager discoveryManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;

        private IList<ServiceAdvertiserInfo> expectations = new List<ServiceAdvertiserInfo>();
        private IDictionary<string, DiscoveredService> discovered = new Dictionary<string, DiscoveredService>();

        private AutoResetEvent stoppedEvent = new AutoResetEvent(false);
        private AutoResetEvent enumCompletedEvent = new AutoResetEvent(false);
        private AutoResetEvent foundAllEvent = new AutoResetEvent(false);

        public WiFiDirectServicesWatcher(
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesDiscoveryManager discoveryManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.sessionManager = sessionManager;
            this.discoveryManager = discoveryManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
        }

        public void AddExpectedService(ServiceAdvertiserInfo service)
        {
            expectations.Add(service);
        }

        public bool HasExpectations()
        {
            return expectations.Count > 0;
        }

        public void Start(string serviceName, string serviceInfo = "")
        {
            ThrowIfDisposed();

            ResetWatcher();
            foundAllEvent.Reset();
            enumCompletedEvent.Reset();
            stoppedEvent.Reset();

            discoveryManager.ClearDiscoveries();

            WiFiDirectTestLogger.Log("Start Services Watcher... (ServiceName={0}, ServiceInfo={1})", serviceName, serviceInfo);

            string serviceSelector = "";
            if (serviceInfo == "")
            {
                serviceSelector = WiFiDirectService.GetSelector(serviceName);
            }
            else
            {
                using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    serviceInfoDataWriter.WriteString(serviceInfo);
                    serviceSelector = WiFiDirectService.GetSelector(serviceName, serviceInfoDataWriter.DetachBuffer());
                }
            }

            WiFiDirectTestLogger.Log("DeviceInformation.CreateWatcher... Using Selector: {0}", serviceSelector);

            List<string> additionalProperties = new List<string>();
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceAddress");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceName");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceInformation");
            additionalProperties.Add("System.Devices.WiFiDirectServices.AdvertisementId");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceConfigMethods");

            watcher = DeviceInformation.CreateWatcher(serviceSelector, additionalProperties);

            watcher.Added += OnAdded;
            watcher.EnumerationCompleted += OnEnumerationComplete;
            watcher.Removed += OnRemoved;
            watcher.Stopped += OnStopped;
            watcher.Updated += OnUpdated;

            watcher.Start();
        }

        public void Stop()
        {
            if (watcher != null)
            {
                watcher.Stop();
            }
        }

        #region Synchronization
        public void WaitForFindAll(int msTimeout = 15000)
        {
            ThrowIfDisposed();
            WaitHandle[] handles = new WaitHandle[] { foundAllEvent, stoppedEvent };
            int waitResult = WaitHandle.WaitAny(handles, msTimeout);
            if (waitResult == WaitHandle.WaitTimeout)
            {
                throw new Exception("Timed out waiting for watcher to find service");
            }
            else if (waitResult == 1)
            {
                throw new Exception("Watcher stopped before finding all services");
            }
        }
        #endregion

        private void CheckExpectation(DiscoveredServiceInfo discoveredServiceInfo)
        {
            if (expectations.Count > 0)
            {
                for (int i = 0; i < expectations.Count; ++i)
                {
                    if (expectations[i].ServiceName == discoveredServiceInfo.ServiceName &&
                        expectations[i].ServiceAddress == discoveredServiceInfo.ServiceAddress)
                    {
                        if (expectations[i].ServiceInfo != discoveredServiceInfo.ServiceInfo)
                        {
                            WiFiDirectTestLogger.Log(
                                "Expecting Service Info:\n\t{0}\nReceived:\n\t{1}",
                                WiFiDirectTestUtilities.GetTruncatedString(expectations[i].ServiceInfo, 32),
                                WiFiDirectTestUtilities.GetTruncatedString(discoveredServiceInfo.ServiceInfo, 32)
                                );
                            // Allow multiple services with same name/different service info
                            // Skip if service info match fails, will fail if no service info found
                            continue;
                        }

                        WiFiDirectTestLogger.Log("Found Expected Service!");
                        expectations.RemoveAt(i);
                    }
                }

                if (expectations.Count == 0)
                {
                    WiFiDirectTestLogger.Log("Found All Expected Services!");
                    foundAllEvent.Set();
                }
            }
        }

        #region events
        private void OnAdded(DeviceWatcher w, DeviceInformation devInfo)
        {
            try
            {
                WiFiDirectTestLogger.Log("Service Added: {0}", devInfo.Id);

                DiscoveredService discoveredService = new DiscoveredService(devInfo, sessionManager, errorHandler, pinManager);
                discovered.Add(devInfo.Id, discoveredService);
                discoveryManager.AddDiscovery(discoveredService);

                if (expectations.Count > 0)
                {
                    DiscoveredServiceInfo discoveredServiceInfo = new DiscoveredServiceInfo(devInfo);
                    CheckExpectation(discoveredServiceInfo);
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnAdded: {0}", ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnUpdated(DeviceWatcher w, DeviceInformationUpdate update)
        {
            try
            {
                WiFiDirectTestLogger.Log("Service Updated: {0}", update.Id);

                // Find the existing discovery and update it
                if (discovered.ContainsKey(update.Id))
                {
                    (discovered[update.Id]).DeviceInfo.Update(update);

                    if (expectations.Count > 0)
                    {
                        DiscoveredServiceInfo discoveredServiceInfo = new DiscoveredServiceInfo((discovered[update.Id]).DeviceInfo);
                        CheckExpectation(discoveredServiceInfo);
                    }
                }
                else
                {
                    WiFiDirectTestLogger.Log("Discovered service not found in our map!!!");
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnUpdated: {0}", ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnRemoved(DeviceWatcher w, DeviceInformationUpdate update)
        {
            WiFiDirectTestLogger.Log("Service Removed: {0}", update.Id);

            DiscoveredService discoveredService = discovered[update.Id];
            discoveryManager.RemoveDiscovery(discoveredService.Handle);
            discovered.Remove(update.Id);
        }

        private void OnEnumerationComplete(DeviceWatcher w, Object o)
        {
            WiFiDirectTestLogger.Log("Enumeration Complete!");
            enumCompletedEvent.Set();
        }

        private void OnStopped(DeviceWatcher w, Object o)
        {
            WiFiDirectTestLogger.Log("Watcher Stopped!");
            stoppedEvent.Set();
        }
        #endregion

        #region Dispose
        bool disposed = false;

        public void ResetWatcher()
        {
            if (watcher != null)
            {
                if (watcher.Status == DeviceWatcherStatus.Started || watcher.Status == DeviceWatcherStatus.EnumerationCompleted)
                {
                    watcher.Stop();
                }

                watcher.Added -= OnAdded;
                watcher.EnumerationCompleted -= OnEnumerationComplete;
                watcher.Removed -= OnRemoved;
                watcher.Stopped -= OnStopped;
                watcher.Updated -= OnUpdated;

                watcher = null;
            }
        }

        public void ResetState()
        {
            ResetWatcher();

            foreach (var discovery in discovered.Values.ToList())
            {
                discovery.Dispose();
            }
            discovered.Clear();

            expectations.Clear();
        }

        ~WiFiDirectServicesWatcher()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                ResetState();

                stoppedEvent.Dispose();
                enumCompletedEvent.Dispose();
                foundAllEvent.Dispose();
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("WiFiDirectServicesWatcher");
            }
        }
        #endregion
    }

    /// <summary>
    /// Data from DeviceInformation which can be serialized to pass over remote control
    /// </summary>
    public class DiscoveredServiceInfo
    {
        public DiscoveredServiceInfo()
        { }

        internal DiscoveredServiceInfo(DeviceInformation deviceInfo)
        {
            object serviceName;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceName", out serviceName))
            {
                ServiceName = serviceName.ToString();
            }
            else
            {
                ServiceName = "";
            }

            object advertisementId;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.AdvertisementId", out advertisementId))
            {
                AdvertisementId = Convert.ToUInt32(advertisementId, CultureInfo.InvariantCulture);
            }
            else
            {
                AdvertisementId = 0;
            }

            object configMethods;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceConfigMethods", out configMethods))
            {
                ConfigMethods = Convert.ToUInt32(configMethods, CultureInfo.InvariantCulture);
            }
            else
            {
                ConfigMethods = 0;
            }

            object serviceAddress;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceAddress", out serviceAddress))
            {
                byte[] macBuffer = (byte[])serviceAddress;
                ServiceAddress = new DOT11_MAC_ADDRESS(macBuffer);
            }
            else
            {
                ServiceAddress = new DOT11_MAC_ADDRESS();
            }

            ServiceInfo = "";
            object serviceInfo;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceInformation", out serviceInfo))
            {
                if (serviceInfo != null)
                {
                    IBuffer serviceInfoBuffer;
                    using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                    {
                        serviceInfoDataWriter.WriteBytes((Byte[])serviceInfo);
                        serviceInfoBuffer = serviceInfoDataWriter.DetachBuffer();
                    }

                    using (DataReader serviceInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(serviceInfoBuffer))
                    {
                        ServiceInfo = serviceInfoDataReader.ReadString(serviceInfoBuffer.Length);
                    }
                    WiFiDirectTestLogger.Log("Got Service Info: {0}",
                        WiFiDirectTestUtilities.GetTruncatedString(ServiceInfo, 256));
                }
            }
        }

        public string ServiceName { get; set; }
        public DOT11_MAC_ADDRESS ServiceAddress { get; set; }
        public UInt32 AdvertisementId { get; set; }
        public UInt32 ConfigMethods { get; set; }
        public string ServiceInfo { get; set; }
    }

    /// <summary>
    /// Data from WiFiDirectServiceProvisioningInfo which can be serialized to pass over remote control
    /// </summary>
    public class ProvisioningInfoWrapper
    {
        public ProvisioningInfoWrapper()
        {
        }

        public ProvisioningInfoWrapper(WiFiDirectServiceProvisioningInfo provisioningInfo)
        {
            if (provisioningInfo == null)
            {
                throw new ArgumentNullException("provisioningInfo");
            }
            IsGroupFormationNeeded = provisioningInfo.IsGroupFormationNeeded;
            SelectedConfigMethod = provisioningInfo.SelectedConfigurationMethod;
        }

        public bool IsGroupFormationNeeded { get; set; }
        public WiFiDirectServiceConfigurationMethod SelectedConfigMethod { get; set; }
    }

    /// <summary>
    /// Wraps Windows.Devices.Enumeration.DeviceInformation
    /// </summary>
    internal class DiscoveredService : IDisposable
    {
        private DeviceInformation deviceInfo;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        // When we start opening/connecting to a session we store this (only used for a single connect)
        private WiFiDirectService service = null;
        // Store config method so we know where PIN comes from (if needed)
        private WiFiDirectServiceConfigurationMethod selectedConfigMethod = WiFiDirectServiceConfigurationMethod.Default;

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;

        private bool wasDeferred = false;
        private string deferredSessionInfo = "";

        internal DiscoveredService(
            DeviceInformation deviceInfo,
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.deviceInfo = deviceInfo;
            this.sessionManager = sessionManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
            
            WiFiDirectTestLogger.Log("Created DiscoveredService wrapper (Discovery={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public DeviceInformation DeviceInfo
        {
            get { return deviceInfo; }
        }
        #endregion

        #region Actions
        /// <summary>
        /// Calls WiFiDirectService.FromId to open the seeker session
        /// Caller can then use this.Service to modify settings before connecting
        /// </summary>
        public void OpenSession()
        {
            ThrowIfDisposed();

            if (service != null)
            {
                throw new Exception("Attempted to connect to discovered service a second time!");
            }

            WiFiDirectTestLogger.Log("Connecting to service: Open Session (Discovery={0})", handle);

            wasDeferred = false;
            deferredSessionInfo = "";

            var serviceTask = WiFiDirectService.FromIdAsync(deviceInfo.Id).AsTask();
            serviceTask.Wait();
            service = serviceTask.Result;

            service.SessionDeferred += OnSessionDeferred;
        }

        public void SetServiceOptions(bool preferGO, string sessionInfo = "")
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectTestLogger.Log(
                "Setting Connect options, preferGO={1}, sessionInfo={2} (Discovery={0})",
                handle,
                preferGO,
                WiFiDirectTestUtilities.GetTruncatedString(sessionInfo)
                );

            service.PreferGroupOwnerMode = preferGO;

            if (sessionInfo != "")
            {
                using (var sessionInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    sessionInfoDataWriter.WriteString(sessionInfo);
                    service.SessionInfo = sessionInfoDataWriter.DetachBuffer();
                }
            }
            else
            {
                service.SessionInfo = null;
            }
        }

        /// <summary>
        /// Calls service.GetProvisioningInfoAsync to begin provision discovery and
        /// determine if a PIN will be needed to connect
        /// </summary>
        /// <param name="configMethod">Selected Config Method to use</param>
        public async Task<WiFiDirectServiceProvisioningInfo> GetProvisioningInfoAsync(
            WiFiDirectServiceConfigurationMethod configMethod
            )
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectTestLogger.Log("Connecting to service: GetProvisioningInfoAsync... (Discovery={0})", handle);

            if (service == null)
            {
                throw new Exception("OpenSession called but service is still null, this is a fatal error, need to bail out.");
            }

            WiFiDirectServiceProvisioningInfo provisioningInfo = await service.GetProvisioningInfoAsync(configMethod);

            WiFiDirectTestLogger.Log("Connecting to service: GetProvisioningInfoAsync DONE (Discovery={0})", handle);
            if (provisioningInfo.IsGroupFormationNeeded)
            {
                selectedConfigMethod = provisioningInfo.SelectedConfigurationMethod;
                if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, should DISPLAY PIN (Discovery={0})",
                        handle
                        );
                    this.pinManager.GeneratePin();
                }
                else if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, should ENTER PIN (Discovery={0})",
                        handle
                        );
                }
                else
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, no PIN (Discovery={0})",
                        handle
                        );
                }
            }

            return provisioningInfo;
        }

        /// <summary>
        /// Calls service.ConnectAsync() to connect the session
        /// If the session is not already opened, it calls OpenSession
        /// </summary>
        /// <returns>handle to the connected session</returns>
        public async Task<WFDSvcWrapperHandle> ConnectAsync()
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectServiceSession session = null;

            string pin = "";

            if (selectedConfigMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
            {
                // Just retrieve the PIN we generated to pass to API
                pin = this.pinManager.RetrievePin();
            }
            else if (selectedConfigMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
            {
                // Wait on PIN entry event, retrieve PIN from service
                this.pinManager.WaitForPinEntry();
                pin = this.pinManager.RetrievePin();
            }

            WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync... (pin={1}) (Discovery={0})", handle, pin);
            
            if (service == null)
            {
                throw new Exception("OpenSession called but service is still null, this is a fatal error, need to bail out.");
            }

            if (pin.Length > 0)
            {
                session = await service.ConnectAsync(pin);
            }
            else
            {
                session = await service.ConnectAsync();
            }

            WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync DONE (Discovery={0})", handle);
            
            // Now we are done with this WiFiDirectService instance
            // Clear state so a new connection can be started
            service.SessionDeferred -= OnSessionDeferred;
            service = null;

            ServiceSession sessionWrapper = new ServiceSession(session, sessionManager, errorHandler, wasDeferred, deferredSessionInfo);
            sessionManager.AddSession(sessionWrapper);
            
            WiFiDirectTestLogger.Log("Added session to list (Discovery={0}, Session={1})", handle, sessionWrapper.Handle);

            return sessionWrapper.Handle;
        }
        #endregion

        #region Events
        private void OnSessionDeferred(WiFiDirectService sender, WiFiDirectServiceSessionDeferredEventArgs args)
        {
            try
            {
                WiFiDirectTestLogger.Log("Session Deferred (Discovery={0})", handle);
                wasDeferred = true;

                if (args.DeferredSessionInfo != null && args.DeferredSessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(args.DeferredSessionInfo))
                    {
                        deferredSessionInfo = sessionInfoDataReader.ReadString(args.DeferredSessionInfo.Length);
                        WiFiDirectTestLogger.Log("Received Deferred Session Info: {1} (Discovery={0})", handle, WiFiDirectTestUtilities.GetTruncatedString(deferredSessionInfo));
                    }
                }
                else
                {
                    deferredSessionInfo = "";
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnSessionDeferred: {1} (Advertiser={0})", handle, ex.Message);
                errorHandler.SetError(ex);
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~DiscoveredService()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                if (service != null)
                {
                    service.SessionDeferred -= OnSessionDeferred;
                }
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("DiscoveredService");
            }
        }
        #endregion
    }

    /// <summary>
    /// Data from WiFiDirectServiceSession which can be serialized to pass over remote control
    /// </summary>
    public class ServiceSessionInfo
    {
        public ServiceSessionInfo()
        { }

        internal ServiceSessionInfo(WiFiDirectServiceSession session)
        {
            ServiceName = session.ServiceName;
            ServiceAddress = new DOT11_MAC_ADDRESS(session.ServiceAddress);
            SessionAddress = new DOT11_MAC_ADDRESS(session.SessionAddress);
            SessionId = session.SessionId;
            Status = session.Status;
            ErrorStatus = session.ErrorStatus;
            DeferredSessionInfo = "";
        }

        public string ServiceName { get; set; }
        public DOT11_MAC_ADDRESS ServiceAddress { get; set; }
        public UInt32 AdvertisementId { get; set; }
        public DOT11_MAC_ADDRESS SessionAddress { get; set; }
        public UInt32 SessionId { get; set; }
        public WiFiDirectServiceSessionStatus Status { get; set; }
        public WiFiDirectServiceSessionErrorStatus ErrorStatus { get; set; }
        public string DeferredSessionInfo { get; set; }

        public override string ToString()
        {
            return String.Format(
                CultureInfo.InvariantCulture,
                "name:{0} service({1}#{2}) session({3}#{4}) status={5}, error={6}",
                ServiceName,
                ServiceAddress,
                AdvertisementId,
                SessionAddress,
                SessionId,
                Status,
                ErrorStatus
                );
        }
    }

    /// <summary>
    /// Wraps Windows.Devices.WiFiDirect.Services.WiFiDirectServiceSession
    /// </summary>
    internal class ServiceSession : IDisposable
    {
        private WiFiDirectServiceSession session;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        private IList<StreamSocketListenerWrapper> streamSocketListeners = new List<StreamSocketListenerWrapper>();
        private IDictionary<WFDSvcWrapperHandle, SocketWrapper> socketCollection =
            new Dictionary<WFDSvcWrapperHandle, SocketWrapper>(new WFDSvcWrapperHandle.EqualityComparer());

        // This is for OnRemotePort added sockets so we can access the socket
        private WFDSvcWrapperHandle lastSocketAddedHandle = null;

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesErrorHandler errorHandler;

        private AutoResetEvent sessionClosedEvent = new AutoResetEvent(false);
        private AutoResetEvent remotePortAddedEvent = new AutoResetEvent(false);

        private bool wasDeferred;
        private string deferredSessionInfo;

        internal ServiceSession(
            WiFiDirectServiceSession session,
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            bool wasDeferred = false,
            string deferredSessionInfo = ""
            )
        {
            this.session = session;
            this.sessionManager = sessionManager;
            this.errorHandler = errorHandler;
            this.wasDeferred = wasDeferred;
            this.deferredSessionInfo = deferredSessionInfo;
            
            this.session.SessionStatusChanged += OnSessionStatusChanged;
            this.session.RemotePortAdded += OnRemotePortAdded;

            WiFiDirectTestLogger.Log("Created ServiceSession wrapper (Session={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public WiFiDirectServiceSession Session
        {
            get { return session; }
        }

        public WFDSvcWrapperHandle LastRemoteSocketAdded
        {
            get { return lastSocketAddedHandle; }
        }

        public SocketWrapper GetSocket(WFDSvcWrapperHandle handle)
        {
            ThrowIfDisposed();

            SocketWrapper socket = socketCollection[handle];

            if (socket == null)
            {
                throw new Exception(String.Format(CultureInfo.InvariantCulture, "Socket not found for handle={0}", handle));
            }

            return socket;
        }

        public bool WasDeferred
        {
            get { return wasDeferred; }
        }

        public string DeferredSessionInfo
        {
            get { return deferredSessionInfo; }
        }
        #endregion

        #region Actions
        public async Task<StreamSocketListenerWrapper> AddStreamSocketListenerAsync(UInt16 port)
        {
            ThrowIfDisposed();

            WiFiDirectTestLogger.Log("Adding stream socket listener for TCP port {1} (Session={0})", handle, port);

            var endpointPairCollection = session.GetConnectionEndpointPairs();

            WiFiDirectTestLogger.Log(
                "Endpoint Pairs: LocalHostName={1}, LocalServiceName={2}, RemoteHostName={3}, RemoteServiceName={4} (Session={0})",
                handle,
                endpointPairCollection[0].LocalHostName,
                endpointPairCollection[0].LocalServiceName,
                endpointPairCollection[0].RemoteHostName,
                endpointPairCollection[0].RemoteServiceName
                );

            // Create listener for socket connection (and add to list to cleanup later)
            StreamSocketListener listenerSocket = new StreamSocketListener();
            StreamSocketListenerWrapper listenerWrapper = new StreamSocketListenerWrapper(
                errorHandler,
                listenerSocket,
                this
                );
            streamSocketListeners.Add(listenerWrapper);

            WiFiDirectTestLogger.Log("Added socket listener (Listener={0})", listenerWrapper.Handle);

            WiFiDirectTestLogger.Log("BindEndpointAsync... (TCP port {1}) (Session={0})", handle, port);
            await listenerSocket.BindEndpointAsync(endpointPairCollection[0].LocalHostName, Convert.ToString(port, CultureInfo.InvariantCulture));
            WiFiDirectTestLogger.Log("BindEndpointAsync...DONE (TCP port {1}) (Session={0})", handle, port);

            WiFiDirectTestLogger.Log("AddStreamSocketListenerAsync... (TCP port {1}) (Session={0})", handle, port);
            await session.AddStreamSocketListenerAsync(listenerSocket);
            WiFiDirectTestLogger.Log("AddStreamSocketListenerAsync...DONE (TCP port {1}) (Session={0})", handle, port);

            return listenerWrapper;
        }

        /// <summary>
        /// Used by the socket listener to add socket to list but track its handle
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="datagram"></param>
        /// <returns>Handle to the new socket</returns>
        public WFDSvcWrapperHandle AddSocketInternal(StreamSocket stream, DatagramSocket datagram)
        {
            return WrapSocket(stream, datagram).Handle;
        }

        public async Task<SocketWrapper> AddDatagramSocketListenerAsync(UInt16 port)
        {
            ThrowIfDisposed();

            WiFiDirectTestLogger.Log("Adding datagram socket for UDP port {1} (Session={0})", handle, port);

            var endpointPairCollection = session.GetConnectionEndpointPairs();

            WiFiDirectTestLogger.Log(
                "Endpoint Pairs: LocalHostName={1}, LocalServiceName={2}, RemoteHostName={3}, RemoteServiceName={4} (Session={0})",
                handle,
                endpointPairCollection[0].LocalHostName,
                endpointPairCollection[0].LocalServiceName,
                endpointPairCollection[0].RemoteHostName,
                endpointPairCollection[0].RemoteServiceName
                );

            DatagramSocket socket = new DatagramSocket();
            SocketWrapper socketWrapper = WrapSocket(null, socket);

            WiFiDirectTestLogger.Log("BindEndpointAsync... (UDP port {1}) (Session={0})", handle, port);
            await socket.BindEndpointAsync(endpointPairCollection[0].LocalHostName, Convert.ToString(port, CultureInfo.InvariantCulture));
            WiFiDirectTestLogger.Log("BindEndpointAsync...DONE (TCP port {1}) (Session={0})", handle, port);

            WiFiDirectTestLogger.Log("AddDatagramSocketAsync... (UDP port {1}) (Session={0})", handle, port);
            await session.AddDatagramSocketAsync(socket);
            WiFiDirectTestLogger.Log("AddDatagramSocketAsync...DONE (UDP port {1}) (Session={0})", handle, port);

            return socketWrapper;
        }

        public void CloseSession()
        {
            ThrowIfDisposed();

            WiFiDirectTestLogger.Log("Closing sockets for session (Session={0})", handle);

            foreach (var socket in socketCollection.Values.ToList())
            {
                socket.Dispose();
            }
            socketCollection.Clear();

            WiFiDirectTestLogger.Log("Closing session (Session={0})", handle);

            session.Dispose();
            WaitForSessionClosed();

            WiFiDirectTestLogger.Log("Session Closed!", handle);
        }
        #endregion

        #region Events
        private void OnSessionStatusChanged(WiFiDirectServiceSession session, object o)
        {
            try
            {
                WiFiDirectTestLogger.Log("Session Status Changed to {1} (Session={0})", handle, session.Status.ToString());

                if (session.Status == WiFiDirectServiceSessionStatus.Closed)
                {
                    WiFiDirectTestLogger.Log("Session was closed (Session={0})", handle);

                    if (!disposed)
                    {
                        sessionClosedEvent.Set();
                    }
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnSessionStatusChanged: {1} (Session={0})", handle, ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private async void OnRemotePortAdded(WiFiDirectServiceSession session, WiFiDirectServiceRemotePortAddedEventArgs args)
        {
            try
            {
                ThrowIfDisposed();

                var endpointPairCollection = args.EndpointPairs;
                var protocol = args.Protocol;
                if (endpointPairCollection.Count == 0)
                {
                    WiFiDirectTestLogger.Error("No endpoint pairs for remote port added event (Session={0})", handle);
                    return;
                }

                WiFiDirectTestLogger.Log(
                    "Port {1} ({2}) added by remote side (Session={0})",
                    handle,
                    endpointPairCollection[0].RemoteServiceName,
                    (protocol == WiFiDirectServiceIPProtocol.Tcp) ? "TCP" : ((protocol == WiFiDirectServiceIPProtocol.Udp) ? "UDP" : "???")
                    );

                WiFiDirectTestLogger.Log(
                    "Endpoint Pairs: LocalHostName={1}, LocalServiceName={2}, RemoteHostName={3}, RemoteServiceName={4} (Session={0})",
                    handle,
                    endpointPairCollection[0].LocalHostName,
                    endpointPairCollection[0].LocalServiceName,
                    endpointPairCollection[0].RemoteHostName,
                    endpointPairCollection[0].RemoteServiceName
                    );

                SocketWrapper socketWrapper = null;

                if (args.Protocol == WiFiDirectServiceIPProtocol.Tcp)
                {
                    StreamSocket streamSocket = new StreamSocket();
                    socketWrapper = WrapSocket(streamSocket, null);
                    WiFiDirectTestLogger.Log("Connecting to StreamSocket... (TCP) (Session={0})", handle);
                    await streamSocket.ConnectAsync(endpointPairCollection[0]);
                    WiFiDirectTestLogger.Log("Done Connecting to StreamSocket (TCP) (Session={0})", handle);
                }
                else if (args.Protocol == WiFiDirectServiceIPProtocol.Udp)
                {
                    DatagramSocket datagramSocket = new DatagramSocket();
                    socketWrapper = WrapSocket(null, datagramSocket);
                    WiFiDirectTestLogger.Log("Connecting to DatagramSocket... (UDP) (Session={0})", handle);
                    await datagramSocket.ConnectAsync(endpointPairCollection[0]);
                    WiFiDirectTestLogger.Log("Done Connecting to DatagramSocket (UDP) (Session={0})", handle);
                }
                else
                {
                    WiFiDirectTestLogger.Error("Bad protocol for remote port added event (Session={0})", handle);
                    return;
                }

                lastSocketAddedHandle = socketWrapper.Handle;

                remotePortAddedEvent.Set();
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("OnRemotePortAdded threw an exception: {1} (Session={0})", handle, ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private SocketWrapper WrapSocket(StreamSocket stream, DatagramSocket datagram)
        {
            SocketWrapper socketWrapper = new SocketWrapper(errorHandler, stream, datagram);
            socketCollection.Add(socketWrapper.Handle, socketWrapper);
            WiFiDirectTestLogger.Log("Created socket wrapper (Session={0}, Socket={1})", handle, socketWrapper.Handle);

            return socketWrapper;
        }
        #endregion

        #region Synchronization
        public void WaitForSessionClosed(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!sessionClosedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for session to close");
            }
        }

        public void WaitForRemotePortAdded(int msTimeout = 5000)
        {
            ThrowIfDisposed();
            if (!remotePortAddedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for remote port added event");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~ServiceSession()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                // Close session
                session.Dispose();

                session.RemotePortAdded -= OnRemotePortAdded;
                session.SessionStatusChanged -= OnSessionStatusChanged;

                foreach (var listener in streamSocketListeners)
                {
                    listener.Dispose();
                }

                foreach (var socket in socketCollection.Values.ToList())
                {
                    socket.Dispose();
                }
                socketCollection.Clear();

                // Remove from the main list
                sessionManager.RemoveSession(handle);

                sessionClosedEvent.Dispose();
                remotePortAddedEvent.Dispose();
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("ServiceSession");
            }
        }
        #endregion
    }

    internal class StreamSocketListenerWrapper : IDisposable
    {
        private StreamSocketListener streamSocketListener;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        private AutoResetEvent connectionEvent = new AutoResetEvent(false);
        private WFDSvcWrapperHandle lastSocket = null;

        private ServiceSession session;
        private IWiFiDirectServicesErrorHandler errorHandler;

        internal StreamSocketListenerWrapper(
            IWiFiDirectServicesErrorHandler errorHandler,
            StreamSocketListener streamSocketListener,
            ServiceSession session
            )
        {
            this.streamSocketListener = streamSocketListener;
            this.session = session;
            this.errorHandler = errorHandler;

            this.streamSocketListener.ConnectionReceived += OnConnectionReceived;

            WiFiDirectTestLogger.Log("Created Stream Socket Listener wrapper (Listener={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public WFDSvcWrapperHandle LastSocketHandle
        {
            get { return lastSocket; }
        }
        #endregion

        #region Events
        private void OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
        {
            WiFiDirectTestLogger.Log("Connection received for TCP Port {1} (Listener={0})", handle, sender.Information.LocalPort);

            lastSocket = session.AddSocketInternal(args.Socket, null);
            connectionEvent.Set();
        }
        #endregion

        #region Synchronization
        public void WaitForConnection(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!connectionEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for socket connection");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~StreamSocketListenerWrapper()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                try
                {
                    streamSocketListener.ConnectionReceived -= OnConnectionReceived;
                }
                catch (Exception)
                { }

                streamSocketListener.Dispose();
                connectionEvent.Dispose();
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StreamSocketListenerWrapper");
            }
        }
        #endregion
    }

    /// <summary>
    /// Wraps a StreamSocket or DatagramSocket
    /// </summary>
    internal class SocketWrapper : IDisposable
    {
        private StreamSocket streamSocket = null;
        private DatagramSocket datagramSocket = null;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        private DataReader reader = null;
        private DataWriter writer;

        private object receivedMessageLock = new object();
        private Queue<string> receivedMessages = new Queue<string>();

        private IWiFiDirectServicesErrorHandler errorHandler;

        private AutoResetEvent messageSent = new AutoResetEvent(false);
        private AutoResetEvent messageReceived = new AutoResetEvent(false);

        internal SocketWrapper(
            IWiFiDirectServicesErrorHandler errorHandler,
            StreamSocket streamSocket = null,
            DatagramSocket datagramSocket = null
            )
        {
            this.streamSocket = streamSocket;
            this.datagramSocket = datagramSocket;
            this.errorHandler = errorHandler;

            if (streamSocket == null && datagramSocket == null)
            {
                WiFiDirectTestLogger.Error("No Socket provided for wrapper!!! (Socket={0})", handle);
                throw new Exception("Bad SocketWrapper parameters");
            }
            else if (streamSocket != null && datagramSocket != null)
            {
                WiFiDirectTestLogger.Error("Socket wrapper should be for TCP or UDP, both provided!!! (Socket={0})", handle);
                throw new Exception("Bad SocketWrapper parameters");
            }
            else if (streamSocket != null)
            {
                reader = new DataReader(streamSocket.InputStream);
                writer = new DataWriter(streamSocket.OutputStream);
            }
            else
            {
                datagramSocket.MessageReceived += OnUDPMessageReceived;
                writer = new DataWriter(datagramSocket.OutputStream);
            }

            if (reader != null)
            {
                reader.UnicodeEncoding = UnicodeEncoding.Utf8;
                reader.ByteOrder = ByteOrder.LittleEndian;
            }

            writer.UnicodeEncoding = UnicodeEncoding.Utf8;
            writer.ByteOrder = ByteOrder.LittleEndian;

            WiFiDirectTestLogger.Log("Created Socket wrapper (Socket={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        // TODO: public StreamSocket StreamSocket

        // TODO: public DatagramSocket DatagramSocket

        public bool IsTCP
        {
            get { return streamSocket != null; }
        }

        /*public bool IsUDP
        {
            get { return datagramSocket != null; }
        }*/

        public bool HasReceivedMessages()
        {
            lock (receivedMessageLock)
            {
                return receivedMessages.Count > 0;
            }
        }

        public string GetNextReceivedMessage()
        {
            lock (receivedMessageLock)
            {
                if (receivedMessages.Count == 0)
                {
                    return "";
                }
                return receivedMessages.Dequeue();
            }
        }
        #endregion

        #region Actions
        /// <summary>
        /// Send a string over the socket (TCP or UDP)
        /// Will send as a uint32 size followed by the message
        /// </summary>
        /// <param name="message">Message to send</param>
        public async Task SendMessageAsync(string message)
        {
            try
            {
                ThrowIfDisposed();

                writer.WriteUInt32(writer.MeasureString(message));
                writer.WriteString(message);
                await writer.StoreAsync();

                uint bytesSent = writer.MeasureString(message);

                WiFiDirectTestLogger.Log(
                    "Sent Message: \"{2}\", {1} bytes (Socket={0})",
                    handle,
                    bytesSent,
                    message.Substring(0, 32) + ((message.Length > 32) ? "..." : "")
                    );

                messageSent.Set();
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("SendMessage threw exception: {1} (Socket={0})", handle, ex.Message);
                throw;
            }
        }

        /// <summary>
        /// Read strings sent over the socket (TCP only currently)
        /// Reads the uint32 size then the actual string
        /// Stores string in a queue that can be read separately
        /// </summary>
        public async Task ReceiveMessageAsync()
        {
            await HandleReceivedMessage(this.reader);
        }

        private async Task HandleReceivedMessage(DataReader datareader, bool load = true)
        {
            try
            {
                ThrowIfDisposed();
                uint bytesRead = 0;

                if (load)
                {
                    bytesRead = await datareader.LoadAsync(sizeof(uint));

                    if (bytesRead != sizeof(uint)) 
                    {
                        WiFiDirectTestLogger.Log(
                            "The underlying socket was closed before we were able to read the whole data. (Socket={0})",
                            handle
                            );
                        return;
                    }
                }

                if (!load || bytesRead > 0)
                {
                    // Determine how long the string is.
                    uint messageLength = (uint)datareader.ReadUInt32();

                    WiFiDirectTestLogger.Log(
                            "Received Message: of size {1}, trying to read (Socket={0})",
                            handle,
                            messageLength
                            );

                    if (load)
                    {
                        bytesRead = await datareader.LoadAsync(messageLength);
                        if (bytesRead != messageLength)
                        {
                            WiFiDirectTestLogger.Log(
                                "The underlying socket was closed before we were able to read the whole data. (Socket={0})",
                                handle
                                ); 
                            return;
                        }
                    }

                    if ((!load && messageLength > 0) || bytesRead > 0)
                    {
                        // Decode the string.
                        string currentMessage = datareader.ReadString(messageLength);
                        uint bytesReceived = messageLength;
                        WiFiDirectTestLogger.Log(
                            "Received Message: \"{2}\", {1} bytes (Socket={0})",
                            handle,
                            bytesReceived,
                            currentMessage.Substring(0, 32) + ((currentMessage.Length > 32) ? "..." : "")
                            );

                        // Store in a queue for reading
                        lock (receivedMessageLock)
                        {
                            receivedMessages.Enqueue(currentMessage);
                            WiFiDirectTestLogger.Log("ReceiveMessage queue size={1} (Socket={0})", handle, receivedMessages.Count);
                        }

                        messageReceived.Set();
                    }
                    else
                    {
                        WiFiDirectTestLogger.Log("ReceiveMessage 0 bytes loaded for message content (Socket={0})", handle);
                    }
                }
                else
                {
                    WiFiDirectTestLogger.Log("ReceiveMessage 0 bytes loaded for message size (Socket={0})", handle);
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("ReceiveMessage threw exception: {1} (Socket={0})", handle, ex.Message);
                throw;
            }
        }
        #endregion

        #region Events
        private async void OnUDPMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
        {
            WiFiDirectTestLogger.Log("Handling received UDP message (Socket={0})", handle);
            try
            {
                DataReader udpReader = args.GetDataReader();
                udpReader.UnicodeEncoding = UnicodeEncoding.Utf8;
                udpReader.ByteOrder = ByteOrder.LittleEndian;

                await HandleReceivedMessage(udpReader, false);
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Catching exception in OnUDPMessageReceived: {1} (Socket={0})", handle, ex.Message);
            }
        }
        #endregion

        #region Synchronization
        public void WaitForSend(int msTimeout = 5000)
        {
            ThrowIfDisposed();
            if (!messageSent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for socket message send");
            }
        }

        public void WaitForReceive(int msTimeout = 5000)
        {
            ThrowIfDisposed();
            if (!messageReceived.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for socket message receive");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~SocketWrapper()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                if (streamSocket != null)
                {
                    streamSocket.Dispose();
                }

                if (datagramSocket != null)
                {
                    datagramSocket.Dispose();
                    datagramSocket.MessageReceived -= OnUDPMessageReceived;
                }

                if (writer != null)
                {
                    writer.Dispose();
                }

                if (reader != null)
                {
                    reader.Dispose();
                }

                messageReceived.Dispose();
                messageSent.Dispose();
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("SocketWrapper");
            }
        }
        #endregion
    }
}