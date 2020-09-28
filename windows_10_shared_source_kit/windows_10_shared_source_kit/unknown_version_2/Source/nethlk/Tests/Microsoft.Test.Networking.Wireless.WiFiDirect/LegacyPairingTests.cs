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
    public class LegacyPairingTests
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
            Verify.IsTrue( localWFDController.Reset() );    // No AP connection allowed here
            Verify.IsTrue( remoteWFDController.Setup() );
        }

        [TestMethod]
        public void LegacyPairing()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                LegacyPairingScenario legacyPairingScenario = new LegacyPairingScenario(localWFDController, remoteWFDController);
                LegacyPairingScenarioResult legacyPairingScenarioResult = legacyPairingScenario.Execute();

                Verify.IsTrue(legacyPairingScenarioResult.ScenarioSucceeded);
            }
        }
    };
}
