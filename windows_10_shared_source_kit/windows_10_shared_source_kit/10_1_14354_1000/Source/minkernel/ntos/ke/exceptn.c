/*++

Copyright (c) Microsoft Corporation

Module Name:

    exceptn.c

Abstract:

    This module implement the code necessary to dispatch expections to the
    proper mode and invoke the exception dispatcher.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

#define KI_GET_CP14_BREAKPOINT(_pAddress, _pControl, _Num)        \
    *(PULONG)_pAddress = _MoveFromCoprocessor(CP14_DBGBVR##_Num); \
    *(PULONG)_pControl = _MoveFromCoprocessor(CP14_DBGBCR##_Num)

#define KI_GET_CP14_WATCHPOINT(_pAddress, _pControl, _Num)        \
    *(PULONG)_pAddress = _MoveFromCoprocessor(CP14_DBGWVR##_Num); \
    *(PULONG)_pControl = _MoveFromCoprocessor(CP14_DBGWCR##_Num)

#define KI_SET_CP14_BREAKPOINT(_Address, _Control, _Num)          \
    _MoveToCoprocessor(_Address, CP14_DBGBVR##_Num);              \
    _MoveToCoprocessor(_Control, CP14_DBGBCR##_Num)

#define KI_SET_CP14_WATCHPOINT(_Address, _Control, _Num)          \
    _MoveToCoprocessor(_Address, CP14_DBGWVR##_Num);              \
    _MoveToCoprocessor(_Control, CP14_DBGWCR##_Num)

C_ASSERT(ARM_MAX_BREAKPOINTS == 8);
C_ASSERT(ARM_MAX_WATCHPOINTS == 1);

#if ARM_VFP_ENABLE_STATISTICS
volatile ARM_VFP_STATS KiArmVfpStats;
#endif

//
// N.B. The log size must be a power of 2 for the code to function
//      properly.
//

#define KI_TRAPFRAME_LOG_SIZE 0x20
#define KI_TRAPFRAME_LOG_CPUS 8

volatile BOOLEAN KiTrapFrameLogEnable = TRUE;

#pragma bss_seg("CACHEALIGNZ")
DECLSPEC_CACHEALIGN TRAPFRAME_LOG_ENTRY KiTrapFrameLog[KI_TRAPFRAME_LOG_CPUS][KI_TRAPFRAME_LOG_SIZE];
DECLSPEC_CACHEALIGN ULONG KiTrapFrameLogIndex[KI_TRAPFRAME_LOG_CPUS];
#pragma bss_seg()

ULONG
LogVfpState (
    _Out_ PKARM_VFP_STATE VfpState
    );

VOID
KiMarkTrapFrameLog (
    VOID
    )

/*++

Routine Description:

    This function adds the trap frame log to the list of triage dump data block
    regions so that the trap frame log is available when debugging a triage
    dump.

    If trap frame logging is not enabled, the associated regions are not added.

    N.B.  This routine is invoked at bugcheck time and may take no locks, nor
          reference any code or data that might fault.

Arguments:

    None.

Return Value:

    None.

--*/

{

    if (ReadUCharNoFence(&KiTrapFrameLogEnable) == FALSE) {
        return;
    }

    IoAddTriageDumpDataBlock(&KiTrapFrameLog[0], sizeof(KiTrapFrameLog));
    IoAddTriageDumpDataBlock(&KiTrapFrameLogIndex[0], sizeof(KiTrapFrameLogIndex));

    return;
}

VOID
KiUpdateDbgdscr (
    __in BOOLEAN Enable
    )

/*++

Routine Description:

    This function enables/disables debug monitor mode in DBGDSCR register.

Arguments:

    Enable - Whether to enable/disable debug monitor mode

Return Value:

    None

--*/

{

    ULONG Dbgdscr;

    if (KiCP14DebugEnabled == FALSE) {
        return;
    }

    Dbgdscr = _MoveFromCoprocessor(CP14_DBGDSCR);
    if (Enable != FALSE) {
        if ((Dbgdscr & DBGDSCR_MON_EN_BIT) != 0) {

            //
            // Debug registers are already enabled.
            //

            return;
        }

        Dbgdscr |= DBGDSCR_MON_EN_BIT;

    } else {
        if ((Dbgdscr & DBGDSCR_MON_EN_BIT) == 0) {

            //
            // Debug registers were already disabled
            //

           return;
        }

        Dbgdscr &= ~DBGDSCR_MON_EN_BIT;
    }

    _MoveToCoprocessor(Dbgdscr, CP14_DBGDSCR);
    _DataSynchronizationBarrier();

    return;
}

VOID
KiCopySavedDebugRegisters (
    __inout PULONG DestinationBvr,
    __in PULONG SourceBvr,
    __inout PULONG DestinationBcr,
    __in PULONG SourceBcr,
    __inout PULONG DestinationWvr,
    __in PULONG SourceWvr,
    __inout PULONG DestinationWcr,
    __in PULONG SourceWcr
    )
/*++

Routine Description:

    This routine copies CP14 [break/watch]point registers.

Arguments:

    DestinationBvr - Supplies the destination array of breakpoint address
        values.

    SourceBvr - Supplies the source array of breakpoint address values.

    DestinationBcr - Supplies the destination array of breakpoint control
        values.

    SourceBcr - Supplies the source array of breakpoint control values.

    DestinationWvr - Supplies the destination array of watchpoint address
        values.

    SourceWvr - Supplies the source array of watchpoint address values.

    DestinationWcr - Supplies the destination array of watchpoint control
        values.

    SourceWcr - Supplies the source array of watchpoint control values.

Return Value:

    None

--*/

{

    PCONTEXT Context;
    PKTRAP_FRAME Frame;

    //
    // Verify at compile time that the potential sources and
    // destinations are are the same size.  If the registers were
    // enclosed in some common structure, this routine could be
    // simplified.
    //

    C_ASSERT(sizeof(Context->Bvr) == sizeof(Frame->Bvr));
    C_ASSERT(sizeof(Context->Bcr) == sizeof(Frame->Bcr));
    C_ASSERT(sizeof(Context->Wvr) == sizeof(Frame->Wvr));
    C_ASSERT(sizeof(Context->Wcr) == sizeof(Frame->Wcr));

    RtlCopyMemory(DestinationBvr, SourceBvr, sizeof(Context->Bvr));
    RtlCopyMemory(DestinationBcr, SourceBcr, sizeof(Context->Bcr));
    RtlCopyMemory(DestinationWvr, SourceWvr, sizeof(Context->Wvr));
    RtlCopyMemory(DestinationWcr, SourceWcr, sizeof(Context->Wcr));

    return;
}

VOID
KiCopyDebugRegs (
    __inout PKTRAP_FRAME PrevTrapFrame,
    __in PKTRAP_FRAME CurrTrapFrame
    )

/*++

Routine Description:

    Copy debug regs from current trap frame to previous trap frame.

Arguments:

    PrevTrapFrame - Supplies a pointer to the previous trap frame

    CurrTrapFrame - Supplies a pointer to the current trap frame

Return Value:

    None

--*/

{

    if (KiCP14DebugEnabled == FALSE) {
        return;
    }

    PrevTrapFrame->DebugRegistersValid = CurrTrapFrame->DebugRegistersValid;
    if (CurrTrapFrame->DebugRegistersValid != FALSE) {
        KiCopySavedDebugRegisters(PrevTrapFrame->Bvr,
                                  CurrTrapFrame->Bvr,
                                  PrevTrapFrame->Bcr,
                                  CurrTrapFrame->Bcr,
                                  PrevTrapFrame->Wvr,
                                  CurrTrapFrame->Wvr,
                                  PrevTrapFrame->Wcr,
                                  CurrTrapFrame->Wcr);

    }

    return;
}

BOOLEAN
KiIsArmedForDebug (
    __in PULONG Bcr,
    __in PULONG Wcr
    )

/*++

Routine Description:

    This function checks the [break/watch]point control regs to
    determine if any control reg is set. This is useful in checking
    whether a particular set of debug regs should be written to h/w
    or not.

Arguments:

    Bcr - Supplies the array of breakpoint control values.
    Wcr - Supplies the array of watchpoint control values.

Return Value:

    TRUE if any [bkpt/watch]point is enabled. FALSE otherwise.

--*/

{

    ULONG Index;
    PKPRCB Prcb;

    Prcb = KeGetCurrentPrcb();
    for (Index = 0; Index < Prcb->MaxBreakpoints; Index++) {
        if (Bcr[Index] & 1) {
            return TRUE;
        }
    }

    for (Index = 0; Index < Prcb->MaxWatchpoints; Index++) {
        if (Wcr[Index] & 1) {
            return TRUE;
        }
    }

    return FALSE;
}

VOID
KeContextFromKframes (
    __in PKTRAP_FRAME TrapFrame,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __inout PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified trap and
    exception frames into the specified context frame according to the
    specified context flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile
        context should be copied into the context record.

    ExceptionFrame - Supplies a pointer to an exception frame from which
        context should be copied into the context record.

    ContextRecord - Supplies a pointer to the context frame that receives
        the context copied from the trap and exception frames.

Return Value:

    None.

--*/

{

    ULONG ContextFlags;
    KIRQL OldIrql;
    PKARM_VFP_STATE VfpState;

    //
    // Raise IRQL to APC_LEVEL to guarantee that a consistent set of context
    // is transferred from the trap and exception frames.
    //

    OldIrql = KeGetCurrentIrql();
    if (OldIrql < APC_LEVEL) {
        KfRaiseIrql(APC_LEVEL);
    }

    //
    // Special behavior for ARM: reflect the state of the
    // CONTEXT_UNWOUND_TO_CALL flag in
    // TrapFrame->ContextFromKFramesUnwound. This is done so that the
    // CONTEXT_UNWOUND_TO_CALL flag is preserved across an
    // RtlRaiseException call.
    //

    if (TrapFrame->ContextFromKFramesUnwound != 0) {
        ContextRecord->ContextFlags |= CONTEXT_UNWOUND_TO_CALL;
    } else {
        ContextRecord->ContextFlags &= ~CONTEXT_UNWOUND_TO_CALL;
    }

    //
    // Set control information if specified.
    //

    ContextFlags = ContextRecord->ContextFlags;
    if ((ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {

        //
        // Set registers SP, LR, PC, and CPSR
        //

        ContextRecord->Sp = TrapFrame->Sp;
        ContextRecord->Lr = TrapFrame->Lr;
        ContextRecord->Pc = TrapFrame->Pc;
        ContextRecord->Cpsr = TrapFrame->Cpsr;
    }

    //
    // Set integer register contents if specified.
    //

    if ((ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {

        //
        // Set integer registers R0-R12.
        // R13-R15 are considered control registers
        //

        ContextRecord->R0 = TrapFrame->R0;
        ContextRecord->R1 = TrapFrame->R1;
        ContextRecord->R2 = TrapFrame->R2;
        ContextRecord->R3 = TrapFrame->R3;
        ContextRecord->R11 = TrapFrame->R11;
        ContextRecord->R12 = TrapFrame->R12;

        ContextRecord->R4 = ExceptionFrame->R4;
        ContextRecord->R5 = ExceptionFrame->R5;
        ContextRecord->R6 = ExceptionFrame->R6;
        ContextRecord->R7 = ExceptionFrame->R7;
        ContextRecord->R8 = ExceptionFrame->R8;
        ContextRecord->R9 = ExceptionFrame->R9;
        ContextRecord->R10 = ExceptionFrame->R10;
    }

    //
    // Set floating point context if specified.
    //

    if ((ContextFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) {

        VfpState = KiGetVfpStatePointer(TrapFrame);
        if (VfpState != NULL) {
            ContextRecord->Fpscr = VfpState->Fpscr;
            RtlCopyMemory(ContextRecord->D, VfpState->VfpD, sizeof(ContextRecord->D));
        } else {
            ContextRecord->Fpscr = 0;
            RtlZeroMemory(ContextRecord->D, sizeof(ContextRecord->D));
        }
    }

    //
    // Set debug register contents if requested.
    //

    if ((ContextFlags & CONTEXT_DEBUG_REGISTERS) == CONTEXT_DEBUG_REGISTERS) {
        if ((KiCP14DebugEnabled != FALSE) &&
            (TrapFrame->DebugRegistersValid != FALSE)) {

            KiCopySavedDebugRegisters(ContextRecord->Bvr,
                                      TrapFrame->Bvr,
                                      ContextRecord->Bcr,
                                      TrapFrame->Bcr,
                                      ContextRecord->Wvr,
                                      TrapFrame->Wvr,
                                      ContextRecord->Wcr,
                                      TrapFrame->Wcr);

        } else {
            RtlZeroMemory(ContextRecord->Bvr, sizeof(ContextRecord->Bvr));
            RtlZeroMemory(ContextRecord->Bcr, sizeof(ContextRecord->Bcr));
            RtlZeroMemory(ContextRecord->Wvr, sizeof(ContextRecord->Wvr));
            RtlZeroMemory(ContextRecord->Wcr, sizeof(ContextRecord->Wcr));
        }
    }

    //
    // Lower IRQL to its previous value.
    //

    if (OldIrql < APC_LEVEL) {
        KeLowerIrql(OldIrql);
    }

    return;
}

VOID
KeContextToKframes (
    __inout PKTRAP_FRAME TrapFrame,
    __inout PKEXCEPTION_FRAME ExceptionFrame,
    __in PCONTEXT ContextRecord,
    __in ULONG ContextFlags,
    __in KPROCESSOR_MODE PreviousMode
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified context frame
    into the specified trap and exception frames according to the specified
    context flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that receives the volatile
        context from the context record.

    ExceptionFrame - Supplies a pointer to an exception frame that receives
        the nonvolatile context from the context record.

    ContextRecord - Supplies a pointer to a context frame that contains the
        context that is to be copied into the trap and exception frames.

    ContextFlags - Supplies the set of flags that specify which parts of the
        context frame are to be copied into the trap and exception frames.

    PreviousMode - Supplies the processor mode for which the exception and
        trap frames are being built.

Return Value:

    None.

--*/

{

    UCHAR DbgActive;
    KIRQL OldIrql;
    ULONG Index;
    PKTHREAD Thread;
    PKARM_VFP_STATE VfpState;

    OldIrql = KeGetCurrentIrql();
    if (OldIrql < APC_LEVEL) {
        KeRaiseIrql(APC_LEVEL, &OldIrql);
    }

    //
    // Special behavior for ARM: reflect the state of the CONTEXT_UNWOUND_TO_CALL
    // flag in TrapFrame->ContextFromKFramesUnwound. This is done so that the
    // CONTEXT_UNWOUND_TO_CALL flag is preserved across an RtlRaiseException call.
    //

    TrapFrame->ContextFromKFramesUnwound =
            ((ContextRecord->ContextFlags & CONTEXT_UNWOUND_TO_CALL) != 0);

    //
    // Set control information if specified.
    //

    if ((ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {

        //
        // Set registers SP, LR, PC, and CPSR
        //

        TrapFrame->Sp = ContextRecord->Sp;
        TrapFrame->Lr = ContextRecord->Lr;
        TrapFrame->Pc = ContextRecord->Pc;
        TrapFrame->Cpsr = SANITIZE_CPSR(ContextRecord->Cpsr, PreviousMode);
    }

    //
    // Set integer registers contents if specified.
    //

    if ((ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {

        //
        // Set integer registers R0-R12.
        // R13-R15 are considered control registers
        //

        TrapFrame->R0 = ContextRecord->R0;
        TrapFrame->R1 = ContextRecord->R1;
        TrapFrame->R2 = ContextRecord->R2;
        TrapFrame->R3 = ContextRecord->R3;
        TrapFrame->R11 = ContextRecord->R11;
        TrapFrame->R12 = ContextRecord->R12;

        ExceptionFrame->R4 = ContextRecord->R4;
        ExceptionFrame->R5 = ContextRecord->R5;
        ExceptionFrame->R6 = ContextRecord->R6;
        ExceptionFrame->R7 = ContextRecord->R7;
        ExceptionFrame->R8 = ContextRecord->R8;
        ExceptionFrame->R9 = ContextRecord->R9;
        ExceptionFrame->R10 = ContextRecord->R10;
    }

    //
    // Set extended CPU state if requested.
    //

    //
    // Set floating register contents if requested.
    //

    if ((ContextFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) {

        VfpState = KiGetVfpStatePointer(TrapFrame);
        if (VfpState != NULL) {
            VfpState->Fpscr = ContextRecord->Fpscr;
            NT_ASSERT(VfpState->Fpscr != -1);
            RtlCopyMemory(VfpState->VfpD, ContextRecord->D, sizeof(VfpState->VfpD));
        }
    }

    //
    // Set debug register state if specified.
    //

    if ((ContextFlags & CONTEXT_DEBUG_REGISTERS) == CONTEXT_DEBUG_REGISTERS) {
        if (KiCP14DebugEnabled != FALSE) {
            for (Index = 0; Index < ARM_MAX_BREAKPOINTS; Index++) {
                TrapFrame->Bvr[Index] =
                    SANITIZE_DEBUG_ADDR(ContextRecord->Bvr[Index],
                                        PreviousMode);

                TrapFrame->Bcr[Index] =
                    SANITIZE_DEBUG_CTRL(ContextRecord->Bcr[Index],
                                        PreviousMode);
            }

            for (Index = 0; Index < ARM_MAX_WATCHPOINTS; Index++) {
                TrapFrame->Wvr[Index] =
                    SANITIZE_DEBUG_ADDR(ContextRecord->Wvr[Index],
                                        PreviousMode);

                TrapFrame->Wcr[Index] =
                    SANITIZE_DEBUG_CTRL(ContextRecord->Wcr[Index],
                                        PreviousMode);
            }

            TrapFrame->DebugRegistersValid =
                KiIsArmedForDebug(TrapFrame->Bcr, TrapFrame->Wcr);

            if (PreviousMode != KernelMode) {
                Thread = KeGetCurrentThread();
                DbgActive = 0;
                if (TrapFrame->DebugRegistersValid != 0) {
                    DbgActive |= DEBUG_ACTIVE_DBG;
                }

                KiSetDebugActive(Thread, DbgActive, DEBUG_ACTIVE_DBG);
            }
        }
    }

    if (OldIrql < APC_LEVEL) {
        KeLowerIrql(OldIrql);
    }

    return;
}

VOID
KiDispatchException (
    __in PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __in PKTRAP_FRAME TrapFrame,
    __in KPROCESSOR_MODE PreviousMode,
    __in BOOLEAN FirstChance
    )

/*++

Routine Description:

    This function is called to dispatch an exception to the proper mode and
    to cause the exception dispatcher to be called. If the previous mode is
    kernel, then the exception dispatcher is called directly to process the
    exception. Otherwise the exception record, exception frame, and trap
    frame contents are copied to the user mode stack. The contents of the
    exception frame and trap are then modified such that when control is
    returned, execution will commense in user mode in a routine which will
    call the exception dispatcher.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame. For NT386,
        this should be NULL.

    TrapFrame - Supplies a pointer to a trap frame.

    PreviousMode - Supplies the previous processor mode.

    FirstChance - Supplies a boolean value that specifies whether this is
        the first (TRUE) or second (FALSE) chance for the exception.

Return Value:

    None.

--*/

{

    CONTEXT ContextRecord;
    BOOLEAN DebugService;
    EXCEPTION_RECORD ExceptionRecord1;
    PMACHINE_FRAME MachineFrame;
    ULONG NumberParameters;
    NTSTATUS OldExceptionCode;
    PEPROCESS Process;
    PVOID StartAddress;
    PCONTEXT UserContextRecord;
    PEXCEPTION_RECORD UserExceptionRecord;

    Process = PsGetCurrentProcess();

    //
    // Increment the number of exceptions dispatched.
    //

    KePrcbIncrementUlong(KeExceptionDispatchCount);


    if ((FirstChance != FALSE) &&
        (PsIsPicoProcess(Process) != FALSE)) {

        OldExceptionCode = ExceptionRecord->ExceptionCode;
        switch (OldExceptionCode) {
        case KI_EXCEPTION_ACCESS_VIOLATION:
            ExceptionRecord->ExceptionCode = STATUS_ACCESS_VIOLATION;
            break;

        case KI_EXCEPTION_INVALID_OP:
            ExceptionRecord->ExceptionCode = STATUS_ILLEGAL_INSTRUCTION;
            break;
        }

        //
        // Let pico attempt to handle all user-mode exceptions and kernel-mode
        // exceptions on user-mode addresses.
        //


        NT_ASSERT((ULONG_PTR)MM_HIGHEST_USER_ADDRESS <
                  (ULONG_PTR)MM_USER_PROBE_ADDRESS);

        if ((KeGetEffectiveIrql() < DISPATCH_LEVEL) &&
            ((PreviousMode != KernelMode) ||
             ((ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION) &&
              (ExceptionRecord->ExceptionInformation[1] <= MM_USER_PROBE_ADDRESS)))) {

            if (PsPicoDispatchException(ExceptionRecord,
                                        ExceptionFrame,
                                        TrapFrame,
                                        0,
                                        PreviousMode) != FALSE) {
            
                goto Handled2;
            }
        }

        ExceptionRecord->ExceptionCode = OldExceptionCode;
    }

    //
    // Move machine state from trap and exception frames to a context frame.
    //

    ContextRecord.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

    KeContextFromKframes(TrapFrame,
                         ExceptionFrame,
                         &ContextRecord);

    //
    // If the exception is a break point, then convert the break point to a
    // fault. (No need to do this on ARM, as breakpoints are passed to us
    // as KI_EXCEPTION_INVALID_OP and converted in KiPreprocessFault below.)
    //

    //
    // If the exception is an internal general protect fault, invalid opcode,
    // or integer divide by zero, then attempt to resolve the problem without
    // actually raising an exception.
    //

    if (KiPreprocessFault(ExceptionRecord,
                          ExceptionFrame,
                          TrapFrame,
                          &ContextRecord,
                          PreviousMode) != FALSE) {

        goto Handled1;
    }

    //
    // Select the method of handling the exception based on the previous mode.
    //

    if (PreviousMode == KernelMode) {

        //
        // Previous mode was kernel.
        //
        // If the kernel debugger is active, then give the kernel debugger
        // the first chance to handle the exception. If the kernel debugger
        // handles the exception, then continue execution. Otherwise, attempt
        // to dispatch the exception to a frame based handler. If a frame
        // based handler handles the exception, then continue execution.
        //
        // If a frame based handler does not handle the exception, give the
        // kernel debugger a second chance, if it's present.
        //
        // If the exception is still unhandled call bugcheck.
        //

        if (FirstChance != FALSE) {
            if ((KiDebugRoutine)(TrapFrame,
                                 ExceptionFrame,
                                 ExceptionRecord,
                                 &ContextRecord,
                                 PreviousMode,
                                 FALSE) != FALSE) {

                goto Handled1;
            }

            //
            // Kernel debugger didn't handle exception.
            //

            if (RtlDispatchException(ExceptionRecord, &ContextRecord) != FALSE) {
                goto Handled1;
            }
        }

        //
        // This is the second chance to handle the exception.
        //

        if ((KiDebugRoutine)(TrapFrame,
                             ExceptionFrame,
                             ExceptionRecord,
                             &ContextRecord,
                             PreviousMode,
                             TRUE) != FALSE) {

            goto Handled1;
        }

        KeBugCheckEx(KMODE_EXCEPTION_NOT_HANDLED,
                     ExceptionRecord->ExceptionCode,
                     (ULONG)ExceptionRecord->ExceptionAddress,
                     ExceptionRecord->ExceptionInformation[0],
                     ExceptionRecord->ExceptionInformation[1]);

    } else {

        //
        // Previous mode was user.
        //
        // If this is the first chance and the current process has a debugger
        // port, then send a message to the debugger port and wait for a reply.
        // If the debugger handles the exception, then continue execution. Else
        // transfer the exception information to the user stack, transition to
        // user mode, and attempt to dispatch the exception to a frame based
        // handler. If a frame based handler handles the exception, then continue
        // execution with the continue system service. Else execute the
        // NtRaiseException system service with FirstChance == FALSE, which
        // will call this routine a second time to process the exception.
        //
        // If this is the second chance and the current process has a debugger
        // port, then send a message to the debugger port and wait for a reply.
        // If the debugger handles the exception, then continue execution. Else
        // if the current process has a subsystem port, then send a message to
        // the subsystem port and wait for a reply. If the subsystem handles the
        // exception, then continue execution. Else terminate the thread.

        //
        // Zero possibly uninitialized part of exception record.
        //

        NumberParameters = ExceptionRecord->NumberParameters;
        StartAddress = &ExceptionRecord->ExceptionInformation[NumberParameters];
        RtlZeroMemory(StartAddress, (PUCHAR)(ExceptionRecord + 1) - (PUCHAR)StartAddress);
        if (FirstChance == TRUE) {

            //
            // This is the first chance to handle the exception.
            //
            // Check whether this user exception should be suppressed.
            //

            if (DbgkFilterUserException(ExceptionRecord) != FALSE) {
                goto Handled2;
            }

            //
            // If the current processor is not being debugged and user mode
            // exceptions are not being ignored, or this is a debug service,
            // then attempt to handle the exception via the kernel debugger.
            //


            DebugService = KdIsThisAKdTrap(ExceptionRecord,
                                           &ContextRecord,
                                           UserMode);

            if (((Process->DebugPort == NULL) &&
                 (KdIgnoreUmExceptions == FALSE)) ||
                 (DebugService == TRUE)) {

                //
                // Attempt to handle the exception with the kernel debugger.
                //

                if ((KiDebugRoutine)(TrapFrame,
                                     ExceptionFrame,
                                     ExceptionRecord,
                                     &ContextRecord,
                                     PreviousMode,
                                     FALSE) != FALSE) {

                    goto Handled1;
                }
            }

            if (DbgkForwardException(ExceptionRecord, TRUE, FALSE) != FALSE) {
                goto Handled2;
            }

            //
            // Transfer exception information to the user stack, transition
            // to user mode, and attempt to dispatch the exception to a frame
            // based handler.
            //

            ExceptionRecord1.ExceptionCode = STATUS_ACCESS_VIOLATION;

        repeat:
            try {

                //
                // Compute address of aligned machine frame, compute address
                // of exception record, compute address of context record,
                // and probe user stack for writeability.
                //
                // -------------------- <-- ContextRecord.Sp
                // | Red Zone         |
                // --------------------
                // | Machine frame    |
                // -------------------- <-- MachineFrame
                // | Exception record |
                // -------------------- <-- UserExceptionRecord
                // | CONTEXT          |
                // -------------------- <-- UserContextRecord
                //

                UserContextRecord = (PCONTEXT)
                    ALIGN_DOWN_BY(ContextRecord.Sp - ARM_RED_ZONE_BYTES - sizeof(MACHINE_FRAME) -
                                            EXCEPTION_RECORD_LENGTH - CONTEXT_LENGTH,
                                  CONTEXT_ALIGN);

                UserExceptionRecord = (PEXCEPTION_RECORD)(
                    (ULONG_PTR)UserContextRecord + CONTEXT_LENGTH);

                MachineFrame = (PMACHINE_FRAME)(
                    (ULONG_PTR)UserExceptionRecord + EXCEPTION_RECORD_LENGTH);

                ProbeForWriteSmallStructure(UserContextRecord,
                                            (ULONG_PTR)ContextRecord.Sp - (ULONG_PTR)UserContextRecord,
                                            CONTEXT_ALIGN);

                //
                // Fill in machine frame information.
                //

                MachineFrame->Sp = ContextRecord.Sp;
                MachineFrame->Pc = ContextRecord.Pc;

                //
                // Copy exception record to the user stack.
                //

                *UserExceptionRecord = *ExceptionRecord;

                //
                // Copy context record to the user stack.
                //

                *UserContextRecord = ContextRecord;

                //
                // Raise IRQL to ensure that a set context does not occur while
                // changing the trap frame and set the address of the new stack
                // pointer in the current trap frame.
                //

                KeFastDisableInterrupts();
                TrapFrame->Sp = (ULONG_PTR)UserContextRecord;

                //
                // Set the address
                // of the exception routine that will call the exception
                // dispatcher and then return to the trap handler. The trap
                // handler will restore the exception and trap frame context
                // and continue execution in the routine that will call the
                // exception dispatcher.
                //

                TrapFrame->Pc = (ULONG)KeUserExceptionDispatcher;
                TrapFrame->Cpsr |= CPSRC_THUMB;
                TrapFrame->Cpsr &= ~CPSR_IT_MASK;
                KiSetupForInstrumentationReturn(TrapFrame);
                KeEnableInterrupts(TRUE);
                return;

            } except(KiCopyInformation(&ExceptionRecord1,
                                       (GetExceptionInformation())->ExceptionRecord)) {

                //
                // If the exception is a stack overflow, then attempt to
                // raise the stack overflow exception. Otherwise, the user's
                // stack is not accessible, or is misaligned, and second
                // chance processing is performed.
                //

                if (ExceptionRecord1.ExceptionCode == STATUS_STACK_OVERFLOW) {
                    ExceptionRecord1.ExceptionAddress = ExceptionRecord->ExceptionAddress;
                    *ExceptionRecord = ExceptionRecord1;

                    goto repeat;
                }
            }
        }

        //
        // This is the second chance to handle the exception.
        //

        if ((DbgkForwardException(ExceptionRecord, TRUE, TRUE) == FALSE) &&
            (DbgkForwardException(ExceptionRecord, FALSE, TRUE) == FALSE)) {

            ZwTerminateProcess(ZwCurrentProcess(), ExceptionRecord->ExceptionCode);
        }

        goto Handled2;

    }

    //
    // Move machine state from context frame to trap and exception frames and
    // then return to continue execution with the restored state.
    //

Handled1:
    KeContextToKframes(TrapFrame,
                       ExceptionFrame,
                       &ContextRecord,
                       ContextRecord.ContextFlags,
                       PreviousMode);

    //
    // Exception was handled by the debugger or the associated subsystem
    // and state was modified, if necessary, using the get state and set
    // state capabilities. Therefore the context frame does not need to
    // be transfered to the trap and exception frames.
    //

Handled2:

    return;
}

__drv_maxIRQL(APC_LEVEL)
__drv_minIRQL(PASSIVE_LEVEL)
__drv_sameIRQL
NTSTATUS
KeRaiseUserException (
    __in NTSTATUS ExceptionCode
    )

/*++

Routine Description:

    This function causes an exception to be raised in the calling thread's
    user context.

Arguments:

    ExceptionCode - Supplies the status value to be raised.

Return Value:

    The status value that should be returned by the caller.

--*/

{

    KIRQL OldIrql;
    PTEB Teb;
    PKTHREAD Thread;
    PKTRAP_FRAME TrapFrame;

    //
    // Obtain the current thread and raise IRQL to APC_LEVEL as the trap frame
    // will be examined (and might be a user mode trap frame).
    //

    Thread = KeGetCurrentThread();
    OldIrql = KeGetCurrentIrql();
    if (OldIrql < APC_LEVEL) {
        KeRaiseIrql(APC_LEVEL, &OldIrql);
    }

    //
    // Save the exception code in the TEB and set the return address in the
    // trap frame to return to the raise user exception code in user mode.
    // This replaces the normal return which would go to the system service
    // dispatch stub. The system service dispatch stub is called thus the
    // return to the system service call site is on the top of the user stack.
    //

    TrapFrame = Thread->TrapFrame;
    if ((TrapFrame != NULL) &&
        (KiGetTrapFrameMode(TrapFrame) == UserMode)) {
        Teb = (PTEB)Thread->Teb;
        try {
            Teb->ExceptionCode = ExceptionCode;

        } except(EXCEPTION_EXECUTE_HANDLER) {
            goto Done;
        }

        //
        // N.B. Interrupts are disabled to ensure that a set context does not
        //      occur while setting either the flags or return address
        //      information.
        //

        _disable();
        TrapFrame->Pc = (ULONG_PTR)KeRaiseUserExceptionDispatcher;
        TrapFrame->Cpsr |= CPSRC_THUMB;
        TrapFrame->Cpsr &= ~CPSR_IT_MASK;

        //
        // Apply instrumentation unless the trap frame is a service frame, and
        // would thus already receive instrumentation during the trap frame restore
        // sequence.
        //

        if (TrapFrame->ExceptionActive != KEXCEPTION_ACTIVE_SERVICE_FRAME) {
            KiSetupForInstrumentationReturn(TrapFrame);
        }
        _enable();
    }

Done:

    //
    // Restore original IRQL
    //

    if (OldIrql < APC_LEVEL) {
        KeLowerIrql(OldIrql);
    }

    return ExceptionCode;
}

VOID
KiSetHwDebugRegs (
    __in ULONG MaxBreakpoints,
    __inout PULONG Bvr,
    __inout PULONG Bcr,
    __in ULONG MaxWatchpoints,
    __inout PULONG Wvr,
    __inout PULONG Wcr
    )

/*++

Routine Description:

    This function writes the debug registers to (current) h/w debug regs.

Arguments:

    Pointers to debug reg arrays. The arrays are assumed to be long enough.

Return Value:

    None

--*/

{

    NT_ASSERT(MaxBreakpoints <= 16);

    NT_ASSERT(MaxWatchpoints <= 16);

    switch (MaxBreakpoints) {
    case 16:
        KI_SET_CP14_BREAKPOINT(Bvr[15], Bcr[15], 15);

    case 15:
        KI_SET_CP14_BREAKPOINT(Bvr[14], Bcr[14], 14);

    case 14:
        KI_SET_CP14_BREAKPOINT(Bvr[13], Bcr[13], 13);

    case 13:
        KI_SET_CP14_BREAKPOINT(Bvr[12], Bcr[12], 12);

    case 12:
        KI_SET_CP14_BREAKPOINT(Bvr[11], Bcr[11], 11);

    case 11:
        KI_SET_CP14_BREAKPOINT(Bvr[10], Bcr[10], 10);

    case 10:
        KI_SET_CP14_BREAKPOINT(Bvr[9], Bcr[9], 9);

    case 9:
        KI_SET_CP14_BREAKPOINT(Bvr[8], Bcr[8], 8);

    case 8:
        KI_SET_CP14_BREAKPOINT(Bvr[7], Bcr[7], 7);

    case 7:
        KI_SET_CP14_BREAKPOINT(Bvr[6], Bcr[6], 6);

    case 6:
        KI_SET_CP14_BREAKPOINT(Bvr[5], Bcr[5], 5);

    case 5:
        KI_SET_CP14_BREAKPOINT(Bvr[4], Bcr[4], 4);

    case 4:
        KI_SET_CP14_BREAKPOINT(Bvr[3], Bcr[3], 3);

    case 3:
        KI_SET_CP14_BREAKPOINT(Bvr[2], Bcr[2], 2);

    case 2:
        KI_SET_CP14_BREAKPOINT(Bvr[1], Bcr[1], 1);

    case 1:
        KI_SET_CP14_BREAKPOINT(Bvr[0], Bcr[0], 0);

    case 0:
        break;
    }

    switch (MaxWatchpoints) {
    case 16:
        KI_SET_CP14_WATCHPOINT(Wvr[15], Wcr[15], 15);

    case 15:
        KI_SET_CP14_WATCHPOINT(Wvr[14], Wcr[14], 14);

    case 14:
        KI_SET_CP14_WATCHPOINT(Wvr[13], Wcr[13], 13);

    case 13:
        KI_SET_CP14_WATCHPOINT(Wvr[12], Wcr[12], 12);

    case 12:
        KI_SET_CP14_WATCHPOINT(Wvr[11], Wcr[11], 11);

    case 11:
        KI_SET_CP14_WATCHPOINT(Wvr[10], Wcr[10], 10);

    case 10:
        KI_SET_CP14_WATCHPOINT(Wvr[9], Wcr[9], 9);

    case 9:
        KI_SET_CP14_WATCHPOINT(Wvr[8], Wcr[8], 8);

    case 8:
        KI_SET_CP14_WATCHPOINT(Wvr[7], Wcr[7], 7);

    case 7:
        KI_SET_CP14_WATCHPOINT(Wvr[6], Wcr[6], 6);

    case 6:
        KI_SET_CP14_WATCHPOINT(Wvr[5], Wcr[5], 5);

    case 5:
        KI_SET_CP14_WATCHPOINT(Wvr[4], Wcr[4], 4);

    case 4:
        KI_SET_CP14_WATCHPOINT(Wvr[3], Wcr[3], 3);

    case 3:
        KI_SET_CP14_WATCHPOINT(Wvr[2], Wcr[2], 2);

    case 2:
        KI_SET_CP14_WATCHPOINT(Wvr[1], Wcr[1], 1);

    case 1:
        KI_SET_CP14_WATCHPOINT(Wvr[0], Wcr[0], 0);

    case 0:
        break;
    }

    _DataSynchronizationBarrier();
    return;
}

VOID
KiWriteHwDebugRegs (
    IN PULONG Bvr,
    IN PULONG Bcr,
    IN PULONG Wvr,
    IN PULONG Wcr
    )

/*++

Routine Description:

    This function writes the debug registers to (current) h/w debug regs.

Arguments:

    Pointers to debug reg arrays. The arrays are assumed to be long enough.

Return Value:

    None

--*/
{
    PKPRCB Prcb;

    NT_ASSERT((KeGetCurrentIrql() >= DISPATCH_LEVEL) ||
              (KeAreInterruptsEnabled() == FALSE));

    Prcb = KeGetCurrentPrcb();

    NT_ASSERT(Prcb->MaxBreakpoints <= ARM_MAX_BREAKPOINTS);
    NT_ASSERT(Prcb->MaxWatchpoints <= ARM_MAX_WATCHPOINTS);

    KiSetHwDebugRegs(Prcb->MaxBreakpoints,
                     Bvr,
                     Bcr,
                     Prcb->MaxWatchpoints,
                     Wvr,
                     Wcr);

    return;
}

VOID
KiReadHwDebugRegs (
    __inout PULONG Bvr,
    __inout PULONG Bcr,
    __inout PULONG Wvr,
    __inout PULONG Wcr
    )

/*++

Routine Description:

    This function loads the debug registers from (current) h/w debug regs.

Arguments:

    Pointers to debug reg arrays. The arrays are assumed to be large enough.

Return Value:

    None

--*/

{

    PKPRCB Prcb;

    NT_ASSERT((KeGetCurrentIrql() >= DISPATCH_LEVEL) ||
              (KeAreInterruptsEnabled() == FALSE) ||
              (KeGetCurrentThread()->SystemThread != 0));

    Prcb = KeGetCurrentPrcb();

    NT_ASSERT(Prcb->MaxBreakpoints <= ARM_MAX_BREAKPOINTS);
    NT_ASSERT(Prcb->MaxWatchpoints <= ARM_MAX_WATCHPOINTS);

    //
    // Because these are coprocessor reads which only accept hard-coded
    // indices, the code below must be unrolled.
    //

    switch (Prcb->MaxBreakpoints) {
    default:
    case 8:
        KI_GET_CP14_BREAKPOINT(&Bvr[7], &Bcr[7], 7);

    case 7:
        KI_GET_CP14_BREAKPOINT(&Bvr[6], &Bcr[6], 6);

    case 6:
        KI_GET_CP14_BREAKPOINT(&Bvr[5], &Bcr[5], 5);

    case 5:
        KI_GET_CP14_BREAKPOINT(&Bvr[4], &Bcr[4], 4);

    case 4:
        KI_GET_CP14_BREAKPOINT(&Bvr[3], &Bcr[3], 3);

    case 3:
        KI_GET_CP14_BREAKPOINT(&Bvr[2], &Bcr[2], 2);

    case 2:
        KI_GET_CP14_BREAKPOINT(&Bvr[1], &Bcr[1], 1);

    case 1:
        KI_GET_CP14_BREAKPOINT(&Bvr[0], &Bcr[0], 0);

    case 0:
        break;
    }

    switch (Prcb->MaxWatchpoints) {
    default:
    case 1:
        KI_GET_CP14_WATCHPOINT(&Wvr[0], &Wcr[0], 0);

    case 0:
        break;
    }
    return;
}

VOID
KiReadDebugRegs (
    __inout PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This function loads the debug registers in the trap frame and
    sets the debug registers from the kernel mode copy. This is done to
    switch h/w debug registers from monitoring only the current thread (user-
    mode) to global system wide debug setting in kernel-mode.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame

Return Value:

    None

--*/

{
    BOOLEAN DebugActive;
    PKSPECIAL_REGISTERS SpecialRegisters;
    PKPRCB Prcb;

    NT_ASSERT((KeGetCurrentIrql() >= DISPATCH_LEVEL) ||
              (KeAreInterruptsEnabled() == FALSE));

    NT_ASSERT((TrapFrame->Cpsr & CPSRM_MASK) == CPSRM_USER);

    NT_ASSERT(KeGetCurrentThread()->Header.DebugActive != FALSE);

    if (KiCP14DebugEnabled == FALSE) {
        return;
    }

    Prcb = KeGetCurrentPrcb();
    SpecialRegisters = &Prcb->ProcessorState.SpecialRegisters;
    TrapFrame->DebugRegistersValid = TRUE;
    KiReadHwDebugRegs(TrapFrame->Bvr,
                      TrapFrame->Bcr,
                      TrapFrame->Wvr,
                      TrapFrame->Wcr);

    KiWriteHwDebugRegs(SpecialRegisters->KernelBvr,
                       SpecialRegisters->KernelBcr,
                       SpecialRegisters->KernelWvr,
                       SpecialRegisters->KernelWcr);

    DebugActive = KiIsArmedForDebug(SpecialRegisters->KernelBcr,
                                    SpecialRegisters->KernelWcr);

    KiUpdateDbgdscr(DebugActive);

    return;
}

VOID
KiWriteDebugRegs (
    __in PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This function loads the debug registers from the trap frame. It is called
    to restore debug regs when returning to user mode and the current thread
    has h/w debugging activated.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame

Return Value:

    None

--*/

{

    UCHAR DebugActive;
    BOOLEAN ThDebugActive;
    PKTHREAD Thread;

    NT_ASSERT((KeGetCurrentIrql() >= DISPATCH_LEVEL) ||
              (KeAreInterruptsEnabled() == FALSE));

    NT_ASSERT((TrapFrame->Cpsr & CPSRM_MASK) == CPSRM_USER);

    if (KiCP14DebugEnabled == FALSE) {
        return;
    }

    Thread = KeGetCurrentThread();
    ThDebugActive = Thread->Header.DebugActive;
    if ((ThDebugActive & ~DEBUG_ACTIVE_MINIMAL_THREAD) != 0) {
        KiWriteHwDebugRegs(TrapFrame->Bvr,
                           TrapFrame->Bcr,
                           TrapFrame->Wvr,
                           TrapFrame->Wcr);

        DebugActive = KiIsArmedForDebug(TrapFrame->Bcr, TrapFrame->Wcr);
        KiUpdateDbgdscr(DebugActive);
    }

    return;
}

ULONG
KiGetDebugExceptionType (
    VOID
    )

/*++

Routine Description:

    This function is called after confirming a h/w debug exception to determine
    it's type (breakpoint or watchpoint). It's called in conjunction with
    KiDebugException.

Arguments:

    None

Return Value:

    Debug exception code

--*/

{

    ULONG Dbgscr;
    ULONG Moe;
    ULONG Type;

    Dbgscr = _MoveFromCoprocessor(CP14_DBGDSCR);
    Moe = (Dbgscr & CP14_DBGDSCR_MOE_MASK) >> CP14_DBGDSCR_MOE_SHIFT;
    switch (Moe) {
    case CP14_DBGDSCR_MOE_BP:
        Type = BREAKPOINT_HW_BREAK;
        break;

    case CP14_DBGDSCR_MOE_WPASYNC:
        Type = BREAKPOINT_HW_ASYNCH_WATCH;
        break;

    case CP14_DBGDSCR_MOE_WPSYNC:
        Type = BREAKPOINT_HW_SYNCH_WATCH;
        break;

    case CP14_DBGDSCR_MOE_BKPT:
    default:
        Type = BREAKPOINT_BREAK;
        break;
    }

    return Type;
}

BOOLEAN
KiDebugException (
    __inout PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This function handles a h/w debug abort and determines the fault address.
    It also disables h/w debugging so that no more h/w [break/watch]points are
    hit while the kernel handles this h/w debug event. Since this function is
    called early on in abort trap handling, the h/w debug exception is not yet
    raised.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame

Return Value:

    FALSE if this is not a monitor mode event. TRUE otherwise.

--*/

{

    ULONG Dbgscr;
    ULONG Moe;
    BOOLEAN Success;

    Success = TRUE;
    Dbgscr = _MoveFromCoprocessor(CP14_DBGDSCR);

    //
    // Halting mode is not supported. Let MMU handler deal this this case.
    //

    if (Dbgscr & DBGDSCR_HLT_EN_BIT) {
        Success = FALSE;
        goto DebugExceptionEnd;
    }

    Moe = (Dbgscr & CP14_DBGDSCR_MOE_MASK) >> CP14_DBGDSCR_MOE_SHIFT;
    switch (Moe)
    {
    case CP14_DBGDSCR_MOE_BP:

        //
        // handle the h/w breakpoint
        //

        break;

    case CP14_DBGDSCR_MOE_WPASYNC:
    case CP14_DBGDSCR_MOE_WPSYNC:

        //
        // handle the [a]sync watchpoint case
        //

        TrapFrame->FaultAddress = _MoveFromCoprocessor(CP14_DBGWFAR) - 4;
        break;

    case CP14_DBGDSCR_MOE_BKPT:

        //
        // advance PC past the BKPT instruction

        TrapFrame->Pc += 2;
        break;

    default:

        //
        // Unknown h/w debug event.
        //

        KePrint("[ke] Unknown h/w debug event! DBGDSCR = 0x%08lx\n", Dbgscr);
        Success = FALSE;
        break;
    }

DebugExceptionEnd:
    if (Success != FALSE) {

        //
        // Disable debugging while we handle this debug exception.
        //

        KiUpdateDbgdscr(FALSE);
    }

    return Success;
}

