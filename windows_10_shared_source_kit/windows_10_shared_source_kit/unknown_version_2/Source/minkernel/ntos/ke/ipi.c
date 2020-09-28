/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    ipi.c

Abstract:

    This module implements ARM specific interprocessor interrupt
    routines.

Environment:

    Kernel mode only.

--*/

#include "ki.h"
#include "ntrtlp.h"

VOID
KiRestoreProcessorState (
    __in PKTRAP_FRAME TrapFrame,
    __in PKEXCEPTION_FRAME ExceptionFrame
    )

/*++

Routine Description:

    This function restores the processor state to the specified exception
    and trap frames, and restores the processor control state.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame.

    ExceptionFrame - Supplies a pointer to an exception frame.

Return Value:

    None.

--*/

{
    PKPRCB Prcb;
    KPROCESSOR_MODE PreviousMode;

    //
    // Get the address of the current processor block, move the specified
    // register state from the processor context structure to the specified
    // trap and exception frames, and restore the processor control state.
    //

    if ((TrapFrame->Cpsr & CPSRM_MASK) == CPSRM_USER) {
        PreviousMode = UserMode;
    } else {
        PreviousMode = KernelMode;
    }

    Prcb = KeGetCurrentPrcb();
    KeContextToKframes(TrapFrame,
                       ExceptionFrame,
                       Prcb->Context,
                       Prcb->Context->ContextFlags,
                       PreviousMode);

    KiRestoreProcessorControlState(&Prcb->ProcessorState);
}

VOID
KiSaveProcessorState (
    __in PKTRAP_FRAME TrapFrame,
    __in PKEXCEPTION_FRAME ExceptionFrame
    )

/*++

Routine Description:

    This function saves the processor state from the specified exception
    and trap frames, and saves the processor control state.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame.

    ExceptionFrame - Supplies a pointer to an exception frame.

Return Value:

    None.

--*/

{

    PCONTEXT Context;
    PKPRCB Prcb;

    //
    // Get the address of the current processor block, move the specified
    // register state from specified trap and exception frames to the current
    // processor context structure, and save the processor control state.
    //

    Prcb = KeGetCurrentPrcb();
    Context = Prcb->Context;
    Context->ContextFlags = Prcb->ContextFlagsInit;
    KeContextFromKframes(TrapFrame, ExceptionFrame, Context);

    KiSaveProcessorControlState(&Prcb->ProcessorState);

    return;
}

