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
    _In_ PBRIDGE Child