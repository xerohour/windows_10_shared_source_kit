///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using AuthServer8021X;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.Dhcp;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Helpers = Microsoft.Test.Networking.Kit.Helpers;

namespace HlkTest.RoamingTests
{

    [FlagsAttribute]
    public enum ROAM_TEST_OPTIONS
    {
        SuddenLossOfSignal = 1,
        SuddenDropOfSignal = 2,
        GradualLossOfSignal = 4,
        GradualDropOfSignal = 8,
        All = 0xF
    }

    public class WhckRoaming : IDisposable
    {
        private ROAM_TEST_OPTIONS m_RoamTestOptions;
        private RunTimeConfiguration RoamingConfiguration;

        private Wlan Api;

        private Collection<Guid> routers;


        public const int EchoPort = 7777;
        public const int EchoByteStream = 768;

        private ApConfigClient AC;

        private List<APConfig> APConfigs;

        private bool AccessPointSetup = false;

        private WlanHckTestLogger traceProvider;

        private WlanInterface TestInterface;

        private bool disposed = false;

        private TCPEchoClient echoClient = null;

        public bool DeviceSupports5Ghz { get; private set; }

        private string EnterpriseRadiusSecret = "ThisIsASecret";
        private Int32 EnterpriseRadiusPort = 1812;
        private string EnterpriseDomain = "corporate";
        private string EnterpriseUser = "johndoe";
        private string EnterprisePassword = "ihaveapasswd";

        // Note we currently have the same auth set on both 24GHZ and 5GHZ
        private SUPPORTED_PHASE1_TYPE phase1Auth = SUPPORTED_PHASE1_TYPE.EAP_PEAP;
        private SUPPORTED_PHASE2_TYPE phase2Auth = SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2;

        private APConfigParameter m_AP1Config24GHz;
        private APConfigParameter m_AP1Config5GHz;
        private APConfigParameter m_AP2Config24GHz;
        private APConfigParameter m_AP2Config5GHz;

        private void Log(String log)
        {
            WEX.Logging.Interop.Log.Comment(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        public void LogMajor(String log)
        {
            WEX.Logging.Interop.Log.Comment(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        private void LogError(String log)
        {
            WEX.Logging.Interop.Log.Error(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        public WhckRoaming(RunTimeConfiguration rc,
            APConfigParameter AP1Config24GHz,
            APConfigParameter AP1Config5GHz,
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            ROAM_TEST_OPTIONS RoamTestOptions)
        {
            if (rc == null)
            {
                throw new ArgumentNullException("rc");
            }
            traceProvider = new WlanHckTestLogger("WhckRoaming");

            this.RoamingConfiguration = rc;
            this.m_AP1Config24GHz = AP1Config24GHz;
            this.m_AP1Config5GHz = AP1Config5GHz;
            this.m_AP2Config24GHz = AP2Config24GHz;
            this.m_AP2Config5GHz = AP2Config5GHz;
            this.m_RoamTestOptions = RoamTestOptions;

            Api = new Wlan();

            TestInterface = Helpers.GetWlanInterface(Api);
            AC = Helpers.ConnectToService(RoamingConfiguration.ServiceBackChannelAddress);

            routers = Helpers.GetRouterCollection(AC);
            AccessPointSetup = false;
            DeviceSupports5Ghz = rc.DeviceSupports5Ghz;
        }

        public void Dispose()
        {
            Dispose(true);

            // This object will be cleaned up by the Dispose method.
            // Therefore, you should call GC.SupressFinalize to
            // take this object off the finalization queue 
            // and prevent finalization code for this object
            // from executing a second time.
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {

                if (disposing)
                {
                    Log("WhckRoaming : Cleaning up");
                    DeleteProfiles();

                    if (echoClient != null)
                    {
                        Log("WhckRoaming : Cleaning up Echo Client");
                        echoClient.Close();
                        echoClient = null;
                    }
                    if (null != Api)
                    {
                        Api.Dispose();
                        Api = null;
                    }
                    if (null != AC)
                    {
                        if (routers != null && routers.Count == 2)
                        {
                            Log("WhckRoaming: Cleaning up static route from AP server to router");
                            AC.ClearStaticRoute(routers[0]);
                        }
                        
                        Log("WhckRoaming : Cleaning up stopping radius server");
                        AC.StopRadiusServer();
                        Log("WhckRoaming : Cleaning up stopping Echo server");
                        AC.StopEchoServer(); // Service checks if echo server was running or not
                        AC.Dispose();
                        AC = null;
                    }

                    // TODO Dispose here
                }
            }

            disposed = true;
        }

        ~WhckRoaming()
        {
            Dispose(false);
        }

        public bool SetRadioState(int routerIndex, bool bEnabled, bool b5Ghz)
        {
            bool result = false;
            Log("Setting state on radio of router " + routerIndex + " to " + bEnabled + " 5 GHZ = " + b5Ghz);
            if (b5Ghz)
            {
                if (!AC.SetRadio5GHzState(routers[routerIndex], bEnabled))
                {
                    LogError("SetRadio5GHzState Failed to set the 5Ghz radio on router:" + routerIndex );
                    result = false;                    
                }
                else
                {
                    result = true;
                }
            }
            else
            {
                if (!AC.SetRadio24GHzState(routers[routerIndex], bEnabled))
                {                    
                    LogError("SetRadio24GHzState Failed to set the 2.4Ghz radio on router:" + routerIndex);
                    result = false;
                }
                else
                {
                    result = true;
                }

            }

            return result;
        }

        public void SetRadioPower(int routerIndex, int power, bool b5Ghz)
        {
            Log("Setting power on radio of router " + routerIndex + " to " + power);
            if (b5Ghz)
            {
                if (!AC.SetRadio5GHzPower(routers[routerIndex], power))
                {
                    Log("SetRadio5GHzPower returned false Ignoring");
                }
            }
            else
            {
                if (!AC.SetRadio24GHzPower(routers[routerIndex], power))
                {
                    Log("SetRadio24GHzPower returned false Ignoring");
                }

            }

        }

        public bool ConnectToEchoServer()
        {            
            CloseEchoClient();
            Log("SetupRoutersAndProfilesIfNeeded: Restart echo services to free stale connections");
            AC.StartEchoServer(EchoPort, EchoByteStream, 5);
            Log("ConnectToEchoServer - Extracting Local IP Address of Interface");
            string localIPAddress = null;
            try
            {
                localIPAddress = Helpers.GetIPAddressOfTestAdapter(TestInterface.Id);
            }
            catch (Exception e)
            {                
                LogError("Failed to extract the IPV4 address associated with the test adapter.  " + e.Message);
                return false;
            }
            Log("ConnectToEchoServer - Creating an echo client");
            echoClient = new TCPEchoClient(localIPAddress, RoamingConfiguration.ServiceAPChannelAddress, EchoPort, EchoByteStream);

            Log("ConnectToEchoServer - Connecting  echo client to EchoServer");
            try
            {
                echoClient.Connect();
            }
            catch (Exception e)
            {
                LogError("Failed to connect to the tcp server:" + RoamingConfiguration.ServiceAPChannelAddress + " - Port: " + EchoPort + " " + e.Message);
                return false;
            }

            return true;

        }

        public void CloseEchoClient()
        {
            if (echoClient != null)
            {
                echoClient.Close();
                echoClient = null;
            }

        }

        public void PerformEcho()
        {
            
            Log("PerformEcho - Sending Echo message");
            try
            {
                echoClient.PerformEcho();

                // Validate the data (if we didnt fail in the send)
                if (!echoClient.IsResponseBufferValid())
                {
                    LogError("TestConnection: Echod Data did not match what was send");
                }
            }
            catch (Exception e)
            {
                LogError("Exchanging data over the established connection FAILED with the tcp server on AP Controller. Please check logs " + e.Message);
                //something happened kill the echo client and server and restart the server
                CloseEchoClient();
                ConnectToEchoServer();
            }
        }

        public void SetupRoutersAndProfilesIfNeeded()
        {

            if (!AccessPointSetup)
            {
                Log("SetupRoutersAndProfilesIfNeeded: Deleting all wlan profiles");
                DeleteProfiles();
                Log("SetupRoutersAndProfilesIfNeeded: setting up access points");
                SetupAccessPoints();
                Log("SetupRoutersAndProfilesIfNeeded: Creating profiles");
                SetupProfiles();
                Log("SetupRoutersAndProfilesIfNeeded: Setting up Radius");
                SetupRadius();
                Log("SetupRoutersAndProfilesIfNeeded: Starting echo services");
                AC.StartEchoServer(EchoPort, EchoByteStream, 5);
                AccessPointSetup = true;
            }
            else
            {
                Log("Routers and Profiles are already setup.");
            }

        }

        private void SetupRadius()
        {
            if (APConfigs[1].Authentication_24GHZ == AuthenticationMethod.WPA2Enterprise
                || (DeviceSupports5Ghz && APConfigs[1].Authentication_5GHZ == AuthenticationMethod.WPA2Enterprise))
            {
                Log("Setting up Radius Server");
                Helpers.SetupRadius(AC, phase1Auth, phase2Auth, EnterpriseDomain, EnterpriseUser, EnterprisePassword, EnterpriseRadiusSecret, EnterpriseRadiusPort);
            }
            else
            {
                Log("Radius setup not needed");
            }
        }

        private void DeleteProfiles()
        {
            if (Api == null)
            {
                Log("Initaliziation not complete .. skipping profile cleanup");
                return;
            }

            try
            {
                var profileInfoList = Api.GetProfileList(TestInterface.Id);

                Log("DeleteProfiles Found " + profileInfoList.Count + " profiles");

                foreach (WlanProfileInfo profileInfo in profileInfoList)
                {
                    Log("Deleting " + profileInfo);
                    Api.DeleteProfile(TestInterface.Id, profileInfo.ProfileName);
                }
            }
            catch (Exception e)
            {
                Log("DeleteProfiles ignoring exception " + e.ToString());
            }
        }
        private void SetupProfiles()
        {
            SetupProfiles(false);
            if (DeviceSupports5Ghz)
                SetupProfiles(true);

        }
        private void SetupProfiles(bool b5Ghz)
        {

            string Profile = null;
            string ssid = null;
            AuthenticationMethod authMethod;
            EncryptionMethod encryptionMethod;

            if (b5Ghz)
            {
                ssid = APConfigs[1].SSID_5GHZ;
                authMethod = APConfigs[1].Authentication_5GHZ;
                encryptionMethod = APConfigs[1].Encryption_5GHZ;
            }
            else
            {
                authMethod = APConfigs[1].Authentication_24GHZ;
                ssid = APConfigs[1].SSID_24GHZ;
                encryptionMethod = APConfigs[1].Encryption_24GHZ;
            }
            Log("Setting up Profile SSID " + ssid + " Auth = " + authMethod + " Cipher = " + encryptionMethod + " b5GHz = " + b5Ghz);

            if (authMethod == AuthenticationMethod.WPA2Enterprise)
            {
                Profile = Helpers.CreateEnterpriseProfile(ssid, ConnectionMode.Manual, encryptionMethod, phase1Auth, phase2Auth);
            }
            else if (b5Ghz)
            {
                Profile = APConfigs[1].CreateProfile5ghz(ConnectionMode.Manual, false);
            }
            else
            {
                Profile = APConfigs[1].CreateProfile24ghz(ConnectionMode.Manual, false);
            }

            if (null == Profile)
            {
                Log("CreateProfile24ghz: null Profile");
                throw new Exception("CreateProfile24ghz ; null Profile");
            }

            Helpers.DeleteProfile(
                Api,
                TestInterface.Id,
                ssid);

            Api.CreateProfile(
                TestInterface.Id,
                Profile, Wlan.ProfileScope.AllUser);

            if (authMethod == AuthenticationMethod.WPA2Enterprise)
            {
                Helpers.SetProfileEapXmlUserDataMSChapV2(phase1Auth, phase2Auth, TestInterface.Id, Api, ssid, EnterpriseUser, EnterprisePassword, EnterpriseDomain);
            }
        }

        public bool ConnectToAP(string profilename)
        {

            // Before starting the connect, perform a scan
            Log("Triggering a scan");
            try
            {
                Api.Scan(TestInterface.Id, false, new TimeSpan(0, 0, 10));
            }
            catch (Exception e)
            {
                // This error is OK
                Log("Failed to do a scan. Please refer to traces for details. " + e.Message);
            }

            Log("Connecting to " + profilename);
            try
            {
                Api.ProfileConnect(TestInterface.Id, profilename, new TimeSpan(0, 0, 30));
            }
            catch (Exception e)
            {
                LogError("Failed to do a profile connect to ssid " + profilename + " with a timeout of 30s. Please refer to traces for details. " + e.Message);
                return false;
            }
            try
            {
                Helpers.WaitForFullNetworkConnectivity(TestInterface.Id, DefaultTimeOuts.DefaultDHCPTimeout, RoamingConfiguration.ServiceAPChannelAddress, EchoPort, EchoByteStream);
            }
            catch (Exception e)
            {
                LogError("An Error was encountered when waiting for full L3 connectivity, after connecting to " + profilename + " with a timeout of " + DefaultTimeOuts.DefaultDHCPTimeout + "ms. Please refer to traces for details." + e.Message);
                return false;
            }

            return true;
        }

        public void ToggleAdapter()
        {
            //Helpers.DisableTestAdapter(TestInterface.Id);
            //Helpers.EnableTestAdapter(TestInterface.Id);
        }

        // Here we will switch based on the router
        private void SetupAccessPoints()
        {
            bool bSetupRadius = false;
            APConfigs = new List<APConfig>();

            APConfigs.Add(WlanAccessPointHelper.CreateWirelessAccessPoint(
                m_AP1Config24GHz.Auth, m_AP1Config24GHz.Cipher, m_AP1Config24GHz.Phy, "TestPassphrase2",
                m_AP1Config5GHz.Auth, m_AP1Config5GHz.Cipher, m_AP1Config5GHz.Phy, "TestPassphrase1"));

            APConfigs[0].RadioChannel24Ghz = m_AP1Config24GHz.Channel;
            APConfigs[0].RadioChannel5Ghz = m_AP1Config5GHz.Channel;

            // TODO: Make this nicer
            if (m_AP1Config24GHz.Auth == AuthenticationMethod.WPA2Enterprise)
            {
                phase1Auth = m_AP1Config24GHz.EapPhase1Type;
                phase2Auth = m_AP1Config24GHz.EapPhase2Type;
                bSetupRadius = true;
            }
            else if (m_AP1Config5GHz.Auth == AuthenticationMethod.WPA2Enterprise)
            {
                phase1Auth = m_AP1Config5GHz.EapPhase1Type;
                phase2Auth = m_AP1Config5GHz.EapPhase2Type;
                bSetupRadius = true;
            }
            else if (m_AP2Config24GHz.Auth == AuthenticationMethod.WPA2Enterprise)
            {
                phase1Auth = m_AP2Config24GHz.EapPhase1Type;
                phase2Auth = m_AP2Config24GHz.EapPhase2Type;
                bSetupRadius = true;
            }
            else if (m_AP2Config5GHz.Auth == AuthenticationMethod.WPA2Enterprise)
            {
                phase1Auth = m_AP2Config5GHz.EapPhase1Type;
                phase2Auth = m_AP2Config5GHz.EapPhase2Type;
                bSetupRadius = true;
            }

            if (bSetupRadius)
            {
                APConfigs[0].RadiusIPAddress_24GHZ = RoamingConfiguration.ServiceAPChannelAddress;
                APConfigs[0].RadiusIPAddress_5GHZ = RoamingConfiguration.ServiceAPChannelAddress;

                APConfigs[0].RadiusSecretKey_24GHZ = EnterpriseRadiusSecret;
                APConfigs[0].RadiusSecretKey_5GHZ = EnterpriseRadiusSecret;
            }

            // Both APs should have same SSID
            APConfigs.Add(WlanAccessPointHelper.DuplicateAPForRoaming(APConfigs[0]));

            // Update stuff for second AP
            APConfigs[1].BSSID_24GHZ = WlanAccessPointHelper.GenerateBSSID();
            APConfigs[1].BSSID_5GHZ = WlanAccessPointHelper.GenerateBSSID();

            APConfigs[1].RadioChannel24Ghz = m_AP2Config24GHz.Channel;
            APConfigs[1].RadioChannel5Ghz = m_AP2Config5GHz.Channel;

            APConfigs[0].DHCPServerEnabled = false;
            APConfigs[0].LocalIPAddress = Helpers.RouterAlternateLocalIPAddress;
            APConfigs[0].NATEnabled = false; // Need this with Static route configured on AP controller machine
            APConfigs[0].SmartCompare = false; //Since the ap's NEED the same SSID this must be false

            APConfigs[1].DHCPServerEnabled = true;
            APConfigs[1].DHCPGateway = Helpers.RouterAlternateLocalIPAddress;
            APConfigs[1].NATEnabled = false; // Need this with Static route configured on AP controller machine
            APConfigs[1].SmartCompare = false; //Since the ap's NEED the same SSID this must be false

            Guid[] routerList = new Guid[2];
            routerList[0] = routers[0];
            routerList[1] = routers[1];

            Collection<ConfiguredSSIDs> configuredSSIDs = new Collection<ConfiguredSSIDs>();

            LogMajor("Configuring Router0");
            LogMajor("Configuring Router1");

            APConfig[] apConfigArray = new APConfig[APConfigs.Count];
            for (int index = 0; index < APConfigs.Count; index++)
            {
                apConfigArray[index] = APConfigs[index];
            }

            AC.ApplyConfigurationParallel(routerList, apConfigArray, out configuredSSIDs);
            
            LogMajor("Configured Router0 SSID:" + APConfigs[0].SSID_24GHZ + "/" + APConfigs[0].SSID_5GHZ);
            LogMajor("Configured Router1 SSID:" + APConfigs[1].SSID_24GHZ + "/" + APConfigs[1].SSID_5GHZ);
            
            LogMajor("Bringing up the Lan ports on both the routers");
            AC.SetLanPortsState(routerList[0], false);
            AC.SetLanPortsState(routerList[1], false);
            LogMajor("Setting up static routes from AP server to router");
            AC.SetStaticRoute(routers[1]);

            Log("Reading the BSSIDs configured on router for validation when connected");
            for (int i = 0; i < 2; i++)
            {
                APConfigs[i].BSSID_24GHZ = AC.GetAccessPointBSSID(routers[i], false);
                LogMajor("BSSID(" + APConfigs[i].SSID_24GHZ + ")=" + APConfigs[i].BSSID_24GHZ);
                LogMajor("Authentication(" + APConfigs[i].SSID_24GHZ + ")=" + APConfigs[i].Authentication_24GHZ.ProfileValue);
                LogMajor("Encryption(" + APConfigs[i].SSID_24GHZ + ")=" + APConfigs[i].Encryption_24GHZ.ProfileValue);
                LogMajor("Phy(" + APConfigs[i].SSID_24GHZ + ")=" + APConfigs[i].PhyType_24GHZ.ProfileValue);
                LogMajor("Channel(" + APConfigs[i].SSID_24GHZ + ")=" + APConfigs[i].RadioChannel24Ghz);

                APConfigs[i].BSSID_5GHZ = AC.GetAccessPointBSSID(routers[i], true);
                LogMajor("BSSID(" + APConfigs[i].SSID_5GHZ + ")=" + APConfigs[i].BSSID_5GHZ);
                LogMajor("AuthenticationMethod(" + APConfigs[i].SSID_5GHZ + ")=" + APConfigs[i].Authentication_5GHZ.ProfileValue);
                LogMajor("Encryption(" + APConfigs[i].SSID_5GHZ + ")=" + APConfigs[i].Encryption_5GHZ.ProfileValue);
                LogMajor("Phy(" + APConfigs[i].SSID_5GHZ + ")=" + APConfigs[i].PhyType_5GHZ.ProfileValue);
                LogMajor("Channel(" + APConfigs[i].SSID_5GHZ + ")=" + APConfigs[i].RadioChannel5Ghz);
            }
        }

        public bool bValidateGHZRadioAssociation(int APIndex, string expectedBssid)
        {
            for (int i = 0; i < 4; i++)
            {
                string associatedWith = Helpers.GetBSSID(Api, TestInterface);
                if (associatedWith == expectedBssid)
                {
                    Log("bValidateGHZRadioAssociation Associated with Radio " + APIndex + " having bssid " + expectedBssid + " as expected");
                    return true;
                }
                Log("bValidateGHZRadioAssociation Not associated with Radio " + APIndex + " having bssid " + expectedBssid + ". Assoicated with" + associatedWith + ". Will Retry to account for propogation issues");
                Wlan.Sleep(500);
            }
            LogError("bValidateGHZRadioAssociation Giving up. Not associated with Radio " + APIndex + " having bssid " + expectedBssid);
            return false;
        }

        public bool ValidateRadioAssociation(int APIndex, bool b5Ghz)
        {
            string expectedBssid = null;
            if (b5Ghz)
                expectedBssid = APConfigs[APIndex].BSSID_5GHZ;
            else
                expectedBssid = APConfigs[APIndex].BSSID_24GHZ;

            return bValidateGHZRadioAssociation(APIndex, expectedBssid);          
        }

        public void SuddenLossOfSignal()
        {
            if (DeviceSupports5Ghz)
                SuddenLossOfSignal(true);
            SuddenLossOfSignal(false);
        }
        public void SuddenLossOfSignal(bool b5Ghz)
        {

            LogMajor("Starting - Scenario Sudden Drop of Signal");

            LogMajor("Resetting Wlan Adaptor");
            ToggleAdapter();

            SetupRoutersAndProfilesIfNeeded();

            string profilename = null;
            if (b5Ghz)
            {
                profilename = APConfigs[0].SSID_5GHZ;
            }
            else
            {
                profilename = APConfigs[0].SSID_24GHZ;
            }

            LogMajor("Testing Roaming with router " + profilename);

            LogMajor("Turning both routers on");
            if(!SetRadioState(0, true, b5Ghz))
            {                
                return;
            }
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }

            LogMajor("Calling WlanConnect using a Wlan Profile " + profilename);

            if(!ConnectToAP(profilename))
            {
                return;
            }

            LogMajor("Making a TCP connection to Echo Services");
            if (!ConnectToEchoServer())
            {
                return;
            }


            Log("Performing Echo Test");
            PerformEcho();

            LogMajor("Turning Router 1 on");
            if(!SetRadioState(1, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 0 off");
            if (!SetRadioState(0, false, b5Ghz))
            {
                return;
            }

            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));

            LogMajor("Ensuring echo Network session is alive and functional by performing echo");
            PerformEcho();

            LogMajor("Ensuring associated with Router 1");
            if(!ValidateRadioAssociation(1, b5Ghz))
            {
                return;
            }


            LogMajor("Turning on Router 0");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning off Router 1");
            if (!SetRadioState(1, false, b5Ghz))
            {
                return;
            }
            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
            LogMajor("Ensuring echo Network session is alive and functional by performing echo");
            PerformEcho();
            LogMajor("Ensuring associated with Router 0");
            if(!ValidateRadioAssociation(0, b5Ghz))
            {
                return;
            }

            LogMajor("Turning on Router 1");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning off Router 0");
            if (!SetRadioState(0, false, b5Ghz))
            {
                return;
            }

            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
            LogMajor("Ensuring echo Network session is alive and functional by performing Echo");

            PerformEcho();
            LogMajor("Ensuring associated with AP 1");
            if(!ValidateRadioAssociation(1, b5Ghz))
            {
                return;
            }


            LogMajor("Turning on Router 0");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning off Router 1");
            if (!SetRadioState(1, false, b5Ghz))
            {
                return;
            }
            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
            LogMajor("Ensuring echo Network session is alive and functional by performing Echo");
            PerformEcho();
            LogMajor("Ensuring associated with AP 0");
            if (!ValidateRadioAssociation(0, b5Ghz))
            {
                return;
            }

            LogMajor("Scenario - Sudden loss of signal ends");
        }

        public void SuddenDropOfSignal()
        {
            if (DeviceSupports5Ghz)
                SuddenDropOfSignal(true);
            SuddenDropOfSignal(false);
        }

        public void SuddenDropOfSignal(bool b5Ghz)
        {
            LogMajor("Starting - Scenario Sudden Drop of Signal");

            LogMajor("Resetting Wlan Adaptor");
            ToggleAdapter();

            SetupRoutersAndProfilesIfNeeded();

            string profilename = null;
            if (b5Ghz)
            {
                profilename = APConfigs[0].SSID_5GHZ;
            }
            else
            {
                profilename = APConfigs[0].SSID_24GHZ;
            }


            LogMajor("Testing Roaming with router " + profilename);

            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }

            LogMajor("Calling WlanConnect using a Wlan Profile " + profilename);
            if(!ConnectToAP(profilename))
            {
                return;
            }

            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 off");
            if (!SetRadioState(1, false, b5Ghz))
            {
                return;
            }
            LogMajor("Setting txPower on Router 0 to 84");
            SetRadioPower(0, 84, b5Ghz);
            LogMajor("Setting txPower on Router 1 to 84");
            SetRadioPower(1, 84, b5Ghz);

            LogMajor("Making a TCP connection to Echo Services");
            if (!ConnectToEchoServer())
            {
                return;
            }

            Log("Ensuring echo Network session is alive and functional");
            PerformEcho();


            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 0 off");
            if (!SetRadioState(0, false, b5Ghz))
            {
                return;
            }
            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));

            LogMajor("Ensuring echo Network session is alive and functional by performing echo");
            PerformEcho();
            LogMajor("Ensuring associated with AP 1");
            if (!ValidateRadioAssociation(1, b5Ghz))
            {
                return;
            }


            LogMajor("Turning Router 1 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }
            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
            LogMajor("Ensuring echo Network session is alive and functiona by performing echol");
            PerformEcho();

            LogMajor("Setting txPower on Router 0 to 84");
            SetRadioPower(0, 84, b5Ghz);
            LogMajor("Setting txPower on Router 1 to 4");
            SetRadioPower(1, 4, b5Ghz);
            LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
            LogMajor("Ensuring echo Network session is alive and functional by performing echo");
            PerformEcho();

            LogMajor("Scenario - Sudden drop of signal ends");
        }

        public void GradualDropOfSignal()
        {
            if (DeviceSupports5Ghz)
                GradualDropOfSignal(true);
            GradualDropOfSignal(false);
        }

        public void GradualDropOfSignal(bool b5Ghz)
        {
            LogMajor("Scenario - Gradual drop of signal starts");

            LogMajor("Resetting Wlan Adaptor");
            ToggleAdapter();

            SetupRoutersAndProfilesIfNeeded();

            string profilename = null;
            if (b5Ghz)
            {
                profilename = APConfigs[0].SSID_5GHZ;
            }
            else
            {
                profilename = APConfigs[0].SSID_24GHZ;
            }


            LogMajor("Testing Roaming with router " + profilename);

            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }

            LogMajor("Calling WlanConnect using a Wlan Profile " + profilename);
            if(!ConnectToAP(profilename))
            {
                return;
            }

            LogMajor("Making a TCP connection to Echo Services");
            if(!ConnectToEchoServer())
            {
                return;
            }

            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }


            for (int power0 = 84, power1 = 4; power0 >= 4 && power1 <= 84; )
            {

                if (power0 < 4)
                    power0 = 4;
                if (power1 > 84)
                    power1 = 84;

                LogMajor("Setting txPower on Router 0 to " + power0);
                SetRadioPower(0, power0, b5Ghz);
                LogMajor("Setting txPower on Router 1 to " + power1);
                SetRadioPower(1, power1, b5Ghz);

                Wlan.Sleep(500);

                LogMajor("Currently associated with " + Helpers.GetBSSID(Api, TestInterface));
                LogMajor("Ensuring echo Network session is alive and functional by performing Echo");
                PerformEcho();


                power0 -= 10;
                power1 += 10;
            }
            LogMajor("Scenario - Gradual drop of signal ends");
        }

        public void GradualLossOfSignal()
        {
            if (DeviceSupports5Ghz)
                GradualLossOfSignal(true);
            GradualLossOfSignal(false);
        }


        public void GradualLossOfSignal(bool b5Ghz)
        {
            LogMajor("Scenario - Gradual loss of signal starts");

            LogMajor("Resetting Wlan Adaptor");
            ToggleAdapter();

            SetupRoutersAndProfilesIfNeeded();

            string profilename = null;
            if (b5Ghz)
            {
                profilename = APConfigs[0].SSID_5GHZ;
            }
            else
            {
                profilename = APConfigs[0].SSID_24GHZ;
            }

            LogMajor("Testing Roaming with router " + profilename);

            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }

            LogMajor("Calling WlanConnect using a Wlan Profile " + profilename);
            if(!ConnectToAP(profilename))
            {
                return;
            }

            LogMajor("Making a TCP connection to Echo Services");
            if(!ConnectToEchoServer())
            {
                return;
            }


            LogMajor("Turning Router 0 on");
            if (!SetRadioState(0, true, b5Ghz))
            {
                return;
            }
            LogMajor("Turning Router 1 on");
            if (!SetRadioState(1, true, b5Ghz))
            {
                return;
            }


            for (int power0 = 84, power1 = 4; power0 >= 4 && power1 <= 84; )
            {

                if (power0 < 4)
                    power0 = 4;
                if (power1 > 84)
                    power1 = 84;

                if (power0 == 4)
                {
                    LogMajor("Turning Router 0 off");
                    if (!SetRadioState(0, false, b5Ghz))
                    {
                        return;
                    }
                }
                LogMajor("Setting txPower on Router 0 to " + power0);
                SetRadioPower(0, power0, b5Ghz);
                LogMajor("Setting txPower on Router 1 to " + power1);
                SetRadioPower(1, power1, b5Ghz);

                Wlan.Sleep(500);

                LogMajor("Currently Associated with " + Helpers.GetBSSID(Api, TestInterface));
                LogMajor("Ensuring echo Network session is alive and functional by performing echo");
                PerformEcho();


                power0 -= 10;
                power1 += 10;

                Wlan.Sleep(500);
            }
            LogMajor("Scenario - Sudden loss of signal ends");
        }

        public static void Roam(RunTimeConfiguration rc,
            APConfigParameter AP1Config24GHz, 
            APConfigParameter AP1Config5GHz, 
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            ROAM_TEST_OPTIONS RoamTestOptions)
        {
            WlanHckTestLogger traceProvider = new WlanHckTestLogger("RoamingDriver");

            using (UsingGroup g = new UsingGroup())
            {

                traceProvider.LogComment("Roaming Scenarios Begin");

                try
                {
                    WhckRoaming hckRoaming = new WhckRoaming(rc,
                        AP1Config24GHz,
                        AP1Config5GHz,
                        AP2Config24GHz,
                        AP2Config5GHz,
                        RoamTestOptions);

                    g.Add(hckRoaming);


                    if (RoamTestOptions.HasFlag(ROAM_TEST_OPTIONS.SuddenLossOfSignal))
                    {
                        string scenarioName = "Roaming " + ROAM_TEST_OPTIONS.SuddenLossOfSignal.ToString();
                        Helpers.LogVariationStart(scenarioName, scenarioName);

                        traceProvider.LogComment("Testing sudden loss of Signal");
                        hckRoaming.SuddenLossOfSignal();
                        Helpers.LogVariationStop(scenarioName);
                    }

                    if (RoamTestOptions.HasFlag(ROAM_TEST_OPTIONS.SuddenDropOfSignal))
                    {
                        string scenarioName = "Roaming " + ROAM_TEST_OPTIONS.SuddenDropOfSignal.ToString();
                        Helpers.LogVariationStart(scenarioName, scenarioName);

                        traceProvider.LogComment("Testing sudden drop of Signal");
                        hckRoaming.SuddenDropOfSignal();
                        Helpers.LogVariationStop(scenarioName);
                    }

                    if (RoamTestOptions.HasFlag(ROAM_TEST_OPTIONS.GradualDropOfSignal))
                    {
                        string scenarioName = "Roaming " + ROAM_TEST_OPTIONS.GradualDropOfSignal.ToString();
                        Helpers.LogVariationStart(scenarioName, scenarioName);

                        traceProvider.LogComment("Testing gradual drop of Signal");
                        hckRoaming.GradualDropOfSignal();
                        Helpers.LogVariationStop(scenarioName);
                    }

                    if (RoamTestOptions.HasFlag(ROAM_TEST_OPTIONS.GradualLossOfSignal))
                    {
                        string scenarioName = "Roaming " + ROAM_TEST_OPTIONS.GradualLossOfSignal.ToString();
                        Helpers.LogVariationStart(scenarioName, scenarioName);

                        traceProvider.LogComment("Testing gradual loss of Signal");
                        hckRoaming.GradualLossOfSignal();
                        Helpers.LogVariationStop(scenarioName);
                    }
                }
                catch (Exception e)
                {
                    traceProvider.LogError("Error - " + e.Message);
                    throw;
                }
            }
        }
    }
}
