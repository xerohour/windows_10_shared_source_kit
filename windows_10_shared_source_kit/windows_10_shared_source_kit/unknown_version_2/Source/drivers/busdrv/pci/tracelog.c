/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    tracelog.c

Abstract:

    This module implements tracelogging support for PCI.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, PciTraceLogInitialize)
    #pragma alloc_text(PAGE, PciTraceLogUninitialize)
#endif

// --------------------------------------------------------------------- Globals

TRACELOGGING_DEFINE_PROVIDER(PciTraceLoggingProvider,
    "PciTraceLoggingProvider",
    (0x69a770dd, 0x1cdb, 0x46c0, 0x91, 0xc9, 0xcd, 0x2a, 0x9b, 0x76, 0xe0, 0x61),
    TraceLoggingOptionMicrosoftTelemetry());

// ------------------------------------------------------------------- Functions

NTSTATUS
PciTraceLogInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes the tracelog provider for PCI.
        
Arguments:

    None.
    
Return Value:

    NTSTATUS value indicating successful tracelog initialization.

--*/

{

    PAGED_CODE();

    TraceLoggingRegister(PciTraceLoggingProvider);

    TraceLoggingWrite(
        PciTraceLoggingProvider, 
        "PciTraceLogInitialized",
        TraceLoggingString("PCI TraceLogging Enabled!", "Yes"),
        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );

    return STATUS_SUCCESS;
}

NTSTATUS
PciTraceLogUninitialize (
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

    PAGED_CODE();

    TraceLoggingWrite(
        PciTraceLoggingProvider,
        "PciTraceLogUnInitialized",
        TraceLoggingString("PCI TraceLogging Disabled!"),
        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );

    TraceLoggingUnregister(PciTraceLoggingProvider);
    return STATUS_SUCCESS;
}
