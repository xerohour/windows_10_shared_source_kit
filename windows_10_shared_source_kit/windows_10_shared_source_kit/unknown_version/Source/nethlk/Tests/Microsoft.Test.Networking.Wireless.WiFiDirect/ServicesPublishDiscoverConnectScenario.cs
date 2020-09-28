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
    /// This is for defining which discovered service/advertiser to connect before publication/discovery is done
    /// </summary>
    internal class ServicesConnectPreDiscoveryParameters
    {
        public ServicesConnectPreDiscoveryParameters(
            int discoveryResultIndex,
            int discoveredHandleIndex,
            int advertisedHandleIndex,
            ServicesConnectOptions options
            )
        {
            Options = options;
            DiscoveryResultIndex = discoveryResultIndex;
            DiscoveredHandleIndex = discoveredHandleIndex;
            AdvertisedHandleIndex = advertisedHandleIndex;
        }

        public ServicesConnectOptions Options { get; private set; }
        // To be replaced with handle
        public int DiscoveryResultIndex { get; private set; }
        // To be replaced with handle
        public int DiscoveredHandleIndex { get; private set; }
        // To be replaced with handle
        public int AdvertisedHandleIndex { get; private set; }
    }

    /// <summary>
    /// Allows publishing an arbitrary number of services then doing an arbitrary number of discoveries and connects
    /// </summary>
    internal class ServicesPublishDiscoverConnectParameters
    {
        /// <summary>
        /// Simple version for a single publish/discovery/connect of same service
        /// </summary>
        public ServicesPublishDiscoverConnectParameters(
            string serviceName,
            ServicesConnectOptions connectOptions,
            bool autoAccept = true,
            bool preferGO = true,
            ServicePublishConfigMethods configMethods = ServicePublishConfigMethods.PinOrDefaultDisplay,
            ServicesDiscoveryType discoveryType = ServicesDiscoveryType.Watcher,
            string deferredSessionInfo = ""
            )
        {
            PublishDiscoverParameters = new ServicesPublishDiscoverParameters(
                new List<ServicesPublishParameters> {
                    new ServicesPublishParameters(serviceName, autoAccept, preferGO, configMethods, sessionInfo: deferredSessionInfo)
                    },
                new List<ServicesDiscoveryPrePublishParameters> {
                    new ServicesDiscoveryPrePublishParameters(serviceName, 1, new List<int>{ 0 }, discoveryType)
                    }
                );
            ConnectParameters = new List<ServicesConnectPreDiscoveryParameters> {
                new ServicesConnectPreDiscoveryParameters(0, 0, 0, connectOptions)
                };
        }

        public ServicesPublishDiscoverParameters PublishDiscoverParameters { get; private set; }
        public List<ServicesConnectPreDiscoveryParameters> ConnectParameters { get; private set; }
    }

    internal class ServicesPublishDiscoverConnectScenarioResult
    {
        public ServicesPublishDiscoverConnectScenarioResult(
            bool scenarioSucceeded,
            List<ServicesPublishScenarioResult> publishResults,
            List<ServicesDiscoveryScenarioResult> discoveryResults,
            List<ServicesConnectScenarioResult> connectResults
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            PublishResults = new List<ServicesPublishScenarioResult>(publishResults);
            DiscoveryResults = new List<ServicesDiscoveryScenarioResult>(discoveryResults);
            ConnectResults = new List<ServicesConnectScenarioResult>(connectResults);
        }

        public bool ScenarioSucceeded { get; private set; }
        public List<ServicesPublishScenarioResult> PublishResults { get; private set; }
        public List<ServicesDiscoveryScenarioResult> DiscoveryResults { get; private set; }
        public List<ServicesConnectScenarioResult> ConnectResults { get; private set; }
    }

    internal class ServicesPublishDiscoverConnectScenario
    {
        public ServicesPublishDiscoverConnectScenario(
            WiFiDirectTestController seekerTestController,
            WiFiDirectTestController advertiserTestController,
            ServicesPublishDiscoverConnectParameters publishDiscoveryConnectParameters
            )
        {
            this.seekerTestController = seekerTestController;
            this.advertiserTestController = advertiserTestController;
            this.publishDiscoveryConnectParameters = publishDiscoveryConnectParameters;
        }

        public ServicesPublishDiscoverConnectScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesPublishDiscoverConnectScenarioResult(
                succeeded,
                publishResults,
                discoveryResults,
                connectResults
                );
        }

        private bool succeeded = false;
        private WiFiDirectTestController seekerTestController;
        private WiFiDirectTestController advertiserTestController;
        private ServicesPublishDiscoverConnectParameters publishDiscoveryConnectParameters;

        private List<ServicesPublishScenarioResult> publishResults;
        private List<ServicesDiscoveryScenarioResult> discoveryResults;
        private List<ServicesConnectScenarioResult> connectResults = new List<ServicesConnectScenarioResult>();

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log("Beginning Publish/Discover/Connect scenario");

                // Do publish/discover(s)
                var publishDiscoverScenario = new ServicesPublishDiscoverScenario(
                    seekerTestController,
                    advertiserTestController,
                    publishDiscoveryConnectParameters.PublishDiscoverParameters
                    );
                ServicesPublishDiscoverScenarioResult publishDiscoverResults = publishDiscoverScenario.Execute();
                publishResults = publishDiscoverResults.PublishResults;
                discoveryResults = publishDiscoverResults.DiscoveryResults;

                if (!publishDiscoverResults.ScenarioSucceeded)
                {
                    throw new Exception("Publish/Discover failed!");
                }

                // BUG: [TH2] Fix race on back-to-back discoveries
                Task.Delay(500).Wait();

                // Do all connects
                foreach (var connectPreParams in publishDiscoveryConnectParameters.ConnectParameters)
                {
                    // Need to translate indices to handles
                    
                    if (connectPreParams.AdvertisedHandleIndex > publishResults.Count || connectPreParams.AdvertisedHandleIndex < 0)
                    {
                        throw new Exception("Bad connect parameters! Index out of range for advertiser");
                    }
                    WFDSvcWrapperHandle advertiserHandle = publishResults[connectPreParams.AdvertisedHandleIndex].AdvertiserHandle;

                    if (connectPreParams.DiscoveryResultIndex > discoveryResults.Count ||
                        connectPreParams.DiscoveryResultIndex < 0 ||
                        connectPreParams.DiscoveredHandleIndex > discoveryResults[connectPreParams.DiscoveryResultIndex].DiscoveryHandles.Count ||
                        connectPreParams.DiscoveredHandleIndex < 0)
                    {
                        throw new Exception("Bad connect parameters! Index out of range for discovered device");
                    }
                    WFDSvcWrapperHandle discoveredDevice = discoveryResults[connectPreParams.DiscoveryResultIndex].DiscoveryHandles[connectPreParams.DiscoveredHandleIndex];

                    // Now run scenario

                    ServicesConnectParameters connectParams = new ServicesConnectParameters(
                        discoveredDevice,
                        advertiserHandle,
                        connectPreParams.Options
                        );

                    var connectScenario = new ServicesConnectScenario(seekerTestController, advertiserTestController, connectParams);
                    ServicesConnectScenarioResult connectResult = connectScenario.Execute();
                    connectResults.Add(connectResult);

                    if (!connectResult.ScenarioSucceeded)
                    {
                        throw new Exception("Connect failed!");
                    }
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service publish/discover/connect scenario: {0}", e);
            }
        }
    }
}
