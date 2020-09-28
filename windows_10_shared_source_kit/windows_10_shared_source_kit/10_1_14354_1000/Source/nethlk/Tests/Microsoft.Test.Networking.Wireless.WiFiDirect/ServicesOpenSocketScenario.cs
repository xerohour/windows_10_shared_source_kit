///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal class ServicesOpenSocketParameters
    {
        public ServicesOpenSocketParameters(
            WFDSvcWrapperHandle senderSessionHandle,
            WFDSvcWrapperHandle receiverSessionHandle,
            WiFiDirectServiceIPProtocol protocol,
            UInt16 port
            )
        {
            SenderSessionHandle = senderSessionHandle;
            ReceiverSessionHandle = receiverSessionHandle;
            Protocol = protocol;
            Port = port;
        }

        public WFDSvcWrapperHandle SenderSessionHandle { get; private set; }
        public WFDSvcWrapperHandle ReceiverSessionHandle { get; private set; }
        public WiFiDirectServiceIPProtocol Protocol { get; private set; }
        public UInt16 Port { get; private set; }
    }

    internal class ServicesOpenSocketScenarioResult
    {
        public ServicesOpenSocketScenarioResult(
            bool scenarioSucceeded,
            WFDSvcWrapperHandle senderSocketHandle,
            WFDSvcWrapperHandle receiverSocketHandle
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            SenderSocketHandle = senderSocketHandle;
            ReceiverSocketHandle = receiverSocketHandle;
        }

        public bool ScenarioSucceeded { get; private set; }
        public WFDSvcWrapperHandle SenderSocketHandle { get; private set; }
        public WFDSvcWrapperHandle ReceiverSocketHandle { get; private set; }
    }

    internal class ServicesOpenSocketScenario
    {
        public ServicesOpenSocketScenario(
            WiFiDirectTestController senderWFDController,
            WiFiDirectTestController receiverWFDController,
            ServicesOpenSocketParameters socketParameters
            )
        {
            this.senderWFDController = senderWFDController;
            this.receiverWFDController = receiverWFDController;
            this.socketParameters = socketParameters;
        }

        public ServicesOpenSocketScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesOpenSocketScenarioResult(
                succeeded,
                senderSocketHandle,
                receiverSocketHandle
                );
        }

        private bool succeeded = false;
        private WFDSvcWrapperHandle senderSocketHandle = null;
        private WFDSvcWrapperHandle receiverSocketHandle = null;
        private WiFiDirectTestController senderWFDController;
        private WiFiDirectTestController receiverWFDController;
        private ServicesOpenSocketParameters socketParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Starting open socket ({6} {7}) from session with handle {0} on device {1} ({2}), expect socket added on session with handle {3} on device {4} ({5})",
                    socketParameters.SenderSessionHandle,
                    senderWFDController.DeviceAddress,
                    senderWFDController.MachineName,
                    socketParameters.ReceiverSessionHandle,
                    receiverWFDController.DeviceAddress,
                    receiverWFDController.MachineName,
                    socketParameters.Protocol.ToString(),
                    socketParameters.Port
                    );

                if (socketParameters.Protocol == WiFiDirectServiceIPProtocol.Tcp)
                {
                    senderSocketHandle = senderWFDController.AddServiceStreamSocket(
                        socketParameters.SenderSessionHandle,
                        socketParameters.Port
                        );

                    receiverSocketHandle = receiverWFDController.GetServiceRemoteSocketAdded(
                        socketParameters.ReceiverSessionHandle
                        );
                }
                else if (socketParameters.Protocol == WiFiDirectServiceIPProtocol.Udp)
                {
                    senderSocketHandle = senderWFDController.AddServiceDatagramSocket(
                        socketParameters.SenderSessionHandle,
                        socketParameters.Port
                        );

                    receiverSocketHandle = receiverWFDController.GetServiceRemoteSocketAdded(
                        socketParameters.ReceiverSessionHandle
                        );
                }
                else
                {
                    throw new Exception("Unsupported Protocol!");
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service open socket scenario: {0}", e);
            }
        }
    }
}
