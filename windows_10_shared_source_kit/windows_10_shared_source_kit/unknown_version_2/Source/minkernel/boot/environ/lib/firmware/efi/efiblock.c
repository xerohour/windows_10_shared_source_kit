/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiblock.c

Abstract:

    Wrappers for EFI Block I/O Protocol routines.

    The BLOCK_IO_PROTOCOL interface provides functional support to read and
    write to a hardware device.  This module provides wrappers for these
    functions to correctly handle cases such as whether the caller is in
    virtual mode, physical mode, etc.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiBlockIoReadBlocks (
    __in EFI_BLOCK_IO *BlockIoInterface,
    __in ULONGLONG BlockNumber,
    __in UINTN NumberBlocks,
    __out VOID *Buffer
    )

/*++

Routine Description:

    Wrapper for an EFI Block I/O Interface call to read from a block i/o
    device.

Arguments:

    BlockIoInterface - Block I/O device's interface. (This is a physical
        address pointer).

    BlockNumber - LBA on device to start read.

    NumberBlock - The number of blocks to read.

    Buffer - Pointer to address to store contents read from device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.
    STATUS_NO_MEDIA if there is no media in the device.
    STATUS_INVALID_PARAMETER if the request block number is not valid.
    STATUS_MEDIA_CHANGED if the underlying media has changed.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    ULONG BlockSize;
    PHYSICAL_ADDRESS BufferPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(BlockIoInterface, &InterfacePa);
        BlockIoInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(UINTN)BufferPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the Block I/O devices ReadBlocks implementation to read the physical
    // blocks from the device.
    //

    BlockSize = BlockIoInterface->Media->BlockSize;
    EfiStatus = BlockIoInterface->ReadBlocks(BlockIoInterface,
                                             BlockIoInterface->Media->MediaId,
                                             BlockNumber,
                                             NumberBlocks * BlockSize,
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
EfiBlockIoWriteBlocks (
    __in EFI_BLOCK_IO *BlockIoInterface,
    __in ULONGLONG BlockNumber,
    __in UINTN NumberBlocks,
    __in VOID *Buffer
    )

/*++

Routine Description:

    Wrapper for an EFI Block I/O Interface call to write to a block i/o
    device.

Arguments:

    BlockIoInterface - Block I/O device's interface.  (This is a physical
        address pointer).

    BlockNumber - LBA on device to start write.

    NumberBlock - The number of blocks to write.

    Buffer - Pointer to address with contents to write to device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.
    STATUS_NO_MEDIA if there is no media in the device.
    STATUS_INVALID_PARAMETER if the request block number is not valid.
    STATUS_MEDIA_CHANGED if the underlying media has changed.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    ULONG BlockSize;
    PHYSICAL_ADDRESS BufferPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(BlockIoInterface, &InterfacePa);
        BlockIoInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(UINTN)BufferPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the Block I/O devices WriteBlocks implementation to write the
    // physical blocks to the device.
    //

    BlockSize = BlockIoInterface->Media->BlockSize;
    EfiStatus = BlockIoInterface->WriteBlocks(BlockIoInterface,
                                              BlockIoInterface->Media->MediaId,
                                              BlockNumber,
                                              NumberBlocks * BlockSize,
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
EfiBlockIoReset (
    __in EFI_BLOCK_IO *BlockIoInterface,
    __in BOOLEAN ExtendedVerification
    )

/*++

Routine Description:

    Wrapper for an EFI Block I/O Interface call to reset a block i/o
    device.

Arguments:

    BlockIoInterface - Block I/O device's interface.  (This is a physical
        address pointer).

    ExtendedVerification  - Indicates that the driver may perform a more
        exhaustive verification operation of the device during reset.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.

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
        BlMmTranslateVirtualAddress(BlockIoInterface, &InterfacePa);
        BlockIoInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the Block I/O devices Reset implementation to reset the device.
    //

    EfiStatus = BlockIoInterface->Reset(BlockIoInterface,
                                        ExtendedVerification);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiBlockIoFlushBlocks (
    __in EFI_BLOCK_IO *BlockIoInterface
    )

/*++

Routine Description:

    Wrapper for an EFI Block I/O Interface call to flush a block i/o
    device.

Arguments:

    BlockIoInterface - Block I/O device's interface.  (This is a physical
        address pointer).

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        operation.
    STATUS_NO_MEDIA if there is no media in the device.

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
        BlMmTranslateVirtualAddress(BlockIoInterface, &InterfacePa);
        BlockIoInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Use the Block I/O devices Flush implementation to flush pending
    // writes to the device.
    //

    EfiStatus = BlockIoInterface->FlushBlocks(BlockIoInterface);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}


