/*++
Copyright (c) Microsoft Corporation

Module Name:

    xhciwmi.h

Abstract:

    This file contains internal WMI support for the xHCI host controller driver.

Environment:

    User-mode

--*/

#pragma once

// {6D88B760-85FA-426a-92D2-3B17CC39A5CA}
DEFINE_GUID(GUID_USBXHCI_GET_CONTROLLER_CAPABILITIES,
            0x6d88b760, 0x85fa, 0x426a, 0x92, 0xd2, 0x3b, 0x17, 0xcc, 0x39, 0xa5, 0xca);

typedef enum {
    UsbXhciControllerParentBusTypeUnknown,
    UsbXhciControllerParentBusTypePci,
    UsbXhciControllerParentBusTypeAcpi
} USBXHCI_CONTROLLER_PARENT_BUS_TYPE;

#define USBXHCI_MAX_ACPI_VENDOR_ID_STRING_LENGTH    5
#define USBXHCI_MAX_ACPI_DEVICE_ID_STRING_LENGTH    5
#define USBXHCI_MAX_ACPI_REVISION_ID_STRING_LENGTH  5

typedef struct _USBXHCI_CONTROLLER_CAPABILITIES {

    USBXHCI_CONTROLLER_PARENT_BUS_TYPE  ParentBusType;

    ULONG                               PciVendorId;
    ULONG                               PciDeviceId;
    ULONG                               PciRevision;

    CHAR                                AcpiVendorId[USBXHCI_MAX_ACPI_VENDOR_ID_STRING_LENGTH];
    CHAR                                AcpiDeviceId[USBXHCI_MAX_ACPI_DEVICE_ID_STRING_LENGTH];
    USHORT                              AcpiRevision[USBXHCI_MAX_ACPI_REVISION_ID_STRING_LENGTH];

    ULONGLONG                           FirmwareVersion;

    ULONG                               MajorRevision;
    ULONG                               MinorRevision;

    ULONG                               Slots;
    ULONG                               Interrupters;
    ULONG                               Ports;

    ULONG                               IsochSchedulingThreshold;
    ULONG                               EventRingSegmentTableMax;
    ULONG                               ScratchpadRestore;
    ULONG                               MaxScratchpadBuffers;

    ULONG                               U1DeviceExitLatency;
    ULONG                               U2DeviceExitLatency;

    ULONG                               AddressCapability;
    ULONG                               BwNegotiationCapability;
    ULONG                               ContextSize;
    ULONG                               PortPowerControl;
    ULONG                               PortIndicators;
    ULONG                               LightHCResetCapability;
    ULONG                               LatencyToleranceMessagingCapability;
    ULONG                               NoSecondarySidSupport;
    ULONG                               ParseAllEventData;
    ULONG                               StoppedShortPacketCapability;
    ULONG                               StoppedEDTLACapability;
    ULONG                               ContiguousFrameIDCapability;
    ULONG                               MaximumPrimaryStreamArraySize;

    ULONG                               U3EntryCapability;
    ULONG                               ConfigureEndpointCommandMaxExitLatencyCapability;
    ULONG                               ForceSaveContextCapability;
    ULONG                               ComplianceTransitionCapability;
    ULONG                               LargeESITPayloadCapability;
    ULONG                               ConfigurationInformationCapability;
    ULONG                               ExtendedTBCCapability;

    ULONG                               MessageSignaledInterrupts;

    ULONG                               ExtendedCapabilities[1];

} USBXHCI_CONTROLLER_CAPABILITIES, *PUSBXHCI_CONTROLLER_CAPABILITIES;

