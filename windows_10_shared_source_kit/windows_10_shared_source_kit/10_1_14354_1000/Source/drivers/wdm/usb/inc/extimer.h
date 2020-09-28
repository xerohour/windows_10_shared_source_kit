/*++

Copyright (c) 2013      Microsoft Corporation

Module Name:

        extimer.h

Abstract:

   Typedefs of the EX_TIMER routines' types.

Environment:

    Kernel mode

Revision History:

    01-25-2013 : created

--*/

#pragma once

typedef
PEX_TIMER
(*PFN_EX_ALLOCATE_TIMER) (
    _In_ PEXT_CALLBACK Callback,
    _In_opt_ PVOID CallbackContext,
    _In_ ULONG Attributes
    );

typedef
BOOLEAN
(*PFN_EX_SET_TIMER) (
    _In_ PEX_TIMER Timer,
    _In_ LONGLONG DueTime,
    _In_ LONGLONG Period,
    _In_opt_ PEXT_SET_PARAMETERS Parameters
    );

typedef
BOOLEAN
(*PFN_EX_CANCEL_TIMER) (
    _Inout_ PEX_TIMER Timer,
    _In_opt_ PEXT_CANCEL_PARAMETERS Parameters
    );

typedef
BOOLEAN
(*PFN_EX_DELETE_TIMER) (
    _In_ PEX_TIMER Timer,
    _In_ BOOLEAN Cancel,
    _In_ BOOLEAN Wait,
    _In_opt_ PEXT_DELETE_PARAMETERS Parameters
    );
