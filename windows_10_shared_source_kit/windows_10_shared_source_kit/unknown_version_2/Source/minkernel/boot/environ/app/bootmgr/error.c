/*++

Copyright (c) Microsoft Corporation

Module Name:

    error.c

Abstract:

    Provides error support functions for the boot manager.

Revision History:

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"

//
// -------------------------------------------------- Local Function Prototypes
//

VOID
BmpErrorLog (
    __in ULONG ErrorCode,
    __in NTSTATUS ErrorStatus,
    __in ULONG ErrorMsgId,
    __in_opt PWCHAR FileName,
    __in BM_ERROR_HELP_TYPE HelpType,
    __in_opt PCHAR SourceFile,
    __in_opt ULONG SourceLine
    );

BM_ERROR_HELP_TYPE
BmpFatalErrorMessageFilter (
    __in NTSTATUS ErrorStatus,
    __inout PULONG ErrorMsgId
    );

VOID
BmpHandleInvalidBcdEntryEvent (
    __in NOTIFICATION_EVENT_TYPE EventType,
    __in_opt PNOTIFICATION_EVENT_CONTEXT EventContext,
    __in_opt PVOID HandlerContext1,
    __in_opt PVOID HandlerContext2
    );

VOID
BmpLogBootappLoadFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    );

VOID
BmpLogBcdFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    );

VOID
BmpLogInvalidBootEntryFailureEvent (
    __in PWSTR ObjectName,
    __in_opt PWSTR Description,
    __in_opt PWSTR ElementName,
    __in NTSTATUS ErrorStatus
    );

VOID
BmpLogNoValidOsEntryFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    );

VOID
BmpLogGeneralFailureEvent (
    __in NTSTATUS Status
    );

//
// --------------------------------------------------------------------- Locals
//

#define FILE_NAME_BUFFER_LENGTH 256

WCHAR BmpFileNameBuffer[FILE_NAME_BUFFER_LENGTH];
static GENERIC_BUFFER BmpPackedBootError;
static PINTERNAL_BOOT_ERROR BmpInternalBootError = NULL;
static INTERNAL_BOOT_ERROR BmpErrorBuffer =
    { 0, 0, 0, NULL, NULL, BmErrorHelpDefault, NULL };

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BmErrorInitialize (
    VOID
    )

/*++

Routine Description:

    This routine performs any initialization required for handling errors
    encountered during the application's execution.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    Status = BlEnRegisterEventHandler(EVENT_BCD_OBJECT_CONFIGURATION_ERROR,
                                      BmpHandleInvalidBcdEntryEvent,
                                      NULL,
                                      NULL);

    return Status;
}

VOID
BmFatalErrorEx (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4,
    __in_opt PCHAR SourceFile,
    __in_opt ULONG SourceLine
    )

/*++

Routine Description:

    Boot Manager routine for processing internal application errors.
    Currently, this routine is a pretty print wrapper for BlStatusError.
    It could additionally be used to present a user interface for the
    specified error.

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorParameter1 - Additional information for the error.

    ErrorParameter2 - Additional information for the error.

    ErrorParameter3 - Additional information for the error.

    ErrorParameter4 - Additional information for the error.

    SourceFile - Source file in which error occurred, #if DBG only.

    SourceLine - Line in source file, where error occurred, #if DBG only.

Returns:

    None.

--*/

{

    BOOLEAN CreateLog;
    WCHAR DescriptionString[256];
    ULONG ErrorMsgId;
    PWCHAR FileName;
    BM_ERROR_HELP_TYPE HelpType;
    BOOLEAN NoErrorDisplay;
    BOOLEAN RestartOnFailure;
    NTSTATUS Status;
    NTSTATUS Status1;
    PWCHAR WString1;
    PWCHAR WString2;
    PWCHAR WString3;

    //
    // Format a descriptive string for the error for the purposes of reporting
    // the error to the library so it can be output to the debugger.  Since this
    // is only for internal debugging purposes, the strings are not localized.
    //
    // Additionally, pick out the localizable message ID that should be used to
    // display the error on the screen.  If the error is associated with a
    // specific file, save this as well.
    //

    CreateLog = TRUE;
    FileName = NULL;
    switch (ErrorCode) {

        //
        // Boot application load failure case.  Parameters:
        //      ErrorParameter1 - Optional name of the boot application.
        //      ErrorParameter2 - Failure status code.
        //

    case BM_ERROR_BOOTAPP_LOAD_FAILURE:
        WString1 = (PWCHAR)ErrorParameter1;
        if ((WString1 == NULL) ||
            (wcslen(WString1) >= FILE_NAME_BUFFER_LENGTH))  {

            WString1 = L"";

        } else {
            wcscpy_s(BmpFileNameBuffer,
                     FILE_NAME_BUFFER_LENGTH / sizeof(WCHAR),
                     WString1);

            WString1 = BmpFileNameBuffer;
        }

        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nAn error occurred (%08x) while attempting to load "
                   L"the boot application %s\n",
                   Status1,
                   WString1);

        FileName = WString1;
        ErrorMsgId = BM_MSG_BOOTAPP_ERROR;
        BmpLogBootappLoadFailureEvent(Status1, WString1);
        break;

        //
        // Boot configuration data file access error case.  Parameters:
        //      ErrorParameter1 - Name of the boot configuration data file.
        //      ErrorParameter2 - Failure status code.
        //

    case BM_ERROR_CONFIGURATION_DATA:
        WString1 = (PWCHAR)ErrorParameter1;
        if ((WString1 == NULL) ||
            (wcslen(WString1) >= FILE_NAME_BUFFER_LENGTH))  {

            WString1 = L"";

        } else {
            wcscpy_s(BmpFileNameBuffer,
                     FILE_NAME_BUFFER_LENGTH / sizeof(WCHAR),
                     WString1);

            WString1 = BmpFileNameBuffer;
        }

        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nAn error occurred (%08x) while attempting to read "
                   L"the boot configuration data file %s\n",
                   Status1,
                   WString1);

        FileName = WString1;
        ErrorMsgId = BM_MSG_BOOT_CONFIGURATION_DATA_ERROR;
        BmpLogBcdFailureEvent(Status1, WString1);
        break;

        //
        // Boot configuration does not contain a valid entry case.  Parameters:
        //      ErrorParameter1 - Name of the boot configuration data file.
        //      ErrorParameter2 - Failure status code.
        //

    case BM_ERROR_NO_VALID_OS_ENTRY:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nNo valid entries found in the boot configuration "
                   L"data file %s\n",
                   WString1);

        FileName = WString1;
        ErrorMsgId = BM_MSG_NO_VALID_OS_ENTRY_ERROR;
        BmpLogNoValidOsEntryFailureEvent(Status1, WString1);
        break;

        //
        // Boot configuration does not contain a valid entry case, customized
        // for a PXE boot scenario.  Parameters:
        //      ErrorParameter1 - Name of the boot configuration data file.
        //      ErrorParameter2 - Failure status code.
        //

    case BM_ERROR_NO_VALID_PXE_OS_ENTRY:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nNo valid entries found in the boot configuration "
                   L"data file %s from the PXE server\n",
                   WString1);

        FileName = WString1;
        ErrorMsgId = BM_MSG_NO_VALID_PXE_OS_ENTRY_ERROR;
        BmpLogNoValidOsEntryFailureEvent(Status1, WString1);
        break;

        //
        // Failure status case.  In this case a simple status code is the only
        // error parameter.
        //

    case BM_ERROR_FAILURE_STATUS:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot manager experienced an error (%08x).\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = BM_MSG_UNKNOWN_ERROR;
        BmpLogGeneralFailureEvent(Status1);
        break;

        //
        // General boot device failure case. A simple status code is the only
        // error parameter.
        //

    case BM_ERROR_BOOT_DEVICE_FAILURE:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot selection failed because a required device "
                   L"is inaccessible (%08x).\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = BM_MSG_BOOT_DEVICE_ERROR;
        BmpLogGeneralFailureEvent(Status1);
        break;

        //
        // Ramdisk device failure case. A simple status code is the only
        // error parameter.
        //

    case BM_ERROR_RAMDISK_MEMORY_FAILURE:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nRamdisk device creation failed due to insufficient "
                   L"memory (%08x).\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = BM_MSG_RAMDISK_MEMORY_ERROR;
        BmpLogGeneralFailureEvent(Status1);
        break;

        //
        // Invalid boot configuration data store case.  Parameters:
        //      ErrorParameter1 - Name of the boot configuration data file.
        //      ErrorParameter2 - Failure status code.
        //

    case BM_ERROR_INVALID_BCD_STORE:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot configuration file %s is invalid (%08x).\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = BM_MSG_BCD_INVALID_ERROR;
        BmpLogBcdFailureEvent(Status1, WString1);
        break;

    case BM_ERROR_INVALID_BCD_ENTRY:
        WString1 = (PWCHAR)ErrorParameter1;
        WString2 = (PWCHAR)ErrorParameter2;
        WString3 = (PWCHAR)ErrorParameter3;
        Status1 = (NTSTATUS)ErrorParameter4;
        if ((WString1 == NULL) || (WString2 == NULL) || (WString3 == NULL)) {
            swprintf_s(DescriptionString,
                       (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                        L"\n The boot manager experienced error (%08x) due to "
                        L"an invalid entry in the BCD store.",
                       Status1);

        } else {
            swprintf_s(DescriptionString,
                       (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                       L"\nThe boot manager experienced an error (%08x) "
                       L"parsing element (%ws) within BCD object %s.\n",
                       Status1,
                       WString3,
                       WString1);
        }

        ErrorMsgId = BM_MSG_BCD_INVALID_BCD_ENTRY_ERROR;
        BmpLogInvalidBootEntryFailureEvent(WString1,
                                           WString2,
                                           WString3,
                                           Status1);

        break;

    case BM_ERROR_NO_SECUREBOOT_POLICY:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot manager experienced an error (%08x) due to "
                   L"the default Secure Boot policy not being found.\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = BM_MSG_UNKNOWN_ERROR;
        BmpLogGeneralFailureEvent(Status1);
        break;

    case BM_ERROR_NO_PAE_SUPPORT:
        Status1 = (NTSTATUS)ErrorParameter1;
        FileName = NULL;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot manager experienced an error (%08x) due to "
                   L"the cpu not supporting the PAE feature.\n",
                   Status1);
        ErrorMsgId = BM_MSG_NO_PAE_SUPPORT;
        BmpLogGeneralFailureEvent(Status1);
        break;

    case BM_ERROR_UNSEALING_NOT_POSSIBLE:
        Status1 = (NTSTATUS)ErrorParameter1;
        FileName = NULL;
        swprintf_s(DescriptionString,
                   (sizeof(DescriptionString) / sizeof(WCHAR)) - 1,
                   L"\nThe boot manager experienced an error (%08x) due to "
                   L"the environment being in a state where the requested boot "
                   L"application cannot unseal sensitive data.\n",
                   Status1);
        ErrorMsgId = BM_ERROR_ENVIRONMENT_CANT_UNSEAL;
        BmpLogGeneralFailureEvent(Status1);
        break;

        //
        // Default case.  Pass the parameters on to the library without
        // a description string.
        //

    default:
        Status1 = STATUS_UNSUCCESSFUL;
        DescriptionString[0] = L'\0';
        FileName = NULL;
        ErrorMsgId = BM_MSG_UNKNOWN_ERROR;
        BmpLogGeneralFailureEvent(Status1);
        break;
    }

    //
    // Report the error to the Boot Environment Library so it can report the
    // error in the boot debugger.  Do not perform this operation if the user
    // requested that boot manager errors be suppressed. Restart on failure
    // option implies no error display.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_RESTART_ON_FAILURE,
                                           &RestartOnFailure);

    if (!NT_SUCCESS(Status)) {
        RestartOnFailure = FALSE;
    }

    if (RestartOnFailure != FALSE) {
        NoErrorDisplay = TRUE;

    } else {
        Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_NO_ERROR_DISPLAY,
                                               &NoErrorDisplay);

        if (!NT_SUCCESS(Status)) {
            NoErrorDisplay = FALSE;
        }
    }

    if (NoErrorDisplay == FALSE) {
        BlStatusPrint(DescriptionString);
        BlStatusError(BL_ERROR_APPLICATION,
                      (ULONG_PTR)ErrorCode,
                      ErrorParameter1,
                      ErrorParameter2,
                      ErrorParameter3);
    }

    //
    // Create an application-specific error log. This will allow the main
    // display loop to display the fatal error. Filter the error status to
    // update specific messages and help.
    //

    if (CreateLog != FALSE) {
        HelpType = BmpFatalErrorMessageFilter(Status1, &ErrorMsgId);
        BmpErrorLog(ErrorCode,
                    Status1,
                    ErrorMsgId,
                    FileName,
                    HelpType,
                    SourceFile,
                    SourceLine);
    }

    return;
}

VOID
BmErrorPurge (
    VOID
    )

/*++

Routine Description:

    This routine resets the local error state so that new events are
    captured, and also, if a PACKED_BOOT_ERROR record was previously
    mapped via BmErrorRestore, then this allocation is unmapped, and
    freed as well.

Arguments:

    None

Return Value:

    None

--*/

{

    //
    // Unmap any packed error info returned from another app.
    //

    if (BmpPackedBootError.Buffer != NULL) {
        BlMmFreeHeap(BmpPackedBootError.Buffer);
        BmpPackedBootError.Buffer = NULL;
    }

    BmpPackedBootError.BufferSize = 0;

    //
    // Reset our internal error state.
    //

    BmpInternalBootError = NULL;
    RtlZeroMemory(&BmpErrorBuffer, sizeof(INTERNAL_BOOT_ERROR));
    return;
}

PINTERNAL_BOOT_ERROR
BmErrorGetLast (
    VOID
    )

/*++

Routine Description:

    This routine returns a pointer to an INTERNAL_BOOT_ERROR, or NULL.

Arguments:

    None

Return Value:

    Pointer to an internal error buffer, or NULL.

--*/

{

    return BmpInternalBootError;
}

NTSTATUS
BmErrorGetData (
    VOID
    )

/*++

Routine Description:

    This routine calls into the libarary to retrieve error information,
    and if it exists, unpacks it into an internal boot error which can be
    retrieved via BmErrorGetLast.

Arguments:

    None

Return Value:

    STATUS_SUCCESS when successful, otherwise an appropriate error status
        if a failure occurs.

--*/

{

    GUID BaErrorGUID = BA_ERROR_GUID;
    ULONG Index;
    PPACKED_BOOT_ERROR PackedBootError;
    NTSTATUS Status;

    //
    // Purge any stale error information.
    //

    if (BmpInternalBootError != NULL) {
        BmErrorPurge();
    }

    //
    // Find the most recent (last) instance of error information, and determine
    // its storage requirement.
    //

    Index = 0;
    do {
        BlInitGenericBuffer(&BmpPackedBootError, NULL, 0);
        Status = BlPdQueryData(NULL, &BaErrorGUID, Index, &BmpPackedBootError);
        Index += 1;
    } while (Status == STATUS_BUFFER_TOO_SMALL);

    if (Index < 2) {
        Status = STATUS_NOT_FOUND;
        goto BmErrorRestoreEnd;
    }

    Index -= 2;
    BlInitGenericBuffer(&BmpPackedBootError, NULL, 0);
    Status = BlPdQueryData(NULL, &BaErrorGUID, Index, &BmpPackedBootError);
    if (Status != STATUS_BUFFER_TOO_SMALL) {
        Status = STATUS_UNSUCCESSFUL;
        goto BmErrorRestoreEnd;
    }

    //
    // Error information exists, allocate storage.
    //

    BmpPackedBootError.Buffer =
        BlMmAllocateHeap(BmpPackedBootError.BufferSize);

    if (BmpPackedBootError.Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BmErrorRestoreEnd;
    }

    //
    // Retrieve the packed error information.
    //

    Status = BlPdQueryData(NULL, &BaErrorGUID, Index, &BmpPackedBootError);
    if (!NT_SUCCESS(Status)) {
        goto BmErrorRestoreEnd;
    }

    //
    // Version 0 is not a valid version of the log.  Skip it.
    //

    PackedBootError = BmpPackedBootError.Buffer;
    if (PackedBootError->Version == 0) {
        Status = STATUS_REVISION_MISMATCH;
        goto BmErrorRestoreEnd;
    }

    //
    // Set our internal error state.
    //

    BmpErrorBuffer.Status = PackedBootError->Status;
    BmpErrorBuffer.SourceLine = PackedBootError->SourceLine;
    BmpErrorBuffer.ErrorMsg =
        Add2Ptr(PackedBootError,
                PackedBootError->ErrorMsgOffset);

    BmpErrorBuffer.FileName = NULL;
    if (PackedBootError->FileNameOffset != 0) {
        BmpErrorBuffer.FileName = Add2Ptr(PackedBootError,
                                          PackedBootError->FileNameOffset);
    }

    BmpErrorBuffer.HelpType = BmErrorHelpDefault;
    BmpErrorBuffer.SourceFile = NULL;
    if (PackedBootError->SourceFileOffset != 0) {
        BmpErrorBuffer.SourceFile = Add2Ptr(PackedBootError,
                                            PackedBootError->SourceFileOffset);
    }

    BmpInternalBootError = &BmpErrorBuffer;

BmErrorRestoreEnd:

    if (!NT_SUCCESS(Status)) {
        if (BmpPackedBootError.Buffer != NULL) {
            BlMmFreeHeap(BmpPackedBootError.Buffer);
            BmpPackedBootError.Buffer = NULL;
        }

        BmpPackedBootError.BufferSize = 0;
        return Status;
    }

    return STATUS_SUCCESS;;
}

//
// ---------------------------------------------------------- Private Functions
//

VOID
BmpErrorLog (
    __in ULONG ErrorCode,
    __in NTSTATUS ErrorStatus,
    __in ULONG ErrorMsgId,
    __in_opt PWCHAR FileName,
    __in BM_ERROR_HELP_TYPE HelpType,
    __in_opt PCHAR SourceFile,
    __in_opt ULONG SourceLine
    )

/*++

Routine Description:

    This routine stores the supplied error information in a local
    INTERNAL_BOOT_ENTRY structure where it can be retrieved later
    by BmErrorGetLast.

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorStatus - Supplies the error status.

    ErrorMsgId - Supplies the ID of the message describing the error.

    FileName - Supplies the name of the file associated with the error.

    HelpType - Supplies an enumerated value that indicates the help text to be
        displayed for the error.

    SourceFile - Supplies the source file in which error occurred.

    SourceLine - Supplies the line in source file where error occurred.

Return Value:

    None

--*/

{

    PWCHAR ErrorMsg;

    //
    // Purge any stale error information.
    //

    if (BmpInternalBootError != NULL) {
        BmErrorPurge();
    }

    ErrorMsg = BlResourceFindMessage(ErrorMsgId);
    if (ErrorMsg == NULL) {
        return;
    }

    //
    // Fill in the error record.
    //

    BmpErrorBuffer.ErrorCode = ErrorCode;
    BmpErrorBuffer.Status = ErrorStatus;
    BmpErrorBuffer.SourceLine = SourceLine;
    BmpErrorBuffer.ErrorMsg = ErrorMsg;
    BmpErrorBuffer.FileName = FileName;
    BmpErrorBuffer.HelpType = HelpType;
    BmpErrorBuffer.SourceFile = SourceFile;
    BmpInternalBootError = &BmpErrorBuffer;
    return;
}

BM_ERROR_HELP_TYPE
BmpFatalErrorMessageFilter (
    __in NTSTATUS ErrorStatus,
    __inout PULONG ErrorMsgId
    )

/*++

Routine Description:

    This routine filters on the error status, and updates the message with
    information specific to the error.

Arguments:

    ErrorStatus - Supplies the error status.

    ErrorMsgId - Supplies a pointer to the error message ID.

Return Value:

    Returns an enumerated value indicating which type of additional help
    information should be displayed to the user.

--*/

{

    BM_ERROR_HELP_TYPE HelpType;

    HelpType = BmErrorHelpDefault;

    //
    // Return error messages specific to the following error types.
    //

    switch (ErrorStatus) {
        case STATUS_FVE_LOCKED_VOLUME:
            *ErrorMsgId = BM_MSG_FVE_VOLUME_LOCKED;
            break;

        case STATUS_INVALID_IMAGE_WIN_64:
            *ErrorMsgId = BM_MSG_INVALID_IMAGE_64_ERROR;
            break;

        case STATUS_UNEXPECTED_IO_ERROR:
            *ErrorMsgId = BM_MSG_UNEXPECTED_IO_ERROR;
            HelpType = BmErrorHelpIo;
            break;

        case STATUS_IMAGE_CHECKSUM_MISMATCH:
            *ErrorMsgId = BM_MSG_IMAGE_CHECKSUM_ERROR;
            break;

        case STATUS_INVALID_IMAGE_HASH:
            *ErrorMsgId = BM_MSG_INVALID_IMAGE_HASH_ERROR;
            HelpType = BmErrorHelpHash;
            break;

        case STATUS_IMAGE_CERT_EXPIRED:
            *ErrorMsgId = BM_MSG_IMAGE_CERT_EXPIRED;
            HelpType = BmErrorHelpHash;
            break;
    }

    return HelpType;
}

VOID
BmpHandleInvalidBcdEntryEvent (
    __in NOTIFICATION_EVENT_TYPE EventType,
    __in_opt PNOTIFICATION_EVENT_CONTEXT EventContext,
    __in_opt PVOID Context1,
    __in_opt PVOID Context2
    )

/*++

Routine Description:

    This routine handles the event generated by an invalid entry in the
    BCD store.

Arguments:

    EventType - Supplies the type; must be EVENT_BCD_OBJECT_CONFIGURATION_ERROR.

    EventContext - Supplies the context provided by the event producer.

    HandlerContext1 - Supplies a pointer to the context passed during
        registration.

    HandlerContext2 - Supplies a pointer to the context passed during
        registration.

Return Value:

    None.

--*/

{

    INVALID_BCD_ENTRY_ERROR BcdEntryError;
    PWSTR Description;
    ULONG ElementId;
    PWSTR ElementName;
    HANDLE ObjectHandle;
    ULONG MessageId;
    ULONG Size;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context1);
    UNREFERENCED_PARAMETER(Context2);

    Description = NULL;
    if ((EventType != EVENT_BCD_OBJECT_CONFIGURATION_ERROR) ||
        (EventContext == NULL)) {

        goto HandleInvalidBcdEntryEventEnd;
    }

    RtlZeroMemory(&BcdEntryError, sizeof(INVALID_BCD_ENTRY_ERROR));
    MessageId = BM_MSG_BCD_INVALID_BCD_ENTRY_ERROR;
    BcdEntryError.ErrorMessage = BlResourceFindMessage(MessageId);
    if (BcdEntryError.ErrorMessage == NULL) {
        goto HandleInvalidBcdEntryEventEnd;
    }

    //
    // Get the description element for the object if available. This should
    // be skipped if the error was generated due to the description element.
    //

    Size = 0;
    ObjectHandle = EventContext->u.BcdObjectError.ObjectHandle;
    ElementId = BCDE_LIBRARY_TYPE_DESCRIPTION;
    ElementName = EventContext->u.BcdObjectError.ElementName;
    if (ElementName != NULL) {
        ElementId = (ULONG)wcstoul(ElementName, NULL, 16);
        if ((ElementId == 0) || (ElementId == MAXULONG)) {
            ElementId = BCDE_LIBRARY_TYPE_DESCRIPTION;
        }
    }

    if (ElementId != BCDE_LIBRARY_TYPE_DESCRIPTION) {
        Status = BcdGetElementData(ObjectHandle,
                                   BCDE_LIBRARY_TYPE_DESCRIPTION,
                                   NULL,
                                   &Size);

        if (Status == STATUS_BUFFER_TOO_SMALL) {
            Description = BlMmAllocateHeap(Size);
            if (Description != NULL) {
                BcdGetElementData(ObjectHandle,
                                  BCDE_LIBRARY_TYPE_DESCRIPTION,
                                  Description,
                                  &Size);
            }
        }
    }

    BcdEntryError.ObjectName = EventContext->u.BcdObjectError.ObjectName;
    BcdEntryError.Description = Description;
    BcdEntryError.ElementName = ElementName;
    BcdEntryError.Status = EventContext->u.BcdObjectError.Status;
    BmFatalErrorEx(BM_ERROR_INVALID_BCD_ENTRY,
                   (ULONG_PTR)BcdEntryError.ObjectName,
                   (ULONG_PTR)BcdEntryError.Description,
                   (ULONG_PTR)BcdEntryError.ElementName,
                   BcdEntryError.Status,
                   NULL,
                   0);

HandleInvalidBcdEntryEventEnd:
    if (Description != NULL) {
        BlMmFreeHeap(Description);
    }

    return;
}

VOID
BmpLogBootappLoadFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that a boot
    application load has failed.

Arguments:

    Status - This provides the status code indicating the reason for the load
        failure.

    Path - This provides the file path of the module that failed to load.

Return Value:

    None.

--*/

{

    PBSD_EVENT_LOGDATA_BOOTMGR_LOAD_FAILURE LoadFailure;
    ULONG LoadFailureSize;
    ULONG PathSize;
    NTSTATUS ReturnStatus;

    LoadFailureSize = FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_LOAD_FAILURE,
                                   FilePath);

    ReturnStatus = RtlULongMult((ULONG)wcslen(Path), sizeof(WCHAR), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(PathSize, sizeof(UNICODE_NULL), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(LoadFailureSize, PathSize, &LoadFailureSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    LoadFailure = BlMmAllocateHeap(LoadFailureSize);
    if (LoadFailure == NULL) {
        return;
    }

    RtlZeroMemory(LoadFailure, LoadFailureSize);
    LoadFailure->Status = Status;
    wcscpy_s((PWSTR)LoadFailure->FilePath, (PathSize / sizeof(WCHAR)), Path);
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_BOOTMGR_EVENT_LOAD_FAILURE,
                  LoadFailure,
                  LoadFailureSize);

    BlMmFreeHeap(LoadFailure);
}

VOID
BmpLogBcdFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that the BCD
    store has failed in some way.

Arguments:

    Status - This provides the status code indicating the reason for the BCD
        failure.

    Path - This provides the file path of the BCD store.

Return Value:

    None.

--*/

{

    PBSD_EVENT_LOGDATA_BOOTMGR_BCD_FAILURE BcdFailure;
    ULONG BcdFailureSize;
    ULONG PathSize;
    NTSTATUS ReturnStatus;

    BcdFailureSize = FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_BCD_FAILURE,
                                  BcdPath);

    ReturnStatus = RtlULongMult((ULONG)wcslen(Path), sizeof(WCHAR), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(PathSize, sizeof(UNICODE_NULL), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(BcdFailureSize, PathSize, &BcdFailureSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    BcdFailure = BlMmAllocateHeap(BcdFailureSize);
    if (BcdFailure == NULL) {
        return;
    }

    RtlZeroMemory(BcdFailure, BcdFailureSize);
    BcdFailure->Status = Status;
    wcscpy_s((PWSTR)BcdFailure->BcdPath, (PathSize / sizeof(WCHAR)), Path);
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_BOOTMGR_EVENT_BCD_FAILURE,
                  BcdFailure,
                  BcdFailureSize);

    BlMmFreeHeap(BcdFailure);
}

VOID
BmpLogInvalidBootEntryFailureEvent (
    __in PWSTR ObjectName,
    __in_opt PWSTR Description,
    __in_opt PWSTR ElementName,
    __in NTSTATUS ErrorStatus
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that an
    invalid boot entry was encountered in the BCD store.

Arguments:

    ObjectName - Supplies the GUID for the failing BCD object.

    Description - Supplies the value of description element for the BCD object.

    ElementName - Supplies the ID for the element for the invalid BCD object.

    ErrorStatus - Supplies the NTSTATUS code for the error.

Return Value:

    None.

--*/

{

    ULONG ElementId;
    PBSD_EVENT_LOGDATA_BOOTMGR_INVALID_BCD_ENTRY Failure;
    ULONG FailureSize;
    GUID Guid;
    ULONG Size;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    Failure = NULL;
    Size = 0;
    if (ARGUMENT_PRESENT(Description) != FALSE) {
        Status = RtlULongMult((ULONG)wcslen(Description), sizeof(WCHAR), &Size);
        if (!NT_SUCCESS(Status)) {
            goto LogInvalidBootEntryFailureEventEnd;
        }
    }

    Status = RtlULongAdd(Size, sizeof(UNICODE_NULL), &Size);
    if (!NT_SUCCESS(Status)) {
        goto LogInvalidBootEntryFailureEventEnd;
    }

    FailureSize = FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_INVALID_BCD_ENTRY,
                               Description);

    Status = RtlULongAdd(FailureSize, Size, &FailureSize);
    if (!NT_SUCCESS(Status)) {
        goto LogInvalidBootEntryFailureEventEnd;
    }

    Failure = BlMmAllocateHeap(FailureSize);
    if (Failure == NULL) {
        goto LogInvalidBootEntryFailureEventEnd;
    }

    RtlZeroMemory(Failure, FailureSize);
    RtlInitUnicodeString(&UnicodeString, ObjectName);
    Status = RtlGUIDFromString(&UnicodeString, &Guid);
    if (NT_SUCCESS(Status)) {
        Failure->ObjectId = Guid;
    }

    if (ARGUMENT_PRESENT(ElementName) != FALSE) {
        ElementId = (ULONG)wcstoul(ElementName, NULL, 16);
        if ((ElementId != 0) && (ElementId != MAXULONG)) {
            Failure->ElementId = ElementId;
        }
    }

    Failure->Status = ErrorStatus;
    if (ARGUMENT_PRESENT(Description) != FALSE) {
        wcscpy_s((PWSTR)Failure->Description,
                 (Size / sizeof(WCHAR)),
                 Description);
    }

    BlBsdLogEntry(BSD_EVENT_LEVEL_WARNING,
                  BSD_BOOTMGR_EVENT_INVALID_BCD_ENTRY_FAILURE,
                  Failure,
                  FailureSize);

LogInvalidBootEntryFailureEventEnd:
    if (Failure != NULL) {
        BlMmFreeHeap(Failure);
    }

    return;
}

VOID
BmpLogNoValidOsEntryFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that a module
    load has failed.

Arguments:

    Status - This provides the status code indicating the reason for the BCD
        failure.

    Path - This provides the file path of the BCD store.

Return Value:

    None.

--*/

{

    PBSD_EVENT_LOGDATA_BOOTMGR_NO_VALID_OS_FAILURE Failure;
    ULONG FailureSize;
    ULONG PathSize;
    NTSTATUS ReturnStatus;

    FailureSize = FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_NO_VALID_OS_FAILURE,
                               BcdPath);

    ReturnStatus = RtlULongMult((ULONG)wcslen(Path), sizeof(WCHAR), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(PathSize, sizeof(UNICODE_NULL), &PathSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    ReturnStatus = RtlULongAdd(FailureSize, PathSize, &FailureSize);
    if (!NT_SUCCESS(ReturnStatus)) {
        return;
    }

    Failure = BlMmAllocateHeap(FailureSize);
    if (Failure == NULL) {
        return;
    }

    RtlZeroMemory(Failure, FailureSize);
    Failure->Status = Status;
    wcscpy_s((PWSTR)Failure->BcdPath, (PathSize / sizeof(WCHAR)), Path);
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_BOOTMGR_EVENT_NO_VALID_OS_ENTRY_FAILURE,
                  Failure,
                  FailureSize);

    BlMmFreeHeap(Failure);
}

VOID
BmpLogGeneralFailureEvent (
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine logs an event in response to a general failure; a failure that
    doesn't fit into one of the other categories.

Arguments:

    Status - This provides the status code indicating the reason for the BCD
        failure.

Return Value:

    None.

--*/

{

    BSD_EVENT_LOGDATA_BOOTMGR_GENERAL_FAILURE GeneralFailure = {0};

    RtlZeroMemory(&GeneralFailure,
                  sizeof(BSD_EVENT_LOGDATA_BOOTMGR_GENERAL_FAILURE));

    GeneralFailure.Status = Status;
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_BOOTMGR_EVENT_GENERAL_FAILURE,
                  &GeneralFailure,
                  sizeof(BSD_EVENT_LOGDATA_BOOTMGR_GENERAL_FAILURE));
}


