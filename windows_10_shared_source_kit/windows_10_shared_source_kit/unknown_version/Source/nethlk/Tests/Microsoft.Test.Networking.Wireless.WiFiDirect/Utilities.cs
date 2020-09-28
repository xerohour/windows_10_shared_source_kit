///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Runtime.CompilerServices;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Microsoft.Test.Networking.Wireless;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal static class WiFiDirectTestUtilities
    {
        public static ushort ParsePortParameter(TestContext context, string parameterName, ushort defaultPort)
        {
            string portParameter = context.Properties[parameterName] as string;
            ushort port = defaultPort;

            if (portParameter != null)
            {
                if (!UInt16.TryParse(portParameter, out port))
                {
                    Verify.Fail("The value of the ListenPort parameter is not valid.");
                }
            }

            return port;
        }

        public static WiFiDirectTestController GetRemoteTestControllerFromTestContext(TestContext context)
        {
            ushort remoteServerPort = WiFiDirectTestUtilities.ParsePortParameter(context, "RemoteServerPort", WiFiDirectRemoteServer.DefaultListenPort);

            string remoteServer = context.Properties["RemoteServer"] as string;
            if (remoteServer == null)
            {
                Verify.Fail("RemoteServer parameter must be provided.");
            }

            return WiFiDirectTestController.CreateFromRemoteCommandClient(remoteServer, remoteServerPort);
        }

        public static void RunDataPathValidation(WiFiDirectTestController localWFDController, WiFiDirectTestController remoteWFDController, string port = "5001")
        {
            // Close existing socket connections
            localWFDController.ResetDataPathTester();
            remoteWFDController.ResetDataPathTester();

            // Turn off the listen state on the remote device before working with sockets.
            remoteWFDController.ConfigureListenState(false);

            remoteWFDController.StartStreamSocketListener(port);
            localWFDController.ConnectStreamSocketAndSendGuid(port);
            remoteWFDController.WaitForSocketConnectionReceived(); // Wait for the connection received callback to complete so the DataPathTestGuid is properly initialized

            // Save GUID values to a local variable, to avoid having them change between the comparison and log prints below
            Guid localTestGuid = localWFDController.DataPathTestGuid;
            Guid remoteTestGuid = remoteWFDController.DataPathTestGuid;

            if (remoteTestGuid != localTestGuid)
            {
                WiFiDirectTestLogger.Error("Received GUID value of {0} did not match transmitted value of {1}", remoteTestGuid, localTestGuid);
                return;
            }
            WiFiDirectTestLogger.Log("Validated transmitted and received GUID values match.");
        }

        public static string GetTruncatedString(string s, int limit = 32)
        {
            return (s.Length > limit) ? (s.Substring(0, limit / 2) + "..." + s.Substring(s.Length - limit / 2)) : s;
        }
    }

    internal class RemoteControllerLogGroup : IDisposable
    {
        private WiFiDirectTestController remoteWFDController;
        private string groupName;

        public RemoteControllerLogGroup(WiFiDirectTestController remoteWFDController, [CallerMemberName] string groupName = "")
        {
            if (remoteWFDController == null)
            {
                throw new System.ArgumentNullException("remoteWFDController");
            }

            if (groupName == null)
            {
                throw new System.ArgumentNullException("groupName");
            }

            this.remoteWFDController = remoteWFDController;
            this.groupName = groupName;

            if (groupName != String.Empty)
            {
                remoteWFDController.StartGroup(groupName);
            }
        }

        ~RemoteControllerLogGroup()
        {
            DisposeInternal();
        }

        public void Dispose()
        {
            DisposeInternal();
            GC.SuppressFinalize(this);
        }

        private void DisposeInternal()
        {
            if (groupName != String.Empty)
            {
                remoteWFDController.EndGroup(groupName);
            }
        }
    }
}
