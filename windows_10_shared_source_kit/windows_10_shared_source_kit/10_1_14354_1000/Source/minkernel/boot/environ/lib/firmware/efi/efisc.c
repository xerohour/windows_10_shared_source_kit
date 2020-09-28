/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efisc.c

Abstract:

    This module contains the wrapper functions to call the EFI BIOS for
    doing Security Commands with the storage device.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
EfiScpReceiveData (
    __in EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *Interface,
    __in UINT32 MediaId,
    __in UINT64 Timeout,
    __in UINT8 SecurityProtocol,
    __in UINT16 SecurityProtocolSpecificData,
    __in UINTN PayloadBufferSize,
    __out VOID *PayloadBuffer,
    __out UINTN *PayloadTransferSize
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_STORAGE_SECURITY_RECEIVE_DATA
    (aka TrustedReceive) which will return the result and data for
    the previous EFI_STORAGE_SECURITY_SEND_DATA call.

Arguments:

    Interface - Supplies a pointer to the protocol interface structure.

    MediaId - Supplies the ID of the medium to receive data from.

    Timeout - Supplies the time, in 100ns units, for the execution of the
        security protocol command. A Timeout value of 0 means this function
        will wait indefinitely for the security protocol command to execute.
        If Timeout is greater than zero, this function will return EFI_TIMEOUT
        if the time required to execute the receive data command is greater
        than Timeout.

    SecurityProtocol - Supplies the value of the "Security Protocol" parameter
        of the security protocol command to be sent.

    SecurityProtocolSpecificData - Supplies the value of the "Security Protocol
        Specific" parameter of the security protocol command to be sent.

    PayloadBufferSize - Supplies the size, in bytes, of the payload data buffer.

    PayloadBuffer - Supplies the pointer to the destination buffer to store the
        security protocol command specific payload data for the security
        protocol command. The caller is responsible for having either implicit
        or explicit ownership of the buffer.

    PayloadTransferSize - Supplies a pointer to a variable to store the size,
        in bytes, of the data written to the payload data buffer.

Return Value:

    NT Status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS PayloadBufferPa = {0};
    PHYSICAL_ADDRESS PayloadTransferSizePa = {0};

    //
    // If the application is running within the application execution
    // context, switch it to firmware execution context for making
    // firmware call. Translate all virtual addresses to their
    // respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Interface, &InterfacePa);
        Interface = (EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *)
                    (UINTN)InterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(PayloadBuffer, &PayloadBufferPa);
        PayloadBuffer = (VOID *)(UINTN)PayloadBufferPa.QuadPart;

        BlMmTranslateVirtualAddress(PayloadTransferSize,
                                    &PayloadTransferSizePa);

        PayloadTransferSize = (UINTN *)(UINTN)PayloadTransferSizePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = Interface->ReceiveData(Interface,
                                       MediaId,
                                       Timeout,
                                       SecurityProtocol,
                                       SecurityProtocolSpecificData,
                                       PayloadBufferSize,
                                       PayloadBuffer,
                                       PayloadTransferSize);

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiScpSendData (
    __in EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *Interface,
    __in UINT32 MediaId,
    __in UINT64 Timeout,
    __in UINT8 SecurityProtocol,
    __in UINT16 SecurityProtocolSpecificData,
    __in UINTN PayloadBufferSize,
    __in VOID *PayloadBuffer
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_STORAGE_SECURITY_SEND_DATA
    (aka TrustedSend) which will send the payload to the device
    using Security Command Protocol.

Arguments:

    Interface - Supplies a pointer to the protocol interface structure.

    MediaId - Supplies the ID of the medium to send data to.

    Timeout - Supplies the time, in 100ns units, for the execution of the
        security protocol command. A Timeout value of 0 means this function
        will wait indefinitely for the security protocol command to execute.
        If Timeout is greater than zero, this function will return EFI_TIMEOUT
        if the time required to execute the send data command is greater than
        Timeout.

    SecurityProtol - Supplies the value of the "Security Protocol" parameter
        of the security protocol command to be sent.

    SecurityProtocolSpecificData - Supplies the value of the "Security
        Protocol Specific" parameter of the security protocol command to be
        sent.

    PayloadBufferSize - Supplies the size, in bytes, of the payload data buffer.

    PayloadBuffer - Supplies a pointer to the buffer containing the security
        protocol command specific payload data for the security protocol
        command.

Return Value:

    NT Status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS PayloadBufferPa = {0};

    //
    // If the application is running within the application execution
    // context, switch it to firmware execution context for making
    // firmware call. Translate all virtual addresses to their
    // respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Interface, &InterfacePa);
        Interface = (EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *)
                    (UINTN)InterfacePa.QuadPart;

        BlMmTranslateVirtualAddress(PayloadBuffer, &PayloadBufferPa);
        PayloadBuffer = (VOID *)(UINTN)PayloadBufferPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = Interface->SendData(Interface,
                                    MediaId,
                                    Timeout,
                                    SecurityProtocol,
                                    SecurityProtocolSpecificData,
                                    PayloadBufferSize,
                                    PayloadBuffer);

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}


