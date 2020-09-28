/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    expressbridge.c

Abstract:

    This module contains functions associated with configuring Express
    bridges also referred to as a Virtual PCI-PCI bridge in the PCI
    Express Base Specification.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "ExpressBridge.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
ExpressProcessNewRootPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressProcessNewUpstreamSwitchPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressProcessNewDownstreamSwitchPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortUpdateMaxPayloadSize (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in USHORT NewMaxPayloadSize
    );

VOID
ExpressRootPortHandlePmeEventOnPowerUp (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __out PBOOLEAN PmePending
    );

NTSTATUS
ExpressBridgeProcessNewHotPlugController (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PEXPRESS_LINK ExpressLink,
    __out PBOOLEAN HotPlugCapable,
    __out PPCI_HOTPLUG_CONTROLLER *HotPlugController
    );

NTSTATUS
ExpressBridgeSetInitialSlotState (
    __in PEXPRESS_BRIDGE ExpressBridge,
    __inout PPCI_HOTPLUG_SLOT Slot
    );

BOOLEAN
ExpressBridgeGetDevicePresenceState (
    __in PPCI_HOTPLUG_SLOT Slot
    );

BOOLEAN
ExpressBridgeGetLatchState (
    __in PPCI_HOTPLUG_SLOT Slot
    );

BOOLEAN
ExpressBridgeGetLinkState (
    __in PPCI_HOTPLUG_SLOT Slot
    );

BOOLEAN
ExpressBridgeExecuteHotPlugCommands (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_COMMAND_TYPE Type,
    __in ULONG_PTR Context
    );

IO_WORKITEM_ROUTINE ExpressBridgeExecuteHotPlugCommandsWorker;

KDEFERRED_ROUTINE ExpressBridgeExecuteHotPlugCommandsTimer;

BOOLEAN
ExpressBridgeEnableDisableControllerInterrupts (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN Enable
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, ExpressBridgeDisconnectInterrupt)
    #pragma alloc_text(PAGE, ExpressBridgeProcessMaxPayloadSize)
    #pragma alloc_text(PAGE, ExpressBridgeProcessNewHotPlugController)
    #pragma alloc_text(PAGE, ExpressBridgeStart)
    #pragma alloc_text(PAGE, ExpressBridgeSetInitialSlotState)
    #pragma alloc_text(PAGE, ExpressBridgeStop)
    #pragma alloc_text(PAGE, ExpressProcessNewDownstreamSwitchPortBridge)
    #pragma alloc_text(PAGE, ExpressProcessNewRootPortBridge)
    #pragma alloc_text(PAGE, ExpressProcessNewUpstreamSwitchPortBridge)
    #pragma alloc_text(PAGE, ExpressProcessNewVirtualBridge)
    #pragma alloc_text(PAGE, ExpressRootPortUpdateMaxPayloadSize)
    #pragma alloc_text(PAGE, ExpressBridgeSetAriEnable)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
ExpressProcessNewVirtualBridge (
    _In_ PPCI_BRIDGE Bridge,
    _In_ PPCI_BRIDGE_INTERFACE Interface,
    _Out_ PEXPRESS_BRIDGE *NewExpressBridge
    )

/*++

Routine Description:

    This routine initializes the structures associated with a Virtual
    PCI-PCI bridge, also referred to as an ExpressBridge in this driver.

    The Virtual bridge can represent either Root Ports or Switch Ports.

Arguments:

    Bridge - Supplies the Pci Bridge whose Express capabilities will
        be processed.

    BridgeInterface - The structure returned by the PDO describing this bridge.
        This will not be valid after the function returns.

    NewExpressBridge - Supplies a placeholder for a pointer to EXPRESS_BRIDGE.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES - on memory allocation failure.

    STATUS_DEVICE_DATA_ERROR - for an unknown bridge type.

    Otherwise an appropriate status as returned from a function call.

--*/

{

    BOOLEAN DeleteExpressBridge;
    PEXPRESS_BRIDGE ExpressBridge;
    NTSTATUS Status;
    PIO_WORKITEM WorkItem;
    PCI_EXPRESS_DEVICE_CAPABILITIES_2_REGISTER DeviceCapabilities2;
    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2;

    PAGED_CODE();

    *NewExpressBridge = NULL;
    DeleteExpressBridge = FALSE;

    PCI_ASSERT(Bridge->ExpressCapability != 0);

    //
    // Allocate a structure to hold the express-specific data.
    //

    ExpressBridge = PciAllocatePool(NonPagedPoolNx, sizeof(EXPRESS_BRIDGE));
    if (ExpressBridge == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    WorkItem = PciAllocatePool(NonPagedPoolNx, IoSizeofWorkItem());
    if (WorkItem == NULL) {
        PciFreePool(ExpressBridge);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(ExpressBridge, sizeof(EXPRESS_BRIDGE));
    RtlZeroMemory(WorkItem, IoSizeofWorkItem());
    ExpressBridge->PciBridge = Bridge;
    ExpressBridge->HotPlugWorkItem = WorkItem;
    *NewExpressBridge = ExpressBridge;

    //
    // First get supporting information about this bridge that will
    // aid in initialization.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 ExpressCapabilities,
                                 &ExpressBridge->ExpressCapabilities);

    ExpressBridge->BridgeType = ExpressBridge->ExpressCapabilities.DeviceType;
    ExpressBridge->AerCapability = Interface->AerCapability;
    ExpressBridge->AcsCapability = Interface->AcsCapability;
    ExpressBridge->AcsNotNeeded = Interface->AcsNotNeeded;
    ExpressBridge->AtsCapability = Interface->AtsCapability;
    if (ExpressBridge->ExpressCapabilities.SlotImplemented != FALSE) {
        ExpressBridge->PhysicalSlotNumber = Interface->PhysicalSlotNumber;
    } else {
        ExpressBridge->PhysicalSlotNumber = 0;
    }

    RtlCopyMemory(&ExpressBridge->ErrorSource,
                  &Interface->ErrorSource,
                  sizeof(WHEA_ERROR_SOURCE_DESCRIPTOR));

    KeInitializeDpc(&ExpressBridge->HotPlugCommandDpc,
                    ExpressBridgeExecuteHotPlugCommandsTimer,
                    Bridge->Bus);

    KeInitializeTimer(&ExpressBridge->HotPlugCommandTimer);
    IoInitializeWorkItem(ExpressBridge->PciBridge->Bus->DeviceObject,
                         ExpressBridge->HotPlugWorkItem);

    ExpressBridge->InterfaceDereference = Interface->InterfaceDereference;
    ExpressBridge->GetPowerParameters = Interface->GetPowerParameters;
    ExpressBridge->AriForwardingEnable = Interface->AriForwardingEnable;
    ExpressBridge->ObffControl = Interface->ObffControl;
    ExpressBridge->ThunderboltCapability = Interface->ThunderboltCapability;

    if (Interface->InterfaceReference != NULL) {
        Interface->InterfaceReference(Interface->Context);
    }

    if (ExpressBridge->AriForwardingEnable != NULL) {

        PciReadExpressBridgeRegister(ExpressBridge,
                                     DeviceCapabilities2,
                                     &DeviceCapabilities2);

        ExpressBridge->AriForwardingSupported =
            (BOOLEAN) DeviceCapabilities2.AriForwardingSupported;

        PciReadExpressBridgeRegister(ExpressBridge,
                                     DeviceControl2,
                                     &DeviceControl2);

        ExpressBridge->AriForwardingEnabled =
            ExpressBridge->AriForwardingSupported &&
            DeviceControl2.AriForwardingEnable;
    }

    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:
        Status = ExpressProcessNewRootPortBridge(ExpressBridge);
        break;

    case PciExpressUpstreamSwitchPort:
        Status = ExpressProcessNewUpstreamSwitchPortBridge(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:
        Status = ExpressProcessNewDownstreamSwitchPortBridge(ExpressBridge);
        break;

    case PciExpressToPciXBridge:
    case PciXToExpressBridge:

        //
        // Such bridge are treated as conventional PCI-PCI bridge.
        //

        Status = STATUS_SUCCESS;
        DeleteExpressBridge = TRUE;
        break;

    default:
        Status = STATUS_DEVICE_DATA_ERROR;
        break;
    }

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    Status = STATUS_SUCCESS;

Exit:

    if (!NT_SUCCESS(Status) || (DeleteExpressBridge != FALSE)) {
        PciFreePool(ExpressBridge);
        *NewExpressBridge = NULL;
    }

    return Status;
}

VOID
PciExpressErrorHandler (
    VOID
    )
{

    return;
}

NTSTATUS
ExpressProcessNewRootPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine initializes an EXPRESS_BRIDGE structure for a Pci Express
    Root Port.

Arguments:

    ExpressBridge - Supplies a pointer to an Express bridge representing a
        Pci Express Root Port.

Return Value:

    STATUS_SUCCESS on success.

--*/

{

    PPCI_BUS Bus;
    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlRegister;
    PEXPRESS_LINK ExpressLink;
    PROOT_COMPLEX RootComplex;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // For a Root Port, read the DeviceControl, RootControl and LinkControl
    // registers. The DeviceControl register is in the realms of the PDO.
    // Other registers will be saved and restored across PnP and Power events.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 DeviceControl,
                                 &DeviceControlRegister);

    ExpressBridge->Downstream.u.RootPort.MaxPayloadSize =
        DeviceControlRegister.MaxPayloadSize;

    PciReadExpressBridgeRegister(ExpressBridge,
                                 RootControl,
                                 &ExpressBridge->Downstream.u.RootPort.RootControl);

    ExpressLink = &ExpressBridge->Downstream.ExpressLink;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkControl,
                                 &ExpressLink->LinkControl);

    ExpressBridge->Downstream.u.RootPort.ReadCompletionBoundary =
        ExpressLink->LinkControl.ReadCompletionBoundary;

    //
    // In addition, cache the link capabilities and link status registers.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkCapabilities,
                                 &ExpressLink->LinkCapabilities);

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkStatus,
                                 &ExpressLink->LinkStatus);

    //
    // Cache the AER capabilities.
    //

    if (ExpressBridge->AerCapability != 0 &&
        PciIsAerGranted(ExpressBridge->PciBridge->Bus->Root)) {

        if ((ExpressBridge->ErrorSource.Flags &
          WHEA_ERROR_SOURCE_FLAG_FIRMWAREFIRST) == 0) {

            //
            // Clear the root error status register.
            //

            ExpressBridge->Downstream.u.RootPort.RootErrorStatus.AsULONG = 0x7fffffff;
            PciWriteExpressRootPortAerRegister(
                ExpressBridge,
                RootErrorStatus,
                &ExpressBridge->Downstream.u.RootPort.RootErrorStatus.AsULONG);

            PciReadExpressRootPortAerRegister(
                ExpressBridge,
                RootErrorStatus,
                &ExpressBridge->Downstream.u.RootPort.RootErrorStatus.AsULONG);

            //
            // Cache the root error command register.
            //

            PciReadExpressRootPortAerRegister(
                ExpressBridge,
                RootErrorCommand,
                &ExpressBridge->Downstream.u.RootPort.RootErrorCommand.AsULONG);
        }

        //
        // Add our error source to those WHEA manages. We do this even if
        // the error source is firmware-first so we can extract error data from
        // the NMI handler in a firmware-first uncorrectable error scenario.
        //

#if !defined(_IA64_)
        HalSetPciErrorHandlerCallback(PciExpressErrorHandler);
#endif
        WheaAddErrorSource(&ExpressBridge->ErrorSource, ExpressBridge);
    }

    //
    // Check for the presence of a slot on a root port and whether this slot
    // is capable of any hot plug operations. In that case initialize a hot
    // plug controller.
    //

    Status = ExpressBridgeProcessNewHotPlugController(
                ExpressBridge,
                ExpressLink,
                &ExpressBridge->Downstream.HotPlugCapable,
                &ExpressBridge->Downstream.HotPlugController);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Initialize a root port PME list entry field.
    //

    InitializeListHead(&ExpressBridge->Downstream.u.RootPort.PmeListEntry);

    //
    // If the slot is hot plug capable, then update the InterruptsEnabled field
    // to represent that the hot plug interrupts are enabled - say by the BIOS,
    // so that these interrupts will be disabled below before any processing of
    // Start Irp.
    //

    if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {
        ExpressBridge->Downstream.InterruptsEnabled |=
            INTERRUPT_SOURCE_EXPRESS_HOTPLUG;

    }

    //
    // Locate the Root Complex for this hierarchy.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    RootComplex = Bus->Root->RootComplex;

    PCI_ASSERT(RootComplex != NULL);

    ExpressBridge->RootComplex = RootComplex;

    //
    // Add the Root Port bridge to the Root Complex.
    //

    PciAcquirePassiveLock(&RootComplex->ListLock);
    ExpressBridge->Sibling = RootComplex->RootPortList;
    RootComplex->RootPortList = ExpressBridge;
    PciReleasePassiveLock(&RootComplex->ListLock);

    //
    // Initialize an interrupt state lock.
    //

    KeInitializeSpinLock(&ExpressBridge->Downstream.InterruptStateLock);

    //
    // Disable interrupts until we get a chance to register an ISR.
    //

    ExpressRootPortDisableInterrupts(ExpressBridge);

    //
    // Initialize the DPC for an ISR. Take a reference on the device object to
    // ensure safe removal.
    //

    ObReferenceObject(Bus->DeviceObject);
    IoInitializeDpcRequest(Bus->DeviceObject, ExpressRootPortInterruptDpc);

    //
    // Initialize and configure express link for ASPM.
    //

    Status = ExpressBridgeInitializeLinkPowerManagement(ExpressBridge,
                                                        ExpressLink);

    return Status;
}

NTSTATUS
ExpressProcessNewUpstreamSwitchPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine initializes the EXPRESS_BRIDGE structure for an upstream
    switch port bridge.

Arguments:

    ExpressBridge - Supplies a pointer to an Express bridge representing an
        upstream switch port.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES - on memory allocation failure.

    STATUS_SUCCESS - otherwise.

--*/

{

    PEXPRESS_BRIDGE ParentExpressBridge;
    PSWITCH_COMPLEX SwitchComplex;

    PAGED_CODE();

    //
    // Allocate a structure that represents a switch complex. A switch
    // complex has a single upstream port and multiple downstream ports
    // where each port represents an Express bridge, also known as a
    // virtual PCI-PCI bridge.
    //

    SwitchComplex = PciAllocatePool(NonPagedPoolNx, sizeof(SWITCH_COMPLEX));
    if (SwitchComplex == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(SwitchComplex, sizeof(SWITCH_COMPLEX));

    //
    // Now establish the lineage for an upstream switch port
    // in the Express hierarchy.
    //

    SwitchComplex->UpstreamPort = ExpressBridge;
    ExpressBridge->UpstreamSwitchPort.SwitchComplex = SwitchComplex;

    ParentExpressBridge =
        ExpressBridge->PciBridge->Bus->ParentBus->PciBridge->ExpressBridge;

    ExpressBridge->Parent = ParentExpressBridge;
    ExpressBridge->RootComplex = ParentExpressBridge->RootComplex;

    //
    // The parent bridge represents a root port or a switch in which
    // case it is a downstream switch port.
    //

    if (ParentExpressBridge->BridgeType == PciExpressRootPort) {
        ExpressBridge->UpstreamSwitchPort.RootPortBridge = ParentExpressBridge;

    } else {

        PCI_ASSERT(ParentExpressBridge->BridgeType ==
                   PciExpressDownstreamSwitchPort);

        ExpressBridge->UpstreamSwitchPort.RootPortBridge =
            ParentExpressBridge->Downstream.u.SwitchPort.RootPortBridge;

    }

    //
    // Update information on whether this switch complex is an integrated
    // component or affixed to a slot.
    //

    PciAcquirePassiveLock(&PciGlobalLock);
    if (ParentExpressBridge->ExpressCapabilities.SlotImplemented == 0) {
        SwitchComplex->IntegratedComponent = TRUE;
    }

    PciReleasePassiveLock(&PciGlobalLock);

    //
    // Track the latencies for links.
    //

    ExpressSwitchUpdateLinkLatency(SwitchComplex);

    //
    // The Device Control and Link Control registers for an upstream switch
    // port are in the realms of the PDO.
    //

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressProcessNewDownstreamSwitchPortBridge (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine initializes the EXPRESS_BRIDGE structure for a downstream
    switch port.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge representing
        a downstream switch port.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_BUS Bus;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_BRIDGE ParentExpressBridge;
    NTSTATUS Status;
    PSWITCH_COMPLEX SwitchComplex;

    PAGED_CODE();

    //
    // For a downstream switch port, the Device Control register is in the
    // realms of the PDO. The link control, link capabilities and link status
    // registers will be saved and restored across Pnp and Power events.
    //

    ExpressLink = &ExpressBridge->Downstream.ExpressLink;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkControl,
                                 &ExpressLink->LinkControl);

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkCapabilities,
                                 &ExpressLink->LinkCapabilities);

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkStatus,
                                 &ExpressLink->LinkStatus);

    //
    // Check for the presence of a slot on a downstream switch port and whether
    // this slot is capable of any hot plug operations. In that case initialize
    // a hot plug controller.
    //

    Status = ExpressBridgeProcessNewHotPlugController(
                ExpressBridge,
                ExpressLink,
                &ExpressBridge->Downstream.HotPlugCapable,
                &ExpressBridge->Downstream.HotPlugController);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // If the slot is hot plug capable then register the downstream switch port
    // for system power change notifications and update the InterruptsEnabled
    // field to indicate that the hot plug interrupts were enabled - say by the
    // BIOS so that these interrupts will be disabled below before any Start
    // Irp processing.
    //

    //
    // Initialize an interrupt state lock.
    //
    // N.B. This lock can be acquired even when the slot on a downstream switch
    //      port is not hot plug capable.
    //

    KeInitializeSpinLock(
        &ExpressBridge->Downstream.InterruptStateLock);

    if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {

        ExpressBridge->Downstream.InterruptsEnabled |=
            INTERRUPT_SOURCE_EXPRESS_HOTPLUG;

        //
        // Disable interrupts until we get a chance to register an ISR.
        //

        ExpressDownstreamSwitchPortDisableInterrupts(ExpressBridge);

        //
        // Initialize the DPC for an ISR. Take a reference on the device object
        // to ensure safe removal.
        //

        Bus = ExpressBridge->PciBridge->Bus;
        ObReferenceObject(Bus->DeviceObject);
        IoInitializeDpcRequest(Bus->DeviceObject,
                               ExpressDownstreamSwitchPortInterruptDpc);

    }

    //
    // Locate the parent bridge for the downstream switch port bridge.
    //

    ParentExpressBridge =
        ExpressBridge->PciBridge->Bus->ParentBus->PciBridge->ExpressBridge;

    ExpressBridge->Parent = ParentExpressBridge;

    //
    // Locate the Root Complex and the Root Port bridge information for the
    // downstream switch port.
    //

    ExpressBridge->RootComplex = ParentExpressBridge->RootComplex;
    ExpressBridge->Downstream.u.SwitchPort.RootPortBridge =
        ParentExpressBridge->UpstreamSwitchPort.RootPortBridge;

    //
    // Now locate the Switch Complex information and link the
    // upstream switch port bridge to the switch complex.
    //

    SwitchComplex = ParentExpressBridge->UpstreamSwitchPort.SwitchComplex;
    ExpressBridge->Downstream.u.SwitchPort.SwitchComplex = SwitchComplex;

    PciAcquirePassiveLock(&PciGlobalLock);
    ExpressBridge->Sibling = SwitchComplex->DownstreamPortList;
    SwitchComplex->DownstreamPortList = ExpressBridge;
    PciReleasePassiveLock(&PciGlobalLock);

    //
    // Initialize and configure express link for ASPM. This configuration is
    // different depending on whether the switch complex is chipset integrated
    // or an add in card.
    //

    Status = ExpressBridgeInitializeLinkPowerManagement(ExpressBridge,
                                                        ExpressLink);

    return Status;
}

NTSTATUS
ExpressBridgeStart (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PCM_RESOURCE_LIST RawList,
    __in PCM_RESOURCE_LIST TranslatedList
    )

/*++

Routine Description:

    This routine restores registers for an Express bridge during Start.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

    RawList - Supplies a list of resources assigned to the bridge.

    TranslatedList - Supplies a list of resources assigned to the bridge in a
        translated form.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlRegister;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:

        //
        // Restore the root control and link control registers for the root
        // port.
        //

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      RootControl,
                                      &ExpressBridge->Downstream.u.RootPort.RootControl);

        PciWriteExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // Restore the max payload size.
        //

        PciReadExpressBridgeRegister(ExpressBridge,
                                     DeviceControl,
                                     &DeviceControlRegister);

        DeviceControlRegister.MaxPayloadSize =
            ExpressBridge->Downstream.u.RootPort.MaxPayloadSize;

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      DeviceControl,
                                      &DeviceControlRegister);

        //
        // Express root ports are supplied with interrupt resources for PME
        // and hot plug events. So register an ISR for specific message/line
        // based interrupt resource, initialize a DPC request to queue from
        // an ISR and enable appropriate interrupts for the root port.
        //

        Status = ExpressRootPortConnectInterrupt(ExpressBridge,
                                                 RawList,
                                                 TranslatedList);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PCI_ASSERT(ExpressBridge->Downstream.InterruptConnected != FALSE);

        ExpressRootPortEnableInterrupts(ExpressBridge);

        //
        // Once the interrupts are enabled, start the hot plug controller for
        // a root port.
        //

        if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {
            Status = PciStartHotPlugController(
                        ExpressBridge->Downstream.HotPlugController);

        }

        break;

    case PciExpressDownstreamSwitchPort:

        PciWriteExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // If a downstream switch port is hot plug capable then it should be
        // provided with interrupt resources for handling hot plug events.
        // Register an ISR for the hot plug specific message or line based
        // interrupt and enable interrupts for hot plug events.
        //

        if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {
            Status = ExpressDownstreamSwitchPortConnectInterrupt(
                        ExpressBridge,
                        RawList,
                        TranslatedList);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            PCI_ASSERT(ExpressBridge->Downstream.InterruptConnected !=
                       FALSE);

            ExpressDownstreamSwitchPortEnableInterrupts(ExpressBridge);

            //
            // Once the interrupts are enabled, start the hot plug controller.
            //

            Status = PciStartHotPlugController(
                        ExpressBridge->Downstream.HotPlugController);

        }

        break;

    default:
        break;
    }

    return Status;
}

NTSTATUS
ExpressBridgeStop (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine saves registers for an Express bridge during Stop.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:

        //
        // Save the Root Control and Link Control registers for the root port.
        //

        PciReadExpressBridgeRegister(ExpressBridge,
                                     RootControl,
                                     &ExpressBridge->Downstream.u.RootPort.RootControl);

        PciReadExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // Disable interrupts and disconnect.
        //

        ExpressBridgeDisableInterrupts(ExpressBridge);
        ExpressRootPortDisconnectInterrupt(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:

        //
        // Save the Link Control register for a downstream switch port.
        //

        PciReadExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {

            //
            // Disable interrupts and disconnect.
            //

            ExpressBridgeDisableInterrupts(ExpressBridge);
            ExpressDownstreamSwitchPortDisconnectInterrupt(ExpressBridge);
        }

        break;

    default:
        break;
    }

    return Status;
}

NTSTATUS
ExpressBridgePowerUp (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine restores registers for an Express bridge during Power Up.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlRegister;
    BOOLEAN PmePending;
    NTSTATUS Status;

    NON_PAGED_CODE();

    Status = STATUS_SUCCESS;
    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:

        //
        // Restore the root control and link control registers for the root
        // port.
        //

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      RootControl,
                                      &ExpressBridge->Downstream.u.RootPort.RootControl);

        PciWriteExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // Restore the max payload size.
        //

        PciReadExpressBridgeRegister(ExpressBridge,
                                     DeviceControl,
                                     &DeviceControlRegister);

        DeviceControlRegister.MaxPayloadSize =
            ExpressBridge->Downstream.u.RootPort.MaxPayloadSize;

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      DeviceControl,
                                      &DeviceControlRegister);

        //
        // On power up, handle a single outstanding PME wake event before
        // enabling interrupts. The processing of PME wake event requires
        // executing at PASSIVE level in which case the interrupts will be
        // enabled after completion of PME handling. So bail out if there is
        // a PME wake event pending.
        //

        ExpressRootPortHandlePmeEventOnPowerUp(ExpressBridge, &PmePending);
        if (PmePending != FALSE) {
            break;
        }

        //
        // Now that we are powered up and there is no PME wake processing to
        // be done, enable interrupts on the root port.
        //

        ExpressRootPortEnableInterrupts(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:

        //
        // Restore the Link Control register and if the downstream switch port
        // is hot plug capable then enable interrupts on them.
        //

        PciWriteExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {
            ExpressDownstreamSwitchPortEnableInterrupts(ExpressBridge);
        }

        break;

    default:
        break;
    }

    return Status;
}

NTSTATUS
ExpressBridgePowerDown (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine save registers for an Express bridge during Power Down.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    NTSTATUS Status;

    NON_PAGED_CODE();

    Status = STATUS_SUCCESS;
    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:

        //
        // The root port is about to be powered down. Update this field to
        // prevent the system power change callback from taking any action.
        //

        ExpressBridge->Downstream.u.RootPort.EnableInterruptsOnS0Callback = FALSE;

        //
        // Save the Root Control and Link Control registers for the root port.
        //

        PciReadExpressBridgeRegister(ExpressBridge,
                                      RootControl,
                                      &ExpressBridge->Downstream.u.RootPort.RootControl);

        PciReadExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // The root port interrupts are disabled in the system power change
        // callback.
        //

        PCI_ASSERT((ExpressBridge->Downstream.InterruptsEnabled == 0) ||
                   (ExpressBridge->PciBridge->Bus->SIrp == NULL));

        //
        // Clear the PME status bit before powering down the root port in
        // order to identify the correct wake source on power up.
        //

        ExpressRootPortAckPmeEvent(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:

        PciReadExpressBridgeRegister(
            ExpressBridge,
            LinkControl,
            &ExpressBridge->Downstream.ExpressLink.LinkControl);

        //
        // The downstream switch port interrupts are disabled in the system
        // power change callback.
        //

        PCI_ASSERT(ExpressBridge->Downstream.InterruptsEnabled == 0);

        break;

    default:
        break;
    }

    return Status;
}

VOID
ExpressBridgeProcessMaxPayloadSize (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked to process the max payload value for all express
    ports directly connected to an express bridge. The PCI express base spec
    requires the max payload value to be the same for all devices in the
    express hierarchy.

    If a device does not support the payload value programmed in a root port,
    it cannot be started without lowering the payload value in the root port.
    On the other hand, if the payload value in the root port is set to a min
    value, all devices beneath the root port hierarchy will operate in a
    reduced performance mode.

    Ideally, an optimized payload value should be computed across the entire
    hierarchy. Since this a heavy weight operation similar to rebalancing,
    the current implementation is based on the following algorithm:

    For express devices that are not directly connected to a root port, the
    payload value from the root port is programmed in the device. If the device
    does not support this value, it will fail enumeration and show up code 10.
    There is no lowering of the payload value for a root port in an attempt to
    start such a device.

    There are two possibilities for an express device directly connected to a
    root port:

    If all functions are endpoints, this routine will find the highest payload
    value across all the functions and root port. This value is programmed in
    the root port and express ports.

    In all other cases, the payload value in the root port may be lowered to
    start all functions. The payload value in the root port is never increased.
    This is because there is no way to know in advance that increasing the
    payload value will not prevent some device in the hierarchy from starting.

    N.B. If any function in the device connected to a root port is started,
         then no adjustment will be made to the payload value of the root port.
         In such case, if a function does not support the value programmed in
         the root port, it will fail enumeration.

    N.B. This routine is not invoked for root ports or root complex integrated
         endpoints.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    None.

--*/

{

    BOOLEAN AllEndpoints;
    PPCI_BUS Bus;
    PPCI_DEVICE ChildDevice;
    PCI_EXPRESS_DEVICE_CAPABILITIES_REGISTER DeviceCapabilitiesRegister;
    USHORT DeviceMaxPayloadSize;
    PEXPRESS_PORT ExpressPort;
    USHORT OptimalPayloadSize;
    PEXPRESS_BRIDGE RootPortBridge;
    USHORT RootPortCurrentPayloadSize;
    USHORT RootPortMaxPayloadSize;

    PAGED_CODE();

    Bus = ExpressBridge->PciBridge->Bus;

    //
    // Process the max payload size based on the express bridge type.
    //

    if (ExpressBridge->BridgeType == PciExpressRootPort) {

        //
        // Save the max payload size supported by the root port.
        //

        PciReadExpressBridgeRegister(ExpressBridge,
                                     DeviceCapabilities,
                                     &DeviceCapabilitiesRegister);

        RootPortMaxPayloadSize =
            (USHORT)DeviceCapabilitiesRegister.MaxPayloadSizeSupported;

        //
        // In the first pass, determine if all functions on an express device
        // are endpoints as well as the optimal payload value.
        //
        // N.B. If any function is started, set the optimal value as the value
        //      currently programmed in the root port.
        //

        AllEndpoints = TRUE;
        OptimalPayloadSize = RootPortMaxPayloadSize;
        PciAcquirePassiveLock(&Bus->ChildDeviceLock);
        for (ChildDevice = Bus->ChildDevices;
             ChildDevice != NULL;
             ChildDevice = ChildDevice->Sibling) {

            if (ChildDevice->ExpressPort == NULL) {
                continue;
            }

            if (ChildDevice->ReportedMissing != FALSE) {
                continue;
            }

            if (ChildDevice->DeviceState != PciNotStarted) {
                OptimalPayloadSize = ExpressBridge->Downstream.u.RootPort.MaxPayloadSize;
                break;
            }

            ExpressPort = ChildDevice->ExpressPort;
            if (ExpressPort->DeviceType != PciExpressEndpoint) {
                AllEndpoints = FALSE;
            }

            DeviceMaxPayloadSize =
                (USHORT)ExpressPort->DeviceCapabilities.MaxPayloadSizeSupported;

            if (OptimalPayloadSize > DeviceMaxPayloadSize) {
                OptimalPayloadSize = DeviceMaxPayloadSize;
            }
        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);

        //
        // Now examine the payload value currently programmed in the root port.
        // Based on the device type, determine if the payload value in the
        // root port needs an adjustment.
        //

        RootPortCurrentPayloadSize = ExpressBridge->Downstream.u.RootPort.MaxPayloadSize;
        if (OptimalPayloadSize > RootPortCurrentPayloadSize) {
            if (AllEndpoints == FALSE) {
                OptimalPayloadSize = RootPortCurrentPayloadSize;

            } else {

                //
                // Raise the MPS value for a root port. This will allow the
                // express device to run in a high performance mode.
                //

                ExpressRootPortUpdateMaxPayloadSize(ExpressBridge,
                                                    OptimalPayloadSize);
            }

        }

        //
        // Lower the MPS value of a root port. This will allow the express
        // device to start.
        //

        if (OptimalPayloadSize < RootPortCurrentPayloadSize) {
            ExpressRootPortUpdateMaxPayloadSize(ExpressBridge,
                                                OptimalPayloadSize);
        }

    } else {
        if (ExpressBridge->BridgeType == PciExpressUpstreamSwitchPort) {
            RootPortBridge = ExpressBridge->UpstreamSwitchPort.RootPortBridge;

        } else {

            PCI_ASSERT(ExpressBridge->BridgeType ==
                       PciExpressDownstreamSwitchPort);

            RootPortBridge = ExpressBridge->Downstream.u.SwitchPort.RootPortBridge;
        }

        //
        // The payload size from the root port should be used.  If this port
        // doesn't support the root's payload size, fail.
        //

        RootPortCurrentPayloadSize = RootPortBridge->Downstream.u.RootPort.MaxPayloadSize;
        OptimalPayloadSize = RootPortCurrentPayloadSize;
    }

    //
    // Now set the optimized payload size for all functions that are connected
    // to the bridge.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (ChildDevice = Bus->ChildDevices;
         ChildDevice != NULL;
         ChildDevice = ChildDevice->Sibling) {

        if (ChildDevice->ExpressPort == NULL) {
            continue;
        }

        if (ChildDevice->ReportedMissing != FALSE) {
            continue;
        }

        if (ChildDevice->DeviceState != PciNotStarted) {
            continue;
        }

        ExpressPort = ChildDevice->ExpressPort;
        DeviceMaxPayloadSize =
            (USHORT)ExpressPort->DeviceCapabilities.MaxPayloadSizeSupported;

        if (DeviceMaxPayloadSize >= OptimalPayloadSize) {
            ExpressPort->DeviceControl.MaxPayloadSize = OptimalPayloadSize;

        } else {
            PciSetEnumFailureCode(ChildDevice,
                                  EXPRESS_INCOMPATIBLE_MAX_PAYLOAD_SIZE);

            ExpressDestroyPort(ExpressPort);
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
ExpressBridgeDisableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine disables interrupts on an express bridge or a virtual PCI-PCI
    bridge.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    None.

--*/

{

    NON_PAGED_CODE();

    switch(ExpressBridge->BridgeType) {
    case PciExpressRootPort:
        ExpressRootPortDisableInterrupts(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:
        ExpressDownstreamSwitchPortDisableInterrupts(ExpressBridge);
        break;

    default:
        break;
    }

    return;
}

VOID
ExpressBridgeDisconnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine disconnects interrupts for an express bridge or a virtual
    PCI-PCI bridge.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    switch(ExpressBridge->BridgeType) {
    case PciExpressRootPort:
        ExpressRootPortDisconnectInterrupt(ExpressBridge);
        break;

    case PciExpressDownstreamSwitchPort:
        ExpressDownstreamSwitchPortDisconnectInterrupt(ExpressBridge);
        break;

    default:
        break;
    }

    return;
}

VOID
ExpressDestroyVirtualBridge (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine unlinks the Virtual Bridge from the Express fabric and
    deallocates it. For an upstream switch port bridge, this routine will
    also destroy the Switch Complex linked to it.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

Return Value:

    None.

--*/

{

    PEXPRESS_BRIDGE Current;
    PEXPRESS_BRIDGE *Head;
    KIRQL OldIrql;
    PEXPRESS_BRIDGE Previous;
    PPCI_ROOT Root;
    PROOT_COMPLEX RootComplex;
    PSWITCH_COMPLEX SwitchComplex;

    //
    // Do an appropriate cleanup based on bridge type.
    //

    switch (ExpressBridge->BridgeType) {
    case PciExpressRootPort:

        PCI_ASSERT(ExpressBridge->Downstream.ExpressLink.TargetLink == NULL);

        if ((ExpressBridge->Downstream.HotPlugCapable != FALSE) &&
            (ExpressBridge->Downstream.HotPlugController != NULL)) {

            PciDestroyHotPlugController(
                &ExpressBridge->Downstream.HotPlugController);

        }

        //
        // Unlink this root port from a PME processing list maintained in the
        // root complex.
        //

        Root = ExpressBridge->PciBridge->Bus->Root;
        KeAcquireSpinLock(&Root->PmeListLock, &OldIrql);
        RemoveEntryList(&ExpressBridge->Downstream.u.RootPort.PmeListEntry);
        KeReleaseSpinLock(&Root->PmeListLock, OldIrql);

        //
        // Dereference the device object. This is to adjust the reference count
        // acquired for a DPC that corresponds to an ISR.
        //

        ObDereferenceObject(ExpressBridge->PciBridge->Bus->DeviceObject);

        //
        // Unregister the root port Aspm policy callback.
        //

        ExpressBridgeUnregisterAspmPolicyCallback(
            ExpressBridge,
            &ExpressBridge->Downstream.AspmPolicyHandle);

        //
        // Unlink the root port bridge from the root complex.
        //

        RootComplex = ExpressBridge->RootComplex;
        PciAcquirePassiveLock(&RootComplex->ListLock);
        Head = &RootComplex->RootPortList;

        PCI_ASSERT(*Head != NULL);

        if (*Head == ExpressBridge) {
            *Head = ExpressBridge->Sibling;

        } else {
            Current = (*Head)->Sibling;
            Previous = *Head;
            while (Current != NULL) {
                if (Current == ExpressBridge) {
                    Previous->Sibling = ExpressBridge->Sibling;
                    break;
                }

                Previous = Current;
                Current = Current->Sibling;
            }
        }

        PciReleasePassiveLock(&RootComplex->ListLock);
        break;

    case PciExpressUpstreamSwitchPort:

        //
        // The structure representing a switch complex is attached to the
        // upstream switch port bridge. If the upstream switch port bridge
        // is about to be deleted then delete the switch complex as well.
        //

        SwitchComplex = ExpressBridge->UpstreamSwitchPort.SwitchComplex;
        PciAcquirePassiveLock(&PciGlobalLock);

        PCI_ASSERT(SwitchComplex->DownstreamPortList == NULL);

        SwitchComplex->UpstreamPort = NULL;
        PciReleasePassiveLock(&PciGlobalLock);

        //
        // Now free the switch complex.
        //

        ExpressBridge->UpstreamSwitchPort.SwitchComplex = NULL;
        PciFreePool(SwitchComplex);
        break;

    case PciExpressDownstreamSwitchPort:

        PCI_ASSERT(
            ExpressBridge->Downstream.ExpressLink.TargetLink == NULL);

        if ((ExpressBridge->Downstream.HotPlugCapable != FALSE) &&
            (ExpressBridge->Downstream.HotPlugController != NULL)) {

            PciDestroyHotPlugController(
                    &ExpressBridge->Downstream.HotPlugController);

            //
            // Dereference the device object. This is to adjust the reference
            // count acquired for a DPC that corresponds to an ISR.
            //

            ObDereferenceObject(ExpressBridge->PciBridge->Bus->DeviceObject);
        }

        //
        // Unregister the downstream switch port Aspm policy callback.
        //

        ExpressBridgeUnregisterAspmPolicyCallback(
            ExpressBridge,
            &ExpressBridge->Downstream.AspmPolicyHandle);


        SwitchComplex = ExpressBridge->Downstream.u.SwitchPort.SwitchComplex;

        //
        // Unlink the downstream switch port bridge from the switch complex.
        //

        PciAcquirePassiveLock(&PciGlobalLock);
        Head = &SwitchComplex->DownstreamPortList;

        PCI_ASSERT(*Head != NULL);

        if (*Head == ExpressBridge) {
            *Head = ExpressBridge->Sibling;

        } else {
            Current = (*Head)->Sibling;
            Previous = *Head;
            while (Current != NULL) {
                if (Current == ExpressBridge) {
                    Previous->Sibling = ExpressBridge->Sibling;
                    break;
                }

                Previous = Current;
                Current = Current->Sibling;
            }
        }

        PciReleasePassiveLock(&PciGlobalLock);
        break;

    default:
        break;
    }

    //
    // Uninitialize and free the hotplug work item.
    //

    IoUninitializeWorkItem(ExpressBridge->HotPlugWorkItem);
    PciFreePool(ExpressBridge->HotPlugWorkItem);

    //
    // Drop the interface reference, if necessary.
    //

    if (ExpressBridge->InterfaceDereference != NULL) {
        ExpressBridge->InterfaceDereference(ExpressBridge->PciBridge->InterfaceContext);
    }

    //
    // Now delete the express bridge.
    //

    PciFreePool(ExpressBridge);
    return;
}

// ------------------------------------------------------------ Routines for PME

VOID
ExpressRootPortHandlePmeEventOnPowerUp (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __out PBOOLEAN PmePending
    )

/*++

Routine Description:

    This routine processes a single outstanding PME wake event on power up before
    completing the S0 Irp for the root port. Since some of the processing needs
    to be done at a PASSIVE level, necessary steps are taken to halt completion
    of S0.

    Once the PME wake event is handled, the S0 Irp completion is resumed and
    interrupts are enabled on the root port.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge for a root port.

    PmePending - Supplies a placeholder to indicate that there is a PME wake
        event pending.

Return Value:

    None.

--*/

{

    BOOLEAN EnqueuePmeEvent = FALSE;
    PCI_EXPRESS_ROOT_STATUS_REGISTER ExpressRootStatusRegister;
    KIRQL OldIrql;
    PPCI_ROOT Root;

    NON_PAGED_CODE();

    *PmePending = FALSE;

    //
    // Special S0 processing doesn't apply if this isn't a system state
    // transition.
    //

    if (ExpressBridge->PciBridge->Bus->SIrp == NULL) {
        return;
    }

    //
    // Read the Root Status register for PME status bit and Requestor Id. Bail
    // out if there are no indications of a PME wake event.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 RootStatus,
                                 &ExpressRootStatusRegister);

    if (ExpressRootStatusRegister.PMEStatus == 0) {
        return;
    }

    *PmePending = TRUE;
    ExpressBridge->Downstream.u.RootPort.PmeRequestorId.AsUSHORT =
        (USHORT)ExpressRootStatusRegister.PMERequestorId;

    //
    // Signal the PME thread for special case handling of a single PME wake
    // event on power up.
    //

    Root = ExpressBridge->PciBridge->Bus->Root;

    KeAcquireSpinLock(&Root->Bus->BusPowerLock, &OldIrql);
    if (!ExpressBridge->Downstream.u.RootPort.OnPmeList) {
        ExpressBridge->Downstream.u.RootPort.OnPmeList = TRUE;
        EnqueuePmeEvent = TRUE;
    }
    KeReleaseSpinLock(&Root->Bus->BusPowerLock, OldIrql);

    if (EnqueuePmeEvent) {
        ExpressBridge->Downstream.u.RootPort.PmeEventType = ExpressPmePowerUpEvent;
        ExInterlockedInsertTailList(&Root->PmeRootPortList,
                                    &ExpressBridge->Downstream.u.RootPort.PmeListEntry,
                                    &Root->PmeListLock);

        KeSetEvent(&Root->PmeEvent[PciRootPmeEventRootPortAttention],
                   IO_NO_INCREMENT,
                   FALSE);
    }
    return;
}

VOID
ExpressRootPortSingletonPmeEventWorker (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked to process a single PME wake event on resume from
    system sleep state. After acknowledging the PME event at the device and
    root port, it resumes the S0 Irp completion and enables interrupts on the
    root port.

Arguments:

    DeviceObject - Supplies a device object for a root port.

    Context - Supplies a pointer to an express bridge for a root port.

Return Value:

    None.

--*/

{

    BOOLEAN SpuriousPmeEvent;
    NTSTATUS Status;

    NON_PAGED_CODE();

    SpuriousPmeEvent = ExpressRootPortCheckForSpuriousPmeEvent(ExpressBridge);
    if (SpuriousPmeEvent != FALSE) {
        goto Exit;
    }

    Status = ExpressRootPortAckPmeEventAtDeviceSource(ExpressBridge);
    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

Exit:

    InterlockedAnd(&ExpressBridge->Downstream.DelegatedWorkItems,
                   ~EXPRESS_PME_WAKE_EVENT_HANDLING);

    //
    // Acknowledge the PME status bit in the root port.
    //

    ExpressRootPortAckPmeEvent(ExpressBridge);

    //
    // Enable Interrupts.
    //

    ExpressRootPortEnableInterrupts(ExpressBridge);
    return;
}

VOID
ExpressRootPortAckPmeEvent (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked to acknowledge a PME wake event by asserting the
    PME Status bit in the RootStatus register.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port.

Return Value:

    None.

--*/

{

    PCI_EXPRESS_ROOT_STATUS_REGISTER ExpressRootStatusRegister;

    NON_PAGED_CODE();

    //
    // Acknowledge the PME status bit.
    //

    RtlZeroMemory(&ExpressRootStatusRegister,
                  sizeof(PCI_EXPRESS_ROOT_STATUS_REGISTER));

    ExpressRootStatusRegister.PMEStatus = 1;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  RootStatus,
                                  &ExpressRootStatusRegister);

    return;
}

// ------------------------------------------------------- Routines for Hot Plug

NTSTATUS
ExpressBridgeProcessNewHotPlugController (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PEXPRESS_LINK ExpressLink,
    __out PBOOLEAN HotPlugCapable,
    __out PPCI_HOTPLUG_CONTROLLER *HotPlugController
    )

/*++

Routine Description:

    This routine initializes a PCI Express hot plug controller. It calls the
    base routine for creating a PCI_HOTPLUG_CONTROLLER and fills in the PCI
    Express specifics.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port or
        a downstream switch port.

    ExpressLink - Supplies a pointer to an express link for a root port or
        downstream switch port.

    HotPlugCapable - Supplies a place holder to indicate that the slot is hot
        plug capable.

    HotPlugController - Supplies a place holder for a hot plug controller.

Return Value:

    STATUS_SUCCESS or an error code returned from a function call.

--*/

{

    PPCI_HOTPLUG_CONTROLLER Controller;
    PCI_EXPRESS_SLOT_CAPABILITIES_REGISTER ExpressSlotCapabilitiesRegister;
    PPCI_HOTPLUG_SLOT Slot;
    NTSTATUS Status;

    PAGED_CODE();

    *HotPlugController = NULL;
    *HotPlugCapable = FALSE;

    PCI_ASSERT((ExpressBridge->BridgeType == PciExpressRootPort) ||
               (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort));

    //
    // If an express bridge does not implement a slot, bail out with success.
    //

    if (ExpressBridge->ExpressCapabilities.SlotImplemented == 0) {
        return STATUS_SUCCESS;
    }

    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotCapabilities,
                                 &ExpressSlotCapabilitiesRegister);

    //
    // If a slot on an express bridge is not hot plug capable, bail out with
    // success.
    //

    if (ExpressSlotCapabilitiesRegister.HotPlugCapable == 0) {
        return STATUS_SUCCESS;
    }

    *HotPlugCapable = TRUE;

    //
    // Invoke the hot plug module routine to create and initialize a hot plug
    // controller for an express bridge.
    //

    Status = PciCreateHotPlugController(ExpressBridge->PciBridge,
                                        1,
                                        &Controller);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    PCI_ASSERT(Controller != NULL);

    Controller->LinkActiveReportingCapable =
        (BOOLEAN)ExpressLink->LinkCapabilities.DataLinkLayerActiveReportingCapable;

    Controller->InstantaneousCommandCompleteSupport =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.NoCommandCompletedSupport;

    //
    // Fill in the callbacks for express bridges.
    //

    Controller->GetDevicePresenceState = ExpressBridgeGetDevicePresenceState;
    Controller->GetLatchState = ExpressBridgeGetLatchState;
    Controller->GetLinkState = ExpressBridgeGetLinkState;
    Controller->ExecuteHotPlugCommands = ExpressBridgeExecuteHotPlugCommands;
    Controller->EnableDisableControllerInterrupts =
        ExpressBridgeEnableDisableControllerInterrupts;

    //
    // Initialize a slot in a controller.
    //

    Slot = &Controller->Slots[0];
    Slot->PhysicalSlotNumber =
        (USHORT)ExpressSlotCapabilitiesRegister.PhysicalSlotNumber;

    //
    // Having a slot implemented implies that the connected device is removable.
    // However, the hack flags can indicate that this bridge does not really
    // expose a removable device.
    //

    if (ExpressBridge->PciBridge->DownstreamDeviceNotRemovable == 0) {
        Slot->Removable = TRUE;

    } else {
        Slot->Removable = FALSE;
    }

    //
    // If the connected device is removable, then also mark whether it can be
    // surprise removed.
    //

    if ((Slot->Removable != FALSE) &&
        (ExpressSlotCapabilitiesRegister.HotPlugSurprise != 0)) {

        Slot->HotPlugSurprise = TRUE;
    }

    //
    // Update information on the type of hot plug elements present for a slot.
    //

    Slot->AttentionButtonPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.AttentionButtonPresent;

    Slot->PowerControllerPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.PowerControllerPresent;

    Slot->MRLSensorPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.MRLSensorPresent;

    Slot->AttentionIndicatorPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.AttentionIndicatorPresent;

    Slot->PowerIndicatorPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.PowerIndicatorPresent;

    Slot->ElectromechanicalLockPresent =
        (BOOLEAN)ExpressSlotCapabilitiesRegister.ElectromechanicalLockPresent;

    //
    // Determine the interrupt to hot plug event mapping. This can vary based
    // on the type of hot plug elements present for a slot.
    //

    PciInitializeHotPlugEventMap(Slot);

    //
    // Examine the slot registers to determine the current state of the slot.
    //

    ExpressBridgeSetInitialSlotState(ExpressBridge, Slot);
    *HotPlugController = Controller;
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressBridgeSetInitialSlotState (
    __in PEXPRESS_BRIDGE ExpressBridge,
    __inout PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine examines various slot registers to determine the current state
    of the slot.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port or
        a downstream switch port.

    Slot - Supplies a pointer to a hot plug capable slot on a bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_SLOT_CONTROL_REGISTER ExpressSlotControlRegister;
    PCI_EXPRESS_SLOT_STATUS_REGISTER ExpressSlotStatusRegister;

    PAGED_CODE();

    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotControl,
                                 &ExpressSlotControlRegister);

    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotStatus,
                                 &ExpressSlotStatusRegister);

    //
    // Check whether the slot retention latch (if present) is open or closed.
    //

    if ((Slot->MRLSensorPresent != FALSE) &&
        (ExpressSlotStatusRegister.MRLSensorState == MRLOpen)) {

        Slot->CurrentState = SlotStateNotReady;
        goto Exit;
    }

    //
    // Check if this slot has an explicit power controller in which case the
    // current slot power setting determines the slot state.
    //

    if (Slot->PowerControllerPresent != FALSE) {
        if (ExpressSlotControlRegister.PowerControllerControl == PowerOn) {
            Slot->CurrentState = SlotStateFullyFunctional;

        } else {

            //
            // Power is off. Set the state of a slot based on the presence of
            // an attention button.
            //

            if (Slot->AttentionButtonPresent != FALSE) {
                if (ExpressSlotStatusRegister.PresenceDetectState == CardPresent) {
                    Slot->CurrentState = SlotStateDisabled;

                } else {
                    Slot->CurrentState = SlotStateNotReady;
                }

            } else {
                Slot->CurrentState = SlotStateDisabled;
            }
        }

        goto Exit;
    }

    //
    // There is no power controller for the slot in which case the slot state
    // can be determined by examining the presence detect state.
    //

    if (ExpressSlotStatusRegister.PresenceDetectState == CardPresent) {
        Slot->CurrentState = SlotStateFullyFunctional;

    } else {
        Slot->CurrentState = SlotStateDisabled;
    }

Exit:
    return STATUS_SUCCESS;
}

BOOLEAN
ExpressBridgeGetDevicePresenceState (
    __in PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine determines whether there is a device in a given slot or not.

Arguments:

    Slot - Supplies a pointer to a hot plug capable slot on a bridge.

Return Value:

    FALSE if the slot is empty (vacant).

    TRUE otherwise.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PCI_EXPRESS_SLOT_STATUS_REGISTER ExpressSlotStatusRegister;

    NON_PAGED_CODE();

    ExpressBridge = Slot->Controller->PciBridge->ExpressBridge;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotStatus,
                                 &ExpressSlotStatusRegister);

    if (ExpressSlotStatusRegister.PresenceDetectState == SlotEmpty) {
        return FALSE;

    } else {
        return TRUE;
    }
}

BOOLEAN
ExpressBridgeGetLatchState (
    __in PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine determines whether the latch (MRL) is closed for a given slot.

Arguments:

    Slot - Supplies a pointer to a hot plug capable slot on a bridge.

Return Value:

    TRUE if the MRL sensors indicate that the latch is closed.

    FALSE otherwise.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PCI_EXPRESS_SLOT_STATUS_REGISTER ExpressSlotStatusRegister;

    NON_PAGED_CODE();

    ExpressBridge = Slot->Controller->PciBridge->ExpressBridge;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotStatus,
                                 &ExpressSlotStatusRegister);

    if (ExpressSlotStatusRegister.MRLSensorState == MRLClosed) {
        return TRUE;

    } else {
        return FALSE;
    }
}

BOOLEAN
ExpressBridgeGetLinkState (
    __in PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine provides the data link layer state.

Arguments:

    Slot - Supplies a pointer to a hot plug capable slot on a bridge.

Return Value:

    TRUE if the data link layer state is active.

    FALSE otherwise.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;

    NON_PAGED_CODE();

    ExpressBridge = Slot->Controller->PciBridge->ExpressBridge;

    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        PCI_ASSERT(DOWNSTREAM_PORT(ExpressBridge));
        return FALSE;
    }

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkStatus,
                                 &ExpressBridge->Downstream.ExpressLink.LinkStatus);

    if (ExpressBridge->Downstream.ExpressLink.LinkStatus.DataLinkLayerActive != 0) {
        return TRUE;

    } else {
        return FALSE;
    }
}

BOOLEAN
ExpressBridgeExecuteHotPlugCommands (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_COMMAND_TYPE Type,
    __in ULONG_PTR Context
    )

/*++

Routine Description:

    This routine is invoked to execute a command on a given slot.

Arguments:

    Slot - Supplies a pointer to a hot plug capable slot on a bridge.

    Type - Supplies the type of command to be executed.

    Context - Supplies context information for the command to be executed.

Return Value:

    TRUE if the command execution requires a command complete interrupt or some
        specified delay before another set of commands can be issued.

    FALSE if the command can be completed instantaneously.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PCI_HOTPLUG_INDICATOR_STATE IndicatorState;
    UCHAR Interrupts;
    PCI_EXPRESS_SLOT_CONTROL_REGISTER NewExpressSlotControlRegister;
    PPCI_BUS Bus;
    BOOLEAN Pending;
    PCI_EXPRESS_SLOT_CONTROL_REGISTER PrevExpressSlotControlRegister;
    PIO_WORKITEM WorkItem;
    PHOTPLUG_WORKER_PARAMETERS WorkItemParameters;

    NON_PAGED_CODE();

    Pending = TRUE;
    Bus = Slot->Controller->PciBridge->Bus;
    ExpressBridge = Slot->Controller->PciBridge->ExpressBridge;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotControl,
                                 &PrevExpressSlotControlRegister);

    RtlCopyMemory(&NewExpressSlotControlRegister,
                  &PrevExpressSlotControlRegister,
                  sizeof(PCI_EXPRESS_SLOT_CONTROL_REGISTER));

    switch (Type) {

    //
    // Set slot power case and set link state case. Performs command by
    // queuing a passive level worker which can do the necessary delays
    // required for these commands.
    //

    case SlotCommandSetSlotPower:
    case SlotCommandSetLinkState:

        WorkItem = ExpressBridge->HotPlugWorkItem;
        WorkItemParameters = &ExpressBridge->HotPlugWorkerParameters;
        WorkItemParameters->Slot = Slot;
        WorkItemParameters->Type = Type;
        WorkItemParameters->CommandContext = Context;

        if (NT_SUCCESS(IoAcquireRemoveLock(&Bus->RemoveLock, NULL)) != FALSE) {
            IoQueueWorkItem(WorkItem,
                            ExpressBridgeExecuteHotPlugCommandsWorker,
                            DelayedWorkQueue,
                            &ExpressBridge->HotPlugWorkerParameters);

        } else {
            return FALSE;
        }

        return TRUE;

    case SlotCommandSetPowerIndicator:

        PCI_ASSERT(Slot->PowerIndicatorPresent != FALSE);

        IndicatorState = (PCI_HOTPLUG_INDICATOR_STATE)Context;
        if (IndicatorState == SlotIndicatorOn) {
            NewExpressSlotControlRegister.PowerIndicatorControl = IndicatorOn;

        } else if (IndicatorState == SlotIndicatorBlinking) {
            NewExpressSlotControlRegister.PowerIndicatorControl =
                IndicatorBlink;

        } else {
            NewExpressSlotControlRegister.PowerIndicatorControl = IndicatorOff;
        }

        break;

    case SlotCommandSetAttentionIndicator:

        PCI_ASSERT(Slot->AttentionIndicatorPresent != FALSE);

        IndicatorState = (PCI_HOTPLUG_INDICATOR_STATE)Context;
        if (IndicatorState == SlotIndicatorOn) {
            NewExpressSlotControlRegister.AttentionIndicatorControl =
                IndicatorOn;

        } else if (IndicatorState == SlotIndicatorBlinking) {
            NewExpressSlotControlRegister.AttentionIndicatorControl =
                IndicatorBlink;

        } else {
            NewExpressSlotControlRegister.AttentionIndicatorControl =
                IndicatorOff;

        }

        break;

    case SlotCommandEnableDisableInterrupts:

        //
        // The context field indicates the specific interrupt bits that should
        // be enabled. If it is 0, then all interrupts should be disabled.
        //

        Interrupts = (UCHAR)Context;
        if ((Interrupts & (1 << SlotInterruptPresenceDetect)) != 0) {
            NewExpressSlotControlRegister.PresenceDetectEnable = 1;

        } else {
            NewExpressSlotControlRegister.PresenceDetectEnable = 0;
        }

        if ((Interrupts & (1 << SlotInterruptLatchChange)) != 0) {
            NewExpressSlotControlRegister.MRLSensorEnable = 1;

        } else {
            NewExpressSlotControlRegister.MRLSensorEnable = 0;
        }

        if ((Interrupts & (1 << SlotInterruptAttentionButton)) != 0) {
            NewExpressSlotControlRegister.AttentionButtonEnable = 1;

        } else {
            NewExpressSlotControlRegister.AttentionButtonEnable = 0;
        }

        if ((Interrupts & (1 << SlotInterruptLinkStateChange)) != 0) {
            NewExpressSlotControlRegister.DataLinkStateChangeEnable = 1;

        } else {
            NewExpressSlotControlRegister.DataLinkStateChangeEnable = 0;
        }

        if ((Interrupts & (1 << SlotInterruptPowerFault)) != 0) {
            NewExpressSlotControlRegister.PowerFaultDetectEnable = 1;

        } else {
            NewExpressSlotControlRegister.PowerFaultDetectEnable = 0;
        }

        //
        // In some systems, if all interrupts are being disabled then
        // command completion interrupts will not be sent out requiring
        // the system to wait 1 second.  This can be costly for sleep
        // transitions.  As such, the system will not wait for this command to
        // complete, but rather assume it completed ok.  Any spurious interrupts
        // that may arise afterwards will be ignored and will be flushed when
        // the system goes down.
        //

        if (Interrupts == 0) {
            Pending = FALSE;
        }

        break;

    default:

        PCI_ASSERT(FALSE);

        Pending = FALSE;
        break;
    }

    //
    // If the controller supports an instantaneous command complete then a
    // software notification will not be delivered and the software is not
    // required to wait for 1 second before issuing the next commmand.
    //

    if (Slot->Controller->InstantaneousCommandCompleteSupport != FALSE){
        Pending = FALSE;
    }

    //
    // Only write to the slot control register if something has actually
    // changed.
    //

    if (NewExpressSlotControlRegister.AsUSHORT !=
        PrevExpressSlotControlRegister.AsUSHORT) {

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      SlotControl,
                                      &NewExpressSlotControlRegister);

    } else {
        Pending = FALSE;
    }

    return Pending;
}

VOID
ExpressBridgeExecuteHotPlugCommandsWorker (
    __in PDEVICE_OBJECT Fdo,
    __in_opt PVOID Context
    )

/*++

Routine Description:

    This routine is a passive level worker to execute commands on a specific
    hotplug slot.

Arguments:

    Context - Supplies a pointer to data required to process command.

Return Value:

    None.

--*/

{

    PHOTPLUG_WORKER_PARAMETERS Parameters;
    ULONG_PTR CommandContext;
    PCI_HOTPLUG_COMMAND_TYPE CommandType;
    LARGE_INTEGER DueTime;
    PEXPRESS_BRIDGE ExpressBridge;
    PPCI_EXPRESS_LINK_CONTROL_REGISTER ExpressLinkControlRegister;
    PCI_EXPRESS_SLOT_CONTROL_REGISTER ExpressSlotControlRegister;
    PCI_HOTPLUG_LINK_STATE LinkState;
    KIRQL OldIrql;
    PPCI_BRIDGE PciBridge;
    PCI_HOTPLUG_POWER_STATE PowerState;
    PPCI_HOTPLUG_SLOT Slot;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Fdo);

    PCI_ASSERT(Context != NULL);

    Parameters = (PHOTPLUG_WORKER_PARAMETERS)Context;
    CommandContext = Parameters->CommandContext;
    CommandType = Parameters->Type;
    Slot = Parameters->Slot;
    PciBridge = Slot->Controller->PciBridge;
    ExpressBridge = PciBridge->ExpressBridge;

    //
    // While these opertions are being completed, block PCI bus scans.
    //

    KeWaitForSingleObject(&PciBridge->Bus->ExpressLinkQuiescentEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    switch (CommandType) {
    case SlotCommandSetSlotPower:

        //
        // Process a slot power change command.
        //

        PCI_ASSERT(Slot->PowerControllerPresent != FALSE);

        PciReadExpressBridgeRegister(ExpressBridge,
                                     SlotControl,
                                     &ExpressSlotControlRegister);

        PowerState = (PCI_HOTPLUG_POWER_STATE)CommandContext;
        if (PowerState == SlotPowerOff) {
            if (ExpressSlotControlRegister.PowerControllerControl != PowerOff) {
                ExpressSlotControlRegister.PowerControllerControl = PowerOff;
                PciWriteExpressBridgeRegister(ExpressBridge,
                                              SlotControl,
                                              &ExpressSlotControlRegister);
            }

        } else {
            if (ExpressSlotControlRegister.PowerControllerControl != PowerOn) {
                ExpressSlotControlRegister.PowerControllerControl = PowerOn;
                PciWriteExpressBridgeRegister(ExpressBridge,
                                              SlotControl,
                                              &ExpressSlotControlRegister);
            }
        }
        break;

    case SlotCommandSetLinkState:

        //
        // Process a link state command to enable or disable a link.
        //

        ExpressLinkControlRegister = NULL;
        if (DOWNSTREAM_PORT(ExpressBridge)) {
            ExpressLinkControlRegister =
                &ExpressBridge->Downstream.ExpressLink.LinkControl;
        } else {
            PCI_ASSERT(FALSE);
            break;
        }

        //
        // Some hot plug controllers require a SBR before any changes to the
        // link disable bit.
        //

        if (PciBridge->SecondaryBusResetOnLinkStateChange != 0) {
            PciBridgeAssertSecondaryReset(PciBridge);
        }

        LinkState = (PCI_HOTPLUG_LINK_STATE)CommandContext;
        if (LinkState == LinkStateDisable) {
            ExpressLinkControlRegister->LinkDisable = 1;

        } else {
            ExpressLinkControlRegister->LinkDisable = 0;
        }

        PciWriteExpressBridgeRegister(ExpressBridge,
                                      LinkControl,
                                      ExpressLinkControlRegister);

        //
        // Delay 1ms after writing the Link Disable bit.  This allows the
        // hardware to observe the link disable.
        //

        ExpressBridge->PciBridge->StallDispatch(ExpressBridge->PciBridge->InterfaceContext, 1000);

        //
        // Deassert SBR on systems that require it.  This finishes the
        // link state change process.
        //

        if (PciBridge->SecondaryBusResetOnLinkStateChange != 0) {
            PciBridgeDeassertSecondaryReset(PciBridge);
        }
        break;

    default:

        PCI_ASSERT(FALSE);

        break;

    }

    //
    // Set a timer to unblock the hotplug action queue.  This timer serves two
    // purposes:
    //
    // 1. This routine is currently holding the link quiescent event lock.
    //    Unblocking the hotplug action queue will release code that needs to
    //    wait on this event.  Unblocking the queue from the timer will reduce
    //    the amount of time spent blocked waiting for this event.
    //
    // 2. Provide a timeout for the slot power command complete interrupt.
    //
    // N.B. Purpose 1 is an optimization to allow more efficient processing of
    //      the work item queue.  Therefore, if it is canceled early by the
    //      arrival of a command complete interrupt, correctness is not lost.
    //      Purpose 2 is required as part of the PCIe spec.
    //

    KeAcquireSpinLock(&Slot->Controller->TimerLock, &OldIrql);
    if (Slot->Controller->TimersDisabled == FALSE) {
        DueTime.QuadPart = -10 * 1000 * 1000;
        KeSetTimer(&Slot->Controller->CommandTimer,
                   DueTime,
                   &Slot->Controller->CommandTimerDpc);
    }

    KeReleaseSpinLock(&Slot->Controller->TimerLock, OldIrql);

    //
    // Set a timer to unblock bus accesses and release reference to the bus.
    //

    DueTime.QuadPart = -10 * 1000 * 100;
    KeSetTimer(&ExpressBridge->HotPlugCommandTimer,
               DueTime,
               &ExpressBridge->HotPlugCommandDpc);

    return;
}

VOID
ExpressBridgeExecuteHotPlugCommandsTimer (
    __in PKDPC Dpc,
    __in_opt PVOID DeferredContext,
    __in_opt PVOID SystemArgument1,
    __in_opt PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine releases locks held by the hotplug command worker after the
    appropriate settle time has elapsed.

Arguments:

    Dpc - Unused.

    DeferredContext - Supplies a pointer to the bus being operated on

    SystemArgument1 - Unused.

    SystemArgument2 - Unused.

Return Value:

    None.

--*/

{
    PPCI_BUS Bus;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    if (DeferredContext == NULL) {
        return;
    }

    Bus = (PPCI_BUS)DeferredContext;

    KeSetEvent(&Bus->ExpressLinkQuiescentEvent, IO_NO_INCREMENT, FALSE);

    IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    return;
}

BOOLEAN
ExpressBridgeEnableDisableControllerInterrupts (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN Enable
    )

/*++

Routine Description:

    This routine is invoked to enable or disable hot polug interrupt or command
    complete interrupt for a controller.

Arguments:

    Controller - Supplies a pointer to a hot plug controller for a slot.

    Enable - Supplies a boolean to indicate if interrupts are to be enabled or
        disabled.

Return Value:

    TRUE if the command execution requires a command complete interrupt or some
        specified delay before another set of commands can be issued.

    FALSE if the command can be completed instantaneously.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PCI_EXPRESS_SLOT_CONTROL_REGISTER ExpressSlotControlRegister;
    BOOLEAN Pending;

    NON_PAGED_CODE();

    ExpressBridge = Controller->PciBridge->ExpressBridge;
    Pending = FALSE;
    PciReadExpressBridgeRegister(ExpressBridge,
                                 SlotControl,
                                 &ExpressSlotControlRegister);

    if (Enable != FALSE) {

        ExpressSlotControlRegister.HotPlugInterruptEnable = 1;
        if (Controller->InstantaneousCommandCompleteSupport == FALSE) {

            ExpressSlotControlRegister.CommandCompletedEnable = 1;

            //
            // Without instaneous command complete support, the system
            // should wait until either a command completion interrupt
            // is received a the 1 second timeout has expired.  Only
            // in the case of disabling interrupts should the system
            // not need to wait.  This may allow a spurious command
            // completion interrupt to get through, but it will be
            // ignored by PCI.
            //

            Pending = TRUE;
        }

    } else {
        ExpressSlotControlRegister.HotPlugInterruptEnable = 0;
        ExpressSlotControlRegister.CommandCompletedEnable = 0;
    }

    PciWriteExpressBridgeRegister(ExpressBridge,
                                  SlotControl,
                                  &ExpressSlotControlRegister);

    //
    // Return a boolean indicating if the command can be completed
    // instantaneously.
    //

    return Pending;
}

// --------------------------------------------------------------- Misc Routines

VOID
ExpressRootPortUpdateMaxPayloadSize (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in USHORT NewMaxPayloadSize
    )

/*++

Routine Description:

    This routine is invoked to update the max payload size for a root port.
    The new value comes from the capability of a device on the root port.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

    NewMaxPayloadSize - Supplies the value for max payload size.

Return Value:

    None.

--*/

{

    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlRegister;

    PAGED_CODE();

    PCI_ASSERT(ExpressBridge->BridgeType == PciExpressRootPort);

    PciReadExpressBridgeRegister(ExpressBridge,
                                 DeviceControl,
                                 &DeviceControlRegister);

    DeviceControlRegister.MaxPayloadSize = NewMaxPayloadSize;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  DeviceControl,
                                  &DeviceControlRegister);

    ExpressBridge->Downstream.u.RootPort.MaxPayloadSize = NewMaxPayloadSize;
    return;
}

BOOLEAN
ExpressBridgeSetAriEnable (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in BOOLEAN OnOrOff
    )

/*++

Routine Description:

    This routine enables or disables ARI forwarding on the bridge.

Arguments:

    ExpressBridge - Supplies the bridge upon which to set ARI forwarding

    OnOrOff - Supplies the state that we'd like ARI forwarding to be; that
        is, TRUE to enable, FALSE to disable.

Return Value:

    Returns TRUE if ARI forwarding was set as requested, otherwise FALSE.

--*/

{

    PAGED_CODE();

    if (ExpressBridge->AriForwardingSupported == FALSE) {
        return FALSE;
    }

    if (ExpressBridge->AriForwardingEnabled == OnOrOff) {
        return TRUE;
    }

    if (ExpressBridge->AriForwardingEnable != NULL) {
        ExpressBridge->AriForwardingEnable(ExpressBridge->PciBridge->InterfaceContext, OnOrOff);
        ExpressBridge->AriForwardingEnabled = OnOrOff != FALSE ? 1 : 0;
    }

    return TRUE;
}
