///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Globalization;
using WEX.Logging.Interop;
using WEX.TestExecution;
using Microsoft.Test.Networking.RemoteControl;
using Microsoft.Test.Networking.RemoteControl.StreamSocketTransport;

namespace HlkTest.DataPathTests
{
    internal class RemoteManager
    {
        public static List<dynamic> GetRemoteInstances<T>(string[] remoteAddresses, out RemoteCommandClient[] clients) where T : class, new()
        {
            List<RemoteCommandClient> remoteCommandClients = new List<RemoteCommandClient>();
            List<dynamic> remoteInstances = new List<dynamic>();
            for (int i = 0; i < remoteAddresses.Length; i++)
            {
                Log.Comment(string.Format(CultureInfo.InvariantCulture, "Connecting to remote endpoint: {0}", new object[] { remoteAddresses[i] }));
                RemoteCommandClient client = StartClient<T>(remoteAddresses[i]);
                remoteCommandClients.Add(client);
                var remoteInstance = RemoteCommandClient.GetRemoteInstance(client, typeof(T));
                remoteInstances.Add(remoteInstance);
            }

            clients = remoteCommandClients.ToArray();
            return remoteInstances;
        }

        static RemoteCommandClient StartClient<T>(string address, int port = 5000) where T : class, new()
        {
            RemoteCommandClient client = new RemoteCommandClient(new Type[]
            {
                typeof(T),
                typeof(byte[]),
            },
            new Type[]
            {
                typeof(byte[]),
            });

            client.Connect(address, port);
            return client;
        }
    }
}
