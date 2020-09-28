/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    devapi.c

Abstract:

    This module contains functions associated with PCI devices











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "devapi.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciRestoreDeviceRegisters(
    __inout PPCI_DEVICE Device,
    __inout PPCI_SAVE_RESTORE_DATA SavedRegisters
    );

VOID
PciResetBridge(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciOnVGAPath(
    __in PPCI_DEVICE Device
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciSaveSlotInformation)
    #pragma alloc_text(PAGE, PciGetSavedSlotInformation)
    #pragma alloc_text(PAGE, PciUpdateSavedDeviceConfiguration)
    #pragma alloc_text(PAGE, PciApplyHPP)
    #pragma alloc_text(PAGE, PciOnPostPath)
    #pragma alloc_text(PAGE, PciOnVGAPath)
    #pragma alloc_text(PAGE, PciSlotPresentInParentMethod)
    #pragma alloc_text(PAGE, PciDeviceOnDebugPath)
#endif

// --------------------------------------------------------------------- Globals

extern PPCI_DEBUG_PORT PciDebugPorts;
extern ULONG           PciDebugPortsCount;

volatile LONG PciDebugSaveDone;
volatile LONG PciDebugSaveNotDone;

// ------------------------------------------------------------------- Functions

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciUpdateHardware(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID ReconnectDevice
    )
/*++

Routine Description:

    This routine is used to update the device hardware with the settings
    stored in the device extension.  These settings include the device
    resources and any saved registers that need to be restored.  To do this,
    the device must be disconnected from the bus.  Because disconnecting
    the device from the bus may cause the machine to crash (if the device is
    a "critical" device like an I/O APIC) this routine may be called in the
    context of KeIpiGenericCall

    In addition, the given ConfigHeader should be updated with the changes
    made to the hardware.

Arguments:

    Device - The PCI_DEVICE in question.

    ReconnectDevice - Supplies a flag used to connect or reconnect the device.


Return Value:

    VOID

--*/
{

    NTSTATUS status;

    NON_PAGED_CODE();

    PciDisconnectDevice(Device);

    status = Device->Operations->SetResources(Device, &Device->Resources);
    if (!NT_SUCCESS(status)) {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x:%x) failed to set device resources %!STATUS!",
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber,
                    status);
        goto exit;
    }

    //
    // This must come after the SetResources, since for a cardbus bridge, the set resources
    // alters the bridge control register's settings for memory prefetchability.
    //
    status = PciRestoreDeviceRegisters(Device, &Device->SavedRegisters);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (((Device->HeaderType == PCI_BRIDGE_TYPE) ||
         (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) &&
        (Device->HackFlags & PCI_HACK_RESET_BRIDGE_ON_POWERUP)) {

        PciResetBridge(Device);
    }

exit:

    //
    // During a link quiescent transaction we perform a Reconnect vs a Connect
    // in order to properly preserve the prior state of the device decodes.
    // This is particularly important for VGA devices which have decodes initially
    // disabled by the BIOS because we honor this config and do not restore them
    // during a standard Connect.
    //

    if (((BOOLEAN)(ULONG_PTR)ReconnectDevice) != FALSE) {
        PciReconnectDevice(Device);

    } else {
        PciConnectDevice(Device);
    }

    //
    // After we've connected the device, restore TPH. This has to be done after
    // connect because it may touch MMIO space on the device, which doesn't work
    // until after the connect.
    //

    if ((Device->ExpressPort != NULL) &&
        (Device->ExpressPort->TphRequesterCapability != 0)) {
        ExpressRestoreTphRequesterCapability(Device->ExpressPort);
    }


    return status;
}

NTSTATUS
PciProbeForRequirements(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID UnusedParameter
    )
/*++

Routine Description:

    This routine probes the given device to determine its resource
    requirements.  To do this, the device must be disconnected from
    the bus.  Because disconnecting the device from the bus may cause
    the machine to crash (if the device is a "critical" device like
    an I/O APIC) this routine may be called in the context of
    KeIpiGenericCall

Parameters:

    Device - The device to probe for resource requirements.

    UnusedParameter - Exists so that the function matches the calling
        convention imposed by PciExecuteCriticalSystemRoutine.  Unused.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(UnusedParameter);

    PciDisconnectDevice(Device);

    status = Device->Operations->GetRequirements(Device,
                                                 &Device->Requirements
                                                 );
    PciReconnectDevice(Device);

    DebugSetDebugFlag(Device, PCI_GOT_REQUIREMENTS);

    return status;
}

NTSTATUS
PciRestoreDeviceRegisters(
    __inout PPCI_DEVICE Device,
    __inout PPCI_SAVE_RESTORE_DATA SavedRegisters
    )
/*++

Routine Description:

    This routine updates a device's configuration registers with
    the versions saved in the device extension.  It does this
    for all registers that the driver modifies except for resource-related
    registers, like BARs.  In addition, it updates the given config
    header with the changes, so that the up-to-date config header
    can be stored in the registry.

Parameters:

    Device - The device to update.

    SavedRegisters - The data to use to update the device.

Return Value:

    NT Status code.

--*/
{
    USHORT bridgeControl, bits;
    USHORT readCacheLineSize;
    USHORT command;

    NON_PAGED_CODE();

    PCI_ASSERT(!Device->Connected);

    PciWriteConfigRegister(Device,
                           common.InterruptLine,
                           &SavedRegisters->InterruptLine
                           );

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        //
        // On a cardbus controller, defer control of the a few cardbus
        // specific bits to the pcmcia driver.
        //
        if (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {

            bits = PCI_ASSERT_BRIDGE_RESET |
                   PCI_ENABLE_CARDBUS_IRQ_ROUTING |
                   PCI_ENABLE_CARDBUS_WRITE_POSTING;

            PciReadConfigRegister(Device,
                                  bridgeCommon.BridgeControl,
                                  &bridgeControl
                                  );

            bridgeControl &= bits;
            SavedRegisters->bridge.BridgeControl &= ~bits;
            SavedRegisters->bridge.BridgeControl |= bridgeControl;
        }

        //
        // Strip the ISA bit from the value written to hardware if this bridge
        // does not function correctly with the ISA bit set.  For these
        // bridges, there are cases where the ISA bit is "logically" set for
        // the purposes of resource arbitration even though it is never
        // reflected back to hardware.
        //
        bridgeControl = SavedRegisters->bridge.BridgeControl;
        if (Device->HackFlags & PCI_HACK_AVOID_HARDWARE_ISA_BIT) {
            bridgeControl &= ~PCI_ENABLE_BRIDGE_ISA;
        }

        PciWriteConfigRegister(Device,
                               bridgeCommon.BridgeControl,
                               &bridgeControl
                               );

        //
        // Validate that the ISA bit stuck with this write unless this bridge
        // either doesn't implement the ISA bit or malfunctions when it is
        // set.  In general resource arbitration depends on the bit being set.
        //
        PciReadConfigRegister(Device,
                              bridgeCommon.BridgeControl,
                              &bridgeControl);

        if ((SavedRegisters->bridge.BridgeControl & PCI_ENABLE_BRIDGE_ISA) &&
            ((bridgeControl & PCI_ENABLE_BRIDGE_ISA) == 0)) {

            if (((Device->HackFlags & PCI_HACK_IGNORE_NON_STICKY_ISA) == 0) &&
                ((Device->HackFlags & PCI_HACK_AVOID_HARDWARE_ISA_BIT) == 0)) {

                TraceEvents(Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_ERROR,
                            DBG_DEVPOWER,
                            "(%x:%x) ISA bit didn't stick",
                            Device->Slot.u.bits.DeviceNumber,
                            Device->Slot.u.bits.FunctionNumber);

                return STATUS_DEVICE_DATA_ERROR;
            }
        }

        PciWriteConfigRegister(Device, bridgeCommon.PrimaryBus, &Device->BusNumber);
        PciWriteConfigRegister(Device,
                               bridgeCommon.SecondaryBus,
                               &SavedRegisters->bridge.SecondaryBusNumber
                               );
        PciWriteConfigRegister(Device,
                               bridgeCommon.SubordinateBus,
                               &SavedRegisters->bridge.SubordinateBusNumber
                               );

    }

    PciWriteConfigRegister(Device, CacheLineSize, &SavedRegisters->CacheLineSize);

    if (Device->ConfigureMwi) {

        //
        // Check if the cache line size stuck which means the hardware liked it
        //
        PciReadConfigRegister(Device, CacheLineSize, &readCacheLineSize);

        if ((readCacheLineSize == SavedRegisters->CacheLineSize) &&
            (readCacheLineSize != 0)) {

            SavedRegisters->Command |= PCI_ENABLE_WRITE_AND_INVALIDATE;

        } else {
            TraceEvents(Device->Parent->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_DEVPOWER,
                        "(%x:%x) cache line size non-sticky, not enabling MWI",
                        Device->Slot.u.bits.DeviceNumber,
                        Device->Slot.u.bits.FunctionNumber);
        }

        PCI_DEVICE_CLEAR_FLAG(Device, ConfigureMwi);
    }

    PciWriteConfigRegister(Device, LatencyTimer, &SavedRegisters->LatencyTimer);

    //
    // If the device has a HyperTransport MSI Mapping capability, enable
    // that now.
    //
    if (Device->HtMsiMapCapability != 0) {
        PciProgramHtMsiMapCapability(Device);
    }

    //
    // Restore the command register, but *not* the decodes.  They will
    // be restored separately.
    //
    command = SavedRegisters->Command & ~PCI_DECODE_ENABLES;
    command |= PCI_DISABLE_LEVEL_INTERRUPT;
    PciWriteConfigRegister(Device, Command, &command);

    //
    // In addition, for Express devices restore the Express specific registers.
    //

    if (Device->ExpressPort != NULL) {
        ExpressRestorePortRegisters(Device->ExpressPort);

    } else if (Device->ExpressCompatibilityPort != NULL) {
        ExpressCompatibilityRestorePortRegisters(
            Device->ExpressCompatibilityPort);

    }

    return STATUS_SUCCESS;
}

BOOLEAN
PciDeviceConnected(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to tell if a device is connected to the PCI bus and
    decoding PCI transactions.  This is determined by checking to see if
    the command register is clear or not.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    TRUE if the device is connected.
    FALSE otherwise.

--*/
{
    USHORT command;

    NON_PAGED_CODE();

    PciReadConfigRegister(Device, Command, &command);

    if (command & (PCI_ENABLE_MEMORY_SPACE |
                   PCI_ENABLE_IO_SPACE |
                   PCI_ENABLE_BUS_MASTER)) {

        return TRUE;
    } else {
        return FALSE;
    }
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciConnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to connect a device to the PCI bus.  This is
    done by setting the I/O, Memory and Bus Master enables in the
    device's command register.

    In addition, update the ConfigHeader with the changes.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    USHORT status, command, decodes;

    NON_PAGED_CODE();

    PCI_ASSERT(!Device->Connected);

    if (Device->HackFlags & PCI_HACK_PRESERVE_COMMAND) {
        return;
    }

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciConnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciReadConfigRegister(Device, Command, &command);

    //
    // We would like to turn on the I/O, memory and bus master enable bits.
    // Mask these against the decodes that have been explicitly disabled
    // on this device, through hacks and other things.
    // Leave interrupts disabled.  They'll be enabled in PciEnableInterrupts.
    //
    decodes = PCI_DECODE_ENABLES & Device->PossibleDecodes & Device->ResourceDecodes;

    command &= ~PCI_DECODE_ENABLES;
    command |= PCI_DISABLE_LEVEL_INTERRUPT;
    command |= decodes;

    PciWriteConfigRegister(Device, Command, &command);

    Device->PreviousCommand = 0;

    //
    // When we first connect the device we want to clear off the status bits
    // indicating errors that may have occured before we got here so we can
    // detect errors that occur from now on.
    //
    status = PCI_STATUS_DATA_PARITY_DETECTED
            | PCI_STATUS_SIGNALED_TARGET_ABORT
            | PCI_STATUS_RECEIVED_TARGET_ABORT
            | PCI_STATUS_RECEIVED_MASTER_ABORT
            | PCI_STATUS_SIGNALED_SYSTEM_ERROR
            | PCI_STATUS_DETECTED_PARITY_ERROR;

    PciWriteConfigRegister(Device, Status, &status);

    //
    // Special case for some devices that require a write to the command
    // register for second time after status register update.
    //
    // N. B. This is the only place where this workaround is required.
    //

    if (Device->HackFlags & PCI_HACK_COMMAND_REWRITE) {
        PciWriteConfigRegister(Device, Command, &command);
    }

    //
    // If this is a bridge then do the same to the Secondary Status.
    // This is conveniently laid out using the same bit definitions as
    // the normal status register.  Unfortunaely they are in different
    // places in the header.
    //
    if (Device->HeaderType == PCI_BRIDGE_TYPE) {
        PciWriteConfigRegister(Device, type1.SecondaryStatus, &status);
    } else if (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {
        PciWriteConfigRegister(Device, type2.SecondaryStatus, &status);
    }

    if (Device->ExpressPort != NULL) {

        //
        // If this is an SR-IOV device and the driver supports SR-IOV then
        // enable the VF BARs.
        //

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciEnableSriovDecodes(Device);
        }

        ExpressPortObffEnable(Device->ExpressPort);
    }

    PCI_DEVICE_SET_FLAG(Device, Connected);

    return;
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciDisconnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to disconnect a device from the PCI bus.  This is
    done by clearing the I/O, Memory and Bus Master enables in the
    device's command register.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    USHORT command = 0;

    NON_PAGED_CODE();

    if (!Device->Connected) {
        return;
    }

    PCI_ASSERT(Device->PreviousCommand == 0);

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciDisconnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciReadConfigRegister(Device, Command, &command);

    Device->PreviousCommand = command;

    command &= ~(PCI_ENABLE_IO_SPACE | PCI_ENABLE_MEMORY_SPACE);
    command |= PCI_DISABLE_LEVEL_INTERRUPT;

    //
    //  If our debugger is bus mastering then dont clear this bit as it
    //  will blow away the DMA engine on the card.
    //

    if (!(Device->OnDebugPath)) {
        command &= ~PCI_ENABLE_BUS_MASTER;
    }

    if (Device->ExpressPort != NULL) {

        //
        // If this is an SR-IOV device and the driver supports SR-IOV,
        // disable the VF BARs now.
        //

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciDisableSriovDecodes(Device);
        }

        ExpressPortObffDisable(Device->ExpressPort);
    }

    PciWriteConfigRegister(Device, Command, &command);

    PCI_DEVICE_CLEAR_FLAG(Device, Connected);

    return;
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciReconnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to reconnect a device from the PCI bus.  This is
    done by setting the I/O, Memory and Bus Master enables in the
    device's command register to the values they were before a previous
    call to PciDisconnectDevice.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciReconnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciWriteConfigRegister(Device, Command, &Device->PreviousCommand);
    Device->PreviousCommand = 0;

    if (Device->ExpressPort != NULL) {

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciEnableSriovDecodes(Device);
        }

        ExpressPortObffEnable(Device->ExpressPort);
    }

    PCI_DEVICE_SET_FLAG(Device, Connected);
}

PCI_DISCONNECTION_POLICY
PciCanDisableDevice(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to check whether a device can be disabled,
    meaning that it can be disconnected from the bus for an indefinite
    period of time, for example because a IRP_MN_STOP_DEVICE IRP has
    been sent.  This is not used to see whether a device can be
    disconnected from the bus for a transient operation, like probing
    the BARs.  Disconnecting for transient operations is not considered
    disabling.  Note that since disabling a device can mean an implicit
    transition to D3, the ability of the device to enter D3 is conveyed by this
    routine as well.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    PowerDownDevice if the device can be disconnected and powered down.

    DisconnectDevice if the device must be kept in a high power state but can
        be disconnected from decoding its resources.

    KeepDeviceActive if the device must be kept fully active.

--*/
{

    BOOLEAN active;

    NON_PAGED_CODE();

    active = FALSE;
    if ((Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) ||
        (Device->HackFlags & PCI_HACK_NO_DISABLE_FOR_STOP_REMOVE) ||
        (Device->HackFlags & PCI_HACK_DONT_DISABLE)) {

        active = TRUE;
    }

    if (Device->HackFlags & PCI_HACK_CB_SHARE_CMD_BITS) {
        //
        // This is a multifunction cardbus controller with a shared
        // command register.  Never turn off any of the functions because it has
        // the unfortunate side effect of turning off all of them!
        //









        active = TRUE;
    }

    //
    // Devices on the debugging path should always stay on.  Debugging devices
    // always fail QueryRemove and QueryStop, but Removes can come through
    // without queries (after a failed start, for example).
    //
    if (Device->OnDebugPath) {

        active = TRUE;
    }

    if (active != FALSE) {
        return KeepDeviceActive;
    }

    if (Device->HackFlags & PCI_HACK_DISABLE_POWER_DOWN) {
        return DisconnectDevice;
    }

    //
    // If a bridge cannot enter D3 because its bus cannot enter B2, keep the
    // bridge in a high power state but disconnect its decodes.
    //

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) &&
        (Device->BridgeFlags.D3Illegal == 1)) {

        return DisconnectDevice;
    }

    return PowerDownDevice;
}

PCI_DISCONNECTION_POLICY
PciCanDisableDeviceForPowerDown(
    __in PPCI_DEVICE Device,
    __in DEVICE_POWER_STATE PowerState,
    __in POWER_ACTION PowerAction
    )
/*++

Routine Description:

    This routine is used to check whether a device can be disabled in
    order to power it down.  This is similar to PciCanDisableDevice,
    but takes into account some power-specific behaviors.

Arguments:

    Device - The PCI_DEVICE in question.

    PowerState - The power state the device will be in if it is
        powered down.

    PowerAction - The cause of the request to power down the device.

Return Value:

    PowerDownDevice if the device can be powered down normally.

    DisconnectDevice if the device must be kept in a high power state but can
        be disconnected from decoding its resources.

    KeepDeviceActive if the device must be kept fully active.

--*/
{

    BOOLEAN active;

    NON_PAGED_CODE();

    active = FALSE;

    //
    // Devices that shouldn't be disabled ever also shouldn't be
    // disabled for power down.
    //

    if ((Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) ||
        (Device->HackFlags & PCI_HACK_CB_SHARE_CMD_BITS) ||
        (Device->HackFlags & PCI_HACK_DONT_DISABLE)) {

        active = TRUE;
    }

    if (Device->HackFlags & PCI_HACK_DISABLE_POWER_DOWN) {
        active = TRUE;
    }

    //
    // If this device is on the debug path then don't power it down so we
    // can report if this crashes the machine...
    //

    if (Device->OnDebugPath) {
        active = TRUE;
    }

    //
    // Device driver should likely not be powering down any device
    // that's on the hibernate path or the crashdump path
    //

    if ((PowerAction == PowerActionHibernate) &&
        (Device->DeviceUsage.Hibernate || Device->DeviceUsage.CrashDump)) {

        active = TRUE;
    }

    if ((Device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
        (Device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI)) {

        //
        // Don't power down bridges for warm reboots, because if
        // the video or boot device is behind the bridge and the BIOS
        // doesn't power things up, we won't be able to get to the
        // boot or video path.
        //

        if ((PowerState == PowerDeviceD3) &&
            (PowerAction == PowerActionShutdownReset)) {

            active = TRUE;
        }
    }

    if (Device->OnPostPath != FALSE) {

        //
        // If we are a device on the VGA path then don't turn off for shutdown
        // so we can display the "Safe to turn off your machine" screen.  For
        // hibernate we also don't want to turn off so we can display the
        // "Dumping to disk" progress bar.  The video device is not put on the
        // hibernation path so that its driver can be updated.
        //

        if ((PowerState == PowerDeviceD3) &&
            ((PowerAction == PowerActionShutdownReset) ||
             (PowerAction == PowerActionShutdownOff) ||
             (PowerAction == PowerActionShutdown) ||
             (PowerAction == PowerActionHibernate))) {

            active = TRUE;

        }
    }

    if (active != FALSE) {
        return KeepDeviceActive;
    }

    //
    // If a bridge cannot enter D3 because its bus cannot enter B2, keep the
    // bridge in a high power state but disconnect its decodes.
    //

    if ((PowerState == PowerDeviceD3) &&
        (Device->HeaderType == PCI_BRIDGE_TYPE) &&
        (Device->BridgeFlags.D3Illegal == 1)) {

        return DisconnectDevice;
    }

    return PowerDownDevice;
}

NTSTATUS
PciReadSlotIdData(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __out PPCI_DEVICE_ID_DATA IdData
    )
/*++

Routine Description:

    This routine reads the configuration data necessary to
    identify a device in the given bus/device/function,
    and returns it in the IdData structure.

Arguments:

    Bus - The PCI_BUS structure representing the bus the device is on.

    Slot - The slot the device is in.

    IdData - On success, this structure will contain the identification data
        of the device.

Return Value:

    STATUS_SUCCESS - If the data was retrieved successfully
    STATUS_DEVICE_PROTOCOL_ERROR - If there was an error communicating
        with the device while retrieving the identification data.

--*/
{
    PCI_CAPABILITIES_HEADER header;
    UCHAR headerType;
    USHORT status;
    UCHAR capPtr;

    NON_PAGED_CODE();

    RtlZeroMemory(IdData, sizeof(PCI_DEVICE_ID_DATA));

    PciReadSlotConfigRegister(Bus, Slot, VendorID, &IdData->VendorID);
    if ((IdData->VendorID == 0xFFFF) || (IdData->VendorID == 0)) {
        return STATUS_DEVICE_DOES_NOT_EXIST;
    }

    PciReadSlotConfigRegister(Bus, Slot, DeviceID, &IdData->DeviceID);
    PciReadSlotConfigRegister(Bus, Slot, RevisionID, &IdData->RevisionID);
    PciReadSlotConfigRegister(Bus, Slot, ProgIf, &IdData->ProgIf);
    PciReadSlotConfigRegister(Bus, Slot, SubClass, &IdData->SubClass);
    PciReadSlotConfigRegister(Bus, Slot, BaseClass, &IdData->BaseClass);
    PciReadSlotConfigRegister(Bus, Slot, HeaderType, &IdData->HeaderType);

    //
    // Now get the subsystem IDs.  These are in a different place per header type.
    //
    headerType = IdData->HeaderType & ~PCI_MULTIFUNCTION;
    switch (headerType) {

    case PCI_DEVICE_TYPE:

        PciReadSlotConfigRegister(Bus, Slot, type0.SubVendorID, &IdData->SubVendorID);
        PciReadSlotConfigRegister(Bus, Slot, type0.SubSystemID, &IdData->SubSystemID);
        break;

    case PCI_BRIDGE_TYPE:

        PciReadSlotConfigRegister(Bus, Slot, Status, &status);
        if (!(status & PCI_STATUS_CAPABILITIES_LIST)) {
            break;
        }
        PciReadSlotConfigRegister(Bus, Slot, type1.CapabilitiesPtr, &capPtr);

        while (capPtr) {

            if ((capPtr < PCI_TYPE1_HEADER_LENGTH) ||
                ((capPtr & 0x3) != 0) ||
                (capPtr > (sizeof(PCI_COMMON_CONFIG)
                           - sizeof(PCI_CAPABILITIES_HEADER)))) {

                return STATUS_DEVICE_PROTOCOL_ERROR;
            }

            PciReadSlotConfig(Bus,
                              Slot,
                              &header,
                              capPtr,
                              sizeof(PCI_CAPABILITIES_HEADER)
                              );
            if (header.CapabilityID == PCI_CAPABILITY_ID_P2P_SSID) {

                PciReadSlotConfig(Bus,
                                  Slot,
                                  &IdData->SubVendorID,
                                  capPtr
                                  + FIELD_OFFSET(PCI_SUBSYSTEM_IDS_CAPABILITY,SubVendorID),
                                  sizeof(USHORT)
                                  );

                PciReadSlotConfig(Bus,
                                  Slot,
                                  &IdData->SubSystemID,
                                  capPtr
                                  + FIELD_OFFSET(PCI_SUBSYSTEM_IDS_CAPABILITY,SubSystemID),
                                  sizeof(USHORT)
                                  );
                break;
            }

            capPtr = header.Next;
        }
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:

        PciReadSlotConfigRegister(Bus,
                                  Slot,
                                  type2.SubVendorID,
                                  &IdData->SubVendorID
                                  );
        PciReadSlotConfigRegister(Bus,
                                  Slot,
                                  type2.SubSystemID,
                                  &IdData->SubSystemID
                                  );
        break;

    default:

        //
        // Illegal header type
        //
        return STATUS_DEVICE_PROTOCOL_ERROR;
    }

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciGetSavedSlotInformation(
    _In_ PPCI_BUS Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ PWSTR InfoKeyName,
    _In_ ULONG SlotInfoSize,
    _Out_ PVOID SlotInfo
    )
/*++

Description:

    This retrieves the saved configuration of a device from the registry.
    This is for information that needs to be maintained across
    PDO deletions.

Arguments:

    Bus - the parent bus of the device in question.

    Slot - The slot number of the device.

    InfoKeyName - The registry key name to read

    SlotInfoSize - The size in bytes of the SlotInfo buffer.

    SlotInfo - Pointer to the buffer receiving the slot information.

Return Value:

    NT Status code.

--*/
{
    WCHAR valueName[sizeof("DEV_xx&FUN_xx")];
    PCHAR valueBuffer;
    ULONG valueLength;
    HANDLE deviceHandle;
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(SlotInfoSize);

    deviceHandle = NULL;

    //
    // The saved config information is stored in a subkey of the *parent's*
    // PnP device registry key.
    //
    status = IoOpenDeviceRegistryKey(Bus->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_READ | KEY_WRITE,
                                     &deviceHandle
                                     );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Under the InfoKeyName subkey are a number of values, each with the
    // format DEV_xx&FUN_xx.  These are either the initial configs of the
    // devices underneath this parent, or other persisted data for the
    // specified slot.  The value data is a REG_BINARY containing the
    // config header for the device as saved.
    //

    status = RtlStringCbPrintfW(valueName,
                                sizeof(valueName),
                                L"DEV_%02x&FUN_%02x",
                                Slot.u.bits.DeviceNumber,
                                Slot.u.bits.FunctionNumber
                                );

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    valueBuffer = NULL;
    valueLength = 0;
    status = PciGetRegistryValue(valueName,
                                 InfoKeyName,
                                 deviceHandle,
                                 REG_BINARY,
                                 &valueBuffer,
                                 &valueLength
                                 );
    if (NT_SUCCESS(status)) {

        PCI_ASSERT(valueLength == SlotInfoSize);

        RtlCopyMemory(SlotInfo, valueBuffer, valueLength);
        PciFreePool(valueBuffer);
    }

exit:

    if (deviceHandle) {
        ZwClose(deviceHandle);
    }

    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciSaveSlotInformation(
    _In_ PPCI_DEVICE Device,
    _In_ PWSTR InfoKeyName,
    _In_ USHORT InfoKeySize,
    _In_ BOOLEAN IsVolatile,
    _In_ ULONG SlotInfoSize,
    _In_ PVOID SlotInfo
    )
/*++

Description:

    This saves the configuration of a device in the registry.
    This should be done when the device is first enumerated
    so that the configuration will be maintained across PDO
    deletions.  It should also be done when changes are made
    to the device that should be maintained across PDO deletions,
    like the interrupt line register being updated.  This
    configuration data is saved in a volatile registry key.

    For hot-swap/replace scenarios, where a device is replaced
    with exactly the same device (differing only by serial number),
    it is important to not report the new serial number in order
    to maintain device specific settings (iSCSI boot etc). This
    information is therefore saved in a non-volatile key that
    will persist across reboots.

Arguments:

    Device - The device to save the config header of.

    InfoKeyName - The name of the registry key to be created/updated under
                  the DeviceParameters key for this device.

    InfoKeySize - Size in bytes of the InfoKeyName string.

    IsVolatile - If true then key will be volatile and not persist
                 across reboots.

    SlotInfoSize - Size in bytes of the SlotInfo to be saved.

    SlotInfo - Pointer to the slot information to be saved.

Return Value:

    Status

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES attributes;
    UNICODE_STRING unicodeString;
    HANDLE deviceHandle, configHandle;
    WCHAR buffer[sizeof("DEV_xx&FUN_xx")];

    PAGED_CODE();

    deviceHandle = NULL;
    configHandle = NULL;

    status = IoOpenDeviceRegistryKey(Device->Parent->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_WRITE,
                                     &deviceHandle
                                     );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    unicodeString.Length = (unicodeString.MaximumLength = InfoKeySize) - sizeof(WCHAR);
    unicodeString.Buffer = InfoKeyName;

    InitializeObjectAttributes(&attributes,
                               &unicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               deviceHandle,
                               NULL
                               );

    status = ZwCreateKey(&configHandle,
                         KEY_WRITE,
                         &attributes,
                         0,
                         NULL,
                         IsVolatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE,
                         NULL
                         );
    if (!NT_SUCCESS(status) || (configHandle == 0)) {
        goto exit;
    }

    status = RtlStringCbPrintfW(buffer,
                                sizeof(buffer),
                                L"DEV_%02x&FUN_%02x",
                                Device->Slot.u.bits.DeviceNumber,
                                Device->Slot.u.bits.FunctionNumber
                                );

    if (!NT_SUCCESS(status)) {
        ERROR("PciSaveSlotInformation: Error constructing device id (0x%08X\n).\n",
              status);

        goto exit;
    }

    RtlInitUnicodeString(&unicodeString, buffer);

    status = ZwSetValueKey(configHandle,
                           &unicodeString,
                           0,
                           REG_BINARY,
                           SlotInfo,
                           SlotInfoSize
                           );

exit:

    if (deviceHandle) {
        ZwClose(deviceHandle);
    }
    if (configHandle) {
        ZwClose(configHandle);
    }
    return status;
}

NTSTATUS
PciUpdateSavedDeviceConfiguration(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine saves away the device state that needs to be preserved in the
    registry.  The registry saved config is used to save settings across
    PDO deletions.  It is a combination of the current settings and the BIOS
    settings.

    The resource settings of a device that are computed by the OS should be
    overwritten by the settings computed by the BIOS, since the registry config
    is used to determine the preferred requirements for a device.  Also, the bus
    number assignment should not be changed from the assignment given by the
    BIOS since the registry config is used to determine whether a device is
    on the debug path, which is determined by BIOS-assigned bus/device/function.

Arguments:

    Device - The device to update the saved config of.

Return Value:

    NT Status code.

--*/
{
    PCI_SAVE_RESTORE_DATA saveRestore;
    PCI_SAVED_CONFIGURATION config;
    NTSTATUS status;

    PAGED_CODE();

    status = PciGetSavedDeviceConfiguration(Device, &config);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // For the most part, the currently saved config should be maintained.  This
    // is so that BIOS-configurations are preserved.  However, get the new values
    // of the registers that are saved/restored, to comprehend updates based
    // on _HPP and other such things.
    //
    RtlCopyMemory(&saveRestore,
                  &Device->SavedRegisters,
                  sizeof(PCI_SAVE_RESTORE_DATA)
                  );

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        //
        // Do NOT update the bus numbers in the saved config
        // because the original bus numbers from the BIOS
        // must be preserved to figure out debug path.
        //
        saveRestore.bridge.SecondaryBusNumber
            = config.SaveRestore.bridge.SecondaryBusNumber;
        saveRestore.bridge.SubordinateBusNumber
            = config.SaveRestore.bridge.SubordinateBusNumber;
    }

    RtlCopyMemory(&config.SaveRestore, &saveRestore, sizeof(PCI_SAVE_RESTORE_DATA));


    //
    // Note that we don't update the resources, ID data or ICH windows.
    // We want to keep the BIOS configured resources around, and same
    // for the ICH windows.  the ID data doesn't change, so it doesn't need
    // to be updated.
    //
    return PciSaveDeviceConfiguration(Device, &config);
}

VOID
PciResetBridge(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine issues a Secondary Bus reset to a PCI-PCI bridge.
    This should only be done for bridges that explicitly require
    this behavior on power-up.

Arguments:

    Device - device extension representing the bridge.

Return Value:

    VOID

--*/
{
    USHORT  bridgeControl;

    NON_PAGED_CODE();

    PCI_ASSERT(!Device->OnDebugPath);
    PCI_ASSERT(Device->HeaderType != PCI_DEVICE_TYPE);

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) resetting bridge",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciReadConfigRegister(Device, bridgeCommon.BridgeControl, &bridgeControl);

    bridgeControl |= PCI_ASSERT_BRIDGE_RESET;

    PciWriteConfigRegister(Device, bridgeCommon.BridgeControl, &bridgeControl);

    //
    // Per PCI 2.1, reset must remain asserted for a minimum
    // of 100 us.
    //

    PciDevice_Stall(Device, 100);
    bridgeControl &= ~PCI_ASSERT_BRIDGE_RESET;
    PciWriteConfigRegister(Device, bridgeCommon.BridgeControl, &bridgeControl);
}

NTSTATUS
PciApplyHPX(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine applies the _HPX parameters read from the ACPI BIOS
    node of the parent of a device to the device itself.  _HPX supercedes
    any _HPP objects.  It should only be called for devices that look like 
    they have been hot-plugged, that is, I/O, memory and bus master decodes 
    are off.

Arguments:

    Device - the device to apply the _HPP to.

Return Value:

    NT Status code.

--*/
{

    PPCI_BUS parentBus;
    NTSTATUS status;

    status = STATUS_NOT_FOUND;
    parentBus = Device->Parent;

    //
    // HPX is only relevent for devices that
    // were initially unconfigured.
    //

    if (Device->NoInitialConfig == 0) {
        return STATUS_NOT_SUPPORTED;
    }

    if (parentBus->AcquiredHPX) {

        TRACE(ENUM_TRACE_LEVEL,
              "Device 0x%p inheriting parent HPX\n",
              Device
              );

        //
        // _HPX is composed of up to 3 different packages, each
        // of which is only applicable to specific type of device
        // type (PCIe, PCIX, PCI).
        //

        if ((Device->ExpressCapability != 0) && 
            (parentBus->_HPX.ValidTypes & HPX_TYPE_2_VALID)) {
                
                status = ExpressApplyHPXSettings(Device->ExpressPort, 
                                                 &parentBus->_HPX);
            
        } else if ((Device->PciXCapability != 0) && 
                  (parentBus->_HPX.ValidTypes & HPX_TYPE_1_VALID)) {
                
                //
                // Type1 support is intentionally not implemented until 
                // telemetry shows that it is a customer scenario.
                //

                NOTHING;

        } else {

            if (parentBus->_HPX.ValidTypes & HPX_TYPE_0_VALID) {
                Device->SavedRegisters.CacheLineSize = parentBus->_HPX.type0.CacheLineSize;
                Device->SavedRegisters.LatencyTimer = parentBus->_HPX.type0.LatencyTimer;
                if (parentBus->_HPX.type0.EnablePERR != FALSE) {
                    Device->SavedRegisters.Command |= PCI_ENABLE_PARITY;
                } else {
                    Device->SavedRegisters.Command &= ~(PCI_ENABLE_PARITY);
                }

                if (parentBus->_HPX.type0.EnableSERR != FALSE) {
                    Device->SavedRegisters.Command |= PCI_ENABLE_SERR;
                } else {
                    Device->SavedRegisters.Command &= ~(PCI_ENABLE_SERR);
                }

                //
                // MWI should be set if the CacheLineSize sticks, but we can't check
                // that until Start, when the decodes are disabled for us to poke around.
                // For now, just mark that we need to do this.
                //

                PCI_DEVICE_SET_FLAG(Device, ConfigureMwi);
                status = STATUS_SUCCESS;
            }
        } 
    }

    return status;
}

VOID
PciApplyHPP(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine applies the _HPP parameters read from the ACPI BIOS
    node of the parent of a device to the device itself.  _HPP alters
    the cacheline size, latency timer and command registers.  It should
    only be called for devices that look like they have been hot-plugged,
    that is, I/O, memory and bus master decodes are off and the latency
    timer and cache line size registers are defaults.

Arguments:

    Device - the device to apply the _HPP to.

Return Value:

    NT Status code.

--*/
{
    PPCI_BUS parentBus;

    PAGED_CODE();

    parentBus = Device->Parent;
    if (parentBus->AcquiredHPP) {

        TRACE(ENUM_TRACE_LEVEL,
              "Device 0x%p inheriting parent HPP\n",
              Device
              );

        Device->SavedRegisters.CacheLineSize = parentBus->_HPP.CacheLineSize;
        Device->SavedRegisters.LatencyTimer = parentBus->_HPP.LatencyTimer;
        if (parentBus->_HPP.EnablePERR != FALSE) {
            Device->SavedRegisters.Command |= PCI_ENABLE_PARITY;
        } else {
            Device->SavedRegisters.Command &= ~(PCI_ENABLE_PARITY);
        }

        if (parentBus->_HPP.EnableSERR != FALSE) {
            Device->SavedRegisters.Command |= PCI_ENABLE_SERR;
        } else {
            Device->SavedRegisters.Command &= ~(PCI_ENABLE_SERR);
        }

        //
        // MWI should be set if the CacheLineSize sticks, but we can't check
        // that until Start, when the decodes are disabled for us to poke around.
        // For now, just mark that we need to do this.
        //
        PCI_DEVICE_SET_FLAG(Device, ConfigureMwi);
    }

    return;
}

BOOLEAN
PciBridgeOnVGAPath(
    __in PPCI_DEVICE Device
    )
/*++

Description:

    A bridge is considered on the VGA path if bit 3 in the Bridge Control
    register is set

Arguments:

    Device - The extension for the device in question

Return Value:

    TRUE if we are on the VGA path, FALSE otherwise

--*/
{
    USHORT bridgeControl;

    PCI_ASSERT(Device->BaseClass == PCI_CLASS_BRIDGE_DEV);

    if ((Device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI) ||
        (Device->SubClass == PCI_SUBCLASS_BR_CARDBUS)) {

        PciReadConfigRegister(Device,
                              bridgeCommon.BridgeControl,
                              &bridgeControl
                              );
        if (bridgeControl & PCI_ENABLE_BRIDGE_VGA) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOLEAN
PciOnPostPath (
    __in PPCI_DEVICE Device
    )

/*++

Description:

    This routine guesses if we are on the POST device path or not. It attempts
    to do this by querying the graphics memory pointer used by the kernel and
    comparing it to the resources in the BARs for this device.

Arguments:

    Device - The extension for the device in question

Return Value:

    TRUE if we are on the POST path, FALSE otherwise

--*/

{
    SYSTEM_BOOT_GRAPHICS_INFORMATION BootGraphicsInfo;
    PCI_DEVICE_RESOURCES DeviceResources;
    PHYSICAL_ADDRESS End;
    USHORT Flags;
    PHYSICAL_ADDRESS FrameBuffer;
    ULONG Index;
    BOOLEAN OnPostPath;
    PPCI_DEVICE_RESOURCES Resources;
    PHYSICAL_ADDRESS Start;
    NTSTATUS Status;

    PAGED_CODE();

    OnPostPath = FALSE;
    if (Device->OnPostPath != FALSE) {
        OnPostPath = TRUE;
        goto OnPostPathEnd;
    }

    //
    // Query the boot graphics memory pointer used by the kernel.
    //

    Status = ZwQuerySystemInformation(SystemBootGraphicsInformation,
                                      &BootGraphicsInfo,
                                      sizeof(BootGraphicsInfo),
                                      NULL);

    if (!NT_SUCCESS(Status)) {
        goto OnPostPathEnd;
    }

    FrameBuffer = BootGraphicsInfo.FrameBuffer;

    //
    // Step through all the resouces and check if any of them overlap the
    // memory used by the kernel, if so, mark this device as a post device.
    //

    Resources = &DeviceResources;
    Status = Device->Operations->GetResources(Device, &DeviceResources);
    if (!NT_SUCCESS(Status)) {
        Resources = &Device->Resources;
    }

    for (Index = 0; Index < PCI_DEVICE_RESOURCE_COUNT; Index += 1) {
        if (Resources->ByIndex[Index].Type == CmResourceTypeMemory) {
            Start = Resources->ByIndex[Index].u.Memory.Start;
            End.QuadPart = Start.QuadPart;
            End.QuadPart += Resources->ByIndex[Index].u.Memory.Length;

        } else if (Resources->ByIndex[Index].Type == CmResourceTypeMemoryLarge) {
            Flags = Resources->ByIndex[Index].Flags;
            if ((Flags & CM_RESOURCE_MEMORY_LARGE_40) != FALSE) {
                Start = Resources->ByIndex[Index].u.Memory40.Start;
                End.QuadPart = Start.QuadPart;
                End.QuadPart += Resources->ByIndex[Index].u.Memory40.Length40;

            } else if ((Flags & CM_RESOURCE_MEMORY_LARGE_48) != FALSE) {
                Start = Resources->ByIndex[Index].u.Memory48.Start;
                End.QuadPart = Start.QuadPart;
                End.QuadPart += Resources->ByIndex[Index].u.Memory48.Length48;

            } else if ((Flags & CM_RESOURCE_MEMORY_LARGE_64) != FALSE) {
                Start = Resources->ByIndex[Index].u.Memory64.Start;
                End.QuadPart = Start.QuadPart;
                End.QuadPart += Resources->ByIndex[Index].u.Memory64.Length64;

            } else {
                continue;
            }

        } else {
            continue;
        }

        //
        // If there is any overlap between this device's memory ranges and the
        // frame buffer pointer, then just treat the device as a post device.
        // path.
        //

        if ((Start.QuadPart <= FrameBuffer.QuadPart) &&
            (FrameBuffer.QuadPart <= End.QuadPart)) {

            OnPostPath = TRUE;
            break;
        }
    }

OnPostPathEnd:

    //
    // This is not really required any more but to maintain the legacy PCI
    // behavior, check if this device is on the VGA path as well.
    //

    if (OnPostPath == FALSE) {
        OnPostPath = PciOnVGAPath(Device);
    }

    return OnPostPath;
}

BOOLEAN
PciOnVGAPath(
    __in PPCI_DEVICE Device
    )
/*++

Description:

    Guesses if we are on the VGA path or not.  A device is considered
    on the VGA path if it has a video controller class code or if
    it is a P-P bridge with its VGA bit set.

Arguments:

    Device - The extension for the device in question

Return Value:

    TRUE if we are on the VGA path, FALSE otherwise

--*/
{

    PAGED_CODE();

    switch (Device->BaseClass) {

        case PCI_CLASS_BRIDGE_DEV:

            //
            // We don't want to turn off bridges that might have the VGA card behind
            // then otherwise video stops working.  Seeing as we can't actually tell
            // where the VGA card is use the hint that if the bridge is passing VGA
            // ranges the video card is probably somewhere down there.
            //
            return PciBridgeOnVGAPath(Device);

        case PCI_CLASS_DISPLAY_CTLR:

            if (Device->SubClass == PCI_SUBCLASS_VID_VGA_CTLR) {
                return TRUE;
            }
            break;

        case PCI_CLASS_PRE_20:

            if (Device->SubClass == PCI_SUBCLASS_PRE_20_VGA) {
                return TRUE;
            }
            break;
    }

    return FALSE;
}

BOOLEAN
PciSubtractiveDecodeBridge(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine determines whether a device is
    a subtractive decode bridge or not.

Parameters:

    Device - The device in question

Return Value:

    TRUE if this device is a subtractive decode bridge
    FALSE otherwise.

--*/
{
    NON_PAGED_CODE();

    //
    // Only P-P bridges can be subtractive.  Assert if this was
    // called on a device.  Fail but don't assert if it was called
    // on a cardbus bridge.
    //
    if (Device->HeaderType != PCI_BRIDGE_TYPE) {
        ERROR("PciSubtractiveDecodeBridge: Not a valid bridge.\n");
        return FALSE;
    }

    //
    // Bridges are subtractive if their programming interface of 0x1 or we
    // have the appropriate hack flag set.
    //
    // They are also subtractive if the IO Limit register isn't sticky (we tried
    // to write 0xFF to it (see MassageHeader) but the top nibble which is meant
    // to be sticky did't stick).
    //
    // By tradition (NT4/Win9x) this means that the bridge performs subtractive
    // decode of both memory and IO.Unfortunatly the PCI spec says that IO in
    // bridges is optional and so if anyone builds a bridge that doesn't do IO
    // then we will think they have subtractive IO.  One might think that the
    // non-optional memory limit register would have been a better choice but
    // seeing as this is how it works so lets be consistent.
    //
    if ((Device->ProgIf == 0x1) ||
        (Device->Requirements.type1.IoWindow.Type == CmResourceTypeNull) ||
        (Device->HackFlags & PCI_HACK_SUBTRACTIVE_DECODE)) {

        return TRUE;
    }

    return FALSE;
}

BOOLEAN
PciSlotPresentInParentMethod(
    __in PPCI_DEVICE Device,
    __in ULONG Method
    )
/*++

Description:

    This function checks if the slot this device is in is present in a
    Method named package on the parent of this device.

Arguments:

    Device - The device to look for the slot of in the method data.

    Method - The Parents method to examine

Return Value:

    TRUE if present, FALSE otherwise

--*/
{
    PACPI_EVAL_OUTPUT_BUFFER output;
    ACPI_EVAL_INPUT_BUFFER input;
    PACPI_METHOD_ARGUMENT argument;
    ULONG outputSize;
    ULONG count, adr;
    NTSTATUS status;
    BOOLEAN result;

    PAGED_CODE();

    result = FALSE;
    output = NULL;

    //
    // Allocate a buffer big enough for all possible slots
    //
    outputSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER) + (sizeof(ACPI_METHOD_ARGUMENT)
                                                    * PCI_MAX_DEVICES
                                                    * PCI_MAX_FUNCTION);

    output = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, outputSize);
    if (!output) {
        goto exit;
    }

    RtlZeroMemory(&input, sizeof(ACPI_EVAL_INPUT_BUFFER));
    RtlZeroMemory(output, outputSize);

    //
    // Send a IOCTL to ACPI to request it to run the method on this device's
    // parent if the method it is present
    //
    input.Signature = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
    input.MethodNameAsUlong = Method;

    status = PciSendIoctl(Device->Parent->PhysicalDeviceObject,
                          IOCTL_ACPI_EVAL_METHOD,
                          &input,
                          sizeof(ACPI_EVAL_INPUT_BUFFER),
                          output,
                          outputSize
                          );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Format my slot number as an _ADR style integer
    //
    adr = (Device->Slot.u.bits.DeviceNumber << 16) |
          Device->Slot.u.bits.FunctionNumber;

    for (count = 0; count < output->Count; count++) {

        //
        // Walking the arguments works like this because we are a package of
        // integers
        //
        argument = &output->Argument[count];

        if (argument->Type != ACPI_METHOD_ARGUMENT_INTEGER) {
            goto exit;
        }

        if (argument->Argument == adr) {

            //
            // Jackpot!
            //
            result = TRUE;
            break;
        }
    }

exit:

    if (output) {
        PciFreePool(output);
    }

    return result;
}

BOOLEAN
PciDeviceOnDebugPath(
    __in PPCI_DEVICE Device,
    __in PPCI_SAVED_CONFIGURATION ConfigData
    )
/*++

Description:

    This function checks if device is on the path to the debugging device

    NOTE: PDO is only partially initialized at this point. Take care to insure
          that fields touched here are valid.

Arguments:

    Device - The device in question.

    ConfigData - The initial config of the device.

Return Value:

    TRUE if on the debug path, FALSE otherwise

--*/

{
    NTSTATUS status;
    PPCI_DEBUG_PORT current;
    ULONG parentBusNumber;
    PCI_SAVED_CONFIGURATION parentConfig;
    USHORT segmentNumber;

    PAGED_CODE();

    PCI_ASSERT(PciDebugPortsCount <= MAX_DEBUGGING_DEVICES_SUPPORTED);

    //
    // We can't be on the debug path if we aren't using a PCI debug port!
    //
    if (PciDebugPortsCount == 0) {
        return FALSE;
    }

    //
    // If its a bridge check if one of its subordinate buses has the debugger
    // port on it
    //

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        FOR_ALL_IN_ARRAY(PciDebugPorts, PciDebugPortsCount, current) {

            if ((Device->BusNumbersConfigured != FALSE) &&
                (SEGMENT_NUMBER_FOR_BUS(current->Bus) ==
                 SEGMENT_NUMBER_FOR_BUS(Device->BusNumber)) &&
                (LOCAL_BUS_NUMBER(current->Bus) >=
                 ConfigData->SaveRestore.bridge.SecondaryBusNumber) &&
                (LOCAL_BUS_NUMBER(current->Bus) <=
                 ConfigData->SaveRestore.bridge.SubordinateBusNumber) &&
                (ConfigData->SaveRestore.bridge.SecondaryBusNumber != 0) &&
                (ConfigData->SaveRestore.bridge.SubordinateBusNumber != 0)) {

                return TRUE;
            }
        }

    } else {

        if (Device->Parent->PciBridge) {

            //
            // Get the BIOS config of the parent so we can get its initial bus
            // number
            //
            status = PciGetSavedBridgeConfiguration(Device->Parent->PciBridge,
                                                    &parentConfig
                                                    );
            if (!NT_SUCCESS(status)) {
                PCI_ASSERTMSG("Shouldn't be able to hit this because adding the device would have failed", FALSE);
                return FALSE;
            }

            if ((parentConfig.SaveRestore.bridge.SecondaryBusNumber == 0) ||
                (parentConfig.SaveRestore.bridge.SubordinateBusNumber == 0)) {

                //
                // This is a bridge that wasn't configured by the firmware so this
                // child can't be on the debug path.
                //
                return FALSE;

            } else {

                segmentNumber =
                    SEGMENT_NUMBER_FOR_BUS(Device->Parent->SecondaryBusNumber);
                parentBusNumber =
                    FULL_BUS_NUMBER(
                        segmentNumber,
                        parentConfig.SaveRestore.bridge.SecondaryBusNumber);

            }

        } else {

            parentBusNumber = Device->Parent->SecondaryBusNumber;
        }

        //
        // Check if we are the device on the correct bus in the correct slot
        //
        FOR_ALL_IN_ARRAY(PciDebugPorts, PciDebugPortsCount, current) {

            if ((current->Bus == parentBusNumber) &&
                (current->Slot.u.AsULONG == Device->Slot.u.AsULONG)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOLEAN
PciIsSameDevice(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData
    )
/*++

Routine Description:

    This routine determines whether a device is the same device
    as that from which the given config header was read.  It
    checks all of the read-only identification registers in a
    device for consistency.

Arguments:

    Device - the device extension to check.

    IdData - a structure containing the values to check against.

Return Value:

    TRUE if the two match.
    FALSE otherwise.

--*/
{
    NON_PAGED_CODE();

    if ((IdData->VendorID != Device->VendorID) ||
        (IdData->DeviceID != Device->DeviceID)) {

        return FALSE;
    }

    if ((!(Device->HackFlags & PCI_HACK_NO_REVISION_AFTER_D3)) &&
        (IdData->RevisionID != Device->RevisionID)) {

        return FALSE;
    }

    //
    // If the device has a subsystem ID make sure that's the same too.
    //

    if ((!(Device->HackFlags & PCI_HACK_NO_SUBSYSTEM)) &&
        (!(Device->HackFlags & PCI_HACK_NO_SUBSYSTEM_AFTER_D3))) {

        if ((IdData->SubVendorID != Device->SubVendorID) ||
            (IdData->SubSystemID != Device->SubSystemID)) {

            //
            // If the device was in D3cold and it lost its SSIDs,
            // remember that and move on.
            //

            if ((Device->LogicalPowerState == PowerDeviceD3) &&
                (Device->D3ColdSupport == D3ColdSupportEnabled)) {

                InterlockedOr64(&Device->HackFlags, PCI_HACK_NO_SUBSYSTEM_AFTER_D3);

            } else {
                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOLEAN
PciIsSameResource(
    __in PPCI_DEVICE_RESOURCES Resources,
    __in PPCI_DEVICE_RESOURCES SavedResources
    )

/*++

Routine Description:

    This routine compares two sets of resources and returns whether
    they are equal.

    N.B. This routine is designed for Type 0 devices only.  It is designed
         for comparing resources of debugging devices.

    N.B. This routine is called very early in boot therefore errors are not
         asserted.

Arguments:

    Resources - Supplies pointer to first set of resources being compared.

    SavedResources - Supplies pointer to second set of resources
                     being compared.

Return Value:

    TRUE if resources match.

--*/

{

    ULONG BarIndex;

    //
    // Compare BARs
    //

    for (BarIndex = 0; BarIndex < PCI_TYPE0_BAR_COUNT; BarIndex += 1) {
        if (Resources->Bars[BarIndex].Type !=
            SavedResources->Bars[BarIndex].Type) {

            return FALSE;
        }

        switch (Resources->Bars[BarIndex].Type) {
        case CmResourceTypeMemoryLarge:
            if (Resources->Bars[BarIndex].u.Memory.Start.HighPart !=
                SavedResources->Bars[BarIndex].u.Memory.Start.HighPart) {

                return FALSE;
            }

            //
            // Fall through.
            //

        case CmResourceTypeMemory:
            if ((Resources->Bars[BarIndex].u.Generic.Length !=
                 SavedResources->Bars[BarIndex].u.Generic.Length) ||
                (Resources->Bars[BarIndex].u.Generic.Start.LowPart !=
                 SavedResources->Bars[BarIndex].u.Generic.Start.LowPart)) {

                return FALSE;
            }

            break;

        case CmResourceTypePort:
            if ((Resources->Bars[BarIndex].u.Generic.Length !=
                 SavedResources->Bars[BarIndex].u.Generic.Length) ||
                (Resources->Bars[BarIndex].u.Generic.Start.LowPart !=
                 SavedResources->Bars[BarIndex].u.Generic.Start.LowPart)) {

                return FALSE;
            }

            break;

        case CmResourceTypeNull:
            break;

        default:

            //
            // This case should never hit as all types which can be set in
            // PciResourceFromBar are accounted for above.
            //

            break;
        }
    }

    //
    // Compare the ROM BAR.
    //

    if ((Resources->type0.RomBar.Type != SavedResources->type0.RomBar.Type) ||
        (Resources->type0.RomBar.u.Generic.Length !=
         SavedResources->type0.RomBar.u.Generic.Length) ||
        (Resources->type0.RomBar.u.Generic.Start.QuadPart !=
         SavedResources->type0.RomBar.u.Generic.Start.QuadPart)) {

        return FALSE;
    }

    return TRUE;
}

VOID
PciGetSaveRestoreData(
    __in PPCI_DEVICE Device,
    __out PPCI_SAVE_RESTORE_DATA HardwareRegisters
    )
/*++

Routine Description:

    This routine reads the various registers we need to save and restore
    that are not resource related out of the hardware and places them
    into the HardwareRegisters buffer.

Arguments:

    Device - The device to read the data from.

    HardwareRegisters - The buffer to copy the data into.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();

    RtlZeroMemory(HardwareRegisters, sizeof(PCI_SAVE_RESTORE_DATA));

    PciReadConfigRegister(Device, Command, &HardwareRegisters->Command);
    PciReadConfigRegister(Device, CacheLineSize, &HardwareRegisters->CacheLineSize);
    PciReadConfigRegister(Device, LatencyTimer, &HardwareRegisters->LatencyTimer);

    PciReadConfigRegister(Device,
                          common.InterruptLine,
                          &HardwareRegisters->InterruptLine
                          );

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        PciReadConfigRegister(Device,
                              bridgeCommon.SecondaryBus,
                              &HardwareRegisters->bridge.SecondaryBusNumber
                              );
        PciReadConfigRegister(Device,
                              bridgeCommon.SubordinateBus,
                              &HardwareRegisters->bridge.SubordinateBusNumber
                              );
        PciReadConfigRegister(Device,
                              bridgeCommon.BridgeControl,
                              &HardwareRegisters->bridge.BridgeControl
                              );
    }
}

VOID
PciUpdateSaveRestoreData(
    __in PPCI_DEVICE Device,
    __out PPCI_SAVE_RESTORE_DATA HardwareRegisters
    )
/*++

Routine Description:

    This routine reads the various registers we need to save and restore that
    are not resource related out of the hardware and places them into the
    HardwareRegisters buffer.

    N.B. This routine should not be invoked for initial population of
         Device->SavedRegisters.  This routine assumes it is safe to read
         values from Device->SavedRegisters.

Arguments:

    Device - The device to read the data from.

    HardwareRegisters - The buffer to copy the data into.

Return Value:

    VOID

--*/
{

    PCI_DEVICE_ID_DATA idData;
    USHORT isaBit;
    PCI_SAVE_RESTORE_DATA tempSaveRestoreData;

    NON_PAGED_CODE();

    //
    // A bridge gets an ISA bit if one of its peers is a VGA bridge that does
    // 10-bit decoding or the parent bridge has an ISA bit set.  This is so
    // that no more than one entity claims any transaction on the bus and is
    // also used by the arbiter for assigning resources.
    //
    // There are certain legacy bridges that have read-only ISA bit in the
    // bridge control register.  The driver, on failure to set the ISA bit
    // (when required), will fail the start irp for this bridge.  Since these
    // bridges worked in the past, even with conflicts, a new hack flag
    // (PCI_HACK_IGNORE_NON_STICKY_ISA) is defined that ignores the results of
    // setting an ISA bit on such bridges.
    //
    // In addition, this hack flag ensures that the ISA bit value in the
    // SavedRegisters.bridge.BridgeControl field is retained across all
    // updates with values read from the device.  The ISA bit is non-sticky
    // and we don't want to lose the value assigned in PciProcessBus with
    // updates from the device.  This is necessary to ensure that arbitration
    // occurs properly and resources are assigned correctly after rebalance or
    // removal.
    //
    // Identical behavior is appropriate for bridges that malfunction when the
    // ISA bit is set.  The bit may be "logically" set for these bridges for
    // arbitration purposes, however this setting is never reflected into
    // hardware.
    //

    PciGetSaveRestoreData(Device, &tempSaveRestoreData);
    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        if (((Device->HackFlags & PCI_HACK_IGNORE_NON_STICKY_ISA) != 0) ||
            ((Device->HackFlags & PCI_HACK_AVOID_HARDWARE_ISA_BIT) != 0)) {

            isaBit = Device->SavedRegisters.bridge.BridgeControl &
                        PCI_ENABLE_BRIDGE_ISA;

            if (isaBit) {
                tempSaveRestoreData.bridge.BridgeControl |= isaBit;
            }
        }
    }

    //
    // There exists a race condition where a device is hot unplugged
    // concurrently with an S3/S4 transition. In this case, it's
    // possible to read all registers as 0xffs (since the device is
    // gone). When the device is re-added before resume, those 0xffs
    // would be restored to the device. Only update the saved registers
    // if the same device is in place.
    //

    if (NT_SUCCESS(PciReadDeviceIdData(Device, &idData)) &&
        PciIsSameDevice(Device, &idData)) {

        RtlCopyMemory(HardwareRegisters,
                      &tempSaveRestoreData,
                      sizeof(PCI_SAVE_RESTORE_DATA));

        InterlockedIncrement(&PciDebugSaveDone);

    } else {
        InterlockedIncrement(&PciDebugSaveNotDone);
        TraceLoggingWrite(PciTraceLoggingProvider,
                          "PciDebugSave",
                          TraceLoggingInt32(PciDebugSaveNotDone, "NotDone"),
                          TraceLoggingInt32(PciDebugSaveDone, "Done"),
                          TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));
    }

    return;
}

NTSTATUS
PciWalkDevicePath(
    _In_ PPCI_DEVICE Device,
    _In_ PPCI_WALK_DEVICE_PATH_CALLBACK Callback,
    _Inout_ PVOID Context,
    _Out_opt_ PPCI_BUS *Root
    )

/*++

Routine Description:

    This routine walks the hierarchical path, applying the callback on each
    routing elements in the path.

Arguments:

    Device - Supplies the device pointer as the start point.

    Callback - Supplies the callback function applied to each node.

    Context - Supplies the context to the callback function.

    Root - Optionally stores the root of the path.

Return Value:

    NT Status value.

--*/

{

    NTSTATUS CallbackStatus;
    PPCI_BUS Parent;

    if (Device == NULL || Callback == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Root != NULL) {
        *Root = NULL;
    }

    Parent = Device->Parent;

    //
    // Walk upstream in the hierarchy until we reach the root.
    //

    while ((PCI_ROOT_BUS(Parent)) == FALSE) {
        CallbackStatus = (*Callback)(Parent, Context);
        if (!NT_SUCCESS(CallbackStatus)) {
            return CallbackStatus;
        }

        if (Root != NULL && PCI_ROOT_BUS(Parent->ParentBus)) {
            *Root = Parent;
        }

        Parent = Parent->ParentBus;
    }

    return STATUS_SUCCESS;
}
