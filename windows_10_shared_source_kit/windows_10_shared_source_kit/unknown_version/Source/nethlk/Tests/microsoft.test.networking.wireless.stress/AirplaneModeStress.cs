///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Text;
using System.Diagnostics.CodeAnalysis;
using System.Diagnostics;
using System.Threading;
using System.Net;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;

namespace HlkTest.Stress
{



    internal class AirplaneModeStress : StressModule
    {
        private int SlowToggles = 0;
        private int FastToggles = 0;
        private int ScanAttempts = 0;
        private int ScanSuccess = 0;


        public AirplaneModeStress(Random random, Collection<Guid> routers, WlanHckTestLogger testLogger)
            : base(random, routers, testLogger)
        {
            this.Name = "Airplane Mode Stress";
        }


        public override void Run(string profileName1, string profileName2)
        {
            DateTime endTime = DateTime.Now.Add(new TimeSpan(0, 5, 0));
            TimeSpan waiterTimeout = new TimeSpan(0, 0, 10);

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                var radioStateList = wlanApi.GetRadioState(wlanInterface);
                if (radioStateList[0].SoftwareRadioState == RadioState.Off)
                {
                    testLogger.LogComment("Turning Radio On");
                    WlanNotificationWaiter waiterOnSetup = wlanApi.CreateEventWaiter(WLAN_NOTIFICATION_MSM.wlan_notification_msm_radio_state_change);
                    wlanApi.SetRadioState(wlanInterface, 0, true);
                    waiterOnSetup.Wait(waiterTimeout);
                }

                try
                {
                    while (DateTime.Now < endTime)
                    {
                        if (Random.Next(0, 2) == 1)
                        {
                            // Slow toggle
                            testLogger.LogComment("Slow Toggle");
                            int iterations = Random.Next(10);
                            for (int toggleLooper = 0; toggleLooper < iterations; toggleLooper++)
                            {
                                testLogger.LogComment("Turning Radio Off");
                                SlowToggles++;
                                WlanNotificationWaiter waiterOff = wlanApi.CreateEventWaiter(WLAN_NOTIFICATION_MSM.wlan_notification_msm_radio_state_change);
                                wlanApi.SetRadioState(wlanInterface, 0, false);
                                waiterOff.Wait(waiterTimeout);


                                testLogger.LogComment("Turning Radio On");
                                WlanNotificationWaiter waiterOn = wlanApi.CreateEventWaiter(WLAN_NOTIFICATION_MSM.wlan_notification_msm_radio_state_change);
                                wlanApi.SetRadioState(wlanInterface, 0, true);
                                waiterOn.Wait(waiterTimeout);

                            }
                        }

                        else
                        {
                            // Fast Toggle
                            testLogger.LogComment("Fast Toggle");
                            int iterations = Random.Next(10);
                            for (int toggleLooper = 0; toggleLooper < iterations; toggleLooper++)
                            {
                                testLogger.LogComment("Turning Radio Off");
                                FastToggles++;
                                wlanApi.SetRadioState(wlanInterface, 0, false);

                                Wlan.Sleep(Random.Next(100, 750));
                                testLogger.LogComment("Turning Radio On");
                                wlanApi.SetRadioState(wlanInterface, 0, true);
                                Wlan.Sleep(Random.Next(100, 750));

                            }
                            testLogger.LogComment("Sleeping for a few seconds to allow the driver to calm down");
                            Wlan.Sleep(10000);
                        }
                        ScanAttempts++;
                        testLogger.LogComment("Scanning");
                        wlanApi.TryScan(wlanInterface.Id, true, WlanStress.ScanWait);
                        var list = wlanApi.GetAvailableNetworkList(wlanInterface.Id);
                        if (list.Count > 0)
                        {
                            ScanSuccess++;
                        }
                        else
                        {
                            testLogger.LogComment("Scanning failed");
                        }
                    }
                }
                finally
                {
                    radioStateList = wlanApi.GetRadioState(wlanInterface);
                    if (radioStateList[0].SoftwareRadioState == RadioState.Off)
                    {
                        testLogger.LogComment("Turning Radio On");
                        WlanNotificationWaiter waiterOnCleanup = wlanApi.CreateEventWaiter(WLAN_NOTIFICATION_MSM.wlan_notification_msm_radio_state_change);
                        wlanApi.SetRadioState(wlanInterface, 0, true);
                        waiterOnCleanup.Wait(waiterTimeout);
                    }
                }



            }


        }
        public override void PrintResults()
        {
            double passRate = 0;
            if (ScanAttempts > 0)
            {
                passRate = ((double)ScanSuccess / (double)ScanAttempts) * 100;
            }
            testLogger.LogComment("Results for Airplane Mode tests:");
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Slow Toggles          : {0}", SlowToggles));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Fast Toggles          : {0}", FastToggles));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Scans                 : {0}", ScanAttempts));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Scans Success Rate    : {0}%", passRate));
        }
    }
}