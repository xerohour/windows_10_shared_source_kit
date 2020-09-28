/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ctxefiarm.c

Abstract:

    This module implements context manipulation routines for UEFI firmware.

    N.B. This is only compiled on ARM/ARM64 builds.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
ArchFwInitializeFirmwareContext (
    __out PEXECUTION_CONTEXT FirmwareContext
    )

/*++

Routine Description:

    This routine initializes the firmware execution context. It sets the page
    directory address, the address translation state, and the interrupt state
    for the firmware context.

Arguments:

    FirmwareContext - Supplies a pointer to the firmware context to be
        initialized.

Return Value:

    NT Status Code.

--*/

{

    FIRMWARE_PARAMETERS UNALIGNED FirmwareBuffer;
    PFIRMWARE_PARAMETERS FirmwareParameters;
    NTSTATUS Status;

    //
    // Get the architecture parameters that were passed by the previous boot
    // application.
    //

    Status = BlFwGetParameters(&FirmwareBuffer);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    FirmwareParameters = &FirmwareBuffer;

    //
    // The firmware's page tables as captured by the MM_STATE are used
    // when running in the firmware context.
    //

    FirmwareContext->MmState = FirmwareParameters->MmState;

    //
    // Address translation is always disabled when running in the firmware
    // execution context. In the ARM case, this means that the application is
    // running with the firmware's page tables and that everything is 1:1
    // mapped.
    //

    FirmwareContext->AddressTranslationEnabled = 0;

    //
    // Use firmware's exception state
    //

    FirmwareContext->ExceptionState = FirmwareParameters->ExceptionState;

    //
    // The firmware context runs with interrupts enabled.
    //

    FirmwareContext->InterruptState = FirmwareParameters->InterruptState;
    return STATUS_SUCCESS;
}

