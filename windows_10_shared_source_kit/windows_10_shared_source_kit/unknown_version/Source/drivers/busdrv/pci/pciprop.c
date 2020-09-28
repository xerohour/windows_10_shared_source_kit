/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pciprop.c

Abstract:

    This module contains routines for setting custom properties for PCI root
    bus and devices.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include "devpkey.h"
#include "pciprop.h"

// --------------------------------------------------------------------- Defines

//
// The CurrentSpeedAndMode is currently spec-defined as a 4-bit field.  Thus,
// valid values have a hard upper limit.
//

#define MAX_DEVICE_SPEED_AND_MODE 0xf

#define PCI_UPDATE_PROPERTY_DATA_AND_STATUS(_PropertyListEntry, _Data)   \
    (_PropertyListEntry)->Data = (_Data);                                \
    (_PropertyListEntry)->Status = STATUS_SUCCESS;

//
// This is a table containing a list of custom properties for a root bus and
// its attributes. The property data value is initialized to 0.
//

const PCI_PROPERTY_LIST_ENTRY PciRootBusDefaultPropertyList[] = {
    {&DEVPKEY_PciRootBus_SecondaryInterface,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_CurrentSpeedAndMode,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_SupportedSpeedsAndModes,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_DeviceIDMessagingCapable,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_SecondaryBusWidth,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_ExtendedConfigAvailable,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_ExtendedPCIConfigOpRegionSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_ASPMSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_ClockPowerManagementSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_PCISegmentGroupsSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_MSISupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_PCIExpressNativeHotPlugControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_SHPCNativeHotPlugControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_PCIExpressNativePMEControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_PCIExpressAERControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_PCIExpressCapabilityControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_NativePciExpressControl,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciRootBus_SystemMsiSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED}};

#define PCI_ROOT_PROPERTY_LIST_SIZE \
    RTL_NUMBER_OF(PciRootBusDefaultPropertyList)

// ---------------------------------------------------------------------- Device

//
// This is a table containing a list of custom properties for a PCI device and
// its attributes. The property data value is initialized to 0.
//

const PCI_PROPERTY_LIST_ENTRY PciDeviceDefaultPropertyList[] = {
    {&DEVPKEY_PciDevice_DeviceType,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_CurrentSpeedAndMode,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_BaseClass,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_SubClass,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_ProgIf,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_CurrentPayloadSize,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_MaxPayloadSize,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_MaxReadRequestSize,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_CurrentLinkSpeed,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_CurrentLinkWidth,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_MaxLinkSpeed,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_MaxLinkWidth,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_ExpressSpecVersion,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_InterruptSupport,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_InterruptMessageMaximum,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_BarTypes,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_AERCapabilityPresent,
      0,
      DEVPROP_TYPE_BOOLEAN,
      sizeof(BOOLEAN),
      FALSE,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_FirmwareErrorHandling,
      0,
      DEVPROP_TYPE_BOOLEAN,
      sizeof(BOOLEAN),
      FALSE,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_Uncorrectable_Error_Mask,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_Uncorrectable_Error_Severity,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_Correctable_Error_Mask,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_ECRC_Errors,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_Error_Reporting,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_RootError_Reporting,
      0,
      DEVPROP_TYPE_UINT32,
      sizeof(ULONG),
      0,
      STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_S0WakeupSupported,
      0,
      DEVPROP_TYPE_BOOLEAN,
      sizeof(BOOLEAN),
      FALSE,
      STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_SriovSupport,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_AcsSupport,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_AriSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     FALSE,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_AcsCompatibleUpHierarchy,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_AcsCapabilityRegister,
     0,
     DEVPROP_TYPE_UINT32,
     sizeof(ULONG),
     0,
     STATUS_NOT_SUPPORTED},

     {&DEVPKEY_PciDevice_AtsSupport,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_RequiresReservedMemoryRegion,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_RequiresReservedMemoryRegion,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     0,
     STATUS_NOT_SUPPORTED},

    {&DEVPKEY_PciDevice_AtomicsSupported,
     0,
     DEVPROP_TYPE_BOOLEAN,
     sizeof(BOOLEAN),
     0,
     STATUS_NOT_SUPPORTED}};

#define PCI_DEVICE_PROPERTY_LIST_SIZE \
    RTL_NUMBER_OF(PciDeviceDefaultPropertyList)

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciRootAssembleCustomProperties (
    __in PPCI_ROOT PciRoot,
    __inout PPCI_PROPERTY_LIST_ENTRY PciRootBusPropertyList
    );

NTSTATUS
PciDeviceAssembleCustomProperties (
    __in PPCI_DEVICE Device,
    __inout PPCI_PROPERTY_LIST_ENTRY PciDevicePropertyList
    );

NTSTATUS
PciBridgeAssembleCustomProperties (
    __in PPCI_DEVICE Device,
    __inout PPCI_PROPERTY_LIST_ENTRY PciDevicePropertyList
    );

NTSTATUS
PciGetDeviceTypeProperty (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceType
    );

NTSTATUS
PciGetCurrentDeviceSpeedAndMode (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceSpeedAndMode
    );

NTSTATUS
PciGetBridgeTypeProperty (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceType
    );

NTSTATUS
PciTranslateHostBridgeToDeviceSpeedAndMode (
    __in PPCI_DEVICE Device,
    __in ULONG BridgeSpeedAndMode,
    __out PULONG TranslatedSpeedAndMode
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciRootSetCustomProperties)
    #pragma alloc_text(PAGE, PciRootAssembleCustomProperties)
    #pragma alloc_text(PAGE, PciDeviceSetCustomProperties)
    #pragma alloc_text(PAGE, PciDeviceAssembleCustomProperties)
    #pragma alloc_text(PAGE, PciBridgeAssembleCustomProperties)
    #pragma alloc_text(PAGE, PciGetDeviceTypeProperty)
    #pragma alloc_text(PAGE, PciGetCurrentDeviceSpeedAndMode)
    #pragma alloc_text(PAGE, PciGetBridgeTypeProperty)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciRootSetCustomProperties (
    __in PPCI_ROOT PciRoot
    )

/*++

Routine Description:

    This routine is invoked to populate a set of custom properties for a root
    bus. It makes a local copy of the default root bus property list and
    updates this copy with the current properties of a root bus. If a property
    is successfully updated in the list, the status field is updated to reflect
    that. Thereafter the root bus property is made available to other components
    by invoking the custom property API.

Arguments:

    PciRoot - Supplies a pointer to a pci root structure for a root bus.

Return Value:

    STATUS_SUCCESS.

--*/

{

    ULONG Index;
    PCI_PROPERTY_LIST_ENTRY PciRootBusPropertyList[PCI_ROOT_PROPERTY_LIST_SIZE];
    PPCI_PROPERTY_LIST_ENTRY PropertyListEntry;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Make a local copy of the default root bus property list.
    //

    RtlCopyMemory(PciRootBusPropertyList,
                  PciRootBusDefaultPropertyList,
                  sizeof(PciRootBusDefaultPropertyList));

    //
    // Invoke a routine to set the current properties for the root bus.
    //

    PciRootAssembleCustomProperties(PciRoot, &PciRootBusPropertyList[0]);

    //
    // Now loop thru every entry in the property list and invoke the custom
    // property API to set the property.
    //

    for (Index = 0, PropertyListEntry = &PciRootBusPropertyList[0];
         Index < PCI_ROOT_PROPERTY_LIST_SIZE;
         Index++, PropertyListEntry++) {

        if (!NT_SUCCESS(PropertyListEntry->Status)) {
            continue;
        }

        Status = IoSetDevicePropertyData(PciRoot->Bus->PhysicalDeviceObject,
                                         PropertyListEntry->DevPropKey,
                                         LOCALE_NEUTRAL,
                                         PropertyListEntry->Flags,
                                         PropertyListEntry->DevPropType,
                                         PropertyListEntry->DataSize,
                                         &PropertyListEntry->Data);

        PCI_ASSERT(NT_SUCCESS(Status));
    }

    return STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(Status);
}

NTSTATUS
PciRootAssembleCustomProperties (
    __in PPCI_ROOT PciRoot,
    __inout PPCI_PROPERTY_LIST_ENTRY PciRootBusPropertyList
    )

/*++

Routine Description:

    This routine updates the root bus property list for custom property APIs.
    If a root bus property is not available/applicable, then no update is made
    to the data/status field in the property list entry. This will result in
    such property to not be set.

    If the property data field is updated, then the status is set to success.

Arguments:

    PciRoot - Supplies a pointer to a pci root structure for a root bus.

    PciRootBusPropertyList - Supplies a place holder for all the properties of
        a root bus.

Return Value:

    STATUS_SUCCESS.

--*/

{

    ULONG Index;
    PCI_ROOT_BUS_OSC_CONTROL_FIELD OscControlGranted;
    PCI_ROOT_BUS_OSC_SUPPORT_FIELD OscFeatureSupport;
    PPCI_PROPERTY_LIST_ENTRY PropertyListEntry;

    PAGED_CODE();

    OscFeatureSupport.u.AsULONG =
        PciRoot->HardwareCapability.OscFeatureSupport.u.AsULONG;

    OscControlGranted.u.AsULONG =
        PciRoot->HardwareCapability.OscControlGranted.u.AsULONG;

    for (Index = 0, PropertyListEntry = PciRootBusPropertyList;
         Index < PCI_ROOT_PROPERTY_LIST_SIZE;
         Index++, PropertyListEntry++) {

        switch (PropertyListEntry->DevPropKey->pid) {
        case 1: // DEVPKEY_PciRootBus_SecondaryInterface.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                PciRoot->HardwareCapability.SecondaryInterface);

            break;

        case 2: // DEVPKEY_PciRootBus_CurrentSpeedAndMode.pid

            //
            // This property is only applicable for conventional PCI and PCI-X
            // host bridges.
            //

            if ((PciRoot->HardwareCapability.SecondaryInterface !=
                 PciExpress) &&
                (PciRoot->HardwareCapability.BusCapabilitiesFound != FALSE)) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    PciRoot->HardwareCapability.CurrentSpeedAndMode);
            }

            break;

        case 3: // DEVPKEY_PciRootBus_SupportedSpeedsAndModes.pid

            //
            // This property is only applicable for conventional PCI and PCI-X
            // host bridges.
            //

            if ((PciRoot->HardwareCapability.SecondaryInterface !=
                 PciExpress) &&
                (PciRoot->HardwareCapability.BusCapabilitiesFound != FALSE)) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    PciRoot->HardwareCapability.SupportedSpeedsAndModes);
            }

            break;

        case 4: // DEVPKEY_PciRootBus_DeviceIDMessagingCapable.pid

            //
            // This property is only applicable for conventional PCI and PCI-X
            // host bridges.
            //

            if ((PciRoot->HardwareCapability.SecondaryInterface !=
                 PciExpress) &&
                (PciRoot->HardwareCapability.BusCapabilitiesFound != FALSE)) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ((PciRoot->HardwareCapability.DeviceIDMessagingCapable !=
                      0) ?
                     DEVPROP_TRUE :
                     DEVPROP_FALSE));

            }

            break;

        case 5: // DEVPKEY_PciRootBus_SecondaryBusWidth.pid

            //
            // This property is only applicable for conventional PCI and PCI-X
            // host bridges.
            //

            if ((PciRoot->HardwareCapability.SecondaryInterface !=
                 PciExpress) &&
                (PciRoot->HardwareCapability.BusCapabilitiesFound != FALSE)) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    PciRoot->HardwareCapability.SecondaryBusWidth);

            }

            break;

        case 6: // DEVPKEY_PciRootBus_ExtendedConfigAvailable.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((PciRoot->Bus->ExtendedConfigAvailable != FALSE) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 7: // DEVPKEY_PciRootBus_ExtendedPCIConfigOpRegionSupport.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscFeatureSupport.u.ExtendedConfigOpRegions != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 8: // DEVPKEY_PciRootBus_ASPMSupport.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscFeatureSupport.u.ActiveStatePowerManagement != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 9: //DEVPKEY_PciRootBus_ClockPowerManagementSupport.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscFeatureSupport.u.ClockPowerManagement != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 10: // DEVPKEY_PciRootBus_PCISegmentGroupsSupport.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscFeatureSupport.u.SegmentGroups != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 11: // DEVPKEY_PciRootBus_MSISupport.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscFeatureSupport.u.MessageSignaledInterrupts != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 12: // DEVPKEY_PciRootBus_PCIExpressNativeHotPlugControl.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscControlGranted.u.ExpressNativeHotPlug != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 13: // DEVPKEY_PciRootBus_SHPCNativeHotPlugControl.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscControlGranted.u.ShpcNativeHotPlug != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 14: // DEVPKEY_PciRootBus_PCIExpressNativePMEControl.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscControlGranted.u.ExpressNativePME != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 15: // DEVPKEY_PciRootBus_PCIExpressAERControl.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscControlGranted.u.ExpressAdvancedErrorReporting != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 16: // DEVPKEY_PciRootBus_PCIExpressCapabilityControl.pid
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                PropertyListEntry,
                ((OscControlGranted.u.ExpressCapabilityStructure != 0) ?
                 DEVPROP_TRUE :
                 DEVPROP_FALSE));

            break;

        case 17: // DEVPKEY_PciRootBus_NativePciExpressControl.pid
            if ((OscControlGranted.u.AsULONG != 0) ||
                (PciSystemWideHackFlags & PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES)) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_TRUE);

            } else {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_FALSE);
            }

            break;

        case 18: // DEVPKEY_PciRootBus_SystemMsiSupport.pid

            //
            // N.B. This is not really the best place to expose system MSI
            //      support.  It is expedient to add it here as it will be
            //      consumed alongside other MSI-related properties that are
            //      exposed by the PCI driver.  But it is primarily a test
            //      property and should be moved to the HAL or removed as soon
            //      as possible.

            if (PciSystemMsiEnabled) {

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_TRUE);

            } else {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_FALSE);
            }

            break;
			
        default:

            PCI_ASSERT(FALSE);

            return STATUS_SUCCESS;
        }
    }

    return STATUS_SUCCESS;
}

// ----------------------------------------------------------------- PCI Devices

NTSTATUS
PciDeviceSetCustomProperties (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine is invoked to populate a set of custom properties for a PCI
    device. It makes a local copy of the default PCI device property list and
    updates this copy with the current properties of the device. If a property
    is set successfully, then the status field in the table for that entry is
    updated to reflect that. Thereafter the device property is set by invoking
    a custom property API.

Arguments:

    Device - Supplies a pointer to a pci device.

Return Value:

    STATUS_SUCCESS.

--*/

{

    ULONG Index;
    PCI_PROPERTY_LIST_ENTRY PciDevicePropertyList[PCI_DEVICE_PROPERTY_LIST_SIZE];
    PPCI_PROPERTY_LIST_ENTRY PropertyListEntry;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Make a local copy of the default device property list.
    //

    RtlCopyMemory(PciDevicePropertyList,
                  PciDeviceDefaultPropertyList,
                  sizeof(PciDeviceDefaultPropertyList));

    //
    // Invoke a routine to update the local copy with the current properties of
    // a PCI device/bridge.
    //

    if (Device->HeaderType == PCI_DEVICE_TYPE) {
        PciDeviceAssembleCustomProperties(Device, &PciDevicePropertyList[0]);

    } else {
        PciBridgeAssembleCustomProperties(Device, &PciDevicePropertyList[0]);
    }

    //
    // Update the custom properties for other components.
    //

    for (Index = 0, PropertyListEntry = &PciDevicePropertyList[0];
         Index < PCI_DEVICE_PROPERTY_LIST_SIZE;
         Index++, PropertyListEntry++) {

        if (!NT_SUCCESS(PropertyListEntry->Status)) {
            continue;
        }

        Status = IoSetDevicePropertyData(Device->DeviceObject,
                                         PropertyListEntry->DevPropKey,
                                         LOCALE_NEUTRAL,
                                         PropertyListEntry->Flags,
                                         PropertyListEntry->DevPropType,
                                         PropertyListEntry->DataSize,
                                         &PropertyListEntry->Data);

        PCI_ASSERT(NT_SUCCESS(Status));
    }

    return STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(Status);
}

NTSTATUS
PciDeviceAssembleCustomProperties (
    __in PPCI_DEVICE Device,
    __inout PPCI_PROPERTY_LIST_ENTRY PciDevicePropertyList
    )

/*++

Routine Description:

    This routine updates the PCI device property list for custom property APIs.
    If a device property is not available/applicable, then no update is made
    to the data/status field in the property list entry. This will result in
    such property to not be set.

    If the property data field is updated, then the status is set to success.

Arguments:

    Device - Supplies a pointer to a pci device.

    PciDevicePropertyList - Supplies a place holder for all properties of a PCI
        device.

Return Value:

    STATUS_SUCCESS;

--*/

{

    PCI_EXPRESS_ACS_CAPABILITY_REGISTER AcsCapabilities;
    PPCI_EXPRESS_AER_CAPABILITY AerCapability;
    BOOLEAN AtomicsSupported;
    PIO_RESOURCE_DESCRIPTOR Bar;
    ULONG BarIndex;
    PCI_EXPRESS_AER_CAPABILITIES CapabilitiesAndControl;
    PEXPRESS_COMPATIBILITY_PORT CompatibilityPort;
    ULONG DeviceSpeedAndMode;
    ULONG DeviceType;
    PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;
    PEXPRESS_LINK ExpressLink;
    BOOLEAN ExpressLinkValid;
    PEXPRESS_PORT ExpressPort;
    HAL_IOMMU_RESERVED_REGION_INFORMATION HalIommuReservedRegion;
    PPCI_ROOT_BUS_HARDWARE_CAPABILITY HardwareCapability;
    ULONG Index;
    UCHAR IoBars;
    UCHAR NonPrefetchableBars;
    UCHAR Prefetchable32Bars;
    UCHAR Prefetchable64Bars;
    PPCI_PROPERTY_LIST_ENTRY PropertyListEntry;
    ULONG ReturnedLength;
    NTSTATUS Status;
    ULONG Support;
    BOOLEAN WakeupSupported;

    PAGED_CODE();

    PCI_ASSERT (Device->HeaderType == PCI_DEVICE_TYPE);

    ExpressLink = NULL;
    ExpressPort = Device->ExpressPort;
    if (ExpressPort != NULL) {
        ExpressLink = ExpressPort->ExpressLink;
    }

    CompatibilityPort = Device->ExpressCompatibilityPort;
    ExpressLinkValid = FALSE;
    if (CompatibilityPort != NULL) {
        ExpressLinkValid = CompatibilityPort->ExpressLinkValid;
    }

    HardwareCapability = &Device->Root->HardwareCapability;
    for (Index = 0, PropertyListEntry = PciDevicePropertyList;
         Index < PCI_DEVICE_PROPERTY_LIST_SIZE;
         Index++, PropertyListEntry++) {

        switch (PropertyListEntry->DevPropKey->pid) {
        case 1: // DEVPKEY_PciDevice_DeviceType
            Status = PciGetDeviceTypeProperty(Device, &DeviceType);
            if (NT_SUCCESS(Status)) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    DeviceType);

            }

            break;

        case 2: // DEVPKEY_PciDevice_CurrentSpeedAndMode
            Status = PciGetCurrentDeviceSpeedAndMode(Device,
                                                     &DeviceSpeedAndMode);

            if (NT_SUCCESS(Status)) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    DeviceSpeedAndMode);
            }

            break;

        case 3: // DEVPKEY_PciDevice_BaseClass
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->BaseClass);

            break;

        case 4: // DEVPKEY_PciDevice_SubClass
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->SubClass);

            break;

        case 5: // DEVPKEY_PciDevice_ProgIf
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->ProgIf);

            break;

        case 6: // DEVPKEY_PciDevice_CurrentPayloadSize
            if (ExpressPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressPort->DeviceControl.MaxPayloadSize);

            } else if (CompatibilityPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->DeviceControl.MaxPayloadSize);

            }

            break;

        case 7: // DEVPKEY_PciDevice_MaxPayloadSize
            if (ExpressPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressPort->DeviceCapabilities.MaxPayloadSizeSupported);

            } else if (CompatibilityPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->DeviceCapabilities.MaxPayloadSizeSupported);

            }

            break;

        case 8: // DEVPKEY_PciDevice_MaxReadRequestSize
            if (ExpressPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressPort->DeviceControl.MaxReadRequestSize);

            } else if (CompatibilityPort != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->DeviceControl.MaxReadRequestSize);

            }

            break;

        case 9: // DEVPKEY_PciDevice_CurrentLinkSpeed
            if (ExpressLink != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressLink->LinkStatus.LinkSpeed);

            } else if (ExpressLinkValid != FALSE) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->LinkStatus.LinkSpeed);

            }

            break;

        case 10: // DEVPKEY_PciDevice_CurrentLinkWidth
            if (ExpressLink != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressLink->LinkStatus.LinkWidth);

            } else if (ExpressLinkValid != FALSE) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->LinkStatus.LinkWidth);

            }

            break;

        case 11: // DEVPKEY_PciDevice_MaxLinkSpeed
            if (ExpressLink != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressLink->LinkCapabilities.MaximumLinkSpeed);

            } else if (ExpressLinkValid != FALSE) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->LinkCapabilities.MaximumLinkSpeed);

            }

            break;

        case 12: // DEVPKEY_PciDevice_MaxLinkWidth
            if (ExpressLink != NULL) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ExpressLink->LinkCapabilities.MaximumLinkWidth);

            } else if (ExpressLinkValid != FALSE) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CompatibilityPort->LinkCapabilities.MaximumLinkWidth);

            }

            break;

        case 13: // DEVPKEY_PciDevice_ExpressSpecVersion
            if (ExpressPort != NULL) {
                if (ExpressPort->BaseVersion11OrGreater) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_11);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_10);
                }

            } else if (ExpressLinkValid != FALSE) {
                if (CompatibilityPort->BaseVersion11OrGreater != FALSE) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_11);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_10);
                }
            }

            break;

        case 14: // DEVPKEY_PciDevice_InterruptSupport
            Support = 0;
            if (Device->InterruptRequirement.LineBased.Type ==
                CmResourceTypeInterrupt) {

                Support |= DevProp_PciDevice_InterruptType_LineBased;
            }

            if (Device->MsiCapability != 0) {
                Support |= DevProp_PciDevice_InterruptType_Msi;
            }

            if (Device->MsiXCapability != 0) {
                Support |= DevProp_PciDevice_InterruptType_MsiX;
            }

            if (Support != 0) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry, Support);
            }

            break;

        case 15: // DEVPKEY_PciDevice_InterruptMessageMaximum
            if (Device->InterruptRequirement.Message.Type != PciNone) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    Device->InterruptRequirement.Message.MessagesRequested);
            }

            break;

        case 16: // DEVPKEY_PciDevice_BarTypes
            IoBars = 0;
            NonPrefetchableBars = 0;
            Prefetchable32Bars = 0;
            Prefetchable64Bars = 0;
            for (BarIndex = 0; BarIndex < PCI_MAX_BAR_COUNT; BarIndex += 1) {
                Bar = &Device->Requirements.Bars[BarIndex];

                //
                // Get the type of BAR based on its resource descriptor.
                // Memory BARs have three types based on whether they are
                // prefetchable and whether they can live above 4GB.  The
                // distinction between 64 and 32 bit nonprefetchable BARs is
                // not made because in general, 64 bit nonprefetchable BARs must
                // be placed in the low 4GB of address space regardless of what
                // they support.  BARs that use the large resource type must be
                // 4GB or greater in size and as a result must be 64 bit BARs.
                //

                switch (Bar->Type) {
                case CmResourceTypePort:
                    IoBars += 1;
                    break;

                case CmResourceTypeMemory:
                    if (Bar->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
                        if (Bar->u.Generic.MaximumAddress.QuadPart > 0xFFFFFFFF) {
                            Prefetchable64Bars += 1;

                        } else {
                            Prefetchable32Bars += 1;
                        }

                    } else {
                        NonPrefetchableBars += 1;
                    }

                    break;

                case CmResourceTypeMemoryLarge:
                    if (Bar->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
                        Prefetchable64Bars += 1;

                    } else {
                        NonPrefetchableBars += 1;
                    }

                    break;

                default:
                    break;
                }
            }

            Support = IoBars |
                      (NonPrefetchableBars << 8) |
                      (Prefetchable32Bars << 16) |
                      (Prefetchable64Bars << 24);

            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry, Support);
            break;

        case 17: // DEVPKEY_PciDevice_AERCapabilityPresent
            if (ExpressPort != NULL) {
                if (ExpressPort->AerCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }

            } else if (CompatibilityPort != NULL) {
                if (CompatibilityPort->AerCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 18: // DEVPKEY_PciDevice_FirmwareErrorHandling
            if ((HardwareCapability->OscControlGranted.u.ExpressAdvancedErrorReporting != 0) &&
                (ExpressPort != NULL)) {

                if ((ExpressPort->ErrorSource.Flags &
                     WHEA_ERROR_SOURCE_FLAG_FIRMWAREFIRST) != 0) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 19: // DEVPKEY_PciDevice_Uncorrectable_Error_Mask
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->UncorrectableErrorMask.AsULONG);
            }

            break;

        case 20: // DEVPKEY_PciDevice_Uncorrectable_Error_Severity
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->UncorrectableErrorSeverity.AsULONG);
            }

            break;

        case 21: // DEVPKEY_PciDevice_Correctable_Error_Mask
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->CorrectableErrorMask.AsULONG);
            }

            break;

        case 22: // DEVPKEY_PciDevice_ECRC_Errors
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                CapabilitiesAndControl.AsULONG = 0;
                CapabilitiesAndControl.ECRCGenerationCapable =
                    AerCapability->CapabilitiesAndControl.ECRCGenerationCapable;

                CapabilitiesAndControl.ECRCGenerationEnable =
                    AerCapability->CapabilitiesAndControl.ECRCGenerationEnable;

                CapabilitiesAndControl.ECRCCheckCapable =
                    AerCapability->CapabilitiesAndControl.ECRCCheckCapable;

                CapabilitiesAndControl.ECRCCheckEnable =
                    AerCapability->CapabilitiesAndControl.ECRCCheckEnable;

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CapabilitiesAndControl.AsULONG);
            }

            break;

        case 23: // DEVPKEY_PciDevice_Error_Reporting
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                ErrorSource = &ExpressPort->ErrorSource;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ErrorSource->Info.AerEndpointDescriptor.DeviceControl);
            }

            break;

        case 24: // DEVPKEY_PciDevice_RootError_Reporting

            //
            // This property is only applicable to root ports.
            //

            break;

        case 25: // DEVPKEY_PciDevice_S0WakeupSupported

            //
            // PME is supported while the system is active on PCI devices, and
            // on PCIe devices operating in native mode.  Many machines do not
            // properly contain the firmware support for PCIe runtime PME in
            // legacy mode.  Only enable runtime wake in this case if the system
            // explicitly claims to support it.
            //

            WakeupSupported = TRUE;
            if (CompatibilityPort != NULL) {
                if (PciEnableLegacyPcieRuntimePme == 0) {
                    WakeupSupported = FALSE;
                }
            }

            if (WakeupSupported != FALSE) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_TRUE);

            } else {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    (ULONG)DEVPROP_FALSE);
            }

            break;

        case 26: // DEVPKEY_PciDevice_SriovSupport

            if (ExpressPort != NULL && ExpressPort->SriovCapability != 0) {
                Status = PciVirtualizationCheckAcsSupport(Device);
                if (!NT_SUCCESS(Status)) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciDevice_SriovSupport_MissingAcs);

                } else if (ExpressPort->FirstVFOffset >> 8 >
                           Device->Parent->CapturedBusNumberCount) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciDevice_SriovSupport_NoBusResource);

                } else if (PciIsSriovDriverAvailable(Device) == FALSE) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciDevice_SriovSupport_MissingPfDriver);

                } else if (Device->SriovDeviceDidntGetVfBarSpace != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciDevice_SriovSupport_DidntGetVfBarSpace);
                
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciDevice_SriovSupport_Ok);
                }
            }

            break;

        case 29: // DEVPKEY_PciDevice_AcsSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->AcsCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_Present);
                } else if (ExpressPort->DeviceType == PciExpressRootPort &&
                           PciIsDeviceFeatureEnabled(Device, PCI_FEATURE_ACS_NOT_REQUIRED) == PciFeatureStateEnabled) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_NotNeeded)

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_Missing);
                }
            }

            break;

        case 30: // DEVPKEY_PciDevice_AriSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->AriCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 31: // DEVPKEY_PciDevice_AcsCompatibleUpHierarchy
            if (ExpressPort != NULL){
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                    ExpressCheckAcsHierarchyCompatibility(ExpressPort));
            }

            break;

        case 32: // DEVPKEY_PciDevice_AcsCapabilityRegister
            if (ExpressPort != NULL){
                AcsCapabilities.AsUSHORT = 0;
                if (ExpressPort->AcsCapability){
                    PciReadAcsRegister(ExpressPort, Capability, &AcsCapabilities);
                }

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                    (ULONG)AcsCapabilities.AsUSHORT);
            }

            break;

        case 33: // DEVPKEY_PciDevice_AtsSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->AtsCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 34: // DEVPKEY_PciDevice_RequiresReservedMemoryRegion
                HalIommuReservedRegion.DeviceObject = Device->DeviceObject;
                HalIommuReservedRegion.RequiresReservedRegion = FALSE;
                ReturnedLength = sizeof(HalIommuReservedRegion);
                Status = HalQuerySystemInformation(HalQueryIommuReservedRegionInformation,
                                                   sizeof(HalIommuReservedRegion),
                                                   &HalIommuReservedRegion,
                                                   &ReturnedLength);

                if (NT_SUCCESS(Status) && 
                    HalIommuReservedRegion.RequiresReservedRegion != FALSE) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_FALSE);
                }

            break;

        case 35: // DEVPKEY_PciDevice_AtomicsSupported
            if (ExpressPort != NULL) {
                Status = PciDeviceQueryAtomics(ExpressPort, &AtomicsSupported);
                if (!NT_SUCCESS(Status) || AtomicsSupported == FALSE) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_FALSE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_TRUE);
                }
            }

            break;

        default:

            PCI_ASSERT(FALSE);

            return STATUS_SUCCESS;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciBridgeAssembleCustomProperties (
    __in PPCI_DEVICE Device,
    __inout PPCI_PROPERTY_LIST_ENTRY PciDevicePropertyList
    )

/*++

Routine Description:

    This routine updates the PCI bridge property list for custom property APIs.
    If a bridge property is not available/applicable, then no update is made
    to the data/status field in the property list entry. This will result in
    such property to not be set.

    If the property data field is updated, then the status is set to success.

Arguments:

    Device - Supplies a pointer to a pci bridge device.

    PciDevicePropertyList - Supplies a place holder for all properties of a PCI
        bridge.

Return Value:

    STATUS_SUCCESS;

--*/

{

    PCI_EXPRESS_ACS_CAPABILITY_REGISTER AcsCapabilities;
    PPCI_EXPRESS_AER_CAPABILITY AerCapability;
    ULONG BridgeType;
    PCI_EXPRESS_AER_CAPABILITIES CapabilitiesAndControl;
    PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;
    PEXPRESS_PORT ExpressPort;
    HAL_IOMMU_RESERVED_REGION_INFORMATION HalIommuReservedRegion;
    PPCI_ROOT_BUS_HARDWARE_CAPABILITY HardwareCapability;
    ULONG Index;
    PPCI_PROPERTY_LIST_ENTRY PropertyListEntry;
    ULONG ReturnedLength;
    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT(Device->HeaderType != PCI_DEVICE_TYPE);

    ExpressPort = Device->ExpressPort;
    HardwareCapability = &Device->Root->HardwareCapability;
    for (Index = 0, PropertyListEntry = PciDevicePropertyList;
         Index < PCI_DEVICE_PROPERTY_LIST_SIZE;
         Index++, PropertyListEntry++) {

        switch (PropertyListEntry->DevPropKey->pid) {
        case 1: // DEVPKEY_PciDevice_DeviceType
            Status = PciGetBridgeTypeProperty(Device, &BridgeType);
            if (NT_SUCCESS(Status)) {
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                    BridgeType);

            }

            break;

        case 3: // DEVPKEY_PciDevice_BaseClass
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->BaseClass);

            break;

        case 4: // DEVPKEY_PciDevice_SubClass
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->SubClass);

            break;

        case 5: // DEVPKEY_PciDevice_ProgIf
            PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                Device->ProgIf);

            break;

        case 13: // DEVPKEY_PciDevice_ExpressSpecVersion
            if (ExpressPort != NULL) {
                if (ExpressPort->BaseVersion11OrGreater) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_11);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        DevProp_PciExpressDevice_Spec_Version_10);
                }
            }

            break;

        case 17: // DEVPKEY_PciDevice_AERCapabilityPresent
            if (ExpressPort != NULL) {
                if (ExpressPort->AerCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 18: // DEVPKEY_PciDevice_FirmwareErrorHandling
            if ((HardwareCapability->OscControlGranted.u.ExpressAdvancedErrorReporting != 0) &&
                (ExpressPort != NULL)) {

                if ((ExpressPort->ErrorSource.Flags &
                     WHEA_ERROR_SOURCE_FLAG_FIRMWAREFIRST) != 0) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 19: // DEVPKEY_PciDevice_Uncorrectable_Error_Mask
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->UncorrectableErrorMask.AsULONG);
            }

            break;

        case 20: // DEVPKEY_PciDevice_Uncorrectable_Error_Severity
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->UncorrectableErrorSeverity.AsULONG);
            }

            break;

        case 21: // DEVPKEY_PciDevice_Correctable_Error_Mask
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    AerCapability->CorrectableErrorMask.AsULONG);
            }

            break;

        case 22: // DEVPKEY_PciDevice_ECRC_Errors
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                AerCapability = &ExpressPort->AerCapabilityBlock;
                CapabilitiesAndControl.AsULONG = 0;
                CapabilitiesAndControl.ECRCGenerationCapable =
                    AerCapability->CapabilitiesAndControl.ECRCGenerationCapable;

                CapabilitiesAndControl.ECRCGenerationEnable =
                    AerCapability->CapabilitiesAndControl.ECRCGenerationEnable;

                CapabilitiesAndControl.ECRCCheckCapable =
                    AerCapability->CapabilitiesAndControl.ECRCCheckCapable;

                CapabilitiesAndControl.ECRCCheckEnable =
                    AerCapability->CapabilitiesAndControl.ECRCCheckEnable;

                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    CapabilitiesAndControl.AsULONG);
            }

            break;

        case 23: // DEVPKEY_PciDevice_Error_Reporting
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                ErrorSource = &ExpressPort->ErrorSource;
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                    PropertyListEntry,
                    ErrorSource->Info.AerEndpointDescriptor.DeviceControl);
            }

            break;

        case 24: // DEVPKEY_PciDevice_RootError_Reporting
            if (PCI_DEVICE_CONFIGURED_FOR_AER(Device)) {
                if (ExpressPort->DeviceType == PciExpressRootPort) {
                    ErrorSource = &ExpressPort->ErrorSource;
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(
                        PropertyListEntry,
                        ErrorSource->Info.AerRootportDescriptor.RootErrorCommand);
                }
            }

            break;

        case 29: // DEVPKEY_PciDevice_AcsSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->AcsCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_Present);
                } else if (ExpressPort->DeviceType == PciExpressRootPort &&
                           PciIsDeviceFeatureEnabled(Device, PCI_FEATURE_ACS_NOT_REQUIRED) == PciFeatureStateEnabled) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_NotNeeded)

                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DevProp_PciDevice_AcsSupport_Missing);
                }
            }

            break;

        case 30: // DEVPKEY_PciDevice_AriSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->DeviceCapabilities2.AriForwardingSupported == TRUE) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                                                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 31: // DEVPKEY_PciDevice_AcsCompatibleUpHierarchy
            if (ExpressPort != NULL){
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                    ExpressCheckAcsHierarchyCompatibility(ExpressPort));
            }

            break;

        case 32: // DEVPKEY_PciDevice_AcsCapabilityRegister
            if (ExpressPort != NULL){
                AcsCapabilities.AsUSHORT = 0;
                if (ExpressPort->AcsCapability){
                    PciReadAcsRegister(ExpressPort, Capability, &AcsCapabilities);
                }
                
                PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                    (ULONG)AcsCapabilities.AsUSHORT);
            }

            break;

        case 33: // DEVPKEY_PciDevice_AtsSupport
            if (ExpressPort != NULL) {
                if (ExpressPort->AtsCapability != 0) {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_FALSE);
                }
            }

            break;

        case 34: // DEVPKEY_PciDevice_RequiresReservedMemoryRegion
                HalIommuReservedRegion.DeviceObject = Device->DeviceObject;
                HalIommuReservedRegion.RequiresReservedRegion = FALSE;
                ReturnedLength = sizeof(HalIommuReservedRegion);
                Status = HalQuerySystemInformation(HalQueryIommuReservedRegionInformation,
                                                   sizeof(HalIommuReservedRegion),
                                                   &HalIommuReservedRegion,
                                                   &ReturnedLength);

                if (NT_SUCCESS(Status) && 
                    HalIommuReservedRegion.RequiresReservedRegion != FALSE) {

                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_TRUE);
                } else {
                    PCI_UPDATE_PROPERTY_DATA_AND_STATUS(PropertyListEntry,
                        (ULONG)DEVPROP_FALSE);
                }

            break;

        default:
            break;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetDeviceTypeProperty (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceType
    )

/*++

Routine Description:

    This routine is invoked to obtain the type of a PCI device.

Arguments:

    Device - Supplies a pointer to a pci device.

    DeviceType - Supplies a place holder for a PCI device type property.

Return Value:

    STATUS_SUCCESS;

--*/

{

    PEXPRESS_PORT ExpressPort;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    ExpressPort = Device->ExpressPort;
    if (ExpressPort != NULL) {
        switch (ExpressPort->DeviceType) {
        case PciExpressEndpoint:
            *DeviceType = DevProp_PciDevice_DeviceType_PciExpressEndpoint;
            break;

        case PciExpressLegacyEndpoint:
            *DeviceType = DevProp_PciDevice_DeviceType_PciExpressLegacyEndpoint;
            break;

        case PciExpressRootComplexIntegratedEndpoint:
            *DeviceType =
                DevProp_PciDevice_DeviceType_PciExpressRootComplexIntegratedEndpoint;

            break;

        case PciExpressRootComplexEventCollector:
            *DeviceType =
                DevProp_PciDevice_BridgeType_PciExpressEventCollector;

            break;

        default:

            PCI_ASSERT(FALSE);

            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        return Status;
    }

    if (Device->ExpressCapability != 0) {
        *DeviceType = DevProp_PciDevice_DeviceType_PciExpressTreatedAsPci;

    } else if (Device->PciXCapability == 0) {
        *DeviceType = DevProp_PciDevice_DeviceType_PciConventional;

    } else {
        *DeviceType = DevProp_PciDevice_DeviceType_PciX;
    }

    return Status;
}

NTSTATUS
PciGetCurrentDeviceSpeedAndMode (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceSpeedAndMode
    )

/*++

Routine Description:

    This routine is invoked to obtain the current speed and operating mode of a
    PCI device. The device speed/mode is derived from its parent. Depending on
    the type of device and its location, this property is gleaned from different
    locations on the parent root bus/bridge. This property is not valid for
    Express devices.

Arguments:

    Device - Supplies a pointer to a pci device.

    DeviceSpeedAndMode - Supplies a placeholder to contain the operating speed
    and mode of a PCI device.

Return Value:

    STATUS_SUCCESS on success.

    STATUS_UNSUCCESSFUL if the current speed and mode is not available.

--*/

{

    PPCI_BRIDGE Bridge;
    PCIX_BRIDGE_CAPABILITY PciXBridgeCapability;
    USHORT SecondaryBridgeStatus;
    NTSTATUS Status;
    ULONG StatusOffset;

    PAGED_CODE();

    //
    // This property is not valid for express device. So return failure to
    // indicate so.
    //

    if (Device->ExpressPort != NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // If a PCI device is a child of a root bus, then the operating speed of a
    // device is same as that of the root bus (obtained by evaluating the
    // _DSM method).
    //

    if (PCI_ROOT_BUS(Device->Parent)) {
        Status = PciTranslateHostBridgeToDeviceSpeedAndMode(
                    Device,
                    Device->Root->HardwareCapability.CurrentSpeedAndMode,
                    DeviceSpeedAndMode);

        return Status;
    }

    Bridge = Device->Parent->PciBridge;

    //
    // The secondary side of a cardbus card operates at 33MHZ. Return success
    // as the property is already initialized.
    //

    if (Bridge->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {
        *DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_Pci_Conventional_33MHz;

        return STATUS_SUCCESS;
    }

    //
    // If this is a PCI-X device then read the secondary status register from
    // the PCI-X bridge capability.
    //

    if ((Device->PciXCapability != 0) &&
        (Bridge->PciXCapability != 0)) {

        StatusOffset = FIELD_OFFSET(PCIX_BRIDGE_CAPABILITY,
                                    SecondaryStatus);

        PciReadBridgeConfig(Bridge,
                            (PUCHAR)&PciXBridgeCapability + StatusOffset,
                            Bridge->PciXCapability + StatusOffset,
                            sizeof(USHORT));

        *DeviceSpeedAndMode =
            PciXBridgeCapability.SecondaryStatus.BusModeFrequency;

        return STATUS_SUCCESS;

    }

    //
    // If this is a conventional PCI device then read the secondary status
    // register from the standard type 1 config space.
    //

    PciReadBridgeConfigRegister(Bridge,
                                type1.SecondaryStatus,
                                &SecondaryBridgeStatus);

    if (SecondaryBridgeStatus & PCI_STATUS_66MHZ_CAPABLE) {
        *DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_Pci_Conventional_66MHz;

    } else {
        *DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_Pci_Conventional_33MHz;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetBridgeTypeProperty (
    __in PPCI_DEVICE Device,
    __out PULONG DeviceType
    )

/*++

Routine Description:

    This routine is invoked to obtain the type of a PCI bridge.

Arguments:

    Device - Supplies a pointer to a pci device for a bridge.

    DeviceType - Supplies a place holder for a PCI bridge type property.

Return Value:

    STATUS_SUCCESS;

--*/

{

    PEXPRESS_PORT ExpressPort;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    ExpressPort = Device->ExpressPort;
    if (ExpressPort != NULL) {
        switch (ExpressPort->DeviceType) {
        case PciExpressRootPort:
            *DeviceType = DevProp_PciDevice_BridgeType_PciExpressRootPort;
            break;

        case PciExpressUpstreamSwitchPort:
            *DeviceType =
                DevProp_PciDevice_BridgeType_PciExpressUpstreamSwitchPort;

            break;

        case PciExpressDownstreamSwitchPort:
            *DeviceType =
                DevProp_PciDevice_BridgeType_PciExpressDownstreamSwitchPort;

            break;

        case PciExpressToPciXBridge:
            *DeviceType =
                DevProp_PciDevice_BridgeType_PciExpressToPciXBridge;

            break;

        case PciXToExpressBridge:
            *DeviceType =
                DevProp_PciDevice_BridgeType_PciXToExpressBridge;

            break;

        default:

            PCI_ASSERT(FALSE);

            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        return Status;
    }

    if (Device->ExpressCapability != 0) {
        *DeviceType = DevProp_PciDevice_BridgeType_PciExpressTreatedAsPci;

    } else if (Device->PciXCapability == 0) {
        *DeviceType = DevProp_PciDevice_BridgeType_PciConventional;

    } else {
        *DeviceType = DevProp_PciDevice_BridgeType_PciX;
    }

    return Status;
}

NTSTATUS
PciTranslateHostBridgeToDeviceSpeedAndMode (
    __in PPCI_DEVICE Device,
    __in ULONG HostBridgeSpeedAndMode,
    __out PULONG TranslatedSpeedAndMode
    )

/*++

Routine Description:

    This routine translates the encoded speed and mode value used by host
    bridges to that used by the given PCI device.

Arguments:

    Device - Supplies a pointer to the PCI device.

    HostBridgeSpeedAndMode - Supplies the encoded speed and mode value used
        by host bridges.

    TranslatedSpeedAndMode - Supplies a pointer to a variable that receives the
        translated speed and mode.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL when unsuccessful.

--*/

{

    ULONG DeviceSpeedAndMode;
    NTSTATUS Status;

    switch (HostBridgeSpeedAndMode) {
    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_Conventional_33Mhz:
        DeviceSpeedAndMode =
            (Device->PciXCapability == 0) ?
            DevProp_PciDevice_CurrentSpeedAndMode_Pci_Conventional_33MHz :
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode_Conventional_Pci;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_Conventional_66Mhz:
        DeviceSpeedAndMode =
            (Device->PciXCapability == 0) ?
            DevProp_PciDevice_CurrentSpeedAndMode_Pci_Conventional_66MHz :
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode_Conventional_Pci;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_66Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_66Mhz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_100Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_100Mhz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_133Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_133MHZ;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_ECC_66Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_66Mhz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_ECC_100Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_100Mhz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_Mode1_ECC_133Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode1_133MHZ;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_266_Mode2_66Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_266_66MHz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_266_Mode2_100Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_266_100MHz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_266_Mode2_133Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_266_133MHz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_533_Mode2_66Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_533_66MHz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_533_Mode2_100Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_533_100MHz;

        break;

    case DevProp_PciRootBus_CurrentSpeedAndMode_Pci_X_533_Mode2_133Mhz:
        DeviceSpeedAndMode =
            DevProp_PciDevice_CurrentSpeedAndMode_PciX_Mode2_533_133MHz;

        break;

    default:
        DeviceSpeedAndMode = HostBridgeSpeedAndMode;
        break;
    }

    if (DeviceSpeedAndMode <= MAX_DEVICE_SPEED_AND_MODE) {
        *TranslatedSpeedAndMode = DeviceSpeedAndMode;
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_UNSUCCESSFUL;
    }

    return Status;
}
