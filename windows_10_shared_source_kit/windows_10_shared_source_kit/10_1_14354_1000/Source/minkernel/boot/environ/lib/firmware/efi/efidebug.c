/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efidebug.c

Abstract:

    Wrappers for EFI Debug Port routines.

    All efi call must be made in physical mode.  When a boot environment
    application runs in virtual mode, pointers need to be modified and fixed
    before every EFI call.  Additionally firmware pointers/addresses need to be
    virtually mapped for the caller to read contents.  The wrappers intend to
    hide as much of that work as possible from the caller.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"

//
// -------------------------------------------------------- Debug Port Services
//

NTSTATUS
EfiDebugPortReset (
    __in EFI_DEBUGPORT *DebugPortInterface
    )

/*++

Routine Description:

    Wrapper for an EFI Debug Port Interface call to reset the debug port.

Arguments:

    DebugPortInterface - Debug port interface. (This is a physical
        address pointer).

Return Value:

    STATUS_SUCCESS when successful.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(DebugPortInterface, &InterfacePa);
        DebugPortInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the debug port protocol Reset implementation to reset debug port.
    //

    EfiStatus = DebugPortInterface->Reset(DebugPortInterface);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiDebugPortWrite (
    __in EFI_DEBUGPORT *DebugPortInterface,
    __in UINT32 Timeout,
    __inout UINTN *BufferSize,
    __in VOID *Buffer
    )

/*++

Routine Description:

    Wrapper for an EFI Debug Port Interface call to write to debug port.

Arguments:

    DebugPortInterface - Debug port interface. (This is a physical
        address pointer).

    Timeout - timeout of the write.

    BufferSize - The size of the buffer.

    Buffer - Pointer to address to store contents write to debug port.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.
    STATUS_TIMEOUT if the device reported timeout.

--*/

{

    PHYSICAL_ADDRESS BufferPa;
    PHYSICAL_ADDRESS BufferSizePa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(DebugPortInterface, &InterfacePa);
        DebugPortInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(UINTN)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (PVOID)(UINTN)BufferSizePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the debug port protocol Write implementation to write the data
    // to debug port.
    //

    EfiStatus = DebugPortInterface->Write(DebugPortInterface,
                                         Timeout,
                                         BufferSize,
                                         Buffer);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiDebugPortRead (
    __in EFI_DEBUGPORT *DebugPortInterface,
    __in UINT32 Timeout,
    __inout UINTN *BufferSize,
    __out VOID *Buffer
    )

/*++

Routine Description:

    Wrapper for an EFI Debug Port Interface call to read from debug port.

Arguments:

    DebugPortInterface - Debug port interface. (This is a physical
        address pointer).

    Timeout - timeout of the read.

    BufferSize - The size of the buffer.

    Buffer - Pointer to address to store contents read from debug port.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.
    STATUS_TIMEOUT if the device reported timeout.

--*/

{

    PHYSICAL_ADDRESS BufferPa;
    PHYSICAL_ADDRESS BufferSizePa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(DebugPortInterface, &InterfacePa);
        DebugPortInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(UINTN)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (PVOID)(UINTN)BufferSizePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the debug port protocol Read implementation to read the data
    // from debug port.
    //

    EfiStatus = DebugPortInterface->Read(DebugPortInterface,
                                         Timeout,
                                         BufferSize,
                                         Buffer);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiDebugPortPoll (
    __in EFI_DEBUGPORT *DebugPortInterface
    )

/*++

Routine Description:

    Wrapper for an EFI Debug Port Interface call to test if new data is available
    on debug port.

Arguments:

    DebugPortInterface - Debug port interface. (This is a physical
        address pointer).

Return Value:

    STATUS_SUCCESS when new data is available.
    STATUS_NOT_FOUND when there is no new data available.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(DebugPortInterface, &InterfacePa);
        DebugPortInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the debug port protocol Poll implementation to query
    // data availability on debug port.
    //

    EfiStatus = DebugPortInterface->Poll(DebugPortInterface);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}


