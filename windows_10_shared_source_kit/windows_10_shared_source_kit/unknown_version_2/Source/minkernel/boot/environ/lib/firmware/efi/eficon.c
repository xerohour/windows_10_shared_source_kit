/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    eficon.c

Abstract:

    Wrappers for EFI Console Protocol routines.

    Both the SIMPLE_TEXT_OUTPUT and SIMPLE_INPUT protocol interfaces provide
    functional support for a console device.  This module provides wrappers
    for each supported routine to correct handle cases where the caller is
    in virtual mode, physical mode, etc.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiConInReset (
    __in SIMPLE_INPUT_INTERFACE *ConInInterface,
    __in BOOLEAN ExtendedVerification
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_INPUT.Reset, which will reset input
    device hardware.

Arguments:

    ConInInterface - Supplies an interface for a console input device, returned
        from either OpenProtocol or provided in the EFI System Table.

    ExtendedVerification - Supplies a boolean to indicate that an EFI driver may
        perform a more exhaustive verification operation during device reset.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the device is not functioning correctly and
        could not be reset.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the console interface.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConInInterface, &InterfacePa);
        ConInInterface = (SIMPLE_INPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConIn.Reset call.
    //

    EfiStatus = ConInInterface->Reset(ConInInterface, ExtendedVerification);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConInReadKeyStroke (
    __in SIMPLE_INPUT_INTERFACE *ConInInterface,
    __out EFI_INPUT_KEY *Key
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_INPUT.ReadKeyStroke, which will
    read the next keystroke from an input device.

    If there is no pending keystroke pending, STATUS_NOT_FOUND
    is returned (EFI_NOT_READY).

Arguments:

    ConInInterface - Supplies an interface for a console input device, returned
        from either OpenProtocol or provided in the EFI System Table.

    Key - Supplies a pointer to a buffer that receives the keystroke information
        for the key that was read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND when there is no pending keystroke.

    STATUS_IO_DEVICE_ERROR if the keystroke information was not returned due
        to hardware errors.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS KeyPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConInInterface, &InterfacePa);
        ConInInterface = (SIMPLE_INPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Key, &KeyPa);
        Key = (EFI_INPUT_KEY *)(UINTN)KeyPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConIn.ReadKeyStroke call.
    //

    EfiStatus = ConInInterface->ReadKeyStroke(ConInInterface, Key);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConInExReadKeyStrokeEx (
    __in EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInExProtocol,
    __out EFI_KEY_DATA *KeyData
    )

/*++

Routine Description:

    This routine is a wrapper for Extended Simple Text Input Protocol's
    ReadKeyStrokeEx routine, which will read the next keystroke from an input
    device. This accomodates extended keyboard input.

    If there is no pending keystroke pending, STATUS_NOT_FOUND is returned
    (EFI_NOT_READY).

Arguments:

    ConInExProtocol - Supplies an interface for a console input device returned
        from OpenProtocol.

    KeyData - Supplies a pointer to a buffer that receives the keystroke
        information for the key that was read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND when there is no pending keystroke.

    STATUS_IO_DEVICE_ERROR if the keystroke information was not returned due
        to hardware errors.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS KeyDataPa = {0};
    PHYSICAL_ADDRESS ProtocolPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConInExProtocol, &ProtocolPa);
        ConInExProtocol =
            (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *)(UINTN)ProtocolPa.QuadPart;

        BlMmTranslateVirtualAddress(KeyData, &KeyDataPa);
        KeyData = (EFI_KEY_DATA *)(UINTN)KeyDataPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConInEx.ReadKeyStrokeEx call.
    //

    EfiStatus = ConInExProtocol->ReadKeyStrokeEx(ConInExProtocol, KeyData);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConInExRegisterKeyNotify (
    __in EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInExProtocol,
    __in EFI_KEY_DATA *KeyData,
    __in EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
    __out EFI_HANDLE *NotifyHandle
    )

/*++

Routine Description:

    This routine is a wrapper for the Extended Simple Text Input Protocol's
    RegisterKeyNotify routine, which will register a function to be called when
    a specified keystroke occurs.

Arguments:

    ConInExProtocol - Supplies an interface for a console input device returned
        from OpenProtocol.

    KeyData - Supplies a pointer to key data information for the key that the
        notification routine should be bound to.

    KeyNotificationFunction - Supplies a pointer to a notify routine to be
        called when the key is pressed.

    NotifyHandle - Supplies a pointer to a handle that receives the handle
        assigned to the registered notification.

Return Value:

    NT status code.

--*/

{

    UNREFERENCED_PARAMETER(ConInExProtocol);
    UNREFERENCED_PARAMETER(KeyData);
    UNREFERENCED_PARAMETER(KeyNotificationFunction);
    UNREFERENCED_PARAMETER(NotifyHandle);

    //
    // Callback notifications from UEFI are not supported by the boot
    // infrastructure.
    //

    return STATUS_NOT_SUPPORTED;
}

NTSTATUS
EfiConInExReset (
    __in EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInExProtocol,
    __in BOOLEAN ExtendedVerification
    )

/*++

Routine Description:

    This routine is a wrapper for Extended Simple Text Input Protocol's Reset
    routine, which will reset input device hardware.

Arguments:

    ConInExProtocol - Supplies an interface for a console input device,
        returned from either OpenProtocol or provided in the EFI System Table.

    ExtendedVerification - Supplies a boolean to indicate that an EFI driver may
        perform a more exhaustive verification operation during device reset.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the device is not functioning correctly and
        could not be reset.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS ProtocolPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the console interface.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConInExProtocol, &ProtocolPa);
        ConInExProtocol =
            (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *)(UINTN)ProtocolPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConInEx.Reset call.
    //

    EfiStatus = ConInExProtocol->Reset(ConInExProtocol, ExtendedVerification);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConInExSetState (
    __in EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInExProtocol,
    __in EFI_KEY_TOGGLE_STATE *KeyToggleState
    )

/*++

Routine Description:

    This routine is a wrapper for the Extended Simple Text Input Protocol's
    SetState routine, which will set the toggle states of the keyboard. These
    include scroll lock, num lock, and caps lock.

Arguments:

    ConInExProtocol - Supplies an interface for a console input device returned
        from OpenProtocol.

    KeyToggleState - Supplies the toggle state of the keyboard to be set.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the keystroke information was not returned due
        to hardware errors.

    STATUS_NOT_SUPPORTED if the keyboard does not support setting states.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS KeyToggleStatePa;
    PHYSICAL_ADDRESS ProtocolPa = {0};


    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConInExProtocol, &ProtocolPa);
        ConInExProtocol =
            (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *)(UINTN)ProtocolPa.QuadPart;

        BlMmTranslateVirtualAddress(KeyToggleState, &KeyToggleStatePa);
        KeyToggleState =
            (EFI_KEY_TOGGLE_STATE *)(UINTN)KeyToggleStatePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConInEx.SetState call.
    //

    EfiStatus = ConInExProtocol->SetState(ConInExProtocol, KeyToggleState);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConInExUnregisterKeyNotify (
    __in EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInExProtocol,
    __in EFI_HANDLE NotificationHandle
    )

/*++

Routine Description:

    This routine is a wrapper for the Extended Simple Text Input Protocol's
    UnregisterKeyNotify routine, which will unregister a function from
    notification on key press.

Arguments:

    ConInExProtocol - Supplies an interface for a console input device from
        OpenProtocol.

    NotificationHandle - Supplies the handle assigned when the notification was
        registered.

Return Value:

    NT status code.

--*/

{

    UNREFERENCED_PARAMETER(ConInExProtocol);
    UNREFERENCED_PARAMETER(NotificationHandle);

    //
    // Callback notifications from UEFI are not supported by the boot
    // infrastructure.
    //

    return STATUS_NOT_SUPPORTED;
}

NTSTATUS
EfiConOutReset (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in BOOLEAN ExtendedVerification
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.Reset, which will reset
    the text output hardware.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    ExtendedVerification - Supplies a boolean to indicate that an EFI driver may
        perform a more exhaustive verification operation during device reset.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the device is not functioning correctly and
        could not be reset.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the console interface.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.Reset call.
    //

    EfiStatus = ConOutInterface->Reset(ConOutInterface, ExtendedVerification);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutOutputString (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in PWCHAR String
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.OutputString, which
    will write a unicode string to the output device.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    OutputString - Supplies a pointer to a NULL terminated unicode string to be
        displayed to the output device(s).

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the output device's mode is not currently in
        a defined textmode.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        output the text.

    STATUS_UNSUCCESSFUL if some of the characters in the unicode string
        could not be rendered and were skipped.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS StringPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(String, &StringPa);
        String = (PWCHAR)(UINTN)StringPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.OutputString call.
    //

    EfiStatus = ConOutInterface->OutputString(ConOutInterface, String);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutQueryMode (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in UINTN ModeNumber,
    __out UINTN *Columns,
    __out UINTN *Rows
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.QueryMode, which
    will return information for an available text mode that the output
    device(s) supports.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    ModeNumber - Supplies the supported mode number to query.

    Columns - Supplies a pointer to a variable that receives the number of
        columns in the supported mode.

    Rows - Supplies a pointer to a variable that recevies the number of rows in
        the supported mode.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the mode number was not valid or unsupported.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS ColumnsPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS RowsPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(Columns, &ColumnsPa);
        Columns = (UINTN *)(UINTN)ColumnsPa.QuadPart;
        BlMmTranslateVirtualAddress(Rows, &RowsPa);
        Rows = (UINTN *)(UINTN)RowsPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.QueryMode call.
    //

    EfiStatus = ConOutInterface->QueryMode(ConOutInterface,
                                           ModeNumber,
                                           Columns,
                                           Rows);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

VOID
EfiConOutReadCurrentMode (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __out SIMPLE_TEXT_OUTPUT_MODE *Mode
    )

/*++

Routine Description:

    This routine reads the contents of the output protocol's current mode.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    Mode - Supplies a pointer to a mode structure which receives the current
        text mode.

Return Value:

    None.

--*/

{

    PHYSICAL_ADDRESS ConOutInterfacePa = {0};
    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS ModePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &ConOutInterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)ConOutInterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(Mode, &ModePa);
        Mode = (SIMPLE_TEXT_OUTPUT_MODE *)(UINTN)ModePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Read the mode
    //

    RtlCopyMemory(Mode,
                  ConOutInterface->Mode,
                  sizeof(*Mode));

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return;
}

NTSTATUS
EfiConOutSetMode (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in UINTN ModeNumber
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.SetMode, which
    will set the output device(s) to a specified text mode.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    ModeNumber - Supplies the textmode to set.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the mode number was not valid or unsupported.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.SetMode call.
    //

    EfiStatus = ConOutInterface->SetMode(ConOutInterface, ModeNumber);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutSetAttribute (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in UINTN Attribute
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.SetAttribute, which
    will set the foreground and background colors for the OutputString and
    ClearScreen functions.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    Attribute - Supplies the attribute to set.

    An Efi Attribute word is defined as follows:
    #define EFI_TEXT_ATTR(foreground, background) \
            ((foreground | ((background) << 4)))

    The possible foreground colors are:

    #define EFI_BLACK         0x00
    #define EFI_BLUE          0x01
    #define EFI_GREEN         0x02
    #define EFI_CYAN          0x03
    #define EFI_RED           0x04
    #define EFI_MAGENTA       0x05
    #define EFI_BROWN         0x06
    #define EFI_LIGHTGRAY     0x07
    #define EFI_BRIGHT        0x08
    #define EFI_DARKGRAY      0x08
    #define EFI_LIGHTBLUE     0x09
    #define EFI_LIGHTGREEN    0x0A
    #define EFI_LIGHTCYAN     0x0B
    #define EFI_LIGHTRED      0x0C
    #define EFI_LIGHTMAGENTA  0x0D
    #define EFI_YELLOW        0x0E
    #define EFI_WHITE         0x0F

    The possible background colors are:

    #define EFI_BACKGROUND_BLACK     0x00
    #define EFI_BACKGROUND_BLUE      0x10
    #define EFI_BACKGROUND_GREEN     0x20
    #define EFI_BACKGROUND_CYAN      0x30
    #define EFI_BACKGROUND_RED       0x40
    #define EFI_BACKGROUND_MAGENTA   0x50
    #define EFI_BACKGROUND_BROWN     0x60
    #define EFI_BACKGROUND_LIGHTGRAY 0x70


Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the requested attribute is not defined.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.SetAttribute call.
    //

    EfiStatus = ConOutInterface->SetAttribute(ConOutInterface, Attribute);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutClearScreen (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.ClearScreen, which
    will clear the output device(s) display to the currently selected
    background color.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the output device is not in a valid textmode.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.ClearScreen call.
    //

    EfiStatus = ConOutInterface->ClearScreen(ConOutInterface);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutSetCursorPosition (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in UINTN Column,
    __in UINTN Row
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.SetCursorPosition, which
    will set the current coordinates of the cursor.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    Column - Supplies the column position to set the cursor to.  Must be greater
        than or equal to zero and less than the number of columns reported by
        QueryMode.

    Row - Supplies the row position to set the cursor to.  Must be greater than
        or equal to zero and less than the number of rows reported by QueryMode.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the output device is not in a valid textmode or
        the position is invalide in the current mode.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.SetCursorPosition call.
    //

    EfiStatus = ConOutInterface->SetCursorPosition(ConOutInterface,
                                                   Column,
                                                   Row);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiConOutEnableCursorPosition (
    __in SIMPLE_TEXT_OUTPUT_INTERFACE *ConOutInterface,
    __in BOOLEAN Visible
    )

/*++

Routine Description:

    This routine is a wrapper for SIMPLE_TEXT_OUTPUT.EnableCursor, which
    will make the cursor visibile or invisible.

Arguments:

    ConOutInterface - Supplies an interface for a console text output device,
        returned from either OpenProtocol or provided in the EFI System Table.

    Visibile - Supplies whether the cursor is set to be visible (TRUE) or
        invisible (FALSE).

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_SUPPORTED if the output device does not support visibility
        control of the cursor.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(ConOutInterface, &InterfacePa);
        ConOutInterface =
            (SIMPLE_TEXT_OUTPUT_INTERFACE *)(UINTN)InterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ConOut.EnableCursor call.
    //

    EfiStatus = ConOutInterface->EnableCursor(ConOutInterface, Visible);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUgaDrawGetMode (
    __in EFI_UGA_DRAW_PROTOCOL *UgaDrawInterface,
    __out UINT32 *HorizontalResolution,
    __out UINT32 *VerticalResolution,
    __out UINT32 *ColorDepth,
    __out UINT32 *RefreshRate
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_UGA_DRAW_PROTOCOL.GetMode, which
    will return information about the current mode.

Arguments:

    UgaDrawInterface - Supplies a pointer to the interface for a graphical
        console device, returned from OpenProtocol.

    HorizontalResolution - Supplies a pointer to a location that will be filled
        with the horizontal resolution of the current mode.

    VerticalResolution - Supplies a pointer to a location that will be filled
        with the vertical resolution of the current mode.

    ColorDepth - Supplies a pointer to the location that will be filled with
        the number of bits per pixel for the current mode.

    RefreshRate - Supplies a pointer to the location that will be filled with
        the refresh rate of the monitor in hertz.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if and of the arguements passed are NULL.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    PHYSICAL_ADDRESS ColorDepthPa = {0};
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS HorizontalResolutionPa = {0};
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS RefreshRatePa = {0};
    PHYSICAL_ADDRESS VerticalResolutionPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(UgaDrawInterface, &InterfacePa);
        UgaDrawInterface = (EFI_UGA_DRAW_PROTOCOL *)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(HorizontalResolution,
                                    &HorizontalResolutionPa);

        HorizontalResolution = (UINT32 *)(UINTN)HorizontalResolutionPa.QuadPart;
        BlMmTranslateVirtualAddress(VerticalResolution, &VerticalResolutionPa);
        VerticalResolution = (UINT32 *)(UINTN)VerticalResolutionPa.QuadPart;
        BlMmTranslateVirtualAddress(ColorDepth, &ColorDepthPa);
        ColorDepth = (UINT32 *)(UINTN)ColorDepthPa.QuadPart;
        BlMmTranslateVirtualAddress(RefreshRate, &RefreshRatePa);
        RefreshRate = (UINT32 *)(UINTN)RefreshRatePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = UgaDrawInterface->GetMode(UgaDrawInterface,
                                          HorizontalResolution,
                                          VerticalResolution,
                                          ColorDepth,
                                          RefreshRate);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUgaDrawSetMode (
    __in EFI_UGA_DRAW_PROTOCOL *UgaDrawInterface,
    __in UINT32 HorizontalResolution,
    __in UINT32 VerticalResolution,
    __in UINT32 ColorDepth,
    __in UINT32 RefreshRate
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_UGA_DRAW_PROTOCOL.SetMode, which
    will attempt to put the console in the requested mode.

    N.B. if successful the console will be cleared to black (Red = 0, Blue = 0,
    Green = 0).

Arguments:

    UgaDrawInterface - Supplies a pointer to the interface for a graphical
        console device, returned from OpenProtocol.

    HorizontalResolution - Supplies the horizontal resolution that the console
        should be set to.

    VerticalResolution - Supplies the vertical resolution that the console
        should be set to.

    ColorDepth - Supplies the number of bits per pixel that the console should
        be set to.

    RefreshRate - Supplies the refresh rate that the monitor should be set to
        (in hertz).

Return Value:

    STATUS_SUCCESS if the graphics mode was changed.

    STATUS_UNSUPPORTED if the requested settings are not supported.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(UgaDrawInterface, &InterfacePa);
        UgaDrawInterface = (EFI_UGA_DRAW_PROTOCOL *)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = UgaDrawInterface->SetMode(UgaDrawInterface,
                                          HorizontalResolution,
                                          VerticalResolution,
                                          ColorDepth,
                                          RefreshRate);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    Status = EfiGetNtStatusCode(EfiStatus);
    if (NT_SUCCESS(Status)) {
        BlStatusPrint(L"Console video mode set to %dx%dx (%d bpp)\r\n",
                      HorizontalResolution,
                      VerticalResolution,
                      ColorDepth);
    }

    return Status;
}

NTSTATUS
EfiUgaDrawBlt (
    __in EFI_UGA_DRAW_PROTOCOL *UgaDrawInterface,
    __inout_opt EFI_UGA_PIXEL *BltBuffer,
    __in EFI_UGA_BLT_OPERATION BltOperation,
    __in UINTN SourceX,
    __in UINTN SourceY,
    __in UINTN DestinationX,
    __in UINTN DestinationY,
    __in UINTN Width,
    __in UINTN Height,
    __in_opt UINTN Delta
)

/*++

Routine Description:

    This routine is a wrapper for EFI_UGA_DRAW_PROTOCOL.Blt which blits a
    rectangle of pixels onto the screen.

Arguments:

    UgaDrawInterface - Supplies a pointer to the interface for a graphical
        console device, returned from OpenProtocol.

    BltBuffer - Supplies the data to transfer to the graphics console. Size
        is at least Width*Height*sizeof(EFI_UGA_PIXEL)

    BltOperation - Supplies the type of blit that should occur. See
        EFI_UGA_BLT_OPERATION for more information.

    SourceX - Supplies the X coordinate of the source for the blit operation.

    SourceY - Supplies the Y coordinate of the source for the blit operation.

    DestinationX - Supplies the X coordinate of the Destination for the blit
        operation.

    DestinationY - Supplies the Y coordinate of the Destination for the blit
        operation.

    Width - Supplies the width of the blit rectangle in pixels.

    Height - Supplies the height of the blit rectangle in pixels.

    Delta - Not used for EfiUgaVideoFill or the EfiUgaVideoToVideo operation.
        If a Delta of zero is used, the entire BltBuffer is being operated on.
        If a subrectangle of the BltBuffer is being used then Delta represents
        the number of bytes in a row of the BltBuffer.


Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INVALID_PARAMETER if BltOperation is not valid.

    STATUS_IO_DEVICE_ERROR if the device reported an error while attempting to
        perform the request.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS BltBufferPa;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(UgaDrawInterface, &InterfacePa);
        UgaDrawInterface = (EFI_UGA_DRAW_PROTOCOL *)(UINTN)InterfacePa.QuadPart;
        if (ARGUMENT_PRESENT(BltBuffer) != FALSE) {
            BlMmTranslateVirtualAddress(BltBuffer, &BltBufferPa);
            BltBuffer = (EFI_UGA_PIXEL *)(UINTN)BltBufferPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = UgaDrawInterface->Blt(UgaDrawInterface,
                                      BltBuffer,
                                      BltOperation,
                                      SourceX,
                                      SourceY,
                                      DestinationX,
                                      DestinationY,
                                      Width,
                                      Height,
                                      Delta);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

UINT32
EfiGopGetModeCount (
    __in EFI_GRAPHICS_OUTPUT_PROTOCOL *GopInterface
    )

/*++

Routine Description:

    This function accesses the MaxMode member of the
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE member of the given
    EFI_GRAPHICS_OUTPUT_PROTOCOL.

Arguments:

    GopInterface - Supplies a pointer to the Gop protocol structures to be read.

Return Value:

    The maximum number of supported modes.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS GopInterfacePa = {0};
    UINT32 MaxMode;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(GopInterface, &GopInterfacePa);
        GopInterface =
            (EFI_GRAPHICS_OUTPUT_PROTOCOL *)(UINTN)GopInterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    MaxMode = GopInterface->Mode->MaxMode;

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return MaxMode;
}

VOID
EfiGopGetFrameBuffer (
    __in EFI_GRAPHICS_OUTPUT_PROTOCOL *GopInterface,
    __out PHYSICAL_ADDRESS *FrameBufferBase,
    __out UINTN *FrameBufferSize
    )

/*++

Routine Description:

    This function accesses the members of the EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
    member of the given EFI_GRAPHICS_OUTPUT_PROTOCOL.

Arguments:

    GopInterface - Supplies a pointer to the GOP protocol structure to be read.

    FrameBufferBase - Supplies a pointer to a variable that receives the
        base address of the frame buffer.

    FrameBufferSize - Supplies a pointer to a variable that receives the size
        of the frame buffer, in bytes.

Return Value:

    None.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS FrameBufferBasePa = {0};
    PHYSICAL_ADDRESS FrameBufferSizePa = {0};
    PHYSICAL_ADDRESS GopInterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(GopInterface, &GopInterfacePa);
        GopInterface =
            (EFI_GRAPHICS_OUTPUT_PROTOCOL *)(UINTN)GopInterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(FrameBufferBase, &FrameBufferBasePa);
        FrameBufferBase = (PVOID)(UINTN)FrameBufferBasePa.QuadPart;
        BlMmTranslateVirtualAddress(FrameBufferSize, &FrameBufferSizePa);
        FrameBufferSize = (UINTN *)(UINTN)FrameBufferSizePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    FrameBufferBase->QuadPart = GopInterface->Mode->FrameBufferBase;
    *FrameBufferSize = GopInterface->Mode->FrameBufferSize;

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return;
}

NTSTATUS
EfiGopGetCurrentMode (
    __in EFI_GRAPHICS_OUTPUT_PROTOCOL *GopInterface,
    __out UINT32 *Mode,
    __out EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Information
    )

/*++

Routine Description:

    Reads the Mode member and copies the structure pointed to by the
    information member of the GOP protocol.

Arguments:

    GopInterface - Supplies a pointer to the GOP protocol to be read.

    Mode - Supplies a pointer to a variable that receives the Mode.

    Information - Supplies a pointer to a variable that receives the
        information structure.

Return Value:

    None.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS GopInterfacePa;
    PHYSICAL_ADDRESS InformationPa;
    BOOLEAN IsMapped;
    PHYSICAL_ADDRESS ModePa;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        IsMapped = BlMmTranslateVirtualAddress(GopInterface, &GopInterfacePa);
        if (IsMapped == FALSE) {
            Status = STATUS_UNSUCCESSFUL;
            goto GopGetCurrentModeExit;
        }

        GopInterface =
            (EFI_GRAPHICS_OUTPUT_PROTOCOL *)(UINTN)GopInterfacePa.QuadPart;

        IsMapped = BlMmTranslateVirtualAddress(Mode, &ModePa);
        if (IsMapped == FALSE) {
            Status = STATUS_UNSUCCESSFUL;
            goto GopGetCurrentModeExit;
        }

        Mode = (UINT32 *)(UINTN)ModePa.QuadPart;
        IsMapped = BlMmTranslateVirtualAddress(Information, &InformationPa);
        if (IsMapped == FALSE) {
            Status = STATUS_UNSUCCESSFUL;
            goto GopGetCurrentModeExit;
        }

        Information = (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *)
                      (UINTN)InformationPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    *Mode = GopInterface->Mode->Mode;
    RtlCopyMemory(Information, GopInterface->Mode->Info, sizeof(*Information));

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

GopGetCurrentModeExit:
    return Status;
}

NTSTATUS
EfiGopQueryMode (
    __in EFI_GRAPHICS_OUTPUT_PROTOCOL *GopInterface,
    __in UINT32 Mode,
    __out EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Information
    )

/*++

Routine Description:

    Wrapper for the GOP QueryMode call.

Arguments:

    GopInterface - Supplies a pointer to the Gop protocol structures to be read.

    ModeNumber - Supplies the mode number to be queried.

    Information - Supplies a pointer to a variable that receives the mode.

Return Value:

    STATUS_SUCCESS if all modes are read.

    STATUS_IO_DEVICE_ERROR if the firmware calls return an error.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS GopInterfacePa = {0};
    PHYSICAL_ADDRESS InformationPa = {0};
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *InformationPtr;
    UINTN Size;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(GopInterface, &GopInterfacePa);
        GopInterface =
            (EFI_GRAPHICS_OUTPUT_PROTOCOL *)(UINTN)GopInterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(Information, &InformationPa);
        Information = (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *)
                      (UINTN)InformationPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = GopInterface->QueryMode(GopInterface,
                                        Mode,
                                        &Size,
                                        &InformationPtr);

    if (EfiStatus == EFI_SUCCESS) {
        RtlCopyMemory(Information, InformationPtr, sizeof(*Information));
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiGopSetMode (
    __in EFI_GRAPHICS_OUTPUT_PROTOCOL *GopInterface,
    __in UINT32 ModeNumber
    )

/*++

Routine Description:

    Wrapper for the GOP SetMode call.

Arguments:

    GopInterface - Supplies a pointer to the Gop protocol structures to be read.

    ModeNumber - Supplies an index in the range [0, EfiGopGetModeCount()).

Return Value:

    STATUS_SUCCESS if the mode was successfully changed.

    STATUS_IO_DEVICE_ERROR if the firmware calls return an error.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS GopInterfacePa = {0};
    BOOLEAN ModeChangeAttempted;
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(GopInterface, &GopInterfacePa);
        GopInterface =
            (EFI_GRAPHICS_OUTPUT_PROTOCOL *)(UINTN)GopInterfacePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Check the requested mode with the current mode to see if a change is
    // really necessary.
    //

    if (ModeNumber != GopInterface->Mode->Mode) {
        EfiStatus = GopInterface->SetMode(GopInterface, ModeNumber);
        ModeChangeAttempted = TRUE;

    } else {
        EfiStatus = EFI_SUCCESS;
        ModeChangeAttempted = FALSE;
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    Status = EfiGetNtStatusCode(EfiStatus);
    if ((ModeChangeAttempted != FALSE) && NT_SUCCESS(Status)) {
        BlStatusPrint(L"Console video mode set to 0x%x\r\n", ModeNumber);
    }

    return Status;
}

