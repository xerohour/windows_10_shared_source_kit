/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    devpnp.c

Abstract:

    This module handles IRPs for PCI PDO's.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "devpnp.tmh"

// --------------------------------------------------------------------- Defines

#define PCI_MAX_ID_LENGTH 256

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciDevice_Start(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_Remove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_SurpriseRemoval(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_CancelRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_Stop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_CancelStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_Eject(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryDeviceRelations(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryCapabilities(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryInterface(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryResources(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryResourceRequirements(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryDeviceText(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_ReadConfig(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_WriteConfig(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryId(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryBusInformation(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_DeviceUsageNotification(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_QueryDeviceState(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_DeviceEnumerated(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciQueryEjectionRelations(
    __in PPCI_DEVICE Device,
    __inout PDEVICE_RELATIONS *PDeviceRelations
    );

NTSTATUS
PciQueryTargetDeviceRelations(
    _In_ PPCI_DEVICE Device,
    _Inout_ PDEVICE_RELATIONS *PDeviceRelations
    );

NTSTATUS
PciQueryPowerCapabilities(
    __in PPCI_DEVICE Device,
    __inout PDEVICE_CAPABILITIES Capabilities
    );

NTSTATUS
PciDetermineSlotNumber(
    __in PPCI_DEVICE Device,
    __inout PULONG SlotNumber
    );

NTSTATUS
PciDecodesFromDeviceResources(
    __in PPCI_DEVICE Device,
    __out PUSHORT Decodes
    );

// --------------------------------------------------------------------- Pragmas

#pragma prefast(disable:__WARNING_UNUSED_SCALAR_ASSIGNMENT, "checked builds")

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciDevice_Start)
    #pragma alloc_text(PAGE, PciDevice_QueryRemove)
    #pragma alloc_text(PAGE, PciDevice_SurpriseRemoval)
    #pragma alloc_text(PAGE, PciDevice_CancelRemove)
    #pragma alloc_text(PAGE, PciDevice_Stop)
    #pragma alloc_text(PAGE, PciDevice_QueryStop)
    #pragma alloc_text(PAGE, PciDevice_CancelStop)
    #pragma alloc_text(PAGE, PciDevice_Eject)
    #pragma alloc_text(PAGE, PciDevice_QueryDeviceRelations)
    #pragma alloc_text(PAGE, PciDevice_QueryInterface)
    #pragma alloc_text(PAGE, PciDevice_QueryCapabilities)
    #pragma alloc_text(PAGE, PciDevice_QueryResources)
    #pragma alloc_text(PAGE, PciDevice_QueryResourceRequirements)
    #pragma alloc_text(PAGE, PciDevice_QueryDeviceText)
    #pragma alloc_text(PAGE, PciDevice_ReadConfig)
    #pragma alloc_text(PAGE, PciDevice_WriteConfig)
    #pragma alloc_text(PAGE, PciDevice_QueryId)
    #pragma alloc_text(PAGE, PciDevice_QueryBusInformation)
    #pragma alloc_text(PAGE, PciDevice_DeviceUsageNotification)
    #pragma alloc_text(PAGE, PciDevice_QueryDeviceState)
    #pragma alloc_text(PAGE, PciDevice_DeviceEnumerated)
    #pragma alloc_text(PAGE, PciQueryPowerCapabilities)
    #pragma alloc_text(PAGE, PciQueryTargetDeviceRelations)
    #pragma alloc_text(PAGE, PciQueryEjectionRelations)
    #pragma alloc_text(PAGE, PciDetermineSlotNumber)
    #pragma alloc_text(PAGE, PciDecodesFromDeviceResources)
#endif

// Disable warning C4100: 'X' : unreferenced formal parameter
#pragma warning(disable:4100)

// --------------------------------------------------------------------- Globals

const PCI_MN_DISPATCH_ENTRY PciDevice_DispatchPnpTable[PCI_MAX_MINOR_PNP_IRP + 2] = {
    { IRP_COMPLETE, PciDevice_Start                     , TRUE }, // 0x00 - IRP_MN_START_DEVICE
    { IRP_COMPLETE, PciDevice_QueryRemove               , FALSE}, // 0x01 - IRP_MN_QUERY_REMOVE_DEVICE
    { IRP_COMPLETE, PciDevice_Remove                    , TRUE }, // 0x02 - IRP_MN_REMOVE_DEVICE
    { IRP_COMPLETE, PciDevice_CancelRemove              , FALSE}, // 0x03 - IRP_MN_CANCEL_REMOVE_DEVICE
    { IRP_COMPLETE, PciDevice_Stop                      , TRUE }, // 0x04 - IRP_MN_STOP_DEVICE
    { IRP_COMPLETE, PciDevice_QueryStop                 , FALSE}, // 0x05 - IRP_MN_QUERY_STOP_DEVICE
    { IRP_COMPLETE, PciDevice_CancelStop                , FALSE}, // 0x06 - IRP_MN_CANCEL_STOP_DEVICE
    { IRP_COMPLETE, PciDevice_QueryDeviceRelations      , FALSE}, // 0x07 - IRP_MN_QUERY_DEVICE_RELATIONS
    { IRP_COMPLETE, PciDevice_QueryInterface            , FALSE}, // 0x08 - IRP_MN_QUERY_INTERFACE
    { IRP_COMPLETE, PciDevice_QueryCapabilities         , FALSE}, // 0x09 - IRP_MN_QUERY_CAPABILITIES
    { IRP_COMPLETE, PciDevice_QueryResources            , TRUE }, // 0x0A - IRP_MN_QUERY_RESOURCES
    { IRP_COMPLETE, PciDevice_QueryResourceRequirements , TRUE }, // 0x0B - IRP_MN_QUERY_RESOURCE_REQUIREMENTS
    { IRP_COMPLETE, PciDevice_QueryDeviceText           , FALSE}, // 0x0C - IRP_MN_QUERY_DEVICE_TEXT
    { IRP_COMPLETE, PciIrpNotSupported                  , FALSE}, // 0x0D - IRP_MN_FILTER_RESOURCE_REQUIREMENTS
    { IRP_COMPLETE, PciIrpNotSupported                  , FALSE}, // 0x0E - NOT USED
    { IRP_COMPLETE, PciDevice_ReadConfig                , TRUE }, // 0x0F - IRP_MN_READ_CONFIG
    { IRP_COMPLETE, PciDevice_WriteConfig               , TRUE }, // 0x10 - IRP_MN_WRITE_CONFIG
    { IRP_COMPLETE, PciDevice_Eject                     , TRUE }, // 0x11 - IRP_MN_EJECT
    { IRP_COMPLETE, PciIrpNotSupported                  , FALSE}, // 0x12 - IRP_MN_SET_LOCK
    { IRP_COMPLETE, PciDevice_QueryId                   , FALSE}, // 0x13 - IRP_MN_QUERY_ID
    { IRP_COMPLETE, PciDevice_QueryDeviceState          , FALSE}, // 0x14 - IRP_MN_QUERY_PNP_DEVICE_STATE
    { IRP_COMPLETE, PciDevice_QueryBusInformation       , FALSE}, // 0x15 - IRP_MN_QUERY_BUS_INFORMATION
    { IRP_COMPLETE, PciDevice_DeviceUsageNotification   , FALSE}, // 0x16 - IRP_MN_DEVICE_USAGE_NOTIFICATION
    { IRP_COMPLETE, PciDevice_SurpriseRemoval           , TRUE }, // 0x17 - IRP_MN_SURPRISE_REMOVAL
    { IRP_COMPLETE, PciIrpNotSupported                  , FALSE}, // 0x18 - IRP_MN_QUERY_LEGACY_BUS_INFORMATION
    { IRP_COMPLETE, PciDevice_DeviceEnumerated          , TRUE }, // 0x19 - IRP_MN_DEVICE_ENUMERATED
    { IRP_COMPLETE, PciIrpNotSupported                  , FALSE}  // unhandled
};

// ------------------------------------------------------------------- Functions

/*++

The majority of functions in this file are called based on their presence
in the Pnp dispatch table.  In the interests of brevity the arguments
to all those functions will be described below:

NTSTATUS
PciDevice_Xxx(
    IN PIRP Irp,
    IN PIO_STACK_LOCATION IrpStack,
    IN PPCI_COMMON_EXTENSION DeviceExtension
    )

Routine Description:

    This function handles the Xxx requests for a given PCI FDO or PDO.

Arguments:

    Irp - Points to the IRP associated with this request.

    IrpStack - Points to the current stack location for this request.

    DeviceExtension - Points to the device's extension.

Return Value:

    Status code that indicates whether or not the function was successful.

    STATUS_NOT_SUPPORTED indicates that the IRP should be completed without
    changing the Irp->IoStatus.Status field otherwise it is updated with this
    status.

--*/

NTSTATUS
PciDevice_Start(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    PCI_DEVICE_RESOURCES deviceResources;
    BOOLEAN ChildNeedsD0Referenced;
    PCI_DEVICE_ID_DATA idData;
    POWER_STATE powerState;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;
    ChildNeedsD0Referenced = FALSE;

    status = PciBeginStateTransition(DeviceExtension, PciStarted);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (device->LegacyDriver) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto exit;
    }

    //
    // Examine the failure code to determine if there is a need to bubble up
    // any previously encountered errors during device enumeration.
    //
    // N. B. No hack flag should be allowed to override these failures.
    //

    if (device->EnumFailureCode != 0) {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    //
    // The PnP manager may send IRP_MN_START_DEVICEs back-to-back when the
    // resource requirements for the child device have changed.  Only bother to
    // confirm the device's ID and set its power state when the device is coming
    // from some other state.
    //

    if (DeviceExtension->DeviceState != PciStarted) {

        status = PciReadDeviceIdData(device, &idData);
        if (!NT_SUCCESS(status) ||
            !PciIsSameDevice(device, &idData)) {

            //
            // Avoid debug spew if this failure is for a
            // PCI host bridge on a known test system.
            //

            if (!((device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
                  (device->SubClass == PCI_SUBCLASS_BR_HOST) &&
                  (PciSystemWideHackFlags &
                   PCI_SYS_HACK_AVOID_ASSERT_ON_BAD_IDDATA_FOR_HOST_BRIDGE))) {

                ERROR("PciDevice_Start: Not the same device!");
            }

            status = STATUS_DEVICE_DOES_NOT_EXIST;
            PciSetEnumFailureCode(device, PCI_INVALID_DEVICE_ID_DATA);
            goto exit;
        }

        //
        // Power up the device. If the device is moved out of D3, take the
        // ChildNeedsD0 reference count of the parent to make sure that
        // *The number of ChildNeedsD0 referencec is equal to the number of
        // child devices with their LogicalPowerState equal to D0* is still
        // preserved.
        //

        if (device->LogicalPowerState == PowerDeviceD3) {
            PciBus_PowerReference(device->Parent, PciBusPowerReasonChildNeedsD0);
            ChildNeedsD0Referenced = TRUE;
        }

        status = PciSetHardwarePowerStatePassive(device, PowerDeviceD0);
        if (!NT_SUCCESS(status)) {
            status = STATUS_DEVICE_POWER_FAILURE;
            PciSetEnumFailureCode(device, PCI_DEVICE_POWER_FAILURE);
            if (ChildNeedsD0Referenced != FALSE) {
                PciBus_PowerDereference(device->Parent, PciBusPowerReasonChildNeedsD0);
            }

            goto exit;
        }

        device->LogicalPowerState = PowerDeviceD0;

        powerState.DeviceState = PowerDeviceD0;
        PoSetPowerState(
            device->DeviceObject,
            DevicePowerState,
            powerState
            );
    }

    if ((device->AlreadyBeenStarted == 0) &&
        (device->ExpressPort != NULL)) {

        //
        // Pick up initial LTR values.
        //

        ExpressInitializeLtrValue(device->ExpressPort);

        //
        // Look to see if the firmware wants to force a particular OBFF mode.
        //

        ExpressInitializeObffValue(device->ExpressPort);
    }

    //
    // Devices that have no resources are finished starting
    //

    if (device->HackFlags & PCI_HACK_ENUM_NO_RESOURCE) {
        status = STATUS_SUCCESS;
        goto exit;
    }


    //
    // Extract the PDO Resources (PCI driver internal style)
    // from the incoming resource list.
    //

    status = PciProcessStartResources(
                 device,
                 IrpSp->Parameters.StartDevice.AllocatedResources,
                 IrpSp->Parameters.StartDevice.AllocatedResourcesTranslated,
                 &deviceResources,
                 &device->InterruptResource
                 );

    if (!NT_SUCCESS(status)) {
        PciSetEnumFailureCode(device, PCI_FAILED_START_RESOURCE_PROCESSING);
        goto exit;
    }

    //
    // The decodes that get enabled depend on the resources assigned
    // to the device.  If no resources of a particular type are assigned,
    // that decode doesn't get enabled.
    //
    status = PciDecodesFromDeviceResources(device, &device->ResourceDecodes);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // If the new settings are different than the old, remember this.
    //
    if (!RtlEqualMemory(&device->Resources, &deviceResources, sizeof(PCI_DEVICE_RESOURCES))) {

        RtlCopyMemory(&device->Resources,
                      &deviceResources,
                      sizeof(PCI_DEVICE_RESOURCES)
                      );

    }

    //
    // Update the hardware.  Raise IRQL and rendevous the processors to do this
    // if the device is critical to system operation and might be in use while
    // this operation is occurring.  Also do this for devices marked as being
    // on the debugger path.  Disabling the debugger around the reprogramming
    // operation is not sufficient to stop the debugger device from being used,
    // since devices that are marked as being on the debug path are sometimes
    // not actually debugger devices - headless console redirection devices are
    // marked in this way as well.
    //

    status = PciDeviceCallCritical(device,
                                   PciUpdateHardware,
                                   NULL,
                                   (device->OnDebugPath != FALSE));

    if (!NT_SUCCESS(status)) {
        PciSetEnumFailureCode(device, PCI_FAILED_HARDWARE_UPDATE);
        goto exit;
    }

    if (device->ExpressPort != NULL) {
        ExpressPortPrepareForLinkPowerManagement(device->ExpressPort);

        //
        // Enable atomics.
        //

        PciDeviceEnableAtomics(device->ExpressPort);
    }

    DebugCheckRegisterSettings(device);

    status = PciProgramInterruptResource(device);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Save the updated config to the registry so it's preserved.
    //

    status = PciUpdateSavedDeviceConfiguration(device);

exit:

    if (NT_SUCCESS(status)) {
        PCI_DEVICE_CLEAR_FLAG(device, Removed);

        if (device->AlreadyBeenStarted == 0) {
            PciBus_PowerDereference(device->Parent, PciBusPowerReasonBusPnpOperationPending);
            PCI_DEVICE_SET_FLAG(device, AlreadyBeenStarted);
        }

        PciCommitStateTransition(DeviceExtension, PciStarted);

    } else {

        PciCancelStateTransition(DeviceExtension, PciStarted);
    }

    return status;
}

NTSTATUS
PciDevice_QueryRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Don't allow the paging device (or a hibernate device) to
    // be removed or stopped
    //
    if (device->DeviceUsage.Hibernate ||
        device->DeviceUsage.Paging    ||
        device->DeviceUsage.CrashDump ||
        device->OnDebugPath ||
        (device->HackFlags & PCI_HACK_FAIL_QUERY_REMOVE)) {

        return STATUS_DEVICE_BUSY;
    }

    //
    // Don't allow devices with legacy drivers to be removed (even thought the
    // driver may be root enumerated)
    //
    if (device->LegacyDriver) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if (DeviceExtension->DeviceState == PciNotStarted) {

        return STATUS_SUCCESS;

    } else {

        return PciBeginStateTransition(DeviceExtension, PciNotStarted);
    }
}

NTSTATUS
PciDevice_Remove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PCI_DEVICE_ID_DATA idData;
    BOOLEAN illegal;
    PCI_DISCONNECTION_POLICY policy;
    POWER_STATE powerState;
    PPCI_DEVICE device;
    NTSTATUS status;
    ULONG tableSize;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // If this device is a bridge that places its secondary bus into B2, disable
    // D3 since as a result of the remove the contents of the secondary bus are
    // no longer known.  The comment in PciProcessPowerCapability describes this
    // further.
    //

    if ((device->HeaderType == PCI_BRIDGE_TYPE) &&
        (device->BridgeFlags.D3Causes66MHzB2 == 1)) {

        illegal = (device->BridgeFlags.D3Illegal != 0);
        device->BridgeFlags.D3Illegal = 1;
        if ((illegal == FALSE) &&
            (device->Parent != NULL) &&
            (device->Parent->PciBridge != NULL)) {

            PciBridgeUpdateD3Availability(device->Parent->PciBridge);
        }
    }

    //
    // If the device isn't there anymore, don't try to disable it.
    //

    status = PciReadDeviceIdData(device, &idData);
    if (!NT_SUCCESS(status) ||
        !PciIsSameDevice(device, &idData)) {
        PCI_DEVICE_CLEAR_FLAG(device, Present);
    }

    if (device->Present == FALSE) {

        //
        // The device is really gone...
        //

        PciDeleteVolatileInterruptRoutingData(device);

    } else {

        //
        // If the device was already started, move it to D3 and remove it.
        // If the device was never started, leave it in its current state.
        // This is necessary as PnP will sometimes send a remove IRP while
        // prior to starting the deivce while waiting to satisfy device
        // dependencies, and the FDO may then get an AddDevice and attempt
        // to query config space, which may fail if the device were in D3.
        //

        //
        // Update our concept of the save/restore registers in case
        // they've changed.
        //

        if (DeviceExtension->DeviceState == PciStarted) {
            PciUpdateSaveRestoreData(device, &device->SavedRegisters);
        }

        //
        // Turn the device off and power it down if we're allowed to.
        // VGA cards, among others, can't be disabled.
        //
        policy = PciCanDisableDevice(device);
        if (policy != KeepDeviceActive) {
            PciDisableInterrupts(device);
            PciDisconnectDevice(device);

            if (device->ExpressPort != NULL) {
                ExpressWaitForPendingTransactions(device->ExpressPort);
            }

            if ((device->LogicalPowerState != PowerDeviceD3) &&
                (policy == PowerDownDevice)) {

                //
                // Don't move the device to D3 if it supports D3Cold.
                // Otherwise, if the device has been disabled and is later
                // reenabled, the driver may attempt to read config space
                // during AddDevice, which would fail if the device were
                // moved to D3Cold. This is to workaround HCK test failures
                // on the Baytrail XHCI controller.
                //
                // The entire code block below should be removed post-Blue
                // to keep all devices in D0 after a remove.
                //

                if ((device->D3ColdSupport != D3ColdSupportEnabled) ||
                    (PCI_ROOT_BUS(device->Parent) == FALSE) ||
                    (device->FirmwareSaysDeviceDoesD3cold == 0)) {

                    powerState.DeviceState = PowerDeviceD3;
                    PoSetPowerState(
                        device->DeviceObject,
                        DevicePowerState,
                        powerState
                        );

                    PciSetHardwarePowerStatePassive(device, PowerDeviceD3);

                    device->LogicalPowerState = PowerDeviceD3;

                    //
                    // Here, the device is transitioned into D3. Release the
                    // ChildNeedsD0 reference on the parent to preserve the
                    // invariant that *The number of ChildNeedsD0 reference
                    // is equal to the number of child devices with their
                    // LogicalPowerState equal to D0*.
                    //

                    PciBus_PowerDereference(device->Parent,
                                            PciBusPowerReasonChildNeedsD0);
                }
            }
        }

        //
        // Mark the device as removed.
        //

        PCI_DEVICE_SET_FLAG(device, Removed);
    }

    //
    // Free a mapping to the hardware MSI-X table.
    //

    if ((device->InterruptResource.Type == PciMsiX) &&
        (device->InterruptResource.MsiX.Table != NULL)) {

        tableSize = device->InterruptRequirement.Message.MessagesRequested *
                    sizeof(PCI_MSIX_TABLE_ENTRY);

        MmUnmapIoSpace(device->InterruptResource.MsiX.Table, tableSize);
        device->InterruptResource.MsiX.Table = NULL;
    }

    if ((device->Present == FALSE) ||
        (idData.VendorID == 0xFFFF)) {

        if ((device->HotPlugSlot != NULL) &&
            !PCI_DEVICE_EJECTABLE(device)) {

            PciHandleHotPlugDriverRemoval(device->HotPlugSlot);
        }
    }

    //
    // If the deviced was removed with SR-IOV enabled, clean up the
    // ACS state on the path to the root port.
    //
    if (device->ExpressPort != NULL &&
        device->ExpressPort->SriovEnabled != FALSE) {
        PCI_EXPRESS_ACS_CONTROL bitState = {0};
        PciVirtualizationConfigureAcs(device, PciAcsBitsForIov, bitState);
        device->ExpressPort->SriovEnabled = FALSE;
    }

    //
    // We can get a remove in one of three states:
    // 1) We have received a QueryRemove/SurpriseRemove in which case we are
    //    transitioning to PciNotStarted.
    // 2) We were never started, so we are already in PciNotStarted.
    // 3) We started the PDO, but the FDO failed start. We are in PciStarted in
    //    this case.
    //
    if (!PciIsInTransitionToState(DeviceExtension, PciNotStarted) &&
        (DeviceExtension->DeviceState == PciStarted)) {

        PciBeginStateTransition(DeviceExtension, PciNotStarted);
    }

    if (PciIsInTransitionToState(DeviceExtension, PciNotStarted)) {

        PciCommitStateTransition(DeviceExtension, PciNotStarted);
    }

    if (device->ReportedMissing) {
        if (device->SurpriseRemoveAndReenumerateSelf != 0) {
            IoInvalidateDeviceRelations(device->Parent->PhysicalDeviceObject, BusRelations);
        }

        status = IoAcquireRemoveLock(&device->RemoveLock, NULL);
        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        status = PciBeginStateTransition(DeviceExtension, PciDeleted);
        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        PciCommitStateTransition(DeviceExtension, PciDeleted);

        PCI_ASSERT(PCI_BUS_EXTENSION(device->Parent));

        IoReleaseRemoveLockAndWait(&device->RemoveLock, NULL);
        PciAcquirePassiveLock(&device->Parent->ChildDeviceLock);
        PciDestroyDevice(device);
        PciReleasePassiveLock(&device->Parent->ChildDeviceLock);

        //
        // If the device was in D0, then release the ChildNeedsD0 reference
        // count on the parent.
        //

        if (device->LogicalPowerState == PowerDeviceD0) {
            PciBus_PowerDereference(device->Parent, PciBusPowerReasonChildNeedsD0);
            device->LogicalPowerState = PowerDeviceD3;
        }

        //
        // There will still be another ref count if the device was never started.
        // remove that now.
        //

        if (device->AlreadyBeenStarted == 0) {
            PciBus_PowerDereference(device->Parent, PciBusPowerReasonBusPnpOperationPending);
            if (device->D3ColdSupport != D3ColdSupportEnabled) {
                PciBus_PowerDereference(device->Parent, PciBusPowerAllowF1DisableBridgeD3);
            }
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_CancelRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PAGED_CODE();

    PciCancelStateTransition(DeviceExtension, PciNotStarted);
    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_Stop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    USHORT command;
    ULONG tableSize;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Update our concept of the save/restore registers in case
    // they've changed.
    //
    PciUpdateSaveRestoreData(device, &device->SavedRegisters);
    if (device->ExpressPort != NULL) {
        ExpressSavePortRegisters(device->ExpressPort);

    } else if (device->ExpressCompatibilityPort != NULL) {
        ExpressCompatibilitySavePortRegisters(device->ExpressCompatibilityPort);
    }

    if (PCI_VGA_CONTROLLER(device)) {

        //
        // It is possible that we left a video controller disabled
        // because the BIOS left it that way.  If this device is
        // actually used, the driver for the device will turn on the
        // decodes.  We need to make sure that the internal field indicating
        // which decodes to enable is updated to match this, so that
        // we do the correct thing on the subsequent re-start.
        //
        PciReadConfigRegister(device, Command, &command);
        device->PossibleDecodes = command & PCI_DECODE_ENABLES;
    }

    //
    // Free a mapping to the hardware MSI-X table.
    //

    if ((device->InterruptResource.Type == PciMsiX) &&
        (device->InterruptResource.MsiX.Table != NULL)) {

        tableSize = device->InterruptRequirement.Message.MessagesRequested *
                    sizeof(PCI_MSIX_TABLE_ENTRY);

        MmUnmapIoSpace(device->InterruptResource.MsiX.Table, tableSize);
        device->InterruptResource.MsiX.Table = NULL;
    }

    //
    // We receive Stop Irp for only those devices that succeed Query Stop
    //

    PciDisableInterrupts(device);
    PciDisconnectDevice(device);
    if (device->ExpressPort != NULL) {
        ExpressWaitForPendingTransactions(device->ExpressPort);

        //
        // Disable atomics.
        //

        PciDeviceDisableAtomics(device->ExpressPort);
    }

    PciCommitStateTransition(DeviceExtension, PciStopped);

    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_QueryStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Don't allow the paging device (or a hibernate device) to
    // be removed or stopped
    //
    if (device->DeviceUsage.Hibernate ||
        device->DeviceUsage.Paging    ||
        device->DeviceUsage.CrashDump ||
        device->OnDebugPath) {

        return STATUS_DEVICE_BUSY;
    }

    //
    // Don't allow devices with legacy drivers to be removed (even though the
    // driver may be root enumerated)
    //
    if (device->LegacyDriver) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // If a device doesn't play nicely with rebalance, reject the request.
    //
    if (device->HackFlags & PCI_HACK_FAIL_QUERY_STOP) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // This system wide hack flag essentially disables multi-level rebalance
    // by failing query stop on PCI-PCI bridges.
    //
    if ((PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_REBALANCE) &&
        ((device->HeaderType == PCI_BRIDGE_TYPE) ||
         (device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE))) {

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // If we cannot free the resources, don't allow reprogramming.
    //
    if (PciCanDisableDevice(device) == KeepDeviceActive) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    status = PciBeginStateTransition(DeviceExtension, PciStopped);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Return a sentinel status if the resource requirements have changed.
    // Indicate changed resource requirements if interrupt requirements were
    // generated with processor affinity awareness, and the number of active
    // processors in the system has changed.
    //

    if ((device->InterruptResource.PolicySpecified != FALSE) &&
        (device->CapturedProcessorCount <
         KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS))) {

        return STATUS_RESOURCE_REQUIREMENTS_CHANGED;
    }

    return status;
}

NTSTATUS
PciDevice_CancelStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PAGED_CODE();

    PciCancelStateTransition(DeviceExtension, PciStopped);
    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_Eject(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )

/*++

Routine Description:

    This routine handles IRP_MN_EJECT for a PCI PDO. Note that IRP status
    is only updated if PCI handles eject natively.

Arguments:

    Irp - Supplies a pointer to IRP_MN_EJECT.

    IrpSp - Supplies a pointer to the current IRP stack location.

    DeviceExtension - Supplies a pointer to the PDO device extension.


Return Value:

    NTSTATUS.

--*/

{
    PPCI_DEVICE device;
    PCI_DEVICE_ID_DATA idData;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;
    status = Irp->IoStatus.Status;

    if ((device->HotPlugSlot != NULL) && PCI_DEVICE_EJECTABLE(device)) {
        status = PciReadDeviceIdData(device, &idData);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        if (PciIsSameDevice(device, &idData) || (idData.VendorID == 0xFFFF)) {
            status = PciHandleHotPlugDriverRemoval(device->HotPlugSlot);
        }
    }

    PciDeleteVolatileInterruptRoutingData(device);

    return status;
}

NTSTATUS
PciDevice_QueryDeviceRelations(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PDEVICE_RELATIONS relations;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    relations = NULL;
    switch(IrpSp->Parameters.QueryDeviceRelations.Type) {

        case EjectionRelations:
            status = PciQueryEjectionRelations(device,
                                               &relations
                                               );
            break;

        case TargetDeviceRelation:
            status = PciQueryTargetDeviceRelations(device,
                                                   &relations
                                                   );
            break;

        default:
            status = STATUS_NOT_SUPPORTED;
            break;
    }

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = (ULONG_PTR)relations;
    }

    return status;
}

NTSTATUS
PciDevice_QueryInterface(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    status = PciQueryInterface(
                device,
                IrpSp->Parameters.QueryInterface.InterfaceType,
                IrpSp->Parameters.QueryInterface.Size,
                IrpSp->Parameters.QueryInterface.Version,
                IrpSp->Parameters.QueryInterface.InterfaceSpecificData,
                IrpSp->Parameters.QueryInterface.Interface,
                FALSE,
                Irp->IoStatus.Status
                );

    return status;
}

NTSTATUS
PciDevice_QueryCapabilities(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PDEVICE_CAPABILITIES capabilities;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;
    capabilities = IrpSp->Parameters.DeviceCapabilities.Capabilities;

#ifndef HANDLE_BOGUS_CAPS
    if (capabilities->Version < 1) {

        //
        // do not touch irp!
        //
        return STATUS_NOT_SUPPORTED;
    }
#endif

    //
    // For PCI devices, the Capabilities Address field contains
    // the Device Number in the upper 16 bits and the function
    // number in the lower.
    //
    capabilities->Address =
        (device->Slot.u.bits.DeviceNumber << 16) |
        device->Slot.u.bits.FunctionNumber;

    //
    // The PCI bus driver only generates Unique IDs for its children
    // if they are PCI Express devices with serial numbers.
    //

    if ((device->ExpressPort != NULL) &&
	    (device->InstanceIdSerialNumber != 0)) {

        capabilities->UniqueID = TRUE;

    } else {
        capabilities->UniqueID = FALSE;
    }

    PciDetermineSlotNumber(device, &capabilities->UINumber);

    if (PCI_DEVICE_EJECTABLE(device)) {
        capabilities->EjectSupported = TRUE;
    }

    if (PCI_DEVICE_REMOVABLE(device)) {
        capabilities->Removable = TRUE;
    }

    if (PCI_DEVICE_SURPRISE_REMOVABLE(device)) {
        capabilities->SurpriseRemovalOK = TRUE;
    }

    if (PCI_HOST_BRIDGE(device) != FALSE) {

        //
        // Host bridges are raw capable.  This allows the devnode for them
        // to start without any service present.  On the vast majority of
        // machines this is not necessary, but on some machines the ACPI
        // BIOS enumerates devices behind a host bridge.  On these machines
        // if the devnode does not start, PnP doesn't send the request to
        // enumerate the devices behind it and the ACPI enumerated devices
        // don't appear.
        //
        // Because this driver is the service for host bridges, and because host
        // bridges are not operationally usable devices, allow them to be
        // surprise removed without issue.
        //

        capabilities->RawDeviceOK = TRUE;
        capabilities->SurpriseRemovalOK = TRUE;

    } else if ((device->ExpressPort != NULL) &&
               (device->ExpressPort->DeviceType ==
                PciExpressRootComplexEventCollector)) {

        //
        // Root Complex Event Collectors are raw capable.
        //

        capabilities->RawDeviceOK = TRUE;

    } else {
        capabilities->RawDeviceOK = FALSE;
    }

    //
    // Finally, compute the power capabilities.
    //
    status = PciQueryPowerCapabilities(device, capabilities);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    DebugDumpDeviceCapabilities(PNP_TRACE_LEVEL, capabilities);

    return status;
}

NTSTATUS
PciDevice_QueryId(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    WCHAR tempBuffer[PCI_MAX_ID_LENGTH];
    PWSTR finalBuffer;
    ULONG bufferLength;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;
    bufferLength = sizeof(tempBuffer);

    switch (IrpSp->Parameters.QueryId.IdType) {
        case BusQueryInstanceID:

            status = PciQueryInstanceID(device, tempBuffer, &bufferLength);
            break;

        case BusQueryDeviceID:

            status = PciQueryDeviceID(device, tempBuffer, &bufferLength);
            break;


        case BusQueryHardwareIDs:

            status = PciQueryHardwareIDs(device, tempBuffer, &bufferLength);
            break;

        case BusQueryCompatibleIDs:

            status = PciQueryCompatibleIDs(device, tempBuffer, &bufferLength);
            break;

        default:

            WARN("PciQueryId unexpected ID type = %d\n",
                 IrpSp->Parameters.QueryId.IdType
                 );

            status = STATUS_NOT_SUPPORTED;
    }

    if (!NT_SUCCESS(status)) {

        Irp->IoStatus.Information = 0;
        return status;
    }

    //
    // The temporary buffer was filled in successfully.  Now allocate
    // one of the right size and copy the information over.
    //
    finalBuffer = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, bufferLength);
    if (finalBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(finalBuffer, tempBuffer, bufferLength);

    Irp->IoStatus.Information = (ULONG_PTR)finalBuffer;

    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_QueryResources(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PCM_RESOURCE_LIST resourceList;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    status = PciBuildCurrentResourceList(device, &resourceList);

    if (NT_SUCCESS(status)) {

        if (resourceList != NULL) {
            Irp->IoStatus.Information = (ULONG_PTR)resourceList;
        } else {
            status = STATUS_NOT_SUPPORTED;
        }
    }

    return status;
}

NTSTATUS
PciDevice_QueryResourceRequirements(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PIO_RESOURCE_REQUIREMENTS_LIST requirementsList;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Capture the processor count.  Interrupt requirement generation
    // consumes this count, and keeping this value allows the requirements
    // to be re-computed if the count changes later.
    //
    device->CapturedProcessorCount =
        KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

    //
    // Build the requirements list.
    //
    status = PciBuildRequirementsList(device, &requirementsList);
    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = (ULONG_PTR)requirementsList;
    }

    return status;
}

NTSTATUS
PciDevice_QueryDeviceText(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    PWSTR deviceText;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    status = PciQueryDeviceText(device,
                                IrpSp->Parameters.QueryDeviceText.DeviceTextType,
                                IrpSp->Parameters.QueryDeviceText.LocaleId,
                                &deviceText
                                );

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = (ULONG_PTR)deviceText;
    }

    return status;
}

NTSTATUS
PciDevice_DeviceEnumerated(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    BOOLEAN canGoCold;
    D3COLD_SUPPORT_INTERFACE d3cold;
    PPCI_DEVICE device;
    HANDLE handle;
    IO_STACK_LOCATION irpStack;
    POWER_STATE powerState;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Evaluate _DSM to determine if the platform provides optimized delay values 
    // for this device.
    //

    if (device->DsmForDelayOptimizationEvaluated == 0) {
        PciEvaluateDsmMethodForDelayOptimization(device->DeviceObject, device->DelayInfo);
        PCI_DEVICE_SET_FLAG(device, DsmForDelayOptimizationEvaluated);
    }

    //
    // Some devices require extra settle time after the parent bridge 
    // transitions from D3 to D0.  This settle time is on by default on
    // X86/64 and off by default on ARM/ARM64.  
    //

    if (PciIsDeviceFeatureEnabled(device, PCI_FEATURE_SETTLE_TIME_REQUIRED) 
            == PciFeatureStateEnabled) {

        PCI_BUS_SET_FLAG(device->Parent, SettleTimeRequired);
    }

    //
    // Some devices generate errors on the bus while performing a PLDR.
    // While the errors are categorized as uncorrectable, they are
    // cleaned up during _RST evaluation and can safely be ignored.
    //

    if (PciIsDeviceFeatureEnabled(device, PCI_FEATURE_IGNORE_ERRORS_PLDR)
            == PciFeatureStateEnabled) {

        PCI_DEVICE_SET_FLAG(device, DisableWheaDuringPLDR);
    }

    //
    // Now that the entire stack has been built, find out if the firmware
    // thinks that this device can do D3cold.
    //

    RtlZeroMemory(&d3cold, sizeof(d3cold));
    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack.Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_D3COLD_SUPPORT_INTERFACE;
    irpStack.Parameters.QueryInterface.Version = D3COLD_SUPPORT_INTERFACE_VERSION;
    irpStack.Parameters.QueryInterface.Size = sizeof(d3cold);
    irpStack.Parameters.QueryInterface.Interface = (PINTERFACE)&d3cold;
    irpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;

    status = PciSendPnpIrp(device->DeviceObject, &irpStack, NULL);
    if (NT_SUCCESS(status)) {

        status = d3cold.GetD3ColdCapability(d3cold.Context, &canGoCold);
        if (NT_SUCCESS(status) && (canGoCold != FALSE)) {
            PCI_DEVICE_SET_FLAG(device, FirmwareSaysDeviceDoesD3cold);
        }

        if (d3cold.InterfaceDereference != NULL) {
            d3cold.InterfaceDereference(d3cold.Context);
        }
    }

    //
    // Go read the registry and see if the INF says that the device can do
    // D3cold.
    //

    if (PciSystemWideHackFlags & PCI_SYS_HACK_NOBODY_SUPPORTS_D3COLD) {
        device->D3ColdSupport = D3ColdSupportDisabled;
    } else if (PciSystemWideHackFlags & PCI_SYS_HACK_EVERYBODY_SUPPORTS_D3COLD) {
        device->D3ColdSupport = D3ColdSupportEnabled;
    }

    if (device->D3ColdSupport == D3ColdSupportUninitialized) {

        switch (PciIsDeviceFeatureEnabled(device, PCI_FEATURE_D3_COLD_SUPPORTED)) {
        case PciFeatureStateDisabled:

            device->D3ColdSupport = D3ColdSupportDisabledInRegistry;
            break;

        case PciFeatureStateEnabled:

            device->D3ColdSupport = D3ColdSupportEnabled;
            break;
        }

        if (device->D3ColdSupport == D3ColdSupportEnabled) {

            //
            // As this is the first transition from Uninitialized
            // to Enabled, power up the device if it's not already
            // in D0.  This is necessary for config space access
            // in FDO AddDevice routines.
            //

            if (device->LogicalPowerState != PowerDeviceD0) {
                powerState.DeviceState = PowerDeviceD0;
                PoSetPowerState(
                    device->DeviceObject,
                    DevicePowerState,
                    powerState
                    );

                PciSetHardwarePowerStatePassive(device, PowerDeviceD0);
                device->LogicalPowerState = PowerDeviceD0;

                //
                // Transitioning into D0, take PciBusPowerReasonChildNeedsD0.
                //

                PciBus_PowerReference(device->Parent, PciBusPowerReasonChildNeedsD0);
            }

            //
            // Release the power reference so future D3 transitions
            // will be D3Cold capable.
            //

            PciBus_PowerDereference(device->Parent,
                                    PciBusPowerAllowF1DisableBridgeD3);
        }
    }

    //
    // This next bit is here to allow people to permanently disable bridges
    // through group policy.  Specifically, the concern is that Thunderbolt
    // bridges will be enabled for DMA by default, and thus you can walk up and
    // plug in a cable and suck out the contents of memory through that cable.
    // When policy is applied, the driver stack is torn down and everything is
    // fine.  But when the machine is rebooted, then the device just never
    // starts.  We detect that condition here by looking to see if there is a
    // "software key" associated with the device.  If there is, then the stack
    // is not disabled and a driver stack has been built on top of it.  (This
    // is a hack.  We shouldn't be depending on side effects of the way PnP
    // works.  But we're really close to shiping Win8 at the moment and this
    // will have to do.  Future maintainers should take another look at this.)
    //
    // To further complicate the situation, it's possible to start a bridge
    // using the critical device database, and it won't have a software key
    // then.  To detect that condition, we look to see whether it's parent got
    // a full-install, on the assumption that a parent bus is processed through
    // the CDDB, too.
    //

    if ((device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
        (device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI)) {

        status = IoOpenDeviceRegistryKey(device->DeviceObject,
                                         PLUGPLAY_REGKEY_DRIVER,
                                         STANDARD_RIGHTS_ALL,
                                         &handle);

        if (NT_SUCCESS(status)) {
            ZwClose(handle);
        } else {
            if (device->Parent->StartedWithoutInf == 0) {

                //
                // Shut this device off.
                //

                TraceEvents(device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_PNP,
                            "Disabling device (%x:%x) to comply with Group Policy",
                            device->Slot.u.bits.DeviceNumber,
                            device->Slot.u.bits.FunctionNumber);

                PciDisconnectDevice(device);
                PciSetHardwarePowerStatePassive(device, PowerDeviceD3);
            }
        }
    }

    //
    // Present the PCI device properties to other components thru custom
    // property APIs.  Host bridge devices should not contain these properties.
    //

    if (PCI_HOST_BRIDGE(device) == FALSE) {
        status = PciDeviceSetCustomProperties(device);
    }

    return status;
}

NTSTATUS
PciDevice_ReadConfig(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;
    NTSTATUS status;
    ULONG lengthRead;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    lengthRead = 0;

    status = PciReadDeviceSpace(device,
                                0,
                                IrpSp->Parameters.ReadWriteConfig.WhichSpace,
                                IrpSp->Parameters.ReadWriteConfig.Buffer,
                                IrpSp->Parameters.ReadWriteConfig.Offset,
                                IrpSp->Parameters.ReadWriteConfig.Length,
                                &lengthRead
                                );

    Irp->IoStatus.Information = lengthRead;

    return status;
}

NTSTATUS
PciDevice_WriteConfig(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    ULONG lengthWritten;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    lengthWritten = 0;

    status = PciWriteDeviceSpace(device,
                                 0,
                                 IrpSp->Parameters.ReadWriteConfig.WhichSpace,
                                 IrpSp->Parameters.ReadWriteConfig.Buffer,
                                 IrpSp->Parameters.ReadWriteConfig.Offset,
                                 IrpSp->Parameters.ReadWriteConfig.Length,
                                 &lengthWritten
                                 );

    Irp->IoStatus.Information = lengthWritten;

    return status;
}

NTSTATUS
PciDevice_QueryBusInformation(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPNP_BUS_INFORMATION information;
    PPCI_DEVICE device;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    information = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION,
                                  sizeof(PNP_BUS_INFORMATION)
                                  );
    if (information == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(&information->BusTypeGuid, &GUID_BUS_TYPE_PCI, sizeof(GUID));
    information->LegacyBusType = PCIBus;
    information->BusNumber = device->BusNumber;

    Irp->IoStatus.Information = (ULONG_PTR)information;

    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_DeviceUsageNotification(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    IO_STACK_LOCATION newStack;
    PPCI_DEVICE device;
    NTSTATUS EarlyRestoreStatus;
    NTSTATUS status;
    ULONG UsageType;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;
    status = STATUS_SUCCESS;

    //
    // Do we have a parent that we must notify?
    //
    if ((device->Parent != NULL) &&
        (device->Parent->PhysicalDeviceObject != NULL)) {

        newStack.MajorFunction = IrpSp->MajorFunction;
        newStack.MinorFunction = IrpSp->MinorFunction;
        newStack.Parameters = IrpSp->Parameters;

        status = PciSendPnpIrp(device->Parent->DeviceObject,
                               &newStack,
                               NULL
                               );
    }

    //
    // If we succeeded, then apply the usages locally
    //
    if (NT_SUCCESS(status)) {

        status = PciLocalDeviceUsage(&DeviceExtension->DeviceUsage,
                                     IrpSp->Parameters.UsageNotification.Type,
                                     IrpSp->Parameters.UsageNotification.InPath
                                     );
    }

    //
    // If this device is relevant to early restore and is an endpoint,
    // then add it to the PCI early restore list.
    //

    if ((IrpSp->Parameters.UsageNotification.InPath != FALSE) &&
        (device->HeaderType == PCI_DEVICE_TYPE)) {

        switch(IrpSp->Parameters.UsageNotification.Type) {
        case DeviceUsageTypeHibernation:
            UsageType = PCI_EARLY_RESTORE_HIBER;
            break;

        case DeviceUsageTypeDumpFile:
            UsageType = PCI_EARLY_RESTORE_CRASHDUMP;
            break;

        default:
            UsageType = PCI_EARLY_RESTORE_INVALID;
            break;
        }

        if (UsageType != PCI_EARLY_RESTORE_INVALID) {
            EarlyRestoreStatus =
                PciEarlyRestoreAddListItem(device->BusNumber,
                                           &(device->Slot),
                                           UsageType);

            PCI_ASSERT(NT_SUCCESS(EarlyRestoreStatus));

        }
    }

    return status;
}

NTSTATUS
PciDevice_SurpriseRemoval(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PCI_DEVICE_ID_DATA idData;
    PCI_DISCONNECTION_POLICY policy;
    POWER_STATE powerState;
    PPCI_DEVICE device;
    NTSTATUS status;
    BOOLEAN DevicePresent;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // There are three kinds of surprise removals
    // - Surprise removals due to removal of our device
    // - Surprise removals due to failure of our device to start
    // - Surprise removals due to the SurpriseRemoveAndReenumerateSelf API.
    //

    status = PciReadDeviceIdData(device, &idData);
    if (!NT_SUCCESS(status) ||
        !PciIsSameDevice(device, &idData)) {
        PCI_DEVICE_CLEAR_FLAG(device, Present);
        DevicePresent = FALSE;
    } else {
        DevicePresent = TRUE;
    }

    if (device->SurpriseRemoveAndReenumerateSelf != 0) {

        //
        // If WHEA was disabled while SRRS running, turn it back on.
        //

        if (device->WheaReportingDisabled != 0) {
            device->Parent->PciBridge->ExpressBridge->WheaInitialized = 1;
            PCI_DEVICE_CLEAR_FLAG(device, WheaReportingDisabled);
        }

        //
        // Clearing SurpriseRemoveAndReenumerateSelf allows future
        // IRP_MN_QUERY_DEVICE_RELATIONS calls to reenumerate
        // the device which was just removed. Prior to that point any
        // QDR calls will pretend the device doesn't exist.
        //

        PCI_DEVICE_CLEAR_FLAG(device, SurpriseRemoveAndReenumerateSelf);

        //
        // Attempt to reset the device before it is reenumerated.
        //

        if (DevicePresent != FALSE) {
            PciAttemptDeviceReset(device);
        }

        //
        // Request a call to IRP_MN_QUERY_DEVICE_RELATIONS to allow the
        // device to be reenumerated.
        //

        IoInvalidateDeviceRelations(device->Parent->PhysicalDeviceObject, BusRelations);
    }

    if (device->Present) {

        //
        // Turn the device off and power it down if we're allowed to.
        // VGA cards, among others, can't be disabled.
        // While you might think this should be done only if we were already
        // headed to PciNotStarted, we may in fact have a boot config that
        // needs to be disabled. Note that we may turn it off again in remove
        // device. No big deal.
        //
        policy = PciCanDisableDevice(device);
        if (policy != KeepDeviceActive) {
            PciDisableInterrupts(device);
            PciDisconnectDevice(device);
            if (device->ExpressPort != NULL) {
                ExpressWaitForPendingTransactions(device->ExpressPort);
            }

            if ((device->LogicalPowerState != PowerDeviceD3) &&
                (policy == PowerDownDevice)) {

                powerState.DeviceState = PowerDeviceD3;
                PoSetPowerState(
                    device->DeviceObject,
                    DevicePowerState,
                    powerState
                    );

                PciSetHardwarePowerStatePassive(device, PowerDeviceD3);

                device->LogicalPowerState = PowerDeviceD3;

                //
                // Here, the device is transitioned into D3. Release the
                // ChildNeedsD0 reference on the parent to preserve the
                // invariant that *The number of ChildNeedsD0 reference
                // is equal to the number of child devices with their
                // LogicalPowerState equal to D0*.
                //

                PciBus_PowerDereference(device->Parent,
                                        PciBusPowerReasonChildNeedsD0);
            }
        }
    } else {
        PciDeleteVolatileInterruptRoutingData(device);
    }

    if (!device->ReportedMissing) {

        if (device->DeviceState != PciNotStarted) {
            PciBeginStateTransition(DeviceExtension, PciNotStarted);
        }

    } else {

        //
        // The device is physically gone, don't dare touch it!
        //
        PciBeginStateTransition(DeviceExtension, PciSurpriseRemoved);
        PciCommitStateTransition(DeviceExtension, PciSurpriseRemoved);
    }

    return STATUS_SUCCESS;
}


NTSTATUS
PciDevice_QueryDeviceState(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_DEVICE device;

    PAGED_CODE();

    device = (PPCI_DEVICE)DeviceExtension;

    //
    // Host brides cannot be disabled and the user should not be given a
    // opportunity to do so.
    //

    if (PCI_HOST_BRIDGE(device) != FALSE) {

        //
        // PNP_DEVICE_STATE us a ULONG value just passed in the information field
        // of the IRP.
        //
        Irp->IoStatus.Information |= PNP_DEVICE_NOT_DISABLEABLE;
    }

    if ((device->DeviceObject->Flags & DO_DEVICE_TO_BE_RESET) &&
        (device->DisableWheaDuringPLDR != FALSE) &&
        (PCI_ROOT_BUS(device->Parent) == FALSE) &&
        (device->Parent->PciBridge->ExpressBridge != NULL) &&
        (device->Parent->PciBridge->ExpressBridge->WheaInitialized != 0)) {

        //
        // Turn off WHEA reporting since the device
        // may generate errors during the PLDR.  Interrupts
        // will still be generated and the errors will be cleared,
        // but PCI will not send them to WHEA for further processing
        //

        device->Parent->PciBridge->ExpressBridge->WheaInitialized = 0;
        PCI_DEVICE_SET_FLAG(device, WheaReportingDisabled);
    }

    return STATUS_SUCCESS;
}

//
// ---------------------------------------------------------------- Utilities


NTSTATUS
PciQueryPowerCapabilities(
    __in PPCI_DEVICE Device,
    __inout PDEVICE_CAPABILITIES Capabilities
    )
/*++

Routine Description:

    Determine a device's power capabilites by using its parent capabilities

    It should be noted that there two ways that the code calculates the system
    and device wake levels. The first method, which is preferred, biases toward
    the deepest possible system state, and the second, which gets used if the
    first fails to find something legal, is biased towards finding the deepest
    possible device wake state

Arguments:

    Device - The device whose capabilities we will provide

    Capablities - Where we will store the device capabilities

Return Value:

    NT Status code.

--*/
{
    SYSTEM_POWER_STATE highestSupportedSleepState, validSystemWakeState;
    DEVICE_POWER_STATE validDeviceWakeState;
    DEVICE_POWER_STATE deviceState;
    SYSTEM_POWER_STATE systemState;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = Device->Parent;

    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {

        //
        // If the device doesn't have any PM caps,
        // use the parent's mapping as our own
        //
        RtlCopyMemory(Capabilities->DeviceState,
                      Device->Parent->SystemStateMapping,
                      (sizeof(DEVICE_POWER_STATE) * PowerSystemMaximum));

        //
        // As D1 and D2 are not supported here, round down to D3.
        //
        //     This code is not enabled so that a hack becomes available for
        // older PCI video cards. Basically, older video cards can do D3 hot
        // but not D3 cold (in which case they need reposting). ACPI supplies
        // a hack by which all PCI-to-PCI bridges are said to map S1->D1. The
        // code below lets the parent's D1 "appear" as a state the child
        // supports, regardless of it's real capabilities. Video drivers for
        // such cards fail D3 (which may be D3-cold), but succeed D1 (which is
        // really D3-hot).
        //
        // Also note that this is not targetted at video cards but rather is
        // targetted at any non-PCI power managed device. That means drivers
        // for older devices need to either map D1&D2 to D3 themselves, or
        // treat unexpected D1&D2 IRPs as if D3. Folklore says that there is
        // also a net card or two that also takes advantage of this hack.
        //
#if 0
        for (systemState = PowerSystemWorking; systemState < PowerSystemMaximum; index++) {

            //
            // This is the device state that the parent supports
            //
            deviceState = Device->Parent->SystemStateMapping.DeviceState[systemState];

            //
            // Round down if D1 or D2
            //
            if ((deviceState == PowerDeviceD1) || (deviceState == PowerDeviceD2)) {

                Capabilities->DeviceState[systemState] = PowerDeviceD3;
            }
        }
#endif

        //
        // The device has no wake capabilities
        //
        Capabilities->DeviceWake = PowerDeviceUnspecified;
        Capabilities->SystemWake = PowerSystemUnspecified;

        //
        // Set these bits explicitly
        //
        Capabilities->DeviceD1 = FALSE;
        Capabilities->DeviceD2 = FALSE;
        Capabilities->WakeFromD0 = FALSE;
        Capabilities->WakeFromD1 = FALSE;
        Capabilities->WakeFromD2 = FALSE;
        Capabilities->WakeFromD3 = FALSE;

        return STATUS_SUCCESS;
    }

    //
    // Now deal with devices that actually do power management.
    //

    //
    // Set the capabilities bits, explicitly based on the
    // device's PM capability.
    //
    Capabilities->DeviceD1 = Device->PowerSupport.D1;
    Capabilities->DeviceD2 = Device->PowerSupport.D2;
    Capabilities->WakeFromD0 = Device->PowerSupport.PMED0;
    Capabilities->WakeFromD1 = Device->PowerSupport.PMED1;
    Capabilities->WakeFromD2 = Device->PowerSupport.PMED2;

    //
    // If D1 and D2 will cause a surprise link down, make these states
    // unavailable.
    //

    if (((bus->PciBridge != NULL) &&
         (bus->PciBridge->AvoidChildD1D2ForSld != 0)) ||
        ((Device->HackFlags & PCI_HACK_AVOID_D1D2_FOR_SLD) != 0)) {

        Capabilities->DeviceD1 = 0;
        Capabilities->DeviceD2 = 0;
        Capabilities->WakeFromD1 = 0;
        Capabilities->WakeFromD2 = 0;
    }

    if (Device->Parent->DeviceWake == PowerDeviceD3) {

        //
        // If our parent can wake from the D3 state, than we only
        // support WakeFromD3 if the device can actually wake from
        // D3 cold. The (obvious) exception to this is if the
        // parent is a root bus...
        //
        if (Device->Parent->PciBridge) {

            Capabilities->WakeFromD3 = Device->PowerSupport.PMED3Cold;

        } else {

            Capabilities->WakeFromD3 = Device->PowerSupport.PMED3Hot;
        }
    } else {

        //
        // If our parent cannot wake from the D3 state, then we support
        // the D3 state if we support PME3Hot
        //
        Capabilities->WakeFromD3 = Device->PowerSupport.PMED3Hot;
    }

    //
    // Now handle the system state -> device state mapping, starting
    // with the parent's mapping, and rounding down where appropriate.
    //










    highestSupportedSleepState = PowerSystemUnspecified;
    validSystemWakeState = PowerSystemUnspecified;
    validDeviceWakeState = PowerDeviceUnspecified;

    for (systemState = PowerSystemWorking;
         systemState < PowerSystemMaximum;
         systemState++) {

        //
        //
        // Start with the device state this system state corresponds to
        // in the parent, and see if the device actually supports
        // that state.  if not, round down.
        //
        deviceState = Device->Parent->SystemStateMapping[systemState];

        if ((deviceState == PowerDeviceD1) && (Capabilities->DeviceD1 == 0)) {
            deviceState++;
        }

        if ((deviceState == PowerDeviceD2) && (Capabilities->DeviceD2 == 0)) {
            deviceState++;
        }

        //
        // D3 support is the lowest common denominator.
        //
        Capabilities->DeviceState[systemState] = deviceState;

        //
        // While we're looping, keep track of some other stuff,
        // like what the deepest sleep state we support is.
        //
        if ((systemState < PowerSystemHibernate) &&
            (Capabilities->DeviceState[systemState] != PowerDeviceUnspecified)) {

            highestSupportedSleepState = systemState;
        }

        //
        // and what system states are legal for wake
        //
        if ((systemState < Device->Parent->SystemWake) &&
            (deviceState >= Device->Parent->SystemStateMapping[systemState]) &&
            (Device->Parent->SystemStateMapping[systemState] !=
             PowerDeviceUnspecified)) {

            if (((deviceState == PowerDeviceD0) && Capabilities->WakeFromD0) ||
                ((deviceState == PowerDeviceD1) && Capabilities->WakeFromD1) ||
                ((deviceState == PowerDeviceD2) && Capabilities->WakeFromD2) ||
                ((deviceState == PowerDeviceD3) && Capabilities->WakeFromD3)) {

                validSystemWakeState = systemState;
                validDeviceWakeState = deviceState;
            }
        }
    }

    //
    // Now compute latency.
    //
    if ((Device->Parent->SystemWake == PowerSystemUnspecified) ||
        (Device->Parent->DeviceWake == PowerDeviceUnspecified) ||
        (Device->WakeLevel == PowerDeviceUnspecified) ||
        (Device->WakeLevel < Device->Parent->DeviceWake)) {

        //
        // The device doesn't support any kind of wakeup (that we know about)
        // or the device doesn't support wakeup from supported D-states, so
        // set the latency and return
        //
        Capabilities->D1Latency  = 0;
        Capabilities->D2Latency  = 0;
        Capabilities->D3Latency  = 0;

        return STATUS_SUCCESS;
    }

    //
    // From the PCI Power Management spec V1.0, table 18
    // "PCI Function State Transition Delays".
    //
    // D1 -> D0  0
    // D2 -> D0  200 us
    // D3 -> D0  10  ms
    //
    // The latency entries are in units of 100 us.
    //
    Capabilities->D1Latency  = 0;
    Capabilities->D2Latency  = 2;
    Capabilities->D3Latency  = 100;

    //
    // Now see what the device and system sleep states we
    // can wake from are.
    //
    // We can't wake from any lighter state than our parent can.
    //
    Capabilities->SystemWake = Device->Parent->SystemWake;
    Capabilities->DeviceWake = Device->WakeLevel;

    //
    // Change our device wake level to include a state that we support
    //
    if ((Capabilities->DeviceWake == PowerDeviceD0) && !Capabilities->WakeFromD0) {

        Capabilities->DeviceWake++;
    }
    if ((Capabilities->DeviceWake == PowerDeviceD1) && !Capabilities->WakeFromD1) {

        Capabilities->DeviceWake++;
    }
    if ((Capabilities->DeviceWake == PowerDeviceD2) && !Capabilities->WakeFromD2) {

        Capabilities->DeviceWake++;
    }
    if ((Capabilities->DeviceWake == PowerDeviceD3) && !Capabilities->WakeFromD3) {

        Capabilities->DeviceWake = PowerDeviceUnspecified;
        Capabilities->SystemWake = PowerSystemUnspecified;
    }

    //
    // This is our fallback position. If we got here and there is no wake
    // capability using the above method of calcuation, then we should
    // check to see if we noticed a valid wake combination while scanning
    // the S to D mapping information
    //
    if (((Capabilities->DeviceWake == PowerDeviceUnspecified) ||
         (Capabilities->SystemWake == PowerSystemUnspecified)) &&
        ((validSystemWakeState != PowerSystemUnspecified) &&
         (validDeviceWakeState != PowerSystemUnspecified))) {

        Capabilities->DeviceWake = validDeviceWakeState;
        Capabilities->SystemWake = validSystemWakeState;

        //
        // Note that in this case, we might have set DeviceWake to D3, without
        // having set the bit, so "correct" that situation.
        //
        if (validDeviceWakeState == PowerDeviceD3) {

            Capabilities->WakeFromD3 = TRUE;
        }
    }

    //
    // We shouldn't allow Wake From S4, S5, unless the supports the D3 state
    // Even then, we really shouldn't allow S4, S5 unless the device supports
    // the D3Cold PME state.  In either one of these cases, reduce the
    // system wake capability to the state we noticed to be supported before.
    //
    if (Capabilities->SystemWake > PowerSystemSleeping3) {

        if (Capabilities->DeviceWake != PowerDeviceD3) {

            Capabilities->SystemWake = highestSupportedSleepState;
        }

        //
        // This is in a separate if statement so that the code can be easily
        // commented out
        //
        if (!Device->PowerSupport.PMED3Cold) {

            //
            // Reduce the systemwake level to something more realistic
            //
            Capabilities->SystemWake = highestSupportedSleepState;
        }
    }

    //
    // Make sure that S0 maps to D0
    //
    PCI_ASSERT( Capabilities->DeviceState[PowerSystemWorking] == PowerDeviceD0);

    return STATUS_SUCCESS;
}

NTSTATUS
PciQueryTargetDeviceRelations(
    _In_ PPCI_DEVICE Device,
    _Inout_ PDEVICE_RELATIONS *PDeviceRelations
    )
/*++

Routine Description:

    This function builds a DEVICE_RELATIONS structure containing a
    one element array of pointers to the device object for which
    Device is the device extension.

Arguments:

    Device - Pointer to the extension for the device.

    PDeviceRelations - Used to return the pointer to the allocated
        device relations structure.

Return Value:

    NT Status code.

--*/
{
    PDEVICE_RELATIONS deviceRelations;

    PAGED_CODE();

    deviceRelations = PciAllocatePool(NonPagedPoolNx, sizeof(DEVICE_RELATIONS));
    if (deviceRelations == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    deviceRelations->Count = 1;
    deviceRelations->Objects[0] = Device->DeviceObject;

    ObReferenceObject(deviceRelations->Objects[0]);

    if (*PDeviceRelations != NULL) {

        //
        // The caller kindly supplied a device relations structure,
        // it's either too small or exactly the right size.   Throw
        // it away.
        //
        PciFreeExternalPool(*PDeviceRelations);
    }

    *PDeviceRelations = deviceRelations;

    return STATUS_SUCCESS;
}

NTSTATUS
PciQueryEjectionRelations(
    __in PPCI_DEVICE Device,
    __inout PDEVICE_RELATIONS *PDeviceRelations
    )
/*++

Routine Description:

    This function builds a DEVICE_RELATIONS structure containing an array
    of pointers to the device objects that would presumably leave if this
    device were ejected. This is constructed from all the functions of a device.

Arguments:

    Device - Pointer to the extension for the device.

    PDeviceRelations - Used to return the pointer to the allocated
        device relations structure.

Return Value:

    STATUS_SUCCESS.

--*/
{
    PDEVICE_RELATIONS ejectionRelations, oldRelations;
    PDEVICE_OBJECT *object;
    ULONG relationCount;
    PPCI_DEVICE sibling;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = Device->Parent;

    //
    // Search the child Pdo list, looking for devices with the
    // same device number as this one.
    //
    relationCount = 0;
    PciAcquirePassiveLock(&bus->ChildDeviceLock);
    for (sibling = bus->ChildDevices; sibling != NULL; sibling = sibling->Sibling) {

        if ((sibling != Device) && sibling->Present &&
            (sibling->Slot.u.bits.DeviceNumber == Device->Slot.u.bits.DeviceNumber)) {

            relationCount++;
        }
    }

    if (relationCount == 0) {
        goto exit;
    }

    oldRelations = *PDeviceRelations;
    if (oldRelations) {
        PCI_MARK_SAFE_ADDITION(relationCount, oldRelations->Count);
        relationCount += oldRelations->Count;
    }

    ejectionRelations = PciAllocatePool(
                            NonPagedPoolNx,
                            sizeof(DEVICE_RELATIONS)
                            + (relationCount-1)*sizeof(PDEVICE_OBJECT)
                            );
    if (ejectionRelations == NULL) {
        goto exit;
    }

    ejectionRelations->Count = relationCount;

    object = ejectionRelations->Objects;
    if (oldRelations) {

        RtlCopyMemory(ejectionRelations->Objects,
                      oldRelations->Objects,
                      oldRelations->Count * sizeof(PDEVICE_OBJECT)
                      );

        object += oldRelations->Count;

        PciFreeExternalPool(oldRelations);
    }

    for (sibling = bus->ChildDevices; sibling != NULL; sibling = sibling->Sibling) {

        if ((sibling != Device) && (sibling->Present) &&
            (sibling->Slot.u.bits.DeviceNumber == Device->Slot.u.bits.DeviceNumber)) {

            ObReferenceObject(sibling->DeviceObject);
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, __WARNING_POTENTIAL_BUFFER_OVERFLOW_LOOP_DEPENDENT)
            *object++ = sibling->DeviceObject;
        }
    }

    *PDeviceRelations = ejectionRelations;

exit:

    PciReleasePassiveLock(&bus->ChildDeviceLock);

    return STATUS_SUCCESS;
}


NTSTATUS
PciDetermineSlotNumber(
    __in PPCI_DEVICE Device,
    __inout PULONG SlotNumber
    )
/*++

Description:

    Inherit our parent's slot number. This result may be filtered further by
    ACPI and other bus filters.

Arguments:

    Device - the device in question.

    SlotNumber - Pointer to slot number to update

Return Value:

    STATUS_SUCCESS if slot # found

--*/
{
    NTSTATUS status;
    ULONG length;

    PAGED_CODE();

    if (Device->ExpressPort != NULL) {
        status = ExpressDetermineSlotNumber(Device->ExpressPort, SlotNumber);
        if (NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // Maybe our parent has a UI Number that we could 'inherit'.
    //
    if (Device->Parent == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    status = IoGetDeviceProperty(Device->Parent->PhysicalDeviceObject,
                                 DevicePropertyUINumber,
                                 sizeof(*SlotNumber),
                                 SlotNumber,
                                 &length
                                 );
    return status;
}

NTSTATUS
PciDecodesFromDeviceResources(
    __in PPCI_DEVICE Device,
    __out PUSHORT Decodes
    )
/*++

Routine Description:

    This routine looks at a devices allocated resources and determines the
    device decodes that should be enabled for the device.  The decodes are
    defined as the memory, I/O and bus master bits.  The bus master bit
    is not affected by the resource list - whether or not it gets set will
    have already been calculated before this call, and its value should
    be preserved here.  The memory and I/O decodes will be enabled if
    resources of the corresponding type are in the resource list. Another
    instance where memory and I/O decodes are enabled is if this device is
    a subtractive bridge. This is because these bits control the bridge's
    response to memory and I/O transaction on the primary side.

    For resource computation, use the list as returned from PnP. The list
    we get in the Start IRP has been filtered by higher-level drivers, and
    doesn't include any resources that aren't managed by this driver.
    However, if a device has a non-standard I/O range, it is still subject
    to the I/O decode, and we don't want to turn off its access to that range
    by mistakenly thinking that the device has no I/O resources.

Arguments:

    Device - The device to determine the decodes for.

    Decodes - A pointer to a buffer to fill in the decodes.

Return Value:

    STATUS_SUCCESS if the routine succeeds.
    STATUS_INSUFFICIENT_RESOURCES if a memory allocation fails.
    A failure code returned from IoGetDeviceProperty if that call fails.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptor;
    PCM_RESOURCE_LIST resources;
    NTSTATUS status;
    USHORT decodes;

    PAGED_CODE();

    status = PciGetDeviceProperty(Device->DeviceObject,
                                  DevicePropertyAllocatedResources,
                                  &resources
                                  );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // The allocated resources device property returns a buffer which is
    // the concatenation of the raw and translated resources for the
    // device.  Since we only care about the raw resources, and since the
    // raw resources are at the start of the buffer, we can treate this
    // buffer as if it is just a raw resource list.  Now run through
    // them to see if there are any memory or I/O resources.  Assume the decode
    // for a given resource type should remain off until a resource is found to
    // indicate otherwise.
    //

    decodes = 0;
    if (resources != NULL) {

        FOR_ALL_IN_RESOURCE_LIST(resources, cmDescriptor) {

            if (cmDescriptor->Type == CmResourceTypePort) {
                decodes |= PCI_ENABLE_IO_SPACE;
            }
            if ((cmDescriptor->Type == CmResourceTypeMemory) ||
                (cmDescriptor->Type == CmResourceTypeMemoryLarge)) {
                decodes |= PCI_ENABLE_MEMORY_SPACE;
            }
        }

        PciFreePool(resources);
    }

    if (((Device->HeaderType == PCI_BRIDGE_TYPE) ||
         (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) &&
         PciSubtractiveDecodeBridge(Device)) {

        decodes |= (PCI_ENABLE_MEMORY_SPACE | PCI_ENABLE_IO_SPACE);
    }

    //
    // Only enable the busmaster bit if it was originally enabled.
    //

    if ((PciEnableConsoleLockHandling == FALSE) ||
        (Device->BusmasterDisabledOnBoot == 0)) {

        decodes |= PCI_ENABLE_BUS_MASTER;
    }

    *Decodes = decodes;
    return STATUS_SUCCESS;
}
