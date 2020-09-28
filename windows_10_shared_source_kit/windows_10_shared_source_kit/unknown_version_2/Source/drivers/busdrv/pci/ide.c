/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ide.c

Abstract:

    This module contains functions for dealing with IDE controllers.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciNativeIdeInterface_Constructor;

VOID
PciNativeIdeInterface_Reference(
    __inout PVOID Context
    );

VOID
PciNativeIdeInterface_Dereference(
    __inout PVOID Context
    );

VOID
PciNativeIdeInterface_InterruptControl(
    __inout PVOID Context,
    __in BOOLEAN Enable
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciNativeIdeInterface_Constructor)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE PciNativeIdeInterface = {
    &GUID_PCI_NATIVE_IDE_INTERFACE,         // InterfaceType
    sizeof(PCI_NATIVE_IDE_INTERFACE),       // MinSize
    PCI_NATIVE_IDE_INTERFACE_VERSION,       // MinVersion
    PCI_NATIVE_IDE_INTERFACE_VERSION,       // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_NativeIde,                 // Signature
    PciNativeIdeInterface_Constructor       // Constructor
};

// ------------------------------------------------------------------- Functions

BOOLEAN
PciConfigureIdeController(
    __inout PPCI_DEVICE Device,
    __in BOOLEAN DisableIoDecode
    )
/*++

Routine Description:

    If this is an IDE contoller that can be switched to native mode
    and its not already there, we change the programming interface
    (yes PCI 2.x does say its read only) and check if it sticks.

Arguments:

    Device - the IDE controller to be switched

    DisableIoDecode - If TRUE indicates that we should disable the
        I/O decode of this IDE controller, regardless of whether
        we have to switch it or not.

Return Value:

    TRUE if the result is that the controller ends up in native mode
    FALSE otherwise

Note:

    We support three styles of PCI IDE controller:
         - Compatible mode controllers that consume 2 ISA interrupts
           and decode fixed legacy resources, together with an optional
           relocateable bus master register
         - Native mode controller which uses all 5 bars and the PCI
           interrupt for both channels
         - Controllers which can be switched between modes.
    We do NOT support running one channel in native mode and one in
    compatible mode.

--*/
{
    BOOLEAN primaryChangeable, secondaryChangeable;
    BOOLEAN primaryNative, secondaryNative;
    PCI_CONFIG_HEADER configHeader;
    UCHAR progIf, tempProgIf;
    BOOLEAN native;
        
    NON_PAGED_CODE();

    primaryChangeable = BITS_SET(Device->ProgIf, PCI_IDE_PRIMARY_MODE_CHANGEABLE);
    secondaryChangeable = BITS_SET(Device->ProgIf, PCI_IDE_SECONDARY_MODE_CHANGEABLE);
    primaryNative = BITS_SET(Device->ProgIf, PCI_IDE_PRIMARY_NATIVE_MODE);
    secondaryNative = BITS_SET(Device->ProgIf, PCI_IDE_SECONDARY_NATIVE_MODE);

    //
    // Do a sanity check on the programming interface bits.
    // Don't touch controllers we don't support - leave ATAPI to deal with it!
    //
    if ((primaryNative != secondaryNative) ||  
        (primaryChangeable != secondaryChangeable)) {

        WARN("Warning unsupported IDE controller configuration for VEN_%04x&DEV_%04x!",
             Device->VendorID,
             Device->DeviceID
             );

        return FALSE;
    } 
    
    if (primaryNative == TRUE) {
        if (DisableIoDecode == TRUE) {

            //
            // For a fully native mode controller turn off the IO decode.
            // In recent controllers MSFT has requested that this prevent
            // the PCI interrupt from being asserted to close a race condition
            // that can occur if an IDE device interrupts before the IDE driver
            // has been loaded on the PCI device.  This is not a issue for
            // compatible mode controllers because they use edge triggered
            // interrupts that can be dismissed as spurious at the interrupt
            // controller, unlike the shared, level triggered,  PCI interrups
            // of native mode.
            //
            // Once loaded and having connected its interrupt the IDE driver
            // will renable IO space access.
            //
            // We only do this during the initial bus scan or if the IDE driver
            // has requested it through the PCI_NATIVE_IDE_INTERFACE.  This is
            // to avoid not enabling IoSpace for 3rd party native IDE controllers
            // with their own drivers.
            //

            PciDisconnectDevice(Device);
        }

        return TRUE;
    } 
    
    native = FALSE;
    if (primaryChangeable && Device->NativeIdeSwitchAllowed) {

        //
        // If we aren't already in native mode, the controller can change modes
        // and the bios is ammenable then do so...
        //
        PciDisconnectDevice(Device);
        
        progIf = Device->ProgIf | (PCI_IDE_PRIMARY_NATIVE_MODE
                                   | PCI_IDE_SECONDARY_NATIVE_MODE);

        PciWriteConfigRegister(Device, ProgIf, &progIf);

        //
        // Check if it stuck
        //
        PciReadConfigRegister(Device, ProgIf, &tempProgIf);

        if (tempProgIf == progIf) {
            
            //
            // If it stuck, remember we did this
            //
            Device->ProgIf = progIf;
            
            //
            // Zero the first 4 bars in the config space because they might have
            // bogus values in them.  This will get them back to their defaults.
            //
            RtlZeroMemory(configHeader.type0.BaseAddresses,
                          4 * sizeof(configHeader.type0.BaseAddresses[0])
                          );

            PciWriteDeviceConfig(Device,
                                 configHeader.type0.BaseAddresses,
                                 CONFIG_FIELD_OFFSET(type0.BaseAddresses),
                                 4 * sizeof(configHeader.type0.BaseAddresses[0])
                                 );

            native = TRUE;

        } else {

            WARN("Warning failed switch to native mode for IDE controller VEN_%04x&DEV_%04x!",
                 Device->VendorID,
                 Device->DeviceID
                 );
        }        
    }

    return native;
}

// --------------------------------------------------------------- Native IDE Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciNativeIdeInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the PCI_NATIVE_IDE_INTERFACE fields.

Arguments:

    DeviceExtension - Extension of the device
    
    InterfaceSpecificData - from the QUERY_INTERFACE irp

    Version - Version of the interface requested
    
    Size - Size of the buffer 
    
    InterfaceReturn - Buffer to return the interface in

    Filtered - Not supported for this interface.
 
Return Value:

    Status

--*/
{
    PPCI_NATIVE_IDE_INTERFACE interface;
    PPCI_DEVICE device;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PCI_NATIVE_IDE_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    device = (PPCI_DEVICE)DeviceExtension;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    if (!PCI_NATIVE_CAPABLE_IDE_CONTROLLER(device)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    interface = (PPCI_NATIVE_IDE_INTERFACE)InterfaceReturn;
    
    interface->Size = sizeof(PCI_NATIVE_IDE_INTERFACE);
    interface->Context = DeviceExtension;
    interface->Version = PCI_NATIVE_IDE_INTERFACE_VERSION;
    interface->InterfaceReference = PciNativeIdeInterface_Reference;
    interface->InterfaceDereference = PciNativeIdeInterface_Dereference;

    interface->InterruptControl = PciNativeIdeInterface_InterruptControl;
    
    return STATUS_SUCCESS;
}

VOID
PciNativeIdeInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedIncrement(&device->PciNativeIdeInterfaceCount);

    return;
}

VOID
PciNativeIdeInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedDecrement(&device->PciNativeIdeInterfaceCount);

    return;
}

VOID
PciNativeIdeInterface_InterruptControl(
    __inout PVOID Context,
    __in BOOLEAN Enable
    )
/*++

Routine Description:

    Controls the enabling and disabling of native mode PCI IDE controllers
    IoSpaceEnable bits which on some controllers (currently Intel ICH3)
    will mask off interrupt generation and this prevent the system from 
    crashing...
    
Arguments:

    Context - Context from the PCI_NATIVE_IDE_INTERFACE
    
    Enable - If TRUE then set the IoSpaceEnable bit in the command register,
             otherwise disable it.


Return Value:

    None - if this operation fails we have aleady bugchecked in the PCI driver

    N.B. This function is called from with an ISR and this must be callable at
    DEVICE_LEVEL

--*/
{
    PPCI_DEVICE device;
    USHORT command;
    
    NON_PAGED_CODE();
    
    device = (PPCI_DEVICE)Context;

    //
    // Remember we gave the IDE driver control of this
    //
    PCI_DEVICE_SET_FLAG(device, IdeInterfaceActive);

    PciReadConfigRegister(device, Command, &command);

    if (Enable) {
        command |= PCI_ENABLE_IO_SPACE;
        device->PossibleDecodes |= PCI_ENABLE_IO_SPACE;
    } else {
        command &= ~PCI_ENABLE_IO_SPACE;
        device->PossibleDecodes &= ~PCI_ENABLE_IO_SPACE; 
    }

    PciWriteConfigRegister(device, Command, &command);
}
