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
    internal class ServicesDisconnectParameters
    {
        public ServicesDisconnectParameters(
            WFDSvcWrapperHandle sessionHandle,
            WFDSvcWrapperHandle otherSessionHandle
            )
        {
            SessionHandle = sessionHandle;
            OtherSessionHandle = otherSessionHandle;
        }

        public WFDSvcWrapperHandle SessionHandle { get; private set; }
        public WFDSvcWrapperHandle OtherSessionHandle { get; private set; }
    }

    internal class ServicesDisconnectScenarioResult
    {
        public ServicesDisconnectScenarioResult(
            bool scenarioSucceeded
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded { get; private set; }
    }

    internal class ServicesDisconnectScenario
    {
        public ServicesDisconnectScenario(
            WiFiDirectTestController localWFDController,
            WiFiDirectTestController remoteWFDController,
            ServicesDisconnectParameters disconnectParameters
            )
        {
            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;
            this.disconnectParameters = disconnectParameters;
        }

        public ServicesDisconnectScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesDisconnectScenarioResult(succeeded);
        }

        private bool succeeded = false;
        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private ServicesDisconnectParameters disconnectParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestController disconnectTestController = localWFDController;
                WiFiDirectTestController otherTestController = remoteWFDController;

                WiFiDirectTestLogger.Log(
                    "Starting Disconnect for session with handle {0} on device {1} ({2}), expect session with handle {3} to close on device {4} ({5})",
                    disconnectParameters.SessionHandle,
                    disconnectTestController.DeviceAddress,
                    disconnectTestController.MachineName,
                    disconnectParameters.OtherSessionHandle,
                    otherTestController.DeviceAddress,
                    otherTestController.MachineName
                    );

                disconnectTestController.DisconnectServiceSession(disconnectParameters.SessionHandle);

                otherTestController.WaitForDisconnectServiceSession(disconnectParameters.OtherSessionHandle);

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service session disconnect scenario: {0}", e);
            }
        }
    }
}
