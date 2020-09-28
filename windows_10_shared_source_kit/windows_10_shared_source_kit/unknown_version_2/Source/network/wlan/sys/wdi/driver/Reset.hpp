/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Reset.hpp

Abstract:

    Header file for the Reset work
Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

typedef enum _RESET_PORT_JOB_STATE
{
    ResetPortJobStateNotStarted,
    ResetPortJobStateWaitingForDisconnectCompletion,
    ResetPortJobStateWaitingForActiveJobsCompletion,
    ResetPortJobStateWaitingForDataPathCleanup,
    ResetPortJobStateWaitingForPortINITState
} RESET_PORT_JOB_STATE;

/*++
Handles method request of OID_DOT11_RESET_REQUEST
--*/
class CDot11ResetJob : public COidJobBase
{
public:
    CDot11ResetJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    ~CDot11ResetJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

    // Externally used. Used to directly initialize
    // this from a parent job
    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter *pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_opt_ PDOT11_MAC_ADDRESS pMacAddress);

private:
    NDIS_STATUS StartPortDataPathCleanup();

    NDIS_STATUS CompletePortDataPathCleanup(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartPortActiveJobsCompletion();

    NDIS_STATUS CompletePortActiveJobsCompletion(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartPortDisconnect();

    NDIS_STATUS CompletePortDisconnect(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartPortSwitchToINITState();

    NDIS_STATUS CompletePortSwitchToINITState(
        _In_ NDIS_STATUS OperationStatus);

    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

private:
    DOT11_RESET_REQUEST m_Request;

    CPort* m_pPort;

    CDisconnectJob  m_DisconnectJob;    
    CAPResetJob m_APResetJob;
    Task            m_Dot11ResetTask;

    // Total buffer we would send down for Dot11 Reset
    BYTE*        m_CommandBuffer;
    
    
    RESET_PORT_JOB_STATE m_JobState;

};
