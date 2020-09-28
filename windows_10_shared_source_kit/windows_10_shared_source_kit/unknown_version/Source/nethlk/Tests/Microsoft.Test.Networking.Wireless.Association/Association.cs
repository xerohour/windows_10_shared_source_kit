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
namespace HlkTest.AssociationTests
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
    public class AssociationTests
    {
        private static RunTimeConfiguration sConfig;
        
        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            sConfig = new RunTimeConfiguration(context);
        }

        public AssociationTests()
        {
        }

        [TestMethod]
        [Priority(2)]
        [TestProperty("ThreadingModel", "MTA")]
        public void Associate_WPA2_PSK_PEAP_AES()
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            //AP1 2.4 GHz
            AP1Config24GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP1Config24GHz.Cipher = EncryptionMethod.AES;
            AP1Config24GHz.Phy = PhyType.n;
            AP1Config24GHz.Channel = 6;

            //AP1 5GHz is never used

            //AP2 2.4 GHz
            AP2Config24GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config24GHz.Cipher = EncryptionMethod.AES;
            AP2Config24GHz.Phy = PhyType.n;
            AP2Config24GHz.Channel = 11;

            //AP2 5 GHz
            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Enterprise;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.a;
            AP2Config5GHz.Channel = 157;

            //Enterprise on second router.
            AP2Config24GHz.EapPhase1Type = AuthServer8021X.SUPPORTED_PHASE1_TYPE.EAP_PEAP;
            AP2Config24GHz.EapPhase2Type = AuthServer8021X.SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2;
            AP2Config5GHz.EapPhase1Type = AuthServer8021X.SUPPORTED_PHASE1_TYPE.EAP_PEAP;
            AP2Config5GHz.EapPhase2Type = AuthServer8021X.SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2;

            WhckAssociation.Associate(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ASSOC_TEST_OPTIONS.None);
            }

        [TestMethod]
        [Priority(2)]
        [TestProperty("ThreadingModel", "MTA")]
        public void Associate_Custom() //Custom Mix
        {
            bool bParameterSpecified = true;
            ASSOC_TEST_OPTIONS customOptions; 

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

            customOptions = (ASSOC_TEST_OPTIONS)sConfig.WlanTestOptions;
            if (customOptions == 0)
                customOptions = ASSOC_TEST_OPTIONS.None;

            WhckAssociation.Associate(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                customOptions);
        }

        [TestMethod]
        [Priority(2)]
        [TestProperty("ThreadingModel", "MTA")]
        public void Associate_OPEN_WPA2_AES() //Sleep/CS and hibernate
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            //AP1 2.4 GHz
            AP1Config24GHz.Auth = AuthenticationMethod.Open;
            AP1Config24GHz.Cipher = EncryptionMethod.None;
            AP1Config24GHz.Phy = PhyType.g;
            AP1Config24GHz.Channel = 1;

            //AP1 5GHz is never used

            //AP2 2.4 GHz
            AP2Config24GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP2Config24GHz.Cipher = EncryptionMethod.AES;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 11;

            //AP2 5 GHz
            AP2Config5GHz.Auth = AuthenticationMethod.WPA2Personal;
            AP2Config5GHz.Cipher = EncryptionMethod.AES;
            AP2Config5GHz.Phy = PhyType.n;
            AP2Config5GHz.Channel = 36;

            WhckAssociation.Associate(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ASSOC_TEST_OPTIONS.Sleep | ASSOC_TEST_OPTIONS.Hibernate);
        }

        [TestMethod]
        [Priority(2)]
        [TestProperty("ThreadingModel", "MTA")]
        public void Associate_OPEN_WEP_WPA_TKIP()
        {
            APConfigParameter AP1Config24GHz = new APConfigParameter(0, false);
            APConfigParameter AP1Config5GHz = new APConfigParameter(0, true);
            APConfigParameter AP2Config24GHz = new APConfigParameter(1, false);
            APConfigParameter AP2Config5GHz = new APConfigParameter(1, true);

            //Because WEP is not supported on 5GHz AP1 2.4 needs to be WEP and AP2 (both) needs to be WPA TKIP.
            //AP1 5GHz is never used

            //AP1 2.4 GHz
            AP1Config24GHz.Auth = AuthenticationMethod.Open;
            AP1Config24GHz.Cipher = EncryptionMethod.WEP40;
            AP1Config24GHz.Phy = PhyType.b;
            AP1Config24GHz.Channel = 1;
            AP1Config24GHz.IsHidden = true;

            //AP1 5 GHz is not used.

            //AP2 2.4 GHz
            AP2Config24GHz.Auth = AuthenticationMethod.WPAPersonal;
            AP2Config24GHz.Cipher = EncryptionMethod.TKIP;
            AP2Config24GHz.Phy = PhyType.g;
            AP2Config24GHz.Channel = 11;

            //AP2 5 GHz
            AP2Config5GHz.Auth = AuthenticationMethod.WPAPersonal;
            AP2Config5GHz.Cipher = EncryptionMethod.TKIP;
            AP2Config5GHz.Phy = PhyType.a;
            AP2Config5GHz.Channel = 36;

            WhckAssociation.Associate(sConfig,
                AP1Config24GHz,
                AP1Config5GHz,
                AP2Config24GHz,
                AP2Config5GHz,
                ASSOC_TEST_OPTIONS.None);
        }
    }
}
