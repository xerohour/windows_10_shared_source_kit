/*++

Copyright (c) Microsoft Corporation

Module Name:

    fvedisp.c

Abstract:

    User Interface for the boot manager's Bitlocker UI.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"

//
// -------------------------------------------------------------------- Globals
//

//
// Contains the total time spent waiting for user input at bitlocker
// display screens.
//

LARGE_INTEGER BmFveUserInputTime;

//
// ---------------------------------------------------------------- Definitions
//

//
// Define recovery entry constants.
//

#define ENTRY_ROW 3
#define ENTRY_COLUMN 1
#define PASSPHRASE_COLS_PER_ROW 76
#define CHECKSUM 11
#define FLASH_CURSOR_INTERVAL 16
#define BAD_OPTION_LENGTH 11

#define LEGACY_WINDOW_SIZE 55

//
// Recovery message constraints
//

#define MAX_RECOVERY_MESSAGE_CCH 900
#define MAX_RECOVERY_URL_CCH 800

//
// End-of-Tranmission character.
//

#define EOT 4

//
// Hidden passphrase, PIN, ePIN characters.
//

#define BULLET 0x25CF
#define ASTERISK L'*'

//
// Password group separator character.
//

#define SEPARATOR 0x2013

//
// Footer flags for different options.
//

#define FVE_FOOTER_FLAGS_NONE             0x00000000
#define FVE_FOOTER_FLAGS_ENTER_REBOOT     0x00000001
#define FVE_FOOTER_FLAGS_ESC_MORE_OPTIONS 0x00000002
#define FVE_FOOTER_FLAGS_F11_ENABLED      0x00000004
#define FVE_FOOTER_FLAGS_NO_KEYBOARD      0x00000008
#define FVE_FOOTER_FLAGS_SKIP_ESC         0x00000010

//
// Display timeout for FVE screens in seconds.
//

#define FVE_DISPLAY_TIMEOUT               60

//
// -------------------------------------------------- Local Function Prototypes
//

PWCHAR
FvepDisplayCreateBadCodeIdString (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

PWCHAR
FvepDisplayCreateBadCodeOptionString (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

PWCHAR
FvepDisplayCreateRecoveryUrlString(
    __in ULONG DefaultMessageId,
    __in ULONG DefaultUrlId
    );

NTSTATUS
FvepDisplayDefault (
    __out PULONG RecoveryFlags
    );

BOOLEAN
FvepDisplayFlashCursor (
    __in ULONG CursorPosition
    );

BOOL
FvepDisplayFullGroup (
    __in_ecount(KEY_DIGITS) PCHAR KeyChars,
    __in ULONG KeyInputLocation
    );

PWCHAR
FvepDisplayGetTitle (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepDisplayInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    );

VOID
FvepDisplayInitializeFunctions (
    VOID
    );

VOID
FvepDisplayInitializeRecoveryInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepDisplayInitializeTitle (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

BOOLEAN
FvepDisplayIsResume (
    VOID
    );

BOOLEAN
FvepDisplayGetMultiBootState (
    VOID
    );

BOOLEAN
FvepDisplayIsPrintableCharacter (
    __in KEY_STROKE KeyStroke
    );

VOID
FvepDisplayLog (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

BOOLEAN
FvepDisplayNeedRecoveryEnvironment (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepDisplayNumberForFunctionChar (
    __inout PKEY_STROKE KeyStroke
    );

NTSTATUS
FvepDisplayPassphrase (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    );

NTSTATUS
FvepDisplayPassphraseConsole (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    );

NTSTATUS
FvepDisplayPassphraseOverride (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext
    );

NTSTATUS
FvepDisplayPassword (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext,
    __out PULONG RecoveryFlags
    );

NTSTATUS
FvepDisplayPasswordConsole (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext,
    __out PULONG RecoveryFlags
    );

NTSTATUS
FvepDisplayPasswordOverride (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext
    );

NTSTATUS
FvepDisplayPin (
    __inout PFVE_PIN_CONTEXT PinContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    );

NTSTATUS
FvepDisplayPinConsole (
    __inout PFVE_PIN_CONTEXT PinContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    );

NTSTATUS
FvepDisplayPinOverride (
    __inout PFVE_PIN_CONTEXT PinContext
    );

BOOLEAN
FvepDisplayRequiresInput (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepDisplayString (
    __in ULONG MessageId,
    __in BOOLEAN StripNewline
    );

VOID
FvepDisplayStripNewline (
    __inout_z PWCHAR String
    );

VOID
FvepDisplayUpdatePassphrase (
    __in_ecount_z(FVEB_MAX_PASSPHRASE_LENGTH + 1) PWCHAR Passphrase,
    __in ULONG PassphraseFlags,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

VOID
FvepDisplayUpdatePassword (
    __in_ecount_z(KEY_DIGITS + 1) PCHAR Password,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

VOID
FvepDisplayUpdatePin (
    __in_ecount_z(FVEB_MAX_PIN_DIGITS + 1) PWCHAR Pin,
    __in ULONG PinFlags,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

BOOL
FvepDisplayValidGroup (
    __in_ecount(KEY_DIGITS) PCHAR KeyChars,
    __in ULONG KeyInputLocation
    );

VOID
FvepLegacyClearScreen (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepLegacyInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    );

VOID
FvepLegacyInitializeErrorInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepLegacyInitializeFooter (
    __in ULONG FooterFlags
    );

VOID
FvepLegacyInitializeHelpInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __out HANDLE *HideShowHandle
    );

VOID
FvepLegacyInitializeInput (
    VOID
    );

VOID
FvepLegacyInitializeRecoveryInformation (
    __in_opt PWSTR RecoveryErrorString
    );

VOID
FvepLegacyInitializeTitle (
    __in_z PWCHAR Title
    );

VOID
FvepLegacyUpdateHelpInformation (
    __in ULONG MessageId,
    __in HANDLE Handle
    );

VOID
FvepLegacyUpdateInput (
    __in_z PWCHAR Input,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

VOID
FvepLegacyUpdateTitle (
    __in ULONG TitleId
    );

BOOLEAN
FvepScanFullGroup (
    __in_ecount(DIGIT_GROUP_SIZE) PCHAR KeyChars,
    __out PULONG GroupValue
    );

VOID
FvepStandardClearScreen (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepStandardInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    );

VOID
FvepStandardInitializeErrorInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

VOID
FvepStandardInitializeFooter (
    __in ULONG FooterFlags
    );

VOID
FvepStandardInitializeHelpInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __out HANDLE *HideShowHandle
    );

VOID
FvepStandardInitializeInput (
    VOID
    );

VOID
FvepStandardInitializeRecoveryInformation (
    __in_opt PWSTR RecoveryErrorString
    );

VOID
FvepStandardInitializeTitle (
    __in_z PWCHAR Title
    );

VOID
FvepStandardUpdateHelpInformation (
    __in ULONG MessageId,
    __in HANDLE Handle
    );

VOID
FvepStandardUpdateInput (
    __in_z PWCHAR Input,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

VOID
FvepStandardUpdateTitle (
    __in ULONG TitleId
    );

typedef
VOID
(*FVE_DISPLAY_CLEAR_SCREEN) (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE) (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_ERROR_INFORMATION) (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_FOOTER) (
    __in ULONG FooterFlags
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_HELP_INFORMATION) (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __out HANDLE *HideShowHandle
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_INPUT) (
    VOID
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_RECOVERY_INFORMATION) (
    __in_opt PWSTR RecoveryErrorString
    );

typedef
VOID
(*FVE_DISPLAY_INITIALIZE_TITLE) (
    __in_z PWCHAR Title
    );

typedef
VOID
(*FVE_DISPLAY_UPDATE_HELP) (
    __in ULONG MessageId,
    __in HANDLE Handle
    );

typedef
VOID
(*FVE_DISPLAY_UPDATE_INPUT) (
    __in_z PWCHAR Input,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    );

typedef
VOID
(*FVE_DISPLAY_UPDATE_TITLE) (
    __in ULONG TitleId
    );

//
// --------------------------------------------------------- Display Interfaces
//

typedef struct _FVE_DISPLAY_INTERFACE {
    FVE_DISPLAY_INITIALIZE Initialize;
    FVE_DISPLAY_INITIALIZE_TITLE InitializeTitle;
    FVE_DISPLAY_INITIALIZE_INPUT InitializeInput;
    FVE_DISPLAY_INITIALIZE_RECOVERY_INFORMATION InitializeRecoveryInformation;
    FVE_DISPLAY_INITIALIZE_ERROR_INFORMATION InitializeErrorInformation;
    FVE_DISPLAY_INITIALIZE_HELP_INFORMATION InitializeHelpInformation;
    FVE_DISPLAY_INITIALIZE_FOOTER InitializeFooter;
    FVE_DISPLAY_UPDATE_TITLE UpdateTitle;
    FVE_DISPLAY_UPDATE_INPUT UpdateInput;
    FVE_DISPLAY_UPDATE_HELP UpdateHelpInformation;
    FVE_DISPLAY_CLEAR_SCREEN ClearScreen;
} FVE_DISPLAY_INTERFACE;
typedef const FVE_DISPLAY_INTERFACE *PCFVE_DISPLAY_INTERFACE;

static const FVE_DISPLAY_INTERFACE StandardDisplayFunctions = {
    FvepStandardInitialize,
    FvepStandardInitializeTitle,
    FvepStandardInitializeInput,
    FvepStandardInitializeRecoveryInformation,
    FvepStandardInitializeErrorInformation,
    FvepStandardInitializeHelpInformation,
    FvepStandardInitializeFooter,
    FvepStandardUpdateTitle,
    FvepStandardUpdateInput,
    FvepStandardUpdateHelpInformation,
    FvepStandardClearScreen
};

static const FVE_DISPLAY_INTERFACE LegacyDisplayFunctions = {
    FvepLegacyInitialize,
    FvepLegacyInitializeTitle,
    FvepLegacyInitializeInput,
    FvepLegacyInitializeRecoveryInformation,
    FvepLegacyInitializeErrorInformation,
    FvepLegacyInitializeHelpInformation,
    FvepLegacyInitializeFooter,
    FvepLegacyUpdateTitle,
    FvepLegacyUpdateInput,
    FvepLegacyUpdateHelpInformation,
    FvepLegacyClearScreen
};

PCFVE_DISPLAY_INTERFACE FveFunctions;

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BmFveDisplayScreen (
    __inout PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine manages drawing the screen contents for the Bitlocker user
    interface.

Arguments:

    DisplayContext - Supplies a pointer to an FVE display context that
        describes the content that needs to be shown.

Return Value:

    NT status code.

--*/

{

    PBOOT_APPLICATION_ENTRY BootEntry;
    HANDLE HideShowHandle;
    LARGE_INTEGER StartTime;
    NTSTATUS Status;

    //
    // Log the reason for displaying the FVE screen to the user.
    //
    // N.B. This needs to happen prior to returning FVE locked status
    //      so that WinRE understands the issue, and can provide the
    //      appropriate UI.
    //

    FvepDisplayLog(DisplayContext);
    if (FvepDisplayNeedRecoveryEnvironment(DisplayContext) != FALSE) {
        BootEntry = BmGetSelectedBootEntry();
        if ((BootEntry != NULL) &&
            (BlBootOptionExists(BootEntry->Options,
                                BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE))) {

            return STATUS_FVE_LOCKED_VOLUME;
        }
    }

    //
    // Initialize the FVE display if it has not yet been done.
    //

    if (FveFunctions == NULL) {
        FvepDisplayInitializeFunctions();
    }

    //
    // If the end screen is requested, just wipe the screen and exit. Also set
    // the function table pointer to NULL as the next application requiring
    // bitlocker input might not have the same menu policy.
    //

    if (DisplayContext->ScreenType == FveScreenClear) {
        FveFunctions->ClearScreen(DisplayContext);
        FveFunctions = NULL;
        return STATUS_SUCCESS;
    }

    //
    // Initialize the display, opening the XSL template for the screen. This
    // happens for both Legacy and Standard bitlocker UI, as the Standard case
    // requires a key listener to be enabled.
    //

    FvepDisplayInitialize(DisplayContext, 0);

    //
    // Display the general screen contents.
    //

    FvepDisplayInitializeTitle(DisplayContext);
    FvepDisplayInitializeRecoveryInformation(DisplayContext);
    FveFunctions->InitializeErrorInformation(DisplayContext);
    FveFunctions->InitializeHelpInformation(DisplayContext, &HideShowHandle);

    //
    // Complete display initialization. This will cause the display to be
    // drawn.
    //

    FvepDisplayInitialize(DisplayContext, 1);

    //
    // Call the display routine for the particular screen.
    //

    StartTime = BlTimeQueryPerformanceCounter(NULL);
    switch (DisplayContext->ScreenType) {
    case FveScreenPassword:
    case FveScreenPasswordIncorrect:
        Status = FvepDisplayPassword(&DisplayContext->PasswordContext,
                                     DisplayContext->RecoveryFlags);

        break;

    case FveScreenPin:
    case FveScreenPinIncorrect:
        Status = FvepDisplayPin(&DisplayContext->PinContext,
                                DisplayContext->RecoveryFlags,
                                HideShowHandle);

        break;

    case FveScreenPassphrase:
    case FveScreenPassphraseIncorrect:
        Status = FvepDisplayPassphrase(&DisplayContext->PassphraseContext,
                                       DisplayContext->RecoveryFlags,
                                       HideShowHandle);

        break;

    default:
        Status = FvepDisplayDefault(DisplayContext->RecoveryFlags);
        break;
    }

    BmFveUserInputTime.QuadPart +=
        BlTimeQueryPerformanceCounter(NULL).QuadPart - StartTime.QuadPart;

    return Status;
}

//
// ------------------------------------------------------------ Local Functions
//

PWCHAR
FvepDisplayCreateBadCodeIdString (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine creates the recovery text for a BAD_CODE_ID error. It requires
    parsing the resource string to look for the insert location of the
    application file name.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    PWCHAR CopyDestination;
    PWCHAR Destination;
    SIZE_T DestinationSize;
    PWSTR FileName;
    PWCHAR FileNameOffset;
    SIZE_T FileNameSize;
    SIZE_T PartOneSize;
    SIZE_T PartTwoSize;
    PWCHAR String;

    //
    // Obtain the string resource.
    //

    String = BlResourceFindMessage(BM_FVE_BAD_CODE_ID);

    NT_ASSERT(String != NULL);

    //
    // Get the file name from the correct context.
    //

    if (DisplayContext->ScreenType == FveScreenUsbRecoveryInsert) {
        FileName = DisplayContext->ExternalContext.FileName;

    } else {

        ASSERT((DisplayContext->ScreenType == FveScreenPassword) ||
               (DisplayContext->ScreenType == FveScreenPasswordIncorrect));

        FileName = DisplayContext->PasswordContext.FileName;
    }

    //
    // Find the size of the destination string. It is comprised of three
    // parts: the resource string before the file name, the file name, and the
    // resource string after the file name.
    //

    FileNameOffset = wcsstr(String, L"%1");

    NT_ASSERT(FileNameOffset != NULL);
    NT_ASSERT(FileName != NULL);

    PartOneSize = (ULONG_PTR)FileNameOffset - (ULONG_PTR)String;
    FileNameSize = wcslen(FileName) * sizeof(WCHAR);
    FileNameOffset += wcslen(L"%1");
    PartTwoSize = wcslen(FileNameOffset) * sizeof(WCHAR);
    DestinationSize = PartOneSize + FileNameSize + PartTwoSize + sizeof(WCHAR);

    //
    // Allocate the string.
    //

    Destination = BlMmAllocateHeap(DestinationSize);
    if (Destination == NULL) {
        return NULL;
    }

    //
    // Copy contents into the string.
    //

    CopyDestination = Destination;
    RtlCopyMemory(CopyDestination, String, PartOneSize);
    CopyDestination = Add2Ptr(CopyDestination, PartOneSize);
    RtlCopyMemory(CopyDestination, FileName, FileNameSize);
    CopyDestination = Add2Ptr(CopyDestination, FileNameSize);
    RtlCopyMemory(CopyDestination, FileNameOffset, PartTwoSize);
    CopyDestination = Add2Ptr(CopyDestination, PartTwoSize);
    *CopyDestination = L'\0';
    return Destination;
}

PWCHAR
FvepDisplayCreateBadCodeOptionString (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine creates the recovery text for a BAD_CODE_OPTION error. It
    requires parsing the resource string to look for the insert location of the
    bad BCD option and the application file name.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    ULONG BadOption;
    PWCHAR BadOptionOffset;
    LONG BadOptionLength;
    SIZE_T BadOptionSize;
    WCHAR BadOptionString[BAD_OPTION_LENGTH];
    PWCHAR CopyDestination;
    PWCHAR Destination;
    SIZE_T DestinationSize;
    PWSTR FileName;
    PWCHAR FileNameOffset;
    SIZE_T FileNameSize;
    SIZE_T PartOneSize;
    SIZE_T PartThreeSize;
    SIZE_T PartTwoSize;
    PWCHAR String;

    //
    // Obtain the string resource.
    //

    String = BlResourceFindMessage(BM_FVE_BAD_CODE_OPTION);

    NT_ASSERT(String != NULL);

    //
    // Get the file name from the correct context.
    //

    if (DisplayContext->ScreenType == FveScreenUsbRecoveryInsert) {
        FileName = DisplayContext->ExternalContext.FileName;
        BadOption = DisplayContext->ExternalContext.BadOption;

    } else {

        ASSERT((DisplayContext->ScreenType == FveScreenPassword) ||
               (DisplayContext->ScreenType == FveScreenPasswordIncorrect));

        FileName = DisplayContext->PasswordContext.FileName;
        BadOption = DisplayContext->PasswordContext.BadOption;
    }

    //
    // Find the size of the destination string. It is comprised of three
    // parts: the resource string before the file name, the file name, and the
    // resource string after the file name.
    //

    BadOptionOffset = wcsstr(String, L"%1");

    NT_ASSERT(BadOptionOffset != NULL);

    FileNameOffset = wcsstr(BadOptionOffset, L"%2");

    NT_ASSERT(FileNameOffset != NULL);
    NT_ASSERT(FileName != NULL);

    PartOneSize = (ULONG_PTR)BadOptionOffset - (ULONG_PTR)String;
    BadOptionSize = BAD_OPTION_LENGTH * sizeof(WCHAR);
    BadOptionOffset += wcslen(L"%1");
    PartTwoSize = (ULONG_PTR)FileNameOffset - (ULONG_PTR)BadOptionOffset;
    FileNameSize = wcslen(FileName) * sizeof(WCHAR);
    FileNameOffset += wcslen(L"%2");
    PartThreeSize = wcslen(FileNameOffset) * sizeof(WCHAR);

    //
    // Remove the NULL character from the bad option size and calculate the
    // total size.
    //

    BadOptionSize -= sizeof(WCHAR);
    DestinationSize = PartOneSize + BadOptionSize + PartTwoSize;
    DestinationSize += FileNameSize + PartThreeSize + sizeof(WCHAR);

    //
    // Allocate the string.
    //

    Destination = BlMmAllocateHeap(DestinationSize);
    if (Destination == NULL) {
        return NULL;
    }

    //
    // Create the Bad Option String Buffer.
    //

    BadOptionLength = swprintf_s(BadOptionString,
                                 BAD_OPTION_LENGTH,
                                 L"0x%x",
                                 BadOption);

    ASSERT(BadOptionLength == (BAD_OPTION_LENGTH - 1));

    //
    // Copy contents into the string.
    //

    CopyDestination = Destination;
    RtlCopyMemory(CopyDestination, String, PartOneSize);
    CopyDestination = Add2Ptr(CopyDestination, PartOneSize);
    RtlCopyMemory(CopyDestination, BadOptionString, BadOptionSize);
    CopyDestination = Add2Ptr(CopyDestination, BadOptionSize);
    RtlCopyMemory(CopyDestination, BadOptionOffset, PartTwoSize);
    CopyDestination = Add2Ptr(CopyDestination, PartTwoSize);
    RtlCopyMemory(CopyDestination, FileName, FileNameSize);
    CopyDestination = Add2Ptr(CopyDestination, FileNameSize);
    RtlCopyMemory(CopyDestination, FileNameOffset, PartThreeSize);
    CopyDestination = Add2Ptr(CopyDestination, PartThreeSize);
    *CopyDestination = L'\0';
    return Destination;
}

PWCHAR
FvepDisplayCreateRecoveryUrlString(
    __in ULONG DefaultMessageId,
    __in ULONG DefaultUrlId
    )

/*++

Routine Description:

This routine constructs the recovery URL string. This string is composed
using default resource strings then overriding values with the BCD
FveRecoveryUrl or FveRecoveryMessage values if present. If malformed or
invalid data is found, the resource strings are used as the fallback to
display the default recovery message.

Arguments:

DefaultMessageId - String resource id of the default message id.

DefaultUrlId - String resource id of the default recovery URL.

Return Value:

The recovery message to display. May be NULL on failure to allocate
memory.

--*/

{
    PWCHAR BcdMessageString;
    PWCHAR BcdUrlString;
    PWCHAR DefaultMessageString;
    PWCHAR DefaultUrlString;
    PWCHAR Destination;
    SIZE_T DestinationSize;
    PWCHAR MessageString;
    SIZE_T PartOneSize;
    SIZE_T PartThreeSize;
    PWCHAR PartThreeString;
    NTSTATUS Status;
    PWCHAR UrlOffset;
    SIZE_T UrlSize;
    PWCHAR UrlString;

    //
    // Return value.
    //

    Destination = NULL;

    //
    // Obtain the default strings from string resources.
    //

    DefaultMessageString = BlResourceFindMessage(DefaultMessageId);
    NT_ASSERT(DefaultMessageString != NULL);

    DefaultUrlString = BlResourceFindMessage(DefaultUrlId);
    NT_ASSERT(DefaultUrlString != NULL);

    //
    // Obtain the recovery message string from the BCD. Truncate if
    // too long.  If the default message is used, determine the URL 
    // substitution offset.
    //

    UrlOffset = NULL;
    BcdMessageString = NULL;

    Status = BlGetApplicationOptionString(
        BCDE_BOOTMGR_TYPE_FVE_RECOVERY_MESSAGE,
        &BcdMessageString);

    if (NT_SUCCESS(Status)) {

        if (wcslen(BcdMessageString) >= MAX_RECOVERY_MESSAGE_CCH) {
            BcdMessageString[MAX_RECOVERY_MESSAGE_CCH - 1] = UNICODE_NULL;
        }

        MessageString = BcdMessageString;
    } else {
        MessageString = DefaultMessageString;
        UrlOffset = wcsstr(MessageString, L"%1");
    }

    //
    // Obtain the recovery URL string from the BCD. Truncate if too long.
    //

    BcdUrlString = NULL;
    Status = BlGetApplicationOptionString(
        BCDE_BOOTMGR_TYPE_FVE_RECOVERY_URL,
        &BcdUrlString);

    if (NT_SUCCESS(Status)) {

        if (wcslen(BcdUrlString) >= MAX_RECOVERY_URL_CCH) {
            BcdUrlString[MAX_RECOVERY_URL_CCH - 1] = UNICODE_NULL;
        }

        UrlString = BcdUrlString;

    } else {
        UrlString = DefaultUrlString;
    }

    //
    // Create and load the buffer to return.
    //

    if (UrlOffset == NULL) {

        //
        // The message was defined in the BCD so there is no URL substitution
        //

        DestinationSize = wcslen(MessageString) + 1;

        Destination = BlMmAllocateHeap(DestinationSize * sizeof(WCHAR));
        if (Destination == NULL) {
            goto DisplayCreateRecoveryUrlStringEnd;
        }

        wcscpy_s(Destination, DestinationSize, MessageString);
    } else {

        //
        // Find the size of the destination string. It is comprised of three
        // parts: the resource string before the URL, the URL, and the
        // resource string after the URL.
        //

        PartOneSize = ((ULONG_PTR)UrlOffset - (ULONG_PTR)MessageString) / sizeof(WCHAR);
        UrlSize = wcslen(UrlString);
        PartThreeString = UrlOffset + RTL_NUMBER_OF(L"%1") - 1;
        PartThreeSize = wcslen(PartThreeString);
        DestinationSize = PartOneSize + UrlSize + PartThreeSize + 1;

        Destination = BlMmAllocateHeap(DestinationSize * sizeof(WCHAR));
        if (Destination == NULL) {
            goto DisplayCreateRecoveryUrlStringEnd;
        }

        wcsncpy_s(Destination, DestinationSize, MessageString, PartOneSize);
        wcscat_s(Destination, DestinationSize, UrlString);
        wcscat_s(Destination, DestinationSize, PartThreeString);
    }

DisplayCreateRecoveryUrlStringEnd:

    if (BcdMessageString != NULL) {
        BlMmFreeHeap(BcdMessageString);
    }

    if (BcdUrlString != NULL) {
        BlMmFreeHeap(BcdUrlString);
    }

    return Destination;
}

NTSTATUS
FvepDisplayDefault (
    __out PULONG RecoveryFlags
    )

/*++

Routine Description:

    This routine handles the user interactions for a generic FVE display screen
    that does not require PIN, password, or passphrase entry.

Arguments:

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

Return Value:

    NT status code.

--*/

{

    ULONG CurrentTime;
    PXML_DOC Post;
    BOOLEAN ShutdownDisabled;
    ULONG StartTime;
    NTSTATUS Status;

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

    if (ShutdownDisabled == FALSE) {
        StartTime = BlTimeGetRelativeTime();

    } else {
        StartTime = 0;
    }

    Status = STATUS_SUCCESS;
    do {
        Post = BlXmiRead(FALSE);
        if (Post != NULL) {
            if (OsxmlResultHasAction(Post, L"continue") != FALSE) {
                Status = STATUS_SUCCESS;

            } else if (OsxmlResultHasAction(Post, L"wincontinue") != FALSE) {
                Status = STATUS_SUCCESS;

            } else if (OsxmlResultHasAction(Post, L"reboot") != FALSE) {
                ObjRelease(Post);
                BlFwReboot();
                Status = STATUS_SUCCESS;

            } else if (OsxmlResultHasAction(Post, L"recovery") != FALSE) {
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;

            } else if (OsxmlResultHasAction(Post, L"winrecovery") != FALSE) {
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_WIN_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;

            } else if (OsxmlResultHasAction(Post, L"multi") != FALSE) {
                Status = STATUS_CANCELLED;
            }

            break;
        }

        //
        // Shutdown the machine after waiting 1 minute for input.
        //

        if (ShutdownDisabled == FALSE) {
            CurrentTime = BlTimeGetRelativeTime();
            if ((CurrentTime - StartTime) > FVE_DISPLAY_TIMEOUT) {
                BlFwShutdown();
            }
        }

    } while (TRUE);

    ObjRelease(Post);
    return Status;
}

BOOLEAN
FvepDisplayFlashCursor (
    __in ULONG CursorPosition
    )

/*++

Routine Description:

    This routine flashes the cursor location.  It is required because we may
    be in graphics mode on the display, in which the cursor is not flashed
    automatically by the display.

Arguments:

    Text - Supplies the value of the character at that location on the screen.

    VerticalOffset - Supplies the vertical offset to position the cursor.

    HorizontalOffset - Supplies the horizontal offset to position the cursor.

    BackgroundColor - Supplies the normal background color.

    ForegroundColor - Supplies the normal foreground color.

    Restore - Indicates whether we should restore original state of last
        flashed char.

Return Value:

    None.

--*/

{

    BOOLEAN FlashCursor;
    static ULONG LastCursorPosition = 0;
    static ULONG LastState = 0;
    ULONG NewState;

    NewState = BlTimeGetTickCount();

    //
    // Force the display of the inverse cursor whenever the cursor moves.
    //

    FlashCursor = FALSE;
    if (((NewState - LastState) > FLASH_CURSOR_INTERVAL) ||
        (CursorPosition != LastCursorPosition)) {

        LastState = NewState;
        FlashCursor = TRUE;
    }

    LastCursorPosition = CursorPosition;
    return FlashCursor;
}

BOOL
FvepDisplayFullGroup (
    __in_ecount(KEY_DIGITS) PCHAR KeyChars,
    __in ULONG KeyInputLocation
    )

/*++

Routine Description:

    This routine determines whether a given group of digits is complete or not.
    If any of the digits have not yet been entered then this routine returns
    FALSE, otherwise it returns TRUE.

Arguments:

    KeyChars - Supplies the characters that are currently in the key.

    KeyInputLocation - Supplies the location in the key whose digit
        group should be checked for completeness.

Return Value:

    TRUE if the digit group is complete.  FALSE otherwise.

--*/

{

    ULONG GroupLocation;
    ULONG Index;

    //
    // If not all of the digits in the group have been entered, then return
    // FALSE.  Otherwise return TRUE.
    //

    GroupLocation = KeyInputLocation - (KeyInputLocation % DIGIT_GROUP_SIZE);
    for (Index = 0; Index < DIGIT_GROUP_SIZE; Index += 1) {
        if (KeyChars[GroupLocation + Index] == 0) {
            return FALSE;
        }
    }

    return TRUE;
}

PWCHAR
FvepDisplayGetTitle (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine returns the title resource for the given context.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    A string resource of the screen title.

--*/

{

    ULONG MessageId;

    //
    // Get the message ID based on the screen type.
    //

    switch (DisplayContext->ScreenType) {
    case FveScreenPin:
        MessageId = BM_FVE_TITLE_PIN;
        break;

    case FveScreenPinIncorrect:
        MessageId = BM_FVE_TITLE_PIN_INCORRECT;
        break;

    case FveScreenPinTooManyAttempts:
        MessageId = BM_FVE_TITLE_PIN_TOO_MANY_ATTEMPTS;
        break;

    case FveScreenPassword:
        MessageId = BM_FVE_TITLE_PASSWORD;
        break;

    case FveScreenPasswordIncorrect:
        MessageId = BM_FVE_TITLE_PASSWORD_INCORRECT;
        break;

    case FveScreenPassphrase:
        MessageId = BM_FVE_TITLE_PASSPHRASE;
        break;

    case FveScreenPassphraseIncorrect:
        MessageId = BM_FVE_TITLE_PASSPHRASE_INCORRECT;
        break;

    case FveScreenPassphraseTooManyAttempts:
        MessageId = BM_FVE_TITLE_PASSPHRASE_TOO_MANY_ATTEMPTS;
        break;

    case FveScreenUsbStartupInsert:
        MessageId = BM_FVE_TITLE_STARTUP_USB;
        break;

    case FveScreenUsbStartupInvalid:
        MessageId = BM_FVE_TITLE_STARTUP_USB_INVALID;
        break;

    case FveScreenUsbRecoveryInsert:
        MessageId = BM_FVE_TITLE_RECOVERY_USB;
        break;

    case FveScreenUsbRecoveryInvalid:
        MessageId = BM_FVE_TITLE_RECOVERY_USB_INVALID;
        break;

    case FveScreenAdvancedOptions:
        MessageId = BM_FVE_TITLE_ADVANCED_OPTIONS;
        break;

    default:
        return NULL;
    }

    return BlResourceFindMessage(MessageId);
}

VOID
FvepDisplayInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    )

/*++

Routine Description:

    This routine calls the menu type specific initialization routine,
    determines if input needs to be enabled for the current screen and
    initializes the footer region.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

    Phase - Supplise the phase of the display initialization for the current
        screen.

Return Value:

    None.

--*/

{

    ULONG FooterFlags;

    if (Phase == 0) {
        FveFunctions->Initialize(DisplayContext, Phase);
        if (FvepDisplayRequiresInput(DisplayContext) != FALSE) {
            FveFunctions->InitializeInput();
        }

        //
        // Set the footer, which sets the key listeners, based on the screen
        // type and whether or not this is a multi-boot system. Do not give a
        // multi-boot option if this is recovery during resume from hibernate.
        //

        FooterFlags = FVE_FOOTER_FLAGS_NONE;
        if ((FvepDisplayIsResume() == FALSE) &&
            (BmIsMultiBootSystem(NULL) != FALSE)) {

            FooterFlags |= FVE_FOOTER_FLAGS_F11_ENABLED;
        }

        //
        // Determine if the machine is a slate or not.
        //

        if (BmDisplayIsKeyboardLessSystem() != FALSE) {
            FooterFlags |= FVE_FOOTER_FLAGS_NO_KEYBOARD;
        }

        //
        // Set footer options that are not the default.
        //

        switch (DisplayContext->ScreenType) {
        case FveScreenPassword:
        case FveScreenPasswordIncorrect:
            FooterFlags |= FVE_FOOTER_FLAGS_ESC_MORE_OPTIONS;
            break;

        case FveScreenPinTooManyAttempts:
        case FveScreenPassphraseTooManyAttempts:
        case FveScreenUsbStartupInsert:
        case FveScreenUsbStartupInvalid:
            FooterFlags |= FVE_FOOTER_FLAGS_ENTER_REBOOT;
            break;

        case FveScreenUsbRecoveryInsert:
        case FveScreenUsbRecoveryInvalid:
            FooterFlags |= FVE_FOOTER_FLAGS_ENTER_REBOOT;
            FooterFlags |= FVE_FOOTER_FLAGS_ESC_MORE_OPTIONS;
            break;

        case FveScreenAdvancedOptions:
            FooterFlags |= FVE_FOOTER_FLAGS_SKIP_ESC;
            FooterFlags &= ~FVE_FOOTER_FLAGS_F11_ENABLED;
            break;

        default:
            break;
        }

        if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_F11_ENABLED)) {
            if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_ENTER_REBOOT)) {
                if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
                    BlXmiWrite(L"footer-multi-reboot-winkey='true' ");

                } else {
                    BlXmiWrite(L"footer-multi-reboot='true' ");
                }

            } else {
                if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
                    BlXmiWrite(L"footer-multi-continue-winkey='true' ");

                } else {
                    BlXmiWrite(L"footer-multi-continue='true' ");
                }
            }

        } else {
            if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_ENTER_REBOOT)) {
                if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
                    BlXmiWrite(L"footer-reboot-winkey='true' ");

                } else {
                    BlXmiWrite(L"footer-reboot='true' ");
                }

            } else {
                if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
                    if (CHECK_FLAG(FooterFlags,
                                   FVE_FOOTER_FLAGS_SKIP_ESC)) {

                        BlXmiWrite(L"footer-continue-only-winkey='true' ");

                    } else {
                        BlXmiWrite(L"footer-continue-winkey='true' ");
                    }

                } else {
                    if (CHECK_FLAG(FooterFlags,
                                   FVE_FOOTER_FLAGS_SKIP_ESC)) {

                        BlXmiWrite(L"footer-continue-only='true' ");

                    } else {
                        BlXmiWrite(L"footer-continue='true' ");
                    }
                }
            }
        }

        //
        // Initialize the menu policy specific footer information.
        //

        FveFunctions->InitializeFooter(FooterFlags);

        //
        // Close the opening tag of the template.
        //

        BlXmiWrite(L">");

    } else {

        NT_ASSERT(Phase == 1);

        FveFunctions->Initialize(DisplayContext, Phase);
    }

    return;
}

VOID
FvepDisplayInitializeFunctions (
    VOID
    )

/*++

Routine Description:

    This routine initializes the function table based on the currently booting
    operating system's menu type.

Arguments:

    None.

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

        FveFunctions = &StandardDisplayFunctions;

    } else {
        FveFunctions = &LegacyDisplayFunctions;
    }

    return;
}

VOID
FvepDisplayInitializeRecoveryInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine initializes the FVE recovery error information to be display
    to the screen.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    ULONG RecoveryFlags;
    PWCHAR RecoveryErrorString;

    if ((DisplayContext->ScreenType == FveScreenPassword) ||
        (DisplayContext->ScreenType == FveScreenPasswordIncorrect) ||
        (DisplayContext->ScreenType == FveScreenUsbRecoveryInsert)) {

        //
        // Don't display a reason for recovery if the user elected to use the
        // recovery password.
        //
        // For all the RecoveryFlags values except for NONE, ESC, WIN:
        // Only display one recovery reason. These are in order of importance.
        //

        RecoveryFlags = *(DisplayContext->RecoveryFlags);
        if ((RecoveryFlags == BSD_FVE_ERROR_NONE) ||
            (RecoveryFlags == BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY) ||
            (RecoveryFlags == BSD_FVE_ERROR_USER_REQUESTED_WIN_KEY)) {

            RecoveryErrorString = NULL;

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_DEVICE_LOCKEDOUT) ||
                   CHECK_FLAG(RecoveryFlags,
                              BSD_FVE_ERROR_DEVICE_LOCKOUT_MISMATCH)) {

            RecoveryErrorString = BlResourceFindMessage(
                                      BM_FVE_DEVICE_LOCKEDOUT);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_SRK)) {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_BAD_SRK);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_TPM_DISABLED)) {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_TPM_DISABLED);

        } else if (CHECK_FLAG(RecoveryFlags,
                              BSD_FVE_ERROR_BAD_PARTITION_SIZE)) {

            RecoveryErrorString = BlResourceFindMessage(
                                      BM_FVE_BAD_PARTITION_SIZE);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_TPM_INVALIDATED)) {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_TPM_INVALIDATED);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_DEBUG_ENABLED)) {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_DEBUG_ENABLED);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_CI_DISABLED)) {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_CI_DISABLED);

        } else if (CHECK_FLAG(RecoveryFlags,
                              BSD_FVE_ERROR_SECUREBOOT_DISABLED)) {

            RecoveryErrorString = BlResourceFindMessage(
                                      BM_FVE_SECURE_BOOT_DISABLED);

        } else if (CHECK_FLAG(RecoveryFlags,
                              BSD_FVE_ERROR_SECUREBOOT_CONFIG_CHANGE)) {

            RecoveryErrorString = BlResourceFindMessage(
                                      BM_FVE_SECURE_BOOT_CHANGED);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_PCR_REGISTERS)) {
            RecoveryErrorString = BlResourceFindMessage(
                                      BM_FVE_BAD_PCR_REGISTERS);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_CODE_ID)) {
            RecoveryErrorString = FvepDisplayCreateBadCodeIdString(
                                      DisplayContext);

        } else if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_CODE_OPTION)) {
            RecoveryErrorString = FvepDisplayCreateBadCodeOptionString(
                                      DisplayContext);

        } else {
            RecoveryErrorString = BlResourceFindMessage(BM_FVE_UNKNOWN_ERROR);
        }

        FveFunctions->InitializeRecoveryInformation(RecoveryErrorString);

        //
        // Free the strings in the two cases where they were not pulled from
        // the resource file.
        //

        if (CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_CODE_ID) ||
            CHECK_FLAG(RecoveryFlags, BSD_FVE_ERROR_BAD_CODE_OPTION)) {

            BlMmFreeHeap(RecoveryErrorString);
        }
    }

    return;
}

VOID
FvepDisplayInitializeTitle (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine displays the screen title for the given context.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    PWCHAR Title;

    Title = FvepDisplayGetTitle(DisplayContext);
    FveFunctions->InitializeTitle(Title);
    return;
}

BOOLEAN
FvepDisplayIsPrintableCharacter (
    __in KEY_STROKE KeyStroke
    )

/*++

Routine Description:

    This routine indicates whether the provided character is printable.

Arguments:

    KeyStroke - Supplies a key stroke to test for printability.

Return Value:

    TRUE if the character is printable; FALSE otherwise.

--*/

{

    BOOLEAN IsPrintableCharacter;

    IsPrintableCharacter = FALSE;
    if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) {
        switch (KeyStroke.UnicodeCharacter) {
        case BL_CHAR_BACKSPACE:
        case BL_CHAR_TAB:
        case BL_CHAR_LINEFEED:
        case BL_CHAR_ENTER:
        case BL_CHAR_NULL:
            break;

        default:
            IsPrintableCharacter = TRUE;
            break;
        }
    }

    return IsPrintableCharacter;
}

BOOLEAN
FvepDisplayIsResume (
    VOID
    )

/*++

Routine Description:

    This routine determines if the BitLocker screens are being displayed during
    a resume from hibernate path.

Arguments:

    None.

Return Value:

    TRUE if a resume from hibernate is occurring. FALSE otherwise.

--*/

{

    BOOLEAN AttemptResume;
    BOOLEAN HiberBoot;
    NTSTATUS Status;

    Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_ATTEMPT_RESUME,
                                           &AttemptResume);

    if (!NT_SUCCESS(Status)) {
        AttemptResume = FALSE;
    }

    Status = BlGetApplicationOptionBoolean(BCDE_BOOTMGR_TYPE_HIBERBOOT,
                                           &HiberBoot);

    if (!NT_SUCCESS(Status)) {
        HiberBoot = FALSE;
    }

    if ((AttemptResume != FALSE) && (HiberBoot == FALSE)) {
        return TRUE;
    }

    return FALSE;
}

VOID
FvepDisplayLog (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine logs the reason for displaying the current FVE screen.

Arguments:

    DisplayContext - Supplies a pointer to the current FVE display context.

Return Value:

    None.

--*/

{

    PWCHAR FileName;
    PBSD_EVENT_LOGDATA_BOOTMGR_FVE_RECOVERY RecoveryEvent;
    ULONG RecoveryEventSize;

    //
    // Logging is only necessary for the recovery screen.
    //

    if (DisplayContext->ScreenType != FveScreenPassword) {
        return;
    }

    //
    // Calculate the size of the log event. If there is no filename, log an
    // empty string.
    //

    RecoveryEventSize = FIELD_OFFSET(BSD_EVENT_LOGDATA_BOOTMGR_FVE_RECOVERY,
                                     ApplicationPath);

    FileName = DisplayContext->PasswordContext.FileName;
    if (FileName != NULL) {
        RecoveryEventSize += (ULONG)(wcslen(FileName) * sizeof(WCHAR));
    }

    RecoveryEventSize += sizeof(WCHAR);

    //
    // Allocate the memory, construct the event data and log the event.
    //

    RecoveryEvent = BlMmAllocateHeap(RecoveryEventSize);
    if (RecoveryEvent == NULL) {
        return;
    }

    RtlZeroMemory(RecoveryEvent, RecoveryEventSize);
    RecoveryEvent->RecoveryError = *(DisplayContext->RecoveryFlags);
    if (CHECK_FLAG(*(DisplayContext->RecoveryFlags),
                   BSD_FVE_ERROR_BAD_CODE_OPTION) != FALSE) {

        RecoveryEvent->BadBcdOption = DisplayContext->PasswordContext.BadOption;
    }

    if (FileName != NULL) {
        wcscpy_s(&RecoveryEvent->ApplicationPath[0],
                 wcslen(FileName) + 1,
                 FileName);
    }

    BlBsdLogEntry(BSD_EVENT_LEVEL_INFORMATION,
                  BSD_BOOTMGR_EVENT_FVE_RECOVERY,
                  RecoveryEvent,
                  RecoveryEventSize);

    BlMmFreeHeap(RecoveryEvent);
    return;
}

BOOLEAN
FvepDisplayNeedRecoveryEnvironment (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine will determine if the user should be directed to the windows
    recovery environment for recovery only. Do not display the recovery screen
    on keyboard-less systems unless it was specifically requested by the user
    via the ESC key or debugging is enabled with the exception of ARM where the
    user is always directed to WinRE for recovery. Also for Device lockout
    scenario skip straight to WinRE recovery. If a recovery sequence is not
    present, continue and show the screen.

Arguments:

    DisplayContext - Supplies a pointer to an FVE display context that
        describes the content that needs to be shown.

Return Value:

    BOOLEAN.

--*/

{

    BOOLEAN DeviceLockedOut;
    BOOLEAN DeviceLockoutMismatch;
    BOOLEAN DebuggerEnabled;
    BOOLEAN EscapeKeyHit;
    BOOLEAN IsArm;
    BOOLEAN PasswordScreen;

#if defined(_ARM_) || defined(_ARM64_)
    IsArm = TRUE;
#else
    IsArm = FALSE;
#endif

    if (DisplayContext->RecoveryFlags == NULL) {
        return FALSE;
    }

    PasswordScreen = (DisplayContext->ScreenType == FveScreenPassword);
    DeviceLockedOut = ((CHECK_FLAG(*DisplayContext->RecoveryFlags,
                                   BSD_FVE_ERROR_DEVICE_LOCKEDOUT)) != 0);

    DeviceLockoutMismatch =
        ((CHECK_FLAG(*DisplayContext->RecoveryFlags,
                     BSD_FVE_ERROR_DEVICE_LOCKOUT_MISMATCH)) != 0);

    EscapeKeyHit = ((CHECK_FLAG(*DisplayContext->RecoveryFlags,
                                BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY)) != 0);

    DebuggerEnabled = ((CHECK_FLAG(*DisplayContext->RecoveryFlags,
                                   BSD_FVE_ERROR_DEBUG_ENABLED)) != 0);

    if (((PasswordScreen != FALSE) && (IsArm != FALSE)) ||
        ((PasswordScreen != FALSE) &&
         (EscapeKeyHit == FALSE) &&
         (DebuggerEnabled == FALSE) &&
         (BmDisplayIsKeyboardLessSystem() != FALSE)) ||
        (DeviceLockedOut != FALSE) ||
        (DeviceLockoutMismatch != FALSE)) {

        return TRUE;
    }

    return FALSE;
}

VOID
FvepDisplayNumberForFunctionChar (
    __inout PKEY_STROKE KeyStroke
    )

/*++

Routine Description:

    This routine returns the number (1, 2, ...) for the given function key
    character (F1, F2, ...). First drop the low byte of the scan code which is
    zero. Then subtract the upper byte of the F1_KEY scan code from the shifted
    scan code. Now the Input value ranges from 0 for F1 to 9 for F10. Convert
    that to 1 for F1 to 9 for F9, and 0 for F10 by adding 1 modulo 10.

Arguments:

    KeyStroke - Supplies a key stroke with a function key to convert to a
        number.

Return Value:

    The number for the function key character.

--*/

{

    WCHAR FunctionCharacter;

    ASSERT(CHECK_FLAG(KeyStroke->Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0);
    ASSERT((KeyStroke->ScanCode >= BL_SCAN_F1) &&
           (KeyStroke->ScanCode <= BL_SCAN_F10));

    //
    // Convert the key stroke from a scan code to a unicode character.
    //

    FunctionCharacter = (WCHAR)KeyStroke->ScanCode;
    FunctionCharacter /= 256;
    FunctionCharacter -= (BL_SCAN_F1 / 256);
    FunctionCharacter = (FunctionCharacter + 1) % 10;
    KeyStroke->UnicodeCharacter = FunctionCharacter;
    KeyStroke->Flags = BL_KEY_STROKE_FLAGS_CHARACTER;
    return;
}

NTSTATUS
FvepDisplayPassphrase (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    )

/*++

Routine Description:

    This routine obtains the full volume encryption passphrase from the console
    keyboard input or BCD override.

Arguments:

    PassphraseContext - Supplies a pointer to the FVE passphrase context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

    HideShowHandle - Supplies a handle to a text region for the hide/show
        passphrase help information.

Return Value:

    STATUS_FVE_LOCKED_VOLUME if the user aborts the passphrase entry.
    STATUS_CANCELLED if the user elects to boot another OS.
    STATUS_SUCCESS on success.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    NTSTATUS Status;

    Status = FvepDisplayPassphraseOverride(PassphraseContext);
    if (NT_SUCCESS(Status)) {
        goto DisplayPassphraseEnd;
    }

    Status = FvepDisplayPassphraseConsole(PassphraseContext,
                                          RecoveryFlags,
                                          HideShowHandle);

DisplayPassphraseEnd:
    return Status;
}

NTSTATUS
FvepDisplayPassphraseConsole (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    )

/*++

Routine Description:

    This routine obtains the full volume encryption passphrase from the console
    keyboard input. Displays instructions for entering the phassphrase.

Arguments:

    PassphraseContext - Supplies a pointer to the FVE passphrase context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

    HideShowHandle - Supplies a handle to a text region for the hide/show
        passphrase help information.

Return Value:

    STATUS_FVE_LOCKED_VOLUME if the user aborts the passphrase entry.
    STATUS_CANCELLED if the user elects to boot another OS.
    STATUS_SUCCESS on success.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    ULONG CurrentTime;
    ULONG CursorFlags;
    KEY_STROKE KeyStroke;
    BOOLEAN MultiBootSystem;
    BOOLEAN NoKeyboard;
    PWSTR Passphrase;
    USHORT PassphraseCursor;
    PULONG PassphraseFlags;
    ULONG ShowHideMessageId;
    BOOLEAN ShutdownDisabled;
    ULONG StartTime;
    NTSTATUS Status;

    Passphrase = PassphraseContext->Passphrase;
    PassphraseFlags = PassphraseContext->PassphraseFlags;

    //
    // Determine if this is a multi-boot system.
    //

    MultiBootSystem = BmIsMultiBootSystem(NULL);

    //
    // Determine if this system has a keyboard.
    //

    NoKeyboard = BmDisplayIsKeyboardLessSystem();

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
    // Set input location at the end of the passphrase.
    //

    for (PassphraseCursor = 0;
         Passphrase[PassphraseCursor] != L'\0';
         PassphraseCursor += 1);

    //
    // Set the console colors.
    //

    KeyStroke.KeyStroke = 0;
    BlDisplaySetForegroundColor(LIGHT_GRAY);
    BlDisplaySetBackgroundColor(BLACK);
    BlDisplaySetCursorType(INVISIBLE);
    CursorFlags = BG_DISPLAY_STRING_FLAGS_NONE;
    Status = STATUS_SUCCESS;

    if (ShutdownDisabled == FALSE) {
        StartTime = BlTimeGetRelativeTime();

    } else {
        StartTime = 0;
    }

    while (TRUE) {

        //
        // Shutdown the machine after waiting 1 minute for input.
        //

        if (ShutdownDisabled == FALSE) {
            CurrentTime = BlTimeGetRelativeTime();
            if ((CurrentTime - StartTime) > FVE_DISPLAY_TIMEOUT) {
                BlFwShutdown();
            }
        }

        if (FvepDisplayFlashCursor(PassphraseCursor) != FALSE) {
            CursorFlags ^= BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
            FvepDisplayUpdatePassphrase(Passphrase,
                                        *PassphraseFlags,
                                        PassphraseCursor,
                                        CursorFlags);
        }

        Status = BlDisplayGetKeyStroke(FALSE, &KeyStroke);
        if (!NT_SUCCESS(Status) ||
            (KeyStroke.Flags == BL_KEY_STROKE_FLAGS_NONE)) {

            continue;
        }

        //
        // User entered a key. Reset the start time.
        //

        if (ShutdownDisabled == FALSE) {
            StartTime = BlTimeGetRelativeTime();

        } else {
            StartTime = 0;
        }

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) {
            if (KeyStroke.UnicodeCharacter == BL_CHAR_ENTER) {
                break;
            }

            if (KeyStroke.UnicodeCharacter == BL_CHAR_ESCAPE) {
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPassphraseConsoleEnd;
            }
        }

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) {
            if ((NoKeyboard != FALSE) &&
                (KeyStroke.ScanCode == BL_SCAN_WINDOWS_KEY)) {

                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_WIN_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPassphraseConsoleEnd;
            }

            if ((KeyStroke.ScanCode == BL_SCAN_F11) &&
                (MultiBootSystem != FALSE)) {

                Status = STATUS_CANCELLED;
                goto DisplayPassphraseConsoleEnd;
            }

            if (KeyStroke.ScanCode == BL_SCAN_HOME) {
                PassphraseCursor = 0;
                RtlSecureZeroMemory(Passphrase,
                                    ((FVEB_MAX_PASSPHRASE_LENGTH + 1) *
                                     sizeof(WCHAR)));

                FvepDisplayUpdatePassphrase(Passphrase,
                                            *PassphraseFlags,
                                            PassphraseCursor,
                                            CursorFlags);

                continue;
            }

            if (KeyStroke.ScanCode == BL_SCAN_INSERT) {
                if (CHECK_FLAG(*PassphraseFlags,
                               FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

                    ShowHideMessageId = BM_FVE_HELP_SHOW_PASSPHRASE;
                    CLEAR_FLAGS(*PassphraseFlags,
                                FVE_CONTEXT_FLAGS_SHOW_TYPING);

                } else {
                    ShowHideMessageId = BM_FVE_HELP_HIDE_PASSPHRASE;
                    SET_FLAGS(*PassphraseFlags, FVE_CONTEXT_FLAGS_SHOW_TYPING);
                }

                FveFunctions->UpdateHelpInformation(ShowHideMessageId,
                                                    HideShowHandle);

                FvepDisplayUpdatePassphrase(Passphrase,
                                            *PassphraseFlags,
                                            PassphraseCursor,
                                            CursorFlags);

                continue;
            }

            if ((KeyStroke.ScanCode >= BL_SCAN_F1) &&
                (KeyStroke.ScanCode <= BL_SCAN_F10)) {

                //
                // The user pressed one of the function keys F1-F10.  The
                // function keys F1-F9 are used to enter the digits 1-9
                // respectively. F10 is used to enter the digit zero.
                //

                FvepDisplayNumberForFunctionChar(&KeyStroke);
                KeyStroke.UnicodeCharacter += L'0';
            }
        }

        if (((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
             (KeyStroke.UnicodeCharacter == BL_CHAR_BACKSPACE)) ||
            ((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
             (KeyStroke.ScanCode == BL_SCAN_LEFT))) {

            if (PassphraseCursor > 0) {
                PassphraseCursor -= 1;
                Passphrase[PassphraseCursor] = L'\0';
            }

            FvepDisplayUpdatePassphrase(Passphrase,
                                        *PassphraseFlags,
                                        PassphraseCursor,
                                        CursorFlags);

            continue;
        }

        if (FvepDisplayIsPrintableCharacter(KeyStroke) == FALSE) {
            continue;
        }

        ASSERT(CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0);

        if (PassphraseCursor < FVEB_MAX_PASSPHRASE_LENGTH) {
            Passphrase[PassphraseCursor] = KeyStroke.UnicodeCharacter;
            PassphraseCursor++;
            Passphrase[PassphraseCursor] = L'\0';
            FvepDisplayUpdatePassphrase(Passphrase,
                                        *PassphraseFlags,
                                        PassphraseCursor,
                                        CursorFlags);
        }
    }

    //
    // Blank the title to indicate that action is being taken.
    //

    FveFunctions->UpdateTitle(BM_FVE_EMPTY_STRING);

DisplayPassphraseConsoleEnd:

    //
    // Remove the cursor if necessary.
    //

    if (CHECK_FLAG(CursorFlags, BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR) != FALSE) {
        CursorFlags &= ~BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
        FvepDisplayUpdatePassphrase(Passphrase,
                                    *PassphraseFlags,
                                    PassphraseCursor,
                                    CursorFlags);
    }

    //
    // Wipe the input buffer.
    //

    BlDisplayWipeInputBuffer(NULL);
    BlDisplaySetCursorType(INVISIBLE);
    return Status;
}

NTSTATUS
FvepDisplayPassphraseOverride (
    __inout PFVE_PASSPHRASE_CONTEXT PassphraseContext
    )

/*++

Routine Description:

    This routine obtains the full volume encryption Passphrase from
    the bcd override. It then purges the override.

Arguments:

    PassphraseContext - Supplies a pointer to the FVE passphrase context.

Return Value:

    STATUS_SUCCESS if a key was completely onbtained, validated and the BCD
        setting was purged.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    HANDLE DataStoreHandle;
    UINTN KeyCharsLength;
    PWCHAR KeyCharsTemp;
    NTSTATUS Status;

    DataStoreHandle = NULL;
    KeyCharsLength = 0;
    KeyCharsTemp = NULL;

    //
    // See if a bcd based test override has been defined for obtaining the
    // key. If so, then obtain the key from the value from the bcd
    // entry.
    //

    Status = BlGetApplicationOptionString(
                 BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE,
                 &KeyCharsTemp);

    if (!NT_SUCCESS(Status)) {
        goto DisplayPassphraseOverrideEnd;
    }

    //
    // Always try to delete the BCD entry after it is read, both from the in-
    // memory list and from the persistent store. If it is valid, it should
    // only be used once. If it is invalid, it should not be used and the
    // invalid value should be deleted.
    //

    BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE);

    Status = BmOpenDataStore(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPassphraseOverrideEnd;
    }

    Status = BmPurgeOption(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPassphraseOverrideEnd;
    }

    //
    // Validate and use the override passphrase.
    //

    KeyCharsLength = wcslen(KeyCharsTemp);
    if (KeyCharsLength > FVEB_MAX_PASSPHRASE_LENGTH) {
        Status = STATUS_CANCELLED;
        goto DisplayPassphraseOverrideEnd;
    }

    wcsncpy_s(PassphraseContext->Passphrase,
              FVEB_MAX_PASSPHRASE_LENGTH + 1,
              KeyCharsTemp,
              FVEB_MAX_PASSPHRASE_LENGTH);

DisplayPassphraseOverrideEnd:
    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
        DataStoreHandle = NULL;
    }

    if(KeyCharsTemp != NULL) {
        BlMmFreeHeap(KeyCharsTemp);
        KeyCharsTemp = NULL;
    }

    return Status;
}

NTSTATUS
FvepDisplayPassword (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext,
    __out PULONG RecoveryFlags
    )

/*++

Routine Description:

    This routine obtains the full volume encryption recovery key from the
    console keyboard input or bcd override.

Arguments:

    PasswordContext - Supplies a pointer to the FVE password context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

Return Value:

    STATUS_INVALID_PARAMETER if the size of the key passed is not supported.
    STATUS_FVE_LOCKED_VOLUME if the user aborted password entry.
    STATUS_CANCELLED if the user chooses to boot from another application.
    STATUS_SUCCESS if a key was completely entered and validated.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{
    ULONG GroupIndex;
    ULONG GroupLocation;
    ULONG GroupValue;
    PUSHORT Key;
    ULONG KeyByteCount;
    PCHAR KeyChars;
    NTSTATUS Status;

    //
    // Initialize local variables used for convenience below.
    //

    Key = PasswordContext->Password;
    KeyChars = PasswordContext->KeyCharacters;
    KeyByteCount = PasswordContext->PasswordByteCount;

    //
    // Validate password context.
    //

    if (KeyByteCount != (DIGIT_GROUP_COUNT * sizeof(USHORT))) {
        Status = STATUS_INVALID_PARAMETER;
        goto DisplayPasswordEnd;
    }

    //
    // First check for the password override. If there's an error getting the
    // override, continue on to prompt the user.
    //

    Status = FvepDisplayPasswordOverride(PasswordContext);
    if (!NT_SUCCESS(Status)) {
        Status = FvepDisplayPasswordConsole(PasswordContext, RecoveryFlags);
        if (!NT_SUCCESS(Status)) {
            goto DisplayPasswordEnd;
        }
    }


    //
    // Convert the recovery password string into the actual key.
    //

    Status = STATUS_SUCCESS;
    for (GroupIndex = 0; GroupIndex < DIGIT_GROUP_COUNT; GroupIndex += 1) {
        GroupLocation = GroupIndex * DIGIT_GROUP_SIZE;
        if ((FvepDisplayFullGroup(KeyChars, GroupLocation) != FALSE) &&
            (FvepScanFullGroup(&KeyChars[GroupLocation], &GroupValue) != FALSE)) {

            GroupValue /= CHECKSUM;

        } else {
            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        Key[GroupIndex] = (USHORT)GroupValue;
    }

DisplayPasswordEnd:
    return Status;
}

NTSTATUS
FvepDisplayPasswordConsole (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext,
    __out PULONG RecoveryFlags
    )

/*++

Routine Description:

    This routine obtains the full volume encryption recovery key from the
    console keyboard input. It displays instructions for entering the key, as
    well as status regarding the correctness of the key as currently entered.

Arguments:

    PasswordContext - Supplies a pointer to the FVE password context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

Return Value:

    STATUS_INVALID_PARAMETER if the size of the key passed is not supported.
    STATUS_FVE_LOCKED_VOLUME if the user aborted password entry.
    STATUS_CANCELLED if the user chooses to boot from another application.
    STATUS_SUCCESS if a key was completely entered and validated.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    ULONG CurrentTime;
    ULONG CursorFlags;
    LONG InputIncrement;
    PCHAR KeyChars;
    LONG KeyInputLocation;
    KEY_STROKE KeyStroke;
    LONG KeyValuesEntered;
    BOOLEAN MultiBootSystem;
    BOOLEAN NoKeyboard;
    BOOLEAN ShutdownDisabled;
    ULONG StartTime;
    NTSTATUS Status;

    KeyChars = PasswordContext->KeyCharacters;

    //
    // Determine if this is a multi-boot system.
    //

    MultiBootSystem = BmIsMultiBootSystem(NULL);

    //
    // Determine if this system has a keyboard.
    //

    NoKeyboard = BmDisplayIsKeyboardLessSystem();

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
    // Set the foreground and background colors to desired values for our UI.
    //

    BlDisplaySetForegroundColor(LIGHT_GRAY);
    BlDisplaySetBackgroundColor(BLACK);

    //
    // Set input location at the end of the recovery key and store the number
    // of keys that have been entered.
    //

    KeyValuesEntered = 0;
    for (KeyInputLocation = 0;
         KeyChars[KeyInputLocation] != L'\0';
         KeyInputLocation += 1) {

        KeyValuesEntered += 1;
    }

    //
    // Read the password from the console.  Validate it as it is entered.
    // Indicate any input errors to the user.
    //

    KeyStroke.KeyStroke = 0;
    BlDisplaySetCursorType(INVISIBLE);
    CursorFlags = BG_DISPLAY_STRING_FLAGS_NONE;;

    if (ShutdownDisabled == FALSE) {
        StartTime = BlTimeGetRelativeTime();

    } else {
        StartTime = 0;
    }

    while (TRUE) {

        //
        // Shutdown the machine after waiting 1 minute for input.
        //

        if (ShutdownDisabled == FALSE) {
            CurrentTime = BlTimeGetRelativeTime();
            if ((CurrentTime - StartTime) > FVE_DISPLAY_TIMEOUT) {
                BlFwShutdown();
            }
        }

        //
        // Limit KeyInputLocation to known valid locations inside the currently
        // entered key.  This limits the cursor movement to the appropriate
        // locations within the entered key.  It also ensures that the
        // KeyInputLocation passed to other routines, and used as an index
        // into the KeyChars array, never falls outside the KeyChars array.
        //

        if (KeyInputLocation < 0) {
            KeyInputLocation = 0;
        }

        if (KeyInputLocation > KeyValuesEntered) {
            KeyInputLocation = KeyValuesEntered;
        }

        //
        // Decide whether or not to flash the cursor. Update the string if the
        // cursor needs flashing.
        //

        if (FvepDisplayFlashCursor(KeyInputLocation) != FALSE) {
            CursorFlags ^= BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
            FvepDisplayUpdatePassword(KeyChars, KeyInputLocation, CursorFlags);
        }

        Status = BlDisplayGetKeyStroke(FALSE, &KeyStroke);
        if (!NT_SUCCESS(Status) ||
            (KeyStroke.Flags == BL_KEY_STROKE_FLAGS_NONE)) {

            continue;
        }

        //
        // User entered a key. Reset the start time.
        //

        if (ShutdownDisabled == FALSE) {
            StartTime = BlTimeGetRelativeTime();

        } else {
            StartTime = 0;
        }

        InputIncrement = 1;

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) {
            if (KeyStroke.UnicodeCharacter == BL_CHAR_ENTER) {
                break;
            }

            if (KeyStroke.UnicodeCharacter == BL_CHAR_ESCAPE) {
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPasswordConsoleEnd;
            }
        }

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) {
            if ((MultiBootSystem != FALSE) &&
                (KeyStroke.ScanCode == BL_SCAN_F11)) {

                Status = STATUS_CANCELLED;
                goto DisplayPasswordConsoleEnd;
            }

            //
            // Check the windows key for appropriate action.
            //

            if ((NoKeyboard != FALSE) &&
                (KeyStroke.ScanCode == BL_SCAN_WINDOWS_KEY)) {

                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_WIN_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPasswordConsoleEnd;
            }
        }

        //
        // All of the keys that move the cursor, will cause groups of digits to
        // be validated when they would move the cursor out of that group.  If
        // the digit group is not valid, then instead of leaving the group, the
        // digit group is highlighed from within the FvepDisplayValidGroup
        // routine, and the cursor is left inside the highlighted group.  This
        // means that the movement keys will only move the cursor around inside
        // that group until the group of digits is valid.  At which point the
        // highlighting will be removed from the group, and the cursor will
        // move to the appropriate location outside the group depending on the
        // key pressed.
        //

        //
        // The BACKSPACE key erases the last digit entered if the current
        // entry location is at the last digit.  Otherwise it behaves like the
        // LEFT arrow key.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
            (KeyStroke.UnicodeCharacter == BL_CHAR_BACKSPACE)) {

            if ((KeyValuesEntered > 0) &&
                (KeyInputLocation == KeyValuesEntered)) {

                if (FvepDisplayFullGroup(KeyChars, KeyInputLocation) != FALSE) {
                    FveFunctions->UpdateTitle(BM_FVE_TITLE_PASSWORD);
                }

                if (KeyChars[KeyInputLocation] != '\0') {
                    KeyChars[KeyInputLocation] = '\0';

                } else {
                    KeyInputLocation -= 1;
                    KeyChars[KeyInputLocation] = '\0';
                    KeyValuesEntered -= 1;
                }

                FvepDisplayUpdatePassword(KeyChars,
                                          KeyInputLocation,
                                          CursorFlags);

                continue;
            }
        }

        //
        // The HOME key moves the cursor to the beginning of all of the entered
        // digits if the current group is valid or not completely entered,
        // otherwise it moves the cursor to the first digit in the current
        // invalid group.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_HOME)) {

            KeyInputLocation -= (KeyInputLocation % DIGIT_GROUP_SIZE);
            if (FvepDisplayValidGroup(KeyChars, KeyInputLocation) != FALSE) {
                if ((KeyValuesEntered - KeyInputLocation) ==
                    (DIGIT_GROUP_SIZE - 1)) {

                    KeyValuesEntered += 1;
                }

            } else {
                if (FvepDisplayFullGroup(KeyChars, KeyInputLocation) != FALSE) {
                    continue;
                }
            }

            KeyInputLocation = 0;
            continue;
        }

        //
        // The END key moves the cursor to the end of all of the entered
        // digits if the current group is valid or not completely entered,
        // otherwise it moves the cursor to the last digit in the current
        // invalid group.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_END)) {

            KeyInputLocation -= (KeyInputLocation % DIGIT_GROUP_SIZE);
            if (FvepDisplayValidGroup(KeyChars, KeyInputLocation) != FALSE) {
                if ((KeyValuesEntered - KeyInputLocation) ==
                    (DIGIT_GROUP_SIZE - 1)) {

                    KeyValuesEntered += 1;
                }

            } else {
                if (FvepDisplayFullGroup(KeyChars, KeyInputLocation) != FALSE) {
                    KeyInputLocation += DIGIT_GROUP_SIZE - 1;
                    continue;
                }
            }

            KeyInputLocation = KeyValuesEntered;
            continue;
        }

        //
        // The TAB key moves the cursor to the start of the next group of
        // digits if the current group is valid, otherwise it moves the cursor
        // to the last digit in the current invalid group.  If the group is
        // not completely entered, the cursor will move to the end of the
        // entered digits in the group.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
            (KeyStroke.UnicodeCharacter == BL_CHAR_TAB)) {

            KeyInputLocation -= (KeyInputLocation % DIGIT_GROUP_SIZE);
            if (FvepDisplayValidGroup(KeyChars, KeyInputLocation) != FALSE) {
                if ((KeyValuesEntered - KeyInputLocation) ==
                    (DIGIT_GROUP_SIZE - 1)) {

                    KeyValuesEntered += 1;
                }

            } else {
                if (FvepDisplayFullGroup(KeyChars, KeyInputLocation) != FALSE) {
                    KeyInputLocation += DIGIT_GROUP_SIZE - 1;
                    continue;
                }
            }

            KeyInputLocation += DIGIT_GROUP_SIZE;
            continue;
        }

        //
        // The BACK_TAB key moves the cursor to the start of the previous
        // group of digits if the current group is valid or not completely
        // entered, otherwise it moves the cursor to the first digit in the
        // current invalid group.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_BACK_TAB)) {

            KeyInputLocation -= (KeyInputLocation % DIGIT_GROUP_SIZE);
            if (FvepDisplayValidGroup(KeyChars, KeyInputLocation) != FALSE) {
                if ((KeyValuesEntered - KeyInputLocation) ==
                    (DIGIT_GROUP_SIZE - 1)) {

                    KeyValuesEntered += 1;
                }

            } else {
                if (FvepDisplayFullGroup(KeyChars, KeyInputLocation) != FALSE) {
                    continue;
                }
            }

            KeyInputLocation -= DIGIT_GROUP_SIZE;
            continue;
        }

        //
        // The LEFT arrow key moves the cursor to the digit just before the
        // current one.  This will cause the group to be validated if the
        // current location the first character in the group.  If the group is
        // valid or incomplete, then the cursor will move to the last character
        // in the previous group, otherwise, it will remain pointing at the
        // first character in the current invalid group.
        //

        if (((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
             (KeyStroke.ScanCode == BL_SCAN_LEFT)) ||
            ((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
             (KeyStroke.UnicodeCharacter == BL_CHAR_BACKSPACE))) {

            if ((KeyInputLocation % DIGIT_GROUP_SIZE) == 0) {
                if (FvepDisplayValidGroup(KeyChars,
                                          KeyInputLocation) != FALSE) {

                    if ((KeyValuesEntered - KeyInputLocation) ==
                        (DIGIT_GROUP_SIZE - 1)) {

                        KeyValuesEntered += 1;
                    }

                } else {
                    if (FvepDisplayFullGroup(KeyChars,
                                             KeyInputLocation) != FALSE) {

                        continue;
                    }
                }
            }

            KeyInputLocation -= 1;
            continue;
        }

        //
        // The RIGHT arrow key moves the cursor to the digit just after the
        // current one. This will cause the group to be validated if the
        // current location is the last character in the group. If the group
        // is valid, then the cursor will move to the first character in the
        // next group, otherwise it will remain pointing at the last character
        // in the current invalid group.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_RIGHT)) {

            if (((KeyInputLocation + 1) % DIGIT_GROUP_SIZE) == 0) {
                if (FvepDisplayValidGroup(KeyChars,
                                          KeyInputLocation) != FALSE) {

                    if (KeyInputLocation == KeyValuesEntered) {
                        KeyValuesEntered += 1;
                    }

                } else {
                    continue;
                }
            }

            KeyInputLocation += 1;
            continue;
        }

        //
        // If the key input location is beyond the end of the array, just
        // continue, ignoring input.
        //

        if (KeyInputLocation > (KEY_DIGITS - 1)) {
            continue;
        }

        //
        // The DOWN_ARROW key changes the value of an already entered digit
        // down by one if it is greater than zero.  It has no effect on
        // locations that have not had a digit entered.  This is a backup
        // mechanism to change recovery password entries in case a keyboard
        // has one or more broken keys.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_DOWN)) {

            if (KeyChars[KeyInputLocation] > '0' &&
                KeyChars[KeyInputLocation] <= '9') {

                KeyStroke.UnicodeCharacter = KeyChars[KeyInputLocation] - 1;
                KeyStroke.Flags = BL_KEY_STROKE_FLAGS_CHARACTER;
                InputIncrement = 0;
            }
        }

        //
        // The UP_ARROW key changes the value of an already entered digit up
        // by one if it is less than nine.  It has no effect on locations that
        // have not had a digit entered.  This is a backup mechanism to change
        // recovery password entries in case a keyboard has one or more broken
        // keys.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
            (KeyStroke.ScanCode == BL_SCAN_UP)) {

            if (KeyChars[KeyInputLocation] >= '0' &&
                KeyChars[KeyInputLocation] < '9') {

                KeyStroke.UnicodeCharacter = KeyChars[KeyInputLocation] + 1;
                KeyStroke.Flags = BL_KEY_STROKE_FLAGS_CHARACTER;
                InputIncrement = 0;
            }
        }

        //
        // Ignore any input except for keys with known scan codes.  The only
        // other input we accept in this routine are the function keys F1 - F10
        // or the number keys 0 - 9.
        //

        if ((CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
            (KeyStroke.UnicodeCharacter >= '0') &&
            (KeyStroke.UnicodeCharacter <= '9')) {

            //
            // The user pressed one of the number keys 0 - 9.  Convert the
            // scan codes for those keys into the corresponding binary values
            // for the numbers 0 - 9.
            //

            KeyStroke.UnicodeCharacter -= '0';

        } else if ((CHECK_FLAG(KeyStroke.Flags,
                               BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
                   (KeyStroke.ScanCode >= BL_SCAN_F1) &&
                   (KeyStroke.ScanCode <= BL_SCAN_F10)) {

            //
            // The user pressed one of the function keys F1-F10.  The function
            // keys F1-F9 are used to enter the digits 1-9 respectively.  F10
            // is used to enter the digit zero.
            //

            FvepDisplayNumberForFunctionChar(&KeyStroke);

        } else {

            //
            // Ignore all input from unknown scan codes.
            //

            continue;
        }

        ASSERT(CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0);

        //
        // Display and store the key digit.
        //

        KeyChars[KeyInputLocation] = (CHAR)KeyStroke.UnicodeCharacter + '0';

        //
        // If a complete group of digits has been entered then validate the
        // group.
        //

        if ((KeyInputLocation + 1) % DIGIT_GROUP_SIZE == 0) {
            if (FvepDisplayValidGroup(KeyChars, KeyInputLocation) != FALSE) {
                KeyInputLocation += InputIncrement;
            }

        } else {
            KeyInputLocation += InputIncrement;
        }

        if (KeyInputLocation > KeyValuesEntered) {
            KeyValuesEntered = KeyInputLocation;
        }

        FvepDisplayUpdatePassword(KeyChars, KeyInputLocation, CursorFlags);
    }

    //
    // Blank the title to indicate that validation is taking place.
    //

    FveFunctions->UpdateTitle(BM_FVE_EMPTY_STRING);

DisplayPasswordConsoleEnd:

    //
    // Remove the cursor if necessary.
    //

    if (CHECK_FLAG(CursorFlags, BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR) != FALSE) {
        CursorFlags &= ~BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
        FvepDisplayUpdatePassword(KeyChars, KeyInputLocation, CursorFlags);
    }

    //
    // Wipe the input buffer.
    //

    BlDisplaySetCursorType(INVISIBLE);
    BlDisplayWipeInputBuffer(NULL);
    return Status;
}

NTSTATUS
FvepDisplayPasswordOverride (
    __inout PFVE_PASSWORD_CONTEXT PasswordContext
    )

/*++

Routine Description:

    This routine obtains the full volume encryption recovery key from
    the bcd override. It then purges the override.

Arguments:

    PasswordContext - Supplies a pointer to the FVE password context.

Return Value:

    STATUS_SUCCESS if a key was completely onbtained, validated and the BCD
        setting was purged.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{
    HANDLE DataStoreHandle;
    UINTN KeyCharsLength;
    PWCHAR KeyCharsTemp;
    NTSTATUS Status;

    DataStoreHandle = NULL;
    KeyCharsLength = 0;
    KeyCharsTemp = NULL;

    //
    // See if a bcd based test override has been defined for obtaining the
    // key. If so, then obtain the key from the value from the bcd
    // entry.
    //

    Status = BlGetApplicationOptionString(BCDE_BOOTMGR_TYPE_PASSWORD_OVERRIDE,
                                          &KeyCharsTemp);

    if (!NT_SUCCESS(Status)) {
        goto DisplayPasswordOverrideEnd;
    }

    //
    // Always try to delete the BCD entry after it is read, both from the in-
    // memory list and from the persistent store. If it is valid, it should
    // only be used once. If it is invalid, it should not be used and the
    // invalid value should be deleted.
    //

    BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_PASSWORD_OVERRIDE);

    Status = BmOpenDataStore(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPasswordOverrideEnd;
    }

    Status = BmPurgeOption(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           BCDE_BOOTMGR_TYPE_PASSWORD_OVERRIDE);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPasswordOverrideEnd;
    }

    //
    // Validate and use the recovery password override.
    //

    KeyCharsLength = wcslen(KeyCharsTemp);
    if (KeyCharsLength != KEY_DIGITS) {
        Status = STATUS_CANCELLED;
        goto DisplayPasswordOverrideEnd;
    }

    BlCopyWcharStringToString(PasswordContext->KeyCharacters, KeyCharsTemp);

DisplayPasswordOverrideEnd:
    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
        DataStoreHandle = NULL;
    }

    if (KeyCharsTemp != NULL) {
        BlMmFreeHeap(KeyCharsTemp);
        KeyCharsTemp = NULL;
    }

    return Status;
}

NTSTATUS
FvepDisplayPin (
    __inout PFVE_PIN_CONTEXT PinContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    )

/*++

Routine Description:

    This routine obtains the full volume encryption PIN from the console
    keyboard input or BCD override.

Arguments:

    PinContext - Supplies a pointer to the FVE pin context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

    HideShowHandle - Supplies a handle to the hide/show pin help region.

Return Value:

    STATUS_INVALID_PARAMETER if the size of the PIN passed is not supported.
    STATUS_SUCCESS if a valid PIN was entered.
    STATUS_CANCELLED if the user aborted the PIN entry.

--*/

{

    NTSTATUS Status;

    Status = FvepDisplayPinOverride(PinContext);
    if(NT_SUCCESS(Status)) {
        goto DisplayPinEnd;
    }

    Status = FvepDisplayPinConsole(PinContext, RecoveryFlags, HideShowHandle);

DisplayPinEnd:
    return Status;
}

NTSTATUS
FvepDisplayPinConsole (
    __inout PFVE_PIN_CONTEXT PinContext,
    __out PULONG RecoveryFlags,
    __in HANDLE HideShowHandle
    )

/*++

Routine Description:

    This routine obtains the full volume encryption PIN from the console
    keyboard input.  It displays instructions for entering the PIN.

Arguments:

    PinContext - Supplies a pointer to the FVE pin context.

    RecoveryFlags - Supplies a pointer to a variable that receives updates
        based on user input.

    HideShowHandle - Supplies a handle to the hide/show pin help region.

Return Value:

    STATUS_INVALID_PARAMETER if the size of the PIN passed is not supported.
    STATUS_SUCCESS if a valid PIN was entered.
    STATUS_CANCELLED if the user aborted the PIN entry.

--*/

{

    ULONG CurrentTime;
    ULONG CursorFlags;
    ULONG InputLocation;
    KEY_STROKE KeyStroke;
    BOOLEAN MultiBootSystem;
    BOOLEAN NoKeyboard;
    PWCHAR Pin;
    ULONG PinByteCount;
    PULONG PinFlags;
    ULONG ShowHideMessageId;
    BOOLEAN ShutdownDisabled;
    ULONG StartTime;
    NTSTATUS Status;
    BOOLEAN UseEnhancedPin;

    Pin = PinContext->Pin;
    PinByteCount = PinContext->PinByteCount;
    UseEnhancedPin = PinContext->UseEnhancedPin;
    PinFlags = PinContext->PinFlags;

    //
    // Validate parameters.
    //

    if (PinByteCount != (FVEB_MAX_PIN_DIGITS + 1) * sizeof(WCHAR)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Determine if this is a multi-boot system.
    //

    MultiBootSystem = BmIsMultiBootSystem(NULL);

    //
    // Determine if this system has a keyboard.
    //

    NoKeyboard = BmDisplayIsKeyboardLessSystem();

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
    // Set the foreground and background colors to desired values for our UI.
    //

    BlDisplaySetForegroundColor(LIGHT_GRAY);
    BlDisplaySetBackgroundColor(BLACK);

    //
    // Set input location at the end of the pin.
    //

    for (InputLocation = 0; Pin[InputLocation] != L'\0'; InputLocation += 1);

    //
    // Read the pin from the console.
    //

    KeyStroke.KeyStroke = 0;
    BlDisplaySetCursorType(INVISIBLE);
    CursorFlags = BG_DISPLAY_STRING_FLAGS_NONE;
    Status = STATUS_SUCCESS;
    if (ShutdownDisabled == FALSE) {
        StartTime = BlTimeGetRelativeTime();

    } else {
        StartTime = 0;
    }

    for(;;) {

        //
        // Shutdown the machine after waiting 1 minute for input.
        //

        if (ShutdownDisabled == FALSE) {
            CurrentTime = BlTimeGetRelativeTime();
            if ((CurrentTime - StartTime) > FVE_DISPLAY_TIMEOUT) {
                BlFwShutdown();
            }
        }

        //
        // Calculate the current on screen location from our location inside
        // the key.  Put the cursor there, and wait for input.
        //

        if (FvepDisplayFlashCursor(InputLocation) != FALSE) {
            CursorFlags ^= BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
            FvepDisplayUpdatePin(Pin, *PinFlags, InputLocation, CursorFlags);
        }

        Status = BlDisplayGetKeyStroke(FALSE, &KeyStroke);
        if (!NT_SUCCESS(Status) ||
            (KeyStroke.Flags == BL_KEY_STROKE_FLAGS_NONE)) {

            continue;
        }

        //
        // User entered a key. Reset the start time.
        //

        if (ShutdownDisabled == FALSE) {
            StartTime = BlTimeGetRelativeTime();

        } else {
            StartTime = 0;
        }

        //
        // If the user presses enter then exit the character entry loop, and
        // calculate the final PIN value.
        //
        // If the user hits the escape key, punt with failure.  Force any
        // entered pin digits to be cleared to zero.
        //

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0) {
            if (KeyStroke.UnicodeCharacter == BL_CHAR_ENTER) {
                break;
            }

            if (KeyStroke.UnicodeCharacter == BL_CHAR_ESCAPE) {
                InputLocation = 0;
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_ESC_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPinConsoleEnd;
            }
        }

        //
        // Handle scan code input.
        //

        if (CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) {

            //
            // Check the windows key for appropriate action.
            //

            if ((NoKeyboard != FALSE) &&
                (KeyStroke.ScanCode == BL_SCAN_WINDOWS_KEY)) {

                InputLocation = 0;
                SET_FLAGS(*RecoveryFlags, BSD_FVE_ERROR_USER_REQUESTED_WIN_KEY);
                Status = STATUS_FVE_LOCKED_VOLUME;
                goto DisplayPinConsoleEnd;
            }

            //
            // If the user hits the F11 key on a multi-boot system, return
            // cancelled status and force any entered pin digits to be cleared
            // to zero.
            //

            if ((KeyStroke.ScanCode == BL_SCAN_F11) &&
                (MultiBootSystem != FALSE)) {

                InputLocation = 0;
                Status = STATUS_CANCELLED;
                goto DisplayPinConsoleEnd;
            }

            //
            // The HOME key deletes all entered digits and moves the cursor to
            // the start of the PIN.  It also clears the screen so the user
            // knows that they are back at the beginning with a clear PIN.
            //

            if (KeyStroke.ScanCode == BL_SCAN_HOME) {
                InputLocation = 0;
                RtlSecureZeroMemory(
                    Pin,
                    ((FVEB_MAX_PIN_DIGITS + 1) * sizeof(WCHAR)));

                FvepDisplayUpdatePin(Pin,
                                     *PinFlags,
                                     InputLocation,
                                     CursorFlags);

                continue;
            }

            if (KeyStroke.ScanCode == BL_SCAN_INSERT) {
                if (CHECK_FLAG(*PinFlags,
                               FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

                    CLEAR_FLAGS(*PinFlags, FVE_CONTEXT_FLAGS_SHOW_TYPING);
                    ShowHideMessageId = BM_FVE_HELP_SHOW_PIN;

                } else {
                    SET_FLAGS(*PinFlags, FVE_CONTEXT_FLAGS_SHOW_TYPING);
                    ShowHideMessageId = BM_FVE_HELP_HIDE_PIN;
                }

                FveFunctions->UpdateHelpInformation(ShowHideMessageId,
                                                    HideShowHandle);

                FvepDisplayUpdatePin(Pin,
                                     *PinFlags,
                                     InputLocation,
                                     CursorFlags);

                continue;
            }

            if ((KeyStroke.ScanCode >= BL_SCAN_F1) &&
                (KeyStroke.ScanCode <= BL_SCAN_F10)) {

                FvepDisplayNumberForFunctionChar(&KeyStroke);
                KeyStroke.UnicodeCharacter += L'0';
            }
        }

        //
        // The LEFT arrow key and the BACKSPACE key move the cursor to the digit
        // just before the current one.  They also erase that digit.
        //

        if (((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_CHARACTER) != 0) &&
             (KeyStroke.UnicodeCharacter == BL_CHAR_BACKSPACE)) ||
            ((CHECK_FLAG(KeyStroke.Flags,
                         BL_KEY_STROKE_FLAGS_SCAN_CODE) != 0) &&
             (KeyStroke.ScanCode == BL_SCAN_LEFT))) {

            if (InputLocation > 0) {
                InputLocation -= 1;
                Pin[InputLocation] = L'\0';
                FvepDisplayUpdatePin(Pin,
                                     *PinFlags,
                                     InputLocation,
                                     CursorFlags);
            }

            FvepDisplayUpdatePin(Pin, *PinFlags, InputLocation, CursorFlags);
            continue;
        }

        //
        // Filter out any more scan codes via a non-printable character check.
        //

        if (FvepDisplayIsPrintableCharacter(KeyStroke) == FALSE) {
            continue;
        }

        ASSERT(CHECK_FLAG(KeyStroke.Flags, BL_KEY_STROKE_FLAGS_CHARACTER) != 0);

        //
        // If this is not an enhanced PIN prompt, then ignore any characters
        // which are non-numeric (note that F1-F10 have already been remapped
        // to look like numeric input).
        //

        if (((KeyStroke.UnicodeCharacter < L'0') ||
             (KeyStroke.UnicodeCharacter > L'9')) &&
            (UseEnhancedPin == FALSE)) {

            continue;
        }

        //
        // Display and store the key digit only if we haven't gone over the
        // 20 character max. We will not show the extra characters over 20.
        // They do not get stored. If the pin is incorrect, the user
        // can still fix the pin by backspacing over it.
        //

        if (InputLocation < FVEB_MAX_PIN_DIGITS)
        {
            Pin[InputLocation] = KeyStroke.UnicodeCharacter;
            InputLocation += 1;
            FvepDisplayUpdatePin(Pin, *PinFlags, InputLocation, CursorFlags);
        }
    }

    //
    // Blank the title to indicate that validation is taking place.
    //

    FveFunctions->UpdateTitle(BM_FVE_EMPTY_STRING);

DisplayPinConsoleEnd:

    //
    // Remove the cursor if necessary.
    //

    if (CHECK_FLAG(CursorFlags, BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR) != FALSE) {
        CursorFlags &= ~BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR;
        FvepDisplayUpdatePin(Pin, *PinFlags, InputLocation, CursorFlags);
    }

    //
    // Clear any remaining PIN digits to zero.
    //

    for ( ; InputLocation < FVEB_MAX_PIN_DIGITS + 1; InputLocation += 1) {
        Pin[InputLocation] = 0;
    }

    //
    // Wipe the input buffer.
    //

    BlDisplaySetCursorType(INVISIBLE);
    BlDisplayWipeInputBuffer(NULL);
    return Status;
}

NTSTATUS
FvepDisplayPinOverride (
    __inout PFVE_PIN_CONTEXT PinContext
    )

/*++

Routine Description:

    This routine obtains the full volume encryption PIN from
    the bcd override. It then purges the override.

Arguments:

    PinContext - Supplies a pointer to the FVE pin context.

Return Value:

    STATUS_SUCCESS if a key was completely onbtained, validated and the BCD
        setting was purged.
    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    HANDLE DataStoreHandle;
    UINTN KeyCharsLength;
    PWCHAR KeyCharsTemp;
    NTSTATUS Status;

    DataStoreHandle = NULL;
    KeyCharsLength = 0;
    KeyCharsTemp = NULL;

    //
    // See if a bcd based test override has been defined for obtaining the
    // key. If so, then obtain the key from the value from the bcd
    // entry.
    //

    Status = BlGetApplicationOptionString(
                 BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE,
                 &KeyCharsTemp);

    if (!NT_SUCCESS(Status)) {
        goto DisplayPinOverrideEnd;
    }

    //
    // Always try to delete the BCD entry after it is read, both from the in-
    // memory list and from the persistent store. If it is valid, it should
    // only be used once. If it is invalid, it should not be used and the
    // invalid value should be deleted.
    //

    BlRemoveApplicationOption(BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE);

    Status = BmOpenDataStore(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPinOverrideEnd;
    }

    Status = BmPurgeOption(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           BCDE_BOOTMGR_TYPE_PINPASSPHRASE_OVERRIDE);
    if (!NT_SUCCESS(Status)) {
        goto DisplayPinOverrideEnd;
    }

    //
    // Validate and use the PIN.
    //

    KeyCharsLength = wcslen(KeyCharsTemp);
    if (KeyCharsLength > FVEB_MAX_PIN_DIGITS) {
        Status = STATUS_CANCELLED;
        goto DisplayPinOverrideEnd;
    }

    wcsncpy_s(PinContext->Pin,
              FVEB_MAX_PIN_DIGITS + 1,
              KeyCharsTemp,
              FVEB_MAX_PIN_DIGITS);

DisplayPinOverrideEnd:
    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
        DataStoreHandle = NULL;
    }

    if(KeyCharsTemp != NULL) {
        BlMmFreeHeap(KeyCharsTemp);
        KeyCharsTemp = NULL;
    }

    return Status;
}

BOOLEAN
FvepDisplayRequiresInput (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine determines if the current FVE screen requires user input for a
    password, PIN, or passphrase.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    TRUE if the screen requires advanced input. FALSE otherwise.

--*/

{

    switch (DisplayContext->ScreenType) {
    case FveScreenPin:
    case FveScreenPinIncorrect:
    case FveScreenPassword:
    case FveScreenPasswordIncorrect:
    case FveScreenPassphrase:
    case FveScreenPassphraseIncorrect:
        return TRUE;
    }

   return FALSE;
}

VOID
FvepDisplayString (
    __in ULONG MessageId,
    __in BOOLEAN StripNewline
    )

/*++

Routine Description:

    This routine displays a string based on a resource message ID.

Arguments:

    MessageId - Supplies the ID of the resource message to be displayed.

    StripNewLine - Supplies the boolean value indicating whether or not to
        strip newline and carriage return characters from the string.

Return Value:

    None.

--*/

{

    PWCHAR OsxmlString;
    NTSTATUS Status;
    PWCHAR String;

    String = BlResourceFindMessage(MessageId);

    NT_ASSERT(String != NULL);

    Status = BlXmiTransformTextForXml(String, &OsxmlString);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    if (StripNewline != FALSE) {
        FvepDisplayStripNewline(OsxmlString);
    }

    BlXmiWrite(OsxmlString);
    BlMmFreeHeap(OsxmlString);
    return;
}

VOID
FvepDisplayStripNewline (
    __inout_z PWCHAR String
    )

/*++

Routine Description:

    This routine removes new line characters and carriage return characters
    from a string.

    This is necessary because concatenating strings from the resource file
    causes extra spaces to appear on the local console where the \n and \r are
    at the end of each string.

Arguments:

    String - Supplies a pointer to a string that will be updated.

Return Value:

    None.

--*/

{

    ULONG Index;

    NT_ASSERT(wcslen(String) >= 1);
    __analysis_assume(wcslen(String) >= 1);

    Index = 0;
    while (String[Index] != L'\0') {
        if ((String[Index] == L'\n') || (String[Index] == L'\r')) {
            String[Index] = L'\0';
        }

        Index += 1;
    }

    return;
}

VOID
FvepDisplayUpdatePassphrase (
    __in_ecount_z(FVEB_MAX_PASSPHRASE_LENGTH + 1) PWCHAR Passphrase,
    __in ULONG PassphraseFlags,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    )

/*++

Routine Description:

    This routine updates the passphrase input. It reads the passphrase flags to
    determine if the text should be shown or not.

Arguments:

    Passphrase - Supplies the passphrase to be displayed.

    PassphraseFlags - Supplies passphrase flags indicating how the passphrase
        should be displayed.

    CursorPosition - Supplies the position of the cursor within the passphrase.

    CursorFlags - Supplies cursor flags indicating whether the cursor should be
        shown or not.

Return Value:

    None.

--*/

{

    PWCHAR DisplayPassphrase;
    WCHAR HiddenCharacter;
    WCHAR HiddenPassphrase[FVEB_MAX_PASSPHRASE_LENGTH + 1];
    ULONG Index;

    __analysis_assume(wcslen(Passphrase) >= 1);
    __analysis_assume(wcslen(Passphrase) <= (FVEB_MAX_PASSPHRASE_LENGTH + 1));

    //
    // Decide which hidden character to used based on the display
    // functions. Legacy cannot use the BULLET because headless terminals are
    // not guaranteed to have that character.
    //

    if (FveFunctions == &StandardDisplayFunctions) {
        HiddenCharacter = BULLET;

    } else {
        HiddenCharacter = ASTERISK;
    }

    if (CHECK_FLAG(PassphraseFlags, FVE_CONTEXT_FLAGS_SHOW_TYPING) == FALSE) {
        for (Index = 0; Passphrase[Index] != L'\0'; Index += 1) {
            HiddenPassphrase[Index] = HiddenCharacter;
        }

        HiddenPassphrase[Index] = L'\0';
        DisplayPassphrase = HiddenPassphrase;

    } else {
        DisplayPassphrase = Passphrase;
    }

    FveFunctions->UpdateInput(DisplayPassphrase,
                              CursorPosition,
                              CursorFlags);

    return;
}

VOID
FvepDisplayUpdatePassword (
    __in_ecount_z(KEY_DIGITS + 1) PCHAR Password,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    )

/*++

Routine Description:

    This routine updates the password input.

Arguments:

    Password - Supplies the password to be displayed.

    CursorPosition - Supplies the position of the cursor within the password.

    CursorFlags - Supplies cursor flags indicating whether the cursor should be
        shown or not.

Return Value:

    None.

--*/

{

    WCHAR DisplayPassword[KEY_DIGITS_WITH_SEPARATORS];
    ULONG DisplayIndex;
    ULONG Index;

    __analysis_assume(strlen(Password) >= 1);
    __analysis_assume(strlen(Password) <=
                      (sizeof(DisplayPassword) / sizeof(WCHAR)));

    //
    // The password displayed has "-" separators between each group.
    //

    DisplayIndex = 0;
    for (Index = 0; Password[Index] != '\0'; Index += 1) {
        __analysis_assume(DisplayIndex <= (KEY_DIGITS_WITH_SEPARATORS - 1));
        DisplayPassword[DisplayIndex] = Password[Index];

        //
        // After a full group (except the last full group), add a separating
        // character in the display password.
        //

        if ((((Index + 1) / DIGIT_GROUP_SIZE) < DIGIT_GROUP_COUNT) &&
            (((Index + 1) % DIGIT_GROUP_SIZE) == 0)) {

            DisplayIndex += 1;
            __analysis_assume(DisplayIndex <= (KEY_DIGITS_WITH_SEPARATORS - 1));
            DisplayPassword[DisplayIndex] = SEPARATOR;
            if (CursorPosition >= DisplayIndex) {
                CursorPosition += 1;
            }
        }

        DisplayIndex += 1;
    }

    __analysis_assume(DisplayIndex <= (KEY_DIGITS_WITH_SEPARATORS - 1));
    DisplayPassword[DisplayIndex] = L'\0';
    FveFunctions->UpdateInput(DisplayPassword,
                              CursorPosition,
                              CursorFlags);

    return;
}

VOID
FvepDisplayUpdatePin (
    __in_ecount_z(FVEB_MAX_PIN_DIGITS + 1) PWCHAR Pin,
    __in ULONG PinFlags,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    )

/*++

Routine Description:

    This routine updates the pin input. It reads the pin flags to determine if
    the text should be shown or not.

Arguments:

    Pin - Supplies the pin to be displayed.

    PinFlags - Supplies pin flags indicating how the pin should be displayed.

    CursorPosition - Supplies the position of the cursor within the pin.

    CursorFlags - Supplies cursor flags indicating whether the cursor should be
        shown or not.

Return Value:

    None.

--*/

{

    PWCHAR DisplayPin;
    WCHAR HiddenCharacter;
    WCHAR HiddenPin[FVEB_MAX_PIN_DIGITS + 1];
    ULONG Index;

    __analysis_assume(wcslen(Pin) >= 1);
    __analysis_assume(wcslen(Pin) <= (FVEB_MAX_PIN_DIGITS + 1));

    //
    // Decide which hidden character to used based on the display
    // functions. Legacy cannot use the BULLET because headless terminals are
    // not guaranteed to have that character.
    //

    if (FveFunctions == &StandardDisplayFunctions) {
        HiddenCharacter = BULLET;

    } else {
        HiddenCharacter = ASTERISK;
    }

    if (CHECK_FLAG(PinFlags, FVE_CONTEXT_FLAGS_SHOW_TYPING) == FALSE) {
        for (Index = 0; Pin[Index] != L'\0'; Index += 1) {
            HiddenPin[Index] = HiddenCharacter;
        }

        HiddenPin[Index] = L'\0';
        DisplayPin = HiddenPin;
    } else {
        DisplayPin = Pin;
    }

    FveFunctions->UpdateInput(DisplayPin,
                              CursorPosition,
                              CursorFlags);

    return;
}

BOOL
FvepDisplayValidGroup (
    __in_ecount(KEY_DIGITS) PCHAR KeyChars,
    __in ULONG KeyInputLocation
    )

/*++

Routine Description:

    This routine determines whether a given group of digits is valid or not. If
    it is not valid, a message is displayed to the user indicating that it is
    invalid.

Arguments:

    KeyChars - Supplies the characters that are currently in the key.

    KeyInputLocation - Supplies the location in the key whose digit
        group should be validated.

    Measurements - Supplies a pointer to a structure containing measurements of
        text cell resolutions.

Return Value:

    TRUE if the digit group is valid.  FALSE otherwise.

--*/

{

    ULONG GroupLocation;
    ULONG GroupValue;
    ULONG Index;

    //
    // If not all of the digits in the group have been entered, then return
    // FALSE without actually validating the group.  In this case we also do
    // not change the display of the group on the screen.  Note that we depend
    // on the fact that KeyChars is cleared to zero before it is used.
    //

    GroupLocation = KeyInputLocation - (KeyInputLocation % DIGIT_GROUP_SIZE);
    for (Index = 0; Index < DIGIT_GROUP_SIZE; Index += 1) {
        if (KeyChars[GroupLocation + Index] == 0) {
            return FALSE;
        }
    }

    //
    // If all of the digits in the group have been entered, then validate the
    // group, and update the display of the group on screen based on the
    // result of the validation.  If the group is invalid, it is highlighted on
    // the screen, and a message asking the user to fix the highlighted group
    // is displayed.  If the group is valid, then any highlighting is removed,
    // from the group and the message is also removed from the screen.
    //

    NT_VERIFY(FvepScanFullGroup(&KeyChars[GroupLocation], &GroupValue));
    if ((GroupValue % CHECKSUM) == 0) {
        FveFunctions->UpdateTitle(BM_FVE_TITLE_PASSWORD);
        return TRUE;
    }

    FveFunctions->UpdateTitle(BM_FVE_TITLE_PASSWORD_WRONG_GROUP);
    return FALSE;
}

VOID
FvepLegacyClearScreen (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine wipes the screen and indicates to the headless terminal that a
    new screen was drawn.

Arguments:

    DisplayContext - Supplies a pointer to the display context for the screen.

Return Value:

    None.

--*/

{

    BlXmiWrite(L"<bootmgr-clear-screen/>");

    //
    // Draw an invisible EOT character.
    //

    BlDisplaySetForegroundColor(BLACK);
    BlDisplaySetBackgroundColor(BLACK);
    BlDisplayPrintString(L"%c", EOT);
    BlDisplayPrintString(L"%d", DisplayContext->ScreenType);
    BlDisplaySetForegroundColor(LIGHT_GRAY);
    return;
}

VOID
FvepLegacyInitializeErrorInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine displays the FVE error information to the screen.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    //
    // Display the error text based on the screen type.
    //

    switch (DisplayContext->ScreenType) {
    case FveScreenAdvancedOptions:
        BlXmiWrite(L"<fve-error-select error-text='");
        FvepDisplayString(BM_FVE_ERROR_ADVANCED_OPTIONS_1, FALSE);
        BlXmiWrite(L"'/>");
        break;

    case FveScreenPinTooManyAttempts:
    case FveScreenPassphraseTooManyAttempts:

        BlXmiWrite(L"<fve-error-select error-text='");
        FvepDisplayString(BM_FVE_ERROR_TOO_MANY_ATTEMPTS_1, TRUE);
        BlXmiWrite(L" ");
        FvepDisplayString(BM_FVE_ERROR_TOO_MANY_ATTEMPTS_2, FALSE);
        BlXmiWrite(L"'/>");
        break;

    case FveScreenUsbStartupInvalid:
    case FveScreenUsbRecoveryInvalid:

        BlXmiWrite(L"<fve-error-select error-text='");
        FvepDisplayString(BM_FVE_ERROR_USB_KEY_INVALID, FALSE);
        BlXmiWrite(L"'/>");
        break;

    default:
        break;
    }

    return;
}

VOID
FvepLegacyInitializeHelpInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __out HANDLE *HideShowHandle
    )

/*++

Routine Description:

    This routine displays the FVE help information to the screen.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

    HideShowHandle - Supplies a pointer that receives a handle to the hide/show
        help region.

Return Value:

    None.

--*/

{

    PWCHAR OsxmlString;
    BOOLEAN ShowHelp;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(HideShowHandle);

    ShowHelp = FALSE;

    //
    // Display the screen based help informaiton.
    //

    switch (DisplayContext->ScreenType) {
    case FveScreenPin:
    case FveScreenPinIncorrect:

        ShowHelp = TRUE;
        BlXmiWrite(L"<fve-help-select ");

        //
        // For PIN screens (not ePIN), display the number help.
        //

        if (DisplayContext->PinContext.UseEnhancedPin == FALSE) {
            BlXmiWrite(L"help-text='");
            FvepDisplayString(BM_FVE_HELP_KEYS, FALSE);
            BlXmiWrite(L"'");
        }

        //
        // Display show/hide PIN help.
        //

        BlXmiWrite(L" showhide-text='");
        if (CHECK_FLAG(*DisplayContext->PinContext.PinFlags,
                       FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

            FvepDisplayString(BM_FVE_HELP_HIDE_PIN, FALSE);

        } else {
            FvepDisplayString(BM_FVE_HELP_SHOW_PIN, FALSE);
        }

        BlXmiWrite(L"'");
        break;

    case FveScreenPassword:
    case FveScreenPasswordIncorrect:

        ShowHelp = TRUE;
        BlXmiWrite(L"<fve-help-select help-text='");
        FvepDisplayString(BM_FVE_HELP_KEYS, FALSE);
        BlXmiWrite(L"'");

        //
        // Display the password ID if present.
        //

        if (DisplayContext->PasswordContext.PasswordId != NULL) {
            Status = BlXmiTransformTextForXml(
                         DisplayContext->PasswordContext.PasswordId,
                         &OsxmlString);

            if (!NT_SUCCESS(Status)) {
                goto InitializeHelpInformationEnd;
            }

            BlXmiWrite(L" passwordid-text='");
            FvepDisplayString(BM_FVE_HELP_PASSWORD_ID, FALSE);
            BlXmiWrite(L" ");
            BlXmiWrite(OsxmlString);
            BlXmiWrite(L"'");
            BlMmFreeHeap(OsxmlString);
        }

        break;

    case FveScreenPassphrase:
    case FveScreenPassphraseIncorrect:

        ShowHelp = TRUE;

        //
        // Display show/hide passphrase help.
        //

        BlXmiWrite(L"<fve-help-select showhide-text='");
        if (CHECK_FLAG(*DisplayContext->PassphraseContext.PassphraseFlags,
                       FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

            FvepDisplayString(BM_FVE_HELP_HIDE_PASSPHRASE, FALSE);

        } else {
            FvepDisplayString(BM_FVE_HELP_SHOW_PASSPHRASE, FALSE);
        }

        BlXmiWrite(L"'");
        break;

    default:
        break;
    }

    //
    // Display the additional help information common to many screens.
    //

InitializeHelpInformationEnd:
    if (ShowHelp != FALSE) {
        BlXmiWrite(L"/>");
    }

    return;
}

VOID
FvepLegacyInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    )

/*++

Routine Description:

    This routine opens and closes the XSL template for the FVE display
    screen. Upon closing the XSL template, it writes EOT characters over the
    headless terminal for test.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

    Phase - Supplise the phase of the display initialization for the current
        screen.

Return Value:

    None.

--*/

{

    if (Phase == 0) {
        BlXmiWrite(L"<fve-legacy-ui ");

    } else {

        NT_ASSERT(Phase == 1);

        //
        // This call will wipe the screen black and then draw the new contents.
        //

        BlXmiWrite(L"</fve-legacy-ui>");

        //
        // Send End-Of-Transmission character for test. Make it invisible. This
        // requires blending it with the footer bar.
        //

        BlDisplaySetForegroundColor(LIGHT_GRAY);
        BlDisplaySetBackgroundColor(LIGHT_GRAY);
        BlDisplayPrintString(L"%c", EOT);
        BlDisplayPrintString(L"%d", DisplayContext->ScreenType);
        BlDisplayPrintString(L"%d", *(DisplayContext->RecoveryFlags));
    }

    return;
}

VOID
FvepLegacyInitializeFooter (
    __in ULONG FooterFlags
    )

/*++

Routine Descriptoin:

    This routine initializes the legacy footer. Nothing needs to be done.

Arguments:

    FooterFlags - Supplies flags indicating which type of footer to display.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(FooterFlags);

    return;
}

VOID
FvepLegacyInitializeInput (
    VOID
    )

/*++

Routine Description:

    This routine enables legacy input by turning on the input selector in the
    XSL template.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BlXmiWrite(L"fve-input-select='true' ");
    return;
}

VOID
FvepLegacyInitializeRecoveryInformation (
    __in_opt PWSTR RecoveryErrorString
    )

/*++

Routine Description:

This routine initializes the legacy recovery error information to be
display to the screen.

Arguments:

RecoveryErrorString - Supplies the recovery error string to display.
NULL means there is no error string to display.

Return Value:

None.

--*/

{

    PWCHAR OsxmlEscapedRecoveryString;
    PWCHAR OsxmlString;
    PWCHAR RecoveryString;
    NTSTATUS Status;

    //
    // Display where the user can find the recovery key.
    //

    RecoveryString = FvepDisplayCreateRecoveryUrlString(
        BM_FVE_RETRIEVE_RECOVERY_KEY_DEFAULT_MESSAGE,
        BM_FVE_RETRIEVE_RECOVERY_KEY_DEFAULT_URL);

    OsxmlEscapedRecoveryString = NULL;

    if (RecoveryString != NULL) {
        Status = BlXmiTransformTextForXml(RecoveryString, &OsxmlEscapedRecoveryString);

        if (NT_SUCCESS(Status)) {
            BlXmiWrite(L"<fve-recovery-help-select recovery-help-text='");
            BlXmiWrite(OsxmlEscapedRecoveryString);
            BlXmiWrite(L"'/>");
        } else {

            // 
            // On failure to transform the message, do an in-place replace of 
            // characters that may cause the XML parser to fail.
            //

            BlXmiSanitizeTextForXml(RecoveryString);

            BlXmiWrite(L"<fve-recovery-help-select recovery-help-text='");
            BlXmiWrite(RecoveryString);
            BlXmiWrite(L"'/>");
        }
    }

    //
    // Display the reason for the recovery.
    //

    if (RecoveryErrorString != NULL) {
        Status = BlXmiTransformTextForXml(RecoveryErrorString, &OsxmlString);
        if (!NT_SUCCESS(Status)) {
            goto LegacyInitializeRecoveryInformationEnd;
        }

        BlXmiWrite(L"<fve-recovery-select recovery-text='");
        FvepDisplayString(BM_FVE_RECOVERY, TRUE);
        BlXmiWrite(L" ");
        BlXmiWrite(OsxmlString);
        BlXmiWrite(L"'/>");
        BlMmFreeHeap(OsxmlString);
    }

LegacyInitializeRecoveryInformationEnd:
    if (RecoveryString != NULL) {
        BlMmFreeHeap(RecoveryString);
    }

    if (OsxmlEscapedRecoveryString != NULL) {
        BlMmFreeHeap(OsxmlEscapedRecoveryString);
    }
    return;
}

VOID
FvepLegacyInitializeTitle (
    __in_z PWCHAR Title
    )

/*++

Routine Description:

    This routine initializes the title with the supplied title string.

Arguments:

    Title - Supplies the title string to be displayed to the screen.

Return Value:

    None.

--*/

{

    PWCHAR OsxmlTitle;
    NTSTATUS Status;

    NT_ASSERT(Title != NULL);

    Status = BlXmiTransformTextForXml(Title, &OsxmlTitle);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    BlXmiWrite(L"<fve-title-select title='");
    BlXmiWrite(OsxmlTitle);
    BlXmiWrite(L"'/>");
    BlMmFreeHeap(OsxmlTitle);
    return;
}

VOID
FvepLegacyUpdateHelpInformation (
    __in ULONG MessageId,
    __in HANDLE Handle
    )

/*++

Routine Description:

    This routine updates the help information field with the string
    corresponding to the resource message ID.

Arguments:

    MessageId - Supplies the resource message ID for the new string.

    Handle - Supplies a handle to the body region that needs updating.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Handle);

    BlXmiWrite(L"<fve-help-info-showhide-update value='");
    FvepDisplayString(MessageId, FALSE);
    BlXmiWrite(L"'/>");
    return;
}

VOID
FvepLegacyUpdateInput (
    __in_z PWCHAR Input,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    )

/*++

Routine Description:

    This routine updates the input region in the legacy windowing system.

Arguments:

    Input - Supplies the updated input string.

    CursorPosition - Supplies the cursor's index within the string.

    CursorFlags - Supplies flags indicating whether or not to show the cursor.

Return Value:

    None.

--*/

{

    CONSOLE_COLOR BackgroundColor;
    ULONG CellHeight;
    ULONG CellWidth;
    CONSOLE_COLOR ForegroundColor;
    ULONG Index;
    ULONG StartIndex;

    BlDisplayGetTextCellResolution(&CellWidth, &CellHeight);
    BlDisplaySetCursorPosition(ENTRY_ROW * CellHeight,
                               ENTRY_COLUMN * CellWidth);

    //
    // Determine the start index to properly window the input.
    //

    if (CursorPosition > LEGACY_WINDOW_SIZE) {
        StartIndex = CursorPosition - LEGACY_WINDOW_SIZE;
    } else {
        StartIndex = 0;
    }

    //
    // Disply the input string to the region, drawing the cursor in the
    // specified location.
    //

    __analysis_assume(wcslen(Input) >= 1);

    for (Index = StartIndex;
         (Input[Index] != L'\0') && (Index < StartIndex + LEGACY_WINDOW_SIZE);
         Index += 1) {

        if ((Index == CursorPosition) &&
            (CHECK_FLAG(CursorFlags,
                        BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR) != FALSE)) {

            BlDisplayGetTextColor(&BackgroundColor, &ForegroundColor);
            BlDisplaySetTextColor(ForegroundColor, BackgroundColor);
            BlDisplayPrintString(L"%c", Input[Index]);
            BlDisplaySetTextColor(BackgroundColor, ForegroundColor);
        } else {

            BlDisplayPrintString(L"%c", Input[Index]);
        }
    }

    //
    // Clear the rest of the line. The clear to end of line routine does not
    // work here as it will destroy the "]" at the end of the input region.
    //

    for (; Index < StartIndex + LEGACY_WINDOW_SIZE; Index += 1) {
        if ((Index == CursorPosition) &&
            (CHECK_FLAG(CursorFlags,
                        BG_DISPLAY_STRING_FLAGS_SHOW_CURSOR) != FALSE)) {

            BlDisplayGetTextColor(&BackgroundColor, &ForegroundColor);
            BlDisplaySetTextColor(ForegroundColor, BackgroundColor);
            BlDisplayPrintString(L"%c", L' ');
            BlDisplaySetTextColor(BackgroundColor, ForegroundColor);

        } else  {
            BlDisplayPrintString(L"%c", L' ');
        }
    }

    return;
}

VOID
FvepLegacyUpdateTitle (
    __in ULONG TitleId
    )

/*++

Routine Description:

    This routine updates the title in the legacy windowing system.

Arguments:

    TitleId - Supplies the resource ID of the new title string to display.

Return Value:

    None.

--*/

{

    BlXmiWrite(L"<fve-title-update value='");
    if (TitleId != BM_FVE_EMPTY_STRING) {
        FvepDisplayString(TitleId, FALSE);

    } else {
        BlXmiWrite(L" ");
    }

    BlXmiWrite(L"'/>");
    return;
}

BOOLEAN
FvepScanFullGroup (
    __in_ecount(DIGIT_GROUP_SIZE) PCHAR KeyChars,
    __out PULONG GroupValue
    )

/*++

Routine Description:

    This routine parses 6 digits in the group string and returns the value.

Arguments:

    KeyChars - Supplies the characters that are currently in the key.

    GroupValue - Receives a value fot the group.

Return Value:

    TRUE if the digit group parsed successfully, FALSE otherwise.

--*/

{

    //
    // NB. Do not use the scanf or variations to parse the input string.
    //     Reference to the CRT adds a significant amount of code to the
    //     bootmgr.
    //

    ULONG CharIndex;
    ULONG CurrentChar;
    BOOLEAN HasDigits;

    *GroupValue = 0;
    HasDigits = FALSE;
    for (CharIndex = 0; CharIndex < DIGIT_GROUP_SIZE; CharIndex += 1) {
        CurrentChar = *(KeyChars + CharIndex);
        if ((CurrentChar < '0') || (CurrentChar > '9')) {
            if (!HasDigits) {
                if (CurrentChar == ' ') {
                    continue;
                }
            }
            return HasDigits;
        }

        HasDigits = TRUE;
        CurrentChar -= '0';
        *GroupValue *= 10;
        *GroupValue += CurrentChar;
    }

    return HasDigits;
}

VOID
FvepStandardClearScreen (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine wipes the screen.

Arguments:

    DisplayContext - Supplies a pointer to the display context for the screen.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(DisplayContext);

    BmBgDisplayDestroy();
    return;
}

VOID
FvepStandardInitialize (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __in ULONG Phase
    )

/*++

Routine Description:

    This routine opens and closes the XSL template for the FVE display
    screen. It additionally initializes the boot manager's BGFX display
    windowing system.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

    Phase - Supplise the phase of the display initialization for the current
        screen.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    PWSTR String;

    UNREFERENCED_PARAMETER(DisplayContext);

    if (Phase == 0) {

        //
        // Unless there is an error, the user is going to have to provide
        // input, so initialize the key strokes before drawing the screen.
        //

        BlDisplayInitializeKeyStrokes();

        //
        // Initialize the boot managers display library that uses BGFX.
        //

        Status = BmBgDisplayInitialize();
        if (!NT_SUCCESS(Status)) {
            return;
        }

        BlXmiWrite(L"<fve-standard-ui ");

        //
        // In the Standard case the XSL template does not draw anything, so set
        // the header string which will be drawn later by the render routine.
        // Use bitlocker recovery header on recovery pages and bitlocker header
        // on all other pages.
        //

        switch (DisplayContext->ScreenType) {
        case FveScreenPassword:
        case FveScreenPasswordIncorrect:
        case FveScreenUsbRecoveryInsert:
        case FveScreenUsbRecoveryInvalid:
            String = BlResourceFindMessage(BM_FVE_HEADER_RECOVERY);
            break;

        default:
            String = BlResourceFindMessage(BM_FVE_HEADER);
            break;
        }

        BmBgDisplaySetHeader(String);

    } else {

        NT_ASSERT(Phase == 1);

        //
        // Close the XSL template. This will not clear the screen or draw new
        // contents. The render routine is what will draw the updated screen
        // based on screen setup during phase 0 initialization.
        //

        BlXmiWrite(L"</fve-standard-ui>");
        BmBgDisplayRender(FALSE);
    }

    return;
}

VOID
FvepStandardInitializeErrorInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext
    )

/*++

Routine Description:

    This routine initializes the FVE error information.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

Return Value:

    None.

--*/

{

    PWCHAR String;

    switch (DisplayContext->ScreenType) {
    case FveScreenAdvancedOptions:
        String = BlResourceFindMessage(BM_FVE_ERROR_ADVANCED_OPTIONS_1);
        BmBgDisplayAddBodyRegion(String, NULL);
        break;

    case FveScreenPinTooManyAttempts:
    case FveScreenPassphraseTooManyAttempts:

        String = BlResourceFindMessage(BM_FVE_ERROR_TOO_MANY_ATTEMPTS_1);
        BmBgDisplayAddBodyRegion(String, NULL);
        BmBgDisplayAddBodyRegion(NULL, NULL);
        String = BlResourceFindMessage(BM_FVE_ERROR_TOO_MANY_ATTEMPTS_2);
        BmBgDisplayAddBodyRegion(String, NULL);
        break;

    case FveScreenUsbStartupInvalid:
    case FveScreenUsbRecoveryInvalid:

        String = BlResourceFindMessage(BM_FVE_ERROR_USB_KEY_INVALID);
        BmBgDisplayAddBodyRegion(String, NULL);
        break;

    default:
        break;
    }

    return;
}

VOID
FvepStandardInitializeFooter (
    __in ULONG FooterFlags
    )

/*++

Routine Descriptoin:

    This routine initializes the standard footer. This adds strings to the
    footer region list. They will be display stacked on top of each other in
    reverse order.

Arguments:

    FooterFlags - Supplies flags indicating which type of footer to display.

Return Value:

    None.

--*/

{

    PWCHAR String;

    //
    // Initialize them in reverse order so they will appear top-down on the
    // screen.
    //

    if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_F11_ENABLED)) {
        String = BlResourceFindMessage(BM_FVE_ACTION_F11);
        BmBgDisplayAddFooterRegion(String);
    }

    if (!CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_SKIP_ESC) &&
         CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_ESC_MORE_OPTIONS)) {

        if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
            String = BlResourceFindMessage(BM_FVE_ACTION_ESC_WIN_MORE_OPTIONS);

        } else {
            String = BlResourceFindMessage(BM_FVE_ACTION_ESC_MORE_OPTIONS);
        }

        BmBgDisplayAddFooterRegion(String);

    } else if (!CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_SKIP_ESC)) {
        if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {
            String = BlResourceFindMessage(BM_FVE_ACTION_ESC_WIN_RECOVERY);

        } else {
            String = BlResourceFindMessage(BM_FVE_ACTION_ESC_RECOVERY);
        }

        BmBgDisplayAddFooterRegion(String);
    }

    if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_ENTER_REBOOT)) {
        String = BlResourceFindMessage(BM_FVE_ACTION_ENTER_REBOOT);

    } else if (CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_SKIP_ESC) &&
               CHECK_FLAG(FooterFlags, FVE_FOOTER_FLAGS_NO_KEYBOARD)) {

        String = BlResourceFindMessage(BM_FVE_ACTION_ENTER_WIN_CONTINUE);

    } else {
        String = BlResourceFindMessage(BM_FVE_ACTION_ENTER_CONTINUE);
    }

    BmBgDisplayAddFooterRegion(String);
    return;
}

VOID
FvepStandardInitializeHelpInformation (
    __in PFVE_DISPLAY_CONTEXT DisplayContext,
    __out HANDLE *HideShowHandle
    )

/*++

Routine Description:

    This routine displays the FVE help information to the screen.

Arguments:

    DisplayContext - Supplies a pointer to the FVE display context.

    HideShowHandle - Supplies a pointer that receives a handle to the hide/show
        help region.

Return Value:

    None.

--*/

{

    PWCHAR Destination;
    ULONG HideShowMessageId;
    NTSTATUS Status;
    PWCHAR String;

    //
    // Display the screen specific help information.
    //

    switch (DisplayContext->ScreenType) {
    case FveScreenPin:
    case FveScreenPinIncorrect:

        //
        // For PIN screens (not ePIN), display the number help.
        //

        if (DisplayContext->PinContext.UseEnhancedPin == FALSE) {
            String = BlResourceFindMessage(BM_FVE_HELP_KEYS);
            BmBgDisplayAddBodyRegion(String, NULL);
            BmBgDisplayAddBodyRegion(NULL, NULL);
        }

        //
        // Display show/hide PIN help.
        //

        if (CHECK_FLAG(*DisplayContext->PinContext.PinFlags,
                       FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

            HideShowMessageId = BM_FVE_HELP_HIDE_PIN;

        } else {
            HideShowMessageId = BM_FVE_HELP_SHOW_PIN;
        }

        String = BlResourceFindMessage(HideShowMessageId);
        BmBgDisplayAddBodyRegion(String, HideShowHandle);
        break;

    case FveScreenPassword:
    case FveScreenPasswordIncorrect:

        String = BlResourceFindMessage(BM_FVE_HELP_KEYS);
        BmBgDisplayAddBodyRegion(String, NULL);

        //
        // Display the password ID if present.
        //

        if (DisplayContext->PasswordContext.PasswordId != NULL) {
            String = BlResourceFindMessage(BM_FVE_HELP_PASSWORD_ID);
            Status = BlConcatenateStringToString(String, L" ", &Destination);
            if (!NT_SUCCESS(Status)) {
                return;
            }

            String = Destination;
            Status = BlConcatenateStringToString(
                         String,
                         DisplayContext->PasswordContext.PasswordId,
                         &Destination);

            BlMmFreeHeap(String);
            if (!NT_SUCCESS(Status)) {
                return;
            }

            BmBgDisplayAddBodyRegion(NULL, NULL);
            BmBgDisplayAddBodyRegion(NULL, NULL);
            BmBgDisplayAddBodyRegion(Destination, NULL);
        }

        break;

    case FveScreenPassphrase:
    case FveScreenPassphraseIncorrect:

        //
        // Display show/hide passphrase help.
        //

        if (CHECK_FLAG(*DisplayContext->PassphraseContext.PassphraseFlags,
                       FVE_CONTEXT_FLAGS_SHOW_TYPING) != FALSE) {

            HideShowMessageId = BM_FVE_HELP_HIDE_PASSPHRASE;

        } else {
            HideShowMessageId = BM_FVE_HELP_SHOW_PASSPHRASE;
        }

        String = BlResourceFindMessage(HideShowMessageId);
        BmBgDisplayAddBodyRegion(String, HideShowHandle);
        break;

    default:
        break;
    }

    //
    // Display the help info that is common to many screens.
    //

    return;
}

VOID
FvepStandardInitializeInput (
    VOID
    )

/*++

Routine Description:

    This routine enables Standard input by initializing the input region with
    an empty string.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Input will be turned on, so initialize the region.
    //

    BmBgDisplayInitializeInput();
    return;
}

VOID
FvepStandardInitializeRecoveryInformation (
    __in_opt PWSTR RecoveryErrorString
    )

/*++

Routine Description:

    This routine initializes the standard recovery error information to be
    display to the screen.

Arguments:

    RecoveryErrorString - Supplies the recovery error string to display.
        NULL means there is no error string to display.

Return Value:

    None.

--*/


{

    PWCHAR BaseString;
    PWSTR Destination;
    PWCHAR HelpString;
    NTSTATUS Status;

    if (RecoveryErrorString != NULL) {
        BaseString = BlResourceFindMessage(BM_FVE_RECOVERY);
        Status = BlConcatenateStringToString(BaseString, L" ", &Destination);
        if (!NT_SUCCESS(Status)) {
            goto StandardInitializeRecoveryInformationEnd;
        }

        BaseString = Destination;
        Status = BlConcatenateStringToString(BaseString,
                                             RecoveryErrorString,
                                             &Destination);

        BlMmFreeHeap(BaseString);
        if (!NT_SUCCESS(Status)) {
            goto StandardInitializeRecoveryInformationEnd;
        }

        BmBgDisplayAddBodyRegion(Destination, NULL);
    }

StandardInitializeRecoveryInformationEnd:
    HelpString = FvepDisplayCreateRecoveryUrlString(
        BM_FVE_RETRIEVE_RECOVERY_KEY_DEFAULT_MESSAGE, 
        BM_FVE_RETRIEVE_RECOVERY_KEY_DEFAULT_URL);

    BmBgDisplayAddBodyRegion(HelpString, NULL);
    BmBgDisplayAddBodyRegion(NULL, NULL);
    return;
}

VOID
FvepStandardInitializeTitle (
    __in_z PWCHAR Title
    )

/*++

Routine Description:

    This routine initializes the title with the supplied title string.

Arguments:

    Title - Supplies the title string to be displayed to the screen.

Return Value:

    None.

--*/

{

    BmBgDisplaySetTitle(Title);
    return;
}

VOID
FvepStandardUpdateInput (
    __in_z PWCHAR Input,
    __in ULONG CursorPosition,
    __in ULONG CursorFlags
    )

/*++

Routine Description:

    This routine updates the input region in the standard windowing system.

Arguments:

    Input - Supplies the updated input string.

    CursorPosition - Supplies the cursor's index within the string.

    CursorFlags - Supplies flags indicating whether or not to show the cursor.

Return Value:

    None.

--*/

{

    BmBgDisplayUpdateInput(Input, CursorFlags, CursorPosition);
    return;
}

VOID
FvepStandardUpdateTitle (
    __in ULONG TitleId
    )

/*++

Routine Description:

    This routine updates the title in the standard windowing system.

Arguments:

    TitleId - Supplies the resource ID of the new title string to display.

Return Value:

    None.

--*/

{

    PWCHAR Title;

    if (TitleId != BM_FVE_EMPTY_STRING) {
        Title = BlResourceFindMessage(TitleId);

    } else {
        Title = NULL;
    }

    BmBgDisplayUpdateTitle(Title);
    return;
}

VOID
FvepStandardUpdateHelpInformation (
    __in ULONG MessageId,
    __in HANDLE Handle
    )

/*++

Routine Description:

    This routine updates the help information field with the string
    corresponding to the resource message ID.

Arguments:

    MessageId - Supplies the resource message ID for the new string.

    Handle - Supplies a handle to the body region that needs updating.

Return Value:

    None.

--*/

{

    PWCHAR String;

    String = BlResourceFindMessage(MessageId);
    BmBgDisplayUpdateBodyRegion(Handle, String);
    return;
}

