/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    connlock.c

Abstract:

    This module implements locking routines required to serialize interrupt
    connect and disconnect requests on a given GSIV.

    Need for serialization:

    When multiple drivers try to connect their interrupts, the first request
    would lead to enabling the interrupt and subsequent ones may lead to
    unmasking of the interrupt. Since the enable and unmask requests are issued
    at passive-level, the unmask request could race ahead of the enable and
    fail. Similarly, trying to disconnect an interrupt may result in the
    interrupt line being masked first. However, such a request could race ahead
    of the (previous) enable request and fail. The interrupt itself could get
    enabled immediately after the mask request failed. Progressing in such
    scenarios could lead to failures and may not be safe (e.g. list corruption
    etc.) as it breaks assumptions in the code. Hence connect and disconnect
    requests on a given GSIV need to be serialized.

    Currently this serialization is only required for interrupt controller
    inputs and not required for message-signalled interrupts. For
    performance optimization, message-signalled interrupts are neither
    synchronized against each other nor put on the global connect list.

    Note this infrastructure is built to be common across primary interrupts
    (i.e. interrupt lines connected behind IO-APIC or GIC) as well as secondary
    interrupts (e.g. lines behind a GPIO controller).

Author:

    Murali Ravirala (muralir) 08/02/2011

Environment:

    Kernel mode only.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "pnpmgrp.h"
#include "hal.h"

//
// -------------------------------------------------------------------- Defines
//

#define ACTIVE_CONNECT_FLAGS_NONE (0x0)
#define ACTIVE_CONNECT_FLAGS_LOCAL_LOCK (0x1)
#define ACTIVE_CONNECT_FLAGS_GLOBAL_LOCK (0x2)

#define CHECK_FLAG(_x, _f)  ((_x) & (_f))

//
// Macros to acquire/release global connect list lock.
//

#define IopAcquireGlobalActiveConnectListLock()                             \
    IopAcquireReleaseConnectLockInternal(NULL,                              \
                                         TRUE,                              \
                                         ACTIVE_CONNECT_FLAGS_GLOBAL_LOCK)

#define IopReleaseGlobalActiveConnectListLock()                             \
    IopAcquireReleaseConnectLockInternal(NULL,                              \
                                         FALSE,                             \
                                         ACTIVE_CONNECT_FLAGS_GLOBAL_LOCK)

//
// Macros to acquire/release local connect lock.
//

#define IopAcquireLocalConnectLock(ConnectBlock)                            \
    IopAcquireReleaseConnectLockInternal((ConnectBlock),                    \
                                         TRUE,                              \
                                         ACTIVE_CONNECT_FLAGS_LOCAL_LOCK)

#define IopReleaseLocalConnectLock(ConnectBlock)                            \
    IopAcquireReleaseConnectLockInternal((ConnectBlock),                    \
                                         FALSE,                             \
                                         ACTIVE_CONNECT_FLAGS_LOCAL_LOCK)

//
// -------------------------------------------------------------------- Globals
//

//
// Define the global connect/disconnect interrupt list. This is a list of
// interrupts that either be actively connected or disconnected.
//

LIST_ENTRY ActiveConnectList;

//
// Define the lock used to protect accesses to the global active
// connect/disconnect list.
//

KEVENT ActiveConnectListLock;

//
// ----------------------------------------------------------------- Prototypes
//

//
// Internal-only routines.
//

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopAcquireReleaseConnectLockInternal (
    __in_opt PACTIVE_CONNECT_BLOCK ConnectBlock,
    __in BOOLEAN AcquireLock,
    __in ULONG Flags
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
PACTIVE_CONNECT_BLOCK
IopFindActiveConnectBlockLocked (
    __in ULONG Gsiv
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopInsertActiveConnectListLocked (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
KINLINE
IopUnlinkActiveConnectBlock (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    );

//
// -------------------------------------------------------------------- Pragmas
//

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, IopAcquireActiveConnectLock)
#pragma alloc_text(PAGE, IopDestroyActiveConnectBlock)
#pragma alloc_text(INIT, IopInitializeActiveConnectList)
#pragma alloc_text(PAGE, IopInitializeActiveConnectBlock)
#pragma alloc_text(PAGE, IopReleaseActiveConnectLock)

//
// Internal-only routines.
//

#pragma alloc_text(PAGE, IopAcquireReleaseConnectLockInternal)
#pragma alloc_text(PAGE, IopFindActiveConnectBlockLocked)
#pragma alloc_text(PAGE, IopInsertActiveConnectListLocked)
#pragma alloc_text(PAGE, IopUnlinkActiveConnectBlock)

#endif

//
// ------------------------------------------------ Exported Functions (Locking)
//

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopAcquireActiveConnectLock (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine acquires the connect lock used for synchronizing active
    connect/disconnect requests.

    Note this routine will enter critical region if lock is acquired. The
    critical region will be exited when the lock is released.

Arguments:

    ConnectBlock - Supplies a pointer to the connect block.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // Acquire the connect lock if it is specified. Note message-signalled
    // interrupts do not need to be serialized and hence the connect lock
    // is not initialized.
    //

    if (ConnectBlock->ActualConnectLock != NULL) {
        IopAcquireReleaseConnectLockInternal(ConnectBlock,
                                             TRUE,
                                             ACTIVE_CONNECT_FLAGS_NONE);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopReleaseActiveConnectLock (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine releases the connect lock used for synchronizing active
    connect/disconnect requests.

    This routine will also exit the critical region if lock is released.

Arguments:

    ConnectBlock - Supplies a pointer to the active connect block.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // Release the connect lock if it is specified. Note message-signalled
    // interrupts do not need to be serialized and hence the connect lock
    // is not initialized.
    //

    if (ConnectBlock->ActualConnectLock != NULL) {
        IopAcquireReleaseConnectLockInternal(ConnectBlock,
                                             FALSE,
                                             ACTIVE_CONNECT_FLAGS_NONE);
    }

    return;
}

//
// -------------------------------------------------- Exported Functions (Init)
//

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopDestroyActiveConnectBlock (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine causes the given connect block to be destroyed. If there are
    active references to the connect block, besides itself, then this routine
    will wait until all reference are released.

Arguments:

    ConnectBlock - Supplies a pointer to the active connect block.

Return Value:

    None.

--*/

{

    volatile LONG *ActualReferenceCount;
    ULONG Count;
    KEVENT LocalDeleteLock;
    volatile LONG *LocalReferenceCount;
    PACTIVE_CONNECT_BLOCK RemoteConnectBlock;
    PKEVENT RemoteDeleteLock;
    NTSTATUS Status;
    BOOLEAN WaitForDelete;

    PAGED_CODE();

    //
    // Connect/disconnect requests for non-interrupt controller inputs are
    // never serialized or put on the gloabl list. Bail out for such interrupts.
    //

    if (ConnectBlock->ActualConnectLock == NULL) {
        goto DestroyActiveConnectBlockEnd;
    }

    ActualReferenceCount = ConnectBlock->ActualReferenceCount;
    LocalReferenceCount = &ConnectBlock->LocalReferenceCount;
    KeInitializeEvent(&LocalDeleteLock, SynchronizationEvent, FALSE);
    RemoteDeleteLock = NULL;
    WaitForDelete = FALSE;

    //
    // Synchronize access to the active connect list. This will prevent
    // any new callers from taking a reference on the connect block.
    //

    IopAcquireGlobalActiveConnectListLock();

    NT_ASSERT((*ActualReferenceCount > 0) && (*LocalReferenceCount > 0));

    //
    // Drop a reference to the entry.
    //

    Count = InterlockedDecrement(ActualReferenceCount);

    //
    // Check if the synchronization lock was local or a remote one.
    //

    if (ActualReferenceCount != LocalReferenceCount) {

        //
        // If a remote lock was used, then check whether the reference count on 

        // the remote connect block has dropped to zero. If so, then it implies
        // the owner of the remote connect block is waiting on the block to
        // become free. Signal the remote block's delete event to allow it to
        // be freed.
        //

        if (Count == 0) {

            //
            // N.B. The actual reference count field points to the local
            //      reference count field of the remote connect block.
            //

            RemoteConnectBlock = CONTAINING_RECORD(ActualReferenceCount,
                                                   ACTIVE_CONNECT_BLOCK,
                                                   LocalReferenceCount);

            NT_ASSERT(RemoteConnectBlock->DeleteLock != NULL);

            //
            // Capture the lock address so that it can be signalled (outside of
            // the list lock).
            //

            RemoteDeleteLock = RemoteConnectBlock->DeleteLock;
        }

        //
        // Drop the reference on the local one as well. A reference on the
        // local entry was taken when the connect block was initialized.
        //

        Count = InterlockedDecrement(LocalReferenceCount);

    //
    // If the synchronization lock is local, check if this was the last
    // reference. Otherwise, there is an active reference still pending on
    // the connect block and cannot be freed immediately. Set a delete lock
    // that will be signalled when the last reference is dropped.
    //

    } else if (Count > 0) {
        ConnectBlock->DeleteLock = &LocalDeleteLock;
        WaitForDelete = TRUE;
    }

    //
    // Remove the entry from the list if it was inserted.
    //
    // N.B. Even though this entry is removed from the list, it is possible
    //      for new connect/disconnect requests to arrive and take additional
    //      references on this entry. This is because there may be other
    //      entries on the global list that reference this entry. So newer
    //      requests for the same GSIV would find that entry and in turn
    //      reference this entry.
    //

    IopUnlinkActiveConnectBlock(ConnectBlock);

    //
    // Drop the global connect list lock.
    //

    IopReleaseGlobalActiveConnectListLock();

    NT_ASSERT((WaitForDelete == FALSE) || (RemoteDeleteLock == NULL));

    //
    // Protect this thread from being suspended while delete lock is held.
    //

    KeEnterCriticalRegion();

    //
    // Wait while the connect block is being actively used by some other
    // connect/disconnect request.
    //

    if (WaitForDelete != FALSE) {
        Status = KeWaitForSingleObject(&LocalDeleteLock,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL);

        NT_ASSERT(Status == STATUS_SUCCESS);

    //
    // Signal the delete event as the reference count has now dropped to zero.
    // So the remote connect block can be freed.
    //

    } else if (RemoteDeleteLock != NULL) {
        KeSetEvent(RemoteDeleteLock, IO_NO_INCREMENT, FALSE);
    }

    //
    // Exit the critical region that was entered when the lock was acquired.
    //

    KeLeaveCriticalRegion();

DestroyActiveConnectBlockEnd:
    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopInitializeActiveConnectBlock (
    __in PINTERRUPT_CONNECTION_DATA ConnectionData,
    __inout PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine will initialize the supplied connect block with the lock to
    be used for synchronization. It will also insert the supplied connect block
    into the global active connect list if the GSIV maps to an interrupt
    controller input. For message-signalled interrupts, they are not put
    on the global list since they do not need to be serialized.

Arguments:

    ConnectionData - Supplies a pointer to interrupt information (viz. GSIV).

     ConnectBlock - Supplies a pointer to a connect lock to be initialized.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    ULONG Gsiv;
    PACTIVE_CONNECT_BLOCK RemoteConnectBlock;

    PAGED_CODE();

    //
    // Initialize all fields to NULL.
    //

    RtlZeroMemory(ConnectBlock, sizeof(ACTIVE_CONNECT_BLOCK));

    //
    // Currently connect/disconnect operations only need to be serialized for
    // interrupt controller inputs.
    //

    if (ConnectionData->Vectors[0].Type != InterruptTypeControllerInput) {
        goto InitializeActiveConnectBlockEnd;
    }

    NT_ASSERT(ConnectionData->Count == 1);

    //
    // Initialize the connect block fields to default values.
    //

    InitializeListHead(&ConnectBlock->ListEntry);
    KeInitializeEvent(&ConnectBlock->LocalConnectLock,
                      SynchronizationEvent,
                      TRUE);

    ConnectBlock->LocalReferenceCount = 1;
    Gsiv = ConnectionData->Vectors[0].ControllerInput.Gsiv;
    ConnectBlock->Gsiv = Gsiv;

    //
    // Acquire the global active list lock prior to manipulating the list.
    //

    IopAcquireGlobalActiveConnectListLock();

    //
    // Check if a active connect block describing the vector already exists. If
    // it exists, then set the connect block to reference the existing event.
    //

    RemoteConnectBlock = IopFindActiveConnectBlockLocked(Gsiv);

    //
    // If an entry is found, then use the connect lock as specified within
    // that (remote) connect block. Otherwise, use the local connect lock.
    //
    // N.B. In the case an existing entry is found, the actual connect lock
    //      and reference count should point to the whatever the other entry is
    //      pointing to (and not to its local lock and count). This is because
    //      that remote entry may go away but the entry it is pointing to
    //      will persist until all the references are dropped.
    //

    if (RemoteConnectBlock != NULL) {
        ConnectBlock->ActualConnectLock = RemoteConnectBlock->ActualConnectLock;
        ConnectBlock->ActualReferenceCount =
            RemoteConnectBlock->ActualReferenceCount;

        InterlockedIncrement(RemoteConnectBlock->ActualReferenceCount);

    } else {
        ConnectBlock->ActualConnectLock = &ConnectBlock->LocalConnectLock;
        ConnectBlock->ActualReferenceCount = &ConnectBlock->LocalReferenceCount;
    }

    //
    // Insert the new entry onto the list even if it references a remote entry.
    // This is required because the remote entry would be taken off the list
    // when it is being destroyed. New requests that arrive after that point
    // would be able to reference the remote object via this (the new) object.
    // Otherwise, there could be situation where connect/disconnect requests
    // on the same GSIV are synchronized using different locks.
    //

    IopInsertActiveConnectListLocked(ConnectBlock);

    IopReleaseGlobalActiveConnectListLock();

InitializeActiveConnectBlockEnd:
    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopInitializeActiveConnectList (
    VOID
    )

/*++

Routine Description:

    This routine performs initialization of the active connect/disconnect list
    and the lock used to protect it.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    InitializeListHead(&ActiveConnectList);
    KeInitializeEvent(&ActiveConnectListLock, SynchronizationEvent, TRUE);
    return;
}

//
// ---------------------------------------------------------- Internal routines
//

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
PACTIVE_CONNECT_BLOCK
IopFindActiveConnectBlockLocked (
    __in ULONG Gsiv
    )

/*++

Routine Description:

    This routine walks the global active connect/disconnect list to check if
    there is an active connect block matching the supplied GSIV.

    N.B. This routine assumes the caller holds the global active connect list
         lock.

Arguments:

    Gsiv - Supplies the GSIV to be searched for.

Return Value:

    Pointer to the active connect block if found. NULL otherwise.

--*/

{

    PACTIVE_CONNECT_BLOCK MatchingEntry;
    PLIST_ENTRY NextEntry;
    PACTIVE_CONNECT_BLOCK ConnectBlock;

    PAGED_CODE();

    //
    // Walk the active connect list and check if there is an entry for the
    // supplied GSIV.
    //

    MatchingEntry = NULL;
    NextEntry = ActiveConnectList.Flink;
    while (NextEntry != &ActiveConnectList) {
        ConnectBlock = CONTAINING_RECORD(NextEntry,
                                         ACTIVE_CONNECT_BLOCK,
                                         ListEntry);

        if (ConnectBlock->Gsiv == Gsiv) {
            MatchingEntry = ConnectBlock;
            break;

        } else if (ConnectBlock->Gsiv > Gsiv) {
            break;
        }

        NextEntry = NextEntry->Flink;
    }

    return MatchingEntry;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopInsertActiveConnectListLocked (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine walks the global active connect/disconnect list to check if
    there is an active connect block matching the supplied GSIV.

    N.B. This routine assumes the caller holds the global active connect list
         lock.

Arguments:

    Gsiv - Supplies the GSIV to be searched for.

Return Value:

    Pointer to the active connect block if found. NULL otherwise.

--*/

{

    PLIST_ENTRY NextEntry;
    PACTIVE_CONNECT_BLOCK LocalConnectBlock;

    PAGED_CODE();

    //
    // Walk the active connect list and insert the entry at correct location
    // (ordered on GSIV value).
    //

    NextEntry = ActiveConnectList.Flink;
    while (NextEntry != &ActiveConnectList) {
        LocalConnectBlock = CONTAINING_RECORD(NextEntry,
                                              ACTIVE_CONNECT_BLOCK,
                                              ListEntry);

        //
        // Insert the new connect block before the existing one.
        //

        if (LocalConnectBlock->Gsiv > ConnectBlock->Gsiv) {
            InsertTailList(&LocalConnectBlock->ListEntry,
                           &ConnectBlock->ListEntry);

            break;
        }

        NextEntry = NextEntry->Flink;
    }

    //
    // If the entire list was walked without insertion, then the given GSIV
    // is the largest and thus insert it at the end of the list. This also
    // covers an empty list case.
    //

    if (NextEntry == &ActiveConnectList) {
        InsertTailList(&ActiveConnectList, &ConnectBlock->ListEntry);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
IopAcquireReleaseConnectLockInternal (
    __in_opt PACTIVE_CONNECT_BLOCK ConnectBlock,
    __in BOOLEAN AcquireLock,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine acquires/relases the lock used for syncrhonization. The lock
    can either be synchronization connect lock, local connect lock or global
    list lock.

    N.B. This routine will enter critical region on acquire and exit critical
         region on release.

Arguments:

    ConnectBlock - Supplies a pointer to the connect block. This is optional
        if the

    Flags - Supplies flag that control the operation. Currently defined flags:
        ACTIVE_CONNECT_FLAGS_NONE - if this flag is set then the connect
            synchronization lock will be acquired/released.

        ACTIVE_CONNECT_FLAGS_LOCAL_LOCK - if this flag is set then the local
            lock will be acquired/released.

        ACTIVE_CONNECT_FLAGS_GLOBAL_LOCK - if this flag is set then the local
            lock will be acquired/released.

Return Value:

    None.

--*/

{

    PKEVENT Lock;
    NTSTATUS Status;

    PAGED_CODE();

    if (CHECK_FLAG(Flags, ACTIVE_CONNECT_FLAGS_LOCAL_LOCK) != FALSE) {
        Lock = &ConnectBlock->LocalConnectLock;

    } else if (CHECK_FLAG(Flags, ACTIVE_CONNECT_FLAGS_GLOBAL_LOCK) != FALSE) {
        Lock = &ActiveConnectListLock;

    } else {

        NT_ASSERT((ConnectBlock->ActualConnectLock != NULL) &&
                  (*ConnectBlock->ActualReferenceCount >= 1));

        Lock = ConnectBlock->ActualConnectLock;
    }

    if (AcquireLock != FALSE) {

        //
        // Protect this thread from being suspended while connect interrupt
        // lock is held. The critical region will be exited when the lock is
        // released.
        //

        KeEnterCriticalRegion();

        Status = KeWaitForSingleObject(Lock,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL);

        NT_ASSERT(Status == STATUS_SUCCESS);

#if DBG

    //
    // Set the current thread as the owner thread of the connect lock.
    //

    if ((ConnectBlock != NULL) && (Lock == ConnectBlock->ActualConnectLock)) {

        NT_ASSERT(ConnectBlock->OwnerThread == NULL);

        ConnectBlock->OwnerThread = KeGetCurrentThread();
    }

#endif

    } else {

#if DBG

    //
    // The critical region must have been entered when the lock was acquired.
    //

    NT_ASSERT(KeAreApcsDisabled() != FALSE);

    //
    // Clear the owner field prior to releasing the connect lock.
    //

    if ((ConnectBlock != NULL) && (Lock == ConnectBlock->ActualConnectLock)) {
        ConnectBlock->OwnerThread = NULL;
    }

#endif

        KeSetEvent(Lock, IO_NO_INCREMENT, FALSE);

        //
        // Exit the critical region that was entered when the lock was acquired.
        //

        KeLeaveCriticalRegion();
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
KINLINE
IopUnlinkActiveConnectBlock (
    __in PACTIVE_CONNECT_BLOCK ConnectBlock
    )

/*++

Routine Description:

    This routine removes the supplied active connect block from the global list.

Arguments:

    ConnectBlock - Supplies a pointer to the connect block.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // Remove the entry from the list if it was put on the global connect list.
    //
    // N.B. Connect/disconnect requests for non-interrupt controller inputs are
    //      never put on the list.
    //

    NT_ASSERT(IsListEmpty(&ConnectBlock->ListEntry) == FALSE);

    RemoveEntryList(&ConnectBlock->ListEntry);

    return;
}

