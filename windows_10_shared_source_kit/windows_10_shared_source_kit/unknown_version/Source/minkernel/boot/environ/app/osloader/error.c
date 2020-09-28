/*++

Copyright (c) Microsoft Corporation

Module Name:

    error.c

Abstract:

    Provides error support functions for the boot loader.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "systemevents.h"

//
// ----------------------------------------------------------------- Prototypes
//

VOID
OslpErrorLog (
    __in NTSTATUS ErrorStatus,
    __in ULONG ErrorMsgId,
    __in_opt PWSTR FileName,
    __in_opt PSTR  SourceFile,
    __in_opt ULONG SourceLine
    );

VOID
OslpFatalErrorMessageFilter (
    __in NTSTATUS ErrorStatus,
    __inout PULONG ErrorMsgId
    );

VOID
OslpLogLoadFailureEvent (
    __in ULONG ErrorCode,
    __in NTSTATUS Status,
    __in PWSTR Path
    );

VOID
OslpLogHiveFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    );

VOID
OslpLogAcpiFailureEvent (
    __in NTSTATUS Status
    );

VOID
OslpLogGeneralFailureEvent (
    __in NTSTATUS Status
    );

VOID
OslpLogLoadImportFailure (
    __in NTSTATUS Status,
    __in PCWSTR Path,
    __in PCWSTR FailedPath
    );

VOID
OslpLogImportFailure (
    __in NTSTATUS Status,
    __in PCWSTR Path,
    __in PCSTR Import
    );


//
// --------------------------------------------------------------------- Locals
//

static BOOLEAN OslpErrorInfoExists = FALSE;
static PSTATIC_BOOT_ERROR StaticBootError = NULL;
static STATIC_BOOT_ERROR StaticBootErrorBuffer;

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslBlStatusErrorHandler(
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4
    )

/*++

Routine Description:

    OS Loader routine for processing boot library errors.  Currently,
    this routine is a wrapper for OslFatalError.

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorParameter1 - Additional information for the error.

    ErrorParameter2 - Additional information for the error.

    ErrorParameter3 - Additional information for the error.

    ErrorParameter4 - Additional information for the error.

Returns:

    STATUS_SUCCESS if successful.

    STATUS_NOT_IMPLEMENTED if error is not handled.

--*/

{
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(ErrorParameter4);

    switch (ErrorCode) {

        //
        // Failure to load a required import case.  Parameters:
        //     ErrorParameter1 - ANSI string name of the required import.
        //     ErrorParameter2 - Path to the image containing the import.
        //     ErrorParameter3 - Failure status code.
        //

    case BL_ERROR_IMPORT:
        OslFatalError(OSL_ERROR_IMPORT,
                      ErrorParameter1,
                      ErrorParameter2,
                      ErrorParameter3);
        Status = STATUS_SUCCESS;
        break;
        
        //
        // Default case.  Let the library handle this library error.
        //

    default:
        Status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    return Status;
}

VOID
OslFatalErrorEx (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in_opt PSTR  SourceFile,
    __in_opt ULONG SourceLine
    )

/*++

Routine Description:

    OS Loader routine for processing internal application errors.  Currently,
    this routine is a pretty print wrapper for BlStatusError.  It could
    additionally be used to present a user interface for the specified error.

Arguments:

    ErrorCode - ULONG value indicating the reason of the error.

    ErrorParameter1 - Additional information for the error.

    ErrorParameter2 - Additional information for the error.

    ErrorParameter3 - Additional information for the error.

    SourceFile - Source file in which error occurred, #if DBG only.

    SourceLine - Line in source file, where error occurred, #if DBG only.

Returns:

    None.

--*/

{

    PCHAR CString1;
    WCHAR DescriptionString[256];
    ULONG ErrorMsgId;
    PWCHAR FileName;
    WCHAR FileNameBuffer[256];
    NTSTATUS Status1;
    PWCHAR WString1;
    PWCHAR WString2;

    //
    // Format a descriptive string for the error for the purposes of reporting
    // the error to the library so it can be output to the debugger.  Since this
    // is only for internal debugging purposes, the strings are not localized.
    //
    // While formatting the debugging string, also pick out the localizable
    // message ID that should be used to display the error on the screen.  If
    // the error is associated with a specific file, save this as well.
    //

    switch (ErrorCode) {

        //
        // Failure to load kernel case.  Parameters:
        //     ErrorParameter1 - Path to the kernel.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_KERNEL:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows kernel %s failed to load (%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_KERNEL_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_KERNEL, Status1, WString1);
        break;

        //
        // Failure to load HAL case.  Parameters:
        //     ErrorParameter1 - Path to the HAL.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_HAL:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows HAL %s failed to load (%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_HAL_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_HAL, Status1, WString1);
        break;

        //
        // Failure to load debugger DLL case.  Parameters:
        //     ErrorParameter1 - Path to the debugger DLL.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_KD_DLL:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows Kernel Debugger DLL %s failed to load "
                   L"(%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_KD_DLL_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_KD_DLL, Status1, WString1);
        break;

        //
        // Failure to load registry hive case.  Parameters:
        //     ErrorParameter1 - Path to the failed hive.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_REGISTRY:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nRegistry hive %s failed to load (%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_REGISTRY_LOAD_ERROR;
        OslpLogHiveFailureEvent(Status1, WString1);
        break;

        //
        // Failure to load critical driver case.  Parameters:
        //     ErrorParameter1 - Path to the root of the OS installation.
        //     ErrorParameter2 - Relative path to the driver being loaded.
        //     ErrorParameter3 - Failure status code.
        //

    case OSL_ERROR_CRITICAL_DRIVER:
        WString1 = (PWCHAR)ErrorParameter1;
        WString2 = (PWCHAR)ErrorParameter2;
        Status1 = (NTSTATUS)ErrorParameter3;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nCritical driver %s%s failed to load (%08x)\n",
                   WString1,
                   WString2,
                   Status1);

        swprintf_s(FileNameBuffer,
                   sizeof(FileNameBuffer) / sizeof(WCHAR) - 1,
                   L"%s%s",
                   WString1,
                   WString2);

        FileName = &FileNameBuffer[0];
        ErrorMsgId = OSL_MSG_CRITICAL_DRIVER_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_CRITICAL_DRIVER, Status1, FileName);
        break;

        //
        // Failure to load an image containing a required import case.
        // Parameters:
        //     ErrorParameter1 - Path to the image that could not be loaded.
        //     ErrorParameter2 - Path to the image containing the needed import.
        //     ErrorParameter3 - Failure status code.
        //

    case OSL_ERROR_LOAD_IMPORT:
        WString1 = (PWCHAR)ErrorParameter1;
        WString2 = (PWCHAR)ErrorParameter2;
        Status1 = (NTSTATUS)ErrorParameter3;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nFailed to load image %s, imported from %s (%08x)\n",
                   WString1,
                   WString2,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_FILE_LOAD_ERROR;
        OslpLogLoadImportFailure(Status1, WString1, WString2);
        break;

        //
        // Failure to load a required import case.  Parameters:
        //     ErrorParameter1 - ANSI string name of the required import.
        //     ErrorParameter2 - Path to the image containing the import.
        //     ErrorParameter3 - Failure status code.
        //

    case OSL_ERROR_IMPORT:
        CString1 = (PCHAR)ErrorParameter1;
        WString1 = (PWCHAR)ErrorParameter2;
        Status1 = (NTSTATUS)ErrorParameter3;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nUnable to import %S from image %s (%08x)\n",
                   CString1,
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_FILE_LOAD_ERROR;
        OslpLogImportFailure(Status1, WString1, CString1);
        break;

        //
        // Machine is not ACPI compliant case.  Parameters:
        //     ErrorParameter1 - None.
        //     ErrorParameter2 - None.
        //     ErrorParameter3 - Failure status code.
        //

    case OSL_ERROR_NO_ACPI_DETECTED:
        Status1 = (NTSTATUS)ErrorParameter3;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nUnable to detect ACPI (%08x)\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_ACPI_ERROR;
        OslpLogAcpiFailureEvent(Status1);
        break;

        //
        // Invalid ACPI table case.  Parameters:
        //     ErrorParameter1 - None.
        //     ErrorParameter2 - None.
        //     ErrorParameter3 - Failure status code.
        //

    case OSL_ERROR_INVALID_RSDP:
        Status1 = (NTSTATUS)ErrorParameter3;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nACPI RSDP table is invalid (%08x)\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_INVALID_RSDP_ERROR;
        OslpLogAcpiFailureEvent(Status1);
        break;

        //
        // Could not find NLS data.  Parameters:
        //     ErrorParameter1 - Failure status code.
        //

    case OSL_ERROR_NLS_DATA:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nNLS data is missing, or corrupt (%08x).\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_NLS_DATA_ERROR;
        OslpLogGeneralFailureEvent(Status1);
        break;

        //
        // Code Integrity initialization failure.  Parameters:
        //     ErrorParameter1 - Path to the winload application.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_CODE_INTEGRITY_INITIALIZATION:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\n%s Code Integrity failed to initialize "
                   L"(%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_CODE_INTEGRITY_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_CODE_INTEGRITY_INITIALIZATION,
                                Status1,
                                WString1);

        break;

        //
        // Memory Map Conflict detected during synchronization.
        //     ErrorParameter1 - Failure status code.
        //

    case OSL_ERROR_MEMORY_MAP_CONFLICT:
        Status1 = (NTSTATUS)ErrorParameter1;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nMemory Map Conflict detected during synchronization "
                   L"(%08x).\n",
                   Status1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_MEMORY_MAP_CONFLICT_ERROR;
        OslpLogGeneralFailureEvent(Status1);
        break;

        //
        // Failure to load an OS Extension case.  Parameters:
        //     ErrorParameter1 - Path to the kernel.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_OS_EXTENSION:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nOS Extension %s failed to load (%08x)\n",
                   WString1,
                   Status1);

        FileName = WString1;
        ErrorMsgId = OSL_MSG_OS_EXTENSION_ERROR;
        OslpLogLoadFailureEvent(OSL_ERROR_OS_EXTENSION, Status1, WString1);
        break;

        //
        // Failure to boot successfully multiple times in a row.
        //

    case OSL_ERROR_BAD_BOOT:
        Status1 = STATUS_UNSUCCESSFUL;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"The system failed to boot multiple times in a row.");

        FileName = NULL;
        ErrorMsgId = OSL_MSG_BAD_BOOT;
        break;

        //
        // Failure to shutdown successfully.
        //

    case OSL_ERROR_BAD_SHUTDOWN:
        Status1 = STATUS_UNSUCCESSFUL;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"The system failed to shutdown properly.");

        FileName = NULL;
        ErrorMsgId = OSL_MSG_BAD_SHUTDOWN;
        break;

        //
        // Failure with manufacturing mode profile.  Parameters:
        //     ErrorParameter1 - Manufacturing mode profile name.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_MANUFACTURING_MODE:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows manufacturing mode profile %s failed to initialize (%08x)\n",
                   WString1,
                   Status1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_MANUFACTURING_MODE;
        break;

        //
        // Failure with manufacturing mode profile due to lock screen security
        // being enabled. Parameters:
        //     ErrorParameter1 - Manufacturing mode profile name.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_MANUFACTURING_MODE_SECURITY_LOCK_SCREEN:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows manufacturing mode profile %s failed to initialize due to a lock screen password being enabled\n",
                   WString1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_MANUFACTURING_MODE_SECURITY_LOCK_SCREEN;
        break;

        //
        // Failure with manufacturing mode profile due to Bitlocker being
        // enabled. Parameters:
        //     ErrorParameter1 - Manufacturing mode profile name.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_MANUFACTURING_MODE_SECURITY_BITLOCKER:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows manufacturing mode profile %s failed to initialize due to Bitlocker being enabled\n",
                   WString1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_MANUFACTURING_MODE_SECURITY_BITLOCKER;
        break;

        //
        // Failure with manufacturing mode profile due to Reset Protection being
        // enabled. Parameters:
        //     ErrorParameter1 - Manufacturing mode profile name.
        //     ErrorParameter2 - Failure status code.
        //

    case OSL_ERROR_MANUFACTURING_MODE_SECURITY_RESET_PROTECTION:
        WString1 = (PWCHAR)ErrorParameter1;
        Status1 = (NTSTATUS)ErrorParameter2;
        swprintf_s(DescriptionString,
                   sizeof(DescriptionString)/sizeof(WCHAR) - 1,
                   L"\nWindows manufacturing mode profile %s failed to initialize due to Reset Protection being enabled\n",
                   WString1);

        FileName = NULL;
        ErrorMsgId = OSL_MSG_MANUFACTURING_MODE_SECURITY_RESET_PROTECTION;
        break;

        //
        // Default case.  Pass the parameters on to the library without
        // a description string.
        //

    default:
        Status1 = STATUS_UNSUCCESSFUL;
        DescriptionString[0] = L'\0';
        FileName = NULL;
        ErrorMsgId = OSL_MSG_UNKNOWN_ERROR;
        OslpLogGeneralFailureEvent(Status1);
        break;
    }

    //
    // Filter the error status to update specific messages.
    //

    OslpFatalErrorMessageFilter(Status1, &ErrorMsgId);

    //
    // Configure the static boot error such that it can be analyzed by the
    // boot debugger extension.
    //

    StaticBootError = &StaticBootErrorBuffer;
    StaticBootError->ErrorStatus = Status1;
    StaticBootError->ErrorMsg = BlResourceFindMessage(ErrorMsgId);
    StaticBootError->FileName = FileName;
    StaticBootError->SourceFile = SourceFile;
    StaticBootError->SourceLine = SourceLine;

    //
    // Instruct the library to report the error to the debugger.
    //

    BlStatusPrint(DescriptionString);
    BlStatusError(BL_ERROR_APPLICATION,
                  (ULONG_PTR)ErrorCode,
                  ErrorParameter1,
                  ErrorParameter2,
                  ErrorParameter3);

    //
    // Create an extended error log in our return arguments in case
    // the Boot Manager wants to display any error UI.
    //

    OslpErrorLog(Status1, ErrorMsgId, FileName, SourceFile, SourceLine);

    //
    // Attempt to log the error to the Boot Status Data file.
    //

    OslSetBootStatusErrorData(OslBootId,
                              ErrorCode,
                              Status1);

    return;
}

VOID
OslpFatalErrorMessageFilter (
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

    None.

--*/

{

    //
    // Return error messages specific to the following error types.
    //

    switch (ErrorStatus) {
        case STATUS_INVALID_IMAGE_HASH:
            *ErrorMsgId = OSL_MSG_INVALID_IMAGE_HASH_ERROR;
            break;

        case STATUS_FVE_LOCKED_VOLUME:
            *ErrorMsgId = OSL_MSG_FVE_VOLUME_LOCKED;
            break;
    }

    return;
}

VOID
OslpErrorLog (
    __in NTSTATUS ErrorStatus,
    __in ULONG ErrorMsgId,
    __in_opt PWSTR FileName,
    __in_opt PSTR  SourceFile,
    __in_opt ULONG SourceLine
    )

/*++

Routine Description:

    This routine allocates a buffer and copies the supplied error
    information to this new allocation in the form of a packed boot error
    record.  Finally, the library is called to persist the error buffer so
    it can be retrieved by the calling application.

Arguments:

    ErrorStatus - The error status.

    ErrorMsgId - ID of message describing the error.

    FileName - Name of the file associated with the error.

    SourceFile - Source file in which error occurred, #if DBG only.

    SourceLine - Line in source file, where error occurred, #if DBG only.

Return Value:

    None.  If there is an error creating the error log it is ignored, since the
    application is already in an error condition.

--*/

{

    UINTN AllocLen;
    GUID BaErrorGUID = BA_ERROR_GUID;
    GENERIC_BUFFER ErrorBuffer;
    PWCHAR ErrorMsg;
    ULONG ErrorMsgOffset;
    ULONG FileNameOffset;
    PPACKED_BOOT_ERROR PackedBootError;
    ULONG SourceFileOffset;
    NTSTATUS Status;

    //
    // Don't overwrite an earlier error as it is assumed to have better
    // information.
    //

    if (OslpErrorInfoExists == TRUE) {
        return;
    }

    //
    // Get the string that corresponds to the resource ID.
    //

    ErrorMsg = BlResourceFindMessage(ErrorMsgId);
    if (ErrorMsg == NULL) {
        return;
    }

    //
    // Allocate storage for the error log.  Use the error data memory type so
    // the error record will be properly handled by the boot manager.
    //

    AllocLen = ALIGN_RANGE_UP(sizeof(PACKED_BOOT_ERROR), sizeof(UINTN));
    ErrorMsgOffset = (ULONG)AllocLen;
    AllocLen += ALIGN_RANGE_UP((wcslen(ErrorMsg) + 1) * sizeof(WCHAR),
                               sizeof(UINTN));

    if (FileName != NULL) {
        FileNameOffset = (ULONG)AllocLen;
        AllocLen += ALIGN_RANGE_UP((wcslen(FileName) + 1) * sizeof(WCHAR),
                                   sizeof(UINTN));

    } else {
        FileNameOffset = 0;
    }

    if (SourceFile != NULL) {
        SourceFileOffset = (ULONG)AllocLen;
        AllocLen += ALIGN_RANGE_UP((strlen(SourceFile) + 1) * sizeof(CHAR),
                                   sizeof(UINTN));

    } else {
        SourceFileOffset = 0;
    }

    PackedBootError = BlMmAllocateHeap(AllocLen);
    if (PackedBootError == NULL) {
        return;
    }

    //
    // Fill in our error record.
    //

    RtlZeroMemory(PackedBootError, AllocLen);
    PackedBootError->Size = (ULONG)AllocLen;
    PackedBootError->Version = BOOT_ERROR_VERSION;
    PackedBootError->Status = ErrorStatus;
    PackedBootError->SourceLine = SourceLine;
    PackedBootError->ErrorMsgOffset = ErrorMsgOffset;
    PackedBootError->FileNameOffset = FileNameOffset;
    PackedBootError->SourceFileOffset = SourceFileOffset;
    wcscpy_s(Add2Ptr(PackedBootError, ErrorMsgOffset),
             wcslen(ErrorMsg) + 1,
             ErrorMsg);

    if (FileNameOffset != 0) {
        wcscpy_s(Add2Ptr(PackedBootError, FileNameOffset),
                 wcslen(FileName) + 1,
                 FileName);
    }

    if (SourceFileOffset != 0) {
        strcpy_s(Add2Ptr(PackedBootError, SourceFileOffset),
                 strlen(SourceFile) + 1,
                 SourceFile);
    }

    //
    // Call the library to persist the error buffer.
    //

    BlInitGenericBuffer(&ErrorBuffer, PackedBootError, (ULONG)AllocLen);
    Status = BlPdSaveData(NULL, &BaErrorGUID, &ErrorBuffer);
    if (NT_SUCCESS(Status)) {
        OslpErrorInfoExists = TRUE;
    }

    BlMmFreeHeap(PackedBootError);
    return;
}

VOID
OslpLogLoadFailureEvent (
    __in ULONG ErrorCode,
    __in NTSTATUS Status,
    __in PWSTR Path
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that a module
    load has failed.

Arguments:

    ErrorCode - The OS Loader specific error code for this failure.

    Status - This provides the status code indicating the reason for the load
        failure.

    Path - This provides the file path of the module that failed to load.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    PBSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE LoadFailure;
    ULONG LoadFailureSize;

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        Path,
                        (ULONG)(wcslen(Path) + 1) * sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[1], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_IMAGE_LOAD_FAILURE,
                  NULL,
                  2,
                  EventDataDescriptor);

    //
    // Log the information into the boot log.
    //

    LoadFailureSize = (ULONG)
        (FIELD_OFFSET(BSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE, FilePath) +
        ((wcslen(Path) + 1) * sizeof(WCHAR)));

    LoadFailure = BlMmAllocateHeap(LoadFailureSize);
    if (LoadFailure == NULL) {
        return;
    }

    RtlZeroMemory(LoadFailure, LoadFailureSize);
    LoadFailure->Size = LoadFailureSize;
    LoadFailure->ErrorCode = ErrorCode;
    LoadFailure->Status = Status;
    wcscpy_s((PWSTR)LoadFailure->FilePath, wcslen(Path) + 1, Path);
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_LOAD_FAILURE,
                  LoadFailure,
                  LoadFailureSize);

    BlMmFreeHeap(LoadFailure);
    return;
}

VOID
OslpLogHiveFailureEvent (
    __in NTSTATUS Status,
    __in PWSTR Path
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that a hive
    load has failed.

Arguments:

    Status - This provides the status code indicating the reason for the
        failure.

    Path - This provides the file path of the hive that failed to load.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    PBSD_LOGDATA_OSLOADER_EVENT_HIVE_FAILURE HiveFailure;
    ULONG HiveFailureSize;

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        Path,
                        (ULONG)(wcslen(Path) + 1) * sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[1], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_HIVE_LOAD_FAILURE,
                  NULL,
                  2,
                  EventDataDescriptor);

    //
    // Log the information into the boot log.
    //

    HiveFailureSize = (ULONG)
        (FIELD_OFFSET(BSD_LOGDATA_OSLOADER_EVENT_HIVE_FAILURE, HivePath) +
        ((wcslen(Path) + 1) * sizeof(WCHAR)));

    HiveFailure = BlMmAllocateHeap(HiveFailureSize);
    if (HiveFailure == NULL) {
        return;
    }

    RtlZeroMemory(HiveFailure, HiveFailureSize);
    HiveFailure->Status = Status;
    wcscpy_s((PWSTR)HiveFailure->HivePath, wcslen(Path) + 1, Path);
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_HIVE_FAILURE,
                  HiveFailure,
                  HiveFailureSize);

    BlMmFreeHeap(HiveFailure);
    return;
}


VOID
OslpLogAcpiFailureEvent (
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that no ACPI
    BIOS was detected, or that the ACPI implementation is faulty.

Arguments:

    Status - This provides the status code indicating the reason for the
        failure.

Return Value:

    None.

--*/

{

    BSD_LOGDATA_OSLOADER_EVENT_ACPI_FAILURE AcpiFailure = {0};
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_ACPI_FAILURE,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Log the information into the boot log.
    //

    AcpiFailure.Status = Status;
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_ACPI_FAILURE,
                  &AcpiFailure,
                  sizeof(BSD_LOGDATA_OSLOADER_EVENT_ACPI_FAILURE));

    return;
}

VOID
OslpLogGeneralFailureEvent (
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine logs a Boot Status Data logging event to indicate that an
    error of unknown origin has occurred.

Arguments:

    Status - This provides the status code indicating the reason for the
        failure.  This is almost always STATUS_UNSUCCESSFUL.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    BSD_LOGDATA_OSLOADER_EVENT_GENERAL_FAILURE GeneralFailure = {0};

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_GENERAL_FAILURE,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Log the information into the boot log.
    //

    GeneralFailure.Status = Status;
    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_GENERAL_FAILURE,
                  &GeneralFailure,
                  sizeof(BSD_LOGDATA_OSLOADER_EVENT_GENERAL_FAILURE));

    return;
}

VOID
OslpLogLoadImportFailure (
    __in NTSTATUS Status,
    __in PCWSTR Path,
    __in PCWSTR FailedPath
    )

/*++

Routine Description:

    This routine logs a boot status data log entry to indicate that the module
    containing a required import could not be loaded.

Arguments:

    Status - Provides the NTSTATUS code indicating the reason for the failure.

    Path - Provides the path of the binary being loaded.

    FailedPath - Provides the path of the binary that contains the required
        import, and has failed to load.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[3];
    PBSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE LoadFailure;
    ULONG LoadFailureSize;

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        Path,
                        (ULONG)(wcslen(Path) + 1) * sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[1],
                        FailedPath,
                        (ULONG)(wcslen(FailedPath) + 1) * sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[2], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_LOAD_IMPORT_IMAGE_FAILURE,
                  NULL,
                  3,
                  EventDataDescriptor);

    //
    // Alocate enough space for both path strings, and a null terminator for
    // each.
    //

    LoadFailureSize = (ULONG)
        (FIELD_OFFSET(BSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE, FilePath) +
        ((wcslen(Path) + wcslen(FailedPath) + 2) * sizeof(WCHAR)));

    LoadFailure = BlMmAllocateHeap(LoadFailureSize);
    if (LoadFailure == NULL) {
        return;
    }

    RtlZeroMemory(LoadFailure, LoadFailureSize);
    LoadFailure->Size = LoadFailureSize;
    LoadFailure->ErrorCode = OSL_ERROR_LOAD_IMPORT;
    LoadFailure->Status = Status;
    wcscpy_s((PWSTR)LoadFailure->FilePath, wcslen(Path) + 1, Path);
    wcscpy_s((PWSTR)LoadFailure->FilePath + wcslen(Path) + 1,
             wcslen(FailedPath) + 1,
             FailedPath);

    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_LOAD_FAILURE,
                  LoadFailure,
                  LoadFailureSize);

    BlMmFreeHeap(LoadFailure);
    return;
}


VOID
OslpLogImportFailure (
    __in NTSTATUS Status,
    __in PCWSTR Path,
    __in PCSTR Import
    )

/*++

Routine Description:

    This routine logs a boot status data log entry indicating that a required
    import from the specified file doesn't exist.

Arguments:

    Status - Provides the NTSTATUS code indicating the reason for the failure.

    Path - Provides the path of the binary being loaded.

    Import - Provides the ANSI string containing the name of the import that is
        missing.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[3];
    PBSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE LoadFailure;
    ULONG LoadFailureSize;

    //
    // Log the information into the ETW log.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        Path,
                        (ULONG)(wcslen(Path) + 1) * sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[1],
                        Import,
                        (ULONG)(strlen(Import) + 1));

    EventDataDescCreate(&EventDataDescriptor[2], &Status, sizeof(ULONG));
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_LOAD_IMPORT_FAILURE,
                  NULL,
                  3,
                  EventDataDescriptor);

    //
    // Alocate enough space for both path strings, and a null terminator for
    // each.
    //

    LoadFailureSize = (ULONG)
        (FIELD_OFFSET(BSD_LOGDATA_OSLOADER_EVENT_LOAD_FAILURE, FilePath) +
        ((wcslen(Path) + strlen(Import) + 2) * sizeof(WCHAR)));

    LoadFailure = BlMmAllocateHeap(LoadFailureSize);
    if (LoadFailure == NULL) {
        return;
    }

    RtlZeroMemory(LoadFailure, LoadFailureSize);
    LoadFailure->Size = LoadFailureSize;
    LoadFailure->ErrorCode = OSL_ERROR_IMPORT;
    LoadFailure->Status = Status;
    wcscpy_s((PWSTR)LoadFailure->FilePath, wcslen(Path) + 1, Path);

    //
    // The Import parameter is an ANSI string, and so the %S format specifier
    // must be specified to convert the string into a wide character string.
    //

    swprintf_s(LoadFailure->FilePath + wcslen(Path) + 1,
               strlen(Import) + 1,
               L"%S",
               Import);

    BlBsdLogEntry(BSD_EVENT_LEVEL_ERROR,
                  BSD_OSLOADER_EVENT_LOAD_FAILURE,
                  LoadFailure,
                  LoadFailureSize);

    BlMmFreeHeap(LoadFailure);
    return;
}


