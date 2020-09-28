/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Port.hpp

Abstract:

    Header file for the Port functions

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

#define PORT_STARTED            0x00000000
#define PORT_PAUSING            0x00000001
#define PORT_PAUSED             0x00000002
#define PORT_HALTED             0x00000004
#define PORT_IN_RESET_ASYNC     0x00000008
#define PORT_IN_RESET_SYNC      0x00000010
#define PORT_SURPRISE_REMOVED   0x00000020
#define PORT_IN_RESET_2         (PORT_IN_RESET_SYNC | PORT_IN_RESET_ASYNC)

#define SET_PORT_STATUS(_Port, _Flag)    \
    SET_FLAG(_Port->m_State, _Flag)

#define CLEAR_PORT_STATUS(_Port, _Flag)  \
    CLEAR_FLAG(_Port->m_State, _Flag)

#define TEST_PORT_STATUS(_Port, _Flag)   \
    TEST_FLAG(_Port->m_State, _Flag)

typedef struct _ROAM_TRACELOGGING_DATA
{
    bool roamOccured;
    bool isNeighborReportSupported;
    bool isAPChannelReportSupported;
    bool isCancelled;

    UCHAR lastScanType;
    WFC_ROAM_CONNECT_TRIGGER connectTrigger;
    WDI_ASSOC_STATUS roamWabiReason;
    WDI_ASSOC_STATUS associationStatusCode;

    UCHAR linkQualityPreviousConnect;
    UCHAR linkQualityDuringRoam;
    UCHAR linkQualityCurrent;
    ULONG channel;
    ULONG bandID;
    ULONG roamAPRank;

    ULONG assocFailureCount;
    ULONG bssCandidateCount;

    ULONGLONG connectJobStartTime;
    ULONGLONG connectJobEndTime;
    ULONGLONG connectRoamTaskStartTime;
    ULONGLONG connectRoamTaskEndTime;

    ULONGLONG disassociationIndicationTime;
    ULONGLONG setCipherKeysTime;

    ULONGLONG timeToFinishPrivacyExemptionListSetMS;
    ULONGLONG timeToFinishBSSListUpdateMS;
    ULONGLONG timeToFinishScanJobCompletionMS;
    ULONGLONG timeToFinishPMKIDUpdateMS;
}ROAM_TRACELOGGING_DATA, *PROAM_TRACELOGGING_DATA;

#if WDI_DISABLED
 typedef struct _TX_DESCRIPTOR_LIST
 {
     //
     // link to the next and prev descriptors
     //
     LIST_ENTRY TxDListLink;

     //
     // address of this object is sent to LE
     //
     TX_DESCRIPTOR Txd;

 } TX_DESCRIPTOR_LIST, *PTX_DESCRIPTOR_LIST;

 typedef struct _TX_DESCRIPTOR_NB_LIST
 {
     //
     // tx descriptors_list list
     //
     LIST_ENTRY TxDListNBLink;

     //
     // active tx_descriptors list
     //
     LIST_ENTRY TxDListHead;

     //
     // active _TX_DESCRIPTORs for this _TX_DESCRIPTOR_NB_LIST list
     //
     ULONG TxDPending;

     //
     // ndis passed buffer
     //
     PNET_BUFFER_LIST NetBufferList;

 } TX_DESCRIPTOR_NB_LIST, *PTX_DESCRIPTOR_NB_LIST;


const ULONG SEND_DATA_MESSAGE_HEADER_SIZE = WDI_MESSAGE_HEADER_SIZE + WDI_TX_DESCRIPTOR_SIZE;

class CRoamReconnectJob;
class CPort;

#endif // WDI_DISABLED

class CSetNeighborReportEntriesJob : public CJobBase
{
public:
    CSetNeighborReportEntriesJob(
        _In_ WFC_ACTIVITY_ID ActivityId) :
        CJobBase(ActivityId),
        m_pCommandBuffer(NULL),
        m_pAdapter(NULL),
        m_CommandSize(0),
        m_DeviceCommand(ActivityId)
    {
    }

    ~CSetNeighborReportEntriesJob()
    {
        if (m_pCommandBuffer != NULL)
        {
            delete[] m_pCommandBuffer;
            m_pCommandBuffer = NULL;
        }
    }

    // Serialized
    NDIS_STATUS InitializeFromNeighborReportEntries(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ WFC_PORT_ID portId,
        _In_ PWDI_SET_NEIGHBOR_REPORT_ENTRIES_PARAMETERS pNeighborReportEntries
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    DeviceCommand m_DeviceCommand;
    ULONG       m_CommandSize;
    BYTE*       m_pCommandBuffer;
    CAdapter*   m_pAdapter;
};

class CPort : INotifyDeviceIndicationCallback,
              IOperationCompletionCallback,
              INotifyBSSListChangedCallback,
              ITimerCallback
{
public:
    CPort();

    ~CPort();

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ WFC_PORT_TYPE PortType,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ UINT16 PortId,
        _In_reads_bytes_(6) DOT11_MAC_ADDRESS MacAddress);

    NDIS_PORT_NUMBER GetNdisPortNumber()
    {
        return m_NdisPortNumber;
    }

    UINT16 GetPortId()
    {
        return m_WfcPortId;
    }

    void SetPortId( _In_ UINT16 WfcPortId )
    {
        m_WfcPortId = WfcPortId;
    }

    WFC_PORT_TYPE GetPortType()
    {
        return m_WfcPortType;
    }

    CAdapter* GetAdapter()
    {
        return m_pAdapter;
    }

    DOT11_MAC_ADDRESS* GetMacAddress()
    {
        return &m_MacAddress;
    }

    void SetMacAddress(_In_ DOT11_MAC_ADDRESS* pMacAddress);


    DOT11_MAC_ADDRESS* GetMacAddressForScan()
    {
        return &m_MacAddressForScan;
    }

    void SetMacAddressForScan(_In_ DOT11_MAC_ADDRESS* pMacAddress)
    {
        RtlCopyMemory(&m_MacAddressForScan, pMacAddress, sizeof(DOT11_MAC_ADDRESS));
    }

    ULONG GetPnpState()
    {
        return m_State;
    }
    
    // TODO: Move to Property cache
    NDIS_DEVICE_POWER_STATE GetPortPowerState()
    {
        return m_PortPowerState;
    }

    // TODO: Move to Property cache
    void SetPortPowerState( _In_ NDIS_DEVICE_POWER_STATE PowerState )
    {
        m_PortPowerState = PowerState;
    }

    // Used by data path. Leaving this here
    void SetKeyState( _In_ BOOLEAN bKeyConfigured )
    {
        m_KeyConfigured = bKeyConfigured;
    }

    void QueueNeighborReportRequest();
    void OnNeighborReportRequestTimerCallback();

    void QueueOpportunisticRoam();
    void CancelOpportunisticRoam();
    void OnOpportunisticRoamTimerCallback();

    void SignalPMKIDUpdate();

    void SetAPStarted( _In_ bool bStarted )
    {
        m_APStarted = bStarted;
    }

    bool GetAPStarted()
    {
        return m_APStarted;
    }

    VOID SendNetBufferLists(
        _In_  PNET_BUFFER_LIST NetBufferLists,
        _In_  ULONG            SendFlags);

    VOID SendCompleteNetBufferLists(
        _In_ PNET_BUFFER_LIST pNbl);

    NDIS_STATUS SendQueuedNetBufferLists();

    VOID CompleteTxPortQueue();

    BOOLEAN GetBSSTransitionFrameRequestReceived()
    {
        return m_BSSTransitionRequestFrameReceived;
    }

    void SetBSSTransitionFrameRequestReceived(bool bReceived)
    {
        m_BSSTransitionRequestFrameReceived = bReceived;
    }

    UCHAR GetBSSTransitionRequestDialogToken()
    {
        return m_BSSTransitionRequestDialogToken;
    }

#if WDI_DISABLED

    VOID CancelSendNetBufferLists(
        _In_  PVOID  CancelId);

    VOID SendTxDescriptorComplete (
        _In_ PTX_DESCRIPTOR pTxD);

#endif // WDI_DISABLED

    VOID ReturnNBLs(
        _In_ LONG NumNBLs);

    NDIS_STATUS Dot11Reset(
        _In_ IOperationCompletionCallback * dot11ResetCompletion);
    
    NDIS_STATUS Halt();

    enum PauseWaitForRxReturn
    {
        NoWaitForRxReturn = false,
        WaitForRxReturn   = true,
    };

    NDIS_STATUS Pause(
        _In_ IOperationCompletionCallback * completionCallback,
        _In_ PauseWaitForRxReturn           fWaitForRxReturn );

    void SurpriseRemoved();

    NDIS_STATUS Restart();

    NDIS_STATUS WaitForPendingJobCompletion(
        _In_ IOperationCompletionCallback * completionCallback);

    WFC_PORT_DOT11_STATE GetDot11State()
    {
        return m_Dot11State;
    }

    void UpdateLinkState();
    void UpdateLinkQuality(
        _In_ CConnectedPeer *pPeer
        );

    NDIS_STATUS UpdateStoredNLOParameterBuffer();

    void SetDot11State(
        _In_ WFC_PORT_DOT11_STATE Dot11State )
    {
        m_Dot11State = Dot11State;
    }

    IConnectedPeerList* GetConnectedPeerList()
    {
        return m_pPeerList;
    }
    CPortPropertyCache* GetPropertyCache()
    {
        return &m_PortPropertyCache;
    }

    CBSSListManager* GetBSSListManager()
    {
        if (m_WfcPortType == WfcPortTypeWFDRole)
        {
            return m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);        
        }
        else
        {
            return m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);
        }
    }

    // TODO: Move to Property cache
    PNDIS_PM_PARAMETERS GetNdisPmParameters()
    {
        return &m_NdisPmParameters;
    }

    BOOLEAN GetbValidPmParameters()
    {
        return m_bValidPmParameters;
    }

    BOOLEAN SetbValidPmParameters( _In_ BOOLEAN bValid )
    /*++
            Set to the new value; return the old.
        --*/
    {
        BOOLEAN bOld = m_bValidPmParameters;

        m_bValidPmParameters = bValid;
        return bOld;
    }

    void SetCurrentOpMode ( _In_ WDI_OPERATION_MODE opMode)
    {
        m_PortPropertyCache.SetPropertyT (WfcPortCurrentOperationalMode, opMode);        
    }

    NDIS_STATUS CPort::IndicateFrames(
        _In_ PNET_BUFFER_LIST pNblChain,
        _In_ UINT32 NumFrames,
        _In_ ULONG ReceiveFlags );

    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                    CommandToken,
        _In_ WFC_PORT_ID                          PortId,
        _In_ ULONG                                EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnOperationCompleted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject );

    bool DetermineIfSwitchToBetterAPNeeded(
        _In_ CConnectedPeer* pPeer);

    NDIS_STATUS TriggerWdiInitiatedRoam(
        _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags
        );

    NDIS_STATUS TriggerIHVRequestedRoam(
        _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags
        );

    NDIS_STATUS TriggerDisconnectAndReconnect(
        _In_ WDI_ASSOC_STATUS DisconnectWabiReason,
        _In_ UINT16 Disassociation80211ReasonCode
        );

    NDIS_STATUS TriggerReconnect(
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags
        );

    NDIS_STATUS StartRoamJob(
        _Out_ CRoamReconnectJob **ppRoamJob,
        _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
        _In_ WDI_ASSOC_STATUS RoamWabiReason,
        _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags,
        _In_ WDF_EXECUTION_LEVEL ExecutionLevel = WdfExecutionLevelInheritFromParent
        );

    void CPort::IndicateSinglePeerDisassociated(
        _In_ CConnectedPeer* pMacAddress,
        _In_ UINT16 ReasonCode,
        _In_ WDI_ASSOC_STATUS WabiAssocStatus );

    void CPort::IndicateAllPeersDisassociated(
        _In_ UINT16 ReasonCode,
        _In_ WDI_ASSOC_STATUS WabiAssocStatus );

    NDIS_STATUS
    RegisterForDisassociationNotifications(
        _In_opt_ IDisassociationCompleteCallback* pCallback);

    void OnDisassociated(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnTaskOffloadCurrentConfigReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void UpdateTaskOffloadCurrentConfigForFIPS(
        _In_ bool bEnable
        );

    void PopulateTaskOffloadDisabledParameters(
        _Inout_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pTcpOffloadCapabilities
        );

    void OnRoamingNeeded(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnLinkStateChange(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnFTAssocParamsNeededIndicationArrived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer);

    void OnIhvEvent(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnTxAbortConfirm();

    void OnRxFlushConfirm();

    void OnIncomingActionFrameReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnBSSEntryNotification(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );
    
    void OnNeighborReportReceived(
        _In_ ULONG NeighborReportBlobSize,
        _In_reads_bytes_(NeighborReportBlobSize) BYTE* pNeighborReportBlob,
        _In_ bool bIsNeighborReportFromBSSTransitionRequest);

    void OnBssTransitionManagementRequestReceived(
        _In_ ULONG RequestBlobSize,
        _In_reads_bytes_(RequestBlobSize) BYTE* pRequestBlob);

    NDIS_STATUS SendANQPQueryResponse(
        _In_ DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE ActionType,
        _In_ UCHAR DialogToken,
        _In_ UCHAR AdvertisementProtocolID,
        _In_ USHORT StatusCode,                 // For Initial and Comeback Responses
        _In_ DOT11_ANQP_FRAGMENT_ID FragmentID, // For Comeback Response
        _In_ USHORT ComebackDelay,              // For Initial and Comeback Responses
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ UINT32 BandId,
        _In_ UINT32 ChannelNumber,
        _In_ USHORT QueryResponseLength,
        _In_bytecount_(QueryResponseLength) PUCHAR ucQueryBuffer
        );

    void OnPublicActionFrameReceived(
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
        _In_ ULONG PublicActionFrameBlobSize,
        _In_reads_bytes_(PublicActionFrameBlobSize) BYTE* pGASRequestBlob);

    void SendResponseToServiceQueryInitialRequest(
        _In_ CServicesManager *pServicesManager,
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
        _In_ ULONG PublicActionFrameBlobSize,
        _In_reads_bytes_(PublicActionFrameBlobSize) PDOT11_ANQP_ACTION_FRAME pGASRequestFrame);

    void SendResponseToServiceQueryComebackRequest(
        _In_ CServicesManager *pServicesManager,
        _In_ WDI_MAC_ADDRESS *pSourceAddress,
        _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
        _In_ ULONG PublicActionFrameBlobSize,
        _In_reads_bytes_(PublicActionFrameBlobSize) PDOT11_ANQP_ACTION_FRAME pGASRequestFrame);

    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);

    void OnNloDiscovery(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnWakeReason(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnTkipMicFailure(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    NDIS_STATUS SaveNetworkCandidatesHistory(
        _In_ PDOT11_SSID pSsid,
        _In_ WFC_CONNECT_SCAN_TYPE CompletedScanType, 
        _In_ ULONG CandidatesCount,
        _In_reads_(CandidatesCount) CBSSEntry** ppBssCandidatesList
        );

    NDIS_STATUS SaveNetworkConnectHistory(
        _In_ PDOT11_SSID pSsid,
        _In_ CBSSEntry*  pBssEntry
        );

    NDIS_STATUS GetChannelListsFromChannelHistory(
        _In_ PDOT11_SSID pSsid,
        _In_ CChannelListManager *pChannelListManager);


#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
    void ProcessRoamTransition(
        _In_ PWFC_TRANSITION_INFO pTransitionInfo);

    void ProcessAssociationCompletion(
        _In_ PDOT11_SSID pSsid,
        _In_ PDOT11_MAC_ADDRESS pBssid,
        _In_ PWFC_BSS_ENTRY_CHANNEL_INFO pChannelInfo);

    // Allocates channel info lists for last BSSID
    NDIS_STATUS GetChannelListsForNeighborScan(
        _Inout_ ULONG* pChannelList24GHzCount,
        _Out_writes_(*pChannelList24GHzCount) WDI_CHANNEL_NUMBER* pChannelList24GHz,
        _Inout_ ULONG* pChannelList5GHzCount,
        _Out_writes_(*pChannelList5GHzCount) WDI_CHANNEL_NUMBER* pChannelList5GHz);

    // Returns channel info of most likely destination for last BSSID
    NDIS_STATUS GetMostLikelyDestinationChannelInfo(
        _Inout_ PWFC_BSS_ENTRY_CHANNEL_INFO pChannelInfo);
#endif

    void OnIncomingAssociationRequestReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnIncomingP2PActionFrameReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnOperatingChannelIndication(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnOperatingChannelAttributesIndication(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnAssociationParametersRequestReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnStopAPReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnCanSustainAPReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnIhvTaskRequestReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    NDIS_STATUS SendResponseBSSTransitionFrame(
        _In_ WDI_BSS_TRANSITION_RESPONSE_RESULT StatusCode,
        _In_opt_ PDOT11_MAC_ADDRESS TargetBSSId );

    void TraceLoggingRoamFinished(
        _In_ NDIS_STATUS OperationStatus
        );

#define MAX_NLO_REPEAT_WAKE 12

public:
    ULONGLONG m_ullLastNloDiscoverTime;
    LARGE_INTEGER m_liNloStart;
    BYTE * m_pbNloCommand;      // also used as key to valid NLO
    UINT32 m_cbNloCommand;
    LONG    m_FastScanIterations;
    LONG    m_nNloWakes; //Nlo wakes for this non-0 Nlo OID
    static const int c_MaxNloRepeatWakes=12;

    //
    // Background Scan Heuristics
    //
    static const UINT32 c_MaxScanSkipCount = 1;
    UINT32 m_ScanSkipCount;

    //Trace Logging
    PROAM_TRACELOGGING_DATA m_pRoamTraceLoggingData;

private:
    
    CAdapter*               m_pAdapter;
    NDIS_PORT_NUMBER        m_NdisPortNumber;
    UINT16                  m_WfcPortId;
    WFC_PORT_TYPE           m_WfcPortType;
    NDIS_DEVICE_POWER_STATE m_PortPowerState;
    DOT11_MAC_ADDRESS       m_MacAddress;
    DOT11_MAC_ADDRESS       m_MacAddressForScan;
    IOperationCompletionCallback *m_Dot11ResetCallback;
    IOperationCompletionCallback *m_PauseCallback;
    IOperationCompletionCallback *m_JobsCompleteCallback;
    Event m_Dot11ResetCallbackEvent;
    Event m_PauseCallbackEvent;
    Event m_JobsCompleteCallbackEvent;
    LONG m_rxPendingFrames;
    PauseWaitForRxReturn    m_fWaitForRxReturn;
    bool m_APStarted;

    WFC_REGISTRATION_TOKEN m_EventRegistrationToken;
    WFC_TIMER_REGISTRATION_TOKEN m_NeighborRequestTimerToken;
    WFC_TIMER_REGISTRATION_TOKEN m_OpportunisticRoamTimerToken;
    //This is whether the opportunistic roam should run.
    // there are various events that could happen after the callback is already in the 
    // event queue which should cause the roam to be cancelled.  Since the timer might not
    // always be able to be cancelled (if it already fired) this variable is used to prevent
    // the callback from proceeding
    bool m_bOpportunisticRoamRequested;

    ULONG m_PauseRefCount;
    ULONG m_QueuedJobCount;

    WFC_PORT_DOT11_STATE m_Dot11State;

    CRoamReconnectJob* m_pRoamJob;          // Roam job for when we are already connected
    CRoamReconnectJob* m_pReconnectJob;     // Not Connected, Connect        
    CDisconnectJob*    m_pDisconnectJob;    // Disconnect and Then Connect
    CAssociationParametersRequestResponderJob* m_pAssociationParametersRequestResponderJob;

    BOOLEAN m_KeyConfigured;

    ULONGLONG     m_TxDId;
    UINT32        m_State;
    BOOLEAN       m_UpdateStateInProgress;

    UCHAR         m_BSSTransitionRequestDialogToken;
    BOOLEAN       m_BSSTransitionRequestFrameReceived;

    LONG m_txPendingFrames;

    //m_TxPortQueue holds Tx frames which either failed TxTargetDescInit (for resources) or came to WDI while any frames were prequeued.
    //    m_TxPortQueue is protected by the TxMgr PrequeueLock
    NblChain m_TxPortQueue;
    // m_bIsTxPortQueueAllowed is protected by the TxMgr PrequeueLock.  It is false when the port is not allowed to send frames which means that it is also not 
    // allowed to prequeue frames.  It is set to true when sends are resumed.
    bool     m_bIsTxPortQueueAllowed;

    bool m_bIsTxAbortConfirm;
    bool m_bIsRxFlushConfirm;

#if WDI_DISABLED
    FRAGMENT_LIST m_FragmentList[MAX_CONCURRENT_MSDU];
#endif // WDI_DISABLED

    CConnectedPeerList* m_pPeerList;
    WDI_FIPS_MODE m_FIPSMode;

    CNdisSpinLock* m_PortLock;

    CPortPropertyCache m_PortPropertyCache;
    NDIS_PM_PARAMETERS m_NdisPmParameters;
    BOOLEAN m_bValidPmParameters;

    ULONGLONG m_LastPMKIDRequest;
    IDisassociationCompleteCallback* m_pDisassociationCompleteCallback;

#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
    WFC_TRANSITION_CACHE m_TransitionInfoCache;
    DOT11_SSID m_LastSsid;
    DOT11_MAC_ADDRESS m_LastBssid;
    WFC_BSS_ENTRY_CHANNEL_INFO m_LastChannelInfo;
#endif

    // Holds history of connection attempts. Only the most recent attempts are saved
    CNetworkHistoryList*    m_pConnectHistory;

    NDIS_STATUS InitializeTxWfcFrame(
        _In_ PNET_BUFFER_LIST pNbl);

    NDIS_STATUS FreeTxWfcFrame(
        _In_ PNET_BUFFER_LIST pNbl,
        _Out_ PBOOLEAN pUpdatePortStatus);

    NDIS_STATUS AddWfcFrameToPendingCount(
        _In_ PWFC_FRAME pWfcFrame);

#if WDI_DISABLED

    VOID InitializeTXDescriptorList(
        _In_ PTX_DESCRIPTOR_NB_LIST txDescLsit,
        _In_ PNET_BUFFER_LIST netBufferList);

     __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS CreateInitializeTXDescriptor(
        _In_  PNET_BUFFER_LIST    netBufferLists,
        _In_  PNET_BUFFER         netBuffer,    
        _In_  PTX_DESCRIPTOR_NB_LIST txDescLsit,
        _Out_ PTX_DESCRIPTOR     *txDescOut);

    __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS TranslateTxNBLsToTxPackets(
        _In_  PNET_BUFFER_LIST      netBufferLists,
        _Out_ PTX_DESCRIPTOR_NB_LIST *ppTxDescListOut,
        _Out_ PTX_DESCRIPTOR      *ppTxDescOut);

    VOID FreeTranslatedTxDescriptorNBLists(
        _In_ PTX_DESCRIPTOR_NB_LIST TxDescNBList);

    VOID FreeTranslatedTxDList(
        _In_ PLIST_ENTRY pTxDescList);

    VOID FreeTxDescriptorList(
        _In_ PTX_DESCRIPTOR_LIST txDescriptor);

    NDIS_STATUS AddTXDListToSendActiveList(
        _In_ PTX_DESCRIPTOR_NB_LIST txDescLsit);
    
    VOID RemoveTXDListFromSendActiveListAndFree(
        _In_ PTX_DESCRIPTOR_NB_LIST txDescLsit);
    
     __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS PrepareTxWiFiMDLChain(
        _In_ PNET_BUFFER_LIST pNetBufferList,
        _In_ PNET_BUFFER pNetBuffer,
        _Out_ PMDL *ppMdlOut,
        _Out_ PWDI_TX_DESCRIPTOR *ppWiFiTxD);

    NDIS_STATUS AllocateSendDataHeaderMDL(
        _Out_ ULONG *buffferSize,
        _Out_ PVOID *wifiSendMessageHeader,
        _Out_ PMDL *pMdlOut);
 
    VOID FreeWiFiHeaderMDL();

#endif // WDI_DISABLED
    VOID StopSendAndReceivesLE(
        _In_ WFC_RX_STOP_REASON);

    VOID RestartSendAndReceivesLE();

    VOID CompletePendingCancelSendsOrHaltJobs();

    NDIS_STATUS GetPortErrorStatus();

public:
    bool PortCanSendAndReceive() 
    {
        return (m_State == PORT_STARTED);
    }

    bool PortMustPauseRxUntilAssociateComplete()
    {
        // Peers on a station port have their RX paused until we recieve association completion
        // So that NWifi doesn't drop the first few packets of the 1x auth if the data
        // path is faster than the control path.
        return (m_WfcPortType == WfcPortTypeExtSTA);
    }

    void BlockSSID(DOT11_SSID blockedSSID,
        WFC_BLOCKED_REASON reason);

    void UnBlockSSID(DOT11_SSID blockedSSID,
        WFC_BLOCKED_REASON reason);

    bool IsValidSSIDForFT(DOT11_SSID SSID,
        ULONGLONG currentTime);

    NDIS_STATUS ClearFTPropertyCache()
    {
        NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
        ndisStatus = m_PortPropertyCache.SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyPMKR0Name,
            0,
            NULL);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }

        ndisStatus = m_PortPropertyCache.SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyR0KHID,
            0,
            NULL);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }
        ndisStatus = m_PortPropertyCache.SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertySNonce,
            0,
            NULL);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }                     
        ndisStatus = m_PortPropertyCache.SetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT,
            0
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }
    exit:
        return ndisStatus;
    }

    bool PerformPMKIDRequest(
        _In_ ULONG CandidatesCount,
        _In_reads_(CandidatesCount) CBSSEntry** ppBssCandidatesList
        );

private:
#if WDI_DISABLED
    ULONGLONG GetTxDesciptorId()
    {
        return InterlockedIncrement64 ((volatile LONGLONG*)&m_TxDId);
    }
#endif // WDI_DISABLED

    void GetFIPSModeFromPropertyCache()
    {
        BOOLEAN bFIPSMode;
        NDIS_STATUS status;
        
        status = m_pAdapter->GetPortPropertyCache(m_WfcPortId)->GetPropertyBoolean(
            (WFC_PROPERTY_NAME)WfcPortPropertySafeModeEnabled,
            &bFIPSMode);

        ASSERT(NDIS_STATUS_SUCCESS == status);
        
        m_FIPSMode = bFIPSMode? WDI_FIPS_MODE_ENABLED : WDI_FIPS_MODE_DISABLED;
    }

    NDIS_STATUS IncorporateBSSEntryList(
        _In_ ArrayOfElements<WDI_BSS_ENTRY_CONTAINER>* BSSEntryList,
        _Inout_ ULONG* pIncorporatedEntryCount,
        _Out_writes_opt_(*pIncorporatedEntryCount) CBSSEntry** ppIncorporatedBssEntryList
        );

    void OnBSSListChanged(
        _In_ CBSSListManager* pBSSListManager );

#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
    // Looks through the transition entries for a match
    // Returns NULL on no match
    PWFC_TRANSITION_ENTRY FindTransitionEntry(
        _In_ DOT11_MAC_ADDRESS Bssid);

    // Looks through destinations entries for a match
    // Returns NULL on no match
    PWFC_DEST_ENTRY FindDestinationEntry(
        _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
        _In_ DOT11_MAC_ADDRESS Bssid);

    // Looks through the cache for a match or an available entry
    // Returns NULL if there isn't a new entry available/match
    PWFC_TRANSITION_ENTRY GetTransitionEntry(
        _In_ DOT11_MAC_ADDRESS Bssid);

    // Look through the destination entries for a match or an available entry
    // Returns NULL if there isn't a new entry available/match
    PWFC_DEST_ENTRY GetDestinationEntry(
        _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
        _In_ DOT11_MAC_ADDRESS Bssid);

    // Returns number of unique bandIds for the transition entry
    UINT16 GetNumberBandIds(
        _In_ PWFC_TRANSITION_ENTRY pTransitionEntry);

    // Returns number of unique channels for given bandId
    UINT16 GetNumberChannels(
        _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
        _In_ UINT32 BandId);
#endif
};


