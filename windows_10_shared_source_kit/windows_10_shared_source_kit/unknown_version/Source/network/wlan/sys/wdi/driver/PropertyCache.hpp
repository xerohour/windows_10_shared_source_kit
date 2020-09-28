#pragma once


typedef enum _WFC_PROPERTY_TYPE
{
    WfcPropertyTypeInvalid = 0,
    WfcPropertyTypeULong,
    WfcPropertyTypeBoolean,
    WfcPropertyTypeUchar,
    WfcPropertyTypeVariableBuffer,
    WfcPropertyTypeVariableBufferPtr,
    WfcPropertyTypePropertyCacheList,
    WfcPropertyTypeList
} WFC_PROPERTY_TYPE;

typedef struct _WFC_PROPERTY_LOAD_INFO
{
    UINT32 PropertyName;
    WFC_PROPERTY_TYPE WfcPropertyType;
    UINT32 ElementSourceOffset;
    UINT32 ElementSourceSize;
    // if the value of WfcPropertyType is WfcPropertyTypeVariableBuffer or WfcPropertyTypeVariableBufferPtr, then ElementCountSourceOffset is the offset to a 
    // field that has the value of element count in the variable sized buffer
    UINT32 ElementCountSourceOffset;
    UINT32 ElementCountSourceSize;
} WFC_PROPERTY_LOAD_INFO, *PWFC_PROPERTY_LOAD_INFO;

typedef ULONG WFC_PROPERTY_NAME;
const WFC_PROPERTY_NAME WfcPropertyNameInvalid = 0xffffffff;

//--------------
typedef enum _WFC_ADAPTER_PROPERTY_NAME : WFC_PROPERTY_NAME
{
    //dot11
    WfcAdapterPropertyInfraSupportedUcastAlgoPairs = 0,
    WfcAdapterPropertyInfraSupportedMcastAlgoPairs,
    WfcAdapterPropertyBandInfo,
    WfcAdapterPropertyPhyInfo,
    WfcAdapterPhyTypeDot11PhyIDMap,     // Saved here for simplicity of processing
    
    //DOT11_EXTSTA_CAPABILITY
    WfcAdapterPropertyScanSSIDListSize,
    WfcAdapterPropertyDesiredBSSIDListSize,
    WfcAdapterPropertyDesiredSSIDListSize,
//    WfcAdapterPropertyExcludedMacAddressListSize,
    WfcAdapterPropertyPrivacyExemptionListSize,
    WfcAdapterPropertyKeyMappingTableSize,
    WfcAdapterPropertyDefaultKeyTableSize,
    WfcAdapterPropertyWEPKeyValueMaxLength,
//    WfcAdapterPropertyPMKIDCacheSize,
    WfcAdapterPropertyMaxNumPerSTADefaultKeyTables,
    WfcAdapterPropertyBSSListCachemanagement,
    WfcAdapterPropertyConnectBSSSelectionOverride,
    WfcAdapterPropertyHESSIDConnectionSupported,

    WfcAdapterPropertyMaxCommandSize,
    
    //general
    WfcAdapterPropertyMaxMulticastListSize,
    WfcAdapterPropertyPermanentMacAddress,
    WfcAdapterPropertySupportedOpModes,
    WfcAdapterResetRecoveryOff,
    WfcAdapterTaskTimeout,
    WfcAdapterCommandTimeout,
    WfcAdapterPropertyRegUnreachableThreshold,
    WfcAdapterPropertyEnhancedCapabilities,
    WfcAdapterPropertyBandCapabilities,
    WfcAdapterPropertyActionFramesSupported,
    WfcAdapterPropertyMACRandomizationSupported,
    WfcAdapterPropertyNonWdiOidsSupported,
    
    //PM
    WfcAdapterPMCapabilitiesFlags,                 
    WfcAdapterPMCapabilitiesSupportedWoLPacketPatterns,
    WfcAdapterPMCapabilitiesNumTotalWoLPatterns,      
    WfcAdapterPMCapabilitiesMaxWoLPatternSize,        
    WfcAdapterPMCapabilitiesMaxWoLPatternOffset,      
    WfcAdapterPMCapabilitiesMaxWoLPacketSaveBuffer,   
    WfcAdapterPMCapabilitiesSupportedProtocolOffloads,
    WfcAdapterPMCapabilitiesNumArpOffloadIPv4Addresses,
    WfcAdapterPMCapabilitiesNumNSOffloadIPv6Addresses,
    WfcAdapterPMCapabilitiesMinMagicPacketWakeUp,     
    WfcAdapterPMCapabilitiesMinPatternWakeUp,         
    WfcAdapterPMCapabilitiesMinLinkChangeWakeUp,      
    WfcAdapterPMCapabilitiesSupportedWakeUpEvents,    
    WfcAdapterPMCapabilitiesMediaSpecificWakeUpEvents,
    
    WfcAdapterCurrentPhyID,
    WfcAdapterPacketCoalescing,
    WfcAdapterHardwareRadioOff,
    WfcAdapterSoftwareRadioOff,

    WfcAdapterPropertyRegTest_FeatureMask,
    WfcAdapterPropertyRegTest_BSSChannel,

    WfcAdapterProperty_LastActionFrameIndicationEventBuffer,
    WfcAdapterProperty_LastAssociationRequestEventBuffer,

    WfcAdapterPropertyCurrentListenState,
    WfcAdapterPropertyMediaStreamingRefcount,

    WfcAdapterPropertyRegRoamControlParametersIndex,
    WfcAdapterPropertyRegRoamLQThreshold,
    WfcAdapterPropertyRegRoamLQChangeBar, // 50
    WfcAdapterPropertyRegRoamTimeGap,
    WfcAdapterPropertyRegRoamAllowScanCancel,
    WfcAdapterPropertyRegRoamFiveGhzParams,
    WfcAdapterPropertyRegRoamThrottleRoamingNeeded,
    WfcAdapterPropertyRegRoamFullPassiveScan,
    WfcAdapterPropertyRegMaxBssEntryCount,
    WfcAdapterPropertyRegHungAtWdiCommandPasses,
    WfcAdapterPropertyRegHungResetRecoveryIterations,
    WfcAdapterPropertyTestEnableResetRecovery,
    WfcAdapterPropertyRegMaxBackgroundScanSkipCount,
    //
    //++instconn
    //
    WfcAdapterPropertyStaLastConnectedChannel,
    WfcAdapterPropertyStaLastConnectedBandId,
    //--

    //offload Capabilities
    
    //Checksum
    WfcAdapterPropertyChecksumV4TxEncapsulation,
    WfcAdapterPropertyChecksumV4TxIpOptionsSupported,
    WfcAdapterPropertyChecksumV4TxTcpOptionsSupported,
    WfcAdapterPropertyChecksumV4TxTcpChecksum,
    WfcAdapterPropertyChecksumV4TxUdpChecksum,
    WfcAdapterPropertyChecksumV4TxIpChecksum,

    WfcAdapterPropertyChecksumV4RxEncapsulation,
    WfcAdapterPropertyChecksumV4RxIpOptionsSupported,
    WfcAdapterPropertyChecksumV4RxTcpOptionsSupported,
    WfcAdapterPropertyChecksumV4RxTcpChecksum,
    WfcAdapterPropertyChecksumV4RxUdpChecksum,
    WfcAdapterPropertyChecksumV4RxIpChecksum,

    WfcAdapterPropertyChecksumV6TxEncapsulation,
    WfcAdapterPropertyChecksumV6TxIpExtensionHeadersSupported,
    WfcAdapterPropertyChecksumV6TxTcpOptionsSupported,
    WfcAdapterPropertyChecksumV6TxTcpChecksum,
    WfcAdapterPropertyChecksumV6TxUdpChecksum,

    WfcAdapterPropertyChecksumV6RxEncapsulation,
    WfcAdapterPropertyChecksumV6RxIpExtensionHeadersSupported,
    WfcAdapterPropertyChecksumV6RxTcpOptionsSupported,
    WfcAdapterPropertyChecksumV6RxTcpChecksum,
    WfcAdapterPropertyChecksumV6RxUdpChecksum,

    WfcAdapterPropertyLargeSendOffloadV1Encapsulation,

    WfcAdapterPropertyLargeSendOffloadV1MaxOffLoadSize,
    WfcAdapterPropertyLargeSendOffloadV1MinSegmentCount,
    WfcAdapterPropertyLargeSendOffloadV1TcpOptions,
    WfcAdapterPropertyLargeSendOffloadV1IpOptions,

    WfcAdapterPropertyLargeSendOffloadV2EncapsulationV4,
    WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV4,
    WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV4,

    WfcAdapterPropertyLargeSendOffloadV2EncapsulationV6,
    WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV6,
    WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV6,
    WfcAdapterPropertyLargeSendOffloadV2IpExtensionHeadersSupported,
    WfcAdapterPropertyLargeSendOffloadV2TcpOptionsSupported,

    WfcAdapterPropertyRSCIpv4Enabled,
    WfcAdapterPropertyRSCIpv6Enabled,

    WfcAdapterPropertyRRDiagnoseLevel,

    WfcAdapterPropertyBackFillSize,
    
    WfcAdapterPropertyRegScanOptChannelCount,
    WfcAdapterPropertyRegAdapterCapabilityOverride,

    WfcAdapterPropertyScanModePassiveOnly,

    WfcAdapterPropertyRegDisableOpportunisticRoam,
    WfcAdapterPropertyRegOpportunisticRoamDelay,

    WfcAdapterDesiredSoftwareRadioOff,

    WfcAdapterSupportedGuids,

    WfcAdapterPropertyRegMaxRoamDeferCount,

    WfcAdapterPropertyNameMax
} WFC_ADAPTER_PROPERTY_NAME;

static const WFC_PROPERTY_LOAD_INFO g_WFC_InfraSupportedUcastAlgoPairs[] =
{ 
    { WfcAdapterPropertyInfraSupportedUcastAlgoPairs,    WfcPropertyTypeVariableBufferPtr,  FIELD_OFFSET (WDI_STATION_ATTRIBUTES_CONTAINER, UnicastAlgorithms.pElements),   sizeof(WDI_ALGO_PAIRS),           
                                                                                                                FIELD_OFFSET (WDI_STATION_ATTRIBUTES_CONTAINER , UnicastAlgorithms.ElementCount), 
                                                                                                                RTL_FIELD_SIZE (WDI_STATION_ATTRIBUTES_CONTAINER, UnicastAlgorithms.ElementCount), },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_InfraSupportedMcastAlgoPairs[] =
{ 
    { WfcAdapterPropertyInfraSupportedMcastAlgoPairs,    WfcPropertyTypeVariableBufferPtr,  FIELD_OFFSET (WDI_STATION_ATTRIBUTES_CONTAINER, MulticastDataAlgorithms.pElements),   sizeof(WDI_ALGO_PAIRS),           
                                                                                                                FIELD_OFFSET (WDI_STATION_ATTRIBUTES_CONTAINER , MulticastDataAlgorithms.ElementCount), 
                                                                                                                RTL_FIELD_SIZE (WDI_STATION_ATTRIBUTES_CONTAINER, MulticastDataAlgorithms.ElementCount), },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_PM_Capabilities[] =
{ 
    { WfcAdapterPMCapabilitiesFlags,                      WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, Flags),                      sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesSupportedWoLPacketPatterns, WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, SupportedWoLPacketPatterns), sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesNumTotalWoLPatterns,        WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, NumTotalWoLPatterns),        sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMaxWoLPatternSize,          WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MaxWoLPatternSize),          sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMaxWoLPatternOffset,        WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MaxWoLPatternOffset),        sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMaxWoLPacketSaveBuffer,     WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MaxWoLPacketSaveBuffer),     sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesSupportedProtocolOffloads,  WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, SupportedProtocolOffloads),  sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesNumArpOffloadIPv4Addresses, WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, NumArpOffloadIPv4Addresses), sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesNumNSOffloadIPv6Addresses,  WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, NumNSOffloadIPv6Addresses),  sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMinMagicPacketWakeUp,       WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MinMagicPacketWakeUp),       sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMinPatternWakeUp,           WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MinPatternWakeUp),           sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMinLinkChangeWakeUp,        WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MinLinkChangeWakeUp),        sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesSupportedWakeUpEvents,      WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, SupportedWakeUpEvents),      sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPMCapabilitiesMediaSpecificWakeUpEvents,  WfcPropertyTypeULong, FIELD_OFFSET (WDI_PM_CAPABILITIES_CONTAINER, MediaSpecificWakeUpEvents),  sizeof( UINT32 ), 0, 0  },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_ExtSta_Capabilities[] =
{
    { WfcAdapterPropertyScanSSIDListSize,             WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, ScanSSIDListSize),              sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyDesiredBSSIDListSize,         WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, DesiredBSSIDListSize),          sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyDesiredSSIDListSize,          WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, DesiredSSIDListSize),           sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyPrivacyExemptionListSize,     WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, PrivacyExemptionListSize),      sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyKeyMappingTableSize,          WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, KeyMappingTableSize),           sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyDefaultKeyTableSize,          WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, DefaultKeyTableSize),           sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyWEPKeyValueMaxLength,         WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, WEPKeyValueMaxLength),          sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyMaxNumPerSTADefaultKeyTables, WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, MaxNumPerSTA),                  sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyBSSListCachemanagement,       WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, BSSListCachemanagement),        sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyConnectBSSSelectionOverride,  WfcPropertyTypeULong, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, ConnectBSSSelectionOverride),   sizeof( UINT32 ), 0, 0  },
    { WfcAdapterPropertyHESSIDConnectionSupported,    WfcPropertyTypeBoolean, FIELD_OFFSET (WDI_STATION_CAPABILITIES_CONTAINER, HESSIDConnectionSupported),   RTL_FIELD_SIZE (WDI_STATION_CAPABILITIES_CONTAINER, HESSIDConnectionSupported), 0, 0  },
};

//Checksum
static const WFC_PROPERTY_LOAD_INFO g_WFC_ChecksumOffloadV4Tx_Capabilities[] =
{
    { WfcAdapterPropertyChecksumV4TxEncapsulation, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, Encapsulation), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4TxIpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, IpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4TxTcpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, TcpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4TxTcpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, TcpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4TxUdpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, UdpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4TxIpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, IpChecksum), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_ChecksumOffloadV4Rx_Capabilities[] =
{
    { WfcAdapterPropertyChecksumV4RxEncapsulation, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, Encapsulation), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4RxIpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, IpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4RxTcpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, TcpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4RxTcpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, TcpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4RxUdpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, UdpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV4RxIpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER, IpChecksum), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_ChecksumOffloadV6Tx_Capabilities[] =
{
    { WfcAdapterPropertyChecksumV6TxEncapsulation, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, Encapsulation), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6TxIpExtensionHeadersSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, IpExtensionHeadersSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6TxTcpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, TcpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6TxTcpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, TcpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6TxUdpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, UdpChecksum), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_ChecksumOffloadV6Rx_Capabilities[] =
{
    { WfcAdapterPropertyChecksumV6RxEncapsulation, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, Encapsulation), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6RxIpExtensionHeadersSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, IpExtensionHeadersSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6RxTcpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, TcpOptionsSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6RxTcpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, TcpChecksum), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyChecksumV6RxUdpChecksum, WfcPropertyTypeULong, FIELD_OFFSET(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER, UdpChecksum), sizeof(UINT32), 0, 0 },
};

//LSO
static const WFC_PROPERTY_LOAD_INFO g_WFC_largeSendOffloadV1_Capabilities[] =
{
    { WfcAdapterPropertyLargeSendOffloadV1Encapsulation, WfcPropertyTypeULong, FIELD_OFFSET(WDI_LSO_V1_CAPABILITIES_CONTAINER, Encapsulation), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV1MaxOffLoadSize, WfcPropertyTypeULong, FIELD_OFFSET(WDI_LSO_V1_CAPABILITIES_CONTAINER, MaxOffLoadSize), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV1MinSegmentCount, WfcPropertyTypeULong, FIELD_OFFSET(WDI_LSO_V1_CAPABILITIES_CONTAINER, MinSegmentCount), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV1TcpOptions, WfcPropertyTypeULong, FIELD_OFFSET(WDI_LSO_V1_CAPABILITIES_CONTAINER, TcpOptions), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV1IpOptions, WfcPropertyTypeULong, FIELD_OFFSET(WDI_LSO_V1_CAPABILITIES_CONTAINER, IpOptions), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_largeSendOffloadV2ipv4_Capabilities[] =
{
    { WfcAdapterPropertyLargeSendOffloadV2EncapsulationV4, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV4_LSO_V2_CONTAINER, EncapsulationV4), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV4, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV4_LSO_V2_CONTAINER, MaxOffloadSizeV4), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV4, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV4_LSO_V2_CONTAINER, MinSegmentCountV4), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_largeSendOffloadV2ipv6_Capabilities[] =
{
    { WfcAdapterPropertyLargeSendOffloadV2EncapsulationV6, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV6_LSO_V2_CONTAINER, EncapsulationV6), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV6, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV6_LSO_V2_CONTAINER, MaxOffloadSizeV6), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV6, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV6_LSO_V2_CONTAINER, MinSegmentCountV6), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2IpExtensionHeadersSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV6_LSO_V2_CONTAINER, IpExtensionHeadersSupported), sizeof(UINT32), 0, 0 },
    { WfcAdapterPropertyLargeSendOffloadV2TcpOptionsSupported, WfcPropertyTypeULong, FIELD_OFFSET(WDI_IPV6_LSO_V2_CONTAINER, TcpOptionsSupported), sizeof(UINT32), 0, 0 },
};

static const WFC_PROPERTY_LOAD_INFO g_WFC_RSC_Capabilities[] =
{
    { WfcAdapterPropertyRSCIpv4Enabled, WfcPropertyTypeBoolean, FIELD_OFFSET(WDI_RECEIVE_COALESCE_OFFLOAD_CAPABILITIES_CONTAINER, Ipv4Enabled), sizeof(UINT8), 0, 0 },
    { WfcAdapterPropertyRSCIpv6Enabled, WfcPropertyTypeBoolean, FIELD_OFFSET(WDI_RECEIVE_COALESCE_OFFLOAD_CAPABILITIES_CONTAINER, Ipv6Enabled), sizeof(UINT8), 0, 0 },
};

//--------------
typedef enum _WFC_BAND_INFO_PROPERTY_NAME
{      
    WfcBandInfoBandId = 0,
    WfcBandInfoBandState,
    WfcBandInfoTempType,
    WfcBandInfoPhyTypeList,
    WfcBandInfoChannelList,
    WfcBandInfoChannelWidthList,

    WfcBandInfoNameMax
} WFC_BAND_INFO_PROPERTY_NAME;

static const WFC_PROPERTY_LOAD_INFO g_WFC_BandInfoProperties[] =
{ 
    { WfcBandInfoBandId,             WfcPropertyTypeULong,             FIELD_OFFSET (WFC_BAND_INFO, bandID),            RTL_FIELD_SIZE (WFC_BAND_INFO, bandID),            0, 0 },
    { WfcBandInfoBandState,          WfcPropertyTypeUchar,             FIELD_OFFSET (WFC_BAND_INFO, bandState),         RTL_FIELD_SIZE (WFC_BAND_INFO, bandState),         0, 0 }, 
    { WfcBandInfoTempType,           WfcPropertyTypeULong,             FIELD_OFFSET (WFC_BAND_INFO, numOfPhyTypes),     RTL_FIELD_SIZE (WFC_BAND_INFO, numOfPhyTypes),     0, 0 },
    { WfcBandInfoPhyTypeList,        WfcPropertyTypeVariableBufferPtr, FIELD_OFFSET (WFC_BAND_INFO, pPhyTypeList),      sizeof(UINT32),           
                                                                                                                        FIELD_OFFSET (WFC_BAND_INFO , numOfPhyTypes), 
                                                                                                                        RTL_FIELD_SIZE (WFC_BAND_INFO, numOfPhyTypes), },
    { WfcBandInfoChannelWidthList,   WfcPropertyTypeVariableBufferPtr, FIELD_OFFSET (WFC_BAND_INFO, pChannelWidthList), sizeof(UINT32), 
                                                                                                                        FIELD_OFFSET (WFC_BAND_INFO , numOfChannelWidths), 
                                                                                                                        RTL_FIELD_SIZE (WFC_BAND_INFO, numOfChannelWidths), },
    { WfcBandInfoChannelList,        WfcPropertyTypeVariableBufferPtr, FIELD_OFFSET (WFC_BAND_INFO, pChannelList),      sizeof(WDI_CHANNEL_MAPPING_ENTRY), 
                                                                                                                        FIELD_OFFSET (WFC_BAND_INFO , numOfChannels), 
                                                                                                                        RTL_FIELD_SIZE (WFC_BAND_INFO, numOfChannels), },
};

//--------------
typedef enum _WFC_ADAPTER_PHY_INFO_PROPERTY_NAME
{      
    WfcAdapterPhyType = 0,
    WfcAdapterPhyCFPoll,
    WfcAdapterPhyMPDUMax,
    WfcAdapterPhyTempertureClass,
    WfcAdapterPhyAntenaDiversitySupported,
    WfcAdapterPhyTxPowerLevelList,
    WfcAdapterPhyDataRateList,

    WfcAdapterDot11PhyID,

    WfcAdapterPhyInfoNameMax
} WFC_ADAPTER_PHY_INFO_PROPERTY_NAME;


static const WFC_PROPERTY_LOAD_INFO g_WFC_AdapterPhyInfoProperties[] =
{ 
    { WfcAdapterPhyType,            WfcPropertyTypeULong,             FIELD_OFFSET (WFC_PHY_INFO, PhyType),           RTL_FIELD_SIZE (WFC_PHY_INFO, PhyType),          0, 0 },
    { WfcAdapterPhyCFPoll,          WfcPropertyTypeUchar,             FIELD_OFFSET (WFC_PHY_INFO, CFPollable),        RTL_FIELD_SIZE (WFC_PHY_INFO, CFPollable),       0, 0 }, 
    { WfcAdapterPhyMPDUMax,         WfcPropertyTypeULong,             FIELD_OFFSET (WFC_PHY_INFO, MPDUMaxLength),     RTL_FIELD_SIZE (WFC_PHY_INFO, MPDUMaxLength),    0, 0 },
    { WfcAdapterPhyTempertureClass, WfcPropertyTypeULong,             FIELD_OFFSET (WFC_PHY_INFO, TempType),          RTL_FIELD_SIZE (WFC_PHY_INFO, TempType),         0, 0 },
    { WfcAdapterPhyAntenaDiversitySupported,   WfcPropertyTypeULong,  FIELD_OFFSET (WFC_PHY_INFO, DiversitySupport),  RTL_FIELD_SIZE (WFC_PHY_INFO, DiversitySupport), 0, 0 },


    { WfcAdapterPhyTxPowerLevelList,WfcPropertyTypeVariableBuffer,    FIELD_OFFSET (WFC_PHY_INFO, TxPowerLevels),   sizeof(UINT32),           
                                                                                                                        FIELD_OFFSET (WFC_PHY_INFO , NumberSupportedPowerLevels), 
                                                                                                                        RTL_FIELD_SIZE (WFC_PHY_INFO, NumberSupportedPowerLevels), },

    { WfcAdapterPhyDataRateList,    WfcPropertyTypeVariableBuffer,    FIELD_OFFSET (WFC_PHY_INFO, DataRateList),      sizeof(WDI_DATA_RATE_ENTRY), 
                                                                                                                       FIELD_OFFSET (WFC_PHY_INFO , NumberDataRateEntries), 
                                                                                                                       RTL_FIELD_SIZE (WFC_PHY_INFO, NumberDataRateEntries), },
};

//--------------
typedef enum _WFC_PORT_PROPERTY_NAME : WFC_PROPERTY_NAME
{   
    WfcPortPropertyNamePacketFilter = 0,
    WfcPortPropertyNameDefaultKeyId,
    WfcPortPropertyNameTxLinkSpeed,
    WfcPortPropertyNameRxLinkSpeed,
    WfcPortPropertyNameLinkQuality,

    // Connection configuration
    WfcPortPropertySSID,
    WfcPortPropertyHESSID,
    WfcPortPropertyAuthAlgoList,
    WfcPortPropertyMulticastCipherAlgoList,
    WfcPortPropertyUnicastCipherAlgoList,
    WfcPortPropertyExcludeUnencrypted,
    WfcPortPropertyHiddenNetwork,
    WfcPortPropertyExtraAssociationRequestIEs,
    WfcPortPropertyAnyBSSIDAllowed,
    WfcPortPropertyDesiredBSSIDList,
    
    WfcPortPropertyIsConnected,
    WfcPortPropertyConnectedBSSID,    

    //[802.11r] Fast transition
    WfcPortPropertyPMKR0Name,
    WfcPortPropertyR0KHID,
    WfcPortPropertySNonce,
    WfcPortPropertyCurrentConnectionFT,

    WfcPortProperty8023MulticastList,
    WfcPortPropertyDot11MulticastList,
    WfcPortPropertyRadioState_Temp,

    WfcPortCurrentOperationalMode,
    WfcPortCurrentBeaconPeriod,

    WfcPortOSConfiguredPhyID,   // This is what OS configured & generally applies to next OID
                                // Next OID should cache this value since it can change if another
                                // OID is allowed to come down
    WfcPortBssType,
    WfcPortPropertyMediaStreamingEnabled,
    WfcPortPropertyAutoConfigEnabled,
    WfcPortPropertyUnicastUseGroupEnabled_Dummy,
    WfcPortPropertySafeModeEnabled,
    WfcPortPropertyMultipleBSSIDNetwork,
    WfcPortPropertyMFPAllowed,
    
    WfcPortPropertyRTSThreshold_Dummy,
    WfcPortPropertyFragmentationThreshold_Dummy,
    WfcPortPropertyUnreachableThreshold_Dummy,
    WfcPortPropertyCurrentChannelNumber,
    WfcPortPropertyCurrentBandID,
    WfcPortPropertyNamePrivacyExemptionList,
    WfcPortPropertyExcludedMacAddressList,
    WfcPortPropertyIgnoreAllMacAddresses,
    WfcPortPropertyPMKIDList,
    WfcPortPropertyPMKIDListUpdatePending,
    WfcPortPropertyAssociationParameterPMKIDUpdatePending,
    WfcPortPropertyPMKIDSupported,
    WfcPortPropertyDesiredPhyList_Dummy,
    WfcPortPropertyActivePhyList_Dummy,

    WfcPortPropertyConnectionQuality,

    WfcPortPropertyMostDesiredPhyIndex,

    //
    // P2P Common Port Configuration
    //
    WfcPortPropertyP2PDeviceCaps,
    WfcPortPropertyP2PGroupCaps,
    WfcPortPropertyP2PDeviceInfo,
    WfcPortPropertyP2PSecondaryDeviceTypeList,
    WfcPortPropertyP2PAdditionalIEs,
    WfcPortPropertyP2PDesiredGroupID,
    WfcPortPropertyP2PAdvertisementList,
    WfcPortPropertyP2PBackgroundDiscoverySettings,

    //
    // P2P Device Port
    //
    WfcPortPropertyCurrentListenState,

    //
    // P2P Client
    //    
    WfcPortPropertyP2PGroupJoinParameters,
    
    //
    // P2P GO
    //
    WfcPortPropertyAPState,
    WfcPortPropertyP2PGroupStartParameters,
    WfcPortPropertyBeaconPeriod,
    WfcPortPropertyDTIMPeriod,
    WfcPortPropertyWpsEnabled,
    WfcPortProperty802_11bDataRateSupport,

    WfcPortPropertyQoSProtocolFlags,
    WfcPortPropertyGoodScanStartTime,

    //
    // power
    //
    WfcPortPropertyAutoPsm,
    WfcPortPropertyProtocolOffloadList,
    WfcPortPropertyProtocolOffload,
    WfcPortPropertyWolPatternList,
    WfcPortPropertyWolPattern,
    WfcPortPropertyReceiveCoalesceList,
    WfcPortPropertyReceiveCoalesce,
    WfcPortPropertyRoamAtResumeD0,
    WfcPortPropertyCreateOpMask,

    WfcPortPropertyIhvRoamRequested,
    WfcPortPropertyTimeOfIhvRoamRequest,
    WfcPortPropertyIhvRoamReason,

    //offload Capabilities

    //Checksum
    WfcPortPropertyTaskOffloadsDisabledForFIPS,

    WfcPortPropertyChecksumV4TxEncapsulation,
    WfcPortPropertyChecksumV4TxIpOptionsSupported,
    WfcPortPropertyChecksumV4TxTcpOptionsSupported,
    WfcPortPropertyChecksumV4TxTcpChecksum,
    WfcPortPropertyChecksumV4TxUdpChecksum,
    WfcPortPropertyChecksumV4TxIpChecksum,

    WfcPortPropertyChecksumV4RxEncapsulation,
    WfcPortPropertyChecksumV4RxIpOptionsSupported,
    WfcPortPropertyChecksumV4RxTcpOptionsSupported,
    WfcPortPropertyChecksumV4RxTcpChecksum,
    WfcPortPropertyChecksumV4RxUdpChecksum,
    WfcPortPropertyChecksumV4RxIpChecksum,

    WfcPortPropertyChecksumV6TxEncapsulation,
    WfcPortPropertyChecksumV6TxIpExtensionHeadersSupported,
    WfcPortPropertyChecksumV6TxTcpOptionsSupported,
    WfcPortPropertyChecksumV6TxTcpChecksum,
    WfcPortPropertyChecksumV6TxUdpChecksum,

    WfcPortPropertyChecksumV6RxEncapsulation,
    WfcPortPropertyChecksumV6RxIpExtensionHeadersSupported,
    WfcPortPropertyChecksumV6RxTcpOptionsSupported,
    WfcPortPropertyChecksumV6RxTcpChecksum,
    WfcPortPropertyChecksumV6RxUdpChecksum,

    WfcPortPropertyLargeSendOffloadV1Encapsulation,

    WfcPortPropertyLargeSendOffloadV1MaxOffLoadSize,
    WfcPortPropertyLargeSendOffloadV1MinSegmentCount,
    WfcPortPropertyLargeSendOffloadV1TcpOptions,
    WfcPortPropertyLargeSendOffloadV1IpOptions,

    WfcPortPropertyLargeSendOffloadV2EncapsulationV4,
    WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV4,
    WfcPortPropertyLargeSendOffloadV2MinSegmentCountV4,

    WfcPortPropertyLargeSendOffloadV2EncapsulationV6,
    WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV6,
    WfcPortPropertyLargeSendOffloadV2MinSegmentCountV6,
    WfcPortPropertyLargeSendOffloadV2IpExtensionHeadersSupported,
    WfcPortPropertyLargeSendOffloadV2TcpOptionsSupported,

    WfcPortPropertyRSCIpv4Enabled,
    WfcPortPropertyRSCIpv6Enabled,

    WfcPortPropertyResumeNLOList,
    WfcPortPropertyLastSleepWasWakeEnabled,

    WfcPortPropertyNameMax
} WFC_PORT_PROPERTY_NAME;

//--------------
//--------------

const DOT11_AUTH_ALGORITHM AUTH_ALGO_PREFERENCE_LIST [] = 
{ 
    DOT11_AUTH_ALGO_RSNA,
    DOT11_AUTH_ALGO_WPA,
    DOT11_AUTH_ALGO_RSNA_PSK,
    DOT11_AUTH_ALGO_WPA_PSK,
    DOT11_AUTH_ALGO_80211_OPEN,
    DOT11_AUTH_ALGO_80211_SHARED_KEY
};

const UINT32 AUTH_ALGO_PREFERENCE_LIST_SIZE = celems (AUTH_ALGO_PREFERENCE_LIST);


typedef struct _WFC_VARIABLE_BUFFER_PROPERTY
{
    _WFC_VARIABLE_BUFFER_PROPERTY();

    void Cleanup();
    ~_WFC_VARIABLE_BUFFER_PROPERTY();

    NDIS_STATUS CopyBuffer(
        _In_ ULONG BufferLength,
        _In_ UINT16 ElementCount,
        _In_reads_bytes_(BufferLength) BYTE* Buffer );

    BYTE* m_PropertyBuffer;
    ULONG m_CurrentBufferLength;    // Bytes
    ULONG m_MaxBufferLength;
    
    // Valid only for list
    UINT16 m_CurrentElementCount;

} WFC_VARIABLE_BUFFER_PROPERTY;

class CPropertyCache;

typedef struct _WFC_PROPERTY_ENTRY
{
    _WFC_PROPERTY_ENTRY();
    ~_WFC_PROPERTY_ENTRY();
    void CleanUp();

    WFC_PROPERTY_NAME Name;
    WFC_PROPERTY_TYPE Type;
    bool              IsPopulated;

    NDIS_STATUS InitializeFromDefault(
        _In_ _WFC_PROPERTY_ENTRY* pDefaultEntry );

    union
    {
        ULONG   ULongProperty;
        BOOLEAN BooleanProperty;
        UCHAR UcharProperty;
        CPropertyCache *PropertyCacheList;
    };

    WFC_VARIABLE_BUFFER_PROPERTY BufferProperty;
    
    //
    // Reference to default entry.
    //
    _WFC_PROPERTY_ENTRY* pDefaultEntry;
} WFC_PROPERTY_ENTRY, *PWFC_PROPERTY_ENTRY;

class CPropertyCache
{
public:
    CPropertyCache();

    ~CPropertyCache();

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyUchar(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ UCHAR* pPropertyValue );

    NDIS_STATUS SetPropertyUchar(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR PropertyValue );

    UCHAR GetPropertyUcharOrDefault(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR bDefaultValue );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyBoolean(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ BOOLEAN* pPropertyValue );

    NDIS_STATUS SetPropertyBoolean(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ BOOLEAN PropertyValue );

    BOOLEAN GetPropertyBooleanOrDefault(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ BOOLEAN bDefaultValue );
    
    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyULong(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ ULONG* pPropertyValue );

    NDIS_STATUS SetPropertyULong(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ ULONG PropertyValue );

    ULONG GetPropertyULongOrDefault(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ ULONG bDefaultValue );


    // Overloads.  If the compile-time-asserts fire, then we need more primitives
    template<typename T>
    __success( return == NDIS_STATUS_SUCCESS )
    __forceinline NDIS_STATUS GetPropertyT( _In_ WFC_PROPERTY_NAME PropertyName, _Out_ T* pPropertyValue )
    {
        C_ASSERT( (sizeof( ULONG ) == sizeof( T )) || (sizeof( UCHAR ) == sizeof( T )) );
#pragma warning(suppress : 4127) // conditional expression is constant - we're counting on this
        if (sizeof( ULONG ) == sizeof( T ))
        {
            return GetPropertyULong( PropertyName, (ULONG*)pPropertyValue );
        }
        return GetPropertyUchar( PropertyName, (UCHAR*)pPropertyValue );
    }
    template<typename T>
    __forceinline NDIS_STATUS SetPropertyT( _In_ WFC_PROPERTY_NAME PropertyName, _In_ T PropertyValue )
    {
        C_ASSERT( (sizeof( ULONG ) == sizeof( T )) || (sizeof( UCHAR ) == sizeof( T )) );
#pragma warning(suppress : 4127) // conditional expression is constant - we're counting on this
        if (sizeof( ULONG ) == sizeof( T ))
        {
            return SetPropertyULong( PropertyName, (ULONG)PropertyValue );
        }
        return SetPropertyUchar( PropertyName, (UCHAR)PropertyValue );
    }
    template<typename T>
    __forceinline T GetPropertyTOrDefault( _In_ WFC_PROPERTY_NAME PropertyName, _In_ T DefaultValue )
    {
        C_ASSERT( (sizeof( ULONG ) == sizeof( T )) || (sizeof( UCHAR ) == sizeof( T )) );
#pragma warning(suppress : 4127) // conditional expression is constant - we're counting on this
        if (sizeof( ULONG ) == sizeof( T ))
        {
            return (T)GetPropertyULongOrDefault( PropertyName, (ULONG)DefaultValue );
        }
        return (T)GetPropertyUcharOrDefault( PropertyName, (UCHAR)DefaultValue );
    }

    __success( return == NDIS_STATUS_SUCCESS )
    NDIS_STATUS GetPropertyBuffer(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ ULONG* pBufferLength,
        _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer );

    NDIS_STATUS SetPropertyBuffer(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ ULONG BufferLength, 
        _In_reads_bytes_(BufferLength) BYTE* Buffer );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyList(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ ULONG* pBufferLength,
        _Out_ UINT16* pElementCount,
        _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer );

    NDIS_STATUS SetPropertyList(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ ULONG BufferLength, 
        _In_ UINT16 ElementCount,
        _In_reads_bytes_(BufferLength) BYTE* Buffer );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyPropertyCacheList(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _Out_ CPropertyCache **ppPropertyValue );

    NDIS_STATUS CPropertyCache::SetPropertyPropertyCacheList(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ CPropertyCache * pPropertyValue );

    ULONG GetNumOfProperties()
    {
        return m_PropertyNameMax;
    }    

    // List entry for maintaining "this" in a list in case if the object is part of enumerable property list
    CPropertyCache *Next;

    NDIS_STATUS Initialize(
        _In_ UINT32 NumOfProperties,
        _In_opt_ CPropertyCache* pDefaultValues );

protected:

    NDIS_STATUS LoadPropertyListProperty(
        _In_ WFC_PROPERTY_NAME enumarablePropertyName,
        _In_ UINT32 bufferElementsCount,
        _In_ UINT32 bufferElementSize,
        _In_reads_bytes_(bufferElementsCount * bufferElementSize) BYTE* pBuffer,    
        _In_reads_(propertyFieldsCount) const WFC_PROPERTY_LOAD_INFO* pPropertyFields, 
        _In_range_(1, MAXULONG32) const ULONG propertyFieldsCount,
        _In_ UINT32 maxPropertyNamesPerElement);

    NDIS_STATUS LoadProperties(
        CPropertyCache *propertyCache,
        _In_ UINT32 bufferElementSize,
        _In_reads_bytes_(bufferElementSize) BYTE* pBuffer,    
        _In_reads_(propertiesCountPerElement) const WFC_PROPERTY_LOAD_INFO* pPropertiesPerElement, 
        _In_range_(1, MAXULONG32) const ULONG propertiesCountPerElement,
        _In_ UINT32 maxPropertiesPerElement);

    NDIS_STATUS LoadProperty(
        CPropertyCache *propertyCache,
        _In_ UINT32 bufferElementSize,
        _In_reads_bytes_(bufferElementSize) BYTE* pBuffer,    
        _In_ const WFC_PROPERTY_LOAD_INFO* pPropertiesPerElement, 
        _In_ UINT32 maxPropertiesPerElement);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPropertyEntryForPropertyName(
        _In_ WFC_PROPERTY_NAME     PropertyName,
        _In_ WFC_PROPERTY_TYPE     ExpectedPropertyType,
        _In_ bool                  RequirePopulatedEntry,
        _Out_ WFC_PROPERTY_ENTRY **ppPropertyEntry );

    ULONG m_PropertyNameMax;
    WFC_PROPERTY_ENTRY *m_PropertyTable;
};

// Forward declaration
class CPhyCacheHelper;
class CBandCacheHelper;
class CFirmwareCapabilities;

class CAdapterPropertyCache:
    public CPropertyCache
{

public:

    CAdapterPropertyCache();

    NDIS_STATUS Initialize();

    NDIS_STATUS SetFirmwareCapabilitiesProperties(
        _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* firmwareCapabilities);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPhyTypeFromDot11PhyID(
        _In_ ULONG Dot11PhyID,
        _Out_ UINT32 *pPhyType
        );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetDot11PhyIDFromPhyType(
        _In_ UINT32 PhyType,
        _Out_ ULONG *pDot11PhyID
        );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS CreateBandCacheHelperForBandID(
        _In_ ULONG BandID,
        _Out_ CBandCacheHelper **ppBandCacheHelper);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetBandIDForStartingFrequency(
        _In_ UINT32 StartingFrequencyInKhz,
        _Out_ UINT32* pBandID );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS CreatePhyCacheHelperForDot11PhyID(
        _In_ ULONG Dot11PhyID,
        _Out_ CPhyCacheHelper **ppPhyCacheHelper);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetMaximumSupportedAuthAlgorithm(
        _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
        _Out_ DOT11_AUTH_ALGORITHM *pAuthAlgorithmIdOut);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS IsAuthAlgorithmSupported(        
        _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
        _In_ DOT11_AUTH_ALGORITHM AlgoId,
        _Out_ BOOLEAN *IsSupported);

    __success(return == NDIS_STATUS_SUCCESS)
     NDIS_STATUS GetMaxMulticastListSize(
         _Out_ PULONG pMulticastListSizeOut);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPermanentMacAddress(
        _Out_ PDOT11_MAC_ADDRESS  pHardwareAddress);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetAuthAlgorithmPairs(        
        _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
        _Out_ ULONG* pNumOfAlgoPairs,
        _Outptr_result_buffer_(*pNumOfAlgoPairs) PDOT11_AUTH_CIPHER_PAIR* ppAlgoPairs);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS PopulateTcpOffloadCapabilities(
        _In_ WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities);

    NDIS_STATUS GetTCPOffloadParametersFromPropertyCache(
        _Out_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER OffloadParameters);

private:

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetBandInfoCacheForBandID(
        _In_ ULONG BandID,
        _Out_ CPropertyCache **ppBandInfoCache);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GetPhyInfoCacheForDot11PhyID(
        _In_ ULONG Dot11PhyID,
        _Out_ CPropertyCache **ppPhyInfoCache);
        
    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS LoadBandCacheForDot11PhyID(
        _In_ ULONG Dot11PhyID,
        _In_ CPhyCacheHelper *pPhyCacheHelper);
        
};

class CPortPropertyCache: 
    public CPropertyCache
{
private:
    CAdapterPropertyCache*      m_pAdapterCache;
    WFC_PORT_TYPE               m_PortType;
public:
    CPortPropertyCache();

    NDIS_STATUS Initialize(
        _In_ WFC_PORT_TYPE PortType,
        _In_ CAdapterPropertyCache *pAdapterCache);

    NDIS_STATUS Dot11Reset(
        _In_ BOOLEAN bSetDefaultMIB);

    VOID AddCiphersToSupportedList(
        _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPropertyName,
        _Out_writes_to_( MaxCiphers, *pCurrentCipherCount ) DOT11_CIPHER_ALGORITHM  *pSupportedCiphersList,
        _Out_ UINT16 *pCurrentCipherCount,
        _In_ UINT16 MaxCiphers,
        _In_reads_opt_( MaxAllowedCiphers ) DOT11_CIPHER_ALGORITHM *pAllowedCipherAlgoList,
        _In_ UINT16 MaxAllowedCiphers);

    NDIS_STATUS GetTCPOffloadParametersFromPortPropertyCache(
        _Out_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pOffloadParameters);

    NDIS_STATUS SetTCPOffloadParametersPortPropertyCache(
        _In_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pOffloadParameters);

private:
    NDIS_STATUS NdisReset();

};


class CBandCacheHelper
{
private:
    CAdapterPropertyCache*  m_pAdapterCache;
    CPropertyCache*         m_pBandInfoCache;
    
public:
    CBandCacheHelper(
        CAdapterPropertyCache* pAdapterCache,
        CPropertyCache*     pBandInfoCache
        );

public:    
    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS DetectPhyID(
        _In_ ULONG ChannelNumber,
        _In_bytecount_(BeaconProbeLength)PDOT11_BEACON_FRAME pBeaconProbeFrame,
        _In_ UINT16 BeaconProbeLength,
        _Out_ ULONG* pDot11PhyID
        );

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS ConvertChannelNumberToFrequency(
        _In_ ULONG ChannelNumber,
        _Out_ ULONG* pChannelCenterFrequency
        );

    CPropertyCache* GetPropertyCache()
    {
        return m_pBandInfoCache;
    }
};

class CPhyCacheHelper
{
private:
    CAdapterPropertyCache*  m_pAdapterCache;
    CPropertyCache*         m_pPhyInfoCache;

public:
    CPhyCacheHelper(
        CAdapterPropertyCache* pAdapterCache,
        CPropertyCache*        pPhyInfoCache
        );

    ~CPhyCacheHelper(
        );

    NDIS_STATUS AddBandInfoCache(
        CPropertyCache*        pBandInfoCache
        );

    CPropertyCache* GetPropertyCache()
    {
        return m_pPhyInfoCache;
    }

};
