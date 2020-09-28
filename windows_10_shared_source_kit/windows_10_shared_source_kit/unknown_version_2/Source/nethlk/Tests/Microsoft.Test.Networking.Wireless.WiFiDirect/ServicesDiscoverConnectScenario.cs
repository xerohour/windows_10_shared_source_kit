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
    internal class ServicesReconnectPreDiscoveryParameters
    {
        public ServicesReconnectPreDiscoveryParameters(
            int discoveryResultIndex,
            int discoveredHandleIndex,
            WFDSvcWrapperHandle advertiserHandle,
            ServicesConnectOptions options
            )
        {
            Options = options;
            DiscoveryResultIndex = discoveryResultIndex;
            DiscoveredHandleIndex = discoveredHandleIndex;
            AdvertiserHandle = advertiserHandle;
        }

        public ServicesConnectOptions Options { get; private set; }
        // To be replaced with handle
        public int DiscoveryResultIndex { get; private set; }
        // To be replaced with handle
        public int DiscoveredHandleIndex { get; private set; }
        public WFDSvcWrapperHandle AdvertiserHandle { get; private set; }
    }

    /// <summary>
    /// Allows doing an arbitrary number of discoveries and connects
    /// </summary>
    internal class ServicesDiscoverConnectParameters
    {
        // TODO add general constructor that just takes the 2 lists if needed
        
        /// <summary>
        /// Simple version for a single discovery/connect of same service
        /// </summary>
        public ServicesDiscoverConnectParameters(
            WFDSvcWrapperHandle advertiserHandle,
            string query,
            ServicesDiscoveryType discoveryType,
            ServicesConnectOptions connectOptions
            )
        {
            DiscoveryParameters = new List<ServicesDiscoveryParameters> {
                new ServicesDiscoveryParameters(query, 1, new List<WFDSvcWrapperHandle>{ advertiserHandle }, discoveryType)
                };
            ConnectParameters = new List<ServicesReconnectPreDiscoveryParameters> {
                new ServicesReconnectPreDiscoveryParameters(0, 0, advertiserHandle, connectOptions)
                };
        }

        public List<ServicesDiscoveryParameters> DiscoveryParameters { get; private set; }
        public List<ServicesReconnectPreDiscoveryParameters> ConnectParameters { get; private set; }
    }

    internal class ServicesDiscoverConnectScenarioResult
    {
        public ServicesDiscoverConnectScenarioResult(
            bool scenarioSucceeded,
            List<ServicesDiscoveryScenarioResult> discoveryResults,
            List<ServicesConnectScenarioResult> connectResults
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            DiscoveryResults = new List<ServicesDiscoveryScenarioResult>(discoveryResults);
            ConnectResults = new List<ServicesConnectScenarioResult>(connectResults);
        }

        public bool ScenarioSucceeded { get; private set; }
        public List<ServicesDiscoveryScenarioResult> DiscoveryResults { get; private set; }
        public List<ServicesConnectScenarioResult> ConnectResults { get; private set; }
    }

    internal class ServicesDiscoverConnectScenario
    {
        public ServicesDiscoverConnectScenario(
            WiFiDirectTestController seekerTestController,
            WiFiDirectTestController advertiserTestController,
            ServicesDiscoverConnectParameters discoveryConnectParameters
            )
        {
            this.seekerTestController = seekerTestController;
            this.advertiserTestController = advertiserTestController;
            this.discoveryConnectParameters = discoveryConnectParameters;
        }

        public ServicesDiscoverConnectScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesDiscoverConnectScenarioResult(
                succeeded,
                discoveryResults,
                connectResults
                );
        }

        private bool succeeded = false;
        private WiFiDirectTestController seekerTestController;
        private WiFiDirectTestController advertiserTestController;
        private ServicesDiscoverConnectParameters discoveryConnectParameters;

        private List<ServicesDiscoveryScenarioResult> discoveryResults = new List<ServicesDiscoveryScenarioResult>();
        private List<ServicesConnectScenarioResult> connectResults = new List<ServicesConnectScenarioResult>();

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log("Beginning Discover/Connect scenario");

                // Do all discovers
                foreach (var discoveryParams in discoveryConnectParameters.DiscoveryParameters)
                {
                    var discoveryScenario = new ServicesDiscoveryScenario(seekerTestController, advertiserTestController, discoveryParams);
                    ServicesDiscoveryScenarioResult discoveryResult = discoveryScenario.Execute();
                    discoveryResults.Add(discoveryResult);

                    if (!discoveryResult.ScenarioSucceeded)
                    {
                        throw new Exception("Discovery failed!");
                    }
                }

                // BUG: [TH2] Fix race on back-to-back discoveries
                Task.Delay(500).Wait();

                // Do all connects
                foreach (var connectPreParams in discoveryConnectParameters.ConnectParameters)
                {
                    // Need to translate indices to handles
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
                        connectPreParams.AdvertiserHandle,
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
                WiFiDirectTestLogger.Error("Caught exception while executing service discover/connect scenario: {0}", e);
            }
        }
    }
}
