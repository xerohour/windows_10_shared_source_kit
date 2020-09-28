/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    memutils.c

Abstract:

    Implementation of the Boot Environment Library's memory management APIs
    for the initialization and destruction of the library's memory manager.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "mm.h"

// -------------------------------------------------------------------- Globals

VA_TRANSLATION_TYPE MmTranslationType = VA_TRANSLATION_TYPE_MAX;

#if defined(BOOTENV_MEMORY_CHECK)

#define MC_DEFAULT_VALIDATE_COUNT 100
ULONG McpCriticalInterfaceCount = 0;
ULONG McpInterfaceCount = 0;
ULONG McpValidateCount = MC_DEFAULT_VALIDATE_COUNT;

#endif

// ----------------------------------------------------------------- Prototypes

NTSTATUS
MmpFreeApplicationAllocations (
    VOID
    );

NTSTATUS
MmpFreeLibraryAllocations (
    VOID
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
BlpMmInitialize (
    __in PALLOCATED_MEMORY AllocatedMemory,
    __in VA_TRANSLATION_TYPE CurrentTranslationType,
    __in PLIBRARY_PARAMETERS Parameters
    )

/*++

Routine Description:

    Initializes the Boot Environment Library's memory subsystem.  This
    process includes the initialization of the page and heap allocators
    as well as performing any steps required to enable virtual address
    translation.

Arguments:

    AllocatedMemory - Supplies the list of memory currently allocated by
        previous boot applications.

    CurrentTranslationType - Supplies the address translation mode of the
        calling boot application.

    Parameters - Supplies application-supplied parameters for configuring the
        boot library.  This contains tuning values for the page allocator and
        heap allocator as well as the type of virtual address translation
        the boot application should use.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation made for initialization failed.

    STATUS_INVALID_PARAMETER if there is an error creating the allocated memory
        descriptor list based on the list the caller provided or if
        current translation type is invalid.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMm);

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.  First initialize
    // the state that this function uses, and then call it.
    //

    MmDescriptorCallTreeCount = 0;
    MmEnterDescriptorLibraryApi();

    //
    // N.B.  If the previous boot application was executing with virtual
    //       address translation enabled, then the input parameter were
    //       mapped and could be referenced.  If not, then the current boot
    //       application is not currently executing with address translation
    //       enabled and the input parameters can still be referenced.
    //

    if ((VA_TRANSLATION_TYPE_MAX <= CurrentTranslationType) ||
        (VA_TRANSLATION_TYPE_MAX <= Parameters->TranslationType)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlMmInitializeEnd;
    }

    //
    // Initialize global static memory descriptors.
    //

    MmMdInitialize(0, Parameters);

    //
    // Perform any required architecture initialization before either
    // the heap or page allocator are initialized.
    //

    Status = MmArchInitialize(0,
                              AllocatedMemory,
                              CurrentTranslationType,
                              Parameters->TranslationType);

    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeEnd;
    }

    //
    // Initialize the Page Allocator.
    //

    Status = MmPaInitialize(AllocatedMemory, Parameters->MinimumPageAllocation);
    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializePaInitFailure;
    }

    //
    // Remove any bad memory from the system memory map.
    //
    // N.B. If the bad page list is fragmented, or large, removing bad
    //      memory from the system can cause a large spike in memory
    //      descriptor usage.  Therefore it's best to do this after switching
    //      to dynamic descriptors, although it does result in a
    //      *slightly* increased probability of boot allocations conflicting
    //      with bad memory.  But this situation is unavoidable since the
    //      firmware might have already loaded the boot manager into bad
    //      memory to begin with.
    //

    BlMmRemoveBadMemory();

    //
    // Initialize the structures for tracking virtual address usage.
    //

#if !defined(MM_MIN)

    Status = MmTrInitialize();
    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeTrInitFailure;
    }

#endif

    //
    // Turn on virtual address translation.  This routine will additionally
    // map the boot application one to one.
    //
    // N.B. This must be done after the page allocator has been
    //      initialized since physical pages may need to be allocated for
    //      platform paging structures.
    //
    // During Phase 1, MmArchInitialize will virtually map regions from
    // the previously allocated memory regions, some of which may be deferred
    // until phase 2.
    //

    Status = MmArchInitialize(1,
                              AllocatedMemory,
                              CurrentTranslationType,
                              Parameters->TranslationType);

    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeArch1InitFailure;
    }

    //
    // Save the type of address translation in a global variable.  This
    // is useful for determining if we can forward any MM API requests
    // based on translation type.  This is commonly used by the Page Allocator.
    // For example, a BlMmAllocatePages request will be forwarded to
    // BlMmAllocatePhysicalPages if address translation is not enabled.
    //

    MmTranslationType = Parameters->TranslationType;

    //
    // Initialize the Heap Allocator.
    //

    Status = MmHaInitialize(Parameters->MinimumHeapSize,
                            Parameters->HeapAllocationAttributes);

    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeHaInitFailure;
    }

    //
    // Initialize dynamic memory descriptors if needed by the application.
    //

    MmMdInitialize(1, Parameters);

     //
    // During Phase 2, virtual mappings for any regions that were deferred
    // will be established. The virtual mapping may be deferred for some regions
    // due to small amount of fixed memory descriptors available prior to
    // dynamic memory descriptors being initialized.
    //

    Status = MmArchInitialize(2,
                              AllocatedMemory,
                              CurrentTranslationType,
                              Parameters->TranslationType);

    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeArch2InitFailure;
    }

    //
    // Initialize the block memory allocator.
    //

    Status = MmBaInitialize();
    if (!NT_SUCCESS(Status)) {
        goto BlMmInitializeBaInitFailure;
    }

    goto BlMmInitializeEnd;

BlMmInitializeBaInitFailure:
BlMmInitializeArch2InitFailure:
    MmMdDestroy();
    MmHaDestroy();
BlMmInitializeHaInitFailure:
BlMmInitializeArch1InitFailure:
    MmPaDestroy(0);

#if !defined(MM_MIN)

    MmTrDestroy();
BlMmInitializeTrInitFailure:

#endif

    MmArchDestroy();
    MmPaDestroy(1);
BlMmInitializePaInitFailure:

    //
    // Free the global memory descriptors consumed during initialization.
    //

BlMmInitializeEnd:
    MmLeaveDescriptorLibraryApi();
    BlpInterfaceExit(InterfaceMm);
    return Status;
}

NTSTATUS
BlpMmDestroy (
    __in ULONG Phase
    )

/*++

Routine Description:

    Destruction routine for the Boot Environment Library's memory management
    subsystem.  All initialization and allocations made by the library
    are undone.

Arguments:

    Phase - Supplies the destruction phase.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if descriptors could not be copied to the global scratch
        array.

    Other failure status codes possible depending on firmware implementation
    of destroy routine.

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMm);
    ReturnStatus = STATUS_SUCCESS;

    //
    // In the first phase of library destruction, free all application
    // allocations.  During the second phase of destruction, destroy library
    // allocations and restore the memory context of the calling application.
    //

    switch (Phase) {
    case 0:

#if !defined(NTKERNEL)

        Status = MmpFreeApplicationAllocations();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

#endif !defined(NTKERNEL)

        break;

    case 1:

        //
        // Destroy the block memory allocator.
        //

        Status = MmBaDestroy();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Free any outstanding library allocations that do not have the
        // library late free attribute set.
        //

#if !defined(NTKERNEL)

        Status = MmpFreeLibraryAllocations();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

#endif !defined(NTKERNEL)

        //
        // Perform dynamic descriptor buffer destruction if required.
        // N.B. This must be done before the heap is destroyed.
        //

        Status = MmMdDestroy();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Free the heap allocator.  It is not important to free the actual
        // pages allocated for the heap since the page allocator will reclaim
        // them when destroying itself.  However, it is important that the heap
        // allocator update it's internal status.
        //

        Status = MmHaDestroy();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Free all virtual memory allocations.
        //

        Status = MmPaDestroy(0);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Release the database of active virtual mappings tracked by the
        // translation subcomponent.  The database includes virtual mappings for
        // the page allocator allocations freed above.
        //

#if !defined(MM_MIN)

        Status = MmTrDestroy();
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

#endif

        //
        // Restore the memory context of the calling application.
        //

        Status = MmArchDestroy();
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Free all physical memory allocations, including the page tables.
        //

        Status = MmPaDestroy(1);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        break;

    default:

        ASSERT(FALSE);

        ReturnStatus = STATUS_INVALID_PARAMETER;
    }

    BlpInterfaceExit(InterfaceMm);
    return ReturnStatus;
}

ULONG_PTR
BlMmGetKsegBase (
    VOID
    )

/*++

Routine Description:

    Returns the base address of kernel address space.

Arguments:

    None.

Return Value:

    When address translation is disabled, zero is returned.

    When address translation is enabled, the following is returned:

    KSEG0_BASE on AMD64.

    KSEG0_BASE or ALTERNATE_BASE on X86, depending on the presence of /3GB in
        the boot application's options.

--*/

{

    BlpInterfaceEnter(InterfaceMm);
    BlpInterfaceExit(InterfaceMm);
    return MmArchKsegBase;
}

ULONG_PTR
BlMmGetKsegBias (
    VOID
    )

/*++

Routine Description:

    Returns the ASLR bias to the base address of kernel address space.

Arguments:

    None.

Return Value:

    When address translation is disabled, zero is returned.
    When address translation is enabled, the ASLR KSEG0 base bias is returned.

--*/

{
    BlpInterfaceEnter(InterfaceMm);
    BlpInterfaceExit(InterfaceMm);
    return MmArchKsegBias;
}

MEMORY_TYPE
MmFwpGetOsMemoryType (
    __in EFI_MEMORY_TYPE EfiMemoryType,
    __in ULONGLONG EfiMemoryAttributes
    )

/*++

Routine Description:

    Maps an EFI Memory Type to an OS defined memory type.

Arguments:

    EfiMemoryType - Efi memory type to convert.

    EfiMemoryAttributes - Efi memory attribute for the memory type.

Return Value:

    OS defined memory type.

--*/

{

    MEMORY_TYPE MemoryType;

    UNREFERENCED_PARAMETER(EfiMemoryAttributes);

    switch (EfiMemoryType) {
    case EfiReservedMemoryType:
        MemoryType = MEMORY_TYPE_PLATFORM_RESERVED;
        break;

    case EfiLoaderCode:
    case EfiLoaderData:
        MemoryType = MEMORY_TYPE_BOOT_APPLICATION;
        break;

    case EfiBootServicesCode:
    case EfiBootServicesData:
        MemoryType = MEMORY_TYPE_BOOT_SERVICE_CODE;
        break;

    case EfiRuntimeServicesCode:
        MemoryType = MEMORY_TYPE_RUNTIME_SERVICE_CODE;
        break;

    case EfiRuntimeServicesData:
        MemoryType = MEMORY_TYPE_RUNTIME_SERVICE_DATA;
        break;

    case EfiConventionalMemory:
        MemoryType = MEMORY_TYPE_FREE;
        break;

    case EfiUnusableMemory:
        MemoryType = MEMORY_TYPE_BAD;
        break;

    case EfiACPIReclaimMemory:
        MemoryType = MEMORY_TYPE_ACPI_TABLES;
        break;

    case EfiACPIMemoryNVS:
        MemoryType = MEMORY_TYPE_ACPI;
        break;

    case EfiMemoryMappedIO:
        MemoryType = MEMORY_TYPE_MMIO_SPACE;
        break;

    case EfiMemoryMappedIOPortSpace:
        MemoryType = MEMORY_TYPE_MMIO_PORT_SPACE;
        break;

    case EfiPalCode:
        MemoryType = MEMORY_TYPE_PAL_CODE;
        break;

    default:
        MemoryType = MEMORY_TYPE_PLATFORM_RESERVED;
        break;
    }

    return MemoryType;
}

// ------------------------------------------------------------------ Debug API

NTSTATUS
MmValidate (
    VOID
    )

/*++

Routine Description:

    Attempts to validate both the Page Allocator and the Heap Allocator
    against internal data structure inconsistencies.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if successful or if validation is not possible in the
        current operating mode.
    STATUS_UNSUCCESSFUL if any inconsistencies are found in internal data
        structures for either the page allocator or the heap allocator.

--*/

{

    NTSTATUS Status;

    //
    // When operating with address translation enabled, page allocator and
    // heap allocator structures are generally not accessible during periods
    // when address translation is off.  This implies that we must exit
    // immediately without performing any validation if the current address
    // translation state does not match the one used when initializing the
    // memory manager.
    //

    if ((BlMmQueryTranslationType() != VA_TRANSLATION_TYPE_NONE) &&
        (BlMmIsTranslationEnabled() == FALSE)) {

        Status = STATUS_SUCCESS;
        goto MmValidateEnd;
    }

    //
    // Hand off to the validation routines.
    //

    Status = MmPaValidate();
    if (!NT_SUCCESS(Status)) {
        goto MmValidateEnd;
    }

    Status = MmHaValidate();
    if (!NT_SUCCESS(Status)) {
        goto MmValidateEnd;
    }

MmValidateEnd:

    return Status;
}

VOID
BlpMcCriticalInterfaceEnter (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    This routine will disable validation of memory manager internal data
    structures.  This routine is called on the entrance to a time sensitive or
    critical interface.

Arguments:

    Name - Supplies the API or API class which is starting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_MEMORY_CHECK)

    McpCriticalInterfaceCount += 1;

#endif

    return;
}

VOID
BlpMcCriticalInterfaceExit (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    If there are no more outstanding critical regions, this routine will
    reenable the validation of memory manager internal data structures.  This
    routine is called on the exit of a time sensitive or critical interface.

Arguments:

    Name - Supplies the API or API class which is exiting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_MEMORY_CHECK)

    ASSERT(McpCriticalInterfaceCount != 0);

    McpCriticalInterfaceCount -= 1;

#endif

    return;
}

VOID
BlpMcInterfaceEnter (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Validates the internal data structures for the Boot Environment Library's
    memory manager.

    This routine is called on the entrance to a Boot Environment Library API or
    API class.

Arguments:

    Name - Supplies the API or API class which is starting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_MEMORY_CHECK)

    if (McpCriticalInterfaceCount == 0) {
        McpInterfaceCount += 1;
        if (McpInterfaceCount >= McpValidateCount) {
            MmValidate();
            McpInterfaceCount = 0;
        }
    }

#endif

    return;
}

VOID
BlpMcInterfaceExit (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Validates the internal data structures for the Boot Environment Library's
    memory manager.

    This routine is called on the exit of a Boot Environment Library API or
    API class.

Arguments:

    Name - Supplies the API or API class which is exiting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_MEMORY_CHECK)

    if (McpCriticalInterfaceCount == 0) {
        McpInterfaceCount += 1;
        if (McpInterfaceCount >= McpValidateCount) {
            MmValidate();
            McpInterfaceCount = 0;
        }
    }

#endif

    return;
}


// --------------------------------------------------------- Internal Functions

NTSTATUS
MmpFreeApplicationAllocations (
    VOID
    )

/*++

Routine Description:

    This routine will free memory allocated by a boot application.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    LIST_ENTRY AllocatedMemoryMap;
    ULONG AllocationFlags;
    GENERIC_BUFFER DescriptorBuffer;
    PMEMORY_DESCRIPTOR MemoryRange;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    UNPACKED_MEMORY_TYPE UnpackedMemoryType;
    PVOID VirtualAddress;

    ReturnStatus = STATUS_SUCCESS;

    //
    // Obtain a memory map of all allocated memory ranges.
    //

    AllocationFlags =
        MEMORYMAP_TYPE_MAPPED_ALLOCATED |
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&AllocatedMemoryMap,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Walk the allocated memory map in search of allocations made by the boot
    // application.  For each application allocation, free the the memory
    // range.
    //

    for (NextEntry = AllocatedMemoryMap.Flink;
         NextEntry != &AllocatedMemoryMap;
         NextEntry = NextEntry->Flink) {

        MemoryRange = CONTAINING_RECORD(NextEntry,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

        UnpackedMemoryType.MemoryType = MemoryRange->MemoryType;
        if ((UnpackedMemoryType.Class != MEMORY_CLASS_APPLICATION) ||
            (UnpackedMemoryType.SubType <
             MEMORY_APPLICATION_TYPE_DEFINABLE_START)) {

            continue;
        }

        if (MemoryRange->MappedBasePage != 0) {
            VirtualAddress = PAGE_TO_ADDRESS(MemoryRange->MappedBasePage);
            Status = BlMmFreePages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart =
                PAGE_TO_PHYSICAL_ADDRESS(MemoryRange->BasePage);

            Status = BlMmFreePhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    BlMmFreeHeap(DescriptorBuffer.Buffer);
    return ReturnStatus;
}

NTSTATUS
MmpFreeLibraryAllocations (
    VOID
    )

/*++

Routine Description:

    This routine will free memory allocated by the boot library.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    LIST_ENTRY AllocatedMemoryMap;
    ULONG AllocationFlags;
    ULONG AvoidLibraryFreeAttributes;
    GENERIC_BUFFER DescriptorBuffer;
    PMEMORY_DESCRIPTOR MemoryRange;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    UNPACKED_MEMORY_TYPE UnpackedMemoryType;
    PVOID VirtualAddress;

    ReturnStatus = STATUS_SUCCESS;

    //
    // During page allocator destruction the free page APIs will not zero
    // memory, therefore, since one of the goals of this routine is to make
    // sure that any outstanding library allocations are not left containing
    // sensitive data, insist that the page allocator is not in destruction.
    //

    ASSERT(MmPaModuleInitializationStatus() != ModuleInDestruction);

    //
    // Obtain a memory map of all allocated memory ranges.
    //

    AllocationFlags =
        MEMORYMAP_TYPE_MAPPED_ALLOCATED |
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&AllocatedMemoryMap,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Walk the allocated memory map in search of allocations made by the boot
    // library.  For each library allocation, free the the memory range.
    //

    for (NextEntry = AllocatedMemoryMap.Flink;
         NextEntry != &AllocatedMemoryMap;
         NextEntry = NextEntry->Flink) {

        MemoryRange = CONTAINING_RECORD(NextEntry,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

        //
        // Don't free library allocations that have the library only free
        // or free last attributes as this could result in persistent data
        // getting zero'd, or prevent a clean shutdown of the library.
        //

        AvoidLibraryFreeAttributes =
            MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_FREE_LAST |
            MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;

        UnpackedMemoryType.MemoryType = MemoryRange->MemoryType;
        if ((UnpackedMemoryType.Class != MEMORY_CLASS_LIBRARY) ||
            ((MemoryRange->Attributes & AvoidLibraryFreeAttributes) != 0)) {

            continue;
        }

        if (MemoryRange->MappedBasePage != 0) {
            VirtualAddress = PAGE_TO_ADDRESS(MemoryRange->MappedBasePage);
            Status = BlMmFreePages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart =
                PAGE_TO_PHYSICAL_ADDRESS(MemoryRange->BasePage);

            Status = BlMmFreePhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    BlMmFreeHeap(DescriptorBuffer.Buffer);
    return ReturnStatus;
}

