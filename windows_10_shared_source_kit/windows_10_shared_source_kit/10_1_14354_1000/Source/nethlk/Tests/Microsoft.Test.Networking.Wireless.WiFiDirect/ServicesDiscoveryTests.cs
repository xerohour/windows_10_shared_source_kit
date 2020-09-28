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

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    [TestClass]
    public class ServicesDiscoveryTests
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
        public void NoMatchDiscovery()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Discover service from local side, using name remote side could publish
                var discoveryParams = new ServicesDiscoveryParameters(
                    remoteWFDController.GenerateUniqueServiceName(),
                    0
                    );
                ExecuteDiscoveryScenario(discoveryParams);
            }
        }

        [TestMethod]
        public void UnPublishNoMatchDiscovery()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish one service on remote
                var publishParams = new ServicesPublishParameters(
                    remoteWFDController.GenerateUniqueServiceName()
                    );
                WFDSvcWrapperHandle advertiser = ExecutePublishScenario(publishParams);

                // Unpublish service
                var unpublishParams = new ServicesUnpublishParameters(advertiser);
                ExecuteUnpublishScenario(unpublishParams);

                // Discover same service from local, should not be found
                var discoveryParams = new ServicesDiscoveryParameters(
                    publishParams.ServiceName,
                    0
                    );
                ExecuteDiscoveryScenario(discoveryParams);
            }
        }

        [TestMethod]
        public void SimpleDiscovery()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish one service on remote and discover same from local
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                var publishDiscoverParams = new ServicesPublishDiscoverParameters(
                    serviceName,
                    discoveryType: ServicesDiscoveryType.FindAllAsync
                    );

                ExecutePublishDiscoverScenario(publishDiscoverParams);
            }
        }

        [TestMethod]
        public void MultipleDiscoveries()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish 2 services on remote
                var publishParams1 = new ServicesPublishParameters(
                    remoteWFDController.GenerateUniqueServiceName()
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    remoteWFDController.GenerateUniqueServiceName()
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);

                // Quick sanity check that the service names are different
                Verify.AreNotEqual(publishParams1.ServiceName, publishParams2.ServiceName);

                // Discover same services from local
                var discoveryParams1 = new ServicesDiscoveryParameters(
                    publishParams1.ServiceName,
                    advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1 }
                    );
                ExecuteDiscoveryScenario(discoveryParams1);

                var discoveryParams2 = new ServicesDiscoveryParameters(
                    publishParams2.ServiceName,
                    advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser2 }
                    );
                ExecuteDiscoveryScenario(discoveryParams2);

                // Publish 3rd service on remote
                var publishParams3 = new ServicesPublishParameters(
                    remoteWFDController.GenerateUniqueServiceName()
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);

                Verify.AreNotEqual(publishParams3.ServiceName, publishParams1.ServiceName);
                Verify.AreNotEqual(publishParams3.ServiceName, publishParams2.ServiceName);

                // Discover 3rd service from local
                var discoveryParams3 = new ServicesDiscoveryParameters(
                    publishParams3.ServiceName,
                    advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser3 }
                    );
                ExecuteDiscoveryScenario(discoveryParams3);
            }
        }

        [TestMethod]
        public void PrefixServiceNameMatchingAll()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish 2 services on remote with same prefix
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix + ".foo",
                    prefixList: new List<String> { prefix }
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix + ".bar",
                    prefixList: new List<String> { prefix, prefix + ".", prefix + ".b" }
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);

                // Discover both services using common prefix
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix + "*",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1, advertiser2 },
                        expectedMatchCount: 2,
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void PrefixServiceNameMatchingSpecific()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish 2 services on remote with same prefix, 3rd service with different prefix
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix + ".1.foo",
                    prefixList: new List<String> { prefix, prefix + ".1" }
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix + ".1.bar",
                    prefixList: new List<String> { prefix, prefix + ".1", prefix + ".1." }
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);
                var publishParams3 = new ServicesPublishParameters(
                    prefix + ".2.foo",
                    prefixList: new List<String> { prefix, prefix + ".2" }
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);

                // Discover first 2 services using common prefix
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix + ".1*",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1, advertiser2 },
                        expectedMatchCount: 2
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void PrefixServiceNameMatchExactAndPrefix()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish 2 services on remote with same prefix
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix + ".foo",
                    prefixList: new List<String> { prefix }
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);

                // Discover both services using common prefix
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix + "*",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1, advertiser2 },
                        expectedMatchCount: 2,
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void ExactServiceNameMatchingSkipsPrefixes()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish 2 services on remote with same prefix, 3rd service with name==prefix
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix + ".1",
                    prefixList: new List<String> { prefix }
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix + ".2",
                    prefixList: new List<String> { prefix }
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);
                var publishParams3 = new ServicesPublishParameters(
                    prefix
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);

                // Discover last service only, don't discover prefixed services
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix,
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser3 },
                        expectedMatchCount: 1
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void PublishServicesLongName()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish multiple services with long names and prefixes
                string prefix1 = remoteWFDController.GenerateUniqueServiceName("com.microsoft.longnameservice.1.2.3.4.5.6.7.8.9.0.");
                var publishParams1 = new ServicesPublishParameters(
                    prefix1 + ".foo"
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix1 + ".bar"
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);
                var publishParams3 = new ServicesPublishParameters(
                    prefix1 + ".baz"
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);
                var publishParams4 = new ServicesPublishParameters(
                    prefix1 + ".bat"
                    );
                WFDSvcWrapperHandle advertiser4 = ExecutePublishScenario(publishParams4);

                // Discover all services, NOT USING PREFIX
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix1 + ".foo",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1 },
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
                var discoveryParams2 = new ServicesDiscoveryParameters(
                        prefix1 + ".bar",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser2 },
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams2);
                var discoveryParams3 = new ServicesDiscoveryParameters(
                        prefix1 + ".baz",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser3 },
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams3);
                var discoveryParams4 = new ServicesDiscoveryParameters(
                        prefix1 + ".bat",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser4 },
                        type: ServicesDiscoveryType.Watcher
                        );
                ExecuteDiscoveryScenario(discoveryParams4);
            }
        }

        [TestMethod]
        public void SimpleServiceInfoDiscovery()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish one service on remote and discover same from local
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                var publishDiscoverParams = new ServicesPublishDiscoverParameters(
                    serviceName,
                    discoveryType: ServicesDiscoveryType.FindAllAsync,
                    serviceInfo: "<foo>bar</foo>",
                    serviceInfoRequest: "bar",
                    doesServiceInfoMatch: true
                    );

                ExecutePublishDiscoverScenario(publishDiscoverParams);
            }
        }

        [TestMethod]
        public void SimpleServiceInfoNoMatchDiscovery()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                // Publish one service on remote and discover same from local
                string serviceName = remoteWFDController.GenerateUniqueServiceName();
                var publishDiscoverParams = new ServicesPublishDiscoverParameters(
                    serviceName,
                    discoveryType: ServicesDiscoveryType.FindAllAsync,
                    serviceInfo: "<foo>bar</foo>",
                    serviceInfoRequest: "baz",
                    doesServiceInfoMatch: false
                    );

                ExecutePublishDiscoverScenario(publishDiscoverParams);
            }
        }

        [TestMethod]
        public void ServiceInfoMultipleMatchSameServiceName()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix,
                    serviceInfo: "this is some example service info. It will match the requested info"
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix,
                    serviceInfo: "Here is another set of service info. It also has the word example"
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);
                var publishParams3 = new ServicesPublishParameters(
                    prefix,
                    serviceInfo: "This service will match without service info. Not expected to find this"
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);

                // Discover all services using same name
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix,
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1, advertiser2, advertiser3 },
                        type: ServicesDiscoveryType.Watcher,
                        serviceInfoRequest: "example",
                        advertiserServiceInfoMatch: new List<bool> { true, true, false }
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void ServiceInfoMultipleMatchSameServicePrefix()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string prefix = remoteWFDController.GenerateUniqueServiceName();
                var publishParams1 = new ServicesPublishParameters(
                    prefix + ".foo",
                    prefixList: new List<String> { prefix },
                    serviceInfo: "this is some example service info. It will match the requested info"
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                var publishParams2 = new ServicesPublishParameters(
                    prefix,
                    serviceInfo: "Here is another set of service info. It also has the word example"
                    );
                WFDSvcWrapperHandle advertiser2 = ExecutePublishScenario(publishParams2);
                var publishParams3 = new ServicesPublishParameters(
                    prefix + ".bar",
                    prefixList: new List<String> { prefix },
                    serviceInfo: "This service will match without service info. Not expected to find this"
                    );
                WFDSvcWrapperHandle advertiser3 = ExecutePublishScenario(publishParams3);

                // Discover all services using same name
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        prefix + "*",
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1, advertiser2, advertiser3 },
                        expectedMatchCount: 3,
                        type: ServicesDiscoveryType.Watcher,
                        serviceInfoRequest: "example",
                        advertiserServiceInfoMatch: new List<bool> { true, true, false }
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }

        [TestMethod]
        public void ServiceInfoMaxSize()
        {
            using (RemoteControllerLogGroup lg = new RemoteControllerLogGroup(remoteWFDController))
            {
                string serviceInfo = "";
                Random rng = new Random();
                const string s_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()_-+={}][|;:'<>,./?~` ";

                StringBuilder builder = new StringBuilder();
                // make a string of 65000 ASCII characters
                // First write 10000 random characters
                // Next write 7 characters to match
                // Then write 54993 random characters
                for (int i = 0; i < 10000; i++)
                {
                    builder.Append(s_chars[rng.Next(s_chars.Length)]);
                }
                builder.Append("foo_bar");
                for (int i = 0; i < 54993; i++)
                {
                    builder.Append(s_chars[rng.Next(s_chars.Length)]);
                }
                serviceInfo = builder.ToString();

                var publishParams1 = new ServicesPublishParameters(
                    remoteWFDController.GenerateUniqueServiceName(),
                    serviceInfo: serviceInfo
                    );
                WFDSvcWrapperHandle advertiser1 = ExecutePublishScenario(publishParams1);
                
                // Discover service with info
                var discoveryParams1 = new ServicesDiscoveryParameters(
                        publishParams1.ServiceName,
                        advertisersToMatch: new List<WFDSvcWrapperHandle> { advertiser1 },
                        type: ServicesDiscoveryType.Watcher,
                        serviceInfoRequest: "foo_bar",
                        advertiserServiceInfoMatch: new List<bool> { true }
                        );
                ExecuteDiscoveryScenario(discoveryParams1);
            }
        }


        WFDSvcWrapperHandle ExecutePublishScenario(ServicesPublishParameters publishParams)
        {
            ServicesPublishScenario publishScenario = new ServicesPublishScenario(remoteWFDController, publishParams);
            ServicesPublishScenarioResult publishScenarioResult = publishScenario.Execute();

            Verify.IsTrue(publishScenarioResult.ScenarioSucceeded);

            return publishScenarioResult.AdvertiserHandle;
        }

        void ExecuteUnpublishScenario(ServicesUnpublishParameters unpublishParams)
        {
            ServicesUnpublishScenario unpublishScenario = new ServicesUnpublishScenario(remoteWFDController, unpublishParams);
            ServicesUnpublishScenarioResult unpublishScenarioResult = unpublishScenario.Execute();

            Verify.IsTrue(unpublishScenarioResult.ScenarioSucceeded);
        }

        List<WFDSvcWrapperHandle> ExecuteDiscoveryScenario(ServicesDiscoveryParameters discoveryParams)
        {
            ServicesDiscoveryScenario discoveryScenario = new ServicesDiscoveryScenario(localWFDController, remoteWFDController, discoveryParams);
            ServicesDiscoveryScenarioResult discoveryScenarioResult = discoveryScenario.Execute();

            Verify.IsTrue(discoveryScenarioResult.ScenarioSucceeded);

            return discoveryScenarioResult.DiscoveryHandles;
        }

        ServicesPublishDiscoverScenarioResult ExecutePublishDiscoverScenario(ServicesPublishDiscoverParameters publishDiscoverParams)
        {
            ServicesPublishDiscoverScenario scenario = new ServicesPublishDiscoverScenario(
                localWFDController,
                remoteWFDController,
                publishDiscoverParams
                );
            ServicesPublishDiscoverScenarioResult scenarioResult = scenario.Execute();

            Verify.IsTrue(scenarioResult.ScenarioSucceeded);

            return scenarioResult;
        }
    };
}
