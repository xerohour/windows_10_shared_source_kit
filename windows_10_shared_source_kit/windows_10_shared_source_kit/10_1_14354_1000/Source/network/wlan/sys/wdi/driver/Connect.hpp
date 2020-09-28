
#pragma once


//==============================================================================================
typedef enum _WFC_ROAM_CONNECT_TRIGGER
{
    WfcRoamConnectTriggerFirstTimeConnect,
    WfcRoamConnectTriggerAssociationLoss,                 // Not currently connected
    WfcRoamConnectTriggerToFindBetterAP_BestEffort,       // Find a better AP (OS Triggered)
    WfcRoamConnectTriggerToFindBetterAP_Critical,         // Find a better AP (Critical - IHV Triggered roam)
    WfcRoamConnectTriggerBssTransitionRequest,            // BSS Transition Request
    WfcRoamConnectTriggerBssTransitionRequestCritical,    // BSS Transition Request Critical (Disassociation imminent)
    WfcRoamConnectTriggerResumeFromDx,                    // Resume from Dx (not currently connected)
    WfcRoamConnectTriggerToFindBetterAP_Opportunistic     // Opportunistically find a better AP (OS Triggered)
} WFC_ROAM_CONNECT_TRIGGER;

typedef enum _WDI_ROAM_CONFIGURATION_FLAGS
{
    RC_CONNECT = 1 << 0,
    RC_RECONNECT = 1 << 1,
    RC_CONNECTED_ROAM = 1 << 2,
    RC_BSS_TRANSITION_MANAGEMENT_REQUEST = 1 << 3,
    RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL = 1 << 4,
    RC_BYPASS_SCAN_THROTTLING = 1 << 5,
    RC_RESUME_FROM_DX = 1 << 6,
    RC_BEST_EFFORT = 1 << 7,
    RC_CHECK_GOOD_ENOUGH_AP = 1 << 8,
    RC_USE_CHANNEL_HISTORY = 1 << 9,
    RC_INCLUDE_SSID = 1 << 10,
    RC_USE_ROAM_EXPIRY_TIME = 1 << 11,
    RC_DEFERRABLE_ROAM = 1 << 12,

    RC_MAX = 1 << 31
} WDI_ROAM_CONFIGURATION_FLAGS;

inline WDI_ROAM_CONFIGURATION_FLAGS operator|(WDI_ROAM_CONFIGURATION_FLAGS a, WDI_ROAM_CONFIGURATION_FLAGS b)
{
    return static_cast<WDI_ROAM_CONFIGURATION_FLAGS>(static_cast<int>(a) | static_cast<int>(b));
}
inline WDI_ROAM_CONFIGURATION_FLAGS operator&(WDI_ROAM_CONFIGURATION_FLAGS a, WDI_ROAM_CONFIGURATION_FLAGS b)
{
    return static_cast<WDI_ROAM_CONFIGURATION_FLAGS>(static_cast<int>(a) & static_cast<int>(b));
}
inline WDI_ROAM_CONFIGURATION_FLAGS operator~(WDI_ROAM_CONFIGURATION_FLAGS a)
{
    return static_cast<WDI_ROAM_CONFIGURATION_FLAGS>(~(static_cast<int>(a)));
}
inline WDI_ROAM_CONFIGURATION_FLAGS& operator&=(WDI_ROAM_CONFIGURATION_FLAGS& a, WDI_ROAM_CONFIGURATION_FLAGS b)
{
    a = a & b;
    return a;
}
inline bool TestRoamConfig(WDI_ROAM_CONFIGURATION_FLAGS a, WDI_ROAM_CONFIGURATION_FLAGS b)
{
    return ((static_cast<int>(a) & static_cast<int>(b)) != 0);
}

typedef struct _ROAM_CONTROL_PARAMETERS
{
    // Roam when the link quality falls below this value
    ULONG BetterAPRoamLinkQualityThreshold;

    // Attempt Roam on Non-Enterprise network
    BOOLEAN  BetterAPRoamOnNonEnterpriseNetworks;

    // Number of times we would attempt to roam to a better AP when change is insignificant
    ULONG BetterAPRoamReattemptCount;

    // How much link quality on AP must change for us think its significantly different
    ULONG APLinkQualitySignificantChangeBar;

    // Timegap between better AP roams when the change is insignificant
    ULONG BetterAPRoamReattemptTimeGapSeconds;

    // Time to block a failed Roam AP when the change is insignificant
    ULONG BetterAPRoamReattemptFailedBlockTimeSeconds;

    // Cancel scan when good enough AP found
    BOOLEAN ScanCancelOnGoodAPFoundPermitted;

    // 5GHz boost parameters
    ULONG FiveGHzLowBoost; //Boost at FiveGHzBoostLinkQualityStart
    ULONG FiveGHzBoostLinkQualityStart; //Link quality at which 5GHz rank boosting starts
    ULONG FiveGHzBoostLinkQualityEnd; //Link quality at which 5GHz rank boosted to 100;
} ROAM_CONTROL_PARAMETERS, *PROAM_CONTROL_PARAMETERS;

extern ROAM_CONTROL_PARAMETERS g_RoamControlParametersTable[];

class CConnectHelpers
{

public:
    static NDIS_STATUS PopulateConnectionParameters(
        _In_ CPortPropertyCache* pPortPropertyCache,
        _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectParameters
        );

    static PROAM_CONTROL_PARAMETERS GetRoamControlParameters(
        _In_ CAdapterPropertyCache* pAdapterPropertyCache
        );

    static WDI_ROAM_CONFIGURATION_FLAGS GetRoamConfig(WFC_ROAM_CONNECT_TRIGGER trigger);

    static NDIS_STATUS FillConnectBSSEntryTLV(
        _In_ CPort* pPort,
        _In_ bool bFTAllowed,
        _In_ ULONG               BSSCandidateCount,
        _In_opt_count_(BSSCandidateCount)CBSSEntry** pBSSCandidateList,
        _In_ ArrayOfElements<WDI_CONNECT_BSS_ENTRY_CONTAINER> *pWDIBSSEntryList,
        _In_ bool bRoamRequest
        );

    // Returns true of the WDI_ASSOC_STATUS confirms that this failure was
    // because of something that the peer did
    static bool IsFailedByPeerAction(
        _In_ WDI_ASSOC_STATUS WdiAssocStatus
        )
    {
        if ((WdiAssocStatus == WDI_ASSOC_STATUS_PEER_DEAUTHENTICATED)   ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_PEER_DISASSOCIATED)     ||

            (WdiAssocStatus == WDI_ASSOC_STATUS_AUTH_RESPONSE_CAPABILITY_MISMATCH)      ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_BAD_AUTH_RESPONSE)      ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_AUTH_FAILED_BY_PEER)    ||

            (WdiAssocStatus == WDI_ASSOC_STATUS_ASSOC_RESPONSE_CAPABILITY_MISMATCH)     ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_BAD_ASSOC_RESPONSE)     ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_ASSOC_FAILED_BY_PEER)
            )
        {
            return true;
        }
        return false;
    }

    // Returns true of the WDI_ASSOC_STATUS confirms that this status tells us
    // that the peer is not around
    static bool IsPeerNotReachable(
        _In_ WDI_ASSOC_STATUS WdiAssocStatus
        )
    {
        if ((WdiAssocStatus == WDI_ASSOC_STATUS_UNREACHABLE)            ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_NO_BEACON_PROBE_RESPONSE)               ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_DISASSOCIATE_NOT_VISIBLE)               ||

            (WdiAssocStatus == WDI_ASSOC_STATUS_AUTH_REQUEST_NO_ACK)                    ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_NO_AUTH_RESPONSE)                       ||
            
            (WdiAssocStatus == WDI_ASSOC_STATUS_ASSOC_REQUEST_NO_ACK)                   ||
            (WdiAssocStatus == WDI_ASSOC_STATUS_NO_ASSOC_RESPONSE)
            )
        {
            return true;
        }
        return false;

    }

    _Success_(return == NDIS_STATUS_SUCCESS)
    static NDIS_STATUS GetAdapterCapabilitiesFromPropertyCache(
    _In_ CAdapter* pAdapter,
    _Out_ PDOT11_ADAPTER_CAPABILITIES* ppAdapterCapabilities)
    {
            NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
            ULONG uBufferLength;
            BYTE* pBuffer = NULL;
            ndisStatus = pAdapter->GetAdapterPropertyCache()->GetPropertyBuffer(
                WfcAdapterPropertyEnhancedCapabilities,
                &uBufferLength,
                &pBuffer
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                goto exit;
            }

            *ppAdapterCapabilities = (PDOT11_ADAPTER_CAPABILITIES)pBuffer;
        exit:
            return ndisStatus;
    }

    static bool IsFastTransitionAllowed(
        _In_ CPort* pPort,
        _In_ CAdapter* pAdapter,
        _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectionParameters
        );
};
//==============================================================================================


// Forward declare
class CBSSEntry;

typedef struct _WFC_CONNECT_JOB_PARAMETERS
{
    WFC_CONNECTION_PROFILE_PARAMETERS pProfileParameters;

    _WFC_CONNECT_JOB_PARAMETERS() :
        pProfileParameters()    
    {
        m_PmkidEntryCount = 0;
        pPmkidEntries = NULL;

        ExtraAssociationRequestIEs.ElementCount = 0;
        ExtraAssociationRequestIEs.pBuffer = NULL;
    }
    
    // 
    // PMKID List
    //
    UINT16 m_PmkidEntryCount;
    PDOT11_PMKID_ENTRY pPmkidEntries;

    //
    // WDI_TLV_EXTRA_ASSOCIATION_REQUEST_IES
    //
    BUFFER_UINT8 ExtraAssociationRequestIEs;
        
    //
    // Multiple TLV < WDI_CONNECT_BSS_ENTRYLIST >
    //
    ULONG m_ConnectionCandidateCount;
    CBSSEntry** ppConnectionCandidates;

} WFC_CONNECT_JOB_PARAMETERS;

typedef enum _WFC_CONNECT_SCAN_TYPE
{
    WfcConnectScanTypeNone = 0,
    WfcConnectScanTypeNeighborOrInstantConnect = 1, // Neighbor OR Instant Connect
    WfcConnectScanTypeChannelHint = 2,
    WfcConnectScanTypeActiveOnly = 3,
    WfcConnectScanTypeFull = 4,
    WfcConnectScanTypeMaximumAllowed = 5
} WFC_CONNECT_SCAN_TYPE;


class CConnectJob : public COidJobBase,
    public INotifyDeviceIndicationCallback,
    public ITimerCallback
{
public:
    CConnectJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CConnectJob();

public:
    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

    bool IsCancelled()
    {
        return m_IsCancelled;
    };

    bool IsDeferred()
    {
        return m_IsDeferred;
    };

    WFC_CONNECT_SCAN_TYPE get_LastScanType()
    {
        return m_LastScanType;
    }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    NDIS_STATUS InitializeForRoamReconnectRequest(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ bool IsP2PClient,
        _In_ WFC_ROAM_CONNECT_TRIGGER ConnectTrigger,
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags,
        _In_ WFC_CONNECT_SCAN_TYPE InitialScanType
        );


    VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

public:
    //
    // INotifyDeviceIndicationCallback Implementation
    //
    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                                EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer);

    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);


    WDI_ASSOC_STATUS GetFinalAssociationStatus()
    {
        return m_ConnectCompletionStatus;
    }

private:
    void NotifyConnectionStartIfNecessary();

    void NotifyConnectionCompletionIfNecessary(
        _In_ NDIS_STATUS CompletionStatus);

    void NotifyRoamingAttemptExhausted();
    
    ULONG NotifyAssociationStartAndCompletionIfNecessary(
        _In_ CBSSEntry* pBssEntry,
        _In_ WDI_ASSOCIATION_RESULT_CONTAINER* pAssociationResult
        );
    void FinishJob(
        _In_ NDIS_STATUS ErrorStatus);

    NDIS_STATUS CheckAndUpdateCandidates();

    NDIS_STATUS CheckAndStartConnectProcess(
        _Out_ WDI_ASSOC_STATUS* pConnectStartStatus);

    NDIS_STATUS CheckAndStartScanProcess(
        _Out_ WDI_ASSOC_STATUS* pScanStartStatus);

    NDIS_STATUS FillConnectionProfileParameters(
        _In_ CPortPropertyCache* pPortPropertyCache,
        _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectParameters
        );

    NDIS_STATUS PickCandidates(
        _In_ WFC_CONNECTION_PROFILE_PARAMETERS  *pConnectionParameters
        );

    NDIS_STATUS StartSetPrivacyExemptionIfNecessary(
        _Out_ bool *pPrivacyExemptionSent
        );

    NDIS_STATUS StartBssListUpdate();

    NDIS_STATUS StartScanJob(
        _In_ WFC_SCAN_JOB_PARAMETERS* pScanJobParameters
        );

    NDIS_STATUS CompleteScanJob(
        _In_ NDIS_STATUS OperationStatus
        );

    void CConnectJob::CompleteSendResponseActionFrameJob(
        _In_ NDIS_STATUS OperationStatus
        );

    bool CheckAndRequestPMKIDUpdate(
        _In_ ULONG               BSSCandidateCount,
        _In_opt_count_(BSSCandidateCount)CBSSEntry** pBSSCandidateList
        );

    NDIS_STATUS StartWaitForPMKIDList();

    void OnPMKIDListUpdateNotificationArrived(
        _In_ ULONG  BufferLength,
        _In_reads_bytes_(BufferLength) BYTE* Buffer);

    VOID StopWaitForPMKIDList();

    NDIS_STATUS FillConnectRoamTaskParameters(
        _In_ PWDI_CONNECT_PARAMETERS_CONTAINER pConnectParameters,
        _In_ ArrayOfElements<WDI_CONNECT_BSS_ENTRY_CONTAINER> *pPreferredBSSEntryList,
        _In_ bool RoamRequest
        );

    // Options used in StartConnectRoamTask
    enum _START_CONNECT_ROAM_CANDIDATE_LIST_CREATION_OPTIONS {
        START_CONNECT_ROAM_RECREATE_CANDIDATE_LIST          = 0,
        START_CONNECT_ROAM_CANDIDATE_LIST_AVAILABLE         = 1
    };

    enum _START_CONNECT_ROAM_NO_CANDIDATE_CONTINUE_OPTIONS {
        START_CONNECT_ROAM_STOP_IF_NO_CANDIDATES_FOUND      = 0,
        START_CONNECT_ROAM_CONTINUE_IF_NO_CANDIDATE_FOUND   = 1
    };
    NDIS_STATUS StartConnectRoamTask(
        _In_  _START_CONNECT_ROAM_CANDIDATE_LIST_CREATION_OPTIONS CandidateGenerationOptions,
        _In_  _START_CONNECT_ROAM_NO_CANDIDATE_CONTINUE_OPTIONS NoCandidateOptions,
        _Out_ WDI_ASSOC_STATUS* pConnectStartStatus);        

    NDIS_STATUS ReorderAssociationPhyTypesList(
        _In_     WDI_ASSOCIATION_RESULT_CONTAINER* pAssociationResult
        );

    void OnAssociationNotificationArrived(
        _In_ ULONG  EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer);

    NDIS_STATUS CompleteConnectTask(
        _In_ NDIS_STATUS OperationStatus);

    _Success_(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GenerateRoamTaskTlv(
        _Out_ ULONG* pBufferLength,
        _Outptr_result_buffer_(*pBufferLength) UINT8** ppBuffer 
        );

    _Success_(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS GenerateConnectTaskTlv(
        _In_ bool RoamRequest,
        _Out_ ULONG* pBufferLength,
        _Outptr_result_buffer_(*pBufferLength) UINT8** ppBuffer 
        );

    void TraceLoggingRoamFinished(
        _In_ NDIS_STATUS OperationStatus
        );

    void OnBssEntryListNotification();

protected:
    //
    // Limit the size of the channel list used to for quick scans
    // to something reasonable. We expect to only be scanning about 2-5
    // channels during a quick scan.
    //
    static const ULONG c_MaxQuickScanChannelListCount = 10;

    enum 
    {
        WiFiConnectJobStateInit,
        WiFiConnectJobStateWaitingForPrivacyExemptionListSet,
        WiFiConnectJobStateWaitingForBSSListUpdate,     // First BSS list update
        WiFiConnectJobStateWaitingForScanJobCompletion, // Scan Job & BSS list updated
        WiFiConnectJobStateWaitingForActionFrameComplete, // BSS Transition Management Response action frame complete
        WiFiConnectJobStateWaitingForPMKIDUpdate,
        WiFiConnectJobStateWaitingForConnectCompletion,
        WiFiConnectJobStateCompleted
    } m_ConnectState;

    bool                    m_IsCancelled;
    BYTE*                   m_pSetConnectionParametersBuffer;
    ULONG                   m_SetConnectionParametersBufferLength;

    bool                    m_NotifiedConnectionStarted;
    bool                    m_NotifiedSuccessfulAssociation;
    WDI_ASSOC_STATUS        m_ConnectCompletionStatus;

    WFC_REGISTRATION_TOKEN    m_AssocNotificationRegistrationToken;
    WDI_ASSOC_STATUS          m_RoamWabiReason;
    WFC_ROAM_CONNECT_TRIGGER  m_ConnectTrigger;
    //bool                      m_bRoamForResumeS4;
    WDI_ROAM_CONFIGURATION_FLAGS m_RoamConfigFlags;

    //set to true if the scan cancel was due to a good enough AP being found before scan completed.
    bool                      m_bIsScanCancelIntended;
    WFC_REGISTRATION_TOKEN    m_BssEntryListNotificationRegistrationToken;

    ULONG                   m_TempBSSCandidateCount;
    CBSSEntry*              m_TempBSSCandidateList[WFC_CONSTANT::MaxConnectBssEntryCount];

    // Connection profile parameter
    WFC_CONNECTION_PROFILE_PARAMETERS  m_ConnectionProfile;
    WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA m_NloMatchingCriteria;

    //
    // Generated structures
    //
    BYTE*                   m_pConnectParametersTlvBuffer;
    ULONG                   m_ConnectParametersTlvLength;

    //
    // These two are static structures. Can be re-used over and over again.
    //
    DOT11_ASSOCIATION_START_PARAMETERS       m_AssociationStartParameters;
    DOT11_ASSOCIATION_COMPLETION_PARAMETERS  m_StaticAssociationCompletionParameters;

    Task                    m_ConnectTask;
    ULONG                   m_ConnectAttemptCount;

    WFC_CONNECT_SCAN_TYPE   m_LastScanType;
    CScanJob*               m_pScanJob;
    CBssTransitionResponseJob* m_pResponseJob;

    CBSSListUpdateJob       m_BSSListUpdateJob;

    bool m_IsP2PConnect;
    DOT11_WFD_GROUP_ID m_GroupIdStorage;
    bool m_RequireSelectedRegistrar;
    bool m_RequireGroupFormationBit;
    DOT11_WFD_CHANNEL m_GOChannelHint;
    CP2PDiscoverJob  m_DiscoverJob;
    PVOID            m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    DeviceCommand   m_PrivacyExemptCommand;
    BYTE*           m_pPrivacyCommandDataBuffer;
    ULONG           m_PrivacyCommandBufferLength;

    WFC_TIMER_REGISTRATION_TOKEN m_PMKIDTimerToken;
    WFC_REGISTRATION_TOKEN  m_PMKIDNotificationRegistrationToken;

    bool                    m_MediaStreamingMode;
    bool                    m_PerformFullPassiveScan;

    bool m_IsRoam;
    bool m_IsReconnectNeededDueTo11r;
    bool m_IsBlockSSIDFor11rNeeded;
    bool m_IsCriticalRoamToLEAlreadyAttempted;
    DOT11_SSID m_SSID;
    bool m_IsDeferred;
};

//==============================================================================================

class CDisconnectJob : public COidJobBase
{
public:
    CDisconnectJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CDisconnectJob();

public:
    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled() { }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    // Use to initialize from a parent job
    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter *pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ bool FullDisconnect,
        _In_ WDI_ASSOC_STATUS DisconnectWabiReason,
        _In_ UINT16 Disassociation80211Reason
        );

private:
    WDI_TASK_DISCONNECT_PARAMETERS  m_DisconnectParameters;
    ULONG                           m_DisconnectParamsTlvBufferLength;
    BYTE*                           m_pDisconnectParamsTlvBuffer;
    Task                            m_DisconnectTask;
    PVOID                           m_OidRequestId;
    NDIS_PORT_NUMBER                m_OidPortNumber;
    bool                            m_FullDisconnect;
    DOT11_DISASSOCIATION_PARAMETERS m_NdisDisassocParameters;   // Use when we are called from OID
};


//==============================================================================================

class CRoamReconnectJob : public CJobBase, public IDisassociationCompleteCallback, public INotifyOperationStarted
{
public:
    CRoamReconnectJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CRoamReconnectJob();

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ bool IsP2PClient,
        _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags
        );

    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

    //
    // INotifyOperationStarted Implementation
    //
    void OnOperationStarted(
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ WFC_SENDER_OBJECT SenderObject);

private:
    void FinishJob(
        _In_ NDIS_STATUS OperationStatus);

    VOID NotifyRoamingStart();

    VOID NotifyRoamingCompletionIfNecessary(
        _In_ NDIS_STATUS OperationStatus
        );

    NDIS_STATUS StartConnectJob(
        _In_opt_ WFC_CONNECT_SCAN_TYPE InitialScanType = WfcConnectScanTypeNone);

    NDIS_STATUS CompleteConnectJob(
        _In_ NDIS_STATUS OperationStatus,
        _Out_ bool* WasCancelled,
        _Out_ bool* WasDeferred,
        _Out_ WFC_CONNECT_SCAN_TYPE* LastScanType
        );

    NDIS_STATUS CompleteAndRequeueConnectJobIfNecessary(
        _In_ NDIS_STATUS OperationStatus
        );

    void OnDisassociationProcessed();

protected:
    enum
    {
        RoamStateInit,
        RoamStateWaitingForConnectCompletion,
        RoamStateDone
    } m_RoamState;

    bool m_IsCancelled;
    CAdapter* m_pAdapter;
    WFC_ROAM_CONNECT_TRIGGER m_RoamTrigger;
    WDI_ROAM_CONFIGURATION_FLAGS m_RoamConfigFlags;
    WDI_ASSOC_STATUS m_RoamWabiReason;
    NDIS_PORT_NUMBER m_NdisPortNumber;

    CConnectJob* m_pConnectJob;
    bool m_RoamStartIndicated;
    bool m_IsP2PClient;
    UINT32 m_RoamDeferCount;
};


//==============================================================================================

/*++
Handles association parameters request and will send a response to the IHV
--*/
class CAssociationParametersRequestResponderJob : public CJobBase,
    public INotifyDeviceIndicationCallback,
    public ITimerCallback

{
public:
    CAssociationParametersRequestResponderJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CAssociationParametersRequestResponderJob();

    // Serialized
    NDIS_STATUS InitializeFromAssociationParametersRequest(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ UINT16 WfcPortId,
        _In_ WDI_INDICATION_ASSOCIATION_PARAMS_REQUEST_PARAMETERS* pAssociationParametersRequest
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

    bool IsCancelled()
    {
        return m_IsCancelled;
    }

public:
    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);

    //
    // INotifyDeviceIndicationCallback Implementation
    //
    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                               EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );


private:
    NDIS_STATUS StartWaitForPMKIDList();

    void OnPMKIDListUpdateNotificationArrived(
        _In_ ULONG  BufferLength,
        _In_reads_bytes_(BufferLength) BYTE* Buffer);


    VOID StopWaitForPMKIDList(
        _In_ NDIS_STATUS StopStatus
        );

    VOID SendAssociationParametersResponse(_In_ NDIS_STATUS DataAvailableStatus);

private:
    enum
    {
        AssociationParametersRequestStateInit,
        AssociationParametersRequestStateWaitingForData,
        AssociationParametersRequestStateWaitingForResponseCompletion
    } m_AssociationParametersJobState;

    WFC_TIMER_REGISTRATION_TOKEN m_PMKIDTimerToken;
    WFC_REGISTRATION_TOKEN  m_PMKIDNotificationRegistrationToken;
    bool        m_IsCancelled;
    CAdapter*   m_pAdapter;
    ULONG       m_TempBSSCandidateCount;
    CBSSEntry*  m_TempBSSCandidateList[WFC_CONSTANT::MaxConnectBssEntryCount];    
    WFC_CONNECTION_PROFILE_PARAMETERS m_ConnectionParameters;
    WDI_SET_ASSOCIATION_PARAMS_PARAMETERS m_AssociationParameters;
    
    DeviceCommand m_DeviceCommand;
    ULONG       m_CommandSize;
    BYTE*       m_pCommandBuffer;;
    
};

//==============================================================================================

/*++
Sets and Queries HESSID Connection parameters
    WiFiJobSetDesiredHESSIDList
    WiFiJobQueryDesiredHESSIDList
--*/

class CSetDesiredHESSIDList :
    public CSimpleSetOidJob
{
public:
    CSetDesiredHESSIDList(
        _In_ WFC_ACTIVITY_ID ActivityId)
            : CSimpleSetOidJob(ActivityId)
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

class CQueryDesiredHESSIDList :
    public CSimpleQueryOidJob
{
public:
    CQueryDesiredHESSIDList(
        _In_ WFC_ACTIVITY_ID ActivityId)
            : CSimpleQueryOidJob(ActivityId)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded);
};

//==============================================================================================

/*++
Handles set of OID_DOT11_SET_FT_REASSOCIATION_PARAMETERS
--*/
class CSetFTReassociationParametersJob : public CDeviceCommandOidJobBase
{
public:
    CSetFTReassociationParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
        : CDeviceCommandOidJobBase(ActivityId),
        m_pFTReassociationParameters(NULL)
    {
    }

    ~CSetFTReassociationParametersJob()
    {
        if ( m_pFTReassociationParameters != NULL )
        {
            delete [] m_pFTReassociationParameters;
        }
    }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest );

    void OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

protected:
    //
    // CDeviceCommandOidJobBase Implementation
    //
    NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pMessageID,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer );

    void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus );

    BYTE *m_pFTReassociationParameters;
};

