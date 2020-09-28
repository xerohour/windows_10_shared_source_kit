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
using Microsoft.Test.Networking.DeviceManager;
using Microsoft.Test.Networking.Tracing;
using Microsoft.Test.Networking.Power;
using Microsoft.Test.Networking.Oids;


namespace HlkTest.ScanningTests
{
   
    [FlagsAttribute]
    public enum SCAN_TEST_OPTIONS
    {
        None = 0,
        DuplicateSSIDs = 1
    }

    public class WhckScanning : IDisposable 
    {
        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("ScanTests");

        private RunTimeConfiguration ScanningConfiguration;
        private SCAN_TEST_OPTIONS m_ScanTestOptions;
        
        private Wlan Api ;

        private Collection<Guid> routers;

        private ApConfigClient AC ;

        private List<APConfig> APConfigs;
        
        private bool AccessPointSetup = false;
        
        private TraceProvider traceProvider;

        private WlanInterface TestInterface;

        private bool disposed = false;
        
        private APConfigParameter m_AP1Config24GHz;
        private APConfigParameter m_AP1Config5GHz;
        private APConfigParameter m_AP2Config24GHz;
        private APConfigParameter m_AP2Config5GHz;

        private  void Log(String log)
        {
            testLogger.LogComment(log);
        }

        public void LogMajor(String log)
        {
            //testLogger.LogComment(log);
            Log(log);
        }

        
        public WhckScanning(RunTimeConfiguration rc,
            APConfigParameter AP1Config24GHz,
            APConfigParameter AP1Config5GHz,
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            SCAN_TEST_OPTIONS ScanTestOptions)
        {
            if (rc == null)
            {
                throw new ArgumentNullException("rc");
            }
            traceProvider = new TraceProvider("WhckScanning");
            this.ScanningConfiguration = rc;
            this.m_AP1Config24GHz = AP1Config24GHz;
            this.m_AP1Config5GHz = AP1Config5GHz;
            this.m_AP2Config24GHz = AP2Config24GHz;
            this.m_AP2Config5GHz = AP2Config5GHz;
            this.m_ScanTestOptions = ScanTestOptions;

            Api = new Wlan();
            

            TestInterface = Helpers.GetWlanInterface(Api);
            AC = null;
            AC = Helpers.ConnectToService(ScanningConfiguration.ServiceBackChannelAddress);

                
            if (AC == null)
            {
                Log("Helpers.ConnectToService failed");
                AC = null;
                throw new Exception("Helpers.ConnectToService failed");
            }

            routers = Helpers.GetRouterCollection(AC);
            
            AccessPointSetup = false;
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
                    if (null != Api)
                    {
                        Api.Dispose();
                        Api = null;
                    }
                    if (null != AC)
                    {
                        //AC.StopEchoServer(); // Service checks if echo server was running or not
                        AC.Dispose();
                        AC = null;
                    }
                    // TODO Dispose here
                }
            }

            disposed = true;
        }        

        ~WhckScanning()
        {
            Dispose(false);
        }

        public void SetupRouters()
        {
            if (AccessPointSetup)
            {
                Log("Routers and Profiles are allready setup");
                return;
            }
            SetupAccessPoints();
            //Log("Sleeping to let APs respond");
            //Wlan.Sleep(5*60*1000);
            //Log("Awake");
            AccessPointSetup = true;
        }


        // Here we will switch based on the router
        private void SetupAccessPoints()
        {
            bool bSameSSID = false;
            APConfigs = new List<APConfig>();

            APConfigs.Add( WlanAccessPointHelper.CreateWirelessAccessPoint(  
                m_AP1Config24GHz.Auth, m_AP1Config24GHz.Cipher, m_AP1Config24GHz.Phy, "TestPassphrase2",
                m_AP1Config5GHz.Auth, m_AP1Config5GHz.Cipher, m_AP1Config5GHz.Phy, "TestPassphrase1"));

            if (m_ScanTestOptions.HasFlag(SCAN_TEST_OPTIONS.DuplicateSSIDs))
            {
                // Both APs should have same SSID
                APConfigs.Add(WlanAccessPointHelper.DuplicateAPForRoaming(APConfigs[0]));
                bSameSSID = true;
            }
            else
            {
                APConfigs.Add( WlanAccessPointHelper.CreateWirelessAccessPoint(  
                    m_AP2Config24GHz.Auth, m_AP2Config24GHz.Cipher, m_AP2Config24GHz.Phy, "TestPassphrase2",
                    m_AP2Config5GHz.Auth, m_AP2Config5GHz.Cipher, m_AP2Config5GHz.Phy, "TestPassphrase1"));
            }
            
            APConfigs[1].BSSID_24GHZ = WlanAccessPointHelper.GenerateBSSID();
            APConfigs[1].BSSID_5GHZ = WlanAccessPointHelper.GenerateBSSID();

            APConfigs[0].RadioChannel24Ghz = m_AP1Config24GHz.Channel;
            APConfigs[0].RadioChannel5Ghz= m_AP1Config5GHz.Channel;
                    
            APConfigs[1].RadioChannel24Ghz = m_AP2Config24GHz.Channel;
            APConfigs[1].RadioChannel5Ghz= m_AP2Config5GHz.Channel;

            APConfigs[0].BeaconEnabled_24GHZ = !m_AP1Config24GHz.IsHidden;
            APConfigs[0].BeaconEnabled_5GHZ = !m_AP1Config5GHz.IsHidden;
            APConfigs[1].BeaconEnabled_24GHZ = !m_AP2Config24GHz.IsHidden;
            APConfigs[1].BeaconEnabled_5GHZ = !m_AP2Config5GHz.IsHidden;

            // For the same SSID case, we want the APs to be configured with the specific SSID
            if (bSameSSID)
            {
                APConfigs[0].SmartCompare = false;
                APConfigs[1].SmartCompare = false;
            }

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

            if (!bSameSSID)
            {
                APConfigs[0].SSID_24GHZ = configuredSSIDs[0].TwoPoint4GhzSSID;
                APConfigs[0].SSID_5GHZ = configuredSSIDs[0].FiveghzSSID;
                APConfigs[1].SSID_24GHZ = configuredSSIDs[1].TwoPoint4GhzSSID;
                APConfigs[1].SSID_5GHZ = configuredSSIDs[1].FiveghzSSID;
            }

            LogMajor("Configured Router0 SSID:" + APConfigs[0].SSID_24GHZ + "/" + APConfigs[0].SSID_5GHZ);
            LogMajor("Configured Router1 SSID:" + APConfigs[1].SSID_24GHZ + "/" + APConfigs[1].SSID_5GHZ);


            Log("Reading the BSSIDs configured on router for validation when connected");
            for (int i = 0;i < 2 ;i++)
            {
                APConfigs[i].BSSID_24GHZ = AC.GetAccessPointBSSID(routers[i],false);
                LogMajor("BSSID("+APConfigs[i].SSID_24GHZ+") = " + APConfigs[i].BSSID_24GHZ);
                LogMajor("Authentication("+APConfigs[i].SSID_24GHZ+") = " + APConfigs[i].Authentication_24GHZ.ProfileValue);
                LogMajor("Encryption("+APConfigs[i].SSID_24GHZ+") = " + APConfigs[i].Encryption_24GHZ.ProfileValue);
                LogMajor("Phy("+APConfigs[i].SSID_24GHZ+") = " + APConfigs[i].PhyType_24GHZ.ProfileValue);
                LogMajor("Channel("+APConfigs[i].SSID_24GHZ+") = " + APConfigs[i].RadioChannel24Ghz);
                LogMajor("Hidden(" + APConfigs[i].SSID_24GHZ+") = " + ((APConfigs[i].BeaconEnabled_24GHZ == false) ? "Yes" : "No"));

                APConfigs[i].BSSID_5GHZ= AC.GetAccessPointBSSID(routers[i],true);
                LogMajor("BSSID("+APConfigs[i].SSID_5GHZ+")= " + APConfigs[i].BSSID_5GHZ);
                LogMajor("AuthenticationMethod("+APConfigs[i].SSID_5GHZ+") = " + APConfigs[i].Authentication_5GHZ.ProfileValue);
                LogMajor("Encryption("+APConfigs[i].SSID_5GHZ+") = " + APConfigs[i].Encryption_5GHZ.ProfileValue);
                LogMajor("Phy("+APConfigs[i].SSID_5GHZ+") = " + APConfigs[i].PhyType_5GHZ.ProfileValue);
                LogMajor("Channel("+APConfigs[i].SSID_5GHZ+") = " + APConfigs[i].RadioChannel5Ghz);
                LogMajor("Hidden(" + APConfigs[i].SSID_5GHZ+") = " + ((APConfigs[i].BeaconEnabled_5GHZ == false) ? "Yes" : "No"));
            }
        }

        private static void LogEtwEvent(EventArrivedEventArgs args)
        {
            object providerName, eventMessage;
            args.Properties.TryGetValue("ProviderName", out providerName);
            args.Properties.TryGetValue("EventMessage", out eventMessage);
            WEX.Logging.Interop.Log.Comment(string.Format(CultureInfo.InvariantCulture, "{0:MM/dd/yy HH:mm:ss.fff}[ETW]{1} - {2}", DateTime.Now, providerName, eventMessage));
        }
        
        public void Scan()
        {
            Log("Scan Test Configuration Parameters: ");
            Log(m_AP1Config24GHz.GetTestParameter());
            Log(m_AP1Config5GHz.GetTestParameter());
            Log(m_AP2Config24GHz.GetTestParameter());
            Log(m_AP2Config5GHz.GetTestParameter());

            SetupRouters();
            // Turn on all Radios
            Log("Turning on all radios");
            AC.SetRadio24GHzState(routers[0], true);
            AC.SetRadio24GHzState(routers[1], true);
            AC.SetRadio5GHzState(routers[0], true);
            AC.SetRadio5GHzState(routers[1], true);

            LogMajor("Turning on Airplane Mode");
            Api.SetRadioState(TestInterface, false);

            EventHandler<EventArrivedEventArgs> ETWCallback = null;
            ETWCallback = (object sender, EventArrivedEventArgs properties) =>
            {
                LogEtwEvent(properties);
            };

            var wlanEtwWatcher = new WlanEtwWatcher();
            wlanEtwWatcher.EventArrived += ETWCallback;
            Log("Start ETW Tracing");
            wlanEtwWatcher.Start();

            bool Radio_0_24Ghz_Found = false;
            bool Radio_1_24Ghz_Found = false;
            bool Radio_0_5Ghz_Found = false;
            bool Radio_1_5Ghz_Found = false;

            string Reason_Radio_0_24Ghz_Found = null;
            string Reason_Radio_1_24Ghz_Found = null;
            string Reason_Radio_0_5Ghz_Found = null;
            string Reason_Radio_1_5Ghz_Found = null;

            try
            {
                Log("Sleeping for a second");
                Wlan.Sleep(1 * 1000); // 1s

                LogMajor("Turning off Airplane Mode");
                Api.SetRadioState(TestInterface, true);

                for (int i = 0; i < 4; i++)
                {
                    Log("Scan Iteration " + i);

                    if (i == 0)
                    {
                        // First scan is always with a flush so that we dont get old data
                        LogMajor("Calling (public) scan with flush");
                        Api.Scan(TestInterface.Id, true, new TimeSpan(0, 0, 10));
                    }
                    else if (ScanningConfiguration.IssueMulitpleScans == true)
                    {
                        // Additional scans are without flush
                        LogMajor("Calling (internal) scan without flush");
                        Api.Scan(TestInterface.Id, false, new TimeSpan(0, 0, 10));
                    }

                    // If any network is marked hidden, we need to specifically search for those
                    for (int j = 0; j < 2; j++)
                    {
                        if (APConfigs[j].BeaconEnabled_24GHZ == false)
                        {
                            LogMajor("Calling (public) scan for " + APConfigs[j].SSID_24GHZ);
                            Api.Scan(TestInterface.Id, false, APConfigs[j].SSID_24GHZ, new TimeSpan(0, 0, 10));
                        }
                        if (APConfigs[j].BeaconEnabled_5GHZ == false)
                        {
                            LogMajor("Calling (public) scan for " + APConfigs[j].SSID_5GHZ);
                            Api.Scan(TestInterface.Id, false, APConfigs[j].SSID_5GHZ, new TimeSpan(0, 0, 10));
                        }
                    }

                    if (!Radio_0_24Ghz_Found)
                    {
                        LogMajor("Scan : Locating " + APConfigs[0].SSID_24GHZ + " in bssid list");
                        if (Helpers.ValidateNetworkVisible(Api, APConfigs[0], false, out Reason_Radio_0_24Ghz_Found))
                        {
                            LogMajor(APConfigs[0].SSID_24GHZ + "found in scan list ");
                            Radio_0_24Ghz_Found = true;
                        }
                        else
                        {
                            LogMajor("ValidateNetworkVisible failed for " + APConfigs[0].SSID_24GHZ);
                        }
                    }

                    if (!Radio_1_24Ghz_Found)
                    {
                        LogMajor("Scan : Locating " + APConfigs[1].SSID_24GHZ + " in bssid list");
                        if (Helpers.ValidateNetworkVisible(Api, APConfigs[1], false, out Reason_Radio_1_24Ghz_Found))
                        {
                            LogMajor(APConfigs[1].SSID_24GHZ + "found in scan list ");
                            Radio_1_24Ghz_Found = true;
                        }
                        else
                        {
                            LogMajor("ValidateNetworkVisible failed for " + APConfigs[1].SSID_24GHZ);

                        }
                    }

                    if (!ScanningConfiguration.DeviceSupports5Ghz)
                    {
                        if (Radio_0_24Ghz_Found && Radio_1_24Ghz_Found)
                        {
                            LogMajor("Scan: Located all routers");
                            return;
                        }
                        else
                        {
                            LogMajor("Scan: Not yet located all routers ");
                            LogMajor("Router 0 2.4Ghz Found =" + Radio_0_24Ghz_Found);
                            LogMajor("Router 1 2.4Ghz Found =" + Radio_1_24Ghz_Found);
                            continue; // skip 5Ghz
                        }
                    }



                    if (!Radio_0_5Ghz_Found)
                    {
                        LogMajor("Scan : Locating " + APConfigs[0].SSID_5GHZ + " in bssid list");
                        if (Helpers.ValidateNetworkVisible(Api, APConfigs[0], true, out Reason_Radio_0_5Ghz_Found))
                        {
                            LogMajor(APConfigs[0].SSID_5GHZ + " found in scan list");
                            Radio_0_5Ghz_Found = true;
                        }
                        else
                        {
                            LogMajor("ValidateNetworkVisible failed for " + APConfigs[0].SSID_5GHZ);
                        }
                    }

                    if (!Radio_1_5Ghz_Found)
                    {

                        LogMajor("Scan : Locating " + APConfigs[1].SSID_5GHZ + " in bssid list");
                        if (Helpers.ValidateNetworkVisible(Api, APConfigs[1], true, out Reason_Radio_1_5Ghz_Found))
                        {
                            LogMajor(APConfigs[1].SSID_5GHZ + " found in scan list");
                            Radio_1_5Ghz_Found = true;
                        }
                        else
                        {
                            LogMajor("ValidateNetworkVisible failed for " + APConfigs[1].SSID_5GHZ);
                        }
                    }

                    if (Radio_0_24Ghz_Found
                        && Radio_1_24Ghz_Found
                        && Radio_0_5Ghz_Found
                        && Radio_1_5Ghz_Found)
                    {
                        LogMajor("Scan: Located all routers");
                        return;
                    }

                    LogMajor("Scan: Not yet located all routers ");
                    LogMajor("Router 0 2.4Ghz Found =" + Radio_0_24Ghz_Found);
                    LogMajor("Router 1 2.4Ghz Found =" + Radio_1_24Ghz_Found);
                    LogMajor("Router 0 5Ghz Found =" + Radio_0_5Ghz_Found);
                    LogMajor("Router 1 5Ghz Found =" + Radio_1_5Ghz_Found);
                }
            }
            catch (Exception error)
            {
                LogMajor(error.ToString());
                throw;

            }
            finally
            {
                wlanEtwWatcher.Dispose();
            }


            LogMajor("Error locating all routers");
            if (!Radio_0_24Ghz_Found)
            {
                LogMajor("Scanning - "+Reason_Radio_0_24Ghz_Found);
                throw new Exception(Reason_Radio_0_24Ghz_Found);
            }
            if (!Radio_1_24Ghz_Found)
            {
                LogMajor("Scanning - "+Reason_Radio_1_24Ghz_Found);
                throw new Exception(Reason_Radio_1_24Ghz_Found);
            }

            if (ScanningConfiguration.DeviceSupports5Ghz)
            {
                if (!Radio_0_5Ghz_Found)
                {
                    LogMajor("Scanning - "+Reason_Radio_0_5Ghz_Found);
                    throw new Exception(Reason_Radio_0_5Ghz_Found);
                }
                if (!Radio_1_5Ghz_Found)
                {
                    LogMajor("Scanning - "+Reason_Radio_1_5Ghz_Found);
                    throw new Exception(Reason_Radio_1_5Ghz_Found);
                }
            }
        }

        public static void Scan (RunTimeConfiguration rc,
            APConfigParameter AP1Config24GHz, 
            APConfigParameter AP1Config5GHz, 
            APConfigParameter AP2Config24GHz,
            APConfigParameter AP2Config5GHz,
            SCAN_TEST_OPTIONS ScanTestOptions
            )
        {
            TraceProvider traceProvider = new TraceProvider("ScanDriver");
            
            using (UsingGroup g = new UsingGroup())
            {            
                traceProvider.TraceInformation("Scanning Scenarios Configuration ");

                WhckScanning hckScanning = new WhckScanning(rc,
                    AP1Config24GHz,
                    AP1Config5GHz,
                    AP2Config24GHz,
                    AP2Config5GHz,
                    ScanTestOptions);
                
                g.Add(hckScanning);

                hckScanning.Scan();
            }
        }
    }
}
