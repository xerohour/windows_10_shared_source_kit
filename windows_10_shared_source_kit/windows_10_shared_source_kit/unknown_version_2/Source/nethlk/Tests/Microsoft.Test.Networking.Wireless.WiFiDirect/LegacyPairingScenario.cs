///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Globalization;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    class LegacyPairingScenarioResult
    {
        public bool ScenarioSucceeded {get; private set;}

        public LegacyPairingScenarioResult(bool scenarioSucceeded)
        {
            this.ScenarioSucceeded = scenarioSucceeded;
        }
    }

    class LegacyPairingScenario
    {
        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private bool scenarioSucceeded;

        public LegacyPairingScenario(WiFiDirectTestController localWFDController, WiFiDirectTestController remoteWFDController)
        {
            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;

            this.scenarioSucceeded = false;
        }

        public LegacyPairingScenarioResult Execute()
        {
            try
            {
                ExecuteInternal();
                this.scenarioSucceeded = true;
            }
            catch(Exception exception)
            {
                WiFiDirectTestLogger.Error("Caught exception while running legacy pairing scenario: {0}", exception);
            }

            return new LegacyPairingScenarioResult(this.scenarioSucceeded);
        }

        private void ExecuteInternal()
        {
            WiFiDirectTestLogger.Log("Starting Autonomous GO on device {0} ({1})", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            remoteWFDController.StartAutonomousGo();

            string goProfile = remoteWFDController.GetDefaultGoProfile();
            string clientProfile = createClientProfileFromGoProfile(goProfile);

            //WiFiDirectTestLogger.Log("GO Profile {0}", goProfile);
            //WiFiDirectTestLogger.Log("Client Profile {0}", clientProfile);

            WiFiDirectTestLogger.Log(
                "Preparing GO on device {0} ({1}) to receive legacy connection from device {2} ({3})",
                remoteWFDController.DeviceAddress,
                remoteWFDController.MachineName,
                localWFDController.DeviceAddress,
                localWFDController.MachineName
                );
            remoteWFDController.PrepareForLegacyConnectionAttempt(localWFDController.DeviceAddress);

            WiFiDirectTestLogger.Log("Performing legacy connection with device {0} ({1})", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            localWFDController.LegacyConnectToGo(clientProfile);
            WiFiDirectTestLogger.Log("Legacy connection established");
        }

        private const string clientProfileFormatString = 
            "<?xml version=\"1.0\"?>" +
            "<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">" +
                "<name>{0}</name>" +
                "<SSIDConfig><SSID><name>{0}</name></SSID></SSIDConfig>" +
                "<connectionType>ESS</connectionType>" +
                "<connectionMode>auto</connectionMode>" +
                "<MSM>" +
                    "<security>" +
                        "<authEncryption><authentication>WPA2PSK</authentication><encryption>AES</encryption><useOneX>false</useOneX></authEncryption>" +
                        "<sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>{1}</keyMaterial></sharedKey>" +
                    "</security>" +
                "</MSM>" +
            "</WLANProfile>";

        private string createClientProfileFromGoProfile(string goProfile)
        {
            string ssid, passphrase;
            localWFDController.ParseGoProfile(goProfile, out ssid, out passphrase);
            return String.Format(CultureInfo.InvariantCulture, clientProfileFormatString, ssid, passphrase);
        }
    }
}
