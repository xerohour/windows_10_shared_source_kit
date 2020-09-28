/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    depend.c

Abstract:

    This module contains code to support dependency relations between devices.

Author:

    Paresh Maisuria (pareshm) 2006-11-01
    Minsang Kim (mikim) 2013-02-20

Environment:

    Kernel mode

--*/

#include "pnpmgrp.h"

#define PNP_DEPENDENCY_TYPE_ALL \
    (PNP_DEPENDENCY_TYPE_INITIALIZATION | PNP_DEPENDENCY_TYPE_START)

//
// Private functions
//

_Requires_lock_not_held_(PiDependencyRelationsLock)
VOID
PipProcessRebuildPowerRelationsQueue (
    VOID
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipAddDependentsToRebuildPowerRelationsQueue (
    __in PDEVICE_OBJECT Provider
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipAddtoRebuildPowerRelationsQueue (
    __in PDEVICE_OBJECT Device
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipDependencyCopyEdge (
    __in PPNP_DEPENDENCY_EDGE Edge,
    __in PPNP_DEPENDENCY_NODE NewProvider
);

_Requires_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckForUnsatisfiedDependencies (
    __in PDEVICE_NODE DeviceNode,
    __in ULONG DependencyTypes
);

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PipAttemptDependentsStart (
    __in PDEVICE_NODE ProviderDevice
);

_Requires_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipAttemptDependentStart (
    __in PDEVICE_OBJECT DependentDevice
);

_Requires_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckIfAllProvidersHaveDevnodes (
    __in PPNP_DEPENDENCY_NODE DependentNode
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
PPNP_DEPENDENCY_NODE
PipCreateDependencyNode (
    __in PPNP_BINDING_RESOLUTION Resolution
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipDeleteBindingId (
    __in PPNP_DEPENDENCY_NODE Node,
    __in PCUNICODE_STRING BindingId
);

VOID
PipFreeBindingId (
    __in PPNP_BINDING_ID BindingId
);

VOID
PipFreeBindingRequestEntry (
    __in PPNP_BINDING_REQUEST_ENTRY Entry
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteBindingIds (
    __in PPNP_DEPENDENCY_NODE Node
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteDependencyEdge (
    __in PPNP_DEPENDENCY_EDGE Edge
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipAddDependencyEdgeBetweenNodes (
    __in PPNP_DEPENDENCY_NODE DependentNode,
    __in PPNP_DEPENDENCY_NODE ProviderNode,
    __in PPNP_BINDING_REQUEST Request
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckValidNewDependencyEdge (
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipDetectCyclicalDependency (
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipDetectCyclicalDependencyVisit (
    PPNP_DEPENDENCY_NODE CurrentNode,
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
PPNP_DEPENDENCY_EDGE
PipCreateNewDependencyEdge (
    __in PPNP_DEPENDENCY_NODE Dependent,
    __in PPNP_DEPENDENCY_NODE Provider,
    __in PPNP_BINDING_REQUEST Request
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
PPNP_BINDING_REQUEST_ENTRY
PipAddRequestToEdge (
    __inout PPNP_DEPENDENCY_EDGE Edge,
    __in PPNP_BINDING_REQUEST Request
);

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
VOID
PipMergeDependencyTypes(
    __in PPNP_DEPENDENCY_EDGE TargetEdge,
    __in ULONG DependencyTypes
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyNodes (
    __in PPNP_DEPENDENCY_NODE Target,
    __in PPNP_DEPENDENCY_NODE Source
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyEdgeList (
    __in PPNP_DEPENDENCY_NODE Target,
    __in PPNP_DEPENDENCY_NODE Source,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyEdges (
    __in PPNP_DEPENDENCY_EDGE Target,
    __in PPNP_DEPENDENCY_EDGE Source
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
VOID
PipFreeDependencyEdge (
    __in PPNP_DEPENDENCY_EDGE Edge,
    __in_opt PPNP_DEPENDENCY_EDGE MergeTarget
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipSetDependency (	
    __in PPNP_BINDING_RESOLUTION Dependent,
    __in PPNP_BINDING_REQUEST Request
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipAddBindingId (
    __in PPNP_DEPENDENCY_NODE DependencyNode,
    __in PCUNICODE_STRING SourceString
);

_Requires_lock_held_(PiDependencyRelationsLock)
PPNP_DEPENDENCY_NODE
PipQueryBindingResolution (
    __in PPNP_BINDING_RESOLUTION Query
);

_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
VOID
PipFindDependencyNodePath (
    __in PPNP_DEPENDENCY_NODE DependentNode,
    __in PPNP_DEPENDENCY_NODE ProviderNode,
    __out PPNP_DEPENDENCY_PATH_TYPE PathType,
    __out PULONG DependencyTypes
);

_Requires_lock_held_(PiDependencyEdgeWriteLock)
BOOLEAN
PipDependencyGraphDepthFirstSearch (
    __in PPNP_DEPENDENCY_NODE StartNode,
    __in PPNP_DEPENDENCY_NODE GoalNode,
    __out PULONG PathLength,
    __out PULONG DependencyTypes
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDependencyNodeClearVisitedFlag (
    VOID
);

VOID
PipMoveListEntries (
    __inout PLIST_ENTRY Target,
    __inout PLIST_ENTRY Source
);

BOOLEAN
PipIsDeviceReadyForPowerRelations (
    __in PDEVICE_OBJECT DeviceObject
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipReferenceDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDereferenceDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipLinkDeviceObjectAndDependencyNode (
    __in PDEVICE_OBJECT DeviceObject,
    __in PPNP_DEPENDENCY_NODE DependencyNode
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipUnlinkDeviceObjectAndDependencyNode (
    __in PDEVICE_OBJECT DeviceObject,
    __in PPNP_DEPENDENCY_NODE DependencyNode
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipConvertResolutionsToReservations (
    __in PPNP_DEPENDENCY_NODE ProviderNode
);

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteAllDependencyRelations (
    __in PDEVICE_OBJECT DeviceObject
);

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PipNotifyDependenciesChanged(
    __in_opt PPNP_DEPENDENCY_NODE Dependent,
    __in_opt PPNP_DEPENDENCY_NODE Provider
    );

_Requires_lock_not_held_(PiDependencyRelationsLock)
VOID
PipNotifyDeviceDependencyList(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
    );

BOOLEAN
PipIsProviderStarted (
    __in PDEVICE_NODE ProviderNode
    );

#pragma alloc_text(INIT, PiDeviceDependencyInit)
#pragma alloc_text(PAGE, PipProcessRebuildPowerRelationsQueue)
#pragma alloc_text(PAGE, PipAddDependentsToRebuildPowerRelationsQueue)
#pragma alloc_text(PAGE, PipAddtoRebuildPowerRelationsQueue)
#pragma alloc_text(PAGE, IoClearDependency)
#pragma alloc_text(PAGE, IoDeleteAllDependencyRelations)
#pragma alloc_text(PAGE, IoDuplicateDependency)
#pragma alloc_text(PAGE, PipDependencyCopyEdge)
#pragma alloc_text(PAGE, IoSetDependency)
#pragma alloc_text(PAGE, IoReserveDependency)
#pragma alloc_text(PAGE, PipCheckForUnsatisfiedDependencies)
#pragma alloc_text(PAGE, PnpAcquireDependencyRelationsLock)
#pragma alloc_text(PAGE, PipAttemptDependentsStart)
#pragma alloc_text(PAGE, PipAttemptDependentStart)
#pragma alloc_text(PAGE, PnpCheckForActiveDependencies)
#pragma alloc_text(PAGE, PipSetDependency)
#pragma alloc_text(PAGE, PipCheckValidNewDependencyEdge)
#pragma alloc_text(PAGE, PipDetectCyclicalDependency)
#pragma alloc_text(PAGE, PipDetectCyclicalDependencyVisit)
#pragma alloc_text(PAGE, PnpReleaseDependencyRelationsLock)
#pragma alloc_text(PAGE, PnpNewDeviceNodeDependencyCheck)
#pragma alloc_text(PAGE, PnpStartedDeviceNodeDependencyCheck)
#pragma alloc_text(PAGE, PnpSurpriseRemovedDeviceNodeDependencyCheck)
#pragma alloc_text(PAGE, PipCheckIfAllProvidersHaveDevnodes)
#pragma alloc_text(PAGE, PiAssertDependencyRelationsLockHeld)
#pragma alloc_text(PAGE, PipCreateDependencyNode)
#pragma alloc_text(PAGE, PipAddBindingId)
#pragma alloc_text(PAGE, PipDeleteDependencyNode)
#pragma alloc_text(PAGE, PipDeleteBindingId)
#pragma alloc_text(PAGE, PipFreeBindingId)
#pragma alloc_text(PAGE, PipFreeBindingRequestEntry)
#pragma alloc_text(PAGE, PipDeleteBindingIds)
#pragma alloc_text(PAGE, PipDeleteDependencyEdge)
#pragma alloc_text(PAGE, PipAddRequestToEdge)
#pragma alloc_text(PAGE, PipMergeDependencyNodes)
#pragma alloc_text(PAGE, PipMergeDependencyEdgeList)
#pragma alloc_text(PAGE, PipMergeDependencyEdges)
#pragma alloc_text(PAGE, PipQueryBindingResolution)
#pragma alloc_text(PAGE, PiGetDependentList)
#pragma alloc_text(PAGE, PiEnumerateDependentListEntry)
#pragma alloc_text(PAGE, PiGetProviderList)
#pragma alloc_text(PAGE, PiEnumerateProviderListEntry)
#pragma alloc_text(PAGE, PipDependencyNodeClearVisitedFlag)
#pragma alloc_text(PAGE, PipIsDeviceReadyForPowerRelations)
#pragma alloc_text(PAGE, PipReferenceDependencyNode)
#pragma alloc_text(PAGE, PipDereferenceDependencyNode)
#pragma alloc_text(PAGE, PipLinkDeviceObjectAndDependencyNode)
#pragma alloc_text(PAGE, PipUnlinkDeviceObjectAndDependencyNode)
#pragma alloc_text(PAGE, PipConvertResolutionsToReservations)
#pragma alloc_text(PAGE, PipDeleteAllDependencyRelations)
#pragma alloc_text(PAGE, PnpGetDeviceDependencyList)
#pragma alloc_text(PAGE, PipNotifyDependenciesChanged)
#pragma alloc_text(PAGE, PipNotifyDeviceDependencyList)
#pragma alloc_text(PAGE, PipIsProviderStarted)

_Guarded_by_(PiDependencyRelationsLock) LIST_ENTRY PiDependencyNodeListHead;
_Guarded_by_(PiDependencyRelationsLock) LIST_ENTRY PiRebuildPowerRelationsQueue;
LIST_ENTRY PiDependencyNodeEmptyList;

NTSTATUS
PiDeviceDependencyInit(
    VOID
    )

/*++

Routine Description:

    This routine initializes the state necessary to support tracking device
    dependencies.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS

--*/

{

    ExInitializeResourceLite(&PiDependencyRelationsLock);
    KeInitializeSpinLock(&PiDependencyEdgeWriteLock);
    PnpAcquireDependencyRelationsLock(TRUE);
    InitializeListHead(&PiDependencyNodeListHead);
    InitializeListHead(&PiDependencyNodeEmptyList);
    InitializeListHead(&PiRebuildPowerRelationsQueue);
    PnpReleaseDependencyRelationsLock();
    return STATUS_SUCCESS;
}

_Requires_lock_not_held_(PiDependencyRelationsLock)
VOID
PipProcessRebuildPowerRelationsQueue (
    VOID
)

/*++
    
Routine Description:
    
    Process the queue of dependency nodes that require power relations to be
    rebuilt. All nodes pending in the queue will try to rebuild their
    power relations; if not successful, the node will stay in the queue until
    it is successful.
    
Arguments:
    
    None.

Return Value:
    
    None.
    
--*/

{

    PPNP_DEPENDENCY_NODE CurrentNode;
    PDEVICE_OBJECT CurrentDevice;
    PLIST_ENTRY ListEntry;

    PAGED_CODE();
    
    PnpAcquireDependencyRelationsLock(TRUE);
    ListEntry = PiRebuildPowerRelationsQueue.Flink;
    while (ListEntry != &PiRebuildPowerRelationsQueue) {
        CurrentNode = CONTAINING_RECORD(ListEntry,
                                        PNP_DEPENDENCY_NODE,
                                        QueueListEntry);

        ListEntry = ListEntry->Flink;
        CurrentDevice = CurrentNode->DeviceObject;
        if (CurrentDevice == NULL) {
            RemoveEntryList(&CurrentNode->QueueListEntry);
            InitializeListHead(&CurrentNode->QueueListEntry);
            PipDereferenceDependencyNode(CurrentNode);
            CurrentNode = NULL;
            break;
        }

        if (PipIsDeviceReadyForPowerRelations(CurrentDevice) == FALSE ||
            PipCheckIfAllProvidersHaveDevnodes(CurrentNode) == FALSE) {

            //
            // Nothing can be done for this entry until the system initializes
            // device nodes for the dependent and all of its providers.
            // Leave this entry in the queue and process it later.
            //

            continue;
        }

        //
        // Remove the entry from queue.
        //

        RemoveEntryList(&CurrentNode->QueueListEntry);
        InitializeListHead(&CurrentNode->QueueListEntry);
        PipDereferenceDependencyNode(CurrentNode);
        CurrentNode = NULL;

        //
        // Let go of the dependency engine lock, perform the operation, and
        // re-acquire the lock.
        //

        PnpReleaseDependencyRelationsLock();
        PiQueryPowerRelations(PP_DO_TO_DN(CurrentDevice), TRUE);

        //
        // Remove the reference that was put when this device was queued.
        //

        ObDereferenceObject(CurrentDevice);
        PnpAcquireDependencyRelationsLock(TRUE);

        //
        // Start the iteration from the beginning since we let go of the lock.
        // The queue might have changed.
        //

        ListEntry = PiRebuildPowerRelationsQueue.Flink;
    }

    PnpReleaseDependencyRelationsLock();
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipAddDependentsToRebuildPowerRelationsQueue (
    __in PDEVICE_OBJECT Provider
)

/*++

Routine Description:

    Queue all dependents of the supplied provider node to rebuild
    power relations.
    
Arguments:

    ProviderNode - Supplies the node in which its dependents will
        be queued for power relations rebuilding.

Return Value:

    None.

--*/

{

    PLIST_ENTRY ListHead;
    PLIST_ENTRY ListEntry;
    PDEVICE_OBJECT CurrentDevice;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    ListHead = PiGetDependentList(Provider);
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        PiEnumerateDependentListEntry(ListEntry, &CurrentDevice, NULL);
        ListEntry = ListEntry->Flink;
        if (CurrentDevice != NULL) {
            PipAddtoRebuildPowerRelationsQueue(CurrentDevice);
        }
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipAddtoRebuildPowerRelationsQueue (
    __in PDEVICE_OBJECT Device
)

/*++

Routine Description:

    Queue an entry to rebuild power relations at a later time.
    Call PipProcessRebuildPowerRelationsQueue to process the queue.

    Call this function when a dependent node has a provider edge being
    added or removed.

    If the entry is already in the queue, it is removed and put at the end.
    
Arguments:

    DependentNode - The node that will be added to the queue.

Return Value:

    None.

--*/

{

    PPNP_DEPENDENCY_NODE Node;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    if (PipIsDeviceReadyForPowerRelations(Device) == FALSE) {

        //
        // There is nothing to do here. This will need to be queued again
        // when the device node is initialized.
        //

        return;
    }

    Node = PNP_DO_TO_DEPENDENCY_NODE(Device);
    if (Node == NULL || Node->DeviceObject == NULL) {

        //
        // This device does not have any dependencies, or the device does not
        // yet exist.
        //

        return;
    }
    
    if (IsListEmpty(&Node->QueueListEntry) != FALSE) {

        //
        // This node is not in the queue yet.
        // Put a reference on it and insert into queue.
        //

        ObReferenceObject(Node->DeviceObject);
        PipReferenceDependencyNode(Node);
        InsertTailList(&PiRebuildPowerRelationsQueue, &Node->QueueListEntry);
    }
}

NTSTATUS
IoClearDependency (
    __in PDEVICE_OBJECT DependentPdo,
    __in PDEVICE_OBJECT ProviderPdo
    )

/*++

Routine Description:

    This routine is used to delete a previously set dependency
    between two devices. It can also be used to delete or satisfy all
    dependencies set on a provider in a single operation.

Arguments:

    DependentPdo - Pdo for the dependent device

    ProviderPdo - Pdo for the provider device

Return Value:

    NTSTATUS - STATUS_SUCCESS if the dependency was deleted.  Otherwise
        STATUS_INVALID_PARAMETER if any of the parameters are invalid or the
        dependency doesn't exist.

--*/

{
    
    BOOLEAN Cleared;
    PPNP_DEPENDENCY_EDGE Edge;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(DependentPdo != NULL);
    PNP_ASSERT(ProviderPdo != NULL);
    if (DependentPdo == NULL || ProviderPdo == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }
    
    Cleared = FALSE;
    Status = STATUS_INVALID_PARAMETER;
    PnpAcquireDependencyRelationsLock(TRUE);

    //
    // Find the edge from Dependent to Provider, and delete it.
    //
    
    ListHead = PiGetProviderList(DependentPdo);
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        Edge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
        ListEntry = ListEntry->Flink;
        if (Edge->Provider->DeviceObject == ProviderPdo) {
            Cleared = TRUE;
            PipAddtoRebuildPowerRelationsQueue(DependentPdo);
            PipDeleteDependencyEdge(Edge);
            Edge = NULL;
            Status = STATUS_SUCCESS;
            break;
        }
    }

    PnpReleaseDependencyRelationsLock();
    if (Cleared != FALSE) {
        PipProcessRebuildPowerRelationsQueue();
    }

Exit:

    return Status;
}

NTSTATUS
IoDeleteAllDependencyRelations (
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This function should be called when the PDO is being deleted.
    Deletes all dependency relations involving this device.

Arguments:

    DeviceObject - the device object on which all dependency relations
        will be deleted.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;
    PPNP_DEPENDENCY_NODE DependencyNode;

    PAGED_CODE();

    if (DeviceObject == NULL) {
        PNP_ASSERT(FALSE);
        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    PnpAcquireDependencyRelationsLock(TRUE);
    DependencyNode = PNP_DO_TO_DEPENDENCY_NODE(DeviceObject);
    if (DependencyNode != NULL) {

        //
        // First, queue power relations update for this device's dependents.
        //

        PipAddDependentsToRebuildPowerRelationsQueue(DeviceObject);

        //
        // Remove all dependency information from this device.
        //

        PipDeleteAllDependencyRelations(DeviceObject);
    }

    PnpReleaseDependencyRelationsLock();
    if (DependencyNode != NULL) {
        PipProcessRebuildPowerRelationsQueue();
    }

    Status = STATUS_SUCCESS;

Exit:

    return Status;
}

NTSTATUS
IoDuplicateDependency (
    __in PDEVICE_OBJECT ProviderPdo,
    __in PDEVICE_OBJECT NewProviderPdo
    )

/*++

Routine Description:

    This routine is used to duplicate dependencies on the provider device to a
    new provider device in a single operation.

Arguments:

    ProviderPdo - Pdo for the provider device

    NewProviderPdo - Pdo for the new provider device if any.

Return Value:

    STATUS_SUCCESS if the dependency was duplicated correctly, otherwise an
    appropriate error status is returned.  Possible error cases are due to
    invalid parameters or combination thereof or if the new dependency ends up
    setting up a cyclical dependency.

--*/

{

    PNP_BINDING_RESOLUTION bindingResolution;
    PLIST_ENTRY currentDependentLink;
    PPNP_DEPENDENCY_EDGE dependentEdge;
    PLIST_ENTRY dependentList;
    NTSTATUS finalStatus;
    PPNP_DEPENDENCY_NODE newProviderNode;
    PPNP_DEPENDENCY_NODE oldProviderNode;
    NTSTATUS status;

    PAGED_CODE();

    if (ProviderPdo == NULL || NewProviderPdo == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    finalStatus = STATUS_SUCCESS;
    status = STATUS_SUCCESS;
    newProviderNode = NULL;
    PnpAcquireDependencyRelationsLock(TRUE);

    //
    // All dependencies on the current provider Pdo will be copied to the new
    // provider PDO
    //

    IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                 "IoDuplicateDependency: Moving all dependencies from %#p to %#p\n",
                 ProviderPdo,
                 NewProviderPdo));

    oldProviderNode = PNP_DO_TO_DEPENDENCY_NODE(ProviderPdo);
    if (oldProviderNode == NULL) {

        //
        // Old provider has no dependencies; nothing to do here.
        //

        PnpReleaseDependencyRelationsLock();
        goto Exit;
    }

    //
    // Find a node for the new provider; if it doesn't exist, create a new one.
    //

    bindingResolution.Type = ResolvedWithPdo;
    bindingResolution.u.Pdo = NewProviderPdo;
    newProviderNode = PipQueryBindingResolution(&bindingResolution);
    if (newProviderNode == NULL) {
        newProviderNode = PipCreateDependencyNode(&bindingResolution);
        if (newProviderNode == NULL) {
            finalStatus = STATUS_INSUFFICIENT_RESOURCES;
            PnpReleaseDependencyRelationsLock();
            goto Exit;
        }

    } else {
        PipReferenceDependencyNode(newProviderNode);
    }

    //
    // Loop through the list of dependents for old provider, and copy
    // each edge to the new node.
    //

    dependentList = PiGetDependentList(ProviderPdo);
    currentDependentLink = dependentList->Flink;
    while (currentDependentLink != dependentList) {
        dependentEdge = PiListEntryToDependencyEdge(currentDependentLink, DependentList);
        currentDependentLink = currentDependentLink->Flink;
        PNP_ASSERT(dependentEdge->Provider->DeviceObject == ProviderPdo);

        //
        // Establish a dependency between the dependent and the new provider,
        // with the same properties as the source edge.
        //

        status = PipDependencyCopyEdge(dependentEdge, newProviderNode);
        if (!NT_SUCCESS(status)) {
            IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                         "IoDuplicateDependency failed during call to "
                         "PipDependencyCopyEdge with status = %lx,"
                         " DependentNode = %#p, ProviderPdo = %#p\n",
                         status,
                         dependentEdge->Dependent,
                         NewProviderPdo));

            finalStatus = status;
            break;
        }
    }

    PipDereferenceDependencyNode(newProviderNode);

    //
    // Rebuild power relations for all dependents of the new provider, which
    // is the same as previous dependents of the old provider.
    //

    PipAddDependentsToRebuildPowerRelationsQueue(NewProviderPdo);
    PnpReleaseDependencyRelationsLock();
    PipProcessRebuildPowerRelationsQueue();

Exit:

    return finalStatus;
}

_Use_decl_annotations_
NTSTATUS
PipDependencyCopyEdge (
    PPNP_DEPENDENCY_EDGE Edge,
    PPNP_DEPENDENCY_NODE NewProvider
)

/*++

Routine Description:

    Copy an edge, duplicating all information on the edge except with a new
    provider node.

Arguments:

    Edge - Supplies a pointer to the dependency edge to duplicate.

    Provider - Supplies a pointer to the new provider node in which the
        duplicated edge will use as the provider.

Return Value:

    NT Status code.

--*/

{

    PPNP_BINDING_REQUEST_ENTRY Entry;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    NTSTATUS Status;
    
    PAGED_CODE();

    PNP_ASSERT(Edge != NULL);

    PNP_ASSERT(NewProvider != NULL);

    Status = STATUS_SUCCESS;
    ListHead = &Edge->Requests;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        Entry = CONTAINING_RECORD(ListEntry, PNP_BINDING_REQUEST_ENTRY, ListEntry);
        ListEntry = ListEntry->Flink;
        Status = PipAddDependencyEdgeBetweenNodes(Edge->Dependent,
                                                  NewProvider,
                                                  &Entry->Request);

        if (!NT_SUCCESS(Status)) {
            break;
        }
    }

    return Status;
}

NTSTATUS
IoSetDependency (
    __in PDEVICE_OBJECT DependentPdo,
    __in PDEVICE_OBJECT ProviderPdo,
    __in ULONG DependencyTypes
)

/*++

Routine Description:

    This routine is used to setup a dependency between two devices.  The real
    work is done by PipSetDependency where this routine simply acquires
    the global dependency relation lock.

Arguments:

    DependentPdo - Pdo for the dependent device

    ProviderPdo - Pdo for the provider device

    DependencyTypes - Supplies flags for specifying the type of dependency.
        For example, PNP_DEPENDENCY_TYPE_INITIALIZATION. At least one
        valid bit must be set.

Return Value:

    STATUS_SUCCESS if the dependency was setup correctly, otherwise an
    appropriate error status is returned.  Possible error cases are due to
    invalid parameters or combination thereof or if the new dependency ends up
    setting up a cyclical dependency.

--*/

{

    PNP_BINDING_RESOLUTION Dependent;
    PNP_BINDING_REQUEST Request;
    NTSTATUS Status;

    PAGED_CODE();

    if (DependentPdo == NULL ||
        ProviderPdo == NULL ||
        (DependencyTypes & PNP_DEPENDENCY_TYPE_ALL) == 0 ||
        (DependencyTypes & ~PNP_DEPENDENCY_TYPE_ALL) != 0) {

        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    //
    // Make sure the dependent PDO is not the provider PDO.
    //

    if (DependentPdo == ProviderPdo) {
        IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                     "IoSetDependency Failed: DependentPdo equals "
                     "Provider Pdo, PDO = %#p\n",
                     DependentPdo));

        return STATUS_UNSUCCESSFUL;
    }
    
    Dependent.Type = ResolvedWithPdo;
    Dependent.u.Pdo = DependentPdo;
    Request.Provider.Type = ResolvedWithPdo;
    Request.Provider.u.Pdo = ProviderPdo;
    Request.DependencyTypes = DependencyTypes;
    PnpAcquireDependencyRelationsLock(TRUE);
    Status = PipSetDependency(&Dependent, &Request);
    if (!NT_SUCCESS(Status)) {
        PnpReleaseDependencyRelationsLock();
        goto Exit;
    }

    PnpReleaseDependencyRelationsLock();
    PipProcessRebuildPowerRelationsQueue();

Exit:

    return Status;
}

NTSTATUS
IoReserveDependency (	
    __in PDEVICE_OBJECT DependentPdo,
    __in PCUNICODE_STRING ProviderId,
    __in ULONG DependencyTypes
)

/*++

Routine Description:

    This routine is used to describe a future dependency between a Pdo and an
    unknown provider device. Until the actual provider device is given via
    IoResolveDependency() is called, the DependentPdo will not be able to start.
    
Arguments:

    DependentPdo - Supplies the PDO for the dependent device
    
    ProviderId – Supplies the binding ID of the provider.

    DependencyTypes - Supplies flags for specifying the type of dependency.
        For example, PNP_DEPENDENCY_TYPE_INITIALIZATION. At least one
        valid bit must be set.

Return Value:

    STATUS_SUCCESS if the dependency reservation was setup correctly, otherwise
    an appropriate error status is returned.  Possible error cases are due to
    invalid parameters or combination thereof or if the new dependency ends up
    setting up a cyclical dependency.

--*/

{

    PNP_BINDING_RESOLUTION Dependent;
    PNP_BINDING_REQUEST Request;
    NTSTATUS Status;
    
    PAGED_CODE();

    if (DependentPdo == NULL ||
        ProviderId == NULL ||
        ProviderId->Length == 0 ||
        (DependencyTypes & PNP_DEPENDENCY_TYPE_ALL) == 0 ||
        (DependencyTypes & ~PNP_DEPENDENCY_TYPE_ALL) != 0) {

        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    Dependent.Type = ResolvedWithPdo;
    Dependent.u.Pdo = DependentPdo;
    Request.Provider.Type = ResolvedWithLabel;
    Request.Provider.u.Label = ProviderId;
    Request.DependencyTypes = DependencyTypes;
    PnpAcquireDependencyRelationsLock(TRUE);
    Status = PipSetDependency(&Dependent, &Request);
    PnpReleaseDependencyRelationsLock();

Exit:

    return Status;
}

NTSTATUS
IoResolveDependency (
    __in PCUNICODE_STRING BindingId,
    __in PDEVICE_OBJECT Device
)

/*++

Routine Description:

    This routine is used to resolve a dependency reservation into an actual
    provider device. Any dependents on the reservation will be transferred
    to this device and the reservation will be removed.
    
    If another device claimed the binding ID, the ID will be transferred to
    the new device while maintaining the existing dependencies associted
    with the old device.

Arguments:

    BindingId - String used to reference the reservation. A copy of the string
        is made.

    Device - Pdo for the device.

Return Value:

    NT Status code.

--*/

{

    PPNP_DEPENDENCY_NODE ExistingNode;
    PPNP_DEPENDENCY_NODE NewNode;
    BOOLEAN RebuildPowerRelations;
    NTSTATUS Status;
    PNP_BINDING_RESOLUTION Query;

    PAGED_CODE();

    PNP_ASSERT(BindingId != NULL);
    PNP_ASSERT(BindingId->Length > 0);
    PNP_ASSERT(Device != NULL);

    NewNode = NULL;
    Status = STATUS_SUCCESS;
    RebuildPowerRelations = FALSE;
    PnpAcquireDependencyRelationsLock(TRUE);
    
    //
    // Check if this binding ID is already in use by another node.
    //

    Query.Type = ResolvedWithLabel;
    Query.u.Label = BindingId;
    ExistingNode = PipQueryBindingResolution(&Query);
    if (ExistingNode != NULL && ExistingNode->DeviceObject != NULL) {
        
        //
        // The same device is being used to resolve the binding ID. Nothing to
        // do here.
        //

        if (ExistingNode->DeviceObject == Device) {
            Status = STATUS_SUCCESS;
            goto Exit;

        //
        // If another PDO already claimed this binding ID, remove the ID from it.
        // This can happen if a device is reported missing & new PDO is reported
        // in the same QDR IRP. Note that dependency relations are unchanged;
        // only the ID is removed and becomes available for re-use.
        //

        } else {
            Status = PipDeleteBindingId(ExistingNode, BindingId);

            PNP_ASSERT(NT_SUCCESS(Status));

            ExistingNode = NULL;
        }
    }

    PNP_ASSERT(ExistingNode == NULL || ExistingNode->DeviceObject == NULL);

    //
    // At this point, there are two cases regarding the ID:
    //     1. ID is available.
    //     2. ID is in use by a dependency node.
    //        (but it must not be associated with another PDO)
    // 
    // In addition, there are two cases for the given PDO:
    //     A. PDO does not have a dependency node.
    //     B. PDO already has a dependency node.
    //
    
    //
    // Case 1x.
    //

    if (ExistingNode == NULL) {

        //
        // Case 1A. Create a new dependency node for the device.
        //

        if (PNP_DO_TO_DEPENDENCY_NODE(Device) == NULL) {
            Query.Type = ResolvedWithPdo;
            Query.u.Pdo = Device;
            NewNode = PipCreateDependencyNode(&Query);
            if (NewNode == NULL) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto Exit;
            }
            
            Status = PipAddBindingId(NewNode, BindingId);
            if (!NT_SUCCESS(Status)) {
                PipUnlinkDeviceObjectAndDependencyNode(Device, NewNode);
                PipDereferenceDependencyNode(NewNode);
                goto Exit;
            }

            PipDereferenceDependencyNode(NewNode);

        //
        // Case 1B. Add the binding ID to the device's dependency node.
        //

        } else {
            Status = PipAddBindingId(PNP_DO_TO_DEPENDENCY_NODE(Device), BindingId);
            if (!NT_SUCCESS(Status)) {
                goto Exit;
            }
        }

    //
    // Case 2x.
    //

    } else if (ExistingNode->DeviceObject == NULL) {

        //
        // Case 2A. Resolve an existing dependency node with the given device.
        //

        if (PNP_DO_TO_DEPENDENCY_NODE(Device) == NULL) {
            PipLinkDeviceObjectAndDependencyNode(Device, ExistingNode);

        //
        // Case 2B. Merge the existing node into the device's node.
        //

        } else {
            PipMergeDependencyNodes(PNP_DO_TO_DEPENDENCY_NODE(Device), ExistingNode);

            //
            // Existing node may have been deleted at this point.
            //

            ExistingNode = NULL;
        }
    }

    RebuildPowerRelations = TRUE;

    //
    // If the device already started, check if its depenents can start.
    //

    if (IS_PDO(Device) &&
        (PipIsProviderStarted(PP_DO_TO_DN(Device)) != FALSE)) {

        PipAttemptDependentsStart(PP_DO_TO_DN(Device));
    }

Exit:
    
    if (RebuildPowerRelations != FALSE) {

        //
        // Rebuild power relations for this device and its dependents.
        //

        PipAddtoRebuildPowerRelationsQueue(Device);
        PipAddDependentsToRebuildPowerRelationsQueue(Device);
        PnpReleaseDependencyRelationsLock();
        PipProcessRebuildPowerRelationsQueue();

    } else {
        PnpReleaseDependencyRelationsLock();
    }
    
    return Status;
}

NTSTATUS
IoTestDependency (
    __in PDEVICE_OBJECT Dependent,
    __in PDEVICE_OBJECT Provider,
    __out PPNP_DEPENDENCY_PATH_TYPE PathType,
    __out PULONG DependencyTypes
)

/*++

Routine Description:

    Test for dependency between the two provided device objects, Dependent
    and provider.

Arguments:

    Dependent - Supplies the dependent device.

    Provider - Supplies the provider device.

    PathType - Returns the type of dependency found between the dependent
        and the provider:

         * DependencyPathNone indicates that there is no path from dependent
           to the provider.

         * DependencyPathDirect indicates that the dependent is directly
           dependent on the provider.

         * DependencyPathIndirect indicates that the dependnet is indirectly
           dependent on the provider; that is, there are some intermediate
           dependencies that create a path from dependent to provider.

        This value is undefined if the returned status code is not
        STATUS_SUCCESS.

    DependencyTypes - Returns the dependency of the first edge from StartNode
        to the next node in the found path. For example, if we have:

            [DependentNode] ---Start---> [Foobar] ---Init---> [ProviderNode]

        then this value will be start dependency. This value is undefined if
        the returned status code is not STATUS_SUCCESS.

Return Value:

    NT status code.

--*/

{

    PPNP_DEPENDENCY_NODE DependentNode;
    PPNP_DEPENDENCY_NODE ProviderNode;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    if (Dependent == NULL ||
        Provider == NULL ||
        Dependent == Provider) {

        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }
    
    DependentNode = PNP_DO_TO_DEPENDENCY_NODE(Dependent);
    ProviderNode = PNP_DO_TO_DEPENDENCY_NODE(Provider);
    if (DependentNode == NULL || ProviderNode == NULL) {
        *PathType = DependencyPathNone;
        *DependencyTypes = 0;
        goto Exit;
    }

    //
    // Find a path from Dependent to Provider.
    //

    PipFindDependencyNodePath(DependentNode,
                              ProviderNode,
                              PathType,
                              DependencyTypes);

Exit:
    
    return Status;
}

_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
VOID
PipFindDependencyNodePath (
    __in PPNP_DEPENDENCY_NODE DependentNode,
    __in PPNP_DEPENDENCY_NODE ProviderNode,
    __out PPNP_DEPENDENCY_PATH_TYPE PathType,
    __out PULONG DependencyTypes
)
/*++

Routine Description:

    Finds a path from DependentNode to ProviderNode.

Arguments:

    Dependent - Supplies the dependent node.

    Provider - Supplies the provider node.

    PathType - Returns the type of dependency found between the dependent
        and the provider:

         * DependencyPathNone indicates that there is no path.

         * DependencyPathDirect indicates that the path length is 1.

         * DependencyPathIndirect indicates that the path has length
           greater than 1.

    DependencyTypes - Returns the dependency of the first edge from StartNode
        to the next node in the found path. For example, if we have:

            [DependentNode] ---Start---> [Foobar] ---Init---> [ProviderNode]

        then this value will be start dependency. 

Return Value:

    None.

--*/
{

    KIRQL OldIrql;
    ULONG PathLength;

    PNP_ASSERT(DependentNode != ProviderNode);
    
    KeAcquireSpinLock(&PiDependencyEdgeWriteLock, &OldIrql);
    PipDependencyGraphDepthFirstSearch(DependentNode,
                                       ProviderNode,
                                       &PathLength,
                                       DependencyTypes);
    
    KeReleaseSpinLock(&PiDependencyEdgeWriteLock, OldIrql);

    if (PathLength == 0) {
        *PathType = DependencyPathNone;
        PNP_ASSERT(*DependencyTypes == 0);

    } else if (PathLength == 1) {
        *PathType = DependencyPathDirect;
        PNP_ASSERT(*DependencyTypes != 0);

    } else {
        *PathType = DependencyPathIndirect;
        PNP_ASSERT(*DependencyTypes != 0);
    }
}

_Requires_lock_held_(PiDependencyEdgeWriteLock)
BOOLEAN
PipDependencyGraphDepthFirstSearch (
    __in PPNP_DEPENDENCY_NODE StartNode,
    __in PPNP_DEPENDENCY_NODE GoalNode,
    __out PULONG PathLength,
    __out PULONG DependencyTypes
)

/*++

Routine Description:

    Perform a depth first search on the dependency graph.

Arguments:

    StartNode - Supplies the starting node to start the search (or in
        recursive calls, supplies the current node).

    GoalNode - Supplies the node that the search is looking for;
        when this node is reached, the search is complete.

    PathLength - Returns the length of a path found from the staritng
        node to the goal node, if a path exists. If a path does not
        exist, returns 0.

    DependencyTypes - Returns the dependency of the first edge from 
        StartNode to next node in the found path.

Return Value:

    TRUE if a path is found, FALSE otherwise.

--*/

{

    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PPNP_DEPENDENCY_NODE ProviderNode;
    PLIST_ENTRY ListEntry;

    NT_ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    PNP_ASSERT(StartNode != NULL);
    PNP_ASSERT(GoalNode != NULL);
    PNP_ASSERT(PathLength != NULL);
    PNP_ASSERT(DependencyTypes != NULL);
    
    *PathLength = 0;
    *DependencyTypes = 0;
    if (StartNode == GoalNode) {

        //
        // Reached the destination; start unwinding recursion.
        //

        return TRUE;
    }

    ListEntry = StartNode->Providers.Flink;
    while (ListEntry != &StartNode->Providers) {
        CurrentEdge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
        ListEntry = ListEntry->Flink;
        PNP_ASSERT(CurrentEdge->Dependent == StartNode);
        ProviderNode = CurrentEdge->Provider;
        if (PipDependencyGraphDepthFirstSearch(ProviderNode,
                                               GoalNode,
                                               PathLength,
                                               DependencyTypes)) {

            //
            // A path is found, and the recursion is unwinding.
            // While iterating the path backwards, accumulate path length.
            //

            *PathLength += 1;

            //
            // Remember the dependency types of the last edge.
            //

            *DependencyTypes = CurrentEdge->DependencyTypes;
            return TRUE;
        }
    }

    //
    // This node is a dead end.
    //

    return FALSE;
}

_Requires_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckForUnsatisfiedDependencies (
    __in PDEVICE_NODE DeviceNode,
    __in ULONG DependencyTypes
    )

/*++

Routine Description:

    This routine checks if the given device node's PDO has any dependencies its
    waiting on.  This routine assumes that the caller has taken the global
    dependency relations lock.

Arguments:

    DeviceNode - The device node whose PDO is being checked for oustanding
        dependencies.

    DependencyTypes - Supplies the type of dependencies to check.

Return Value:

    TRUE if the device node is still waiting on dependencies.

--*/

{

    ULONG CurrentDependencyTypes;
    PLIST_ENTRY CurrentProviderLink;
    PDEVICE_OBJECT PhysicalDeviceObject;
    PDEVICE_OBJECT Provider;
    PLIST_ENTRY ProviderList;
    PDEVICE_NODE ProviderNode;
    BOOLEAN Result;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(FALSE);
    Result = FALSE;
    PhysicalDeviceObject = DeviceNode->PhysicalDeviceObject;
    ProviderList = PiGetProviderList(PhysicalDeviceObject);
    CurrentProviderLink = ProviderList->Flink;
    while (CurrentProviderLink != ProviderList) {
        PiEnumerateProviderListEntry(CurrentProviderLink,
                                     &Provider,
                                     &CurrentDependencyTypes);

        CurrentProviderLink = CurrentProviderLink->Flink;
        ProviderNode = PP_DO_TO_DN(Provider);
        if (Provider == NULL || ProviderNode == NULL) {

            //
            // Provider device node has not been resolved yet.
            // From PNP's perspective, the provider device does not exist
            // in the system, so this dependency is unsatisfied, regardless
            // of the dependency type.
            //

            Result = TRUE;
            break;
        }

        //
        // Check for init and start dependency.
        // If the provider is not started, then the dependency is unsatisfied.
        //

        if ((CurrentDependencyTypes &
             DependencyTypes &
             PNP_DEPENDENCY_TYPE_START) != 0 ||
            (CurrentDependencyTypes &
             DependencyTypes &
             PNP_DEPENDENCY_TYPE_INITIALIZATION) != 0) {

            Result = (PipIsProviderStarted(ProviderNode) == FALSE);
        }

        if (Result != FALSE) {
            break;
        }
    }

    if (Result) {
        IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                     "PipCheckForUnsatisfiedDependencies: stalling "
                     "enumeration of devnode %wZ due to outstanding "
                     "dependencies.\n",
                     PnpGetDeviceInstancePath(DeviceNode)));

    }

    return Result;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipSetDependency (	
    __in PPNP_BINDING_RESOLUTION Dependent,
    __in PPNP_BINDING_REQUEST Request
)

/*++

Routine Description:

    This routine is used to setup a dependency between two devices.

Arguments:

    Dependent - Supplies the dependent in the relationship. It must be a PDO.

    Request - Supplies a pointer to the request to establish a dependency.

Return Value:

    STATUS_SUCCESS if the dependency was setup correctly, otherwise an
    appropriate error status is returned.  Possible error cases are due to
    invalid parameters or combination thereof or if the new dependency ends up
    setting up a cyclical dependency.

--*/

{

    PPNP_DEPENDENCY_NODE DependentNode;
    PPNP_DEPENDENCY_EDGE Edge;
    PPNP_DEPENDENCY_NODE ProviderNode;
    NTSTATUS Status;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);

    PNP_ASSERT(Dependent != NULL);

    PNP_ASSERT(Request != NULL);

    PNP_ASSERT(Request->DependencyTypes != 0);

    //
    // Dependent must be a PDO.
    //

    PNP_ASSERT(Dependent->Type == ResolvedWithPdo);

    DependentNode = NULL;
    Edge = NULL;
    ProviderNode = NULL;

    //
    // Find an existing dependent node, or create a new one.
    //

    DependentNode = PipQueryBindingResolution(Dependent);
    if (DependentNode == NULL) {
        DependentNode = PipCreateDependencyNode(Dependent);
        if (DependentNode == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

    } else {
        PipReferenceDependencyNode(DependentNode);
    }

    //
    // Find an existing provider node, or create a new one.
    //

    ProviderNode = PipQueryBindingResolution(&Request->Provider);
    if (ProviderNode == NULL) {
        ProviderNode = PipCreateDependencyNode(&Request->Provider);
        if (ProviderNode == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }
    } else {
        PipReferenceDependencyNode(ProviderNode);
    }

    Status = PipAddDependencyEdgeBetweenNodes(DependentNode,
                                              ProviderNode,
                                              Request);

Exit:

    //
    // If a node was just created but the edge was not successfully created,
    // dereferencing here may cause the node to be deleted.
    //

    if (DependentNode != NULL) {
        PipDereferenceDependencyNode(DependentNode);
    }

    if (ProviderNode != NULL) {
        PipDereferenceDependencyNode(ProviderNode);
    }

    return Status;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipAddDependencyEdgeBetweenNodes (
    __in PPNP_DEPENDENCY_NODE DependentNode,
    __in PPNP_DEPENDENCY_NODE ProviderNode,
    __in PPNP_BINDING_REQUEST Request
)

/*++

Routine Description:

    Add a dependency edge between two nodes. Checks are made to see if the
    edge already exists (in which DependencyTypes values are merged in), or
    if a new edge needs to be created, provided that it does not introduce a
    cycle in the graph.

Arguments:

    DependentNode - Supplies the dependent node in the relationship.

    ProviderNode - Supplies the provider node in the relationship.

    Request - Supplies a pointer to how the provider was specified when asked
        to establish this edge.

Return Value:

    NT status code.

--*/

{

    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PPNP_DEPENDENCY_EDGE Edge;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    NTSTATUS Status;

    PiAssertDependencyRelationsLockHeld(TRUE);
    Status = STATUS_SUCCESS;
    Edge = NULL;

    //
    // Check if there already is an identical edge, which would have the same
    // provider and dependent. 
    //
    
    ListHead = &DependentNode->Providers;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        CurrentEdge = CONTAINING_RECORD(ListEntry, PNP_DEPENDENCY_EDGE, DependentListEntry);
        ListEntry = ListEntry->Flink;

        PNP_ASSERT(CurrentEdge->Dependent == DependentNode);
        if (CurrentEdge->Provider == ProviderNode) {

            //
            // An existing edge is found with the same provider and dependent.
            // Merge in the new properties.
            //

            PipAddRequestToEdge(CurrentEdge, Request);
            goto Exit;
        }
    }
    
    PNP_ASSERT(Edge == NULL);

    //
    // Existing edge not found; create an edge between the provider and dependent.
    //

    //
    // Check for invalid scenarios.
    //

    if (PipCheckValidNewDependencyEdge(DependentNode, ProviderNode) == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    //
    // Try adding an edge.
    //

    Edge = PipCreateNewDependencyEdge(DependentNode,
                                      ProviderNode,
                                      Request);
        
    if (Edge == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    //
    // The edge was added successfully.
    // The dependent will need to update its power relations.
    //

    if (Edge->Provider->DeviceObject != NULL) {
        PipAddtoRebuildPowerRelationsQueue(Edge->Dependent->DeviceObject);
    }

    Status = STATUS_SUCCESS;

Exit:
    return Status;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckValidNewDependencyEdge (
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
)

/*++

Routine Description:

    Checks if an edge between two two nodes should be allowed.
    Invalid cases are the following:

        * Dependent and Provider cannot be the same node.
        * Dependent cannot be an ancestor of Provider.
        * Provider cannot be dependent of Dependent.
        * Check if the new edge introduces a cycle (DBG only).
        
Arguments:

    Dependent - Supplies the dependent node in the relationship.

    Provider - Supplies the provider node in the relationship.

Return Value:

    TRUE if an edge is allowed to be created; FALSE otherwise.

--*/

{
    
    PDEVICE_NODE CurrentDevice;
    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PDEVICE_NODE DependentDevice;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    PDEVICE_NODE ProviderDevice;
    
    PAGED_CODE();

    PNP_ASSERT(Dependent != NULL && Provider != NULL);
    PiAssertDependencyRelationsLockHeld(TRUE);

    //
    // Dependent and Provider cannot be the same node.
    //

    if (Dependent == Provider) {
        return FALSE;
    }

    //
    // Dependent cannot be an ancestor of Provider. This would create
    // problems with enumeration and removal.
    //

    DependentDevice = PP_DO_TO_DN(Dependent->DeviceObject);
    ProviderDevice = PP_DO_TO_DN(Provider->DeviceObject);
    if (DependentDevice != NULL && ProviderDevice != NULL) {
        CurrentDevice = ProviderDevice;
        do {
            CurrentDevice = CurrentDevice->Parent;
            if (CurrentDevice == DependentDevice) {
                return FALSE;
            }
        } while (CurrentDevice != IopRootDeviceNode);
    }

    //
    // There must not be an edge from Provider to Dependent.
    //

    ListHead = &Provider->Providers;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        CurrentEdge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
        ListEntry = ListEntry->Flink;
        PNP_ASSERT(CurrentEdge->Dependent == Provider);
        if (CurrentEdge->Provider == Dependent) {
            return FALSE;
        }
    }

#if DBG

    //
    // Check if the new edge introduces a cycles in the graph.
    //

    if (PipDetectCyclicalDependency(Dependent, Provider) != FALSE) {
        return FALSE;
    }

#endif

    return TRUE;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipDetectCyclicalDependency (
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
)

/*++

Routine Description:

    Check if adding an edge from Dependent to Provider will result in a cycle
    in the graph.

Arguments:

    Dependent - supplies the dependent in the new edge.

    Provider - supplies the provider in the new edge.

Return Value:

    TRUE if a cycle is found; FALSE otherwise.

--*/

{

    PLIST_ENTRY ListEntry;
    PPNP_DEPENDENCY_NODE CurrentNode;
    BOOLEAN Cyclic;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    Cyclic = FALSE;
    PipDependencyNodeClearVisitedFlag();
    ListEntry = PiDependencyNodeListHead.Flink;
    while (ListEntry != &PiDependencyNodeListHead) {
        CurrentNode = CONTAINING_RECORD(ListEntry,
                                        PNP_DEPENDENCY_NODE,
                                        ListEntry);

        ListEntry = ListEntry->Flink;
        if ((CurrentNode->Flags & PNP_DEPENDENCY_NODE_VISITED) == 0) {

            //
            // Start a DFS on this unvisited node.
            //

            Cyclic = PipDetectCyclicalDependencyVisit(CurrentNode,
                                                      Dependent,
                                                      Provider);

            if (Cyclic != FALSE) {
                break;
            }
        }
    }

    return Cyclic;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipDetectCyclicalDependencyVisit (
    PPNP_DEPENDENCY_NODE CurrentNode,
    PPNP_DEPENDENCY_NODE Dependent,
    PPNP_DEPENDENCY_NODE Provider
)

/*++

Routine Description:

    Starting with the provided node, recursively visit dependency nodes (using
    depth first search), trying to check if there is a back-edge in the
    spanning tree. If a back-edge is found, this means a cycle exists in the
    subgraph.

Arguments:

    CurrentNode - supplies the current node being enumerated.

    Dependent - supplies the provider node in the new edge being tested.

    Provider - supplies the provider node in the new edge being tested.

Return Value:

    TRUE if a cycle is found; FALSE otherwise.

--*/

{

    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PLIST_ENTRY ListHead;
    PLIST_ENTRY ListEntry;

    PAGED_CODE();

    PNP_ASSERT(CurrentNode != NULL);
    PNP_ASSERT(Dependent != NULL);
    PNP_ASSERT(Provider != NULL);

    PiAssertDependencyRelationsLockHeld(TRUE);
    if ((CurrentNode->Flags & PNP_DEPENDENCY_NODE_PROCESSING) != 0) {

        //
        // A back-edge is detected during a depth-first search.
        // This means a cycle exists in the graph.
        //

        return TRUE;
    }

    if ((CurrentNode->Flags & PNP_DEPENDENCY_NODE_VISITED) == 0) {

        //
        // Temporarily mark this node as Processing so that if we run into 
        // this node again in recursive calls, it means a cycle exists.
        //

        CurrentNode->Flags |= PNP_DEPENDENCY_NODE_PROCESSING;

        //
        // Recursively visit each provider node.
        //

        ListHead = &CurrentNode->Providers;
        ListEntry = ListHead->Flink;
        while (ListEntry != ListHead) {
            CurrentEdge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
            ListEntry = ListEntry->Flink;
            PNP_ASSERT(CurrentEdge->Dependent == CurrentNode);
            if (PipDetectCyclicalDependencyVisit(CurrentEdge->Provider,
                                                 Dependent,
                                                 Provider)) {

                //
                // Cycle was detected; unwind recursion and return TRUE.
                //

                return TRUE;
            }
        }

        //
        // Visit the new edge being tested for cycles.
        //

        if (CurrentNode == Dependent) {
            if (PipDetectCyclicalDependencyVisit(Provider, Dependent, Provider)) {

                //
                // Cycle was detected; unwind recursion and return TRUE.
                //

                return TRUE;
            }
        }


        //
        // This node is done processing; mark it as visited.
        //

        CurrentNode->Flags &= ~PNP_DEPENDENCY_NODE_PROCESSING;
        CurrentNode->Flags |= PNP_DEPENDENCY_NODE_VISITED;
    }

    //
    // No cycle was detected in this subgraph.
    //

    return FALSE;
}

_Acquires_lock_(IopDeviceTreeLock)
_When_(ExclusiveLock != FALSE, _Acquires_exclusive_lock_(PiDependencyRelationsLock))
_When_(ExclusiveLock == FALSE, _Acquires_shared_lock_(PiDependencyRelationsLock))
VOID
PnpAcquireDependencyRelationsLock(
    __in BOOLEAN ExclusiveLock
    )

/*++

Routine Description:

    This function acquires the global dependency relations lock.

Arguments:

    ExclusiveLock - specifies whether an exclusive lock is required,
        rather than a shared lock.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PpDevNodeLockTree(PPL_SIMPLE_READ);

    //
    // No need to disable APC as above call to lock tree already does this.
    //

    if (ExclusiveLock != FALSE) {
        ExAcquireResourceExclusiveLite(&PiDependencyRelationsLock, TRUE);

    } else {
        ExAcquireResourceSharedLite(&PiDependencyRelationsLock, TRUE);
    }
}

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PipAttemptDependentsStart (
    __in PDEVICE_NODE ProviderDevice
)

/*++

Routine Description:

    This routine attempts to start all dependents of the given devnode if
    there are no more dependencies on which the dependent PDO is waiting.
    A device event is queued and the dependent device is started.

Arguments:

    ProviderDevice - Supplies the device node of the provider device in which
        its dependents are attempted to be started.

Return Value:

    None.

--*/

{

    ULONG DependencyTypes;
    PDEVICE_OBJECT DependentPdo;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    PDEVICE_OBJECT ProviderPdo;

    PAGED_CODE();

    PNP_ASSERT(ProviderDevice != NULL);

    PiAssertDependencyRelationsLockHeld(FALSE);

    ProviderPdo = ProviderDevice->PhysicalDeviceObject;
    ListHead = PiGetDependentList(ProviderPdo);
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        PiEnumerateDependentListEntry(ListEntry, &DependentPdo, &DependencyTypes);
        PNP_ASSERT((DependencyTypes &
                    ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                      PNP_DEPENDENCY_TYPE_START)) == 0);

        ListEntry = ListEntry->Flink;
        if (DependentPdo == NULL) {

            //
            // The dependent device does not exist in the system yet.
            // Nothing to do.
            //

            continue;
        }

        PipAttemptDependentStart(DependentPdo);
    }
}

_Use_decl_annotations_
NTSTATUS
PipAttemptDependentStart (
    PDEVICE_OBJECT DependentDevice
)

/*++

Routine Description:

    This routine attempts to a device if there are no more dependencies on
    which the device is waiting on.
    A device event is queued and the dependent device is started.

Arguments:

    DependentDevice - Supplies the device node that will be checked if it can
        restart with dependencies satisfied.

Return Value:

    None.

--*/

{

    PDEVICE_NODE DependentDeviceNode;
    NTSTATUS Status;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(FALSE);

    DependentDeviceNode = PP_DO_TO_DN(DependentDevice);

    //
    // If no device node exists for this PDO then assume that the pnp has
    // yet to allocate one.  In this case there is nothing to be done as
    // the start of this device will only be halted once the device node
    // is allocated and dependencies still exist.
    //

    if (DependentDeviceNode == NULL) {
        IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                     "PipAttemptDependentStart: No device node exists"
                     " for PDO (%#p).\n",
                     DependentDevice));

        Status = STATUS_UNSUCCESSFUL;
        goto PipAttemptDependentStartEnd;
    }

    //
    // Check the problem code to see if this dependent was stalled waiting
    // for dependencies.
    //

    if (PipIsDevNodeProblem(DependentDeviceNode,
                            CM_PROB_WAITING_ON_DEPENDENCY) == FALSE) {

        Status = STATUS_UNSUCCESSFUL;
        goto PipAttemptDependentStartEnd;
    }

    //
    // If there are still dependencies on this device, then
    // there is nothing to be done. Leave the devnode as it is.
    //

    if (PipCheckForUnsatisfiedDependencies(
            DependentDeviceNode,
            (PNP_DEPENDENCY_TYPE_INITIALIZATION | PNP_DEPENDENCY_TYPE_START))) {

        IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                     "PipAttemptDependentStart: Dependencies still exist "
                     " for PDO (%#p) preventing start.\n",
                     DependentDevice));

        Status = STATUS_UNSUCCESSFUL;
        goto PipAttemptDependentStartEnd;
    }

    //
    // Resume enumeration of the dependent device.
    //

    IopDbgPrint((IOP_IOAPI_TRACE_LEVEL,
                 "PipAttemptDependentStart: Attempting to start dependent"
                 " device with PDO = %#p.\n",
                 DependentDevice));

    PipClearDevNodeProblem(DependentDeviceNode);
    Status = PnpRequestDeviceAction(DependentDevice,
                                    StartDevice,
                                    TRUE,
                                    0,
                                    NULL,
                                    NULL);

PipAttemptDependentStartEnd:
    return Status;
}

BOOLEAN
PnpCheckForActiveDependencies (
    __in PDEVICE_NODE DeviceNode,
    __in ULONG DependencyTypes
    )

/*++

Routine Description:

    This routine checks if the given device node's PDO has any dependencies its
    waiting on.  This routine is simply a wrapper around
    PipCheckForUnsatisfiedDependencies that takes the global dependency relations
    lock.

Arguments:

    DeviceNode - The device node whose PDO is being checked for oustanding
        dependencies.

Return Value:

    TRUE if the device node is still waiting on dependencies.

--*/

{

    BOOLEAN result;

    PAGED_CODE();

    //
    // If the device node should be considered started, then no problem
    // code should be assigned to this device node.  This should only
    // happen when there is a race between the creation of a dependency and
    // the transitioning of this device to started state.  If the device node
    // has transitioned to a started state before the dependency could
    // be satisfied, then it should be allowed to continue.  All dependencies
    // and dependents will be cleared after enumeration is complete.
    //

    if (PipIsDevNodeDNStarted(DeviceNode) != FALSE) {
        return FALSE;
    }

    PnpAcquireDependencyRelationsLock(FALSE);
    result = PipCheckForUnsatisfiedDependencies(DeviceNode, DependencyTypes);
    PnpReleaseDependencyRelationsLock();
    return result;
}

_Releases_lock_(PiDependencyRelationsLock)
_Releases_lock_(IopDeviceTreeLock)
VOID
PnpReleaseDependencyRelationsLock (
    VOID
    )

/*++

Routine Description:

    This function releases the global dependency relations list.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    ExReleaseResourceLite(&PiDependencyRelationsLock);
    PpDevNodeUnlockTree(PPL_SIMPLE_READ);
}

VOID
PnpNewDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function determines if the device object is involved in any dependency
    relations and runs any updates (i.e. power relations) that require a device
    node to be successful.

Arguments:

    DeviceObject - Supplies a pointer to the device object that has recently 
                   been assigned a device node.

Return Value:

    None.

--*/

{

    HPNPOPCTX hPnpOpCtx;

    PAGED_CODE();

    hPnpOpCtx = NULL;
    
    PnpAcquireDependencyRelationsLock(TRUE);
    
    //
    // Queue a request to rebuild power relations for this device.
    //

    PipAddtoRebuildPowerRelationsQueue(DeviceNode->PhysicalDeviceObject);
    PnpReleaseDependencyRelationsLock();

    //
    // Check if any pending operations for power relation can be processed
    // due to this device node being initialized.
    //

    PipProcessRebuildPowerRelationsQueue();

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    PipNotifyDeviceDependencyList(DeviceNode, ProviderList);
    PipNotifyDeviceDependencyList(DeviceNode, DependentList);

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }
}

_Requires_lock_held_(PiDependencyRelationsLock)
BOOLEAN
PipCheckIfAllProvidersHaveDevnodes (
    __in PPNP_DEPENDENCY_NODE DependentNode
    )

/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentProviderLink;
    PDEVICE_OBJECT ProviderPdo;
    PLIST_ENTRY ProviderList;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(FALSE);
    PNP_ASSERT(DependentNode != NULL);
    PNP_ASSERT(DependentNode->DeviceObject != NULL);

    //
    // Check if every provider has a device node.
    //

    ProviderList = PiGetProviderList(DependentNode->DeviceObject);
    CurrentProviderLink = ProviderList->Flink;
    while (CurrentProviderLink != ProviderList) {
        PiEnumerateProviderListEntry(CurrentProviderLink,
                                     &ProviderPdo,
                                     NULL);

        if (PipIsDeviceReadyForPowerRelations(ProviderPdo) == FALSE) {

            //
            // At least one provider does not have an initialized device node.
            //

            return FALSE;
        }

        CurrentProviderLink = CurrentProviderLink->Flink;
    }

    return TRUE;
}

VOID
PnpStartedDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    Check if the provided device's dependent can be started.
    This function should be called when the device has completed the
    start routine.

Arguments:

    DeviceNode - Supplies the device node that has just started.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PnpAcquireDependencyRelationsLock(FALSE);
    PipAttemptDependentsStart(DeviceNode);
    PnpReleaseDependencyRelationsLock();
}

NTSTATUS
PnpSurpriseRemovedDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function should be called when the device is surprise removed.

    Deletes all binding IDs from the dependency node to allow another PDO
    to resolve with the same binding IDs. Dependency information is not deleted
    for this device, since device removal still needs this information to
    send the remove IRPs.

Arguments:

    DeviceNode - Supplies the device node that has been surprise removed.

Return Value:

    NT status code.

--*/

{

    PPNP_DEPENDENCY_NODE DependencyNode;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    if ((DeviceNode->Flags & DNF_ENUMERATED) != 0) {
        goto Exit;
    }

    PnpAcquireDependencyRelationsLock(TRUE);
    DependencyNode = PNP_DO_TO_DEPENDENCY_NODE(DeviceNode->PhysicalDeviceObject);
    if (DependencyNode != NULL) {
        Status = PipConvertResolutionsToReservations(DependencyNode);
    }

    PnpReleaseDependencyRelationsLock();
    PipProcessRebuildPowerRelationsQueue();

Exit:
    return Status;
}

_Use_decl_annotations_
VOID
PiAssertDependencyRelationsLockHeld (
    BOOLEAN ExclusiveLock
)

/*++

Routine Description:

    Asserts that dependency relations lock is being held.

Arguments:

    ExclusiveLock - specifies whether an exclusive lock is required,
        rather than a shared lock.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_SIMPLE_READ);
    if (ExclusiveLock != FALSE) {
        PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiDependencyRelationsLock));

    } else {
        PNP_ASSERT(ExIsResourceAcquiredLite(&PiDependencyRelationsLock));
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
PPNP_DEPENDENCY_NODE
PipCreateDependencyNode (
    __in PPNP_BINDING_RESOLUTION Resolution
)

/*++

Routine Description:

    Create a new dependency node. The reference count starts at 1.

Arguments:

    Resolution - Supplies a PDO or binding ID for the node, giving it enough
        information to be identified uniquely in the system.

    DependencyNode - Returns the newly created dependency node.

Return Value:

    NT status code.

--*/

{

    PPNP_DEPENDENCY_NODE NewNode;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(Resolution != NULL);
    PiAssertDependencyRelationsLockHeld(TRUE);
    Status = STATUS_SUCCESS;
    NewNode = (PPNP_DEPENDENCY_NODE) ExAllocatePoolWithTag(
        NonPagedPoolNx,
        sizeof(PNP_DEPENDENCY_NODE),
        PNP_POOLTAG_DEPENDENCY_INFO);

    if (NewNode == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    //
    // Initialize fields.
    //
    
    InitializeListHead(&NewNode->BindingIds);
    InitializeListHead(&NewNode->Dependents);
    InitializeListHead(&NewNode->Providers);
    InitializeListHead(&NewNode->QueueListEntry);
    NewNode->ReferenceCount = 0;
    NewNode->DeviceObject = NULL;
    NewNode->Flags = 0;

    //
    // Insert into master list of nodes.
    //

    InsertTailList(&PiDependencyNodeListHead, &NewNode->ListEntry);

    //
    // Set unique identifiers.
    //

    if (Resolution->Type == ResolvedWithLabel) {

        //
        // Add the label to the new node as a new binding ID.
        //

        Status = PipAddBindingId(NewNode, Resolution->u.Label);
        if (!NT_SUCCESS(Status)) {
            goto Exit;
        }

    } else if (Resolution->Type == ResolvedWithPdo) {

        //
        // Create a link between PDO and dependency node.
        //

        PipLinkDeviceObjectAndDependencyNode(Resolution->u.Pdo, NewNode);

    } else {
        PNP_ASSERT(FALSE);
    }

    //
    // Put a reference on the node before returning it.
    //

    PipReferenceDependencyNode(NewNode);

Exit:
    
    if (!NT_SUCCESS(Status) && NewNode != NULL) {
        PipDeleteDependencyNode(NewNode);
        NewNode = NULL;
    }

    return NewNode;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipAddBindingId (
    __in PPNP_DEPENDENCY_NODE DependencyNode,
    __in PCUNICODE_STRING SourceString
)

/*++

Routine Description:

    Add a binding ID to the given dependency node.

Arguments:

    DependencyNode - Supplies the node in which the ID is being added.

    SourceString - Supplies the new binding ID. A copy of the string is made
        internally.

Return Value:

    NT status code.

--*/

{

    PPNP_BINDING_ID Id;
    NTSTATUS Status;

#if DBG

    PNP_BINDING_RESOLUTION Query;

#endif

    PAGED_CODE();

    PNP_ASSERT(SourceString != NULL);
    PNP_ASSERT(SourceString->Length > 0);
    PiAssertDependencyRelationsLockHeld(TRUE);

#if DBG
    
    //
    // Check for duplicate ID. This should have been prevented before calling
    // this function. For boot performance, this is only done for CHK builds.
    //

    Query.Type = ResolvedWithLabel;
    Query.u.Label = SourceString;
    if (PipQueryBindingResolution(&Query) != NULL) {

        PNP_ASSERT(FALSE);

        Id = NULL;
        Status = STATUS_OBJECT_NAME_COLLISION;
        goto Exit;
    }

#endif

    Id = (PPNP_BINDING_ID) ExAllocatePoolWithTag (
        PagedPool,
        sizeof(PNP_BINDING_ID),
        PNP_POOLTAG_DEPENDENCY_INFO);

    if (Id == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    //
    // Create an exact copy of SourceString, and store it as the label.
    //

    Status = RtlDuplicateUnicodeString(0, SourceString, &Id->Label);
    if (NT_SUCCESS(Status)) {
        InsertTailList(&DependencyNode->BindingIds, &Id->ListEntry);
    }

Exit:
    return Status;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
)

/*++

Routine Description:

    Delete the given dependency node, including of all of its binding IDs.

Arguments:

    Node - Supplies the node to delete.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Node != NULL);
    PiAssertDependencyRelationsLockHeld(TRUE);
    PNP_ASSERT(IsListEmpty(&Node->Dependents));
    PNP_ASSERT(IsListEmpty(&Node->Providers));
    PNP_ASSERT(Node->DeviceObject == NULL);
    PNP_ASSERT(Node->ReferenceCount == 0);
    
    PipDeleteBindingIds(Node);
    RemoveEntryList(&Node->ListEntry);

    //
    // Remove the device from power relations update queue.
    //

    RemoveEntryList(&Node->QueueListEntry);
    ExFreePoolWithTag(Node, PNP_POOLTAG_DEPENDENCY_INFO);
    Node = NULL;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteBindingIds (
    __in PPNP_DEPENDENCY_NODE Node
)

/*++

Routine Description:

    Remove and free all binding IDs on the given dependency node.

Arguments:

    Node - Supplies the node that will have its binding IDs removed.

Return Value:

    None.

--*/

{

    PPNP_BINDING_ID CurrentBindingId;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    ListHead = &Node->BindingIds;
    while (IsListEmpty(ListHead) == FALSE) {
        ListEntry = RemoveTailList(ListHead);
        CurrentBindingId = CONTAINING_RECORD(ListEntry,
                                             PNP_BINDING_ID,
                                             ListEntry);

        PipFreeBindingId(CurrentBindingId);
        CurrentBindingId = NULL;
    }
}

_Use_decl_annotations_
NTSTATUS
PipDeleteBindingId (
    PPNP_DEPENDENCY_NODE Node,
    PCUNICODE_STRING BindingId
)

/*++

Routine Description:

    Remove the provided binding ID from the given dependency node.

Arguments:

    Node - Supplies a pointer to the node that will have the binding ID
        removed.

    BindingId - Supplies a pointer to the binding ID label that will be
        removed.

Return Value:

    NT Status code.

--*/

{

    PPNP_BINDING_ID CurrentBindingId;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_NOT_FOUND;
    PiAssertDependencyRelationsLockHeld(TRUE);
    ListHead = &Node->BindingIds;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        CurrentBindingId = CONTAINING_RECORD(ListEntry,
                                             PNP_BINDING_ID,
                                             ListEntry);

        if (RtlEqualUnicodeString(&CurrentBindingId->Label, BindingId, FALSE)) {
            RemoveEntryList(&CurrentBindingId->ListEntry);
            PipFreeBindingId(CurrentBindingId);
            CurrentBindingId = NULL;
            Status = STATUS_SUCCESS;
            break;
        }

        ListEntry = ListEntry->Flink;
    }

    return Status;
}

_Use_decl_annotations_
VOID
PipFreeBindingId (
    PPNP_BINDING_ID BindingId
)

/*++

Routine Description:

    Free memory used for a binding ID.
    Caller's responsibility is to unlink from any list that the given ID may be
    part of.

Arguments:

    BindingId - Supplies a pointer to the binding ID to free.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(BindingId != NULL);

    RtlFreeUnicodeString(&BindingId->Label);
    ExFreePoolWithTag(BindingId, PNP_POOLTAG_DEPENDENCY_INFO);
    BindingId = NULL;
}

_Use_decl_annotations_
VOID
PipFreeBindingRequestEntry (
    PPNP_BINDING_REQUEST_ENTRY Entry
)

/*++

Routine Description:

    Free memory used for a binding resolution entry.

Arguments:

    Entry - Supplies a pointer to the memory to free.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Entry != NULL);

    if (Entry->Request.Provider.Type == ResolvedWithLabel &&
        Entry->Request.Provider.u.Label != NULL) {

        RtlFreeUnicodeString((PUNICODE_STRING) Entry->Request.Provider.u.Label);
        ExFreePoolWithTag((PVOID) Entry->Request.Provider.u.Label,
                          PNP_POOLTAG_DEPENDENCY_INFO);

        Entry->Request.Provider.u.Label = NULL;
    }
    
    ExFreePoolWithTag(Entry, PNP_POOLTAG_DEPENDENCY_INFO);
    Entry = NULL;
}

_Use_decl_annotations_
VOID
PipDeleteDependencyEdge (
    PPNP_DEPENDENCY_EDGE Edge
)

/*++

Routine Description:

    Unlink and delete a connected dependency edge.
    References on the dependent and provider node are dropped.

Arguments:

    Edge - Supplies the edge to delete.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Edge != NULL);
    PNP_ASSERT(Edge->Provider != NULL);
    PNP_ASSERT(Edge->Dependent != NULL);

    PipFreeDependencyEdge(Edge, NULL);
    Edge = NULL;
}

_Use_decl_annotations_
VOID
PipFreeDependencyEdge (
    PPNP_DEPENDENCY_EDGE Edge,
    PPNP_DEPENDENCY_EDGE MergeTarget
)

/*++

Routine Description:

    Free the memory used by a dependency edge.
    References on the dependent and provider node are dropped.

Arguments:

    Edge - Supplies a pointer to the edge to free.

    MergeTarget - Optionally supplies a pointer to an edge that is the target
        of a merge between two edges.

Return Value:

    None.

--*/

{

    PPNP_BINDING_REQUEST_ENTRY CurrentRequest;
    PLIST_ENTRY ListEntry;
    KIRQL OldIrql;

    //
    // Remove all saved reservation requests.
    //

    while (IsListEmpty(&Edge->Requests) == FALSE) {
        ListEntry = RemoveHeadList(&Edge->Requests);
        CurrentRequest = CONTAINING_RECORD(ListEntry,
                                           PNP_BINDING_REQUEST_ENTRY,
                                           ListEntry);

        PipFreeBindingRequestEntry(CurrentRequest);
        CurrentRequest = NULL;
    }

    KeAcquireSpinLock(&PiDependencyEdgeWriteLock, &OldIrql);
    if (ARGUMENT_PRESENT(MergeTarget)) {

        //
        // Optionally merge the types field into the target edge.
        //

        MergeTarget->DependencyTypes |= Edge->DependencyTypes;
    }

    //
    // Remove the source edge from nodes.
    //

    RemoveEntryList(&Edge->DependentListEntry);
    RemoveEntryList(&Edge->ProviderListEntry);
    KeReleaseSpinLock(&PiDependencyEdgeWriteLock, OldIrql);

    PipNotifyDependenciesChanged(Edge->Dependent, Edge->Provider);

    //
    // Dereference provider and dependent nodes, and free the memory.
    //

    PipDereferenceDependencyNode(Edge->Provider);
    PipDereferenceDependencyNode(Edge->Dependent);
    ExFreePoolWithTag(Edge, PNP_POOLTAG_DEPENDENCY_INFO);
    Edge = NULL;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
PPNP_DEPENDENCY_EDGE
PipCreateNewDependencyEdge (	
    __in PPNP_DEPENDENCY_NODE Dependent,
    __in PPNP_DEPENDENCY_NODE Provider,
    __in PPNP_BINDING_REQUEST Request
)

/*++

Routine Description:

    Create a new dependency edge between from the Dependent to the Provider.
    No error checking is done to see if the edge is valid or if it creates a
    cycle; this is caller's responsibility.

Arguments:

    DependentNode - Supplies the dependent node in the relationship.

    ProviderNode - Supplies the provider node in the relationship.

    Request - Supplies a pointer to the request to add a dependency.

Return Value:

    Pointer to the newly created dependency edge; NULL if memory allocation failed.

--*/

{

    PPNP_DEPENDENCY_EDGE Edge;
    KIRQL OldIrql;
    PPNP_BINDING_REQUEST_ENTRY RequestEntry;

    PNP_ASSERT(Provider != NULL);

    PNP_ASSERT(Dependent != NULL);

    PNP_ASSERT(Dependent != Provider);

    PNP_ASSERT(Request->DependencyTypes != 0);

    PNP_ASSERT((Request->DependencyTypes & ~PNP_DEPENDENCY_TYPE_ALL) == 0);

    Edge = NULL;
    Edge = (PPNP_DEPENDENCY_EDGE) ExAllocatePoolWithTag(
        NonPagedPoolNx,
        sizeof(PNP_DEPENDENCY_EDGE),
        PNP_POOLTAG_DEPENDENCY_INFO);
    
    if (Edge == NULL) {
        goto Exit;
    }

    Edge->Dependent = Dependent;
    Edge->Provider = Provider;
    Edge->DependencyTypes = 0;
    InitializeListHead(&Edge->Requests);
    Edge->DeletePending = FALSE;
    RequestEntry = PipAddRequestToEdge(Edge, Request);
    if (RequestEntry == NULL) {
        ExFreePoolWithTag(Edge, PNP_POOLTAG_DEPENDENCY_INFO);
        Edge = NULL;
        goto Exit;
    }

    //
    // Put a reference on the dependent and the provider
    //

    PipReferenceDependencyNode(Dependent);
    PipReferenceDependencyNode(Provider);
    
    //
    // Hold the spin lock while the edge is being linked to nodes.
    //

    KeAcquireSpinLock(&PiDependencyEdgeWriteLock, &OldIrql);

    //
    // For the provider node, this edge specifies its dependent.
    // Add the edge as a dependent edge.
    //

    InsertTailList(&Provider->Dependents, &Edge->ProviderListEntry);

    //
    // For the dependent node, this edge specifies its provider.
    // Add the edge as a provider edge.
    //
    
    InsertTailList(&Dependent->Providers, &Edge->DependentListEntry);
    KeReleaseSpinLock(&PiDependencyEdgeWriteLock, OldIrql);

    PipNotifyDependenciesChanged(Dependent, Provider);

Exit:

    return Edge;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
PPNP_BINDING_REQUEST_ENTRY
PipAddRequestToEdge (
    __inout PPNP_DEPENDENCY_EDGE Edge,
    __in PPNP_BINDING_REQUEST Request
)

/*++

Routine Description:

    Add a binding request entry to the given edge. A binding request stores the
    original intent that describes why the edge was added. An edge may have
    multiple requests stored, but duplicates are prevented.

Arguments:

    Edge - Supplies a pointer to the edge that will receive a new binding
        request.

    Request - Supplies a pointer to the request.

Return Value:

    A newly created binding request entry; in case of a duplicate (same request
    already exists), an existing entry is returned.

--*/

{
 
    PPNP_BINDING_REQUEST_ENTRY NewEntry;
    PPNP_BINDING_REQUEST_ENTRY CurrentEntry;
    PPNP_BINDING_REQUEST_ENTRY DuplicateEntry;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    NTSTATUS Status;

    PAGED_CODE();

    NewEntry = NULL;
    DuplicateEntry = NULL;

    //
    // Check for duplicates.
    //

    ListHead = &Edge->Requests;
    ListEntry = ListHead->Flink;
    while ((ListEntry != ListHead) && (DuplicateEntry == NULL)) {
        CurrentEntry = CONTAINING_RECORD(ListEntry, PNP_BINDING_REQUEST_ENTRY, ListEntry);
        ListEntry = ListEntry->Flink;
        if (CurrentEntry->Request.Provider.Type != Request->Provider.Type) {
            continue;
        }

        switch (CurrentEntry->Request.Provider.Type) {
        case ResolvedWithPdo:
            if (CurrentEntry->Request.Provider.u.Pdo == Request->Provider.u.Pdo) {
                DuplicateEntry = CurrentEntry;
            }

            break;

        case ResolvedWithLabel:
            if (RtlEqualUnicodeString(CurrentEntry->Request.Provider.u.Label,
                                      Request->Provider.u.Label,
                                      FALSE)) {

                DuplicateEntry = CurrentEntry;
            }

            break;

        default:

            PNP_ASSERT(FALSE);

            break;
        }
    }
    
    //
    // If a duplicate exists, merge in the type field and exit out.
    //

    if (DuplicateEntry != NULL) {
        DuplicateEntry->Request.DependencyTypes |= Request->DependencyTypes;
        goto Exit;
    }

    //
    // No duplicates found; copy data and insert into the list.
    //

    NewEntry = (PPNP_BINDING_REQUEST_ENTRY) ExAllocatePoolWithTag(
                                               PagedPool,
                                               sizeof(*NewEntry),
                                               PNP_POOLTAG_DEPENDENCY_INFO);

    if (NewEntry == NULL) {
        goto Exit;
    }

    NewEntry->Request.DependencyTypes = Request->DependencyTypes;
    NewEntry->Request.Provider.Type = Request->Provider.Type;
    switch (NewEntry->Request.Provider.Type) {
    case ResolvedWithPdo:
        NewEntry->Request.Provider.u.Pdo = Request->Provider.u.Pdo;
        break;

    case ResolvedWithLabel:
        NewEntry->Request.Provider.u.Label = ExAllocatePoolWithTag(
                                                 PagedPool,
                                                 sizeof(UNICODE_STRING),
                                                 PNP_POOLTAG_DEPENDENCY_INFO);

        Status = RtlDuplicateUnicodeString(
                     0,
                     Request->Provider.u.Label,
                     (PUNICODE_STRING) NewEntry->Request.Provider.u.Label);

        if (!NT_SUCCESS(Status)) {
            PipFreeBindingRequestEntry(NewEntry);
            NewEntry = NULL;
            goto Exit;
        }

        break;

    default:

        PNP_ASSERT(FALSE);

        break;
    }

    InsertTailList(&Edge->Requests, &NewEntry->ListEntry);

Exit:

    if (DuplicateEntry != NULL) {

        PNP_ASSERT(NewEntry == NULL);

        NewEntry = DuplicateEntry;
    }

    if (NewEntry != NULL) {
        PipMergeDependencyTypes(Edge, Request->DependencyTypes);
    }

    return NewEntry;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
_Requires_lock_not_held_(PiDependencyEdgeWriteLock)
VOID
PipMergeDependencyTypes (
    __in PPNP_DEPENDENCY_EDGE TargetEdge,
    __in ULONG DependencyTypes
)

/*++

Routine Description:

    Merge in dependency types into the given edge.

Arguments:

    TargetEdge - Supplies a pointer to the edge that will receive the new
        dependency types.

    DependencyTypes - Supplies the dependency types to be merged.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    NT_ASSERT(TargetEdge != NULL);

    NT_ASSERT(DependencyTypes != 0);

    KeAcquireSpinLock(&PiDependencyEdgeWriteLock, &OldIrql);
    TargetEdge->DependencyTypes |= DependencyTypes;
    KeReleaseSpinLock(&PiDependencyEdgeWriteLock, OldIrql);
    return;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyNodes (
    __in PPNP_DEPENDENCY_NODE Target,
    __in PPNP_DEPENDENCY_NODE Source
)

/*++

Routine Description:

    Merge two dependency nodes. Binding IDs, provider edges, and dependent
    edges are merged into the target node from the source node.

Arguments:

    Target - Supplies the node that will receive merged data.

    Source - Supplies the node that will provide node data. All of its
        binding IDs and edges will be deleted.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Target != NULL);
    PNP_ASSERT(Source != NULL);
    PNP_ASSERT(Target->DeviceObject != Source->DeviceObject);
    PiAssertDependencyRelationsLockHeld(TRUE);

    //
    // Put a reference on the nodes while work is being performed.
    //

    PipReferenceDependencyNode(Target);
    PipReferenceDependencyNode(Source);

    //
    // Take a union of the flags.
    //

    Target->Flags |= Source->Flags;

    //
    // Move all Binding IDs from the source to target.
    // There should be no overlap between binding IDs since global uniqueness
    // is enforced when adding new IDs to a node.
    //

    PipMoveListEntries(&Target->BindingIds, &Source->BindingIds);

    //
    // Merge the list of providers and list of dependents.
    //

    PipMergeDependencyEdgeList(Target, Source, ProviderList);
    PipMergeDependencyEdgeList(Target, Source, DependentList);

    //
    // At the end, the source node should not have any edges.
    //

    PNP_ASSERT(IsListEmpty(&Source->Providers) &&
               IsListEmpty(&Source->Dependents));

    //
    // Remove the references put on the nodes.
    //

    PipDereferenceDependencyNode(Target);
    PipDereferenceDependencyNode(Source);
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyEdgeList (
    __in PPNP_DEPENDENCY_NODE Target,
    __in PPNP_DEPENDENCY_NODE Source,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
)

/*++

Routine Description:

    Merge two lists of dependent edges, or two lists of provider edges.
    All of list entires in the source list will be moved to the target list.

Arguments:

    Target - Supplies the node that supplies the target list.

    Source - Supplies the node that supplies the source list.

    Mode - If ProviderList, the nodes' Providers list will be merged together.
        If DependentList, the nodes' Dependents list will be merged together.

Return Value:

    None.

--*/

{

    BOOLEAN Duplicate;
    HPNPOPCTX hPnpOpCtx;
    PPNP_DEPENDENCY_EDGE SourceEdge;
    PLIST_ENTRY SourceList;
    PLIST_ENTRY SourceListEntry;
    PPNP_DEPENDENCY_EDGE TargetEdge;
    PLIST_ENTRY TargetList;
    PLIST_ENTRY TargetListEntry;

    PAGED_CODE();

    hPnpOpCtx = NULL;

    PNP_ASSERT(Target != NULL);
    PNP_ASSERT(Source != NULL);

    if (Mode == ProviderList) {
        SourceList = &Source->Providers;
        TargetList = &Target->Providers;

    } else {
        SourceList = &Source->Dependents;
        TargetList = &Target->Dependents;
    }

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    //
    // First, deal with duplicate edges that exist in both source and target.
    // Loop through source list to look for candidate edges to merge.
    //

    SourceListEntry = SourceList->Flink;
    while (SourceListEntry != SourceList) {
        SourceEdge = PiListEntryToDependencyEdge(SourceListEntry, Mode);
        SourceListEntry = SourceListEntry->Flink;

        //
        // Loop through target list, checking if a duplicate exists in the
        // target list.
        // 

        Duplicate = FALSE;
        TargetListEntry = TargetList->Flink;
        while (TargetListEntry != TargetList) {
            TargetEdge = PiListEntryToDependencyEdge(TargetListEntry, Mode);
            TargetListEntry = TargetListEntry->Flink;

            //
            // Check if the two edges share the same provider or dependent.
            //

            Duplicate = FALSE;
            if (Mode == ProviderList) {
                Duplicate = (SourceEdge->Provider == TargetEdge->Provider);

            } else {
                Duplicate = (SourceEdge->Dependent == TargetEdge->Dependent);
            }

            if (Duplicate != FALSE) {

                //
                // Target edge and source edge are duplicates. Merge in the
                // flags into the target edge. The source edge will be deleted.
                //

                PipMergeDependencyEdges(TargetEdge, SourceEdge);
                SourceEdge = NULL;
                break;
            }
        }

        if (Duplicate == FALSE) {

            //
            // Source edge did not have a duplicate in the target list.
            // Still, the source node needs to be swapped out with target node.
            //

            if (Mode == ProviderList) {
                PNP_ASSERT(SourceEdge->Dependent == Source);
                SourceEdge->Dependent = Target;
                PipNotifyDependenciesChanged(Target, SourceEdge->Provider);

            } else {
                PNP_ASSERT(SourceEdge->Provider == Source);
                SourceEdge->Provider = Target;
                PipNotifyDependenciesChanged(SourceEdge->Dependent, Target);
            }

            //
            // Update the references.
            //

            PipReferenceDependencyNode(Target);
            PipDereferenceDependencyNode(Source);
        }
    }

    //
    // At this point, there should be no intersection between the source list
    // and the target list. Move the rest of the entires from source list and
    // append them to the target list.
    //
    
    PipMoveListEntries(TargetList, SourceList);

#if DBG

    //
    // Verify integrity of edges in the target list.
    //

    TargetListEntry = TargetList->Flink;
    while (TargetListEntry != TargetList) {
        TargetEdge = PiListEntryToDependencyEdge(TargetListEntry, Mode);
        TargetListEntry = TargetListEntry->Flink;

        if (Mode == ProviderList) {
            PNP_ASSERT(TargetEdge->Dependent == Target);

        } else {
            PNP_ASSERT(TargetEdge->Provider == Target);
        }
    }

#endif

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }
}

PPNP_DEPENDENCY_EDGE
PiListEntryToDependencyEdge (
    __in PLIST_ENTRY ListEntry,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
)
/*++

Routine Description:

    Given a list entry that belongs to a dependency edge, convert it to a
    pointer to the dependency edge containing the list entry. Since an edge
    contains two list entries - one owned by the provider, and another owned
    by the dependent - this function simplifies access and avoids confusion.

    A simplified version of this would be 

        * PiGetDependentList / PiEnumerateDependentListEntry

        * PiGetProviderList / PiEnumerateProviderListEntry.

    Those functions should be preferred when only dealing with PDOs.

    Example:

        * Enumerating providers of a node
            
            ListHead = Node->Providers;
            ListEntry = ListHead->Flink;
            while (ListEntry != ListHead) {
                CurrentEdge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
                ListEntry = ListEntry->Flink;
                ...

        * Enumerating dependents of a node

            ListHead = Node->Dependents;
            ListEntry = ListHead->Flink;
            while (ListEntry != ListHead) {
                CurrentEdge = PiListEntryToDependencyEdge(ListEntry, DependentList);
                ListEntry = ListEntry->Flink;
                ...

Arguments:

    Mode - Supply ProviderList when iterating over the list of providers for a
        node.  Supply DependentList when iterating over the list of dependents
        for a node.

Return Value:

--*/

{

    PPNP_DEPENDENCY_EDGE Edge;

    if (Mode == ProviderList) {

        //
        // Iteration is over the list of provider edges for a dependent device.
        // The dependent device links the edges with DependentListEntry.
        //

        Edge = CONTAINING_RECORD(ListEntry,
                                 PNP_DEPENDENCY_EDGE,
                                 DependentListEntry);

    } else {

        //
        // Iteration is over the list of dependent edges for a provider device.
        // The provider device links the edges with ProviderListEntry.
        //

        Edge = CONTAINING_RECORD(ListEntry,
                                 PNP_DEPENDENCY_EDGE,
                                 ProviderListEntry);

    }

    return Edge;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipMergeDependencyEdges (
    __in PPNP_DEPENDENCY_EDGE Target,
    __in PPNP_DEPENDENCY_EDGE Source
)

/*++

Routine Description:

    Merge two dependency edges, combining information present in both.
    The two edges must have the same provider and the same dependent.
    The source edge will be deleted.

Arguments:

    Target - Supplies the target edge that will contain merged information from
        both edges.

    Source - Supplies the source edge; this edge will be freed.

Return Value:

    None.

--*/

{

    PPNP_BINDING_REQUEST_ENTRY Entry;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;

    PAGED_CODE();

    PNP_ASSERT(Target != NULL);
    PNP_ASSERT(Source != NULL);
    PNP_ASSERT(Target->Provider == Source->Provider ||
               Target->Dependent == Source->Dependent);
    
    //
    // Merge in reservation requests into target.
    //

    ListHead = &Source->Requests;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        Entry = CONTAINING_RECORD(ListEntry, PNP_BINDING_REQUEST_ENTRY, ListEntry);
        ListEntry = ListEntry->Flink;
        PipAddRequestToEdge(Target, &Entry->Request);
        Entry = NULL;
    }

    //
    // Clean up.
    //

    PipFreeDependencyEdge(Source, Target);
    Source = NULL;
}

_Requires_lock_held_(PiDependencyRelationsLock)
PPNP_DEPENDENCY_NODE
PipQueryBindingResolution (
    __in PPNP_BINDING_RESOLUTION Query
)

/*++

Routine Description:
    
    Search for a dependency node using the given query.

Arguments:

    Query - Supplies the basic information that uniquely identifiers a node.
        This can be either a PDO or a Binding ID.

Return Value:

    Returns the found node that matches the description provided in provided
    Query; NULL if not found.

--*/

{

    PPNP_BINDING_ID Id;
    PLIST_ENTRY IdListEntry;
    PPNP_DEPENDENCY_NODE Node;
    PLIST_ENTRY NodeListEntry;
    PPNP_DEPENDENCY_NODE Result;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(FALSE);
    PNP_ASSERT(Query != NULL);
    Result = NULL;
    switch(Query->Type) {
        case ResolvedWithPdo:
            Result = PNP_DO_TO_DEPENDENCY_NODE(Query->u.Pdo);
            break;

        case ResolvedWithLabel:

            //
            // Loop through each dependency node.
            //

            NodeListEntry = PiDependencyNodeListHead.Flink;
            while (NodeListEntry != &PiDependencyNodeListHead) {
                Node = CONTAINING_RECORD(NodeListEntry,
                                         PNP_DEPENDENCY_NODE,
                                         ListEntry);
                //
                // Loop through each ID of this node.
                //

                NodeListEntry = NodeListEntry->Flink;
                IdListEntry = Node->BindingIds.Flink;
                while (IdListEntry != &Node->BindingIds) {
                    Id = CONTAINING_RECORD(IdListEntry,
                                           PNP_BINDING_ID,
                                           ListEntry);

                    IdListEntry = IdListEntry->Flink;
                    if (RtlEqualUnicodeString(&Id->Label,
                                              Query->u.Label,
                                              TRUE) != FALSE) {

                        Result = Node;
                        break;
                    }
                }

                if (Result != NULL) {
                    break;
                }
            }

            break;

        default:
            PNP_ASSERT(FALSE);
            break;
    }

    return Result;
}

_Requires_lock_held_(PiDependencyRelationsLock)
PLIST_ENTRY
PiGetDependentList (
    __in PDEVICE_OBJECT DeviceObject
)

/*++

Routine Description:

    Use this function to initiate an enumeration over the list of dependents
    of the given device. See PiEnumerateDependentListEntry for details.

Arguments:

    DeviceObject - Supplies the device for enumerating its dependent devices.

Return Value:

    Head of the list of dependent edges of the corresponding device node;
    if the device has no dependents, an empty list is returned.
    
--*/

{

    PPNP_DEPENDENCY_NODE Node;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);
    PiAssertDependencyRelationsLockHeld(FALSE);
    Node = PNP_DO_TO_DEPENDENCY_NODE(DeviceObject);
    if (Node == NULL) {
        return &PiDependencyNodeEmptyList;
    } else {
        return &Node->Dependents;
    }
}

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PiEnumerateDependentListEntry (
    __in PLIST_ENTRY ListEntry,
    __out PDEVICE_OBJECT *Dependent,
    __out_opt PULONG DependencyTypes
)

/*++

Routine Description:

    Used to enumerate over the list of dependents started by a call to
    PiGetDependentList.

    Example:

        ListHead = PiGetDependentList(DeviceObject);
        ListEntry = ListHead->Flink;
        while (ListEntry != ListHead) {
            PiEnumerateProviderListEntry(ListEntry, &DependentPdo, &DependencyTypes);
            ListEntry = ListHead->Flink;

            //
            // Use Dependent and DependencyTypes
            //
        }

Arguments:

    ListEntry - Supplies the current list entry in enumeration.

    Dependent - Retruns the dependent PDO. May be null if the dependent is not
        yet known in the system.

    DependencyTypes - Returns the types of dependency that the dependent device
        has on the returned provider PDO.

Return Value:

    None.

--*/

{

    PPNP_DEPENDENCY_EDGE Edge;

    PAGED_CODE();

    PNP_ASSERT(ListEntry != NULL);
    PNP_ASSERT(Dependent != NULL);
    PiAssertDependencyRelationsLockHeld(FALSE);
    Edge = PiListEntryToDependencyEdge(ListEntry, DependentList);
    *Dependent = Edge->Dependent->DeviceObject;
    if (ARGUMENT_PRESENT(DependencyTypes)) {
        *DependencyTypes = Edge->DependencyTypes;
    }
}

_Requires_lock_held_(PiDependencyRelationsLock)
PLIST_ENTRY
PiGetProviderList (
    __in PDEVICE_OBJECT DeviceObject
)

/*++

Routine Description:

    Use this function to initiate an enumeration over the list of providers
    of the given device. See PiEnumerateProviderListEntry for details.

Arguments:

    DeviceObject - Supplies the device for enumerating its provider devices.

Return Value:

    Head of the list of providing edges of the corresponding device node;
    if the device has no providers, an empty list is returned.
    
--*/

{

    PPNP_DEPENDENCY_NODE Node;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);
    PiAssertDependencyRelationsLockHeld(FALSE);
    Node = PNP_DO_TO_DEPENDENCY_NODE(DeviceObject);
    if (Node == NULL) {
        return &PiDependencyNodeEmptyList;
    } else {
        return &Node->Providers;
    }
}

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PiEnumerateProviderListEntry (
    __in PLIST_ENTRY ListEntry,
    __out PDEVICE_OBJECT *Provider,
    __out_opt PULONG DependencyTypes
)

/*++

Routine Description:

    Used to enumerate over the list of providers started by a call to
    PiGetProviderList.

    Example:

        ListHead = PiGetProviderList(DeviceObject);
        ListEntry = ListHead->Flink;
        while (ListEntry != ListHead) {
            PiEnumerateProviderListEntry(ListEntry, &ProviderPdo, &DependencyTypes);
            ListEntry = ListHead->Flink;

            //
            // Use ProviderPdo and DependencyTypes
            //
        }

Arguments:

    ListEntry - Supplies the current list entry in enumeration.

    Provider - Retruns the provider PDO. May be null if the provider is not
        yet known in the system.

    DependencyTypes - Returns the types of dependency that the dependent device
        has on the returned provider PDO.

Return Value:

    None.

--*/

{

    PPNP_DEPENDENCY_EDGE Edge;

    PAGED_CODE();

    PNP_ASSERT(ListEntry != NULL);
    PNP_ASSERT(Provider != NULL);
    PiAssertDependencyRelationsLockHeld(FALSE);
    Edge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
    *Provider = Edge->Provider->DeviceObject;
    if (ARGUMENT_PRESENT(DependencyTypes) != FALSE) {
        *DependencyTypes = Edge->DependencyTypes;
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDependencyNodeClearVisitedFlag (
    VOID
)

/*++

Routine Description:

    Clear PNP_DEPENDENCY_NODE_VISITED and PNP_DEPENDENCY_NODE_PROCESSING flags
    from all dependency nodes in the system. These transient flags are used
    for graph traversal operations to temporarily tag nodes.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PLIST_ENTRY ListEntry;
    PPNP_DEPENDENCY_NODE Node;

    PAGED_CODE();
    
    PiAssertDependencyRelationsLockHeld(TRUE);
    ListEntry = PiDependencyNodeListHead.Flink;
    while (ListEntry != &PiDependencyNodeListHead) {
        Node = CONTAINING_RECORD(ListEntry, PNP_DEPENDENCY_NODE, ListEntry);
        ListEntry = ListEntry->Flink;
        Node->Flags &= ~PNP_DEPENDENCY_NODE_VISITED;
        Node->Flags &= ~PNP_DEPENDENCY_NODE_PROCESSING;
    }
}

VOID
PipMoveListEntries (
    __inout PLIST_ENTRY TargetHead,
    __inout PLIST_ENTRY SourceHead
)

/*++

Routine Description:

    Given two lists, move all list entries in the source list to the target
    list, appending the beginning of source list (excluding the source head)
    to the end of target list.

Arguments:

    TargetHead - Supplies the head of the list that receives list entries.

    SourceHead - Supplies the head of the list that provides list entries.

Return Value:

    None.

--*/

{

    PLIST_ENTRY FirstEntry;

    if (IsListEmpty(SourceHead) == FALSE) {
        FirstEntry = SourceHead->Flink;
        RemoveEntryList(SourceHead);
        InitializeListHead(SourceHead);
        AppendTailList(TargetHead, FirstEntry);
    }

    PNP_ASSERT(IsListEmpty(SourceHead));
}

BOOLEAN
PipIsDeviceReadyForPowerRelations (
    __in PDEVICE_OBJECT DeviceObject
)

/*++

Routine Description:

    Check if the device is able to rebuild power relations.

Arguments:

    DeviceObject - supplies the device to check.

Return Value:

    TRUE if power relations can be successfully rebuilt for this device;
    FALSE otherwise.

--*/

{

    PAGED_CODE();

    if (DeviceObject == NULL ||
        PP_DO_TO_DN(DeviceObject) == NULL ||
        PP_DO_TO_DN(DeviceObject)->State == DeviceNodeUninitialized) {

        return FALSE;

    } else {
        return TRUE;
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipReferenceDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
)

/*++

Routine Description:

    Put a reference on the given dependency node.

Arguments:

    Node - Supplies the dependency node to add a reference on.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    Node->ReferenceCount++;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDereferenceDependencyNode (
    __in PPNP_DEPENDENCY_NODE Node
)

/*++

Routine Description:

    Remove a reference from the given dependency node. The node may be deleted
    when the reference count reahces zero after decrementing.

Arguments:

    Node - Supplies the dependency node to remove a reference from.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    PNP_ASSERT(Node->ReferenceCount > 0);
    Node->ReferenceCount--;
    if (Node->ReferenceCount == 0) {
        PipDeleteDependencyNode(Node);
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipLinkDeviceObjectAndDependencyNode (
    __in PDEVICE_OBJECT DeviceObject,
    __in PPNP_DEPENDENCY_NODE DependencyNode
)

/*++

Routine Description:

    Cross-reference a device object and a dependency node.
    Puts a reference on the dependency node.

Arguments:

    DeviceObject - Supplies the device object to link to a dependency node.

    DependencyNode - Supplies the dependency node to link to a device object.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    PNP_ASSERT(DeviceObject != NULL);
    PNP_ASSERT(DependencyNode != NULL);
    PNP_ASSERT(DependencyNode->DeviceObject == NULL);
    PNP_ASSERT(PNP_DO_TO_DEPENDENCY_NODE(DeviceObject) == NULL);

    PipReferenceDependencyNode(DependencyNode);
    DependencyNode->DeviceObject = DeviceObject;
    DeviceObject->DeviceObjectExtension->DependencyNode = DependencyNode;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipUnlinkDeviceObjectAndDependencyNode (
    __in PDEVICE_OBJECT DeviceObject,
    __in PPNP_DEPENDENCY_NODE DependencyNode
)

/*++

Routine Description:

    Remove the cross-reference between a device object and a dependency node.
    Removes the reference on the dependency node.

Arguments:

    DeviceObject - Supplies the device object to unlink from a dependency node.

    DependencyNode - Supplies the dependency node to unlink from a device object.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    PNP_ASSERT(DeviceObject != NULL);
    PNP_ASSERT(DependencyNode != NULL);
    PNP_ASSERT(DependencyNode->DeviceObject == DeviceObject);
    PNP_ASSERT(PNP_DO_TO_DEPENDENCY_NODE(DeviceObject) == DependencyNode);

    DeviceObject->DeviceObjectExtension->DependencyNode = NULL;
    DependencyNode->DeviceObject = NULL;
    PipDereferenceDependencyNode(DependencyNode);
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipConvertResolutionsToReservations (
    __in PPNP_DEPENDENCY_NODE ProviderNode
)

/*++

Routine Description:

    Convert all ID resolutions made by this dependency node back to
    reservations. If a dependent device depended on label FOO and the provided
    device resolved it, the dependent device will not be dependeont a new
    placeholder dependency node with label FOO, so that if another device
    resolves FOO the dependent device can start.

    After calling this method, power relations update queue must be checked
    since new edges may have been created.

Arguments:

    DependencyNode - Supplies the dependency node that will have its binding IDs
        removed and converted to reservations.

Return Value:

    NT status code.

--*/

{
    
    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PNP_BINDING_RESOLUTION Dependent;
    PLIST_ENTRY EdgeListEntry;
    PLIST_ENTRY EdgeListHead;
    PPNP_BINDING_REQUEST_ENTRY CurrentRequest;
    PLIST_ENTRY RequestListEntry;
    PLIST_ENTRY RequestListHead;
    NTSTATUS Status;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);
    if (ProviderNode == NULL) {

        PNP_ASSERT(FALSE);

        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    Status = STATUS_SUCCESS;

    //
    // The device is going away; let go of all the IDs so that other device can
    // claim them later.
    //

    PipDeleteBindingIds(ProviderNode);

    //
    // Loop through dependent (incoming) edges.
    //

    EdgeListHead = &ProviderNode->Dependents;
    EdgeListEntry = EdgeListHead->Flink;
    while (EdgeListEntry != EdgeListHead) {
        CurrentEdge = CONTAINING_RECORD(EdgeListEntry,
                                        PNP_DEPENDENCY_EDGE,
                                        ProviderListEntry);

        EdgeListEntry = EdgeListEntry->Flink;
        if (CurrentEdge->DeletePending != FALSE) {
            continue;
        }

        //
        // Loop through original reservation requests.
        //

        RequestListHead = &CurrentEdge->Requests;
        RequestListEntry = RequestListHead->Flink;
        while (RequestListEntry != RequestListHead) {
            CurrentRequest = CONTAINING_RECORD(RequestListEntry,
                                              PNP_BINDING_REQUEST_ENTRY,
                                              ListEntry);

            RequestListEntry = RequestListEntry->Flink;

            //
            // Convert these label-based resolutions to reservations.
            //

            if (CurrentRequest->Request.Provider.Type == ResolvedWithLabel) {

                PNP_ASSERT(CurrentEdge->Dependent->DeviceObject != NULL);

                Dependent.Type = ResolvedWithPdo;
                Dependent.u.Pdo = CurrentEdge->Dependent->DeviceObject;
                Status = PipSetDependency(&Dependent,
                                          &CurrentRequest->Request);
            }
        }

        CurrentEdge->DeletePending = TRUE;
    }

Exit:

    return Status;
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PipDeleteAllDependencyRelations (
    __in PDEVICE_OBJECT DeviceObject
)

/*++

Routine Description:

    This routine is used to clear all dependency relations for a given
    device. This should be called when the device has been deleted.

    After calling this method, power relations update queue must be checked.

Arguments:

    DeviceObject - Supplies the device where all dependency relations that
        involves this device will be deleted.

Return Value:

    None.

--*/

{
    
    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PPNP_DEPENDENCY_NODE DependencyNode;
    PPNP_DEPENDENCY_NODE Dependent;
    PDEVICE_NODE DeviceNode;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;

    PAGED_CODE();

    PiAssertDependencyRelationsLockHeld(TRUE);

    DeviceNode = PP_DO_TO_DN(DeviceObject);
    if (DeviceNode != NULL) {
        PNP_ASSERT((DeviceNode->Flags & DNF_ENUMERATED) == 0);
    }

    //
    // If the device isn't invovled in any dependencies, nothing to do.
    //

    DependencyNode = PNP_DO_TO_DEPENDENCY_NODE(DeviceObject);
    if (DependencyNode == NULL) {
        goto Exit;
    }

    PNP_ASSERT(DependencyNode->DeviceObject == DeviceObject);

    //
    // Put a reference on the dependency node while work is performed.
    //

    PipReferenceDependencyNode(DependencyNode);

    //
    // Check if this device still has any binding IDs that need to be
    // converted back to reservations.
    //

    PipConvertResolutionsToReservations(DependencyNode);

    PNP_ASSERT(IsListEmpty(&DependencyNode->BindingIds));
    
    //
    // Clear the list of Providers. This node is a dependent in this relationship.
    //

    ListHead = &DependencyNode->Providers;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        CurrentEdge = PiListEntryToDependencyEdge(ListEntry, ProviderList);
        ListEntry = ListEntry->Flink;

        PNP_ASSERT(CurrentEdge->Dependent == DependencyNode);
        
        PipDeleteDependencyEdge(CurrentEdge);
        CurrentEdge = NULL;
    }

    PNP_ASSERT(IsListEmpty(&DependencyNode->Providers));

    //
    // Clear the list of Dependents. This node is a provider in this relationship.
    //

    ListHead = &DependencyNode->Dependents;
    ListEntry = ListHead->Flink;
    while (ListEntry != ListHead) {
        CurrentEdge = PiListEntryToDependencyEdge(ListEntry, DependentList);
        ListEntry = ListEntry->Flink;

        PNP_ASSERT(CurrentEdge->Provider == DependencyNode);

        //
        // Remember the dependent node and hold a reference on it, since
        // deleting the edge may cause the node to reach zero references and
        // be freed.
        //

        Dependent = CurrentEdge->Dependent;
        PipReferenceDependencyNode(Dependent);

        //
        // Remove the edge.
        //

        PipDeleteDependencyEdge(CurrentEdge);
        CurrentEdge = NULL;

        //
        // Check if the dependent can be restarted.
        //
        // It is possible that its provider requirements became satisfied
        // while removal was being processed.
        //
        // Only do this if the dependent actually has dependencies, otherwise
        // it would be restarted unexpectedly right after removing it.
        //

        if ((Dependent->DeviceObject != NULL) &&
            (IsListEmpty(&Dependent->Providers) == FALSE)) {

            PipAttemptDependentStart(Dependent->DeviceObject);
        }

        PipDereferenceDependencyNode(Dependent);
    }

    PNP_ASSERT(IsListEmpty(&DependencyNode->Dependents));

    //
    // Break the link between device object and dependency node.
    //

    PipUnlinkDeviceObjectAndDependencyNode(DeviceObject, DependencyNode);

    //
    // Remove the reference. This may cause the node to be deleted.
    //

    PipDereferenceDependencyNode(DependencyNode);
    DependencyNode = NULL;

Exit:

    return;
}

_Requires_lock_not_held_(PiDependencyRelationsLock)
NTSTATUS
PnpGetDeviceDependencyList(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode,
    __out_ecount(DependencyListLength) PZZWSTR DependencyList,
    __in ULONG DependencyListLength,
    __out_opt PULONG RequiredLength
    )

/*++

Routine Description:

    This routine returns a list of any dependencies of the specified type that
    the given device node participates in.

Arguments:

    DeviceNode - The device node whose PDO is being checked for dependencies.

    Mode - Supply ProviderList when iterating over the list of providers for a
        node.  Supply DependentList when iterating over the list of dependents
        for a node.

    DependencyList - Returns the list of dependencies that the given
        device participates in.  If no dependencies are found, this will be an
        empty multi-sz string.

    DependencyProviderListLength - Supplies the number of characters in
        DependencyList.

    RequiredLength - Optionally returns the number of characters required to
        store the list of dependencies.

Return Value:

    NTSTATUS - STATUS_SUCCESS if the dependency list is successfully returned.
        STATUS_BUFFER_TOO_SMALL if DependencyList is not large enough to hold
        the dependency list.

--*/

{

    PPNP_BINDING_ID BindingId;
    PLIST_ENTRY BindingIdsLink;
    size_t CharactersRemaining;
    PDEVICE_NODE CurrentDeviceNode;
    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PLIST_ENTRY CurrentLink;
    PWSTR CurrentPosition;
    PDEVICE_OBJECT DeviceObject;
    PLIST_ENTRY EdgeList;
    size_t IdLength;
    PWSTR Label;
    PPNP_DEPENDENCY_NODE Node;
    PDEVICE_OBJECT PhysicalDeviceObject;
    ULONG RequiredCharacters;
    NTSTATUS Status;

    PAGED_CODE();

    CharactersRemaining = DependencyListLength;
    CurrentPosition = DependencyList;
    Label = NULL;
    PhysicalDeviceObject = DeviceNode->PhysicalDeviceObject;
    RequiredCharacters = 0;
    Status = STATUS_SUCCESS;

    PnpAcquireDependencyRelationsLock(FALSE);

    if (Mode == ProviderList) {
        EdgeList = PiGetProviderList(PhysicalDeviceObject);
    } else if (Mode == DependentList) {
        EdgeList = PiGetDependentList(PhysicalDeviceObject);
    } else {
        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    CurrentLink = EdgeList->Flink;
    while (CurrentLink != EdgeList) {
        CurrentEdge = PiListEntryToDependencyEdge(CurrentLink,
                                                  Mode);
        CurrentLink = CurrentLink->Flink;

        if (Mode == ProviderList) {
            Node = CurrentEdge->Provider;
        } else {
            Node = CurrentEdge->Dependent;
        }

        DeviceObject = Node->DeviceObject;
        CurrentDeviceNode = PP_DO_TO_DN(DeviceObject);
        if ((DeviceObject == NULL) ||
            (CurrentDeviceNode == NULL) ||
            (PnpIsDeviceInstancePathEmpty(CurrentDeviceNode))) {

            BindingIdsLink = Node->BindingIds.Flink;
            while (BindingIdsLink != &(Node->BindingIds)) {
                BindingId = CONTAINING_RECORD(BindingIdsLink,
                                              PNP_BINDING_ID,
                                              ListEntry);
                BindingIdsLink = BindingIdsLink->Flink;

                Status = PnpUnicodeStringToWstr(&Label, NULL, &(BindingId->Label));
                if (!NT_SUCCESS(Status)) {
                    break;
                }

                Status = RtlStringCchLengthW(Label,
                                             NTSTRSAFE_UNICODE_STRING_MAX_CCH,
                                             &IdLength);
                if (!NT_SUCCESS(Status)) {
                    PnpUnicodeStringToWstrFree(Label, &(BindingId->Label));
                    break;
                }

                IdLength++;
                RequiredCharacters += (ULONG)IdLength;
                if (IdLength <= CharactersRemaining) {
                    Status = RtlStringCchCopyExW(CurrentPosition,
                                                 CharactersRemaining,
                                                 Label,
                                                 &CurrentPosition,
                                                 &CharactersRemaining,
                                                 STRSAFE_NULL_ON_FAILURE);
                    if (!NT_SUCCESS(Status)) {
                        PnpUnicodeStringToWstrFree(Label, &(BindingId->Label));
                        break;
                    }

                    //
                    // Move past the null terminator.
                    //

                    CurrentPosition++;
                    CharactersRemaining--;
                }

                PnpUnicodeStringToWstrFree(Label, &(BindingId->Label));
            }

            if (!NT_SUCCESS(Status)) {
                break;
            }
        } else {
            Status = RtlStringCchLengthW(PnpGetDeviceInstancePath(CurrentDeviceNode)->Buffer,
                                         MAX_DEVICE_ID_LEN,
                                         &IdLength);
            if (!NT_SUCCESS(Status)) {
                break;
            }

            IdLength++;
            RequiredCharacters += (ULONG)IdLength;
            if (IdLength <= CharactersRemaining) {
                Status = RtlStringCchCopyExW(CurrentPosition,
                                             CharactersRemaining,
                                             PnpGetDeviceInstancePath(CurrentDeviceNode)->Buffer,
                                             &CurrentPosition,
                                             &CharactersRemaining,
                                             STRSAFE_NULL_ON_FAILURE);
                if (!NT_SUCCESS(Status)) {
                    break;
                }

                //
                // Move past the null terminator.
                //

                CurrentPosition++;
                CharactersRemaining--;
            }
        }
    }

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // Add on the NULL to end the multi-sz
    //

    RequiredCharacters++;
    if (RequiredCharacters > DependencyListLength) {
        Status = STATUS_BUFFER_TOO_SMALL;
        goto Exit;
    }
    DependencyList[RequiredCharacters-1] = UNICODE_NULL;

  Exit:

    PnpReleaseDependencyRelationsLock();

    if (((NT_SUCCESS(Status)) || (Status == STATUS_BUFFER_TOO_SMALL)) &&
        (RequiredLength != NULL)) {
        *RequiredLength = RequiredCharacters;
    }

    return Status;
}

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PipNotifyDependenciesChanged(
    __in_opt PPNP_DEPENDENCY_NODE Dependent,
    __in_opt PPNP_DEPENDENCY_NODE Provider
    )

/*++

Routine Description:

    Notify that dependencies have changed.

Arguments:

    DependentNode - Supplies the dependent node in the relationship.

    ProviderNode - Supplies the provider node in the relationship.

Return Value:

    None.

--*/
{
    PDEVICE_NODE DeviceNode;

    PAGED_CODE();

    //
    // Raise PnpRtl event for updated dependents of the provider device.
    //

    if (Provider && Provider->DeviceObject) {
        DeviceNode = PP_DO_TO_DN(Provider->DeviceObject);

        if ((DeviceNode) && (!PnpIsDeviceInstancePathEmpty(DeviceNode))) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_DEPENDENCY_DEPENDENTS);
        }
    }

    //
    // Raise PnpRtl event for updated provider of the dependent device.
    //

    if (Dependent && Dependent->DeviceObject) {
        DeviceNode = PP_DO_TO_DN(Dependent->DeviceObject);

        if ((DeviceNode) && (!PnpIsDeviceInstancePathEmpty(DeviceNode))) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_DEPENDENCY_PROVIDERS);
        }
    }
}

_Requires_lock_not_held_(PiDependencyRelationsLock)
VOID
PipNotifyDeviceDependencyList(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
    )

/*++

Routine Description:

    This routine notifies about changes in dependencies of any devices involved
    in a dependency with the specified device.

Arguments:

    DeviceNode - The device node whose dependencies should be notified.

    Mode - Supply ProviderList when iterating over the list of providers for a
        node.  Supply DependentList when iterating over the list of dependents
        for a node.

Return Value:

    None.

--*/

{

    PPNP_DEPENDENCY_EDGE CurrentEdge;
    PLIST_ENTRY CurrentLink;
    PLIST_ENTRY EdgeList;
    PDEVICE_OBJECT PhysicalDeviceObject;

    PAGED_CODE();

    PhysicalDeviceObject = DeviceNode->PhysicalDeviceObject;

    PnpAcquireDependencyRelationsLock(FALSE);

    if (Mode == ProviderList) {
        EdgeList = PiGetProviderList(PhysicalDeviceObject);
    } else if (Mode == DependentList) {
        EdgeList = PiGetDependentList(PhysicalDeviceObject);
    } else {
        goto Exit;
    }

    CurrentLink = EdgeList->Flink;
    while (CurrentLink != EdgeList) {
        CurrentEdge = PiListEntryToDependencyEdge(CurrentLink,
                                                  Mode);
        CurrentLink = CurrentLink->Flink;

        if (Mode == ProviderList) {
            PipNotifyDependenciesChanged(NULL, CurrentEdge->Provider);
        } else {
            PipNotifyDependenciesChanged(CurrentEdge->Dependent, NULL);
        }
    }

  Exit:

    PnpReleaseDependencyRelationsLock();
}

BOOLEAN
PipIsProviderStarted (
    __in PDEVICE_NODE ProviderNode
    )

/*++

Routine Description:

    This routine determines if a device node has in a valid "started" state
    such that its dependents are allowed to start.

Arguments:

    ProviderNode - The device node whose state is being checked.

Return Value:

    TRUE if the device is started; FALSE otherwise.

--*/

{

    BOOLEAN Started;

    PAGED_CODE();

    NT_ASSERT(ProviderNode != NULL);

    switch (ProviderNode->State) {
    case DeviceNodeUninitialized:
    case DeviceNodeInitialized:
    case DeviceNodeDriversAdded:
    case DeviceNodeResourcesAssigned:
    case DeviceNodeStartPending:
    case DeviceNodeStartCompletion:
    case DeviceNodeStartPostWork:
        Started = FALSE;
        break;

    case DeviceNodeQueryStopped:
    case DeviceNodeStopped:
    case DeviceNodeRestartCompletion:
        Started = FALSE;
        break;

    case DeviceNodeStarted:
    case DeviceNodeEnumeratePending:
    case DeviceNodeEnumerateCompletion:
        Started = TRUE;
        break;

    case DeviceNodeAwaitingQueuedDeletion:
    case DeviceNodeAwaitingQueuedRemoval:
    case DeviceNodeQueryRemoved:
    case DeviceNodeRemovePendingCloses:
    case DeviceNodeRemoved:
    case DeviceNodeDeletePendingCloses:
    case DeviceNodeDeleted:
        Started = FALSE;
        break;

    default:

        PNP_ASSERT(FALSE);

        Started = FALSE;
        break;
    }

    return Started;
}