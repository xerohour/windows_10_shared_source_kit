///
///
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///
///
/// <file>
///  $File$
/// </file>
///
/// <author>  $Author$ </author>
/// <date>    $Date$ </date>
/// <version> $Revision$ </version>
///
/// Information:
///
///
///
using System;
using System.Linq;
using System.Text;
using System.Diagnostics.CodeAnalysis;
using System.Diagnostics;
using System.Threading;
using System.Net;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.Sockets;
using Microsoft.Test.Networking.Dhcp;

namespace HlkTest.Stress
{
    
    [TestClass]
    public class WlanStress
    {
        private const int IntegrityCheckIntervalMin = 15;
        private const int DisconnectWaitSec = 10;
        private const int ProfileConnectWaitSec = 60;
        private const int ScanWaitSec = 20;

        private TimeSpan integrityCheckInterval;
        internal static TimeSpan DisconnectWait;
        internal static TimeSpan ConnectWait;
        internal static TimeSpan ScanWait;

        internal static String APServer = "";
        private static Collection<Guid> routers;

        private static string ServiceAPChannelAddress = "";

        internal static bool DeviceSupports5Ghz = true;
        private static int TestDuration = 60;

        private List<StressModule> StressModules;
        private Random random;
        private DateTime nextIntegrityCheckTime;
        private bool apConfigCreated;
        private string profileName1;
        private string profileName2;
        private bool failForNotEnoughTime;
        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("WlanStress");

        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            APServer = null;

            if (context.Properties.Contains("DeviceSupports5Ghz"))
            {
                DeviceSupports5Ghz = bool.Parse(context.Properties["DeviceSupports5Ghz"].ToString());
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "DeviceSupports5Ghz = {0}", DeviceSupports5Ghz));
            }

            if (context.Properties.Contains("APServer"))
            {
                APServer = context.Properties["APServer"].ToString();
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "APServer Name = {0}", APServer));
            }
            if(context.Properties.Contains("ServiceAPChannelAddress"))
            {
                ServiceAPChannelAddress = context.Properties["ServiceAPChannelAddress"].ToString();
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "ServiceAPChannelAddress = {0}", ServiceAPChannelAddress));
            }

            if(context.Properties.Contains("Duration"))
            {
                int duration;
                if(Int32.TryParse(context.Properties["Duration"].ToString(), out duration))
                {
                    TestDuration = duration;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Duration = {0} minutes", TestDuration));
                }
                else
                {
                    testLogger.LogComment("Could not parse Duration. Using default.");
                }
            }

            if (APServer == null)
            {
                testLogger.LogError("Did not get AP Server name as parameter");
                return;
            }


            testLogger.LogComment("Connecting to AP Config Server - " + APServer);
            using (ApConfigClient apConfigClient = new ApConfigClient(APServer))
            {
                bool connect = apConfigClient.Connect();
                if (connect)
                {
                    testLogger.LogComment("Connection Succeeded");
                }
                else
                {
                    testLogger.LogError("Connection failed");
                    return;
                }

                testLogger.LogComment("Enum Routers");
                routers = apConfigClient.EnumerateAccessPoints();
                if (routers == null)
                {
                    testLogger.LogError("Unable to enum routers, EnumerateAccessPoints returned null");
                    return;
                }
                if (routers.Count >= 2)
                {
                    testLogger.LogComment("Found 2 routers as expected");
                }
                else
                {
                    testLogger.LogError("Did not find both APs");
                    return;
                }
            }

        }


        public WlanStress()
        {
            random = new Random();

            integrityCheckInterval = new TimeSpan(0, IntegrityCheckIntervalMin, 0);
            DisconnectWait = new TimeSpan(0, 0, DisconnectWaitSec);
            ConnectWait = new TimeSpan(0, 0, ProfileConnectWaitSec);
            ScanWait = new TimeSpan(0, 0, ScanWaitSec);
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void Stress()
        {
            // Populate Stress Modules
            StressModules = new List<StressModule>();
            StressModules.Add(new ConnectDisconnectStress(random, routers, testLogger));
            StressModules.Add(new AirplaneModeStress(random, routers, testLogger));
            StressModules.Add(new ScanStress(random, routers, testLogger));
            //StressModules.Add(new SleepStress(random, routers, testLogger));
            apConfigCreated = false;

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Stress at : {0}", DateTime.Now));
            DateTime endTime = DateTime.Now.Add(new TimeSpan(0, TestDuration, 0));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Stress will end around : {0}", endTime));
            bool testFailed = false;
            if (TestDuration < 50)
            {
                failForNotEnoughTime = true;
                testLogger.LogError("The test will fail because it is not scheduled to run long enough.  Stress will still run but will ultimatly fail.  To fix this inrease the stress run time.");
            }
            else
            {
                failForNotEnoughTime = false;
            }
            
            if(RunIntegrityCheck() == false)
            {
                testLogger.LogError("Ending test because of failed integrity check.");
                return;
            }

            while(DateTime.Now < endTime)
            {
                if (nextIntegrityCheckTime < DateTime.Now)
                {
                    if(RunIntegrityCheck() == false)
                    {
                        testLogger.LogError("Ending test because of failed integrity check.");
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }


                StressModule stressModule = null;
                try
                {
                    stressModule = GetNextStressModule();
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting test {0}", stressModule.Name));
                    stressModule.Run(profileName1, profileName2);
                    // Wait a few seconds between stress run and next item
                    Wlan.Sleep(5000);
                }
                catch (Exception error)
                {
                    if (stressModule == null)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Exception when preparing test: {0}", error.ToString()));
                    }
                    else
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Exception when running test: {0} {1}", stressModule.Name, error.ToString()));
                    }
                    testFailed = true;
                    break;
                }
            }


            bool integrityCheck = true;
            if (RunIntegrityCheck() == false)
            {
                testLogger.LogError("Ending test because of failed integrity check.");
                integrityCheck = false;
            }

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Deleting Profile {0}", profileName1));
                wlanApi.TryDeleteProfile(wlanInterface.Id, profileName1);

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Deleting Profile {0}", profileName2));
                wlanApi.TryDeleteProfile(wlanInterface.Id, profileName2);
            }

            foreach (var stressModule in StressModules)
            {
                stressModule.PrintResults();
            }

            Verify.IsTrue(testFailed == false);
            Verify.IsTrue(integrityCheck == true);

            if (failForNotEnoughTime == true)
            {
                testLogger.LogError("The stress test failed because it was not scheduled to run at a long enough duration.  To fix this inrease the stress run time.");
            }
            Verify.IsTrue(failForNotEnoughTime == false);


        }

        
        private bool RunIntegrityCheck()
        {
            testLogger.LogComment("Running integrity check");

            UInt16 port = 7777;
            UInt16 streamBytes = Convert.ToUInt16(random.Next(10, 2000));
            nextIntegrityCheckTime = DateTime.Now.Add(integrityCheckInterval);
            using (ApConfigClient apConfigClient = new ApConfigClient(WlanStress.APServer))
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "WlanStress.APServer: {0}", WlanStress.APServer));
                int retries = 3;
                bool connected = false;
                while (retries > 0)
                {
                    retries--;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Attempting to connect to AP machine.  Retries remaining: {0}", retries));
                    bool connect = apConfigClient.Connect();
                    if (connect)
                    {
                        testLogger.LogComment("Connection to AP Server Succeeded");
                        connected = true;
                        break;
                    }
                    else
                    {
                        testLogger.LogComment("Connection AP Server failed");
                        Wlan.Sleep(10000);
                    }

                }
                if (connected == false)
                {
                    testLogger.LogError("Giving up connecting to AP machine");
                    return false;
                }

                apConfigClient.StartEchoServer(port, streamBytes, 1);

                ConnectForIntegrityCheck(apConfigClient, port, streamBytes);

                try
                {

                    using (Wlan wlanApi = new Wlan())
                    {
                        var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                        Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                        var wlanInterface = wlanInterfaceList[0];

                        using (Win32Sockets win32Sockets = new Win32Sockets())
                        {
                            string localAddress = win32Sockets.FindIpAddress(wlanInterface.Id);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "IPAddress: {0}", localAddress));

                            using (TCPEchoClient echoClient = new TCPEchoClient(localAddress, ServiceAPChannelAddress, port, streamBytes))
                            {
                                echoClient.Connect();
                                echoClient.PerformEcho();
                                bool result = echoClient.IsResponseBufferValid();
                                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Echo Client Success: {0}", result));
                                return result;

                            }

                        }
                    }



                }
                catch (Exception error)
                {
                    testLogger.LogComment("TlukeTest:  Exception hit");
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Error While trying data path: {0}", error.ToString()));
                    return false;

                }
                finally
                {
                    testLogger.LogComment("Cleaning Up Each Server");
                    apConfigClient.StopEchoServer();

                    using (Wlan wlanApi = new Wlan())
                    {
                        var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                        Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                        var wlanInterface = wlanInterfaceList[0];

                        if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                        {
                            wlanApi.Disconnect(wlanInterface.Id, DisconnectWait);
                        }


                    }
                }



            }




        }

        private void ConnectForIntegrityCheck(ApConfigClient apConfigClient, int port, int streamBytes)
        {
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    return;
                }

                if (apConfigCreated == false)
                {
                    var currentConfig1 = WlanAccessPointHelper.CreateWirelessAccessPoint(AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password", AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password");
                    var currentConfig2 = WlanAccessPointHelper.CreateWirelessAccessPoint(AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password", AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "Password");

                    ConfiguredSSIDs configuredSSID1 = new ConfiguredSSIDs("", "");
                    ConfiguredSSIDs configuredSSID2 = new ConfiguredSSIDs("", "");

                    if (apConfigClient.ApplyConfiguration(routers[0], currentConfig1, out configuredSSID1))
                    {
                        currentConfig1.SSID_24GHZ = configuredSSID1.TwoPoint4GhzSSID;
                        currentConfig1.SSID_5GHZ = configuredSSID1.FiveghzSSID;
                        testLogger.LogComment("Successfully applied the Configuration to AP[0]");
                    }
                    else
                    {
                        testLogger.LogError("Unable to apply the configuration to AP [0]");
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Applying AP Configuration to AP[1] {0}", currentConfig2.SSID_24GHZ));
                    if (apConfigClient.ApplyConfiguration(routers[1], currentConfig2, out configuredSSID2))
                    {
                        currentConfig2.SSID_24GHZ = configuredSSID1.TwoPoint4GhzSSID;
                        currentConfig2.SSID_5GHZ = configuredSSID1.FiveghzSSID;
                        testLogger.LogComment("Successfully applied the Configuration to AP[2]");
                    }
                    else
                    {
                        testLogger.LogError("Unable to apply the configuration to AP[2]");
                    }

                    string profile1;
                    string profile2;

                    profile1 = currentConfig1.CreateProfile24ghz(ConnectionMode.Manual, false);
                    profileName1 = currentConfig1.SSID_24GHZ;

                    profile2 = currentConfig2.CreateProfile24ghz(ConnectionMode.Manual, false);
                    profileName2 = currentConfig2.SSID_24GHZ;

                    testLogger.LogComment(profile1);

                    wlanApi.CreateProfile(wlanInterface.Id, profile1);
                    wlanApi.CreateProfile(wlanInterface.Id, profile2);

                    testLogger.LogComment("Wait to allow APs to beacon");
                    Wlan.Sleep(10000);

                    apConfigCreated = true;

                }

                int retries = 3;

                while (retries > 0)
                {
                    retries--;

                    testLogger.LogComment("Scan");
                    wlanApi.TryScan(wlanInterface.Id, false, ScanWait);

                    try
                    {
                        testLogger.LogComment(String.Format(CultureInfo.InvariantCulture, "Attempt to Connect to SSID : {0}", profileName1));
                        wlanApi.ProfileConnect(wlanInterface.Id, profileName1, ConnectWait);
                        break;

                    }
                    catch (Exception)
                    {
                        testLogger.LogComment("Connect failed.");
                        if (retries == 0)
                        {
                            testLogger.LogError(String.Format(CultureInfo.InvariantCulture, "Could not connect to SSID: {0}", profileName1));
                            return;
                        }
                    }
                    

                }
                
                testLogger.LogComment("Wait for DHCP");
                Microsoft.Test.Networking.Kit.Helpers.WaitForWlanConnectivity(wlanInterface.Id, 1000 * 15, testLogger);


            }
        }

        private StressModule GetNextStressModule()
        {
            return StressModules[random.Next(0, StressModules.Count)];
        }




    }


}
