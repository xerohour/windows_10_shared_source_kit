/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    util.c

Abstract:

    This file contains helper functions to perform common tasks.

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#include "storufs.h"

/* SSS_WARNINGS_OFF */
#pragma warning(disable:6011)   // possible null pointer dereference

NTSTATUS
UfsAllocateContiguousMemory(
    _In_ PVOID   AdapterExtension,
    _In_ ULONG   BufferLength,
    _Post_writable_byte_size_(BufferLength) PVOID* Buffer
    )
/*++

Routine Description:

    Wrapper function for allocating a continuous block of memory

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    BufferLength - the number of bytes to allocate

    Buffer - the address of the allocated memory

Return Value:

    NT_STATUS value of Storport function call

--*/
{
    ULONG status;
    PHYSICAL_ADDRESS minPhysicalAddress;
    PHYSICAL_ADDRESS maxPhysicalAddress;
    PHYSICAL_ADDRESS boundaryPhysicalAddress;

    minPhysicalAddress.QuadPart = 0;
    maxPhysicalAddress.QuadPart = 0x7FFFFFFF;
    boundaryPhysicalAddress.QuadPart = 0;

    status = StorPortAllocateContiguousMemorySpecifyCacheNode(AdapterExtension,
                                                              BufferLength,
                                                              minPhysicalAddress,
                                                              maxPhysicalAddress,
                                                              boundaryPhysicalAddress,
                                                              MmCached,
                                                              MM_ANY_NODE_OK,
                                                              Buffer);

    if (Buffer == NULL) {
        NT_ASSERT(Buffer != NULL);
        // LogError: "Error Allocating Continuous Memory of Size:0x%08x\n"
        return status;
    }

    RtlZeroMemory(*Buffer, BufferLength);

    return status;
}

NTSTATUS
UfsFreeContiguousMemory (
    _In_  PVOID      AdapterExtension,
    _In_  ULONG_PTR  BufferLength,
    _In_reads_bytes_(BufferLength) _Post_invalid_ PVOID Buffer
    )
/*++

Routine Description:

    Wrapper function for freeing a continuous pool of memory

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    BufferLength - length of the buffer to free

    Buffer - the address of the allocated memory

Return Value:

    Error code returned by StorPortFreePool

--*/
{
    if (Buffer == NULL) {
        return STOR_STATUS_INVALID_PARAMETER;
    }

    return StorPortFreeContiguousMemorySpecifyCache(AdapterExtension,
                                                    Buffer,
                                                    BufferLength,
                                                    MmCached);
}

VOID
UfsGetRegisterAddress(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PPORT_CONFIGURATION_INFORMATION ConfigInfo
    )
/*++

Routine Description:

    Goes through the access ranges and locates the base address of the memory
    mapped registers.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    ConfigInfo - Bus configuration information

Return Value:

    Pointer to the memory mapped registers, NULL if failure

--*/
{
    ULONG i;
    ULONGLONG bar = AdapterExtension->UfsBar;
    PUFS_HCI_REG *reg = &AdapterExtension->UfsRegisters;
    INTERFACE_TYPE busType = ConfigInfo->AdapterInterfaceType;
    STOR_PHYSICAL_ADDRESS addrMphyRegisters;











    for (i = 0; i < ConfigInfo->NumberOfAccessRanges; i++) {

        //
        // PCI.sys makes sure that the first memory type of resource
        // is from BAR0
        //

        if ((ULONGLONG)(*(ConfigInfo->AccessRanges))[i].RangeStart.QuadPart == bar) {
            *reg = StorPortGetDeviceBase(AdapterExtension,
                                         busType,
                                         ConfigInfo->SystemIoBusNumber,
                                         (*(ConfigInfo->AccessRanges))[i].RangeStart,
                                         (*(ConfigInfo->AccessRanges))[i].RangeLength,
                                         (BOOLEAN)!(*(ConfigInfo->AccessRanges))[i].RangeInMemory);
            break;
        }
    }
















}

ULONG
UfsAcquireTaskManBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Find the first available index in the Task Man Request List, and update
    the value of "BuildingRequests" to indicate the newly acquired resource.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    The index of the first available slot in the list, or -1 on failure.

--*/
{
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG index = 0, tmrlResources;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension,
                              &tmrl->RequestsLock,
                              &lock);

    tmrlResources = tmrl->BuildingRequests |
                    tmrl->ExecutingRequests |
                    tmrl->ProcessingRequests;

    tmrlResources |= StorPortReadRegisterUlong(AdapterExtension,
                                               &reg->TaskMgmt.Doorbell.AsUlong);

    index = UfsFindListIndex(tmrlResources, tmrl->ListSize);

    if (index != INVALID_INDEX) {
        NT_ASSERT(index < AdapterExtension->Tmrl.ListSize);

        tmrl->BuildingRequests |= (0x1 << index);

        NT_ASSERT((tmrl->BuildingRequests &
                   tmrl->ExecutingRequests &
                   tmrl->ProcessingRequests) == 0);
    }

    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);
    
    return index;
}

VOID
UfsReleaseTaskManBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index
    )
/*++

Routine Description:

    Update the index from being built to being executed.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Index - The index of the resource in the TMRL to be updated

Return Value:

    N/A

--*/
{
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UTMRLDBR doorbell;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension,
                              &tmrl->RequestsLock,
                              &lock);

    tmrl->BuildingRequests &= ~(1 << Index);
    
    NT_ASSERT((AdapterExtension->Power.Active != 0) ||
              (AdapterExtension->Power.DStateTransition != 0));

    tmrl->ExecutingRequests |= (1 << Index);

    doorbell.AsUlong = 1 << Index;

    StorPortWriteRegisterUlong(AdapterExtension,
                                &reg->TaskMgmt.Doorbell.AsUlong,
                                doorbell.AsUlong);

    NT_ASSERT((tmrl->BuildingRequests &
               tmrl->ExecutingRequests &
               tmrl->ProcessingRequests) == 0);

    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);
}

ULONG
UfsAcquireTransferBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Find the first available index in the Transfer Request List, and update the
    value of "BuildingRequests" to indicate the newly acquired resource.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    The index of the first available slot in the list, or -1 on failure.

--*/
{
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG index = 0, trlResources;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension,
                              &trl->RequestsLock,
                              &lock);

    //
    // Find an empty Transfer Request slot by reading the Doorbell Register
    // and the adapter extension TRL resource variable
    //

    trlResources = trl->BuildingRequests |
                   trl->ExecutingRequests |
                   trl->ExecutingSyncRequests |
                   trl->ProcessingRequests;

    trlResources |= StorPortReadRegisterUlong(AdapterExtension,
                                              &reg->TransReq.Doorbell.AsUlong);

    index = UfsFindListIndex(trlResources, trl->ListSize);

    if (index != INVALID_INDEX) {
        NT_ASSERT(index < AdapterExtension->Trl.ListSize);

        trl->BuildingRequests |= (0x1 << index);

        NT_ASSERT((trl->BuildingRequests &
                   trl->ExecutingRequests &
                   trl->ExecutingSyncRequests &
                   trl->ProcessingRequests) == 0);
    }
    
    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);

    return index;
}

VOID
UfsReleaseTransferBuildIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index,
    _In_ BOOLEAN Sync
    )
/*++

Routine Description:

    Update the index from being built to being executed.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Index - The index of the resource in the TRL to be updated

    Sync - Whether to update the *SyncRequests or *Requests bit vector

Return Value:

    N/A

--*/
{
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UTRLDBR doorbell;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension, 
                              &trl->RequestsLock,
                              &lock);

    trl->BuildingRequests &= ~(1 << Index);

    NT_ASSERT((AdapterExtension->Power.Active != 0) ||
              (AdapterExtension->Power.DStateTransition != 0));
    
    if (Sync == FALSE) {
        trl->ExecutingRequests |= (1 << Index);
    } else {
        trl->ExecutingSyncRequests |= (1 << Index);
    }

    doorbell.AsUlong = 1 << Index;

    StorPortWriteRegisterUlong(AdapterExtension,
                                &reg->TransReq.Doorbell.AsUlong,
                                doorbell.AsUlong);

    NT_ASSERT((trl->BuildingRequests &
               trl->ExecutingRequests &
               trl->ExecutingSyncRequests &
               trl->ProcessingRequests) == 0);
    
    UfsReleaseSpinlock(AdapterExtension,
                       &lock, 
                       irql);
}

ULONG
UfsAcquireTaskManCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Compare the Task Management Doorbell Register with the executing resources
    variable, and move all the completed resources from the executing variable
    to the processing variable to indicate that the resources aren't free yet,
    but are no longer executing.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    Bit array of the completed resources.

--*/
{
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG tmrlResources;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension,
                              &tmrl->RequestsLock,
                              &lock);

    //
    // Find out which offsets in the TMRL completed by comparing the Doorbell
    // Register with the value of occupied resources in the Adapter Extension
    //

    tmrlResources = tmrl->ExecutingRequests;
    tmrlResources &= ~StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->TaskMgmt.Doorbell.AsUlong);

    //
    // Mark which interrupts in the TMRL you are in the process of handling so
    // we can release the lock and let other processes access it.
    //

    tmrl->ProcessingRequests |= tmrlResources;
    tmrl->ExecutingRequests &= ~tmrlResources;

    NT_ASSERT((tmrl->BuildingRequests &
               tmrl->ExecutingRequests &
               tmrl->ProcessingRequests) == 0);

    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);

    return tmrlResources;
}

VOID
UfsReleaseTaskManCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TmrlResources
    )
/*++

Routine Description:

    Release the given resources from the processing resources variable,
    indicating that they are no longer being processed and are free for a
    new I/O.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    TmrlResources - Bit array corresponding to the indices to be released.

Return Value:

    N/A

--*/
{
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;
    
    irql = UfsAcquireSpinlock(AdapterExtension,
                              &tmrl->RequestsLock,
                              &lock);

    tmrl->ProcessingRequests &= ~TmrlResources;

    NT_ASSERT((tmrl->BuildingRequests &
               tmrl->ExecutingRequests &
               tmrl->ProcessingRequests) == 0);

    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);
}

ULONG
UfsAcquireTransferCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ BOOLEAN Sync
    )
/*++

Routine Description:

    Compare the Transfer Request Doorbell Register with the executing
    resources variable, and move all the completed resources from the
    executing variable to the processing variable to indicate that the
    resources aren't free yet, but are no longer executing.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Sync - Specifies whether to complete the *ExecutingSyncRequests or 
           *ExecutingRequests indices

Return Value:

    Bit array of the completed resources.

--*/
{
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG trlResources;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;
    
    irql = UfsAcquireSpinlock(AdapterExtension,
                              &trl->RequestsLock,
                              &lock);

    //
    // Find out which offsets in the TRL completed by comparing the Doorbell
    // Register with the value of occupied resources in the Adapter Extension
    //

    if (Sync == FALSE) {
        trlResources = trl->ExecutingRequests;
    } else {
        trlResources = trl->ExecutingSyncRequests;
    }

    trlResources &= ~StorPortReadRegisterUlong(AdapterExtension,
                                               &reg->TransReq.Doorbell.AsUlong);

    //
    // Mark which interrupts in the TRL you are in the process of handling so
    // we can release the lock and let other processes access it.
    //

    trl->ProcessingRequests |= trlResources;

    if (Sync == FALSE) {
        trl->ExecutingRequests &= ~trlResources;
    } else {
        trl->ExecutingSyncRequests &= ~trlResources;
    }

    NT_ASSERT((trl->BuildingRequests &
               trl->ExecutingRequests &
               trl->ExecutingSyncRequests &
               trl->ProcessingRequests) == 0);

    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);

    return trlResources;

}

VOID
UfsReleaseTransferCompletionIndex (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TrlResources
    )
/*++

Routine Description:

    Release the given resources from the processing resources variable,
    indicating that they are no longer being processed and are free for a
    new I/O.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    TrlResources - Bit array corresponding to the indices to be released.

Return Value:

    N/A

--*/
{
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    KLOCK_QUEUE_HANDLE lock = {0};
    KIRQL irql;

    irql = UfsAcquireSpinlock(AdapterExtension, 
                              &trl->RequestsLock,
                              &lock);

    trl->ProcessingRequests &= ~TrlResources;

    NT_ASSERT((trl->BuildingRequests &
               trl->ExecutingRequests &
               trl->ExecutingSyncRequests &
               trl->ProcessingRequests) == 0);
    
    UfsReleaseSpinlock(AdapterExtension,
                       &lock,
                       irql);
}
