/*++

Copyright (c) 2000-2008  Microsoft Corporation

Module Name:

    initkr.c

Abstract:

    This module contains the code to initialize the kernel data structures
    and to initialize the idle thread, its process, the processor control
    block, and the processor control region.

Environment:

    Kernel mode only.

--*/

#include "ki.h"
#include "inbv.h"

//
// References to exception vector table
//

extern void (*KiArmExceptionVectors[])(VOID);

//
// References to security cookie implementation
//

extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;
extern ULONG_PTR ExpSecurityCookieRandomData;

//
// Define forward referenced prototypes.
//

VOID
KiInitializeBootStructures (
    PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
KiInitializeDebugRegisters (
    VOID
    );

VOID
KiInitializeGSCookieValue(
    VOID
    );

VOID
KiSetCacheInformation (
    VOID
    );

VOID
KiSetFeatureBits (
    IN PKPRCB Prcb
    );

//
// Configuration used when initializing task base register.  Must
// match the remap registers (currently intialized in the loader)
//

ULONG KiTbrConfiguration;

//
// Cortex-A9 errata workaround
//

PKPROFILE KiA9ErrataProfileObject = NULL;

#pragma data_seg("INITDATA")

#pragma data_seg()

#pragma alloc_text(INIT, KiInitializeGSCookieValue)

#pragma alloc_text(PAGELK, KiInitializeBootStructures)
#pragma alloc_text(PAGELK, KiInitializeKernel)
#pragma alloc_text(PAGELK, KiSetFeatureBits)
#pragma alloc_text(PAGELK, KiSetCacheInformation)

DECLSPEC_SAFEBUFFERS
VOID
KiInitializeGSCookieValue(
    VOID
    )

/*++

Routine Description:

    This function initializes the GS security cookie.

Arguments:

    None.

Return Value:

    None.

Note:

    This function can't have locals that would invoke a GS probe.

--*/

{

    //
    // Assert to make sure that entropy breakages do not inadvertently creep
    // in.
    //

    NT_ASSERT(ExpSecurityCookieRandomData != 0);

    //
    // Initialize security cookie using time and counters to provide
    // some randomness
    //

    __security_cookie = (ULONG)KeQueryPerformanceCounter(NULL).QuadPart;
    __security_cookie ^= ExpSecurityCookieRandomData;
    __security_cookie_complement = ~__security_cookie;
    return;
}

VOID
KiInitializeKernel (
    IN PKPROCESS Process,
    IN PKTHREAD Thread,
    IN PVOID IdleStack,
    IN PKPRCB Prcb,
    IN ULONG Number,
    PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This function gains control after the system has been booted, but before
    the system has been completely initialized. Its function is to initialize
    the kernel data structures, initialize the idle thread and process objects,
    complete the initialization of the processor control block (PRCB) and
    processor control region (PCR), call the executive initialization routine,
    then return to the system startup routine. This routine is also called to
    initialize the processor specific structures when a new processor is
    brought on line.

    N.B. Kernel initialization is called with interrupts disabled at IRQL
         HIGH_LEVEL and returns with with interrupts enabled at DISPATCH_LEVEL.

Arguments:

    Process - Supplies a pointer to a control object of type process for
        the specified processor.

    Thread - Supplies a pointer to a dispatcher object of type thread for
        the specified processor.

    IdleStack - Supplies a pointer the base of the real kernel stack for
        idle thread on the specified processor.

    Prcb - Supplies a pointer to a processor control block for the specified
        processor.

    Number - Supplies the number of the processor that is being
        initialized.

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.


--*/

{

    KCPU_FEATURES FeatureBits;
    GROUP_AFFINITY GroupAffinity;
    ARM_IDCODE Id;
    PKPRCB Prcb0;
    ULONG Value;

    //
    // Indicate to the hypervisor that a new processor is running in the
    // system.
    //

    if (Number == 0) {
        HvlPhase0Initialize(LoaderBlock);

    } else {
        HvlEnlightenProcessor();
    }

    //
    // Get the processor feature bits.
    //

    FeatureBits = Prcb->FeatureBits;

    //
    // NEON is now required.  Bugcheck here for diagnosibility.
    //

    if ((FeatureBits & KF_SIMD) == 0) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     9,
                     _MoveFromCoprocessor(CPVFP_MVFR0),
                     _MoveFromCoprocessor(CPVFP_MVFR1),
                     0);

    }

    //
    // If this is processor zero, then set the number of logical
    // processors per physical processor and the initial number of
    // physical processors.
    //

    if (Number == 0) {

        //
        // Flush the entire TB to clear loader leftovers.
        //

        KeFlushCurrentTb();

        //
        // Set per-processor feature bits regarding NX policy which is always
        // on for ARM processors.
        //

        FeatureBits |= KF_GLOBAL_32BIT_NOEXECUTE;

        //
        // Configure that processor block and the temporary NUMA nodes.
        //

        KiConfigureInitialNodes(Prcb);
        KiConfigureProcessorBlock(Prcb, FALSE);

        //
        // Compact the system service table.
        //

        KeCompactServiceTable(&KiServiceTable[0],
                              &KiArgumentTable[0],
                              KiServiceLimit,
                              KE_COMPACT_SYSTEM_TABLE,
                              0);

    }

    //
    // Initialize the package processor set and core processor set.
    //
    // N.B. Affinities will be determined later after all processors have been
    //      started.
    //

    KeInitializeAffinityEx(&Prcb->PackageProcessorSet);
    KeAddProcessorAffinityEx(&Prcb->PackageProcessorSet, Number);
    Prcb->CoreProcessorSet = Prcb->GroupSetMember;

    //
    // Set processor cache size information.
    //

    KiSetCacheInformation();

    //
    // Initialize power state information.
    //

    PoInitializePrcb(Prcb);

    //
    // If the initial processor is being initialized, then initialize the
    // per system data structures. Otherwise, check for a valid system.
    //

    if (Number == 0) {

        //
        // Set global architecture and feature information.
        //

        KeProcessorArchitecture = PROCESSOR_ARCHITECTURE_ARM;
        KeProcessorLevel = Prcb->ProcessorModel;
        KeProcessorRevision = Prcb->ProcessorRevision;
        KeFeatureBits = FeatureBits;

        SharedUserData->ProcessorFeatures[PF_ARM_VFP_32_REGISTERS_AVAILABLE] = TRUE;
        SharedUserData->ProcessorFeatures[PF_ARM_NEON_INSTRUCTIONS_AVAILABLE] = TRUE;

        //
        // Enable CP14 debugging.
        //
        // N.B: Currently the use of the CP14 debug registers is causing hangs
        // on the Qualcomm 8660. Figure out why this is so these can be
        // re-enabled.
        //

        Id.Ulong = _MoveFromCoprocessor(CP15_MIDR);
        if ((Id.Implementer != 'Q') || (Id.Model != 0x2D)) {
            KiCP14DebugEnabled = TRUE;
        }

        //
        // Lower IRQL to APC level.
        //

        KeLowerIrql(APC_LEVEL);

        //
        // Initialize kernel internal spinlocks
        //

        KeInitializeSpinLock(&KiFreezeExecutionLock);

        //
        // Perform architecture independent initialization.
        //

        KiInitSystem();

        //
        // Initialize the idle process object and set the process quantum.
        //

        InitializeListHead(&KiProcessListHead);
        KeProcessorGroupAffinity(&GroupAffinity, 0);
        KeInitializeProcess(Process,
                            0,
                            &GroupAffinity,
                            NULL,
                            NULL,
                            KPROCESS_FLAG_HANDLE_ALIGNMENT_EXCEPTIONS);

        Process->QuantumReset = MAXCHAR;

        //
        // Initialize the pointer encode/decode keys.
        //

        _InstructionSynchronizationBarrier();
        Value = (ULONG)_MoveFromCoprocessor(CP15_PMCCNTR);
        Value ^= RotateLeft32((ULONG)&Value, 13);
        Value += (ULONG)&KiWaitAlways;
        KiWaitNever = Value;

        _InstructionSynchronizationBarrier();
        Value = (ULONG)_MoveFromCoprocessor(CP15_PMCCNTR);
        Value ^= RotateRight32((ULONG)&KiWaitAlways, 17);
        Value -= (ULONG)&Value;
        KiWaitAlways = Value;

    } else {

        //
        // If the CPU features are not identical, then bugcheck.
        //

        if ((FeatureBits != (KeFeatureBits & ~(KF_GLOBAL_32BIT_NOEXECUTE | KF_GLOBAL_32BIT_EXECUTE)))) {
            KeBugCheckEx(MULTIPROCESSOR_CONFIGURATION_NOT_SUPPORTED,
                         (ULONG)FeatureBits,
                         (ULONG)KeFeatureBits,
                         0,
                         0);

        }

        Prcb0 = KiProcessorBlock[0];
        if ((Prcb->MaxBreakpoints != Prcb0->MaxBreakpoints) ||
            (Prcb->MaxWatchpoints != Prcb0->MaxWatchpoints)) {

                //
                // All processors must have the same number of h/w debug
                // registers.
                //

                KeBugCheckEx(MULTIPROCESSOR_CONFIGURATION_NOT_SUPPORTED,
                             Number,
                             0,
                             0,
                             0);

        }

        //
        // Lower IRQL to DISPATCH level.
        //

        KeLowerIrql(DISPATCH_LEVEL);
    }

    //
    // Set global processor features.
    //

    SharedUserData->ProcessorFeatures[PF_COMPARE_EXCHANGE_DOUBLE] = TRUE;

    //
    // Initialize the idle thread object.
    //

    KiInitializeIdleThread(Thread, IdleStack, Process, Prcb);

    //
    // Call the executive initialization routine.
    //

    try {
        ExpInitializeExecutive(Number, LoaderBlock);

    } except(KiFatalFilter(PHASE0_EXCEPTION, GetExceptionInformation())) {
        NOTHING;
    }

    //
    // Complete kernel initialization.
    //

    KiCompleteKernelInit(Prcb, Thread, Number);

    //
    // Signal that this processor has completed its initialization.
    //

    LoaderBlock->Prcb = (ULONG)NULL;
    return;
}

VOID
KiInitializeExceptionVectorTable (
    VOID
    )

/*++

Routine Description:

    This function initializes the exception vector table for the current
    processor.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG Value;

    _MoveToCoprocessor ((ULONG_PTR)KiArmExceptionVectors & ~1, CP15_VBAR);
    _InstructionSynchronizationBarrier();
    Value = _MoveFromCoprocessor (CP15_SCTLR);
    Value &= ~CP15_SCTLR_V;
    _MoveToCoprocessor (Value, CP15_SCTLR);
    _InstructionSynchronizationBarrier();
    return;
}

VOID
KiInitializeBootStructures (
    PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This function initializes the boot structures for a processor. It is only
    called by the system start up code. Certain fields in the boot structures
    have already been initialized. In particular:

    The processor number in the PCR.

    N.B. All uninitialized fields are zero.

Arguments:

    LoaderBlock - Supplies a pointer to the loader block that has been
        initialized for this processor.

Return Value:

    None.

--*/

{

    ULONG Allocation;
    BOOLEAN BootDebuggerActive;
    PKDPC_STACK_FRAME DpcStackFrame;
    ULONG Number;
    PKPCR Pcr = KeGetPcr();
    PKPRCB Prcb = KeGetCurrentPrcb();
    PKPROCESS Process;
    PCHAR Ptr;
    PKTHREAD Thread;
    PKARM_VFP_STATE VfpState;

    BootDebuggerActive = FALSE;

    //
    // Initialize the PCR major and minor version numbers.
    //

    Pcr->MajorVersion = PCR_MAJOR_VERSION;
    Pcr->MinorVersion = PCR_MINOR_VERSION;

    //
    // initialize the PRCB major and minor version numbers and build type.
    //

    Prcb->MajorVersion = PRCB_MAJOR_VERSION;
    Prcb->MinorVersion =  PRCB_MINOR_VERSION;
    Prcb->BuildType = 0;

#if DBG

    Prcb->BuildType |= PRCB_BUILD_DEBUG;

#endif

#if defined(NT_UP)

    Prcb->BuildType |= PRCB_BUILD_UNIPROCESSOR;

#endif

    //
    // Initialize the PRCB set member.
    //

    Number = Pcr->Prcb.Number;

    //
    // If this is processor zero, then initialize the address of the system
    // process, initial thread, shared ready queue and size of kernel small
    // stacks.
    //

    if (Number == 0) {
        LoaderBlock->Process = (ULONG)&KiInitialProcess;
        LoaderBlock->Thread = (ULONG)&KiInitialThread;
        KeKernelStackSize = LoaderBlock->KernelStackSize;
        Prcb->IsrStack = (PVOID)(LoaderBlock->KernelStack +
                                 DOUBLE_FAULT_STACK_SIZE +
                                 ISR_STACK_SIZE +
                                 PAGE_SIZE);

        Allocation = (ULONG)Prcb;
        Allocation += sizeof(KPRCB);
        Allocation += (KMAXIMUM_PROCESSORS - 1) * sizeof(REQUEST_MAILBOX);
    }

    //
    // Initialize the PRCB scheduling thread address and the thread process
    // address.
    //

    Prcb->SharedReadyQueue = &Prcb->LocalSharedReadyQueue;
    Thread = (PVOID)LoaderBlock->Thread;
    Process = (PKPROCESS)LoaderBlock->Process;
    Prcb->CurrentThread = Thread;
    _MoveToCoprocessor((ULONG)Thread, CP15_TPIDRURO);
    Prcb->NextThread = NULL;
    Prcb->IdleThread = Thread;


    //
    // Set the energy profiling bit on the boot processor's idle thread
    // unconditionally for now. Since the flag is checked for bucketizing ISR
    // DPC cycles for a processor, this needs to be done before any
    // interrupt/dpc/exception is generated for boot processor.
    // PoEnergyEstimationEnabled can't be checked yet as reghive is not loaded
    // yet. Later this bit will be cleared for boot processor once reghive is
    // loaded and feature is determined to be disabled.
    //

    if (Number == 0) {
        InterlockedBitTestAndSet(&Thread->Header.Lock,
                                 THREAD_FLAGS_ENERGY_PROFILING_LOCK_BIT);
        
    } else if (PoEnergyEstimationEnabled() != FALSE) {
        InterlockedBitTestAndSet(&Thread->Header.Lock,
                                 THREAD_FLAGS_ENERGY_PROFILING_LOCK_BIT);
    }


    Thread->ApcState.Process = (PKPROCESS)Process;
    InitializeListHead(&Thread->ApcState.ApcListHead[KernelMode]);

    //
    // Initialize the processor block address.
    //

    KiProcessorBlock[Number] = Prcb;

    //
    // Initialize the PRCB address of the DPC stack.
    //

    DpcStackFrame = (PKDPC_STACK_FRAME)(LoaderBlock->KernelStack - sizeof(KDPC_STACK_FRAME));
    DpcStackFrame->MachineFrame.Pc = (ULONG)KiDispatchInterruptContinue;
    Prcb->DpcStack = (PVOID)DpcStackFrame;

    //
    // Initialize a temporary VfpState pointer, which is needed to dispatch
    // any exceptions which occur early during boot.
    //

    VfpState = (PKARM_VFP_STATE)DpcStackFrame - 1;
    RtlZeroMemory(VfpState, sizeof(*VfpState));
    Thread->VfpState = VfpState;

    //
    // Initialize the extended context pointer in PRCB, which is used by
    // the KeBugCheckEx codepath.
    //

    if (Prcb->Context == NULL) {
        Prcb->Context = &Prcb->ProcessorState.ContextFrame;
        Prcb->ContextFlagsInit = CONTEXT_FULL;
    }

    if (Number == 0) {

        //
        // Initialize the random number generator since memory management
        // requires it to assign address spaces.
        //

        ExRngInitializeSystem ();

        if (LoaderBlock->Extension->BootDebuggerActive != 0) {
            BootDebuggerActive = TRUE;

        } else {
            BootDebuggerActive = FALSE;
        }

        //
        // Set the exception vector base to point to the vectors embedded in the
        // kernel; this effectively randomizes their location due to ASLR in the
        // kernel. If the boot debugger is active, leave the boot environment
        // exception vectors in place.
        //

        if (BootDebuggerActive == FALSE) {
            KiInitializeExceptionVectorTable();
        }


        //
        // Initialize the boot display driver as early as possible during
        // system startup. There should be no hard coded calls to KeBugCheck
        // before this point.
        //

        InbvDriverInitialize(-1, LoaderBlock, 0);

        //
        // Override the default maximum group size if it is presented in loader
        // options.
        //

        if (LoaderBlock->LoadOptions != NULL) {
            Ptr = strstr(LoaderBlock->LoadOptions, "GROUPSIZE");
            if (Ptr != NULL) {
                while ((*Ptr != '\0') &&
                       (*Ptr != ' ') &&
                       ((*Ptr < '0') || (*Ptr > '9'))) {

                    Ptr += 1;
                }

                KiMaximumGroupSize = (ULONG)atoi(Ptr);

                //
                // Group size must be power of 2 and no greater than
                // MAXIMUM_PROC_PER_GROUP.
                //

                if ((KiMaximumGroupSize == 0) ||
                    (KiMaximumGroupSize > MAXIMUM_PROC_PER_GROUP) ||
                    ((KiMaximumGroupSize & (KiMaximumGroupSize - 1)) != 0)) {

                    KiMaximumGroupSize = MAXIMUM_PROC_PER_GROUP;
                }
            }
        }

    } else {
        KiInitializeExceptionVectorTable();
    }

    //
    // Initialize NX support.  This must occur prior to HAL initialization as
    // the HAL will create mappings in its reserved VA and these mappings need
    // to be marked NX where applicable.
    //

    KiInitializeNXSupport();

    //
    // Initialize the HAL for this processor.
    //

    HalInitializeProcessor(Number, LoaderBlock);

    //
    // Set processor CPU vendor and feature bits.
    //

    KiSetFeatureBits(Prcb);

    //
    // Initialize TLB and region (ASID) support
    //

    KiTbInitialize(Number, Process);

    //
    // Set debug registers to initial state
    //

    KiInitializeDebugRegisters();

    //
    // Determine cache type, initialize cache management routines, and
    // record cache description.
    //

    KiCacheInitialize();

    //
    // Initialize the per processor control block.
    //

    KiInitPrcb(Prcb, Number);

    //
    // Initialize IDT
    //

    KiInitializeIdt(Number, Pcr);

    //
    // Initialize the PRCB temporary pool look aside pointers.
    //

    ExInitPoolLookasidePointers();

    //
    // Set the appropriate member in the active processors set for the boot
    // processor. Other updates will be performed atomically as processors
    // are added.
    //

    if (Number == 0) {

        //
        // Set processor number.
        //

        KeNumberProcessors = 1;
        KeNumberProcessorsGroup0 = 1;

        //
        // Initialize active processor affinity mask.
        //

        KeInitializeAffinityEx((PKAFFINITY_EX)&KeActiveProcessors);
        KeAddProcessorAffinityEx((PKAFFINITY_EX)&KeActiveProcessors, 0);
    }

    return;
}

VOID
FASTCALL
KiA9ErrataProfileInterrupt (
    __in PKTRAP_FRAME TrapFrame,
    __in_opt PVOID Context
    )

/*++

Routine description:

    This dummy function does nothing; it is here merely to catch the
    generated interrupt.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame.

    Context - Supplies an optional callback context.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(TrapFrame);
    UNREFERENCED_PARAMETER(Context);
    return;

}

extern
KINLINE
BOOLEAN
KiInitMachineDependent (
    VOID
    )

/*++

Routine Description:

    This function initializes machine dependent data structures and hardware.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ARM_ERRATA_INFO ArmErrataInfo;
    KFLOATING_SAVE Dummy;
    ARM_IDCODE Id;
    ULONG64 Interval;
    GROUP_AFFINITY PreviousAffinity;
    ULONG Size;
    NTSTATUS Status;

    //
    // The following two lines make sure that KeSaveFloatingPointState and
    // KeRestoreFloatingPointState will have a body so thay could be
    // exported from ntoskrnl.exe
    //

    KeSaveFloatingPointState (&Dummy);
    KeRestoreFloatingPointState (&Dummy);

    //
    // Determine cache flush promote thresholds now that all caches --
    // architectural and external have been enumerated and enabled
    //

    KiSetSystemAffinityThreadToProcessor(0, &PreviousAffinity);
    KiCacheAdjustFlushPromoteThresholds();
    KeRevertToUserGroupAffinityThread(&PreviousAffinity);

    //
    // For Cortex-A9 and some Krait processors, work around the livelock errata
    // by creating a performance counter that runs all the time and periodically
    // generates interrupts to break potential livelocks.
    //

    Id.Ulong = _MoveFromCoprocessor (CP15_MIDR);
    if ( ((Id.Model == 0xC09) && (Id.Implementer == 'A')) ||
         ((Id.Model == 0x04D) && (Id.Implementer == 'Q') && (Id.Revision == 1)) ||
         ((Id.Model == 0x06F) && (Id.Implementer == 'Q') && (Id.Revision == 0))
       ) {

        KiA9ErrataProfileObject = ExAllocatePoolWithTag(NonPagedPoolNx,
                                                        sizeof(KPROFILE),
                                                        '  eK');

        if (KiA9ErrataProfileObject == NULL) {
            return FALSE;
        }

        //
        // Set the profiling interval to one tenth of the processor's
        // maximum frequency.
        //

        Interval = KeQueryCycleCounterFrequency(KeGetCurrentPrcb(), FALSE) / 10;
        KeInitializeProfileCallback(KiA9ErrataProfileObject,
                                    KiA9ErrataProfileInterrupt,
                                    NULL,
                                    ProfileTotalCycles);

        KeSetIntervalProfile((ULONG)Interval, ProfileTotalCycles);
        if (KeStartProfile(KiA9ErrataProfileObject) == FALSE) {
            return FALSE;
        }
    }

    //
    // Ask the HAL whether to apply an errata fix which necessitates sending IPIs 
    // to flush on global TLB changes. 
    //
    // N.B. The check must be done here, and not in KiTbInitialize(), because the 
    //      HAL is not yet initialized when KiTbInitialize() is called.
    //

    Status = HalQuerySystemInformation(HalQueryArmErrataInformation,
                                        sizeof(ArmErrataInfo),
                                        &ArmErrataInfo,
                                        &Size);

    if (NT_SUCCESS(Status)) {

        //
        // If the HAL has requested it, force IPIs for broadcast flushes.
        //

        if (ArmErrataInfo.SignalIpiOnTlbFlush != FALSE) {
            KiTbIpiTypeToSendForBroadcast = IPI_FLUSH_ALL;
            KiTbSendIpiForBroadcastFlushes = TRUE;
        }
    }

    return TRUE;
}

VOID
KiSetCacheInformation (
    VOID
    )

/*++

Routine Description:

    This function sets the current processor cache information in the PCR and
    PRCB.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PCACHE_DESCRIPTOR Cache;
    ULONG i;
    PKPCR Pcr;
    PKPRCB Prcb;

    Pcr = KeGetPcr();
    Prcb = KeGetCurrentPrcb();

    //
    // Scan through the cache descriptors initialized by the processor
    // specific code and compute cache parameters for page coloring.
    //

    Cache = Prcb->Cache;
    for (i = 0; i < Prcb->CacheCount; i++) {
        if (((Cache->Type == CacheData) || (Cache->Type == CacheUnified)) &&
            (Cache->Size > Pcr->SecondLevelCacheSize)) {

            Pcr->SecondLevelCacheSize = Cache->Size;
            Pcr->SecondLevelCacheAssociativity = Cache->Associativity;
        }

        Cache += 1;
    }

    return;
}

VOID
KiSetFeatureBits (
    IN PKPRCB Prcb
    )

/*++

Routine Description:

    Set the current processor feature bits in the PRCB.

Arguments:

    Prcb - Supplies a pointer to the current processor block.

Return Value:

    None.

--*/

{

    KCPU_FEATURES FeatureBits;
    ARM_IDCODE Id;

    //
    // This code only supports the ARMv7 architecture level and beyond
    // which uses a feature bit-style mechanism to indicate core
    // capabilities.
    //

    Id.Ulong = _MoveFromCoprocessor (CP15_MIDR);
    if (Id.Architecture != 0xF) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 1, Id.Ulong, 0, 0);
    }

    //
    // Verify minimum instruction set requirements; this is pretty much
    // everything defined in the ARMv7 specification.
    //

    if (READ_ARM_FEATURE(CP15_ISAR0, ISAR0_BITCOUNT) < ISAR0_BITCOUNT_CLZ) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 0, ISAR0_BITCOUNT, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR0, ISAR0_BITFIELD) < ISAR0_BITFIELD_BFC_ETC) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 0, ISAR0_BITFIELD, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR0, ISAR0_CMPBRANCH) < ISAR0_CLZ) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 0, ISAR0_CMPBRANCH, 0);
    }

    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_EXCEPT) < ISAR1_EXCEPT_LDM_STM) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_EXCEPT, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_EXCEPT_AR) < ISAR1_EXCEPT_AR_SRS) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_EXCEPT_AR, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_EXTEND) < ISAR1_EXTEND_16) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_EXTEND, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_IFTHEN) < ISAR1_IFTHEN_IT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_IFTHEN, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_IMMEDIATE) < ISAR1_IMMEDIATE_MOVT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_IMMEDIATE, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR1, ISAR1_INTERWORK) < ISAR1_INTERWORK_THUMB2) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 1, ISAR1_INTERWORK, 0);
    }

    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_LOADSTORE) < ISAR2_LOADSTORE_LDRD) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_LOADSTORE, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_MEMHINT) < ISAR2_MEMHINT_PLDW) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_MEMHINT, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_MULT) < ISAR2_MULT_MLS) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_MULT, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_MULTS) < ISAR2_MULTS_SMLAD) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_MULTS, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_MULTU) < ISAR2_UMAAL) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_MULTU, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_PSR_AR) < ISAR2_PSR_AR_MRS_MSR) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_PSR_AR, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_REVERSAL) < ISAR2_REVERSAL_RBIT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 2, ISAR2_REVERSAL, 0);
    }

    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_SATURATE) < ISAR3_SATURATE_QADD) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_SATURATE, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_SIMD) < ISAR3_SIMD_PKHBT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_SIMD, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_SVC) < ISAR3_SVC_SVC) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_SVC, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_SYNCHPRIM) < ISAR3_SYNCHPRIM_LDREXD) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_SYNCHPRIM, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_TABBRANCH) < ISAR3_TABBRANCH_TBB) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_TABBRANCH, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_THUMBCOPY) < ISAR3_THUMBCOPY_MOV) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_THUMBCOPY, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR3, ISAR3_TRUENOP) < ISAR3_TRUENOP_NOP) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 3, ISAR3_TRUENOP, 0);
    }

    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_UNPRIV) < ISAR4_UNPRIV_LDRHT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_UNPRIV, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_WITHSHIFTS) < ISAR4_WITHSHIFTS_REG) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_WITHSHIFTS, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_WRITEBACK) < ISAR4_WRITEBACK_FULL) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_WRITEBACK, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_SMC) < ISAR4_SMC_SMC) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_SMC, 0);
    }
    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_BARRIER) < ISAR4_BARRIER_DMB) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_BARRIER, 0);
    }

    //
    // Documentation about SyncPrim_frac is confusing; real v7 systems read back 0.
    // For now, just don't check it at all, relying on SyncPrim in ISAR3 to tell all.
    //
//  if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_SYNCHPRIM_FRAC) < ISAR4_SYNCHPRIM_FRAC_CLREX) {
//      KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_SYNCHPRIM_FRAC, 0);
//  }

    if (READ_ARM_FEATURE(CP15_ISAR4, ISAR4_UNPRIV) < ISAR4_UNPRIV_LDRHT) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 2, 4, ISAR4_UNPRIV, 0);
    }

    //
    // Set processor family and stepping information.
    //

    Prcb->ProcessorModel = (USHORT) Id.Model;
    Prcb->ProcessorRevision = (USHORT) (Id.Revision << 8 | Id.MinorRevision);

    //
    // Build vendor string
    //

    Prcb->ProcessorVendorString[0] = (UCHAR) Id.Implementer;
    Prcb->ProcessorVendorString[1] = '\0';

    //
    // Get CPU feature information.
    //

    FeatureBits = KF_NOEXECUTE | KF_LARGE_PAGE;

    //
    // Check for presence of MP extensions which are designed to
    // increase the efficiency of MP (and UP) platforms.  ARM allows
    // for the possibility that the core may implement a subset of the
    // MP extensions, but we don't.
    //

    if ((READ_ARM_FEATURE(CP15_MMFR2, MMFR2_UTLB) >= MMFR2_UTLB_VA_ALL) &&
        (READ_ARM_FEATURE(CP15_MMFR3, MMFR3_BCAST) >= MMFR3_BCAST_ALL) &&
        (READ_ARM_FEATURE(CP15_MMFR3, MMFR3_HPW) >= MMFR3_HPW_COHERENT)) {

        FeatureBits |= KF_MPEXT;

    } else {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     2,
                     _MoveFromCoprocessor(CP15_MMFR2),
                     _MoveFromCoprocessor(CP15_MMFR3),
                     0);
    }

    //
    // Snap TBR configuration setup by the boot loader
    //

    KiTbrConfiguration = (ULONG) _MoveFromCoprocessor(CP15_TTBR0);
    KiTbrConfiguration &= CP15_TTBRx_VALID_MPEXT_MASK;

    //
    // The security extensions and CP14-style debug registers are required.
    //

    if ((READ_ARM_FEATURE(CP15_PFR1, PFR1_SECURITY) < PFR1_SECURITY_TZONE) ||
        (READ_ARM_FEATURE(CP15_DFR0, DFR0_CP14_DEBUG) < DFR0_CP14_DEBUG_V7)) {

        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     3,
                     _MoveFromCoprocessor(CP15_PFR1),
                     _MoveFromCoprocessor(CP15_DFR0),
                     0);
    }

    //
    // Check for SDIV/UDIV support
    //

    if (READ_ARM_FEATURE(CP15_ISAR0, ISAR0_DIVIDE) >= ISAR0_DIVIDE_DIV) {
        SharedUserData->ProcessorFeatures[PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE] = TRUE;
    }

    //
    // Check for ARMv8 instructions
    //

    try {
        if (READ_ARM_FEATURE(CP15_ISAR2, ISAR2_LOADSTORE) >= ISAR2_LOADSTORE_ACQREL &&
            READ_ARM_FEATURE(CPVFP_MVFR2, MVFR2_FPMISC) >= MVFR2_FPMISC_MINMAX &&
            READ_ARM_FEATURE(CPVFP_MVFR2, MVFR2_SIMDMISC) >= MVFR2_SIMDMISC_MINMAX) {

            SharedUserData->ProcessorFeatures[PF_ARM_V8_INSTRUCTIONS_AVAILABLE] = TRUE;
        }

    } except(EXCEPTION_EXECUTE_HANDLER) {
        NOTHING;
    }

    //
    // Check for ARMv8 crypto instructions
    //

    try {
        if (READ_ARM_FEATURE(CP15_ISAR5, ISAR5_AES) >= ISAR5_AES_PMULL &&
            READ_ARM_FEATURE(CP15_ISAR5, ISAR5_SHA1) >= ISAR5_SHA1_SHA1C &&
            READ_ARM_FEATURE(CP15_ISAR5, ISAR5_SHA2) >= ISAR5_SHA2_SHA256H) {

            SharedUserData->ProcessorFeatures[PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE] = TRUE;
        }

    } except(EXCEPTION_EXECUTE_HANDLER) {
        NOTHING;
    }

    //
    // Check for ARMv8 CRC32 instructions
    //

    try {
        if (READ_ARM_FEATURE(CP15_ISAR5, ISAR5_CRC32) >= ISAR5_CRC32_IMP) {

            SharedUserData->ProcessorFeatures[PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE] = TRUE;
        }

    } except(EXCEPTION_EXECUTE_HANDLER) {
        NOTHING;
    }

    //
    // Check for PXN support
    //

    if (READ_ARM_FEATURE(CP15_MMFR0, MMFR0_VMSA) >= MMFR0_VMSA_V7PXN) {
        FeatureBits |= KF_PXN;
    }

    //
    // Check for atomic 64bit load/stores
    //

    if (READ_ARM_FEATURE(CP15_MMFR0, MMFR0_VMSA) >= MMFR0_VMSA_LPAE) {
        SharedUserData->ProcessorFeatures[PF_ARM_64BIT_LOADSTORE_ATOMIC] = TRUE;
    }

    //
    // Determine the number of cores per physical processor and the number
    // of logical processors per core.
    //
    // N.B. On ARM, it is not possible to determine these values in
    //      advance of starting the other processors. To avoid
    //      conflicts with per-package licensing, we set these values
    //      high now and update them later after all processors have
    //      been started.
    //

    Prcb->LogicalProcessorsPerCore = 8;
    Prcb->CoresPerPhysicalProcessor = 8;

    //
    // Save per core affinity information for later processing
    //

    Prcb->MPAffinity = _MoveFromCoprocessor(CP15_MPIDR);

    //
    // Determine which floating point features this processor supports.
    //

    FeatureBits |= KiInitializeVfp();

    //
    // Apply model specific fixes
    //

    if ((Id.Implementer == 'Q') &&
        ((Id.Model & 0xFC0) == 0x40)) {

        if (Id.Revision == 0) {

            //
            // Qualcomm Krait r0px claims to have functionality it does
            // not including LPAE, Virtualization, and SDIV/DIV.
            //

            FeatureBits &= ~KF_PXN;
            SharedUserData->ProcessorFeatures[PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE] = FALSE;
            SharedUserData->ProcessorFeatures[PF_ARM_64BIT_LOADSTORE_ATOMIC] = FALSE;
        }

        if (Id.Revision <= 1) {

            //
            // Qualcomm Krait r[0,1]px has FMAC but doesn't expose it.
            //

            SharedUserData->ProcessorFeatures[PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE] = TRUE;
        }
    }

    if (Id.Implementer == 'A') {

        //
        // Cortex-A8 errata 728018 - Cache maintenance operations by
        //    MVA for a non-cacheable memory region can result in
        //    processor deadlock
        //
        // Addressed by promoting cache attribute flushes to full
        // cache flushes.  Cache management for I/O coherency has not
        // been effected.
        //

        if (Id.Model == 0xC08) {
            FeatureBits |= KF_PROMOTE_ATTRIBUTE_FLUSH;
        }
    }

    //
    // Allow hypervisor to modify visible processor features.
    //

    //HvlFilterProcessorFeatures(&FeatureBits);
    Prcb->FeatureBits = FeatureBits;

    return;
}

VOID
KiConfigureDynamicProcessor (
    VOID
    )

/*++

Description:

    This routine is called on a new dynamic processor to perform configuration
    of the processor.  It exists as a separate routine from those called during
    normal processor startup to perform tasks that are only done once for all
    processors at boot, and are therefore not part of the common processor init
    path.  This routine is called at IPI level.

Parameters

    None.

Return Value:

    None.

--*/

{

    return;
}


ULONG
KiInitializeVfp(
    VOID
    )

/*++

Routine Description:

    This routine will inspect the processor to determine if hardware support
    for floating point exists, and if so, which features are present.

 Arguments:

    NONE.

 Return Value:

    Feature bits that indicate specific hardware features available on the
    platform.

--*/

{

    ULONG CpacrValue;
    KCPU_FEATURES FeatureBits;
    ULONG FpexcValue;
    ULONG FpsidValue;

    FeatureBits = 0;

    //
    // Configure the CPACR register to enable full VFP and Advanced SIMD
    // support by clearing the disable bits and enabling CP10/CP11 coprocessors.
    //

    CpacrValue = _MoveFromCoprocessor(CP15_CPACR);
    CpacrValue &= ~(CP15_CPACR_D32DIS | CP15_CPACR_ASEDIS);
    CpacrValue |= CP15_CPACR_VFP_MASK;
    _MoveToCoprocessor(CpacrValue, CP15_CPACR);
    _InstructionSynchronizationBarrier();

    //
    // Read back the CPACR value to make sure our changes stuck; if not, VFP
    // support is not present.
    //

    CpacrValue = _MoveFromCoprocessor(CP15_CPACR);
    if ((CpacrValue & CP15_CPACR_VFP_MASK) != CP15_CPACR_VFP_MASK) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     4,
                     CpacrValue,
                     0,
                     0);
    }

    //
    // The processor will consume floating point instructions, but does
    // it require additional software emulation support?
    //

    FpsidValue = _MoveFromCoprocessor(CPVFP_FPSID);
    if ((FpsidValue & CPVFP_FPSID_SW) != 0) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR, 5, FpsidValue, 0, 0);
    }

    //
    // Verify that the complete set of required floating-point features is present
    // in hardware:
    //
    //    1. All rounding modes
    //    2. VSQRT support
    //    3. VDIV support
    //    4. VFPv3 double-precision support
    //    5. VPFv3 single-precision support
    //
    // In the future, we may want to also require:
    //
    //    6. Propogation of NaN values
    //    7. Full denormalized arithmetic
    //

    if ((READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_VFPRND) < MVFR0_VFPRND_ALL) ||
        (READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_FSQRT) < MVFR0_FSQRT_SUPPORTED) ||
        (READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_FDIV) < MVFR0_FDIV_SUPPORTED) ||
        (READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_VDPREC) < MVFR0_VDPREC_V3) ||
        (READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_VSPREC) < MVFR0_VSPREC_V3)/* ||
        (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_VDNAN) < MVFR1_VDNAN_SUPPORTED) ||
        (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_VFTZ) < MVFR1_VFTZ_SUPPORTED)*/) {

        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     6,
                     _MoveFromCoprocessor(CPVFP_MVFR0),
                     0,
                     0);
    }

    //
    // Check if the processor has 32 floating-point/SIMD registers rather than
    // only 16.
    //

    if (READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_AREGS) >= MVFR0_AREGS_32) {
        FeatureBits |= KF_VFP_32REG;
    }

    //
    // Check if the processor has FPINST or FPINST2 support
    //

    _MoveToCoprocessor(CPVFP_FPEXC_EN | CPVFP_FPEXC_EX | CPVFP_FPEXC_FP2V, CPVFP_FPEXC);
    _InstructionSynchronizationBarrier();

    FpexcValue = _MoveFromCoprocessor(CPVFP_FPEXC);
    if ((FpexcValue & CPVFP_FPEXC_EX) != 0) {
        FeatureBits |= KF_VFP_FPINST;

        if ((FpexcValue & CPVFP_FPEXC_FP2V) != 0) {
            FeatureBits |= KF_VFP_FPINST2;
        }
    }

    _MoveToCoprocessor(CPVFP_FPEXC_EN, CPVFP_FPEXC);
    _InstructionSynchronizationBarrier();

    //
    // See if the processor has the Advanced SIMD instruction support
    // by verifying:
    //
    //    1. Advanced SIMD registers present
    //    2. Single-precision floating-point SIMD support present
    //    3. Integer SIMD support present
    //    4. Advanced SIMD load/store present
    //

    if ((READ_ARM_FEATURE(CPVFP_MVFR0, MVFR0_AREGS) >= MVFR0_AREGS_32) &&
        (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_ASPREC) >= MVFR1_ASPREC_SUPPORTED) &&
        (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_AINT) >= MVFR1_AINT_SUPPORTED) &&
        (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_ALDSTR) >= MVFR1_ALDSTR_SUPPORTED)) {

        FeatureBits |= KF_SIMD;
    }

    //
    // Check for fuse multiply accumulate support
    //

    if (READ_ARM_FEATURE(CPVFP_MVFR1, MVFR1_ASFMAC) >= MVFR1_ASFMAC_SUPPORTED) {
        SharedUserData->ProcessorFeatures[PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE] = TRUE;
    }

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
    //
    // Now disable access to floating point state
    //

    CpacrValue &= ~CP15_CPACR_VFP_MASK;
    _MoveToCoprocessor(CpacrValue, CP15_CPACR);
    _InstructionSynchronizationBarrier();
#endif

    return FeatureBits;
}

VOID
KiInitializeDebugRegisters (
    VOID
    )

/*++

Routine Description:

    This routine initialized the CP14 debug state on the current processor.

 Arguments:

    None.

 Return Value:

    None.

--*/

{

    ULONG Value;

    //
    // Test for implementation of debug registers
    //

    Value = READ_ARM_FEATURE(CP15_DFR0, DFR0_CP14_DEBUG);
    if (Value < DFR0_CP14_DEBUG_V7) {
        KeBugCheckEx(UNSUPPORTED_PROCESSOR,
                     7,
                     _MoveFromCoprocessor(CP15_DFR0),
                     0,
                     0);
    }

    //
    // Initialize the debug state.
    //

    KiInitializeCP14DebugState();
    return;
}

