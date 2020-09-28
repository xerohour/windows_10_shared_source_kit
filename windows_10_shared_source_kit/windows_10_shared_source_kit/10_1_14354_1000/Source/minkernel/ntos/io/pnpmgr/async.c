/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    async.c

Abstract:

    This module contains the support for asynchronous PNP IRPs. Currently, we only
    support asynchronous handling of IRP_MN_START_DEVICE and
    IRP_MN_QUERY_DEVICE_RELATIONS-BusRelations.

Author:

    Santosh Jodh (santoshj) 19-Feb-2003

Environment:

    Kernel mode


Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Device completion queue for asynchronous PNP operations.
//
PNP_DEVICE_COMPLETION_QUEUE PnpDeviceCompletionQueue;

//
// PNP supports async start and enumeration across the entire device tree.
//

ULONG PnpAsyncOptions = PNP_ASYNC_START_ENABLED | PNP_ASYNC_ENUM_ENABLED | PNP_ASYNC_TREE_WIDE;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PnpDeviceCompletionRequestCreate)
#pragma alloc_text(PAGE, PnpDeviceCompletionProcessCompletedRequest)
#pragma alloc_text(PAGE, PnpDeviceCompletionProcessCompletedRequests)
#endif // ALLOC_PRAGMA

PPNP_DEVICE_COMPLETION_REQUEST
PnpDeviceCompletionRequestCreate (
    __in PDEVICE_NODE         DeviceNode,
    __in PNP_DEVNODE_STATE    CompletionState,
    __in_opt PVOID            Context
    )

/*++

Description

    This function creates an individual PNP_DEVICE_COMPLETION_REQUEST and initializes it.

Parameters

    DeviceNode - Pointer to the associated devnode for the entry being created.

    CompletionState - Devnode will be set to this state on completion of the operation.

    Context - Pointer to any associated context.

Return

    Pointer to a newly created and initialize PNP_DEVICE_COMPLETION_REQUEST if successful.
    NULL on failure.

--*/

{

    PPNP_DEVICE_COMPLETION_REQUEST request;

    PAGED_CODE();

    request = ExAllocatePoolWithTag(NonPagedPoolNx,
                                    sizeof(*request),
                                    PNP_POOLTAG_DEVICE_COMPLETION_REQUEST);
    if (!request) {
        goto exit;
    }

    //
    // Initialize the completion request.
    //

    InitializeListHead(&request->ListEntry);
    request->ReferenceCount = 1;
    request->IrpPended = FALSE;
    request->Context = Context;
    request->DeviceNode = DeviceNode;
    request->CompletionState = CompletionState;
    request->Status = STATUS_INTERNAL_ERROR;
    request->Information = NULL;

exit:

    return request;
}

VOID
PnpDeviceCompletionRequestDestroy (
    __inout PPNP_DEVICE_COMPLETION_REQUEST Entry
    )

/*++

Description

    This function will free storage for the device completion request.

Parameters

    Entry - Pointer to the device completion request.

Return

    None.

--*/

{

    //
    // This routine must be non-paged because it runs during the device
    // completion routine.
    //

    if (InterlockedDecrement(&Entry->ReferenceCount) == 0) {
        ExFreePoolWithTag(Entry, PNP_POOLTAG_DEVICE_COMPLETION_REQUEST);
    }

    return;
}


NTSTATUS
PnpDeviceCompletionProcessCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_REQUEST CompletedRequest
    )

/*++

Description

    This function performs the post-processing and sets the devnode state to the
    completionState once the PNP IRP completes.

Parameters

    CompletedRequest - Pointer to the complete entry to be processed.

Return

    NTSTATUS.

--*/

{
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    //
    // Extract the IRP status and information.
    //
    deviceNode = CompletedRequest->DeviceNode;
    deviceNode->OverUsed1.Information = CompletedRequest->Information;
    deviceNode->CompletionStatus = CompletedRequest->Status;

    if (CompletedRequest->IrpPended) {

        IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                     "%s IRP was pended for %wZ\n",
                     (CompletedRequest->CompletionState == DeviceNodeEnumerateCompletion)? "Enumeration" : ((CompletedRequest->CompletionState == DeviceNodeRestartCompletion)? "Restart" : "Start"),
                     PnpGetDeviceInstancePath(deviceNode)));
    }

    //
    // Update the devnode state.
    //
    PipSetDevNodeState(deviceNode, CompletedRequest->CompletionState, NULL);

    //
    // Do additional processing for IRP_MN_START completion.
    //

    if (NT_SUCCESS(CompletedRequest->Status) &&
        ((CompletedRequest->CompletionState == DeviceNodeStartCompletion) ||
        (CompletedRequest->CompletionState == DeviceNodeRestartCompletion))) {
     
       PipUpdatePostStartCharacteristics(deviceNode->PhysicalDeviceObject);

       //
       // Clear DEVPKEY_Device_DriverProblemDesc
       //

       _PnpSetObjectProperty(PiPnpRtlCtx,
                             PnpGetDeviceInstancePath(deviceNode)->Buffer,
                             PNP_OBJECT_TYPE_DEVICE,
                             NULL,
                             NULL,
                             &DEVPKEY_Device_DriverProblemDesc,
                             DEVPROP_TYPE_EMPTY,
                             NULL,
                             0,
                             0);

       //
       // Allow software device creation to enumerate
       // software devices children.
       //

        PiSwProcessParentStartIrp(deviceNode->PhysicalDeviceObject);
    }

    //
    // If this is an enumeration completion, undo the previous device
    // activation.
    // 
    
    if (CompletedRequest->CompletionState == DeviceNodeEnumerateCompletion) {

        if ((deviceNode->StateFlags & DNSF_POFX_BUS_RELATIONS) == 0) {
            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                          PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE,
                          (ULONG_PTR) deviceNode,
                          DNSF_POFX_BUS_RELATIONS,
                          0);

        }

        PoFxIdleDevice(deviceNode->PhysicalDeviceObject);
        deviceNode->StateFlags &= ~DNSF_POFX_BUS_RELATIONS;
    }

    //
    // Cleanup the entry.
    //
    PnpDeviceCompletionRequestDestroy(CompletedRequest);

    return deviceNode->CompletionStatus;
}

NTSTATUS
PnpDeviceCompletionProcessCompletedRequests (
    __in LOGICAL                      All,
    __in LOGICAL                      Wait,
    __out LOGICAL                     *Completed
    )

/*++

Description

    This routines processes completed requests as specified.

Parameters

    All - Specifies whether all completed entries are to be removed or any.

    Wait - Specified if its OK to wait for a request to complete.

    Completed - Pointer that recieves result if any completed operation was processed.

Return

    NTSTATUS.

--*/

{
    NTSTATUS status, finalStatus;
    PLIST_ENTRY entry;
    PPNP_DEVICE_COMPLETION_REQUEST completedEntry;

    PAGED_CODE();

    PNP_ASSERT(!(All != FALSE && Wait == FALSE));
    PNP_ASSERT(Completed);

    *Completed = FALSE;
    finalStatus = STATUS_SUCCESS;

    for (;;) {

        if (PnpDeviceCompletionQueueIsEmpty(&PnpDeviceCompletionQueue)) {

            //
            // Nothing in dispatched or completed queue - we are done.
            //
            break;
        }

        if (!PnpDeviceCompletionQueueGetCompletedCount(&PnpDeviceCompletionQueue)) {

            //
            // If we are not told to wait or we are told to get any and we got at least
            // one, then bail.
            //
            if (!Wait || (!All && (*Completed) != FALSE)) {

                break;
            }
        }

        //
        // Get the completed operation (waiting if neccessary).
        //
        entry = PnpDeviceCompletionQueueGetCompletedRequest(&PnpDeviceCompletionQueue);
        completedEntry = CONTAINING_RECORD(entry,
                                           PNP_DEVICE_COMPLETION_REQUEST,
                                           ListEntry);

        //
        // Process the completed entry.
        //
        status = PnpDeviceCompletionProcessCompletedRequest(completedEntry);
        if (status == STATUS_PNP_RESTART_ENUMERATION) {

            finalStatus = status;
        }

        //
        // We found at least one completed operation.
        //
        *Completed = TRUE;
    }

    return finalStatus;
}

