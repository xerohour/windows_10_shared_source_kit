/*++

Copyright (c) Microsoft Corporation

Module Name:

    detect.c

Abstract:

    This module contains code for detecting the appropriate kernel and hal
    type for an ARM system.

Environment:

    Boot.

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include "acpitabl.h"

// ---------------------------------------------------------------- Definitions

#define ARM_KERNEL_NAME L"ntoskrnl.exe"
#define ARM_HAL_NAME L"hal.dll"

// ------------------------------------------------------------------ Functions

NTSTATUS
OslArchDetectKernelHal (
    __inout PUNICODE_STRING KernelName,
    __inout PUNICODE_STRING HalName
    )

/*++

Routine Description:

    Determines the kernal and hal most appropriate for the executing
    platform.

    Memory will be allocated to hold the strings for the kernel and hal
    filename.  The caller is responsible for freeing the memory.

Arguments:

    KernelName - The name of the kernel to load.

    HalName - The name of the hal to load.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.

--*/

{

    BOOLEAN BoolRet;

    BoolRet = BlAppendUnicodeToString(KernelName, ARM_KERNEL_NAME);
    if (BoolRet == FALSE) {
         return STATUS_NO_MEMORY;
    }

    BoolRet = BlAppendUnicodeToString(HalName, ARM_HAL_NAME);
    if (BoolRet == FALSE) {
        return STATUS_NO_MEMORY;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslArchDetectUpdateLibrary (
    __inout PUNICODE_STRING UpdateLibrary
    )

/*++

Routine Description:

    Determines the most appropriate microcode update library for the executing
    platform.  This module does not check for the existence of the update
    library.  This check is left to the caller to perform.

    Memory will be allocated to hold the string for the microcode update library
    filename.  The caller is responsible for freeing the memory.

Arguments:

    UpdateLibrary - Supplies a UNICODE_STRING that will be used to return the
        name of the update library to load.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.
    STATUS_NOT_SUPPORTED if CPUID is not supported on this CPU.

--*/

{
    UNREFERENCED_PARAMETER(UpdateLibrary);

    return STATUS_NOT_SUPPORTED;
}

