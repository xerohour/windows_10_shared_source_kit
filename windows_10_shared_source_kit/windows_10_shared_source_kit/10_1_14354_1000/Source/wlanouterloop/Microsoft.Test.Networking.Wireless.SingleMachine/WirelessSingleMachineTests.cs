///
///
/// <copyright>
///  (C) Microsoft Corporation. All Rights Reserved.
/// </copyright>
///
///
/// <file>
///  $File$
/// </file>
///
/// <author>  $Author$ </author>
/// <date>    $Date$ </date>
/// <version> $Revision$ </version>
///
/// Information:
///
///
///


using Microsoft.Test.Networking.Power;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Oids;
using Microsoft.Test.Networking.Tracing;
using Microsoft.Test.Networking.DeviceManager;
using Microsoft.Test.Networking.Registry;
using Microsoft.Test.Networking.Wmi;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading;
using WEX.Logging.Interop;
using WEX.TestExecution.Markup;
namespace Microsoft.Test.Networking.Wireless.Outerloop
{
    [TestClass]
    public partial class SystemTests
    {
        /* 
         *  01.	Enumerate device information and capabilities with the wlan service
         *   	Device should show up as present and able to enumerate support for common frequencies and cipher types
         * 
         *  02. Query interface properties (RSNA must be supported, radios must report as on, device must be enabled for autoconfig)
         *      Expectation is that device should be able to report it's properties back.
         *      
         *  03. Set interface properties and toggle radio state.
         *      Expectation: Toggles background scanning and media streaming on the adapter as well as the radio states.
         *      
         *  04. Check for Mandatory OIDs
         *      This test case looks at the list of Mandatory OIDs on MSDN, interrogates the driver of what they are capable of, and compares the two. (This does not include AOAC.)
         * 
         *  05. This test only applies to AOAC systems and checks that the Mandatory AOAC oids have been implemented.
         *   
         *  06.  This test checks device initialization time. It is a windows blue test only. This test will disable and enable the device.
         *  
         *  07. Checks that the device has not reset. Reset count must be 0.
         *  
         *  08.	Scan for networks
         *      Scan should succeed in under 3 seconds and contain at least one network. This test ALSO disables / enables all radios before a scan.
         *  
         *  09.	Add a wireless profile to the OS for a non-existent network and connect
         *      Expectation is that the device should time-out (or return specific error regarding the SSID not being available), no other errors. Timeout is 2 seconds once association starts.
         *      
         *  10.	Connect to an existing WPA2-PSK network using a parameter for SSID and Parameter for Password. Checks that the connection succeeds AND pings the gateway
         *      Expectation is that the device should connect to a WPA2-PSK network in less than 3 seconds. Test also opens a HTTP connection over port 80 and must succeed.
         *         
         *  11.	Check for Hosted Network Capable (SoftAP)
         *      This test checks to see if the adapter is capable of hosted networking (SoftAP)
         * 
         *  12. This test will enable WFD Autonomous GO and check that an SSID is returned.
         *      The test ALSO calls get WFD capabilities. The api must return properly and contain the needed data.
         *  
         */



        public TestContext TestContext
        {
            get;
            set;
        }

        #region Class Setup

        /// <summary>
        /// Class Setup
        /// </summary>
        /// <param name="testContext">The test context.</param>
        [ClassInitialize]
        [TestProperty("Kits.PublishingOrganization", "Microsoft Corporation")]
        [TestProperty("Kits.IsInProc", "False")]
        [TestProperty("Kits.RequiresSpecialConfiguration", "True")]
        [TestProperty("Kits.SupportedOS", "Windows v10.0 Client x86")]
        [TestProperty("Kits.SupportedOS", "Windows v10.0 Client x64")]
        [TestProperty("Kits.SupportedOS", "Windows v10.0 Client ARM")]
        [TestProperty("Kits.SupportedOS", "Windows v10.0 Client ARM64")]
        [TestProperty("Kits.MinRelease", "TH1")]
        [TestProperty("Kits.CorePackageComposition", "Full")]
        [TestProperty("Kits.CorePackageComposition", "Mobile")]
        [TestProperty("Kits.DeploymentItem", @"WlanOuterLoop\Microsoft.Test.Networking.dll")]
        [TestProperty("Kits.DeploymentItem", @"WlanOuterLoop\Microsoft.Test.Networking.Kit.dll")]
        [TestProperty("Kits.DeploymentItem", @"WlanOuterLoop\Microsoft.Test.Networking.Radius.Client.dll")]
        [TestProperty("Kits.DeploymentItem", @"WlanOuterLoop\Microsoft.Test.Networking.RemoteControl.dll")]
        [TestProperty("Kits.DeploymentItem", @"WlanOuterLoop\Microsoft.Test.Networking.Wireless.dll")]
        [TestProperty("Kits.DeploymentItem", @"nethlk\TraceProviders.txt")]
        [TestProperty("Kits.DeploymentItem", @"nethlk\SleepProvider.dll")]
        
        //
        [TestProperty("Kits.Parameter", "EnableTracing")]
        [TestProperty("Kits.Parameter.EnableTracing.Description", "Enable Tracing")]
        [TestProperty("Kits.Parameter.EnableTracing.Default", "NO")]
        [TestProperty("Kits.Parameter.EnableTracing.IsRequired", "True")]
        [TestProperty("Kits.Parameter.EnableTracing.IsUserSettable", "True")]
        //
        [TestProperty("Kits.TestTask", "CheckforCoreOS")]
        [TestProperty("Kits.TestTask.CheckforCoreOS.Stage", "Setup")]
        [TestProperty("Kits.TestTask.CheckforCoreOS.CommandLine", @"cmd /c if /I ""[HLK\ClientType]"" == ""OneCore"" (setx coresku ""TRUE"" /M)")]        
        [TestProperty("Kits.TestTask.CheckforCoreOS.RunElevated", "True")]
        [TestProperty("Kits.TestTask.CheckforCoreOS.ExpectedExitCode", "0,1")]
        [TestProperty("Kits.TestTask.CheckforCoreOS.Order", "1")]
        //
        [TestProperty("Kits.TestTask", "Disablelwtnetlog")]
        [TestProperty("Kits.TestTask.Disablelwtnetlog.Stage", "Setup")]
        [TestProperty("Kits.TestTask.Disablelwtnetlog.CommandLine", @"cmd /c IF NOT ""[coresku]"" == ""TRUE"" ( logman stop lwtnetlog -ets > lwtnetlog.log 2>&1) else (echo no client which is ok)")]
        [TestProperty("Kits.TestTask.Disablelwtnetlog.RunElevated", "True")]
        [TestProperty("Kits.TestTask.Disablelwtnetlog.ExpectedExitCode", "0,1,-2144337918")]
        [TestProperty("Kits.TestTask.Disablelwtnetlog.Order", "2")]        
        //
        [TestProperty("Kits.TestTask", "StopAnyRunningTracing")]
        [TestProperty("Kits.TestTask.StopAnyRunningTracing.Stage", "Setup")]
        [TestProperty("Kits.TestTask.StopAnyRunningTracing.CommandLine", @"cmd /c IF /I ""[EnableTracing]"" == ""TEST"" (netsh trace stop & echo yes) ELSE (if /I ""[coresku]"" == ""TRUE"" (tracelog -stop Wlan_dbg_log & echo yes) ELSE (logman stop Wlan_dbg_log -ets & echo yes))")]
        [TestProperty("Kits.TestTask.StopAnyRunningTracing.RunElevated", "True")]
        [TestProperty("Kits.TestTask.StopAnyRunningTracing.ExpectedExitCode", "0,1")]
        [TestProperty("Kits.TestTask.StopAnyRunningTracing.Order", "3")]
        //
        [TestProperty("Kits.TestTask", "StartTracing")]
        [TestProperty("Kits.TestTask.StartTracing.Stage", "Setup")]
        [TestProperty("Kits.TestTask.StartTracing.CommandLine", @"cmd /c if /I ""[EnableTracing]"" == ""YES""  (if /I ""[coresku]"" == ""TRUE"" (tracelog -start Wlan_dbg_log -f Wlan_dbg_log.etl -guid TraceProviders.txt -cir 2000 -rt -ls -ft 1 > tracelog_trace.log 2>&1) ELSE (logman create trace Wlan_dbg_log  -pf TraceProviders.txt -o Wlan_dbg_log.etl -ow -max 2000 -f bin -nb 16 512 -bs 1024 -y -ets 1>logman_trace.log 2>&1)) ELSE (IF /I ""[EnableTracing]"" == ""TEST"" (netsh trace start wlan_dbg provider={98388ca6-f76d-45c6-9a17-7d5a1150856c} 0xff provider={9F584AE9-2A70-43BB-B015-9CCB241FB079} 0xff capture=yes tracefile=Wlan_dbg_log.etl maxSize=1000 over=yes report=disabled correlation=disabled 1>netsh_trace.log 2>&1))")]
        [TestProperty("Kits.TestTask.StartTracing.RunElevated", "True")]
        [TestProperty("Kits.TestTask.StartTracing.ExpectedExitCode", "0")]
        [TestProperty("Kits.TestTask.StartTracing.Order", "4")]
        //
        [TestProperty("Kits.TestTask", "StopTracing")]
        [TestProperty("Kits.TestTask.StopTracing.Stage", "Cleanup")]
        [TestProperty("Kits.TestTask.StopTracing.CommandLine", @"cmd /c if /I ""[EnableTracing]"" == ""YES""  (if /I ""[coresku]"" == ""TRUE"" (tracelog -stop Wlan_dbg_log >> StopTracing.log) ELSE (logman stop Wlan_dbg_log -ets >> StopTracing.log)) ELSE (IF /I ""[EnableTracing]"" == ""TEST"" (netsh trace stop >> StopTracing.log))")]
        [TestProperty("Kits.TestTask.StopTracing.RunElevated", "True")]
        [TestProperty("Kits.TestTask.StopTracing.ExpectedExitCode", "0")]
        [TestProperty("Kits.TestTask.StopTracing.Order", "1")]  
        //
        [TestProperty("Kits.CopyTestResult", "Wlan_dbg_log.etl")]
        [TestProperty("Kits.CopyTestResult", "*.log")]
    

        public static void ClassSetup(TestContext context)
        {

        }

        #endregion

        #region Wlan System Test Method Setup

        [TestInitialize]
        public void TestMethodSetup()
        {
            //logger.LogComment("Entering TestMethodSetup");

            try
            {
                if (TestContext == null)
                {
                    //      logger.LogError( "Test Context has not been set yet, this is a Fatal Error. Taef should be setting this.");
                    return;
                }

                //if (!m_testContext.Properties.Contains("queryTestDeviceID") && !m_testContext.Properties.Contains("DQ"))
                //{
                //    logger.LogError( "There was no value for \"queryTestDeviceID\" or \"DQ\" passed to the WlanL1Tests. Parameters passed: \r\n{0}", m_testContext.Properties);
                //    return;
                //}
                EventTraceWatcher.StopandDeleteAllWlanETWListeners();


            }
            catch (System.Exception setupEx)
            {
                if (setupEx is NullReferenceException || setupEx is System.Runtime.InteropServices.SEHException)
                {
                    throw;
                }
                //logger.LogError( "There was an exception during TestMethodSetup: {0}. This is a fatal error.", setupEx);
            }
        }
        [TestCleanup]
        public void TestMethodCleanup()
        {

        }

        #endregion

        #region Wlan Test - Wlan Device enumeration

        /// <summary>
        /// Wlan Test - Wlan Device enumeration
        /// 
        /// Wlan Device enumerates using the Wlan Service and lists the device state
        /// 
        /// </summary>        
        [TestProperty("Kits.TestId", "177CFB0E-1A09-4EC7-9053-02CA973415A0")]
        [TestProperty("Kits.TestId2", "564fb6dd-030b-48e8-9df7-42831dbacc6a")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device enumeration")]
        [TestProperty("Kits.Description", "Wlan Device enumerates using the Wlan Service and lists the device state")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "System.Client.WLAN.BasicConnectivity.WlanBasicConnectivity")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.ExtendedDocumentation", "5317A62E-3CC8-491B-B64B-1B5DB0045F4A")]
        [TestMethod]
        public void SystemTest_DeviceAppearsAndEnumeratesProperties()
        {
            Test_01_DeviceAppearsAndEnumeratesProperties();
        }

        [TestProperty("Kits.TestId", "78796EFA-D844-4CCB-AB95-5BC54F002410")]
        [TestProperty("Kits.TestId2", "5918a9ce-1281-4c24-b70f-584c7347c3df")]
        [TestProperty("Kits.TestName", "Wlan Test - Wlan Device enumeration")]
        [TestProperty("Kits.Description", "Wlan Device enumerates using the Wlan Service and lists the device state")]
        [TestProperty("Kits.ExpectedRuntime", "1")]
        [TestProperty("Kits.TimeoutInMinutes", "5")]
        [TestProperty("Kits.Specification", "Device.Network.WLAN.SupportConnectionToAP.ConnectionToAP")]
        [TestProperty("Kits.DevelopmentPhase", "Bring Up")]
        [TestProperty("Kits.TestType", "Development")]
        [TestProperty("Kits.Parameter", "queryTestDeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Description", "Device id of device under test")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.Default", "//Devnode/DeviceID")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsUserSettable", "False")]
        [TestProperty("Kits.Parameter.queryTestDeviceID.IsRequired", "False")]
        [TestProperty("Kits.ExtendedDocumentation", "5317A62E-3CC8-491B-B64B-1B5DB0045F4A")]
        [TestMethod]
        public void DeviceTest_DeviceAppearsAndEnumeratesProperties()
        {
            Test_01_DeviceAppearsAndEnumeratesProperties();
        }
        private void Test_01_DeviceAppearsAndEnumeratesProperties()
        {
            RunTest((wlanApi, wlanInterface, logger) =>
            {
                logger.LogComment("Interface Description: {0}", wlanInterface.Description);

                WlanInterfaceCapability interfaceCaps = wlanApi.GetInterfaceCapabilities(wlanInterface);
                logger.LogComment("Interface Phy Types");
                foreach (var phy in interfaceCaps.PhyTypes)
                {
                    logger.LogComment("    Phy Type {0} Wlan Type {1}", phy.ToString(), ((WlanType)phy).ToString());
                }
                logger.LogComment("Max SSID List {0}", interfaceCaps.MaxDesiredSsidList);
                logger.LogComment("Max BSSID List {0}", interfaceCaps.MaxDesiredBssidList);
                logger.LogComment("Interface Radio Type: {0}", interfaceCaps.InterfaceType);

                logger.LogComment("Interface State: {0}", wlanInterface.State);

                if (wlanInterface.State.Equals(WLAN_INTERFACE_STATE.wlan_interface_state_not_ready))
                {
                    logger.LogError("Device is reporting as \"Not Ready!\"");
                    logger.LogResult(TestResult.Failed);
                    return;
                }

                logger.LogResult(TestResult.Passed);
            });
        }
        #endregion


        #region Wlan Test - Query Interface Properties

       
        private void Test_02_QueryInterfaceProperties()
        {
            bool xboxRun = false;
            if (TestContext != null)
            {
                if (TestContext.Properties.Contains("xboxRun"))
                {
                    xboxRun = bool.Parse(TestContext.Properties["xboxRun"].ToString());
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
          {
              bool bFailed = false;
              try
              {
                  logger.LogComment("Querying driver whether it supports wlan_intf_opcode_media_streaming_mode ...");
                  try
                  {
                      var mediaStreamingMode = wlanApi.IsMediaStreamingEnabled(wlanInterface);
                      logger.LogComment("Media Streaming mode is:{0}", mediaStreamingMode);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsMediaStreamingEnabled with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  logger.LogComment("Querying driver whether background scan is enabled ...");
                  try
                  {
                      var backgroundScanEnabled = wlanApi.IsBackgroundScanEnabled(wlanInterface);
                      logger.LogComment("Background Scan Enabled is:{0}", backgroundScanEnabled);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsBackgroundScanEnabled with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  logger.LogComment("Querying driver whether safe mode is supported ...");
                  try
                  {
                      var supportSafeMode = wlanApi.IsSafeModeSupported(wlanInterface);
                      logger.LogComment("Supports safe mode is:{0}", supportSafeMode);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsSafeModeSupported with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  logger.LogComment("Querying driver whether safe mode is certified ...");
                  try
                  {
                      var certifiedSafeMode = wlanApi.IsSafeModeCertified(wlanInterface);
                      logger.LogComment("Certified safe mode is:{0}", certifiedSafeMode);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsSafeModeCertified with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  logger.LogComment("Querying driver for current operation mode ...");
                  try
                  {
                      var currentOperationMode = wlanApi.GetCurrentOperationMode(wlanInterface);
                      logger.LogComment("Current Operation mode is:{0}", currentOperationMode);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("GetCurrentOperationMode with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  logger.LogComment("Querying driver for current interface state ...");
                  try
                  {
                      var interfaceState = wlanApi.GetInterfaceState(wlanInterface);
                      logger.LogComment("Current Interface state is:{0}", interfaceState);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("GetInterfaceState with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  logger.LogComment("Querying driver for management frame protection support ...");
                  try
                  {
                      var supportManagementFrameProtection = wlanApi.IsManagementFrameProtectionSupported(wlanInterface);
                      logger.LogComment("Management Frame Protection: {0}", supportManagementFrameProtection);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsManagementFrameProtectionSupported with error {0}", GetStringForErrorCode(ex.HResult));
                  }


                  logger.LogComment("Querying driver for hosted network capable ...");
                  try
                  {
                      var hostedNetworkCapable = wlanApi.IsHostedNetworkCapable(wlanInterface);
                      logger.LogComment("Hosted Network Capable: {0}", hostedNetworkCapable);

                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsHostedNetworkCapable with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  try
                  {
                      var radioStates = wlanApi.GetRadioState(wlanInterface);
                      foreach (var radioState in radioStates)
                      {
                          logger.LogComment("Radio Interface {0}, Software State {1}, Hardware State {2}", radioState.PhysicalIndex, radioState.SoftwareRadioState, radioState.HardwareRadioState);
                          if (!radioState.HardwareRadioState.Equals(RadioState.On))
                          {
                              logger.LogError("Radio {0} is not on in hardware. This will cause a failure.", radioState.PhysicalIndex);
                              logger.LogResult(TestResult.Failed);
                          }
                          if (!radioState.SoftwareRadioState.Equals(RadioState.On))
                          {
                              logger.LogError("Radio {0} is not on in software. This will cause a failure.", radioState.PhysicalIndex);
                              logger.LogResult(TestResult.Failed);
                          }
                      }
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("GetRadioState with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  try
                  {
                      var autoConfigEnabled = wlanApi.IsAutoConfigEnabled(wlanInterface);
                      logger.LogComment("Autoconfig enabled on interface {0}", autoConfigEnabled);
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("IsAutoConfigEnabled with error {0}", GetStringForErrorCode(ex.HResult));
                  }
                  try
                  {
                      var supportedInfrastructureCipherPairs = wlanApi.GetSupportedInfrastructureCipherPairs(wlanInterface);
                      if (supportedInfrastructureCipherPairs.Count < 3)
                      {
                          logger.LogError("CipherPair count should not be less than 3.");
                          logger.LogResult(TestResult.Failed);
                          bFailed = true;
                      }

                      bool bRSNAPSK = false;
                      logger.LogComment("Supported Infrastructure Cipher Pairs:");
                      foreach (var cipherPair in supportedInfrastructureCipherPairs)
                      {
                          logger.LogComment("Auth Algo Id: {0} Cipher Algo Id: {1}", cipherPair.AuthAlgoId, cipherPair.CipherAlgoId);
                          if (cipherPair.AuthAlgoId == DOT11_AUTH_ALGORITHM.DOT11_AUTH_ALGO_RSNA_PSK)
                          {
                              bRSNAPSK = true;
                          }
                      }
                      if (!bRSNAPSK)
                      {
                          logger.LogError("WPA2-PSK is not supported!! This will cause a failure.");
                          logger.LogResult(TestResult.Failed);
                          bFailed = true;
                      }
                  }
                  catch (COMException ex)
                  {
                      logger.LogError("GetSupportedInfrastructureCipherPairs with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  bool IsAoAc = PowerManager.IsAoAcCapableMachine();

                  if (!IsAoAc)
                  {
                      try
                      {
                          var supportedAdhocCipherPairs = wlanApi.GetSupportedAdHocCipherPairs(wlanInterface);
                          logger.LogComment("Supported Adhoc Cipher Pairs:");
                          foreach (var cipherPair in supportedAdhocCipherPairs)
                          {
                              logger.LogComment("Auth Algo Id: {0} Cipher Algo Id: {1}", cipherPair.AuthAlgoId, cipherPair.CipherAlgoId);
                          }
                      }
                      catch (COMException ex)
                      {
                          logger.LogWarning("GetSupportedAdHocCipherPairs with error {0}", GetStringForErrorCode(ex.HResult));
                      }
                  }
                  else
                  {
                      logger.LogComment("Skipping supportedAdhocCipherPairs on AoAc machines.");
                  }

                  var interfaceCaps = wlanApi.GetInterfaceCapabilities(wlanInterface);
                  logger.LogComment("Interface type {0} Supports Dot11D {1}", interfaceCaps.InterfaceType, interfaceCaps.SupportsDot11D);
                  logger.LogComment("Phy Types");
                  foreach (var phy in interfaceCaps.PhyTypes)
                  {
                      logger.LogComment("Phy Type {0} Wlan Type {1}", phy.ToString(), ((WlanType)phy).ToString());
                  }
                  try
                  {
                      var wlanStatistics = wlanApi.GetStatistics(wlanInterface);
                      logger.LogComment("Statistics {0}", wlanStatistics);
                  }

                  catch (System.Runtime.InteropServices.COMException ex)
                  {
                      logger.LogWarning("GetStatistics with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  try
                  {
                      var bssMode = wlanApi.GetBssMode(wlanInterface);
                      logger.LogComment("Bss Mode is {0}", bssMode);
                  }
                  catch (System.Runtime.InteropServices.COMException ex)
                  {
                      logger.LogError("GetBssMode with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  //Support appears to be optional
                  try
                  {
                      var supportedCountries = wlanApi.GetCountryList(wlanInterface);
                      logger.LogComment("Supported Countries:");
                      foreach (var country in supportedCountries)
                      {
                          logger.LogComment(country);
                      }
                  }

                  catch (System.Runtime.InteropServices.COMException ex)
                  {
                      logger.LogWarning("GetCountryList with error {0}", GetStringForErrorCode(ex.HResult));
                  }

                  logger.LogComment("Querying GetIfEntry2 for Media Types");
                  var adapters = IpHelpers.GetAddresses();
                  var adapter = adapters.Find(x => { return x.AdapterGuid == wlanInterface.Id; });
                  if (adapter == null)
                  {
                      logger.LogError("Adapter was not found via GetAdapterInfo");
                  }
                  else
                  {
                      logger.LogComment("Adapter returned IFTYPE:{0} NDIS_MEDIUM:{1} NDIS_PHYSICAL_MEDIUM:{2}", adapter.Type, adapter.MediaType, adapter.PhysicalMediumType);
                      if (adapter.Type != IFTYPE.Ieee80211)
                      {
                          if (xboxRun)
                          {
                              logger.LogWarning("Adapter IFTYPE was not set to Ieee80211 was {0}", adapter.Type);
                          }
                          else
                          {
                              logger.LogError("Adapter IFTYPE was not set to Ieee80211 was {0}", adapter.Type);
                          }
                      }
                      if (adapter.MediaType != NDIS_MEDIUM.Medium802_3)
                      {
                          logger.LogError("Adapter NDIS_MEDIUM was not set to Medium802_3 was {0}", adapter.MediaType);
                      }
                      if (adapter.PhysicalMediumType != NDIS_PHYSICAL_MEDIUM.Native802_11)
                      {
                          logger.LogError("Adapter NDIS_PHYSICAL_MEDIUM was not set to Native802_11");
                      }
                  }

              }
              catch (System.Exception ex)
              {
                  logger.LogError("During Test case 02, there has been an unknown error. This will cause a test failure.\r\n{0}", ex.ToString());
                  logger.LogResult(TestResult.Failed);
                  bFailed = true;
              }

              if (!bFailed)
              {
                  logger.LogResult(TestResult.Passed);
              }
          });
        }

        private static string GetStringForErrorCode(int hResult)
        {
            return InteropUtilities.GetWin32FromHr(hResult).ToString();
        }

        #endregion
        #region Wlan Test - Toggle airplane mode





        private void Test_06_ToggleAirplaneMode()
        {
            RunTest((wlanApi, wlanInterface, logger) =>
           {


               RadioResetResult result = DisableAndEnableRadios(wlanApi, wlanInterface, logger, true);
               if (result != RadioResetResult.RadioHasBeenReset)
               {
                   logger.LogError("There was an error while resetting the radios! {0}", result.ToString());
               }
               else
               {
                   logger.LogComment("Radios were successfully disabled and enabled.");
               }
           });
        }

        #endregion
        #region Wlan Test - Scan for Networks

        /// <summary>
        /// Wlan Test - Scan for Networks
        /// Scan for networks using the Wlan device and expect results
        /// Fail if it takes longer than 6 seconds!
        /// </summary>


        private void Test_08_ScanForNetworks()
        {
            bool xboxRun = false;
            if (TestContext.Properties.Contains("xboxrun"))
            {
                if (!bool.TryParse(TestContext.Properties["xboxrun"] as string, out xboxRun))
                {
                    xboxRun = false;
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
          {
              ScanAndVerfiyNonEmpty(wlanApi, wlanInterface, logger, true, true, !xboxRun);
          });
        }

        #endregion
        #region Wlan Test - Attempt a connection to a non-existent network

        /// <summary>
        /// This test case will create and save a profile for a non-existent network. 
        /// The test is designed to ensure that connecting to this non-existent network fails quickly and does not cause
        /// any other failures either.
        /// </summary>



        private void Test_09_NonExistentWlanProfileConnect()
        {
            bool xboxRun = false;
            if (TestContext.Properties.Contains("xboxrun"))
            {
                if (!bool.TryParse(TestContext.Properties["xboxrun"] as string, out xboxRun))
                {
                    xboxRun = false;
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
          {
              logger.LogComment("{0}Performing scan prior to connect attempt.", xboxRun ? "" : "Resetting Wlan Device AND ");

              ScanAndVerfiyNonEmpty(wlanApi, wlanInterface, logger, false, true, !xboxRun);

              EventHandler<WlanConnectionEventArgs> HandleConnectStart = null;
              EventHandler<WlanConnectionEventArgs> HandleConnectFail = null;
              EventHandler<WlanConnectionEventArgs> HandleConnectComplete = null;
              EventHandler<ConnectionStatusEventArgs> HandleAssociationStart = null;
              EventHandler<ConnectionStatusEventArgs> HandleAssociationComplete = null;
              EventHandler<WlanInterfaceEventArgs> HandleScanComplete = null;
              EventHandler<WlanInterfaceEventArgs> HandleScanRefresh = null;

              Stopwatch Association = new Stopwatch();
              Stopwatch ConnectTimer = new Stopwatch();

              ManualResetEvent ConnectStart = new ManualResetEvent(false);
              ManualResetEvent AssociationStart = new ManualResetEvent(false);
              ManualResetEvent AssociationFail = new ManualResetEvent(false);
              ManualResetEvent ConnectFail = new ManualResetEvent(false);

              Dictionary<string, bool> States = new Dictionary<string, bool>();
              States["AssociationStart"] = false;
              States["AssociationFail"] = false;
              States["ConnectStart"] = false;
              States["ConnectFail"] = false;
              States["ConnectComplete"] = false;

              try
              {
                  var profileToUse = CreatePskWlanProfile("NonExistWPA2PSK", "NonExistWPA2PSK", "B@dPassw0rd!", false, true);

                  logger.LogComment("Creating profile for non-existent network.");
                  wlanApi.CreateProfile(wlanInterface.Id, profileToUse, Wlan.ProfileScope.AllUser);

                  HandleConnectStart = (sender, eventArgs) =>
                  {
                      States["ConnectStart"] = true;
                      logger.LogComment("WlanSvc reports Connection Started (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                      ConnectStart.Set();
                  };

                  HandleConnectFail = (sender, eventArgs) =>
                  {
                      ConnectTimer.Stop();
                      if (Association.IsRunning)
                      {
                          Association.Stop();
                      }

                      States["ConnectFail"] = true;

                      logger.LogComment("Connection Failed!:{0} (Time: {1}ms)", eventArgs.ReasonCode, ConnectTimer.ElapsedMilliseconds);

                      if (!States["AssociationStart"])
                      {
                          AssociationStart.Set();
                      }
                      if (!States["AssociationFail"])
                      {
                          AssociationFail.Set();
                      }
                      if (!States["ConnectStart"])
                      {
                          ConnectStart.Set();
                      }
                      ConnectFail.Set();
                  };

                  HandleConnectComplete = (sender, eventArgs) =>
                  {
                      ConnectTimer.Stop();
                      logger.LogComment("Connection Complete!:{0} (Time: {1}ms)", eventArgs.ReasonCode, ConnectTimer.ElapsedMilliseconds);
                      States["ConnectComplete"] = true;
                      if ((eventArgs.ReasonCode != WlanReasonCode.WLAN_REASON_CODE_SUCCESS) && (!States["ConnectFail"]))
                      {
                          HandleConnectFail(sender, eventArgs);
                      }
                  };

                  HandleAssociationStart = (sender, eventArgs) =>
                  {
                      Association.Restart();
                      logger.LogComment("MSM has reported that it is attempting to associate. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                      States["AssociationStart"] = true;
                      AssociationStart.Set();
                  };

                  HandleAssociationComplete = (sender, eventArgs) =>
                  {
                      Association.Stop();
                      logger.LogComment("MSM has reported that association has completed. Result: {2} Association Time: {1}ms (Overall Time: {0}ms) ", ConnectTimer.ElapsedMilliseconds, Association.ElapsedMilliseconds, eventArgs.ReasonCode);
                      if (eventArgs.ReasonCode != WlanReasonCode.WLAN_REASON_CODE_SUCCESS)
                      {
                          States["AssociationFail"] = true;
                          AssociationFail.Set();
                      }
                  };

                  HandleScanComplete = (sender, eventArgs) =>
                  {
                      logger.LogComment("Driver reports it has completed a scan operation. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                  };

                  HandleScanRefresh = (sender, eventArgs) =>
                  {
                      logger.LogComment("Driver reports it has refreshed a scan operation. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                  };

                  wlanApi.ConnectionStart += HandleConnectStart;
                  wlanApi.ConnectionFail += HandleConnectFail;
                  wlanApi.ConnectionComplete += HandleConnectComplete;
                  wlanApi.Associating += HandleAssociationStart;
                  wlanApi.Associated += HandleAssociationComplete;
                  wlanApi.ScanComplete += HandleScanComplete;
                  wlanApi.ScanRefreshed += HandleScanRefresh;

                  logger.LogComment("Starting profile connect now...");

                  ConnectTimer.Restart();
                  wlanApi.ProfileConnect(wlanInterface.Id, "NonExistWPA2PSK");

                  if (!ConnectStart.WaitOne(11000))
                  {
                      logger.LogError("Failed to get \"Connect Start\" event within 11 seconds.");
                  }
                  else logger.LogComment("Connection started... (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);

                  if (!AssociationStart.WaitOne(11000))
                  {
                      logger.LogError("Failed to get \"Association Start\" event within 11 seconds. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                  }
                  else logger.LogComment("Association started... (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);

                  if (!AssociationFail.WaitOne(11000))
                  {
                      logger.LogError("Failed to get \"Association Failure\" event within 11 seconds. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                  }
                  else logger.LogComment("Association failed as expected... (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);

                  if (Association.ElapsedMilliseconds > 11000)
                  {
                      logger.LogError("Association failure took longer than 11s to complete. Measured association time:{0}", Association.ElapsedMilliseconds);
                  }

                  if (!ConnectFail.WaitOne(11000))
                  {
                      logger.LogError("Failed to get \"Connect Failure\" event within 11 seconds. (Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);
                  }
                  else logger.LogComment("Connect completed... (Total Time: {0}ms)", ConnectTimer.ElapsedMilliseconds);

                  if (ConnectTimer.ElapsedMilliseconds > 11000)
                  {
                      logger.LogError("Connect failure took longer than 11s to complete.");
                  }

                  wlanApi.DeleteProfile(wlanInterface.Id, "NonExistWPA2PSK");
              }
              catch (System.Exception ex)
              {
                  if (ex is NullReferenceException || ex is System.Runtime.InteropServices.SEHException)
                  {
                      throw;
                  }
                  logger.LogError("Exception hit while running Test 09 - Attempt a profile connect: {0}", ex.ToString());
              }
              finally
              {
                  wlanApi.ConnectionStart -= HandleConnectStart;
                  wlanApi.ConnectionFail -= HandleConnectFail;
                  wlanApi.ConnectionComplete -= HandleConnectComplete;
                  wlanApi.Associating -= HandleAssociationStart;
                  wlanApi.Associated -= HandleAssociationComplete;
                  wlanApi.ScanComplete -= HandleScanComplete;
              }
          });
        }

        #endregion
        #region Wlan Test - Connect to a WPA2 PSK AES AP

        /// <summary>
        /// This test requires two parameters WPA2_PSK_AES_SSID and WPA2_PSK_Password.
        /// 
        /// The test will simply attempt to connect to the network and expect the connection to succeed.
        /// </summary>




        private void Test_10_WlanConnectTest()
        {
            bool strict = true;
            if (TestContext.Properties.Contains("strict"))
            {
                if (!bool.TryParse(TestContext.Properties["strict"] as string, out strict))
                {
                    strict = true;
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
          {
              ConnectTest(wlanApi, wlanInterface, strict, true, logger, false);
          });
        }

        #endregion
        #region Wlan Test - Check Hosted Network Capable

        /// <summary>
        /// Checks that the device is capable of supporting SoftAP
        /// </summary>



        private void Test_11_CheckHostedNetwork()
        {
            RunTest((wlanApi, wlanInterface, logger) =>
          {
              try
              {
                  bool bSoftAP = wlanApi.IsHostedNetworkCapable(wlanInterface);

                  if (!bSoftAP)
                  {
                      logger.LogComment("Device does not support SoftAP.");
                      return;
                  }
                  else
                  {
                      logger.LogComment("Device Supports SoftAP - running the test");
                      HostedNetworkTest(wlanApi, logger);
                  }
              }
              catch (System.Exception ex)
              {
                  if (ex is NullReferenceException || ex is System.Runtime.InteropServices.SEHException)
                  {
                      throw;
                  }
                  logger.LogError("Exception hit while running Test 11 - Check Hosted Network Capable: {0}", ex);
              }
          });
        }

        #endregion
        #region Wlan Test - Enable and Disable WFD Autonomous GO



       
        private void Test_12_EnableDisableAutoGO()
        {
            RunTest((wlan, wlanInterface, logger) =>
            {
                logger.LogComment("Entering Test_12_EnableDisableAutoGO");
                var enhancedCaps = wlan.GetWlanEnhancedCapabilites(wlanInterface);
                if (!enhancedCaps.WFDGOModeSupported)
                {
                    logger.LogComment("Wifi Direct GO not supported, Exiting");
                    return;
                }
                using (var wifiDirect = new WifiDirect())
                {
                    wifiDirect.WfdNotificationStateChanged += (object sender, EventArgs eventArgs) => { logger.LogComment("{0}:{1}", sender, eventArgs); };
                    try
                    {
                        logger.LogComment("Getting WFD Capabilities ...");
                        var capabilities = wifiDirect.QueryWFDCapabilities();

                        logger.LogComment("Service Discovery Supported: {0}", capabilities.DeviceCapabilities.ServiceDiscoverySupported);
                        logger.LogComment("Client Discovery Supported: {0}", capabilities.DeviceCapabilities.ClientDiscoverabilitySupported);
                        logger.LogComment("Concurrent Operation Supported: {0}", capabilities.DeviceCapabilities.ConcurrentOperationSupported);
                        logger.LogComment("Infrastructure Management Supported: {0}", capabilities.DeviceCapabilities.InfrastructureManagementSupported);
                        logger.LogComment("Device Limit Reached: {0}", capabilities.DeviceCapabilities.DeviceLimitReached);
                        logger.LogComment("Invitation Procedure Supported: {0}", capabilities.DeviceCapabilities.InvitationProcedureSupported);

                        if (!capabilities.DeviceCapabilities.ConcurrentOperationSupported)
                        {
                            logger.LogError("Concurrent Operation is not supported. This will cause a failure!");
                            logger.LogResult(TestResult.Failed);
                        }
                    }
                    catch (System.Exception ex)
                    {
                        logger.LogError("There was an error while attempting to call QueryWFDCapabilities on the device: {0}", ex.ToString());
                        logger.LogResult(TestResult.Failed);

                        logger.LogComment("The test will fail. However, we will attempt to still enable and disable AutoGO...");
                    }

                    try
                    {
                        logger.LogComment("Enabling Wifi-Direct AutoGO ...");

                        string WFDSSID = wifiDirect.StartAutoGO();

                        //Waiting half-second for errors to appear. Ideally, this needs to just check for the proper events.
                        Wlan.Sleep(500);

                        logger.LogComment("AutoGO started with SSID: {0}", WFDSSID);
                        logger.LogComment("Retrieving Device ID");
                        try
                        {
                            var deviceId = wifiDirect.GetDeviceId();
                            logger.LogComment("Device ID {0}", deviceId);
                        }
                        catch (System.Exception ex)
                        {
                            logger.LogError("There was an error while attempting to retrieve the Device Id on the device: {0}", ex.ToString());
                            logger.LogResult(TestResult.Failed);
                        }
                        logger.LogComment("Retrieving Interface Address");
                        try
                        {
                            var interfaceAddress = wifiDirect.GetInterfaceAddress();
                            logger.LogComment("Interface Address {0}", interfaceAddress);
                        }
                        catch (System.Exception ex)
                        {
                            logger.LogError("There was an error while attempting to retrieve the interface Address on the device: {0}", ex.ToString());
                            logger.LogResult(TestResult.Failed);
                        }
                    }
                    catch (System.Exception ex)
                    {
                        logger.LogError("There was an error while attempting to enable Wifi-Direct AutoGO: {0}", ex.ToString());
                        logger.LogResult(TestResult.Failed);
                        return;
                    }
                    finally
                    {
                        logger.LogComment("Disabling Wifi-Direct AutoGO ...");

                        wifiDirect.StopAutoGO();
                    }
                    logger.LogComment("Attempting WFD Search");
                    var wfdDevices = wifiDirect.WfdDiscoverDevices();
                    logger.LogComment("Found {0} WFD Devices", wfdDevices.Count);
                    foreach (var wfdDevice in wfdDevices)
                    {
                        logger.LogComment("\t{0}:{1}", wfdDevice.DeviceName, wfdDevice.DeviceAddress);
                    }
                    logger.LogComment("Retrieving Visible Devices");
                    var visibleDevices = wifiDirect.WfdGetVisibleDevices();
                    logger.LogComment("Found {0} WFD Devices", visibleDevices.Count);
                    foreach (var wfdDevice in visibleDevices)
                    {
                        logger.LogComment("\t{0}:{1}", wfdDevice.DeviceName, wfdDevice.DeviceAddress);
                    }
                }

            });
        }

        #endregion
        #region Wlan Test - Wlan Device Capabilities Check



        private void Test_13_WlanCapabilitiesTest()
        {
            bool validateVersion = true;
            if (TestContext.Properties.Contains("ValidateVersion"))
            {
                if (!bool.TryParse(TestContext.Properties["ValidateVersion"] as string, out validateVersion))
                {
                    validateVersion = true;
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
            {
                logger.LogComment("Interface Description: {0}", wlanInterface.Description);

                var enhancedCaps = wlanApi.GetWlanEnhancedCapabilites(wlanInterface);
                logger.LogComment(enhancedCaps.ToString());

                if (ValidateWlanAdapterCapabilities(enhancedCaps, logger, validateVersion))
                {

                    logger.LogResult(TestResult.Passed);
                }
                else
                {
                    logger.LogResult(TestResult.Failed);
                }


            });
        }

        #endregion
        #region Wlan Test - Wlan Device Reset Check



        
        private void Test_14_WlanDeviceReset()
        {
            var timeout = 60 * 1000;
            var commandCount = 20;
            var useNdisEvent = true;
            CountdownEvent remainingRemovals = new CountdownEvent(2);
            CountdownEvent remainingArrivals = new CountdownEvent(2);
            RunTest((wlanApi, wlanInterface, logger) =>
           {

               EventHandler<WlanInterfaceEventArgs> AdapterArrivalHandler = (object sender, WlanInterfaceEventArgs e) =>
               {

                   if (e.InterfaceId == wlanInterface.Id)
                   {
                       logger.LogComment("Adapter arrival Event seen on Interface: {0}", e.InterfaceId);
                       if (!useNdisEvent)
                       {
                           remainingArrivals.Signal();
                       }
                   }
               };
               EventHandler<WlanInterfaceEventArgs> AdapterRemovalHandler = (object sender, WlanInterfaceEventArgs e) =>
               {

                   if (e.InterfaceId == wlanInterface.Id)
                   {
                       logger.LogComment("Adapter removal Event seen on Interface: {0}", e.InterfaceId);
                       if (!useNdisEvent)
                       {
                           remainingRemovals.Signal();
                       }
                   }
               };                            

               var enhancedCaps = wlanApi.GetWlanEnhancedCapabilites(wlanInterface);
               var systemCaps = wlanApi.GetWlanSystemCapabilites(wlanInterface);

               logger.LogComment("Device reported platform Level reset=({0}) System wlan bus Level Reset=({1})", enhancedCaps.PlatformLevelResetRecoverySupported, enhancedCaps.WLANBusLevelDeviceResetSupported);
               if (enhancedCaps.WLANBusLevelDeviceResetSupported != DOT11_TRI_STATE.dot11_tri_state_true)
               {
                   logger.LogComment("System bus does not support reset recovery,Exiting");
                   return;
               }
               if (!(enhancedCaps.PlatformLevelResetRecoverySupported == DOT11_TRI_STATE.dot11_tri_state_true))
               {
                   //dont return here as wdi doesnt actually use this to determine what type of RR its going to do (pull power or disable/enable)
                   logger.LogComment("Device Claims it does not support reset recovery, but since the system bus does we shall continue");                   
               }
               try
               {
                   using (var ndisAdapter = new NdisAdapter())
                   {
                       using (var deviceKey = DeviceObject.GetDeviceRegistryKey(wlanInterface.Id, RegistryAccessFlags.KEY_SET_VALUE | RegistryAccessFlags.KEY_READ))
                       {
                           ndisAdapter.AdapterArrival += (sender, args) =>
                           {
                               logger.LogComment("NDIS Arrival {0} {1} {2}", args.AdapterName, args.Adapter, args.AdapterInstance);
                               if (args.Adapter == wlanInterface.Id && useNdisEvent)
                               {
                                   remainingArrivals.Signal();
                               }
                           };
                           ndisAdapter.AdapterRemoval += (sender, args) =>
                           {
                               logger.LogComment("NDIS Removal {0} {1} {2}", args.AdapterName, args.Adapter, args.AdapterInstance);
                               if (args.Adapter == wlanInterface.Id && useNdisEvent)
                               {
                                   remainingRemovals.Signal();
                               }
                           };
                           ndisAdapter.AdapterBind += (sender, args) =>
                           {
                               logger.LogComment("NDIS Bind {0} {1} {2} {3}", args.AdapterName, args.Adapter, args.AdapterInstance, args.Binding);
                           };
                           ndisAdapter.AdapterUnbind += (sender, args) =>
                           {
                               logger.LogComment("NDIS Unbind {0} {1} {2} {3}", args.AdapterName, args.Adapter,
                                   args.AdapterInstance, args.Binding);
                           };
                           ndisAdapter.FilterArrival += (sender, args) =>
                           {
                               logger.LogComment("Filter Arrival {0} {1} {2} {3}", args.AdapterName, args.Adapter, args.AdapterInstance, args.FilterId);
                           };
                           ndisAdapter.FilterRemoval += (sender, args) =>
                           {
                               logger.LogComment("Filter Removal {0} {1} {2} {3}", args.AdapterName, args.Adapter, args.AdapterInstance, args.FilterId);
                           };
                           wlanApi.AdapterArrival += AdapterArrivalHandler;
                           wlanApi.AdapterRemoval += AdapterRemovalHandler;
                           logger.LogComment("Device supports Reset, Configuring for Reset Simulation");

                           var adapterResetEnabled = WaitForAdapterArrival(wlanApi, wlanInterface.Id, () =>
                           {

                               deviceKey.SetDword("HungAtWdiCommandPasses", commandCount);
                               deviceKey.SetDword("HungResetRecoveryIterations", 1);
                               logger.LogComment("Reset Simulation enabled, resetting adapter for test");
                               DeviceObject.ResetDevice(wlanInterface.Id);


                           }, timeout);
                           if (adapterResetEnabled)
                           {
                               logger.LogComment("Adapter Enabled for Reset");
                           }
                                                     
                           logger.LogComment("Initiating activity to trigger simulated hang");
                           int maxActivitiy = 5;
                           for (int i = 0; i < maxActivitiy; i++)
                           {
                               if (remainingRemovals.IsSet)
                               {
                                   logger.LogComment("Recieved adapter removal, checking that reset recovery initiated the removal");
                                   break;
                               }

                               for (int j = 0; j < commandCount; j++)
                               {
                                   try
                                   {
                                       //wlanApi.GetStatistics(wlanInterface);
                                       wlanApi.Scan(wlanInterface.Id, true);
                                   }
                                   catch (COMException ex)
                                   {
                                       unchecked
                                       {
                                           if (ex.HResult != (int)0x80070490 && ex.HResult != (int)0x8007139F)
                                           {
                                               logger.LogComment(ex.ToString());
                                           }
                                       }
                                   }
                                   catch (Exception ex)
                                   {
                                       logger.LogComment(ex.ToString());
                                   }
                               }                               
                               logger.LogComment("Waiting for adapter removal on hang");
                               var adapterRemoved = remainingRemovals.Wait(timeout);
                               if (adapterRemoved)
                               {
                                   logger.LogComment("Recieved adapter removal, checking that reset recovery initiated the removal");
                                   break;
                               }
                               else
                               {                                 
                                   if (i + 1 == maxActivitiy)
                                   {
                                       logger.LogError("Adapter was not removed");
                                       return;
                                   }
                                   else
                                   {
                                       logger.LogComment("Device has not been removed, creating additional activity");
                                   }
                               }
                           }

                           var remainingResets = deviceKey.GetDword("HungResetRecoveryIterations");
                           if (remainingResets == 0)
                           {
                               logger.LogComment("Reset Recovery successfully reset the adapter");
                           }

                           logger.LogComment("Waiting for Wlan device to reenumerate");
                           
                           var adapterAdded = remainingArrivals.Wait(timeout);
                           if (adapterAdded)
                           {
                               logger.LogResult(TestResult.Passed, "Adapter successfully reenumerated");
                           }
                           else
                           {
                               logger.LogResult(TestResult.Failed, "Adapter failed to reenumerate");
                           }
                                                      
                           ndisAdapter.AdapterArrival = null;
                           ndisAdapter.AdapterRemoval = null;
                       }
                   }              
               }
               finally
               {
                   wlanApi.AdapterArrival -= AdapterArrivalHandler;
                   wlanApi.AdapterRemoval -= AdapterRemovalHandler;
                   
                   logger.LogComment("Reseting wireless adapter to remove Reset Recovery simulation");
                   var adapterAdded = WaitForAdapterArrival(wlanApi, wlanInterface.Id, () =>
                                        {
                                            using (var deviceKey = DeviceObject.GetDeviceRegistryKey(wlanInterface.Id, RegistryAccessFlags.KEY_SET_VALUE | RegistryAccessFlags.KEY_READ))
                                            {
                                                deviceKey.DeleteValue("HungResetRecoveryIterations");
                                                deviceKey.DeleteValue("HungAtWdiCommandPasses");
                                                DeviceObject.ResetDevice(wlanInterface.Id);
                                            }
                                        },
                                        timeout);
                   if (!adapterAdded)
                   {
                       logger.LogError("Adapter failed to reenumerate after reset");
                   }

               }


           });

        }
        #endregion

        #region Wlan Test - Query All Reported Oids




        private void Test_14_QueryAllReportedOids()
        {
            RunTest((wlanApi, wlanInterface, logger) =>
           {
               using (var reference = new NicReference(wlanInterface.Id))
               {
                   logger.LogComment("Retrieving supported oids from NDIS. This list is sent during miniport initialize.");
                   var oids = OidQuery.GetSupportedOids(wlanInterface.Id);
                   ValidateOids(wlanApi, wlanInterface, oids, logger);
               }
           });
        }
        #endregion

        #region Wlan L1 test 14 - Connect to WPA2 PSK with MAC randmization On/Off

        /// <summary>
        /// This test requires two parameters WPA2_PSK_AES_SSID and WPA2_PSK_Password.
        /// 
        /// The test will try to scan and connect to the network with MAC randomization ON, then with MAC randomization Off.
        /// Both attempts are expected to succeed. During each attempt, the test verifies that the appropriate
        /// MAC address is set, and that connectivity is properly established. 
        /// </summary>


       
        private void TestCase15_MacRandomizationTest()
        {
            bool strict = true;
            if (TestContext.Properties.Contains("strict"))
            {
                if (!bool.TryParse(TestContext.Properties["strict"] as string, out strict))
                {
                    strict = true;
                }
            }


            RunTest((wlanApi, wlanInterface, logger) =>
            {
                var enhancedCaps = wlanApi.GetWlanEnhancedCapabilites(wlanInterface);
                if (enhancedCaps.MACAddressRandomizationSupported != DOT11_TRI_STATE.dot11_tri_state_true)
                {
                    logger.LogComment("Mac Address Randomization reports not supported {0}", enhancedCaps.MACAddressRandomizationSupported);
                    return;
                }

                logger.LogComment("Testing connectivity with MAC address randomization enabled.");
                ConnectTest(wlanApi, wlanInterface, strict, true, logger, false, true);

                logger.LogComment("Testing connectivity with MAC address randomization disabled.");
                ConnectTest(wlanApi, wlanInterface, strict, true, logger, false, false);
            });
        }

        #endregion

        #region Wlan Test - Hotspot2 ANQP/GAS
        private void Test_16_SendReceiveAnqpGasQuery()
        {
            RunTest((wlanApi, wlanInterface, logger) =>
            {

                bool errorOccured = false;
                int hotspotNetworksCount = 0;
                WlanEnhancedCapabilities enhancedCaps = null;
                try
                {
                    enhancedCaps = wlanApi.GetWlanEnhancedCapabilites(wlanInterface);
                }
                catch (Exception ex)
                {
                    logger.LogError("Get adapter enhanced capabilities failed due to error 0x" + ex.InnerException.HResult.ToString("X", CultureInfo.InvariantCulture));
                    logger.LogResult(TestResult.Failed);
                    return;
                }

                if (enhancedCaps.ANQPServiceInformationDiscoverySupported != DOT11_TRI_STATE.dot11_tri_state_true)
                {
                    logger.LogError("Device is not hotspot2 compliant since ANQP/GAS is not supported");
                    logger.LogResult(TestResult.Failed);
                    return;
                }
                logger.LogComment("Device adapter supports ANQP/GAS");

                logger.LogComment("Scanning for networks");
                try
                {
                    wlanApi.Scan(wlanInterface.Id, true);
                    Wlan.Sleep(4000);
                }
                catch(Exception ex)
                {
                    logger.LogError("Scan for networks failed due to error 0x" + ex.InnerException.HResult.ToString("X", CultureInfo.InvariantCulture));
                    logger.LogResult(TestResult.Failed);
                    return;
                }

                logger.LogComment("Clear Anqp Cache");
                try
                {
                    wlanApi.ClearAnqpCache();
                }
                catch (Exception ex)
                {
                    //This can only fail if there are pending anqp requests. This further means that there is already hotspot2 profile installed
                    // and most likely driver doesn't work well. In order to avoid this case test should be run on device that doesn't have 
                    // any hotspot2 profiles installed.
                    logger.LogError("Clear anqp cache failed due to error 0x" + ex.InnerException.HResult.ToString("X", CultureInfo.InvariantCulture) + ".");
                    logger.LogError("Make sure that device doesn't have any hotspot2.0 profile installed");
                    logger.LogResult(TestResult.Failed);
                    return;
                }

                logger.LogComment("Refresh Anqp Cache");
                System.Collections.ObjectModel.Collection<WlanNetwork> availableNetworkList = wlanApi.GetAvailableNetworkList2(wlanInterface.Id);
                foreach (WlanNetwork availableNetwork in availableNetworkList)
                {
                    if (availableNetwork.IsHotspot2Network())
                    {
                        logger.LogComment("Calling ANQP/GAS query for network = <" + availableNetwork.Ssid + ">");
                        try
                        {
                            wlanApi.RefreshAnqpCache(wlanInterface.Id, availableNetwork.Ssid);
                            Wlan.Sleep(4000);
                            logger.LogComment("RefreshAnqpCache called");
                        }
                        catch (Exception ex)
                        {
                            logger.LogError("Refresh anqp cache failed due to error 0x" + ex.InnerException.HResult.ToString("X", CultureInfo.InvariantCulture));
                            errorOccured = true;
                        }
                    }
                }

                logger.LogComment("Get Anqp Results");
                foreach (WlanNetwork availableNetwork in availableNetworkList)
                {
                    if (availableNetwork.IsHotspot2Network())
                    {
                        hotspotNetworksCount++;
                        logger.LogComment("Get ANQP cache response for network = <" + availableNetwork.Ssid + ">");
                        try
                        {
                            byte[] anqpResultMessage = wlanApi.GetAnqpCacheResponse(wlanInterface.Id, availableNetwork.Ssid);
                            if (anqpResultMessage != null && anqpResultMessage.Length > 0)
                            {
                                String anqpResultMessageStr = String.Empty;
                                for (int index = 0; index < anqpResultMessage.Length; index++)
                                {
                                    anqpResultMessageStr += " 0x" + anqpResultMessage[index].ToString("X", CultureInfo.InvariantCulture);
                                }
                                logger.LogComment("ANQP response=<" + anqpResultMessageStr + ">");
                            }
                            else
                            {
                                logger.LogError("ANQP response is empty");
                                errorOccured = true;
                            }
                        }
                        catch (Exception ex)
                        {
                            logger.LogError("Get anpq cache response failed due to error 0x" + ex.InnerException.HResult.ToString("X", CultureInfo.InvariantCulture));
                            errorOccured = true;
                        }

                    }
                }

                //test is consider failed if unexpected error occured or there is no
                //hotspot2 networks in the range
                if (errorOccured || hotspotNetworksCount == 0)
                {
                    if(hotspotNetworksCount == 0)
                    {
                        logger.LogError("Test failed since there is no hotspot networks in the range");
                    }
                    logger.LogResult(TestResult.Failed);
                }

            });
        }
        #endregion Wlan ANQP/GAS query

        /// <summary>
        /// This test requires three parameters WPA2_DOMAIN_SSID,WPA2_DOMAIN_USERNAME and WPA2_DOMAIN_PASSWORD .
        /// 
        /// The test will simply attempt to connect to the network and expect the connection to succeed.
        /// </summary>
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [TestProperty("Ignore", "True")]
        public void Test_CorporateConnectTest()
        {
            bool strict = true;
            if (TestContext.Properties.Contains("strict"))
            {
                if (!bool.TryParse(TestContext.Properties["strict"] as string, out strict))
                {
                    strict = true;
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
            {
                ConnectTest(wlanApi, wlanInterface, false, false, logger, false);
            });
        }
        /// <summary>
        /// This test requires one parameter OPENSSID.
        /// 
        /// The test will simply attempt to connect to the network and expect the connection to succeed.
        /// </summary>
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [TestProperty("Ignore", "True")]
        public void Test_OpenConnectTest()
        {
            string ssid = "MSFTOPEN";
            bool strict = true;
            if (TestContext != null)
            {
                if (TestContext.Properties.Contains("OPENSSID"))
                {
                    ssid = TestContext.Properties["OPENSSID"] as string;
                }
                if (TestContext.Properties.Contains("strict"))
                {
                    if (!bool.TryParse(TestContext.Properties["strict"] as string, out strict))
                    {
                        strict = true;
                    }
                }
            }
            RunTest((wlanApi, wlanInterface, logger) =>
            {
                Action action = () =>
                {
                    var profileToUse = CreateOpenWlanProfile(ssid, ssid, false);
                    wlanApi.CreateProfile(wlanInterface.Id, profileToUse, Wlan.ProfileScope.AllUser);
                };
                ConnectTest(wlanApi, wlanInterface, logger, action, ssid, ssid, false, strict, true, false, "http://bing.com");
            });
        }

        /// <summary>
        /// This test requires the following parameters        ///  
        ///  SleepDurationInSec
        ///  SleepCycleIterations
        ///  
        /// optional:
        /// url - default to http://bing.com
        /// OPENSSID - default to MSFTOPEN
        /// 
        /// The test will attempt to connect to the open network specified and expect the connection to succeed and verify connectivity it will then put the machine 
        /// in the deepest sleep supported for the SleepDurationInSec time then wake it up.  At this point it will verify connectivity again.  The sleep->resume->check 
        /// cycle will repeat for the SleepCycleIterations specified 
        /// </summary>
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [TestProperty("Ignore", "True")]
        public void Test_OpenConnectWithSleep()
        {   
            if (TestContext != null)
            {
                if (!(TestContext.Properties.Contains("SleepDurationInSec")))
                {                    
                    testLogger.LogResult(TestResult.Failed, "Missing required SleepDurationInSec parameter");
                    return;
                }

                if (!(TestContext.Properties.Contains("SleepCycleIterations")))
                {
                    testLogger.LogResult(TestResult.Failed, "Missing required SleepCycleIterations parameter");
                    return;
                }                
            }

            Test_OpenConnectTest(); //call the regular openconnect test
        }

        // <summary>
        /// This test requires the following parameters        ///  
        ///  SleepDurationInSec
        ///  SleepCycleIterations
        ///  WPA2_DOMAIN_SSID
        ///  WPA2_DOMAIN_USERNAME
        ///  WPA2_DOMAIN_PASSWORD
        ///  WPA2_DOMAIN
        ///  
        /// optional:
        /// url - default to http://bing.com
        /// OPENSSID - default to MSFTOPEN
        /// 
        /// The test will attempt to connect to the corperate network specified and expect the connection to succeed and verify connectivity it will then put the machine 
        /// in the deepest sleep supported for the SleepDurationInSec time then wake it up.  At this point it will verify connectivity again.  The sleep->resume->check 
        /// cycle will repeat for the SleepCycleIterations specified 
        /// </summary>
        [TestMethod]
        [TestProperty("ThreadingModel", "MTA")]
        [TestProperty("Ignore", "True")]
        public void Test_CorperateConnectWithSleep()
        {
            //validate sleep parameters other required params are validated in test.
            if (TestContext != null)
            {
                if (!(TestContext.Properties.Contains("SleepDurationInSec")))
                {
                    testLogger.LogResult(TestResult.Failed, "Missing required SleepDurationInSec parameter");
                    return;
                }

                if (!(TestContext.Properties.Contains("SleepCycleIterations")))
                {
                    testLogger.LogResult(TestResult.Failed, "Missing required SleepCycleIterations parameter");
                    return;
                }      
            }

            Test_CorporateConnectTest(); //call the regular corp connect test test
        }
    }
}
