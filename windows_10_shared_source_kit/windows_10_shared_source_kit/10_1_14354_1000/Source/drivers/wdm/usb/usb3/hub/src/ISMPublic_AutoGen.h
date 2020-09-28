
/*++
Copyright (c) Microsoft Corporation

Module Name:

ISMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _ISM_EVENT{
    IsmEventNull                                     = 0x000003e8,
    IsmEventCallbackReturned                         = 0x00001771,
    IsmEventIrpCancelled                             = 0x00001772,
    IsmEventNewRequest                               = 0x00001773,
    IsmEventOperationFailure                         = 0x00001774,
    IsmEventOperationSuccess                         = 0x00001775,
    IsmEventPDOCleanup                               = 0x00001776,
    IsmEventPDOD0                                    = 0x00001777,
    IsmEventPDOD3                                    = 0x00001778,
    IsmEventStart                                    = 0x00001779,
    IsmEventWorkerRoutine                            = 0x0000177a,
} ISM_EVENT, *PISM_EVENT;



//
// Definitions for State Machine Operations 
//
ISM_OPERATION_SYNC_DISPATCH         HUBIDLE_CheckIfSelectiveSuspendIsEnabled;

ISM_OPERATION_SYNC_DISPATCH         HUBIDLE_MarkIrpAsCancellable;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusCancelled;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusCancelledAndReturnPending;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusDeviceBusy;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusInvalidDeviceState;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusNoSuchDevice;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsCompleteIrpWithStatusSuccess;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsInvokeClientCallback;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsQueueWorkItem;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_SetActionAsQueueWorkItem;

ISM_OPERATION_ASYNC_DISPATCH        HUBIDLE_StoreRequest;

ISM_OPERATION_SYNC_DISPATCH         HUBIDLE_UnMarkIrpAsCancellable;

