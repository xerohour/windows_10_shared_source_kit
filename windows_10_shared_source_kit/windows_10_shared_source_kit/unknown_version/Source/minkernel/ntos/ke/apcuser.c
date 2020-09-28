/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    apcuser.c

Abstract:

    This module implements the machine dependent code necessary to initialize
    a user mode APC.

Environment:

    Kernel mode only, IRQL APC_LEVEL.

--*/

#include "ki.h"
#include "ntrtlp.h"

VOID
KiInitializeUserApc (
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN PKNORMAL_ROUTINE NormalRoutine,
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )

/*++

Routine Description:

    This function is called to initialize the context for a user mode APC.

Arguments:

    ExceptionFrame - Supplies a pointer to an exception frame.

    TrapFrame - Supplies a pointer to a trap frame.

    NormalRoutine - Supplies a pointer to the user mode APC routine.

    NormalContext - Supplies a pointer to the user context for the APC
        routine.

    SystemArgument1 - Supplies the first system supplied value.

    SystemArgument2 - Supplies the second system supplied value.

Return Value:

    None.

--*/

{

    PKAPC_RECORD ApcRecord;
    PCONTEXT ContextRecord;
    EXCEPTION_RECORD ExceptionRecord;
    PMACHINE_FRAME MachineFrame;
    ULONG UserStack;

    //
    // Transfer the context information to the user stack, initialize the
    // APC routine parameters, and modify the trap frame so execution will
    // continue in user mode at the user mode APC dispatch routine.
    //

    try {
    
        //
        // AMD64 has an optimization here for referencing the context directly
        // from the user stack. It is unclear whether this is really worthwhile
        // to do for ARM.

        {
            //
            // Compute address of aligned machine frame, compute address of
            // context record, and probe user stack for writeability.
            //
    
            MachineFrame = (PMACHINE_FRAME)
                ((TrapFrame->Sp - ARM_RED_ZONE_BYTES - sizeof(MACHINE_FRAME)) & ~STACK_ROUND);
    
            ContextRecord = (PCONTEXT)(((ULONG)MachineFrame - sizeof(CONTEXT)) & ~CONTEXT_ROUND);
            ApcRecord = (PKAPC_RECORD)(((ULONG)ContextRecord - sizeof(KAPC_RECORD)) & ~STACK_ROUND);
            UserStack = ((ULONG_PTR) MachineFrame - (ULONG_PTR) ApcRecord) +
                sizeof(MACHINE_FRAME);

            ProbeForWriteSmallStructure(ApcRecord,
                                        UserStack,
                                        STACK_ALIGN);
    
            //
            // Move machine state from trap and exception frames to the context
            // record on the user stack.
            //
    
            ContextRecord->ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
            KeContextFromKframes(TrapFrame, ExceptionFrame, ContextRecord);

            //
            // Fill in machine frame information.
            //
    
            MachineFrame->Sp = ContextRecord->Sp;
            MachineFrame->Pc = ContextRecord->Pc;
        }

        //
        // Initialize the user APC parameters.
        //

        ApcRecord->NormalContext = NormalContext;
        ApcRecord->SystemArgument1 = SystemArgument1;
        ApcRecord->SystemArgument2 = SystemArgument2;
        ApcRecord->NormalRoutine = NormalRoutine;

        //
        // Set the address new stack pointer in the current trap frame and
        // the continuation address so control will be transfered to the user
        // APC dispatcher.
        //

        TrapFrame->Sp = (ULONG)ApcRecord;
        TrapFrame->Pc = (ULONG)KeUserApcDispatcher;
        TrapFrame->Cpsr |= CPSRC_THUMB;
        TrapFrame->Cpsr &= ~CPSR_IT_MASK;

        //
        // If the trap frame is not a system service trap frame (which will
        // already have instrumentation applied) then activate instrumentation
        // as appropriate.
        //

        if (TrapFrame->ExceptionActive != KEXCEPTION_ACTIVE_SERVICE_FRAME) {
            KiSetupForInstrumentationReturn(TrapFrame);
        }

        KiRecordInstrumentationTrapFrame(TrapFrame);

    } except (KiCopyInformation(&ExceptionRecord,
                                (GetExceptionInformation())->ExceptionRecord)) {

        //
        // Lower the IRQL to PASSIVE_LEVEL, set the exception address to
        // the current program address, and raise an exception by calling
        // the exception dispatcher.
        //
        // N.B. The IRQL is lowered to PASSIVE_LEVEL to allow APC interrupts
        //      during the dispatching of the exception. The current thread
        //      will be terminated during the dispatching of the exception,
        //      but lowering of the IRQL is required to enable the debugger
        //      to obtain the context of the current thread.
        //

        KeLowerIrql(PASSIVE_LEVEL);
        ExceptionRecord.ExceptionAddress = (PVOID)(TrapFrame->Pc);
        KiDispatchException(&ExceptionRecord,
                            ExceptionFrame,
                            TrapFrame,
                            UserMode,
                            TRUE);
    }

    return;
}

