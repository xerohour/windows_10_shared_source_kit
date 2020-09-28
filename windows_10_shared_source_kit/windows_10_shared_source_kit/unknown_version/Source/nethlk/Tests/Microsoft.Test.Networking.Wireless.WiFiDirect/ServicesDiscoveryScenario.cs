///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    enum ServicesDiscoveryType
    {
        FindAllAsync,
        Watcher
    }

    internal class ServicesDiscoveryParameters
    {
        public ServicesDiscoveryParameters(
            string query,
            uint expectedMatchCount = 1,
            List<WFDSvcWrapperHandle> advertisersToMatch = null,
            ServicesDiscoveryType type = ServicesDiscoveryType.FindAllAsync,
            string serviceInfoRequest = "",
            List<bool> advertiserServiceInfoMatch = null
            )
        {
            Query = query;
            ExpectedMatchCount = expectedMatchCount;
            AdvertisersToMatch = advertisersToMatch;
            Type = type;
            ServiceInfoRequest = serviceInfoRequest;
            AdvertiserServiceInfoMatch = advertiserServiceInfoMatch;
        }

        public string Query { get; private set; }
        public uint ExpectedMatchCount { get; private set; }
        public List<WFDSvcWrapperHandle> AdvertisersToMatch { get; private set; }
        public ServicesDiscoveryType Type { get; private set; }
        public string ServiceInfoRequest { get; private set; }
        public List<bool> AdvertiserServiceInfoMatch { get; private set; }
    }

    internal class ServicesDiscoveryScenarioResult
    {
        public ServicesDiscoveryScenarioResult(
            bool scenarioSucceeded,
            List<WFDSvcWrapperHandle> discoveryHandles
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            DiscoveryHandles = discoveryHandles;
        }

        public bool ScenarioSucceeded { get; private set; }
        public List<WFDSvcWrapperHandle> DiscoveryHandles { get; private set; }
    }

    internal class ServicesDiscoveryScenario
    {
        public ServicesDiscoveryScenario(
            WiFiDirectTestController discoveryTestController,
            WiFiDirectTestController advertiserTestController,
            ServicesDiscoveryParameters discoveryParameters
            )
        {
            this.discoveryTestController = discoveryTestController;
            this.advertiserTestController = advertiserTestController;
            this.discoveryParameters = discoveryParameters;
        }

        public ServicesDiscoveryScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesDiscoveryScenarioResult(succeeded, discoveryHandles);
        }

        private bool succeeded = false;
        private List<WFDSvcWrapperHandle> discoveryHandles = null;
        private WiFiDirectTestController discoveryTestController;
        private WiFiDirectTestController advertiserTestController;
        private ServicesDiscoveryParameters discoveryParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Starting Discovery for \"{0}\" on device {1} ({2})",
                    discoveryParameters.Query,
                    discoveryTestController.DeviceAddress,
                    discoveryTestController.MachineName
                    );

                if (discoveryParameters.Type == ServicesDiscoveryType.FindAllAsync)
                {
                    DoFindAllDiscovery();
                }
                else if (discoveryParameters.Type == ServicesDiscoveryType.Watcher)
                {
                    DoWatcherDiscovery();
                }
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service discovery scenario: {0}", e);
            }
        }

        private void DoFindAllDiscovery()
        {
            Stopwatch discoveryStopwatch = new Stopwatch();
            discoveryStopwatch.Start();

            discoveryHandles = discoveryTestController.DiscoverServices(
                discoveryParameters.Query,
                discoveryParameters.ExpectedMatchCount > 0,
                discoveryParameters.ServiceInfoRequest
                );

            discoveryStopwatch.Stop();
            WiFiDirectTestLogger.Log("Services Discovery (FindAllAsync) completed in {0} ms.", discoveryStopwatch.ElapsedMilliseconds);

            if (discoveryHandles.Count != discoveryParameters.ExpectedMatchCount)
            {
                WiFiDirectTestLogger.Error(
                    "Expected {0} devices but discovered {1} devices",
                    discoveryParameters.ExpectedMatchCount,
                    discoveryHandles.Count
                    );
                return;
            }

            if (discoveryHandles.Count > 0 &&
                discoveryParameters.AdvertisersToMatch != null &&
                discoveryParameters.AdvertisersToMatch.Count > 0)
            {
                WiFiDirectTestLogger.Log("Checking discovery results for expected services");
                bool foundAll = true;

                IList<DiscoveredServiceInfo> discoveredDevices = new List<DiscoveredServiceInfo>();
                foreach (var handle in discoveryHandles)
                {
                    DiscoveredServiceInfo discovery = discoveryTestController.GetDiscoveredServiceInfo(handle);
                    discoveredDevices.Add(discovery);

                    WiFiDirectTestLogger.Log(
                        "Discovered service: {0} with address: {1}",
                        discovery.ServiceName,
                        discovery.ServiceAddress.ToString()
                        );
                }

                int adIdx = 0;
                foreach (var handle in discoveryParameters.AdvertisersToMatch)
                {
                    ServiceAdvertiserInfo advertiser = advertiserTestController.GetAdvertiserInfo(handle);
                    bool found = false;

                    // Check discovered list for match, remove
                    for (int i = 0; i < discoveredDevices.Count; i++)
                    {
                        if (advertiser.ServiceName == discoveredDevices[i].ServiceName &&
                            advertiser.ServiceAddress == discoveredDevices[i].ServiceAddress)
                        {
                            WiFiDirectTestLogger.Log(
                                "Found Expected Service: {0} with address: {1}",
                                discoveredDevices[i].ServiceName,
                                discoveredDevices[i].ServiceAddress.ToString()
                                );

                            if (discoveryParameters.AdvertiserServiceInfoMatch != null &&
                                discoveryParameters.AdvertiserServiceInfoMatch.Count >= adIdx)
                            {
                                if (discoveryParameters.AdvertiserServiceInfoMatch[adIdx])
                                {
                                    WiFiDirectTestLogger.Log(
                                        "Expecting Service Info:\n\t{0}\nReceived:\n\t{1}",
                                        WiFiDirectTestUtilities.GetTruncatedString(advertiser.ServiceInfo, 32),
                                        WiFiDirectTestUtilities.GetTruncatedString(discoveredDevices[i].ServiceInfo, 32)
                                        );
                                    if (advertiser.ServiceInfo != discoveredDevices[i].ServiceInfo)
                                    {
                                        // Allow multiple services with same name/different service info
                                        // Skip if service info match fails, will fail if no service info found
                                        continue;
                                    }
                                }
                                else
                                {
                                    WiFiDirectTestLogger.Log(
                                        "Expecting No Service Info, Received:\n\t{0}",
                                        WiFiDirectTestUtilities.GetTruncatedString(discoveredDevices[i].ServiceInfo, 32)
                                        );
                                    if ("" != discoveredDevices[i].ServiceInfo)
                                    {
                                        // Allow multiple services with same name/different service info
                                        // Skip if service info match fails, will fail if no service info found
                                        continue;
                                    }
                                }
                            }

                            discoveredDevices.RemoveAt(i);

                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        WiFiDirectTestLogger.Error(
                                "Did NOT Find Expected Service: {0} with address: {1}",
                                advertiser.ServiceName,
                                advertiser.ServiceAddress.ToString()
                                );
                        foundAll = false;
                        // Continue checking complete list
                    }

                    adIdx++;
                }

                if (discoveredDevices.Count > 0)
                {
                    WiFiDirectTestLogger.Error("Found unexpected services!");
                    foundAll = false;
                }

                if (!foundAll)
                {
                    return;
                }
            }

            succeeded = true;
        }

        private void DoWatcherDiscovery()
        {
            List<ServiceAdvertiserInfo> servicesExpected = new List<ServiceAdvertiserInfo>();

            int adIdx = 0;
            foreach (var handle in discoveryParameters.AdvertisersToMatch)
            {
                ServiceAdvertiserInfo advertiser = advertiserTestController.GetAdvertiserInfo(handle);
                if (discoveryParameters.AdvertiserServiceInfoMatch == null ||
                    discoveryParameters.AdvertiserServiceInfoMatch.Count < adIdx ||
                    !discoveryParameters.AdvertiserServiceInfoMatch[adIdx])
                {
                    // Clear the service info if not expected
                    advertiser.ServiceInfo = "";
                }
                servicesExpected.Add(advertiser);
                adIdx++;
            }

            Stopwatch discoveryStopwatch = new Stopwatch();
            discoveryStopwatch.Start();

            discoveryHandles = discoveryTestController.WatcherDiscoverServices(
                discoveryParameters.Query,
                servicesExpected,
                discoveryParameters.ServiceInfoRequest
                );

            discoveryStopwatch.Stop();
            WiFiDirectTestLogger.Log("Services Discovery (watcher) completed in {0} ms.", discoveryStopwatch.ElapsedMilliseconds);

            succeeded = true;
        }
    }
}
