/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    resizablebar.c

Abstract:

    This module contains functions associated with configuring the PCI
    Express resizable bars.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

PPCI_RESIZABLE_BAR_INFO
PciResizableBarGetInfoPtr (
    __in PPCI_DEVICE Device
    );

// --------------------------------------------------------------------- Pragmas

#pragma alloc_text(PAGE, PciResizableBarInitialize)
#pragma alloc_text(PAGE, PciGetBarRequirementCount)
#pragma alloc_text(PAGE, PciPopulateBarRequirements)

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

PPCI_RESIZABLE_BAR_INFO
PciResizableBarGetInfoPtr (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This helper routine returns a pointer to the ResizableBar structure embedded
    in the ExpressPort or ExpressCompatibility port.

Parameters:

    Device - The PCI_DEVICE to be examined.

Return Value:

    Pointer to the ResizableBar structure, or NULL if the device doesn't include
    this structure.

--*/

{

    PPCI_RESIZABLE_BAR_INFO ResizableBar;

    NON_PAGED_CODE();

    if (Device->ExpressPort != NULL) {
        ResizableBar = &Device->ExpressPort->ResizableBar;
    } else if (Device->ExpressCompatibilityPort != NULL) {
        ResizableBar = &Device->ExpressCompatibilityPort->ResizableBar;
    } else
        ResizableBar = NULL;

    return ResizableBar;
}

NTSTATUS
PciResizableBarInitialize (
    __in PPCI_DEVICE Device,
    __in USHORT CapOffset
    )

/*++

Routine Description:

    This routine intializes the PCI Express device's Resizable BAR capabilities.

Arguments:

    ExpressPort - Supplies the Express device whose Resizable BAR capability will
        be processed.

    CapOffset - Supplies the offset to the Resizable BAR capability in PCIe
        configuration space.

Return Value:

    STATUS_SUCCESS on success.

--*/

{

    ULONG BarIndex;
    BYTE Index;
    ULONG NumberOfResizableBars;
    PPCI_RESIZABLE_BAR_INFO ResizableBar;
    PCI_EXPRESS_RESIZABLE_BAR_CAPABILITY ResizableBarCapability;
    ULONG SizesSupported;

    PAGED_CODE();

    ResizableBar = PciResizableBarGetInfoPtr(Device);
    if (ResizableBar == NULL) {
        return STATUS_SUCCESS;
    }

    ResizableBar->Capability = CapOffset;

    //
    // The count of ResizableBarControl registers varies from 1 to 6.
    // This count is provided in the NumberOfResizableBars field of the first
    // ResizableBarControl register.
    //

    //
    // Read the ResizableBarControl capability including the first
    // entry, but no other entries.
    //

    PciReadDeviceConfig(
        Device,
        &ResizableBarCapability,
        CapOffset,
        FIELD_OFFSET(PCI_EXPRESS_RESIZABLE_BAR_CAPABILITY, Entry[1]));

    NumberOfResizableBars =
        ResizableBarCapability.Entry[0].Control.NumberOfResizableBars;

    PCI_ASSERT((NumberOfResizableBars > 0) &&
               (NumberOfResizableBars <= PCI_RESIZABLE_BAR_COUNT_MAX));

    if (NumberOfResizableBars > PCI_RESIZABLE_BAR_COUNT_MAX) {
        NumberOfResizableBars = PCI_RESIZABLE_BAR_COUNT_MAX;
    }

    //
    // If there is more than one resizable bar, read the additional registers.
    //

    if (NumberOfResizableBars > 1) {
        PciReadDeviceConfig(
            Device,
            &ResizableBarCapability.Entry[1],
            CapOffset +
            FIELD_OFFSET(PCI_EXPRESS_RESIZABLE_BAR_CAPABILITY, Entry[1]),
            (NumberOfResizableBars - 1) *
            sizeof(PCI_EXPRESS_RESIZABLE_BAR_ENTRY));
    }

    //
    // Fill in the mappings for resizable bars which are implemented.
    // It is assumed that unimplemented mappings will have had the
    // ResizableBarSizesSupported array previously initialized to 0.
    //

    for (Index = 0; Index < NumberOfResizableBars; Index += 1) {

        SizesSupported = ResizableBarCapability.Entry[Index].Capability.SizesSupported;
        BarIndex = ResizableBarCapability.Entry[Index].Control.BarIndex;

        //
        // Ignore any entries that appear invalid or which duplicate a
        // previous BAR mapping.
        //

        PCI_ASSERT(SizesSupported != 0);
        PCI_ASSERT(BarIndex < PCI_MAX_BAR_COUNT);
        PCI_ASSERT(ResizableBar->SizesSupported[BarIndex] == 0);

        if (SizesSupported == 0) {
            continue;
        }

        if (BarIndex >= PCI_MAX_BAR_COUNT) {
            continue;
        }

        if (ResizableBar->SizesSupported[BarIndex] != 0) {
            continue;
        }

        ResizableBar->SizesSupported[BarIndex] = SizesSupported;
        ResizableBar->ControlRegister[BarIndex] = Index;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciResizableBarSetSize (
    __in PPCI_DEVICE Device,
    __in ULONG BarIndex,
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    )

/*++

Routine Description:

    This routine examines the Resizable BAR and updates it if necessary to match
    the resource requirement.

    This routine may only be called while the memory decode for the device is
    disabled.

Arguments:

    ExpressPort - Supplies the Express device whose Resizable BAR capability will
        be processed.

    BarIndex - Supplies the index of the BAR to be examined.

    Requirement - Supplies the resource requirement to be satisfied.

Return Value:

    STATUS_SUCCESS

--*/

{

    ULONG BarSize;
    BYTE Index;
    ULONGLONG Length;
    ULONG RegisterOffset;
    PPCI_RESIZABLE_BAR_INFO ResizableBar;
    PCI_EXPRESS_RESIZABLE_BAR_CONTROL_REGISTER ResizableBarControl;
    ULONG SizesSupported;

    NON_PAGED_CODE();

    //
    // If this isn't an express port, or doesn't support resizing on this bar,
    // or isn't a memory bar, just return.
    //

    if (Requirement->Type != CmResourceTypeMemory) {
        goto Exit;
    }

    ResizableBar = PciResizableBarGetInfoPtr(Device);
    if (ResizableBar == NULL) {
        goto Exit;
    }

    SizesSupported = ResizableBar->SizesSupported[BarIndex];
    if (SizesSupported == 0) {
        goto Exit;
    }

    //
    // Determine requested length.
    //

    Length = RtlIoDecodeMemIoResource(Requirement, NULL, NULL, NULL);

    //
    // Length must be a power of 2.
    //

    PCI_ASSERT((Length & (Length - 1)) == 0);
    if ((Length & (Length - 1)) != 0) {
        goto Exit;
    }

    //
    // Convert to a length in 1MB increments.
    // Length must be at least 1MB.
    //

    Length >>= 20;

    //
    // Validate that this size is supported.
    //

    if ((SizesSupported & Length) == 0) {
        goto Exit;
    }

    //
    // Determine which bit is set. This is the value to write into the
    // Resizable Bar Size register.
    //

    _BitScanReverse(&BarSize, (ULONG)Length);

    //
    // If the current bar size doesn't match the desired size, update the bar size.
    //

    Index = ResizableBar->ControlRegister[BarIndex];
    RegisterOffset =
        ResizableBar->Capability +
        FIELD_OFFSET(PCI_EXPRESS_RESIZABLE_BAR_CAPABILITY, Entry[Index].Control);

    PciReadDeviceConfig(
        Device,
        &ResizableBarControl,
        RegisterOffset,
        sizeof(PCI_EXPRESS_RESIZABLE_BAR_CONTROL_REGISTER));

    if (ResizableBarControl.BarSize != BarSize) {
        ResizableBarControl.BarSize = BarSize;
        PciWriteDeviceConfig(
            Device,
            &ResizableBarControl,
            RegisterOffset,
            sizeof(PCI_EXPRESS_RESIZABLE_BAR_CONTROL_REGISTER));
    }

Exit:
    return STATUS_SUCCESS;
}

ULONG
PciGetBarRequirementCount (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine returns the maximum total number of resource descriptors
    required to encode resource requirements for all the BARs in a device
    within a single resource list. This function is called during resource
    requirement population to help size the resource requirements list prior
    to populating it.

Arguments:

    Device - Supplies the device for which resource requirements will be
    generated.

Return Value:

    The number of IO_RESOURCE_DESCRIPTOR entries needed to hold all requirements
    for all the bars in the device.

--*/

{

    ULONG Index;
    PPCI_RESIZABLE_BAR_INFO ResizableBar;
    ULONG ResourceCount;
    ULONG SizesSupported;

    PAGED_CODE();

    //
    // Number of resources for standard Mem/IO BARs:
    //
    // PCI_DEVICE_RESOURCE_COUNT is 13, which is the maximum number of BARs
    // a device can require. Not all will be used.
    // Within Device->Requirements.ByIndex[index], the index values range from
    // 0 to 12, with the following mappings:
    //
    // 0-5: Standard BARs.
    // 6: ROM Bar
    // 7-12: VF BARs.
    //
    // Multiply by 3 because for each BAR there are three resource requirements
    // generated: preferred, generic, and private.
    //


    ResourceCount = PCI_DEVICE_RESOURCE_COUNT * 3;

    //
    // Determine if additional entries need to be allocated for resizable BARs
    // which will be prepended to and potentially override the preferred
    // resource requirements.
    //

    ResizableBar = PciResizableBarGetInfoPtr(Device);
    if ((ResizableBar != NULL) && (ResizableBar->Capability != 0)) {
        for (Index = 0; Index < PCI_RESIZABLE_BAR_COUNT_MAX; Index += 1) {
            SizesSupported = ResizableBar->SizesSupported[Index];
            ResourceCount += RtlNumberOfSetBitsUlongPtr(SizesSupported);
        }
    }

    return ResourceCount;
}

ULONG
PciPopulateBarRequirements (
    __in PPCI_DEVICE Device,
    __in ULONG Index,
    __in PPCI_DEVICE_RESOURCES PreferredResources,
    __inout PIO_RESOURCE_DESCRIPTOR RequirementList
    )

/*++

Routine Description:

    This routine populates the resource requirements list for one BAR with the
    preferred and alternate requirements for that BAR.

Arguments:

    Device - Supplies the device for which resource requirements will be
    generated.

    Index - Supplies the index of the BAR for which requirements will be generated.
    Potential index values are:
         0-5: Standard BARs.
           6: ROM Bar
        7-12: VF BARs.

Return Value:

    The number of IO_RESOURCE_DESCRIPTOR entries populated by the function.

--*/

{

    LONG BitNum;
    ULONGLONG GenericLength;
    PIO_RESOURCE_DESCRIPTOR GenericRequirement;
    ULONGLONG Length;
    ULONGLONG MaximumAddress;
    ULONGLONG MinimumAddress;
    IO_RESOURCE_DESCRIPTOR PreferredRequirement;
    ULONG ReqCount;
    ULONG ReqIndex;
    PPCI_RESIZABLE_BAR_INFO ResizableBar;
    ULONG SizesSupported;
    NTSTATUS Status;

    PAGED_CODE();

    ReqCount = 0;

    //
    // Populate the resizable bar requirements if present. Use the generic
    // requirement as a template.
    //

    GenericRequirement = &Device->Requirements.ByIndex[Index];
    if ((Index < PCI_RESIZABLE_BAR_COUNT_MAX) &&
        (GenericRequirement->Type == CmResourceTypeMemory)) {

        ResizableBar = PciResizableBarGetInfoPtr(Device);
        if ((ResizableBar != NULL) &&
            (ResizableBar->SizesSupported[Index] != 0)) {

            //
            // Determine the generic requirement length and min/max addresses.
            //

            GenericLength = RtlIoDecodeMemIoResource(GenericRequirement,
                                                     NULL,
                                                     &MinimumAddress,
                                                     &MaximumAddress);

            //
            // Iterate over the set of BAR sizes and generate a requirement for
            // the largest and smallest size.  The current PCI spec defines 20 sizes, 
            // from 1MB up to 512GB in powers of 2. The SizesSupported field is a
            // bitfield with one bit for each size.
            //

            SizesSupported = ResizableBar->SizesSupported[Index];
            while (_BitScanReverse(&BitNum, SizesSupported) != 0) {
                SizesSupported &= ~(1UL << BitNum);

                //
                // Convert  to length. Minimum length is 1MB (2^20).
                //

                Length = 1i64 << (BitNum + 20);

                //
                // Don't generate resizable bar requirements for lengths
                // that are the same or less than the generic requirement.
                // Resizable bars are currently only used to allow
                // increasing the bar size. If support for reducing the bar
                // sizes is needed, smaller BARs should be added after the
                // preferred and generic requirements.
                //

                if (Length <= GenericLength) {
                    break;
                }

                //
                // Copy the generic requirement to use as a template.
                //

                RtlCopyMemory(&RequirementList[ReqCount],
                              GenericRequirement,
                              sizeof(IO_RESOURCE_DESCRIPTOR));

                //
                // Update the length and alignment fields.
                //

                Status = RtlIoEncodeMemIoResource(&RequirementList[ReqCount],
                                                  CmResourceTypeMemory,
                                                  Length,
                                                  Length,
                                                  MinimumAddress,
                                                  MaximumAddress);

                if (NT_SUCCESS(Status)) {
                    ReqCount += 1;
                }

                if (SizesSupported != 0) {
                    _BitScanForward(&BitNum, SizesSupported);
                    SizesSupported = (1UL << BitNum);
                }
            }
        }
    }

    //
    // Populate the preferred requirement if present.
    //

    if (PreferredResources->ByIndex[Index].Type != CmResourceTypeNull) {
        PciRequirementFromPreferredResource(&PreferredResources->ByIndex[Index],
                                            &PreferredRequirement);

        RtlCopyMemory(&RequirementList[ReqCount],
                      &PreferredRequirement,
                      sizeof(IO_RESOURCE_DESCRIPTOR));

        ReqCount += 1;
    }

    //
    // Populate the generic requirement if allowed.
    //

    if (PciGenericRequirementsAllowed(Device, Index)) {
        RtlCopyMemory(&RequirementList[ReqCount],
                      GenericRequirement,
                      sizeof(IO_RESOURCE_DESCRIPTOR));

        ReqCount += 1;
    }

    if (ReqCount > 0) {

        //
        // Mark the first requirement as preferred, and mark all other
        // requirements alternatives.
        //

        RequirementList[0].Option |= IO_RESOURCE_PREFERRED;
        for (ReqIndex = 1; ReqIndex < ReqCount; ReqIndex+=1) {
            RequirementList[ReqIndex].Option |= IO_RESOURCE_ALTERNATIVE;
        }

    } else {

        //
        // If no requirement was set, add a NULL requirement as a placeholder.
        //

        NullResourceRequirement(&RequirementList[0]);
        ReqCount = 1;
    }

    //
    // Populate the device private.  This needs to be here for
    // backwards compatibility, but is not currently used.
    //

    RequirementList[ReqCount].Type = CmResourceTypeDevicePrivate;
    RequirementList[ReqCount].ShareDisposition = CmResourceShareDeviceExclusive;

    RequirementList[ReqCount].u.DevicePrivate.Data[0] = 0x1;
    RequirementList[ReqCount].u.DevicePrivate.Data[1] = Index;
    ReqCount += 1;

    return ReqCount;
}
