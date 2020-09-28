///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    [TestClass]
    public class BasicPairingTests
    {
        static WiFiDirectTestController localWFDController;
        static WiFiDirectTestController remoteWFDController;

        static void CleanupTestControllers()
        {
            if (localWFDController != null)
            {
                localWFDController.Dispose();
                localWFDController = null;
            }

            if (remoteWFDController != null)
            {
                remoteWFDController.Dispose();
                remoteWFDController = null;
            }
        }

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            try
            {
                localWFDController = new WiFiDirectTestController(context);
                remoteWFDController = WiFiDirectTestUtilities.GetRemoteTestControllerFromTestContext(context);
            }
            catch(Exception)
            {
                CleanupTestControllers();
                throw;
            }
        }

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            CleanupTestControllers();
        }

        [TestInitialize]
        public void TestMethodSetup()
        {
            Verify.IsTrue( localWFDController.Setup() );
            Verify.IsTrue( remoteWFDController.Setup() );
        }

        [TestMethod]
        public void Invitation_PushButton()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecutePairingScenario(PairingScenarioType.Invitation, DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON);
            }
        }

        [TestMethod]
        public void GONegotiationDutBecomesGo_PinDisplay()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecutePairingScenario(PairingScenarioType.GoNegotiationDutBecomesGo, DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY);
            }
        }

        [TestMethod]
        public void GONegotiationDutBecomesClient_PushButton()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecutePairingScenario(PairingScenarioType.GoNegotiationDutBecomesClient, DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON);
            }
        }

        [TestMethod]
        public void JoinExistingGo_PinKeypad()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecutePairingScenario(PairingScenarioType.JoinExistingGo, DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD);
            }
        }

        void ExecutePairingScenario(PairingScenarioType pairingScenarioType, DOT11_WPS_CONFIG_METHOD configMethod)
        {
            PairingScenario pairingScenario = new PairingScenario(localWFDController, remoteWFDController, pairingScenarioType, configMethod, false, true);
            PairingScenarioResult pairingScenarioResult = pairingScenario.Execute();

            Verify.IsTrue(pairingScenarioResult.ScenarioSucceeded);
        }
    };
}
