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
    internal class ServicesOpenSocketSendDataParameters
    {
        public ServicesOpenSocketSendDataParameters(
            WFDSvcWrapperHandle senderSessionHandle,
            WFDSvcWrapperHandle receiverSessionHandle,
            WiFiDirectServiceIPProtocol protocol,
            UInt16 port,
            UInt32 dataSize
            )
        {
            SenderSessionHandle = senderSessionHandle;
            ReceiverSessionHandle = receiverSessionHandle;
            Protocol = protocol;
            Port = port;
            DataSize = dataSize;
        }

        public WFDSvcWrapperHandle SenderSessionHandle { get; private set; }
        public WFDSvcWrapperHandle ReceiverSessionHandle { get; private set; }
        public WiFiDirectServiceIPProtocol Protocol { get; private set; }
        public UInt16 Port { get; private set; }
        public UInt32 DataSize { get; private set; }
    }

    internal class ServicesOpenSocketSendDataScenarioResult
    {
        public ServicesOpenSocketSendDataScenarioResult(
            bool scenarioSucceeded,
            ServicesOpenSocketScenarioResult openSocketResult
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            OpenSocketResult = openSocketResult;
        }

        public bool ScenarioSucceeded { get; private set; }
        public ServicesOpenSocketScenarioResult OpenSocketResult { get; private set; }
    }

    internal class ServicesOpenSocketSendDataScenario
    {
        public ServicesOpenSocketSendDataScenario(
            WiFiDirectTestController senderWFDController,
            WiFiDirectTestController receiverWFDController,
            ServicesOpenSocketSendDataParameters socketDataParameters
            )
        {
            this.senderWFDController = senderWFDController;
            this.receiverWFDController = receiverWFDController;
            this.socketDataParameters = socketDataParameters;
        }

        public ServicesOpenSocketSendDataScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesOpenSocketSendDataScenarioResult(
                succeeded,
                openSocketResult
                );
        }

        private bool succeeded = false;
        ServicesOpenSocketScenarioResult openSocketResult = null;
        private WiFiDirectTestController senderWFDController;
        private WiFiDirectTestController receiverWFDController;
        private ServicesOpenSocketSendDataParameters socketDataParameters;

        private void ExecuteInternal()
        {
            try
            {
                var openSocketScenario = new ServicesOpenSocketScenario(
                    senderWFDController,
                    receiverWFDController,
                    new ServicesOpenSocketParameters(
                        socketDataParameters.SenderSessionHandle,
                        socketDataParameters.ReceiverSessionHandle,
                        socketDataParameters.Protocol,
                        socketDataParameters.Port
                        )
                    );
                openSocketResult = openSocketScenario.Execute();

                if (!openSocketResult.ScenarioSucceeded)
                {
                    throw new Exception("Open Socket failed!");
                }

                ServicesSendDataScenario sendDataScenario = null;

                if (socketDataParameters.Protocol == WiFiDirectServiceIPProtocol.Tcp)
                {
                    sendDataScenario = new ServicesSendDataScenario(
                        senderWFDController,
                        receiverWFDController,
                        new ServicesSendDataParameters(
                            socketDataParameters.SenderSessionHandle,
                            openSocketResult.SenderSocketHandle,
                            socketDataParameters.ReceiverSessionHandle,
                            openSocketResult.ReceiverSocketHandle,
                            socketDataParameters.DataSize
                            )
                        );
                }
                else
                {
                    // special case for UDP, just send on the opposite side for now
                    sendDataScenario = new ServicesSendDataScenario(
                        receiverWFDController,
                        senderWFDController,
                        new ServicesSendDataParameters(
                            socketDataParameters.ReceiverSessionHandle,
                            openSocketResult.ReceiverSocketHandle,
                            socketDataParameters.SenderSessionHandle,
                            openSocketResult.SenderSocketHandle,
                            socketDataParameters.DataSize
                            )
                        );
                }
                ServicesSendDataScenarioResult sendDataResult = sendDataScenario.Execute();

                if (!sendDataResult.ScenarioSucceeded)
                {
                    throw new Exception("Send Data failed!");
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
