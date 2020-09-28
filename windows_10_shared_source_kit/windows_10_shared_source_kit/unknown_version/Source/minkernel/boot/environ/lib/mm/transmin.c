/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    transmin.c

Abstract:

    Implementation of the Boot Environment Library's memory management APIs
    for address translation.

    This is a thin layer used for applications that run without address
    translation enabled.

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
    PMEMORY_DESCRIPTOR Descriptor;

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);

    if ((VirtualAddress == NULL) || (PhysicalAddress == NULL)) {
        ReturnValue = FALSE;
        goto TranslateVirtualAddressEnd;
    }

    //
    // Ensure that the physical page exists in the memory map.
    //

    Descriptor = MmMdFindDescriptor((ULONG_PTR)VirtualAddress >> PAGE_SHIFT,
                                    MEMORYMAP_TYPE_ALL,
                                    MDL_FLAGS_PHYSICAL_ADDRESS);

    if (Descriptor == NULL) {
        ReturnValue = FALSE;
        goto TranslateVirtualAddressEnd;
    }

    PhysicalAddress->QuadPart = (ULONG_PTR)VirtualAddress;
    ReturnValue = TRUE;

TranslateVirtualAddressEnd:
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

    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Attributes);
    UNREFERENCED_PARAMETER(Flags);

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);
    ASSERT(ARGUMENT_PRESENT(VirtualAddress) != FALSE);
    ASSERT(PhysicalAddress.QuadPart != MM_PFN_NO_MAP);

    if (Size == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto MapPhysicalAddressExEnd;
    }

    *VirtualAddress = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
    Status = STATUS_SUCCESS;

MapPhysicalAddressExEnd:
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

    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Flags);

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);

    BlpInterfaceExit(InterfaceTranslate);
    return STATUS_SUCCESS;
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

    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Attributes);

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);

    BlpInterfaceExit(InterfaceTranslate);
    return STATUS_NOT_SUPPORTED;
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

    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Pages);
    UNREFERENCED_PARAMETER(Attributes);
    UNREFERENCED_PARAMETER(Alignment);

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);

    BlpInterfaceExit(InterfaceTranslate);
    return STATUS_NOT_SUPPORTED;
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

    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Pages);

    BlpInterfaceEnter(InterfaceTranslate);

    ASSERT(BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE);

    BlpInterfaceExit(InterfaceTranslate);
    return STATUS_NOT_SUPPORTED;
}

