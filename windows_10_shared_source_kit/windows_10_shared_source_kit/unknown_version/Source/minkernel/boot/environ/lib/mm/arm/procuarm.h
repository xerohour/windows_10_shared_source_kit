/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    procuarm.h

Abstract:

    This module contains the private data structures and procedure
    prototypes for x86-64 address translation.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "mm.h"

//
// -------------------------------------------------------------------- Globals
//

ULONG MmPdeBase;
ULONG BlMmPteBase;

//
// -------------------------------------------------------------------- Defines
//

typedef ULONG NATIVE_MEMORY_ATTRIBUTES, *PNATIVE_MEMORY_ATTRIBUTES;

typedef MEMORY_DESCRIPTOR NATIVE_MEMORY_DESCRIPTOR, *PNATIVE_MEMORY_DESCRIPTOR;

//
// Structure describing the output retrieved from the CP15_PAR register.
//

typedef union _ARM_PAR {
    ULONG Ulong;
    struct {
        ULONG Aborted       :  1;
        ULONG Supersection  :  1;
        ULONG OuterAttrib   :  2;
        ULONG InnerAttrib   :  3;
        ULONG InnerShared   :  1;
        ULONG Reserved2     :  1;
        ULONG NS            :  1;
        ULONG NOS           :  1;
        ULONG Reserved1     :  1;
        ULONG Pfn           : 20;
    };
} ARM_PAR;

//
// ARM PDE definitions
//

#define NATIVE_PDE_TYPE_MASK       0x3
#define NATIVE_PDE_VALID_MASK      0x1
#define NATIVE_PDE_SECTION_MASK    0x2
#define NATIVE_PDE_TEX_MASK        (7 << 12)
#define NATIVE_PDE_NS_MASK         (1 < 3)

#define NATIVE_PDE_TEMPLATE        (NATIVE_PDE_VALID_MASK)

//
// ARM PTE definitions
//
// These definitions refer to the "native" ARMv7 page table format
// without the enablement of the 'remap mechanism'.
//
// ARM architecture requires the NX bit to be set for strong ordering and
// preventing speculative execution.
//

#define NATIVE_PTE_NX_MASK         0x1
#define NATIVE_PTE_VALID_MASK      0x2
#define NATIVE_PTE_WRITABLE_MASK   (3 << 0x4)
#define NATIVE_PTE_C_MASK          (1 << 3)
#define NATIVE_PTE_B_MASK          (1 << 2)

#define NATIVE_PTE_TEX0_MASK       (1 << 6)
#define NATIVE_PTE_TEX1_MASK       (1 << 7)
#define NATIVE_PTE_TEX2_MASK       (1 << 8)
#define NATIVE_PTE_SHAREABLE_MASK  (1 << 10)
#define NATIVE_PTE_TEX_MASK        (7 << 6)

#define NATIVE_PTE_NORMAL       NATIVE_PTE_TEX2_MASK
#define NATIVE_PTE_SO           NATIVE_PTE_NX_MASK

#define NATIVE_PTE_DEVICE_SHARED   (NATIVE_PTE_B_MASK | NATIVE_PTE_NX_MASK)
#define NATIVE_PTE_DEVICE_UNSHARED (NATIVE_PTE_TEX1_MASK | NATIVE_PTE_NX_MASK)

#define NATIVE_PTE_OUTER_NC     0
#define NATIVE_PTE_OUTER_WBWA   (NATIVE_PTE_TEX0_MASK)
#define NATIVE_PTE_OUTER_WT     (NATIVE_PTE_TEX1_MASK)
#define NATIVE_PTE_OUTER_WB     (NATIVE_PTE_TEX1_MASK | NATIVE_PTE_TEX0_MASK)

#define NATIVE_PTE_INNER_NC     0
#define NATIVE_PTE_INNER_WBWA   (NATIVE_PTE_B_MASK)
#define NATIVE_PTE_INNER_WT     (NATIVE_PTE_C_MASK)
#define NATIVE_PTE_INNER_WB     (NATIVE_PTE_C_MASK | NATIVE_PTE_B_MASK)

#define NATIVE_PTE_MEMORY_TYPE_MASK (NATIVE_PTE_C_MASK | \
                                     NATIVE_PTE_B_MASK | \
                                     NATIVE_PTE_TEX_MASK)

#define NATIVE_PTE_TEMPLATE (NATIVE_PTE_VALID_MASK | NATIVE_PTE_WRITABLE_MASK)

#define NATIVE_PAGE_DIRECTORY_SIZE (PAGE_SIZE * 4)

//
// PTE masks for OS PTEs and PDEs
//

#define MM_PTE_VALID_MASK           (0x02)
#define MM_PTE_CC_MASK              (0x00)
#define MM_PTE_UC_MASK              (0x05)
#define MM_PTE_WC_MASK              (0x0C)
#define MM_PTE_CACHE_CONFIG_MASK    (0x0C)
#define MM_PTE_ACCESS_MASK          (0x10)
#define MM_PTE_LARGE_PAGE_MASK      (0x400)
#define MM_PTE_NON_GLOBAL_MASK      (0x800)

//
// PteTemplate defines the basic PTE value used by the HAL.
// The intent is Valid, Access, Kernel access only, writable

#define MM_PTE_TEMPLATE             (MM_PTE_VALID_MASK  | \
                                     MM_PTE_ACCESS_MASK)


//
// Assume minimum cache line size for cleans to HPW
//

#define MI_MINIMUM_CACHE_LINE_SIZE  (16)

#define MI_CHECK_CACHE_ATTRIBUTES_WHILE_MAPPING_ADDRESS 0

//
// --------------------------------------------------------------------- Macros
//

#define MI_ARM_SYNC_PTE_WRITE()              \
     _SyncPte()

#define MI_ARM_SYNC_PTE_TO_HPW(_PointerPte)  \
     _SyncPte()

//
// Native PTE and PDE Management
//

#define MI_IS_NATIVE_PTE_VALID(_PointerPte) \
    ((*_PointerPte & NATIVE_PTE_VALID_MASK) == NATIVE_PTE_VALID_MASK)

#define NATIVE_PTE_TO_PA(_PointerPte) \
    ((*_PointerPte >> PAGE_SHIFT) << PAGE_SHIFT)

#define MI_IS_NATIVE_PDE_VALID(_PointerPde) \
    MI_IS_NATIVE_PTE_VALID(_PointerPde)

#define NATIVE_PT_PDE_TO_PA(_PointerPde) \
    NATIVE_PTE_TO_PA(_PointerPde)

#define NATIVE_PDE_TO_PTE(_PointerPde) \
    ((*_PointerPde)

#define NATIVE_LARGE_PAGE_SIZE 0x400000

#define PAGE_PFN_MASK      0xFFFFF000

//++
//PULONG
//MiGetPdeAddress (
//    __in PVOID va
//    );
//
// Routine Description:
//
//    MiGetPdeAddress returns the address of the PDE which maps the
//    given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the PDE for.
//
// Return Value:
//
//    The address of the PDE.
//
//--

#define MiGetPdeAddress(va)  \
    ((PULONG)Add2Ptr(MmPdeBase, ((((ULONG)(va)) >> 22) << 2)))

//++
//PULONG
//MiGetPteAddress (
//    __in PVOID va
//    );
//
// Routine Description:
//
//    MiGetPteAddress returns the address of the PTE which maps the
//    given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the PTE for.
//
// Return Value:
//
//    The address of the PTE.
//
//--

#define MiGetPteAddress(va) \
    ((PULONG)Add2Ptr(BlMmPteBase, ((((ULONG)(va)) >> 12) << 2)))

//++
//ULONG
//MiGetPdeOffset (
//    __in PVOID va
//    );
//
// Routine Description:
//
//    MiGetPdeOffset returns the offset into a page directory
//    for a given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the offset for.
//
// Return Value:
//
//    The offset into the page directory table the corresponding PDE is at.
//
//--

#define MiGetPdeOffset(va) (((ULONG)(va)) >> 22)

//++
//ULONG
//MiGetPteOffset (
//    __in PVOID va
//    );
//
// Routine Description:
//
//    MiGetPteOffset returns the offset into a page table page
//    for a given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the offset for.
//
// Return Value:
//
//    The offset into the page table page table the corresponding PTE is at.
//
//--

#define MiGetPteOffset(va) ((((ULONG)(va)) << 10) >> 22)

//++
//PVOID
//MiGeneratePdeBase (
//    __in PVOID va
//    );
//
// Routine Description:
//
//     MiGeneratePdeBase returns the address of the dynamic PDE base used for
//     the self map.
//
// Arguments
//
//     Va - Supplies the virtual address that marks the start of the 4MB
//        reserved for the self map.
//
// Return Value:
//
//    The address of the PDE base.
//
//--

#define MiGeneratePdeBase(va) \
    (ULONG)((MiGetPdeOffset(va) << 22) | (MiGetPdeOffset(va) << 12))

//++
//PVOID
//MiGeneratePteBase (
//    __in PVOID va
//    );
//
// Routine Description:
//
//     MiGeneratePteBase returns the address of the dynamic PTE base used for
//     the self map.
//
// Arguments
//
//     Va - Supplies the virtual address that marks the start of the 4MB
//        reserved for the self map.
//
// Return Value:
//
//    The address of the PTE base.
//
//--

#define MiGeneratePteBase(va) (ULONG)(MiGetPdeOffset(va) << 22)

//
// N.B. The PDE/PTE accessor routines for the native page table format
//      are identical to the OS versions.  These routines are only used
//      when accessing application state page tables and thus the self map and
//      other characteristics of the OS version can beassumed.
//

#define MiGetNativePdeAddress(va) MiGetPdeAddress(va)
#define MiGetNativePteAddress(va) MiGetPteAddress(va)
#define MiGetNativePdeOffset(va)  MiGetPdeOffset(va)
#define MiGetNativePteOffset(va)  MiGetPteOffset(va)

// ----------------------------------------------------------------- Prototypes

NTSTATUS
MmArmRemapVirtualAddress (
    __in_bcount(Size) PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONGLONG Size,
    __in ULONG CacheAttribute
    );

//
// Due to intermediate caching of page table translations on ARM, extra care
// has to be taking if the boot code ever switches to flushing individual TLB
// entries. Kernel MM has to make sure it invalidates all 4 hardware PDE
// entries when invalidating a PDE. For now, only use a full flush.
//

VOID
MmArchFlushTlb (
    VOID
    );

NTSTATUS
MmArmAllocatePageTables (
    __inout_ecount(PagesToAllocate) PPHYSICAL_ADDRESS AllocatedPageTables,
    __in ULONG PagesToAllocate
    );

VOID
MmArmSyncRangeToHPW(
    VOID
    );

#if OLDMODEL

VOID
MmArmZeroPde (
    __out PULONG PointerPde,
    __in PULONG HardwarePageDirectory
    );

VOID
MmArmWritePde (
    __out PULONG PointerPde,
    __out PULONG HardwarePageDirectory,
    __in ULONG PhysicalAddress
    );
VOID
MmArmWritePte (
    __in PULONG PointerPte,
    __in ULONG PhysicalAddress,
    __in ULONG CacheAttribute
    );

#endif

VOID
MmArmWriteNativePde (
    __out PULONG PointerPde,
    __out PULONG HardwarePageDirectory,
    __in PHYSICAL_ADDRESS PhysicalAddress
    );

VOID
MmArmZeroNativePde (
    __out PULONG PointerPde,
    __out PULONG HardwarePageDirectory
    );

VOID
MmArmWriteNativePte (
    __in PULONG PointerPte,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONG CacheAttribute
    );

VOID
MmArmRelocateSelfMap (
    VOID
    );

NTSTATUS
MmArmZeroVirtualAddressRange (
    __in PVOID VirtualAddress,
    __in ULONGLONG Size
    );

NTSTATUS
MmArmMoveVirtualAddressRange (
    __in PVOID DestinationAddress,
    __in PVOID SourceAddress,
    __in ULONGLONG Size
    );

NTSTATUS
MmArmDetermineMatchingMemoryAttributes (
    __in PHYSICAL_ADDRESS Address,
    __out PNATIVE_MEMORY_ATTRIBUTES Attributes
    );

NATIVE_MEMORY_ATTRIBUTES
MmArmMapAttributes (
    __in ULONG CacheAttributes
    );

ULONG
MmArmDeterminePageTableAttributes (
    VOID
    );

NTSTATUS
MmArmDetermineMatchingMemoryAttributes (
    __in PHYSICAL_ADDRESS Address,
    __out PNATIVE_MEMORY_ATTRIBUTES Attributes
    );

VOID
MmArmSetNativePageTableAttributes (
    __in ULONG NativeMemoryAttributes
    );

ULONG
MmArmMapNativeMemoryAttributesToTtbConfig (
    __in ULONG NativeMemoryAttributes
    );
