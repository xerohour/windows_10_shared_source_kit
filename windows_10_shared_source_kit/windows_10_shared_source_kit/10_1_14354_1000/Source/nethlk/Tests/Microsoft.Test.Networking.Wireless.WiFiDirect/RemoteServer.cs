///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.RemoteControl;
using System;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using System.Globalization;
using Windows.Devices.WiFiDirect.Services;
using System.Collections.Generic;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    [TestClass]
    public class WiFiDirectRemoteServer
    {
        public const ushort DefaultListenPort = 5000;

        private static ushort listenPort;
        private static TestContext testContext;

        public WiFiDirectRemoteServer()
        {
        }

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            testContext = context;
            listenPort = WiFiDirectTestUtilities.ParsePortParameter(context, "ListenPort", DefaultListenPort);
        }

        [TestMethod]
        public void Start()
        {
            Logger.SetAdditionalLogger(WEX.Logging.Interop.Log.Comment);

            WiFiDirectTestLogger.Log("Starting remote server on port {0}", listenPort);

            RemoteCommandServer remoteCommandServer = new RemoteCommandServer(
                new Type[]
                {
                    typeof(DOT11_MAC_ADDRESS),
                    typeof(WFDSvcWrapperHandle),
                    typeof(WiFiDirectServiceStatus),
                    typeof(List<WiFiDirectServiceConfigurationMethod>),
                    typeof(List<WFDSvcWrapperHandle>),
                    typeof(ServiceAdvertiserInfo),
                    typeof(DiscoveredServiceInfo),
                    typeof(ProvisioningInfoWrapper),
                    typeof(ServiceSessionInfo),
                    typeof(DOT11_WPS_CONFIG_METHOD),
                    typeof(List<ServiceAdvertiserInfo>),
                    typeof(List<String>)
                },
                new object[]
                {
                    new WiFiDirectTestController(testContext, true),
                }
                );

            remoteCommandServer.Run(listenPort);
        }
    }
}
