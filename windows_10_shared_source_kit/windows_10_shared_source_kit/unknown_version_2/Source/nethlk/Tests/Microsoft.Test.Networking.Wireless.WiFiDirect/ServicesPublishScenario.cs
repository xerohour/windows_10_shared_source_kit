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
    internal enum ServicePublishConfigMethods
    {
        NotSet, // Use the default
        PinOrDefaultDisplay,
        PinOnlyDisplay,
        // These are less common for the advertising side
        PinOrDefaultKeypad,
        PinOnlyKeypad,
        Any,
        PinOnlyDisplayKeypad
    }

    internal class ServicesPublishParameters
    {
        public ServicesPublishParameters(
            string serviceName,
            bool autoAccept = true,
            bool preferGO = true,
            ServicePublishConfigMethods configMethods = ServicePublishConfigMethods.PinOrDefaultDisplay,
            List<string> prefixList = null,
            string serviceInfo = "",
            string sessionInfo = ""
            )
        {
            ServiceName = serviceName;
            AutoAccept = autoAccept;
            PreferGO = preferGO;
            ConfigMethods = configMethods;
            PrefixList = prefixList;
            ServiceInfo = serviceInfo;
            SessionInfo = sessionInfo;
        }

        public string ServiceName { get; private set; }
        public bool AutoAccept { get; private set; }
        public bool PreferGO { get; private set; }
        public ServicePublishConfigMethods ConfigMethods { get; private set; }
        public List<string> PrefixList { get; private set; }
        public string ServiceInfo { get; private set; }
        public string SessionInfo { get; private set; }
    }

    internal class ServicesPublishScenarioResult
    {
        public ServicesPublishScenarioResult(
            bool scenarioSucceeded,
            WFDSvcWrapperHandle advertiserHandle
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            AdvertiserHandle = advertiserHandle;
        }

        public bool ScenarioSucceeded { get; private set; }
        public WFDSvcWrapperHandle AdvertiserHandle { get; private set; }
    }

    internal class ServicesPublishScenario
    {
        public ServicesPublishScenario(
            WiFiDirectTestController advertisingWFDController,
            ServicesPublishParameters publishParameters
            )
        {
            this.advertisingWFDController = advertisingWFDController;
            this.publishParameters = publishParameters;
        }

        public ServicesPublishScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesPublishScenarioResult(succeeded, handle);
        }

        private bool succeeded = false;
        private WFDSvcWrapperHandle handle = null;
        private WiFiDirectTestController advertisingWFDController;
        private ServicesPublishParameters publishParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Starting Publish for service \"{0}\" on device {1} ({2})",
                    publishParameters.ServiceName,
                    advertisingWFDController.DeviceAddress,
                    advertisingWFDController.MachineName
                    );

                List<WiFiDirectServiceConfigurationMethod> configMethods = null;

                if (publishParameters.ConfigMethods != ServicePublishConfigMethods.NotSet)
                {
                    configMethods = new List<WiFiDirectServiceConfigurationMethod>();

                    if (publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOnlyDisplay ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOnlyDisplayKeypad ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOrDefaultDisplay ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.Any)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.PinDisplay);
                    }
                    if (publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOnlyKeypad ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOnlyDisplayKeypad ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOrDefaultKeypad ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.Any)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.PinEntry);
                    }
                    if (publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOrDefaultDisplay ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.PinOrDefaultKeypad ||
                        publishParameters.ConfigMethods == ServicePublishConfigMethods.Any)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.Default);
                    }
                }

                handle = advertisingWFDController.PublishService(
                    publishParameters.ServiceName,
                    publishParameters.AutoAccept,
                    publishParameters.PreferGO,
                    configMethods,
                    WiFiDirectServiceStatus.Available, // TODO status
                    0, // TODO custom status
                    publishParameters.ServiceInfo,
                    publishParameters.SessionInfo,
                    publishParameters.PrefixList
                    );

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service publish scenario: {0}", e);
            }
        }
    }
}
