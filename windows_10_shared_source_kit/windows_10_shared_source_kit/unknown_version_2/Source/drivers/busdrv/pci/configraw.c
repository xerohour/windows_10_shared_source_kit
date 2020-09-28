/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    configraw.c

Abstract:

    Implements functions for reading/writing config space using just raw
    bus/device/function IDs and without using the PCI driver's
    device/bus/bridge structures.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "configraw.h"

// ------------------------------------------------------------------ Prototypes

// ------------------------------------------------------------------- Functions

ULONG
PciFindCapOffsetRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR CapabilityID,
    _In_opt_ PCI_FIND_CAPS_CALLBACK Callback,
    _Inout_opt_ PVOID Context
    )

/*++

Routine Description:

    Searches the PCI-compatible 256-byte config space for a specific capability.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Slot - Provides the device/function number.

    CapabilityID - Provides the ID of the capability to search for.

    Callback - Provides an optional callback function. This function is called
        on each CapabilityID match. Searching continues until either the
        callback returns TRUE or no further matches are found. This feature is
        useful for finding a specific instance of a capability which may occur
        more than once in config space.

    Context - Provides an optional context pointer to be passed into the
        callback.

Return Value:

    If found, the offset of the config record. Otherwise 0.

--*/

{

    PCI_CAPABILITIES_HEADER CapHeader;
    ULONG CapOffset;
    ULONG CapOffsetMax;
    ULONG CapOffsetMin;
    UCHAR HeaderType;

    //
    // Determine starting offset and the min/max range from which to read.
    //

    CapOffset = 0;
    CapOffsetMin = 0;
    CapOffsetMax = sizeof(PCI_COMMON_CONFIG) - sizeof(PCI_CAPABILITIES_HEADER);
    HeaderType = PciReadHeaderTypeRaw(PciIF, Bus, Slot);
    HeaderType &= ~PCI_MULTIFUNCTION;
    switch (HeaderType) {
    case PCI_DEVICE_TYPE:
        PciReadConfigRegRaw(PciIF, Bus, Slot, &CapOffset, type0.CapabilitiesPtr);
        CapOffsetMin = PCI_TYPE0_HEADER_LENGTH;
        break;

    case PCI_BRIDGE_TYPE:
        PciReadConfigRegRaw(PciIF, Bus, Slot, &CapOffset, type1.CapabilitiesPtr);
        CapOffsetMin = PCI_TYPE1_HEADER_LENGTH;
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:
        PciReadConfigRegRaw(PciIF, Bus, Slot, &CapOffset, type2.CapabilitiesPtr);
        CapOffsetMin = PCI_TYPE2_HEADER_LENGTH;
        break;

    default:
        PCI_ASSERT(HeaderType <= PCI_CARDBUS_BRIDGE_TYPE);
        break;
    }

    //
    // Capabilities are implemented as a singly-linked list. An offset of 0
    // signifies the end of the list. Iterate over each entry until the end.
    //

    while (CapOffset != 0) {

        //
        // Validate that the offset is within the valid range and is on a
        // dword boundary.
        //

        if ((CapOffset < CapOffsetMin) ||
            (CapOffset > CapOffsetMax) ||
            ((CapOffset & 0x3) != 0)) {

            CapOffset = 0;
            break;
        }

        //
        // Read the config header and see if the ID matches.
        //

        PciReadConfigRaw(PciIF, Bus, Slot, &CapHeader, CapOffset, sizeof(CapHeader));
        if (CapHeader.CapabilityID == CapabilityID) {

            //
            // If no callback, return now.
            //

            if (Callback == NULL) {
                break;
            }

            //
            // If the callback returns TRUE, return now.
            //

            if (Callback(PciIF, Bus, Slot, CapOffset, Context) != FALSE) {
                break;
            }
        }

        CapOffset = CapHeader.Next;
    }

    return CapOffset;
}

ULONG
PciFindExtCapOffsetRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ USHORT CapabilityID,
    _In_opt_ PCI_FIND_CAPS_CALLBACK Callback,
    _Inout_opt_ PVOID Context
    )

/*++

Routine Description:

    Searches PCIe config space for a specific capability.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    CapabilityID - Provides the ID of the capability to search for.

    Callback - Provides an optional callback function. This function is called
        on each CapabilityID match. Searching continues until either the
        callback returns TRUE or no further matches are found. This feature is
        useful for finding a specific instance of a capability which may occur
        more than once in config space.

    Context - Provides an optional context pointer to be passed into the
        callback.

Return Value:

    If found, the offset of the config record. Otherwise 0.

--*/

{

    PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER CapHeader;
    ULONG CapOffset;
    ULONG CapOffsetMax;
    ULONG CapOffsetMin;

    //
    // Determine starting offset and the min/max range from which to read.
    //

    CapOffset = 0x100;
    CapOffsetMin = 0x100;
    CapOffsetMax = 0x1000 - sizeof(PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER);

    //
    // Capabilities are implemented as a singly-linked list. An offset of 0
    // signifies the end of the list. Iterate over each entry until the end.
    //

    while (CapOffset != 0) {

        //
        // Validate that the offset is within the valid range and is on a
        // dword boundary.
        //

        if ((CapOffset < CapOffsetMin) ||
            (CapOffset > CapOffsetMax) ||
            ((CapOffset & 0x3) != 0)) {

            CapOffset = 0;
            break;
        }

        //
        // Read the config header and see if the ID matches.
        //

        PciReadConfigRaw(PciIF, Bus, Slot, &CapHeader, CapOffset, sizeof(CapHeader));

        //
        // Guard against searching for an ID on a device which doesn't support
        // extended config space.
        //

        if (CapHeader.CapabilityID == PCI_EXPRESS_INVALID_ENHANCED_CAP_ID) {
            CapOffset = 0;
            break;
        }

        if (CapHeader.CapabilityID == CapabilityID) {

            //
            // If no callback, return now.
            //

            if (Callback == NULL) {
                break;
            }

            //
            // If the callback returns TRUE, return now.
            //

            if (Callback(PciIF, Bus, Slot, CapOffset, Context) != FALSE) {
                break;
            }
        }

        CapOffset = CapHeader.Next;
    }

    return CapOffset;
}

USHORT
PciReadVendorIdRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Returns the Vendor ID field from the PCI config header.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    Vendor ID.

--*/

{

    USHORT VendorId = 0;

    PciReadConfigRegRaw(PciIF, Bus, Slot, &VendorId, VendorID);
    return VendorId;
}

BOOLEAN
PciIsDevicePresentRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Returns whether a device is present in the slot.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    TRUE if a device is present, FALSE otherwise.

--*/

{

    USHORT VendorId;

    VendorId = PciReadVendorIdRaw(PciIF, Bus, Slot);
    if ((VendorId == 0xFFFF) || (VendorId == 0)) {
        return FALSE;

    } else {
        return TRUE;
    }
}

BOOLEAN
PciIsDeviceABridge(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Returns whether a device is a bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    TRUE if a device is present, FALSE otherwise.

--*/

{

    UCHAR HeaderType;

    HeaderType = PciReadHeaderTypeRaw(PciIF, Bus, Slot);
    HeaderType &= ~PCI_MULTIFUNCTION;
    return ((HeaderType == PCI_BRIDGE_TYPE) || (HeaderType == PCI_CARDBUS_BRIDGE_TYPE));
}

BOOLEAN
PciIsDeviceMultifunction(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Returns whether a device is a multifunction.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    TRUE if a device is multifunction, FALSE otherwise.

--*/

{

    UCHAR HeaderType;

    HeaderType = PciReadHeaderTypeRaw(PciIF, Bus, Slot);
    return ((HeaderType & PCI_MULTIFUNCTION) != 0);
}

UCHAR
PciReadHeaderTypeRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Returns the type field from the PCI config header, which can be used to
    distinguish a bridge from a device.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    Header type field, one of:
        PCI_DEVICE_TYPE
        PCI_BRIDGE_TYPE
        PCI_CARDBUS_BRIDGE_TYPE

--*/

{

    UCHAR HeaderType = 0;

    PciReadConfigRegRaw(PciIF, Bus, Slot, &HeaderType, HeaderType);
    return HeaderType;
}


VOID
PciWriteBusNumsRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR Primary,
    _In_ UCHAR Secondary,
    _In_ UCHAR Subordinate
    )


/*++

Routine Description:

    Writes the primary, secondary, and subordinate bus numbers to a bridge.
    It is the callers responsibility to ensure that the device begin written to
    is a bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Primary - Provides the primary bus number.

    Secondary - Provides the secondary bus number.

    Subordinate - Provides the subordinate bus number.

Return Value:

    None.

--*/

{

    PciWriteConfigRegRaw(PciIF, Bus, Slot, &Primary, bridgeCommon.PrimaryBus);
    PciWriteConfigRegRaw(PciIF, Bus, Slot, &Secondary, bridgeCommon.SecondaryBus);
    PciWriteConfigRegRaw(PciIF, Bus, Slot, &Subordinate, bridgeCommon.SubordinateBus);
}

VOID
PciReadBusNumsRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_ UCHAR *Primary,
    _Out_ UCHAR *Secondary,
    _Out_ UCHAR *Subordinate
    )


/*++

Routine Description:

    Reads the primary, secondary, and subordinate bus numbers from a bridge.
    It is the callers responsibility to ensure that the device begin read from
    is a bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Primary - Provides the primary bus number.

    Secondary - Provides the secondary bus number.

    Subordinate - Provides the subordinate bus number.

Return Value:

    None.

--*/

{

    PciReadConfigRegRaw(PciIF, Bus, Slot, Primary, bridgeCommon.PrimaryBus);
    PciReadConfigRegRaw(PciIF, Bus, Slot, Secondary, bridgeCommon.SecondaryBus);
    PciReadConfigRegRaw(PciIF, Bus, Slot, Subordinate, bridgeCommon.SubordinateBus);
}

BOOLEAN
PciIsUnconfiguredBridge(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Determines if any of the bus numbers on a bridge have been configured.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Offset - Provides the offset to read from.

    Length - Provides the length of the buffer in bytes.

    Buffer - Provides a pointer to the buffer to read into.

Return Value:

    TRUE if the Primary, Secondary, and Subordinate IDs in the bridge are all 0.
    FALSE otherwise.

--*/


{

    UCHAR Primary;
    UCHAR Secondary;
    UCHAR Subordinate;

    PciReadBusNumsRaw(PciIF, Bus, Slot, &Primary, &Secondary, &Subordinate);
    if ((Primary == 0) && (Secondary == 0) && (Subordinate == 0)) {
        return TRUE;
    }

    return FALSE;
}


BOOLEAN
PciFindIntelTbVsecCallback(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG Offset,
    _Inout_opt_ PVOID Context
    )

/*++

Routine Description:

    Callback from PciFindExtCapOffsetRaw to determine if the Intel
    vendor-specific capability represents a Thunderbolt device.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Offset - Provides the offset of the capability.

    Context - Provides the context pointer.

Return Value:

    TRUE if the capability is for a Thunderbolt device, FALSE otherwise.

--*/

{

    PCI_EXPRESS_VENDOR_SPECIFIC_CAPABILITY VsecHeader;

    UNREFERENCED_PARAMETER(Context);

    PciReadConfigRaw(PciIF, Bus, Slot, &VsecHeader, Offset, sizeof(VsecHeader));
    if (VsecHeader.VsecId == PCI_EXPRESS_THUNDERBOLT_VSEC_ID) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
PciIsDeviceSlotImplementedRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG ExpressCapsOffset
    )

/*++

Routine Description:

    Determines if the device has slot implemented set.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    ExpressCapsOffset - Offset to the ExpressCaps structure, or 0 if this
        device is not a PCIe device.

Return Value:

    TRUE if this bridge supports hotplug, FALSE otherwise.

--*/


{

    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilities;

    //
    // Only look at PCIe ports.
    //

    if (ExpressCapsOffset == 0) {
        return FALSE;
    }

    //
    // Is the port a root or downstream switch port?
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &ExpressCapabilities,
                     ExpressCapsOffset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, ExpressCapabilities),
                     FIELD_SIZE(PCI_EXPRESS_CAPABILITY, ExpressCapabilities));

    if ((ExpressCapabilities.DeviceType != PciExpressRootPort) &&
        (ExpressCapabilities.DeviceType != PciExpressDownstreamSwitchPort)) {
        return FALSE;
    }

    //
    // Check if the slot capability is implemented.
    //

    if (ExpressCapabilities.SlotImplemented == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

BOOLEAN
PciIsDeviceHotplugCapableRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ ULONG ExpressCapsOffset
    )

/*++

Routine Description:

    Determines if the device appears to be a hotplug capable root or
    downstream switch port.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    ExpressCapsOffset - Offset to the ExpressCaps structure, or 0 if this
        device is not a PCIe device.

Return Value:

    TRUE if this bridge supports hotplug, FALSE otherwise.

--*/

{

    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilities;
    PCI_EXPRESS_SLOT_CAPABILITIES_REGISTER SlotCapabilities;

    //
    // Only look at PCIe ports.
    //

    if (ExpressCapsOffset == 0) {
        return FALSE;
    }

    //
    // Is the port a root or downstream switch port?
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &ExpressCapabilities,
                     ExpressCapsOffset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, ExpressCapabilities),
                     FIELD_SIZE(PCI_EXPRESS_CAPABILITY, ExpressCapabilities));

    if ((ExpressCapabilities.DeviceType != PciExpressRootPort) &&
        (ExpressCapabilities.DeviceType != PciExpressDownstreamSwitchPort)) {
        return FALSE;
    }

    //
    // If the slot capability isn't implemented, the HotPlugCapable bit doesn't
    // exist.
    //

    if (ExpressCapabilities.SlotImplemented == 0) {
        return FALSE;
    }

    //
    // Is it hotplug capable?
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &SlotCapabilities,
                     ExpressCapsOffset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, SlotCapabilities),
                     FIELD_SIZE(PCI_EXPRESS_CAPABILITY, SlotCapabilities));

    if (SlotCapabilities.HotPlugCapable == 0) {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
PciIsDeviceThunderboltRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Determines if the device appears to be a Thunderbolt bridge.

    N.B. The caller should have already verified that the device is hotplug
         capable by calling PciIsDeviceHotplugCapableRaw.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

Return Value:

    TRUE if this is a Thunderbolt device, FALSE otherwise.

--*/

{

    ULONG Offset;
    USHORT VendorId;

    //
    // If this is an Intel device, look for an Intel-specific Thunderbolt ID
    // in any of the vendor-specific capability records.
    //

    VendorId = PciReadVendorIdRaw(PciIF, Bus, Slot);
    if (VendorId != PCI_VENDOR_INTEL) {
        return FALSE;
    }

    Offset = PciFindExtCapOffsetRaw(PciIF,
                                    Bus,
                                    Slot,
                                    PCI_EXPRESS_VENDOR_SPECIFIC_CAP_ID,
                                    PciFindIntelTbVsecCallback,
                                    NULL);

    if (Offset != 0) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
PciTryEnableBridgeARIRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG DeviceBus,
    _In_ PCI_SLOT_NUMBER DeviceSlot,
    _In_ ULONG BridgeBus,
    _In_ PCI_SLOT_NUMBER BridgeSlot
    )

/*++

Routine Description:

    Determines if the specified bridge and device below it both support ARI,
    and if so enables ARI on the bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    DeviceBus - Provides the bus number of the device.

    DeviceSlot - Provides the device/function number of the device.

    BridgeBus - Provides the bus number of the bridge.

    BridgeSlot - Provides the device/function number of the bridge.

Return Value:

    TRUE if ARI was enabled, FALSE otherwise.

--*/

{
    PCI_EXPRESS_DEVICE_CAPABILITIES_2_REGISTER DeviceCapabilities2;
    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2;
    ULONG Offset;

    //
    // Determine if the device supports ARI by searching for the ARI
    // PCIe capability.
    //

    Offset = PciFindExtCapOffsetRaw(PciIF,
                                    DeviceBus,
                                    DeviceSlot,
                                    PCI_EXPRESS_ARI_CAP_ID,
                                    NULL,
                                    NULL);

    if (Offset == 0) {
        return FALSE;
    }

    //
    // Determine if the bridge supports ARI by examining the
    // AriForwardingSupported bit in the DeviceCapabilities2 PCIe capability.
    //

    Offset = PciFindCapOffsetRaw(PciIF,
                                 BridgeBus,
                                 BridgeSlot,
                                 PCI_CAPABILITY_ID_PCI_EXPRESS,
                                 NULL,
                                 NULL);

    if (Offset == 0) {
        return FALSE;
    }

    PciReadConfigRaw(PciIF,
                     BridgeBus,
                     BridgeSlot,
                     &DeviceCapabilities2,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceCapabilities2),
                     FIELD_SIZE(PCI_EXPRESS_CAPABILITY, DeviceCapabilities2));

    if (DeviceCapabilities2.AriForwardingSupported == 0) {
        return FALSE;
    }

    //
    // Determine if ARI forwarding is already enabled in the bridge, and enable
    // it if necessary.
    //

    PciReadConfigRaw(PciIF,
                     BridgeBus,
                     BridgeSlot,
                     &DeviceControl2,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceControl2),
                     FIELD_SIZE(PCI_EXPRESS_CAPABILITY, DeviceControl2));

    if (DeviceControl2.AriForwardingEnable == 0) {
        DeviceControl2.AriForwardingEnable = 1;
        PciWriteConfigRaw(PciIF,
                          BridgeBus,
                          BridgeSlot,
                          &DeviceControl2,
                          Offset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceControl2),
                          FIELD_SIZE(PCI_EXPRESS_CAPABILITY, DeviceControl2));
    }

    return TRUE;
}

ULONG
PciReadNextARIFunctionRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Reads the NextFunctionNumber field of the PCIe ARI Capability structure.
    This provides the Function number of the next implemented function in the
    hardware, avoiding the need to iterate over all 256 potential functions
    while enumerating ARI-enabled devcies.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number of the device.

    Slot - Provides the device/function number of the previously enumerated device.

Return Value:

    TRUE if ARI was enabled, FALSE otherwise.

--*/

{

    PCI_EXPRESS_ARI_CAPABILITY_REGISTER Capability;
    ULONG Offset;

    Offset = PciFindExtCapOffsetRaw(PciIF,
                                    Bus,
                                    Slot,
                                    PCI_EXPRESS_ARI_CAP_ID,
                                    NULL,
                                    NULL);

    if (Offset == 0) {
        return 0;
    }

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &Capability,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_ARI_CAPABILITY, Capability),
                     FIELD_SIZE(PCI_EXPRESS_ARI_CAPABILITY, Capability));

    return Capability.NextFunctionNumber;
}

UCHAR
PciSriovBusNumbersRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ BOOLEAN AriEnabled
    )

/*++

Routine Description:

    This function calculates how many additional bus numbers are needed to
    address all the VF's on an SR-IOV device (above and beyond the first bus
    number required to address the first 256 PFs/VFs).

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number of the device.

    Slot - Provides the device/function number of the previously enumerated device.

    AriEnabled - Provides a boolean specifying whether ARI is enabled in the
        parent bridge.

Return Value:

    The maximum number of bus numbers the device can consume.

--*/
{

    USHORT FirstVFOffset = 0;
    ULONG Offset;
    USHORT RoutingIdDelta;
    PCI_EXPRESS_SRIOV_CONTROL SRIOVControl;
    USHORT TotalVFs = 0;
    USHORT VFStride = 0;

    //
    // Search for the SR-IOV capability.
    //

    Offset = PciFindExtCapOffsetRaw(PciIF,
                                    Bus,
                                    Slot,
                                    PCI_EXPRESS_SINGLE_ROOT_IO_VIRTUALIZATION_CAP_ID,
                                    NULL,
                                    NULL);

    //
    // If no capability, then no additional bus numbers are required.
    //

    if (Offset == 0) {
        return 0;
    }

    //
    // The device may report different values depending on whether ARI is
    // enabled. If ARI is enabled, inform the SR-IOV capability by turning on
    // the ARICapableHierarchy bit.
    //

    if (AriEnabled != FALSE) {
        PciReadConfigRaw(PciIF,
                         Bus,
                         Slot,
                         &SRIOVControl,
                         Offset + FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, SRIOVControl),
                         FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, SRIOVControl));

        SRIOVControl.ARICapableHierarchy = TRUE;

        PciWriteConfigRaw(PciIF,
                          Bus,
                          Slot,
                          &SRIOVControl,
                          Offset + FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, SRIOVControl),
                          FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, SRIOVControl));
    }

    //
    // Now read the SR-IOV fields to determine the maximum function number that
    // may be used.
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &FirstVFOffset,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, FirstVFOffset),
                     FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, FirstVFOffset));

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &TotalVFs,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, TotalVFs),
                     FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, TotalVFs));

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     &VFStride,
                     Offset + FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, VFStride),
                     FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, VFStride));

    //
    // Assume that each additional bus number allocated may support up to 256
    // VFs, but that VFs below 256 will not require any additional bus numbers.
    //

    RoutingIdDelta = FirstVFOffset + ((TotalVFs - 1) * VFStride);
    return RoutingIdDelta / 256;
}

VOID
PciProbeConfigRegistersRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_writes_bytes_(Length) PVOID ReadBuffer,
    _Out_writes_bytes_(Length) PVOID ProbeBuffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )

/*++

Routine Description:

    Writes all 1's to the region of config space and then reads the result back.
    Also saves/restores the original contents of the region.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    ReadBuffer - Buffer which returns the original contents of the config
        region probed. Must be at least as long as Length.

    ProbeBuffer - Buffer which returns results of the probe operation. Must be
        at least as long as Length.

    Offset - Offset to the config space region to be probed.

    Length - Length in bytes of the region to be probed.

Return Value:

    None.

--*/

{

    //
    // Save off the original registers.
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     ReadBuffer,
                     Offset,
                     Length);

    //
    // Write all 1's.
    //

    RtlFillMemory(ProbeBuffer, Length, 0xFF);
    PciWriteConfigRaw(PciIF,
                      Bus,
                      Slot,
                      ProbeBuffer,
                      Offset,
                      Length);

    //
    // Read back.
    //

    PciReadConfigRaw(PciIF,
                     Bus,
                     Slot,
                     ProbeBuffer,
                     Offset,
                     Length);

    //
    // Restore the original contents.
    //

    PciWriteConfigRaw(PciIF,
                      Bus,
                      Slot,
                      ReadBuffer,
                      Offset,
                      Length);

    return;
}

VOID
PciReadConfigRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )

/*++

Routine Description:

    Reads PCI Config Space.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Buffer - Provides a pointer to the buffer to be read into.

    Offset - Provides the offset to read from.

    Length - Provides the length of the buffer.

Return Value:

    None.

--*/

{

    ULONG LengthRead;

    LengthRead = PciIF->ReadConfig(PciIF->Context,
                                   Bus,
                                   Slot.u.AsULONG,
                                   Buffer,
                                   Offset,
                                   Length);

    if (LengthRead != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Bus,                // Bus
                     (ULONG_PTR)Slot.u.AsULONG,     // Slot
                     (ULONG_PTR)Offset,             // Offset
                     (ULONG_PTR)TRUE                // Read
                     );
    }
}

VOID
PciWriteConfigRaw(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )

/*++

Routine Description:

    Writes PCI Config Space.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    Buffer - Provides a pointer to the buffer to be written from.

    Offset - Provides the offset to write to.

    Length - Provides the length of the buffer.

Return Value:

    None.

--*/

{

    ULONG LengthWritten;

    LengthWritten = PciIF->WriteConfig(PciIF->Context,
                                       Bus,
                                       Slot.u.AsULONG,
                                       Buffer,
                                       Offset,
                                       Length);

    if (LengthWritten != Length) {
        KeBugCheckEx(PCI_CONFIG_SPACE_ACCESS_FAILURE,
                     (ULONG_PTR)Bus,                // Bus
                     (ULONG_PTR)Slot.u.AsULONG,     // Slot
                     (ULONG_PTR)Offset,             // Offset
                     (ULONG_PTR)FALSE               // Read
                     );
    }
}
