,
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

            Window->Size += BarRequ