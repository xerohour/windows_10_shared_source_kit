/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

BSSList.hpp

Abstract:

Header file for the BSS List

Environment:

Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

// Forward declare
struct _ROAM_CONTROL_PARAMETERS;
typedef _ROAM_CONTROL_PARAMETERS ROAM_CONTROL_PARAMETERS, *PROAM_CONTROL_PARAMETERS;
enum _WFC_ROAM_CONNECT_TRIGGER;
typedef _WFC_ROAM_CONNECT_TRIGGER WFC_ROAM_CONNECT_TRIGGER;
enum _WDI_ROAM_CONFIGURATION_FLAGS;
typedef _WDI_ROAM_CONFIGURATION_FLAGS WDI_ROAM_CONFIGURATION_FLAGS;

//
// Max range for number of bytes that the random number generator can generate
//
#define RNG_BATCH_SIZE 2048


typedef struct _BSS_ENTRY_CONNECT_MATCHING_CRITERIA
{
    WFC_CONNECTION_PROFILE_PARAMETERS* pConnectionParameters;
    ULONGLONG MaxAgeOfEntry;
    DOT11_WFD_GROUP_ID* pGroupIdMatch;
    bool* pRequireSelectedRegistrar;
    bool* pRequireGroupFormation;

    // When set we will include the current peer even if its not visible. This is to
    // handle the roaming case where we dont find the peer in a scan
    bool  bIncludeCurrentPeer;

    // When set, current AP is considered important to 
    // stay with
    bool  bPreferToStayWithCurrentPeer;
    WDI_ROAM_CONFIGURATION_FLAGS roamConfigFlags;
} BSS_ENTRY_CONNECT_MATCHING_CRITERIA;

typedef struct _WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA
{
    _WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA()
    {
        SSIDList.ElementCount = 0;
        SSIDList.pBuffer = NULL;
    }
    
    // Only using SSID
    BUFFER_DOT11_SSID SSIDList;
} WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA, *PWFC_BSS_ENTRY_NLO_MATCHING_CRITERIA;

typedef enum _WFC_BLOCKED_REASON
{
    WFC_BLOCKED_REASON_NONE = 0x00,
    WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED = 0x01,
    WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE = 0x02,
    WFC_BLOCKED_REASON_ALL = 0xFF
} WFC_BLOCKED_REASON;

typedef struct _BSS_ENTRY_BLOCKED_INFO
{
    void Initialize()
    {
        BlockedReasons = WFC_BLOCKED_REASON_NONE;
        
        LastAssocFailTime = 0;
        LastAssocFailLinkQuality = 0;
    }

    ULONG      BlockedReasons;

    ULONGLONG  LastAssocFailTime;
    ULONG      LastAssocFailLinkQuality;
} BSS_ENTRY_BLOCKED_INFO, *PBSS_ENTRY_BLOCKED_INFO;

typedef struct _WDI_STRUCT_FTE
{
    UCHAR ElementId;
    UCHAR Length;
    UCHAR MICControl[SIZEOF_FT_IE_MIC_CONTROL];
    UCHAR MIC[SIZEOF_FT_IE_MIC];
    UCHAR ANonce[SIZEOF_NONCE];
    UCHAR SNonce[SIZEOF_NONCE];
} WDI_STRUCT_FTE, *PWDI_STRUCT_FTE;

typedef enum _WFC_BSS_ENTRY_FRAME_TYPE
{
    WFC_BSS_ENTRY_BEACON_FRAME,
    WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME,
    WFC_BSS_ENTRY_TYPE_UNKNOWN,
} WFC_BSS_ENTRY_FRAME_TYPE;

typedef struct _BSS_ENTRY_WFD_SERVICES_INFO
{
    USHORT usNumServices;
    USHORT usNumMatchedServices;
    USHORT usServicesDescriptorOffset;
    USHORT usServiceNamesDescriptorLength;
    WFD_OTA_ADVERTISED_SERVICE_INFO ServiceInfo[1];
} BSS_ENTRY_WFD_SERVICES_INFO, *PBSS_ENTRY_WFD_SERVICES_INFO;

class CBSSListManager;
class CChannelListManager;
class CBSSEntry
{
public:
    CBSSEntry(
        _In_ const DOT11_MAC_ADDRESS& bssId);    // (in 100 nano-seconds)

    ~CBSSEntry();

    static CBSSEntry *GetBSSEntryFromListEntry(
        _In_ LIST_ENTRY *pBssListEntry);

    PLIST_ENTRY GetListEntry();

    BOOLEAN IsBssIdEqual(
        _In_ const DOT11_MAC_ADDRESS& BssId);

    BOOLEAN IsMatchingBssEntryForConnect(
        _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
        _In_ BOOLEAN bIsFTConnection
        );

    BOOLEAN IsWFDFilledIn(_In_ bool AllowBeaconMatch = false)
    {
        return m_ProbeResponseFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) ||
            (AllowBeaconMatch && m_BeaconFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements));
    }

    ULONGLONG GetUpdateTime()
    {
        return (m_ProbeResponseTime > m_BeaconTime) ?
        m_ProbeResponseTime : m_BeaconTime;
    }

    VOID SetMaxValidityTime(ULONGLONG maxValidityTime)
    {
        m_MaxValidityTime = maxValidityTime;
    }

    PDOT11_MAC_ADDRESS GetBssId()
    {
        return &m_BssID;
    }

    NDIS_STATUS GetSSID(
        _Out_ DOT11_SSID& outSSID);

    PWFC_BSS_ENTRY_CHANNEL_INFO GetChannelInfo()
    {
        return &m_BssChannelInfo;
    }

    PWFC_BSS_ENTRY_SIGNAL_INFO GetSignalInfo()
    {
        return &m_SignalInfo;
    }
    PVARIABLE_LENGTH_BUFFER_UINT8 GetProbeResponse(
        ULONGLONG currentTime)
    {
        // If its too old, return NULL
        if ((m_ProbeResponseTime + m_MaxValidityTime) < currentTime)
        {
            return NULL;
        }

        return &m_ProbeResponseFrameBody;
    }

    PVARIABLE_LENGTH_BUFFER_UINT8 GetBeacon(
        ULONGLONG currentTime)
    {
        // If its too old, return NULL
        if ((m_BeaconTime + m_MaxValidityTime) < currentTime)
        {
            return NULL;
        }

        return &m_BeaconFrameBody;
    }

    PVARIABLE_LENGTH_BUFFER_UINT8 GetBeaconOrProbeResponse();

    PVARIABLE_LENGTH_BUFFER_UINT8 GetDeviceContext()
    {
        return &m_DeviceSpecificContext;
    }

    NDIS_STATUS SetBeaconOrProbeResponse(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _In_ ULONGLONG HostTimeStamp,
        _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
        _In_ UINT16 bufferLength,
        _In_opt_ DOT11_MAC_ADDRESS *pWfdDeviceAddress
        );

    NDIS_STATUS SetDeviceContext(
        _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
        _In_ UINT16 bufferLength
        );

    void SetSignalInfo(
        _In_ INT32 RSSI,
        _In_ UINT32 LinkQuality
        );

    void SetChannelInfo(
        _In_ UINT32 ChannelNumber,
        _In_ UINT32 BandId
        );

    void SetChannelAndPhyID(
        _In_ bool   bOnSTAPort,
        _In_ CAdapterPropertyCache *pAdapterCache,
        _In_ UINT32 ChannelNumber,
        _In_ UINT32 BandId
        );

    WFC_BSS_ENTRY_FRAME_TYPE GetBeaconOrProbeFrame(
        _Out_ PDOT11_BEACON_FRAME       *ppBeacon,
        _Out_ UINT16                    *pBeaconLength
        );

    NDIS_STATUS CopyDot11BSSEntry(
        _Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
        _In_ ULONG      DestinationLength,
        _Out_ PULONG    pBssEntrySize
        );

    NDIS_STATUS CopyDot11WfdBSSEntry(
        _Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
        _In_ ULONG      DestinationLength,
        _Out_ PULONG    pEntrySize
        );

    //
    void SetVisibility(
        bool Visible)
    {
        if (!Visible)
        {
            m_BeaconTime = 0;
            m_ProbeResponseTime = 0;

            // Do not change m_IsCurrentlyAssociatedPeer or blocking state
            // since we may mark an associated or blocked AP as not visible
            
            //
            // Reset PMKID Hint
            //
            m_RsnaAuthAlgosSupported = 0;

            //
            // Reset P2P Hints
            //
            SetChannelHint(0, 0);
            RtlZeroMemory(&m_GroupId, sizeof(m_GroupId));
        }
    }

    void SetAssociatedFlag(
        bool Associated)
    {
        m_IsCurrentlyAssociatedPeer = Associated;
    }

    bool GetAssociatedFlag()
    {
        return m_IsCurrentlyAssociatedPeer;
    }

    void Block(WFC_BLOCKED_REASON reason);

    void UnBlock(ULONG blockedReasons);

    PBSS_ENTRY_BLOCKED_INFO GetBlockedInfo()
    {
        return &m_BlockedInfo;
    }

    ULONG CalculateRank(
        _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
        _In_ PROAM_CONTROL_PARAMETERS pRoamControl
        );

    ULONG GetCachedRank()
    {
        return m_CachedRank;
    }

    ULONG GetCachedRawRank()
    {
        return m_CachedRawRank;
    }

    ULONG GetBssSelectionFlags()
    {
        return m_BssSelectionFlags;
    }

    VOID SetBssSelectionFlags(ULONG newFlags)
    {
        m_BssSelectionFlags = newFlags;
    }

    ULONG GetIsSuggestedByAPBSSTransitionRequest()
    {
        return m_IsSuggestedByAPBSSTransitionRequest;
    }

    VOID SetIsSuggestedByAPBSSTransitionRequest(bool value)
    {
        m_IsSuggestedByAPBSSTransitionRequest = value;
    }

    void UpdateP2PFields(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _In_ ULONG IEsLength,
        _In_bytecount_(IEsLength) BYTE* pIEBlob,
        _In_opt_ DOT11_MAC_ADDRESS *pWfdDeviceAddress
        );

    NDIS_STATUS UpdateBackgroundDeviceIndicatedByDriver(
        _In_ CAdapter *pAdapter
        );

    VOID UpdateBackgroundDeviceLastQueriedTime(
        _In_ CAdapter *pAdapter
        );

    VOID UpdateBackgroundDeviceExpired(
        _In_ CAdapter *pAdapter
        );

    bool IsGroupIdMatch(
        _In_ const DOT11_WFD_GROUP_ID& groupId,
        _In_ bool MatchDeviceAddress,
        _In_ bool MatchSSID,
        _In_ bool MatchAll = true)
    {
        bool isMatch = true;

        if (MatchSSID)
        {
            //
            // If we were passed the wildcard SSID, then just match the Entry's SSID upto the wildcard
            //
            DOT11_SSID wildcardSSID = {7,"DIRECT-"};
            ULONG uEntrySsidLength = m_GroupId.SSID.uSSIDLength;    // Default

            //
            // If this entry is prepended with the wildcard SSID, then change the length to
            // be matched to be that of the wildcard SSID
            //
            if ((m_GroupId.SSID.uSSIDLength > wildcardSSID.uSSIDLength) &&  // Entry's SSID length > wildcard SSID
                (groupId.SSID.uSSIDLength == wildcardSSID.uSSIDLength) &&   // The SSID to be matched is the wildcard
                (memcmp( groupId.SSID.ucSSID,
                         wildcardSSID.ucSSID,
                         wildcardSSID.uSSIDLength ) == 0 ))
            {
                //
                // Set the new length which will be used to match up to the wildcard SSID only
                //
                uEntrySsidLength = groupId.SSID.uSSIDLength;
            }

            isMatch = ((uEntrySsidLength == groupId.SSID.uSSIDLength) &&
                       (memcmp( m_GroupId.SSID.ucSSID,
                                groupId.SSID.ucSSID,
                                uEntrySsidLength) == 0));

            //
            // If the caller requested to match either SSID or Device, then do not exit yet
            //
            if ((!isMatch) &&
                (MatchAll))
            {
                goto exit;
            }
        }

        if (MatchDeviceAddress)
        {
            isMatch = memcmp( m_GroupId.DeviceAddress,
                              groupId.DeviceAddress,
                              sizeof(groupId.DeviceAddress)) == 0;

            if (!isMatch)
            {
                goto exit;
            }
        }

    exit:
        return isMatch;
    }

    void SetGroupID(
        _In_ DOT11_WFD_GROUP_ID* pGroupId)
    {
        RtlCopyMemory(
            &m_GroupId,
            pGroupId,
            sizeof(m_GroupId));
    }

    WFD_OTA_GROUP_CAPABILITY* GetGroupCapability()
    {
        return &m_GroupCaps;
    }

    bool GetSelectedRegistrarState()
    {
        return m_IsSelectedRegistrar;
    }

    void SetChannelHint(
        _In_ UINT32 BandID,
        _In_ UINT32 ChannelNumber)
    {
        if (ChannelNumber != 0)
        {
            m_CachedBandID = BandID;
            m_CachedChannelNumber = ChannelNumber;
            m_CachedChannelInfoAvailable = true;
        }
        else
        {
            m_CachedChannelInfoAvailable = false;
        }
    }

    _Success_(return)
    _Must_inspect_result_
    bool GetChannelHint(
        _Out_ UINT32* pBandID,
        _Out_ UINT32* pChannelNumber)
    {
        if (m_CachedChannelInfoAvailable)
        {
            *pBandID = m_CachedBandID;
            *pChannelNumber = m_CachedChannelNumber;
        }

        return m_CachedChannelInfoAvailable;
    }

    void AddNeighborReport(
        _In_ PDOT11_NEIGHBOR_REPORT_IE pNeighborReportInfoElement)
    {
        if (m_pNeighborReportInfoElement == NULL)
        {
            m_pNeighborReportInfoElement = new DOT11_NEIGHBOR_REPORT_IE;
        }

        RtlCopyMemory(
            m_pNeighborReportInfoElement,
            pNeighborReportInfoElement,
            sizeof(DOT11_NEIGHBOR_REPORT_IE));
    }

    void FlushNeighborReport()
    {
        if (m_pNeighborReportInfoElement != NULL)
        {
            delete m_pNeighborReportInfoElement;
            m_pNeighborReportInfoElement = NULL;
        }
    }

    PDOT11_NEIGHBOR_REPORT_IE GetNeighborReport()
    {
        return m_pNeighborReportInfoElement;
    }

    bool IsNeighborReportSupported()
    {
        return m_IsNeighborReportSupported;
    }

    bool IsAPChannelReportSupported()
    {
        return m_IsAPChannelReportSupported;
    }

    bool IsHESSIDPresent()
    {
        return m_IsHESSIDPresent;
    }

    //
    // 802.11r Fast BSS Transition only supported if AP provided MDIE
    // 
    bool IsFastBSSTransitionSupported()
    {
        return (m_MobilityDomainIEBody.pBuffer != NULL && m_MobilityDomainIEBody.dataLength != 0);
    }

    PVARIABLE_LENGTH_BUFFER_UINT8 GetMobilityDomainIEBody()
    {
        if (m_MobilityDomainIEBody.pBuffer != NULL && 
            m_MobilityDomainIEBody.dataLength != 0)
        {
            return &m_MobilityDomainIEBody;
        }

        return NULL;
    }

    void UpdateFastBSSTransitionCapabilities(
        WFC_BSS_ENTRY_FRAME_TYPE sourceType
        );

    void UpdateRMEnabledCapabilities(
        WFC_BSS_ENTRY_FRAME_TYPE sourceType
        );

    void UpdateRSNAInfo(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType
        );

    void UpdateHESSIDInfo(
        WFC_BSS_ENTRY_FRAME_TYPE sourceType
        );

    VOID OnLinkQualityUpdate(
        _In_ UCHAR  LinkQuality
        );

    ULONG GetPMKIDCandidateFlags()
    {
        return m_PMKIDCandidateFlags;
    }

    void GenerateSNonce(
        _Out_writes_bytes_(bufferLength) PBYTE pBuffer,
        _In_range_(1, RNG_BATCH_SIZE) ULONG bufferLength
        )
    {
        SystemPrng(
            pBuffer, 
            bufferLength
            );
    }

    bool IsCorrectFTAKMSuitesAdvertised()
    {
        return m_AdvertisesCorrectFTAKMSuites;
    }

    bool IsOnlyFTAKMSuiteAdvertised()
    {
        return m_AdvertisesOnlyFTAKMSuite;
    }

private:

    NDIS_STATUS CBSSEntry::UpdateVariableBuffers(
        _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
        _In_ UINT16 bufferLength,
        _Inout_ PVARIABLE_LENGTH_BUFFER_UINT8 pVariableLengthBuffer);

    NDIS_STATUS CBSSEntry::SaveBeaconOrProbeResponse(
        _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
        _In_ UINT16 bufferLength,
        _Inout_ PVARIABLE_LENGTH_BUFFER_UINT8 pVariableLengthBuffer
        );

    void GetIEBlobFromBeaconOrProbeReponse(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _Outptr_result_buffer_(*pBufferLength)PBYTE *pIEBuffer,
        _Out_ ULONG *pBufferLength
        );

    NDIS_STATUS
        FillIEs(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
        _In_ ULONG      DestinationLength,
        _Out_ PULONG    pIEsSize
        );

    NDIS_STATUS
        FillDot11BSSEntry(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _Out_writes_bytes_opt_(DestinationLength) PDOT11_BSS_ENTRY pDestBSSEntry,
        _In_ ULONG      DestinationLength,
        _Out_ PULONG    pBSSEntrySize
        );

    NDIS_STATUS
        FillDot11WfdDeviceEntry(
        _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
        _Out_writes_bytes_opt_(DestinationLength) PDOT11_WFD_DEVICE_ENTRY pDestBSSEntry,
        _In_ ULONG      DestinationLength,
        _Out_ PULONG    pBSSEntrySize
        );

private:
    // List entry for maintaining this in a list
    CPP_LIST_ENTRY m_BssListEntry;

    // Stuff parsed/received from the device
    DOT11_MAC_ADDRESS m_BssID;
    ULONGLONG       m_MaxValidityTime;  // In 100 nanoseconds

    WFC_BSS_ENTRY_SIGNAL_INFO m_SignalInfo;

    VARIABLE_LENGTH_BUFFER_UINT8 m_ProbeResponseFrameBody;
    ULONGLONG       m_ProbeResponseTime;

    VARIABLE_LENGTH_BUFFER_UINT8 m_BeaconFrameBody;
    ULONGLONG       m_BeaconTime;

    WFC_BSS_ENTRY_CHANNEL_INFO   m_BssChannelInfo;

    BSS_ENTRY_BLOCKED_INFO m_BlockedInfo;
    ULONG           m_CachedRank;
    ULONG           m_CachedRawRank;
    ULONG           m_BssSelectionFlags;

    PDOT11_NEIGHBOR_REPORT_IE m_pNeighborReportInfoElement;

    ULONG           m_CachedChannelNumber;
    ULONG           m_CachedBandID;
    bool            m_CachedChannelInfoAvailable;

    ULONG           m_CachedDot11PhyID;
    ULONG           m_CachedChannelCenterFrequency;

    DOT11_MAC_ADDRESS m_DeviceAddress;

    VARIABLE_LENGTH_BUFFER_UINT8 m_DeviceSpecificContext;

    DOT11_WFD_GROUP_ID m_GroupId;
    WFD_OTA_GROUP_CAPABILITY m_GroupCaps;
    bool m_IsSelectedRegistrar;

    ULONG m_RsnaAuthAlgosSupported;
    bool m_AdvertisesCorrectFTAKMSuites;
    bool m_AdvertisesOnlyFTAKMSuite;
    ULONG m_PMKIDCandidateFlags;

    bool m_IsHESSIDPresent;
    DOT11_HESSID m_HESSID;

    ULONG m_BSSEntryDiscoveryContext;
    BOOLEAN m_IsWFDServiceUpdated;
    PBSS_ENTRY_WFD_SERVICES_INFO m_pBeaconWfdServiceNames;
    PBSS_ENTRY_WFD_SERVICES_INFO m_pProbeWfdServiceNames;

    // 802.11r
    VARIABLE_LENGTH_BUFFER_UINT8 m_MobilityDomainIEBody;

    // 802.11k
    bool m_IsNeighborReportSupported;
    bool m_IsAPChannelReportSupported;

    // 802.11v
    bool m_IsSuggestedByAPBSSTransitionRequest;

    bool m_IsCurrentlyAssociatedPeer;
};


//==============================================================================================
/*++
Maintains the BSS list
--*/
class CBSSListManager : public IBSSList
{
public:
    CBSSListManager();

    ~CBSSListManager();

    NDIS_STATUS Initialize(
        _In_ CAdapter *pAdapter,
        _In_ CAdapterPropertyCache *pAdapterCache,
        _In_ WFC_PORT_TYPE PortType);

    // Returns a pointer to the BSS entry in the list
    CBSSEntry* FindBSSEntry(
        _In_ const DOT11_MAC_ADDRESS& BssId);

    //
    // BUGBUG: OS does not provide correct group ID when connecting to
    // P2P client port, so there is an option here to ignore device
    // address when matching against group ID.
    //
    CBSSEntry* FindBSSEntryByGroupID(
        _In_ const DOT11_WFD_GROUP_ID& GroupID,
        _In_ bool MatchDeviceAddress,
        _In_ bool MatchSSID = true,
        _In_ bool MatchAll = true);

    bool CheckForExpiredEntries(
        _Out_ bool *pDeleteExpiredEntries);

    void ExpireOldEntries();

    NDIS_STATUS AddBSSEntry(
        _In_ CBSSEntry* bssEntry);

    void MoveBSSEntryToHeadOfList(
        _In_ CBSSEntry* bssEntry);

    NDIS_STATUS FindMatchingBSSEntriesForConnect(
        _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
        _In_ ULONG MaxAgeOfEntries100nS,
        _Inout_ ULONG* pMatchingEntryCount,
        _Out_writes_opt_(*pMatchingEntryCount) CBSSEntry** ppBssEntryList);

    NDIS_STATUS CBSSListManager::FindMatchingBSSEntriesForNlo(
        _In_ WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA* pMatchingCriteria,
        _In_ ULONG MaxAgeOfEntries100nS,
        _Inout_ bool* MatchFound);

    NDIS_STATUS GetChannelListsForNeighborScan(
        _In_ CChannelListManager *pChannelListManager
        );

    bool HasNewerBssEntries(
        _In_  ULONGLONG oldestAllowedEntry
        );

    NDIS_STATUS
        GetDot11BSSEntryList(
        _In_  ULONGLONG oldestAllowedEntry,
        _Inout_updates_bytes_(TotalLength)
        PDOT11_BYTE_ARRAY    Dot11ByteArray,
        _In_  ULONG                   TotalLength
        );

    NDIS_STATUS
        CopyWfdDevicesList(
        _Inout_updates_bytes_(TotalLength)
        PBYTE pDot11Buffer,
        _In_  ULONG TotalLength,
        _Out_ ULONG *pNumEntries,
        _Out_ ULONG *pTotalListEntries,
        _Out_ ULONG *pBytesCopied,
        _Out_ ULONG *pTotalListLength
        );

    NDIS_STATUS
        GetDot11WfdDeviceEntryList(
        _Inout_updates_bytes_(TotalLength)
        PDOT11_BYTE_ARRAY    Dot11ByteArray,
        _In_  ULONG                   TotalLength
        );

    NDIS_STATUS
        GetWfdDiscoverRequestResultList(
        _Out_writes_bytes_(TotalLength)
        PDOT11_WFD_DISCOVER_COMPLETE_PARAMETERS pDiscoverCompleteParams,
        _Inout_ ULONG TotalLength);

    void RemoveBSSEntry(
        _In_ const DOT11_MAC_ADDRESS& BssId);


    void FlushAllEntries(
        _In_ bool bFullFlush
        );

    void FlushAllNeighborReports();

    void FlushAllAssociationState();    

    NDIS_STATUS
        RegisterForChangeNotifications(
        _In_ INotifyBSSListChangedCallback* pCallback)
    {

            if (pCallback != NULL &&
                m_pChangeCallback != NULL)
            {
                NT_ASSERT(FALSE);
                return NDIS_STATUS_INVALID_STATE;
            }

            m_pChangeCallback = pCallback;
            return NDIS_STATUS_SUCCESS;
        }

    void NotifyBSSListChanged()
    {
        if (m_pChangeCallback != NULL)
        {
            m_pChangeCallback->OnBSSListChanged(this);
        }
    }

    void SetBackgroundDiscoveryEnabled(
        bool BackgroundDiscoveryEnabled)
    {
        m_BackgroundDiscoveryEnabled = BackgroundDiscoveryEnabled;
    }

    bool IsBackgroundDiscoveryEnabled()
    {
        return m_BackgroundDiscoveryEnabled;
    }

public:
    // For BSS list caching
    void SetBSSListCacheMode(
        _In_ bool IHVMaintainedExtSTABSSListCache
        );

    bool IsIHVMaintainedExtSTABSSListCache();    
    
    void OnScanStart(
        _In_ WDI_SCAN_TRIGGER  m_ScanTrigger
        );

    void OnScanComplete();

    void OnBSSListUpdateComplete(
        _In_ NDIS_STATUS CompletionStatus
        );

    bool DoesBSSListNeedUpdate(
        _In_ ULONGLONG MaximumAgeOfIhvCachedList
        );

    bool IsScanInProgress(
        _Out_ WDI_SCAN_TRIGGER  *pScanInProgressTrigger
        );

private:
    INotifyBSSListChangedCallback* m_pChangeCallback;
    CAdapter              *m_pAdapter;
    CAdapterPropertyCache *m_pAdapterCache;
    WFC_PORT_TYPE          m_PortType;
    ULONG                  m_MaxBSSEntryCount;
    ULONG                  m_CurrentBSSEntryCount;
    bool                   m_IHVMaintainedExtSTABSSListCache;
    bool                   m_ScanInProgress;
    WDI_SCAN_TRIGGER       m_ScanInProgressTrigger;
    bool                   m_BackgroundDiscoveryEnabled;
    ULONGLONG              m_BSSListUpdateTime;
    ULONGLONG              m_BssEntryExpiryTime;
    ULONGLONG              m_LastExpiredEntriesCheckTime;
    LIST_ENTRY m_BSSEntryList;
};

//==============================================================================================
/*++
Maintains the band/channel list
--*/
class CChannelListManager
{
public:
    CChannelListManager();

    ~CChannelListManager();

public:
    ULONG GetNumberOfChannels(UINT32 band);

    WDI_CHANNEL_NUMBER * GetChannelList(UINT32 band);

    NDIS_STATUS AddChannel(UINT32 channel, UINT32 band);

private:
    //
    // Limit the size of the channel list used to for quick scans
    // to something reasonable. We expect to only be scanning about 2-5
    // channels during a quick scan.
    //
    static const ULONG c_MaxNeighborScanChannelListCount = 10;

    WDI_CHANNEL_NUMBER m_ChannelList24GHz[c_MaxNeighborScanChannelListCount];
    ULONG m_ChannelList24GHzCount;
    WDI_CHANNEL_NUMBER m_ChannelList5GHz[c_MaxNeighborScanChannelListCount];
    ULONG m_ChannelList5GHzCount;
};

//==============================================================================================

class CP2PBackgroundContext
{
public:
    CP2PBackgroundContext(
        ) :
        m_LastTimeIndicatedByDriver(0),
        m_bLastVisibleState(0)
    {
        InitializeCppListEntry(this, &m_P2PBackgroundContextListEntry);
    }


    ~CP2PBackgroundContext()
    {
    }

    static CP2PBackgroundContext*
    GetP2PBackgroundContextFromListEntry(
        _In_ LIST_ENTRY *pP2PBackgroundListEntry)
    {
        return (CP2PBackgroundContext*) GetCppObjectFromListEntry (pP2PBackgroundListEntry);
    }

    PLIST_ENTRY
    GetListEntry()
    {
        return &m_P2PBackgroundContextListEntry.ListEntry;
    }

    BOOLEAN
    ClearVisibleStateAtQueryTime()
    {
        BOOLEAN bLastState = m_bLastVisibleState;
        m_bLastVisibleState = FALSE;

        return (bLastState);
    }

    BOOLEAN
    SetVisibleStateAtQueryTime()
    {
        BOOLEAN bLastState = m_bLastVisibleState;

        m_bLastVisibleState = TRUE;
        return (bLastState);
    }

    BOOLEAN
    CheckForStateChangeOnDriverIndication(
        _In_ ULONGLONG currentTime
        )
    {
        m_LastTimeIndicatedByDriver = currentTime;

        //
        // If the current query state is not-visible, then return true
        //
        return (m_bLastVisibleState ? FALSE : TRUE);
    }

    BOOLEAN
    CheckForNotVisibleStateChange(
        _In_ ULONGLONG currentTime,
        _In_ ULONGLONG deviceNotVisibleTimeout
        )
    {
        //
        // Device is no longer visible if it had been reported as visible earlier, but is not longer visible
        //
        if ((m_bLastVisibleState) &&
            (currentTime > (m_LastTimeIndicatedByDriver + deviceNotVisibleTimeout)))
        {
            m_bLastVisibleState = FALSE;
            return (TRUE);
        }

        return (FALSE);
    }

private:
    // List entry for maintaining this in a list
    CPP_LIST_ENTRY m_P2PBackgroundContextListEntry;

    ULONGLONG m_LastTimeIndicatedByDriver;
    BOOLEAN m_bLastVisibleState;
};

//
// P2P Background Device Context
//
class CP2PBackgroundDeviceContext : public CP2PBackgroundContext
{
public:
    CP2PBackgroundDeviceContext(
        _In_ const DOT11_MAC_ADDRESS *pDeviceAddress
        )
    {
        RtlCopyMemory(&m_DeviceAddress, pDeviceAddress, sizeof(DOT11_MAC_ADDRESS));
    }

    ~CP2PBackgroundDeviceContext()
    {
    }

    BOOLEAN
    IsContextMatch(
        _In_ const DOT11_MAC_ADDRESS *pDeviceAddress
        )
    {
        return (0 == memcmp(pDeviceAddress, &m_DeviceAddress, sizeof(DOT11_MAC_ADDRESS)));
    }

    PDOT11_MAC_ADDRESS
    GetContext(
        )
    {
        return (&m_DeviceAddress);
    }

private:
    DOT11_MAC_ADDRESS m_DeviceAddress;
};

//
// P2P Background Service Context
//
class CP2PBackgroundServiceContext : public CP2PBackgroundContext
{
public:
    CP2PBackgroundServiceContext(
        _In_ const UCHAR uServiceNameLength,
        _In_reads_bytes_(uServiceNameLength) PUCHAR pucServiceName
        )
    {
        m_uServiceNameLength = min(uServiceNameLength, DOT11_WFD_SERVICE_NAME_MAX_LENGTH);

        RtlZeroMemory(m_ucServiceName, sizeof(m_ucServiceName));
        for (ULONG i = 0; i < m_uServiceNameLength; i++)
        {
            // Convert ASCII characters to lower case (tolower will ignore multi-byte UTF-8 characters)
            m_ucServiceName[i] = (UCHAR) tolower (pucServiceName[i]);
        }
    }

    ~CP2PBackgroundServiceContext()
    {
    }

    BOOLEAN
    IsContextMatch(
        _In_ const UCHAR uServiceNameLength,
        _In_reads_bytes_(uServiceNameLength) PUCHAR pucServiceName
        )
    {
        //
        // First, check if the sizes match
        //
        if (uServiceNameLength != m_uServiceNameLength)
        {
            return (FALSE);
        }

        //
        // Compare lower case characters - the member name is already saved in lower-case
        //
        for (ULONG i = 0; i < m_uServiceNameLength; i++)
        {
            if (m_ucServiceName[i] != (tolower (pucServiceName[i])))
            {
                return (FALSE);
            }
        }

        return (TRUE);
    }

    PUCHAR
    GetContext(
        )
    {
        return (m_ucServiceName);
    }

private:
    UCHAR   m_uServiceNameLength;
    UCHAR   m_ucServiceName[DOT11_WFD_SERVICE_NAME_MAX_LENGTH+1];
};


class CServiceComebackResponseContext
{
public:
    CServiceComebackResponseContext(
    _In_ const USHORT MaxResponseFragmentSize,
    _In_ const WDI_MAC_ADDRESS *pSourceAddress,
    _In_ const UCHAR DialogToken,
    _In_ const DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
    _In_ USHORT uQueryResponseLength,
    _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse);

    ~CServiceComebackResponseContext();

    static CServiceComebackResponseContext*
    GetComebackResponseContextFromListEntry(
        _In_ LIST_ENTRY *pComebackResponseListEntry)
    {
        return (CServiceComebackResponseContext*)GetCppObjectFromListEntry(pComebackResponseListEntry);
    }

    PLIST_ENTRY
    GetListEntry()
    {
        return &m_ComebackResponseListEntry.ListEntry;
    }

    void
    UpdateContext(
        _In_ DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
        _In_ USHORT uQueryResponseLength,
        _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse);

    NDIS_STATUS
    GetNextFragmentToSend(
        _Out_ UCHAR *puAdvertisementProtocolID,
        _Out_ DOT11_ANQP_FRAGMENT_ID *pFragmentID,
        _Out_ USHORT *puQueryResponseLength,
        _Outptr_result_bytebuffer_(*puQueryResponseLength) BYTE** ppQueryResponse);

    bool
    IsComebackResponseMatch(
        _In_ const WDI_MAC_ADDRESS *pSourceAddress,
        _In_ const UCHAR DialogToken)
    {
        if  ((DialogToken == m_DialogToken) &&
             (0 == memcmp(&m_SourceAddress, pSourceAddress, sizeof(WDI_MAC_ADDRESS))))
        {
            SetLastTouchedTime();
            return true;
        }

        return false;
    }

    bool
    IsEntryExpired(
        _In_ ULONGLONG ExpiryTime)
    {
        return (GetLastTouchedTime() < ExpiryTime);
    }

private:
    ULONGLONG
    GetLastTouchedTime()
    {
        return (m_LastTouchedTime);
    }

    void
    SetLastTouchedTime()
    {
        m_LastTouchedTime = CSystem::get_CurrentTime();
    }

private:
    // List entry for maintaining this in a list
    CPP_LIST_ENTRY m_ComebackResponseListEntry;

    USHORT m_MaxResponseFragmentSize;
    WDI_MAC_ADDRESS m_SourceAddress;
    UCHAR m_DialogToken;
    USHORT m_uQueryResponseLength;
    PBYTE m_pQueryResponse;

    UCHAR m_uNextFragmentID;
    USHORT m_BytesLeft;
    PBYTE m_pNextQueryResponse;

    DOT11_AVERTISEMENT_PROTOCOL_ELEMENT m_AdvertisementProtocol;

    ULONGLONG m_LastTouchedTime;
};


/*++
Maintains the Services list
--*/
class CServicesManager : public ITimerCallback
{
public:
    CServicesManager();
    ~CServicesManager();

    NDIS_STATUS
    Initialize(
        _In_ CAdapter* pAdapter,
        _In_ CAdapterPropertyCache *pAdapterCache);

    //
    // Comeback Response Context handling
    //
    USHORT
    GetMaxResponseFragmentSize()
    {
        return (m_MaxResponseFragmentSize);
    }

    CServiceComebackResponseContext*
    FindComebackResponseContext(
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ UCHAR DialogToken);

    CServiceComebackResponseContext*
    AddComebackResponseContext(
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ UCHAR DialogToken,
        _In_ DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
        _In_ USHORT uQueryResponseLength,
        _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse);

    void
    RemoveComebackResponseContext(
        _In_ CServiceComebackResponseContext* pServiceComebackResponseContext);

    //
    // Background Service Discovery handling
    //
    VOID
    InitializeForP2PDeviceQuery();

    VOID
    StartDeviceExpiryNotificationTimer();

    VOID
    CheckStartDeviceUpdateNotificationTimer();

    NDIS_STATUS
    SetNewBackgroundDiscoveryContexts(
        _In_ BOOLEAN IndicateFilteredObjectsOnly,
        _In_ ULONG DiscoveryBufferLength,
        _In_reads_bytes_(DiscoveryBufferLength) PBYTE pDiscoveryBuffer);

    NDIS_STATUS
    UpdateBackgroundDeviceIndicatedByDriver(
        _Inout_ ULONG *pBSSEntryDiscoveryContext,
        _Inout_ BOOLEAN *pIsWFDServiceUpdated,
        _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
        _In_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
        _In_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
        );

    NDIS_STATUS
    UpdateBackgroundDeviceExpired(
        _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
        _In_ ULONG BSSEntryDiscoveryContext,
        _In_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
        _In_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
        );

    ULONG
    CheckForNewDevices(
        _In_ ULONGLONG currentTime,
        _In_ DOT11_MAC_ADDRESS *pDeviceAddress
        );

    ULONG
    CheckForNewServices(
        _In_ ULONGLONG currentTime,
        _In_ PBSS_ENTRY_WFD_SERVICES_INFO pWfdServiceNames
        );

    ULONG
    CheckForExpiredDevicesAndServices(
        _In_ ULONGLONG currentTime
        );

    VOID
    MarkP2PBackgroundContextsQueried(
        _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
        _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
        _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
        );

    ULONG
    MarkP2PBackgroundServicesQueried(
        _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pWfdServiceNames
        );

private:
    void
    FlushDiscoveryContexts();

    void
    FlushComebackResponseContexts();

    void
    ExpireOldComebackEntries();

    //
    // ITimerCallback Implementation
    //
    void
    OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);

private:
    CAdapter *m_pAdapter;
    CAdapterPropertyCache *m_pAdapterCache;
    NDIS_PORT_NUMBER m_NdisDevicePortNumber;
    USHORT m_MaxResponseFragmentSize;

    LIST_ENTRY m_PendingComebackResponseList;
    ULONG m_PendingComebackResponseCount;

    bool m_P2PBackgroundNotificationsEnabled;
    BOOLEAN m_NotifyAllDevicesAndServices;
    BOOLEAN m_WaitingForP2PDeviceQuery;

    LIST_ENTRY m_TrackedP2PDevicesList;
    LIST_ENTRY m_TrackedP2PServicesList;
    ULONG m_TrackedP2PServicesCount;
    ULONG m_TrackedP2PDevicesCount;
    ULONG m_AllCurrentDevices;
    ULONG m_AllNewDevices;
    ULONG m_AllExpiredDevices;
    ULONG m_AllUpdatedServices;
    ULONG m_VisibleFilteredDevices;
    ULONG m_VisibleFilteredServices;
    ULONG m_CurrentDiscoveryContext;
    WFC_TIMER_REGISTRATION_TOKEN m_ExpiredDevicesCheckTimerToken;
    WFC_TIMER_REGISTRATION_TOKEN m_DeviceUpdateNotificationTimerToken;
    BOOLEAN m_DeviceUpdateNotificationTimerRunning;
    BOOLEAN m_DeviceExpiryNotificationTimerRunning;
    CNdisSpinLock *m_DeviceAndServiceUpdateLock;

    ULONGLONG m_WfdDeviceExpiryTimeNSecs;
    ULONG m_WfdDeviceExpiryCheckTimeoutMSecs;
    ULONG m_WfdDeviceUpdateNotificationDelayTimeoutMSecs;
};



