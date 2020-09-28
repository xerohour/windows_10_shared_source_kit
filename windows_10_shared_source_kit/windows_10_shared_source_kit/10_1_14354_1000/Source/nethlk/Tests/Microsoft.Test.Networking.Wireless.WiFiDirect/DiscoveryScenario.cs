///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Test.Networking.SafeHandles;
using Microsoft.Test.Networking.Wireless;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    //
    // The DiscoveryScenario class will be the main interface through which the tests perform Wi-Fi Direct discoveries.
    // The discovery tests themselves will primarily call into this class.
    // The pairing and reconnect logic will internally reuse the DiscoveryScenario to perform targetted discoveries before pairing.
    //
    internal enum DiscoveryScenarioType
    {
        DiscoverAsDevice,
        DiscoverAsGo
    }

    internal class DiscoveryScenarioResult
    {
        public DiscoveryScenarioResult(bool scenarioSucceeded)
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded {get; private set;}
    }

    internal class DiscoveryScenario
    {
        public DiscoveryScenario(WiFiDirectTestController localWFDController,
                                 WiFiDirectTestController remoteWFDController,
                                 DiscoveryScenarioType discoveryScenarioType,
                                 bool isTargetedDiscovery,
                                 WFD_DISCOVER_TYPE discoverType,
                                 uint searchTimeoutMs,
                                 bool discoverTestIe
                                 )
        {
            this.succeeded = false;
            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;
            this.discoveryScenarioType = discoveryScenarioType;
            this.isTargetedDiscovery = isTargetedDiscovery;
            this.discoverType = discoverType;
            this.searchTimeoutMs = searchTimeoutMs;
            this.discoverTestIe = discoverTestIe;
        }

        public DiscoveryScenarioResult Execute()
        {
            ExecuteInternal();

            return new DiscoveryScenarioResult(succeeded);
        }

        private bool succeeded;
        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private DiscoveryScenarioType discoveryScenarioType;
        private bool isTargetedDiscovery;
        private WFD_DISCOVER_TYPE discoverType;
        private uint searchTimeoutMs;
        private bool discoverTestIe;

        private void ExecuteInternal()
        {
            try
            {
                if (discoverTestIe)
                {
                    remoteWFDController.SetTestIe();
                }

                if (discoveryScenarioType == DiscoveryScenarioType.DiscoverAsDevice)
                {
                    // Enable listen state on the remote device if it is not already enabled.
                    remoteWFDController.ConfigureListenState(true);
                }
                else if (discoveryScenarioType == DiscoveryScenarioType.DiscoverAsGo)
                {
                    // Start autonomous GO on the remote device if it is not already running
                    remoteWFDController.StartAutonomousGo();
                }

                // Flush the visible device list on the local device
                localWFDController.FlushVisibleDeviceList();

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                WfdDeviceDescriptor discoveredDeviceDescriptor;
                if(isTargetedDiscovery)
                {
                    discoveredDeviceDescriptor = PerformTargetedDiscovery();
                }
                else
                {
                    discoveredDeviceDescriptor = PerformWildcardDiscovery();
                }

                if (discoveredDeviceDescriptor == null)
                {
                    return;
                }

                WiFiDirectTestLogger.Log("Successfully discovered device {0} ({1})", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);

                stopwatch.Stop();
                WiFiDirectTestLogger.Log("Discovery completed in {0} ms.", stopwatch.ElapsedMilliseconds);

                if (discoverTestIe)
                {
                    Guid discoveredGuid;
                    bool parsed = TestIeManager.TryParseIeBuffer(discoveredDeviceDescriptor.IE, out discoveredGuid);
                    if (!parsed)
                    {
                        WiFiDirectTestLogger.Error("Expected vendor extension IE not present in discovery result.");
                        return;
                    }

                    if (discoveredGuid == remoteWFDController.CurrentIEGuid)
                    {
                        WiFiDirectTestLogger.Log("Validated expected GUID {0} is present in vendor extension IE.", discoveredGuid);
                    }
                    else
                    {
                        WiFiDirectTestLogger.Error("Found incorrect GUID in vendor extension IE.  Expected: {0}, Found: {1}", remoteWFDController.CurrentIEGuid, discoveredGuid);
                        return;
                    }
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                 WiFiDirectTestLogger.Error("Caught exception while running discovery scenario: {0}", e);
            }
        }

        private WfdDeviceDescriptor PerformTargetedDiscovery()
        {
            DOT11_WFD_DISCOVER_DEVICE_FILTER_BITMASK deviceFilterBitmask;
            switch(discoveryScenarioType)
            {
                case DiscoveryScenarioType.DiscoverAsDevice:
                    deviceFilterBitmask = DOT11_WFD_DISCOVER_DEVICE_FILTER_BITMASK.Device;
                    break;

                case DiscoveryScenarioType.DiscoverAsGo:
                    deviceFilterBitmask = DOT11_WFD_DISCOVER_DEVICE_FILTER_BITMASK.GO;
                    break;

                default:
                    throw new Exception("Unable to map from discovery scenario type to discovery filter bitmask");
            }

            //
            // Perform the discovery
            //

            WiFiDirectTestLogger.Log("Performing targetted discovery for device {0} ({1}) as {2}, Timeout: {3} ms, Discovery Type: {4}",
                remoteWFDController.DeviceAddress,
                remoteWFDController.MachineName,
                GetFriendlyStringForDiscoveryScenarioType(discoveryScenarioType),
                searchTimeoutMs,
                GetFriendlyStringForDiscoverType(discoverType)
                );

            WfdDeviceDescriptor discoveredDeviceDescriptor = localWFDController.PerformTargetedDiscovery(
                remoteWFDController.DeviceAddress,
                deviceFilterBitmask,
                searchTimeoutMs,
                discoverType
                );
            if (discoveredDeviceDescriptor == null)
            {
                WiFiDirectTestLogger.Error("Target device {0} ({1}) not found.", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            }

            return discoveredDeviceDescriptor;
        }

        private WfdDeviceDescriptor PerformWildcardDiscovery()
        {
            WfdDeviceDescriptor discoveredDeviceDescriptor = null;

            //
            // Perform the discovery
            //

            WiFiDirectTestLogger.Log("Performing wildcard discovery for device {0} ({1}) as {2}, Timeout: {3} ms, Discovery Type: {4}",
                remoteWFDController.DeviceAddress,
                remoteWFDController.MachineName,
                GetFriendlyStringForDiscoveryScenarioType(discoveryScenarioType),
                searchTimeoutMs,
                GetFriendlyStringForDiscoverType(discoverType)
                );

            List<WfdDeviceDescriptor> discoveryResults = localWFDController.DiscoverDevices(searchTimeoutMs, discoverType);

            WiFiDirectTestLogger.Log("Discovery returned {0} results.", discoveryResults == null ? 0 : discoveryResults.Count);

            if (discoveryResults != null)
            {
                foreach(WfdDeviceDescriptor discoveryResult in discoveryResults)
                {
                    if (discoveryResult.DeviceAddress == remoteWFDController.DeviceAddress)
                    {
                        WiFiDirectTestLogger.Log("Target device {0} ({1}) found in discovery results", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);

                        if (discoveryScenarioType == DiscoveryScenarioType.DiscoverAsGo &&
                            !discoveryResult.GroupCapabilities.GroupOwner)
                        {
                            WiFiDirectTestLogger.Error("Expected to find device {0} ({1}) as a group owner, but there's no GO running on the device.", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
                        }
                        else
                        {
                            discoveredDeviceDescriptor = discoveryResult;
                        }

                        break;
                    }
                }
            }

            if (discoveredDeviceDescriptor == null)
            {
                WiFiDirectTestLogger.Error("Target device {0} ({1}) not found.", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            }

            return discoveredDeviceDescriptor;
        }

        private string GetFriendlyStringForDiscoveryScenarioType(DiscoveryScenarioType discoveryScenarioType)
        {
            string result = "<Unrecognized Discovery Scenario Type>";

            switch(discoveryScenarioType)
            {
                case DiscoveryScenarioType.DiscoverAsDevice:
                    result = "Device";
                    break;
                case DiscoveryScenarioType.DiscoverAsGo:
                    result = "Group Owner";
                    break;
            }

            return result;
        }

        private string GetFriendlyStringForDiscoverType(WFD_DISCOVER_TYPE discoverType)
        {
            string result = "<Unrecognized Discover Type>";

            switch(discoverType)
            {
                case WFD_DISCOVER_TYPE.wfd_discover_type_scan_only:
                    result = "Scan Only";
                    break;
                case WFD_DISCOVER_TYPE.wfd_discover_type_find_only:
                    result = "Find Only";
                    break;
                case WFD_DISCOVER_TYPE.wfd_discover_type_no_preference:
                case WFD_DISCOVER_TYPE.wfd_discover_type_auto:
                    result = "Auto";
                    break;
                case WFD_DISCOVER_TYPE.wfd_discover_type_scan_social_channels:
                    result = "Scan Social Channels";
                    break;
            }

            return result;
        }
    }
}
