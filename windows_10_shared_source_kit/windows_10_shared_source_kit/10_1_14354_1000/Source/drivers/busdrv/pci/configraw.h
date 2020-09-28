/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    configraw.h

Abstract:











--*/

#pragma once

typedef BOOLEAN (*PCI_FIND_CAPS_CALLBACK)(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG Offset,
    _Inout_opt_ PVOID Context);

BOOLEAN
PciIsDevicePresentRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

BOOLEAN
PciIsUnconfiguredBridge(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot);

UCHAR
PciReadHeaderTypeRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

BOOLEAN
PciIsDeviceABridge(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

BOOLEAN
PciIsDeviceMultifunction(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

ULONG
PciFindCapOffsetRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR CapabilityID,
    _In_opt_ PCI_FIND_CAPS_CALLBACK Callback,
    _Inout_opt_ PVOID Context
    );

ULONG
PciFindExtCapOffsetRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ USHORT CapabilityID,
    _In_opt_ PCI_FIND_CAPS_CALLBACK Callback,
    _Inout_opt_ PVOID Context
    );

VOID
PciWriteBusNumsRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR Primary,
    _In_ UCHAR Secondary,
    _In_ UCHAR Subordinate
    );

VOID
PciReadBusNumsRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_ UCHAR *Primary,
    _Out_ UCHAR *Secondary,
    _Out_ UCHAR *Subordinate
    );

BOOLEAN
PciIsDeviceSlotImplementedRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG ExpressCapsOffset
    );

BOOLEAN
PciIsDeviceHotplugCapableRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG ExpressCapsOffset
    );

BOOLEAN
PciIsDeviceThunderboltRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

BOOLEAN
PciTryEnableBridgeARIRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG DeviceBus,
    _In_ PCI_SLOT_NUMBER DeviceSlot,
    _In_ ULONG BridgeBus,
    _In_ PCI_SLOT_NUMBER BridgeSlot
    );

ULONG
PciReadNextARIFunctionRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    );

UCHAR
PciSriovBusNumbersRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ BOOLEAN AriEnabled
    );

VOID
PciProbeConfigRegistersRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_writes_bytes_(Length) PVOID ReadBuffer,
    _Out_writes_bytes_(Length) PVOID ProbeBuffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    );

VOID
PciReadConfigRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    );

VOID
PciWriteConfigRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    );

#define PciWriteConfigRegRaw(PciIF, Bus, Slot, Buf, Reg)            \
            PciWriteConfigRaw(PciIF, Bus, Slot, Buf,                \
            CONFIG_FIELD_OFFSET(Reg), CONFIG_FIELD_LENGTH(Reg))     \

#define PciReadConfigRegRaw(PciIF, Bus, Slot, Buf, Reg)             \
            PciReadConfigRaw(PciIF, Bus, Slot, Buf,                 \
            CONFIG_FIELD_OFFSET(Reg), CONFIG_FIELD_LENGTH(Reg))     \


