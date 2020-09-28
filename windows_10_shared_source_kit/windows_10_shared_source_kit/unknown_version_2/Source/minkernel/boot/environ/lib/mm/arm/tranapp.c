/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    tranapp.c

Abstract:

    This module implements routines to build and free Arm page tables to allow
    an application (32-bit or 64-bit) to transition into another 64-bit
    application.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>
#include "tranapp.h"

//
// ------------------------------------------------------------------- Defines
//

//
// Define the absolute maximum for the initial number of memory blocks.
//
// N.B The values below are arbitrary and need to be updated

#define MM_APP_BLOCK_MEMORY_INITIAL_MAXIMUM        64

//
// Define the minimum for the initial number of memory blocks.
//

#define MM_APP_BLOCK_MEMORY_INITIAL_MINIMUM        16

//
// Define the default number of blocks to be used when the total number of
// pages being mapped is high.
//

#define MM_APP_BLOCK_MEMORY_DEFAULT_HIGH           32

//
// Define the default number of blocks to be used when the total number of
// pages being mapped is low.
//

#define MM_APP_BLOCK_MEMORY_DEFAULT_LOW            16

//
// ------------------------------------------------------------------- Globals
//

//
// Define the handle to be used for making block memory allocations.
//

BLOCK_ALLOCATOR_HANDLE MmArmpBlockAllocatorHandle;

//
// Store the physical address of the dynamic self map reserved memory so it can
// be freed upon return from the application.
//

PHYSICAL_ADDRESS DynamicSelfMapBase;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
MmArmpAllocateAndInitializePageTables (
    __out_ecount(PagesToAllocate) PVOID *PageTables,
    __in ULONG PagesToAllocate
    );

VOID
MmArmpEstimateBlockSizes (
    __in PLIST_ENTRY DescriptorList,
    __out PULONG InitialBlocks,
    __out PULONG DefaultBlocks
    );

NTSTATUS
MmArmpAllocateApplicationHardwarePageDirectory (
    __in ULONG MemoryAttributes,
    __out PULONG *MappedHardwarePageDirectory,
    __out PPHYSICAL_ADDRESS HardwarePageDirectory
    );

VOID
MmArmpFreeApplicationHardwarePageDirectory (
    __in PULONG MappedHardwarePageDirectory,
    __in PHYSICAL_ADDRESS HardwarePageDirectory
    );

NTSTATUS
MmArmFreeApplicationPageTable (
    __in PULONG SoftwarePageDirectory,
    __in PULONG HardwarePageDirectory
    );

NTSTATUS
MmArmFreeApplicationPageTables (
    __in PARM_MM_STATE MmState
    );

NTSTATUS
MmArmpCreateMapping (
    __in PULONG PageDirectory,
    __in PULONG MappedHardwarePageDirectory,
    __in ULONGLONG VirtualBasePage,
    __in ULONGLONG PhysicalBasePage,
    __in ULONGLONG PageCount,
    __in BOOLEAN IgnoreConflicts
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
MmArmpMapFirmwareData (
    __in PULONG PageDirectory,
    __in PULONG MappedHardwarePageDirectory
    )

/*++

Routine Description:

    This routine creates auxiliary memory mappings that may be referenced
    by the new boot application on EFI systems.

    The auxiliary mappings include firmware boot and runtime code/data.

Arguments:

    PageDirectory - Supplies a pointer to the top level page table into which all the
           mappings will be effected.

    MappedHardwarePageDirectory - Supplies a pointer to the hardware page
           directory in which the mappings will be effected.

Return Value:

    STATUS_SUCCESS if successful.
    Appropriate NTSTATUS code otherwise.

--*/

{
    LIST_ENTRY FirmwareMemoryMap;
    PLIST_ENTRY NextEntry;
    MEMORY_DESCRIPTOR *MemoryDescriptor;
    GENERIC_BUFFER DescriptorBuffer;
    NTSTATUS Status;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    FirmwareMemoryMap.Flink = NULL;

    //
    // Walk the system memory descriptor list and identity map all firmware
    // data regions that may be accessed by a boot application.
    //
    // TODO: Should MEMORY_TYPE_UNMAPPED_UNALLOCATED be used for efficiency?
    //

    Status = BlMmGetMemoryMap(&FirmwareMemoryMap,
                              &DescriptorBuffer,
                              MEMORYMAP_TYPE_ALL,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto MmArmpMapFirmwareDataExit;
    }

    NextEntry = FirmwareMemoryMap.Flink;
    while (NextEntry != &FirmwareMemoryMap) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // EFIHACK: All firmware calls are made in physical mode.  Therefore,
        //          firmware code should not require any mappings, just data.
        //          Unfortunately, some EFI implementations are currently
        //          marking data as code. (e.g. EfiConOut->Mode)
        //

        if ((MemoryDescriptor->MemoryType == MEMORY_TYPE_BOOT_SERVICE_CODE) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_RUNTIME_SERVICE_CODE) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_RUNTIME_SERVICE_DATA) ||
            (MemoryDescriptor->MemoryType == MEMORY_TYPE_BOOT_APPLICATION)) {

            Status = MmArmpCreateMapping(PageDirectory,
                                         MappedHardwarePageDirectory,
                                         MemoryDescriptor->BasePage,
                                         MemoryDescriptor->BasePage,
                                         MemoryDescriptor->PageCount,
                                         FALSE);

            if (!NT_SUCCESS(Status)) {
                goto MmArmpMapFirmwareDataExit;
            }
        }

        NextEntry = NextEntry->Flink;
    }

MmArmpMapFirmwareDataExit:

    //
    // Free the memory allocated for the memory descriptor list.
    //
    //

    if (DescriptorBuffer.Buffer != NULL) {
        BlMmFreeHeap(DescriptorBuffer.Buffer);
    }

    return Status;
}

NTSTATUS
MmArmBuildApplicationPageTables (
    __in PARM_MM_STATE MmState,
    __in ULONG Flags,
    __out PULONG SelfMapPdeBase
    )

/*++

Routine Description:

    This routine builds a multi-level Arm page table hierarchy for use
    by a new 32-bit boot application. It identity maps all the memory that
    may be referenced by the new application.

Arguments:

    MmState - Supplies a pointer to store the accumulated application state.

    Flags - Supplies a set of flags that dictate how the page tables are
        created.

    SelfMapPdeBase - Supplies a pointer to a variable the receives the location
        of the self map PDE base.

Return Value:

    STATUS_SUCCESS when successful.

    Appropriate NTSTATUS code otherwise.

--*/

{

    PHYSICAL_ADDRESS Address;
    LIST_ENTRY AllocatedMdl;
    BLOCK_ALLOCATOR_HANDLE BlockAllocatorHandle;
    ULONG DefaultBlocks;
    GENERIC_BUFFER DescriptorBuffer;
    PHYSICAL_ADDRESS HardwarePageDirectory;
    ULONG InitialBlocks;
    PULONG MappedHardwarePageDirectory;
    ULONG MemoryAttributes;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    NATIVE_MEMORY_ATTRIBUTES NativeMemoryAttributes;
    PLIST_ENTRY NextEntry;
    PULONG Pde;
    ULONG PdeOffset;
    PULONG SoftwarePageDirectory;
    NTSTATUS Status;
    ULONGLONG VirtualBasePage;

    BlockAllocatorHandle = BLOCK_ALLOCATOR_INVALID_HANDLE;
    MappedHardwarePageDirectory = NULL;
    HardwarePageDirectory.QuadPart = 0;
    SoftwarePageDirectory = NULL;
    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);

    //
    // Allocate hardware page directory. Do this first so it gets mapped 1:1. It
    // will be mapped to the well-known address ARM_PAGE_DIRECTORY_VA_INITIAL
    // after ExitBootServices.
    //

    MemoryAttributes = MEMORY_ATTRIBUTE_CACHE_WRITE_BACK;
    Status = MmArmpAllocateApplicationHardwarePageDirectory(
                 MemoryAttributes,
                 &MappedHardwarePageDirectory,
                 &HardwarePageDirectory);

    if (!NT_SUCCESS(Status)) {
        goto MmArmBuildAppPageTableEnd;
    }

    //
    // Get a list of memory currently allocated by the boot application.  This
    // list describe the memory which may be accessed by the loaded boot
    // application and therefore must be mapped within the new page table.
    //

    AllocatedMdl.Flink = NULL;
    Status = BlMmGetMemoryMap(&AllocatedMdl,
                              &DescriptorBuffer,
                              MEMORYMAP_TYPE_ALLOCATED,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto MmArmBuildAppPageTableEnd;
    }

    //
    // Estimate the initial and default number of blocks for block memory
    // allocations.
    //

    MmArmpEstimateBlockSizes(&AllocatedMdl, &InitialBlocks, &DefaultBlocks);

    //
    // Initialize the block of physical pages to be used for allocating
    // page table pages. To ensure an allocated block is freed back to the
    // firmware with exactly the same number of pages as it was originally
    // allocated with, do not coalesce the actual memory descriptors.
    //

    BlockAllocatorHandle = BlpMmCreateBlockAllocator(PhysicalPages,
                                                     MEMORY_TYPE_PAGE_TABLE,
                                                     MemoryAttributes,
                                                     0,
                                                     InitialBlocks,
                                                     DefaultBlocks,
                                                     PAGE_SIZE);

    if (BlockAllocatorHandle == BLOCK_ALLOCATOR_INVALID_HANDLE) {
        goto MmArmBuildAppPageTableEnd;
    }

    MmArmpBlockAllocatorHandle = BlockAllocatorHandle;

    //
    // Allocate a page for the top level of the page table.
    //

    Status = MmArmpAllocateAndInitializePageTables(&SoftwarePageDirectory, 1);
    if (!NT_SUCCESS(Status)) {
        goto MmArmBuildAppPageTableEnd;
    }

    //
    // We asked for cached memory for page tables.  Determine what we
    // actually received and make the PTE mappings and the TTB config
    // match.  These same settings will be derived by WINLOAD by the
    // environment.
    //

    Address = HardwarePageDirectory;
    Status = MmArmDetermineMatchingMemoryAttributes(Address,
                                                    &NativeMemoryAttributes);

    if (!NT_SUCCESS(Status)) {
        NativeMemoryAttributes = MmArmMapAttributes(MemoryAttributes);
    }

    MmArmSetNativePageTableAttributes(NativeMemoryAttributes);
    MmState->TTB_Config =
        MmArmMapNativeMemoryAttributesToTtbConfig(NativeMemoryAttributes);

    //
    // Map the memory specified in each descriptor.
    //

    NextEntry = AllocatedMdl.Flink;
    while (NextEntry != &AllocatedMdl) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Do not create mappings for persistent data.
        //

        if (MemoryDescriptor->MemoryType != MEMORY_TYPE_PERSISTENT_DATA) {

            //
            // If the memory descriptor contains a virtual address region,
            // then honor it.  Otherwise, just map the region one to one.
            //

            if (MemoryDescriptor->MappedBasePage == 0) {
                VirtualBasePage = MemoryDescriptor->BasePage;

            } else {
                VirtualBasePage = MemoryDescriptor->MappedBasePage;
            }

            Status = MmArmpCreateMapping(SoftwarePageDirectory,
                                         MappedHardwarePageDirectory,
                                         VirtualBasePage,
                                         MemoryDescriptor->BasePage,
                                         MemoryDescriptor->PageCount,
                                         FALSE);

            if (!NT_SUCCESS(Status)) {
                goto MmArmBuildAppPageTableEnd;
            }
        }

        NextEntry = NextEntry->Flink;
    }

    //
    // Map other auxiliary memory regions that could be referenced by the new
    // boot application.
    //

    Status = MmArmpMapFirmwareData(SoftwarePageDirectory,
                                   MappedHardwarePageDirectory);

    if (!NT_SUCCESS(Status)) {
        goto MmArmBuildAppPageTableEnd;
    }

    //
    // Create the self-map for the page table. Use the well known self-map
    // region unless the PDE is already valid. If it is already valid, allocate
    // a 4MB physical region, whose 1:1 VA range will be used for the self-map.
    //

    DynamicSelfMapBase.QuadPart = 0;
    Pde = &SoftwarePageDirectory[MiGetNativePdeOffset(PDE_BASE)];
    if ((CHECK_FLAG(Flags, CONTEXT_FLAGS_DYNAMIC_SELF_MAP) == FALSE) &&
        (MI_IS_NATIVE_PDE_VALID(Pde) == FALSE)) {

        Address.QuadPart = (ULONG_PTR)SoftwarePageDirectory;
        MmArmWriteNativePde(Pde, MappedHardwarePageDirectory, Address);
        *SelfMapPdeBase = PDE_BASE;

    } else {
        Status = MmPaReserveSelfMapPages(&DynamicSelfMapBase,
                                         _4MB >> PAGE_SHIFT,
                                         _4MB >> PAGE_SHIFT);

        if (!NT_SUCCESS(Status)) {
            goto MmArmBuildAppPageTableEnd;
        }

        PdeOffset = MiGetNativePdeOffset(
                        (PVOID)(ULONG_PTR)DynamicSelfMapBase.QuadPart);

        Pde = &SoftwarePageDirectory[PdeOffset];
        Address.QuadPart = (ULONG_PTR)SoftwarePageDirectory;
        MmArmWriteNativePde(Pde, MappedHardwarePageDirectory, Address);
        *SelfMapPdeBase = MiGeneratePdeBase(
                              (PVOID)(ULONG_PTR)DynamicSelfMapBase.QuadPart);
    }

    //
    // Establish known mapping of hardware page directory for system process.
    //
    // N.B. This is no longer necessary but is here to ease the pain of
    //      transitioning from boot managers and winloads that relied on this
    //      fixed address to boot managers and winloads that use the 1:1
    //      mapping of the hardware page directory.
    //

    Status = MmArmpCreateMapping(SoftwarePageDirectory,
                                 MappedHardwarePageDirectory,
                                 ARM_PAGE_DIRECTORY_VA_INITIAL >> PAGE_SHIFT,
                                 HardwarePageDirectory.QuadPart >> PAGE_SHIFT,
                                 NATIVE_PAGE_DIRECTORY_SIZE / PAGE_SIZE,
                                 TRUE);

    if (!NT_SUCCESS(Status)) {
        goto MmArmBuildAppPageTableEnd;
    }

    Status = STATUS_SUCCESS;

MmArmBuildAppPageTableEnd:
    MmState->MappedHardwarePageDirectory = MappedHardwarePageDirectory;
    MmState->HardwarePageDirectory = (ULONG_PTR)HardwarePageDirectory.QuadPart;
    MmState->SoftwarePageDirectory = (ULONG_PTR)SoftwarePageDirectory;

    //
    // Free the memory allocated for the MDL.
    //

    if (DescriptorBuffer.Buffer != NULL) {
        BlMmFreeHeap(DescriptorBuffer.Buffer);
    }

    //
    // On failure, unmap and free all the allocated page tables.
    //

    if (!NT_SUCCESS(Status)) {
        MmArmFreeApplicationPageTables(MmState);
        if (DynamicSelfMapBase.QuadPart != 0) {
            MmPaReleaseSelfMapPages(DynamicSelfMapBase);
        }
    }

    return Status;
}

NTSTATUS
MmArmFreeApplicationPageTable (
    __in PULONG SoftwarePageDirectory,
    __in PULONG MappedHardwarePageDirectory
    )

/*++

Routine Description:

    This routine unmaps and frees the paging hierarchy built for an Arm boot
    application.

Arguments:

    SoftwarePageDirectory - Supplies the address of the software page directory.

    MappedHardwarePageDirectory - Supplies the address of the mapped
        hardware page directory.

Return Value:

    STATUS_SUCCESS when successful.

    Appropriate NTSTATUS code.

--*/

{

    BLOCK_MEMORY_ADDRESS BlockAddress;
    PULONG PageDirectory;
    PULONG Pde;
    ULONG PdeOffset;
    ULONG PdtIndex;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    PVOID VirtualAddress;

    ASSERT(MmArmpBlockAllocatorHandle != BLOCK_ALLOCATOR_INVALID_HANDLE);

    //
    // Invalidate self-mapping to prevent double-free of the top-level page
    // table.
    //

    PageDirectory = SoftwarePageDirectory;
    if (DynamicSelfMapBase.QuadPart != 0) {
        PdeOffset = MiGetNativePdeOffset(
                        (PVOID)(ULONG_PTR)DynamicSelfMapBase.QuadPart);

        Pde = &PageDirectory[PdeOffset];
        MmArmZeroNativePde(Pde, MappedHardwarePageDirectory);
        MmPaReleaseSelfMapPages(DynamicSelfMapBase);

    } else {
        Pde = &PageDirectory[MiGetNativePdeOffset(PDE_BASE)];
        MmArmZeroNativePde(Pde, MappedHardwarePageDirectory);
    }

    //
    // Walk through the PXT table to find valid PPTs that need to be freed.
    //

    ReturnStatus = STATUS_SUCCESS;
    for (PdtIndex = 0; PdtIndex < PDE_PER_PAGE; PdtIndex++) {

        //
        // If the PDE is valid but a large page, then there is no page
        // table.
        //

        if (!MI_IS_NATIVE_PDE_VALID(&PageDirectory[PdtIndex])) {
            continue;
        }

        //
        // The PDE points to a valid page table, unmap and free it.
        //

        VirtualAddress = (PVOID)(ULONG_PTR)
            NATIVE_PT_PDE_TO_PA(&PageDirectory[PdtIndex]);

        Status = BlMmUnmapVirtualAddress(VirtualAddress, PAGE_SIZE);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        BlockAddress.PhysicalAddress.QuadPart = (UINTN)VirtualAddress;
        Status = BlpMmFreeMemoryBlocks(MmArmpBlockAllocatorHandle,
                                       BlockAddress,
                                       1);

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    //
    // All the PPTs contained in the PXT have been freed, unmap and free the
    // PXT itself.
    //

    Status = BlMmUnmapVirtualAddress(PageDirectory, PAGE_SIZE);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlockAddress.PhysicalAddress.QuadPart = (ULONG_PTR)PageDirectory;
    Status = BlpMmFreeMemoryBlocks(MmArmpBlockAllocatorHandle,
                                   BlockAddress,
                                   1);

    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    return ReturnStatus;
}

// ------------------------------------------------------------ Local Functions


NTSTATUS
MmArmpAllocateApplicationHardwarePageDirectory (
    __in ULONG MemoryAttributes,
    __out PULONG *MappedHardwarePageDirectory,
    __out PPHYSICAL_ADDRESS HardwarePageDirectory
    )

/*++

Routine Description:

    This routine allocates the hardware page directory used by the
    application and maps it.

Arguments:

    MemoryAttributes - Supplies the memory attributes to use for the allocation.

    MappedHardwarePageDirectory - Supplies the address where the mapped
        hardware page directory address will be stored.

    SoftwarePageDirectory - Supplies the pointer to the physical address of
        the hardware page directory.

Return Value:

    STATUS_SUCCESS when successful.

    Appropriate NTSTATUS code.

--*/

{

    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    PhysicalAddress.QuadPart = 0;
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       NATIVE_PAGE_DIRECTORY_SIZE / PAGE_SIZE,
                                       MEMORY_TYPE_PAGE_TABLE,
                                       MemoryAttributes,
                                       NATIVE_PAGE_DIRECTORY_SIZE / PAGE_SIZE);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    VirtualAddress = (PVOID) (ULONG_PTR) PhysicalAddress.QuadPart;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    NATIVE_PAGE_DIRECTORY_SIZE,
                                    MemoryAttributes | MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        return Status;
    }

    RtlZeroMemory(VirtualAddress, NATIVE_PAGE_DIRECTORY_SIZE);
    MmArmSyncRangeToHPW();
    *MappedHardwarePageDirectory = VirtualAddress;
    *HardwarePageDirectory = PhysicalAddress;
    return STATUS_SUCCESS;
}

VOID
MmArmpFreeApplicationHardwarePageDirectory (
    __in PULONG MappedHardwarePageDirectory,
    __in PHYSICAL_ADDRESS HardwarePageDirectory
    )

/*++

Routine Description:

    This routine frees the hardware page directory used by the
    application and unmaps it.

Arguments:

    MappedHardwarePageDirectory - Supplies the address where the mapped
        hardware page directory address will be stored.

    SoftwarePageDirectory - Supplies the pointer to the physical address of
        the hardware page directory.

Return Value:

    STATUS_SUCCESS when successful.

    Appropriate NTSTATUS code.

--*/

{
    if (MappedHardwarePageDirectory != NULL) {
        BlMmUnmapVirtualAddress(MappedHardwarePageDirectory,
                                NATIVE_PAGE_DIRECTORY_SIZE);
    }

    if (HardwarePageDirectory.QuadPart != 0) {
        BlMmFreePhysicalPages(HardwarePageDirectory);
    }
}

NTSTATUS
MmArmpAllocateAndInitializePageTables (
    __out_ecount(PagesToAllocate) PVOID *PageTables,
    __in ULONG PagesToAllocate
    )

/*++

Routine Description:

    This routine allocates page-table pages. All allocated pages are identity
    mapped and zeroed out. The caller is responsible for unmapping and
    freeing the page.

Arguments:

    PageTables - Supplies a pointer to a buffer which receives the
        physical addresses of the allocated page tables on success.

    PagesToAllocate - Supplies the number of page table pages to allocate.

Return Value:

    STATUS_SUCCESS if all pages could be allocated and mapped.

    Appropriate NTSTATUS code otherwise.

--*/

{

    BLOCK_MEMORY_ADDRESS BlockAddress;
    PPHYSICAL_ADDRESS AllocatedPageTables;
    ULONG Index;
    ULONG NumPagesAllocated;
    ULONG NumPagesMapped;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG Size;
    NTSTATUS Status;

    NumPagesAllocated = 0;
    NumPagesMapped = 0;

    //
    // Allocate a buffer to store the physical address for the page tables.
    //

    Size = PagesToAllocate * sizeof(PHYSICAL_ADDRESS);
    AllocatedPageTables = BlMmAllocateHeap(Size);
    if (AllocatedPageTables == NULL) {
        Status = STATUS_NO_MEMORY;
        goto MmArmpAllocateAndInitializePageTablesExit;
    }

    RtlZeroMemory(AllocatedPageTables, Size);

    //
    // Allocate all the required pages using block allocator.
    //
    // TODO: The pages are being individually allocated. With block memory
    //       support, attempt can be made to allocate them all at once and
    //       fallback to individual allocations if that fails.
    //

    for (NumPagesAllocated = 0;
         NumPagesAllocated < PagesToAllocate;
         NumPagesAllocated += 1) {

        Status = BlpMmAllocateMemoryBlocks(MmArmpBlockAllocatorHandle,
                                           1,
                                           &BlockAddress);

        if (!NT_SUCCESS(Status)) {
            goto MmArmpAllocateAndInitializePageTablesExit;
        }

        AllocatedPageTables[NumPagesAllocated] = BlockAddress.PhysicalAddress;
    }

    //
    // Establish identity mappings for all the allocated pages. The caller is
    // responsible for unmapping them.
    //

    for (NumPagesMapped = 0;
         NumPagesMapped < PagesToAllocate;
         NumPagesMapped++) {

        PhysicalAddress.QuadPart = AllocatedPageTables[NumPagesMapped].QuadPart;
        PageTables[NumPagesMapped] = (PVOID)(UINTN)PhysicalAddress.QuadPart;
        Status = BlMmMapPhysicalAddress(&PageTables[NumPagesMapped],
                                        PhysicalAddress,
                                        PAGE_SIZE,
                                        MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            goto MmArmpAllocateAndInitializePageTablesExit;
        }

        //
        // Zero the page table page.
        //

        RtlZeroMemory(PageTables[NumPagesMapped], PAGE_SIZE);
        MmArmSyncRangeToHPW();
    }

    Status = STATUS_SUCCESS;

MmArmpAllocateAndInitializePageTablesExit:

    ASSERT(NumPagesAllocated >= NumPagesMapped);

    //
    // Unmap and free the allocated pages on failure.
    //

    if (!NT_SUCCESS(Status)) {
        for (Index = 0; Index < NumPagesMapped; Index++) {
            BlMmUnmapVirtualAddress(PageTables[Index], PAGE_SIZE);
        }

        for (Index = 0; Index < NumPagesAllocated; Index++) {
            BlockAddress.PhysicalAddress = AllocatedPageTables[Index];
            BlpMmFreeMemoryBlocks(MmArmpBlockAllocatorHandle,
                                  BlockAddress,
                                  1);
        }
    }

    //
    // Free the buffer used for holding physical addresses of allocated page
    // tables.
    //

    if (AllocatedPageTables != NULL) {
        BlMmFreeHeap(AllocatedPageTables);
    }

    return Status;
}

NTSTATUS
MmArmFreeApplicationPageTables (
    __in PARM_MM_STATE MmState
    )

/*++

Routine Description:

    This routine frees previously allocated page-table pages.

Arguments:

    MmState - Supplies a pointer to an ARM_MM_STATE which contains references to
        the previously allocated page tables.

Return Value:

    STATUS_SUCCESS if all pages could be allocated and mapped.

    Appropriate NTSTATUS code otherwise.

--*/

{

    PHYSICAL_ADDRESS Address;

    Address.QuadPart = (ULONG_PTR) MmState->HardwarePageDirectory;
    if (MmState->SoftwarePageDirectory != 0) {
        MmArmFreeApplicationPageTable((PULONG) MmState->SoftwarePageDirectory,
                                      MmState->MappedHardwarePageDirectory);
    }

    if (MmState->HardwarePageDirectory != 0) {
        MmArmpFreeApplicationHardwarePageDirectory(
            MmState->MappedHardwarePageDirectory,
            Address);
    }

    if (MmArmpBlockAllocatorHandle != BLOCK_ALLOCATOR_INVALID_HANDLE) {

        //
        // All the mappings have been undone and physical pages
        // freed. Tear down the block memory allocator instance
        // that was created.
        //

        (VOID) BlpMmDeleteBlockAllocator(MmArmpBlockAllocatorHandle);
        MmArmpBlockAllocatorHandle = BLOCK_ALLOCATOR_INVALID_HANDLE;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
MmArmpCreateMapping (
    __in PULONG PageDirectory,
    __in PULONG MappedHardwarePageDirectory,
    __in ULONGLONG VirtualBasePage,
    __in ULONGLONG PhysicalBasePage,
    __in ULONGLONG PageCount,
    __in BOOLEAN IgnoreConflicts
    )

/*++

Routine Description:

    This routine manually walks the input page table hierarchy and maps the
    specified virtual address range [VirtualBasePage, VirtualBasePage +
    PageCount] to the specified physical address range [PhysicalBasePage,
    PhysicalBasePage + PageCount].

Arguments:

    PageDirectory - Supplies the base address for the top-level page table.

    VirtualBasePage - Starting page of the virtual address range to map to
        the specified physical address range.

    PhysicalBasePage - The physical page that starts the physical range
        being mapped.

    PageCount - Number of pages to map.

    IgnoreConflicts - Supplies a boolean indicating if physical address conflict
        checking should be ignored.

Return Value:

    STATUS_SUCCESS when successful.
    Appropriate NTSTATUS code otherwise.

--*/

{

    PHYSICAL_ADDRESS Address;
    ULONG Index;
    NATIVE_MEMORY_ATTRIBUTES NativeMemoryAttributes;
    PULONG PageTable;
    PVOID PageTableArray[3];
    ULONG PagesToAllocate;
    PULONG Pde;
    PULONG Pte;
    NTSTATUS Status;

    for (Index = 0; Index < PageCount; Index += 1, VirtualBasePage += 1) {
        PagesToAllocate = 0;
        PageTable = PageDirectory;
        Pte = NULL;

        //
        // Compute the PDE address and check if it is valid.
        //

        Pde = &PageTable[MiGetNativePdeOffset(VirtualBasePage << PAGE_SHIFT)];
        if (!MI_IS_NATIVE_PDE_VALID(Pde)) {
            PagesToAllocate = 1;
            goto AllocatePageTables;
        }

        PageTable = (PULONG)NATIVE_PT_PDE_TO_PA(Pde);
        Pte = &PageTable[MiGetNativePteOffset(VirtualBasePage << PAGE_SHIFT)];

        //
        // Determine how many page tables need to be allocated to create
        // the mapping.
        //

AllocatePageTables:

        //
        // If any page table was invalid, allocate and identity map the table.
        //
        // N.B. The mapping acquired in MmArmpAllocateAndInitializePageTables
        //      will unmapped later inside BlpArchFreeArmPageTable. All page
        //      table pages referenced inside this routine are only mapped once
        //      (even if invoked multiple times) since
        //      MmArmpAllocateAndInitializePageTables is only called once
        //      per-page table page.
        //

        if (PagesToAllocate != 0) {
            Status = MmArmpAllocateAndInitializePageTables(PageTableArray,
                                                           PagesToAllocate);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            switch (PagesToAllocate) {
            case 1:

                //
                // A page table page was allocated and needs to be
                // initialized.
                //

                //
                // Initialize PDE for the newly allocated page table page.
                //

                Address.QuadPart = (ULONG_PTR) PageTableArray[0];
                MmArmWriteNativePde(Pde,
                                    MappedHardwarePageDirectory,
                                    Address);

                PageTable = (PULONG) NATIVE_PT_PDE_TO_PA(Pde);
                Pte = &PageTable[MiGetNativePteOffset(
                                     VirtualBasePage << PAGE_SHIFT)];

                break;

            default:
                break;
            }
        }

        if (!MI_IS_NATIVE_PTE_VALID(Pte)) {

            //
            // Map the PTE to the desired physical page.
            //

            Address.QuadPart =
                (ULONG_PTR) (PhysicalBasePage + Index) << PAGE_SHIFT;

            //
            // Determine whether the cache/memory attribute associated
            // with this physical page is already known -- if so match it.
            //

            Status =
                MmArmDetermineMatchingMemoryAttributes(Address,
                                                       &NativeMemoryAttributes);

            if (!NT_SUCCESS(Status)) {

                //
                // Lacking a match, use write back cached memory.
                //

                NativeMemoryAttributes =
                    MmArmMapAttributes(MEMORY_ATTRIBUTE_CACHE_WRITE_BACK);
            }

            MmArmWriteNativePte(Pte, Address, NativeMemoryAttributes);

        } else {

            //
            // Verify that all previously mapped PTEs only map to the desired
            // physical page.
            //

            if (IgnoreConflicts == FALSE) {
                ASSERT(NATIVE_PTE_TO_PA(Pte) ==
                       ((PhysicalBasePage + Index) << PAGE_SHIFT));
            }
        }
    }

    return STATUS_SUCCESS;
}

VOID
MmArmpEstimateBlockSizes (
    __in PLIST_ENTRY DescriptorList,
    __out PULONG InitialBlocks,
    __out PULONG DefaultBlocks
    )

/*++

Routine Description:

    This routine estimates the number of intial blocks and the default
    number of blocks required for block memory allocation routine.

Arguments:

    DescriptorList - Supplies a pointer to a memory descriptors list that
        describes all the currently allocated memory.

    InitialBlocks - Supplies a pointer to a variable that receives the
        initial number of blocks to allocate. This value is between
        MM_APP_BLOCK_MEMORY_INITIAL_MINIMUM and MM_APP_BLOCK_MEMORY_INITIAL_MAXIMUM
        (both inclusive).

    DefaultBlocks - Supplies a pointer to a variable that receives the default
        number of blocks to allocate. This value is either
        MM_APP_BLOCK_MEMORY_DEFAULT_LOW or MM_APP_BLOCK_MEMORY_DEFAULT_HIGH.

Return Value:

    None.

--*/

{
    ULONG InitialPages;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    ULONG PdtPages;
    ULONG PptPages;
    ULONG PttPages;
    ULONG PxtPages;
    ULONGLONG TotalPages;

    //
    // Count the total number of pages being mapped into the new page tables.
    //

    TotalPages = 0;
    NextEntry = DescriptorList->Flink;
    while (NextEntry != DescriptorList) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        TotalPages += MemoryDescriptor->PageCount;
        NextEntry = NextEntry->Flink;       // Advance pointer.
    }

    //
    // The number of initial blocks to allocate depends on the total amount of
    // pages that will be allocated. The number of page table pages required
    // depends on the total amount of memory being mapped. The following
    // heuristic is applied:
    // If T number of pages are being mapped, then
    // t = (T / PTEs per page) page table pages are required.
    //
    // Number of Initial blocks =  t.
    //
    // N.B. The above value is only an estimate; the actual number of blocks
    //      used may vary (due to the additional firmware mappings).
    //

    PttPages =
        (ULONG)(ALIGN_RANGE_UP(TotalPages, PTE_PER_PAGE) / PTE_PER_PAGE);

    PptPages = 0;
    PxtPages = 0;
    PdtPages = 0;
    InitialPages = PttPages + PdtPages + PptPages + PxtPages;

    //
    // Since the InitialPages value is only an estimate, cap off the initial
    // number of pages at MM_APP_BLOCK_MEMORY_INITIAL_MAXIMUM to prevent over
    // consumption block memory resources. Initial estimate might fall
    // short of the minimum value as firmware page table mappings are not
    // accounted for.
    //

    if (InitialPages >= MM_APP_BLOCK_MEMORY_INITIAL_MAXIMUM) {
        *InitialBlocks = MM_APP_BLOCK_MEMORY_INITIAL_MAXIMUM;
        *DefaultBlocks = MM_APP_BLOCK_MEMORY_DEFAULT_HIGH;

    } else {
        if (InitialPages >= MM_APP_BLOCK_MEMORY_INITIAL_MINIMUM) {
            *InitialBlocks = InitialPages;

        } else {
            *InitialBlocks = MM_APP_BLOCK_MEMORY_INITIAL_MINIMUM;
        }

        *DefaultBlocks = MM_APP_BLOCK_MEMORY_DEFAULT_LOW;
    }

    return;
}

