/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    notify.h

Abstract:

    This module implements PNP device notifications.

Author:

    Santosh Jodh (santoshj) 08-09-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// Kernel session object name format.
//

#define SESSION_OBJECT_NAME TEXT("\\KernelObjects\\Session%d")

#define PNP_NOTIFICATION_VERSION            1
#define NOTIFY_DEVICE_CLASS_HASH_BUCKETS    13

//
// Notify entry header - all notify entries have these
//

typedef struct _NOTIFY_ENTRY_HEADER {

    //
    // List Entry structure
    //

    LIST_ENTRY ListEntry;

    //
    // Notification event category for this notification entry.
    //

    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;

    //
    // SessionId.
    //
    ULONG SessionId;

    //
    // Session space object to attach to for sending notification.
    //
    HANDLE SessionHandle;

    //
    // Callback routine passed in at registration
    //

    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;

    //
    // Context passed in at registration
    //

    PVOID Context;

    //
    // Driver object of the driver that registered for notifications.  Required
    // so we can dereference it when it unregisters
    //

    PDRIVER_OBJECT DriverObject;

    //
    // RefCount is the number of outstanding pointers to the node and avoids
    // deletion while another notification is taking place
    //

    USHORT RefCount;

    //
    // Unregistered is set if this notification has been unregistered but cannot
    // be removed from the list because other entities are using it
    //

    BOOLEAN Unregistered;

    //
    // Lock is a pointer to the fast mutex which is used to synchronise access
    // to the list this node is a member of and is required so that the correct
    // list can be locked during IoUnregisterPlugPlayNotification.  If no locking
    // is required it is NULL
    //

    PKGUARDED_MUTEX Lock;

    //
    // EntryLock is an executive resource used to synchronize between a 
    // notification callback and unregistration.  It is specific to this
    // notification entry.
    // 

    PERESOURCE EntryLock;

} NOTIFY_ENTRY_HEADER, *PNOTIFY_ENTRY_HEADER;

//
// Data to store for each target device registration
//

typedef struct _TARGET_DEVICE_NOTIFY_ENTRY {

    //
    // Standard notify entry header.
    //

    NOTIFY_ENTRY_HEADER;

    //
    // FileObject - the file object of the target device we are interested in
    //

    PFILE_OBJECT FileObject;

    //
    // PhysicalDeviceObject -- the PDO upon which this notification is hooked.
    // We need to keep this here, so we can dereference it when the refcount
    // on this notification entry drops to zero.
    //

    PDEVICE_OBJECT PhysicalDeviceObject;

} TARGET_DEVICE_NOTIFY_ENTRY, *PTARGET_DEVICE_NOTIFY_ENTRY;

//
// Data to store for each device class registration
//

typedef struct _DEVICE_CLASS_NOTIFY_ENTRY {

    //
    // Standard notify entry header.
    //

    NOTIFY_ENTRY_HEADER;

    //
    // ClassGuid - the guid of the device class we are interested in
    //

    GUID ClassGuid;

} DEVICE_CLASS_NOTIFY_ENTRY, *PDEVICE_CLASS_NOTIFY_ENTRY;

//
// Data to store for registration for HardwareProfileChange Events
//

typedef struct _HWPROFILE_NOTIFY_ENTRY {

    //
    // Standard notify entry header.
    //

    NOTIFY_ENTRY_HEADER;

} HWPROFILE_NOTIFY_ENTRY, *PHWPROFILE_NOTIFY_ENTRY;

typedef struct _ASYNC_TDC_WORK_ITEM {

    WORK_QUEUE_ITEM WorkItem;
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_CHANGE_COMPLETE_CALLBACK Callback;
    PVOID Context;
    PTARGET_DEVICE_CUSTOM_NOTIFICATION NotificationStructure;

}   ASYNC_TDC_WORK_ITEM, *PASYNC_TDC_WORK_ITEM;

typedef struct _DEFERRED_REGISTRATION_ENTRY {

    LIST_ENTRY            ListEntry;
    PNOTIFY_ENTRY_HEADER  NotifyEntry;

} DEFERRED_REGISTRATION_ENTRY, *PDEFERRED_REGISTRATION_ENTRY;

//
// Locks the list and inserts the entry at the tail.
//
#define PnpLockedInsertTailList(list, lock, entry) { \
    PnpAcquireNotifyLock((lock));                   \
    InsertTailList((list), (entry));                \
    PnpReleaseNotifyLock((lock));                   \
}

//
// This macros maps a guid to a hash value based on the number of hash
// buckets we are using.  It does this by treating the  guid as an array of
// 4 ULONGs, suming them and MOD by the number of hash buckets we are using.
//
//  ULONG
//  PnpHashGuid(
//      LPGUID Guid
//      );
//

#define PnpHashGuid(_Guid) \
            ( ( ((PULONG)_Guid)[0] + ((PULONG)_Guid)[1] + ((PULONG)_Guid)[2] \
                + ((PULONG)_Guid)[3]) % NOTIFY_DEVICE_CLASS_HASH_BUCKETS)

//
// These macros abstract the notify lock routines.
//

#define PnpAcquireNotifyLock(Lock)     KeAcquireGuardedMutex(Lock);
#define PnpReleaseNotifyLock(Lock)     KeReleaseGuardedMutex(Lock);

VOID
PnpInitializeNotification (
    VOID
    );

NTSTATUS
PnpRequestHwProfileChangeNotification (
    __in const GUID* EventGuid,
    __in PROFILE_NOTIFICATION_TIME NotificationTime,
    __out_opt PPNP_VETO_TYPE VetoType,
    __out_opt PUNICODE_STRING VetoName
    );

NTSTATUS
PnpNotifyTargetDeviceChange (
    __in const GUID* EventGuid,
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PTARGET_DEVICE_CUSTOM_NOTIFICATION NotificationStructure,
    __out_opt PDRIVER_OBJECT* VetoingDriver
    );

NTSTATUS
PnpNotifyDeviceClassChange (
    __in GUID* EventGuid,
    __in GUID* ClassGuid,
    __in PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
PnpNotifyHwProfileChange (
    __in GUID* EventGuid,
    __out_opt PPNP_VETO_TYPE VetoType,
    __out_opt PUNICODE_STRING VetoName
    );

VOID
PnpOrphanNotification (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpProcessDeferredRegistrations (
    VOID
    );

NTSTATUS
PnpGetRelatedTargetDevice (
    __in PFILE_OBJECT FileObject,
    __out PDEVICE_NODE *DeviceNode
    );

VOID
PnpDereferenceNotify (
    __in PNOTIFY_ENTRY_HEADER Notify
    );

NTSTATUS
PnpNotifyDriverCallback (
    __in PNOTIFY_ENTRY_HEADER NotifyEntry,
    __in PVOID NotificationStructure,
    __out_opt PNTSTATUS CallbackStatus
    );

VOID
PnpReferenceNotify (
    __in PNOTIFY_ENTRY_HEADER Notify
    );

VOID
PnpReportTargetDeviceChangeAsyncWorker (
    __in PVOID Context
    );

NTSTATUS
PnpDeferNotification (
    __in PNOTIFY_ENTRY_HEADER Entry
    );

NTSTATUS
PnpInitializeNotifyEntry (
    __inout PNOTIFY_ENTRY_HEADER NotifyEntry,
    __in IO_NOTIFICATION_EVENT_CATEGORY EventCategory,
    __in PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine,
    __in_opt PVOID Context,
    __in PDRIVER_OBJECT DriverObject,
    __in_opt PKGUARDED_MUTEX Lock
    );
