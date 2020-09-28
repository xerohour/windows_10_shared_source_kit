///
///
/// <copyright>
///  (C) Microsoft Corporation. All Rights Reserved.
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
using WEX.TestExecution.Markup;

namespace Microsoft.Test.Networking.Wireless.Outerloop
{

    public partial class SystemTests
    {
        [TestProperty("Kits.TestId", "38099FC3-B893-4165-9727-5D4CE70D479E")]
        [TestProperty("Kits.TestId2", "365d7beb-e1c6-4477-ad91-f736cbabcb6d")]
        [TestProperty("Kits.TestName", "Wlan Test - Query Interface Properties")]
        [TestProperty("Kits.Description", "Query Interface Properties")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "7CA44864-F951-440C-A651-94CA797AF859")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void DeviceTest_QueryInterfaceProperties()
        {
            Test_02_QueryInterfaceProperties();
        }

        [TestProperty("Kits.TestId", "1559752D-39A8-4B28-A79F-13EFF48DC06D")]
        [TestProperty("Kits.TestId2", "5f5c9607-eb30-43aa-9eb4-c5e994e33f29")]
        [TestProperty("Kits.TestName", "Wlan Test - Toggle airplane mode")]
        [TestProperty("Kits.Description", "Toggle airplane mode")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.RunElevated", "True")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "EF206565-9163-438D-BB56-F7F9B4455882")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_ToggleAirplaneMode()
        {
            Test_06_ToggleAirplaneMode();
        }
        [TestProperty("Kits.TestId", "65EAC5B5-219F-459A-A8BE-B53B96D96A28")]
        [TestProperty("Kits.TestId2", "1205c9a8-ac68-4ac7-8656-64efe85a663e")]
        [TestProperty("Kits.TestName", "Wlan Test - Scan for Networks")]
        [TestProperty("Kits.Description", "Scan for networks using the Wlan device and expect results")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "5C671923-3FFF-4CA9-9A61-036EB3B3F375")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_ScanForNetworks()
        {
            Test_08_ScanForNetworks();
        }

        [TestProperty("Kits.TestId", "2C57AC94-F46E-4E40-9E23-AE6B0A397542")]
        [TestProperty("Kits.TestId2", "1bdd0f33-1d2c-437c-8161-e0d1dc75fafa")]
        [TestProperty("Kits.TestName", "Wlan Test - Attempt a connection to a non-existent network")]
        [TestProperty("Kits.Description", "Attempts a connection to a non-existent profile")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "3664D3E6-751F-4B7C-B57A-02EBE628086E")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_NonExistentWlanProfileConnect()
        {
            Test_09_NonExistentWlanProfileConnect();
        }

        [TestProperty("Kits.TestId", "93197AC8-E6B8-4C2E-8252-B8AFEAD392A8")]
        [TestProperty("Kits.TestId2", "73436424-e997-46a8-9a9c-efb53a5c570e")]
        [TestProperty("Kits.TestName", "Wlan Test - Connect to a WPA2 PSK AES AP")]
        [TestProperty("Kits.Description", "Connect to a specified WPA2PSK AES network (Defaults: SSID=kitstestssid Passphrase=password)")]
        [TestProperty("Kits.ExpectedRuntime", "5")]
        [TestProperty("Kits.TimeoutInMinutes", "15")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_AES_SSID")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Description", "SSID of a WPA2_PSK_AES network to connect to (MUST be WPA2 PSK AES)")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Default", "kitstestssid")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsUserSettable", "True")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_Password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Description", "Password for WPA2_PSK network")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Default", "password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsUserSettable", "True")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "DC535BDE-D9D6-47E6-82BC-0F94272416D4")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_WlanConnectTest()
        {
            Test_10_WlanConnectTest();
        }

        [TestProperty("Kits.TestId", "2AE2CB26-EA04-4DEF-8E07-AE7B92E61E1B")]
        [TestProperty("Kits.TestId2", "0a737ffb-cbd2-41cd-a596-b939fbddc9ec")]
        [TestProperty("Kits.TestName", "Wlan Test - Connect to a WPA2 PSK AES AP with Sleep cycle")]
        [TestProperty("Kits.Description", "Connect to a specified WPA2PSK AES network with sleep resume (Defaults: SSID=kitstestssid Passphrase=password SleepDurationInSec=20, SleepCycleIterations=2)")]
        [TestProperty("Kits.ExpectedRuntime", "5")]
        [TestProperty("Kits.TimeoutInMinutes", "15")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_AES_SSID")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Description", "SSID of a WPA2_PSK_AES network to connect to (MUST be WPA2 PSK AES)")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Default", "kitstestssid")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsUserSettable", "True")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_Password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Description", "Password for WPA2_PSK network")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Default", "password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsUserSettable", "True")]
        [TestProperty("Kits.Parameter", "SleepDurationInSec")]
        [TestProperty("Kits.Parameter.SleepDurationInSec.Description", "The amount of time in seconds the device will asleep")]
        [TestProperty("Kits.Parameter.SleepDurationInSec.Default", "20")]
        [TestProperty("Kits.Parameter.SleepDurationInSec.IsRequired", "True")]
        [TestProperty("Kits.Parameter.SleepDurationInSec.IsUserSettable", "True")]
        [TestProperty("Kits.Parameter", "SleepCycleIterations")]
        [TestProperty("Kits.Parameter.SleepCycleIterations.Description", "The number of sleep/wake cycles to do")]
        [TestProperty("Kits.Parameter.SleepCycleIterations.Default", "2")]
        [TestProperty("Kits.Parameter.SleepCycleIterations.IsRequired", "True")]
        [TestProperty("Kits.Parameter.SleepCycleIterations.IsUserSettable", "True")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "DD82CA1B-FD19-4655-A1AE-1179B9E7D688")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_WlanConnectTestWithSleep()
        {
            Test_10_WlanConnectTest();
        }

        [TestProperty("Kits.TestId", "0DD672BA-55D9-4138-9B9A-129DB683506F")]
        [TestProperty("Kits.TestId2", "d24e1e1a-a659-4b60-8627-7ddbae382e3e")]
        [TestProperty("Kits.TestName", "Wlan Test - Check Hosted Network Capable")]
        [TestProperty("Kits.Description", "Checks that the device is capable of starting a hosted network (Soft AP)")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportHostedNetwork.HostedNetwork")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "E8A61651-2D52-474A-ACB5-290CCC959486")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void DeviceTest_CheckHostedNetwork()
        {
            Test_11_CheckHostedNetwork();
        }
        [TestProperty("Kits.TestId", "24f9bc8e-591d-4fa0-8dfd-735369428379")]
        [TestProperty("Kits.TestId2", "3d2f68b9-f4ce-43d1-87ec-898b008c4f12")]
        [TestProperty("Kits.TestName", "Wlan Test - Enable and Disable WFD Autonomous GO")]
        [TestProperty("Kits.Description", "Enables and disables Wifi-Direct Autonomous GO")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportWiFiDirect.WiFiDirect")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "C4B0532B-314E-4DE8-9DBE-5BE1495684A9")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void DeviceTest_EnableDisableAutoGO()
        {
            Test_12_EnableDisableAutoGO();
        }

        [TestProperty("Kits.TestId", "1949BD77-A684-463F-9512-6812657B8706")]
        [TestProperty("Kits.TestId2", "efdacab3-52bc-42a0-81ae-0cad900db51f")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device Capabilities Check")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device reported capabilities")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "14787C42-0CAD-49AC-9267-B05C8851C1AF")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_WlanCapabilitiesTest()
        {
            Test_13_WlanCapabilitiesTest();
        }
        [TestProperty("Kits.TestId", "ab3df3e5-1f37-46e4-9da9-a9663c803fb5")]
        [TestProperty("Kits.TestId2", "782846ed-7b82-4cd4-9bfb-4058ca4dc10f")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device Reset Check")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device has properly implemented the reset functionality")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "10")]
        [TestProperty("Kits.DisableDriverVerifier", "true")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "6CD757B9-CA2E-4488-8B50-A325B93924DA")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_WlanDeviceReset()
        {
            Test_14_WlanDeviceReset();
        }
        [TestProperty("Kits.TestId", "26568A8C-39CB-46EF-B8B8-E1BC835B05C1")]
        [TestProperty("Kits.TestId2", "f0a2e1d4-394d-4076-ac20-0a7d655f69b5")]
        [TestProperty("Kits.TestName", "Wlan Test - Query All Reported Oids")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device does not bugcheck")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "97678A51-82FF-4522-83F2-6B5A7652967B")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_QueryAllReportedOids()
        {
            Test_14_QueryAllReportedOids();
        }
        [TestProperty("Kits.TestId", "F27109B8-54CE-4279-8276-F5D2B8CB71F3")]
        [TestProperty("Kits.TestId2", "b08f847c-5bc1-45ea-b2da-3fa007429964")]
        [TestProperty("Kits.TestName", "Wlan Test - MAC Randomization test")]
        [TestProperty("Kits.Description", "Test MAC randomization against WPA2PSK AES network (Defaults: SSID=kitstestssid Passphrase=password)")]
        [TestProperty("Kits.ExpectedRuntime", "10")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportMACAddressRandomization.MACAddressRandomization")]
        [TestProperty("Kits.ExtendedDocumentation", "1C38D306-93A2-49D0-803B-9017CA6ED03A")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_AES_SSID")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Description", "SSID of a WPA2_PSK_AES network to connect to (MUST be WPA2 PSK AES)")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.Default", "kitstestssid")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_AES_SSID.IsUserSettable", "True")]
        [TestProperty("Kits.Parameter", "WPA2_PSK_Password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Description", "Password for WPA2_PSK network")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.Default", "password")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsRequired", "True")]
        [TestProperty("Kits.Parameter.WPA2_PSK_Password.IsUserSettable", "True")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void DeviceTest_MacRandomizationTest()
        {
            TestCase15_MacRandomizationTest();
        }

        [TestProperty("Kits.TestId", "B967267F-8E0A-43C1-9AB2-CC54D388E9C7")]
        [TestProperty("Kits.TestId2", "9a2117d6-c80b-40b7-9532-fc8e1e8cd8a1")]
        [TestProperty("Kits.TestName", "Wlan Test - Hotspot2 ANQP and GAS")]
        [TestProperty("Kits.Description", "Send and receive ANQP messages")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportHotspot2Dot0.Hotspot2Dot0")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "BAED6237-8107-4F80-9A4E-EB504AEFEDFB")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void DeviceTest_SendReceiveAnqpGasQuery()
        {
            Test_16_SendReceiveAnqpGasQuery();
        }
    }

}
