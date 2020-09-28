/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    util.h

Abstract:

    Contains inline utility functions.

Author:

    Erik Schmidt (eriksch) - May 2013

Environment:



Revision History:

--*/

#pragma once

#define UFS_POOL_TAG                 ' sfU'

//
// Inline Functions
//

__inline
VOID
UfsDelay(
    _In_ ULONG Time
    )
/*++

Routine Description:

    Delay for the specified number of milliseconds

Arguments:

    Time - the number of milliseconds to delay for

Return Value:

    N/A

--*/
{
    ULONG i;

    for (i = 0; i < Time; i++) {
        StorPortStallExecution(1000);
    }
}

__inline
BOOLEAN
UfsQueryDumpMode(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Returns whether the driver is in crashdump or hibernate mode

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    True if the driver is in dump mode, false if not

--*/
{
    return (AdapterExtension->DumpMode > 0);
}

__inline
ULONG
UfsFindListIndex (
    _In_ ULONG List,
    _In_ ULONG MaxSize
    )
/*++

Routine Description:

    Find the first available index in the Task Management Request List or the Transfer
    Request List.

Arguments:

    List - bit vector containing the available (0) indices of the list

    MaxSize - the maximum number of elements in the list

Return Value:

    The index of the first available slot in the list.

--*/
{
    ULONG i;

    for (i = 0; i < MaxSize; i++){
        if ((List & (1 << i)) == 0) {
            return i;
        }
    }

    return INVALID_INDEX;
}

#pragma warning(push)
#pragma warning(suppress: 28167 26166 26135)
__inline
_Post_same_lock_(*SpinLock, *LockHandle)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_saves_global_(UfsSpinlock, LockHandle)
KIRQL
UfsAcquireSpinlock(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _Inout_ PKSPIN_LOCK SpinLock,
    _Inout_ PKLOCK_QUEUE_HANDLE LockHandle
    )
/*++

Routine Description:

    Acquire the passed in spinlock. If in Dump Mode, do nothing, and aquire
    the lock at DPC level if requested.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    SpinLock - Pointer to the spinlock to acquire

    LockHandle - The lock handle used to release the lock

    Dispatch - Specifies whether to acquire the lock at DPC level

Return Value:

    N/A

--*/
{
    KIRQL irql = KeGetCurrentIrql();

    if (UfsQueryDumpMode(AdapterExtension) == FALSE) {
        if (irql == DISPATCH_LEVEL) {
            NT_ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

            KeAcquireInStackQueuedSpinLockAtDpcLevel(SpinLock,
                                                     LockHandle);
        } else if (irql < DISPATCH_LEVEL) {
            KeAcquireInStackQueuedSpinLock(SpinLock,
                                           LockHandle);
        } else {
            //
            // We shouldn't be calling this function from higher than Dispatch
            //
            NT_ASSERT(FALSE);
        }
    }

    return irql;
}
#pragma warning(pop)

#pragma warning(suppress: 26165)
__inline
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_restores_global_(UfsSpinlock, LockHandle)
VOID
UfsReleaseSpinlock(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PKLOCK_QUEUE_HANDLE LockHandle,
    _In_ KIRQL Irql
    )
/*++

Routine Description:

    Acquire the passed in spinlock. If in Dump Mode, do nothing, and aquire
    the lock at DPC level if requested.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    LockHandle - The lock handle used to release the lock

    Dispatch - Specifies whether to acquire the lock at DPC level

Return Value:

    N/A

--*/
{
    if (UfsQueryDumpMode(AdapterExtension) == FALSE) {
        if (Irql == DISPATCH_LEVEL) {
#pragma warning(suppress: 26110 26135)
            KeReleaseInStackQueuedSpinLockFromDpcLevel(LockHandle);
        } else if (Irql < DISPATCH_LEVEL) {
#pragma warning(suppress: 26110 26135)
            KeReleaseInStackQueuedSpinLock(LockHandle);
        } else {
            //
            // We shouldn't be calling this function from higher than Dispatch
            //
            NT_ASSERT(FALSE);
        }
    }
}

//
// Function Prototypes
//

NTSTATUS
UfsAllocateContiguousMemory(
    _In_ PVOID   AdapterExtension,
    _In_ ULONG   BufferLength,
    _Post_writable_byte_size_(BufferLength) PVOID* Buffer
    );

NTSTATUS
UfsFreeContiguousMemory (
    _In_  PVOID      AdapterExtension,
    _In_  ULONG_PTR  BufferLength,
    _In_reads_bytes_(BufferLength) _Post_invalid_ PVOID Buffer
    );

VOID
UfsGetRegisterAddress(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PPORT_CONFIGURATION_INFORMATION ConfigInfo
    );

ULONG
UfsAcquireTaskManBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsReleaseTaskManBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index
    );

ULONG
UfsAcquireTransferBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsReleaseTransferBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index,
    _In_ BOOLEAN Sync
    );

ULONG
UfsAcquireTaskManCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsReleaseTaskManCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TmrlResources
    );

ULONG
UfsAcquireTransferCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ BOOLEAN Sync
    );

VOID
UfsReleaseTransferCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TrlResources
    );