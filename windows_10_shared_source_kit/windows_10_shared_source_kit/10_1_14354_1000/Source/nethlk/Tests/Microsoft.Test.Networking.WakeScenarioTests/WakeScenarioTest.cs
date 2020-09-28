///
///
/// <copyright>
///  Copyright (C) Microsoft. All rights reserved.
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

//skip due to phone no-need:
//skip wait for 5g in testsetup
//skip aoac check and damx check in testbody
//skip adapter disable in testbody (and reenable in cleanup)

using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.DeviceManager;
using Microsoft.Test.Networking.Tracing;
using Microsoft.Test.Networking.Power;
using Microsoft.Test.Networking.Oids;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using WEX.TestExecution.Markup;
using WEX.Logging.Interop;
using System.Globalization;

#pragma warning disable 0618

namespace HckTests
{
    [TestClass]
    public class WakeScenarioTests
    {
        private bool isCoreSku = false;
        private bool isLan = false;
        private Wlan wlanApi = null;
        private AdapterInfo testAdapterInfo = null;
        private Guid interfaceUnderTest = Guid.Empty;
		private UInt64 m_LuidUnderTest=0;
        private WakeEtwWatcher etwWatcher = null;
        private WakeScenarioHelper wakeHelper = null;
        private static String APServer = null;
        private static ApConfigClient apConfigClient = null;
        private static Collection<Guid> routers = new Collection<Guid>();
        private static APConfig apConfig = null;
        private WLAN_INTERFACE_STATE queryMediaValue;
        private List<Guid> disabledAdapters = null;
        private ConfiguredSSIDs configuredSSIDs = new ConfiguredSSIDs("", "");
        private List<AdapterInfo> allAdapters = IpHelpers.GetAddresses();
        private byte[] SrcIPv4Array = new byte[4];
        private byte[] DestIPv4Array = new byte[4];
        private byte[] SrcIPv6Array = new byte[16];
        private byte[] DestIPv6Array = new byte[16];
        private byte[] DestMACArray = new byte[6];
        private byte[] BitmapPatternArray;

        private double PktAmount = 1;
        private TimeSpan ManualTestTime = new TimeSpan(0, 0, 30);
        private TimeSpan D2SleepTime = new TimeSpan(0, 5, 0);
        private TimeSpan D2AL = new TimeSpan(0, 2, 30);
        private TimeSpan D2GTKdisconnect = new TimeSpan(0, 2, 30);  // must greater than EnterD2TimeOut
        private TimeSpan D2NLO = new TimeSpan(0, 2, 30);
        private TimeSpan D2PKT = new TimeSpan(0, 2, 30);  // must greater than EnterD2TimeOut
        private TimeSpan ConnectStatusTimeOut = new TimeSpan(0, 0, 60);
        ///private UInt32 EnterD2TimeOut = 60; //in sec        
        private int WaitForLeaveCS = 60; // in sec, set it to a large one for now to capture interest event when it happens outside CS
        private TimeSpan scanLength = new TimeSpan(0, 1, 30);
        private DateTime ExitSafeTimeForALNLO = DateTime.Now;
        private static bool DeviceSupports5Ghz = false;
        private static bool NoAPConfigApply = false;
        private static bool IsUtilTest = false;
        private static bool KeepConnection = false;
        private string UtilCommand = null;
        private bool protDriverInstalled = false;
        private int driverInitRetry = 3;
        private string DriverFullPath = "";

        private EventHandler<EventArrivedEventArgs> ETWCallback = null;
        private EventHandler<EventArrivedEventArgs> ETWPrint = null;

        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("WakeScenarioTests", false);

        TestContext context;
        public TestContext TestContext
        {
            get { return context; }
            set { context = value; }
        }

        [TestInitialize]
        public void TestSetup()
        {
            do
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Setup test at {0}", DateTime.Now));

                if (context.Properties.Contains("IsLan"))
                {
                    isLan = bool.Parse(context.Properties["IsLan"].ToString());                    
                }
                if (isLan)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test device is LAN"));
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test device is WLAN"));
                }

                //its necessary to check for core and if sku is undefined to pick up the odd one core based sku
                // Convert test to use in code prot driver loading for all sku's which will render this check unnecessary
                if (ProductInfo.Edition.ToString().ToUpper().Contains("CORE") || !System.Enum.IsDefined(ProductInfo.Edition.GetType(), ProductInfo.Edition))
                {
                    isCoreSku = true; 
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test platform is a core sku"));
                }                
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test platform is a client sku"));
                    
                }

                ETWCallback = (object sender, EventArrivedEventArgs properties) =>
                {
                    WakeHelper.LogEtwEvent(properties);
                };

                ETWPrint = (object sender, EventArrivedEventArgs properties) =>
                {
                    WakeHelper.PrintPropertyList(properties);
                };

                if (isLan)
                {
                    foreach (var a in allAdapters)
                    {
                        //when multiple present, use the first one as default
                        if (a.PhysicalMediumType == NDIS_PHYSICAL_MEDIUM.Medium802_3 && a.OperationalStatus == IF_OPER_STATUS.Up && InterfaceAndOperStatusFlags.HardwareInterface == (a.InterfaceAndOperStatusFlags & InterfaceAndOperStatusFlags.HardwareInterface))
                        {
                            testAdapterInfo = a;
                            interfaceUnderTest = a.AdapterGuid;
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Using lan adapter: {0}", interfaceUnderTest));
                            // there are non-static API been used from Wlan
                            wlanApi = new Wlan();
                            wakeHelper = new WakeScenarioHelper(wlanApi, interfaceUnderTest);
                            break;
                        }
                    }

                    if (interfaceUnderTest == Guid.Empty)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "No lan adapter found"));
                        break;
                    }
                }
                else
                {
                    foreach (var a in allAdapters)
                    {
                        //when multiple present, use the first one as default
                        if (a.PhysicalMediumType == NDIS_PHYSICAL_MEDIUM.Native802_11 && InterfaceAndOperStatusFlags.HardwareInterface == (a.InterfaceAndOperStatusFlags & InterfaceAndOperStatusFlags.HardwareInterface))
                        {
                            testAdapterInfo = a;
                            interfaceUnderTest = a.AdapterGuid;
							m_LuidUnderTest = a.InterfaceLuid;
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Using wlan adapter: {0}", interfaceUnderTest));
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Using wlan adapter: LUID={0}", m_LuidUnderTest));
                            wlanApi = new Wlan();
                            wakeHelper = new WakeScenarioHelper(wlanApi, interfaceUnderTest);
                            break;
                        }
                    }

                    if (interfaceUnderTest == Guid.Empty)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "No wlan adapter found"));
                        break;
                    }
                }

                etwWatcher = new WakeEtwWatcher(interfaceUnderTest, isLan);
				etwWatcher.SetLuidUnderTest( m_LuidUnderTest );
                //etwWatcher.EventArrived += ETWCallback;
                etwWatcher.ConnectSuccess += ETWCallback;
                etwWatcher.DisconnectSuccess += ETWCallback;
                etwWatcher.NicActiveAcquired += ETWCallback;
                etwWatcher.PowerOff += ETWCallback;
                etwWatcher.PowerOn += ETWCallback;
                etwWatcher.NicWakeReason += ETWCallback;

                if (context.Properties.Contains("KeepConnection"))
                {
                    KeepConnection = bool.Parse(context.Properties["KeepConnection"].ToString());
                }

                if (context.Properties.Contains("PktAmount"))
                {
                    PktAmount = double.Parse(context.Properties["PktAmount"].ToString(), CultureInfo.InvariantCulture);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Pkts to send is {0}", (int)PktAmount));
                }

                if (context.Properties.Contains("LengthInSec"))
                {
                    double LengthInSec = double.Parse(context.Properties["LengthInSec"].ToString(), CultureInfo.InvariantCulture);
                    D2SleepTime = TimeSpan.FromSeconds(LengthInSec);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Device sleep length changed to {0}", D2SleepTime));
                }

                if (context.Properties.Contains("LengthInMin"))
                {
                    double LengthInMin = double.Parse(context.Properties["LengthInMin"].ToString(), CultureInfo.InvariantCulture);
                    D2SleepTime = TimeSpan.FromMinutes(LengthInMin);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Device sleep length changed to {0}", D2SleepTime));
                }

                if (context.Properties.Contains("LengthInHour"))
                {
                    double LengthInHour = double.Parse(context.Properties["LengthInHour"].ToString(), CultureInfo.InvariantCulture);
                    D2SleepTime = TimeSpan.FromHours(LengthInHour);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Device sleep length changed to {0}", D2SleepTime));
                }

                if (context.Properties.Contains("DeviceSupports5Ghz"))
                {
                    DeviceSupports5Ghz = bool.Parse(context.Properties["DeviceSupports5Ghz"].ToString());
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "DeviceSupports5Ghz = {0}", DeviceSupports5Ghz));
                }

                if (context.Properties.Contains("IsUtilTest"))
                {
                    IsUtilTest = bool.Parse(context.Properties["IsUtilTest"].ToString());
                    if (IsUtilTest)
                    {
                        // to execute protocol driver related steps, manual test, util commands, no test setup required 
                        break;
                    }
                }

                if (context.Properties.Contains("DriverFullPath"))
                {
                    DriverFullPath = context.Properties["DriverFullPath"].ToString();
                    char lastChar = DriverFullPath[DriverFullPath.Length - 1];
                    if (lastChar != '\\' && lastChar != '/')
                    {
                        DriverFullPath = DriverFullPath + "/";
                    }
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Protocol driver is located at {0}", DriverFullPath));
                }
                
                // make sure button.sys is installed.  on non one core machines its installed with wdtf.  on onecore machines its up to the image creator to install it
                // this is just a check for this functionality before the test is run
                if (PowerManager.IsPowerButtonInstalled())
                {
                    testLogger.LogComment("Power button utility is installed. Continuing");
                }
                else
                {
                    testLogger.LogComment("Power button utility is not installed.  Test will fail with out it.");
                }

                if (isLan)
                {
                    // the rest of setup is for WLAN
                    break;
                }

                // same settings for both 2.4 and 5
                apConfig = WlanAccessPointHelper.CreateWirelessAccessPoint(AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.g, "12345678",
                                                                                        AuthenticationMethod.WPA2Personal, EncryptionMethod.AES, PhyType.n, "12345678");

                if (context.Properties.Contains("GTKInterval"))
                {
                    int GTKInterval = Int32.Parse(context.Properties["GTKInterval"].ToString(), CultureInfo.InvariantCulture);
                    apConfig.GTKRekey_24GHZ = GTKInterval;
                    apConfig.GTKRekey_5GHZ = GTKInterval;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Change GTK refresh interval to {0}", GTKInterval));
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "\t2.4 Ghz Settings \n\t\tTentative SSID: {0}, BSSID: {1}, PhyType: {2}, Channel: {3}, Auth/Cipher: {4}/{5}",
                        apConfig.SSID_24GHZ, apConfig.BSSID_24GHZ, apConfig.PhyType_24GHZ.ApConfigValue, apConfig.RadioChannel24Ghz,
                        apConfig.Authentication_24GHZ.ApConfigValue, apConfig.Encryption_24GHZ.ApConfigValue));

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "\t5 Ghz Settings \n\t\tTentative SSID: {0}, BSSID: {1}, PhyType: {2}, Channel: {3}, Auth/Cipher: {4}/{5}",
                        apConfig.SSID_5GHZ, apConfig.BSSID_5GHZ, apConfig.PhyType_5GHZ.ApConfigValue, apConfig.RadioChannel5Ghz,
                        apConfig.Authentication_5GHZ.ApConfigValue, apConfig.Encryption_5GHZ.ApConfigValue));

                if (context.Properties.Contains("APServer"))
                {
                    APServer = context.Properties["APServer"].ToString();
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "APServer Address = {0}", APServer));
                }

                if (APServer == null)
                {
                    testLogger.LogError("Did not get AP Server name as parameter");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Connecting to AP Config Server - {0}", APServer));
                apConfigClient = new ApConfigClient(APServer);
                bool APCCconnect = apConfigClient.Connect();
                if (APCCconnect)
                {
                    testLogger.LogComment("AP Config Server Connection Succeeded");
                }
                else
                {
                    testLogger.LogError("AP Config Server Connection failed");
                    break;
                }

                testLogger.LogComment("Enum Routers");
                routers = apConfigClient.EnumerateAccessPoints();
                if (routers == null)
                {
                    testLogger.LogError("Unable to enum routers, EnumerateAccessPoints returned null");
                    break;
                }
                if (routers.Count >= 2)
                {
                    testLogger.LogComment("Found 2 routers as expected and turn on radio in case it is not");
                    if (!apConfigClient.SetRadio24GHzState(routers[0], true))
                    {
                        testLogger.LogError("Failed to send radio on command, quit here");
                        break;
                    }
                }
                else
                {
                    testLogger.LogError("Did not find both APs");
                    break;
                }

                if (context.Properties.Contains("NoAPConfigApply"))
                {
                    NoAPConfigApply = bool.Parse(context.Properties["NoAPConfigApply"].ToString());
                    if (NoAPConfigApply)
                    {
                        // no setup step 
                        break;
                    }
                }

                wakeHelper.DeleteProfiles(isCoreSku);

                testLogger.LogComment("Applying Configuration");

                ConfiguredSSIDs configuredSSID = new ConfiguredSSIDs("", "");
                if (apConfigClient.ApplyConfiguration(routers[0], apConfig, out configuredSSID))
                {
                    testLogger.LogComment("Successfully applied the configuration");
                    apConfig.SSID_24GHZ = configuredSSID.TwoPoint4GhzSSID;
                    apConfig.SSID_5GHZ = configuredSSID.FiveghzSSID;

                    testLogger.LogComment("Configured 2.4GHz SSID: {0}", apConfig.SSID_24GHZ);
                    testLogger.LogComment("Configured 5GHz SSID: {0}", apConfig.SSID_5GHZ);
                }
                else
                {
                    testLogger.LogError("Unable to apply the configuration");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Waiting for AP beaconing for {0} on each supported band", scanLength));
                if (wakeHelper.IsApBeaconing(apConfig.SSID_24GHZ, scanLength))
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "AP {0} is found at {1}", apConfig.SSID_24GHZ, DateTime.Now));
                }
                else
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "AP {0} is not found", apConfig.SSID_24GHZ));
                    break;
                }
            }
            while (false);

            // testLogger.Flush();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Clearup test at {0}", DateTime.Now));
            do
            {
                // in case AL and NLO test finish before WlanScenario finish
                while (DateTime.Now < ExitSafeTimeForALNLO)
                {

                }

                if (disabledAdapters != null)
                {
                    allAdapters = IpHelpers.GetAddresses();
                    WakeScenarioHelper.EnableNoiseAdapters(allAdapters, disabledAdapters);
                }

                if (protDriverInstalled)
                {
                    if (!WakeScenarioHelper.RemoveAllWolPattern(interfaceUnderTest))
                    {
                        testLogger.LogError("Failed to remove existing patterns, check warning message");
                    }
                    testLogger.LogComment("Removing protocol driver");
                    WakeScenarioHelper.RemoveProtDriver();
                }

                // (isCoreSku)
                //
                //  if (apConfigClient != null)
                //  {
                //      testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Turn on radio in case test body failed to do so"));
                //      if (!apConfigClient.SetRadio24GHzState(routers[0], true))
                //      {
                //          testLogger.LogWarning("Failed to send radio on command, quit here");
                //      }
                //  }

                //  testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Leave CS in case still in D2"));
                //  WakeHelper.LeaveConnectedStandbyMode();
                //

                try
                {
                    if (etwWatcher != null)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Delete etwWatcher"));
                        etwWatcher.Dispose();
                        etwWatcher = null;
                    }
                }
                catch (System.Exception ex)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Error hit during classcleanup attempt to dispose etw watcher object: {0}", ex.ToString()));
                }

                if (isLan)
                {
                    break;
                }

                if (!KeepConnection)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Delete connection and profile"));
                    wakeHelper.DeleteProfiles(isCoreSku);
                }

                try
                {
                    if (wlanApi != null)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Delete wlanApi"));
                        wlanApi.Dispose();
                        wlanApi = null;
                    }
                }
                catch (System.Exception ex)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Error hit during classcleanup attempt to dispose wlan api object: {0}", ex.ToString()));
                }
            }
            while (false);

            // testLogger.Flush();
        }
           
        // util test
        [TestMethod]
        public void UtilTest()
        {
            do
            {
                if (context.Properties.Contains("UtilCommand"))
                {
                    UtilCommand = context.Properties["UtilCommand"].ToString();
                }
                else
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Please specify your command"));
                    break;
                }
                string[] commandParameters = UtilCommand.Split(new Char[] { ':' });

                switch (commandParameters[0])
                {
                    case "DisplayCapability":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start display wake capability at: {0}", DateTime.Now));
                        // by default random pattern which is supported on both Lan and Wlan
                        WoLSupportedPatterns testSupportedPattern = WoLSupportedPatterns.NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED;

                        if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, new Collection<WoLSupportedPatterns> { testSupportedPattern }))
                        {
                            testLogger.LogError("Failed to validate keyword support, check warning message");
                            break;
                        }
                        break;

                    case "WakePacket":
                        WakePatternType patternToSend = WakePatternType.RandomPattern;
                        if (commandParameters.Length < 2)
                        {
                            // by default using random pattern
                        }
                        else if (!Enum.IsDefined(typeof(WakePatternType), commandParameters[1]))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern type is wrong"));
                            break;
                        }
                        else
                        {
                            patternToSend = (WakePatternType)Enum.Parse(typeof(WakePatternType), commandParameters[1]);
                        }

                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start sending wake packet at: {0}", DateTime.Now));
                        if (!WakeScenarioHelper.PktSender(interfaceUnderTest, (int)PktAmount, patternToSend))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Failed to send packet"));
                            break;
                        }
                        break;

                    case "AddPattern":
                        WakePatternType patternToAdd = WakePatternType.RandomPattern;
                        if (commandParameters.Length < 2)
                        {
                            // by default using random pattern
                        }
                        else if (!Enum.IsDefined(typeof(WakePatternType), commandParameters[1]))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern type is wrong"));
                            break;
                        }
                        else
                        {
                            patternToAdd = (WakePatternType)Enum.Parse(typeof(WakePatternType), commandParameters[1]);
                        }
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start injecting pattern for {0} at: {1}", patternToAdd, DateTime.Now));
                        PM_WolPatternContext wolPatternContext = new PM_WolPatternContext(patternToAdd);
                        if (!WakeScenarioHelper.AddWolPattern(wolPatternContext, testAdapterInfo, out BitmapPatternArray))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be injected"));
                            break;
                        }
                        break;

                    case "RemovePattern":
                        if (commandParameters.Length < 2)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern Id not specified"));
                        }
                        else
                        {
                            uint patternId = UInt32.Parse(commandParameters[1], CultureInfo.InvariantCulture);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start removing pattern with id {0} at: {1}", patternId, DateTime.Now));
                            if (!WakeScenarioHelper.RemoveWolPattern(patternId, interfaceUnderTest))
                            {
                                testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be removed"));
                                break;
                            }
                        }
                        break;

                    case "QueryPatterns":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start querying patterns at: {0}", DateTime.Now));
                        List<NDIS_PM_WOL_PATTERN> allPatterns = new List<NDIS_PM_WOL_PATTERN> { };
                        if (!WakeScenarioHelper.QueryWolPatterns(allPatterns, interfaceUnderTest))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Patterns failed to be queried"));
                            break;
                        }
                        if (allPatterns.Count > 0)
                        {
                            foreach (var p in allPatterns)
                            {
                                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found pattern {0} with id {1}", p.WoLPacketType, p.PatternId));
                                if (p.WoLPacketType == NDIS_PM_WOL_PACKET.NdisPMWoLPacketBitmapPattern)
                                {

                                }
                                else if (p.WoLPacketType == NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv4TcpSyn)
                                {
                                    byte[] IPv4SrcAddr = new byte[4];
                                    byte[] IPv4DestAddr = new byte[4];
                                    IPV4_TCP_SYN_WOL_PACKET_PARAMETERS para = p.IPv4TcpSynParameters;
                                    unsafe
                                    {
                                        Marshal.Copy((IntPtr)para.IPv4SourceAddress, IPv4SrcAddr, 0, 4);
                                        Marshal.Copy((IntPtr)para.IPv4DestAddress, IPv4DestAddr, 0, 4);
                                    }

                                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Source IPv4 address: {0}.{1}.{2}.{3}", IPv4SrcAddr[0], IPv4SrcAddr[1], IPv4SrcAddr[2], IPv4SrcAddr[3]));
                                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Destination IPv4 address: {0}.{1}.{2}.{3}", IPv4DestAddr[0], IPv4DestAddr[1], IPv4DestAddr[2], IPv4DestAddr[3]));
                                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Source and destionation ports are: {0}, {1}", para.TCPSourcePortNumber, para.TCPDestPortNumber));
                                }
                                else if (p.WoLPacketType == NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv6TcpSyn)
                                {

                                }
                            }
                        }
                        break;

                    case "ProtInstall":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Install protocol driver for CoreSku at: {0}", DateTime.Now));
                        if (WakeNativeMethods.PhoneProtInstall())
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Protocol driver install success"));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Protocol driver install fails"));
                        }
                        break;

                    case "ProtUninstall":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Uninstall protocol driver for CoreSku at: {0}", DateTime.Now));
                        if (WakeNativeMethods.PhoneProtUninstall())
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Protocol driver uninstall success"));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Protocol driver uninstall fails"));
                        }
                        break;

                    case "ShowEnabledAdapters":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "List all adapters info at: {0}", DateTime.Now));
                        foreach (var a in allAdapters)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0}: GUID {1}, LUID {2}, Index {3}, Status {4}, Type {5}, MediaType {6}, PhysicalMediumType {7}, IsHardwareInterface {8}", a.Description, a.AdapterGuid, a.InterfaceLuid, a.InterfaceIndex, a.OperationalStatus.ToString(), a.Type.ToString(), a.MediaType.ToString(), a.PhysicalMediumType.ToString(), InterfaceAndOperStatusFlags.HardwareInterface == (a.InterfaceAndOperStatusFlags & InterfaceAndOperStatusFlags.HardwareInterface)));
                        }
                        break;

                    case "EnableAdapter":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start enabling adapter at: {0}", DateTime.Now));
                        if (commandParameters.Length < 2)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Adapter Guid not specified"));
                        }
                        else
                        {
                            Guid enableGuid = new Guid(commandParameters[1]);

                            if (WakeScenarioHelper.EnableAdapter(allAdapters, enableGuid))
                            {
                                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Enable success"));
                            }
                            else
                            {
                                testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Enable failed"));
                            }
                        }
                        break;

                    case "DisableAdapter":
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start disabling adapter at: {0}", DateTime.Now));
                        if (commandParameters.Length < 2)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Adapter Guid not specified"));
                        }
                        else
                        {
                            Guid disableGuid = new Guid(commandParameters[1]);

                            if (WakeScenarioHelper.DisableAdapter(allAdapters, disableGuid))
                            {
                                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disable success"));
                            }
                            else
                            {
                                testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Disable failed"));
                            }
                        }
                        break;

                    case "ManualTest":
                        if (commandParameters.Length > 1)
                        {
                            double testTime = Double.Parse(commandParameters[1], CultureInfo.InvariantCulture);
                            ManualTestTime = TimeSpan.FromSeconds(testTime);
                        }

                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start manual testing for {0} at: {1}", ManualTestTime, DateTime.Now));
                        etwWatcher.Start();
                        Wlan.Sleep((int)ManualTestTime.TotalMilliseconds);
                        break;

                    default:
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Command not support"));
                        break;
                }
            }
            while (false);

            // testLogger.Flush();
        }

        // test oids and max size
        [TestMethod]
        public void WakeSingleMachineTest()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting single machine tests at: {0}", DateTime.Now));

            do
            {
                Collection<WoLSupportedPatterns> supportedSPOnLan = new Collection<WoLSupportedPatterns> { WoLSupportedPatterns.NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED };
                Collection<WoLSupportedEvents> supportedSEOnLan = new Collection<WoLSupportedEvents> { };

                Collection<WoLSupportedPatterns> supportedSPOnWlan = new Collection<WoLSupportedPatterns> { WoLSupportedPatterns.NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED };
                Collection<WoLSupportedEvents> supportedSEOnWlan = new Collection<WoLSupportedEvents> { WoLSupportedEvents.NDIS_WLAN_WAKE_ON_NLO_DISCOVERY_SUPPORTED, WoLSupportedEvents.NDIS_WLAN_WAKE_ON_AP_ASSOCIATION_LOST_SUPPORTED, WoLSupportedEvents.NDIS_WLAN_WAKE_ON_GTK_HANDSHAKE_ERROR_SUPPORTED, WoLSupportedEvents.NDIS_WLAN_WAKE_ON_4WAY_HANDSHAKE_REQUEST_SUPPORTED };

                Collection<WoLSupportedPatterns> hardwareSP = new Collection<WoLSupportedPatterns> { };
                Collection<WoLSupportedEvents> hardwareSE = new Collection<WoLSupportedEvents> { };

                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Reading hardware caps"));
                if (!WakeScenarioHelper.GetPowerCap(interfaceUnderTest, NdisOids.OID_PM_HARDWARE_CAPABILITIES, hardwareSP, hardwareSE))
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Failed to query hardware cap"));
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating support caps are consistent among all oids"));
                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, hardwareSP))
                {
                    testLogger.LogError("Failed to validate pattern support, check warning message");
                }
                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, hardwareSE))
                {
                    testLogger.LogError("Failed to validate event support, check warning message");
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating must-support patterns and events are included"));
                if (isLan)
                {
                    foreach (var s in supportedSPOnLan)
                    {
                        if (!hardwareSP.Contains(s))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "{0} is not in the support list", s));
                        }
                    }
                    foreach (var s in supportedSEOnLan)
                    {
                        if (!hardwareSE.Contains(s))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "{0} is not in the support list", s));
                        }
                    }
                }
                else
                {
                    foreach (var s in supportedSPOnWlan)
                    {
                        if (!hardwareSP.Contains(s))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "{0} is not in the support list", s));
                        }
                    }
                    foreach (var s in supportedSEOnWlan)
                    {
                        if (!hardwareSE.Contains(s))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "{0} is not in the support list", s));
                        }
                    }
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Reading max support bitmap patterns"));
                byte[] bytes = OidQuery.GetBytesforOidGlobal(interfaceUnderTest, NdisOids.OID_PM_HARDWARE_CAPABILITIES);
                IntPtr buffer = Marshal.AllocHGlobal(bytes.Length);
                Marshal.Copy(bytes, 0, buffer, bytes.Length);
                NDIS_PM_CAPABILITIES PMCap = (NDIS_PM_CAPABILITIES)Marshal.PtrToStructure(buffer, typeof(NDIS_PM_CAPABILITIES));
                Marshal.FreeHGlobal(buffer);
                if (PMCap.NumTotalWoLPatterns >= 32)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Max amount of patterns is {0}", PMCap.NumTotalWoLPatterns));
                }
                else
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Max amount of patterns is {0}, but require amount is 32", PMCap.NumTotalWoLPatterns));
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating actual max support bitmap patterns by adding them"));
                if (!WakeScenarioHelper.RemoveAllWolPattern(interfaceUnderTest))
                {
                    testLogger.LogError("Failed to remove existing patterns, check warning message");
                    break;
                }
                for (int i = 0; i < PMCap.NumTotalWoLPatterns; i++)
                {
                    PM_WolPatternContext wolPatternContext = new PM_WolPatternContext(WakePatternType.RandomPattern);
                    if (!WakeScenarioHelper.AddWolPattern(wolPatternContext, testAdapterInfo, out BitmapPatternArray))
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be injected at {0} iteration", i + 1));
                        break;
                    }
                    Wlan.Sleep(1000);
                }
                if (!WakeScenarioHelper.RemoveAllWolPattern(interfaceUnderTest))
                {
                    testLogger.LogError("Failed to remove added patterns, check warning message");
                    break;
                }
            }
            while (false);

            // testLogger.Flush();
        }        

        [TestMethod]
        public void WakeByPKT()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Wake by incoming packet test at: {0}", DateTime.Now));
            do
            {
                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, new Collection<WoLSupportedPatterns> { WoLSupportedPatterns.NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED }))
                {
                    testLogger.LogError("Failed to validate keyword support, check warning message");
                    break;
                }
                Collection<WoLSupportedPatterns> hardwareSP = new Collection<WoLSupportedPatterns> { };
                Collection<WoLSupportedEvents> hardwareSE = new Collection<WoLSupportedEvents> { };
                if (!WakeScenarioHelper.GetPowerCap(interfaceUnderTest, NdisOids.OID_PM_HARDWARE_CAPABILITIES, hardwareSP, hardwareSE))
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Failed to query hardware cap"));
                    break;
                }

                if (isCoreSku)
                {
                    protDriverInstalled = true;
                    bool driverInit = false;
                    for (int i = 0; i < driverInitRetry; i++)
                    {
                        testLogger.LogComment("Attempting to install the protocol driver");
                        if (WakeScenarioHelper.InitProtDriver(DriverFullPath))
                        {
                            driverInit = true;
                            break;
                        }
                        else
                        {
                            WakeScenarioHelper.RemoveProtDriver();
                            testLogger.LogComment("Sleeping for 10 seconds, then retry");
                            Wlan.Sleep(10 * 1000);
                        }
                    }
                    
                    if (!driverInit)
                    {
                        testLogger.LogError("Failed to initial protocol driver, check warning message");
                        break;
                    }
                }

                if (!WakeScenarioHelper.RemoveAllWolPattern(interfaceUnderTest))
                {
                    testLogger.LogError("Failed to remove existing patterns, check warning message");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create profile and make the connection"));
                if (!wakeHelper.CreateProfileConnect(apConfig, isCoreSku, false))
                {
                    testLogger.LogError("Failed to make connection");
                    break;
                }
                allAdapters = IpHelpers.GetAddresses();
                foreach (var a in allAdapters)
                {
                    if (a.AdapterGuid == interfaceUnderTest)
                    {
                        testAdapterInfo = a;
                        break;
                    }
                }

                IPAddress ipv4 = null;
                foreach (var addr in testAdapterInfo.LocalAddresses)
                {
                    if (addr.AddrType == AddressFamily.AF_INET)
                    {
                        ipv4 = addr;
                        break;
                    }
                }
                if (ipv4 == null)
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "testAdapterInfo has no IPv4 address"));
                    break;
                }
                DestIPv4Array = IPAddress.GetMappedIPv4(ipv4);

                IPAddress ipv6 = null;
                foreach (var addr in testAdapterInfo.LocalAddresses)
                {
                    if (addr.AddrType == AddressFamily.AF_INET6)
                    {
                        ipv6 = addr;
                        break;
                    }
                }
                if (ipv6 == null)
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "testAdapterInfo has no IPv6 address"));
                    break;
                }
                DestIPv6Array = ipv6.ipv6Address;

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Connect AP to profile"));
                apConfigClient.ProfileConnect();

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Send destination MAC to AP"));
                DestMACArray = WakeScenarioHelper.GetMacAddress(interfaceUnderTest);
                if (DestMACArray == null)
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Failed to get local MAC address"));
                    break;
                }
                apConfigClient.SetDestMAC(DestMACArray);

                DateTime TestStartTime;
                DateTime SendTime = DateTime.Now;  // assign Now to avoid CS0165
                DateTime TestEndTime;  

                PM_WolPatternContext wolPatternContext;
                foreach (var sp in hardwareSP)
                {
                    // under developing
                    if (sp == WoLSupportedPatterns.NDIS_PM_WOL_MAGIC_PACKET_SUPPORTED || sp == WoLSupportedPatterns.NDIS_PM_WOL_EAPOL_REQUEST_ID_MESSAGE_SUPPORTED)
                    {
                        continue;
                    }

                    queryMediaValue = wlanApi.GetInterfaceState(interfaceUnderTest);
                    if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                    }
                    else
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                        break;
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Exchange pattern info and inject to driver for {0}", sp.ToString()));
                    if (sp == WoLSupportedPatterns.NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED)
                    { 
                        wolPatternContext = new PM_WolPatternContext(WakePatternType.RandomPattern);
                        if (!WakeScenarioHelper.AddWolPattern(wolPatternContext, testAdapterInfo, out BitmapPatternArray))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be injected"));
                            continue;
                        }
                        apConfigClient.SetBitmapPattern(BitmapPatternArray);   
                    }
                    else if (sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV4_TCP_SYN_SUPPORTED || sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV4_DEST_ADDR_WILDCARD_SUPPORTED)
                    {
                        SrcIPv4Array = apConfigClient.ExchangeIPv4(DestIPv4Array);
                        if (sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV4_TCP_SYN_SUPPORTED)
                        {
                            wolPatternContext = new PM_WolPatternContext(WakePatternType.IPv4_TCP_Syn);
                        }
                        else
                        {
                            wolPatternContext = new PM_WolPatternContext(WakePatternType.IPv4_Wildcard);
                        }
                        wolPatternContext.SrcIPv4 = SrcIPv4Array;
                        wolPatternContext.DestIPv4 = DestIPv4Array;
                        if (!WakeScenarioHelper.AddWolPattern(wolPatternContext, testAdapterInfo, out BitmapPatternArray))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be injected"));
                            continue;
                        }
                    }
                    else if (sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV6_TCP_SYN_SUPPORTED || sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV6_DEST_ADDR_WILDCARD_SUPPORTED)
                    {
                        SrcIPv6Array = apConfigClient.ExchangeIPv6(DestIPv6Array);
                        if (sp == WoLSupportedPatterns.NDIS_PM_WOL_IPV6_TCP_SYN_SUPPORTED)
                        {
                            wolPatternContext = new PM_WolPatternContext(WakePatternType.IPv6_TCP_Syn);
                        }
                        else
                        {
                            wolPatternContext = new PM_WolPatternContext(WakePatternType.IPv6_Wildcard);
                        }
                        wolPatternContext.SrcIPv6 = SrcIPv6Array;
                        wolPatternContext.DestIPv6 = DestIPv6Array;
                        if (!WakeScenarioHelper.AddWolPattern(wolPatternContext, testAdapterInfo, out BitmapPatternArray))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be injected"));
                            continue;
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Sceanrio: Incoming Packet {0}", sp.ToString()));
                    if (!apConfigClient.StartApScenario(routers[0], routers[1], sp.ToString(), (int)D2PKT.TotalSeconds, 1))
                    {
                        testLogger.LogError("Failed to run Wlanscenario");
                        continue;
                    }
                    SendTime = DateTime.Now + D2PKT;

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnect apconfig connection before disable LAN"));
                    apConfigClient.Disconnect();

                    allAdapters = IpHelpers.GetAddresses();
                    disabledAdapters = WakeScenarioHelper.DisableNoiseAdapters(allAdapters, interfaceUnderTest);

                    TestStartTime = DateTime.Now;

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start ETW Tracing and try to go to D2 at {0}", TestStartTime));
                    etwWatcher.Start();

                    // CS session
                    #if _OBSOLETE_
                    if (isCoreSku)
                    {
                        if (!WakeHelper.EnterConnectedStandbyMode(EnterD2TimeOut))
                        {
                            testLogger.LogError("Test failed to enter D2, quit here");
                            continue;
                        }
                        Wlan.Sleep((int)D2SleepTime.TotalMilliseconds);
                        TestEndTime = DateTime.Now;
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}, calling Leave CS", TestEndTime));
                        WakeHelper.LeaveConnectedStandbyMode();
                    }
                    else
                    #endif
                    {
                        TestEndTime = TestStartTime + D2SleepTime;

                        Wlan.StaticD2NQMSleep(D2SleepTime);
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}", DateTime.Now));
                    }

                    Wlan.Sleep(WaitForLeaveCS * 1000);
                    etwWatcher.Stop();

                    if (disabledAdapters != null)
                    {
                        allAdapters = IpHelpers.GetAddresses();
                        WakeScenarioHelper.EnableNoiseAdapters(allAdapters, disabledAdapters);
                        disabledAdapters = null;
                    }

                    testLogger.LogComment("Sleeping for 30 seconds after wake up");
                    Wlan.Sleep(30 * 1000); // 30s
                    bool APCCconnect = apConfigClient.Connect();
                    if (APCCconnect)
                    {
                        testLogger.LogComment("AP Config Server reconnection succeeded");
                    }
                    else
                    {
                        testLogger.LogError("AP Config Server reconnection failed");
                        break;
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Check error message from Scenario thread"));
                    string errorMessage = apConfigClient.GetScenarioError();
                    if (errorMessage != "No error")
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "AP: {0}", errorMessage));
                    }

                    WakeScenarioHelper.WakeTestSummary("IncomingPKT", etwWatcher, TestStartTime, SendTime, TestEndTime, isCoreSku);
                    etwWatcher.Cleanup();
                    if (!WakeScenarioHelper.RemoveAllWolPattern(interfaceUnderTest))
                    {
                        testLogger.LogError("Failed to remove existing patterns, check warning message");
                        break;
                    }
                }                
            }
            while (false);

            // testLogger.Flush();
        }               

        [TestMethod]
        public void WakeByGTKRefresh()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting WakeByGTKRefresh test at: {0}", DateTime.Now));

            do
            {
                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating wake capability"));
                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, new Collection<WoLSupportedEvents> { WoLSupportedEvents.NDIS_WLAN_WAKE_ON_GTK_HANDSHAKE_ERROR_SUPPORTED }))
                {
                    testLogger.LogError("Failed to validate keyword support, check warning message");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create profile and make the connection"));
                if (!wakeHelper.CreateProfileConnect(apConfig, isCoreSku, false))
                {
                    testLogger.LogError("Failed to make connection");
                    break;
                }                

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Sceanrio: GTKRefresh"));
                if (!apConfigClient.StartApScenario(routers[0], routers[1], "GTKRefresh", (int)D2GTKdisconnect.TotalSeconds, 1))
                {
                    testLogger.LogError("Failed to run Wlanscenario");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnect apconfig connection before disable LAN"));
                apConfigClient.Disconnect();

                disabledAdapters = WakeScenarioHelper.DisableNoiseAdapters(allAdapters, interfaceUnderTest);

                DateTime TestStartTime = DateTime.Now;
                DateTime TestEndTime;               
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start ETW Tracing and try to go to D2 at {0}", TestStartTime));
                etwWatcher.Start();

                // CS session
                #if _OBSOLETE_
                if (isCoreSku)
                {
                    if (!WakeHelper.EnterConnectedStandbyMode(EnterD2TimeOut))
                    {
                        testLogger.LogError("Test failed to enter D2, quit here");
                        break;
                    }
                    Wlan.Sleep((int)D2SleepTime.TotalMilliseconds);
                    TestEndTime = DateTime.Now;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}, calling Leave CS", TestEndTime));
                    WakeHelper.LeaveConnectedStandbyMode();
                }
                else
                #endif // 0 
                {
                    TestEndTime = TestStartTime + D2SleepTime;

                    Wlan.StaticD2NQMSleep(D2SleepTime);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}", DateTime.Now));
                }

                Wlan.Sleep(WaitForLeaveCS * 1000);
                //etwWatcher.Stop();

                if (disabledAdapters != null)
                {
                    allAdapters = IpHelpers.GetAddresses();
                    WakeScenarioHelper.EnableNoiseAdapters(allAdapters, disabledAdapters);
                    disabledAdapters = null;
                }

                testLogger.LogComment("Sleeping for 30 seconds after wake up");
                Wlan.Sleep(30 * 1000); // 30s
                bool APCCconnect = apConfigClient.Connect();
                if (APCCconnect)
                {
                    testLogger.LogComment("AP Config Server reconnection succeeded");
                }
                else
                {
                    testLogger.LogError("AP Config Server reconnection failed");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Check error message from Scenario thread"));
                string errorMessage = apConfigClient.GetScenarioError();
                if (errorMessage != "No error")
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "AP: {0}", errorMessage));
                }

                WakeScenarioHelper.WakeTestSummary("D2Sleep", etwWatcher, TestStartTime, DateTime.Now, TestEndTime, isCoreSku);
            }
            while (false);

            // testLogger.Flush();
        }

        [TestMethod]
        public void WakeByNLODiscovery()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting WakeByNLODiscovery test at: {0}", DateTime.Now));

            do
            {
                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating wake capability"));
                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, new Collection<WoLSupportedEvents> { WoLSupportedEvents.NDIS_WLAN_WAKE_ON_NLO_DISCOVERY_SUPPORTED }))
                {
                    testLogger.LogError("Failed to validate keyword support, check warning message");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create profile and make the connection"));
                if (!wakeHelper.CreateProfileConnect(apConfig, isCoreSku, true))
                {
                    testLogger.LogError("Failed to make connection");
                    break;
                }

                DateTime TestStartTime;
                DateTime RadioOnTime = DateTime.Now;  // assign Now to avoid CS0165
                DateTime TestEndTime;

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Sceanrio: SingleApSignalRecovered"));
                if (!apConfigClient.StartApScenario(routers[0], routers[1], "SingleApSignalRecovered", (int)D2NLO.TotalSeconds, 1))
                {
                    testLogger.LogError("Failed to run Wlanscenario");
                    break;
                }
                RadioOnTime = DateTime.Now + D2NLO;
                ExitSafeTimeForALNLO = DateTime.Now + D2NLO + D2NLO;

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnect apconfig connection before disable LAN"));
                apConfigClient.Disconnect();

                disabledAdapters = WakeScenarioHelper.DisableNoiseAdapters(allAdapters, interfaceUnderTest);

                if (!wakeHelper.WaitConnectStatusChange(false, ConnectStatusTimeOut))
                {
                    testLogger.LogError("Failed to disconnect");
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnect happened at {0}", DateTime.Now));
                }

                TestStartTime = DateTime.Now;
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start ETW Tracing and try to go to D2 at {0}", TestStartTime));
                etwWatcher.Start();

                // CS session
                #if _OBSOLETE_
                if (isCoreSku)
                {
                    if (!WakeHelper.EnterConnectedStandbyMode(EnterD2TimeOut))
                    {
                        testLogger.LogError("Test failed to enter D2, quit here");
                        break;
                    }
                    Wlan.Sleep((int)D2SleepTime.TotalMilliseconds);
                    TestEndTime = DateTime.Now;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}, calling Leave CS", TestEndTime));
                    WakeHelper.LeaveConnectedStandbyMode();
                }
                else
                #endif //0
                {
                    TestEndTime = TestStartTime + D2SleepTime;

                    WakeHelper.StaticD2Sleep(D2SleepTime);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}", DateTime.Now));
                }

                Wlan.Sleep(WaitForLeaveCS * 1000);
                //etwWatcher.Stop();

                if (disabledAdapters != null)
                {
                    allAdapters = IpHelpers.GetAddresses();
                    WakeScenarioHelper.EnableNoiseAdapters(allAdapters, disabledAdapters);
                    disabledAdapters = null;
                }

                testLogger.LogComment("Sleeping for 30 seconds after wake up");
                Wlan.Sleep(30 * 1000); // 30s
                bool APCCconnect = apConfigClient.Connect();
                if (APCCconnect)
                {
                    testLogger.LogComment("AP Config Server reconnection succeeded");
                }
                else
                {
                    testLogger.LogError("AP Config Server reconnection failed");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Check error message from Scenario thread"));
                string errorMessage = apConfigClient.GetScenarioError();
                if (errorMessage != "No error")
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "AP: {0}", errorMessage));
                }

                WakeScenarioHelper.WakeTestSummary("NLODiscovery", etwWatcher, TestStartTime, RadioOnTime, TestEndTime, isCoreSku);
            }
            while (false);

            // testLogger.Flush();
        }

        [TestMethod]
        public void WakeByAssociateLost()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting WakeByAssociateLost test at: {0}", DateTime.Now));

            do
            {
                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Validating wake capability"));
                if (!WakeScenarioHelper.ValidateWakeSupport(interfaceUnderTest, new Collection<WoLSupportedEvents> { WoLSupportedEvents.NDIS_WLAN_WAKE_ON_AP_ASSOCIATION_LOST_SUPPORTED }))
                {
                    testLogger.LogError("Failed to validate keyword support, check warning message");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create profile and make the connection"));
                if (!wakeHelper.CreateProfileConnect(apConfig, isCoreSku, false))
                {
                    testLogger.LogError("Failed to make connection");
                    break;
                }

                DateTime TestStartTime;
                DateTime RadioOffTime = DateTime.Now;  // assign Now to avoid CS0165
                DateTime TestEndTime;

                if (!isCoreSku)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting Sceanrio: SingleApSignalLoss"));
                    if (!apConfigClient.StartApScenario(routers[0], routers[1], "SingleApSignalLoss", (int)D2AL.TotalSeconds, 1))
                    {
                        testLogger.LogError("Failed to run Wlanscenario");
                        break;
                    }
                    RadioOffTime = DateTime.Now + D2AL;
                    ExitSafeTimeForALNLO = DateTime.Now + D2AL + D2AL;

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnect apconfig connection before disable LAN"));
                    apConfigClient.Disconnect();

                    disabledAdapters = WakeScenarioHelper.DisableNoiseAdapters(allAdapters, interfaceUnderTest);
                }

                TestStartTime = DateTime.Now;
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start ETW Tracing and try to go to D2 at {0}", TestStartTime));
                etwWatcher.Start();

                // CS session
                #if _OBSOLETE_
                if (isCoreSku)
                {
                    if (!WakeHelper.EnterConnectedStandbyMode(EnterD2TimeOut))
                    {
                        testLogger.LogError("Test failed to enter D2, quit here");
                        break;
                    }
                    Wlan.Sleep((int)D2SleepTime.TotalMilliseconds);
                    TestEndTime = DateTime.Now;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}, calling Leave CS", TestEndTime));
                    WakeHelper.LeaveConnectedStandbyMode();
                }
                else
                #endif //0
                {
                    TestEndTime = TestStartTime + D2SleepTime;

                    Wlan.StaticD2NQMSleep(D2SleepTime);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}", DateTime.Now));
                }

                Wlan.Sleep(WaitForLeaveCS * 1000);
                //etwWatcher.Stop();

                if (disabledAdapters != null)
                {
                    allAdapters = IpHelpers.GetAddresses();
                    WakeScenarioHelper.EnableNoiseAdapters(allAdapters, disabledAdapters);
                    disabledAdapters = null;
                }

                testLogger.LogComment("Sleeping for 30 seconds after wake up");
                Wlan.Sleep(30 * 1000); // 30s
                bool APCCconnect = apConfigClient.Connect();
                if (APCCconnect)
                {
                    testLogger.LogComment("AP Config Server reconnection succeeded");
                }
                else
                {
                    testLogger.LogError("AP Config Server reconnection failed");
                    break;
                }

                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Check error message from Scenario thread"));
                string errorMessage = apConfigClient.GetScenarioError();
                if (errorMessage != "No error")
                {
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "AP: {0}", errorMessage));
                }

                WakeScenarioHelper.WakeTestSummary("AssociateLost", etwWatcher, TestStartTime, RadioOffTime, TestEndTime, isCoreSku);
            }
            while (false);

            // testLogger.Flush();
        }

        [TestMethod]
        public void D2SleepResume()
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting D2SleepBVT test at: {0}", DateTime.Now));

            do
            {
                if (!PowerManager.IsAoAcCapableMachine())
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is NOT a AOAC device, skip the test"));
                    break;
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "This is a AOAC device"));
                }

                if (!isLan)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create profile and make the connection"));
                    if (!wakeHelper.CreateProfileConnect(apConfig, isCoreSku, false))
                    {
                        testLogger.LogError("Failed to make connection");
                        break;
                    }
                }

                disabledAdapters = WakeScenarioHelper.DisableNoiseAdapters(allAdapters, interfaceUnderTest);

                DateTime TestStartTime = DateTime.Now;
                DateTime TestEndTime;
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start ETW Tracing and try to go to D2 at {0}", TestStartTime));
                etwWatcher.Start();

                // CS session
                #if _OBSOLETE_
                if ( isCoreSku )
                {
                    if (!WakeHelper.EnterConnectedStandbyMode(EnterD2TimeOut))
                    {
                        testLogger.LogError("Test failed to enter D2, quit here");
                        break;
                    }
                    Wlan.Sleep((int)D2SleepTime.TotalMilliseconds);
                    TestEndTime = DateTime.Now;
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}, calling Leave CS", TestEndTime));
                    WakeHelper.LeaveConnectedStandbyMode();
                }
                else
                #endif // 0
                {
                    TestEndTime = TestStartTime + D2SleepTime;

                    Wlan.StaticD2NQMSleep(D2SleepTime);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep session ends at {0}", DateTime.Now));
                }

                Wlan.Sleep(WaitForLeaveCS * 1000);
                //etwWatcher.Stop();

                WakeScenarioHelper.WakeTestSummary("D2Sleep", etwWatcher, TestStartTime, DateTime.Now, TestEndTime, isCoreSku);
            }
            while (false);

            // testLogger.Flush();
        }
    }
}
