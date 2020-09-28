/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

RandomMacForScan.hpp

Abstract:

Header file for the Random MAC for Scan work

Environment:

Kernel mode

Revision History :

--*/
#pragma once

//==============================================================================================

/*++
Handles method request of OID_DOT11_RANDOM_MAC_FOR_SCAN
--*/

typedef enum _RANDOM_MAC_FOR_SCAN_JOB_STATE
{
    RandomMacForScanNotStarted,
    RandomMacForScanWaitingForResetJob,
    RandomMacForScanWaitingPowerStateJob,
    RandomMacForScanWaitingNLOJob,
    RandomMacForScanCompleted
} RANDOM_MAC_FOR_SCAN_JOB_STATE;

class CDot11RandomMacForScan : public COidJobBase
{
public:
    CDot11RandomMacForScan(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CDot11RandomMacForScan();

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
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    NDIS_STATUS StartResetJob();
    NDIS_STATUS CompleteResetJob(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartPowerParameterRestoreJob();
    NDIS_STATUS CompletePowerParameterRestoreJob(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartNLOParameterResetJob();
    NDIS_STATUS CompleteNLOParameterResetJob(
        _In_ NDIS_STATUS OperationStatus);

    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

private:
    RANDOM_MAC_FOR_SCAN_JOB_STATE           m_JobState;
    CPort*                                  m_pPort;
    DOT11_MAC_ADDRESS                       m_MacAddress;

    CDot11ResetJob                          m_ResetJob; // Reset job will perform the MAC Address reset.

    BOOLEAN                                 m_bOidAutoPowerSave; // Store the value before reset, reset it if not default.
    CUpdateConnectionQualityJob             m_ConnectionQualityJob;

    DeviceCommand                           m_DeviceCommand;

};


