///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using WEX.TestExecution;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;

namespace HlkTest.DataPathTests
{


    internal class MulticastSender : IDataPathProtocolTest
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
        private TestMode testMode;
        private Guid localInterfaceGuid;
        private string identifier;

        public MulticastSender(WlanHckTestLogger testLogger)
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

        ~MulticastSender()
        {

            Dispose(false);
        }

        public void Dispose()
        {
            testLogger.LogTrace("MulticastSender[{0}]  Dispose", this.identifier);
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2213:DisposableFieldsShouldBeDisposed", Justification = "Task.Dispose does not exist on coreCLR")]

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                Stop();
                //sendTask.Dispose();
                cancelToken.Dispose();
                sockets.Dispose();
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            this.testMode = testMode;
            this.localInterfaceGuid = localInterfaceGuid;
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

            testLogger.LogComment("MulticastSender[{0}]  remote {1}:{2}", this.identifier, this.remoteAddress, this.remotePort);

        }
        public void Start()
        {
            testLogger.LogComment("MulticastSender[{0}]   Start", this.identifier);
            running = true;
            sendTask.Start();
        }
        public void Stop()
        {
            testLogger.LogTrace("MulticastSender[{0}]   Stop", this.identifier);
            if (running)
            {
                cancelToken.Cancel();
                running = false;
                testLogger.LogTrace("MulticastSender[{0}] Waiting For Send Thread", this.identifier);
                sendTask.Wait(new TimeSpan(0, 0, 30));
                testLogger.LogTrace("MulticastSender[{0}]   Stopping Complete", this.identifier);
            }
        }

        private void SendThread(object obj)
        {
            CancellationToken token = (CancellationToken)obj;

            IntPtr socket = IntPtr.Zero;
            try
            {
                testLogger.LogComment("Multicast Sends from {0}:{1} to {2}:{3}", localAddress, localPort, remoteAddress, remotePort);
                if (ipv6Mode)
                {
                    if (testMode == TestMode.Wlan)
                    {
                        using (Wlan wlanApi = new Wlan())
                        {
                            var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                            if (wlanInterfaceList.Count < 1)
                            {
                                throw new TestConfigException("No WLAN Interfaces were discovered.  Ensure that WLAN interfaces are enabled, discoverable, and operational.");
							}
                            var wlanInterface = wlanInterfaceList[0];

                            UInt32 wlanInterfaceIndex = NetworkInterfaceDataPathTests.GetNetworkIndex(wlanInterface.Id);
                            socket = sockets.CreateMulticastSocket(localAddress, localPort, remoteAddress, remotePort, wlanInterfaceIndex);
                        }
                    }
                    else
                    {
                        UInt32 lanInterfaceIndex = NetworkInterfaceDataPathTests.GetNetworkIndex(localInterfaceGuid);
                        socket = sockets.CreateMulticastSocket(localAddress, localPort, remoteAddress, remotePort, lanInterfaceIndex);
                    }

                }
                else
                {
                    socket = sockets.CreateMulticastSocket(localAddress, localPort, remoteAddress, remotePort);
                }

                Byte[] sendData;
                while (!token.IsCancellationRequested)
                {
                    sendData = NetworkInterfaceDataPathTests.GeneratePayload(100);
                    testLogger.LogTrace("MulticastSender[{0}]   Sending Packet", this.identifier);
                    sockets.SendTo(socket, sendData, remoteAddress, remotePort, ipv6Mode);
                    UnitsTransfered++;
                    Wlan.Sleep(NetworkInterfaceDataPathTests.RandomWaitTime());
                    DateTime nextLogTime = DateTime.Now;
                    if (DateTime.Now > nextLogTime)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sending Multicast Data to {0}:{1}.  Packets Sent {2}", remoteAddress, remotePort, UnitsTransfered));
                        nextLogTime = DateTime.Now.Add(logInterval);

                    }

                    testLogger.LogTrace("MulticastSender[{0}]   Packets Sent {1}", this.identifier, UnitsTransfered);

                }

                testLogger.LogComment("Multicast Send Completed from {0}:{1} to {2}:{3}. Packet Count = {4}", 
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
                    testLogger.LogTrace("MulticastSender[{0}] Closing Send Socket", this.identifier);
                    sockets.CloseSocket(socket);
                }
            }
        }
    }

    internal class MulticastReceiver : IDataPathProtocolTest
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

        public MulticastReceiver(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            this.identifier = String.Empty;
            sockets = new Sockets(testLogger);
        }

        ~MulticastReceiver()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            testLogger.LogTrace("MulticastReceiver[{0}]   Dispose", this.identifier);
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (listenSocket != IntPtr.Zero)
                {
                    testLogger.LogTrace("MulticastReceiver[{0}] Closing Receive Socket", this.identifier);
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
            testLogger.LogComment("MulticastReceiver[{0}]  local address", this.identifier);


            testLogger.LogTrace("Creating Multicast Receive Socket");
            if (ipv6Mode)
            {
                if (testMode == TestMode.Wlan)
                {
                    using (Wlan wlanApi = new Wlan())
                    {
                        var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                        if (wlanInterfaceList.Count < 1)
                        {
                            throw new TestConfigException("No WLAN Interfaces were discovered.  Ensure that WLAN interfaces are enabled, discoverable, and operational.");
						}
                        var wlanInterface = wlanInterfaceList[0];

                        UInt32 wlanInterfaceIndex = NetworkInterfaceDataPathTests.GetNetworkIndex(wlanInterface.Id);
                        listenSocket = sockets.CreateMulticastSocket(localAddress, localPort, address, port, wlanInterfaceIndex);
                    }
                }
                else
                {
                    UInt32 lanInterfaceIndex = NetworkInterfaceDataPathTests.GetNetworkIndex(localInterfaceGuid);
                    listenSocket = sockets.CreateMulticastSocket(localAddress, localPort, address, port, lanInterfaceIndex);
                }

            }
            else
            {
                listenSocket = sockets.CreateMulticastSocket(localAddress, localPort, address, port);
            }


        }

        public void Start()
        {
            testLogger.LogComment("MulticastReceiver[{0}] Start", this.identifier);
            sockets.StartReceiveUdp(listenSocket);

        }
        public void Stop()
        {
            testLogger.LogTrace("MulticastReceiver[{0}]  Stop", this.identifier);
            UnitsTransfered = sockets.StopReceiveUdp(listenSocket, false);
            testLogger.LogComment("MulticastReceiver[{0}]  Stopped. Total Packets Received {1}", this.identifier, UnitsTransfered);
        }
    }


}
