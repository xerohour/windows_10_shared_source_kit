/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pnpevent.c

Abstract:

    Routines dealing with Plug and Play event management/notification.

Author:

    Lonny McMichael (lonnym) 02/14/95
    Paula Tomlinson (paulat) 07/01/96

--*/

#include "pnpmgrp.h"
#pragma hdrstop
#include <pnpmgr.h>
#include <pnpsetup.h>

/*
 * Design notes:
 *
 *     When UmPnpMgr needs to initiate an action (which it might do to complete
 * a CmXxx API), it calls NtPlugPlayControl. NtPlugPlayControl then usually
 * invokes one of the PnpSetXxx functions. Similarly, if Io routines need to
 * initiate such an action (say due a hardware initiated eject), they call one
 * of the following PnpSetXxx functions (or an intermediate):
 *
 * Operations synchronized via the queue
 *     PnpSetDeviceClassChange           (async)
 *     PnpSetTargetDeviceRemove          (optional event)
 *     PnpSetCustomTargetEvent           (optional event)
 *     PnpSetHwProfileChangeEvent        (optional event)
 *     PnpSetPlugPlayEvent               (async)
 *     PnpSetDeviceRemovalSafe           (optional event)
 *     PnpSetBlockedDriverEvent          (async)
 *     PnpSynchronizeDeviceEventQueue    (sync, enqueues a noop to flush queue)
 *
 * The PnpSetXxx functions enqueue items to be processed into the Pnp event
 * queue (via PnpInsertEventInQueue). Whenever one of these events are inserted
 * into the queue a worker routine is ensured to be available to process it
 * (PnpDeviceEventWorker).
 *
 * In general, events processed in PnpDeviceEventWorker fall into two categories -
 * those that are notifications for user mode (queued by kernel mode), and those
 * that are queued operations.
 *
 */

#pragma alloc_text(INIT, PnpInitializeDeviceEvents)
#pragma alloc_text(PAGE, PnpAddVetoInformation)
#pragma alloc_text(PAGE, PnpAllocateCriticalMemory)
#pragma alloc_text(PAGE, PnpBuildUnsafeRemovalDeviceList)
#pragma alloc_text(PAGE, PnpCollectOpenHandles)
#pragma alloc_text(PAGE, PnpCollectOpenHandlesCallBack)
#pragma alloc_text(PAGE, PnpCompareGuid)
#pragma alloc_text(PAGE, PnpCompleteDeviceEvent)
#pragma alloc_text(PAGE, PnpCreateDeviceEventEntry)
#pragma alloc_text(PAGE, PnpDeviceEventWorker)
#pragma alloc_text(PAGE, PnpDumpDeviceEvent)
#pragma alloc_text(PAGE, PnpFinalizeVetoedRemove)
#pragma alloc_text(PAGE, PnpFreeVetoInformation)
#pragma alloc_text(PAGE, PnpInitializeTargetDeviceRemoveEvent)
#pragma alloc_text(PAGE, PnpInsertEventInQueue)
#pragma alloc_text(PAGE, PnpLogVetoInformation)
#pragma alloc_text(PAGE, PnpLookupGuid)
#pragma alloc_text(PAGE, PnpNotifyUserModeDeviceRemoval)
#pragma alloc_text(PAGE, PnpProcessCustomDeviceEvent)
#pragma alloc_text(PAGE, PnpProcessQueryRemoveAndEject)
#pragma alloc_text(PAGE, PnpProcessTargetDeviceEvent)
#pragma alloc_text(PAGE, PnpRemoveEventFromQueue)
#pragma alloc_text(PAGE, PnpResizeTargetDeviceBlock)
#pragma alloc_text(PAGE, PnpCompileDeviceInstancePaths)
#pragma alloc_text(PAGE, PnpSetBlockedDriverEvent)
#pragma alloc_text(PAGE, PnpSetCustomTargetEvent)
#pragma alloc_text(PAGE, PnpSetDeviceClassChange)
#pragma alloc_text(PAGE, PnpSetDeviceRemovalSafe)
#pragma alloc_text(PAGE, PnpSetHwProfileChangeEvent)
#pragma alloc_text(PAGE, PnpSetInvalidIDEvent)
#pragma alloc_text(PAGE, PnpSetPowerVetoEvent)
#pragma alloc_text(PAGE, PnpSetPlugPlayEvent)
#pragma alloc_text(PAGE, PnpSetTargetDeviceRemove)
#pragma alloc_text(PAGE, PnpSynchronizeDeviceEventQueue)
#pragma alloc_text(PAGE, PnpWaitForEmptyDeviceEventQueue)

//
// Global Data
//

#pragma  data_seg("PAGEDATA")

PPNP_DEVICE_EVENT_LIST  PnpDeviceEventList = NULL;
BOOLEAN                 PnpNotificationInProgress = FALSE;

#pragma  data_seg()

volatile PETHREAD       PnpDeviceEventThread = NULL;
KGUARDED_MUTEX          PnpNotificationInProgressLock;

//
// PnpEventQueueEmpty - Manual reset event which is set when the queue is empty
//

KEVENT PnpEventQueueEmpty;

NTSTATUS
PnpInitializeDeviceEvents (
    VOID
    )

/*++

Routine Description:

    This routine performs initialization required before any of the notification
    events can be processed.  This routine performs init for the master device
    event queue processing.

Parameters:

    None

Return Value:

    Returns a STATUS_Xxx value that indicates whether the function succeeded
    or not.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;

    //
    // Allocate and initialize the master device event list.
    //

    PnpDeviceEventList = ExAllocatePoolWithTag(NonPagedPoolNx,
                                               sizeof(PNP_DEVICE_EVENT_LIST),
                                               PNP_POOLTAG_DEVICE_EVENT_LIST);

    if (PnpDeviceEventList == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean0;
    }

    KeInitializeMutex(&PnpDeviceEventList->EventQueueMutex, 0);
    KeInitializeGuardedMutex(&PnpDeviceEventList->Lock);
    InitializeListHead(&(PnpDeviceEventList->List));
    PnpDeviceEventList->Status = STATUS_PENDING;

    KeInitializeGuardedMutex(&PnpNotificationInProgressLock);
    KeInitializeEvent(&PnpEventQueueEmpty, NotificationEvent, TRUE);

Clean0:
    return status;

}

VOID
PnpAddVetoInformation (
    __in PDEVICE_OBJECT DeviceObject,
    __in PLIST_ENTRY VetoProcessListHead,
    __in ULONG Pid
    )
/*++

Routine Description:

    This routine inserts both the pid of the vetoing process as well
    as a reference to the device object that the process has a handle
    open against.

Arguments:

    DeviceObject - Supplies a pointer to the device against which the process
        has a handle.

    VetoProcessListHead - Supplies a pointer to the head of the list.

    Pid - Supplies the pid to be inserted into the list.

Return Value:

    None

--*/
{
    PPNP_VETO_DEVICE_OBJECT_INFO currentDevObjNode;
    PPNP_VETO_PROCESS_INFO currentVetoProcessNode;
    PPNP_VETO_DEVICE_OBJECT_INFO newDevObjEntry;
    PPNP_VETO_PROCESS_INFO newVetoProcessEntry;
    BOOLEAN newVetoProcessNodeNeeded;
    PLIST_ENTRY nextDevObjLink;
    PLIST_ENTRY nextVetoProcessLink;

    PAGED_CODE();
    PNP_ASSERT(VetoProcessListHead != NULL);
    PNP_ASSERT(DeviceObject != NULL);
    currentDevObjNode = NULL;
    currentVetoProcessNode = NULL;
    newDevObjEntry = NULL;
    newVetoProcessEntry = NULL;
    nextDevObjLink = NULL;
    newVetoProcessNodeNeeded = TRUE;

    //
    // Make sure that this device object is indeed the Pdo.
    //

    PNP_ASSERT(IS_PDO(DeviceObject) != FALSE);

    //
    // Walk the list and find the right place to insert the new veto process.
    // If the pid already exists, then find the insertion point for the
    // device object node.  The insertion point for the veto process is
    // initialized as the end of the list.
    //

    nextVetoProcessLink = VetoProcessListHead;
    PNP_FOR_EACH_IN_LIST(PNP_VETO_PROCESS_INFO,
                        ListEntry,
                        VetoProcessListHead,
                        currentVetoProcessNode) {

        if (currentVetoProcessNode->Pid == Pid) {

            //
            // An existing node for this process exists.  Search if this
            // specific device object has already been logged.
            //

            nextDevObjLink = &currentVetoProcessNode->DevObjListHead;
            newVetoProcessNodeNeeded = FALSE;
            PNP_FOR_EACH_IN_LIST(PNP_VETO_DEVICE_OBJECT_INFO,
                                 ListEntry,
                                 &currentVetoProcessNode->DevObjListHead,
                                 currentDevObjNode) {

                if (currentDevObjNode->DeviceObject == DeviceObject) {
                    goto cleanup;

                } else if (currentDevObjNode->DeviceObject > DeviceObject) {

                    //
                    // Insert before this node.
                    //

                    nextDevObjLink = &currentDevObjNode->ListEntry;
                    break;
                }
            }

        } else if (currentVetoProcessNode->Pid > Pid) {

            //
            // Insert before this node.
            //

            nextVetoProcessLink = &currentVetoProcessNode->ListEntry;
            break;
        }
    }

    //
    // Allocate a new node for the process if required and insert it
    // into the veto process list.
    //

    if (newVetoProcessNodeNeeded != FALSE) {
        newVetoProcessEntry = ExAllocatePoolWithTag(PagedPool,
                                                    sizeof(PNP_VETO_PROCESS_INFO),
                                                    PNP_POOLTAG_VETO_PROCESS);

        if (newVetoProcessEntry == NULL) {
            goto cleanup;
        }

        newVetoProcessEntry->Pid = Pid;
        PNP_ASSERT(nextVetoProcessLink != NULL);
        InsertTailList(nextVetoProcessLink, &newVetoProcessEntry->ListEntry);
        InitializeListHead(&newVetoProcessEntry->DevObjListHead);
        nextDevObjLink = &newVetoProcessEntry->DevObjListHead;
    }

    //
    // Allocate a new node for the device object
    //

    newDevObjEntry = ExAllocatePoolWithTag(PagedPool,
                                           sizeof(PNP_VETO_DEVICE_OBJECT_INFO),
                                           PNP_POOLTAG_VETO_DEVICE_OBJECT);

    if (newDevObjEntry == NULL) {
        goto cleanup;
    }

    //
    // Now add the device object to the correct process node
    //

    ObReferenceObject(DeviceObject);
    newDevObjEntry->DeviceObject = DeviceObject;
    PNP_ASSERT(nextDevObjLink != NULL);
    InsertTailList(nextDevObjLink, &newDevObjEntry->ListEntry);
    return;

cleanup:
    if (newVetoProcessEntry != NULL) {
        ExFreePoolWithTag(newVetoProcessEntry, PNP_POOLTAG_VETO_PROCESS);
    }

    if (newDevObjEntry != NULL) {
        ExFreePoolWithTag(newDevObjEntry, PNP_POOLTAG_VETO_DEVICE_OBJECT);
    }

    return;
}

NTSTATUS
PnpInsertEventInQueue(
    __in PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    )
{
    GUID activityId;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    LPGUID guidPtr;
    NTSTATUS status;
    PWORK_QUEUE_ITEM workItem;

    PAGED_CODE();

    activityId = NullGuid;
    guidPtr = NULL;
    status = STATUS_SUCCESS;
    workItem = NULL;

#if DBG

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpInsertEventInQueue: Event queued\n"));
    PnpDumpDeviceEvent(&DeviceEvent->Data);

#endif

    //
    // If the kernel thread has an ID, we get that ID.  Otherwise if this is a
    // user mode request we try and get the ID from the TEB.  Note that this
    // needs to be done outside the guarded mutex, since that raises the IRQL to
    // APC_LEVEL.
    //

    NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(EVENT_ACTIVITY_CTRL_GET_ID,
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

    RtlCopyMemory(&DeviceEvent->ActivityId, &activityId, sizeof(GUID));

    if ((EventEnabledKMPnPEvt_DeviceRemoval_Queue()) &&
        ((PnpCompareGuid(&DeviceEvent->Data.EventGuid,
                         &GUID_DEVICE_QUERY_AND_REMOVE)) ||
         (PnpCompareGuid(&DeviceEvent->Data.EventGuid,
                         &GUID_DEVICE_EJECT)))) {

        deviceObject = (PDEVICE_OBJECT)DeviceEvent->Data.DeviceObject;
        deviceNode = PP_DO_TO_DN(deviceObject);
        EventWriteKMPnPEvt_DeviceRemoval_Queue(NULL,
                                               PnpGetDeviceInstancePath(deviceNode)->Buffer);
    }

    //
    // Check if a new work item needs to be kicked off. A new work item gets
    // kicked off iff this is the first event in the list.
    //

    KeAcquireGuardedMutex(&PnpDeviceEventList->Lock);
    KeAcquireGuardedMutex(&PnpNotificationInProgressLock);
    if (PnpNotificationInProgress == FALSE) {
        workItem = ExAllocatePoolWithTag(NonPagedPoolNx,
                                         sizeof(WORK_QUEUE_ITEM),
                                         PNP_POOLTAG_DEVICE_EVENT_WORK_ITEM);

        if (workItem != NULL) {
            PnpNotificationInProgress = TRUE;
            KeClearEvent(&PnpEventQueueEmpty);

        } else {
            IopDbgPrint((IOP_IOEVENT_ERROR_LEVEL,
                         "PnpInsertEventInQueue: Could not allocate memory to kick off a worker thread\n"));

            status = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpInsertEventInQueue: Worker thread already running\n"));
    }

    //
    // Insert the event iff successfull so far.
    //

    InsertTailList(&PnpDeviceEventList->List, &DeviceEvent->ListEntry);
    KeReleaseGuardedMutex(&PnpNotificationInProgressLock);
    KeReleaseGuardedMutex(&PnpDeviceEventList->Lock);

    //
    // Queue the work item if any.
    //

    if (workItem != NULL) {
        ExInitializeWorkItem(workItem, PnpDeviceEventWorker, workItem);
        ExQueueWorkItem(workItem, DelayedWorkQueue);
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpInsertEventInQueue: Kicked off worker thread\n"));
    }

    return status;
}

LOGICAL
PnpRemoveEventFromQueue (
    __inout PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    )
/*++

Routine Description:

    This routine attempts to remove an event that is queued in the device
    event list.  The caller is responsible for ensuring the event stays
    valid for the lifetime of this call.

Arguments:

    DeviceEvent - Event for which removalis attempted.

Return Value:

    TRUE if the entry was successfully removed, FALSE otherwise.

--*/
{
    LOGICAL result = FALSE;

    PAGED_CODE();

    //
    // If the event is still queued, remove it.
    //

    KeAcquireGuardedMutex(&PnpDeviceEventList->Lock);

    if (!IsListEmpty(&DeviceEvent->ListEntry)) {

        RemoveEntryList(&DeviceEvent->ListEntry);
        result = TRUE;
        PnpDeleteDeviceEventEntry(DeviceEvent);
    }

    KeReleaseGuardedMutex(&PnpDeviceEventList->Lock);

    return result;
}

NTSTATUS
PnpSetDeviceClassChange(
    __in CONST GUID *EventGuid,
    __in CONST GUID *ClassGuid,
    __in PUNICODE_STRING SymbolicLinkName
    )

/*++

Routine Description:

    This routine is called by user-mode pnp manager and drivers (indirectly) to
    submit device interface change events into a serialized asynchronous queue.
    This queue is processed by a work item.

Arguments:

    EventGuid - Indicates what event is triggered has occured.

    ClassGuid - Indicates the class of the device interface that changed.

    SymbolicLinkName - Specifies the symbolic link name associated with the
                interface device.


Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG totalSize;

#if DBG

    CHAR classGuidString[80];
    CHAR eventGuidString[80];

#endif

    PAGED_CODE();

    PNP_ASSERT(EventGuid != NULL);
    PNP_ASSERT(ClassGuid != NULL);
    PNP_ASSERT(SymbolicLinkName != NULL);

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

#if DBG

    {
        PnpLookupGuid(EventGuid, eventGuidString, sizeof(eventGuidString));
        PnpLookupGuid(ClassGuid, classGuidString, sizeof(classGuidString));
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                     "PnpSetDeviceClassChange: Entered\n    EventGuid = %s\n    ClassGuid = %s\n    SymbolicLinkName = %wZ\n",
                     eventGuidString,
                     classGuidString,
                     SymbolicLinkName));
    }

#endif

    status = STATUS_SUCCESS;
    try {

        //
        // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
        // we record in the TotalSize field later (the length doesn't count the
        // terminating null but we're already counting the first index into the
        // SymbolicLinkName field so it works out.
        //

        dataSize = sizeof(PLUGPLAY_EVENT_BLOCK) + SymbolicLinkName->Length;
        totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
        deviceEvent = PnpCreateDeviceEventEntry(totalSize);
        if (deviceEvent == NULL) {
            status = STATUS_NO_MEMORY;
            goto Clean0;
        }

        RtlCopyMemory(&deviceEvent->Data.EventGuid, EventGuid, sizeof(GUID));
        deviceEvent->Data.EventCategory = DeviceClassChangeEvent;
        deviceEvent->Data.TotalSize = dataSize;
        RtlCopyMemory(&deviceEvent->Data.u.DeviceClass.ClassGuid, ClassGuid, sizeof(GUID));
        RtlCopyMemory(&deviceEvent->Data.u.DeviceClass.SymbolicLinkName,
                      SymbolicLinkName->Buffer,
                      SymbolicLinkName->Length);
        deviceEvent->Data.u.DeviceClass.SymbolicLinkName[SymbolicLinkName->Length/sizeof(WCHAR)] = 0x0;
        status = PnpInsertEventInQueue(deviceEvent);

Clean0:
        NOTHING;

    } except(PiControlExceptionFilter(GetExceptionInformation())) {
        IopDbgPrint((IOP_IOEVENT_ERROR_LEVEL,
                     "PnpSetDeviceClassChange: Exception 0x%08X\n",
                     GetExceptionCode()));
    }

    return status;
}

NTSTATUS
PnpSetCustomTargetEvent(
    __in  PDEVICE_OBJECT DeviceObject,
    __in_opt PKEVENT SyncEvent,
    __out_opt PULONG Result,
    __in_opt PDEVICE_CHANGE_COMPLETE_CALLBACK Callback,
    __in_opt PVOID Context,
    __in  PTARGET_DEVICE_CUSTOM_NOTIFICATION NotificationStructure
    )

/*++

Routine Description:

    This routine is called by user-mode pnp manager and drivers (indirectly) to
    submit target device change events into a serialized asynchronous queue.
    This queue is processed by a work item.

Arguments:

    DeviceObject - Indicates the device object for the device that changed.

    SyncEvent - Optionally, specifies a kernel-mode event that will be set when the
            event is finished processing.

    Result - Supplies a pointer to a ULONG that will be filled in with the status
            after the event has actual completed (notification finished and the
            event processed). This value is not used when SyncEvent is NULL and
            is REQUIRED when SyncEvent is supplied.

    NotificationStructure - Specifies the custom Notification to be processed.

Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    PNP_ASSERT(NotificationStructure != NULL);
    PNP_ASSERT(DeviceObject != NULL);

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                 "PnpSetCustomTargetEvent: DeviceObject = 0x%p, SyncEvent = 0x%p, Result = 0x%p\n",
                 DeviceObject,
                 SyncEvent,
                 Result));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                 "    Callback = 0x%p, Context = 0x%p, NotificationStructure = 0x%p\n",
                 Callback,
                 Context,
                 NotificationStructure));

    if (SyncEvent != NULL) {
        PNP_ASSERT(Result != NULL);
        *Result = STATUS_PENDING;
    }

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    status = STATUS_SUCCESS;

    //
    // Reference the device object so it can't go away until after we're
    // done with notification.
    //

    deviceNode = PP_DO_TO_DN(DeviceObject);

    //
    // This is a custom event block, so build up the PLUGPLAY_EVENT_BLOCK
    // but copy the Notification Structure and put that in the EventBlock
    // so we can dig it out in the handler later
    //

    dataSize = PnpGetDeviceInstancePath(deviceNode)->Length +
                sizeof(UNICODE_NULL) + sizeof(PLUGPLAY_EVENT_BLOCK);

    //
    // We need to ensure that the Notification structure remains aligned
    // so round up dataSize to a multiple of sizeof(PVOID).
    //

    dataSize += sizeof(PVOID) - 1;
    dataSize &= ~(sizeof(PVOID) - 1);
    dataSize += NotificationStructure->Size;
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ObReferenceObject(DeviceObject);
    deviceEvent->CallerEvent = SyncEvent;
    deviceEvent->Callback = Callback;
    deviceEvent->Context = Context;
    deviceEvent->Data.EventGuid = GUID_PNP_CUSTOM_NOTIFICATION;
    deviceEvent->Data.EventCategory = CustomDeviceEvent;
    deviceEvent->Data.Result = Result;
    deviceEvent->Data.Flags = 0;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = (PVOID)DeviceObject;
    if (PnpIsDeviceInstancePathEmpty(deviceNode) == FALSE) {
        RtlCopyMemory((PVOID)deviceEvent->Data.u.CustomNotification.DeviceId,
                      PnpGetDeviceInstancePath(deviceNode)->Buffer,
                      PnpGetDeviceInstancePath(deviceNode)->Length);

        //
        // No need to NULL terminate this string since we initially zeroed the
        // buffer after allocation.
        //
    }

    //
    // Point the custom notification block to the extra space at the
    // end of the allocation
    //

    deviceEvent->Data.u.CustomNotification.NotificationStructure =
         (PVOID)((PUCHAR)deviceEvent + totalSize - NotificationStructure->Size);

    RtlCopyMemory(deviceEvent->Data.u.CustomNotification.NotificationStructure,
                  NotificationStructure,
                  NotificationStructure->Size);

    status = PnpInsertEventInQueue(deviceEvent);
    return status;

}

NTSTATUS
PnpSetTargetDeviceRemove (
    __in        PDEVICE_OBJECT DeviceObject,
    __in        LOGICAL KernelInitiated,
    __in        LOGICAL NoRestart,
    __in        LOGICAL DoEject,
    __in        LOGICAL NotifyVeto,
    __in        ULONG Problem,
    __in        NTSTATUS ProblemStatus,
    __in_opt    PKEVENT SyncEvent,
    __in_opt    PDEVICE_CHANGE_COMPLETE_CALLBACK Callback,
    __in_opt    PVOID Context,
    __out_opt   PULONG Result,
    __out_opt   PPNP_VETO_TYPE VetoType,
    __out_opt   PUNICODE_STRING VetoName,
    __out_opt   PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    )

/*++

Routine Description:

    This routine is called by user-mode pnp manager and drivers (indirectly) to
    submit target device change events into a serialized asynchronous queue.
    This queue is processed by a work item.

Arguments:

    DeviceObject - Supplies a pointer to the device being removed.

    KernelInitiated - TRUE if removal initiated by kernel, FALSE otherwise.

    NoRestart - TRUE if device should be restarted after removal,
        FALSE otherwise.

    DoEject - TRUE if removal is for ejection, FALSE otherwise.

    NotifyVeto - TRUE if the caller wants the OS to display
        veto UI, FALSE otherwise.

    Problem - Supplies the problem code to be set on the device
        after removal.

    ProblemStatus - Supplies the problem status to be set on the device
        after removal.

    SyncEvent - Optionally, specifies a kernel-mode event that will
        be set when the event is finished processing.

    Callback - Supplies the callback routine which will be called when the
               event is finished.

    Context - Supplies a general context parameter to the callback

    Result - Supplies a pointer to a ULONG that will be filled in with the status
            after the event has actual completed (notification finished and the
            event processed). This value is not used when SyncEvent is NULL and
            is REQUIRED when SyncEvent is supplied.

    VetoType - Supplies a pointer that recieves the veto type.

    VetoName - Supplies a pointer that recieves the veto name.

Return Value:

    NTSTATUS.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    ULONG totalSize;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);

    if (SyncEvent != NULL) {
        PNP_ASSERT(Result != NULL);
    }

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpSetTargetDeviceRemove: Entered\n"));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "    DeviceObject = 0x%p, NoRestart = %d, Problem = %d, ProblemStatus = 0x%08X\n",
               DeviceObject,
               NoRestart,
               Problem,
               ProblemStatus));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "    SyncEvent = 0x%p, Result = 0x%p\n",
               SyncEvent,
               Result));

    if (Result != NULL) {
        *Result = STATUS_PENDING;
    }

    if (PnpIsShutDown() &&
        (InterlockedCompareExchangePointer(&PnpDeviceActionThread, NULL, NULL) ==
         NULL)) {

        //
        // System is shutting down.
        //

        return STATUS_TOO_LATE;
    }

    deviceNode = PP_DO_TO_DN(DeviceObject);

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field so it works out. Add one more for double-null term.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    dataSize += PnpGetDeviceInstancePath(deviceNode)->Length + sizeof(WCHAR);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;

    //
    // Treat kernel initiated removals to be critical for the purpose of
    // memory allocation.
    // This is to avoid the PNP_ERR_DUPLICATE_PDO bugcheck because the state
    // of the device that dissappeared could not be cleaned up as a result of
    // memory allocation failure.
    //

    deviceEvent = PnpAllocateCriticalMemory(
                    KernelInitiated ? SurpriseRemoveDevice : QueryRemoveDevice,
                    PagedPool,
                    totalSize,
                    PNP_POOLTAG_DEVICE_EVENT_ENTRY);

    PNP_ASSERT(deviceEvent != NULL);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(deviceEvent, totalSize);

    //
    // Initialize and queue the event.
    //

    PnpInitializeTargetDeviceRemoveEvent(deviceEvent,
                                         totalSize,
                                         DeviceObject,
                                         KernelInitiated,
                                         NoRestart,
                                         DoEject,
                                         NotifyVeto,
                                         Problem,
                                         ProblemStatus,
                                         SyncEvent,
                                         Result,
                                         VetoType,
                                         VetoName);

    deviceEvent->Callback = Callback;
    deviceEvent->Context = Context;
    if (ARGUMENT_PRESENT(DeviceEvent)) {
        *DeviceEvent = deviceEvent;
        deviceEvent->RefCount++;
    }

    return PnpInsertEventInQueue(deviceEvent);
}

VOID
PnpInitializeTargetDeviceRemoveEvent (
    __out_bcount(DeviceEventSize) PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    __in ULONG DeviceEventSize,
    __in PDEVICE_OBJECT DeviceObject,
    __in LOGICAL KernelInitiated,
    __in LOGICAL NoRestart,
    __in LOGICAL DoEject,
    __in LOGICAL NotifyVeto,
    __in ULONG Problem,
    __in NTSTATUS ProblemStatus,
    __in_opt PKEVENT SyncEvent,
    __out_opt PULONG Result,
    __out_opt PPNP_VETO_TYPE VetoType,
    __out_opt PUNICODE_STRING VetoName
    )

/*++

Routine Description:

    This routine initializes the target device event for a removal operation.

Arguments:

    DeviceEvent - Supplies a pointer to the device event to be initialized.

    DeviceEventSize - Supplies size of the device event structure in bytes.

    DeviceObject - Supplies a pointer to the device being removed.

    KernelInitiated - TRUE if removal initiated by kernel, FALSE otherwise.

    NoRestart - TRUE if device should be restarted after removal,
        FALSE otherwise.

    DoEject - TRUE if removal is for ejection, FALSE otherwise.

    NotifyVeto - TRUE if the caller wants the OS to display
        veto UI, FALSE otherwise.

    Problem - Supplies the problem code to be set on the device
        after removal.

    ProblemStatus - Supplies the problem status to be set on the device
        after removal.

    SyncEvent - Optionally, specifies a kernel-mode event that will
        be set when the event is finished processing.

    Result - Supplies a pointer to a ULONG that will be filled in with the status
            after the event has actual completed (notification finished and the
            event processed). This value is not used when SyncEvent is NULL and
            is REQUIRED when SyncEvent is supplied.

    VetoType - Supplies a pointer that recieves the veto type.

    VetoName - Supplies a pointer that recieves the veto name.

Return Value:

    None.

--*/

{
    PDEVICE_NODE deviceNode;
    PPLUGPLAY_EVENT_BLOCK eventData;
    ULONG instancePathLength;

    PAGED_CODE();

    PNP_ASSERT(DeviceEvent != NULL);
    PNP_ASSERT(DeviceEventSize >= sizeof(PNP_DEVICE_EVENT_ENTRY));
    __analysis_assume(DeviceEventSize >= sizeof(PNP_DEVICE_EVENT_ENTRY));
    PNP_ASSERT(DeviceObject != NULL);

    ObReferenceObject(DeviceObject);
    deviceNode = PP_DO_TO_DN(DeviceObject);
    instancePathLength = CB_TO_CWC(PnpGetDeviceInstancePath(deviceNode)->Length);

    RtlZeroMemory((PVOID)DeviceEvent, DeviceEventSize);
    DeviceEvent->CallerEvent = SyncEvent;
    DeviceEvent->Argument = Problem;
    DeviceEvent->ArgumentStatus = ProblemStatus;
    DeviceEvent->VetoType = VetoType;
    DeviceEvent->VetoName = VetoName;
    DeviceEvent->RefCount = 1;
    eventData = &DeviceEvent->Data;
    eventData->TotalSize = CWC_TO_CB(instancePathLength) +
                            sizeof(WCHAR) + sizeof(PLUGPLAY_EVENT_BLOCK);

    PNP_ASSERT(eventData->TotalSize < DeviceEventSize);
    __analysis_assume(eventData->TotalSize < DeviceEventSize);

    eventData->DeviceObject = (PVOID)DeviceObject;
    eventData->EventCategory = TargetDeviceChangeEvent;
    eventData->Result = Result;
    if (DoEject) {
        eventData->EventGuid = GUID_DEVICE_EJECT;

    } else {
        eventData->EventGuid = GUID_DEVICE_QUERY_AND_REMOVE;
    }

    if (NoRestart) {
        eventData->Flags |= TDF_NO_RESTART;
    }

    if (KernelInitiated) {
        eventData->Flags |= TDF_KERNEL_INITIATED;
    }

    if (NotifyVeto) {
        eventData->Flags |= TDF_NOTIFY_VETO;
    }

    if (instancePathLength != 0) {
        RtlCopyMemory(eventData->u.TargetDevice.DeviceId,
                      PnpGetDeviceInstancePath(deviceNode)->Buffer,
                      CWC_TO_CB(instancePathLength));
    }

    eventData->u.TargetDevice.DeviceId[instancePathLength] = UNICODE_NULL;
    return;
}


NTSTATUS
PnpSetDeviceRemovalSafe(
    __in  PDEVICE_OBJECT DeviceObject,
    __in_opt PKEVENT SyncEvent           ,
    __out_opt PULONG Result
    )

/*++

Routine Description:

    This routine is called to notify user mode a device can be removed. The IO
    system may queue this event when a hardware initiated eject has completed.

Arguments:

    DeviceObject - Indicates the device object for the device that changed.

    SyncEvent - Optionally, specifies a kernel-mode event that will be set when the
            event is finished processing.

    Result - Supplies a pointer to a ULONG that will be filled in with the status
            after the event has actual completed (notification finished and the
            event processed). This value is not used when SyncEvent is NULL and
            is REQUIRED when SyncEvent is supplied.

Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    ULONG i;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpSetDeviceRemovalSafe: Entered\n"));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "    DeviceObject = 0x%p, SyncEvent = 0x%p, Result = 0x%p\n",
               DeviceObject,
               SyncEvent,
               Result));

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    if (SyncEvent != NULL) {
        PNP_ASSERT(Result != NULL);
        *Result = STATUS_PENDING;
    }

    //
    // Reference the device object so it can't go away until after we're
    // done with notification.
    //

    deviceNode = PP_DO_TO_DN(DeviceObject);

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field so it works out. Add one more for double-null term.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    dataSize += PnpGetDeviceInstancePath(deviceNode)->Length + sizeof(WCHAR);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ObReferenceObject(DeviceObject);
    deviceEvent->CallerEvent = SyncEvent;
    deviceEvent->Argument = 0;
    deviceEvent->ArgumentStatus = 0;
    deviceEvent->VetoType = NULL;
    deviceEvent->VetoName = NULL;
    deviceEvent->Data.EventGuid = GUID_DEVICE_SAFE_REMOVAL;
    deviceEvent->Data.EventCategory = TargetDeviceChangeEvent;
    deviceEvent->Data.Result = Result;
    deviceEvent->Data.Flags = 0;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = (PVOID)DeviceObject;
    if (PnpIsDeviceInstancePathEmpty(deviceNode) == FALSE) {
        RtlCopyMemory((PVOID)deviceEvent->Data.u.TargetDevice.DeviceId,
                    PnpGetDeviceInstancePath(deviceNode)->Buffer,
                    PnpGetDeviceInstancePath(deviceNode)->Length);
    }

    i = PnpGetDeviceInstancePath(deviceNode)->Length/sizeof(WCHAR);
    deviceEvent->Data.u.TargetDevice.DeviceId[i] = L'\0';
    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

NTSTATUS
PnpSetHwProfileChangeEvent(
    __in   GUID CONST *EventTypeGuid,
    __in_opt PKEVENT CompletionEvent    ,
    __out_opt PNTSTATUS CompletionStatus ,
    __out_opt PPNP_VETO_TYPE VetoType    ,
    __out_opt PUNICODE_STRING VetoName
    )
{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    ULONG totalSize;
    NTSTATUS status;

    PAGED_CODE();

#if DBG

    {
        CHAR    eventGuidString[80];

        PnpLookupGuid(EventTypeGuid, eventGuidString, sizeof(eventGuidString));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpSetHwProfileChangeEvent: Entered\n    EventGuid = %s\n\n",
                   eventGuidString));
    }

#endif

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    status = STATUS_SUCCESS;
    dataSize =  sizeof(PLUGPLAY_EVENT_BLOCK);
    totalSize = dataSize + FIELD_OFFSET (PNP_DEVICE_EVENT_ENTRY,Data);
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Setup the PLUGPLAY_EVENT_BLOCK
    //

    deviceEvent->CallerEvent = CompletionEvent;
    deviceEvent->VetoType = VetoType;
    deviceEvent->VetoName = VetoName;
    deviceEvent->Data.EventCategory = HardwareProfileChangeEvent;
    RtlCopyMemory(&deviceEvent->Data.EventGuid, EventTypeGuid, sizeof(GUID));
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.Result = (PULONG)CompletionStatus;
    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

NTSTATUS
PnpSetBlockedDriverEvent(
    __in   GUID CONST *BlockedDriverGuid
    )

/*++

Routine Description:

    This routine is called to notify user mode of blocked driver events.

Arguments:

    BlockedDriverGuid - Specifies the GUID which identifies the blocked driver.

Return Value:

    Returns the status of inserting the event into the synchronized pnp event
    queue.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Allocate a device event entry.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Setup the PLUGPLAY_EVENT_BLOCK
    //

    deviceEvent->Data.EventGuid = GUID_DRIVER_BLOCKED;
    deviceEvent->Data.EventCategory = BlockedDriverEvent;
    deviceEvent->Data.TotalSize = dataSize;
    RtlCopyMemory(&deviceEvent->Data.u.BlockedDriverNotification.BlockedDriverGuid,
                  BlockedDriverGuid,
                  sizeof(GUID));

    //
    // Insert the event into the queue.
    //

    status = PnpInsertEventInQueue(deviceEvent);
    return status;

}

NTSTATUS
PnpSetInvalidIDEvent(
    __in   PUNICODE_STRING ParentInstance
    )
/*++

Routine Description:

    This routine is called to notify user mode when an invalid ID is encountered.

Arguments:

    ParentInstance - Specifies the instance path of the parent of the device with
    invalid ID.

Return Value:

    Returns the status of inserting the event into the synchronized pnp event
    queue.

--*/
{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Allocate a device event entry (note that we have one WCHAR reserved
    // as part of the block structure itself).
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK) +
                    ParentInstance->Length + sizeof(UNICODE_NULL);

    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Setup the PLUGPLAY_EVENT_BLOCK
    //

    deviceEvent->Data.EventGuid = GUID_DEVICE_INVALID_ID;
    deviceEvent->Data.EventCategory = InvalidIDEvent;
    deviceEvent->Data.TotalSize = dataSize;
    RtlCopyMemory(&deviceEvent->Data.u.InvalidIDNotification.ParentId[0],
                  ParentInstance->Buffer,
                  ParentInstance->Length);

    deviceEvent->Data.u.InvalidIDNotification.ParentId[ParentInstance->Length / sizeof(WCHAR)] = UNICODE_NULL;
    deviceEvent->Data.u.InvalidIDNotification.ParentId[(ParentInstance->Length / sizeof(WCHAR)) + 1] = UNICODE_NULL;

    //
    // Insert the event into the queue.
    //

    status = PnpInsertEventInQueue(deviceEvent);
    return status;

}

NTSTATUS
PnpSetPowerVetoEvent(
    __in  POWER_ACTION    VetoedPowerOperation,
    __in_opt PKEVENT         CompletionEvent,
    __out_opt PNTSTATUS       CompletionStatus,
    __in  PDEVICE_OBJECT  DeviceObject,
    __in  PNP_VETO_TYPE   VetoType,
    __in_opt PUNICODE_STRING VetoName
    )
/*++

--*/
{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    ULONG i;
    NTSTATUS status;
    ULONG totalSize;
    PWCHAR vetoData;

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Given the pdo, retrieve the devnode (the device instance string is
    // attached to the devnode in the InstancePath field).
    //

    deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;
    if (deviceNode == NULL) {
        return STATUS_INVALID_PARAMETER_2;
    }

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (because of the first index into
    // the DeviceIdVetoNameBuffer, this is double null terminated).
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK) +
               PnpGetDeviceInstancePath(deviceNode)->Length +
               (VetoName ? VetoName->Length : 0) +
               sizeof(WCHAR)*2;

    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    //
    // Reference the device object so it can't go away until after we're
    // done with notification.
    //

    ObReferenceObject(DeviceObject);
    deviceEvent->CallerEvent = CompletionEvent;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = (PVOID)DeviceObject;
    deviceEvent->Data.Result = (PULONG)CompletionStatus;
    deviceEvent->Data.u.VetoNotification.VetoType = VetoType;

    //
    // You can think of this as a MultiSz string where the first entry is the
    // DeviceId for the device being removed, and the next Id's all corrospond
    // to the vetoers.
    //

    RtlCopyMemory(deviceEvent->Data.u.VetoNotification.DeviceIdVetoNameBuffer,
                  PnpGetDeviceInstancePath(deviceNode)->Buffer,
                  PnpGetDeviceInstancePath(deviceNode)->Length);

    i = PnpGetDeviceInstancePath(deviceNode)->Length;
    deviceEvent->Data.u.VetoNotification.DeviceIdVetoNameBuffer[i/sizeof(WCHAR)] = UNICODE_NULL;
    if (VetoName) {
        vetoData = (&deviceEvent->Data.u.VetoNotification.DeviceIdVetoNameBuffer[i/sizeof(WCHAR)])+1;
        RtlCopyMemory(vetoData, VetoName->Buffer, VetoName->Length);
        vetoData[VetoName->Length/sizeof(WCHAR)] = UNICODE_NULL;
    }

    //
    // No need to NULL terminate the entry after the last one as we prezero'd
    // the buffer. Now set the appropriate GUID so the UI looks right.
    //

    if (VetoedPowerOperation == PowerActionWarmEject) {
        deviceEvent->Data.EventGuid = GUID_DEVICE_WARM_EJECT_VETOED;

    } else if (VetoedPowerOperation == PowerActionHibernate) {
        deviceEvent->Data.EventGuid = GUID_DEVICE_HIBERNATE_VETOED;

    } else {
        deviceEvent->Data.EventGuid = GUID_DEVICE_STANDBY_VETOED;
    }

    deviceEvent->Data.EventCategory = VetoEvent;
    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

VOID
PnpSetPlugPlayEvent (
    __in CONST GUID *EventGuid,
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine allows kernel mode enumerator to inform Plug and Play Manager
    on the events triggered by enumeration, i.e., DeviceArrived and DeviceRemoved.
    The PnP manager can then inform user-mode about the event.

Arguments:

    EventId - Indicates what event is triggered by enumeration.


Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    ULONG totalSize;

#if DBG

    CHAR eventGuidString[80];

#endif

    PAGED_CODE();

    PNP_ASSERT(EventGuid != NULL);
    PNP_ASSERT(DeviceObject != NULL);

#if DBG

    {
        PnpLookupGuid(EventGuid, eventGuidString, sizeof(eventGuidString));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpSetPlugPlayEvent: Entered\n    EventGuid = %s\n",
                   eventGuidString));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceObject = 0x%p\n",
                   DeviceObject));
    }

#endif

    if (PnpIsShutDown()) {
        return;
    }

    //
    // Given the pdo, retrieve the devnode (the device instance string is
    // attached to the devnode in the InstancePath field).
    //

    deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;
    if (deviceNode == NULL) {
        return;
    }

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK) +
                PnpGetDeviceInstancePath(deviceNode)->Length;

    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return;
    }

    RtlCopyMemory(&deviceEvent->Data.EventGuid, EventGuid, sizeof(GUID));
    deviceEvent->Data.TotalSize = dataSize;
    if (PnpCompareGuid(EventGuid, &GUID_DEVICE_ENUMERATED)) {

        //
        // GUID_DEVICE_ENUMERATED events are device installation requests for
        // user-mode, and are sent using the DeviceInstallEvent event type.
        //

        deviceEvent->Data.EventCategory = DeviceInstallEvent;
        RtlCopyMemory(&deviceEvent->Data.u.InstallDevice.DeviceId,
                      PnpGetDeviceInstancePath(deviceNode)->Buffer,
                      PnpGetDeviceInstancePath(deviceNode)->Length);

        deviceEvent->Data.u.InstallDevice.DeviceId[PnpGetDeviceInstancePath(deviceNode)->Length/sizeof(WCHAR)] = 0x0;

    } else {

        //
        // All other target events are sent using the TargetDeviceChangeEvent
        // event type, and are distinguished by the EventGuid.
        //

        deviceEvent->Data.EventCategory = TargetDeviceChangeEvent;
        RtlCopyMemory(&deviceEvent->Data.u.TargetDevice.DeviceId,
                      PnpGetDeviceInstancePath(deviceNode)->Buffer,
                      PnpGetDeviceInstancePath(deviceNode)->Length);
        deviceEvent->Data.u.TargetDevice.DeviceId[PnpGetDeviceInstancePath(deviceNode)->Length/sizeof(WCHAR)] = 0x0;
    }

    //
    // Don't hold a reference count against the DO for these events. This can
    // lead to really annoying Critical Device Database vs. Installation vs.
    // I/O driver refcount races.
    //

    if (PnpCompareGuid(EventGuid, &GUID_DEVICE_ENUMERATED) ||
        PnpCompareGuid(EventGuid, &GUID_DEVICE_ARRIVAL)) {
        DeviceObject = NULL;

    } else {

        //
        // Reference the device object so it can't go away until after we're
        // done with notification.
        //
        PNP_ASSERT(DeviceObject);
        ObReferenceObject(DeviceObject);
    }

    deviceEvent->Data.DeviceObject = (PVOID)DeviceObject;
    PnpInsertEventInQueue(deviceEvent);
    return;
}

NTSTATUS
PnpSynchronizeDeviceEventQueue (
    VOID
    )
{
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    KEVENT event;
    ULONG result;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Note this is the only queuing function which is valid when PpShuttingDown
    // is TRUE.
    //

    deviceEvent = PnpCreateDeviceEventEntry(sizeof(PNP_DEVICE_EVENT_ENTRY));
    if (deviceEvent == NULL) {
        return STATUS_NO_MEMORY;
    }

    KeInitializeEvent(&event, NotificationEvent, FALSE);
    deviceEvent->CallerEvent = &event;
    deviceEvent->Data.EventGuid = GUID_DEVICE_NOOP;
    deviceEvent->Data.EventCategory = TargetDeviceChangeEvent;
    deviceEvent->Data.Result = &result;
    deviceEvent->Data.TotalSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    status = PnpInsertEventInQueue(deviceEvent);
    if (NT_SUCCESS(status)) {
        status = KeWaitForSingleObject(&event,
                                       Executive,
                                       KernelMode,
                                       FALSE,       // not alertable
                                       0);          // infinite
    }

    return status;
}

VOID
PnpDeviceEventWorker(
    __in PVOID Context
    )

/*++

Routine Description:

    If the master device list contains any device events, empty the list now.
    This is a worker item thread routine (queued by PiPostNotify). We walk the
    list - this will cause the oldest device event on the list to be sent to
    all registered recipients and then the device event will be removed (if at
    least one recipient received it).

    Order Rules:
        Interface Devices - kernel mode first, user-mode second
        Hardware profile changes - user-mode first, kernel-mode second
        Target device changes (query remove, remove) : user-mode first, send
        (cancel remove)        : kernel-mode first, post
        (custom)               : kernel-mode first, post

Arguments:

    NONE.

Return Value:

    NONE.

--*/

{
    BOOLEAN clearActivityId;
    PLIST_ENTRY current;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    LPGUID guidPtr;
    GUID localActivityId;
    NTSTATUS  status;
    UNICODE_STRING tempString;

    PAGED_CODE();

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpDeviceEventWorker: Worker thread entered\n"));

    PnpDeviceEventThread = PsGetCurrentThread();

    //
    // Empty the device event list, remove entries from the head of the list
    // (deliver oldest entries first).
    //
    // As this function always executes in a system process workitem, we
    // don't have to grab a critical region lock.
    //

    status = KeWaitForSingleObject(&PnpDeviceEventList->EventQueueMutex,
                                   Executive,
                                   KernelMode,
                                   FALSE,       // not alertable
                                   0);          // infinite

    if (!NT_SUCCESS(status)) {
        KeAcquireGuardedMutex(&PnpNotificationInProgressLock);
        KeSetEvent(&PnpEventQueueEmpty, 0, FALSE);
        PnpNotificationInProgress = FALSE;
        KeReleaseGuardedMutex(&PnpNotificationInProgressLock);
        PnpDeviceEventThread = NULL;
        return;
    }

    for ( ; ; ) {
        clearActivityId = FALSE;
        KeAcquireGuardedMutex(&PnpDeviceEventList->Lock);
        if (!IsListEmpty(&PnpDeviceEventList->List)) {
            current = RemoveHeadList(&PnpDeviceEventList->List);
            InitializeListHead(current);
            KeReleaseGuardedMutex(&PnpDeviceEventList->Lock);
            deviceEvent = CONTAINING_RECORD(current,                // address
                                            PNP_DEVICE_EVENT_ENTRY, // type
                                            ListEntry);             // field

#if DBG

            {
                CHAR    guidString[256];

                PnpLookupGuid(&deviceEvent->Data.EventGuid, guidString, sizeof(guidString));
                IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                           "PnpDeviceEventWorker: Processing queued event - EventGuid = %s\n",
                           guidString));
            }

#endif

            //
            // If a non-empty activity ID exists in device event, associate it
            // with the thread for the duration of the processing.
            //

            if (!IsEqualGUID(&NullGuid, &deviceEvent->ActivityId)) {

                guidPtr = &localActivityId;

                //
                // Assert that the thread doesn't currently have an activity
                // ID.  Since this thread is potentially processing multiple
                // events from the device event list, this could indicate that
                // processing a previous event didn't properly clean up the
                // activity ID.  Alternatively, since this runs on a worker
                // thread, this could indicate that the previous work item using
                // this thread did not clean up the activity ID they set.  This
                // code currently clearly cleans up the activity ID, so the
                // assert firing means a previous work item using this thread
                // didn't clean up the activity ID it set.
                //

                PNP_ASSERT(PsGetCurrentThread()->ActivityId == NULL);

                localActivityId = deviceEvent->ActivityId;
                NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(
                                         EVENT_ACTIVITY_CTRL_SET_ID,
                                         &guidPtr)));

                clearActivityId = TRUE;
            }

            status = STATUS_SUCCESS;
            if (deviceEvent->Data.DeviceObject != NULL) {
                deviceNode = PP_DO_TO_DN((PDEVICE_OBJECT)deviceEvent->Data.DeviceObject);
                if (deviceNode == NULL) {
                    status = STATUS_NO_SUCH_DEVICE;
                }
            }

            //
            // Ensure that the action queue is not running.  If it is,
            // outstanding reenumeration requests may cause a removal to be
            // queued which should be allowed to complete.
            //

            if (PnpIsShutDown() &&
                (InterlockedCompareExchangePointer(&PnpDeviceActionThread,
                                                   NULL,
                                                   NULL) == NULL)) {

                status = STATUS_TOO_LATE;
            }

            if (NT_SUCCESS(status)) {
                switch (deviceEvent->Data.EventCategory) {
                    case DeviceClassChangeEvent:

                        //
                        // Notify kernel-mode (synchronous).
                        //

                        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                   "PnpDeviceEventWorker: DeviceClassChangeEvent - notifying kernel-mode\n"));

                        RtlInitUnicodeString(&tempString,
                                             deviceEvent->Data.u.DeviceClass.SymbolicLinkName);

                        PnpNotifyDeviceClassChange(&deviceEvent->Data.EventGuid,
                                                   &deviceEvent->Data.u.DeviceClass.ClassGuid,
                                                   &tempString);

                        //
                        // Notify user-mode (synchronous).
                        //

                        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                   "PnpDeviceEventWorker: DeviceClassChangeEvent - user kernel-mode\n"));

                        PiUEventNotifyUserMode(deviceEvent);
                        status = STATUS_SUCCESS;
                        break;

                    case CustomDeviceEvent:
                        status = PnpProcessCustomDeviceEvent(&deviceEvent);
                        break;

                    case TargetDeviceChangeEvent:
                        status = PnpProcessTargetDeviceEvent(&deviceEvent);
                        break;

                    case DeviceInstallEvent:

                        //
                        // Notify user-mode
                        //

                        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                   "PnpDeviceEventWorker: DeviceInstallEvent - notifying user-mode\n"));

                        PiUEventNotifyUserMode(deviceEvent);

                        status = STATUS_SUCCESS;
                        break;

                    case HardwareProfileChangeEvent:

                        //
                        // Notify user-mode (synchronous).
                        //

                        status = PiUEventNotifyUserMode(deviceEvent);

                        if (NT_SUCCESS(status)) {

                            //
                            // Notify K-Mode
                            //

                            PnpNotifyHwProfileChange(&deviceEvent->Data.EventGuid,
                                                     deviceEvent->VetoType,
                                                     deviceEvent->VetoName);
                        }

                        break;

                    case VetoEvent:
                    case BlockedDriverEvent:
                    case InvalidIDEvent:

                        //
                        // Do nothing with these
                        //

                        break;

                    case DeviceInstanceRemovalEvent:

                        //
                        // Forward onto user-mode.
                        //

                        status = PiUEventNotifyUserMode(deviceEvent);
                        break;

                    case DevicePropertyChangeEvent:

                        //
                        // Forward onto user-mode
                        //

                        status = PiUEventNotifyUserMode(deviceEvent);
                        break;

                    case DeviceInstanceStartedEvent:

                        //
                        // Forward onto user-mode
                        //

                        status = PiUEventNotifyUserMode(deviceEvent);
                        break;

                    default:

                        //
                        // These should never be queued to kernel mode. They are
                        // notifications for user mode, and should only be seen
                        // through the PiNotifyUserModeXxx functions.
                        //

                        PNP_ASSERT(0);
                        status = STATUS_UNSUCCESSFUL;
                        break;
                };
            }

            if (status != STATUS_PENDING) {
                PnpCompleteDeviceEvent(deviceEvent, status);
            }

            //
            // Commit pending registrations after processing each event.
            //

            PnpProcessDeferredRegistrations();

            if (clearActivityId) {

                guidPtr = NULL;

                PNP_ASSERT(PsGetCurrentThread()->ActivityId != NULL);

                NT_VERIFY(NT_SUCCESS(EtwActivityIdControlKernel(
                                         EVENT_ACTIVITY_CTRL_SET_ID,
                                         &guidPtr)));
            }

        } else {
            KeAcquireGuardedMutex(&PnpNotificationInProgressLock);
            KeSetEvent(&PnpEventQueueEmpty, 0, FALSE);
            PnpNotificationInProgress = FALSE;

            //
            // Commit pending registrations after processing all queued events.
            //

            PnpProcessDeferredRegistrations();
            KeReleaseGuardedMutex(&PnpNotificationInProgressLock);
            KeReleaseGuardedMutex(&PnpDeviceEventList->Lock);
            break;
        }
    }

    if (Context != NULL) {
        ExFreePoolWithTag(Context, PNP_POOLTAG_DEVICE_EVENT_WORK_ITEM);
    }

    PnpDeviceEventThread = NULL;
    KeReleaseMutex(&PnpDeviceEventList->EventQueueMutex, FALSE);
    return;
}

VOID
PnpCompleteDeviceEvent(
    __inout PPNP_DEVICE_EVENT_ENTRY  DeviceEvent,
    __in    NTSTATUS                 FinalStatus
    )

/*++

Routine Description:


Arguments:

    DeviceEvent     - Event to complete.
    FinalStatus     - Final status for this event.

Return Value:

    NONE.

--*/

{

    LARGE_INTEGER delay = {(ULONG)(-10 * 1000 * 100), -1}; // 100ms

#if DBG

    CHAR guidString[256];

#endif

    PAGED_CODE();

    PNP_ASSERT(DeviceEvent);

#if DBG

    PnpLookupGuid(&DeviceEvent->Data.EventGuid, guidString, sizeof(guidString));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpCompleteDeviceEvent: Completing queued event - EventGuid = %s with %08lx\n",
               guidString,
               FinalStatus));

#endif

    //
    // Get the device event lock
    //
    while (InterlockedExchange((PLONG) &DeviceEvent->Lock, TRUE) != FALSE) {
        KeDelayExecutionThread(KernelMode, FALSE, &delay);
    }

    if (DeviceEvent->Parent != NULL) {
        while (InterlockedExchange((PLONG) &DeviceEvent->Parent->Lock, TRUE) != FALSE) {
            KeDelayExecutionThread(KernelMode, FALSE, &delay);
        }
    }

    //
    // Check if we should post-process this device event.
    //
    if (DeviceEvent->Cancel == FALSE &&
        (DeviceEvent->Parent == NULL ||
         DeviceEvent->Parent->Cancel == FALSE)) {

        //
        // If synchronous, signal the user-supplied event.
        //

        if (DeviceEvent->CallerEvent != NULL) {
            *DeviceEvent->Data.Result = FinalStatus;
            KeSetEvent(DeviceEvent->CallerEvent, 0, FALSE);

        //
        // N.B. Update the final status for target device change events as it
        //      is relied upon in device reset scenarios (non sync).
        //

        } else if ((DeviceEvent->Data.Result != NULL) &&
                   (DeviceEvent->Data.EventCategory ==
                        TargetDeviceChangeEvent)) {

            *DeviceEvent->Data.Result = FinalStatus;
        }

        if (DeviceEvent->Callback != NULL) {
            DeviceEvent->Callback(DeviceEvent->Context);
        }
    } else {

        //
        // If this event was cancelled we need to free the veto
        // buffer if present as the originator will not do so.
        //
        if (DeviceEvent->VetoType != NULL) {
            ExFreePool(DeviceEvent->VetoType);
        }
    }

    //
    // Release the reference we took for this device object during
    // the PnpSetCustomTargetEvent call.
    //

    if (DeviceEvent->Data.DeviceObject != NULL) {
        ObDereferenceObject(DeviceEvent->Data.DeviceObject);
    }

    //
    // Assume device event was delivered successfully, get rid of it.
    //

    if (DeviceEvent->Parent != NULL) {
        PnpDeleteDeviceEventEntry(DeviceEvent->Parent);
    }

    PnpDeleteDeviceEventEntry(DeviceEvent);
    return;
}

PPNP_DEVICE_EVENT_ENTRY
PnpCreateDeviceEventEntry (
    __in ULONG Size
    )


/*++

Routine Description

    This routine allocates a device event entry of specified size.

Arguments:

    Size - Supplies size of device event entry.

Return Value:

    Retuns a zeroed device event entry of specified size iff successful,
    else NULL.

--*/

{
    PPNP_DEVICE_EVENT_ENTRY entry;

    PAGED_CODE();

    entry = ExAllocatePoolWithTag(PagedPool,
                                  Size,
                                  PNP_POOLTAG_DEVICE_EVENT_ENTRY);
    if (entry != NULL) {
        RtlZeroMemory(entry, Size);
        entry->RefCount = 1;
    }

    return entry;
}

NTSTATUS
PnpNotifyUserModeDeviceRemoval (
    __in  PPNP_DEVICE_EVENT_ENTRY TemplateDeviceEvent,
    __in  PWSTR                   RelatedDeviceInstances,
    __in  CONST GUID              *EventGuid,
    __out_opt PPNP_VETO_TYPE      VetoType,
    __out_opt PUNICODE_STRING     VetoName
    )
/*++

Routine Description:

    This routine tells user mode to perform a specific device removal
    operation.

Arguments:

    TemplateDeviceEvent - Device event containing information about the
                          intended event (includes a list of devices.) The
                          event is temporarily used by this function, and is
                          restored before this function returns.

                          NOTE: The TargetDevice.DeviceId field must be big
                          enough to hold the largest device instance id. This
                          is done by calling PnpResizeTargetDeviceEvent.

    EventGuid - Points to the event user mode should process:
        GUID_TARGET_DEVICE_QUERY_REMOVE
        GUID_TARGET_DEVICE_REMOVE_CANCELLED
        GUID_DEVICE_REMOVE_PENDING
        GUID_TARGET_DEVICE_REMOVE_COMPLETE
        GUID_DEVICE_SURPRISE_REMOVAL

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

Return Value:

    NTSTATUS.

--*/
{
    GUID oldGuid;
    PUNICODE_STRING oldVetoName;
    PPNP_VETO_TYPE oldVetoType;
    WCHAR originalTargetDeviceId[MAX_DEVICE_ID_LEN];
    PWSTR p;
    NTSTATUS status = STATUS_SUCCESS;

#if DBG

    CHAR guidString[256];

#endif

    PAGED_CODE();

#if DBG

    PnpLookupGuid(EventGuid, guidString, sizeof(guidString));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpNotifyUserModeDeviceRemoval: %s - notifying user-mode\n",
               guidString));

#endif

    //
    // Save the old guid so we can use the template without making a copy. We
    // preserve it so that the removal veto UI can use the original event GUID
    // to let the UI differentiate disables from ejects, etc.
    //

    RtlCopyMemory(&oldGuid, &TemplateDeviceEvent->Data.EventGuid, sizeof(GUID));

    //
    // Do the same with the vetoname and vetobuffer.
    //

    oldVetoType = TemplateDeviceEvent->VetoType;
    oldVetoName = TemplateDeviceEvent->VetoName;

    //
    // Save the original target device id. This is because we loop over the
    // related device instance id list and send one notification per id to
    // user-mode by copying over the DeviceId field.
    //

    status = RtlStringCchCopyW(originalTargetDeviceId,
                               MAX_DEVICE_ID_LEN,
                               TemplateDeviceEvent->Data.u.TargetDevice.DeviceId);

    PNP_ASSERT(status == STATUS_SUCCESS);

    if (status != STATUS_SUCCESS) {
        return status;
    }

    //
    // Copy in the new data.
    //

    RtlCopyMemory(&TemplateDeviceEvent->Data.EventGuid, EventGuid, sizeof(GUID));
    TemplateDeviceEvent->VetoType = VetoType;
    TemplateDeviceEvent->VetoName = VetoName;

    //
    // Send it.
    //

    for (p = RelatedDeviceInstances; *p; p += (wcslen(p) + 1)) {

        //
        // Copy the device instance into the device event and send it to user-mode. We
        // bail at the first sign of the an error. Note that the DeviceId field of the
        // device event has been resized to be able to contain the largest device
        // instance id, so we shouldn't have any buffer overflow issues here.
        //

        RtlCopyMemory(TemplateDeviceEvent->Data.u.TargetDevice.DeviceId, p, CWC_TO_CB(wcslen(p)+1));

        status = PiUEventNotifyUserMode(TemplateDeviceEvent);

        if (!NT_SUCCESS(status)) {
            break;
        }

    }

    //
    // Restore the old info.
    //

    RtlCopyMemory(&TemplateDeviceEvent->Data.EventGuid, &oldGuid, sizeof(GUID));

    TemplateDeviceEvent->VetoType = oldVetoType;
    TemplateDeviceEvent->VetoName = oldVetoName;

    RtlCopyMemory(TemplateDeviceEvent->Data.u.TargetDevice.DeviceId, originalTargetDeviceId, CWC_TO_CB(wcslen(originalTargetDeviceId) + 1));

    return status;
}

NTSTATUS
PnpProcessQueryRemoveAndEject(
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    )

/*++

Routine Description:

    This routine processes the various flavours of remove: Eject, SurpriseRemove,
    Remove, and QueryRemove.

    Eject
        Retrieve bus, removal, and eject relations.
        Do queries on all relations
        Send IRP_MN_REMOVE_DEVICE to all the relations.
        Queue the pending eject

        Once the eject happens
            Reenumerate all the indirect relation's parents

    SurpriseRemove
        Retrieve bus, removal, and eject relations.
        Send IRP_MN_SURPRISE_REMOVAL to all the direct relations.
        Notify everyone that the device is gone.
        Reenumerate the parents of all the indirect relations.
        Remove the indirect relations from the relations list.
        Queue the pending surprise removal.

        Once the last handle is closed.

            Send IRP_MN_REMOVE_DEVICE to all the direct relations.

    RemoveFailedDevice
        Retrieve bus and removal relations.
        Notify everyone that the device is going.
        Reenumerate the parents of all the indirect relations.
        Remove the indirect relations from the relations list.
        Queue as a pending surprise removal.

        Once the last handle is closed.

            Send IRP_MN_REMOVE_DEVICE to all the direct relations.

    RemoveUnstartedFailedDevice
        Retrieve bus relations.
        Notify everyone that the device is going.
        Send IRP_MN_REMOVE_DEVICE to all the direct relations.

    Remove
        Use the relations from the last QueryRemove or retrieve new bus, removal,
        and eject relations if the device wasn't already QueryRemoved.
        Send IRP_MN_REMOVE_DEVICE to all the relations.

Arguments:

    Response - Result of event processing in user-mode.

Return Value:

    NTSTATUS code.

--*/

{

    PNP_DEVICE_DELETE_TYPE deleteType;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    PNP_DEVICE_STATE deviceState;
    PDOCK_INTERFACE dockInterface = NULL;
    BOOLEAN excludeDeviceRelations;
    BOOLEAN hotEjectSupported;
    BOOLEAN ignoreOpenHandlesonDeviceReset;
    LONG index;
    PUNICODE_STRING internalVetoString;
    PPNP_VETO_TYPE internalVetoType;
    PVOID internalVetoBase = NULL;
    RELATION_LIST_MARKER marker;
    PDEVICE_OBJECT *pdoList = NULL;
    ULONG pdoListCount;
    PPENDING_RELATIONS_LIST_ENTRY pendingRelations;
    BOOLEAN possibleProfileChangeInProgress = FALSE;
    PDEVICE_NODE relatedDeviceNode;
    PDEVICE_OBJECT relatedDeviceObject;
    DEVICE_RELATION_LEVEL relationLevel;
    PRELATION_LIST relationsList;
    PRESET_REMOVAL_CONTEXT resetRemovalContext;
    UNICODE_STRING singleVetoListItem;
    NTSTATUS status;
    BOOLEAN subsumingProfileChange = FALSE;
    PDEVICE_OBJECT vetoingDevice = NULL;
    PDRIVER_OBJECT vetoingDriver = NULL;
    PWSTR vetoList, relatedDeviceInstances = NULL, surpriseRemovalList = NULL;
    BOOLEAN warmEjectSupported;

    PAGED_CODE();

    deviceEvent = *DeviceEvent;
    deviceObject = (PDEVICE_OBJECT)deviceEvent->Data.DeviceObject;
    deviceNode = deviceObject->DeviceObjectExtension->DeviceNode;
    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);
    if (PipIsDevNodeDeleted(deviceNode)) {

        //
        // Device already deleted, nothing to do here.
        //

        status = STATUS_DELETE_PENDING;
        goto Clean0;
    }

    PNP_ASSERT(deviceNode->Parent != NULL);

    deleteType = PiEventRemovalGetDeleteType(deviceNode, deviceEvent);
    excludeDeviceRelations = PiEventExcludeDeviceRelations(deleteType);

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessQueryRemoveAndEject: Remove type %x\n", deleteType));

    if ((deleteType == QueryRemoveDevice) || (deleteType == EjectDevice)) {
        if (deviceNode->Flags & DNF_LEGACY_DRIVER) {
            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Vetoed by legacy driver\n"));

            PnpFinalizeVetoedRemove(deviceEvent,
                                    PNP_VetoLegacyDevice,
                                    PnpGetDeviceInstancePath(deviceNode));

            status = STATUS_PLUGPLAY_QUERY_VETOED;
            goto Clean0;
        }
    }

    if (deleteType == QueryRemoveDevice && deviceEvent->Argument == CM_PROB_DISABLED) {

        //
        // if we're trying to remove the device to disable the device
        //

        if (deviceNode->DisableableDepends > 0) {
            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Device is non-disableable\n"));

            //
            // we should have caught this before (in usermode PnP)
            // but a rare scenario can exist where the device becomes non-disableable
            // There is still a potential gap, if the device hasn't got around
            // to marking itself as non-disableable yet
            //

            PnpFinalizeVetoedRemove(deviceEvent,
                                    PNP_VetoNonDisableable,
                                    PnpGetDeviceInstancePath(deviceNode));

            status = STATUS_PLUGPLAY_QUERY_VETOED;
            goto Clean0;
        }
    }

    //
    // Allocate room for a possible veto buffer.
    //

    internalVetoBase = PiEventAllocateVetoBuffer(deleteType,
                                                 &internalVetoType,
                                                 &internalVetoString);

    if (internalVetoBase == NULL) {
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpProcessQueryRemoveAndEject: Vetoed due to failure to allocate vetobuffer\n"));

        PnpFinalizeVetoedRemove(deviceEvent,
                                PNP_VetoTypeUnknown,
                                NULL);

        status = STATUS_PLUGPLAY_QUERY_VETOED;
        goto Clean0;
    }

    if (deleteType == EjectDevice) {
        if (deviceNode->Flags & DNF_LOCKED_FOR_EJECT) {
            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Device already being ejected\n"));

            //
            // Either this node or one of its ancestors is already being ejected.
            //

            status = STATUS_SUCCESS;
            goto Clean0;
        }

        if ((deviceNode->DockInfo.DockStatus == DOCK_DEPARTING) ||
            (deviceNode->DockInfo.DockStatus == DOCK_EJECTIRP_COMPLETED)) {

            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Dock already being ejected\n"));

            //
            // We already have an eject queued against this device. Don't allow
            // another eject to break into the middle of a queue/cancel warm
            // eject sequence.
            //

            status = STATUS_SUCCESS;
            goto Clean0;
        }

        //
        // What types of ejection can we do? (warm/hot)
        //

        if (!IopDeviceNodeFlagsToCapabilities(deviceNode)->Removable) {

            //
            // This device is neither ejectable, nor even removable.
            //

            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Device not removable\n"));

            PnpFinalizeVetoedRemove(deviceEvent,
                                    PNP_VetoIllegalDeviceRequest,
                                    PnpGetDeviceInstancePath(deviceNode));

            status = STATUS_PLUGPLAY_QUERY_VETOED;
            goto Clean0;
        }
    }

    if ((deleteType == QueryRemoveDevice) && (!PipAreDriversLoaded(deviceNode))) {

        //
        // The device doesn't have an FDO.
        //

        status = PiEventQueryRemoveNoFdo(deviceNode, deviceEvent);
        goto Clean0;
    }

    status = PnpBuildRemovalRelationList(deviceObject,
                                         deleteType,
                                         internalVetoType,
                                         internalVetoString,
                                         &relationsList);

    if (!NT_SUCCESS(status)) {
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpProcessQueryRemoveAndEject: Failed to build removal relations\n"));

        PnpFinalizeVetoedRemove(deviceEvent,
                                *internalVetoType,
                                internalVetoString);

        status = STATUS_PLUGPLAY_QUERY_VETOED;
        goto Clean0;
    }

    PNP_ASSERT(relationsList != NULL);
    PNP_ASSERT(!IopGetRelationsTaggedCount(relationsList));

    //
    // PdoList will become a list of devices that must be queried. This is
    // a subset of all the devices that might disappear, all of which appear
    // in the relations list.
    //

    status = PiEventBuildPdoList(relationsList,
                                 deleteType,
                                 deviceEvent,
                                 &pdoListCount,
                                 &pdoList,
                                 &possibleProfileChangeInProgress,
                                 &subsumingProfileChange);

    if (NT_SUCCESS(status)) {
        status = PnpResizeTargetDeviceBlock(DeviceEvent, deleteType);
        deviceEvent = *DeviceEvent;
        if (NT_SUCCESS(status)) {
            status = PnpCompileDeviceInstancePaths(deleteType,
                                                   relationsList,
                                                   excludeDeviceRelations,
                                                   FALSE,
                                                   &relatedDeviceInstances);

            if (deleteType == SurpriseRemoveDevice) {
                PnpBuildUnsafeRemovalDeviceList(deviceEvent,
                                                relationsList,
                                                &surpriseRemovalList);
            }
        }
    }

    if (!NT_SUCCESS(status)) {
        IopFreeRelationList(relationsList);
        PnpFinalizeVetoedRemove(deviceEvent,
                                PNP_VetoTypeUnknown,
                                NULL);

        goto Clean0;
    }

    //
    // We may need to take the hardware profile change semaphore, and also
    // broadcast a hardware profile change request...
    //

    if (possibleProfileChangeInProgress) {
        PpProfileBeginHardwareProfileTransition(subsumingProfileChange);

        //
        // Walk the list of docks who are going to disappear and mark them as
        // in profile transition.
        //

        for (index = pdoListCount - 1; index >= 0; index--) {
            relatedDeviceObject = pdoList[ index ];
            relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
            PNP_ASSERT(relatedDeviceNode->DockInfo.DockStatus != DOCK_ARRIVING);
            if (relatedDeviceNode->DockInfo.DockStatus == DOCK_QUIESCENT) {

                PpProfileIncludeInHardwareProfileTransition(
                    relatedDeviceNode,
                    DOCK_DEPARTING
                    );
            }
        }

        //
        // We can only be in one of the following deleteType situations
        //
        // 1) EjectDevice          - Good, normal ejection request by our user
        //                           (we are using EjectionRelations)
        //
        // 2) SurpriseRemoveDevice - Someone yanked the dock out.
        //                           (we are using EjectionRelations)
        //
        // 3) RemoveFailedDevice   - A start failed after a stop on a parent or
        //                           even our device. This case is not handled
        //                           correctly. We assert for now, and we
        //                           maroon the dock, ie lose it's devnode but
        //                           the dock stays physically present and is
        //                           in the users eye's unejectable.
        //
        // 4) RemoveDevice         - This occurs in three cases:
        //                              a) A removed device is disappearing.
        //                              b) A device is being removed but has not
        //                                 been started.
        //                              c) A device has failed start.
        //
        //                           We pass through case a) during a normal
        //                           ejection and as part of a profile
        //                           transition begun earlier. c) is similar
        //                           to a) but the transition was begun by the
        //                           start code. For case b) we don't want to
        //                           turn it into an eject, as the OS might be
        //                           removing our parent as a normal part of
        //                           setup, and we wouldn't want to undock then
        //                           (and we probably aren't changing profiles
        //                           anyway).
        //
        // 5) QueryRemoveDevice      This should never be the case here per the
        //                           explicit veto in the IopEnumerateRelations
        //                           code above.
        //
        // RemoveFailedDevice is a PathTrap - the only parent of a dock is
        // the ACPI root devnode right now. We shouldn't get into that case.
        //

        PNP_ASSERT(deleteType != QueryRemoveDevice);
        if (deleteType == EjectDevice) {

            //
            // Are there any legacy drivers in the system?
            //

            status = IoGetLegacyVetoList(&vetoList, internalVetoType);
            if (NT_SUCCESS(status) &&
                (*internalVetoType != PNP_VetoTypeUnknown)) {

                IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                           "PnpProcessQueryRemoveAndEject: Vetoed due to presence of a legacy driver\n"));

                //
                // Release any docks in profile transition
                //

                PpProfileCancelHardwareProfileTransition();
                IopFreeRelationList(relationsList);

                //
                // Failure occured, notify user mode as appropriate, or fill in
                // the veto buffer.
                //

                if (deviceEvent->VetoType != NULL) {
                    *deviceEvent->VetoType = *internalVetoType;
                }

                if (deviceEvent->VetoName != NULL) {

                    //
                    // The veto data in the PNP_DEVICE_EVENT_ENTRY block is
                    // a UNICODE_STRING field. Since that type of data structure
                    // cannot handle Multi-Sz data, we cull the information down
                    // to one entry here.
                    //

                    RtlInitUnicodeString(&singleVetoListItem, vetoList);
                    RtlCopyUnicodeString(deviceEvent->VetoName, &singleVetoListItem);
                }

                ExFreePool(vetoList);

                status = STATUS_PLUGPLAY_QUERY_VETOED;

                goto Clean0;
            }

            //
            // Broadcast the query for a profile change against our current
            // list of docks in transition...
            //

            status = PpProfileQueryHardwareProfileChange(subsumingProfileChange,
                                                         PROFILE_IN_PNPEVENT,
                                                         internalVetoType,
                                                         internalVetoString);

            if (!NT_SUCCESS(status)) {
                IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                           "PnpProcessQueryRemoveAndEject: Vetoed due to failed HW profile change\n"));

                //
                // Release any docks in profile transition
                //

                PpProfileCancelHardwareProfileTransition();
                IopFreeRelationList(relationsList);
                PnpFinalizeVetoedRemove(deviceEvent,
                                        *internalVetoType,
                                        internalVetoString);

                status = STATUS_PLUGPLAY_QUERY_VETOED;

                goto Clean0;
            }
        }
    }

    if ((deleteType == QueryRemoveDevice) || (deleteType == EjectDevice)) {

        //
        // The basic rule of query removal is that if anything fails at any
        // point, all "listeners" who received a query remove notification/IRP
        // receives a corresponding cancel notification/IRP.
        //
        // It is possible for listeners/devices later in the list to not
        // receive any notifications/IRP if a cancellation happened in the
        // middle of the list.
        //
        // An overview of the query removal process:
        //
        // 1) Send UM query removal notifications.
        //     1a) If UM vetoes, send UM cancel notifications and exit.
        //
        // 2) Send KM query removal notifications.
        //     2a) If KM vetoes, send KM cancel notifications, UM cancel
        //         notifications and exit.
        //
        // 3) Send query remove IRPs to devices in the relation list.
        //     3a) If query remove IRP vetoes, send cancel IRPs, KM cancel
        //         notifications, and UM cancel notifications, and exit.
        //
        // 4) Check for open file handle references against the devices.
        //     4a) If references are present, follow the steps in 3a.
        //
        // 5) If all is well, start the final remove process.
        //

        //
        // Send query notification to user-mode.
        //

        status = PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                                relatedDeviceInstances,
                                                &GUID_TARGET_DEVICE_QUERY_REMOVE,
                                                internalVetoType,
                                                internalVetoString);

        if (NT_SUCCESS(status)) {

            //
            // User mode notifications have been sent and they were successful.
            // Send query notification to kernel-mode drivers.
            //

            status = PiEventQueryRemoveNotifyKernel(deviceEvent,
                                                    pdoList,
                                                    pdoListCount,
                                                    &vetoingDriver,
                                                    internalVetoType,
                                                    internalVetoString);

            //
            // If status is STATUS_SUCCESS, they have received QueryRemove.
            // If status is not STATUS_SUCCESS, QueryRemove and CancelRemove
            // notifications have been sent out to kernel mode listeners.
            //

            if (NT_SUCCESS(status)) {
                IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                             "PnpProcessQueryRemoveAndEject: QueryRemove DevNodes\n"));

                //
                // At this point, user mode and kernel mode listeners have been notified.
                // Send out actual query removal IRPs and check for open handles. 
                //

                status = PiEventQueryRemoveDevices(deleteType,
                                                   deviceEvent,
                                                   deviceObject,
                                                   relationsList,
                                                   pdoListCount,
                                                   pdoList,
                                                   internalVetoType,
                                                   internalVetoString,
                                                   &vetoingDevice);

                if (!NT_SUCCESS(status)) {

                    //
                    // Either:
                    //    * A device stack in the list failed the query remove
                    //      IRP, or
                    //    * Open file handles were referencing device(s) in
                    //      the list
                    //
                    // In either failure case, send out kernel mode
                    // notifications that the query remove is being canceled.
                    //

                    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                 "PnpProcessQueryRemoveAndEject: Vetoed due someone in the stack failed QR\n"));

                    //
                    // Send cancel notification to kernel-mode drivers.
                    //

                    PiEventRemovalCanceledNotifyKernel(pdoList, pdoListCount);
                }
            }

            if (NT_SUCCESS(status)) {
                if (deviceEvent->Argument == CM_PROB_DEVICE_RESET) {

                    //
                    // Query remove succeeded. Clean up hung devices since they
                    // were ignored during the query remove process.
                    //

                    status = PnpCancelRemoveOnHungDevices(deviceEvent,
                                                          deleteType,
                                                          excludeDeviceRelations,
                                                          relationsList,
                                                          &relatedDeviceInstances);
                }

            } else {

                //
                // Query remove failed. This could be any of:
                //
                //  * KM notification veto
                //  * Query remove IRP failed
                //  * Open handles against devices
                //

                IopDbgPrint((IOP_IOEVENT_WARNING_LEVEL,
                             "PnpProcessQueryRemoveAndEject: Vetoed by \"%wZ\" (type 0x%x)\n",
                             internalVetoString,
                             *internalVetoType));

                PnpFinalizeVetoedRemove(deviceEvent,
                                        *internalVetoType,
                                        internalVetoString);

                //
                // A driver vetoed the query remove, go back and send
                // cancels to user-mode (cancels already sent to drivers
                // that received the query).
                //

                PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                               relatedDeviceInstances,
                                               &GUID_TARGET_DEVICE_REMOVE_CANCELLED,
                                               NULL,
                                               NULL);
            }

        } else {

            //
            // User mode notifications have been sent but it was vetoed.
            //

            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                       "PnpProcessQueryRemoveAndEject: Vetoed due to UM failing QR\n"));

            PnpFinalizeVetoedRemove(deviceEvent,
                                    *internalVetoType,
                                    internalVetoString);
        }

        if (!NT_SUCCESS(status)) {

            //
            // Query remove failed. This could be any of:
            //  * UM notification veto (UM cancel notifications have been sent
            //    to listeners that received query remove notification)
            //
            //  * KM notification veto (KM cancel notifications have been sent
            //    to listeners that received query remove notification)
            //
            //  * Query remove IRP failed
            //    (cancel IRPs have been sent to devices that received
            //     query remove)
            //
            //  * Open handles against devices (cancel IRPs have been sent to
            //    all devices, along with UM and KM notifications)
            //

            //
            // Broadcast a cancel HW profile change event if appropriate.
            //

            if (possibleProfileChangeInProgress) {

                //
                // Release any docks in profile transition. We also broadcast
                // the cancel.
                //

                PpProfileCancelHardwareProfileTransition();
            }

            if (deviceEvent->Argument == CM_PROB_DEVICE_RESET) {
                IopInitEnumerationMarker(ENUMERATE_FORWARD, &marker);
                while (IopEnumerateRelations(relationsList,
                                             &marker,
                                             &relatedDeviceObject,
                                             NULL,
                                             NULL)) {

                    relatedDeviceNode = PP_DO_TO_DN(relatedDeviceObject);
                    if (relatedDeviceNode != NULL) {
                        relatedDeviceNode->StateFlags &= ~DNSF_DEVICE_HUNG_DURING_RESET;
                    }
                }
            }

            if (deviceEvent->Argument == CM_PROB_DEVICE_RESET) {

                //
                // Check if the device is marked to have open handles ignored (default value from ACPI).
                //

                resetRemovalContext = deviceEvent->Context;
                ignoreOpenHandlesonDeviceReset =
                    ((resetRemovalContext->Flags & DEVICE_RESET_IGNORE_OPEN_HANDLES) != 0) ? TRUE : FALSE;

                if (IopGetRelationsCount(relationsList) == 1) {

                    //
                    // For a QR failure on single device, allow the bus driver to
                    // reset the device. This allows forward progression on the
                    // following cases: UM veto, KM veto, and open handles.
                    //

                    //
                    // It is never the case that the query remove IRP failed in this
                    // path since hung devices are ignored for reset requests.
                    // The device node can't be in deleted state because this is the
                    // root device for the subtree query removal.
                    //

                    if ((*internalVetoType == PNP_VetoDevice) ||
                        (deviceNode->State == DeviceNodeDeleted)) {

#pragma prefast(suppress: 28159)
                        KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                                     PNP_ERR_INVALID_REMOVAL_REQUEST,
                                     (ULONG_PTR)deleteType,
                                     (ULONG_PTR)*internalVetoType,
                                     (ULONG_PTR)relationsList);
                    }

                    deviceObject->Flags |= DO_DEVICE_TO_BE_RESET;
                    IopQueryDeviceState(deviceObject, &deviceState);
                    status = STATUS_SUCCESS;

                } else if ((ignoreOpenHandlesonDeviceReset != FALSE) &&
                           (*internalVetoType == PNP_VetoOutstandingOpen)) {

                    IopInitEnumerationMarker(ENUMERATE_FORWARD, &marker);
                    while (IopEnumerateRelations(relationsList,
                                                 &marker,
                                                 &relatedDeviceObject,
                                                 &relationLevel,
                                                 NULL)) {

                        relatedDeviceNode = PP_DO_TO_DN(relatedDeviceObject);
                        if (relationLevel != RELATION_LEVEL_DEPENDENT) {
                            if (relatedDeviceNode->State != DeviceNodeDeleted) {

                                //
                                // Send 'removal complete' QDS.
                                //

                                relatedDeviceObject->Flags |= DO_DEVICE_TO_BE_RESET;
                                status = IopQueryDeviceState(relatedDeviceObject, &deviceState);
                            }
                        }
                    }

                    status = STATUS_SUCCESS;

                } else {
                    status = STATUS_PLUGPLAY_QUERY_VETOED;
                }
            }

            IopFreeRelationList(relationsList);

            //
            // Query remove failed. Exit out with an error code.
            //

            goto Clean0;
        }

    } else if ((deleteType == SurpriseRemoveDevice) ||
               (deleteType == RemoveFailedDevice)) {

        //
        // Send IRP_MN_SURPRISE_REMOVAL, IopDeleteLockDeviceNodes ignores
        // indirect descendants for SurpriseRemoveDevice.
        //

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpProcessQueryRemoveAndEject: QueryRemove DevNodes\n"));

        PnpDeleteLockedDeviceNodes(deviceObject,
                                   relationsList,
                                   SurpriseRemoveDevice,
                                   FALSE,
                                   0,
                                   STATUS_SUCCESS,
                                   NULL,
                                   NULL);
    }

    //
    // Notify user-mode and drivers that a remove is happening. User-mode
    // sees this as a remove pending if it's user initiated, we don't give
    // them the "remove" until it's actually gone.
    //

    if (deleteType != SurpriseRemoveDevice) {


        PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                       relatedDeviceInstances,
                                       &GUID_DEVICE_REMOVE_PENDING,
                                       NULL,
                                       NULL);

    } else {
        if (surpriseRemovalList) {
            PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                           surpriseRemovalList,
                                           &GUID_DEVICE_SURPRISE_REMOVAL,
                                           NULL,
                                           NULL);

            ExFreePoolWithTag(surpriseRemovalList, PNP_POOLTAG_DEVICE_EVENT_ENTRY);
            surpriseRemovalList = NULL;
        }

        PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                       relatedDeviceInstances,
                                       &GUID_TARGET_DEVICE_REMOVE_COMPLETE,
                                       NULL,
                                       NULL);
    }

    //
    // Notify kernel mode listeners that removal is complete.
    //

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessQueryRemoveAndEject: REMOVE_COMPLETE - notifying kernel-mode\n"));

    PiEventRemovalCompleteNotifyKernel(deleteType, pdoList, pdoListCount);
    if ((deleteType == RemoveDevice) ||
        (deleteType == RemoveFailedDevice) ||
        (deleteType == SurpriseRemoveDevice)) {

        //
        // For these operations indirect relations are speculative.
        //
        // So for each of the indirect relations, invalidate their parents and
        // remove them from the relations list.
        //

        PnpInvalidateRelationsInList(relationsList, deleteType, TRUE, FALSE);
        IopRemoveDeviceRelationsFromList(relationsList);
    }

    if ((deleteType == RemoveFailedDevice) ||
        (deleteType == SurpriseRemoveDevice)) {

        //
        // We've sent the surprise remove IRP to the original device and all its
        // direct descendants and dependents.  We've also notified user-mode.
        // Now, queue the relation list where it will wait until handles are
        // closed, and finally receive remove IRPs.
        //

        PiEventRemovalPostSurpriseRemove(deviceNode,
                                         deviceEvent,
                                         deleteType,
                                         &relationsList);

        //
        // Release the engine lock *before* PnpIsChainDereferenced,
        // as it may call back into us...
        //

        PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
        (VOID)PnpIsChainDereferenced(pdoList,
                                     pdoListCount,
                                     FALSE,
                                     TRUE,
                                     NULL);

        status = STATUS_SUCCESS;
        goto Clean1;
    }

    if (deviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {
        status = IopQueryDockRemovalInterface(deviceObject,
                                              &dockInterface);

        if (dockInterface != NULL) {

            //
            // Make sure updates don't occur on removes during an ejection.
            // We may change this to PDS_UPDATE_ON_EJECT *after* the remove
            // IRPs go through (as only then do we know our power
            // constraints)
            //

            dockInterface->ProfileDepartureSetMode(dockInterface->Context,
                                                   PDS_UPDATE_ON_INTERFACE);
        }
    }

    //
    // For Query Remove / Remove, the device is being remove intentionally and
    // thus can be considered 'uninstalled'.  Set the DNF_UNINSTALLED flag to
    // ensure that this devnode is not restarted on subsequent uninstalls of
    // other devices.
    //

    if (deleteType == QueryRemoveDevice) {
        PipSetDevNodeFlags(deviceNode, DNF_UNINSTALLED);
    }

    //
    // Send the remove to the devnode tree.
    //

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessQueryRemoveAndEject: RemoveDevice DevNodes\n"));

    status = PnpDeleteLockedDeviceNodes(deviceObject,
                                        relationsList,
                                        RemoveDevice,
                                        (BOOLEAN)(deleteType == QueryRemoveDevice || deleteType == EjectDevice),
                                        deviceEvent->Argument,
                                        deviceEvent->ArgumentStatus,
                                        NULL,
                                        NULL);

    hotEjectSupported =
        (BOOLEAN) IopDeviceNodeFlagsToCapabilities(deviceNode)->EjectSupported;

    warmEjectSupported =
        (BOOLEAN) IopDeviceNodeFlagsToCapabilities(deviceNode)->WarmEjectSupported;

    if (deleteType != EjectDevice) {
        PiEventRestartRemovalRelations(deviceEvent,
                                       relationsList,
                                       deviceObject);

        PnpUnlinkDeviceRemovalRelations(deviceObject,
                                        relationsList,
                                        (deviceNode->State != DeviceNodeDeletePendingCloses) ?
                                            UnlinkRemovedDeviceNodes : UnlinkAllDeviceNodesPendingClose);

        IopFreeRelationList(relationsList);

    } else if (hotEjectSupported || warmEjectSupported) {

        //
        // From this point on we cannot return any sort of failure without
        // going through IopEjectDevice or cancelling any outstanding profile
        // change. Set a flag to let kernel-mode know we'll be wanting to
        // restart these devnodes, eventually. If any ejections have been
        // subsumed, free the corresponding relations list as its no longer
        // needed.
        //

        IopInitEnumerationMarker(ENUMERATE_FORWARD, &marker);
        while (IopEnumerateRelations(relationsList,
                                     &marker,
                                     &relatedDeviceObject,
                                     NULL,
                                     NULL)) {

            relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
            if (relatedDeviceNode)  {
                PipSetDevNodeFlags(relatedDeviceNode, DNF_LOCKED_FOR_EJECT);
                pendingRelations = relatedDeviceNode->PendingEjectRelations;
                if (pendingRelations != NULL){

                    //
                    // The pending eject relations pointer within the devnode
                    // is only set to NULL if the eject has already been
                    // subsumed or if it has completed.  In both cases,
                    // the RelationsList in the eject entry should have been
                    // set to NULL;
                    //

                    PNP_ASSERT(pendingRelations->RelationsList != NULL);

                    IopFreeRelationList(pendingRelations->RelationsList);
                    pendingRelations->RelationsList = NULL;

                    //
                    // The eject relations list entry will be freed
                    // when the eject irp completes.  The list entry is
                    // being kept track of in the global IopPendingEjects list.
                    // As such it is safe to set this pointer to NULL here.
                    //

                    relatedDeviceNode->PendingEjectRelations = NULL;
                }
            }
        }

        PnpUnlinkDeviceRemovalRelations(deviceObject,
                                        relationsList,
                                        UnlinkRemovedDeviceNodes);

        //
        // Send the eject.
        //

        pendingRelations = (PPENDING_RELATIONS_LIST_ENTRY) ExAllocatePool(
            NonPagedPoolNx,
            sizeof(PENDING_RELATIONS_LIST_ENTRY));

        if (pendingRelations == NULL) {

            //
            // It's cleanup time. Free up everything that matters
            //

            if (dockInterface != NULL) {
                dockInterface->ProfileDepartureSetMode(dockInterface->Context,
                                                       PDS_UPDATE_DEFAULT);

                dockInterface->InterfaceDereference(dockInterface->Context);
            }

            if (possibleProfileChangeInProgress) {

                //
                // Release any docks in profile transition. We also broadcast
                // the cancel.
                //

                PpProfileCancelHardwareProfileTransition();
            }

            //
            // This will bring back online the devices that were held offline
            // for the duration of the undock.
            //

            PnpInvalidateRelationsInList(relationsList, deleteType, FALSE, TRUE);

            //
            // Free the relations list
            //

            IopFreeRelationList(relationsList);

            //
            // Let the user know we were unable to process the request.
            //

            PnpFinalizeVetoedRemove(deviceEvent,
                                    PNP_VetoTypeUnknown,
                                    NULL);
            status = STATUS_PLUGPLAY_QUERY_VETOED;

            goto Clean0;
        }

        //
        // Fill out the pending eject information.
        //

        ObReferenceObject(deviceObject);
        pendingRelations->DeviceEvent = deviceEvent;
        pendingRelations->DeviceObject = deviceObject;
        pendingRelations->RelationsList = relationsList;
        pendingRelations->ProfileChangingEject = possibleProfileChangeInProgress;
        pendingRelations->DisplaySafeRemovalDialog =
            (BOOLEAN)((deviceEvent->Data.Flags & TDF_NOTIFY_VETO) != 0);

        pendingRelations->DockInterface = dockInterface;
        deviceNode->PendingEjectRelations = pendingRelations;

        //
        // Now that we've removed all the devices that won't be present
        // in the new hardware profile state (eg batteries, etc),
        //

        status = PoGetLightestSystemStateForEject(possibleProfileChangeInProgress,
                                                  hotEjectSupported,
                                                  warmEjectSupported,
                                                  &pendingRelations->LightestSleepState);

        if (!NT_SUCCESS(status)) {
            if (status == STATUS_INSUFFICIENT_POWER) {
                PnpFinalizeVetoedRemove(deviceEvent,
                                        PNP_VetoInsufficientPower,
                                        NULL);

            } else {
                IopDbgPrint((IOP_IOEVENT_WARNING_LEVEL,
                             "PnpProcessQueryRemoveAndEject: Vetoed by power system (%x)\n",
                             status));

                PnpFinalizeVetoedRemove(deviceEvent,
                                        PNP_VetoTypeUnknown,
                                        NULL);
            }

            //
            // We'll complete this one ourselves thank you.
            //

            pendingRelations->DeviceEvent = NULL;
            pendingRelations->DisplaySafeRemovalDialog = FALSE;

            //
            // Release any profile transitions.
            //

            InitializeListHead( &pendingRelations->Link );
            PnpProcessCompletedEject((PVOID)pendingRelations);

            status = STATUS_PLUGPLAY_QUERY_VETOED;

            goto Clean0;
        }

        PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

        //
        // Completion routine for the eject IRP handles display of the
        // safe removal dialog and completion of the event. Returning
        // STATUS_PENDING does let other events get processed though.
        //

        IopEjectDevice(deviceObject, pendingRelations);
        status = STATUS_PENDING;

        goto Clean1;

    } else {

        //
        // All docks must be hot or warm ejectable.
        //

        PNP_ASSERT(!dockInterface);

        //
        // Unlock the device relations list.
        //

        PnpUnlinkDeviceRemovalRelations(deviceObject,
                                        relationsList,
                                        UnlinkRemovedDeviceNodes);

        IopFreeRelationList(relationsList);

    }

    if (deleteType == RemoveDevice) {

        //
        // Notify user-mode one last time that everything is actually done.
        //

        PnpNotifyUserModeDeviceRemoval(deviceEvent,
                                       relatedDeviceInstances,
                                       &GUID_TARGET_DEVICE_REMOVE_COMPLETE,
                                       NULL,
                                       NULL);
    }

    //
    // Queue query-removed device to be restarted if it is being reinstalled.
    //

    if ((deleteType == QueryRemoveDevice) &&
        (!(deviceEvent->Data.Flags & TDF_NO_RESTART)) &&
        (PipIsDevNodeProblem(deviceNode, CM_PROB_REINSTALL))) {

        PipClearDevNodeProblem(deviceNode);
        PnpRestartDeviceNode(deviceNode);

        PnpRequestDeviceAction(deviceNode->PhysicalDeviceObject,
                               StartDevice,
                               TRUE,
                               0,
                               NULL,
                               NULL);
    }

    if (dockInterface != NULL) {
        dockInterface->ProfileDepartureSetMode(dockInterface->Context,
                                               PDS_UPDATE_DEFAULT);

        dockInterface->InterfaceDereference(dockInterface->Context);
    }

    status = STATUS_SUCCESS;

Clean0:
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

Clean1:

    if (internalVetoBase != NULL &&
        PnpIsShutDown() == FALSE) {
        ExFreePool(internalVetoBase);
    }

    if (relatedDeviceInstances != NULL) {
        ExFreePool(relatedDeviceInstances);
    }

    if (pdoList != NULL) {
        ExFreePool(pdoList);
    }

    return status;
}


NTSTATUS
PnpProcessTargetDeviceEvent(
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    )

/*++

Routine Description:

    This routine processes each type of event in the target device category.
    These events may have been initiated by either user-mode or kernel mode.

Arguments:

    deviceEvent - Data describing the type of target device event and the
            target device itself.


Return Value:

    None.

--*/

{
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    HPNPOPCTX hPnpOpCtx = NULL;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    deviceEvent = *DeviceEvent;
    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessTargetDeviceEvent: Entered\n"));

    deviceObject = (PDEVICE_OBJECT)deviceEvent->Data.DeviceObject;
    deviceNode = PP_DO_TO_DN(deviceObject);

    //
    // If creating the operation fails, continue anyway.  The object changes
    // just won't be batched.
    //

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    //-----------------------------------------------------------------
    // QUERY and REMOVE
    //-----------------------------------------------------------------

    if (PnpCompareGuid(&deviceEvent->Data.EventGuid,
                      &GUID_DEVICE_QUERY_AND_REMOVE)) {

        if (EventEnabledKMPnPEvt_DeviceRemoval_Start()) {
            EventWriteKMPnPEvt_DeviceRemoval_Start(NULL,
                                                   PnpGetDeviceInstancePath(deviceNode)->Buffer);
        }

        status = PnpProcessQueryRemoveAndEject(DeviceEvent);

        if (EventEnabledKMPnPEvt_DeviceRemoval_Stop()) {
            EventWriteKMPnPEvt_DeviceRemoval_Stop(NULL,
                                                  PnpGetDeviceInstancePath(deviceNode)->Buffer);
        }

    }

    //-----------------------------------------------------------------
    // EJECT
    //-----------------------------------------------------------------

    else if (PnpCompareGuid(&deviceEvent->Data.EventGuid,
                           &GUID_DEVICE_EJECT)) {

        if (EventEnabledKMPnPEvt_DeviceRemoval_Start()) {
            EventWriteKMPnPEvt_DeviceRemoval_Start(NULL,
                                                   PnpGetDeviceInstancePath(deviceNode)->Buffer);
        }

        status = PnpProcessQueryRemoveAndEject(DeviceEvent);

        if (EventEnabledKMPnPEvt_DeviceRemoval_Stop()) {
            EventWriteKMPnPEvt_DeviceRemoval_Stop(NULL,
                                                  PnpGetDeviceInstancePath(deviceNode)->Buffer);
        }

    }

    //-----------------------------------------------------------------
    // ARRIVAL
    //-----------------------------------------------------------------

    else if (PnpCompareGuid(&deviceEvent->Data.EventGuid,
                           &GUID_DEVICE_ARRIVAL)) {

        //
        // Notify user-mode (not drivers) that an arrival just happened.
        //

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "PnpProcessTargetDeviceEvent: ARRIVAL - notifying user-mode\n"));

        PiUEventNotifyUserMode(deviceEvent);
    }

    //-----------------------------------------------------------------
    // NO-OP REQUEST (to flush device event queue)
    //-----------------------------------------------------------------

    else if (PnpCompareGuid(&deviceEvent->Data.EventGuid,
                           &GUID_DEVICE_NOOP)) {

        status = STATUS_SUCCESS;

    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

NTSTATUS
PnpProcessCustomDeviceEvent(
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    )

/*++

Routine Description:

    This routine processes each type of event in the custom device category.
    These events may have been initiated by either user-mode or kernel mode.

Arguments:

    deviceEvent - Data describing the type of custom device event and the
            target device itself.


Return Value:

    None.

--*/

{
    PTARGET_DEVICE_CUSTOM_NOTIFICATION  customNotification;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_OBJECT deviceObject;

    PAGED_CODE();

    deviceEvent = *DeviceEvent;

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessCustomDeviceEvent: Entered\n"));

    PNP_ASSERT(PnpCompareGuid(&deviceEvent->Data.EventGuid,
                         &GUID_PNP_CUSTOM_NOTIFICATION));

    deviceObject = (PDEVICE_OBJECT)deviceEvent->Data.DeviceObject;
    customNotification = (PTARGET_DEVICE_CUSTOM_NOTIFICATION)deviceEvent->Data.u.CustomNotification.NotificationStructure;

    //
    // Notify device container support that something just happened.
    //

    PiDcHandleCustomDeviceEvent(deviceEvent);

    //
    // Notify user-mode that something just happened.
    //

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessCustomDeviceEvent: CUSTOM_NOTIFICATION - notifying user-mode\n"));

    PiUEventNotifyUserMode(deviceEvent);

    //
    // Notify K-mode
    //

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PnpProcessCustomDeviceEvent: CUSTOM_NOTIFICATION - notifying kernel-mode\n"));

    PnpNotifyTargetDeviceChange(&customNotification->Event,
                                deviceObject,
                                customNotification,
                                NULL);

    return STATUS_SUCCESS;
}

NTSTATUS
PnpResizeTargetDeviceBlock(
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent,
    __in PNP_DEVICE_DELETE_TYPE DeleteType
    )

/*++

Routine Description:

    This routine takes the passed in device event block and resizes it to
    hold the largest possible device instance id in the DeviceId field.

    This list of related device instance ids is returned in the
    RelatedDeviceInstances parameter.

Arguments:

    DeviceEvent - On entry, contains the original device event block, on
            return it contains the newly allocated device event block and
            a complete list of related device id strings.

    DeviceRelations - structure that contains a list of related device objects.

    RelateDeviceInstances - On return, contains a multi-sz list of device
            instance ids of device objects in the relations list structure.

Return Value:

    NTSTATUS value.

--*/

{

    ULONG currentSize;
    ULONG newSize;
    PPNP_DEVICE_EVENT_ENTRY newDeviceEvent = NULL;
    LPWSTR targetDevice;
    ULONG targetDeviceInstancePathLength;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    targetDeviceInstancePathLength = 0;
    targetDevice = (*DeviceEvent)->Data.u.TargetDevice.DeviceId;

    //
    // Calculate the size of the PNP_DEVICE_EVENT_ENTRY block
    //

    currentSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) +
                  (*DeviceEvent)->Data.TotalSize;

    newSize = currentSize;

    //
    // newSize is the total size including the length of the device instance path
    // and thus should always be greater.
    //

    targetDeviceInstancePathLength = (ULONG)((wcslen(targetDevice)+1) *
                                     sizeof(WCHAR));


    //
    // Don't add space for NULL characters if no instance path exists.
    //

    if (targetDeviceInstancePathLength == sizeof(WCHAR)) {
        targetDeviceInstancePathLength = 0;
    }

    PNP_ASSERT(newSize > targetDeviceInstancePathLength);
    __analysis_assume(newSize > targetDeviceInstancePathLength);
    newSize -= targetDeviceInstancePathLength;
    newSize += CWC_TO_CB(MAX_DEVICE_ID_LEN);

    PNP_ASSERT(newSize >= currentSize);

    //
    // Allocate space for the new device event entry which can hold the largest
    // possible device instance id
    //

    if (newSize > currentSize) {
        newDeviceEvent = (PPNP_DEVICE_EVENT_ENTRY) PnpAllocateCriticalMemory(
                             DeleteType,
                             PagedPool,
                             newSize,
                             PNP_POOLTAG_DEVICE_EVENT_ENTRY);

        if (newDeviceEvent == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Clean0;
        }

        //
        // Copy the old buffer into the new buffer, it's only the new stuff at
        // the end that changes.
        //

        RtlZeroMemory(newDeviceEvent, newSize);
        RtlCopyMemory(newDeviceEvent, *DeviceEvent, currentSize);
        newDeviceEvent->RefCount = 1;

        //
        // Update the size of the PLUGPLAY_EVENT_BLOCK
        //

        newDeviceEvent->Data.TotalSize =
            (newSize - FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data));

        newDeviceEvent->Parent = *DeviceEvent;
    }

    if (newDeviceEvent != NULL) {
        *DeviceEvent = newDeviceEvent;
    }

Clean0:

    if (!NT_SUCCESS(status)) {
        if (newDeviceEvent) {
            PNP_ASSERT(*DeviceEvent != newDeviceEvent);
            ExFreePoolWithTag(newDeviceEvent, PNP_POOLTAG_DEVICE_EVENT_ENTRY);
        }
    }

    return status;
}

NTSTATUS
PnpCompileDeviceInstancePaths (
    __in PNP_DEVICE_DELETE_TYPE DeleteType,
    __in PRELATION_LIST RelationsList,
    __in BOOLEAN ExcludeDeviceRelations,
    __in BOOLEAN ExcludeHungDevices,
    __out PWSTR *RelatedDeviceInstances
    )

/*++

Routine Description:

    This routine takes the passed in device event block and resizes it to
    hold the largest possible device instance id in the DeviceId field.

    This list of related device instance ids is returned in the
    RelatedDeviceInstances parameter.

Arguments:

    DeviceEvent - On entry, contains the original device event block, on
            return it contains the newly allocated device event block and
            a complete list of related device id strings.

    DeviceRelations - structure that contains a list of related device objects.

    RelateDeviceInstances - On return, contains a multi-sz list of device
            instance ids of device objects in the relations list structure.

Return Value:

    NTSTATUS value.

--*/

{

    RELATION_LIST_MARKER marker;
    ULONG listSize;
    LPWSTR p;
    PDEVICE_NODE relatedDeviceNode;
    PDEVICE_OBJECT relatedDeviceObject;
    DEVICE_RELATION_LEVEL relationLevel;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    if (RelationsList == NULL) {
        RtlFailFast(FAST_FAIL_INVALID_ARG);
    }

    *RelatedDeviceInstances = NULL;

    //
    // Calculate the size of the list that will hold the multi-sz list of
    // related device ids
    //

    listSize = 0;
    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &marker);
    while (IopEnumerateRelations(RelationsList,
                                 &marker,
                                 &relatedDeviceObject,
                                 &relationLevel,
                                 NULL)) {

        relatedDeviceNode = PP_DO_TO_DN(relatedDeviceObject);
        if (relatedDeviceNode == NULL) {
            continue;
        }

        if ((ExcludeHungDevices != FALSE) &&
            ((relatedDeviceNode->StateFlags &
              DNSF_DEVICE_HUNG_DURING_RESET) != 0)) {

            continue;
        }

        if (PnpIsDeviceInstancePathEmpty(relatedDeviceNode) != FALSE) {
            continue;
        }

        if (!ExcludeDeviceRelations || relationLevel != RELATION_LEVEL_REMOVE_EJECT) {
            listSize += PnpGetDeviceInstancePath(relatedDeviceNode)->Length + sizeof(WCHAR);
        }
    }

    //
    // Add space for the multi-sz terminator
    //

    listSize += sizeof(WCHAR);

    //
    // Allocate space for the multi-sz list of related device instance ids
    //

    *RelatedDeviceInstances = (PWSTR)PnpAllocateCriticalMemory(
                                        DeleteType,
                                        PagedPool,
                                        listSize,
                                        PNP_POOLTAG_DEVICE_EVENT_ENTRY);

    if (*RelatedDeviceInstances == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean0;
    }

    //
    // Create the list of related device instance ids. The target device id is also
    // contained in this list.
    //

    IopInitEnumerationMarker(ENUMERATE_BACKWARD, &marker);
    p = *RelatedDeviceInstances;
    while (IopEnumerateRelations(RelationsList,
                                 &marker,
                                 &relatedDeviceObject,
                                 &relationLevel,
                                 NULL)) {

        relatedDeviceNode = PP_DO_TO_DN(relatedDeviceObject);
        if (relatedDeviceNode == NULL) {
            continue;
        }

        if ((ExcludeHungDevices != FALSE) &&
            ((relatedDeviceNode->StateFlags &
              DNSF_DEVICE_HUNG_DURING_RESET) != 0)) {

            continue;
        }

        if (PnpIsDeviceInstancePathEmpty(relatedDeviceNode) != FALSE) {
            continue;
        }

        if (!ExcludeDeviceRelations || relationLevel != RELATION_LEVEL_REMOVE_EJECT) {
            relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
            if (relatedDeviceNode != NULL) {
                if (PnpIsDeviceInstancePathEmpty(relatedDeviceNode) == FALSE) {
                    RtlCopyMemory(p,
                                  PnpGetDeviceInstancePath(relatedDeviceNode)->Buffer,
                                  PnpGetDeviceInstancePath(relatedDeviceNode)->Length);

                    p += PnpGetDeviceInstancePath(relatedDeviceNode)->Length /
                                                  sizeof(WCHAR);

                    *p = UNICODE_NULL;
                    p += 1;
                }
            }
        }
    }

    //
    // The buffer must have a double null termination.
    //

    *p = UNICODE_NULL;

Clean0:

    if (!NT_SUCCESS(status)) {
        if (*RelatedDeviceInstances != NULL) {
            ExFreePoolWithTag(*RelatedDeviceInstances, PNP_POOLTAG_DEVICE_EVENT_ENTRY);
        }
    }

    return status;
}

VOID
PnpBuildUnsafeRemovalDeviceList(
    __in  PPNP_DEVICE_EVENT_ENTRY     OriginalDeviceEvent,
    __in  PRELATION_LIST              RelationsList,
    __out PWSTR                       *UnsafeRemovalList
    )
/*++

Routine Description:

    This routine builds a multi-sz list of related device instance ids in case
    of unsafe removal.THIS DOES NOT ADD ANY REFERENCE TO THE EMBEDDED DEVICE
    OBJECT.

Arguments:

    OriginalDeviceEvent - Contains the original device event block.

    RelationList - structure that contains a list of related device objects.

    UnsafeRemovalList - List of related device instance ids

Return Value:

    None.

--*/
{
    ULONG listSize;
    RELATION_LIST_MARKER marker;
    LPWSTR p;
    PDEVICE_NODE relatedDeviceNode;
    PDEVICE_OBJECT relatedDeviceObject;
    DEVICE_RELATION_LEVEL relationLevel;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(OriginalDeviceEvent);

    //
    // Preinit.
    //

    *UnsafeRemovalList = NULL;

    if (RelationsList == NULL) {
        return;
    }

    //
    // Calculate the size of the multi-sz list
    //

    listSize = 0;
    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &marker);
    while (IopEnumerateRelations(RelationsList,
                                 &marker,
                                 &relatedDeviceObject,
                                 &relationLevel,
                                 NULL)) {

        if (relationLevel == RELATION_LEVEL_REMOVE_EJECT) {
            continue;
        }

        relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
        if (PnpIsBeingRemovedSafely(relatedDeviceNode, relationLevel) != FALSE) {
            continue;
        }

        if (PnpIsDeviceInstancePathEmpty(relatedDeviceNode) == FALSE) {
            listSize += PnpGetDeviceInstancePath(relatedDeviceNode)->Length + sizeof(WCHAR);
        }
    }

    if (listSize == 0) {

        //
        // No entries, bail.
        //

        return;
    }

    //
    // Add space for a multi-sz terminator
    //

    listSize += sizeof(WCHAR);

    //
    // If we can't get memory, there simply won't be a message sent.
    //

    *UnsafeRemovalList = ExAllocatePoolWithTag(PagedPool,
                                               listSize,
                                               PNP_POOLTAG_DEVICE_EVENT_ENTRY);

    if (*UnsafeRemovalList == NULL) {
        return;
    }

    //
    // Zero out the memory to make sure we NULL terminate each string
    //

    RtlZeroMemory(*UnsafeRemovalList, listSize);

    //
    // Add device instance string for each device relation to the list.
    //

    IopInitEnumerationMarker(ENUMERATE_BACKWARD, &marker);
    p = *UnsafeRemovalList;
    while (IopEnumerateRelations(RelationsList,
                                 &marker,
                                 &relatedDeviceObject,
                                 &relationLevel,
                                 NULL)) {

        if (relationLevel == RELATION_LEVEL_REMOVE_EJECT) {
            continue;
        }

        relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
        if (PnpIsBeingRemovedSafely(relatedDeviceNode, relationLevel) != FALSE) {
            continue;
        }

        if (PnpIsDeviceInstancePathEmpty(relatedDeviceNode) == FALSE) {
            RtlCopyMemory(p,
                          PnpGetDeviceInstancePath(relatedDeviceNode)->Buffer,
                          PnpGetDeviceInstancePath(relatedDeviceNode)->Length);

            p += PnpGetDeviceInstancePath(relatedDeviceNode)->Length / sizeof(WCHAR) + 1;
        }
    }

    *p = UNICODE_NULL;

    return;

} // PnpBuildUnsafeRemovalDeviceList

VOID
PnpFinalizeVetoedRemove(
    __in PPNP_DEVICE_EVENT_ENTRY  VetoedDeviceEvent,
    __in PNP_VETO_TYPE            VetoType,
    __in_opt PUNICODE_STRING          VetoName
    )
/*++

Routine Description:

    This routine takes care of updating the event results with the veto
    information, puts up UI if neccessary, and dumps failure information to
    the debugger for debugging purposes.

Arguments:

    VetoedDeviceEvent - Data describing the device event failed.

    VetoType - The veto code best describing why the operation failed.

    VetoName - A unicode string appropriate to the veto code that describes
               the vetoer.

Return Value:

    None.

--*/
{
    PWCHAR ch;
    ULONG count;
    PDEVICE_OBJECT deviceObject;
    PWCHAR end;

#if DBG

    PUNICODE_STRING devNodeName;
    const char *failureReason;

#endif

    deviceObject = (PDEVICE_OBJECT) VetoedDeviceEvent->Data.DeviceObject;

#if DBG

    devNodeName = PnpGetDeviceInstancePath((PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode);
    switch(VetoType) {

        case PNP_VetoTypeUnknown:
            failureReason = "for unspecified reason";
            break;

        case PNP_VetoLegacyDevice:
            failureReason = "due to legacy device";
            break;

        case PNP_VetoPendingClose:

            //
            // ADRIAO N.B. 07/10/2000 - I believe this case is vestigal...
            //
            PNP_ASSERT(0);
            failureReason = "due to pending close";
            break;

        case PNP_VetoWindowsApp:
            failureReason = "due to windows application";
            break;

        case PNP_VetoWindowsService:
            failureReason = "due to service";
            break;

        case PNP_VetoOutstandingOpen:
            failureReason = "due to outstanding handles on device";
            break;

        case PNP_VetoDevice:
            failureReason = "by device";
            break;

        case PNP_VetoDriver:
            failureReason = "by driver";
            break;

        case PNP_VetoIllegalDeviceRequest:
            failureReason = "as the request was invalid for the device";
            break;

        case PNP_VetoInsufficientPower:
            failureReason = "because there would be insufficient system power to continue";
            break;

        case PNP_VetoNonDisableable:
            failureReason = "due to non-disableable device";
            break;

        case PNP_VetoLegacyDriver:
            failureReason = "due to legacy driver";
            break;

        case PNP_VetoInsufficientRights:
            failureReason = "insufficient permissions";
            break;

        default:
            PNP_ASSERT(0);
            failureReason = "due to uncoded reason";
            break;
    }

    if (VetoName != NULL) {
        IopDbgPrint((IOP_IOEVENT_WARNING_LEVEL,
                     "PnpFinalizeVetoedRemove: Removal of %wZ vetoed %s %wZ.\n",
                     devNodeName,
                     failureReason,
                     VetoName));

    } else {
        IopDbgPrint((IOP_IOEVENT_WARNING_LEVEL,
                     "PnpFinalizeVetoedRemove: Removal of %wZ vetoed %s.\n",
                     devNodeName,
                     failureReason));
    }

#endif

    //
    // Update the vetoType field if the caller is interested.
    //

    if (VetoedDeviceEvent->VetoType != NULL) {
        *VetoedDeviceEvent->VetoType = VetoType;
    }

    //
    // Return veto information to the caller if requested.
    //

    if (VetoedDeviceEvent->VetoName != NULL) {
        if (VetoName != NULL) {
            RtlCopyUnicodeString(VetoedDeviceEvent->VetoName, VetoName);
        }
    }

    //
    // Caller wants the OS to display veto UI.
    //

    if (VetoedDeviceEvent->Data.Flags & TDF_NOTIFY_VETO) {
        if (VetoType == PNP_VetoDevice) {

            //
            // Strip out the extra diagnostic veto information if present.
            // Format of extra information is:
            // <Bus>\<Device>\<Instance>\Driver\<service name> OR
            // <Bus>\<Device>\<Instance>\FileSystem\<service name>
            //

            count = 0;
            ch = VetoName->Buffer;
            end = (PWCHAR)((PUCHAR)ch + VetoName->Length);
            while (ch < end) {
                if (*ch == OBJ_NAME_PATH_SEPARATOR) {
                    if (++count == 3) {
                        break;
                    }
                }

                ch++;
            }

            PNP_ASSERT(count>= 2);
            if (count == 3) {
                VetoName->Length = (USHORT) ((PUCHAR)ch - (PUCHAR)VetoName->Buffer);
            }
        }

    }

    return;
}

VOID
PnpFreeVetoInformation (
    __in PLIST_ENTRY VetoProcessListHead
    )
/*++

Routine Description:

    This routine frees the list of vetoing processes and
    device objects.

Arguments:

    VetoProcessListHead - Supplies a pointer to the head of the list.

Return Value:

    None

--*/
{
    PLIST_ENTRY currentDevObjLink;
    PPNP_VETO_DEVICE_OBJECT_INFO currentDevObjNode;
    PLIST_ENTRY currentVetoProcessLink;
    PPNP_VETO_PROCESS_INFO currentVetoProcessNode;

    PAGED_CODE();

    while (!IsListEmpty(VetoProcessListHead)) {

        currentVetoProcessLink = RemoveHeadList(VetoProcessListHead);
        currentVetoProcessNode = CONTAINING_RECORD(currentVetoProcessLink,
                                                   PNP_VETO_PROCESS_INFO,
                                                   ListEntry);

        while (!IsListEmpty(&currentVetoProcessNode->DevObjListHead)) {

            currentDevObjLink = RemoveHeadList(&currentVetoProcessNode->DevObjListHead);
            currentDevObjNode = CONTAINING_RECORD(currentDevObjLink,
                                                  PNP_VETO_DEVICE_OBJECT_INFO,
                                                  ListEntry);

            PNP_ASSERT(currentDevObjNode->DeviceObject != NULL);
            ObDereferenceObject(currentDevObjNode->DeviceObject);
            ExFreePoolWithTag(currentDevObjNode, PNP_POOLTAG_VETO_DEVICE_OBJECT);
        }

        ExFreePoolWithTag(currentVetoProcessNode, PNP_POOLTAG_VETO_PROCESS);
    }

    return;
}

BOOLEAN
PnpCompareGuid(
    __in CONST GUID *Guid1,
    __in CONST GUID *Guid2
    )
/*++

Routine Description:

    This routine compares two guids.

Arguments:

    Guid1 - First guid to compare

    Guid2 - Second guid to compare

Return Value:

    Returns TRUE if the guids are equal and FALSE if they're different.

--*/
{
    PAGED_CODE();

    if (RtlCompareMemory((PVOID)Guid1, (PVOID)Guid2, sizeof(GUID)) == sizeof(GUID)) {
        return TRUE;
    }

    return FALSE;

}

PVOID
PnpAllocateCriticalMemory (
    __in  PNP_DEVICE_DELETE_TYPE    DeleteType,
    __in  POOL_TYPE                 PoolType,
    __in  SIZE_T                    Size,
    __in  ULONG                     Tag
    )
/*++

Routine Description:

    This function allocates memory and never fails if the DeleteType isn't
    QueryRemoveDevice or EjectDevice. This function will disappear in the next
    version of the PnP engine as we will instead requeue failed operations
    (which will also result in a second attempt to allocate the memory) or
    preallocate the required memory when bringing new devnode's into the world.

Arguments:

    DeleteType - Operation (EjectDevice, SurpriseRemoveDevice, ...)

    PoolType - PagedPool, NonPagedPool etc.

    Size - Size

    Tag - Allocation tag

Return Value:

    Allocation, NULL due to insufficient resources.

--*/
{
    PVOID memory;
    LARGE_INTEGER timeOut;

    PAGED_CODE();

    //
    // Retries only have a hope of succeeding if we are at PASSIVE_LEVEL
    //

    PNP_ASSERT(KeGetCurrentIrql() != DISPATCH_LEVEL);

    while(1) {
        memory = ExAllocatePoolWithTag(PoolType, Size, Tag);
        if ((memory != NULL) ||
            (DeleteType == QueryRemoveDevice) ||
            (DeleteType == EjectDevice)) {

            //
            // Either we got memory or the op was failable. Get out of here.
            //

            break;
        }

        //
        // We're stuck until more memory comes along. Let some other
        // threads run before we get another shot...
        //

        timeOut.QuadPart = Int32x32To64(1, -10000);
        KeDelayExecutionThread(KernelMode, FALSE, &timeOut);
    }

    return memory;
}

const struct {
    CONST GUID *Guid;
    PCHAR   Name;
} EventGuidTable[] = {

    { &GUID_HWPROFILE_QUERY_CHANGE,         "GUID_HWPROFILE_QUERY_CHANGE" },
    { &GUID_HWPROFILE_CHANGE_CANCELLED,     "GUID_HWPROFILE_CHANGE_CANCELLED" },
    { &GUID_HWPROFILE_CHANGE_COMPLETE,      "GUID_HWPROFILE_CHANGE_COMPLETE" },
    { &GUID_DEVICE_INTERFACE_ARRIVAL,       "GUID_DEVICE_INTERFACE_ARRIVAL" },
    { &GUID_DEVICE_INTERFACE_REMOVAL,       "GUID_DEVICE_INTERFACE_REMOVAL" },
    { &GUID_TARGET_DEVICE_QUERY_REMOVE,     "GUID_TARGET_DEVICE_QUERY_REMOVE" },
    { &GUID_TARGET_DEVICE_REMOVE_CANCELLED, "GUID_TARGET_DEVICE_REMOVE_CANCELLED" },
    { &GUID_TARGET_DEVICE_REMOVE_COMPLETE,  "GUID_TARGET_DEVICE_REMOVE_COMPLETE" },
    { &GUID_PNP_CUSTOM_NOTIFICATION,        "GUID_PNP_CUSTOM_NOTIFICATION" },
    { &GUID_DEVICE_ARRIVAL,                 "GUID_DEVICE_ARRIVAL" },
    { &GUID_DEVICE_ENUMERATED,              "GUID_DEVICE_ENUMERATED" },
    { &GUID_DEVICE_ENUMERATE_REQUEST,       "GUID_DEVICE_ENUMERATE_REQUEST" },
    { &GUID_DEVICE_START_REQUEST,           "GUID_DEVICE_START_REQUEST" },
    { &GUID_DEVICE_REMOVE_PENDING,          "GUID_DEVICE_REMOVE_PENDING" },
    { &GUID_DEVICE_QUERY_AND_REMOVE,        "GUID_DEVICE_QUERY_AND_REMOVE" },
    { &GUID_DEVICE_EJECT,                   "GUID_DEVICE_EJECT" },
    { &GUID_DEVICE_NOOP,                    "GUID_DEVICE_NOOP" },
    { &GUID_DEVICE_SURPRISE_REMOVAL,        "GUID_DEVICE_SURPRISE_REMOVAL" },
    { &GUID_DEVICE_SAFE_REMOVAL,            "GUID_DEVICE_SAFE_REMOVAL" },
    { &GUID_DEVICE_EJECT_VETOED,            "GUID_DEVICE_EJECT_VETOED" },
    { &GUID_DEVICE_REMOVAL_VETOED,          "GUID_DEVICE_REMOVAL_VETOED" },
};

#define EVENT_GUID_TABLE_SIZE   ARRAYSIZE(EventGuidTable)

VOID
PnpLogVetoInformation (
    __in PUNICODE_STRING DeviceInstance,
    __in PLIST_ENTRY VetoProcessListHead
)
/*++

Routine Description:

    This routine logs the list of vetoing processes to the
    event log.

Arguments:

    DeviceInstance - Supplies a pointer to the device instance path for the
        vetoed device.

    VetoProcessListHead - Supplies a pointer to the head of the list of processes that
        vetoed the removal of the device.

Return Value:

    None

--*/
{
    PPNP_VETO_DEVICE_OBJECT_INFO currentDevObjNode;
    PEPROCESS currentProcess;
    PPNP_VETO_PROCESS_INFO currentVetoProcessNode;
    PDEVICE_NODE deviceNodeWithHandle;
    BOOLEAN locallyAllocated;
    UNICODE_STRING localProcessName;
    PUNICODE_STRING processName;
    NTSTATUS status;

    PAGED_CODE();

    currentVetoProcessNode = NULL;
    currentProcess = NULL;
    PNP_FOR_EACH_IN_LIST(PNP_VETO_PROCESS_INFO,
                        ListEntry,
                        VetoProcessListHead,
                        currentVetoProcessNode) {

        locallyAllocated = FALSE;
        status = PsLookupProcessByProcessId(UlongToHandle(currentVetoProcessNode->Pid),
                                           &currentProcess);

        if (!NT_SUCCESS(status)) {
            IopDbgPrint((IOP_IOEVENT_ERROR_LEVEL,
                "PnpLogVetoInformation: Could not find process for pid %d\n",
                currentVetoProcessNode->Pid));

            continue;
        }

        PNP_ASSERT(currentProcess != NULL);

        //
        // Log the veto event.
        //

        status = PsGetAllocatedFullProcessImageName(currentProcess, &processName);
        if (!NT_SUCCESS(status)) {
            processName = NULL;

        } else if (processName->Length == 0) {

            ANSI_STRING ansiProcessName;

            //
            // Full path application name not available.  Default to the
            // 8.3 name.
            //

            locallyAllocated = TRUE;
            ExFreePool(processName);
            processName = &localProcessName;
            RtlInitAnsiString(&ansiProcessName, (PCSZ)PsGetProcessImageFileName(currentProcess));
            RtlAnsiStringToUnicodeString(processName, &ansiProcessName, TRUE);
        }

        PnpDiagnosticTraceAppVeto(&KMPnPEvt_DeviceEject_Pend,
                                  HandleToULong(currentProcess->UniqueProcessId),
                                  processName,
                                  DeviceInstance);

        //
        // Log the occurance. 
        //
        // Note that we log both the process, the device upon
        // which the process has a handle open causing the veto, and
        // finally the device that we wish to eject.
        //

        PNP_FOR_EACH_IN_LIST(PNP_VETO_DEVICE_OBJECT_INFO,
                             ListEntry,
                             &currentVetoProcessNode->DevObjListHead,
                             currentDevObjNode) {

            PNP_ASSERT(IS_PDO(currentDevObjNode->DeviceObject) != FALSE);
            deviceNodeWithHandle = PP_DO_TO_DN(currentDevObjNode->DeviceObject);
            PnpTraceDeviceRemoveProcessVeto(DeviceInstance,
                                            PnpGetDeviceInstancePath(deviceNodeWithHandle),
                                            processName);
        }

        if (locallyAllocated != FALSE) {
            PNP_ASSERT(processName == &localProcessName);
            RtlFreeUnicodeString(processName);

        } else if (processName != NULL) {
            ExFreePool(processName);
        }

        //
        // Dereference the process object
        //

        PNP_ASSERT(currentProcess != NULL);
        ObDereferenceObjectWithTag(currentProcess, REFTAG_DEFAULT);
    }

    return;
}

VOID
PnpLookupGuid (
    __in CONST GUID *Guid,
    __inout_bcount(StringLength) PCHAR String,
    __in ULONG StringLength
    )
{
    int i;

    PAGED_CODE();

    for (i = 0; i < EVENT_GUID_TABLE_SIZE; i++) {
        if (PnpCompareGuid(Guid, EventGuidTable[i].Guid)) {
            strncpy_s(String,
                      StringLength,
                      EventGuidTable[i].Name,
                      strlen(EventGuidTable[i].Name));
            return;
        }
    }

    RtlStringCchPrintfA(String, StringLength, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
                        Guid->Data1,
                        Guid->Data2,
                        Guid->Data3,
                        Guid->Data4[0],
                        Guid->Data4[1],
                        Guid->Data4[2],
                        Guid->Data4[3],
                        Guid->Data4[4],
                        Guid->Data4[5],
                        Guid->Data4[6],
                        Guid->Data4[7]);
}

VOID
PnpDumpDeviceEvent (
    __in PPLUGPLAY_EVENT_BLOCK EventBlock
    )
{
    CHAR guidString[256];

    PAGED_CODE();

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "PlugPlay Event Block @ 0x%p\n", EventBlock));

    PnpLookupGuid(&EventBlock->EventGuid, guidString, sizeof(guidString));
    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "    EventGuid = %s\n", guidString));

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
               "    DeviceObject = 0x%p\n", EventBlock->DeviceObject));

    switch (EventBlock->EventCategory) {
    case HardwareProfileChangeEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    HardwareProfileChangeEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        break;

    case TargetDeviceChangeEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    TargetDeviceChangeEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceId = %S\n", EventBlock->u.TargetDevice.DeviceId));

        break;

    case DeviceClassChangeEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceClassChangeEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        PnpLookupGuid(&EventBlock->u.DeviceClass.ClassGuid, guidString, sizeof(guidString));
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    ClassGuid = %s\n",
                   guidString));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    SymbolicLinkName = %S\n",
                   EventBlock->u.DeviceClass.SymbolicLinkName));

        break;

    case CustomDeviceEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    CustomDeviceEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    NotificationStructure = 0x%p\n    DeviceId = %S\n",
                   EventBlock->u.CustomNotification.NotificationStructure,
                   EventBlock->u.CustomNotification.DeviceId));

        break;

    case DeviceInstallEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceInstallEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceId = %S\n", EventBlock->u.InstallDevice.DeviceId));

        break;

    case DeviceArrivalEvent:
        IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                   "    DeviceArrivalEvent, Result = 0x%p, Flags = 0x%08X, TotalSize = %d\n",
                   EventBlock->Result,
                   EventBlock->Flags,
                   EventBlock->TotalSize));

        break;
    };

    return;
}


VOID
PnpCollectOpenHandles (
    __in_ecount(ArrayCount) PDEVICE_OBJECT *DeviceObjectArray,
    __in      ULONG           ArrayCount,
    __in      PENUM_HANDLES_CONTEXT EnumContext
    )
/*++

Routine Description:

    This helper routine finds any handles opened against the passed in array of
    device objects to either the veto string, the debugger console, or neither.

Arguments:

    DeviceObjectArray - Array of Physical Device Objects.

    ArrayCount - Number of device objects in the passed in array

    Context - Supplies a pointer to the enumeration context structure
              allocated by the caller.

Return Value:

    None

--*/
{
    ULONG i;

    PAGED_CODE();

    if (EnumContext->DumpHandles != FALSE) {
        IopDebugPrint(IOP_IOEVENT_WARNING_LEVEL, "Beginning handle dump:\n");
    }

    EnumContext->HandleCount = 0;
    PNP_ASSERT(EnumContext->TargetDeviceInstance);
    InitializeListHead(&EnumContext->VetoProcessList);

    //
    // Only enumerate handles if we need to, either if the dumping flag
    // has been set or the caller requested it.
    //

    if ((EnumContext->DumpHandles != FALSE)|| (EnumContext->EnableLogging != FALSE)) {
        for(i=0; i<ArrayCount; i++) {
            EnumContext->PhysicalDeviceObject = DeviceObjectArray[i];
            PnpHandleEnumerateHandlesAgainstPdoStack(DeviceObjectArray[i],
                                                     PnpCollectOpenHandlesCallBack,
                                                     (PVOID)EnumContext);
        }
    }

    if (EnumContext->DumpHandles) {
        IopDebugPrint(IOP_IOEVENT_WARNING_LEVEL,
                      "Dump complete - %d total handles found.\n",
                      EnumContext->HandleCount);
    }
}

LOGICAL
PnpCollectOpenHandlesCallBack (
    __in  PDEVICE_OBJECT  DeviceObject,
    __in  PEPROCESS       Process,
    __in  PFILE_OBJECT    FileObject,
    __in  HANDLE          HandleId,
    __in  PVOID           Context
    )
/*++

Routine Description:

    This helper routine for PnpCollectOpenHandlesCallBack. It gets called
    back for each handle opened against a given device object.

Arguments:

    DeviceObject - Device Object handle was against. Will be valid (referenced)

    Process - Process handle was against. Will be valid (referenced)

    FileObject - File object pertaining to handle - might not be valid

    HandleId - Handle relating to open device - might not be valid

    Context - Context passed in to PnpHandleEnumerateHandlesAgainstPdoStack.

Return Value

    TRUE if enumeration should be stopped.  FALSE otherwise.

--*/
{
    PENUM_HANDLES_CONTEXT enumContext;

    PAGED_CODE();

    enumContext = (PENUM_HANDLES_CONTEXT) Context;
    if (enumContext->DumpHandles) {

        //
        // Display the handle.
        //

        IopDebugPrint(IOP_IOEVENT_WARNING_LEVEL,
                      "  DeviceObject:%p ProcessID:%dT FileObject:%p Handle:%dT\n",
                      DeviceObject,
                      Process->UniqueProcessId,
                      FileObject,
                      HandleId);
    }

    if (enumContext->EnableLogging) {
        PnpAddVetoInformation(enumContext->PhysicalDeviceObject,
                              &enumContext->VetoProcessList,
                              (ULONG)(ULONG_PTR)Process->UniqueProcessId);
    }

    enumContext->HandleCount++;

    //
    // Always return FALSE since we want to explore all open handles
    // against the device.
    //

    return FALSE;
}


NTSTATUS
PnpWaitForEmptyDeviceEventQueue (
    VOID
    )

/*++

Routine Description:

    This routine will wait until all currently queued device events complete.

Arguments:

    None.

Return Value:

    NTSTATUS.

--*/

{
    PAGED_CODE();

    return KeWaitForSingleObject(&PnpEventQueueEmpty,
                                 Executive,
                                 KernelMode,
                                 FALSE,
                                 NULL);
}

NTSTATUS
PnpSetDeviceInstanceRemovalEvent (
    __in  PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine is called to notify user mode that a device node has been
    unlinked from the tree.  Any subsequent queries for device node status will
    return with STATUS_NO_SUCH_DEVICE.

Arguments:

    DeviceNode - Supplies a pointer to the device node that was unlinked.

Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_OBJECT deviceObject;
    ULONG nullIndex;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Make sure this device has been unlinked.
    //

    PNP_ASSERT((DeviceNode->Parent == NULL) &&
               (DeviceNode->Child == NULL) &&
               (DeviceNode->Sibling == NULL) &&
               (DeviceNode->LastChild == NULL));

    deviceObject = DeviceNode->PhysicalDeviceObject;

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field so it works out. Add one more for double-null term.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    dataSize += PnpGetDeviceInstancePath(DeviceNode)->Length + sizeof(WCHAR);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The device object is not referenced here as we want it to be deleted
    // when the remove irp completes and not when user mode finishes processing
    // this event.
    //

    deviceEvent->CallerEvent = NULL;
    deviceEvent->Argument = 0;
    deviceEvent->VetoType = NULL;
    deviceEvent->VetoName = NULL;
    deviceEvent->Data.EventGuid = GUID_DEVINST_REMOVE_COMPLETE;
    deviceEvent->Data.EventCategory = DeviceInstanceRemovalEvent;
    deviceEvent->Data.Result = 0;
    deviceEvent->Data.Flags = 0;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = NULL;
    if (PnpIsDeviceInstancePathEmpty(DeviceNode) == FALSE) {
        RtlCopyMemory((PVOID)deviceEvent->Data.u.TargetDevice.DeviceId,
                    PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                    PnpGetDeviceInstancePath(DeviceNode)->Length);
    } else {
        //
        // While the code is structured to handle this case, it is
        // unexpected and we should determine why this path is taken.
        //
        PNP_ASSERT(FALSE);
    }

    nullIndex = PnpGetDeviceInstancePath(DeviceNode)->Length/sizeof(WCHAR);
    deviceEvent->Data.u.TargetDevice.DeviceId[nullIndex] = L'\0';

    //
    // Insert the event into the queue.
    //

    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

NTSTATUS
PnpSetDeviceInstancePropertyChangeEvent (
    __in  PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine is called to notify user mode that a property on the device node
    has changed.

Arguments:

    DeviceNode - Supplies a pointer to the device node on which a property has changed

Return Value:

    None.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    status = PnpSetDeviceInstancePropertyChangeEventFromDeviceInstance(PnpGetDeviceInstancePath(DeviceNode));

    return status;
}

NTSTATUS
PnpSetDeviceInstancePropertyChangeEventFromDeviceInstance(
    __in  PUNICODE_STRING DeviceInstance
    )

/*++

Routine Description:

    This routine is called to notify user mode that a property on the device instance
    has changed.

Arguments:

    DeviceInstance - Supplies a pointer to the device instance string.

Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    ULONG nullIndex;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field so it works out. Add one more for double-null term.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    dataSize += DeviceInstance->Length + sizeof(WCHAR);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The device object is not referenced here as the event is asynchronous and the client
    // must handle cases where this device object does not exist when the notification is
    // received.
    //

    deviceEvent->CallerEvent = NULL;
    deviceEvent->Argument = 0;
    deviceEvent->VetoType = NULL;
    deviceEvent->VetoName = NULL;
    deviceEvent->Data.EventGuid = GUID_DEVICE_PROPERTY_CHANGED;
    deviceEvent->Data.EventCategory = DevicePropertyChangeEvent;
    deviceEvent->Data.Result = 0;
    deviceEvent->Data.Flags = 0;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = NULL;
    if (DeviceInstance->Buffer) {
        RtlCopyMemory((PVOID)deviceEvent->Data.u.PropertyChangeNotification.DeviceId,
                       DeviceInstance->Buffer,
                       DeviceInstance->Length);
    }

    nullIndex = DeviceInstance->Length/sizeof(WCHAR);
    deviceEvent->Data.u.PropertyChangeNotification.DeviceId[nullIndex] = L'\0';

    //
    // Insert the event into the queue.
    //

    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

NTSTATUS
PnpSetDeviceInstanceStartedEvent (
    __in  PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine is called to notify user mode that a device instance is
    "started" (i.e. the driver on the device represented by the instance
    path is ready to receive I/O)

Arguments:

    DeviceNode - Supplies a pointer to the device node which is started

Return Value:

    None.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    status = PnpSetDeviceInstanceStartedEventFromDeviceInstance(PnpGetDeviceInstancePath(DeviceNode));

    return status;
}

NTSTATUS
PnpSetDeviceInstanceStartedEventFromDeviceInstance(
    __in  PUNICODE_STRING DeviceInstance
    )

/*++

Routine Description:

    This routine is called to notify user mode that the device represented by
    the device instance path is in the started state.

Arguments:

    DeviceInstance - Supplies a pointer to the device instance string.

Return Value:

    None.

--*/

{
    ULONG dataSize;
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    ULONG nullIndex;
    NTSTATUS status;
    ULONG totalSize;

    PAGED_CODE();

    if (PnpIsShutDown()) {
        return STATUS_TOO_LATE;
    }

    //
    // Calculate the size of the PLUGPLAY_EVENT_BLOCK, this is the size that
    // we record in the TotalSize field later (the length doesn't count the
    // terminating null but we're already counting the first index into the
    // DeviceId field so it works out. Add one more for double-null term.
    //

    dataSize = sizeof(PLUGPLAY_EVENT_BLOCK);
    dataSize += DeviceInstance->Length + sizeof(WCHAR);
    totalSize = FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) + dataSize;
    deviceEvent = PnpCreateDeviceEventEntry(totalSize);
    if (deviceEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The device object is not referenced here as the event is asynchronous and the client
    // must handle cases where this device object does not exist when the notification is
    // received.
    //

    deviceEvent->CallerEvent = NULL;
    deviceEvent->Argument = 0;
    deviceEvent->VetoType = NULL;
    deviceEvent->VetoName = NULL;
    deviceEvent->Data.EventGuid = GUID_DEVICE_ARRIVAL;
    deviceEvent->Data.EventCategory = DeviceInstanceStartedEvent;
    deviceEvent->Data.Result = 0;
    deviceEvent->Data.Flags = 0;
    deviceEvent->Data.TotalSize = dataSize;
    deviceEvent->Data.DeviceObject = NULL;
    if (DeviceInstance->Buffer) {
        RtlCopyMemory((PVOID)deviceEvent->Data.u.PropertyChangeNotification.DeviceId,
                       DeviceInstance->Buffer,
                       DeviceInstance->Length);
    }

    nullIndex = DeviceInstance->Length/sizeof(WCHAR);
    deviceEvent->Data.u.PropertyChangeNotification.DeviceId[nullIndex] = L'\0';

    //
    // Insert the event into the queue.
    //

    status = PnpInsertEventInQueue(deviceEvent);
    return status;
}

