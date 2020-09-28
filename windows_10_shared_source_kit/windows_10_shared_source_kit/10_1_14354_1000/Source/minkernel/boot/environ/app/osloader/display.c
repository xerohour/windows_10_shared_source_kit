/*++

Copyright (c) Microsoft Corporation

Module Name:

    display.c

Abstract:

    User Interface for the OS loader.  Provides the infrastructure for the
    Advanced Options Menu.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "systemevents.h"

//
// ---------------------------------------------------------------- Definitions
//

#define MAX_TEMP_STRING_CHARS 80
#define GUID_FORMAT_STRING L"'&#123;%08x&#45;%04x&#45;%04x&#45;%02x%02x&#45;%02x%02x%02x%02x%02x%02x&#125;'"

#define DEFAULT_LOCALE L"en-US"
#define MAX_VISIBLE_OPTIONS 9

//
// ----------------------------------------------------------------- Structures
//

typedef struct _BOOT_OPTION {
    ULONG BootOptionIndex;
    PWCHAR BootOptionString;
} BOOT_OPTION, *PBOOT_OPTION;

//
// --------------------------------------------------------------------- Locals
//

static BOOLEAN OslpGraphicsModeEnabled = FALSE;

const UNICODE_STRING OslpDomainControllerSkuType =
    RTL_CONSTANT_STRING (L"LanmanNT");

const UNICODE_STRING OslpAdvancedMenuChoice[MAX_ADVANCED_BOOT_OPTION + 1] = {
    RTL_CONSTANT_STRING(L"SafeMode"),
    RTL_CONSTANT_STRING(L"SafeMode-Net"),
    RTL_CONSTANT_STRING(L"SafeMode-Cmd"),
    RTL_CONSTANT_STRING(L"Recovery"),
    RTL_CONSTANT_STRING(L"BootLogging"),
    RTL_CONSTANT_STRING(L"LowResVideo"),
    RTL_CONSTANT_STRING(L"LKG"),
    RTL_CONSTANT_STRING(L"DSR"),
    RTL_CONSTANT_STRING(L"Debugging"),
    RTL_CONSTANT_STRING(L"DisableCrashReboot"),
    RTL_CONSTANT_STRING(L"DisableCodeIntegrity"),
    RTL_CONSTANT_STRING(L"Normal"),
    RTL_CONSTANT_STRING(L"NoGui"),
    RTL_CONSTANT_STRING(L"NoELAM"),
    RTL_CONSTANT_STRING(L"")
};

//
// -------------------------------------------------- Local Function Prototypes
//

NTSTATUS
OslpAppendAdvancedOptions (
    __in ULONG AdvancedOption
    );

NTSTATUS
OslpDisplayAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount
    );

NTSTATUS
OslpDisplayEditBootOptions (
    VOID
    );

BOOLEAN
OslpDisplayInitializeBgfx (
    __in ULONG Flags
    );

BOOLEAN
OslpDisplayIsKeyboardLessSystem (
    VOID
    );

NTSTATUS
OslpDisplayLegacyAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle
    );

VOID
OslpDisplayLoadNextScreen (
    __in PXML_DOC Post
    );

NTSTATUS
OslpDisplayStandardAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount
    );

NTSTATUS
OslpCheckPermission (
    __in PERMISSION_ACTION_TYPE ActionRequested
    );

BOOLEAN
OslpIsDomainController (
    __in ULONG SystemHiveHandle
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslDisplayInitialize (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine initializes (and reinitializes) the display for the
    OS loader.

Arguements:

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    STATUS_SUCCESS if initialization succeeds, or a failure status code
    if the display console could not be registered or the BlDisplay APIs
    return an error.

--*/

{

    BOOLEAN BugcheckDisabled;
    BOOLEAN DisplayParameters;
    BOOLEAN EmoticonDisabled;
    ULONG Flags;
    ULONG HorizontalResolution;
    NTSTATUS Status;
    PWCHAR StyleSheet;
    ULONG VerticalResolution;

    //
    // Fire up the XML engine.
    //

    StyleSheet = BlResourceFindHtml(L"OSLOADER.XSL");
    if (StyleSheet == NULL) {
        return STATUS_NOT_FOUND;
    }

    Status = BlXmiInitialize(StyleSheet, NULL);
    if (!NT_SUCCESS(Status)) {
        goto DisplayInitializeEnd;
    }

    //
    // Let the remote console know we're loading the OS, and start watching
    // for key-strokes indicating the user wants the advanced options
    // screen.
    //

    OslpGraphicsModeEnabled = FALSE;
    BlXmiWrite(L"<osload-main/>");

    //
    // Check if the display library is currently in graphics mode.
    //

    BlDisplayGetScreenResolution(&HorizontalResolution, &VerticalResolution);
    if ((HorizontalResolution == TEXT_MODE_HORIZONTAL_RESOLUTION) &&
        (VerticalResolution == TEXT_MODE_VERTICAL_RESOLUTION)) {

        goto DisplayInitializeEnd;
    }

    //
    // Query bug check related options.
    //

    Flags = BGFX_FLAGS_NONE;
    OslHiveQueryBugcheckOptions(SystemHiveHandle,
                                &BugcheckDisabled,
                                &EmoticonDisabled,
                                &DisplayParameters);

    if (BugcheckDisabled != FALSE) {
        SET_FLAGS(Flags, BGFX_FLAGS_DISABLE_BUGCHECK);
    }

    if (EmoticonDisabled != FALSE) {
        SET_FLAGS(Flags, BGFX_FLAGS_DISABLE_EMOTICON);
    }

    if (DisplayParameters != FALSE) {
        SET_FLAGS(Flags, BGFX_FLAGS_DISPLAY_BUGCHECK_PARAMETERS);
    }

    //
    // Initialize the BGFX library. Failure here is non-fatal.
    //

    OslpDisplayInitializeBgfx(Flags);

DisplayInitializeEnd:
    return Status;
}

VOID
OslDisplayDestroy (
    __in_opt PVOID DisplayContext
    )

/*++

Routine Description:

    This routine reverts the setup done by display initialization, and disables
    boot graphics.

    N.B. Standard console output is not affected.

Arguments:

    DisplayContext - Supplies the display context.

Return Value:

    None.

--*/

{

    if (OslpGraphicsModeEnabled != FALSE) {
        if (ARGUMENT_PRESENT(DisplayContext) != FALSE) {
            BgFreeContext(DisplayContext);
        }

        BgLibraryDestroy();
        OslpGraphicsModeEnabled = FALSE;
    }

    return;
}

NTSTATUS
OslDisplayAdvancedOptionsProcess (
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount,
    __out PULONG ReturnFlags
    )

/*++

Routine Description:

    Displays the Advanced Options Menu, presenting the user with a
    list of advanced options to select from.

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    LastBootSucceeded - Supplies the last boot's status from bootstat.dat.

    LastBootShutdown - Supplies the last boot's shutdown status from
        bootstat.dat.

    LastBootCheckpoint - Supplies the last boot's checkpoint status from
        bootstat.dat.

    BootAttemptCount - Supplies the boot attempt count from the last boot.

    ReturnFlags - Supplies a pointer to flags that are set if the user chooses
        to execute the recovery sequence for the application, or selects
        advanced boot options that are not currently permitted.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_CANCELLED if the user decides to cancel advanced selection.

--*/

{

    BOOLEAN AdvancedOptions;
    LONG AdvancedChoiceIndex;
    BOOLEAN BootOptions;
    NTSTATUS Status;

    *ReturnFlags = FALSE;

    //
    // Check if the user has requested to edit the boot options.
    //

    BootOptions = FALSE;
    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_DISPLAY_OPTIONS_EDIT,
                 &BootOptions);

    if (NT_SUCCESS(Status) && (BootOptions != FALSE)) {
        Status = OslpDisplayEditBootOptions();
        goto DisplayAdvancedOptionsProcessEnd;
    }

    //
    // Determine whether the user has requested advanced options.
    //

    AdvancedOptions = FALSE;
    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS,
                 &AdvancedOptions);

    //
    // Display the advanced OS options menu.
    //

    if (NT_SUCCESS(Status) && (AdvancedOptions != FALSE)) {
        Status = OslpDisplayAdvancedBootMenu(&AdvancedChoiceIndex,
                                             SystemHiveHandle,
                                             LastBootSucceeded,
                                             LastBootShutdown,
                                             LastBootCheckpoint,
                                             BootAttemptCount);

        if (!NT_SUCCESS(Status)) {
            goto DisplayAdvancedOptionsProcessEnd;

        } else if (AdvancedChoiceIndex == RECOVERY_SEQUENCE) {
            SET_FLAGS(*ReturnFlags, RETURN_ARGUMENT_LAUNCH_RECOVERY_MANUAL);
            goto DisplayAdvancedOptionsProcessEnd;

        } else {
            Status = OslpAppendAdvancedOptions(AdvancedChoiceIndex);
            goto DisplayAdvancedOptionsProcessEnd;
        }
    }

    Status = STATUS_SUCCESS;

DisplayAdvancedOptionsProcessEnd:
    return Status;
}

VOID
OslDisplayMessage (
    VOID
    )

/*++

Routine Description:

    This routine displays a message to the screen based on the BCD store.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONGLONG DisplayMessageType;
    PWCHAR Message;
    NTSTATUS Status;

    //
    // Determine if a custom message is set in the current BCD entry. Use the
    // default message otherwise.
    //

    Status = BlGetApplicationOptionInteger(
                 BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE,
                 &DisplayMessageType);

    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionInteger(
                     BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE,
                     &DisplayMessageType);

        if (!NT_SUCCESS(Status)) {
            DisplayMessageType = BcdeBootuxMessageDefault;
        }
    }

    //
    // Only display a string in the case of Hyper-V.
    //

    if (DisplayMessageType == BcdeBootuxMessageHyperV) {
        Message = BlResourceFindMessage(OSL_DISPLAY_STRING_HYPER_V);

        ASSERT(Message != NULL);

        BgDisplayString(Message);
    }

    return;
}

//
// ------------------------------------------------------------ Local Functions
//

NTSTATUS
OslpDisplayEditBootOptions (
    VOID
    )

/*++

Routine Description:

    This routine present a menu which allows the user to view, and
    modify the boot options for the current application.

Arguments:

    None

Return Value:

    STATUS_SUCCESS, STATUS_CANCELLED, or STATUS_UNSUCCESSFUL

--*/

{

    PWCHAR ApplicationPath;
    BOOLEAN AttemptDisplay;
    PCWSTR ChoiceString;
    DEVICE_ID DeviceId;
    PWCHAR FriendlyName;
    PUCHAR GuidOffset;
    HARD_DISK_IDENTIFIER HardDisk;
    DEVICE_INFORMATION Information;
    SIZE_T Length;
    PWCHAR OptionsCopy;
    PWCHAR OsxmlString;
    PBLOCK_IO_IDENTIFIER Parent;
    PDEVICE_PARTITION_INFORMATION PartitionInformation;
    ULONG PartitionNumber;
    PXML_DOC Post;
    NTSTATUS Status;
    WCHAR TempString[MAX_TEMP_STRING_CHARS];

    ChoiceString = NULL;
    Post = NULL;

    //
    // Determine if the UI can be displayed.  Only proceed if no component
    // vetos.
    //

    Status = OslpCheckPermission(PermissionActionWinloadDisplayEditBootOptions);
    if (!NT_SUCCESS(Status)) {
        return STATUS_SUCCESS;
    }

    //
    // Initializers that are not functionally required but are needed to allow
    // this code to compile with unintialized variable checking enabled.
    //

    Parent = NULL;
    GuidOffset = NULL;
    PartitionNumber = 0;

    //
    // Make a copy of the OS load options string, in wide character format.
    //

    OptionsCopy = NULL;
    if (OslLoadOptions != NULL) {
        Length = (strlen(OslLoadOptions) + 1) * sizeof(WCHAR);
        OptionsCopy = BlMmAllocateHeap(Length);
        if (OptionsCopy == NULL) {
            Status = STATUS_NO_MEMORY;
            goto DisplayEditBootOptionsEnd;
        }

        BlCopyStringToWcharString(OptionsCopy, OslLoadOptions);
    }

    //
    // Present the edit boot options menu.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_DESCRIPTION,
                                          &FriendlyName);

    if (!NT_SUCCESS(Status)) {
        FriendlyName = NULL;
    }

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          &ApplicationPath);

    if (!NT_SUCCESS(Status)) {
        ApplicationPath = NULL;
    }

    BlXmiWrite(L"<osload-edit");
    if (FriendlyName != NULL) {
        BlXmiWrite(L" name='");
        Status = BlXmiTransformTextForXml(FriendlyName, &OsxmlString);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(OsxmlString);
            BlMmFreeHeap(OsxmlString);

        } else {
            BlXmiSanitizeTextForXml(FriendlyName);
            BlXmiWrite(FriendlyName);
        }

        BlXmiWrite(L"'");
        BlMmFreeHeap(FriendlyName);
    }

    if (ApplicationPath != NULL) {
        BlXmiWrite(L" path='");
        Status = BlXmiTransformTextForXml(ApplicationPath, &OsxmlString);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(OsxmlString);
            BlMmFreeHeap(OsxmlString);

        } else {
            BlXmiSanitizeTextForXml(ApplicationPath);
            BlXmiWrite(ApplicationPath);
        }

        BlXmiWrite(L"'");
        BlMmFreeHeap(ApplicationPath);
    }

    //
    // Print device information for partition devices to help users distinguish
    // operating systems in a multi-boot environment.
    //
    // Load pointers to the relevant fields depending upon the type of
    // partition device being displayed.
    //

    AttemptDisplay = FALSE;
    if (OslLoadDevice->DeviceType == DEVICE_PARTITION) {
        AttemptDisplay = TRUE;
        Parent = &OslLoadDevice->u.Partition.ParentIdentifier;
        GuidOffset = (PUCHAR)&OslLoadDevice->u.Partition.Gpt.PartitionSignature;
        PartitionNumber = OslLoadDevice->u.Partition.Mbr.PartitionNumber;

    } else if (OslLoadDevice->DeviceType == DEVICE_PARTITION_EX) {

        //
        // The partition number is always displayed for MBR disks since it is
        // marginally easier for a user to correlate a partition number with
        // the associated device compared to trying to correlate a 64-bit
        // partition offset on disk.
        //
        // The partition number is only part of the deprecated MBR partition
        // identifier structure.  For the current structure, open the
        // partition device and extract the partition number from the device
        // information structure.
        //

        Parent = &OslLoadDevice->u.PartitionEx.ParentIdentifier;
        GuidOffset =
            (PUCHAR)&OslLoadDevice->u.PartitionEx.Gpt.PartitionSignature;

        if ((Parent->BlockIoType == BLOCK_IO_HARD_DISK) ||
            (Parent->BlockIoType == BLOCK_IO_VIRTUAL_HARD_DISK)) {

            if (Parent->u.HardDisk.PartitionType == GPT) {
                AttemptDisplay = TRUE;

            } else {
                Status = BlDeviceOpen(OslLoadDevice, OPEN_READ, &DeviceId);
                if (NT_SUCCESS(Status)) {
                    Status = BlDeviceGetInformation(DeviceId, &Information);

                    ASSERT(NT_SUCCESS(Status));

                    PartitionInformation =
                        &Information.u.BlockIo.DeviceInformation.Partition;

                    PartitionNumber = PartitionInformation->Mbr.Number;
                    BlDeviceClose(DeviceId);
                    AttemptDisplay = TRUE;
                }
            }
        }
    }

    if ((AttemptDisplay != FALSE) &&
        ((Parent->BlockIoType == BLOCK_IO_HARD_DISK) ||
         (Parent->BlockIoType == BLOCK_IO_VIRTUAL_HARD_DISK))) {

        HardDisk = Parent->u.HardDisk;
        if (HardDisk.PartitionType == GPT) {
            BlXmiWrite(L" partition=");
            swprintf_s(TempString,
                       MAX_TEMP_STRING_CHARS,
                       GUID_FORMAT_STRING,
                       *(PULONG)GuidOffset,
                       *((PUSHORT)GuidOffset + 2),
                       *((PUSHORT)GuidOffset + 3),
                       *((PUCHAR)GuidOffset + 8),
                       *((PUCHAR)GuidOffset + 9),
                       *((PUCHAR)GuidOffset + 10),
                       *((PUCHAR)GuidOffset + 11),
                       *((PUCHAR)GuidOffset + 12),
                       *((PUCHAR)GuidOffset + 13),
                       *((PUCHAR)GuidOffset + 14),
                       *((PUCHAR)GuidOffset + 15));

            BlXmiWrite(TempString);
            BlXmiWrite(L" disk=");
            GuidOffset = (PUCHAR)&HardDisk.Gpt.Signature;
            swprintf_s(TempString,
                       MAX_TEMP_STRING_CHARS,
                       GUID_FORMAT_STRING,
                       *(PULONG)GuidOffset,
                       *((PUSHORT)GuidOffset + 2),
                       *((PUSHORT)GuidOffset + 3),
                       *((PUCHAR)GuidOffset + 8),
                       *((PUCHAR)GuidOffset + 9),
                       *((PUCHAR)GuidOffset + 10),
                       *((PUCHAR)GuidOffset + 11),
                       *((PUCHAR)GuidOffset + 12),
                       *((PUCHAR)GuidOffset + 13),
                       *((PUCHAR)GuidOffset + 14),
                       *((PUCHAR)GuidOffset + 15));

            BlXmiWrite(TempString);

       } else if (HardDisk.PartitionType == MBR) {
            BlXmiWrite(L" partition=");
            swprintf_s(TempString,
                       MAX_TEMP_STRING_CHARS,
                       L"'%x'",
                       PartitionNumber);

            BlXmiWrite(TempString);
            BlXmiWrite(L" disk=");
            swprintf_s(TempString,
                       MAX_TEMP_STRING_CHARS,
                       L"'%x'",
                       HardDisk.Mbr.Signature);

            BlXmiWrite(TempString);
        }
    }

    //
    // Output the existing load options string, or a blank string if there is
    // no current options string.
    //

    BlXmiWrite(L" options='");
    if (OptionsCopy != NULL) {
        Status = BlXmiTransformTextForXml(OptionsCopy, &OsxmlString);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(OsxmlString);
            BlMmFreeHeap(OsxmlString);

        } else {
            BlXmiSanitizeTextForXml(OptionsCopy);
            BlXmiWrite(OptionsCopy);
        }

        BlMmFreeHeap(OptionsCopy);

    } else {
        BlXmiWrite(L"");
    }

    BlXmiWrite(L"'/>");

    //
    // Log that the edit options menu was displayed to the user.
    //

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_EDIT_OPTIONS_MENU_INFORMATION,
                  NULL,
                  0,
                  NULL);

    //
    // Block until a result is posted.
    //

    Post = BlXmiRead(TRUE);
    if (Post == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto DisplayEditBootOptionsEnd;
    }

    //
    // If the user did not cancel editing boot options, update the boot options
    // with the control's contents.
    //
    // N.B. The result value will be freed when the post is released.
    //

    if (OsxmlResultHasAction(Post, L"cancel")) {
        Status = STATUS_CANCELLED;
        goto DisplayEditBootOptionsEnd;
    }

    ChoiceString = OsxmlResultGetValue(Post, L"default", L"bootop");
    if (ChoiceString == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto DisplayEditBootOptionsEnd;
    }

    //
    // Does policy indicate we need to filter the user's string?
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_ENABLE_EDITED_BOOT_OPTIONS_FILTERING) != FALSE) {

        Status = OslCheckForInvalidBootOptions(ChoiceString, TRUE);
        if (!NT_SUCCESS(Status)) {

            //
            // If an invalid parameter was found, just boot without updating
            // the options. If STATUS_INVALID_PARAMTER is returned the user
            // will go to WinRE, and if STATUS_CANCELLED is returned the user
            // will be returned to the legacy Boot Manager menu. Neither of
            // those seems acceptable.
            //

            if (Status == STATUS_INVALID_PARAMETER) {
                Status = STATUS_SUCCESS;
            }

            goto DisplayEditBootOptionsEnd;
        }
    }

    Status = OslUpdateBootOptions(ChoiceString);

DisplayEditBootOptionsEnd:

    //
    // Finally, process any tags for transitioning to different screens and
    // return to the caller.
    //

    if (Post != NULL) {
        OslpDisplayLoadNextScreen(Post);
        ObjRelease(Post);
    }

    return Status;
}

NTSTATUS
OslpDisplayAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount
    )

/*++

Routine Description:

    This function controls the advanced boot menu.

Arguments:

    ChoiceIndex - Supplies a pointer to a variable that receives the option
        chosen.

    SystemHiveHandle - Supplies a handle to the system hive.

    LastBootSucceeded - Supplies the last boot's status from bootstat.dat.

    LastBootShutdown - Supplies the last boot's shutdown status from
        bootstat.dat.

    LastBootCheckpoint - Supplies the last boot's checkpoint status from
        bootstat.dat.

    BootAttemptCount - Supplies the boot attempt count from the last boot.

Return Value:

    Returns STATUS_SUCCESS if menu entry was chosen.

    Returns STATUS_CANCELLED if menu was cancelled.

    Returns other error if menu was vetoed.

--*/

{

    BOOLEAN BootEmsEnabled;
    ULONGLONG BootMenuPolicy;
    PCUNICODE_STRING ChoiceStr;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    NTSTATUS Status;

    //
    // Determine if the UI can be displayed.  Only proceed if no component
    // vetos.
    //

    Status = OslpCheckPermission(
                 PermissionActionWinloadDisplayAdvancedBootMenu);

    if (!NT_SUCCESS(Status)) {
        *ChoiceIndex = BOOT_NORMAL;
        return STATUS_SUCCESS;
    }

    //
    // Determine if the Standard or Legacy menu should be shown.
    //

    BootMenuPolicy = MenuPolicyStandard;
    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_EMS_ENABLED,
                                           &BootEmsEnabled);

    if (NT_SUCCESS(Status) && (BootEmsEnabled != FALSE)) {
        BootMenuPolicy = MenuPolicyLegacy;

    } else {
        Status = BlGetApplicationOptionInteger(
                     BCDE_OSLOADER_TYPE_BOOT_MENU_POLICY,
                     &BootMenuPolicy);

        if (!NT_SUCCESS(Status)) {
            BootMenuPolicy = MenuPolicyLegacy;
        }
    }

    //
    // Check to make sure BGFX is initialized before using the Standard
    // display.
    //

    if ((BootMenuPolicy == MenuPolicyStandard) &&
        (OslpGraphicsModeEnabled == FALSE)) {

        BootMenuPolicy = MenuPolicyLegacy;
    }

    //
    // Display the menu.
    //

    if (BootMenuPolicy == MenuPolicyLegacy) {
        Status = OslpDisplayLegacyAdvancedBootMenu(ChoiceIndex,
                                                   SystemHiveHandle);

    } else {
        Status = OslpDisplayStandardAdvancedBootMenu(ChoiceIndex,
                                                     SystemHiveHandle,
                                                     LastBootSucceeded,
                                                     LastBootShutdown,
                                                     LastBootCheckpoint,
                                                     BootAttemptCount);
    }

    //
    // Log the user's choice for telemetry
    //

    ChoiceStr = &OslpAdvancedMenuChoice[min((ULONG)*ChoiceIndex,
                                            MAX_ADVANCED_BOOT_OPTION)];

    TraceLoggingWrite(OslBootProvider,
                      "OslAdvancedMenu",
                      TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                      TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY),
                      TraceLoggingBootPartB(OslApplicationIdentifier, OslBootId),
                      TraceLoggingString((BootMenuPolicy == MenuPolicyLegacy ? "Legacy" : "Standard"), "MenuPolicy"),
                      TraceLoggingUnicodeString(ChoiceStr, "Choice")
                      );

    //
    // Log the user's choice to ETW for SQM pick-up unless the menu failed.
    //

    if (NT_SUCCESS(Status)) {
        EventDataDescCreate(&EventDataDescriptor[0],
                            (PVOID)ChoiceIndex,
                            sizeof(LONG));

        BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                      &BOOT_ADVANCED_OPTIONS_MENU_INFORMATION,
                      NULL,
                      1,
                      EventDataDescriptor);
    }

    return Status;
}

BOOLEAN
OslpDisplayInitializeBgfx (
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine initializes the BGFX library to be used by the OS loader.

Arguments:

    Flags - Supplies the flags to be used when initializing the BGFX library.

Return Value:

    TRUE if BGFX initialized successfully. FALSE otherwise.

--*/

{

    BOOT_GRAPHICS_LIBRARY_PARAMETERS BgParameters;
    PWSTR DefaultLocale;
    PWSTR Locale;
    PWSTR RootDirectory;
    SIZE_T StringSize;
    NTSTATUS Status;

    DefaultLocale = DEFAULT_LOCALE;
    Locale = NULL;
    RootDirectory = NULL;

    if (OslpGraphicsModeEnabled != FALSE) {
        return OslpGraphicsModeEnabled;
    }

    //
    // Initialize the Boot Graphics Library (BGFX).
    //
    // N.B. If there is no locale BCD setting, fall back to default: en-US.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_PREFERRED_LOCALE,
                                          &Locale);

    if (!NT_SUCCESS(Status)) {
        Locale = DefaultLocale;
        Status = STATUS_SUCCESS;
    }

    //
    // Allocate memory to hold the system root path. In the loader, the path
    // has a trailing backslash which must be removed before calling into the
    // boot graphics library.
    //

    StringSize = wcslen(OslSystemRoot) * sizeof(WCHAR);
    RootDirectory = BlMmAllocateHeap(StringSize);
    if (RootDirectory == NULL) {
        goto DisplayInitializeBgfxEnd;
    }

    RtlZeroMemory(RootDirectory, StringSize);

    //
    // Copy the system root path without the termininating backslash.
    //

    RtlCopyMemory(RootDirectory, OslSystemRoot, StringSize - sizeof(WCHAR));
    BgParameters.Application = BlGetApplicationIdentifier();
    BgParameters.Device = OslLoadDevice;
    BgParameters.RootDirectory = RootDirectory;
    BgParameters.Locale = Locale;
    BgParameters.Flags = Flags;
    Status = BgLibraryInitialize(&BgParameters, 0);
    if (!NT_SUCCESS(Status)) {
        goto DisplayInitializeBgfxEnd;
    }

    OslpGraphicsModeEnabled = TRUE;

DisplayInitializeBgfxEnd:
    if (RootDirectory != NULL) {
        BlMmFreeHeap(RootDirectory);
    }

    if ((Locale != NULL) && (Locale != DefaultLocale)) {
        BlMmFreeHeap(Locale);
    }

    return OslpGraphicsModeEnabled;
}

BOOLEAN
OslpDisplayIsKeyboardLessSystem (
    VOID
    )

/*++

Routine Description:

    This routine determines if the current system typically has a keyboard or
    not. This is defined in ACPI or can be overridden with the BCD store.

Arguments:

    None.

Return Value:

    TRUE if the system typically doesn't have a keyboard. FALSE, otherwise.

--*/

{

    PFADT Fadt;
    BOOLEAN NoKeyboard;
    NTSTATUS Status;

    //
    // Check the BCD override.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_FORCE_NO_KEYBOARD,
                                           &NoKeyboard);

    if (NT_SUCCESS(Status) && (NoKeyboard != FALSE)) {
        goto IsKeyboardLessSystemEnd;
    }

    //
    // Check the FADT for the power platform role.
    //

    Status = BlUtlGetAcpiTable(&Fadt, FADT_SIGNATURE);
    NoKeyboard = FALSE;
    if (!NT_SUCCESS(Status)) {
        goto IsKeyboardLessSystemEnd;
    }

    //
    // The slate power platform role is the only role that does not have a
    // keyboard.
    //

    if (Fadt->pm_profile == PlatformRoleSlate) {
        NoKeyboard = TRUE;
    }

IsKeyboardLessSystemEnd:
    return NoKeyboard;
}

NTSTATUS
OslpDisplayLegacyAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This function controls the advanced boot menu.

Arguments:

    ChoiceIndex - Supplies a pointer to a variable that receives the option
        chosen.

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    Returns STATUS_SUCCESS if menu entry was chosen.

    Returns STATUS_CANCELLED if menu was cancelled.

    Returns other error if menu was vetoed.

--*/

{

    PCWSTR ChoiceString;
    PWCHAR FriendlyName;
    PWCHAR OsxmlString;
    PXML_DOC Post;
    NTSTATUS Status;

    //
    // Obtain the friendly name for the entry being displayed.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_DESCRIPTION,
                                          &FriendlyName);

    if (NT_SUCCESS(Status)) {
        BlXmiWrite(L"<osload-advops-legacy name='");
        Status = BlXmiTransformTextForXml(FriendlyName, &OsxmlString);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(OsxmlString);
            BlMmFreeHeap(OsxmlString);

        } else {
            BlXmiSanitizeTextForXml(FriendlyName);
            BlXmiWrite(FriendlyName);
        }

        BlXmiWrite(L"'");
        BlMmFreeHeap(FriendlyName);

    } else {
        BlXmiWrite(L"<osload-advops-legacy");
    }

    //
    // Add an attribute to display the recovery sequence if appropriate.
    //

    if (BlApplicationOptionExists(BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE)) {
        BlXmiWrite(L" recovery='true'");
    }

    //
    // Only display LKG option if enabled in the system hive where the
    // following key is set to 1:
    //
    // HKLM\SYSTEM\CurrentControlSet\Control\SessionManager\Configuration
    // Manager\LastKnownGood\Enabled
    //

    if (OslHiveIsLkgEnabled(SystemHiveHandle) != FALSE) {
        BlXmiWrite(L" lkg='true'");
    }

    //
    // Only display 'Directory Services Repair Mode' option if the system is a
    // domain controller.
    //

    if (OslpIsDomainController(SystemHiveHandle) != FALSE) {
        BlXmiWrite(L" dsrm='true'");
    }

    //
    // Do not display safe mode options on ARM systems.
    //

#pragma prefast ( suppress: __WARNING_CONST_CONST_COMP, "Intentionally const" )
    if (BOOT_APPLICATION_MACHINE_TYPE != IMAGE_FILE_MACHINE_ARMNT) {
        BlXmiWrite(L" safemode='true'");
    }

    //
    // Check that the code integrity options are allowed.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_DISABLE_DRIVER_SIGNATURE_ENFORCEMENT_MENU) == FALSE) {

        BlXmiWrite(L" ci='true'");
    }

    //
    // Check that the debugger option is allowed.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_DISABLE_DEBUGGING_MODE_MENU) == FALSE) {

        BlXmiWrite(L" kd='true'");
    }

    BlXmiWrite(L"/>");

    //
    // Get the output from the menu display.
    //

    *ChoiceIndex = -1;
    Post = BlXmiRead(TRUE);
    if (Post == NULL) {
        return STATUS_CANCELLED;
    }

    if (OsxmlResultHasAction(Post, L"cancel") == FALSE) {
        ChoiceString = OsxmlResultGetValue(Post, L"default", L"advop");
        if (ChoiceString != NULL) {
            *ChoiceIndex = _wtoi(ChoiceString);
        }
    }

    //
    // Finally, process any tags for transitioning to different screens and
    // return to the caller.
    //

    OslpDisplayLoadNextScreen(Post);
    ObjRelease(Post);
    if (*ChoiceIndex == -1) {
        return STATUS_CANCELLED;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpDisplayStandardAdvancedBootMenu (
    __out PLONG ChoiceIndex,
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount
    )

/*++

Routine Description:

    This function controls the advanced boot menu.

Arguments:

    ChoiceIndex - Supplies a pointer to a variable that receives the option
        chosen.

    SystemHiveHandle - Supplies a handle to the system hive.

    LastBootSucceeded - Supplies the last boot's status from bootstat.dat.

    LastBootShutdown - Supplies the last boot's shutdown status from
        bootstat.dat.

    LastBootCheckpoint - Supplies the last boot's checkpoint status from
        bootstat.dat.

    BootAttemptCount - Supplies the boot attempt count from the last boot.

Return Value:

    Returns STATUS_SUCCESS if menu entry was chosen.

    Returns STATUS_CANCELLED if menu was cancelled.

    Returns other error if menu was vetoed.

--*/

{

    PWCHAR BodyString;
    ULONG CurrentIndex;
    PWCHAR Destination;
    ULONG EndIndex;
    PWCHAR FooterString;
    PWCHAR HeaderString;
    ULONG Index;
    ULONG LocalIndex;
    ULONG MessageIdStart;
    BOOT_OPTION Options[MAX_ADVANCED_BOOT_OPTION];
    PWCHAR OptionKeys[MAX_VISIBLE_OPTIONS];
    PXML_DOC Post;
    ULONG SelectedIndex;
    BOOLEAN ShutdownDisabled;
    ULONG StartIndex;
    NTSTATUS Status;
    PWCHAR String;
    PWCHAR TitleString;
    ULONG TotalOptions;

    //
    // Force any deferred fonts to get loaded. If loading of the fonts fails
    // the menu will not display.
    //

    Status = BlResourceForceLoadFonts();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Unless there is an error, the user is going to have to provide input, so
    // initialize the key strokes before drawing the screen.
    //

    BlDisplayInitializeKeyStrokes();

    //
    // Initialize the option key text.
    //

    MessageIdStart = OSL_ADVANCED_OPTIONS_ONE;
    for (Index = 0; Index < MAX_VISIBLE_OPTIONS; Index += 1) {
        OptionKeys[Index] = BlResourceFindMessage(MessageIdStart + Index);
    }

    //
    // Initialize the advanced boot options available on this system.
    //

    TotalOptions = 0;

    //
    // Only display 'Directory Services Repair Mode' option if the system is a
    // domain controller.
    //

    if (OslpIsDomainController(SystemHiveHandle) != FALSE) {
        Options[TotalOptions].BootOptionIndex = DS_REPAIR_MODE;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_DS_REPAIR_MODE);

        TotalOptions += 1;
    }

    //
    // Check that the debugger option is allowed.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_DISABLE_DEBUGGING_MODE_MENU) == FALSE) {

        Options[TotalOptions].BootOptionIndex = DEBUG_MODE;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_DEBUG_MODE);

        TotalOptions += 1;
    }

    //
    // Boot logging is always present.
    //

    Options[TotalOptions].BootOptionIndex = BOOT_LOGGING;
    Options[TotalOptions].BootOptionString =
        BlResourceFindMessage(OSL_ADVANCED_OPTIONS_BOOT_LOGGING);

    TotalOptions += 1;

    //
    // Low resolution video mode is always present.
    //

    Options[TotalOptions].BootOptionIndex = BASICDISPLAY_MODE;
    Options[TotalOptions].BootOptionString =
        BlResourceFindMessage(OSL_ADVANCED_OPTIONS_BASICDISPLAY_MODE);

    TotalOptions += 1;

    //
    // Only display LKG option if enabled in the system hive where the
    // following key is set to 1:
    //
    // HKLM\SYSTEM\CurrentControlSet\Control\SessionManager\Configuration
    // Manager\LastKnownGood\Enabled
    //

    if (OslHiveIsLkgEnabled(SystemHiveHandle) != FALSE) {
        Options[TotalOptions].BootOptionIndex = LAST_KNOWN_GOOD;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_LAST_KNOWN_GOOD);

        TotalOptions += 1;
    }

    //
    // Do not display safe mode options on ARM systems.
    //

#pragma prefast ( suppress: __WARNING_CONST_CONST_COMP, "Intentionally const" )
    if (BOOT_APPLICATION_MACHINE_TYPE != IMAGE_FILE_MACHINE_ARMNT) {
        Options[TotalOptions].BootOptionIndex = SAFEMODE;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_SAFEMODE);

        TotalOptions += 1;
        Options[TotalOptions].BootOptionIndex = SAFEMODE_NET;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_SAFEMODE_NET);

        TotalOptions += 1;
        Options[TotalOptions].BootOptionIndex = SAFEMODE_CMD;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_SAFEMODE_CMD);

        TotalOptions += 1;
    }

    //
    // Check that the code integrity options are allowed.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_DISABLE_DRIVER_SIGNATURE_ENFORCEMENT_MENU) == FALSE) {

        Options[TotalOptions].BootOptionIndex = DISABLE_CODE_INTEGRITY;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_DISABLE_CODE_INTEGRITY);

        TotalOptions += 1;
    }

    //
    // Disabling the ELAM driver is always present.
    //

    Options[TotalOptions].BootOptionIndex = DISABLE_ELAM_DRIVERS;
    Options[TotalOptions].BootOptionString =
        BlResourceFindMessage(OSL_ADVANCED_OPTIONS_DISABLE_ELAM_DRIVERS);

    TotalOptions += 1;

    //
    // Disabling auto reboot on bugcheck is always present.
    //

    Options[TotalOptions].BootOptionIndex = DISABLE_CRASH_AUTOREBOOT;
    Options[TotalOptions].BootOptionString =
        BlResourceFindMessage(OSL_ADVANCED_OPTIONS_DISABLE_CRASH_AUTOREBOOT);

    TotalOptions += 1;

    //
    // Add an option to boot to recovery if a recovery sequence exists.
    //

    if (BlApplicationOptionExists(BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE)) {
        Options[TotalOptions].BootOptionIndex = RECOVERY_SEQUENCE;
        Options[TotalOptions].BootOptionString =
            BlResourceFindMessage(OSL_ADVANCED_OPTIONS_RECOVERY_SEQUENCE);

        TotalOptions += 1;
    }

    //
    // Initialize the XML template to read the correct key strokes. If the
    // shutdown timer is enabled, set it at 60 seconds.
    //

    BlXmiWrite(L"<osload-advops-standard");
    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_BOOT_SHUTDOWN_DISABLED,
                 &ShutdownDisabled);

    if (!NT_SUCCESS(Status) || (ShutdownDisabled == FALSE)) {
        BlXmiWrite(L" timeout='60'");
    }

    BlXmiWrite(L"/>");

    //
    // Loop continually at the screen until the user selects an option of the
    // screen times out.
    //

    StartIndex = 0;
    SelectedIndex = MAX_ADVANCED_BOOT_OPTION;
    while (TRUE) {
        Status = OslBgDisplayInitialize();
        if (!NT_SUCCESS(Status)) {
            *ChoiceIndex = BOOT_NORMAL;
            Status = STATUS_SUCCESS;
            goto StandardAdvancedBootMenuEnd;
        }

        //
        // Set the main header text.
        //

        HeaderString = BlResourceFindMessage(OSL_ADVANCED_OPTIONS_HEADER);
        OslBgDisplaySetHeader(HeaderString);

        //
        // Establish the title text that tells the user what to do.
        //

        TitleString = BlResourceFindMessage(OSL_ADVANCED_OPTIONS_TITLE);
        OslBgDisplaySetTitle(TitleString);

        //
        // Help Text is small.
        //

        BodyString = BlResourceFindMessage(OSL_ADVANCED_OPTIONS_HELP_TEXT);
        OslBgDisplayAddBodyRegion(BodyString,
                                  OSL_BGFX_FLAGS_BODY_TEXT_SMALL,
                                  NULL);

        OslBgDisplayAddBodyRegion(NULL, OSL_BGFX_FLAGS_BODY_TEXT_LARGE, NULL);

        //
        // Display the boot options in focus.
        //

        EndIndex = StartIndex + MAX_VISIBLE_OPTIONS;
        for (CurrentIndex = StartIndex, LocalIndex = 0;
             (CurrentIndex < EndIndex) && (CurrentIndex < TotalOptions);
             CurrentIndex += 1, LocalIndex += 1) {

            Status = BlConcatenateStringToString(OptionKeys[LocalIndex],
                                                 L" ",
                                                 &Destination);

            if (!NT_SUCCESS(Status)) {
                *ChoiceIndex = BOOT_NORMAL;
                Status = STATUS_SUCCESS;
                goto StandardAdvancedBootMenuEnd;
            }

            String = Destination;
            Status = BlConcatenateStringToString(
                         String,
                         Options[CurrentIndex].BootOptionString,
                         &Destination);

            BlMmFreeHeap(String);
            if (!NT_SUCCESS(Status)) {
                *ChoiceIndex = BOOT_NORMAL;
                Status = STATUS_SUCCESS;
                goto StandardAdvancedBootMenuEnd;
            }

            OslBgDisplayAddBodyRegion(Destination,
                                      OSL_BGFX_FLAGS_BODY_TEXT_LARGE,
                                      NULL);
        }

        //
        // Add footer text, adding a key press to display any hidden options.
        //

        if (OslpDisplayIsKeyboardLessSystem() != FALSE) {
            FooterString = BlResourceFindMessage(
                               OSL_ADVANCED_OPTIONS_WIN_START_NORMALLY);

        } else {
            FooterString = BlResourceFindMessage(
                               OSL_ADVANCED_OPTIONS_START_NORMALLY);
        }

        OslBgDisplayAddFooterRegion(FooterString);
        if (TotalOptions > MAX_VISIBLE_OPTIONS) {
            FooterString = BlResourceFindMessage(
                               OSL_ADVANCED_OPTIONS_MORE_OPTIONS);
            OslBgDisplayAddFooterRegion(FooterString);
        }

        //
        // Render the screen.
        //

        OslBgDisplayRender();

        //
        // Collect input from the user. This should return if the timer expires
        // or if a key is pressed.
        //

        Post = BlXmiRead(TRUE);
        if (Post == NULL) {
            *ChoiceIndex = BOOT_NORMAL;
            Status = STATUS_SUCCESS;
            goto StandardAdvancedBootMenuEnd;
        }

        //
        // Reset the boot status information and shutdown if the timer expired.
        //

        if (OsxmlResultHasAction(Post, L"shutdown") != FALSE) {
            OslSetBootStatusData(LastBootSucceeded,
                                 LastBootShutdown,
                                 LastBootCheckpoint,
                                 OslBootCountUpdateDecrement,
                                 (BootAttemptCount - 1));

            BlFwShutdown();
        }

        Status = STATUS_UNSUCCESSFUL;
        if (OsxmlResultHasAction(Post, L"cancel") != FALSE) {
            Status = STATUS_CANCELLED;

        } else if (OsxmlResultHasAction(Post, L"continue") != FALSE) {
            *ChoiceIndex = BOOT_NORMAL;
            Status = STATUS_SUCCESS;

        } else if ((TotalOptions > MAX_VISIBLE_OPTIONS) &&
                   (OsxmlResultHasAction(Post, L"more_options") != FALSE)) {

            BlXmiWrite(L"<timeout-cancel/>");
            if (StartIndex == MAX_VISIBLE_OPTIONS) {
                StartIndex = 0;

            } else {
                StartIndex = MAX_VISIBLE_OPTIONS;
            }

        } else if (OsxmlResultHasAction(Post, L"option_one") != FALSE) {
            SelectedIndex = StartIndex;

        } else if (OsxmlResultHasAction(Post, L"option_two") != FALSE) {
            SelectedIndex = StartIndex + 1;

        } else if (OsxmlResultHasAction(Post, L"option_three") != FALSE) {
            SelectedIndex = StartIndex + 2;

        } else if (OsxmlResultHasAction(Post, L"option_four") != FALSE) {
            SelectedIndex = StartIndex + 3;

        } else if (OsxmlResultHasAction(Post, L"option_five") != FALSE) {
            SelectedIndex = StartIndex + 4;

        } else if (OsxmlResultHasAction(Post, L"option_six") != FALSE) {
            SelectedIndex = StartIndex + 5;

        } else if (OsxmlResultHasAction(Post, L"option_seven") != FALSE) {
            SelectedIndex = StartIndex + 6;

        } else if (OsxmlResultHasAction(Post, L"option_eight") != FALSE) {
            SelectedIndex = StartIndex + 7;

        } else if (OsxmlResultHasAction(Post, L"option_nine") != FALSE) {
            SelectedIndex = StartIndex + 8;
        }

        ObjRelease(Post);
        if (SelectedIndex < TotalOptions) {
            *ChoiceIndex = Options[SelectedIndex].BootOptionIndex;
            Status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(Status) || (Status == STATUS_CANCELLED)) {
            goto StandardAdvancedBootMenuEnd;
        }
    }

StandardAdvancedBootMenuEnd:

    //
    // Destroy the screen, load the default XML template and return.
    //

    OslBgDisplayDestroy();
    BlXmiWrite(L"<osload-main/>");
    return Status;
}

NTSTATUS
OslpAppendAdvancedOptions (
    __in ULONG AdvancedOption
    )

/*++

Routine Description:

    This routine adds application options as specified by a user-selected
    advanced boot option.

Arguments:

    AdvancedOption - Supplies the index of the selected advanced boot option.
        Must match the option list in the application style sheet.

Return Value:

    NT Status code.

--*/

{

    PBCDE_BOOLEAN BooleanData;
    ULONG BufferSize;
    PVOID Data;
    BCDE_DATA_TYPE IgnoreOption;
    ULONG Index;
    PBCDE_INTEGER IntegerData;
    PBOOT_ENTRY_OPTION Option;
    ULONG OptionCount;
    PBOOT_ENTRY_OPTION OptionList;
    ULONG OptionOffset;
    NTSTATUS Status;

#define MAXIMUM_OPTION_COUNT        5
#define MAXIMUM_OPTION_DATA_LENGTH  0x20

    struct {
        BCDE_DATA_TYPE Type;
        ULONG DataSize;
        UCHAR Data[MAXIMUM_OPTION_DATA_LENGTH];
    } OptionInfo[MAXIMUM_OPTION_COUNT];

    IgnoreOption = 0;
    OptionList = NULL;

    //
    // Populate the array of option descriptions with the options to add.
    //

    switch (AdvancedOption) {

        //
        // Safe mode case.  This case receives a safe boot option, kernel init
        // logging, and a disable of the boot GUI display.  The safe boot
        // choice to launch a cmd shell additionally adds the alternate shell
        // option.
        //

    case SAFEMODE:
    case SAFEMODE_NET:
    case SAFEMODE_CMD:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_SAFEBOOT;
        OptionInfo[0].DataSize = sizeof(BCDE_INTEGER);
        IntegerData = (PBCDE_INTEGER)OptionInfo[0].Data;
        IntegerData->Value = 0;
        if ((AdvancedOption == SAFEMODE) || (AdvancedOption == SAFEMODE_CMD)) {
            *(PBCDE_SAFEBOOT_TYPE)&IntegerData->Value = SafemodeMinimal;

        } else {
            *(PBCDE_SAFEBOOT_TYPE)&IntegerData->Value = SafemodeNetwork;
        }

        OptionInfo[1].Type = BCDE_OSLOADER_TYPE_LOG_INITIALIZATION;
        OptionInfo[1].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[2].Data;
        BooleanData->Value = TRUE;
        OptionInfo[2].Type = BCDE_OSLOADER_TYPE_DISABLE_BOOT_DISPLAY;
        OptionInfo[2].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[3].Data;
        BooleanData->Value = TRUE;
        OptionInfo[3].Type = BCDE_OSLOADER_TYPE_TPM_BOOT_ENTROPY_POLICY;
        OptionInfo[3].DataSize = sizeof(BCDE_INTEGER);
        IntegerData = (PBCDE_INTEGER)OptionInfo[4].Data;
        IntegerData->Value = TpmBootEntropyPolicyForceDisable;

        //
        // Append the alternate shell option for the CMD safe mode choice, and
        // set the option count appropriately.
        //

        if (AdvancedOption == SAFEMODE_CMD) {
            OptionInfo[4].Type = BCDE_OSLOADER_TYPE_SAFEBOOT_ALTERNATE_SHELL;
            OptionInfo[4].DataSize = sizeof(BCDE_BOOLEAN);
            BooleanData = (PBCDE_BOOLEAN)OptionInfo[4].Data;
            BooleanData->Value = TRUE;
            OptionCount = 5;

        } else {
            OptionCount = 4;
        }

        break;

        //
        // Boot logging case.  Enable kernel initialization to log.
        //

    case BOOT_LOGGING:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_LOG_INITIALIZATION;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

        //
        // BasicDisplay mode case. Indicate that the OS should use the
        // fallback display driver.
        //

    case BASICDISPLAY_MODE:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_USE_VGA_DRIVER;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

        //
        // Last known good case.  Record that the last good settings are to be
        // applied to the booting OS.
        //

    case LAST_KNOWN_GOOD:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_LAST_GOOD_SETTINGS;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

        //
        // DS repair mode case.  Set a safe mode option for DS repair.
        //

    case DS_REPAIR_MODE:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_SAFEBOOT;
        OptionInfo[0].DataSize = sizeof(BCDE_INTEGER);
        IntegerData = (PBCDE_INTEGER)OptionInfo[0].Data;
        IntegerData->Value = 0;
        *(PBCDE_SAFEBOOT_TYPE)&IntegerData->Value = SafemodeDsRepair;
        OptionCount = 1;
        break;

        //
        // Debug mode case.  Enable kernel debugging.
        //

    case DEBUG_MODE:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        IgnoreOption = OptionInfo[0].Type;
        break;

        //
        // Crash auto-reboot disable case.  Record that auto-reboot on a
        // bugcheck is to be disabled for this boot.
        //

    case DISABLE_CRASH_AUTOREBOOT:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_DISABLE_CRASH_AUTOREBOOT;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

        //
        // Code integrity disable case.  Record that driver integrity checks
        // are to be disabled for this boot.
        //

    case DISABLE_CODE_INTEGRITY:
        OptionInfo[0].Type = BCDE_LIBRARY_TYPE_DISABLE_INTEGRITY_CHECKS;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        IgnoreOption = OptionInfo[0].Type;
        break;

        //
        // Disable GUI boot case.  Set the /NOGUIBOOT and /BOOTLOGO
        // OS options.
        //

    case NOGUIBOOT:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_DISABLE_BOOT_DISPLAY;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

    case DISABLE_ELAM_DRIVERS:
        OptionInfo[0].Type = BCDE_OSLOADER_TYPE_DISABLE_ELAM_DRIVERS;
        OptionInfo[0].DataSize = sizeof(BCDE_BOOLEAN);
        BooleanData = (PBCDE_BOOLEAN)OptionInfo[0].Data;
        BooleanData->Value = TRUE;
        OptionCount = 1;
        break;

    default:
        return STATUS_SUCCESS;
    }

    ASSERT(OptionCount != 0);
    ASSERT(OptionCount <= MAXIMUM_OPTION_COUNT);

    //
    // Check if it is OK to modify boot options.
    //

    Status = OslpCheckPermission(PermissionActionWinloadAppendAdvancedOptions);
    if (!NT_SUCCESS(Status)) {
        goto OslpAppendAdvancedOptionsEnd;
    }

    //
    // Since the policy has granted permission to set the advanced
    // options, let the user overrule policy for this boot.
    //

    if (IgnoreOption != 0 && BlSecureBootIsPolicyActive() != FALSE) {
        BlSecureBootIgnoreSingleBootOption(IgnoreOption);
    }

    //
    // Determine the appropriate buffer size and allocate space for the options.
    //

    BufferSize = 0;
    for (Index = 0; Index < OptionCount; Index += 1) {

        ASSERT(OptionInfo[Index].DataSize <= MAXIMUM_OPTION_DATA_LENGTH);

        MARK_SAFE_ADDITION(BufferSize, sizeof(BOOT_ENTRY_OPTION));
        BufferSize += sizeof(BOOT_ENTRY_OPTION);
        MARK_SAFE_ADDITION(BufferSize, OptionInfo[Index].DataSize);
        BufferSize += OptionInfo[Index].DataSize;
    }

    OptionList = BlMmAllocateHeap(BufferSize);
    if (OptionList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpAppendAdvancedOptionsEnd;
    }

    RtlZeroMemory(OptionList, BufferSize);

    //
    // Fill in the options in a format that can be passed to the library.
    //

    Option = OptionList;
    OptionOffset = 0;
    for (Index = 0; Index < OptionCount; Index += 1) {
        Option = Add2Ptr(OptionList, OptionOffset);

        //
        // Fill in the option description with the type and data size.
        //

        Option->Type = OptionInfo[Index].Type;
        Option->DataSize = OptionInfo[Index].DataSize;

        //
        // Fill in the data for the option.
        //

        Data = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
        RtlCopyMemory(Data, OptionInfo[Index].Data, Option->DataSize);

        //
        // Finally, fill in the offsets to the data and next element, and
        // iterate.  The data offset is relative to the beginning of the option,
        // while the next option offset is relative to the beginning of the
        // option list.
        //

        Option->DataOffset = (ULONG)PtrOffset(Option, Data);
        OptionOffset += (Option->DataOffset + Option->DataSize);
        Option->NextOptionOffset = OptionOffset;
    }

    //
    // The last option in a list must have a next offset of zero to terminate
    // the list.
    //

    Option->NextOptionOffset = 0;

    //
    // Update the options in the current application options.
    //

    Status = BlUpdateApplicationOptions(OptionList);
    if (!NT_SUCCESS(Status)) {
        goto OslpAppendAdvancedOptionsEnd;
    }

    //
    // Convert the new application options list to a load options string,
    // freeing the existing string.
    //

    if (OslLoadOptions != NULL) {
        BlMmFreeHeap(OslLoadOptions);
    }

    Status = AhCreateLoadOptionsString(BlGetApplicationEntry()->Options,
                                       &OslLoadOptions);

OslpAppendAdvancedOptionsEnd:
    if (OptionList != NULL) {
        BlMmFreeHeap(OptionList);
    }

    return Status;
}

VOID
OslpDisplayLoadNextScreen (
    __in PXML_DOC Post
    )

/*++

Routine Description:

    This routine processes XML input, looking for a tag that indicates the
    next tag to be output.  This allows an XSL template to provide an action
    that generically specifies the next template to be matched.

Arguments:

    Post - Supplies the XML document to be parsed.

Return Value:

    None.

--*/

{

    //
    // Clear the previous screen by loading a blank document.
    //

    BlXmiWrite(L"<osload-end/>");

    //
    // At this point, the only supported transition is back to the main
    // loader template.  If the document contains the "reload main" action,
    // make this transition.
    //

    if (OsxmlResultHasAction(Post, L"reload-main") != FALSE) {
        BlXmiWrite(L"<osload-main/>");
    }

    return;
}

NTSTATUS
OslpCheckPermission (
    __in PERMISSION_ACTION_TYPE ActionRequested
    )

/*++

Routine Description:

    This routine calls all veto routines to check if the action is permitted.

Arguments:

    ActionRequested - The action that permission is being requested for.

Return Value:

    STATUS_SUCCESS when the action is allowed, this is the typical case.

    STATUS_RESTART_BOOT_APPLICATION to cause the application to be restarted.
        In this case, the veto function must ensure that a second call through
        of the application will not return STATUS_RESTART_BOOT_APPLICATION.

    Other errors may be returned which will prevent the action from occurring.

--*/

{

    NTSTATUS Status;

    //
    // Check if secure boot grants this permission.
    //

    Status = BlSecureBootDoesActivePolicyGrantPermission(ActionRequested);
    if (!NT_SUCCESS(Status)) {
        goto CheckPermissionEnd;
    }

    //
    // Check if SIPolicy grants this permission
    //

    Status = BlSIPolicyDoesActivePolicyGrantPermission(ActionRequested);
    if (!NT_SUCCESS(Status)) {
        goto CheckPermissionEnd;
    }

    //
    // Check if FVE grants this permission.
    //

    Status = BlFveCheckPermission();
CheckPermissionEnd:
    return Status;
}

BOOLEAN
OslpIsDomainController (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine determines if the system is a domain controller. It does so by
    checking the SKU type that is stored in the registry.

Arguments:

    SystemHiveHandle - Supplies a handle to the system registry hive.

Return Value:

    TRUE if the system is a domain controller. FALSE otherwise.

--*/

{

    NTSTATUS Status;
    UNICODE_STRING SkuType;

    Status = OslHiveFindSkuType(SystemHiveHandle, &SkuType);
    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    return RtlEqualUnicodeString(&SkuType, &OslpDomainControllerSkuType, TRUE);
}

