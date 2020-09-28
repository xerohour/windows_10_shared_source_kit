/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    queue.h

Abstract:

    This module contains the plug-and-play queues.

Author:

    Santosh Jodh (santoshj) 19-Feb-2003

Environment:

    Kernel mode


Revision History:

--*/

#pragma once

//
// This queue is used to support asynchronous handling of PNP IRPS.
// Currently, we only support asynchronous handling of:
// 1. IRP_MN_START_DEVICE and
// 2. IRP_MN_QUERY_DEVICE_RELATIONS - BusRelations.
//
typedef struct _PNP_DEVICE_COMPLETION_QUEUE PNP_DEVICE_COMPLETION_QUEUE, *PPNP_DEVICE_COMPLETION_QUEUE;

struct _PNP_DEVICE_COMPLETION_QUEUE {

    LIST_ENTRY  DispatchedList;     // List of operations in progress.
    ULONG       DispatchedCount;    // Number of operations in progress but not completed.

    LIST_ENTRY  CompletedList;      // List of completed operations.
    KSEMAPHORE  CompletedSemaphore; // Tracks number of completed operations.

    KSPIN_LOCK  SpinLock;           // Lock for accessing queue members.
};

TRIAGE_SIZE_CHECK(PNP_DEVICE_COMPLETION_QUEUE);
TRIAGE_FIELD_CHECK(PNP_DEVICE_COMPLETION_QUEUE, DispatchedList);

KINLINE
ULONG
PnpDeviceCompletionQueueGetDispatchedCount (
    __in PPNP_DEVICE_COMPLETION_QUEUE Queue
    )

/*++

Description

    This function returns the number of asynchronous PNP IRPs currently dispatched (IRP
    pending).

Parameters

    Queue - Pointer to the queue containing the dispatched IRPs.

Return

    Number of asynchronous PNP IRPs currently dispatched.

--*/

{
    return Queue->DispatchedCount;
}

KINLINE
ULONG
PnpDeviceCompletionQueueGetCompletedCount (
    __in PPNP_DEVICE_COMPLETION_QUEUE Queue
    )

/*++

Description

    This function returns the number of completed asynchronous PNP IRPs.

Parameters

    Queue - Pointer to the queue containing the completed IRPs.

Return

    Number of completed asynchronous PNP IRPs.

--*/

{
    return KeReadStateSemaphore(&Queue->CompletedSemaphore);
}

KINLINE
VOID
PnpDeviceCompletionQueueRemoveDispatchedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     DispatchedEntry
    )

/*++

Description

    This function removes the specified DispatchedEntry from the completion queue. This
    does not acquire the queue lock (caller is expected to acquire the lock before calling
    this).

Parameters

    Queue - Pointer to the completion queue.

    DispatchedEntry - Pointer to the entry to be removed.

Return

    None.

--*/

{
    PNP_ASSERT(!IsListEmpty(&Queue->DispatchedList));
    RemoveEntryList(DispatchedEntry);

    PNP_ASSERT(Queue->DispatchedCount);
    Queue->DispatchedCount--;

    return;
}

KINLINE
LOGICAL
PnpDeviceCompletionQueueAddCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     CompletedEntry
    )

/*++

Description

    This function adds the specified completed entry to the completion queue. This
    does not acquire the queue lock (caller is expected to acquire the lock before calling
    this).

Parameters

    Queue - Pointer to the completion queue.

    CompletedEntry - Pointer to the entry to be added.

Return

    TRUE iff the complete list was initially empty. FALSE otherwise..

--*/

{
    InsertTailList(&Queue->CompletedList, CompletedEntry);

    return KeReleaseSemaphore(&Queue->CompletedSemaphore, IO_NO_INCREMENT, 1, FALSE) ? FALSE : TRUE;
}

NTSTATUS
PnpDeviceCompletionQueueInitialize (
    __inout PPNP_DEVICE_COMPLETION_QUEUE Queue
    );

LOGICAL
PnpDeviceCompletionQueueIsEmpty (
    __in PPNP_DEVICE_COMPLETION_QUEUE Queue
    );

LOGICAL
PnpDeviceCompletionQueueAddDispatchedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     DispatchedEntry
    );

PLIST_ENTRY
PnpDeviceCompletionQueueRemoveCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     CompletedEntry
    );

PLIST_ENTRY
PnpDeviceCompletionQueueGetCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue
    );

VOID
PnpDeviceCompletionQueueDispatchedEntryCompleted (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     DispatchedEntry
    );
