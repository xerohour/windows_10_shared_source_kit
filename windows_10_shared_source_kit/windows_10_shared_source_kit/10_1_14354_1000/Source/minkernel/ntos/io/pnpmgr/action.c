/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    action.c

Abstract:

    This module implements the PNP device actions.

Author:

    Santosh Jodh (santoshj) 08-09-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Regular data segment
//
#ifdef ALLOC_DATA_PRAGMA

#pragma  data_seg()

#endif // ALLOC_DATA_PRAGMA

//
// PnpEnumerationLock - to synchronize boot phase device enumeration
//
KEVENT PnpEnumerationLock;

BOOLEAN PnpEnumerationInProgress;

WORK_QUEUE_ITEM PnpDeviceEnumerationWorkItem;

volatile PETHREAD PnpDeviceActionThread = NULL;

//
// PnpEnumerationRequestList - a link list of device enumeration requests to worker thread.
//
LIST_ENTRY PnpEnumerationRequestList;

#ifdef ALLOC_PRAGMA

#pragma alloc_text(INIT, PnpInitializeDeviceActions)

//#pragma alloc_text(NONPAGE, PnpLockDeviceActionQueue)
//#pragma alloc_text(NONPAGE, PnpUnlockDeviceActionQueue)
//#pragma alloc_text(NONPAGE, PnpRequestDeviceAction)
//#pragma alloc_text(NONPAGE, PnpDeviceActionWorker)
//#pragma alloc_text(NONPAGE, PnpRemoveDeviceActionRequests)

#pragma alloc_text(PAGE, PnpWaitForEmptyDeviceActionQueue)

#endif // ALLOC_PRAGMA

NTSTATUS
PnpInitializeDeviceActions (
    VOID
    )

/*++

Routine Description:

    This function initializes the data structures related to device actions.

Argumenths:

    None.

Return Value:

    NTSTATUS.

--*/

{
    PAGED_CODE();

    InterlockedExchangePointer(&PnpDeviceActionThread, NULL);
    PnpEnumerationInProgress = FALSE;
    KeInitializeEvent(&PnpEnumerationLock, NotificationEvent, TRUE);
    InitializeListHead(&PnpEnumerationRequestList);
    return STATUS_SUCCESS;
}

VOID
PnpLockDeviceActionQueue (
    VOID
    )

/*++

Description

    This function locks the device action queue so no further actions get
    dispatched.

Parameters

    None.

Return

    None.

--*/

{
    KIRQL oldIrql;

    for (;;) {

        //
        // Lock the device tree so that power operations dont overlap PnP
        // operations like rebalance.
        //

        PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);
        ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
        if (!PnpEnumerationInProgress) {

            //
            // Device action worker queue is empty. Make it so that new requests
            // get queued but new device action worker item does not get kicked
            // off.
            //

            PnpEnumerationInProgress = TRUE;
            KeClearEvent(&PnpEnumerationLock);
            ExReleaseSpinLock(&PnpSpinLock, oldIrql);
            break;
        }

        ExReleaseSpinLock(&PnpSpinLock, oldIrql);

        //
        // Unlock the tree so device action worker can finish current processing.
        //

        PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

        //
        // Wait for current device action worker item to complete.
        //

        KeWaitForSingleObject(&PnpEnumerationLock,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }

    return;
}

VOID
PnpUnlockDeviceActionQueue (
    VOID
    )

/*++

Description

    This function unlocks a previously locked device action queue.

Parameters

    None.

Return

    None.

--*/

{
    KIRQL oldIrql;

    //
    // Check if we need to kick off the enumeration worker here.
    //

    ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
    if (!IsListEmpty(&PnpEnumerationRequestList)) {
        ExInitializeWorkItem(&PnpDeviceEnumerationWorkItem,
                             PnpDeviceActionWorker,
                             NULL);

        ExQueueWorkItem(&PnpDeviceEnumerationWorkItem, DelayedWorkQueue);

    } else {
        PnpEnumerationInProgress = FALSE;
        KeSetEvent(&PnpEnumerationLock, 0, FALSE);
    }

    ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
    return;
}

NTSTATUS
PnpRequestDeviceAction (
    __in_opt    PDEVICE_OBJECT DeviceObject,
    __in        PNP_DEVICE_ACTION_REQUEST RequestType,
    __in        BOOLEAN ReorderingBarrier,
    __in        ULONG_PTR RequestArgument,
    __in_opt    PKEVENT CompletionEvent,
    __out_opt   PNTSTATUS CompletionStatus
    )

/*++

Description

    This function is used to request a new device action.

Parameters

    DeviceObject - Pointer to the device object for which the action is being requested.

    RequestType - Specified the type of device action.

    ReorderingBarrier - If TRUE, device action entries after this cannot be dispatched
        before this one.

    RequestArgument - Optional pointer to the request specific data.

    CompletionEvent - Pointer to the optional event to be signalled on completion of the
        device action.

    CompletionStatus - Pointer that recieves the final status from the device action.

Return

    NTSTATUS.

--*/

{
    GUID activityId;
    LPGUID guidPtr;
    KIRQL oldIrql;
    BOOLEAN propagateActivityId;
    PPNP_DEVICE_ACTION_ENTRY request;
    NTSTATUS status;

    activityId = NullGuid;
    status = STATUS_SUCCESS;

    if (PnpIsShutDown()) {
        status = STATUS_TOO_LATE;
        goto exit;
    }

    //
    // Create the device action request and initialize it.
    //

    request = ExAllocatePoolWithTag(NonPagedPoolNx,
                                    sizeof(PNP_DEVICE_ACTION_ENTRY),
                                    PNP_POOLTAG_DEVICE_ACTION_REQUEST);

    if (!request) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Put this request onto the pending list
    //

    if (DeviceObject == NULL) {
        DeviceObject = IopRootDeviceNode->PhysicalDeviceObject;
    }

    ObReferenceObject(DeviceObject);
    request->DeviceObject = DeviceObject;
    request->RequestType = RequestType;
    request->ReorderingBarrier = ReorderingBarrier;
    request->RequestArgument = RequestArgument;
    request->CompletionEvent = CompletionEvent;
    request->CompletionStatus = CompletionStatus;

    //
    // Check if the thread's activity id should be propagated for this action.
    //

    switch(RequestType) {

        //
        // These requests can be collapsed, causing activity Id information to
        // be lost, so never propagate activity Id for them.
        //

    case ReenumerateRootDevices:
    case ReenumerateDeviceTree:
    case RestartEnumeration:
    case Rebalance:
        propagateActivityId = FALSE;
        break;

        //
        // These requests are internal, so there is no point in propagating
        // activity id for them.
        //
    case StartSystemDevicesPass0:
    case StartSystemDevicesPass1:
    case AssignResources:
        propagateActivityId = FALSE;
        break;

        //
        // For all other requests, propagate activity Id.
        //

    default:
        propagateActivityId = TRUE;
        break;
    }

    if (propagateActivityId) {

        //
        // If the kernel thread has an ID, we get that ID.  Otherwise if this is
        // a user mode request we try and get the ID from the TEB.
        //
        // Note this doesn't apply when DPCs are running as in that case the
        // caller is running in an arbitrary context.
        //

        if (!KeIsExecutingDpc()) {
            guidPtr = NULL;

            NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(
                                     EVENT_ACTIVITY_CTRL_GET_ID,
                                     &guidPtr)));

            if (guidPtr != NULL) {

                activityId = *guidPtr;

            } else if (PnpIsSafeToExamineUserModeTeb()) {

                try {

                    if (NtCurrentTeb() != NULL) {
                        RtlCopyMemory(&activityId,
                                      &NtCurrentTeb()->ActivityId,
                                      sizeof(GUID));
                    }

                } except (EXCEPTION_EXECUTE_HANDLER) {
                }
            }
        }
    }

    RtlCopyMemory(&request->ActivityId, &activityId, sizeof(GUID));

    PnpLogActionQueueEvent(PP_DO_TO_DN(DeviceObject),
                           RequestType,
                           PNP_ACTION_QUEUE);

    //
    // Insert the  request to the request queue.  If the request queue is
    // not currently being worked on, request a worker thread to start it.
    //

    ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
    InsertTailList(&PnpEnumerationRequestList, &request->ListEntry);
    if ((RequestType == ReenumerateBootDevices) ||
        (RequestType == ReenumerateRootDevices)) {

        PNP_ASSERT(!PnpEnumerationInProgress);

        //
        // This is a special request used when booting the system.  Instead
        // of queuing a work item it synchronously calls the worker routine.
        //

        PnpEnumerationInProgress = TRUE;
        KeClearEvent(&PnpEnumerationLock);
        ExReleaseSpinLock(&PnpSpinLock, oldIrql);
        PnpDeviceActionWorker(NULL);

    } else if (PnPBootDriversLoaded && !PnpEnumerationInProgress) {
        PnpEnumerationInProgress = TRUE;
        KeClearEvent(&PnpEnumerationLock);
        ExReleaseSpinLock(&PnpSpinLock, oldIrql);

        //
        // Queue a work item to do the actual work.
        //

        ExInitializeWorkItem(&PnpDeviceEnumerationWorkItem,
                             PnpDeviceActionWorker,
                             NULL);

        ExQueueWorkItem(&PnpDeviceEnumerationWorkItem, DelayedWorkQueue);

    } else {

        //
        // Device action work item is already running or we are booting up.
        //

        ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    }

exit:
    return status;
}

VOID
PnpDeviceActionWorker (
    __in_opt PVOID Context
    )

/*++

Description

    This function is the work item used to process device actions which include things
    like assigning resources, starting devices etc.

Parameters

    Context - Not used.

Return

    None.

--*/

{
    ADD_CONTEXT addContext;
    BOOLEAN assignResources;
    BOOLEAN bootProcess;
    BOOLEAN clearActivityId;
    PPNP_DEVICE_ACTION_ENTRY collapsedRequest;
    BOOLEAN dereferenceDevice;
    PLIST_ENTRY entry;
    ENUM_TYPE enumType;
    LPGUID guidPtr;
    GUID localActivityId;
    LOGICAL moreCollapsedRequests;
    KIRQL oldIrql;
    PPNP_DEVICE_ACTION_ENTRY request;
    NTSTATUS status;
    BOOLEAN completeSystemStart;
    TARGET_DEVICE_CUSTOM_NOTIFICATION notifyInfo;

    UNREFERENCED_PARAMETER(Context);

    InterlockedExchangePointer(&PnpDeviceActionThread, PsGetCurrentThread());
    assignResources = FALSE;
    bootProcess = FALSE;
    completeSystemStart = FALSE;
    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);
    for ( ; ; ) {
        clearActivityId = FALSE;
        status = STATUS_SUCCESS;

        //
        // PipProcessDevNodeTree always dereferences passed in device. Set this
        // to false if PipProcessDevNodeTree is called with the device in the
        // original request.
        //

        dereferenceDevice = TRUE;
        ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
        if (IsListEmpty(&PnpEnumerationRequestList)) {
            if ((assignResources == FALSE) &&
                (bootProcess == FALSE) &&
                (completeSystemStart == FALSE)) {

                //
                // No more processing.
                //

                break;
            }

            entry = NULL;

        } else {
            entry = RemoveHeadList(&PnpEnumerationRequestList);
        }

        ExReleaseSpinLock(&PnpSpinLock, oldIrql);
        if (entry == NULL) {
            PNP_ASSERT(assignResources || bootProcess || completeSystemStart);

            if (assignResources || bootProcess) {
                addContext.DriverStartType = SERVICE_DEMAND_START;
                addContext.LoadDriver = PnPBootDriversInitialized;
                ObReferenceObject(IopRootDeviceNode->PhysicalDeviceObject);

                if (bootProcess) {
                    enumType = EnumTypeNone;

                } else {
                    enumType = EnumTypeIgnoreUninstalledDevices;
                }

                status = PipProcessDevNodeTree(
                    IopRootDeviceNode,
                    NULL,
                    &addContext,
                    enumType,
                    assignResources,    // ReallocateResources
                    FALSE,              // Synchronous
                    FALSE);             // ProcessOnlyIntermediateStates

                if (!NT_SUCCESS(status)) {
                    status = STATUS_SUCCESS;
                }

                assignResources = FALSE;
                bootProcess = FALSE;

            } else {
                PnpCompleteSystemStartProcess();
                completeSystemStart = FALSE;
            }

            continue;
        }

        //
        // We have a list of requests to process. Processing depends on the type
        // of the first one in the list.
        //

        PNP_ASSERT(entry != NULL);
        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);

        //
        // If a non-empty activity ID exists in the action request, associate it
        // with the thread for the duration of the processing.
        //

        if (!IsEqualGUID(&NullGuid, &request->ActivityId)) {

            guidPtr = &localActivityId;

            PNP_ASSERT(PsGetCurrentThread()->ActivityId == NULL);

            localActivityId = request->ActivityId;
            NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(
                                     EVENT_ACTIVITY_CTRL_SET_ID,
                                     &guidPtr)));

            clearActivityId = TRUE;
        }

        //
        // Collapsed actions will be linked into the list in this request.
        //

        InitializeListHead(&request->ListEntry);
        if (PnpIsShutDown()) {
            status = STATUS_TOO_LATE;

        } else if (PP_DO_TO_DN(request->DeviceObject)->State == DeviceNodeDeleted) {
            status = STATUS_UNSUCCESSFUL;

        } else {
            switch (request->RequestType) {
            case AssignResources:

                //
                // Resources were freed, we want to try to satisfy any
                // DNF_INSUFFICIENT_RESOURCES devices.
                //

                assignResources = TRUE;
                break;

            case ClearProblem:
                status = PpProcessClearProblem(request);
                break;

            case ClearDeviceProblem:
            case ClearEjectProblem:
                status = PiProcessClearDeviceProblem(request);
                break;

            case HaltDevice:
                status = PiProcessHaltDevice(request);
                break;

            case QueryPowerRelations:
                status = PiQueryPowerRelations(PP_DO_TO_DN(request->DeviceObject),
                                               FALSE);

                break;

            case Rebalance:
                status = PnpProcessRebalance(request);
                break;

            case RequeryDeviceState:
                status = PiProcessRequeryDeviceState(request);
                break;

            case ResetDevice:
                status = PiRestartDevice(request);
                break;

            case ResourceRequirementsChanged:

                status = PiProcessResourceRequirementsChanged(request);
                if (!NT_SUCCESS(status)) {

                    //
                    // The device wasn't started when
                    // IopResourceRequirementsChanged was called.
                    //

                    assignResources = TRUE;
                    status = STATUS_SUCCESS;
                }

                break;

            case ReenumerateBootDevices:

                //
                // This is during boot driver initialization phase.
                //

                bootProcess = TRUE;
                break;

            case RestartEnumeration:    // Used after completion of async I/O
            case ReenumerateRootDevices:
            case ReenumerateDeviceTree:

                //
                // FALL THROUGH...
                //

            case ReenumerateDeviceOnly:
                status = PiProcessReenumeration(request);
                dereferenceDevice = FALSE;
                break;

            case SetDeviceProblem:
                status = PiProcessSetDeviceProblem(request);
                break;

            case StartDevice:
                status = PiRestartDevice(request);
                break;

            case StartSystemDevicesPass0:
                status = PiProcessStartSystemDevices(request);
                dereferenceDevice = FALSE;
                break;

            case StartSystemDevicesPass1:
                completeSystemStart = TRUE;
                break;

            case NotifyTransportRelationsChange:

                //
                // Notify the change notification system that Transport relations have changed.
                //

                RtlZeroMemory(&notifyInfo, sizeof(notifyInfo));
                notifyInfo.Version = 1;
                notifyInfo.Size = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION,
                                                       CustomDataBuffer);

                notifyInfo.Event = GUID_TARGET_DEVICE_TRANSPORT_RELATIONS_CHANGED;
                notifyInfo.FileObject = NULL;
                notifyInfo.NameBufferOffset = -1;
                IoReportTargetDeviceChangeAsynchronous(request->DeviceObject,
                                                       &notifyInfo,
                                                       NULL,
                                                       NULL);

                //
                // Raise PnpRtl events for the updated NtPlugPlay property.
                //

                PiPnpRtlPdoRaiseNtPlugPlayPropertyChangeEvent(request->DeviceObject,
                                                              NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_TRANSPORT_RELATIONS);
                break;

            case NotifyEjectionRelationsChange:

                //
                // Raise PnpRtl events for the updated NtPlugPlay property.
                //

                PiPnpRtlPdoRaiseNtPlugPlayPropertyChangeEvent(request->DeviceObject,
                                                              NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_EJECT_RELATIONS);
                break;

            case ConfigureDevice:
            case ConfigureDeviceClass:
            case ConfigureDeviceExtensions:
            case ConfigureDeviceReset:
                status = PiConfigureDevice(request);
                break;

            default:
                PNP_ASSERT(FALSE);
                status = STATUS_UNSUCCESSFUL;
                break;
            }
        }

        //
        // Free the list.
        //

        moreCollapsedRequests = TRUE;
        do {
            entry = RemoveHeadList(&request->ListEntry);
            collapsedRequest = CONTAINING_RECORD(entry,
                                                 PNP_DEVICE_ACTION_ENTRY,
                                                 ListEntry);

            //
            // Done with this enumeration request
            //

            if (collapsedRequest->CompletionStatus) {
                *collapsedRequest->CompletionStatus = status;
            }

            if (collapsedRequest->CompletionEvent) {
                KeSetEvent(collapsedRequest->CompletionEvent, 0, FALSE);
            }

            //
            // Only dereference the original request, the rest get dereferenced
            // when we collapse.
            //

            if (collapsedRequest == request) {
                moreCollapsedRequests = FALSE;
                if (dereferenceDevice) {
                    ObDereferenceObject(collapsedRequest->DeviceObject);
                }
            }

            ExFreePoolWithTag(collapsedRequest, PNP_POOLTAG_DEVICE_ACTION_REQUEST);

        } while (moreCollapsedRequests != FALSE);

        if (clearActivityId) {

            guidPtr = NULL;

            PNP_ASSERT(PsGetCurrentThread()->ActivityId != NULL);

            NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(
                                     EVENT_ACTIVITY_CTRL_SET_ID,
                                     &guidPtr)));
        }
    }

    PnpEnumerationInProgress = FALSE;
    KeSetEvent(&PnpEnumerationLock, 0, FALSE);
    ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    InterlockedExchangePointer(&PnpDeviceActionThread, NULL);
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
    return;
}

VOID
PnpRemoveDeviceActionRequests (
    __in PDEVICE_OBJECT DeviceObject
    )
{
    PLIST_ENTRY entry;
    PLIST_ENTRY next;
    KIRQL oldIrql;
    PPNP_DEVICE_ACTION_ENTRY request;

    ExAcquireSpinLock(&PnpSpinLock, &oldIrql);

    //
    // Walk the list and build the list of collapsed requests.
    //

    for (entry = PnpEnumerationRequestList.Flink;
         entry != &PnpEnumerationRequestList;
         entry = next) {

        next = entry->Flink;
        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);
        if (request->DeviceObject == DeviceObject) {
            RemoveEntryList(entry);
            if (request->CompletionStatus) {
                *request->CompletionStatus = STATUS_NO_SUCH_DEVICE;
            }

            if (request->CompletionEvent) {
                KeSetEvent(request->CompletionEvent, 0, FALSE);
            }

            ObDereferenceObject(request->DeviceObject);
            ExFreePoolWithTag(request, PNP_POOLTAG_DEVICE_ACTION_REQUEST);
        }
    }

    ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    return;
}

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
PnpWaitForEmptyDeviceActionQueue (
    VOID
    )

/*++

Routine Description:

    This routine will wait until all currently queued device actions complete.

Arguments:

    None.

Return Value:

    NTSTATUS.

--*/

{
    PAGED_CODE();

    return KeWaitForSingleObject(&PnpEnumerationLock,
                                 Executive,
                                 KernelMode,
                                 FALSE,
                                 NULL);
}

VOID
PnpLogActionQueueEvent(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEVICE_ACTION_REQUEST RequestType,
    __in PNP_ACTION_QUEUE_OP Op
    )
{
    //
    // If enabled, log an ETW event for this request.
    //

    switch (RequestType) {

    case ReenumerateDeviceOnly:
        if ((Op == PNP_ACTION_QUEUE) && (EventEnabledKMPnPEvt_ReenumerateDeviceOnly_Queue())) {
            EventWriteKMPnPEvt_ReenumerateDeviceOnly_Queue(NULL,
                                                           DeviceNode ?
                                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                               NULL);
        } else if ((Op == PNP_ACTION_START) && (EventEnabledKMPnPEvt_ReenumerateDeviceOnly_Start())) {
            EventWriteKMPnPEvt_ReenumerateDeviceOnly_Start(NULL,
                                                           DeviceNode ?
                                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                               NULL);
        } else if ((Op == PNP_ACTION_STOP) && (EventEnabledKMPnPEvt_ReenumerateDeviceOnly_Stop())) {
            EventWriteKMPnPEvt_ReenumerateDeviceOnly_Stop(NULL,
                                                          DeviceNode ?
                                                              PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                              NULL);
        }

        break;

    case ReenumerateRootDevices:
    case ReenumerateDeviceTree:
    case RestartEnumeration:
        if ((Op == PNP_ACTION_QUEUE) && (EventEnabledKMPnPEvt_ReenumerateDeviceTree_Queue())) {
            EventWriteKMPnPEvt_ReenumerateDeviceTree_Queue(NULL,
                                                           DeviceNode ?
                                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                               NULL,
                                                           RequestType);
        } else if ((Op == PNP_ACTION_START) && (EventEnabledKMPnPEvt_ReenumerateDeviceTree_Start())) {
            EventWriteKMPnPEvt_ReenumerateDeviceTree_Start(NULL,
                                                           DeviceNode ?
                                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                               NULL,
                                                           RequestType);
        } else if ((Op == PNP_ACTION_STOP) && (EventEnabledKMPnPEvt_ReenumerateDeviceTree_Stop())) {
            EventWriteKMPnPEvt_ReenumerateDeviceTree_Stop(NULL,
                                                          DeviceNode ?
                                                              PnpGetDeviceInstancePath(DeviceNode)->Buffer :
                                                              NULL,
                                                          RequestType);
        }

        break;

        //
        // For all other requests, don't log anything.
        //

    default:
        break;
    }
}
