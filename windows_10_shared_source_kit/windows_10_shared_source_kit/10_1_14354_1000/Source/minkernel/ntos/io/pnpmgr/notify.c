/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    notify.c

Abstract:

    This module contains APIs and routines for handling device event
    notifications.

Author:


Environment:

    Kernel mode

Revision History:


--*/

#include "pnpmgrp.h"
#pragma hdrstop
#include <pnpmgr.h>
#include <pnpsetup.h>

#pragma warning(disable:4054)   // function pointer to data pointer cast

//
// Kernel mode notification data
//

#pragma  const_seg("PAGECONST")
#pragma  bss_seg("PAGEDATAZ")

LIST_ENTRY PnpDeviceClassNotifyList[NOTIFY_DEVICE_CLASS_HASH_BUCKETS];
LIST_ENTRY PnpProfileNotifyList;
LIST_ENTRY PnpDeferredRegistrationList;

#pragma  bss_seg()

KGUARDED_MUTEX PnpDeviceClassNotifyLock;
KGUARDED_MUTEX PnpTargetDeviceNotifyLock;
KGUARDED_MUTEX PnpHwProfileNotifyLock;
KGUARDED_MUTEX PnpDeferredRegistrationLock;

NTSTATUS
PnpUnregisterPlugPlayNotification (
    __in PVOID NotificationEntry,
    __in BOOLEAN AcquireEntryLock
    );

#pragma alloc_text(INIT, PnpInitializeNotification)
#pragma alloc_text(PAGE, IoGetRelatedTargetDevice)
#pragma alloc_text(PAGE, IoNotifyPowerOperationVetoed)
#pragma alloc_text(PAGE, IoPnPDeliverServicePowerNotification)
#pragma alloc_text(PAGE, IoRegisterPlugPlayNotification)
#pragma alloc_text(PAGE, IoReportTargetDeviceChange)
#pragma alloc_text(PAGE, IoUnregisterPlugPlayNotification)
#pragma alloc_text(PAGE, IoUnregisterPlugPlayNotificationEx)
#pragma alloc_text(PAGE, PnpDereferenceNotify)
#pragma alloc_text(PAGE, PnpNotifyDeviceClassChange)
#pragma alloc_text(PAGE, PnpNotifyHwProfileChange)
#pragma alloc_text(PAGE, PnpNotifyTargetDeviceChange)
#pragma alloc_text(PAGE, PnpOrphanNotification)
#pragma alloc_text(PAGE, PnpProcessDeferredRegistrations)
#pragma alloc_text(PAGE, PnpReferenceNotify)
#pragma alloc_text(PAGE, PnpReportTargetDeviceChangeAsyncWorker)
#pragma alloc_text(PAGE, PnpRequestHwProfileChangeNotification)
#pragma alloc_text(PAGE, PnpNotifyDriverCallback)
#pragma alloc_text(PAGE, PnpDeferNotification)
#pragma alloc_text(PAGE, PnpInitializeNotifyEntry)
//#pragma alloc_text(NONPAGE, PnpGetRelatedTargetDevice)

__drv_maxIRQL(PASSIVE_LEVEL)
__drv_freesMem(Pool)
NTSTATUS
IoUnregisterPlugPlayNotification (
    __in PVOID NotificationEntry
    )

/*++

Routine Description:

    This routine unregisters a notification previously registered via
    IoRegisterPlugPlayNotification.  A driver cannot be unloaded until it has
    unregistered all of its notification handles.

    Note that this routine has been deprecated. Drivers should use
    IoUnregisterPlugPlayNotificationEx instead to protect against
    getting a callback after unregistration has completed.

Parameters:

    NotificationEntry - This provides the cookie returned by
        IoRegisterPlugPlayNotification which identifies the registration in
        question.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{

    PAGED_CODE();

    return PnpUnregisterPlugPlayNotification(NotificationEntry, FALSE);
}

__drv_maxIRQL(PASSIVE_LEVEL)
__drv_freesMem(Pool)
NTSTATUS
IoUnregisterPlugPlayNotificationEx (
    __in PVOID NotificationEntry
    )

/*++

Routine Description:

    This routine unregisters a notification previously registered via
    IoRegisterPlugPlayNotification.  A driver cannot be unloaded until it has
    unregistered all of its notification handles.

    Note that this routine synchronizes between unregistration and pending
    callbacks such that when this routine returns, the caller can be guaranteed
    no further callbacks will be invoked.  If this routine is called from
    within the callback, it must be called on the same thread as the callback
    or a deadlock in pnp will occur.

Parameters:

    NotificationEntry - This provides the cookie returned by
        IoRegisterPlugPlayNotification which identifies the registration in
        question.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{

    PAGED_CODE();

    return PnpUnregisterPlugPlayNotification(NotificationEntry, TRUE);
}

NTSTATUS
PnpUnregisterPlugPlayNotification (
    __in PVOID NotificationEntry,
    __in BOOLEAN AcquireEntryLock
    )

/*++

Routine Description:

    This routine does the main work of unregistering a notification previously
    registered via IoRegisterPlugPlayNotification.  A driver cannot be unloaded
    until it has unregistered all of its notification handles.

    By specifying AcquireEntryLock to be true, this routines ensures that
    no further callbacks will be invoked for the given notification entry
    after this routine returns to the caller.

Parameters:

    NotificationEntry - This provides the cookie returned by
        IoRegisterPlugPlayNotification which identifies the registration in
        question.

    AcquireNotifyLock - If TRUE, specifies that unregistrations should be
        synchronized with any in flight callback.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{

    PDEFERRED_REGISTRATION_ENTRY deferredNode;
    PNOTIFY_ENTRY_HEADER entry;
    PLIST_ENTRY link;
    PKGUARDED_MUTEX lock;
    BOOLEAN wasDeferred;

    PAGED_CODE();

    PNP_ASSERT(NotificationEntry);

    wasDeferred = FALSE;
    entry = (PNOTIFY_ENTRY_HEADER)NotificationEntry;
    lock = entry->Lock;
    KeAcquireGuardedMutex(&PnpNotificationInProgressLock);
    if (PnpNotificationInProgress) {

        //
        // Before unregistering the entry, we need to make sure that it's not
        // sitting around in the deferred registration list.
        //

        PnpAcquireNotifyLock(&PnpDeferredRegistrationLock);
        link = PnpDeferredRegistrationList.Flink;
        while (link != (PLIST_ENTRY)&PnpDeferredRegistrationList) {
            deferredNode = (PDEFERRED_REGISTRATION_ENTRY)link;

            PNP_ASSERT(deferredNode->NotifyEntry->Unregistered);

            if (deferredNode->NotifyEntry == entry) {
                wasDeferred = TRUE;
                if (lock) {
                    PnpAcquireNotifyLock(lock);
                }

                link = link->Flink;
                RemoveEntryList((PLIST_ENTRY)deferredNode);
                PnpDereferenceNotify(
                    (PNOTIFY_ENTRY_HEADER)deferredNode->NotifyEntry);

                if (lock) {
                    PnpReleaseNotifyLock(lock);
                }

                ExFreePoolWithTag(deferredNode, PNP_POOLTAG_DEFERRED_NOTIFY);

            } else {
                link = link->Flink;
            }
        }

        PnpReleaseNotifyLock(&PnpDeferredRegistrationLock);

    } else {

        //
        // If there is currently no notification in progress, the deferred
        // registration list must be empty.
        //

        PNP_ASSERT(IsListEmpty(&PnpDeferredRegistrationList));
    }

    KeReleaseGuardedMutex(&PnpNotificationInProgressLock);
    if (AcquireEntryLock != FALSE) {
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);

    } else if (lock) {
        PnpAcquireNotifyLock(lock);
    }


    PNP_ASSERT(wasDeferred == entry->Unregistered);

    if (!entry->Unregistered || wasDeferred) {

        //
        // Mark the entry as unregistered so we don't notify on it
        //

        entry->Unregistered = TRUE;
        if (AcquireEntryLock != FALSE) {
            ExReleaseResourceLite(entry->EntryLock);
            KeLeaveCriticalRegion();

            if (lock) {
                PnpAcquireNotifyLock(lock);
            }
        }

        //
        // Dereference it thus deleting if no longer required
        //

        PnpDereferenceNotify(entry);
        if (lock) {
            PnpReleaseNotifyLock(lock);
        }

    } else if (AcquireEntryLock != FALSE) {
        ExReleaseResourceLite(entry->EntryLock);
        KeLeaveCriticalRegion();

    } else if (lock) {
        PnpReleaseNotifyLock(lock);
    }

    return STATUS_SUCCESS;
}

VOID
PnpProcessDeferredRegistrations (
    VOID
    )
/*++

Routine Description:

    This routine removes notification entries from the deferred registration
    list, marking them as "registered" so that they can receive notifications.

Parameters:

    None.

Return Value:

    None.

  --*/
{
    PDEFERRED_REGISTRATION_ENTRY deferredNode;
    PKGUARDED_MUTEX lock;

    PAGED_CODE();

    PnpAcquireNotifyLock(&PnpDeferredRegistrationLock);

    while (!IsListEmpty(&PnpDeferredRegistrationList)) {

        deferredNode = (PDEFERRED_REGISTRATION_ENTRY)RemoveHeadList(&PnpDeferredRegistrationList);

        //
        // Acquire this entry's list lock.
        //
        lock = deferredNode->NotifyEntry->Lock;
        if (lock) {
            PnpAcquireNotifyLock(lock);
        }

        //
        // Mark this entry as registered.
        //
        deferredNode->NotifyEntry->Unregistered = FALSE;

        //
        // Dereference the notification entry when removing it from the deferred
        // list, and free the node.
        //

        PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)deferredNode->NotifyEntry);
        ExFreePoolWithTag(deferredNode, PNP_POOLTAG_DEFERRED_NOTIFY);

        //
        // Release this entry's list lock.
        //
        if (lock) {
            PnpReleaseNotifyLock(lock);
            lock = NULL;
        }
    }

    PnpReleaseNotifyLock(&PnpDeferredRegistrationLock);
}

NTSTATUS
PnpDeferNotification (
    __in PNOTIFY_ENTRY_HEADER Entry
    )

/*++

Routine Description:

    If a notification is currently in progress, this routine will insert the
    notification into the list of deferred notifications.

Parameters:

    Entry - Notification entry.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status;
    PDEFERRED_REGISTRATION_ENTRY deferredNode;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    KeAcquireGuardedMutex(&PnpNotificationInProgressLock);
    if (PnpNotificationInProgress) {

        //
        // If a notification is in progress, simply add it to the
        // end of the deferred list.
        //

        deferredNode = ExAllocatePoolWithTag(PagedPool,
                                             sizeof(DEFERRED_REGISTRATION_ENTRY),
                                             PNP_POOLTAG_DEFERRED_NOTIFY);
        if (deferredNode) {

            //
            // Save the entry being deferred.
            //

            deferredNode->NotifyEntry = Entry;

            //
            // Consider this entry unregistered during the current
            // notification
            //

            Entry->Unregistered = TRUE;

            //
            // Reference the entry so that it doesn't go away until it has
            // been removed from the deferred registration list
            //

            PnpReferenceNotify(Entry);

            //
            // Add this entry to the deferred registration list
            //

            PnpAcquireNotifyLock(&PnpDeferredRegistrationLock);
            InsertTailList(&PnpDeferredRegistrationList,
                           &deferredNode->ListEntry);
            PnpReleaseNotifyLock(&PnpDeferredRegistrationLock);

        } else {
            status = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {

        //
        // If there is currently no notification in progress, the deferred
        // registration list must be empty.
        //

        PNP_ASSERT(IsListEmpty(&PnpDeferredRegistrationList));
    }

    KeReleaseGuardedMutex(&PnpNotificationInProgressLock);
    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
IoReportTargetDeviceChange (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in PVOID NotificationStructure  // always begins with a PLUGPLAY_NOTIFICATION_HEADER
    )

/*++

Routine Description:

    This routine may be used to give notification of 3rd-party target device
    change events.  This API will notify every driver that has registered for
    notification on a file object associated with PhysicalDeviceObject about
    the event indicated in the NotificationStructure.

Parameters:

    PhysicalDeviceObject - Provides a pointer to the PDO that the change begin
        reported is associated with.

    NotificationStructure - Provides a pointer to the notification structure to be
        sent to all parties registered for notifications about changes to
        PhysicalDeviceObject.

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    This API may only be used to report non-PnP target device changes.  In particular,
    it will fail if it's called with the NotificationStructure->Event field set to
    GUID_TARGET_DEVICE_QUERY_REMOVE, GUID_TARGET_DEVICE_REMOVE_CANCELLED, or
    GUID_TARGET_DEVICE_REMOVE_COMPLETE.

--*/
{

    NTSTATUS status = STATUS_SUCCESS;
    KEVENT completionEvent;
    NTSTATUS completionStatus;
    PTARGET_DEVICE_CUSTOM_NOTIFICATION notifyStruct;
    LONG                   dataSize;

    PAGED_CODE();

    notifyStruct = (PTARGET_DEVICE_CUSTOM_NOTIFICATION)NotificationStructure;

    PNP_ASSERT(notifyStruct);

    ASSERT_PDO(PhysicalDeviceObject);

    PNP_ASSERT(NULL == notifyStruct->FileObject);

    if (IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_QUERY_REMOVE) ||
        IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_REMOVE_CANCELLED) ||
        IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_REMOVE_COMPLETE)) {
        //
        //  Passed in an illegal value
        //
        IopDbgPrint((
            IOP_IOEVENT_ERROR_LEVEL,
            "IoReportTargetDeviceChange: "
            "Illegal Event type passed as custom notification\n"));

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if (notifyStruct->Size < FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer)) {

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    dataSize = notifyStruct->Size - FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);

    if (notifyStruct->NameBufferOffset != -1 && notifyStruct->NameBufferOffset > dataSize)  {

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    KeInitializeEvent(&completionEvent, NotificationEvent, FALSE);

    status = PnpSetCustomTargetEvent(PhysicalDeviceObject,
                                     &completionEvent,
                                     (PULONG)&completionStatus,
                                     NULL,
                                     NULL,
                                     notifyStruct);
    if (NT_SUCCESS(status))  {

        KeWaitForSingleObject(&completionEvent, Executive, KernelMode, FALSE, NULL);
        status = completionStatus;
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
IoReportTargetDeviceChangeAsynchronous (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in PVOID NotificationStructure,  // always begins with a PLUGPLAY_NOTIFICATION_HEADER
    __in_opt PDEVICE_CHANGE_COMPLETE_CALLBACK Callback,
    __in_opt PVOID Context
    )

/*++

Routine Description:

    This routine may be used to give notification of 3rd-party target device
    change events.  This API will notify every driver that has registered for
    notification on a file object associated with PhysicalDeviceObject about
    the event indicated in the NotificationStructure.

Parameters:

    PhysicalDeviceObject - Provides a pointer to the PDO that the change begin
        reported is associated with.

    NotificationStructure - Provides a pointer to the notification structure to be
        sent to all parties registered for notifications about changes to
        PhysicalDeviceObject.

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    This API may only be used to report non-PnP target device changes.  In particular,
    it will fail if it's called with the NotificationStructure->Event field set to
    GUID_TARGET_DEVICE_QUERY_REMOVE, GUID_TARGET_DEVICE_REMOVE_CANCELLED, or
    GUID_TARGET_DEVICE_REMOVE_COMPLETE.

--*/
{
    PASYNC_TDC_WORK_ITEM    asyncWorkItem;
    PWORK_QUEUE_ITEM        workItem;
    NTSTATUS                status;
    LONG                    dataSize;
    PTARGET_DEVICE_CUSTOM_NOTIFICATION   notifyStruct;

    notifyStruct = (PTARGET_DEVICE_CUSTOM_NOTIFICATION)NotificationStructure;

    PNP_ASSERT(notifyStruct);

    ASSERT_PDO(PhysicalDeviceObject);

    PNP_ASSERT(NULL == notifyStruct->FileObject);

    if (IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_QUERY_REMOVE) ||
        IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_REMOVE_CANCELLED) ||
        IopCompareGuid(&notifyStruct->Event, &GUID_TARGET_DEVICE_REMOVE_COMPLETE)) {

        //
        //  Passed in an illegal value
        //

        IopDbgPrint((IOP_IOEVENT_ERROR_LEVEL,
                     "IoReportTargetDeviceChangeAsynchronous: "
                     "Illegal Event type passed as custom notification\n"));
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if (notifyStruct->Size < FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    dataSize = notifyStruct->Size - FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
    if (notifyStruct->NameBufferOffset != -1 && notifyStruct->NameBufferOffset > dataSize)  {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Since this routine can be called at DPC level we need to queue
    // a work item and process it when the irql drops.
    //

    if (KeGetCurrentIrql() == DISPATCH_LEVEL) {
        asyncWorkItem = ExAllocatePoolWithTag(NonPagedPoolNx,
                                              sizeof(ASYNC_TDC_WORK_ITEM) +
                                                notifyStruct->Size,
                                              PNP_POOLTAG_ASYNC_TDC_NOTIFY);
        if (asyncWorkItem != NULL) {

            ObReferenceObject(PhysicalDeviceObject);
            asyncWorkItem->DeviceObject = PhysicalDeviceObject;
            asyncWorkItem->NotificationStructure =
                (PTARGET_DEVICE_CUSTOM_NOTIFICATION)((PUCHAR)asyncWorkItem +
                    sizeof(ASYNC_TDC_WORK_ITEM));
            RtlCopyMemory(asyncWorkItem->NotificationStructure,
                          notifyStruct,
                          notifyStruct->Size);
            asyncWorkItem->Callback = Callback;
            asyncWorkItem->Context = Context;
            workItem = &asyncWorkItem->WorkItem;
            ExInitializeWorkItem(workItem,
                                 PnpReportTargetDeviceChangeAsyncWorker,
                                 asyncWorkItem);

            //
            // Queue a work item to do the enumeration
            //

            ExQueueWorkItem(workItem, DelayedWorkQueue);
            status = STATUS_PENDING;

        } else {

            //
            // Failed to allocate memory for work item.  Nothing we can do ...
            //

            status = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {

        status = PnpSetCustomTargetEvent(PhysicalDeviceObject,
                                         NULL,
                                         NULL,
                                         Callback,
                                         Context,
                                         notifyStruct);
    }

    return status;
}

VOID
PnpReportTargetDeviceChangeAsyncWorker (
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is the worker routine of IoInvalidateDeviceState.
    Its main purpose is to invoke IopSynchronousQueryDeviceState and release
    work item space.

Parameters:

    Context - Supplies a pointer to the ASYNC_TDC_WORK_ITEM.

ReturnValue:

    None.

--*/

{
    PASYNC_TDC_WORK_ITEM asyncWorkItem;

    PAGED_CODE();

    asyncWorkItem = (PASYNC_TDC_WORK_ITEM)Context;
    PnpSetCustomTargetEvent(asyncWorkItem->DeviceObject,
                            NULL,
                            NULL,
                            asyncWorkItem->Callback,
                            asyncWorkItem->Context,
                            asyncWorkItem->NotificationStructure);
    ObDereferenceObject(asyncWorkItem->DeviceObject);
    ExFreePoolWithTag(asyncWorkItem, PNP_POOLTAG_ASYNC_TDC_NOTIFY);
    return;
}

VOID
PnpInitializeNotification (
    VOID
    )

/*++

Routine Description:

    This routine performs initialization required before any of the notification
    APIs can be called.

Parameters:

    None

Return Value:

    None

--*/

{
    ULONG count;

    PAGED_CODE();

    //
    // Initialize the notification structures
    //
    for (count = 0; count < NOTIFY_DEVICE_CLASS_HASH_BUCKETS; count++) {

        InitializeListHead(&PnpDeviceClassNotifyList[count]);

    }
    //
    // Initialize the profile notification list
    //
    InitializeListHead(&PnpProfileNotifyList);

    //
    // Initialize the deferred registration list
    //
    InitializeListHead(&PnpDeferredRegistrationList);

    KeInitializeGuardedMutex(&PnpDeviceClassNotifyLock);
    KeInitializeGuardedMutex(&PnpTargetDeviceNotifyLock);
    KeInitializeGuardedMutex(&PnpHwProfileNotifyLock);
    KeInitializeGuardedMutex(&PnpDeferredRegistrationLock);
    return;
}

VOID
PnpReferenceNotify (
    __in PNOTIFY_ENTRY_HEADER Notify
    )

/*++

Routine Description:

    This routine increments the reference count for a notification entry.

Parameters:

    Notify - Supplies a pointer to the notification entry to be referenced

Return Value:

    None

Note:

    The appropriate synchronization lock must be held on the notification
    list before this routine can be called

--*/

{
    PAGED_CODE();

    PNP_ASSERT(Notify);
    PNP_ASSERT(Notify->RefCount > 0);

    Notify->RefCount++;

}

VOID
PnpDereferenceNotify (
    __in PNOTIFY_ENTRY_HEADER Notify
    )

/*++

Routine Description:

    This routine decrements the reference count for a notification entry, removing
    the entry from the list and freeing the associated memory if there are no
    outstanding reference counts.

Parameters:

    Notify - Supplies a pointer to the notification entry to be referenced

Return Value:

    None

Note:

    The appropriate synchronization lock must be held on the notification
    list before this routine can be called.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Notify);

    //
    // One possible reason to have a RefCount already at 0 before entering this
    // routine is that the driver called both IoUnregisterPlugPlayNotification
    // and IoUnregisterPlugPlayNotificationEx. Catch that case here by
    // asserting. Note that whichever call completes last will essentially be
    // a no-op as the RefCount will have been decremented past 0.
    //

    PNP_ASSERT(Notify->RefCount > 0);

    Notify->RefCount--;
    if (Notify->RefCount == 0) {

        //
        // If the refcount is zero then the node should have been deregisterd
        // and is no longer needs to be in the list so remove and free it
        //

        PNP_ASSERT(Notify->Unregistered);

        //
        // Remove the notification entry from its list.
        //
        // Note that this MUST be done first, since the notification list head
        // for a target device notification entry resides in the target device
        // node, which may be freed immediately after the device object is
        // dereferenced.  For notification entry types other than target device
        // change this is not critical, but still a good idea.
        //

        RemoveEntryList((PLIST_ENTRY)Notify);

        //
        // Dereference the driver object that registered for notifications
        //

        ObDereferenceObject(Notify->DriverObject);

        //
        // If this notification entry is for target device change, dereference
        // the PDO upon which this notification entry was hooked.
        //

        if (Notify->EventCategory == EventCategoryTargetDeviceChange) {
            PTARGET_DEVICE_NOTIFY_ENTRY entry = (PTARGET_DEVICE_NOTIFY_ENTRY)Notify;
            if (entry->PhysicalDeviceObject) {
                ObDereferenceObject(entry->PhysicalDeviceObject);
                entry->PhysicalDeviceObject = NULL;
            }
        }

        //
        // Close the handle to the kernel session object.
        //

        if (Notify->SessionHandle != NULL) {
            ZwClose(Notify->SessionHandle);
            Notify->SessionHandle = NULL;
        }

        //
        // Free the notification entry
        //

        ExDeleteResourceLite(Notify->EntryLock);
        ExFreePoolWithTag(Notify->EntryLock, PNP_POOLTAG_NOTIFY_ENTRY_LOCK);
        ExFreePool(Notify);
    }
}

NTSTATUS
PnpRequestHwProfileChangeNotification (
    __in const GUID* EventGuid,
    __in PROFILE_NOTIFICATION_TIME NotificationTime,
    __out_opt PPNP_VETO_TYPE VetoType,
    __out_opt PUNICODE_STRING VetoName
    )

/*++

Routine Description:

    This routine is used to notify all registered drivers of a hardware profile
    change.  If the operation is a HW provile change query then the operation
    is synchronous and the veto information is propagated.  All other operations
    are asynchronous and veto information is not returned.

Parameters:

    EventTypeGuid       - The event that has occured

    NotificationTime    - This is used to tell if we are already in an event
                          when delivering a synchronous notification (ie,
                          querying profile change to eject). It is one of
                          three values:
                              PROFILE_IN_PNPEVENT
                              PROFILE_NOT_IN_PNPEVENT
                              PROFILE_PERHAPS_IN_PNPEVENT

    VetoType            - Type of vetoer.

    VetoName            - Name of vetoer.

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    The contents of the notification structure *including* all pointers is only
    valid during the callback routine to which it was passed.  If the data is
    required after the duration of the callback then it must be physically copied
    by the callback routine.

--*/

{
    NTSTATUS status=STATUS_SUCCESS,completionStatus;
    KEVENT completionEvent;

    PAGED_CODE();

    if ((!IopCompareGuid(EventGuid, (LPGUID)&GUID_HWPROFILE_QUERY_CHANGE)) &&
        (!IopCompareGuid(EventGuid, (LPGUID)&GUID_HWPROFILE_CHANGE_CANCELLED)) &&
        (!IopCompareGuid(EventGuid, (LPGUID)&GUID_HWPROFILE_CHANGE_COMPLETE))) {

        //
        //  Passed in an illegal value
        //

        IopDbgPrint((
            IOP_IOEVENT_ERROR_LEVEL,
            "PnpRequestHwProfileChangeNotification: "
            "Illegal Event type passed as profile notification\n"));

        return STATUS_INVALID_DEVICE_REQUEST;
    }
    //
    // Only the query changes are synchronous, and in that case we must
    // know definitely whether we are nested within a Pnp event or not.
    //
    PNP_ASSERT((!IopCompareGuid(EventGuid, (LPGUID)&GUID_HWPROFILE_QUERY_CHANGE))||
           (NotificationTime != PROFILE_PERHAPS_IN_PNPEVENT)) ;

    if (!IopCompareGuid(EventGuid, (LPGUID)&GUID_HWPROFILE_QUERY_CHANGE) ) {
        //
        // Asynchronous case. Very easy.
        //
        PNP_ASSERT(!ARGUMENT_PRESENT(VetoName));
        PNP_ASSERT(!ARGUMENT_PRESENT(VetoType));

        return PnpSetHwProfileChangeEvent(EventGuid,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
    }
    //
    // Query notifications are synchronous. Determine if we are currently
    // within an event, in which case we must do the notify here instead
    // of queueing it up.
    //
    if (NotificationTime == PROFILE_NOT_IN_PNPEVENT) {
        //
        // Queue up and block on the notification.
        //
        KeInitializeEvent(&completionEvent, NotificationEvent, FALSE);

        status = PnpSetHwProfileChangeEvent(EventGuid,
                                            &completionEvent,
                                            &completionStatus,
                                            VetoType,
                                            VetoName);

        if (NT_SUCCESS(status))  {

            KeWaitForSingleObject( &completionEvent, Executive, KernelMode, FALSE, NULL );

            status = completionStatus;
        }

        return status;
    }

    //
    // Synchronous notify inside our Pnp event.
    //


    PNP_ASSERT(PnpNotificationInProgress != FALSE);

    //
    // Notify K-Mode
    //
    status = PnpNotifyHwProfileChange((GUID*)EventGuid,
                                      VetoType,
                                      VetoName);
    if (!NT_SUCCESS(status)) {

        return status;
    }
    
    return status;
}

NTSTATUS
PnpNotifyHwProfileChange (
    __in GUID* EventGuid,
    __out_opt PPNP_VETO_TYPE VetoType,
    __out_opt PUNICODE_STRING VetoName
    )

/*++

Routine Description:

    This routine is used to deliver the HWProfileNotifications. It is
    called from the worker thread only
    It does not return until all interested parties have been notified.

Parameters:

    EventTypeGuid - The event that has occured

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    The contents of the notification structure *including* all pointers is only
    valid during the callback routine to which it was passed.  If the data is
    required after the duration of the callback then it must be physically copied
    by the callback routine.

    Two locks synchronize how the notification entry is accessed.
    PnpHwProfileNotifyLock is a global lock protecting the list of
    notification entries. It must be acquired whenever an entry is referenced
    or dereferenced. The EntryLock within each notify entry synchronizes
    against unregistration. It must be acquired before checking whether a notify
    entry is still registered and if so only released after the callback has
    been invoked.  Note that these two locks should never overlap as a deadlock
    may occur.

--*/

{

    NTSTATUS dispatchStatus;
    PHWPROFILE_NOTIFY_ENTRY  entry;
    PLIST_ENTRY link;
    HWPROFILE_CHANGE_NOTIFICATION notification;
    NTSTATUS status;
    PHWPROFILE_NOTIFY_ENTRY  vetoEntry;

    PAGED_CODE();

    status = STATUS_SUCCESS;

    //
    // Lock the Profile Notification List
    //

    PnpAcquireNotifyLock (&PnpHwProfileNotifyLock);

    //
    //  Grab the list head (inside the lock)
    //

    link = PnpProfileNotifyList.Flink;

    //
    // Walk the circular list.
    //

    while (link != (PLIST_ENTRY)&PnpProfileNotifyList) {
        entry = (PHWPROFILE_NOTIFY_ENTRY)link;
        PnpReferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
        PnpReleaseNotifyLock(&PnpHwProfileNotifyLock);
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);

        //
        // Note that if a driver uses the deprecated
        // IoUnregisterPlugPlayNotification routine, access to the Unregistered
        // flag is not protected.  As such, even though the entry may be seen
        // as still registered, it may become unregistered prior to the
        // callback, thus allowing a callback to be invoked after an entry
        // has been unregistered.
        //

        if (!entry->Unregistered) {

            //
            // Reference the entry so that no one deletes during the callback
            // and then release the lock
            //

            notification.Version = PNP_NOTIFICATION_VERSION;
            notification.Size = sizeof(HWPROFILE_CHANGE_NOTIFICATION);
            notification.Event = *EventGuid;

            //
            // Dispatch the notification to the callback routine for the
            // appropriate session.
            //

            dispatchStatus = PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                                     &notification,
                                                     &status);

            PNP_ASSERT(NT_SUCCESS(dispatchStatus));

            ExReleaseResourceLite(entry->EntryLock);
            KeLeaveCriticalRegion();

            //
            // Failure to dispatch the notification to the specified callback
            // should not be considered a veto.
            //

            if (!NT_SUCCESS(dispatchStatus)) {
                status = STATUS_SUCCESS;
            }

            //
            // If the caller returned anything other than success and it was a
            // query hardware profile change, we veto the query and send cancels
            // to all callers that already got the query.
            //

            if ((!NT_SUCCESS(status)) &&
                (IopCompareGuid(EventGuid,
                                (LPGUID)&GUID_HWPROFILE_QUERY_CHANGE))) {

                if (VetoType) {
                    *VetoType = PNP_VetoDriver;
                }

                if (VetoName) {
                    VetoName->Length = 0;
                    RtlCopyUnicodeString(VetoName,
                                         &entry->DriverObject->DriverName);
                }

                notification.Event = GUID_HWPROFILE_CHANGE_CANCELLED;
                notification.Size = sizeof(GUID_HWPROFILE_CHANGE_CANCELLED);

                //
                // Keep track of the entry that vetoed the query.  We can't
                // dereference it just yet, because we may need to send it a
                // cancel-remove first.  Since it's possible that the entry
                // may have been unregistered when the list was unlocked
                // during the query callback (removing all but the reference
                // we are currently holding), we need to make sure we don't
                // dereference it until we're absolutely done with it.
                //

                vetoEntry = entry;
                PnpAcquireNotifyLock(&PnpHwProfileNotifyLock);

                //
                // Make sure we are starting where we left off above, at the
                // vetoing entry.
                //

                PNP_ASSERT((PHWPROFILE_NOTIFY_ENTRY)link == vetoEntry);

                do {
                    entry = (PHWPROFILE_NOTIFY_ENTRY)link;
                    PnpReferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
                    PnpReleaseNotifyLock(&PnpHwProfileNotifyLock);
                    KeEnterCriticalRegion();
                    ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);
                    if (entry->Unregistered == FALSE) {
                        dispatchStatus =
                            PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                                    &notification,
                                                    NULL);

                        PNP_ASSERT(NT_SUCCESS(dispatchStatus));
                    }

                    ExReleaseResourceLite(entry->EntryLock);
                    KeLeaveCriticalRegion();
                    PnpAcquireNotifyLock(&PnpHwProfileNotifyLock);
                    link = link->Blink;
                    PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
                    if (entry == vetoEntry) {

                        //
                        // Dereference the entry which vetoed the query change.
                        //

                        PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
                    }

                } while (link != (PLIST_ENTRY)&PnpProfileNotifyList);

                goto Clean0;
            }

        } else {
            ExReleaseResourceLite(entry->EntryLock);
            KeLeaveCriticalRegion();
        }

        PnpAcquireNotifyLock (&PnpHwProfileNotifyLock);
        link = link->Flink;
        PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
    }

Clean0:

    //
    // Unlock the Profile Notification List.
    //

    PnpReleaseNotifyLock(&PnpHwProfileNotifyLock);
    return status;
}

NTSTATUS
PnpNotifyTargetDeviceChange (
    __in const GUID* EventGuid,
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PTARGET_DEVICE_CUSTOM_NOTIFICATION NotificationStructure,
    __out_opt PDRIVER_OBJECT* VetoingDriver
    )

/*++

Routine Description:

    This routine is used to notify all registered drivers of a change to a
    particular device. It does not return until all interested parties have
    been notified.

Parameters:

    EventGuid - The event guid to send to the drivers.

    DeviceObject - The device object for the affected device.  The devnode for
        this device object contains a list of callback routines that have
        registered for notification of any changes on this device object.

    NotificationStructure - Custom notification structure to send to the
        registrants.

    VetoingDriver - Driver that vetoed the event if
                    (EventGuid == GUID_TARGET_DEVICE_QUERY_REMOVE).

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    The contents of the notification structure *including* all pointers is only
    valid during the callback routine to which it was passed.  If the data is
    required after the duration of the callback then it must be physically copied
    by the callback routine.

    Two locks synchronize how the notification entry is accessed.
    PnpTargetDeviceNotifyLock is a global lock protecting the list of
    notification entries. It must be acquired whenever an entry is referenced
    or dereferenced. The EntryLock within each notify entry synchronizes
    again unregistration. It must be acquired before checking whether a notify
    entry is still registered and if so only released after the callback has
    been invoked. Note that these two locks should never overlap as a deadlock
    may occur.

--*/

{

    ULONG cancelDeviceSessionId;
    PDEVICE_NODE deviceNode;
    ULONG deviceSessionId;
    NTSTATUS dispatchStatus;
    PTARGET_DEVICE_NOTIFY_ENTRY entry;
    PLIST_ENTRY link;
    PVOID notification;
    BOOLEAN reverse;
    NTSTATUS status;
    TARGET_DEVICE_REMOVAL_NOTIFICATION targetNotification;
    PTARGET_DEVICE_NOTIFY_ENTRY vetoEntry;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);
    PNP_ASSERT(EventGuid != NULL);

    deviceSessionId = INVALID_SESSION;

    //
    // Reference the device object so it can't go away while we're doing
    // notification
    //

    ObReferenceObject(DeviceObject);
    deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;

    PNP_ASSERT(deviceNode != NULL);

    if (ARGUMENT_PRESENT(NotificationStructure)) {

        //
        // We're handling a custom notification
        //

        NotificationStructure->Version = PNP_NOTIFICATION_VERSION;

    } else {

        //
        // Fill in the notification structure
        //

        targetNotification.Version = PNP_NOTIFICATION_VERSION;
        targetNotification.Size = sizeof(TARGET_DEVICE_REMOVAL_NOTIFICATION);
        targetNotification.Event = *EventGuid;
    }

    //
    // Lock the notify list
    //

    PnpAcquireNotifyLock(&PnpTargetDeviceNotifyLock);

    //
    // Get the first entry
    //

    reverse =
        (BOOLEAN)IopCompareGuid(EventGuid,
                                (LPGUID)&GUID_TARGET_DEVICE_REMOVE_CANCELLED);

    if (reverse) {
        link = deviceNode->TargetDeviceNotify.Blink;

    } else {
        link = deviceNode->TargetDeviceNotify.Flink;
    }

    //
    // Iterate through the list
    //

    while (link != &deviceNode->TargetDeviceNotify) {
        entry = (PTARGET_DEVICE_NOTIFY_ENTRY)link;

        //
        // Per Session devices - check entry's SessionId
        //

        if (entry->SessionId != SERVICE_SESSION) {
            deviceSessionId = IopGetSessionIdFromPDO(DeviceObject);
        }

        PnpReferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
        PnpReleaseNotifyLock(&PnpTargetDeviceNotifyLock);
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);

        //
        // Note that if a driver uses the deprecated
        // IoUnregisterPlugPlayNotification routine, access to the Unregistered
        // flag is not protected.  As such, even though the entry may be seen
        // as still registered, it may become unregistered prior to the
        // callback, thus allowing a callback to be invoked after an entry
        // has been unregistered.
        //

        if (((deviceSessionId == INVALID_SESSION) ||
             (entry->SessionId == deviceSessionId)) &&
            (entry->Unregistered == FALSE)){

            //
            // Select the notification structure to deliver and set the file
            // object in the notification structure to that for the current
            // entry
            //

            if (ARGUMENT_PRESENT(NotificationStructure)) {
                NotificationStructure->FileObject = entry->FileObject;
                notification = (PVOID)NotificationStructure;

            } else {
                targetNotification.FileObject = entry->FileObject;
                notification = (PVOID)&targetNotification;
            }

            //
            // Dispatch the notification to the callback routine for the
            // appropriate session.
            //

            dispatchStatus =
                PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                        notification,
                                        &status);

            PNP_ASSERT(NT_SUCCESS(dispatchStatus));

            ExReleaseResourceLite(entry->EntryLock);
            KeLeaveCriticalRegion();

            //
            // Failure to dispatch the notification to the specified callback
            // should not be considered a veto.
            //

            if (!NT_SUCCESS(dispatchStatus)) {
                status = STATUS_SUCCESS;
            }

            //
            // If the caller returned anything other than success and it was
            // a query remove, we veto the query remove and send cancels to
            // all callers that already got the query remove.
            //

            if (!NT_SUCCESS(status)) {
                if (IopCompareGuid(EventGuid,
                                   (LPGUID)&GUID_TARGET_DEVICE_QUERY_REMOVE)) {

                    PNP_ASSERT(notification == (PVOID)&targetNotification);

                    if (VetoingDriver != NULL) {
                        *VetoingDriver = entry->DriverObject;
                    }

                    targetNotification.Event =
                        GUID_TARGET_DEVICE_REMOVE_CANCELLED;

                    //
                    // Keep track of the entry that vetoed the query.  We can't
                    // dereference it just yet, because we may need to send it a
                    // cancel-remove first.  Since it's possible that the entry
                    // may have been unregistered when the list was unlocked
                    // during the query callback (removing all but the reference
                    // we are currently holding), we need to make sure we don't
                    // dereference it until we're absolutely done with it.
                    //

                    vetoEntry = entry;
                    PnpAcquireNotifyLock(&PnpTargetDeviceNotifyLock);

                    //
                    // Make sure we are starting where we left off above, at the
                    // vetoing entry.
                    //

                    PNP_ASSERT((PTARGET_DEVICE_NOTIFY_ENTRY)link == vetoEntry);

                    do {
                        cancelDeviceSessionId = INVALID_SESSION;
                        entry = (PTARGET_DEVICE_NOTIFY_ENTRY)link;

                        //
                        // Per Session devices - check entry's SessionId
                        //

                        if (entry->SessionId != SERVICE_SESSION) {
                            cancelDeviceSessionId =
                                IopGetSessionIdFromPDO(DeviceObject);
                        }

                        PnpReferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
                        PnpReleaseNotifyLock(&PnpTargetDeviceNotifyLock);
                        KeEnterCriticalRegion();
                        ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);
                        if (((cancelDeviceSessionId == INVALID_SESSION) ||
                             (entry->SessionId == cancelDeviceSessionId)) &&
                            (entry->Unregistered == FALSE)) {

                            //
                            // Set the file object in the notification structure
                            // to that for the current entry
                            //

                            targetNotification.FileObject = entry->FileObject;

                            //
                            // Dispatch the notification to the callback routine
                            // for the appropriate session.
                            //

                            dispatchStatus =
                                PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                                        &targetNotification,
                                                        NULL);

                            PNP_ASSERT(NT_SUCCESS(dispatchStatus));
                        }

                        ExReleaseResourceLite(entry->EntryLock);
                        KeLeaveCriticalRegion();

                        //
                        // Reacquire the lock and dereference
                        //

                        PnpAcquireNotifyLock(&PnpTargetDeviceNotifyLock);
                        link = link->Blink;
                        PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
                        if (entry == vetoEntry) {

                            //
                            // Dereference the entry which vetoed the query
                            // remove.
                            //

                            PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)vetoEntry);
                        }

                    } while (link != &deviceNode->TargetDeviceNotify);

                    goto Clean0;

                } else {
                    PNP_ASSERT(notification == (PVOID)NotificationStructure);
                    IopDbgPrint((IOP_IOEVENT_ERROR_LEVEL,
                                 "PnpNotifyTargetDeviceChange: "
                                 "Driver %Z, handler @ 0x%p failed non-failable"
                                 " notification 0x%p with return code %x\n",
                                 &entry->DriverObject->DriverName,
                                 entry->CallbackRoutine,
                                 notification,
                                 status));

                    PNP_ASSERT(NT_SUCCESS(status));
                }
            }

        } else {
            ExReleaseResourceLite(entry->EntryLock);
            KeLeaveCriticalRegion();
        }

        PnpAcquireNotifyLock(&PnpTargetDeviceNotifyLock);

        //
        // Advance down the list
        //

        if (reverse) {
            link = link->Blink;

        } else {
            link = link->Flink;
        }

        PnpDereferenceNotify((PNOTIFY_ENTRY_HEADER)entry);
    }

    //
    // If it's not a query, it can't be failed.
    //

    status = STATUS_SUCCESS;

Clean0:

    //
    // Release the lock and dereference the object
    //

    PnpReleaseNotifyLock(&PnpTargetDeviceNotifyLock);
    ObDereferenceObject(DeviceObject);
    return status;
}

NTSTATUS
PnpNotifyDeviceClassChange (
    __in GUID* EventGuid,
    __in GUID* ClassGuid,
    __in PUNICODE_STRING SymbolicLinkName
    )

/*++

Routine Description:

    This routine is used to notify all registered drivers of a changes to a
    particular class of device. It does not return until all interested parties have
    been notified.

Parameters:

    EventTypeGuid - The event that has occured

    ClassGuid - The device class this change has occured in

    SymbolicLinkName - The kernel mode symbolic link name of the interface device
        that changed

Return Value:

    Status code that indicates whether or not the function was successful.

Note:

    The contents of the notification structure *including* all pointers is only
    valid during the callback routine to which it was passed.  If the data is
    required after the duration of the callback then it must be physically copied
    by the callback routine.

    Two locks synchronize how the notification entry is accessed.
    PnpDeviceClassNotifyLock is a global lock protecting the list of
    notification entries. It must be acquired whenever an entry is referenced
    or dereferenced. The EntryLock within each notify entry synchronizes
    again unregistration. It must be acquired before checking whether a notify
    entry is still registered and if so only released after the callback has
    been invoked. Note that these two locks should never overlap as a deadlock
    may occur.

--*/

{

    ULONG deviceSessionId;
    NTSTATUS dispatchStatus;
    PDEVICE_CLASS_NOTIFY_ENTRY entry;
    ULONG hash;
    PLIST_ENTRY link;
    DEVICE_INTERFACE_CHANGE_NOTIFICATION notification;
    NTSTATUS status;

    PAGED_CODE();

    deviceSessionId = INVALID_SESSION;
    dispatchStatus = STATUS_SUCCESS;

    //
    // Fill in the notification structure
    //

    notification.Version = PNP_NOTIFICATION_VERSION;
    notification.Size = sizeof(DEVICE_INTERFACE_CHANGE_NOTIFICATION);
    notification.Event = *EventGuid;
    notification.InterfaceClassGuid = *ClassGuid;
    notification.SymbolicLinkName = SymbolicLinkName;

    //
    // Lock the notify list
    //

    PnpAcquireNotifyLock(&PnpDeviceClassNotifyLock);

    //
    // Get the first entry
    //

    hash = PnpHashGuid(ClassGuid);
    link = PnpDeviceClassNotifyList[hash].Flink;

    //
    // Iterate through the list
    //

    while (link != &PnpDeviceClassNotifyList[hash]) {
        entry = (PDEVICE_CLASS_NOTIFY_ENTRY)link;

        //
        // Per Session devices - check entry's SessionId
        //

        if (entry->SessionId != SERVICE_SESSION) {
            deviceSessionId = IopGetSessionIdFromSymbolicName(SymbolicLinkName);
        }

        PnpReferenceNotify( (PNOTIFY_ENTRY_HEADER) entry );
        PnpReleaseNotifyLock(&PnpDeviceClassNotifyLock);
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(entry->EntryLock, TRUE);

        //
        // Note that if a driver uses the deprecated
        // IoUnregisterPlugPlayNotification routine, access to the Unregistered
        // flag is not protected.  As such, even though the entry may be seen
        // as still registered, it may become unregistered prior to the
        // callback, thus allowing a callback to be invoked after an entry
        // has been unregistered.
        //

        if (((deviceSessionId == INVALID_SESSION) ||
             (entry->SessionId == deviceSessionId)) &&
            ((entry->Unregistered == FALSE) &&
             IopCompareGuid(&(entry->ClassGuid), ClassGuid))) {

            //
            // Dispatch the notification to the callback routine for the
            // appropriate session.  Ignore the returned result for non-query
            // type events.
            //

            dispatchStatus = PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                                     &notification,
                                                     &status);

            PNP_ASSERT(NT_SUCCESS(dispatchStatus));
        }

        ExReleaseResourceLite(entry->EntryLock);
        KeLeaveCriticalRegion();
        PnpAcquireNotifyLock(&PnpDeviceClassNotifyLock);
        link = link->Flink;
        PnpDereferenceNotify( (PNOTIFY_ENTRY_HEADER) entry );
    }

    PnpReleaseNotifyLock(&PnpDeviceClassNotifyLock);
    return STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoRegisterPlugPlayNotification(
    _In_ IO_NOTIFICATION_EVENT_CATEGORY EventCategory,
    _In_ ULONG EventCategoryFlags,
    _In_opt_ PVOID EventCategoryData,
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine,
    _Inout_opt_ __drv_aliasesMem PVOID Context,
    _Outptr_result_nullonfailure_
    _At_(*NotificationEntry,
        _When_(return==0, __drv_allocatesMem(Mem)))
    PVOID *NotificationEntry
    )

/*++

Routine Description:

    IoRegisterPlugPlayNotification provides a mechanism by which WDM drivers may
    receive notification (via callback) for a variety of Plug&Play events.

Arguments:

    EventCategory - Specifies the event category being registered for.  WDM drivers
        may currently register for hard-ware profile changes, device class changes
        (instance arrivals and removals), and target device changes (query-removal,
        cancel-removal, removal-complete, as well as 3rd-party extensible events).

    EventCategoryFlags - Supplies flags that modify the behavior of event registration.
        There is a separate group of flags defined for each event category.  Presently,
        only the interface device change event category has any flags defined:

            DEVICE_CLASS_NOTIFY_FOR_EXISTING_DEVICES -- Drivers wishing to retrieve a
                complete list of all interface devices presently available, and keep
                the list up-to-date (i.e., receive notification of interface device
                arrivals and removals), may specify this flag.  This will cause the
                PnP manager to immediately notify the driver about every currently-existing
                device of the specified interface class.

    EventCategoryData - Used to  'filter' events of the desired category based on the
        supplied criteria.  Not all event categories will use this parameter.  The
        event categories presently defined use this information as fol-lows:

        EventCategoryHardwareProfileChange -- this parameter is unused, and should be NULL.
        EventCategoryDeviceClassChange -- LPGUID representing the interface class of interest
        EventCategoryTargetDeviceChange -- PFILE_OBJECT of interest

    DriverObject - The caller must supply a reference to its driver object (obtained via
        ObReferenceObject), to prevent the driver from being unloaded while registered for
        notification.  The PnP Manager will dereference the driver object when the driver
        unregisters for notification via IoUnregisterPlugPlayNotification).

    CallbackRoutine - Entry point within the driver that the PnP manager should call
        whenever an applicable PnP event occurs.  The entry point must have the
        following prototype:

            typedef
            NTSTATUS
            (*PDRIVER_NOTIFICATION_CALLBACK_ROUTINE) (
                __in PVOID NotificationStructure,
                __in PVOID Context
                );

        where NotificationStructure contains information about the event.  Each event
        GUID within an event category may potentially have its own notification structure
        format, but the buffer must al-ways begin with a PLUGPLAY_NOTIFICATION_HEADER,
        which indicates the size and ver-sion of the structure, as well as the GUID for
        the event.

        The Context parameter provides the callback with the same context data that the
        caller passed in during registration.

    Context - Points to the context data passed to the callback upon event notification.

    NotificationEntry - Upon success, receives a handle representing the notification
        registration.  This handle may be used to unregister for notification via
        IoUnregisterPlugPlayNotification.

--*/

{

    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(NotificationEntry != NULL);

    //
    // Initialize out parameters
    //

    *NotificationEntry = NULL;

    //
    // Reference the driver object so it doesn't go away while we still have
    // a pointer outstanding
    //

    status = ObReferenceObjectByPointer(DriverObject,
                                        0,
                                        IoDriverObjectType,
                                        KernelMode);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    switch (EventCategory) {
    case EventCategoryHardwareProfileChange:
        {
            PHWPROFILE_NOTIFY_ENTRY entry;

            //
            // new entry
            //
            entry = ExAllocatePoolWithTag(PagedPool,
                                          sizeof(HWPROFILE_NOTIFY_ENTRY),
                                          PNP_POOLTAG_HW_PROFILE_NOTIFY);
            if (!entry) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean0;
            }

            //
            // Initialize the standard notify entry.
            //

            status = PnpInitializeNotifyEntry((PNOTIFY_ENTRY_HEADER)entry,
                                              EventCategory,
                                              CallbackRoutine,
                                              Context,
                                              DriverObject,
                                              &PnpHwProfileNotifyLock);
            if (!NT_SUCCESS(status)) {
                goto clean0;
            }

            //
            // Defer notification if neccessary.
            //

            status = PnpDeferNotification((PNOTIFY_ENTRY_HEADER)entry);
            if (!NT_SUCCESS(status)) {
                ExFreePoolWithTag(entry, PNP_POOLTAG_HW_PROFILE_NOTIFY);
                goto clean0;
            }
            //
            // Insert the entry into its list.
            //
            PnpLockedInsertTailList(&PnpProfileNotifyList,
                                    &PnpHwProfileNotifyLock,
                                    &entry->ListEntry);

            *NotificationEntry = entry;

            break;
        }
    case EventCategoryTargetDeviceChange:
        {
            PTARGET_DEVICE_NOTIFY_ENTRY entry;
            PDEVICE_NODE deviceNode;

            PNP_ASSERT(EventCategoryData);

            //
            // Retrieve the device object associated with this file handle.
            //

            status = PnpGetRelatedTargetDevice((PFILE_OBJECT)EventCategoryData,
                                               &deviceNode);
            if (!NT_SUCCESS(status)) {
                goto clean0;
            }

            //
            // The returned PDO is already referenced.
            //

            PNP_ASSERT(deviceNode->PhysicalDeviceObject);

            //
            // Allocate a new list entry
            //

            entry = ExAllocatePoolWithTag(PagedPool,
                                          sizeof(TARGET_DEVICE_NOTIFY_ENTRY),
                                          PNP_POOLTAG_TARGET_DEVICE_NOTIFY);
            if (!entry) {
                ObDereferenceObject(deviceNode->PhysicalDeviceObject);
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean0;
            }

            //
            // Initialize the standard notify entry.
            //

            status = PnpInitializeNotifyEntry((PNOTIFY_ENTRY_HEADER)entry,
                                              EventCategory,
                                              CallbackRoutine,
                                              Context,
                                              DriverObject,
                                              &PnpTargetDeviceNotifyLock);
            if (!NT_SUCCESS(status)) {
                ExFreePoolWithTag(entry, PNP_POOLTAG_TARGET_DEVICE_NOTIFY);
                ObDereferenceObject(deviceNode->PhysicalDeviceObject);
                goto clean0;
            }

            //
            // Initialize category specific information.
            //

            entry->FileObject = (PFILE_OBJECT)EventCategoryData;
            entry->PhysicalDeviceObject = deviceNode->PhysicalDeviceObject;
            status = PnpDeferNotification((PNOTIFY_ENTRY_HEADER)entry);
            if (!NT_SUCCESS(status)) {
                ExFreePoolWithTag(entry, PNP_POOLTAG_TARGET_DEVICE_NOTIFY);
                ObDereferenceObject(deviceNode->PhysicalDeviceObject);
                goto clean0;
            }

            //
            // Insert the entry into its list.
            //

            PnpLockedInsertTailList(&deviceNode->TargetDeviceNotify,
                                    &PnpTargetDeviceNotifyLock,
                                    &entry->ListEntry);
            *NotificationEntry = entry;
            break;
        }

    case EventCategoryDeviceInterfaceChange:
        {
            PDEVICE_CLASS_NOTIFY_ENTRY entry;
            ULONG DeviceSessionId = INVALID_SESSION;

            PNP_ASSERT(EventCategoryData);

            //
            // Allocate a new list entry
            //

            entry = ExAllocatePoolWithTag(PagedPool,
                                          sizeof(DEVICE_CLASS_NOTIFY_ENTRY),
                                          PNP_POOLTAG_DEVICE_CLASS_NOTIFY);
            if (!entry) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean0;
            }

            //
            // Initialize the standard notify entry.
            //

            status = PnpInitializeNotifyEntry((PNOTIFY_ENTRY_HEADER)entry,
                                              EventCategory,
                                              CallbackRoutine,
                                              Context,
                                              DriverObject,
                                              &PnpDeviceClassNotifyLock);
            if (!NT_SUCCESS(status)) {
                goto clean0;
            }

            entry->ClassGuid = *((LPGUID) EventCategoryData);

            //
            // Defer notification if needed.
            //

            status = PnpDeferNotification((PNOTIFY_ENTRY_HEADER)entry);
            if (!NT_SUCCESS(status)) {
                ExFreePoolWithTag(entry, PNP_POOLTAG_DEVICE_CLASS_NOTIFY);
                goto clean0;
            }

            //
            // Insert the entry into its list.
            //

            PnpLockedInsertTailList((PLIST_ENTRY)&PnpDeviceClassNotifyList[PnpHashGuid(&(entry->ClassGuid))],
                                    &PnpDeviceClassNotifyLock,
                                    &entry->ListEntry);

            //
            // See if we need to notify for all the device classes already present
            //

            if (EventCategoryFlags & PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES) {

                PWCHAR pSymbolicLinks, pCurrent;
                DEVICE_INTERFACE_CHANGE_NOTIFICATION notification;
                UNICODE_STRING unicodeString;

                //
                // Fill in the notification structure
                //

                notification.Version = PNP_NOTIFICATION_VERSION;
                notification.Size = sizeof(DEVICE_INTERFACE_CHANGE_NOTIFICATION);
                notification.Event = GUID_DEVICE_INTERFACE_ARRIVAL;
                notification.InterfaceClassGuid = entry->ClassGuid;

                //
                // Get the list of all the devices of this function class that are
                // already in the system
                //

                status = IoGetDeviceInterfaces(&(entry->ClassGuid),
                                                NULL,
                                                0,
                                                &pSymbolicLinks);
                if (!NT_SUCCESS(status)) {

                    //
                    // No buffer will have been returned so just return status
                    //

                    goto clean0;
                }

                //
                // Callback for each device currently in the system
                //

                pCurrent = pSymbolicLinks;
                while(*pCurrent != UNICODE_NULL) {

                    NTSTATUS dispatchStatus, tempStatus;

                    RtlInitUnicodeString(&unicodeString, pCurrent);
                    notification.SymbolicLinkName = &unicodeString;

                    //
                    // Per Session devices - check entry's SessionId
                    //
                    if (entry->SessionId != SERVICE_SESSION) {
                        DeviceSessionId = IopGetSessionIdFromSymbolicName(notification.SymbolicLinkName);
                        //
                        // Check if caller and device belong to same session id - only then notify
                        //
                        if ((DeviceSessionId != INVALID_SESSION) && (entry->SessionId != DeviceSessionId)) {
                            //
                            // Advance down the list
                            //
                            IopDbgPrint((IOP_TRACE_LEVEL, "EventCategoryDeviceInterfaceChange: DeviceSessionId = %d, CallerSessionId = %d, hence filtering this \n\n", DeviceSessionId, entry->SessionId));
                            pCurrent += (unicodeString.Length / sizeof(WCHAR)) + 1;
                            continue;
                        }
                    }

                    //
                    // Dispatch the notification to the callback routine for the
                    // appropriate session.  Ignore the returned result for non-query
                    // type events.
                    //

                    dispatchStatus = PnpNotifyDriverCallback((PNOTIFY_ENTRY_HEADER)entry,
                                                             &notification,
                                                             &tempStatus);


                    PNP_ASSERT(NT_SUCCESS(dispatchStatus));
                    pCurrent += (unicodeString.Length / sizeof(WCHAR)) + 1;

                }

                ExFreePool(pSymbolicLinks);
            }

            *NotificationEntry = entry;
        }

        break;

    case EventCategoryReserved:

        //
        // Fall through...
        //

    default:
        status = STATUS_INVALID_PARAMETER_1;
        break;
    }

clean0:

    if (!NT_SUCCESS(status)) {
        ObDereferenceObject(DriverObject);
    }

    return status;
}

NTSTATUS
PnpGetRelatedTargetDevice (
    __in PFILE_OBJECT FileObject,
    __out PDEVICE_NODE *DeviceNode
    )

/*++

Routine Description:

    PnpGetRelatedTargetDevice retrieves the device object associated with
    the specified file object and then sends a query device relations irp
    to that device object.

    NOTE: The PDO associated with the returned device node has been referenced,
    and must be dereferenced when no longer needed.

Arguments:

    FileObject - Specifies the file object that is associated with the device
                 object that will receive the query device relations irp.

    DeviceNode - Returns the related target device node.

ReturnValue

    Returns an NTSTATUS value.

--*/

{
    NTSTATUS status;
    IO_STACK_LOCATION irpSp;
    PDEVICE_OBJECT deviceObject, targetDeviceObject;
    PDEVICE_RELATIONS deviceRelations;
    PDEVICE_NODE targetDeviceNode;
    KIRQL irql;

    //
    // This routine is NONPAGED because it acquires the spinlock below.
    // However, its expected to be called at PASSIVE_LEVEL.
    //

    PNP_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    PNP_ASSERT(FileObject);

    *DeviceNode = NULL;

    //
    // Retrieve the device object associated with this file handle.
    //

    irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    deviceObject = IoGetRelatedDeviceObject(FileObject);
    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    if (!deviceObject) {
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Query what the "actual" target device node should be for
    // this file object. Initialize the stack location to pass to
    // IopSynchronousCall() and then send the IRP to the device
    // object that's associated with the file handle.
    //

    PnpInitializeStackLocation(&irpSp, IRP_MN_QUERY_DEVICE_RELATIONS);

    //
    // Set the parameters for this relation type.
    //

    irpSp.Parameters.QueryDeviceRelations.Type = TargetDeviceRelation;
    irpSp.DeviceObject = deviceObject;
    irpSp.FileObject = FileObject;
    status = PnpSendIrp(deviceObject,
                        &irpSp,
                        0,
                        NULL,
                        &deviceRelations);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    PNP_ASSERT(deviceRelations);

    if (deviceRelations) {

        PNP_ASSERT(deviceRelations->Count == 1);

        if (deviceRelations->Count == 1) {
            targetDeviceObject = deviceRelations->Objects[0];

        } else {
            targetDeviceObject = NULL;
        }

        ExFreePool(deviceRelations);
        if (targetDeviceObject) {
            targetDeviceNode =
                         targetDeviceObject->DeviceObjectExtension->DeviceNode;

            if (targetDeviceNode) {
                *DeviceNode = targetDeviceNode;
                return status;
            }
        }
    }

    return STATUS_NO_SUCH_DEVICE;
}

NTSTATUS
IoGetRelatedTargetDevice (
    __in PFILE_OBJECT FileObject,
    __out PDEVICE_OBJECT *DeviceObject
    )

/*++

Routine Description:

    IoGetRelatedTargetDevice retrieves the device object associated with
    the specified file object and then sends a query device relations irp
    to that device object.

    NOTE: The PDO associated with the returned device node has been referenced,
    and must be dereferenced when no longer needed.

Arguments:

    FileObject - Specifies the file object that is associated with the device
                 object that will receive the query device relations irp.

    DeviceObject - Returns the related target device object.

ReturnValue

    Returns an NTSTATUS value.

--*/

{
    NTSTATUS status;
    PDEVICE_NODE deviceNode = NULL;

    PAGED_CODE();

    status = PnpGetRelatedTargetDevice(FileObject, &deviceNode);
    if (NT_SUCCESS(status) && deviceNode != NULL) {

        *DeviceObject = deviceNode->PhysicalDeviceObject;
    }
    return status;
}

NTSTATUS
IoNotifyPowerOperationVetoed (
    __in POWER_ACTION VetoedPowerOperation,
    __in_opt PDEVICE_OBJECT TargetedDeviceObject,
    __in PDEVICE_OBJECT VetoingDeviceObject
    )

/*++

Routine Description:

    This routine is called by the power subsystem to initiate user-mode
    notification of vetoed system power events.  The power events are submitted
    into a serialized asynchronous queue.  This queue is processed by a work
    item.  This routine does not wait for the event to be processed.

Parameters:

    VetoedPowerOperation - Specifies the system-wide power action that was
                           vetoed.

    TargetedDeviceObject - Optionally, supplies the device object target of the
                           vetoed operation.

    VetoingDeviceObject  - Specifies the device object responsible for vetoing
                           the power operation.

Return Value:

    Status code that indicates whether or not the event was successfully
    inserted into the asynchronous event queue..

--*/
{
    PDEVICE_NODE deviceNode, vetoingDeviceNode;
    PDEVICE_OBJECT deviceObject;

    PAGED_CODE();

    //
    // We have two types of power events, system wide (standby) and device
    // targetted (warm eject). Rather than have two different veto mechanisms,
    // we just retarget the operation against the root device if none is
    // specified (hey, someone's gotta represent the system, right?).
    //
    if (TargetedDeviceObject) {

        deviceObject = TargetedDeviceObject;

    } else {

        deviceObject = IopRootDeviceNode->PhysicalDeviceObject;
    }

    deviceNode = (PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode;
    if (!deviceNode) {
        return STATUS_INVALID_PARAMETER_2;
    }

    vetoingDeviceNode = (PDEVICE_NODE)VetoingDeviceObject->DeviceObjectExtension->DeviceNode;
    if (!vetoingDeviceNode) {
        return STATUS_INVALID_PARAMETER_3;
    }

    return PnpSetPowerVetoEvent(VetoedPowerOperation,
                                NULL,
                                NULL,
                                deviceObject,
                                PNP_VetoDevice,
                                PnpGetDeviceInstancePath(vetoingDeviceNode));
}

VOID
PnpOrphanNotification (
    __in PDEVICE_NODE TargetNode
    )

/*++

Routine Description:

    This routine releases the references to the device object for all the
    notifications entries of a device object, then fixes up the notification
    node to not point to a physical device object.

Parameters:

    TargetNode - Specifies the device node whose registered target device
                 notification recipients are to be orphaned.

Return Value:

    None.

Notes:

    The notification node will be released when IoUnregisterPlugPlayNotification
    is actually called, but the device object will already be gone.

--*/

{
    PTARGET_DEVICE_NOTIFY_ENTRY entry;

    PAGED_CODE();

    PnpAcquireNotifyLock(&PnpTargetDeviceNotifyLock);

    while (!IsListEmpty(&TargetNode->TargetDeviceNotify)) {
        //
        // Remove all target device change notification entries for this devnode
        //
        entry = (PTARGET_DEVICE_NOTIFY_ENTRY)
            RemoveHeadList(&TargetNode->TargetDeviceNotify);

        PNP_ASSERT(entry->EventCategory == EventCategoryTargetDeviceChange);
        //
        // Re-initialize the orphaned list entry so we don't attempt to remove
        // it from the list again.
        //
        InitializeListHead((PLIST_ENTRY)entry);
        //
        // Dereference the target device object, and NULL it out so we don't
        // attempt to dereference it when the entry is actually unregistered.
        //
        if (entry->PhysicalDeviceObject) {

            ObDereferenceObject(entry->PhysicalDeviceObject);
            entry->PhysicalDeviceObject = NULL;
        }
    }

    PnpReleaseNotifyLock(&PnpTargetDeviceNotifyLock);
    return;
}

NTSTATUS
PnpNotifyDriverCallback (
    __in PNOTIFY_ENTRY_HEADER NotifyEntry,
    __in PVOID NotificationStructure,
    __out_opt PNTSTATUS CallbackStatus
    )
/*++

Routine Description:

    This routine dispatches a plug and play notification event to a specified
    callback routine.

    If the callback routine specifies an address outside of session space, or if
    the calling process is already in the context of the specified session, it
    will call the callback routine directly.

    Otherwise, this routine will attempt to attach to the specified session and
    call the callback routine.

Parameters:

    CallbackRoutine - Entry point within the driver that will be called with
                      information about the event that has occured.

    NotificationStructure - Contains information about the event.

    Context         - Points to the context data supplied at registration.

    SessionId       - Specifies the ID of the session in which the specified
                      callback is to be called.

    OpqueSession    - Optionally, specifies the opaque handle to the session that
                      to attach to when the specified callback is called.

    CallbackStatus  - Optionally, supplies the address of a variable to receive
                      the NTSTATUS code returned by the callback routine.

Return Value:

    Status code that indicates whether or not the function was successful.

Notes:

    Returns STATUS_NOT_FOUND if the specified session was not found.

--*/
{
    KAPC_STATE apcState;
    NTSTATUS callStatus;
    ULONG combinedApcDisable;
    KIRQL irql;
    KIRQL newIrql;
    ULONG newCombinedApcDisable;
    PVOID opaqueSession;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(NotifyEntry);
    PNP_ASSERT(NotifyEntry->CallbackRoutine);
    PNP_ASSERT(NotificationStructure);

    //
    // For per-session driver, make sure there is a valid session handle.
    //

    if (MmIsSessionAddress((PVOID)NotifyEntry->CallbackRoutine) &&
        NotifyEntry->SessionHandle == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    irql = 0;
    newIrql = 0;
    combinedApcDisable = 0;
    newCombinedApcDisable = 0;
    if (!MmIsSessionAddress((PVOID)NotifyEntry->CallbackRoutine) ||
        ((PsGetCurrentProcess()->Flags & PS_PROCESS_FLAGS_IN_SESSION) &&
            NotifyEntry->SessionId == PsGetCurrentProcessSessionId())) {

        //
        // Notification to a non-session driver or currently
        // in the context of session specific driver's session.
        //

        IopDbgPrint((IOP_IOEVENT_TRACE_LEVEL,
                     "PnpNotifyDriverCallback: Calling notification "
                     "callback @ 0x%p directly...\n",
                     NotifyEntry->CallbackRoutine));

        //
        // Remember the IRQL and combined APC disable count before
        // the callout.
        //

        irql = KeGetCurrentIrql();
        combinedApcDisable = KeGetCurrentThread()->CombinedApcDisable;
        callStatus = (NotifyEntry->CallbackRoutine)(NotificationStructure,
                                                    NotifyEntry->Context);
        newIrql = KeGetCurrentIrql();
        newCombinedApcDisable = KeGetCurrentThread()->CombinedApcDisable;
        if (ARGUMENT_PRESENT(CallbackStatus)) {
            *CallbackStatus = callStatus;
        }

        status = STATUS_SUCCESS;

    } else {
        PNP_ASSERT(NotifyEntry->SessionHandle != NULL);

        //
        // Notification to a session driver.
        //

        opaqueSession = MmGetSessionById(NotifyEntry->SessionId);
        if (opaqueSession != NULL) {
            status = MmAttachSession(opaqueSession,
                                     &apcState);
            if (NT_SUCCESS(status)) {

                IopDbgPrint((IOP_IOEVENT_TRACE_LEVEL,
                             "PnpNotifyDriverCallback: Calling per-session notification "
                             "callback @ 0x%p for SessionId %d...\n",
                             NotifyEntry->CallbackRoutine,
                             NotifyEntry->SessionId));

                //
                // Remember the IRQL and combined APC disable count before
                // the callout.
                //

                irql = KeGetCurrentIrql();
                combinedApcDisable = KeGetCurrentThread()->CombinedApcDisable;
                callStatus = (NotifyEntry->CallbackRoutine)(NotificationStructure,
                                                            NotifyEntry->Context);
                newIrql = KeGetCurrentIrql();
                newCombinedApcDisable = KeGetCurrentThread()->CombinedApcDisable;
                if (ARGUMENT_PRESENT(CallbackStatus)) {
                    *CallbackStatus = callStatus;
                }

                status = MmDetachSession(opaqueSession,
                                         &apcState);
            }

            MmQuitNextSession(opaqueSession);

        } else {
            status = STATUS_UNSUCCESSFUL;
        }
    }

    if ((irql != newIrql) || (combinedApcDisable != newCombinedApcDisable)) {

        //
        // Enforce correct driver notification callback behavior.
        //

        PNP_SAVE_DRIVEROBJECT_TO_TRIAGE_DUMP(NotifyEntry->DriverObject);
        KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                     PNP_ERR_INCORRECT_NOTIFY_CALLBACK_BEHAVIOR,
                     (ULONG_PTR)NotifyEntry->DriverObject,
                     newIrql,
                     newCombinedApcDisable);
    }

    return status;
}

NTSTATUS
PnpInitializeNotifyEntry (
    __inout PNOTIFY_ENTRY_HEADER NotifyEntry,
    __in IO_NOTIFICATION_EVENT_CATEGORY EventCategory,
    __in PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine,
    __in_opt PVOID Context,
    __in PDRIVER_OBJECT DriverObject,
    __in_opt PKGUARDED_MUTEX Lock
    )

/*++

Routine Description:

    This function will initialize the standard notify entry header.

Arguments:

    NotifyEntry - Supplies a pointer to the notify entry to be initialized.

    EventCategory - Supplies the event category.

    CallbackRoutine - Supplies a pointer to the driver's callback.

    Context - Supplies a pointer to the driver's callback context.

    DriverObject - Supplies a pointer to the driver object. Caller is
        expected to have already referenced this object.

    Lock - Supplies a pointer to the lock for this notify entry.

Return Value:

    NTSTATUS.

--*/

{
    OBJECT_ATTRIBUTES objectAttributes;
    ULONG sessionId;
    WCHAR sessionName[256];
    UNICODE_STRING unicodeSessionName;
    HANDLE sessionHandle;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(NotifyEntry);
    PNP_ASSERT(CallbackRoutine);
    PNP_ASSERT(DriverObject);

    sessionId = 0;
    sessionHandle = NULL;
    status = STATUS_SUCCESS;
    if (MmIsSessionAddress((PVOID)CallbackRoutine)) {

        //
        // Get session information if a per session driver
        // is registering for notifications.
        //

        sessionId = MmGetSessionIdEx(PsGetCurrentProcess());
        PNP_ASSERT(sessionId != (ULONG)-1);
        if (sessionId == (ULONG)-1) {
            status = STATUS_INVALID_PARAMETER;
            goto InitializeNotifyEntryEnd;
        }

        //
        // Open and cache a handle to the kernel session object.
        // This prevents the session id from being recycled
        // while its cached in the notify entry.
        //

        swprintf_s(sessionName,
                   ARRAYSIZE(sessionName),
                   SESSION_OBJECT_NAME,
                   sessionId);
        RtlInitUnicodeString(&unicodeSessionName, sessionName);
        InitializeObjectAttributes(&objectAttributes,
                                   &unicodeSessionName,
                                   OBJ_KERNEL_HANDLE,
                                   NULL,
                                   NULL);
        status = ZwOpenSession(&sessionHandle,
                               0,
                               &objectAttributes);
        if (!NT_SUCCESS(status) || sessionHandle == NULL) {
            status = STATUS_INVALID_PARAMETER;
            goto InitializeNotifyEntryEnd;
        }
    }

    InitializeListHead(&NotifyEntry->ListEntry);
    NotifyEntry->EventCategory = EventCategory;
    NotifyEntry->SessionId = sessionId;
    NotifyEntry->SessionHandle = sessionHandle;
    NotifyEntry->CallbackRoutine = CallbackRoutine;
    NotifyEntry->Context = Context;
    NotifyEntry->DriverObject = DriverObject;
    NotifyEntry->RefCount = 1;
    NotifyEntry->Unregistered = FALSE;
    NotifyEntry->Lock = Lock;
    NotifyEntry->EntryLock =
        (PERESOURCE)ExAllocatePoolWithTag(NonPagedPoolNx,
                                          sizeof(ERESOURCE),
                                          PNP_POOLTAG_NOTIFY_ENTRY_LOCK);

    if (NotifyEntry->EntryLock == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto InitializeNotifyEntryEnd;
    }

    ExInitializeResourceLite(NotifyEntry->EntryLock);

InitializeNotifyEntryEnd:
    return status;
}

