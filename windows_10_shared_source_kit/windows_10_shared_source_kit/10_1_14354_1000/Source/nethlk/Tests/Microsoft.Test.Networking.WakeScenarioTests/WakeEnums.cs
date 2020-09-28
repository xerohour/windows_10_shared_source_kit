///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

#pragma warning disable 0169, 0414, 0649

namespace HckTests
{
    public class Constants
    {
        public const int NDIS_PM_MAX_STRING_SIZE = 64;
        public const int NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION = 196; //size of  NDIS_PM_WOL_PATTERN, use Marshal.SizeOf(typeof()) to validate

        public const int MAX_PAYLOAD_LINE_LEN = 16;
        public const int NDT_ETHERNET_HEADER_SIZE = 14;
        public const byte NDIS_OBJECT_TYPE_DEFAULT = 0x80;    // used when object type is implicit in the API call

        public const int NDIS_PM_WOL_PATTERN_REVISION_1 = 1;

        //  
        // Values used in Priority field of NDIS_PM_WOL_PATTERN  
        //  
        public const uint NDIS_PM_WOL_PRIORITY_HIGHEST = 0x00000001;  
    }

    public enum WoLSupportedPatterns
    {
        NDIS_PM_WOL_BITMAP_PATTERN_SUPPORTED = 0x00000001,
        NDIS_PM_WOL_MAGIC_PACKET_SUPPORTED = 0x00000002,
        NDIS_PM_WOL_IPV4_TCP_SYN_SUPPORTED = 0x00000004,
        NDIS_PM_WOL_IPV6_TCP_SYN_SUPPORTED = 0x00000008,
        NDIS_PM_WOL_IPV4_DEST_ADDR_WILDCARD_SUPPORTED = 0x00000200,
        NDIS_PM_WOL_IPV6_DEST_ADDR_WILDCARD_SUPPORTED = 0x00000800,
        NDIS_PM_WOL_EAPOL_REQUEST_ID_MESSAGE_SUPPORTED = 0x00010000,
    }

    public enum WoLSupportedEvents
    {
        NDIS_WLAN_WAKE_ON_NLO_DISCOVERY_SUPPORTED = 0x00000001,
        NDIS_WLAN_WAKE_ON_AP_ASSOCIATION_LOST_SUPPORTED = 0x00000002,
        NDIS_WLAN_WAKE_ON_GTK_HANDSHAKE_ERROR_SUPPORTED = 0x00000004,
        NDIS_WLAN_WAKE_ON_4WAY_HANDSHAKE_REQUEST_SUPPORTED = 0x00000008,
    }

    public enum WakePatternType  
    {  
        Unknown            = 0,  
        IPv4_TCP_Syn       = 1,  
        IPv6_TCP_Syn       = 2,  
        IPv4_Wildcard      = 3,  
        IPv6_Wildcard      = 4,  
        EAPRequest         = 5,  
        RandomPattern      = 6,  
        MaxSizePattern     = 7,  
        Max                = 8,  
    }

    public enum NDIS_PM_WOL_PACKET
    {
        NdisPMWoLPacketUnspecified,
        NdisPMWoLPacketBitmapPattern,
        NdisPMWoLPacketMagicPacket,
        NdisPMWoLPacketIPv4TcpSyn,
        NdisPMWoLPacketIPv6TcpSyn,
        NdisPMWoLPacketEapolRequestIdMessage,
        NdisPMWoLPacketMaximum
    }

    public enum DEVICE_POWER_STATE
    {
        PowerDeviceUnspecified = 0,
        PowerDeviceD0,
        PowerDeviceD1,
        PowerDeviceD2,
        PowerDeviceD3,
        PowerDeviceMaximum,
    }

    public enum SYSTEM_POWER_STATE
    {
        PowerSystemUnspecified = 0,
        PowerSystemWorking = 1,
        PowerSystemSleeping1 = 2,
        PowerSystemSleeping2 = 3,
        PowerSystemSleeping3 = 4,
        PowerSystemHibernate = 5,
        PowerSystemShutdown = 6,
        PowerSystemMaximum = 7,
    }

    public struct NDIS_OBJECT_HEADER
    {
        public byte Type;
        public byte Revision;
        public ushort Size;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BATTERY_REPORTING_SCALE
    {
        public uint Granularity;
        public uint Capacity;
    }

    public struct NDIS_PM_PARAMETERS
    {
        //      
        // Header.Type = NDIS_OBJECT_TYPE_DEFAULT;      
        // Header.Revision = NDIS_PM_PARAMETERS_REVISION_2;     
        // Header.Size = NDIS_SIZEOF_NDIS_PM_PARAMETERS_REVISION_2;      
        //      
        public NDIS_OBJECT_HEADER Header;

        public uint EnabledWoLPacketPatterns; // NDIS_PM_WOL_XXX_ENABLED flags      
        public uint EnabledProtocolOffloads;  // NDIS_PM_PROTOCOL_OFFLOAD_XXX_ENABLED flags      
        public uint WakeUpFlags;              // NDIS_PM_WAKE_ON_XXX_ENABLED flags 

        //#if (NDIS_SUPPORT_NDIS630)  
        public uint MediaSpecificWakeUpEvents; // NDIS_{WLAN|WWAN}_WAKE_ON_XXX_ENABLED flags  
        //#endif // (NDIS_SUPPORT_NDIS630)  
    }

    public struct NDIS_PM_CAPABILITIES
    {
        //      
        // Header.Type = NDIS_OBJECT_TYPE_DEFAULT;    
        // Header.Revision = NDIS_PM_CAPABILITIES_REVISION_2;    
        // Header.Size = NDIS_SIZEOF_NDIS_PM_CAPABILITIES_REVISION_2;    
        //    
        NDIS_OBJECT_HEADER Header;

        uint Flags;                          // NDIS_PM_XXX_SUPPORTED flags 
        public uint SupportedWoLPacketPatterns;     // NDIS_PM_WOL_XXX_SUPPORTED flags     
        public uint NumTotalWoLPatterns;
        public uint MaxWoLPatternSize;              // maximum bytes that can be compared against a pattern   
        uint MaxWoLPatternOffset;            // strting from MAC header, how many bytes in the packet can be examined   
        uint MaxWoLPacketSaveBuffer;         // how many bytes of WOL packet can be saved to a buffer and indicated up   
        uint SupportedProtocolOffloads;      // NDIS_PM_PROTOCOL_OFFLOAD_XXX_SUPPORTED flags    
        uint NumArpOffloadIPv4Addresses;
        uint NumNSOffloadIPv6Addresses;
        DEVICE_POWER_STATE MinMagicPacketWakeUp;
        DEVICE_POWER_STATE MinPatternWakeUp;
        DEVICE_POWER_STATE MinLinkChangeWakeUp;

        //#if (NDIS_SUPPORT_NDIS630)  

        uint SupportedWakeUpEvents;          // NDIS_PM_WAKE_ON_XXX_SUPPORTED flags    
        public uint MediaSpecificWakeUpEvents;      // NDIS_{WLAN|WWAN}_WAKE_ON_XXX_SUPPORTED flags    
        //#endif // (NDIS_SUPPORT_NDIS630)    
    }

    public struct NDIS_PM_COUNTED_STRING
    {
        public ushort Length; // in -Bytes-      
        public unsafe fixed char String[Constants.NDIS_PM_MAX_STRING_SIZE + 1];
    }

    //          
    // IPv4 TCP SYN information  sizeof = 16        
    //
    public struct IPV4_TCP_SYN_WOL_PACKET_PARAMETERS
    {
        public uint Flags;
        public unsafe fixed byte IPv4SourceAddress[4];   // IPv4 source address              
        public unsafe fixed byte IPv4DestAddress[4];     // IPv4 destination address              
        public ushort TCPSourcePortNumber;    // TCP source port             
        public ushort TCPDestPortNumber;      // TCP destination port    
    }

    //          
    // IPv6 TCP SYN information    sizeof = 40   
    //          
    public struct IPV6_TCP_SYN_WOL_PACKET_PARAMETERS
    {
        public uint Flags;
        public unsafe fixed byte IPv6SourceAddress[16];  // source IPv6 address              
        public unsafe fixed byte IPv6DestAddress[16];    // destination IPv6 address              
        public ushort TCPSourcePortNumber;    // source TCP port              
        public ushort TCPDestPortNumber;      // destination TCP port          
    }

    //          
    // 802.1X EAPOL request identity message parameters          sizeof = 4
    //          
    public struct EAPOL_REQUEST_ID_MESSAGE_WOL_PACKET_PARAMETERS
    {
        public uint Flags;
    }

    public struct WOL_BITMAP_PATTERN  //sizeof = 20
    {
        public uint Flags;
        public uint MaskOffset;     // offset for mask buffer from the beginning of NDIS_PM_WOL_PATTERN structure              
        public uint MaskSize;       // Mask size              
        public uint PatternOffset;  // offset for pattern buffer from the beginning of NDIS_PM_WOL_PATTERN structure              
        public uint PatternSize;    // pattern size          
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct NDIS_PM_WOL_PATTERN
    {
        //      
        // Header.Type = NDIS_OBJECT_TYPE_DEFAULT;      
        // Header.Revision = NDIS_PM_WOL_PATTERN_REVISION_2;      
        // Header.Size = NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION_2;      
        //  
        [FieldOffset(0)]
        public NDIS_OBJECT_HEADER Header;

        [FieldOffset(4)]
        public uint Flags;
        [FieldOffset(8)]
        public uint Priority;
        [FieldOffset(12)]
        public NDIS_PM_WOL_PACKET WoLPacketType;
        [FieldOffset(16)]
        public NDIS_PM_COUNTED_STRING FriendlyName;
        [FieldOffset(148)]
        public uint PatternId;   // Pattern ID set by NDIS  
        [FieldOffset(152)]
        public uint NextWoLPatternOffset;

        [FieldOffset(156)]
        public IPV4_TCP_SYN_WOL_PACKET_PARAMETERS IPv4TcpSynParameters;

        [FieldOffset(156)]
        public IPV6_TCP_SYN_WOL_PACKET_PARAMETERS IPv6TcpSynParameters;

        [FieldOffset(156)]
        public EAPOL_REQUEST_ID_MESSAGE_WOL_PACKET_PARAMETERS EapolRequestIdMessageParameters;

        [FieldOffset(156)]
        public WOL_BITMAP_PATTERN WoLBitMapPattern;
    }

    public class PM_WolPatternContext
    {
        public WakePatternType Type;
        public IntPtr NdisPmWolPatternBuffer;
        public uint NdisPmWolPatternBufferSize;
        public IntPtr BitmapPattern;
        public uint BitmapPatternSize;
        public IntPtr Mask;
        public uint MaskSize;
        public byte[] SrcIPv4;
        public byte[] DestIPv4;
        public byte[] SrcIPv6;
        public byte[] DestIPv6;
        bool NoFailIfPatternListFull;

        public PM_WolPatternContext()
        {
            Type = WakePatternType.Unknown;
            NdisPmWolPatternBuffer = IntPtr.Zero;
            NdisPmWolPatternBufferSize = 0;
        }

        public PM_WolPatternContext(WakePatternType type)
        {
            Type = type;
            NdisPmWolPatternBuffer = IntPtr.Zero;
            NdisPmWolPatternBufferSize = 0;
            BitmapPattern = IntPtr.Zero;
            BitmapPatternSize = 0;
            Mask = IntPtr.Zero;
            MaskSize = 0;
            NoFailIfPatternListFull = false;
            SrcIPv4 = new byte[4];
            DestIPv4 = new byte[4];
            SrcIPv6 = new byte[16];
            DestIPv6 = new byte[16];
        }

        ~PM_WolPatternContext()
        {
            if (NdisPmWolPatternBuffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(NdisPmWolPatternBuffer); 
            }
        }
    }
}