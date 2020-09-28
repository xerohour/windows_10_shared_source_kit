/*++

Copyright (c) Microsoft Corporation

Module Name:

    resume.c

Abstract:

    Implementation of the Windows Boot Manager Resume Application support
    functions.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"
#include <bcd.h>
#include "systemevents.h"

//
// ---------------------------------------------------------------- Definitions
//

#define LEGACY_HIBER_FILE_PATH  L"\\hiberfil.sys"
#define LEGACY_HB_PTE_COUNT     16
#define RESUME_ATTEMPT_COUNT    3
#define RESUME_APP_LEGACY       L"\\NTLDR"

//
// Generate definitions matching the properties of the legacy hiberfile
// header.  These definitions are not available in a power manager header
// file since the current hiberfile is decoupled from this legacy format and
// no other component of the system is required to interpret legacy
// hiberfiles.
//

#define LEGACY_HIBERFILE_SIGNATURE          'rbih'
#define LEGACY_HIBERFILE_SIGNATURE_WAKE     'ekaw'
#define LEGACY_HIBERFILE_SIGNATURE_BREAK    'pkrb'
#define LEGACY_HIBERFILE_SIGNATURE_LINK     'knil'

typedef struct _LEGACY_HIBERFILE_HEADER {
    ULONG Signature;
    ULONG Version;
    ULONG CheckSum;
    ULONG LengthSelf;
    union {
        struct {
            ULONG PageSelf;
            ULONG PageSize;
            ULONG ImageType;
        } x86;

        struct {
            ULONGLONG PageSelf;
            ULONG PageSize;
            ULONG ImageType;
        } x64;
    };
} LEGACY_HIBERFILE_HEADER, *PLEGACY_HIBERFILE_HEADER;

typedef struct _LEGACY_HIBERFILE_LINK {
    ULONG Signature;
    WCHAR Name[1];
} LEGACY_HIBERFILE_LINK, *PLEGACY_HIBERFILE_LINK;

//
// -------------------------------------------------- Local Function Prototypes
//

NTSTATUS
BmpResumeCreateBootEntry (
    __in HANDLE DataStoreHandle,
    __deref_out PBOOT_APPLICATION_ENTRY *BootEntryOut
    );

NTSTATUS
BmpResumeClearAttemptIndicator (
    __in HANDLE DataStoreHandle
    );

VOID
BmpResumeLegacy (
    VOID
    );

NTSTATUS
BmpResumeProbeLegacyHiberFile (
    __out PULONG ImageType,
    __deref_opt_out PBOOT_ENVIRONMENT_DEVICE *Device,
    __deref_opt_out PWSTR *FilePath
    );

NTSTATUS
BmpResumeReadLegacyHiberFileHeader (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PWSTR FilePath,
    __out PLEGACY_HIBERFILE_HEADER *Header
    );

NTSTATUS
BmpResumeInvalidateLegacyHiberfile (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PWSTR FilePath
    );

VOID
BmpSaveResumeError (
    __in BOOLEAN Hiberboot
    );

//
// ------------------------------------------------------------------ Functions
//


NTSTATUS
BmResumeFromHibernate (
    __inout PHANDLE DataStoreHandle
    )

/*++

Routine Description:

    This function attempts to load a boot application to resume the
    system from hibernate.  If the resume succeeds, the resume app
    will transfer control to the resumed OS.  Otherwise, control will
    return here.

Arguments:

    DataStoreHandle - Supplies a pointer to a variable containing a handle to
        the system's boot configuration data store.  This handle will be
        altered on output if the data store is closed and reopened as part
        of the resume process.

Return Value:

    An error status is returned if and only if the data store is closed in
        preparation for a resume attempt and cannot be reopened afterward.

    STATUS_SUCCESS is returned otherwise, indicating that the boot manager
        should proceed to the boot menu.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE AppDevice;
    BOOLEAN AttemptResume;
    BOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    PBOOT_APPLICATION_ENTRY BootEntry;
    ULONG BootEntryCount;
    ULONGLONG BootMenuPolicy;
    BOOLEAN ErrorPresent;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    BOOLEAN FatalError;
    BOOLEAN HiberBoot;
    ULONG ImageHandle;
    NTSTATUS ImageStartStatus;
    ULONG LaunchReason;
    NTSTATUS LocalStatus;
    BOOLEAN MultiBootSystem;
    NTSTATUS Status;
    ULONGLONG Timeout;

    BootEntry = NULL;
    FatalError = FALSE;

    //
    // Check if the BCD element is TRUE that indicates the boot manager should
    // launch the resume application.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_ATTEMPT_RESUME,
                                           &AttemptResume);

    if (!NT_SUCCESS(Status)) {
        AttemptResume = FALSE;

        //
        // Check if the BCD element is TRUE that indicates HORM is enabled.
        //

        Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_HORM_ENABLED,
                                               &AttemptResume);

        if (!NT_SUCCESS(Status)) {
            AttemptResume = FALSE;
        }
    }

    //
    // If a resume attempt isn't indicated in the BCD store, proceed directly
    // to any legacy resume attempts that may be needed on this system.
    //

    if (AttemptResume == FALSE) {
        goto LegacyResume;
    }

    //
    // Attempt to launch the active resume loader described in the BCD store.
    // Determine whether this resume object is well formed and, if so, create
    // a boot entry for it.  Failure indicates that the BCD store is not
    // properly formed for resume.  Clear the attempt resume flag in this case
    // to prevent future fruitless resume attempts.
    //

    Status = BmpResumeCreateBootEntry(*DataStoreHandle,
                                      &BootEntry);

    if (!NT_SUCCESS(Status)) {
        BmpResumeClearAttemptIndicator(*DataStoreHandle);
        goto Exit;
    }

    //
    // If the resume menu policy is 'MenuPolicyStandard', read keyboard input
    // for user requests to pause at the multi OS menu or the system advanced
    // options menu. If no input was given and the policy is
    // 'MenuPolicyStandard', then determine the number of boot entries and
    // automatically trigger the multi OS menu if appropriate.
    //

    MultiBootSystem = BmIsMultiBootSystem(&BootEntryCount);
    BootMenuPolicy = BmGetBootMenuPolicy(BootEntry);
    if (BootMenuPolicy == MenuPolicyStandard) {

        //
        // If the boot manager's timeout is set to 0, then do not pass up the
        // multi-boot system setting.
        //

        Status = BlGetApplicationOptionInteger(BCDE_BOOTMGR_TYPE_TIMEOUT,
                                               &Timeout);

        if ((!NT_SUCCESS(Status) || (Timeout != 0)) &&
            (MultiBootSystem != FALSE)) {

            (VOID)BlAppendBootOptionBoolean(BootEntry,
                                            BCDE_LIBRARY_TYPE_MULTI_BOOT_SYSTEM,
                                            TRUE);
        }
    }

    //
    // Log the boot entry count to ETW for SQM pick-up.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&BootEntryCount,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BOOTMGR_MULTI_OS_COUNT,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Dispatch any custom OEM actions prior to resume from hibernate.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_HIBERBOOT,
                                           &HiberBoot);

    if (!NT_SUCCESS(Status)) {
        HiberBoot = FALSE;
    }

    //
    // Set the resume application as the currently selected boot application
    // to launch.
    //

    BmSetSelectedBootEntry(BootEntry);

    //
    // Attempt to load the resume application from disk.
    //

    Status = BlImgLoadBootApplication(BootEntry, NULL, NULL, &ImageHandle);
    if (Status == STATUS_NOT_FOUND) {
        LocalStatus = BlGetBootOptionDevice(
                          BootEntry->Options,
                          BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                          &AppDevice,
                          NULL);

        if (NT_SUCCESS(LocalStatus)) {
            LocalStatus = BlFwEnumerateDevice(AppDevice);
            if (NT_SUCCESS(LocalStatus)) {
                Status = BlImgLoadBootApplication(BootEntry,
                                                  NULL,
                                                  NULL,
                                                  &ImageHandle);
            }
        }
    }

    //
    // Handle returns from FVE load failure. If the load is cancelled, this
    // indicates that the user requested the multi-boot menu. Decide
    // whether or not to launch recovery for the multi-boot menu based on
    // the menu policy of the application that was being loaded. For other
    // recovery errors, launch the recovery sequence.
    //

    if ((Status == STATUS_CANCELLED) && (HiberBoot != FALSE)) {
        if ((BootMenuPolicy == MenuPolicyStandard) &&
            (BlBootOptionExists(BootEntry->Options,
                                BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE))) {

            LaunchReason = BSD_APP_LAUNCH_REASON_MULTI_BOOT_REQUESTED;
            BmpResumeClearAttemptIndicator(*DataStoreHandle);
            BmCloseDataStore(*DataStoreHandle);
            Status = BmLaunchRecoverySequence(BootEntry, LaunchReason);

        } else {
            BmpDisplayBootMenu = TRUE;
            BmDisplayStateCached = TRUE;
        }

    } else if (Status == STATUS_FVE_LOCKED_VOLUME) {
        LaunchReason = BSD_APP_LAUNCH_REASON_FVE_RECOVERY;
        BmpResumeClearAttemptIndicator(*DataStoreHandle);
        BmCloseDataStore(*DataStoreHandle);
        Status = BmLaunchRecoverySequence(BootEntry, LaunchReason);
    }

    if (!NT_SUCCESS(Status)) {
        BmpResumeClearAttemptIndicator(*DataStoreHandle);
        BmCloseDataStore(*DataStoreHandle);
        Status = BmOpenDataStore(DataStoreHandle);
        if (!NT_SUCCESS(Status)) {
            FatalError = TRUE;
            *DataStoreHandle = NULL;
        }

        goto Exit;
    }

    //
    // Execute the resume loader.  On success, the resume loader will
    // transition to a restored OS and control will not return.  The data
    // store is closed and, if necessary, reopened across this call so that
    // any changes made by the boot manager will be flushed to disk and thus
    // be available to the resumed OS.
    //

    BmCloseDataStore(*DataStoreHandle);
    ImageStartStatus = BlImgStartBootApplication(ImageHandle,
                                                 IMAGE_FLAGS_NONE,
                                                 &BaReturnArgs);

    Status = BmOpenDataStore(DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"BmResumeFromHibernate: Failed to reopen the BCD "
                      L"store after resume loader execution (0x%08x)\n",
                      Status);

        BlImgUnloadBootApplication(ImageHandle);
        FatalError = TRUE;
        *DataStoreHandle = NULL;
        goto Exit;
    }

    //
    // Control has returned from the resume loader, implying that there is
    // potential for proceeding to OS boot.  OS boot can lead to modification
    // of the file system associated with a hibernated OS.  The hibernation
    // image contains cached copies of portions of the file system, therefore
    // restarting it on top of a modified filesystem can cause significant
    // corruption.
    //
    // This implies that resume must not be retried if control returns and
    // proceeds toward the boot menu.  To enforce this requirement,
    // unconditionally attempt to remove the attempt resume indicator from the
    // boot manager BCD object.
    //

    BmpResumeClearAttemptIndicator(*DataStoreHandle);

    //
    // Cases where control returns from the resume application fall into three
    // groups.
    //
    // First, there are explicit fatal resume errors indicated by persistent
    // error data returned by the resume application.  These errors are
    // reported to the user.  Second, there are non-fatal errors reported
    // using STATUS_ABANDON_HIBERFILE.  These errors correspond to cases where
    // an existing, valid hiberfile has been explicitly invalidated according
    // to user preference.  Third, there are all remaining non-fatal errors.
    // These indicate conditions, such as failing to find a valid hiberfile,
    // where resume from hibernate is not required and boot should proceed
    // silently.
    //
    // Very early fatal errors may not have persistent data attached when
    // control returns and could thus be incorrectly be considered part of the
    // third group.  These cases should be very rare and the user experience
    // would not be catastrophic even if this corner case does arise.
    //

    ErrorPresent = FALSE;
    if (!NT_SUCCESS(ImageStartStatus)) {
        Status = BmErrorGetData();
        if (NT_SUCCESS(Status)) {
            ErrorPresent = TRUE;
        }
    }

    BlImgUnloadBootApplication(ImageHandle);
    if (ErrorPresent == TRUE) {
        BmpSaveResumeError(HiberBoot);
        BmErrorPurge();

    } else if (ImageStartStatus == STATUS_ABANDON_HIBERFILE) {

        //
        // The user has explicitly requested that the hiberfile be abandoned
        // and that OS boot continue.  Proceed directly to the boot menu.
        //

        goto Exit;

    } else {

        //
        // A silent failure has been reported.  Act as if the resume attempt
        // did not occur and proceed to any legacy resume attempts that are
        // necessary on this system.
        //

        NOTHING;
    }

    //
    // PC/AT is the only platform where older OSes use a legacy resume scheme
    // incompatible with the one implemented above.  Attempt legacy resume
    // only on PC/AT systems.
    //

LegacyResume:

#ifdef PCAT

    BmpResumeLegacy();

#endif

Exit:
    if (BootEntry != NULL) {
        BmSetSelectedBootEntry(NULL);
        BlDestroyBootEntry(BootEntry);
    }

    if (FatalError == FALSE) {
        Status = STATUS_SUCCESS;
    }

    return Status;
}

//
// ------------------------------------------------------------ Local Functions
//

//
// Define the list of element types required to be present in a resume loader
// object.
//

BCDE_DATA_TYPE BmpResumeRequiredElements[] = {
    BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
    BCDE_RESUME_LOADER_TYPE_HIBERFILE_DEVICE,
    BCDE_RESUME_LOADER_TYPE_HIBERFILE_PATH
};

VOID
BmpSaveResumeError (
    __in BOOLEAN Hiberboot
    )

/*++

Routine Description:

    This function saves the error returned from the resume application so that
    it can be preserved when the system boots.

Arguments:

    Hiberboot - Supplies whether or not this boot was a hybrid boot.

Return Value:

    None.

--*/

{

    PINTERNAL_BOOT_ERROR ErrorLog;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    PCEVENT_DESCRIPTOR EventDescriptor;

    if (Hiberboot != FALSE) {
        EventDescriptor = &BOOT_HIBERBOOT_FAILURE;

    } else {
        EventDescriptor = &BOOT_RESUME_FAILURE;
    }

    ErrorLog = BmErrorGetLast();

    //
    // Save the error status and message id.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&ErrorLog->Status,
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&ErrorLog->ErrorMsg,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  EventDescriptor,
                  NULL,
                  2,
                  EventDataDescriptor);

    return;
}

NTSTATUS
BmpResumeCreateBootEntry (
    __in HANDLE DataStoreHandle,
    __deref_out PBOOT_APPLICATION_ENTRY *BootEntryOut
    )

/*++

Routine Description:

    This routine allocates and creates a boot entry corresponding to
    the boot manager's resume object.

Arguments:

    DataStoreHandle - Supplies a handle to the system's boot configuration
        data store.

    BootEntryOut - Supplies a pointer to a variable that will be
        filled in with a pointer to the allocated boot entry.

Return Value:

    STATUS_SUCCESS if the function succeds, or an approprate error status code.

--*/

{

    PBOOT_APPLICATION_ENTRY BootEntry;
    BCD_OBJECT_DESCRIPTION Description;
    ULONG Index;
    PBOOT_ENTRY_OPTION Options;
    ULONG OptionListSize;
    BOOLEAN PathOptionExists;
    GUID ResumeObject;
    NTSTATUS Status;

    BootEntry = NULL;
    Options = NULL;

    //
    // Get the object associated with the OS to resume.
    //

    Status = BlGetApplicationOptionGuid(BCDE_BOOTMGR_TYPE_RESUME_OBJECT,
                                        &ResumeObject);

    if (!NT_SUCCESS(Status)) {
        goto BmpResumeCreateBootEntryEnd;
    }

    //
    // Get the option list for the resume object, and make sure it contains
    // the required elements.
    //

    Status = BmGetOptionList(DataStoreHandle, &ResumeObject, &Options);
    if (!NT_SUCCESS(Status)) {
        goto BmpResumeCreateBootEntryEnd;
    }

    for (Index = 0;
         Index < RTL_NUMBER_OF(BmpResumeRequiredElements);
         Index += 1) {

        if (BlBootOptionExists(
                Options,
                BmpResumeRequiredElements[Index]) == FALSE) {

            Status = STATUS_UNSUCCESSFUL;
            goto BmpResumeCreateBootEntryEnd;
        }
    }

    //
    // Create a boot entry for this resume object.
    //

    OptionListSize = BlGetBootOptionListSize(Options);
    BootEntry = BlMmAllocateHeap(sizeof(BOOT_APPLICATION_ENTRY) +
                                 OptionListSize);

    if (BootEntry == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto BmpResumeCreateBootEntryEnd;
    }

    //
    // Copy the identifier and options, zeroing the rest of the identifier.
    //

    RtlZeroMemory(BootEntry, sizeof(BOOT_APPLICATION_ENTRY));
    RtlCopyMemory(&BootEntry->Identifier, &ResumeObject, sizeof(GUID));
    BootEntry->Options = Add2Ptr(BootEntry,
                                 sizeof(BOOT_APPLICATION_ENTRY));

    RtlCopyMemory(BootEntry->Options, Options, OptionListSize);

    //
    // Verify the object description, and set the appropriate attribute
    // for the resume loader.  And if the path element is missing, use the
    // default path.
    //

    Status = BmGetEntryDescription(DataStoreHandle,
                                   &ResumeObject,
                                   &Description);

    if (!NT_SUCCESS(Status)) {
        goto BmpResumeCreateBootEntryEnd;
    }

    if ((GET_BCD_OBJECT_TYPE(Description.Type) !=
         BCD_OBJECT_TYPE_APPLICATION) ||
        (GET_BCD_OBJECT_IMAGE_TYPE(Description.Type) !=
         BCD_IMAGE_TYPE_BOOT_APPLICATION) ||
        (GET_BCD_OBJECT_APPLICATION_TYPE(Description.Type) !=
         BCD_APPLICATION_TYPE_WINDOWS_RESUME_APPLICATION)) {

        Status = STATUS_UNSUCCESSFUL;
        goto BmpResumeCreateBootEntryEnd;
    }

    PathOptionExists = BlBootOptionExists(Options,
                                          BCDE_LIBRARY_TYPE_APPLICATION_PATH);

    if (PathOptionExists == FALSE) {
        Status = BlAppendBootOptionString(BootEntry,
                                          BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          BM_DEFAULT_RESUME_LOADER_PATH);

        if (!NT_SUCCESS(Status)) {
            goto BmpResumeCreateBootEntryEnd;
        }
    }

    SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OS_RESUME_LOADER);

BmpResumeCreateBootEntryEnd:
    if (Options != NULL) {
        BlMmFreeHeap(Options);
    }

    if (NT_SUCCESS(Status)) {
        *BootEntryOut = BootEntry;

    } else {
        if (BootEntry != NULL) {
            BlDestroyBootEntry(BootEntry);
        }
    }

    return Status;
}

NTSTATUS
BmpResumeClearAttemptIndicator (
    __in HANDLE DataStoreHandle
    )

/*++

Routine Description:

    This function attempts to clear the global attempt resume indicator from
    the BCD store.

Arguments:

    DataStoreHandle - Supplies a handle to the system's boot configuration
        data store.

Return Value:

    STATUS_SUCCESS if the function succeds, or an approprate error status code.

--*/

{

    NTSTATUS Status;

    //
    // Invalidate the resume settings from the current in-memory BCD settings.
    //

    BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_ATTEMPT_RESUME);
    BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_HIBERBOOT);

    //
    // The hiberboot flag is always present, so clear it here as well.
    //

    Status = BmPurgeOption(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           BCDE_BOOTMGR_TYPE_HIBERBOOT);

    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"BmResumeFromHibernate: Failed to purge hiberboot "
                      L"option in the boot manager's BCD settings "
                      L"(0x%08x)\n",
                      Status);
    }

    //
    // Delete the permanent indication from the BCD store.
    //

    Status = BmPurgeOption(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           BCDE_BOOTMGR_TYPE_ATTEMPT_RESUME);

    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"BmResumeFromHibernate: Failed to purge attempt resume "
                      L"option in the boot manager's BCD settings (0x%08x)\n",
                      Status);
    }

    return Status;
}

NTSTATUS
BmpResumeCreateLegacyBootEntry (
    __in PWSTR FileName,
    __deref_out PBOOT_APPLICATION_ENTRY *BootEntryOut
    )

/*++

Routine Description:

    This routine allocates and creates a boot entry corresponding to
    the system-wide resume from hibernation application.

Arguments:

    FileName - Supplies the name of the resume application.

    BootEntryOut - Supplies a pointer to a variable that will be
        filled in with a pointer to the allocated boot entry.

Return Value:

    STATUS_SUCCESS if the function succeds, or an approprate error status code.

--*/

{

    PBOOT_ENTRY_OPTION ApplicationOptions;
    ULONG ApplicationOptionSize;
    PBOOT_APPLICATION_ENTRY BootEntry;
    ULONG Offset;
    PBOOT_ENTRY_OPTION Option;
    ULONG RequiredSize;
    NTSTATUS Status;
    PBCDE_STRING StringElement;
    ULONG StringSize;

    //
    // First compute the size of the boot entry, field by field.
    //
    // Header.
    //

    RequiredSize = sizeof(BOOT_APPLICATION_ENTRY);

    //
    // Options.  The resume app will inherit the global options
    // of the boot manager.  Remove the boot manager's application path option,
    // since the correct option for the resume application must be used instead.
    // The application device for the resume app is the same as this
    // application, so it can be preserved.
    //

    Status = BlCopyApplicationOptions(&ApplicationOptions);
    if (!NT_SUCCESS(Status) || (ApplicationOptions == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    BlRemoveBootOption(ApplicationOptions,
                       BCDE_LIBRARY_TYPE_APPLICATION_PATH);

    ApplicationOptionSize = BlGetBootOptionListSize(ApplicationOptions);
    Status = RtlULongAdd(RequiredSize, ApplicationOptionSize, &RequiredSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Assert that the application device actually exists in the copied options.
    //

    ASSERT(BlBootOptionExists(ApplicationOptions,
                              BCDE_LIBRARY_TYPE_APPLICATION_DEVICE) != FALSE);

    //
    // Reserve space for the file path in the options list.
    //

    Status = RtlULongAdd(RequiredSize,
                         sizeof(BOOT_ENTRY_OPTION),
                         &RequiredSize);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongAdd(RequiredSize,
                         FIELD_OFFSET(BCDE_STRING, String),
                         &RequiredSize);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongMult((ULONG)wcslen(FileName),
                          sizeof(WCHAR),
                          &StringSize);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongAdd(StringSize, sizeof(UNICODE_NULL), &StringSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongAdd(RequiredSize, StringSize, &RequiredSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Now allocate memory for the structure and fill it in.
    //

    BootEntry = BlMmAllocateHeap(RequiredSize);
    if (BootEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(BootEntry, RequiredSize);

    //
    // Fill in the identifier using the well-known resume app GUID.
    //

    RtlCopyMemory(&BootEntry->Identifier,
                  &GUID_WINDOWS_RESUME_APP,
                  sizeof(GUID));

    //
    // Copy the boot manager options, removing the terminator at the end of the
    // list, since one more option will be tacked onto the end.
    //

    BootEntry->Options = Add2Ptr(BootEntry, sizeof(BOOT_APPLICATION_ENTRY));
    RtlCopyMemory(BootEntry->Options,
                  ApplicationOptions,
                  ApplicationOptionSize);

    Offset = 0;
    do {
        Option = Add2Ptr(BootEntry->Options, Offset);
        Offset = Option->NextOptionOffset;
    } while (Offset != 0);

    Option->NextOptionOffset = ApplicationOptionSize;

    //
    // Add the application path option containing the resume filename.  This
    // is assumed to be boot-device relative.
    //

    Option = Add2Ptr(BootEntry->Options, ApplicationOptionSize);
    Option->Type = BCDE_LIBRARY_TYPE_APPLICATION_PATH;
    Option->DataSize = StringSize;
    Option->DataOffset = sizeof(BOOT_ENTRY_OPTION);
    StringElement = Add2Ptr(Option, Option->DataOffset);
    RtlCopyMemory(StringElement->String, FileName, StringSize);
    Option->NextOptionOffset = 0;
    SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_NTLDR);

    //
    // Return the boot entry to the caller.
    //

    *BootEntryOut = BootEntry;
    return STATUS_SUCCESS;
}

VOID
BmpResumeLegacy (
    VOID
    )

/*++

Routine Description:

    This function attempts to load a boot application to resume the
    legacy system from hibernate.  If the resume succeeds, the resume app
    will transfer control to the resumed OS.  Otherwise, control will
    return here.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    PBOOT_APPLICATION_ENTRY BootEntry;
    PWCHAR FileName;
    PBOOT_ENVIRONMENT_DEVICE HiberfileDevice;
    PWCHAR HiberfilePath;
    ULONG ImageHandle;
    ULONG ImageType;
    NTSTATUS Status;

    //
    // Check if a legacy hiberfile exists on the machine, and also determine
    // its image type.  If a hiberfile is found and a legacy resume app cannot
    // be executed, this routine will always attempt to invalidate the
    // hiberfile.  If a valid hiberfile is on disk when the boot menu is
    // displayed, then the selected OS can alter filesystems in a manner than
    // invalidates the file system cache contents still stored in the
    // hiberfile.  Once in this condition resume can cause disk corruption and
    // thus must be avoided.
    //

    Status = BmpResumeProbeLegacyHiberFile(&ImageType,
                                           &HiberfileDevice,
                                           &HiberfilePath);

    if (!NT_SUCCESS(Status)) {
        return;
    }


    //
    // Attempt to load \NTLDR to resume this pre-Longhorn hiberfile.
    //

    FileName = RESUME_APP_LEGACY;
    Status = BmpResumeCreateLegacyBootEntry(FileName, &BootEntry);
    if (!NT_SUCCESS(Status)) {
        goto InvalidateHiberfile;
    }

    Status = BlImgLoadBootApplication(BootEntry, NULL, NULL, &ImageHandle);
    if (!NT_SUCCESS(Status)) {
        BlDestroyBootEntry(BootEntry);
        goto InvalidateHiberfile;
    }

    //
    // Attempt to transfer control to the loaded image.  Control will never
    // return here if resume is successful.  Control will only return in cases
    // where an error occurs prior to control being transferred to NTLDR.
    //

    Status = BlImgStartBootApplication(ImageHandle,
                                       IMAGE_FLAGS_NONE,
                                       &BaReturnArgs);

    ASSERT(NT_SUCCESS(Status));

    BlImgUnloadBootApplication(ImageHandle);
    BlDestroyBootEntry(BootEntry);

    //
    // The legacy hiberfile cannot be resumed, attempt to invalidate it to
    // prevent this now stale hiberfile from causing future resume attempts.
    //

InvalidateHiberfile:
    BmpResumeInvalidateLegacyHiberfile(HiberfileDevice, HiberfilePath);
    BlMmFreeHeap(HiberfileDevice);
    BlMmFreeHeap(HiberfilePath);
    return;
}

NTSTATUS
BmpResumeProbeLegacyHiberFile (
    __out PULONG ImageType,
    __deref_opt_out PBOOT_ENVIRONMENT_DEVICE *DeviceOut,
    __deref_opt_out PWSTR *FilePathOut
    )

/*++

Routine Description:

    This routine attempts to determine whether a valid legacy hiberfile is
    present.  If a candidate hiberfile is found, validity is determined based
    upon the signature, image header length, and page size reported in the
    file header.

    Legacy hiberfiles are located by first checking HIBERFIL.SYS in the root
    directory of the device that houses BOOTMGR (i.e., the boot device).  This
    file may either be the hiberfile itself or a link specifying the ARC path
    to the device that does contain the real HIBERFIL.SYS in its root
    directory.

Arguments:

    ImageType - Supplies a pointer to a variable that receives the processor
        architecture reported in the hiberfile header.  The state of this
        variable is indeterminate when an error status is returned.  All
        returned values are one of the PROCESSOR_ARCHITECTURE_Xxx identifiers.

    DeviceOut - Supplies an optional pointer to a variable that receives a
        pointer to the boot environment device that houses the legacy
        hiberfile.  This structure is allocated from pool and must be freed by
        the caller.

    FilePathOut - Supplies an optional pointer to a variable that receives the
        absolute path to the location of the legacy hiberfile on the returned
        device.

Return Value:

    STATUS_SUCCESS is returned if a hiberfile was found that passes all
        validity checks.

    STATUS_INVALID_IMAGE_FORMAT is returned if a hiberfile was found that did
        not pass all validity checks.

    Other error codes may be returned if failures are encountered trying to
        open or read the hiberfile.

--*/

{

    ULONG CommonPageSize;
    PBOOT_ENVIRONMENT_DEVICE Device;
    PWCHAR FilePath;
    GENERIC_BUFFER FilePathBuffer;
    PLEGACY_HIBERFILE_HEADER Header;
    PLEGACY_HIBERFILE_LINK ImageLink;
    NTSTATUS Status;

    Device = NULL;
    Header = NULL;

    //
    // Attempt to open the hiberfile from the root directory of the system
    // partition (boot device).  If present, read the header into memory.
    // The header will indicate if the hiberfile is valid and resumable.
    //

    Status = BlGetApplicationOptionDevice(BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                          &Device,
                                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto BmpProbeLegacyHiberFileEnd;
    }

    Status = BmpResumeReadLegacyHiberFileHeader(Device,
                                                LEGACY_HIBER_FILE_PATH,
                                                &Header);

    if (!NT_SUCCESS(Status)) {
        goto BmpProbeLegacyHiberFileEnd;
    }

    //
    // It's possible for the hiberfile in the root of the system disk to point
    // to a linked file in another location.  If the signature indicates that
    // this is the case, open the linked hiberfile.
    //

    if (Header->Signature == LEGACY_HIBERFILE_SIGNATURE_LINK) {
        ImageLink = (PLEGACY_HIBERFILE_LINK)Header;

        //
        // The link is expressed as an ARC path.  Convert this to a boot
        // environment native file path.  The file path consists of a device
        // structure prepended to a string of the file path off of that device.
        // The hiberfile is still expected to be in the root directory in this
        // scenario.
        //

        BlInitGenericBuffer(&FilePathBuffer, NULL, 0);
        Status = AhGetBootEnvironmentDevice((PCHAR)ImageLink->Name,
                                            NULL,
                                            &FilePathBuffer,
                                            TRUE);

        if (!NT_SUCCESS(Status)) {
            goto BmpProbeLegacyHiberFileEnd;
        }

        BlMmFreeHeap(Device);
        Device = (PBOOT_ENVIRONMENT_DEVICE)FilePathBuffer.Buffer;
        if (Device == NULL) {
            Status = STATUS_NO_SUCH_DEVICE;
            goto BmpProbeLegacyHiberFileEnd;
        }

        BlMmFreeHeap(Header);
        Header = NULL;
        Status = BmpResumeReadLegacyHiberFileHeader(Device,
                                                    LEGACY_HIBER_FILE_PATH,
                                                    &Header);

        if (!NT_SUCCESS(Status)) {
            goto BmpProbeLegacyHiberFileEnd;
        }
    }

    //
    // Exit if the hiberfile disposition is set to ignore, or it has the wrong
    // version.
    //

    if (((Header->Signature != LEGACY_HIBERFILE_SIGNATURE) &&
         (Header->Signature != LEGACY_HIBERFILE_SIGNATURE_WAKE)) ||
        (Header->Version != 0)) {

        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto BmpProbeLegacyHiberFileEnd;
    }

    //
    // Attempt to return the processor architecture associated with this
    // hiberfile.  The offset of the image type member is different between
    // machines with a 32-bit PFN_NUMBER type and those with a 64-bit
    // PFN_NUMBER.  Legacy resume applies only to the x86 and x64 processor
    // architectures.  On these machines, the boot manager is compiled into
    // x86 instructions and thus sees a 32-bit PFN_NUMBER.
    //
    // The 32-bit PFN_NUMBER generates an image type field offset four bytes
    // short of the same field with a 64-bit PFN_NUMBER.  Due to the layout of
    // the structure, this places the 32-bit image type offset at the same
    // offset as the page size field on 64-bit systems.  Ensure that the page
    // size and the image type are both consistent with either x86 or x64.
    // This is more reliable than simply checking the image type since well
    // formed values may appear randomly (e.g., PROCESSOR_ARCHITECTURE_INTEL
    // is defined to zero).
    //
    // N.B. All x86 and x64 operating systems that used the legacy resume
    //      process also used a 4KB page size.
    //

    CommonPageSize = 0x1000;
    if ((Header->x86.ImageType == PROCESSOR_ARCHITECTURE_INTEL) &&
        (Header->x86.PageSize == CommonPageSize)) {

        *ImageType = PROCESSOR_ARCHITECTURE_INTEL;

    } else if ((Header->x64.ImageType == PROCESSOR_ARCHITECTURE_AMD64) &&
               (Header->x64.PageSize == CommonPageSize)) {

        *ImageType = PROCESSOR_ARCHITECTURE_AMD64;

    } else {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto BmpProbeLegacyHiberFileEnd;
    }

    Status = STATUS_SUCCESS;

    //
    // If requested by the caller, return the device and file path used to
    // access the hiberfile on successful completion.
    //

BmpProbeLegacyHiberFileEnd:
    if (NT_SUCCESS(Status) &&
        ARGUMENT_PRESENT(DeviceOut) &&
        ARGUMENT_PRESENT(FilePathOut)) {

        FilePath = BlMmAllocateHeap(sizeof(LEGACY_HIBER_FILE_PATH));
        if (FilePath != NULL) {
            RtlCopyMemory(FilePath,
                          LEGACY_HIBER_FILE_PATH,
                          sizeof(LEGACY_HIBER_FILE_PATH));

            *FilePathOut = FilePath;
            *DeviceOut = Device;
            Device = NULL;

        } else {
            Status = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if (Device != NULL) {
        BlMmFreeHeap(Device);
    }

    if (Header != NULL) {
        BlMmFreeHeap(Header);
    }

    return Status;
}

NTSTATUS
BmpResumeReadLegacyHiberFileHeader (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PWSTR FilePath,
    __out PLEGACY_HIBERFILE_HEADER *HeaderOut
    )

/*++

Routine Description:

    Attempts to open the hiberfile at the specified location.  When
    successful, the image header is read into an allocated memory buffer.

Arguments:

    Device - Supplies the boot environment device containing the hiberfile
        to open.

    FilePath - Supplies the file path to the hiberfile to open.

    HeaderOut - Supplies a pointer to a variable that receives a pointer to an
        allocated buffer holding the entire hiberfile header.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if the hiberfile does not exist.
    STATUS_NO_SUCH_DEVICE if the specified device does not exist.

--*/

{

    DEVICE_ID DeviceId;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    PVOID Header;
    ULONG Length;
    OPEN_FLAGS OpenFlags;
    NTSTATUS Status;

    ASSERT(Device != NULL);
    ASSERT(FilePath != NULL);

    DeviceId = INVALID_DEVICE_ID;
    FileId = INVALID_FILE_ID;

    OpenFlags = OPEN_READ;
    Status = BlDeviceOpen(Device, OpenFlags, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    Status = BlFileOpen(DeviceId, FilePath, OpenFlags, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // Determine the number of header bytes to read.  The hiberfile header is
    // normally one page in size, however it can be smaller in the case of
    // hiber link files.
    //

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (FileInformation.FileSize < sizeof(LEGACY_HIBERFILE_HEADER)) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto Exit;

    } else if (FileInformation.FileSize < PAGE_SIZE) {
        Length = (ULONG)FileInformation.FileSize;

    } else {
        Length = PAGE_SIZE;
    }

    //
    // Allocate a buffer large enough to accomodate the header and fill it
    // with data from the start of the file.
    //

    Header = BlMmAllocateHeap(Length);
    if (Header == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    Status = BlFileReadAtOffset(FileId, Length, 0, Header, NULL);
    if (NT_SUCCESS(Status)) {
        *HeaderOut = Header;

    } else {
        BlMmFreeHeap(Header);
    }

Exit:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    return Status;
}

NTSTATUS
BmpResumeInvalidateLegacyHiberfile (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PWSTR FilePath
    )

/*++

Routine Description:

    This function attempts to invalidate a legacy hiberfile by clearing the
    hiberfile signature.

Arguments:

    Device - Supplies the device that houses the legacy hiberfile.

    FilePath - Supplies the absolute path to the location of the legacy
        hiberfile on the given device.

Return Value:

    STATUS_SUCCESS is returned if the hiberfile is opened and invalidated
        without error.

    Other error codes may be returned if failures occur while trying to open
        or write the hiberfile.

--*/
{

    DEVICE_ID DeviceId;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    OPEN_FLAGS OpenFlags;
    ULONG Signature;
    NTSTATUS Status;

    ASSERT(Device != NULL);
    ASSERT(FilePath != NULL);

    DeviceId = INVALID_DEVICE_ID;
    FileId = INVALID_FILE_ID;

    OpenFlags = OPEN_READ | OPEN_WRITE;
    Status = BlDeviceOpen(Device, OpenFlags, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    Status = BlFileOpen(DeviceId, FilePath, OpenFlags, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // Clear the four byte signature that lies at the beginning of the
    // hiberfile.
    //

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (FileInformation.FileSize < sizeof(ULONG)) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto Exit;
    }

    Signature = 0;
    Status = BlFileWriteAtOffset(FileId, sizeof(ULONG), 0, &Signature, NULL);

Exit:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    return Status;
}

