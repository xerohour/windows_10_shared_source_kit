/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    procuarm.c

Abstract:

    This module implements ARM virtual address translation routines.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "procuarm.h"

// -------------------------------------------------------------------- Defines

#define KSEG_BIAS_LIMIT ((16 * 1024 * 1024) / PAGE_SIZE) // 16MB

#define ATTRIBUTE_CACHE_SIZE 40

// -------------------------------------------------------------------- Globals

ULONG_PTR MmArchKsegBase;
ULONG_PTR MmArchKsegBias;
ULONG_PTR MmArchBaseOfApplicationAddressSpace;
ULONG_PTR MmArchTopOfApplicationAddressSpace;

//
// Contains the attributes used for page tables for the WINLOAD
// application state.  This will be initialized by the WINLOAD
// application state construction code as it allocates page tables and
// again by WINLOAD itself.
//

NATIVE_MEMORY_ATTRIBUTES MmArmNativePageTableAttributes;

//
// Define the range of virtual addresses to be used for KSEG allocations.
//

ADDRESS_RANGE MmArchKsegAddressRange;

//
// ARM memory management considerations
// -------------------------------------
//
// The ARMv7-A architecture has some properties that complicate this code:
//    * Supports multiple page table formats
//    * Allows the core to be configured so as to remap some bits within
//      the PTEs so that others may be managed by the OS.
//    * Features a 16k physically contiguous and aligned page directory in
//      which the page directory entries describe only a 1k page table.
//    * PTEs and PDEs are not in the same format.
//
// Windows supports ARMv7-A in this way:
//     * The remap mechanism is used to to claw back PTE bits for use by MM.
//     * A software page directory is created in PTE format and modifications
//       to it are migrated to the hardware page directory.
//     * Entries in the hardware page directory are allocated 4 at a time such
//       that the 4 hardware PDEs point to sequential locations on the same
//       page.
//
// The UEFI-ARM binding specifies the stock ARMv7 page table format
// without remapping as any specific remapping would be OS specific.
// It does not requirements any specific allocation of hardware PDEs
// other than requiring a 1:1 virtual physical mapping. The
// specification limits PTEs to 4k pages.
//
// Specific SCTLR configuration is:
//    I=1, C=1, B=0, XP=1, TRE=0, M=1
//
// This is referred to in this code as the 'native' page table format.
//
// N.B. Maintaining the 'application context' using the OS PTE format
//      with its extensive remap of PTEs represents significant
//      problems chief of which are the lack of obvious 1:1 mappings
//      between the 31 memory types supported in the native format and
//      the 3 supported in the OS format. This creates the real
//      possibility of inconsistent cache/sharing attributes between
//      the firmware and application contexts.
//
// The 'application context' maintained by this code uses the native
// EFI page table format except that it adds a self-map if the
// translation type is not VA_TRANSLATION_TYPE_NONE.  When WINLOAD
// takes over the memory management wiht ExitBootServices(), these
// page tables are converted to the OS format.
//

// ----------------------------------------------------------------- Prototypes

NTSTATUS
MmArmBuildApplicationPageTables (
    __in PARM_MM_STATE MmState,
    __in ULONG Flags,
    __out PULONG SelfMapPdeBase
    );

NTSTATUS
MmArmFreeApplicationPageTables (
    __in PARM_MM_STATE MmState
    );

// ------------------------------------------------------------------ Functions

BOOLEAN
BlMmIsTranslationEnabled (
    VOID
    )

/*++

Routine Description:

    Determines if the current boot application is executing with address
    translation enabled.




    N.B. This routine cannot call BlpInterfaceEnter or BlpInterfaceExit.
         These routines can call out to this function, so calling back into
         them would trigger infinite recursion.

Arguments:

    None.

Return Value:

    TRUE if address translation is currently enabled.

    FALSE otherwise.

--*/

{

    //
    // Address translation status depends on the current execution context
    // for the boot application.
    //
    // N.B. Address translation is never really disabled because an
    //      ARM boot application always runs with the MMU enabled.
    //      Instead this routine returns whether non-identity mappings
    //      are supported for the current execution context or not.
    //

    return BlpArchIsTranslationEnabled();
}

VOID
MmArmSyncRangeToHPW (
    VOID
    )

/*++

Routine Description:

    Syncs the range of PTEs with the hardware page table walker.

Arguments:

    None.

Return Value:

    None.

--*/

{
    MI_ARM_SYNC_PTE_WRITE();
}

VOID
MmArmWriteNativePte (
    __in PULONG PointerPte,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in NATIVE_MEMORY_ATTRIBUTES Attributes
    )

/*++

Routine Description:

    Initializes the given page table entry for the given virtual address
    with the supplied physical page and memory attributes.

Arguments:

    PointerPte - Supplies a pointer to the PTE to be written

    PhysicalAddress - Supplies physical address of memory (or device) to be
        mapped at the virtual address.

    Attribute - Supplies the desired cache, sharing, and memory type of the
        mapping.

Return Value:

    None.

--*/

{
    ULONG PteContents;

    PteContents = NATIVE_PTE_TEMPLATE;

    PteContents |= (ULONG) PhysicalAddress.QuadPart & PAGE_PFN_MASK;

    //
    // Set the memory attribute for the mapping into the PTE.
    //

    PteContents |= Attributes;
    *PointerPte = PteContents;

    //
    // Synchronize PTE write with hardware page table walker
    //

    MI_ARM_SYNC_PTE_TO_HPW (PointerPte);
}

VOID
MmArmZeroNativePte (
    __in PULONG PointerPte
    )

/*++

Routine Description:

    Initializes the given page table entry for the given virtual address
    with the supplied physical page and cache attributes.

Arguments:

    PointerPte - Supplies a pointer to the PTE to be written

Return Value:

    None.

--*/

{
    *PointerPte = 0;

    //
    // Synchronize PTE write with hardware page table walker
    //

    MI_ARM_SYNC_PTE_TO_HPW (PointerPte);
}

VOID
MmArmWriteNativePde (
    __out PULONG PointerPde,
    __out PULONG HardwarePageDirectory,
    __in PHYSICAL_ADDRESS PhysicalAddress
   )

/*++

Routine Description:

    Initializes the given page directory entry for the given virtual address
    with the supplied physical page.

Arguments:

    PointerPde - Supplies a pointer to the PDE to be written

    PhysicalAddress - Supplies physical address of memory to become a page
        table.

Return Value:

    None.

--*/

{
    ULONG HardwarePdeContents;
    ULONG PdeContents;
    PULONG PointerHardwarePde;

    //
    // Build selfmap PTE
    //

    PdeContents = NATIVE_PTE_TEMPLATE;
    PdeContents |= (ULONG) PhysicalAddress.QuadPart & PAGE_PFN_MASK;
    PdeContents |= MmArmNativePageTableAttributes;

    //
    // Write selfmap PTE
    //

    *PointerPde = PdeContents;
    MI_ARM_SYNC_PTE_TO_HPW (PointerPde);

    //
    // Build Hardware PDE
    //

    HardwarePdeContents = NATIVE_PDE_TEMPLATE;
    HardwarePdeContents |= (ULONG) PhysicalAddress.QuadPart & PAGE_PFN_MASK;
    HardwarePdeContents |= NATIVE_PDE_NS_MASK;
    PointerHardwarePde = (PULONG) ((ULONG_PTR) HardwarePageDirectory +
                                   (((ULONG) PointerPde & (PAGE_SIZE - 1)) << 2));

    //
    // Write hardware PDE in application context style
    //

    *PointerHardwarePde = HardwarePdeContents;
    *(PointerHardwarePde + 1) = HardwarePdeContents | 0x400;
    *(PointerHardwarePde + 2) = HardwarePdeContents | 0x800;
    *(PointerHardwarePde + 3) = HardwarePdeContents | 0xC00;
    MI_ARM_SYNC_PTE_TO_HPW (PointerHardwarePde);
}

VOID
MmArmZeroNativePde (
    __out PULONG PointerPde,
    __out PULONG HardwarePageDirectory
    )
{

    PULONG PointerHardwarePde;

    //
    // Write selfmap PTE
    //

    *PointerPde = 0;
    MI_ARM_SYNC_PTE_TO_HPW (PointerPde);

    PointerHardwarePde = (PULONG) ((ULONG_PTR) HardwarePageDirectory +
                                   (((ULONG) PointerPde & (PAGE_SIZE - 1)) << 2));

    //
    // Write hardware PDE in application context style
    //

    *PointerHardwarePde = 0;
    *(PointerHardwarePde + 1) = 0;
    *(PointerHardwarePde + 2) = 0;
    *(PointerHardwarePde + 3) = 0;
    MI_ARM_SYNC_PTE_TO_HPW (PointerHardwarePde);
}

NTSTATUS
BlMmArmGetNativeAttributes(
    __in ULONG Address,
    __out_opt PULONG PARResult
    )
{

    ARM_PAR PARValue;

    _MoveToCoprocessor(Address, CP15_ATS1CPR);
    _InstructionSynchronizationBarrier();
    PARValue.Ulong = _MoveFromCoprocessor(CP15_PAR);

    if (PARValue.Aborted != 0) {
        return STATUS_UNSUCCESSFUL;
    }

    if (ARGUMENT_PRESENT(PARResult)) {
        *PARResult = PARValue.Ulong;
    }

    return STATUS_SUCCESS;
}

ULONG
MmArmMapAttributesToNativePte (
    ULONG PARAttributes
    )
{

    ULONG InnerCacheable;
    ARM_PAR ParValue;
    ULONG Pte;
    ULONG OuterCacheable;
    BOOLEAN Device;
    BOOLEAN StronglyOrdered;

    Device = FALSE;
    StronglyOrdered = FALSE;
    InnerCacheable = 0;
    OuterCacheable = 0;

    ParValue.Ulong = PARAttributes;

    switch(ParValue.OuterAttrib) {
    case 0:
        OuterCacheable = NATIVE_PTE_OUTER_NC;
        break;
    case 1:
        OuterCacheable = NATIVE_PTE_OUTER_WBWA;
        break;
    case 2:
        OuterCacheable = NATIVE_PTE_OUTER_WT;
        break;
    case 3:
        OuterCacheable = NATIVE_PTE_OUTER_WB;
        break;
    }

    switch(ParValue.InnerAttrib) {
    case 0:
        InnerCacheable = NATIVE_PTE_INNER_NC;
        break;
    case 1:
        StronglyOrdered = TRUE;
        break;
    case 3:
        Device = TRUE;
        break;
    case 5:
        InnerCacheable = NATIVE_PTE_INNER_WBWA;
        break;
    case 6:
        InnerCacheable = NATIVE_PTE_INNER_WT;
        break;
    case 7:
        InnerCacheable = NATIVE_PTE_INNER_WB;
        break;
    }

    if (StronglyOrdered != FALSE) {
        Pte = NATIVE_PTE_SO;
    } else if (Device != FALSE) {
        if (ParValue.InnerShared) {
            Pte = NATIVE_PTE_DEVICE_SHARED;
        } else {
            Pte = NATIVE_PTE_DEVICE_UNSHARED;
        }
    } else {

        Pte = NATIVE_PTE_NORMAL | OuterCacheable | InnerCacheable;
        if (ParValue.InnerShared) {
            Pte |= NATIVE_PTE_SHAREABLE_MASK;
        }
    }

    return Pte;
}

NATIVE_MEMORY_ATTRIBUTES
MmArmMapAttributes (
    ULONG CacheAttributes
    )
{

    NATIVE_MEMORY_ATTRIBUTES NativeMemoryAttributes;

    if (((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_UNCACHED) != 0) ||
        ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_WRITE_THROUGH) != 0) ||
        ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_DMA) != 0)) {

        NativeMemoryAttributes = NATIVE_PTE_SO;
    } else if ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_WRITE_COMBINED) != 0) {
        NativeMemoryAttributes = NATIVE_PTE_NORMAL | NATIVE_PTE_INNER_NC | NATIVE_PTE_OUTER_NC;
    } else {
        NativeMemoryAttributes = NATIVE_PTE_NORMAL | NATIVE_PTE_INNER_WBWA | NATIVE_PTE_OUTER_WBWA;
    }

    return NativeMemoryAttributes;
}

ULONG
MmArmDeterminePageTableAttributes (
    VOID
    )
{

    ULONG TTB_Config;
    ULONG Value;

    Value = _MoveFromCoprocessor(CP15_TTBR0);
    TTB_Config = Value & CP15_TTBRx_VALID_MPEXT_MASK;
    if ((TTB_Config &
         (CP15_TTBRx_MPEXT_INNER_CACHED | CP15_TTBRx_MPEXT_OUTER_CACHED)) != 0) {
        return MEMORY_ATTRIBUTE_CACHE_WRITE_BACK;

    } else {
        return MEMORY_ATTRIBUTE_CACHE_UNCACHED;
    }
}

VOID
MmArmSetNativePageTableAttributes (
    __in ULONG NativeMemoryAttributes
    )
{
    MmArmNativePageTableAttributes = NativeMemoryAttributes;
}

ULONG
MmArmComputeNativeAtributeIndex (
    ULONG NativeMemoryAttributes
    )
{

    ULONG Index;

    Index = 0;
    Index |= (NativeMemoryAttributes & NATIVE_PTE_B_MASK ? 1 : 0);
    Index |= (NativeMemoryAttributes & NATIVE_PTE_C_MASK ? 2 : 0);
    Index |= (NativeMemoryAttributes & NATIVE_PTE_TEX0_MASK ? 4 : 0);
    Index |= (NativeMemoryAttributes & NATIVE_PTE_TEX1_MASK ? 8 : 0);
    Index |= (NativeMemoryAttributes & NATIVE_PTE_TEX2_MASK ? 16 : 0);

    return Index;
}

ULONG
MmArmMapNativeMemoryAttributesToTtbConfig (
    __in ULONG NativeMemoryAttributes
    )
{

    ULONG Index;
    ULONG TTBConfig;

    const ULONG NativeAttributeToTTBConfig[32] = {
        0,         // Storngly-ordered
        0,         // Sharable Device
        CP15_TTBRx_MPEXT_INNER_WT   | CP15_TTBRx_MPEXT_OUTER_WT,
        CP15_TTBRx_MPEXT_INNER_WB   | CP15_TTBRx_MPEXT_OUTER_WB,
        CP15_TTBRx_MPEXT_INNER_NC   | CP15_TTBRx_MPEXT_OUTER_NC,
        0,
        0,
        CP15_TTBRx_MPEXT_INNER_WBWA | CP15_TTBRx_MPEXT_OUTER_WBWA,
        0,         // Non-shareable device
        0,         // Reserved
        0,         // Reserved
        0,         // Reserved
        0,         // Reserved
        0,         // Reserved
        0,         // Reserved
        0,         // Reserved
        CP15_TTBRx_MPEXT_INNER_NC   | CP15_TTBRx_MPEXT_OUTER_NC,
        CP15_TTBRx_MPEXT_INNER_WBWA | CP15_TTBRx_MPEXT_OUTER_NC,
        CP15_TTBRx_MPEXT_INNER_WT   | CP15_TTBRx_MPEXT_OUTER_NC,
        CP15_TTBRx_MPEXT_INNER_WB   | CP15_TTBRx_MPEXT_OUTER_NC,
        CP15_TTBRx_MPEXT_INNER_NC   | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WBWA | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WT   | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WB   | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_NC   | CP15_TTBRx_MPEXT_OUTER_WT,
        CP15_TTBRx_MPEXT_INNER_WBWA | CP15_TTBRx_MPEXT_OUTER_WT,
        CP15_TTBRx_MPEXT_INNER_WT   | CP15_TTBRx_MPEXT_OUTER_WT,
        CP15_TTBRx_MPEXT_INNER_WB   | CP15_TTBRx_MPEXT_OUTER_WT,
        CP15_TTBRx_MPEXT_INNER_NC   | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WBWA | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WT   | CP15_TTBRx_MPEXT_OUTER_WBWA,
        CP15_TTBRx_MPEXT_INNER_WB   | CP15_TTBRx_MPEXT_OUTER_WBWA
    };

    Index = MmArmComputeNativeAtributeIndex(NativeMemoryAttributes);
    TTBConfig = NativeAttributeToTTBConfig[Index];

    if ((NativeMemoryAttributes & NATIVE_PTE_SHAREABLE_MASK) != 0) {
        TTBConfig |= CP15_TTBRx_INNER_SHARED;
    }

    return TTBConfig;
}

NTSTATUS
MmArmDetermineMatchingMemoryAttributes (
    __in PHYSICAL_ADDRESS Address,
    __out PNATIVE_MEMORY_ATTRIBUTES Attributes
    )

/*++

Routine Description:

    This routine returns native memory attributes for a physical address.
    It is called when mapping pages into the application address space
    to ensure that memory attributes are compatible with already existing
    mapping.

Arguments:

    Address - The physical address.

    Attributes - Returns a memory attributes value.

Return Value:

    STATUS_SUCCESS when successful, STATUS_UNSUCCESSFUL otherwise.

--*/

{
    EXECUTION_CONTEXT_TYPE ContextType;
    ULONG NativeAttribute;
    NTSTATUS Status;

    NT_ASSERT(0 == Address.HighPart);

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    Status = BlMmArmGetNativeAttributes(Address.LowPart, &NativeAttribute);

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextApplication);
    }

    if (NT_SUCCESS(Status)) {
        *Attributes = MmArmMapAttributesToNativePte(NativeAttribute);
    }

    return Status;
}

NTSTATUS
MmArchInitialize (
    __in ULONG Phase,
    __in PALLOCATED_MEMORY AllocatedMemory,
    __in VA_TRANSLATION_TYPE CurrentTranslationType,
    __in VA_TRANSLATION_TYPE ApplicationTranslationType
    )

/*++

Routine Description:

    Phase 0 : No-op

    Phase 1 : Initializes the translation structures required for the specified
        type of virtual address translation.

    Phase 2: No-op

    This routine is hardware specific for a ARM processor executing with the
    MMU enabled.

    N.B.  This module is built for a ARM processor with MMU enabled.

Arguments:

    AllocatedMemory - Data structure describing memory currently allocated
        by previous boot applications.

    CurrentTranslationType - The address translation mode of the calling
        boot application.  If this value is not VA_TRANSLATION_TYPE_NONE, then
        the required paging structure exists from the previous boot
        application and virtual address translation was enabled at the time
        execution was passed to this executing boot application's entry point.

    ApplicationTranslationType - The type of virtual address translation
        the boot application should use.  On x86-64, this value must be
        VA_TRANSLATION_TYPE_DEFAULT.

Return Value:

    STATUS_SUCCESS when successful.

    Appropriate NTSTATUS code otherwise.

--*/

{

    ULONGLONG BasePage;
    ULONGLONG IncreaseUserVa;
    ULONGLONG PageCount;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(AllocatedMemory);
    UNREFERENCED_PARAMETER(CurrentTranslationType);
    UNREFERENCED_PARAMETER(ApplicationTranslationType);

    if (Phase == 1) {

        //
        // Determine the base address of the kernel address space.
        // In most cases this is KSEG0_BASE.  But if the application's
        // options indicate that user VA space is to be increased,
        // there is up to 3GB of usermode address space and kernel
        // address space starts at ALTERNATE_BASE.
        //

        Status = BlGetApplicationOptionInteger(
                     BCDE_OSLOADER_TYPE_INCREASE_USER_VA,
                     &IncreaseUserVa);

        if (NT_SUCCESS(Status) && (IncreaseUserVa != 0)) {
            MmArchKsegBase = ALTERNATE_BASE;

        } else {
            MmArchKsegBase = KSEG0_BASE;
        }

        //
        // FW-kejohns-20111029- BlArchGetPerformanceCounter() is (quite) far
        //                      from ideal here on ARM and should be replaced.
        //

        MmArchKsegBias =
            (RotateLeft32((ULONG)BlArchGetPerformanceCounter(), 5) % KSEG_BIAS_LIMIT) << PAGE_SHIFT;

        MmArchKsegBase += MmArchKsegBias;
        MmArchKsegAddressRange.Minimum = MmArchKsegBase;
        MmArchKsegAddressRange.Maximum = MAXULONGLONG;
        MmArchBaseOfApplicationAddressSpace = 0;
        MmArchTopOfApplicationAddressSpace = KSEG0_BASE - 0x10000000;

        //
        // Reserve the virtual address space used by the self-map. Reserve the
        // VAs for both the fixed kernel self-map and the dynamic self-map if
        // they are different.
        //

        BasePage = PTE_BASE >> PAGE_SHIFT;
        PageCount = (PTE_TOP - PTE_BASE + 1) >> PAGE_SHIFT;
        Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                         BasePage,
                                         PageCount,
                                         MDL_FLAGS_VIRTUAL_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            goto MmArchInitializeExit;
        }

        if (BlMmPteBase != PTE_BASE) {
            BasePage = BlMmPteBase >> PAGE_SHIFT;
            PageCount = (PTE_TOP - PTE_BASE + 1) >> PAGE_SHIFT;
            Status = MmMdRemoveRegionFromMdl(&MmMdlFreeVirtual,
                                             BasePage,
                                             PageCount,
                                             MDL_FLAGS_VIRTUAL_ADDRESS);

            if (!NT_SUCCESS(Status)) {
                goto MmArchInitializeExit;
            }
        }

        BlMmRelocateSelfMap = MmArmRelocateSelfMap;
        BlMmZeroVirtualAddressRange = MmArmZeroVirtualAddressRange;
        BlMmMoveVirtualAddressRange = MmArmMoveVirtualAddressRange;
        BlMmFlushTlb = MmArchFlushTlb;

    } else {

        NT_ASSERT((Phase == 0) || (Phase == 2));

        Status = STATUS_SUCCESS;
    }

MmArchInitializeExit:
    return Status;
}

NTSTATUS
MmArchDestroy (
    VOID
    )

/*++

Routine Description:

    The destruction routine for the architecture specific address translation
    code.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{

    return STATUS_SUCCESS;
}

BOOLEAN
MmArchTranslateVirtualAddress (
    __in PVOID VirtualAddress,
    __out_opt PPHYSICAL_ADDRESS PhysicalAddress,
    __out_opt PULONG CacheAttributes
    )

/*++

Routine Description:

    Determines if a linear address is virtually mapped.  If mapped,
    this routine will return the physical address mapped.

Arguments:

    LinearAddress - Supplies the linear address to check.

    PhysicalAddress - Supplies a pointer to a variable that receives the
        physical address mapped by VirtualAddress.

    CacheAttributes - Supplies a pointer to a variable that receives the cache
        attributes of the existing mapping at the linear address.

        N.B. This parameter is not supported on ARM.

Return Value:

    TRUE if VirtualAddress is mapped.

    FALSE otherwise.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    BOOLEAN IsFirmwareContext;
    PARM_MM_STATE MmState;
    ULONG Offset;
    PULONG PageTable;
    PULONG Pde;
    PULONG Pte;

    //
    // The CacheAttributes parameter is not supported on ARM as its a
    // lossy representation of the underlying memory attribute.
    //

    if (ARGUMENT_PRESENT(CacheAttributes)) {
        *CacheAttributes = MEMORY_ATTRIBUTE_CACHE_INDIFFERENT;
    }

    //
    // N.B. This routine has dubious semantics that seem to imply a
    //      lot more assumptions about the caller's state and intent
    //      then are baked into the arguments.
    //

    //
    // If the boot application was not initialized for using its own page
    // tables, then simply return the physical address.
    //

    if (BlGetCurrentLibraryParameters().TranslationType == VA_TRANSLATION_TYPE_NONE) {

        //
        // Ensure that the physical page exists in the memory map.
        //

        Descriptor = MmMdFindDescriptor((ULONG_PTR)VirtualAddress >> PAGE_SHIFT,
                                        MEMORYMAP_TYPE_ALL,
                                        MDL_FLAGS_PHYSICAL_ADDRESS);

        if (ARGUMENT_PRESENT(PhysicalAddress)) {
            PhysicalAddress->QuadPart = (ULONG_PTR)VirtualAddress;
        }

        if (Descriptor != NULL) {
            return TRUE;
        }

        return FALSE;
    }

    //
    // If this routine is called in the firmware's context, then it will
    // manually walk the application's page tables to translate the
    // address. Otherwise, the self-map pointers are valid and are used.
    //

    if (BlpArchQueryCurrentContextType() == ExecutionContextFirmware) {
        IsFirmwareContext = TRUE;

        MmState = BlpArchGetMmState(ExecutionContextApplication);
        PageTable = (PULONG)MmState->SoftwarePageDirectory;
        if (PageTable == NULL) {
            return FALSE;
        }

    } else {
        IsFirmwareContext = FALSE;
        PageTable = NULL;
    }

    if (IsFirmwareContext != FALSE) {
        Pde = &PageTable[MiGetNativePdeOffset(VirtualAddress)];

    } else {
        Pde = MiGetNativePdeAddress(VirtualAddress);
    }

    //
    // ARM self-map PDEs are actually PTEs
    //

    if (MI_IS_NATIVE_PTE_VALID(Pde) != TRUE) {
        return FALSE;
    }

    //
    // For regular pages, compute the PTE address and check if it is valid.
    //

    if (IsFirmwareContext != FALSE) {
        PageTable = (PULONG) NATIVE_PTE_TO_PA(Pde);
        Pte = &PageTable[MiGetNativePteOffset(VirtualAddress)];

    } else {
        Pte = MiGetNativePteAddress(VirtualAddress);
    }

    Offset = (ULONG)((UINTN)VirtualAddress & (PAGE_SIZE - 1));

    if (MI_IS_NATIVE_PTE_VALID(Pte) == FALSE) {
        return FALSE;
    }

    if (ARGUMENT_PRESENT(PhysicalAddress) != FALSE) {
        PhysicalAddress->QuadPart = NATIVE_PTE_TO_PA(Pte) + Offset;
    }

    return TRUE;
}

ULONG
MmArchQueryLargePageSize (
    VOID
    )

/*++

Routine Description:

    Returns the number of pages in a large page.

Arguments:

    None.

Return Value:

    The number of pages in a large page.

--*/

{

    return (PAGE_SIZE / sizeof(HARDWARE_PTE));
}


NTSTATUS
MmArchMapPhysicalAddress (
    __in PHYSICAL_ADDRESS PhysicalStartAddress,
    __in_bcount(Size) PVOID VirtualStartAddress,
    __in ULONGLONG Size,
    __in ULONG CacheAttribute
    )
/*++

Routine Description:

    This routine will map the specified physical address range using the
    prescribed virtual address range.

Arguments:

    VirtualStartAddress - Starting address of the virtual address range to be
        mapped to the specified physical address range.  Must be page aligned.

    PhysicalStartAddress - Starting address of the physical address range
        being mapped.  Must be page aligned.

        If PhysicalStartAddress == MM_PFN_NO_MAP, then the paging structures
        will be allocated for the virtual address mapping, but the PTE
        will not be initialized for any PFN.

    Size - The size of the address range to map.

    CacheAttribute - Supplies the cache attribute to apply to the mapping.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for a page table page allocation.

--*/

{

    PHYSICAL_ADDRESS Address;
    ULONG Index;
    NATIVE_MEMORY_ATTRIBUTES NativeMemoryAttributes;
    PARM_MM_STATE MmState;
    ULONGLONG NumPages;
    PULONG PageTable;
    PHYSICAL_ADDRESS PageTableArray[1];
    ULONG PagesToAllocate;
    PULONG Pde;
    ULONGLONG PhysicalStartPage;
    PULONG Pte;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // If mappings are permitted, then translation should be enabled on ARM
    // and the application should be in the application's context.
    //

    if (BlMmIsTranslationEnabled() == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    ASSERT(BlpArchQueryCurrentContextType() != ExecutionContextFirmware);
    MmState = BlpArchGetMmState(ExecutionContextApplication);

    //
    // Input addresses are assumed to be page aligned.
    //

    ASSERT(((ULONG64)VirtualStartAddress  & (PAGE_SIZE - 1)) == 0);

    //
    // Pacify the compiler.
    //

    PageTable = NULL;
    Pde = NULL;
    Pte = NULL;

    //
    // Map the entire virtual address range to the specified physical pages.
    //

    VirtualAddress = VirtualStartAddress;
    PhysicalStartPage = PhysicalStartAddress.QuadPart >> PAGE_SHIFT;
    NumPages = Size >> PAGE_SHIFT;
    for (Index = 0;
         Index < NumPages;
         Index += 1, VirtualAddress = Add2Ptr(VirtualAddress, PAGE_SIZE)) {

        //
        // Skip over all ranges that are currently mapped.
        //
        // N.B. MmArchMapPhysicalAddress ensured that if a mapping existed,
        //      it was a mapping to the desired PFN.
        //

        if (MmArchTranslateVirtualAddress(VirtualAddress,
                                          NULL,
                                          NULL) != FALSE) {

            continue;
        }

        PagesToAllocate = 0;

        //
        // Obtain pointers to each level of the paging structures and
        // calculate the number of page tables to be allocated.
        //

        //
        // Compute the PDE address and check if it is valid.
        //

        Pde = MiGetNativePdeAddress(VirtualAddress);
        if (MI_IS_NATIVE_PDE_VALID(Pde) == FALSE) {
            PagesToAllocate = 1;
            goto AllocatePageTables;
        }

        Pte = MiGetNativePteAddress(VirtualAddress);

        //
        // Determine how many page tables need to be allocated to create
        // the mapping.
        //

AllocatePageTables:

        //
        // If any page table was invalid, allocate and map the table.
        //

        if (PagesToAllocate != 0) {

            //
            // N.B. Need hyperspace like construct to do this safely.
            //

            // XXX have to ensure these are mapped with an appropriate
            // TTB CR compatibile cache attribute.
            // XXXX MmArmSyncRangeToHPW(PageTable, NATIVE_PAGE_TABLE_SIZE);

            Status = MmArmAllocatePageTables(PageTableArray,
                                             PagesToAllocate);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            //
            // TODO: The following TLB flushes are most likely not necessary
            //       since these pages should have been invalidated if they
            //       were previously unmapped.  Early in a ship cycle, these
            //       should probably be changed to full memory barriers to
            //       ensure that the P[XPD]E's are setup prior to the access
            //       attempts that follow which zero out the newly allocated
            //       pages.
            //

            switch (PagesToAllocate) {
            case 1:

                //
                // A page table page was allocated and needs to be
                // initialized.
                //

                //
                // Initialize PDE for the newly allocated page table
                // page.
                //
                // N.B. hyperspace needed here.
                //

                MmArmWriteNativePde(Pde,
                                    MmState->MappedHardwarePageDirectory,
                                    PageTableArray[0]);

                //
                // Ensure the page is mapped before referencing.
                //

                MmArchFlushTlb();

                //
                // Reference the newly allocated page table page and zero it.
                //

                PageTable = MiGetNativePteAddress((ULONG_PTR)VirtualAddress & ~(NATIVE_LARGE_PAGE_SIZE-1));
                RtlZeroMemory(PageTable, PAGE_SIZE);
                MmArmSyncRangeToHPW();
                MmArchFlushTlb();

                Pte = MiGetNativePteAddress(VirtualAddress);
                break;

            default:
                break;
            }
        }

        //
        // If the physical address is equal to MM_PFN_NO_MAP, then skip
        // the final step of initializing the PTE.
        //

        if (PhysicalStartAddress.QuadPart != MM_PFN_NO_MAP) {

            //
            // Set the PTE for the specified physical page.
            //

            Address.QuadPart =
                PhysicalStartAddress.QuadPart + Index * PAGE_SIZE;

            Status = MmArmDetermineMatchingMemoryAttributes(Address,
                                                            &NativeMemoryAttributes);
            if (!NT_SUCCESS(Status)) {
                NativeMemoryAttributes = MmArmMapAttributes(CacheAttribute);
            }

            MmArmWriteNativePte(Pte, Address, NativeMemoryAttributes);
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
MmArchUnmapVirtualAddress (
    __in_bcount(Size) PVOID VirtualStartAddress,
    __in ULONGLONG Size
    )

/*++

Routine Description:

    This routine will unmap the specified virtual address range, invalidating
    all PTEs for the specified virtual address range.

Arguments:

    VirtualStartAddress - Starting address of the virtual address range to be
        unmapped.  Must be page aligned.

    Size - The size of the address range to unmap.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    ULONG Index;
    ULONGLONG NumPages;
    PULONG Pde;
    PULONG Pte;
    PVOID VirtualAddress;

    //
    // If unmappings are permitted, then translation should be enabled on ARM
    // and the application should be in the application's context.
    //

    if (BlMmIsTranslationEnabled() == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    ASSERT(BlpArchQueryCurrentContextType() != ExecutionContextFirmware);

    //
    // Input addresses are assumed to be page aligned.
    //

    ASSERT(((ULONG64)VirtualStartAddress  & (PAGE_SIZE - 1)) == 0);

    //
    // Pacify the compiler.
    //

    Pde = NULL;
    Pte = NULL;

    //
    // Invalidate the entire virtual address range.
    //

    VirtualAddress = VirtualStartAddress;
    NumPages = Size >> PAGE_SHIFT;
    for (Index = 0; Index < NumPages;
         Index += 1, VirtualAddress = Add2Ptr(VirtualAddress, PAGE_SIZE)) {

        //
        // If address translation is enabled, the self-map pointers to each
        // page table can be used.
        //

        //
        // Compute the PDE address and check if it is valid. If it is invalid,
        // try to invalidate the page tables starting at the third level.
        //

        Pde = MiGetPdeAddress(VirtualAddress);

        if (MI_IS_NATIVE_PDE_VALID(Pde) == FALSE) {
            continue;
        }

        //
        // Compute the PTE address and check if it is mapped.
        //

        Pte = MiGetPteAddress(VirtualAddress);

        //
        // If the virtual address is mapped, invalidate the PTE and flush TLB.
        //

        if (MI_IS_NATIVE_PTE_VALID(Pte) != FALSE) {
            MmArmZeroNativePte(Pte);
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
MmArchRemapVirtualAddress (
    __in_bcount(Size) PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG CacheAttribute
    )

/*++

Routine Description:

    This routine will remap the specified virtual address range using the
    prescribed physical address range.

Arguments:

    VirtualAddress - Supplies the starting address of the virtual address
        range to be remapped to the specified physical address range.  Must be
        page aligned.

    PhysicalAddress - Supplies the starting address of the physical address
        range being mapped. Must be page aligned.

    Size - Supplies the size of the address range to remap, in bytes.

    CacheAttribute - Supplies the cache attribute to apply to the remapping.

Return Value:

    NTSTATUS Code.

--*/

{

    PHYSICAL_ADDRESS Address;
    ULONG Index;
    NATIVE_MEMORY_ATTRIBUTES NativeMemoryAttributes;
    ULONGLONG NumPages;
    ULONGLONG PhysicalStartPage;
    PULONG Pte;
    NTSTATUS Status;

    //
    // If mappings are permitted, then translation should be enabled on x64
    // and the application should be in the application's context.
    //

    if (BlMmIsTranslationEnabled() == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    ASSERT(BlpArchQueryCurrentContextType() != ExecutionContextFirmware);

    //
    // MM_PFN_NO_MAP is not a valid physical address for this routine.
    //

    if (PhysicalAddress.QuadPart == MM_PFN_NO_MAP) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Input addresses are assumed to be page aligned.
    //

    ASSERT(((ULONG)VirtualAddress  & (PAGE_SIZE - 1)) == 0);

    //
    // Pacify the compiler.
    //

    Pte = NULL;

    //
    // Map the entire virtual address range to the specified physical pages.
    //

    PhysicalStartPage = PhysicalAddress.QuadPart >> PAGE_SHIFT;
    NumPages = Size >> PAGE_SHIFT;
    for (Index = 0;
         Index < NumPages;
         Index += 1, VirtualAddress = Add2Ptr(VirtualAddress, PAGE_SIZE)) {

        Pte = MiGetPteAddress(VirtualAddress);

        //
        // Assert that the PTE exists.
        //

        ASSERT(Pte != NULL);

        //
        // Zero the PTE.
        //

        MmArmZeroNativePte(Pte);

        //
        // Set the PTE to the new physical page.
        //

        Address.QuadPart = ((PhysicalStartPage + Index) << PAGE_SHIFT);
        Status = MmArmDetermineMatchingMemoryAttributes(Address,
                                                        &NativeMemoryAttributes);
        if (!NT_SUCCESS(Status)) {
            NativeMemoryAttributes = MmArmMapAttributes(CacheAttribute);
        }

        MmArmWriteNativePte(Pte, Address, NativeMemoryAttributes);
    }

    MmArchFlushTlb();
    return STATUS_SUCCESS;
}

NTSTATUS
MmArchSetPageProtection (
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

    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Protection);
    UNREFERENCED_PARAMETER(Flags);

    //
    // TODO: Implement.
    //

    return STATUS_NOT_SUPPORTED;
}

VOID
MmArchFlushTlb (
    VOID
    )

/*++

Routine Description:

    Flush the translation lookaside buffer.

Arguments:

    None.

Return Value:

    None.

--*/

{
    _MoveToCoprocessor(0, CP15_TLBIALL);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    return;
}

NTSTATUS
MmArmAllocatePageTables (
    __inout_ecount(PagesToAllocate) PPHYSICAL_ADDRESS AllocatedPageTables,
    __in ULONG PagesToAllocate
    )

/*++

Routine Description:

    Allocates page table pages.  The allocated pages may not be contiguous.

Arguments:

    AllocatedPageTables - Supplies a pointer to a buffer which receives the
        physical address of the allocated page tables.

    PagesToAllocate - Supplies the number of page table pages to allocate.

Return Value:

    STATUS_SUCCESSFUL when successful.

    STATUS_NO_MEMORY when allocation fails.

--*/

{

    ULONG Index;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    ULONG MemoryAttributes;

    MemoryAttributes = MmArmDeterminePageTableAttributes();

    //
    // Attempt to allocate the page tables in a single allocation to minimize
    // calls into the library.  If this fails, break up the allocation into
    // single page allocations.
    //
    // When allocating a single page allocation, do not attempt to coalesce with
    // adjacent page table allocations.  This is the one location in this module
    // which attempts to free a page table allocation and care must be made to
    // not free adjacent coalesced allocations.
    //

    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       PagesToAllocate,
                                       MEMORY_TYPE_PAGE_TABLE,
                                       MEMORY_ATTRIBUTE_DESCRIPTION_COALESCE | MemoryAttributes,
                                       0);  // No alignment requirement.

    if (NT_SUCCESS(Status)) {
        for (Index = 0; Index < PagesToAllocate; Index += 1) {
            AllocatedPageTables[Index] = PhysicalAddress;
            PhysicalAddress.QuadPart += PAGE_SIZE;
        }

    } else {
        for (Index = 0; Index < PagesToAllocate; Index += 1) {
            Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                               1,
                                               MEMORY_TYPE_PAGE_TABLE,
                                               MemoryAttributes,
                                               0); // No alignment requirement.

            if (!NT_SUCCESS(Status)) {
                while (Index != 0) {
                    PhysicalAddress = AllocatedPageTables[Index - 1];
                    BlMmFreePhysicalPages(PhysicalAddress);
                    Index -= 1;
                }

                return STATUS_NO_MEMORY;
            }

            AllocatedPageTables[Index] = PhysicalAddress;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
BlpMmArmBuildApplicationMmState (
    __out PARM_MM_STATE MmState,
    __in ULONG Flags,
    __out PULONG SelfMapPdeBase
    )
{

    NTSTATUS Status;

    //
    // Capture the MM state used by the firmware.
    //

    BlpMmArmCaptureCurrentMmState(MmState);

    //
    // These parameters will be replaced by the new page tables
    //

    MmState->MappedHardwarePageDirectory = NULL;
    MmState->HardwarePageDirectory = 0;
    MmState->SoftwarePageDirectory = 0;

    //
    // Build the new page ables and fill in state.
    //

    Status = MmArmBuildApplicationPageTables(MmState, Flags, SelfMapPdeBase);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
BlpMmArmFreeApplicationMmState (
    __out PARM_MM_STATE MmState
    )
{

    NTSTATUS Status;

    Status = MmArmFreeApplicationPageTables(MmState);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    return STATUS_SUCCESS;
}

VOID
BlpMmArmCaptureCurrentMmState (
    PARM_MM_STATE MmState
    )
{
    BOOLEAN MpExtensions;
    ULONG Value;

    //
    // Save hardware MMU state
    //

    MpExtensions =
        (READ_ARM_FEATURE(CP15_MMFR2, MMFR2_UTLB) >= MMFR2_UTLB_VA_ALL) &&
        (READ_ARM_FEATURE(CP15_MMFR3, MMFR3_BCAST) >= MMFR3_BCAST_ALL) &&
        (READ_ARM_FEATURE(CP15_MMFR3, MMFR3_HPW) >= MMFR3_HPW_COHERENT);
    MmState->MpExtensions = MpExtensions;
    Value = _MoveFromCoprocessor(CP15_TTBR0);
    MmState->HardwarePageDirectory = Value & CP15_TTBRx_PD_MASK;
    if (MpExtensions != FALSE) {
        MmState->TTB_Config = Value & CP15_TTBRx_VALID_MPEXT_MASK;
    } else {

        MmState->TTB_Config = Value & CP15_TTBRx_VALID_NOMP_MASK;
    }

    MmState->MappedHardwarePageDirectory = NULL;
    MmState->SoftwarePageDirectory = 0;
}

VOID
BlpMmArmCaptureApplicationMmState (
    PARM_MM_STATE MmState
    )
{

    ULONGLONG DynamicPdeBase;
    ULONG NativeMemoryAttributes;
    PULONG PointerPte;
    NTSTATUS Status;

    //
    // Set the dynamic PDE base.
    //

    Status = BlGetApplicationOptionInteger(BCDE_LIBRARY_TYPE_SELF_MAP_PDE_BASE,
                                           &DynamicPdeBase);

    ASSERT(NT_SUCCESS(Status));

    MmPdeBase = (ULONG)DynamicPdeBase;
    BlMmPteBase = MiGeneratePteBase(MmPdeBase);

    //
    // Determine what attributes should be used for mapping page
    // tables by seeing what attributes is already in use for the page
    // directory mapping.
    //

    PointerPte = MiGetNativePteAddress(MmPdeBase);
    NativeMemoryAttributes =
        *PointerPte & (NATIVE_PTE_MEMORY_TYPE_MASK | NATIVE_PTE_SHAREABLE_MASK);

    MmArmSetNativePageTableAttributes(NativeMemoryAttributes);

    //
    // Capture the directly observable MM state
    //

    BlpMmArmCaptureCurrentMmState(MmState);

    //
    // Capture data specific to the application state. The hardware page
    // directory was mapped 1:1 upon creation.
    //

    MmState->MappedHardwarePageDirectory =
        (PULONG)MmState->HardwarePageDirectory;

    MmState->SoftwarePageDirectory = NATIVE_PTE_TO_PA(PointerPte);
}

VOID
FORCEINLINE
MiWritebackInvalidateDcacheV7BySetWay(
    ULONG FlushLevel,
    BOOLEAN InvalidateOnly
    )
/*++

Routine Description:

    This function writes back and invalidates the data/unified caches
    until the specified cache level.

Arguments:

    FlushLevel - Write back and invalidate all data/unified cache
       levels up to this level.

    InvalidateOnly - Indicate the operation is invalidation only.

Return Value:

    None.

Note:

    This routine can be called at high IRQL.

--*/
{
    ARM_CACHE_INFO CacheInfo;
    ARM_CACHE_INFO_SELECTOR CacheSelector;
    ULONG Level;
    ULONG OpValue;
    ULONG Set;
    ULONG SetShift;
    ULONG Value;
    ULONG Way;
    ULONG WayShift;

    Level = 0;
    while (Level < FlushLevel) {

        CacheSelector.Ulong = 0;
        CacheSelector.Level = Level;
        CacheSelector.Select = ARM_CACHE_SELECT_DATA_OR_UNIFIED;

        _MoveToCoprocessor(CacheSelector.Ulong, CP15_CSSELR);
        _InstructionSynchronizationBarrier();

        CacheInfo.Ulong = _MoveFromCoprocessor(CP15_CCSIDR);

        Way = CacheInfo.Associativity + 1;

        //
        // Cache operation register format
        //
        // 31 ----- 32-A 31-A ------ L+S L+S-1 --- L  L-1 ---- 4 3 Level 1 SBZ
        //     WAY             SBZ             SET        SBZ      Level
        // where A = log2n # of ways
        //       L = log2n line size
        //       S = log2n # of sets

        Value = Way;
        WayShift = 0;
        while (Value >>= 1) {
            WayShift += 1;
        }

        if (Way & (Way - 1)) {
            WayShift += 1;
        }

        WayShift = 32 - WayShift;
        SetShift = CacheInfo.LineSize + 4;

        do {

            Way -= 1;
            Set = CacheInfo.NumberOfSets + 1;

            do {
                Set -= 1;

                OpValue = (Level << 1)      |
                          (Way << WayShift) |
                          (Set << SetShift);

                if (InvalidateOnly != FALSE) {
                    _MoveToCoprocessor(OpValue, CP15_DCISW);

                } else {
                    _MoveToCoprocessor(OpValue, CP15_DCCISW);
                }
            } while (Set != 0);
        } while (Way != 0);

        Level += 1;
    }
}

VOID
MiWritebackInvalidateCurrentCache (
    BOOLEAN InvalidateOnly
    )
/*++

Routine Description:

    This function writes back and invalidates the caches
    on the current processor

Arguments:

    InvalidateOnly - Indicate the operation is invalidation only.

Return Value:

    None.

--*/
{
    ARM_CACHE_LEVEL_INFO LevelInfo;

    LevelInfo.Ulong = _MoveFromCoprocessor(CP15_CLIDR);

    //
    // ARM instruction caches do not snoop the data cache.  Therefore,
    // the data cache must first be flushed back to the to the
    // implicit point of unification before invalidating the
    // instruction cache
    // Clean and invalidate the Dcache to the Point of Coherency
    //

    MiWritebackInvalidateDcacheV7BySetWay(LevelInfo.LoC, InvalidateOnly);
    _DataSynchronizationBarrier();

    //
    // Invalidate icache
    //

    _MoveToCoprocessor(0, CP15_ICIALLU);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();
}

VOID
BlpMmArmSwitchMmState (
    PARM_MM_STATE MmState
    )
{
    ULONG Value;

    //
    // Switch to new page tables (same format)
    //

    Value = MmState->HardwarePageDirectory | MmState->TTB_Config;
    _MoveToCoprocessor(Value, CP15_TTBR0);
    _InstructionSynchronizationBarrier();

    //
    // Invalidate TLB
    //

    _MoveToCoprocessor(0, CP15_TLBIALL);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();
}

VOID
MmArmRelocateSelfMap (
    VOID
    )

/*++

Routine Description:

    This routine relocates the dynamic self-map created during boot library
    initialization to the fixed location the kernel expects. This is easily
    achieved by swapping the PDE contents.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PHYSICAL_ADDRESS DestinationPhysical;
    PULONG DestinationPde;
    BOOLEAN DestinationValid;
    PARM_MM_STATE MmState;
    PHYSICAL_ADDRESS SourcePhysical;
    PULONG SourcePde;

    //
    // If the relocation doesn't need to happen, exit immediately.
    //

    if ((ULONG_PTR)BlMmPteBase == PTE_BASE) {
        return;
    }

    ASSERT(BlpArchQueryCurrentContextType() != ExecutionContextFirmware);

    MmState = BlpArchGetMmState(ExecutionContextApplication);

    //
    // Get the PDE for the self map and the physical addresses for the leaf
    // pages.
    //

    SourcePde = MiGetPdeAddress(BlMmPteBase);
    DestinationPde = MiGetPdeAddress(PTE_BASE);
    SourcePhysical.QuadPart = *SourcePde & PAGE_PFN_MASK;
    DestinationPhysical.QuadPart = *DestinationPde & PAGE_PFN_MASK;

    //
    // There had be a self-map at boot, so the source PDE should be valid. Write
    // the physical address of the page table base to the well-known self-map
    // PDE. Also check to see if the destination address is in use before
    // updating it.
    //

    ASSERT(MI_IS_NATIVE_PDE_VALID(SourcePde));

    DestinationValid = MI_IS_NATIVE_PDE_VALID(DestinationPde);
    MmArmWriteNativePde(DestinationPde,
                        MmState->MappedHardwarePageDirectory,
                        SourcePhysical);

    //
    // If the well-known self-map PDE was in use, copy its contents to the
    // dynamic self-map's PDE. This allows any leaf page tables to be cleaned up
    // by kernel MM. If it was invalid, just zero the dynamic self-map's PDE.
    //

    if (DestinationValid != FALSE) {
        MmArmWriteNativePde(SourcePde,
                            MmState->MappedHardwarePageDirectory,
                            DestinationPhysical);

    } else {
        MmArmZeroNativePde(SourcePde,
                           MmState->MappedHardwarePageDirectory);
    }

    //
    // Peform a full flush of the TLB. Any mappings in the kernel self-map range
    // need to be flushed.
    //

    MmArchFlushTlb();

    //
    // This will make the self-map routines available again.
    //

    MmPdeBase = PDE_BASE;
    BlMmPteBase = PTE_BASE;
    return;
}

NTSTATUS
MmArmZeroVirtualAddressRange (
    __in PVOID VirtualAddress,
    __in ULONGLONG Size
    )

/*++

Routine Description:

    This routine clears the PTEs for the virtual address range defined by the
    base address and size supplied. The virtual address range must be valid
    with page tables initialized or this routine will return an error.

Arguments:

    VirtualAddress - Supplies the base address of the region that will be
        cleared.

    Size - Supplies the size of the virtual address region that will be cleared.

Return Value:

    NTSTATUS Code.

--*/

{

    ULONG Index;
    PULONG Pde;
    ULONG PdeCount;
    PULONG Pte;
    ULONG PteCount;

    //
    // Input address must be page aligned.
    //

    if (((ULONG_PTR)VirtualAddress & (PAGE_SIZE - 1)) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate the size input. It should be page aligned and cannot exceed the
    // 4GB virtual address space.
    //

    if (((Size >> 32) != 0) || ((Size & (PAGE_SIZE - 1)) != 0) || (Size == 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Ensure the page tables exist for this VA space.
    //

    PdeCount = ROUND_UP((ULONG)Size, _4MB) / _4MB;
    Pde = MiGetPdeAddress(VirtualAddress);
    for (Index = 0; Index < PdeCount; Index += 1, Pde += 1) {
        if (MI_IS_NATIVE_PDE_VALID(Pde) == FALSE) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    //
    // Zero the PTE entries for the virtual address range.
    //

    PteCount = (ULONG)Size / PAGE_SIZE;
    Pte = MiGetPteAddress(VirtualAddress);
    for (Index = 0; Index < PteCount; Index += 1, Pte += 1) {
        MmArmZeroNativePte(Pte);
    }

    MmArchFlushTlb();
    return STATUS_SUCCESS;
}

NTSTATUS
MmArmMoveVirtualAddressRange (
    __in PVOID DestinationAddress,
    __in PVOID SourceAddress,
    __in ULONGLONG Size
    )

/*++

Routine Description:

    This routine moves the PTEs from the source virtual address range defined by
    the base address and size supplied into the destination virtual address
    range. The virtual address ranges must be valid with page tables initialized
    or else this routine will return an error. The source virtual address
    range's PTEs are cleared on return.

Arguments:

    DestinationAddress - Supplies the base address of the region that will
        receive the PTE values from the source region.

    SourceAddress - Supplies the base address of the region whose PTE values
        will be moved to the destination.

    Size - Supplies the size of the virtual address region that will be moved.

Return Value:

    NTSTATUS Code.

--*/

{

    PULONG DestinationPde;
    PULONG DestinationPte;
    ULONG Index;
    ULONG PdeCount;
    ULONG PteCount;
    PULONG SourcePde;
    PULONG SourcePte;

    //
    // Nothing to be done for a move onto itself.
    //

    if (SourceAddress == DestinationAddress) {
        return STATUS_SUCCESS;
    }

    //
    // Input addresses must be page aligned.
    //

    if ((((ULONG_PTR)DestinationAddress & (PAGE_SIZE - 1)) != 0) ||
        (((ULONG_PTR)SourceAddress & (PAGE_SIZE - 1)) != 0)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate the size input. It should be page aligned and cannot exceed the
    // 4GB virtual address space.
    //

    if (((Size >> 32) != 0) || ((Size & (PAGE_SIZE - 1)) != 0) || (Size == 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate that the ranges do not overlap.
    //

    if (((SourceAddress < DestinationAddress) &&
         (Add2Ptr(SourceAddress, Size) > DestinationAddress)) ||
        ((DestinationAddress < SourceAddress) &&
         (Add2Ptr(DestinationAddress, Size) > SourceAddress))) {

        return STATUS_NOT_SUPPORTED;
    }

    //
    // Ensure the page table pages exist for the both the destination and the
    // source.
    //

    PdeCount = ROUND_UP((ULONG)Size, _4MB) / _4MB;
    DestinationPde = MiGetPdeAddress(DestinationAddress);
    SourcePde = MiGetPdeAddress(SourceAddress);
    for (Index = 0; Index < PdeCount; Index += 1) {
        if ((MI_IS_NATIVE_PDE_VALID(DestinationPde) == FALSE) ||
            (MI_IS_NATIVE_PDE_VALID(SourcePde) == FALSE)) {

            return STATUS_INVALID_PARAMETER;
        }

        DestinationPde += 1;
        SourcePde += 1;
    }

    //
    // Move the PTE entries for the virtual address range to the destination and
    // zero the source.
    //
    // N.B. This routine could call MmPaeZeroVirtualAddressRange to zero the
    //      source, but since the parameters are already checked and this loop
    //      is moving through the source range, it takes advantage of zeroing
    //      them on the spot.
    //

    PteCount = (ULONG)Size >> PAGE_SHIFT;
    DestinationPte = MiGetPteAddress(DestinationAddress);
    SourcePte = MiGetPteAddress(SourceAddress);
    for (Index = 0; Index < PteCount; Index += 1) {
        *(volatile ULONG_PTR *)DestinationPte = *(ULONG_PTR *)SourcePte;
        MI_ARM_SYNC_PTE_TO_HPW(DestinationPte);
        MmArmZeroNativePte(SourcePte);
        DestinationPte += 1;
        SourcePte += 1;
    }

    MmArchFlushTlb();
    return STATUS_SUCCESS;
}

VOID
MmArmWriteOSPte (
    __in PULONG PointerPte,
    __in ULONG PhysicalAddress,
    __in ULONG OSAttribute
    )
{

    PhysicalAddress &= PAGE_PFN_MASK;

    *PointerPte = MM_PTE_TEMPLATE | PhysicalAddress | OSAttribute;
    MI_ARM_SYNC_PTE_TO_HPW(PointerPte);
    return;
}

VOID
BlMmArmWriteOSPte (
    __out PULONG PointerPte,
    __in ULONG PhysicalAddress,
    __in ULONG CacheAttributes
    )
{

    ULONG OSAttribute;

    if (((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_UNCACHED) != 0) ||
        ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_WRITE_THROUGH) != 0) ||
        ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_DMA) != 0)) {

        OSAttribute = MM_PTE_UC_MASK;

    } else if ((CacheAttributes & MEMORY_ATTRIBUTE_CACHE_WRITE_COMBINED) != 0) {
        OSAttribute = MM_PTE_WC_MASK;
    } else {
        OSAttribute = MM_PTE_CC_MASK;
    }
    PhysicalAddress &= PAGE_PFN_MASK;

    *PointerPte = MM_PTE_TEMPLATE | PhysicalAddress | OSAttribute;
    MI_ARM_SYNC_PTE_TO_HPW(PointerPte);
    return;
}
VOID
MmArmConvertToOSPte (
    PULONG Pte
    )
{

    ULONG Index;
    ULONG MmAttribute;
    ULONG MemoryType;
    ULONG Page;

    //
    // N.B. Strongly ordered or device attributes are mapped to UNCACHED.
    //      Write-back attributes are mapped to CACHED
    //      Non-cacheable normal memory is mapped to WRITE COMBINED.
    //      Write-through normal memory is mapped to WRITE COMBINED
    //      Reserved memory is mapped to CACHED

    const ULONG MemoryTypeToMmAttribute[32] = {
        MM_PTE_UC_MASK,         // Storngly-ordered
        MM_PTE_UC_MASK,         // Sharable Device
        MM_PTE_WC_MASK,         // Write-through, no write allocate normal
        MM_PTE_CC_MASK,         // Write-back, no write allocate normal
        MM_PTE_WC_MASK,         // Non-Cacheable normal
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Implementation defined
        MM_PTE_CC_MASK,         // write back, write allocate
        MM_PTE_UC_MASK,         // Non-shareable device
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_CC_MASK,         // Reserved
        MM_PTE_WC_MASK,         // Inner non-cacheable nornal
        MM_PTE_CC_MASK,         // Inner write back write allocate normal
        MM_PTE_WC_MASK,         // Inner write through, no write allocate normal
        MM_PTE_CC_MASK,         // Inner write back, no write allocate normal
        MM_PTE_WC_MASK,         // Inner non-cacheable nornal
        MM_PTE_CC_MASK,         // Inner write back write allocate normal
        MM_PTE_WC_MASK,         // Inner write through, no write allocate normal
        MM_PTE_CC_MASK,         // Inner write back, no write allocate normal
        MM_PTE_WC_MASK,         // Inner non-cacheable nornal
        MM_PTE_CC_MASK,         // Inner write back write allocate normal
        MM_PTE_WC_MASK,         // Inner write through, no write allocate normal
        MM_PTE_CC_MASK,         // Inner write back, no write allocate normal
        MM_PTE_WC_MASK,         // Inner non-cacheable nornal
        MM_PTE_CC_MASK,         // Inner write back write allocate normal
        MM_PTE_WC_MASK,         // Inner write through, no write allocate normal
        MM_PTE_CC_MASK,         // Inner write back, no write allocate normal
    };

    Page = *Pte & PAGE_PFN_MASK;
    MemoryType = *Pte & NATIVE_PTE_MEMORY_TYPE_MASK;

    Index = MmArmComputeNativeAtributeIndex(*Pte);
    MmAttribute = MemoryTypeToMmAttribute[Index];
    MmArmWriteOSPte(Pte, Page, MmAttribute);
    return;
}

VOID
BlpMmArmKernelSetup (
    PARM_MM_STATE MmState
    )
{
    ULONG Entry;
    PULONG HardwarePageDirectory;
    ULONG Index;
    PULONG PageTable;
    ULONG PrimaryRemap;
    PULONG SoftwarePageDirectory;
    ULONG TTBConfig;
    ULONG Value;

    //
    // Move the mapped hardware page directory from 1:1 to the well-known
    // virtual address: ARM_PAGE_DIRECTORY_VA_INITIAL.
    //

    MmArmMoveVirtualAddressRange((PVOID)ARM_PAGE_DIRECTORY_VA_INITIAL,
                                 (PVOID)MmState->MappedHardwarePageDirectory,
                                 NATIVE_PAGE_DIRECTORY_SIZE);

    //
    // Preserve data from existing mappings before making the transition
    //

    MiWritebackInvalidateCurrentCache(FALSE);

    //
    // Disable MMU and caches.  Assumes 1:1 mapping.
    //

    Value = (ULONG) _MoveFromCoprocessor(CP15_SCTLR);
    Value &= ~(CP15_SCTLR_M |CP15_SCTLR_I | CP15_SCTLR_C);
    _MoveToCoprocessor (Value, CP15_SCTLR);
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    //
    // Fetch pointers
    //

    HardwarePageDirectory = (PULONG) MmState->HardwarePageDirectory;
    SoftwarePageDirectory = (PULONG) MmState->SoftwarePageDirectory;

    //
    // Rewrite page table hierarchy into the OS format
    //

    for (Index = 0; Index < PDE_PER_PAGE; Index += 1) {

        //
        // N.B. The PDE format is the same between state and OS state.
        //

        //
        // Validate that the software and hardware page directories are in sync
        //

#if DBG

        NT_ASSERT((HardwarePageDirectory[Index * 4] & PAGE_PFN_MASK) ==
                  (SoftwarePageDirectory[Index] & PAGE_PFN_MASK));
#endif

        if (MI_IS_NATIVE_PDE_VALID(&SoftwarePageDirectory[Index]) != FALSE) {

            PageTable = (PULONG) (HardwarePageDirectory[Index * 4] & PAGE_PFN_MASK);

            for (Entry = 0; Entry < PTE_PER_PAGE; Entry +=1) {

                if (MI_IS_NATIVE_PTE_VALID(&PageTable[Entry]) != FALSE) {
                    MmArmConvertToOSPte(&PageTable[Entry]);
                }
            }
        }
    }

    //
    // Mark selfmap entry as non-Global.
    //

    SoftwarePageDirectory[MiGetPdeOffset(PDE_BASE)] |= MM_PTE_NON_GLOBAL_MASK;

    //
    // Reprogram to new state
    //

    _MoveToCoprocessor(0x1, CP15_DACR);
    _InstructionSynchronizationBarrier();

    _MoveToCoprocessor(0, CP15_TTBCR);
    _InstructionSynchronizationBarrier();

    //
    // Determine page table walk inner/outer caching attributes.  The
    // register format is different depending on whether MP extensions
    // are implemented.
    //

    TTBConfig = CP15_TTBRx_OUTER_UNSHARED;
    if (MmState->MpExtensions != FALSE) {
        TTBConfig |= CP15_TTBRx_MPEXT_OUTER_CACHED |
            CP15_TTBRx_MPEXT_INNER_CACHED;

    } else {
        TTBConfig |= CP15_TTBRx_OUTER_CACHED |
            CP15_TTBRx_INNER_CACHED;
    }

    //
    // Determine matching inner sharing attributes for normal memory
    // and page table walks.
    //
    // N.B. Currently outer sharing e.g. between clusters is not
    //      enabled as there are no mechanisms to determine its
    //      applicability.
    //

    if (READ_ARM_FEATURE (CP15_MMFR0, MMFR0_L1SHARED) >= MMFR0_L1SHARED_L1) {
        TTBConfig |= CP15_TTBRx_INNER_SHARED;
        PrimaryRemap = CP15_CR10_PRRR_SHARED;

    } else {
        PrimaryRemap = CP15_CR10_PRRR_UNSHARED;
    }

    _MoveToCoprocessor(PrimaryRemap, CP15_PRRR);
    _InstructionSynchronizationBarrier();

    _MoveToCoprocessor(CP15_NMRR_DEFAULT, CP15_NMRR);
    _InstructionSynchronizationBarrier();

    //
    // Be explicit that this code is running in the 'transition ASID'
    //

    _DataSynchronizationBarrier();
    _MoveToCoprocessor(TRANSITION_ASID, CP15_CONTEXTIDR);
    _InstructionSynchronizationBarrier();

    Value = MmState->HardwarePageDirectory | TTBConfig;
    _MoveToCoprocessor(Value, CP15_TTBR0);
    _InstructionSynchronizationBarrier();

    //
    // Invalidate TLB
    //

    _MoveToCoprocessor(0, CP15_TLBIALL);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    //
    // Invalidate caches since TEX remap may have been cached
    //
    // N.B. Since it is in MMU disabled context, invalidate cache only to avoid
    //      writing back any stale data and corrupt variables in memory.
    //

    MiWritebackInvalidateCurrentCache(TRUE);

    //
    // Enable MMU, caches and general OS configuration
    //

    Value = CP15_RES1_MASK      | // reserved '1' bits
            CP15_SCTLR_M        | // enable MMU
//          CP15_SCTLR_A        | // do NOT enable alignment faults
            CP15_SCTLR_C        | // enable data and unified caches
//          CP15_SCTLR_CP15BEN  | // do NOT enable CP15 barrier operations
//          CP15_SCTLR_THEE     | // do NOT enable ThumbEE mode
//          CP15_SCTLR_ITD      | // do NOT disable deprecated IT forms
            CP15_SCTLR_SED      | // disable SETEND instructions
//          CP15_SCTLR_SW       | // do NOT enable SWP/SWPB instructions
            CP15_SCTLR_Z        | // enable branch prediction
            CP15_SCTLR_I        | // enable instruction cache
//          CP15_SCTLR_V        | // do NOT force vectors to 0xFFFF0000
//          CP15_SCTLR_RR       | // do NOT force predictable cache strategy
//          CP15_SCTLR_nTWI     | // do NOT allow WFI at EL0
//          CP15_SCTLR_HA       | // do NOT enable hardware access flag
//          CP15_SCTLR_nTWE     | // do NOT allow WFE at EL0
//          CP15_SCTLR_WXN      | // do NOT imply writeable == XN
//          CP15_SCTLR_UWXN     | // do NOT imply user writeable == PXN
//          CP15_SCTLR_FI       | // do NOT enable low-latency interrupts
//          CP15_SCTLR_VE       | // do NOT use implementation defined vectors
//          CP15_SCTLR_EE       | // do NOT use big-endian exceptions
//          CP15_SCTLR_NMFI     | // do NOT make FIQ into an NMI
            CP15_SCTLR_TRE      | // enable TEX remap in MMU
            CP15_SCTLR_AFE      | // enable access bit functionality in MMU
            CP15_SCTLR_TE;        // enable exceptions in thumb mode

    _MoveToCoprocessor (Value, CP15_SCTLR);
    _InstructionSynchronizationBarrier();
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    return;
}

BOOLEAN
MmArchShouldFlushTlbAfterMappingAddress (
    VOID
    )

/*++

Routine Description:

    This routine, meant for use in MmArchMapPhysicalAddress, reports
    whether the system should flush the TLB after it maps an
    address.

Arguments:

    None.

Return Value:

    None.

--*/

{

    return TRUE;
}

BOOLEAN
MmArchShouldFlushTlbAfterUnmappingAddress (
    VOID
    )

/*++

Routine Description:

    This routine, meant for use in MmUnmapVirtualAddress, reports
    whether the system should flush the TLB after it unmaps an
    address.

Arguments:

    None.

Return Value:

    TRUE if the TLB should be flushed after a virtual address has been unmapped.

    FALSE otherwise.

--*/

{

    return TRUE;
}
