/*++

Copyright (c) Microsoft Corporation

Module Name:

    entry.c

Abstract:

    This module contains the entry point for the Windows Boot Manager on
    an EFI platform.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "bootmgr.h"

// ------------------------------------------------------------------ Functions

EFI_STATUS
EFIAPI
EfiEntry (
    __in EFI_HANDLE ImageHandle,
    __in struct _EFI_SYSTEM_TABLE *SystemTable
    )

/*++

Routine Description:

    The entry point for the Windows Boot Manager on an EFI platform.

Arguments:

    ImageHandle - Firmware allocated handle for the Boot Manager's image.

    SystemTable - A pointer to the EFI system table.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER when unable to initialize boot application
        input data structure.

    Any other error is possible when the Boot Manager fails.

--*/

{

    PBOOT_APPLICATION_PARAMETERS InputParameters;
    NTSTATUS Status;

    //
    // All boot environment applications have a common input data structure.
    // Since this application was loaded as an EFI Application, we need
    // to create our expected input data structure.
    //

    InputParameters = EfiInitCreateInputParameters(ImageHandle, SystemTable);
    if (InputParameters == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto EntryEnd;
    }

    //
    // Transfer execution to the boot application's firmware independent
    // entry point.
    //

    Status = BmMain(InputParameters);

EntryEnd:
    return EfiGetEfiStatusCode(Status);
}

