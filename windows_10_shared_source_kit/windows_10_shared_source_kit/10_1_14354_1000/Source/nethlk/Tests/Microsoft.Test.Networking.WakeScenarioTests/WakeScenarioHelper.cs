///
///
/// <copyright>
///  Copyright (C) Microsoft. All rights reserved.
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
///------------------------------------------------------------------------------------------------
///

using System.Globalization;
using Microsoft.Test.Networking.Kit;
using Microsoft.Test.Networking.Kit.Tracing;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Oids;
using Microsoft.Test.Networking.DeviceManager;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Text;

#pragma warning disable 0618

namespace HckTests
{
    public static class WakeNativeMethods
    {
        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool AddPattern(IntPtr pName, IntPtr pBuffer, uint bufferLength);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool RemovePattern(IntPtr pName, uint patternId);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool QueryAllPatterns(IntPtr pName, IntPtr pBuffer, uint bufferLength, out uint returnedLength);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool PktSend(IntPtr pName, IntPtr pBuffer, UInt32 bufferLength, IntPtr srcIPv4Buffer, IntPtr destIPv4Buffer, IntPtr srcIPv6Buffer, IntPtr destIPv6Buffer, IntPtr pMACBuffer, WakePatternType type, int iteration);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool GetLocalMac(IntPtr pName, IntPtr pMACBuffer);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool CoreWriteFile([MarshalAs(UnmanagedType.LPTStr)] string pName, IntPtr pMACBuffer, uint length);

        [DllImport("WakePatternTool.dll", SetLastError = true)]
        public static extern bool CoreReadFile([MarshalAs(UnmanagedType.LPTStr)] string pName, [Out] byte[] pMACBuffer, out uint length);

        [DllImport("PhoneProtDriverInstallTool.dll", SetLastError = true)]
        public static extern bool PhoneProtInstall();

        [DllImport("PhoneProtDriverInstallTool.dll", SetLastError = true)]
        public static extern bool PhoneProtUninstall();

        //Zero Memory
        [DllImport("ntdll.dll", SetLastError = true)]
        public static extern void RtlZeroMemory(IntPtr dest, int size);
    }

    public class WakeScenarioHelper
    {
        private const string CMD_EXE_NAME = "cmd.exe";
        private Guid interfaceGuid;
        private Wlan wlanApi;
        private const int EnterConnectTimeOut = 30; // in sec
        private const int WlanOperationInterval = 10; // in sec
        private const int DHCPStablize = 120; // in sec
        private static TimeSpan SchedulePhoneWakeGrace = new TimeSpan(0, 0, 10);
        private static TimeSpan ScheduleWakeGrace = new TimeSpan(0, 0, 60); // windows has DAM state which may cause button tool hang for > 30s
        private static TimeSpan ConnectStatusTimeOut = new TimeSpan(0, 0, 60);
        private static ushort DefaultTcpSrcPort = 5001;
        private static ushort DefaultTcpDestPort = 5002;
        private static ushort PartialWildcardTcpPort = 0;
        private static WlanHckTestLogger testLogger = new WlanHckTestLogger("WakeScenarioHelper", false);

        public WakeScenarioHelper(Wlan wlanApi, Guid interfaceGuid)
        {
            this.wlanApi = wlanApi;
            this.interfaceGuid = interfaceGuid;
        }

        public static IPAddress LocateIPAddress(List<IPAddress> addrList, AddressFamily returnType)
        {
            IPAddress returnAddr = null;
            foreach (var addr in addrList)
            {
                if (addr.AddrType == returnType)
                {
                    returnAddr = addr;
                    break;
                }
            }
            return returnAddr;
        }

        public static List<Guid> DisableNoiseAdapters(List<AdapterInfo> adapters, Guid interfaceKeepAlive)
        {
            List<Guid> disabledList = new List<Guid> { };

            foreach (var a in adapters)
            {
                if (InterfaceAndOperStatusFlags.HardwareInterface == (a.InterfaceAndOperStatusFlags & InterfaceAndOperStatusFlags.HardwareInterface) && (a.AdapterGuid != interfaceKeepAlive))
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Disabling {0}", a.Description));
                    if (DisableAdapter(adapters, a.AdapterGuid))
                    {
                        disabledList.Add(a.AdapterGuid);
                    }
                }
            }

            return disabledList;
        }

        public static void EnableNoiseAdapters(List<AdapterInfo> adapters, List<Guid> ensabledList)
        {
            foreach (var guid in ensabledList)
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Enabling {0}", guid));
                EnableAdapter(adapters, guid);
            }
        }

        public static bool EnableAdapter(List<AdapterInfo> adapters, Guid enableGuid)
        {
            bool isEnable = false;
            foreach (var a in adapters)
            {
                if (enableGuid == a.AdapterGuid)
                {
                    isEnable = true;
                    break;
                }
            }

            if (isEnable)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Adapter already in enabled list"));
                return true;
            }
            else
            {
                DeviceObject.EnableDevice(enableGuid);
                Wlan.Sleep(5000);

                List<AdapterInfo> adaptersAfterEnable = IpHelpers.GetAddresses();
                foreach (var a in adaptersAfterEnable)
                {
                    if (enableGuid == a.AdapterGuid)
                    {
                        isEnable = true;
                        break;
                    }
                }

                if (!isEnable)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Adapter not in enabled list"));
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        public static bool DisableAdapter(List<AdapterInfo> adapters, Guid disableGuid)
        {
            if (adapters.Count == 0)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Enabled adapter list is empty!"));
                return true;
            }

            bool isEnable = false;
            foreach (var a in adapters)
            {
                if (disableGuid == a.AdapterGuid)
                {
                    isEnable = true;
                    break;
                }
            }

            if (!isEnable)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Adapter not found in enabled list"));
                return true;
            }
            else
            {
                DeviceObject.DisableDevice(disableGuid);
                Wlan.Sleep(5000);
                isEnable = false;
                List<AdapterInfo> adaptersAfterDisable = IpHelpers.GetAddresses();

                foreach (var a in adaptersAfterDisable)
                {
                    if (disableGuid == a.AdapterGuid)
                    {
                        isEnable = true;
                        break;
                    }
                }

                if (isEnable)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Adapter still in enabled list"));
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        public static bool ValidateWakeSupport(Guid guid, Collection<WoLSupportedPatterns> sp)
        {
            bool retval = true;

            Collection<WoLSupportedPatterns> hardwareSP = new Collection<WoLSupportedPatterns> { };
            Collection<WoLSupportedPatterns> currentSP = new Collection<WoLSupportedPatterns> { };
            Collection<WoLSupportedPatterns> activeSP = new Collection<WoLSupportedPatterns> { };

            Collection<WoLSupportedEvents> hardwareSE = new Collection<WoLSupportedEvents> { };
            Collection<WoLSupportedEvents> currentSE = new Collection<WoLSupportedEvents> { };
            Collection<WoLSupportedEvents> activeSE = new Collection<WoLSupportedEvents> { };

            if (!GetPowerCap(guid, NdisOids.OID_PM_HARDWARE_CAPABILITIES, hardwareSP, hardwareSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query hardware cap"));
                retval = false;
            }

            if (!GetPowerCap(guid, NdisOids.OID_PM_CURRENT_CAPABILITIES, currentSP, currentSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query current cap"));
                retval = false;
            }

            if (!GetPowerCap(guid, NdisOids.OID_PM_PARAMETERS, activeSP, activeSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query active cap"));
                retval = false;
            }

            foreach (var s in sp)
            {
                if (!hardwareSP.Contains(s) || !currentSP.Contains(s) || !activeSP.Contains(s))
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "{0} is not in (some of) three PM caps", s));
                    retval = false;
                }
            }

            return retval;
        }

        public static bool ValidateWakeSupport(Guid guid, Collection<WoLSupportedEvents> se)
        {
            bool retval = true;

            Collection<WoLSupportedPatterns> hardwareSP = new Collection<WoLSupportedPatterns> { };
            Collection<WoLSupportedPatterns> currentSP = new Collection<WoLSupportedPatterns> { };
            Collection<WoLSupportedPatterns> activeSP = new Collection<WoLSupportedPatterns> { };

            Collection<WoLSupportedEvents> hardwareSE = new Collection<WoLSupportedEvents> { };
            Collection<WoLSupportedEvents> currentSE = new Collection<WoLSupportedEvents> { };
            Collection<WoLSupportedEvents> activeSE = new Collection<WoLSupportedEvents> { };

            if (!GetPowerCap(guid, NdisOids.OID_PM_HARDWARE_CAPABILITIES, hardwareSP, hardwareSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query hardware cap"));
                retval = false;
            }

            if (!GetPowerCap(guid, NdisOids.OID_PM_CURRENT_CAPABILITIES, currentSP, currentSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query current cap"));
                retval = false;
            }

            if (!GetPowerCap(guid, NdisOids.OID_PM_PARAMETERS, activeSP, activeSE))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to query active cap"));
                retval = false;
            }

            foreach (var s in se)
            {
                if (!hardwareSE.Contains(s) || !currentSE.Contains(s) || !activeSE.Contains(s))
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "{0} is not in (some of) three PM caps", s));
                    retval = false;
                }
            }

            return retval;
        }

        public static bool GetPowerCap(Guid guid, NdisOids oid, Collection<WoLSupportedPatterns> supportedPatterns, Collection<WoLSupportedEvents> supportedEvents)
        {
            byte[] bytes;
            bool retval = true;

            do
            {
                if (oid == NdisOids.OID_PM_PARAMETERS)
                {
                    NDIS_PM_PARAMETERS PMCap = new NDIS_PM_PARAMETERS();
                    bytes = OidQuery.GetBytesforOidGlobal(guid, oid);
                    if (bytes.Length == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "none returned from oid {0} query", oid));
                        retval = false;
                        break;
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0} bytes returned from oid {1} query", bytes.Length, oid));
                    }

                    IntPtr buffer = Marshal.AllocHGlobal(bytes.Length);
                    Marshal.Copy(bytes, 0, buffer, bytes.Length);
                    PMCap = (NDIS_PM_PARAMETERS)Marshal.PtrToStructure(buffer, typeof(NDIS_PM_PARAMETERS));
                    Marshal.FreeHGlobal(buffer);

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Enabled wake types in hardware are:"));

                    foreach (WoLSupportedPatterns sp in Enum.GetValues(typeof(WoLSupportedPatterns)))
                    {
                        if ((PMCap.EnabledWoLPacketPatterns & (uint)sp) != 0)
                        {
                            supportedPatterns.Add(sp);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0}", sp.ToString()));
                        }
                    }

                    foreach (WoLSupportedEvents se in Enum.GetValues(typeof(WoLSupportedEvents)))
                    {
                        if ((PMCap.MediaSpecificWakeUpEvents & (uint)se) != 0)
                        {
                            supportedEvents.Add(se);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0}", se.ToString()));
                        }
                    }
                }
                else
                {
                    NDIS_PM_CAPABILITIES PMCap = new NDIS_PM_CAPABILITIES();
                    if (oid == NdisOids.OID_PM_HARDWARE_CAPABILITIES || oid == NdisOids.OID_PM_CURRENT_CAPABILITIES)
                    {
                        bytes = OidQuery.GetBytesforOidGlobal(guid, oid);
                        if (bytes.Length == 0)
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "none returned from oid {0} query", oid));
                            retval = false;
                            break;
                        }
                        else
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0} bytes returned from oid {1} query", bytes.Length, oid));
                        }
                    }
                    else
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "oid {0} is not supported in GetPowerCap", oid));
                        retval = false;
                        break;
                    }

                    IntPtr buffer = Marshal.AllocHGlobal(bytes.Length);
                    Marshal.Copy(bytes, 0, buffer, bytes.Length);
                    PMCap = (NDIS_PM_CAPABILITIES)Marshal.PtrToStructure(buffer, typeof(NDIS_PM_CAPABILITIES));
                    Marshal.FreeHGlobal(buffer);

                    if (oid == NdisOids.OID_PM_HARDWARE_CAPABILITIES)
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Supported wake types in hardware are:"));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Supported wake types in current are:"));
                    }

                    foreach (WoLSupportedPatterns sp in Enum.GetValues(typeof(WoLSupportedPatterns)))
                    {
                        if ((PMCap.SupportedWoLPacketPatterns & (uint)sp) != 0)
                        {
                            supportedPatterns.Add(sp);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0}", sp.ToString()));
                        }
                    }

                    foreach (WoLSupportedEvents se in Enum.GetValues(typeof(WoLSupportedEvents)))
                    {
                        if ((PMCap.MediaSpecificWakeUpEvents & (uint)se) != 0)
                        {
                            supportedEvents.Add(se);
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "{0}", se.ToString()));
                        }
                    }
                }
            }
            while (false);

            return retval;
        }

        public static bool PktSender(Guid guid, int iter, WakePatternType testPatternType)
        {
            string devName = string.Format(CultureInfo.InvariantCulture, "\\DEVICE\\{{{0}}}", guid.ToString().ToUpper());
            byte[] patternArray = new byte[1514];
            byte[] MACArray = new byte[6];
            byte[] SrcIPv4Array = new byte[4];
            byte[] DestIPv4Array = new byte[4];
            byte[] SrcIPv6Array = new byte[16];
            byte[] DestIPv6Array = new byte[16];
            uint patternlength = 0;
            uint maclength = 0;
            uint srcipv4length = 0;
            uint destipv4length = 0;
            uint srcipv6length = 0;
            uint destipv6length = 0;
            IntPtr patternBuffer = IntPtr.Zero;
            IntPtr MACBuffer = IntPtr.Zero;
            IntPtr SrcIPv4Buffer = IntPtr.Zero;
            IntPtr DestIPv4Buffer = IntPtr.Zero;
            IntPtr SrcIPv6Buffer = IntPtr.Zero;
            IntPtr DestIPv6Buffer = IntPtr.Zero;
            bool retval = true;

            do
            {
                if (testPatternType == WakePatternType.RandomPattern)
                {
                    testLogger.LogComment("Load pattern from file");
                    if (!WakeNativeMethods.CoreReadFile("Logs\\pattern.txt", patternArray, out patternlength))
                    {
                        retval = false;
                        break;
                    }
                    PrintPatternWithoutMask(patternArray, (int)patternlength);

                    if (patternlength == 0)
                    {
                        testLogger.LogWarning("Pattern is empty");
                        retval = false;
                        break;
                    }

                    patternBuffer = Marshal.AllocHGlobal((int)patternlength);
                    Marshal.Copy(patternArray, 0, patternBuffer, (int)patternlength);
                }
                else if (testPatternType == WakePatternType.IPv4_TCP_Syn || testPatternType == WakePatternType.IPv4_Wildcard)
                {
                    testLogger.LogComment("Load source IPv4 from file");
                    if (!WakeNativeMethods.CoreReadFile("Logs\\IPv4SrcAddress.txt", SrcIPv4Array, out srcipv4length))
                    {
                        retval = false;
                        break;
                    }
                    if (srcipv4length == 0)
                    {
                        testLogger.LogWarning("Source IPv4 is empty");
                        retval = false;
                        break;
                    }
                    SrcIPv4Buffer = Marshal.AllocHGlobal((int)srcipv4length);
                    Marshal.Copy(SrcIPv4Array, 0, SrcIPv4Buffer, (int)srcipv4length);

                    testLogger.LogComment("Load destination IPv4 from file");
                    if (!WakeNativeMethods.CoreReadFile("Logs\\IPv4DestAddress.txt", DestIPv4Array, out destipv4length))
                    {
                        retval = false;
                        break;
                    }
                    if (destipv4length == 0)
                    {
                        testLogger.LogWarning("Destination IPv4 is empty");
                        retval = false;
                        break;
                    }
                    DestIPv4Buffer = Marshal.AllocHGlobal((int)destipv4length);
                    Marshal.Copy(DestIPv4Array, 0, DestIPv4Buffer, (int)destipv4length);
                }
                else if (testPatternType == WakePatternType.IPv6_TCP_Syn || testPatternType == WakePatternType.IPv6_Wildcard)
                {
                    testLogger.LogComment("Load source IPv6 from file");
                    if (!WakeNativeMethods.CoreReadFile("Logs\\IPv6SrcAddress.txt", SrcIPv6Array, out srcipv6length))
                    {
                        retval = false;
                        break;
                    }
                    if (srcipv6length == 0)
                    {
                        testLogger.LogWarning("Source IPv6 is empty");
                        retval = false;
                        break;
                    }
                    SrcIPv6Buffer = Marshal.AllocHGlobal((int)srcipv6length);
                    Marshal.Copy(SrcIPv6Array, 0, SrcIPv6Buffer, (int)srcipv6length);

                    testLogger.LogComment("Load destination IPv6 from file");
                    if (!WakeNativeMethods.CoreReadFile("Logs\\IPv6DestAddress.txt", DestIPv6Array, out destipv6length))
                    {
                        retval = false;
                        break;
                    }
                    if (destipv6length == 0)
                    {
                        testLogger.LogWarning("Destination IPv6 is empty");
                        retval = false;
                        break;
                    }
                    DestIPv6Buffer = Marshal.AllocHGlobal((int)destipv6length);
                    Marshal.Copy(DestIPv6Array, 0, DestIPv6Buffer, (int)destipv6length);
                }

                testLogger.LogComment("Load destination MAC from file");
                if (!WakeNativeMethods.CoreReadFile("Logs\\DestMAC.txt", MACArray, out maclength))
                {
                    retval = false;
                    break;
                }
                if (maclength == 0)
                {
                    testLogger.LogWarning("Destination MAC is empty");
                    retval = false;
                    break;
                }
                MACBuffer = Marshal.AllocHGlobal((int)maclength);
                Marshal.Copy(MACArray, 0, MACBuffer, (int)maclength);

                if (WakeNativeMethods.PktSend(Marshal.StringToHGlobalUni(devName), patternBuffer, patternlength, SrcIPv4Buffer, DestIPv4Buffer, SrcIPv6Buffer, DestIPv6Buffer, MACBuffer, testPatternType, iter))
                {
                    testLogger.LogComment("sent packets success");
                }
                else
                {
                    testLogger.LogWarning("Failed to send packets");
                    retval = false;
                }
            }
            while (false);

            if (patternBuffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(patternBuffer);
            }
            if (MACBuffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(MACBuffer);
            }
            if (SrcIPv4Buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(SrcIPv4Buffer);
            }
            if (DestIPv4Buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(DestIPv4Buffer);
            }
            if (SrcIPv6Buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(SrcIPv4Buffer);
            }
            if (DestIPv6Buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(DestIPv4Buffer);
            }
            return retval;           
        }

        public static byte[] GetMacAddress(Guid guid)
        {
            int length = 6;
            byte[] localMAC = null;
            string devName = string.Format(CultureInfo.InvariantCulture, "\\DEVICE\\{{{0}}}", guid.ToString().ToUpper());
            IntPtr MACBuffer = Marshal.AllocHGlobal(length);

            if (!WakeNativeMethods.GetLocalMac(Marshal.StringToHGlobalUni(devName), MACBuffer))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to get local MAC"));
            }
            else
            {
                localMAC = new byte[length];
                Marshal.Copy(MACBuffer, localMAC, 0, length);
                StringBuilder sb = new StringBuilder();
                sb.Append("Local MAC:");
                for (int i = 0; i < length; i++)
                {
                    sb.AppendFormat(CultureInfo.InvariantCulture, "{0:X2} ", localMAC[i]);
                }
                testLogger.LogComment(sb.ToString());
            }

            Marshal.FreeHGlobal(MACBuffer);
            return localMAC;
        }

        private static void PrintPatternWithoutMask(byte[] displayPattern, int length)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append("WAKE PATTERN without Mask:");

            for (int patternOffset = 0; patternOffset < length; patternOffset++)
            {
                if (patternOffset % Constants.MAX_PAYLOAD_LINE_LEN == 0)
                {
                    sb.AppendFormat(CultureInfo.InvariantCulture, "\n0x{0:X4}:   ", patternOffset);
                }

                sb.AppendFormat(CultureInfo.InvariantCulture, "{0:X2} ", displayPattern[patternOffset]);
            }

            testLogger.LogComment(sb.ToString());
        }

        private static void PrintPattern(IntPtr pattern,
                      uint sizeOfPattern,
                      IntPtr mask,
                      uint sizeOfMask
                      )
        {
            StringBuilder sb = new StringBuilder();
            Byte[] displayPattern = new Byte[sizeOfPattern];
            Byte[] displayMask = new Byte[sizeOfMask];
            uint patternOffset = 0;
            uint maskByte = 0;

            Marshal.Copy(pattern, displayPattern, 0, Convert.ToInt32(sizeOfPattern));
            Marshal.Copy(mask, displayMask, 0, Convert.ToInt32(sizeOfMask));

            sb.Append("WAKE PATTERN:");

            for (maskByte = 0; maskByte < sizeOfMask; maskByte++)
            {
                for (int maskBit = 0; maskBit < 8; maskBit++)
                {
                    if (patternOffset % Constants.MAX_PAYLOAD_LINE_LEN == 0)
                    {
                        sb.AppendFormat(CultureInfo.InvariantCulture, "\n0x{0:X4}:   ", patternOffset);
                    }

                    if ((Byte)(displayMask[maskByte] & (1 << maskBit)) != 0x00)
                    {
                        sb.AppendFormat(CultureInfo.InvariantCulture, "{0:X2} ", displayPattern[patternOffset]);
                    }
                    else
                    {
                        sb.Append("__ ");
                    }

                    patternOffset++;

                    //  Error checking
                    if (patternOffset > sizeOfPattern)
                    {
                        if (maskByte != sizeOfMask - 1)
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Invalid mask! Size of Mask = {0} > ceil(Size of Pattern / 8) = {1}", sizeOfMask, sizeOfPattern));
                        }
                    }
                }
            }

            testLogger.LogComment(sb.ToString());
        }

        /**
         *  Generate a random pattern and corresponding mask of given size.
         *  The pattern must be a valid Ethernet frame to work on WLK setups,
         *  but IP conformance is not necessary
         */
        private static void GeneratePatternAndMask(IntPtr pattern,
                                                   uint sizeOfPattern,
                                                   IntPtr mask,
                                                   uint sizeOfMask,
                                                   Random m_Random
                                                   )
        {
            //
            //  We could either generate a valid Ethernet header in the pattern
            //  or set the mask to ignore the Ethernet header. For now we'll go 
            //  with the latter since it is easier.
            //
            Byte[] managedPattern = new Byte[sizeOfPattern];
            Byte[] managedMask = new Byte[sizeOfMask];

            m_Random.NextBytes(managedPattern);
            m_Random.NextBytes(managedMask);

            //  Overwrite first NDT_ETHERNET_HEADER_SIZE bits of mask with 0
            //  We could certainly do this more efficiently, but this is the most
            //  readable method
            for (int i = 0; i < Constants.NDT_ETHERNET_HEADER_SIZE; i++)
            {
                int maskByte = i / 8;
                int maskBit = i % 8;

                //  eg. AND with 11110111 to clear 4th bit of maskByte
                managedMask[maskByte] = (Byte)(managedMask[maskByte] & ~(1 << maskBit));
            }

            //
            //  The last byte of mask could correspond to bits beyond the last byte of pattern
            //  Make sure they are cleared
            //
            if (sizeOfPattern % 8 != 0)
            {
                int lastMaskByte = Convert.ToInt32(sizeOfPattern / 8);       // remember we're 0-indexed
                int numLastMaskBits = Convert.ToInt32(sizeOfPattern % 8);

                //  Zero out extra bits in last mask byte
                //  ((1 << numMaskBits) - 1) is high only for lowest numMaskBits
                managedMask[lastMaskByte] = (Byte)(managedMask[lastMaskByte] & ((1 << numLastMaskBits) - 1));
            }

            Marshal.Copy(managedPattern, 0, pattern, Convert.ToInt32(sizeOfPattern));
            Marshal.Copy(managedMask, 0, mask, Convert.ToInt32(sizeOfMask));

            PrintPattern(pattern, sizeOfPattern, mask, sizeOfMask);
        }

        public static bool RemoveAllWolPattern(Guid ifGuid)
        {
            List<NDIS_PM_WOL_PATTERN> allPatterns = new List<NDIS_PM_WOL_PATTERN> { };
            if (!WakeScenarioHelper.QueryWolPatterns(allPatterns, ifGuid))
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Patterns failed to be queried"));
                return false;
            }

            if (allPatterns.Count == 0)
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "No bitmap pattern found"));
                return true;
            }
            else
            {
                foreach (var p in allPatterns)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Removing pattern {0} with id {1}", p.WoLPacketType, p.PatternId));
                    if (!WakeScenarioHelper.RemoveWolPattern(p.PatternId, ifGuid))
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Pattern failed to be removed"));
                        return false;
                    }
                }

                return true;
            }
        }

        public static bool RemoveWolPattern(uint patternId, Guid ifGuid)
        {
            string devName = string.Format(CultureInfo.InvariantCulture, "\\DEVICE\\{{{0}}}", ifGuid.ToString().ToUpper());
            if (WakeNativeMethods.RemovePattern(Marshal.StringToHGlobalUni(devName), patternId))
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Remove pattern success"));
                return true;
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Remove pattern failed"));
                return false;
            }
        }

        public static bool QueryWolPatterns(List<NDIS_PM_WOL_PATTERN> allPatterns, Guid ifGuid)
        {
            NDIS_PM_CAPABILITIES PMCap = new NDIS_PM_CAPABILITIES();
            byte[] bytes = OidQuery.GetBytesforOidGlobal(ifGuid, NdisOids.OID_PM_HARDWARE_CAPABILITIES);
            uint byteReturned = 0;

            if (bytes.Length == 0)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "none returned from OID_PM_HARDWARE_CAPABILITIES query"));
                return false;
            }

            IntPtr buffer = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, buffer, bytes.Length);
            PMCap = (NDIS_PM_CAPABILITIES)Marshal.PtrToStructure(buffer, typeof(NDIS_PM_CAPABILITIES));
            Marshal.FreeHGlobal(buffer);
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Max amount of patterns is {0} and max size of pattern is {1}", PMCap.NumTotalWoLPatterns, PMCap.MaxWoLPatternSize));

            uint maxBufferSize = (uint)(PMCap.NumTotalWoLPatterns * (PMCap.MaxWoLPatternSize + Math.Ceiling(PMCap.MaxWoLPatternSize / 8.0) + Constants.NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Max possible size for all patterns is {0}", maxBufferSize));

            IntPtr returnedPatternsBuffer = IntPtr.Zero;
            returnedPatternsBuffer = Marshal.AllocHGlobal(Convert.ToInt32(maxBufferSize));
            if (returnedPatternsBuffer == IntPtr.Zero)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to allocate memory for return patterns"));
                return false;
            }
            WakeNativeMethods.RtlZeroMemory(returnedPatternsBuffer, Convert.ToInt32(maxBufferSize));

            string devName = string.Format(CultureInfo.InvariantCulture, "\\DEVICE\\{{{0}}}", ifGuid.ToString().ToUpper());
            if (WakeNativeMethods.QueryAllPatterns(Marshal.StringToHGlobalUni(devName), returnedPatternsBuffer, maxBufferSize, out byteReturned))
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Query patterns success"));

                if (byteReturned == 0)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "No wake pattern found"));
                }
                else
                {
                    int offset = 0;
                    NDIS_PM_WOL_PATTERN temp = new NDIS_PM_WOL_PATTERN();
                    do
                    {
                        temp = (NDIS_PM_WOL_PATTERN)Marshal.PtrToStructure(returnedPatternsBuffer + offset, typeof(NDIS_PM_WOL_PATTERN));
                        allPatterns.Add(temp);                        
                        offset = (int)temp.NextWoLPatternOffset;
                    }
                    while (offset != 0);
                }

                Marshal.FreeHGlobal(returnedPatternsBuffer);
                return true;
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Query patterns failed"));
                Marshal.FreeHGlobal(returnedPatternsBuffer);
                return false;
            }
        }

        public static unsafe bool AddWolPattern(PM_WolPatternContext wolPatternContext, AdapterInfo testAdapterInfo, out byte[] bitmapPattern)
        {
            uint requiredBytes;
            IntPtr pBytesRead = IntPtr.Zero;
            IntPtr pBytesNeeded = IntPtr.Zero;
            string patternName = null;
            NDIS_PM_WOL_PATTERN ndisPmWolPattern = new NDIS_PM_WOL_PATTERN();
            Random m_Random = new Random((int)DateTime.Now.Ticks);
            bitmapPattern = null;

            NDIS_PM_CAPABILITIES PMCap = new NDIS_PM_CAPABILITIES();
            byte[] bytes = OidQuery.GetBytesforOidGlobal(testAdapterInfo.AdapterGuid, NdisOids.OID_PM_HARDWARE_CAPABILITIES);

            if (bytes.Length == 0)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "none returned from OID_PM_HARDWARE_CAPABILITIES query"));
                return false;
            }

            IntPtr buffer = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, buffer, bytes.Length);
            PMCap = (NDIS_PM_CAPABILITIES)Marshal.PtrToStructure(buffer, typeof(NDIS_PM_CAPABILITIES));
            Marshal.FreeHGlobal(buffer);

            requiredBytes = Constants.NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION;

            if (wolPatternContext.Type == WakePatternType.RandomPattern)
            {
                wolPatternContext.BitmapPatternSize = Convert.ToUInt32(m_Random.Next((int)Math.Ceiling(PMCap.MaxWoLPatternSize / 4.0) + 1, (int)PMCap.MaxWoLPatternSize));
                wolPatternContext.MaskSize = Convert.ToUInt32(Math.Ceiling(wolPatternContext.BitmapPatternSize / 8.0));
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Random bitmap size (include eth header): {0} bytes", wolPatternContext.BitmapPatternSize));
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Mask size: {0} bytes", wolPatternContext.MaskSize));
            }
            else if (wolPatternContext.Type == WakePatternType.MaxSizePattern)
            {
                wolPatternContext.BitmapPatternSize = Convert.ToUInt32(PMCap.MaxWoLPatternSize);
                wolPatternContext.MaskSize = Convert.ToUInt32(Math.Ceiling(wolPatternContext.BitmapPatternSize / 8.0));
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Max bitmap size: {0} bytes", wolPatternContext.BitmapPatternSize));
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Mask size: {0} bytes", wolPatternContext.MaskSize));
            }
            else
            {
                wolPatternContext.BitmapPatternSize = 0;
                wolPatternContext.MaskSize = 0;
            }

            requiredBytes += wolPatternContext.BitmapPatternSize;
            requiredBytes += wolPatternContext.MaskSize;

            wolPatternContext.NdisPmWolPatternBuffer = Marshal.AllocHGlobal(Convert.ToInt32(requiredBytes));
            wolPatternContext.NdisPmWolPatternBufferSize = requiredBytes;

            if (wolPatternContext.NdisPmWolPatternBuffer == IntPtr.Zero)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Failed to allocate memory for NDIS_PM_WOL_PATTERN"));
                return false;
            }
            WakeNativeMethods.RtlZeroMemory(wolPatternContext.NdisPmWolPatternBuffer, Convert.ToInt32(requiredBytes));
            byte[] IPv4SrcAddr = new byte[] { 0x00, 0x00, 0x00, 0x00 };
            byte[] IPv6SrcAddr = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            byte[] IPv4DestAddr = new byte[] { 0x00, 0x00, 0x00, 0x00 };
            byte[] IPv6DestAddr = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

            switch (wolPatternContext.Type)
            {
                case WakePatternType.IPv4_TCP_Syn:
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start to create IPv4 TCP SYN pattern"));
                    patternName = "NDT_IPv4TCPSyn";
                    ndisPmWolPattern.WoLPacketType = NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv4TcpSyn;

                    IPv4SrcAddr = wolPatternContext.SrcIPv4;
                    Marshal.Copy(IPv4SrcAddr, 0, (IntPtr)ndisPmWolPattern.IPv4TcpSynParameters.IPv4SourceAddress, 4);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read source IPv4 address: {0}.{1}.{2}.{3}", IPv4SrcAddr[0], IPv4SrcAddr[1], IPv4SrcAddr[2], IPv4SrcAddr[3]));

                    IPv4DestAddr = wolPatternContext.DestIPv4;
                    Marshal.Copy(IPv4DestAddr, 0, (IntPtr)ndisPmWolPattern.IPv4TcpSynParameters.IPv4DestAddress, 4);
                    testLogger.LogComment(String.Format(CultureInfo.InvariantCulture, "Read destination IPv4 address: {0}.{1}.{2}.{3}", IPv4DestAddr[0], IPv4DestAddr[1], IPv4DestAddr[2], IPv4DestAddr[3]));

                    ndisPmWolPattern.IPv4TcpSynParameters.TCPSourcePortNumber = DefaultTcpSrcPort;
                    ndisPmWolPattern.IPv4TcpSynParameters.TCPDestPortNumber = DefaultTcpDestPort;
                    break;

                case WakePatternType.IPv6_TCP_Syn:
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start to create IPv6 TCP SYN pattern"));
                    patternName = "NDT_IPv6TCPSyn";
                    ndisPmWolPattern.WoLPacketType = NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv6TcpSyn;

                    IPv6SrcAddr = wolPatternContext.SrcIPv6;
                    Marshal.Copy(IPv6SrcAddr, 0, (IntPtr)ndisPmWolPattern.IPv6TcpSynParameters.IPv6SourceAddress, 16);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read source IPv6 address: {0:X}{1:X}:{2:X}{3:X}:{4:X}{5:X}:{6:X}{7:X}:{8:X}{9:X}:{10:X}{11:X}:{12:X}{13:X}:{14:X}{15:X}", IPv6SrcAddr[0],
                                                                        IPv6SrcAddr[1], IPv6SrcAddr[2], IPv6SrcAddr[3], IPv6SrcAddr[4], IPv6SrcAddr[5], IPv6SrcAddr[6], IPv6SrcAddr[7], IPv6SrcAddr[8],
                                                                        IPv6SrcAddr[9], IPv6SrcAddr[10], IPv6SrcAddr[11], IPv6SrcAddr[12], IPv6SrcAddr[13], IPv6SrcAddr[14], IPv6SrcAddr[15]));

                    IPv6DestAddr = wolPatternContext.DestIPv6;
                    Marshal.Copy(IPv6DestAddr, 0, (IntPtr)ndisPmWolPattern.IPv6TcpSynParameters.IPv6DestAddress, 16);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read destination IPv6 address: {0:X}{1:X}:{2:X}{3:X}:{4:X}{5:X}:{6:X}{7:X}:{8:X}{9:X}:{10:X}{11:X}:{12:X}{13:X}:{14:X}{15:X}", IPv6DestAddr[0],
                                                                        IPv6DestAddr[1], IPv6DestAddr[2], IPv6DestAddr[3], IPv6DestAddr[4], IPv6DestAddr[5], IPv6DestAddr[6], IPv6DestAddr[7], IPv6DestAddr[8],
                                                                        IPv6DestAddr[9], IPv6DestAddr[10], IPv6DestAddr[11], IPv6DestAddr[12], IPv6DestAddr[13], IPv6DestAddr[14], IPv6DestAddr[15]));

                    ndisPmWolPattern.IPv6TcpSynParameters.TCPSourcePortNumber = DefaultTcpSrcPort;
                    ndisPmWolPattern.IPv6TcpSynParameters.TCPDestPortNumber = DefaultTcpDestPort;
                    break;

                case WakePatternType.IPv4_Wildcard:
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start to create IPv4 TCP wildcard pattern"));
                    patternName = "NDT_IPv4TCPSyn_Wildcard";
                    ndisPmWolPattern.WoLPacketType = NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv4TcpSyn;

                    Marshal.Copy(IPv4SrcAddr, 0, (IntPtr)ndisPmWolPattern.IPv4TcpSynParameters.IPv4SourceAddress, 4);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read source IPv4 address: {0}.{1}.{2}.{3}", IPv4SrcAddr[0], IPv4SrcAddr[1], IPv4SrcAddr[2], IPv4SrcAddr[3]));

                    IPv4DestAddr = wolPatternContext.DestIPv4;
                    Marshal.Copy(IPv4DestAddr, 0, (IntPtr)ndisPmWolPattern.IPv4TcpSynParameters.IPv4DestAddress, 4);
                    testLogger.LogComment(String.Format(CultureInfo.InvariantCulture, "Read destination IPv4 address: {0}.{1}.{2}.{3}", IPv4DestAddr[0], IPv4DestAddr[1], IPv4DestAddr[2], IPv4DestAddr[3]));

                    ndisPmWolPattern.IPv4TcpSynParameters.TCPSourcePortNumber = PartialWildcardTcpPort;
                    ndisPmWolPattern.IPv4TcpSynParameters.TCPDestPortNumber = DefaultTcpDestPort;
                    break;

                case WakePatternType.IPv6_Wildcard:
                    // match specific source address for Ipv6 since we used a wildcard on source for v4 testcase
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start to create IPv6 TCP wildcard pattern"));
                    patternName = "NDT_IPv6TCPSyn_Wildcard";
                    ndisPmWolPattern.WoLPacketType = NDIS_PM_WOL_PACKET.NdisPMWoLPacketIPv6TcpSyn;

                    IPv6SrcAddr = wolPatternContext.SrcIPv6;
                    Marshal.Copy(IPv6SrcAddr, 0, (IntPtr)ndisPmWolPattern.IPv6TcpSynParameters.IPv6SourceAddress, 16);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read source IPv6 address: {0:X}{1:X}:{2:X}{3:X}:{4:X}{5:X}:{6:X}{7:X}:{8:X}{9:X}:{10:X}{11:X}:{12:X}{13:X}:{14:X}{15:X}", IPv6SrcAddr[0],
                                                                        IPv6SrcAddr[1], IPv6SrcAddr[2], IPv6SrcAddr[3], IPv6SrcAddr[4], IPv6SrcAddr[5], IPv6SrcAddr[6], IPv6SrcAddr[7], IPv6SrcAddr[8],
                                                                        IPv6SrcAddr[9], IPv6SrcAddr[10], IPv6SrcAddr[11], IPv6SrcAddr[12], IPv6SrcAddr[13], IPv6SrcAddr[14], IPv6SrcAddr[15]));

                    Marshal.Copy(IPv6DestAddr, 0, (IntPtr)ndisPmWolPattern.IPv6TcpSynParameters.IPv6DestAddress, 16);
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Read destination IPv6 address: {0:X}{1:X}:{2:X}{3:X}:{4:X}{5:X}:{6:X}{7:X}:{8:X}{9:X}:{10:X}{11:X}:{12:X}{13:X}:{14:X}{15:X}", IPv6DestAddr[0],
                                                                        IPv6DestAddr[1], IPv6DestAddr[2], IPv6DestAddr[3], IPv6DestAddr[4], IPv6DestAddr[5], IPv6DestAddr[6], IPv6DestAddr[7], IPv6DestAddr[8],
                                                                        IPv6DestAddr[9], IPv6DestAddr[10], IPv6DestAddr[11], IPv6DestAddr[12], IPv6DestAddr[13], IPv6DestAddr[14], IPv6DestAddr[15]));

                    ndisPmWolPattern.IPv6TcpSynParameters.TCPSourcePortNumber = DefaultTcpSrcPort;
                    ndisPmWolPattern.IPv6TcpSynParameters.TCPDestPortNumber = PartialWildcardTcpPort;
                    break;

                case WakePatternType.EAPRequest:
                    break;

                case WakePatternType.RandomPattern:
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start to create random bitmap pattern"));
                    patternName = "NDT_RandomBitmap";
                    //
                    //  Set Bitmap params
                    //
                    ndisPmWolPattern.WoLPacketType = NDIS_PM_WOL_PACKET.NdisPMWoLPacketBitmapPattern;
                    ndisPmWolPattern.WoLBitMapPattern.MaskOffset = Constants.NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION;
                    ndisPmWolPattern.WoLBitMapPattern.MaskSize = wolPatternContext.MaskSize;
                    ndisPmWolPattern.WoLBitMapPattern.PatternOffset = Constants.NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION + wolPatternContext.MaskSize;
                    ndisPmWolPattern.WoLBitMapPattern.PatternSize = wolPatternContext.BitmapPatternSize;

                    //
                    //  Set pointers in context object
                    //
                    wolPatternContext.BitmapPattern = wolPatternContext.NdisPmWolPatternBuffer + Convert.ToInt32(ndisPmWolPattern.WoLBitMapPattern.PatternOffset);
                    wolPatternContext.Mask = wolPatternContext.NdisPmWolPatternBuffer + Convert.ToInt32(ndisPmWolPattern.WoLBitMapPattern.MaskOffset);

                    GeneratePatternAndMask(wolPatternContext.BitmapPattern,
                                           wolPatternContext.BitmapPatternSize,
                                           wolPatternContext.Mask,
                                           wolPatternContext.MaskSize,
                                           m_Random
                                           );
                    break;

                case WakePatternType.MaxSizePattern:
                    break;

                default:
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Unrecognized pattern type!"));
                    return false;
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Set pattern-specific fields"));

            //
            //  Populate generic fields
            //
            ndisPmWolPattern.Header.Type = Constants.NDIS_OBJECT_TYPE_DEFAULT;
            ndisPmWolPattern.Header.Revision = (Byte)Constants.NDIS_PM_WOL_PATTERN_REVISION_1;
            ndisPmWolPattern.Header.Size = Constants.NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION;
            ndisPmWolPattern.Priority = Constants.NDIS_PM_WOL_PRIORITY_HIGHEST;
            if (patternName == null)
            {
                testLogger.LogWarning("Pattern name is empty");
                return false;
            }
            if (patternName.Length > Constants.NDIS_PM_MAX_STRING_SIZE)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Pattern name is longer than max allowed"));
                return false;
            }
            else
            {
                for (int i = 0; i < patternName.Length; i++)
                {
                    ndisPmWolPattern.FriendlyName.String[i] = patternName[i];
                }
                ndisPmWolPattern.FriendlyName.String[patternName.Length] = '\0';
                ndisPmWolPattern.FriendlyName.Length = (ushort)(patternName.Length * 2);
            }

            // Marshal API fails to copy bytes from 177 to 192 which causes issue on v6 cases. use unsafe code instead
            // Marshal.StructureToPtr(ndisPmWolPattern, wolPatternContext.NdisPmWolPatternBuffer, false);

            unsafe {
                NDIS_PM_WOL_PATTERN* pattern = ((NDIS_PM_WOL_PATTERN*)wolPatternContext.NdisPmWolPatternBuffer);
                *pattern = ndisPmWolPattern;
            }          

            string devName = string.Format(CultureInfo.InvariantCulture, "\\DEVICE\\{{{0}}}", testAdapterInfo.AdapterGuid.ToString().ToUpper());

            if (WakeNativeMethods.AddPattern(Marshal.StringToHGlobalUni(devName), wolPatternContext.NdisPmWolPatternBuffer, wolPatternContext.NdisPmWolPatternBufferSize))
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Add pattern success"));

                if ((wolPatternContext.Type == WakePatternType.RandomPattern) || (wolPatternContext.Type == WakePatternType.MaxSizePattern))
                {
                    //output the pattern for copying to SUT, the pattern injected include eth header, but pattern transferred to SUT doesnt include such (14 bytes difference at the beginning)                    
                    bitmapPattern = new byte[wolPatternContext.BitmapPatternSize - Constants.NDT_ETHERNET_HEADER_SIZE];
                    Marshal.Copy(wolPatternContext.BitmapPattern + Constants.NDT_ETHERNET_HEADER_SIZE, bitmapPattern, 0, (int)(wolPatternContext.BitmapPatternSize - Constants.NDT_ETHERNET_HEADER_SIZE));
                }

                return true;
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Add pattern failed"));
                return false;
            }
        }

        public bool WaitConnectStatusChange(bool isConnect, TimeSpan timeOut)
        {
            if (isConnect)
            {
                WLAN_INTERFACE_STATE queryMediaValue = WLAN_INTERFACE_STATE.wlan_interface_state_disconnected;
                double timer = 0;
                while (timer < timeOut.TotalSeconds)
                {
                    Wlan.Sleep(1000);
                    queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
                    if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                    {
                        break;
                    }
                    else
                    {
                        timer++;
                    }
                }

                if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_disconnected)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                WLAN_INTERFACE_STATE queryMediaValue = WLAN_INTERFACE_STATE.wlan_interface_state_connected;
                double timer = 0;
                while (timer < timeOut.TotalSeconds)
                {
                    Wlan.Sleep(1000);
                    queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
                    if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_disconnected)
                    {
                        break;
                    }
                    else
                    {
                        timer++;
                    }
                }

                if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        // in current tests, prefer to use this in windows SUT, since we dont know the impact of using wlan api on phone
        public bool ImportProfileConnect()
        {
            byte[] profilebuffer = new byte[1000];
            byte[] namebuffer = new byte[100];
            uint profilelength = 0;
            uint namelength = 0;

            if (!WakeNativeMethods.CoreReadFile("Logs\\wlanprofile.txt", profilebuffer, out profilelength))
            {
                return false;
            }

            if (!WakeNativeMethods.CoreReadFile("Logs\\wlanprofilename.txt", namebuffer, out namelength))
            {
                return false;
            }

            if (profilelength == 0 || namelength == 0)
            {
                testLogger.LogWarning("either profile or profilename is empty");
                return false;
            }

            string profile = System.Text.Encoding.UTF8.GetString(profilebuffer, 0, (int)profilelength);
            string profileName = System.Text.Encoding.UTF8.GetString(namebuffer, 0, (int)namelength);

            WLAN_INTERFACE_STATE queryMediaValue;

            wlanApi.CreateProfile(interfaceGuid, profile);

            queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting connection to wlan profile {0}", profileName));

            wlanApi.ProfileConnect(interfaceGuid, profileName, new TimeSpan(0, 0, EnterConnectTimeOut));
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep {0} sec for connection stablized", WlanOperationInterval));
            Wlan.Sleep(WlanOperationInterval * 1000);

            queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
            if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                return true;
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                return false;
            }
        }

        public bool CreateProfileConnect(APConfig apConfig, bool isPhone, bool isAuto)
        {
            if (apConfig == null)
            {
                testLogger.LogWarning("apconfig is empty");
                return false;
            }
            string profileName = apConfig.SSID_24GHZ;
            string profile = null;
            WLAN_INTERFACE_STATE queryMediaValue;
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting creating and importing the wlan profile {0}", profileName));

            if (isAuto)
            {
                profile = apConfig.CreateProfile24ghz(ConnectionMode.Auto, false);
                wlanApi.CreateProfile(interfaceGuid, profile);
                queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0} and start waiting for auto connect", queryMediaValue.ToString()));
                if (!WaitConnectStatusChange(true, ConnectStatusTimeOut))
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Auto connect doesnt happen within {0}, start manual connect to wlan profile {1}", ConnectStatusTimeOut, profileName));
                    wlanApi.ProfileConnect(interfaceGuid, profileName, new TimeSpan(0, 0, EnterConnectTimeOut));
                }
                else
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Auto connect completed at {0}", DateTime.Now));
                }
            }
            else
            {
                profile = apConfig.CreateProfile24ghz(ConnectionMode.Manual, false);
                wlanApi.CreateProfile(interfaceGuid, profile);
                queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Starting connection to wlan profile {0}", profileName));
                wlanApi.ProfileConnect(interfaceGuid, profileName, new TimeSpan(0, 0, EnterConnectTimeOut));
            }

            //testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Sleep {0} sec for DHCPv6 stablized", DHCPStablize));
            //Wlan.Sleep(DHCPStablize * 1000);
            Wlan.Sleep(10 * 1000);

            queryMediaValue = wlanApi.GetInterfaceState(interfaceGuid);
            if (queryMediaValue == WLAN_INTERFACE_STATE.wlan_interface_state_connected)
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));

                if (!WakeNativeMethods.CoreWriteFile("Logs\\wlanprofilename.txt", Marshal.StringToCoTaskMemAnsi(profileName), (uint)profileName.Length))
                {
                    return false;
                }

                if (!WakeNativeMethods.CoreWriteFile("Logs\\wlanprofile.txt", Marshal.StringToCoTaskMemAnsi(profile), (uint)profile.Length))
                {
                    return false;
                }

                return true;
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Current connect state is {0}", queryMediaValue.ToString()));
                return false;
            }
        }

        public bool IsApBeaconing(string ssidAp, TimeSpan scanLength)
        {
            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start scanning AP {0} for {1}", ssidAp, scanLength));

            bool found = false;

            Collection<WlanNetwork> networkList = new Collection<WlanNetwork>();

            //Scan repeatedly. If we find the network then return true
            int iGetAvailableAttempt = 1;
            DateTime dtStart = DateTime.Now;

            while (DateTime.Now.Subtract(dtStart) < scanLength)
            {
                //call scan once with a flush
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Calling wlan scan for the {0} attempt with flush = true", iGetAvailableAttempt));
                wlanApi.Scan(interfaceGuid, true);
                Wlan.Sleep(5000);   // wait 5s 

                networkList.Clear();  //empty the list  before getting it again.
                networkList = wlanApi.GetAvailableNetworkList(interfaceGuid);
                foreach (var network in networkList)
                {
                    if (String.Compare(network.Ssid, ssidAp, StringComparison.OrdinalIgnoreCase) == 0)
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found AP {0} after {1} get Available network attempts.", ssidAp, iGetAvailableAttempt));
                    break;
                }
                iGetAvailableAttempt++;
                Wlan.Sleep(1000);  //sleep for 1s
            }

            //Dump the scan list if not found
            if (!found)
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Unable to find AP {0} in the scan list", ssidAp));
                testLogger.LogComment("Scan List:");
                foreach (var network in networkList)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "SSID: {0}", network.Ssid));
                }
            }

            return found;
        }

        public bool DeleteProfiles(bool isPhone)
        {
            if (wlanApi == null)
            {
                testLogger.LogWarning("Wlan object is null .. profile cleanup failed");
                return false;
            }

            try
            {
                var profileInfoList = wlanApi.GetProfileList(interfaceGuid);
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found {0} existing profiles", profileInfoList.Count));
                foreach (WlanProfileInfo profileInfo in profileInfoList)
                {
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Deleting {0}...", profileInfo));
                    wlanApi.DeleteProfile(interfaceGuid, profileInfo.ProfileName);
                }
            }
            catch (Exception e)
            {
                testLogger.LogWarning("DeleteProfiles has exception " + e.ToString());
                return false;
            }

            return true;
        }

        public static bool InitProtDriver(string DriverFullPath)
        {
            if (DriverFullPath == "")
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Please specify the driver full path"));
                return false;
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Install protocol driver at: {0}", DateTime.Now));
            if (WakeNativeMethods.PhoneProtInstall())
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Protocol driver install success"));
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Protocol driver install fails"));
                return false;
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Create service for protocol driver at: {0}", DateTime.Now));
            using (var serviceProcess = Process.StartProcess(CMD_EXE_NAME, "/C sc create ndisprot type=kernel start=system binpath=" + DriverFullPath + "ndisprot630.sys group=NDIS displayname=\"NDIS Sample Protocol\""))
            {
                serviceProcess.WaitForProcessExit();
                var retval = serviceProcess.GetProcessExitCode();
                if (retval == 1073)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "The service already exists"));
                }
                else if (retval != 0)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Create service failed with code {0}", retval));
                    return false;
                }
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Start service for protocol driver at: {0}", DateTime.Now));
            using (var serviceProcess = Process.StartProcess(CMD_EXE_NAME, "/C sc start ndisprot"))
            {
                serviceProcess.WaitForProcessExit();
                var retval = serviceProcess.GetProcessExitCode();
                if (retval == 1056)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "An instance of the service is already running"));
                }
                else if (retval != 0)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Start service failed with code {0}", retval));
                    return false;
                }
            }

            return true;
        }

        public static bool RemoveProtDriver()
        {
            bool retval = true;

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Stop service for protocol driver at: {0}", DateTime.Now));
            using (var serviceProcess = Process.StartProcess(CMD_EXE_NAME, "/C sc stop ndisprot"))
            {
                serviceProcess.WaitForProcessExit();
                var procRet = serviceProcess.GetProcessExitCode();
                if (procRet != 0)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Stop service failed with code {0}", procRet));
                    retval = false;
                }
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Delete service for protocol driver at: {0}", DateTime.Now));
            using (var serviceProcess = Process.StartProcess(CMD_EXE_NAME, "/C sc delete ndisprot"))
            {
                serviceProcess.WaitForProcessExit();
                var procRet = serviceProcess.GetProcessExitCode();
                if (procRet != 0)
                {
                    testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Delete service failed with code {0}", procRet));
                    retval = false;
                }
            }

            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Uninstall protocol driver at: {0}", DateTime.Now));
            if (WakeNativeMethods.PhoneProtUninstall())
            {
                testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Protocol driver uninstall success"));
            }
            else
            {
                testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "Protocol driver uninstall fails"));
                retval = false;
            }

            return retval;
        }

        public static void WakeTestSummary(string scenarioName, WakeEtwWatcher wlanEtwWatcher, DateTime TestStartTime, DateTime EventTime, DateTime TestEndTime, bool isPhone)
        {
            TimeSpan FinalWakeBuffer;
            string FinalRef;
           
            FinalWakeBuffer = ScheduleWakeGrace;
            FinalRef = "WCM";
           
            switch (scenarioName)
            {
                case "D2Sleep":
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test Summary:"));
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Connect event {0} times", wlanEtwWatcher.connect.Count));
                    if (wlanEtwWatcher.connect.Count > 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Reconnect happened during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Disconnect event {0} times", wlanEtwWatcher.disconnect.Count));
                    if (wlanEtwWatcher.disconnect.Count > 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Connection lost during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweroff event {0} times", wlanEtwWatcher.poweroff.Count));
                    if (wlanEtwWatcher.poweroff.Count > 1)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device go to D2 multiple times"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device failed go to D2"));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device took {0} sec to enter D2", wlanEtwWatcher.poweroff[0].TimeStamp - TestStartTime));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweron event {0} times", wlanEtwWatcher.poweron.Count));
                    if (wlanEtwWatcher.poweron.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by schedule"));
                    }
                    else if (wlanEtwWatcher.poweron.Count > 1)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device waked up multiple times, see NicActive and WakeReason summary"));
                    }
                    else
                    {
                        if ((wlanEtwWatcher.poweron[0].TimeStamp >= TestEndTime) && (wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime + FinalWakeBuffer))
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device waked in scheduled time frame at {0}", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device waked up out of scheduled time frame at {0} {1} {2}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp, TestEndTime, TestEndTime + FinalWakeBuffer));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found NicActive event {0} times", wlanEtwWatcher.nicactive.Count));
                    if (wlanEtwWatcher.nicactive.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----No nic ref acquiring event happened"));
                    }
                    else
                    {
                        Dictionary<string, int> nicActiveCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.nicactive);
                        foreach (KeyValuePair<string, int> kvp in nicActiveCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by component {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.nicactive.Count == 1 && wlanEtwWatcher.nicactive[0].Keywords.Contains(FinalRef)))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Either wake is not from {0} or there are wrong amount of wakes", FinalRef));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found WakeReason event {0} times", wlanEtwWatcher.wakereason.Count));
                    if (wlanEtwWatcher.wakereason.Count > 0)
                    {
                        Dictionary<string, int> wakeReasonCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.wakereason);
                        foreach (KeyValuePair<string, int> kvp in wakeReasonCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by reason {0} {1} times", kvp.Key, kvp.Value));
                        }
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Wake by WLAN wake events, driver team needs to take look at trace"));
                    }
                    break;

                case "IncomingPKT":
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test Summary:"));
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Connect event {0} times", wlanEtwWatcher.connect.Count));
                    if (wlanEtwWatcher.connect.Count > 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Reconnect happened during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Disconnect event {0} times", wlanEtwWatcher.disconnect.Count));
                    if (wlanEtwWatcher.disconnect.Count > 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Connection lost during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweroff event {0} times", wlanEtwWatcher.poweroff.Count));
                    if (wlanEtwWatcher.poweroff.Count == 1)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device power off after wake didnt happen during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device failed go to D2"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count > 2)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device power off more than 2 times during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff[1].TimeStamp >= TestEndTime || wlanEtwWatcher.poweroff[1].TimeStamp <= EventTime)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----poweroff happened outside scheduled CS session at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device took {0} sec to enter D2", wlanEtwWatcher.poweroff[0].TimeStamp - TestStartTime));
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----2nd poweroff happened within scheduled time frame at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweron event {0} times", wlanEtwWatcher.poweron.Count));
                    if (wlanEtwWatcher.poweron.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by schedule and wake event"));
                    }
                    else if (wlanEtwWatcher.poweron.Count == 1)
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp <= EventTime)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Waked before sending wake packet at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Waked only once at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by wake event"));
                        }
                    }
                    else if (wlanEtwWatcher.poweron.Count > 2)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device power on more than 2 times during CS session, see NicActive and WakeReason summary"));
                    }
                    else
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp >= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp <= TestEndTime + FinalWakeBuffer)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device wake during CS session at {0}", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----one of or both wake happened out of scheduled time frame at {0}, {1}", wlanEtwWatcher.poweron[0].TimeStamp, wlanEtwWatcher.poweron[1].TimeStamp));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found NicActive event {0} times", wlanEtwWatcher.nicactive.Count));
                    if (wlanEtwWatcher.nicactive.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----No nic ref acquiring event happened"));
                    }
                    else
                    {
                        Dictionary<string, int> nicActiveCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.nicactive);
                        foreach (KeyValuePair<string, int> kvp in nicActiveCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by component {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.nicactive.Count == 1 && wlanEtwWatcher.nicactive[0].Keywords.Contains(FinalRef)))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Either wake is not from {0} or there are wrong amount of wakes", FinalRef));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found WakeReason event {0} times", wlanEtwWatcher.wakereason.Count));
                    if (wlanEtwWatcher.wakereason.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Cannot find any wake reason"));
                    }
                    else
                    {
                        Dictionary<string, int> wakeReasonCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.wakereason);
                        foreach (KeyValuePair<string, int> kvp in wakeReasonCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by reason {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.wakereason.Count == 1 && wlanEtwWatcher.wakereason[0].Keywords.Contains("Incoming Packet")))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Either wake reason is not Incoming Packet or there are wrong amount of wakes"));
                        }
                    }
                    break;

                case "AssociateLost":
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test Summary:"));
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Connect event {0} times", wlanEtwWatcher.connect.Count));
                    if (wlanEtwWatcher.connect.Count > 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Reconnect happened during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Disconnect event {0} times", wlanEtwWatcher.disconnect.Count));
                    if (wlanEtwWatcher.disconnect.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----No disconnect happened"));
                    }
                    else if (wlanEtwWatcher.disconnect.Count > 1)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Multiple disconnect happened"));
                    }
                    else if (wlanEtwWatcher.disconnect[0].TimeStamp >= TestEndTime || wlanEtwWatcher.disconnect[0].TimeStamp <= EventTime)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Disconnect happened outside scheduled time frame at {0}", wlanEtwWatcher.disconnect[0].TimeStamp));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Disconnect took {0} sec to occur since radio off", wlanEtwWatcher.disconnect[0].TimeStamp - EventTime));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweroff event {0} times", wlanEtwWatcher.poweroff.Count));
                    if (wlanEtwWatcher.poweroff.Count == 1)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power off after wake didnt happen during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Device failed go to D2; Not a device Driver issue; Check OS componenets or retry"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count > 2)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power off more than 2 times during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff[1].TimeStamp >= TestEndTime || wlanEtwWatcher.poweroff[1].TimeStamp <= EventTime)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----poweroff happened outside scheduled time frame at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device took {0} sec to enter D2", wlanEtwWatcher.poweroff[0].TimeStamp - TestStartTime));
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----2nd poweroff happened within scheduled time frame at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweron event {0} times", wlanEtwWatcher.poweron.Count));
                    if (wlanEtwWatcher.poweron.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by schedule and wake event"));
                    }
                    else if (wlanEtwWatcher.poweron.Count == 1)
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp <= EventTime)
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Waked before radio off at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime)
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Waked only once at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by wake event"));
                        }
                    }
                    else if (wlanEtwWatcher.poweron.Count > 2)
                    {                    
						//
						// D0 more than 2 times during CS is not necessary wrong. Now 1) some moulde, e.g. CDE will take an Ref in wake to D0 and 
						// 2) WFD may an active ref right we go Dx.
						//
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power on more than 2 times during CS session, see NicActive and WakeReason summary"));
                    }
                    else
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp >= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp <= TestEndTime + FinalWakeBuffer)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device wake during CS session at {0}", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----one of or both wake happened out of scheduled time frame at {0}, {1}", wlanEtwWatcher.poweron[0].TimeStamp, wlanEtwWatcher.poweron[1].TimeStamp));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found NicActive event {0} times", wlanEtwWatcher.nicactive.Count));
                    if (wlanEtwWatcher.nicactive.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----No nic ref acquiring event happened"));
                    }
                    else
                    {
                        Dictionary<string, int> nicActiveCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.nicactive);
                        foreach (KeyValuePair<string, int> kvp in nicActiveCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by component {0} {1} times", kvp.Key, kvp.Value));
                        }

						testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** NicActive Count={0}", wlanEtwWatcher.nicactive.Count));
						for ( int i=0; i < wlanEtwWatcher.nicactive.Count; i++ )
						{
							testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** NicActive KeyWord[{0}]={1}",i,  wlanEtwWatcher.nicactive[0].Keywords));							
						}
							
						
                        if (!(wlanEtwWatcher.nicactive.Count >= 1 && wlanEtwWatcher.nicactive[0].Keywords.Contains("Wireless LAN")))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Either wake is not from Wireless LAN and {0} or there are wrong amount of wakes", FinalRef));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found WakeReason event {0} times", wlanEtwWatcher.wakereason.Count));
                    if (wlanEtwWatcher.wakereason.Count == 0)
                    {
                        testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Cannot find any wake reason"));
                    }
                    else
                    {
                        Dictionary<string, int> wakeReasonCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.wakereason);
                        foreach (KeyValuePair<string, int> kvp in wakeReasonCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by reason {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.wakereason.Count == 1 && wlanEtwWatcher.wakereason[0].Keywords.Contains("AP Association Lost")))
                        {
                            testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "-----Either wake reason is not AP Association Lost or there are wrong amount of wakes"));
                        }
                    }
                    break;

                case "NLODiscovery":

					//
					// change LogError to LogWarning up to the new State machine to decide pass or fail. State machine should be more accurate for the decision.
					//
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Test Summary:"));
                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Connect event {0} times", wlanEtwWatcher.connect.Count));
                    if (wlanEtwWatcher.connect.Count == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----No connect happened"));
                    }
                    else if (wlanEtwWatcher.connect.Count > 1)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Multiple disconnect happened"));
                    }
                    else if (wlanEtwWatcher.connect[0].TimeStamp >= TestEndTime || wlanEtwWatcher.connect[0].TimeStamp <= EventTime)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Connect happened outside scheduled time frame at {0}", wlanEtwWatcher.connect[0].TimeStamp));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Connect took {0} sec to occur since radio on", wlanEtwWatcher.connect[0].TimeStamp - EventTime));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Disconnect event {0} times", wlanEtwWatcher.disconnect.Count));
                    if (wlanEtwWatcher.disconnect.Count > 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----disconnect happened during test"));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweroff event {0} times", wlanEtwWatcher.poweroff.Count));
                    if (wlanEtwWatcher.poweroff.Count == 1)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power off after wake didnt happen during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device failed go to D2"));
                    }
                    else if (wlanEtwWatcher.poweroff.Count > 2)
                    {
                       	//
                    	// Dx more than 2 times during CS is not necessary wrong. Now 1) some moulde, e.g. CDE will take an Ref in wake to D0 and 
                    	// 2) WFD may an active ref right we go Dx.
                    	//
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power off more than 2 times during CS session"));
                    }
                    else if (wlanEtwWatcher.poweroff[1].TimeStamp >= TestEndTime || wlanEtwWatcher.poweroff[1].TimeStamp <= EventTime)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----poweroff happened outside scheduled time frame at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }
                    else
                    {
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device took {0} sec to enter D2", wlanEtwWatcher.poweroff[0].TimeStamp - TestStartTime));
                        testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----2nd poweroff happened within scheduled time frame at {0}", wlanEtwWatcher.poweroff[1].TimeStamp));
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found Poweron event {0} times", wlanEtwWatcher.poweron.Count));
                    if (wlanEtwWatcher.poweron.Count == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by schedule and wake event"));
                    }
                    else if (wlanEtwWatcher.poweron.Count == 1)
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp <= EventTime)
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Waked before radio on at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime)
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Waked only once at {0}, see NicActive and WakeReason summary", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Failed to wake by wake event"));
                        }
                    }
                    else if (wlanEtwWatcher.poweron.Count > 2)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Device power on more than 2 times during CS session, see NicActive and WakeReason summary"));
                    }
                    else
                    {
                        if (wlanEtwWatcher.poweron[0].TimeStamp >= EventTime && wlanEtwWatcher.poweron[0].TimeStamp <= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp >= TestEndTime && wlanEtwWatcher.poweron[1].TimeStamp <= TestEndTime + FinalWakeBuffer)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Device wake during CS session at {0}", wlanEtwWatcher.poweron[0].TimeStamp));
                        }
                        else
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----one of or both wake happened out of scheduled time frame at {0}, {1}", wlanEtwWatcher.poweron[0].TimeStamp, wlanEtwWatcher.poweron[1].TimeStamp));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found NicActive event {0} times", wlanEtwWatcher.nicactive.Count));
                    if (wlanEtwWatcher.nicactive.Count == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----No nic ref acquiring event happened"));
                    }
                    else
                    {
                        Dictionary<string, int> nicActiveCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.nicactive);
                        foreach (KeyValuePair<string, int> kvp in nicActiveCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by component {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.nicactive.Count == 2 && wlanEtwWatcher.nicactive[0].Keywords.Contains("Wireless LAN") && wlanEtwWatcher.nicactive[1].Keywords.Contains(FinalRef)))
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Either wake is not from Wireless LAN and {0} or there are wrong amount of wakes", FinalRef));
                        }
                    }

                    testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "Found WakeReason event {0} times", wlanEtwWatcher.wakereason.Count));
                    if (wlanEtwWatcher.wakereason.Count == 0)
                    {
                        testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Cannot find any wake reason"));
                    }
                    else if (wlanEtwWatcher.wakereason.Count > 1)
                    {
                        Dictionary<string, int> wakeReasonCount = WakeHelper.ParseWakeEvent(wlanEtwWatcher.wakereason);
                        foreach (KeyValuePair<string, int> kvp in wakeReasonCount)
                        {
                            testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "-----Wake by reason {0} {1} times", kvp.Key, kvp.Value));
                        }

                        if (!(wlanEtwWatcher.wakereason.Count == 1 && wlanEtwWatcher.wakereason[0].Keywords.Contains("Network List Offload Discovery")))
                        {
                            testLogger.LogWarning(string.Format(CultureInfo.InvariantCulture, "-----Either wake reason is not Network List Offload Discovery or there are wrong amount of wakes"));
                        }
                    }
					//
					// analyze all events; use state machine to be more accurate in deciding Success or Failure
					/*
					eWakeEventTypeUnknown = 0,
					eWakeEventTypeConnect,
					eWakeEventTypeDisconnect,
					eWakeEventTypePowerOff,
					eWakeEventTypePowerOn,
					eWakeEventTypeNicActive,
					eWakeEventTypeWakeReason,
					eWakeEventTypeWwIrpCompleted,
					eWakeEventTypeNloDiscovery,
					*/
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event Connect count	{0}", wlanEtwWatcher.connect.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event Disconnect count {0}", wlanEtwWatcher.disconnect.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event PowerOff count	{0}", wlanEtwWatcher.poweroff.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event PowerOn count	{0}", wlanEtwWatcher.poweron.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event NicActive count	{0}", wlanEtwWatcher.nicactive.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event WakeReason count {0}", wlanEtwWatcher.wakereason.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event WWIrp count		{0}", wlanEtwWatcher.wwIrpCompleted.Count));
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** Event NloDiscovery 	{0}", wlanEtwWatcher.nloDiscovery.Count));
					
					int inConnect = -1;
					int nicActive = 0;
					bool bPowerOn = false;
					bool bNicWake = false;
					bool bWakeForCorrectReason = false;
					bool bExpectedConnect = false;
					
					if ( wlanEtwWatcher.poweroff.Count == 0  ) {
						testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "NIC never receives Dx. This is probably an OS issue. Rerun the test"));
					}				
					
					testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, "***** [inConnect, bD0, nicActive]"));
					foreach ( WakeEventObject ev in wlanEtwWatcher.allEvents ) {
						switch ( ev.GetEventType()) {
							case WakeEventType.eWakeEventTypeConnect:
								inConnect = 1;
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) [{1}, {2}, {3}] Get Connect", ev.TimeStamp, inConnect, bPowerOn, nicActive));
								if ( bWakeForCorrectReason ) {
									bExpectedConnect = true;
								}
								break;
					
							case WakeEventType.eWakeEventTypeDisconnect:
								inConnect = 0;
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) [{1}, {2}, {3}] Get Disconnect", ev.TimeStamp, inConnect, bPowerOn, nicActive));
								break;
					
							case WakeEventType.eWakeEventTypePowerOff:
								bPowerOn = false;
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) [{1}, {2}, {3}] Get Dx", ev.TimeStamp, inConnect, bPowerOn, nicActive));
								break;
					
							case WakeEventType.eWakeEventTypePowerOn:
								bPowerOn = true;
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) [{1}, {2}, {3}] Get D0", ev.TimeStamp, inConnect, bPowerOn, nicActive));
								break;
								
							case WakeEventType.eWakeEventTypeNicActive:
								nicActive++;
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) [{1}, {2}, {3}] Get NicActive", ev.TimeStamp,  inConnect, bPowerOn, nicActive));
								break;
					
							case WakeEventType.eWakeEventTypeWakeReason:
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) Get WakeReason {1}", ev.TimeStamp, ev.Keywords));
								if ( ev.Keywords.Contains("Network List Offload Discovery") ) {
									bWakeForCorrectReason = true;
								}
								break;
					
							case WakeEventType.eWakeEventTypeWwIrpCompleted:
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) Get WwIrpCompleted", ev.TimeStamp));
								bNicWake = true;
								break;
					
							case WakeEventType.eWakeEventTypeNloDiscovery:
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) Get NloDiscovery", ev.TimeStamp));
								break;
					
							default:
								testLogger.LogComment(string.Format(CultureInfo.InvariantCulture, " 	 ({0}) Get Unknown event", ev.TimeStamp));
								break;
								
						}
					}	
					if ( !bExpectedConnect ) {
						testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Expect NIC to connect, but did not happen"));
					}
					
					if ( !bNicWake ) {
						testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Expect NIC to wake, but did not happen. May be the OS put NIC in D0. Rerun test"));
					}
					
					if ( !bWakeForCorrectReason ) {
						if ( !bNicWake ) {
							testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "NIC did not report wake reason as expected, May be the OS put NIC in D0 too soon. Rerun test"));
						} else {
							testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Nic woke, but did not report wake reason as expected or wrong reason"));	
						}
					}
					break;

                default:
                    testLogger.LogError(string.Format(CultureInfo.InvariantCulture, "Wrong test scenario {0}", scenarioName));
                    break;
            }
        }
    }
}
