#pragma once

#define WDI_POOL_TAG     (ULONG) 'GidW'
#define WDI_POOL_TAG_KEY (ULONG) 'KidW'

#ifndef MSECS_TO_CURRENT_TIME_UNITS
#define MSECS_TO_CURRENT_TIME_UNITS(x) (((ULONGLONG)(x)) * 10000)
#endif

#ifndef CURRENT_TIME_UNITS_TO_MSECS
#define CURRENT_TIME_UNITS_TO_MSECS(x) ((x) / 10000)
#endif

NTSTATUS
WdiInitialize(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath);


VOID
WdiCleanup();

NTSTATUS
WdiLibCreateWdfControlObject(
    _In_ WDFDRIVER Driver,
    _Out_ WDFDEVICE* m_WdfDevice
    );

typedef enum _WFC_PORT_TYPE {
    WfcPortTypeUnspecified = 0,
    WfcPortTypeExtSTA = 0x1,
    WfcPortTypeExtAP = 0x2,
    WfcPortTypeWFDDevice = 0x4,
    WfcPortTypeWFDRole = 0x8,
    WfcPortTypeAll = (WfcPortTypeExtSTA | WfcPortTypeExtAP | WfcPortTypeWFDDevice | WfcPortTypeWFDRole),
    WfcPortTypeWithDataPath = (WfcPortTypeExtSTA | WfcPortTypeExtAP | WfcPortTypeWFDRole),
} WFC_PORT_TYPE;

typedef enum _WFC_PORT_DOT11_STATE {
    WfcPortDot11StateInit = 0,      // Dot11 Init State
    WfcPortDot11StateOp = 1,        // Dot11 Op State (link should be Active)
} WFC_PORT_DOT11_STATE;

#define WDI_TLV_HEADER_SIZE        4      // sizeof(UINT16) + sizeof(UINT16)
#define WDI_TLV_LENGTH_LIMIT       0xFFF0 // A TLV cannot be greater than these many bytes


typedef class _WFC_CONSTANT
{
public:
    static const UINT8 MaximumNumberOfPorts = 5;  // ExtSTA, ExtAP, WFDDevice, WFDRole 1, WFDRole 2
    static const ULONG DesiredBSSIDListSize = 8;
    static const ULONG ExcludedMacAddressListSize = 8;
    static const ULONG PMKIDCacheSize = 16;

    static const UINT  CheckForHangTimeSeconds = 10;

    static const UINT  LegacyMtuSize = 1500;
    static const UINT  NumOfTXBuffers = 256;
    static const UINT  NumOfRXBuffers = 256;

    static const ULONG APAssociationTableSize = 16;

    static const ULONG DefaultRTSThreshold = 2347;
    static const ULONG DefaultFragmentationThreshold = 2346;

    static const ULONG DefaultChannel = 1;
    
    static const ULONG DefaultUnreachableThreshold = 2000;

    static const ULONG RegDomain = DOT11_REG_DOMAIN_FCC;

    static const ULONG DefaultNeighborReportSendTimeout = 200; // 200 milliseconds
    static const ULONG DefaultNeighborReportPostACKDwellTime = 500; // 500 milliseconds

    static const ULONG DefaultActionFrameRequestSendTimeoutForSTA = 200; // 200 milliseconds
    static const ULONG DefaultActionFrameResponseSendTimeoutForSTA = 200; // 200 milliseconds
    static const ULONG DefaultActionFrameRequestSendTimeoutForP2P = 10000; // 10000 milliseconds
    static const ULONG DefaultActionFrameResponseSendTimeoutForP2P = 1000; // 1000 milliseconds

    static const ULONG DefaultActionFramePostACKDwellTime = 500; // 500 milliseconds

    static const ULONG DefaultANQPResponseFromAPTimeoutMs = 1000;    // 1 second
    static const ULONG DefaultANQPResponseFromP2PTimeoutMs = 2000;  // 2 seconds

    static const ULONG DefaultWFDActiveChannelDwellTime = 40; // 40 milliseconds
    static const ULONG DefaultWFDPassiveChannelDwellTime = 100; // 100 milliseconds

    static const ULONG DefaultActiveChannelDwellTime = 80; // 80 milliseconds
    static const ULONG DefaultPassiveChannelDwellTime = 110; // 110 milliseconds
    static const ULONG RoamScanShortActiveChannelDwellTime = 30; // 30 milliseconds
    static const ULONG RoamScanShortPassiveChannelDwellTime = 30; // 30 milliseconds   
    static const ULONG RoamScanLongActiveChannelDwellTime = 50;  // 50 milliseconds
    static const ULONG RoamScanLongPassiveChannelDwellTime = 110;  // 110 milliseconds
    static const ULONG MediaStreamingOffChannelTimeLimit = 20; // 20 milliseconds

    static const ULONG MaxScanOidTimeInMs = 10000;  // 10 seconds
    static const ULONG ScanRetryLimitTime = 40000000;  // 4 seconds

    static const ULONG DefaultBssEntryExpiryTime = 1200000000;   // 120 seconds (in 100 ns)
    static const ULONG WFDDeviceBssEntryExpiryTime = 3600000000;   // 360 seconds (in 100 ns)
    static const ULONG DefaultWfdDeviceNotVisibleTimeoutNSecs = 3600000000;   // 6 minutes (in 100 ns)
    static const ULONG DefaultWfdDeviceNotVisibleCheckIntervalMSecs = 360000;   // 6 minutes (in msecs)
    static const ULONG DefaultWfdDeviceNotVisibleDelayIntervalMSecs = 5000;   // 5 seconds (in msecs)
    static const ULONG DefaultWfdDeviceOrServiceUpdateDelayMSecs = 5000;   // 5 seconds (in msecs)
    static const ULONG MinimumBssEntryExpiryCheckTime = 5000;   // 5 seconds (in msecs)
    
    static const ULONG ConnectBssEntryExpiryTime = 750000000;   // 75 seconds (in 100 ns)    
    static const ULONG RoamBssEntryExpiryTime = 50000000;       // 5 seconds (in 100 ns)
    static const ULONG PartialScanBssEntryExpiryTime = 30000000;       // 3 seconds (in 100 ns)
    static const ULONG PMKIDReportingInterval = 10000000;   // 1 second (in 100 ns)
    static const ULONG PMKIDUpdateWaitTimeMs = 20;              // 20 milliseconds
    static const ULONG MaxP2PConfigurationTimeoutMs = 2550; // 2550 milliseconds
    static const ULONG RequestNeighborReportDelayMs = 5000;       // 5 seconds
    static const ULONG OpportunisticRoamDelayMs = 3 * 60 * 1000; // 3 minutes
    static const ULONG OpportunisticRoamDelayMinMs = 5 * 1000; // 5 seconds
    static const ULONG OpportunisticRoamDelayMaxMs = 60 * 60 * 1000; // 60 minutes

    static const ULONG MaxRoamDeferCountDefault = 4; // Note: Setting the value to 0 disables the feature
    static const ULONG MaxRoamDeferCountMin = 0;
    static const ULONG MaxRoamDeferCountMax = 10;

    static const ULONG TargetedDiscoveryTimeoutMs = 1000;   // 1 second

    static const ULONG IhvCachedEnumBssListMaxAge = 20000000;   // 2 seconds (in 100 ns) 
    static const ULONG MaxConnectBssEntryCount = 30;
    static const ULONG DefaultServiceComebackEntryExpiryTime = 300000000;   // 30 seconds (in 100 ns)
    static const ULONG IhvRoamRequestTimeout = 200000000;   // 20 seconds (in 100 ns) 

    static const ULONG RoamPartialScanLinkQualityAdjustment = 20;
    static const ULONG BssStillVisibleLinkQualityLowerLimit = 30;        // Lowest link quality for internal min-bar consideration
    static const ULONG BssRankBoostMaximumFor5GHz = 8;
    static const ULONG BssRankBoostRangeFor5GHz = 4;    

    static const ULONG RankAdjustment_BetterAP_RemoteDisconnect_Recent = 0; // 0
    static const ULONG RankAdjustment_BetterAP_RemoteDisconnect_Old = 500;  // 0.5
    static const ULONG RankAdjustment_BetterAP_Unreachable_NoLQChange = 100; // 0.1
    static const ULONG RankAdjustment_Connect_RemoteDisconnect_Recent = 500; // 0.5

    static const ULONG FastTransitionConnectRoamMinBlockedTimeSeconds = 7200; // 7200 seconds

    static const ULONG PortConnectHistoryListSize = 8;
    static const ULONG MaxNetworkChannelHistorySize = 8;
    static const ULONG ConnectBSSChannelHistoryBoost = 2; // Hit is always 1
    static const ULONG MaxConnectBSSChannelUsage = 6;     // Will try upto these many times if we dont see an AP on a channel we previously connected to
}WFC_CONSTANT;

//
// Macros for assigning and verifying NDIS_OBJECT_HEADER
//
#define WFC_ASSIGN_NDIS_OBJECT_HEADER(_header, _type, _revision, _size) \
    (_header).Type = _type; \
    (_header).Revision = _revision; \
    (_header).Size = _size; 

#ifndef celems
#define celems(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define IS_DRIVER_VERSION_650_OR_ABOVE(_Major, _Minor) (((_Major) > 6) || (((_Major) == 6) && ((_Minor) >= 50)))

#define INVALID_NDIS_PORT_NUMBER ((NDIS_PORT_NUMBER)-1)

#define NTSTATUS2NDISSTATUS(x)      ((!NT_SUCCESS(x)) ? NDIS_STATUS_FAILURE : NDIS_STATUS_SUCCESS)

#define NDISSTATUS2NTSTATUS(x)      ((x != NDIS_STATUS_SUCCESS) ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS)

#ifndef MIN
    #define MIN(a, b)                  ((a) < (b)? a : b)
#endif  // MIN
#ifndef MAX
    #define MAX(a, b)                  ((a) > (b)? a : b)
#endif  // MAX

/** Returns success of the MAC addresses are equal, else failure */
#define ARE_MAC_ADDRESS_EQUAL(_MacAddr1, _MacAddr2)    \
    (RtlCompareMemory(_MacAddr1, _MacAddr2, sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS))

#define DtPrintOff( fmt, ... )

#ifdef DBGDBG // don't use, moving to use WPP
#define DtPrint( fmt, ... ) DbgPrintEx(0,0, fmt, ##__VA_ARGS__)
#define DtErrMsg( fmt, ... ) DbgPrintEx(0,0, fmt, ##__VA_ARGS__); __debugbreak()
#define DtBreak() __debugbreak()
#define DtAssert( _exp ) if ( !(_exp) ) { DtBreak(); }
#define DtAssertMsg( exp, fmt,... ) if ( !(exp) ) { DtPrint( fmt, ##__VA_ARGS__);  DtBreak();}

#else // DBGDBG
#define DtPrint( fmt, ... )
#define DtErrMsg( fmt, ... )
#define DtBreak() 
#define DtAssert ( _exp )
#define DtAssertMsg( exp, fmt,... )
#endif // DBGDBG

// ONLY VALID FOR UPPER EDGE

// These bits can be set in g_Test_FeatureMask to enable paths in UE
#define TEST_FEATUREMASK_DISABLE_ALL_TASK_CANCEL    0x00000002
#define TEST_FEATUREMASK_CANCEL_NON_SCAN_TASK       0x00000004
#define TEST_FEATUREMASK_ENABLE_VWIFI               0x00000010
#define TEST_FEATUREMASK_UPDATE_HARDWARE_CHANNEL    0x00000020
#define TEST_FEATUREMASK_STATISTICS                 0x00000040
#define TEST_FEATUREMASK_PHY_LIST                   0x00000080
#define TEST_FEATUREMASK_OLDER_OS                   0x00000100

#define IS_FEATURE_ENABLED(_FeatureBit_) ((g_Test_FeatureMask & (_FeatureBit_)) != 0)
extern ULONG g_Test_FeatureMask;


typedef enum _ROAM_CONTROL_LEVEL
{
    WfcNeverRoam = 0,
    WfcMediaStreamingRoam = 1,
    WfcNormalRoam = 2,
    WfcSuperRoam = 3,
    WfcCustomRoam = 4,
    WfcMaxRoamControlLevel = 4
} ROAM_CONTROL_LEVEL;

typedef struct tagWFC_BAND_INFO 
{
    UINT32  bandID;
    UCHAR   bandState;
    ULONG   numOfPhyTypes;
    PUINT32 pPhyTypeList; 
    ULONG   numOfChannelWidths;
    PUINT32 pChannelWidthList; 
    ULONG   numOfChannels;
    PWDI_CHANNEL_MAPPING_ENTRY pChannelList;
} WFC_BAND_INFO, *PWFC_BAND_INFO;

typedef struct tagWFC_PHY_INFO 
{
    WDI_PHY_TYPE  PhyType;
    UCHAR   CFPollable;
    UINT32  MPDUMaxLength;
    DOT11_TEMP_TYPE TempType;
    DOT11_DIVERSITY_SUPPORT DiversitySupport;
    ULONG   NumberSupportedPowerLevels;
    UINT32  TxPowerLevels[8];
    ULONG   NumberDataRateEntries;
    WDI_DATA_RATE_ENTRY DataRateList[WDI_MAX_DATA_RATE_ENTRIES];
} WFC_PHY_INFO, *PWFC_PHY_INFO;

typedef struct _VARIABLE_LENGTH_BUFFER_UINT8
{
    UINT16 dataLength;
    UINT16 bufferCapacity;
    _Field_size_bytes_part_(bufferCapacity, dataLength)  UINT8 *pBuffer;
} VARIABLE_LENGTH_BUFFER_UINT8, *PVARIABLE_LENGTH_BUFFER_UINT8;

typedef struct _BUFFER_UINT32
{
    UINT16 ElementCount;
    _Field_size_(ElementCount) UINT32 *pBuffer;
} BUFFER_UINT32, *PBUFFER_UINT32;

typedef struct _BUFFER_UINT8
{
    UINT16 ElementCount;
    _Field_size_(ElementCount) UINT8 *pBuffer;
} BUFFER_UINT8, *PBUFFER_UINT8;

typedef struct _BUFFER_DOT11_SSID
{
    UINT16 ElementCount;   
    _Field_size_(ElementCount) PDOT11_SSID pBuffer;
} BUFFER_DOT11_SSID, *PBUFFER_DOT11_SSID;

typedef struct _BUFFER_DOT11_HESSID
{
    UINT16 ElementCount;
    _Field_size_(ElementCount) PDOT11_HESSID pBuffer;
} BUFFER_DOT11_HESSID, *PBUFFER_DOT11_HESSID;

typedef struct _BUFFER_DOT11_AUTH_ALGO
{
    UINT16 ElementCount;
    _Field_size_(ElementCount) DOT11_AUTH_ALGORITHM* pBuffer;
} BUFFER_DOT11_AUTH_ALGO, *PBUFFER_DOT11_AUTH_ALGO;

typedef struct _BUFFER_DOT11_CIPHER_ALGO
{
    UINT16 ElementCount;
    _Field_size_(ElementCount) DOT11_CIPHER_ALGORITHM* pBuffer;
} BUFFER_DOT11_CIPHER_ALGO, *PBUFFER_DOT11_CIPHER_ALGO;

typedef struct _BUFFER_MAC_ADDRESS
{
    UINT16 ElementCount;   
    _Field_size_(ElementCount) PDOT11_MAC_ADDRESS pBuffer;
} BUFFER_MAC_ADDRESS, *PBUFFER_MAC_ADDRESS;

typedef struct _WFC_BSS_ENTRY_CHANNEL_INFO
{
    UINT32 Channel;
    UINT32 BandId;
} WFC_BSS_ENTRY_CHANNEL_INFO, *PWFC_BSS_ENTRY_CHANNEL_INFO;

typedef struct _WFC_BSS_ENTRY_SIGNAL_INFO
{
    INT32  Rssi;
    UINT32 LinkQuality;
    
} WFC_BSS_ENTRY_SIGNAL_INFO, *PWFC_BSS_ENTRY_SIGNAL_INFO;

typedef struct _WFC_CONNECTION_PROFILE_PARAMETERS
{
    _WFC_CONNECTION_PROFILE_PARAMETERS() :
        SSIDList(),
        BSSIDList(),
        HESSIDList(),
        HotspotIndicationElement(),
        AuthAlgoList(),
        MulticastCipherAlgoList(),
        UnicastCipherAlgoList(),
        AcceptAnyBSSID(TRUE),
        RoamRequest(FALSE),
        HiddenNetwork(FALSE),
        ExcludeUnencrypted(FALSE),
        MFPEnabled(FALSE),
        HostFIPSModeEnabled(FALSE),
        IgnoreAllMACAddresses(FALSE),
        AccessNetworkType(0)
    {
        AuthAlgoList.pBuffer = NULL;
        AuthAlgoList.ElementCount = 0;

        MulticastCipherAlgoList.pBuffer = NULL;
        MulticastCipherAlgoList.ElementCount = 0;

        UnicastCipherAlgoList.pBuffer = NULL;
        UnicastCipherAlgoList.ElementCount = 0;

        SSIDList.pBuffer = NULL;
        SSIDList.ElementCount = 0;

        HESSIDList.pBuffer = NULL;
        HESSIDList.ElementCount = 0;
        HotspotIndicationElement.pBuffer = NULL;
        HotspotIndicationElement.ElementCount = 0;

        // Ugly hack to silence OACR, so it knows that SSIDList.pBuffer->uSSIDLength is in range
        static DOT11_SSID temp = { 0 };
        _Analysis_assume_(SSIDList.pBuffer == &temp);
        _Analysis_assume_(SSIDList.pBuffer->uSSIDLength == 0);

        BSSIDList.pBuffer = NULL;        
        BSSIDList.ElementCount = 0;

        ExcludedMacAddressList.pBuffer = NULL;        
        ExcludedMacAddressList.ElementCount = 0;

    }

    ~_WFC_CONNECTION_PROFILE_PARAMETERS()
    {
        // We dont own any pointers. We dont worry about deleting them
    }


    //
    // Multiple TLV<WDI_TLV_SSID>
    //
    BUFFER_DOT11_SSID SSIDList;
    
    //
    // WDI_TLV_HESSID_INFO
    //
    UINT8 AccessNetworkType;
    BUFFER_DOT11_HESSID HESSIDList;
    BUFFER_UINT8 HotspotIndicationElement;

    //
    // WDI_TLV_AUTH_ALGO_LIST
    //
    BUFFER_DOT11_AUTH_ALGO AuthAlgoList;

    //
    // WDI_TLV_MULTICAST_CIPHER_ALGO_LIST
    //
    BUFFER_DOT11_CIPHER_ALGO MulticastCipherAlgoList;

    //
    // WDI_TLV_UNICAST_CIPHER_ALGO_LIST
    //
    BUFFER_DOT11_CIPHER_ALGO UnicastCipherAlgoList;

    //
    // WDI_TLV_CONNECTION_SETTINGS
    //
    BOOLEAN RoamRequest;
    BOOLEAN HiddenNetwork;
    BOOLEAN ExcludeUnencrypted;
    BOOLEAN MFPEnabled;
    BOOLEAN HostFIPSModeEnabled;
    
    // 
    // Connection Parameters that are consumed by the driver
    //
    BUFFER_MAC_ADDRESS BSSIDList;
    BOOLEAN AcceptAnyBSSID;

    BUFFER_MAC_ADDRESS ExcludedMacAddressList;
    BOOLEAN IgnoreAllMACAddresses;

} WFC_CONNECTION_PROFILE_PARAMETERS, *PWFC_CONNECTION_PROFILE_PARAMETERS;

typedef struct _WFC_DATAPATH_CAPABILITIES
{
    WDI_INTERCONNECT_TYPE InterconnectType;

    UINT8   MaxNumPeers;
    BOOLEAN TxTargetPriorityQueueing;
    UINT16  TxMaxScatterGatherElementsPerFrame;
    BOOLEAN TxExplicitSendCompleteFlagRequired;
    UINT16  TxMinEffectiveFrameSize;
    UINT16  TxFrameSizeGranularity;

    BOOLEAN RxTxForwarding;
    UINT32  RxMaxThroughput;
} WFC_DATAPATH_CAPABILITIES, *PWFC_DATAPATH_CAPABILITIES;

#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
typedef struct _WFC_DEST_ENTRY
{
    DOT11_MAC_ADDRESS Bssid;
    UINT64 Hits;
    WFC_BSS_ENTRY_CHANNEL_INFO ChannelInfo;
} WFC_DEST_ENTRY, *PWFC_DEST_ENTRY;

#define WFC_TRANSITION_DEST_ENTRY_COUNT 8
typedef struct _WFC_TRANSITION_ENTRY
{
    DOT11_MAC_ADDRESS Bssid;
    UINT64 Hits;
    WFC_BSS_ENTRY_CHANNEL_INFO ChannelInfo;
    WFC_DEST_ENTRY DestinationList[WFC_TRANSITION_DEST_ENTRY_COUNT];
} WFC_TRANSITION_ENTRY, *PWFC_TRANSITION_ENTRY;

#define WFC_TRANSITION_ENTRY_COUNT 256
typedef struct _WFC_TRANSITION_CACHE
{
    UINT32 NumberTransitionEntries;
    UINT32 NumberDestinationEntries;
    WFC_TRANSITION_ENTRY TransitionList[WFC_TRANSITION_ENTRY_COUNT];
} WFC_TRANSITION_CACHE, *PWFC_TRANSITION_CACHE;

typedef struct _WFC_TRANSITION_INFO
{
    DOT11_MAC_ADDRESS FromBssid;
    DOT11_MAC_ADDRESS ToBssid;
    WFC_BSS_ENTRY_CHANNEL_INFO FromChannelInfo;
    WFC_BSS_ENTRY_CHANNEL_INFO ToChannelInfo;
} WFC_TRANSITION_INFO, *PWFC_TRANSITION_INFO;
#endif

