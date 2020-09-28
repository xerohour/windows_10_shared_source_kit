/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pagalloc.c

Abstract:

    Implementation of the Boot Environment Library's Page Allocator.

    The Page Allocator provides the support for allocating large contiguous
    blocks of memory as well as the ability to allocate memory with
    alignment or cachability requirements.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "pagalloc.h"
#include "mm.h"

// ---------------------------------------------------------------- Definitions

#define ZEROING_BYTE_RANGE_LENGTH (_512MB)

//
// Mask for the two firmware memory map types that a calling application can
// request.
//

#define MEMORYMAP_TYPE_ALL_FIRMWARE (MEMORYMAP_TYPE_FIRMWARE | \
                                     MEMORYMAP_TYPE_FIRMWARE_NOT_COALESCED)

// -------------------------------------------------------------------- Globals

//
// Global descriptor lists for the four types of memory previously discussed.
//

#if !defined(MM_MIN)

MEMORY_DESCRIPTOR_LIST MmMdlMappedAllocated;
MEMORY_DESCRIPTOR_LIST MmMdlMappedUnallocated;
MEMORY_DESCRIPTOR_LIST MmMdlFwAllocationTracker;

#endif

MEMORY_DESCRIPTOR_LIST MmMdlUnmappedAllocated;
MEMORY_DESCRIPTOR_LIST MmMdlUnmappedUnallocated;
MEMORY_DESCRIPTOR_LIST MmMdlReservedAllocated;
MEMORY_DESCRIPTOR_LIST MmMdlBadMemory;
MEMORY_DESCRIPTOR_LIST MmMdlTruncatedMemory;
MEMORY_DESCRIPTOR_LIST MmMdlPersistentMemory;

//
// The minimum number of pages that the page allocator's pool can be
// grown by at one time.  This is configurable by the boot application.
//

ULONG PapMinimumAllocationCount;

//
// The maximum physical page that the page allocator should use.  This is set
// as a result of truncation requests coming from boot configuration data, or
// when under certain translation modes that cannot address the entire physical
// address space.
//

ULONGLONG PapMaximumPhysicalPage;

//
// The minimum physical page that the page allocator should use.  This is set
// as a result of low memory avoidance requests coming from boot configuration
// data.
//

ULONGLONG PapMinimumPhysicalPage;

//
// Global variable indicating the initialization status of the page allocator.
//

MODULE_INITIALIZATION_STATUS PapInitializationStatus = ModuleUninitialized;

// ----------------------------------------------------------------- Prototypes

ULONGLONG
BlpMmMarkBadMemory (
    __in_ecount(Count) ULONGLONG UNALIGNED *BadPageList,
    __in ULONGLONG Count
    );

NTSTATUS
MmPapAllocateRegionFromMdlWithFallback (
    __inout_opt PMEMORY_DESCRIPTOR_LIST Destination,
    __out_opt PMEMORY_DESCRIPTOR Allocation,
    __inout PMEMORY_DESCRIPTOR_LIST Source,
    __inout PPAGE_ALLOCATION_REQUEST AllocationRequest,
    __in MEMORY_TYPE MemoryType
    );

// ----------------------------------------------- Boot Environment Library API

NTSTATUS
BlMmAllocatePages (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Pages * PAGE_SIZE))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Pages * PAGE_SIZE))
         PVOID *Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages which satisfy the caller's
    size, attribute and alignment requirements.

    When executing with address translation enabled, the address referred to
    by the parameter, Address, is virtual.

Arguments:

    Address - Supplies the preferred address for the allocation.  A preferred
        address is a virtual address if address translation is enabled.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

    STATUS_NOT_SUPPORTED if a specified allocation attribute is not supported.

--*/

{

    return BlMmAllocatePagesInRange(Address,
                                    Pages,
                                    MemoryType,
                                    Attributes,
                                    Alignment,
                                    NULL,
                                    0);
}

NTSTATUS
BlMmAllocatePagesInRange (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Pages * PAGE_SIZE))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Pages * PAGE_SIZE))
         PVOID *Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __in_opt PMEMORY_ADDRESS_RANGE Range,
    __in_opt ULONG RangeFlags
    )

/*++

Routine Description:

    This routine allocates a block of contiguous memory pages which satisfy the
    caller's size, attribute, alignment, and range requirements.

    When executing with address translation enabled, the address referred to
    by the parameter, Address, is virtual.

Arguments:

    Address - Supplies the preferred address for the allocation.  A preferred
        address is a virtual address if address translation is enabled.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

    Range - Supplies the range of base physical pages that will satisfy the
        request. The ranges are physical addresses and are inclusive (that is,
        the maximum page is included in the range). NULL indicates the default
        physical address range should be used.

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

    STATUS_NOT_SUPPORTED if a specified allocation attribute is not supported.

--*/

{

    PMEMORY_DESCRIPTOR_LIST Destination;

#if defined(MM_MIN)

    Destination = &MmMdlUnmappedAllocated;

#else

    Destination = &MmMdlMappedAllocated;

#endif

    return MmPapAllocatePagesInRange(Address,
                                     Pages,
                                     MemoryType,
                                     Attributes,
                                     Alignment,
                                     Destination,
                                     Range,
                                     RangeFlags);
}

NTSTATUS
BlMmAllocatePhysicalPages (
    __inout PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages (physical) which satisfy the
    caller's size, attribute and alignment requirements.

Arguments:

    Address - Supplies the preferred address for the allocation.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    return BlMmAllocatePhysicalPagesInRange(Address,
                                            Pages,
                                            MemoryType,
                                            Attributes,
                                            Alignment,
                                            NULL,
                                            0);
}

NTSTATUS
BlMmAllocatePhysicalPagesInRange (
    __inout PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __in_opt PMEMORY_ADDRESS_RANGE Range,
    __in_opt ULONG RangeFlags
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages (physical) which satisfy the
    caller's size, attribute, alignment, and range requirements.

Arguments:

    Address - Supplies the preferred address for the allocation.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

    Range - Supplies the range of base pages that will satisfy the request. The
        ranges are physical addresses and are inclusive (that is, the maximum
        page is included in the range). NULL indicates the default address range
        should be used.

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    return MmPapAllocatePhysicalPagesInRange(Address,
                                             Pages,
                                             MemoryType,
                                             Attributes,
                                             Alignment,
                                             &MmMdlUnmappedAllocated,
                                             Range,
                                             RangeFlags);
}

NTSTATUS
BlMmAllocatePersistentPhysicalPages (
    __inout PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages (physical) which satisfy the
    caller's size, attribute and alignment requirements. Additionally it places
    these pages on the list of persistent memory which can be persisted to a
    parent application.

Arguments:

    Address - Supplies the preferred address for the allocation.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    return BlMmAllocatePersistentPhysicalPagesInRange(Address,
                                                      Pages,
                                                      MemoryType,
                                                      Attributes,
                                                      Alignment,
                                                      NULL,
                                                      0);
}

NTSTATUS
BlMmAllocatePersistentPhysicalPagesInRange (
    __inout PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __in_opt PMEMORY_ADDRESS_RANGE Range,
    __in_opt ULONG RangeFlags
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages (physical) which satisfy the
    caller's size, attribute, alignment, and range requirements. Additionally it
    places these pages on the list of persistent memory which can be persisted
    to a parent application.

Arguments:

    Address - Supplies the preferred address for the allocation.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

    Range - Supplies the range of base pages that will satisfy the request. The
        ranges are physical addresses and are inclusive (that is, the maximum
        page is included in the range). NULL indicates the default address range
        should be used.

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    Attributes |= MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;
    return MmPapAllocatePhysicalPagesInRange(Address,
                                             Pages,
                                             MemoryType,
                                             Attributes,
                                             Alignment,
                                             &MmMdlPersistentMemory,
                                             Range,
                                             RangeFlags);
}

NTSTATUS
BlMmFreePages (
    __in PVOID Address
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocatePages.

Arguments:

    Address - Supplies the address of the allocation to free.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address was not allocated by BlMmAllocatePages.

--*/

{

    NTSTATUS Status;

    Status = MmPapFreePages(Address, MEMORYMAP_TYPE_MAPPED_ALLOCATED);

    NT_ASSERT(NT_SUCCESS(Status));

    return Status;
}

NTSTATUS
BlMmFreePersistentPages (
    __in PVOID Address
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocatePages.

Arguments:

    Address - Supplies the address of the allocation to free.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address was not allocated by BlMmAllocatePages.

--*/

{

    return MmPapFreePages(Address, MEMORYMAP_TYPE_PERSISTENT);
}

NTSTATUS
BlMmFreePhysicalPages (
    __in PHYSICAL_ADDRESS Address
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocatePhyiscalPages.

Arguments:

    Address - Supplies the address of the allocation to free.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address was not allocated by
    BlMmAllocatePhysicalPages.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfacePagalloc);
    Status = MmPapFreePhysicalPages(Address,
                                    0,
                                    MEMORYMAP_TYPE_UNMAPPED_ALLOCATED);

    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
BlpMmFreePhysicalPagesEx (
    __in PHYSICAL_ADDRESS Address,
    __in ULONGLONG PageCount
    )


/*++

Routine Description:

    This routine frees an allocation made by BlMmAllocatePhysicalPages. The
    routine will attempt to remove the region specified by the address and page
    count. This will only succeed if the region fully maps to a memory
    descriptor or if the region is a sub-region of a coalesced memory
    descriptor.

Arguments:

    Address - Supplies the address of the allocation to free.

    PageCount - Supplies the number of pages to free.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if address was not allocated by
        BlMmAllocatePhysicalPages or an invalid page count was supplied.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfacePagalloc);

    //
    // Do not allow attempts to free zero pages.
    //

    if (PageCount == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlpMmFreePhysicalPagesExEnd;
    }

    Status = MmPapFreePhysicalPages(Address,
                                    PageCount,
                                    MEMORYMAP_TYPE_UNMAPPED_ALLOCATED);

BlpMmFreePhysicalPagesExEnd:
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
BlMmFreePersistentPhysicalPages (
    __in PHYSICAL_ADDRESS Address
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocatePersistentPhyiscalPages.

Arguments:

    Address - Supplies the address of the allocation to free.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address was not allocated by
    BlMmAllocatePhysicalPages.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfacePagalloc);
    Status = MmPapFreePhysicalPages(Address,
                                    0,
                                    MEMORYMAP_TYPE_PERSISTENT);

    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
BlMmGetMemoryMap (
    __out PLIST_ENTRY MemoryMap,
    __inout PGENERIC_BUFFER DescriptorBuffer,
    __in ULONG MemoryMapType,
    __in ULONG Flags
    )

/*++

Routine Description:

    Obtains a memory map from the page allocator.

Arguments:

    MemoryMap - Supplies a pointer to a variable that receives the memory
        map list head.

    DescriptorBuffer - Supplies a pointer to a generic buffer, describing a
        contiguous memory buffer used for memory descriptor allocations for
        the returned memory map.

    MemoryMapType - Supplies the type of memory map to return.  Refer to the
        memory map types (MEMORYMAP_TYPE_*) in memtype.h for the definitions of
        the supported types.

    Flags - Supplies insertion flags.  Possible values include:

        MDL_FLAGS_SORT_BY_VIRTUAL_ADDRESS - Specifies that the returned memory
            map will be sorted by virtual addresses.

        MDL_FLAGS_SORT_BY_PHYSICAL_ADDRESS - Specifies that the returned memory
            map will be sorted by physical addresses.  This is the default
            behavior if MDL_FLAGS_SORT_BY_VIRTUAL_ADDRESS is not specified.

        MDL_FLAGS_ALLOCATE_DESCRIPTORS - Specifies that the API should allocate
            a new descriptor buffer if the caller allocated buffer is too
            small.  In the event of an allocation, the caller's buffer is
            freed when non-zero and the caller is responsible for freeing the
            allocation.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if an internal allocation fails.

    STATUS_INVALID_PARAMETER if MemoryMap is NULL or MemoryMapType is invalid.

    STATUS_BUFFER_TOO_SMALL if the caller's buffer is too small for the memory
        map and MDL_FLAGS_ALLOCATE_DESCRIPTORS was not specified.

--*/

{

    ULONG AllocationRetryCount;
    ULONG DescriptorCount;
    MEMORY_DESCRIPTOR_LIST FirmwareMdl;
    MEMORY_DESCRIPTOR_LIST Mdl;
    ULONG RequiredSize;
    NTSTATUS Status;
    ULONG Used;

    BlpInterfaceEnter(InterfacePagalloc);
    MmMdInitializeList(&FirmwareMdl, MdlTypePhysical, NULL);

    //
    // Parameter checking.
    //
    // N.B. The firmware memory map is mutually exclusive with all other
    //      memory map types.
    //

    if ((MemoryMap == NULL) ||
        ((MemoryMapType & ~MEMORYMAP_TYPE_ALL_PLUS_TRUNCATED) &&
         (MemoryMapType & ~MEMORYMAP_TYPE_ALL_FIRMWARE)) ||
        ((MemoryMapType & MEMORYMAP_TYPE_ALL_FIRMWARE) &&
         (MemoryMapType & MEMORYMAP_TYPE_ALL_PLUS_TRUNCATED)) ||
        (DescriptorBuffer == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlMmGetMemoryMapEnd;
    }

    //
    // Initially empty the given descriptor list so it can accrue descriptors
    // as this routine executes.
    //

    InitializeListHead(MemoryMap);

    //
    // We need to ensure that the memory map does not change while copying
    // each requested MDL into the caller's List.  In order to do so, we
    // need to allocate enough memory up front for every descriptor that will
    // be added to the caller's list.
    //
    // Count the number of descriptors that need to be copied, allowing for
    // retry in rare cases where the act of allocating space for a descriptor
    // buffer generates a large number of new descriptors.
    //

    AllocationRetryCount = 0;
    do {
        DescriptorCount = 0;

#if !defined(MM_MIN)

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_MAPPED_ALLOCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlMappedAllocated);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_MAPPED_UNALLOCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlMappedUnallocated);
        }

#endif

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_UNMAPPED_ALLOCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlUnmappedAllocated);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlUnmappedUnallocated);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_RESERVED_ALLOCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlReservedAllocated);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_BAD)) {
            DescriptorCount += MmMdCountList(&MmMdlBadMemory);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_TRUNCATED)) {
            DescriptorCount += MmMdCountList(&MmMdlTruncatedMemory);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_PERSISTENT)) {
            DescriptorCount += MmMdCountList(&MmMdlPersistentMemory);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_FIRMWARE)) {
            MmMdFreeList(&FirmwareMdl);
            Status = MmFwGetMemoryMap(&FirmwareMdl,
                                      MM_MEMORYMAP_FLAGS_COALESCE);

            if (!NT_SUCCESS(Status)) {
                goto BlMmGetMemoryMapEnd;
            }

            DescriptorCount = MmMdCountList(&FirmwareMdl);
        }

        if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_FIRMWARE_NOT_COALESCED)) {
            MmMdFreeList(&FirmwareMdl);
            Status = MmFwGetMemoryMap(&FirmwareMdl, MM_MEMORYMAP_FLAGS_NONE);
            if (!NT_SUCCESS(Status)) {
                goto BlMmGetMemoryMapEnd;
            }

            DescriptorCount = MmMdCountList(&FirmwareMdl);
        }

        if (DescriptorCount == 0) {
            Status = STATUS_SUCCESS; // No work to perform.
            goto BlMmGetMemoryMapEnd;
        }

        //
        // Ensure that the descriptor buffer is large enough for the entire
        // memory descriptor list.
        //

        RequiredSize = DescriptorCount * sizeof(MEMORY_DESCRIPTOR);
        if (DescriptorBuffer->BufferSize >= RequiredSize) {
            break;
        }

        //
        // If we have to allocate memory for the descriptor list, then
        // allocate enough memory for a few additional descriptors.  This will
        // help in cases where, for example, an allocation will not fit in the
        // current heap, causes a page allocator extend, and then causes
        // several page table allocations as the new extend is mapped.  All of
        // these events can add descriptors to the global MDLs, making the
        // post-allocation descriptor count higher than the one currently
        // being used to compute the allocation size.
        //

        DescriptorCount += 4;
        RequiredSize += (4 * sizeof(MEMORY_DESCRIPTOR));

        //
        // If the caller did not request a new buffer to be allocated,
        // return with the status, STATUS_BUFFER_TOO_SMALL.  Otherwise,
        // attempt to allocate a new buffer here.
        //

        if (CHECK_FLAG(Flags, MDL_FLAGS_ALLOCATE_DESCRIPTORS) == FALSE) {
            DescriptorBuffer->BufferSize = RequiredSize;
            Status = STATUS_BUFFER_TOO_SMALL;
            goto BlMmGetMemoryMapEnd;
        }

        //
        // The expectation is that in retry cases the additions to the global
        // MDLs always reflect a large amount of new space being available for
        // additional buffer allocations.  Therefore, the retry loop is
        // terminated if descriptor count growth unexpectedly occurs across
        // several consecutive allocation attempts.
        //

        AllocationRetryCount += 1;
        if (AllocationRetryCount > 4) {
            Status = STATUS_NO_MEMORY;
            goto BlMmGetMemoryMapEnd;
        }

        //
        // Allocate a sufficiently sized buffer.
        //

        if (DescriptorBuffer->BufferSize != 0) {
            BlMmFreeHeap(DescriptorBuffer->Buffer);
        }

        DescriptorBuffer->BufferSize = RequiredSize;
        DescriptorBuffer->Buffer = BlMmAllocateHeap(RequiredSize);
        if (DescriptorBuffer->Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlMmGetMemoryMapEnd;
        }

        //
        // Loop to verify that the allocated buffer can still accomodate the
        // requested memory descriptors taking into account any descriptors
        // generated during the descriptor buffer allocation.
        //

        NOTHING;

    } while (TRUE);

    //
    // Copy the requested descriptor lists(s) into the pre-allocated buffer,
    // initializing an internal MDL structure to route descriptors into the
    // descriptor list head provided by the caller.
    //
    // N.B. The mapped descriptor lists need to be added first, since
    //      MmMdlAddDescriptorToList will insert the descriptor based on
    //      the virtual address if mapped, physical address if not mapped.
    //

    if (DescriptorBuffer->Buffer != NULL) {
        RtlZeroMemory(DescriptorBuffer->Buffer, RequiredSize);
    }

    MmMdInitializeList(&Mdl, MdlTypePhysical, MemoryMap);
    Used = 0;
    Status = STATUS_SUCCESS;

#if !defined(MM_MIN)

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_MAPPED_ALLOCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlMappedAllocated,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_MAPPED_UNALLOCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlMappedUnallocated,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

#endif

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_UNMAPPED_ALLOCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlUnmappedAllocated,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlUnmappedUnallocated,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_RESERVED_ALLOCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlReservedAllocated,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_BAD)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlBadMemory,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_TRUNCATED)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlTruncatedMemory,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_PERSISTENT)) {
        Status = MmMdCopyList(&Mdl,
                              &MmMdlPersistentMemory,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_FIRMWARE)) {
        Status = MmMdCopyList(&Mdl,
                              &FirmwareMdl,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    if (CHECK_FLAG(MemoryMapType, MEMORYMAP_TYPE_FIRMWARE_NOT_COALESCED)) {
        Status = MmMdCopyList(&Mdl,
                              &FirmwareMdl,
                              DescriptorBuffer->Buffer,
                              &Used,
                              DescriptorCount,
                              Flags);

        if (!NT_SUCCESS(Status)) {
            goto BlMmGetMemoryMapEnd;
        }
    }

    //
    // Set the buffer's data size before returning.
    //

    Status = RtlULongMult(Used,
                          sizeof(MEMORY_DESCRIPTOR),
                          &DescriptorBuffer->DataSize);

    if (!NT_SUCCESS(Status)) {
        goto BlMmGetMemoryMapEnd;
    }

BlMmGetMemoryMapEnd:
    MmMdFreeList(&FirmwareMdl);
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
BlMmEnumerateAllocations (
    __out PLIST_ENTRY AllocationList,
    __in MEMORY_TYPE MemoryType,
    __out PMEMORY_MAP_HANDLE MapHandle
    )

/*++

Routine Description:

    This routine will enumerate allocated memory map for allocations matching
    supplied memory type.

Arguments:

    AllocationList - Supplies pointer to the list which will be populated with memory
        descriptors of the matching type.

    MemoryType - Supplies memory type to match.

    MapHandle - Supplies handle used to the memory map, which will be used to
         release the data.

Return Value:

    STATUS_SUCCESS when successfull.

    STATUS_NO_MEMORY if an internal allocation fails.

--*/

{

    NTSTATUS Status;
    LIST_ENTRY LocalAllocationList;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;

    BlInitGenericBuffer(&MapHandle->Descriptors, NULL, 0);
    InitializeListHead(AllocationList);

    Status = BlMmGetMemoryMap(&LocalAllocationList,
                              &MapHandle->Descriptors,
                              MEMORYMAP_TYPE_ALLOCATED,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Filter down to allocation of the matching type.
    //

    NextEntry = LocalAllocationList.Flink;
    while (NextEntry != &LocalAllocationList) {         
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        NextEntry = NextEntry->Flink;
        if (MemoryDescriptor->MemoryType == MemoryType) {
            InsertTailList(AllocationList, &MemoryDescriptor->ListEntry);
        }
    }

    return STATUS_SUCCESS;
}

VOID
BlMmReleaseMemoryMap (
    __in PMEMORY_MAP_HANDLE MapHandle
    )

/*++

Routine Description:

    This routine is used to release previously allocated memory map.

Arguments:

    MapHandle - Supplies handle to the allocated memory map.

Return Value:

    None.

--*/

{

    NT_ASSERT(MapHandle != NULL);

    if (MapHandle->Descriptors.Buffer != NULL) {
        BlMmFreeHeap(MapHandle->Descriptors.Buffer);
    }

    RtlZeroMemory(MapHandle, sizeof(MEMORY_MAP_HANDLE));
    return;
}


ULONG
BlMmQueryLargePageSize (
    VOID
    )

/*++

Routine Description:

    This routine queries the Boot Environment Library to determine the number
    of pages in a large page.

Arguments:

    None.

Return Value:

    The number of pages in a large page.

--*/

{

    return MmArchQueryLargePageSize();
}

NTSTATUS
BlMmSynchronizeMemoryMap (
    __in ULONG MemoryFlags
    )

/*++

Routine Description:

    This routine synchronizes the library's internal memory map based on the
    firmware memory map for runtime code and runtime data regions.

Arguments:

    MemoryFlags - Supplies a flag to specify which memory types to synchronize.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_CONFLICTING_ADDRESSES if conflicts cannot be resolved.

    STATUS_INSUFFICIENT_RESOURCES if a descriptor initialization failed.

    STATUS_NO_MEMORY if an internal allocation failed.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfacePagalloc);
    Status = MmFwSynchronizeMemoryMap(MemoryFlags);
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

VOID
BlMmRemoveBadMemory (
    VOID
    )

/*++

Routine Description:

    This routine reads bad memory information from BCD, then attempts to
    remove any such regions from system memory so they will not be
    available to boot applications, or the OS.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BOOLEAN AllowAccess;
    ULONGLONG UNALIGNED *BadPageList;
    ULONGLONG Count;
    ULONGLONG Index;
    ULONGLONG Inner;
    NTSTATUS Status;
    ULONGLONG Temp;

    //
    // Perform no action if the option list specifically requests that this
    // boot application be able to access and manipulate pages in the bad
    // memory list.
    //

    AllowAccess = FALSE;
    Status = BlGetBootOptionBoolean(BlGetApplicationEntry()->Options,
                                    BCDE_LIBRARY_TYPE_ALLOW_BAD_MEMORY_ACCESS,
                                    &AllowAccess);

    if (NT_SUCCESS(Status) && (AllowAccess == TRUE)) {
        return;
    }

    //
    // Fetch the bad memory information from BCD.
    //

    Status = BlpGetBootOptionIntegerList(BlGetApplicationEntry()->Options,
                                         BCDE_LIBRARY_TYPE_BAD_MEMORY_LIST,
                                         (PULONGLONG *)&BadPageList,
                                         &Count,
                                         TRUE);

    if (!NT_SUCCESS(Status)) {
        return;
    }

    //
    // Before looking for contiguous runs, first sort the bad page list and
    // remove any duplicate entries.
    //

    if (Count > 1) {
        for (Index = 0; Index < (Count - 1); Index += 1) {
            for (Inner = (Index + 1); Inner < Count; Inner += 1) {
                if (BadPageList[Index] > BadPageList[Inner]) {
                    Temp = BadPageList[Index];
                    BadPageList[Index] = BadPageList[Inner];
                    BadPageList[Inner] = Temp;
                }
            }
        }

        Index = 0;
        while ((Count > 1) && (Index < (Count - 1))) {
            if (BadPageList[Index] == BadPageList[Index + 1]) {
                RtlMoveMemory(&BadPageList[Index],
                              &BadPageList[Index + 1],
                              (SIZE_T)(Count - (Index + 1)) *
                              sizeof(ULONGLONG));

                Count -= 1;

            } else {
                Index += 1;
            }
        }
    }

    //
    // Cruft up a descriptor for the largest run of bad pages beginning at
    // the current position in the array and add it to the bad memory list.
    //

    Index = 0;
    while (Index < Count) {
        Index += BlpMmMarkBadMemory(&BadPageList[Index], Count - Index);
    }

    return;
}

NTSTATUS
BlMmAddEnclavePageRange (
    _In_ ULONGLONG BasePage,
    _In_ ULONGLONG PageCount
    )

/*++

Routine Description:

    This routine marks the specified physical page range as SGX enclave memory
    in the memory map.  Firmware may or may not have fully or partially
    reported enclave regions, as varying memory types.  This routine fails if
    any page in the range is invalid for enclave usage.  A page is considered
    invalid for enclave usage if it is already marked as any memory type that
    behaves like normal RAM, or if it is already allocated.

Arguments:

    BasePage - Supplies the first page number in the enclave region.

    PageCount - Supplies the number of pages in the enclave region.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_CONFLICTING_ADDRESSES if any page in the specified page range is
        invalid for enclave usage.

    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    ULONGLONG CurrentEndPage;
    ULONGLONG CurrentPage;
    PMEMORY_DESCRIPTOR Descriptor;
    ULONG EnclaveAttributes;
    MEMORY_TYPE EnclaveMemoryType;
    ULONGLONG EndPage;
    BOOLEAN ExtendLastDescriptor;
    LIST_ENTRY FreeDescriptorList;
    ULONGLONG LastBasePage;
    MEMORY_DESCRIPTOR ListDescriptor;
    ULONGLONG ListEndPage;
    PLIST_ENTRY ListEntry;
    UNPACKED_MEMORY_TYPE ListMemoryType;
    ULONG ListTypes;
    PMEMORY_DESCRIPTOR_LIST Mdl;
    LONG NumberOfDescriptors;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    BlpInterfaceEnter(InterfacePagalloc);
    MmEnterDescriptorLibraryApi();
    EnclaveMemoryType = MEMORY_TYPE_ENCLAVE;
    EnclaveAttributes = MEMORY_ATTRIBUTE_NONE;
    EndPage = BasePage + PageCount;
    InitializeListHead(&FreeDescriptorList);
    NumberOfDescriptors = 0;

    //
    // Check that all existing memory descriptors that overlap the enclave
    // region describe pages that are valid for enclave usage. Also determine
    // the number of descriptors that need to be allocated to describe the
    // enclave region, taking into account that the enclave region may split
    // one or two existing descriptors or be split by bad memory ranges.
    //

    CurrentPage = BasePage;
    ExtendLastDescriptor = FALSE;
    while (CurrentPage < EndPage) {
        Descriptor = MmMdFindOverlappingDescriptor(
                         CurrentPage,
                         EndPage,
                         MEMORYMAP_TYPE_ALL_PLUS_TRUNCATED,
                         &Mdl);

        if (Descriptor == NULL) {
            if (ExtendLastDescriptor == FALSE) {
                NumberOfDescriptors += 1;
            }

            break;
        }

        //
        // If the existing descriptor is not on these lists, it (probably)
        // describes allocated memory, which is not valid for enclave usage.
        // Alternatively the descriptor could describe physical pages that were
        // explicitly virtually mapped, and that were not actually allocated
        // but did appear in the firmware memory map - it's still okay to
        // reject the enclave region in this case because enclave pages should
        // never have been virtually mapped.
        //

        if ((Mdl != &MmMdlUnmappedUnallocated) &&
            (Mdl != &MmMdlReservedAllocated) &&
            (Mdl != &MmMdlBadMemory)) {

            Status = STATUS_CONFLICTING_ADDRESSES;
            goto AddEnclavePageRangeEnd;
        }

        //
        // If the existing descriptor isn't a platform region, it is allocated
        // memory which is not valid for enclave usage. However, ignore memory
        // classes that we don't understand.
        //

        ListMemoryType.MemoryType = Descriptor->MemoryType;
        if ((ListMemoryType.Class == MEMORY_CLASS_LIBRARY) ||
            (ListMemoryType.Class == MEMORY_CLASS_APPLICATION)) {

            Status = STATUS_CONFLICTING_ADDRESSES;
            goto AddEnclavePageRangeEnd;
        }

        //
        // If the existing descriptor is a platform region that behaves like
        // normal RAM, it is not valid for enclave usage.
        //

        if ((Descriptor->MemoryType == MEMORY_TYPE_FREE) ||
            (Descriptor->MemoryType == MEMORY_TYPE_FREE_ZEROED) ||
            (Descriptor->MemoryType == MEMORY_TYPE_BOOT_SERVICE_CODE) ||
            (Descriptor->MemoryType == MEMORY_TYPE_ACPI_TABLES)) {

            Status = STATUS_CONFLICTING_ADDRESSES;
            goto AddEnclavePageRangeEnd;
        }

        ListEndPage = Descriptor->BasePage + Descriptor->PageCount;

        //
        // Pages within the enclave region that are marked as bad remain so and
        // will not be used. The enclave memory type otherwise overrides all
        // other memory types, including unknown memory types.
        //

        if (Descriptor->MemoryType == MEMORY_TYPE_BAD) {

            //
            // A new descriptor is needed to describe the enclave pages up to
            // the start of the bad range.
            //

            if ((CurrentPage < Descriptor->BasePage) &&
                (ExtendLastDescriptor == FALSE)) {

                NumberOfDescriptors += 1;
            }

            ExtendLastDescriptor = FALSE;

        } else {

            //
            // Pretend the existing descriptor will be removed.
            //

            NumberOfDescriptors -= 1;

            //
            // If the existing descriptor starts before the enclave region, a
            // new descriptor is needed to describe the first portion.
            //

            if (Descriptor->BasePage < CurrentPage) {
                NumberOfDescriptors += 1;
                if ((Mdl == &MmMdlReservedAllocated) &&
                    (Descriptor->MemoryType == EnclaveMemoryType) &&
                    (Descriptor->Attributes == EnclaveAttributes) &&
                    (Descriptor->MappedBasePage == 0)) {

                    ExtendLastDescriptor = TRUE;
                }
            }

            //
            // A new descriptor is needed to describe the enclave pages up to
            // the end of the existing descriptor.
            //

            if (ExtendLastDescriptor == FALSE) {
                NumberOfDescriptors += 1;
                ExtendLastDescriptor = TRUE;
            }

            //
            // If the existing descriptor ends after the enclave region, a new
            // descriptor is needed to describe the last portion.
            //

            if ((EndPage < ListEndPage) &&
                ((Mdl != &MmMdlReservedAllocated) ||
                 (Descriptor->MemoryType != EnclaveMemoryType) ||
                 (Descriptor->Attributes != EnclaveAttributes) ||
                 (Descriptor->MappedBasePage != 0))) {

                NumberOfDescriptors += 1;
            }
        }

        CurrentPage = ListEndPage;
    }

    //
    // Allocate any additional memory descriptors that are needed.
    //

    while (NumberOfDescriptors > 0) {
        Descriptor = MmMdInitDescriptor(0, 0, 0, 0, 0);
        if (Descriptor == NULL) {
            Status = STATUS_NO_MEMORY;
            goto AddEnclavePageRangeEnd;
        }

        NumberOfDescriptors -= 1;
        InsertTailList(&FreeDescriptorList, &Descriptor->ListEntry);
    }

    //
    // Remove all memory descriptors (excluding bad regions) that are
    // completely contained within the enclave region.
    //

    ListTypes = MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED |
                MEMORYMAP_TYPE_RESERVED_ALLOCATED;

    CurrentPage = BasePage;
    while (CurrentPage < EndPage) {
        Descriptor = MmMdFindOverlappingDescriptor(CurrentPage,
                                                   EndPage,
                                                   ListTypes,
                                                   &Mdl);

        if (Descriptor == NULL) {
            break;
        }

        ListEndPage = Descriptor->BasePage + Descriptor->PageCount;
        if ((Descriptor->MemoryType != MEMORY_TYPE_BAD) &&
            (CurrentPage <= Descriptor->BasePage) &&
            (ListEndPage <= EndPage)) {

            MmMdRemoveDescriptorFromList(Mdl, Descriptor);
            InsertTailList(&FreeDescriptorList, &Descriptor->ListEntry);
        }

        CurrentPage = ListEndPage;
    }

    //
    // Create memory descriptors for the enclave pages, splitting any partially
    // overlapping descriptors.
    //

    ListTypes = MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED |
                MEMORYMAP_TYPE_RESERVED_ALLOCATED |
                MEMORYMAP_TYPE_BAD;

    CurrentPage = BasePage;
    ExtendLastDescriptor = FALSE;
    LastBasePage = 0;
    RtlZeroMemory(&ListDescriptor, sizeof(MEMORY_DESCRIPTOR));
    while (CurrentPage < EndPage) {
        Descriptor = MmMdFindOverlappingDescriptor(CurrentPage,
                                                   EndPage,
                                                   ListTypes,
                                                   &Mdl);

        if (Descriptor == NULL) {

            //
            // Pages up to the end of the enclave region are enclave pages.
            //

            ListEndPage = EndPage;
            CurrentEndPage = EndPage;

        } else {
            ListDescriptor = *Descriptor;
            ListEndPage = Descriptor->BasePage + Descriptor->PageCount;
            if (Descriptor->MemoryType == MEMORY_TYPE_BAD) {

                //
                // Pages (if any) up to the start of the bad range are enclave
                // pages.
                //

                CurrentEndPage = Descriptor->BasePage;

            } else {

                //
                // Pages up to the end of the existing descriptor are enclave
                // pages.
                //

                CurrentEndPage = min(ListEndPage, EndPage);
                MmMdRemoveDescriptorFromList(Mdl, Descriptor);

                //
                // If the existing descriptor starts before the enclave region,
                // create a descriptor to describe the first portion. If it is
                // also an enclave descriptor, extend it to describe the new
                // enclave pages if possible.
                //

                if (Descriptor->BasePage < CurrentPage) {
                    if ((Mdl == &MmMdlReservedAllocated) &&
                        (Descriptor->MemoryType == EnclaveMemoryType) &&
                        (Descriptor->Attributes == EnclaveAttributes) &&
                        (Descriptor->MappedBasePage == 0)) {

                        LastBasePage = Descriptor->BasePage;
                        ExtendLastDescriptor = TRUE;
                        InsertTailList(&FreeDescriptorList,
                                       &Descriptor->ListEntry);

                    } else {
                        Descriptor->PageCount =
                            CurrentPage - Descriptor->BasePage;

                        MmMdAddDescriptorToList(Mdl,
                                                Descriptor,
                                                MDL_FLAGS_NONE);
                    }

                } else {
                    InsertTailList(&FreeDescriptorList,
                                   &Descriptor->ListEntry);
                }
            }
        }

        //
        // Create or extend a descriptor for the enclave pages up to the end of
        // the current enclave range.
        //

        if ((CurrentPage < CurrentEndPage) &&
            (ExtendLastDescriptor == FALSE)) {

            LastBasePage = CurrentPage;
            ExtendLastDescriptor = TRUE;
        }

        //
        // If the existing descriptor ends after the enclave region, create or
        // extend a descriptor to describe the last portion.
        //

        if ((EndPage < ListEndPage) &&
            (ListDescriptor.MemoryType != MEMORY_TYPE_BAD)) {

            if ((Mdl == &MmMdlReservedAllocated) &&
                (ListDescriptor.MemoryType == EnclaveMemoryType) &&
                (ListDescriptor.Attributes == EnclaveAttributes) &&
                (ListDescriptor.MappedBasePage == 0)) {

                CurrentEndPage = ListEndPage;

            } else {

                NT_ASSERT(!IsListEmpty(&FreeDescriptorList));

                ListEntry = RemoveHeadList(&FreeDescriptorList);
                Descriptor = CONTAINING_RECORD(ListEntry,
                                               MEMORY_DESCRIPTOR,
                                               ListEntry);

                *Descriptor = ListDescriptor;
                if (Descriptor->MappedBasePage != 0) {
                    Descriptor->MappedBasePage +=
                        EndPage - Descriptor->BasePage;
                }

                Descriptor->PageCount -= EndPage - Descriptor->BasePage;
                Descriptor->BasePage = EndPage;
                MmMdAddDescriptorToList(Mdl, Descriptor, MDL_FLAGS_NONE);
            }
        }

        //
        // If the current enclave range is the end of the enclave region or is
        // followed by a bad memory range, add the enclave descriptor to the
        // list. Enclave descriptors go on the reserved list so they can be
        // propagated to other boot apps.
        //

        if (((CurrentEndPage >= EndPage) ||
             (ListDescriptor.MemoryType == MEMORY_TYPE_BAD)) &&
            (ExtendLastDescriptor != FALSE)) {

            NT_ASSERT(!IsListEmpty(&FreeDescriptorList));

            ListEntry = RemoveHeadList(&FreeDescriptorList);
            Descriptor = CONTAINING_RECORD(ListEntry,
                                           MEMORY_DESCRIPTOR,
                                           ListEntry);

            Descriptor->BasePage = LastBasePage;
            Descriptor->MappedBasePage = 0;
            Descriptor->PageCount = CurrentEndPage - LastBasePage;
            Descriptor->Attributes = EnclaveAttributes;
            Descriptor->MemoryType = EnclaveMemoryType;
            MmMdAddDescriptorToList(&MmMdlReservedAllocated,
                                    Descriptor,
                                    MDL_FLAGS_COALESCE);

            ExtendLastDescriptor = FALSE;
        }

        CurrentPage = ListEndPage;
    }

    NT_ASSERT(ExtendLastDescriptor == FALSE);

AddEnclavePageRangeEnd:

    //
    // Free memory descriptors that were removed and not reused, or if not all
    // allocations succeeded.
    //

    while (!IsListEmpty(&FreeDescriptorList)) {
        ListEntry = RemoveHeadList(&FreeDescriptorList);
        Descriptor = CONTAINING_RECORD(ListEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        MmMdFreeDescriptor(Descriptor);
        NumberOfDescriptors += 1;
    }

    NT_ASSERT(!NT_SUCCESS(Status) || (NumberOfDescriptors == 0));

    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
BlpMmInitializeConstraints (
    VOID
    )

/*++

Routine Description:

    This routine reads physical memory constraint requirements from the
    application options and saves them for subsequent use by the page allocator.

Arguments:

    None

Return Value:

    NT Status code.

--*/

{

    NTSTATUS GetOptionStatus;
    ULONGLONG MaximumPhysicalAddress;
    ULONGLONG MinimumPhysicalAddress;
    NTSTATUS Status;

    //
    // Check the option string associated with the initializing boot
    // application to see if any physical memory should be excluded from the
    // internal memory descriptor lists. Attempt each operation, even if one
    // fails.
    //

    Status = STATUS_SUCCESS;
    GetOptionStatus = BlGetApplicationOptionInteger(
                          BCDE_LIBRARY_TYPE_AVOID_LOW_PHYSICAL_MEMORY,
                          &MinimumPhysicalAddress);

    if (NT_SUCCESS(GetOptionStatus)) {

        //
        // Ensure that the minimum physical address is actually addressible
        // given the current processor mode. If it is not, silently ignore the
        // flag.
        //

        MinimumPhysicalAddress = ALIGN_RANGE_UP(MinimumPhysicalAddress,
                                                PAGE_SIZE);

        if ((MinimumPhysicalAddress >> PAGE_SHIFT) <
            MAXIMUM_PAGE_RANGE_LENGTH) {

            PapMinimumPhysicalPage = MinimumPhysicalAddress >> PAGE_SHIFT;
        }
    }

    GetOptionStatus = BlGetApplicationOptionInteger(
                          BCDE_LIBRARY_TYPE_TRUNCATE_PHYSICAL_MEMORY,
                          &MaximumPhysicalAddress);

    if (NT_SUCCESS(GetOptionStatus)) {
        Status = MmPaTruncateMemory(MaximumPhysicalAddress >> PAGE_SHIFT);
    }

    return Status;
}

ULONGLONG
BlpMmMarkBadMemory (
    __in_ecount(Count) ULONGLONG UNALIGNED *BadPageList,
    __in ULONGLONG Count
    )

/*++

Routine Description:

    This routine marks supplies contiguous pages as bad, removing them from the
    memory map, and adding them to the bad page list.

Arguments:

    PfnArray - Supplies a pointer to an array of page frame numbers.

    Count - Supplies the number of elements in the array.

Return Value:

    Number of contiguous pages consumed.

--*/

{

    MEMORY_DESCRIPTOR BadMemoryDescriptor;

    MmPapDeterminePfnRun(BadPageList,
                         Count,
                         &BadMemoryDescriptor);

    BadMemoryDescriptor.MemoryType =
        GET_MEMORY_TYPE(MEMORY_CLASS_PLATFORM,
                        MEMORY_PLATFORM_TYPE_BAD);

    BadMemoryDescriptor.Attributes = MEMORY_ATTRIBUTE_NONE;
    BadMemoryDescriptor.MappedBasePage = 0;
    MmPapRemoveBadMemory(&BadMemoryDescriptor);
    return BadMemoryDescriptor.PageCount;
}

// --------------------------------------------------------------------- MM API

NTSTATUS
MmPaInitialize (
    __in PALLOCATED_MEMORY AllocatedMemory,
    __in ULONG MinimumPageAllocation
    )

/*++

Routine Description:

    Initializes the required data structures for the Page Allocator.

Arguments:

    AllocatedMemory - Supplies a description of the memory currently allocated
        by previously executing boot applications.

    MinimumPageAllocation - Supplies the application-provided minimum number of
        pages that the page allocator's pool should be grown by at any one time.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if the page allocator was unable to allocate memory for
        itself.

    STATUS_INVALID_PARAMETER if there is an error creating the allocated memory
        descriptor list based on the list the caller provided.

    Other failure status codes are possible depending on firmware
        implementation of MmGetFirmwareMemoryMap.

--*/

{

    MEMORY_DESCRIPTOR UNALIGNED *MemoryDescriptor;
    ULONG MemoryMapFlags;
    UNPACKED_MEMORY_TYPE MemoryType;
    PMEMORY_DESCRIPTOR NewDescriptor;
    ULONG NumberOfDescriptors;
    NTSTATUS Status;

    //
    // Store the minimum page allocation size.
    //

    PapMinimumAllocationCount = MinimumPageAllocation;

    //
    // Initialize the maximum and minimum physical pages that the page allocator
    // should use. Avoid using the first physical page because applications that
    // run in physical mode or identity map pages could have trouble receiving
    // NULL memory.
    //

    PapMaximumPhysicalPage = (MAXULONGLONG >> PAGE_SHIFT);
    PapMinimumPhysicalPage = 1;

    //
    // Initialize memory descriptor lists.
    //

#if !defined(MM_MIN)

    MmMdInitializeList(&MmMdlMappedAllocated, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlMappedUnallocated, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlFwAllocationTracker, MdlTypePhysical, NULL);

#endif

    MmMdInitializeList(&MmMdlUnmappedAllocated, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlUnmappedUnallocated, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlReservedAllocated, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlBadMemory, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlTruncatedMemory, MdlTypePhysical, NULL);
    MmMdInitializeList(&MmMdlPersistentMemory, MdlTypePhysical, NULL);

    //
    // Obtain the system memory descriptor list from the firmware.
    //

    MemoryMapFlags = MM_MEMORYMAP_FLAGS_USE_FIRMWARE_ALLOCATOR;
    MemoryMapFlags |= MM_MEMORYMAP_FLAGS_COALESCE;
    Status = MmFwGetMemoryMap(&MmMdlUnmappedUnallocated, MemoryMapFlags);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The caller may have provided a list of memory regions that have been
    // allocated.  These need to be removed from the system memory list and
    // kept in a separate list, describing all reserved memory.
    //
    // N.B. Unlike other pieces of the application parameter structure,
    //      MEMORY_DESCRIPTOR does not carry an implicit UNALIGNED qualifier.
    //      Therefore an explicitly UNALIGNED memory descriptor pointer must
    //      be used here.
    //

    NumberOfDescriptors = AllocatedMemory->NumberOfDescriptors;
    MemoryDescriptor = Add2Ptr(AllocatedMemory, AllocatedMemory->MdlOffset);

    //
    // Adjust for difference in MEMORY_DESCRIPTOR definition.
    //

    MemoryDescriptor = Add2Ptr(
                           MemoryDescriptor,
                           (UINTN)AllocatedMemory->FixedDataOffset
                           - FIELD_OFFSET(MEMORY_DESCRIPTOR, FixedLengthData));

    while (NumberOfDescriptors != 0) {

        //
        // Remove the specified memory range from the system memory descriptor
        // list.
        //
        // N.B.  When removing memory regions from system memory descriptor
        //       list, we remove based on physical address ranges since the
        //       system memory descriptor list was created using physical
        //       address ranges.
        //

        Status = MmMdRemoveRegionFromMdl(&MmMdlUnmappedUnallocated,
                                         MemoryDescriptor->BasePage,
                                         MemoryDescriptor->PageCount,
                                         MDL_FLAGS_PHYSICAL_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            return STATUS_INVALID_PARAMETER;
        }

        //
        // Add the specified exclusion range to the reserved MDL.  Copy the
        // descriptor to a global descriptor and insert the copy into the
        // library's global reserved memory list.
        //
        // If the specified exclusion range has an application definable
        // memory type, convert this type to one that avoids confusion to the
        // meaning of the memory type.
        //

        MemoryType.MemoryType = MemoryDescriptor->MemoryType;
        if ((MemoryType.Class == MEMORY_CLASS_APPLICATION) &&
            (MemoryType.SubType >= MEMORY_APPLICATION_TYPE_DEFINABLE_START) &&
            (MemoryType.SubType <= MEMORY_APPLICATION_TYPE_DEFINABLE_END)) {

            MemoryType.SubType = MEMORY_APPLICATION_TYPE_UNAVAILABLE;
        }

        NewDescriptor = MmMdInitDescriptor(MemoryDescriptor->BasePage,
                                           MemoryDescriptor->MappedBasePage,
                                           MemoryDescriptor->PageCount,
                                           MemoryDescriptor->Attributes,
                                           MemoryDescriptor->MemoryType);

        if (NewDescriptor == NULL) {
            return STATUS_NO_MEMORY;
        }

        Status = MmMdAddDescriptorToList(&MmMdlReservedAllocated,
                                         NewDescriptor,
                                         MDL_FLAGS_NONE);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Update pointer and loop conditional.
        //

        NumberOfDescriptors -= 1;
        MemoryDescriptor = Add2Ptr(MemoryDescriptor,
                                   AllocatedMemory->SizeOfDescriptor);
    }

    //
    // Initialize the minimum and maximum physical memory pages that should be
    // used.
    //

    Status = BlpMmInitializeConstraints();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Do not initially reserve and map memory for page allocations.  Give
    // each firmware's MM initialization code the initial choice of which
    // memory to reserve/map.
    //

    //
    // After any call to a routine that uses global memory descriptors
    // (MmSetStaticMemoryDescriptor), MmFreeStaticMemoryDescriptors needs
    // to be called to allocate heap for the global descriptor.  This
    // can not be done by this routine since the heap allocator has yet to
    // be initialized.  BlMemoryInitialize will free these descriptors.
    //

    PapInitializationStatus = ModuleInitialized;
    return STATUS_SUCCESS;
}

NTSTATUS
MmPaDestroy (
    __in ULONG Phase
    )

/*++

Routine Description:

    Destruction routine for the Page Allocator. Free all memory allocated from
    the firmware via the Page Allocator.

    Phase 0 executes in the normal application addressing mode. It migrates all
    of the memory descriptor lists to the global descriptors, then frees all
    mapped allocations.

    Phase 1 executes with address translation disabled. It frees all unmapped
    allocations.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if page allocator runs out of global descriptors.

--*/

{

    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

#if !defined(MM_MIN)

    ULONGLONG Size;
    PVOID VirtualAddress;

#endif


    PapInitializationStatus = ModuleInDestruction;
    if (Phase == 0) {

        //
        // Attempt to move as many descriptors to the global descriptor array
        // as possible.  All memory allocated via the page allocator must be
        // freed.  This includes memory allocated by the heap allocator.  Each
        // page allocator MDL uses heap memory for the individual descriptors.
        // Therefore, in order to properly free the global MDLs, their memory
        // must be freed.
        //

#if !defined(MM_MIN)

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlMappedAllocated);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlMappedUnallocated);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlFwAllocationTracker);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

#endif

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlUnmappedAllocated);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlUnmappedUnallocated);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlReservedAllocated);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlBadMemory);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlTruncatedMemory);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlPersistentMemory);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

#if !defined(MM_MIN)

        if (BlMmQueryTranslationType() != VA_TRANSLATION_TYPE_NONE) {
            Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlMappingTrackers);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            Status = MmMdMigrateMdlToGlobalDescriptors(&MmMdlFreeVirtual);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }
        }

#endif

        //
        // Unmap all mapped persistent data allocations. The rest of the
        // persistent data clean-up of the persistent memory list is handled by
        // the PD API destruction routine.
        //

#if !defined(MM_MIN)

        NextEntry = MmMdlPersistentMemory.Head->Flink;
        while (NextEntry != MmMdlPersistentMemory.Head) {
            MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            VirtualAddress = PAGE_TO_ADDRESS(MemoryDescriptor->MappedBasePage);
            if (VirtualAddress != NULL) {
                Size = MemoryDescriptor->PageCount << PAGE_SHIFT;
                Status = BlMmUnmapVirtualAddress(VirtualAddress, Size);
                if (!NT_SUCCESS(Status)) {
                    return Status;
                }
            }

            NextEntry = NextEntry->Flink;
        }

        //
        // Free all mapped allocations. Some of these regions will be returned
        // to the unmapped/unallocated list. Others will be coalesced on the
        // mapped/unallocated list. It depends on which APIs were used to
        // allocate and map them.
        //

        NextEntry = MmMdlMappedAllocated.Head->Flink;
        while (NextEntry != MmMdlMappedAllocated.Head) {
            MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            VirtualAddress = PAGE_TO_ADDRESS(MemoryDescriptor->MappedBasePage);
            Status = BlMmFreePages(VirtualAddress);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            //
            // BlMmFreePages removed the last entry.  The next entry is once
            // again the head of the list.
            //

            NextEntry = MmMdlMappedAllocated.Head->Flink;
        }

        //
        // Unmap all allocations on the mapped/unallocated list. This will
        // unmap any firmware regions (boot/runtime services, ACPI tables) and
        // all of the page allocator extend regions.
        //

        NextEntry = MmMdlMappedUnallocated.Head->Flink;
        while (NextEntry != MmMdlMappedUnallocated.Head) {
            MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            Size = MemoryDescriptor->PageCount << PAGE_SHIFT;
            VirtualAddress = PAGE_TO_ADDRESS(MemoryDescriptor->MappedBasePage);
            Status = BlMmUnmapVirtualAddress(VirtualAddress, Size);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            //
            // The unmap operation removed the last entry.  The next entry is
            // once again the head of the list.
            //

            NextEntry = MmMdlMappedUnallocated.Head->Flink;
        }

        //
        // Free the firmware allocations from the page allocator extendor.
        //

        NextEntry = MmMdlFwAllocationTracker.Head->Flink;
        while (NextEntry != MmMdlFwAllocationTracker.Head) {
            MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            Status = MmFwFreePages(MemoryDescriptor->BasePage,
                                   MemoryDescriptor->PageCount);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            NextEntry = NextEntry->Flink;
        }

#endif

    } else {

        ASSERT(Phase == 1);

        //
        // Free all unmapped pages allocated from the firmware.  This list
        // includes the page tables and other structures necessary for address
        // translation.
        //

        NextEntry = MmMdlUnmappedAllocated.Head->Flink;
        while (NextEntry != MmMdlUnmappedAllocated.Head) {
            MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            PhysicalAddress.QuadPart = MemoryDescriptor->BasePage << PAGE_SHIFT;
            Status = BlMmFreePhysicalPages(PhysicalAddress);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            //
            // BlMmFreePhysicalPages removed the last entry.  The next entry
            // is once again the head of the list.
            //

            NextEntry = MmMdlUnmappedAllocated.Head->Flink;
        }
    }

    return STATUS_SUCCESS;
}

ULONGLONG
MmPaLookupMaximumPhysicalPage (
    VOID
    )

/*++

Routine Description:

    This routine returns the maximum physical page that should be used by the
    page allocator.

Arguments:

    None.

Return Value:

    The maximum physical page that should be used by the page allocator.

--*/

{

    return PapMaximumPhysicalPage;
}

ULONGLONG
MmPaLookupMinimumPhysicalPage (
    VOID
    )

/*++

Routine Description:

    This routine returns the minimum physical page that should be used by the
    page allocator.

Arguments:

    None.

Return Value:

    The minimum physical page that should be used by the page allocator.

--*/

{

    return PapMinimumPhysicalPage;
}

MODULE_INITIALIZATION_STATUS
MmPaModuleInitializationStatus (
    VOID
    )

/*++

Routine Description:

    Returns the page allocator's module initialization status.

Arguments:

    None.

Return Value:

    The module initialization status.

--*/

{

    return PapInitializationStatus;
}

NTSTATUS
MmPaTruncateMemory (
    __in ULONGLONG PhysicalPage
    )

/*++

Routine Description:

    Removes all references in the Page Allocator to memory above (or equal) the
    specified physical address.

Arguments:

    PhysicalPage - Specifies the maximum PFN allowed in the Page Allocator.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    NTSTATUS Status;

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Keep track of the maximum physical page the page allocator should be
    // using, which is the smallest value this function has ever been called
    // with.  This allows the memory synchronization code to determine when
    // certain regions need to return to the truncated list.
    //

    if (PhysicalPage < PapMaximumPhysicalPage) {
        PapMaximumPhysicalPage = PhysicalPage;
    }

#if !defined(MM_MIN)

    Status = MmMdTruncateDescriptors(&MmMdlMappedAllocated,
                                     &MmMdlTruncatedMemory,
                                     PhysicalPage);

    if (!NT_SUCCESS(Status)) {
        goto TruncateMemoryEnd;
    }

#endif

    Status = MmMdTruncateDescriptors(&MmMdlUnmappedAllocated,
                                     &MmMdlTruncatedMemory,
                                     PhysicalPage);

    if (!NT_SUCCESS(Status)) {
        goto TruncateMemoryEnd;
    }

#if !defined(MM_MIN)

    Status = MmMdTruncateDescriptors(&MmMdlMappedUnallocated,
                                     &MmMdlTruncatedMemory,
                                     PhysicalPage);

    if (!NT_SUCCESS(Status)) {
        goto TruncateMemoryEnd;
    }

#endif

    Status = MmMdTruncateDescriptors(&MmMdlUnmappedUnallocated,
                                     &MmMdlTruncatedMemory,
                                     PhysicalPage);

    if (!NT_SUCCESS(Status)) {
        goto TruncateMemoryEnd;
    }

    Status = MmMdTruncateDescriptors(&MmMdlReservedAllocated,
                                     &MmMdlTruncatedMemory,
                                     PhysicalPage);

    if (!NT_SUCCESS(Status)) {
        goto TruncateMemoryEnd;
    }

TruncateMemoryEnd:
    MmLeaveDescriptorLibraryApi();
    return Status;
}

VOID
MmPaPrint (
    VOID
    )

/*++

Routine Description:

    Prints out the internal structures of the Page Allocator.

Arguments:

    None.

Return Value:

    None

--*/

{

#if !defined(MM_MIN)

    ConsolePrint(L"Mapped / Allocated MDL:\r\n");
    MmMdPrintList(&MmMdlMappedAllocated);
    ConsolePrint(L"Mapped / Unallocated MDL:\r\n");
    MmMdPrintList(&MmMdlMappedUnallocated);

#endif

    ConsolePrint(L"Unmapped / Allocated MDL:\r\n");
    MmMdPrintList(&MmMdlUnmappedAllocated);
    ConsolePrint(L"Unmapped / Unallocated MDL:\r\n");
    MmMdPrintList(&MmMdlUnmappedUnallocated);
    return;
}

NTSTATUS
MmPaValidate (
    VOID
    )

/*++

Routine Description:

    Attempts to validate the internal structure of the Page Allocator.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_UNSUCCESSFUL if there are any overlapping descriptors in any global
        memory list.

--*/

{

    NTSTATUS Status;

    if (PapInitializationStatus != ModuleInitialized) {
        return STATUS_SUCCESS;
    }

    //
    // Check each memory descriptor list to make sure that no consecutive
    // descriptors overlap one another.
    //

#if !defined(MM_MIN)

    Status = MmMdValidateList(&MmMdlMappedAllocated);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = MmMdValidateList(&MmMdlMappedUnallocated);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

#endif

    Status = MmMdValidateList(&MmMdlUnmappedAllocated);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = MmMdValidateList(&MmMdlUnmappedUnallocated);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    return STATUS_SUCCESS;
}

#if !defined(MM_MIN)

NTSTATUS
MmPaReleaseSelfMapPages (
    __in PHYSICAL_ADDRESS Address
    )

/*++

Routine Description:

    This routine releases a self-map reservation region allocated with
    MmPaReserveSelfMapPages.

Arguments:

    Address - Supplies a physical address marking the base of the self-map
        reserved region of physical memory.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONGLONG BasePage;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    NTSTATUS Status;
    PMEMORY_DESCRIPTOR VirtualDescriptor;

    //
    // Ensure that the caller's address is at least page aligned. All self map
    // reservations should be page aligned.
    //

    if (((ULONG_PTR)Address.QuadPart & (PAGE_SIZE - 1)) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // As the self map reserved pages remain on the unmapped/unallocated memory
    // descriptor list, walk this list in attempt to find a descriptor that
    // starts at the specified address.
    //

    BasePage = Address.QuadPart >> PAGE_SHIFT;
    MemoryDescriptor = MmMdFindDescriptor(BasePage,
                                          MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED,
                                          MDL_FLAGS_PHYSICAL_ADDRESS);

    if (MemoryDescriptor == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Ensure the desired page to free defines the start of the region returned.
    //

    if (MemoryDescriptor->BasePage != BasePage) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Free the physical pages back to the firmware.
    //
    // N.B. There is no need to zero the reserved self map pages. They were not
    //      used by the application and do not contain sensitive data.
    //

    Status = MmFwFreePages(BasePage, MemoryDescriptor->PageCount);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // If the pages were returned to the firmware, then clear the firmware
    // allocated attribute.
    //

    CLEAR_FLAGS(MemoryDescriptor->Attributes,
                MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_ALLOCATED);

    CLEAR_FLAGS(MemoryDescriptor->Attributes,
                FIRMWARE_SERVICE_MEMORY_ATTRIBUTES);

    //
    // Mark the region as free in the unmapped unallocated list.
    //

    MemoryDescriptor->MemoryType = MEMORY_TYPE_FREE;

    //
    // Mark the region as free in the virtual free list.
    //

    VirtualDescriptor = MmMdInitDescriptor(BasePage,
                                           0,
                                           MemoryDescriptor->PageCount,
                                           MEMORY_ATTRIBUTE_NONE,
                                           MEMORY_TYPE_FREE);

    if (VirtualDescriptor == NULL) {
        return STATUS_NO_MEMORY;
    }

    Status = MmMdAddDescriptorToList(&MmMdlFreeVirtual,
                                     VirtualDescriptor,
                                     MDL_FLAGS_COALESCE | MDL_FLAGS_TRUNCATE);

    return Status;
}

NTSTATUS
MmPaReserveSelfMapPages (
    __out PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in ULONG Alignment
    )

/*++

Routine Description:

    This routine reserves physical memory for the self-map region. The self-map
    occupies a virtual address region, but the physical pages corresponding to
    the 1:1 virtual address to physical address mappings must be consumed. This
    is because firmware is mapped 1:1. If firmware allocates physical memory
    within the self-map region, it could never be mapped 1:1. Until this 1:1
    mapping is no longer required, the self-map region must be physically
    backed.

Arguments:

    Address - Supplies a pointer to a physical address to receive the base
        address of the range reserved for the self-map.

    Pages - Supplies the number of pages requested for the self-map reservation
        range.

    Alignment - Supplies the alignment requirements for the self-map region.

Return Value:

    NTSTATUS Code.

--*/

{

    MEMORY_DESCRIPTOR Allocation;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    ULONGLONG RangeSize;
    NTSTATUS Status;

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Parameter checking.
    //

    if ((Address == NULL) || (Pages == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto MmPaReserveSelfMapPagesEnd;
    }

    //
    // When no alignment request is made, page align the allocation.
    //

    if (Alignment == 0) {
        Alignment = 1;
    }

    //
    // The mapped range is not used for the physical allocation of the self-map
    // reserved pages.
    //

    INITIALIZE_UNUSED_RANGE(&AllocationRequest.MappedRange);

    //
    // Search the entire physical memory range for the pages.
    //

    RangeSize = MAXIMUM_PAGE_RANGE_LENGTH - PapMinimumPhysicalPage;
    INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                             PapMinimumPhysicalPage,
                             RangeSize);

    //
    // Reserve the self-map pages from the unmapped/unallocated list, restoring
    // the allocation to the unmapped unallocated list.
    //

    AllocationRequest.Alignment = Alignment;
    AllocationRequest.Attributes = MEMORY_ATTRIBUTE_NONE;
    AllocationRequest.PageCount = Pages;
    AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;
    Status = MmPaAllocatePages(&MmMdlUnmappedUnallocated,
                               &Allocation,
                               &MmMdlUnmappedUnallocated,
                               &AllocationRequest,
                               MEMORY_TYPE_SELF_MAP_RESERVED);

    if (!NT_SUCCESS(Status)) {
        goto MmPaReserveSelfMapPagesEnd;
    }

    //
    // Reserve the virtual address space used by the self-map region.
    //

    Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                     Allocation.BasePage,
                                     Allocation.PageCount,
                                     MDL_FLAGS_VIRTUAL_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        goto MmPaReserveSelfMapPagesEnd;
    }

    //
    // Return the address of the allocation.
    //

    Address->QuadPart = Allocation.BasePage << PAGE_SHIFT;

MmPaReserveSelfMapPagesEnd:
    MmLeaveDescriptorLibraryApi();
    return Status;
}

#endif

NTSTATUS
MmPaAllocatePages (
    __inout_opt PMEMORY_DESCRIPTOR_LIST MdlDestination,
    __out PMEMORY_DESCRIPTOR AllocatedDescriptor,
    __inout PMEMORY_DESCRIPTOR_LIST MdlSource,
    __in PPAGE_ALLOCATION_REQUEST AllocationRequest,
    __in MEMORY_TYPE DestinationType
    )

/*++

Routine Description:

    Allocates a memory region from the unallocated (source) memory descriptor
    list, and places it in the allocated (destination) memory descriptor list.

    If the request has any special allocation attributes, and the allocation
    fails as a result, this routine will allocate enough special memory to
    succeed the request.

Arguments:

    MdlDestination - Memory descriptor list containing allocated memory.  If
        a memory region can be successfully allocated, it will be removed
        from the source (unallocated) list and moved to the destination
        (allocated) list.

    AllocatedDescriptor - Supplies a pointer to a variable that receives the
        result of the page allocation, if it is successful.

    MdlSource - Memory descriptor list containing unallocated memory.  Any
        successful allocation will be made from this list.

    AllocationRequest - Supplies the parameters of the page allocation request.
        The request includes a physical and virtual address rnage, a size (in
        pages), alignment, and required memory attributes.

    DestinationType - Supplies the memory type to use for the allocated page
        set.

    InsertionFlags - Flags to use when inserting a memory descriptor
        into a list (whether it be the allocated memory descriptor into
        the allocated list or a leftover piece of a region carved out of the
        unallocated list for the allocation).

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL if there are any overlapping descriptors in any global
        memory list.

--*/

{

    NTSTATUS Status;

#if !defined(MM_MIN)

    ULONG Attribute;
    PLIST_ENTRY Entry;
    MEMORY_DESCRIPTOR LargePageAllocation;
    PAGE_ALLOCATION_REQUEST LargePageRequest;
    ULONG LargePageSize;
    MEMORY_DESCRIPTOR_LIST LocalDescriptorList;
    PMEMORY_DESCRIPTOR MemoryDescriptor;

    MemoryDescriptor = NULL;

#endif

    //
    // Apply additional attributes based on type.
    //
    // N.B. Only do this automatically for library memory types.
    //

    //
    // Identify memory allocations that cannot be freed until the last stage
    // of library destruction.
    //

    switch (DestinationType) {
        case MEMORY_TYPE_HEAP:
        case MEMORY_TYPE_PAGE_TABLE:
        case MEMORY_TYPE_PAGE_TABLE_DATA:
            AllocationRequest->Attributes |=
                MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_FREE_LAST;

            break;

        default:
            break;
    }

    //
    // Search the source memory descriptor list for a region that will
    // satisfy the caller's request.
    //

    Status = MmPapAllocateRegionFromMdlWithFallback(MdlDestination,
                                                    AllocatedDescriptor,
                                                    MdlSource,
                                                    AllocationRequest,
                                                    DestinationType);

    //
    // If we could not allocate the desired region from a previously
    // allocated region, determine if it was the result of an allocation
    // attribute.
    //

#if !defined(MM_MIN)

    if (!NT_SUCCESS(Status) &&
        (AllocationRequest->Attributes & MEMORY_ATTRIBUTE_ALLOCATION_MASK)) {

        //
        // Fork according to the allocation attribute.
        //

        if (CHECK_FLAG(AllocationRequest->Attributes,
                       MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE)) {

            ASSERT(BlMmQueryTranslationType() != VA_TRANSLATION_TYPE_NONE);

            //
            // Allocate a large page (or large pages) for the requested
            // allocation.  Do not put the resultant allocation into
            // the destination memory descriptor list, because it does not
            // have the large page allocation flag set.
            //
            // In the large page allocation :
            //    - Use the preferred base page of the caller.
            //    - Allocate the memory region as free memory.  It will be
            //      added to the source (unallocated) memory descriptor list
            //      for future allocations.
            //

            RtlCopyMemory(&LargePageRequest,
                          AllocationRequest,
                          sizeof(PAGE_ALLOCATION_REQUEST));

            CLEAR_FLAGS(LargePageRequest.Attributes,
                        MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE);

            //
            // Allocate at least a large page of memory.
            //

            LargePageSize = MmArchQueryLargePageSize();
            LargePageRequest.PageCount =
                ROUND_UP(AllocationRequest->PageCount, LargePageSize);

            LargePageRequest.Alignment = ROUND_UP(AllocationRequest->Alignment,
                                                  LargePageSize);

            MmMdInitializeList(&LocalDescriptorList, MdlSource->Type, NULL);

            //
            // Make the large page allocation.
            //

            Status = MmPapAllocateRegionFromMdlWithFallback(&LocalDescriptorList,
                                                            &LargePageAllocation,
                                                            MdlSource,
                                                            &LargePageRequest,
                                                            MEMORY_TYPE_FREE);

            if (!NT_SUCCESS(Status)) {
                goto PaAllocatePagesEnd;
            }

            ASSERT(IsListEmpty(LocalDescriptorList.Head) == FALSE);

            Entry = LocalDescriptorList.Head->Flink;
            MemoryDescriptor = CONTAINING_RECORD(Entry,
                                                 MEMORY_DESCRIPTOR,
                                                 ListEntry);

            //
            // Mark the allocated region with large page attribute and add it
            // back to the source MDL.
            //

            SET_FLAGS(MemoryDescriptor->Attributes,
                      MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE);

            Status = MmMdAddDescriptorToList(MdlSource,
                                             MemoryDescriptor,
                                             MDL_FLAGS_NONE);

            if (!NT_SUCCESS(Status)) {
                goto PaAllocatePagesEnd;
            }

            MemoryDescriptor = NULL;

            //
            // Attempt to allocate from the allocation just made.  This should
            // not fail since the memory region just allocated was at least
            // as large as the caller's allocation request.
            //

            AllocationRequest->MemoryType = MEMORY_TYPE_FREE;
            Status = MmPapAllocateRegionFromMdl(MdlDestination,
                                                AllocatedDescriptor,
                                                MdlSource,
                                                AllocationRequest,
                                                DestinationType);

            ASSERT(Status == STATUS_SUCCESS);

        } else if (CHECK_FLAG(AllocationRequest->Attributes,
                              MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {

            //
            // Look on the same Mdl to see if the fixed address is marked as a
            // self map reserved page. These can be used for fixed address
            // allocations. They have already been allocated from the firmware,
            // so this is a library allocation only.
            //

            AllocationRequest->MemoryType = MEMORY_TYPE_SELF_MAP_RESERVED;
            AllocationRequest->Attributes |=
                MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_ALLOCATION;

            Status = MmPapAllocateRegionFromMdl(MdlDestination,
                                                AllocatedDescriptor,
                                                MdlSource,
                                                AllocationRequest,
                                                DestinationType);
        }
    }

    //
    // On failure, if the descriptor is not empty, it implies that the
    // descriptor could not be added back to the source MDL after allocation
    // and thus become orphan (and not tracked anywhere). Free the range
    // described by it back to the firmware if applicable.
    //

PaAllocatePagesEnd:
    if (!NT_SUCCESS(Status) && (MemoryDescriptor != NULL)) {
        Attribute = MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_ALLOCATED;
        if (CHECK_FLAG(MemoryDescriptor->Attributes, Attribute) != FALSE) {
            MmFwFreePages(MemoryDescriptor->BasePage,
                          MemoryDescriptor->PageCount);
        }
    }

#endif

    return Status;
}


// --------------------------------------------------------- Internal Functions

NTSTATUS
MmPapAllocateRegionFromMdl (
    __inout_opt PMEMORY_DESCRIPTOR_LIST Destination,
    __out_opt PMEMORY_DESCRIPTOR Allocation,
    __inout PMEMORY_DESCRIPTOR_LIST Source,
    __in PPAGE_ALLOCATION_REQUEST AllocationRequest,
    __in MEMORY_TYPE MemoryType
    )

/*++

Routine Description:

    Makes a page allocation from the specified descriptor list.

Arguments:

    Destination - Supplies an optional pointer to a memory descriptor list where
        the allocated region is to be inserted.

        N.B. If the region was allocated from the firmware, the allocated
             descriptor would be marked as "firmware allocated" by this routine
             prior to inserting it in the destination. In cases where this
             parameter is NULL, the caller is responsible for setting the flag.

    Allocation - Supplies an optional pointer to a variable that receives the
        details of the page allocation.

    Source - Supplies the descriptor list from which the allocation is to be
        made.

    AllocationRequest - Supplies a pointer to the page allocation request
        parameters that must be satisfied by a region in the source descriptor
        list.

    MemoryType - Supplies the memory type to be used for the allocated region.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if a required parameter is not provided, or if
        neither a destination MDL nor an allocation descriptor is provided.

    STATUS_NO_MEMORY if a temporary descriptor can not be made when carving up
        a memory range.

--*/

{

    ULONGLONG AllocatedEnd;
    MEMORY_DESCRIPTOR AllocationRegion;
    ULONG Attribute;
    BOOLEAN FirmwareAllocated;
    BOOLEAN MarkAsFirmwareAllocated;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    BOOLEAN PreferHighAddresses;
    ULONGLONG RegionEnd;
    NTSTATUS Status;
    ULONGLONG TempBasePage;
    PMEMORY_DESCRIPTOR TempDescriptor;

    //
    // Parameter checking.
    //

    if ((Source == NULL) || (AllocationRequest == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    if ((Destination == NULL) && (Allocation == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Initialize local variables.
    //

    Status = STATUS_NO_MEMORY;
    RtlZeroMemory(&AllocationRegion, sizeof(MEMORY_DESCRIPTOR));

    //
    // Look for an adequate memory region in the memory descriptor list.  The
    // memory regions are ordered from lowest addresses to highest addresses,
    // so search forward if the request is for low addresses and backwards if
    // the request is for high addresses.
    //

    if (CHECK_FLAG(AllocationRequest->RangeFlags,
                   MEMORY_RANGE_PREFER_HIGH_ADDRESSES)) {

        PreferHighAddresses = TRUE;
        NextEntry = Source->Head->Blink;

    } else {
        PreferHighAddresses = FALSE;
        NextEntry = Source->Head->Flink;
    }

    MarkAsFirmwareAllocated = FALSE;
    while (NextEntry != Source->Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Check to see if any part of the current descriptor satisfies all of
        // the caller's requests.
        //

        if (MmMdFindSatisfyingRegion(MemoryDescriptor,
                                     &AllocationRegion,
                                     AllocationRequest->PageCount,
                                     &AllocationRequest->BaseRange,
                                     &AllocationRequest->MappedRange,
                                     PreferHighAddresses,
                                     AllocationRequest->MemoryType,
                                     AllocationRequest->Attributes,
                                     AllocationRequest->Alignment) != FALSE) {

            //
            // Normalize the satisfying region's memory attributes.  Memory
            // attributes contain four partitions, two describing support and
            // two for describing traits of the memory regions, such as
            // allocation or coalescing policy.  Only the support attributes
            // are inherited from the satisfying region.
            //

            AllocationRegion.Attributes =
                (AllocationRegion.Attributes & MEMORY_ATTRIBUTE_SUPPORT_MASK) |
                (AllocationRequest->Attributes & MEMORY_ATTRIBUTE_NON_SUPPORT_MASK);

            //
            // If the source descriptor list was the unmapped / unallocated
            // MDL, the memory has to be additionally allocated using firmware
            // services unless it has already been allocated from firmware
            // earlier.
            //

            Attribute = MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_ALLOCATION;
            FirmwareAllocated = FALSE;
            if (CHECK_FLAG(AllocationRequest->Attributes, Attribute) != FALSE) {
                FirmwareAllocated = TRUE;
            }

            Attribute = MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_ALLOCATED;
            if (CHECK_FLAG(MemoryDescriptor->Attributes, Attribute) != FALSE) {
                FirmwareAllocated = TRUE;
            }

            if ((Source == &MmMdlUnmappedUnallocated) &&
                (FirmwareAllocated == FALSE)) {

                Status = MmFwAllocatePages(AllocationRegion.BasePage,
                                           AllocationRegion.PageCount);

                if (!NT_SUCCESS(Status)) {
                    return Status;
                }

                MarkAsFirmwareAllocated = TRUE;
            }

            //
            // Remove MemoryDescriptor from the free list since we successfully
            // found a region that satisfies the caller's requirements.
            // MemoryDescriptor will be modified to look like
            // AllocationRegion and added to the allocated list.
            //

            MmMdRemoveDescriptorFromList(Source, MemoryDescriptor);
            AllocatedEnd =
                AllocationRegion.BasePage + AllocationRegion.PageCount;

            RegionEnd =
                MemoryDescriptor->BasePage + MemoryDescriptor->PageCount;

            //
            // Whenever a mapped region is split, set the suballocation
            // attribute on each piece to indicate that it should not be freed
            // all the way back to the firmware when released from the
            // mapped/allocated list or persistent list.
            //
            // Otherwise, in the case where the allocation region size is a
            // match, preserve the suballocation attribute for the case where
            // a fragment of a previously split allocation is subsequently
            // freed, and is currently being re-allocated.
            //
            // N.B. Absent additional tracking information, once a region is
            //      split, there is no intelligent way to reassemble it after
            //      all its constituent parts are freed. Hence the
            //      suballocation attribute must be preserved in this case,
            //      otherwise a reallocated fragment could potentially get
            //      freed back to the firmware. This is a necessary exception
            //      to the rule that descriptive attributes are not inherited
            //      from satisfying regions.
            //

#if !defined(MM_MIN)

            if (Source == &MmMdlMappedUnallocated) {
                if ((AllocationRegion.BasePage != MemoryDescriptor->BasePage) ||
                    (AllocatedEnd != RegionEnd)) {

                    SET_FLAGS(
                        MemoryDescriptor->Attributes,
                        MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_SUBALLOCATED);

                    if (ARGUMENT_PRESENT(Destination)) {
                        SET_FLAGS(
                            AllocationRegion.Attributes,
                            MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_SUBALLOCATED);
                    }

                } else if ((CHECK_FLAG(MemoryDescriptor->Attributes,
                                       MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_SUBALLOCATED) != FALSE) && (ARGUMENT_PRESENT(Destination))) {

                    SET_FLAGS(
                        AllocationRegion.Attributes,
                        MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_SUBALLOCATED);
                }
            }

#endif

            //
            // If needed, use a global descriptor to describe the memory
            // region before or after the allocated region, but still
            // described by the original descriptor, MemoryDescriptor.  If
            // either of these regions exists, they must be added back to the
            // original (source) descriptor list.
            //

            if (AllocationRegion.BasePage != MemoryDescriptor->BasePage) {
                __analysis_assume(AllocationRegion.BasePage >
                                  MemoryDescriptor->BasePage);

                TempDescriptor = MmMdInitDescriptor(
                                     MemoryDescriptor->BasePage,
                                     MemoryDescriptor->MappedBasePage,
                                     (AllocationRegion.BasePage -
                                      MemoryDescriptor->BasePage),
                                     MemoryDescriptor->Attributes,
                                     MemoryDescriptor->MemoryType);

                ASSERT(TempDescriptor != NULL);

                //
                // Add back any region described at the start of the original
                // descriptor, but not described by the allocated region.
                //
                // N.B. We do not have to worry about coalescing the slush
                // descriptor with its adjacent descriptors.  On one side,
                // there is the newly allocated region.  On the other end,
                // the descriptor has the same adjacent descriptor as before.
                // If these descriptors could have been coalesced, they
                // already would be.
                //

                Status = MmMdAddDescriptorToList(Source,
                                                 TempDescriptor,
                                                 MDL_FLAGS_NONE);

                if (!NT_SUCCESS(Status)) {
                    return Status;
                }
            }

            //
            // If needed, use a global descriptor to describe the memory region
            // after the allocated region, but still described by the
            // original descriptor, MemoryDescriptor.
            //

            if (AllocatedEnd != RegionEnd) {
                __analysis_assume(AllocatedEnd < RegionEnd);

                //
                // Only keep the mapped offset if the memory region is
                // virtually mapped.
                //

                if (AllocationRegion.MappedBasePage != 0) {
                    TempBasePage =
                        AllocationRegion.MappedBasePage +
                        AllocationRegion.PageCount;

                } else {
                    TempBasePage = 0;
                }

                TempDescriptor = MmMdInitDescriptor(
                                     AllocatedEnd,
                                     TempBasePage,
                                     RegionEnd - AllocatedEnd,
                                     MemoryDescriptor->Attributes,
                                     MemoryDescriptor->MemoryType);

                ASSERT(TempDescriptor != NULL);

                //
                // Add back any region described at the end of the original
                // descriptor, but not described by the allocated region.
                //
                // N.B. We do not have to worry about coalescing the slush
                // descriptor with its adjacent descriptors.  On one side,
                // there is the newly allocated region.  On the other end,
                // the descriptor has the same adjacent descriptor as before.
                // If these descriptors could have been coalesced, they
                // already would be.
                //

                Status = MmMdAddDescriptorToList(Source,
                                                 TempDescriptor,
                                                 MDL_FLAGS_NONE);

                if (!NT_SUCCESS(Status)) {
                    return Status;
                }
            }

            //
            // At this point, a memory region that satisfies the caller's
            // request has been found and removed from the source descriptor
            // list.  The final step that remains is the insertion of the
            // allocated region into the destination list.
            //

            Status = STATUS_SUCCESS;

            //
            // The original descriptor, MemoryDescriptor, will be used to
            // describe the allocated memory region.  Update its fields
            // and insert the updated descriptor into the destination
            // list.  If the destination list is undefined, free the
            // memory descriptor.
            //

            if (ARGUMENT_PRESENT(Destination)) {
                MemoryDescriptor->BasePage = AllocationRegion.BasePage;
                MemoryDescriptor->MappedBasePage =
                    AllocationRegion.MappedBasePage;

                MemoryDescriptor->PageCount = AllocationRegion.PageCount;
                MemoryDescriptor->MemoryType = MemoryType;
                MemoryDescriptor->Attributes = AllocationRegion.Attributes;
                if (MarkAsFirmwareAllocated != FALSE) {
                    SET_FLAGS(MemoryDescriptor->Attributes,
                              MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_ALLOCATED);
                }

                Status = MmMdAddDescriptorToList(Destination,
                                                 MemoryDescriptor,
                                                 MDL_FLAGS_NONE);

                if (!NT_SUCCESS(Status)) {
                    return Status;
                }

            } else {
                MmMdFreeDescriptor(MemoryDescriptor);
            }

            break;
        }

        if (PreferHighAddresses != FALSE) {
            NextEntry = NextEntry->Blink;

        } else {
            NextEntry = NextEntry->Flink;
        }
    }

    //
    // If successful, return the allocated region to the caller.
    //

    if (NT_SUCCESS(Status)) {
        RtlCopyMemory(Allocation, &AllocationRegion, sizeof(MEMORY_DESCRIPTOR));
    }

    return Status;
}

NTSTATUS
MmPapAllocateRegionFromMdlWithFallback (
    __inout_opt PMEMORY_DESCRIPTOR_LIST Destination,
    __out_opt PMEMORY_DESCRIPTOR Allocation,
    __inout PMEMORY_DESCRIPTOR_LIST Source,
    __inout PPAGE_ALLOCATION_REQUEST AllocationRequest,
    __in MEMORY_TYPE MemoryType
    )

/*++

Routine Description:

    Makes a page allocation from the specified descriptor list with a
    fallback that will handle specific types of allocation failures.

Arguments:

    Destination - Supplies an optional pointer to a memory descriptor list where
        the allocated region is to be inserted.

        N.B. If the region was allocated from the firmware, the allocated
             descriptor would be marked as "firmware allocated" by this routine
             prior to inserting it in the destination. In cases where this
             parameter is NULL, the caller is responsible for setting the flag.

    Allocation - Supplies an optional pointer to a variable that receives the
        details of the page allocation.

    Source - Supplies the descriptor list from which the allocation is to be
        made.

    AllocationRequest - Supplies a pointer to the page allocation request
        parameters that must be satisfied by a region in the source descriptor
        list.

    MemoryType - Supplies the memory type to be used for the allocated region.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if a required parameter is not provided, or if
        neither a destination MDL nor an allocation descriptor is provided.

    STATUS_NO_MEMORY if a temporary descriptor can not be made when carving up
        a memory range.

--*/

{

    ULONG Attempt;
    NTSTATUS Status;
    NTSTATUS SyncStatus;

    //
    // The allocation will spin in a loop until the result will not
    // indicate the memory map known by the boot MM and the firmware memory
    // map are in sync.
    //

    AllocationRequest->MemoryType = MEMORY_TYPE_FREE;

    //
    // Limit the loop to not spin infinitely in case
    // if firmware behaves in unexpected way.
    //

    for (Attempt = 0; Attempt < 10; Attempt += 1) {
        Status = MmPapAllocateRegionFromMdl(Destination,
                                            Allocation,
                                            Source,
                                            AllocationRequest,
                                            MemoryType);

        if (NT_SUCCESS(Status)) {
            break;
        }

        //
        // Recovery steps are only possible
        // for the memory in the firmware-owned list.
        //

        if (Source != &MmMdlUnmappedUnallocated) {
            break;
        }

        //
        // If the firmware returned EFI_NOT_FOUND, the Memory Map may be out
        // of sync so the memory region is not free anymore. Synchronize
        // memory map and try again.
        //

        if (Status == STATUS_NOT_FOUND) {
            SyncStatus = BlMmSynchronizeMemoryMap(MEMORY_SYNC_FLAGS_BOOT_SERVICE);
            if (!NT_SUCCESS(SyncStatus)) {
                break;
            }

            AllocationRequest->MemoryType = MEMORY_TYPE_FREE;
            Status = STATUS_NO_MEMORY;
            continue;
        }

        //
        // If ran out of regular memory, try to allocate zeroed range.
        // In case if zeroing is ensured by the hardware, this may cause
        // the guaranteed zeroing of the entire reguion to be reset.
        //

        if ((Status == STATUS_NO_MEMORY) &&
            (AllocationRequest->MemoryType == MEMORY_TYPE_FREE)) {

            AllocationRequest->MemoryType = MEMORY_TYPE_FREE_ZEROED;
            continue;
        }

        //
        // Some unrecoverable allcation failure.
        //

        break;
    }

    return Status;
}

NTSTATUS
MmPapFreePages (
    __in PVOID Address,
    __in ULONG ListType
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocatePages or
    BlMmAllocatePersistentPages.

Arguments:

    Address - Supplies the address of the allocation to free.

    ListTypes - Supplies the memory descriptor list to search for the pages.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address was not allocated by BlMmAllocatePages
    or BlMmAllocatePersistentPages.

--*/

{

    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

#if !defined(MM_MIN)

    ULONGLONG BasePage;
    ULONGLONG ByteSize;
    LIBRARY_PARAMETERS LibraryParameters;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    BOOLEAN PersistAllocation;
    PVOID VirtualAddress;

#endif

    BlpInterfaceEnter(InterfacePagalloc);

    //
    // If the boot application is not running with virtual address translation,
    // then call BlMmFreePhysicalPages.
    //

    Status = STATUS_SUCCESS;
    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        PhysicalAddress.QuadPart = (ULONG_PTR)Address;
        if (ListType != MEMORYMAP_TYPE_PERSISTENT) {
            ListType = MEMORYMAP_TYPE_UNMAPPED_ALLOCATED;
        }

        Status = MmPapFreePhysicalPages(PhysicalAddress, 0, ListType);
        goto FreePagesEnd;
    }

#if !defined(MM_MIN)

    //
    // Ensure that the caller's address is at least page aligned to protect
    // the library from an application calling BlMmFreePages with an
    // address allocated by the Heap Allocator.
    //
    // N.B. BlMmFreePages requires the caller to specify the base page of
    //      an allocation.  The base page of an allocated heap is reserved
    //      for the internal data structure, HEAP_BOUNDARY, describing the
    //      allocation.  It should be sufficient to mitigate against the
    //      remaining heap allocations from the base page.
    //

    if (((ULONG_PTR)Address & (PAGE_SIZE - 1)) != 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto FreePagesEnd;
    }

    //
    // This routine should only search for pages on the mapped / allocated list
    // or the persistent list.
    //

    ASSERT((ListType == MEMORYMAP_TYPE_MAPPED_ALLOCATED) ||
           (ListType == MEMORYMAP_TYPE_PERSISTENT));

    //
    // Search the specified memory descriptor list in attempt to find
    // a descriptor that starts at the specified address.
    //

    BasePage = (ULONG_PTR)Address >> PAGE_SHIFT;
    MemoryDescriptor = MmMdFindDescriptor(BasePage,
                                          ListType,
                                          MDL_FLAGS_VIRTUAL_ADDRESS);

    if ((MemoryDescriptor == NULL) ||
        (MemoryDescriptor->MappedBasePage != BasePage)) {

        Status = STATUS_INVALID_PARAMETER;
        goto FreePagesEnd;
    }

    //
    // When specified, zero the memory region.  If the application is exiting,
    // do not zero any persistent allocations.
    //

    PersistAllocation = FALSE;
    if ((PapInitializationStatus == ModuleInDestruction) &&
        (CHECK_FLAG(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE) != FALSE)) {

        PersistAllocation = TRUE;

    } else {
        CLEAR_FLAGS(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE);
    }

    LibraryParameters = BlGetCurrentLibraryParameters();
    if ((PersistAllocation == FALSE) &&
        (CHECK_FLAG(LibraryParameters.Flags, LF_ZERO_SENSITIVE_DATA) !=
         FALSE)) {

        ByteSize = MemoryDescriptor->PageCount << PAGE_SHIFT;

        ASSERT(ByteSize <= MAXIMUM_BYTE_RANGE_LENGTH);

        RtlSecureZeroMemory(Address, (UINTN)ByteSize);
    }

    //
    // If the memory allocation was never split, return the memory to the
    // system. Otherwise, if the descriptor begins with the specified address,
    // move the descriptor from the allocated list to the unallocated list.
    //
    // TODO: If the allocated list had descriptors coalesced, this routine will
    //       free the entire coalesced descriptor.   It can't do anything more
    //       intelligent since the number of pages to free is not a parameter
    //       to this routine.  Fix this limitation by providing a page count as
    //       an optional parameter.
    //

    if (ListType == MEMORYMAP_TYPE_PERSISTENT) {
        MmMdRemoveDescriptorFromList(&MmMdlPersistentMemory, MemoryDescriptor);

    } else {
        MmMdRemoveDescriptorFromList(&MmMdlMappedAllocated, MemoryDescriptor);
    }

    MemoryDescriptor->MemoryType = MEMORY_TYPE_FREE;
    if ((CHECK_FLAG(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_SUBALLOCATED) ==
         FALSE)) {

        ByteSize = MemoryDescriptor->PageCount << PAGE_SHIFT;
        VirtualAddress = PAGE_TO_ADDRESS(MemoryDescriptor->MappedBasePage);
        Status = BlMmUnmapVirtualAddress(VirtualAddress, ByteSize);
        if (!NT_SUCCESS(Status)) {
            goto FreePagesEnd;
        }

        //
        // Only release the pages back to the firmware if they are not meant to
        // persist.
        //

        if (PersistAllocation == FALSE) {
            MemoryDescriptor->MappedBasePage = 0;
            Status = MmFwFreePages(MemoryDescriptor->BasePage,
                                   MemoryDescriptor->PageCount);

            if (!NT_SUCCESS(Status)) {
                goto FreePagesEnd;
            }
        }

        //
        // This region could have been allocated as part of a page allocator
        // extension and then never subdivided. Since it has been free'd back
        // to the firmware or persisted, remove it from the firware allocation
        // list so library desctruction does not free it to the firmware a
        // second time. Failure to find the region is not a problem.
        //

        MmMdRemoveRegionFromMdl(&MmMdlFwAllocationTracker,
                                MemoryDescriptor->BasePage,
                                MemoryDescriptor->PageCount,
                                MDL_FLAGS_PHYSICAL_ADDRESS);

        MemoryDescriptor->Attributes &= MEMORY_ATTRIBUTE_CACHE_MASK |
                                        MEMORY_ATTRIBUTE_PROTECT_MASK;

        Status = MmMdAddDescriptorToList(&MmMdlUnmappedUnallocated,
                                         MemoryDescriptor,
                                         MDL_FLAGS_COALESCE);

        goto FreePagesEnd;
    }

    //
    // This is a fragment of a larger system allocation which was suballocated.
    // Coalesce it back on the mapped/unallocated list. It will get released to
    // the system by the firmware allocation tracking list during library
    // destruction.
    //

    Status = MmMdAddDescriptorToList(&MmMdlMappedUnallocated,
                                     MemoryDescriptor,
                                     MDL_FLAGS_COALESCE);

#endif

FreePagesEnd:
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
MmPapFreePhysicalPages (
    __in PHYSICAL_ADDRESS Address,
    __in_opt ULONGLONG PageCount,
    __in ULONG ListType
    )

/*++

Routine Description:

    This routine frees an allocation made by BlMmAllocatePhysicalPages. If a
    page count is specified and valid, the routine will attempt to remove only
    the region specified by the address and page count. This will only succeed
    if the region fully maps to a memory descriptor or if the region is a
    sub-region of a coalesced memory descriptor.

Arguments:

    Address - Supplies the address of the allocation to free.

    PageCount - Supplies the number of pages to free, or 0 if the argument is to
        be ignored.

    ListTypes - Supplies the memory descriptor list to search for the physical
        pages.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if address was not allocated by
        BlMmAllocatePhysicalPages or an invalid page count was supplied.

--*/

{

    ULONGLONG BasePage;
    ULONGLONG ByteSize;
    ULONG InsertionFlags;
    LIBRARY_PARAMETERS LibraryParameters;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    ULONGLONG PageOffset;
    ULONGLONG PagesToFree;
    BOOLEAN PageTableTypePage;
    BOOLEAN PersistAllocation;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // Ensure that the caller's address is at least page aligned to protect
    // the library from an application calling BlMmFreePhysicalPages with an
    // address allocated by the Heap Allocator.
    //
    // N.B. BlMmFreePhysicalPages requires the caller to specify the base page
    //      of an allocation.  The base page of an allocated heap is reserved
    //      for the internal data structure, HEAP_BOUNDARY, describing the
    //      allocation.  It should be sufficient to mitigate against the
    //      remaining heap allocations from the base page.
    //

    if (((ULONG_PTR)Address.QuadPart & (PAGE_SIZE - 1)) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Physical pages should only be on the unmapped / allocated list or the
    // persistent list.
    //

    ASSERT((ListType == MEMORYMAP_TYPE_UNMAPPED_ALLOCATED) ||
           (ListType == MEMORYMAP_TYPE_PERSISTENT));

    //
    // Walk the specified memory descriptor list in attempt to find a descriptor
    // that starts at the specified address.
    //

    BasePage = Address.QuadPart >> PAGE_SHIFT;
    MemoryDescriptor = MmMdFindDescriptor(BasePage,
                                          ListType,
                                          MDL_FLAGS_PHYSICAL_ADDRESS);

    if (MemoryDescriptor == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Entries in the persistent memory list could be mapped. Be sure that this
    // routine was not called on a mapped entry.
    //

    ASSERT(MemoryDescriptor->MappedBasePage == 0);

    //
    // If the memory descriptor does not have the COALESCE attribute, then the
    // supplied page count must either be 0 or it must equal the page count
    // stored in the memory descriptor.
    //

    if (CHECK_FLAG(MemoryDescriptor->Attributes,
                   MEMORY_ATTRIBUTE_DESCRIPTION_COALESCE) == FALSE) {

        if ((PageCount != 0) && (PageCount != MemoryDescriptor->PageCount)) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    //
    // Persist the allocation if the memory descriptor has the library only
    // free flag set and the library is being destructed.
    //

    PersistAllocation = FALSE;
    if ((PapInitializationStatus == ModuleInDestruction) &&
        (CHECK_FLAG(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE) != FALSE)) {

        PersistAllocation = TRUE;

    } else {
        CLEAR_FLAGS(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE);
    }

    PageTableTypePage = FALSE;
    if ((MemoryDescriptor->MemoryType == MEMORY_TYPE_PAGE_TABLE) ||
        (MemoryDescriptor->MemoryType == MEMORY_TYPE_PAGE_TABLE_DATA)) {

        PageTableTypePage = TRUE;
    }

    //
    // If a non-zero page count is supplied, then determine if the base page and
    // page count define a valid sub-region of the descriptor. Otherwise, ensure
    // the descriptor begins with the specified physical base page.
    //
    // A valid descriptor or a valid sub-region will be moved to the unallocated
    // list below.
    //

    if (PageCount != 0) {
        PageOffset = BasePage - MemoryDescriptor->BasePage;
        if ((PageOffset + PageCount) > MemoryDescriptor->PageCount) {
            return STATUS_INVALID_PARAMETER;
        }

        PagesToFree = PageCount;

    } else {
        if (MemoryDescriptor->BasePage != BasePage) {
            return STATUS_INVALID_PARAMETER;
        }

        PagesToFree = MemoryDescriptor->PageCount;
    }

    //
    // When specified, zero the memory region.  Do not perform this operation
    // if the page allocator is in destruction.  At that point, translation
    // services are not available since they may require page allocations for
    // new page tables. Page table pages are skipped as they do not contain
    // any sensitive data. Mapping page table pages virtually could result in
    // a self-mapping, invalidating the mapping when the page table is zeroed.
    //
    // To zero a physical allocation, the memory range needs to be mapped.  For
    // a large range, break up the allocation into smaller pieces to ensure the
    // range can be mapped by a virtually contiguous memory range.
    //
    // Since the memory is freed regardless, failure to zero-out the memory is
    // being ignored.
    //

    LibraryParameters = BlGetCurrentLibraryParameters();
    if ((PapInitializationStatus != ModuleInDestruction) &&
        (PersistAllocation == FALSE) &&
        (PageTableTypePage == FALSE) &&
        (CHECK_FLAG(LibraryParameters.Flags,
                    LF_ZERO_SENSITIVE_DATA) != FALSE)) {

        ByteSize = PagesToFree << PAGE_SHIFT;
        PhysicalAddress.QuadPart = PAGE_TO_PHYSICAL_ADDRESS(BasePage);
        Status = STATUS_SUCCESS;
        while (ByteSize >= ZEROING_BYTE_RANGE_LENGTH) {
            Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                            PhysicalAddress,
                                            ZEROING_BYTE_RANGE_LENGTH,
                                            MEMORY_ATTRIBUTE_NONE);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            RtlSecureZeroMemory(VirtualAddress, ZEROING_BYTE_RANGE_LENGTH);
            Status = BlMmUnmapVirtualAddress(VirtualAddress,
                                             ZEROING_BYTE_RANGE_LENGTH);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            ByteSize -= ZEROING_BYTE_RANGE_LENGTH;
            PhysicalAddress.QuadPart += ZEROING_BYTE_RANGE_LENGTH;
        }

        if (NT_SUCCESS(Status) && (ByteSize != 0)) {
            Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                            PhysicalAddress,
                                            ByteSize,
                                            MEMORY_ATTRIBUTE_NONE);

            if (NT_SUCCESS(Status)) {
                RtlSecureZeroMemory(VirtualAddress, (UINTN)ByteSize);
                BlMmUnmapVirtualAddress(VirtualAddress, ByteSize);
            }
        }
    }

    //
    // Free the physical pages back to the firmware unless they need to be
    // persisted.
    //

    if (PersistAllocation == FALSE) {
        Status = MmFwFreePages(BasePage, PagesToFree);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // If the pages were returned to the firmware, then clear the firmware
        // allocated attribute.
        //

        CLEAR_FLAGS(MemoryDescriptor->Attributes,
                    MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_ALLOCATED);

        CLEAR_FLAGS(MemoryDescriptor->Attributes,
                    FIRMWARE_SERVICE_MEMORY_ATTRIBUTES);
    }

    //
    // Insert the descriptor into the free list after updating the allocated
    // list.  If the allocation is to be persisted, ensure the memory region is
    // not coalesced with adjacent regions.  If coalesced with another free
    // descriptor, the page allocator may get into a situation where the memory
    // region is reallocated.  Because the memory region is still allocated in
    // the firmware's viewpoint, this reallocation attempt will fail.
    //

    InsertionFlags = MDL_FLAGS_COALESCE;
    if (PersistAllocation != FALSE) {
        InsertionFlags |= MDL_FLAGS_OVERRIDE_COALESCE;
    }

    //
    // If a valid sub-region of the descriptor was free'd, then move the
    // sub-region to the unmapped/unallocated list. Otherwise, move the entire
    // descriptor.
    //

    if (PagesToFree != MemoryDescriptor->PageCount) {

        ASSERT(CHECK_FLAG(MemoryDescriptor->Attributes,
                          MEMORY_ATTRIBUTE_DESCRIPTION_COALESCE) != FALSE);

        MemoryDescriptor = MmMdInitDescriptor(BasePage,
                                              0,
                                              PagesToFree,
                                              MemoryDescriptor->Attributes,
                                              MEMORY_TYPE_FREE);

        if (MemoryDescriptor == NULL) {
            return STATUS_NO_MEMORY;
        }

        Status = MmMdRemoveRegionFromMdl(&MmMdlUnmappedAllocated,
                                         BasePage,
                                         PagesToFree,
                                         MDL_FLAGS_PHYSICAL_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    } else {
        MmMdRemoveDescriptorFromList(&MmMdlUnmappedAllocated, MemoryDescriptor);
        MemoryDescriptor->MemoryType = MEMORY_TYPE_FREE;
    }

    //
    // Clear the coalesce flag since all free regions should be coalescable,
    // but the coalesce flag prevents a non-coalescable region from merging
    // with a coalescable region.
    //

    CLEAR_FLAGS(MemoryDescriptor->Attributes,
                MEMORY_ATTRIBUTE_DESCRIPTION_COALESCE);

    Status = MmMdAddDescriptorToList(&MmMdlUnmappedUnallocated,
                                     MemoryDescriptor,
                                     InsertionFlags);

    return Status;
}

VOID
MmPapRemoveBadMemory (
    __in PMEMORY_DESCRIPTOR MemoryDescriptor
    )

/*++

Routine Description:

    This routine attempts to move the bad memory region from the free
    memory list to the bad, where it will no longer be available to the
    boot library, boot applications, or the OS.

Arguments:

    MemoryDescriptor - Supplies a pointer to descriptor for the bad memory
        region.

Return Value:

    None.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    PLIST_ENTRY NextEntry;
    MEMORY_DESCRIPTOR_LIST RemovalList;
    NTSTATUS Status;

    if (MemoryDescriptor->PageCount < 1) {
        return;
    }

    //
    // Remove as much bad memory as possible from the free memory list.
    //

    Status = MmMdRemoveRegionFromMdlEx(&MmMdlUnmappedUnallocated,
                                       MemoryDescriptor->BasePage,
                                       MemoryDescriptor->PageCount,
                                       MDL_FLAGS_PHYSICAL_ADDRESS,
                                       &RemovalList);

    if (NT_SUCCESS(Status)) {
        NextEntry = RemovalList.Head->Flink;
        while (NextEntry != RemovalList.Head) {
            Descriptor = CONTAINING_RECORD(NextEntry,
                                           MEMORY_DESCRIPTOR,
                                           ListEntry);

            NextEntry = NextEntry->Flink;

            //
            // Add each bad memory region that was successfully removed from
            // the free memory list above to the bad memory list.
            //

            MmMdRemoveDescriptorFromList(&RemovalList, Descriptor);
            Descriptor->MemoryType = MemoryDescriptor->MemoryType;
            Descriptor->Attributes = MemoryDescriptor->Attributes;
            Descriptor->MappedBasePage = 0;
            MmMdAddDescriptorToList(&MmMdlBadMemory,
                                    Descriptor,
                                    MDL_FLAGS_NONE);
        }
    }

    return;
}

VOID
MmPapDeterminePfnRun (
    __in_ecount(Count) ULONGLONG UNALIGNED *PfnArray,
    __in ULONGLONG Count,
    __out PMEMORY_DESCRIPTOR MemoryDescriptor
    )

/*++

Routine Description:

    This routine determines the number of contiguous pages, either ascending,
    or descending, in the array starting from PfnArray[0].

Arguments:

    PfnArray - Supplies a pointer to an array of page frame numbers.

    Count - Supplies the number of elements in the array.

    MemoryDescriptor - Supplies a pointer to a memory descriptor that
        returns the base and length of the run.

Return Value:

    None.

--*/

{

    ULONG Index;
    ULONG RunLength;

    RtlZeroMemory(MemoryDescriptor, sizeof(MEMORY_DESCRIPTOR));
    MemoryDescriptor->BasePage = PfnArray[0];
    if ((Count == 0) || (Count == 1)) {
        MemoryDescriptor->PageCount = Count;
        return;
    }

    //
    // Snoop to see if the next pfn is ascending, or descending, then
    // loop to the extent of the run to detemine its length and base.
    //

    RunLength = 1;
    if (PfnArray[1] == (PfnArray[0] + 1)) {
        RunLength += 1;
        for (Index = 1; Index < (Count - 1); Index += 1) {
            if (PfnArray[Index + 1] == (PfnArray[Index] + 1)) {
                RunLength += 1;

            } else {
                break;
            }
        }

    } else if (PfnArray[1] == (PfnArray[0] - 1)) {
        RunLength += 1;
        MemoryDescriptor->BasePage = PfnArray[1];
        for (Index = 1; Index < (Count - 1); Index++) {
            if (PfnArray[Index + 1] == (PfnArray[Index] - 1)) {
                RunLength += 1;
                MemoryDescriptor->BasePage = PfnArray[Index + 1];

            } else {
                break;
            }
        }
    }

    MemoryDescriptor->PageCount = RunLength;
    return;
}

#if !defined(MM_MIN)

NTSTATUS
MmPapPageAllocatorExtend (
    __in ULONGLONG RequiredPages,
    __in ULONGLONG VirtualBasePage,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __in PMEMORY_ADDRESS_RANGE Range,
    __in ULONG RangeFlags
    )

/*++

Routine Description:

    Reserves and virtually maps memory for the use of the page allocator.
    This routine will allocate memory from the unmapped/unallocated memory
    descriptor list.  If an allocation is successful, the region will be
    virtually mapped and inserted into the mapped/unallocated list.

    This routine is to be called whenever a page allocation can not be made
    from the mapped/unallocated memory descriptor list.

    When executing with virtual address translation enabled, the page allocator
    grows in chunks because there could be architectural limitations to how
    memory is mapped.  For example, IA64 uses translation registers to map a
    memory region.  There are only a fixed number of TRs to use and therefore,
    require larger granularity in their allocations.

Arguments:

    RequiredPages - Supplies a page number request for the amount of memory to
        reserve.

    VirtualBasePage - Supplies the preferred virtual base page for the
        allocation.  This is only valid if the attributes specify that a
        preferred base page is to be respected.

    Attributes - Supplies the attributes that the firmware allocation should be
        made with.

    Alignment - Supplies the required page alignment of the firmware allocation.

    Range - Supplies the range of base pages the page allocator should extend
        to. The ranges are expected to be page-aligned physical addresses and
        are inclusive (that is, the maximum page is included in the range).

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    MEMORY_DESCRIPTOR Allocation;
    ULONG AllocationAttributes;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    ULONG CacheAttributes;
    BOOL FixedAddressRequest;
    ULONG LargePageSize;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONGLONG RangeSize;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // Adjust the size and alignment requirements based on the minimum size
    // requirements of the page allocator.
    //
    // If the caller has specific region requests, do not modify the request,
    // possibly jeopardizing the success of the operation.
    //

    FixedAddressRequest = CHECK_FLAG(Attributes,
                                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if ((FixedAddressRequest == FALSE) && (ARGUMENT_PRESENT(Range) == FALSE)) {
        if (RequiredPages < PapMinimumAllocationCount) {
            RequiredPages = PapMinimumAllocationCount;
            Alignment = PapMinimumAllocationCount;
        }
    }

    //
    // If the request has any special allocation attribute requests, ensure
    // that the memory allocated for the page allocator contains such
    // attributes.
    //
    // At the same time, do not include allocation attributes in the request.
    // These attributes are superficial and created by the page allocator.
    //

    AllocationAttributes = Attributes & MEMORY_ATTRIBUTE_ALLOCATION_MASK;
    CLEAR_FLAGS(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_MASK);
    if (CHECK_FLAG(AllocationAttributes,
                   MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE)) {

        //
        // Allocate at least a large page of memory.
        //

        LargePageSize = MmArchQueryLargePageSize();
        RequiredPages = ROUND_UP(RequiredPages, LargePageSize);
        Alignment = ROUND_UP(Alignment, LargePageSize);
    }

    //
    // Reserve physical memory for the page allocator, using the attributes and
    // alignment requested by the caller.  Remove any cacheability attributes
    // since they do not apply to the physical page allocation.
    //
    // N.B. The memory type used in the page allocator extend does not matter
    //      since no destination descriptor list is specified in the below
    //      call.
    //

    INITIALIZE_UNUSED_RANGE(&AllocationRequest.MappedRange);
    if (ARGUMENT_PRESENT(Range) != FALSE) {
        RangeSize = Range->MaximumAddress.QuadPart -
                    Range->MinimumAddress.QuadPart;

        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 Range->MinimumAddress.QuadPart >> PAGE_SHIFT,
                                 RangeSize >> PAGE_SHIFT);

    } else {
        RangeSize = MAXIMUM_PAGE_RANGE_LENGTH - PapMinimumPhysicalPage;
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 PapMinimumPhysicalPage,
                                 RangeSize);
    }

    CacheAttributes = Attributes & MEMORY_ATTRIBUTE_CACHE_MASK;
    CLEAR_FLAGS(Attributes, MEMORY_ATTRIBUTE_CACHE_MASK);
    if ((RangeFlags != 0) &&
        ((RangeFlags & (~MEMORY_RANGE_VALID_FLAGS)) == 0)) {

        AllocationRequest.RangeFlags = RangeFlags;

    } else {
        AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;
    }

    AllocationRequest.PageCount = RequiredPages;
    AllocationRequest.Attributes = Attributes;
    AllocationRequest.Alignment = Alignment;
    Status = MmPaAllocatePages(NULL,
                               &Allocation,
                               &MmMdlUnmappedUnallocated,
                               &AllocationRequest,
                               MEMORY_TYPE_FREE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Initialize a global memory descriptor for the allocated region.
    // Regardless of the status of BlMmMapPhysicalAddress, this routine
    // needs to insert the memory region into a memory descriptor list.
    // (mapped list when successful, unmapped list when unsuccessful).
    //

    SET_FLAGS(Allocation.Attributes, AllocationAttributes);
    MemoryDescriptor = MmMdInitDescriptor(Allocation.BasePage,
                                          Allocation.MappedBasePage,
                                          Allocation.PageCount,
                                          Allocation.Attributes,
                                          MEMORY_TYPE_FREE);

    ASSERT(MemoryDescriptor != NULL);

    //
    // Virtually map the allocated region.
    //

    VirtualAddress = (PVOID)(UINTN)(VirtualBasePage << PAGE_SHIFT);
    PhysicalAddress.QuadPart = Allocation.BasePage << PAGE_SHIFT;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    Allocation.PageCount << PAGE_SHIFT,
                                    AllocationAttributes | CacheAttributes);

    //
    // When successful, insert the memory region into a mapped memory
    // descriptor list and record the allocation in the firmware allocation
    // tracking list. On failure, free the pages and insert the memory
    // region back into the unmapped list. If the memory region could not be
    // freed, move the region to the unmapped/allocated list.
    //

    if (Status == STATUS_SUCCESS) {
        VirtualBasePage = (UINTN)VirtualAddress >> PAGE_SHIFT;
        MemoryDescriptor->MappedBasePage = VirtualBasePage;
        SET_FLAGS(MemoryDescriptor->Attributes, CacheAttributes);
        Status = MmMdAddDescriptorToList(&MmMdlMappedUnallocated,
                                         MemoryDescriptor,
                                         MDL_FLAGS_COALESCE);

        ReturnStatus = Status;

        //
        // Add the region to the firmware allocation tracking list. Do not
        // coalesce these regions.
        //

        MemoryDescriptor = MmMdInitDescriptor(Allocation.BasePage,
                                              0,
                                              Allocation.PageCount,
                                              MEMORY_ATTRIBUTE_NONE,
                                              MEMORY_TYPE_FREE);

        ASSERT(MemoryDescriptor != NULL);

        Status = MmMdAddDescriptorToList(&MmMdlFwAllocationTracker,
                                         MemoryDescriptor,
                                         MDL_FLAGS_NONE);

        ASSERT(NT_SUCCESS(Status));

    } else {
        ReturnStatus = Status;
        Status = MmFwFreePages(Allocation.BasePage, Allocation.PageCount);
        if (NT_SUCCESS(Status)) {

            //
            // Clear any attributes, preserving cacheability and protection.
            //

            MemoryDescriptor->Attributes &= (MEMORY_ATTRIBUTE_CACHE_MASK |
                                             MEMORY_ATTRIBUTE_PROTECT_MASK);

            Status = MmMdAddDescriptorToList(&MmMdlUnmappedUnallocated,
                                             MemoryDescriptor,
                                             MDL_FLAGS_COALESCE);

        } else {
            Status = MmMdAddDescriptorToList(&MmMdlUnmappedAllocated,
                                             MemoryDescriptor,
                                             MDL_FLAGS_COALESCE);
        }

        //
        // Check that addition of the descriptor to the unmapped/unallocated or
        // unmapped/allocated succeeded.
        //
        // TODO: Failure to add the descriptor to unmapped/unallocated list on
        //       successful free would create a physical memory hole. This
        //       might lead to issues during memory map synchronization.
        //       Specifically, synchronization would fail because the region
        //       freed above would be described as available in the firmware
        //       memory map but not described in the library memory map at all.
        //

        ASSERT(NT_SUCCESS(Status) != FALSE);
    }

    return ReturnStatus;
}

#endif

NTSTATUS
MmPapAllocatePhysicalPagesInRange (
    __inout PPHYSICAL_ADDRESS Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __inout PMEMORY_DESCRIPTOR_LIST Destination,
    __in_opt PMEMORY_ADDRESS_RANGE Range,
    __in_opt ULONG RangeFlags
    )

/*++

Routine Description:

    Allocates a block of contiguous memory pages (physical) which satisfy the
    caller's size, attribute, alignment, and range requirements.

Arguments:

    Address - Supplies the preferred address for the allocation.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

    Destination - Supplies a pointer to a memory descriptor list containing
        allocated memory. If a memory region can be successfully allocated, it
        will be removed from the source (unallocated) list and moved to the
        destination (allocated) list.

    Range - Supplies the range of base pages that will satisfy the request. The
        ranges are physical addresses and are inclusive (that is, the maximum
        page is included in the range). NULL indicates the default address range
        should be used.

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    MEMORY_DESCRIPTOR Allocation;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    ULONGLONG RangeSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfacePagalloc);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Parameter checking.
    //

    if ((Address == NULL) || (Pages == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePhysicalPagesInRangeEnd;
    }

    //
    // Physical allocations should only end up on the unmapped / allocated list
    // or the persistent memory list.
    //

    if ((Destination != &MmMdlUnmappedAllocated) &&
        (Destination != &MmMdlPersistentMemory)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePhysicalPagesInRangeEnd;
    }

    //
    // When no alignment request is made, page align the allocation.
    //

    if (Alignment == 0) {
        Alignment = 1;
    }

    //
    // Validate the range parameter if it was passed.
    //

    if ((ARGUMENT_PRESENT(Range) != FALSE) &&
        ((ULONGLONG)Range->MinimumAddress.QuadPart >=
         (ULONGLONG)Range->MaximumAddress.QuadPart)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePhysicalPagesInRangeEnd;
    }

    //
    // Attempt to allocate the requested memory from the unallocated memory
    // descriptor list.  The mapped range is not used for physical memory
    // allocations.
    //

    INITIALIZE_UNUSED_RANGE(&AllocationRequest.MappedRange);
    if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 Address->QuadPart >> PAGE_SHIFT,
                                 Pages);

    } else if (ARGUMENT_PRESENT(Range) != FALSE) {
        RangeSize = Range->MaximumAddress.QuadPart -
                    Range->MinimumAddress.QuadPart;

        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 Range->MinimumAddress.QuadPart >> PAGE_SHIFT,
                                 RangeSize >> PAGE_SHIFT);

    } else {
        RangeSize = MAXIMUM_PAGE_RANGE_LENGTH - PapMinimumPhysicalPage;
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 PapMinimumPhysicalPage,
                                 RangeSize);
    }

    if ((RangeFlags != 0) &&
        ((RangeFlags & (~MEMORY_RANGE_VALID_FLAGS)) == 0)) {

        AllocationRequest.RangeFlags = RangeFlags;

    } else {
        AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;
    }

    AllocationRequest.PageCount = Pages;
    AllocationRequest.Attributes = Attributes;
    AllocationRequest.Alignment = Alignment;
    Status = MmPaAllocatePages(Destination,
                               &Allocation,
                               &MmMdlUnmappedUnallocated,
                               &AllocationRequest,
                               MemoryType);

    //
    // Return the address of the allocation.
    //

    if (NT_SUCCESS(Status)) {
        Address->QuadPart = Allocation.BasePage << PAGE_SHIFT;
    }

AllocatePhysicalPagesInRangeEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}

NTSTATUS
MmPapAllocatePagesInRange (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Pages * PAGE_SIZE))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Pages * PAGE_SIZE))
         PVOID *Address,
    __in ULONGLONG Pages,
    __in MEMORY_TYPE MemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __inout PMEMORY_DESCRIPTOR_LIST Destination,
    __in_opt PMEMORY_ADDRESS_RANGE Range,
    __in_opt ULONG RangeFlags
    )

/*++

Routine Description:

    This routine allocates a block of contiguous memory pages which satisfy the
    caller's size, attribute, alignment, and range requirements.

    When executing with address translation enabled, the address referred to
    by the parameter, Address, is virtual.

Arguments:

    Address - Supplies the preferred address for the allocation.  A preferred
        address is a virtual address if address translation is enabled.  As an
        input parameter, this is ignored unless the attributes indicate a fixed
        address is required.  Also receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    MemoryType - Supplies the memory type the allocation should be made for.

    Attributes - Supplies the attributes required of the memory region to be
        allocated.

    Destination - Supplies a pointer to a memory descriptor list containing
        allocated memory. If a memory region can be successfully allocated, it
        will be removed from the source (unallocated) list and moved to the
        destination (allocated) list.

    Alignment - Supplies the required alignment of the allocation.  All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

    Range - Supplies the range of base physical pages that will satisfy the
        request. The ranges are physical addresses and are inclusive (that is,
        the maximum page is included in the range). NULL indicates the default
        physical address range should be used.

    RangeFlags - Supplies range flags indicating how the allocation request
        should be satisfied. Supported flags are:

        MEMORY_RANGE_PREFER_LOW_ADDRESSES

        MEMORY_RANGE_PREFER_HIGH_ADDRESSES

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Address is NULL.

    STATUS_NO_MEMORY if allocation fails.

    STATUS_NOT_SUPPORTED if a specified allocation attribute is not supported.

--*/

{

    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

#if !defined(MM_MIN)

    MEMORY_DESCRIPTOR Allocation;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    ULONGLONG RangeSize;

#endif

    BlpInterfaceEnter(InterfacePagalloc);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Parameter checking.
    //

    if ((ARGUMENT_PRESENT(Address) == FALSE) || (Pages == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePagesInRangeEnd;
    }

    //
    // Validate the range parameter if it was passed.
    //

    if ((ARGUMENT_PRESENT(Range) != FALSE) &&
        (Range->MinimumAddress.QuadPart >= Range->MaximumAddress.QuadPart)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePagesInRangeEnd;
    }

    //
    // If the boot application is running without virtual address translation,
    // send request to MmPapAllocatePhysicalPages.
    //

    Status = STATUS_SUCCESS;
    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {
            PhysicalAddress.QuadPart = (ULONGLONG)(UINTN)*Address;

        } else {
            PhysicalAddress.QuadPart = 0;
        }

        //
        // If this was not intended to be a persistent allocation, the physical
        // allocation should end up on unmapped / allocated.
        //

        if (Destination != &MmMdlPersistentMemory) {
            Destination = &MmMdlUnmappedAllocated;
        }

        Status = MmPapAllocatePhysicalPagesInRange(&PhysicalAddress,
                                                   Pages,
                                                   MemoryType,
                                                   Attributes,
                                                   Alignment,
                                                   Destination,
                                                   Range,
                                                   RangeFlags);

        *Address = (PVOID)(UINTN)PhysicalAddress.QuadPart;
        goto AllocatePagesInRangeEnd;
    }

#if !defined(MM_MIN)

    //
    // Allocations through this routine should only end up on the mapped /
    // allocated list or the persistent memory list.
    //

    if ((Destination != &MmMdlMappedAllocated) &&
        (Destination != &MmMdlPersistentMemory)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AllocatePagesInRangeEnd;
    }

    //
    // When no alignment request is made, page align the allocation.
    //

    if (Alignment == 0) {
        Alignment = 1;
    }

    //
    // Explicitly disallow virtual memory allocations with with either the
    // library only allocate or library only free set.  Allocations made via
    // this API are made from the mapped unallocated list.  All memory in this
    // list has already been allocated using firmware services and therefore
    // this routine can not service special firmware allocator attributes.
    //

    if (CHECK_FLAG(Attributes, FIRMWARE_SERVICE_MEMORY_ATTRIBUTES)) {
        Status = STATUS_NOT_SUPPORTED;
        goto AllocatePagesInRangeEnd;
    }

    //
    // Attempt to allocate the requested memory from the MDL that has been
    // pre-populated with mapped pages for use by the page allocator.
    //

    if (ARGUMENT_PRESENT(Range) != FALSE) {
        RangeSize = Range->MaximumAddress.QuadPart -
                    Range->MinimumAddress.QuadPart;

        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 Range->MinimumAddress.QuadPart >> PAGE_SHIFT,
                                 RangeSize >> PAGE_SHIFT);

    } else {
        RangeSize = MAXIMUM_PAGE_RANGE_LENGTH - PapMinimumPhysicalPage;
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 PapMinimumPhysicalPage,
                                 RangeSize);
    }

    if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.MappedRange,
                                 (UINTN)*Address >> PAGE_SHIFT,
                                 Pages);

    } else if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_KSEG)) {
        AllocationRequest.MappedRange.Minimum =
            MmArchKsegAddressRange.Minimum >> PAGE_SHIFT;

        AllocationRequest.MappedRange.Maximum =
            MmArchKsegAddressRange.Maximum >> PAGE_SHIFT;

// ARM64_64KPAGE_WORKAROUND: hack for 64KB alignment
#if defined(_ARM64_)
        Alignment = ALIGN_RANGE_UP(Alignment, _64KB / PAGE_SIZE);
#endif
    } else {
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.MappedRange,
                                 0,
                                 MAXIMUM_PAGE_RANGE_LENGTH);
    }

    //
    // Use the supplied range flags if they are valid, otherwise use the
    // default bottoms up allocation policy.
    //

    if ((RangeFlags != 0) &&
        ((RangeFlags & (~MEMORY_RANGE_VALID_FLAGS)) == 0)) {

        AllocationRequest.RangeFlags = RangeFlags;

    } else {
        AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;
    }

    AllocationRequest.PageCount = Pages;
    AllocationRequest.Attributes = Attributes;
    AllocationRequest.Alignment = Alignment;
    Status = MmPaAllocatePages(Destination,
                               &Allocation,
                               &MmMdlMappedUnallocated,
                               &AllocationRequest,
                               MemoryType);

    //
    // If we could not allocate the desired region from a previously allocated
    // region, we need to grow the mapped region for the page allocator.
    //

    if (!NT_SUCCESS(Status)) {

        //
        // Grow the virtually mapped page allocator.
        //

        Status = MmPapPageAllocatorExtend(Pages,
                                          (UINTN)*Address >> PAGE_SHIFT,
                                          Attributes,
                                          Alignment,
                                          Range,
                                          RangeFlags);

        if (!NT_SUCCESS(Status)) {

            //
            // Could not grow page allocator.  Must be out of memory.
            //

            Status = STATUS_NO_MEMORY;

        } else {

            //
            // If we successfully grew the page allocator, make the requested
            // allocation.
            //

            Status = MmPaAllocatePages(Destination,
                                       &Allocation,
                                       &MmMdlMappedUnallocated,
                                       &AllocationRequest,
                                       MemoryType);

            //
            // This allocation should not result in an error since the page
            // allocator extension was made exclusively for this allocation
            // request.
            //

            ASSERT(Status == STATUS_SUCCESS);
        }
    }

    //
    // Return the address of the allocation.
    //

    if (NT_SUCCESS(Status)) {
        *Address = (PVOID)(ULONG_PTR)(Allocation.MappedBasePage << PAGE_SHIFT);
    }

#endif

AllocatePagesInRangeEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfacePagalloc);
    return Status;
}


