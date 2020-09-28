///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
namespace HlkTest.ScanningTests
{
    using System;
    using System.Text;
    using System.Diagnostics.CodeAnalysis;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;
    using System.Diagnostics;
    using System.Threading;
    using System.Net;
    using Microsoft.Test.Networking;
    using Microsoft.Test.Networking.Wireless;
    using Microsoft.Test.Networking.Dhcp;
    using Microsoft.Test.Networking.Kit;
    using Microsoft.Test.Networking.Kit.Tracing;


    [TestClass]
    public class ScanningTests
    {
        private static RunTimeConfiguration sConfig;
        
        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            sConfig = new RunTimeConfiguration(context);
        }

        public ScanningTests()
        {
        }

        [TestMethod]
        [Priority(2)]
        public void Scan_VariousAuthCipher() // WPA-Personal/TKIP, WPA-Entprise/AES; Open/None/Hidden, WPA2-Personal/AES
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.WPAPersonal;
            AP1Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP1Config24GHz.Phy = PhyType.b;
            AP1Config24GHz.Channel = 5;

            AP1Config5GHz.Auth = AuthenticationMethod.WPAEnterprise;
            AP1Config5GHz.Cipher = EncryptionMethod.AES;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 36;

            AP2Config24GHz.Auth = AuthenticationMethod.Open;
            AP2Config24GHz.Cipher = EncryptionMethod.None;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 11;
            AP2Config24GHz.IsHidden = true;   // Hidden

            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 44;

            WhckScanning.Scan(sConfig, 
                AP1Config24GHz, 
                AP1Config5GHz, 
                AP2Config24GHz, 
                AP2Config5GHz, 
                SCAN_TEST_OPTIONS.None);
        }

        [TestMethod]
        [Priority(2)]
        public void Scan_MultipleBSSID()     // Two Open and WPA2 Enterprise
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.Open;
            AP1Config24GHz.Cipher = EncryptionMethod.None;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 1;

            AP1Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP1Config5GHz.Cipher = EncryptionMethod.AES;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 44;

            AP2Config24GHz.Auth = AuthenticationMethod.Open;
            AP2Config24GHz.Cipher = EncryptionMethod.None;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 6;

            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 36;

            WhckScanning.Scan(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                SCAN_TEST_OPTIONS.DuplicateSSIDs);
        }

        [TestMethod]
        [Priority(2)]
        public void Scan_Custom()     // Custom
        {
            bool bParameterSpecified = true;
            SCAN_TEST_OPTIONS customOptions; 

            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            bParameterSpecified = AP1Config24GHz.Load(sConfig.TaefTestContext) && bParameterSpecified;
            bParameterSpecified = AP1Config5GHz.Load(sConfig.TaefTestContext) && bParameterSpecified;
            bParameterSpecified = AP2Config24GHz.Load(sConfig.TaefTestContext) && bParameterSpecified;
            bParameterSpecified = AP2Config5GHz.Load(sConfig.TaefTestContext) && bParameterSpecified;

            if (!bParameterSpecified)
            {
                // No parameters specified. Dont run the test
                return;
            }

            customOptions = (SCAN_TEST_OPTIONS)sConfig.WlanTestOptions;
            if (customOptions == 0)
                customOptions = SCAN_TEST_OPTIONS.None;

            WhckScanning.Scan(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                customOptions);
        }

        [TestMethod]
        [Priority(2)]
        public void Scan_ScanMix4()
        {
        }
    
    }
}
