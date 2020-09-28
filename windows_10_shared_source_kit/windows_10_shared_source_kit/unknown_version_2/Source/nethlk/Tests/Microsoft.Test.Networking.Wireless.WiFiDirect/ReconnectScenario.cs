///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal enum ReconnectScenarioType
    {
        SendReinvokeInvitationToClient,
        SendReinvokeInvitationToGo,
        AssociateToExistingGo
    }

    internal class ReconnectScenarioResult
    {
        public ReconnectScenarioResult(bool scenarioSucceeded)
        {
            this.ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded {get; private set;}
    }

    internal class ReconnectScenario
    {
        public ReconnectScenario(
            WiFiDirectTestController localWFDController,
            WiFiDirectTestController remoteWFDController,
            ReconnectScenarioType reconnectScenarioType
            )
        {
            if (localWFDController == null)
            {
                throw new ArgumentNullException("localWFDController");
            }
            if (remoteWFDController == null)
            {
                throw new ArgumentNullException("remoteWFDController");
            }

            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;
            this.reconnectScenarioType = reconnectScenarioType;
        }

        public ReconnectScenarioResult Execute()
        {
            try
            {
                bool pairingSucceeded = Pair();
                if (!pairingSucceeded)
                {
                    return new ReconnectScenarioResult(false);
                }

                Disconnect();

                bool discoverySucceeded = Discover();
                if (!discoverySucceeded)
                {
                    return new ReconnectScenarioResult(false);
                }

                Reconnect();

                WiFiDirectTestUtilities.RunDataPathValidation(localWFDController, remoteWFDController);
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing reconnect scenario: {0}", e);
                return new ReconnectScenarioResult(false);
            }

            return new ReconnectScenarioResult(true);
        }

        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private ReconnectScenarioType reconnectScenarioType;

        private bool Pair()
        {
            PairingScenario pairingScenario = new PairingScenario(
                this.localWFDController,
                this.remoteWFDController,
                this.reconnectScenarioType == ReconnectScenarioType.SendReinvokeInvitationToClient ? PairingScenarioType.Invitation : PairingScenarioType.JoinExistingGo,
                DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON,
                true /* persistent */,
                false /* skip data path validation */
                );

            PairingScenarioResult pairingScenarioResult = pairingScenario.Execute();

            return pairingScenarioResult.ScenarioSucceeded;
        }

        private void Disconnect()
        {
            this.localWFDController.CloseSession();
            this.remoteWFDController.CloseSession();
        }

        private bool Discover()
        {
            DiscoveryScenario discoveryScenario = new DiscoveryScenario(
                this.localWFDController,
                this.remoteWFDController,
                this.reconnectScenarioType == ReconnectScenarioType.SendReinvokeInvitationToClient ? DiscoveryScenarioType.DiscoverAsDevice : DiscoveryScenarioType.DiscoverAsGo,
                true, /* targetted discovery */
                WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                10000, /* 10 second timeout */
                false /* skip Vendor Specific IE validation */
                );

            DiscoveryScenarioResult discoveryScenarioResult = discoveryScenario.Execute();

            return discoveryScenarioResult.ScenarioSucceeded;
        }

        private void Reconnect()
        {
            WiFiDirectTestLogger.Log("Reconnecting to {0} ({1})", this.remoteWFDController.DeviceAddress, this.remoteWFDController.MachineName);

            this.remoteWFDController.AcceptNextConnectRequest();
            this.localWFDController.OpenSessionToLastPairedDevice();
        }
    }
}
