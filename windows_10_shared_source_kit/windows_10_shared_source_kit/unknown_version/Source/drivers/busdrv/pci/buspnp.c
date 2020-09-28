/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    buspnp.c

Abstract:

    This module handles IRPs for PCI FDO's.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "buspnp.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciBus_Start(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_Remove(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );


NTSTATUS
PciBus_SurpriseRemoval(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_CancelRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_Stop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_CancelStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryDeviceRelations(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryCapabilities(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_FilterResourceRequirements(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryInterface(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryLegacyBusInformation(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_DeviceUsageNotification(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_QueryDeviceState(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciBus_Start)
    #pragma alloc_text(PAGE, PciBus_QueryRemove)
    #pragma alloc_text(PAGE, PciBus_SurpriseRemoval)
    #pragma alloc_text(PAGE, PciBus_CancelRemove)
    #pragma alloc_text(PAGE, PciBus_QueryStop)
    #pragma alloc_text(PAGE, PciBus_Stop)
    #pragma alloc_text(PAGE, PciBus_CancelStop)
    #pragma alloc_text(PAGE, PciBus_QueryDeviceRelations)
    #pragma alloc_text(PAGE, PciBus_QueryDeviceState)
    #pragma alloc_text(PAGE, PciBus_QueryInterface)
    #pragma alloc_text(PAGE, PciBus_QueryCapabilities)
    #pragma alloc_text(PAGE, PciBus_FilterResourceRequirements)
    #pragma alloc_text(PAGE, PciBus_DeviceUsageNotification)
    #pragma alloc_text(PAGE, PciBus_QueryLegacyBusInformation)
#endif

// Disable warning C4100: 'X' : unreferenced formal parameter
#pragma warning(disable:4100)

// --------------------------------------------------------------------- Globals

const PCI_MN_DISPATCH_ENTRY PciBus_DispatchPnpTable[PCI_MAX_MINOR_PNP_IRP + 2] = {
    { IRP_UPWARD,   PciBus_Start                       , FALSE}, // 0x00 - IRP_MN_START_DEVICE
    { IRP_DOWNWARD, PciBus_QueryRemove                 , FALSE}, // 0x01 - IRP_MN_QUERY_REMOVE_DEVICE
    { IRP_DISPATCH, PciBus_Remove                      , FALSE}, // 0x02 - IRP_MN_REMOVE_DEVICE
    { IRP_DOWNWARD, PciBus_CancelRemove                , FALSE}, // 0x03 - IRP_MN_CANCEL_REMOVE_DEVICE
    { IRP_DOWNWARD, PciBus_Stop                        , FALSE}, // 0x04 - IRP_MN_STOP_DEVICE
    { IRP_UPWARD,   PciBus_QueryStop                   , FALSE}, // 0x05 - IRP_MN_QUERY_STOP_DEVICE
    { IRP_DOWNWARD, PciBus_CancelStop                  , FALSE}, // 0x06 - IRP_MN_CANCEL_STOP_DEVICE
    { IRP_DISPATCH, PciBus_QueryDeviceRelations        , FALSE}, // 0x07 - IRP_MN_QUERY_DEVICE_RELATIONS
    { IRP_DISPATCH, PciBus_QueryInterface              , FALSE}, // 0x08 - IRP_MN_QUERY_INTERFACE
    { IRP_UPWARD,   PciBus_QueryCapabilities           , FALSE}, // 0x09 - IRP_MN_QUERY_CAPABILITIES
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x0A - IRP_MN_QUERY_RESOURCES
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x0B - IRP_MN_QUERY_RESOURCE_REQUIREMENTS
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x0C - IRP_MN_QUERY_DEVICE_TEXT
    { IRP_DOWNWARD, PciBus_FilterResourceRequirements  , FALSE}, // 0x0D - IRP_MN_FILTER_RESOURCE_REQUIREMENTS
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x0E - NOT USED
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x0F - IRP_MN_READ_CONFIG
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x10 - IRP_MN_WRITE_CONFIG
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x11 - IRP_MN_EJECT
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x12 - IRP_MN_SET_LOCK
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x13 - IRP_MN_QUERY_ID
    { IRP_DOWNWARD, PciBus_QueryDeviceState            , FALSE}, // 0x14 - IRP_MN_QUERY_PNP_DEVICE_STATE
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x15 - IRP_MN_QUERY_BUS_INFORMATION
    { IRP_UPWARD,   PciBus_DeviceUsageNotification     , FALSE}, // 0x16 - IRP_MN_DEVICE_USAGE_NOTIFICATION
    { IRP_DOWNWARD, PciBus_SurpriseRemoval             , FALSE}, // 0x17 - IRP_MN_SURPRISE_REMOVAL
    { IRP_DOWNWARD, PciBus_QueryLegacyBusInformation   , FALSE}, // 0x18 - IRP_MN_QUERY_LEGACY_BUS_INFORMATION
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}, // 0x19 - IRP_MN_DEVICE_ENUMERATED
    { IRP_DOWNWARD, PciIrpNotSupported                 , FALSE}  //      - UNHANDLED PNP IRP
};

// ------------------------------------------------------------------- IRP Handler Functions

/*++

The majority of functions in this file are called based on their presence
in the Pnp dispatch table.  In the interests of brevity the arguments
to all those functions will be described below:

NTSTATUS
PciBus_Xxx(
    IN PIRP Irp,
    IN PIO_STACK_LOCATION IrpStack,
    IN PPCI_COMMON_EXTENSION DeviceExtension
    )

Routine Description:

    This function handles the Xxx requests for a given PCI Bus.

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
PciBus_Start(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resourceArray;
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    PCM_RESOURCE_LIST resources, translated;
    PEXPRESS_BRIDGE expressBridge;
    ULONG resourceCount;
    BOOLEAN firstStart;
    BOOLEAN runClear;
    ULONG_PTR state;
    NTSTATUS status;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    status = PciBeginStateTransition(DeviceExtension, PciStarted);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    resources = IrpSp->Parameters.StartDevice.AllocatedResources;
    translated = IrpSp->Parameters.StartDevice.AllocatedResourcesTranslated;

    if (bus->StartedAlready == 0) {
        status = BusPowerRegisterPep(bus);
    }

    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    firstStart = (bus->StartedAlready == 0) ? TRUE : FALSE;

    PCI_BUS_SET_FLAG(bus, StartedAlready);

    if (bus->PciBridge) {
        status = PciBridgeStart(bus->PciBridge, resources, translated);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        resourceArray = bus->PciBridge->Resources.ByIndex;
        resourceCount = PCI_BRIDGE_RESOURCE_COUNT;

        //
        // There are a few things that need doing related to express bridges
        // on the first start.
        //

        if (DeviceExtension->DeviceState == PciNotStarted) {

            expressBridge = bus->PciBridge->ExpressBridge;
            if (expressBridge != NULL) {

                //
                // Check to see whether this bus contains a hotplug controller
                // and if it does, reference the wake state machine so that the
                // bus will be armed for wake when it idles.  This allows the
                // BIOS to indicate that a hot-plug event occurred by doing a
                // "Notify(bus, 2)" statement in AML.
                //

                if (PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(bus)) {
                    PciBus_ArmForWake(bus);
                }

                if ((firstStart != FALSE) &&
                    (expressBridge->GetPowerParameters != NULL)) {

                    //
                    // Populate power parameters at the FDO layer from the PDO.
                    //

                    expressBridge->GetPowerParameters(
                        expressBridge->PciBridge->InterfaceContext,
                        &expressBridge->UpstreamLatency,
                        &expressBridge->ObffConfiguration);

                    //
                    // Calculate the LTR values for downstream devices.
                    //

                    ExpressBridgeCalculateLtrValues(
                        expressBridge,
                        expressBridge->UpstreamLatency.AsULONG);
                }
            }
        }

    } else {

        status = PciRootFilterStartResources(bus, resources, translated);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        //
        // Present the properties of a PCI root bus to other user or kernel
        // mode components with use of custom property APIs.
        //

        status = PciRootSetCustomProperties(bus->Root);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        //
        // --HACK--
        //
        // Some BIOSes report that host bridges only pass bus number 0,
        // even though they are single host bridge machines that pass all
        // bus numbers (0-FF).  We don't want to detect host bridges that
        // just pass 0 and increase them to 0-FF, since it's perfectly
        // valid to pass only 0 in multi host bridge machines.  So detect
        // single host bridge machines that only pass 0.  We can't do this
        // with the rest of the bus number configuration in AddDevice
        // because we don't know if it's a single host bridge at that point.
        // A single host brige will always be in segment 0.
        //
        PciAcquirePassiveLock(&PciGlobalLock);
        if ((PciSegmentList->Next == NULL) &&
            (PciSegmentList->SegmentNumber == 0) &&
            (bus->Root->Segment->PciRootBusList ==
             bus->Root->Segment->PciRootBusTail) &&
            (LOCAL_BUS_NUMBER(bus->SecondaryBusNumber) == 0) &&
            (LOCAL_BUS_NUMBER(bus->SubordinateBusNumber) == 0)) {

            runClear = RtlAreBitsClear(&bus->Root->Segment->BusNumberMap,
                                       1,
                                       0xFF);

            PCI_ASSERT(runClear == TRUE);
            UNREFERENCED_PARAMETER(runClear);

            bus->SubordinateBusNumber = FULL_BUS_NUMBER(0, 0xFF);
            PciSetBitRangeInclusive(&bus->Root->Segment->BusNumberMap, 1, 0xFF);
        }

        PciReleasePassiveLock(&PciGlobalLock);

        //
        // For a root bus, blindly initialize the arbiters
        // with the start resources.
        //
        PCI_ASSERT(!bus->ArbitersInitialized);

        if (resources) {

            resourceArray = resources->List[0].PartialResourceList.PartialDescriptors;
            resourceCount = resources->List[0].PartialResourceList.Count;

        } else {

            resourceArray = NULL;
            resourceCount = 0;
        }
    }

    //
    // If this is the first start for the bus and the bus
    // has some resources, initialize the arbiters.
    //
    if (!bus->ArbitersInitialized) {

        status = PciInitializeArbiters(bus);
        if (!NT_SUCCESS(status)) {
            TRACE(RESOURCE_TRACE_LEVEL,
                  "PciBus_Start: Failure to initialize arbiters for bus 0x%p\n",
                  bus
                  );
            goto cleanup;
        }
    }

    if (resourceArray) {

        //
        // Now program the arbiters with the start resources.
        // If an arbiter is in the REPROGRAMMED state, this means that the
        // arbiter has already been re-initialized as part of a rebalance,
        // and this should not be redone here.
        //
        if (bus->MemoryArbiter) {

            extension = bus->MemoryArbiter->Extension;

            if (extension->ArbiterState != PCI_ARBITER_STATE_REPROGRAMMED) {

                if (extension->ArbiterState == PCI_ARBITER_STATE_CONFIGURED) {

                    RtlFreeRangeList(bus->MemoryArbiter->Allocation);
                }

                status = PciProgramMemoryArbiter(bus,
                                                 bus->MemoryArbiter,
                                                 resourceArray,
                                                 resourceCount,
                                                 bus->MemoryArbiter->Allocation
                                                 );
                if (!NT_SUCCESS(status)) {
                    goto cleanup;
                }
            }

            extension->ArbiterState = PCI_ARBITER_STATE_CONFIGURED;
        }

        if (bus->IoArbiter) {

            state = (ULONG_PTR)bus->IoArbiter->Extension;

            if (state != PCI_ARBITER_STATE_REPROGRAMMED) {

                if (state == PCI_ARBITER_STATE_CONFIGURED) {

                    RtlFreeRangeList(bus->IoArbiter->Allocation);
                }

                status = PciProgramIoArbiter(bus,
                                             bus->IoArbiter,
                                             resourceArray,
                                             resourceCount,
                                             bus->IoArbiter->Allocation
                                             );
                if (!NT_SUCCESS(status)) {
                    goto cleanup;
                }
            }

            bus->IoArbiter->Extension = (PVOID)PCI_ARBITER_STATE_CONFIGURED;
        }

    } else {

        PCI_ASSERT(!bus->MemoryArbiter && !bus->IoArbiter);
    }

    PciCommitStateTransition(DeviceExtension, PciStarted);
    return STATUS_SUCCESS;

cleanup:

    PciCancelStateTransition(DeviceExtension, PciStarted);
    return status;
}

NTSTATUS
PciBus_QueryRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PAGED_CODE();

    return PciBeginStateTransition(DeviceExtension, PciDeleted);
}

NTSTATUS
PciBus_Remove(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PDEVICE_OBJECT attachedDevice;
    PPCI_CHILDREN_BUSNUM_HINT ChildBusNumHint;
    PPCI_DEVICE device;
    PLIST_ENTRY Entry;
    NTSTATUS status;
    PPCI_BUS bus;

    NON_PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;
    while ((Entry = bus->ChildrenBusNumHints.Flink) !=
           &bus->ChildrenBusNumHints) {

        RemoveEntryList(Entry);
        ChildBusNumHint = CONTAINING_RECORD(Entry,
                                            PCI_CHILDREN_BUSNUM_HINT,
                                            ListEntry);

        PciFreePool(ChildBusNumHint);
    }

    //
    // If this bus contained a hot-plug controller, drop the wake reference
    // that was taken in PciBus_Start above.
    //

    if (PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(bus)) {
        PciBus_DisarmForWake(bus);
    }

    //
    // Drive the power and wake state machines to completion.
    //

    PciBus_RunDownWake(bus);

    //
    // It's possible at this point that child devices have D0 IRPs pending.
    // Let them go.  And, since power is required for this, and further
    // that PciBus_RetireChildPowerIrps drops a power reference, take
    // a reference here.  (It's guaranteed at this point that the bus is
    // actually "active" with respect to PoFx since we're in the middle
    // of removal.)
    //

    if (bus->StartedAlready != 0) {
        PciBus_PowerReference(bus, PciBusPowerReasonRetiringChildD0Irps);
        PciBus_RetireChildPowerIrps(bus);
    }

    if (bus->D3coldIntrf.InterfaceDereference != NULL) {
        bus->D3coldIntrf.InterfaceDereference(bus->D3coldIntrf.Context);
    }

    PciAcquirePassiveLock(&PciGlobalLock);
    PciAcquirePassiveLockInCriticalSection(&bus->ChildDeviceLock);

    while (bus->ChildDevices) {

        device = (PPCI_DEVICE)bus->ChildDevices;

        status = IoAcquireRemoveLock(&device->RemoveLock, NULL);

        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        IoReleaseRemoveLockAndWait(&device->RemoveLock, NULL);

        TRACE(PNP_TRACE_LEVEL,
              "Killing PDO %p PDOx %p (b=%x, d=%d, f=%d)\n",
              device->DeviceObject, device,
              bus->SecondaryBusNumber,
              device->Slot.u.bits.DeviceNumber,
              device->Slot.u.bits.FunctionNumber
              );

        PCI_ASSERT(device->DeviceState == PciNotStarted);

        PciDestroyDevice(device);
    }

    PciReleasePassiveLockInCriticalSection(&bus->ChildDeviceLock);
    PciReleasePassiveLock(&PciGlobalLock);

    //
    // Disconnect from the Po runtime framework.
    //

    BusPowerUnregisterPep(bus);

    //
    // Note that a filter above us may have failed Start. If this is so, we get
    // no query because the "devnode" has never been started...
    //
    if (!PciIsInTransitionToState(DeviceExtension, PciDeleted)) {

        status = PciBeginStateTransition(DeviceExtension, PciDeleted);
        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);
    }

    PciCommitStateTransition(DeviceExtension, PciDeleted);

    PciRemoveBusFromTree(bus);

    //
    // Track interface reference counts
    //
    if (bus->PciRoutingInterfaceCount ||
        bus->PciLegacyDeviceInterfaceCount ||
        bus->PciPmeInterfaceCount ||
        bus->PciLocationInterfaceCount) {
        TRACE(PNP_TRACE_LEVEL, "Outstanding interface reference count(s)\n"
                               "     PciRoutingInterfaceCount : %x\n"
                               "PciLegacyDeviceInterfaceCount : %x\n"
                               "         PciPmeInterfaceCount : %x\n"
                               "    PciLocationInterfaceCount : %x\n",
                               bus->PciRoutingInterfaceCount,
                               bus->PciLegacyDeviceInterfaceCount,
                               bus->PciPmeInterfaceCount,
                               bus->PciLocationInterfaceCount);
    }

    status = IoAcquireRemoveLock(&bus->RemoveLock, NULL);

    PCI_ASSERT(NT_SUCCESS(status));
    UNREFERENCED_PARAMETER(status);

    if (bus->PciBridge != NULL) {
        if (bus->PciBridge->ExpressBridge != NULL) {

            ExpressBridgeDisableInterrupts(bus->PciBridge->ExpressBridge);
            ExpressBridgeDisconnectInterrupt(bus->PciBridge->ExpressBridge);
        }
    }

    IoReleaseRemoveLockAndWait(&bus->RemoveLock, NULL);
    PciDestroyArbiters(bus);

    if (bus->PciBridge != NULL) {
        if (bus->PciBridge->ExpressBridge != NULL) {

            //
            // Before destroying the virtual bridge, allow any DPCs
            // queued by the ISRs to complete.  These DPCs operate on
            // the bridge.
            //

            KeFlushQueuedDpcs();
            ExpressDestroyVirtualBridge(bus->PciBridge->ExpressBridge);
            bus->PciBridge->ExpressBridge = NULL;
        }

        PciFreePool(bus->PciBridge);
        bus->PciBridge = NULL;

    } else {

        PciDestroyRootBus(bus);
    }

    TRACE(PNP_TRACE_LEVEL, "Bus (%p) destroyed.", bus);

    if (bus->BlackBoxHandle != NULL) {
        WppRecorderLogDelete(bus->BlackBoxHandle);
    }

    //
    // The IRP needs to go down the device stack but we
    // need to remove the device from the stack so grab
    // the next object first, then detach, then pass it
    // down.
    //
    attachedDevice = bus->AttachedDeviceObject;
    IoDetachDevice(attachedDevice);
    IoDeleteDevice(bus->DeviceObject);

    //
    // Delete the control device if root count drops to 0
    //
    if (InterlockedDecrement(&PciCreateControlDeviceCount) == 0) {
        PciCleanupCtrlDevExt(
            (PCONTROL_DEVICE_EXTENSION)PciControlDevice->DeviceExtension);
        IoDeleteDevice(PciControlDevice);
        PciControlDevice = NULL;
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(attachedDevice, Irp);
}

NTSTATUS
PciBus_SurpriseRemoval(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_BUS bus;
    NTSTATUS status;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    status = PciBeginStateTransition(DeviceExtension, PciSurpriseRemoved);
    if (!NT_SUCCESS(status)) {
        ERROR("PciBus_SurpriseRemoval: begin state transition failed (0x%08X).\n",
              status);

        return status;
    }

    PciCommitStateTransition(DeviceExtension, PciSurpriseRemoved);

    //
    // Disable interrupts and disconnect.
    //

    if ((bus->PciBridge != NULL) && (bus->PciBridge->ExpressBridge != NULL)) {
        ExpressBridgeDisableInterrupts(bus->PciBridge->ExpressBridge);
        ExpressBridgeDisconnectInterrupt(bus->PciBridge->ExpressBridge);
    }

    return PciBeginStateTransition(DeviceExtension, PciDeleted);
}

NTSTATUS
PciBus_CancelRemove(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PAGED_CODE();

    PciCancelStateTransition(DeviceExtension, PciDeleted);
    return STATUS_SUCCESS;
}

NTSTATUS
PciBus_QueryStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_BRIDGE_REQUIREMENTS minimums;
    PPCI_BRIDGE_RESOURCES resources;
    ULONGLONG prefetchWindowLength;
    ULONGLONG prefetchWindowStart;
    PPCI_BRIDGE bridge;
    NTSTATUS status;
    PPCI_BUS bus;
    BOOLEAN initialPrefetchWindowDisabled;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    //
    // Only support rebalance on PCI-PCI bridges.  Root
    // busses are not rebalanceable.
    //
    if (!bus->PciBridge) {

        return STATUS_UNSUCCESSFUL;
    }

    bridge = bus->PciBridge;

    //
    // Save the initial state of the prefetch window.
    // Report that resource requirements have changed if this window opens up
    // while handling this IRP.
    //

    initialPrefetchWindowDisabled =
        bridge->MinimumRequirements.PrefetchWindow.Disabled;

    //
    // Get the new requirements of the bridge, based on the current set
    // of devices on the secondary bus.
    //
    status = PciBridgeComputeRequirements(bridge,
                                          &bridge->PreferredRequirements,
                                          &bridge->FallbackRequirements,
                                          &bridge->SubtractiveRequirements
                                          );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    minimums = &bridge->FallbackRequirements;
    resources = &bridge->Resources;

    //
    // If any of the bridge windows are smaller or not aligned properly or
    // constrained due to the location characteristics for the newly computed
    // requirements, return a special status code that will cause a new
    // IRP_MN_QUERY_RESOURCE_REQUIREMENTS to be sent.
    //
    // Only do this if the current resources do not meet the *fallback*
    // requirements. If the resources meet the fallback requirements but not the
    // preferred requirements, don't trigger a change in requirements. This has
    // the effect of causing fewer bridges to have their requirements
    // re-queried, simplifying the rebalance operation.
    //
    // I/O Windows in subtractive bridges should never trigger expansion, as
    // we never want to open them.  (Note that merely avoiding triggering
    // expansion doesn't stop them from being opened, as something else may
    // return STATUS_RESOURCE_REQUIREMENTS_CHANGED.)
    //

    if ((bridge->Attributes.Subtractive == 0) &&
        ((minimums->IoWindow.Size > resources->IoWindow.u.Port.Length) ||
         ((resources->IoWindow.u.Port.Start.QuadPart %
           minimums->IoWindow.Alignment) != 0))) {

        PCI_ASSERT(minimums->IoWindow.Disabled == FALSE);

        status = STATUS_RESOURCE_REQUIREMENTS_CHANGED;
        goto Exit;
    }

    if ((minimums->MemoryWindow.Size > resources->MemoryWindow.u.Memory.Length) ||
        ((resources->MemoryWindow.u.Memory.Start.QuadPart %
          minimums->MemoryWindow.Alignment) != 0)) {

        status = STATUS_RESOURCE_REQUIREMENTS_CHANGED;
        goto Exit;
    }

    if (resources->PrefetchWindow.Type != CmResourceTypeNull) {
        prefetchWindowLength =
            RtlCmDecodeMemIoResource(&resources->PrefetchWindow,
                                     &prefetchWindowStart);

        if ((minimums->PrefetchWindow.Size > prefetchWindowLength) ||
            ((prefetchWindowStart % minimums->PrefetchWindow.Alignment) != 0)) {

            status = STATUS_RESOURCE_REQUIREMENTS_CHANGED;
            goto Exit;
        }
    }

    //
    // If we have forced a previously disabled window to open up in this query stop,
    // ask for a query requirements IRP to be sent.
    //

    if(resources->PrefetchWindow.Type == CmResourceTypeNull &&
       !minimums->PrefetchWindow.Disabled &&
       initialPrefetchWindowDisabled)
    {
        status = STATUS_RESOURCE_REQUIREMENTS_CHANGED;
        goto Exit;
    }


    if (PciBridgeCheckLocationCompatibility(bridge, resources, minimums) == FALSE) {
        status = STATUS_RESOURCE_REQUIREMENTS_CHANGED;
        goto Exit;
    }

    status = STATUS_SUCCESS;

Exit:

    PciBeginStateTransition(DeviceExtension, PciStopped);
    return status;
}

NTSTATUS
PciBus_Stop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;
    if ((bus->PciBridge != NULL) &&
        (bus->PciBridge->ExpressBridge != NULL)) {

        ExpressBridgeStop(bus->PciBridge->ExpressBridge);
    }

    PciCommitStateTransition(DeviceExtension, PciStopped);
    return STATUS_SUCCESS;
}

NTSTATUS
PciBus_CancelStop(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    if (bus->MemoryArbiter) {

        extension = bus->MemoryArbiter->Extension;
        extension->ArbiterState = PCI_ARBITER_STATE_CONFIGURED;
    }

    if (bus->IoArbiter) {

        bus->IoArbiter->Extension = (PVOID)PCI_ARBITER_STATE_CONFIGURED;
    }

    PciCancelStateTransition(DeviceExtension, PciStopped);
    return STATUS_SUCCESS;
}

NTSTATUS
PciBus_QueryDeviceRelations(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{

    PPCI_BUS bus;
    PPCI_DEVICE childDevice;
    LARGE_INTEGER delay;
    ULONG deviceCount;
    PDEVICE_RELATIONS deviceRelations;    
    PDEVICE_RELATIONS oldDeviceRelations;
    PDEVICE_OBJECT *object;
    ULONG size;
    BOOLEAN stateLockOwned;
    NTSTATUS status;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;
    stateLockOwned = FALSE;

    //
    // Only bus relations are handled.
    //
    if (IrpSp->Parameters.QueryDeviceRelations.Type != BusRelations) {
        status = PciPassIrp(DeviceExtension, Irp);
        return status;
    }

    //
    // Bring this bus into D0, if it isn't already.
    //

    PciBus_PowerReference(bus, PciBusPowerReasonBusPnpOperationPending);

    //
    // Now wait 100ms (as required by the PCI specs (conventional, X and
    // Express) after turning on bus power so that the devices are ready for
    // their close-ups.  Add some additional time for non-compliant devices.
    //

    delay = PciBus_WaitTimeForBusSettle(bus,
                                        DEVICE_LINK_ON_SETTLE_TIME +
                                        (DEVICE_LINK_ON_SETTLE_TIME / 2));

    if (delay.QuadPart != 0) {

        //
        // Make it a relative wait.
        //

        delay.QuadPart = (-1) * delay.QuadPart;
        KeDelayExecutionThread(KernelMode, FALSE, &delay);
    }

    //
    // Check that it reasonable to perform this operation now.
    //
    if (DeviceExtension->DeviceState != PciStarted) {

        TraceEvents(bus->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_PNP,
                    "device is not started, current device state: %!PNP_STATE!.",
                    DeviceExtension->DeviceState);

        status = STATUS_INVALID_DEVICE_REQUEST;
        goto exit;
    }

    oldDeviceRelations = (PDEVICE_RELATIONS)Irp->IoStatus.Information;

    //
    // We're going to mess with the child pdo list - lock the state...
    //
    status = PCI_ACQUIRE_STATE_LOCK(DeviceExtension);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    stateLockOwned = TRUE;

    //
    // Enumerate the bus.
    //

    KeWaitForSingleObject(&bus->ExpressLinkQuiescentEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    status = PciScanBus(bus, &deviceCount);
    KeSetEvent(&bus->ExpressLinkQuiescentEvent, IO_NO_INCREMENT, FALSE);
    if (!NT_SUCCESS(status) && (status != STATUS_NOT_FOUND)) {
        TraceEvents(bus->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_PNP,
                    "bus scan failed %!STATUS!",
                    status);
        goto exit;
    }

    TraceEvents(bus->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_PNP,
                "Scan bus completed");

    //
    // We could be either (a) creating the DEVICE_RELATIONS structure
    // (list) here, or (b) adding our PDOs to an existing list.
    //
    if (oldDeviceRelations) {

        deviceCount += oldDeviceRelations->Count;
    }

    size = FIELD_OFFSET(DEVICE_RELATIONS, Objects)
           + (deviceCount * sizeof(PDEVICE_OBJECT));

    deviceRelations = PciAllocatePool(NonPagedPoolNx, size);
    if (deviceRelations == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    deviceRelations->Count = deviceCount;
    object = deviceRelations->Objects;

    //
    // Copy and free the old list.
    //
    if (oldDeviceRelations) {

#pragma prefast(suppress:__WARNING_POTENTIAL_BUFFER_OVERFLOW_LOOP_DEPENDENT, "io_x.h isn't specfic enough for this to be expressable")
        RtlCopyMemory(deviceRelations->Objects,
                      oldDeviceRelations->Objects,
                      oldDeviceRelations->Count * sizeof(PDEVICE_OBJECT)
                      );

        object += oldDeviceRelations->Count;

        PciFreeExternalPool(oldDeviceRelations);
        oldDeviceRelations = NULL;
    }

    //
    // Add our entries.
    //
    TraceEvents(bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "QueryBusRelations");

    PciAcquirePassiveLock(&bus->ChildDeviceLock);
    childDevice = bus->ChildDevices;
    while (childDevice) {

        if (childDevice->Present != FALSE) {
            TraceEvents(bus->BlackBoxHandle,
                        TRACE_LEVEL_INFORMATION,
                        DBG_PNP,
                        "  PDO %p (x %p)",
                        childDevice->DeviceObject,
                        childDevice);
        } else {
            TraceEvents(bus->BlackBoxHandle,
                        TRACE_LEVEL_INFORMATION,
                        DBG_PNP,
                        "  PDO %p (x %p)  <Omitted, device flaged not present>",
                        childDevice->DeviceObject,
                        childDevice);
        }

        if (childDevice->Present) {

            ObReferenceObject(childDevice->DeviceObject);
            *object++ = childDevice->DeviceObject;
        }
        childDevice = childDevice->Sibling;
    }

    PciReleasePassiveLock(&bus->ChildDeviceLock);

    TraceEvents(bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "  Total PDO count = 0x%x (0x%x already in list)",
                deviceRelations->Count + deviceCount,
                deviceRelations->Count);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = (ULONG_PTR)deviceRelations;

    //
    // The platform indicates to the system software that it can ignore
    // boot configs for a PCI device or bridge by defining a _DSM method
    // in the scope of the device/bridge. The operating system is required
    // to evaluate this method and clear the boot configs on the device.
    // In order to evaluate this method on Windows platform, there needs
    // to be an ACPI filter object in the stack. So block this Irp on its
    // way up since by then the ACPI driver got a chance to attach its
    // filter to the PDO and do the necessary processing.
    //

    status = PciCallDownIrpStack(DeviceExtension, Irp);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = PciScanBusCompletion(bus);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // In order to keep the bus in a high power state during boot, we keep
    // track of the first bus scan, keeping the bus powered until we find
    // devices (or not).  If we find devices, those devices will keep it
    // powered (until they themselves go into D3cold.)
    //

    if (bus->FirstBusScanDone == 0) {
        PciBus_PowerDereference(bus, PciBusPowerReasonBusPnpOperationPending);
        PCI_BUS_SET_FLAG(bus, FirstBusScanDone);
    }

exit:

    if (stateLockOwned != FALSE) {
        PCI_RELEASE_STATE_LOCK(DeviceExtension);
    }

    //
    // Allow this bus to drift back into a low-power state, if there are
    // no other reasons to keep it powered.
    //

    PciBus_PowerDereference(bus, PciBusPowerReasonBusPnpOperationPending);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS
PciBus_QueryDeviceState(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    //
    // If the requirements on the bus have changed, which might happen in
    // response to a hot-plugged device, then indicate to the PnP manager that
    // it should requery the resource requirements.
    //

    if (bus->RequirementsChanged == 1) {
        PCI_BUS_CLEAR_FLAG(bus, RequirementsChanged);
        Irp->IoStatus.Information |= PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciBus_QueryInterface(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
   NTSTATUS status;

   PAGED_CODE();






















   status = PciQueryInterface(
                DeviceExtension,
                IrpSp->Parameters.QueryInterface.InterfaceType,
                IrpSp->Parameters.QueryInterface.Size,
                IrpSp->Parameters.QueryInterface.Version,
                IrpSp->Parameters.QueryInterface.InterfaceSpecificData,
                IrpSp->Parameters.QueryInterface.Interface,
                FALSE,
                Irp->IoStatus.Status
                );

   if (NT_SUCCESS(status)) {

       Irp->IoStatus.Status = status;
       return PciPassIrp(DeviceExtension, Irp);

   } else if (status == STATUS_NOT_SUPPORTED) {

       //
       // Status == STATUS_NOT_SUPPORTED. Pass IRP down the stack
       // and see if anyone else is kind enough to provide this
       // interface.
       //
       status = PciCallDownIrpStack(DeviceExtension, Irp);

       if (status == STATUS_NOT_SUPPORTED) {

           //
           // If nobody provided the interface, try again at
           // this level.
           //
           status = PciQueryInterface(
               DeviceExtension,
               IrpSp->Parameters.QueryInterface.InterfaceType,
               IrpSp->Parameters.QueryInterface.Size,
               IrpSp->Parameters.QueryInterface.Version,
               IrpSp->Parameters.QueryInterface.InterfaceSpecificData,
               IrpSp->Parameters.QueryInterface.Interface,
               TRUE,  // last chance
               Irp->IoStatus.Status
               );
       }
   }

   if (status != STATUS_NOT_SUPPORTED) {

       Irp->IoStatus.Status = status;
   } else {

       status = Irp->IoStatus.Status;
   }

   IoCompleteRequest(Irp, IO_NO_INCREMENT);
   return status;
}

NTSTATUS
PciBus_QueryCapabilities(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PDEVICE_CAPABILITIES capabilities;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    capabilities = IrpSp->Parameters.DeviceCapabilities.Capabilities;

    //
    // Grab the S-state to D-State mapping and wake states
    //
    RtlCopyMemory(
        bus->SystemStateMapping,
        capabilities->DeviceState,
        sizeof(DEVICE_POWER_STATE) * PowerSystemMaximum
        );

    //
    // Make sure that we have sane S->D state mappings.
    //

    if (bus->SystemStateMapping[PowerSystemWorking] ==
        PowerDeviceUnspecified) {

        bus->SystemStateMapping[PowerSystemWorking] = PowerDeviceD0;
    }

    if (bus->SystemStateMapping[PowerSystemShutdown] ==
        PowerDeviceUnspecified) {

        bus->SystemStateMapping[PowerSystemShutdown] = PowerDeviceD3;
    }

    bus->SystemWake = capabilities->SystemWake;
    bus->DeviceWake = capabilities->DeviceWake;

    //
    // Make sure thunderbolt bridges do not show in the eject UI.
    //

    if (PCI_BUS_IS_THUNDERBOLT_BUS(bus)) {
        capabilities->Removable = FALSE;
    }

    DebugDumpDeviceCapabilities(PNP_TRACE_LEVEL, capabilities);

    return STATUS_SUCCESS;
}

NTSTATUS
PciBus_FilterResourceRequirements(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PIO_RESOURCE_REQUIREMENTS_LIST filteredRequirements;
    PIO_RESOURCE_REQUIREMENTS_LIST irpRequirements;
    NTSTATUS status;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    //
    // If the PDO didn't generate any requirements, there is nothing to filter.
    //
    irpRequirements = (PIO_RESOURCE_REQUIREMENTS_LIST)Irp->IoStatus.Information;
    if (irpRequirements == NULL) {
        return STATUS_SUCCESS;
    }

    if (PCI_ROOT_BUS(bus)) {

        //
        // The only adjument for root bus is to set the
        // CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE
        // on memory requirements.
        //
        status = PciRootFilterRequirementsList(bus, irpRequirements);
        return status;
    }

    status = PciBridgeFilterRequirementsList(bus->PciBridge,
                                             irpRequirements,
                                             &filteredRequirements
                                             );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (filteredRequirements != irpRequirements) {
        PciFreeExternalPool(irpRequirements);
        Irp->IoStatus.Information = (ULONG_PTR)filteredRequirements;
    }

    return status;
}

NTSTATUS
PciBus_DeviceUsageNotification(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
{
   PAGED_CODE();

   return PciLocalDeviceUsage(&DeviceExtension->DeviceUsage,
                              IrpSp->Parameters.UsageNotification.Type,
                              IrpSp->Parameters.UsageNotification.InPath
                              );
}

NTSTATUS
PciBus_QueryLegacyBusInformation(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    PLEGACY_BUS_INFORMATION information;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)DeviceExtension;

    information = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION,
                                  sizeof(LEGACY_BUS_INFORMATION)
                                  );
    if (information == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(&information->BusTypeGuid, &GUID_BUS_TYPE_PCI, sizeof(GUID));
    information->LegacyBusType = PCIBus;
    information->BusNumber = bus->SecondaryBusNumber;

    Irp->IoStatus.Information = (ULONG_PTR)information;

    return STATUS_SUCCESS;
}
