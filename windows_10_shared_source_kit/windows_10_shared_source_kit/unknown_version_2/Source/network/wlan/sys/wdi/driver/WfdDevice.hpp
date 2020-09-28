#pragma once

class CGetNextDialogTokenJob :
    public CSimpleQueryDeviceCommandOidJob
{
public:
    CGetNextDialogTokenJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleQueryDeviceCommandOidJob(ActivityId)
    {
    }

    ~CGetNextDialogTokenJob()
    {
    }

    NDIS_STATUS GetDeviceCommandId(
        _Out_ ULONG* pCommandId);

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG CommandBufferLength,
        _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
        _In_ PVOID pInformationBuffer,
        _In_ UINT InformationBufferLength,
        _Inout_ PUINT pBytesWritten,
        _Inout_ PUINT pBytesNeeded);
};



class CWfdChangeListenStateJob :
    public CJobBase
{
public:
    CWfdChangeListenStateJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CJobBase(ActivityId),
        m_pTlvBuffer(NULL),
        m_DeviceCommand(ActivityId)
    {

    }

    ~CWfdChangeListenStateJob()
    {
        if ( m_pTlvBuffer != NULL )
        {
            delete [] m_pTlvBuffer;
            m_pTlvBuffer = NULL;
        }
    }

public:
    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ WDI_P2P_LISTEN_STATE NewListenState );

public:
    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled() { }

protected:
    WDI_P2P_LISTEN_STATE m_NewListenState;
    DeviceCommand m_DeviceCommand;
    CAdapter* m_pAdapter;

    ULONG m_TlvBufferLength;
    BYTE* m_pTlvBuffer;
};



class CP2PDiscoverJob : public COidJobBase,
                        public INotifyDeviceIndicationCallback,
                        public ITimerCallback

{
protected:
    enum
    {
        DiscoverStateInit,
        DiscoverStateWaitingForToggleListenStateCompletion,
        DiscoverStateWaitingForDiscoverCompletion,
        DiscoverStateWaitingForFallbackDiscoverCompletion,
        DiscoverStateWaitingForRevertListenStateCompletion,
        DiscoverStateDone
    } m_DiscoverState;

    WFC_TIMER_REGISTRATION_TOKEN m_TimerToken;
    WDI_SSID m_DefaultDiscoverSSIDs[2];
    DOT11_WFD_GROUP_ID m_TargetGroupID;

    ULONG m_DiscoverTimeoutInMs;
    ULONG m_RemainingTimeoutInMs;

    Task m_DiscoverTask;

    bool m_IsCancelled : 1;
    bool m_IsInternalCancel : 1;
    bool m_TriggerFallbackDiscover : 1;
    bool m_RequireSelectedRegistrar : 1;
    bool m_RequireGroupFormationBit : 1;
    bool m_ChannelHintAvailable : 1;
    bool m_AllowFallbackDiscover : 1;
    bool m_RevertListenState : 1;
    bool m_AllowBeaconMatch : 1;
    bool m_IsInternalDiscover : 1;

    CBSSListManager* m_pDiscoveryCache;

    DOT11_WFD_DISCOVER_REQUEST* m_pDiscoverRequest;

    ULONG m_DeviceFilterCount;
    DOT11_WFD_DISCOVER_DEVICE_FILTER* m_pDeviceFilterList;

    WDI_BAND_CHANNEL_LIST_CONTAINER m_ChannelHintBandInfo;
    WDI_P2P_DISCOVERY_CHANNEL_SETTINGS_CONTAINER m_DiscoveryChannelSettings;
    WDI_CHANNEL_NUMBER m_ChannelHintChannelNumber;

    WFC_REGISTRATION_TOKEN m_ScanResultRegistration;

    WDI_TASK_P2P_DISCOVER_PARAMETERS m_DiscoverParameters;

    PVOID            m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    ULONG m_DiscoverTlvBufferLength;
    BYTE* m_pDiscoverTlvBuffer;

    ULONG m_IndicationBufferLength;
    BYTE* m_pIndicationBuffer;

    CWfdChangeListenStateJob m_ListenStateJob;

    ULONGLONG m_DiscoverStartTime;
    bool IsDeviceFilterSatisfied();

public:
    CP2PDiscoverJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_DiscoverTask(ActivityId),
        m_IsCancelled(false),
        m_IsInternalCancel(false),
        m_pDiscoveryCache(NULL),
        m_pDiscoverRequest(NULL),
        m_DeviceFilterCount(0),
        m_pDeviceFilterList(NULL),
        m_ScanResultRegistration(WFC_REGISTRATION_TOKEN_INVALID),
        m_TimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
        m_pDiscoverTlvBuffer(NULL),
        m_DiscoverState(DiscoverStateInit),
        m_pIndicationBuffer(NULL),
        m_RequireSelectedRegistrar(false),
        m_RequireGroupFormationBit(false),
        m_AllowFallbackDiscover(false),
        m_RevertListenState(false),
        m_ListenStateJob(ActivityId),
        m_AllowBeaconMatch(false),
        m_IsInternalDiscover(false)
    {
        m_DefaultDiscoverSSIDs[0].pElements = (UINT8*)"DIRECT-";
        m_DefaultDiscoverSSIDs[0].ElementCount = sizeof("DIRECT-") - 1;
        m_DefaultDiscoverSSIDs[1].pElements = (UINT8*)"";
        m_DefaultDiscoverSSIDs[1].ElementCount = 0;
    }

    ~CP2PDiscoverJob()
    {
        if ( m_ScanResultRegistration != WFC_REGISTRATION_TOKEN_INVALID )
        {
            m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
                m_ScanResultRegistration );
            m_ScanResultRegistration = WFC_REGISTRATION_TOKEN_INVALID;
        }

        if ( m_pDiscoverTlvBuffer != NULL )
        {
            delete [] m_pDiscoverTlvBuffer;
            m_pDiscoverTlvBuffer = NULL;
        }

        if ( m_pIndicationBuffer != NULL )
        {
            delete [] m_pIndicationBuffer;
            m_pIndicationBuffer = NULL;
        }

        if ( m_pDeviceFilterList != NULL )
        {
            delete [] m_pDeviceFilterList;
            m_pDeviceFilterList = NULL;
        }

        if ( m_TimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID )
        {
            (VOID)m_pEventQueue->DeregisterTimeoutCallback(
                m_TimerToken );
            m_TimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
        }
    }

public:
    NDIS_STATUS InitializeDiscoverByGroupID(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ const DOT11_WFD_GROUP_ID& groupID,
        _In_ bool SelectedRegistrar,
        _In_ bool GroupFormation,
        _In_ bool AllowBeaconMatch,
        _In_ bool P2PRoamingReconnect,
        _In_opt_ DOT11_WFD_CHANNEL* pChannelHint,
        _In_opt_ CBSSEntry* pBSSEntryHint );

    void GetDiscoverySettingsFromBitmask(
        _In_ UCHAR ucBitmask,
        _Out_ bool *pMatchDeviceAddress,
        _Out_ bool *pMatchSSID,
        _Out_ bool *pMatchAll );

    NDIS_STATUS TriggerDiscover();
    NDIS_STATUS PrepareCompletionIndication();

public:
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest );

    void CP2PDiscoverJob::OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus );

    void OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

public:
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

public:
    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                               EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

public:
    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext );

};


class CP2PSendActionFrameRequestJob : public COidJobBase
{
public:
    CP2PSendActionFrameRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_DiscoverJob(ActivityId),
        m_TaskCommand(ActivityId),
        m_AlreadyDiscovered(false),
        m_pTlvBuffer(NULL),
        m_pSendCompleteIndication(NULL),
        m_IsCancelled(false)
    {

    }

    ~CP2PSendActionFrameRequestJob()
    {
        if ( m_pTlvBuffer != NULL )
        {
            delete [] m_pTlvBuffer;
            m_pTlvBuffer = NULL;
        }

        if ( m_pSendCompleteIndication != NULL )
        {
            delete [] m_pSendCompleteIndication;
            m_pSendCompleteIndication = NULL;
        }
    }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest );

    void OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus );

    void OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

public:
    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

protected:
    NDIS_STATUS PopulateGONegotiationRequestParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS* pRequestParams );

    NDIS_STATUS PopulateInvitationRequestParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) DOT11_SEND_INVITATION_REQUEST_PARAMETERS* pRequestParams );

    NDIS_STATUS PopulateProvisionDiscoveryRequestParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) NDIS_OBJECT_HEADER *pNdisHeader );

    NDIS_STATUS CopyExtraIEsToByteBlob(
        _In_ WDI_BYTE_BLOB* pTarget,
        _In_ ULONG DataLength,
        _In_reads_bytes_(DataLength) BYTE* pExtraIEs );

    NDIS_STATUS TrySendActionRequestFrame();
    NDIS_STATUS PopulateBSSEntry(
        _In_ CBSSEntry* pBSSEntry,
        _Out_ WDI_BSS_ENTRY_CONTAINER* pBSSEntryContainer );
    NDIS_STATUS ConstructCompletionIndication();

    WDI_TASK_P2P_SEND_REQUEST_ACTION_FRAME_PARAMETERS m_RequestParameters;

    //
    // Static local storage for SSID (wherever there is a GroupID),
    // since OID request will disappear before we generate TLVs
    //
    DOT11_SSID m_SSIDStorage;
    DOT11_SSID m_PersistedGroupSSID;

    enum
    {
        P2PSendRequestStateInit,
        P2PSendRequestStateFindingDevice,
        P2PSendRequestStateWaitingForSendCompletion,
        P2PSendRequestStateDone
    } m_SendFrameState;

    bool m_IsCancelled;

    Task m_TaskCommand;
    CP2PDiscoverJob m_DiscoverJob;

    BYTE* m_pTlvBuffer;
    ULONG m_TlvBufferLength;

    PVOID m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    NDIS_STATUS m_SendCompleteIndicationType;
    ULONG m_SendCompleteIndicationLength;
    BYTE* m_pSendCompleteIndication;

    bool m_AlreadyDiscovered;

    union
    {
        DOT11_GO_NEGOTIATION_REQUEST_SEND_COMPLETE_PARAMETERS m_GONRequestSendCompleteParams;
        DOT11_PROVISION_DISCOVERY_REQUEST_SEND_COMPLETE_PARAMETERS m_ProvDiscRequestSendCompleteParams;
        DOT11_INVITATION_REQUEST_SEND_COMPLETE_PARAMETERS m_InvitationRequestSendCompleteParams;
    };
};


class CP2PSendActionFrameResponseJob : public CDeviceCommandOidJobBase
{
public:
    CP2PSendActionFrameResponseJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CDeviceCommandOidJobBase(ActivityId),
        m_pSendCompleteIndication(NULL)
    {

    }

    ~CP2PSendActionFrameResponseJob()
    {
        if ( m_pSendCompleteIndication != NULL )
        {
            delete [] m_pSendCompleteIndication;
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

public:
    //
    // CDeviceCommandOidJobBase implementation
    //
    NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pMessageID,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer );

    NDIS_STATUS HandleCommandCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN CommandToken,
        _In_ NDIS_STATUS OperationStatus,
        _In_ NDIS_STATUS WiFiStatus,
        _In_ ULONG CommandCompletionBufferLength,
        _In_reads_bytes_(CommandCompletionBufferLength) BYTE* pCommandCompletionBuffer,
        _Out_ bool* pQueryNextCommand );

    void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus );

protected:
    NDIS_STATUS PopulateGONegotiationResponseParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_RESPONSE_PARAMETERS* pRequestParams );

    NDIS_STATUS PopulateGONegotiationConfirmationParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS* pRequestParams );

    NDIS_STATUS PopulateInvitationResponseParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) DOT11_SEND_INVITATION_RESPONSE_PARAMETERS* pRequestParams );

    NDIS_STATUS PopulateProvisionDiscoveryResponseParameters(
        _In_ ULONG RequestLength,
        _In_reads_bytes_(RequestLength) NDIS_OBJECT_HEADER* pRequestParams );

    NDIS_STATUS CopyExtraIEsToByteBlob(
        _In_ WDI_BYTE_BLOB* pTarget,
        _In_ ULONG DataLength,
        _In_reads_bytes_(DataLength) BYTE* pExtraIEs );

    WDI_TASK_P2P_SEND_RESPONSE_ACTION_FRAME_PARAMETERS m_ResponseParameters;

    //
    // Static local storage for SSID (wherever there is a GroupID),
    // since OID request will disappear before we generate TLVs
    //
    DOT11_SSID m_SSIDStorage;
    DOT11_SSID m_PersistedGroupSSID;

    PVOID m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    NDIS_STATUS m_SendCompleteIndicationType;
    ULONG m_SendCompleteIndicationLength;
    BYTE* m_pSendCompleteIndication;

    union
    {
        DOT11_GO_NEGOTIATION_RESPONSE_SEND_COMPLETE_PARAMETERS m_GONResponseSendCompleteParams;
        DOT11_GO_NEGOTIATION_CONFIRMATION_SEND_COMPLETE_PARAMETERS m_GONConfirmationSendCompleteParams;
        DOT11_PROVISION_DISCOVERY_RESPONSE_SEND_COMPLETE_PARAMETERS m_ProvDiscResponseSendCompleteParams;
        DOT11_INVITATION_RESPONSE_SEND_COMPLETE_PARAMETERS m_InvitationResponseSendCompleteParams;
    };
};
