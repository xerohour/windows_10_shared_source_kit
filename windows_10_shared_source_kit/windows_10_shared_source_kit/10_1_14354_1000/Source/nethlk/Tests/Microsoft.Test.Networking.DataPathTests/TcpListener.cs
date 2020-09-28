///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;

namespace HlkTest.DataPathTests
{

    internal class TcpSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return true;
            }
        }

        private string remoteAddress;
        private UInt16 remotePort;
        private string localAddress;
        private UInt16 localPort;
        private bool running;
        private bool ipv6Mode;
        private TimeSpan logInterval;
        private Task sendTask;
        private CancellationTokenSource cancelToken;
        private Sockets sockets;
        private WlanHckTestLogger testLogger;
        private string identifier;

        public TcpSender(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            logInterval = new TimeSpan(0, 0, 10);
            UnitsTransfered = 0;
            cancelToken = new CancellationTokenSource();
            sendTask = new Task(SendThread, this.cancelToken.Token);
            running = false;
            sockets = new Sockets(testLogger);
            identifier = String.Empty;
        }

        ~TcpSender()
        {

            Dispose(false);
        }

        public void Dispose()
        {
            testLogger.LogTrace("TcpSender[{0}]  Dispose", this.identifier);
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2213:DisposableFieldsShouldBeDisposed", Justification = "Task.Dispose does not exist on coreCLR")]

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                Stop();
                cancelToken.Dispose();
                sockets.Dispose();
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {

            this.remoteAddress = address;
            this.remotePort = port;
            this.ipv6Mode = ipv6;
            localPort = port;

            if (testMode == TestMode.Wlan)
            {
                localAddress = NetworkInterfaceDataPathTests.GetWirelessEndpoint(ipv6Mode);
            }
            else
            {
                localAddress = NetworkInterfaceDataPathTests.GetLanEndpoint(ipv6Mode, localInterfaceGuid);
            }
            this.identifier = String.Format(CultureInfo.InvariantCulture, "{0}:{1}", this.localAddress, this.localPort);

            testLogger.LogComment("TcpSender[{0}] remote address {1}:{2}", this.identifier, this.remoteAddress, this.remotePort);

        }
        public void Start()
        {
            testLogger.LogComment("TcpSender[{0}]  Start", this.identifier);
            running = true;
            sendTask.Start();
        }
        public void Stop()
        {
            testLogger.LogTrace("TcpSender[{0}]  Stop", this.identifier);
            if (running)
            {
                cancelToken.Cancel();
                running = false;
                testLogger.LogTrace("TcpSender[{0}] Waiting For Send Thread", this.identifier);
                sendTask.Wait(new TimeSpan(0, 0, 30));
                testLogger.LogTrace("TcpSender[{0}] Stopping Complete", this.identifier);
            }
        }

        private void SendThread(object obj)
        {
            CancellationToken token = (CancellationToken)obj;

            IntPtr socket = IntPtr.Zero;
            try
            {
                testLogger.LogComment("TCP Sends from {0}:{1} to {2}:{3}", localAddress, localPort, remoteAddress, remotePort);
                socket = sockets.CreateTcpSocket(localAddress, localPort, ipv6Mode);
                sockets.Connect(socket, remoteAddress, remotePort, ipv6Mode);
                testLogger.LogComment("Connected to TCP Server at {0}:{1}", remoteAddress, remotePort);

                Byte[] sendData;
                while (!token.IsCancellationRequested)
                {
                    sendData = NetworkInterfaceDataPathTests.GeneratePayload(1000);
                    testLogger.LogTrace("TcpSender[{0}]  Sending Data", this.identifier);
                    sockets.Send(socket, sendData);
                    UnitsTransfered += sendData.Length;
                    Wlan.Sleep(NetworkInterfaceDataPathTests.RandomWaitTime());
                    DateTime nextLogTime = DateTime.Now;
                    if (DateTime.Now > nextLogTime)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sending TCP Data to {0}:{1}.  Bytes Sent {2}", remoteAddress, remotePort, UnitsTransfered));
                        nextLogTime = DateTime.Now.Add(logInterval);

                    }
                    testLogger.LogTrace("TcpSender[{0}]  Total Bytes Sent: {1}", this.identifier, UnitsTransfered);

                }

                testLogger.LogComment("TCP Send Completed from {0}:{1} to {2}:{3} Bytes Count = {4}", 
					localAddress, localPort, remoteAddress, remotePort, UnitsTransfered);

            }
            catch (Exception error)
            {
                testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Error when sending TCP Data from {0}:{1} to {2}:{3} : {4}", localAddress, localPort, remoteAddress, remotePort, error.ToString()));
                throw;
            }
            finally
            {
                if (socket != IntPtr.Zero)
                {
                    testLogger.LogTrace("TcpSender[{0}] Closing Send Socket", this.identifier);
                    sockets.CloseSocket(socket);
                }
            }
        }
    }

    internal class TcpReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return true;
            }
        }
        private string localAddress;
        private UInt16 localPort;
        private Sockets sockets;
        private IntPtr listenSocket;
        private bool ipv6Mode;
        private WlanHckTestLogger testLogger;
        private string identifier;


        public TcpReceiver(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            this.identifier = String.Empty;
            sockets = new Sockets(testLogger);
        }

        ~TcpReceiver()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            testLogger.LogTrace("TcpReceiver[{0}]  Dispose", this.identifier);
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (listenSocket != IntPtr.Zero)
                {
                    testLogger.LogTrace("TcpReceiver[{0}]  Closing Receive Socket", this.identifier);
                    sockets.CloseSocket(listenSocket);
                }
                sockets.Dispose();
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {

            this.ipv6Mode = ipv6;
            localPort = port;

            if (testMode == TestMode.Wlan)
            {
                localAddress = NetworkInterfaceDataPathTests.GetWirelessEndpoint(ipv6Mode);
            }
            else
            {
                localAddress = NetworkInterfaceDataPathTests.GetLanEndpoint(ipv6Mode, localInterfaceGuid);
            }

            this.identifier = String.Format(CultureInfo.InvariantCulture, "{0}:{1}", localAddress, localPort);
            testLogger.LogComment("TcpReceiver[{0}] local address", this.identifier);

            listenSocket = sockets.CreateTcpSocket(localAddress, localPort, ipv6Mode);


        }

        public void Start()
        {
            testLogger.LogComment("TcpReceiver[{0}] Start", this.identifier);
            sockets.StartReceiveTcp(listenSocket, ipv6Mode);

        }
        public void Stop()
        {
            testLogger.LogTrace("TcpReceiver[{0}]  Stop", this.identifier);
            UnitsTransfered = sockets.StopReceiveTcp(listenSocket);

            testLogger.LogComment("TcpReceiver[{0}]  Stopped. Total Bytes Received: {1}", this.identifier, UnitsTransfered);
        }
    }


}
