/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    arbmemory.c

Abstract:

    This module contains functions for arbitrating memory resources.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

//
// Flags for RangeAttributes
//
#define MEMORY_RANGE_ROM    0x10

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciInitializeMemoryOrderingLists(
    __inout PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount
    );

VOID
PciRollbackMemoryOrderingLists(
    __in PARBITER_INSTANCE Arbiter
    );

NTSTATUS
PciMemoryArbiter_TestAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_TEST_ALLOCATION_PARAMETERS Parameters
    );

NTSTATUS
PciMemoryArbiter_RetestAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_RETEST_ALLOCATION_PARAMETERS Parameters
    );

NTSTATUS
PciMemoryArbiter_RollbackAllocation(
    __in PARBITER_INSTANCE Arbiter
    );

NTSTATUS
PciMemoryArbiter_CommitAllocation(
    __in PARBITER_INSTANCE Arbiter
    );

NTSTATUS
PciMemoryArbiter_ReinitializeArbiter(
    _Inout_ PARBITER_INSTANCE Arbiter,
    _In_ ULONG ResourceCount,
    _In_ PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    _Inout_ PRTL_RANGE_LIST RangeList
    );

NTSTATUS
PciMemoryArbiter_QueryArbitrate(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_QUERY_ARBITRATE_PARAMETERS Parameters
    );

NTSTATUS
PciMemoryArbiter_PreprocessEntry(
    __inout PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciMemoryArbiter_GetNextAllocationRange(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciMemoryArbiter_FindSuitableRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

BOOLEAN
PciMemoryArbiter_OverrideConflict(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    );

// --------------------------------------------------------------------- Pragmas

#pragma prefast(disable:__WARNING_UNUSED_SCALAR_ASSIGNMENT, "checked builds")

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciInitializeMemoryArbiter)
    #pragma alloc_text(PAGE, PciProgramMemoryArbiter)
    #pragma alloc_text(PAGE, PciInitializeMemoryOrderingLists)
    #pragma alloc_text(PAGE, PciRollbackMemoryOrderingLists)
    #pragma alloc_text(PAGE, PciMemoryArbiter_TestAllocation)
    #pragma alloc_text(PAGE, PciMemoryArbiter_RetestAllocation)
    #pragma alloc_text(PAGE, PciMemoryArbiter_RollbackAllocation)
    #pragma alloc_text(PAGE, PciMemoryArbiter_CommitAllocation)
    #pragma alloc_text(PAGE, PciMemoryArbiter_ReinitializeArbiter)
    #pragma alloc_text(PAGE, PciMemoryArbiter_QueryArbitrate)
    #pragma alloc_text(PAGE, PciMemoryArbiter_PreprocessEntry)
    #pragma alloc_text(PAGE, PciMemoryArbiter_GetNextAllocationRange)
    #pragma alloc_text(PAGE, PciMemoryArbiter_FindSuitableRange)
    #pragma alloc_text(PAGE, PciMemoryArbiter_OverrideConflict)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciInitializeMemoryArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *MemoryArbiter
    )
/*++

Routine Description:

    This routine is called once per instantiation of a memory arbiter.
    Performs initialization of this instantiation's context.

Parameters:

    Bus - The Bus that this arbiter is being initialized for.
    
    MemoryArbiter - The arbiter to initialize.
    
    Resources - The resources to use to initialize this arbiter.
    
    ResourceCount - The number of resources in the Resources array.

Return Value:

    Returns the status of this operation.

--*/
{
    PARBITER_INSTANCE arbiter;
    PWCHAR arbiterName;
    NTSTATUS status;

    PAGED_CODE();

    *MemoryArbiter = NULL;
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
    arbiter->TestAllocation          = PciMemoryArbiter_TestAllocation;
    arbiter->RetestAllocation        = PciMemoryArbiter_RetestAllocation;
    arbiter->RollbackAllocation      = PciMemoryArbiter_RollbackAllocation;
    arbiter->CommitAllocation        = PciMemoryArbiter_CommitAllocation;
    arbiter->InitializeRangeList     = PciMemoryArbiter_ReinitializeArbiter;
    arbiter->PreprocessEntry         = PciMemoryArbiter_PreprocessEntry;
    arbiter->GetNextAllocationRange  = PciMemoryArbiter_GetNextAllocationRange;
    arbiter->FindSuitableRange       = PciMemoryArbiter_FindSuitableRange;
    arbiter->OverrideConflict        = PciMemoryArbiter_OverrideConflict;
    arbiter->QueryArbitrate          = PciMemoryArbiter_QueryArbitrate;

    arbiter->UnpackRequirement = PciArbiter_UnpackRequirement;
    arbiter->PackResource      = PciArbiter_PackResource;
    arbiter->UnpackResource    = PciArbiter_UnpackResource;
    arbiter->ScoreRequirement  = PciArbiter_ScoreRequirement;











    //
    // Allocate and zero the arbiter extension, it is initialized in
    // armem_StartArbiter
    //
    arbiter->Extension = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, 
                                         sizeof(PCI_ARBITER_MEMORY_EXTENSION)
                                         );
    if (arbiter->Extension == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    RtlZeroMemory(arbiter->Extension, sizeof(PCI_ARBITER_MEMORY_EXTENSION));

    //
    // Name the arbiter.
    //
    arbiterName = PciAllocatePool(NonPagedPoolNx, sizeof(L"PCI Memory (b=xxxxxxxx)"));
    if (arbiterName == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    status = RtlStringCbPrintfW(arbiterName,
                                sizeof(L"PCI Memory (b=xxxxxxxx)"),
                                L"PCI Memory (b=%x)",
                                Bus->SecondaryBusNumber
                                );

    PCI_ASSERT(NT_SUCCESS(status));    
    
    //
    // Initialize the arbiter library's fields.
    //
    status = ArbInitializeArbiterInstance(arbiter,
                                          Bus->DeviceObject,
                                          CmResourceTypeMemory,
                                          arbiterName,
                                          L"Pci",
                                          NULL
                                          );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    *MemoryArbiter = arbiter;

    return STATUS_SUCCESS;

cleanup:

    if (arbiterName) {
        PciFreePool(arbiterName);
    }
    if (arbiter) {
        
        if (arbiter->Extension) {
            PciFreePool(arbiter->Extension);
        }
        PciFreePool(arbiter);
    }

    return status;
}

NTSTATUS
PciProgramMemoryArbiter(
    __in PPCI_BUS Bus,
    __inout PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount,
    __out PRTL_RANGE_LIST RangeList
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
    NTSTATUS status;

    PAGED_CODE();

    //
    // Initialize the range list with the resources provided.
    //
    status = PciRangeListFromResourceList(Bus,
                                          Arbiter,
                                          Resources,
                                          ResourceCount,
                                          RangeList
                                          );
    if (!NT_SUCCESS(status)) {
        
        TRACE(ARBITER_TRACE_LEVEL, "Could not program arbiter with new resources.\n");
        return status;
    }

    //
    // Invoke the arbiter library routine to add the inaccessible 
    // range list (if any) for the root bus.
    //
    if (PCI_ROOT_BUS(Bus)) {

        status = ArbAddInaccessibleAllocationRange(Arbiter, RangeList);
        if (!NT_SUCCESS(status)) {
            TRACE(ARBITER_TRACE_LEVEL, "ArbAddInaccessibleAllocationRange "
                                       "failed with status %x.\n", status);

            return status;
        }
    }

    //
    // Invoke the arbiter library routine to reserve the PCI memory mapped
    // config range in the RangeList. This makes the config range unavailable
    // for arbitration, though devices that have boot resource in the PCI
    // config range will succeed their allocations in the config range.
    //
    status = ArbAddMmConfigRangeAsBootReserved(Arbiter, RangeList);
    if (!NT_SUCCESS(status)) {
        TRACE(ARBITER_TRACE_LEVEL, "ArbAddMmConfigRangeAsBootReserved "
                                   "failed with status %x.\n", status);

        return status;
    }

    //
    // Munge the arbiter based on these new resources.
    //
    status = PciInitializeMemoryOrderingLists(Arbiter,
                                              Resources,
                                              ResourceCount
                                              );
    return status;
}

NTSTATUS
PciInitializeMemoryOrderingLists(
    __inout PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount
    )
/*++

Routine Description:
   
    This is called after ArbInitializeArbiterInstance as it uses
    information initialized there.  The arbiter lock should be held.
    Seeing as this is only applicable to memory descriptors we manipulate
    the resource descriptors directlty as opposed to using the pack/unpack
    routines in the arbiter.

    Example:

    StartResources contain the prefetchable range 0xfff00000 - 0xfffeffff

    OriginalOrdering (from the registry) says:
        0x00100000 - 0xFFFFFFFF
        0x000F0000 - 0x000FFFFF
        0x00080000 - 0x000BFFFF
        0x00080000 - 0x000FFFFF
        0x00080000 - 0xFFBFFFFF

    ReservedList contains 0xfff0a000-0xfff0afff

    Then out ordering lists will be:

        PrefetchableOrdering            NonprefetchableOrdering

        0xFFF0B000 - 0xFFFEFFFF
        0xFFF00000 - 0xFFF09FFF
        0xFFFF0000 - 0xFFFFFFFF         0xFFF0b000 - 0xFFFFFFFF
        0x00100000 - 0xFFFEFFFF         0x00100000 - 0xFFF09FFF
        0x000F0000 - 0x000FFFFF         0x000F0000 - 0x000FFFFF
        0x00080000 - 0x000BFFFF         0x00080000 - 0x000BFFFF
        0x00080000 - 0x000FFFFF         0x00080000 - 0x000FFFFF

     This means that when following the prefetchable ordering we try to
     allocate in the prefetchable range and if we can't then we allocate
     none prefetchable memory.  In the Nonprefetchable ordering we avoid the
     prefetchable ranges. GetNextAllocationRange is changed so that it will not
     allow
     
Parameters:

    Arbiter - The arbiter for which to initialize the ordering lists.
    
    Resources - Array of resources that are being used to create the range list
        for this arbiter.  
        
    ResourceCount - The number of resources in the Resources array.
    
Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR current;
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    PARBITER_ORDERING currentOrdering;
    ULONGLONG start, end;
    NTSTATUS status;

    PAGED_CODE();

    extension = Arbiter->Extension;

    switch (extension->ArbiterState) {
        
    case PCI_ARBITER_STATE_UNINITIALIZED:

        //
        // Copy the default memory ordering list from the arbiter
        //
        extension->OriginalOrdering = Arbiter->OrderingList;
        RtlZeroMemory(&Arbiter->OrderingList, sizeof(ARBITER_ORDERING_LIST));
        break;

    case PCI_ARBITER_STATE_REPROGRAMMED:

        //
        // We are reinitializing the arbiter, so free anything we
        // allocated during the first initialization.
        //
        PCI_ASSERT(extension->Saved.PrefetchableOrdering.Orderings == NULL);
        PCI_ASSERT(extension->Saved.NonprefetchableOrdering.Orderings == NULL);
        RtlCopyMemory(&extension->Saved, 
                      &extension->Current, 
                      sizeof(PCI_ARBITER_PREFETCHABLE_STATE)
                      ); 

        extension->PrefetchableStateSaved = TRUE;
        break;

    case PCI_ARBITER_STATE_CONFIGURED:
    default:
        break;
    }
    
    RtlZeroMemory(&extension->Current, sizeof(PCI_ARBITER_PREFETCHABLE_STATE));

    //
    // Check if we have any prefetchable memory - if not we're done
    //
    if (Resources != NULL) {

        FOR_ALL_IN_ARRAY(Resources, ResourceCount, current) {

            if (((current->Type == CmResourceTypeMemory) ||
                 (current->Type == CmResourceTypeMemoryLarge)) &&  
                (current->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE)) {
                
                extension->Current.PrefetchablePresent = TRUE;
                break;
            }
        }
    }

    if (!extension->Current.PrefetchablePresent) {

        //
        // Restore the original ordering list
        //
        Arbiter->OrderingList = extension->OriginalOrdering;
        return STATUS_SUCCESS;
    }

    status = ArbInitializeOrderingList(&extension->Current.PrefetchableOrdering);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Copy of the original ordering into the new Nonprefetchable ordering
    //
    status = ArbCopyOrderingList(&extension->Current.NonprefetchableOrdering,
                                 &extension->OriginalOrdering
                                 );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Add the range 0-MAXULONGLONG to the list so we will calculate the reserved
    // orderings in the list.  This will ensure that we don't give a half
    // prefetchable and half not range to a device.  Prefetchable devices should
    //  probably be able to deal with this but its asking for trouble!
    //
    status = ArbAddOrdering(&extension->Current.NonprefetchableOrdering,
                            0,
                            MAXULONGLONG
                            );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // N.B. In initialized arbiter ordering list, the count field can never
    //      exceed the maximum.
    //
    PCI_ASSERT(extension->Current.NonprefetchableOrdering.Count <
               extension->Current.NonprefetchableOrdering.Maximum);

    __assume(extension->Current.NonprefetchableOrdering.Count <
             extension->Current.NonprefetchableOrdering.Maximum);
   
    //
    // For each prefetchable range delete it from the nonprefetchabe ordering
    // and add it to the prefetchable one.
    //
    // NB - We take it "to be self evident that that all prefetchable memory is
    // created equal" and therefore initialize the ordering list in the order
    // the prefetchable memory desciptors are found in the resource list.
    //
    FOR_ALL_IN_ARRAY(Resources, ResourceCount, current) {

        if (((current->Type == CmResourceTypeMemory) ||
             (current->Type == CmResourceTypeMemoryLarge)) &&  
            (current->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE)) {

            ULONGLONG length;
            
            extension->Current.PrefetchableCount++;

            length = RtlCmDecodeMemIoResource(current, &start);
            end = start + length - 1;
            
            //
            // Add to the prefetchable ordering
            //
            status = ArbAddOrdering(&extension->Current.PrefetchableOrdering, start, end);
            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }

            //
            // And prune it from the Nonprefetchable ordering
            //
            status = ArbPruneOrdering(&extension->Current.NonprefetchableOrdering, 
                                      start, 
                                      end
                                      );
            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }

            ARB_PRINT(1,("Processed prefetchable range 0x%I64x-0x%I64x\n", start, end));
        }
    }

    //
    // Now prune out any explicitly reserved ranges from our new prefetchable
    // ordering - these have already been precalculated into the Nonprefetchable
    // ordering
    //
    FOR_ALL_IN_ARRAY(Arbiter->ReservedList.Orderings,
                     Arbiter->ReservedList.Count,
                     currentOrdering) {

        status = ArbPruneOrdering(&extension->Current.PrefetchableOrdering,
                                  currentOrdering->Start,
                                  currentOrdering->End
                                  );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    //
    // Finally append the Nonprefetchable ordering onto the end of the prefetchable
    //
    FOR_ALL_IN_ARRAY(extension->Current.NonprefetchableOrdering.Orderings,
                     extension->Current.NonprefetchableOrdering.Count,
                     currentOrdering) {

        status = ArbAddOrdering(&extension->Current.PrefetchableOrdering,
                                currentOrdering->Start,
                                currentOrdering->End
                               );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    return STATUS_SUCCESS;

cleanup:

    return status;
}

VOID
PciRollbackMemoryOrderingLists(
    __in PARBITER_INSTANCE Arbiter
    )
{
    PPCI_ARBITER_MEMORY_EXTENSION extension;

    PAGED_CODE();
    
    extension = (PPCI_ARBITER_MEMORY_EXTENSION)Arbiter->Extension;

    if (extension->PrefetchableStateSaved) {
        ArbFreeOrderingList(&extension->Current.PrefetchableOrdering);
        ArbFreeOrderingList(&extension->Current.NonprefetchableOrdering);
    
        RtlCopyMemory(&extension->Current, 
                      &extension->Saved, 
                      sizeof(PCI_ARBITER_PREFETCHABLE_STATE)
                      );
        RtlZeroMemory(&extension->Saved, sizeof(PCI_ARBITER_PREFETCHABLE_STATE));    
    }
    
    extension->PrefetchableStateSaved = FALSE;
}

// ------------------------------------------------------------ Memory Arbiter entrypoints

NTSTATUS
PciMemoryArbiter_TestAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_TEST_ALLOCATION_PARAMETERS Parameters
    )
{
    NTSTATUS status;
    
    PAGED_CODE();
    
    status = ArbTestAllocation(Arbiter, Parameters);
    if (!NT_SUCCESS(status)) {
        
        PciRollbackMemoryOrderingLists(Arbiter);
    }

    return status;
}

NTSTATUS
PciMemoryArbiter_RetestAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_RETEST_ALLOCATION_PARAMETERS Parameters
    )
{
    NTSTATUS status;
    
    PAGED_CODE();
    
    status = ArbRetestAllocation(Arbiter, Parameters);
    if (!NT_SUCCESS(status)) {
        
        PciRollbackMemoryOrderingLists(Arbiter);
    }

    return status;
}

NTSTATUS
PciMemoryArbiter_RollbackAllocation(
    __in PARBITER_INSTANCE Arbiter
    )
{
    PAGED_CODE();
    
    PciRollbackMemoryOrderingLists(Arbiter);

    return ArbRollbackAllocation(Arbiter);
}

NTSTATUS
PciMemoryArbiter_CommitAllocation(
    __in PARBITER_INSTANCE Arbiter
    )
{
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    
    PAGED_CODE();
    
    extension = (PPCI_ARBITER_MEMORY_EXTENSION)Arbiter->Extension;
    ArbFreeOrderingList(&extension->Saved.PrefetchableOrdering);
    ArbFreeOrderingList(&extension->Saved.NonprefetchableOrdering);
    RtlZeroMemory(&extension->Saved, sizeof(PCI_ARBITER_PREFETCHABLE_STATE));
    extension->PrefetchableStateSaved = FALSE;

    return ArbCommitAllocation(Arbiter);
}

NTSTATUS
PciMemoryArbiter_ReinitializeArbiter(
    _Inout_ PARBITER_INSTANCE Arbiter,
    _In_ ULONG ResourceCount,
    _In_ PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    _Inout_ PRTL_RANGE_LIST RangeList
    )
/*++

Routine Description:

    This is the ReInitializeArbiter entrypoint for the PCI memory arbiter.
    This takes a set of resources being speculatively assigned to the
    bridge this arbiter is for, and initializes the arbiter with these
    resources.  This is used during rebalance.  After this call, a speculative
    arbitration using these resources will take place, and if that is
    successful, the bridge will be reinitialized with these resources.
   
Parameters:

    Arbiter - The arbiter to re-initialize.
    
    ResourceCount - The number of resources in the Resources array.
    
    Resources - The resources to use to re-initialize the arbiters.
    
    RangeList - The range list to initialize with the Resources.
    
Return Value:

    NT Status code.
    
--*/
{
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    PCI_BRIDGE_RESOURCES bridgeResources;
    ULONG arbiterState;
    NTSTATUS status;
    PPCI_BUS bus;
    
    PAGED_CODE();

    bus = (PPCI_BUS)Arbiter->BusDeviceObject->DeviceExtension;
    if (!bus->PciBridge) {
        ERROR("PciMemoryArbiter_ReinitializeArbiter: PciBridge not intialized.\n");
        return STATUS_INVALID_PARAMETER;
    }

    extension = Arbiter->Extension;

    arbiterState = extension->ArbiterState;
    extension->ArbiterState = PCI_ARBITER_STATE_REPROGRAMMED;
    
    TRACE(ARBITER_TRACE_LEVEL, "\nProgramming arbiter %p with new resources:\n", Arbiter);
    DebugDumpResourceArray(ARBITER_TRACE_LEVEL, Resources, ResourceCount);

    status = PciBridgeProcessStartResources(bus->PciBridge,
                                            Resources,
                                            ResourceCount,
                                            &bridgeResources
                                            );
    if (!NT_SUCCESS(status)) {
        
        TRACE(ARBITER_TRACE_LEVEL, "Could not process start resources - 0x%x.\n", status);
        goto cleanup;
    }
    
    //
    // Now free up the old range list and re-program it with the new resources.
    //
    RtlFreeRangeList(RangeList);

    status = PciProgramMemoryArbiter(bus,
                                     Arbiter,
                                     bridgeResources.ByIndex,
                                     PCI_BRIDGE_RESOURCE_COUNT,
                                     RangeList
                                     );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    return status;

cleanup:

    extension->ArbiterState = arbiterState;
    return status;
}

NTSTATUS
PciMemoryArbiter_QueryArbitrate(
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
              "PciMemoryArbiter_QueryArbitrate: Not arbitrating request.\n"
              );
        return STATUS_UNSUCCESSFUL;
    }
}

NTSTATUS
PciMemoryArbiter_PreprocessEntry(
    __inout PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the PreprocessEntry entrypoint for the PCI memory arbiter.
    The entry to preprocess corresponds to a requirement that needs
    to be arbitrated for.  This routine gets called before the arbiter
    actually tries to find a solution for this requirement, and is
    used to set some flags for use by RtlFindRange, and to select the
    correct ordering list that should be used for this requirement,
    based on whether it's a requirement for prefetchable memory or not.
   
Parameters:

    Arbiter - The arbiter being used to arbitrate this requirement.
    
    State - The current allocation state of the requirement.
    
Return Value:

    NT Status code.
    
--*/
{
    PPCI_ARBITER_MEMORY_EXTENSION extension;
    PARBITER_ALTERNATIVE current;
    BOOLEAN prefetchable;
    PPCI_DEVICE device;

    PAGED_CODE();

    extension = Arbiter->Extension;
    PCI_ASSERT(extension);
    TRACE(ARBITER_TRACE_LEVEL, "\nPciMemoryArbiter_PreprocessEntry for arbiter %p\n", Arbiter);

    //
    // If this is a PnP arbitration request for a PCI PDO, make sure the PDO
    // isn't already controlled by a legacy driver.
    //
    if (PCI_DEVICE_OBJECT(State->Entry->PhysicalDeviceObject) &&
        (State->Entry->RequestSource == ArbiterRequestPnpEnumerated)) {
        
        device = (PPCI_DEVICE)State->Entry->PhysicalDeviceObject->DeviceExtension;    
        PCI_ASSERT(PCI_DEVICE_EXTENSION(device));
        if (device->LegacyDriver) {
            return STATUS_DEVICE_BUSY;
        }
    }

    //
    // Check if this is a request for a ROM - read only fixed range requirement.
    // If so, then allow Null conflicts.
    //
    // N. B. On downlevel OSes a fixed, read-write, single alternative, legacy
    //       requirement was allowed to conflict with NULL (not-on-bus). It is
    //       assumed that this allowed ISA ROM and VGA frame buffer requirement
    //       to succeed arbitration. Removing this for now to see if it needs
    //       to be revisited.
    //

    if ((State->Alternatives[0].Descriptor->Flags & 
         CM_RESOURCE_MEMORY_READ_ONLY) &&
        (State->Alternatives[0].Flags & ARBITER_ALTERNATIVE_FLAG_FIXED)) {

        //
        // Consider other ROMS to be available.
        //

        State->RangeAvailableAttributes |= MEMORY_RANGE_ROM;

        //
        // Mark this range as a ROM.
        //

        State->RangeAttributes |= MEMORY_RANGE_ROM;

        //
        // Allow NULL conflicts.
        //

        State->Flags |= ARBITER_STATE_FLAG_NULL_CONFLICT_OK;
    }

    //
    // Check if this is a request for prefetchable memory and select
    // the correct ordering list
    //
    if (extension->Current.PrefetchablePresent) {

        //
        // In NT < 5 IoReportResourceUsage had no notion of prefetchable memory
        // so in order to be backward compatible we hope the BIOS/firmware got
        // it right!
        //
        if (State->Entry->RequestSource == ArbiterRequestLegacyReported) {
            Arbiter->OrderingList = extension->OriginalOrdering;
            return STATUS_SUCCESS;
        }

        prefetchable = BITS_SET(State->Alternatives[0].Descriptor->Flags,
                                CM_RESOURCE_MEMORY_PREFETCHABLE
                                );

        if (prefetchable) {

            Arbiter->OrderingList = extension->Current.PrefetchableOrdering;

        } else {

            Arbiter->OrderingList = extension->Current.NonprefetchableOrdering;
        }

        //
        // Make sure that all the alternatives are of the same type
        //
    
        FOR_ALL_IN_ARRAY(State->Alternatives, State->AlternativeCount, current) {
    
            PCI_ASSERT(BITS_SET(current->Descriptor->Flags,CM_RESOURCE_MEMORY_PREFETCHABLE)
                        == prefetchable);
        }
    }

    TRACE(ARBITER_TRACE_LEVEL, "Allocation State after preprocessing\n");
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return STATUS_SUCCESS;
}

BOOLEAN
PciMemoryArbiter_GetNextAllocationRange(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the GetNextAllocationRange entrypoint for the PCI memory 
    arbiter.  GetNextAllocationRange is the outer loop of finding
    a solution for a requirement.  It loops through entries in the
    arbiter's ordering list, trying to find one that works.  This
    routine comprehends the fact that the ordering lists in the
    arbiter have been specially designed to include reserved ranges,
    so if we're processing a reserved range, we have already
    processed all the actual possibilities, and we can fail.
   
Parameters:

    Arbiter - The arbiter being used to arbitrate this requirement.
    
    State - The current allocation state of the requirement.
    
Return Value:

    NT Status code.
    
--*/
{
    PPCI_ARBITER_MEMORY_EXTENSION extension = Arbiter->Extension;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, 
          "\nPciMemoryArbiter_GetNextAllocationRange for arbiter %p\n", 
          Arbiter
          );

    //
    // Call the default implementation
    //
    if (!ArbGetNextAllocationRange(Arbiter, State)) {
        return FALSE;
    }

    if ((extension->Current.PrefetchablePresent) &&  
        (State->Entry->RequestSource != ArbiterRequestLegacyReported)) {

        //
        // We have already precalculated the reserved ranges into the ordering
        // so if we end up in the reserved ranges we're out of luck...
        //
        if (State->CurrentAlternative->Priority > ARBITER_PRIORITY_PREFERRED_RESERVED) {
            return FALSE;
        }
    }

    TRACE(ARBITER_TRACE_LEVEL, "Next Allocation range found\n");
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return TRUE;
}

BOOLEAN
PciMemoryArbiter_FindSuitableRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the FindSuitableRange entrypoint for the PCI memory arbiter.
    FindSuitableRange is the inner loop of the arbitration.  Within 
    every range selected by GetNextAllocationRange, this routine loops 
    selecting possibilities.  It tries to find a free range that matches the
    requirements in State while restricting its possible solutions to the range
    State->Start to State->CurrentMaximum.  On success State->Start and
    State->End represent this range.  Conflicts between boot configs are allowed

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if we found a range, FALSE otherwise.

--*/
{
    BOOLEAN returnValue;

    PAGED_CODE();

    //
    // If this was a boot config then consider other boot configs to be
    // available
    //
    if (State->Entry->Flags & ARBITER_FLAG_BOOT_CONFIG) {
        State->RangeAvailableAttributes |= ARBITER_RANGE_BOOT_ALLOCATED;
    }

    //
    // Make a note in the allocation state if the current alternative is a
    // fixed requirement.  If the allocation is successful, updating the
    // allocation state in this manner ensures that the fixed requirement
    // indicator propagates into the allocated range list entry where it can
    // be used to provide conflict override semantics during future
    // allocations.
    //
    State->RangeAttributes &= ~ARBITER_RANGE_FIXED_REQUIREMENT;
    if (State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED) {
        State->RangeAttributes |= ARBITER_RANGE_FIXED_REQUIREMENT;      
    }

    //
    // Do the default thing
    //
    returnValue = ArbFindSuitableRange(Arbiter, State);

    //
    // If successful, then we are assured that no conflict with the MCFG table
    // exists.  Unset the flag set on any earlier attempts.
    //

    if (returnValue) {
        State->Flags &= ~ARBITER_STATE_FLAG_MCFG_CONFLICT;
    }

    TRACE(ARBITER_TRACE_LEVEL, "\nPciMemoryArbiter_FindSuitableRange returning %d\n", returnValue);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return returnValue;
}

BOOLEAN
PciMemoryArbiter_OverrideConflict(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the OverrideConflict entrypoint of the PCI memory arbiter.
    This is called when a conflict between a possible solution for
    a requirement and a solution for another requirement has been found.
    It determines whether the conflict is real or should be allowed
    to stand.  It allows conflicts between fixed requirements from the same
    device.  Conflicts between ROMs and NULL and between boot requirements
    are built into the arbiter, so don't need to be handled here.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if the conflict is allowable, FALSE otherwise

--*/
{

    RTL_RANGE_LIST_ITERATOR iterator;
    PRTL_RANGE current;
    BOOLEAN ok;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, "\nPciMemoryArbiter_OverrideConflict for arbiter %p\n", Arbiter);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    if (!(State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED)) {
        return FALSE;
    }

    ok = FALSE;
    FOR_ALL_RANGES(Arbiter->PossibleAllocation, &iterator, current) {

        //
        // Only test the overlapping ones
        //
        if (INTERSECT(current->Start, current->End, 
                      State->CurrentMinimum, State->CurrentMaximum)) {


            //
            // Check if we should ignore the range because of its attributes
            //
            if (current->Attributes & State->RangeAvailableAttributes) {

                //
                // We DON'T set ok to true because we are just ignoring the range,
                // as RtlFindRange would have and thus it can't be the cause of
                // RtlFindRange failing, so ignoring it can't fix the conflict.
                //
                continue;
            }

            //
            // Check if we are conflicting with ourselves AND the conflicting range
            // is a fixed requirement
            //
            if ((current->Owner == State->Entry->PhysicalDeviceObject) && 
                (current->Attributes & ARBITER_RANGE_FIXED_REQUIREMENT)) {

                State->Start=State->CurrentMinimum;
                State->End=State->CurrentMaximum;

                ok = TRUE;
                
                TRACE(ARBITER_TRACE_LEVEL, "Possible allowed conflict with range:\n");
                TRACE(ARBITER_TRACE_LEVEL, "%I64x-%I64x Flags-0x%x O=%p U=%p\n",
                      current->Start, current->End, 
                      current->Flags, current->Owner, 
                      current->UserData
                      );
                continue;
            }

            //
            // The conflict is still valid.  Check if we are conflicting with the 
            // MMCFG range.
            //

            if (ArbIsConflictWithMmConfigRange(current->Start, current->End)) {

                //
                // Mark the arbiter with the appropriate flag such
                // that Pnp can mark the devnode as conflicting
                // with the mmcfg range rather than not having
                // sufficient resources.
                // 

                State->Flags |= ARBITER_STATE_FLAG_MCFG_CONFLICT;
            }
            
            return FALSE;
        }
    }

    if (ok) {
        TRACE(ARBITER_TRACE_LEVEL, "Conflict allowed\n");
    }
    return ok;
}
