/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    decode.c

Abstract:

    This module implement the code necessary to preprocess exceptions so
    that they can be dispatched appropriately.

Environment:

    Kernel mode only.

--*/

#include "ki.h"


BOOLEAN
KiPreprocessInternalInvalidOpcode (
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __in PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord,
    __in KPROCESSOR_MODE PreviousMode
    );

BOOLEAN
KiOpPreprocessAccessViolation (
    __in PEXCEPTION_RECORD ExceptionRecord,
    __inout PCONTEXT ContextRecord
    );

BOOLEAN
KiPreprocessHardwareError (
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord
    );

BOOLEAN
KiDispatchFastFail (
    __in PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __in PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord,
    __in KPROCESSOR_MODE PreviousMode
    );


DECLSPEC_NOINLINE
BOOLEAN
KiPreprocessFault (
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __in PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord,
    __in KPROCESSOR_MODE PreviousMode
    )

/*++

Routine Description:

    A wide variety of exception conditions can result in a general
    protection fault.

    This routine attempts to further determine the cause of the fault
    and take appropriate action, which can include

        - Updating the context record with a more appropriate status

        - Emulating the instruction or otherwise modifying the caller's
          context and/or istream and returning

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame.

    ContextRecord - Supplies a pointer to a context record.

    TrapFrame - Supplies a pointer to the trap frame.

    PreviousMode - Supplies the execution mode at the time of the exception.

Return Value:

    TRUE - the caller's context should be updated and control returned
           to the caller.

    FALSE - the exception should be raised with (perhaps) an updated
            context record.

--*/

{
    BOOLEAN Retry;

    //
    // Determine whether this is an internal exception and if so replace
    // it with the canonical one and continue processing.
    //

    switch (ExceptionRecord->ExceptionCode) {

        case KI_EXCEPTION_INVALID_OP:
            Retry = KiPreprocessInternalInvalidOpcode(ExceptionRecord,
                                                      ExceptionFrame,
                                                      TrapFrame,
                                                      ContextRecord,
                                                      PreviousMode);

            break;

        case KI_EXCEPTION_ACCESS_VIOLATION:
            Retry = KiOpPreprocessAccessViolation(ExceptionRecord,
                                                  ContextRecord);

            if (Retry != FALSE) {
                return TRUE;
            }

            ExceptionRecord->ExceptionCode = STATUS_ACCESS_VIOLATION;
            break;

        case KI_EXCEPTION_HARDWARE_ERROR:
            Retry = KiPreprocessHardwareError(ExceptionRecord,
                                              TrapFrame,
                                              ContextRecord);
            break;

        default:
            Retry = FALSE;
            break;
    }

    return Retry;
}

BOOLEAN
KiPreprocessInternalInvalidOpcode(
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __inout PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord,
    __in KPROCESSOR_MODE PreviousMode
    )

/*++

Routine Description:

    This routine attempts to further determine the cause of the fault
    and take appropriate action, which can include

        - Updating the exception record with a more appropriate status

        - Changing the meaning of the exception

    Currently it converts exceptions caused by specific undefined instructions
    into breakpoints, asserts, fastfails, and debug services.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame.

    ContextRecord - Supplies a pointer to a context record.

    TrapFrame - Supplies a pointer to the trap frame.

    PreviousMode - Supplies the execution mode at the time of the exception.

Return Value:

    TRUE - the caller's context should be updated and control returned
           to the caller.

    FALSE - the exception should be raised with (perhaps) an updated
            context record.

--*/

{

    ULONG Instruction;
    KIRQL OldIrql;
    BOOLEAN Result;

    UNREFERENCED_PARAMETER(TrapFrame);

    //
    // Replace the internal exception code with the canonical one.
    //

    NT_ASSERT((ExceptionRecord->ExceptionCode == KI_EXCEPTION_INVALID_OP) ||
              (ExceptionRecord->ExceptionCode == STATUS_ILLEGAL_INSTRUCTION));

    //
    // Block potential concurrent attempts to modify the trap frame via
    // the context setting interface.
    //

    Result = FALSE;
    OldIrql = KeGetCurrentIrql();
    if (OldIrql < APC_LEVEL) {
        KfRaiseIrql(APC_LEVEL);
    }


    //
    // Only support thumb mode; if the PC indicates that the trap occured in
    // ARM mode, leave the instruction as 0 which will fall through to the
    // general "Illegal Instruction" case.
    //

    Instruction = 0;

    if ((ContextRecord->Pc & 1) != 0) {

        //
        // The undefined instruction handler saves the undefined instruction
        // before re-enabling interrupts. This allows for correct operation
        // when multiple threads are racing to hit a breakpoint instruction
        // inserted by the kernel debugger. Without saving the instruciton
        // bytes, one thread may get interrupted between hitting the undefined
        // breakpoint instruction and running this code to determine the fault
        // reason. Another thread can put the original instruction back in that
        // window, which would cause this code to fail to recognize the
        // breakpoint.
        //
        // By capturing the instruction with interrupts still disabled, the
        // debugger cannot get control to replace the breakpoint until after the
        // instruction bytes have been captured. This relies on the fact that
        // the debugger uses maskable interrupts to gain control of alternate
        // processors.
        //

        if (ExceptionRecord->ExceptionCode == KI_EXCEPTION_INVALID_OP) {

            NT_ASSERT(ExceptionRecord->NumberParameters == 1);

            Instruction = ExceptionRecord->ExceptionInformation[0];

        } else if ((ContextRecord->Cpsr & CPSRM_MASK) == CPSRM_SVC) {
            Instruction = *(PUSHORT) (ContextRecord->Pc - 1);
            if (Instruction >= 0xe800) {
                Instruction <<= 16;
                Instruction |= *(PUSHORT) (ContextRecord->Pc + 1);
            }

        } else {

            //
            // Fetching from user mode requires probing.
            //

            try {
                Instruction = ProbeAndReadUshort((PUSHORT) (ContextRecord->Pc - 1));
                if (Instruction >= 0xe800) {
                    Instruction <<= 16;
                    Instruction |= ProbeAndReadUshort((PUSHORT) (ContextRecord->Pc + 1));
                }

            } except (EXCEPTION_EXECUTE_HANDLER) {
                NOTHING;
            }
        }
    }

    //
    // Checks if the undefined instruction is a known sequence with
    // specific meaning.
    //
    // For the debug breakpoints, services, and asserts, it is important to
    // adjust the ExceptionAddress and TrapFrame->Pc to match the AMD64
    // behaviors, as follows:
    //
    //      KiBreakpointTrap => STATUS_BREAKPOINT
    //          TrapFrame->Rip = address of the opcode following the INT 3
    //          ExceptionAddress = address of the INT 3 opcode
    //
    //      KiDebugServiceTrap => STATUS_BREAKPOINT
    //          TrapFrame->Rip = address of the opcode after the INT 3 that is assumed to follow
    //          ExceptionAddress = same as TrapFrame->Rip
    //
    //      KiRaiseAssertion => STATUS_ASSERTION_FAILURE
    //          TrapFrame->Rip = advanced backward to point to the original INT 2C opcode
    //          ExceptionAddress = same as TrapFrame->Rip
    //
    // Keep in mind in these calculations that on ARM, the PC in the trap frame
    // points to the illegal instruction itself, so appropriate adjustments are
    // made to the TrapFrame->Pc offset.
    //

    switch (Instruction) {

    case THUMB_BREAKPOINT:
        TrapFrame->Pc += 2;
        ExceptionRecord->ExceptionCode = STATUS_BREAKPOINT;
        ExceptionRecord->NumberParameters = 1;
        ExceptionRecord->ExceptionInformation[0] = BREAKPOINT_BREAK;
        ExceptionRecord->ExceptionAddress = (PVOID) (TrapFrame->Pc - 2);
        break;

    case THUMB_DEBUG_SERVICE:
        TrapFrame->Pc += 4;
        ExceptionRecord->ExceptionCode = STATUS_BREAKPOINT;
        ExceptionRecord->NumberParameters = 1;
        ExceptionRecord->ExceptionInformation[0] = ContextRecord->R12;
        ExceptionRecord->ExceptionAddress = (PVOID) TrapFrame->Pc;
        break;

    case THUMB_ASSERT:
        ExceptionRecord->ExceptionCode = STATUS_ASSERTION_FAILURE;
        ExceptionRecord->NumberParameters = 0;
        ExceptionRecord->ExceptionAddress = (PVOID) TrapFrame->Pc;
        break;

    case THUMB_FASTFAIL:
        ExceptionRecord->ExceptionCode = STATUS_STACK_BUFFER_OVERRUN;
        ExceptionRecord->NumberParameters = 1;
        ExceptionRecord->ExceptionInformation[0] = ContextRecord->R0;

        //
        // Dispatch the fast fail here, while we know that it is really a
        // genuine fail fast request (as the exception is fatal and cannot pass
        // through try/excepts).
        //
        // N.B.  APCs may be delivered during fast fail dispatching !
        //

        Result = KiDispatchFastFail(ExceptionRecord,
                                    ExceptionFrame,
                                    TrapFrame,
                                    ContextRecord,
                                    PreviousMode);
        break;

    case THUMB_DIVIDE_BY_0:
        ExceptionRecord->ExceptionCode = STATUS_INTEGER_DIVIDE_BY_ZERO;
        ExceptionRecord->NumberParameters = 0;
        ExceptionRecord->ExceptionAddress = (PVOID) ContextRecord->Pc;
        break;

    default:
        ExceptionRecord->ExceptionCode = STATUS_ILLEGAL_INSTRUCTION;
        ExceptionRecord->NumberParameters = 0;
        ExceptionRecord->ExceptionAddress = (PVOID) ContextRecord->Pc;
        break;
    }

    //
    // The ContextRecord->Rip is set to the same value as TrapFrame->Rip.
    // However, in AMD64's KiDispatchException, if STATUS_BREAKPOINT is detected
    // as the ExceptionCode, the ContextRecord->Rip is decremented by 1, pointing
    // back to the INT 3 opcode. Perform the equivalent operation for ARM.
    //

    if (ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) {
        ContextRecord->Pc = TrapFrame->Pc - 2;
    }

    if (OldIrql < APC_LEVEL) {
        KeLowerIrql(OldIrql);
    }

    return Result;
}

BOOLEAN
KiOpPreprocessAccessViolation (
    __in PEXCEPTION_RECORD ExceptionRecord,
    __inout PCONTEXT ContextRecord
    )

/*++

Routine Description:

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ContextRecord - Supplies a pointer to a context record.

Return Value:

    TRUE - the caller's context should be updated and control returned
           to the caller.

    FALSE - do not return control to the caller at this time, continue
            exception handling.

--*/

{
    PKTHREAD currentThread;
    PVOID faultVa;
    USHORT faultCount;
    ULONG_PTR slistFault;
    ULONG_PTR slistResume;

    if ((ContextRecord->Cpsr & CPSRM_MASK) == CPSRM_USER) {
        slistFault = (ULONG_PTR)KeUserPopEntrySListFault;
        slistResume = (ULONG_PTR)KeUserPopEntrySListResume;

    } else {
        slistFault = (ULONG_PTR)&ExpInterlockedPopEntrySListFault;
        slistResume = (ULONG_PTR)&ExpInterlockedPopEntrySListResume;
    }

    if (ContextRecord->Pc != slistFault) {
        return FALSE;
    }

    if ((ContextRecord->Cpsr & CPSRM_MASK) == CPSRM_SVC) {
        ContextRecord->Pc = slistResume;
        ContextRecord->Cpsr |= CPSRC_THUMB;
        ContextRecord->Cpsr &= ~CPSR_IT_MASK;
        return TRUE;
    }

    currentThread = KeGetCurrentThread();
    faultVa = (PVOID)ExceptionRecord->ExceptionInformation[1];

    if (faultVa == currentThread->SListFaultAddress) {

        faultCount = currentThread->SListFaultCount;
        if (faultCount > KI_SLIST_FAULT_COUNT_MAXIMUM) {
            currentThread->SListFaultCount = 0;
            return FALSE;
        }

        currentThread->SListFaultCount = faultCount + 1;

    } else {

        currentThread->SListFaultCount = 0;
        currentThread->SListFaultAddress = faultVa;
    }

    ContextRecord->Pc = slistResume;
    ContextRecord->Cpsr |= CPSRC_THUMB;
    ContextRecord->Cpsr &= ~CPSR_IT_MASK;
    return TRUE;

}

BOOLEAN
KeInvalidAccessAllowed (
    __in_opt PVOID TrapInformation
    )

/*++

Routine Description:

    Mm will pass a pointer to a trap frame prior to issuing a bug check on
    a pagefault.  This routine lets Mm know if it is ok to bugcheck.  The
    specific case we must protect are the interlocked pop sequences which can
    blindly access memory that may have been freed and/or reused prior to the
    access.  We don't want to bugcheck the system in these cases, so we check
    the instruction pointer here.

    For a usermode fault, Mm uses this routine for similar reasons, to determine
    whether a guard page fault should be ignored.

Arguments:

    TrapInformation - Supplies a trap frame pointer.  NULL means return False.

Return Value:

    True if the invalid access should be ignored, False otherwise.

--*/

{
    BOOLEAN result;
    ULONG_PTR slistFaultIP;
    PKTRAP_FRAME trapFrame;

    if (ARGUMENT_PRESENT(TrapInformation) == FALSE) {
        return FALSE;
    }

    trapFrame = TrapInformation;
    result = FALSE;

    if ((trapFrame->Cpsr & CPSRM_MASK) == CPSRM_SVC) {

        //
        // Fault occured in kernel mode
        //

        slistFaultIP = (ULONG_PTR)&ExpInterlockedPopEntrySListFault;

    } else {

        //
        // Fault occured in native usermode
        //

        slistFaultIP = (ULONG_PTR)KeUserPopEntrySListFault;
    }

    if (trapFrame->Pc == slistFaultIP) {
        result = TRUE;
    }

    return result;
}

BOOLEAN
KiIsExclusiveInstruction (
    __in ULONG Instruction
    )

/*++

Routine Description:

    This routine determines if the instruction is an store or load
    exclusive i.e. an interlocked operation.

Arguments:

    Instruction - Supplies the instruction that caused the fault.

Return Value:

    TRUE - Instruction is an exclusive operation.

    FALSE - Instruction is not an exclusive operation.

--*/

{
    Instruction &= 0xFFF000F0;

    switch (Instruction) {
    case 0xE8C00040:            // STREXB instruction
    case 0xE8C00050:            // STREXH instruction
    case 0xE8C00070:            // STREXD instruction
    case 0xE8D00040:            // LDREXB instruction
    case 0xE8D00050:            // LDREXH instruction
    case 0xE8D00070:            // LDREXD instruction
        return TRUE;
    }

    Instruction &= 0xFFF00000;

    //
    // Test for LDREX and STREX instructions
    //

    if ((Instruction == 0xE8400000) ||
        (Instruction == 0xE8500000)) {
        return TRUE;
    }

    return FALSE;
}


BOOLEAN
KiPreprocessHardwareError (
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This routine attempts to further determine the cause of the hardware error
    and take appropriate action, which can include

        - Updating the exception record with a more appropriate status

        - Changing the meaning of the exception

    Currently it converts hardware errors into more manageable exceptions.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ContextRecord - Supplies a pointer to a context record.

    TrapFrame - Supplies a pointer to the trap frame.

Return Value:

    TRUE - the caller's context should be updated and control returned
           to the caller.

    FALSE - the exception should be raised with (perhaps) an updated
            context record.

--*/

{

    ULONG Instruction;
    KIRQL OldIrql;
    BOOLEAN Result;

    Result = FALSE;

    //
    // Block potential concurrent attempts to modify the trap frame via
    // the context setting interface.
    //

    OldIrql = KeGetCurrentIrql();
    if (OldIrql < APC_LEVEL) {
        KfRaiseIrql(APC_LEVEL);
    }

    ExceptionRecord->NumberParameters = 0;
    ExceptionRecord->ExceptionCode = STATUS_HARDWARE_MEMORY_ERROR;

    //
    // Only decode instruction in thumb.  Only examine the instruction
    // if the fault address is not the instruction itself.
    //

    if (((ContextRecord->Pc & 1) != 0) &&
        ((ContextRecord->Pc & ~1) != TrapFrame->FaultAddress)) {

        Instruction = 0;
        if ((ContextRecord->Cpsr & CPSRM_MASK) == CPSRM_SVC) {
            Instruction = *(PUSHORT) (ContextRecord->Pc - 1);
            if (Instruction >= 0xe800) {
                Instruction <<= 16;
                Instruction |= *(PUSHORT) (ContextRecord->Pc + 1);
            }

        } else {

            //
            // Fetching from user mode requires probing.
            //

            try {
                Instruction = ProbeAndReadUshort((PUSHORT) (ContextRecord->Pc - 1));
                if (Instruction >= 0xe800) {
                    Instruction <<= 16;
                    Instruction |= ProbeAndReadUshort((PUSHORT) (ContextRecord->Pc + 1));
                }

            } except (EXCEPTION_EXECUTE_HANDLER) {
                NOTHING;
            }
        }

        //
        // Adjust exception code if the instruction is an exclusive
        // operation
        //

        if (KiIsExclusiveInstruction(Instruction)) {
            ExceptionRecord->ExceptionCode = STATUS_ILLEGAL_INSTRUCTION;
        }
    }

    if (OldIrql < APC_LEVEL) {
        KeLowerIrql(OldIrql);
    }

    return Result;
}

BOOLEAN
KiDispatchFastFail (
    __inout PEXCEPTION_RECORD ExceptionRecord,
    __in PKEXCEPTION_FRAME ExceptionFrame,
    __inout PKTRAP_FRAME TrapFrame,
    __inout PCONTEXT ContextRecord,
    __in KPROCESSOR_MODE PreviousMode
    )

/*++

Routine Description:

    This routine dispatches a fail fast request.  If the previous mode was user
    then a second chance exception is ralsed, else if the previous mode was
    kernel then a bug check is initiated.

    N.B.  The routine is entered at APC level to preserve the integrity of the
          trap frame but as we perform no trap frame edits here, we are free to
          drop IRQL.  The caller guarantees that they do not require atomicity
          of an edited trap frame across a fast fail request.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame.

    ContextRecord - Supplies a pointer to a context record.

    TrapFrame - Supplies a pointer to the trap frame.

    PreviousMode - Supplies the execution mode at the time of the exception.

Return Value:

    TRUE - the caller's context should be updated and control returned
           to the caller.

    FALSE - the exception should be raised with (perhaps) an updated
            context record.

--*/

{
    ULONG FailCode;
    KIRQL OldIrql;
    BOOLEAN Result;

    UNREFERENCED_PARAMETER(ContextRecord);
    ExceptionRecord->ExceptionFlags |= EXCEPTION_NONCONTINUABLE;

    Result = FALSE;

    if (PreviousMode == UserMode) {

        //
        // Run the exception up as a second chance exception.  This prevents
        // a frame unwind, but allows the debugger (and the error reporting
        // service) a shot at the exception before the process goes away.
        //
        // N.B.  There is no return from a second chance exception unless we
        //       are being debugged.
        //
        // N.B.  This results in a nested invocation to KiPreprocessFault, but
        //       now under the guise of the fail fast exception code and not
        //       KI_EXCEPTION_INVALID_OP.
        //

        OldIrql = KeGetCurrentIrql();
        NT_ASSERT(OldIrql == APC_LEVEL);
        KeLowerIrql(PASSIVE_LEVEL);

        KiDispatchException(ExceptionRecord,
                            ExceptionFrame,
                            TrapFrame,
                            PreviousMode,
                            FALSE);

        KfRaiseIrql(OldIrql);

        Result = TRUE;
    } else {

        //
        // Extract the fast fail code from the exception record.
        //

        if (ExceptionRecord->NumberParameters >= 1) {

            FailCode = (ULONG)ExceptionRecord->ExceptionInformation[0];
        } else {

            FailCode = FAST_FAIL_INVALID_FAST_FAIL_CODE;
        }

        //
        // Now perform the fast fail bug check, this is a kernel caller.
        //

        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE,
                     FailCode,
                     (ULONG64)TrapFrame,
                     (ULONG64)ExceptionRecord,
                     0);
    }

    return Result;
}

