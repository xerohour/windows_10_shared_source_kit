///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Text;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    [TestClass]
    public class ServicesConnectTests
    {
        static WiFiDirectTestController localWFDController;
        static WiFiDirectTestController remoteWFDController;

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
        }

        [TestMethod]
        public void SimpleAutoAcceptPairingSeekerDisconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(validateData: true)
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void SimpleAutoAcceptPairingAdvertiserDisconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(validateData: true)
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect (from advertiser side)
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].AdvertiserSessionHandle,
                        results.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );
            }
        }

        [TestMethod]
        public void DiscoverUnpublishConnectFails()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish/Discover
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverParameters discoverParams = new ServicesPublishDiscoverParameters(
                    serviceName
                    );

                var publishDiscoverResults = ExecutePublishDiscoverScenario(discoverParams);

                // Verify results before we start using them
                Verify.AreEqual(1, publishDiscoverResults.PublishResults.Count);
                Verify.AreEqual(1, publishDiscoverResults.DiscoveryResults.Count);
                Verify.AreEqual(1, publishDiscoverResults.DiscoveryResults[0].DiscoveryHandles.Count);

                // Unpublish
                ServicesUnpublishParameters unpublishParams = new ServicesUnpublishParameters(
                    publishDiscoverResults.PublishResults[0].AdvertiserHandle,
                    false
                    );
                ExecuteUnpublishScenario(unpublishParams);

                // Connect (fail)
                ServicesConnectParameters connectParams = new ServicesConnectParameters(
                    publishDiscoverResults.DiscoveryResults[0].DiscoveryHandles[0],
                    publishDiscoverResults.PublishResults[0].AdvertiserHandle,
                    new ServicesConnectOptions(expectConnectSuccess: false)
                    );
                ExecuteConnectScenario(connectParams);
            }
        }

        [TestMethod]
        public void SimpleAutoAcceptMultipleSockets()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions()
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                var tcp1Results = ExecuteOpenSocketSendDataScenario(
                    new ServicesOpenSocketSendDataParameters(
                        results.ConnectResults[0].AdvertiserSessionHandle,
                        results.ConnectResults[0].SeekerSessionHandle,
                        WiFiDirectServiceIPProtocol.Tcp,
                        6000,
                        131072 // 128k
                        ),
                    false
                    );

                var tcp2Results = ExecuteOpenSocketSendDataScenario(
                    new ServicesOpenSocketSendDataParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle,
                        WiFiDirectServiceIPProtocol.Tcp,
                        6007,
                        131072 // 128k
                        )
                    );

                var udp1Results = ExecuteOpenSocketSendDataScenario(
                    new ServicesOpenSocketSendDataParameters(
                        results.ConnectResults[0].AdvertiserSessionHandle,
                        results.ConnectResults[0].SeekerSessionHandle,
                        WiFiDirectServiceIPProtocol.Udp,
                        7000,
                        8000
                        ),
                    false
                    );

                var udp2Results = ExecuteOpenSocketSendDataScenario(
                    new ServicesOpenSocketSendDataParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle,
                        WiFiDirectServiceIPProtocol.Udp,
                        7007,
                        8000
                        )
                    );

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void SimpleAutoAcceptPairingReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesConnectOptions connectOptions = new ServicesConnectOptions(validateData: true);
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    connectOptions
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
                    connectOptions
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
        public void SimpleAutoAcceptPairingReconnectDifferentService()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesConnectOptions connectOptions = new ServicesConnectOptions(validateData: true);
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    connectOptions
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
                string serviceName2 = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters reconnectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName2,
                    connectOptions
                    );

                var reconnectResults = ExecutePublishDiscoverConnectScenario(reconnectParams);

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
        public void SimpleAutoAcceptMultipleSimultaneousSessions()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string serviceName1 = remoteWFDController.GenerateUniqueServiceName();
                string serviceName2 = remoteWFDController.GenerateUniqueServiceName();
                string serviceName3 = remoteWFDController.GenerateUniqueServiceName();

                ServicesConnectOptions connectOptions = new ServicesConnectOptions(validateData: true);

                // NOTE: this will do FindAllAsync while all other tests use the watcher/incremental updates
                // Publish 2 services, discover/connect to first
                ServicesPublishDiscoverConnectParameters pdcParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName1,
                    connectOptions
                    );
                var pdcResults = ExecutePublishDiscoverConnectScenario(pdcParams);

                Verify.AreEqual(1, pdcResults.PublishResults.Count);
                Verify.AreEqual(1, pdcResults.ConnectResults.Count);

                // Discover/connect to second
                ServicesPublishDiscoverConnectParameters pdc2Params = new ServicesPublishDiscoverConnectParameters(
                    serviceName2,
                    connectOptions,
                    discoveryType: ServicesDiscoveryType.Watcher
                    );
                var pdc2Results = ExecutePublishDiscoverConnectScenario(pdc2Params);

                Verify.AreEqual(1, pdc2Results.PublishResults.Count);
                Verify.AreEqual(1, pdc2Results.ConnectResults.Count);

                // Publish/discover/connect third service
                ServicesPublishDiscoverConnectParameters pdc3Params = new ServicesPublishDiscoverConnectParameters(
                    serviceName3,
                    connectOptions
                    );
                var pdc3Results = ExecutePublishDiscoverConnectScenario(pdc3Params);

                Verify.AreEqual(1, pdc3Results.PublishResults.Count);
                Verify.AreEqual(1, pdc3Results.ConnectResults.Count);

                // Disconnect second service
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdc2Results.ConnectResults[0].SeekerSessionHandle,
                        pdc2Results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );

                // Try to send some more data on 1st session, expect still connected
                ServicesSendDataParameters sendDataParams = new ServicesSendDataParameters(
                    pdcResults.ConnectResults[0].SeekerSessionHandle,
                    pdcResults.ConnectResults[0].SeekerSocketHandle,
                    pdcResults.ConnectResults[0].AdvertiserSessionHandle,
                    pdcResults.ConnectResults[0].AdvertiserSocketHandle,
                    65536
                    );
                var sendDataResult = ExecuteSendDataScenario(sendDataParams);

                // Disconnect first/third service
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdcResults.ConnectResults[0].AdvertiserSessionHandle,
                        pdcResults.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdc3Results.ConnectResults[0].SeekerSessionHandle,
                        pdc3Results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void AutoAcceptPairDeferredReconnect()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        validateData: true
                        )
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

                // Reconnect with auto-accept false (deferred)
                string serviceName2 = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters reconnectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName2,
                    new ServicesConnectOptions(
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false
                    );

                var reconnectResults = ExecutePublishDiscoverConnectScenario(reconnectParams);

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
        public void PinKeypadPairAdvertiserGO()
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
                        )
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void PinKeypadPairSeekerGO()
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
                    preferGO: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void PinKeypadPairNoPreferenceGO()
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
                    preferGO: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void PinKeypadPairBothPreferGO()
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
                    preferGO: true
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairDefaultConfig()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        validateData: true,
                        expectDeferred: true
                        ),
                    autoAccept: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairPinKeypad()
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

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairPinDisplay()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinDisplay, // seeker
                        validateData: true,
                        expectDeferred: true
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairNoCommonConfigMethodFail()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.PinDisplay, // seeker
                        validateData: true,
                        expectGetProvisioningSuccess: false
                        ),
                    configMethods: ServicePublishConfigMethods.PinOnlyDisplay, // advertiser
                    autoAccept: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);
            }
        }

        [TestMethod]
        public void DeferredPairDeferredSessionInfo()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: true
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false,
                    deferredSessionInfo: "abcdefghijklmnopqrstuvwxyz"
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        private Random rng = new Random();
        private const string s_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()_-+={}][|;:'<>,./?~` ";

        [TestMethod]
        public void DeferredPairDeferredSessionInfoMaxSize()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < 144; i++)
                {
                    builder.Append(s_chars[rng.Next(s_chars.Length)]);
                }
                string sessionInfo = builder.ToString();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: true
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false,
                    deferredSessionInfo: sessionInfo
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairSessionInfo()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: true,
                        sessionInfo: "a"
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        [TestMethod]
        public void DeferredPairSessionInfoMaxSize()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < 144; i++)
                {
                    builder.Append(s_chars[rng.Next(s_chars.Length)]);
                }
                string sessionInfo = builder.ToString();

                // Connect
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                ServicesPublishDiscoverConnectParameters connectParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: true,
                        sessionInfo: sessionInfo
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false
                    );

                var results = ExecutePublishDiscoverConnectScenario(connectParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        results.ConnectResults[0].SeekerSessionHandle,
                        results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        // Test Session Info/Deferred Session Info on ASPCP messages
        [TestMethod]
        public void DeferredPairMultipleSessionSessionInfo()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string serviceName1 = remoteWFDController.GenerateUniqueServiceName();
                string serviceName2 = remoteWFDController.GenerateUniqueServiceName();

                // First publish/discover/connect
                ServicesPublishDiscoverConnectParameters pdc1Params = new ServicesPublishDiscoverConnectParameters(
                    serviceName1,
                    new ServicesConnectOptions(validateData: true)
                    );
                var pdc1Results = ExecutePublishDiscoverConnectScenario(pdc1Params);

                // Now test ASPCP messages for session info
                ServicesPublishDiscoverConnectParameters pdc2Params = new ServicesPublishDiscoverConnectParameters(
                    serviceName2,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: false,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: true,
                        expectGroupFormationNeeded: false,
                        sessionInfo: "abcd123"
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: false,
                    deferredSessionInfo: "zyxw987"
                    );
                var pdc2Results = ExecutePublishDiscoverConnectScenario(pdc2Params);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdc1Results.ConnectResults[0].SeekerSessionHandle,
                        pdc1Results.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdc2Results.ConnectResults[0].AdvertiserSessionHandle,
                        pdc2Results.ConnectResults[0].SeekerSessionHandle
                        ),
                    false
                    );
            }
        }

        [TestMethod]
        public void AutoAcceptSessionInfo()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string serviceName = remoteWFDController.GenerateUniqueServiceName();

                // Now test ASPCP messages for session info
                ServicesPublishDiscoverConnectParameters pdcParams = new ServicesPublishDiscoverConnectParameters(
                    serviceName,
                    new ServicesConnectOptions(
                        callGetProvisioningInfo: true,
                        preferredConfigMethod: WiFiDirectServiceConfigurationMethod.Default, // seeker
                        validateData: true,
                        expectDeferred: false,
                        sessionInfo: "abcd123" // This should still be sent to advertiser
                        ),
                    configMethods: ServicePublishConfigMethods.Any, // advertiser
                    autoAccept: true,
                    deferredSessionInfo: "zyxw987" // want to make sure this isn't seen by seeker
                    );
                var pdcResults = ExecutePublishDiscoverConnectScenario(pdcParams);

                // Disconnect
                ExecuteDisconnectScenario(
                    new ServicesDisconnectParameters(
                        pdcResults.ConnectResults[0].SeekerSessionHandle,
                        pdcResults.ConnectResults[0].AdvertiserSessionHandle
                        )
                    );
            }
        }

        ServicesPublishDiscoverScenarioResult ExecutePublishDiscoverScenario(ServicesPublishDiscoverParameters pubDiscoParams)
        {
            ServicesPublishDiscoverScenario scenario = new ServicesPublishDiscoverScenario(
                localWFDController,
                remoteWFDController,
                pubDiscoParams
                );
            ServicesPublishDiscoverScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
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

        ServicesConnectScenarioResult ExecuteConnectScenario(ServicesConnectParameters connectParams)
        {
            ServicesConnectScenario scenario = new ServicesConnectScenario(
                localWFDController,
                remoteWFDController,
                connectParams
                );
            ServicesConnectScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
        }

        ServicesOpenSocketSendDataScenarioResult ExecuteOpenSocketSendDataScenario(ServicesOpenSocketSendDataParameters dataParams, bool localInitiate = true)
        {
            ServicesOpenSocketSendDataScenario scenario;
            if (localInitiate)
            {
                scenario = new ServicesOpenSocketSendDataScenario(
                    localWFDController,
                    remoteWFDController,
                    dataParams
                    );
            }
            else
            {
                scenario = new ServicesOpenSocketSendDataScenario(
                    remoteWFDController,
                    localWFDController,
                    dataParams
                    );
            }
            ServicesOpenSocketSendDataScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
        }

        ServicesSendDataScenarioResult ExecuteSendDataScenario(ServicesSendDataParameters dataParams)
        {
            ServicesSendDataScenario scenario = new ServicesSendDataScenario(
                localWFDController,
                remoteWFDController,
                dataParams
                );
            ServicesSendDataScenarioResult scenarioResult = scenario.Execute();

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

        void ExecuteUnpublishScenario(ServicesUnpublishParameters unpublishParams)
        {
            ServicesUnpublishScenario unpublishScenario = new ServicesUnpublishScenario(remoteWFDController, unpublishParams);
            ServicesUnpublishScenarioResult unpublishScenarioResult = unpublishScenario.Execute();

            Verify.IsTrue(unpublishScenarioResult.ScenarioSucceeded);
        }

        void CleanupBeforeReconnect(bool fWaitForGOStop = false)
        {
            localWFDController.WaitForAllWFDSessionClose(fWaitForGOStop);
            remoteWFDController.WaitForAllWFDSessionClose(fWaitForGOStop);

            localWFDController.FlushVisibleDeviceList();
            remoteWFDController.FlushVisibleDeviceList();
        }
    };
}
