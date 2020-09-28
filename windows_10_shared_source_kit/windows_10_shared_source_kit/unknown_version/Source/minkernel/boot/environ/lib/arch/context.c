/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    context.c

Abstract:

    This module implements architecture-independent execution context
    manipulation routines.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "arch.h"

// -------------------------------------------------------------------- Globals

//
// Define storage for the two possible execution contexts.
//

EXECUTION_CONTEXT FirmwareExecutionContext;
EXECUTION_CONTEXT ApplicationExecutionContext;

//
// Define the current execution context for the boot application.
//

PEXECUTION_CONTEXT CurrentExecutionContext;

// ------------------------------------------------------------------ Functions

NTSTATUS
ArchInitializeContexts (
    VOID
    )

/*++

Routine Description:

    This routine initializes the firmware and application execution contexts
    for the boot application.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS ApplicationStatus;
    NTSTATUS FirmwareStatus;
    ULONG FirmwareSupported;

    //
    // Initialize global execution contexts.
    //

    CurrentExecutionContext = NULL;
    FirmwareExecutionContext.ContextType = ExecutionContextFirmware;
    ApplicationExecutionContext.ContextType = ExecutionContextApplication;

    //
    // If an application execution context is supported, then the
    // application should run in its own context.
    //

    ApplicationStatus = ArchInitializeContext(&ApplicationExecutionContext);
    if (NT_SUCCESS(ApplicationStatus)) {
        CurrentExecutionContext = &ApplicationExecutionContext;
    }

    //
    // Initialize the firmware execution contexts, if supported.
    //

    FirmwareSupported =
        CHECK_FLAG(BlPlatformFlags, FW_FLAGS_EXECUTION_CONTEXT_SUPPORTED);

    if (FirmwareSupported != 0) {
        FirmwareStatus = ArchInitializeContext(&FirmwareExecutionContext);
        if (!NT_SUCCESS(FirmwareStatus)) {
            return FirmwareStatus;
        }

        //
        // If a firmware context is supported but the application context is not
        // supported, then the application should run in the firmware's context.
        //
        // Additionally, ensure that the firmware context is set. Some
        // applications can be launched with their own page tables and GDT/IDT,
        // and with interrupts disabled, but need to run in the firmware's
        // context. This will set them back to the firmare's context.
        //

        if (!NT_SUCCESS(ApplicationStatus)) {
            CurrentExecutionContext = &FirmwareExecutionContext;
        }
    }

    ASSERT(CurrentExecutionContext != NULL);
    ArchSetInitialContext(CurrentExecutionContext);

    return STATUS_SUCCESS;
}

VOID
BlpArchSwitchContext (
    __in EXECUTION_CONTEXT_TYPE ContextType
    )

/*++

Routine Description:

    This routine transitions a boot application to execute within the
    specified execution context. On return, the application will be running
    in the new context.

Arguments:

    ContextType - Supplies the type of the target execution context to switch
        the boot application to.

Return Value:

    None.

--*/

{

    PEXECUTION_CONTEXT NewContext;

    BlpInterfaceEnter(InterfaceArch);
    NewContext = ArchGetContext(ContextType);

    //
    // Switch to the target execution context if the application is not already
    // executing within it.
    //

    if (BlpArchQueryCurrentContextType() != ContextType) {
        ArchSwitchContext(NewContext, CurrentExecutionContext);
        CurrentExecutionContext = NewContext;
    }

    BlpInterfaceExit(InterfaceArch);
    return;
}

EXECUTION_CONTEXT_TYPE
BlpArchQueryCurrentContextType (
    VOID
    )

/*++

Routine Description:

    This routine returns the current execution context type for the boot
    application.

Arguments:

    None.

Return Value:

    EXECUTION_CONTEXT_TYPE - Type of the execution context within which the boot
        application is currently running.

--*/

{

    BlpInterfaceEnter(InterfaceArch);

    ASSERT(CurrentExecutionContext != NULL);

    BlpInterfaceExit(InterfaceArch);
    return CurrentExecutionContext->ContextType;
}

PEXECUTION_CONTEXT
ArchGetContext (
    __in EXECUTION_CONTEXT_TYPE ContextType
    )

/*++

Routine Description:

    This routine returns the execution context of the requested type.

Arguments:

    ContextType - Supplies the type of context the caller is requesting.

Return Value:

    PEXECUTION_CONTEXT - The execution context of the type requested.

--*/

{

    PEXECUTION_CONTEXT Context;

    if (ContextType == ExecutionContextFirmware) {
        Context = &FirmwareExecutionContext;

    } else {
        Context = &ApplicationExecutionContext;
    }

    return Context;
}

PEXECUTION_CONTEXT
ArchGetCurrentContext (
    VOID
    )

/*++

Routine Description:

    This routine returns the current execution context.

Arguments:

    None.

Return Value:

    PEXECUTION_CONTEXT - The current execution context.

--*/

{

    return CurrentExecutionContext;
}

#if defined(_X86_) || defined(_AMD64_)

NTSTATUS
ArchExecuteTransition (
    __in PARCH_TRANSITION_ROUTINE TransitionRoutine,
    __inout PBOOT_APPLICATION_PARAMETERS EntryParameters
    )

/*++

Routine Description:

    This routine contains common X86/AMD64 transition code when launching boot
    application.

Arguments:

    TransitionRoutine - Supplies architecture specific transition routine to
        execute final transition to the boot application being launched.

    EntryParameters - Supplies pointer to the input parameters for the boot
        application.

Return Value:

    Status value returned by the application.

--*/

{

    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArguments;

    //
    // Restore processor features enabled by the boot application to their
    // original state.
    //

    ArchRestoreProcessorFeatures();

    //
    // If the debugger is currently connected, stop it. That way the new boot
    // application can start a new session.
    //

    BlBdStop();

    //
    // Call into architecture specific routine to transfer control.
    //

    (*TransitionRoutine)();

    //
    // Resume the boot debugger, if it was formerly connected.
    //

    BlBdStart();

    //
    // Enable processor features being used in the boot environment.
    //

    ArchEnableProcessorFeatures();

    //
    // A boot application's returned the status is contained within the
    // input parameter structure.
    //

    ReturnArguments = Add2Ptr(EntryParameters,
                              EntryParameters->ReturnArgumentOffset);

    if (ReturnArguments->Version >= 1) {
        return ReturnArguments->ReturnStatus;
    }

    return STATUS_SUCCESS;
}

#endif

