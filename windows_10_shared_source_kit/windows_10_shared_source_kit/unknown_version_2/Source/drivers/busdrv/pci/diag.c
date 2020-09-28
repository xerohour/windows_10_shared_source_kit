/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    diag.c

Abstract:

    This module implements  diagnostic tracing for PCI.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <systemevents.h>
#include <evntrace.h>

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas

VOID
PciDiagTraceControlCallback (
    __in LPCGUID SourceId,
    __in ULONG ControlCode,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    );

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, PciDiagInitialize)
    #pragma alloc_text(PAGE, PciDiagUninitialize)
    #pragma alloc_text(PAGE, PciDiagTraceControlCallback)
#endif

// --------------------------------------------------------------------- Globals

REGHANDLE PciDiagHandle;

// ------------------------------------------------------------------- Functions

NTSTATUS
PciDiagInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes the ETW trace provider for PCI.
        
Arguments:

    None.
    
Return Value:

    NTSTATUS value indicating successful diagnostic initialization.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    //
    // Register the PCI provider guid with ETW.
    //

    Status = EtwRegister(&PCI_ETW_PROVIDER,
                         PciDiagTraceControlCallback,
                         NULL,
                         &PciDiagHandle);

    if (!NT_SUCCESS(Status)) {

        //
        // This isn't a fatal error, events just won't be traced.
        //

        Status = STATUS_SUCCESS;
        goto Exit;
    }

    Status = STATUS_SUCCESS;

Exit:

    return Status;
}

NTSTATUS
PciDiagUninitialize (
    VOID
    )

/*++

Routine Description:

    This routine uninitializes the ETW trace provider for PCI.
        
Arguments:

    None.
    
Return Value:

    NTSTATUS value indicating successful diagnostic uninitialization.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    Status = EtwUnregister(PciDiagHandle);
    return Status;
}


VOID
PciDiagTraceControlCallback (
    __in LPCGUID SourceId,
    __in ULONG ControlCode,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    This routine is called when a provider enables a trace, disables a trace,
    or requests a rundown.
    
Arguments:

    SourceId - Supplies the GUID from the call to EnableTraceEx.

    ControlCode - Supplies the control code from the call to EnableTraceEx.

    Level - Supplies the level enabled for the tracing.

    MatchAnyKeyword - Supplies information for filtering trace information.

    MatchAllKeyword - Supplies information for fitlering trace information.

    FilterData - Supplies information for filtering trace information.

    CallbackContext - Supplies the context provided to EtwRegister.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(Level);
    UNREFERENCED_PARAMETER(MatchAnyKeyword);
    UNREFERENCED_PARAMETER(MatchAllKeyword);
    UNREFERENCED_PARAMETER(FilterData);
    UNREFERENCED_PARAMETER(CallbackContext);

    //
    // Only respond to capture state control codes.
    //

    if (ControlCode != EVENT_CONTROL_CODE_CAPTURE_STATE) {
        return;
    }

    if (PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_ASPM) {
        EtwWrite(PciDiagHandle,
                 &PCI_ETW_EVENT_ASPM_ERRATA_RUNDOWN,
                 NULL,
                 0,
                 NULL);

    }

    return;
}


