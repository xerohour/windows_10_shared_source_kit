/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    config.c

Abstract:

    Two kinds of config space access are allowed.  One for the config space
    associated with a specific PDO and one for a device specified in terms of
    a (RootFdo, BusNumber, Slot) tuple.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "config.tmh"

// --------------------------------------------------------------------- Defines

#define BUSINTRF_VERSION 1
#define AGPINTRF_VERSION 1
#define VIRTINTRF_VERSION 1

// ----------------------------------------------------------------------- Types

typedef struct _PCI_LOCATION {
    union {
        struct {
            ULONG FunctionNumber:3;
            ULONG DeviceNumber:5;
            ULONG BusNumber:16;
            ULONG Reserved:8;
        } bits;
        ULONG   AsULONG;
    } u;
} PCI_LOCATION,  *PPCI_LOCATION;

// ------------------------------------------------------------------ Prototypes

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciBusInterface_Constructor;

VOID
PciBusInterface_Reference(
    __inout PVOID Context
    );

VOID
PciBusInterface_Dereference(
    __inout PVOID Context
    );

__drv_functionClass(TRANSLATE_BUS_ADDRESS)
_IRQL_requires_same_
TRANSLATE_BUS_ADDRESS PciBusInterface_TranslateBusAddress;

__drv_functionClass(GET_DMA_ADAPTER)
_IRQL_requires_same_
GET_DMA_ADAPTER PciBusInterface_GetDmaAdapter;

__drv_functionClass(GET_SET_DEVICE_DATA)
_IRQL_requires_same_
GET_SET_DEVICE_DATA PciBusInterface_GetBusData;

__drv_functionClass(GET_SET_DEVICE_DATA)
_IRQL_requires_same_
GET_SET_DEVICE_DATA PciBusInterface_SetBusData;

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciAgpTargetInterface_Constructor;

VOID
PciAgpTargetInterface_Reference(
    __inout PVOID Context
    );

VOID
PciAgpTargetInterface_Dereference(
    __inout PVOID Context
    );

// --------------------------------------------------------------------- Pragmas

#pragma prefast(disable: __WARNING_USE_OTHER_FUNCTION, "bugchecks")

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE,    PciBusInterface_Constructor)
    #pragma alloc_text(PAGE,    PciBusInterface_TranslateBusAddress)
    #pragma alloc_text(PAGE,    PciBusInterface_GetDmaAdapter)
    #pragma alloc_text(PAGE,    PciAgpTargetInterface_Constructor)
    #pragma alloc_text(PAGE,    PciAgpTargetInterface_Reference)
    #pragma alloc_text(PAGE,    PciAgpTargetInterface_Dereference)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE PciBusHandlerInterface = {
    &GUID_BUS_INTERFACE_STANDARD,           // InterfaceType
    sizeof(BUS_INTERFACE_STANDARD),         // MinSize
    BUSINTRF_VERSION,                       // MinVersion
    BUSINTRF_VERSION,                       // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_BusHandler,                // Signature
    PciBusInterface_Constructor             // Constructor
};

const PCI_INTERFACE PciAgpTargetInterface = {
    &GUID_AGP_TARGET_BUS_INTERFACE_STANDARD,   // InterfaceType
    sizeof(AGP_TARGET_BUS_INTERFACE_STANDARD), // MinSize
    AGPINTRF_VERSION,                          // MinVersion
    AGPINTRF_VERSION,                          // MaxVersion
    PCIIF_PDO,                                 // Flags
    PciInterface_AgpTarget,                    // Signature
    PciAgpTargetInterface_Constructor          // Constructor
};

UCHAR PciConfigDoubleBuffer[0x1000];
KEVENT PciConfigDoubleBufferLock;

// ---------------------------------------------------------- Config Access APIs

VOID
PciGetConfigSlotAndBus(
    __in  PPCI_DEVICE      Device,
    __in  USHORT           FunctionOffset,
    __out PPCI_SLOT_NUMBER Slot,
    __out PULONG           BusNumber
    )
/*++

Routine Description:

    Calculate the slot for a device or virtual function within the device.

Arguments:

    Device - The Device representing the device who's config space we want

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    Slot - Pointer to location that will be filled with the slot number to
        use.

    BusNumber - Pointer to location that will contain the bus number to use

Return Value:

    None

--*/
{
    PCI_LOCATION location;

    NON_PAGED_CODE();

    //
    // Copy the slot fields to from the WDM layout to the hardware layout
    //
    location.u.AsULONG = 0;
    location.u.bits.FunctionNumber = Device->Slot.u.bits.FunctionNumber;
    location.u.bits.DeviceNumber = Device->Slot.u.bits.DeviceNumber;
    location.u.bits.BusNumber = (USHORT)Device->BusNumber;

    //
    // Now the function offset can be added
    //
    location.u.AsULONG += FunctionOffset;

    //
    // Copy back to WDM layout
    //
    Slot->u.AsULONG = 0;
    Slot->u.bits.FunctionNumber = location.u.bits.FunctionNumber;
    Slot->u.bits.DeviceNumber = location.u.bits.DeviceNumber;

    *BusNumber = location.u.bits.BusNumber;

    //
    // There should be no overflow from the low 16-bits
    //
    PCI_ASSERT(SEGMENT_NUMBER_FOR_BUS(*BusNumber) ==
               SEGMENT_NUMBER_FOR_BUS(Device->BusNumber));

    return;
}

VOID
PciReadDeviceConfigEx (
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Read the config space for a specific device or virtual function

Arguments:

    Device - The Device representing the device who's config space we want

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    Buffer - A buffer where the data will be read

    Offset - The byte offset in config space where we should start to read

    Length - The number of bytes to read

Return Value:

    None

--*/
{
    ULONG maxConfigAddress;
    ULONG lengthRead;
    PCI_SLOT_NUMBER slot;
    ULONG busNumber;

    NON_PAGED_CODE();

    lengthRead = 0;

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Read - (%x,%x,%x) + %x offset %x length %x buffer %p\n",
          Device->BusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber,
          FunctionOffset,
          Offset,
          Length,
          Buffer
          );

    //
    // Calculate the actual routing ID by adding the function offset to
    // the slot number.
    //
    PciGetConfigSlotAndBus(Device, FunctionOffset, &slot, &busNumber);

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        TraceEvents(Device->Parent->BlackBoxHandle,
            TRACE_LEVEL_ERROR,
            DBG_CONFIG,
            "PciReadDeviceConfigEx - (%x,%x,%x) Offset[%x] + length[%x] > %x\n",
            Device->BusNumber,
            Device->Slot.u.bits.DeviceNumber,
            Device->Slot.u.bits.FunctionNumber,
            Offset,
            Length,
            maxConfigAddress);

        RtlFillMemory(Buffer, Length, 0xFF);
        return;
    }

    lengthRead =
        Device->Root->PciBusInterface->ReadConfig(
            Device->Root->PciBusInterface->Context,
            busNumber,
            slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (lengthRead != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Device->BusNumber,         // Bus
                     (ULONG_PTR)Device->Slot.u.AsULONG,    // Slot
                     (ULONG_PTR)Offset,                    // Offset
                     (ULONG_PTR)TRUE                       // Read
                     );
    }
}

VOID
PciReadDeviceConfig(
    __in PPCI_DEVICE Device,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Read the config space for a specific device

Arguments:

    Device - The Device representing the device who's config space we want

    Buffer - A buffer where the data will be read

    Offset - The byte offset in config space where we should start to read

    Length - The number of bytes to read

Return Value:

    None

--*/
{
    ULONG maxConfigAddress;
    ULONG lengthRead;

    NON_PAGED_CODE();

    lengthRead = 0;

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Read - (%x,%x,%x) offset %x length %x buffer %p\n",
          Device->BusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber,
          Offset,
          Length,
          Buffer
          );

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_CONFIG,
                    "PciReadDeviceConfig - (%x,%x,%x) Offset[%x] + length[%x\n] > %x",
                    Device->BusNumber,
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber,
                    Offset,
                    Length,
                    maxConfigAddress);

        RtlFillMemory(Buffer, Length, 0xFF);
        return;
    }

    lengthRead =
        Device->Root->PciBusInterface->ReadConfig(
            Device->Root->PciBusInterface->Context,
            Device->BusNumber,
            Device->Slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (lengthRead != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Device->BusNumber,         // Bus
                     (ULONG_PTR)Device->Slot.u.AsULONG,    // Slot
                     (ULONG_PTR)Offset,                    // Offset
                     (ULONG_PTR)TRUE                       // Read
                     );
    }
}


VOID
PciWriteDeviceConfigEx (
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Write the config space for a specific device or virutal function

Arguments:

    Device - The Device representing the device who's config space we want

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    Buffer - A buffer where the data will be written

    Offset - The byte offset in config space where we should start to write

    Length - The number of bytes to write

Return Value:

    None

--*/
{
    ULONG maxConfigAddress;
    ULONG lengthWritten;
    ULONG busNumber;
    PCI_SLOT_NUMBER slot;


    NON_PAGED_CODE();

    lengthWritten = 0;

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Write - (%x,%x,%x) + %x offset %x length %x buffer %p\n",
          Device->BusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber,
          FunctionOffset,
          Offset,
          Length,
          Buffer
          );

    //
    // Calculate the actual routing ID by adding the function offset to
    // the slot number.
    //
    PciGetConfigSlotAndBus(Device, FunctionOffset, &slot, &busNumber);

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        TraceEvents(Device->Parent->BlackBoxHandle,
            TRACE_LEVEL_ERROR,
            DBG_CONFIG,
            "PciWriteDeviceConfigEx - (%x,%x,%x) Offset[%x] + length[%x\n] > %x",
            Device->BusNumber,
            Device->Slot.u.bits.DeviceNumber,
            Device->Slot.u.bits.FunctionNumber,
            Offset,
            Length,
            maxConfigAddress);

        return;
    }

    lengthWritten =
        Device->Root->PciBusInterface->WriteConfig(
            Device->Root->PciBusInterface->Context,
            busNumber,
            slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (lengthWritten != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Device->BusNumber,         // Bus
                     (ULONG_PTR)Device->Slot.u.AsULONG,    // Slot
                     (ULONG_PTR)Offset,                    // Offset
                     (ULONG_PTR)FALSE                      // Write
                     );
    }
}

VOID
PciWriteDeviceConfig(
    __in PPCI_DEVICE Device,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Write the config space for a specific device

Arguments:

    Device - The Device representing the device who's config space we want

    Buffer - A buffer where the data will be written

    Offset - The byte offset in config space where we should start to write

    Length - The number of bytes to write

Return Value:

    None

--*/
{
    ULONG maxConfigAddress;
    ULONG lengthWritten;

    NON_PAGED_CODE();

    lengthWritten = 0;

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Write - (%x,%x,%x) offset %x length %x buffer %p\n",
          Device->BusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber,
          Offset,
          Length,
          Buffer
          );

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        TraceEvents(Device->Parent->BlackBoxHandle,
            TRACE_LEVEL_ERROR,
            DBG_CONFIG,
            "PciWriteDeviceConfig - (%x,%x,%x) Offset[%x] + length[%x\n] > %x",
            Device->BusNumber,
            Device->Slot.u.bits.DeviceNumber,
            Device->Slot.u.bits.FunctionNumber,
            Offset,
            Length,
            maxConfigAddress);

        return;
    }

    lengthWritten =
        Device->Root->PciBusInterface->WriteConfig(
            Device->Root->PciBusInterface->Context,
            Device->BusNumber,
            Device->Slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (lengthWritten != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Device->BusNumber,         // Bus
                     (ULONG_PTR)Device->Slot.u.AsULONG,    // Slot
                     (ULONG_PTR)Offset,                    // Offset
                     (ULONG_PTR)FALSE                      // Write
                     );
    }
}

VOID
PciProbeDeviceConfig(
    __in PPCI_DEVICE Device,
    __out_bcount(Length) PVOID ReadBuffer,
    __out_bcount(Length) PVOID ProbeBuffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Probes config space for a device.  This involves writing all F's to the
    given register to see which bits are "sticky."  The result is returned
    to the caller.  After probing, the original contents of the register
    are restored.

Arguments:

    Device - The device whose config space will be probed

    Offset - The byte offset in config space where we should start to probe

    Length - The number of bytes to probe

    ReadBuffer - On output, will contain the value originally read from config space.

    ProbeBuffer - On output, will contain the result of the probe.

Return Value:

    None

--*/
{

    NON_PAGED_CODE();

    PciReadDeviceConfig(Device, ReadBuffer, Offset, Length);

    RtlFillMemory(ProbeBuffer, Length, 0xFF);

    PciWriteDeviceConfig(Device, ProbeBuffer, Offset, Length);

    PciReadDeviceConfig(Device, ProbeBuffer, Offset, Length);

    PciWriteDeviceConfig(Device, ReadBuffer, Offset, Length);
}

VOID
PciReadSlotConfig (
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Read config space for a specific bus/slot

Arguments:

    Bus - The bus on which the device whose config space we want resides

    Slot - The Device/Function of the device on that bus we are interested in

    Buffer - A buffer where the data will be read

    Offset - The byte offset in config space where we should start to read

    Length - The number of bytes to read

Return Value:

    None

--*/
{

    ULONG LengthRead;
    ULONG MaxConfigAddress;

    NON_PAGED_CODE();

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Read - (%x,%x,%x) offset %x length %x buffer %p\n",
          Bus->SecondaryBusNumber,
          Slot.u.bits.DeviceNumber,
          Slot.u.bits.FunctionNumber,
          Offset,
          Length,
          Buffer
          );

    if (Bus->ExtendedConfigAvailable != FALSE) {
        MaxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        MaxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    //
    // Don't access extended config space in the SlotConfig path.
    // We don't know if extended config is available in this case.
    //
    if ((Length + Offset) > MaxConfigAddress) {
        RtlFillMemory(Buffer, Length, 0xFF);
        return;
    }

    LengthRead =
        Bus->Root->PciBusInterface->ReadConfig(
            Bus->Root->PciBusInterface->Context,
            Bus->SecondaryBusNumber,
            Slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (LengthRead != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Bus->SecondaryBusNumber, // Bus
                     (ULONG_PTR)Slot.u.AsULONG,          // Slot
                     (ULONG_PTR)Offset,                  // Offset
                     (ULONG_PTR)TRUE                     // Read
                     );
    }
}

VOID
PciWriteSlotConfig (
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Write config space for a specific bus/slot

Arguments:

    ParentFdo - The FDO of the bus who's config space we want

    Slot - The Device/Function of the device on that bus we are interested in

    Buffer - A buffer where the data will be written

    Offset - The byte offset in config space where we should start to write

    Length - The number of bytes to write

Return Value:

    None

--*/
{

    ULONG LengthWritten;
    ULONG MaxConfigAddress;

    NON_PAGED_CODE();

    TRACE(CONFIG_TRACE_LEVEL,
          "Config Write - (%x,%x,%x) offset %x length %x buffer %p\n",
          Bus->SecondaryBusNumber,
          Slot.u.bits.DeviceNumber,
          Slot.u.bits.FunctionNumber,
          Offset,
          Length,
          Buffer
          );

    if (Bus->ExtendedConfigAvailable != FALSE) {
        MaxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        MaxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    //
    // Don't access extended config space in the SlotConfig path.
    // We don't know if extended config is available in this case.
    //
    if ((Length + Offset) > MaxConfigAddress) {
        return;
    }

    LengthWritten =
        Bus->Root->PciBusInterface->WriteConfig(
            Bus->Root->PciBusInterface->Context,
            Bus->SecondaryBusNumber,
            Slot.u.AsULONG,
            Buffer,
            Offset,
            Length
            );

    if (LengthWritten != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR) Bus->SecondaryBusNumber,  // Bus
                     (ULONG_PTR) Slot.u.AsULONG,           // Slot
                     (ULONG_PTR) Offset,                   // Offset
                     (ULONG_PTR) FALSE                     // Write
                     );
    }
}

// ------------------------------------------------------ Bus Interface Standard
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciBusInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the BUS_INTERFACE_STANDARD fields.

Arguments:

    PciInterface    Pointer to the PciInterface record for this
                    interface type.
    InterfaceSpecificData
                    A ULONG containing the resource type for which
                    arbitration is required.
    InterfaceReturn

    Filtered - Not supported for this interface.
 
Return Value:

    TRUE is this device is not known to cause problems, FALSE
    if the device should be skipped altogether.

--*/
{
    PULONG busNumber;
    PBUS_INTERFACE_STANDARD standard;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(BUS_INTERFACE_STANDARD)) {
        return STATUS_UNSUCCESSFUL;
    }

    standard = (PBUS_INTERFACE_STANDARD)InterfaceReturn;
    standard->Size = sizeof(BUS_INTERFACE_STANDARD);
    standard->Version = BUSINTRF_VERSION;
    standard->Context = DeviceExtension;
    busNumber = (PULONG)InterfaceSpecificData;
    if (busNumber != NULL) {
        *busNumber = ((PPCI_DEVICE)DeviceExtension)->BusNumber;
    }

    standard->InterfaceReference = PciBusInterface_Reference;
    standard->InterfaceDereference = PciBusInterface_Dereference;

    standard->TranslateBusAddress = PciBusInterface_TranslateBusAddress;
    standard->GetDmaAdapter = PciBusInterface_GetDmaAdapter;
    standard->GetBusData = PciBusInterface_GetBusData;
    standard->SetBusData = PciBusInterface_SetBusData;

    return STATUS_SUCCESS;
}

VOID
PciBusInterface_Reference(
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

    InterlockedIncrement(&device->PciBusInterfaceCount);

    return;
}

VOID
PciBusInterface_Dereference(
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

    InterlockedDecrement(&device->PciBusInterfaceCount);

    return;
}

__drv_functionClass(TRANSLATE_BUS_ADDRESS)
_IRQL_requires_same_
BOOLEAN
PciBusInterface_TranslateBusAddress(
    _Inout_opt_ PVOID Context,
    _In_ PHYSICAL_ADDRESS BusAddress,
    _In_ ULONG Length,
    _Out_ PULONG AddressSpace,
    _Out_ PPHYSICAL_ADDRESS TranslatedAddress
    )
/*++

Routine Description:

    This function is used to translate bus addresses from legacy drivers.

Arguments:

    Context - Supplies a pointer to the interface context.  This is actually
        the PDO for the root bus.

    BusAddress - Supplies the orginal address to be translated.

    Length - Supplies the length of the range to be translated.

    AddressSpace - Points to the location of of the address space type such as
        memory or I/O port.  This value is updated by the translation.

    TranslatedAddress - Returns the translated address.

Return Value:

    Returns a boolean indicating if the operations was a success.

--*/
{
    PPCI_DEVICE device;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Length);

    device = (PPCI_DEVICE)Context;

    PCI_ASSERT(device->Signature == PCI_DEVICE_SIGNATURE);

#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "incompatible annotations on TRANSLATE_BUSADDRESS and HalTranslateBusAddress")
    return HalTranslateBusAddress(PCIBus,
                                  device->BusNumber,
                                  BusAddress,
                                  AddressSpace,
                                  TranslatedAddress
                                  );
}

__drv_functionClass(GET_DMA_ADAPTER)
_IRQL_requires_same_
struct _DMA_ADAPTER *
PciBusInterface_GetDmaAdapter (
    _Inout_opt_ PVOID Context,
    _In_ struct _DEVICE_DESCRIPTION *DeviceDescriptor,
    _Out_ PULONG NumberOfMapRegisters
    )
/*++

Routine Description:

    This function gets a DMA adapter for the device.

Arguments:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device.

    DeviceDescriptor - Supplies the device descriptor used to allocate the dma
        adapter object.

    NubmerOfMapRegisters - Returns the maximum number of map registers a device
        can allocate at one time.

Return Value:

    Returns a DMA adapter or NULL.

--*/
{
    PPCI_DEVICE device;;

    PAGED_CODE();

    device = (PPCI_DEVICE)Context;

    PCI_ASSERT(device->Signature == PCI_DEVICE_SIGNATURE);

    //
    // If this is DMA on a PCI bus update the bus number, otherwise leave well
    // alone
    //
    if (DeviceDescriptor->InterfaceType == PCIBus) {
        DeviceDescriptor->BusNumber = device->BusNumber;
    }

    return IoGetDmaAdapter(
               device->Parent->PhysicalDeviceObject,
               DeviceDescriptor,
               NumberOfMapRegisters
               );
}

__drv_functionClass(GET_SET_DEVICE_DATA)
_IRQL_requires_same_
ULONG
PciBusInterface_GetBusData (
    _Inout_opt_ PVOID Context,
    _In_ ULONG WhichSpace,
    _Inout_updates_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )
/*++

Routine Description:

    This function reads the PCI configuration space of a device.

Arguments:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device.

    WhichSpace - what type of space we want to read - of the form PCI_WHICHSPACE_*

    Buffer - Supplies a pointer to where the data is to be returned

    Offset - Indicates the offset into the space where the reading should begin.

    Length - Indicates the count of bytes which should be read.

Return Value:

    Returns the number of bytes actually read.

--*/
{
    ULONG lengthRead = 0;

    NON_PAGED_CODE();

    PciReadDeviceSpace(
        (PPCI_DEVICE)Context,
        0,
        WhichSpace,
        Buffer,
        Offset,
        Length,
        &lengthRead
        );

    return lengthRead;
}

__drv_functionClass(GET_SET_DEVICE_DATA)
_IRQL_requires_same_
ULONG
PciBusInterface_SetBusData (
    _Inout_opt_ PVOID Context,
    _In_ ULONG WhichSpace,
    _Inout_updates_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )
/*++

Routine Description:

    This function writes the PCI configuration space of a device.

Arguments:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device.

    WhichSpace - what type of space we want to write - of the form PCI_WHICHSPACE_*

    Buffer - Supplies a pointer to where the data is contained

    Offset - Indicates the offset into the space where the writing should begin.

    Length - Indicates the count of bytes which should be written.

Return Value:

    Returns the number of bytes actually written.

--*/
{
    ULONG lengthWritten = 0;

    NON_PAGED_CODE();

    PciWriteDeviceSpace(
        (PPCI_DEVICE)Context,
        0,
        WhichSpace,
        Buffer,
        Offset,
        Length,
        &lengthWritten
        );

    return lengthWritten;
}

// ------------------------------------------------------------------------- AGP Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciAgpTargetInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the AGP_TARGET_BUS_INTERFACE fields.
    This interface can only be requested from the device stack of an AGP
    bridge.  This routine looks for an AGP capability in the requester
    bridge and any peer host bridges.  If one is found, config access
    to the device with the capability is granted.

Arguments:

    PciInterface    Pointer to the PciInterface record for this
                    interface type.
    InterfaceSpecificData
                    A ULONG containing the resource type for which
                    arbitration is required.
    InterfaceReturn

    Filtered - Not supported for this interface.
 
Return Value:

    TRUE is this device is not known to cause problems, FALSE
    if the device should be skipped altogether.

--*/
{
    PAGP_TARGET_BUS_INTERFACE_STANDARD targetInterface;
    PPCI_DEVICE device, current, targetDevice;
    PCI_CAPABILITIES_HEADER header;
    PPCI_BUS bus;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(AGP_TARGET_BUS_INTERFACE_STANDARD)) {
        return STATUS_UNSUCCESSFUL;
    }

    targetInterface = (PAGP_TARGET_BUS_INTERFACE_STANDARD)InterfaceReturn;
    device = (PPCI_DEVICE)DeviceExtension;

    if ((device->BaseClass != PCI_CLASS_BRIDGE_DEV) ||
        (device->SubClass != PCI_SUBCLASS_BR_PCI_TO_PCI)) {

        //
        // This interface is only supported on AGP bridges,
        // which are PCI-PCI bridges.
        //
        return STATUS_NOT_SUPPORTED;
    }

    targetDevice = NULL;

    if (device->AgpCapability) {

        //
        // The bridge has a target AGP capability itself.  Give the
        // caller access to its config space.
        //
        targetDevice = device;

    } else {

        //
        // No target AGP capability on the bridge itself.  Look on the same
        // bus as the bridge for host bridges with the AGP capability.
        //
        bus = device->Parent;

        if (bus->PciBridge) {

            //
            // Not likely to find host bridges below P-P bridges.
            // Even if we could, constrain this interface to only support
            // root busses.
            //
            return STATUS_NOT_SUPPORTED;
        }

        PciAcquirePassiveLock(&bus->ChildDeviceLock);
        for (current = bus->ChildDevices; current != NULL; current = current->Sibling) {

            if ((PCI_HOST_BRIDGE(current) != FALSE) &&
                (current->AgpCapability != 0)) {

                //
                // We have a host bridge with a target AGP capability.  Check to make
                // sure that there's only one such host bridge.  If there are multiple,
                // we don't know which to grant access to, so fail the call.
                //
                if (targetDevice != NULL) {
                    PciReleasePassiveLock(&bus->ChildDeviceLock);
                    return STATUS_NOT_SUPPORTED;
                }

                targetDevice = current;
            }
        }
        PciReleasePassiveLock(&bus->ChildDeviceLock);

        if (targetDevice == NULL) {
            return STATUS_NO_SUCH_DEVICE;
        }
    }

    PCI_ASSERT((targetDevice != NULL) && (targetDevice->AgpCapability != 0));

    PciReadDeviceConfig(targetDevice,
                        &header,
                        targetDevice->AgpCapability,
                        sizeof(PCI_CAPABILITIES_HEADER)
                        );

    targetInterface->Size = sizeof(AGP_TARGET_BUS_INTERFACE_STANDARD);
    targetInterface->Version = AGPINTRF_VERSION;
    targetInterface->Context = targetDevice;
    targetInterface->InterfaceReference = PciAgpTargetInterface_Reference;
    targetInterface->InterfaceDereference = PciAgpTargetInterface_Dereference;

    targetInterface->CapabilityID = header.CapabilityID;
    targetInterface->GetBusData = PciBusInterface_GetBusData;
    targetInterface->SetBusData = PciBusInterface_SetBusData;

    return STATUS_SUCCESS;
}

VOID
PciAgpTargetInterface_Reference(
    __inout PVOID Context
    )
{
    PPCI_DEVICE targetDevice = (PPCI_DEVICE)Context;

    PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(targetDevice));

    if (InterlockedIncrement(&targetDevice->AgpTargetInterfaceRefCount) == 1) {

        ObReferenceObject(targetDevice->DeviceObject);
    }
}

VOID
PciAgpTargetInterface_Dereference(
    __inout PVOID Context
    )
{
    PPCI_DEVICE targetDevice = (PPCI_DEVICE)Context;

    PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(targetDevice));

    if (InterlockedDecrement(&targetDevice->AgpTargetInterfaceRefCount) == 0) {

        ObDereferenceObject(targetDevice->DeviceObject);
    }
}

// ------------------------------------- External Driver Config Access Functions
// ------------------------------ Used for both interface and IRP config access.

NTSTATUS
PciExternalReadDeviceConfig(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Called when agents outside the PCI driver want to access config space
    (either from a READ_CONFIG IRP or through BUS_INTERFACE_STANDARD).

    This function performs extra sanity checks and sanitization on the
    arguments and also double buffers the data as Buffer might be
    pageable and we access config space at high IRQL.

Arguments:

    Device - The Device representing the device who's config space we want

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    Buffer - A buffer where the data will be read or written

    Offset - The byte offset in config space where we should start to read/write

    Length - The number of bytes to read/write

Return Value:

    None

--*/
{
    ULONG maxConfigAddress;
    PUCHAR readBuffer;
    KIRQL irql;

    NON_PAGED_CODE();

    TRACE(CONFIG_TRACE_LEVEL, "External Config Read\n");

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Avoid touching config space for devices which are in D3cold.
    //
    if (PCI_DEVICE_IN_D3COLD(Device)) {

        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "PciExternalReadDeviceConfig: %p reading config space while in D3cold - offset: 0x%x len: 0x%x",
                    Device,
                    Offset,
                    Length);

        RtlFillMemory(Buffer, Length, 0xFF);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // If this call is coming in at a lower IRQL than DISPATCH_LEVEL,
    // read the data into a global buffer which is guaranteed to not
    // be paged.  This is because we access config space  at
    // > DISPATCH_LEVEL and the DDK says that the buffer *should*
    // be in paged pool.  Don't do this for accesses at >= DISPATCH_LEVEL
    // because synchronizing these accesses would require a HIGH_LEVEL
    // spinlock and because any callers at this IRQL need to have
    // non paged buffers already to access the data at that IRQL.
    //
    // This used to be done with a stack-based double buffer.  However,
    // extended config space means that a single access can span 4K
    // of config space, which is a lot of stack space to use up.
    //
    irql = KeGetCurrentIrql();
    if (irql < DISPATCH_LEVEL) {

        PciAcquirePassiveLock(&PciConfigDoubleBufferLock);
        readBuffer = &PciConfigDoubleBuffer[Offset];

    } else {
        readBuffer = Buffer;
    }

    PciReadDeviceConfigEx(Device, FunctionOffset, readBuffer, Offset, Length);

    //
    // If we are reading the interrupt line register then adjust it.
    //

    if ((Device->InterruptPin != 0) &&
        INTERSECT_CONFIG_FIELD(Offset, Length, common.InterruptLine)) {

        readBuffer[CONFIG_FIELD_OFFSET(common.InterruptLine) - Offset]
            = Device->InterruptResource.InterruptLine;
    }

    if (irql < DISPATCH_LEVEL) {
        RtlCopyMemory(Buffer, &PciConfigDoubleBuffer[Offset], Length);
        PciReleasePassiveLock(&PciConfigDoubleBufferLock);
    }

    return STATUS_SUCCESS;
}


NTSTATUS
PciExternalWriteDeviceConfig(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )
/*++

Routine Description:

    Called when agents outside the PCI driver want to access config space
    (either from a WRITE_CONFIG IRP or through BUS_INTERFACE_STANDARD).

    This function performs extra sanity checks and sanitization on the
    arguments and also double buffers the data as Buffer might be
    pageable and we access config space at high IRQL.

Arguments:

    Device - The Device representing the device who's config space we want

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    Buffer - A buffer where the data will be written

    Offset - The byte offset in config space where we should start to write

    Length - The number of bytes to write

Return Value:

    None

--*/
{
    PPCI_VERIFIER_DATA verifierData;
    ULONG maxConfigAddress;
    BOOLEAN illegalAccess;
    PUCHAR writeBuffer;
    KIRQL irql;

    NON_PAGED_CODE();

    TRACE(CONFIG_TRACE_LEVEL, "External Config Write\n");

    if (Device->ExtendedConfigAvailable != FALSE) {
        maxConfigAddress = PCI_EXTENDED_CONFIG_LENGTH;

    } else {
        maxConfigAddress = sizeof(PCI_COMMON_CONFIG);
    }

    if ((Length + Offset) > maxConfigAddress) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Avoid touching config space for devices which are in D3cold.
    //
    if (PCI_DEVICE_IN_D3COLD(Device)) {

        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "PciExternalWriteDeviceConfig: %p writing config space while in D3cold - offset: 0x%x len: 0x%x",
                    Device,
                    Offset,
                    Length);

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    illegalAccess = FALSE;

    //
    // Make sure they are not touching registers they should not be.  For
    // backward compatiblity we will just complain and let the request through.
    //
    switch (Device->HeaderType) {
        case PCI_DEVICE_TYPE:

            //
            // They should not be writing to their BARS including the ROM BAR
            //
            if (INTERSECT_CONFIG_FIELD(Offset, Length, type0.BaseAddresses)
            ||  INTERSECT_CONFIG_FIELD(Offset, Length, type0.ROMBaseAddress)) {
                illegalAccess = TRUE;
            }
            break;

        case PCI_BRIDGE_TYPE:
            //
            // For bridges they should not touch the bars, the base and limit registers,
            // the bus numbers or bridge control
            //
            if (INTERSECT_CONFIG_FIELD(Offset, Length, type1.BaseAddresses)
            ||  INTERSECT_CONFIG_FIELD_RANGE(Offset, Length, type1.PrimaryBus, type1.SubordinateBus)
            ||  INTERSECT_CONFIG_FIELD_RANGE(Offset, Length, type1.IOBase, type1.IOLimit)
            ||  INTERSECT_CONFIG_FIELD_RANGE(Offset, Length, type1.MemoryBase, type1.IOLimitUpper16)
            ||  INTERSECT_CONFIG_FIELD(Offset, Length, type1.ROMBaseAddress)
            ||  INTERSECT_CONFIG_FIELD(Offset, Length, type1.BridgeControl)) {
                illegalAccess = TRUE;
            }
            break;

        case PCI_CARDBUS_BRIDGE_TYPE:

            //
            // For cardbus bridges they should not touch the bars, the base and limit registers
            // or the bus numbers.  Bridge control is modified by PCMCIA to control cardbus
            // IRQ routing so must be ok.
            //
            if (INTERSECT_CONFIG_FIELD(Offset, Length, type2.SocketRegistersBaseAddress)
            ||  INTERSECT_CONFIG_FIELD_RANGE(Offset, Length, type2.PrimaryBus, type2.SubordinateBus)
            ||  INTERSECT_CONFIG_FIELD_RANGE(Offset, Length, type2.MemoryBase0, type2.IoLimit1)) {
                illegalAccess = TRUE;
            }
            break;
    }

    if ((Device->PowerCapability != 0) &&
        !((Offset > ((ULONG)Device->PowerCapability + FIELD_OFFSET(PCI_PM_CAPABILITY, Data))) ||
          (Offset + Length <= (Device->PowerCapability)))) {
        illegalAccess = TRUE;
    }

    if (illegalAccess) {

        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_CONFIG,
                    "PciExternalWriteDeviceConfig: %p writing illegal range - offset: 0x%x len: 0x%x",
                    Device,
                    Offset,
                    Length);

        verifierData = PciVerifierRetrieveFailureData(
            PCI_VERIFIER_PROTECTED_CONFIGSPACE_ACCESS
            );

        if (!verifierData) {
            ERROR("PciExternalWriteDeviceConfig: Verfier data not found at line %d.\n",
                  __LINE__);

            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // We fail the devnode instead of the driver because we don't actually
        // have an address to pass to the driver verifier.
        //
        VfFailDeviceNode(
            Device->DeviceObject,
            PCI_VERIFIER_DETECTED_VIOLATION,
            PCI_VERIFIER_PROTECTED_CONFIGSPACE_ACCESS,
            verifierData->FailureClass,
            &verifierData->Flags,
            verifierData->FailureText,
            "%DevObj%Ulong%Ulong",
            Device->DeviceObject,
            Offset,
            Length
            );
    }

    //
    // If this call is coming in at a lower IRQL than DISPATCH_LEVEL,
    // copy the data into a global buffer which is guaranteed to not
    // be paged.  This is because we access config space  at
    // > DISPATCH_LEVEL and the DDK says that the buffer *should*
    // be in paged pool.  Don't do this for accesses at >= DISPATCH_LEVEL
    // because synchronizing these accesses would require a HIGH_LEVEL
    // spinlock and because any callers at this IRQL need to have
    // non paged buffers already to access the data at that IRQL.
    //
    // This used to be done with a stack-based double buffer.  However,
    // extended config space means that a single access can span 4K
    // of config space, which is a lot of stack space to use up.
    //
    irql = KeGetCurrentIrql();
    if (irql < DISPATCH_LEVEL) {

        PciAcquirePassiveLock(&PciConfigDoubleBufferLock);
        RtlCopyMemory(&PciConfigDoubleBuffer[Offset], Buffer, Length);
        writeBuffer = &PciConfigDoubleBuffer[Offset];

    } else {
        writeBuffer = Buffer;
    }

    //
    // If we are writing the interrupt line register then adjust it so we write
    // the raw value back again
    //
    if ((Device->InterruptPin != 0) &&
        INTERSECT_CONFIG_FIELD(Offset, Length, common.InterruptLine)) {

        writeBuffer[CONFIG_FIELD_OFFSET(common.InterruptLine) - Offset]
            = Device->SavedRegisters.InterruptLine;
    }

    PciWriteDeviceConfigEx(Device,
                           FunctionOffset, 
                           writeBuffer, 
                           Offset, 
                           Length);

    if (irql < DISPATCH_LEVEL) {
        PciReleasePassiveLock(&PciConfigDoubleBufferLock);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciReadDeviceSpace(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in ULONG WhichSpace,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __out PULONG LengthRead
    )
/*++

Routine Description:

    This function handles reading from PCI device spaces and is called for both
    the IRP_MN_READ_CONFIG and the BUS_INTERFACE_STANDARD.GetBusData cases.

Arguments:

    Device - the device we want to read from

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    WhichSpace - what type of space we want to read - of the form PCI_WHICHSPACE_*

    Buffer - Supplies a pointer to where the data is to be returned

    Offset - Indicates the offset into the space where the reading should begin.

    Length - Indicates the count of bytes which should be read.

    LengthRead - Pointer to a variable that will contain the number of bytes
        actually read.

Return Value:

    NT Status code.

--*/
{
    ULONG requestEnd;
    NTSTATUS status;
    PPCI_VERIFIER_DATA verifierData;

    NON_PAGED_CODE();

    //
    // Ensure that the offset and length passed in by the caller do not
    // result in a range of offsets that overflow a ULONG.
    //
    requestEnd = 0;
    status = RtlULongAdd(Offset, Length, &requestEnd);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    switch (WhichSpace) {

        default:

            //
            // Many people hand in the wrong WhichSpace parameters
            // slap them around if we are verifing...
            //
            verifierData = PciVerifierRetrieveFailureData(PCI_VERIFIER_INVALID_WHICHSPACE);

            if (!verifierData) {
                ERROR("PciReadDeviceSpace: Verfier data not found at line %d.\n",
                      __LINE__);

                return STATUS_INSUFFICIENT_RESOURCES;
            }

            VfFailDeviceNode(
                Device->DeviceObject,
                PCI_VERIFIER_DETECTED_VIOLATION,
                PCI_VERIFIER_INVALID_WHICHSPACE,
                verifierData->FailureClass,
                &verifierData->Flags,
                verifierData->FailureText,
                "%DevObj%Ulong",
                Device->DeviceObject,
                WhichSpace
                );

            // fall through

        case PCI_WHICHSPACE_CONFIG:

            status = PciExternalReadDeviceConfig(
                        Device,
                        FunctionOffset,
                        Buffer,
                        Offset,
                        Length
                        );

            *LengthRead = NT_SUCCESS(status) ? Length : 0;

            break;

        case PCI_WHICHSPACE_ROM:

            *LengthRead = Length;

            status = PciReadRomImage(
                         Device,
                         WhichSpace,
                         Buffer,
                         Offset,
                         LengthRead
                         );
            break;
    }

    return status;
}


NTSTATUS
PciWriteDeviceSpace(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in ULONG WhichSpace,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __out PULONG LengthWritten
    )
/*++

Routine Description:

    This function handles reading from PCI device spaces and is called for both
    the IRP_MN_WRITE_CONFIG and the BUS_INTERFACE_STANDARD.SetBusData cases.

Arguments:

    Device - the device we want to write to

    FunctionOffset - Routing ID delta of the virtual function.  This
        is zero for non SR-IOV devices, and for physical functions and
        should be VFStride * VfNumber + FirstVfOffset when reading a
        virtual function's space

    WhichSpace - what type of space we want to write - of the form PCI_WHICHSPACE_*

    Buffer - Supplies a pointer to where the data is to be written resides

    Offset - Indicates the offset into the space where the writing should begin.

    LengthWritten - Pointer to a variable that will contain the number of bytes
        actually written.

Return Value:

    NT Status code.

--*/
{
    ULONG requestEnd;
    NTSTATUS status;
    PPCI_VERIFIER_DATA verifierData;

    NON_PAGED_CODE();

    //
    // Ensure that the offset and length passed in by the caller do not
    // result in a range of offsets that overflow a ULONG.
    //

    requestEnd = 0;
    status = RtlULongAdd(Offset, Length, &requestEnd);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    switch (WhichSpace) {

        default:

            //
            // Many people hand in the wrong WhichSpace parameters
            // slap them around if we are verifing...
            //
            verifierData = PciVerifierRetrieveFailureData(PCI_VERIFIER_INVALID_WHICHSPACE);

            if (!verifierData) {
                ERROR("PciWriteDeviceSpace: Verfier data not found at line %d.\n",
                      __LINE__);

                return STATUS_INSUFFICIENT_RESOURCES;
            }

            VfFailDeviceNode(
                Device->DeviceObject,
                PCI_VERIFIER_DETECTED_VIOLATION,
                PCI_VERIFIER_INVALID_WHICHSPACE,
                verifierData->FailureClass,
                &verifierData->Flags,
                verifierData->FailureText,
                "%DevObj%Ulong",
                Device->DeviceObject,
                WhichSpace
                );

            // fall through

        case PCI_WHICHSPACE_CONFIG:

            status = PciExternalWriteDeviceConfig(
                        Device,
                        FunctionOffset,
                        Buffer,
                        Offset,
                        Length
                        );

            *LengthWritten = NT_SUCCESS(status) ? Length : 0;

            break;

        case PCI_WHICHSPACE_ROM:

            //
            // You can't write ROM
            //
            WARN("devext %p: WRITE_CONFIG IRP for ROM, failing.\n",Device);

            *LengthWritten = 0;
            status = STATUS_INVALID_DEVICE_REQUEST;

            break;

    }

    return status;
}
