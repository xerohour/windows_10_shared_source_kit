#pragma once

typedef enum _WFC_JOB_TYPE
{
    WiFiJobTypeInvalid = 0,             // Invalid
    WiFiJobOIDSetNotYetImplemented, // TODO: Place holder - Remove this
    WiFiJobOIDQueryNotYetImplemented, // TODO: Place holder - Remove this
    WiFiJobMiniportInitialize,
    WiFiJobMiniportHalt,
    WiFiJobMiniportPause,
    WiFiJobMiniportRestart,
    WiFiJobMiniportSurpriseRemoval,
    WiFiJobCreatePort,
    WiFiJobDeletePort,
    WiFiJobQuerySupportedGuids,
    WiFiJobSetPacketFilter,
    WiFiJobQuery8023MulticastList,
    WiFiJobSet8023MulticastList,
    WiFiJobQueryDot11MulticastList,
    WiFiJobSetDot11MulticastList,
    WiFiJobDot11ResetPort,
    WiFiJobQueryOperationMode,
    WiFiJobSetOperationMode,
    WiFiJobTypeConnect,
    WiFiJobSetPower,
    WiFiJobQueryPower,
    WiFiJobSetAutoPowerSave,
    WiFiJobGetAutoPowerSave,
    WiFiJobSetPowerMgmtRequest,
    WiFiJobGetPowerMgmtRequest,
    WiFiJobSetNlo,
    WiFiJobQueryDesiredBssType,
    WiFiJobSetDesiredBssType,
    WiFiJobQueryEnabledAuthAlgo,
    WiFiJobSetEnabledAuthAlgo,
    WiFiJobSetFTParameters,
    WiFiJobQueryEnabledMulticastCipherAlgo,
    WiFiJobSetEnabledMulticastCipherAlgo,
    WiFiJobQueryEnabledUnicastCipherAlgo,
    WiFiJobSetEnabledUnicastCipherAlgo,
    WiFiJobQueryPhyId,
    WiFiJobSetPhyId,
    WiFiJobScan,
    WiFiJobScanOidRequest,
    WiFiJobEnumBSSList,
    WiFiJobQueryDesiredSSIDList,
    WiFiJobSetDesiredSSIDList,
    WiFiJobQueryDesiredHESSIDList,
    WiFiJobSetDesiredHESSIDList,
    WiFiJobQueryHiddenNetworkEnabled,
    WiFiJobSetHiddenNetworkEnabled,
    WiFiJobQueryExcludeUnencrypted,
    WiFiJobSetExcludeUnencrypted,
    WiFiJobConnectRequest,
    WiFiJobQueryOperationalRateSet,
    WiFiJobDisconnectRequest,
    WiFiJobSetPrivacyExemptionList,
    WiFiJobSetDefaultKey,
    WiFiJobSetDefaultKeyId,
    WiFiJobSetKeyMappingKey,
    WiFiJobSetAddWolPattern,
    WiFiJobSetRemoveWolPattern,
    WiFiJobQueryDesiredBSSIDList,
    WiFiJobSetDesiredBSSIDList,
    WiFiJobQueryStaAssociationIEs,
    WiFiJobSetStaAssociationIEs,
    WiFiJobGetPmParameters,
    WiFiJobSetPmParameters,
    WiFiJobSetAddPmProtocolOffload,
    WiFiJobSetRemovePmProtocolOffload,
    WiFiJobGetPmProtocolOffload,
    WiFiJobGetReceiveCoalescingMatchCount,
    WiFiJobSetReceiveCoalescing,
    WiFiJobSetClearReceiveCoalescing,
    WiFiJobFlushBssList,
    WiFiJobEnumAssociationInfo,
    WiFiJobRoam,
    WiFiJobCreateMac,
    WiFiJobDeleteMac,
    WiFiJobQueryNicPowerState,
    WiFiJobSetNicPowerState,
    WiFiJobQueryOperationModeCapability,
    WiFiJobQueryMacAddress,
    WiFiJobQueryHardwareAddress,
    WiFiJobSetOperationalRateSet,
    WiFiJobQueryMediaStreamingEnabled,
    WiFiJobSetMediaStreamingEnabled,
    WiFiJobQueryHardwarePhyState,
    WiFiJobQueryPrivacyExemptionList,
    WiFiJobQueryDefaultKeyId,
    WiFiJobQueryActivePhyList,
    WiFiJobQueryAutoConfigEnabled,
    WiFiJobSetAutoConfigEnabled,
    WiFiJobQueryDesiredPhyList,
    WiFiJobSetDesiredPhyList,
    WiFiJobQueryExcludedMacAddressList,
    WiFiJobSetExcludedMacAddressList,
    WiFiJobQueryPMKIDList,
    WiFiJobSetPMKIDList,
    WiFiJobQuerySupportedMulticastAlgorithmPair,
    WiFiJobQuerySupportedUnicastAlgorithmPair,
    WiFiJobQueryUnicastUseGroupEnabled,
    WiFiJobSetUnicastUseGroupEnable,
    WiFiJobQueryVendorDescription,
    WiFiJobQueryVendorDriverVersion,
    WiFiJobQueryVendorID,
    WiFiJobQueryRTSThreshold,
    WiFiJobSetRTSThreshold,
    WiFiJobQueryFragmentationThreshold,
    WiFiJobSetFragmentationThreshold,
    WiFiJobQueryCurrentRegDomain,
    WiFiJobQueryCurrentChannel,
    WiFiJobSetCurrentChannel,
    WiFiJobQueryCurrentFrequency,
    WiFiJobSetCurrentFrequency,
    WiFiJobQueryRegDomainsSupportValue,
    WiFiJobQueryUnreachableDetectionThreshold,
    WiFiJobSetUnreachableDetectionThreshold,
    WiFiJobQueryIBSSParams,
    WiFiJobSetIBSSParams,
    WiFiJobQuerySafeModeEnabled,
    WiFiJobSetSafeModeEnabled,
    WiFiJobQueryExtSTACapability,
    WiFiJobQueryStatistics,
    WiFiJobOIDQueryBeaconPeriod,
    WiFiJobOIDSetBeaconPeriod,
    WiFiJobSetRadioState,
    WiFiJobGetRadioState,
    WiFiJobP2PSetDeviceCapability,
    WiFiJobP2PSetAdditionalIE,
    WiFiJobP2PConnectRequest,
    WiFiJobP2PSetDesiredGroupID,
    WiFiJobP2PSetDeviceInfo,
    WiFiJobP2PDisconnectPeerRequest,
    WiFiJobP2PDiscover,
    WiFiJobP2PSetGroupJoinParameters,
    WiFiJobP2PSetGroupOwnerCapability,
    WiFiJobP2PSetListenState,
    WiFiJobP2PSetListenStatePassiveAvailability,
    WiFiJobP2PGetAdvertisementList,
    WiFiJobP2PSetAdvertisementList,
    WiFiJobP2PStartBackgroundDiscovery,
    WiFiJobP2PStopBackgroundDiscovery,
    WiFiJobP2PSetSecondaryDeviceTypeList,
    WiFiJobP2PSendActionFrameRequest,
    WiFiJobP2PSetGroupStartParameters,
    WiFiJobP2PSendActionFrameResponse,
    WiFiJobStartP2PAp,
    WiFiJobStartLegacyAp,
    WiFiJobStopAp,
    WiFiJobP2PCancelDiscover,
    WiFiJobSendAssociationResponse,
    WiFiJobSetWpsEnabled,
    WiFiJobP2PEnumDeviceList,
    WiFiJobP2PGetDialogToken,
    WiFiJobP2PFlushDeviceList,
    WiFiJobGetPeerInfo,
    WiFiJobSetQoSParams,
    WiFiJobGetQoSParams,
    WiFiJobResumeFirmware,
    WiFiJobResumeP2pPort,
    WiFiJobNicSpecificExtension,
    WiFiJobUpdateBSSList,
    WiFiJobResetRecovery,
    WiFiJobSendActionFrameRequest,
    WiFiJobANQPQuery,
    WiFiJobQueryTcpOffloadCurrentConfig,
    WiFiJobQueryTcpOffloadHardwareCaps,
    WiFiJobSetTcpOffloadParameters,
    WiFiJobQueryTcpRscStatistics,
    WiFiJobAssociationParametersRequestResponder,
    WiFiJobUpdateConnectionQuality,
    WiFiJobSetEncapsulationOffload,
    WiFiJobRandomMacForScan,
    WiFiJobP2PEnableHrdssDevices,
    WiFiJobGetAdapterEnhancedCapabilities,
    WiFiJobGetAdapterBandCapabilities,
    WiFiJobGeneralStatistics,
    WiFiJobUnrecognizedOidHandler,
    WiFiJobIhvTaskRequest,
    WiFiJobSetFTReassociationParameters,
    WiFiJobSetNeighborReportEntries,
    WiFiJobQueryXmitOk,
    WiFiJobQueryRcvOk,
    WiFiJobSetEndDwellOnChannel
} WFC_JOB_TYPE;

typedef enum _WFC_JOB_STATE
{
    WiFiJobStateInit,
    WiFiJobStateStartOrContinue,
    WiFiJobStateStepPending
} WFC_JOB_STATE;

class ActiveJobsList;
class SerializedJobList;

class CJobBase : public IOperationCompletionCallback,
                 public IEventQueueCallback,
                 public IActivityId
{
public:
    //
    // Job Implementation Functions
    //
    virtual void OnJobStarted() = 0;

    virtual void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject ) = 0;

    virtual void OnJobCancelled() = 0;

    virtual void OnAborted();

public:
    virtual ~CJobBase() { }

    WFC_JOB_TYPE get_JobType();

    //
    // Serialized Job Metadata
    //
    bool get_IsPreemptable();
    bool get_CanPreempt();
    WFC_SERIALIZED_JOB_PRIORITY_SCOPE get_PriorityScope();
    WFC_SERIALIZED_JOB_PRIORITY get_JobPriority();
    NDIS_PORT_NUMBER get_NdisPortNumber();
    bool get_IsDeferrable();
    bool get_DeferHint();
    void set_DeferHint(bool DeferHint);

protected:
    void set_JobPriority(
        WFC_SERIALIZED_JOB_PRIORITY NewPriority);

    void set_IsPreemptible(
        _In_ bool  NewIsPreemptible);

    // true if OnJobStarted was called
    bool get_WasJobStarted();
    
public:
    //
    // Parent APIs
    //
    static NDIS_STATUS s_CreateInstanceFromOIDBuffer(
        _In_ WFC_JOB_TYPE                   JobType,
        _In_ DWORD                           BufferSize,
        _In_reads_bytes_(BufferSize) BYTE*   Buffer,
        _In_ IOperationCompletionCallback*   pCompletionCallback,
        _Out_ CJobBase**                     ppJob );

    NDIS_STATUS StartJob(
        _In_ IOperationCompletionCallback* pCompletionCallback );

    void CancelJob();

    //
    // Event Notifications
    //
    INotifyOperationStarted* EventJobStarted;

public:
    //
    // Parent List Helpers (Active Jobs/Serialized Jobs)
    //
    NDIS_STATUS get_FailsafeCompletionEvent(
        _Out_ Event** ppFailsafeCompletionEvent );

    PLIST_ENTRY get_ActiveJobListEntry();

    PLIST_ENTRY get_SerializedJobListEntry();

    static CJobBase* get_JobFromListEntry(
        _In_ PLIST_ENTRY pListEntry );

    NDIS_STATUS set_ActiveJobCompletionCallback(
        _In_ IOperationCompletionCallback* pCompletionCallback );

    NDIS_STATUS get_ActiveJobCompletionCallback(
        _Out_ IOperationCompletionCallback** ppCompletionCallback );

    NDIS_STATUS set_SerializedJobCompletionCallback(
        _In_ IOperationCompletionCallback* pCompletionCallback );

    NDIS_STATUS get_SerializedJobCompletionCallback(
        _Out_ IOperationCompletionCallback** ppCompletionCallback );

    void set_RequiresPassiveIrql(
        _In_ bool RequiresPassiveIrql );

    bool get_RequiresPassiveIrql();
    
public:
    //
    // IOperationCompletionCallback Implementation
    //
    void OnOperationCompleted(
        _In_ NDIS_STATUS        OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

public:
    //
    // IEventQueueCallback Implementation
    //
    virtual void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE        Type,
        _In_opt_ NDIS_STATUS        Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload );

private:
    //
    // Private Event Queue Callbacks
    //
    void OnStartJobEventHandler();

protected:
    //
    // Job Implementation Helpers
    //
    CJobBase(
        _In_ WFC_ACTIVITY_ID ActivityId );

    WFC_PORT_ID get_PortId();

    NDIS_STATUS Initialize(
        _In_ EventQueue*                        pEventQueue,
        _In_ DeviceCommandScheduler*            pCommandScheduler,
        _In_ ActiveJobsList*                    pActiveJobsList,
        _In_ SerializedJobList*                 pSerializedJobList,    
        _In_ WFC_JOB_TYPE                      JobType,
        _In_ bool                               IsPreemptable,
        _In_ bool                               CanPreempt,
        _In_ WFC_SERIALIZED_JOB_PRIORITY_SCOPE PriorityScope,
        _In_ WFC_SERIALIZED_JOB_PRIORITY       JobPriority,
        _In_ IPropertyCacheDirectory*           pPropertyCacheDirectory,
        _In_opt_ INdisConversion*               pPortConversion = NULL,
        _In_opt_ NDIS_PORT_NUMBER               PortNumber      = INVALID_NDIS_PORT_NUMBER,
        _In_opt_ bool                           IsDeferrable = false );

    NDIS_STATUS StartChildJob(
        _In_ CJobBase* pChildJob );

    NDIS_STATUS QueueSerializedJob(
        _In_ CJobBase* pChildJob );

    NDIS_STATUS StartTask(
        _In_ Task* pTaskCommand );

    NDIS_STATUS InitializeAndStartTask(
        _In_ Task* pTaskCommand,
        _In_ ULONG CommandId,
        _In_ ULONG MessageBufferLength,
        _In_reads_bytes_(MessageBufferLength) BYTE* pMessageBuffer,
        _In_ NotificationManager* pNotificationManager,
        _In_opt_ ULONG* pRelativeTimeoutInMSec = NULL );

    NDIS_STATUS QueueDeviceCommand(
        _In_ DeviceCommand* pDeviceCommand );

    NDIS_STATUS SetPending();

    NDIS_STATUS CancelActivePendingCommand();

    NDIS_STATUS CompleteJob(
        _In_ NDIS_STATUS ErrorStatus = STATUS_SUCCESS );

    void NotifyStarted(
        _In_ NDIS_STATUS OperationStatus );

    // May or may not be present
    NDIS_STATUS get_PortPropertyCache(
        _Out_  CPortPropertyCache** ppPortPropertyCache );

    CAdapterPropertyCache* get_AdapterPropertyCache();

protected:
    EventQueue*                        m_pEventQueue;
    bool                               m_IsPreemptable;
    bool                               m_CanPreempt;
    bool                               m_RequiresPassiveIrql;
    WFC_SERIALIZED_JOB_PRIORITY_SCOPE  m_PriorityScope;
    WFC_SERIALIZED_JOB_PRIORITY        m_JobPriority;
    WFC_PORT_ID                        m_PortId;
    NDIS_PORT_NUMBER                   m_NdisPortNumber;
    WFC_JOB_TYPE                       m_JobType;

private:
    //
    // Job Base Internal State
    //
    WFC_JOB_STATE                      m_State;
    bool                               m_OnJobStartCalled;

    DeviceCommandScheduler*            m_pDeviceCommandScheduler;
    ActiveJobsList*                    m_pActiveJobsList;
    SerializedJobList*                 m_pSerializedJobList;
    
    CJobBase*                          m_pDebugChildJob;
    Task*                              m_pDebugTaskCommand;
    DeviceCommand*                     m_pDebugDeviceCommand;
    bool                               m_KnownCommandPending;

    IOperationCompletionCallback*      m_pJobCompletionCallback;

    Event                              m_FailsafeJobStartedEvent;
    Event                              m_FailsafeJobCompletionEvent;
    Event                              m_FailsafeDeferJobStartEvent;
    Event                              m_FailsafeDeferJobOperationCompletedEvent;

    CPP_LIST_ENTRY                     m_ActiveJobLink;
    IOperationCompletionCallback*      m_pActiveJobCompletionCallback;

    CPP_LIST_ENTRY                     m_SerializedJobLink;
    IOperationCompletionCallback*      m_pSerializedJobCompletionCallback;

    INdisConversion*                   m_pNdisConversion;
    IPropertyCacheDirectory*           m_pPropertyCacheDirectory;
    bool                               m_IsDeferrable;
    bool                               m_DeferHint;
};
