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

    internal class ConnectDisconnectStress : StressModule
    {
        private int ConnectionAttempts = 0;
        private int ConnectionSuccess = 0;
        public ConnectDisconnectStress(Random random, Collection<Guid> routers, WlanHckTestLogger testLogger)
            : base(random, routers, testLogger)
        {
            this.Name = "Connect/Disconnect Stress";
        }

        public override void Run(string profileName1, string profileName2)
        {
            DateTime endTime = DateTime.Now.Add(new TimeSpan(0, 10, 0));

            using (Wlan wlanApi = new Wlan())
            {
                var wlanInterfaceList = wlanApi.EnumWlanInterfaces();
                Verify.IsTrue(wlanInterfaceList.Count >= 1, string.Format(CultureInfo.InvariantCulture, "wlanInterfaceList.Count = {0}", wlanInterfaceList.Count));
                var wlanInterface = wlanInterfaceList[0];

                if (wlanInterface.State == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    wlanApi.Disconnect(wlanInterface.Id, WlanStress.DisconnectWait);
                }


                while (DateTime.Now < endTime)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Connecting to {0}", profileName1));
                    ConnectionAttempts++;
                    var waiter1 = wlanApi.TryProfileConnect(wlanInterface.Id, profileName1, WlanStress.ConnectWait);
                    if (waiter1 != null)
                    {
                        if (waiter1.WlanReasonCode == 0)
                        {
                            ConnectionSuccess++;
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnecting from {0}", profileName1));
                            wlanApi.Disconnect(wlanInterface.Id, WlanStress.DisconnectWait);
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Connecting to {0}", profileName2));
                    ConnectionAttempts++;
                    var waiter2 = wlanApi.TryProfileConnect(wlanInterface.Id, profileName2, WlanStress.ConnectWait);
                    if (waiter2 != null)
                    {
                        if (waiter2.WlanReasonCode == 0)
                        {
                            ConnectionSuccess++;
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disconnecting from {0}", profileName2));
                            wlanApi.Disconnect(wlanInterface.Id, WlanStress.DisconnectWait);
                        }
                    }
                }

            }


        }
        public override void PrintResults()
        {
            double passRate = 0;
            if (ConnectionAttempts > 0)
            {
                passRate = ((double)ConnectionSuccess / (double)ConnectionAttempts) * 100;
            }
            testLogger.LogComment("Results for Connect/Disconnect tests:");
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Connection Attempts   : {0}", ConnectionAttempts));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Connection Success    : {0}", ConnectionSuccess));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "  Connection Pass Rate  : {0}%", passRate));
        }
    }

}