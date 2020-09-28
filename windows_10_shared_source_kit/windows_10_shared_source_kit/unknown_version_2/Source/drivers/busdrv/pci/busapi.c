/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    busapi.c

Abstract:

    This module contains functions that apply to busses.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "busapi.tmh"

// --------------------------------------------------------------------- Defines

//
// _HPP method for HotPlugParameters
//
//    Method (_HPP, 0) {
//        Return (Package(){
//            0x00000008,     // CacheLineSize in DWORDS
//            0x00000040,     // LatencyTimer in PCI clocks
//            0x00000001,     // Enable SERR (Boolean)
//            0x00000001      // Enable PERR (Boolean)
//         })
//
#define PCI_HPP_CACHE_LINE_SIZE_INDEX   0
#define PCI_HPP_LATENCY_TIMER_INDEX     1
#define PCI_HPP_ENABLE_SERR_INDEX       2
#define PCI_HPP_ENABLE_PERR_INDEX       3
#define PCI_HPP_PACKAGE_COUNT           4

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

VOID
PciAddBusToTree(
    __inout PPCI_BUS Bus,
    __inout_opt PPCI_BUS ParentBus
    );

NTSTATUS
PciEarlyRestoreExecuteRestore(
   PPCI_DEVICE Device
   );

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciEarlyRestoreDevice(
    PPCI_DEVICE Device,
    ULONG RestoreType,
    BOOLEAN RestorePoweredDevices
    );

NTSTATUS
PciEarlyRestoreBus(
    PPCI_DEVICE Device,
    ULONG RestoreType,
    BOOLEAN RestorePoweredDevices
    );

NTSTATUS
PciFindDeviceInTree(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PPCI_BUS *ParentBus,
    __out PPCI_SLOT_NUMBER Slot
    );

VOID
PciGetHotPlugParameters(
    __inout PPCI_BUS Bus
    );

NTSTATUS
PciBusGetSecondaryInterface (
    __inout PPCI_BUS Bus
    );

NTSTATUS
PciUpdateLegacyHardwareDescription(
    __in PPCI_BUS Bus
    );

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciDebugDevicePowerCallback (
    __in DEVICE_POWER_STATE PowerState,
    __in PVOID Context
    );

NTSTATUS
PciGetHPXParameters(
    __inout PPCI_BUS Bus
    );

VOID
PciGetHPPParameters(
    __inout PPCI_BUS Bus
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciAddDevice)
    #pragma alloc_text(PAGE, PciAddBusToTree)
    #pragma alloc_text(PAGE, PciEarlyRestoreAddListItem)
    #pragma alloc_text(PAGE, PciRemoveBusFromTree)
    #pragma alloc_text(PAGE, PciRootCountInTree)
    #pragma alloc_text(PAGE, PciFindDeviceInTree)
    #pragma alloc_text(PAGE, PciQueryDeviceUsageInTree)
    #pragma alloc_text(PAGE, PciGetHotPlugParameters)
    #pragma alloc_text(PAGE, PciBusGetSecondaryInterface)
    #pragma alloc_text(PAGE, PciUpdateLegacyHardwareDescription)
    #pragma alloc_text(PAGE, PciDebugDevicePowerCallbackRegister)
    #pragma alloc_text(PAGE, PciDebugDevicePowerCallbackDeregister)
#endif

// --------------------------------------------------------------------- Globals

extern PPCI_DEBUG_PORT PciDebugPorts;
extern ULONG PciDebugPortsCount;
LIST_ENTRY PciEarlyRestoreList;
PPCI_SEGMENT PciSegmentList;
KEVENT PciLegacyDescriptionLock;
volatile LONG PciCreateControlDeviceCount = 0;

// ------------------------------------------------------------------- Functions

NTSTATUS
PciAddDevice(
    __in PDRIVER_OBJECT DriverObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    Given a physical device object, this routine creates a functional
    device object for it.

Arguments:

    DriverObject - Pointer to our driver's DRIVER_OBJECT structure.

    PhysicalDeviceObject - Pointer to the physical device object for which
                           we must create a functional device object.

Return Value:

    NT status.

--*/
{
    BOOLEAN addedToTree;
    PPCI_BUS bus;
    BOOLEAN delayOnSxResume;
    PDEVICE_OBJECT deviceObject;
    HANDLE handle;
    IO_STACK_LOCATION irpStack;
    __volatile static volatile LONG logIteration;    
    LONG logIterationResult;
    PPCI_BUS parentBus;
    RECORDER_LOG_CREATE_PARAMS recorderParams;
    BOOLEAN runClear;    
    PCI_SLOT_NUMBER slot;
    NTSTATUS status;
    
    PAGED_CODE();

    addedToTree = FALSE;
    deviceObject = NULL;
    bus = NULL;

    status = IoCreateDevice(
                DriverObject,               // our driver object
                sizeof(PCI_BUS),            // size of our extension
                NULL,                       // our name
                FILE_DEVICE_BUS_EXTENDER,   // device type
                0,                          // device characteristics
                FALSE,                      // not exclusive
                &deviceObject               // store new device object here
                );
    if (!NT_SUCCESS(status)) {
        __assume(deviceObject == NULL);
        goto cleanup;
    }

    bus = (PPCI_BUS)deviceObject->DeviceExtension;

    //
    // Initialize some common fields
    //
    _No_competing_thread_begin_;
    RtlZeroMemory(bus, sizeof(*bus));
    bus->Signature = PCI_BUS_SIGNATURE;
    bus->DeviceObject = deviceObject;
    bus->PhysicalDeviceObject = PhysicalDeviceObject;
    bus->PowerState = PowerDeviceD0;
    bus->BusPowerState = PciBusPowerS0On;
    KeInitializeEvent(&bus->ChildDeviceLock, SynchronizationEvent, TRUE);
    KeInitializeEvent(&bus->ExpressLinkQuiescentEvent,
                      SynchronizationEvent,
                      TRUE);

    KeInitializeSpinLock(&bus->BusPowerLock);
    KeInitializeSpinLock(&bus->BusWakeLock);
    KeInitializeEvent(&bus->BusPowerEvent, NotificationEvent, FALSE);
    IoInitializeRemoveLock(&bus->RemoveLock, 'BicP', 0, 0);

    RECORDER_LOG_CREATE_PARAMS_INIT(&recorderParams, NULL);

#if DBG
    recorderParams.TotalBufferSize = 0x2000;
#else
    recorderParams.TotalBufferSize = 0x400;
#endif

    logIterationResult = InterlockedIncrement(&logIteration);
    RtlStringCbPrintfA(recorderParams.LogIdentifier,
                       RECORDER_LOG_IDENTIFIER_MAX_CHARS,
                       "FDO %x",
                       logIterationResult);

    status = WppRecorderLogCreate(&recorderParams, &bus->BlackBoxHandle);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    TraceEvents(bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "AddDevice to PDO %p",
                PhysicalDeviceObject);

    bus->AttachedDeviceObject = IoAttachDeviceToDeviceStack(
                                    deviceObject,
                                    PhysicalDeviceObject
                                    );

    if (bus->AttachedDeviceObject == NULL) {
        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_PNP,
                    "Failed to attach device to stack. PDO: %p",
                    PhysicalDeviceObject);
        status =  STATUS_NO_SUCH_DEVICE;
        goto cleanup;
    }

    PciInitializeState((PPCI_COMMON_EXTENSION)bus);

    //
    // Take a power reference on this bus, which will be dropped once it has
    // been scanned for the first time.  This will ensure that the bus doesn't
    // turn off during boot, before any devices have been discovered.
    //

    bus->BusPowerAvailable = 1;
    bus->BusPowerReferenceReason[PciBusPowerReasonBusPnpOperationPending] = 1;

    _No_competing_thread_end_;

#if defined(_AMD64_) || defined(_X86_)

    //
    // X86/64 platforms generally require additional settle time
    // on bridge power transitions before child devices become
    // accessible.  This delay is not necessary on ARM/ARM64.  
    // Child devices can opt into this delay by including 
    // the PciExtraSettleTimeRequired from machine.inf or pci.inf.
    // Devices cannot currently opt out of this delay.

    PCI_BUS_SET_FLAG(bus, SettleTimeRequired);

#endif

    status = PciGetBusHackFlags(bus, &bus->HackFlags);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Find out if the firmware is prepared for this bus to idle out in S0.
    // If _S0W is present, it will return a D-state, from 0 through 4, where
    // 4 means D3cold.  D0 means that wake just doesn't work, and that may
    // override the parent bus's choice.
    //

    bus->IdleDState = DeviceWakeDepthNotWakeable;

    if (!(PciSystemWideHackFlags & PCI_SYS_HACK_NOBODY_SUPPORTS_D3COLD)) {

        irpStack.MajorFunction = IRP_MJ_PNP;
        irpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;
        irpStack.Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_D3COLD_SUPPORT_INTERFACE;
        irpStack.Parameters.QueryInterface.Version = D3COLD_SUPPORT_INTERFACE_VERSION;
        irpStack.Parameters.QueryInterface.Size = sizeof(bus->D3coldIntrf);
        irpStack.Parameters.QueryInterface.Interface = (PINTERFACE)&bus->D3coldIntrf;
        irpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;

        status = PciSendPnpIrp(PhysicalDeviceObject, &irpStack, NULL);
        if (NT_SUCCESS(status)) {
            status = bus->D3coldIntrf.GetIdleWakeInfo(bus->D3coldIntrf.Context,
                                                      PowerSystemWorking,
                                                      &bus->IdleDState);
            if (!NT_SUCCESS(status)) {
                bus->IdleDState = DeviceWakeDepthNotWakeable;
            }
        }
    }

    //
    // The rest of the initialization is dependent on whether this
    // is a root bus or a bridged bus. Find out if the PDO was created
    // by the PCI driver.  If parentBus returns non-NULL, there is a
    // parent bus for this bus in the tree, so the bus is not root.
    //
    PciFindDeviceInTree(PhysicalDeviceObject, &parentBus, &slot);

    if (parentBus) {

        if (bus->IdleDState == DeviceWakeDepthNotWakeable) {
            if (parentBus->IdleDState > DeviceWakeDepthD0) {
                bus->IdleDState = DeviceWakeDepthD3hot;
            } else {
                bus->IdleDState = DeviceWakeDepthD0;
            }
        }

        bus->Root = parentBus->Root;
        PciAddBusToTree(bus, parentBus);
        addedToTree = TRUE;

        //
        // This is a bridged bus.  Allocate and initialize a PCI_BRIDGE structure for it.
        //
        status = PciProcessNewBridgedBus(bus, slot);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        PciBusGetSecondaryInterface(bus);

        TraceEvents(bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_PNP,
                    "AddDevice (new bus %p is child of bus 0x%x).",
                    bus,
                    parentBus->SecondaryBusNumber);

    } else {

        status = PciProcessNewRootBus(bus);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        PciGetRootBusNumbersFromBootConfig(bus->Root,
                                           &bus->SecondaryBusNumber,
                                           &bus->SubordinateBusNumber
                                           );

        //
        // The root bus's bridge map shows where bridges are located
        // in the bus number space.  If bit x is set, there is a bridge
        // whose secondary bus number is x.  Initialize it so that the bits
        // for all bus numbers not passed by this root are set as well,
        // indicating they're not available.  Note that the bitmap has space
        // for 257 entries, so that there's a buffer bit that will always
        // be set.
        //
        RtlInitializeBitMap(&bus->Root->BridgeMap,
                            bus->Root->BridgeMapBuffer,
                            0x101
                            );

        RtlClearAllBits(&bus->Root->BridgeMap);
        PciSetBitRangeInclusive(&bus->Root->BridgeMap,
                                0,
                                LOCAL_BUS_NUMBER(bus->SecondaryBusNumber)
                                );

        PciSetBitsToEnd(&bus->Root->BridgeMap,
                        LOCAL_BUS_NUMBER(bus->SubordinateBusNumber) + 1
                        );

        //
        // The segment bus number map shows which bus numbers are assigned to
        // PCI root buses. Ensure that the Secondary->Subordinate range on this
        // root bus is available in the segment bus number map.
        //
        runClear =
            RtlAreBitsClear(
                &bus->Root->Segment->BusNumberMap,
                LOCAL_BUS_NUMBER(bus->SecondaryBusNumber),
                bus->SubordinateBusNumber - bus->SecondaryBusNumber + 1
                );

        if (runClear == FALSE) {

            //
            // Check for the presence of a hack flag to ignore the overlapping
            // bus number ranges on pci root buses.
            //
            if (!(PciSystemWideHackFlags &
                  PCI_SYS_HACK_IGNORE_OVERLAPPING_ROOTBUS_RANGES)) {

                status = STATUS_UNSUCCESSFUL;
                goto cleanup;
            }
        }

        PciSetBitRangeInclusive(&bus->Root->Segment->BusNumberMap,
                                LOCAL_BUS_NUMBER(bus->SecondaryBusNumber),
                                LOCAL_BUS_NUMBER(bus->SubordinateBusNumber)
                                );

        //
        // Evaluate _DSM to determine if the OS needs a power-on reset
        // delay on Sx resume.  This call is only valid for root busses.
        //

        if (bus->Root->DsmForDelayOnSxResumeEvaluated == 0) {
            status = PciEvaluateDsmMethodForDelayOnSxResume (bus->DeviceObject,
                                                             &delayOnSxResume);

            if (NT_SUCCESS(status) && delayOnSxResume == FALSE) {
                PCI_ROOT_CLEAR_FLAG(bus->Root, DelayOnSxResume);
            } else {
                PCI_ROOT_SET_FLAG(bus->Root, DelayOnSxResume);
            }

            PCI_ROOT_SET_FLAG(bus->Root, DsmForDelayOnSxResumeEvaluated);
        }

        PciAddBusToTree(bus, NULL);
        addedToTree = TRUE;
    }

    //
    // Get any hotplug parameters
    //

    PciGetHotPlugParameters(bus);

    //
    // Initialize the wake state machine to its starting state.
    //
    _No_competing_thread_begin_;
    if (PCI_BUS_USES_NATIVE_PME(bus)) {
        bus->BusWakeState = PciExpressWakeDisarmedOn;
    } else {
        bus->BusWakeState = PciConvWakeDisarmed;
    }
    _No_competing_thread_end_;

    //
    // The bus's bus number map shows which bus numbers are assigned to
    // child bridges of this bus.  Every bus number in a child bridge's
    // secondary->subordinate range is set in this map. Initialize it
    // so that the bus numbers from 0 to the secondary bus number of the
    // bridge are set, indicating they're not available.
    //
    RtlInitializeBitMap(&bus->BusNumberMap, bus->BusNumberMapBuffer, 0x100);
    RtlClearAllBits(&bus->BusNumberMap);

    PciSetBitRangeInclusive(&bus->BusNumberMap,
                            0,
                            LOCAL_BUS_NUMBER(bus->SecondaryBusNumber));

    //
    // A reminder of the children devices that should not be enumerated.
    //

    InitializeListHead(&bus->ChildrenBusNumHints);

    //
    // Update the legacy hardware tree that would have been built by the ARC
    // firmware or NTDetect which no longer exists.
    // Since this routine builds subkeys for unpopulated buses up to the max
    // known bus number, restrict this to segment 0. Otherwise, with multiple
    // segments in a system, this routine creates 256 entries per segment up to
    // the max bus number on the highest segment. This will result in huge
    // perf hit during boot time. This restriction has its own implications.
    // The VGA driver invokes legacy APIs to walk thru the bus tree and stops
    // when it doesnt find the next bus. This means, that VGA bus scanning is
    // essentially restricted to buses on segment 0. Since this is not desirable,
    // this needs to be revisited.
    //
    if (SEGMENT_NUMBER_FOR_BUS(bus->SecondaryBusNumber) == 0) {
        status = PciUpdateLegacyHardwareDescription(bus);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    //
    // Initialize the map that we use to store the routing IDs that we evaluate
    // during a bus scan. There can be a maximum of 256 routing IDs for a bus.
    //
    RtlInitializeBitMap(&bus->EvaluatedRidMap,
                        bus->EvaluatedRidMapBuffer,
                        0x100
                        );

    //
    // Check to see if this bus was started with the Critical Device Database,
    // which is interesting when we try to figure out whether a child device was
    // disabled with group policy.
    //

    status = IoOpenDeviceRegistryKey(bus->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DRIVER,
                                     STANDARD_RIGHTS_ALL,
                                     &handle);

    if (NT_SUCCESS(status)) {
        ZwClose(handle);
    } else {
        bus->StartedWithoutInf = 1;
    }

    //
    // Create control device when 1st bus created.  Doing this in DriverEntry
    // would be more natural, but it would cause Driver Verifier to freak out
    // because this is a PnP driver.
    //

    if (InterlockedIncrement(&PciCreateControlDeviceCount) == 1) {
        PCI_ASSERT(PciControlDevice == NULL);
        status = PciCreateControlDevice();
        if (!NT_SUCCESS(status)) {

            //
            // Control device creation failure is fatal.
            //

            TraceEvents(WppRecorderLogGetDefault(),
                        TRACE_LEVEL_INFORMATION,
                        DBG_PNP,
                        "PciCreateControlDevice failed - %!STATUS!",
                        status);
            goto cleanup;
        }
    }

    //
    // We can receive IRPs now...
    //
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;

cleanup:

    PCI_ASSERT(!NT_SUCCESS(status));

    if (bus) {

        if (addedToTree) {
            PciRemoveBusFromTree(bus);
        }
        if (bus->AttachedDeviceObject) {
            IoDetachDevice(bus->AttachedDeviceObject);
        }
        if (bus->PciBridge) {
            if (bus->PciBridge->ExpressBridge != NULL) {
                ExpressDestroyVirtualBridge(bus->PciBridge->ExpressBridge);
                bus->PciBridge->ExpressBridge = NULL;
            }

            PciFreePool(bus->PciBridge);
            bus->PciBridge = NULL;
        }
        if (bus->Root && (bus->Root->Bus == bus)) {
            PciFreePool(bus->Root);
            bus->Root = NULL;
        }
    }

    if (deviceObject) {
        IoDeleteDevice(deviceObject);
    }

    return status;
}

VOID
PciAddBusToTree(
    __inout PPCI_BUS Bus,
    __inout_opt PPCI_BUS ParentBus
    )
/*++

Routine Description:

    This routine adds a bus extension to the bus extension tree.
    New busses are added to the end of the list of child busses
    of a given parent bus.

Arguments:

    Bus - The bus extension to add.

    ParentBus - The parent bus of this bus, or NULL if Bus is a
        root bus.

Return Value:

    VOID

--*/
{
    PPCI_BUS *listHead, *listTail;

    PAGED_CODE();

    PciAcquirePassiveLock(&PciGlobalLock);

    if (ParentBus) {

        listHead = &ParentBus->ChildBuses;
        listTail = &ParentBus->ChildBusTail;

    } else {

        listHead = &Bus->Root->Segment->PciRootBusList;
        listTail = &Bus->Root->Segment->PciRootBusTail;
    }

    Bus->ParentBus = ParentBus;
    Bus->SiblingBus = NULL;

    if (*listHead) {

        PCI_ASSERT(*listTail);

        (*listTail)->SiblingBus = Bus;
        *listTail = Bus;

    } else {

        *listHead = *listTail = Bus;
    }

    PciReleasePassiveLock(&PciGlobalLock);
    return;
}

VOID
PciRemoveBusFromTree (
    __in PPCI_BUS Bus
    )
/*++

Routine Description:

    This routine removes a bus extension from the bus extension tree.

Arguments:

    Bus - The bus extension to remove.

Return Value:

    None

--*/
{
    PPCI_BUS *listHead, *listTail;
    PPCI_BUS current, previous;
    BOOLEAN found;

    PAGED_CODE();

    found = FALSE;
    PciAcquirePassiveLock(&PciGlobalLock);
    if (Bus->ParentBus) {
        listHead = &Bus->ParentBus->ChildBuses;
        listTail = &Bus->ParentBus->ChildBusTail;

    } else {
        listHead = &Bus->Root->Segment->PciRootBusList;
        listTail = &Bus->Root->Segment->PciRootBusTail;
    }

    PCI_ASSERT(*listHead);

    if (*listHead == Bus) {
        found = TRUE;
        *listHead = Bus->SiblingBus;
        if (*listTail == Bus) {

            PCI_ASSERT(*listHead == NULL);

            *listTail = NULL;
        }

    } else {
        current = *listHead;
        previous = NULL;
        while (current) {
            if (current == Bus) {
                found = TRUE;

                PCI_ASSERT(previous != NULL);

                previous->SiblingBus = Bus->SiblingBus;
                if (*listTail == Bus) {
                    *listTail = previous;
                }

                break;
            }

            previous = current;
            current = current->SiblingBus;
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);

    //
    // We didn't find the bus in the tree!
    //

    PCI_ASSERT(found);
    UNREFERENCED_PARAMETER(found);

    return;
}

PPCI_BUS
PciNextBusInSegment(
    __in PPCI_BUS Bus
    )
/*++

Routine Description:

    This routine iterates through the bus tree in the same segment,
    returning the next bus in the interation after the given bus.

Arguments:

    Bus - The bus extension to find the next bus after.

Return Value:

    The next bus in the segment, or NULL if all busses in the
    segment have been iterated through.

--*/
{
    PPCI_BUS current;

    NON_PAGED_CODE();

    if (Bus->ChildBuses) {

        //
        // First process down the tree
        //
        return Bus->ChildBuses;

    } else if (Bus->SiblingBus) {

        //
        // If not possible, then process sideways
        //
        return Bus->SiblingBus;

    } else {

        //
        // If not possible, go up the tree until either
        // processing sideways is possible, or we reach
        // the root.
        //
        current = Bus->ParentBus;

        while (current) {

            if (current->SiblingBus) {

                current = current->SiblingBus;
                break;
            }
            current = current->ParentBus;
        }

        return current;
    }
}

NTSTATUS
PciFindDeviceInTree(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PPCI_BUS *ParentBus,
    __out PPCI_SLOT_NUMBER Slot
    )
/*++

Routine Description:

    For each bus in the tree, search the child lists for the supplied
    PhysicalDeviceObject.

Arguments:

    PhysicalDeviceObject - Pdo to find.

    ParentBus - Buffer to be filled in with a pointer to the parent
        bus of the PDO, if it is found.

    Slot - Buffer to be filled in with the slot number the PDO
        sits in on the parent bus.

Return Value:

    NT Status code.

--*/
{
    PPCI_BUS bus;
    PPCI_DEVICE currentDevice;
    PPCI_SEGMENT segment;
    NTSTATUS status;

    PAGED_CODE();

    *ParentBus = NULL;
    Slot->u.AsULONG = 0;
    status = STATUS_UNSUCCESSFUL;

    PciAcquirePassiveLock(&PciGlobalLock);

    for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
        bus = segment->PciRootBusList;
        while (bus) {
            PciAcquirePassiveLockInCriticalSection(&bus->ChildDeviceLock);
            currentDevice = bus->ChildDevices;
            while (currentDevice) {
                if (currentDevice->DeviceObject == PhysicalDeviceObject) {
                    *ParentBus = bus;
                    *Slot = currentDevice->Slot;
                    status = STATUS_SUCCESS;
                    break;
                }

                currentDevice = currentDevice->Sibling;
            }

            PciReleasePassiveLockInCriticalSection(&bus->ChildDeviceLock);

            if (status == STATUS_SUCCESS) {
                break;
            }

            bus = PciNextBusInSegment(bus);
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);
    return status;
}


NTSTATUS
PciQueryDeviceUsageInTree(
    __in ULONG BusNumber,
    __in ULONG DeviceNumber,
    __in ULONG FunctionNumber,
    __out PPCI_QUERY_DEVICE_USAGE_OUTPUT DeviceUsageOutput
    )
/*++

Routine Description:

    Search pci device tree by location. Then fill in the usage.

Arguments:

    PciLocation - Pci device loaction.

    DeviceUsage - Buffer to be filled in with usage, if it is found.

Return Value:

    NT Status code.

--*/
{
    PPCI_BUS bus;
    PPCI_DEVICE currentDevice;
    PPCI_SEGMENT segment;
    NTSTATUS status;

    PAGED_CODE();

    DeviceUsageOutput->AsULONG = 0;
    status = STATUS_NOT_FOUND;

    PciAcquirePassiveLock(&PciGlobalLock);

    for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
        bus = segment->PciRootBusList;
        while (bus) {
            PciAcquirePassiveLockInCriticalSection(&bus->ChildDeviceLock);
            currentDevice = bus->ChildDevices;
            while (currentDevice) {
                if (currentDevice->BusNumber == BusNumber &&
                    currentDevice->Slot.u.bits.DeviceNumber == DeviceNumber &&
                    currentDevice->Slot.u.bits.FunctionNumber ==
                    FunctionNumber) {

                    DeviceUsageOutput->bits.Paging =
                        currentDevice->DeviceUsage.Paging;
                    DeviceUsageOutput->bits.Hibernate =
                        currentDevice->DeviceUsage.Hibernate;
                    DeviceUsageOutput->bits.CrashDump =
                        currentDevice->DeviceUsage.CrashDump;
                    DeviceUsageOutput->bits.OnDebugPath =
                        (currentDevice->OnDebugPath? 1 : 0);
                    status = STATUS_SUCCESS;
                    break;
                }

                currentDevice = currentDevice->Sibling;
            }

            PciReleasePassiveLockInCriticalSection(&bus->ChildDeviceLock);

            if (status == STATUS_SUCCESS) {
                break;
            }

            bus = PciNextBusInSegment(bus);
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);
    return status;
}


ULONG
PciRootCountInTree()
/*++

Routine Description:

    This routine returns the root bus count

Arguments:

Return Value:

    Root bus count.

--*/
{
    PPCI_BUS rootBus;
    ULONG rootCount;
    PPCI_SEGMENT segment;

    PAGED_CODE();

    rootCount = 0;

    PciAcquirePassiveLock(&PciGlobalLock);

    for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
        rootBus = segment->PciRootBusList;
        while (rootBus) {
            rootCount++;
            rootBus = rootBus->SiblingBus;
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);

    return rootCount;
}

VOID
PciGetHotPlugParameters(
    __inout PPCI_BUS Bus
    )
{
    NTSTATUS status;

    //
    // The _HPX object supersedes the _HPP object.  Evaluate _HPX first,
    // and only evaluate _HPP as a fallback.
    //

    status = PciGetHPXParameters(Bus);
    if (!NT_SUCCESS(status)) {
        PciGetHPPParameters(Bus);
    }

    return;
}

NTSTATUS
PciGetHPXParameters(
    __inout PPCI_BUS Bus
    )
/*++

Description:

    Runs _HPX on the device and saves the parameters if available.

    Method (_HPX, 0) {
        Return (
            Package(3){
                Package(6){ // PCI Setting Record
                0x00, // Type 0
                0x01, // Revision 1
                0x08, // CacheLineSize in DWORDS
                0x40, // LatencyTimer in PCI clocks
                0x01, // Enable SERR (Boolean)
                0x00  // Enable PERR (Boolean)
                },
                Package(5){ // PCI-X Setting Record
                0x01, // Type 1
                0x01, // Revision 1
                0x03, // Maximum Memory Read Byte Count
                0x04, // Average Maximum Outstanding Split Transactions
                0x07  // Total Maximum Outstanding Split Transactions
                },
                Package(18) { // PCIe Setting Record
                0x2,          // Type 2  
                0x1,          // Revision
                0xfc000fcf,   // Uncorrectable Error Mask Register AND Mask
                0x3a18000,    // Uncorrectable Error Mask Register OR Mask
                0xfc000fcf,   // Uncorrectable Error Severity AND Mask
                0x4e7030,     // Uncorrectable Error Severity OR Mask
                0xffff0e3e,   // Correctable Error Mask Register AND Mask
                0xf1c1,       // Correctable Error Mask Register OR Mask
                0xfffffebf,   // Advanced Error Capabilities and Control Register AND Mask
                0x140,        // Advanced Error Capabilities and Control Register OR Mask
                0xfff1,       // Device Control Register AND Mask
                0xe,          // Device Control Register OR Mask
                0xffff,       // Link Control Register AND Mask
                0x0,          // Link Control Register OR Mask
                0xffffc010,   // Secondary Uncorrectable Error Severity Register AND Mask
                0x1bc0,       // Secondary Uncorrectable Error Severity Register OR Mask
                0xffffc010,   // Secondary Uncorrectable Error Mask Register AND Mask
                0x242f        // Secondary Uncorrectable Error Mask Register OR Mask
                }
            }
        )
    }

Arguments:

    Bus - The bus to get the _HPX from.

Return Value:

    NTSTATUS

--*/
{

#define PCI_HPX_TYPE0_PACKAGE_COUNT 6
#define PCI_HPX_TYPE1_PACKAGE_COUNT 5
#define PCI_HPX_TYPE2_PACKAGE_COUNT 18
#define PCI_HPX_REVISION 1

    ULONG buffer[PCI_HPX_TYPE2_PACKAGE_COUNT];
    ULONG bufferIndex;
    PACPI_METHOD_ARGUMENT innerArg;
    PUCHAR innerEnd;
    ACPI_EVAL_INPUT_BUFFER input;
    PACPI_METHOD_ARGUMENT outerArg;
    ULONG outerIndex;
    PACPI_EVAL_OUTPUT_BUFFER output;
    ULONG outputSize;
    NTSTATUS status;
    ACPI_EVAL_OUTPUT_BUFFER tmpOut;

    PAGED_CODE();

    RtlZeroMemory(&input, sizeof(ACPI_EVAL_INPUT_BUFFER));
    RtlZeroMemory(&tmpOut, sizeof(ACPI_EVAL_OUTPUT_BUFFER));
    output = NULL;

    //
    // Send a IOCTL to ACPI to request it to run the _HPX method on this device
    // if the method is present.  The first invocation is used to properly size
    // the real output buffer.
    //

    input.Signature = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
    input.MethodNameAsUlong = (ULONG)'XPH_';
    status = PciSendIoctl(Bus->PhysicalDeviceObject,
                          IOCTL_ACPI_EVAL_METHOD,
                          &input,
                          sizeof(ACPI_EVAL_INPUT_BUFFER),
                          &tmpOut,
                          sizeof(ACPI_EVAL_OUTPUT_BUFFER)
                          );

    if (status == STATUS_BUFFER_OVERFLOW) {

        //
        // Allocate a properly sized output buffer and resend
        // the Ioctl.
        //

        outputSize = tmpOut.Length;
        output = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, outputSize);
        if (output == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        RtlZeroMemory(output, outputSize);
        status = PciSendIoctl(Bus->PhysicalDeviceObject,
                              IOCTL_ACPI_EVAL_METHOD,
                              &input,
                              sizeof(ACPI_EVAL_INPUT_BUFFER),
                              output,
                              outputSize
                              );
    }

    if (!NT_SUCCESS(status)) {

        //
        // Inherit settings from my parent if they exist.
        //

        if ((Bus->ParentBus != NULL) &&
            (Bus->ParentBus->AcquiredHPX)) {

            RtlCopyMemory(&Bus->_HPX,
                          &Bus->ParentBus->_HPX,
                          sizeof(Bus->_HPX)
                          );

            PCI_BUS_SET_FLAG(Bus, AcquiredHPX);
        }

    } else {

        //
        // Verify the top level output is a buffer and
        // that it contains 3 or less packagss.
        //

        if (output->Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) {
            status = STATUS_ACPI_INVALID_ARGTYPE;
            goto exit;
        }

        if (output->Count > 3) {
            status = STATUS_ACPI_INVALID_ARGTYPE;
            goto exit;
        }

        outerArg = output->Argument;
        for (outerIndex = 0; outerIndex < output->Count; outerIndex++) {

            //
            // Verify that each entry in the buffer is a package
            //

            if (outerArg->Type != ACPI_METHOD_ARGUMENT_PACKAGE) {
                status = STATUS_ACPI_INVALID_ARGTYPE;
                goto exit;           
            }

            RtlZeroMemory(buffer, sizeof(buffer));
            bufferIndex = 0;
            innerArg = (PACPI_METHOD_ARGUMENT)outerArg->Data;
            innerEnd = Add2Ptr(innerArg, outerArg->DataLength);
            while (((PUCHAR)innerArg < innerEnd) && 
                   (bufferIndex < PCI_HPX_TYPE2_PACKAGE_COUNT)) {

                //
                // Each package is an array of integers.
                //

                if (innerArg->Type != ACPI_METHOD_ARGUMENT_INTEGER) {
                    status = STATUS_ACPI_INVALID_ARGTYPE;
                    goto exit;
                }

                buffer[bufferIndex] = innerArg->Argument;
                bufferIndex++;
                innerArg = ACPI_METHOD_NEXT_ARGUMENT(innerArg);
            }

            //
            // Validate that the package data is recognizable.  buffer[0] 
            // contains the type and buffer[1] contains the revision.  The
            // remaining buffer contains the pci-type specific information.
            //

            if (buffer[1] == PCI_HPX_REVISION) {
                switch (buffer[0]) {
                case 0:
                    if (bufferIndex == PCI_HPX_TYPE0_PACKAGE_COUNT) {
                        Bus->_HPX.type0.CacheLineSize = (UCHAR)buffer[2];
                        Bus->_HPX.type0.LatencyTimer = (UCHAR)buffer[3];
                        Bus->_HPX.type0.EnableSERR = (BOOLEAN)buffer[4];
                        Bus->_HPX.type0.EnablePERR = (BOOLEAN)buffer[5];
                        Bus->_HPX.ValidTypes |= HPX_TYPE_0_VALID;
                    }
                    break;

                case 1:
                    if (bufferIndex == PCI_HPX_TYPE1_PACKAGE_COUNT) {

                        //
                        // Intentionally not supporting type1 
                        // until telemetry shows this case exists
                        // on customer scenarios.  Setting the VALID 
                        // flag simply for telemetry purposes.
                        //

                        Bus->_HPX.ValidTypes |= HPX_TYPE_1_VALID;
                    }
                    break;

                case 2:
                    if (bufferIndex == PCI_HPX_TYPE2_PACKAGE_COUNT) {
                        RtlCopyMemory(&(Bus->_HPX.type2),
                                      &(buffer[2]),
                                      sizeof(ULONG) * (bufferIndex-2));

                        Bus->_HPX.ValidTypes |= HPX_TYPE_2_VALID;
                        PCI_BUS_SET_FLAG(Bus, AcquiredHPX);
                    }
                    break;

                default:
                    break;
                }
            }

            outerArg = ACPI_METHOD_NEXT_ARGUMENT(outerArg);
        }
    }

exit:

    TraceLoggingWrite(
        PciTraceLoggingProvider, 
        "PciHPXSupported",
        TraceLoggingString((Bus->_HPX.ValidTypes & HPX_TYPE_0_VALID)?
                           "Yes": "No", "HPX Type0"),

        TraceLoggingString((Bus->_HPX.ValidTypes & HPX_TYPE_1_VALID)?
                           "Yes": "No", "HPX Type1"),

        TraceLoggingString((Bus->_HPX.ValidTypes & HPX_TYPE_2_VALID)?
                           "Yes": "No", "HPX Type2"),

        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );

    if (output) {
        PciFreePool(output);
    }

    return status;
}

VOID
PciGetHPPParameters(
    __inout PPCI_BUS Bus
    )
/*++

Description:

    Runs the _HPP (described below) on the device and saves the parameters if available

    Method (_HPP, 0) {
        Return (Package(){
            0x00000008,     // CacheLineSize in DWORDS
            0x00000040,     // LatencyTimer in PCI clocks
            0x00000001,     // Enable SERR (Boolean)
            0x00000001      // Enable PERR (Boolean)
            })

Arguments:

    Bus - The bus to get the _HPP of.

Return Value:

    VOID

--*/
{
    PACPI_EVAL_OUTPUT_BUFFER output;
    ACPI_EVAL_INPUT_BUFFER input;
    ULONG outputSize;
    NTSTATUS status;
    ULONG current;
    ULONG count;

    PAGED_CODE();

    outputSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER)
                 + (sizeof(ACPI_METHOD_ARGUMENT) * (PCI_HPP_PACKAGE_COUNT - 1));

    output = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, outputSize);
    if (!output) {
        goto exit;
    }

    RtlZeroMemory(&input, sizeof(ACPI_EVAL_INPUT_BUFFER));
    RtlZeroMemory(output, outputSize);

    //
    // Send a IOCTL to ACPI to request it to run the _HPP method on this device
    // if the method it is present
    //
    input.Signature = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
    input.MethodNameAsUlong = (ULONG)'PPH_';

    status = PciSendIoctl(Bus->PhysicalDeviceObject,
                          IOCTL_ACPI_EVAL_METHOD,
                          &input,
                          sizeof(ACPI_EVAL_INPUT_BUFFER),
                          output,
                          outputSize
                          );
    if (!NT_SUCCESS(status)) {

        //
        // Inherit them from my parent (If I have one)
        //
        if ((Bus->ParentBus != NULL) &&
            (Bus->ParentBus->AcquiredHPP == TRUE)) {

            RtlCopyMemory(&Bus->_HPP,
                          &Bus->ParentBus->_HPP,
                          sizeof(Bus->_HPP)
                          );

            PCI_BUS_SET_FLAG(Bus, AcquiredHPP);
        }

    } else {

        if (output->Count != PCI_HPP_PACKAGE_COUNT) {
            goto exit;
        }

        //
        // Check they are all integers and in the right bounds
        //
        for (count = 0; count < PCI_HPP_PACKAGE_COUNT; count++) {

            if (output->Argument[count].Type != ACPI_METHOD_ARGUMENT_INTEGER) {
                goto exit;
            }

            current = output->Argument[count].Argument;
            switch (count) {
                case PCI_HPP_CACHE_LINE_SIZE_INDEX:
                case PCI_HPP_LATENCY_TIMER_INDEX:
                    //
                    // These registers are only a UCHAR in length
                    //
                    if (current > 0xFF) {
                        goto exit;
                    }
                    break;
                case PCI_HPP_ENABLE_SERR_INDEX:
                case PCI_HPP_ENABLE_PERR_INDEX:
                    //
                    // These are booleans - 1 or 0
                    //
                    if (current > 1) {
                        goto exit;
                    }
                    break;
            }
        }

        //
        // Finally save them and remember we got them.
        //
        Bus->_HPP.CacheLineSize = (UCHAR)output->Argument[PCI_HPP_CACHE_LINE_SIZE_INDEX].Argument;
        Bus->_HPP.LatencyTimer = (UCHAR)output->Argument[PCI_HPP_LATENCY_TIMER_INDEX].Argument;
        Bus->_HPP.EnableSERR = (BOOLEAN)output->Argument[PCI_HPP_ENABLE_SERR_INDEX].Argument;
        Bus->_HPP.EnablePERR = (BOOLEAN)output->Argument[PCI_HPP_ENABLE_PERR_INDEX].Argument;
        PCI_BUS_SET_FLAG(Bus, AcquiredHPP);
    }

exit:

    if (output) {
        PciFreePool(output);
    }
}

NTSTATUS
PciBusGetSecondaryInterface (
    __inout PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine derives the physical/electrical characteristics of the
    secondary interface for a PCI-PCI bridge.

Arguments:

    Bus - Supplies a PCI bus whose secondary interface is to be determined.
        This bus represents the PCI-PCI bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_DEVICE_TYPE BridgeType;
    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilitiesRegister;
    USHORT mode;
    PCIX_BRIDGE_CAPABILITY pciXBridgeCapability;
    ULONG statusOffset;

    PAGED_CODE();

    PCI_ASSERT(Bus->PciBridge != NULL);

    //
    // Start with an assumption that the secondary interface
    // is a conventional PCI bus.
    //

    Bus->SecondaryInterface = PciConventional;

    //
    // Bridges with Express capabilities expose Express on their secondary
    // sides, barring two exceptions. One is the Express to Pci-X bridges
    // that have Pci-X on their secondary sides. These bridges are handled
    // with other Pci-X bridges below. The other exception is for Pci/Pci-X
    // to Express bridges. Since these bridges do not have any specifications
    // for them, they are treated as conventional Pci bridges.
    //

    if (Bus->PciBridge->ExpressCapability != 0) {
        PciReadExpressBridgeRegisterEx(
            Bus->PciBridge,
            ExpressCapabilities,
            &ExpressCapabilitiesRegister);

        BridgeType = ExpressCapabilitiesRegister.DeviceType;
        if ((BridgeType != PciExpressToPciXBridge) &&
            (BridgeType != PciXToExpressBridge)) {

            PCI_ASSERT((BridgeType == PciExpressRootPort) ||
                       (BridgeType == PciExpressUpstreamSwitchPort) ||
                       (BridgeType == PciExpressDownstreamSwitchPort));

            Bus->SecondaryInterface = PciExpress;
            return STATUS_SUCCESS;
        }
    }

    //
    // Pci-X bridges have Pci-X capabilities. Bridges from Pci-X to Express
    // also have Pci-X capabilities, but they are treated as conventional Pci
    // bridges.
    //

    if (Bus->PciBridge->PciXCapability) {
        statusOffset = FIELD_OFFSET(PCIX_BRIDGE_CAPABILITY, SecondaryStatus);
        PciReadBridgeConfig(Bus->PciBridge,
                            (PUCHAR)&pciXBridgeCapability + statusOffset,
                            Bus->PciBridge->PciXCapability + statusOffset,
                            sizeof(USHORT));

        //
        // The secondary interface of a PCI-X bridge depends on its current
        // operating mode. PCI-X bridges can operate in conventional PCI,
        // PCI-X mode 1 or PCI-X mode 2, and the operational mode can depend
        // on the currently plugged-in cards on the secondary bus.
        //

        mode = pciXBridgeCapability.SecondaryStatus.BusModeFrequency;
        if (mode == PCIX_MODE_CONVENTIONAL_PCI) {
            Bus->SecondaryInterface = PciConventional;

        } else if (mode < PCIX_MODE2_266_66MHZ) {
            Bus->SecondaryInterface = PciXMode1;

        } else {
            Bus->SecondaryInterface = PciXMode2;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciUpdateLegacyHardwareDescription(
    __in PPCI_BUS Bus
    )
/*++

Routine Description:

    On Win64, update the legacy hardware tree that would have been built
    by the ARC firmware or NTDetect which don't exist. This is primarily
    used to specify which legacy bus numbers exist.

Arguments:

    Bus - The bus extension to update the legacy hardware description with.

Return Value:

    NT Status code.

--*/
{
    HANDLE multifunctionHandle, indexHandle;
    UNICODE_STRING valueString, pciString;
    CM_FULL_RESOURCE_DESCRIPTOR descriptor;
    PCM_FULL_RESOURCE_DESCRIPTOR full;
    CONFIGURATION_COMPONENT component;
    OBJECT_ATTRIBUTES attributes;
    WCHAR indexStringBuffer[10];
    UNICODE_STRING indexString;
    UNICODE_STRING tempString;
    NTSTATUS status;
    BOOLEAN createdNewKey;
    PUCHAR valueBuffer;
    ULONG valueLength;
    ULONG disposition;
    ULONG pciBusCount;
    ULONG keyIndex;
    ULONG index;

    PAGED_CODE();

    multifunctionHandle = NULL;
    indexHandle = NULL;
    createdNewKey = FALSE;

    //
    // This routine is called from the context of a PCI FDO AddDevice routine.
    // As a result, while this routine isn't called frequently it can run
    // simultaneously in the event that multiple FDOs are being added at the
    // same time.  To avoid races, just surround the entire routine with a
    // PASSIVE_LEVEL lock.
    //
    // N.B. Since AddDevice is called in the context of a system thread, we
    //      don't need to worry about deadlocks due to thread suspension and
    //      consequently don't need to disable APCs.
    //
    KeWaitForSingleObject(&PciLegacyDescriptionLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    if (!PciOpenKey(L"\\REGISTRY\\MACHINE\\HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter",
                    NULL,
                    KEY_READ | KEY_WRITE,
                    &multifunctionHandle,
                    &status)) {
        goto exit;
    }

    //
    // HKLM\Hardware\Description\System\MultifunctionAdapter is structured as
    // a set of 0 base consecutive numbered keys.  In theory, each key
    // corresponds to a single bus, with the InterfaceType and BusNumber given
    // by the contents of the "Configuration Data" value.
    //
    // Unfortunately, there are many places in the system that assume the bus
    // number is derived a different way.  Consider building an array
    // containing the different MultifunctionAdapter subkeys that correspond
    // to a given bus type.  The alternate interpretation is to refer to the
    // index in this array corresponding to a given bus as its "bus number."
    // In other words, the <N>th PCI bus described under the
    // MultifunctionAdapter key is assumed to describe bus <N>.
    //
    // This causes trouble on systems with ranges of unpopulated buses.  For
    // example, on a machine where the first PCI bus is bus 0x20, its hardware
    // description will be interpreted as either bus 0x20 or bus 0x0.  These
    // alternate interpretations have grown out of some code in Cm that builds
    // "Configuration Data" entries this way.  The problems this causes have
    // been masked by the fact that x86 and amd64 fill in a
    // MultifunctionAdapter entry for every bus up to the maximum bus number
    // reported by the PCI BIOS.  As a result, the possible interpretations
    // will only differ in the ia64 case.
    //
    // To preserve compatibility with existing code, we'll populate
    // MultifunctionAdapter in a manner that ensures that both bus number
    // interpretations will be consistent.
    //

    //
    // Start by walking through the MultifunctionAdapter key and finding both
    // the number of subkeys that refer to PCI buses along with the smallest
    // unpopulated subkey number.
    //

Retry:

    RtlInitEmptyUnicodeString(&indexString,
                              indexStringBuffer,
                              sizeof(indexStringBuffer)
                              );

    pciBusCount = 0;

    for (index = 0;;index++) {

        //
        // Open the next subkey under MultifunctionAdapter.
        //
        status = RtlIntegerToUnicodeString(index, 10, &indexString);
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        InitializeObjectAttributes(&attributes,
                                   &indexString,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   multifunctionHandle,
                                   NULL
                                   );

        status = ZwCreateKey(&indexHandle,
                             KEY_READ | KEY_WRITE,
                             &attributes,
                             0,
                             NULL,
                             REG_OPTION_VOLATILE,
                             &disposition
                             );
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        //
        // As the keys are all consecutive then if we created this key we have
        // enumerated them all and we can get on with registering out data
        //
        if (disposition == REG_CREATED_NEW_KEY) {
            createdNewKey = TRUE;
            break;
        }

        //
        // We have a subkey.  Extract its Identifier value to see if this
        // entry describes a PCI bus.
        //
        valueBuffer = NULL;
        valueLength = 0;
        status = PciGetRegistryValue(L"Identifier",
                                     NULL,
                                     indexHandle,
                                     REG_SZ,
                                     &valueBuffer,
                                     &valueLength
                                     );

        if (NT_SUCCESS(status)) {

            //
            // Build counted strings of the REG_SZ which we assume has been
            // NULL terminated and then compare knowing we can't overrun
            // as everything is counted.  If the string is longer than MAXUSHORT
            // we truncate it.
            //
            PciConstStringToUnicodeString(&pciString, L"PCI");

            valueString.Buffer = (PWSTR)valueBuffer;
            valueString.MaximumLength = (USHORT)valueLength;
            valueString.Length = valueString.MaximumLength - sizeof(UNICODE_NULL);

            if (RtlEqualUnicodeString(&pciString, &valueString, FALSE)) {

                //
                // This is a PCI bus, now check if it's our bus number.  If it
                // isn't, increment the count of detected PCI buses and
                // continue.
                //
                PciFreePool(valueBuffer);
                valueBuffer = NULL;
                valueLength = 0;

                status = PciGetRegistryValue(L"Configuration Data",
                                             NULL,
                                             indexHandle,
                                             REG_FULL_RESOURCE_DESCRIPTOR,
                                             &valueBuffer,
                                             &valueLength
                                             );
                if (NT_SUCCESS(status)) {

                    full = (PCM_FULL_RESOURCE_DESCRIPTOR)valueBuffer;

                    PCI_ASSERT(full->InterfaceType == PCIBus);
                    PCI_ASSERT(full->BusNumber == pciBusCount);

                    //
                    // If we've already reported this bus then we don't
                    // need to do anything, otherwise update our running
                    // counter of detected PCI buses and continue.
                    //
                    if (full->BusNumber == Bus->SecondaryBusNumber) {

                        ZwClose(indexHandle);
                        PciFreePool(valueBuffer);
                        status = STATUS_SUCCESS;
                        goto exit;
                    } else {
                        pciBusCount += 1;
                    }
                }
            }

            PciFreePool(valueBuffer);
        }

        ZwClose(indexHandle);
    }

    //
    // If we created a new key then this bus is not currently described in the
    // registry.  In order to ensure equivalence between different
    // interpretations of legacy bus numbers (as described in the comment at
    // the top of this routine), we need to ensure that an entry added to
    // describe bus <N> is also the <N>th PCI bus described under the
    // MultifunctionAdapter key.
    //
    PCI_ASSERT(createdNewKey);
    UNREFERENCED_PARAMETER(createdNewKey);

    ZwDeleteKey(indexHandle);
    ZwClose(indexHandle);
    keyIndex = index;

    RtlZeroMemory(&descriptor, sizeof(CM_FULL_RESOURCE_DESCRIPTOR));
    RtlZeroMemory(&component, sizeof(CONFIGURATION_COMPONENT));

    for (index = pciBusCount; index <= Bus->SecondaryBusNumber; index++) {

        //
        // Create the next subkey under MultifunctionAdapter.
        //
        status = RtlIntegerToUnicodeString(keyIndex, 10, &indexString);
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        InitializeObjectAttributes(&attributes,
                                   &indexString,
                                   OBJ_CASE_INSENSITIVE  | OBJ_KERNEL_HANDLE,
                                   multifunctionHandle,
                                   NULL
                                   );

        status = ZwCreateKey(&indexHandle,
                             KEY_READ | KEY_WRITE,
                             &attributes,
                             0,
                             NULL,
                             REG_OPTION_VOLATILE,
                             &disposition
                             );
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        //
        // If our creation disposition indicates that we opened an
        // existing key, then either 1) the contents of the
        // MultifunctionAdapter key are malformed or 2) we've raced with
        // some other component (i.e. not PCI) that is updating the
        // hardware description map at the same time.
        //
        // In either case, retrying this operation from the top has a
        // fairly good chance of filling in the PCI entries under
        // MultifunctionAdapter correctly whereas simply erroring out
        // would leave the hardware description map inaccurately
        // populated.
        //
        // N.B. When retrying the loop at the top of this routine will
        //      walk through all of the subkeys that we've already created
        //      on this pass.  In other words, the structure of the top
        //      loop ensures that this bottom loop will restart at the
        //      point where it left off.
        //
        if (disposition != REG_CREATED_NEW_KEY) {

            PCI_ASSERT(disposition == REG_OPENED_EXISTING_KEY);

            ZwClose(indexHandle);
            goto Retry;
        }

        //
        // Fill in the Identifier entry.  All of the new entries will be
        // for PCI buses.
        //
        PciConstStringToUnicodeString(&tempString, L"Identifier");

        status = ZwSetValueKey(indexHandle,
                               &tempString,
                               0,
                               REG_SZ,
                               L"PCI",
                               sizeof(L"PCI")
                               );

        if (!NT_SUCCESS(status)) {
            ZwDeleteKey(indexHandle);
            ZwClose(indexHandle);
            goto exit;
        }

        //
        // Fill in the Configuration Data entry.
        //
        // N.B. The complete descriptor is not written to the registry
        //      just enough data to indicate that this is an empty list
        //      (the first 16 bytes).  This is a bit gross but it is what
        //      happens on x86 machines today and after all we're only
        //      doing this for backward compatibility.
        //
        descriptor.InterfaceType = PCIBus;
        descriptor.BusNumber = index;

        PciConstStringToUnicodeString(&tempString, L"Configuration Data");

        status = ZwSetValueKey(indexHandle,
                               &tempString,
                               0,
                               REG_FULL_RESOURCE_DESCRIPTOR,
                               &descriptor,
                               16
                               );

        if (!NT_SUCCESS(status)) {
            ZwDeleteKey(indexHandle);
            ZwClose(indexHandle);
            goto exit;
        }

        //
        // Fill in the Component Information entry.  This is the Flags,
        // Revision, Version, Key and AffinityMask members from the
        // CONFIGURATION_COMPONENT structure.
        //
        // For PCI buses the affinity is set to all processors
        // (0xFFFFFFFF) and everything else is 0.
        //
        component.AffinityMask = 0xFFFFFFFF;

        PciConstStringToUnicodeString(&tempString,
                                      L"Component Information");
        status = ZwSetValueKey(indexHandle,
                               &tempString,
                               0,
                               REG_BINARY,
                               &component.Flags,
                               FIELD_OFFSET(CONFIGURATION_COMPONENT,
                                            ConfigurationDataLength)
                                   - FIELD_OFFSET(CONFIGURATION_COMPONENT,
                                                  Flags)
                               );

        if (!NT_SUCCESS(status)) {
            ZwDeleteKey(indexHandle);
            ZwClose(indexHandle);
            goto exit;
        }

        ZwClose(indexHandle);

        //
        // Move on to the next available MultifunctionAdapter subkey
        // number.
        //
        keyIndex += 1;
    }

    status = STATUS_SUCCESS;

exit:

    if (multifunctionHandle) {
        ZwClose(multifunctionHandle);
    }

    //
    // Set the legacy description lock event now that we're done modifying the
    // registry.
    //
    KeSetEvent(&PciLegacyDescriptionLock, 0, FALSE);

    return status;
}

NTSTATUS
PciEarlyRestoreAddListItem (
    ULONG Bus,
    PPCI_SLOT_NUMBER Slot,
    ULONG DeviceType
    )

/*++

Routine Description:

    This routine adds a new device to the early restore list.  Devices are
    never removed from the restore list.  There is no way to remove a device
    from the debug port list and removing the hibernate device is unlikely.
    However, these cases are handled by code that examines this list.

Arguments:

    Bus - Supplies the bus number of the device to add.

    Slot - Supplies a pointer to the slot number of the device.

    DeviceType - Supplies the reason this device is added to the list.
                 PCI_EARLY_RESTORE_DEBUG
                 PCI_EARLY_RESTORE_HIBER
                 PCI_EARLY_RESTORE_CRASHDUMP

Return Value:

    NTSTATUS code.

Environment:

    PASSIVE_LEVEL

--*/

{

    PLIST_ENTRY Entry;
    PPCI_EARLY_RESTORE_LIST NewEntry;
    PPCI_EARLY_RESTORE_LIST TmpEntry;

    PAGED_CODE();

    //
    // Modifications to this list are protected by the global PCI lock.
    // Reading the list is done only during very early boot when modification
    // is impossible and is therefore not protected.
    //

    PciAcquirePassiveLock(&PciGlobalLock);

    //
    // Check that this device is not already in list. If the device is already
    // in the list, update the device type field.
    //

    for (Entry = PciEarlyRestoreList.Flink;
         Entry != &PciEarlyRestoreList;
         Entry = Entry->Flink) {

        TmpEntry = CONTAINING_RECORD(Entry, PCI_EARLY_RESTORE_LIST, ListEntry);
        if ((TmpEntry->Bus == Bus) &&
            (TmpEntry->Slot.u.bits.DeviceNumber ==
             Slot->u.bits.DeviceNumber) &&
            (TmpEntry->Slot.u.bits.FunctionNumber ==
             Slot->u.bits.FunctionNumber)) {

            TmpEntry->DeviceType |= DeviceType;
            PciReleasePassiveLock(&PciGlobalLock);
            return STATUS_SUCCESS;
        }
    }

    NewEntry = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_EARLY_RESTORE_LIST));
    if (NewEntry == NULL) {
        PciReleasePassiveLock(&PciGlobalLock);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    NewEntry->Bus = Bus;
    NewEntry->Slot.u.AsULONG = Slot->u.AsULONG;
    NewEntry->DeviceType = DeviceType;
    InsertHeadList(&PciEarlyRestoreList, &NewEntry->ListEntry);
    PciReleasePassiveLock(&PciGlobalLock);
    return STATUS_SUCCESS;
}

NTSTATUS
PciEarlyRestoreExecuteRestore(
    PPCI_DEVICE Device
   )

/*++

Routine Description:

    This routine will disable decodes for a device, restore resources,
    then re-enable decodes.

Arguments:

    Device - Supplies a pointer to the device extension for the device to
        be restore.

Return Value:

    STATUS_SUCCESS on success.

--*/
{

    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Disable decodes.
    //

    PCI_DEVICE_SET_FLAG(Device, Connected);
    PciDisconnectDevice(Device);

    //
    // Restore Resources
    //

    Status = Device->Operations->SetResources(Device, &Device->Resources);
    if (Status != STATUS_SUCCESS) {
        return Status;
    }

    Status = PciRestoreDeviceRegisters(Device, &Device->SavedRegisters);
    if (Status != STATUS_SUCCESS) {
        return Status;
    }

    //
    // Restore decodes
    //

    PciConnectDevice(Device);

    return Status;
}

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciEarlyRestoreDevice(
    PPCI_DEVICE Device,
    ULONG RestoreType,
    BOOLEAN RestorePoweredDevices
    )

/*++

Routine Description:

    This routine will restore resources for an endpoint.

    Devices which are in D0 with decodes enabled will be restored to the
    resource and decode state that they had before hibernate.

    Devices which are in D3 will be ignored unless they are on the debugging
    path.

Arguments:

    Device - Supplies pointer to device to be examined and restored.

    RestoreType - Suplies the type of restore.

    RestorePoweredDevices - Supplies whether devices alread in D0 should be
        restored.

Return Value:

    STATUS_SUCCESS on successful restore.

--*/

{

    BOOLEAN DeviceRequired;
    PCI_PM_CAPABILITY PmCap;
    PPCI_PMCSR PmCsr;
    BOOLEAN PowerChanged;
    NTSTATUS Status;

    PowerChanged = FALSE;
    DeviceRequired = FALSE;
    Status = STATUS_SUCCESS;
    if (Device->PowerCapability != 0) {
        PciReadDeviceCapability(Device, PowerCapability, &PmCap);

        PmCsr = &PmCap.PMCSR.ControlStatus;

        //
        // Check if device is powered up or if it is being used for crashdump.
        // In the crashdump case, the PEP's power-on process may have left the
        // device in an uninitialized D0 state.  Thus, in the crashdump case we
        // should attempt to power up the device even if it seems to be in D0.
        //

        if (PCI_TO_NT_POWER_STATE(PmCsr->PowerState) != PowerDeviceD0 ||
            (((RestoreType & PCI_EARLY_RESTORE_CRASHDUMP) != 0) &&
             (Device->DeviceUsage.CrashDump != 0))) {


            //
            // Check if device is required.
            //

            if ((((RestoreType & PCI_EARLY_RESTORE_DEBUG) != 0) &&
                 (Device->OnDebugPath != FALSE)) ||
                (((RestoreType & PCI_EARLY_RESTORE_HIBER) != 0) &&
                 (Device->DeviceUsage.Hibernate != 0)) ||
                (((RestoreType & PCI_EARLY_RESTORE_CRASHDUMP) != 0) &&
                 (Device->DeviceUsage.CrashDump != 0))) {

                //
                // The debugging device is the only non-bridge device which
                // will have OnDebugPath set.  Similarly the boot device is
                // the only non-bridge device which will have Hibernate set.
                // For hibernate, this path should never be hit, since the
                // boot device was required to be on in the boot environment
                // and nothing should have turned it off since then.
                //

                DeviceRequired = TRUE;
            }

            //
            // The device is not in D0 and is not a required device. Ignore it.
            //

            if (DeviceRequired == FALSE) {
                return STATUS_SUCCESS;
            }

            //
            // Power on the device.
            //

            PowerChanged = TRUE;
            PmCsr->PowerState = NT_TO_PCI_POWER_STATE(PowerDeviceD0);
            PciWritePowerCapabilityRegister(Device, PMCSR, PmCsr);

            PciSetPowerChangeDelay(Device,
                                   Device->LogicalPowerState,
                                   PowerDeviceD0);

            Device->DstateChangeRetryCount = DEVICE_DSTATE_CHANGE_LIMIT;
            PciStallForPowerChange(Device, PowerDeviceD0);
        }
    }

    //
    // Restore the devices resources if it is safe to disconnect the device.
    // Devices which are marked to never be disconnected are omitted.  These
    // devices do not consume resources.  Furthermore, restoring these devices
    // (such as host bridges) is unsafe.
    //

    if (((Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) == 0) &&
         ((PowerChanged != FALSE) || (RestorePoweredDevices != FALSE))) {

        Status = PciEarlyRestoreExecuteRestore(Device);
    }

    return Status;
}

NTSTATUS
PciEarlyRestoreBus(
    PPCI_DEVICE Device,
    ULONG RestoreType,
    BOOLEAN RestorePoweredDevices
    )

/*++

Routine Description:

    Bridges which are in D0 will have resources restored.  Bridges in D3 will
    be ignored unless they are on the debugging path.  Those bridges will be
    powered up and have resources restored.

    This function will recurse on all children of the bridge.

Arguments:

    Device - Supplies a pointer to the device to be restored.

    RestoreType - Supplies the requirements of the restore.

    RestorePoweredDevices - Supplies whether devices already in D0 should still
        be restored.

Return Value:

    STATUS_SUCCESS on successful restore.

--*/

{

    BOOLEAN BridgeRequired;
    PPCI_BUS CurrentBus;
    PPCI_DEVICE CurrentDevice;
    PPCI_BUS ParentBus;
    PCI_PM_CAPABILITY PmCap;
    PPCI_PMCSR PmCsr;
    BOOLEAN PowerChanged;
    NTSTATUS Status;

    //
    // Get FDO for bridge
    //

    PowerChanged = FALSE;
    BridgeRequired = FALSE;
    ParentBus = Device->Parent;

    if (ParentBus == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    CurrentBus = ParentBus->ChildBuses;
    while (CurrentBus != NULL) {
        if (CurrentBus->PhysicalDeviceObject == Device->DeviceObject) {
            break;
        }

        CurrentBus = CurrentBus->SiblingBus;
    }

    if (CurrentBus == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Get the current power state for the bridge.
    // If the bridge doesn't have a power capability it is likely a host
    // bridge and always on.  Therefore, it must be restored.
    //

    if (Device->PowerCapability != 0) {
        PciReadDeviceCapability(Device, PowerCapability, &PmCap);

        PmCsr = &PmCap.PMCSR.ControlStatus;

        //
        // Check if bridge is powered up.
        //

        if (PCI_TO_NT_POWER_STATE(PmCsr->PowerState) != PowerDeviceD0) {

            //
            // Check if bridge is on the path of a required device
            //

            if ((((RestoreType & PCI_EARLY_RESTORE_DEBUG) != 0) &&
                 (Device->OnDebugPath != FALSE)) ||
                (((RestoreType & PCI_EARLY_RESTORE_HIBER) != 0) &&
                 (Device->DeviceUsage.Hibernate != 0)) ||
                (((RestoreType & PCI_EARLY_RESTORE_CRASHDUMP) != 0) &&
                 (Device->DeviceUsage.CrashDump != 0))) {

                BridgeRequired = TRUE;
            }

            //
            // The bridge is not in D0 and is not on the path of a
            // required device.  Ignore it.
            //

            if (BridgeRequired == FALSE) {
                return STATUS_SUCCESS;
            }

            //
            // Power on the bridge.
            //

            PowerChanged = TRUE;
            PmCsr->PowerState = NT_TO_PCI_POWER_STATE(PowerDeviceD0);
            PciWritePowerCapabilityRegister(Device, PMCSR, PmCsr);

            //
            // Stall for spec-defined powerup delay
            //

            PciDevice_Stall(Device, 10000);
        }
    }

    if ((PowerChanged != FALSE) || (RestorePoweredDevices != FALSE)) {
        PciEarlyRestoreExecuteRestore(Device);
    }

    //
    // Recurse over children of this bridge
    //

    for (CurrentDevice = CurrentBus->ChildDevices;
         CurrentDevice != NULL;
         CurrentDevice = CurrentDevice->Sibling) {

        if (CurrentDevice->HeaderType == 0) {
            Status = PciEarlyRestoreDevice(CurrentDevice,
                                           RestoreType,
                                           RestorePoweredDevices);
        } else {
            Status = PciEarlyRestoreBus(CurrentDevice,
                                        RestoreType,
                                        RestorePoweredDevices);
        }

        if (Status != STATUS_SUCCESS) {
            return Status;
        }
    }

    return STATUS_SUCCESS;
}

BOOLEAN
PciEarlyRestoreSkipAllowed(
    ULONG RestoreType
    )

/*++

Routine Description:

    This routine will determine if restoration is necessary.  Restoration is
    not necessary if: 1.) there are no relevant devices, 2.) the devices have
    the same resources that would be restored.

Arguments:

    RestoreType - Supplies flags that determine which devices should be checked.

Return Value:

    TRUE if the restore is not required.

--*/

{

    PPCI_DEVICE Device;
    PLIST_ENTRY Entry;
    PCI_PM_CAPABILITY PmCap;
    PPCI_PMCSR PmCsr;
    PCI_DEVICE_RESOURCES Resources;
    BOOLEAN ResourcesEqual;
    NTSTATUS Status;
    PPCI_EARLY_RESTORE_LIST TmpList;

    //
    // Skip the restore if there is no PCI deubgging device or if the debugging
    // device already has the correct resources.
    //

    ResourcesEqual = TRUE;
    for (Entry = PciEarlyRestoreList.Flink;
         Entry != &PciEarlyRestoreList;
         Entry = Entry->Flink) {

        TmpList = CONTAINING_RECORD(Entry, PCI_EARLY_RESTORE_LIST, ListEntry);

        //
        // Check if device qualifies for this type of restore.
        //

        if ((TmpList->DeviceType & RestoreType) == 0) {
            continue;
        }

        Device = PciFindDeviceByLocation(TmpList->Bus,
                                         TmpList->Slot,
                                         TRUE);

        //
        // If the device cannot be found then it was not enumerated or was
        // removed.  The restore can be skipped as resources aren't
        // stored anyway.
        //

        if (Device == NULL) {
            continue;
        }

        //
        // Check that the device is in D0.
        //

        if (Device->PowerCapability != 0) {
            PciReadDeviceCapability(Device, PowerCapability, &PmCap);

            PmCsr = &PmCap.PMCSR.ControlStatus;

            if (PCI_TO_NT_POWER_STATE(PmCsr->PowerState) != PowerDeviceD0) {
                ResourcesEqual = FALSE;
                break;
            }
        }

        //
        // Get the resources that the device is currently configured to use
        // compare those resources with those in the device extension.  If
        // resources are not found then the restore can be skipped.
        //

        Status = Device->Operations->GetResources(Device, &Resources);

        if (Status != STATUS_SUCCESS) {
            continue;
        }

        if (PciIsSameResource(&Resources, &Device->Resources) == FALSE) {
            ResourcesEqual = FALSE;
            break;
        }
    }

    return ResourcesEqual;
}

NTSTATUS
PciEarlyRestoreResources (
    _In_ SYSTEM_POWER_STATE SleepState
    )

/*++

Routine Description:

    This routine will restore resources during early resume.

    Devices which are in D0 with decodes enabled will be restored to the
    resource and decode state that they had before hibernate.

    Devices which are in D3 and devices with a clear command register will
    be ignored unless they are on the debugging path.

Arguments:

    SleepState - Supplies the sleep state being restored from.

Return Value:

    STATUS_SUCCESS on successful restore.

--*/

{

    ULONG RestoreType;

    //
    // Restoration is not required for resume from S1.  The definition of S2
    // and S3 are identical within ACPI, so they should be treated the same way.
    //

    if (SleepState <= PowerSystemSleeping1) {
        return STATUS_SUCCESS;
    }

    //
    // Set restore type based on sleep state.
    // 3 is resume from S3 and should restore only debugger requirements.
    // 4 is resume from S4 and should restore both debugger and hibernate.
    //

    RestoreType = PCI_EARLY_RESTORE_DEBUG;
    if (SleepState == PowerSystemHibernate) {
        RestoreType |= PCI_EARLY_RESTORE_HIBER;
    }

    return PciEarlyRestoreResourcesInternal(RestoreType, TRUE);
}

NTSTATUS
PciEarlyRestoreResourcesInternal (
    _In_ ULONG RestoreType,
    _In_ BOOLEAN RestorePoweredDevices
    )

/*++

Routine Description:

    This routine will restore resources during early resume.

    Devices which are in D0 with decodes enabled will be restored to the
    resource and decode state that they had before hibernate.

    Devices which are in D3 and devices with a clear command register will
    be ignored unless they are on the debugging path.

Arguments:

    RestoreType - Supplies the type of devices that should be restored.

    RestorePoweredDevices - Supplies a boolean indicating whether devices in
        D0 must also be restored. This is true for restores occurring during
        resume, but false when restoring during BugCheck.

Return Value:

    STATUS_SUCCESS on successful restore.

--*/

{

    PPCI_BUS CurrentBus;
    PPCI_DEVICE CurrentDevice;
    PPCI_SEGMENT CurrentSegment;
    NTSTATUS FailureStatus;
    NTSTATUS Status;

    //
    // Determine if this restoration can be skipped. This is possible
    // if the necessary devices are already in D0 with the correct resources.
    //

    if (PciEarlyRestoreSkipAllowed(RestoreType) != FALSE) {
        return STATUS_SUCCESS;
    }

    //
    // Examine all PCI Segments in the system.
    //

    Status = STATUS_SUCCESS;
    FailureStatus = STATUS_SUCCESS;
    for (CurrentSegment = PciSegmentList;
         CurrentSegment != NULL;
         CurrentSegment = CurrentSegment->Next) {

        //
        // Examine all buses in the Segment.
        // These are root buses and do not require restoration.
        //

        for (CurrentBus = CurrentSegment->PciRootBusList;
             CurrentBus != NULL;
             CurrentBus = CurrentBus->SiblingBus) {

            //
            // Examine all devices under this root bus.
            //

            for (CurrentDevice = CurrentBus->ChildDevices;
                 CurrentDevice != NULL;
                 CurrentDevice = CurrentDevice->Sibling) {

                if (CurrentDevice->HeaderType == 0) {
                    Status = PciEarlyRestoreDevice(CurrentDevice,
                                                   RestoreType,
                                                   RestorePoweredDevices);
                } else {
                    Status = PciEarlyRestoreBus(CurrentDevice,
                                                RestoreType,
                                                RestorePoweredDevices);
                }

                if (!NT_SUCCESS(Status)) {
                    FailureStatus = Status;
                }
            }
        }
    }

    return FailureStatus;
}

VOID
PciMarkHiberPhase (
    VOID
    )

/*++

Routine Description:

    This routine is called during hibernate to mark all necessary pages for the
    boot phase. These are pages that will be accessed very early in resume and
    therefore would need to restored in the resume loader.

Arguments:

    None.

Return Value:

    None.

--*/

{
    PPCI_BUS CurrentBus;
    PPCI_DEVICE CurrentDevice;
    PPCI_SEGMENT CurrentSegment;
    PLIST_ENTRY Entry;
    PPCI_ROOT LastRoot;
    PPCI_BUS_INTERFACE_STANDARD LastPciBusInterface;
    PPCI_HEADER_OPERATIONS LastOperations;
    PPCI_EARLY_RESTORE_LIST TmpRestoreList;

    //
    // Mark the debug ports array.
    //

    if (PciDebugPortsCount != 0) {

        PoSetHiberRange(NULL,
                        PO_MEM_BOOT_PHASE,
                        PciDebugPorts,
                        PciDebugPortsCount * sizeof(*PciDebugPorts),
                        'picP');
    }

    //
    // Mark the early restore list.
    //

    for (Entry = PciEarlyRestoreList.Flink;
         Entry != &PciEarlyRestoreList;
         Entry = Entry->Flink) {

        TmpRestoreList = CONTAINING_RECORD(Entry,
                                           PCI_EARLY_RESTORE_LIST,
                                           ListEntry);

        PoSetHiberRange(NULL,
                        PO_MEM_BOOT_PHASE,
                        TmpRestoreList,
                        sizeof(PCI_EARLY_RESTORE_LIST),
                        'picP');
    }

    //
    // Mark all of the segments, buses, and devices.
    //

    LastRoot = NULL;
    LastPciBusInterface = NULL;
    LastOperations = NULL;

    for (CurrentSegment = PciSegmentList;
         CurrentSegment != NULL;
         CurrentSegment = CurrentSegment->Next) {

        PoSetHiberRange(NULL,
                        PO_MEM_BOOT_PHASE,
                        CurrentSegment,
                        sizeof(*CurrentSegment),
                        'sicP');


        for (CurrentBus = CurrentSegment->PciRootBusList;
             CurrentBus != NULL;
             CurrentBus = PciNextBusInSegment(CurrentBus)) {

            PoSetHiberRange(NULL,
                            PO_MEM_BOOT_PHASE,
                            CurrentBus,
                            sizeof(*CurrentBus),
                            'bicP');

            if (PCI_ROOT_BUS(CurrentBus)) {

                PoSetHiberRange(NULL,
                                PO_MEM_BOOT_PHASE,
                                CurrentBus->Root,
                                sizeof(*CurrentBus->Root),
                                'bicP');

            } else {

                PoSetHiberRange(NULL,
                                PO_MEM_BOOT_PHASE,
                                CurrentBus->PciBridge,
                                sizeof(*CurrentBus->PciBridge),
                                'bicP');

                if (CurrentBus->PciBridge->ExpressBridge != NULL) {

                    PEXPRESS_BRIDGE expressBridge =
                        CurrentBus->PciBridge->ExpressBridge;

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    expressBridge,
                                    sizeof(*expressBridge),
                                    'bicP');

                    if (CurrentBus->PciBridge->ExpressBridge->BridgeType ==
                        PciExpressUpstreamSwitchPort) {

                        PSWITCH_COMPLEX switchComplex =
                            expressBridge->UpstreamSwitchPort.SwitchComplex;

                        PoSetHiberRange(NULL,
                                        PO_MEM_BOOT_PHASE,
                                        switchComplex,
                                        sizeof(*switchComplex),
                                        'bicP');
                    }
                }
            }

            for (CurrentDevice = CurrentBus->ChildDevices;
                 CurrentDevice != NULL;
                 CurrentDevice = CurrentDevice->Sibling) {

                PoSetHiberRange(NULL,
                                PO_MEM_BOOT_PHASE,
                                CurrentDevice,
                                sizeof(*CurrentDevice),
                                'dicP');

                if (LastRoot != CurrentDevice->Root) {

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    CurrentDevice->Root,
                                    sizeof(*CurrentDevice->Root),
                                    'dicP');

                    LastRoot = CurrentDevice->Root;
                }

                if (LastPciBusInterface != CurrentDevice->Root->PciBusInterface) {

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    CurrentDevice->Root->PciBusInterface,
                                    sizeof(*CurrentDevice->Root->PciBusInterface),
                                    'dicP');

                    LastPciBusInterface = CurrentDevice->Root->PciBusInterface;
                }

                if (LastOperations != CurrentDevice->Operations) {

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    CurrentDevice->Operations,
                                    sizeof(*CurrentDevice->Operations),
                                    'dicP');

                    LastOperations = CurrentDevice->Operations;
                }

                if (CurrentDevice->ExpressPort != NULL) {

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    CurrentDevice->ExpressPort,
                                    sizeof(*CurrentDevice->ExpressPort),
                                    'dicP');

                    if (CurrentDevice->ExpressPort->ExpressLink != NULL) {
                        PoSetHiberRange(NULL,
                                        PO_MEM_BOOT_PHASE,
                                        CurrentDevice->ExpressPort->ExpressLink,
                                        sizeof(*CurrentDevice->ExpressPort->ExpressLink),
                                        'dicP');
                    }


                    if (CurrentDevice->ExpressPort->TphSteeringTags != NULL) {
                        PoSetHiberRange(NULL,
                                        PO_MEM_BOOT_PHASE,
                                        CurrentDevice->ExpressPort->TphSteeringTags,
                                        (CurrentDevice->ExpressPort->TphRequesterCapabilityBlock.RequesterCapability.StTableSize + 1) *
                                            sizeof(PCI_EXPRESS_TPH_ST_TABLE_ENTRY),
                                        'dicP');
                    }

                    if (CurrentDevice->ExpressPort->VcSaveArea != NULL) {
                        PoSetHiberRange(NULL,
                                        PO_MEM_BOOT_PHASE,
                                        CurrentDevice->ExpressPort->VcSaveArea,
                                        CurrentDevice->ExpressPort->VcSaveSize,
                                        'dicP');
                    }

                } else if (CurrentDevice->ExpressCompatibilityPort != NULL) {

                    PoSetHiberRange(NULL,
                                    PO_MEM_BOOT_PHASE,
                                    CurrentDevice->ExpressCompatibilityPort,
                                    sizeof(*CurrentDevice->ExpressCompatibilityPort),
                                    'dicP');
                }
            }

            //
            // Chain into any filter in the stack, letting them mark their
            // ranges, too.
            //

            if (PCI_ROOT_BUS(CurrentBus)) {

                if (CurrentBus->Root->PciBusInterface->PrepareMultistageResume != NULL) {
                    CurrentBus->Root->PciBusInterface->PrepareMultistageResume(
                        CurrentBus->Root->PciBusInterface->Context
                        );
                } else {

                    //
                    // Failure here means that multi-stage resume should not
                    // have occurred.
                    //

                    NT_ASSERT(FALSE);
                }
            }
        }
    }

    return;
}

BOOLEAN
PciMultiStageResumeCapable(
    VOID
    )

/*++

Routine Description:

    This routine is called at the beginning of a hibernate transition to
    determine if a pci filter exists which does not support multi-phase
    resume and therefore multi-phase resume should be disabled.

Arguments:

    None.

Return Value:

    TRUE means that every bus can be accessed during the early part of
    multi-stage resume.  FALSE means that it can't, and that multi-stage
    resume should not be attempted.

--*/

{

   PPCI_BUS CurrentBus;
   PPCI_SEGMENT CurrentSegment;

   for (CurrentSegment = PciSegmentList;
         CurrentSegment != NULL;
         CurrentSegment = CurrentSegment->Next) {

        for (CurrentBus = CurrentSegment->PciRootBusList;
             CurrentBus != NULL;
             CurrentBus = PciNextBusInSegment(CurrentBus)) {

            if (PCI_ROOT_BUS(CurrentBus)) {
                if (CurrentBus->Root->PciBusInterface->PrepareMultistageResume == NULL) {

                    //
                    // Failure here means that multi-stage resume should not
                    // occur.
                    //

                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciDebugDevicePowerCallback (
    __in DEVICE_POWER_STATE PowerState,
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is called back from kd when the system attempts to
    enter or leave an idle state requiring power to the debug device to be
    turned off or turned back on and state restored.

Arguments:

    PowerState - Supplies a PowerDeviceDx value specifying which power state to
    transition the device to. Currently only PowerDeviceD0 and PowerDeviceD3 are
    supported.

    Context - Supplies a pointer to the device.

Return Value:

    STATUS_SUCCESS on success.

--*/

{
    PPCI_DEVICE Device;
    DEVICE_POWER_STATE OldPowerState;
    PCI_PM_CAPABILITY PmCap;
    PPCI_PMCSR PmCsr;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    if ((PowerState != PowerDeviceD0) &&
        (PowerState != PowerDeviceD3)) {

        return Status;
    }

    Device = (PPCI_DEVICE)Context;

    //
    // Set OnDebugPath to reflect whether the debugger is actively using
    // the device.
    //

    Device->OnDebugPath = (PowerState == PowerDeviceD0);

    //
    // If the debugger is disconnecting, don't move the device to D3 if
    // the device has a running driver associated with it that's not also
    // already in D3.
    //

    if ((PowerState == PowerDeviceD3) &&
        (Device->DeviceState == PciStarted) &&
        (Device->LogicalPowerState != PowerDeviceD3)) {

        return Status;
    }

    if (Device->PowerCapability == 0) {
        return Status;
    }

    PciReadDeviceCapability(Device, PowerCapability, &PmCap);
    PmCsr = &PmCap.PMCSR.ControlStatus;
    OldPowerState = PCI_TO_NT_POWER_STATE(PmCsr->PowerState);
    if (OldPowerState == PowerState) {
        return Status;
    }

    //
    // Set the new power state without clearing PMEStatus.  PMEStatus is RW1C.
    //

    PmCsr->PowerState = NT_TO_PCI_POWER_STATE(PowerState);
    PmCsr->PMEStatus = 0;  
    PciWritePowerCapabilityRegister(Device, PMCSR, PmCsr);
    PciSetPowerChangeDelay(Device,
                           OldPowerState,
                           PowerState);

    Device->DstateChangeRetryCount = DEVICE_DSTATE_CHANGE_LIMIT;
    PciStallForPowerChange(Device, PowerState);
    if (PowerState == PowerDeviceD0) {

        //
        // On a transition to D0 restore the device state.
        //

        Status = PciEarlyRestoreExecuteRestore(Device);
    }

    return Status;
}

NTSTATUS
PciDebugDevicePowerCallbackRegister (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine registers a debug device with kd power management. This will
    cause callbacks to be issued to the device when the system attempts to
    enter an idle state requiring power to the debug device to be turned off
    and later turned back on and state restored.

Arguments:

    Device - Supplies a pointer to the device to be registered.

Return Value:

    STATUS_SUCCESS on successful deregistration.

--*/

{

    if (Device->DebugDevicePowerCallbackHandle != NULL) {
        return STATUS_SUCCESS;
    }

    if (Device->OnDebugPath != TRUE) {
        return STATUS_SUCCESS;
    }

    if (Device->HeaderType != PCI_DEVICE_TYPE) {
        return STATUS_SUCCESS;
    }

    if (Device->PowerCapability == 0) {
        return STATUS_SUCCESS;
    }

    return KdRegisterPowerHandler(PciDebugDevicePowerCallback,
                                  Device,
                                  &Device->DebugDevicePowerCallbackHandle);
}

NTSTATUS
PciDebugDevicePowerCallbackDeregister (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine deregisters a previously registered debug device from
    kd power management.

Arguments:

    Device - Supplies a pointer to the device to be deregistered.

Return Value:

    STATUS_SUCCESS on successful deregistration.

--*/

{

    NTSTATUS Status;

    if (Device->DebugDevicePowerCallbackHandle == NULL) {
        return STATUS_SUCCESS;
    }

    Status = KdDeregisterPowerHandler(Device->DebugDevicePowerCallbackHandle);
    Device->DebugDevicePowerCallbackHandle = NULL;
    return Status;
}
