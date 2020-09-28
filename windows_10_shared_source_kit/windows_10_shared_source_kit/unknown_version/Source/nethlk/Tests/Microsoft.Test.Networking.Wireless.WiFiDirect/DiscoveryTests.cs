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
    public class BasicDiscoveryTests
    {
        static WiFiDirectTestController localWFDController;
        static WiFiDirectTestController remoteWFDController;

        const uint longTimeoutMs = 10000; // Timeout used in most test cases
        const uint shortTimeoutMs = 3000; // Timeout used in cases we can expect to be quick (like scan only discovery on 2.4)

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
        public void TargetInListenState_WildcardDiscovery_Auto()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsDevice,
                                         false, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                                         longTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetInListenState_WildcardDiscovery_ScanOnly()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsDevice,
                                         false, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_scan_only,
                                         shortTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetInListenState_TargetedDiscovery_Auto()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsDevice,
                                         true, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                                         longTimeoutMs
                                         );
            }
        }


        [TestMethod]
        public void TargetInListenState_TargetedDiscovery_ScanOnly()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsDevice,
                                         true, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_scan_only,
                                         shortTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetIsAutonomousGo_WildcardDiscovery_Auto()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsGo,
                                         false, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                                         longTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetIsAutonomousGo_WildcardDiscovery_ScanOnly()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsGo,
                                         false, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_scan_only,
                                         shortTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetIsAutonomousGo_TargetedDiscovery_Auto()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsGo,
                                         true, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                                         longTimeoutMs
                                         );
            }
        }

        [TestMethod]
        public void TargetIsAutonomousGo_TargetedDiscovery_ScanOnly()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                ExecuteDiscoveryScenario(DiscoveryScenarioType.DiscoverAsGo,
                                         true, // isTargetedDiscovery
                                         WFD_DISCOVER_TYPE.wfd_discover_type_scan_only,
                                         shortTimeoutMs
                                         );
            }
        }

        void ExecuteDiscoveryScenario(DiscoveryScenarioType discoveryScenarioType,
                                      bool isTargetedDiscovery,
                                      WFD_DISCOVER_TYPE discoverType,
                                      uint searchTimeoutMs)
        {
            DiscoveryScenario discoveryScenario = new DiscoveryScenario(
                localWFDController,
                remoteWFDController,
                discoveryScenarioType,
                isTargetedDiscovery,
                discoverType,
                searchTimeoutMs,
                true // Validate Vendor extension IEs
                );

            DiscoveryScenarioResult discoveryScenarioResult = discoveryScenario.Execute();

            Verify.IsTrue(discoveryScenarioResult.ScenarioSucceeded);
        }
    };
}
