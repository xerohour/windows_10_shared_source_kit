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
    internal class ServicesSendDataParameters
    {
        public ServicesSendDataParameters(
            WFDSvcWrapperHandle senderSessionHandle,
            WFDSvcWrapperHandle senderSocketHandle,
            WFDSvcWrapperHandle receiverSessionHandle,
            WFDSvcWrapperHandle receiverSocketHandle,
            UInt32 dataSize
            )
        {
            SenderSessionHandle = senderSessionHandle;
            SenderSocketHandle = senderSocketHandle;
            ReceiverSessionHandle = receiverSessionHandle;
            ReceiverSocketHandle = receiverSocketHandle;
            DataSize = dataSize;
        }

        public WFDSvcWrapperHandle SenderSessionHandle { get; private set; }
        public WFDSvcWrapperHandle SenderSocketHandle { get; private set; }
        public WFDSvcWrapperHandle ReceiverSessionHandle { get; private set; }
        public WFDSvcWrapperHandle ReceiverSocketHandle { get; private set; }
        public UInt32 DataSize { get; private set; }
    }

    internal class ServicesSendDataScenarioResult
    {
        public ServicesSendDataScenarioResult(
            bool scenarioSucceeded
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded { get; private set; }
    }

    internal class ServicesSendDataScenario
    {
        public ServicesSendDataScenario(
            WiFiDirectTestController senderWFDController,
            WiFiDirectTestController receiverWFDController,
            ServicesSendDataParameters dataParameters
            )
        {
            this.senderWFDController = senderWFDController;
            this.receiverWFDController = receiverWFDController;
            this.dataParameters = dataParameters;
        }

        public ServicesSendDataScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesSendDataScenarioResult(
                succeeded
                );
        }

        private bool succeeded = false;
        private WiFiDirectTestController senderWFDController;
        private WiFiDirectTestController receiverWFDController;
        private ServicesSendDataParameters dataParameters;

        private readonly Random rng = new Random();
        private const string s_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Sending data ({6} char string) from socket with handle {0} on device {1} ({2}), expect received on socket with handle {3} on device {4} ({5})",
                    dataParameters.SenderSocketHandle,
                    senderWFDController.DeviceAddress,
                    senderWFDController.MachineName,
                    dataParameters.ReceiverSocketHandle,
                    receiverWFDController.DeviceAddress,
                    receiverWFDController.MachineName,
                    dataParameters.DataSize
                    );

                // Generate message of "DataSize" characters
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < dataParameters.DataSize; i++)
                {               
                    builder.Append(s_chars[rng.Next(s_chars.Length)]);
                }
                string msg = builder.ToString();

                senderWFDController.SendServiceSocketMessage(
                    dataParameters.SenderSessionHandle,
                    dataParameters.SenderSocketHandle,
                    msg
                    );

                string rcv = receiverWFDController.ReceiveServiceSocketMessage(
                    dataParameters.ReceiverSessionHandle,
                    dataParameters.ReceiverSocketHandle
                    );

                // Validate
                if (msg != rcv)
                {
                    WiFiDirectTestLogger.Error("Sent: '{0}', Received: '{1}'",
                        WiFiDirectTestUtilities.GetTruncatedString(msg, 32),
                        WiFiDirectTestUtilities.GetTruncatedString(rcv, 32)
                        );
                    throw new Exception("Received Message was not the same as expected!");
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service send data scenario: {0}", e);
            }
        }
    }
}
