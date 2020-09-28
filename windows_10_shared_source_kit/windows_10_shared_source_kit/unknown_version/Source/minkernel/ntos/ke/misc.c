/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    misc.c

Abstract:

    This module implements machine dependent miscellaneous kernel functions.

Environment:

    Kernel mode only.

--*/

#include "ki.h"
#include <winerror.h>


#pragma alloc_text(INIT, KiEnableNXSupport)
#pragma alloc_text(PAGELK, KiIsNXSupported)

KPROCESSOR_MODE
KiGetTrapFrameMode (
    _In_ PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This routine returns the processor mode associated with the specified
    trap frame.

Arguments:

    TrapFrame - Supplies the address of a trap frame.

Return Value:

    Processor mode of the trap frame.

--*/

{

    if ((TrapFrame->Cpsr & CPSRM_MASK) != CPSRM_SVC) {

        NT_ASSERT(KeAreAllApcsDisabled() != FALSE);

        return UserMode;
    }

    return KernelMode;
}

PKARM_VFP_STATE
KiGetVfpStatePointer(
    _In_ PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    Returns a pointer to a KARM_VFP_STATE that represents the
    VFP state at the time of a trap frame.

Arguments:

    TrapFrame - Supplies a pointer to the trap frame in question.

Return Value:

    A pointer to the correct VFP state.

--*/

{
    PKARM_VFP_STATE VfpState;

    VfpState = TrapFrame->VfpState;

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)

    if (VfpState == NULL) {
        PKTHREAD Thread;
        Thread = KeGetCurrentThread();
        if (Thread != NULL) {
            VfpState = Thread->VfpState;
        }
    }

#endif

    if (VfpState == NULL) {
        return NULL;
    }

    NT_ASSERT(VfpState->Fpscr != -1);

    return VfpState;
}

VOID
KiSetDebugActive (
    __inout PKTHREAD Thread,
    __in UCHAR DebugActive,
    __in UCHAR DebugActiveMask
    )

/*++

Routine Description:

    This routine updates the debug active flag in the thread with the
    indicated value in the presence of concurrent modification.

Arguments:

    Thread - Supplies the thread whose DebugActive field should be modified.

    DebugActive - Supplies the debug active flag.

    DebugActiveMask - Supplies the mask of bits to be edited in debug active.

Return Value:

    None.

--*/

{
    LONG OldMask;
    LONG NewMask;

    NT_ASSERT (Thread == KeGetCurrentThread ());

    PrefetchForWrite(&Thread->Header.Lock);
    NewMask = ReadNoFence(&Thread->Header.Lock);
    while (TRUE) {
        OldMask = NewMask;

        //
        // Atomically update the Thread->Header.DebugActive field.
        //

        NewMask = (OldMask & ~(DebugActiveMask << DEBUG_ACTIVE_BIT_OFFSET)) |
            ((ULONG)DebugActive << DEBUG_ACTIVE_BIT_OFFSET);

        if ((NewMask = InterlockedCompareExchange (&Thread->Header.Lock,
                                                   NewMask,
                                                   OldMask)) == OldMask) {

            break;
        }
    }

    return;
}

extern
KINLINE
BOOLEAN
KeIsContextSwapActive (
    __in PRKTHREAD Thread
    )

/*++

Routine Description:

    This routine determines if the current thread is in the midst of
    a context swap. If false, then the current thread will be executing
    on its own stack, rather than the stack of the previously executing
    thread.

Arguments:

    Thread - The currently executing thread.

Return Value:

    Boolean result.

--*/

{
    BOOLEAN Enabled;
    PVOID SpBase;

    Enabled = KeDisableInterrupts();
    SpBase = KeGetCurrentPrcb()->SpBase;
    if (Enabled) {
        _enable();
    }

    return Thread->InitialStack != SpBase;

}

VOID
KiWaitForReboot (
    VOID
    )

/*++

Routine Description:

    Frozen processors are resumed to this routine in the event that
    a .reboot is being processed.

Arguments:

    None.

Return Value:

    Never returns.

--*/

{
    while (TRUE) {
        NOTHING;
    }
}

VOID
KiSaveProcessorControlState (
     __inout PKPROCESSOR_STATE ProcessorState
     )

/*++

Routine Description:

    This routine saves the control state of the current processor.

Arguments:

    ProcessorState - Supplies a pointer to a processor state structure.

Return Value:

    None.

--*/

{

    ULONG ContextFlags;
    KCPU_FEATURES FeatureBits;
    PKSPECIAL_REGISTERS SpecialRegisters;
    PKARM_ARCH_STATE ArchState;

    ContextFlags = ProcessorState->ContextFrame.ContextFlags;
    SpecialRegisters = &ProcessorState->SpecialRegisters;
    ArchState = &ProcessorState->ArchState;

    ArchState->Cp15_Cr1_Control = _MoveFromCoprocessor(CP15_SCTLR);
    ArchState->Cp15_Cr1_AuxControl = _MoveFromCoprocessor(CP15_ACTLR);
    ArchState->Cp15_Cr1_Cpacr = _MoveFromCoprocessor(CP15_CPACR);

    ArchState->Cp15_Cr2_Ttb0 = KiTbrConfiguration |
        (_MoveFromCoprocessor(CP15_TTBR0) & CP15_TTBRx_PD_MASK);

    ArchState->Cp15_Cr2_TtbControl = _MoveFromCoprocessor(CP15_TTBCR);
    ArchState->Cp15_Cr2_Ttb1 = _MoveFromCoprocessor(CP15_TTBR1);
    ArchState->Cp15_Cr3_Dacr = _MoveFromCoprocessor(CP15_DACR);
    ArchState->Cp15_Cr5_Dfsr = _MoveFromCoprocessor(CP15_DFSR);
    ArchState->Cp15_Cr5_Ifsr = _MoveFromCoprocessor(CP15_IFSR);
    ArchState->Cp15_Cr6_Dfar = _MoveFromCoprocessor(CP15_DFAR);
    ArchState->Cp15_Cr6_Ifar = _MoveFromCoprocessor(CP15_IFAR);

    ArchState->Cp15_Cr10_PrimaryMemoryRemap =
        _MoveFromCoprocessor(CP15_PRRR);

    ArchState->Cp15_Cr10_NormalMemoryRemap =
        _MoveFromCoprocessor(CP15_NMRR);

    ArchState->Cp15_Cr13_ContextId =
        _MoveFromCoprocessor(CP15_CONTEXTIDR);

    ArchState->Cp15_Cr12_VBARns = _MoveFromCoprocessor(CP15_VBAR);

    //
    // Save thread registers.
    //

    SpecialRegisters->Cp15_Cr13_UsrRW = _MoveFromCoprocessor(CP15_TPIDRURW);
    SpecialRegisters->Cp15_Cr13_UsrRO = _MoveFromCoprocessor(CP15_TPIDRURO);
    SpecialRegisters->Cp15_Cr13_SvcRW = _MoveFromCoprocessor(CP15_TPIDRPRW);

    //
    // Save banked state.
    //

    KiReadBankedRegisters(CPSRM_SYS,
                      &SpecialRegisters->UserSp,
                      &SpecialRegisters->UserLr,
                      NULL);

    KiReadBankedRegisters(CPSRM_ABT,
                      &SpecialRegisters->AbortSp,
                      &SpecialRegisters->AbortLr,
                      &SpecialRegisters->AbortSpsr);

    KiReadBankedRegisters(CPSRM_UDF,
                      &SpecialRegisters->UdfSp,
                      &SpecialRegisters->UdfLr,
                      &SpecialRegisters->UdfSpsr);

    KiReadBankedRegisters(CPSRM_INT,
                      &SpecialRegisters->IrqSp,
                      &SpecialRegisters->IrqLr,
                      &SpecialRegisters->IrqSpsr);

    //
    // Save FP unit control state. Start with a default state and then modify it
    // based on what is supported in the processor.
    //

    SpecialRegisters->Fpexc = CPVFP_FPEXC_EN;
    SpecialRegisters->Fpinst = 0;
    SpecialRegisters->Fpinst2 = 0;
#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
    if (1) {
#else
    if ((ArchState->Cp15_Cr1_Cpacr & CP15_CPACR_VFP_MASK) == CP15_CPACR_VFP_MASK) {
#endif
        SpecialRegisters->Fpexc = _MoveFromCoprocessor(CPVFP_FPEXC);
        FeatureBits = KeGetCurrentPrcb()->FeatureBits;
        if ((FeatureBits & KF_VFP_FPINST) != 0) {

            SpecialRegisters->Fpinst = _MoveFromCoprocessor(CPVFP_FPINST);
            if ((FeatureBits & KF_VFP_FPINST2) != 0) {
                SpecialRegisters->Fpinst2 = _MoveFromCoprocessor(CPVFP_FPINST2);
            }
        }
    }

    //
    // Save debug register state.
    //

    if (KiCP14DebugEnabled != FALSE) {
        KiReadHwDebugRegs(SpecialRegisters->KernelBvr,
                          SpecialRegisters->KernelBcr,
                          SpecialRegisters->KernelWvr,
                          SpecialRegisters->KernelWcr);

    }

    return;
}

VOID
KiRestoreProcessorControlState (
    __in PKPROCESSOR_STATE ProcessorState
    )

/*++

Routine Description:

    This routine restores the control state of the current processor.

Arguments:

    ProcessorState - Supplies a pointer to a processor state structure.

Return Value:

    None.

--*/

{

    ULONG ContextFlags;
    BOOLEAN DebugActive;
    KCPU_FEATURES FeatureBits;
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
    ULONG OriginalCpacr;
#endif
    PKSPECIAL_REGISTERS SpecialRegisters;

    ContextFlags = ProcessorState->ContextFrame.ContextFlags;
    SpecialRegisters = &ProcessorState->SpecialRegisters;

    // FIXME: Restore other kspecial regs here as well if needed

    //
    // Restore thread registers.
    //

    _MoveToCoprocessor(SpecialRegisters->Cp15_Cr13_UsrRW, CP15_TPIDRURW);
    _MoveToCoprocessor(SpecialRegisters->Cp15_Cr13_UsrRO, CP15_TPIDRURO);
    _MoveToCoprocessor(SpecialRegisters->Cp15_Cr13_SvcRW, CP15_TPIDRPRW);

    //
    // Restore banked state.
    //

    KiWriteBankedRegisters(CPSRM_SYS,
                      &SpecialRegisters->UserSp,
                      &SpecialRegisters->UserLr,
                      NULL);

    KiWriteBankedRegisters(CPSRM_ABT,
                      &SpecialRegisters->AbortSp,
                      &SpecialRegisters->AbortLr,
                      &SpecialRegisters->AbortSpsr);

    KiWriteBankedRegisters(CPSRM_UDF,
                      &SpecialRegisters->UdfSp,
                      &SpecialRegisters->UdfLr,
                      &SpecialRegisters->UdfSpsr);

    KiWriteBankedRegisters(CPSRM_INT,
                      &SpecialRegisters->IrqSp,
                      &SpecialRegisters->IrqLr,
                      &SpecialRegisters->IrqSpsr);

    //
    // Restore floating point register state.
    //

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
    OriginalCpacr = _MoveFromCoprocessor(CP15_CPACR);
    if ((OriginalCpacr & CP15_CPACR_VFP_MASK) == 0) {
        _MoveToCoprocessor(OriginalCpacr | CP15_CPACR_VFP_MASK, CP15_CPACR);
        _InstructionSynchronizationBarrier();
    }
#endif
    FeatureBits = KeGetCurrentPrcb()->FeatureBits;
    _MoveToCoprocessor(SpecialRegisters->Fpexc | CPVFP_FPEXC_EN, CPVFP_FPEXC);
    if ((FeatureBits & KF_VFP_FPINST) != 0) {
        _MoveToCoprocessor(SpecialRegisters->Fpinst, CPVFP_FPINST);
        if ((FeatureBits & KF_VFP_FPINST2) != 0) {
            _MoveToCoprocessor(SpecialRegisters->Fpinst2, CPVFP_FPINST2);
        }
    }
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
    if ((OriginalCpacr & CP15_CPACR_VFP_MASK) == 0) {
        _MoveToCoprocessor(OriginalCpacr, CP15_CPACR);
        _InstructionSynchronizationBarrier();
    }
#endif

    //
    // Restore debug register state.
    //

    if (KiCP14DebugEnabled != FALSE) {

        //
        // Re-initialize CP14 debug state.
        //

        KiInitializeCP14DebugState();

        //
        // Enable/Disable debugging if debug state has changed since
        // it was last saved.
        //

        KiWriteHwDebugRegs(SpecialRegisters->KernelBvr,
                           SpecialRegisters->KernelBcr,
                           SpecialRegisters->KernelWvr,
                           SpecialRegisters->KernelWcr);

        DebugActive = KiIsArmedForDebug(SpecialRegisters->KernelBcr,
                                        SpecialRegisters->KernelWcr);

        KiUpdateDbgdscr(DebugActive);
    }

    return;
}

VOID
KiRestoreArchitecturalState (
     __inout PKPROCESSOR_STATE ProcessorState
     )

/*++

Routine Description:

    This routine restores architectural state.

Arguments:

    ProcessorState - Supplies a pointer to a processor state structure.

Return Value:

    None.

--*/

{

    PKARM_ARCH_STATE ArchState;

    ArchState = &ProcessorState->ArchState;
    _MoveToCoprocessor(ArchState->Cp15_Cr1_Control, CP15_SCTLR);
    _MoveToCoprocessor(ArchState->Cp15_Cr1_Cpacr, CP15_CPACR);
    _MoveToCoprocessor(ArchState->Cp15_Cr3_Dacr, CP15_DACR);
    _MoveToCoprocessor(ArchState->Cp15_Cr5_Dfsr, CP15_DFSR);
    _MoveToCoprocessor(ArchState->Cp15_Cr5_Ifsr, CP15_IFSR);
    _MoveToCoprocessor(ArchState->Cp15_Cr6_Dfar, CP15_DFAR);
    _MoveToCoprocessor(ArchState->Cp15_Cr6_Ifar, CP15_IFAR);
    _MoveToCoprocessor(ArchState->Cp15_Cr10_PrimaryMemoryRemap, CP15_PRRR);
    _MoveToCoprocessor(ArchState->Cp15_Cr10_NormalMemoryRemap, CP15_NMRR);
    _MoveToCoprocessor(ArchState->Cp15_Cr13_ContextId, CP15_CONTEXTIDR);
    _MoveToCoprocessor(ArchState->Cp15_Cr12_VBARns, CP15_VBAR);
    _InstructionSynchronizationBarrier();
    return;
}

VOID
KiInitializeCP14DebugState (
    VOID
    )

/*++

Routine Description:

    This routine initializes the CP14 debug state on the current processor.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG Breakpoints;
    ULONG Didr;
    PKPRCB Prcb;
    ULONG Value;
    ULONG Watchpoints;

    Prcb = KeGetCurrentPrcb();

    //
    // Unlock debug registers
    //

    Value = _MoveFromCoprocessor(CP14_DBGOSLSR);
    if ((Value & CP14_DBGOSLSR_LOCKED) != 0) {
        _MoveToCoprocessor(0, CP14_DBGOSLAR);
        _DataSynchronizationBarrier();
        Value = _MoveFromCoprocessor(CP14_DBGOSLSR);

        NT_ASSERT((Value & CP14_DBGOSLSR_LOCKED) == 0);

    }

    if (READ_ARM_FEATURE(CP15_DFR0, DFR0_CP14_DEBUG) == DFR0_CP14_DEBUG_V7) {

        //
        // Clear sticky power down registers by reading the status
        // only on the v7 debug architecture.  This register was
        // removed on the v7.1 debug architecture.
        //
        // N.B. Currently required to clear sticky power bit on
        //      Qualcomm 8660 platforms.  Arguably a firmware bug since
        //      it shouldn't get into that state in the first place.
        //

        do {
            Value = _MoveFromCoprocessor(CP14_DBGPRSR);
            _InstructionSynchronizationBarrier();
        } while ((Value & CP14_DBGPRSR_STICKYPD) != 0);
    }

    //
    // Sanitize debug status and control. Disable user-mode access to CP14 debug
    // registers.
    //

    Value = _MoveFromCoprocessor(CP14_DBGDSCR);
    Value |= CP14_DBGDSCR_UDCCdis;
    _MoveToCoprocessor(Value, CP14_DBGDSCR);
    _InstructionSynchronizationBarrier();
    _MoveToCoprocessor(0, CP14_DBGVCR);

    //
    // Determine how many breakpoints and watchpoints are
    // implemented.
    //

    Didr = (ULONG)_MoveFromCoprocessor(CP14_DBGDIDR);
    Breakpoints = ((Didr & DIDR_BKPT_MASK) >> DIDR_BKPT_SHIFT) + 1;
    Watchpoints = ((Didr & DIDR_WPT_MASK) >> DIDR_WPT_SHIFT) + 1;

    //
    // Each processor supports at least two breakpoints and one watchpoint.
    //

    NT_ASSERT((Breakpoints >= 2) && (Watchpoints >= 1));

    //
    // Ensure that even debug registers not being used are initialized
    // to a safe state.
    //

    KiSetHwDebugRegs(Breakpoints,
                     KiZeroedDebugRegs,
                     KiZeroedDebugRegs,
                     Watchpoints,
                     KiZeroedDebugRegs,
                     KiZeroedDebugRegs);

    //
    // Perform one-time initialization of [break/watch]point count in the PRCB.
    //

    if (Prcb->MaxBreakpoints == 0) {

        NT_ASSERT(Prcb->MaxWatchpoints == 0);

        //
        // Constrain the number of supported breakpoints and watchpoints
        // based on the storage area allocated in the CONTEXT and
        // KTRAPFRAME structures.
        //

        if (Breakpoints > ARM_MAX_BREAKPOINTS) {
            Breakpoints = ARM_MAX_BREAKPOINTS;
        }

        if (Watchpoints > ARM_MAX_WATCHPOINTS) {
            Watchpoints = ARM_MAX_WATCHPOINTS;
        }

        Prcb->MaxBreakpoints = Breakpoints;
        Prcb->MaxWatchpoints = Watchpoints;

    } else {

        NT_ASSERT((Prcb->MaxBreakpoints >= 2) &&
                  (Prcb->MaxWatchpoints >= 1) &&
                  (Prcb->MaxBreakpoints <= ARM_MAX_BREAKPOINTS) &&
                  (Prcb->MaxWatchpoints <= ARM_MAX_WATCHPOINTS));

    }

    return;
}

VOID
KeRestoreProcessorState (
    __in PKPROCESSOR_STATE ProcessorState
    )

/*++

Routine Description:

    Restores all processor-specific state that must be preserved
    across a processor reset.

Arguments:

    ProcessorState - Supplies the KPROCESSOR_STATE where the current CPU's
        state was previously saved.

Return Value:

    None.

--*/

{

    KiRestoreArchitecturalState(ProcessorState);
    KiRestoreProcessorControlState(ProcessorState);
    RtlRestoreContext(&ProcessorState->ContextFrame, NULL);
    return;
}

VOID
KiEnableNXSupport (
    VOID
    )

/*++

Routine Description:

    This function enables NX support on the current processor.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Set NX enabled in user shared page.
    //

    SharedUserData->ProcessorFeatures[PF_NX_ENABLED] = TRUE;
    return;
}

BOOLEAN
KiIsNXSupported (
    VOID
    )

/*++

Routine Description:

    Determines whether or not this processor supports NX protection.

Arguments:

    None.

Return Value:

    A flag indicating whether or not this processor supports NX protection.

--*/

{

    //
    // All supported ARM processors must have NX support.
    //

    return TRUE;
}

#include "kimisc.h"

#pragma data_seg("INITDATA")

ULONG_PTR KiInitData[] = {
    11,
    sizeof(KUSER_SHARED_DATA),
    sizeof(SEVALIDATEIMAGE_CALLBACKS),
    0,
    0,
    0,
    KOBJECT_LOCK_BIT_NUMBER,
    0,
    sizeof(CI_MINTCB_LIST) * SE_MIN_TCB_ENTRIES,
    sizeof(RTL_PROTECTED_ACCESS) * PsProtectedSignerMax,
    sizeof(CI_PROTECTED_MAPPING) * PsProtectedSignerMax
};

#pragma data_seg()



