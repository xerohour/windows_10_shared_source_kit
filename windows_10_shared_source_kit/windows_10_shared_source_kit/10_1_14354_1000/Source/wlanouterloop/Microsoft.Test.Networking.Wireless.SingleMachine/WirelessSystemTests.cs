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

        [TestProperty("Kits.TestId", "F0F57674-F91C-4285-9E65-7A5ABAA5A482")]
        [TestProperty("Kits.TestId2", "b5abc8f4-d985-466d-97e2-031eda44cd88")]
        [TestProperty("Kits.TestName", "Wlan Test - Query Interface Properties")]
        [TestProperty("Kits.Description", "Query Interface Properties")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "7CA44864-F951-440C-A651-94CA797AF859")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void SystemTest_QueryInterfaceProperties()
        {
            Test_02_QueryInterfaceProperties();
        }
        [TestProperty("Kits.TestId", "EF5E9BB9-3BB5-46C3-BCE6-F1542B06906B")]
        [TestProperty("Kits.TestId2", "f248aee1-aada-44d2-8b8d-f03ffbeacef3")]
        [TestProperty("Kits.TestName", "Wlan Test - Toggle airplane mode")]
        [TestProperty("Kits.Description", "Toggle airplane mode")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.RunElevated", "True")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.ExtendedDocumentation", "EF206565-9163-438D-BB56-F7F9B4455882")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_ToggleAirplaneMode()
        {
            Test_06_ToggleAirplaneMode();
        }
        [TestProperty("Kits.TestId", "D99B9F5B-0B2E-452C-8075-305F8F6F6C56")]
        [TestProperty("Kits.TestId2", "9bb11845-6a1c-4bd4-8646-65e386ef50df")]
        [TestProperty("Kits.TestName", "Wlan Test - Scan for Networks")]
        [TestProperty("Kits.Description", "Scan for networks using the Wlan device and expect results")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.ExtendedDocumentation", "5C671923-3FFF-4CA9-9A61-036EB3B3F375")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_ScanForNetworks()
        {
            Test_08_ScanForNetworks();
        }
        [TestProperty("Kits.TestId", "CCBE843E-31DC-4B31-803E-18A4D21B8469")]
        [TestProperty("Kits.TestId2", "32cc3c32-08a1-4e38-9eed-ad62f12eeb80")]
        [TestProperty("Kits.TestName", "Wlan Test - Attempt a connection to a non-existent network")]
        [TestProperty("Kits.Description", "Attempts a connection to a non-existent profile")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Scenario")]
        [TestProperty("Kits.ExtendedDocumentation", "3664D3E6-751F-4B7C-B57A-02EBE628086E")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_NonExistentWlanProfileConnect()
        {
            Test_09_NonExistentWlanProfileConnect();
        }
        [TestProperty("Kits.TestId", "CD739BC6-FC0E-403A-96A0-D6649FE6D374")]
        [TestProperty("Kits.TestId2", "11305d2f-35d1-449e-9e3b-5f9b897c2607")]
        [TestProperty("Kits.TestName", "Wlan Test - Connect to a WPA2 PSK AES AP")]
        [TestProperty("Kits.Description", "Connect to a specified WPA2PSK AES network (Defaults: SSID=kitstestssid Passphrase=password)")]
        [TestProperty("Kits.ExpectedRuntime", "5")]
        [TestProperty("Kits.TimeoutInMinutes", "15")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
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
        [TestProperty("Kits.ExtendedDocumentation", "DC535BDE-D9D6-47E6-82BC-0F94272416D4")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_WlanConnectTest()
        {
            Test_10_WlanConnectTest();
        }
        [TestProperty("Kits.TestId", "EC6E3701-5E69-48AD-BF4D-E3100C504B2C")]
        [TestProperty("Kits.TestId2", "a6f72491-07de-47e4-8701-b34a975a554c")]
        [TestProperty("Kits.TestName", "Wlan Test - Connect to a WPA2 PSK AES AP with Sleep cycle")]
        [TestProperty("Kits.Description", "Connect to a specified WPA2PSK AES network with sleep resume (Defaults: SSID=kitstestssid Passphrase=password SleepDurationInSec=20, SleepCycleIterations=2)")]
        [TestProperty("Kits.ExpectedRuntime", "5")]
        [TestProperty("Kits.TimeoutInMinutes", "15")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
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
        [TestProperty("Kits.ExtendedDocumentation", "DD82CA1B-FD19-4655-A1AE-1179B9E7D688")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_WlanConnectTestWithSleep()
        {
            Test_10_WlanConnectTest();
        }
        [TestProperty("Kits.TestId", "DC2222BD-B44F-45B0-AAB7-03D022E4FE96")]
        [TestProperty("Kits.TestId2", "046935c4-7662-4342-842e-0c3b31ad99af")]
        [TestProperty("Kits.TestName", "Wlan Test - Check Hosted Network Capable")]
        [TestProperty("Kits.Description", "Checks that the device is capable of starting a hosted network (Soft AP)")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.HostedNetwork.WlanHostedNetwork")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "E8A61651-2D52-474A-ACB5-290CCC959486")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void SystemTest_CheckHostedNetwork()
        {
            Test_11_CheckHostedNetwork();
        }

        [TestProperty("Kits.TestId", "D7BB7C94-10CF-4275-83CF-2D1CFB8A2A1A")]
        [TestProperty("Kits.TestId2", "35d12db3-45c6-47e0-aab4-6d1b2e22ab6d")]
        [TestProperty("Kits.TestName", "Wlan Test - Enable and Disable WFD Autonomous GO")]
        [TestProperty("Kits.Description", "Enables and disables Wifi-Direct Autonomous GO")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.WiFiDirect.WlanWiFiDirect")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "C4B0532B-314E-4DE8-9DBE-5BE1495684A9")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        public void SystemTest_EnableDisableAutoGO()
        {
            Test_12_EnableDisableAutoGO();
        }
        [TestProperty("Kits.TestId", "F652C855-E7C6-424D-8361-773C422A2456")]
        [TestProperty("Kits.TestId2", "d92acb18-4445-47cd-91fe-b05d60a6d036")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device Capabilities Check")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device reported capabilities")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "14787C42-0CAD-49AC-9267-B05C8851C1AF")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_WlanCapabilitiesTest()
        {
            Test_13_WlanCapabilitiesTest();
        }
        [TestProperty("Kits.TestId", "0E1D7D53-F0E3-4D28-9E5D-9EC28F3E04E9")]
        [TestProperty("Kits.TestId2", "665d8378-8865-40a1-839e-a50cf303919d")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device Reset Check")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device has properly implemented the reset functionality")]
        [TestProperty("Kits.ExpectedRuntime", "2")]
        [TestProperty("Kits.TimeoutInMinutes", "10")]
        [TestProperty("Kits.DisableDriverVerifier", "true")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.HangDetectionAndRecovery.WlanHangDetectionAndRecovery")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "6CD757B9-CA2E-4488-8B50-A325B93924DA")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_WlanDeviceReset()
        {
            Test_14_WlanDeviceReset();
        }
        [TestProperty("Kits.TestId", "C557859A-9C07-4DBD-8851-1B752629222F")]
        [TestProperty("Kits.TestId2", "4849e50f-5cd0-404b-9602-fd90ef2f49a9")]
        [TestProperty("Kits.TestName", "Wlan Test - Query All Reported Oids")]
        [TestProperty("Kits.Description", "Check the reported Wlan Device does not bugcheck")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "97678A51-82FF-4522-83F2-6B5A7652967B")]
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_QueryAllReportedOids()
        {
            Test_14_QueryAllReportedOids();
        }
        [TestProperty("Kits.TestId", "8AE23647-35B9-4535-9C5E-465281D4DE61")]
        [TestProperty("Kits.TestId2", "fb04abc8-5f4d-45a1-98c8-a837e167fd64")]
        [TestProperty("Kits.TestName", "Wlan Test - MAC Randomization test")]
        [TestProperty("Kits.Description", "Test MAC randomization against WPA2PSK AES network (Defaults: SSID=kitstestssid Passphrase=password)")]
        [TestProperty("Kits.ExpectedRuntime", "10")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.SupportMACAddressRandomization.MACAddressRandomization")]
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
        [TestProperty("Kits.CorePackageComposition", "OneCoreUAP")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        public void SystemTest_MacRandomizationTest()
        {
            TestCase15_MacRandomizationTest();
        }
    }
}
