
/*++
Copyright (c) Microsoft Corporation

Module Name:

ESMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _ESM_EVENT{
    EsmEventNull                                     = 0x000003e8,
    EsmEventContinueIfNoSubState                     = 0x000003e9 | EventPrioritySync,
    EsmEventCommandCompletionFailure                 = 0x0000000c | EventPriorityCritical,
    EsmEventCommandCompletionSuccess                 = 0x00000010 | EventPriorityCritical,
    EsmEventCommonBufferAllocationFailure            = 0x00000064 | EventPriorityNormal,
    EsmEventControllerResetComplete                  = 0x00000067 | EventPriorityLow,
    EsmEventControllerResetStart                     = 0x0000000c | EventPriorityLow,
    EsmEventControllerSurpriseRemoved                = 0x00000010 | EventPriorityLow,
    EsmEventDisabled                                 = 0x00000018 | EventPriorityLow,
    EsmEventEnabled                                  = 0x0000006f | EventPriorityLow,
    EsmEventExpectedEventTRBsProcessed               = 0x00000094 | EventPriorityNormal,
    EsmEventFSEReceived                              = 0x00000074 | EventPriorityNormal,
    EsmEventHaltedCompletionCodeReceived             = 0x00000098 | EventPriorityNormal,
    EsmEventMappingStopped                           = 0x00000014 | EventPriorityCritical,
    EsmEventNo                                       = 0x00000014 | EventPrioritySync,
    EsmEventOkToCancelTransfers                      = 0x00000018 | EventPriorityCritical,
    EsmEventOperationFailure                         = 0x00000018 | EventPrioritySync,
    EsmEventOperationSuccess                         = 0x0000001c | EventPrioritySync,
    EsmEventResetRequestFromClient                   = 0x00000014 | EventPriorityLow,
    EsmEventStopEndpointFailureWithEndpointHalted    = 0x0000001c | EventPriorityCritical,
    EsmEventStopEndpointFailureWithEndpointRunning   = 0x00000020 | EventPriorityCritical,
    EsmEventStopEndpointFailureWithEndpointStopped   = 0x00000024 | EventPriorityCritical,
    EsmEventStreamsDisabled                          = 0x0000008c | EventPriorityNormal,
    EsmEventStreamsEnabled                           = 0x00000090 | EventPriorityNormal,
    EsmEventTimerFired                               = 0x0000007c | EventPriorityNormal,
    EsmEventTransferCanceled                         = 0x00000080 | EventPriorityNormal,
    EsmEventTransfersReclaimed                       = 0x00000088 | EventPriorityNormal,
    EsmEventYes                                      = 0x00000020 | EventPrioritySync,
} ESM_EVENT, *PESM_EVENT;



//
// Definitions for State Machine Operations 
//
ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_AcknowledgeControllerResetComplete;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_AssertAndSimulateEndpointHaltReceived;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterHalt;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterStop;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_AssertAndSimulateFSEReceived;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_CompleteResetRequestFromClient;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_DropFSE;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_IncrementStopEndpointReissueCount;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_IsDelayInitialEndpointStopCommandFlagSet;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_IsEndpointTypeControl;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_IsIgnoreContextErrorOnEPStopFlagSet;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_IsStopEndpointReissueCountLimitReached;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_IsUseSingleInterrupterFlagSet;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsEndpointHalted;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsEndpointResetFromClient;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsFSEReceived;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsOkToReclaimTransfersOnCancel;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsProcessExpectedEventTRBs;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyTransferRingsReclaimTransfers;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_NotifyUCXCancelTransfers;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_OkToCompleteEndpointResetRequest;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_PurgeTransferRingQueuesIfNeeded;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ReconfigureEndpoint;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ReconfigureEndpointAfterStop;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ReportFSE;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_RequestControllerResetDueToRepeatedStopEndpointFailure;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ResetControlEndpoint;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ResetEndpoint;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_ResetStopEndpointReissueCount;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_RestartTransferRingQueuesIfNeeded;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_SendStopEndpointCommand;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_ShouldReconfigureEndpointAfterStop;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_ShouldReconfigureEndpointOnResetWhenNotHalted;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_ShouldResetEndpointAfterHalt;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_Start200MSTimer;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_Start2SecondTimer;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_Start500MSTimer;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_StartMapping;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_StopMapping;

ESM_OPERATION_ASYNC_DISPATCH        Endpoint_SM_UpdateTransferRingDequeuePointers;

ESM_OPERATION_SYNC_DISPATCH         Endpoint_SM_WasDoorbellRungSinceMappingStart;

