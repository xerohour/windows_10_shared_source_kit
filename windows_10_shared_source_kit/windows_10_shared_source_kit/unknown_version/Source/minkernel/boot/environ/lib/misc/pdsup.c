/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pdsup.c

Abstract:

    Implementation of persistent data support routines for sharing
    information between boot applications and OS init.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>
#include <bapdsup.h>
#include <systemevents.h>

//
// --------------------------------------------------------------------- Locals
//

static LIST_ENTRY BlpPdListHead;

BOOLEAN PdPersistAllocations;

//
// ---------------------------------------------------------------- Definitions
//

#define BAPD_ITEM_V1_SIZE_ALIGNED \
    ALIGN_RANGE_UP(sizeof(BAPD_ITEM_V1), sizeof(UINT_PTR))

//
// PD_ALLOCATION_LIST_GUID: b89cece7-d091-4002-b0f2-d72a7ea029ce
//

#define PD_ALLOCATION_LIST_GUID { 0xb89cece7, 0xd091, 0x4002, \
    { 0xb0, 0xf2, 0xd7, 0x2a, 0x7e, 0xa0, 0x29, 0xce} }

typedef struct _PD_MEMORY_DESCRIPTOR {
    ULONGLONG BasePage;
    ULONGLONG PageCount;
    ULONG Attributes;
    MEMORY_TYPE MemoryType;
} PD_MEMORY_DESCRIPTOR, *PPD_MEMORY_DESCRIPTOR;

typedef struct _PD_ALLOCATION_LIST {
    ULONGLONG AllocationCount;
    PD_MEMORY_DESCRIPTOR AllocationList[ANYSIZE_ARRAY];
} PD_ALLOCATION_LIST, *PPD_ALLOCATION_LIST;

//
// ----------------------------------------------------------------- Prototypes
//

VOID
PdInsertListItem (
    __inout PLIST_ENTRY ListHead,
    __in PBAPD_LIST_ITEM InsertItem
    );

NTSTATUS
PdDestroyAllocations (
    VOID
    );

NTSTATUS
PdPersistAllocationList (
    __in PBAPD_ITEM_V1 PersistentData
    );

NTSTATUS
PdPopulateArrayFromList (
    __in PLIST_ENTRY ListHead,
    __out PULONGLONG PhysicalBasePage,
    __out PULONGLONG PageCount
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BlPdAllocateData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in ULONG Attributes,
    __inout PGENERIC_BUFFER PersistentDataBuffer
    )

/*++

Routine Description:

    This routine allocates persistent memory and returns its address such
    that data can be shared between the calling application and during OS
    initialization.

Arguments:

    BootApplication - Supplies an optional pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    Attributes - Supplies additional attributes for the persistent memory
        allocation.

    PersistentDataBuffer - Supplies a buffer containing the size of the data
        buffer to allocate as persistent and returns its address.

Return Value:

    STATUS_SUCCESS if the persistent data buffer was successfully allocated,
        otherwise an error status describing the reason for the failure is
        returned.

--*/

{

    ULONG AllocationFlags;
    ULONG Length;
    ULONG PageCount;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    //
    // Sanitize parameters.
    //

    if ((PersistentDataBuffer == NULL) ||
        (PersistentDataBuffer->DataSize == 0) ||
        (DataType == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    Status = RtlULongAdd(PersistentDataBuffer->DataSize, BAPD_ITEM_V1_SIZE_ALIGNED, &Length);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }


    //
    // Allocate internal pages for the persistent buffer.  All applications do
    // not execute with virtual address translation requiring persisted
    // allocations to be physical.  Set the 'library only free' free flag on
    // the allocation request to persist the allocation across library
    // destruction.
    //
    // N.B. This is a persistent data item allocation that includes space for
    //      internal metadata. This allocation should NOT use
    //      BlMmAllocatePersistentPhysicalPages. That API should only be used
    //      for persistent allocations with external metadata - typically to be
    //      used by an application and not the boot library.
    //

    if (ALIGN_RANGE_UP(Length, PAGE_SIZE) < Length) {
        return STATUS_INTEGER_OVERFLOW;
    }

    PageCount = ALIGN_RANGE_UP(Length, PAGE_SIZE) >> PAGE_SHIFT;
    AllocationFlags = MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       PageCount,
                                       MEMORY_TYPE_PERSISTENT_DATA,
                                       AllocationFlags,
                                       1);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    PdItem = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
    Status = BlMmMapPhysicalAddress(&PdItem,
                                    PhysicalAddress,
                                    Length,
                                    Attributes);

    //
    // If the physical pages cannot be virtually mapped, free the physical
    // pages.
    //

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        return Status;
    }

    //
    // Initialize the internal header for the allocation.  Use the version 1
    // definition to maximize compatibility.
    //

    RtlZeroMemory(PdItem, Length);
    PdItem->DataSize = PersistentDataBuffer->DataSize;
    PdItem->Revision = 1;
    PdItem->DataOffset = BAPD_ITEM_V1_SIZE_ALIGNED;
    PdItem->DataType = *DataType;
    if (BootApplication != NULL) {
        PdItem->Application = *BootApplication;
    }

    //
    // Return the offset to the persistent data buffer.
    //

    PersistentDataBuffer->Buffer = (PUCHAR)PdItem + PdItem->DataOffset;

    //
    // Add this persistent data buffer to our list.
    //

    PdListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
    if (PdListItem == NULL) {
        BlMmUnmapVirtualAddress(PdItem, Length);
        BlMmFreePhysicalPages(PhysicalAddress);
        return STATUS_NO_MEMORY;
    }

    PdListItem->ItemDesc.u.PhysAddr = PhysicalAddress;
    PdListItem->ItemDesc.Length = Length;
    InsertTailList(&BlpPdListHead, &PdListItem->Entry);
    return STATUS_SUCCESS;
}

NTSTATUS
BlPdDestroyData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in ULONG Index
    )

/*++

Routine Description:

    This routine destroys a persistent memory item created by BlPdSaveData.

Arguments:

    BootApplication - Supplies an optional pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    Index - Supplies a count for the instance of data, in case more than one
        instance of a particular BootApplication/DataType exists.

Return Value:

    NT status code.

--*/

{

    ULONG Instance;
    GUID InvalidDataGuid = BAPD_INVALID_DATA_GUID;
    PLIST_ENTRY NextEntry;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    NTSTATUS Status;

    //
    // Sanitize parameters.
    //

    if (DataType == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Iterate through the persistent data items looking for a match.
    //

    Instance = 0;
    PdItem = NULL;
    NextEntry = BlpPdListHead.Flink;
    while (NextEntry != &BlpPdListHead) {
        PdListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        NextEntry = NextEntry->Flink;
        Status = BlMmMapPhysicalAddress(&PdItem,
                                        PdListItem->ItemDesc.u.PhysAddr,
                                        PdListItem->ItemDesc.Length,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Make sure this is the correct version.
        //

        if ((PdItem->Revision == 0) || (PdItem->Revision > BAPD_ITEM_VERSION)) {
            BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
            continue;
        }

        //
        // If the data type matches, optionally check if it's the correct
        // application.
        //

        if (RtlEqualMemory(DataType, &PdItem->DataType, sizeof(GUID))) {
            if ((BootApplication != NULL) &&
                (RtlEqualMemory(BootApplication,
                                &PdItem->Application,
                                sizeof(GUID)) == FALSE)) {

                BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
                continue;
            }

            //
            // Check if this is the instance the caller is looking for.
            //

            if (Index == Instance) {

                //
                // Invalidate the data.
                //

                PdItem->DataType = InvalidDataGuid;
                PdItem->Application = InvalidDataGuid;
                RtlZeroMemory(Add2Ptr(PdItem, PdItem->DataOffset),
                              PdItem->DataSize);

                PdItem->DataOffset = 0;
                PdItem->DataSize = 0;

                //
                // Delete the persistent data item, freeing all associated
                // allocations, and removing the list item from the global list
                // of persistent data.
                //

                BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);

                //
                // N.B. Calling BlMmFreePhysicalPages will fail, because when
                //      the data gets created, a Virtual Address is returned to
                //      the caller, since that VA is never unmapped, the memory
                //      remains mapped and MM can't free it. Instead, the data
                //      is invalidated above.
                //
                // BlMmFreePhysicalPages(PdListItem->ItemDesc.u.PhysAddr);
                //

                //
                // Remove the data entry from the persisted data list.
                //

                RemoveEntryList(&PdListItem->Entry);
                BlMmFreeHeap(PdListItem);
                return STATUS_SUCCESS;
            }

            Instance += 1;
        }

        BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
    }

    return STATUS_NOT_FOUND;
}

NTSTATUS
BlPdFreeData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in PGENERIC_BUFFER PersistentDataBuffer
    )

/*++

Routine Description:

    This routine frees a persistent memory buffer that was allocated by
    BlPdAllocateData.

Arguments:

    BootApplication - Supplies an optional pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    PersistentDataBuffer - Supplies the persistent data buffer to free.

Return Value:

    STATUS_SUCCESS if the persistent data buffer was successfully allocated,
        otherwise an error status describing the reason for the failure is
        returned.

--*/

{

    ULONG Length;
    PLIST_ENTRY NextEntry;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    PHYSICAL_ADDRESS PhysicalAddress;

    //
    // Sanitize parameters.
    //

    if ((PersistentDataBuffer == NULL) ||
        (PersistentDataBuffer->Buffer == NULL) ||
        (PersistentDataBuffer->DataSize == 0) ||
        (DataType == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate the the preceeding header matches the specified parameters.
    //

    PdItem = (PBAPD_ITEM_V1)
        ((PUCHAR)PersistentDataBuffer->Buffer - BAPD_ITEM_V1_SIZE_ALIGNED);

    if ((BootApplication != NULL) &&
        (RtlEqualMemory(BootApplication,
                        &PdItem->Application,
                        sizeof(GUID)) == FALSE)) {

        return STATUS_UNSUCCESSFUL;
    }

    if (RtlEqualMemory(DataType, &PdItem->DataType, sizeof(GUID)) == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Determine the physical address of this persistent data item, remove it
    // from the list, then unmap and free it.
    //

    if (BlMmTranslateVirtualAddress(PdItem, &PhysicalAddress) == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    Length = PersistentDataBuffer->DataSize + BAPD_ITEM_V1_SIZE_ALIGNED;
    NextEntry = BlpPdListHead.Flink;
    while (NextEntry != &BlpPdListHead) {
        PdListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        if ((PdListItem->ItemDesc.u.PhysAddr.QuadPart ==
             PhysicalAddress.QuadPart) &&
            (PdListItem->ItemDesc.Length == Length)) {

            RemoveEntryList(NextEntry);
            BlMmUnmapVirtualAddress(PdItem, Length);
            BlMmFreePhysicalPages(PhysicalAddress);
            BlMmFreeHeap(PdListItem);
            return STATUS_SUCCESS;
        }

        NextEntry = NextEntry->Flink;
    }

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS
BlPdQueryData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in ULONG Index,
    __inout PGENERIC_BUFFER PersistentDataBuffer
    )

/*++

Routine Description:

    This routine copies persistent data into the supplied buffer.

Arguments:

    BootApplication - Supplies a pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    Index - Supplies a count for the instance of data, in case more than one
        instance of a particular BootApplication/DataType exists.

    PersistentDataBuffer - Supplies a buffer to copy the persistent data into.

Return Value:

    STATUS_SUCCESS if the data was successfully copied from persistent memory.
    STATUS_NOT_FOUND if the data was not found.
    STATUS_INVALID_PARAMETER if the function parameters are inconsistent.
    STATUS_BUFFER_TOO_SMALL if the buffer is too small to hold the data.

--*/

{

    ULONG Instance;
    PLIST_ENTRY NextEntry;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    NTSTATUS Status;

    //
    // Sanitize parameters.
    //

    if ((PersistentDataBuffer == NULL) ||
        (DataType == NULL) ||
        ((PersistentDataBuffer->BufferSize > 0) &&
         (PersistentDataBuffer->Buffer == NULL))) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Iterate through the persistent data items looking for a match.
    //

    Instance = 0;
    PdItem = NULL;
    NextEntry = BlpPdListHead.Flink;
    while (NextEntry != &BlpPdListHead) {
        PdListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        NextEntry = NextEntry->Flink;
        Status = BlMmMapPhysicalAddress(&PdItem,
                                        PdListItem->ItemDesc.u.PhysAddr,
                                        PdListItem->ItemDesc.Length,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Make sure this is the correct version.
        //

        if ((PdItem->Revision == 0) || (PdItem->Revision > BAPD_ITEM_VERSION)) {
            BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
            continue;
        }

        //
        // We found a data type match, optionally check if it's the correct
        // application.
        //

        if (RtlEqualMemory(DataType, &PdItem->DataType, sizeof(GUID))) {
            if ((BootApplication != NULL) &&
                (RtlEqualMemory(BootApplication,
                                &PdItem->Application,
                                sizeof(GUID)) == FALSE)) {

                BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
                continue;
            }

            //
            // Check if this is the instance the caller is looking for.
            //

            if (Index == Instance) {

                //
                // Return the necessary buffer size when the buffer is too
                // small.
                //

                if (PersistentDataBuffer->BufferSize < PdItem->DataSize) {
                    PersistentDataBuffer->BufferSize = PdItem->DataSize;
                    BlMmUnmapVirtualAddress(PdItem,
                                            PdListItem->ItemDesc.Length);

                    return STATUS_BUFFER_TOO_SMALL;
                }

                //
                // Copy the data to the supplied buffer.
                //

                PersistentDataBuffer->DataSize = PdItem->DataSize;
                RtlCopyMemory(PersistentDataBuffer->Buffer,
                              Add2Ptr(PdItem, PdItem->DataOffset),
                              PdItem->DataSize);

                BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
                return STATUS_SUCCESS;
            }

            Instance += 1;
        }

        BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
    }

    return STATUS_NOT_FOUND;
}

NTSTATUS
BlPdSaveData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in PGENERIC_BUFFER PersistentDataBuffer
    )

/*++

Routine Description:

    This routine allocates persistent memory and copies the supplied buffer
    contents to it such that the data is available to the calling application
    and during OS initialization.

Arguments:

    BootApplication - Supplies a pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    PersistentDataBuffer - Supplies a buffer containing the data to make
        persistent.

Return Value:

    STATUS_SUCCESS if the data was successfully copied into persistent memory,
        otherwise an error status describing the reason for the failure is
        returned.

--*/

{

    return BlpPdSaveData(BootApplication, DataType, 1, PersistentDataBuffer);
}

NTSTATUS
BlPdPersistAllocations (
    __in const GUID *BootApplicationGuid
    )

/*++

Routine Description:

    This routine creates a persistent data allocation list item describing the
    allocations on the persistent memory descriptor list. The item is marked
    with the supplied application GUID.

Arguments:

    BootApplicationGuid - Supplies a pointer to a GUID for the boot application
        requesting its allocations to be persisted.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG AllocationCount;
    PPD_ALLOCATION_LIST AllocationList;
    GUID AllocationListGuid = PD_ALLOCATION_LIST_GUID;
    PMEMORY_DESCRIPTOR Descriptor;
    GENERIC_BUFFER DescriptorBuffer;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    GENERIC_BUFFER PdBuffer;
    LIST_ENTRY PersistentMemoryMap;
    NTSTATUS Status;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&PersistentMemoryMap,
                              &DescriptorBuffer,
                              MEMORYMAP_TYPE_PERSISTENT,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto PersistAllocationsEnd;
    }

    //
    // Determine the number of persistent allocations the application made.
    //

    AllocationCount = 0;
    for (NextEntry = PersistentMemoryMap.Flink;
         NextEntry != &PersistentMemoryMap;
         NextEntry = NextEntry->Flink) {

        AllocationCount += 1;
    }

    //
    // The call to BlPdAllocateData should not add an entry to
    // MmMdlPersistentMemory.
    //

    BlInitGenericBuffer(&PdBuffer, NULL, 0);
    PdBuffer.DataSize = FIELD_OFFSET(PD_ALLOCATION_LIST, AllocationList);
    PdBuffer.DataSize +=
        (ULONG)(sizeof(PD_MEMORY_DESCRIPTOR) * AllocationCount);

    Status = BlPdAllocateData(BootApplicationGuid,
                              &AllocationListGuid,
                              MEMORY_ATTRIBUTE_NONE,
                              &PdBuffer);

    if (!NT_SUCCESS(Status)) {
        goto PersistAllocationsEnd;
    }

    //
    // Store the allocations in the newly created persistent data item.
    //

    Index = 0;
    AllocationList = (PPD_ALLOCATION_LIST)PdBuffer.Buffer;
    AllocationList->AllocationCount = AllocationCount;
    for (NextEntry = PersistentMemoryMap.Flink;
         NextEntry != &PersistentMemoryMap;
         NextEntry = NextEntry->Flink) {

        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        AllocationList->AllocationList[Index].BasePage = Descriptor->BasePage;
        AllocationList->AllocationList[Index].PageCount = Descriptor->PageCount;
        AllocationList->AllocationList[Index].Attributes =
            Descriptor->Attributes;

        AllocationList->AllocationList[Index].MemoryType =
            Descriptor->MemoryType;

        Index += 1;
    }

    ASSERT(Index == AllocationCount);

    //
    // Indicate that the allocations should not be free'd during library
    // desctruction.
    //

    PdPersistAllocations = TRUE;

PersistAllocationsEnd:
    return Status;
}

NTSTATUS
BlPdDestroyPersistentAllocations (
    __in const GUID *BootApplication
    )

/*++

Routine Description:

    This routine frees the persistent allocations described by the persistent
    data allocation list of the supplied application.

Arguments:

    BootApplication - Supplies a pointer to a GUID representing the application
        whose allocations should be destroyed.

Return Value:

    NTSTATUS code.

--*/

{

    PPD_ALLOCATION_LIST AllocationList;
    GUID AllocationListGuid = PD_ALLOCATION_LIST_GUID;
    GENERIC_BUFFER DescriptorBuffer;
    ULONG FreeCount;
    ULONGLONG Index;
    BOOLEAN MatchFound;
    LIST_ENTRY MemoryMap;
    ULONG MemoryMapTypeAllMinusPersistent;
    PMEMORY_DESCRIPTOR MmDescriptor;
    PLIST_ENTRY MmEntry;
    PPD_MEMORY_DESCRIPTOR PdDescriptor;
    PLIST_ENTRY PdEntry;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // Search through the list of persistent data items for the allocation list
    // which was created by the supplied application.
    //

    MatchFound = FALSE;
    PdItem = NULL;
    PdListItem = NULL;
    PdEntry = BlpPdListHead.Flink;
    while (PdEntry != &BlpPdListHead) {
        PdListItem = CONTAINING_RECORD(PdEntry, BAPD_LIST_ITEM, Entry);
        Status = BlMmMapPhysicalAddress(&PdItem,
                                        PdListItem->ItemDesc.u.PhysAddr,
                                        PdListItem->ItemDesc.Length,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            goto DestroyPersistentAllocationsEnd;
        }

        //
        // Check to see that this persistent data item has the correct version,
        // is an allocation list and that it belongs to the correct application.
        //

        if ((PdItem->Revision != 0) &&
            (PdItem->Revision <= BAPD_ITEM_VERSION)) {

            if (RtlEqualMemory(&AllocationListGuid,
                               &PdItem->DataType,
                               sizeof(GUID)) != FALSE) {

                if (RtlEqualMemory(BootApplication,
                                   &PdItem->Application,
                                   sizeof(GUID)) != FALSE) {

                    //
                    // A match was found, break out of the loop.
                    //

                    MatchFound = TRUE;
                    break;
                }
            }
        }

        BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
        PdEntry = PdEntry->Flink;
    }

    if (MatchFound == FALSE) {
        Status = STATUS_NOT_FOUND;
        goto DestroyPersistentAllocationsEnd;
    }

    //
    // Get the allocation list from the persistent data item.
    //

    AllocationList = (PPD_ALLOCATION_LIST)Add2Ptr(PdItem, PdItem->DataOffset);

    //
    // Generate a memory map of all allocated lists minus the persistent list.
    //

    MemoryMapTypeAllMinusPersistent = MEMORYMAP_TYPE_ALL;
    MemoryMapTypeAllMinusPersistent &= ~MEMORYMAP_TYPE_PERSISTENT;
    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&MemoryMap,
                              &DescriptorBuffer,
                              MemoryMapTypeAllMinusPersistent,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto DestroyPersistentAllocationsEnd;
    }

    //
    // Search the memory map for the memory descriptors corresponding to the
    // ranges describes by the persistent allocation list and call the
    // appropriate free routine on each allocation.
    //

    Index = 0;
    FreeCount = 0;
    for (MmEntry = MemoryMap.Flink;
         MmEntry != &MemoryMap;
         MmEntry = MmEntry->Flink) {

        PdDescriptor = &AllocationList->AllocationList[Index];
        MmDescriptor = CONTAINING_RECORD(MmEntry,
                                         MEMORY_DESCRIPTOR,
                                         ListEntry);

        //
        // If the physical base page of the MM descriptor does not match the
        // base page of the PD descriptor, then this is not a match. PD
        // descriptors should not be coalesced.
        //

        if (MmDescriptor->BasePage != PdDescriptor->BasePage) {
            continue;
        }

        //
        // Use the correct 'free' routine to destroy the allocation.
        //

        if (MmDescriptor->MappedBasePage != 0) {
            VirtualAddress = (PVOID)(ULONG_PTR)
                (MmDescriptor->MappedBasePage << PAGE_SHIFT);

            Status = BlMmFreePages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart = MmDescriptor->BasePage << PAGE_SHIFT;
            Status = BlMmFreePhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            goto DestroyPersistentAllocationsEnd;
        }

        Index += 1;
        FreeCount += 1;
    }

    //
    // If any allocations were free'd then return success. It may be that some
    // allocations were already free'd by the application before this routine
    // was called, so it would be incorrect to assert that every element of the
    // allocation list was free'd.
    //

    if (FreeCount != 0) {
        Status = STATUS_SUCCESS;
        goto DestroyPersistentAllocationsEnd;
    }

    //
    // If the free count is zero, then it may be that the application is trying
    // to destroy its own allocation list. In that case, the descriptors are on
    // the persistent memory list. Search that list now.
    //

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&MemoryMap,
                              &DescriptorBuffer,
                              MEMORYMAP_TYPE_PERSISTENT,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto DestroyPersistentAllocationsEnd;
    }

    //
    // Search the memory map for the memory descriptors corresponding to the
    // ranges describes by the persistent allocation list and call the
    // appropriate free routine on each allocation.
    //

    Index = 0;
    FreeCount = 0;
    for (MmEntry = MemoryMap.Flink;
         MmEntry != &MemoryMap;
         MmEntry = MmEntry->Flink) {

        PdDescriptor = &AllocationList->AllocationList[Index];
        MmDescriptor = CONTAINING_RECORD(MmEntry,
                                         MEMORY_DESCRIPTOR,
                                         ListEntry);

        //
        // If the physical base page of the MM descriptor does not match the
        // base page of the PD descriptor, then this is not a match. PD
        // descriptors should not be coalesced.
        //

        if (MmDescriptor->BasePage != PdDescriptor->BasePage) {
            continue;
        }

        //
        // Use the correct 'free' routine to destroy the allocation.
        //

        if (MmDescriptor->MappedBasePage != 0) {
            VirtualAddress = (PVOID)(ULONG_PTR)
                (MmDescriptor->MappedBasePage << PAGE_SHIFT);

            Status = BlMmFreePersistentPages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart = MmDescriptor->BasePage << PAGE_SHIFT;
            Status = BlMmFreePersistentPhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            goto DestroyPersistentAllocationsEnd;
        }

        Index += 1;
        FreeCount += 1;
    }

    //
    // It could still be the case that no descriptors were free'd. If this is
    // so, then the application must have already free'd them. This is OK;
    // return success.
    //

    Status = STATUS_SUCCESS;

DestroyPersistentAllocationsEnd:

    //
    // Destroy the persistent data item describing this persistent allocation
    // list.
    //
    // N.B. This should only occur on success. Otherwise, there could be a
    //      situation in which the allocation list item gets destroyed but some
    //      persistent allocations do not get free'd. This could cause problems
    //      in the parent when the parent asks the firmware for one of the pages
    //      that did not get free'd.
    //
    //      In this same failure case, the parent will alo try to allocate the
    //      persistent allocations that did get free'd back to the firmware.
    //      This is not as bad because the parent will do a 'library only'
    //      allocation on the page, which won't talk to the firmware. A
    //      subsequent free on this 'library only' allocation, however, could
    //      cause an error when calling MmFwFreePage. There is currently no good
    //      way to avoid this, but the parent should generally not be calling
    //      'free' on a (grand)child's persistent allocations.
    //

    if (NT_SUCCESS(Status)) {
        RemoveEntryList(PdEntry);
        BlMmUnmapVirtualAddress(PdItem, PdListItem->ItemDesc.Length);
        BlMmFreePhysicalPages(PdListItem->ItemDesc.u.PhysAddr);
        BlMmFreeHeap(PdListItem);
    }

    return Status;
}

//
// ---------------------------------------------------------- Library Functions
//

NTSTATUS
BlpPdDestroy (
    VOID
    )

/*++

Routine Description:

    This routine allocates a persistent data array which gets returned to the
    calling application in the return arguments.

Arguments:

    None

Return Value:

    STATUS_SUCCESS

--*/

{

    PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    ULONGLONG PageCount;
    ULONGLONG PhysicalBasePage;
    NTSTATUS Status;

    BaReturnArgs = NULL;

    //
    // Populate the persisted data array that will be passed on to the calling
    // application.
    //

    Status = PdPopulateArrayFromList(&BlpPdListHead,
                                     &PhysicalBasePage,
                                     &PageCount);

    if (!NT_SUCCESS(Status)) {
        goto BlpPdDestroyExit;
    }

    //
    // Return the persistent data information in the application's
    // return arguments.
    //

    BaReturnArgs = BlGetApplicationReturnArguments();
    BaReturnArgs->PersistentData.PageCount = PageCount;
    BaReturnArgs->PersistentData.PhysicalBasePage = PhysicalBasePage;

BlpPdDestroyExit:

    //
    // Clean up the persistent allocations if the creation of the return
    // arguments persistent data fails or if the persistent allocations were not
    // flagged to be persisted. In both cases, no persistent data item
    // describing them will be passed to the parent.
    //

    if (!NT_SUCCESS(Status) || (PdPersistAllocations == FALSE)) {
        PdDestroyAllocations();
    }

    return Status;
}

NTSTATUS
BlpPdFreeDataList (
    __in PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This routine frees an item descriptor linked list allocated by
    BlpPdQueryDataList.

Arguments:

    ListHead - Supplies a persistent data linked list.

Return Value:

    STATUS_SUCCESS when all allocations and mappings are successfully freed.

    Other error code in case of error.

--*/

{

    PBAPD_LIST_ITEM ListItem;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = STATUS_SUCCESS;
    NextEntry = ListHead->Flink;
    while (NextEntry != ListHead) {
        ListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        Status = BlMmUnmapVirtualAddress(ListItem->ItemDesc.u.VirtualAddr,
                                         ListItem->ItemDesc.Length);

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        NextEntry = NextEntry->Flink;
        BlMmFreeHeap(ListItem);
    }

    return ReturnStatus;
}

NTSTATUS
BlpPdInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initilizes the boot library persistent data support.

Arguments:

    None

Return Value:

    STATUS_SUCCESS

--*/

{

    InitializeListHead(&BlpPdListHead);
    PdPersistAllocations = FALSE;
    return STATUS_SUCCESS;
}

NTSTATUS
BlpPdParseReturnArguments (
    __in PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs
    )

/*++

Routine Description:

    This routine parses an application's return arguments, and reallocates
    the items returned as persisted data.

Arguments:

    BaReturnArgs - Supplies a pointer to the application's return arguments.

Return Value:

    None

--*/

{

    ULONG AllocationFlags;
    GUID AllocationListGuid = PD_ALLOCATION_LIST_GUID;
    EVENT_DATA_DESCRIPTOR EventData[1];
    ULONG Index;
    ULONG Length;
    ULONGLONG MappingSize;
    PBAPD_ARRAY_HEADER PdArrayHeader = NULL;
    PHYSICAL_ADDRESS PdArrayPhysicalAddress;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_LIST_ITEM PdListItem;
    ULONG PdPageCount;
    PHYSICAL_ADDRESS PdPhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    ReturnStatus = STATUS_SUCCESS;

    //
    // Bail if there is no persistent data.
    //

    if (BaReturnArgs->PersistentData.PageCount == 0) {
        return STATUS_SUCCESS;
    }

    //
    // When allocating each persisted region, request a library only
    // allocation. The memory region is still allocated in the firmware's
    // viewpoint and a firmware reallocation request will fail.
    //

    AllocationFlags = MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_ALLOCATION |
                      MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS;

    PdArrayPhysicalAddress.QuadPart =
        BaReturnArgs->PersistentData.PhysicalBasePage << PAGE_SHIFT;

    ReturnStatus = BlMmAllocatePhysicalPages(
                       &PdArrayPhysicalAddress,
                       BaReturnArgs->PersistentData.PageCount,
                       MEMORY_TYPE_MISC_DATA,
                       AllocationFlags,
                       1);

    if (!NT_SUCCESS(ReturnStatus)) {
        goto PdParseReturnArgumentsEnd;
    }

    MappingSize = (BaReturnArgs->PersistentData.PageCount << PAGE_SHIFT);
    ReturnStatus = BlMmMapPhysicalAddress(&PdArrayHeader,
                                          PdArrayPhysicalAddress,
                                          MappingSize,
                                          MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(ReturnStatus)) {
        goto PdParseReturnArgumentsEnd;
    }

    //
    // Loop through the returning boot applications persistent data items,
    // and reallocate each item and add it to the library's persistent data
    // list.
    //
    // For each persisted region, additionally request that the library does
    // not free the memory allocation.
    //

    ReturnStatus = STATUS_SUCCESS;
    AllocationFlags |= MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;
    for (Index = 0; Index < PdArrayHeader->ItemCount; Index += 1) {
        Length = PdArrayHeader->ItemDesc[Index].Length;
        PdPageCount = (Length + (PAGE_SIZE - 1)) / PAGE_SIZE;
        PdPhysicalAddress = PdArrayHeader->ItemDesc[Index].u.PhysAddr;

        //
        // This is a persistent data item allocation that includes space for
        // internal metadata. This allocation should NOT use
        // BlMmAllocatePersistentPhysicalPages. That API should only be used
        // for persistent allocations with external metadata - typically to be
        // used by an application and not the boot library.
        //

        Status = BlMmAllocatePhysicalPages(&PdPhysicalAddress,
                                           PdPageCount,
                                           MEMORY_TYPE_PERSISTENT_DATA,
                                           AllocationFlags,
                                           1);

        //
        // Add the persisted memory region to the internal list.  If a failure
        // is experienced, proceed with the rest of the array, remembering the
        // last error.
        //

        if (NT_SUCCESS(Status)) {
            PdListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
            if (PdListItem != NULL) {
                PdListItem->ItemDesc.u.PhysAddr = PdPhysicalAddress;
                PdListItem->ItemDesc.Length = Length;
                InsertTailList(&BlpPdListHead, &PdListItem->Entry);

            } else {
                ReturnStatus = STATUS_NO_MEMORY;
            }

        } else {
            ReturnStatus = Status;
        }

        //
        // Map the persistent data to see if it is an allocation list.
        //

        Status = BlMmMapPhysicalAddress(&PdItem,
                                        PdPhysicalAddress,
                                        (PdPageCount << PAGE_SHIFT),
                                        MEMORY_ATTRIBUTE_NONE);

         if (!NT_SUCCESS(Status)) {
             ReturnStatus = Status;
             continue;
         }

         //
         // If it is an allocation list, persist the allocations. Otherwise,
         // unmap the persistent data item and continue.
         //

         if (RtlEqualMemory(&AllocationListGuid,
                            &PdItem->DataType,
                            sizeof(GUID)) != FALSE) {

             Status = PdPersistAllocationList(PdItem);
             if (!NT_SUCCESS(Status)) {
                 ReturnStatus = Status;
             }
         }

         Status = BlMmUnmapVirtualAddress(PdItem, (PdPageCount << PAGE_SHIFT));
         if (!NT_SUCCESS(Status)) {
             ReturnStatus = Status;
         }
    }

    //
    // Unmap and free the memory allocated for the PD array.
    //

    Status = BlMmUnmapVirtualAddress(PdArrayHeader, MappingSize);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlMmFreePhysicalPages(PdArrayPhysicalAddress);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

PdParseReturnArgumentsEnd:

    //
    // Log ETW event indicating error processing persistent data.
    //

    if (!NT_SUCCESS(ReturnStatus)) {
        EventDataDescCreate(&EventData[0],
                            &ReturnStatus,
                            sizeof(ReturnStatus));

        BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                      &BOOT_PROCESS_PERSISTENT_DATA_FAILURE,
                      NULL,
                      1,
                      EventData);
    }

    return ReturnStatus;
}

NTSTATUS
BlpPdSaveData (
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType,
    __in ULONG PersistentDataVersion,
    __in PGENERIC_BUFFER PersistentDataBuffer
    )

/*++

Routine Description:

    This routine allocates persistent memory and copies the supplied buffer
    contents to it such that the data is available to the calling application
    and during OS initialization.

Arguments:

    BootApplication - Supplies a pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

    PersistentDataVersion - Supplies the data structure version to use.

    PersistentDataBuffer - Supplies a buffer containing the data to make
        persistent.

Return Value:

    STATUS_SUCCESS if the data was successfully copied into persistent memory,
        otherwise an error status describing the reason for the failure is
        returned.

--*/

{

    ULONG AllocationFlags;
    ULONG DataOffset;
    ULONG Length;
    ULONG PageCount;
    PBAPD_ITEM_V1 PdItem;
    PBAPD_ITEM_V2 PdItemV2;
    PBAPD_LIST_ITEM PdListItem;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    //
    // Sanitize parameters.
    //

    if ((PersistentDataBuffer == NULL) ||
        (PersistentDataBuffer->Buffer == NULL) ||
        (PersistentDataBuffer->DataSize == 0) ||
        (DataType == NULL) ||
        (PersistentDataVersion == 0) ||
        (PersistentDataVersion > BAPD_ITEM_VERSION)) {

        return STATUS_INVALID_PARAMETER;
    }

    if (PersistentDataVersion == 2) {
        DataOffset = sizeof(BAPD_ITEM_V2);

    } else {
        DataOffset = sizeof(BAPD_ITEM_V1);
    }

    Status = RtlULongAdd(PersistentDataBuffer->DataSize, DataOffset, &Length);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Allocate internal pages for the supplied data.  All applications do
    // not execute with virtual address translation requiring persisted
    // allocations to be physical.  Set the 'library only free' free flag on
    // the allocation request to persist the allocation across library
    // destruction.  Copy the supplied contents into the internal buffer after
    // explicitly mapping the region.
    //
    // N.B. This is a persistent data item allocation that includes space for
    //      internal metadata. This allocation should NOT use
    //      BlMmAllocatePersistentPhysicalPages. That API should only be used
    //      for persistent allocations with external metadata - typically to be
    //      used by an application and not the boot library.
    //

    if (ALIGN_RANGE_UP(Length, PAGE_SIZE) < Length) {
        return STATUS_INTEGER_OVERFLOW;
    }

    PageCount = ALIGN_RANGE_UP(Length, PAGE_SIZE) >> PAGE_SHIFT;
    AllocationFlags = MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       PageCount,
                                       MEMORY_TYPE_PERSISTENT_DATA,
                                       AllocationFlags,
                                       1);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    PdItem = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
    Status = BlMmMapPhysicalAddress(&PdItem,
                                    PhysicalAddress,
                                    Length,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    //
    // If the physical pages cannot be virtually mapped, free the physical
    // pages. Clear the 'library only free' flag for the memory region to
    // force the pages to be returned back to the firmware and not just
    // deleted from the library.
    //

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        return Status;
    }

    //
    // Initialize the internal header for the allocation and copy the supplied
    // buffer to the internal buffer.
    //

    RtlZeroMemory(PdItem, Length);
    PdItem->DataSize = PersistentDataBuffer->DataSize;
    PdItem->Revision = PersistentDataVersion;
    PdItem->DataOffset = DataOffset;
    PdItem->DataType = *DataType;
    RtlCopyMemory((PUCHAR)PdItem + PdItem->DataOffset,
                  PersistentDataBuffer->Buffer,
                  PdItem->DataSize);

    if (BootApplication != NULL) {
        PdItem->Application = *BootApplication;
    }

    if (PersistentDataVersion >= 2) {
        PdItemV2 = (PBAPD_ITEM_V2)PdItem;
        Status = BlGetTime(&PdItemV2->Time, NULL);
        if (!NT_SUCCESS(Status)) {
            BlMmUnmapVirtualAddress(PdItem, Length);
            BlMmFreePhysicalPages(PhysicalAddress);
            return Status;
        }

        PdItemV2->PerformanceCounter = BlArchGetPerformanceCounter();
    }

    BlMmUnmapVirtualAddress(PdItem, Length);

    //
    // Add this persistent data to our list. If buffer for the persistent
    // item cannot be allocated, clear the 'library only free' flag for the
    // memory region to force the pages to be returned back to the firmware.
    //

    PdListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
    if (PdListItem == NULL) {
        BlMmFreePhysicalPages(PhysicalAddress);
        return STATUS_NO_MEMORY;
    }

    PdListItem->ItemDesc.u.PhysAddr = PhysicalAddress;
    PdListItem->ItemDesc.Length = Length;
    InsertTailList(&BlpPdListHead, &PdListItem->Entry);
    return STATUS_SUCCESS;
}

NTSTATUS
BlpPdQueryData (
    __out PBAPD_ITEM_DESCRIPTOR DataDescriptor,
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType
    )

/*++

Routine Description:

    This routine returns a persistent data buffer descriptor for the requested
    persistent data.

Arguments:

    DataDescriptor - Supplies a persistent data descriptor which will be
        populated by this routine on successful return.

        N.B. The persistent data buffer should be accessed by the virtual
             address described in the data descriptor.

    BootApplication - Supplies an optional pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

Return Value:

    STATUS_SUCCESS if the data was successfully returned to the caller.

    STATUS_NOT_FOUND if the data was not found.

    STATUS_INVALID_PARAMETER if the function parameters are inconsistent.

    STATUS_DUPLICATE_NAME if more than one buffer exists with the specified
        boot application and data type.

--*/

{

    LIST_ENTRY ListHead;
    PBAPD_LIST_ITEM ListItem;
    NTSTATUS Status;

    if (ARGUMENT_PRESENT(DataDescriptor) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Obtain a linked list of all persistent data with the specified boot
    // application and data type.  If more than one entry exists, return an
    // error to the caller.  Otherwise, return a copy of the buffer description
    // to the caller.
    //
    // Only free the list item on success.  When the buffer is released,
    // BlpPdFreeDataList will be called and the buffer will be unmapped.
    //

    Status = BlpPdQueryDataList(&ListHead, BootApplication, DataType);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    ASSERT(IsListEmpty(&ListHead) == FALSE);

    if (ListHead.Flink != ListHead.Blink) {
        Status = STATUS_DUPLICATE_NAME;
        BlpPdFreeDataList(&ListHead);

    } else {
        ListItem = CONTAINING_RECORD(ListHead.Flink, BAPD_LIST_ITEM, Entry);
        *DataDescriptor = ListItem->ItemDesc;
        BlMmFreeHeap(ListItem);
    }

    return Status;
}

NTSTATUS
BlpPdQueryDataList (
    __out PLIST_ENTRY ListHead,
    __in_opt const GUID *BootApplication,
    __in const GUID *DataType
    )

/*++

Routine Description:

    This routine allocates a persistent data item descriptor linked list
    containing all of the persistent data allocations with the specified
    attributes.

    If a persistent data allocation is version 2 or higher, the returned
    linked list will be ordered relative to the time the data was saved.

Arguments:

    ListHead - Supplies a pointer to a variable which receives the allocated
        linked list.

        N.B. Each entry in this list can be accessed by its virtual address.

    BootApplication - Supplies a pointer to a GUID representing the
        application.

    DataType - Supplies a pointer to a GUID representing the type of data.

Return Value:

    STATUS_SUCCESS if the data was successfully returned to the caller.

    STATUS_NOT_FOUND if the data was not found.

    STATUS_INVALID_PARAMETER if the function parameters are inconsistent.

--*/

{

    ULONGLONG MappingSize;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    LIST_ENTRY AllocationList;
    MEMORY_MAP_HANDLE MemoryMapHandle;
    PLIST_ENTRY NextEntry;
    PBAPD_ITEM PersistentData;
    PBAPD_LIST_ITEM ListItem;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    //
    // Validate input parameters.
    //

    if ((ARGUMENT_PRESENT(ListHead) == FALSE) ||
        (ARGUMENT_PRESENT(DataType) == FALSE)) {

        return STATUS_INVALID_PARAMETER;
    }

    InitializeListHead(ListHead);

    //
    // Get a list of persistent data allocations.
    //

    Status = BlMmEnumerateAllocations(&AllocationList,
                                      MEMORY_TYPE_PERSISTENT_DATA,
                                      &MemoryMapHandle);
    if (!NT_SUCCESS(Status)) {
        goto QueryDataListEnd;
    }

    //
    // Walk through the list of memory allocations searching for persistent
    // data allocations.  For each caller specified persistent data allocation,
    // insert an entry into the list returned to the caller.
    //

    NextEntry = AllocationList.Flink;
    while (NextEntry != &AllocationList) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        NextEntry = NextEntry->Flink;

        NT_ASSERT(MemoryDescriptor->MemoryType == MEMORY_TYPE_PERSISTENT_DATA);

        PhysicalAddress.QuadPart = MemoryDescriptor->BasePage << PAGE_SHIFT;
        MappingSize = MemoryDescriptor->PageCount << PAGE_SHIFT;
        Status = BlMmMapPhysicalAddress(&PersistentData,
                                        PhysicalAddress,
                                        MappingSize,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            goto QueryDataListEnd;
        }

        //
        // Make sure this is the correct version.
        //

        if ((PersistentData->Revision == 0) ||
            (PersistentData->Revision > BAPD_ITEM_VERSION)) {

            BlMmUnmapVirtualAddress(PersistentData, MappingSize);
            continue;
        }

        //
        // Check to see if the persistent data memory allocation is the
        // caller's data type.  Additionally check the application type,
        // if specified.
        //

        if (RtlEqualMemory(DataType,
                           &PersistentData->DataType,
                           sizeof(GUID)) == FALSE) {

            BlMmUnmapVirtualAddress(PersistentData, MappingSize);
            continue;
        }

        if ((ARGUMENT_PRESENT(BootApplication) != FALSE) &&
            (RtlEqualMemory(BootApplication,
                            &PersistentData->Application,
                            sizeof(GUID)) == FALSE)) {

            BlMmUnmapVirtualAddress(PersistentData, MappingSize);
            continue;
        }

        //
        // Insert an entry into the item descriptor linked list.
        //

        ListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
        if (ListItem == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            BlMmUnmapVirtualAddress(PersistentData, MappingSize);
            goto QueryDataListEnd;
        }

        ASSERT((ULONG)MappingSize == MappingSize);

        ListItem->ItemDesc.Length = (ULONG)MappingSize;
        ListItem->ItemDesc.u.VirtualAddr = PersistentData;
        PdInsertListItem(ListHead, ListItem);
    }

    if (IsListEmpty(ListHead) != FALSE) {
        Status = STATUS_NOT_FOUND;

    } else {
        Status = STATUS_SUCCESS;
    }

QueryDataListEnd:
    BlMmReleaseMemoryMap(&MemoryMapHandle);

    if (!NT_SUCCESS(Status)) {
        BlpPdFreeDataList(ListHead);
        InitializeListHead(ListHead);
    }

    return Status;
}

NTSTATUS
BlpPdReleaseData (
    __in PBAPD_ITEM_DESCRIPTOR DataDescriptor
    )

/*++

Routine Description:

    This routine releases a persistent data descriptor returned by
    BlpPdQueryData.

Arguments:

    DataDescriptor - Supplies a persistent data descriptor.

Return Value:

    STATUS_SUCCESS when all allocations and mappings are successfully freed.

    Other error code in case of error.

--*/

{

    LIST_ENTRY ListHead;
    PBAPD_LIST_ITEM ListItem;
    NTSTATUS Status;

    if (ARGUMENT_PRESENT(DataDescriptor) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Unmap the persistent data allocation by calling BlpPdFreeDataList.  This
    // requires a heap-allocated data descriptor list which can be freed by the
    // API.
    //

    ListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
    if (ListItem == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    InitializeListHead(&ListHead);
    ListItem->ItemDesc = *DataDescriptor;
    InsertTailList(&ListHead, &ListItem->Entry);
    Status = BlpPdFreeDataList(&ListHead);
    return Status;
}

//
// --------------------------------------------------------- Internal Functions
//

VOID
PdInsertListItem (
    __inout PLIST_ENTRY ListHead,
    __in PBAPD_LIST_ITEM InsertItem
    )

/*++

Routine Description:

    This routine inserts a persistent data descriptor into a linked list.

    If the item descriptor is version 1, it is inserted at the end of the
    linked list.  If the descriptor is greater than version 1, it is inserted
    in relative order based its the time field (smaller values at the front of
    the list).

Arguments:

    ListHead - Supplies a pointer to a variable which receives the allocated
        linked list.

        N.B. Each entry in this list can be accessed by its virtual address.

    ItemDescriptor - Supplies a persistent data allocation.

Return Value:

    None.

--*/

{

    PBAPD_ITEM_V1 ListEntry;
    PBAPD_ITEM_V2 ListEntryV2;
    PBAPD_LIST_ITEM ListItem;
    PLIST_ENTRY NextEntry;
    PBAPD_ITEM_V1 PdEntry;
    PBAPD_ITEM_V2 PdEntryV2;

    //
    // If the data entry is version 1, insert the item at the end of the list.
    //

    PdEntry = InsertItem->ItemDesc.u.VirtualAddr;
    if (PdEntry->Revision == 1) {
        InsertTailList(ListHead, &InsertItem->Entry);
        return;
    }

    PdEntryV2 = (PBAPD_ITEM_V2)PdEntry;

    //
    // Walk the linked list, inserting the new entry before the first entry
    // with a larger time.  Insert all version 2+ entries before all version
    // 1 entries.
    //

    NextEntry = ListHead->Flink;
    while (NextEntry != ListHead) {
        ListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        ListEntry = ListItem->ItemDesc.u.VirtualAddr;
        if (ListEntry->Revision == 1) {
            InsertItem->Entry.Flink = NextEntry;
            InsertItem->Entry.Blink = NextEntry->Blink;
            NextEntry->Blink->Flink = &InsertItem->Entry;
            NextEntry->Blink = &InsertItem->Entry;
            return;
        }

        ListEntryV2 = (PBAPD_ITEM_V2)ListEntry;
        if (ListEntryV2->PerformanceCounter > PdEntryV2->PerformanceCounter) {
            InsertItem->Entry.Flink = NextEntry;
            InsertItem->Entry.Blink = NextEntry->Blink;
            NextEntry->Blink->Flink = &InsertItem->Entry;
            NextEntry->Blink = &InsertItem->Entry;
            return;
        }

        NextEntry = NextEntry->Flink;
    }

    InsertTailList(ListHead, &InsertItem->Entry);
    return;
}

NTSTATUS
PdDestroyAllocations (
    VOID
    )

/*++

Routine Description:

    This routine will loop through the persistent memory list and free any
    persistent allocations made by the application. This will not free
    persistent data items as these may contain logging information that is meant
    to persist even when the application is not successful.

Arguments:

    None.

Return Value:

    NTSTATUS Code.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    GENERIC_BUFFER DescriptorBuffer;
    PLIST_ENTRY NextEntry;
    LIST_ENTRY PersistentMemoryMap;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&PersistentMemoryMap,
                              &DescriptorBuffer,
                              MEMORYMAP_TYPE_PERSISTENT,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    for (NextEntry = PersistentMemoryMap.Flink;
         NextEntry != &PersistentMemoryMap;
         NextEntry = NextEntry->Flink) {

        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        //
        // Free the allocation with the correct routine based on its properties.
        //

        if (Descriptor->MappedBasePage != 0) {
            VirtualAddress =
                (PVOID)(ULONG_PTR)(Descriptor->MappedBasePage << PAGE_SHIFT);

            Status = BlMmFreePersistentPages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart = Descriptor->BasePage << PAGE_SHIFT;
            Status = BlMmFreePersistentPhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    return Status;
}

NTSTATUS
PdPersistAllocationList (
    __in PBAPD_ITEM_V1 PersistentData
    )

/*++

Routine Description:

    This routine reads a PD_ALLOCATION_LIST_GUID type persistent data item and
    re-allocates the allocations described by the memory descriptors stored in
    the item. These re-allocations are "library only" allocations because the
    memory has already been allocated from the firmware's perspective.

Arguments:

    PersistentData - Supplies a pointer to a persistent data item that holds a
        list of memory allocations that need to be persisted.

Return Value:

    NTSTATUS Code.

--*/

{

    PPD_ALLOCATION_LIST AllocationList;
    ULONG Attributes;
    ULONG DefaultAttributes;
    ULONGLONG Index;
    PPD_MEMORY_DESCRIPTOR MemoryDescriptor;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = STATUS_SUCCESS;

#if DBG

    GUID AllocationListGuid = PD_ALLOCATION_LIST_GUID;

    ASSERT(RtlEqualMemory(&AllocationListGuid,
                          &PersistentData->DataType,
                          sizeof(GUID)) != FALSE);

#endif

    AllocationList = (PPD_ALLOCATION_LIST)Add2Ptr(PersistentData,
                                                  PersistentData->DataOffset);

    //
    // These allocations were already allocated from the firmware's perspective,
    // set the appropriate flag so the boot library will not request these pages
    // from the firmware again.
    //

    DefaultAttributes = (MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_ALLOCATION |
                         MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    //
    // Loop through the descriptor list and make the fixed address allocations.
    //

    for (Index = 0; Index < AllocationList->AllocationCount; Index += 1) {
        MemoryDescriptor = &AllocationList->AllocationList[Index];
        PhysicalAddress.QuadPart = (MemoryDescriptor->BasePage << PAGE_SHIFT);
        Attributes = DefaultAttributes | MemoryDescriptor->Attributes;

        //
        // Only the current application's allocations should be on the
        // persistent memory descriptor list, therefore allocate these with the
        // normal non-persistent API.
        //

        Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                           MemoryDescriptor->PageCount,
                                           MemoryDescriptor->MemoryType,
                                           Attributes,
                                           0);

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    return ReturnStatus;
}

NTSTATUS
PdPopulateArrayFromList (
    __in PLIST_ENTRY ListHead,
    __out PULONGLONG PhysicalBasePage,
    __out PULONGLONG PageCount
    )

/*++

Routine Description:

    This routine allocates and populates an array of BAPD_ITEM_DESCRIPTOR that
    is kept in physical memory and can be passed on to a calling application.

Arguments:

    ListHead - Supplies a pointer to the head of the list of items that will be
        used to populate the array.

    PhyiscalBasePage - Supplies a pointer to a variable that will contain the
        physical base page of the create array.

    PageCount - Supplies a pointer to a variable that will contain the number of
        pages the array takes up in memory.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONG AllocationFlags;
    PBAPD_ARRAY_HEADER ArrayHeader;
    ULONG Index;
    ULONG ItemCount;
    PBAPD_LIST_ITEM ListItem;
    ULONGLONG LocalPageCount;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONGLONG RequiredSize;
    NTSTATUS Status;

    NT_ASSERT(ListHead != NULL);
    NT_ASSERT(PhysicalBasePage != NULL);
    NT_ASSERT(PageCount != NULL);

    Status = STATUS_SUCCESS;
    ItemCount = 0;
    NextEntry = ListHead->Flink;
    while (NextEntry != ListHead) {
        ItemCount += 1;
        NextEntry = NextEntry->Flink;
    }

    if (0 == ItemCount) {
        *PhysicalBasePage = 0;
        *PageCount = 0;
        return STATUS_SUCCESS;
    }

    RequiredSize =
        sizeof(BAPD_ARRAY_HEADER) +
        ((ItemCount - 1) * sizeof(BAPD_ITEM_DESCRIPTOR));

    LocalPageCount = ALIGN_RANGE_UP(RequiredSize, PAGE_SIZE) >> PAGE_SHIFT;

    //
    // Allocate the descriptor array that will be returned in the application's
    // return arguments.
    //
    // N.B. This allocation should NOT use BlMmAllocatePersistentPhysicalPages.
    //      That API should only be used for persistent allocations with
    //      external metadata - typically to be used by an application and not
    //      the boot library.
    //

    AllocationFlags = MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       LocalPageCount,
                                       MEMORY_TYPE_MISC_DATA,
                                       AllocationFlags,
                                       1);

    if (!NT_SUCCESS(Status)) {
        goto PdPopulateArrayFromListEnd;
    }

    Status = BlMmMapPhysicalAddress(&ArrayHeader,
                                    PhysicalAddress,
                                    LocalPageCount << PAGE_SHIFT,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        goto PdPopulateArrayFromListEnd;
    }

    //
    // Populate the array with the descriptors in the data list.
    //

    Index = 0;
    NextEntry = ListHead->Flink;
    ArrayHeader->ItemCount = ItemCount;
    while (NextEntry != ListHead) {
        ListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        RtlCopyMemory(&ArrayHeader->ItemDesc[Index],
                      &ListItem->ItemDesc,
                      sizeof(BAPD_ITEM_DESCRIPTOR));

        NextEntry = NextEntry->Flink;
        Index += 1;
    }

    BlMmUnmapVirtualAddress(ArrayHeader, LocalPageCount << PAGE_SHIFT);
    *PhysicalBasePage = (((ULONG64) PhysicalAddress.QuadPart) >> PAGE_SHIFT);
    *PageCount = LocalPageCount;

PdPopulateArrayFromListEnd:

    //
    // Free the data list.
    //

    NextEntry = RemoveTailList(ListHead);
    while (NextEntry != ListHead) {
        ListItem = CONTAINING_RECORD(NextEntry, BAPD_LIST_ITEM, Entry);
        BlMmFreeHeap(ListItem);
        NextEntry = RemoveTailList(ListHead);
    }

    return Status;
}

