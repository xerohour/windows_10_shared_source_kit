/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    acpi.h

Abstract:

   Contains declarations for ACPI support functions

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/
#ifndef _ACPI_H_
#define _ACPI_H_

//
// Public functions
//

EXTERN_C
NTSTATUS
AcpiInitialize(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    );


//
// Private functions
//

EXTERN_C
NTSTATUS
_AcpiGetDeviceInformation(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    );

EXTERN_C
NTSTATUS
_AcpiGetDeviceMethod(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    );

EXTERN_C
NTSTATUS
_AcpiDsmSupported(
        _In_ PDEVICE_CONTEXT    FxDeviceContext,
        _In_ ULONG              FunctionIndex,
        _Out_ PBOOLEAN          Supported
        );

EXTERN_C
VOID
_AcpiPrepareInputParametersForDsm (
    _Inout_ PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer,
    _In_ ULONG ParametersBufferSize,
    _In_ ULONG FunctionIndex
    );

#endif
