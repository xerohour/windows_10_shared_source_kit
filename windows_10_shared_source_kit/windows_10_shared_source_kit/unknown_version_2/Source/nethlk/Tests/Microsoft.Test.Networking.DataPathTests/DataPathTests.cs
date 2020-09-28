///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Linq;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Globalization;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using WEX.Logging.Interop;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.RemoteControl;
using Microsoft.Test.Networking.RemoteControl.StreamSocketTransport;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.Dhcp;

namespace HlkTest.DataPathTests
{

    public enum TestMode
    {
        Wlan,
        Lan,
        Unknown
    }

    
    [TestClass]
    public class NetworkInterfaceDataPathTests
    {

        private const UInt16 Port = 2329;
        private const UInt16 PortUdpSendIpv4 = 2330;
        private const UInt16 PortUdpReceiveIpv4 = 2331;
        private const UInt16 PortUdpSendIpv6 = 2332;
        private const UInt16 PortUdpReceiveIpv6 = 2333;
        private const UInt16 PortTcpSendIpv4 = 2334;
        private const UInt16 PortTcpReceiveIpv4 = 2335;
        private const UInt16 PortTcpSendIpv6 = 2336;
        private const UInt16 PortTcpReceiveIpv6 = 2337;
        private const UInt16 PortMulticastSendIpv4 = 2338;
        private const UInt16 PortMulticastReceiveIpv4 = 2339;
        private const UInt16 PortMulticastSendIpv6 = 2340;
        private const UInt16 PortMulticastReceiveIpv6 = 2341;
        private const UInt16 PortBroadcastSendIpv4 = 2342;
        private const UInt16 PortBroadcastReceiveIpv4 = 2343;

        private const UInt16 PortTcpSendIpv4_Backup = 2344;
        private const UInt16 PortTcpReceiveIpv4_Backup = 2345;
        private const UInt16 PortTcpSendIpv6_Backup = 2346;
        private const UInt16 PortTcpReceiveIpv6_Backup = 2347;



        private static Random random = new Random();
        private Object lockObject;
        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("DataPathTests");
        private static dynamic remoteInstances;

        private static TestMode testMode;
        private static RemoteCommandClient[] remoteCommandClients;
        private static List<string> remoteServers = new List<string>();
        private static string Sut = "";

        // WLAN Specific Parameters
        private static String APServer = "";
        private static bool DeviceSupports5Ghz = false;
        private static Collection<Guid> routers = null;
        private static string ssid24;
        private static string ssid5;

        // LAN Specific Parameters
        private static string dutInterfaceId;
        private static string sutInterfaceId;
        private static Guid dutInterfaceGuid;
        private static Guid sutInterfaceGuid;
        
        // Test IP Addresses
        private static string sutIPAddressIPv4;
        private static string sutIPAddressIPv6;
        private static string sutIPAddressIPv6WithLocalIndex;
        private static string dutIPAddressIPv4;
        private static string dutIPAddressIPv6;
        private static string dutIPAddressIPv6WithNoIndex;

        //Data Path configs
        private static DataPathConfig configs = new DataPathConfig();

        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            testMode = TestMode.Unknown;

            /* Load the configs from the parameter passed to the test */
            configs.Load(context);
            testLogger.LogComment("Test Parameter - {0}", configs.GetTestParameter());

            // Read Parameters
            APServer = null;

            if (context.Properties.Contains("TestMode"))
            {
                if (String.Compare(context.Properties["TestMode"].ToString(), "Wlan", StringComparison.OrdinalIgnoreCase) == 0)
                {
                    testMode = TestMode.Wlan;

                    if (context.Properties.Contains("DeviceSupports5Ghz"))
                    {
                        DeviceSupports5Ghz = bool.Parse(context.Properties["DeviceSupports5Ghz"].ToString());
                    }
                    testLogger.LogComment("DeviceSupports5Ghz = {0}", DeviceSupports5Ghz);

                    if (context.Properties.Contains("APServer"))
                    {
                        APServer = context.Properties["APServer"].ToString();
                        testLogger.LogComment("APServer Name = {0}", APServer);
                    }

                    dutInterfaceGuid = new Guid();
                    sutInterfaceGuid = new Guid();
                }
                else if (String.Compare(context.Properties["TestMode"].ToString(), "Lan", StringComparison.OrdinalIgnoreCase) == 0)
                {
                    testMode = TestMode.Lan;

                    if (context.Properties.Contains("DutInterfaceGuid"))
                    {
                        dutInterfaceId = context.Properties["DutInterfaceGuid"].ToString();
                        if (Guid.TryParse(dutInterfaceId, out dutInterfaceGuid) == false)
                        {
                            testLogger.LogComment("Could not Parse '{0}' as a GUID.", dutInterfaceId);
                        }
                    }

                    if (context.Properties.Contains("SutInterfaceGuid"))
                    {
                        sutInterfaceId = context.Properties["SutInterfaceGuid"].ToString();
                        if (Guid.TryParse(sutInterfaceId, out sutInterfaceGuid) == false)
                        {
                            testLogger.LogComment("Could not Parse '{0}' as a GUID.", sutInterfaceId);
                        }
                    }
                }
            }


            if(context.Properties.Contains("Sut"))
            {
                Sut = context.Properties["Sut"].ToString();
                testLogger.LogComment("Sut Name = {0}", Sut);
                remoteServers.Add(Sut);
            }

            Logger.SetAdditionalLogger(testLogger.LogTrace);

            // Fail is we did not get the neccessary parameters
            if (testMode == TestMode.Wlan)
            {
                if (APServer == null)
                {
                    testLogger.LogError("Did not get AP Server name as parameter");
                    testMode = TestMode.Unknown;
                    return;
                }
            }
            else if (testMode == TestMode.Lan)
            {
                if (String.IsNullOrEmpty(dutInterfaceId))
                {
                    testLogger.LogComment("DutInterfaceGuid is a required paramter for TestMode=Lan");
                    testMode = TestMode.Unknown;
                    return;
                }
                if (String.IsNullOrEmpty(sutInterfaceId))
                {
                    testLogger.LogComment("SutInterfaceGuid is a required paramter for TestMode=Lan");
                    testMode = TestMode.Unknown;
                    return;
                }
            }
            else
            {
                testLogger.LogError("TestMode=Lan or TestMode=Wlan is a required parameter");
                testMode = TestMode.Unknown;
                return;
            }

            if (remoteServers.Count == 0)
            {
                testLogger.LogError("Did not get SUT name as parameter");
                testMode = TestMode.Unknown;
                return;
            }

            if (testMode == TestMode.Wlan)
            {
                // Configure the APs
                testLogger.LogComment("Connecting to AP Config Server - " + APServer);
                using (ApConfigClient apConfigClient = new ApConfigClient(APServer))
                {
                    bool connect = apConfigClient.Connect();
                    if (connect)
                    {
                        testLogger.LogComment("Connection to AP Config Server Succeeded");
                    }
                    else
                    {
                        testLogger.LogError("Connection to AP Config Server failed");
                        testMode = TestMode.Unknown;
                        return;
                    }

                    testLogger.LogComment("Enum Routers");
                    routers = apConfigClient.EnumerateAccessPoints();
                    if (routers == null)
                    {
                        testLogger.LogError("Unable to enum routers, EnumerateAccessPoints returned null");
                        testMode = TestMode.Unknown;
                        return;
                    }
                    if (routers.Count >= 2)
                    {
                        testLogger.LogComment("AP Service is configured with 2 routers as expected");
                    }
                    else
                    {
                        testLogger.LogError("The AP Service is not properly configued with two APs.");
                        testLogger.LogError("Verify the Access Points are on, and the IPAddress, Password passed is correct.");
                        testMode = TestMode.Unknown;
                        return;
                    }
                }
            }
            

            // Verify we can connect to the SUT
            try
            {
                testLogger.LogComment("Verify we can communincate with the SUT");
                remoteInstances = RemoteManager.GetRemoteInstances<DataPathServer>(remoteServers.ToArray(), out remoteCommandClients).ToArray();
            }
            catch (InvalidOperationException)
            {
                testLogger.LogError("Unexpected Error when setting up commuincation with SUT.");
                testLogger.LogError("Verify that the DUT can communicate with the SUT ({0}) through the LAN interface.", remoteServers[0]);
                testMode = TestMode.Unknown;
                return;
            }

            // Configure the Network
            try
            {
                if (testMode == TestMode.Wlan)
                {
                    ConfigureNetwork();
                }
                else
                {
                    // For non-wlan, we can get IP addresses here. For WLAN, we have to
                    // do this after we finish the connect
                    testLogger.LogComment("Populate (non-WLAN) IP AddressList");
                    PopulateIPAddressList();
                }
            }
            catch (Exception error)
            {

                if (remoteCommandClients != null)
                {
                    foreach (RemoteCommandClient remoteCommandClient in remoteCommandClients)
                    {
                        testLogger.LogError("Disconnecting Client because of Error.");
                        testLogger.LogError(error.ToString());
                        testLogger.Flush();
                        remoteCommandClient.Disconnect();
                    }
                }

            }

        }


        public NetworkInterfaceDataPathTests()
        {
            lockObject = new Object();
        }


        private bool TestParameters()
        {
            if (testMode == TestMode.Unknown)
            {
                testLogger.Flush();
                return false;
            }
            return true;
            
        }

        [TestMethod]
        public void DataPathTestsLan()
        {
            if (testMode != TestMode.Lan)
            {
                testLogger.LogError("This test requires TestMode=Lan");
                testLogger.Flush();
                return;
            }

            testLogger.LogComment("For additional tracing use the following GUID: {0}", testLogger.TraceGuid.ToString());

            if (remoteInstances.Length != 1)
            {
                testLogger.LogError("TCP connection to the SUT over the management (non test) network has failed");
                return;
            }
            RunDataPathTests(false);
        }

        [TestMethod]
        public void DataPathTests24Ghz()
        {
            if (testMode != TestMode.Wlan)
            {
                testLogger.LogError("This test requires TestMode=Wlan");
                testLogger.LogResult(TestResult.Failed);
                testLogger.Flush();
                return;
            }

            testLogger.LogComment("For additional tracing use the following GUID: {0}", testLogger.TraceGuid.ToString());

            if (remoteInstances.Length != 1)
            {
                testLogger.LogError("TCP connection to the SUT over the manaegment (non test) network has failed");
                testLogger.LogResult(TestResult.Failed);
                return;
            }


            if (ConnectToNetwork(ssid24) == false)
            {
                testLogger.LogError("Error in connecting to the 2.4 GHz WLAN network");
                testLogger.LogResult(TestResult.Failed);
                return;
            }
            RunDataPathTests(false);

        }


        [TestMethod]
        public void DataPathTests5Ghz()
        {
            if (testMode != TestMode.Wlan)
            {
                testLogger.LogError("This test requires TestMode=Wlan");
                testLogger.LogResult(TestResult.Failed);
                testLogger.Flush();
                return;
            }

            testLogger.LogComment("For additional tracing use the following GUID: {0}", testLogger.TraceGuid.ToString());

            if (DeviceSupports5Ghz == false)
            {
                testLogger.LogComment("5GHz is not supported on this device.  Passing test");
                return;
            }
            if (remoteInstances.Length != 1)
            {
                testLogger.LogError("TCP connection to the SUT over the management (non test) network has failed");
                testLogger.LogResult(TestResult.Failed);
                return;
            }

            if (ConnectToNetwork(ssid5) == false)
            {
                testLogger.LogError("Error in connecting to the 5 GHz WLAN network");
                testLogger.LogResult(TestResult.Failed);
                return;
            }
            // Use backup TCP port number so that we dont collide with sockets that are still in TIME_WAIT state
            RunDataPathTests(true);

        }

        public void RunDataPathTests(bool UsebackupTcpPorts)
        {
            List<SendReceiveTestPair> testList = new List<SendReceiveTestPair>();
            testLogger.LogComment("Parameter - UDP {0}", configs.l4protocol.UDP);
            testLogger.LogComment("Parameter - TCP {0}", configs.l4protocol.TCP);
            testLogger.LogComment("Parameter - ipv4 {0}", configs.ipversion.ipv4);
            testLogger.LogComment("Parameter - ipv6 {0}", configs.ipversion.ipv6);
            testLogger.LogComment("Parameter - Unicast {0}", configs.traffic.Unicast);
            testLogger.LogComment("Parameter - Multicast {0}", configs.traffic.Multicast);
            testLogger.LogComment("Parameter - Broadcast {0}", configs.traffic.Broadcast);

            try
            {
                if (configs.traffic.Unicast)
                {
                    if (configs.l4protocol.UDP)
                    {
                        if (configs.ipversion.ipv4)
                        {
                            testList.Add(UdpTrafficSend(PortUdpSendIpv4, false));
                            testList.Add(UdpTrafficReceive(PortUdpReceiveIpv4, false));
                            testLogger.LogComment("Added UDP V4 data path test");
                        }
                        if (configs.ipversion.ipv6)
                        {
                            testList.Add(UdpTrafficSend(PortUdpSendIpv6, true));
                            testList.Add(UdpTrafficReceive(PortUdpReceiveIpv6, true));
                            testLogger.LogComment("Added UDP V6 data path test");
                        }
                    }
                    if (configs.l4protocol.TCP)
                    {
                        if (configs.ipversion.ipv4)
                        {
                            testList.Add(TcpTrafficSend(UsebackupTcpPorts ? PortTcpSendIpv4_Backup : PortTcpSendIpv4, false));
                            testList.Add(TcpTrafficReceive(UsebackupTcpPorts ? PortTcpReceiveIpv4_Backup : PortTcpReceiveIpv4, false));
                            testLogger.LogComment("Added TCP V4 data path test");
                        }
                        if (configs.ipversion.ipv6)
                        {
                            testList.Add(TcpTrafficSend(UsebackupTcpPorts ? PortTcpSendIpv6_Backup : PortTcpSendIpv6, true));
                            testList.Add(TcpTrafficReceive(UsebackupTcpPorts ? PortTcpReceiveIpv6_Backup : PortTcpReceiveIpv6, true));
                            testLogger.LogComment("Added TCP V6 data path test");
                        }
                    }
                }
                if (configs.traffic.Multicast)
                {
                    if (configs.ipversion.ipv4)
                    {
                        testList.Add(MulticastTrafficSend(PortMulticastSendIpv4, false));
                        testList.Add(MulticastTrafficReceive(PortMulticastReceiveIpv4, false));
                        testLogger.LogComment("Added Multicast V4 data path test");
                    }
                    if (configs.ipversion.ipv6)
                    {
                        testList.Add(MulticastTrafficSend(PortMulticastSendIpv6, true));
                        testList.Add(MulticastTrafficReceive(PortMulticastReceiveIpv6, true));
                        testLogger.LogComment("Added Multicast V6 data path test");
                    }
                }
                if (configs.traffic.Broadcast)
                {
                    testList.Add(BroadcastTrafficSend(PortBroadcastSendIpv4));
                    testList.Add(BroadcastTrafficReceive(PortBroadcastReceiveIpv4));
                    testLogger.LogComment("Added Broadcast V4 data path test");
                }


                testLogger.LogComment("Start Listening for data");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.ReceiveTest.Start();
                }

                // Wait for all listeners to Start
                Wlan.Sleep(5000);

                // Start Sending
                testLogger.LogComment("Start Sending data");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.SendTest.Start();
                }

                // The test is running now.  Stop test after two minutes
                Wlan.Sleep(1000 * 60 * 2); // 2 Minutes

                // Stop Sending
                testLogger.LogComment("Stop Sending data");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.SendTest.Stop();
                }

                // Wait for all Senders to drain
                Wlan.Sleep(5000);

                testLogger.LogComment("Stop Receiving data");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.ReceiveTest.Stop();
                }

                // Wait for all Receievers to drain
                Wlan.Sleep(5000);

                testLogger.LogComment("Results:");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.LogResults();
                    testLogger.LogComment("\n");
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while running test: {0}", error.ToString());
                testLogger.LogResult(TestResult.Failed);
            }
            finally
            {
                testLogger.LogComment("Cleaning up tests");
                foreach (SendReceiveTestPair testPair in testList)
                {
                    testPair.Dispose();
                }
            }
        }


        public SendReceiveTestPair UdpTrafficSend(UInt16 port, bool ipv6)
        {
            string name;
            if (ipv6)
            {
                name = "UDP Send Test (IPv6)";
            }
            else
            {
                name = "UDP Send Test (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1}]", name, port);

            string remoteIpAddress;
            string remoteIpAddressWithLocalIndex;
            if (ipv6)
            {
                remoteIpAddress = sutIPAddressIPv6;
                remoteIpAddressWithLocalIndex = sutIPAddressIPv6WithLocalIndex;
            }
            else
            {
                remoteIpAddress = sutIPAddressIPv4;
                remoteIpAddressWithLocalIndex = sutIPAddressIPv4;
            }

            testLogger.LogTrace("remoteIpAddress               : {0}", remoteIpAddress);
            testLogger.LogTrace("remoteIpAddressWithLocalIndex : {0}", remoteIpAddressWithLocalIndex);

            UdpSender udpSender = new UdpSender(testLogger);
            testLogger.LogTrace("UdpSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpAddressWithLocalIndex, ipv6, port);
            udpSender.Configure(testMode, dutInterfaceGuid, remoteIpAddressWithLocalIndex, ipv6, port);

            RemoteUdpReceiver udpReceiver = new RemoteUdpReceiver(remoteInstances[0]);
            testLogger.LogTrace("RemoteUdpReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpAddress, ipv6, port);
            udpReceiver.Configure(testMode, sutInterfaceGuid, remoteIpAddress, ipv6, port);


            return new SendReceiveTestPair(name, udpSender, udpReceiver, .8, testLogger);


        }

        public SendReceiveTestPair UdpTrafficReceive(UInt16 port, bool ipv6)
        {
            string name;
            if (ipv6)
            {
                name = "UDP Receive Test (IPv6)";
            }
            else
            {
                name = "UDP Receive Test (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1}]", name, port);


            string remoteIpAddressWithIndex;
            string remoteIpAddress;
            if (ipv6)
            {
                remoteIpAddressWithIndex = dutIPAddressIPv6;
                remoteIpAddress = dutIPAddressIPv6WithNoIndex;
            }
            else
            {
                remoteIpAddressWithIndex = dutIPAddressIPv4;
                remoteIpAddress = dutIPAddressIPv4;
            }

            testLogger.LogTrace("remoteIpAddressWithIndex : {0}", remoteIpAddressWithIndex);
            testLogger.LogTrace("remoteIpAddress          : {0}", remoteIpAddress);


            RemoteUdpSender udpSender = new RemoteUdpSender(remoteInstances[0]);
            testLogger.LogTrace("RemoteUdpSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpAddress, ipv6, port);
            udpSender.Configure(testMode, sutInterfaceGuid, remoteIpAddress, ipv6, port);

            UdpReceiver udpReceiver = new UdpReceiver(testLogger);
            testLogger.LogTrace("UdpReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpAddressWithIndex, ipv6, port);
            udpReceiver.Configure(testMode, dutInterfaceGuid, remoteIpAddressWithIndex, ipv6, port);

            return new SendReceiveTestPair(name, udpSender, udpReceiver, .8, testLogger);


        }

        public SendReceiveTestPair TcpTrafficSend(UInt16 port, bool ipv6)
        {
            string name;
            if (ipv6)
            {
                name = "TCP Send (IPv6)";
            }
            else
            {
                name = "TCP Send (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1}]", name, port);

            string remoteIpAddress;
            string remoteIpAddressWithLocalIndex;
            if (ipv6)
            {
                remoteIpAddress = sutIPAddressIPv6;
                remoteIpAddressWithLocalIndex = sutIPAddressIPv6WithLocalIndex;
            }
            else
            {
                remoteIpAddress = sutIPAddressIPv4;
                remoteIpAddressWithLocalIndex = sutIPAddressIPv4;
            }

            testLogger.LogTrace("remoteIpAddress               : {0}", remoteIpAddress);
            testLogger.LogTrace("remoteIpAddressWithLocalIndex : {0}", remoteIpAddressWithLocalIndex);


            TcpSender tcpSender = new TcpSender(testLogger);
            testLogger.LogTrace("TcpSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpAddressWithLocalIndex, ipv6, port);
            tcpSender.Configure(testMode, dutInterfaceGuid, remoteIpAddressWithLocalIndex, ipv6, port);

            RemoteTcpReceiver tcpReceiver = new RemoteTcpReceiver(remoteInstances[0]);
            testLogger.LogTrace("RemoteTcpReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpAddress, ipv6, port);
            tcpReceiver.Configure(testMode, sutInterfaceGuid, remoteIpAddress, ipv6, port);

            return new SendReceiveTestPair(name, tcpSender, tcpReceiver, .95, testLogger);

        }

        public SendReceiveTestPair TcpTrafficReceive(UInt16 port, bool ipv6)
        {
            string name;
            if (ipv6)
            {
                name = "TCP Receive (IPv6)";
            }
            else
            {
                name = "TCP Receive (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1}]", name, port);

            string remoteIpAddressWithIndex;
            string remoteIpAddress;
            if (ipv6)
            {
                remoteIpAddressWithIndex = dutIPAddressIPv6;
                remoteIpAddress = dutIPAddressIPv6WithNoIndex;
            }
            else
            {
                remoteIpAddressWithIndex = dutIPAddressIPv4;
                remoteIpAddress = dutIPAddressIPv4;
            }

            testLogger.LogTrace("remoteIpAddressWithIndex : {0}", remoteIpAddressWithIndex);
            testLogger.LogTrace("remoteIpAddress          : {0}", remoteIpAddress);

            RemoteTcpSender tcpSender = new RemoteTcpSender(remoteInstances[0]);
            testLogger.LogTrace("RemoteTcpSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpAddress, ipv6, port);
            tcpSender.Configure(testMode, sutInterfaceGuid, remoteIpAddress, ipv6, port);

            TcpReceiver tcpReceiver = new TcpReceiver(testLogger);
            testLogger.LogTrace("TcpReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpAddressWithIndex, ipv6, port);
            tcpReceiver.Configure(testMode, dutInterfaceGuid, remoteIpAddressWithIndex, ipv6, port);

            return new SendReceiveTestPair(name, tcpSender, tcpReceiver, .95, testLogger);

        }

        public SendReceiveTestPair MulticastTrafficSend(UInt16 port, bool ipv6)
        {
            string name;
            string multicastIPAddress;
            if (ipv6)
            {
                multicastIPAddress = "FF02::1:FF28:9C5A";
                name = "Multicast Send Test (IPv6)";
            }
            else
            {
                multicastIPAddress = "224.5.6.7";
                name = "Multicast Send Test (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1} IP = {2}]", name, port, multicastIPAddress);

            testLogger.LogTrace("multicastIPAddress : {0}", multicastIPAddress);

            MulticastSender multicastSender = new MulticastSender(testLogger);
            testLogger.LogTrace("MulticastSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, multicastIPAddress, ipv6, port);
            multicastSender.Configure(testMode, dutInterfaceGuid, multicastIPAddress, ipv6, port);

            RemoteMulticastReceiver multicastReceiver = new RemoteMulticastReceiver(remoteInstances[0]);
            testLogger.LogTrace("RemoteMulticastReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, multicastIPAddress, ipv6, port);
            multicastReceiver.Configure(testMode, sutInterfaceGuid, multicastIPAddress, ipv6, port);

            return new SendReceiveTestPair(name, multicastSender, multicastReceiver, .8, testLogger);


        }

        public SendReceiveTestPair MulticastTrafficReceive(UInt16 port, bool ipv6)
        {
            string name;
            string multicastIPAddress;
            if (ipv6)
            {
                multicastIPAddress = "FF02::1:FF28:9C5B";
                name = "Multicast Receive Test (IPv6)";
            }
            else
            {
                multicastIPAddress = "224.5.6.8";
                name = "Multicast Receive Test (IPv4)";
            }
            testLogger.LogComment("Preparing {0} [Port = {1} IP = {2}]", name, port, multicastIPAddress);

            testLogger.LogTrace("multicastIPAddress : {0}", multicastIPAddress);

            RemoteMulticastSender multicastSender = new RemoteMulticastSender(remoteInstances[0]);
            testLogger.LogTrace("RemoteMulticastSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, multicastIPAddress, ipv6, port);
            multicastSender.Configure(testMode, sutInterfaceGuid, multicastIPAddress, ipv6, port);

            MulticastReceiver multicastReceiver = new MulticastReceiver(testLogger);
            testLogger.LogTrace("MulticastReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, multicastIPAddress, ipv6, port);
            multicastReceiver.Configure(testMode, dutInterfaceGuid, multicastIPAddress, ipv6, port);

            return new SendReceiveTestPair(name, multicastSender, multicastReceiver, .8, testLogger);


        }

        public SendReceiveTestPair BroadcastTrafficSend(UInt16 port)
        {
            string name = "Broadcast Send Test (IPv4)";
            string remoteIpEndPoint = "255.255.255.255";

            testLogger.LogComment("Preparing {0} [Port = {1} IP = {2}]", name, port, remoteIpEndPoint);

            testLogger.LogTrace("remoteIpEndPoint : {0}", remoteIpEndPoint);

            BroadcastSender broadcastSender = new BroadcastSender(testLogger);
            testLogger.LogTrace("BroadcastSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpEndPoint, false, port);
            broadcastSender.Configure(testMode, dutInterfaceGuid, remoteIpEndPoint, false, port);

            RemoteBroadcastReceiver broadcastReceiver = new RemoteBroadcastReceiver(remoteInstances[0]);
            testLogger.LogTrace("RemoteBroadcastReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpEndPoint, false, port);
            broadcastReceiver.Configure(testMode, sutInterfaceGuid, remoteIpEndPoint, false, port);

            return new SendReceiveTestPair(name, broadcastSender, broadcastReceiver, .8, testLogger);


        }

        public SendReceiveTestPair BroadcastTrafficReceive(UInt16 port)
        {
            string name = "Broadcast Receive (IPv4)";

            string remoteIpEndPoint = "255.255.255.255";
            testLogger.LogComment("Preparing {0} [Port = {1} IP = {2}]", name, port, remoteIpEndPoint);

            testLogger.LogTrace("remoteIpEndPoint : {0}", remoteIpEndPoint);

            RemoteBroadcastSender broadcastSender = new RemoteBroadcastSender(remoteInstances[0]);
            testLogger.LogTrace("RemoteBroadcastSender.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), sutInterfaceGuid, remoteIpEndPoint, false, port);
            broadcastSender.Configure(testMode, sutInterfaceGuid, remoteIpEndPoint, false, port);

            BroadcastReceiver broadcastReceiver = new BroadcastReceiver(testLogger);
            testLogger.LogTrace("BroadcastReceiver.Configure : TestMode {0}  InterfaceGuid {1}  Address {2}  IPv6  {3}  Port {4}", testMode.ToString(), dutInterfaceGuid, remoteIpEndPoint, false, port);
            broadcastReceiver.Configure(testMode, dutInterfaceGuid, remoteIpEndPoint, false, port);

            return new SendReceiveTestPair(name, broadcastSender, broadcastReceiver, .8, testLogger);


        }

        
        [ClassCleanup]
        public static void Cleanup()
        {
            testLogger.LogComment("Entering: Suite Cleanup");

            if (testMode == TestMode.Wlan)
            {
                DisconnectNetwork(remoteInstances);
                DeleteWlanProfiles(remoteInstances);
            }

            if (remoteCommandClients != null)
            {
                foreach (RemoteCommandClient remoteCommandClient in remoteCommandClients)
                {
                    if (remoteCommandClient != null)
                    {
                        testLogger.LogComment("Disconnecting Client at end of test.");
                        remoteCommandClient.Disconnect();
                    }
                }


            }
            testLogger.LogComment("Exiting:Suite Cleanup");
        }

        // Payload Helpers
        internal static Byte[] GenerateUdpPayload()
        {
            return GeneratePayload(random.Next(1, 1200));
        }
        internal static Byte[] GeneratePayload(int length)
        {
            byte[] randomBytes = new Byte[length];
            for (int index = 0; index < randomBytes.Length; index++)
            {
                randomBytes[index] = Convert.ToByte(random.Next(0, 255));
            }
            return randomBytes;
        }

        internal static string GetWirelessEndpoint(bool ipv6)
        {
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                if (wlanInterfaceList.Count < 1)
                {
                    throw new TestConfigException("No WLAN Interfaces were discovered.  Ensure that WLAN interfaces are enabled, discoverable, and operational.");
    			}
                var wlanInterface = wlanInterfaceList[0];

                using (Sockets win32Sockets = new Sockets(testLogger))
                {
                    testLogger.LogTrace("Calling win32Sockets.FindIpAddress.  Interface: {0} IPv6: {1}", wlanInterface.Id.ToString(), ipv6);
                    string returnString = win32Sockets.FindIpAddress(wlanInterface.Id, ipv6);
                    testLogger.LogTrace("Calling win32Sockets.FindIpAddress returned: {0}", returnString);
                    return returnString;
                }

            }

        }

        internal static string GetLanEndpoint(bool ipv6, Guid interfaceGuid)
        {
            using (Sockets win32Sockets = new Sockets(testLogger))
            {
                testLogger.LogTrace("Calling win32Sockets.FindIpAddress.  Interface: {0} IPv6: {1}", interfaceGuid.ToString(), ipv6);
                string returnString = win32Sockets.FindIpAddress(interfaceGuid, ipv6);
                testLogger.LogTrace("Calling win32Sockets.FindIpAddress returned: {0}", returnString);
                return returnString;

            }

        }

        private static void PopulateIPAddressList()
        {
            if (testMode == TestMode.Wlan)
            {
                PopulateIPAddressListWlan();
            }
            else
            {
                PopulateIPAddressListLan();

	            testLogger.LogComment("Sut IP Address IPv4                  : {0}", sutIPAddressIPv4);
	            testLogger.LogComment("Sut IP Address IPv6                  : {0}", sutIPAddressIPv6);
	            testLogger.LogComment("Sut IP Address IPv6 With Local Index : {0}", sutIPAddressIPv6WithLocalIndex);
	            testLogger.LogComment("Dut IP Address IPv4                  : {0}", dutIPAddressIPv4);
	            testLogger.LogComment("Dut IP Address IPv6                  : {0}", dutIPAddressIPv6);
	            testLogger.LogComment("Dut IP Address IPv6 With No Index    : {0}", dutIPAddressIPv6WithNoIndex);
            }
        }

        private static void PopulateIPAddressListWlan()
        {
            sutIPAddressIPv4 = remoteInstances[0].FindLocalWlanAddress(false);
            testLogger.LogComment("Sut IP Address IPv4                  : {0}", sutIPAddressIPv4);

            sutIPAddressIPv6 = remoteInstances[0].FindLocalWlanAddress(true);
            testLogger.LogComment("Sut IP Address IPv6                  : {0}", sutIPAddressIPv6);

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                if (wlanInterfaceList.Count < 1)
                {
                    throw new TestConfigException("No WLAN Interfaces were discovered.  Ensure that WLAN interfaces are enabled, discoverable, and operational.");
				}
                var wlanInterface = wlanInterfaceList[0];

                UInt32 wlanInterfaceIndex = GetNetworkIndex(wlanInterface.Id);
                sutIPAddressIPv6WithLocalIndex = AddIpv6Index(sutIPAddressIPv6, wlanInterfaceIndex);
            }
            testLogger.LogComment("Sut IP Address IPv6 With Local Index : {0}", sutIPAddressIPv6WithLocalIndex);

            dutIPAddressIPv4 = GetWirelessEndpoint(false);
            testLogger.LogComment("Dut IP Address IPv4                  : {0}", dutIPAddressIPv4);

            dutIPAddressIPv6 = GetWirelessEndpoint(true);
            testLogger.LogComment("Dut IP Address IPv6                  : {0}", dutIPAddressIPv6);

            dutIPAddressIPv6WithNoIndex = RemoveIpv6Index(dutIPAddressIPv6);
            testLogger.LogComment("Dut IP Address IPv6 With No Index    : {0}", dutIPAddressIPv6WithNoIndex);
        }

        private static void PopulateIPAddressListLan()
        {
            sutIPAddressIPv4 = remoteInstances[0].FindLocalLanAddress(false, sutInterfaceGuid);
            sutIPAddressIPv6 = remoteInstances[0].FindLocalLanAddress(true, sutInterfaceGuid);

            UInt32 lanInterfaceIndex = GetNetworkIndex(dutInterfaceGuid);
            sutIPAddressIPv6WithLocalIndex = AddIpv6Index(sutIPAddressIPv6, lanInterfaceIndex);

            dutIPAddressIPv4 = GetLanEndpoint(false, dutInterfaceGuid);
            dutIPAddressIPv6 = GetLanEndpoint(true, dutInterfaceGuid);
            dutIPAddressIPv6WithNoIndex = RemoveIpv6Index(dutIPAddressIPv6);
        }

        private static void ConfigureNetwork()
        {
            APConfig config = CreateWlanNetwork();

            string profile24 = null;
            string profile5 = null;


            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();

                if (CheckWlanInterfaceCount(wlanApi) == false)
                {
                    throw new TestConfigException("No WLAN Interfaces were discovered.  Ensure that WLAN interfaces are enabled, discoverable, and operational.");
                }

                var wlanInterface = wlanInterfaceList[0];

                if (DeviceSupports5Ghz)
                {
                    ssid5 = config.SSID_5GHZ;
                    testLogger.LogComment("Creating Wlan Profile: {0}", ssid5);
                    profile5 = config.CreateProfile5ghz(ConnectionMode.Manual, false);
                    wlanApi.CreateProfile(wlanInterface.Id, profile5);
                }
                

                ssid24 = config.SSID_24GHZ;
                testLogger.LogComment("Creating Wlan Profile: {0}", ssid24);
                profile24 = config.CreateProfile24ghz(ConnectionMode.Manual, false);
                wlanApi.CreateProfile(wlanInterface.Id, profile24);
                
                if (DeviceSupports5Ghz)
                {
                    testLogger.LogComment("Creating Wlan Profile in SUT : {0}", ssid5);
                    if (remoteInstances[0].CreateProfile(profile5, ssid5) == false)
                    {
                        throw new TestConfigException("Error when creating the WLAN profile on the SUT.  See Sut logs for failure details");
                    }
                }
                if (remoteInstances[0].CreateProfile(profile24, ssid24) == false)
                {
                    testLogger.LogComment("Creating Wlan Profile in SUT : {0}", ssid24);
                    throw new TestConfigException("Error when creating the WLAN profile on the SUT.  See Sut logs for failure details");
                }

            }
        }
        private static bool ConnectToNetwork(string ssid)
        {
            try
            {
                using (Wlan wlanApi = new Wlan())
                {
                    var wlanInterfaceList = wlanApi.EnumWlanInterfaces();

                    if (CheckWlanInterfaceCount(wlanApi) == false)
                    {
                        testLogger.LogError("Did not find WLAN Interface.  Verify the WLAN device is enabled.");
                        return false;
                    }
                    var wlanInterface = wlanInterfaceList[0];

                    bool localConnectionNeeded = true;

                    if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                    {
                        if (String.Compare(ssid, wlanInterface.Ssid, StringComparison.OrdinalIgnoreCase) == 0)
                        {
                            localConnectionNeeded = false;
                        }
                        else
                        {
                            wlanApi.Disconnect(wlanInterface.Id, new TimeSpan(0, 0, 10));
                        }
                    }

                    // Connect DUT next
                    if (localConnectionNeeded)
                    {
                        testLogger.LogComment("Connecting to SSID: {0}", ssid);
                        wlanApi.TryScan(wlanInterface.Id, false, new TimeSpan(0, 0, 10));
                        wlanApi.ProfileConnect(wlanInterface.Id, ssid, new TimeSpan(0, 0, 30));
                    }

                    // Connect SUT second so that if there is a config problem, we catch is
                    // in the DUT connection. We also do the wait before the SUT
                    testLogger.LogComment("Connecting to SSID on SUT: {0}", ssid);
                    if (remoteInstances[0].ConnectToNetwork(ssid) == false)
                    {
                        testLogger.LogError("SUT failed to connect to {0}.  See SUT logs for details", ssid);
                        return false;
                    }

                    // On DUT wait for IP
                    if (localConnectionNeeded)
                    {
                        testLogger.LogComment("Waiting for local DHCP to be stable");
                        Microsoft.Test.Networking.Kit.Helpers.WaitForWlanConnectivity(wlanInterface.Id, 1000 * 15, testLogger);
                    }

                    testLogger.LogComment("Populate (WLAN) IP AddressList");
                    PopulateIPAddressList();
                }
            }
            catch (Exception error)
            {
                testLogger.LogError("Error encountered while Connecting to the Wlan Profile {0}", ssid);
                testLogger.LogError(error.ToString());
                return false;
            }
            return true;
        }



        internal static bool CheckWlanInterfaceCount(Wlan wlanApi)
        {
            var wlanInterfaceList = wlanApi.EnumWlanInterfaces();

            if (wlanInterfaceList.Count >= 1)
            {
                return true;
            }
            testLogger.LogError("Could not enumerate the WLAN Interface.");
            testLogger.LogError("Ensure the WLAN device is enabled, that it can be enumerated with 'NETSH WLAN SHOW INT', and that the wlansvc service is running.");
            return false;
        }
        private static void DisconnectNetwork(dynamic remoteInstances)
        {
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                if (CheckWlanInterfaceCount(wlanApi) == false)
                {
                    return;
                }
                var wlanInterface = wlanInterfaceList[0];

                if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    testLogger.LogComment("Disconnecting from WLAN Network");
                    wlanApi.Disconnect(wlanInterface.Id, new TimeSpan(0, 0, 10));
                }

                remoteInstances[0].DisconnectNetwork();

            }
        }
        private static void DeleteWlanProfiles(dynamic remoteInstances)
        {

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                if (CheckWlanInterfaceCount(wlanApi) == false)
                {
                    return;
                }
                var wlanInterface = wlanInterfaceList[0];

                if (DeviceSupports5Ghz)
                {
                    testLogger.LogComment("Deleting profile: {0}", ssid5);
                    wlanApi.DeleteProfile(wlanInterface.Id, ssid5);
                    remoteInstances[0].DeleteWlanProfile(ssid5);
                }
                testLogger.LogComment("Deleting profile: {0}", ssid24);
                wlanApi.DeleteProfile(wlanInterface.Id, ssid24);
                remoteInstances[0].DeleteWlanProfile(ssid24);

                
            }
        }

        private static APConfig CreateWlanNetwork()
        {
            testLogger.LogComment("Creating WPA2PSK/AES AP");
            APConfig currentConfig = WlanAccessPointHelper.CreateWirelessAccessPoint(AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password",
                                                                            AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password");

            testLogger.LogComment("\t2.4 Ghz Settings \n\t\tTentative SSID: {0}, BSSID: {1}, PhyType: {2}, Auth/Cipher: {3}/{4}",
                currentConfig.SSID_24GHZ, currentConfig.BSSID_24GHZ, currentConfig.PhyType_24GHZ.ApConfigValue,
                currentConfig.Authentication_24GHZ.ApConfigValue, currentConfig.Encryption_24GHZ.ApConfigValue);

            if (DeviceSupports5Ghz)
            {
                testLogger.LogComment("\t5 Ghz Settings \n\t\tTentative SSID: {0}, BSSID: {1}, PhyType: {2}, Auth/Cipher: {3}/{4}",
                    currentConfig.SSID_5GHZ, currentConfig.BSSID_5GHZ, currentConfig.PhyType_5GHZ.ApConfigValue,
                    currentConfig.Authentication_5GHZ.ApConfigValue, currentConfig.Encryption_5GHZ.ApConfigValue);
            }

            using (ApConfigClient apConfigClient = new ApConfigClient(APServer))
            {
                bool connect = apConfigClient.Connect();
                if (connect)
                {
                    testLogger.LogComment("Connection to the AP Config Server Succeeded");
                }
                else
                {
                    testLogger.LogError("Connection to the AP Config Server failed");
                    return null;
                }

                ConfiguredSSIDs configuredSSID = new ConfiguredSSIDs("", "");
                if (apConfigClient.ApplyConfiguration(routers[0], currentConfig, out configuredSSID))
                {
                    testLogger.LogComment("Successfully applied the Access Point configuration");
                    currentConfig.SSID_24GHZ = configuredSSID.TwoPoint4GhzSSID;
                    currentConfig.SSID_5GHZ = configuredSSID.FiveghzSSID;

                    testLogger.LogComment("Configured 2.4GHz SSID: {0}", currentConfig.SSID_24GHZ);
                    if (DeviceSupports5Ghz)
                    {
                        testLogger.LogComment("Configured 5GHz SSID: {0}", currentConfig.SSID_5GHZ);
                    }
                }
                else
                {
                    testLogger.LogError("Unable to apply the Access Point configuration");
                }
            }
            testLogger.LogComment("Waiting for Access point to beacon after reset");
            Wlan.Sleep(10000);
            return currentConfig;



        }

        private static string FormatPercentage(double input)
        {
            if (input == 0)
            {
                return "0.00%";
            }
            input = input * 100;
            return string.Format(CultureInfo.InvariantCulture, "{0}%", Math.Round(input, 2));

        }

        public static string RemoveIpv6Index(string address)
        {
            int percentageIndex = address.IndexOf('%');
            if (percentageIndex == -1)
            {
                return address;
            }
            else
            {
                return address.Substring(0, percentageIndex);
            }
        }
        public static string AddIpv6Index(string address, UInt32 index)
        {
            StringBuilder returnAddress = new StringBuilder(RemoveIpv6Index(address));
            returnAddress.Append("%");
            returnAddress.Append(index.ToString(CultureInfo.InvariantCulture));
            return returnAddress.ToString();

        }

        public static UInt32 GetNetworkIndex(Guid wlanInterfaceId)
        {
            using (Sockets win32Sockets = new Sockets(testLogger))
            {
                return win32Sockets.FindInterfaceIndexIpv6(wlanInterfaceId);

            }

        }

        public static int RandomWaitTime()
        {
            return random.Next(1000, 3000);
        }


    }

    public class SendReceiveTestPair : IDisposable
    {
        public string Name { get; private set; }
        public IDataPathProtocolTest SendTest { get; private set; }
        public IDataPathProtocolTest ReceiveTest { get; private set; }
        public double RequiredResults {get; private set;}
        
        private WlanHckTestLogger testLogger; 

        public SendReceiveTestPair(string name, IDataPathProtocolTest sendTest, IDataPathProtocolTest receiveTest, double requiredResults, WlanHckTestLogger testLogger)
        {
            this.Name = name;
            this.SendTest = sendTest;
            this.ReceiveTest = receiveTest;
            this.RequiredResults = requiredResults;
            this.testLogger = testLogger;
        }

        ~SendReceiveTestPair()
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
                SendTest.Dispose();
                ReceiveTest.Dispose();
            }
        }

        public void LogResults()
        {
            testLogger.LogComment("{0} Test Results", Name);
            

            String unitType;
            if (SendTest.ReportAsBytes)
            {
                unitType = "Bytes";
            }
            else
            {
                unitType = "Packets";
            }

            testLogger.LogComment("{0} Sent: {1}  {0} Received: {2}", unitType, SendTest.UnitsTransfered, ReceiveTest.UnitsTransfered);

            if (SendTest.UnitsTransfered == 0)
            {
                testLogger.LogError("DUT failed to send {0} {1}", Name, unitType);
                testLogger.LogResult(TestResult.Failed);
                return;
            }
            if (ReceiveTest.UnitsTransfered == 0)
            {
                testLogger.LogError("SUT failed to receive data {0} {1}", Name, unitType);
                testLogger.LogResult(TestResult.Failed);
                return;
            }
            double sendPercentage = (double)ReceiveTest.UnitsTransfered / (double)SendTest.UnitsTransfered;
            testLogger.LogComment("Success Rate: {0}", FormatPercentage(sendPercentage));
            if (sendPercentage < RequiredResults)
            {
                testLogger.LogError("Success rate must be > {0}", FormatPercentage(RequiredResults));
                testLogger.LogResult(TestResult.Failed);
                return;
            }
        }
        private static string FormatPercentage(double input)
        {
            if (input == 0)
            {
                return "0.00%";
            }
            input = input * 100;
            return string.Format(CultureInfo.InvariantCulture, "{0}%", Math.Round(input, 2));

        }

    }

    public interface IDataPathProtocolTest : IDisposable
    {
        Int64 UnitsTransfered { get; }
        void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port);
        void Start();
        void Stop();
        bool ReportAsBytes { get; }
    }

    public class RemoteUdpSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }
        private Guid testId;
        private dynamic remoteInstance;

        public RemoteUdpSender(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteUdpSender()
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
                remoteInstance.DisposeUdpSender(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureUdpSender(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureUdpSender(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartUdpSender(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopUdpSender(testId);
        }



    }
    public class RemoteUdpReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set;}
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }

        private Guid testId;
        private dynamic remoteInstance;

        public RemoteUdpReceiver(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteUdpReceiver()
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
                remoteInstance.DisposeUdpReceiver(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureUdpReceiver(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureUdpReceiver(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartUdpReceiver(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopUdpReceiver(testId);
        }



    }

    public class RemoteTcpSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return true;
            }
        }
        private Guid testId;
        private dynamic remoteInstance;

        public RemoteTcpSender(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteTcpSender()
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
                remoteInstance.DisposeTcpSender(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureTcpSender(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureTcpSender(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartTcpSender(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopTcpSender(testId);
        }



    }
    public class RemoteTcpReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return true;
            }
        }
        private Guid testId;
        private dynamic remoteInstance;

        public RemoteTcpReceiver(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteTcpReceiver()
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
                remoteInstance.DisposeTcpReceiver(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid,  string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureTcpReceiver(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureTcpReceiver(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartTcpReceiver(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopTcpReceiver(testId);
        }



    }

    public class RemoteMulticastSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }
        private Guid testId;
        private dynamic remoteInstance;

        public RemoteMulticastSender(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteMulticastSender()
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
                remoteInstance.DisposeMulticastSender(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureMulticastSender(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureMulticastSender(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartMulticastSender(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopMulticastSender(testId);
        }



    }
    public class RemoteMulticastReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }

        private Guid testId;
        private dynamic remoteInstance;

        public RemoteMulticastReceiver(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteMulticastReceiver()
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
                remoteInstance.DisposeMulticastReceiver(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureMulticastReceiver(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureMulticastReceiver(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartMulticastReceiver(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopMulticastReceiver(testId);
        }



    }

    public class RemoteBroadcastSender : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }
        private Guid testId;
        private dynamic remoteInstance;

        public RemoteBroadcastSender(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteBroadcastSender()
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
                remoteInstance.DisposeBroadcastSender(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureBroadcastSender(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureBroadcastSender(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartBroadcastSender(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopBroadcastSender(testId);
        }



    }
    public class RemoteBroadcastReceiver : IDataPathProtocolTest
    {
        public Int64 UnitsTransfered { get; private set; }
        public bool ReportAsBytes
        {
            get
            {
                return false;
            }
        }

        private Guid testId;
        private dynamic remoteInstance;

        public RemoteBroadcastReceiver(dynamic remoteInstance)
        {
            this.remoteInstance = remoteInstance;
        }

        ~RemoteBroadcastReceiver()
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
                remoteInstance.DisposeBroadcastReceiver(testId);
            }
        }

        public void Configure(TestMode testMode, Guid localInterfaceGuid, string address, bool ipv6, UInt16 port)
        {
            if (testMode == TestMode.Wlan)
            {
                testId = remoteInstance.ConfigureBroadcastReceiver(null, address, ipv6, port);
            }
            else
            {
                testId = remoteInstance.ConfigureBroadcastReceiver(localInterfaceGuid.ToString(), address, ipv6, port);
            }

        }
        public void Start()
        {
            remoteInstance.StartBroadcastReceiver(testId);
        }
        public void Stop()
        {
            UnitsTransfered = remoteInstance.StopBroadcastReceiver(testId);
        }



    }


}
