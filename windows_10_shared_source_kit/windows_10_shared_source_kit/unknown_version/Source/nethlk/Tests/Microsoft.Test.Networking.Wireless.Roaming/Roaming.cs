///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
namespace HlkTest.RoamingTests
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
    using AuthServer8021X;
    using Microsoft.Test.Networking;
    using Microsoft.Test.Networking.Wireless;
    using Microsoft.Test.Networking.Dhcp;
    using Microsoft.Test.Networking.Kit;
    using Microsoft.Test.Networking.Kit.Tracing;
    

    [TestClass]
    public class RoamingTests
    {
        private static RunTimeConfiguration sConfig;
        
        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            sConfig = new RunTimeConfiguration(context);
        }

        public RoamingTests()
        {
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [Priority(3)]
        public void Roam_OPEN_NONE()    // Public Wi-Fi AP's
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.Open;
            AP1Config24GHz.Cipher = EncryptionMethod.None;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 5;

            AP1Config5GHz.Auth = AuthenticationMethod.Open;
            AP1Config5GHz.Cipher = EncryptionMethod.None;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 36;

            AP2Config24GHz.Auth = AuthenticationMethod.Open;
            AP2Config24GHz.Cipher = EncryptionMethod.None;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 11;

            AP2Config5GHz.Auth = AuthenticationMethod.Open;
            AP2Config5GHz.Cipher = EncryptionMethod.None;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 44;

            WhckRoaming.Roam(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ROAM_TEST_OPTIONS.SuddenLossOfSignal | ROAM_TEST_OPTIONS.SuddenDropOfSignal);
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [Priority(2)]
        public void Roam_WPA2_PSK_AES()     // Home AP's (both bands)
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP1Config24GHz.Cipher = EncryptionMethod.AES;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 1;

            AP1Config5GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP1Config5GHz.Cipher = EncryptionMethod.AES;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 44;

            AP2Config24GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP2Config24GHz.Cipher = EncryptionMethod.AES;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 9;

            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 36;

            WhckRoaming.Roam(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ROAM_TEST_OPTIONS.SuddenLossOfSignal | ROAM_TEST_OPTIONS.GradualLossOfSignal);
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [Priority(3)]
        public void Roam_WPA2_ENTERPRISE_PEAP_TKIP_AES()    // Enterprise PEAP 
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP1Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 1;

            // Only first Enterprise AP's settings are used
            AP1Config24GHz.EapPhase1Type = SUPPORTED_PHASE1_TYPE.EAP_PEAP;
            AP1Config24GHz.EapPhase2Type = SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2;

            AP1Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP1Config5GHz.Cipher = EncryptionMethod.AES;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 44;

            AP2Config24GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 6;

            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 36;

            WhckRoaming.Roam(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ROAM_TEST_OPTIONS.SuddenLossOfSignal | ROAM_TEST_OPTIONS.GradualLossOfSignal);
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [Priority(3)]
        public void Roam_WPA2_ENTERPRISE_TTLS_TKIP_AES()    // Enterprise TTLS
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            AP1Config24GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP1Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 11;

            // Only first Enterprise AP's settings are used
            AP1Config24GHz.EapPhase1Type = SUPPORTED_PHASE1_TYPE.EAP_TTLS;
            AP1Config24GHz.EapPhase2Type = SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2;

            AP1Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP1Config5GHz.Cipher = EncryptionMethod.AES;
            AP1Config5GHz.Phy = PhyType.a;
            AP1Config5GHz.Channel = 40;

            AP2Config24GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 1;

            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 36;

            WhckRoaming.Roam(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ROAM_TEST_OPTIONS.SuddenLossOfSignal | ROAM_TEST_OPTIONS.GradualDropOfSignal);
        }

        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [Priority(2)]
        public void Roam_Custom()     // Custom
        {
            bool bParameterSpecified = true;
            ROAM_TEST_OPTIONS customOptions; 

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

            customOptions = (ROAM_TEST_OPTIONS)sConfig.WlanTestOptions;
            if (customOptions == 0)
                customOptions = ROAM_TEST_OPTIONS.SuddenLossOfSignal;

            WhckRoaming.Roam(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                customOptions);
        }        
    }
}
