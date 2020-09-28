
/*++
Copyright (c) Microsoft Corporation

Module Name:

RTPMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBPORT
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _RTPM_EVENT{
    RtpmEventNull                                    = 0x000003e8,
    RtpmEventContinueIfNoSubState                    = 0x000003e9 | EventPrioritySync,
    RtpmEventD0CompleteFailure                       = 0x000007d4 | EventPriorityNormal,
    RtpmEventD0CompleteSuccess                       = 0x000007d8 | EventPriorityNormal,
    RtpmEventDxCompleteFailure                       = 0x000007dc | EventPriorityNormal,
    RtpmEventDxCompleteSuccess                       = 0x000007e0 | EventPriorityNormal,
    RtpmEventOperationFailure                        = 0x000007d4 | EventPrioritySync,
    RtpmEventOperationSuccess                        = 0x000007d8 | EventPrioritySync,
    RtpmEventPowerNotRequired                        = 0x000007d4 | EventPriorityLow,
    RtpmEventPowerRequired                           = 0x000007d8 | EventPriorityLow,
    RtpmEventWaitWakeFailure                         = 0x000007e4 | EventPriorityNormal,
    RtpmEventWaitWakeSuccess                         = 0x000007e8 | EventPriorityNormal,
} RTPM_EVENT, *PRTPM_EVENT;



//
// Definitions for State Machine Operations 
//
RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_AcknowledgePower;

RTPM_OPERATION_SYNC_DISPATCH         USBPORTRTPM_ArmForWakeFromS0;

RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_CompleteHubWWIrp;

RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_DisableTimerWork;

RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_DisarmForWakeFromS0;

RTPM_OPERATION_SYNC_DISPATCH         USBPORTRTPM_EnableInterrupts;

RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_EnableTimerWork;

RTPM_OPERATION_ASYNC_DISPATCH        USBPORTRTPM_IndicateCriticalPowerFailure;

RTPM_OPERATION_SYNC_DISPATCH         USBPORTRTPM_RequestD0;

RTPM_OPERATION_SYNC_DISPATCH         USBPORTRTPM_RequestDx;

