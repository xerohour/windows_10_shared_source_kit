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


    internal class UdpSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
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

        public UdpSender(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            this.identifier = String.Empty;
            logInterval = new TimeSpan(0, 0, 10);
            UnitsTransfered = 0;
            cancelToken = new CancellationTokenSource();
            sendTask = new Task(SendThread, this.cancelToken.Token);
            running = false;
            sockets = new Sockets(testLogger);
        }

        ~UdpSender()
        {

            Dispose(false);
        }

        public void Dispose()
        {
            testLogger.LogTrace("UdpSender[{0}]  Dispose", this.identifier);
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

            this.identifier = String.Format(CultureInfo.InvariantCulture, "{0}:{1}", localAddress, localPort);
            testLogger.LogComment("UdpSender[{0}]  remote: {1}:{2}", this.identifier, this.remoteAddress, this.remotePort);
        }
        public void Start()
        {
            testLogger.LogComment("UdpSender[{0}] Start", this.identifier);
            running = true;
            sendTask.Start();
        }
        public void Stop()
        {
            testLogger.LogTrace("UdpSender[{0}] Stop", this.identifier);
            if (running)
            {
                
                cancelToken.Cancel();
                running = false;
                testLogger.LogTrace("UdpSender[{0}] Waiting For Send Thread", this.identifier);
                sendTask.Wait(new TimeSpan(0, 0, 30));
                testLogger.LogTrace("UdpSender[{0}] Stopping complete", this.identifier);
            }
        }

        private void SendThread(object obj)
        {
            CancellationToken token = (CancellationToken)obj;

            IntPtr socket = IntPtr.Zero;
            try
            {
                testLogger.LogComment("UDP Sends from {0}:{1} to {2}:{3}", localAddress, localPort, remoteAddress, remotePort);
                socket = sockets.CreateUdpSocket(localAddress, localPort, ipv6Mode);

                Byte[] sendData;
                while (!token.IsCancellationRequested)
                {
                    sendData = NetworkInterfaceDataPathTests.GeneratePayload(100);
                    testLogger.LogTrace("UdpSender[{0}] Sending Packet", this.identifier);
                    sockets.SendTo(socket, sendData, remoteAddress, remotePort, ipv6Mode);
                    UnitsTransfered++;
                    Wlan.Sleep(NetworkInterfaceDataPathTests.RandomWaitTime());
                    DateTime nextLogTime = DateTime.Now;
                    if (DateTime.Now > nextLogTime)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sending UDP Data to {0}:{1}.  Packets Sent {2}", remoteAddress, remotePort, UnitsTransfered));
                        nextLogTime = DateTime.Now.Add(logInterval);
                    }
                    testLogger.LogTrace("UdpSender[{0}] Packet Count {1}", this.identifier, UnitsTransfered);

                }
                testLogger.LogComment("UDP Send Completed from {0}:{1} to {2}:{3} Packet Count = {4}", 
					localAddress, localPort, remoteAddress, remotePort, UnitsTransfered);

            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
            finally
            {
                if (socket != IntPtr.Zero)
                {
                    testLogger.LogTrace("UdpSender[{0}] Closing Send Socket", this.identifier);
                    sockets.CloseSocket(socket);
                }
            }
        }
    }

    internal class UdpReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }
        private string localAddress;
        private UInt16 localPort;
        private Sockets sockets;
        private IntPtr listenSocket;
        private bool ipv6Mode;
        private WlanHckTestLogger testLogger;
        private string identifier;

        public UdpReceiver(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            this.identifier = String.Empty;
            sockets = new Sockets(testLogger);
        }

        ~UdpReceiver()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (listenSocket != IntPtr.Zero)
                {
                    testLogger.LogTrace("UdpReceiver[{0}] Closing Receive Socket", this.identifier);
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
            testLogger.LogComment("UdpReceiver[{0}]  local address", this.identifier);

            listenSocket = sockets.CreateUdpSocket(localAddress, localPort, ipv6Mode);


        }

        public void Start()
        {
            testLogger.LogComment("UdpReceiver[{0}] Start", this.identifier);
            sockets.StartReceiveUdp(listenSocket);

        }
        public void Stop()
        {
            testLogger.LogTrace("UdpReceiver[{0}] Stop", this.identifier);
            UnitsTransfered = sockets.StopReceiveUdp(listenSocket, false);
            testLogger.LogComment("UdpReceiver[{0}] Stopped. Total Packet Count {1}", this.identifier, UnitsTransfered);
        }
    }


}
