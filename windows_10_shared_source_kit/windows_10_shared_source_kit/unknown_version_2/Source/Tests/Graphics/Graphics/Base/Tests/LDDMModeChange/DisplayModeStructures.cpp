C) Microsoft. All rights reserved.
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
     