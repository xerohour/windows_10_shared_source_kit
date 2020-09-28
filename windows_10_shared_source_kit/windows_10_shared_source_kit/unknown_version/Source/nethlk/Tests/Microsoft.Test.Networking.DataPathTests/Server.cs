///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Globalization;
using WEX.TestExecution.Markup;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.RemoteControl;
using Microsoft.Test.Networking.RemoteControl.StreamSocketTransport;
using Microsoft.Test.Networking.Wireless;

namespace HlkTest.DataPathTests
{
    [TestClass]
    class Server
    {
        [TestMethod]
        public void StartServer()
        {
            Logger.SetAdditionalLogger(DataPathServer.testLogger.LogTrace);
            RemoteCommandServer server = new RemoteCommandServer(new Type[]
            {
                typeof(DataPathServer)
            },
            new object[]
            {
                new DataPathServer()
            });
            server.Run();
        }
    }

    public class DataPathServer
    {
        private Dictionary<Guid, IDataPathProtocolTest> testList;
        private Object lockObject;

        internal static WlanHckTestLogger testLogger = new WlanHckTestLogger("DataPathServer");
       

        public DataPathServer()
        {
            lockObject = new Object();
            testList = new Dictionary<Guid, IDataPathProtocolTest>();

            testLogger.LogComment("For additional tracing use the following GUID: {0}", testLogger.TraceGuid.ToString());
        }

        public bool CreateProfile(string profile, string profileName)
        {
            try
            {
                using (Wlan wlanApi = new Wlan())
                {
                    testLogger.LogTrace("CreateProfile");
                    var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                    if (NetworkInterfaceDataPathTests.CheckWlanInterfaceCount(wlanApi) == false)
                    {
                        return false;
                    }
                    var wlanInterface = wlanInterfaceList[0];

                    testLogger.LogComment("Creating Profile {0}", profileName);
                    wlanApi.CreateProfile(wlanInterface.Id, profile);

                    return true;
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while Creating the Wlan Profile {0}", profileName);
                testLogger.LogError(error.ToString());
                return false;
            }

        }

        public bool ConnectToNetwork(string ssid)
        {
            try
            {
                using (Wlan wlanApi = new Wlan())
                {
                    testLogger.LogComment("ConnectToNetwork");
                    var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                    if (NetworkInterfaceDataPathTests.CheckWlanInterfaceCount(wlanApi) == false)
                    {
                        return false;
                    }
                    var wlanInterface = wlanInterfaceList[0];

                    bool localConnectionNeeded = true;

                    if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                    {
                        if (String.Compare(ssid, wlanInterface.Ssid, StringComparison.OrdinalIgnoreCase) == 0)
                        {
                            testLogger.LogComment("We already connected to the correct network.");
                            localConnectionNeeded = false;
                        }
                        else
                        {
                            testLogger.LogComment("We already connected a different network.  Disconnecting.");
                            wlanApi.Disconnect(wlanInterface.Id, new TimeSpan(0, 0, 10));
                            Wlan.Sleep(1000);
                        }
                    }

                    if (localConnectionNeeded)
                    {
                        testLogger.LogComment("Connecting to SSID {0}", ssid);
                        wlanApi.TryScan(wlanInterface.Id, false, new TimeSpan(0, 0, 10));
                        wlanApi.ProfileConnect(wlanInterface.Id, ssid, new TimeSpan(0, 0, 30));
                        testLogger.LogComment("Connection Completed {0}", ssid);

                        testLogger.LogComment("Waiting for DHCP to be stable");
                        Microsoft.Test.Networking.Kit.Helpers.WaitForWlanConnectivity(wlanInterface.Id, 1000 * 15, testLogger);
                    }
                    return true;
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while Connecting to the Wlan Profile {0}", ssid);
                testLogger.LogError(error.ToString());
                return false;
            }

        }

        public bool DeleteWlanProfile(string profileName)
        {
            try
            {
                using (Wlan wlanApi = new Wlan())
                {
                    testLogger.LogComment("DeleteWlanProfile");
                    var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                    if (NetworkInterfaceDataPathTests.CheckWlanInterfaceCount(wlanApi) == false)
                    {
                        return false;
                    }
                    var wlanInterface = wlanInterfaceList[0];

                    testLogger.LogComment("Deleting Profile {0}", profileName);
                    wlanApi.DeleteProfile(wlanInterface.Id, profileName);

                    return true;
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while Deleting the Wlan Profile {0}", profileName);
                testLogger.LogError(error.ToString());
                return false;
            }

        }

        public bool DisconnectNetwork()
        {
            testLogger.LogComment("Disconnecting from WLAN");
            try
            {
                using (Wlan wlanApi = new Wlan())
                {
                    var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                    if (NetworkInterfaceDataPathTests.CheckWlanInterfaceCount(wlanApi) == false)
                    {
                        return false;
                    }

                    var wlanInterface = wlanInterfaceList[0];

                    if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                    {
                        testLogger.LogComment("Disconnecting from WLAN network {0}", wlanInterface.Ssid);
                        wlanApi.Disconnect(wlanInterface.Id, new TimeSpan(0, 0, 10));
                    }

                    return true;
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while Disconnecting from the WLAN Network");
                testLogger.LogError(error.ToString());
                return false;
            }
        }


        public Guid ConfigureUdpSender(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();

            testLogger.LogTrace("Enter ConfigureUdpSender [{0}] port = {1}", testId, port);

            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);

            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            UdpSender udpSender = new UdpSender(testLogger);
            udpSender.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, udpSender);
            testLogger.LogTrace("Exit ConfigureUdpSender [{0}]", testId);
            return testId;
        }
        public void StartUdpSender(Guid testId)
        {
            testLogger.LogTrace("Enter StartUdpSender [{0}]", testId);
            IDataPathProtocolTest udpSender = testList[testId];
            udpSender.Start();
            testLogger.LogTrace("Exit StartUdpSender [{0}]", testId);
        }
        public Int64 StopUdpSender(Guid testId)
        {
            testLogger.LogTrace("Enter StopUdpSender [{0}]", testId);
            IDataPathProtocolTest udpSender = testList[testId];
            udpSender.Stop();
            Int64 unitsTransfered = udpSender.UnitsTransfered;
            testLogger.LogTrace("Exit StopUdpSender [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeUdpSender(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeUdpSender [{0}]", testId);
            IDataPathProtocolTest udpSender = testList[testId];
            udpSender.Stop();
            udpSender.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeUdpSender [{0}]", testId);
        }

        public Guid ConfigureUdpReceiver(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureUdpReceiver [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            UdpReceiver udpReceiver = new UdpReceiver(testLogger);
            udpReceiver.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, udpReceiver);
            testLogger.LogTrace("Exit ConfigureUdpReceiver [{0}]", testId);
            return testId;
        }
        public void StartUdpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StartUdpReceiver [{0}]", testId);
            IDataPathProtocolTest udpReceiver = testList[testId];
            udpReceiver.Start();
            testLogger.LogTrace("Exit StartUdpReceiver [{0}]", testId);
        }
        public Int64 StopUdpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StopUdpReceiver [{0}]", testId);
            IDataPathProtocolTest udpReceiver = testList[testId];
            udpReceiver.Stop();
            Int64 unitsTransfered = udpReceiver.UnitsTransfered;
            testLogger.LogTrace("Exit StopUdpReceiver [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeUdpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeUdpReceiver [{0}]", testId);
            IDataPathProtocolTest udpReceiver = testList[testId];
            udpReceiver.Stop();
            udpReceiver.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeUdpReceiver [{0}]", testId);
        }


        public Guid ConfigureTcpSender(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureTcpSender [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            TcpSender tcpSender = new TcpSender(testLogger);
            tcpSender.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, tcpSender);
            testLogger.LogTrace("Exit ConfigureTcpSender [{0}]", testId);
            return testId;
            

        }
        public void StartTcpSender(Guid testId)
        {
            testLogger.LogTrace("Enter StartTcpSender [{0}]", testId);
            IDataPathProtocolTest tcpSender = testList[testId];
            tcpSender.Start();
            testLogger.LogTrace("Exit StartTcpSender [{0}]", testId);
        }
        public Int64 StopTcpSender(Guid testId)
        {
            testLogger.LogTrace("Enter StopTcpSender [{0}]", testId);
            IDataPathProtocolTest tcpSender = testList[testId];
            tcpSender.Stop();
            Int64 unitsTransfered = tcpSender.UnitsTransfered;
            testLogger.LogTrace("Exit StopTcpSender [{0}]", testId);
            return unitsTransfered;
            
        }
        public void DisposeTcpSender(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeTcpSender [{0}]", testId);
            IDataPathProtocolTest tcpSender = testList[testId];
            tcpSender.Stop();
            tcpSender.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeTcpSender [{0}]", testId);
        }

        public Guid ConfigureTcpReceiver(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureTcpReceiver [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            TcpReceiver tcpReceiver = new TcpReceiver(testLogger);
            tcpReceiver.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, tcpReceiver);
            testLogger.LogTrace("Exit ConfigureTcpReceiver [{0}]", testId);
            return testId;
        }
        public void StartTcpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StartTcpReceiver [{0}]", testId);
            IDataPathProtocolTest tcpReceiver = testList[testId];
            tcpReceiver.Start();
            testLogger.LogTrace("Exit StartTcpReceiver [{0}]", testId);
        }
        public Int64 StopTcpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StopTcpReceiver [{0}]", testId);
            IDataPathProtocolTest tcpReceiver = testList[testId];
            tcpReceiver.Stop();
            Int64 unitsTransfered = tcpReceiver.UnitsTransfered;
            testLogger.LogTrace("Exit StopTcpReceiver [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeTcpReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeTcpReceiver [{0}]", testId);
            IDataPathProtocolTest tcpReceiver = testList[testId];
            tcpReceiver.Stop();
            tcpReceiver.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeTcpReceiver [{0}]", testId);
        }

        public Guid ConfigureMulticastSender(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureMulticastSender [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            MulticastSender multicastSender = new MulticastSender(testLogger);
            multicastSender.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, multicastSender);
            testLogger.LogTrace("Exit ConfigureMulticastSender [{0}]", testId);
            return testId;
        }
        public void StartMulticastSender(Guid testId)
        {
            testLogger.LogTrace("Enter StartMulticastSender [{0}]", testId);
            IDataPathProtocolTest multicastSender = testList[testId];
            multicastSender.Start();
            testLogger.LogTrace("Exit StartMulticastSender [{0}]", testId);
        }
        public Int64 StopMulticastSender(Guid testId)
        {
            testLogger.LogTrace("Enter StopMulticastSender [{0}]", testId);
            IDataPathProtocolTest multicastSender = testList[testId];
            multicastSender.Stop();
            Int64 unitsTransfered = multicastSender.UnitsTransfered;
            testLogger.LogTrace("Exit StopMulticastSender [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeMulticastSender(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeMulticastSender [{0}]", testId);
            IDataPathProtocolTest multicastSender = testList[testId];
            multicastSender.Stop();
            multicastSender.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeMulticastSender [{0}]", testId);
        }

        public Guid ConfigureMulticastReceiver(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureMulticastReceiver [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            MulticastReceiver multicastReceiver = new MulticastReceiver(testLogger);
            multicastReceiver.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, multicastReceiver);
            testLogger.LogTrace("Exit ConfigureMulticastReceiver [{0}]", testId);
            return testId;
        }
        public void StartMulticastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StartMulticastReceiver [{0}]", testId);
            IDataPathProtocolTest multicastReceiver = testList[testId];
            multicastReceiver.Start();
            testLogger.LogTrace("Exit StartMulticastReceiver [{0}]", testId);
        }
        public Int64 StopMulticastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StopMulticastReceiver [{0}]", testId);
            IDataPathProtocolTest multicastReceiver = testList[testId];
            multicastReceiver.Stop();
            Int64 unitsTransfered = multicastReceiver.UnitsTransfered;
            testLogger.LogTrace("Exit StopMulticastReceiver [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeMulticastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeMulticastReceiver [{0}]", testId);
            IDataPathProtocolTest multicastReceiver = testList[testId];
            multicastReceiver.Stop();
            multicastReceiver.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeMulticastReceiver [{0}]", testId);
        }

        public Guid ConfigureBroadcastSender(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureBroadcastSender [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            BroadcastSender broadcastSender = new BroadcastSender(testLogger);
            broadcastSender.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, broadcastSender);
            testLogger.LogTrace("Exit ConfigureBroadcastSender [{0}]", testId);
            return testId;

        }
        public void StartBroadcastSender(Guid testId)
        {
            testLogger.LogTrace("Enter StartBroadcastSender [{0}]", testId);
            IDataPathProtocolTest broadcastSender = testList[testId];
            broadcastSender.Start();
            testLogger.LogTrace("Exit StartBroadcastSender [{0}]", testId);
        }
        public Int64 StopBroadcastSender(Guid testId)
        {
            testLogger.LogTrace("Enter StopMulticastSender [{0}]", testId);
            IDataPathProtocolTest broadcastSender = testList[testId];
            broadcastSender.Stop();
            Int64 unitsTransfered = broadcastSender.UnitsTransfered;
            testLogger.LogTrace("Exit StopMulticastSender [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeBroadcastSender(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeBroadcastSender [{0}]", testId);
            IDataPathProtocolTest broadcastSender = testList[testId];
            broadcastSender.Stop();
            broadcastSender.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeBroadcastSender [{0}]", testId);
        }

        public Guid ConfigureBroadcastReceiver(string localInterfaceGuid, string remoteIpAddress, bool ipv6, UInt16 port)
        {
            Guid testId = Guid.NewGuid();
            testLogger.LogTrace("Enter ConfigureBroadcastReceiver [{0}] port = {1}", testId, port);
            testLogger.LogTrace("localInterfaceGuid: {0}  remoteIpAddress: {1} ipv6: {2}  port: {3}", localInterfaceGuid, remoteIpAddress, ipv6, port);
            TestMode testMode = TestMode.Lan;
            Guid interfaceGuid;
            if (String.IsNullOrEmpty(localInterfaceGuid))
            {
                testMode = TestMode.Wlan;
                interfaceGuid = new Guid();
            }
            else
            {
                interfaceGuid = Guid.Parse(localInterfaceGuid);
            }

            BroadcastReceiver broadcastReceiver = new BroadcastReceiver(testLogger);
            broadcastReceiver.Configure(testMode, interfaceGuid, remoteIpAddress, ipv6, port);
            testList.Add(testId, broadcastReceiver);
            testLogger.LogTrace("Exit ConfigureBroadcastReceiver [{0}]", testId);
            return testId;
        }
        public void StartBroadcastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StartMulticastReceiver [{0}]", testId);
            IDataPathProtocolTest broadcastReceiver = testList[testId];
            broadcastReceiver.Start();
            testLogger.LogTrace("Exit StartMulticastReceiver [{0}]", testId);
        }
        public Int64 StopBroadcastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter StopBroadcastReceiver [{0}]", testId);
            IDataPathProtocolTest broadcastReceiver = testList[testId];
            broadcastReceiver.Stop();
            Int64 unitsTransfered = broadcastReceiver.UnitsTransfered;
            testLogger.LogTrace("Exit StopBroadcastReceiver [{0}]", testId);
            return unitsTransfered;
        }
        public void DisposeBroadcastReceiver(Guid testId)
        {
            testLogger.LogTrace("Enter DisposeBroadcastReceiver [{0}]", testId);
            IDataPathProtocolTest broadcastReceiver = testList[testId];
            broadcastReceiver.Stop();
            broadcastReceiver.Dispose();
            testList.Remove(testId);
            testLogger.LogTrace("Exit DisposeBroadcastReceiver [{0}]", testId);
        }


        public string FindLocalWlanAddress(bool ipv6)
        {
            testLogger.LogTrace("FindLocalWlanAddress");
            string returnAddress = NetworkInterfaceDataPathTests.GetWirelessEndpoint(ipv6);
            testLogger.LogComment("FindLocalWlanAddress: {0}", returnAddress);
            testLogger.LogTrace("FindLocalWlanAddress Length: {0}", returnAddress.Length);
            return returnAddress;

        }

        public string FindLocalLanAddress(bool ipv6, Guid interfaceId)
        {
            testLogger.LogTrace("FindLocalLanAddress");
            string returnAddress = NetworkInterfaceDataPathTests.GetLanEndpoint(ipv6, interfaceId);
            testLogger.LogComment("FindLocalLanAddress: {0}", returnAddress);
            testLogger.LogTrace("FindLocalLanAddress Length: {0}", returnAddress.Length);
            return returnAddress;

        }

    }
}
