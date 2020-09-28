///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    /// <summary>
    /// This is for defining which advertiser to match before publication is done
    /// </summary>
    internal class ServicesDiscoveryPrePublishParameters
    {
        public ServicesDiscoveryPrePublishParameters(
            string query,
            uint expectedMatchCount = 1,
            List<int> advertisersToMatchIndices = null,
            ServicesDiscoveryType type = ServicesDiscoveryType.FindAllAsync,
            string serviceInfoRequest = "",
            List<bool> advertiserServiceInfoMatch = null
            )
        {
            Parameters = new ServicesDiscoveryParameters(query, expectedMatchCount, null, type, serviceInfoRequest, advertiserServiceInfoMatch);
            AdvertisersToMatchIndices = advertisersToMatchIndices;
        }

        public ServicesDiscoveryParameters Parameters { get; private set; }
        // These are going to match to the advertiser handles after publication is done
        public List<int> AdvertisersToMatchIndices { get; private set; }
    }

    /// <summary>
    /// Allows publishing an arbitrary number of services then doing an arbitrary number of discoveries
    /// </summary>
    internal class ServicesPublishDiscoverParameters
    {
        public ServicesPublishDiscoverParameters(
            List<ServicesPublishParameters> publishParameters,
            List<ServicesDiscoveryPrePublishParameters> discoveryParameters
            )
        {
            PublishParameters = new List<ServicesPublishParameters>(publishParameters);
            DiscoveryParameters = new List<ServicesDiscoveryPrePublishParameters>(discoveryParameters);
        }

        /// <summary>
        /// Simple version for a single publish/discovery of same service
        /// </summary>
        public ServicesPublishDiscoverParameters(
            string serviceName,
            bool autoAccept = true,
            bool preferGO = true,
            ServicePublishConfigMethods configMethods = ServicePublishConfigMethods.PinOrDefaultDisplay,
            ServicesDiscoveryType discoveryType = ServicesDiscoveryType.Watcher,
            string serviceInfo = "",
            string serviceInfoRequest = "",
            bool doesServiceInfoMatch = false
            )
        {
            PublishParameters = new List<ServicesPublishParameters> {
                new ServicesPublishParameters(serviceName, autoAccept, preferGO, configMethods, serviceInfo: serviceInfo)
                };
            DiscoveryParameters = new List<ServicesDiscoveryPrePublishParameters> {
                new ServicesDiscoveryPrePublishParameters(serviceName, 1, new List<int>{ 0 }, discoveryType, serviceInfoRequest, new List<bool>{ doesServiceInfoMatch })
                };
        }

        public List<ServicesPublishParameters> PublishParameters { get; set; }
        public List<ServicesDiscoveryPrePublishParameters> DiscoveryParameters { get; set; }
    }

    internal class ServicesPublishDiscoverScenarioResult
    {
        public ServicesPublishDiscoverScenarioResult(
            bool scenarioSucceeded,
            List<ServicesPublishScenarioResult> publishResults,
            List<ServicesDiscoveryScenarioResult> discoveryResults
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            PublishResults = new List<ServicesPublishScenarioResult>(publishResults);
            DiscoveryResults = new List<ServicesDiscoveryScenarioResult>(discoveryResults);
        }

        public bool ScenarioSucceeded { get; private set; }
        public List<ServicesPublishScenarioResult> PublishResults { get; private set; }
        public List<ServicesDiscoveryScenarioResult> DiscoveryResults { get; private set; }
    }

    internal class ServicesPublishDiscoverScenario
    {
        public ServicesPublishDiscoverScenario(
            WiFiDirectTestController discoveryTestController,
            WiFiDirectTestController advertiserTestController,
            ServicesPublishDiscoverParameters publishDiscoveryParameters
            )
        {
            this.discoveryTestController = discoveryTestController;
            this.advertiserTestController = advertiserTestController;
            this.publishDiscoveryParameters = publishDiscoveryParameters;
        }

        public ServicesPublishDiscoverScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesPublishDiscoverScenarioResult(
                succeeded,
                publishResults,
                discoveryResults
                );
        }

        private bool succeeded = false;
        private WiFiDirectTestController discoveryTestController;
        private WiFiDirectTestController advertiserTestController;
        private ServicesPublishDiscoverParameters publishDiscoveryParameters;

        private List<ServicesPublishScenarioResult> publishResults = new List<ServicesPublishScenarioResult>();
        private List<ServicesDiscoveryScenarioResult> discoveryResults = new List<ServicesDiscoveryScenarioResult>();

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log("Beginning Publish/Discover scenario");

                // Do all publications
                foreach (var publishParams in publishDiscoveryParameters.PublishParameters)
                {
                    var publishScenario = new ServicesPublishScenario(advertiserTestController, publishParams);
                    ServicesPublishScenarioResult publishResult = publishScenario.Execute();
                    publishResults.Add(publishResult);

                    if (!publishResult.ScenarioSucceeded)
                    {
                        throw new Exception("Publication failed!");
                    }
                }

                // Do all discoveries
                foreach (var discoveryPreParams in publishDiscoveryParameters.DiscoveryParameters)
                {
                    // Need to translate indices to handles
                    List<WFDSvcWrapperHandle> advertisersToMatch = new List<WFDSvcWrapperHandle>();
                    foreach (int i in discoveryPreParams.AdvertisersToMatchIndices)
                    {
                        if (i > publishResults.Count || i < 0)
                        {
                            throw new Exception("Bad discovery parameters! Index out of range");
                        }
                        advertisersToMatch.Add(publishResults[i].AdvertiserHandle);
                    }
                    ServicesDiscoveryParameters discoveryParams = new ServicesDiscoveryParameters(
                        discoveryPreParams.Parameters.Query,
                        discoveryPreParams.Parameters.ExpectedMatchCount,
                        advertisersToMatch,
                        discoveryPreParams.Parameters.Type,
                        discoveryPreParams.Parameters.ServiceInfoRequest,
                        discoveryPreParams.Parameters.AdvertiserServiceInfoMatch
                        );
                    var discoveryScenario = new ServicesDiscoveryScenario(discoveryTestController, advertiserTestController, discoveryParams);
                    ServicesDiscoveryScenarioResult discoveryResult = discoveryScenario.Execute();
                    discoveryResults.Add(discoveryResult);

                    if (!discoveryResult.ScenarioSucceeded)
                    {
                        throw new Exception("Discovery failed!");
                    }
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service publish/discovery scenario: {0}", e);
            }
        }
    }
}
