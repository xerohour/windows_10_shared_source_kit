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


    internal class ScanStress : StressModule
    {
        private int SlowScans = 0;
        private int FastScans = 0;

        public ScanStress(Random random, Collection<Guid> routers, WlanHckTestLogger testLogger)
            : base(random, routers, testLogger)
        {
            this.Name = "Scan Stress";
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

                while (DateTime.Now < endTime)
                {
                    if (Random.Next(0, 2) == 1)
                    {
                        // Slow Scan
                        testLogger.LogComment("Slow Scan");
                        int iterations = Random.Next(10);
                        for (int toggleLooper = 0; toggleLooper < iterations; toggleLooper++)
                        {
                            testLogger.LogComment("Scanning...");
                            wlanApi.TryScan(wlanInterface.Id, false, WlanStress.ScanWait);
                            testLogger.LogComment("Scanning...Complete");
                            SlowScans++;
                        }
                    }
                    else
                    {
                        // Fast Scan
                        testLogger.LogComment("Fast Scan");
                        int iterations = Random.Next(10);
                        for (int toggleLooper = 0; toggleLooper < iterations; toggleLooper++)
                        {
                            testLogger.LogComment("Start Fast Scan");
                            wlanApi.TryScan(wlanInterface.Id, false);
                            FastScans++;
                        }
                        testLogger.LogComment("Sleeping for a few seconds to allow the driver to calm down");
                        Wlan.Sleep(10000);
                    }

                }

            }


        }
        public override void PrintResults()
        {
            testLogger.LogComment("Results for Scan tests:");
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Slow Scans          : {0}", SlowScans));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Fast Scans          : {0}", FastScans));
        }
    }
}