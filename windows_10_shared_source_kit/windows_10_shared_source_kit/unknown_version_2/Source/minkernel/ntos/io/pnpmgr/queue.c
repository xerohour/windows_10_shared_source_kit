/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    queue.c

Abstract:

    This module contains the plug-and-play queues.
    Following queues are supported:
        DeviceCompletionQueue - used for asynchronous PnP operations.

Author:

    Santosh Jodh (santoshj) 19-Feb-2003

Environment:

    Kernel mode


Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#ifdef ALLOC_PRAGMA

#pragma alloc_text(INIT, PnpDeviceCompletionQueueInitialize)

//#pragma alloc_text(NONPAGED, PnpDeviceCompletionQueueIsEmpty)
//#pragma alloc_text(NONPAGED, PnpDeviceCompletionQueueAddDispatchedRequest)
//#pragma alloc_text(NONPAGED, PnpDeviceCompletionQueueRemoveCompletedRequest)
//#pragma alloc_text(NONPAGED, PnpDeviceCompletionQueueGetCompletedRequest)
//#pragma alloc_text(NONPAGED, PnpDeviceCompletionQueueDispatchedEntryCompleted)

#endif // ALLOC_PRAGMA


NTSTATUS
PnpDeviceCompletionQueueInitialize (
    __inout PPNP_DEVICE_COMPLETION_QUEUE Queue
    )
/*++

Routine description:

    This routine initializes the specified PNP_DEVICE_COMPLETION_QUEUE.

Parameters:

    Queue - pointer to the queue to be initialized.

Return value:

    NTSTATUS.

--*/
{
    PAGED_CODE();

    PNP_ASSERT(Queue);

    KeInitializeSpinLock(&Queue->SpinLock);

    Queue->DispatchedCount = 0;
    InitializeListHead(&Queue->DispatchedList);

    KeInitializeSemaphore(&Queue->CompletedSemaphore, 0, MAXLONG);
    InitializeListHead(&Queue->CompletedList);

    return STATUS_SUCCESS;
}

LOGICAL
PnpDeviceCompletionQueueIsEmpty (
    __in PPNP_DEVICE_COMPLETION_QUEUE Queue
    )
{
    KIRQL irql;
    LOGICAL result;

    PNP_ASSERT(Queue);

    result = FALSE;

    KeAcquireSpinLock(&Queue->SpinLock, &irql);

    if (IsListEmpty(&Queue->DispatchedList)) {

        if (IsListEmpty(&Queue->CompletedList)) {

            result = TRUE;
        }
    }

    KeReleaseSpinLock(&Queue->SpinLock, irql);

    return result;
}

LOGICAL
PnpDeviceCompletionQueueAddDispatchedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     DispatchedEntry
    )
/*++

Routine description:

    This routine enqueues a dispatched entry.

Parameters:

    Queue - queue where the entry will be enqueued.

    DispatchedEntry - entry to be enqueued.

Return value:

    TRUE if this is the first entry being enqueued, else FALSE.

--*/
{
    LOGICAL dispatchedListWasEmpty;
    KIRQL irql;

    PNP_ASSERT(Queue);
    PNP_ASSERT(DispatchedEntry);

    KeAcquireSpinLock(&Queue->SpinLock, &irql);

    if (IsListEmpty(&Queue->DispatchedList)) {

        dispatchedListWasEmpty = TRUE;
    } else {

        dispatchedListWasEmpty = FALSE;
    }

    InsertTailList(&Queue->DispatchedList, DispatchedEntry);
    Queue->DispatchedCount++;

    KeReleaseSpinLock(&Queue->SpinLock, irql);

    return dispatchedListWasEmpty;
}

PLIST_ENTRY
PnpDeviceCompletionQueueRemoveCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     CompletedEntry
    )

/*++

Description

    This function will remove the completed entry from the queue.

Parameters

    Queue - Pointer to the completion queue.

    CompletedEntry - Pointer to the entry to be removed.

Return

    Pointer to the completed entry.

--*/

{
    KIRQL irql;

    PNP_ASSERT(Queue);

    KeWaitForSingleObject(&Queue->CompletedSemaphore,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    KeAcquireSpinLock(&Queue->SpinLock, &irql);

    PNP_ASSERT(!IsListEmpty(&Queue->CompletedList));

    RemoveEntryList(CompletedEntry);

    KeReleaseSpinLock(&Queue->SpinLock, irql);

    return CompletedEntry;
}

PLIST_ENTRY
PnpDeviceCompletionQueueGetCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue
    )

/*++

Description

    This function will remove the first completed entry from the queue. It will block
    until there is at least one completed entry.

Parameters

    Queue - Pointer to the completion queue.

Return

    Pointer to the completed entry.

--*/

{
    PLIST_ENTRY entry;
    KIRQL irql;

    PNP_ASSERT(Queue);

    KeWaitForSingleObject(&Queue->CompletedSemaphore,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    KeAcquireSpinLock(&Queue->SpinLock, &irql);

    PNP_ASSERT(!IsListEmpty(&Queue->CompletedList));

    entry = RemoveHeadList(&Queue->CompletedList);

    KeReleaseSpinLock(&Queue->SpinLock, irql);

    return entry;
}


VOID
PnpDeviceCompletionQueueDispatchedEntryCompleted (
    __inout PPNP_DEVICE_COMPLETION_QUEUE    Queue,
    __inout PLIST_ENTRY                     DispatchedEntry
    )

/*++

Description

    This function is called to move the entry from the dispatched to the completed list
    in the queue.

Parameters

    Queue - Pointer to the completion queue.

    DispatchedEntry - Pointer to the entry to be moved.

Return

    None.

--*/

{
    KIRQL irql;

    PNP_ASSERT(Queue);
    PNP_ASSERT(DispatchedEntry);

    KeAcquireSpinLock(&Queue->SpinLock, &irql);

    PnpDeviceCompletionQueueRemoveDispatchedRequest(Queue, DispatchedEntry);

    PnpDeviceCompletionQueueAddCompletedRequest(Queue, DispatchedEntry);

    KeReleaseSpinLock(&Queue->SpinLock, irql);

    return;
}
