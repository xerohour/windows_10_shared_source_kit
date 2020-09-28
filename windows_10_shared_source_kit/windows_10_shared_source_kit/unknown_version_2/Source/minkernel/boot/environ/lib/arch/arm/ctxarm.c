/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ctxarm.c

Abstract:

    This module implements execution context manipulation routines for the ARM
    platform.

Environment:

    Boot

--*/

//
// Define the application and firmware paging contexts for an ARM boot
// application.
//
// Application paging context:
// ---------------------------
//
// 1. Non-identity mappings are supported.
// 2. The page-table self-mapping is established.  Page tables can be accessed
//    using page-table virtual address space.
// 3. The application has complete control over the paging context (i.e., it
//    can establish or delete any mappings desired by it).
//
// Firmware paging context:
// ------------------------
//
// 1. Only identity mappings are supported.  All pointers can be interpreted as
//    physical address pointers.
// 2. The firmware paging context must be used by the application for all
//    firmware calls.
// 3. The firmware has complete control over the paging context.
// 4. The boot application only has additive control (i.e., it can establish
//    new identity mappings but never delete them).
//
// An application runs in the application paging context during normal
// execution.  The application switches to the firmware paging context for
// making firmware calls.  The context is reverted after the completion of
// firmware call.
//
// N.B. In the following scenarios, cases firmware and application contexts are identical:
//      ARM EFI when application is loaded by firmware -  In this scenario,
//          there is no application context.  The firmware context is used by
//          the boot application.
//
// Execution contexts and translation type
// ------------------------------------
//
// VA_TRANSLATION_TYPE_NONE: Applications run in "physical mode" with
//      address translation disabled. The application paging context is
//      identical to the firmware paging context except for the exception
//      vectors and interrupts are disabled.
//
//  N.B. Physical mode on ARM means physical address == virtual address
//       (i.e., identity mappings only).
//
//  N.B. The application can be made to run in the application context
//       by disabling address translation. However, since switch of paging
//       contexts is currently coupled to enabling/disabling translation,
//       it introduces the following problem:
//       A boot application requests disabling of address translation
//       only if it finds translation enabled. With address translation already
//       disabled in application context, the boot application will never
//       attempt to disable translation and hence never switch to the firmware
//       context for making firmware calls. This can be enabled once
//       switching of paging contexts is decoupled from enabling/disabling
//       of translation.
//
// VA_TRANSLATION_TYPE_DEFAULT: Applications run in the application context and
//      switch to the firmware context for making firmware calls (the firmware
//      context could be similar to application context in some cases).
//

//
// ------------------------------------------------------------------- Includes
//

#include "context.h"

//
// -------------------------------------------------------------------- Globals
//

//
// References to exception vector table
//

extern void (*ArchStubExceptionVectors[])(VOID);

//
// ------------------------------------------------------------------ Functions
//

VOID
ArchDestroyContexts (
    VOID
    )

/*++

Routine Description:

    This routine destroys the executions contexts and resets the processor state
    to that which the application started in as necessary.

Arguments:

    None.

Return Value:

    None.

--*/

{

    VA_TRANSLATION_TYPE TranslationType;

    //
    // If the application is running without address translation, but in the
    // application's context, then it needs to switch back into the firmwares
    // mode as the application is about the exit to the firmware. This should
    // only be the case in the boot manager. If another application is running
    // without translation, then the parent (boot manager) will restore its own
    // state when the child exits.
    //

    TranslationType = BlGetCurrentLibraryParameters().TranslationType;
    if ((TranslationType == VA_TRANSLATION_TYPE_NONE) &&
        (BlpArchQueryCurrentContextType() == ExecutionContextApplication)) {

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    return;
}

NTSTATUS
ArchInitializeApplicationContext (
    __out PEXECUTION_CONTEXT ApplicationContext
    )

/*++

Routine Description:

    This routine initializes the application execution context. It sets the page
    directory address, the address translation state, and the interrupt state
    for the application context.

Arguments:

    ApplicationContext - Supplies a pointer to the application context to be
        initialized.

Return Value:

    STATUS_SUCCESS if successful. STATUS_NOT_SUPPORTED if the current boot
    application was launched by the firmware with address translation disabled.

--*/

{

    VA_TRANSLATION_TYPE TranslationType;

    TranslationType = BlGetCurrentLibraryParameters().TranslationType;

    //
    // Application context is not supported on ARM architecture if virtual
    // address translation is disabled. This means that the application will use
    // the firmware's page tables. If the application were to use its own page
    // tables here, a 1:1 mapping would have to be created for all of physical
    // memory.
    //

    if (TranslationType == VA_TRANSLATION_TYPE_NONE) {
        return STATUS_NOT_SUPPORTED;
    }

    ApplicationContext->AddressTranslationEnabled = 1;

    //
    // Override BOOTMGR configured exception vectors.
    //

    BlpArchInstallExceptionVectors(ArchStubExceptionVectors);

    //
    // Capture the application context created in BOOTMGR
    //
    // N.B. It could be argued that this information should be just passed
    //      to the application rather than inferred. One possible explanation
    //      is that inference is easier than supporting a mix of BOOTMGR and
    //      and applications from different OS revisions.
    //

    BlpMmArmCaptureApplicationMmState(&ApplicationContext->MmState);
    BlpArchArmCaptureExceptionState(&ApplicationContext->ExceptionState);

    //
    // Boot applications run with the interrupts disabled when in the
    // application's context.
    //

    ApplicationContext->InterruptState = 0;
    return STATUS_SUCCESS;
}

NTSTATUS
ArchInitializeBootmgrApplicationContext (
    __out PEXECUTION_CONTEXT ApplicationContext
    )

/*++

Routine Description:

    This routine initializes the bootmgr application execution
    context. This context is derivative of the firmware context but with
    interrupts disabled and the exception vectors redirected.

Arguments:

    ApplicationContext - Supplies a pointer to the application context to be
        initialized.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{

    PARM_EXCEPTION_STATE ExceptionState;
    ULONG Flags;
    VA_TRANSLATION_TYPE TranslationType;

    TranslationType = BlGetCurrentLibraryParameters().TranslationType;
    Flags = BlGetCurrentLibraryParameters().Flags;

    ASSERT(TranslationType == VA_TRANSLATION_TYPE_NONE);

    //
    // If the application requested to stay in the firmware's context, do not
    // switch exception state. Such applications cannot be debugged.
    //

    if (CHECK_FLAG(Flags, LF_FORCE_FIRMWARE_CONTEXT) != 0) {
        return STATUS_NOT_SUPPORTED;
    }

    ArchFwInitializeFirmwareContext(ApplicationContext);

    //
    // Alter context to specify exceptions are taken in Thumb-2 mode,
    // enable alignment fixups and set the exception vectors to our
    // own.
    //

    ExceptionState = &ApplicationContext->ExceptionState;
    ExceptionState->Control &= ~(CP15_SCTLR_A | CP15_SCTLR_V);
    ExceptionState->Control |= CP15_SCTLR_TE;
    ExceptionState->Vbar = ((ULONG_PTR) ArchStubExceptionVectors) & ~1;

    //
    // Boot applications run with the interrupts disabled when in the
    // application's context.
    //

    ApplicationContext->InterruptState = 0;
    return STATUS_SUCCESS;
}

NTSTATUS
ArchInitializeContext (
    __inout PEXECUTION_CONTEXT Context
    )

/*++

Routine Description:

    This routine initializes the specified execution context on x64 platform.

Arguments:

    Context - Supplies a pointer to the execution context to be initialized.

Return Value:

    NTSTATUS code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    NTSTATUS Status;
    VA_TRANSLATION_TYPE TranslationType;

    TranslationType = BlGetCurrentLibraryParameters().TranslationType;
    ContextType = Context->ContextType;

    ASSERT((ContextType == ExecutionContextFirmware) ||
           (ContextType == ExecutionContextApplication));

    if (ContextType == ExecutionContextApplication) {

        //
        // if the translation type is NONE, then this code is
        // executing in bootmgr.  Establish a bootmgr-specific
        // application context where we can take our own exceptions e.g.
        // enable the debugger.
        //

        if (TranslationType == VA_TRANSLATION_TYPE_NONE) {
            Status = ArchInitializeBootmgrApplicationContext(Context);

        } else {
            Status = ArchInitializeApplicationContext(Context);
        }

    } else {
        Status = ArchFwInitializeFirmwareContext(Context);
    }

    return Status;
}

VOID
ArchSetInitialContext (
    __in PEXECUTION_CONTEXT Context
    )

/*++

Routine Description:

    This routine sets the current execution context to the one specified. On
    return, the application will be running in the new context.

Arguments:

    Context - Supplies a pointer to the execution context to be set.

Return Values:

    None.

--*/

{

    ArchSwitchContext(Context, NULL);
    return;
}

VOID
BlpArchInstallExceptionVectors (
    VOID (*VectorTable[])(VOID)
    )

/*++

Routine Description:

    This routine installs a new vector table. If no vector table is
    provided, install a stub vector table that loops in place.

Arguments:

    VectorTable - Supplies an optional pointer to vector table.

Returns:

    None.

--*/

{

    ULONG VectorAddress;

    //
    // ARM exceptions (including interrupts) result in an instruction
    // fetch from a vector table at a fixed physical or virtual
    // address (depending on whether translation is enabled).
    // Handlers are installed by copying a set of instructions and
    // vector table to the address.  This must be followed by ensuring
    // that the I/D caches are consistent.
    //
    // On cores without the TZ security extension, the virtual address
    // can be at 0x0 or 0xFFFF0000 depending on the V bit in the
    // system control register.  On cores with the security extension,
    // the virtual address can be altered via the CP15_VBAR register.
    //
    // This code relies on the presence of the security extension and
    // uses CP15_VBAR.
    //

    NT_ASSERT((_MoveFromCoprocessor (CP15_SCTLR) & CP15_SCTLR_V) == 0);
    NT_ASSERT((READ_ARM_FEATURE (CP15_PFR1, PFR1_SECURITY) >= PFR1_SECURITY_TZONE));

    //
    // If no vector table was specified, install the stub vectors
    // which loop in place.
    //

    if (VectorTable == NULL) {
        VectorTable = ArchStubExceptionVectors;
    }

    VectorAddress = ((ULONG_PTR)VectorTable) & ~1;

    //
    // Vector table must be 32 byte aligned.
    //

    NT_ASSERT((VectorAddress & (32 - 1)) == 0);

    _MoveToCoprocessor(VectorAddress, CP15_VBAR);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();
    return;
}

VOID
ArchArmBuildApplicationExceptionState (
    __out PARM_EXCEPTION_STATE ExceptionState
    )

/*++

Routine Description:

    This routine builds out the application exception state for
    non-bootmgr application state.

Arguments:

    ExceptionState - Supplies a pointer to the exception state to be
        initialized.

Returns:

    None.

--*/

{

    ULONG Value;

    //
    // Setup application state so that alignment fixups are active and
    // exceptions are in thumb mode.
    //

    Value = _MoveFromCoprocessor(CP15_SCTLR);
    Value &= ~(CP15_SCTLR_A | CP15_SCTLR_V);
    Value |= CP15_SCTLR_TE;
    ExceptionState->Control = Value;

    //
    // Set the ID register to NULL as it is not used in the application state.
    //

    ExceptionState->IdSvcRW = 0;

    //
    // Initial exception vector will be local to the launching
    // application..
    //

    ExceptionState->Vbar = ((ULONG_PTR) ArchStubExceptionVectors) & ~1;
    return;
}


ULONG
BlpArchArmCaptureInterruptState (
    VOID
    )

/*++

Routine Description:

    This routine captures the state of interrupts in the currently
    active context and preseves it to the provided storage.

Arguments:

    None.

Returns:

    1 if interrupts are enabled, 0, if interrupts are disabled.

--*/

{
    UINT32 Status;

    Status = _ReadStatusReg(0);

    //
    // N.B. CPSRC_INT and CPSRC_FIQ are interrupt *disable* bits, If
    //      CPSRC_INT is clear, then interrupts were enabled.  We
    //      don't test for FIQ as the bit may be protected by TrustZone.
    //

    if ((Status & CPSRC_INT) != 0) {
        return 0;
    } else {
        return 1;
    }
}

VOID
BlpArchArmCaptureExceptionState (
    __out PARM_EXCEPTION_STATE ExceptionState
    )

/*++

Routine Description:

    This routine captures the exception state in the in the currently
    active context and preseves it to the provided storage.

Arguments:

    ExceptionState - Supplies a pointer to a struct where the
        exception state will be captured.

Returns:

    None.

--*/

{

    //
    // Capture current exception state
    //

    ExceptionState->Control = _MoveFromCoprocessor(CP15_SCTLR);
    ExceptionState->Vbar = _MoveFromCoprocessor(CP15_VBAR);
    ExceptionState->IdSvcRW = _MoveFromCoprocessor(CP15_TPIDRPRW);
    return;
}

VOID
ArchSetExceptionState (
    __in PARM_EXCEPTION_STATE ExceptionState
    )
/*++

Routine Description:

    This routine retrieves the exception state stored in the provided
    structure and restores that state to the processor.  This function
    runs with interrupts disabled.

Arguments:

    ExceptionState - Supplies a pointer to a struct from which the
        exception state will be restored..

Returns:

    None.

--*/

{

    //
    // Restore the ID register in case the new state is using this
    // register for scratch space.
    //

    _MoveToCoprocessor(ExceptionState->IdSvcRW, CP15_TPIDRPRW);
    _DataSynchronizationBarrier();

    //
    // Switch to new exception state
    //

    _MoveToCoprocessor(ExceptionState->Control, CP15_SCTLR);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    //
    // Set active exception handlers
    //

    _MoveToCoprocessor(ExceptionState->Vbar, CP15_VBAR);
    _InstructionSynchronizationBarrier();
    return;
}

VOID
ArchSetPagingContext (
    __in PEXECUTION_CONTEXT NewContext
    )

/*++

Routine Description:

    This routine sets the page directory address for a boot application to the
    one specified in the new context. On return, all address translations will
    occur with respect to the new paging hierarchy.

Arguments:

    NewContext - Supplies a pointer to the new context that contains the new
        page directory address.

Return Value:

    None.

--*/

{
    BlpMmArmSwitchMmState(&NewContext->MmState);
    return;
}

DECLSPEC_NOINLINE
VOID
ArchSwitchContext (
    __in PEXECUTION_CONTEXT NewContext,
    __in_opt PEXECUTION_CONTEXT CurrentContext
    )

/*++

Routine Description:

     This routine effects a switch of an execution context from the current
     context to the specified one. On return, the application will be running in
     the new context.

Arguments:

    NewContext - Supplies a pointer to the new execution context to be set for
        the application.

    CurrentContext - Supplies a pointer to the current execution context for the
        application.

Return Value:

    None.

--*/

{
    ULONG Par1, Par2;
    NTSTATUS Status;

    //
    // If we know the current state, preserve state that may have been
    // altered.
    //

    if (ARGUMENT_PRESENT(CurrentContext)) {

#ifdef NOTYET

        CurrentContext->InterruptState = BlpArchArmCaptureInterruptState();

#endif

        BlpArchArmCaptureExceptionState(&CurrentContext->ExceptionState);
    }

    // When switching from an application with interrupts enabled to another
    // application, disable interrupts before the paging context changes.
    //

    BlpArchDisableInterrupts();

    //
    // Switch the paging context and exception context.
    //

    Status = BlMmArmGetNativeAttributes((ULONG_PTR)NewContext, &Par1);

    NT_ASSERT(NT_SUCCESS(Status));

    ArchSetPagingContext(NewContext);

    Status = BlMmArmGetNativeAttributes((ULONG_PTR)NewContext, &Par2);

    NT_ASSERT(NT_SUCCESS(Status));
    NT_ASSERT(Par1 == Par2);

    ArchSetExceptionState(&NewContext->ExceptionState);

    //
    // When switching from an application to an application with interrupts
    // enabled, enable interrupts after the paging context has been changed.
    //

    if (NewContext->InterruptState != 0) {
        BlpArchEnableInterrupts();
    }

    return;
}

DECLSPEC_NOINLINE
NTSTATUS
BlpArchBuildApplicationContext (
    __out PEXECUTION_CONTEXT ApplicationContext,
    __in ULONG Flags,
    __out PULONG SelfMapPdeBase
    )

/*++

Routine Description:

    This routine builds the application execution context including
    the exception and MM state.  As part of the transition to the new
    appllication, this context will be made active.  The new
    application will capture the resulting state as it's application context.

Arguments:

    ApplicationContext - Supplies a pointer to the application context to be
        build.

    Flags - Supplies a set of flags that dictate how the page tables are
        created.

    SelfMapPdeBase - Supplies a pointer to a variable the receives the location
        of the self map PDE base.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{

    NTSTATUS Status;

    //
    // Create page tables that will be used by the new boot application.
    //

    Status = BlpMmArmBuildApplicationMmState(&ApplicationContext->MmState,
                                             Flags,
                                             SelfMapPdeBase);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    ArchArmBuildApplicationExceptionState(&ApplicationContext->ExceptionState);

    //
    // Boot applications run with the interrupts disabled when in the
    // application's context.
    //

    ApplicationContext->AddressTranslationEnabled = 1;
    ApplicationContext->InterruptState = 0;
    ApplicationContext->ContextType = ExecutionContextApplication;
    return STATUS_SUCCESS;
}

NTSTATUS
BlpArchDestroyApplicationContext (
    __inout PEXECUTION_CONTEXT ApplicationContext
    )

/*++

Routine Description:

    This routine destroys the application execution context including
    the MM state.

Arguments:

    ApplicationContext - Supplies a pointer to the application context to be
        destroyed.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    //
    // Free the application MM state.
    //

    Status = BlpMmArmFreeApplicationMmState(&ApplicationContext->MmState);
    return Status;
}

VOID
BlArchKernelSetup (
    VOID
    )
/*++

Routine Description:

    This routine sets up the kernel execution environment.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PEXECUTION_CONTEXT ApplicationContext;

    ApplicationContext = ArchGetContext(ExecutionContextApplication);
    BlpMmArmKernelSetup(&ApplicationContext->MmState);
    return;
}

PARM_MM_STATE
BlpArchGetMmState (
    __in EXECUTION_CONTEXT_TYPE ContextType
    )

/*++

Routine Description:

    This routine returns the MM States of the supplied context type.

Arguments:

    ContextType - Supplies the context type for which the caller is requesting
        the page directory address.

Return Value:

    A pointer to the ARM_MM_STATE associated with the specified context

--*/

{

    PEXECUTION_CONTEXT Context;

    BlpInterfaceEnter(InterfaceArch);
    Context = ArchGetContext(ContextType);
    BlpInterfaceExit(InterfaceArch);
    return &Context->MmState;
}

BOOLEAN
BlpArchIsTranslationEnabled (
    VOID
    )

/*++

Routine Description:

    This routine determines if the boot application's current context has
    address translation enabled.

    N.B. This routine cannot call BlpInterfaceEnter or BlpInterfaceExit.
         These routines can call out to this function, so calling back into
         them would trigger infinite recursion.

Arguments:

    None.

Return Value:

    TRUE if address translation is enabled. FALSE otherwise.

--*/

{

    PEXECUTION_CONTEXT Context;

    Context = ArchGetCurrentContext();
    if ((Context != NULL) &&
        (Context->AddressTranslationEnabled != 0)) {

        return TRUE;
    }

    return FALSE;
}

