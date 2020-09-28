///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Diagnostics;

using Microsoft.Test.Networking.Wireless;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal enum PairingScenarioType
    {
        Invitation,
        GoNegotiationDutBecomesGo,
        GoNegotiationDutBecomesClient,
        JoinExistingGo
    }

    internal class PairingScenarioResult
    {
        public PairingScenarioResult(bool scenarioSucceeded)
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded {get; private set;}
    }

    internal class PairingScenario
    {
        public PairingScenario(
            WiFiDirectTestController localWFDController,
            WiFiDirectTestController remoteWFDController,
            PairingScenarioType pairingScenarioType,
            DOT11_WPS_CONFIG_METHOD configMethod,
            bool isPersistent,
            bool runDataPathValidation
            )
        {
            this.succeeded = false;
            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;
            this.pairingScenarioType = pairingScenarioType;

            if (configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY &&
                configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON &&
                configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD)
            {
                throw new ArgumentException("Invalid value for configMethod.  Expect pin display, pushbutton, or keypad.", "configMethod");
            }
            this.configMethod = configMethod;
            this.isPersistent = isPersistent;
            this.runDataPathValidation = runDataPathValidation;
        }

        public PairingScenarioResult Execute()
        {
            ExecuteInternal();

            return new PairingScenarioResult(succeeded);
        }

        private bool succeeded;
        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private PairingScenarioType pairingScenarioType;
        private DOT11_WPS_CONFIG_METHOD configMethod;
        private bool isPersistent;
        private bool runDataPathValidation;

        private const uint pairingScenarioDiscoveryTimeoutMs = 10000;

        private void ExecuteInternal()
        {
            try
            {
                // For non-pushbutton pairing, generate a new passkey and make sure both test controllers have it.
                if (configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON)
                {
                    remoteWFDController.PassKey = localWFDController.GenerateAndStoreNewPassKey();
                }

                // Prepare the remote device to receive the connection request.
                remoteWFDController.AcceptNextGroupRequest(
                    localWFDController.DeviceAddress,
                    (pairingScenarioType == PairingScenarioType.GoNegotiationDutBecomesGo) ? ((byte) 0) : ((byte) 14),
                    GetAcceptConfigMethod()
                    );

                // Begin by performing a targeted discovery for the remote device.
                bool discoverySucceeded = PerformTargetedDiscovery();
                if (!discoverySucceeded)
                {
                    return;
                }

                WFD_PAIR_WITH_DEVICE_PREFERENCE pairWithDevicePreference;

                switch(this.pairingScenarioType)
                {
                    case PairingScenarioType.JoinExistingGo:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_NONE;
                        break;

                    case PairingScenarioType.Invitation:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_INVITATION;
                        break;

                    case PairingScenarioType.GoNegotiationDutBecomesGo:
                    case PairingScenarioType.GoNegotiationDutBecomesClient:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_GO_NEGOTIATION;
                        break;

                    default:
                        throw new Exception("Cannot map pairing scenario to pairing preference.");
                }

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                // Pair with the target device.
                WiFiDirectTestLogger.Log("Starting pairing with device {0} ({1})", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
                localWFDController.PairWithDevice(
                    remoteWFDController.DeviceAddress,
                    pairWithDevicePreference,
                    (pairingScenarioType == PairingScenarioType.GoNegotiationDutBecomesClient) ? ((byte) 0) : ((byte) 14),
                    configMethod,
                    isPersistent
                    );
                WiFiDirectTestLogger.Log("Pairing successful", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);

                stopwatch.Stop();
                WiFiDirectTestLogger.Log("Pairing completed in {0} ms.", stopwatch.ElapsedMilliseconds);

                bool sessionStateValid = VerifySessionState();
                if (!sessionStateValid)
                {
                    return;
                }

                if (this.runDataPathValidation)
                {
                    WiFiDirectTestUtilities.RunDataPathValidation(localWFDController, remoteWFDController);
                }

                succeeded = true;
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing pairing scenario: {0}", e);
            }
        }

        private DOT11_WPS_CONFIG_METHOD GetAcceptConfigMethod()
        {
            DOT11_WPS_CONFIG_METHOD acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON;
            switch(configMethod)
            {
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY:
                    acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD;
                    break;
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD:
                    acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY;
                    break;
            }
            return acceptConfigMethod;
        }

        private bool PerformTargetedDiscovery()
        {
            DiscoveryScenarioType discoveryScenarioType;

            switch(pairingScenarioType)
            {
                case PairingScenarioType.JoinExistingGo:
                    discoveryScenarioType = DiscoveryScenarioType.DiscoverAsGo;
                    break;

                case PairingScenarioType.Invitation:
                case PairingScenarioType.GoNegotiationDutBecomesGo:
                case PairingScenarioType.GoNegotiationDutBecomesClient:
                    discoveryScenarioType = DiscoveryScenarioType.DiscoverAsDevice;
                    break;

                default:
                    throw new Exception("Cannot map pairing scenario to discovery scenario.");
            }

            DiscoveryScenario discoveryScenario = new DiscoveryScenario(
                localWFDController,
                remoteWFDController,
                discoveryScenarioType,
                true, // isTargettedDiscovery
                WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                pairingScenarioDiscoveryTimeoutMs,
                false // discoverTestIe
                );
            DiscoveryScenarioResult discoveryScenarioResult = discoveryScenario.Execute();

            if (!discoveryScenarioResult.ScenarioSucceeded)
            {
                WiFiDirectTestLogger.Error("Targetted discovery failed for device {0} ({1}).  Aborting pairing operation.", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            }

            return discoveryScenarioResult.ScenarioSucceeded;
        }

        private bool VerifySessionState()
        {
            WfdGlobalSessionState globalSessionState = localWFDController.QueryGlobalSessionState();

            if ( globalSessionState.Sessions.Length != 1 )
            {
                WiFiDirectTestLogger.Error("Expected a single active session.  Current session count = {0}", globalSessionState.Sessions.Length );
                return false;
            }

            WFD_ROLE_TYPE expectedRole;
            switch(pairingScenarioType)
            {
                case PairingScenarioType.Invitation:
                case PairingScenarioType.GoNegotiationDutBecomesGo:
                    expectedRole = WFD_ROLE_TYPE.WFD_ROLE_TYPE_GROUP_OWNER;
                    break;

                case PairingScenarioType.GoNegotiationDutBecomesClient:
                case PairingScenarioType.JoinExistingGo:
                    expectedRole = WFD_ROLE_TYPE.WFD_ROLE_TYPE_CLIENT;
                    break;

                default:
                    throw new Exception("Unable to map pairing scenario type to role type.");
            }

            if ( globalSessionState.Sessions[0].Role != expectedRole )
            {
                WiFiDirectTestLogger.Error("DUT became the WFD {0}.  Expected {1}", MapRoleToString(globalSessionState.Sessions[0].Role), MapRoleToString(expectedRole));
                return false;
            }

            return true;
        }

        private string MapRoleToString(WFD_ROLE_TYPE roleType)
        {
            return (roleType == WFD_ROLE_TYPE.WFD_ROLE_TYPE_GROUP_OWNER) ? "GO" : "Client";
        }
    }
}
