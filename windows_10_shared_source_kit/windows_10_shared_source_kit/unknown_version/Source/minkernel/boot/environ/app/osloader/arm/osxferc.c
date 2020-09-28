/*++

Copyright (c) Microsoft Corporation

Module Name:

    osxferc.c

Abstract:

    ARM routines required to transfer execution to the Operating System.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// ---------------------------------------------------------------- Definitions
//

#define USERSHARED_PAGE_COUNT    1

//
// ---------------------------------------------------------------- Globals
//

#pragma bss_seg(push)
#pragma bss_seg()
ULONG OslArchKernelStack;
#pragma bss_seg(pop)

PVOID UserSharedAddress;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
OslArchpKernelSetupPhase0 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslArchpKernelSetupPhase1 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslArchpKernelSetupPhase2 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslArmTransferToKernel(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock,
    IN PVOID KernelEntry
    );

// ------------------------------------------------------------------ Functions

#pragma code_seg(push)
#pragma code_seg()

NTSTATUS
OslArchKernelSetup (
    __in ULONG Phase,
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will perform the architecture specific steps to prepare for
    the transfer into the kernel.  This proces is broken into two phases.
    In the first phase, MM (library) services can be utilized.  In the second
    phase, MM services are illegal.

Arguments:

    Phase - The phase of kernel setup.

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.

--*/

{

    NTSTATUS Status;

    if (Phase == 0) {
        Status = OslArchpKernelSetupPhase0(LoaderBlock);

    } else if (Phase == 1) {
        Status = OslArchpKernelSetupPhase1(LoaderBlock);

    } else {

        ASSERT(Phase == 2);

        Status = OslArchpKernelSetupPhase2(LoaderBlock);
    }

    return Status;
}

#pragma code_seg(pop)

NTSTATUS
OslArchLoadFirmwareDrivers (
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PLIST_ENTRY LoadedFirmwareDriverList
    )

/*++

Routine Description:

    This routine will load any firmware specific drivers required by the kernel
    for this architecture.  No-op on ARM.

Arguments:

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies a handle to the device containing the OS.

    LoadedFirmwareDriverList - Supplies a pointer to the list tracking firmware
        drivers loaded by the OS loader.

Return Value:

    STATUS_SUCCESS

--*/

{

    UNREFERENCED_PARAMETER(OsFilePath);
    UNREFERENCED_PARAMETER(OsDeviceId);
    UNREFERENCED_PARAMETER(LoadedFirmwareDriverList);

    return STATUS_SUCCESS;
}

VOID
OslArchDebugPatchKernelInterrupts (
    VOID
    )

/*++

Routine Description:

    This routine installs the boot debugger's trap handlers in the kernel's IDT.

Arguments:

    None.

Return Value:

    None.

--*/

{

    return;
}


// ------------------------------------------------------------ Local Functions

NTSTATUS
OslArchpKernelSetupPhase0 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine performs the architecture steps required for kernel transition
    which require MM services.

    In particular, the following is performed:

    - Allocate kernel stacks (Idle thread, DPC)
    - Allocate and map Shared User page.

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.

--*/

{
    ULONG DataSize;
    PVOID KdCpuBuffer;
    PVOID KernelData;
    ULONG KernelStackSize;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    KernelStackSize = OslDetermineKernelStackSize(LoaderBlock);
    KernelData = NULL;
    PhysicalAddress.QuadPart = 0;
    LoaderBlock->KernelStackSize = KernelStackSize;

    //
    // The kernel determines if the Operating System was booted with
    // /3GB from the loader block field, VirtualBias.  This is the relative
    // offset of the kernel's address space to KSEG0_BASE.
    //

    LoaderBlock->u.Arm.VirtualBias =
        (BlMmGetKsegBase() - BlMmGetKsegBias()) - KSEG0_BASE;

    //
    // Allocate enough memory for the kernel, DPC, and double fault stacks.
    //

    DataSize = KernelStackSize +          // Idle thread stack.
               PAGE_SIZE +                // Idle thread stack guard page.
               KernelStackSize +          // DPC stack.
               PAGE_SIZE +                // DPC stack guard page.
               DOUBLE_FAULT_STACK_SIZE +  // Double Fault stack.
               ISR_STACK_SIZE +           // ISR stack.
               PAGE_SIZE;                 // ISR stack guard page.

    Status = BlMmAllocatePages(&KernelData,
                               DataSize >> PAGE_SHIFT,
                               OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Zero out memory block.
    //

    RtlZeroMemory(KernelData, DataSize);

    //
    // Set kernel and DPC stack pointers
    //

    OslArchKernelStack = (ULONG)Add2Ptr(KernelData,
                                        KernelStackSize +PAGE_SIZE);

    LoaderBlock->KernelStack = (ULONG)Add2Ptr(KernelData,
                                              (KernelStackSize +PAGE_SIZE) *2);

    //
    // The kernel assumes the Panic and ISR stacks are after the kernel stack.
    //

#if defined(NTKERNEL)

    //
    // Allocate a page for user shared data and exception data.
    // When running on NT, this page will be mapped to the target VA right away.
    //

    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       USERSHARED_PAGE_COUNT,
                                       OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE,
                                       MEMORY_ATTRIBUTE_NONE,
                                       0);

    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

    //
    // Zero shared user data page.
    //
    // N.B. In case of Soft Restart we can't zero the page at its fixed address,
    //      because the page is only going to be mapped in the shadow address
    //      space and not in the host.
    //

    Status = OslZeroPhysicalPages(PhysicalAddress, USERSHARED_PAGE_COUNT);
    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

    VirtualAddress = (PVOID)KI_USER_SHARED_DATA;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    USERSHARED_PAGE_COUNT * PAGE_SIZE,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

    //
    // On top of allocations made above, apply the HAL VA reservations.
    // The order matters because user shared data is within a HAL VA space.
    //

    VirtualAddress = (PVOID) HAL_VA_START;
    PhysicalAddress.QuadPart = MM_PFN_NO_MAP;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    HAL_VA_SIZE,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

#else


    //
    // Allocate memory for the user shared data. This can get mapped to an
    // arbitrary address. It will get patched to the well-known virtual address
    // in the kernel's address space during Arch Kernel Setup Phase 2.
    //

    Status = BlMmAllocatePages(&UserSharedAddress,
                               USERSHARED_PAGE_COUNT,
                               OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE,
                               MEMORY_ATTRIBUTE_NONE,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

    RtlZeroMemory(UserSharedAddress, USERSHARED_PAGE_COUNT * PAGE_SIZE);

    //
    // Since Phase 2 cannot allocate memory, ensure that the page tables are in
    // place for the fixed virtual addresses of the user shared data page, the
    // page directory pages, and the HAL virtual address range. Since these are
    // all in the last 4MB, only one mapping call is needed with MM_PFN_NO_MAP
    //

    VirtualAddress = (PVOID)KI_USER_SHARED_DATA;
    PhysicalAddress.QuadPart = MM_PFN_NO_MAP;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    USERSHARED_PAGE_COUNT * PAGE_SIZE,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        goto OslArchpKernelSetupPhase0End;
    }

    //
    // N.B. The HAL uses the last PDEs worth of virtual address space
    // for memory mapping prior to MM initialization using its private
    // memory mapping infrastructure.  This area is shared with the
    // few fixed mappings performed above.  Since those allocations
    // have already taken place, no additional initialization of the
    // HAL area is required.
    //
    // The following asserts should catch breaks in these assumptions.
    //

    ASSERT(HAL_VA_START < KI_USER_SHARED_DATA);
    ASSERT(HAL_VA_START < ARM_PAGE_DIRECTORY_VA_INITIAL);

#endif

    //
    // Allocate Kd CPU buffer when KD is enabled.
    //

    if (KdDebugDevice != NULL) {
        Status = BlMmAllocatePages(&KdCpuBuffer,
                                   1,
                                   OSL_MEMORY_TYPE_KD_EXECUTABLE_DATA,
                                   MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                                   0);

        if (!NT_SUCCESS(Status)) {
            goto OslArchpKernelSetupPhase0End;
        }

        //
        // Zero out memory block.
        //

        RtlZeroMemory(KdCpuBuffer, PAGE_SIZE);
        LoaderBlock->u.Arm.KdCpuBuffer = KdCpuBuffer;
    }

    //
    // Cleanup.
    //

OslArchpKernelSetupPhase0End:

    if (!NT_SUCCESS(Status)) {
        if (KernelData != NULL) {
            BlMmFreePages(KernelData);
        }

        if (PhysicalAddress.QuadPart != 0) {
            BlMmFreePhysicalPages(PhysicalAddress);
        }
    }

    return Status;
}

NTSTATUS
OslArchpKernelSetupPhase1 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine performs the firmware steps requied for kernel transition
    which must be performed after MM services are illegal.

    In particular, this routine will:

    - Ensure the I/D caches are consistent

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS always.

--*/

{
    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    PUCHAR BaseAddress;
    ULONG Length;
    PLIST_ENTRY ListEntry;

    //
    // Ensure that the I and D caches are consistent prior to
    // launching the kernel
    //
    // Walk through the loaded module list list being passed to the
    // kernel and determine the ranges of cache lines to be flushed.
    //

    for (ListEntry = LoaderBlock->LoadOrderListHead.Flink;
         ListEntry != &LoaderBlock->LoadOrderListHead;
         ListEntry = ListEntry->Flink) {

        DataTableEntry = CONTAINING_RECORD(ListEntry,
                                       KLDR_DATA_TABLE_ENTRY,
                                       InLoadOrderLinks);

        BaseAddress = (PUCHAR) DataTableEntry->DllBase;
        Length = DataTableEntry->SizeOfImage;

        BlArchSweepIcacheRange(BaseAddress, Length);
    }

    return STATUS_SUCCESS;
}

#pragma code_seg(push)
#pragma code_seg()

NTSTATUS
OslArchpKernelSetupPhase2 (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine performs the architecture steps requied for kernel transition
    which must be performed after MM services are illegal and after firmware
    services are no longer available.

    On ARM, there are no such operations to take.  This routine always
    returns successfully.

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    UNREFERENCED_PARAMETER(LoaderBlock);

#if !defined(NTKERNEL)


    //
    // Relocate the self-map to the fixed virtual address expected by the
    // kernel.
    //

    BlMmRelocateSelfMap();

    //
    // Zero the PTEs in the HAL's virtual address region. The HAL's VA allocator
    // skips used mappings. If the firmware consumed the HAL VA region with 1:1
    // mappings, then the HAL would fail. Therefore it needs to be zero'd. There
    // should be no kernel mappings in the HAL VA region as it was reserved
    // during library initialization.
    //
    // N.B. This must be done before setting the PTEs for the user shared data
    //      page and page direcotry because those fixed addresses are within the
    //      HAL VA region.
    //

    NT_VERIFY(
        NT_SUCCESS(
            BlMmZeroVirtualAddressRange((PVOID)HAL_VA_START, HAL_VA_SIZE)));

    //
    // Set the fixed virtual address mapping for the user shared page.
    //

    NT_VERIFY(
        NT_SUCCESS(
            BlMmMoveVirtualAddressRange((PVOID)KI_USER_SHARED_DATA,
                                        UserSharedAddress,
                                        USERSHARED_PAGE_COUNT * PAGE_SIZE)));

#endif

    BlArchKernelSetup();
    return STATUS_SUCCESS;
}

VOID
OslArchTransferToKernel (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PVOID KernelEntry
    )

/*++

Routine Description:

    This routine will transfer execution to the kernel on an ARM platform.

Arguments:

    LoaderBlock - Supplies a  pointer to the kernel's input parameter
        structure.

    KernelEntry - Supplies a pointer to the kernel's entry point.

Return Value:

    None.

--*/

{

#if !defined(NTKERNEL)

    ULARGE_INTEGER PerformanceCounter;

    //
    // Two co-processor registers were hijacked on ARM to create a virtual 48bit
    // cycle counter, but only one survives the loader to kernel transition.
    // Compress the necessary data down into the sole register that survives.
    //
    // N.B. The hijacked co-processor register will contain the top 32 bits
    //      of the virtual 48 bit cycle counter.
    //

    PerformanceCounter.LowPart = _MoveFromCoprocessor(CP15_TPIDRURO);
    PerformanceCounter.HighPart = _MoveFromCoprocessor(CP15_TPIDRURW);
    PerformanceCounter.HighPart &= 0x0000FFFF;
    PerformanceCounter.QuadPart >>= 16;
    _MoveToCoprocessor(PerformanceCounter.LowPart, CP15_TPIDRURW);

#endif

    //
    // Execute the transition.
    //

    OslArmTransferToKernel(LoaderBlock, KernelEntry);
    return;
}

#pragma code_seg(pop)


