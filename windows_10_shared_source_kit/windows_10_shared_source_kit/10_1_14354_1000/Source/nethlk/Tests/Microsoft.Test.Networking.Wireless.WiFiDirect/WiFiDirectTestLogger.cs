///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using Microsoft.Test.Networking;
using System;
using System.Globalization;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    // Class for test logging utilities.
    internal static class WiFiDirectTestLogger
    {
        public static void StartGroup(string group)
        {
            WEX.Logging.Interop.Log.StartGroup(group);
        }

        public static void EndGroup(string group)
        {
            WEX.Logging.Interop.Log.EndGroup(group);
        }

        public static void Log(string format, params object[] args)
        {
            string input = String.Format(CultureInfo.InvariantCulture, format, args);

            WEX.Logging.Interop.Log.Comment(CultureInfo.InvariantCulture, "{0}: {1:HH:mm:ss.fff} {2}", InteropUtilities.MachineName, DateTime.Now, input);
        }

        public static void LogFromWrapper(string format, params object[] args)
        {
            string input = String.Format(CultureInfo.InvariantCulture, format, args);

            WEX.Logging.Interop.Log.Comment(CultureInfo.InvariantCulture, "{0}: {1}", InteropUtilities.MachineName, input);
        }

        public static void Error(string format, params object[] args)
        {
            string input = String.Format(CultureInfo.InvariantCulture, format, args);

            WEX.Logging.Interop.Log.Error(CultureInfo.InvariantCulture, "{0}: {1:HH:mm:ss.fff} {2}", InteropUtilities.MachineName, DateTime.Now, input);
        }
    }
}
