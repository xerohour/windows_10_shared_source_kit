/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

ANQPQuery.hpp

Abstract:

Header file for the ANQP Query handlers

Environment:

Kernel mode

Revision History:

--*/
#pragma once


//==============================================================================================


typedef enum _ANQP_QUERY_JOB_STATE
{
    WiFiANQPQueryJobStateInit,
    WiFiANQPQueryJobStateWaitingForScanJobCompletion,
    WiFiANQPQueryJobStateWaitingForDialogToken,
    WiFiANQPQueryJobStateWaitingForGasResponse,
    WiFiANQPQueryJobStateWaitingToSendComebackRequest,
    WiFiANQPQueryJobStateWaitingForEndDwellTimeToSendComebackRequest,
    WiFiANQPQueryJobStateWaitingForEndDwellTimeToCompleteJob,
    WiFiANQPQueryJobStateGasProtocolComplete
} ANQP_QUERY_JOB_STATE;


typedef struct _ANQP_RESPONSE_FRAGMENT
{
    LIST_ENTRY  Linkage;
    USHORT      uFragmentSize;
    UCHAR       uFragmentID;
    UCHAR       ucFragment[1];
} ANQP_RESPONSE_FRAGMENT, *PANQP_RESPONSE_FRAGMENT;


/*++
Handles ANQP Query request
--*/
class CANQPQueryJob : public COidJobBase,
                      public INotifyDeviceIndicationCallback,
                      public ITimerCallback
{
public:
    CANQPQueryJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CANQPQueryJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    // CJobBase
    VOID OnJobStarted();

    VOID OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    VOID OnJobCancelled();

    //
    // INotifyDeviceIndicationCallback Implementation
    //
    VOID OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                                EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer);

private:

    VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus);

    VOID CleanupAndCompleteJob(
        _In_ NDIS_STATUS JobCompleteStatus);

    CBSSEntry* FindCachedBSSEntry(
        );

    NDIS_STATUS StartANQPQueryTask(
        _In_    CBSSEntry               *pMatchedBssEntry
        );

    DOT11_ANQP_QUERY_RESULT MapStatusCodeToQueryResult(
        _In_    USHORT  uStatusCode
        );

    NDIS_STATUS CacheANQPFragment(
        _In_    UCHAR                           uFragmentID,
        _In_    USHORT                          uFragmentSize,
        _In_reads_bytes_(uFragmentSize) UCHAR   *pucFragment
        );

    NDIS_STATUS HandleFinalANQPResponseReceived(
        );

    VOID OnActionFrameReceived(
        _In_ ULONG  EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer
        );

    NDIS_STATUS BuildAndSendANQPRequestActionFrame(
        _In_    DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE   ActionType,
        _In_    USHORT                                  QueryRequestLength,
        _In_bytecount_(QueryRequestLength) PUCHAR       ucQueryBuffer,
        _In_    BOOLEAN                                 bRequestTask
        );

    BOOLEAN CheckAndMarkPendingJobs(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject
        );

    //
    // ITimerCallback Implementation
    //
    VOID OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext
        );

private:
    WFC_PORT_TYPE                               m_PortType;
    NDIS_STATUS                                 m_JobStatus;

    //
    // Command information
    PVOID                                       m_OidRequestId;
    NDIS_PORT_NUMBER                            m_OidPortNumber;
    DOT11_MAC_ADDRESS                           m_Dot11BSSID;
    PDOT11_ANQP_QUERY_INFO                      m_ANQPQueryInfo;
    ULONG                                       m_OidQueryTimeoutInMs;
    ULONGLONG                                   m_JobExpireTime;
    USHORT                                      m_QueryBufferLength;
    DOT11_ANQP_QUERY_RESULT                     m_QueryResult;
    BOOLEAN                                     m_ActionFramesSupported;

    //
    // State machine parameters
    //
    WFC_TIMER_REGISTRATION_TOKEN                m_OidTimerToken;
    WFC_TIMER_REGISTRATION_TOKEN                m_AwaitingANQPResponseTimerToken;
    ANQP_QUERY_JOB_STATE                        m_State;
    LIST_ENTRY                                  m_ANQPFragmentsList;
    USHORT                                      m_ANQPFragmentsBytesCached;
    USHORT                                      m_uComebackDelay;
    BOOLEAN                                     m_IsCancelled;
    BOOLEAN                                     m_ComebackTimerPending;
    BOOLEAN                                     m_AwaitingANQPResponseTimerPending;

    //
    // Scan to get BandId and Channel (if not in cache)
    //
    CScanJob                                    m_ScanJob;
    WFC_SCAN_JOB_PARAMETERS                     m_ScanJobParameters;
    BOOLEAN                                     m_bScanJobPending;
    CP2PDiscoverJob                             m_DiscoverJob;
    BOOLEAN                                     m_bDiscoverJobPending;
    UINT32                                      m_BandId;
    UINT32                                      m_ChannelNumber;

    //
    // To get Dialog Token
    //
    DeviceCommand                               *m_pGetTokenDeviceCommand;
    WFC_MESSAGE_METADATA                        m_GetTokenBuffer;
    UCHAR                                       m_DialogToken;
    BOOLEAN                                     m_bGetTokenCommandPending;

    //
    // Send the End Dwell Command
    //
    DeviceCommand                               *m_pEndDwellDeviceCommand;
    WFC_MESSAGE_METADATA                        m_SetEndDwellBuffer;

    //
    // Send Action frame
    //
    CSendActionFrameRequestJob                  *m_pSendActionFrameJob;
    WFC_REGISTRATION_TOKEN                      m_ActionFrameReceivedRegistrationToken;
    WFC_TIMER_REGISTRATION_TOKEN                m_ComebackTimerToken;
    BOOLEAN                                     m_bActionFrameTypeIsRequest;

    //
    // Query Results
    //
    PDOT11_ANQP_QUERY_COMPLETE_PARAMETERS       m_pANQPQueryCompleteParams;
    USHORT                                      m_ResponseBufferLength;

};

