/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    arbport.c

Abstract:

    This module contains functions for arbitrating I/O port resources











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PORT_ARBITER_VERSION   0

//
// Flags for AllocationState WorkSpace
//
#define PORT_ARBITER_PREPROCESSED       0x00000001
#define PORT_ARBITER_IMPROVISED_DECODE  0x00000002
#define PORT_ARBITER_ISA_BIT_SET        0x00000004
#define PORT_ARBITER_BRIDGE_WINDOW      0x00000008

//
// Constants
//
#define PCI_BRIDGE_ISA_BIT_STRIDE       0x400
#define PCI_BRIDGE_ISA_BIT_WIDTH        0x100
#define PCI_BRIDGE_ISA_BIT_MAX          0xFFFF
#define MAX_10_BIT_DECODE               0x3FF
#define MAX_12_BIT_DECODE               0xFFF
#define MAX_16_BIT_DECODE               0xFFFF

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciIoArbiter_ReinitializeArbiter(
    _Inout_ PARBITER_INSTANCE Arbiter,
    _In_ ULONG ResourceCount,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    _Inout_ PRTL_RANGE_LIST RangeList
    );

NTSTATUS
PciIoArbiter_QueryArbitrate(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_QUERY_ARBITRATE_PARAMETERS Parameters
    );

NTSTATUS
PciIoArbiter_PreprocessEntry(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciIoArbiter_GetNextAllocationRange(
    __inout PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciIoArbiter_FindSuitableRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciIoArbiter_OverrideConflict(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

VOID
PciIoArbiter_AddAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    );

VOID
PciIoArbiter_BacktrackAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State
     );

VOID
PciAddOrBacktrackIoAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State,
     __in PARBITER_BACKTRACK_ALLOCATION Callback
    );

VOID
PciSetAllocationAttributes(
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciFindSuitableIsaBitRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciAllAliasesAvailable(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    );

NTSTATUS
PciExcludeRangesFromWindow(
    __in ULONGLONG Start,
    __in ULONGLONG End,
    __inout PRTL_RANGE_LIST ArbiterRanges,
    __in PRTL_RANGE_LIST ExclusionRanges
    );

// --------------------------------------------------------------------- Pragmas

#pragma prefast(disable:__WARNING_UNUSED_SCALAR_ASSIGNMENT, "checked builds")

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciInitializeIoArbiter)
    #pragma alloc_text(PAGE, PciProgramIoArbiter)
    #pragma alloc_text(INIT, PciBuildDefaultIoExclusionLists)
    #pragma alloc_text(PAGE, PciIoArbiter_ReinitializeArbiter)
    #pragma alloc_text(PAGE, PciIoArbiter_QueryArbitrate)
    #pragma alloc_text(PAGE, PciIoArbiter_PreprocessEntry)
    #pragma alloc_text(PAGE, PciIoArbiter_GetNextAllocationRange)
    #pragma alloc_text(PAGE, PciIoArbiter_FindSuitableRange)
    #pragma alloc_text(PAGE, PciIoArbiter_OverrideConflict)
    #pragma alloc_text(PAGE, PciIoArbiter_AddAllocation)
    #pragma alloc_text(PAGE, PciIoArbiter_BacktrackAllocation)
    #pragma alloc_text(PAGE, PciAddOrBacktrackIoAllocation)
    #pragma alloc_text(PAGE, PciSetAllocationAttributes)
    #pragma alloc_text(PAGE, PciFindSuitableIsaBitRange)
    #pragma alloc_text(PAGE, PciAllAliasesAvailable)
    #pragma alloc_text(PAGE, PciExcludeRangesFromWindow)
#endif

// --------------------------------------------------------------------- Globals

//
// Range lists indicating the ranges excluded from decode when the ISA and/or
// VGA bits are set on a bridge.  Initialized by PciBuildDefaultIoExclusionLists
// from DriverEntry.
//
RTL_RANGE_LIST PciIsaBitExclusionList;
RTL_RANGE_LIST PciVgaAndIsaBitExclusionList;

ARBITER_ORDERING PciBridgeOrderings[] = {
    { 0x10000, MAXULONGLONG },
    { 0,       0xFFFF }
};

ARBITER_ORDERING_LIST PciBridgeOrderingList = {
    sizeof(PciBridgeOrderings) / sizeof (ARBITER_ORDERING),
    sizeof(PciBridgeOrderings) / sizeof (ARBITER_ORDERING),
    PciBridgeOrderings
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciInitializeIoArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *IoArbiter
    )
/*++

Routine Description:

    This routine is called once per instantiation of an arbiter.
    It initializes the arbiter with the correct callbacks and
    with the provided resources.

Arguments:

    Bus - The bus this arbiter is for.

    IoArbiter - The arbiter to initialize.

    Resources - An array of resources to use to initialize the arbiter.

    ResourceCount - The number of elements in the Resources array.

Return Value:

    NT Status code.

--*/
{
    PARBITER_INSTANCE arbiter;
    PWSTR arbiterName;
    NTSTATUS status;

    PAGED_CODE();

    *IoArbiter = NULL;
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
    arbiter->InitializeRangeList    = PciIoArbiter_ReinitializeArbiter;
    arbiter->PreprocessEntry        = PciIoArbiter_PreprocessEntry;
    arbiter->GetNextAllocationRange = PciIoArbiter_GetNextAllocationRange;
    arbiter->FindSuitableRange      = PciIoArbiter_FindSuitableRange;
    arbiter->AddAllocation          = PciIoArbiter_AddAllocation;
    arbiter->BacktrackAllocation    = PciIoArbiter_BacktrackAllocation;
    arbiter->OverrideConflict       = PciIoArbiter_OverrideConflict;
    arbiter->QueryArbitrate         = PciIoArbiter_QueryArbitrate;

    arbiter->UnpackRequirement = PciArbiter_UnpackRequirement;
    arbiter->PackResource      = PciArbiter_PackResource;
    arbiter->UnpackResource    = PciArbiter_UnpackResource;
    arbiter->ScoreRequirement  = PciArbiter_ScoreRequirement;

    //
    // Name the arbiter.
    //
    arbiterName = PciAllocatePool(NonPagedPoolNx, sizeof(L"PCI I/O Port (b=xxxxxxxx)"));
    if (arbiterName == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    status = RtlStringCbPrintfW(arbiterName,
                                sizeof(L"PCI I/O Port (b=xxxxxxxx)"),
                                L"PCI I/O Port (b=%x)",
                                Bus->SecondaryBusNumber
                                );

    PCI_ASSERT(NT_SUCCESS(status));    
    
    //
    // Initialize the arbiter library's fields.
    //
    status =  ArbInitializeArbiterInstance(arbiter,
                                           Bus->DeviceObject,
                                           CmResourceTypePort,
                                           arbiterName,
                                           L"Pci",
                                           NULL
                                           );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    *IoArbiter = arbiter;

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

NTSTATUS
PciProgramIoArbiter(
    __in PPCI_BUS Bus,
    __in PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount,
    __inout PRTL_RANGE_LIST RangeList
    )
/*++

Routine Description:

    This routine programs the provided resources into the arbiter.
    It first initializes the given range list with the resources
    and then does any arbiter setup required based on these resources.

Arguments:

    Bus - The bus this arbiter is for.

    Arbiter - The arbiter to program.

    Resources - An array of resources to use to program the arbiter.

    ResourceCount - The number of elements in the Resources array.

    RangeList - The range list to program.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
    PRTL_RANGE_LIST exclusionList;
    PARBITER_INSTANCE rootArbiter;
    BOOLEAN foundResource;
    PPCI_BRIDGE bridge;
    PPCI_BUS rootBus;
    NTSTATUS status;
    ULONGLONG dummy;

    PAGED_CODE();

    //
    // First program the range list with the resources provided.
    //
    status = PciRangeListFromResourceList(Bus,
                                          Arbiter,
                                          Resources,
                                          ResourceCount,
                                          RangeList
                                          );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Now do some munging for PCI-PCI bridges.  If this isn't
    // a bridge or there are no resources here, early out.
    //
    if ((ResourceCount == 0) || !Bus->PciBridge) {
        status = STATUS_SUCCESS;
        goto exit;
    }

    bridge = Bus->PciBridge;

    //
    // Select the appropriate exclusion list.  Note that bridges with the
    // 16 bit VGA decoding bit don't get the VgaAndIsa exclusion list,
    // because the 16 bit VGA resources don't overlap with bridge windows.
    // This is true UNLESS the bridge window lives at 0-0xFFF, which can
    // never happen today.  If that ever can happen, a new exclusion
    // list is required and the following code is wrong.
    //
    exclusionList = NULL;
    if (bridge->Attributes.IsaBitSet) {

        if (bridge->Attributes.LegacyVgaBitSet) {
            exclusionList = &PciVgaAndIsaBitExclusionList;
        } else {
            exclusionList = &PciIsaBitExclusionList;
        }
    }

    //
    // Find the port window and if the ISA bit is set exclude
    // the ranges in the window that aren't available when
    // the ISA bit is set.
    //
    foundResource = FALSE;
    FOR_ALL_IN_ARRAY(Resources, ResourceCount, descriptor) {

        if ((descriptor->Type == CmResourceTypePort) &&
            (descriptor->Flags & CM_RESOURCE_PORT_WINDOW_DECODE)) {

            if (exclusionList) {
                status = PciExcludeRangesFromWindow(
                             descriptor->u.Port.Start.QuadPart,
                             descriptor->u.Port.Start.QuadPart
                                + descriptor->u.Port.Length - 1,
                             RangeList,
                             exclusionList
                             );
                if (!NT_SUCCESS(status)) {
                    goto exit;
                }
            }

            foundResource = TRUE;
            break;
        }
    }

    //
    // Now deal with sparse root busses.  This is when a root bus (typically
    // not the first root bus) is assigned only tiny slices of I/O space, not
    // enough for an entire bridge window.  Bridges can operate underneath
    // these, but no devices can be placed in the ranges that don't get down
    // the bus.  To handle this, make sure all ranges not allocated to the
    // root bus's arbiter are excluded from this arbiter.
    //
    if (foundResource == FALSE) {

        //
        // No I/O on the bus, so nothing to do for sparse roots.
        //
        status = STATUS_SUCCESS;
        goto exit;
    }

    rootBus = Bus->Root->Bus;
    rootArbiter = rootBus->IoArbiter;

    if (rootArbiter == NULL) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    ArbAcquireArbiterLock(rootArbiter);

    status = PciExcludeRangesFromWindow(
                descriptor->u.Port.Start.QuadPart,
                descriptor->u.Port.Start.QuadPart
                   + descriptor->u.Port.Length - 1,
                RangeList,
                rootArbiter->Allocation
                );

    ArbReleaseArbiterLock(rootArbiter);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Sanity check this to make sure that at least one port is available - if
    // not then fail start.  You could argue that we should really have this
    // marked as insufficient resources (code 12) as oppose to failed start
    // (code 10) but that is much harder and this has the desired effect.
    // We check by seeing if we can find a range for the minimal PCI requirements
    // of 4 ports alignment 4.
    //
    status = RtlFindRange(RangeList,
                          0,
                          MAXULONGLONG,
                          4,
                          4,
                          0,     // Flags
                          0,     // AttribureAvailableMask
                          NULL,  // Context
                          NULL,  // Callback
                          &dummy
                          );
    if (!NT_SUCCESS(status)) {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

exit:

    return status;
}

NTSTATUS
PciBuildDefaultIoExclusionLists(
    VOID
    )
{
    NTSTATUS status;
    ULONG windowBase;

    PAGED_CODE();

    PCI_ASSERT(PciIsaBitExclusionList.Count == 0);
    PCI_ASSERT(PciVgaAndIsaBitExclusionList.Count == 0);

    RtlInitializeRangeList(&PciIsaBitExclusionList);
    RtlInitializeRangeList(&PciVgaAndIsaBitExclusionList);

    for (windowBase = 0; windowBase <= 0xFFFF; windowBase += 0x400) {

        //
        // Add the x100-x3ff range to the ISA list
        //
        status = RtlAddRange(&PciIsaBitExclusionList,
                             windowBase + 0x100,
                             windowBase + 0x3FF,
                             0,
                             RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             NULL,
                             NULL // this range is not on the bus
                             );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        //
        // Add the x100-x3af, x3bc-x3bf and x3e0-x3ff ranges to the VGA/ISA list
        //
        status = RtlAddRange(&PciVgaAndIsaBitExclusionList,
                             windowBase + 0x100,
                             windowBase + 0x3AF,
                             0,
                             RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             NULL,
                             NULL // this range is not on the bus
                             );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        status = RtlAddRange(&PciVgaAndIsaBitExclusionList,
                             windowBase + 0x3BC,
                             windowBase + 0x3BF,
                             0,
                             RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             NULL,
                             NULL // this range is not on the bus
                             );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        status = RtlAddRange(&PciVgaAndIsaBitExclusionList,
                             windowBase + 0x3E0,
                             windowBase + 0x3FF,
                             0,
                             RTL_RANGE_LIST_ADD_IF_CONFLICT,
                             NULL,
                             NULL // this range is not on the bus
                             );
         if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    return STATUS_SUCCESS;

cleanup:

    RtlFreeRangeList(&PciIsaBitExclusionList);
    RtlFreeRangeList(&PciVgaAndIsaBitExclusionList);

    return status;
}

// --------------------------------------------------------------- I/O Arbiter entrypoints

NTSTATUS
PciIoArbiter_ReinitializeArbiter(
    _Inout_ PARBITER_INSTANCE Arbiter,
    _In_ ULONG ResourceCount,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    _Inout_ PRTL_RANGE_LIST RangeList
    )
/*++

Routine Description:

    This is the ReInitializeArbiter entrypoint for the PCI I/O arbiter.
    This takes a set of resources being speculatively assigned to the
    bridge this arbiter is for, and programs the arbiter with these
    resources.  This is used during rebalance.  After this call, a speculative
    arbitration using these resources will take place, and if that is
    successful, the bridge will be restarted with these resources.

Parameters:

    Arbiter - The arbiter to re-initialize.

    ResourceCount - The number of resources in the Resources array.

    Resources - The resources to use to re-initialize the arbiters.

    RangeList - The range list to program with the Resources.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;
    PCI_BRIDGE_RESOURCES bridgeResources;
    PPCI_BUS bus;

    PAGED_CODE();

    bus = (PPCI_BUS)Arbiter->BusDeviceObject->DeviceExtension;

    //
    // Root buses can't be reconfigured.
    //

    if (!bus->PciBridge) {
        ERROR("PciIoArbiter_ReinitializeArbiter: PciBridge not initiazlied.\n");
        return STATUS_INVALID_PARAMETER;
    }

    TRACE(ARBITER_TRACE_LEVEL, "\nProgramming arbiter %p with new resources:\n", Arbiter);
    DebugDumpResourceArray(ARBITER_TRACE_LEVEL, Resources, ResourceCount);

    status = PciBridgeProcessStartResources(bus->PciBridge,
                                            Resources,
                                            ResourceCount,
                                            &bridgeResources
                                            );
    if (!NT_SUCCESS(status)) {

        TRACE(ARBITER_TRACE_LEVEL, "Could not process start resources - 0x%x.\n", status);
        return status;
    }

    //
    // Now free up the old range list and re-program it with the new resources.
    //
    RtlFreeRangeList(RangeList);

    status = PciProgramIoArbiter(bus,
                                 Arbiter,
                                 bridgeResources.ByIndex,
                                 PCI_BRIDGE_RESOURCE_COUNT,
                                 RangeList
                                 );

    Arbiter->Extension = (PVOID)PCI_ARBITER_STATE_REPROGRAMMED;

    return status;
}

NTSTATUS
PciIoArbiter_QueryArbitrate(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_QUERY_ARBITRATE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine examines the arbiter state and determins whether the
    entry in the ArbitrationList should be arbitrated by this arbiter.

Arguments:

    Arbiter - The arbiter to examine

    Parameters - The parameters for the call.  This has one element,
        ArbitrationList, a list of ARBITER_LIST_ENTRY's, containing only one entry.
        This routine should determine if this list entry should be arbitrated
        by the arbiter.

Return Value:

    TRUE if this arbiter handles this list entry.
    FALSE otherwise.

--*/
{
    PARBITER_LIST_ENTRY arbiterEntry;

    PAGED_CODE();

    //
    // The parameters for the call are an ARBITER_LIST_ENTRY to
    // QueryArbitrate.  It should be the only entry in the list.
    //
    arbiterEntry = CONTAINING_RECORD(Parameters->ArbitrationList->Flink,
                                     ARBITER_LIST_ENTRY,
                                     ListEntry
                                     );

    PCI_ASSERT(arbiterEntry->ListEntry.Flink == Parameters->ArbitrationList);
    if (PciArbitrateRequirementInsideBridgeWindow(
            (PPCI_BUS)Arbiter->BusDeviceObject->DeviceExtension,
            arbiterEntry->PhysicalDeviceObject,
            arbiterEntry->Alternatives,
            arbiterEntry->AlternativeCount
            )) {

        return STATUS_SUCCESS;

    } else {

        TRACE(ARBITER_TRACE_LEVEL,
              "PciIoArbiter_QueryArbitrate: Not arbitrating request.\n"
              );
        return STATUS_UNSUCCESSFUL;
    }
}

NTSTATUS
PciIoArbiter_PreprocessEntry(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the PreprocessEntry entrypoint for the PCI I/O arbiter.
    The entry to preprocess corresponds to a requirement that needs
    to be arbitrated for.  This routine gets called before the arbiter
    actually tries to find a solution for this requirement, and is
    used to set some flags for use by RtlFindRange.

Parameters:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    NT Status code.

--*/
{
    BOOLEAN improviseDecode, windowDetected;
    PARBITER_ALTERNATIVE current;
    PDEVICE_OBJECT deviceObject;
    PPCI_DEVICE device;
    ULONGLONG greatestPort;
    ULONG defaultDecode;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Arbiter);

    TRACE(ARBITER_TRACE_LEVEL, "\nPciIoArbiter_PreprocessEntry for arbiter %p\n", Arbiter);

    deviceObject = State->Entry->PhysicalDeviceObject;
    device = (PPCI_DEVICE)deviceObject->DeviceExtension;

    //
    // If this is a PnP arbitration request for a PCI PDO, make sure the PDO
    // isn't already controlled by a legacy driver.
    //
    if (PCI_DEVICE_OBJECT(deviceObject) &&
        (State->Entry->RequestSource == ArbiterRequestPnpEnumerated)) {

        if (device->LegacyDriver) {

            return STATUS_DEVICE_BUSY;
        }
    }

    if (State->WorkSpace & PORT_ARBITER_PREPROCESSED) {

        //
        // We have already proprocessed this entry so don't repeat the work
        //
        return STATUS_SUCCESS;

    }

    State->WorkSpace = PORT_ARBITER_PREPROCESSED;

    //
    // Scan the alternatives and check if we have set any of the decode flags
    // are set or if we have to improvise.  Also check for any windows.
    //
    improviseDecode = FALSE;
    windowDetected = FALSE;
    greatestPort = 0;
    FOR_ALL_IN_ARRAY(State->Alternatives, State->AlternativeCount, current) {
        PCI_ASSERT(current->Descriptor->Type == CmResourceTypePort);
        PCI_ASSERT(current->Descriptor->Flags == State->Alternatives->Descriptor->Flags);

        //
        // Detect any I/O windows.  For root busses, all I/O resources are
        // windows.  For P-P bridges, only those marked with the window decode
        // flag are.
        //
        if (PCI_DEVICE_OBJECT(deviceObject) &&
            (current->Descriptor->Flags & CM_RESOURCE_PORT_WINDOW_DECODE)) {

            if (current != State->Alternatives) {
                PCI_ASSERT(windowDetected);
            }

            windowDetected = TRUE;
        }

        //
        // See if this requirement doesn't have any of the flags
        // indicating its decode type set.  Remember the maximum
        // value of all the alternatives to try to improvise the
        // decode type.
        //
        if (!(current->Descriptor->Flags &
              (CM_RESOURCE_PORT_10_BIT_DECODE
               | CM_RESOURCE_PORT_12_BIT_DECODE
               | CM_RESOURCE_PORT_16_BIT_DECODE
               | CM_RESOURCE_PORT_POSITIVE_DECODE))) {

            improviseDecode = TRUE;

            if (!LEGACY_REQUEST(State->Entry)) {

                ARB_PRINT(0,
                          ("Pnp device (%p) did not specify decodes for IO ports\n",
                           State->Entry->PhysicalDeviceObject
                          ));
            }
        }

        if (current->Maximum > greatestPort) {
            greatestPort = current->Maximum;
        }
    }

    if (improviseDecode) {

        State->WorkSpace |= PORT_ARBITER_IMPROVISED_DECODE;

        ARB_PRINT(1, ("Improvising decode "));

        switch (State->Entry->InterfaceType) {
        case PNPISABus:
        case Isa:

            //
            // If any of the ports is greater than can be decoded in 10 bits
            // assume a 16 bit decode
            //
            if (greatestPort > MAX_10_BIT_DECODE) {
                defaultDecode = CM_RESOURCE_PORT_16_BIT_DECODE;
                ARB_PRINT(1, ("of 16bit for Isa with ports > 0x3FF\n"));
            } else {
                defaultDecode = CM_RESOURCE_PORT_10_BIT_DECODE;
                ARB_PRINT(1, ("of 10bit for Isa\n"));
            }
            break;

        case Eisa:
        case MicroChannel:
        case PCMCIABus:
            ARB_PRINT(1, ("of 16bit for Eisa/MicroChannel/Pcmcia\n"));
            defaultDecode = CM_RESOURCE_PORT_16_BIT_DECODE;
            break;

        case PCIBus:
            ARB_PRINT(1, ("of positive for PCI\n"));
            defaultDecode = CM_RESOURCE_PORT_POSITIVE_DECODE;
            break;

        default:

            //
            // In NT < 5 we considered everything to be 16 bit decode so in the
            // absence of better information continue that tradition.
            //
            ARB_PRINT(1, ("of 16bit for unknown bus\n"));

            defaultDecode = CM_RESOURCE_PORT_16_BIT_DECODE;
            break;
        }

        FOR_ALL_IN_ARRAY(State->Alternatives, State->AlternativeCount, current) {

            current->Descriptor->Flags |= defaultDecode;
        }

    } else {

        //
        // Even if we are not improvising decodes make sure that they didn't
        // report 10 bit decode for a range over 0x3FF - if so we assume 16 bit
        // decode - think EISA net cards that say they're ISA
        //
        FOR_ALL_IN_ARRAY(State->Alternatives, State->AlternativeCount, current) {

            if ((current->Descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) &&
                (greatestPort > MAX_10_BIT_DECODE )) {

                current->Descriptor->Flags &= ~CM_RESOURCE_PORT_10_BIT_DECODE;
                current->Descriptor->Flags |= CM_RESOURCE_PORT_16_BIT_DECODE;
            }
        }
    }

    //
    // If we detected a bridge window then check if the ISA bit is set on the bridge
    //
    if (windowDetected) {

        //
        // Make sure its a PCI bridge...
        //

        if ((device->HeaderType != PCI_BRIDGE_TYPE) &&
            (device->HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {
            
            ERROR("PciIoArbiter_PreprocessEntry: unexpected header type.\n");
            return STATUS_INVALID_PARAMETER;
        }

        if (device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_ISA) {

            State->WorkSpace |= PORT_ARBITER_ISA_BIT_SET;
        }

        State->WorkSpace |= PORT_ARBITER_BRIDGE_WINDOW;
    }

    //
    // Set some attributes
    //
    PciSetAllocationAttributes(State);

    TRACE(ARBITER_TRACE_LEVEL, "Allocation State after preprocessing\n");
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return STATUS_SUCCESS;
}

BOOLEAN
PciIoArbiter_GetNextAllocationRange(
    __inout PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the GetNextAllocationRange entrypoint for the PCI I/O
    arbiter.  GetNextAllocationRange is the outer loop of finding
    a solution for a requirement.  It loops through entries in the
    arbiter's ordering list, trying to find one that works.  This
    version comprehends the fact that bridges with their ISA bit
    set have a very simple ordering list - they can live either
    in 32 bit space or 16 bit space.  It sets up the ordering list
    for those bridges.

Parameters:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    NT Status code.

--*/
{
    ARBITER_ORDERING_LIST savedOrderingList = {0};
    BOOLEAN rangeFound, doIsaBit;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL,
          "\nPciIoArbiter_GetNextAllocationRange for arbiter %p\n",
          Arbiter
          );

    doIsaBit = BITS_SET(State->WorkSpace,
                        PORT_ARBITER_BRIDGE_WINDOW | PORT_ARBITER_ISA_BIT_SET);


    if (doIsaBit) {
        savedOrderingList = Arbiter->OrderingList;
        Arbiter->OrderingList = PciBridgeOrderingList;
    }

    rangeFound = ArbGetNextAllocationRange(Arbiter, State);

    if (doIsaBit) {

        //
        // If we have reached preferred reserved priority then we fail as we
        // have already considered both the 16 and 32 bit IO cases and using
        // the reserved may allow us to stradle the boundry.
        //
        if (rangeFound &&
            (State->CurrentAlternative->Priority > ARBITER_PRIORITY_PREFERRED_RESERVED)) {

            rangeFound = FALSE;
        }

        Arbiter->OrderingList = savedOrderingList;
    }

    //
    // ArbGetNextAllocationRange may have updated the current alternative.
    // This routine updates the attributes for RtlFindRange based on this change.
    //
    PciSetAllocationAttributes(State);

    if (rangeFound) {
        TRACE(ARBITER_TRACE_LEVEL, "Next Allocation range found\n");
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);
    }

    return rangeFound;
}

BOOLEAN
PciIoArbiter_FindSuitableRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the FindSuitableRange entrypoint for the PCI I/O arbiter.
    FindSuitableRange is the inner loop of the arbitration.  Within
    every range selected by GetNextAllocationRange, this routine loops
    selecting possibilities.  It tries to find a free range that matches the
    requirements in State while restricting its possible solutions to the range
    State->Start to State->CurrentMaximum.  On success State->Start and
    State->End represent this range.  Conflicts between boot configs are allowed.
    This version detects requirements for bridges with the ISA bit set, which
    relaxes the ranges that must be available for the range to be allowed.
    It also makes sure all aliases are available for ranges that don't do
    full 16 bit decode.












Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if we found a range, FALSE otherwise.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    BOOLEAN returnValue;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, "\nPciIoArbiter_FindSuitableRange for arbiter %p\n", Arbiter);

    //
    // On downlevel OSes, a fixed I/O requirement was treated like a boot config
    // if the parent was a root bus or a bridge on its initial I/O window
    // assignment. In this case treating like a boot config means allowing NULL
    // conflicts. This is now deemed unnecessary and is being removed.
    //

    if (State->CurrentAlternative->Length == 0) {

        returnValue = ArbFindSuitableRange(Arbiter, State);

        if (returnValue) {

            ARB_PRINT(2,
                ("Zero length solution solution for %p = 0x%I64x-0x%I64x, %s\n",
                State->Entry->PhysicalDeviceObject,
                State->Start,
                State->End,
                State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_SHARED ?
                    "shared" : "non-shared"
                ));

            //
            // Set the result in the arbiter appropriately so that we
            // don't try and translate this zero requirement - it won't!
            //
            State->Entry->Result = ArbiterResultNullRequest;
        }

        TRACE(ARBITER_TRACE_LEVEL, "\tarbitrating zero length requirement - returning %d\n", returnValue);
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

        return returnValue;
    }

    //
    // Check if this is a window for a bridge with the ISA bit set.
    // If so, the bridge is only decoding the ISA aliases.  Look
    // for a suitable range given this relaxation.
    //
    if ((State->WorkSpace & PORT_ARBITER_BRIDGE_WINDOW) &&
        (State->WorkSpace & PORT_ARBITER_ISA_BIT_SET) &&
        (State->CurrentMaximum <= PCI_BRIDGE_ISA_BIT_MAX)) {

        returnValue = PciFindSuitableIsaBitRange(Arbiter, State);

        TRACE(ARBITER_TRACE_LEVEL, "\tfinding ISA bit window range - returning %d\n", returnValue);
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

        return returnValue;
    }

    //
    // Otherwise, call the default arbitration routine to find
    // a solution, munging as necessary.
    //
    returnValue = FALSE;
    while (State->CurrentMaximum >= State->CurrentMinimum) {

        if (ArbFindSuitableRange(Arbiter, State)) {

            descriptor = State->CurrentAlternative->Descriptor;
            if (!(descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) &&
                !(descriptor->Flags & CM_RESOURCE_PORT_12_BIT_DECODE)) {

                PCI_ASSERT((descriptor->Flags & CM_RESOURCE_PORT_POSITIVE_DECODE) ||
                           (descriptor->Flags & CM_RESOURCE_PORT_16_BIT_DECODE));

                //
                // If the 10 or 12 bit flags are not set, there are no aliases.
                // The range is good.
                //
                returnValue = TRUE;
                break;
            }

            //
            // If all of the aliases of the chosen range are available, the range is good.
            //
            if (PciAllAliasesAvailable(Arbiter, State)) {

                returnValue = TRUE;
                break;
            }

            //
            // Otherwise, update the state and try again.
            //
            if ((State->Start - 1) > State->Start) {
                break;
            }

            State->CurrentMaximum = State->Start - 1;

        } else {

            //
            // The default routine failed, so there's no solution.
            //
            TRACE(ARBITER_TRACE_LEVEL, "ArbFindSuitableRange couldn't find solution\n");
            break;
        }
    }

    TRACE(ARBITER_TRACE_LEVEL, "FindSuitableRange returning %d\n", returnValue);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return returnValue;
}

VOID
PciIoArbiter_AddAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the AddAllocation entry point to the PCI I/O arbiter.
    This routine is called from AllocateEntry once we have found
    a possible solution (State->Start - State->End).  It adds the
    ranges that will not be available if we commit to this solution
    to Arbiter->PossibleAllocation.  This version handles bridges
    with the ISA bit, for which only pieces of the allocated range
    actually need to be considered unavailable.  It also handles
    ranges with aliases, for which all the aliases need to be
    added as unavailable.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    PciAddOrBacktrackIoAllocation(Arbiter,
                                  State,
                                  ArbAddAllocation
                                  );
}

VOID
PciIoArbiter_BacktrackAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State
     )
/*++

Routine Description:

    This is the BacktrackAllocation entry point to the PCI I/O arbiter.
    This routine is called from AllocateEntry if it is determined that
    the range indicated by State->Start will not work (after it was added
    to the range list by AddAllocation).  It removes these ranges
    from the range list. This version handles bridges with the ISA bit,
    for which only pieces of the allocated range actually need to be
    removed.  It also handles ranges with aliases, for which all the
    aliases need to be added as unavailable.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    PciAddOrBacktrackIoAllocation(Arbiter,
                                  State,
                                  ArbBacktrackAllocation
                                  );
}

BOOLEAN
PciIoArbiter_OverrideConflict(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the OverrideConflict entrypoint of the PCI I/O arbiter.
    This is called when a conflict between a possible solution for
    a requirement and a solution for another requirement has been found.
    It determines whether the conflict is real or should be allowed
    to stand.  It allows conflicts between fixed requirements from the same
    device.  Conflicts between passively decoded ranges and NULL are also
    allowed.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if the conflict is allowable, FALSE otherwise

--*/
{
    PPCI_DEVICE ownerExtension;
    RTL_RANGE_LIST_ITERATOR iterator;
    PDEVICE_OBJECT owner;
    PRTL_RANGE current;
    BOOLEAN ok;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, "\nPciIoArbiter_OverrideConflict for arbiter %p\n", Arbiter);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    if (!(State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED)) {
        return FALSE;
    }

    ok = FALSE;
    FOR_ALL_RANGES(Arbiter->PossibleAllocation, &iterator, current) {

        if (INTERSECT(current->Start, current->End,
                      State->CurrentMinimum, State->CurrentMaximum)) {

            if (current->Attributes & State->RangeAvailableAttributes) {

                //
                // We DON'T set ok to true because RtlFindRange would have allowed
                // this conflict as well, so it can't be the cause of the conflict.
                //
                continue;
            }

            //
            // Allow conflicts between fixed requirements from the same device.
            //
            if ((current->Owner == State->Entry->PhysicalDeviceObject) &&
                (State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED) &&
                (current->Attributes & ARBITER_RANGE_FIXED_REQUIREMENT)) {

                ARB_PRINT(1,
                    ("PnP Warning: Device reported self-conflicting requirement\n"
                    ));

                State->Start=State->CurrentMinimum;
                State->End=State->CurrentMaximum;

                ok = TRUE;

                TRACE(ARBITER_TRACE_LEVEL, "Possible allowed self-conflicting range:\n");
                TRACE(ARBITER_TRACE_LEVEL, "%I64x-%I64x Flags-0x%x O=%p U=%p\n",
                      current->Start, current->End,
                      current->Flags, current->Owner,
                      current->UserData
                      );
                continue;
            }

            //
            // If the passive decode flag is set and we conflict with a bridge then
            // allow the conflict.  We also allow the conflict if the range never
            // makes it onto the bus (Owner == NULL)
            //










            if (State->CurrentAlternative->Descriptor->Flags & CM_RESOURCE_PORT_PASSIVE_DECODE) {

                owner = (PDEVICE_OBJECT)current->Owner;
                if (owner && PCI_DEVICE_OBJECT(owner)) {

                    ownerExtension = (PPCI_DEVICE)owner->DeviceExtension;
                } else {
                    ownerExtension = NULL;
                }

                if ((owner == NULL) ||
                    (ownerExtension && (ownerExtension->HeaderType != PCI_DEVICE_TYPE))) {

                    State->Start=State->CurrentMinimum;
                    State->End=State->CurrentMaximum;

                    ok = TRUE;

                    TRACE(ARBITER_TRACE_LEVEL, "Possible allowed conflict with passive decode:\n");
                    TRACE(ARBITER_TRACE_LEVEL, "%I64x-%I64x Flags-0x%x O=%p U=%p\n",
                          current->Start, current->End,
                          current->Flags, current->Owner,
                          current->UserData
                          );
                    continue;
                }
            }

            //
            // The conflict is still valid
            //
            return FALSE;
        }
    }
    return ok;
}

// ------------------------------------------------------------------ I/O Arbiter helpers

VOID
PciAddOrBacktrackIoAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State,
     __in PARBITER_BACKTRACK_ALLOCATION Callback
     )
/*++

Routine Description:

    This relies on the fact that PARBITER_BACKTRACK_ALLOCATION and
    PARBITER_ADD_ALLOCATION are of the same type.  It does the
    work of both AddAllocation and BacktrackAllocation, which
    is the same, except for the work of actually adding or
    backtracking each range.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

    Callback - The routine to execute for each range that needs to
        be added/backtracked.  If this is called from AddAllocation,
        it will be ArbAddAllocation, for instance.

Return Value:

    None.

--*/
{
    ARBITER_ALLOCATION_STATE localState;
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG increment;

    PAGED_CODE();

    PCI_ASSERT(Arbiter);
    PCI_ASSERT(State);

    RtlCopyMemory(&localState, State, sizeof(ARBITER_ALLOCATION_STATE));

    if ((State->WorkSpace & PORT_ARBITER_BRIDGE_WINDOW) &&
        (State->WorkSpace & PORT_ARBITER_ISA_BIT_SET) &&
        (State->Start < PCI_BRIDGE_ISA_BIT_MAX)) {

        //
        // We don't support IO windows that straddle the 16/32 bit boundry
        //
        PCI_ASSERT(State->End <= PCI_BRIDGE_ISA_BIT_MAX);

        //
        // This is a bridge window with the ISA bit, so we only have to
        // add/backtrack up to ISA alias ranges or the end of range as
        // the case may be. The latter case is more appropriate for cardbus
        // bridges.
        //
        increment = PCI_BRIDGE_ISA_BIT_WIDTH - 1;

        for (;
             (localState.Start < State->End) && (localState.Start < PCI_BRIDGE_ISA_BIT_MAX);
             localState.Start += PCI_BRIDGE_ISA_BIT_STRIDE) {

            if ((State->End - localState.Start) >= increment) {
                localState.End = localState.Start + increment;
            } else {
                localState.End = State->End;
            }

            Callback(Arbiter, &localState);
        }

    } else {

        //
        // Process the base range
        //
        Callback(Arbiter, State);

        //
        // and any aliases
        //
        descriptor = State->CurrentAlternative->Descriptor;
        if ((descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) ||
            (descriptor->Flags & CM_RESOURCE_PORT_12_BIT_DECODE)) {

            if (descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) {
                increment = MAX_10_BIT_DECODE + 1;
            } else {

                increment = MAX_12_BIT_DECODE + 1;
            }

            ARB_PRINT(2, ("Adding aliases\n"));

            //
            // Set the alias attribute for ranges other than the base range,
            // as long as the range is being subtractively decoded.  The alias
            // flag indicates that the range can be allowed to conflict with
            // positive decode ranges, since it is both subtractively decoded
            // and not the base range required by the device.  Do not set the
            // flag on aliases of positive decode ranges (which appear most
            // frequently on bridges with the VGA bit set), since this would
            // cause real conflicts between positive decode ranges to occur.
            //
            if (!(State->CurrentAlternative->Descriptor->Flags & CM_RESOURCE_PORT_POSITIVE_DECODE)) {
                localState.RangeAttributes |= ARBITER_RANGE_ALIAS;
            }

            localState.Start += increment;

            while (localState.Start <= 0xFFFF) {

                localState.End = localState.Start + State->CurrentAlternative->Length - 1;

                Callback(Arbiter, &localState);

                localState.Start += increment;
            }
        }
    }
}

VOID
PciSetAllocationAttributes(
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This routine sets attributes of an arbiter allocation based on
    the descriptor the allocation is satisfying.  This must be called
    once from PreprocessEntry to set these flags based on the initial
    descriptor, and then in GetNextAllocationRange to updated it
    for new alternatives within the same allocation.

Arguments:

    State - The allocation state to update.

Return Value:

    VOID

--*/
{
    PARBITER_ALTERNATIVE alternative;

    PAGED_CODE();

    //
    // We want to set attributes based on the current alternative.  In
    // PreprocessEntry, however, the CurrentAlternative hasn't been set up
    // yet.  So set the attributes based on the first alternative in the
    // list, if CurrentAlternative is NULL
    //
    if (State->CurrentAlternative == NULL) {

        alternative = State->Alternatives;

    } else {

        alternative = State->CurrentAlternative;
    }

    //
    // If this request is for a device which performs positive decode
    // mark it as such, and consider all aliased ranges available
    //
    State->RangeAttributes &= ~ARBITER_RANGE_POSITIVE_DECODE;
    State->RangeAvailableAttributes &= ~ARBITER_RANGE_ALIAS;

    if (alternative->Descriptor->Flags & CM_RESOURCE_PORT_POSITIVE_DECODE) {

        State->RangeAttributes |= ARBITER_RANGE_POSITIVE_DECODE;
        State->RangeAvailableAttributes |= ARBITER_RANGE_ALIAS;
    }

    //
    // If the requirement is fixed, mark this in the range attributes so that
    // this information can be used in OverrideConflict later.
    //
    State->RangeAttributes &= ~ARBITER_RANGE_FIXED_REQUIREMENT;
    if (alternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED) {

        State->RangeAttributes |= ARBITER_RANGE_FIXED_REQUIREMENT;
    }

    //
    // For legacy requests from IoAssignResources (directly or by way of
    // HalAssignSlotResources) or IoReportResourceUsage we consider preallocated
    // resources to be available for backward compatibility reasons.
    //
    // If we are allocating a devices boot config then we consider all other
    // boot configs to be available.
    //
    if ((State->Entry->RequestSource == ArbiterRequestLegacyReported) ||
        (State->Entry->RequestSource == ArbiterRequestLegacyAssigned) ||
        (State->Entry->Flags & ARBITER_FLAG_BOOT_CONFIG)) {

        State->RangeAvailableAttributes |= ARBITER_RANGE_BOOT_ALLOCATED;
    }
}

BOOLEAN
PciFindSuitableIsaBitRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This routine finds a suitable range for the given allocation state
    assuming that the requirement being satisfied is for a bridge
    window with its ISA bit set.  In this case, even though the requirement
    asks for a large, contiguous range, only the ISA aliases within
    the range will actually be decoded, so only they need to be
    considered.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if a suitable range can be found.  FALSE otherwise.

--*/
{
    PARBITER_ALTERNATIVE alternative;
    PDEVICE_OBJECT deviceObject;
    PPCI_DEVICE device;
    ULONGLONG length, lengthRemaining;
    ULONGLONG start, current;
    ULONG findRangeFlags;
    ULONG alignment;
    BOOLEAN available;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Validate that the requirement is for a P-P bridge.
    //
    deviceObject = State->Entry->PhysicalDeviceObject;
    device = (PPCI_DEVICE)deviceObject->DeviceExtension;

    if (!PCI_DEVICE_OBJECT(deviceObject)) {
        return FALSE;
    }

    if ((device->HeaderType != PCI_BRIDGE_TYPE) &&
        (device->HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {
        
        ERROR("PciFindSuitableIsaBitRange: unexpected header type.\n");
        return FALSE;
    }

    alternative = State->CurrentAlternative;

    //
    // Do some validation and bounds checking.
    //
    PCI_ASSERT((State->WorkSpace & PORT_ARBITER_BRIDGE_WINDOW) &&
               (State->WorkSpace & PORT_ARBITER_ISA_BIT_SET));

    PCI_ASSERT(alternative->Descriptor->Flags & CM_RESOURCE_PORT_POSITIVE_DECODE);

    if (State->CurrentMaximum < (alternative->Length + 1)) {

        return FALSE;
    }

    //
    // Bridge windows perform positive decode and so can conflict with
    // aliases
    //
    State->RangeAvailableAttributes |= ARBITER_RANGE_ALIAS;

    //
    // If we decided in FindSuitableRange that a NULL conflict was okay
    // in this case, make it so.
    //
    findRangeFlags = 0;
    if (State->Flags & ARBITER_STATE_FLAG_NULL_CONFLICT_OK) {
        findRangeFlags |= RTL_RANGE_LIST_NULL_CONFLICT_OK;
    }

    if (alternative->Flags & ARBITER_ALTERNATIVE_FLAG_SHARED) {
        findRangeFlags |= RTL_RANGE_LIST_SHARED_OK;
    }

    //
    // If the alternative's alignment is less than the isa bit stride,
    // consider the *effective* alignment to be the isa bit stride, so that
    // only the beginning of strides are tested.  The asserts are paranoia.
    // Natural power of 2 alignment of BARs should prevent then from hitting.
    //
    if (alternative->Alignment < PCI_BRIDGE_ISA_BIT_STRIDE) {

        PCI_ASSERT((PCI_BRIDGE_ISA_BIT_STRIDE % alternative->Alignment) == 0);
        alignment = PCI_BRIDGE_ISA_BIT_STRIDE;

    } else {

        PCI_ASSERT((alternative->Alignment % PCI_BRIDGE_ISA_BIT_STRIDE) == 0);
        PCI_ASSERT(alternative->Alignment <= MAXULONG);
        alignment = (ULONG)alternative->Alignment;
    }

    //
    // Iterate through the possible window positions, top down like the rest of
    // arbitration.
    //
    start = State->CurrentMaximum - alternative->Length + 1;
    start = ALIGN_RANGE_DOWN(start, (ULONGLONG)alignment);

    available = FALSE;
    while (!available) {

        if (start < State->CurrentMinimum) {
            break;
        }

        //
        // We only test the beginning of ISA bit strides.  This relies on
        // the alternative being aligned to the size of an ISA bit stride
        // and this loop incrementing start by an ISA bit stride.
        //
        PCI_ASSERT((start % PCI_BRIDGE_ISA_BIT_STRIDE) == 0);

        current = start;
        lengthRemaining = alternative->Length;

        //
        // We only care about the ISA alias ranges, not the entire range.
        //
        while (current < PCI_BRIDGE_ISA_BIT_MAX) {

            length = min(PCI_BRIDGE_ISA_BIT_WIDTH, lengthRemaining);

            status = RtlIsRangeAvailable(
                         Arbiter->PossibleAllocation,
                         current,
                         current + length - 1,
                         findRangeFlags,
                         State->RangeAvailableAttributes,
                         Arbiter->ConflictCallbackContext,
                         Arbiter->ConflictCallback,
                         &available
                         );

            PCI_ASSERT(NT_SUCCESS(status));

            //
            // If this alias isn't available, the allocation failed.
            // If the last of the aliases has been tested, the allocation
            // succeeded.  Either way, break out of the loop.
            //
            if (!available || (lengthRemaining <= PCI_BRIDGE_ISA_BIT_STRIDE)) {
                break;
            }

            lengthRemaining -= PCI_BRIDGE_ISA_BIT_STRIDE;
            current += PCI_BRIDGE_ISA_BIT_STRIDE;
        }

        if (available) {

            State->Start = start;
            State->End = start + alternative->Length - 1;

            PCI_ASSERT(State->Start >= State->CurrentMinimum);
            PCI_ASSERT(State->End <= State->CurrentMaximum);

            break;

        } else {

            //
            // Move to next range if we can
            //
            if (start < alignment) {
                break;
            }

            start -= alignment;
            continue;
        }
    }

    return available;
}

BOOLEAN
PciAllAliasesAvailable(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This routine determines if the aliases of the range described
    in the given State are available.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if the aliases are available, FALSE otherwise.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    BOOLEAN aliasAvailable;
    UCHAR userFlagsMask;
    NTSTATUS status;
    ULONGLONG alias;
    ULONG increment;

    PAGED_CODE();

    descriptor = State->CurrentAlternative->Descriptor;

    //
    // If the 10/12 bit decode flags aren't set, there are no aliases,
    // so by definition all aliases are available.
    //
    if (!(descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) &&
        !(descriptor->Flags & CM_RESOURCE_PORT_12_BIT_DECODE)) {
         
        ERROR("PciAllAliasesAvailable: 10/12 bit decode flags not set.\n");
        return TRUE;
    }

    if (descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) {

        increment = MAX_10_BIT_DECODE + 1;

    } else {

        increment = MAX_12_BIT_DECODE + 1;
    }

    //
    // If we improvised the decode then add the aliases but don't care it they
    // conflict - more 4.0 compatibility.
    //
    if (State->WorkSpace & PORT_ARBITER_IMPROVISED_DECODE) {

        return TRUE;
    }

    //
    // Positive decode devices can conflict with aliases
    //
    userFlagsMask = ARBITER_RANGE_POSITIVE_DECODE;

    //
    // For legacy requests from IoAssignResources (directly or by way of
    // HalAssignSlotResources) or IoReportResourceUsage we consider preallocated
    // resources to be available for backward compatibility reasons.
    //
    if ((State->Entry->RequestSource == ArbiterRequestLegacyReported) ||
        (State->Entry->RequestSource == ArbiterRequestLegacyAssigned) ||
        (State->Entry->Flags & ARBITER_FLAG_BOOT_CONFIG)) {

        userFlagsMask |= ARBITER_RANGE_BOOT_ALLOCATED;
    }

    for (alias = State->Start + increment; alias <= 0xFFFF; alias += increment) {

        status = RtlIsRangeAvailable(
                     Arbiter->PossibleAllocation,
                     alias,
                     alias + State->CurrentAlternative->Length - 1,
                     RTL_RANGE_LIST_NULL_CONFLICT_OK |
                        (State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_SHARED ?
                            RTL_RANGE_LIST_SHARED_OK : 0),
                     userFlagsMask,
                     Arbiter->ConflictCallbackContext,
                     Arbiter->ConflictCallback,
                     &aliasAvailable
                     );

        PCI_ASSERT(NT_SUCCESS(status));

        if (!aliasAvailable) {

            ARBITER_ALLOCATION_STATE tempState;

            //
            // Check if we allow this conflict by calling OverrideConflict -
            // we will need to falsify ourselves an allocation state first
            //











            RtlCopyMemory(&tempState, State, sizeof(ARBITER_ALLOCATION_STATE));

            tempState.CurrentMinimum = alias;
            tempState.CurrentMaximum = alias + State->CurrentAlternative->Length - 1;

            if (Arbiter->OverrideConflict(Arbiter, &tempState)) {

                //
                // We decided this conflict was ok so contine checking the rest
                // of the aliases
                //
                continue;

            }

            TRACE(ARBITER_TRACE_LEVEL,
                  "Alias 0x%I64x-0x%I64x not available\n",
                   alias,
                   alias + State->CurrentAlternative->Length - 1
                   );

            return FALSE;
        }
    }

    return TRUE;
}

NTSTATUS
PciExcludeRangesFromWindow(
    __in ULONGLONG Start,
    __in ULONGLONG End,
    __inout PRTL_RANGE_LIST ArbiterRanges,
    __in PRTL_RANGE_LIST ExclusionRanges
    )
/*++

Routine Description:

    This routine excludes the ranges in the range list
    ExclusionRanges from the range list ArbiterRanges, if they
    intersect with the range specified by Start and End.

Arguments:

    Start - The start of the range to search for in ExclusionRangs.

    End - The end of this range.

    ArbiterRanges - The range list to exclude the ranges from.

    ExclusionRanges - The range list whose ranges that intersect with
        the Start/End pair should be excluded.

Return Value:

    NT Status code.

--*/
{
    RTL_RANGE_LIST_ITERATOR iterator;
    PRTL_RANGE current;
    NTSTATUS status;

    PAGED_CODE();

    FOR_ALL_RANGES(ExclusionRanges, &iterator, current) {

        if ((current->Owner == NULL) &&
            INTERSECT(current->Start, current->End, Start, End)) {

            status = RtlAddRange(ArbiterRanges,
                                 current->Start,
                                 current->End,
                                 0,
                                 RTL_RANGE_LIST_ADD_IF_CONFLICT,
                                 NULL,
                                 NULL // Owner - this range is not on the bus
                                 );

            if (!NT_SUCCESS(status)) {
                ERROR("PciExcludeRangesFromWindow: RtlAddRange failed (0x%08X).\n", status);
                return status;
            }
        }
    }

    return STATUS_SUCCESS;
}
