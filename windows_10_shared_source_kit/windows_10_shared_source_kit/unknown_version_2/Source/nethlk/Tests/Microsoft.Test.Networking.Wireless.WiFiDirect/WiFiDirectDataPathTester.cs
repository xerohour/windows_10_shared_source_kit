///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    class WiFiDirectDataPathTester : IDisposable
    {
        private Object criticalSection = new Object();

        private HostName expectedRemoteHost;
        private StreamSocketListener streamSocketListener;

        private StreamSocket streamSocket;

        private ManualResetEvent connectionReceivedCallbackCallbackEvent = new ManualResetEvent(false);

        private Guid guidInternal;
        public Guid Guid
        {
            get
            {
                lock(this.criticalSection)
                {
                    return this.guidInternal;
                }
            }
        }

        public void Dispose()
        {
            Reset();
            connectionReceivedCallbackCallbackEvent.Dispose();
            GC.SuppressFinalize(this);
        }

        public void Reset()
        {
            lock(this.criticalSection)
            {
                if (streamSocketListener != null)
                {
                    streamSocketListener.Dispose();
                    streamSocketListener = null;
                }

                if (streamSocket != null)
                {
                    streamSocket.Dispose();
                    streamSocket = null;
                }

                this.guidInternal = Guid.Empty;
                connectionReceivedCallbackCallbackEvent.Reset();
            }
        }

        public void StartConnectionListener(string localAddress, string remoteAddress, string localPort)
        {
            if (localAddress == null)
            {
                throw new ArgumentNullException("localAddress");
            }

            if (remoteAddress == null)
            {
                throw new ArgumentNullException("remoteAddress");
            }

            if (localPort == null)
            {
                throw new ArgumentNullException("localPort");
            }

            lock(this.criticalSection)
            {
                if (this.streamSocketListener != null)
                {
                    throw new InvalidOperationException("The stream socket listener is already running");
                }

                if (this.streamSocket != null)
                {
                    throw new InvalidOperationException("A stream socket is already connected");
                }

                WiFiDirectTestLogger.Log("Listening for incoming TCP connection.  Local Address = {0}, Remote Address = {1}, Local Port={2}", localAddress, remoteAddress, localPort);

                this.expectedRemoteHost = new HostName(remoteAddress);

                StreamSocketListener streamSocketListener = new StreamSocketListener();
                try
                {
                    streamSocketListener.ConnectionReceived += this.ConnectionReceived;
                    streamSocketListener.BindEndpointAsync(new HostName(localAddress), localPort).AsTask().Wait();

                    this.streamSocketListener = streamSocketListener;
                    streamSocketListener = null;
                }
                finally
                {
                    if (streamSocketListener != null)
                    {
                        streamSocketListener.Dispose();
                    }
                }
            }
        }

        public void ConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
        {
            try
            {
                lock(this.criticalSection)
                {
                    StreamSocket streamSocket = args.Socket;
                    try
                    {
                        if (streamSocket.Information.RemoteAddress.IsEqual(this.expectedRemoteHost))
                        {
                            WiFiDirectTestLogger.Log("Connection received from {0}", streamSocket.Information.RemoteAddress);

                            this.streamSocketListener.Dispose();
                            this.streamSocketListener = null;

                            this.streamSocket = streamSocket;
                            streamSocket = null;
                        }
                        else
                        {
                            WiFiDirectTestLogger.Log("Closing unexpected connection received from {0}", streamSocket.Information.RemoteAddress);
                        }
                    }
                    finally
                    {
                        if (streamSocket != null)
                        {
                            streamSocket.Dispose();
                        }
                    }

                    if (this.streamSocket == null)
                    {
                        return;
                    }

                    using (DataReader dataReader = new DataReader(this.streamSocket.InputStream))
                    {
                        WiFiDirectTestLogger.Log("Waiting to receive GUID value {0}", this.streamSocket.Information.RemoteAddress);
                        Task readTask = dataReader.LoadAsync(16U).AsTask();
                        bool readComplete = readTask.Wait(new TimeSpan(0, 0, 1));
                        if (readComplete)
                        {
                            this.guidInternal = dataReader.ReadGuid();
                            WiFiDirectTestLogger.Log("Received GUID value {0}", this.guidInternal);
                        }
                        else
                        {
                            WiFiDirectTestLogger.Log("Timed out waiting for GUID value");
                        }
                    }
                }

                connectionReceivedCallbackCallbackEvent.Set();
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while handling connection received event. {0}", e);
                throw;
            }
        }

        public void WaitForConnectionReceivedCallback()
        {
            // Wait one second for the callback to complete.  Throw a timeout exception if 
            bool signalled = connectionReceivedCallbackCallbackEvent.WaitOne(1000);
            if (!signalled)
            {
                throw new TimeoutException("Timed out waiting for connection received callback to complete.");
            }
        }

        public void ConnectAndSendGuid(string localAddress, string remoteAddress, string remotePort)
        {
            if (localAddress == null)
            {
                throw new ArgumentNullException("localAddress");
            }

            if (remoteAddress == null)
            {
                throw new ArgumentNullException("remoteAddress");
            }

            if (remotePort == null)
            {
                throw new ArgumentNullException("remotePort");
            }

            lock(this.criticalSection)
            {
                if (this.streamSocketListener != null)
                {
                    throw new InvalidOperationException("The stream socket listener is running.");
                }

                if (this.streamSocket != null)
                {
                    throw new InvalidOperationException("A stream socket is already connected.");
                }

                WiFiDirectTestLogger.Log("Establishing TCP connection.  Local Address = {0}, Remote Address = {1}, Remote Port={2}", localAddress, remoteAddress, remotePort);

                StreamSocket streamSocket = new StreamSocket();
                try
                {
                    streamSocket.ConnectAsync(
                        new EndpointPair( 
                            new HostName(localAddress),
                            "",
                            new HostName(remoteAddress),
                            remotePort
                            )
                        ).AsTask().Wait();

                    this.streamSocket = streamSocket;
                    streamSocket = null;
                }
                finally
                {
                    if (streamSocket != null)
                    {
                        streamSocket.Dispose();
                    }
                }

                WiFiDirectTestLogger.Log("TCP connection successful.");

                this.guidInternal = Guid.NewGuid();

                using (DataWriter dataWriter = new DataWriter(this.streamSocket.OutputStream))
                {
                    WiFiDirectTestLogger.Log("Transmitting GUID value: {0}", this.guidInternal);
                    dataWriter.WriteGuid(this.guidInternal);
                    dataWriter.StoreAsync().AsTask().Wait();
                }
            }
        }
    }
}
