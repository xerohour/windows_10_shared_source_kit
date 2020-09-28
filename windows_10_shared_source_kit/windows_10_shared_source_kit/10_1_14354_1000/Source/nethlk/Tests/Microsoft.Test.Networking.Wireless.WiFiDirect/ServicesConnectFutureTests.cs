///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    // These tests did not get on-boarded to HLK in time
    [TestClass]
    public class ServicesConnectFutureTests
    {
        static WiFiDirectTestController localWFDController;
        static WiFiDirectTestController remoteWFDController;

        private bool needToCloseSession = false;

        static void CleanupTestControllers()
        {
            // Try to remove existing DAF association (if it exists)
            if (localWFDController != null && remoteWFDController != null)
            {
                localWFDController.TryRemoveDafAssociation(remoteWFDController.DeviceAddress);
                remoteWFDController.TryRemoveDafAssociation(localWFDController.DeviceAddress);
            }

            if (localWFDController != null)
            {
                localWFDController.WaitForAllWFDSessionClose(true);
                localWFDController.Dispose();
                localWFDController = null;
            }

            if (remoteWFDController != null)
            {
                remoteWFDController.WaitForAllWFDSessionClose(true);
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
            catch (Exception)
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
            Verify.IsTrue(localWFDController.Reset());
            Verify.IsTrue(remoteWFDController.Reset());

            // Also try to remove existing DAF association (if it exists)
            localWFDController.TryRemoveDafAssociation(remoteWFDController.DeviceAddress);
            remoteWFDController.TryRemoveDafAssociation(localWFDController.DeviceAddress);

            localWFDController.WaitForAllWFDSessionClose(true);
            remoteWFDController.WaitForAllWFDSessionClose(true);

            localWFDController.FlushVisibleDeviceList();
            remoteWFDController.FlushVisibleDeviceList();

            needToCloseSession = false;
        }

        [TestCleanup]
        public void TestMethodCleanup()
        {
            try
            {
                localWFDController.CheckServicesManagerErrors();
                remoteWFDController.CheckServicesManagerErrors();
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Found error in Wi-Fi Direct Services manager: {0}", ex);
            }

            if (needToCloseSession)
            {
                localWFDController.CloseSession();
            }
        }
        [TestMethod]
        public void DeclinePairingConnectionIsRejected()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectConnectSuccess: false,
                        declineConnect: true
                        ),
                    false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);
            }
        }
        [TestMethod]
        public void PinKeypadPairAdvertiserGOReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].SeekerSessionHandle,
                        pairResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                CleanupBeforeReconnect(true);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        validateData: true,
                        expectDeferred: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void PinKeypadPairSeekerGOReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferGO: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false,
                    preferGO: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].SeekerSessionHandle,
                        pairResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                CleanupBeforeReconnect(true);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        preferGO: true,
                        validateData: true,
                        expectDeferred: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void AutoAcceptSeekerGOReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferGO: true,
                        validateData: true
                        ),
                    preferGO: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        preferGO: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredSeekerGOReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        expectDeferred: true,
                        callGetProvisioningInfo: true,
                        preferGO: true,
                        validateData: true
                        ),
                    preferGO: false,
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        expectDeferred: true,
                        callGetProvisioningInfo: true,
                        preferGO: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void ConnectionCapabilitySeekerAutoGO()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Start autonomous GO
                localWFDController.StartAutonomousGo();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true
                        )
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(false);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle,
                        reconnectResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                localWFDController.StopAutonomousGo();
            }
        }

        [TestMethod]
        public void ConnectionCapabilityAdvertiserAutoGO()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Start autonomous GO
                remoteWFDController.StartAutonomousGo();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true
                        )
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].SeekerSessionHandle,
                        pairResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                CleanupBeforeReconnect(false);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                remoteWFDController.StopAutonomousGo();
            }
        }

        [TestMethod]
        public void DeferredConnectionCapabilitySeekerAutoGO()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Start autonomous GO
                localWFDController.StartAutonomousGo();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectDeferred: true,
                        validateData: true
                        ),
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(false);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectDeferred: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle,
                        reconnectResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                localWFDController.StopAutonomousGo();
            }
        }

        [TestMethod]
        public void DeferredConnectionCapabilityAdvertiserAutoGO()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Start autonomous GO
                remoteWFDController.StartAutonomousGo();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectDeferred: true,
                        validateData: true
                        ),
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].SeekerSessionHandle,
                        pairResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                CleanupBeforeReconnect(false);

                // Reconnect
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectDeferred: true,
                        validateData: true,
                        expectGroupFormationNeeded: false
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                remoteWFDController.StopAutonomousGo();
            }
        }

        [TestMethod]
        public void ForgottenGroupAutoAcceptSeeker()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true
                        )
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);
                // Forget pairing on seeker side
                localWFDController.TryRemoveDafAssociation(remoteWFDController.DeviceAddress);
                localWFDController.FlushVisibleDeviceList();

                // Reconnect, which should do pairing
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void ForgottenGroupDeferredSeeker()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);
                // Forget pairing on seeker side
                localWFDController.TryRemoveDafAssociation(remoteWFDController.DeviceAddress);
                localWFDController.FlushVisibleDeviceList();

                // Reconnect, which should do pairing
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void ForgottenGroupAutoAcceptAdvertiser()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        validateData: true
                        )
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);
                // Forget pairing on advertiser side
                remoteWFDController.TryRemoveDafAssociation(localWFDController.DeviceAddress);
                remoteWFDController.FlushVisibleDeviceList();

                // BUG: [TH2] Fix race where UE cancels back-to-back discoveries because set additional IEs OID preempts the scan
                Task.Delay(500).Wait();

                // Reconnect, which should do pairing
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectGroupFormationNeeded: true,
                        validateData: true
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void ForgottenGroupDeferredAdvertiser()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                // Verify results before we start using them
                Verify.AreEqual(1, pairResults.PublishResults.Count);
                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                CleanupBeforeReconnect(true);
                // Forget pairing on advertiser side
                remoteWFDController.TryRemoveDafAssociation(localWFDController.DeviceAddress);
                remoteWFDController.FlushVisibleDeviceList();

                // BUG: [TH2] Fix race where UE cancels back-to-back discoveries because set additional IEs OID preempts the scan
                Task.Delay(500).Wait();

                // Reconnect, which should do pairing
                ServicesDiscoverConnectParameters reconnectParams = new ServicesDiscoverConnectParameters(
                    pairResults.PublishResults[0].AdvertiserHandle,
                    serviceName,
                    ServicesDiscoveryType.Watcher,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        expectGroupFormationNeeded: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinEntry,
                        validateData: true,
                        expectDeferred: true
                        )
                    );

                var reconnectResults = ExecuteDiscoverConnectScenario(reconnectParams);

                Verify.AreEqual(1, reconnectResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        reconnectResults.ConnectResults[0].SeekerSessionHandle,
                        reconnectResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void ConnectASPOverExistingP2PGroup()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Start by doing WFD pairing
                // NOTE: use invitation (not used for services) and config method push button (not used for services)
                ExecutePairingScenario(PairingScenarioType.Invitation, DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON);

                // Make sure we close this session on cleanup
                needToCloseSession = true;

                // Connect, auto accept
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(validateData: true)
                    );

                var pairResults = ExecutePublishDiscoverConnectScenario(connectParams);

                Verify.AreEqual(1, pairResults.ConnectResults.Count);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pairResults.ConnectResults[0].AdvertiserSessionHandle,
                        pairResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );

                // WFD connection should stay up
                WiFiDirectTestLogger.Log("Closed ASP Session, waiting to make sure WFD session stays active...");
                Task.Delay(8000).Wait();

                WfdGlobalSessionState globalSessionStateLocal = localWFDController.QueryGlobalSessionState();

                if (globalSessionStateLocal.Sessions.Length != 1)
                {
                    WiFiDirectTestLogger.Error("Expected a single active session.  Current session count = {0}", globalSessionStateLocal.Sessions.Length);
                    throw new Exception("Local machine session is closed");
                }

                WiFiDirectTestUtilities.RunDataPathValidation(localWFDController, remoteWFDController, "6001");

                // Close WFD connection
                localWFDController.CloseSession();
                needToCloseSession = false;
            }
        }

        ServicesPublishDiscoverConnectScenarioResult ExecutePublishDiscoverConnectScenario(ServicesPublishDiscoverConnectParameters publishDiscoverConnectParams)
        {
            ServicesPublishDiscoverConnectScenario scenario = new ServicesPublishDiscoverConnectScenario(
                localWFDController,
                remoteWFDController,
                publishDiscoverConnectParams
                );
            ServicesPublishDiscoverConnectScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
        }

        ServicesDiscoverConnectScenarioResult ExecuteDiscoverConnectScenario(ServicesDiscoverConnectParameters discoverConnectParams)
        {
            ServicesDiscoverConnectScenario scenario = new ServicesDiscoverConnectScenario(
                localWFDController,
                remoteWFDController,
                discoverConnectParams
                );
            ServicesDiscoverConnectScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
        }

        void ExecuteDisconnectScenario(ServicesDisconnectParameters disconnectParams, bool localInitiate = true)
        {
            ServicesDisconnectScenario scenario;
            if (localInitiate)
            {
                scenario = new ServicesDisconnectScenario(localWFDController, remoteWFDController, disconnectParams);
            }
            else
            {
                scenario = new ServicesDisconnectScenario(remoteWFDController, localWFDController, disconnectParams);
            }

            ServicesDisconnectScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);
        }

        void CleanupBeforeReconnect(bool fWaitForGOStop = false)
        {
            localWFDController.WaitForAllWFDSessionClose(fWaitForGOStop);
            remoteWFDController.WaitForAllWFDSessionClose(fWaitForGOStop);

            localWFDController.FlushVisibleDeviceList();
            remoteWFDController.FlushVisibleDeviceList();
        }

        void ExecutePairingScenario(PairingScenarioType pairingScenarioType, DOT11_WPS_CONFIG_METHOD configMethod)
        {
            PairingScenario pairingScenario = new PairingScenario(localWFDController, remoteWFDController, pairingScenarioType, configMethod, false, true);
            PairingScenarioResult pairingScenarioResult = pairingScenario.Execute();

            Verify.IsTrue(pairingScenarioResult.ScenarioSucceeded);
        }
    };
}
