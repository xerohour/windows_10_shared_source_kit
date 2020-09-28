/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    translat.c

Abstract:

    Implementation of the Boot Environment Library's memory management APIs
    for address translation.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "mm.h"

// -------------------------------------------------------------------- Globals

PMM_ENABLE_TRANSLATION BlMmEnableTranslation;
PMM_DISABLE_TRANSLATION BlMmDisableTranslation;
PMM_RELOCATE_SELF_MAP BlMmRelocateSelfMap;
PMM_ZERO_VIRTUAL_ADDRESS_RANGE BlMmZeroVirtualAddressRange;
PMM_MOVE_VIRTUAL_ADDRESS_RANGE BlMmMoveVirtualAddressRange;
PMM_FLUSH_TLB BlMmFlushTlb;

//
// Define a memory descriptor list for tracking free virtual addresses.
//
// This list tracks all free virtual address space available for allocations
// that do not specify an allocation attribute. On x86 and amd64, this includes
// all unused virtual address space except for the page tables.
//

MEMORY_DESCRIPTOR_LIST MmMdlFreeVirtual;

//
// Define a memory descriptor list for tracking active virtual mappings.
//

MEMORY_DESCRIPTOR_LIST MmMdlMappingTrackers;

// ----------------------------------------------------------------- Prototypes

VOID
TrpDeleteMappingTracker (
    __in PVOID VirtualAddress,
    __in ULONG64 Size
    );

NTSTATUS
TrpGenerateMappingTracker (
    __in PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONG64 Size,
    __in ULONG Attributes
    );

NTSTATUS
TrpMapCacheAttributesAreCompatible (
    __in ULONG CurrentAttributes,
    __in ULONG DesiredAttributes,
    __out PBOOLEAN Compatible
    );

// ------------------------------------------------------------------ Functions

VA_TRANSLATION_TYPE
BlMmQueryTranslationType (
    VOID
    )

/*++

Routine Description:

    Queries the Boot Environment Library to determine the address translation
    type of the executing Boot Environment Application.

    N.B. This routine cannot call BlpInterfaceEnter or BlpInterfaceExit.
         These routines can call out to this function, so calling back into
         them would trigger infinite recursion.

Arguments:

    None.

Return Value:

    The type of address translation the executing boot application is using.

--*/

{

    return MmTranslationType;
}

BOOLEAN
BlMmTranslateVirtualAddress (
    __in PVOID VirtualAddress,
    __out PPHYSICAL_ADDRESS PhysicalAddress
    )

/*++

Routine Description:

    Determines if an address is virtually mapped.

Arguments:

    VirtualAddress - Supplies the linear address for which the corresponding
        physical address is to be determined.

    PhysicalAddress - Supplies a pointer to a variable that receives the
        physical address mapped by the given virtual address.

Return Value:

    TRUE if the virtual address is mapped.

    FALSE otherwise.

--*/

{

    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceTranslate);
    if ((VirtualAddress == NULL) || (PhysicalAddress == NULL)) {
        ReturnValue = FALSE;

    } else {
        ReturnValue = MmArchTranslateVirtualAddress(VirtualAddress,
                                                    PhysicalAddress,
                                                    NULL);
    }

    BlpInterfaceExit(InterfaceTranslate);
    return ReturnValue;
}

NTSTATUS
BlMmMapPhysicalAddress (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Size))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Size))
         PVOID *VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG Attributes
    )

/*++

Routine Description:

    Virtually maps a physical address.

Arguments:

    VirtualAddress - Supplies a pointer to the preferred virtual address to use
        for the mapping.  This is only valid if mapping attributes specify a
        fixed address.  Also receives the virtually mapped address of the
        specified physical address.

    PhysicalAddress - Supplies the physical address to virtually map.  If
        MM_PFN_NO_MAP is specified, then all required paging structures will be
        allocated to map the specified virtual address range.  However, no final
        mappings will be made. MM_PFN_NO_MAP is only supported on x64.  Calls to
        map this physical address will fail on other architectures.

    Size - Supplies the number of bytes to virtually map.

    Attributes - Supplies the attributes to use when creating the mapping.
        These include cache attributes and VA selection policy.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if PhysicalAddress or VirtualAddress are NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    return BlMmMapPhysicalAddressEx(VirtualAddress,
                                    PhysicalAddress,
                                    Size,
                                    Attributes,
                                    MEMORY_TRANSLATE_FLAGS_NONE);
}

NTSTATUS
BlMmMapPhysicalAddressEx (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Size))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Size))
         PVOID *VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG Attributes,
    __in ULONG Flags
    )

/*++

Routine Description:

    Virtually maps a physical address.

Arguments:

    VirtualAddress - Supplies a pointer to the preferred virtual address to use
        for the mapping.  This is only valid if mapping attributes specify a
        fixed address.  Also receives the virtually mapped address of the
        specified physical address.

    PhysicalAddress - Supplies the physical address to virtually map.  If
        MM_PFN_NO_MAP is specified, then all required paging structures will be
        allocated to map the specified virtual address range.  However, no final
        mappings will be made. MM_PFN_NO_MAP is only supported on x64.  Calls to
        map this physical address will fail on other architectures.

    Size - Supplies the number of bytes to virtually map.

    Attributes - Supplies the attributes to use when creating the mapping.
        These include cache attributes and VA selection policy.

    Flags - Supplies the flags defining the behavior of this mapping.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if PhysicalAddress or VirtualAddress are NULL.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    ULONG AllocationPolicy;
    ULONGLONG BasePage;
    ULONG CacheAttributes;
    PMEMORY_DESCRIPTOR Descriptor;
    ULONGLONG Diff;
    ULONGLONG EndPage;
    PVOID MappedVirtualAddress;
    ULONGLONG MapSize;
    PMEMORY_DESCRIPTOR_LIST Mdl;
    PHYSICAL_ADDRESS PA;
    ULONGLONG Page;
    PMEMORY_DESCRIPTOR SplitDescriptor;
    NTSTATUS Status;
    PVOID VA;
    ULONGLONG VirtualPage;

    BlpInterfaceEnter(InterfaceTranslate);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Parameter checking.  Ensure that an output parameter exists and that
    // the caller requested something to be mapped.
    //

    MappedVirtualAddress = 0;
    if ((ARGUMENT_PRESENT(VirtualAddress) == FALSE) || (Size == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlMmMapPhysicalAddressExEnd;
    }

    if ((CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) &&
        (PhysicalAddress.QuadPart == MM_PFN_NO_MAP) &&
        (*VirtualAddress == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlMmMapPhysicalAddressExEnd;
    }

    if (CHECK_FLAG(Flags, MEMORY_TRANSLATE_FLAGS_RESERVED_VIRTUAL) &&
        !CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Allocation policy is determined by the attributes flags.  Make sure that
    // exactly one of the supported allocation policies are supplied.
    //

    AllocationPolicy = Attributes & (MEMORY_ATTRIBUTE_ALLOCATION_KSEG |
                                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if ((AllocationPolicy != MEMORY_ATTRIBUTE_ALLOCATION_KSEG) &&
        (AllocationPolicy != MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) &&
        (AllocationPolicy != MEMORY_ATTRIBUTE_ALLOCATION_INDIFFERENT)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Cache attributes are determined by the attributes as well.  Make sure
    // that only one cache attribute is supplied.
    //

    CacheAttributes = Attributes & MEMORY_ATTRIBUTE_CACHE_MASK;
    if ((CacheAttributes & (CacheAttributes - 1)) != 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Convert a DMA cache attribute to the "none" attribute.  All currently
    // supported platforms guarantee DMA cache coherency, so any attribute
    // can be used for allocations designed for DMA.
    //

    if (CacheAttributes == MEMORY_ATTRIBUTE_CACHE_DMA) {
        CacheAttributes = MEMORY_ATTRIBUTE_CACHE_INDIFFERENT;
    }

    //
    // Based on policy, choose the virtual address to map the physical address
    // to.
    //

    Status = MmSelectMappingAddress(&VA,
                                    PhysicalAddress,
                                    *VirtualAddress,
                                    Size,
                                    AllocationPolicy,
                                    Attributes);

    if (!NT_SUCCESS(Status)) {
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Attempt to map the specified physical address range.  This will return
    // the base physical and virtual address that are mapped, which may
    // encompass a greater region than the range requested by the caller.
    //

    PA.QuadPart = PhysicalAddress.QuadPart;
    MapSize = Size;

    //
    // Currently only amd64 and x86 support protection attributes.
    //

#if defined(_X86_) || defined(_AMD64_)

    Status = MmMapPhysicalAddress(&PA, &VA, &MapSize, Attributes);

#else

    Status = MmMapPhysicalAddress(&PA, &VA, &MapSize, CacheAttributes);

#endif

    if (!NT_SUCCESS(Status)) {
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Record the virtual base of the range requested by the caller.
    //

    MappedVirtualAddress = Add2Ptr(VA,
                                   (PhysicalAddress.QuadPart - PA.QuadPart));

    //
    // There are no changes to make to the global MDLs if translation is not
    // enabled.  In this case, exit now.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Mark the virtual address consumption by removing the mapped range from
    // the MDL describing free virtual addresses.
    //

    if (!CHECK_FLAG(Flags, MEMORY_TRANSLATE_FLAGS_RESERVED_VIRTUAL)) {
        BasePage = ((ULONG_PTR)VA) >> PAGE_SHIFT;
        Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                         BasePage,
                                         MapSize >> PAGE_SHIFT,
                                         MDL_FLAGS_VIRTUAL_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            MmUnmapVirtualAddress(&VA, &MapSize);
            goto BlMmMapPhysicalAddressExEnd;
        }
    }

    //
    // Generate a tracker for this mapping so it can be released during
    // library destruction if the caller does not explicitly unmap it.  The
    // contents of the tracker are intended to match the parameters the caller
    // will use when eventually unmapping this routine.  Therefore the mapping
    // size requested by the caller is logged rather than the expanded size
    // (which is not exposed to the caller) returned by the architecture
    // specific mapping routine.
    //
    // N.B. The MDL insertion routine uses static global descriptors instead
    //      of allocating memory and can not fail in general.  The section
    //      removed from the free virtual MDL is not reinserted on failure due
    //      to the low probability of failure and the absence of a good
    //      mechanism for determining exactly which pages (out of the total
    //      mapped range) were really removed from the MDL and should now be
    //      reinserted.
    //

    Status = TrpGenerateMappingTracker(MappedVirtualAddress,
                                       PhysicalAddress,
                                       Size,
                                       Attributes);

    if (!NT_SUCCESS(Status)) {
        MmUnmapVirtualAddress(&VA, &MapSize);
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // If MM_PFN_NO_MAP was specified, there is no physical page to migrate
    // across physical address MDLs.  Exit now.
    //

    if (PhysicalAddress.QuadPart == MM_PFN_NO_MAP) {
        goto BlMmMapPhysicalAddressExEnd;
    }

    //
    // Migrate any descriptors describing the newly mapped region.
    //

    Page = PA.QuadPart >> PAGE_SHIFT;
    EndPage = (PA.QuadPart + MapSize) >> PAGE_SHIFT;
    VirtualPage = (UINTN)VA >> PAGE_SHIFT;

    ASSERT(Page != EndPage);

    do {

        //
        // Attempt to find the memory descriptor describing the newly mapped
        // region.  Start by checking the allocated memory descriptor list.
        //

        Mdl = &MmMdlUnmappedAllocated;
        Descriptor = MmMdFindDescriptor(Page,
                                        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED,
                                        MDL_FLAGS_PHYSICAL_ADDRESS);

        if (Descriptor == NULL) {

            //
            // Try once more, searching the persistent allocation list.
            //

            Mdl = &MmMdlPersistentMemory;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_PERSISTENT,
                                            MDL_FLAGS_PHYSICAL_ADDRESS);
        }

        if (Descriptor == NULL) {

            //
            // Try once more, searching for the unmapped region in the
            // free list.
            //

            Mdl = &MmMdlUnmappedUnallocated;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED,
                                            MDL_FLAGS_PHYSICAL_ADDRESS);
        }

        if (Descriptor == NULL) {

            //
            // Try once more, searching the reserved list.
            //

            Mdl = &MmMdlReservedAllocated;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_RESERVED_ALLOCATED,
                                            MDL_FLAGS_PHYSICAL_ADDRESS);
        }

        if (Descriptor != NULL) {

            //
            // Remove the memory descriptor from the unmapped MDL so it can be
            // added to an mapped memory descriptor list.
            //

            MmMdRemoveDescriptorFromList(Mdl, Descriptor);

            //
            // If needed, create a memory descriptor for any memory described
            // by the descriptor that was not mapped.  These region need to
            // remain in the original list.
            //
            // The mapped region can either split a descriptor at the start
            // or end of the region described by Descriptor.
            //

            if (Descriptor->BasePage < Page) {
                Diff = Page - Descriptor->BasePage;
                SplitDescriptor = MmMdInitDescriptor(
                                          Descriptor->BasePage,
                                          0,
                                          Diff,
                                          Descriptor->Attributes,
                                          Descriptor->MemoryType);

                ASSERT(SplitDescriptor != NULL);

                MmMdAddDescriptorToList(Mdl, SplitDescriptor, MDL_FLAGS_NONE);

                //
                // Continue to use Descriptor to describe the newly
                // mapped region.  Update its values to illustrate the
                // omission above.
                //

                Descriptor->PageCount -= Diff;
                Descriptor->BasePage = Page;
            }

            if (EndPage < (Descriptor->BasePage + Descriptor->PageCount)) {
                Diff = EndPage - Page;
                SplitDescriptor = MmMdInitDescriptor(
                                          EndPage,
                                          0,
                                          Descriptor->PageCount - Diff,
                                          Descriptor->Attributes,
                                          Descriptor->MemoryType);

                ASSERT(SplitDescriptor != NULL);

                MmMdAddDescriptorToList(Mdl, SplitDescriptor, MDL_FLAGS_NONE);

                //
                // Continue to use Descriptor to describe the newly
                // mapped region.  Update its values to illustrate the
                // omission above.
                //

                Descriptor->PageCount = Diff;
            }

            //
            // Add the mapped region to the proper mapped descriptor list.
            //

            Descriptor->MappedBasePage = VirtualPage;
            if ((Mdl != &MmMdlReservedAllocated) &&
                (Mdl != &MmMdlPersistentMemory)) {

                if (Mdl == &MmMdlUnmappedAllocated) {
                    Mdl = &MmMdlMappedAllocated;

                } else {
                    Mdl = &MmMdlMappedUnallocated;
                }
            }

            MmMdAddDescriptorToList(Mdl, Descriptor, MDL_FLAGS_NONE);
            Page += Descriptor->PageCount;
            VirtualPage += Descriptor->PageCount;

        } else {

            //
            // If we were unable to find the mapped region, skip the page.
            // It could just be a hole in the physical memory map or, a
            // previously mapped page that was remapped (to the same virtual
            // address) by MmArchMapPhysicalAddressEx and lumped into the mapped
            // range returned to us, or a previously mapped page that is now
            // being additionally mapped to a different virtual address.
            //

            Page += 1;
            VirtualPage += 1;
        }

    } while (Page < EndPage);

BlMmMapPhysicalAddressExEnd:

    //
    // Upon success, return the virtual address of the mapped region.
    //

    if (NT_SUCCESS(Status)) {
        *VirtualAddress = MappedVirtualAddress;
    }

    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
BlMmUnmapVirtualAddress (
    __in_bcount(Size) PVOID VirtualAddress,
    __in ULONGLONG Size
    )

/*++

Routine Description:

    This routine will invalidate the specified virtual memory range.

Arguments:

    VirtualAddress - Supplies the start of the virtual address range to unmap.

    Size - Supplies the number of bytes to unmap.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if VirtualAddress is NULL or VirtualAddress is
        not currently mapped.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    return BlMmUnmapVirtualAddressEx(VirtualAddress,
                                     Size,
                                     MEMORY_TRANSLATE_FLAGS_NONE);
}

NTSTATUS
BlMmUnmapVirtualAddressEx (
    __in_bcount(Size) PVOID VirtualAddress,
    __in ULONGLONG Size,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine will invalidate the specified virtual memory range.

Arguments:

    VirtualAddress - Supplies the start of the virtual address range to unmap.

    Size - Supplies the number of bytes to unmap.

    Flags - Supplies the flags defining the behavior of this unmapping.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if VirtualAddress is NULL or VirtualAddress is
        not currently mapped.

    STATUS_NO_MEMORY if allocation fails.

--*/

{

    ULONGLONG BasePage;
    ULONG64 CapturedSize;
    PVOID CapturedVa;
    PMEMORY_DESCRIPTOR Descriptor;
    ULONGLONG Diff;
    ULONGLONG EndPage;
    PMEMORY_DESCRIPTOR_LIST Mdl;
    ULONGLONG Page;
    ULONG ReinsertionFlags;
    PMEMORY_DESCRIPTOR SplitDescriptor;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTranslate);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Sanitize input parameters.
    //

    if ((VirtualAddress == NULL) || (Size == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlMmUnmapVirtualAddressExEnd;
    }

    //
    // Unmap the address range.
    // N.B. This call may unmap a larger range than specified by the caller.
    //

    CapturedVa = VirtualAddress;
    CapturedSize = Size;
    Status = MmUnmapVirtualAddress(&VirtualAddress, &Size);
    if (!NT_SUCCESS(Status)) {
        goto BlMmUnmapVirtualAddressExEnd;
    }

    //
    // Update all memory descriptors describing any part of the unmapped
    // memory region.  If address translation is not enabled, there are
    // no changes to make and therefore the API can successfully return.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        goto BlMmUnmapVirtualAddressExEnd;
    }

    //
    // [VirtualAddress, VirtualAddress + Size] has been successfully unmapped.
    // Mark this address space as free for future virtual address allocations.
    // The tracker MDL entry inserted during mapping construction must also be
    // removed.  The trackers carry parameters supplied by the caller, not the
    // possibly expanded mapping dimensions returned by the architecture
    // specific unmap routine.
    //
    // N.B. Some mapping dispositions allow multiple active mappings to share
    //      the same virtual address range, implying that the same range may
    //      be unmapped multiple times.  When adding ranges to the free
    //      virtual MDL, overlapping descriptors are truncated to avoid a
    //      wasteful condition where multiple overlapping MDL entries mark the
    //      same single range as available.
    //

    TrpDeleteMappingTracker(CapturedVa, CapturedSize);
    if (!CHECK_FLAG(Flags, MEMORY_TRANSLATE_FLAGS_RESERVED_VIRTUAL)) {
        BasePage = ((ULONG_PTR)VirtualAddress) >> PAGE_SHIFT;
        Descriptor = MmMdInitDescriptor(BasePage,
                                        0,
                                        Size >> PAGE_SHIFT,
                                        MEMORY_ATTRIBUTE_NONE,
                                        MEMORY_TYPE_FREE);

        if (Descriptor == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlMmUnmapVirtualAddressExEnd;
        }

        MmMdAddDescriptorToList(&MmMdlFreeVirtual,
                                Descriptor,
                                MDL_FLAGS_COALESCE | MDL_FLAGS_TRUNCATE);
    }


    //
    // Each physical address descriptor describing the physical range that is
    // now unmapped must be updated and moved to the proper unmapped MDL.
    //

    Page = (UINTN)VirtualAddress >> PAGE_SHIFT;
    EndPage = ((UINTN)VirtualAddress + Size) >> PAGE_SHIFT;

    ASSERT(Page != EndPage);

    do {

        //
        // Attempt to find the memory descriptor describing the region that
        // was just unmapped.  Start by checking the mapped / allocated MDL.
        //

        Mdl = &MmMdlMappedAllocated;
        Descriptor = MmMdFindDescriptor(Page,
                                        MEMORYMAP_TYPE_MAPPED_ALLOCATED,
                                        MDL_FLAGS_VIRTUAL_ADDRESS);

        if (Descriptor == NULL) {

            //
            // Try once more, searching the persistent memory MDL.
            //

            Mdl = &MmMdlPersistentMemory;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_PERSISTENT,
                                            MDL_FLAGS_VIRTUAL_ADDRESS);
        }

        if (Descriptor == NULL) {

            //
            // Try once more, searching the mapped / unallocated MDL.
            //

            Mdl = &MmMdlMappedUnallocated;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_MAPPED_UNALLOCATED,
                                            MDL_FLAGS_VIRTUAL_ADDRESS);
        }

        if (Descriptor == NULL) {

            //
            // Try once more, searching the reserved list.
            //

            Mdl = &MmMdlReservedAllocated;
            Descriptor = MmMdFindDescriptor(Page,
                                            MEMORYMAP_TYPE_RESERVED_ALLOCATED,
                                            MDL_FLAGS_VIRTUAL_ADDRESS);
        }

        if (Descriptor != NULL) {

            //
            // Remove the memory descriptor from the mapped MDL so it can be
            // added to an unmapped memory descriptor list.
            //

            MmMdRemoveDescriptorFromList(Mdl, Descriptor);

            //
            // If needed, create a memory descriptor for any memory described
            // by the descriptor that was not unmapped.  These region(s)
            // need to remain in the original list.
            //
            // The mapped region can either split a descriptor at the start
            // or end of the region described by Descriptor.
            //

            if (Descriptor->MappedBasePage < Page) {
                SplitDescriptor = MmMdInitDescriptor(
                                          Descriptor->BasePage,
                                          Descriptor->MappedBasePage,
                                          Page - Descriptor->MappedBasePage,
                                          Descriptor->Attributes,
                                          Descriptor->MemoryType);

                ASSERT(SplitDescriptor != NULL);

                MmMdAddDescriptorToList(Mdl, SplitDescriptor, MDL_FLAGS_NONE);

                //
                // Continue to use Descriptor to describe the newly
                // unmapped region.  Update its values to illustrate the
                // omission above.
                //

                Diff = Page - Descriptor->MappedBasePage;
                Descriptor->BasePage += Diff;
                Descriptor->MappedBasePage += Diff;
                Descriptor->PageCount -= Diff;
            }

            if (EndPage < (Descriptor->MappedBasePage +
                           Descriptor->PageCount)) {

                Diff = EndPage - Page;
                SplitDescriptor = MmMdInitDescriptor(
                                      Descriptor->BasePage + Diff,
                                      EndPage,
                                      Descriptor->PageCount - Diff,
                                      Descriptor->Attributes,
                                      Descriptor->MemoryType);

                ASSERT(SplitDescriptor != NULL);

                MmMdAddDescriptorToList(Mdl, SplitDescriptor, MDL_FLAGS_NONE);

                //
                // Continue to use Descriptor to describe the newly
                // unmapped region.  Update its values to illustrate the
                // omission above.
                //

                Descriptor->PageCount = Diff;
            }

            //
            // Add the unmapped region to the proper unmapped descriptor list.
            // Entries on the unmapped allocated list represent individual
            // allocations from the firmware and thus should not be coalesced.
            // Coalescing is appropriate for the unmapped unallocated list
            // since it tracks the runs of physical memory present in the
            // machine rather than single allocations.  It is important to
            // coalesce the unmapped unallocated list to avoid excessive
            // fragmentation in cases where a large number of platform memory
            // areas are mapped and unmapped during boot application
            // execution (e.g., scanning the PCI bus using memory mapped
            // configuration space).
            //

            Descriptor->MappedBasePage = 0;
            ReinsertionFlags = MDL_FLAGS_NONE;
            if ((Mdl != &MmMdlReservedAllocated) &&
                (Mdl != &MmMdlPersistentMemory)) {

                if (Mdl == &MmMdlMappedAllocated) {
                    Mdl = &MmMdlUnmappedAllocated;

                } else {
                    Mdl = &MmMdlUnmappedUnallocated;
                    ReinsertionFlags = MDL_FLAGS_COALESCE;
                }
            }

            MmMdAddDescriptorToList(Mdl, Descriptor, ReinsertionFlags);
            Page += Descriptor->PageCount;

        } else {

            //
            // If we were unable to find the mapped region, skip the page.
            //

            Page += 1;
        }

    } while (Page < EndPage);

BlMmUnmapVirtualAddressExEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
BlMmRemapVirtualAddress (
    __in_bcount(Size) PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG Attributes
    )

/*++

Routine Description:

    This routine will remap the specified virtual memory range to the given
    physical memory range, destroying any current mappings.

Arguments:

    VirtualAddress - Supplies a pointer to a virtual address signifying the base
        of the virtual memory region to be remapped.

    PhysicalAddress - Supplies a physical address signifying the base of the
        physical memory region to be mapped.

    Size - Supplies the number of bytes to be virtually remapped.

    Attributes - Supplies the attributes to use when performing the remapping.
        These include the cache attributes.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG AllocationPolicy;
    ULONG CacheAttributes;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTranslate);

    //
    // If the application is currently running without its own page tables, do
    // not edit mappings.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Assert virtual address is not on the list of free virtual addresses.
    //

    ASSERT(MmMdFindDescriptorFromMdl(&MmMdlFreeVirtual,
                                     ((ULONGLONG)VirtualAddress >> PAGE_SHIFT),
                                     MDL_FLAGS_VIRTUAL_ADDRESS) == NULL);

    //
    // Parameter checking.  Ensure that an output parameter exists and that
    // the caller requested something to be mapped.
    //

    if ((VirtualAddress == NULL) || (Size == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto MmRemapVirtualAddressEnd;
    }

    if (PhysicalAddress.QuadPart == MM_PFN_NO_MAP) {
        Status = STATUS_INVALID_PARAMETER;
        goto MmRemapVirtualAddressEnd;
    }

    //
    // Remap doesn't allow use of any allocation policies.
    //

    AllocationPolicy = Attributes & (MEMORY_ATTRIBUTE_ALLOCATION_KSEG |
                                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (AllocationPolicy != MEMORY_ATTRIBUTE_ALLOCATION_INDIFFERENT) {
        Status = STATUS_INVALID_PARAMETER;
        goto MmRemapVirtualAddressEnd;
    }

    //
    // Cache attributes are determined by the attributes as well.  Make sure
    // that only one cache attribute is supplied.
    //

    CacheAttributes = Attributes & MEMORY_ATTRIBUTE_CACHE_MASK;
    if ((CacheAttributes & (CacheAttributes - 1)) != 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto MmRemapVirtualAddressEnd;
    }

    //
    // Convert a DMA cache attribute to the "none" attribute.  All currently
    // supported platforms guarantee DMA cache coherency, so any attribute
    // can be used for allocations designed for DMA.
    //

    if (CacheAttributes == MEMORY_ATTRIBUTE_CACHE_DMA) {
        CacheAttributes = MEMORY_ATTRIBUTE_CACHE_INDIFFERENT;
    }

    Status = MmRemapVirtualAddress(VirtualAddress,
                                   PhysicalAddress,
                                   Size,
                                   CacheAttributes);

    if (!NT_SUCCESS(Status)) {
        goto MmRemapVirtualAddressEnd;
    }

    //
    // There are no changes to make to the global MDLs if translation is not
    // enabled.  In this case, exit now.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        goto MmRemapVirtualAddressEnd;
    }

    //
    // Delete the old mapping tracker for this virtual address. The virtual
    // address is no longer associated with the physical address to which it was
    // previously mapped. Additionally, since a virtual address can only ever be
    // associated with one physical address at a time, it does not make sense to
    // have two entries for one virtual address in the mapping tracker MDL.
    //

    TrpDeleteMappingTracker(VirtualAddress, Size);

    //
    // Add the new mapping to the tracker list. This will ensure that the
    // mapping gets cleaned up during library destruction.
    //

    Status = TrpGenerateMappingTracker(VirtualAddress,
                                       PhysicalAddress,
                                       Size,
                                       Attributes);

    if (!NT_SUCCESS(Status)) {
        goto MmRemapVirtualAddressEnd;
    }

MmRemapVirtualAddressEnd:
    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
BlMmSetPageProtection (
    _In_reads_bytes_(Size) PVOID VirtualAddress,
    _In_ ULONGLONG Size,
    _In_ ULONG Protection,
    _In_ ULONG Flags
    )
/*++

Routine Description:

    This routine sets the specified virtual address range to the desired
    protection. This assumes that the virtual addresses are backed by PTEs which
    can be set (i.e. not large pages).

    N.B. All pages are assumed to allow read access.

Arguments:

    VirtualAddress - Supplies the base of the virtual address range to be
        modified.

    Size - Supplies the number of bytes in the virtual address range.

    Protection - Supplies the protection to be applied to the address range.
        Supported flags are:

        MM_PROTECTION_WRITE - Allow write access to the virtual address range.

        MM_PROTECTION_EXECUTE - Allow execute access to the virtual address
            range.

    Flags - Supplies flag defining the behavior of this routine. Supported flags
        are:

        MEMORY_PROTECTION_OR - OR the new protection attributes with the
            existing protection.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTranslate);

    NT_ASSERT(BlpArchQueryCurrentContextType() != ExecutionContextFirmware);

    NT_ASSERT(BlMmIsTranslationEnabled() != FALSE);

    NT_ASSERT(((ULONG_PTR)VirtualAddress % PAGE_SIZE) == 0);

    Status = MmArchSetPageProtection(VirtualAddress, Size, Protection, Flags);

    if (MmArchShouldFlushTlbAfterMappingAddress() != FALSE) {
        MmArchFlushTlb();
    }

    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
BlMmAllocateVirtualPages (
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) != 0,
           __deref_inout_bcount(Pages * PAGE_SIZE))
    _When_((Attributes & MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS) == 0,
           __deref_out_bcount(Pages * PAGE_SIZE))
         PVOID *VirtualAddress,
    __in ULONGLONG Pages,
    __in ULONG Attributes,
    __in ULONG Alignment
    )

/*++

Routine Description:

    This routine reserves a block of contiguous virtual memory addresses which
    satisfy the caller's size, attribute, and alignment requirements.

Arguments:

    Address - Supplies the preferred address for the allocation.  A preferred
        address is a virtual address. As an input parameter, this is ignored
        unless the attributes indicate a fixed address is required. Also
        receives the address allocated.

    Pages - Supplies the number of pages to allocate.

    Attributes - Supplies the attributes required of the virtual memory region
        to be allocated.

    Alignment - Supplies the required alignment of the allocation. All
        allocations will be page aligned (indicated by alignment 0 or 1).  Any
        other alignment is viewed as the page alignment that is required by the
        caller.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if VirtualAddress is NULL.

    STATUS_NO_MEMORY if the allocation fails.

    STATUS_NOT_SUPPORTED if a specified allocation attribute is not supported.

--*/

{

    MEMORY_DESCRIPTOR Allocation;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTranslate);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Validate Parameters.
    //

    if ((ARGUMENT_PRESENT(VirtualAddress) == FALSE) || (Pages == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlMmAllocateVirtualPagesEnd;
    }

    //
    // If the boot application is running without virtual address translation,
    // the request is not supported.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        Status = STATUS_NOT_SUPPORTED;
        goto BlMmAllocateVirtualPagesEnd;
    }

    //
    // When no alignment request is made, page align the allocation.
    //

    if (Alignment == 0) {
        Alignment = 1;
    }

    //
    // As virtual address allocations are only ever library allocations, do not
    // allow firmware services attributes.
    //

    if (CHECK_FLAG(Attributes, FIRMWARE_SERVICE_MEMORY_ATTRIBUTES)) {
        Status = STATUS_NOT_SUPPORTED;
        goto BlMmAllocateVirtualPagesEnd;
    }

    //
    // There are three types of virtual address allocations allowed: fixed,
    // KSEG, and anywhere.
    //
    // - Fixed searches the free virtual descriptor list for a specified range.
    // - KSEG searches the free virtual descriptor list starting at the KSEG
    //     base and moves upwards.
    // - The anywhere mappings start just below KSEG and progress downward.
    //

    INITIALIZE_UNUSED_RANGE(&AllocationRequest.MappedRange);
    if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {
        INITIALIZE_ADDRESS_RANGE(&AllocationRequest.BaseRange,
                                 (UINTN)*VirtualAddress >> PAGE_SHIFT,
                                 Pages);

    } else if (CHECK_FLAG(Attributes, MEMORY_ATTRIBUTE_ALLOCATION_KSEG)) {
        AllocationRequest.BaseRange.Minimum =
            MmArchKsegAddressRange.Minimum >> PAGE_SHIFT;

        AllocationRequest.BaseRange.Maximum =
            MmArchKsegAddressRange.Maximum >> PAGE_SHIFT;

        AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;

// ARM64_64KPAGE_WORKAROUND: hack for 64KB alignment
#if defined(_ARM64_)
        Alignment = ALIGN_RANGE_UP(Alignment, _64KB / PAGE_SIZE);
#endif

    } else {
        AllocationRequest.BaseRange.Minimum =
            MmArchBaseOfApplicationAddressSpace >> PAGE_SHIFT;

        AllocationRequest.BaseRange.Maximum =
            MmArchTopOfApplicationAddressSpace >> PAGE_SHIFT;

        AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_HIGH_ADDRESSES;
    }

    //
    // Search the free virtual descriptor list for the desired virtual address
    // range.
    //

    AllocationRequest.PageCount = Pages;
    AllocationRequest.Attributes = Attributes;
    AllocationRequest.Alignment = Alignment;
    Status = MmPaAllocatePages(NULL,
                               &Allocation,
                               &MmMdlFreeVirtual,
                               &AllocationRequest,
                               MEMORY_TYPE_FREE);

    if (!NT_SUCCESS(Status)) {
        goto BlMmAllocateVirtualPagesEnd;
    }

    //
    // Return the address of the virtual allocation to the caller.
    //

    if (NT_SUCCESS(Status)) {
        *VirtualAddress = (PVOID)(ULONG_PTR)(Allocation.BasePage << PAGE_SHIFT);
    }

BlMmAllocateVirtualPagesEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
BlMmFreeVirtualPages (
    __in PVOID VirtualAddress,
    __in ULONGLONG Pages
    )

/*++

Routine Description:

    This routine releases a page-aligned range of virtual addresses beginning at
    the virtual address specified.

Arguments:

    VirtualAddress - Supplies the base of the virtual address range to be
        returned to the list of free virtual addresses.

    Pages - Supplies the number of virtual pages in the virtual address range
        that is to be release back tot he list of free virtual addresses.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONGLONG BasePage;
    PMEMORY_DESCRIPTOR Descriptor;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTranslate);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Sanitize input parameters.
    //

    if ((VirtualAddress == NULL) || (Pages == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlMmFreeVirtualPagesEnd;
    }

    //
    // If the boot application is running without virtual address translation,
    // the request is not supported.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        Status = STATUS_NOT_SUPPORTED;
        goto BlMmFreeVirtualPagesEnd;
    }

    BasePage = ((ULONG_PTR)VirtualAddress) >> PAGE_SHIFT;
    Descriptor = MmMdInitDescriptor(BasePage,
                                    0,
                                    Pages,
                                    MEMORY_ATTRIBUTE_NONE,
                                    MEMORY_TYPE_FREE);

    if (Descriptor == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlMmFreeVirtualPagesEnd;
    }

    Status = MmMdAddDescriptorToList(&MmMdlFreeVirtual,
                                     Descriptor,
                                     MDL_FLAGS_COALESCE | MDL_FLAGS_TRUNCATE);

BlMmFreeVirtualPagesEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfaceTranslate);
    return Status;
}

NTSTATUS
MmSelectMappingAddress (
    __out PVOID *SelectedAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in PVOID PreferredAddress,
    __in ULONGLONG Size,
    __in ULONG MappingPolicy,
    __in ULONG Attributes
    )

/*++

Routine Description:

    This routine selects the address used to map the given physical address
    for a system using page tables.

Arguments:

    SelectedAddress - Supplies a pointer to a variable that receives the
        selected address for the mapping.

    PhysicalAddress - Supplies the physical address being mapped.

    PreferredAddress - Supplies the preferred address to use for the mapping.
        This is only valid if the mapping policy indicates that a preferred
        address is to be used.

    Size - Supplies the size in bytes of the mapping.

    MappingPolicy - Supplies a policy describing how the mapping address should
        be chosen.

    Attributes - Supplies the attributes to use when creating the mapping.
        These include cache attributes and VA selection policy.

Return Value:

    STATUS_SUCCESS.

--*/

{

    MEMORY_DESCRIPTOR Allocation;
    PAGE_ALLOCATION_REQUEST AllocationRequest;
    ULONGLONG FinalAddress;
    ULONGLONG PageStart;
    ULONGLONG RangeMaximum;
    ULONGLONG RangeMinimum;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Attributes);

    Status = STATUS_SUCCESS;
    FinalAddress = 0;

    //
    // Parameter checking.  This routine only executes when address translation
    // is in use.  Mapping addresses for physical mode have architecture-specifc
    // requirements that are not comprehended here.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        FinalAddress = PhysicalAddress.QuadPart;
        goto SelectPageTableMappingAddressEnd;
    }

    //
    // Choose a virtual address based on the caller's allocation policy.
    //

    if (((PhysicalAddress.QuadPart == MM_PFN_NO_MAP) && (PreferredAddress != NULL)) ||
        CHECK_FLAG(MappingPolicy,
                   MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS)) {

        //
        // Use the caller's fixed address if it is provided.
        //

        FinalAddress = (ULONG_PTR)PreferredAddress;

    } else {

        //
        // Allocate KSEG addresses bottoms up, starting at KSEG0_BASE.
        //
        // All other allocations are top-down starting from the address space
        // just below KSEG. KSEG addresses are not in the allowable range: this
        // used to be in order to avoid the old KSEG mapping policy where
        // (VA = PA | KSEG0_BASE), but now it prevents interleaving of KSEG and
        // non-KSEG mappings. Low addresses are avoided if possible to avoid
        // conflicts with 1-1 mappings.
        //

        if (CHECK_FLAG(MappingPolicy,
                       MEMORY_ATTRIBUTE_ALLOCATION_KSEG)) {

            RangeMinimum = MmArchKsegAddressRange.Minimum >> PAGE_SHIFT;
            RangeMaximum = MmArchKsegAddressRange.Maximum >> PAGE_SHIFT;
            AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;

        } else {

            //
            // The pages being mapped may already be covered by any number of
            // other virtual mappings. Existing mappings cannot be reused. For
            // example, say a single page virtual mapping V covers page P. Once
            // this mapping has been established, additional virtual mapping
            // requests can be satisfied using the virtual addresses immediately
            // preceding and following V. If in the future a second, larger
            // mapping is requested starting at P, V cannot be reused since a
            // larger extent would overlap the adjacent mappings that have been
            // established in the meantime.
            //
            // To prevent such conflicts, new virtual addresses are allocated
            // for every mapping request not carrying an explicit allocation
            // policy. The physical page MDLs are unable to represent this
            // multiple mapping. Using them alone, many virtual mappings would
            // be unreferenced by the physical MDLs and would only be reflected
            // through allocated VA blocks in the free virtual MDL.  In the
            // interest of precisely tracking these mappings and more
            // efficiently releasing them during library destruction, every
            // successful mapping is explicitly tracked in a global virtual
            // mapping tracker MDL.
            //

            RangeMinimum = MmArchBaseOfApplicationAddressSpace >> PAGE_SHIFT;
            RangeMaximum = MmArchTopOfApplicationAddressSpace >> PAGE_SHIFT;
            AllocationRequest.RangeFlags = MEMORY_RANGE_PREFER_HIGH_ADDRESSES;
        }

        //
        // Set allocation properties common to both KSEG and non-KSEG mappings.
        //

        INITIALIZE_UNUSED_RANGE(&AllocationRequest.MappedRange);
        AllocationRequest.BaseRange.Minimum = RangeMinimum;
        AllocationRequest.BaseRange.Maximum = RangeMaximum;

        //
        // Set the requested allocation attributes.
        //

        AllocationRequest.Attributes = MEMORY_ATTRIBUTE_NONE;
        AllocationRequest.Attributes |=
            (Attributes & MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE);

        AllocationRequest.Alignment = 1;
// ARM64_64KPAGE_WORKAROUND: hack for 64KB alignment
#if defined(_ARM64_)
        AllocationRequest.Alignment = ALIGN_RANGE_UP(AllocationRequest.Alignment, _64KB / PAGE_SIZE);
#endif
        AllocationRequest.PageCount =
            ADDRESS_AND_SIZE_TO_SPAN_PAGES(PhysicalAddress.QuadPart, Size);

        //
        // Search for an available mapping on the free virtual memory descriptor
        // list.
        //

        Status = MmPaAllocatePages(NULL,
                                   &Allocation,
                                   &MmMdlFreeVirtual,
                                   &AllocationRequest,
                                   MEMORY_TYPE_FREE);

        if (!NT_SUCCESS(Status)) {
            goto SelectPageTableMappingAddressEnd;
        }

        //
        // Set the selected address to be in the page chosen by the page
        // allocator.  Offset the selected address into the page the same
        // amount as the physical address is offset into the physical page.
        //

        PageStart = ALIGN_RANGE_DOWN(PhysicalAddress.QuadPart, PAGE_SIZE);
        FinalAddress = Allocation.BasePage << PAGE_SHIFT;
        if (PhysicalAddress.QuadPart != MM_PFN_NO_MAP) {
            FinalAddress += (PhysicalAddress.QuadPart - PageStart);
        }
    }

SelectPageTableMappingAddressEnd:
    if (NT_SUCCESS(Status)) {
        *SelectedAddress = (PVOID)(ULONG_PTR)FinalAddress;
    }

    return Status;
}

// -------------------------------------------------- Library Internal Routines

NTSTATUS
MmTrInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes the translation routines implemented by this
    module.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if initialization succeeds.

    An appropriate error status code if the free virtual address MDL could not
    be initialized.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    PLIST_ENTRY NextEntry;
    ULONGLONG PageCount;
    PMEMORY_DESCRIPTOR ReservedDescriptor;
    NTSTATUS Status;

#if defined(_ARM_) || defined(_ARM64_)

    PMEMORY_DESCRIPTOR MemoryDescriptor;

#endif

    Descriptor = NULL;

    //
    // This routine initializes MDLs for tracking free virtual addresses and
    // virtual address mappings.  This is not required if executing in
    // physical mode.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        Status = STATUS_SUCCESS;
        goto TrInitializeEnd;
    }

    //
    // Initialize the virtual mapping tracker MDL.
    //

    MmMdInitializeList(&MmMdlMappingTrackers, MdlTypeByteGranular, NULL);

    //
    // Initialize the free virtual MDL.
    //

    MmMdInitializeList(&MmMdlFreeVirtual, MdlTypeVirtual, NULL);

    //
    // Add a single descriptor to the list covering all of address space since
    // all virtual address space is free to start.
    //

    PageCount = ((ULONG_PTR)-1 >> PAGE_SHIFT) + 1;
    Descriptor = MmMdInitDescriptor(0,
                                    0,
                                    PageCount,
                                    MEMORY_ATTRIBUTE_NONE,
                                    MEMORY_TYPE_FREE);

    if (Descriptor == NULL) {
        Status = STATUS_NO_MEMORY;
        goto TrInitializeEnd;
    }

    Status = MmMdAddDescriptorToList(&MmMdlFreeVirtual,
                                     Descriptor,
                                     MDL_FLAGS_COALESCE);

    if (!NT_SUCCESS(Status)) {
        goto TrInitializeEnd;
    }

    //
    // Iterate through the reserved MDL, removing any virtual pages described
    // there from the free virtual address space.  This insures that VA space
    // used by a previous application is not trampled by this application.
    //

    NextEntry = MmMdlReservedAllocated.Head->Flink;
    while (NextEntry != MmMdlReservedAllocated.Head) {
        ReservedDescriptor = CONTAINING_RECORD(NextEntry,
                                               MEMORY_DESCRIPTOR,
                                               ListEntry);

        if (ReservedDescriptor->MappedBasePage != 0) {
            Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                             ReservedDescriptor->MappedBasePage,
                                             ReservedDescriptor->PageCount,
                                             MDL_FLAGS_VIRTUAL_ADDRESS);

            if (!NT_SUCCESS(Status)) {
                goto TrInitializeEnd;
            }

#if defined(_ARM_) || defined(_ARM64_)

        } else {
            Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                             ReservedDescriptor->BasePage,
                                             ReservedDescriptor->PageCount,
                                             MDL_FLAGS_VIRTUAL_ADDRESS);

            if (!NT_SUCCESS(Status)) {
                goto TrInitializeEnd;
            }

#endif

        }

        NextEntry = NextEntry->Flink;
    }

#if defined(_ARM_) || defined(_ARM64_)

    //
    // Also iterate through the list of firmware allocations, removing the 1:1
    // VAs used for those mappings by the parent.
    //

    NextEntry = MmMdlUnmappedUnallocated.Head->Flink;
    while (NextEntry != MmMdlUnmappedUnallocated.Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                               MEMORY_DESCRIPTOR,
                                               ListEntry);

        if ((MemoryDescriptor->MemoryType == MEMORY_TYPE_BOOT_SERVICE_CODE) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_RUNTIME_SERVICE_CODE) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_RUNTIME_SERVICE_DATA) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_RAMDISK) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_BOOT_APPLICATION)) {

            Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                             MemoryDescriptor->BasePage,
                                             MemoryDescriptor->PageCount,
                                             MDL_FLAGS_VIRTUAL_ADDRESS);

            if (!NT_SUCCESS(Status)) {
                goto TrInitializeEnd;
            }
        }

        NextEntry = NextEntry->Flink;
    }

#endif

    Status = STATUS_SUCCESS;

TrInitializeEnd:
    if (!NT_SUCCESS(Status)) {

        //
        // On error zero the global descriptor so that it is not migrated to
        // heap without being on an MDL.  This would orphan the resulting heap.
        //

        if (Descriptor != NULL) {
            RtlZeroMemory(Descriptor, sizeof(MEMORY_DESCRIPTOR));
        }
    }

    return Status;
}

NTSTATUS
MmTrDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys the state kept by the virtual address translation
    routines in this module.

    N.B. The caller must migrate the contents of all global MDLs into the
         global descriptor array before calling this routine.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PLIST_ENTRY Current;
    PMEMORY_DESCRIPTOR Tracker;

    //
    // Free all virtual address mappings described in the tracker list.  The
    // trackers themselves have been migrated to the global descriptor array
    // by the caller, ensuring that they will not be affected by the removal
    // of these mappings.
    //
    // N.B. Since the trackers have been migrated to static global
    //      descriptors, no heap free or list entry unlink operations are
    //      required.
    //

    Current = MmMdlMappingTrackers.Head->Flink;
    while (Current != MmMdlMappingTrackers.Head) {
        Tracker = CONTAINING_RECORD(Current, MEMORY_DESCRIPTOR, ListEntry);

        //
        // Unmapping the VA range will result in the static memory descriptor
        // being zeroed out (thereby clearing out the Flink). Hence the Current
        // value must be updated before unmapping operation.
        //

        Current = Current->Flink;
        BlMmUnmapVirtualAddress(
            (PVOID)(ULONG_PTR)Tracker->ByteGranular.MappedBase,
            Tracker->ByteGranular.Size);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
TrpGenerateMappingTracker (
    __in PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONG64 Size,
    __in ULONG Attributes
    )

/*++

Routine Description:

    This routine generates a mapping tracker for the given mapping and inserts
    it in the global tracker list.

Arguments:

    VirtualAddress - Supplies the virtual address of the mapping.

    PhysicalAddress - Supplies the physical address mapped by the given
        virtual address.

    Size - Supplies the size in bytes of the mapping.

    Attributes - Supplies the mapping attributes used during construction of
        the given mapping.

Return Value:

    STATUS_SUCCESS is always returned since the global descriptor allocator
        used to construct new trackers cannot in general fail.

--*/

{

    PLIST_ENTRY Current;
    PMEMORY_DESCRIPTOR NewTracker;
    PMEMORY_DESCRIPTOR Tracker;

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // The given mapping may have been generated in an attempt to build a new
    // heap, making direct heap allocation unsafe.  The library provides
    // machinery that dispenses temporary statically allocated memory
    // descriptors that are dynamically converted into heap allocations when
    // the broadest enclosing memory descriptor operation completes.  Use this
    // machinery to allocate and initialize a memory descriptor to track this
    // mapping.
    //

    NewTracker = MmMdInitByteGranularDescriptor(
                     (ULONG64)PhysicalAddress.QuadPart,
                     (ULONG64)(ULONG_PTR)VirtualAddress,
                     Size,
                     Attributes,
                     0);

    //
    // Insert the new tracker into the list.  The list is sorted in ascending
    // order by starting virtual address.
    //

    Current = MmMdlMappingTrackers.Head->Flink;
    while (Current != MmMdlMappingTrackers.Head) {
        Tracker = CONTAINING_RECORD(Current, MEMORY_DESCRIPTOR, ListEntry);
        if (Tracker->ByteGranular.MappedBase <
            NewTracker->ByteGranular.MappedBase) {

            Current = Current->Flink;
            continue;
        }

        //
        // The starting address of the new tracker is less than or equal to
        // the current descriptor.  Insert the new tracker before the current
        // descriptor.
        //

        InsertTailList(&Tracker->ListEntry, &NewTracker->ListEntry);
        goto TrackerInserted;
    }

    //
    // The virtual address of this mapping exceeds all virtual addresses
    // currently recorded in the list.  Insert the new tracker at the end of
    // the list.
    //

    InsertTailList(MmMdlMappingTrackers.Head, &NewTracker->ListEntry);

TrackerInserted:
    MmLeaveDescriptorLibraryApi();
    return STATUS_SUCCESS;
}

VOID
TrpDeleteMappingTracker (
    __in PVOID VirtualAddress,
    __in ULONG64 Size
    )

/*++

Routine Description:

    This function removes the given virtual address mapping from the global
    mapping tracker MDL.

Arguments:

    VirtualAddress - Supplies the virtual address of the start of the range
        being deleted.

    Size - Supplies the size in bytes of the range being deleted.

Return Value:

    None.

--*/

{

    PLIST_ENTRY Current;
    PMEMORY_DESCRIPTOR Tracker;
    ULONG64 Va;

    //
    // Search for the given virtual mapping in the tracker MDL and remove it
    // when found.  The tracker MDL is sorted in ascending order by virtual
    // address.
    //

    Va = (ULONG64)(ULONG_PTR)VirtualAddress;
    Current = MmMdlMappingTrackers.Head->Flink;
    while (Current != MmMdlMappingTrackers.Head) {
        Tracker = CONTAINING_RECORD(Current, MEMORY_DESCRIPTOR, ListEntry);
        if ((Tracker->ByteGranular.MappedBase == Va) &&
            (Tracker->ByteGranular.Size == Size)) {

            RemoveEntryList(&Tracker->ListEntry);
            MmMdFreeDescriptor(Tracker);
            return;
        }

        if (Tracker->ByteGranular.MappedBase > Va) {
            goto TrackerNotFound;
        }

        Current = Current->Flink;
    }

    //
    // Note that equivalence between the dimensions of generated mappings and
    // the parameters used by callers to unmap these ranges can be enforced by
    // adding an unconditional assertion here.
    //

TrackerNotFound:
    return;
}

NTSTATUS
MmMapPhysicalAddress (
    __inout PPHYSICAL_ADDRESS PA,
    __deref_inout_bcount(*Size) PVOID *VA,
    __inout PULONGLONG Size,
    __in ULONG Attributes
    )

/*++

Routine Description:

    This routine will attempt to create a virtual address mapping to the
    specified physical address.  If VA's pointer has a value, it is viewed
    as the preferred linear address and this routine will attempt to map
    that linear address to the specified physical address.

Arguments:

    PA - Supplies a pointer to the start of the physical memory region to map.
        Additionally, receives a pointer of the start of the physical region
        that was mapped.

        If PA == MM_PFN_NO_MAP, then no physical address will be mapped,
        but the page tables will be allocated for the virtual mapping.

    VA - Supplies a pointer to the value of the start of the virtual memory
        region to map to the specified physical memory region.  Additonally,
        receives a pointer to the virtual address region mapped.

    Size - Supplies a pointer to the size (in bytes) of the physical memory
        region to map.  Additionally, receives the actual number of bytes
        mapped.

    Attributes - Supplies the attributes to be used for the mapping.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if memory can not be allocated for a page table page
        required for the mapping.

    STATUS_NOT_FOUND if the a virtual address range can not be found to map
        the specified physical address range.

    STATUS_INVALID_PARAMETER if the address range specified in VA is not
        available.

    STATUS_INVALID_PARAMETER if VA is NULL or the value stored in VA is not
        aligned on a page boundary.

--*/

{

    ULONGLONG AlignedMappingSize;
    PHYSICAL_ADDRESS CheckAddress;
    ULONG CheckAttributes;
    BOOLEAN Compatible;
    PVOID EndAddress;
    ULONGLONG MappingSize;
    BOOLEAN PfnNoMap;
    PHYSICAL_ADDRESS PhysicalAddress;
    PHYSICAL_ADDRESS PhysicalStartAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;
    PVOID VirtualStartAddress;

    //
    // Simple parameter checking.
    //

    if ((PA == NULL) ||
        (VA == NULL) ||
        (Size == NULL) ||
        (*Size > MI_MAX_MAPPED_REGION_SIZE)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is currently running without its own page tables, do
    // not create mappings.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        return STATUS_SUCCESS;
    }

    //
    // The application has virtual address translation enabled. If it is running
    // in the firmware's context, then explicitly fail, as no attempts to create
    // mappings while in the firmware's context should be made.
    //

    if (BlpArchQueryCurrentContextType() == ExecutionContextFirmware) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // If the specified physical address == MM_PFN_NO_MAP, then the required
    // paging structures to map the virtual address will be allocated.
    // However, the page table entry will not be initialized.
    //

    VirtualStartAddress = (PVOID)(ULONG_PTR)*VA;
    PhysicalStartAddress.QuadPart = PA->QuadPart;
    MappingSize = *Size;
    if (PA->QuadPart == MM_PFN_NO_MAP) {
        PfnNoMap = TRUE;

    } else {
        PfnNoMap = FALSE;

        //
        // Expand the address range to map according to page granularity.
        //

        PhysicalStartAddress.QuadPart =
            ALIGN_RANGE_DOWN(PhysicalStartAddress.QuadPart, PAGE_SIZE);

        VirtualStartAddress =
            (PVOID)(ULONG_PTR)ALIGN_RANGE_DOWN((ULONG_PTR)VirtualStartAddress,
                                               PAGE_SIZE);

        MappingSize += (PA->QuadPart - PhysicalStartAddress.QuadPart);
        AlignedMappingSize = ALIGN_RANGE_UP(MappingSize, PAGE_SIZE);
        if (AlignedMappingSize < MappingSize) {
            return STATUS_INTEGER_OVERFLOW;
        }

        MappingSize = AlignedMappingSize;
    }

    //
    // Ensure that the entire virtual address range is either completely
    // unmapped or contains the requested mapping.
    //

    if (PfnNoMap == FALSE) {
        VirtualAddress = VirtualStartAddress;
        PhysicalAddress = PhysicalStartAddress;

        //
        // Avoid wrapping.
        //

        EndAddress = Add2Ptr(VirtualAddress, MappingSize - 1);
        while (VirtualAddress < EndAddress) {
            if (MmArchTranslateVirtualAddress(VirtualAddress,
                                              &CheckAddress,
                                              &CheckAttributes) != FALSE) {

                if (PhysicalAddress.QuadPart != CheckAddress.QuadPart) {
                    return STATUS_INVALID_PARAMETER;
                }

                //
                // Verify that the desired mapping is compatible with the
                // existing one. Let the architecture make the determination
                // and simply check that the desired value is compatible.
                //

                Status = TrpMapCacheAttributesAreCompatible(CheckAttributes,
                                                            Attributes,
                                                            &Compatible);

                if (!NT_SUCCESS(Status)) {
                    return Status;
                }

                if (Compatible == FALSE) {
                    return STATUS_INVALID_PARAMETER;
                }
            }

            VirtualAddress = Add2Ptr(VirtualAddress, PAGE_SIZE);
            PhysicalAddress.QuadPart += PAGE_SIZE;
        }
    }

    //
    // Create the requested virtual mapping.
    //

    Status = MmArchMapPhysicalAddress(PhysicalStartAddress,
                                      VirtualStartAddress,
                                      MappingSize,
                                      Attributes);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set return arguments.
    //

    PA->QuadPart = PhysicalStartAddress.QuadPart;
    *VA = VirtualStartAddress;
    *Size = MappingSize;

    //
    // If the TLB should be flushed after mapping an address, flush it.
    //
    // TODO: Consider removing this TLB flush completely, since the map
    //       routines above are only changing page table entries from invalid
    //       (and guaranteed to be flushed) to valid.
    //

    if (MmArchShouldFlushTlbAfterMappingAddress() != FALSE) {
        MmArchFlushTlb();
    }

    return STATUS_SUCCESS;
}

NTSTATUS
MmRemapVirtualAddress (
    __in_bcount(Size) PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG CacheAttributes
    )

/*++

Routine Description:

    This routine will attempt to remap the specified virtual address
    mapping to the specified physical range.

Arguments:

    VirtualAddress - Supplies the virtual address specifying the start of the
        range that will be remapped.

    PhysicalAddress - Supplies the physical address specifying the start of the
        physical range to be mapped.

    Size - Supplies the number of bytes to be remapped.

    CacheAttributes - Supplies the cachattributes the remappings will maintain.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONGLONG AlignedSize;
    PHYSICAL_ADDRESS PhysicalStartAddress;
    NTSTATUS Status;
    PVOID VirtualStartAddress;

    //
    // Simple parameter checking.
    //

    if ((VirtualAddress == NULL) || (Size == 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // The application has virtual address translation enabled. If it is running
    // in the firmware's context, then explicitly fail, as no attempts to create
    // mappings while in the firmware's context should be made.
    //

    ASSERT(BlMmQueryTranslationType() != VA_TRANSLATION_TYPE_NONE);

    if (BlpArchQueryCurrentContextType() == ExecutionContextFirmware) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Disallow use of MM_PFN_NO_MAP through this API.
    //

    if (PhysicalAddress.QuadPart == MM_PFN_NO_MAP) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Expand the address range to remap according to page granularity.
    //

    PhysicalStartAddress.QuadPart =
        ALIGN_RANGE_DOWN(PhysicalAddress.QuadPart, PAGE_SIZE);

    VirtualStartAddress =
        (PVOID)(ULONG_PTR)ALIGN_RANGE_DOWN((ULONG_PTR)VirtualAddress,
                                           PAGE_SIZE);

    Size += (PhysicalAddress.QuadPart - PhysicalStartAddress.QuadPart);
    AlignedSize = ALIGN_RANGE_UP(Size, PAGE_SIZE);
    if (AlignedSize < Size) {
        return STATUS_INTEGER_OVERFLOW;
    }

    //
    // Create the requested virtual mapping.
    //

    Status = MmArchRemapVirtualAddress(VirtualStartAddress,
                                       PhysicalStartAddress,
                                       AlignedSize,
                                       CacheAttributes);

    return Status;
}

NTSTATUS
MmUnmapVirtualAddress (
    __deref_inout_bcount(*Size) PVOID *VirtualAddress,
    __inout PULONGLONG Size
    )

/*++

Routine Description:

    This routine will attempt to invalidate the specified virtual address
    mapping.

Arguments:

    VA - On input, contains the value of the start of the virtual memory
         region to unmap.  On output, contains the start of the region actually
         unmapped.  All operations must have page granularity.  if the
         specified region does not fall on page boundaries, this routine will
         unmap the next largest page aligned region.

    Size - On input, the size (in bytes) of the virtually mapped region to
         unmap.  On output, contains the actual size (in bytes) of the
         unmapped region.

Return Value:

    STATUS_SUCCESS always.

    STATUS_SUCCESS if VirtualAddress or Size are NULL.

--*/

{

    ULONGLONG AlignedSize;
    NTSTATUS Status;
    PVOID VirtualStartAddress;

    //
    // Simple parameter checking.
    //

    if ((VirtualAddress == NULL) ||
        (Size == NULL) ||
        (*Size > MI_MAX_MAPPED_REGION_SIZE)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is currently running without its own page tables, do
    // not unmap the virtual address.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {
        return STATUS_SUCCESS;
    }

    //
    // The application has virtual address translation enabled. If it is running
    // in the firmware's context, then explicitly fail, as no attempts to unmap
    // virtual addresses while in the firmware's context should be made.
    //

    if (BlpArchQueryCurrentContextType() == ExecutionContextFirmware) {
        return STATUS_UNSUCCESSFUL;
    }

    VirtualStartAddress =
        (PVOID)(ULONG_PTR)ALIGN_RANGE_DOWN((ULONG_PTR)*VirtualAddress,
                                           PAGE_SIZE);

    //
    // Round off Size to contain all pages contained in specified memory
    // range.  (Round down start page and round up end page).
    //

    *Size += PtrOffset(VirtualStartAddress, *VirtualAddress);
    AlignedSize = ALIGN_RANGE_UP(*Size, PAGE_SIZE);
    if (AlignedSize < *Size) {
        return STATUS_INTEGER_OVERFLOW;
    }

    *Size = AlignedSize;

    //
    // Unmap every page in the specified virtual address range.
    //

    Status = MmArchUnmapVirtualAddress(VirtualStartAddress, *Size);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set return arguments.
    //

    *VirtualAddress = VirtualStartAddress;

    //
    // If the TLB should be flushed after unmapping addresses, flush the TLB.
    //
    // TODO: Consider removing the following TLB flush in favor of single
    //       entry flushes in the unmap routine above.
    //

    if (MmArchShouldFlushTlbAfterUnmappingAddress() != FALSE) {
        MmArchFlushTlb();
    }

    return STATUS_SUCCESS;
}

NTSTATUS
TrpMapCacheAttributesAreCompatible (
    __in ULONG CurrentAttributes,
    __in ULONG DesiredAttributes,
    __out PBOOLEAN Compatible
    )
{

    NTSTATUS Status;

#if MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS

    MEMORY_CACHE_ATTRIBUTES_RELATIONSHIP AttributesRelationship;

#endif  // MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS

    Status = STATUS_SUCCESS;
    *Compatible = TRUE;

#if MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS

    Status = BlArchCheckCacheAttributesCompatibility(CurrentAttributes,
                                                     DesiredAttributes,
                                                     &AttributesRelationship,
                                                     RecommendedAttributes);

    if (NT_SUCCESS(Status)) {
        *Compatible =
            MEMORY_CACHE_ATTRIBUTES_ARE_COMPATIBLE(AttributesRelationship);
    }

#else   // MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS

    UNREFERENCED_PARAMETER(CurrentAttributes);
    UNREFERENCED_PARAMETER(DesiredAttributes);

#endif  // MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS

    return Status;
}

