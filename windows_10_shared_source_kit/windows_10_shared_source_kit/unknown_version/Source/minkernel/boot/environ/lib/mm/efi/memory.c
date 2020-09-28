/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    memory.c

Abstract:

    This module contains EFI specific memory routines.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "mm.h"
#include "systemevents.h"

// ----------------------------------------------- Internal Function Prototypes

ULONG
MmFwpGetOsAttributeType (
    __in UINT64 EfiAttributeType
    );

MEMORY_TYPE
MmFwpGetOsMemoryType (
    __in EFI_MEMORY_TYPE EfiMemoryType,
    __in ULONGLONG EfiMemoryAttributes
    );

VOID
MmFwpWriteBadMemoryEvent(
    __in PMEMORY_DESCRIPTOR FwDescriptor,
    __in PMEMORY_DESCRIPTOR BlDescriptor
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
MmFwGetMemoryMap (
    __out PMEMORY_DESCRIPTOR_LIST MemoryMap,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine will build a memory map that reflects the current EFI view
    of system memory.

    The routine will query EFI for the system memory map and then convert
    the firmware output to an understood internal format, a system memory
    descriptor list.  The descriptors for the list are initialized from a
    global scratch buffer.

Arguments:

    MemoryMap - Supplies a pointer to an initialized linked list node. On
        output, contains the resultant memory map list.

    Flags - Supplies a set of flags for the memory map creation.

Return Value:

    STATUS_SUCCESS if successful.
    STATUS_INVALID_PARAMETER if MemoryMap is NULL.
    STATUS_INSUFFICIENT_RESOURCES if there are an insufficient number of
        global descriptors to build the requested OS memory map.

--*/

{

    BOOLEAN AlwaysUseFirmwareAllocator;
    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    BOOLEAN BootDeviceIsRamDisk;
    PVOID BootLibBufferBase;
    EXECUTION_CONTEXT_TYPE ContextType;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_PHYSICAL_ADDRESS EfiBufferBase;
    ULONGLONG EfiEndPage;
    EFI_MEMORY_DESCRIPTOR *EfiMemoryMap;
    UINTN EfiMemoryMapSize;
    EFI_MEMORY_DESCRIPTOR EfiMm;
    ULONGLONG EfiStartPage;
    EFI_STATUS EfiStatus;
    PMEMORY_DESCRIPTOR FreeDescriptor;
    LIBRARY_PARAMETERS LibraryParameters;
    UINTN MapKey;
    ULONG MdlFlags;
    MEMORY_TYPE MemoryType;
    MEMORY_TYPE Below1MbMemoryType;
    PMEMORY_DESCRIPTOR OsDescriptor;
    ULONGLONG OsPageCount;
    ULONGLONG OsStartPage;
    UINTN Pages;
    ULONGLONG RamDiskBase;
    ULONGLONG RamDiskEfiEndPage;
    ULONGLONG RamDiskEfiStartPage;
    ULONGLONG RamDiskSize;
    NTSTATUS Status;

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Initialize local variables.
    //

    BootLibBufferBase = NULL;
    ContextType = ExecutionContextApplication;
    EfiBufferBase = (EFI_PHYSICAL_ADDRESS)0;
    Pages = 0;

    //
    // Set the firmware allocator boolean based on the input flags.
    //

    if (CHECK_FLAG(Flags, MM_MEMORYMAP_FLAGS_USE_FIRMWARE_ALLOCATOR) != FALSE) {
        AlwaysUseFirmwareAllocator = TRUE;

    } else {
        AlwaysUseFirmwareAllocator = FALSE;
    }

    //
    // Always use the firmware based allocator if the library has not yet been
    // initialized.
    //

    LibraryParameters = BlGetCurrentLibraryParameters();
    if (CHECK_FLAG(LibraryParameters.Flags, LF_INITIALIZED) == FALSE) {
        AlwaysUseFirmwareAllocator = TRUE;
    }

    //
    // Parameter checking.
    //

    if (MemoryMap == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto FwGetMemoryMapEnd;
    }

    //
    // Build the memory map from scratch.  If there are any entries in the
    // list, remove them.
    //

    MmMdFreeList(MemoryMap);

    //
    // Get the size of the buffer needed for EFI memory map.
    //

    EfiMemoryMapSize = 0;
    Status = EfiGetMemoryMap(&EfiMemoryMapSize,
                             NULL,
                             &MapKey,
                             &DescriptorSize,
                             &DescriptorVersion);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        if (NT_SUCCESS(Status)) {
            Status = STATUS_UNSUCCESSFUL;
        }

        goto FwGetMemoryMapEnd;
    }

    //
    // Add space for a few more memory descriptors in case the EFI's
    // memory map changes between the time of initial EfiGetMemoryMap
    // call and the subsequent EfiGetMemoryMap call.  If using the firmware
    // allocator, then ensure the resulting buffer is aligned to an OS page
    // boundary of a length which is and an even multiple of an OS page to
    // avoid truncation problems when fixing up the resulting descriptor list.
    //

    EfiMemoryMapSize += (4 * DescriptorSize);
    if (AlwaysUseFirmwareAllocator != FALSE) {

        //
        // Since boot library is not initialized yet, allocate a buffer
        // from EFI for EFI memory map.
        //

        Pages = ALIGN_RANGE_UP(EfiMemoryMapSize, PAGE_SIZE) >> PAGE_SHIFT;

        //
        // Add an extra page leaving room to allocate a smaller OS page-aligned
        // region.
        //

        Pages++;
        Status = EfiAllocatePages(AllocateAnyPages,
                                  EfiLoaderData,
                                  (UINTN)GET_EFIPAGE(Pages),
                                  &EfiBufferBase);

        if (!NT_SUCCESS(Status)) {
            goto FwGetMemoryMapEnd;
        }

        Status = EfiFreePages(EfiBufferBase, (UINTN)GET_EFIPAGE(Pages));
        if (!NT_SUCCESS(Status)) {
            EfiBufferBase = (EFI_PHYSICAL_ADDRESS)0;
            goto FwGetMemoryMapEnd;
        }

        //
        // Align EfiBufferBase forward to an OS page boundary.  Remove the
        // extra page reserving space for this purpose.  Allocate this
        // OS page aligned region from the firmware.
        //

        EfiBufferBase = ALIGN_RANGE_UP(EfiBufferBase, PAGE_SIZE);
        Pages--;
        Status = EfiAllocatePages(AllocateAddress,
                                  EfiLoaderData,
                                  (UINTN)GET_EFIPAGE(Pages),
                                  &EfiBufferBase);

        if (!NT_SUCCESS(Status)) {
            EfiBufferBase = (EFI_PHYSICAL_ADDRESS)0;
            goto FwGetMemoryMapEnd;
        }

        //
        // Get the memory map buffer address and size.
        //

        EfiMemoryMapSize = Pages * PAGE_SIZE;
        EfiMemoryMap = (EFI_MEMORY_DESCRIPTOR *)(ULONG_PTR)EfiBufferBase;

        //
        // Query EFI for the firmware memory map. Call EFI boot service
        // routine GetMemoryMap as opposed to boot library function
        // EfiGetMemoryMap as EfiMemoryMap is not mapped.
        //
        // N.B. Since the stack is guaranteed to be identity mapped,
        //      there is no need to translate other parameters of
        //      GetMemoryMap routine.
        //

        //
        // If the application is running within the application execution
        // context, switch it to firmware execution context for making firmware
        // the call.
        //

        ContextType = BlpArchQueryCurrentContextType();
        if (ContextType != ExecutionContextFirmware) {
            BlpArchSwitchContext(ExecutionContextFirmware);
        }

        EfiStatus = EfiBS->GetMemoryMap(&EfiMemoryMapSize,
                                        EfiMemoryMap,
                                        &MapKey,
                                        &DescriptorSize,
                                        &DescriptorVersion);

        //
        // Restore original execution context.
        //

        if (ContextType != ExecutionContextFirmware) {
            BlpArchSwitchContext(ContextType);
        }

        Status = EfiGetNtStatusCode(EfiStatus);

    } else {

        //
        // Since boot library is already initialized, allocate a buffer
        // from boot library for EFI memory map.
        //

        Pages = ALIGN_RANGE_UP(EfiMemoryMapSize, PAGE_SIZE) >> PAGE_SHIFT;
        Status = BlMmAllocatePages(&BootLibBufferBase,
                                   Pages,
                                   MEMORY_TYPE_MISC_DATA,
                                   MEMORY_ATTRIBUTE_NONE,
                                   0);

        if (!NT_SUCCESS(Status)) {
            goto FwGetMemoryMapEnd;
        }

        //
        // Get the memory map buffer address and size.
        //

        EfiMemoryMapSize = Pages * PAGE_SIZE;
        EfiMemoryMap = (EFI_MEMORY_DESCRIPTOR *)BootLibBufferBase;

        //
        // Query EFI for the firmware memory map.
        //

        Status = EfiGetMemoryMap(&EfiMemoryMapSize,
                                 EfiMemoryMap,
                                 &MapKey,
                                 &DescriptorSize,
                                 &DescriptorVersion);
    }

    if (!NT_SUCCESS(Status)) {
        goto FwGetMemoryMapEnd;
    }

    //
    // Validate that the arguments returned make sense.
    //

    if (((EfiMemoryMapSize % DescriptorSize) != 0) ||
        (DescriptorSize < sizeof(EFI_MEMORY_DESCRIPTOR))) {

        Status = STATUS_UNSUCCESSFUL;
        goto FwGetMemoryMapEnd;
    }

    BootDevice = BlGetBootDevice();
    if ((BootDevice->DeviceType == DEVICE_BLOCK_IO) &&
        (BootDevice->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK)) {

        BootDeviceIsRamDisk = TRUE;
        RamDiskBase = BootDevice->u.BlockIo.u.RamDisk.ImageBase.QuadPart;
        RamDiskSize = BootDevice->u.BlockIo.u.RamDisk.ImageSize;
        RamDiskEfiStartPage = GET_EFIPAGE(RamDiskBase >> PAGE_SHIFT);
        RamDiskEfiEndPage = RamDiskEfiStartPage +
            COMPUTE_EFI_PAGES_SPANNED(RamDiskBase, RamDiskSize);

    } else {
        RamDiskEfiStartPage = RamDiskEfiEndPage = 0;
        BootDeviceIsRamDisk = FALSE;
    }


    //
    // Build an internal memory map based on the firmware output.
    //
    // Walk the list as a linear block of memory so to allow the code to
    // work if we are out of sync with a newer firmware version and it's
    // memory descriptor size.
    //

    for (; (EfiMemoryMapSize != 0);
         EfiMemoryMapSize -= DescriptorSize,
         EfiMemoryMap = Add2Ptr(EfiMemoryMap, DescriptorSize)) {

        //
        // Copy one EFI memory descriptor from the memory map buffer for
        // easy access. This is only necessary when EfiMemoryMap is not
        // mapped, but it is done here regardless to make code less divergent,
        // at the price of copying the entire memory map buffer.
        //

        if ((AlwaysUseFirmwareAllocator != FALSE) &&
            (ContextType != ExecutionContextFirmware)) {

            BlpArchSwitchContext(ExecutionContextFirmware);
        }

        EfiMm = *EfiMemoryMap;

        //
        // Restore original execution context.
        //

        if ((AlwaysUseFirmwareAllocator != FALSE) &&
            (ContextType != ExecutionContextFirmware)) {

            BlpArchSwitchContext(ContextType);
        }

        MemoryType = MmFwpGetOsMemoryType(EfiMm.Type, EfiMm.Attribute);

        //
        // For each memory region, always align the start and end pages to an
        // OS page boundary.  Since the firmware could use a smaller page
        // granularity than the OS, it is possible that some firmware
        // descriptors will be truncated while others expanded.  Allow the
        // more intelligent code in MmMdAddDescriptorToList to handle this
        // case.
        //
        // For partial descriptors (where start or size is non-page aligned)
        // describing available regions, use a more restrictive mapping, i.e,
        // treat the entire page as non-available. For other types of partial
        // descriptors, expand the descriptor to span the entire page.
        //
        //
        // N.B. The end pages are calculated to be the next page after the
        //      region described by the descriptor. Such that the end page
        //      minus the start page equals the page count. In most cases the
        //      end page will be the first page of the next descriptor.
        //

        if ((MemoryType == MEMORY_TYPE_FREE) ||
            (MemoryType == MEMORY_TYPE_FREE_ZEROED)) {

            EfiStartPage = ALIGN_RANGE_UP(EfiMm.PhysicalStart, EFI_PAGE_SIZE);
            EfiStartPage >>= EFI_PAGE_SHIFT;
            EfiEndPage = ROUND_DOWN(EfiStartPage + EfiMm.NumberOfPages,
                                    GET_EFIPAGE(1));

        } else {
            EfiStartPage = EfiMm.PhysicalStart >> EFI_PAGE_SHIFT;
            EfiEndPage = ROUND_UP(EfiStartPage + EfiMm.NumberOfPages,
                                  GET_EFIPAGE(1));
        }

        //
        // Skip over any partial firmware descriptors that become zero in
        // size due to shrinking.
        //

        if (EfiStartPage == EfiEndPage) {
            continue;
        }

        //
        // For free pages below 1MB, create a MDL with special attribute so that
        // normal page allocation will not use pages from below 1MB.
        //

        if (EfiStartPage < (_1MB >> EFI_PAGE_SHIFT)) {
            Below1MbMemoryType = MemoryType;

            //
            // If the region begins at base page zero, create a descriptor for
            // the first physical page of memory. This page needs to be marked
            // as firmware permanent so the OS does not reclaim it. Until UEFI +
            // CSM platforms are no longer available, this work around will need
            // to exist.
            //

            if (EfiStartPage == 0) {
                OsDescriptor = MmMdInitDescriptor(
                    GET_OSPAGE(EfiStartPage),
                    0,
                    1,
                    MmFwpGetOsAttributeType(EfiMm.Attribute),
                    MEMORY_TYPE_PLATFORM_RESERVED);

                if (OsDescriptor == NULL) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                Status = MmMdAddDescriptorToList(MemoryMap,
                                                 OsDescriptor,
                                                 MDL_FLAGS_TRUNCATE);

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                EfiStartPage = 1;

                //
                // If the entire firmware descriptor was the first page,
                // advanced to the next firmware descriptor, otherwise fall
                // through and create the other boot library descriptors for
                // this firmware descriptor.
                //

                if (EfiStartPage == EfiEndPage) {
                    continue;
                }
            }

            //
            // If the region ends above 1MB, create two descriptors: one for
            // the portion below 1MB, and one for the portion above 1MB.
            // entirely below 1MB. For the descriptor with memory region below
            // 1MB, mark its memory type using Below1MbMemoryType. For the
            // descriptor with memory region above 1MB, mark its memory type
            // as reported by the firmware.
            //

            if (EfiEndPage > (_1MB >> EFI_PAGE_SHIFT)) {
                OsDescriptor = MmMdInitDescriptor(
                    GET_OSPAGE(EfiStartPage),
                    0,
                    (_1MB >> PAGE_SHIFT) - GET_OSPAGE(EfiStartPage),
                    MmFwpGetOsAttributeType(EfiMm.Attribute),
                    Below1MbMemoryType);

                if (OsDescriptor == NULL) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                //
                // Add a special attribute to the free pages below 1MB.
                //

                if ((OsDescriptor->MemoryType == MEMORY_TYPE_FREE) ||
                    (OsDescriptor->MemoryType == MEMORY_TYPE_FREE_ZEROED)) {

                    OsDescriptor->Attributes |=
                        MEMORY_ATTRIBUTE_ALLOCATION_REALMODE_ACCESSIBLE;
                }

                Status = MmMdAddDescriptorToList(MemoryMap,
                                                 OsDescriptor,
                                                 MDL_FLAGS_TRUNCATE);

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                //
                // Fall through and create a descriptor for the part above 1MB.
                //

                EfiStartPage = (_1MB >> EFI_PAGE_SHIFT);

            } else {
                MemoryType = Below1MbMemoryType;
            }
        }

        //
        // When booting from an EFI supplied RAM disk, mark this memory with
        // the special RAMDISK type in order to be correctly recognized by the
        // kernel during transition to the OS.
        //
        // Because EFI page sizes can be smaller than OS page sizes,
        // it is possible to encounter multiple EFI memory regions
        // that overlap one OS page. When this happens a winner is
        // chosen based on precedence of the memory type. The RAM disk
        // memory type is library-class and would ordinarily be lower
        // precedent than most other EFI memory which is of platform-
        // class. Since the RAM disk section needs to be kept wholly intact,
        // special-case truncation is done here.
        //

        if (BootDeviceIsRamDisk != FALSE) {

            //
            // Found the EFI memory descriptor containing the RAM disk.
            //

            if ((EfiStartPage <= RamDiskEfiStartPage) &&
                (EfiEndPage >= RamDiskEfiEndPage)) {

                //
                // There is memory in the descriptor before
                // the RAM disk starts.
                //

                if (GET_OSPAGE(EfiStartPage) <
                    GET_OSPAGE(RamDiskEfiStartPage)) {

                    OsDescriptor = MmMdInitDescriptor(
                                       GET_OSPAGE(EfiStartPage),
                                       0,
                                       GET_OSPAGE(RamDiskEfiStartPage) -
                                       GET_OSPAGE(EfiStartPage),
                                       MmFwpGetOsAttributeType(EfiMm.Attribute),
                                       MemoryType);

                    if (OsDescriptor == NULL) {
                        Status = STATUS_INSUFFICIENT_RESOURCES;
                        break;
                    }

                    Status = MmMdAddDescriptorToList(MemoryMap,
                                                     OsDescriptor,
                                                     MDL_FLAGS_TRUNCATE);

                    if (!NT_SUCCESS(Status)) {
                        break;
                    }
                }

                //
                // Add memory descriptor containing the RAM disk using the
                // special type MEMORY_TYPE_RAMDISK.
                //

                OsDescriptor = MmMdInitDescriptor(
                                   GET_OSPAGE(RamDiskEfiStartPage),
                                   0,
                                   GET_OSPAGE(RamDiskEfiEndPage) -
                                   GET_OSPAGE(RamDiskEfiStartPage),
                                   MmFwpGetOsAttributeType(EfiMm.Attribute),
                                   MEMORY_TYPE_RAMDISK);

                if (OsDescriptor == NULL) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                Status = MmMdAddDescriptorToList(MemoryMap,
                                                 OsDescriptor,
                                                 MDL_FLAGS_TRUNCATE);

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                //
                // There is additional memory at the end of this descriptor
                // that is not being used by the RAM disk. Adjust the start
                // page and fall-through to the standard memory addition
                // code.
                //

                if (GET_OSPAGE(EfiEndPage) > GET_OSPAGE(RamDiskEfiEndPage)) {
                    EfiStartPage = GET_EFIPAGE(
                                       GET_OSPAGE(RamDiskEfiEndPage - 1) + 1);

                } else {
                    continue;
                }

            } else if ((GET_OSPAGE(EfiStartPage) <
                        GET_OSPAGE(RamDiskEfiStartPage)) &&
                       (GET_OSPAGE(EfiEndPage) >
                        GET_OSPAGE(RamDiskEfiStartPage))) {

                //
                // Truncate memory that overlaps with the start of the RAM disk
                // memory segment.
                //

                EfiEndPage = RamDiskEfiStartPage;
                if (GET_OSPAGE(EfiStartPage) == GET_OSPAGE(EfiEndPage)) {
                    continue;
                }

            } else if ((GET_OSPAGE(EfiStartPage) <
                        GET_OSPAGE(RamDiskEfiEndPage)) &&
                       (GET_OSPAGE(EfiEndPage) >
                        GET_OSPAGE(RamDiskEfiEndPage))) {

                //
                // Truncate memory that overlaps with the end of the RAM disk
                // memory segment.
                //

                EfiStartPage = RamDiskEfiEndPage;
                if (GET_OSPAGE(EfiStartPage) == GET_OSPAGE(EfiEndPage)) {
                    continue;
                }
            }
        }

       OsDescriptor = MmMdInitDescriptor(
                           GET_OSPAGE(EfiStartPage),
                           0,
                           GET_OSPAGE(EfiEndPage) - GET_OSPAGE(EfiStartPage),
                           MmFwpGetOsAttributeType(EfiMm.Attribute),
                           MemoryType);

        if (OsDescriptor == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // Add a special attribute to the free pages below 1MB.
        //

        if (((OsDescriptor->MemoryType == MEMORY_TYPE_FREE) ||
             (OsDescriptor->MemoryType == MEMORY_TYPE_FREE_ZEROED)) &&
            (EfiEndPage <= (_1MB >> EFI_PAGE_SHIFT))) {

            OsDescriptor->Attributes |=
                MEMORY_ATTRIBUTE_ALLOCATION_REALMODE_ACCESSIBLE;
        }

        //
        // Add the equivalent OS memory descriptor to the specified list.
        //

        MdlFlags = MDL_FLAGS_TRUNCATE;
        if (CHECK_FLAG(Flags, MM_MEMORYMAP_FLAGS_COALESCE) != FALSE) {
            SET_FLAGS(MdlFlags, MDL_FLAGS_COALESCE);
        }

        Status = MmMdAddDescriptorToList(MemoryMap,
                                         OsDescriptor,
                                         MdlFlags);

        if (!NT_SUCCESS(Status)) {
            break;
        }
    }

    //
    // Update the descriptors in the map to reflect the upcoming free of this
    // routines efi-based temporary allocation.
    //

    if (AlwaysUseFirmwareAllocator != FALSE) {
        EfiStartPage = EfiBufferBase >> EFI_PAGE_SHIFT;
        EfiEndPage = ROUND_UP(EfiStartPage + GET_EFIPAGE(Pages),
                              GET_EFIPAGE(1));

        OsStartPage = GET_OSPAGE(EfiStartPage);
        OsPageCount = (GET_OSPAGE(EfiEndPage) - GET_OSPAGE(EfiStartPage));

        //
        // Free the pages back to the firmware, if this fails, avoid updating
        // the memory map to reflect the free.  This failure should not be
        // propagated, it is better to have the library think the pages are
        // in use by the firmware, then to potentially fail the boot.
        //

        Status = EfiFreePages(EfiBufferBase, (UINTN)GET_EFIPAGE(Pages));
        if (!NT_SUCCESS(Status)) {
            Status = STATUS_SUCCESS;
            goto FwGetMemoryMapEnd;
        }

        EfiBufferBase = (EFI_PHYSICAL_ADDRESS)0;
        OsDescriptor = MmMdFindDescriptorFromMdl(MemoryMap,
                                                 OsStartPage,
                                                 MDL_FLAGS_PHYSICAL_ADDRESS);

        if (OsDescriptor == NULL) {
            Status = STATUS_UNSUCCESSFUL;
            goto FwGetMemoryMapEnd;
        }

        FreeDescriptor = MmMdInitDescriptor(OsStartPage,
                                            0,
                                            OsPageCount,
                                            OsDescriptor->Attributes,
                                            MEMORY_TYPE_FREE);

        if (FreeDescriptor == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto FwGetMemoryMapEnd;
        }

        Status = MmMdRemoveRegionFromMdl(MemoryMap,
                                         OsStartPage,
                                         OsPageCount,
                                         MDL_FLAGS_PHYSICAL_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            MmMdFreeDescriptor(FreeDescriptor);
            goto FwGetMemoryMapEnd;
        }

        Status = MmMdAddDescriptorToList(MemoryMap,
                                         FreeDescriptor,
                                         MDL_FLAGS_COALESCE);
    }

FwGetMemoryMapEnd:
    if (EfiBufferBase != (EFI_PHYSICAL_ADDRESS)0) {
        EfiFreePages(EfiBufferBase, (UINTN)GET_EFIPAGE(Pages));
    }

    if (BootLibBufferBase != NULL) {
        BlMmFreePages(BootLibBufferBase);
    }

    if (!NT_SUCCESS(Status)) {
        MmMdFreeList(MemoryMap);
    }

    MmLeaveDescriptorLibraryApi();
    return Status;
}

NTSTATUS
MmFwAllocatePages (
    __in ULONGLONG PhysicalBasePage,
    __in ULONGLONG Pages
    )

/*++

Routine Description:

    Makes platform specific allocation for the requested memory region.

    The OS owns the memory map on a PC/AT BIOS system.  Resultantly, there
    is nothing to do.

Arguments:

    PhysicalBasePage - The base page of the physical memory region to allocate.

    Pages - The number of pages to allocate.

Return Value:

    STATUS_SUCCESS always.
    STATUS_INVALID_PARAMETER or STATUS_NO_MEMORY on error.

--*/

{

    EFI_PHYSICAL_ADDRESS Address;
    NTSTATUS Status;

    //
    // Allocate the requested address range using EFI boot services.
    //

    __analysis_assume(Pages < (MAXULONGLONG >> PAGE_SHIFT));

    Address = PhysicalBasePage << PAGE_SHIFT;
    Status = EfiAllocatePages(AllocateAddress,
                              EfiLoaderCode,
                              (UINTN)GET_EFIPAGE(Pages),
                              &Address);
    return Status;
}

NTSTATUS
MmFwFreePages (
    __in ULONGLONG PhysicalBasePage,
    __in ULONGLONG Pages
    )

/*++

Routine Description:

    Frees memory allocated via the firmware.

Arguments:

    PhysicalBasePage - The base page of the physical address to be freed.

    Pages - The number of pages (in OS page size) to be freed.  Although the
        firmware and the OS may have different memory page sizes, this should
        not be a problem.  This routine should only every attempt to free
        memory allocated from MmFwAllocatePages, and that routine always
        allocates memory regions which are OS page aligned.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND if BaseAllocationPage was not allocated via the EFI boot
        service, AllocatePages.

--*/

{

    EFI_PHYSICAL_ADDRESS Address;
    NTSTATUS Status;

    Address = PhysicalBasePage << PAGE_SHIFT;
    Status = EfiFreePages(Address, (UINTN)GET_EFIPAGE(Pages));
    return Status;
}

NTSTATUS
MmFwSynchronizeMemoryMap (
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

    PMEMORY_DESCRIPTOR BlDescriptor;
    ULONGLONG BlEndPage;
    ULONGLONG FwCurrentPage;
    PMEMORY_DESCRIPTOR FwDescriptor;
    ULONGLONG FwEndPage;
    MEMORY_DESCRIPTOR_LIST FwMemoryMap;
    ULONGLONG MappedCurrentPage;
    PMEMORY_DESCRIPTOR_LIST Mdl;
    ULONG MemoryMapFlags;
    PMEMORY_DESCRIPTOR NewDescriptor;
    PLIST_ENTRY NextEntry;
    ULONGLONG Pages;
    BOOLEAN RunTruncation;
    NTSTATUS Status;
    NTSTATUS TruncationStatus;

    //
    // Any routine that can result in the allocation of a global memory
    // descriptor, needs to announce its execution.
    //

    MmEnterDescriptorLibraryApi();

    //
    // Get the current firmware memory map.
    //

    RunTruncation = FALSE;
    MmMdInitializeList(&FwMemoryMap, MdlTypePhysical, NULL);
    MemoryMapFlags = MM_MEMORYMAP_FLAGS_USE_FIRMWARE_ALLOCATOR;
    MemoryMapFlags |= MM_MEMORYMAP_FLAGS_COALESCE;
    Status = MmFwGetMemoryMap(&FwMemoryMap, MemoryMapFlags);
    if (!NT_SUCCESS(Status)) {
        goto SynchronizeMemoryMapEnd;
    }

    //
    // The OS Loader will use firmware services to load and unload runtime
    // drivers causing runtime code, runtime data, and free memory regions
    // to change.  The firmware should always be the most accurrate, so scan
    // for each of these regions and update the appropriate descriptors on
    // the library's memory descriptor lists.
    //

    NextEntry = FwMemoryMap.Head->Flink;
    while (NextEntry != FwMemoryMap.Head) {
        FwDescriptor = CONTAINING_RECORD(NextEntry,
                                         MEMORY_DESCRIPTOR,
                                         ListEntry);

        switch (FwDescriptor->MemoryType) {
        case MEMORY_TYPE_RUNTIME_SERVICE_CODE:
        case MEMORY_TYPE_RUNTIME_SERVICE_DATA:
        case MEMORY_TYPE_FREE:
        case MEMORY_TYPE_FREE_ZEROED:
            break;

        case MEMORY_TYPE_BOOT_SERVICE_CODE:
            if (CHECK_FLAG(MemoryFlags, MEMORY_SYNC_FLAGS_BOOT_SERVICE) == 0) {
                NextEntry = NextEntry->Flink;
                continue;
            }

            break;

        default:
            NextEntry = NextEntry->Flink;
            continue;
        }

        //
        // Find the corresponding internal descriptors for this region.
        //

        FwCurrentPage = FwDescriptor->BasePage;
        FwEndPage = FwDescriptor->BasePage + FwDescriptor->PageCount;
        while (FwCurrentPage < FwEndPage) {

            //
            // Fail if any descriptors are found on the allocated lists.
            //

            BlDescriptor = MmMdFindDescriptor(FwCurrentPage,
                                              MEMORYMAP_TYPE_ALLOCATED,
                                              MDL_FLAGS_PHYSICAL_ADDRESS);

            if (BlDescriptor != NULL) {
                BlEndPage = BlDescriptor->BasePage + BlDescriptor->PageCount;
                BlStatusPrint(L"Unexpected, allocated descriptor found for "
                              L"firmware region "
                              L"FW[%08x%08x:%08x%08x]:%08x:%08x "
                              L"BL[%08x%08x:%08x%08x]:%08x:%08x\r\n",
                              (ULONG)(FwDescriptor->BasePage >> 32),
                              (ULONG)FwDescriptor->BasePage,
                              (ULONG)(FwEndPage >> 32),
                              (ULONG)(FwEndPage),
                              FwDescriptor->MemoryType,
                              FwDescriptor->Attributes,
                              (ULONG)(BlDescriptor->BasePage >> 32),
                              (ULONG)BlDescriptor->BasePage,
                              (ULONG)(BlEndPage >> 32),
                              (ULONG)(BlEndPage),
                              BlDescriptor->MemoryType,
                              BlDescriptor->Attributes);

                //
                // Firmware may have reported enclave regions as runtime
                // services. This is allowed, though unusual.
                //

                if ((BlDescriptor->MemoryType == MEMORY_TYPE_ENCLAVE) &&
                    ((FwDescriptor->MemoryType ==
                      MEMORY_TYPE_RUNTIME_SERVICE_CODE) ||
                     (FwDescriptor->MemoryType ==
                      MEMORY_TYPE_RUNTIME_SERVICE_DATA))) {

                    FwCurrentPage = BlEndPage;
                    continue;
                }

                Status = STATUS_CONFLICTING_ADDRESSES;
                goto SynchronizeMemoryMapEnd;
            }

            //
            // Attempt to locate the library descriptor that describes this
            // region, fail if this cannot be found as this is an indication of
            // a major inconsistency.
            //

            Mdl = &MmMdlUnmappedUnallocated;
            BlDescriptor =
                MmMdFindDescriptor(FwCurrentPage,
                                   MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED,
                                   MDL_FLAGS_PHYSICAL_ADDRESS);

            if (BlDescriptor == NULL) {
                Mdl = &MmMdlMappedUnallocated;
                BlDescriptor =
                    MmMdFindDescriptor(FwCurrentPage,
                                       MEMORYMAP_TYPE_MAPPED_UNALLOCATED,
                                       MDL_FLAGS_PHYSICAL_ADDRESS);
            }

            if (BlDescriptor == NULL) {
                Mdl = &MmMdlTruncatedMemory;
                BlDescriptor =
                    MmMdFindDescriptor(FwCurrentPage,
                                       MEMORYMAP_TYPE_TRUNCATED,
                                       MDL_FLAGS_PHYSICAL_ADDRESS);
            }

            if (BlDescriptor == NULL) {
                Mdl = &MmMdlBadMemory;
                BlDescriptor =
                    MmMdFindDescriptor(FwCurrentPage,
                                       MEMORYMAP_TYPE_BAD,
                                       MDL_FLAGS_PHYSICAL_ADDRESS);
            }

            if (BlDescriptor == NULL) {
                BlStatusPrint(L"Unexpected, no descriptor found for "
                              L"firmware region [%08x%08x:%08x%08x]\r\n",
                              (ULONG)(FwCurrentPage >> 32),
                              (ULONG)FwCurrentPage,
                              (ULONG)(FwEndPage >> 32),
                              (ULONG)(FwEndPage));

                Status = STATUS_CONFLICTING_ADDRESSES;
                goto SynchronizeMemoryMapEnd;
            }

            BlEndPage = BlDescriptor->BasePage + BlDescriptor->PageCount;
            Pages = (min(BlEndPage,FwEndPage) - FwCurrentPage);

            //
            // Check to see if all or part of this firmware region is bad
            // memory.  If so, and it is in-use by the firmware log an error in
            // the system event log.
            //

            if (Mdl == &MmMdlBadMemory) {
                if ((FwDescriptor->MemoryType != MEMORY_TYPE_FREE) &&
                    (FwDescriptor->MemoryType != MEMORY_TYPE_FREE_ZEROED)) {

                    BlStatusPrint(L"Unexpected, in-use firmware region using "
                                  L"bad memory. "
                                  L"FW[%08x%08x:%08x%08x]:%08x:%08x "
                                  L"BL[%08x%08x:%08x%08x]:%08x:%08x\r\n",
                                  (ULONG)(FwDescriptor->BasePage >> 32),
                                  (ULONG)FwDescriptor->BasePage,
                                  (ULONG)(FwEndPage >> 32),
                                  (ULONG)(FwEndPage),
                                  FwDescriptor->MemoryType,
                                  FwDescriptor->Attributes,
                                  (ULONG)(BlDescriptor->BasePage >> 32),
                                  (ULONG)BlDescriptor->BasePage,
                                  (ULONG)(BlEndPage >> 32),
                                  (ULONG)(BlEndPage),
                                  BlDescriptor->MemoryType,
                                  BlDescriptor->Attributes);

                    MmFwpWriteBadMemoryEvent(FwDescriptor, BlDescriptor);
                }

                FwCurrentPage = BlEndPage;
                continue;
            }

            //
            // Compare the types and attributes for this region between the
            // library and the firmware.  Skip over regions which are an exact
            // match.
            //

            if ((BlDescriptor->MemoryType == FwDescriptor->MemoryType) &&
                (BlDescriptor->Attributes == FwDescriptor->Attributes)) {

                FwCurrentPage = BlEndPage;
                continue;
            }

            //
            // Allow synchronization of regions to float between any of the
            // following types.  Attributes will also be updated if they are
            // out of synchronization.
            //

            switch (BlDescriptor->MemoryType) {
            case MEMORY_TYPE_RUNTIME_SERVICE_CODE:
            case MEMORY_TYPE_RUNTIME_SERVICE_DATA:
            case MEMORY_TYPE_FREE:
            case MEMORY_TYPE_FREE_ZEROED:
                break;

            default:
                BlStatusPrint(L"Unexpected, memory type conflict for "
                              L"firmware region "
                              L"FW[%08x%08x:%08x%08x]:%08x:%08x "
                              L"BL[%08x%08x:%08x%08x]:%08x:%08x\r\n",
                              (ULONG)(FwDescriptor->BasePage >> 32),
                              (ULONG)FwDescriptor->BasePage,
                              (ULONG)(FwEndPage >> 32),
                              (ULONG)(FwEndPage),
                              FwDescriptor->MemoryType,
                              FwDescriptor->Attributes,
                              (ULONG)(BlDescriptor->BasePage >> 32),
                              (ULONG)BlDescriptor->BasePage,
                              (ULONG)(BlEndPage >> 32),
                              (ULONG)(BlEndPage),
                              BlDescriptor->MemoryType,
                              BlDescriptor->Attributes);

                FwCurrentPage = BlEndPage;
                continue;
            }

            NewDescriptor = MmMdInitDescriptor(FwCurrentPage, 0, Pages,
                                               FwDescriptor->Attributes,
                                               FwDescriptor->MemoryType);

            if (NewDescriptor == NULL) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto SynchronizeMemoryMapEnd;
            }

            //
            // Remove any virtual mappings for this region if it is mapped,
            // prior to taking it off the mapped list.  This case is not
            // currently expected to occur.  Assert if this happens so that
            // it can be debugged.
            //

            if (Mdl == &MmMdlMappedUnallocated) {
                BlStatusPrint(L"Unexpected, memory type conflict for firmware "
                              L"region [%08x%08x:%08x%08x] found on "
                              L"MappedUnallocated list.\r\n",
                              (ULONG)(FwCurrentPage >> 32),
                              (ULONG)FwCurrentPage,
                              (ULONG)(FwEndPage >> 32),
                              (ULONG)(FwEndPage));

                ASSERT(FALSE);

                MappedCurrentPage = BlDescriptor->MappedBasePage +
                    (FwCurrentPage - BlDescriptor->BasePage);

                BlMmUnmapVirtualAddress((PVOID)(ULONG_PTR)
                                        (MappedCurrentPage << PAGE_SHIFT),
                                        (Pages << PAGE_SHIFT));
            }

            //
            // Remove this region from the unallocated list it was found on.
            //

            Status = MmMdRemoveRegionFromMdl(Mdl, FwCurrentPage, Pages,
                                             MDL_FLAGS_PHYSICAL_ADDRESS);

            if (!NT_SUCCESS(Status)) {
                MmMdFreeDescriptor(NewDescriptor);
                goto SynchronizeMemoryMapEnd;
            }

            //
            // Add the new descriptor to the unmapped, unallocated list.
            //

            Status = MmMdAddDescriptorToList(&MmMdlUnmappedUnallocated,
                                             NewDescriptor,
                                             (MDL_FLAGS_COALESCE |
                                              MDL_FLAGS_TRUNCATE));

            if (!NT_SUCCESS(Status)) {
                MmMdFreeDescriptor(NewDescriptor);
                goto SynchronizeMemoryMapEnd;
            }

            //
            // Remember that regions have been added back to the unmapped,
            // unallocated list such that truncation can be performed against
            // that list in case certain regions should not be used by the
            // page allocator or the operating system.
            //

            RunTruncation = TRUE;
            FwCurrentPage = BlEndPage;
        }

        NextEntry = NextEntry->Flink;
    }

SynchronizeMemoryMapEnd:
    if (RunTruncation != FALSE) {
        TruncationStatus =
            MmMdTruncateDescriptors(&MmMdlUnmappedUnallocated,
                                    &MmMdlTruncatedMemory,
                                    MmPaLookupMaximumPhysicalPage());

        if (!NT_SUCCESS(TruncationStatus)) {
            Status = TruncationStatus;
        }
    }

    MmMdFreeList(&FwMemoryMap);
    MmLeaveDescriptorLibraryApi();
    return Status;
}

// --------------------------------------------------------- Internal Functions

ULONG
MmFwpGetOsAttributeType (
    __in UINT64 EfiAttributes
    )

/*++

Routine Description:

    Maps an EFI Attributes bitmask to a OS defined bitmask for a memory
    region's attributes.

Arguments:

    EfiAttributes - Efi Attribute bitmask.

    Efi Attributes : (the bit OR of the following constants) -

    EFI_MEMORY_UC : Cachability - Memory region supports being configured as
                                  NOT cachable.
    EFI_MEMORY_WC : Cachability - Memory region supports being configured as
                                  write combined.
    EFI_MEMORY_WT : Cachability - Memory region supports being configured with
                                  a "write through" policy.  Writes that hit
                                  the cache are also written to main memory.
    EFI_MEMORY_WB : Cachability - Memory region supports being configured with
                                  a "write back" policy.  Reads and writes that
                                  hit the cache do not propogate into main
                                  memory.  Dirty data is written to main memory
                                  when a new cache line is allocated.

    EFI_MEMORY_UCE : Cachability - Memory region supports being configured as
                                   NOT cachable, exported.  Supports the
                                   "fetch and add" semaphore mechanism.
    EFI_MEMORY_WP  : Protection - Memory region supports being configured to
                                  protect memory from being written.
    EFI_MEMORY_RP  : Protection - Memory region supports being configured to
                                  protect memory from being read.
    EFI_MEMORY_XP  : Protection - Memory region supports being configured to
                                  memory from being executed.
    EFI_MEMORY_RUNTIME : Runtime Memory.

Return Value:

    OS defined Attribute bitmask.

--*/

{

    ULONG OsAttributes = 0;

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_UC)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_UNCACHED);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WC)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_COMBINED);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WT)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_THROUGH);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WB)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_WRITE_BACK);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_UCE)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_CACHE_UNCACHED_EXPORTED);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_WP)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_WRITE);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_RP)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_READ);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_XP)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_PROTECT_EXECUTE);
    }

    if (CHECK_FLAG(EfiAttributes, EFI_MEMORY_RUNTIME)) {
        SET_FLAGS(OsAttributes, MEMORY_ATTRIBUTE_DESCRIPTION_FIRMWARE_RUNTIME);
    }

    return OsAttributes;
}

VOID
MmFwpWriteBadMemoryEvent(
    __in PMEMORY_DESCRIPTOR FwDescriptor,
    __in PMEMORY_DESCRIPTOR BlDescriptor
    )

/*++

Routine Description:

    This routine will log an event indicating that firmware has a memory region
    containing at least one page that is marked bad.

Arguments:

    FwDescriptor - Supplies the memory descriptor from the firmware memory map
        describing the firmware allocation.

    BlDescriptor - Supplies the memory descriptor from the boot application
        memory map describing the bad memory being used.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[8];

    EventDataDescCreate(&EventDataDescriptor[0],
                        &FwDescriptor->BasePage,
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[1],
                        &FwDescriptor->PageCount,
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[2],
                        &FwDescriptor->Attributes,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[3],
                        &FwDescriptor->MemoryType,
                        sizeof(MEMORY_TYPE));

    EventDataDescCreate(&EventDataDescriptor[4],
                        &BlDescriptor->BasePage,
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[5],
                        &BlDescriptor->PageCount,
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[6],
                        &BlDescriptor->Attributes,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[7],
                        &BlDescriptor->MemoryType,
                        sizeof(MEMORY_TYPE));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BAD_MEMORY_USED_BY_FIRMWARE,
                  NULL,
                  RTL_NUMBER_OF(EventDataDescriptor),
                  EventDataDescriptor);
}

