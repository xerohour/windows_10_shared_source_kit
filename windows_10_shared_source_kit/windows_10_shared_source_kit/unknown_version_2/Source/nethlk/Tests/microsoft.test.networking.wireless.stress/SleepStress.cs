///
///
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
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
using Microsoft.Test.Networking.Kit;

namespace HlkTest.Stress
{


    internal class SleepStress : StressModule
    {
        private int scanAfterSleepAttempts = 0;
        private int scanAfterSleepSuccess = 0;
        private int connectedAfterSleepAttempts = 0;
        private int connectedAfterSleepSuccess = 0;
        private int connectedAfterRoamAttempts = 0;
        private int connectedAfterRoamSuccess = 0;

        public SleepStress(Random random, Collection<Guid> routers, WlanHckTestLogger testLogger)
            : base(random, routers, testLogger)
        {
            this.Name = "Sleep Stress";
        }

        public override void Run(string profileName1, string profileName2)
        {
            DateTime endTime = DateTime.Now.Add(new TimeSpan(0, 10, 0));


            switch (Random.Next(0, 2))
            {
                case 0:
                    SleepWakeAndScan(endTime, profileName1);
                    break;
                case 1:
                    SleepWhileConnected(endTime, profileName1);
                    break;
            }
        }

        private void SleepWakeAndScan(DateTime endTime, string profileName1)
        {
            testLogger.LogComment("SleepWakeAndScan");

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    wlanApi.Disconnect(wlanInterface.Id, WlanStress.DisconnectWait);
                }
                

            }

            Verify.IsTrue(VerifyAPsAreInScanList(profileName1), "Verify APs are in Scan List");

            while (DateTime.Now < endTime)
            {
                WakeHelper.StaticD2Sleep(new TimeSpan(0, 0, 30));

                scanAfterSleepAttempts++;
                if (VerifyAPsAreInScanList(profileName1))
                {
                    scanAfterSleepSuccess++;
                }
            }



        }
        private void SleepWhileConnected(DateTime endTime, string profileName1)
        {
            testLogger.LogComment("SleepWhileConnected");


            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                if (wlanInterface.State != WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Connecting to {0}", profileName1));
                    wlanApi.ProfileConnect(wlanInterface.Id, profileName1, WlanStress.ConnectWait);
                }

                while (DateTime.Now < endTime)
                {
                    int sleepTime = Random.Next(1, 60);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleeping for {0} seconds", sleepTime));

                    WakeHelper.StaticD2Sleep(new TimeSpan(0, 0, sleepTime));

                    testLogger.LogComment("Verify we are still connected");
                    connectedAfterSleepAttempts++;
                    if (VerifyWeAreConnected(wlanApi))
                    {
                        testLogger.LogComment("We are still connected");
                        connectedAfterSleepSuccess++;
                    }
                    else
                    {
                        testLogger.LogComment("We are not still connected");
                    }
                    Wlan.Sleep(1000);



                }



            }


        }

        private bool VerifyAPsAreInScanList(string ssid)
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Verify SSID {0} is in scan list", ssid));
            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                wlanApi.TryScan(wlanInterface.Id, true, new TimeSpan(0, 0, 10));
                Collection<WlanNetwork> list = wlanApi.GetAvailableNetworkList(wlanInterface.Id);

                bool found = false;
                foreach (var network in list)
                {
                    if (String.Compare(network.Ssid, ssid, StringComparison.OrdinalIgnoreCase) == 0)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found SSID {0}", ssid));
                        found = true;
                        break;
                    }
                }
                if (found == false)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Did not find SSID {0}", ssid));
                }
                return found;

            }
        }

        private bool VerifyWeAreConnected(Wlan wlanApi)
        {
            var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
            if (wlanInterfaceList[0].State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public override void PrintResults()
        {
            double scanAfterSleepSuccessRate = 100;
            if (scanAfterSleepAttempts > 0)
            {
                scanAfterSleepSuccessRate = ((double)scanAfterSleepSuccess / (double)scanAfterSleepAttempts) * 100;
            }

            double connectedAfterSleepSuccessRate = 100;
            if (connectedAfterSleepAttempts > 0)
            {
                connectedAfterSleepSuccessRate = ((double)connectedAfterSleepSuccess / (double)connectedAfterSleepAttempts) * 100;
            }

            double connectedAfterRoamSuccessRate = 100;
            if (connectedAfterRoamAttempts > 0)
            {
                connectedAfterRoamSuccessRate = ((double)connectedAfterRoamSuccess / (double)connectedAfterRoamAttempts) * 100;
            }

            testLogger.LogComment("Results for Sleep tests:");
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Scans After Sleep Attempts         : {0}", scanAfterSleepAttempts));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Scans After Sleep Success Rate     : {0}%", scanAfterSleepSuccessRate));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Connected After Sleep Attempts     : {0}", connectedAfterSleepAttempts));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Connected After Sleep Success Rate : {0}%", connectedAfterSleepSuccessRate));

            
        }
    }
}