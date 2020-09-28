///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Text;
using System.Diagnostics.CodeAnalysis;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using System.Diagnostics;
using System.Threading;
using System.Net;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.Dhcp;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Power;
using System.Runtime.InteropServices;
using System.Globalization;
using Helpers = Microsoft.Test.Networking.Kit.Helpers;

namespace HlkTest.Dot11WTests
{    
    [TestClass]
    public class Dot11WTests
    {
        private const int DisconnectWaitSec = 10;
        private const int ProfileConnectWaitSec = 60;
        private const int ScanWaitSec = 20;

        internal static TimeSpan DisconnectWait = new TimeSpan(0, 0, DisconnectWaitSec);
        internal static TimeSpan ConnectWait = new TimeSpan(0, 0, ProfileConnectWaitSec);
        internal static TimeSpan ScanWait = new TimeSpan(0, 0, ScanWaitSec);

        private static string dot11wSSID = "";
        private static string dot11wPassword = "";
        private static bool enableSleepTest = false;

        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("Dot11WTests");

        TestContext context;
        public TestContext TestContext
        {
            get { return context; }
            set { context = value; }
        }

        private static RunTimeConfiguration sConfig;

        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
            sConfig = new RunTimeConfiguration(context);
        }

        [TestInitialize]
        public void TestSetup()
        {
            if (context.Properties.Contains("Dot11wSSID"))
            {
                dot11wSSID = context.Properties["Dot11wSSID"].ToString();
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Dot11wSSID = {0}", dot11wSSID));
            }
            else
            {
                Log.Error("Dot11wSSID is a required field");
                return;
            }

            if (context.Properties.Contains("Dot11wPassword"))
            {
                dot11wPassword = context.Properties["Dot11wPassword"].ToString();
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Dot11wPassword = {0}", dot11wPassword));
            }
            else
            {
                Log.Error("Dot11wPassword is a required field");
                return;
            }

            int customOptions = sConfig.WlanTestOptions;
            if (customOptions != 0)
                enableSleepTest = true;
        }

        [TestCleanup]
        public void TestCleanup()
        {
            testLogger.LogComment("Entering: Suite Cleanup");
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                // Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                // Delete Profile if present
                DisconnectAndDeleteProfile(wlanApi);
            }

            testLogger.LogComment("Exiting:Suite Cleanup");
        }


        [DllImport("wlanapi.dll", SetLastError = false)]
        internal static extern Win32Error WlanGetMFPNegotiated(
            ref Guid pInterfaceGuid,
            ref bool pfMFPNegotiated);


        private void DisconnectAndDeleteProfile(Wlan wlanApi)
        {
            var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
            Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
            if (wlanInterfaceList[0].State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
            {
                testLogger.LogComment("Disconnecting from AP");
                wlanApi.Disconnect(wlanInterfaceList[0].Id);
                Wlan.Sleep(5000);
            }

            try
            {
                testLogger.LogComment("Deleting Profile");
                wlanApi.DeleteProfile(wlanInterfaceList[0].Id, dot11wSSID);
            }
            catch (System.Runtime.InteropServices.COMException)
            {
                // The profile was not present to delete
            }
        }

        private bool IsConnected(Wlan wlanApi)
        {
            var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
            // Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
            if (wlanInterfaceList[0].State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        private bool IsMfpNeogotiated(Guid wlanInterfaceId)
        {
            bool negotiated = false;
            Win32Error retVal = WlanGetMFPNegotiated(ref wlanInterfaceId, ref negotiated);
            if (retVal != Win32Error.ERROR_SUCCESS)
            {
                testLogger.LogError("WlanGetMFPNegotiated Returned " + retVal);
                throw InteropUtilities.GetExceptionForWin32Error(retVal);
            }
            return negotiated;

        }

        private string GenerateProfile()
        {
            SSIDConfig ssidConfig = new SSIDConfig();
            ssidConfig.SSIDs.Add(new SSIDClass(dot11wSSID, String.Empty));
            MSMElement msmElement = new MSMElement();
            msmElement.Security.SharedKey.KeyType = KeyType.PassPhrase;
            msmElement.Security.SharedKey.KeyMaterial = dot11wPassword;

            WlanProfile wlanProfile = new WlanProfile(dot11wSSID, ssidConfig, ConnectionType.ESS, ConnectionMode.Manual, false, msmElement);
            return wlanProfile.ToXml();
        }


        [TestMethod]
        public void TestDot11wConnection()
        {
            Guid interfaceId;
            testLogger.LogComment("Testing 802.11w Connection to AP with SSID " + dot11wSSID + " and password " + dot11wPassword );
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                
                var wlanInterface = wlanInterfaceList[0];
                interfaceId = wlanInterface.Id;

                // Delete Profile if present
                DisconnectAndDeleteProfile(wlanApi);

                // Scan
                testLogger.LogComment("Scan");
                wlanApi.Scan(wlanInterface.Id, false, ScanWait);

                // Check to see if SSID is in scan list
                testLogger.LogComment("Check to see if SSID is in scan list");
                var networkList = wlanApi.GetAvailableNetworkList(interfaceId);
                bool found = false;
                foreach (WlanNetwork network in networkList)
                {
                    if (String.Compare(network.Ssid, dot11wSSID, StringComparison.Ordinal) == 0)
                    {
                        found = true;
                        break;
                    }
                }
                if (found == false)
                {
                    testLogger.LogError("Could not find 11w router in scan list: " + dot11wSSID);
                    testLogger.LogResult(TestResult.Failed);
                    return;
                }
                else
                {
                    testLogger.LogComment("Found the 11w router in the scan list: " + dot11wSSID);
                }

                // Create Profile
                string profile = GenerateProfile();
                testLogger.LogComment("Creating Profile");
                wlanApi.CreateProfile(interfaceId, profile);

                // Connect
                testLogger.LogComment("Connecting to AP");
                wlanApi.ProfileConnect(interfaceId, dot11wSSID, new TimeSpan(0, 0, 30));
            }

            // Check MFP Status
            bool mfpStatus = IsMfpNeogotiated(interfaceId);
            testLogger.LogComment("IsMfpNeogotiated == " + mfpStatus);
            if (mfpStatus == false)
            {
                testLogger.LogError("The device connected to the dot11w router but IsMfpNeogotiated == false");
                testLogger.LogResult(TestResult.Failed);
                return;
            }

            if (enableSleepTest)
            {
                PowerManager powerManager;

                powerManager = new PowerManager();

                // Wait for DHCP Stable
                testLogger.LogComment("Wait for DHCP to be stable");
                Wlan.Sleep(1000 * 60 * 2);

                testLogger.LogComment("Going into Connected standby/Sleep for 1 minute");
                powerManager.DeepestSleep(60);

                using (Wlan wlanApi = new Wlan())
                {
                    bool isConnected = IsConnected(wlanApi);
                    if (isConnected == false)
                    {
                        testLogger.LogError("After waking from connected standby the device is no longer connected to the AP");
                    }
                    else
                    {
                        testLogger.LogComment("After waking from connected standby the device is still connected to the AP");
                    }
                }

                // Check MFP Status
                mfpStatus = IsMfpNeogotiated(interfaceId);
                testLogger.LogComment("IsMfpNeogotiated == " + mfpStatus);
                if (mfpStatus == false)
                {
                    testLogger.LogError("The device connected to the dot11w router but IsMfpNeogotiated == false");
                    return;
                }
            }
        }

    }


}
