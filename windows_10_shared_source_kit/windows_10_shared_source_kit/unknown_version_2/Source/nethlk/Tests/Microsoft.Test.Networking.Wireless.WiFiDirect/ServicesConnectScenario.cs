///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal class ServicesConnectOptions
    {
        public ServicesConnectOptions(
            bool callGetProvisioningInfo = false,
            bool preferGO = false,
            WiFiDirectServiceConfigurationMethod preferredConfigMethod = WiFiDirectServiceConfigurationMethod.Default,
            bool expectDeferred = false,
            bool expectGetProvisioningSuccess = true,
            bool expectGroupFormationNeeded = true,
            bool expectConnectSuccess = true,
            bool expectAdvertiserConnectSuccess = true,
            bool declineConnect = false,
            bool validateData = false,
            string sessionInfo = "",
            bool delaySeekerConnectCall = false
            )
        {
            CallGetProvisioningInfo = callGetProvisioningInfo;
            PreferGO = preferGO;
            PreferredConfigMethod = preferredConfigMethod;
            ExpectDeferred = expectDeferred;
            ExpectGetProvisioningSuccess = expectGetProvisioningSuccess;
            ExpectGroupFormationNeeded = expectGroupFormationNeeded;
            ExpectConnectSuccess = expectConnectSuccess;
            ExpectAdvertiserConnectSuccess = expectAdvertiserConnectSuccess;
            DeclineConnect = declineConnect;
            ValidateData = validateData;
            SessionInfo = sessionInfo;
            DelaySeekerConnectCall = delaySeekerConnectCall;
        }

        public bool CallGetProvisioningInfo { get; private set; }
        public bool PreferGO { get; private set; }
        public WiFiDirectServiceConfigurationMethod PreferredConfigMethod { get; private set; }
        public bool ExpectDeferred { get; private set; }
        public bool ExpectGetProvisioningSuccess { get; private set; }
        public bool ExpectGroupFormationNeeded { get; private set; }
        public bool ExpectConnectSuccess { get; private set; }
        public bool ExpectAdvertiserConnectSuccess { get; private set; }
        public bool DeclineConnect { get; private set; }
        public bool DelaySeekerConnectCall { get; private set; }

        // If true, this will open a TCP socket and send a 32k character string from the seeker
        public bool ValidateData { get; private set; }
        public string SessionInfo { get; private set; }
    }

    internal class ServicesConnectParameters
    {
        public ServicesConnectParameters(
            WFDSvcWrapperHandle discoveryHandle,
            WFDSvcWrapperHandle advertiserHandle,
            ServicesConnectOptions options
            )
        {
            DiscoveryHandle = discoveryHandle;
            AdvertiserHandle = advertiserHandle;
            Options = options;
        }

        public WFDSvcWrapperHandle DiscoveryHandle { get; private set; }
        public WFDSvcWrapperHandle AdvertiserHandle { get; private set; }
        public ServicesConnectOptions Options { get; private set; }
    }

    internal class ServicesConnectScenarioResult
    {
        public ServicesConnectScenarioResult(
            bool scenarioSucceeded,
            WFDSvcWrapperHandle seekerSessionHandle,
            WFDSvcWrapperHandle advertiserSessionHandle,
            WFDSvcWrapperHandle seekerSocketHandle,
            WFDSvcWrapperHandle advertiserSocketHandle
            )
        {
            ScenarioSucceeded = scenarioSucceeded;
            SeekerSessionHandle = seekerSessionHandle;
            AdvertiserSessionHandle = advertiserSessionHandle;
            SeekerSocketHandle = seekerSocketHandle;
            AdvertiserSocketHandle = advertiserSocketHandle;
        }

        public bool ScenarioSucceeded { get; private set; }
        public WFDSvcWrapperHandle SeekerSessionHandle { get; private set; }
        public WFDSvcWrapperHandle AdvertiserSessionHandle { get; private set; }
        // Set if doing validate data
        public WFDSvcWrapperHandle SeekerSocketHandle { get; private set; }
        // Set if doing validate data
        public WFDSvcWrapperHandle AdvertiserSocketHandle { get; private set; }
    }

    internal class ServicesConnectScenario
    {
        public ServicesConnectScenario(
            WiFiDirectTestController seekerWFDController,
            WiFiDirectTestController advertiserWFDController,
            ServicesConnectParameters connectParameters
            )
        {
            this.seekerWFDController = seekerWFDController;
            this.advertiserWFDController = advertiserWFDController;
            this.connectParameters = connectParameters;
        }

        public ServicesConnectScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesConnectScenarioResult(
                succeeded,
                seekerSessionHandle,
                advertiserSessionHandle,
                (validateDataResults != null) ? validateDataResults.OpenSocketResult.SenderSocketHandle : null,
                (validateDataResults != null) ? validateDataResults.OpenSocketResult.ReceiverSocketHandle : null
                );
        }

        private bool succeeded = false;
        private WFDSvcWrapperHandle seekerSessionHandle = null;
        private WFDSvcWrapperHandle advertiserSessionHandle = null;
        private ServicesOpenSocketSendDataScenarioResult validateDataResults = null;
        private WiFiDirectTestController seekerWFDController;
        private WiFiDirectTestController advertiserWFDController;
        private ServicesConnectParameters connectParameters;

        private static ushort validationPort = 3333;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Beginning Connect Scenario from Discovery Handle {0} to Advertiser {1}. GetProvisioning={2}, ValidateData={3}",
                    connectParameters.DiscoveryHandle,
                    connectParameters.AdvertiserHandle,
                    connectParameters.Options.CallGetProvisioningInfo,
                    connectParameters.Options.ValidateData
                    );

                string pin = "";

                // Get the current session counts
                int advertiserCountBefore = advertiserWFDController.GetServiceSessionCount();
                int seekerCountBefore = seekerWFDController.GetServiceSessionCount();

                // Set any options (like prefer GO)
                // This also calls OpenSession()

                if (connectParameters.Options.DeclineConnect)
                {
                    if (!connectParameters.Options.CallGetProvisioningInfo ||
                        connectParameters.Options.ExpectConnectSuccess)
                    {
                        throw new Exception(String.Format(
                            CultureInfo.InvariantCulture,
                            "Bad test case, decline connection should call get provisioning info and expect failure"
                            ));
                    }

                    advertiserWFDController.DeclineNextServiceConnectionRequest(connectParameters.AdvertiserHandle);
                }

                if (!connectParameters.Options.ExpectAdvertiserConnectSuccess)
                {
                    advertiserWFDController.ExpectNextServiceConnectionRequestFailure(connectParameters.AdvertiserHandle);
                }

                Stopwatch openSessionStopwatch = new Stopwatch();
                openSessionStopwatch.Start();
                seekerWFDController.SetServiceOptions(
                    connectParameters.DiscoveryHandle,
                    connectParameters.Options.PreferGO,
                    connectParameters.Options.SessionInfo
                    );
                openSessionStopwatch.Stop();

                WiFiDirectTestLogger.Log("OpenSession completed in {0} ms.", openSessionStopwatch.ElapsedMilliseconds);

                // Optionally GetProvisioningInfo before doing a PIN connection

                if (connectParameters.Options.CallGetProvisioningInfo)
                {
                    ProvisioningInfoWrapper provisioningInfo = null;
                    Stopwatch provDiscStopwatch = new Stopwatch();
                    // Handle expected failures in GetProvisioningInfo
                    try
                    {
                        provDiscStopwatch.Start();
                        provisioningInfo = seekerWFDController.GetServiceProvisioningInfo(
                            connectParameters.DiscoveryHandle,
                            connectParameters.Options.PreferredConfigMethod
                            );
                    }
                    catch (Exception e)
                    {
                        if (connectParameters.Options.ExpectGetProvisioningSuccess)
                        {
                            throw;
                        }
                        else
                        {
                            WiFiDirectTestLogger.Log("Caught expected exception in GetProvisioningInfo: {0}", e.Message);

                            if (connectParameters.Options.DeclineConnect)
                            {
                                advertiserWFDController.CheckAdvertiserLastSessionDeclined(connectParameters.AdvertiserHandle);
                            }

                            succeeded = true;
                            return;
                        }
                    }
                    finally
                    {
                        provDiscStopwatch.Stop();
                        WiFiDirectTestLogger.Log("GetServiceProvisioningInfo completed in {0} ms.", provDiscStopwatch.ElapsedMilliseconds);

                        if (succeeded && connectParameters.Options.DeclineConnect)
                        {
                            // 30 seconds is much more time than actually needed
                            if (provDiscStopwatch.ElapsedMilliseconds > 30000)
                            {
                                throw new Exception(String.Format(
                                    CultureInfo.InvariantCulture,
                                    "Expected decline to fail connect before 2 minute timeout, took too long, decline did not succeed properly"
                                    ));
                            }
                        }
                    }

                    if (!connectParameters.Options.ExpectGetProvisioningSuccess)
                    {
                        throw new Exception(String.Format(
                            CultureInfo.InvariantCulture,
                            "Expected GetProvisioningInfo to fail"
                            ));
                    }

                    if (provisioningInfo.IsGroupFormationNeeded != connectParameters.Options.ExpectGroupFormationNeeded)
                    {
                        throw new Exception(String.Format(
                            CultureInfo.InvariantCulture,
                            "Expected Group Formation {0}, Actual Group Formation {1}",
                            (connectParameters.Options.ExpectGroupFormationNeeded) ? "Needed" : "Not Needed",
                            (provisioningInfo.IsGroupFormationNeeded) ? "Needed" : "Not Needed"
                            ));
                    }

                    if (provisioningInfo.IsGroupFormationNeeded &&
                        provisioningInfo.SelectedConfigMethod != WiFiDirectServiceConfigurationMethod.Default)
                    {
                        if (provisioningInfo.SelectedConfigMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                        {
                            // Seeker should have generated a PIN, now advertiser can enter the PIN
                            pin = seekerWFDController.GetServiceDisplayedPin();
                            advertiserWFDController.EnterServicesPin(pin);
                        }
                        else
                        {
                            // Advertiser will generate/display PIN, enter on seeker side before connecting
                            pin = advertiserWFDController.GetServiceDisplayedPin();
                            seekerWFDController.EnterServicesPin(pin);
                        }
                    }
                    WiFiDirectTestLogger.Log("Get Provisioning Info complete, using PIN: {0}", pin);
                }

                // Some tests will validate that this works even if there is a time delay from provisioning info available on seeker to calling connect
                // Add a sleep here if that is the case
                if (connectParameters.Options.DelaySeekerConnectCall)
                {
                    // 5 seconds should be long enough for advertiser to attempt the connect before seeker is ready
                    Task.Delay(5000).Wait();
                }

                // Now do the real connect
                Stopwatch connectStopwatch = new Stopwatch();

                // Handle expected failures in ConnectService
                try
                {
                    connectStopwatch.Start();
                    seekerSessionHandle = seekerWFDController.ConnectService(
                        connectParameters.DiscoveryHandle
                        );
                }
                catch (Exception e)
                {
                    if (connectParameters.Options.ExpectConnectSuccess)
                    {
                        throw;
                    }
                    else
                    {
                        WiFiDirectTestLogger.Log("Caught expected exception in ConnectService: {0}", e.Message);
                        succeeded = true;
                        return;
                    }
                }
                finally
                {
                    connectStopwatch.Stop();
                    WiFiDirectTestLogger.Log("Connect completed in {0} ms.", connectStopwatch.ElapsedMilliseconds);

                    if (!connectParameters.Options.ExpectAdvertiserConnectSuccess)
                    {
                        advertiserWFDController.CheckAdvertiserLastSessionFailed(connectParameters.AdvertiserHandle);
                    }

                    if (succeeded && connectParameters.Options.DeclineConnect)
                    {
                        // 30 seconds is much more time than actually needed
                        if (connectStopwatch.ElapsedMilliseconds > 30000)
                        {
                            throw new Exception(String.Format(
                                CultureInfo.InvariantCulture,
                                "Expected decline to fail connect before 2 minute timeout, took too long, decline did not succeed properly"
                                ));
                        }
                    }
                }

                if (!connectParameters.Options.ExpectConnectSuccess)
                {
                    throw new Exception(String.Format(
                        CultureInfo.InvariantCulture,
                        "Expected Connect to fail"
                        ));
                }

                // Advertiser should have gotten the connection
                advertiserSessionHandle = advertiserWFDController.GetAdvertiserLastSession(
                    connectParameters.AdvertiserHandle,
                    pin.Length == 0 // autoAccept
                    );

                ServiceSessionInfo advertiserSession = advertiserWFDController.GetServiceSessionInfo(advertiserSessionHandle);

                if (advertiserSession.Status != WiFiDirectServiceSessionStatus.Open)
                {
                    throw new Exception(String.Format(
                        CultureInfo.InvariantCulture,
                        "Advertiser session is not open (Status={0}, ErrorStatus={1})",
                        advertiserSession.Status,
                        advertiserSession.ErrorStatus
                        ));
                }

                if (connectParameters.Options.SessionInfo.Length > 0 &&
                    connectParameters.Options.SessionInfo != advertiserSession.DeferredSessionInfo)
                {
                    throw new Exception(String.Format(
                        CultureInfo.InvariantCulture,
                        "Advertiser and Seeker session info mismatch! (Seeker={0}) (Advertiser={1})",
                        WiFiDirectTestUtilities.GetTruncatedString(connectParameters.Options.SessionInfo),
                        WiFiDirectTestUtilities.GetTruncatedString(advertiserSession.DeferredSessionInfo)
                        ));
                }

                ServiceSessionInfo seekerSession = seekerWFDController.GetServiceSessionInfo(seekerSessionHandle);

                // Compare sessions for correct data
                if (advertiserSession.ServiceName != seekerSession.ServiceName ||
                    advertiserSession.AdvertisementId != seekerSession.AdvertisementId ||
                    advertiserSession.ServiceAddress != seekerSession.ServiceAddress ||
                    advertiserSession.SessionAddress != seekerSession.SessionAddress ||
                    advertiserSession.SessionId != seekerSession.SessionId)
                {
                    throw new Exception(String.Format(
                        CultureInfo.InvariantCulture,
                        "Advertiser and Seeker session mismatch! (Seeker={0}) (Advertiser={1})",
                        seekerSession.ToString(),
                        advertiserSession.ToString()
                        ));
                }

                if (connectParameters.Options.ExpectDeferred)
                {
                    seekerWFDController.CheckServiceSessionDeferred(seekerSessionHandle, true);
                    var advertiserInfo = advertiserWFDController.GetAdvertiserInfo(connectParameters.AdvertiserHandle);

                    if (advertiserInfo.DeferredSessionInfo.Length > 0 &&
                        seekerSession.DeferredSessionInfo != advertiserInfo.DeferredSessionInfo)
                    {
                        throw new Exception(String.Format(
                            CultureInfo.InvariantCulture,
                            "Advertiser and Seeker deferred session info mismatch! (Seeker={0}) (Advertiser={1})",
                            WiFiDirectTestUtilities.GetTruncatedString(seekerSession.DeferredSessionInfo),
                            WiFiDirectTestUtilities.GetTruncatedString(advertiserInfo.DeferredSessionInfo)
                            ));
                    }
                }
                else
                {
                    seekerWFDController.CheckServiceSessionDeferred(seekerSessionHandle, false);

                    if (seekerSession.DeferredSessionInfo != "")
                    {
                        throw new Exception(String.Format(
                            CultureInfo.InvariantCulture,
                            "Seeker Received Deferred Session info, was not deferred session! (Session Info={0})",
                            WiFiDirectTestUtilities.GetTruncatedString(seekerSession.DeferredSessionInfo)
                            ));
                    }
                }

                // Check that we added 1 session to each side
                int advertiserCountAfter = advertiserWFDController.GetServiceSessionCount();
                int seekerCountAfter = seekerWFDController.GetServiceSessionCount();

                if (advertiserCountBefore + 1 != advertiserCountAfter ||
                    seekerCountBefore + 1 != seekerCountAfter)
                {
                    throw new Exception(String.Format(
                        CultureInfo.InvariantCulture,
                        "Expected 1 new session on each side! (Seeker {0} -> {1}, Advertiser {2} -> {3})",
                        seekerCountBefore,
                        seekerCountAfter,
                        advertiserCountBefore,
                        advertiserCountAfter
                        ));
                }

                if (connectParameters.Options.ValidateData)
                {
                    var dataScenario = new ServicesOpenSocketSendDataScenario(
                        seekerWFDController,
                        advertiserWFDController,
                        new ServicesOpenSocketSendDataParameters(
                            seekerSessionHandle,
                            advertiserSessionHandle,
                            WiFiDirectServiceIPProtocol.Tcp,
                            validationPort++,
                            65536
                            )
                        );

                    validateDataResults = dataScenario.Execute();

                    if (!validateDataResults.ScenarioSucceeded)
                    {
                        throw new Exception("Failed data validation!");
                    }
                }

                succeeded = true;
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service connect scenario: {0}", e);
            }
        }
    }
}
