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
using Microsoft.Test.Networking.Power;
using Helpers = Microsoft.Test.Networking.Kit.Helpers;


namespace HlkTest.AssociationTests
{
    [FlagsAttribute]
    public enum ASSOC_TEST_OPTIONS
    {
        None = 0,
        Sleep = 1,
        Hibernate = 2,
        All = 0xF
    }

    public enum ASSOCIATION_VARIATION
    {
        WPA2_PSK_PEAP_AES,
        WPA2_PSK_TTLS_AES,
        WPA2_PSK_PEAP_TKIP_AES,
        WPA2_PSK_TTLS_TKIP_AES,
        OPEN_WPA2_AES,
        OPEN_WEP
    }

    public class WhckAssociation : IDisposable
    {
        private ASSOC_TEST_OPTIONS m_AssocTestOptions;
        private RunTimeConfiguration m_AssociationConfiguration;
        
        private Wlan m_WlanApi;

        private ApConfigClient m_ApConfigClient ;

        private Collection<APConfig> m_APConfigs;

        private Collection<Guid> m_Routers;

        private PowerManager m_PowerManager;


        public const int c_EchoPort = 7777;
        public const int c_EchoByteStream = 1024;
        

        //private APConfig[] m_APConfigs;
        
        private bool m_bIsAccessPointSetup = false;

        private WlanHckTestLogger m_TraceProvider;

        private WlanInterface m_TestInterface;

        private bool m_bDisposed = false;

        public bool m_DeviceSupports5GHz {get; private set;}

        private string m_EnterpriseRadiusSecret="ThereExistsASecret";
        private Int32 m_EnterpriseRadiusPort = 1812;
        private string m_EnterpriseDomain = "corporation";
        private string m_EnterpriseUser = "johndoe";
        private string m_EnterprisePassword = "ihaveapassword";

        // Note we currently have the same auth set on both 24GHZ and 5GHZ
        private SUPPORTED_PHASE1_TYPE[] m_Phase1Auth = new SUPPORTED_PHASE1_TYPE[2];
        private SUPPORTED_PHASE2_TYPE[] m_Phase2Auth = new SUPPORTED_PHASE2_TYPE[2];

        private string m_LocalIP = null;

        private APConfigParameter m_AP1Config24GHz;
        private APConfigParameter m_AP1Config5GHz;
        private APConfigParameter m_AP2Config24GHz;
        private APConfigParameter m_AP2Config5GHz;

        //PowerManager m_PowerManager;
        private void Log(String log)
        {
            WEX.Logging.Interop.Log.Comment(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        public void LogMajor(String log)
        {
            WEX.Logging.Interop.Log.Comment(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        public void LogError(String log)
        {
            WEX.Logging.Interop.Log.Error(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ASSOC]{1}", DateTime.Now, log));
        }

        public WhckAssociation(RunTimeConfiguration RuntimeConfig,
            APConfigParameter AP1Config24GHz,
            APConfigParameter AP1Config5GHz,
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            ASSOC_TEST_OPTIONS AssocTestOptions)
        {
            if (RuntimeConfig == null)
            {
                throw new ArgumentNullException("RuntimeConfig");
            }
            m_TraceProvider = new WlanHckTestLogger("WhckAssociation");

            this.m_AssociationConfiguration = RuntimeConfig;
            this.m_AP1Config24GHz = AP1Config24GHz;
            this.m_AP1Config5GHz = AP1Config5GHz;
            this.m_AP2Config24GHz = AP2Config24GHz;
            this.m_AP2Config5GHz = AP2Config5GHz;
            this.m_AssocTestOptions = AssocTestOptions;

            m_WlanApi = new Wlan();

            m_TestInterface = Helpers.GetWlanInterface(m_WlanApi);

            m_ApConfigClient = Helpers.ConnectToService(m_AssociationConfiguration.ServiceBackChannelAddress);

            m_Routers = Helpers.GetRouterCollection(m_ApConfigClient);
            this.m_DeviceSupports5GHz = RuntimeConfig.DeviceSupports5Ghz;
            m_PowerManager = new PowerManager();
            m_bIsAccessPointSetup = false;

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
            if (!this.m_bDisposed)
            {

                if (disposing)
                {
                    if (null != m_WlanApi)
                    {
                        m_WlanApi.Dispose();
                        m_WlanApi = null;
                    }
                    if (null != m_ApConfigClient)
                    {
                        //m_ApConfigClient.StopRadiusServer();
                        m_ApConfigClient.StopEchoServer(); // Service checks if echo server was running or not
                        m_ApConfigClient.Dispose();
                        m_ApConfigClient = null;
                    }
                    // TODO Dispose here
                }
            }

            m_bDisposed = true;
        }

        ~WhckAssociation()
        {
            Dispose(false);
        }

        public void CreateEchoServer()
        {
            Log("Starting a new EchoServer accept thread on the service");
            m_ApConfigClient.StartEchoServer(c_EchoPort,c_EchoByteStream,5);
        }


        public bool PerformEcho(Int32 AssociatedRouter,bool b5Ghz)
        {
            try
            {
                Helpers.PerformEcho(m_TestInterface.Id, m_LocalIP, m_AssociationConfiguration.ServiceAPChannelAddress,c_EchoPort,c_EchoByteStream);
            }
            catch (Exception e)
            {
                LogError("An error was encountered while testing network connectivity to the AP Server. The interface is assumed to be associated with [" + GetSSID(AssociatedRouter, b5Ghz) + "] during the test.  Error:" + e.Message);
                return false;
                //Helpers.LogExceptionAndThrowNewException("An exception was encountered while testing network connectivity to the AP Server. The interface is assumed to be associated with "+GetSSID(AssociatedRouter,b5Ghz)+" during the test.",e);
            }
            return true;
        }

        public void SetRadioState(int routerIndex, bool bEnabled,bool b5Ghz)
        {
            //Log("Setting state on radio of router "+routerIndex+" to "+bEnabled + " 5 GHZ = "+b5Ghz);
            Log(String.Format(CultureInfo.InvariantCulture,"Setting state on radio of router: [{0}] to [{1}] 5GHZ = [{2}] ", routerIndex, bEnabled, b5Ghz));
            if (b5Ghz)
            {
                if (!m_ApConfigClient.SetRadio5GHzState(m_Routers[routerIndex], bEnabled))
                {
                    Log("SetRadio5GHzState returned false ignoring error");
                    //throw new Exception("SetRadio5GHzState returned false");
                }
            }
            else
            {
                if (!m_ApConfigClient.SetRadio24GHzState(m_Routers[routerIndex], bEnabled))
                {
                    Log("SetRadio24GHzState returned false ignoring error");
                    //throw new Exception("SetRadio24GHzState returned false");
                }
            }

            if (bEnabled == true)
            {
                string ssidtoscan = GetSSID( routerIndex, b5Ghz);
                string bssidtoscan = GetBSSID( routerIndex, b5Ghz);
                Log("SetRadioState waiting for radio to appear in scan list");
                Helpers.IssueScanWaitForSSIDToAppearInScanList( m_WlanApi,m_TestInterface,ssidtoscan,bssidtoscan,60,500);
            }
            
        }

        public void SetupRoutersIfNeeded()
        {

            if (!m_bIsAccessPointSetup)
            {
                SetupAccessPoints();
                m_bIsAccessPointSetup = true;
            }
            Log("Routers and Profiles are  setup.Turning All radios on");
            SetRadioState(0,true,false);
            SetRadioState(1,true,false);
            if(m_DeviceSupports5GHz)
            {
                SetRadioState(0,true,true);
                SetRadioState(1,true,true);
            }

        }

        private void StartRadius(int routerIndex, bool b5GHZ)
        {
            AuthenticationMethod authMode;
            if (b5GHZ)
            {
                authMode = m_APConfigs[routerIndex].Authentication_5GHZ;
            }
            else
            {
                authMode = m_APConfigs[routerIndex].Authentication_24GHZ;
            }
            if (authMode == AuthenticationMethod.WPA2Enterprise)
            {
                m_ApConfigClient.StopRadiusServer();
                Log("Setting up Radius Server");
                Helpers.SetupRadius( m_ApConfigClient, m_Phase1Auth[routerIndex], m_Phase2Auth[routerIndex], m_EnterpriseDomain, m_EnterpriseUser, m_EnterprisePassword, m_EnterpriseRadiusSecret, m_EnterpriseRadiusPort);                
            }
        }

        public bool ConnectToAP(string profilename)
        {

            Log("Connecting to "+profilename);
            try
            {
                m_WlanApi.ProfileConnect(m_TestInterface.Id, profilename,new TimeSpan(0, 0, 30));
            }
            catch (Exception e)
            {
                LogError("Failed to Connect to to ssid " + profilename + " with a timeout of 30s. Please refer to traces for details.  Error: " + e.Message);
                return false;
            }

            try
            {
                LogMajor("Waiting for full network connectivity");
                m_LocalIP = Helpers.WaitForFullNetworkConnectivity(m_TestInterface.Id,DefaultTimeOuts.DefaultDHCPTimeout, m_AssociationConfiguration.ServiceAPChannelAddress, c_EchoPort, c_EchoByteStream);
            }
            catch (Exception e)
            {
                LogError("An error was encountered when waiting for full L3 connectivity, after connecting to [" + profilename + "] with a timeout of " + DefaultTimeOuts.DefaultDHCPTimeout + "ms. Please refer to traces for details. Error: " + e.Message);
                return false;
            }

            return true;
        }

        private void DeleteProfiles()
        {

            if (m_WlanApi == null)
            {
                Log("Initaliziation not complete .. skipping profile cleanup");
                return;
            }

            try
            {
                var profileInfoList = m_WlanApi.GetProfileList(m_TestInterface.Id);

                //Log ("DeleteProfiles Found "+profileInfoList.Count+" profiles");
                Log(String.Format(CultureInfo.InvariantCulture, "DeleteProfiles Found {0} profiles", profileInfoList.Count));
                    
                foreach(WlanProfileInfo profileInfo in profileInfoList)
                {
                    //Log("Deleting "+profileInfo);
                    Log(String.Format(CultureInfo.InvariantCulture,"Deleting {0}", profileInfo));
                    m_WlanApi.DeleteProfile(m_TestInterface.Id,profileInfo.ProfileName);
                }
            }
            catch (Exception e)
            {
                Log("DeleteProfiles ignoring exception "+e.ToString());
            }

        }

        private string CreateProfile(int RouterIndex,ConnectionMode mode,bool b5GHZ)
        {

            string Profile = null;
            string ssid  = null;
            AuthenticationMethod authMethod;
            EncryptionMethod encryptionMethod;

            // Create  a Radius Server if needed first
            StartRadius(RouterIndex, b5GHZ);
            
            if (b5GHZ)
            {
                ssid = m_APConfigs[RouterIndex].SSID_5GHZ;
                authMethod = m_APConfigs[RouterIndex].Authentication_5GHZ;
                encryptionMethod = m_APConfigs[RouterIndex].Encryption_5GHZ;
            }
            else
            {
                authMethod = m_APConfigs[RouterIndex].Authentication_24GHZ;
                ssid = m_APConfigs[RouterIndex].SSID_24GHZ;
                encryptionMethod = m_APConfigs[RouterIndex].Encryption_24GHZ;
            }

            if (authMethod == AuthenticationMethod.WPA2Enterprise)
            {
                bool FIPSMode = false;

                // The FIPS Mode parameter will only be honored  for AES + WPA2 profiles
                if (encryptionMethod == EncryptionMethod.AES)
                    FIPSMode = m_AssociationConfiguration.FIPSMode;
                Profile = Helpers.CreateEnterpriseProfile(ssid, mode, encryptionMethod, m_Phase1Auth[RouterIndex], m_Phase2Auth[RouterIndex], FIPSMode);
            }
            else if (b5GHZ)
            {
                Profile = m_APConfigs[RouterIndex].CreateProfile5ghz(mode, false);
            }
            else
            {
                Profile = m_APConfigs[RouterIndex].CreateProfile24ghz(mode, false);
            }
            if (null == Profile)
            {
                Log("CreateProfile24ghz: null Profile");
                throw new Exception("CreateProfile24ghz ; null Profile");
            }
            
            Helpers.DeleteProfile(
                m_WlanApi, 
                m_TestInterface.Id, 
                ssid);

            Log(String.Format(CultureInfo.InvariantCulture, "Create profile for ssid: {0}", ssid));

            m_WlanApi.CreateProfile(
                m_TestInterface.Id, 
                Profile, Wlan.ProfileScope.AllUser);

            if (authMethod == AuthenticationMethod.WPA2Enterprise)
            {
                Log(String.Format(CultureInfo.InvariantCulture, "Adding EAP Data for MSCHAPV2 for ssid: {0}", ssid));
                Helpers.SetProfileEapXmlUserDataMSChapV2(m_Phase1Auth[RouterIndex],m_Phase2Auth[RouterIndex],m_TestInterface.Id,m_WlanApi,ssid,m_EnterpriseUser,m_EnterprisePassword,m_EnterpriseDomain);

                if (mode == ConnectionMode.Auto) //must issue a connect after updating the eap data in the profile after an auto connect
                {
                    m_WlanApi.ProfileConnect(m_TestInterface.Id, ssid);
                }
            }
            return Profile;
        }


        private void SetupAccessPoints()
        {
            Log(String.Format(CultureInfo.InvariantCulture, "ASSOCIATION_VARIATION: Setup {0}", m_AssocTestOptions.ToString()));

            m_APConfigs = new Collection<APConfig>();

            // FIPS mode only phys
            if (m_AssociationConfiguration.FIPSMode)
            {
                if (m_AssociationConfiguration.TestNFIPS)
                {
                    //If TestNFIPS is set then use n phy regardless of what it was before
                    m_AP1Config24GHz.Phy = PhyType.n;
                    m_AP1Config5GHz.Phy = PhyType.n;

                    m_AP2Config24GHz.Phy = PhyType.n;
                    m_AP2Config24GHz.Phy = PhyType.n;
                }
                else
                {
                    //If TestNFIPS not set then need to downgrade any n phy.
                    if (m_AP1Config24GHz.Phy == PhyType.n)
                    {
                        m_AP1Config24GHz.Phy = PhyType.g;
                    }
                    if (m_AP1Config5GHz.Phy == PhyType.n)
                    {
                        m_AP1Config5GHz.Phy = PhyType.a;
                    }
                    if (m_AP2Config24GHz.Phy == PhyType.n)
                    {
                        m_AP2Config24GHz.Phy = PhyType.g;
                    }
                    if (m_AP2Config5GHz.Phy == PhyType.n)
                    {
                        m_AP2Config5GHz.Phy = PhyType.a;
                    }
                }
            }


            m_APConfigs.Add(WlanAccessPointHelper.CreateWirelessAccessPoint(
                m_AP1Config24GHz.Auth, m_AP1Config24GHz.Cipher, m_AP1Config24GHz.Phy, "1234567890",
                m_AP1Config5GHz.Auth, m_AP1Config5GHz.Cipher, m_AP1Config5GHz.Phy, "1234567890"));

            m_APConfigs[0].RadioChannel24Ghz = m_AP1Config24GHz.Channel;
            m_APConfigs[0].RadioChannel5Ghz = m_AP1Config5GHz.Channel;

            m_APConfigs.Add(WlanAccessPointHelper.CreateWirelessAccessPoint(
                m_AP2Config24GHz.Auth, m_AP2Config24GHz.Cipher, m_AP2Config24GHz.Phy, "1234567890",
                m_AP2Config5GHz.Auth, m_AP2Config5GHz.Cipher, m_AP2Config5GHz.Phy, "1234567890"));

            m_APConfigs[1].RadioChannel24Ghz = m_AP2Config24GHz.Channel;
            m_APConfigs[1].RadioChannel5Ghz = m_AP2Config5GHz.Channel;

            //set hidden.
            m_APConfigs[0].BeaconEnabled_24GHZ = !m_AP1Config24GHz.IsHidden;
            m_APConfigs[0].BeaconEnabled_5GHZ = !m_AP1Config5GHz.IsHidden;
            m_APConfigs[1].BeaconEnabled_24GHZ = !m_AP2Config24GHz.IsHidden;
            m_APConfigs[1].BeaconEnabled_5GHZ = !m_AP2Config5GHz.IsHidden;

            // We only use Enterprise on second router. Setting both
            m_Phase1Auth[0] = m_AP2Config24GHz.EapPhase1Type;
            m_Phase2Auth[0] = m_AP2Config24GHz.EapPhase2Type;

            m_Phase1Auth[1] = m_AP2Config24GHz.EapPhase1Type;
            m_Phase2Auth[1] = m_AP2Config24GHz.EapPhase2Type;

            m_APConfigs[0].RadiusIPAddress_24GHZ = m_AssociationConfiguration.ServiceAPChannelAddress;
            m_APConfigs[1].RadiusIPAddress_24GHZ = m_AssociationConfiguration.ServiceAPChannelAddress;
            m_APConfigs[0].RadiusIPAddress_5GHZ = m_AssociationConfiguration.ServiceAPChannelAddress;
            m_APConfigs[1].RadiusIPAddress_5GHZ = m_AssociationConfiguration.ServiceAPChannelAddress;
            
            m_APConfigs[0].RadiusSecretKey_24GHZ = m_EnterpriseRadiusSecret;
            m_APConfigs[1].RadiusSecretKey_24GHZ = m_EnterpriseRadiusSecret;            
            m_APConfigs[0].RadiusSecretKey_5GHZ = m_EnterpriseRadiusSecret;
            m_APConfigs[1].RadiusSecretKey_5GHZ = m_EnterpriseRadiusSecret;            
           

            Guid[] routerList = new Guid[2];
            routerList[0] = m_Routers[0];
            routerList[1] = m_Routers[1];

            Collection<ConfiguredSSIDs> configuredSSIDs = new Collection<ConfiguredSSIDs>();

            LogMajor("Configuring Router0");
            LogMajor("Configuring Router1");

            APConfig[] apConfigArray = new APConfig[m_APConfigs.Count];
            for (int index = 0; index < m_APConfigs.Count; index++)
            {
                apConfigArray[index] = m_APConfigs[index];
            }

            m_ApConfigClient.ApplyConfigurationParallel(routerList, apConfigArray, out configuredSSIDs);

            m_APConfigs[0].SSID_24GHZ = configuredSSIDs[0].TwoPoint4GhzSSID;
            m_APConfigs[0].SSID_5GHZ = configuredSSIDs[0].FiveghzSSID;
            m_APConfigs[1].SSID_24GHZ = configuredSSIDs[1].TwoPoint4GhzSSID;
            m_APConfigs[1].SSID_5GHZ = configuredSSIDs[1].FiveghzSSID;

            LogMajor("Configured Router0 SSID:" + m_APConfigs[0].SSID_24GHZ + "/" + m_APConfigs[0].SSID_5GHZ);
            LogMajor("Configured Router1 SSID:" + m_APConfigs[1].SSID_24GHZ + "/" + m_APConfigs[1].SSID_5GHZ);

            LogMajor("Taking the LAN ports down on both routers");
            m_ApConfigClient.SetLanPortsState(routerList[0], true);
            m_ApConfigClient.SetLanPortsState(routerList[1], true);

            Log("Reading the BSSIDs configured on router for validation when connected");
            for (int i = 0;i < 2 ;i++)
            {
                m_APConfigs[i].BSSID_24GHZ = m_ApConfigClient.GetAccessPointBSSID(m_Routers[i],false);
                LogMajor("BSSID("+m_APConfigs[i].SSID_24GHZ+")="+m_APConfigs[i].BSSID_24GHZ);
                LogMajor("Authentication("+m_APConfigs[i].SSID_24GHZ+")="+m_APConfigs[i].Authentication_24GHZ.ProfileValue);
                LogMajor("Encryption("+m_APConfigs[i].SSID_24GHZ+")="+m_APConfigs[i].Encryption_24GHZ.ProfileValue);
                LogMajor("Phy("+m_APConfigs[i].SSID_24GHZ+")="+m_APConfigs[i].PhyType_24GHZ.ProfileValue);
                LogMajor("Channel("+m_APConfigs[i].SSID_24GHZ+")="+m_APConfigs[i].RadioChannel24Ghz);

                m_APConfigs[i].BSSID_5GHZ= m_ApConfigClient.GetAccessPointBSSID(m_Routers[i],true);
                LogMajor("BSSID("+m_APConfigs[i].SSID_5GHZ+")="+m_APConfigs[i].BSSID_5GHZ);
                LogMajor("AuthenticationMethod("+m_APConfigs[i].SSID_5GHZ+")="+m_APConfigs[i].Authentication_5GHZ.ProfileValue);
                LogMajor("Encryption("+m_APConfigs[i].SSID_5GHZ+")="+m_APConfigs[i].Encryption_5GHZ.ProfileValue);
                LogMajor("Phy("+m_APConfigs[i].SSID_5GHZ+")="+m_APConfigs[i].PhyType_5GHZ.ProfileValue);
                LogMajor("Channel("+m_APConfigs[i].SSID_5GHZ+")="+m_APConfigs[i].RadioChannel5Ghz);
            }

        }

        public bool bValidateGHZRadioAssociation(int APIndex,string expectedBssid)
        {
            string associatedWith = Helpers.GetBSSID(m_WlanApi,m_TestInterface);

            if (associatedWith == expectedBssid)
            {
                Log("Associated with Router "+APIndex+" having bssid "+expectedBssid+" as expected");
                return true;
            }

            LogError("Not associated with Router "+APIndex+ " having bssid "+ expectedBssid+". Assoicated with "+associatedWith);
            return false;
        }

        public bool ValidateRadioAssociation(int APIndex,bool b5Ghz)
        {
            
            string expectedBssid = null;
            if (b5Ghz)
            {
                expectedBssid = m_APConfigs[APIndex].BSSID_5GHZ;
            }
            else
            {
                expectedBssid = m_APConfigs[APIndex].BSSID_24GHZ;
            }

            return bValidateGHZRadioAssociation(APIndex,expectedBssid);
        }


        private string GetSSID(int routerIndex,bool b5Ghz)
        {
            return (b5Ghz) ? m_APConfigs[routerIndex].SSID_5GHZ : m_APConfigs[routerIndex].SSID_24GHZ;
        }

        private string GetBSSID(int routerIndex,bool b5Ghz)
        {
            return (b5Ghz) ? m_APConfigs[routerIndex].BSSID_5GHZ : m_APConfigs[routerIndex].BSSID_24GHZ;
        }

        private void AssociateSameAP(int routerIndex, bool b5Ghz, bool bHibernate)
        {
            Log("Deleting all wlan profiles");
            DeleteProfiles();

            Log("Starting Echo Services");
            CreateEchoServer();

            //Radios of all APs are turned on by the SetupRoutersIfNeeded();
            Log("Setting up Routers");
            SetupRoutersIfNeeded();
            
            string Profile;
            APConfig apConfig = m_APConfigs[routerIndex];
            string ap_ssid = (b5Ghz) ? apConfig.SSID_5GHZ : apConfig.SSID_24GHZ;

            //Turn off all APs
            SetRadioState(0, false, false);
            SetRadioState(0, false, true);
            SetRadioState(1, false, false);
            SetRadioState(1, false, true);
            //Turn on AP used for this run
            SetRadioState(routerIndex, true, b5Ghz);

            LogMajor("Associate with manual profile to " + ap_ssid + " - Enter lower powerstate - Wake - Verify connectivity to same AP");

            Log("Creating a manual profile for SSID " + ap_ssid);
            Profile = CreateProfile(routerIndex, ConnectionMode.Manual, b5Ghz);

            LogMajor("Connecting to " + ap_ssid);
            if (!ConnectToAP(ap_ssid))
            {
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(routerIndex, b5Ghz))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(routerIndex, b5Ghz))
            {
                return;
            }

            m_ApConfigClient.Disconnect(); //must disconnect from ap controller before going to sleep
            if (bHibernate)
            {
                LogMajor("Entering hibernate");
                m_PowerManager.Sleep(120 * 1000, SleepState.S4);

            } else
            {
                LogMajor("Entering deepest supported sleep");
                m_PowerManager.DeepestSleep(120 * 1000);
            }
            LogMajor("Sleeping for 30 seconds after wake up");
            Wlan.Sleep(30 * 1000); // 30s
            m_ApConfigClient.Connect(); //must reconnect to ap controller after waking up

            if (!Helpers.IsConnected(m_WlanApi, m_TestInterface))
            {
                LogError("After resuming system not connected to " + ap_ssid);
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(routerIndex, b5Ghz))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(routerIndex, b5Ghz))
            {
                return;
            }

            m_WlanApi.DeleteAllProfiles(m_TestInterface.Id);

        }

        private void AssociateDifferentAP(int firstRouterIndex, int secondRouterIndex, bool b5GhzFirstRouter, bool b5GhzSecondRouter, bool bHibernate)
        {
            Log("Deleting all wlan profiles");
            DeleteProfiles();

            Log("Starting Echo Services");
            CreateEchoServer();

            //Radios of all APs are turned on by the SetupRoutersIfNeeded();
            Log("Setting up Routers");
            SetupRoutersIfNeeded();

            string Profile;
            APConfig ap1Config = m_APConfigs[firstRouterIndex];
            APConfig ap2Config = m_APConfigs[secondRouterIndex];
            string ap1_ssid = (b5GhzFirstRouter) ? ap1Config.SSID_5GHZ : ap1Config.SSID_24GHZ;
            string ap2_ssid = (b5GhzSecondRouter) ? ap2Config.SSID_5GHZ : ap2Config.SSID_24GHZ;
            string ap2_bssid = (b5GhzSecondRouter) ? ap2Config.BSSID_5GHZ : ap2Config.BSSID_24GHZ;

            //Turn off all APs
            SetRadioState(0, false, false);
            SetRadioState(0, false, true);
            SetRadioState(1, false, false);
            SetRadioState(1, false, true);
            //Turn on AP used for this run
            SetRadioState(firstRouterIndex, true, b5GhzFirstRouter);

            LogMajor("Associate with manual profile to " + ap1_ssid + " - Enter lower powerstate turn off first AP and turn on second AP - Wake - Connect with manual profile to " + ap2_ssid);

            Log("Creating a manual profile for SSID " + ap1_ssid);
            Profile = CreateProfile(firstRouterIndex, ConnectionMode.Manual, b5GhzFirstRouter);

            LogMajor("Connecting to " + ap1_ssid);
            if (!ConnectToAP(ap1_ssid))
            {
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(firstRouterIndex, b5GhzFirstRouter))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(firstRouterIndex, b5GhzFirstRouter))
            {
                return;
            }

            int delayInSeconds = 30;
            int ScenarioToRun = 1;

            //switch
            LogMajor("Putting machine into lower power state for 2 minutes. " + ap1_ssid + " will turn off and " + ap2_ssid + "will turn on");
            m_ApConfigClient.StartApScenarioAssoc(m_Routers[secondRouterIndex], m_Routers[firstRouterIndex], ScenarioToRun, delayInSeconds, b5GhzSecondRouter, b5GhzFirstRouter);

            m_ApConfigClient.Disconnect(); //must disconnect from ap controller before going to sleep
            if (bHibernate)
            {
                LogMajor("Entering hibernate");
                m_PowerManager.Sleep(120 * 1000, SleepState.S4);

            }
            else
            {
                LogMajor("Entering deepest supported sleep");
                m_PowerManager.DeepestSleep(120 * 1000);
            }
            LogMajor("Sleeping for 30 seconds after wake up");
            Wlan.Sleep(30 * 1000); // 30s
            m_ApConfigClient.Connect(); //must reconnect to ap controller after waking up

            //scan for new AP
            Helpers.IssueScanWaitForSSIDToAppearInScanList(m_WlanApi, m_TestInterface, ap2_ssid, ap2_bssid, 60, 500);

            //connect to next AP.
            Log("Creating a manual profile for SSID " + ap2_ssid);
            Profile = CreateProfile(secondRouterIndex, ConnectionMode.Manual, b5GhzSecondRouter);

            LogMajor("Connecting to " + ap2_ssid);
            if (!ConnectToAP(ap2_ssid))
            {
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(secondRouterIndex, b5GhzSecondRouter))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(secondRouterIndex, b5GhzSecondRouter))
            {
                return;
            }

            m_WlanApi.DeleteAllProfiles(m_TestInterface.Id);
        }

        //does sleep association tests and hibernate association tests.
        public void AdvancedAssociationTests()
        {
            if (m_AssocTestOptions.HasFlag(ASSOC_TEST_OPTIONS.Sleep))
            {
                if (!(PowerManager.IsS3Supported() || PowerManager.IsAoAcCapableMachine()))
                {
                    LogError("S3 or AOAC is not supported.  At least one must be supported.  Run powercfg -a from an elevated cmd prompt to determine supported sleep states");
                }

                //test AP1 - sleep|CS - AP1 then AP1 - sleep|CS - AP2
                if (PowerManager.IsAoAcCapableMachine())
                {
                    //AP1 - CS - AP1
                    string sameAPName = "Association sleep and resume - Same AP - Connected Standby";
                    string sameAPDescription = "Sleep and resume with manual profile - Connected device goes to CS - Test connection to same AP";

                    Helpers.LogVariationStart(sameAPName, sameAPDescription);
                    AssociateSameAP(0, false, false);
                    Helpers.LogVariationStop(sameAPName);

                    //AP1 - CS - AP2
                    string differentAPName = "Association sleep and resume - Different AP - Connected Standby";
                    string differentAPDescription = "Sleep and resume with manual profile - Connected device goes to CS - Turn off AP radio - Turn on another AP radio - Test connection to new AP";

                    Helpers.LogVariationStart(differentAPName, differentAPDescription);
                    AssociateDifferentAP(0, 1, false,m_DeviceSupports5GHz, false);
                    Helpers.LogVariationStop(differentAPName);
                }
                else
                {
                    //AP1 - Sleep - AP1
                    string sameAPName = "Association sleep and resume - Same AP";
                    string sameAPDescription = "Sleep and resume with manual profile - Connected device goes to sleep - Test connection to same AP";

                    Helpers.LogVariationStart(sameAPName, sameAPDescription);
                    AssociateSameAP(0, false, false);
                    Helpers.LogVariationStop(sameAPName);

                    //AP1 - Sleep - AP2
                    string differentAPName = "Association sleep and resume - Different AP";
                    string differentAPDescription = "Sleep and resume with manual profile - Connected device goes to sleep - Turn off AP radio - Turn on another AP radio - Test connection to new AP";

                    Helpers.LogVariationStart(differentAPName, differentAPDescription);
                    AssociateDifferentAP(0, 1, false, m_DeviceSupports5GHz, false);
                    Helpers.LogVariationStop(differentAPName);
                }
            }

            if (m_AssocTestOptions.HasFlag(ASSOC_TEST_OPTIONS.Hibernate))
            {
                //test AP2 - hibernate - AP2 then AP2 - hibernate - AP1
                //AP2 - hiber - AP2
                if (PowerManager.IsS4Supported())
                {
                    string sameAPName = "Association hibernate and resume - Same AP";
                    string sameAPDescription = "Hibernate and resume with manual profile - Connected device hibernates - Test connection to same AP";

                    Helpers.LogVariationStart(sameAPName, sameAPDescription);
                    AssociateSameAP(1, m_DeviceSupports5GHz, true);
                    Helpers.LogVariationStop(sameAPName);

                    //AP2 - hiber - AP1
                    string differentAPName = "Association hibernate and resume - Different AP";
                    string differentAPDescription = "Hibernate and resume with manual profile - Connected device hibernates - Turn off AP radio - Turn on another AP radio - Test connection to new AP";

                    Helpers.LogVariationStart(differentAPName, differentAPDescription);
                    AssociateDifferentAP(1, 0, m_DeviceSupports5GHz, false, true);
                    Helpers.LogVariationStop(differentAPName);
                }
            }
        }

        public void BasicAssociationTest()
        {
            string variationName = "Basic Association test";
            string variationDescription = "Associate to first AP with manual profile - Turn Off Radio - Associate to second AP with manual profile";
            Helpers.LogVariationStart(variationName, variationDescription);

            Log("Deleting all wlan profiles");
            DeleteProfiles();

            Log("Starting Echo Services");
            CreateEchoServer();

            //Radios of all APs are turned on by the SetupRoutersIfNeeded();
            Log("Setting up Routers");
            SetupRoutersIfNeeded();

            string Profile;
            APConfig ap1Config = m_APConfigs[0];
            APConfig ap2Config = m_APConfigs[1];
            string ap1_ssid = ap1Config.SSID_24GHZ;
            string ap2_ssid = (m_DeviceSupports5GHz) ? ap2Config.SSID_5GHZ : ap2Config.SSID_24GHZ;

            //In general the typical usage pattern of radio states for APs should be that they are only turned on when needed
            // as this best matches the scenario where the user moves from location A to location B.

            //Turn off AP1 5 GHz (never used)
            SetRadioState(0,false,true);

            //Turn off AP2 (both 2.4 and 5, will be used later)
            SetRadioState(1, false, false);
            SetRadioState(1, false, true);

            LogMajor("Associate with manual profile to " + ap1_ssid + " - Turn AP off - Associate with manual profile to " + ap2_ssid);

            // Create a 2.4 GHz profile for first router.
            Log("Creating a manual profile for SSID " + ap1_ssid);
            Profile = CreateProfile(0, ConnectionMode.Manual, false);

            LogMajor("Connecting to " + ap1_ssid);
            if (!ConnectToAP(ap1_ssid))
            {
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(0,false))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(0, false))
            {
                return;
            }
            //Turn off AP1 2.4 GHz (not needed anymore)
            SetRadioState(0, false, false);

            m_WlanApi.DeleteProfile(m_TestInterface.Id, ap1_ssid);

            //AP2 next

            //Turn on AP2
            SetRadioState(1, true, m_DeviceSupports5GHz);

            // Create a profile for second router.
            Log("Creating a manual profile for SSID " + ap2_ssid);
            Profile = CreateProfile(1, ConnectionMode.Manual, m_DeviceSupports5GHz);

            LogMajor("Connecting to " + ap2_ssid);
            if (!ConnectToAP(ap2_ssid))
            {
                return;
            }

            LogMajor("Validating association bssid");
            if (!ValidateRadioAssociation(1, m_DeviceSupports5GHz))
            {
                return;
            }
            LogMajor("Validating connection by connecting to echo services and performing echo");
            if (!PerformEcho(1, m_DeviceSupports5GHz))
            {
                return;
            }

            m_WlanApi.DeleteProfile(m_TestInterface.Id, ap2_ssid);

            Helpers.LogVariationStop(variationName);
        }


        public static void Associate (RunTimeConfiguration rc,
            APConfigParameter AP1Config24GHz,
            APConfigParameter AP1Config5GHz,
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            ASSOC_TEST_OPTIONS AssocTestOptions)
        {
            WlanHckTestLogger traceProvider = new WlanHckTestLogger("Associate Driver");

            using (UsingGroup g = new UsingGroup())
            {
                try
                {
                    traceProvider.LogComment("Association Scenarios Begin");

                    WhckAssociation hckAssociation = new WhckAssociation(rc,
                        AP1Config24GHz,
                        AP1Config5GHz,
                        AP2Config24GHz,
                        AP2Config5GHz,
                        AssocTestOptions);

                    g.Add(hckAssociation);

                    //Basic Association AP1 2.4GHz -> AP2 5 GHz (2.4 GHz if 5 not supported), no sleep
                    hckAssociation.BasicAssociationTest();

                    //If requested perform advanced association tests
                    if (AssocTestOptions.HasFlag(ASSOC_TEST_OPTIONS.Sleep) || AssocTestOptions.HasFlag(ASSOC_TEST_OPTIONS.Hibernate))
                    {
                        hckAssociation.AdvancedAssociationTests();
                    }
                }
                catch (Exception e)
                {
                    traceProvider.LogError("Error - " + e.Message);
                    throw;
                }
                finally
                {
                    traceProvider.Flush();
                }
            }
        }
    }
}
