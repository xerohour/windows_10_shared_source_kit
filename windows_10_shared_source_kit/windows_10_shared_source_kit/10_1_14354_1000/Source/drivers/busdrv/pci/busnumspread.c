/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    busnumspread.c

Abstract:

    This module implements routines for preconfiguring Bus numbers below a
    hotplug port.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "configraw.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types

//
// The BRIDGE structure is used only in this file to recurse through the
// tree of unconfigured PCI devices. To reduce the stack space used by the
// recursion, all variables which otherwise might be implemented as locals
// and which need to remain live during recursion should be kept here.
//

typedef struct _BRIDGE {
    LIST_ENTRY Link;            // Link to sibling bridges.
    LIST_ENTRY Children;        // List of child bridges.
    struct _BRIDGE *Parent;     // Parent bridge.
    PCI_SLOT_NUMBER Slot;       // Slot number of this bridge.
    UCHAR ChildCount;           // Count of child bridges.
    UCHAR BusNumCount;          // Count of all Bus numbers below this bridge.
    UCHAR HotplugCount;         // Count of all hotplug ports below this bridge.
    UCHAR Primary;              // Primary bus of this bridge.
    UCHAR Secondary;            // Secondary bus of this bridge.
    UCHAR Subordinate;          // Subordinate bus of this bridge.
    UCHAR DevicesPresent;       // Number of bridges & devices below this bridge.
    UCHAR HeaderType;           // Type of bridge
    BOOLEAN RootBus;            // Flag to represent whether the device is a PCI root bus.
    PCI_SLOT_NUMBER EnumSlot;   // Used for enumerating devices below this bridge.
    BOOLEAN FirstSlot;          // ""
    BOOLEAN AriEnabled;         // ARI is enabled on this bridge.
    BOOLEAN SlotImplemented;    // True if the bridge has slot implemented.
    BOOLEAN HotplugCapable;     // True if this bridge supports hotplug.
    ULONG ExpressCaps;          // Offset to express capabilities
    UCHAR ActualPrimary;        // Actual primary bus number of this bridge.
    UCHAR ActualSecondary;      // Actual secondary bus number of this bridge.
    PPCI_BRIDGE_REQUIREMENTS Requirements; // Holder for bridge requirements.
    BOOLEAN IsThunderBolt;      // True if the device is a thunderbolt device.
} BRIDGE, *PBRIDGE;

typedef enum _PCI_TRAVERSE_BRIDGE_DIRECTION {
    ToParent,
    ToChild
} PCI_TRAVERSE_BRIDGE_DIRECTION;

// ------------------------------------------------------------------ Prototypes

VOID
PciSpreadBridgesHotplug(
    _In_ PPCI_BUS Bus
    );

VOID
PciResetSlotEnum(
    _In_ PBRIDGE Bridge
    );

BOOLEAN
PciNextPopulatedSlot(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge
    );

VOID
PciAllocBridgeSubtree(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PPCI_BUS Bus,
    _In_ PBRIDGE Parent,
    _In_ UCHAR MaxSubordinate
    );

VOID
PciAssignBridgeSubtree(
    _In_ PBRIDGE Parent,
    _In_ UCHAR BusNumsPerHotplugPort,
    _In_ UCHAR BusNumsPerHotplugPortRemainder,
    _In_ UCHAR MaxSubordinate
    );

VOID
PciCommitBridgeSubtree(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Parent
    );

VOID
PciFreeBridgeSubtree(
    _In_ PBRIDGE Parent
    );

VOID
PciUpdateAncestorSubordinateBuses(
    _In_ PPCI_BUS Bus,
    _In_ UCHAR Subordinate
    );

PBRIDGE
PciAllocateBridge(
    VOID
    );

VOID
PciAddBridgeWindowRequirement(
    _In_ PPCI_BRIDGE_WINDOW_REQUIREMENT Child,
    _Inout_ PPCI_BRIDGE_WINDOW_REQUIREMENT Parent
    );

VOID
PciAddBridgeWindowRequirements(
    _In_ PBRIDGE Child,
    _Inout_ PBRIDGE Parent
    );

VOID
PciAddChildRequirementsToBridgeRequirements(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR HeaderType,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS BridgeRequirements
    );

VOID
PciSaveBridgeRequirements(
    _In_ PBRIDGE Parent,
    _In_ PPCI_SEGMENT Segment
    );

VOID
PciOverrideRequirement(
    _In_ PPCI_BRIDGE_WINDOW_REQUIREMENT Override,
    _Inout_ PPCI_BRIDGE_WINDOW_REQUIREMENT Requirement
    );

VOID
PciAdjustBridgeRequirements(
    _Inout_ PBRIDGE Bridge
    );

ULONG PciValidateBridgeConfig(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge,
    _In_ ULONG Indent
    );

VOID
PciValidateBridgeConfiguration(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciSpreadBridgesHotplug)
    #pragma alloc_text(PAGE, PciResetSlotEnum)
    #pragma alloc_text(PAGE, PciNextPopulatedSlot)
    #pragma alloc_text(PAGE, PciAllocBridgeSubtree)
    #pragma alloc_text(PAGE, PciAssignBridgeSubtree)
    #pragma alloc_text(PAGE, PciCommitBridgeSubtree)
    #pragma alloc_text(PAGE, PciFreeBridgeSubtree)
    #pragma alloc_text(PAGE, PciUpdateAncestorSubordinateBuses)
    #pragma alloc_text(PAGE, PciAllocateBridge)
    #pragma alloc_text(PAGE, PciAddBridgeWindowRequirement)
    #pragma alloc_text(PAGE, PciAddBridgeWindowRequirements)
    #pragma alloc_text(PAGE, PciOverrideRequirement)
    #pragma alloc_text(PAGE, PciOverrideBridgeRequirements)
    #pragma alloc_text(PAGE, PciAdjustBridgeRequirements)
    #pragma alloc_text(PAGE, PciAddChildRequirementsToBridgeRequirements)
    #pragma alloc_text(PAGE, PciSaveBridgeRequirements)
    #pragma alloc_text(PAGE, PciValidateBridgeConfig)
    #pragma alloc_text(PAGE, PciValidateBridgeConfiguration)

#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

VOID
PciSpreadBridgesHotplug(
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine spreads out the available bus numbers between
    unconfigured bridges.  It is only called if ALL the bridges on a particular
    bus are not configured - eg we just hot docked!

Arguments:

    Bus - The bus extension for the bridge we are enumerating.

Return Value:

    None

--*/

{

    PBRIDGE Bridge;
    ULONG BusNumsAvailable;
    ULONG BusNumsLeftover;
    ULONG BusNumsNeeded;
    UCHAR BusNumsPerHotplugPort;
    UCHAR BusNumsPerHotplugPortRemainder;
    PPCI_CHILDREN_BUSNUM_HINT ChildBusNumHint;
    PPCI_DEVICE Device;
    PLIST_ENTRY Entry;
    ULONG HotplugPorts;
    UCHAR MaxSubordinate;
    ULONG NextSetBit;
    PPCI_BUS_INTERFACE_STANDARD PciIF;
    UCHAR Primary;
    UCHAR Secondary;
    UCHAR Subordinate;

    PAGED_CODE();

    PciIF = Bus->Root->PciBusInterface;

    PCI_ASSERT(LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber) <= PCI_MAX_BRIDGE_NUMBER);

    Secondary = (UCHAR)LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber);

    //
    // The limit is constrained by the next configured bridge underneath this
    // root bus.  The secondary bus numbers of every bridge underneath this
    // root are stored in the root bridge map.  Find the first bit
    // set in this bitmap beyond this bus's Subordinate bus number.
    // (Actually get one bit earlier than this, since the first set bit is
    // the secondary bus of another bridge.  We can
    // only consume bus numbers up to the bus number before this.)
    //

    if (LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber) < PCI_MAX_BRIDGE_NUMBER) {
        NextSetBit = RtlFindSetBits(
                        &Bus->Root->BridgeMap,
                        1,
                        LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber) + 1);

        //
        // If the next set bit is not found, use the bus's subordinate.
        //

        if (NextSetBit == -1) {
            MaxSubordinate = (UCHAR)Bus->SubordinateBusNumber;
        } else {
            MaxSubordinate = (UCHAR)NextSetBit;

            PCI_ASSERT(MaxSubordinate > 1);

            MaxSubordinate -= 1;
        }

    } else {
        MaxSubordinate = PCI_MAX_BRIDGE_NUMBER;
    }

    TRACE(BUS_NUMBER_TRACE_LEVEL,
          "PciSpreadBridgesHotplug: Spreading bridges between 0x%x and 0x%x\n",
          Secondary,
          MaxSubordinate
          );

    if (Secondary > MaxSubordinate) {

        //
        // This normally means the BIOS or HAL messed up and got the subordinate
        // bus number for the root bus wrong.  There's not much we can do..
        //

        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "PciSpreadBridgesHotplug: Error: bus number limit %d < base %d\n",
              Secondary,
              MaxSubordinate);

        PCI_ASSERT(Secondary <= MaxSubordinate);

        return;

    } else if (Secondary == MaxSubordinate) {

        //
        // We don't have any bus numbers available - bail now
        //

        TRACE(BUS_NUMBER_TRACE_LEVEL, "PciSpreadBridgesHotplug: Error: out of bus numbers\n");
        return;
    }

    //
    // The bus should be unconfigured, hence no hint should be present yet.
    //

    if (!IsListEmpty(&Bus->ChildrenBusNumHints)) {

        PCI_ASSERT(FALSE);

        while ((!IsListEmpty(&Bus->ChildrenBusNumHints))) {
            Entry = Bus->ChildrenBusNumHints.Flink;
            RemoveEntryList(Entry);
            ChildBusNumHint = CONTAINING_RECORD(Entry,
                                                PCI_CHILDREN_BUSNUM_HINT,
                                                ListEntry);

            PciFreePool(ChildBusNumHint);
        }
    }

    //
    // Mock up a bridge to sit at the root of the hierarchy.
    //

    Bridge = PciAllocateBridge();
    if (Bridge == NULL) {
        return;
    }

    InitializeListHead(&Bridge->Children);
    Bridge->Parent = NULL;
    Bridge->ChildCount = 0;
    Bridge->BusNumCount = 1;
    Bridge->HotplugCount = 0;
    Bridge->DevicesPresent = 0;
    Bridge->Secondary = Secondary;
    Bridge->Subordinate = MaxSubordinate;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    //
    // The PCI root bus doesn't have a parent, nor is it
    // a PciBridge.
    //

    if (PCI_ROOT_BUS(Bus) == FALSE) {
        Bridge->Slot = Bus->PciBridge->Slot;
        Bridge->Primary = (UCHAR)Bus->ParentBus->SecondaryBusNumber;
        Bridge->ActualPrimary = Bridge->Primary;
        Bridge->ActualSecondary = Bridge->Secondary;
        Bridge->RootBus = FALSE;
        Bridge->HeaderType = PciReadHeaderTypeRaw(PciIF,
                                                  Bridge->Primary,
                                                  Bridge->Slot);

        Bridge->HeaderType &= ~PCI_MULTIFUNCTION;

        Bridge->IsThunderBolt = PciIsDeviceThunderboltRaw(PciIF,
                                                          Bridge->Primary,
                                                          Bridge->Slot);

        //
        // Update the bridges above to ensure access during enumeration.
        //

        PciUpdateAncestorSubordinateBuses(Bus, MaxSubordinate);

        //
        // Update the bridge being enumerated to open it up as wide as possible.
        //

        PciWriteBusNumsRaw(PciIF,
                           Bridge->Primary,
                           Bridge->Slot,
                           Bridge->Primary,
                           Bridge->Secondary,
                           Bridge->Subordinate);

    } else {
        Bridge->IsThunderBolt = FALSE;
        Bridge->RootBus = TRUE;
    }

    //
    // Enumerate the tree under the bridge to determine how many
    // Bus numbers and hotplug ports are needed.
    //

    PciAllocBridgeSubtree(PciIF, Bus, Bridge, MaxSubordinate);

    //
    // Retrieve the data on how many bus numbers and hotplug ports were found.
    //

    BusNumsNeeded = Bridge->BusNumCount;
    HotplugPorts = Bridge->HotplugCount;
    BusNumsAvailable = MaxSubordinate - Secondary + 1;

    //
    // Calculate how many bus numbers are still available after accounting
    // for all the actual devices which must be satisfied.
    //

    if (BusNumsAvailable > BusNumsNeeded) {
        BusNumsLeftover = BusNumsAvailable - BusNumsNeeded;

    } else {
        BusNumsLeftover = 0;
    }

    //
    // Calculate how many additional bus numbers are available to be assigned to
    // each hotplug port. If no hotplug ports were found, just set the number
    // to 0. Also calculate the remainder, and split the remainder among the
    // first N hotplug ports until it's used up.
    //

    if (HotplugPorts > 0) {
        BusNumsPerHotplugPort = BusNumsLeftover / HotplugPorts;
        BusNumsPerHotplugPortRemainder = BusNumsLeftover % HotplugPorts;
    } else {
        BusNumsPerHotplugPort = 0;
        BusNumsPerHotplugPortRemainder = 0;
    }

    //
    // Calculate the ID assignments for each bridge. This doesn't change
    // any hardware; it just sets the primary/secondary/subordinate bus numbers
    // in each bridge structure to the desired values.
    //

    PciAssignBridgeSubtree(Bridge,
                           BusNumsPerHotplugPort,
                           BusNumsPerHotplugPortRemainder,
                           MaxSubordinate);

    //
    // Write the bus numbers through the hierarchy.
    //

    PciCommitBridgeSubtree(PciIF, Bridge);

    //
    // Validate that the bus numbers were written out successfully.
    //

    PciValidateBridgeConfiguration(PciIF, Bridge);

    //
    // Save each bridge resource requirements in the segment to be used later
    // as a hint to help allocate sufficient IO/Memory to the bridge.
    //

    PciSaveBridgeRequirements(Bridge, Bus->Root->Segment);

    //
    // Free the data structure representing the tree.
    //

    PciFreeBridgeSubtree(Bridge);

    //
    // Fixup data structures maintained by the PCI driver. This only needs to
    // be done for the topmost set of bridges, which were already created
    // earlier in PciProcessNewDevice before the bus numbers were assigned in
    // this function.
    //
    // When bridges deeper in the hierarchy are enumerated later,
    // PciProcessNewDevice will discover that their bus numbers were
    // already assigned and will perform an equivalent operation.
    //

    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        if ((Device->HeaderType == PCI_DEVICE_TYPE) || !Device->Present) {
            continue;
        }

        PciReadBusNumsRaw(PciIF,
                         Device->BusNumber,
                         Device->Slot,
                         &Primary,
                         &Secondary,
                         &Subordinate);

        Device->SavedRegisters.bridge.SecondaryBusNumber = Secondary;
        Device->SavedRegisters.bridge.SubordinateBusNumber = Subordinate;

        PciSetBitRangeInclusive(&Bus->BusNumberMap, Secondary, Subordinate);
        RtlSetBit(&Bus->Root->BridgeMap, Secondary);
        PCI_DEVICE_SET_FLAG(Device, BusNumbersConfigured);
        DebugClearDebugFlag(Device, PCI_BRIDGE_DISABLED);
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    return;
}

VOID
PciResetSlotEnum(
    _In_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine resets the slot enumeration for the bridge. It should be called
    once prior to enumerating slots under the bridge.

Arguments:

    Bridge - Provides a pointer to the bridge being enumerated.

Return Value:

    None

--*/

{

    PAGED_CODE();

    Bridge->FirstSlot = TRUE;
    Bridge->EnumSlot.u.bits.FunctionNumber = 0;
    Bridge->EnumSlot.u.bits.DeviceNumber = 0;
}

BOOLEAN
PciNextPopulatedSlot(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine is used to enumerate populated slots under a bridge. On each
    return from the function, the Bridge->EnumSlot field will be updated to
    reference a slot with a device or bridge plugged in.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bridge - Provides a pointer to the bridge being enumerated.

Return Value:

    TRUE if a Bridge->EnumSlot points to the next populated slot.
    FALSE if no other populated slots were found.

--*/

{
    BOOLEAN Found;
    ULONG Function;

    PAGED_CODE();

    //
    // Initialize Found so that on all calls other than the first the code
    // inside the loop knows that the previous slot was found (or else the
    // caller would not have called back in to get the next slot).
    //

    Found = (Bridge->FirstSlot == FALSE);
    do {

        if (Bridge->FirstSlot == FALSE) {

            //
            // Bridge->EnumSlot contains the slot of the previously found device.
            // Increment to the next slot.
            //

            if (Bridge->AriEnabled != FALSE) {

                //
                // If ARI was when dev/fn 0 was enumerated, get the next slot
                // by examining the ARI capability of the previous slot. This
                // will provide an 8-bit function number from 0-255, where 0 is
                // the terminating condition.
                //

                Function = PciReadNextARIFunctionRaw(PciIF,
                                                     Bridge->Secondary,
                                                     Bridge->EnumSlot);

                if (Function == 0) {
                    Found = FALSE;
                    break;
                }

                //
                // Store the ARI function ID in the slot structure,
                // splitting the 8-bit function between the 5-bit device and
                // 3-bit function fields.
                //

                SET_ARI_SLOT(&Bridge->EnumSlot, Function);

            } else if ((Bridge->EnumSlot.u.bits.FunctionNumber == 0) &&
                       ((Found == FALSE) ||
                        (!PciIsDeviceMultifunction(PciIF, Bridge->Secondary, Bridge->EnumSlot)))) {

                //
                // If the last slot enumerated was function 0, there are two
                // cases where we should skip to the next device instead of
                // just incrementing the function:
                //
                // 1. Function 0 was not found. Function 0 must be implemented
                //    for each device, so if it was not found then the
                //    entire device must not be present. Note that otherwise
                //    functions need not be contiguous (e.g. a device may
                //    implement only functions {0, 2, 6}).
                //
                // 2. The device was not a multifunction device. Only
                //    multifunction devices implement functions beyond 0.
                //

                Bridge->EnumSlot.u.bits.DeviceNumber += 1;
                if (Bridge->EnumSlot.u.bits.DeviceNumber == 0) {
                    Found = FALSE;
                    break;
                }

            } else {

                //
                // Increment the function, and on overflow increment the device.
                // Note that the code relies on wraparound to 0 to detect overflow.
                //

                Bridge->EnumSlot.u.bits.FunctionNumber += 1;
                if (Bridge->EnumSlot.u.bits.FunctionNumber == 0) {
                    Bridge->EnumSlot.u.bits.DeviceNumber += 1;
                    if (Bridge->EnumSlot.u.bits.DeviceNumber == 0) {
                        Found = FALSE;
                        break;
                    }
                }
            }
        }

        //
        // Look for the device.
        //

        Found = PciIsDevicePresentRaw(PciIF, Bridge->Secondary, Bridge->EnumSlot);

        //
        // If the device was found, and it was the first slot (dev==0/fn==0),
        // check to see if ARI should be used to enumerate additional functions.
        //
        // PciTryEnableBridgeARIRaw will take the following steps:
        // 1. Determine if the device supports the ARI capability.
        // 2. Determine if the parent bridge supports ARI forwarding.
        // 3. If conditions 1 & 2 are met, enable ARI forwarding in the parent
        //    bridge and return TRUE.
        //

        if ((Found != FALSE) && (Bridge->FirstSlot != FALSE)) {
            Bridge->AriEnabled = PciTryEnableBridgeARIRaw(PciIF,
                                                          Bridge->Secondary,
                                                          Bridge->EnumSlot,
                                                          Bridge->Primary,
                                                          Bridge->Slot);
        }

        Bridge->FirstSlot = FALSE;
    } while (Found == FALSE);

    return Found;
}

VOID
PciAllocBridgeSubtree(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PPCI_BUS Bus,
    _In_ PBRIDGE Parent,
    _In_ UCHAR MaxSubordinate
    )

/*++

Routine Description:

    This routine searches an unconfigured subtree of the PCI hierarchy below a
    specific bus to determine how many bus numbers are required for all
    populated devices, and also the number of unpopulated hotplug ports.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides a pointer to the bus structure.

    Parent - Provides a pointer to the topmost bridge being enumerated.

    MaxSubordinate - Provides the bus number of the maximum subordinate bus available.

Return Value:

    None.

--*/

{

    PBRIDGE Child;
    PPCI_CHILDREN_BUSNUM_HINT ChildBusNumHint;
    PBRIDGE CurrentBridge;
    BOOLEAN DecreaseParentDevice;
    PCI_TRAVERSE_BRIDGE_DIRECTION Direction;
    BOOLEAN NotDone;
    PBRIDGE ParentBridge;
    UCHAR HeaderType;

    PAGED_CODE();

    NotDone = TRUE;
    DecreaseParentDevice = FALSE;
    Child = NULL;
    ParentBridge = NULL;
    CurrentBridge = Parent;
    Direction = ToChild;

    while (NotDone != FALSE) {
        switch (Direction) {
        case ToChild:

            //
            // Normal traversal case, will be handled later.
            //

            break;

        default:
            if (CurrentBridge == Parent) {

                //
                // The loop finishes whenever returning to the starting point
                // of the traversal (which is "parent" bridge).
                //

                return;
            } else {

                //
                // The traversal at one of the children has finished,
                // go back up one level and change the direction to ToChild to
                // continue on the next child.
                //

                if (DecreaseParentDevice != FALSE) {
                    PciFreePool(CurrentBridge);
                }

                CurrentBridge = ParentBridge;
                ParentBridge = CurrentBridge->Parent;
                Direction = ToChild;
            }
        }

        if (CurrentBridge->Secondary >= MaxSubordinate) {

            //
            // If the secondary bus has reached the maximum, terminate the
            // search now and don't continue on any child. Set DevicePresent to
            // 1 here to make sure the device will not be ignored.
            //

            CurrentBridge->DevicesPresent = 1;
            goto UpdateParent;
        }

        //
        // Only do the following initialization once. If a bridge has children,
        // at the second time it reaches here, the DevicesPresent count
        // will not be 0. If a bridge has no child, it will not reach here
        // twice because the above code will move the traversal up one level.
        //

        if (CurrentBridge->DevicesPresent == 0) {
            if (CurrentBridge->RootBus == FALSE) {
                CurrentBridge->ExpressCaps = PciFindCapOffsetRaw(
                                                PciIF,
                                                CurrentBridge->Primary,
                                                CurrentBridge->Slot,
                                                PCI_CAPABILITY_ID_PCI_EXPRESS,
                                                NULL,
                                                NULL);

                CurrentBridge->SlotImplemented = PciIsDeviceSlotImplementedRaw(
                                                    PciIF,
                                                    CurrentBridge->Primary,
                                                    CurrentBridge->Slot,
                                                    CurrentBridge->ExpressCaps);

                CurrentBridge->HotplugCapable = PciIsDeviceHotplugCapableRaw(
                                                    PciIF,
                                                    CurrentBridge->Primary,
                                                    CurrentBridge->Slot,
                                                    CurrentBridge->ExpressCaps);
            } else {
                CurrentBridge->HotplugCapable = 0;
            }

            PciResetSlotEnum(CurrentBridge);
        }

        //
        // If DecreaseParentDevice is true, this means the last enumerated child
        // of the current bridge is ignored because it has no slot implemented
        // and is not hotplug capable. In this case, decrease the device count
        // of the current bridge by 1 as the child is ignored.
        //

        if (DecreaseParentDevice != FALSE) {
            CurrentBridge->DevicesPresent -= 1;

            PCI_ASSERT(CurrentBridge->DevicesPresent >= 0);

            DecreaseParentDevice = FALSE;
        }

        //
        // Traverse on children of this bridge looking for other bridges. If no
        // more child is found, go up one level.
        //

        if (PciNextPopulatedSlot(PciIF, CurrentBridge) == FALSE) {

            //
            // If there was nothing under the bridge, see if it supports hotplug.
            // If so, add to the HotPlugCount total. Once actual devices are
            // satisfied, any excess bus numbers will be distributed to bridges that
            // are hotplug capable.
            //
            // N.B. In the interest of conserving bus numbers for ports which
            //      have no devices plugged in, this underfunds bus numbers to ports
            //      which already have a device installed. The risk of this choice is
            //      that one may have no available bus numbers in the scenario where a
            //      user hot-removes a device and replaces it with a device which
            //      requires more bus numbers. If this turns out to be a problem,
            //      one can eliminate it by removing the check for
            //      (Parent->DevicesPresent == 0), so we allocate extra bus numbers
            //      for all hotplug ports.
            //

            if ((CurrentBridge->DevicesPresent == 0) &&
                (CurrentBridge->HotplugCapable != FALSE)) {

                CurrentBridge->HotplugCount += 1;
            }

            //
            // All the bridge resource requirements have now been calculated.
            // Adjust as necessary for alignment here.
            //

            PciAdjustBridgeRequirements(CurrentBridge);

            //
            // Update parent's information.
            //

UpdateParent:

            if (ParentBridge != NULL) {
                PciWriteBusNumsRaw(
                    PciIF,
                    CurrentBridge->Primary,
                    CurrentBridge->Slot,
                    0,
                    0,
                    0);

                //
                // Allocate an auxiliary data structure that remembers the
                // secondary/subordinate bus numbers that are assigned to the
                // current bridge. This will be used later during PciScanBus to
                // determine whether the bridge has child or is hotplug capable,
                // thus determines whether the PDO of the bridge will be created.
                //

                ChildBusNumHint = PciAllocatePool(PagedPool,
                                                  sizeof(PCI_CHILDREN_BUSNUM_HINT));

                if (ChildBusNumHint == NULL) {
                    PCI_ASSERT(FALSE);

                } else {
                    RtlZeroMemory(ChildBusNumHint, sizeof(PCI_CHILDREN_BUSNUM_HINT));
                    ChildBusNumHint->BusNumber = CurrentBridge->ActualPrimary;
                    ChildBusNumHint->Slot.u.AsULONG = CurrentBridge->Slot.u.AsULONG;
                }

                //
                // If the bridge has no child, nor is it hotplug capable, and
                // in addition is a Thunderbolt device, do not enumerate this
                // bridge and do not assign bus number to it.
                //

                if ((CurrentBridge->DevicesPresent == 0) &&
                    (CurrentBridge->SlotImplemented == 0) &&
                    (CurrentBridge->HotplugCount == 0) &&
                    (CurrentBridge->IsThunderBolt != FALSE)) {

                    DecreaseParentDevice = TRUE;
                    if (ChildBusNumHint != NULL) {
                        ChildBusNumHint->Secondary = 0;
                        ChildBusNumHint->Subordinate = 0;
                    }

                } else {
                    if (ChildBusNumHint != NULL) {
                        ChildBusNumHint->Secondary =
                            CurrentBridge->ActualSecondary;

                        ChildBusNumHint->Subordinate =
                            ChildBusNumHint->Secondary + CurrentBridge->BusNumCount - 1;
                    }

                    ParentBridge->BusNumCount += CurrentBridge->BusNumCount;
                    ParentBridge->HotplugCount += CurrentBridge->HotplugCount;
                    ParentBridge->ChildCount += 1;

                    //
                    // Add the child's BAR resource requirements to the
                    // parent requirements.
                    //

                    PciAddBridgeWindowRequirements(CurrentBridge, ParentBridge);
                    InsertTailList(&ParentBridge->Children, &CurrentBridge->Link);
                }

                if (ChildBusNumHint != NULL) {
                    InsertTailList(&Bus->ChildrenBusNumHints,
                                   &ChildBusNumHint->ListEntry);
                }

                CurrentBridge->Primary = 0;
                CurrentBridge->Secondary = 0;
                CurrentBridge->Subordinate = 0;
            }

            Direction = ToParent;
            continue;
        }

        CurrentBridge->DevicesPresent += 1;

        //
        // Add the child's resource requirements to the parent requirements.
        // This adds requirements specified by the BARs in the
        // child (whether bridge or device). It does not look
        // at windows or recurse beneath the child bridge to determine what
        // additional resources are required underneath child bridges.
        // These additional resources will be added later on in the function.
        //

        HeaderType = PciReadHeaderTypeRaw(PciIF,
                                          CurrentBridge->Secondary,
                                          CurrentBridge->EnumSlot);

        HeaderType &= ~PCI_MULTIFUNCTION;
        PciAddChildRequirementsToBridgeRequirements(PciIF,
                                                    CurrentBridge->Secondary,
                                                    CurrentBridge->EnumSlot,
                                                    HeaderType,
                                                    CurrentBridge->Requirements);

        if ((HeaderType != PCI_BRIDGE_TYPE) &&
            (HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {

            //
            // The child is a device (not a bridge).
            //
            // If the device supports SR-IOV, PciSriovBusNumbersRaw will return
            // the number of additional buses (worst-case) to address all VFs
            // implemented in the device.
            //

            CurrentBridge->BusNumCount += PciSriovBusNumbersRaw(
                                            PciIF,
                                            CurrentBridge->Secondary,
                                            CurrentBridge->EnumSlot,
                                            CurrentBridge->AriEnabled);

            //
            // Continue normal traversal on the current level, keep CurrentBridge
            // unchanged and Direction still ToChild.
            //

            continue;
        }

        if (!PciIsUnconfiguredBridge(PciIF,
                                     CurrentBridge->Secondary,
                                     CurrentBridge->EnumSlot)) {

            PCI_ASSERT(FALSE);

            //
            // If this edge case happens, terminate traversal.
            //

            goto UpdateParent;
        }

        //
        // Allocate a new bridge structure and initialize.
        //

        Child = PciAllocateBridge();
        if (Child == NULL) {

            PCI_ASSERT(FALSE);

            //
            // Terminate searching on this child, continue on other children.
            //

            continue;
        }

        Child->Parent = CurrentBridge;
        Child->ActualPrimary = CurrentBridge->ActualSecondary;
        Child->ActualSecondary = CurrentBridge->ActualSecondary + CurrentBridge->BusNumCount;
        Child->ChildCount = 0;
        Child->BusNumCount = 1;
        Child->HotplugCount = 0;
        Child->DevicesPresent = 0;
        Child->HeaderType = HeaderType;
        Child->Primary = CurrentBridge->Secondary;
        Child->RootBus = FALSE;
        Child->Secondary = Child->Primary + 1;
        Child->Subordinate = MaxSubordinate;
        Child->Slot = CurrentBridge->EnumSlot;
        Child->IsThunderBolt = PciIsDeviceThunderboltRaw(PciIF,
                                                         Child->Primary,
                                                         Child->Slot);

        InitializeListHead(&Child->Children);

        //
        // Set the bridge's bus numbers, recurse underneath, and then restore them.
        //

        PciWriteBusNumsRaw(PciIF,
                           Child->Primary,
                           Child->Slot,
                           Child->Primary,
                           Child->Secondary,
                           Child->Subordinate);

        //
        // Start on the child.
        //

        ParentBridge = CurrentBridge;
        CurrentBridge = Child;
    }
}

VOID
PciAssignBridgeSubtree(
    _In_ PBRIDGE Parent,
    _In_ UCHAR BusNumsPerHotplugPort,
    _In_ UCHAR BusNumsPerHotplugPortRemainder,
    _In_ UCHAR MaxSubordinate
    )

/*++

Routine Description:

    This routine assigns primary, secondary, and subordinate ids to each bridge.
    Note that this only updates the data structures.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Parent - Provides a pointer to the parent or topmost bridge.

    BusNumsPerHotplugPort - Provides the count of bus numbers to be allocated to
        each hotplug port.

    BusNumsPerHotplugPortRemainder - Provides the count of leftover bus numbers
        to be allocated to hotplug ports. One per port is allocated until the
        remainder is exhausted.

Return Value:

    None.

--*/

{
    UCHAR BusNumsRemainder;
    ULONG BusNumsToAllocate;
    PBRIDGE Child;
    PLIST_ENTRY Entry;
    ULONG Secondary;
    ULONG Subordinate;

    PAGED_CODE();

    Secondary = Parent->Secondary + 1;
    for (Entry = Parent->Children.Flink;
         Entry != &Parent->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        BusNumsToAllocate = Child->BusNumCount;
        BusNumsToAllocate += (Child->HotplugCount * BusNumsPerHotplugPort);
        if (BusNumsPerHotplugPortRemainder > Child->HotplugCount) {
            BusNumsRemainder = Child->HotplugCount;

        } else {
            BusNumsRemainder = BusNumsPerHotplugPortRemainder;
        }

        BusNumsToAllocate += BusNumsRemainder;
        BusNumsPerHotplugPortRemainder -= BusNumsRemainder;
        Subordinate = Secondary + BusNumsToAllocate - 1;

        //
        // If the secondary bus number exceeds the max subordinate,
        // there's no way to configure this bridge. Just return,
        // leaving the bridge in an unconfigured state.
        // (Pri/Sec/Sub == 0)
        //

        if (Secondary > MaxSubordinate) {
            return;
        }

        //
        // If the subordinate bus number exceeds the max subordinate,
        // cap it to be no greater than the maximum, and adjust the
        // number to allocate appropriately.
        //

        if (Subordinate > MaxSubordinate) {
            Subordinate = MaxSubordinate;
            BusNumsToAllocate = Subordinate - Secondary + 1;
        }

        PCI_ASSERT(BusNumsToAllocate > 0);

        Child->Primary = Parent->Secondary;
        Child->Secondary = (UCHAR)Secondary;
        Child->Subordinate = (UCHAR)Subordinate;
        if ((Child->Secondary <= Child->Primary) ||
            (Child->Subordinate < Child->Secondary)) {

            PCI_ASSERT(FALSE);

            return;
        }

        PciAssignBridgeSubtree(Child,
                               BusNumsPerHotplugPort,
                               BusNumsRemainder,
                               MaxSubordinate);

        Secondary += BusNumsToAllocate;
    }

    return;
}

VOID
PciCommitBridgeSubtree(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Parent
)

/*++

Routine Description:

    This routine updates the primary, secondary, and subordinate ids in each bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Parent - Provides a pointer to the topmost bridge.

Return Value:

    None.

--*/

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;

    PAGED_CODE();

    for (Entry = Parent->Children.Flink;
         Entry != &Parent->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        PciWriteBusNumsRaw(PciIF,
                           Child->Primary,
                           Child->Slot,
                           Child->Primary,
                           Child->Secondary,
                           Child->Subordinate);

        PciCommitBridgeSubtree(PciIF, Child);
    }

    return;
}

VOID
PciFreeBridgeSubtree(
    _In_ PBRIDGE Parent
    )

/*++

Routine Description:

    This routine frees memory used by the temporary bridge hierarchy used to
    configure bus numbers.

Arguments:

    Parent - Provides a pointer to the topmost bridge.

Return Value:

    None.

--*/

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;

    PAGED_CODE();

    while ((Entry = Parent->Children.Flink) != &Parent->Children) {
        RemoveEntryList(Entry);
        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        PciFreeBridgeSubtree(Child);
    }

    PciFreePool(Parent);
    return;
}

PBRIDGE
PciAllocateBridge(
    VOID
    )

/*++

Routine Description:

    This routine allocates a bridge structure and a bridge requirements
    structure attached to the bridge.

    N.B. The requirements structure is allocated separately from the bridge
    structure so that it can be kept around and used to size the bridge BARs
    long after enumeration has completed and the BRIDGE has been freed.

Arguments:

    None.

Return Value:

    Pointer to a BRIDGE structure, or NULL on error.

--*/

{

    PBRIDGE Bridge;
    PPCI_BRIDGE_REQUIREMENTS Requirements;

    PAGED_CODE();

    Bridge = PciAllocatePool(PagedPool, sizeof(BRIDGE));
    if (Bridge == NULL) {
        return NULL;
    }

    Requirements = PciAllocatePool(PagedPool, sizeof(PCI_BRIDGE_REQUIREMENTS));
    if (Requirements == NULL) {
        PciFreePool(Bridge);
        return NULL;
    }

    RtlZeroMemory(Bridge, sizeof(BRIDGE));
    RtlZeroMemory(Requirements, sizeof(PCI_BRIDGE_REQUIREMENTS));
    Bridge->Requirements = Requirements;
    return Bridge;
}

VOID
PciAddBridgeWindowRequirement(
    _In_ PPCI_BRIDGE_WINDOW_REQUIREMENT Child,
    _Inout_ PPCI_BRIDGE_WINDOW_REQUIREMENT Parent
    )

/*++

Routine Description:

    This routine adds one child bridge window requirement to
    its parent window requirement.

Arguments:

    Child - Provides a pointer to the child requirement.

    Parent - Provides a pointer to the parent requirement.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    Parent->Size += Child->Size;
    if (Parent->Alignment < Child->Alignment) {
        Parent->Alignment = Child->Alignment;
    }
}

VOID
PciAddBridgeWindowRequirements(
    _In_ PBRIDGE Child,
    _Inout_ PBRIDGE Parent
    )

/*++

Routine Description:

    This routine adds each child bridge window requirements to
    its parent window requirements.

Arguments:

    Child - Provides a pointer to the child bridge structure.

    Parent - Provides a pointer to the parent bridge structure.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PciAddBridgeWindowRequirement(&Child->Requirements->IoWindow,
                                  &Parent->Requirements->IoWindow);

    PciAddBridgeWindowRequirement(&Child->Requirements->MemoryWindow,
                                  &Parent->Requirements->MemoryWindow);

    PciAddBridgeWindowRequirement(&Child->Requirements->PrefetchWindow,
                                  &Parent->Requirements->PrefetchWindow);

}

VOID
PciAddChildRequirementsToBridgeRequirements(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ ULONG Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ UCHAR HeaderType,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS BridgeRequirements
    )

/*++

Routine Description:

    This routine examines the resource requirements of a device and updates the
    BridgeRequirements structure to include those requirements.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bus - Provides the bus number.

    Slot - Provides the device/function number.

    HeaderType - Provides the HeaderType field of the device.

    BridgeRequirements - Provides a pointer to the BridgeRequirements to be updated.

Return Value:

    None.

--*/

{

    BOOLEAN Bar64Bit;
    ULONG BarCount;
    ULONG BarIndex;
    ULONG BarOffset;
    IO_RESOURCE_DESCRIPTOR BarRequirement;
    ULONG ProbedBar[PCI_TYPE0_BAR_COUNT+1];
    ULONG SavedBar[PCI_TYPE0_BAR_COUNT];
    NTSTATUS Status;
    PPCI_BRIDGE_WINDOW_REQUIREMENT Window;

    PAGED_CODE();

    //
    // Note: SavedBar and ProbedBar are sized to read the largest number of
    // BAR registers supported by any PCI/PCIe device (type 0). Also,
    // ProbedBar has one additional entry to allow the last field to be
    // initialized to 0 to simplify the possible error case where the last
    // BAR reports itself as 64-bit.
    //

    //
    // Calculate the actual count and offset of the BAR registers based on the
    // bridge type. Note that TYPE2 bridges do not have any BAR registers.
    //

    switch (HeaderType) {
    case PCI_DEVICE_TYPE:
        BarCount = PCI_TYPE0_BAR_COUNT;
        BarOffset = CONFIG_FIELD_OFFSET(type0.BaseAddresses);
        break;

    case PCI_BRIDGE_TYPE:
        BarCount = PCI_TYPE1_BAR_COUNT;
        BarOffset = CONFIG_FIELD_OFFSET(type1.BaseAddresses);
        break;

    default:
        return;
    }

    //
    // Probe the BARs for this device. On return SavedBar will contain
    // the original BAR registers, and ProbedBar will contain the results
    // of writing all 1's to the BARs and reading them back.
    //

    PciProbeConfigRegistersRaw(PciIF,
                               Bus,
                               Slot,
                               SavedBar,
                               ProbedBar,
                               BarOffset,
                               BarCount * sizeof(ULONG));

    //
    // Set the entry after the last to 0 so that PciRequirementFromBar
    // is passed a 0 value for ProbedBar[BarIndex+1] on the last BAR.
    //

    ProbedBar[BarCount] = 0;
    for (BarIndex = 0; BarIndex < BarCount; BarIndex++) {
        RtlZeroMemory(&BarRequirement, sizeof(IO_RESOURCE_DESCRIPTOR));
        Status = PciRequirementFromBar(ProbedBar[BarIndex],
                                       ProbedBar[BarIndex+1],
                                       &BarRequirement,
                                       &Bar64Bit
                                       );

        if (!NT_SUCCESS(Status)) {
            return;
        }

        if (Bar64Bit != 0) {
            BarIndex += 1;
        }

        //
        // Examine the Bar Requirement to determine which bridge window to update.
        //

        switch (BarRequirement.Type) {
        case CmResourceTypeMemory:
        case CmResourceTypeMemoryLarge:
            if (BarRequirement.Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
                Window = &BridgeRequirements->PrefetchWindow;

            } else {
                Window = &BridgeRequirements->MemoryWindow;
            }

            Window->Size += BarRequirement.u.Memory.Length;
            if (Window->Alignment < BarRequirement.u.Memory.Alignment) {
                Window->Alignment = BarRequirement.u.Memory.Alignment;
            }

            break;

        case CmResourceTypePort:
            Window = &BridgeRequirements->IoWindow;
            Window->Size += BarRequirement.u.Port.Length;
            if (Window->Alignment < BarRequirement.u.Port.Alignment) {
                Window->Alignment = BarRequirement.u.Port.Alignment;
            }

            break;
        }
    }
}

VOID
PciSaveBridgeRequirements(
    _In_ PBRIDGE Bridge,
    _In_ PPCI_SEGMENT Segment
)

/*++

Routine Description:

    This routine updates the requirement override hints. Hints are stored
    in an array attached to the Segment, and are indexed by the secondary
    bus number below the bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Parent - Provides a pointer to the topmost bridge.

Return Value:

    None.

--*/

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;
    PPCI_BRIDGE_REQUIREMENTS *Requirement;

    PAGED_CODE();

    //
    // If requirements had previously been saved for this bridge,
    // discard them now.
    //

    Requirement = &Segment->BridgeRequirementOverride[Bridge->Secondary];
    if (*Requirement != NULL) {
        PciFreePool(*Requirement);
    }

    //
    // Save the requirements for this bridge and clear the Bridge requirements
    // pointer.
    //

    *Requirement = Bridge->Requirements;
    Bridge->Requirements = NULL;

    //
    // Recurse and save requirements for any child bridges.
    //

    for (Entry = Bridge->Children.Flink;
         Entry != &Bridge->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        PciSaveBridgeRequirements(Child, Segment);
    }

    return;
}

VOID
PciOverrideRequirement(
    _In_ PPCI_BRIDGE_WINDOW_REQUIREMENT Override,
    _Inout_ PPCI_BRIDGE_WINDOW_REQUIREMENT Requirement
    )

/*++

Routine Description:

    This routine overrides one bridge requirement.

Arguments:

    Override - Provides a pointer to the resource requirements found during hotplug enumeration.

    Preferred - Provides a pointer to the requirement to update.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // If the requirement is disabled, don't update it.
    //

    if (Requirement->Disabled != FALSE) {
        return;
    }

    //
    // If the requirement is less than the override, use the override.
    //

    if (Requirement->Size < Override->Size) {
        Requirement->Size = Override->Size;
    }

    //
    // If the alignment is less than the override, use the override.
    //

    if (Requirement->Alignment < Override->Alignment) {
        Requirement->Alignment = Override->Alignment;
    }

    return;
}

VOID
PciOverrideBridgeRequirements(
    _In_ PPCI_BRIDGE Bridge,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Preferred,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Fallback
    )

/*++

Routine Description:

    This routine looks for resource requirements which may have been determined
    during hotplug enumeration and, if found, updates the Preferred bridge
    requirements if more resources are needed than the default allocation.

Arguments:

    Bridge - Provides a pointer to bridge being configured.

    Preferred - Provides a pointer to the Preferred resource requirements of
        the bridge.

Return Value:

    None.

--*/

{

    PPCI_BRIDGE_REQUIREMENTS Override;
    UCHAR SecondaryBusNumber;
    PPCI_SEGMENT Segment;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Fallback);

    //
    // See if any resource requirements were stored for this bridge.
    //

    SecondaryBusNumber = LOCAL_BUS_NUMBER(Bridge->Bus->SecondaryBusNumber);
    Segment = Bridge->Bus->Root->Segment;
    Override = Segment->BridgeRequirementOverride[SecondaryBusNumber];

    if (Override == NULL) {
        return;
    }

    //
    // Override each resoource requirement.
    //

    PciOverrideRequirement(&Override->IoWindow, &Preferred->IoWindow);
    PciOverrideRequirement(&Override->MemoryWindow, &Preferred->MemoryWindow);
    PciOverrideRequirement(&Override->PrefetchWindow, &Preferred->PrefetchWindow);

    return;
}

VOID
PciAdjustBridgeRequirements(
    _Inout_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine updates each of the bridge window requirements to round the
    size and alignment to alignment boundary appropriate to the bridge. This
    isn't strictly necessary, but might be useful if insufficient resources
    are preallocated to the bridge.

Arguments:

    Bridge - Provides a pointer to the bridge structure.

Return Value:

    None.

--*/

{

    ULONG IoWindowAlignment;
    ULONG MemoryWindowAlignment;
    PPCI_BRIDGE_REQUIREMENTS Requirements;

    PAGED_CODE();

    switch(Bridge->HeaderType) {
    case PCI_BRIDGE_TYPE:
        IoWindowAlignment = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        MemoryWindowAlignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:
        IoWindowAlignment = PCI_TYPE2_IO_WINDOW_ALIGNMENT;
        MemoryWindowAlignment = PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT;
        break;

    default:
        return;
    }

    Requirements = Bridge->Requirements;
    if (Requirements->IoWindow.Size > 0) {
        Requirements->IoWindow.Size =
            ALIGN_RANGE_UP(Requirements->IoWindow.Size,
                           IoWindowAlignment);

        if (Requirements->IoWindow.Alignment < IoWindowAlignment) {
            Requirements->IoWindow.Alignment = IoWindowAlignment;
        }
    }

    if (Requirements->MemoryWindow.Size > 0) {
        Requirements->MemoryWindow.Size =
            ALIGN_RANGE_UP(Requirements->MemoryWindow.Size,
                           MemoryWindowAlignment);

        if (Requirements->MemoryWindow.Alignment < MemoryWindowAlignment) {
            Requirements->MemoryWindow.Alignment = MemoryWindowAlignment;
        }
    }

    if (Requirements->PrefetchWindow.Size > 0) {
        Requirements->PrefetchWindow.Size =
            ALIGN_RANGE_UP(Requirements->PrefetchWindow.Size,
                           MemoryWindowAlignment);

        if (Requirements->PrefetchWindow.Alignment < MemoryWindowAlignment) {
            Requirements->PrefetchWindow.Alignment = MemoryWindowAlignment;
        }
    }
}

ULONG PciValidateBridgeConfig(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge,
    _In_ ULONG Indent
    )

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;
    ULONG NodeCount;
    UCHAR Primary;
    UCHAR Secondary;
    UCHAR Subordinate;

    PAGED_CODE();

    NodeCount = 1;

    if (Bridge->RootBus == FALSE) {
        PciReadBusNumsRaw(PciIF,
                          Bridge->Primary,
                          Bridge->Slot,
                          &Primary,
                          &Secondary,
                          &Subordinate);

        TRACE(BUS_NUMBER_TRACE_LEVEL | HOTPLUG_TRACE_LEVEL | RESOURCE_TRACE_LEVEL,
              "%*sBus:%d Dev:%d Fun:%d Sec:%d Sub:%d\n"
              "%*sIoWindow:       Size %#8I64x Align %#8I64x\n"
              "%*sMemoryWindow:   Size %#8I64x Align %#8I64x\n"
              "%*sPrefetchWindow: Size %#8I64x Align %#8I64x\n\n",
              Indent," ",
              Bridge->Primary,
              Bridge->Slot.u.bits.DeviceNumber,
              Bridge->Slot.u.bits.FunctionNumber,
              Bridge->Secondary,
              Bridge->Subordinate,
              Indent," ",
              Bridge->Requirements->IoWindow.Size,
              Bridge->Requirements->IoWindow.Alignment,
              Indent," ",
              Bridge->Requirements->MemoryWindow.Size,
              Bridge->Requirements->MemoryWindow.Alignment,
              Indent," ",
              Bridge->Requirements->PrefetchWindow.Size,
              Bridge->Requirements->PrefetchWindow.Alignment);

        //
        // If this bridge was configured successfully, validate
        // the primary/secondary/subordinate values.
        //

        if ((Bridge->Primary !=0) &&
            (Bridge->Secondary !=0) &&
            (Bridge->Subordinate != 0) &&
            ((Bridge->Secondary <= Bridge->Primary) ||
             (Bridge->Subordinate < Bridge->Secondary))) {

            PCI_ASSERT(FALSE);

            return NodeCount;
        }
    }

    for (Entry = Bridge->Children.Flink;
         Entry != &Bridge->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        NodeCount += PciValidateBridgeConfig(PciIF, Child, Indent+4);
    }

    return NodeCount;
}


VOID
PciValidateBridgeConfiguration(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine validates the bus numbers in each node of the hierarchy.
    To be used in debugging.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bridge - Provides a pointer to the topmost bridge.

Return Value:

    Number of bridge nodes in the hierarchy.

--*/

{

    ULONG Count;

    PAGED_CODE();

    Count = PciValidateBridgeConfig(PciIF, Bridge, 0);
    return;
}
