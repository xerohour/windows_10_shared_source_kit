/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    busnumbers.c

Abstract:

    This module implements routines pertaining to PCI bus numbers.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

VOID
PciFitBridge(
    __inout PPCI_BUS Bus,
    __inout PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
_Success_(return != 0)
UCHAR
PciFindLargestBusNumberGap(
    _Inout_ PPCI_BUS Bus,
    _Out_ PUCHAR GapBase
    );

VOID
PciAssignBusNumber(
    __inout PPCI_DEVICE Device,
    __in UCHAR BusNumber
    );

VOID
PciUpdateAncestorSubordinateBuses(
    __in PPCI_BUS Bus,
    __in UCHAR Subordinate
    );


VOID
PciCaptureBusNumbers(
    __in PPCI_BUS Bus,
    __in UCHAR CaptureCount
    );


// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciConfigureBusNumbers)
    #pragma alloc_text(PAGE, PciFitBridge)
    #pragma alloc_text(PAGE, PciFindLargestBusNumberGap)
    #pragma alloc_text(PAGE, PciAssignBusNumber)
    #pragma alloc_text(PAGE, PciExtendBusNumberRange)
    #pragma alloc_text(PAGE, PciUpdateAncestorSubordinateBuses)
    #pragma alloc_text(PAGE, PciAreBusNumbersConfigured)
    #pragma alloc_text(PAGE, PciCaptureBusNumbers)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

VOID
PciConfigureBusNumbers(
    __inout PPCI_BUS Bus
    )
/*++

Routine Description:

    This routine is called after scanning a PCI bus (root or bridge) and
    configures the bus numbers for any newly encountered bridges if possible.

    Any unconfigurable bridges will be set to Primary = Secondary = Subordinate = 0
    and their IO, Memory and BusMaster bits will be disabled.  When PCI is later
    asked to Add to them it will fail.

Arguments:

    Bus - The bridge we have just enumerated.

Return Value:

    Status.

--*/
{
    UCHAR bridgeCount, configuredBridgeCount;
    UCHAR captureCount;
    UCHAR deviceCaptureCount;
    UCHAR secondary, subordinate;
    PPCI_DEVICE device;
    UCHAR zero;

    PAGED_CODE();

    TRACE(BUS_NUMBER_TRACE_LEVEL,
          "PciConfigureBusNumbers called for bus 0x%x( 0x%p )\n",
          Bus->SecondaryBusNumber,
          Bus
          );

    PciClearBitRangeInclusive(&Bus->BusNumberMap,
                              LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber) + 1,
                              LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber)
                              );

    PciClearBitRangeInclusive(&Bus->Root->BridgeMap,
                              LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber) + 1,
                              LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber)
                              );

    //
    // Walk the list of child PDO's for this bus and count the number of
    // bridges and configured bridges
    //
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    bridgeCount = 0;
    configuredBridgeCount = 0;
    captureCount = 0;

    for (device = Bus->ChildDevices; device != NULL; device = device->Sibling) {

        if (device->HeaderType == PCI_DEVICE_TYPE) {
            continue;
        }

        if (!device->Present) {
            TRACE(BUS_NUMBER_TRACE_LEVEL,
                  "PciConfigureBusNumbers: Skipping bus numbers on not present bridge %x/%x/%x ( 0x%p )\n",
                  device->BusNumber,
                  device->Slot.u.bits.DeviceNumber,
                  device->Slot.u.bits.FunctionNumber,
                  device
                  );
            continue;
        }

        bridgeCount++;

        if (device->BusNumbersConfigured) {

            secondary = device->SavedRegisters.bridge.SecondaryBusNumber;
            subordinate = device->SavedRegisters.bridge.SubordinateBusNumber;

            PciSetBitRangeInclusive(&Bus->BusNumberMap, secondary, subordinate);

            RtlSetBit(&Bus->Root->BridgeMap, secondary);

            TRACE(BUS_NUMBER_TRACE_LEVEL,
                  "PciConfigureBusNumbers: Bridge %x/%x/%x ( 0x%p ) already configured at 0x%x->0x%x\n",
                  device->BusNumber,
                  device->Slot.u.bits.DeviceNumber,
                  device->Slot.u.bits.FunctionNumber,
                  device,
                  secondary,
                  subordinate
                  );

            configuredBridgeCount++;
            continue;

        } else {

            //
            // The bridge needs to be configured.
            // Disable it and we will fix it later.  Disabling means
            // zeroing the bus numbers and turning off the decodes.
            //
            DebugSetDebugFlag(device, PCI_BRIDGE_DISABLED);

            zero = 0;

            PciWriteConfigRegister(device, bridgeCommon.PrimaryBus, &zero);
            PciWriteConfigRegister(device, bridgeCommon.SecondaryBus, &zero);
            PciWriteConfigRegister(device, bridgeCommon.SubordinateBus, &zero);

            if (PciCanDisableDevice(device) != KeepDeviceActive) {
                PciDisconnectDevice(device);
            }
        }
    }

    //
    // Scan for devices that can benefit from bus number capturing.
    // SR-IOV devices may require capturing of more bus numbers if they
    // support more than 255 VFs or if ARI is not supported.
    // In either case there will be no bridges on the bus.
    //
    for (device = Bus->ChildDevices;
         device != NULL;
         device = device->Sibling) {

        if (!device->Present) {
            continue;
        }

        deviceCaptureCount = PciMaximumBusNumbersToCapture(device);
        if (deviceCaptureCount > captureCount) {
            captureCount = deviceCaptureCount;
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    if (captureCount != 0) {
        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "PciConfigureBusNumbers: Capture 0x%x numbers on bus 0x%x\n",
              captureCount,
              Bus->SecondaryBusNumber
            );

        PciCaptureBusNumbers(Bus, captureCount);
    }

    //
    // Now there are three posibilities...
    //
    if (bridgeCount == configuredBridgeCount) {

        //
        // All the bridges are configured - not a lot to do...
        //
        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "PciConfigureBusNumbers: 0x%x bridges found on bus 0x%x - all already configured\n",
              bridgeCount,
              Bus->SecondaryBusNumber
              );

        goto exit;
    }

    if (configuredBridgeCount == 0 && captureCount == 0) {

        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "PciConfigureBusNumbers: 0x%x bridges found on bus 0x%x - all need configuration\n",
              bridgeCount,
              Bus->SecondaryBusNumber
              );

        //
        // All the bridges require configuration so we should use a spreading
        // out algorithm
        //

        PciSpreadBridgesHotplug(Bus);

    } else {

        //
        // Some of the bridges are configured and some are not - we should try
        // to fit the unconfigured ones into the holes left by the configured
        // ones
        //
        PCI_ASSERT(configuredBridgeCount < bridgeCount);

        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "PciConfigureBusNumbers: 0x%x bridges found on bus 0x%x - 0x%x need configuration\n",
              bridgeCount,
              Bus->SecondaryBusNumber,
              bridgeCount - configuredBridgeCount
              );

        //
        // Walk the list of devices again and configure each one separately
        //
        PciAcquirePassiveLock(&Bus->ChildDeviceLock);
        for (device = Bus->ChildDevices; device != NULL; device = device->Sibling) {

            if ((device->HeaderType != PCI_DEVICE_TYPE) &&
                device->Present &&
                !device->BusNumbersConfigured) {

                PCI_ASSERT(device->DebugFlags & PCI_BRIDGE_DISABLED);

                PciFitBridge(Bus, device);
            }
        }
        PciReleasePassiveLock(&Bus->ChildDeviceLock);
    }

exit:

    TRACE(BUS_NUMBER_TRACE_LEVEL, "Root Bridge map\n");
    DebugDumpBitMap(BUS_NUMBER_TRACE_LEVEL, &Bus->Root->BridgeMap);
    TRACE(BUS_NUMBER_TRACE_LEVEL, "Bus number map\n");
    DebugDumpBitMap(BUS_NUMBER_TRACE_LEVEL, &Bus->BusNumberMap);
    TRACE(BUS_NUMBER_TRACE_LEVEL, "PciConfigureBusNumbers complete for bus 0x%p\n", Bus);
}

VOID
PciFitBridge(
    __inout PPCI_BUS Bus,
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine attemps to find a range of bus numbers for Bridge given the
    constraints of the already configured bridges.

    If a particular bridge can not be configured it is disabled (Decodes OFF and
    bus number 0->0-0) and the subsequent AddDevice will fail.

Arguments:

    Bus - The bus extension for the bridge we are enumerating.

    Device - The bridge we want to configure

Return Value:

    None

--*/
{
    UCHAR gapBase, gapLength, number;

    PAGED_CODE();

    gapLength = PciFindLargestBusNumberGap(Bus, &gapBase);

    TRACE(BUS_NUMBER_TRACE_LEVEL, "PciFitBridge: device %p\n", Device);
    TRACE(BUS_NUMBER_TRACE_LEVEL, "PciFitBridge: largest gap = 0x%x\n", gapLength);

    //
    // Did we find anywhere to put the bridge?
    //
    if (gapLength >= 1) {

        //
        // Ok - we have some space to play with so we can configure out bridge
        // right in the middle of the gap, if the bestGap is 1 (ie the bridge
        // just fits) then this still works.
        //
        number = gapBase + (gapLength / 2);

        PciAssignBusNumber(Device, number);

        //
        // It's possible that the bus number we chose for this
        // bridge is greater than its parent's subordinte bus.
        // if so, update the ancestor subordinates
        //
        if (number > LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber)) {
            PciUpdateAncestorSubordinateBuses(Bus, number);
        }
    }
}


VOID
PciCaptureBusNumbers(
    __in PPCI_BUS Bus,
    __in UCHAR CaptureCount
    )
/*++

Routine Description:

    This routine attempts to capture bus numbers for child devices on the bus.

    Capturing less than requested is not fatal although it may lead to
    limited capacity.

Arguments:

    Bus - The bus extension for the bridge we are enumerating.

    CaptureCount - Maximum count of Bus Numbers to capture.

Return Value:

    None.

--*/
{
    UCHAR gapBase;
    UCHAR gapLength;
    UCHAR secondary;
    UCHAR subordinate;

    PAGED_CODE();

    secondary = LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber);
    subordinate = LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber);

    //
    // See how many numbers are available for capture.  The gap must
    // start at SecondaryBusNumber + 1 because there is only one
    // device on the bus (and it is not a bridge).
    //
    gapLength = PciFindLargestBusNumberGap(Bus, &gapBase);
    PCI_ASSERT(gapLength == 0 || gapBase == secondary + 1);

    if (gapBase != secondary + 1) {
        gapLength = 0;
    }

    if (gapLength < CaptureCount) {
        CaptureCount = gapLength;
    }

    TRACE(BUS_NUMBER_TRACE_LEVEL,
          "Capturing 0x%x bus numbers on Bus %x ( 0x%p )\n",
          CaptureCount,
          Bus->SecondaryBusNumber,
          Bus
          );

    //
    // Mark the captured buses as if they were consumed by bridges.
    //
    RtlSetBits(&Bus->Root->BridgeMap, secondary + 1, CaptureCount);
    RtlSetBits(&Bus->BusNumberMap, secondary + 1, CaptureCount);

    //
    // It's possible that the bus numbers captured for this
    // device are greater than its parent's subordinte bus.
    // if so, update the ancestor subordinates
    //
    if (secondary + CaptureCount > subordinate) {
        PciUpdateAncestorSubordinateBuses(Bus, secondary + CaptureCount);
    }

    Bus->CapturedBusNumberCount = CaptureCount;

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
_Success_(return != 0)
UCHAR
PciFindLargestBusNumberGap(
    _Inout_ PPCI_BUS Bus,
    _Out_ PUCHAR GapBase
    )
/*++

Routine Description:

    This routine finds the largest available bus number hole available
    to bridges that sit on the given bus.  The range to search is limited
    on the low end by the bus's Secondary bus number and on the high end
    by the largest number this bus's Subordinate bus number could be expanded
    to without colliding with another already-configured bridge.

    Bridges that are already configured on this bus will already occupy
    portions of this range, so this routine needs to find the largest gap
    still available.

Arguments:

    Bus - The bus whose bus number range should be searched for gaps.

    GapBase - On exit, will point to the start of the largest gap.  If
        there are no gaps, this value is undefined.

Return Value:

    The size of the largest gap, or 0 if there are no gaps.

--*/
{
    ULONG limit, base, length;

    PAGED_CODE();

    //
    // First find the end of the range of bus numbers we could assign to this bus.
    // This is done by seeing what bridges have been configured at bus numbers
    // higher than this bus's Subordinate bus number.  This routine returns
    // the location of the next bridge, meaning that it is the first bus number
    // that it is illegal to assign to this bridge.
    //
    limit = RtlFindSetBits(&Bus->Root->BridgeMap,
                           1,
                           LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber) + 1
                           );

    PCI_ASSERT(limit <= 0x100);

    TRACE(BUS_NUMBER_TRACE_LEVEL, "bus number range constraint = 0x%x\n", limit);

    //
    // Now consider all bus numbers starting with this limit and going to the end
    // of the bus number space to be unavailable on this bus.
    //
    if (limit < 0x100) {

        RtlSetBits(&Bus->BusNumberMap, limit, 0x100 - limit);
    }

    //
    // This routine returns the largest string of clear bits, which corresponds
    // to the largest gap between bus numbers.
    //
    length = RtlFindLongestRunClear(&Bus->BusNumberMap, &base);

    PCI_ASSERT(length < 0x100);
    PCI_ASSERT(base < 0x100);

    *GapBase = (UCHAR)base;
    return (UCHAR)length;
}

VOID
PciAssignBusNumber(
    __inout PPCI_DEVICE Device,
    __in UCHAR BusNumber
    )
/*++

Routine Description:

    This routine assigns a bus number to the given bridge.  In the initial
    configuration done at enumeration time, only a single bus number
    is ever assigned to a bridge.  The bridge's bus number range can
    be expanded later.

Arguments:

    Device - The bridge to assign a bus number to.

    BusNumber - The bus number to assign.

Return Value:

    VOID

--*/
{
    PAGED_CODE();

    PCI_ASSERT(Device->HeaderType != PCI_DEVICE_TYPE);

    TRACE(BUS_NUMBER_TRACE_LEVEL,
          "Assigning bus number 0x%x to bridge %x/%x/%x ( 0x%p )\n",
          BusNumber,
          Device->BusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber,
          Device
          );

    Device->SavedRegisters.bridge.SecondaryBusNumber = BusNumber;
    Device->SavedRegisters.bridge.SubordinateBusNumber = BusNumber;

    RtlSetBit(&Device->Root->BridgeMap, BusNumber);
    RtlSetBit(&Device->Parent->BusNumberMap, BusNumber);

    PCI_DEVICE_SET_FLAG(Device, BusNumbersConfigured);
    DebugClearDebugFlag(Device, PCI_BRIDGE_DISABLED);
}

VOID
PciUpdateAncestorSubordinateBuses(
    __in PPCI_BUS Bus,
    __in UCHAR Subordinate
    )
/*++

Routine Description:

    This routine walks the bridge hierarchy updating the subordinate bus numbers
    of each ancestor to ensure that numbers up to Subordinate are passed.

Arguments:

    Bus - the parent of the bridge(s) we have just configured

    Subordinate - The maximum (subordinate) bus number to pass


Return Value:

    VOID

--*/
{
    PPCI_BUS current;

    PAGED_CODE();

    //
    // For all ancestor bridges update the subordinate bus number
    //
    for (current = Bus; current && current->PciBridge; current = current->ParentBus) {

        PCI_ASSERT(current->ParentBus);

        if (Subordinate > LOCAL_BUS_NUMBER(current->SubordinateBusNumber)) {

            TRACE(BUS_NUMBER_TRACE_LEVEL,
                  "Updating bus 0x%x ( 0x%p ) to subordinate bus 0x%x\n",
                  current->SecondaryBusNumber,
                  current,
                  FULL_BUS_NUMBER(
                    SEGMENT_NUMBER_FOR_BUS(current->SecondaryBusNumber),
                    Subordinate)
                  );

            PciExtendBusNumberRange(current->PciBridge, Subordinate);

        } else {

            break;
        }
    }

    //
    // lets make sure the subordinate value we came up with actually gets
    // down this root...
    //
    PCI_ASSERT(Subordinate <=
               LOCAL_BUS_NUMBER(Bus->Root->Bus->SubordinateBusNumber));

}

BOOLEAN
PciAreBusNumbersConfigured(
    __in PPCI_BUS ParentBus,
    __in UCHAR Primary,
    __in UCHAR Secondary,
    __in UCHAR Subordinate
    )
/*++

Routine Description:

    This checks if the bus numbers values passed in indicate
    a valid configuration for a bridge.

Arguments:

    ParentBus - The bus extension of the parent of this bridge.

    Primary - The value read from the primary bus number register.

    Secondary - The value read from the secondary bus number register.

    Subordinate - The value read from the subordinate bus number register.

Return Value:

    TRUE if numbers are valid FALSE otherwise.

--*/
{
    UCHAR localSubordinateOnParent;
    BOOLEAN runClear;
    USHORT segmentNumber;

    PAGED_CODE();

    //
    // Check this bridge is configured to run on the bus we found it.
    //
    if (Primary != LOCAL_BUS_NUMBER(ParentBus->SecondaryBusNumber)) {
        return FALSE;
    }

    //
    // Ensure the child bus number is greater than the parent bus.
    // (HP Omnibooks actually break this rule when not plugged into
    // their docking stations).
    //
    if (Secondary <= Primary) {
        return FALSE;
    }

    //
    // make sure the secondary bus is in the range
    // of busses the bridge is programmed for.  Paranoia.
    //
    if (Subordinate < Secondary) {
        return FALSE;
    }

    //
    // Make sure the end of the bus number range is not beyond
    // what the parent will pass. This check was introduced in LH
    // and caused grief to some systems that had no issues running
    // downlevel OSes. Make an exception for the root bus when a
    // system wide hack flag is present as a workaround for
    // the BIOS bug.
    //
    segmentNumber = SEGMENT_NUMBER_FOR_BUS(ParentBus->SecondaryBusNumber);
    localSubordinateOnParent =
        LOCAL_BUS_NUMBER(ParentBus->SubordinateBusNumber);

    if (Subordinate > localSubordinateOnParent) {
        if (!PCI_ROOT_BUS(ParentBus) ||
            !(PciSystemWideHackFlags & PCI_SYS_HACK_IGNORE_ROOTBUS_RANGE)) {

            return FALSE;
        }

        //
        // Expand the pci root - subordinate bus number.
        // The PciConfigureBusNumbers routine updates the BridgeMap bit mask
        // to reflect this change. The BusNumberMap in the segments is
        // updated at this point though.
        //

        runClear = RtlAreBitsClear(
                        &ParentBus->Root->Segment->BusNumberMap,
                        localSubordinateOnParent + 1,
                        Subordinate - localSubordinateOnParent
                        );

        PCI_ASSERT(runClear != FALSE);

        PciSetBitRangeInclusive(&ParentBus->Root->Segment->BusNumberMap,
                                localSubordinateOnParent + 1,
                                Subordinate
                                );

        ParentBus->SubordinateBusNumber =
            FULL_BUS_NUMBER(segmentNumber, Subordinate);

    }

    return TRUE;
}

VOID
PciExtendBusNumberRange(
    __in PPCI_BRIDGE PciBridge,
    __in UCHAR Subordinate
    )
/*++

Routine Description:

    This routine updates the subordinate bus number assigned to a bridge.
    Note that the bus numbers must be kept in both the
    bus extension and the device extension for the bridge, so this code
    must reach into the device extension of the PDO.  This is the ONLY
    case of this in the PCI driver, and is not to be duplicated anywhere.

    Once a secondary bus number has been assigned to a bridge, it cannot
    be changed during the lifetime of the PDO for that bridge.  So this
    routine can only update the subordinate bus number for a bridge.

Arguments:

    PciBridge - The bridge to set the bus numbers of.

    Subordinate - The subordinate bus number to assign

Return Value:

    None

--*/
{
    PPCI_DEVICE device;
    USHORT segmentNumber;

    PAGED_CODE();

    //
    // THE FOLLOWING TWO LINES ARE NOT TO BE USED ANYWHERE
    // ELSE IN THE PCI DRIVER.
    //
    device = (PPCI_DEVICE)PciBridge->Bus->PhysicalDeviceObject->DeviceExtension;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    PCI_ASSERT(LOCAL_BUS_NUMBER(PciBridge->Bus->SecondaryBusNumber) <
               Subordinate);

    PCI_ASSERT(LOCAL_BUS_NUMBER(PciBridge->Bus->SubordinateBusNumber) <
               Subordinate);

    //
    // Grab the PCI Bus lock - this will let hwverifier reliably check the
    // config space against our extension.
    //
    PciAcquirePassiveLock(&PciBusLock);
    segmentNumber = SEGMENT_NUMBER_FOR_BUS(PciBridge->Bus->SecondaryBusNumber);
    PciBridge->Bus->SubordinateBusNumber =
        FULL_BUS_NUMBER(segmentNumber, Subordinate);

    device->SavedRegisters.bridge.SubordinateBusNumber = Subordinate;
    PciWriteConfigRegister(device, bridgeCommon.SubordinateBus, &Subordinate);
    PciReleasePassiveLock(&PciBusLock);
}
