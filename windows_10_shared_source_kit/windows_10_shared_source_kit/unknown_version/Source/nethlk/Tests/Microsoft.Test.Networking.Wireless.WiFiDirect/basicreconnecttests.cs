///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Threading.Tasks;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    [TestClass]
    public class BasicReconnectTests
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
        public void SendReinvokeInvitationToClient()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteReconnectScenario(ReconnectScenarioType.SendReinvokeInvitationToClient);
            }
        }

        [TestMethod]
        public void SendReinvokeInvitationToGo()
        {
            //
            // Temporary workaround for bug 3464582.
            // The DHCP Client and DHCP server can interfere with each other's IP address assignments when quickly switching between GO and client modes.
            // Until this can be resolved entirely, introducing a short delay here helps improve test reliability.
            //
            Task.Delay(5000).Wait();

            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteReconnectScenario(ReconnectScenarioType.SendReinvokeInvitationToGo);
            }
        }

        [TestMethod]
        public void AssociateToExistingGo()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteReconnectScenario(ReconnectScenarioType.AssociateToExistingGo);
            }
        }

        void ExecuteReconnectScenario(ReconnectScenarioType reconnectScenarioType)
        {
            ReconnectScenario reconnectScenario = new ReconnectScenario(localWFDController, remoteWFDController, reconnectScenarioType);
            ReconnectScenarioResult reconnectScenarioResult = reconnectScenario.Execute();

            Verify.IsTrue(reconnectScenarioResult.ScenarioSucceeded);
        }
    };
}
