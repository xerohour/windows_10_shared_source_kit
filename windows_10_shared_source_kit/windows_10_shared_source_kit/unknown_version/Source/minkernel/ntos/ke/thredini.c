/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    thredini.c

Abstract:

    This module implements the machine dependent function to set the initial
    context and data alignment handling mode for a process or thread object.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

VOID
KiInitializeContextThread (
    IN PKTHREAD Thread,
    IN PKSYSTEM_ROUTINE SystemRoutine,
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL,
    IN PCONTEXT ContextRecord OPTIONAL
    )

/*++

Routine Description:

    This function initializes the machine dependent context of a thread
    object.

    N.B. This function does not check if context record is accessibile.
         It is assumed the the caller of this routine is either prepared
         to handle access violations or has probed and copied the context
         record as appropriate.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

    SystemRoutine - Supplies a pointer to the system function that is to be
        called when the thread is first scheduled for execution.

    StartRoutine - Supplies an optional pointer to a function that is to be
        called after the system has finished initializing the thread. This
        parameter is specified if the thread is a system thread and will
        execute totally in kernel mode.

    StartContext - Supplies an optional pointer to a data structure that
        will be passed to the StartRoutine as a parameter. This parameter
        is specified if the thread is a system thread and will execute
        totally in kernel mode.

    ContextRecord - Supplies an optional pointer a context record which
        contains the initial user mode state of the thread. This parameter
        is specified if the thread will execute in user mode.

Return Value:

    None.

--*/

{

    CONTEXT ContextFrame;
    PKEXCEPTION_FRAME ExFrame;
    ULONG_PTR InitialStack;
    PKSTART_FRAME SfFrame;
    PKSTACK_CONTROL StackControl;
    PKSWITCH_FRAME SwFrame;
    PKTRAP_FRAME TrFrame;
    PKARM_VFP_STATE VfpState;

    //
    // Initialize VFP save area
    //

    InitialStack = (ULONG_PTR)Thread->InitialStack;

    VfpState = (PKARM_VFP_STATE)InitialStack - 1;

    NT_ASSERT(((ULONG_PTR)VfpState & STACK_ROUND) == 0);

    RtlZeroMemory(VfpState, sizeof(*VfpState));
    
    //
    // Allocate stack control region.
    //

    StackControl = (PKSTACK_CONTROL)VfpState - 1;

    NT_ASSERT(((ULONG_PTR)StackControl & STACK_ROUND) == 0);

    //
    // Initialize the current kernel stack segment descriptor in the kernel
    // stack control area. This descriptor is used to control kernel stack
    // expansion from drivers.
    //
    // N.B. The previous stack segment descriptor is zeroed.
    //

    RtlZeroMemory(StackControl, InitialStack - (ULONG_PTR)StackControl);

    StackControl->StackBase = InitialStack;
    StackControl->ActualLimit = InitialStack - KeKernelStackSize;

    //
    // If a context record is specified, then initialize a trap frame, and
    // an exception frame with the specified user mode context.
    //
    // N.B. The initial context of a thread cannot set the debug or floating
    //      state.
    //

    if (ARGUMENT_PRESENT(ContextRecord)) {
        ContextFrame = *ContextRecord;
        ContextRecord = &ContextFrame;
        ContextRecord->ContextFlags &=
            CONTEXT_CONTROL | CONTEXT_INTEGER;

        //
        // Force control registers on. This will make KeContextToKframes
        // function to sanitize control registers in the trap frame even if
        // the context record is invalid.
        //

        ContextRecord->ContextFlags |= CONTEXT_CONTROL;

        //
        // Allocate a trap frame, an exception frame, a start frame and a
        // context switch frame.
        //

        TrFrame = (PKTRAP_FRAME)(((ULONG)StackControl - KTRAP_FRAME_LENGTH - sizeof(KARM_VFP_STATE)));
        ExFrame = (PKEXCEPTION_FRAME)(((ULONG)TrFrame - KEXCEPTION_FRAME_LENGTH));
        SfFrame = (PKSTART_FRAME)(((ULONG)ExFrame - KSTART_FRAME_LENGTH));
        SwFrame = (PKSWITCH_FRAME)(((ULONG)SfFrame - KSWITCH_FRAME_LENGTH));

        //
        // Zero the exception and trap frames and copy information from the
        // specified context frame to the trap and exception frames.
        //

        RtlZeroMemory(ExFrame, sizeof(KEXCEPTION_FRAME));
        RtlZeroMemory(TrFrame, sizeof(KTRAP_FRAME) + sizeof(KARM_VFP_STATE));

#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
        TrFrame->VfpState = (PKARM_VFP_STATE)(TrFrame + 1);
#endif

        //
        // Set the trap frame type in the trap frame.
        //

        TrFrame->ExceptionActive = KEXCEPTION_ACTIVE_EXCEPTION_FRAME;

        KeContextToKframes(TrFrame,
                           ExFrame,
                           ContextRecord,
                           ContextRecord->ContextFlags,
                           UserMode);

        //
        // Initialize user thread startup information.
        //

        SfFrame->R0 = (ULONG)StartContext;
        SfFrame->R1 = (ULONG)StartRoutine;
        SfFrame->R2 = (ULONG)SystemRoutine;
        ExFrame->Return = (ULONG)KiStartUserThreadReturn;


        //
        // Initialize start address.
        //

        SwFrame->Return = (ULONG)KiStartUserThread;

        //
        // Set initial coprocessor state
        //

        //
        // Set the saved previous processor mode in the thread object.
        //

        NT_ASSERT(Thread->SystemThread == FALSE);

        Thread->PreviousMode = UserMode;

    } else {

        //
        // Allocate an exception frame and a context switch frame.
        //

        TrFrame = NULL;
        SfFrame = (PKSTART_FRAME)(((ULONG)StackControl - KSTART_FRAME_LENGTH));
        SwFrame = (PKSWITCH_FRAME)(((ULONG)SfFrame - KSWITCH_FRAME_LENGTH));

        //
        // Initialize the system thread start frame.
        //

        SfFrame->R0 = (ULONG)StartContext;
        SfFrame->R1 = (ULONG)StartRoutine;
        SfFrame->R2 = (ULONG)SystemRoutine;
        SfFrame->Return = 0;

        //
        // Initialize start address.
        //

        SwFrame->Return = (ULONG)KiStartSystemThread;

        //
        // Set the previous mode in thread object to kernel.
        //

        NT_ASSERT(Thread->SystemThread != FALSE);

        Thread->PreviousMode = KernelMode;
    }

    //
    // Initialize context switch frame and set thread start up parameters.
    //

    SwFrame->ApcBypass = APC_LEVEL;
    SwFrame->R11 = (ULONG)TrFrame;

    //
    // Set the initial kernel stack pointer and floating point save
    // area.
    //

    Thread->InitialStack = (PVOID)StackControl;
    Thread->KernelStack = SwFrame;
    Thread->VfpState = VfpState;
    return;
}

