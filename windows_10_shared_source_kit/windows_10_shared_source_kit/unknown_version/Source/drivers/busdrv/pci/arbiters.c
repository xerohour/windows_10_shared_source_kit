/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    arbiters.c

Abstract:

    This module implements the routines common to the PCI Memory,
    I/O and bus number arbiters, including initialization functions,
    and functions to respond to the arbiter interface.















--*/
// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define ARBITER_INTERFACE_VERSION  0

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciArbiter_Constructor;

VOID
PciArbiter_Reference(
    __inout PVOID Context
    );

VOID
PciArbiter_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciInitializeBusNumberArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *BusNumberArbiter
    );

BOOLEAN
PciRequirementInsideWindow (
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Window,
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciInitializeArbiters)
    #pragma alloc_text(PAGE, PciDestroyArbiters)
    #pragma alloc_text(PAGE, PciRangeListFromResourceList)
    #pragma alloc_text(PAGE, PciRequirementInsideWindow)
    #pragma alloc_text(PAGE, PciArbitrateRequirementInsideBridgeWindow)
    #pragma alloc_text(PAGE, PciArbiter_Constructor)
    #pragma alloc_text(PAGE, PciArbiter_Reference)
    #pragma alloc_text(PAGE, PciArbiter_Dereference)
    #pragma alloc_text(PAGE, PciArbiter_UnpackRequirement)
    #pragma alloc_text(PAGE, PciArbiter_PackResource)
    #pragma alloc_text(PAGE, PciArbiter_UnpackResource)
    #pragma alloc_text(PAGE, PciArbiter_ScoreRequirement)
    #pragma alloc_text(PAGE, PciInitializeBusNumberArbiter)
#endif

// -------------------------------------------------------------------- Globals

const PCI_INTERFACE PciArbiterInterface = {
    &GUID_ARBITER_INTERFACE_STANDARD,  // InterfaceType
    sizeof(ARBITER_INTERFACE),         // MinSize
    ARBITER_INTERFACE_VERSION,         // MinVersion
    ARBITER_INTERFACE_VERSION,         // MaxVersion
    PCIIF_FDO,                         // Flags
    PciInterface_Arbiter,              // Signature
    PciArbiter_Constructor             // Constructor
};

// --------------------------------------------------- Initialization Functions

NTSTATUS
PciInitializeArbiters(
    __inout PPCI_BUS Bus
    )
{
    NTSTATUS status;

    PAGED_CODE();

    if (Bus->ArbitersInitialized) {
        ERROR("PciInitializeArbiters: Arbiters already initialized!\n");
        return STATUS_INVALID_PARAMETER;
    }

    Bus->MemoryArbiter = NULL;
    Bus->IoArbiter = NULL;
    Bus->BusNumberArbiter = NULL;

    status = PciApplyArbiterInitializationHack();
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    status = PciInitializeMemoryArbiter(Bus, &Bus->MemoryArbiter);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    status = PciInitializeIoArbiter(Bus, &Bus->IoArbiter);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    status = PciInitializeBusNumberArbiter(Bus, &Bus->BusNumberArbiter);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    PCI_BUS_SET_FLAG(Bus, ArbitersInitialized);

    return STATUS_SUCCESS;

cleanup:

    PciDestroyArbiters(Bus);
    return status;
}

VOID
PciDestroyArbiters(
    __inout PPCI_BUS Bus
    )
/*++

Routine Description:

    This routine is called when a PCI Secondary Extension that
    contains an arbiter instance is being torn down.   Its function
    is to do any arbiter specific teardown.

Arguments:

    Extension - Address of PCI secondary extension containing
        the arbiter.

Return Value:

    None.

--*/
{
    PARBITER_INSTANCE arbiter;
    PPCI_ARBITER_MEMORY_EXTENSION extension;

    PAGED_CODE();

    if (Bus->MemoryArbiter) {

        arbiter = Bus->MemoryArbiter;
        PCI_ASSERT(!arbiter->ReferenceCount);
        PCI_ASSERT(!arbiter->TransactionInProgress);

        extension = arbiter->Extension;
        if (extension != NULL) {
            ArbFreeOrderingList(&extension->Current.PrefetchableOrdering);
            ArbFreeOrderingList(&extension->Current.NonprefetchableOrdering);
            ArbFreeOrderingList(&extension->OriginalOrdering);

            PCI_ASSERT(!extension->PrefetchableStateSaved);

            //
            // Arbiter->OrderingList is one of the above three lists we just freed -
            // don't free it again
            //
            RtlZeroMemory(&arbiter->OrderingList, sizeof(ARBITER_ORDERING_LIST));
        }

        if (arbiter->Name) {
            PciFreePool(arbiter->Name);
        }
        ArbDeleteArbiterInstance(arbiter);

        if (extension != NULL) {
            PciFreePool(extension);
        }
        PciFreePool(arbiter);
        Bus->MemoryArbiter = NULL;
    }

    if (Bus->IoArbiter) {

        PCI_ASSERT(!Bus->IoArbiter->ReferenceCount);
        PCI_ASSERT(!Bus->IoArbiter->TransactionInProgress);

        if (Bus->IoArbiter->Name) {
            PciFreePool(Bus->IoArbiter->Name);
        }
        ArbDeleteArbiterInstance(Bus->IoArbiter);
        PciFreePool(Bus->IoArbiter);
        Bus->IoArbiter = NULL;
    }

    if (Bus->BusNumberArbiter) {

        PCI_ASSERT(!Bus->BusNumberArbiter->ReferenceCount);
        PCI_ASSERT(!Bus->BusNumberArbiter->TransactionInProgress);

        if (Bus->BusNumberArbiter->Name) {
            PciFreePool(Bus->BusNumberArbiter->Name);
        }
        ArbDeleteArbiterInstance(Bus->BusNumberArbiter);
        PciFreePool(Bus->BusNumberArbiter);
        Bus->BusNumberArbiter = NULL;
    }

    PCI_BUS_CLEAR_FLAG(Bus, ArbitersInitialized);
}

NTSTATUS
PciRangeListFromResourceList(
    __in PPCI_BUS Bus,
    __in PARBITER_INSTANCE Arbiter,
    __in_ecount(Count) PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptors,
    __in ULONG Count,
    __out PRTL_RANGE_LIST RangeList
    )
/*++

Description:

    Generates a range list for the resources of a given type
    from a resource list.

    Note: This routine supports only Memory or Io resources.

    Overlapping ranges in the incoming list will be combined.

Arguments:

    Bus - The bus for whom a range list is being generated.

    Descriptors - An array of CM_PARTIAL_RESOURCE_DESCRIPTORS to be
        translated into a range list.

    Count - The number of descriptors in the Descriptors array.

    Arbiter - The arbiter for whom a range list is being created.

    RangeList - Output range list.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    RTL_RANGE_LIST invertedAllocation;
    NTSTATUS status;
    ULONGLONG start;
    ULONGLONG length;

    PAGED_CODE();

    PCI_ASSERT((Arbiter->ResourceType == CmResourceTypeMemory) ||
               (Arbiter->ResourceType == CmResourceTypePort));

    RtlInitializeRangeList(&invertedAllocation);

    FOR_ALL_IN_ARRAY(Descriptors, Count, resource) {

        //
        // We need to explicity map large memory resources
        // to the memory arbiter
        //

        if ((resource->Type == Arbiter->ResourceType) ||
            ((resource->Type == CmResourceTypeMemoryLarge) &&
             (Arbiter->ResourceType == CmResourceTypeMemory))) {

            status = Arbiter->UnpackResource(resource,
                                             &start,
                                             &length
                                             );
            if (!NT_SUCCESS(status)) {
                ERROR("PciRangeListFromResourceList: UnpackResource failed (%08X) at line %d.\n",
                     status,
                     __LINE__);

                goto exit;
            }

            if (length > 0) {

                //
                // we don't care about Attributes, UserData or Owner since this
                // list is going to get trashed in a minute anyway
                //
                status = RtlAddRange(
                            &invertedAllocation,
                            start,
                            start + length - 1,
                            0,                             // Attributes
                            RTL_RANGE_LIST_ADD_SHARED | RTL_RANGE_LIST_ADD_IF_CONFLICT,
                            0,                             // UserData
                            NULL                           // Owner
                            );

                if (!NT_SUCCESS(status)) {
                    ERROR("PciRangeListFromResourceList: RtlAddRange failed (%08X) at line %d.\n",
                         status,
                         __LINE__);

                    goto exit;
                }

                if ((resource->Type == CmResourceTypePort) &&
                    ((resource->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) ||
                     (resource->Flags & CM_RESOURCE_PORT_12_BIT_DECODE))) {

                    if (resource->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) {
                        start += (1 << 10);
                    } else {
                        start += (1 << 12);
                    }

                    while (start < (1 << 16)) {

                        status = RtlAddRange(
                                    &invertedAllocation,
                                    start,
                                    start + length - 1,
                                    0,                             // Attributes
                                    RTL_RANGE_LIST_ADD_SHARED
                                    | RTL_RANGE_LIST_ADD_IF_CONFLICT,
                                    0,                             // UserData
                                    NULL                           // Owner
                                    );

                        if (!NT_SUCCESS(status)) {
                            ERROR("PciRangeListFromResourceList: RtlAddRange failed (%08X) at line %d.\n",
                                 status,
                                 __LINE__);

                            goto exit;
                        }

                        if (resource->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) {
                            start += (1 << 10);
                        } else {
                            start += (1 << 12);
                        }
                    }
                }
            }
        }
    }

#if defined (_X86_)

    //
    // -- HACK --
    //
    // Some MPS BIOS implementations don't report the
    // memory range 0xA0000 thru 0xBFFFF.  HACK
    // them into the memory list.  Since this is to enable
    // legacy video, assume this applies only to segment 0, bus 0.
    //
    // The 400 hack is because some cards (Matrox MGA) want access
    // to the SYSTEM BIOS DATA area which is in memory at address
    // 0x400 thru 0x4ff.
    //
    // These are old hacks that are not well targeted.  Trying to
    // remove them in the future is a good idea.
    //
    if ((Arbiter->ResourceType == CmResourceTypeMemory) &&
        (Bus->SecondaryBusNumber == 0)) {

        status = RtlAddRange(&invertedAllocation,
                             0x70,                   // HACK Trident
                             0x70,
                             0,                             // Attributes
                             RTL_RANGE_LIST_ADD_SHARED | RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             0,                             // UserData
                             NULL                           // Owner
                             );

        if (!NT_SUCCESS(status)) {
            ERROR("PciRangeListFromResourceList: RtlAddRange failed (%08X) at line %d.\n",
                 status,
                 __LINE__);

            goto exit;
        }

        status = RtlAddRange(&invertedAllocation,
                             0x400,                  // HACK Matrox MGA
                             0x4FF,
                             0,                             // Attributes
                             RTL_RANGE_LIST_ADD_SHARED | RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             0,                             // UserData
                             NULL                           // Owner
                             );

        if (!NT_SUCCESS(status)) {
            ERROR("PciRangeListFromResourceList: RtlAddRange failed (%08X) at line %d.\n",
                 status,
                 __LINE__);

            goto exit;
        }

        status = RtlAddRange(&invertedAllocation,
                             0xA0000,                // HACK broken MPS BIOS
                             0xBFFFF,
                             0,                             // Attributes
                             RTL_RANGE_LIST_ADD_SHARED | RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             0,                             // UserData
                             NULL                           // Owner
                             );

        if (!NT_SUCCESS(status)) {
            ERROR("PciRangeListFromResourceList: RtlAddRange failed (%08X) at line %d.\n",
                 status,
                 __LINE__);

            goto exit;
        }
    }
#else

    UNREFERENCED_PARAMETER(Bus);

#endif

    status = RtlInvertRangeList(RangeList, &invertedAllocation);

exit:

    RtlFreeRangeList(&invertedAllocation);

    return status;
}

BOOLEAN
PciRequirementInsideWindow (
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Window,
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    )

/*++

Routine Description:

    This routine determines whether the requirement fits within the specified
    window. This function is only valid for memory and I/O ports.

Arguments:

    Window - Provides the bounds of the window.

    Requirement - Provides the requirement.

Return Value:

    TRUE if the requirement fits inside the window.
    FALSE otherwise.

--*/

{

    ULONGLONG RequirementEnd;
    ULONGLONG RequirementStart;
    ULONGLONG WindowEnd;
    ULONGLONG WindowSize;
    ULONGLONG WindowStart;

    switch (Window->Type) {
    case CmResourceTypeMemory:
    case CmResourceTypeMemoryLarge:
    case CmResourceTypePort:
        break;
    default:
        return FALSE;
    }

    WindowSize = RtlCmDecodeMemIoResource(Window,  &WindowStart);
    WindowEnd = WindowStart + WindowSize - 1;
    if (WindowSize == 0) {
        return FALSE;
    }

    RequirementStart = Requirement->u.Generic.MinimumAddress.QuadPart;
    if (WindowStart  > RequirementStart) {
        return FALSE;
    }

    RequirementEnd = Requirement->u.Generic.MaximumAddress.QuadPart;
    if (WindowEnd < RequirementEnd) {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
PciArbitrateRequirementInsideBridgeWindow(
    __in PPCI_BUS Bus,
    __in PDEVICE_OBJECT DeviceObject,
    __in_ecount(Count) PIO_RESOURCE_DESCRIPTOR Alternatives,
    __in ULONG Count
    )
/*++

Routine Description:

    This routine examines the arbiter state and determins whether the
    ArbiterEntry should be arbitrated by this arbiter.

Arguments:

    Bus - The bus underneath which this requirement exists.

    DeviceObject - The device object that generated the requirement.

    Alternatives - The alternatives of the requirement.

    Count - The number of alternatives in the Alternatives array.

Return Value:

    TRUE if this arbiter handles this list entry.
    FALSE otherwise.

--*/
{
    BOOLEAN ioAvailable, memAvailable, prefetchAvailable;
    PIO_RESOURCE_DESCRIPTOR ioDescriptor;
    BOOLEAN insideWindow;
    ULONG i;

    PAGED_CODE();

    insideWindow = FALSE;

    if (PCI_CARDBUS_BRIDGE(Bus)) {

        // CardBus bridges can have two kinds of devices below them - CardBus devices,
        // which have PCI-like resources, and PCMCIA R2 devices, which have ISA like
        // resources.  CardBus cards are enumerated by PCI, and R2 devices are not.
        // We don't want to arbitrate R2 devices, since in R2 mode the bridge just
        // acts as a pass-through.
        //
        if (PCI_DEVICE_OBJECT(DeviceObject)) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else if (Bus->PciBridge && Bus->PciBridge->Attributes.Subtractive) {

        ioAvailable = !Bus->PciBridge->MinimumRequirements.IoWindow.Disabled;
        memAvailable = !Bus->PciBridge->MinimumRequirements.MemoryWindow.Disabled;
        prefetchAvailable = !Bus->PciBridge->MinimumRequirements.PrefetchWindow.Disabled;

        //
        // Subtractive bridges should arbitrate requests for legacy PCI resources
        // outside the bridge window and standard PCI resources inside the window.
        // The first way to identify PCI resources is whether they come from
        // PCI devices or not.
        //
        if (!PCI_DEVICE_OBJECT(DeviceObject)) {
            return FALSE;
        }

        //
        // Requirements that PCI generates get marked with the following flags.
        // Arbitrate any of these requirements inside the window, as long as the
        // appropriate window exists.  If not, fall back to arbitrating outside
        // the window.
        //
        ioDescriptor = &Alternatives[0];
        if (ioDescriptor->Type == CmResourceTypePort) {

            if (ioAvailable &&
                (ioDescriptor->Flags & (CM_RESOURCE_PORT_BAR
                                        | CM_RESOURCE_PORT_WINDOW_DECODE))) {

                insideWindow = TRUE;
            }
        }

        if ((ioDescriptor->Type == CmResourceTypeMemory) ||
            (ioDescriptor->Type == CmResourceTypeMemoryLarge)) {

            if (ioDescriptor->Flags & (CM_RESOURCE_MEMORY_BAR
                                       | CM_RESOURCE_MEMORY_WINDOW_DECODE)) {


                if (memAvailable ||
                    ((ioDescriptor->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) &&
                     prefetchAvailable)) {

                    insideWindow = TRUE;
                }
            }
        }

        //
        // Now deal with requirements that are for PCI devices, but were not
        // generated by PCI.  For these, attempt to respect the way the
        // BIOS set this up.  If there is a fixed requirement for a range
        // that was inside the boot config'ed bridge window, always keep
        // this range inside the window.  If it was boot config'ed outside
        // the window, keep it there.
        //
        for (i = 0; i < Count; i++) {

            ioDescriptor = &Alternatives[i];

            if (((ioDescriptor->Type != CmResourceTypePort) &&
                 (ioDescriptor->Type != CmResourceTypeMemory) &&
                 (ioDescriptor->Type != CmResourceTypeMemoryLarge)) ||
                (PciIsFixedRequirement(ioDescriptor) == FALSE)) {
                continue;
            }

            if (ioDescriptor->Type == CmResourceTypePort) {

                if (PciRequirementInsideWindow(
                        &Bus->PciBridge->BiosConfiguration.IoWindow,
                        ioDescriptor
                        )) {
                    insideWindow = TRUE;
                }
            }
            if ((ioDescriptor->Type == CmResourceTypeMemory) ||
                (ioDescriptor->Type == CmResourceTypeMemoryLarge)) {

                if (PciRequirementInsideWindow(
                        &Bus->PciBridge->BiosConfiguration.MemoryWindow,
                        ioDescriptor
                        ) ||
                    ((ioDescriptor->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) &&
                     PciRequirementInsideWindow(
                        &Bus->PciBridge->BiosConfiguration.PrefetchWindow,
                        ioDescriptor
                        ))) {

                    insideWindow = TRUE;
                }
            }
            if (insideWindow) {
                break;
            }
        }

        return insideWindow;

    } else {

        //
        // If this bridge is a garden-variety positive decode PCI-PCI bridge
        // or host bridge, the requirement must live inside the window.
        //
        return TRUE;
    }
}


// --------------------------------------------------------- Arbiter Interface Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciArbiter_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Check the InterfaceSpecificData to see if this is the correct
    arbiter (we already know the required interface is an arbiter
    from the GUID) and if so, allocate (and reference) a context
    for this interface.

Arguments:

    PciInterface- Pointer to the PciInterface record for this
        interface type.

    InterfaceSpecificData - A ULONG containing the resource
        type for which arbitration is required.

    InterfaceReturn

    Filtered - Not supported for this interface.

Return Value:

    TRUE is this device is not known to cause problems, FALSE
    if the device should be skipped altogether.

--*/
{
    PARBITER_INTERFACE arbiterInterface;
    PARBITER_INSTANCE arbiter;
    PPCI_BUS bus;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(ARBITER_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    bus = (PPCI_BUS)DeviceExtension;

    if (!bus->ArbitersInitialized) {

        return STATUS_NOT_SUPPORTED;
    }

    switch ((ULONG_PTR)InterfaceSpecificData) {

        case CmResourceTypeMemory:
        case CmResourceTypeMemoryLarge:
            arbiter = bus->MemoryArbiter;
            break;
        case CmResourceTypePort:
            arbiter = bus->IoArbiter;
            break;
        case CmResourceTypeBusNumber:
            arbiter = bus->BusNumberArbiter;
            break;
        default:

            //
            // We have arbiters for memory, I/O and bus numbers only.
            //
            return STATUS_INVALID_PARAMETER_5;
    }

    //
    // NULL arbiter means this resource type isn't arbitrated
    // by this bridge (subtractive bridge, or whatever).
    //
    if (arbiter == NULL) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Have already verified that the InterfaceReturn variable
    // points to an area in memory large enough to contain an
    // ARBITER_INTERFACE.  Fill it in for the caller.
    //
    arbiterInterface = (PARBITER_INTERFACE)InterfaceReturn;

    arbiterInterface->Context              = arbiter;
    arbiterInterface->Size                 = sizeof(ARBITER_INTERFACE);
    arbiterInterface->Version              = ARBITER_INTERFACE_VERSION;
    arbiterInterface->InterfaceReference   = PciArbiter_Reference;
    arbiterInterface->InterfaceDereference = PciArbiter_Dereference;
    arbiterInterface->ArbiterHandler       = ArbArbiterHandler;

    arbiterInterface->Flags = ARBITER_PARTIAL;

    return STATUS_SUCCESS;
}

VOID
PciArbiter_Reference(
    __inout PVOID Context
    )
{
    PARBITER_INSTANCE arbiter = (PARBITER_INSTANCE)Context;

    PAGED_CODE();

    InterlockedIncrement(&arbiter->ReferenceCount);
}

VOID
PciArbiter_Dereference(
    __inout PVOID Context
    )
{
    PARBITER_INSTANCE arbiter = (PARBITER_INSTANCE)Context;

    PAGED_CODE();

    InterlockedDecrement(&arbiter->ReferenceCount);
}

// --------------------------------------------------------- Common Arbiter Utility Functions

NTSTATUS
PciArbiter_UnpackRequirement(
    __in PIO_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONGLONG Minimum,
    __out PULONGLONG Maximum,
    __out PULONGLONG Length,
    __out PULONGLONG Alignment
    )

/*++

Routine Description:

    This routine unpacks an resource requirement descriptor.

Arguments:

    Descriptor - The descriptor describing the requirement to unpack.

    Minimum - Pointer to where the minimum acceptable start value should be
        unpacked to.

    Maximum - Pointer to where the maximum acceptable end value should be
        unpacked to.

    Length - Pointer to where the required length should be unpacked to.

    Minimum - Pointer to where the required alignment should be unpacked to.

Return Value:

    Returns the status of this operation.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    PCI_ASSERT(Descriptor);
    PCI_ASSERT((Descriptor->Type == CmResourceTypePort) ||
               (Descriptor->Type == CmResourceTypeMemory) ||
               (Descriptor->Type == CmResourceTypeMemoryLarge) ||
               (Descriptor->Type == CmResourceTypeBusNumber));

    if (Descriptor->Type != CmResourceTypeBusNumber)
    {
        *Length = RtlIoDecodeMemIoResource(Descriptor,
                                           Alignment,
                                           Minimum,
                                           Maximum);

    } else {

        *Minimum = (ULONGLONG)Descriptor->u.Generic.MinimumAddress.QuadPart;
        *Maximum = (ULONGLONG)Descriptor->u.Generic.MaximumAddress.QuadPart;
        *Length = Descriptor->u.Generic.Length;
        *Alignment = Descriptor->u.Generic.Alignment;
    }


    //
    // Fix the broken hardware that reports 0 alignment
    //
    if (*Alignment == 0) {
        *Alignment = 1;
    }

    //
    // Fix broken INF's that report they support 24bit memory > 0xFFFFFF
    //
    if ((Descriptor->Type == CmResourceTypeMemory) &&
        (Descriptor->Flags & CM_RESOURCE_MEMORY_24) &&
        (*Maximum > 0xFFFFFF)) {

        if (*Minimum > 0xFFFFFF) {
            ERROR("PciArbiter_UnpackRequirement: 24 bit decode specified but both min and max are greater than 0xFFFFFF, most probably due to broken INF!\n");
            status = STATUS_UNSUCCESSFUL;

        } else {
            *Maximum = 0xFFFFFF;
        }
    }

    return status;
}

NTSTATUS
PciArbiter_PackResource(
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __in ULONGLONG Start,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor
    )

/*++

Routine Description:

    This routine packs an resource descriptor.

Arguments:

    Requirement - The requirement from which this resource was chosen.

    Start - The start value of the resource.

    Descriptor - Pointer to the descriptor to pack into.

Return Value:

    Returns the status of this operation.

--*/

{
    PAGED_CODE();

    PCI_ASSERT(Descriptor);
    PCI_ASSERT(Requirement);
    PCI_ASSERT((Requirement->Type == CmResourceTypePort) ||
               (Requirement->Type == CmResourceTypeMemory) ||
               (Requirement->Type == CmResourceTypeMemoryLarge) ||
               (Requirement->Type == CmResourceTypeBusNumber));

    //
    // For bridge windows that are zero-length, change the type to
    // CmResourceTypeNull to avoid confusion.  Only do this for
    // bridge windows because zero-length resources are common
    // for bridge windows and to maintain backwards-compatibility
    // for other devices whose drivers might not expect this.
    //
    if (PCI_WINDOW_REQUIREMENT(Requirement) &&
        (Requirement->u.Generic.Length == 0)) {

        Descriptor->Type = CmResourceTypeNull;
    } else {

        Descriptor->Type = Requirement->Type;
    }

    Descriptor->Flags = Requirement->Flags;
    Descriptor->ShareDisposition = Requirement->ShareDisposition;
    Descriptor->u.Generic.Start.QuadPart = Start;
    Descriptor->u.Generic.Length = Requirement->u.Generic.Length;

    return STATUS_SUCCESS;
}

NTSTATUS
PciArbiter_UnpackResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONGLONG Start,
    __out PULONGLONG Length
    )

/*++

Routine Description:

    This routine unpacks an resource descriptor.

Arguments:

    Descriptor - The descriptor describing the resource to unpack.

    Start - Pointer to where the start value should be unpacked to.

    End - Pointer to where the end value should be unpacked to.

Return Value:

    Returns the status of this operation.

--*/

{
    PAGED_CODE();

    PCI_ASSERT(Descriptor);
    PCI_ASSERT(Start);
    PCI_ASSERT(Length);
    PCI_ASSERT((Descriptor->Type == CmResourceTypePort) ||
               (Descriptor->Type == CmResourceTypeMemory) ||
               (Descriptor->Type == CmResourceTypeMemoryLarge) ||
               (Descriptor->Type == CmResourceTypeBusNumber));


    if (Descriptor->Type != CmResourceTypeBusNumber)
    {
        *Length = RtlCmDecodeMemIoResource(Descriptor, Start);

    } else {
        *Length= Descriptor->u.Generic.Length;
        *Start = Descriptor->u.Generic.Start.QuadPart;
    }

    return STATUS_SUCCESS;
}

LONG
PciArbiter_ScoreRequirement (
    __in PIO_RESOURCE_DESCRIPTOR Descriptor
    )

/*++

Routine Description:

    This routine scores a requirement based on how flexible it is.  The least
    flexible devices are scored the least and so when the arbitration list is
    sorted we try to allocate their resources first.

Arguments:

    Descriptor - The descriptor describing the requirement to score.


Return Value:

    The score.

--*/

{
    LONG score;
    ULONGLONG start, end;
    ULONGLONG bigscore;
    ULONGLONG alignment;
    ULONGLONG length;

    PAGED_CODE();

    PCI_ASSERT(Descriptor);
    PCI_ASSERT((Descriptor->Type == CmResourceTypePort) ||
               (Descriptor->Type == CmResourceTypeMemory) ||
               (Descriptor->Type == CmResourceTypeMemoryLarge) ||
               (Descriptor->Type == CmResourceTypeBusNumber));

    if (Descriptor->Type != CmResourceTypeBusNumber) {
        length = RtlIoDecodeMemIoResource(Descriptor,
                                          &alignment,
                                          &start,
                                          &end);

    } else {

        alignment = Descriptor->u.Generic.Alignment;
        length = Descriptor->u.Generic.Length;
        start = Descriptor->u.Generic.MinimumAddress.QuadPart;
        end = Descriptor->u.Generic.MaximumAddress.QuadPart;
    }

    //
    // Fix the broken hardware that reports 0 alignment
    //
    if (alignment == 0) {

        //
        // Set this to 1 here, because we arbitrate ISA
        // devices in the context of PCI. (subtractive decode)
        //
        // Any PCI device that has alignment 0 will also
        // have Length 0, and we deal with that problem elsewhere.
        //
        alignment = 1;
    }

    start = ALIGN_ADDRESS_UP(
                (ULONGLONG)start,
                alignment
                );

    //
    // The score is the number of possible allocations that could be made
    // given the alignment and length constraints
    //

    bigscore = (((end - length + 1) - start) / alignment) + 1;

    //
    // Note, the scores for each possibility are added together.  To
    // avoid overflowing the total, we need to limit the range returned.
    //
    // Make it a sort of logarithmic score.  Find the highest byte
    // set, weight it (add 0x100) and use the log (I said "sort of").
    //
    // This puts the result in the range 0xff80 down to 0x0100.
    //

    for (score = sizeof(bigscore) - 1; score >= 0; score--) {

        UCHAR v = *(((PUCHAR)&bigscore) + score);
        if (v != 0) {
            score = (v + 0x100) << score;
            break;
        }
    }

    //
    // The resulting TOTAL from scoring all the alternatives is used
    // to sort the list.  The highest total is considered the easiest
    // to place,....  which is probably true,... What if we got some-
    // thing like a single fit preferred setting followed by a fits
    // anywhere setting?   We don't want that to score higher than
    // another device which only specified the fit anywhere setting,
    // the preferred setting is harder to achieve.
    //
    // And, are two alternatives, each half as good an a 'fit anywhere'
    // as good as the 'fit anywhere'.  Not really.
    //
    // So, we weight the result even further depending on what options
    // are set in this resource.
    //

    if (Descriptor->Option &
                    (IO_RESOURCE_PREFERRED | IO_RESOURCE_ALTERNATIVE)) {
        score -= 0x100;
    }

    ARB_PRINT(
        3,
        ("  %s resource %p (0x%I64x-0x%I64x) => %i\n",
        Descriptor->Type == CmResourceTypeMemory ||
            Descriptor->Type == CmResourceTypeMemoryLarge ? "Memory" : "Io",
        Descriptor,
        Descriptor->u.Generic.MinimumAddress.QuadPart,
        end,
        score
        ));

    return score;
}

// --------------------------------------------------------- Bus Number Arbiter

NTSTATUS
PciInitializeBusNumberArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *BusNumberArbiter
    )
/*++

Routine Description:

    This routine is called once per instantiation of an arbiter.
    Performs initialization of this instantiation's context.

Arguments:

    Bus - The bus this arbiter is for.

    BusNumberArbiter - On successful exit, this will point to a
        pointer to the newly allocated and initialized arbiter.

    Resources - The resources to use to initialze the arbiter.

    ResourceCount - The number of resources in the Resources array.

Return Value:

    NT Status code.

--*/
{
    PARBITER_INSTANCE arbiter;
    PWSTR arbiterName;
    NTSTATUS status;

    PAGED_CODE();

    *BusNumberArbiter = NULL;
    arbiter = NULL;
    arbiterName = NULL;

    arbiter = PciAllocatePool(NonPagedPoolNx, sizeof(ARBITER_INSTANCE));
    if (arbiter == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    RtlZeroMemory(arbiter, sizeof(ARBITER_INSTANCE));

    //
    // Set the Action Handler entry points.
    //
    arbiter->UnpackRequirement = PciArbiter_UnpackRequirement;
    arbiter->PackResource      = PciArbiter_PackResource;
    arbiter->UnpackResource    = PciArbiter_UnpackResource;
    arbiter->ScoreRequirement  = PciArbiter_ScoreRequirement;

    //
    // Name the arbiter.
    //
    arbiterName = PciAllocatePool(NonPagedPoolNx, sizeof(L"PCI Bus Number (b=xxxxxxxx)"));
    if (arbiterName == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    status = RtlStringCbPrintfW(arbiterName,
                                sizeof(L"PCI Bus Number (b=xxxxxxxx)"),
                                L"PCI Bus Number (b=%x)",
                                Bus->SecondaryBusNumber
                                );

    PCI_ASSERT(NT_SUCCESS(status));
    UNREFERENCED_PARAMETER(status);

    //
    // Initialize the arbiter library's fields.
    //
    status = ArbInitializeArbiterInstance(arbiter,
                                          Bus->DeviceObject,
                                          CmResourceTypeBusNumber,
                                          arbiterName,
                                          L"Pci",
                                          NULL    // no translation of bus numbers
                                          );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    *BusNumberArbiter = arbiter;
    return STATUS_SUCCESS;

cleanup:

    if (arbiterName) {
        PciFreePool(arbiterName);
    }
    if (arbiter) {
        PciFreePool(arbiter);
    }

    return status;
}
