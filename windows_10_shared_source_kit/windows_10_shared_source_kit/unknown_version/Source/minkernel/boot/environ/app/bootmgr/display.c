/*++

Copyright (c) Microsoft Corporation

Module Name:

    display.c

Abstract:

    User Interface for the boot manager.  Provides the infrastructure for the
    Boot Menu.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"

//
// ---------------------------------------------------------------- Definitions
//

#define MAX_ERROR_STRING 256
#define INVALID_CHOICE_INDEX MAXULONG
#define DEFAULT_LOCALE L"en-US"

//
// -------------------------------------------------- Local Function Prototypes
//

VOID
BmpOutputApplicationEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG EntryIndex,
    __in BOOLEAN EntryIsTool
    );

VOID
BmpDisplayAddServerAddress (
    VOID
    );

VOID
BmpDisplayDumpErrorLegacy (
    __in ULONG Phase,
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in PINTERNAL_BOOT_ERROR ErrorLog,
    __in BOOLEAN MultiBootSystem,
    __in BOOLEAN RecoveryAvailable,
    __in BOOLEAN OemRecoveryAvailable,
    __in BOOLEAN NoKeyboard
    );

VOID
BmpDisplayDumpErrorStandard (
    __in ULONG Phase,
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in PINTERNAL_BOOT_ERROR ErrorLog,
    __in ULONG LaunchReason,
    __in BOOLEAN MultiBootSystem,
    __in BOOLEAN RecoveryAvailable,
    __in BOOLEAN OemRecoveryAvailable,
    __in BOOLEAN NoKeyboard
    );

VOID
BmpDisplayErrorSimple (
    __in ULONG Phase
    );

PWCHAR
BmpDisplayGetProgressMessage (
    VOID
    );

BCDE_BOOTUX_MESSAGE_TYPE
BmpDisplayGetProgressMessageType (
    VOID
    );

VOID
BmpDisplayLegacyMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN Display
    );

VOID
BmpDisplayStandardMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN Display
    );

NTSTATUS
BmpUpdateLegacyMulticastMessage (
    __in PVOID Parameter
    );

VOID
BmpUpdateLegacyProgress (
    __in PVOID Context,
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    );

VOID
BmpUpdateLegacyProgressInformation (
    __in PVOID Context,
    __in PWSTR ProgressInformation
    );

NTSTATUS
BmpUpdateStandardMulticastMessage (
    __in PVOID Parameter
    );

VOID
BmpUpdateStandardProgress (
    __in PVOID Context,
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    );

VOID
BmpUpdateStandardProgressInformation (
    __in PVOID Context,
    __in PWSTR ProgressInformation
    );

//
// -------------------------------------------------------------------- Globals
//

//
// When TRUE, forces the OS Boot selection menu.
//

BOOLEAN BmForceOsBootList = FALSE;

//
// Preserves the current OS selection.
//

GUID BmDefaultObject;

//
//---------------------------------------------------------------------- Locals
//

static LPWSTR BmSpinner[] = { L"-", L"\\", L"|", L"/" };
static ULONG BmSpinnerIndex = 0;
static BOOLEAN BmGraphicsInitialized = FALSE;

//
// ------------------------------------------------------------------ Functions
//

BOOLEAN
BmDisplayBootGraphics (
    VOID
    )

/*++

Routine Description:

    This routine attempts to initialize the boot graphics library subsystem,
    and returns its result.

Arguments:

    None.

Return Value:

    TRUE if boot graphics was successfully initialized, else FALSE.

--*/

{

    BOOT_GRAPHICS_LIBRARY_PARAMETERS BgParameters;
    PWSTR Locale;
    PWSTR RootDirectory;
    BOOLEAN RootDirectoryAllocated;
    NTSTATUS Status;

    if (BmGraphicsInitialized != FALSE) {
        return BmGraphicsInitialized;
    }

    //
    // Create and initialize the boot graphics library parameters.
    //

    RootDirectory = NULL;
    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_PREFERRED_LOCALE,
                                          &Locale);

    if (!NT_SUCCESS(Status)) {
        Locale = DEFAULT_LOCALE;
    }

    //
    // Call BGFX initialize with the desired parameters.
    //

    RootDirectory = BmFwGetBgfxRootDirectory(&RootDirectoryAllocated);
    BgParameters.Application = BlGetApplicationIdentifier();
    BgParameters.Device = BlGetBootDevice();
    BgParameters.RootDirectory = RootDirectory;
    BgParameters.Locale = Locale;
    BgParameters.Flags = BGFX_FLAGS_NONE;
    Status = BgLibraryInitialize(&BgParameters, 0);
    if (!NT_SUCCESS(Status)) {
        goto DisplayBootGraphicsEnd;
    }

    BmGraphicsInitialized = TRUE;

DisplayBootGraphicsEnd:
    if ((RootDirectoryAllocated != FALSE) && (RootDirectory != NULL)) {
        BlMmFreeHeap(RootDirectory);
    }

    return BmGraphicsInitialized;
}

NTSTATUS
BmDisplayBootMenu (
    __in HANDLE DataStoreHandle,
    __in_ecount(NumBootEntries) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __in ULONG NumBootEntries,
    __in_ecount(NumToolEntries) PBOOT_APPLICATION_ENTRY *ToolEntryList,
    __in ULONG NumToolEntries,
    __deref_out PBOOT_APPLICATION_ENTRY *SelectedBootEntry,
    __out PBOOLEAN ReturnToFirmware
    )

/*++

Routine Description:

    Displays the Boot Menu, presenting the user with a list of boot entries to
    select from.

Arguments:

    DataStoreHandle - Supplies a handle to the boot configuration data store
        containing the boot entries.

    BootEntryList - Supplies a list of boot entries.

    NumBootEntries - Supplies the number of entries in the boot entry list.

    ToolEntryList - Supplies a list of tool application entries.

    NumToolEntries - Supplies the number of entries in the tool entry list.

    SelectedBootEntry - Supplies a pointer to a variable that receives the
        selected boot entry.

    ReturnToFirmware - Supplies a pointer to a variable that receives a boolean
        indicating if the user requested the boot manager to return to the
        firmware.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Row or Column is larger than the size of the
        display.

    Other Error status if firmware was unable to set cursor position.

--*/

{

    PBOOT_APPLICATION_ENTRY BootEntry;
    BOOT_MENU_STATUS BootStatus;
    ULONG Count;
    BOOLEAN DisplayBootMenu;
    ULONG Index;
    NTSTATUS Status;
    ULONGLONG Timeout;
    BOOLEAN TimeoutActive;
    WCHAR TimeoutStr[32];

    ASSERT(NumBootEntries != 0);

    *ReturnToFirmware = FALSE;
    Timeout = 0;  // Satisfy compiler.
    BootEntry = NULL;

    //
    // Get the default boot entry and timeout options, or use the cached
    // default setting (and no timeout).
    //

    TimeoutActive = FALSE;
    if (BmDisplayStateCached == FALSE) {
        Status = BlGetApplicationOptionInteger(BCDE_BOOTMGR_TYPE_TIMEOUT,
                                               &Timeout);

        if (NT_SUCCESS(Status)) {
            TimeoutActive = TRUE;
        }

        Status = BlGetApplicationOptionGuid(BCDE_BOOTMGR_TYPE_DEFAULT_OBJECT,
                                            &BmDefaultObject);

        if (!NT_SUCCESS(Status)) {
            BmDefaultObject = BootEntryList[0]->Identifier;
        }
    }

    //
    // Output the OS choices menu.
    //

DisplayBootMenu:
    if (NumToolEntries > 0) {
        BlXmiWrite(L"<osboot-list-tools");

    } else {
        BlXmiWrite(L"<osboot-list");
    }

    if (TimeoutActive != FALSE) {
        BlXmiWrite(L" timeout='");
        swprintf_s(TimeoutStr,
                   sizeof(TimeoutStr) / sizeof(WCHAR),
                   L"%I64u'",
                   Timeout);

        BlXmiWrite(TimeoutStr);
    }

    //
    // For pxe boot, add server IP address.
    //

    BmpDisplayAddServerAddress();
    BlXmiWrite(L">");

    //
    // Output the individual boot entries.  Resume applications can only
    // be started by the resume from hibernate code and thus can never
    // legitimately appear on the generic boot menu.
    //

    for (Index = 0; Index < NumBootEntries; Index += 1) {
        if (CHECK_FLAG(BootEntryList[Index]->Attributes,
                       BOOT_ENTRY_OS_RESUME_LOADER) == FALSE) {

            BmpOutputApplicationEntry(BootEntryList[Index], Index, FALSE);
        }
    }

    //
    // Output the individual tool application entries.
    //

    for (Count = 0; Count < NumToolEntries; Count += 1) {
        BmpOutputApplicationEntry(ToolEntryList[Count], (Index + Count), TRUE);
    }

    if (NumToolEntries > 0) {
        BlXmiWrite(L"</osboot-list-tools>");

    } else {
        BlXmiWrite(L"</osboot-list>");
    }

    do {

        //
        // Get the return value of the selected boot entry.
        //

        BmDisplayGetBootMenuStatus(&BootStatus);

        //
        // Cancel the default selection timer if a key was pressed.
        //

        if ((TimeoutActive != FALSE) && (BootStatus.AnyKey != 0)) {
            BlXmiWrite(L"<timeout-cancel/>");
            TimeoutActive = FALSE;
        }

        //
        // Return to firmware if the user has cancelled out of the menu.
        //

        if (BootStatus.Exit != 0) {
            *ReturnToFirmware = TRUE;
            goto DisplayBootMenuEnd;
        }

        //
        // Clear the screen, process any custom OEM actions, then redraw
        // the boot menu.
        //

        if (BootStatus.OemAction != 0) {
            BlXmiWrite(L"<bootmgr-clear-screen/>");
            BmProcessCustomAction(DataStoreHandle, &BootStatus.OemKey);
            goto DisplayBootMenu;
        }

        if (BootStatus.BootIndex != INVALID_CHOICE_INDEX) {
            if (BootStatus.BootIndex < NumBootEntries) {
                BootEntry = BootEntryList[BootStatus.BootIndex];

                //
                // Update the default entry based on the selected index.
                // This allows subsequent boot displays to default to the
                // index selected on a previous iteration.
                //

                BmDefaultObject = BootEntry->Identifier;

            } else {
                BootEntry =
                    ToolEntryList[(BootStatus.BootIndex - NumBootEntries)];
            }

            //
            // Only allow advanced options for OS loader boot entries.
            // Otherwise, consider the selection invalid.
            //

            if ((BootStatus.AdvancedOptions != 0) ||
                (BootStatus.BootOptions != 0)) {

                BmDisplayGetOsLoaderKeyInput(&BootStatus,
                                             BootEntry,
                                             &DisplayBootMenu);
            }
        }

    } while (BootStatus.BootIndex == INVALID_CHOICE_INDEX);

    //
    // Return the selected boot entry after clearing the screen.
    //

    *SelectedBootEntry = BootEntry;

DisplayBootMenuEnd:
    BlXmiWrite(L"<bootmgr-clear-screen/>");
    return STATUS_SUCCESS;
}

VOID
BmDisplayClearScreen (
    VOID
    )

/*++

Routine Description:

    This routine clears the screen of all contents.

Arguments:

    None.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    //
    // BGFX initialization isn't required; use XML layer as a fallback to
    // clearing the screen.
    //

    Status = BmBgDisplayClearScreen();
    if (!NT_SUCCESS(Status)) {
        BlDisplayClearScreen();
    }

    return;
}

BM_ERROR_DISPOSITION
BmDisplayDumpError (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG LaunchReason
    )

/*++

Routine Description:

    Displays an error screen, and data pertaining to a previously logged error.

Arguments:

    BootEntry - Supplies an optional pointer to a boot entry that experienced
        an error.  If this is supplied, the user should be given the option to
        recover from this error using the boot entry recovery sequence.

    LaunchReason - Supplies an indicating of why recovery needed to be
        launched. This is useful for displaying appropriate messages when
        recovery is missing.

Return Value:

    The user-requested disposition for how to proceed in the face of this
    error.  This may be to exit the boot manager, continue on with displaying
    the boot choices, or to attempt recovery from the failure.

--*/

{

    ULONGLONG BootErrorUxType;
    ULONG CurrentTime;
    BM_ERROR_DISPOSITION Disposition;
    PINTERNAL_BOOT_ERROR ErrorLog;
    BOOLEAN MultiBootSystem;
    BOOLEAN NoErrorDisplay;
    BOOLEAN NoKeyboard;
    BOOLEAN OemRecoveryAvailable;
    PXML_DOC Post;
    BOOLEAN RecoveryAvailable;
    BOOLEAN RestartOnFailure;
    BOOLEAN ShutdownDisabled;
    ULONG StartTime;
    NTSTATUS Status;
    BOOLEAN TimerEnabled;

    Disposition = BmQuit;

    //
    // Do not display an error screen if error messages are suppressed by the
    // entry. Restart on failure option implies no error display.
    // Assume RestartonFailure not set on read failure.
    //

    RestartOnFailure = FALSE;
    if (ARGUMENT_PRESENT(BootEntry) != FALSE) {
        Status = BlGetBootOptionBoolean(BootEntry->Options,
                                        BCDE_LIBRARY_TYPE_RESTART_ON_FAILURE,       
                                        &RestartOnFailure);

        if (!NT_SUCCESS(Status)) {
            RestartOnFailure = FALSE;
        }
    }    

    if (RestartOnFailure != FALSE) {
        NoErrorDisplay = TRUE;

    } else {
        Status = BlGetApplicationOptionBoolean(
                     BCDE_BOOTMGR_TYPE_NO_ERROR_DISPLAY,
                     &NoErrorDisplay);

        if (!NT_SUCCESS(Status)) {
            NoErrorDisplay = FALSE;
        }
    }

    if (NoErrorDisplay != FALSE) {
        return BmQuit;
    }

    //
    // Get the saved error information.
    //

    ErrorLog = BmErrorGetLast();
    if (ErrorLog == NULL) {
        return BmContinue;
    }

    //
    // Determine if recovery is available. If WinRE is available, prefer it
    // over OEM recovery. If WinRE is not available, see if an OEM recovery
    // sequence is available.
    //

    RecoveryAvailable = TRUE;
    if ((ARGUMENT_PRESENT(BootEntry) != FALSE) &&
        (BlBootOptionExists(BootEntry->Options,
                            BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE) != FALSE)) {

        OemRecoveryAvailable = FALSE;

    } else {
        OemRecoveryAvailable = BmOemCustomActionExists();
        if (OemRecoveryAvailable == FALSE) {
            RecoveryAvailable = FALSE;
        }
    }

    NoKeyboard = BmDisplayIsKeyboardLessSystem();
    MultiBootSystem = BmIsMultiBootSystem(NULL);

    //
    // Call the correct display routine based on the boot error ux type.
    //
    // N.B. When boot error ux type is set to standard, fallback to using
    // legacy functions if the graphics subsystem fails. Currently there is no
    // fallback supported when the boot error ux type is set to simple.
    //

    BootErrorUxType = BmGetBootErrorUxType(BootEntry, NoKeyboard);
    if (BootErrorUxType == BgErrorUxSimple) {
        
        //
        // Ignore graphics subsytem failure. This policy is planned to be used
        // only by the phone team at the moment and falling back to legacy
        // functions is not ideal given the PC specific contents the legacy
        // function displays.
        //

        BmDisplayBootGraphics();

    } else if ((BootErrorUxType == BgErrorUxStandard) &&
        (BmDisplayBootGraphics() == FALSE)) {

        BootErrorUxType = BgErrorUxLegacy;
    }

    if (BootErrorUxType == BgErrorUxSimple) {
        BmpDisplayErrorSimple(0);

    } else if (BootErrorUxType == BgErrorUxLegacy) {
        BmpDisplayDumpErrorLegacy(0,
                                  BootEntry,
                                  ErrorLog,
                                  MultiBootSystem,
                                  RecoveryAvailable,
                                  OemRecoveryAvailable,
                                  NoKeyboard);

    } else {
        BmpDisplayDumpErrorStandard(0,
                                    BootEntry,
                                    ErrorLog,
                                    LaunchReason,
                                    MultiBootSystem,
                                    RecoveryAvailable,
                                    OemRecoveryAvailable,
                                    NoKeyboard);
    }

    //
    // Only enable the shutdown timer if it has not been disabled in the BCD
    // store.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_BOOT_SHUTDOWN_DISABLED,
                 &ShutdownDisabled);

    if (!NT_SUCCESS(Status)) {
        ShutdownDisabled = FALSE;
    }

    //
    // Enable the timer if shutdown isn't disabled or this is a BCD file load
    // error.
    //

    if ((ShutdownDisabled == FALSE) ||
        (ErrorLog->ErrorCode == BM_ERROR_CONFIGURATION_DATA)) {
        TimerEnabled = TRUE;
        StartTime = BlTimeGetRelativeTime();

    } else {
        TimerEnabled = FALSE;
        StartTime = 0;
    }

    if (BootErrorUxType != BgErrorUxSimple) {
        //
        // Block on input and return and convert the selected action into the
        // return value.  If recognized actions are not present, assume the
        // "quit" disposition, which corresponds to a fatal error.
        //

        while (TRUE) {
            Post = BlXmiRead(FALSE);
            if (Post != NULL) {
                if (OsxmlResultHasAction(Post, L"tryagain")) {
                    Disposition = BmTryAgain;

                } else if (OsxmlResultHasAction(Post, L"nextos")) {
                    Disposition = BmNextOs;

                } else if (OsxmlResultHasAction(Post, L"osselection")) {
                    Disposition = BmOsSelection;

                } else if (OsxmlResultHasAction(Post, L"recoveroem")) {
                    Disposition = BmRecoverOem;

                } else if (OsxmlResultHasAction(Post, L"recover")) {

                    ASSERT(ARGUMENT_PRESENT(BootEntry));

                    Disposition = BmRecover;

                } else if (OsxmlResultHasAction(Post, L"advops")) {

                    ASSERT(ARGUMENT_PRESENT(BootEntry));

                    //
                    // Ignore this option on non OS loader entries.
                    //

                    if (CHECK_FLAG(BootEntry->Attributes,
                                   BOOT_ENTRY_OS_LOADER) == FALSE) {

                        continue;
                    }

                    Disposition = BmAdvancedOptions;

                } else if (OsxmlResultHasAction(Post, L"bootops")) {

                    ASSERT(ARGUMENT_PRESENT(BootEntry));

                    //
                    // Ignore this option on non OS loader entries.
                    //

                    if (CHECK_FLAG(BootEntry->Attributes,
                                   BOOT_ENTRY_OS_LOADER) == FALSE) {

                        continue;
                    }

                    Disposition = BmEditOptions;

                } else if (OsxmlResultHasAction(Post, L"firmware")) {

                    //
                    // If this fails, loop for another option. It should not
                    // return if it succeeds.
                    //

                    Status = BmFwBootToFirmware();
                    if (!NT_SUCCESS(Status)) {
                        continue;
                    }

                    Disposition = BmQuit;

                } else {
                    Disposition = BmQuit;
                }

                ObjRelease(Post);
                break;
            }
        
            //
            // Either shutdown or reset the machine after waiting 1 minute for
            // input.
            //

            if (TimerEnabled != FALSE) {
                CurrentTime = BlTimeGetRelativeTime();
                if ((CurrentTime - StartTime) > 60) {
                    if (ErrorLog->ErrorCode == BM_ERROR_CONFIGURATION_DATA) {
                        BlFwReboot();

                    } else {
                        BlFwShutdown();
                    }
                }
            }
        }

    } else {
        //
        // Either shutdown or reset the machine after waiting 30 seconds.
        //

        if (TimerEnabled != FALSE) {
            while (TRUE) {
                CurrentTime = BlTimeGetRelativeTime();
                if ((CurrentTime - StartTime) > 30) {
                    if (ErrorLog->ErrorCode == BM_ERROR_CONFIGURATION_DATA) {
                        BlFwReboot();

                    } else {
                        BlFwShutdown();
                    }
                }
                
                //
                // Stall for 5 seconds
                //
                BlTimeStallExecution(5000000);
            }
        }

    }

    if (BootErrorUxType == BgErrorUxSimple) {
        BmpDisplayErrorSimple(1);

    } else if (BootErrorUxType == BgErrorUxLegacy) {
        BmpDisplayDumpErrorLegacy(1,
                                  BootEntry,
                                  ErrorLog,
                                  MultiBootSystem,
                                  RecoveryAvailable,
                                  OemRecoveryAvailable,
                                  NoKeyboard);

    } else {
        BmpDisplayDumpErrorStandard(1,
                                    BootEntry,
                                    ErrorLog,
                                    LaunchReason,
                                    MultiBootSystem,
                                    RecoveryAvailable,
                                    OemRecoveryAvailable,
                                    NoKeyboard);
    }

    return Disposition;
}

VOID
BmDisplayGetBootMenuStatus (
    __out PBOOT_MENU_STATUS BootStatus
    )

/*++

Routine Description:

    This routine reads the boot entry selection from the appropriate
    device.

Arguments:

    BootStatus - Supplies a pointer to a buffer that receives status relating
        to the boot menu selection.

Return Value:

    None.

--*/

{

    PCWSTR ChoiceString;
    BOOL Form;
    PCWSTR OemKeyString;
    PXML_DOC Post;

    RtlZeroMemory(BootStatus, sizeof(BOOT_MENU_STATUS));
    BootStatus->BootIndex = INVALID_CHOICE_INDEX;
    ChoiceString = NULL;

    //
    // Get the input document.
    //

    Post = BlXmiRead(FALSE);
    if (Post == NULL) {
        goto GetBootMenuStatusEnd;
    }

    if (OsxmlResultHasAction(Post, L"anykey") != FALSE) {
        BootStatus->AnyKey = 1;
    }

    if (OsxmlResultHasAction(Post, L"exit") != FALSE) {
        BootStatus->Exit = 1;
    }

    if (OsxmlResultHasAction(Post, L"advops") != FALSE) {
        BootStatus->AdvancedOptions = 1;
    }

    if (OsxmlResultHasAction(Post, L"bootops") != FALSE) {
        BootStatus->BootOptions = 1;
    }

    if (OsxmlResultHasAction(Post, L"oem") != FALSE) {
        BootStatus->OemAction = 1;
    }

    if (BootStatus->OemAction != 0) {
        OemKeyString = OsxmlResultGetLastKeyValue(Post);
        if (OemKeyString != NULL) {
            BootStatus->OemKey = (WCHAR)_wtoi(OemKeyString);
        }
    }

    Form = OsxmlResultHasAction(Post, L"submit");
    if (Form) {
        ChoiceString = OsxmlResultGetActiveValue(Post, L"default");
        if (ChoiceString != NULL) {
            BootStatus->BootIndex = _wtoi(ChoiceString);
        }
    }

GetBootMenuStatusEnd:
    if (Post != NULL) {
        ObjRelease(Post);
    }

    return;
}

VOID
BmDisplayGetOsLoaderKeyInput (
    __in PBOOT_MENU_STATUS BootMenuStatus,
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __out PBOOLEAN DisplayBootMenu
    )

/*++

Routine Description:

    This routine collects advanced user input for OS loader entries. It serves
    a dual purpose in doing so for cases where there is a single boot entry or
    the default boot entry has a "Standard" boot menu policy, and during
    multi-OS selection. For the latter it determines if the advanced input is
    valid for the given OS loader entry. For the former, it determines if the
    key input should force the boot menu to be displayed.

    If the provided entry is not an OS loader entry, this routine marks the
    selection as invalid.

Arguments:

    BootMenuStatus - Supplies a pointer to a boot menu status structure holding
        user input data.

    BootEntry - Supplies a pointer to the boot entry being processed.

    DisplayBootMenu - Supplies a pointer to a variable that receives TRUE if
        the boot menu should be forced based on user input and FALSE otherwise.

Return Value:

    None.

--*/

{

    ULONGLONG BootMenuPolicy;

#ifndef DISABLE_INTERNAL_POLICIES

    NTSTATUS Status;

#endif

    //
    // The below key presses only apply to OS loader applications. If a key was
    // pressed and the default or single entry is not an OS loader application,
    // display the legacy boot menu. This preserves downlevel behavior.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OS_LOADER) == FALSE) {
        BootMenuStatus->BootIndex = INVALID_CHOICE_INDEX;
        *DisplayBootMenu = TRUE;
        return;
    }

    //
    // Determine the boot menu policy of the currently selected boot entry.
    //

    *DisplayBootMenu = FALSE;

#ifdef DISABLE_INTERNAL_POLICIES

    BootMenuPolicy = MenuPolicyLegacy;

#else

    Status = BlGetBootOptionInteger(BootEntry->Options,
                                    BCDE_OSLOADER_TYPE_BOOT_MENU_POLICY,
                                    &BootMenuPolicy);

    if (!NT_SUCCESS(Status)) {
        BootMenuPolicy = MenuPolicyLegacy;
    }

#endif

    //
    // All boot policy types can launch the F8 or F10 menus. If any other key
    // was pressed and the menu policy is "Legacy", it is an invalid choice and
    // triggers the boot menu to be displayed by the boot manager for single
    // boot entries. Otherwise (if any other key is pressed for a "Standard"
    // policy entry), the choice is invalid, but the boot menu should not be
    // displayed.
    //

    if (BootMenuStatus->AdvancedOptions != 0) {
        (VOID)BlAppendBootOptionBoolean(BootEntry,
                                        BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS,
                                        TRUE);

    } else if (BootMenuStatus->BootOptions != 0) {
        (VOID)BlAppendBootOptionBoolean(BootEntry,
                                        BCDE_LIBRARY_TYPE_DISPLAY_OPTIONS_EDIT,
                                        TRUE);

    } else if (BootMenuPolicy == MenuPolicyLegacy) {
        BootMenuStatus->BootIndex = INVALID_CHOICE_INDEX;
        *DisplayBootMenu = TRUE;

    } else {
        BootMenuStatus->BootIndex = INVALID_CHOICE_INDEX;
    }

    return;
}

BOOLEAN
BmDisplayIsKeyboardLessSystem (
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

    BlMmUnmapVirtualAddress(Fadt, Fadt->Header.Length);

IsKeyboardLessSystemEnd:
    return NoKeyboard;
}

//
// ------------------------------------------------------------ Local Functions
//

VOID
BmpDisplayAddServerAddress (
    VOID
    )

/*++

Routine Description:

    If Boot Manager has been launched using PXE boot, then adds an xml element
    to the output containing the address of the PXE server.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    BOOLEAN ElementAdded;
    PPXE_INFO PxeInfo;
    WCHAR ServerStr[PXE_IP_ADDRESS_STRING_BUFFER_LENGTH];
    NTSTATUS Status;

    ElementAdded = FALSE;
    BootDevice = NULL;
    Status = BlGetApplicationOptionDevice(BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                          &BootDevice,
                                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto AddServerAddressEnd;
    }

    if (BootDevice->DeviceType == DEVICE_UDP) {
        PxeInfo = BlNetGetPxeInfo();
        if ((PxeInfo == NULL) || (PxeInfo->PxenvEntry == NULL)) {
            Status = STATUS_NO_SUCH_DEVICE;
            goto AddServerAddressEnd;
        }

        Status = BlNetAddressToString(ServerStr,
                                      sizeof(ServerStr) / sizeof(WCHAR),
                                      &PxeInfo->ServerAddress);
        if (!NT_SUCCESS(Status)) {
            goto AddServerAddressEnd;
        }

        BlXmiWrite(L" wds='");
        BlXmiWrite(ServerStr);
        BlXmiWrite(L"'");
        ElementAdded = TRUE;
    }

AddServerAddressEnd:
    if (ElementAdded == FALSE) {
        BlXmiWrite(L" nowds='true'");
    }

    return;
}

PWCHAR
BmpDisplayGetProgressMessage (
    VOID
    )

/*++

Routine Description:

    This routine returns the progress message to be displayed during RAMDISK
    boot.

Arguments:

    None.

Return Value:

    The string message to display.

--*/

{

    BCDE_BOOTUX_MESSAGE_TYPE DisplayMessageType;
    PWCHAR ProgressMessage;
    ULONG ProgressMessageId;

    //
    // Get the message type based on the current BCD settings.
    //

    DisplayMessageType = BmpDisplayGetProgressMessageType();

    //
    // Determine the resource message ID for the string message.
    //

    switch (DisplayMessageType) {
    case BcdeBootuxMessageHyperV:
        ProgressMessageId = BM_DISPLAY_STRING_HYPER_V;
        break;

    case BcdeBootuxMessageRecovery:
        ProgressMessageId = BM_DISPLAY_STRING_RECOVERY;
        break;

    case BcdeBootuxMessageStartupRepair:
        ProgressMessageId = BM_DISPLAY_STRING_STARTUP_REPAIR;
        break;

    case BcdeBootuxMessageSystemImageRecovery:
        ProgressMessageId = BM_DISPLAY_STRING_SYSTEM_IMAGE_RECOVERY;
        break;

    case BcdeBootuxMessageCommandPrompt:
        ProgressMessageId = BM_DISPLAY_STRING_COMMAND_PROMPT;
        break;

    case BcdeBootuxMessageSystemRestore:
        ProgressMessageId = BM_DISPLAY_STRING_SYSTEM_RESTORE;
        break;

    case BcdeBootuxMessagePushButtonReset:
        ProgressMessageId = BM_DISPLAY_STRING_PUSH_BUTTON_RESET;
        break;

    case BcdeBootuxMessageFactoryReset:
        ProgressMessageId = BM_DISPLAY_STRING_FACTORY_RESET;
        break;

    case BcdeBootuxMessageFveRecovery:
        ProgressMessageId = BM_DISPLAY_STRING_FVE_RECOVERY;
        break;

    default:
        ProgressMessageId = BM_DISPLAY_STRING_DEFAULT;
        break;
    }

    //
    // Retrieve the resource string and return it to the caller.
    //

    ProgressMessage = BlResourceFindMessage(ProgressMessageId);

    ASSERT(ProgressMessage != NULL);

    return ProgressMessage;
}

BCDE_BOOTUX_MESSAGE_TYPE
BmpDisplayGetProgressMessageType (
    VOID
    )

/*++

Routine Description:

    This routine determines which progress message should be displayed based on
    the BCD store.

Arguments:

    None.

Return Value:

    Returns the BCD boot UX message type to be displayed.

--*/

{

    PBOOT_APPLICATION_ENTRY BootEntry;
    HANDLE DataStoreHandle;
    ULONGLONG DisplayMessageType;
    GUID Identifier;
    NTSTATUS Status;

    //
    // If a boot entry has not been selected, return the default message.
    //

    BootEntry = BmGetSelectedBootEntry();
    if (BootEntry == NULL) {
        return BcdeBootuxMessageDefault;
    }

    //
    // Test to see if a message override has been specified for one-time
    // boot. Purge the setting from the BCD store if it exists.
    //

    Status = BlGetBootOptionInteger(
                 BootEntry->Options,
                 BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE,
                 &DisplayMessageType);

    if (NT_SUCCESS(Status)) {
        DataStoreHandle = NULL;
        Status = BmOpenDataStore(&DataStoreHandle);
        if (NT_SUCCESS(Status)) {
            RtlCopyMemory(&Identifier, &BootEntry->Identifier, sizeof(GUID));
            BmPurgeOption(DataStoreHandle,
                          &Identifier,
                          BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE_OVERRIDE);

            BmCloseDataStore(DataStoreHandle);
        }

        goto DisplayGetProgressMessageTypeEnd;
    }

    //
    // Determine if the current application does not use the default message,
    // opting for the specified message if it exists.
    //

    Status = BlGetBootOptionInteger(
                 BootEntry->Options,
                 BCDE_LIBRARY_TYPE_BOOTUX_DISPLAY_MESSAGE,
                 &DisplayMessageType);

    if (NT_SUCCESS(Status)) {
        goto DisplayGetProgressMessageTypeEnd;
    }

    //
    // Fall back to the default message.
    //

    DisplayMessageType = BcdeBootuxMessageDefault;

DisplayGetProgressMessageTypeEnd:
    return (BCDE_BOOTUX_MESSAGE_TYPE)DisplayMessageType;
}

VOID
BmpOutputApplicationEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in ULONG EntryIndex,
    __in BOOLEAN EntryIsTool
    )

/*++


Routine Description:

    This routine outputs a boot entry to the appropriate devices.

Arguements:

    BootEntry - Pointer to the internal boot entry to print

    EntryIndex - Index value associated with this boot entry

    EntryIsTool - Supplies a boolean indicating if the entry is a tool.

Return Value:

    None

--*/

{

    PWCHAR AdvOpsPrompt;
    PWCHAR Description;
    BOOLEAN Enabled;
    WCHAR IndexString[8];
    PWCHAR OsxmlAdvOpsPrompt;
    PWCHAR OsxmlDescription;
    NTSTATUS Status;

    //
    // Output the boot entry tag and the entry name.
    //

    Description = NULL;
    Status = BlGetBootOptionString(BootEntry->Options,
                                   BCDE_LIBRARY_TYPE_DESCRIPTION,
                                   &Description);

    ASSERT(NT_SUCCESS(Status));
    __analysis_assume(NT_SUCCESS(Status));

    if (EntryIsTool != FALSE) {
        BlXmiWrite(L"<osboot-tool name='");

    } else {
        BlXmiWrite(L"<osboot-entry name='");
    }

    //
    // Remove any XML reserved characters, such as '<', '>', '&', etc.
    //

    Status = BlXmiTransformTextForXml(Description, &OsxmlDescription);
    if (NT_SUCCESS(Status)) {
        BlXmiWrite(OsxmlDescription);
        BlMmFreeHeap(OsxmlDescription);

    } else {
        BlXmiSanitizeTextForXml(Description);
        BlXmiWrite(Description);
    }

    BlMmFreeHeap(Description);

    //
    // Output the index, which is used as the identifier of the entry to
    // determine whether it is selected.
    //

    swprintf_s(IndexString,
               sizeof(IndexString) / sizeof(WCHAR),
               L"%lu",
               EntryIndex);

    BlXmiWrite(L"' value='");
    BlXmiWrite(IndexString);

    //
    // Perform OS loader-specific processing if the entry is a loader.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OS_LOADER)) {

        //
        // Inform Secure Boot to temporarily process osloader rules.
        //

        BlSecureBootSetApplicationType(
            MAKE_BCD_APPLICATION_OBJECT(
            BCD_IMAGE_TYPE_BOOT_APPLICATION,
            BCD_APPLICATION_TYPE_WINDOWS_BOOT_LOADER));

        //
        // Output a tool tip indicating advanced options are present, and a
        // carat at the end of the entry..
        //

        AdvOpsPrompt = BlResourceFindMessage(BM_MSG_ADVANCED_OPTIONS_PROMPT);
        if (AdvOpsPrompt != NULL) {
            Status = BlXmiTransformTextForXml(AdvOpsPrompt, &OsxmlAdvOpsPrompt);
            if (NT_SUCCESS(Status)) {
                BlXmiWrite(L"' tip='");
                BlXmiWrite(OsxmlAdvOpsPrompt);
                BlXmiWrite(L"' carat='true"); // Displays a '>' char @ menu EOL.
                BlMmFreeHeap(OsxmlAdvOpsPrompt);
            }
        }

        //
        // Indicate that kernel EMS and debugging are enabled if the boot entry
        // is an OS loader and the appropriate options are present.
        //

        Status = BlGetBootOptionBoolean(BootEntry->Options,
                                        BCDE_OSLOADER_TYPE_KERNEL_EMS_ENABLED,
                                        &Enabled);

        if (NT_SUCCESS(Status) && (Enabled != FALSE)) {
            BlXmiWrite(L"' ems='enabled");
        }

        Status = BlGetBootOptionBoolean(
                     BootEntry->Options,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &Enabled);

        if (NT_SUCCESS(Status) && (Enabled != FALSE)) {
            BlXmiWrite(L"' debugger='enabled");
        }

        //
        // Inform Secure Boot to revert to processing bootmgr rules.
        //

        BlSecureBootSetApplicationType(
            MAKE_BCD_APPLICATION_OBJECT(
            BCD_IMAGE_TYPE_FIRMWARE_APPLICATION,
            BCD_APPLICATION_TYPE_WINDOWS_BOOT_MANAGER));
    }

    if (IsEqualGUID(&BootEntry->Identifier, &BmDefaultObject)) {
        BlXmiWrite(L"' default='true");
    }

    BlXmiWrite(L"'/>");
}

VOID
BmDisplayMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN Display
    )

/*++

Routine Description:

    This routine is called to display and remove messages which are used to
    indicate to the user that the boot manager is waiting for a multicast
    session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

    Display - Supplies TRUE to display the message, FALSE to remove the message.

Return Value:

    None.

--*/

{


    ULONGLONG BootMenuPolicy;

    //
    // Fallback to using legacy functions if the graphics subsystem fails.
    //

    BootMenuPolicy = BmGetBootMenuPolicy(NULL);
    if ((BootMenuPolicy == MenuPolicyStandard) &&
        (BmDisplayBootGraphics() != FALSE)) {

        BmpDisplayStandardMulticastMessage(Parameter, Display);

    } else {
        BmpDisplayLegacyMulticastMessage(Parameter, Display);
    }

    return;
}

NTSTATUS
BmUpdateMulticastMessage (
    __in PVOID Parameter
    )

/*++


Routine Description:

    This routine updates the spinner on the display to indicate that the boot
    manager is still alive and waiting for the multicast session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

Return Value:

    STATUS_SUCCESS on success.
    STATUS_CANCELLED if user has cancelled the download.

--*/

{

    ULONGLONG BootMenuPolicy;
    NTSTATUS Status;

    //
    // Fallback to using legacy functions if the graphics subsystem fails.
    //

    BootMenuPolicy = BmGetBootMenuPolicy(NULL);
    if ((BootMenuPolicy == MenuPolicyStandard) &&
        (BmDisplayBootGraphics() != FALSE)) {

        Status = BmpUpdateStandardMulticastMessage(Parameter);

    } else {
        Status = BmpUpdateLegacyMulticastMessage(Parameter);
    }

    return Status;
}

VOID
BmUpdateProgress (
    __in PVOID Context,
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    )

/*++

Routine Description:

    This routine shows the progress bar of an image loading.

    TODO: This routine was copied from lib\io\device\ramapi.c where it
    was originally written to show progress for ramdisk loading.  The decision
    was made to move the progress notification out of the library and into the
    app.  Eventually a XML-based progress indication should be implemented.
    This one will be used for now.  Currently the only consumer of
    BlUtlUpdateProgress, which causes this routine to be called, is the
    ramdisk support.

Arguments:

    Context - Supplies a pointer to a structure containing progress bar
        context.

    PercentComplete - Supplies the current percentage of completion.

    ProgressDisplayComplete - Supplies a boolean that receives a value
        indicating if the progress display is complete.

Return Value:

    None.

--*/

{

    ULONGLONG BootMenuPolicy;

    //
    // Fallback to using legacy functions if the graphics subsystem fails.
    //

    BootMenuPolicy = BmGetBootMenuPolicy(NULL);
    if ((BootMenuPolicy == MenuPolicyStandard) &&
        (BmDisplayBootGraphics() != FALSE)) {

        BmpUpdateStandardProgress(Context,
                                  PercentComplete,
                                  ProgressDisplayComplete);

    } else {
        BmpUpdateLegacyProgress(Context,
                                PercentComplete,
                                ProgressDisplayComplete);
    }

    return;
}

VOID
BmUpdateProgressInfo (
    __in PVOID Context,
    __in PWSTR ProgressInformation
    )

/*++

Routine Description:

    This routine displays additional information about the progress bar.

Arguments:

    Context - Supplies a pointer to a structure containing progress bar
        context.

    ProgressInformation - Supplies additional info for the progress indicator.

Return Value:

    None.

--*/

{

    ULONGLONG BootMenuPolicy;

    //
    // Fallback to using legacy functions if the graphics subsystem fails.
    //

    BootMenuPolicy = BmGetBootMenuPolicy(NULL);
    if ((BootMenuPolicy == MenuPolicyStandard) &&
        (BmDisplayBootGraphics() != FALSE)) {

        BmpUpdateStandardProgressInformation(Context, ProgressInformation);

    } else {
        BmpUpdateLegacyProgressInformation(Context, ProgressInformation);
    }

    return;
}

VOID
BmpDisplayDumpErrorLegacy (
    __in ULONG Phase,
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in PINTERNAL_BOOT_ERROR ErrorLog,
    __in BOOLEAN MultiBootSystem,
    __in BOOLEAN RecoveryAvailable,
    __in BOOLEAN OemRecoveryAvailable,
    __in BOOLEAN NoKeyboard
    )

/*++

Routine Description:

    This routine display the legacy boot error screen.

Arguments:

    Phase - Supplies the display phase for the legacy error screen.

    BootEntry - Supplies an optional pointer to a boot entry that experienced
        an error.  If this is supplied, the user should be given the option to
        recover from this error using the boot entry recovery sequence.

    ErrorLog - Supplies a pointer to the error information.

    MultiBootSystem - Supplies a boolean indicating if this current system has
        more than one operating system.

    RecoveryAvailable - Supplies a boolean indicating if the windows recovery
        environment is available.

    OemRecoveryAvailable - Supplies a boolean indicating if an OEM recovery
        environment is available.

    NoKeyboard - Supplies a boolean indicating if no keyboard is present on the
        device.

Return Value:

    None.

--*/

{

    PWCHAR ErrorInfo;
    NTSTATUS ErrorStatus;
    PWCHAR FileName;
    BM_ERROR_HELP_TYPE HelpType;
    PCHAR SourceFile;
    ULONG SourceLine;
    NTSTATUS Status;
    PWCHAR TempInfo;
    WCHAR TempString[MAX_ERROR_STRING];

#if DBG

    ULONG Index;

#endif

    //
    // Phase 1 clears the screen.
    //

    if (Phase == 1) {
        BlXmiWrite(L"<bootmgr-clear-screen/>");
        return;
    }

    ErrorStatus = ErrorLog->Status;
    ErrorInfo = ErrorLog->ErrorMsg;
    FileName = ErrorLog->FileName;
    HelpType = ErrorLog->HelpType;
    SourceLine = ErrorLog->SourceLine;
    SourceFile = ErrorLog->SourceFile;

    //
    // First output the help text.
    //

    BlXmiWrite(L"<osboot-error-legacy ");
    if (HelpType == BmErrorHelpHash) {
        BlXmiWrite(L" hash-help='true'");

    } else if (HelpType == BmErrorHelpIo) {
        BlXmiWrite(L" io-help='true'");

    } else {
        BlXmiWrite(L" generic-help='true'");
    }

    //
    // Output the error status if not STATUS_SUCCESS.
    //

    if (!NT_SUCCESS(ErrorStatus)) {
        BlXmiWrite(L" status='");
        swprintf_s(TempString,
                   sizeof(TempString) / sizeof(WCHAR),
                   L"0x%08x",
                   ErrorStatus);

        BlXmiWrite(TempString);
    }

    //
    // Dump the line and source file where this error ocurred.
    //

#if DBG

    BlXmiWrite(L"' source='");
    if (SourceFile == NULL) {
        BlXmiWrite(L"NULL");

    } else {
        for (Index = 0;
             ((Index < (MAX_ERROR_STRING - 1)) && (SourceFile[Index] != 0));
             Index++) {

            TempString[Index] = SourceFile[Index];
        }

        TempString[Index] = 0;
        BlXmiWrite(TempString);
    }

    BlXmiWrite(L"' line='");
    swprintf_s(TempString,
               sizeof(TempString) / sizeof(WCHAR),
               L"%d",
               SourceLine);

    BlXmiWrite(TempString);

#endif

    //
    // Display specific information pertaining to this error.
    //
    // N.B. Be sure to cleanse the error info of any restricted XML.
    //

    if (ErrorInfo != NULL) {
        Status = BlXmiTransformTextForXml(ErrorInfo, &TempInfo);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(L"' info='");
            BlXmiWrite(TempInfo);
            BlMmFreeHeap(TempInfo);
        }
    }

    //
    // Display the file associated with this error.
    //

    if (FileName != NULL) {
        Status = BlXmiTransformTextForXml(FileName, &TempInfo);
        if (NT_SUCCESS(Status)) {
            BlXmiWrite(L"' file='");
            BlXmiWrite(TempInfo);
            BlMmFreeHeap(TempInfo);
        }
    }

    BlXmiWrite(L"'");

    //
    // Add Windows Deployment Server information.
    //

    BmpDisplayAddServerAddress();

    //
    // Determine which footer options to display. If no boot entry was
    // supplied, do not give the user an option.
    //

    if (ARGUMENT_PRESENT(BootEntry) != FALSE) {
        if (RecoveryAvailable != FALSE) {
            if (NoKeyboard != FALSE) {
                if (OemRecoveryAvailable != FALSE) {
                    BlXmiWrite(L" nokeyboard-recovery-oem='true'");

                } else {
                    BlXmiWrite(L" nokeyboard-recovery='true'");
                }

            } else {
                if (OemRecoveryAvailable != FALSE) {
                    BlXmiWrite(L" recovery-oem='true'");

                } else {
                    BlXmiWrite(L" recovery='true'");
                }
            }

        } else {
            if (NoKeyboard != FALSE) {
                if (MultiBootSystem != FALSE) {
                    BlXmiWrite(L" nokeyboard-norecovery-multi='true'");

                } else {
                    if (BmFwBootToFirmwareSupported() != FALSE) {
                        BlXmiWrite(L" nokeyboard-norecovery-firmware='true'");

                    } else {
                        BlXmiWrite(L" nokeyboard-norecovery-tryagain='true'");
                    }
                }

            } else {
                if (BmFwBootToFirmwareSupported() != FALSE) {
                    BlXmiWrite(L" norecovery-firmware='true'");

                } else {
                    BlXmiWrite(L" norecovery='true'");
                }
            }
        }

    } else {
        if (BmFwBootToFirmwareSupported() != FALSE) {
            if (NoKeyboard != FALSE) {
                BlXmiWrite(L" nokeyboard-firmware-only='true'");

            } else {
                BlXmiWrite(L" keyboard-firmware-only='true'");
            }

        } else {
            BlXmiWrite(L" noaction='true'");
        }
    }

    BlXmiWrite(L"/>");
    return;
}

VOID
BmpDisplayDumpErrorStandard (
    __in ULONG Phase,
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in PINTERNAL_BOOT_ERROR ErrorLog,
    __in ULONG LaunchReason,
    __in BOOLEAN MultiBootSystem,
    __in BOOLEAN RecoveryAvailable,
    __in BOOLEAN OemRecoveryAvailable,
    __in BOOLEAN NoKeyboard
    )

/*++

Routine Description:

    This routine display the standard boot error screen.

Arguments:

    Phase - Supplies the display phase for the standard error screen.

    BootEntry - Supplies an optional pointer to a boot entry that experienced
        an error.  If this is supplied, the user should be given the option to
        recover from this error using the boot entry recovery sequence.

    ErrorLog - Supplies a pointer to the error information.

    LaunchReason - Supplies an indicating of why recovery needed to be
        launched. This is useful for displaying appropriate messages when
        recovery is missing.

    MultiBootSystem - Supplies a boolean indicating if this current system has
        more than one operating system.

    RecoveryAvailable - Supplies a boolean indicating if the windows recovery
        environment is available.

    OemRecoveryAvailable - Supplies a boolean indicating if an OEM recovery
        environment is available.

    NoKeyboard - Supplies a boolean indicating if no keyboard is present on the
        device.

Return Value:

    None.

--*/

{

    UNICODE_STRING CopiedString;
    PWCHAR Destination;
    PWCHAR ErrorInfo;
    NTSTATUS ErrorStatus;
    PWCHAR FileName;
    PWCHAR FooterString;
    PWCHAR HeaderString;
    PWCHAR HelpString;
    BM_ERROR_HELP_TYPE HelpType;
    PWCHAR InformationString;
    PCHAR SourceFile;
    ULONG SourceLine;
    NTSTATUS Status;
    WCHAR TempString[MAX_ERROR_STRING];
    PWCHAR TitleString;

#if DBG

    ULONG Index;

#endif

    //
    // Phase 1 destroys the display.
    //

    if (Phase == 1) {
        BmBgDisplayDestroy();
        return;
    }

    //
    // Unless there is an error, the user is going to have to provide input, so
    // initialize the key strokes before drawing the screen.
    //

    BlDisplayInitializeKeyStrokes();

    //
    // Initialize the generic screen contents.
    //

    ErrorStatus = ErrorLog->Status;
    ErrorInfo = ErrorLog->ErrorMsg;
    FileName = ErrorLog->FileName;
    HelpType = ErrorLog->HelpType;
    SourceLine = ErrorLog->SourceLine;
    SourceFile = ErrorLog->SourceFile;
    BmBgDisplayInitialize();
    HeaderString = BlResourceFindMessage(BM_ERROR_HEADER);
    BmBgDisplaySetHeader(HeaderString);
    if (LaunchReason == BSD_APP_LAUNCH_REASON_FVE_RECOVERY) {
        TitleString = BlResourceFindMessage(BM_ERROR_TITLE_NO_FVE_RECOVERY);

    } else if (LaunchReason == BSD_APP_LAUNCH_REASON_BAD_BOOT) {
        TitleString = BlResourceFindMessage(BM_ERROR_TITLE_BAD_BOOT);

    } else if (LaunchReason == BSD_APP_LAUNCH_REASON_BAD_SHUTDOWN) {
        TitleString = BlResourceFindMessage(BM_ERROR_TITLE_BAD_SHUTDOWN);

    } else if (LaunchReason == BSD_APP_LAUNCH_REASON_VSM_CANT_UNSEAL) {
        TitleString = BlResourceFindMessage(BM_ERROR_ENVIRONMENT_CANT_UNSEAL);

    } else if (HelpType == BmErrorHelpIo) {
        TitleString = BlResourceFindMessage(BM_ERROR_TITLE_IO);

    } else {
        TitleString = BlResourceFindMessage(BM_ERROR_TITLE_GENERIC);
    }

    BmBgDisplaySetTitle(TitleString);

    //
    // Add additional error information from the Error Log if this screen was
    // not manually requested.
    //

    if (LaunchReason != BSD_APP_LAUNCH_REASON_FVE_RECOVERY) {

        //
        // Display specific information pertaining to this error.
        //

        if (ErrorInfo != NULL) {
            if (RtlCreateUnicodeString(&CopiedString, ErrorInfo) != FALSE) {
                BmBgDisplayAddBodyRegion(CopiedString.Buffer, NULL);
                BmBgDisplayAddBodyRegion(NULL, NULL);
            }
        }

        if (FileName != NULL) {
            InformationString = BlResourceFindMessage(BM_ERROR_INFO_FILE);
            Status = BlConcatenateStringToString(InformationString,
                                                 L" ",
                                                 &Destination);

            if (NT_SUCCESS(Status)) {
                InformationString = Destination;
                Status = BlConcatenateStringToString(InformationString,
                                                     FileName,
                                                     &Destination);

                BlMmFreeHeap(InformationString);
                if (NT_SUCCESS(Status)) {
                    BmBgDisplayAddBodyRegion(Destination, NULL);
                }
            }
        }

        //
        // Dump the line and source file where this error ocurred.
        //

#if DBG

        if (SourceFile != NULL) {
            for (Index = 0;
                 ((Index < (MAX_ERROR_STRING - 1)) && (SourceFile[Index] != 0));
                 Index++) {

                TempString[Index] = SourceFile[Index];
            }

            TempString[Index] = 0;
            InformationString = BlResourceFindMessage(BM_ERROR_INFO_SOURCE);
            Status = BlConcatenateStringToString(InformationString,
                                                 L" ",
                                                 &Destination);

            if (NT_SUCCESS(Status)) {
                InformationString = Destination;
                Status = BlConcatenateStringToString(InformationString,
                                                     TempString,
                                                     &Destination);

                BlMmFreeHeap(InformationString);
                if (NT_SUCCESS(Status)) {
                    BmBgDisplayAddBodyRegion(Destination, NULL);
                }
            }

            swprintf_s(TempString,
                       sizeof(TempString) / sizeof(WCHAR),
                       L" %d",
                       SourceLine);

            InformationString = BlResourceFindMessage(BM_ERROR_INFO_LINE);
            Status = BlConcatenateStringToString(InformationString,
                                                 TempString,
                                                 &Destination);

            if (NT_SUCCESS(Status)) {
                BmBgDisplayAddBodyRegion(Destination, NULL);
            }
        }

#endif

        if (!NT_SUCCESS(ErrorStatus)) {
            swprintf_s(TempString,
                       sizeof(TempString) / sizeof(WCHAR),
                       L" 0x%08x",
                       ErrorStatus);

            InformationString = BlResourceFindMessage(BM_ERROR_INFO_ERROR_CODE);
            Status = BlConcatenateStringToString(InformationString,
                                                 TempString,
                                                 &Destination);

            if (NT_SUCCESS(Status)) {
                BmBgDisplayAddBodyRegion(Destination, NULL);
            }
        }
    }

    //
    // Determine which help information to display. I/O errors have their own
    // special help text. If recovery is not available, just display the
    // generic text. Otherwise, if more than 1 option will be displayed, then
    // display text telling the user to make a choice. The no keyboard, slate,
    // case is the only case where 1 recovery option will be displayed to the
    // user.
    //

    if (HelpType == BmErrorHelpIo) {
        HelpString = BlResourceFindMessage(BM_ERROR_HELP_IO);

    } else if (RecoveryAvailable == FALSE) {
        HelpString = BlResourceFindMessage(BM_ERROR_HELP_GENERIC);

    } else if (NoKeyboard == FALSE) {
        HelpString = BlResourceFindMessage(BM_ERROR_HELP_RECOVERY);

    } else {
        HelpString = NULL;
    }

    if (HelpString != NULL) {
        BmBgDisplayAddBodyRegion(NULL, NULL);
        BmBgDisplayAddBodyRegion(HelpString, NULL);
    }

    //
    // Initialize the input controls, adding footer regions to the screen. If
    // no boot entry was supplied, do not give the user any options.
    //

    BlXmiWrite(L"<osboot-error-standard ");
    if (ARGUMENT_PRESENT(BootEntry) != FALSE) {
        BlXmiWrite(L" advanced-options='true'");
        if (RecoveryAvailable != FALSE) {
            if (NoKeyboard != FALSE) {
                if (OemRecoveryAvailable != FALSE) {
                    BlXmiWrite(L" nokeyboard-recovery-oem='true'");

                } else {
                    BlXmiWrite(L" nokeyboard-recovery='true'");
                }

                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_RECOVERY_WINKEY);

                BmBgDisplayAddFooterRegion(FooterString);

            } else {
                if (MultiBootSystem != FALSE) {
                    BlXmiWrite(L" keyboard-multi='true'");
                    FooterString = BlResourceFindMessage(
                        BM_ERROR_FOOTER_NEXTOS_F9);

                    BmBgDisplayAddFooterRegion(FooterString);
                }

                if (OemRecoveryAvailable != FALSE) {
                    BlXmiWrite(L" keyboard-recovery-oem='true'");

                } else {
                    BlXmiWrite(L" keyboard-recovery='true'");
                }

                //
                // Display an F8 option if the boot entry is for an OS loader.
                //

                if (CHECK_FLAG(BootEntry->Attributes,
                               BOOT_ENTRY_OS_LOADER) != FALSE) {

                    FooterString = BlResourceFindMessage(
                                       BM_ERROR_FOOTER_ADVANCED_F8);

                    BmBgDisplayAddFooterRegion(FooterString);
                }

                BlXmiWrite(L" keyboard-tryagain='true'");
                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_TRYAGAIN_ENTER);

                BmBgDisplayAddFooterRegion(FooterString);
                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_RECOVERY_ESC);

                BmBgDisplayAddFooterRegion(FooterString);
            }

        } else {
            if (NoKeyboard != FALSE) {
                if (MultiBootSystem != FALSE) {
                    BlXmiWrite(L" nokeyboard-multi='true'");
                    FooterString = BlResourceFindMessage(
                                       BM_ERROR_FOOTER_NEXTOS_WINKEY);

                    BmBgDisplayAddFooterRegion(FooterString);

                } else {

                    //
                    // If the firmware supports stopping at its menus, present
                    // an option for the user to go to the firmware menus.
                    // Otherwise, allow the user to try again.
                    //

                    if (BmFwBootToFirmwareSupported() != FALSE) {
                        BlXmiWrite(L" nokeyboard-firmware='true'");
                        FooterString = BlResourceFindMessage(
                                           BM_ERROR_FOOTER_FIRMWARE_WINKEY);

                        BmBgDisplayAddFooterRegion(FooterString);

                    } else {
                        BlXmiWrite(L" nokeyboard-tryagain='true'");
                        FooterString = BlResourceFindMessage(
                                           BM_ERROR_FOOTER_TRYAGAIN_WINKEY);

                        BmBgDisplayAddFooterRegion(FooterString);
                    }
                }

            } else {
                if (MultiBootSystem != FALSE) {
                    BlXmiWrite(L" keyboard-multi='true'");
                    FooterString = BlResourceFindMessage(
                                       BM_ERROR_FOOTER_NEXTOS_F9);

                    BmBgDisplayAddFooterRegion(FooterString);
                }

                //
                // If the firmware supports stopping at its menus, present an
                // option for the user to go to the firmware menus.
                //

                if (BmFwBootToFirmwareSupported() != FALSE) {
                    BlXmiWrite(L" keyboard-firmware='true'");
                    FooterString = BlResourceFindMessage(
                                       BM_ERROR_FOOTER_FIRMWARE_ESC);

                    BmBgDisplayAddFooterRegion(FooterString);
                }

                //
                // Display an F8 option if the boot entry is for an OS loader.
                //

                if (CHECK_FLAG(BootEntry->Attributes,
                               BOOT_ENTRY_OS_LOADER) != FALSE) {

                    FooterString = BlResourceFindMessage(
                                       BM_ERROR_FOOTER_ADVANCED_F8);

                    BmBgDisplayAddFooterRegion(FooterString);
                }

                BlXmiWrite(L" keyboard-tryagain='true'");
                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_TRYAGAIN_ENTER);

                BmBgDisplayAddFooterRegion(FooterString);
            }
        }

    } else {
        if (BmFwBootToFirmwareSupported() != FALSE) {
            if (NoKeyboard != FALSE) {
                BlXmiWrite(L" nokeyboard-firmware='true'");
                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_FIRMWARE_WINKEY);

            } else {
                BlXmiWrite(L" keyboard-firmware='true'");
                FooterString = BlResourceFindMessage(
                                   BM_ERROR_FOOTER_FIRMWARE_ESC);

            }

            BmBgDisplayAddFooterRegion(FooterString);

        } else {
            BlXmiWrite(L" noaction='true'");
        }
    }

    BlXmiWrite(L"/>");

    //
    // Render to the screen.
    //

    BmBgDisplayRender(FALSE);
    return;
}

VOID
BmpDisplayErrorSimple (
    __in ULONG Phase
    )

/*++

Routine Description:

    This routine display a simple boot error screen.

Arguments:

    Phase - Supplies the display phase for the simple error screen.

Return Value:

    None.

--*/

{
    PWCHAR SimpleString;

    //
    // Phase 1 destroys the display.
    //

    if (Phase == 1) {
        BmBgDisplayDestroy();
        return;
    }

    //
    // Initialize the generic screen contents.
    //

    BmBgDisplayInitialize();

    SimpleString = BlResourceFindMessage(BM_ERROR_SIMPLE_EMOTICON);
    BmBgDisplaySetSimpleMessage(SimpleString);

    BmBgDisplayRender(TRUE);

    return;
}


VOID
BmpDisplayLegacyMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN Display
    )

/*++

Routine Description:

    This routine is called to display and remove messages which are used to
    indicate to the user that the boot manager is waiting for a multicast
    session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

    Display - Supplies TRUE to display the message, FALSE to remove the message.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    PPXE_INFO PxeInformation;
    WCHAR ServerAddress[PXE_IP_ADDRESS_STRING_BUFFER_LENGTH];

    UNREFERENCED_PARAMETER(Parameter);

    if (Display == FALSE) {
        BlXmiWrite(L"<bootmgr-clear-screen/>");

    } else {
        PxeInformation = BlNetGetPxeInfo();
        if (PxeInformation != NULL) {
            Status = BlNetAddressToString(ServerAddress,
                                          sizeof(ServerAddress) / sizeof(ServerAddress[0]),
                                          &PxeInformation->ServerAddress);

            if (NT_SUCCESS(Status)) {
                BlXmiWrite(L"<multicast-prestart-legacy wds='");
                BlXmiWrite(ServerAddress);
                BlXmiWrite(L"'/>");
            }
        }
    }

    return;
}

VOID
BmpDisplayStandardMulticastMessage (
    __in PVOID Parameter,
    __in BOOLEAN Display
    )

/*++

Routine Description:

    This routine is called to display and remove messages which are used to
    indicate to the user that the boot manager is waiting for a multicast
    session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

    Display - Supplies TRUE to display the message, FALSE to remove the message.

Return Value:

    None.

--*/

{

    PWCHAR Destination;
    PWCHAR DisplayString;
    PWCHAR MulticastString;
    PPXE_INFO PxeInformation;
    WCHAR ServerAddress[PXE_IP_ADDRESS_STRING_BUFFER_LENGTH];
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Parameter);

    Destination = NULL;
    if (Display == FALSE) {
        BgDisplayProgressIndicator(FALSE);

    } else {
        BlXmiWrite(L"<multicast-prestart-standard/>");
        MulticastString = BlResourceFindMessage(BM_DISPLAY_STRING_MULTICAST);
        PxeInformation = BlNetGetPxeInfo();
        if (PxeInformation != NULL) {
            Status = BlNetAddressToString(ServerAddress,
                                          sizeof(ServerAddress) / sizeof(ServerAddress[0]),
                                          &PxeInformation->ServerAddress);
            if (!NT_SUCCESS(Status)) {
                goto DisplayStandardMulticastMessageEnd;
            }

            Status = BlConcatenateStringToString(MulticastString,
                                                 L" IP: ",
                                                 &Destination);
            if (!NT_SUCCESS(Status)) {
                goto DisplayStandardMulticastMessageEnd;
            }

            Status = BlConcatenateStringToString(Destination,
                                                 ServerAddress,
                                                 &DisplayString);

            BlMmFreeHeap(Destination);
            if (!NT_SUCCESS(Status)) {
                goto DisplayStandardMulticastMessageEnd;
            }

        } else {
            DisplayString = MulticastString;
        }

        BgDisplayString(DisplayString);
        BlMmFreeHeap(DisplayString);
        BgDisplayProgressIndicator(TRUE);
    }

DisplayStandardMulticastMessageEnd:
    return;
}

NTSTATUS
BmpUpdateLegacyMulticastMessage (
    __in PVOID Parameter
    )

/*++


Routine Description:

    This routine updates the spinner on the display to indicate that the boot
    manager is still alive and waiting for the multicast session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

Return Value:

    STATUS_SUCCESS on success.
    STATUS_CANCELLED if user has cancelled the download.

--*/

{

    PXML_DOC Post;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Parameter);

    //
    // Update spinner on display.
    //

    Status = STATUS_SUCCESS;
    BlXmiWrite(L"<multicast-activity-update value='");
    BlXmiWrite(BmSpinner[BmSpinnerIndex]);
    BlXmiWrite(L"'/>");
    if ((BmSpinnerIndex + 1) == (sizeof(BmSpinner) / sizeof(BmSpinner[0]))) {
        BmSpinnerIndex = 0;

    } else {
        BmSpinnerIndex += 1;
    }

    //
    // Check if user has cancelled the operation.
    //

    Post = BlXmiRead(FALSE);
    if (Post == NULL) {
        goto UpdateMulticastMessageEnd;
    }

    if (OsxmlResultHasAction(Post, L"exit") != FALSE) {
        Status = STATUS_CANCELLED;
        goto UpdateMulticastMessageEnd;
    }

UpdateMulticastMessageEnd:
    if (Post != NULL) {
        ObjRelease(Post);
    }

    return Status;
}

VOID
BmpUpdateLegacyProgress (
    __in PVOID Context,
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    )

/*++

Routine Description:

    This routine shows the progress bar of an image loading.

    TODO: This routine was copied from lib\io\device\ramapi.c where it
    was originally written to show progress for ramdisk loading.  The decision
    was made to move the progress notification out of the library and into the
    app.  Eventually a XML-based progress indication should be implemented.
    This one will be used for now.  Currently the only consumer of
    BlUtlUpdateProgress, which causes this routine to be called, is the
    ramdisk support.

Arguments:

    Context - Supplies a pointer to a structure containing progress bar
        context.

    PercentComplete - Supplies the current percentage of completion.

    ProgressDisplayComplete - Supplies a boolean that receives a value
        indicating if the progress display is complete.

Return Value:

    None.

--*/

{

    BOOLEAN Complete;
    PWCHAR OsxmlMessage;
    PWCHAR ProgressMessage;
    NTSTATUS Status;
    WCHAR String[sizeof(L"100'/>")];

    UNREFERENCED_PARAMETER(Context);

    if (PercentComplete == 0) {
        BlXmiWrite(L"<progress-bar progresstitle='");
        ProgressMessage = BmpDisplayGetProgressMessage();
        Status = BlXmiTransformTextForXml(ProgressMessage,
                                          &OsxmlMessage);

        if (NT_SUCCESS(Status)) {
            BlXmiWrite(OsxmlMessage);
            BlMmFreeHeap(OsxmlMessage);
        }

        BlXmiWrite(L"'/>");
        Complete = FALSE;

    } else if (PercentComplete == 100) {
        BlXmiWrite(L"<bootmgr-clear-screen/>");
        Complete = TRUE;

    } else {
        BlXmiWrite(L"<progress-update value='");
        swprintf_s(String,
                   sizeof(String) / sizeof(WCHAR),
                   L"%d'/>",
                   PercentComplete);

        BlXmiWrite(String);
        Complete = FALSE;
    }

    if (ARGUMENT_PRESENT(ProgressDisplayComplete) != FALSE) {
        *ProgressDisplayComplete = Complete;
    }

    return;
}

VOID
BmpUpdateLegacyProgressInformation (
    __in PVOID Context,
    __in PWSTR ProgressInformation
    )

/*++

Routine Description:

    This routine displays additional information about the progress bar.

Arguments:

    Context - Supplies a pointer to a structure containing progress bar
        context.

    ProgressInformation - Supplies additional info for the progress indicator.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);

    BlXmiWrite(L"<progress-info value='");
    BlXmiWrite(ProgressInformation);
    BlXmiWrite(L"'/>");
    return;
}

NTSTATUS
BmpUpdateStandardMulticastMessage (
    __in PVOID Parameter
    )

/*++


Routine Description:

    This routine updates the animation on the display to indicate that the boot
    manager is still alive and waiting for the multicast session to start.

Arguements:

    Parameter - (unused) Supplies callback specific parameter.

Return Value:

    STATUS_SUCCESS on success.
    STATUS_CANCELLED if user has cancelled the download.

--*/

{

    PXML_DOC Post;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Parameter);

    //
    // Destroy the progress animation if the progress in complete.
    //

    BgDisplayProgressIndicator(TRUE);

    //
    // Check if user has cancelled the operation, turning off the progress
    // indicator if the user did cancel.
    //

    Status = STATUS_SUCCESS;
    Post = BlXmiRead(FALSE);
    if (Post == NULL) {
        goto UpdateMulticastMessageEnd;
    }

    if (OsxmlResultHasAction(Post, L"exit") != FALSE) {
        BgDisplayProgressIndicator(FALSE);
        Status = STATUS_CANCELLED;
        goto UpdateMulticastMessageEnd;
    }

UpdateMulticastMessageEnd:
    if (Post != NULL) {
        ObjRelease(Post);
    }

    return Status;
}

VOID
BmpUpdateStandardProgress (
    __in PVOID Context,
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    )

/*++

Routine Description:

    This routine shows the progress message during image load.

Arguments:

    Context - Supplies a pointer to a structure containing progress bar
        context.

    PercentComplete - Supplies the current percentage of completion.

    ProgressDisplayComplete - Supplies a boolean that receives a value
        indicating if the progress display is complete.

Return Value:

    None.

--*/

{

    BOOLEAN OemValid;
    PWCHAR ProgressMessage;

    UNREFERENCED_PARAMETER(Context);

    //
    // Display the background logo and text the first time progress is display.
    //

    if (PercentComplete == 0) {
        OemValid = BlDisplayValidOemBitmap(TRUE);
        if (OemValid == FALSE) {
            BgDisplayBackgroundImage();
        }

        ProgressMessage = BmpDisplayGetProgressMessage();
        BgDisplayString(ProgressMessage);
    }

    //
    // Standard progress only takes action on the first call. This first call
    // and all subsequent calls should return that it is complete.
    //

    if (ARGUMENT_PRESENT(ProgressDisplayComplete) != FALSE) {
        *ProgressDisplayComplete = TRUE;
    }

    return;
}

VOID
BmpUpdateStandardProgressInformation (
    __in PVOID Context,
    __in PWSTR ProgressInformation
    )

/*++

Routine Description:

    This routine updates the boot ux display string with the provided progress
    information.

Arguments:

    Context - Supplies context for the progress information.

    ProgressInformation - Supplies progress information to be display to the
        screen.

Return Value:

    None.

--*/

{

    PWCHAR DisplayString;
    PWCHAR ProgressMessage;
    PWCHAR ScratchString;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Update the display text with the progress info appended to the message
    // string.
    //

    ProgressMessage = BmpDisplayGetProgressMessage();
    Status = BlConcatenateStringToString(ProgressMessage,
                                         L" ",
                                         &ScratchString);

    if (!NT_SUCCESS(Status)) {
        return;
    }

    Status = BlConcatenateStringToString(ScratchString,
                                         ProgressInformation,
                                         &DisplayString);

    BlMmFreeHeap(ScratchString);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    BgDisplayString(DisplayString);
    BlMmFreeHeap(DisplayString);
    return;
}

