/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    status.c

Abstract:

    This module implements Boot Environment Library API for reporting errors
    and status.

Environment:

    Boot.

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ---------------------------------------------------------------- Definitions

#define LOCAL_SCRATCH_BUFFER_SIZE 1024

// --------------------------------------------------------------------- Locals

static PBL_STATUS_ERROR_FUNCTION BlpStatusErrorHandler;
static BOOLEAN BlpStatusErrorInProgress;

// ------------------------------------------------------------------------ API

VOID
BlpStatusError (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4
    );

VOID
BlStatusPrint (
    __in PCWSTR Format,
    ...
    )

/*++

Routine Description:

    Printf style string routine for reporting status to the Boot Environment
    Library.

    N.B. This function is used to report heap corruptions.  This implies that
         any memory allocation attempt could result in a recursive call back
         to this routine if the heap is corrupt.  In order to avoid infinite
         recursion this routine must not attempt any heap allocations.

Arguments:

    printf style arguments.

Returns:

    None.

--*/

{

    static CHAR AnsiBuffer[LOCAL_SCRATCH_BUFFER_SIZE];
    ANSI_STRING AnsiString;
    va_list ArgList;
    LONG ReturnValue;
    ULONG Size;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    //
    // If the debugger is connected, print the formatted string to the
    // debugger.
    //

    if (BlBdDebuggerEnabled() != FALSE) {

        //
        // Use the global scratch buffer to initialize a UNICODE_STRING using
        // the caller's arguments.
        //

        ASSERT((LOCAL_SCRATCH_BUFFER_SIZE * sizeof(WCHAR)) <
               GLOBAL_SCRATCH_BUFFER_SIZE);

        va_start(ArgList, Format);
        Size = LOCAL_SCRATCH_BUFFER_SIZE - 1;
        ReturnValue = vswprintf_s((PWCHAR)BlScratchBuffer,
                                  Size,
                                  Format,
                                  ArgList);

        va_end(ArgList);
        if (ReturnValue > 0) {

            //
            // Initialize an ANSI string to pass to the debugger API.
            //

            RtlInitUnicodeString(&UnicodeString, (PWCHAR)BlScratchBuffer);
            AnsiString.Length = 0;
            AnsiString.MaximumLength = LOCAL_SCRATCH_BUFFER_SIZE;
            AnsiString.Buffer = AnsiBuffer;
            Status = RtlUnicodeStringToAnsiString(&AnsiString,
                                                  &UnicodeString,
                                                  FALSE);

            if (NT_SUCCESS(Status)) {
                BlBdPrint(AnsiString.Buffer);
            }
        }
    }

    return;
}

VOID
BlStatusError (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4
    )

/*++

Routine Description:

    Boot Environment Library interface for reporting an error (application
    or library).

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorParameter1 - Additional information for the error.

    ErrorParameter2 - Additional information for the error.

    ErrorParameter3 - Additional information for the error.

    ErrorParameter4 - Additional information for the error.

Returns:

    None.

--*/

{
    NTSTATUS Status;

    Status = STATUS_UNSUCCESSFUL;

    //
    // Report library errors to registered error handler, if registered.
    //

    if (ErrorCode != BL_ERROR_APPLICATION &&
        BlpStatusErrorInProgress == FALSE &&
        BlpStatusErrorHandler != NULL) {

        //
        // Set Error In Progress flag to protect against reentrency and
        // recursive calls as boot application error handlers often call
        // BlStatusError.
        //

        BlpStatusErrorInProgress = TRUE;
        Status = BlpStatusErrorHandler(ErrorCode,
                                       ErrorParameter1,
                                       ErrorParameter2,
                                       ErrorParameter3,
                                       ErrorParameter4);
        BlpStatusErrorInProgress = FALSE;
    }

    //
    // Call default error method if error handler wasn't called, or was
    // error handler was successful.
    //

    if (!NT_SUCCESS(Status)) {
        BlpStatusError(ErrorCode,
                       ErrorParameter1,
                       ErrorParameter2,
                       ErrorParameter3,
                       ErrorParameter4);
    }

    return;
}

VOID
BlStatusRegisterErrorHandler(
    __in PBL_STATUS_ERROR_FUNCTION Callback
    )

/*++

Routine Description:

    Boot Environment Library interface for registering a callback that is
    invoked when reporting a library error.

Arguments:

    Callback - Supplies a pointer to the callback function.
    
Returns:

    None.

--*/

{

    BlpStatusErrorHandler = Callback;
    return;
}

VOID
BlStatusUnregisterErrorHandler(
    )

/*++

Routine Description:

    Boot Environment Library interface for unregistering the callback that is
    invoked when reporting a library error.

Arguments:

    None.
    
Returns:

    None.

--*/

{

    BlpStatusErrorHandler = NULL;
    return;
}

VOID
BlpStatusError (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4
    )

/*++

Routine Description:

    Boot Environment Library interface for reporting an error (application
    or library).

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorParameter1 - Additional information for the error.

    ErrorParameter2 - Additional information for the error.

    ErrorParameter3 - Additional information for the error.

    ErrorParameter4 - Additional information for the error.

Returns:

    None.

--*/

{

    //
    // The implementation details of this function need to be resolved. In the
    // shorterm, break into the debugger with the error parameters.
    //

    if (BlBdDebuggerEnabled() != FALSE) {
        BlStatusPrint(L"\n*** Fatal Error 0x%08x :\n"
                      L"                (0x%p, 0x%p, 0x%p, 0x%p)\n\n",
                      ErrorCode,
                      ErrorParameter1,
                      ErrorParameter2,
                      ErrorParameter3,
                      ErrorParameter4);

        DbgBreakPoint();
    }

    return;
}

