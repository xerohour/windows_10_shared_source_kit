/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efipci.c

Abstract:

    Wrappers for EFI PCI Root Bridge I/O Protocol routines.

    The PCI Root Bridge I/O Protocol provides functional support to read and
    write PCI device resources beneath a root bus.  This module provides
    wrappers for these functions to correctly handle cases such as whether the
    caller is in virtual mode, physical mode, etc.

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiPciAccessConfigSpace (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *RootBridgeInterface,
    __in EFI_PCI_IO_PROTOCOL_WIDTH Width,
    __in UINT64 Address,
    __in UINTN Count,
    __inout PVOID Buffer,
    __in BOOLEAN ReadAccess
    )

/*++

Routine Description:

    This routine accesses PCI configuration space of a device using the given
    PCI Root Bridge I/O Interface.

Arguments:

    RootBridgeInterface - Supplies a pointer to the PCI Root Bridge I/O
        Interface to use.

    Width - Supplies the width and access type to use.  This dictates whether
        the address is considered to be a hardware-fill FIFO or a buffer, as
        well as whether the address is to be accessed in 1, 2, 4 or 8 byte
        chunks.

    Address - Supplies the PCI configuration space address to access.  This is
        constructed with the following bit fields:

            Register (low byte) -   0-7
            Function Number -       7-15
            Device Number -         16-23
            Bus Number -            24-31
            Register (remaining) -  32-64

        The segment number of the access is fixed by the interface in use.

    Count - Supplies the number of transactions to issue.  The total number of
        bytes accessed will be this value multiplied by the width indicated
        above.

    Buffer - Supplies the buffer to use to transfer data to or from the device.

    ReadAccess - Supplies a boolean indicating whether the access is a read
        or write access.

Return Value:

    NT Status code.

    STATUS_SUCCESS if the access completes successfully.

    STATUS_INVALID_PARAMETER if the width parameter or address field are out
        of range for this root bus.

    STATUS_INSUFFICIENT_RESOURCES if the firmware encounters a resource failure.

--*/

{

    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM AccessRoutine;
    PHYSICAL_ADDRESS BufferPa;
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
        BlMmTranslateVirtualAddress(RootBridgeInterface, &InterfacePa);
        RootBridgeInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(UINTN)BufferPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    if (ReadAccess != FALSE) {
        AccessRoutine = RootBridgeInterface->Pci.Read;

    } else {
        AccessRoutine = RootBridgeInterface->Pci.Write;
    }

    EfiStatus = AccessRoutine(RootBridgeInterface,
                              Width,
                              Address,
                              Count,
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
EfiPciGetConfiguration (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *RootBridgeInterface,
    __out PPHYSICAL_ADDRESS Resources
    )

/*++

Routine Description:

    This routine retrieves the resource configuration from the root bus that
    supports the given PCI Root Bridge I/O Interface.

Arguments:

    RootBridgeInterface - Supplies the PCI Root Bridge I/O Interface for which
        the resource configuration is to be retrieved.

    Resources - Supplies a pointer to a variable that receives the physical
        address of the resource configuration buffer.  This physical address is
        to BIOS-internal memory and must be treated as read-only.  The resource
        buffer is an array of ACPI resource descriptors terminated by an end
        tag.  The valid length of the buffer can only be determined by walking
        it.

        N.B. If a caller of this routine is executing with virtual address
             translation enabled, it must either revert to physical mode to
             walk the buffer for the purposes of determining its length or
             carefully map the buffer speculatively.

Return Value:

--*/

{

    PVOID Buffer;
    PVOID *BufferPointer;
    PHYSICAL_ADDRESS BufferPointerPa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa;
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    Buffer = NULL;
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(RootBridgeInterface, &InterfacePa);
        RootBridgeInterface = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(&Buffer, &BufferPointerPa);
        BufferPointer = (PVOID *)(UINTN)BufferPointerPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);

    } else {
        BufferPointer = &Buffer;
    }

    //
    // Call the interface configuration routine to get the resources decoded
    // by the root bus exposing this interface.
    //

    EfiStatus = RootBridgeInterface->Configuration(RootBridgeInterface,
                                                   BufferPointer);

    Status = EfiGetNtStatusCode(EfiStatus);

    //
    // The configuration routine populates the buffer pointer with a physical
    // mode pointer to the resource configuration.
    //

    if (NT_SUCCESS(Status)) {
        Resources->QuadPart = (ULONG_PTR)Buffer;
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return Status;
}

