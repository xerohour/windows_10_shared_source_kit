/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    transitc.c

Abstract:

    This module contains ARM specific code for the transitioning of control
    between boot applications.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>
#include "context.h"

//
// -------------------------------------------------------------------- Globals
//

VOID
BlpArchTransferToApplicationState (
    __in PBOOT_APPLICATION_ENTRYPOINT EntryRoutine,
    __inout PBOOT_APPLICATION_PARAMETERS EntryParameters,
    __in PVOID Stack
    );

PEXECUTION_CONTEXT
ArchGetContext (
    __in EXECUTION_CONTEXT_TYPE ContextType
    );

VOID
ArchSetInitialContext (
    __in PEXECUTION_CONTEXT Context
    );

VOID
ArchSwitchContext (
    __in PEXECUTION_CONTEXT NewContext,
    __in_opt PEXECUTION_CONTEXT CurrentContext
    );

//
// ------------------------------------------------------------------ Functions
//

DECLSPEC_NOINLINE
NTSTATUS
BlpArchTransferToEfiBootApplication (
    __in PBOOT_APPLICATION_ENTRYPOINT EntryRoutine,
    __inout PBOOT_APPLICATION_PARAMETERS EntryParameters,
    __in PVOID ApplicationContext,
    __in PVOID Stack
    )

/*++

Routine Description:

    Transfers exeuction to an EFI boot application.

Arguments:

    EntryRoutine - Pointer to the entry point for the x64 boot application.

    EntryParameters - Input parameters for the boot application to start.

    ApplicationContext - Supplies a pointer to the context to instantiate.

    Stack - Allocated stack for the boot application.

Return Value:

    The return status of the loaded application.

--*/

{

    PEXECUTION_CONTEXT PreviousContext;
    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArguments;
    NTSTATUS Status;

    //
    // If the debugger is currently connected, stop it.  That way the
    // new boot application can start a new session.
    //

    BlBdStop();

    //
    // Capture the current context in case we return from the
    // application.
    //

    PreviousContext = ArchGetCurrentContext();
    ArchSwitchContext(ApplicationContext, PreviousContext);

    //
    // Transfer control.
    //

    BlpArchTransferToApplicationState(EntryRoutine,
                                      EntryParameters,
                                      Stack);

    //
    // Resume previous context
    //

    ArchSwitchContext(PreviousContext, ApplicationContext);

    //
    // Resume the boot debugger, if it was formerly connected.
    //

    BlBdStart();

    //
    // A boot application's returned the status is contained within the
    // input parameter structure.
    //

    ReturnArguments = Add2Ptr(EntryParameters,
                              EntryParameters->ReturnArgumentOffset);

    if (ReturnArguments->Version >= 1) {
        Status = ReturnArguments->ReturnStatus;

    } else {

        Status = STATUS_SUCCESS;
    }

    return Status;
}

