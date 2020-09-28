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
}#include "Test_Include.fx"

int Test_Count = 219;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_x_inst_dst";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_x : dst source reg combination v0, r0 is allowed";
string VS_001 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"dst r0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_x : dst source reg combination v0, c0 is allowed";
string VS_002 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"dst r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_x : dst source reg combination r0, v0 is allowed";
string VS_003 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"dst r0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_x : dst source reg combination r0, r1 is allowed";
string VS_004 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_x : dst source reg combination r0, c0 is allowed";
string VS_005 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dst r0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_x : dst source reg combination c0, v0 is allowed";
string VS_006 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"dst r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_x : dst source reg combination c0, r0 is allowed";
string VS_007 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dst r0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_x : dst source reg combination -r0, -r1 is allowed";
string VS_008 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, -r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_x : dst source reg combination -r0, r1 is allowed";
string VS_009 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, -r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_x : dst source reg combination r0, -r1 is allowed";
string VS_010 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_x : dst source reg combination r0, r1 is allowed";
string VS_011 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_2_x : dst source reg combination r0.x, r1 is allowed";
string VS_012 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.x, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_2_x : dst source reg combination r0.y, r1 is allowed";
string VS_013 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.y, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_2_x : dst source reg combination r0.z, r1 is allowed";
string VS_014 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.z, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_2_x : dst source reg combination r0.w, r1 is allowed";
string VS_015 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.w, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_2_x : dst source reg combination r0.yzxw, r1 is allowed";
string VS_016 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.yzxw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_2_x : dst source reg combination r0.zxyw, r1 is allowed";
string VS_017 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.zxyw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_2_x : dst source reg combination r0.wzyx, r1 is allowed";
string VS_018 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.wzyx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_2_x : dst source reg combination r0.wyxz, r1 is allowed";
string VS_019 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.wyxz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_x : dst source reg combination r0.xzyw, r1 is allowed";
string VS_020 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.xzyw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_x : dst source reg combination r0.xywz, r1 is allowed";
string VS_021 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.xywz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_x : dst source reg combination r0.zyx, r1 is allowed";
string VS_022 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.zyx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_x : dst source reg combination r0.xzy, r1 is allowed";
string VS_023 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.xzy, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_x : dst source reg combination r0.ywx, r1 is allowed";
string VS_024 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.ywx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_x : dst source reg combination r0.yx, r1 is allowed";
string VS_025 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.yx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_x : dst source reg combination r0.wz, r1 is allowed";
string VS_026 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.wz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_x : dst source reg combination r0.zy, r1 is allowed";
string VS_027 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0.zy, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_x : dst source reg combination r0, r1.x is allowed";
string VS_028 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_x : dst source reg combination r0, r1.y is allowed";
string VS_029 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_x : dst source reg combination r0, r1.z is allowed";
string VS_030 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_x : dst source reg combination r0, r1.w is allowed";
string VS_031 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_x : dst source reg combination r0, r1.yzxw is allowed";
string VS_032 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_x : dst source reg combination r0, r1.zxyw is allowed";
string VS_033 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_x : dst source reg combination r0, r1.wzyx is allowed";
string VS_034 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_x : dst source reg combination r0, r1.wyxz is allowed";
string VS_035 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_x : dst source reg combination r0, r1.xzyw is allowed";
string VS_036 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_x : dst source reg combination r0, r1.xywz is allowed";
string VS_037 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_x : dst source reg combination r0, r1.zyx is allowed";
string VS_038 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_x : dst source reg combination r0, r1.xzy is allowed";
string VS_039 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_x : dst source reg combination r0, r1.ywx is allowed";
string VS_040 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_x : dst source reg combination r0, r1.yx is allowed";
string VS_041 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_x : dst source reg combination r0, r1.wz is allowed";
string VS_042 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_x : dst source reg combination r0, r1.zy is allowed";
string VS_043 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"dst r0, r0, r1.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_x : dst source reg combination v0, v1 is NOT allowed";
string VS_044 = 
	"vs_2_x "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dst r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_x : dst source reg combination c0, c1 is NOT allowed";
string VS_045 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dst r0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_x : dst source reg combination a0, a0 is NOT allowed";
string VS_046 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_x : dst source reg combination a0, b0 is NOT allowed";
string VS_047 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"dst r0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_x : dst source reg combination a0, i0 is NOT allowed";
string VS_048 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"dst r0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_x : dst source reg combination a0, aL is NOT allowed";
string VS_049 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"dst r0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_050_Desc = "vs_2_x : dst source reg combination a0, p0 is NOT allowed";
string VS_050 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_x : dst source reg combination a0, oPos is NOT allowed";
string VS_051 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_x : dst source reg combination a0, oFog is NOT allowed";
string VS_052 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_x : dst source reg combination a0, oPts is NOT allowed";
string VS_053 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"dst r0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_x : dst source reg combination a0, oD0 is NOT allowed";
string VS_054 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_x : dst source reg combination a0, oT0 is NOT allowed";
string VS_055 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_x : dst source reg combination b0, a0 is NOT allowed";
string VS_056 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"dst r0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_x : dst source reg combination b0, b0 is NOT allowed";
string VS_057 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_x : dst source reg combination b0, i0 is NOT allowed";
string VS_058 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dst r0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_x : dst source reg combination b0, aL is NOT allowed";
string VS_059 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"dst r0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_060_Desc = "vs_2_x : dst source reg combination b0, p0 is NOT allowed";
string VS_060 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_x : dst source reg combination b0, oPos is NOT allowed";
string VS_061 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_x : dst source reg combination b0, oFog is NOT allowed";
string VS_062 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_x : dst source reg combination b0, oPts is NOT allowed";
string VS_063 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"dst r0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_x : dst source reg combination b0, oD0 is NOT allowed";
string VS_064 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_x : dst source reg combination b0, oT0 is NOT allowed";
string VS_065 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_x : dst source reg combination i0, a0 is NOT allowed";
string VS_066 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"dst r0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_x : dst source reg combination i0, b0 is NOT allowed";
string VS_067 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_x : dst source reg combination i0, i0 is NOT allowed";
string VS_068 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_x : dst source reg combination i0, aL is NOT allowed";
string VS_069 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"dst r0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_070_Desc = "vs_2_x : dst source reg combination i0, p0 is NOT allowed";
string VS_070 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_x : dst source reg combination i0, oPos is NOT allowed";
string VS_071 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_x : dst source reg combination i0, oFog is NOT allowed";
string VS_072 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_x : dst source reg combination i0, oPts is NOT allowed";
string VS_073 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"dst r0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_x : dst source reg combination i0, oD0 is NOT allowed";
string VS_074 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_x : dst source reg combination i0, oT0 is NOT allowed";
string VS_075 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_x : dst source reg combination aL, a0 is NOT allowed";
string VS_076 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"dst r0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_x : dst source reg combination aL, b0 is NOT allowed";
string VS_077 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"dst r0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_x : dst source reg combination aL, i0 is NOT allowed";
string VS_078 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"dst r0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_x : dst source reg combination aL, aL is NOT allowed";
string VS_079 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_080_Desc = "vs_2_x : dst source reg combination aL, p0 is NOT allowed";
string VS_080 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_x : dst source reg combination aL, oPos is NOT allowed";
string VS_081 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_x : dst source reg combination aL, oFog is NOT allowed";
string VS_082 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_x : dst source reg combination aL, oPts is NOT allowed";
string VS_083 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"dst r0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_x : dst source reg combination aL, oD0 is NOT allowed";
string VS_084 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_x : dst source reg combination aL, oT0 is NOT allowed";
string VS_085 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_086_Desc = "vs_2_x : dst source reg combination p0, a0 is NOT allowed";
string VS_086 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_087_Desc = "vs_2_x : dst source reg combination p0, b0 is NOT allowed";
string VS_087 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_088_Desc = "vs_2_x : dst source reg combination p0, i0 is NOT allowed";
string VS_088 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_089_Desc = "vs_2_x : dst source reg combination p0, aL is NOT allowed";
string VS_089 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_090_Desc = "vs_2_x : dst source reg combination p0, p0 is NOT allowed";
string VS_090 = 
	"vs_2_x "
	"dst r0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_091_Desc = "vs_2_x : dst source reg combination p0, oPos is NOT allowed";
string VS_091 = 
	"vs_2_x "
	"dst r0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_092_Desc = "vs_2_x : dst source reg combination p0, oFog is NOT allowed";
string VS_092 = 
	"vs_2_x "
	"dst r0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_093_Desc = "vs_2_x : dst source reg combination p0, oPts is NOT allowed";
string VS_093 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_094_Desc = "vs_2_x : dst source reg combination p0, oD0 is NOT allowed";
string VS_094 = 
	"vs_2_x "
	"dst r0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_095_Desc = "vs_2_x : dst source reg combination p0, oT0 is NOT allowed";
string VS_095 = 
	"vs_2_x "
	"dst r0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_x : dst source reg combination oPos, a0 is NOT allowed";
string VS_096 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_x : dst source reg combination oPos, b0 is NOT allowed";
string VS_097 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_x : dst source reg combination oPos, i0 is NOT allowed";
string VS_098 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_x : dst source reg combination oPos, aL is NOT allowed";
string VS_099 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_100_Desc = "vs_2_x : dst source reg combination oPos, p0 is NOT allowed";
string VS_100 = 
	"vs_2_x "
	"dst r0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_x : dst source reg combination oPos, oPos is NOT allowed";
string VS_101 = 
	"vs_2_x "
	"dst r0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_x : dst source reg combination oPos, oFog is NOT allowed";
string VS_102 = 
	"vs_2_x "
	"dst r0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_x : dst source reg combination oPos, oPts is NOT allowed";
string VS_103 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_x : dst source reg combination oPos, oD0 is NOT allowed";
string VS_104 = 
	"vs_2_x "
	"dst r0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_x : dst source reg combination oPos, oT0 is NOT allowed";
string VS_105 = 
	"vs_2_x "
	"dst r0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_x : dst source reg combination oFog, a0 is NOT allowed";
string VS_106 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_x : dst source reg combination oFog, b0 is NOT allowed";
string VS_107 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_x : dst source reg combination oFog, i0 is NOT allowed";
string VS_108 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_x : dst source reg combination oFog, aL is NOT allowed";
string VS_109 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_110_Desc = "vs_2_x : dst source reg combination oFog, p0 is NOT allowed";
string VS_110 = 
	"vs_2_x "
	"dst r0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_x : dst source reg combination oFog, oPos is NOT allowed";
string VS_111 = 
	"vs_2_x "
	"dst r0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_x : dst source reg combination oFog, oFog is NOT allowed";
string VS_112 = 
	"vs_2_x "
	"dst r0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_x : dst source reg combination oFog, oPts is NOT allowed";
string VS_113 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_x : dst source reg combination oFog, oD0 is NOT allowed";
string VS_114 = 
	"vs_2_x "
	"dst r0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_x : dst source reg combination oFog, oT0 is NOT allowed";
string VS_115 = 
	"vs_2_x "
	"dst r0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_x : dst source reg combination oPts, a0 is NOT allowed";
string VS_116 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"dst r0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_x : dst source reg combination oPts, b0 is NOT allowed";
string VS_117 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"dst r0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_x : dst source reg combination oPts, i0 is NOT allowed";
string VS_118 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"dst r0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_2_x : dst source reg combination oPts, aL is NOT allowed";
string VS_119 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"dst r0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_120_Desc = "vs_2_x : dst source reg combination oPts, p0 is NOT allowed";
string VS_120 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_x : dst source reg combination oPts, oPos is NOT allowed";
string VS_121 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_x : dst source reg combination oPts, oFog is NOT allowed";
string VS_122 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_x : dst source reg combination oPts, oPts is NOT allowed";
string VS_123 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_x : dst source reg combination oPts, oD0 is NOT allowed";
string VS_124 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_2_x : dst source reg combination oPts, oT0 is NOT allowed";
string VS_125 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_2_x : dst source reg combination oD0, a0 is NOT allowed";
string VS_126 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_2_x : dst source reg combination oD0, b0 is NOT allowed";
string VS_127 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_2_x : dst source reg combination oD0, i0 is NOT allowed";
string VS_128 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
str