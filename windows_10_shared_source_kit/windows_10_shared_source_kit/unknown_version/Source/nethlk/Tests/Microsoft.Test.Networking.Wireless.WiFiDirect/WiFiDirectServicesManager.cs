///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.RemoteControl;
using Windows.Devices.WiFiDirect.Services;
using Windows.Devices.Enumeration;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    /// <summary>
    /// Interface for class that manages a collection of sessions
    /// Used by sessions for handling status change callback and removing selves
    /// and used by Advertiser/Discovered service for adding connected sessions
    /// </summary>
    internal interface IWiFiDirectServicesSessionManager
    {
        void AddSession(ServiceSession session);

        void RemoveSession(WFDSvcWrapperHandle handle);
    }

    internal interface IWiFiDirectServicesDiscoveryManager
    {
        void AddDiscovery(DiscoveredService service);

        void RemoveDiscovery(WFDSvcWrapperHandle handle);

        void ClearDiscoveries();
    }

    internal interface IWiFiDirectServicesErrorHandler
    {
        void SetError(Exception ex);

        bool HasError();

        Exception GetError();
    }

    internal interface IWiFIDirectServicesPinManager
    {
        void GeneratePin();

        void EnterPin(string pin);

        string RetrievePin();

        void ClearPin();

        void WaitForPinDisplay(int msTimeout = 3000);

        void WaitForPinEntry(int msTimeout = 3000);
    }

    /// <summary>
    /// Manages all WFD Service wrappers and objects
    /// Used as a factory to create objects (start discovery, publish)
    /// Used to lookup created objects
    /// 
    /// Wrapper classes themselves handle all other logic
    /// </summary>
    internal class WiFiDirectServicesManager
        : IWiFiDirectServicesSessionManager,
          IWiFiDirectServicesErrorHandler,
          IWiFiDirectServicesDiscoveryManager,
          IWiFIDirectServicesPinManager,
          IDisposable
    {
        private IDictionary<WFDSvcWrapperHandle, ServiceAdvertiser> advertiserCollection =
            new Dictionary<WFDSvcWrapperHandle, ServiceAdvertiser>(new WFDSvcWrapperHandle.EqualityComparer());
        private IDictionary<WFDSvcWrapperHandle, DiscoveredService> discoveryCollection =
            new Dictionary<WFDSvcWrapperHandle, DiscoveredService>(new WFDSvcWrapperHandle.EqualityComparer());
        private IDictionary<WFDSvcWrapperHandle, ServiceSession> sessionCollection =
            new Dictionary<WFDSvcWrapperHandle, ServiceSession>(new WFDSvcWrapperHandle.EqualityComparer());

        private WiFiDirectServicesWatcher watcher;

        private string pin = "";
        private AutoResetEvent pinDisplayedEvent = new AutoResetEvent(false);
        private AutoResetEvent pinEnteredEvent = new AutoResetEvent(false);

        private Exception error = null;
        private static uint serviceNameSeed = 1;
        private Random rng = new Random();

        public WiFiDirectServicesManager()
        {
            watcher = new WiFiDirectServicesWatcher(this, this, this, this);
        }

        #region Lookup
        public ServiceAdvertiser GetAdvertiser(WFDSvcWrapperHandle handle)
        {
            ThrowIfDisposed();
            ServiceAdvertiser advertiser = advertiserCollection[handle];

            if (advertiser == null)
            {
                throw new Exception(String.Format(CultureInfo.InvariantCulture, "Advertiser not found for handle={0}", handle));
            }

            return advertiser;
        }

        public DiscoveredService GetDiscoveredService(WFDSvcWrapperHandle handle)
        {
            ThrowIfDisposed();
            DiscoveredService service = discoveryCollection[handle];

            if (service == null)
            {
                throw new Exception(String.Format(CultureInfo.InvariantCulture, "Discovered service not found for handle={0}", handle));
            }

            return service;
        }

        public ServiceSession GetSession(WFDSvcWrapperHandle handle)
        {
            ThrowIfDisposed();
            ServiceSession session = sessionCollection[handle];

            if (session == null)
            {
                throw new Exception(String.Format(CultureInfo.InvariantCulture, "Session not found for handle={0}", handle));
            }

            return session;
        }

        public int GetSessionCount()
        {
            ThrowIfDisposed();

            return sessionCollection.Count;
        }
        #endregion

        #region Advertiser
        /// <summary>
        /// Generate a service name that should be unique to the machine
        /// Avoid conflicts with other advertised services by including the MAC address
        /// Avoid conflicts with previous test cases by appending a unique integer
        /// </summary>
        /// <param name="macAddress"></param>
        /// <param name="prefix"></param>
        /// <returns></returns>
        public string GenerateUniqueServiceName(
            DOT11_MAC_ADDRESS macAddress,
            string prefix = "com.microsoft.kit"
            )
        {
            return string.Format(
                CultureInfo.InvariantCulture,
                "{0}.{1}.{2}",
                prefix,
                macAddress.ToString().Replace(":", null),
                serviceNameSeed++
                );
        }

        /// <summary>
        /// Start advertising a service
        /// </summary>
        /// <param name="serviceName"></param>
        /// <param name="autoAccept"></param>
        /// <param name="preferGO"></param>
        /// <param name="configMethods"></param>
        /// <param name="status"></param>
        /// <param name="customStatus"></param>
        /// <param name="serviceInfo"></param>
        /// <param name="deferredServiceInfo"></param>
        /// <returns></returns>
        public WFDSvcWrapperHandle PublishService(
            string serviceName,
            bool autoAccept = true,
            bool preferGO = true,
            List<WiFiDirectServiceConfigurationMethod> configMethods = null,
            WiFiDirectServiceStatus status = WiFiDirectServiceStatus.Available,
            uint customStatus = 0,
            string serviceInfo = "",
            string deferredServiceInfo = "",
            List<String> prefixList = null
            )
        {
            ThrowIfDisposed();

            WiFiDirectTestLogger.Log("Creating Service: \"{0}\"", serviceName);
            
            WiFiDirectServiceAdvertiser advertiser = new WiFiDirectServiceAdvertiser(serviceName);
            advertiser.AutoAcceptSession = autoAccept;
            advertiser.PreferGroupOwnerMode = preferGO;
            advertiser.ServiceStatus = status;
            advertiser.CustomServiceStatusCode = customStatus;

            if (serviceInfo != null && serviceInfo.Length > 0)
            {
                using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    serviceInfoDataWriter.WriteString(serviceInfo);
                    advertiser.ServiceInfo = serviceInfoDataWriter.DetachBuffer();
                }
                WiFiDirectTestLogger.Log("Included Service Info: \"{0}\"", WiFiDirectTestUtilities.GetTruncatedString(serviceInfo));
            }
            else
            {
                advertiser.ServiceInfo = null;
            }

            if (deferredServiceInfo != null && deferredServiceInfo.Length > 0)
            {
                using (var deferredSessionInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    deferredSessionInfoDataWriter.WriteString(deferredServiceInfo);
                    advertiser.DeferredSessionInfo = deferredSessionInfoDataWriter.DetachBuffer();
                }
                WiFiDirectTestLogger.Log("Included Session Info: \"{0}\"", WiFiDirectTestUtilities.GetTruncatedString(deferredServiceInfo));
            }
            else
            {
                advertiser.DeferredSessionInfo = null;
            }

            if (configMethods != null)
            {
                advertiser.PreferredConfigurationMethods.Clear();
                foreach (var configMethod in configMethods)
                {
                    advertiser.PreferredConfigurationMethods.Add(configMethod);
                    WiFiDirectTestLogger.Log("Added config method {0}", configMethod.ToString());
                }
            }

            if (prefixList != null && prefixList.Count > 0)
            {
                advertiser.ServiceNamePrefixes.Clear();
                foreach (var prefix in prefixList)
                {
                    advertiser.ServiceNamePrefixes.Add(prefix);
                    WiFiDirectTestLogger.Log("Added prefix {0}", prefix);
                }
            }

            ServiceAdvertiser advertiserWrapper = new ServiceAdvertiser(advertiser, this, this, this);
            advertiserCollection.Add(advertiserWrapper.Handle, advertiserWrapper);

            WiFiDirectTestLogger.Log("Starting Service: \"{1}\" (Advertiser={0})", advertiserWrapper.Handle, serviceName);
            advertiser.Start();

            return advertiserWrapper.Handle;
        }

        public void UnPublishService(ServiceAdvertiser advertiser, bool fRemove = false)
        {
            ThrowIfDisposed();

            WFDSvcWrapperHandle handle = advertiser.Handle;

            WiFiDirectTestLogger.Log("Unpublishing advertiser (Advertiser={0} ServiceName={1})", handle, advertiser.Advertiser.ServiceName);

            advertiser.Advertiser.Stop();

            if (fRemove)
            {
                WiFiDirectTestLogger.Log("Removing advertiser (Advertiser={0} ServiceName={1})", handle, advertiser.Advertiser.ServiceName);
                advertiser.Dispose();

                advertiserCollection.Remove(handle);
            }

            WiFiDirectTestLogger.Log("Unpublish service done (Advertiser={0})", handle);
        }
        #endregion

        #region Discovery
        public async Task<AsyncHandleListResult> DiscoverServicesAsync(string serviceName, bool fExpectResults = true, string serviceInfo = "")
        {
            List<WFDSvcWrapperHandle> results = new List<WFDSvcWrapperHandle>();
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log("Discover Services... (ServiceName={0}, ServiceInfo={1})", serviceName, serviceInfo);

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

                WiFiDirectTestLogger.Log("FindAllAsync... Using Selector: {0}", serviceSelector);

                List<string> additionalProperties = new List<string>();
                additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceAddress");
                additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceName");
                additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceInformation");
                additionalProperties.Add("System.Devices.WiFiDirectServices.AdvertisementId");
                additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceConfigMethods");

                DeviceInformationCollection deviceInfoCollection = await DeviceInformation.FindAllAsync(serviceSelector, additionalProperties);

                WiFiDirectTestLogger.Log(
                    "FindAllAsync...DONE Got {0} results",
                    (deviceInfoCollection == null) ? 0 : deviceInfoCollection.Count
                    );

                if (deviceInfoCollection == null || deviceInfoCollection.Count == 0)
                {
                    if (fExpectResults)
                    {
                        WiFiDirectTestLogger.Error("No services found!");
                        throw new Exception("Expected services discovered, found none!");
                    }
                    else
                    {
                        WiFiDirectTestLogger.Log("No services found (expected)");
                    }
                }
                else
                {
                    if (!fExpectResults)
                    {
                        WiFiDirectTestLogger.Error("Services found, none expected!");
                        throw new Exception("Expected no services discovered, found at least 1!");
                    }
                    else
                    {
                        // NOTE: we don't clear this list
                        // Possible to discover A, discover B, then connect to A because its entry still exists
                        foreach (DeviceInformation deviceInfo in deviceInfoCollection)
                        {
                            WiFiDirectTestLogger.Log(
                                "Adding discovered service (Id={0}, Name={1})",
                                deviceInfo.Id,
                                deviceInfo.Name
                                );

                            DiscoveredService discovered = new DiscoveredService(deviceInfo, this, this, this);
                            discoveryCollection.Add(discovered.Handle, discovered);

                            results.Add(discovered.Handle);
                        }
                    }
                }

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Exception in DiscoverServicesAsync");
                error = ex;
            }

            return new AsyncHandleListResult(results, success, error);
        }

        #region Watcher
        public void ResetWatcher()
        {
            watcher.ResetState();
        }
        public void AddExpectedServiceToWatcher(ServiceAdvertiserInfo service)
        {
            watcher.AddExpectedService(service);
        }
        public List<WFDSvcWrapperHandle> WatchServices(string serviceName, string serviceInfo = "")
        {
            if (!watcher.HasExpectations())
            {
                WiFiDirectTestLogger.Error("Watcher needs expected services to run");
                throw new Exception("Cannot use watcher without a condition to stop");
            }

            watcher.Start(serviceName, serviceInfo);

            watcher.WaitForFindAll();

            watcher.Stop();

            List<WFDSvcWrapperHandle> results = discoveryCollection.Keys.ToList();

            return results;
        }
        #endregion
        #endregion

        #region Connect
        public async Task<AsyncProvisioningInfoResult> GetProvisioningInfoAsync(
            DiscoveredService discovered,
            WiFiDirectServiceConfigurationMethod configMethod
            )
        {
            WiFiDirectServiceProvisioningInfo provisioningInfo = null;
            ProvisioningInfoWrapper provisioningWrapper = null;
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log(
                    "GetProvisioningInfoAsync... (config method={0}) (Discovery={1})",
                    configMethod.ToString(),
                    discovered.Handle
                    );

                provisioningInfo = await discovered.GetProvisioningInfoAsync(configMethod);

                provisioningWrapper = new ProvisioningInfoWrapper(provisioningInfo);

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in GetProvisioningInfoAsync (this may be expected)");
                error = ex;
            }

            return new AsyncProvisioningInfoResult(provisioningWrapper, success, error);
        }

        public async Task<AsyncHandleResult> ConnectAsync(
            DiscoveredService discovered
            )
        {
            WFDSvcWrapperHandle sessionHandle = null;
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log("ConnectAsync... (Discovery={0})", discovered.Handle);

                sessionHandle = await discovered.ConnectAsync();

                ServiceSession session = GetSession(sessionHandle);

                if (session.Session.Status != WiFiDirectServiceSessionStatus.Open)
                {
                    throw new Exception("Session did not start successfully");
                }

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in ConnectAsync (this may be expected)");
                error = ex;
            }

            return new AsyncHandleResult(sessionHandle, success, error);
        }

        public WFDSvcWrapperHandle GetLastAdvertiserSession(
            ServiceAdvertiser advertiser,
            bool autoAccept
            )
        {
            WiFiDirectTestLogger.Log("Waiting for Advertiser to get session connection (Advertiser={0})", advertiser.Handle);
            if (autoAccept)
            {
                advertiser.WaitForAutoAcceptSessionConnected();
            }
            else
            {
                advertiser.WaitForSessionRequested();
            }

            WFDSvcWrapperHandle sessionHandle = advertiser.LastConnectedSession;

            return sessionHandle;
        }
        #endregion

        #region Session
        public async Task<AsyncHandleResult> AddStreamSocketAsync(
            ServiceSession session,
            UInt16 port
            )
        {
            WFDSvcWrapperHandle socketHandle = null;
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log(
                    "AddStreamSocketAsync... (port={0}) (Session={1})",
                    port.ToString(),
                    session.Handle
                    );

                StreamSocketListenerWrapper listener = await session.AddStreamSocketListenerAsync(port);

                listener.WaitForConnection();

                socketHandle = listener.LastSocketHandle;

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in AddStreamSocketAsync (this may be expected)");
                error = ex;
            }

            return new AsyncHandleResult(socketHandle, success, error);
        }

        public async Task<AsyncHandleResult> AddDatagramSocketAsync(
            ServiceSession session,
            UInt16 port
            )
        {
            WFDSvcWrapperHandle socketHandle = null;
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log(
                    "AddDatagramSocketAsync... (port={0}) (Session={1})",
                    port.ToString(),
                    session.Handle
                    );

                SocketWrapper socket = await session.AddDatagramSocketListenerAsync(port);

                socketHandle = socket.Handle;

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in AddDatagramSocketAsync (this may be expected)");
                error = ex;
            }

            return new AsyncHandleResult(socketHandle, success, error);
        }

        public WFDSvcWrapperHandle GetLastRemoteSocketAdded(
            ServiceSession session
            )
        {
            WiFiDirectTestLogger.Log("Waiting for session to get RemotePortAdded event and setup socket (Session={0})", session.Handle);
            session.WaitForRemotePortAdded();

            WFDSvcWrapperHandle socketHandle = session.LastRemoteSocketAdded;

            return socketHandle;
        }

        public async Task<AsyncResult> SendMessageAsync(
            SocketWrapper socket,
            string message
            )
        {
            Exception error = null;
            bool success = false;

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log(
                    "SendMessageAsync... (Socket={0})",
                    socket.Handle
                    );

                await socket.SendMessageAsync(message);

                socket.WaitForSend();

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in SendMessageAsync (this may be expected)");
                error = ex;
            }

            return new AsyncResult(success, error);
        }

        public async Task<AsyncStringResult> ReceiveMessageAsync(
            SocketWrapper socket
            )
        {
            Exception error = null;
            bool success = false;
            string message = "";

            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log(
                    "ReceiveMessageAsync... (Socket={0})",
                    socket.Handle
                    );

                // TCP explicitly receives a message, UDP has a callback
                if (socket.IsTCP)
                {
                    // Wait to receive a message for up to 20 seconds
                    Task receiveMessageTask = socket.ReceiveMessageAsync();
                    if (receiveMessageTask == await Task.WhenAny(receiveMessageTask, Task.Delay(20000)))
                    {
                        WiFiDirectTestLogger.Log(
                            "ReceiveMessageAsync DONE (Socket={0})",
                            socket.Handle
                            );
                    }
                    else
                    {
                        throw new Exception("Timeout waiting for socket to receive message!");
                    }
                }

                socket.WaitForReceive();

                if (!socket.HasReceivedMessages())
                {
                    throw new Exception("Did not actually receive message over socket!");
                }

                while (socket.HasReceivedMessages())
                {
                    WiFiDirectTestLogger.Log("Reading message...");
                    message += socket.GetNextReceivedMessage();
                }

                success = true;
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Exception in SendMessageAsync (this may be expected)");
                error = ex;
            }

            return new AsyncStringResult(message, success, error);
        }
        #endregion

        #region IWiFiDirectServicesSessionManager
        public void AddSession(ServiceSession session)
        {
            sessionCollection.Add(session.Handle, session);
            // Make sure we clear the global pin display/entry state for new connections
            ClearPin();
        }

        public void RemoveSession(WFDSvcWrapperHandle handle)
        {
            bool fRemoved = sessionCollection.Remove(handle);
            if (!fRemoved)
            {
                WiFiDirectTestLogger.Error("Remove session failed to find session in list! (Session={0})", handle);
                throw new Exception("Failed to remove session");
            }
        }
        #endregion

        #region IWiFiDirectServicesDiscoveryManager
        public void AddDiscovery(DiscoveredService service)
        {
            discoveryCollection.Add(service.Handle, service);
        }

        public void RemoveDiscovery(WFDSvcWrapperHandle handle)
        {
            bool fRemoved = discoveryCollection.Remove(handle);
            if (!fRemoved)
            {
                WiFiDirectTestLogger.Error("Remove discovery failed to find discovery in list! (Session={0})", handle);
                throw new Exception("Failed to remove discovery");
            }
        }

        public void ClearDiscoveries()
        {
            discoveryCollection.Clear();
        }
        #endregion

        #region IWiFiDirectServicesErrorHandler
        public void SetError(Exception ex)
        {
            error = ex;
        }

        public bool HasError()
        {
            return error != null;
        }

        public Exception GetError()
        {
            return error;
        }
        #endregion

        #region IWiFIDirectServicesPinManager
        public void GeneratePin()
        {
            ThrowIfDisposed();
            const string s_chars = "1234567890";

            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < 8; i++)
            {
                builder.Append(s_chars[rng.Next(s_chars.Length)]);
            }
            this.pin = builder.ToString();
            
            WiFiDirectTestLogger.Log("Generated PIN for display: {0}", this.pin);

            pinDisplayedEvent.Set();
        }

        public void EnterPin(string pin)
        {
            ThrowIfDisposed();
            this.pin = pin;

            WiFiDirectTestLogger.Log("Entered PIN: {0}", this.pin);

            pinEnteredEvent.Set();
        }

        public string RetrievePin()
        {
            ThrowIfDisposed();
            if (this.pin == "")
            {
                throw new Exception(String.Format(CultureInfo.InvariantCulture, "Attempted to retrieve pin, not found"));
            }
            
            return this.pin;
        }

        public void ClearPin()
        {
            WiFiDirectTestLogger.Log("Clearing PIN state");
            
            pinDisplayedEvent.Reset();
            pinEnteredEvent.Reset();
            this.pin = "";
        }

        public void WaitForPinDisplay(int msTimeout = 3000)
        {
            ThrowIfDisposed();
            if (!pinDisplayedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for pin display");
            }
        }

        public void WaitForPinEntry(int msTimeout = 3000)
        {
            ThrowIfDisposed();
            if (!pinEnteredEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for pin entry");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        public void ResetState()
        {
            foreach (var advertiser in advertiserCollection.Values.ToList())
            {
                advertiser.Dispose();
            }
            advertiserCollection.Clear();

            foreach (var discovery in discoveryCollection.Values.ToList())
            {
                discovery.Dispose();
            }
            discoveryCollection.Clear();

            foreach (var session in sessionCollection.Values.ToList())
            {
                session.Dispose();
            }
            sessionCollection.Clear();

            watcher.ResetState();

            // Reset old errors
            error = null;
            ClearPin();
        }

        ~WiFiDirectServicesManager()
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

                watcher.Dispose();

                pinDisplayedEvent.Dispose();
                pinEnteredEvent.Dispose();
            }
            
            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("WiFiDirectServicesManager");
            }
        }
        #endregion
    }
}