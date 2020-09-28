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

using AuthServer8021X;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.DeviceManager;
using Microsoft.Test.Networking.Dhcp;
using Microsoft.Test.Networking.Http;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Oids;
using Microsoft.Test.Networking.Power;
using Microsoft.Test.Networking.Tracing;
using Microsoft.Test.Networking.Wireless;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using WEX.Logging.Interop;
using WEX.TestExecution.Markup;


namespace Microsoft.Test.Networking.Wireless.Outerloop
{


    internal struct RadioTime
    {
        public RadioState StartState;
        public RadioState EndState;
        public long StartTime;
        public long EndTime;
        public DateTime StartETWTime;
        public DateTime EndETWTime;
        public bool Sent;
    };
    public enum RadioResetResult
    {
        RadioNotEnabled,
        RadioDoesNotDisable,
        RadioDoesNotReEnable,
        RadioHasBeenReset,
        Unknown // Add new above this line
    }
    public enum L3ConnectivityResults
    {
        NoMatchingDevice,
        NoDHCPandNoGateway,
        PINGConnectivityFailure,
        HTTPConnectivityFailure,
        ConnectionSucceeded,
        UnknownError //Add additional options above this one.
    }

    public class WlanOuterloopTestException : Exception
    {
        public WlanOuterloopTestException(string message)
            : base(message)
        {

        }
    }

    public class ConnectionFailedException : WlanOuterloopTestException
    {
        public WlanReasonCode FailureReason { get; private set; }
        public ConnectionFailedException(string message)
            : base(message)
        {

        }
        public ConnectionFailedException(string message, WlanReasonCode reason)
            : base(message)
        {
            FailureReason = reason;
        }
    }
    public class AssociationFailedException : ConnectionFailedException
    {
        public AssociationFailedException(string message)
            : base(message)
        {

        }
    }

    public class ConnectionTimedOutException : ConnectionFailedException
    {
        public ConnectionTimedOutException(string message)
            : base(message)
        {

        }
    }
    public class AssociationTimedOutException : ConnectionFailedException
    {
        public AssociationTimedOutException(string message)
            : base(message)
        {

        }
    }

    public partial class SystemTests
    {
        private static bool m_RunningOnVNE = false;
        private static WlanHckTestLogger testLogger = new WlanHckTestLogger();
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1026:DefaultParametersShouldNotBeUsed", Justification = "Using CallerMemberName")]

        public void RunTestUnderBatterySimulation(Action<Wlan, WlanInterface, WlanHckTestLogger> action, [CallerMemberName]string testName = "undefined")
        {
            RunTest(action, true, testName);
        }
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1026:DefaultParametersShouldNotBeUsed", Justification = "Using CallerMemberName")]
        public void RunTest(Action<Wlan, WlanInterface, WlanHckTestLogger> action, [CallerMemberName]string testName = "undefined")
        {
            RunTest(action, false, testName);
        }
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1026:DefaultParametersShouldNotBeUsed", Justification = "Using CallerMemberName")]
        public void RunTest(Action<Wlan, WlanInterface, WlanHckTestLogger> action, bool simulateBattery, [CallerMemberName]string testName = "undefined")
        {
            using (WlanHckTestLogger logger = new WlanHckTestLogger())
            {
                logger.LogComment("Entering {0}", testName);
                if (action == null)
                {
                    throw new ArgumentNullException("action");
                }
                if (simulateBattery)
                {
                    SimulateBattery(() => ExecuteTest(action, testName, logger), logger);
                }
                else
                {
                    ExecuteTest(action, testName, logger);
                }
            }
        }
        private void SimulateBattery(Action action, WlanHckTestLogger logger)
        {
            logger.LogComment("Installing simulated battery and unplugging AC");
            PowerManager.InstallSimulatedBattery(true);
            logger.LogComment("Setting simulated battery to 50%");
            PowerManager.SetBatteryLevel(50);
            try
            {
                action();
            }
            finally
            {
                logger.LogComment("Removing Simulated battery and restoring default state");
                PowerManager.UnInstallSimulatedBattery();
            }
        }
        private void ExecuteTest(Action<Wlan, WlanInterface, WlanHckTestLogger> action, string testName, WlanHckTestLogger logger)
        {
            using (var wlanApi = new Wlan())
            {
                var wlanInterface = FindWlanInterface(wlanApi, TestContext, logger);
                if (wlanInterface == null)
                {
                    logger.LogError("No Wlan device was found in the system matching the provided query(s), or installed device is not being managed by the windows wlan service.");
                    logger.LogResult(TestResult.Failed);
                    return;
                }

                DisconnectAndDeleteAllProfiles(wlanApi, wlanInterface, logger);
                EnableRadios(wlanApi, wlanInterface, logger);

                var stopWatch = Stopwatch.StartNew();
                try
                {
                    action(wlanApi, wlanInterface, logger);
                }
                finally
                {
                    stopWatch.Stop();
                    logger.LogComment("Test {0} completed in {1}", testName, stopWatch.Elapsed);
                    var interfaces = wlanApi.EnumWlanInterfaces();
                    if (interfaces.Exists(x => x.Id == wlanInterface.Id))
                    {
                        DisconnectAndDeleteAllProfiles(wlanApi, wlanInterface, logger);
                        EnableRadios(wlanApi, wlanInterface, logger);
                        // Dont check for reset on an xbox run
                        bool xboxRun = false;
                        if (TestContext != null)
                        {
                            if (TestContext.Properties.Contains("xboxRun"))
                            {
                                xboxRun = bool.Parse(TestContext.Properties["xboxRun"].ToString());
                            }
                        }

                        if (!xboxRun)
                        {
                            CheckForResets(wlanApi, wlanInterface, logger);
                        }
                    }
                }
            }
        }
        [TestProperty("ThreadingModel", "MTA")]
        private Collection<WlanNetwork> ScanAndVerfiyNonEmpty(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool strict, bool flushCache = true, bool disableAdapter = true)
        {
            var scanSuccess = RefreshScan(wlanApi, wlanInterface, logger, strict, flushCache, disableAdapter);
            if (!scanSuccess)
            {
                logger.LogResult(TestResult.Failed);
                return new Collection<WlanNetwork>();
            }
            return ValidateScanList(wlanApi, wlanInterface, logger, strict);
        }
        private bool RefreshScan(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool strict, bool flushCache = true, bool disableAdapter = true)
        {

            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }


            EventHandler<WlanInterfaceEventArgs> ScanCompleteHandler = null;
            EventHandler<WlanScanFailureEventArgs> ScanFailHandler = null;
            EventHandler<WlanInterfaceEventArgs> ScanRefreshHandler = null;
            EventHandler<WlanRadioStateEventArgs> RadioChangedHandler = null;
            EventHandler<WlanInterfaceEventArgs> AdapterArrivalHandler = null;

            EventHandler<EventArrivedEventArgs> ETWEvent = null;
            EventHandler<EventArrivedEventArgs> ScanStart = null;
            EventHandler<EventArrivedEventArgs> ScanEnd = null;
            StateTimer etwScanTimer = new StateTimer(logger, "ETW Scan");
            StateTimer serviceScanTimer = new StateTimer(logger, "Service Scan");
            try
            {
                int scanQuietTimeout = 4000;

                IdleDetector scanIdle = new IdleDetector(scanQuietTimeout);

                bool scanTestStarted = false;


                WlanReasonCode scanFailureReason = WlanReasonCode.WLAN_REASON_CODE_SUCCESS;


                ETWEvent = (object sender, EventArrivedEventArgs properties) =>
                {
                    LogEtwEvent(properties, logger);
                };

                ScanStart = (object sender, EventArrivedEventArgs properties) =>
               {
                   Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                   if (AdapterGuid != wlanInterface.Id)
                   {
                       return;
                   }
                   if (scanTestStarted)
                   {
                       etwScanTimer.Start(properties.TimeStamp);
                   }
                   else
                   {
                       logger.LogComment(properties.TimeStamp, "ETW Scan Event seen");
                       scanIdle.Activitiy();
                   }

               };

                ScanEnd = (object sender, EventArrivedEventArgs properties) =>
                {
                    Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                    if (AdapterGuid != wlanInterface.Id)
                    {
                        return;
                    }
                    if (scanTestStarted)
                    {
                        etwScanTimer.Stop(properties.TimeStamp);
                    }
                    else
                    {
                        logger.LogComment(properties.TimeStamp, "ETW Scan Event seen");
                        scanIdle.Activitiy();
                    }

                };




                ScanCompleteHandler = (object sender, WlanInterfaceEventArgs e) =>
                {
                    if (e.InterfaceId != wlanInterface.Id)
                    {
                        return;
                    }

                    if (scanIdle.Activitiy())
                    {
                        logger.LogComment("Scan Complete Event seen");
                    }
                    else
                    {
                        logger.LogTrace("Scan Complete Event seen");
                    }

                };

                ScanFailHandler = (object sender, WlanScanFailureEventArgs e) =>
                {
                    if (e.InterfaceId != wlanInterface.Id)
                    {
                        return;
                    }

                    if (scanIdle.Activitiy())
                    {
                        logger.LogComment("Scan Failure Event seen");
                    }
                    else
                    {
                        logger.LogWarning("There was a scan failure returned by the driver. Error: {0}", e.ReasonCode.ToString());
                        scanFailureReason = e.ReasonCode;
                    }
                    //If the scan failed, we will not get a refresh.
                    if (scanTestStarted)
                    {
                        serviceScanTimer.Stop(DateTime.Now);
                    }

                };

                ScanRefreshHandler = (object sender, WlanInterfaceEventArgs e) =>
                {
                    if (e.InterfaceId != wlanInterface.Id)
                    {
                        return;
                    }
                    if (scanTestStarted)
                    {
                        serviceScanTimer.Stop(DateTime.Now);
                    }
                    if (scanIdle.Activitiy())
                    {
                        logger.LogComment("Scan Refresh Event seen");
                    }
                };

                RadioChangedHandler = (object sender, WlanRadioStateEventArgs e) =>
                {
                    if (e.InterfaceId != wlanInterface.Id)
                    {
                        return;
                    }

                    scanIdle.Activitiy();
                    logger.LogComment("RadioStateChanged Event - Idx:{0}:Sw:{1} Hw:{2}", e.State.PhysicalIndex, e.State.SoftwareRadioState, e.State.HardwareRadioState);
                };

                AdapterArrivalHandler = (object sender, WlanInterfaceEventArgs e) =>
                {
                    //If ANY new  adapter comes in during the backoff time, it must be reset.
                    logger.LogComment("AdapterArrival Event seen on Interface: {0}", e.InterfaceId);
                    scanIdle.Activitiy();
                };

                if (disableAdapter)
                {
                    logger.LogComment("Disabling and Enabling adapter");
                    if (!ResetWirelessDevice(wlanApi, wlanInterface.Id))
                    {
                        logger.LogWarning("There was an error while Disabling and Re-Enabling the wlan device, see above in the log.");
                        return false;
                    }

                    logger.LogComment("Wlan Device at '{0}' has been disabled and re-enabled", wlanInterface.Id);
                }

                logger.LogTrace("Testing for any disabled radios...");
                CheckForDisabledRadios(wlanApi, wlanInterface, logger);

                //Just a warning, if you move this section above reset, you may be recording spurious scan start and complete messages that the OS initiates.
                wlanApi.RadioStateChanged += RadioChangedHandler;
                wlanApi.ScanComplete += ScanCompleteHandler;
                wlanApi.ScanFail += ScanFailHandler;
                wlanApi.ScanRefreshed += ScanRefreshHandler;
                wlanApi.AdapterArrival += AdapterArrivalHandler;
                using (var nicReference = new NicReference(wlanInterface.Id))
                {
                    using (var wlanEtwWatcher = new WlanEtwWatcher())
                    {
                        wlanEtwWatcher.EventArrived += ETWEvent;
                        wlanEtwWatcher.ScanStart += ScanStart;
                        wlanEtwWatcher.ScanEnd += ScanEnd;
                        wlanEtwWatcher.Start();
                        etwScanTimer.Reset();
                        serviceScanTimer.Reset();

                        logger.LogComment("Waiting for {0}ms without radio/scan activity.", scanQuietTimeout);


                        if (!scanIdle.WaitForIdle(60 * 1000))
                        {
                            logger.LogWarning("Waited for more than 60s for wlan events to settle.");
                            return false;
                        }

                        int retries = 5;
                        logger.LogComment("Starting scan test now... (Scan must complete in under {0} seconds)", strict ? "4" : "6");

                        scanTestStarted = true;
                        for (int i = 0; i < retries; i++)
                        {
                            logger.LogComment("Calling Scan, flush cache:{0}", flushCache);
                            serviceScanTimer.Start(DateTime.Now);
                            wlanApi.Scan(wlanInterface.Id, flushCache);
                            if (etwScanTimer.WaitForStart(1000))
                            {
                                break;
                            }

                            logger.LogComment("ETW Scan Start not detected yet. Re-Sending Scan call.");
                            serviceScanTimer.Reset();

                        }
                        if (!etwScanTimer.Started)
                        {
                            logger.LogWarning("Test has been tried {0} times to issue a scan to the driver. No scan event ever arrived.", retries);
                        }
                        var scanTimeout = strict ? 10000 : 12000;

                        if (etwScanTimer.WaitForFinish(scanTimeout))
                        {
                            if (etwScanTimer.Elapsed.Milliseconds > (strict ? 4000 : 6000))
                            {
                                logger.LogWarning("Driver did not return SCAN COMPLETE within {1} seconds!  Elapsed: {0}ms", etwScanTimer.Elapsed.TotalMilliseconds, strict ? "4" : "6");
                                return false;
                            }
                            else
                            {
                                logger.LogComment("ETW Scan completed in {0}ms", etwScanTimer.Elapsed.TotalMilliseconds);
                                if (serviceScanTimer.WaitForFinish(10 * 1000))
                                {
                                    logger.LogComment("Service Scan completed in {0}ms", serviceScanTimer.Elapsed.TotalMilliseconds);
                                }
                                else
                                {
                                    logger.LogWarning("Did not recieve service scan complete");
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            logger.LogComment("Scan operation has timed out after {0}ms", scanTimeout);
                        }



                        if (scanFailureReason != WlanReasonCode.WLAN_REASON_CODE_SUCCESS)
                        {
                            if (strict)
                            {
                                logger.LogWarning("Driver returned a scan failure: {0}", scanFailureReason.ToString());
                                return false;
                            }
                            else
                            {
                                logger.LogWarning("Warning: Driver returned a scan failure: {0}", scanFailureReason.ToString());
                            }
                        }

                        return true;
                    }
                }
            }
            catch (System.Exception ex)
            {
                if (ex is NullReferenceException || ex is System.Runtime.InteropServices.SEHException)
                {
                    throw;
                }
                logger.LogError("Exception hit while scanning: {0}", ex);
                throw;
            }
            finally
            {
                wlanApi.ScanComplete -= ScanCompleteHandler;
                wlanApi.ScanFail -= ScanFailHandler;
                wlanApi.ScanRefreshed -= ScanRefreshHandler;
                wlanApi.RadioStateChanged -= RadioChangedHandler;
                wlanApi.AdapterArrival -= AdapterArrivalHandler;
                etwScanTimer.Dispose();
                serviceScanTimer.Dispose();
            }
        }

        private static Collection<WlanNetwork> ValidateScanList(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool strict)
        {
            logger.LogComment("Calling GetAvailableNetworkList to enumerate scan list.");
            int scanRetries = strict ? 0 : 2;

            Collection<WlanNetwork> lastScanResult = wlanApi.GetAvailableNetworkList(wlanInterface.Id);
            for (int i = 0; i < scanRetries; i++)
            {
                if (lastScanResult.Count != 0)
                {
                    break;
                }
                logger.LogWarning("There are no networks in the scan result. Pausing 500ms.");
                Wlan.Sleep(500);
                lastScanResult = wlanApi.GetAvailableNetworkList(wlanInterface.Id);
            }
            if (lastScanResult.Count == 0)
            {
                logger.LogError("There are no networks in the scan result. Fail.");
                logger.LogResult(TestResult.Failed);
                return lastScanResult;
            }


            logger.LogComment("Scan Results:{0} Networks Seen", lastScanResult.Count);

            foreach (WlanNetwork ssid in lastScanResult)
            {
                LogWlanNetwork(ssid, logger);
            }
            return lastScanResult;
        }
        private bool ValidateNetworkIsInScanList(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool strict, string wpaSsid, bool disableAdapter = true, int attempts = 3)
        {

            for (int i = 0; i < attempts; i++)
            {
                var lastScanResult = ScanAndVerfiyNonEmpty(wlanApi, wlanInterface, logger, strict, false, i == 0 && disableAdapter);
                if (lastScanResult == null || lastScanResult.Count == 0)
                {
                    logger.LogError("Scan Results unavailable. Check logs above.");
                    logger.LogResult(TestResult.Failed);
                    return false;
                }

                logger.LogComment("Searching for {0} in scan results.", wpaSsid);

                if (lastScanResult.Count(ssid => ssid.Ssid.Equals(wpaSsid, StringComparison.Ordinal)) != 0)
                {
                    logger.LogComment("Found {0} in scan results!", wpaSsid);
                    return true;
                }
                logger.LogWarning("{0} was NOT found in the above scan results!", wpaSsid);
                if (strict)
                {
                    logger.LogError("Since strict was enabled, this is a failure.");
                    return false;
                }
                logger.LogWarning("Retrying {0}", i);
            }
            return false;
        }

        private static void LogWlanNetwork(WlanNetwork ssid, WlanHckTestLogger logger)
        {
            logger.LogTrace("Ssid: {0}", ssid.Ssid);
            logger.LogTrace("______NumberOfBssids: {0}", ssid.NumberOfBssids);
            foreach (var bssid in ssid.Bssids)
            {
                logger.LogTrace("_______Id:{0} Signal:{1}%", bssid.Bssid, bssid.LinkQuality);
            }
            logger.LogTrace("______DefaultAuthentication: {0}", ssid.DefaultAuthentication);
            logger.LogTrace("______DefaultCipher: {0}", ssid.DefaultCipher);
        }

        private static void CheckForDisabledRadios(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger)
        {
            try
            {
                List<WlanRadioState> radios = wlanApi.GetRadioState(wlanInterface);

                int RadiosOff = radios.Count(x => { return x.SoftwareRadioState == RadioState.Off; });

                if (RadiosOff != 0)
                {
                    logger.LogWarning("At least one radio found to be off. Calling Disable/Enable function to reset radio state.");
                    RadioResetResult attempt = DisableAndEnableRadios(wlanApi, wlanInterface, logger, false);
                    if (attempt != RadioResetResult.RadioHasBeenReset)
                    {
                        logger.LogWarning("Radios could not be re-enabled. This will probably cause a failure below. Result: {0}", attempt);
                    }
                }
            }
            catch (COMException ex)
            {
                logger.LogError("Exception thrown while getting radio state for adapter {0}:{1}", wlanInterface.Id, ex.HResult);
            }
        }



        //    #region Wlan L1 Helper Functions



        public static bool ResetWirelessDevice(Wlan wlanApi, Guid interfaceGuid)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            return WaitForAdapterArrival(wlanApi, interfaceGuid, () => DeviceObject.ResetDevice(interfaceGuid), 30 * 1000);
        }

        private static bool WaitForAdapterArrival(Wlan wlanApi, Guid interfaceId, Action performAction, int timeout)
        {
            using (var deviceFound = new ManualResetEvent(false))
            {
                EventHandler<WlanInterfaceEventArgs> adapterArrival = (source, args) =>
                {
                    if (args.InterfaceId == interfaceId)
                    {
                        deviceFound.Set();
                    }
                };
                wlanApi.AdapterArrival += adapterArrival;
                try
                {
                    performAction();
                    return deviceFound.WaitOne(timeout);
                }
                finally
                {
                    wlanApi.AdapterArrival -= adapterArrival;
                }
            }
        }
        //private static bool WaitForAdapterRemoval(Wlan wlanApi, Guid interfaceId, Action performAction, int timeout)
        //{
        //    using (var deviceFound = new ManualResetEvent(false))
        //    {
        //        EventHandler<WlanInterfaceEventArgs> adapterRemoval = (source, args) =>
        //        {
        //            if (args.InterfaceId == interfaceId)
        //            {
        //                deviceFound.Set();
        //            }
        //        };
        //        wlanApi.AdapterRemoval += adapterRemoval;
        //        try
        //        {
        //            performAction();
        //            return deviceFound.WaitOne(timeout);
        //        }
        //        finally
        //        {
        //            wlanApi.AdapterRemoval -= adapterRemoval;
        //        }
        //    }
        //}

        public static void DisconnectAndDeleteAllProfiles(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            if (wlanInterface == null)
            {
                throw new ArgumentNullException("wlanInterface");
            }
            logger.LogComment("Disconnecting Wlan Device and deleting all profiles");
            DisconnectInterface(wlanApi, wlanInterface, logger);
            DeleteProfiles(wlanApi, wlanInterface, logger);
        }

        private static void DisconnectInterface(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger)
        {
            Stopwatch disconnectTimer = new Stopwatch();
            using (var disconnectedEvent = new ManualResetEvent(false))
            {
                EventHandler<WlanConnectionEventArgs> HandleDisconnect = (sender, eventArgs) =>
                {
                    if (eventArgs.InterfaceId != wlanInterface.Id)
                    {
                        return;
                    }
                    disconnectTimer.Stop();
                    logger.LogComment("Received disconnect message on adapter: {0}. Time: {1}ms", eventArgs.InterfaceId, disconnectTimer.ElapsedMilliseconds);
                    disconnectedEvent.Set();
                };
                wlanApi.Disconnected += HandleDisconnect;
                try
                {
                    WLAN_INTERFACE_STATE currentState = wlanApi.GetInterfaceState(wlanInterface);

                    if (currentState != WLAN_INTERFACE_STATE.wlan_interface_state_disconnected)
                    {
                        logger.LogComment("Interface {0} is currently connected, Disconnecting {1}", wlanInterface.Id, currentState);
                        disconnectTimer.Start();
                        if (currentState == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                        {
                            wlanApi.Disconnect(wlanInterface.Id);
                        }
                        if (!disconnectedEvent.WaitOne(10000))
                        {
                            disconnectTimer.Stop();
                            logger.LogWarning("Warning. Disconnect on interface {0} took more than {1}ms.", wlanInterface.Id, disconnectTimer.ElapsedMilliseconds);
                        }
                    }
                }
                finally
                {
                    wlanApi.Disconnected -= HandleDisconnect;
                }
            }
        }

        private static void DeleteProfiles(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger)
        {
            logger.LogComment("Deleting all existing profiles on {0}", wlanInterface.Id);
            List<WlanProfileInfo> CurrentProfiles = wlanApi.GetProfileList(wlanInterface.Id);

            foreach (WlanProfileInfo profile in CurrentProfiles)
            {
                logger.LogComment("Deleting \"{0}\" profile ...", profile.ProfileName);
                try
                {
                    wlanApi.DeleteProfile(wlanInterface.Id, profile.ProfileName);
                }
                catch (InvalidOperationException)
                {
                    // logger.LogComment("There is no matching profile on the system.");
                }
                catch (UnauthorizedAccessException)
                {
                    //Ignore GP profile deletion failures.
                }
            }
        }

        internal static RadioResetResult DisableAndEnableRadios(Wlan wlanApi, WlanInterface Interface, WlanHckTestLogger logger, bool strict)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            EnableRadios(wlanApi, Interface, logger);

            AutoResetEvent RadioChanged = new AutoResetEvent(false);
            AutoResetEvent ETWRadioCompleted = new AutoResetEvent(false);

            Stopwatch timer = Stopwatch.StartNew();

            var radioState = wlanApi.GetRadioState(Interface);
            int radioCount = radioState.Count;
            logger.LogComment("There are {0} radios returned for this device [{1}].", radioCount, Interface.Id);

            int radioCounter = 0;
            int nwifiPowerOidCounter = 0;
            RadioTime[] RadioStates = new RadioTime[radioCount];
            bool NextStage = false;
            for (int i = 0; i < radioCount; i++)
            {
                RadioStates[i].StartState = radioState[i].SoftwareRadioState;
                RadioStates[i].EndTime = 0;
                RadioStates[i].StartETWTime = DateTime.MinValue;
                RadioStates[i].EndETWTime = DateTime.MinValue;
                RadioStates[i].Sent = false;
                logger.LogComment("Initial radio State for Radio {2} is Software:{1}, Hardware:{0}", radioState[i].HardwareRadioState, RadioStates[i].StartState, i);
            }

            EventHandler<WlanRadioStateEventArgs> RadioChangeHandler = null;
            EventHandler<EventArrivedEventArgs> ETWCallback = null;
            EventHandler<EventArrivedEventArgs> RadioStart = null;
            EventHandler<EventArrivedEventArgs> RadioChange = null;
            EventHandler<EventArrivedEventArgs> RadioEnd = null;
            #region EventHandlers
            #region Windows 8.1+ ETW Events

            RadioStart = (object sender, EventArrivedEventArgs properties) =>
                {
                    Guid adapterGuid = GetAdapterGuid(wlanApi, properties);
                    if (adapterGuid != Interface.Id)
                    {
                        logger.LogTrace(properties.TimeStamp, "Recieved Radio Change event for non-test adapter {0}", adapterGuid);
                        return;
                    }

                    bool CSResult = false;
                    int CurrentCounter = radioCounter;
                    try
                    {
                        logger.LogTrace(properties.TimeStamp, "ETW START on Radio {0}", CurrentCounter);

                        CSResult = Monitor.TryEnter(RadioStates);
                        if (!CSResult)
                        {
                            logger.LogTrace(properties.TimeStamp, "Warning, radio operation on radio {0} has not yet completed. Will still store state and time.", CurrentCounter);
                        }

                        if (RadioStates[CurrentCounter].Sent == false)
                        {
                            logger.LogTrace(properties.TimeStamp, "Warning, Radio {0} has had an ETW start event, but test code did NOT send start operation. will NOT save time.", CurrentCounter);
                            return;
                        }

                        if (RadioStates[CurrentCounter].StartETWTime != DateTime.MinValue)
                        {
                            logger.LogTrace(properties.TimeStamp, "Warning, Radio {0} has been set by ETW already. Double notification recieved. Will not overwrite.", CurrentCounter);
                            return;
                        }
                        RadioStates[CurrentCounter].StartETWTime = properties.TimeStamp;


                        logger.LogComment(properties.TimeStamp, "ETW Radio change START on radio {0} detected at {1:HH:mm:ss.fff}", CurrentCounter, RadioStates[CurrentCounter].StartETWTime);
                        return;
                    }
                    finally
                    {
                        if (CSResult)
                        {
                            Monitor.Exit(RadioStates);
                        }
                    }

                };
            RadioChange = (object sender, EventArrivedEventArgs properties) =>
            {
                Guid adapterGuid = GetAdapterGuid(wlanApi, properties);
                if (adapterGuid != Interface.Id)
                {
                    logger.LogTrace(properties.TimeStamp, "Recieved Radio Change event for non-test adapter {0}", adapterGuid);
                    return;
                }
                //logger.LogComment(properties.TimeStamp, properties.EventMessage);
                bool CSResult = false;
                try
                {
                    string phyString = properties.Properties["PHY"].ToString();
                    int phy = int.Parse(phyString, CultureInfo.InvariantCulture);
                    //int phy = (int)properties.Properties["PHY"];
                    logger.LogTrace(properties.TimeStamp, "ETW CHANGE on Radio {0}", phy);

                    CSResult = Monitor.TryEnter(RadioStates);
                    if (!CSResult)
                    {
                        logger.LogTrace(properties.TimeStamp, "Warning, currently performing radio action on radio {0} while an ETW notification for radio {1} has arrived. Recording time/state for radio {1},", radioCounter, phy);
                    }

                    if (RadioStates[phy].Sent == true)
                    {
                        logger.LogTrace(properties.TimeStamp, "ETW Change event for radio {0} is being ignored because, this was a request. A START and COMPLETE are required.", phy);
                        return;
                    }

                    RadioStates[phy].EndETWTime = properties.TimeStamp;

                    string eventMessage = properties.EventMessage;
                    string state = properties.Properties["SoftwareState"].ToString();

                    logger.LogComment(properties.TimeStamp, "ETW Radio change on radio {0} detected as {2} at {1:HH:mm:ss.fff}", phy, properties.TimeStamp, state);

                    return;
                }
                finally
                {
                    if (CSResult)
                    {
                        Monitor.Exit(RadioStates);
                    }
                }
            };
            RadioEnd = (object sender, EventArrivedEventArgs properties) =>
                {
                    Guid adapterGuid = GetAdapterGuid(wlanApi, properties);
                    if (adapterGuid != Interface.Id)
                    {
                        logger.LogTrace(properties.TimeStamp, "Recieved Radio Change event for non-test adapter {0}", adapterGuid);
                        return;
                    }
                    bool CSResult = false;
                    try
                    {
                        logger.LogTrace(properties.TimeStamp, "ETW COMPLETE on Radio {0}", nwifiPowerOidCounter);

                        CSResult = Monitor.TryEnter(RadioStates);
                        if (!CSResult)
                        {
                            logger.LogTrace(properties.TimeStamp, "Warning, currently performing radio action on radio {0} while an ETW notification for radio {1} has arrived. Recording time/state for radio {1},", radioCounter, nwifiPowerOidCounter);
                        }

                        if (nwifiPowerOidCounter >= radioCount)
                        {
                            logger.LogTrace(properties.TimeStamp, "Counter advanced past radio count of {0}. Will NOT save time.", radioCount);
                            return;
                        };

                        if (RadioStates[nwifiPowerOidCounter].EndETWTime != DateTime.MinValue)
                        {
                            logger.LogTrace(properties.TimeStamp, "Warning, Radio {0} has been set by ETW already. Double notification recieved. Will not overwrite.", nwifiPowerOidCounter);
                            return;
                        }

                        RadioStates[nwifiPowerOidCounter].EndETWTime = properties.TimeStamp;


                        logger.LogComment(properties.TimeStamp, "ETW Radio change COMPLETE on radio {0} detected at {1:HH:mm:ss.fff}", nwifiPowerOidCounter, RadioStates[nwifiPowerOidCounter].EndETWTime);

                        ETWRadioCompleted.Set();
                        return;
                    }
                    finally
                    {
                        if (CSResult)
                        {
                            Monitor.Exit(RadioStates);
                        }
                    }
                };
            ETWCallback = (object sender, EventArrivedEventArgs properties) =>
            {
                LogEtwEvent(properties, logger);

            };



            #endregion
            RadioChangeHandler = (sender, eventArgs) =>
            {
                if (eventArgs.InterfaceId != Interface.Id)
                {
                    //This is not the adapter you are looking for
                    logger.LogTrace("Recieved Radio Change event for non-test adapter {0}", eventArgs.InterfaceId);
                    return;
                }
                bool fireevent = false;
                long eventarrival = timer.ElapsedMilliseconds;
                if (RadioStates.Length > eventArgs.State.PhysicalIndex)
                {
                    if (RadioStates[eventArgs.State.PhysicalIndex].EndTime != 0)
                    {
                        logger.LogWarning("Warning: Radio at index {0} has issued a double change notification!", eventArgs.State.PhysicalIndex);
                    }
                    else
                    {
                        bool CSResult = false;
                        try
                        {
                            CSResult = Monitor.TryEnter(RadioStates);
                            if (!CSResult)
                            {
                                logger.LogTrace("Warning, currently performing radio action on radio {0} while a notification for radio {1} has arrived. Recording time/state for radio {1},", radioCounter, eventArgs.State.PhysicalIndex);
                            }

                            RadioStates[eventArgs.State.PhysicalIndex].EndTime = eventarrival;
                            RadioStates[eventArgs.State.PhysicalIndex].EndState = eventArgs.State.SoftwareRadioState;
                            fireevent = true;
                        }
                        finally
                        {
                            if (CSResult)
                            {
                                Monitor.Exit(RadioStates);
                            }
                        }
                    }
                }
                else
                {
                    logger.LogWarning("Warning: Radio change on index greater than specified radios! Change index: {0}, Max Radios: {1}", eventArgs.State.PhysicalIndex, RadioStates.Length);
                }

                logger.LogComment("Radio State has changed on Radio {0} - Hardware: {1}, Software:{2}", eventArgs.State.PhysicalIndex, eventArgs.State.HardwareRadioState, eventArgs.State.SoftwareRadioState);
                if (fireevent)
                {
                    RadioChanged.Set();
                }
            };
            #endregion
            try
            {

                logger.LogTrace("Setting up ETW listening.");
                using (var wlanEtwWatcher = new WlanEtwWatcher())
                {
                    wlanEtwWatcher.EventArrived += ETWCallback;
                    wlanEtwWatcher.RadioStart += RadioStart;
                    wlanEtwWatcher.RadioChange += RadioChange;
                    wlanEtwWatcher.RadioEnd += RadioEnd;
                    wlanEtwWatcher.Start();


                    wlanApi.RadioStateChanged += RadioChangeHandler;

                    logger.LogComment("Disabling Radios - Waiting 20s for change to appear.");

                    radioCounter = 0;
                    timer.Restart();
                    while (!NextStage && (timer.ElapsedMilliseconds <= 20000))
                    {

                        if (radioCounter < radioCount)
                        {
                            bool CheckForTimeout = false;
                            lock (RadioStates)
                            {
                                if (RadioStates[radioCounter].EndTime != 0)
                                {
                                    logger.LogComment("Radio {0} already off, skipping.", radioCounter);
                                }
                                else if (RadioStates[radioCounter].StartState == RadioState.Off)
                                {
                                    logger.LogWarning("Warning: Radio at index {0} is already off. Skipping.", radioCounter);
                                    RadioStates[radioCounter].EndTime = RadioStates[radioCounter].StartTime = timer.ElapsedMilliseconds;
                                }
                                else
                                {
                                    logger.LogComment("Turning off Radio {0}", radioCounter);
                                    RadioStates[radioCounter].StartTime = timer.ElapsedMilliseconds;
                                    RadioStates[radioCounter].Sent = true;
                                    nwifiPowerOidCounter = radioCounter;
                                    wlanApi.SetRadioState(Interface, radioCounter, false);
                                    CheckForTimeout = true;
                                }
                            }

                            if (CheckForTimeout)
                            {

                                CheckForTimeout = ETWRadioCompleted.WaitOne(20000);
                                if (!CheckForTimeout)
                                {
                                    logger.LogError("Could not see COMPLETE ETW event on index {0} in under 20 seconds. Re-Enabling ALL radios. This will cause a failure.", radioCounter);
                                    for (int i = 0; i < radioCount; i++)
                                    {
                                        wlanApi.SetRadioState(Interface, i, true);
                                    }
                                    return RadioResetResult.RadioDoesNotDisable;
                                }

                                //Timing Enhancement. Some devices return radio status on each phy quickly
                                //This allows those events to come in.
                                Wlan.Sleep(75);
                            }
                        }

                        NextStage = true;
                        for (int i = 0; i < radioCount; i++)
                        {
                            if (RadioStates[i].EndState != RadioState.Off)
                            {
                                NextStage = false;
                            }
                        }
                        radioCounter++;
                    }
                    timer.Stop();
                    if (NextStage) logger.LogComment("All Radios now report off. Elapsed: {0}ms", timer.ElapsedMilliseconds);

                    if (timer.ElapsedMilliseconds > 20000)
                    {
                        logger.LogError("Could not change all Radio states in under 20 seconds. Re-Enabling all radios. This will cause a failure. Elapsed: {0}ms", timer.ElapsedMilliseconds);
                        for (int i = 0; i < radioCount; i++)
                        {
                            wlanApi.SetRadioState(Interface, i, true);
                        }
                        return RadioResetResult.RadioDoesNotDisable;
                    }



                    bool AllETWEventsPresent = false;
                    logger.LogTrace("Waiting for all Start and Complete ETW events.");
                    var ETWTimeout = DateTime.Now.AddMilliseconds(20000 - timer.ElapsedMilliseconds);
                    while (!AllETWEventsPresent && (ETWTimeout >= DateTime.Now))
                    {
                        AllETWEventsPresent = AllEventsPresent(radioCount, RadioStates, AllETWEventsPresent);


                        if (ETWTimeout < DateTime.Now)
                        {
                            logger.LogError("Radio ETW event not recieved in les than 20 seconds. Please re-run test. Re-Enabling all radios. This will cause a failure. Elapsed: {0}ms", timer.ElapsedMilliseconds);
                            for (int i = 0; i < radioCount; i++)
                            {
                                wlanApi.SetRadioState(Interface, i, true);
                            }
                            return RadioResetResult.RadioDoesNotDisable;
                        }
                        logger.LogTrace("All ETW events present.");
                    }

                    for (int i = 0; i < radioCount; i++)
                    {

                        long serviceTime = RadioStates[i].EndTime - RadioStates[i].StartTime;
                        double totalTime = serviceTime;


                        if (((i + 1) < radioCount) && (RadioStates[i + 1].StartETWTime == DateTime.MinValue))
                        {
                            RadioStates[i + 1].StartETWTime = RadioStates[i].StartETWTime;
                        }
                        totalTime = (RadioStates[i].EndETWTime - RadioStates[i].StartETWTime).TotalMilliseconds;


                        if ((strict) && ((totalTime) > 2500))
                        {
                            logger.LogError("Error: Radio {0} took longer than 2500ms to disable [Actual: {1}ms]. Wlan driver requirements state each radio must change state in less than 2000ms.", i, totalTime);
                        }
                        else if (totalTime > 2000)
                        {
                            logger.LogWarning("Warning: Radio {0} took longer than 2000ms to disable [Actual: {1}ms]. Wlan driver requirements state each radio must change state in less than 2000ms. This will fail the L2 strict test.", i, totalTime);
                        }
                        else
                        {
                            logger.LogComment("Radio {0} was disabled in {1}ms (ETW Time) and {2}ms (Service Event Time)", i, totalTime, serviceTime);
                        }

                        RadioStates[i].StartState = RadioState.Off;
                        RadioStates[i].EndState = RadioState.Unknown;
                        RadioStates[i].StartTime = 0;
                        RadioStates[i].EndTime = 0;
                        RadioStates[i].Sent = false;

                        RadioStates[i].StartETWTime = DateTime.MinValue;
                        RadioStates[i].EndETWTime = DateTime.MinValue;

                    }

                    logger.LogComment("Enabling Radios - Waiting 20s for change to appear.");

                    radioCounter = 0;
                    nwifiPowerOidCounter = 0;
                    NextStage = false;
                    timer.Restart();
                    while (!NextStage && (timer.ElapsedMilliseconds <= 20000))
                    {
                        if (radioCounter < radioCount)
                        {
                            bool CheckForTimeout = false;
                            lock (RadioStates)
                            {
                                if (RadioStates[radioCounter].EndTime != 0)
                                {
                                    logger.LogComment("Radio {0} already on, skipping.", radioCounter);
                                }
                                else if (RadioStates[radioCounter].StartState == RadioState.On)
                                {
                                    logger.LogWarning("Warning: Radio at index {0} is already on. Skipping. This is unexpected.", radioCounter);
                                    RadioStates[radioCounter].EndTime = RadioStates[radioCounter].StartTime = timer.ElapsedMilliseconds;
                                }
                                else
                                {
                                    logger.LogComment("Turning on Radio {0}", radioCounter);
                                    RadioStates[radioCounter].StartTime = timer.ElapsedMilliseconds;
                                    RadioStates[radioCounter].Sent = true;
                                    nwifiPowerOidCounter = radioCounter;
                                    wlanApi.SetRadioState(Interface, radioCounter, true);
                                    CheckForTimeout = true;
                                }
                            }

                            if (CheckForTimeout)
                            {

                                CheckForTimeout = ETWRadioCompleted.WaitOne(20000);
                                if (!CheckForTimeout)
                                {
                                    logger.LogError("Could not see ETW Start event on index {0} in under 20 seconds. Re-Enabling ALL radios. This will cause a failure.", radioCounter);
                                    for (int i = 0; i < radioCount; i++)
                                    {
                                        wlanApi.SetRadioState(Interface, i, true);
                                    }
                                    return RadioResetResult.RadioDoesNotReEnable;
                                }

                                //Timing Enhancement. Some devices return radio status on each phy quickly
                                //This allows those events to come in.
                                Wlan.Sleep(75);
                            }
                        }

                        NextStage = true;
                        for (int i = 0; i < radioCount; i++)
                        {
                            if (RadioStates[i].EndState != RadioState.On)
                            {
                                NextStage = false;
                            }
                        }
                        radioCounter++;
                    }
                    timer.Stop();
                    if (NextStage) logger.LogComment("All Radios now report on. Elapsed: {0}ms", timer.ElapsedMilliseconds);

                    if (timer.ElapsedMilliseconds > 20000)
                    {
                        logger.LogError("Could not change Radio state in under 20 seconds. Re-Enabling All Radios. This will cause a failure. Elapsed: {0}ms", timer.ElapsedMilliseconds);
                        for (int i = 0; i < radioCount; i++)
                        {
                            wlanApi.SetRadioState(Interface, i, true);
                        }
                        return RadioResetResult.RadioDoesNotReEnable;
                    }


                    logger.LogTrace("Waiting for ETW Events to complete.");
                    AllETWEventsPresent = false;
                    ETWTimeout = DateTime.Now.AddMilliseconds(20000 - timer.ElapsedMilliseconds);
                    while (!AllETWEventsPresent && (ETWTimeout >= DateTime.Now))
                    {
                        AllETWEventsPresent = AllEventsPresent(radioCount, RadioStates, AllETWEventsPresent);


                        if (ETWTimeout < DateTime.Now)
                        {
                            logger.LogError("At least one radio ETW event not recieved in les than 20 seconds. Re-Enabling all radios. This will cause a failure. Elapsed: {0}ms", timer.ElapsedMilliseconds);
                            for (int i = 0; i < radioCount; i++)
                            {
                                wlanApi.SetRadioState(Interface, i, true);
                            }
                            return RadioResetResult.RadioDoesNotReEnable;
                        }
                        logger.LogTrace("ETW Events all completed");
                    }

                    radioState = wlanApi.GetRadioState(Interface);
                    radioCount = radioState.Count;
                    for (int i = 0; i < radioCount; i++)
                    {
                        if (radioState[i].SoftwareRadioState == RadioState.Off)
                        {
                            logger.LogError("Error found during confirmation. Radio {0} is still off. Re-Enabling All Radios. This will cause a failure.", i);
                            for (int j = 0; j < radioCount; j++)
                            {
                                wlanApi.SetRadioState(Interface, j, true);
                            }
                            return RadioResetResult.RadioDoesNotReEnable;
                        }

                        long serviceTime = RadioStates[i].EndTime - RadioStates[i].StartTime;
                        double totalTime = serviceTime;


                        if (((i + 1) < radioCount) && (RadioStates[i + 1].StartETWTime == DateTime.MinValue))
                        {
                            RadioStates[i + 1].StartETWTime = RadioStates[i].StartETWTime;
                        }
                        totalTime = (RadioStates[i].EndETWTime - RadioStates[i].StartETWTime).TotalMilliseconds;

                        if ((strict) && ((totalTime) > 2500))
                        {
                            logger.LogError("Error: Radio {0} took longer than 2500ms to enable [Actual: {1}ms]. Wlan driver requirements state each radio must change state in less than 2000ms.", i, totalTime);
                        }
                        else if (totalTime > 2000)
                        {
                            logger.LogWarning("Warning: Radio {0} took longer than 2000ms to enable [Actual: {1}ms]. Wlan driver requirements state each radio must change state in less than 2000ms. This will fail the L2 strict test.", i, totalTime);
                        }
                        else
                        {

                            logger.LogComment("Radio {0} enabled in {1}ms (ETW Time) and {2}ms (Service Event Time)", i, totalTime, serviceTime);

                        }
                    }

                    logger.LogComment("Confirmed through GetRadioState that all radios are on.");

                    return RadioResetResult.RadioHasBeenReset;
                }
            }
            catch (System.Exception ex)
            {
                logger.LogError("Error found during Airplane mode transition. This will cause a failure. Attempting to re-enable all radios. If that fails, device state will be unknown. Exception: {0}", ex.ToString());
                for (int j = 0; j < radioCount; j++)
                {
                    wlanApi.SetRadioState(Interface, j, true);
                }
                throw;
            }
            finally
            {
                wlanApi.RadioStateChanged -= RadioChangeHandler;
            }
        }

        private static bool AllEventsPresent(int radioCount, RadioTime[] RadioStates, bool AllETWEventsPresent)
        {
            AllETWEventsPresent = true;
            for (int i = 0; i < radioCount; i++)
            {
                if (RadioStates[i].Sent && (RadioStates[i].StartETWTime == DateTime.MinValue))
                {
                    AllETWEventsPresent = false;
                    Wlan.Sleep(100);
                    break;
                }
                if (RadioStates[i].EndETWTime == DateTime.MinValue)
                {
                    AllETWEventsPresent = false;
                    Wlan.Sleep(100);
                    break;
                }
            }
            return AllETWEventsPresent;
        }

        private static void EnableRadios(Wlan wlanApi, WlanInterface Interface, WlanHckTestLogger logger)
        {
            logger.LogComment("Checking for disabled radios");
            var radioState = wlanApi.GetRadioState(Interface);
            for (int i = 0; i < radioState.Count; i++)
            {
                var radioEnabled = false;
                foreach (var radio in radioState)
                {
                    if (radio.SoftwareRadioState == RadioState.Off)
                    {
                        logger.LogComment("Radio {0} was reported as off, attempting to enable", radio.PhysicalIndex);
                        var enabled = EnableRadioIfNecessary(wlanApi, Interface, logger, radio);
                        if (!enabled)
                        {
                            logger.LogWarning("Radio {0} did not enable within 10 seconds", radio.PhysicalIndex);
                        }
                        else
                        {
                            logger.LogComment("Radio {0} was successfully enabled", radio.PhysicalIndex);
                        }
                        radioEnabled = true;
                        break;
                    }
                }
                if (!radioEnabled)
                {
                    radioState = wlanApi.GetRadioState(Interface);
                    var allOn = radioState.TrueForAll(x => x.SoftwareRadioState == RadioState.On);
                    if (!allOn)
                    {

                        foreach (var radio in radioState.Where(x => x.SoftwareRadioState != RadioState.On))
                        {
                            logger.LogWarning("Radio {0} does not report as On", radio.PhysicalIndex);
                        }
                    }
                    return;
                }
                radioState = wlanApi.GetRadioState(Interface);
            }
        }

        private static bool EnableRadioIfNecessary(Wlan wlanApi, WlanInterface Interface, WlanHckTestLogger logger, WlanRadioState radio)
        {
            using (var resetEvent = new ManualResetEvent(false))
            {
                EventHandler<WlanRadioStateEventArgs> radioStateHandler = (sender, eventArgs) =>
                {
                    if (eventArgs.State.PhysicalIndex == radio.PhysicalIndex && eventArgs.State.SoftwareRadioState == RadioState.On)
                    {
                        resetEvent.Set();
                    }
                };
                wlanApi.RadioStateChanged += radioStateHandler;
                try
                {
                    wlanApi.SetRadioState(Interface, radio.PhysicalIndex, true);
                    if (!resetEvent.WaitOne(10 * 1000))
                    {
                        return false;
                    }
                    return true;
                }
                finally
                {
                    wlanApi.RadioStateChanged -= radioStateHandler;
                }
            }
        }

        private static void LogEtwEvent(EventArrivedEventArgs args, WlanHckTestLogger logger)
        {

            logger.LogTrace(args.TimeStamp, "ETW: {2:HH:mm:ss.fff} - {0} - {1}", args.ProviderName, args.EventMessage, args.TimeStamp);

        }

        private static Guid GetAdapterGuid(Wlan wlanApi, EventArrivedEventArgs properties)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            Guid adapterGuid = Guid.Empty;
            if (properties.Properties.ContainsKey("AdapterGuid"))
            {
                adapterGuid = Guid.Parse(properties.Properties["AdapterGuid"].ToString());
            }
            if (properties.Properties.ContainsKey("InterfaceGuid"))
            {
                adapterGuid = Guid.Parse(properties.Properties["InterfaceGuid"].ToString());
            }
            return adapterGuid;
        }
        private static WlanInterface FindWlanInterface(Wlan wlanApi, TestContext context, WlanHckTestLogger logger)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            if (context == null)
            {
                throw new ArgumentNullException("context");
            }
            var testProperties = context.Properties;
            string WlanDeviceID = null;
            if (testProperties.Contains("DQ") && ((string)testProperties["DQ"] != string.Format(CultureInfo.InvariantCulture, "DeviceID='{0}'", (string)testProperties["queryTestDeviceID"])))
            {
                //logger.LogComment("DQ parameter is not equal to queryTestDeviceID. Test is assuming this is being called from the WDK and will us the DQ parameter overriding queryTestDeviceID");

                //WlanDeviceID = GetDeviceFromWdtf(Properties);

                //if (WlanDeviceID == null)
                //{
                //    logger.LogError( "There was an error finding the device with the given DQ parameter!");
                //    throw new KeyNotFoundException("There was an error finding the device with the given DQ parameter!");
                //}

                //PORTME:
                throw new NotImplementedException();
            }
            else if (testProperties.Contains("queryTestDeviceID") && !String.IsNullOrEmpty(testProperties["queryTestDeviceID"] as string) && ("//Devnode/DeviceID" != testProperties["queryTestDeviceID"] as string))
            {
                //if (!testProperties.Contains("queryTestDeviceID"))
                //{
                //    logger.LogError( "There was no DQ or queryTestDeviceID provided!");
                //    throw new KeyNotFoundException("There was no DQ or queryTestDeviceID provided!");
                //}

                logger.LogComment("Using queryTestDeviceID: {0}", (string)testProperties["queryTestDeviceID"]);
                WlanDeviceID = (string)testProperties["queryTestDeviceID"];
            }
            else
            {

                logger.LogComment("No adapter specified, using first Wlan Adapter");
                var adapters = wlanApi.EnumWlanInterfaces();
                if (adapters.Count > 0)
                {
                    var adapter = adapters[0];
                    logger.LogComment("Using adapter {0}:{1}", adapter.Id, adapter.Description);
                    return adapter;
                }
                else
                {
                    throw new KeyNotFoundException("No adapter found!");
                }
            }

            return GetWlanInterfaceByDeviceId(wlanApi, WlanDeviceID, logger);
        }

        private static WlanInterface GetWlanInterfaceByDeviceId(Wlan wlanApi, string WlanDeviceID, WlanHckTestLogger logger)
        {
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            logger.LogComment("Checking Machine Wlan capability for Wlan Device ID: {0}", WlanDeviceID);

            try
            {
                var interfaceId = DeviceObject.GetNetworkIdForPnPId(WlanDeviceID);
                logger.LogComment("Enumerating Wlan Devices... Looking for {0}", interfaceId);
                int retries = 3;
                WlanInterface wlanInterface = GetWlanInterface(wlanApi, interfaceId, retries, logger);
                if (wlanInterface == null)
                {
                    throw new Exception("Ran out of retries while attempting to find the target wlan interface. Check the logs above.");
                }
                logger.LogComment("Found matching Wlan Adapter ({0}) . Querying whether it is enabled for autoconfig.", wlanInterface.Id);

                if (!wlanApi.IsAutoConfigEnabled(wlanInterface))
                {
                    logger.LogError("Windows Autoconfig is not configured to manage {0}. Check that Wireless optional component is installed and that there is no other wireless connection manager installed on the machine.", wlanInterface.Id);
                    throw new Exception(string.Format(CultureInfo.InvariantCulture, "Windows Autoconfig is not configured to manage {0}. Check that Wireless optional component is installed and that there is no other wireless connection manager installed on the machine.", wlanInterface.Id));
                }
                else
                {
                    logger.LogComment("Autoconfig reports it is ready and configured for {0}. Tests will continue.", wlanInterface.Id);
                    return wlanInterface;
                }

            }
            catch (System.Exception ex)
            {
                logger.LogError("Exception hit while trying to find Wlan Interface. If exception says 'Wlan Service not Running' check that the device you selected is a wireless lan device and check for any errors in Device Manager on the machine. Exception: {0}", ex);
                throw;
            }
        }

        private static WlanInterface GetWlanInterface(Wlan wlanApi, Guid interfaceId, int retries, WlanHckTestLogger logger)
        {
            WlanInterface wlanInterface = null;
            using (var deviceFound = new ManualResetEvent(false))
            {
                EventHandler<WlanInterfaceEventArgs> adapterArrival = (source, args) =>
                {
                    logger.LogComment("Device Arrived {0}", args.InterfaceId);
                    if (args.InterfaceId == interfaceId) { deviceFound.Set(); }
                };
                wlanApi.AdapterArrival += adapterArrival;
                try
                {
                    for (int i = 0; i < retries; i++)
                    {
                        List<WlanInterface> WlanInterfaces = wlanApi.EnumWlanInterfaces();
                        wlanInterface = WlanInterfaces.Find(x => { return x.Id == interfaceId; });
                        if (wlanInterface != null)
                        {
                            break;
                        }
                        EnableDevice(interfaceId, deviceFound, logger);
                    }
                }
                finally
                {
                    wlanApi.AdapterArrival -= adapterArrival;
                }
            }
            return wlanInterface;
        }

        private static void EnableDevice(Guid interfaceId, ManualResetEvent deviceFound, WlanHckTestLogger logger)
        {
            logger.LogComment("There were no matching wlan devices found. Possibly disabled from a previous test? Will attempt to enable and find device again.");
            try
            {
                DeviceObject.EnableDevice(interfaceId);
                logger.LogComment("Device Enabled, waiting for wlan device arrival");
                if (!deviceFound.WaitOne(30 * 1000))
                {
                    logger.LogComment("Device did not arrive in 30 seconds");
                }
            }
            catch (NullReferenceException)
            { throw; }

            catch (Exception ex)
            {
                logger.LogWarning("Exception during enable: {0}", ex.ToString());
            }
        }

        private void HostedNetworkTest(Wlan wlanApi, WlanHckTestLogger logger)
        {
            logger.LogComment("Entering HostedNetwork Test");
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }

            try
            {
                logger.LogComment("Starting HostedNetwork");
                WLAN_HOSTED_NETWORK_REASON hostedNetworkReason;
                hostedNetworkReason = wlanApi.StartHostedNetwork();
                if (hostedNetworkReason != WLAN_HOSTED_NETWORK_REASON.wlan_hosted_network_reason_success)
                {
                    logger.LogError("StartHostedNetwork Failed with: {0}", hostedNetworkReason);
                    return;
                }
                logger.LogComment("Hosted Network Started: {0}", hostedNetworkReason);

                logger.LogComment("Checking HostedNetwork Status");
                WlanHostedNetworkStatus hostedNetworkStatus;
                hostedNetworkStatus = wlanApi.GetHostedNetworkStatus();
                if (hostedNetworkStatus.HostedNetworkState == WLAN_HOSTED_NETWORK_STATE.wlan_hosted_network_unavailable)
                {
                    logger.LogError("HostedNetwork state is unavailable after starting");
                    return;
                }

                logger.LogComment("Stopping HostedNetwork");
                hostedNetworkReason = wlanApi.StopHostedNetwork();
                if (hostedNetworkReason != WLAN_HOSTED_NETWORK_REASON.wlan_hosted_network_reason_success)
                {
                    logger.LogError("StopHostedNetwork Failed with: {0}", hostedNetworkReason);
                    return;
                }
                logger.LogComment("Hosted Network Stopped: {0}", hostedNetworkReason);
            }
            catch (System.Exception ex)
            {
                if (ex is NullReferenceException || ex is System.Runtime.InteropServices.SEHException)
                {
                    throw;
                }
                logger.LogError("Exception hit while running HostedNetwork test: {0}", ex.Message);
            }

        }
        private void ConnectTest(Wlan wlanApi, WlanInterface wlanInterface, bool strict, bool psk, WlanHckTestLogger logger, bool autoconnect, bool validateMacRandomization = false, string url = null)
        {
            string ssid;
            string password;
            string username;
            string domain;
            string[] rootCAs;
            if (!ValidateConnectParameters(psk, logger, out ssid, out password, out username, out domain, out rootCAs))
            {
                return;
            }
            string profileName = ssid;

            Action addProfile = () =>
            {
                logger.LogComment("Adding profile \"{0}\" ...", profileName);
                CreateProfile(wlanApi, wlanInterface, logger, profileName, ssid, password, psk, domain, username, rootCAs, autoconnect);
            };
            ConnectTest(wlanApi, wlanInterface, logger, addProfile, ssid, profileName, autoconnect, strict, psk, validateMacRandomization, url);
        }
        [TestProperty("ThreadingModel", "MTA")]
        private void ConnectTest(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, Action addProfile, string ssid, string profileName, bool autoconnect, bool strict, bool checkHttp, bool validateMacRandomization, string url = null)
        {
            logger.LogComment("Entering ConnectTest, Strict == {0}", strict);
            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            bool initialRandomizationState = false;
            try
            {
                bool xboxRun = false;
                bool doSleepTest = false;
                int sleepDurationInSec = 0;
                int sleepCycleIterations = 0;
                if (TestContext != null) //extract parameters
                {
                    if ((TestContext.Properties.Contains("xboxrun")))
                    {
                        xboxRun = bool.Parse(TestContext.Properties["xboxrun"].ToString());
                    }
                    if (TestContext.Properties.Contains("SleepDurationInSec"))
                    {
                        sleepDurationInSec = Int32.Parse(TestContext.Properties["SleepDurationInSec"].ToString(), CultureInfo.InvariantCulture.NumberFormat);  

                    }

                    if (TestContext.Properties.Contains("SleepCycleIterations"))
                    {
                        sleepCycleIterations = Int32.Parse(TestContext.Properties["SleepCycleIterations"].ToString(), CultureInfo.InvariantCulture.NumberFormat);  
                    }

                    if (sleepCycleIterations > 0 || sleepDurationInSec > 0 )
                    {
                        doSleepTest = true;
                    }
                }
                if (validateMacRandomization)
                {
                    var currentRandomizationState = wlanApi.GetRandomizationState(wlanInterface.Id);
                    switch (currentRandomizationState)
                    {
                        case Dot11MacRandomizationStatus.dot11_mac_randomization_enabled:
                        case Dot11MacRandomizationStatus.dot11_mac_randomization_disabled:
                            {
                                logger.LogComment("MAC Address randomization is supported on test interface");
                                initialRandomizationState = (currentRandomizationState == Dot11MacRandomizationStatus.dot11_mac_randomization_enabled);
                            }
                            break;
                        case Dot11MacRandomizationStatus.dot11_mac_randomization_blocked_by_admin:
                            {
                                logger.LogError("MAC Address randomization is blocked on test interface");
                            }
                            return;
                        default:
                            {
                                logger.LogError("MAC Address randomization is not available on test interface {0}", currentRandomizationState);
                                logger.LogResult(TestResult.Failed);
                            }
                            return;
                    }
                    if (initialRandomizationState)
                    {
                        logger.LogComment("Enabling MAC randomization");
                        wlanApi.SetRandomizationState(wlanInterface.Id, true);
                    }
                    else
                    {
                        logger.LogComment("MAC randomization state is currently enabled");
                    }
                }

                try
                {
                    if (xboxRun)
                    {
                        ValidateNetworkIsInScanList(wlanApi, wlanInterface, logger, false, ssid, false);
                    }
                    else
                    {
                        ValidateNetworkIsInScanList(wlanApi, wlanInterface, logger, strict, ssid);
                    }
                    ConnectAndValidate(wlanApi, wlanInterface, logger, strict, ssid, addProfile, autoconnect);
                    GetConnectionAttributes(wlanApi, wlanInterface, logger);
                    var adapterInfo = WaitForIPAssignment(wlanApi, wlanInterface, logger, 60000);
                    ValidateConnectivity(checkHttp, url, logger, adapterInfo);

                    if (doSleepTest)
                    {
                        PowerManager m_PowerManager = new PowerManager();
                        logger.LogComment("Sleep for {0} seconds {1} times", sleepDurationInSec, sleepCycleIterations);
                        for (int i = 1; i <= sleepCycleIterations; i++)
                        {
                            logger.LogComment("=========================================");
                            logger.LogComment("Start sleep cycle {0}", i);
                            m_PowerManager.DeepestSleep(sleepDurationInSec * 1000);
                            logger.LogComment("finish sleep cycle {0}", i);
                            logger.LogComment("=========================================");
                            Wlan.Sleep(5000);
                            ValidateConnectivity(checkHttp, url, logger, adapterInfo);
                        }
                    }

                    logger.LogComment("Disconnecting from the AP...");
                    DisconnectInterface(wlanApi, wlanInterface, logger);
                }
                finally
                {
                    if (validateMacRandomization)
                    {
                        if (!initialRandomizationState)
                        {
                            wlanApi.SetRandomizationState(wlanInterface.Id, false);
                        }
                    }

                }
            }
            catch (TimeoutException)
            {
                logger.LogError("The connection attempt timed out");
                logger.LogResult(TestResult.Failed);
            }
            catch (ConnectionTimedOutException ex)
            {
                logger.LogError(ex.Message);
                logger.LogResult(TestResult.Failed);
            }
            catch (ConnectionFailedException ex)
            {
                logger.LogError(String.Format(CultureInfo.InvariantCulture, "{0} Reason Code:{1}", ex.Message, ex.FailureReason));
                logger.LogResult(TestResult.Failed);
            }
            catch (WlanOuterloopTestException ex)
            {
                logger.LogError(ex.Message);
                logger.LogResult(TestResult.Failed);
            }
            catch (System.Exception ex)
            {
                if (ex is NullReferenceException || ex is System.Runtime.InteropServices.SEHException)
                {
                    throw;
                }
                logger.LogError("Exception hit while running ConnectTest: {0}", ex);
                logger.LogResult(TestResult.Failed);
            }
            finally
            {
                try
                {
                    logger.LogComment("Deleting profile {0}", ssid);
                    wlanApi.DeleteProfile(wlanInterface.Id, ssid);
                }
                catch
                {
                }
            }

        }
        private AdapterInfo WaitForIPAssignment(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, int l3WaitTimeout)
        {
            logger.LogComment("Waiting {0}ms for Network Connectivity on interface: {1}", l3WaitTimeout, wlanInterface.Id);

            Stopwatch addressAssignment = Stopwatch.StartNew();
            var adapterInfo = WaitForConnectivity(wlanInterface.Id, l3WaitTimeout, logger);
            addressAssignment.Stop();
            logger.LogComment("Layer 3 connectivity was detected in {0}ms.", addressAssignment.ElapsedMilliseconds);
            return adapterInfo;
        }
        private void ValidateConnectivity(bool psk, string url, WlanHckTestLogger logger, AdapterInfo adapterInfo)
        {
            if (String.IsNullOrEmpty(url) && TestContext != null)
            {
                if (TestContext.Properties.Contains("Url"))
                {
                    url = TestContext.Properties["Url"] as string;
                }
            }
            CheckConnectivity(logger, adapterInfo, psk, url);
        }
        private static void CheckForResets(Wlan wlanApi, WlanInterface wlanInterface, Microsoft.Test.Networking.Kit.Tracing.WlanHckTestLogger logger)
        {
            bool resetAdapter = false;
            try
            {
                var resetCount = OidQuery.GetDeviceResetCount(wlanInterface.Id);
                if (resetCount > 0)
                {
                    logger.LogError("Querying NDIS for OID_GEN_RESET_COUNTS corresponding to selected device. Result must be 0.");
                    logger.LogError("Device has been reset {0} times since it was last loaded.", resetCount);
                    logger.LogError("Device should not reset under normal conditions");
                    logger.LogError("This number is a sum of a miniport's OWN reset count as well as the number of times the miniport did not respond to an NDIS OID request.");
                    logger.LogComment("Reseting adapter to clear reset count");
                    resetAdapter = true;
                }
            }
            catch (System.IO.FileNotFoundException)
            {
                logger.LogComment("The adapter is no longer responding, attempting to reset");
                resetAdapter = true;
            }
            if (resetAdapter)
            {
                logger.LogComment("Reseting adapter");
                if (ResetWirelessDevice(wlanApi, wlanInterface.Id))
                {
                    logger.LogComment("Reset Complete");
                }
                else
                {
                    logger.LogError("Failed to reset adapter, additional tests may fail");
                }
            }
        }
        private bool ValidateConnectParameters(bool psk, WlanHckTestLogger logger, out string ssid, out string password, out string username, out string domain, out string[] rootCAs)
        {
            ssid = "kitstestssid";
            password = "password";
            username = null;
            domain = null;
            rootCAs = null;
            string[] defaultRootCAs = { "12 39 2d d6 e5 f5 2a 45 b3 3d fd 1a 5b a3 66 8e 8b 00 b4 f4", "28 25 29 75 a2 b0 d7 9e 0b f5 f1 9a 81 c4 83 e8 c6 c2 e9 4a", "75 f5 06 9c a4 12 0e 9b db bc a1 d9 9d d0 f0 75 fa 3b b8 78" };

            if (TestContext == null)
            {
                return false;
            }

            if (psk == true)
            {

                if (!TestContext.Properties.Contains("WPA2_PSK_AES_SSID"))
                {
                    logger.LogComment("WPA2_PSK_AES_SSID was not found as a parameter. Using Default {0}", ssid);
                }
                else
                {
                    ssid = (string)TestContext.Properties["WPA2_PSK_AES_SSID"];
                }

                if (!TestContext.Properties.Contains("WPA2_PSK_Password"))
                {
                    logger.LogComment("WPA2_PSK_Password was not found as a parameter. Using Default {0}", password);
                }
                else
                {
                    password = (string)TestContext.Properties["WPA2_PSK_Password"];
                }

            }
            else
            {
                //Changed parameters so both tests can be run at the same time

                if (!TestContext.Properties.Contains("WPA2_DOMAIN_SSID"))
                {
                    logger.LogError("WPA2_DOMAIN_SSID was not found as a parameter. Failing.");
                    logger.LogResult(TestResult.Failed);
                    return false;
                }
                else
                {
                    ssid = (string)TestContext.Properties["WPA2_DOMAIN_SSID"];
                }

                if (!TestContext.Properties.Contains("WPA2_DOMAIN_PASSWORD"))
                {
                    logger.LogError("WPA2_DOMAIN_PASSWORD was not found as a parameter. Failing.");
                    logger.LogResult(TestResult.Failed);
                    return false;
                }
                else
                {
                    password = (string)TestContext.Properties["WPA2_DOMAIN_PASSWORD"];
                }

                if (!TestContext.Properties.Contains("WPA2_DOMAIN_USERNAME"))
                {
                    logger.LogError("WPA2_DOMAIN_USERNAME was not found as a parameter. Failing.");
                    logger.LogResult(TestResult.Failed);
                    return false;
                }
                else
                {
                    username = (string)TestContext.Properties["WPA2_DOMAIN_USERNAME"];
                }

                if (!TestContext.Properties.Contains("WPA2_DOMAIN"))
                {
                    logger.LogError("WPA2_DOMAIN was not found as a parameter. Failing.");
                    logger.LogResult(TestResult.Failed);
                    return false;
                }
                else
                {
                    domain = (string)TestContext.Properties["WPA2_DOMAIN"];
                }


                if (TestContext != null)
                {
                    if ((!TestContext.Properties.Contains("WPA2_PEAP_ROOTCAS")) || (TestContext.Properties.Contains("WPA2_PEAP_ROOTCAS").ToString() == ""))
                    {
                        logger.LogComment("No WPA2_PEAP_ROOTCAS specified using defaults... ");
                        foreach (string rootCA in defaultRootCAs)
                        {
                            logger.LogComment("   RootCA - [{0}]", rootCA);
                        }
                        rootCAs = defaultRootCAs;
                    }
                    else
                    {
                        rootCAs = ((string)TestContext.Properties["WPA2_PEAP_ROOTCAS"]).Split(',');
                    }
                }
            }
            return true;
        }

        private void ConnectAndValidate(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool strict, string profileName, Action addProfile, bool autoconnect = true)
        {
            if (strict)
            {
                ConnectToProfile(wlanApi, wlanInterface, logger, true, 300, true, 4000, profileName, addProfile, autoconnect);
            }
            else
            {
                ConnectToProfile(wlanApi, wlanInterface, logger, false, 300, false, 6000, profileName, addProfile, autoconnect);
            }

        }
        private static int GetDisplayBarCount(long signalQuality, bool threshold)
        {
            if (signalQuality == 0)
            {
                return 0;
            }
            if (signalQuality > Int32.MaxValue)
            {
                throw new ArgumentOutOfRangeException("signalQuality");
            }
            return (int)Math.Ceiling(((int)signalQuality) / (threshold ? 25.0 : 20.0));
        }
        private static void GetConnectionAttributes(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger)
        {
            int channel = 0;
            int rssi = 0;
            try
            {
                channel = wlanApi.GetChannel(wlanInterface);
            }
            catch (System.Runtime.InteropServices.COMException ex)
            {
                logger.LogError("GetChannel with error {0}", GetStringForErrorCode(ex.HResult));
            }
            try
            {
                rssi = wlanApi.GetRssi(wlanInterface);
            }
            catch (System.Runtime.InteropServices.COMException ex)
            {
                logger.LogError("GetRssi with error {0}", GetStringForErrorCode(ex.HResult));
            }
            try
            {
                var connectionAttributes = wlanApi.GetConnectionAttributes(wlanInterface);
                logger.LogComment("Current Connection is:{0}", connectionAttributes.ConnectionMode);
                logger.LogComment("Associated with BSSID:{0} Signal:{1} RX:{2} TX:{3} Phy:{4} Bars W8:{5} TH:{6} Channel:{7} RSSI:{8}", connectionAttributes.AssociatedBssid, connectionAttributes.SignalQuality, connectionAttributes.RxRate, connectionAttributes.TxRate, connectionAttributes.PhyType, GetDisplayBarCount(connectionAttributes.SignalQuality, false), GetDisplayBarCount(connectionAttributes.SignalQuality, true), channel, rssi);
            }
            catch (System.Runtime.InteropServices.COMException ex)
            {
                logger.LogError("GetConnectionAttributes with error {0}", GetStringForErrorCode(ex.HResult));
            }
        }

        private static void CheckConnectivity(WlanHckTestLogger logger, AdapterInfo adapterInfo, bool checkHttp, string url = null)
        {
            var localAddress = adapterInfo.LocalAddresses.First(x => x.AddrType == AddressFamily.AF_INET);
            var gatewayAddress = adapterInfo.GatewayAddresses.First(x => x.AddrType == AddressFamily.AF_INET);
            if (localAddress == null)
            {
                logger.LogError("No IPv4 address detected");
                return;
            }
            if (gatewayAddress == null)
            {
                logger.LogError("No IPv4 gateway detected");
                return;
            }
            TestICMPGatewayConnectivity(logger, localAddress, gatewayAddress, 20, 90, 10);

            if (checkHttp == true && m_RunningOnVNE == false || !String.IsNullOrEmpty(url))
            {
                ValidateHttpConnectivity(logger, adapterInfo, url);



            }
        }

        private static void ValidateHttpConnectivity(WlanHckTestLogger logger, AdapterInfo adapterInfo, string url)
        {
            var uri = String.Format(CultureInfo.InvariantCulture, "http://{0}", adapterInfo.GatewayAddresses[0].ToString());
            if (!String.IsNullOrEmpty(url))
            {
                uri = url;
            }
            logger.LogComment("Starting http connectivity tests to host {0}", uri);

            var httpSuccess = MakeHttpRequest(adapterInfo, uri, logger);

            if (httpSuccess)
            {
                logger.LogComment("Successfully connected to the AP on port 80...");
            }
            else
            {
                logger.LogError("There was an error during the Layer 3 connectivity tests. Check the log above.");
                logger.LogResult(TestResult.Failed);
            }
        }

        private static void TestICMPGatewayConnectivity(WlanHckTestLogger logger, IPAddress localAddress, IPAddress destinationAddress, int totalPings, int miniumSuccessRate, int intraPacketSpacingInMilliseconds)
        {
            logger.LogComment("Sending ping from {0} to {1}", localAddress, destinationAddress);
            int success = 0;
            for (int i = 0; i < totalPings; i++)
            {
                var reply = IpHelpers.Ping(localAddress, destinationAddress);
                switch (reply.Status)
                {
                    case IpStatus.IP_SUCCESS:
                        {
                            logger.LogComment("Ping success reply from {0} time {1}ms", new IPAddress(reply.Address), reply.RoundTripTime);
                            success++;
                        }
                        break;
                    case IpStatus.IP_REQ_TIMED_OUT:
                        {
                            logger.LogWarning("Ping timed out no reply from {0} time {1}ms", new IPAddress(reply.Address), reply.RoundTripTime);

                        }
                        break;
                    case IpStatus.IP_GENERAL_FAILURE:
                        {
                            logger.LogError("Failing Test: Unable to send ICMP Ping packet, waiting 1 second between requests");
                            Wlan.Sleep(1000);
                            logger.LogResult(TestResult.Failed);
                        }
                        break;
                    default:
                        {
                            logger.LogError("Recieved unexpected ICMP reply {0}", reply.Status);
                            logger.LogResult(TestResult.Failed);
                        }
                        break;

                }
                if (i < totalPings)
                {
                    Wlan.Sleep(intraPacketSpacingInMilliseconds);
                }

            }
            var successRate = (success / (totalPings * 1.0)) * 100;
            if (successRate >= miniumSuccessRate)
            {
                logger.LogComment("Successfully recieved replies for {0}% of ICMP(ping)", successRate);
            }
            else
            {
                logger.LogError("Failed to recieve 90% response rate for ICMP(ping) Actual:{0}", successRate);
                logger.LogResult(TestResult.Failed);
            }
        }

        private static bool MakeHttpRequest(AdapterInfo adapterInfo, string uri, WlanHckTestLogger logger)
        {
            try
            {
                using (HttpRequest request = new HttpRequest(new Uri(uri)))
                {
                    int status = 0;
                    var response = request.Request(out status, adapterInfo.InterfaceIndex);
                    logger.LogComment("Http Status is {0} recieved {1} bytes", status, response.Length);
                    if (response.Length != 0)
                    {
                        var output = InteropUtilities.GetFormmatedByteArray(response);
                        if (output.Count != 0)
                        {
                            logger.LogTrace("First 16 bytes");
                            logger.LogTrace("{0}", output[0]);
                        }
                    }
                    return true;
                }
            }
            catch (TimeoutException)
            {
                logger.LogComment("Http request has timed out");
                return false;
            }
            catch (COMException ex)
            {
                logger.LogComment("Http request faied with HRresult:{0:X}", ex.HResult);
                return false;
            }
        }


        public void ConnectToProfile(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, bool failOnAssociationTimeout, int associationTimeout, bool failOnConnectTimeout, int connectTimeout, string profileName, Action addProfile, bool autoconnect)
        {
            if (logger == null)
            {
                throw new ArgumentNullException("logger");
            }

            if (wlanApi == null)
            {
                throw new ArgumentNullException("wlanApi");
            }
            if (addProfile == null)
            {
                throw new ArgumentNullException("addProfile");
            }


            EventHandler<WlanConnectionEventArgs> HandleConnectStart = null;
            EventHandler<WlanConnectionEventArgs> HandleConnectFail = null;
            EventHandler<WlanConnectionEventArgs> HandleConnectComplete = null;
            EventHandler<ConnectionStatusEventArgs> HandleAssociationStart = null;
            EventHandler<ConnectionStatusEventArgs> HandleAssociationComplete = null;
            EventHandler<WlanInterfaceEventArgs> HandleScanComplete = null;
            EventHandler<WlanInterfaceEventArgs> HandleScanRefresh = null;

            EventHandler<EventArrivedEventArgs> ETWCallback = null;
            EventHandler<EventArrivedEventArgs> ConnectStart = null;
            EventHandler<EventArrivedEventArgs> ConnectEnd = null;
            EventHandler<EventArrivedEventArgs> AssociateStart = null;
            EventHandler<EventArrivedEventArgs> AssociateEnd = null;

            WlanEtwWatcher wlanEtwWatcher = null;
            using (StateTimer serviceConnectTimer = new StateTimer(logger, "Service Connect"),
             etwConnectTimer = new StateTimer(logger, "ETW Connect"),
             etwAssociateTimer = new StateTimer(logger, "ETW Association"),
             serviceAssociateTimer = new StateTimer(logger, "Service Association"))
            {
                using (ManualResetEvent connectionFailedEvent = new ManualResetEvent(false))
                {
                    try
                    {

                        WlanReasonCode connectionFailedReasonCode = (WlanReasonCode)0;
                        Stopwatch ConnectTimer = new Stopwatch();


                        ETWCallback = (object sender, EventArrivedEventArgs properties) =>
                        {
                            LogEtwEvent(properties, logger);
                        };
                        ConnectStart = (object sender, EventArrivedEventArgs properties) =>
                        {
                            Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                            if (AdapterGuid != wlanInterface.Id)
                            {
                                return;
                            }
                            etwConnectTimer.Start(properties.TimeStamp);
                        };


                        ConnectEnd = (object sender, EventArrivedEventArgs properties) =>
                        {
                            Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                            if (AdapterGuid != wlanInterface.Id)
                            {
                                return;
                            }
                            etwConnectTimer.Stop(properties.TimeStamp);
                        };
                        AssociateStart = (object sender, EventArrivedEventArgs properties) =>
                        {
                            Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                            if (AdapterGuid != wlanInterface.Id)
                            {
                                return;
                            }
                            etwAssociateTimer.Start(properties.TimeStamp);

                        };
                        AssociateEnd = (object sender, EventArrivedEventArgs properties) =>
                        {
                            Guid AdapterGuid = GetAdapterGuid(wlanApi, properties);
                            if (AdapterGuid != wlanInterface.Id)
                            {
                                return;
                            }
                            etwAssociateTimer.Stop(properties.TimeStamp);
                        };



                        HandleConnectStart = (sender, eventArgs) =>
                        {
                            if (eventArgs.InterfaceId != wlanInterface.Id)
                            {
                                return;
                            }
                            serviceConnectTimer.Start(DateTime.Now);
                        };

                        HandleConnectFail = (sender, eventArgs) =>
                        {
                            if (eventArgs.InterfaceId != wlanInterface.Id)
                            {
                                return;
                            }
                            logger.LogComment("Connection Failed!:{0} (Time: {1}ms)", eventArgs.ReasonCode, ConnectTimer.ElapsedMilliseconds);

                            if (connectionFailedReasonCode == (WlanReasonCode)0)
                            {
                                connectionFailedReasonCode = eventArgs.ReasonCode;
                            }
                            connectionFailedEvent.Set();
                        };

                        HandleConnectComplete = (sender, eventArgs) =>
                        {
                            if (eventArgs.InterfaceId != wlanInterface.Id)
                            {
                                return;
                            }
                            serviceConnectTimer.Stop(DateTime.Now);
                            ConnectTimer.Stop();
                        };

                        HandleAssociationStart = (sender, eventArgs) =>
                        {
                            serviceAssociateTimer.Start(DateTime.Now);
                        };

                        HandleAssociationComplete = (sender, eventArgs) =>
                        {
                            if (eventArgs.ReasonCode != WlanReasonCode.WLAN_REASON_CODE_SUCCESS)
                            {
                                logger.LogComment("Association completed with WlanReason:{0}", eventArgs.ReasonCode);
                            }
                            serviceAssociateTimer.Stop(DateTime.Now);
                        };

                        HandleScanComplete = (sender, eventArgs) =>
                        {
                            if (eventArgs.InterfaceId != wlanInterface.Id)
                            {
                                return;
                            }
                            logger.LogComment("WlanSVC has completed a scan operation");
                        };

                        HandleScanRefresh = (sender, eventArgs) =>
                        {
                            if (eventArgs.InterfaceId != wlanInterface.Id)
                            {
                                return;
                            }
                            logger.LogComment("WlanSVC has reported a scan refresh");
                        };

                        wlanApi.ConnectionStart += HandleConnectStart;
                        wlanApi.ConnectionFail += HandleConnectFail;
                        wlanApi.ConnectionComplete += HandleConnectComplete;
                        wlanApi.Associating += HandleAssociationStart;
                        wlanApi.Associated += HandleAssociationComplete;
                        wlanApi.ScanComplete += HandleScanComplete;
                        wlanApi.ScanRefreshed += HandleScanRefresh;

                        if (ETWCallback != null)
                        {
                            wlanEtwWatcher = new WlanEtwWatcher();
                            wlanEtwWatcher.EventArrived += ETWCallback;
                            wlanEtwWatcher.ConnectStart += ConnectStart;
                            wlanEtwWatcher.ConnectEnd += ConnectEnd;
                            wlanEtwWatcher.AssociateStart += AssociateStart;
                            wlanEtwWatcher.AssociateEnd += AssociateEnd;
                            wlanEtwWatcher.Start();
                        }
                        addProfile();
                        ConnectTimer.Restart();
                        if (!autoconnect)
                        {
                            wlanApi.ProfileConnect(wlanInterface.Id, profileName);
                        }
                        var uiTimeout = 30000;
                        var connectMax = uiTimeout * 3;

                        switch (WaitHandle.WaitAny(new WaitHandle[] { serviceConnectTimer.Complete, connectionFailedEvent }, connectMax))
                        {
                            case WaitHandle.WaitTimeout:
                                {
                                    ConnectTimer.Stop();
                                    throw new ConnectionTimedOutException(String.Format(CultureInfo.InvariantCulture, "Connect has timed out after {0}ms", (int)connectMax));
                                }

                            case 0:
                                {
                                    logger.LogComment("Connect finished in {0}ms", serviceConnectTimer.Elapsed.TotalMilliseconds);
                                    if (serviceConnectTimer.Elapsed.TotalMilliseconds > uiTimeout)
                                    {
                                        logger.LogError("Connect exceeded maximum allowable time {0}ms Expected {1}ms", serviceConnectTimer.Elapsed.TotalMilliseconds, uiTimeout);
                                    }
                                    if (serviceAssociateTimer.Finished)
                                    {
                                        logger.LogComment("Association finished in {0}ms", serviceAssociateTimer.Elapsed.TotalMilliseconds);
                                    }
                                    else
                                    {
                                        throw new AssociationFailedException("Association not completed. Association failed.");
                                    }
                                }
                                break;
                            case 1:
                                {
                                    throw new ConnectionFailedException("There was a failure connecting to the AP", connectionFailedReasonCode);
                                }
                        }


                        if (!etwConnectTimer.WaitForFinish(30000))
                        {
                            throw new ConnectionTimedOutException("Waited for more than 30s with no connect start & complete ETW events appearing.");
                        }
                        else
                        {
                            if (etwConnectTimer.Elapsed.TotalMilliseconds > connectTimeout)
                            {
                                throw new ConnectionTimedOutException(String.Format(CultureInfo.InvariantCulture, "ETW Connect timer timed out Required:{0} Actual:{1}.", connectTimeout, etwConnectTimer.Elapsed.TotalMilliseconds));
                            }
                            logger.LogComment("Informational: ETW Connect Time was {0}ms while Service Connect time was {1}ms", etwConnectTimer.Elapsed.TotalMilliseconds, ConnectTimer.ElapsedMilliseconds);
                        }

                        if (etwAssociateTimer.Finished)
                        {
                            if (etwAssociateTimer.Elapsed.TotalMilliseconds > associationTimeout)
                            {
                                if (failOnAssociationTimeout)
                                {
                                    throw new AssociationTimedOutException(String.Format(CultureInfo.InvariantCulture, "Association time of {0}ms is greater than specified test maximum of {1}ms. This has caused a failure.", etwAssociateTimer.Elapsed.TotalMilliseconds, associationTimeout));
                                }
                                else
                                {
                                    logger.LogWarning("WARNING: The Time between Connection Request and Association Complete was greater than {1}ms. Actual: {0}ms", etwAssociateTimer.Elapsed.TotalMilliseconds, associationTimeout);
                                    logger.LogWarning("This may become a blocking error in the future. The current HCK requirements list 200ms as the boundary.");
                                }
                            }
                            logger.LogComment("Informational: ETW Assoc Time was {0}ms while Service Assoc time was {1}ms", etwAssociateTimer.Elapsed.TotalMilliseconds, serviceAssociateTimer.Elapsed.TotalMilliseconds);
                        }

                        WLAN_INTERFACE_STATE constate = wlanApi.GetInterfaceState(wlanInterface);
                        if (constate != WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                        {
                            throw new ConnectionTimedOutException(String.Format(CultureInfo.InvariantCulture, "Interface not reporting as connected"));
                        }

                    }
                    finally
                    {

                        if (wlanEtwWatcher != null)
                        {
                            wlanEtwWatcher.Dispose();
                            wlanEtwWatcher = null;
                        }

                        wlanApi.ConnectionStart -= HandleConnectStart;
                        wlanApi.ConnectionFail -= HandleConnectFail;
                        wlanApi.ConnectionComplete -= HandleConnectComplete;
                        wlanApi.Associating -= HandleAssociationStart;
                        wlanApi.Associated -= HandleAssociationComplete;
                        wlanApi.ScanComplete -= HandleScanComplete;
                        wlanApi.ScanRefreshed -= HandleScanRefresh;
                    }
                }
            }
        }

        private static void CreateProfile(Wlan wlanApi, WlanInterface wlanInterface, WlanHckTestLogger logger, string profileName, string wPA2SSID, string wPA2Password, bool psk, string wPA2Domain, string wPA2Username, string[] wPA2RootCAs, bool autoconnect)
        {
            if (psk == false)
            {
                if (wPA2Username == null)
                {
                    throw new ArgumentNullException("wPA2Username");
                }

                if (wPA2Domain == null)
                {
                    throw new ArgumentNullException("wPA2Domain");
                }

                if (wPA2RootCAs == null)
                {
                    throw new ArgumentNullException("wPA2RootCAs");
                }
            }

            string profileToUse = "";
            if (psk == true)
            {
                profileToUse = CreatePskWlanProfile(profileName, wPA2SSID, wPA2Password, autoconnect, false);
                logger.LogComment("Creating \"{0}\" profile using provided SSID and Password ...", profileName);
                logger.LogComment("SSID Provided: [{0}] Password Provided: [{1}]", wPA2SSID, wPA2Password);
                wlanApi.CreateProfile(wlanInterface.Id, profileToUse, Wlan.ProfileScope.AllUser);
            }
            else
            {
                //profileToUse = LoadWlanProfile(wPA2SSID,logger);                
                profileToUse = CreateEnterpriseWlanProfile(profileName, autoconnect, wPA2SSID, wPA2Username, wPA2Password, wPA2RootCAs, "");
                logger.LogComment("Creating \"{0}\" profile using provided SSID, domain, username and Password ...", profileName);
                logger.LogComment("SSID Provided: [{0}] Domain Provided: [{1}] Username Provided: [{2}] Password Provided: [{3}]", wPA2SSID, wPA2Domain, wPA2Username, wPA2Password);
                wlanApi.CreateProfile(wlanInterface.Id, profileToUse, Wlan.ProfileScope.AllUser);
                logger.LogComment("ProfileToUse: {0}", profileToUse);
                string EAPCredentials = OneX.CreateEapCredentialsString(wPA2Username, wPA2Password, wPA2Domain);
                wlanApi.WlanSetProfileEapXmlUserData(wlanInterface.Id, profileName, 1, EAPCredentials);
            }
        }

        private static string CreatePskWlanProfile(string profileName, string wPA2SSID, string wPA2Password, bool autoconnect, bool hidden)
        {
            var ssidConfig = new SSIDConfig();
            var ssid = new SSIDClass(name: wPA2SSID);
            ssidConfig.SSIDs.Add(ssid);
            if (hidden)
            {
                ssidConfig.NonBroadcast = true;
            }
            var msmElement = new MSMElement();
            msmElement.Security.SharedKey.KeyType = KeyType.PassPhrase;
            msmElement.Security.SharedKey.KeyMaterial = wPA2Password;


            var wlanProfile = new WlanProfile(profileName, ssidConfig, ConnectionType.ESS, autoconnect ? ConnectionMode.Auto : ConnectionMode.Manual, false, msmElement);
            var ProfileToUse = wlanProfile.ToXml();
            return ProfileToUse;
        }

        public static string CreateOpenWlanProfile(string profileName, string ssid, bool autoconnect)
        {
            var wlanProfile = new WlanProfile(profileName, new SSIDConfig(ssid), ConnectionType.ESS, autoconnect ? ConnectionMode.Auto : ConnectionMode.Manual, false, new MSMElement(new SecurityClass(new AuthEncryption(AuthenticationMethod.Open, EncryptionMethod.None, false, false), null)));
            return wlanProfile.ToXml();
        }

        private static string CreateEnterpriseWlanProfile(string profileName, bool autoconnect, string wPA2EntSSID, string wPA2EntUserName, string wPA2EntPassword, string[] trustedServerRootHashs, string trustedServerName)
        {
            var ssidConfig = new SSIDConfig();
            var ssid = new SSIDClass(name: wPA2EntSSID);
            ssidConfig.SSIDs.Add(ssid);
            var msmElement = new MSMElement(new SecurityClass(new AuthEncryption(AuthenticationMethod.WPA2Enterprise, EncryptionMethod.AES, true, false),
                             PMKCacheMode.enabled, 720, 128, PreAuthMode.disabled, SUPPORTED_PHASE1_TYPE.EAP_PEAP, SUPPORTED_PHASE2_TYPE.EAP_MSCHAPV2, trustedServerRootHashs, trustedServerName));

            var wlanProfile = new WlanProfile(profileName, ssidConfig, ConnectionType.ESS, autoconnect ? ConnectionMode.Auto : ConnectionMode.Manual, false, msmElement);

            var ProfileToUse = wlanProfile.ToXml();
            return ProfileToUse;

        }

        private static void ValidateOids(Wlan wlan, WlanInterface wlanInterface, IEnumerable<NdisOids> supportedOids, WlanHckTestLogger logger)
        {
            var filtered = supportedOids.Except(Wlan.UnQueryableRequiredOids);
            var successfulOids = new List<Tuple<NdisOids, Byte[], bool, string>>();
            var failedOids = new List<Tuple<NdisOids, bool>>();


            foreach (var oid in filtered)
            {
                try
                {
                    var bytes = OidQuery.GetBytesforOidGlobal(wlanInterface.Id, oid);
                    successfulOids.Add(Tuple.Create(oid, bytes, !Wlan.RequiredOids.Contains(oid), " Global "));

                }
                catch (NotSupportedException)
                {
                    try
                    {
                        var bytes = OidQuery.GetBytesforOidSelected(wlanInterface.Id, oid);
                        successfulOids.Add(Tuple.Create(oid, bytes, !Wlan.RequiredOids.Contains(oid), "Selected"));

                    }
                    catch (NotSupportedException)
                    {
                        try
                        {
                            var bytes = OidQuery.GetBytesforOidAll(wlanInterface.Id, oid);
                            successfulOids.Add(Tuple.Create(oid, bytes, !Wlan.RequiredOids.Contains(oid), "  All  "));
                        }
                        catch (NotSupportedException)
                        {
                            failedOids.Add(Tuple.Create(oid, !Wlan.RequiredOids.Contains(oid)));
                        }
                    }
                }


            }

            logger.LogComment("{0} oids were queried successfully", successfulOids.Count);

            //successfulOids.ForEach(x =>
            //{
            //    Console.WriteLine(" {0,-41} Source:({1,8}) Optional:{2}", x.Item1, x.Item4, x.Item3);
            //    Console.WriteLine("\t {0}", InteropUtilities.DisplayByteArray(x.Item2).Replace(Environment.NewLine, Environment.NewLine + "\t "));
            //});
            logger.LogComment("{0} oids did not respond to query", failedOids.Count);
            //failedOids.ForEach(x => Console.WriteLine("\t{0} Optional:{1}", x.Item1, x.Item2));



        }

        public AdapterInfo WaitForConnectivity(Guid adapterId, int l3WaitTimeout, WlanHckTestLogger logger)
        {
            if (logger == null)
            {
                throw new ArgumentNullException("logger");
            }
            logger.LogComment("Querying DHCP Lease Information");
            var dhcpWait = new DHCPRouterRequest(adapterId, l3WaitTimeout);
            var router = dhcpWait.WaitAndGetRouterAddress();
            logger.LogComment("Router address is {0}", router);
            logger.LogComment("Retrieving Adapter Addresses");
            var adapterInfo = GetAdapterInfo(adapterId, logger);
            var localAddress = adapterInfo.LocalAddresses.First(x => x.AddrType == AddressFamily.AF_INET);
            var gatewayAddress = adapterInfo.GatewayAddresses.First(x => x.AddrType == AddressFamily.AF_INET);
            if (localAddress == null)
            {
                logger.LogError("No IPv4 address detected");
                return adapterInfo;
            }
            if (gatewayAddress == null)
            {
                logger.LogError("No IPv4 gateway detected");
                return adapterInfo;
            }

            logger.LogComment("Adapter Addresses Local {0} Gateway {1}", localAddress, gatewayAddress);
            logger.LogComment("Retrieving Route Information");
            var routeFound = DoesRouteExist(adapterInfo, gatewayAddress, logger);
            logger.LogComment(routeFound ? "A route was found" : "No route found");
            logger.LogComment("Resolving gateway");
            var gatewayResolved = ResolveGateway(adapterInfo, logger);
            logger.LogComment(gatewayResolved ? "Gateway resolved" : "Gateway not resolved");
            return adapterInfo;
        }

        private static bool ResolveGateway(AdapterInfo adapterInfo, WlanHckTestLogger logger)
        {
            for (int i = 0; i < 3; i++)
            {
                try
                {
                    var gatewayAddr = IpHelpers.ResolveNetEntry(adapterInfo.LocalAddresses[0], adapterInfo.InterfaceIndex, adapterInfo.GatewayAddresses[0]);
                    logger.LogComment("Gateway address {0} resolved to physical address {1}", adapterInfo.GatewayAddresses[0], gatewayAddr);
                    return true;
                }
                catch (TimeoutException)
                {
                    logger.LogComment("Resolving Net Entry failed, waiting for 500ms");
                    Wlan.Sleep(500);
                }
            }
            return false;
        }
        private static bool DoesRouteExist(AdapterInfo adapterInfo, IPAddress gatewayAddress, WlanHckTestLogger logger)
        {
            for (int i = 0; i < 3; i++)
            {
                var routeChange = IpHelpers.RouteChange(10 * 1000);
                var routesToDestination = IpHelpers.GetRoutesToHost(adapterInfo, gatewayAddress);

                if (routesToDestination.Count() > 0)
                {
                    logger.LogComment("A route to the destination exists");
                    foreach (var route in routesToDestination)
                    {
                        logger.LogComment("Route {0}", route);
                    }
                    return true;
                }
                else
                {
                    logger.LogComment("No matching routes found,waiting");
                    var result = routeChange.Result;
                    logger.LogComment(result ? "Route Change" : "No Route Change");
                }
            }
            return false;
        }

        private static AdapterInfo GetAdapterInfo(Guid adapterId, WlanHckTestLogger logger)
        {
            for (int attempt = 0; attempt < 10; attempt++)
            {
                var addressChange = IpHelpers.AddressChange(10 * 1000);
                var addressInfos = IpHelpers.GetAddresses();

                var adapterInfo = addressInfos.First(x => { return x.AdapterGuid == adapterId; });
                if (adapterInfo.LocalAddresses.Count == 0)
                {
                    logger.LogWarning("No local address was retrieved waiting");
                    var addressChanged = addressChange.Result;
                    if (addressChanged)
                    {
                        logger.LogComment("Recieved addressChange");
                    }
                    else
                    {
                        logger.LogComment("Did not recieve address Change");
                    }
                    continue;
                }
                if (adapterInfo.GatewayAddresses.Count == 0)
                {
                    logger.LogWarning("No gateway address was retrieved");
                    var addressChanged = addressChange.Result;
                    if (addressChanged)
                    {
                        logger.LogComment("Recieved addressChange");
                    }
                    else
                    {
                        logger.LogComment("Did not recieve address Change");
                    }
                    continue;
                }

                return adapterInfo;
            }
            throw new Exception("No Addresses Found!");
        }

        private static bool ValidateWlanAdapterCapabilities(WlanEnhancedCapabilities wlanEnhancedCaps, WlanHckTestLogger logger, bool validateVersion = true)
        {
            if (wlanEnhancedCaps == null)
            {
                throw new ArgumentNullException("wlanEnhancedCaps");
            }
            if (logger == null)
            {
                throw new ArgumentNullException("logger");
            }
            bool result = true;

            if (validateVersion && wlanEnhancedCaps.IhvWdiVersion.RawVersion != 0 &&  wlanEnhancedCaps.IhvWdiVersion.Major < 1)
            {
                logger.LogError("Reported Major WDI version is less than 1.  Reported Version:{0}", wlanEnhancedCaps.IhvWdiVersion);
                result = false;
            }
            return result;

        }

    }
    public class StateTimer : IDisposable
    {
        enum TimerState
        {
            NotStarted,
            Started,
            Finished
        }
        public EventWaitHandle Complete { get { return timerCompleted; } }

        DateTime startTime = DateTime.MinValue;
        DateTime stopTime = DateTime.MaxValue;
        TimerState currentState = TimerState.NotStarted;
        object lockObject = new object();
        ManualResetEvent timerCompleted = new ManualResetEvent(false);
        ManualResetEvent timerStarted = new ManualResetEvent(false);
        WlanHckTestLogger logger;
        string source;
        bool disposed = false;
        public StateTimer(WlanHckTestLogger logger, string source)
        {
            this.logger = logger;
            this.source = source;
        }
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }
            timerCompleted.Dispose();
            timerStarted.Dispose();
        }
        ~StateTimer()
        {
            Dispose(false);
        }
        public void Start(DateTime timeStamp)
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StateTimer");
            }
            lock (lockObject)
            {
                if (currentState == TimerState.NotStarted)
                {
                    currentState = TimerState.Started;
                    timerStarted.Set();
                    startTime = timeStamp;
                    logger.LogComment(timeStamp, "{0} START detected at {1:HH:mm:ss.fff}", source, timeStamp);
                }
            }
        }
        public void Stop(DateTime timeStamp)
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StateTimer");
            }
            lock (lockObject)
            {
                if (currentState == TimerState.Started)
                {
                    stopTime = timeStamp;
                    currentState = TimerState.Finished;
                    logger.LogComment(timeStamp, "{0} END detected at {1:HH:mm:ss.fff}", source, timeStamp);
                    timerCompleted.Set();
                }
            }
        }
        public void Reset()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StateTimer");
            }
            lock (lockObject)
            {
                currentState = TimerState.NotStarted;
                timerCompleted.Reset();
                timerStarted.Reset();
            }
        }
        public bool WaitForStart(int timeout)
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StateTimer");
            }
            return timerStarted.WaitOne(timeout);
        }
        public bool WaitForFinish(int timeout)
        {
            if (disposed)
            {
                throw new ObjectDisposedException("StateTimer");
            }
            return timerCompleted.WaitOne(timeout);
        }
        public bool Started { get { return currentState == TimerState.Started || currentState == TimerState.Finished; } }
        public bool Finished { get { return currentState == TimerState.Finished; } }
        public TimeSpan Elapsed { get { return (stopTime == DateTime.MaxValue ? DateTime.Now : stopTime) - startTime; } }
    }


}
