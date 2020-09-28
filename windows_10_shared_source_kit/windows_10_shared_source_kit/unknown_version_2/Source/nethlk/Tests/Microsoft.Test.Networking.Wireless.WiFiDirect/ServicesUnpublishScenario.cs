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
    internal class ServicesUnpublishParameters
    {
        public ServicesUnpublishParameters(
            WFDSvcWrapperHandle advertiserHandle,
            bool remove = true
            )
        {
            AdvertiserHandle = advertiserHandle;
            Remove = remove;
        }

        public WFDSvcWrapperHandle AdvertiserHandle { get; private set; }
        public bool Remove { get; private set; }
    }

    internal class ServicesUnpublishScenarioResult
    {
        public ServicesUnpublishScenarioResult(
            bool scenarioSucceeded
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded { get; private set; }
    }

    internal class ServicesUnpublishScenario
    {
        public ServicesUnpublishScenario(
            WiFiDirectTestController advertisingWFDController,
            ServicesUnpublishParameters unpublishParameters
            )
        {
            this.advertisingWFDController = advertisingWFDController;
            this.unpublishParameters = unpublishParameters;
        }

        public ServicesUnpublishScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesUnpublishScenarioResult(succeeded);
        }

        private bool succeeded = false;
        private WiFiDirectTestController advertisingWFDController;
        private ServicesUnpublishParameters unpublishParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Starting Unpublish for service with handle {0} on device {1} ({2})",
                    unpublishParameters.AdvertiserHandle,
                    advertisingWFDController.DeviceAddress,
                    advertisingWFDController.MachineName
                    );

                advertisingWFDController.UnpublishService(
                    unpublishParameters.AdvertiserHandle,
                    unpublishParameters.Remove
                    );

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service unpublish scenario: {0}", e);
            }
        }
    }
}
