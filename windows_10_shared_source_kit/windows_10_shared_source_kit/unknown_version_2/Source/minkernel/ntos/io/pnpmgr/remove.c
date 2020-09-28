/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    remove.c

Abstract:

    This module contains routines to perform device removal

Author:

    Robert B. Nelson (RobertN) Jun 1, 1998.

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'edpP')
#endif

_Guarded_by_(IopSurpriseRemoveListLock) BOOLEAN PnpDelayedRemovePending = FALSE;
WORK_QUEUE_ITEM PnpDelayedRemoveWorkItem;
WORKER_THREAD_ROUTINE PnpDelayedRemoveWorker;

BOOLEAN
PipDeviceRemovalCheckDeviceNodeState (
    PDEVICE_NODE DeviceNode,
    PDEVICE_NODE TargetDeviceNode,
    BOOLEAN TreeDeletion
);

NTSTATUS
PipRemoveDevicesInRelationList (
    _In_ PPENDING_RELATIONS_LIST_ENTRY Entry
);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PnpChainDereferenceComplete)
#pragma alloc_text(PAGE, PnpDelayedRemoveWorker)
#pragma alloc_text(PAGE, PipRemoveDevicesInRelationList)
#pragma alloc_text(PAGE, PnpDeleteLockedDeviceNode)
#pragma alloc_text(PAGE, PnpCancelRemoveOnHungDevices)
#pragma alloc_text(PAGE, PnpSurpriseRemoveLockedDeviceNode)
#pragma alloc_text(PAGE, PnpQueryRemoveLockedDeviceNode)
#pragma alloc_text(PAGE, PnpCancelRemoveLockedDeviceNode)
#pragma alloc_text(PAGE, PnpDeleteLockedDeviceNodes)
#pragma alloc_text(PAGE, PnpInvalidateRelationsInList)
#pragma alloc_text(PAGE, PnpBuildRemovalRelationList)
#pragma alloc_text(PAGE, PnpProcessCompletedEject)
#pragma alloc_text(PAGE, PnpProcessRelation)
#pragma alloc_text(PAGE, PnpQueuePendingEject)
#pragma alloc_text(PAGE, PnpQueuePendingSurpriseRemoval)
#pragma alloc_text(PAGE, PnpDequeuePendingSurpriseRemoval)
#pragma alloc_text(PAGE, PnpUnloadAttachedDriver)
#pragma alloc_text(PAGE, PnpUnlinkDeviceRemovalRelations)
#pragma alloc_text(PAGE, PnpRequestDeviceRemoval)
#pragma alloc_text(PAGE, PnpRequestDeviceRemovalWorker)
#pragma alloc_text(PAGE, PnpIsBeingRemovedSafely)
#pragma alloc_text(PAGE, PnpProcessBusRelations)
#pragma alloc_text(PAGE, PnpProcessDependencyRelations)
#pragma alloc_text(PAGE, PipDeviceRemovalCheckDeviceNodeState)
#endif

VOID
PnpChainDereferenceComplete (
    __in  PDEVICE_OBJECT  PhysicalDeviceObject,
    __in  LOGICAL         OnCleanStack
    )

/*++

Routine Description:

    This routine is invoked when the reference count on a PDO and all its
    attached devices transitions to a zero.  It tags the devnode as ready for
    removal.  If all the devnodes are tagged then PnpDelayedRemoveWorker is
    called to actually send the remove IRPs.

Arguments:

    PhysicalDeviceObject - Supplies a pointer to the PDO whose references just
        went to zero.

    OnCleanStack - Indicates whether the current thread is in the middle a
                   driver operation.

Return Value:

    None.

--*/

{

    PPENDING_RELATIONS_LIST_ENTRY   entry;
    PLIST_ENTRY                     link;
    ULONG                           count;
    PDEVICE_NODE                    deviceNode;
    ULONG                           taggedCount;
    NTSTATUS                        status;

    PAGED_CODE();

    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&IopSurpriseRemoveListLock, TRUE);

    //
    // Find the relation list this devnode is a member of.
    //

    for (link = IopPendingSurpriseRemovals.Flink;
         link != &IopPendingSurpriseRemovals;
         link = link->Flink) {

        entry = CONTAINING_RECORD(link, PENDING_RELATIONS_LIST_ENTRY, Link);
        if (entry->DequeuePending != FALSE) {
            continue;
        }

        //
        // Try to tag the devnode as ready for remove.
        //

        status = IopSetRelationsTag(entry->RelationsList,
                                    PhysicalDeviceObject,
                                    DEVICE_TAGGED);

        //
        // If the device is not in this relation list, continue searching.
        //

        if (!NT_SUCCESS(status)) {

            PNP_ASSERT(status == STATUS_NO_SUCH_DEVICE);

            continue;
        }

        taggedCount = IopGetRelationsTaggedCount(entry->RelationsList);
        count = IopGetRelationsCount(entry->RelationsList);
        deviceNode = PP_DO_TO_DN(entry->DeviceObject);

        NT_ASSERT_ASSUME(deviceNode != NULL);

        //
        // If the device node is not going through another transition
        // (surprise remove after a surprise fail) and has no references,
        // it is ready for removal.
        //

        PNP_ASSERT(deviceNode->State != DeviceNodeAwaitingQueuedRemoval);

        if (deviceNode->State != DeviceNodeAwaitingQueuedDeletion &&
            taggedCount == count) {

            //
            // Mark the entry as ready for removal.
            //

            entry->DequeuePending = TRUE;

            //
            // If there is a work item pending, nothing more to do.
            //

            if (PnpDelayedRemovePending != FALSE) {
                goto PnpChainDereferenceCompleteUnlock;
            }

            PnpDelayedRemovePending = TRUE;
            ExReleaseResourceLite(&IopSurpriseRemoveListLock);
            KeLeaveCriticalRegion();
            if ((!OnCleanStack) ||
                (PsGetCurrentProcess() != PsInitialSystemProcess)) {

                //
                // Queue a work item to do the removal so we call the
                // driver in the system process context rather than the
                // random one we're in now.
                //

                ExInitializeWorkItem(&PnpDelayedRemoveWorkItem,
                                     PnpDelayedRemoveWorker,
                                     NULL);

                ExQueueWorkItem(&PnpDelayedRemoveWorkItem, DelayedWorkQueue);

            } else {

                //
                // We are already in the system process and not in some
                // random ObDeref call, so call the worker inline.
                //

                PnpDelayedRemoveWorker(NULL);
            }

            goto PnpChainDereferenceCompleteEnd;
        }
    }

PnpChainDereferenceCompleteUnlock:
    ExReleaseResourceLite(&IopSurpriseRemoveListLock);
    KeLeaveCriticalRegion();

PnpChainDereferenceCompleteEnd:
    return;
}

_Use_decl_annotations_
VOID
PnpDelayedRemoveWorker (
    PVOID Context
    )

/*++

Routine Description:

    This routine is usually called from a worker thread to actually send the
    remove IRPs once the reference count on a PDO and all its attached devices
    transitions to a zero.

Arguments:

    Context - Unused.

Return Value:

    None.

--*/

{

    PPENDING_RELATIONS_LIST_ENTRY Entry;
    LIST_ENTRY ReadyListHead;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);
    PNP_ASSERT(Context == NULL);

    InitializeListHead(&ReadyListHead);
    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    //
    // No need to enter critical region since acquiring tree lock already
    // did it.
    //

    ExAcquireResourceExclusiveLite(&IopSurpriseRemoveListLock, TRUE);
    PnpDequeuePendingSurpriseRemoval(&ReadyListHead);
    PnpDelayedRemovePending = FALSE;
    ExReleaseResourceLite(&IopSurpriseRemoveListLock);
    while (IsListEmpty(&ReadyListHead) == FALSE) {
        Entry = CONTAINING_RECORD(RemoveHeadList(&ReadyListHead),
                                  PENDING_RELATIONS_LIST_ENTRY,
                                  Link);

        Status = PipRemoveDevicesInRelationList(Entry);

        //
        // The entry could be put back into the queue for failure cases, but
        // that would require the lock to be held throughout, which is not
        // desirable.
        //

        NT_ASSERT(NT_SUCCESS(Status));
    }

    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
}

NTSTATUS
PipRemoveDevicesInRelationList (
    PPENDING_RELATIONS_LIST_ENTRY Entry
)

/*++

Routine Description:

    Send final remove IRPs to all devices in the given relation list.

Arguments:

    Entry - Supplies a pointer to the pending relation list.

Return Value:

    NT status code.

--*/

{

    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    BOOLEAN unlink;

    PAGED_CODE();

    PNP_ASSERT(Entry != NULL);
    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    deviceNode = PP_DO_TO_DN(Entry->DeviceObject);
    PNP_ASSERT(deviceNode != NULL);
    if (deviceNode == NULL) {
        status = STATUS_UNSUCCESSFUL;
        goto DelayedRemoveWorkerExit;
    }

    unlink = FALSE;
    if (!(deviceNode->Flags & DNF_ENUMERATED) && deviceNode->Parent) {

        //
        // We are sending the final remove. If the devnode has been reported missing and
        // is still in the tree, we need to remove it from the tree after sending the
        // remove irp.
        //

        unlink = TRUE;
    }

    //
    // Sort the devices according to their removal order. This needs to be
    // re-done since the relation list may have been merged with others.
    //

    status = IopSortRelationListForRemove(Entry->RelationsList);
    if (!NT_SUCCESS(status) || Entry->RelationsList->Sorted == FALSE) {

        //
        // Sorting failed - we will continue but remove IRPs will be delievered
        // in wrong order. However, this is better than leaving surprise
        // removed devices in limbo.
        //

        NT_ASSERT(FALSE);
    }

    //
    // Call RemoveDevice on the list.
    //

    PnpDeleteLockedDeviceNodes(Entry->DeviceObject,
                               Entry->RelationsList,
                               RemoveDevice,           // OperationCode
                               FALSE,                  // ProcessDeviceRelations
                               Entry->Problem,         // Problem
                               STATUS_SUCCESS,         // ProblemStatus
                               NULL,                   // VetoType
                               NULL);                  // VetoName

    if (unlink) {

        //
        // If the root device is being converted from a surprise fail to a
        // surprise remove, leave it alone here and let the surprise remove
        // take care of unlinking it from the tree.
        //

        if (deviceNode->State == DeviceNodeAwaitingQueuedDeletion) {

            PNP_ASSERT((deviceNode->Parent != NULL) &&
                       (deviceNode->PreviousState ==
                        DeviceNodeRemovePendingCloses));

            IopRemoveRelationFromList(Entry->RelationsList,
                                      Entry->DeviceObject);
        }

        PnpUnlinkDeviceRemovalRelations(Entry->DeviceObject,
                                        Entry->RelationsList,
                                        UnlinkRemovedDeviceNodes);
    }

DelayedRemoveWorkerExit:

    if (Entry != NULL) {

        //
        // The final reference on DeviceNodes in the DeviceNodeDeletePendingCloses
        // state is dropped here.
        //

        IopFreeRelationList(Entry->RelationsList);
        ExFreePool(Entry);
    }

    return status;
}

NTSTATUS
PnpDeleteLockedDeviceNode (
    __in  PDEVICE_NODE              DeviceNode,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  ULONG                     Problem,
    __in  NTSTATUS                  ProblemStatus,
    __out_opt PNP_VETO_TYPE         *VetoType,
    __out_opt PUNICODE_STRING       VetoName
    )

/*++

Routine Description:

    This function assumes that the specified device is a bus and will
    recursively remove all its children.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be removed.

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS success;

    PAGED_CODE();

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
              "PnpDeleteLockedDeviceNode: Entered\n    DeviceNode = 0x%p\n    OperationCode = 0x%08X\n    Problem = %d\n    ProblemStatus = 0x%08X\n",
              DeviceNode,
              OperationCode,
              Problem,
              ProblemStatus));

    //
    // Make the device active/idle depending on the operation.  Only in the case
    // of cancel remove is the device made idle.  In all other cases, mark the
    // device active.  Doing this multiple times (i.e. for each remove irp in a
    // transition) is fine as the device will be guaranteed to be unregistered
    // after the removal has completed.
    //

    success = STATUS_SUCCESS;
    switch(OperationCode) {

        case SurpriseRemoveDevice:

            PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
            DeviceNode->StateFlags |= DNSF_POFX_SURPRISE_REMOVE;
            PnpSurpriseRemoveLockedDeviceNode(DeviceNode, Problem, ProblemStatus);
            break;

        case RemoveDevice:


            PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
            DeviceNode->StateFlags |= DNSF_POFX_REMOVE;
            PnpRemoveLockedDeviceNode(DeviceNode, Problem, ProblemStatus);
            break;

        case QueryRemoveDevice:

            PNP_ASSERT(VetoType &&
                       VetoName &&
                       ((PipIsDevNodeDNStarted(DeviceNode) == FALSE) ||
                        ((DeviceNode->StateFlags & DNSF_POFX_QUERY_REMOVE) == 0)));

            PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
            DeviceNode->StateFlags |= DNSF_POFX_QUERY_REMOVE;
            success = PnpQueryRemoveLockedDeviceNode(DeviceNode,
                                                     Problem,
                                                     VetoType,
                                                     VetoName);

            break;

        case CancelRemoveDevice:

            if ((DeviceNode->StateFlags & DNSF_POFX_QUERY_REMOVE) == 0) {
                KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                              PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE,
                              (ULONG_PTR) DeviceNode,
                              DNSF_POFX_QUERY_REMOVE,
                              0);

            }

            PoFxIdleDevice(DeviceNode->PhysicalDeviceObject);
            DeviceNode->StateFlags &= ~DNSF_POFX_QUERY_REMOVE;
            PnpCancelRemoveLockedDeviceNode(DeviceNode);
            break;

        default:
            PNP_ASSERT(0);
            break;
    }

    return success;
}

VOID
PnpSurpriseRemoveLockedDeviceNode(
    __in      PDEVICE_NODE     DeviceNode,
    __in      ULONG            Problem,
    __in      NTSTATUS         ProblemStatus
    )
/*++

Routine Description:

    This function sends a surprise remove IRP to a devnode and processes the
    results.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be surprise removed.

Return Value:

    None.

--*/
{

    PDEVICE_NODE child;
    PDEVICE_OBJECT deviceObject;
    PNP_DEVNODE_STATE devnodeState;
    PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT lockContext = {0};
    PDEVICE_NODE nextChild;
    PNP_DEVNODE_STATE schedulerState;
    NTSTATUS status;
    BOOLEAN surpriseRemoveIrpNeeded;

    PAGED_CODE();

    deviceObject = DeviceNode->PhysicalDeviceObject;

    //
    // There are cases where the scheduling state may no longer be on the stack.
    // This occurs when two removes are queued, a remove failed device and a
    // remove of some ancestor of that device.  Both removes will apply a
    // scheduling state to the stack, but only one will get a chance to clear
    // it.  As such, only restore the previous device node state if a scheduling
    // state exists on the stack.
    //

    schedulerState = DeviceNodeUnspecified;
    if ((DeviceNode->State == DeviceNodeAwaitingQueuedDeletion) ||
        (DeviceNode->State == DeviceNodeAwaitingQueuedRemoval)) {

        //
        // Clear the scheduling state off the state stack.
        //

        schedulerState = DeviceNode->State;
        PipRestoreDevNodeState(DeviceNode);
    }

    devnodeState = DeviceNode->State;

    //
    // Do our state updates.
    //

    PpHotSwapInitRemovalPolicy(DeviceNode);
    if (devnodeState == DeviceNodeRemovePendingCloses) {
        if (schedulerState == DeviceNodeAwaitingQueuedDeletion) {

            //
            // If the state is DeviceNodeRemovePendingCloses, and we got here via
            // DeviceNodeAwaitingQueuedDeletion, we're probably surprise
            // removing a device that was already surprise failed.
            //

            PipClearDevNodeFlags(DeviceNode, DNF_ENUMERATED);
            PipSetDevNodeState(DeviceNode, DeviceNodeDeletePendingCloses, NULL);
            return;

        } else {

#if DBG

            //
            // This must be a dependent. If this dependent is not already
            // deleted, it must have a provider.
            //

            PnpAcquireDependencyRelationsLock(FALSE);
            PNP_ASSERT(PipIsDevNodeDeleted(DeviceNode) != FALSE ||
                       IsListEmpty(PiGetProviderList(deviceObject)) == FALSE);

            PnpReleaseDependencyRelationsLock();

#endif

            //
            // Depenedents which were surprised failed due to their provider
            // being removed may reach here if the removal is pending. They
            // should not have received yet another removal scheduler state
            // while the removal is pending.
            //

            PNP_ASSERT(schedulerState == DeviceNodeUnspecified);
        }
    }

    //
    // Detach any children from the tree here. If they needed SurpriseRemove
    // IRPs, they already will have received them.
    //

    for (child = DeviceNode->Child; child; child = nextChild) {

        //
        // Grab a copy of the next sibling before we blow away this devnode.
        //

        nextChild = child->Sibling;
        if (child->Flags & DNF_ENUMERATED) {
            child->Flags &= ~DNF_ENUMERATED;
        }

        //
        // If the child has resources and we are wiping out the parent, we need
        // to drop the resources (the parent will lose them when his arbiter is
        // removed with the upcoming SurpriseRemoveDevice.)
        //

        if (PipDoesDevNodeHaveResources(child)) {
            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                       "PnpSurpriseRemoveLockedDeviceNode: Releasing resources "
                       "for child device = 0x%p\n",
                       child->PhysicalDeviceObject));

            IopReleaseDeviceResources(child, FALSE);
        }

        //
        // The devnode will be removed from the tree in
        // PnpUnlinkDeviceRemovalRelations. We don't remove it here as we want
        // the tree structure in place for the upcoming broadcast down to user
        // mode.
        //
        // Note - Children in the Uninitialized/Initialized states are not
        //        put directly into DeviceNodeDeleted today. This could be
        //        done but we'd have to verify what happens to API calls in
        //        response to SurpriseRemoval notifications. (Actually, those
        //        API's are blocked in ppcontrol.c, hotplug cannot in fact
        //        walk the tree!)
        //

        PipSetDevNodeState(child, DeviceNodeDeletePendingCloses, NULL);
    }

    if (devnodeState == DeviceNodeStopped) {

        //
        // During rebalance, we had already acquired the device lock
        // to synchronize with mounts. Mark the VPB and release the lock
        // before sending the actual surprise removal.
        //

        if (DeviceNode->Flags & DNF_LOCKED_FOR_REBALANCE) {
            PnpMarkDeviceForRemove(deviceObject, TRUE, &lockContext);
            PnpUnlockMountableDevice(deviceObject);
            PipClearDevNodeFlags(DeviceNode, DNF_LOCKED_FOR_REBALANCE);
        }
    }

    //
    // Send the surprise remove irp...
    //

    switch(DeviceNode->State) {
        case DeviceNodeUninitialized:
        case DeviceNodeInitialized:
            surpriseRemoveIrpNeeded = FALSE;
            break;

        case DeviceNodeRemovePendingCloses:
        case DeviceNodeDeletePendingCloses:
        case DeviceNodeRemoved:
        case DeviceNodeDeleted:
            surpriseRemoveIrpNeeded = FALSE;
            break;

        case DeviceNodeStarted:
        case DeviceNodeStopped:
        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:

            //
            // Expected.
            //

            surpriseRemoveIrpNeeded = TRUE;
            break;

        case DeviceNodeDriversAdded:
        case DeviceNodeResourcesAssigned:
        case DeviceNodeQueryRemoved:
        case DeviceNodeRestartCompletion:
        case DeviceNodeQueryStopped:
        case DeviceNodeEnumeratePending:
        case DeviceNodeStartPending:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeAwaitingQueuedDeletion:
        case DeviceNodeUnspecified:
        default:

            //
            // These could be seen here if an ancestor was surprise removed.
            // While it may not be necessary to send a surprise remove irp,
            // keep the existing behavior as there's no harm in sending the irp
            // as the DDK mentions that any driver should be able to handle
            // a surprise remove after AddDevice is called.
            //

            surpriseRemoveIrpNeeded = TRUE;
            break;
    }

    //
    // Only send a surprise remove if needed.
    //

    if (surpriseRemoveIrpNeeded != FALSE) {
        status = IopRemoveDevice(deviceObject, IRP_MN_SURPRISE_REMOVAL);

        //
        // Disable any device interfaces that may still be enabled for this
        // device after the removal.
        //

        PnpDisableDeviceInterfaces(PnpGetDeviceInstancePath(DeviceNode));

        if (NT_SUCCESS(status)) {

            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                       "PnpSurpriseRemoveLockedDeviceNode: Releasing devices resources\n"));

            IopReleaseDeviceResources(DeviceNode, FALSE);
        }

        if (DeviceNode->Flags & DNF_ENUMERATED) {

            PipSetDevNodeState(DeviceNode, DeviceNodeRemovePendingCloses, NULL);

            //
            // If the device is being converted from a surprise fail to a
            // surprise remove, put the scheduler state back at the top so that
            // it will be converted at the next stage (either remove processing
            // for the current surprise fail or surprise remove processing for
            // the queued surprise remove event).
            //

            if (schedulerState == DeviceNodeAwaitingQueuedDeletion) {
                PipSetDevNodeState(DeviceNode, schedulerState, NULL);
            }

        } else {

            PNP_ASSERT(schedulerState == DeviceNodeAwaitingQueuedDeletion);
            PipSetDevNodeState(DeviceNode, DeviceNodeDeletePendingCloses, NULL);
        }
    }

    PNP_ASSERT(DeviceNode->DockInfo.DockStatus != DOCK_ARRIVING);

    //
    // If a mountable device was surprise removed then this thread took a
    // reference to the FS device (during PnpMarkDeviceForRemove).
    // Dereference it now.
    //

    if (devnodeState == DeviceNodeStopped) {

        if (lockContext.FsDevice != NULL) {
            IopDecrementDeviceObjectHandleCount(lockContext.FsDevice);
            ObDereferenceObject(lockContext.FsDevice);
        }

    }

    //
    // Set the problem code, if one doesn't exist already.
    //

    if (Problem != 0 && PipDoesDevNodeHaveProblem(DeviceNode) == FALSE) {
        PipSetDevNodeProblem(DeviceNode, Problem, ProblemStatus);
    }

    //
    // The device is surprise removed; it is no longer considered hung.
    //

    DeviceNode->StateFlags &= ~DNSF_DEVICE_HUNG_DURING_RESET;

    //
    // Let dependency engine know that this device is surprise removed.
    //

    PnpSurpriseRemovedDeviceNodeDependencyCheck(DeviceNode);
}

NTSTATUS
PnpQueryRemoveLockedDeviceNode (
    __in  PDEVICE_NODE        DeviceNode,
    __in  ULONG               Problem,
    __out PNP_VETO_TYPE      *VetoType,
    __out PUNICODE_STRING     VetoName
    )
/*++

Routine Description:

    This function sends a query remove IRP to a devnode and processes the
    results.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be query removed.

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

Return Value:

    NTSTATUS code.

--*/
{
    PNP_DEVNODE_STATE devnodeState;
    PDEVICE_OBJECT deviceObject;
    NTSTATUS status;
    PDRIVER_OBJECT failingDriver;
    LONG remainingSize;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    devnodeState = DeviceNode->State;

    switch(devnodeState) {
        case DeviceNodeUninitialized:
        case DeviceNodeInitialized:
        case DeviceNodeRemoved:
            //
            // Don't send Queries to devices that haven't been started.
            //
            PNP_ASSERT(DeviceNode->Child == NULL);
            return status;

        case DeviceNodeDriversAdded:
        case DeviceNodeResourcesAssigned:
        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:


            break;

        case DeviceNodeStarted:
            //
            // This guy needs to be queried
            //
            break;

        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeAwaitingQueuedDeletion:
        case DeviceNodeRemovePendingCloses:
        case DeviceNodeStopped:
        case DeviceNodeRestartCompletion:
            //
            // These states should have been culled by PnpProcessRelation
            //
            PNP_ASSERT(0);
            return status;

        case DeviceNodeQueryStopped:
        case DeviceNodeEnumeratePending:
        case DeviceNodeStartPending:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeQueryRemoved:
        case DeviceNodeDeletePendingCloses:
        case DeviceNodeDeleted:
        case DeviceNodeUnspecified:
        default:
            //
            // None of these should be seen here.
            //
            PNP_ASSERT(0);
            return status;
    }

    PNP_ASSERT(PipAreDriversLoaded(DeviceNode));

    deviceObject = DeviceNode->PhysicalDeviceObject;

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
               "PnpQueryRemoveLockedDeviceNode: Sending QueryRemove irp to device = 0x%p\n",
               deviceObject));

    failingDriver = NULL;
    status = PiIrpQueryRemoveDevice(deviceObject, &failingDriver);

    if (NT_SUCCESS(status)) {
        PipSetDevNodeState(DeviceNode, DeviceNodeQueryRemoved, NULL);


    } else {

        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                   "PnpQueryRemoveLockedDeviceNode: QueryRemove vetoed by device = 0x%p, sending CancelRemove\n",
                   deviceObject));

        IopRemoveDevice(deviceObject, IRP_MN_CANCEL_REMOVE_DEVICE);

        *VetoType = PNP_VetoDevice;
        RtlCopyUnicodeString(VetoName, PnpGetDeviceInstancePath(DeviceNode));
        if (failingDriver != NULL) {
            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                       "PnpQueryRemoveLockedDeviceNode: QueryRemove vetoed by driver %wZ\n",
                       &failingDriver->DriverName));

            remainingSize = VetoName->MaximumLength - VetoName->Length;
            if (remainingSize >= (LONG)(failingDriver->DriverName.Length + 2 * sizeof(WCHAR))) {

                RtlAppendUnicodeStringToString(VetoName, &failingDriver->DriverName);
            }
        }
    }

    return status;
}


VOID
PnpCancelRemoveLockedDeviceNode (
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This function sends a cancel remove IRP to a devnode and processes the
    results.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be cancel removed.

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT deviceObject;

    PAGED_CODE();

    if (DeviceNode->State != DeviceNodeQueryRemoved) {

        return;
    }


    deviceObject = DeviceNode->PhysicalDeviceObject;

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
               "PnpCancelRemoveLockedDeviceNode: Sending CancelRemove irp to device = 0x%p\n",
               deviceObject));

    IopRemoveDevice(deviceObject, IRP_MN_CANCEL_REMOVE_DEVICE);

    PipRestoreDevNodeState(DeviceNode);
    PnpStartedDeviceNodeDependencyCheck(DeviceNode);
}


VOID
PnpRemoveLockedDeviceNode (
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Problem,
    __in NTSTATUS ProblemStatus
    )

/*++

Routine Description:

    This function sends a remove IRP to a devnode and processes the
    results.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be removed.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT deviceObject = DeviceNode->PhysicalDeviceObject;
    PDEVICE_OBJECT *attachedDevices, device1, *device2;
    PDRIVER_OBJECT *attachedDrivers, *driver;
    KIRQL irql;
    ULONG length = 0;
    PDEVICE_NODE child, nextChild;
    BOOLEAN removeIrpNeeded;

    PAGED_CODE();

    //
    // Do our state updates.
    //

    PpHotSwapInitRemovalPolicy(DeviceNode);

    //
    // Make sure we WILL drop our references to its children.
    //

    for(child = DeviceNode->Child; child; child = nextChild) {

        //
        // Grab a copy of the next sibling before we blow away this devnode.
        //

        nextChild = child->Sibling;

        if (child->Flags & DNF_ENUMERATED) {
            child->Flags &= ~DNF_ENUMERATED;
        }

        PNP_ASSERT(child->State == DeviceNodeRemoved);
        PNP_ASSERT(!PipAreDriversLoaded(child));

        //
        // If the child has resources and we are wiping out the parent, we need
        // to drop the resources (the parent will lose them when his arbiter is
        // removed with the upcoming RemoveDevice.)
        //

        if (PipDoesDevNodeHaveResources(child)) {
            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                       "PnpRemoveLockedDeviceNode: Releasing resources for child device = 0x%p\n",
                       child->PhysicalDeviceObject));

            IopRemoveDevice(child->PhysicalDeviceObject, IRP_MN_REMOVE_DEVICE);
            IopReleaseDeviceResources(child, FALSE);
        }

        //
        // The devnode will be removed from the tree in
        // PnpUnlinkDeviceRemovalRelations. We don't remove it here as we want
        // the tree structure in place for the upcoming broadcast down to user
        // mode.
        //

        PipSetDevNodeState(child, DeviceNodeDeleted, NULL);
    }

    if (DeviceNode->State == DeviceNodeAwaitingQueuedDeletion ||
        DeviceNode->State == DeviceNodeAwaitingQueuedRemoval) {

        if (DeviceNode->PreviousState == DeviceNodeRemovePendingCloses) {

            //
            // Sending the removal IRP upon completion of the surprise fail
            // path, with the device having just been reported as surprise
            // removed. Let the surprise removal path handle sending the final
            // remove for this device.
            //

            PNP_ASSERT(DeviceNode->State == DeviceNodeAwaitingQueuedDeletion);

            return;

        } else {

            //
            // This happens when pnpevent shortcircuits the surprise failed or
            // surprise remove path upon discovering a nonstarted device.
            //

            PNP_ASSERT(
                (DeviceNode->PreviousState != DeviceNodeStarted) &&
                (DeviceNode->PreviousState != DeviceNodeStopped) &&
                (DeviceNode->PreviousState != DeviceNodeStartPostWork) &&
                (DeviceNode->PreviousState != DeviceNodeRestartCompletion) &&
                (DeviceNode->PreviousState != DeviceNodeDeletePendingCloses));

            PipRestoreDevNodeState(DeviceNode);
        }
    }

    //
    // Do the final remove cleanup on the device...
    //

    switch(DeviceNode->State) {
    case DeviceNodeUninitialized:
    case DeviceNodeInitialized:
    case DeviceNodeRemoved:


        removeIrpNeeded = TRUE;
        break;

    case DeviceNodeDriversAdded:
    case DeviceNodeResourcesAssigned:
    case DeviceNodeStartCompletion:
    case DeviceNodeStartPostWork:
    case DeviceNodeQueryRemoved:
    case DeviceNodeRemovePendingCloses:
    case DeviceNodeDeletePendingCloses:

        //
        // Expected.
        //

        removeIrpNeeded = TRUE;
        break;

    case DeviceNodeDeleted:

        //
        // Already deleted, no need to send a remove again.
        //

        removeIrpNeeded = FALSE;
        break;

    case DeviceNodeStarted:
    case DeviceNodeStopped:
    case DeviceNodeRestartCompletion:
    case DeviceNodeQueryStopped:
    case DeviceNodeEnumeratePending:
    case DeviceNodeStartPending:
    case DeviceNodeEnumerateCompletion:
    case DeviceNodeAwaitingQueuedRemoval:
    case DeviceNodeAwaitingQueuedDeletion:
    case DeviceNodeUnspecified:
    default:

        //
        // None of these should be seen here.
        //

        PNP_ASSERT(0);
        removeIrpNeeded = TRUE;
        break;
    }

    //
    // Add a reference to each FDO attached to the PDO such that the FDOs won't
    // actually go away until the removal operation is completed.
    // Note we need to make a copy of all the attached devices because we won't be
    // able to traverse the attached chain when the removal operation is done.
    //


    device1 = deviceObject->AttachedDevice;
    while (device1) {
        length++;
        device1 = device1->AttachedDevice;
    }

    attachedDevices = NULL;
    attachedDrivers = NULL;
    if (length != 0) {

        length = (length + 2) * sizeof(PDEVICE_OBJECT);

        attachedDevices = (PDEVICE_OBJECT *) ExAllocatePool(NonPagedPoolNx,
                                                            length);

        if (attachedDevices) {

            attachedDrivers = (PDRIVER_OBJECT *) ExAllocatePool(PagedPool, length);
            if (attachedDrivers) {

                RtlZeroMemory(attachedDevices, length);
                RtlZeroMemory(attachedDrivers, length);
                device1 = deviceObject->AttachedDevice;
                device2 = attachedDevices;
                driver = attachedDrivers;

                while (device1) {
                    ObReferenceObject(device1);
                    *device2++ = device1;
                    *driver++ = device1->DriverObject;
                    device1 = device1->AttachedDevice;
                }

            } else {

                ExFreePool(attachedDevices);
                attachedDevices = NULL;
            }
        }
    }

    //
    // Convert DOE_REMOVE_PENDING to DOE_REMOVE_PROCESSED
    //

    irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    device2 = attachedDevices;
    if (device2 != NULL) {
        while (*device2 != NULL) {
            (*device2)->DeviceObjectExtension->ExtensionFlags &=
                ~(DOE_REMOVE_PENDING);

            (*device2)->DeviceObjectExtension->ExtensionFlags |=
                (DOE_REMOVE_PROCESSED);

            device2++;
        }
    }

    deviceObject->DeviceObjectExtension->ExtensionFlags &= ~(DOE_REMOVE_PENDING);
    deviceObject->DeviceObjectExtension->ExtensionFlags |= DOE_REMOVE_PROCESSED;
    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);

    if (removeIrpNeeded) {
        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                   "PnpRemoveLockedDeviceNode: Sending remove irp to device = 0x%p\n",
                   deviceObject));

        IopRemoveDevice(deviceObject, IRP_MN_REMOVE_DEVICE);
        if (DeviceNode->State == DeviceNodeQueryRemoved) {

            //
            // Disable any device interfaces that may still be enabled for this
            // device after the removal.
            //

            PnpDisableDeviceInterfaces(PnpGetDeviceInstancePath(DeviceNode));
        }

        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                   "PnpRemoveLockedDeviceNode: Releasing devices resources\n"));


        IopReleaseDeviceResources(
            DeviceNode,
            (BOOLEAN) ((DeviceNode->Flags & DNF_ENUMERATED) != 0)
            );
    }

    if (!(DeviceNode->Flags & DNF_ENUMERATED)) {

        //
        // If the device is a dock, remove it from the list of dock devices
        // and change the current Hardware Profile, if necessary.
        //

        PNP_ASSERT(DeviceNode->DockInfo.DockStatus != DOCK_ARRIVING) ;
        if ((DeviceNode->DockInfo.DockStatus == DOCK_DEPARTING)||
            (DeviceNode->DockInfo.DockStatus == DOCK_EJECTIRP_COMPLETED)) {

            PpProfileCommitTransitioningDock(DeviceNode, DOCK_DEPARTING);
        }
    }

    //
    // Remove the reference to the attached FDOs to allow them to be actually
    // deleted.
    //

    device2 = attachedDevices;
    if (device2 != NULL) {
        driver = attachedDrivers;
        while (*device2) {
            irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
            (*device2)->DeviceObjectExtension->ExtensionFlags &=
                ~(DOE_REMOVE_PROCESSED);

            (*device2)->DeviceObjectExtension->ExtensionFlags |=
                DOE_START_PENDING;

            KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
            PnpUnloadAttachedDriver(*driver);
            ObDereferenceObject(*device2);
            device2++;
            driver++;
        }
        ExFreePool(attachedDevices);
        ExFreePool(attachedDrivers);
    }

    irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    deviceObject->DeviceObjectExtension->ExtensionFlags &=
        ~(DOE_REMOVE_PROCESSED);

    deviceObject->DeviceObjectExtension->ExtensionFlags |= DOE_START_PENDING;
    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);

    //
    // Now mark this one removed if it's still in the tree.
    //

    if (DeviceNode->Flags & DNF_ENUMERATED) {

        PNP_ASSERT(DeviceNode->Parent);
        PipSetDevNodeState(DeviceNode, DeviceNodeRemoved, NULL);

    } else if (DeviceNode->Parent != NULL) {

        //
        // The devnode will be removed from the tree in
        // PnpUnlinkDeviceRemovalRelations.
        //

        PipSetDevNodeState(DeviceNode, DeviceNodeDeleted, NULL);

    } else {

        PNP_ASSERT(DeviceNode->State == DeviceNodeDeletePendingCloses);
        PipSetDevNodeState(DeviceNode, DeviceNodeDeleted, NULL);
    }

    //
    // Set the problem codes appropriatly. We don't change the problem codes
    // on a devnode unless:
    // a) It disappeared.
    // b) We're disabling it.
    // c) A read-only problem code is overwriting a non-read-only code.
    //

    if (Problem &&
        ((PipDoesDevNodeHaveProblem(DeviceNode) == FALSE) ||
         (Problem == CM_PROB_DEVICE_NOT_THERE) ||
         (Problem == CM_PROB_DISABLED) ||
         ((PipIsProblemReadonly(DeviceNode, Problem) != FALSE) &&
          (PipIsProblemReadonly(DeviceNode, DeviceNode->Problem) == FALSE)))) {

        PipClearDevNodeProblem(DeviceNode);
        PipSetDevNodeProblem(DeviceNode,
                             Problem,
                             (ProblemStatus != STATUS_SUCCESS) ?
                                ProblemStatus : DeviceNode->CompletionStatus);
    }

    //
    // Notify the dependency engine to perform clean-up.
    //

    if ((DeviceNode->Flags & DNF_ENUMERATED) == 0) {
        IoDeleteAllDependencyRelations(deviceObject);
    }

    //
    // Raise PnpRtl event for updated device stack property.
    //

    if (!PnpIsDeviceInstancePathEmpty(DeviceNode)) {
        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STACK);
    }
}

NTSTATUS
PnpDeleteLockedDeviceNodes (
    __in  PDEVICE_OBJECT            DeviceObject,
    __in  PRELATION_LIST            DeviceRelations,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  BOOLEAN                   ProcessDeviceRelations,
    __in  ULONG                     Problem,
    __in  NTSTATUS                  ProblemStatus,
    __out_opt PNP_VETO_TYPE         *VetoType,
    __out_opt PUNICODE_STRING       VetoName
    )
/*++

Routine Description:

    This routine performs requested operation on the DeviceObject and
    the device objects specified in the DeviceRelations.

Arguments:

    DeviceObject - Supplies a pointer to the device object.

    DeviceRelations - supplies a pointer to the device's removal relations.

    OperationCode - Operation code, i.e., QueryRemove, CancelRemove, Remove...

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT relatedDeviceObject;
    DEVICE_RELATION_LEVEL relationLevel;
    ULONG problemCode;
    NTSTATUS problemStatus;
    RELATION_LIST_MARKER marker;
    PDEVICE_NODE parentDeviceNode;
    PDEVICE_OBJECT parentDeviceObject;
    PDEVICE_OBJECT vetoedDevice;

    PAGED_CODE();

    vetoedDevice = NULL;

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                 "PnpDeleteLockedDeviceNodes: Entered\n    DeviceObject = 0x%p\n    DeviceObjectList = 0x%p\n    OperationCode = %d\n",
                  DeviceObject,
                  DeviceRelations,
                  OperationCode));

    if (DeviceRelations->Sorted == FALSE) {
        PNP_ASSERT(FALSE);
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    IopInitEnumerationMarker(ENUMERATE_FORWARD, &marker);
    while (IopEnumerateRelations(DeviceRelations,
                                 &marker,
                                 &relatedDeviceObject,
                                 &relationLevel,
                                 NULL)) {

        //
        // Depending on the operation we need to do different things.
        //
        //  QueryRemoveDevice / CancelRemoveDevice
        //      Process all types.
        //
        //  SurpriseRemoveDevice / RemoveDevice
        //      Process direct descendants and dependencies.
        //

        if (relationLevel != RELATION_LEVEL_REMOVE_EJECT ||
            ProcessDeviceRelations != FALSE) {

            deviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;


            } else if (relatedDeviceObject == DeviceObject) {
                problemCode = Problem;
                problemStatus = ProblemStatus;

            } else if (relationLevel == RELATION_LEVEL_DEPENDENT && OperationCode != CancelRemoveDevice) {
                problemCode = CM_PROB_WAITING_ON_DEPENDENCY;
                problemStatus = STATUS_SUCCESS;

            } else {
                problemCode = 0;
                problemStatus = STATUS_SUCCESS;
            }

            if ((OperationCode == RemoveDevice) &&
                ((deviceNode->StateFlags & DNSF_DEVICE_HUNG_DURING_RESET) != 0)) {

                continue;
            }

            status = PnpDeleteLockedDeviceNode(deviceNode,
                                               OperationCode,
                                               problemCode,
                                               problemStatus,
                                               VetoType,
                                               VetoName);

            if ((OperationCode == QueryRemoveDevice) &&
                (Problem == CM_PROB_DEVICE_RESET) &&
                (status == STATUS_DEVICE_HUNG)) {

                parentDeviceNode = deviceNode->Parent;
                parentDeviceObject = parentDeviceNode->PhysicalDeviceObject;

                //
                // If the device becomes hung we have to mark all the ancestors of the
                // device as hung. Once by marking the parent it will result in recursively
                // marking all the ancestors as hung. This is possible because
                // PnpDeleteLockedDeviceNode returns STATUS_DEVICE_HUNG
                // for any Query Remove if it sees the device is already
                // marked with DNSF_DEVICE_HUNG_DURING_RESET unless the
                // devices fails the Query Remove for some other reason.
                //

                if (IopIsRelationInList(DeviceRelations, parentDeviceObject) != FALSE) {
                    parentDeviceNode->StateFlags |= DNSF_DEVICE_HUNG_DURING_RESET;
                }

                status = STATUS_SUCCESS;

            } else if (!NT_SUCCESS(status)) {

                PNP_ASSERT(OperationCode == QueryRemoveDevice);

                vetoedDevice = relatedDeviceObject;
                if ((deviceNode->StateFlags & DNSF_POFX_QUERY_REMOVE) == 0) {
#pragma prefast(suppress: 28159)
                    KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                                 PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE,
                                 (ULONG_PTR) deviceNode,
                                 DNSF_POFX_QUERY_REMOVE,
                                 0);
                }

                PoFxIdleDevice(deviceNode->PhysicalDeviceObject);
                deviceNode->StateFlags &= ~DNSF_POFX_QUERY_REMOVE;

                //
                // Iterate backwards and send CancelRemove to previous
                // devices, not including the current device that vetoed
                // the request.
                //

                IopReverseEnumerationOrder(DeviceRelations, &marker);
                while (IopEnumerateRelations(DeviceRelations,
                                             &marker,
                                             &relatedDeviceObject,
                                             NULL,
                                             NULL)) {

                    //
                    // Skip the device that vetoed the request, since cancel
                    // remove has been already sent.
                    //

                    if (vetoedDevice == relatedDeviceObject) {
                        continue;
                    }

                    deviceNode = PP_DO_TO_DN(relatedDeviceObject);
                    PnpDeleteLockedDeviceNode(deviceNode,
                                              CancelRemoveDevice,
                                              0,
                                              STATUS_SUCCESS,
                                              VetoType,
                                              VetoName);
                }

                status = STATUS_UNSUCCESSFUL;
                goto exit;
            }
        }
    }

exit:
    return status;
}

NTSTATUS
PnpCancelRemoveOnHungDevices (
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_ BOOLEAN ExcludeDeviceRelations,
    _In_ PRELATION_LIST RelationList,
    _Out_ PWSTR *RelatedDeviceInstances
    )

/*++

Routine Description:

    This routine sends cancel remove IRPs, kernel mode notifications and
    user mode notifications to hung devices and their ancestors.

    This is required because hung devices have "failed" query remove IRPs
    but PNP chose to ignore them at the time. However, this routine should
    be called to revert the state of these hung devices back to normal as if
    a query remove was vetoed.
    
Arguments:

    DeviceEvent - Supplies a pointer to the device event.

    DeleteType - Supplies the delete operation; only Eject and QueryRemove are
        allowed.

    ExcludeDeviceRelations - Supplies whether device relations should be
        included in the list

    RelationList - Supplies a pointer to the devices being removed.

    RelatedDeviceInstances - Supplies a pointer to receive the string of device
        instance IDs for user mode notification. This will not contain IDs of
        devices that are hung.

Return Value:

    None.

--*/

{

    PDEVICE_NODE DeviceNode;
    PWSTR HungDevices;
    PRELATION_LIST HungDevicesList;
    RELATION_LIST_MARKER Marker;
    PDEVICE_OBJECT Pdo;
    DEVICE_RELATION_LEVEL RelationLevel;
    NTSTATUS Status;

    PAGED_CODE();

    NT_ASSERT(DeviceEvent != NULL);
    NT_ASSERT(RelationList != NULL);

    HungDevices = NULL;
    if ((DeleteType != EjectDevice) && (DeleteType != QueryRemoveDevice)) {
        RtlFailFast(FAST_FAIL_INVALID_ARG);
    }

    //
    // Re-calculate the device list for user mode notifications
    // to prevent further notifications being sent out to hung devices.
    //

    Status = PnpCompileDeviceInstancePaths(DeleteType,
                                           RelationList,
                                           ExcludeDeviceRelations,
                                           TRUE, // exclude hung devices
                                           RelatedDeviceInstances);
    
    if (!NT_SUCCESS(Status)) {
        goto CancelRemoveOnHungDevicesEnd;
    }

    //
    // Construct a new list composed of hung devices and their ancestors.
    //

    HungDevicesList = IopAllocateRelationList(DeleteType);
    if (HungDevicesList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CancelRemoveOnHungDevicesEnd;
    }

    IopInitEnumerationMarker(ENUMERATE_BACKWARD, &Marker);
    while (IopEnumerateRelations(RelationList,
                                 &Marker,
                                 &Pdo,
                                 &RelationLevel,
                                 NULL)) {

        DeviceNode = PP_DO_TO_DN(Pdo);
        if (DeviceNode == NULL) {
            continue;
        }

        if ((DeviceNode->StateFlags & DNSF_DEVICE_HUNG_DURING_RESET) == 0) {
            continue;
        }

        Status = IopAddRelationToList(HungDevicesList,
                                      Pdo,
                                      RelationLevel,
                                      DEVICE_UNTAGGED);

        if (!NT_SUCCESS(Status)) {
            goto CancelRemoveOnHungDevicesEnd;
        }
    }

    HungDevicesList->Sorted = TRUE;

    //
    // Iterate backwards and send cancel remove IRPs to hung devices.
    //

    IopInitEnumerationMarker(ENUMERATE_FORWARD, &Marker);
    while (IopEnumerateRelations(HungDevicesList, &Marker, &Pdo, NULL, NULL)) {
        DeviceNode = PP_DO_TO_DN(Pdo);
        if (DeviceNode->State == DeviceNodeQueryRemoved) {
            PnpDeleteLockedDeviceNode(DeviceNode,
                                      CancelRemoveDevice,
                                      0,
                                      STATUS_SUCCESS,
                                      NULL,
                                      NULL);
        }
    }

    //
    // Notify kernel mode listeners.
    //

    IopInitEnumerationMarker(ENUMERATE_FORWARD, &Marker);
    while (IopEnumerateRelations(HungDevicesList, &Marker, &Pdo, NULL, NULL)) {
        DeviceNode = PP_DO_TO_DN(Pdo);

        //
        // No need to send cancel query remove notification
        // to devices which are already removed.
        //

        if ((DeviceNode->State == DeviceNodeRemoved) ||
            (DeviceNode->State == DeviceNodeInitialized) ||
            (DeviceNode->State == DeviceNodeUninitialized)) {

            continue;
        }

        PnpNotifyTargetDeviceChange(&GUID_TARGET_DEVICE_REMOVE_CANCELLED,
                                    Pdo,
                                    NULL,
                                    NULL);
    }

    //
    // Notify user mode listeners.
    //
    
    Status = PnpCompileDeviceInstancePaths(DeleteType,
                                           HungDevicesList,
                                           FALSE,
                                           FALSE,
                                           &HungDevices);

    if (!NT_SUCCESS(Status)) {
        goto CancelRemoveOnHungDevicesEnd;
    }

    Status = PnpNotifyUserModeDeviceRemoval(DeviceEvent,
                                            HungDevices,
                                            &GUID_TARGET_DEVICE_REMOVE_CANCELLED,
                                            NULL,
                                            NULL);

CancelRemoveOnHungDevicesEnd:
    if (HungDevices != NULL) {
        ExFreePoolWithTag(HungDevices, PNP_POOLTAG_DEVICE_EVENT_ENTRY);
    }

    return Status;
}

NTSTATUS
PnpBuildRemovalRelationList (
    __in  PDEVICE_OBJECT            DeviceObject,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __out PNP_VETO_TYPE             *VetoType,
    __out PUNICODE_STRING           VetoName,
    __out PRELATION_LIST            *RelationsList
    )
/*++

Routine Description:

    This routine locks the device subtrees for removal operation and returns
    a list of device objects which need to be removed with the specified
    DeviceObject.

    Caller must hold a reference to the DeviceObject.

Arguments:

    DeviceObject - Supplies a pointer to the device object to be removed.

    OperationCode - Operation code, i.e., QueryEject, CancelEject, Eject...

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

    RelationList - supplies a pointer to a variable to receive the device's
                   relations.

Return Value:

    NTSTATUS code.

--*/

{

    PDEVICE_NODE deviceNode;
    PRELATION_LIST newRelationsList;
    NTSTATUS status;

    PAGED_CODE();

    *RelationsList = NULL;

    deviceNode = (PDEVICE_NODE) DeviceObject->DeviceObjectExtension->DeviceNode;

    //
    // Obviously no one should try to delete the whole device node tree.
    //
    PNP_ASSERT(DeviceObject != IopRootDeviceNode->PhysicalDeviceObject);

    if ((newRelationsList = IopAllocateRelationList(OperationCode)) == NULL) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // First process the object itself
    //
    status = PnpProcessRelation(deviceNode,
                                OperationCode,
                                RELATION_LEVEL_DIRECT_DESCENDANT,
                                VetoType,
                                VetoName,
                                newRelationsList);

    PNP_ASSERT(status != STATUS_INVALID_DEVICE_REQUEST);

    if (NT_SUCCESS(status)) {
        *RelationsList = newRelationsList;

        //
        // At this point we have a list of all the relations.
        //
        // Relations which were merged from an existing eject have the tagged
        // bit set.
        //
        // All of the relations and their parents are locked.
        //
        // There is a reference on each device object by virtue of it being in
        // the list.  There is another one on each device object because it is
        // locked and the lock count is >= 1.
        //
        // There is also a reference on each relation's parent and it's lock
        // count is >= 1.
        //

        //
        // Sort the relation list.
        //

        IopSortRelationListForRemove(newRelationsList);

    } else {

        IopFreeRelationList(newRelationsList);
    }

    return status;
}

NTSTATUS
PnpProcessBusRelations (
    __in      PDEVICE_NODE              DeviceNode,
    __in      PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in      DEVICE_RELATION_LEVEL     RelationLevel,
    __out     PNP_VETO_TYPE             *VetoType,
    __out     PUNICODE_STRING           VetoName,
    __inout   PRELATION_LIST            RelationsList
    )
/*++

Routine Description:

    This routine processes the BusRelations for the specified devnode.
    Caller must hold the device tree lock.

Arguments:

    DeviceNode - Supplies a pointer to the device object to be collected.

    OperationCode - Operation code, i.e., QueryRemove, QueryEject, ...

    RelationLevel - Indicates the level of relationship with the target device.

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

    RelationList - supplies a pointer to a variable to receive the device's
                   removal relations.

Return Value:

    NTSTATUS code.

--*/
{
    PDEVICE_NODE child;
    NTSTATUS status;

    PAGED_CODE();

    for(child = DeviceNode->Child;
        child != NULL;
        child = child->Sibling) {

        status = PnpProcessRelation(child,
                                    OperationCode,
                                    RelationLevel,
                                    VetoType,
                                    VetoName,
                                    RelationsList);

        PNP_ASSERT(status == STATUS_SUCCESS || status == STATUS_UNSUCCESSFUL);

        if (!NT_SUCCESS(status)) {

            return status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PnpProcessDependencyRelations (
    __in      PDEVICE_NODE              DeviceNode,
    __in      PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in      DEVICE_RELATION_LEVEL     RelationLevel,
    __out     PNP_VETO_TYPE             *VetoType,
    __out     PUNICODE_STRING           VetoName,
    __inout   PRELATION_LIST            RelationsList
    )
/*++

Routine Description:

    This routine processes the dependency relations for the specified devnode.
    Caller must hold the device tree lock.

Arguments:

    DeviceNode - Supplies a pointer to the device object to be collected.

    OperationCode - Operation code, i.e., QueryRemove, QueryEject, ...

    RelationLevel - Indicates the level of relationship with target device.

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

    RelationList - supplies a pointer to a variable to receive the device's
                   removal relations.

Return Value:

    NTSTATUS code.

--*/
{

    PDEVICE_OBJECT currentDependentDevice;
    PDEVICE_NODE currentDependentDeviceNode;
    PLIST_ENTRY currentDependentLink;
    ULONG dependencyTypes;
    PLIST_ENTRY dependentList;
    PDEVICE_OBJECT providerPdo;
    BOOLEAN processRelation;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    providerPdo = DeviceNode->PhysicalDeviceObject;
    PnpAcquireDependencyRelationsLock(FALSE);
    dependentList = PiGetDependentList(providerPdo);
    currentDependentLink = dependentList->Flink;
    while (currentDependentLink != dependentList) {
        PiEnumerateDependentListEntry(currentDependentLink,
                                      &currentDependentDevice,
                                      &dependencyTypes);

        PNP_ASSERT((dependencyTypes &
                    ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                      PNP_DEPENDENCY_TYPE_START)) == 0);

        currentDependentLink = currentDependentLink->Flink;
        if (currentDependentDevice == NULL) {

            //
            // The dependent device has not been resolved yet.
            //

            continue;
        }

        currentDependentDeviceNode = PP_DO_TO_DN(currentDependentDevice);
        PNP_ASSERT(currentDependentDeviceNode != NULL);
        switch(currentDependentDeviceNode->State) {
            case DeviceNodeAwaitingQueuedRemoval:
            case DeviceNodeAwaitingQueuedDeletion:

                if (currentDependentDeviceNode->PreviousState == DeviceNodeRemoved ||
                    currentDependentDeviceNode->PreviousState == DeviceNodeDeleted) {

                    //
                    // Ignore dependents that have been already removed or deleted
                    // by another removal event.
                    //

                    processRelation = FALSE;

                } else {
                    processRelation = TRUE;
                }

                break;

            case DeviceNodeRemovePendingCloses:
            case DeviceNodeDeletePendingCloses:

                //
                // Include these so that the relation lists can be merged.
                //

                processRelation = TRUE;

            default:

                //
                // For all other cases, include the dependent in the
                // relation list if it is started.
                //

                processRelation = (BOOLEAN) PipIsDevNodeDNStarted(currentDependentDeviceNode);
                break;
        }

        if (processRelation != FALSE) {
            status = PnpProcessRelation(currentDependentDeviceNode,
                                        OperationCode,
                                        RelationLevel,
                                        VetoType,
                                        VetoName,
                                        RelationsList);

        }

        PNP_ASSERT(status == STATUS_SUCCESS ||
                    status == STATUS_UNSUCCESSFUL);

        if (!NT_SUCCESS(status)) {
            goto exit;
        }
    }

exit:
    PnpReleaseDependencyRelationsLock();
    return status;
}

NTSTATUS
PnpProcessRelation (
    __in     PDEVICE_NODE           DeviceNode,
    __in     PNP_DEVICE_DELETE_TYPE OperationCode,
    __in     DEVICE_RELATION_LEVEL  RelationLevel,
    __out    PNP_VETO_TYPE          *VetoType,
    __out    PUNICODE_STRING        VetoName,
    __inout  PRELATION_LIST         RelationsList
    )
/*++

Routine Description:

    This routine builds the list of device objects that need to be removed or
    examined when the passed in device object is torn down.

    Caller must hold the device tree lock.

Arguments:

    DeviceNode - Supplies a pointer to the device object to be collected.

    OperationCode - Operation code, i.e., QueryRemove, QueryEject, ...

    RelationLevel - Indicates the level of relationship with target device.

    VetoType - Pointer to address that receives the veto type if the operation
               failed.

    VetoName - Pointer to a unicode string that will receive data appropriate
               to the veto type.

    RelationList - supplies a pointer to a variable to receive the device's
                   removal relations.


Return Value:

    NTSTATUS code.

--*/
{

    PDEVICE_NODE                    relatedDeviceNode;
    PDEVICE_OBJECT                  relatedDeviceObject;
    PDEVICE_RELATIONS               deviceRelations;
    PLIST_ENTRY                     ejectLink;
    PPENDING_RELATIONS_LIST_ENTRY   ejectEntry;
    NTSTATUS                        status;
    ULONG                           i;
    PNP_DEVNODE_STATE               devnodeState;
    NTSTATUS                        mergeStatus;
    DEVICE_RELATION_LEVEL           newRelationLevel;

    PAGED_CODE();

    if (OperationCode == QueryRemoveDevice || OperationCode == EjectDevice) {

        if (DeviceNode->State == DeviceNodeDeleted) {

            //
            // The device has already been removed, fail the attempt.
            //
            return STATUS_UNSUCCESSFUL;
        }

        if ((DeviceNode->State == DeviceNodeAwaitingQueuedRemoval) ||
            (DeviceNode->State == DeviceNodeAwaitingQueuedDeletion)) {

            //
            // The device has failed or is going away.  Let the queued
            // remove deal with it.
            //
            return STATUS_UNSUCCESSFUL;
        }

        if (DeviceNode->DeletedChildren > 0) {

            //
            // The device has removed children and dependents that are still
            // waiting for their final remove IRP. The vetoing device returned
            // may not have any handles opened directly on it, but to get the
            // actual device would require walking through the
            // IopPendingSurpriseRemovals list, which would give a more
            // accurate vetoing device and may be done in the future.
            //

            *VetoType = PNP_VetoOutstandingOpen;
            RtlCopyUnicodeString(VetoName, PnpGetDeviceInstancePath(DeviceNode));
            return STATUS_UNSUCCESSFUL;
        }

        if ((DeviceNode->State == DeviceNodeRemovePendingCloses) ||
            (DeviceNode->State == DeviceNodeDeletePendingCloses)) {

            //
            // The device is in the process of being surprise removed, let it finish
            //
            *VetoType = PNP_VetoOutstandingOpen;
            RtlCopyUnicodeString(VetoName, PnpGetDeviceInstancePath(DeviceNode));
            return STATUS_UNSUCCESSFUL;
        }

        if ((DeviceNode->State == DeviceNodeStopped) ||
            (DeviceNode->State == DeviceNodeRestartCompletion)) {

            //
            // We are recovering from a rebalance. This should never happen and
            // this return code will cause us to PNP_ASSERT.
            //
            return STATUS_INVALID_DEVICE_REQUEST;
        }

    } else if (DeviceNode->State == DeviceNodeDeleted) {

        //
        // The device has already been removed, ignore it. We should only have
        // seen such a thing if it got handed to us in a Removal or Ejection
        // relations or dependencies.
        //

        PNP_ASSERT(RelationLevel != RELATION_LEVEL_DIRECT_DESCENDANT);
        return STATUS_SUCCESS;
    }

    status = IopAddRelationToList(RelationsList,
                                  DeviceNode->PhysicalDeviceObject,
                                  RelationLevel,
                                  DEVICE_UNTAGGED);

    if (status == STATUS_SUCCESS) {

        if (!(DeviceNode->Flags & DNF_LOCKED_FOR_EJECT)) {

            //
            // Then process the bus relations
            //

            status = PnpProcessBusRelations(DeviceNode,
                                            OperationCode,
                                            RelationLevel,
                                            VetoType,
                                            VetoName,
                                            RelationsList);
            if (!NT_SUCCESS(status)) {
                return status;
            }

            //
            // Retrieve the state of the devnode when it failed.
            //

            devnodeState = DeviceNode->State;
            if ((devnodeState == DeviceNodeAwaitingQueuedRemoval) ||
                (devnodeState == DeviceNodeAwaitingQueuedDeletion)) {

                devnodeState = DeviceNode->PreviousState;
            }

            //
            // Next the removal relations
            //
            if ((devnodeState == DeviceNodeStarted) ||
                (devnodeState == DeviceNodeStopped) ||
                (devnodeState == DeviceNodeStartPostWork) ||
                (devnodeState == DeviceNodeRestartCompletion)) {

                status = PnpQueryDeviceRelations(DeviceNode->PhysicalDeviceObject,
                                                 RemovalRelations,
                                                 NULL,
                                                 &deviceRelations);

                if (NT_SUCCESS(status) && deviceRelations) {

                    for (i = 0; i < deviceRelations->Count; i++) {

                        relatedDeviceObject = deviceRelations->Objects[i];
                        if (relatedDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_DELETE_PENDING) {
                            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(relatedDeviceObject);
                            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                                          PNP_ERR_DELETED_PDO_REPORTED_AS_RELATION,
                                          (ULONG_PTR)relatedDeviceObject,
                                          RemovalRelations,
                                          0);
                        }

                        relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
                        PNP_ASSERT(relatedDeviceNode);
                        if ((relatedDeviceNode == NULL) || (relatedDeviceNode->Parent == NULL)) {
                            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(relatedDeviceObject);
                            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                                          PNP_ERR_DELETED_PDO_REPORTED_AS_RELATION,
                                          (ULONG_PTR)relatedDeviceObject,
                                          RemovalRelations,
                                          0);
                        }

                        if (relatedDeviceNode) {

                            status = PnpProcessRelation(relatedDeviceNode,
                                                        OperationCode,
                                                        RELATION_LEVEL_REMOVE_EJECT,
                                                        VetoType,
                                                        VetoName,
                                                        RelationsList);
                        }

                        ObDereferenceObject( relatedDeviceObject );

                        PNP_ASSERT(status == STATUS_SUCCESS ||
                               status == STATUS_UNSUCCESSFUL);

                        if (!NT_SUCCESS(status)) {

                            ExFreePool(deviceRelations);

                            return status;
                        }
                    }

                    ExFreePool(deviceRelations);
                } else {
                    if (status != STATUS_NOT_SUPPORTED) {
                        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                                   "PnpProcessRelation: PnpQueryDeviceRelations failed, DeviceObject = 0x%p, status = 0x%08X\n",
                                   DeviceNode->PhysicalDeviceObject, status));
                    }
                }
            }

            //
            // Finally the eject relations if we are doing an eject operation
            //
            if (OperationCode != QueryRemoveDevice &&
                OperationCode != RemoveFailedDevice &&
                OperationCode != RemoveUnstartedFailedDevice) {
                status = PnpQueryDeviceRelations(DeviceNode->PhysicalDeviceObject,
                                                 EjectionRelations,
                                                 NULL,
                                                 &deviceRelations);

                if (NT_SUCCESS(status) && deviceRelations) {

                    for (i = 0; i < deviceRelations->Count; i++) {

                        relatedDeviceObject = deviceRelations->Objects[i];
                        if (relatedDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_DELETE_PENDING) {
                            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(relatedDeviceObject);
                            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                                          PNP_ERR_DELETED_PDO_REPORTED_AS_RELATION,
                                          (ULONG_PTR)relatedDeviceObject,
                                          EjectionRelations,
                                          0);
                        }

                        relatedDeviceNode = (PDEVICE_NODE)relatedDeviceObject->DeviceObjectExtension->DeviceNode;
                        PNP_ASSERT(relatedDeviceNode);
                        if ((relatedDeviceNode == NULL) || (relatedDeviceNode->Parent == NULL)) {
                            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(relatedDeviceObject);
                            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                                          PNP_ERR_DELETED_PDO_REPORTED_AS_RELATION,
                                          (ULONG_PTR)relatedDeviceObject,
                                          EjectionRelations,
                                          0);
                        }

                        if (relatedDeviceNode) {

                            status = PnpProcessRelation(relatedDeviceNode,
                                                        OperationCode,
                                                        RELATION_LEVEL_REMOVE_EJECT,
                                                        VetoType,
                                                        VetoName,
                                                        RelationsList);
                        }

                        ObDereferenceObject( relatedDeviceObject );

                        PNP_ASSERT(status == STATUS_SUCCESS ||
                               status == STATUS_UNSUCCESSFUL);

                        if (!NT_SUCCESS(status)) {

                            ExFreePool(deviceRelations);

                            return status;
                        }
                    }

                    ExFreePool(deviceRelations);
                } else {
                    if (status != STATUS_NOT_SUPPORTED) {
                        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                                   "PnpProcessRelation: PnpQueryDeviceRelations failed, DeviceObject = 0x%p, status = 0x%08X\n",
                                   DeviceNode->PhysicalDeviceObject,
                                   status));
                    }
                }
            }

            //
            // One more left to do.  The dependency relations.
            //

            newRelationLevel = (RelationLevel == RELATION_LEVEL_REMOVE_EJECT) ?
                RELATION_LEVEL_REMOVE_EJECT : RELATION_LEVEL_DEPENDENT;

            status = PnpProcessDependencyRelations(DeviceNode,
                                                   OperationCode,
                                                   newRelationLevel,
                                                   VetoType,
                                                   VetoName,
                                                   RelationsList);

            if (!NT_SUCCESS(status)) {
                return status;
            }

            status = STATUS_SUCCESS;

        } else {

            //
            // Look to see if this device is already part of a pending ejection.
            // If it is and we are doing an ejection then we will subsume it
            // within the larger ejection.  If we aren't doing an ejection then
            // we better be processing the removal of one of the ejected devices.
            //
            for(ejectLink = IopPendingEjects.Flink;
                ejectLink != &IopPendingEjects;
                ejectLink = ejectLink->Flink) {

                ejectEntry = CONTAINING_RECORD( ejectLink,
                                                PENDING_RELATIONS_LIST_ENTRY,
                                                Link);

                if (ejectEntry->RelationsList != NULL &&
                    IopIsRelationInList(ejectEntry->RelationsList, DeviceNode->PhysicalDeviceObject)) {
                    if (OperationCode == EjectDevice) {
                        status = IopRemoveRelationFromList(
                            RelationsList,
                            DeviceNode->PhysicalDeviceObject);

                        PNP_ASSERT(NT_SUCCESS(status));

                        IopCancelPendingEject(ejectEntry);

                        //
                        // If a parent fails eject and it has a child that is
                        // infinitely pending an eject, this means the child now
                        // wakes up. One suggestion brought up that does not involve
                        // a code change is to amend the WDM spec to say if driver
                        // gets a start IRP for a device pending eject, it should
                        // cancel the eject IRP automatically.
                        //

                        mergeStatus = IopMergeRelationLists(
                                          RelationsList,
                                          ejectEntry->RelationsList,
                                          FALSE);

                        PNP_ASSERT(NT_SUCCESS(mergeStatus));

                        if (RelationLevel == RELATION_LEVEL_DIRECT_DESCENDANT) {
                            //
                            // If RelationLevel is DirectDescendant then we need to
                            // get that bit set on the relation that caused us to
                            // do the merge.  IopAddRelationToList will fail with
                            // STATUS_OBJECT_NAME_COLLISION but the bit will still
                            // be set as a side effect.
                            //
                            IopAddRelationToList(RelationsList,
                                                 DeviceNode->PhysicalDeviceObject,
                                                 RELATION_LEVEL_DIRECT_DESCENDANT,
                                                 DEVICE_UNTAGGED);
                        }
                    } else if (OperationCode != QueryRemoveDevice) {

                        //
                        // Either the device itself disappeared or an ancestor
                        // of this device failed in some way. In both cases this
                        // happened before we completed the eject IRP. We'll
                        // remove it from the list in the pending ejection and
                        // return it.
                        //

                        status = IopRemoveRelationFromList( ejectEntry->RelationsList,
                                                            DeviceNode->PhysicalDeviceObject);

                        PipClearDevNodeFlags(DeviceNode, DNF_LOCKED_FOR_EJECT);

                        PNP_ASSERT(NT_SUCCESS(status));

                    } else {

                        //
                        // Someone is trying to take offline a supertree of this
                        // device which happens to be prepared for ejection.
                        // Whistler like Win2K won't let this happen (doing so
                        // isn't too hard, it involves writing code to cancel
                        // the outstanding eject and free the relation list.)
                        //

                        return STATUS_UNSUCCESSFUL;
                    }
                    break;
                }
            }

            PNP_ASSERT(ejectLink != &IopPendingEjects);

            if (ejectLink == &IopPendingEjects) {

                PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(DeviceNode->PhysicalDeviceObject);
                KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                              PNP_ERR_DEVICE_MISSING_FROM_EJECT_LIST,
                              (ULONG_PTR)DeviceNode->PhysicalDeviceObject,
                              0,
                              0);
            }
        }
    } else if (status == STATUS_OBJECT_NAME_COLLISION) {

        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                   "PnpProcessRelation: Duplicate relation, DeviceObject = 0x%p\n",
                   DeviceNode->PhysicalDeviceObject));

        status = PnpProcessBusRelations(DeviceNode,
                                        OperationCode,
                                        RelationLevel,
                                        VetoType,
                                        VetoName,
                                        RelationsList);

    } else if (status != STATUS_INSUFFICIENT_RESOURCES) {

        PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(DeviceNode->PhysicalDeviceObject);
        KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                      PNP_ERR_UNEXPECTED_ADD_RELATION_ERR,
                      (ULONG_PTR)DeviceNode->PhysicalDeviceObject,
                      (ULONG_PTR)RelationsList,
                      status);
    }

    return status;
}

BOOLEAN
PnpQueuePendingEject (
    __in PPENDING_RELATIONS_LIST_ENTRY Entry
    )
{
    PAGED_CODE();

    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    InsertTailList(&IopPendingEjects, &Entry->Link);

    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

    return TRUE;
}

NTSTATUS
PnpInvalidateRelationsInList (
    __in  PRELATION_LIST            RelationsList,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  BOOLEAN                   OnlyDeviceRelations,
    __in  BOOLEAN                   RestartDevNode
    )
/*++

Routine Description:

    Iterate over the relations in the list creating a second list containing the
    parent of each entry skipping parents which are also in the list.  In other
    words, if the list contains node P and node C where node C is a child of node
    P then the parent of node P would be added but not node P itself.


Arguments:

    RelationsList           - List of relations

    OperationCode           - Type of operation the invalidation is associated
                              with.

    OnlyDeviceRelations     - Indirect relations are those which aren't direct
                              descendants (bus relations) of the PDO originally
                              targetted for the operation or its direct
                              descendants.  This would include Removal or
                              Eject relations.

    RestartDevNode          - If true then any node who's parent was invalidated
                              is restarted.  This flag requires that all the
                              relations in the list have been previously
                              sent a remove IRP.


Return Value:

    NTSTATUS code.

--*/

{

    PRELATION_LIST                  parentsList;
    PDEVICE_OBJECT                  deviceObject, parentObject;
    PDEVICE_NODE                    deviceNode;
    RELATION_LIST_MARKER            marker;
    DEVICE_TAGGED_FOR_REMOVAL       tagged;
    DEVICE_RELATION_LEVEL           relationLevel;

    PAGED_CODE();

    parentsList = IopAllocateRelationList(OperationCode);

    if (parentsList == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IopSetAllRelationsTags( RelationsList, FALSE );

    //
    // Traverse the list creating a new list with the topmost parents of
    // each sublist contained in RelationsList.
    //

    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &marker);
    while (IopEnumerateRelations(RelationsList,
                                 &marker,
                                 &deviceObject,
                                 &relationLevel,
                                 &tagged)) {

        if (!OnlyDeviceRelations || relationLevel == RELATION_LEVEL_REMOVE_EJECT) {

            if (tagged == DEVICE_UNTAGGED) {

                parentObject = deviceObject;

                while (IopSetRelationsTag( RelationsList, parentObject, DEVICE_TAGGED ) == STATUS_SUCCESS) {

                    deviceNode = (PDEVICE_NODE) parentObject->DeviceObjectExtension->DeviceNode;

                    if (RestartDevNode)  {

                        PipClearDevNodeFlags(deviceNode, DNF_LOCKED_FOR_EJECT);
                        //
                        // Bring the devnode back online if it:
                        // a) It is still physically present
                        // b) It was held for an eject
                        //
                        if (deviceNode->Flags & DNF_ENUMERATED) {

                            PNP_ASSERT(deviceNode->Child == NULL);
                            PNP_ASSERT(!PipAreDriversLoaded(deviceNode));

                            if (PipIsDevNodeProblem(deviceNode, CM_PROB_HELD_FOR_EJECT)) {
                                //
                                // This operation is a reorder barrier. This keeps
                                // our subsequent enumeration from draining prior
                                // to our problem clearing.
                                //
                                PnpRequestDeviceAction(parentObject,
                                                        ClearEjectProblem,
                                                        TRUE,
                                                        0,
                                                        NULL,
                                                        NULL);

                            } else if (OperationCode == EjectDevice) {

                                PnpRequestDeviceAction(parentObject,
                                                       ResetDevice,
                                                       TRUE,
                                                       0,
                                                       NULL,
                                                       NULL);
                            }
                        }
                    }

                    if (deviceNode->Parent != NULL) {

                        parentObject = deviceNode->Parent->PhysicalDeviceObject;

                    } else {
                        parentObject = NULL;
                        break;
                    }
                }

                if (parentObject != NULL)  {
                    IopAddRelationToList(parentsList,
                                         parentObject,
                                         RELATION_LEVEL_DIRECT_DESCENDANT,
                                         DEVICE_UNTAGGED);
                }
            }

        }
    }

    //
    // Reenumerate each of the parents
    //

    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &marker);
    while (IopEnumerateRelations(parentsList,
                                 &marker,
                                 &deviceObject,
                                 NULL,
                                 NULL)) {

        PnpRequestDeviceAction(deviceObject,
                               ReenumerateDeviceTree,
                               FALSE,
                               0,
                               NULL,
                               NULL);
    }

    //
    // Free the parents list
    //

    IopFreeRelationList( parentsList );

    return STATUS_SUCCESS;
}

_IRQL_requires_same_
_Function_class_(WORKER_THREAD_ROUTINE)
VOID
PnpProcessCompletedEject (
    _In_ PVOID Context
    )
/*++

Routine Description:

    This routine is called at passive level from a worker thread that was queued
    either when an eject IRP completed (see io\pnpirp.c - IopDeviceEjectComplete
    or io\pnpirp.c - IopEjectDevice), or when a warm eject needs to be performed.
    We also may need to fire off any enumerations of parents of ejected devices
    to verify they have indeed left.

Arguments:

    Context - Pointer to the pending relations list which contains the device
              to eject (warm) and the list of parents to reenumerate.

Return Value:

    None.

--*/

{

    PDEVICE_NODE deviceNode;
    PPENDING_RELATIONS_LIST_ENTRY entry = (PPENDING_RELATIONS_LIST_ENTRY)Context;
    NTSTATUS status = STATUS_SUCCESS;


    PAGED_CODE();

    if ((entry->LightestSleepState != PowerSystemWorking) &&
        (entry->LightestSleepState != PowerSystemUnspecified)) {

        //
        // For docks, WinLogon gets to do the honors. For other devices, the
        // user must infer when it's safe to remove the device (if we've powered
        // up, it may not be safe now!)
        //
        entry->DisplaySafeRemovalDialog = FALSE;

        //
        // This is a warm eject request, initiate it here.
        //
        status = IopWarmEjectDevice(entry->DeviceObject, entry->LightestSleepState);

        //
        // We're back and we either succeeded or failed. Either way...
        //
    }

    if (entry->DockInterface) {

        entry->DockInterface->ProfileDepartureSetMode(
            entry->DockInterface->Context,
            PDS_UPDATE_DEFAULT
            );

        entry->DockInterface->InterfaceDereference(
            entry->DockInterface->Context
            );
    }

    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    RemoveEntryList( &entry->Link );

    //
    // Check if the RelationsList pointer in the context structure is NULL.  If
    // so, this means we were cancelled because this eject is part of a new
    // larger eject.  In that case all we want to do is unlink and free the
    // context structure.
    //

    //
    // Two interesting points about such code.
    //
    // 1) If you wait forever to complete an eject of a dock, we *wait* forever
    //    in the Query profile change state. No sneaky adding another dock. You
    //    must finish what you started...
    // 2) Let's say you are ejecting a dock, and it is taking a long time. If
    //    you try to eject the parent, that eject will *not* grab this lower
    //    eject as we will block on the profile change semaphore. Again, finish
    //    what you started...
    //

    deviceNode = PP_DO_TO_DN(entry->DeviceObject);
    if (entry->RelationsList != NULL) {
        if (entry->ProfileChangingEject) {

            PpProfileMarkAllTransitioningDocksEjected();
        }

        PnpInvalidateRelationsInList(entry->RelationsList,
                                     EjectDevice,
                                     FALSE,
                                     TRUE);

        //
        // Free the relations list.  The device node for the device should
        // still be around as the device has not been subsumed into another
        // remove.
        //

        IopFreeRelationList( entry->RelationsList );

        PNP_ASSERT((deviceNode != NULL) &&
                   (deviceNode->PendingEjectRelations != NULL));

        deviceNode->PendingEjectRelations = NULL;

    } else {
        entry->DisplaySafeRemovalDialog = FALSE;
        if (deviceNode != NULL) {

            PNP_ASSERT(deviceNode->PendingEjectRelations == NULL);

        }
    }

    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Complete the event
    //
    if (entry->DeviceEvent != NULL ) {

        PnpCompleteDeviceEvent( entry->DeviceEvent, status );
    }

    if (entry->DisplaySafeRemovalDialog) {

        PnpSetDeviceRemovalSafe(entry->DeviceObject, NULL, NULL);
    }

    ObDereferenceObject(entry->DeviceObject);
    ExFreePool( entry );
}

VOID
PnpQueuePendingSurpriseRemoval (
    __in PDEVICE_OBJECT DeviceObject,
    __inout PRELATION_LIST *List,
    __in ULONG Problem,
    __in PNP_DEVICE_DELETE_TYPE DeleteType
    )
{

    PPENDING_RELATIONS_LIST_ENTRY entry, existingEntry;
    PRELATION_LIST currentRelationList;
    PLIST_ENTRY     link, nextLink;
    PDEVICE_NODE relatedDeviceNode;
    PDEVICE_OBJECT relatedDeviceObject;
    PDEVICE_NODE childDeviceNode;
    RELATION_LIST_MARKER marker;
    PRELATION_LIST originalRelationList;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(DeleteType == RemoveFailedDevice ||
               DeleteType == SurpriseRemoveDevice);

    originalRelationList = *List;
    currentRelationList = *List;
    entry = NULL;

    //
    // The relation list may be freed, so caller should not be referencing it
    // after passing it into this function.
    //

    *List = NULL;

    //
    // Acquire the surprise remove lock and determine if there is an existing
    // entry in the list that should be considered part of the same relation
    // list (previously deleted child, same device previously failed, etc).
    //

    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&IopSurpriseRemoveListLock, TRUE);
    PnpAcquireDependencyRelationsLock(TRUE);
    nextLink = IopPendingSurpriseRemovals.Flink;
    while (nextLink != &IopPendingSurpriseRemovals) {
        link = nextLink;
        nextLink = nextLink->Flink;
        existingEntry = CONTAINING_RECORD(link,
                                          PENDING_RELATIONS_LIST_ENTRY,
                                          Link);

        childDeviceNode = PP_DO_TO_DN(existingEntry->DeviceObject);

        //
        // Special case for an existing entry with the same target device
        // as the new target device.
        //

        if (existingEntry->DeviceObject == DeviceObject) {

            //
            // A device that was previously failed is now being directly
            // surprise removed. Since the entry is already in the list
            // just reuse it. Since its insertion should have already picked
            // up any relations, done walking the list.
            //

            PNP_ASSERT(DeleteType == SurpriseRemoveDevice);
            PNP_ASSERT(existingEntry->DeleteType == RemoveFailedDevice);
            PNP_ASSERT(childDeviceNode->State == DeviceNodeDeletePendingCloses);
            PNP_ASSERT(childDeviceNode->PreviousState == DeviceNodeRemovePendingCloses);
            PNP_ASSERT(childDeviceNode->PreviousParent != NULL);
            PNP_ASSERT(currentRelationList == originalRelationList);

            entry = existingEntry;
            currentRelationList = existingEntry->RelationsList;
            break;
        }

        //
        // In the normal case, check if there is an ordering requirement
        // based on parent and dependency chain; if there is, a merge
        // is required.
        //

        if (IopCheckIfMergeRequired(existingEntry->RelationsList,
                                    currentRelationList) == FALSE) {

            //
            // A merge is not required. Continue checking the queue.
            //

            continue;
        }

        if (currentRelationList == originalRelationList) {
            entry = existingEntry;

            //
            // Create new relation list initially populated with the
            // contents of the relation list passed in as part of this
            // call.
            //

            currentRelationList = IopAllocateRelationList(
                                      SurpriseRemoveDevice);

            PNP_ASSERT(currentRelationList != NULL);
            if (currentRelationList == NULL) {

                //
                // Try to leave something in the list on failure
                //

                currentRelationList = originalRelationList;
                break;
            }

            status = IopMergeRelationLists(currentRelationList,
                                           originalRelationList,
                                           FALSE);

            PNP_ASSERT(status == STATUS_SUCCESS);
        }

        //
        // Perform the merge between two relation lists:
        // merge devices in the existing list into the current list.
        //

        status = IopMergeRelationLists(currentRelationList,
                                       existingEntry->RelationsList,
                                       TRUE);

        PNP_ASSERT(status == STATUS_SUCCESS);

        //
        // Remove the Existing entry from IopPendingSurpriseRemovals
        // list.
        //

        IopFreeRelationList(existingEntry->RelationsList);
        if (entry != existingEntry) {
            RemoveEntryList(link);
            ExFreePool(existingEntry);

        } else {

            entry->RelationsList = NULL;
        }
    }

    PnpReleaseDependencyRelationsLock();

    if (originalRelationList != currentRelationList) {
        IopFreeRelationList(originalRelationList);
        originalRelationList = NULL;

    } else {

        //
        // Allocate and initialize the pending removal entry.
        //

        entry = (PPENDING_RELATIONS_LIST_ENTRY)
                PnpAllocateCriticalMemory(SurpriseRemoveDevice,
                                          NonPagedPoolNx,
                                          sizeof(PENDING_RELATIONS_LIST_ENTRY),
                                          PNP_POOLTAG_DEVICE_REMOVAL);

        //
        // Insert the new entry into the IopPendingSurpriseRemovals list.
        //

        InsertTailList(&IopPendingSurpriseRemovals, &entry->Link);
    }

    PNP_ASSERT(entry != NULL);

    //
    // Adjust DeletedChildren count for new relation lists.
    // Even if a merge did not occur, it is possible that device
    // nodes were in the surprise removal queue with RemovePendingCloses
    // state but final removes are not actually sent because its state
    // changed to AwaitingQueuedDeletion while in the queue.
    //

    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &marker);
    while (IopEnumerateRelations(currentRelationList,
                                 &marker,
                                 &relatedDeviceObject,
                                 NULL,
                                 NULL)) {

        relatedDeviceNode = PP_DO_TO_DN(relatedDeviceObject);

        //
        // If the previous parent is in the same list and a reference
        // has been previously added, remove the reference.
        //

        if ((((ULONG_PTR) relatedDeviceNode->PreviousParent) & PNP_DN_PREVIOUS_PARENT_REFERENCED) != 0 &&
            IopIsDescendantNode(currentRelationList, relatedDeviceObject) != FALSE) {

            //
            // Remove PNP_DN_PREVIOUS_PARENT_REFERENCED tag to
            // avoid decrementing again when remove IRP is sent.
            //

            relatedDeviceNode->PreviousParent = PNP_DN_PREVIOUS_PARENT(relatedDeviceNode);

            //
            // Decrement the counter for the parent.
            //

            relatedDeviceNode->PreviousParent->DeletedChildren -= 1;

            ASSERT_NOT_NEGATIVE(relatedDeviceNode->PreviousParent->DeletedChildren);
        }
    }

    entry->DeviceObject = DeviceObject;
    entry->Problem = Problem;
    entry->ProfileChangingEject = FALSE ;
    entry->RelationsList = currentRelationList;
    entry->DeleteType = DeleteType;

    //
    // Always clear this flag to cancel any scheduled removal worker.
    //

    entry->DequeuePending = FALSE;

    //
    // Done updating pending suprise removal list.
    //

    ExReleaseResourceLite(&IopSurpriseRemoveListLock);
    KeLeaveCriticalRegion();
    return;
}

_Use_decl_annotations_
VOID
PnpDequeuePendingSurpriseRemoval (
    PLIST_ENTRY ReadyListHead
    )

/*++

Routine Description:

    Dequeue all entries in pending surprise removals queue that are marked as
    ready for the final remove IRP.

Arguments:

    ReadyListHead - Supplies a pointer to list head that will receive a list of
        pending list surprise remove entries ready for final remove IRP.

Return Value:

    None.

--*/

{

    PPENDING_RELATIONS_LIST_ENTRY RelationsList;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY NextListEntry;

    PAGED_CODE();

    ListEntry = IopPendingSurpriseRemovals.Flink;
    while (ListEntry != &IopPendingSurpriseRemovals) {
        RelationsList = CONTAINING_RECORD(ListEntry, PENDING_RELATIONS_LIST_ENTRY, Link);
        NextListEntry = ListEntry->Flink;
        if (RelationsList->DequeuePending != FALSE) {
            if (IopGetRelationsTaggedCount(RelationsList->RelationsList) !=
                IopGetRelationsCount(RelationsList->RelationsList)) {

                //
                // Not expected; DequeuePending is set but not all devices are
                // tagged.
                //

                PNP_ASSERT(FALSE);
                RelationsList->DequeuePending = FALSE;

            } else {

                //
                // Remove from pending surprise removal queue and insert into
                // the list provided by caller to be processed in batch.
                //

                RemoveEntryList(ListEntry);
                InsertTailList(ReadyListHead, ListEntry);
            }
        }

        ListEntry = NextListEntry;
    }
}

VOID
PnpUnlinkDeviceRemovalRelations (
    __in    PDEVICE_OBJECT              RemovedDeviceObject,
    __inout PRELATION_LIST              RelationsList,
    __in    PNP_UNLOCK_UNLINK_ACTION    UnlinkAction
    )
/*++

Routine Description:

    This routine unlocks the device tree deletion operation.
    If there is any pending kernel deletion, this routine initiates
    a worker thread to perform the work.

Arguments:

    RemovedDeviceObject - Supplies a pointer to the device object to which the
        remove was originally targetted (as opposed to one of the relations).

    DeviceRelations - supplies a pointer to the device's removal relations.

    UnlinkAction - Specifies which devnodes will be unlinked from the devnode
        tree.

        UnLinkRemovedDeviceNodes - Devnodes which are no longer enumerated and
            have been sent a REMOVE_DEVICE IRP are unlinked.

        UnlinkAllDeviceNodesPendingClose - This is used when a device is
            surprise removed.  Devnodes in RelationsList are unlinked from the
            tree if they don't have children and aren't consuming any resources.

        UnlinkOnlyChildDeviceNodesPendingClose - This is used when a device fails
            while started.  We unlink any child devnodes of the device which
            failed but not the failed device's devnode.

Return Value:

    None.

--*/
{

    BOOLEAN descendant;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    RELATION_LIST_MARKER marker;
    PDEVICE_NODE previousParent;
    LARGE_INTEGER removeTime;
    DEVICE_RELATION_LEVEL relationLevel;
    HPNPOPCTX hPnpOpCtx = NULL;

    PAGED_CODE();

    PpDevNodeLockTree(PPL_TREEOP_BLOCK_READS_FROM_ALLOW);
    if (ARGUMENT_PRESENT(RelationsList)) {
        if (RelationsList->Sorted == FALSE) {
            PNP_ASSERT(FALSE);
            goto Exit;
        }

        //
        // Note that this iteration is destructive (elements can be removed
        // during enumeration).
        //

        IopInitEnumerationMarker(ENUMERATE_FORWARD, &marker);
        while (IopEnumerateRelations(RelationsList,
                                     &marker,
                                     &deviceObject,
                                     &relationLevel,
                                     NULL)) {

            deviceNode = (PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode;
            descendant = IopIsDescendantNode(RelationsList, deviceObject);

            //
            // There are three different scenarios in which we want to unlink a
            // devnode from the tree.
            //
            // 1) A devnode is no longer enumerated and has been sent a
            //    remove IRP.
            //
            // 2) A devnode has been surprise removed, has no children, has
            //    no resources or they've been freed.  UnlinkAction will be
            //    UnlinkAllDeviceNodesPendingClose.
            //
            // 3) A devnode has failed and a surprise remove IRP has been sent.
            //    Then we want to remove children without resources but not the
            //    failed devnode itself.  UnlinkAction will be
            //    UnlinkOnlyChildDeviceNodesPendingClose.
            //
            switch(UnlinkAction) {

                case UnlinkRemovedDeviceNodes:

                    //
                    // Removes have been sent to every devnode in this relation
                    // list. Deconstruct the tree appropriately.
                    //

                    break;

                case UnlinkAllDeviceNodesPendingClose:

                    if (deviceObject == RemovedDeviceObject ||
                        descendant != FALSE) {

                        //
                        // The target device, or any descendant devices are deleted.
                        //

                        PNP_ASSERT((deviceNode->State == DeviceNodeDeletePendingCloses) ||
                                   (deviceNode->State == DeviceNodeDeleted));

                    } else {
                        PNP_ASSERT(relationLevel == RELATION_LEVEL_DEPENDENT);
                    }

                    break;

                case UnlinkOnlyChildDeviceNodesPendingClose:

                    if (deviceObject == RemovedDeviceObject) {
                        PNP_ASSERT((deviceNode->State == DeviceNodeRemovePendingCloses) ||
                                   (deviceNode->State == DeviceNodeAwaitingQueuedDeletion));

                    } else if (descendant != FALSE) {

                        //
                        // Any descendant devices are deleted.
                        //

                        PNP_ASSERT((deviceNode->State == DeviceNodeDeletePendingCloses) ||
                                   (deviceNode->State == DeviceNodeDeleted));

                    } else {
                        PNP_ASSERT(relationLevel == RELATION_LEVEL_DEPENDENT);
                    }

                    break;

                default:
                    PNP_ASSERT(0);
                    break;
            }

            //
            // Deconstruct the tree appropriately.
            //
            if ((deviceNode->State == DeviceNodeDeletePendingCloses) ||
                (deviceNode->State == DeviceNodeDeleted)) {

                BOOLEAN bRemoved;

                PNP_ASSERT(!(deviceNode->Flags & DNF_ENUMERATED));

                //
                // Remember the parent node before unlinking.
                //

                previousParent = deviceNode->Parent;

                //
                // Remove the devnode from the tree.
                //

                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                           "PnpUnlinkDeviceRemovalRelations: Cleaning up registry values, instance = %wZ\n",
                           PnpGetDeviceInstancePath(deviceNode)));

                PiLockPnpRegistry(TRUE);

                PnpCleanupDeviceRegistryValues(PnpGetDeviceInstancePath(deviceNode));

                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                           "PnpUnlinkDeviceRemovalRelations: Removing DevNode tree, DevNode = 0x%p\n",
                           deviceNode));

                bRemoved = PpDevNodeRemoveFromTree(deviceNode);

                PiUnlockPnpRegistry();

                if (bRemoved) {
                    deviceNode->PreviousParent = previousParent;
                    if (!PnpIsDeviceInstancePathEmpty(deviceNode)) {

                        PiPnpRtlBeginOperation(&hPnpOpCtx);

                        //
                        // Record the last removal data
                        //

                        KeQuerySystemTime(&removeTime);

                        _PnpSetObjectProperty(PiPnpRtlCtx,
                                              PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                              PNP_OBJECT_TYPE_DEVICE,
                                              NULL,
                                              NULL,
                                              &DEVPKEY_Device_LastRemovalDate,
                                              DEVPROP_TYPE_FILETIME,
                                              (PUCHAR)&removeTime,
                                              sizeof(LARGE_INTEGER),
                                              0);

                        //
                        // Raise PnpRtl events for the updated NtPlugPlay properties.
                        //

                        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_IS_PRESENT);

                        //
                        // Notify user-mode that this devnode has been removed
                        // from the tree.
                        //

                        PnpSetDeviceInstanceRemovalEvent(deviceNode);

                        if (hPnpOpCtx) {

                            PiPnpRtlEndOperation(hPnpOpCtx);
                            hPnpOpCtx = NULL;
                        }
                    }

                    if (deviceNode->State == DeviceNodeDeleted) {
                        IopRemoveCurrentRelationFromList(RelationsList, deviceObject, &marker);

                        //
                        // Remove the reference added during enum.
                        // This is the last reference dropped by PNP.
                        //

                        ObDereferenceObject(deviceObject);

                    } else {

                        //
                        // Remove the reference added during enum.
                        //

                        ObDereferenceObject(deviceObject);

                        //
                        // There is still one more ref on the device object, one
                        // holding it to the relation list. Once the final removes
                        // are sent the relationlist will be freed and then the
                        // final ref will be dropped.
                        //

                    }
                }
            } else {

                PNP_ASSERT(deviceNode->Flags & DNF_ENUMERATED);
            }
        }
    }

Exit:

    PpDevNodeUnlockTree(PPL_TREEOP_BLOCK_READS_FROM_ALLOW);
}

//
// The routines below are specific to kernel mode PnP configMgr.
//
NTSTATUS
PnpUnloadAttachedDriver (
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This function unloads the driver for the specified device object if it does not
    control any other device object.

Arguments:

    DeviceObject - Supplies a pointer to a device object

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    PWCHAR buffer;
    UNICODE_STRING unicodeName;
    ULONG bufferSize;
    PUNICODE_STRING serviceName = &DriverObject->DriverExtension->ServiceKeyName;

    PAGED_CODE();

    if (DriverObject->DriverSection != NULL) {

        if (DriverObject->DeviceObject == NULL) {

            bufferSize = CmRegistryMachineSystemCurrentControlSetServices.Length +
                                     serviceName->Length + sizeof(WCHAR) +
                                     sizeof(L"\\");
            buffer = (PWCHAR) ExAllocatePool(
                                 PagedPool,
                                 bufferSize);
            if (!buffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlStringCbPrintfW(buffer,
                               bufferSize,
                               L"%s\\%s",
                               CmRegistryMachineSystemCurrentControlSetServices.Buffer,
                               serviceName->Buffer);

            RtlInitUnicodeString(&unicodeName, buffer);

            status = IopUnloadDriver(&unicodeName, TRUE);

            if (NT_SUCCESS(status)) {

                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                           "****** Unloaded driver (%wZ)\n",
                           serviceName));

            } else {
                IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                           "****** Error unloading driver (%wZ), status = 0x%08X\n",
                           serviceName,
                           status));

            }

            ExFreePool(unicodeName.Buffer);
        }
        else {
            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                       "****** Skipping unload of driver (%wZ), DriverObject->DeviceObject != NULL\n",
                       serviceName));
        }
    }
    else {
        //
        // This is a boot driver, can't be unloaded just return SUCCESS
        //
        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                   "****** Skipping unload of boot driver (%wZ)\n",
                   serviceName));
    }
    return STATUS_SUCCESS;
}

VOID
PnpRequestDeviceRemoval (
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN      TreeDeletion,
    __in ULONG        Problem,
    __in NTSTATUS     ProblemStatus
    )
/*++

Routine Description:

    This routine queues a work item to remove or delete a device.

Arguments:

    DeviceNode - Supplies a pointer to the device object to be cleaned up.

    TreeDeletion - If TRUE, the devnode is physically missing and should
                   eventually end up in the deleted state. If FALSE, the
                   stack just needs to be torn down.

    Problem - Problem code to assign to the removed stack.

    ProblemStatus - Problem status to assign to the removed stack.

Return Value:

    None.

--*/
{

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    if (DeviceNode) {
        if (PnpIsDeviceInstancePathEmpty(DeviceNode) != FALSE) {
            IopDbgPrint((IOP_ERROR_LEVEL, "Driver %wZ reported child %p missing right after enumerating it!\n", &DeviceNode->Parent->ServiceName, DeviceNode));
            PNP_ASSERT(PnpIsDeviceInstancePathEmpty(DeviceNode) == FALSE);
        }

        PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

        //
        // Queue up the removal.
        //

        PnpAcquireDependencyRelationsLock(TRUE);
        PnpRequestDeviceRemovalWorker(DeviceNode,
                                      DeviceNode,
                                      TreeDeletion,
                                      RELATION_LEVEL_DIRECT_DESCENDANT,
                                      FALSE);

        PnpReleaseDependencyRelationsLock();

        //
        // Queue the event, we'll return immediately after it's queued.
        //

        PnpSetTargetDeviceRemove(DeviceNode->PhysicalDeviceObject,
                                 TRUE,
                                 TRUE,
                                 FALSE,
                                 TRUE,
                                 Problem,
                                 ProblemStatus,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL);
    }
}

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PnpRequestDeviceRemovalWorker (
    __in PDEVICE_NODE TargetDeviceNode,
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN TreeDeletion,
    __in DEVICE_RELATION_LEVEL RelationLevel,
    __in BOOLEAN DescendantNode
    )

/*++

Routine Description:

    This function is a worker routine for PnpRequestDeviceRemoval routine. It
    is used to mark an entire subtree for removal.

    This function may be called twice for a device while traversing the device
    graph. If either call caused a deletion, the end result should be the device
    being marked for deletion, even if the other call intended a removal.

Arguments:

    TargetDeviceNode - Supplies a pointer to the target device being removed.

    DeviceNode - Supplies a pointer to the device node to mark.

    TreeDeletion - Supplies a boolean that indicates whether the removal is
                   physical or stack specific.

    RelationLevel - Supplies the relation level of the target removal device.

    DescendantNode - TRUE if this node is a descendant node; in other words,
        its parent was part of the tree walk.

Return Value:

    None.

--*/

{

    BOOLEAN addSentinelState;
    ULONG dependencyTypes;
    PDEVICE_OBJECT dependentPdo;
    PDEVICE_NODE child;
    PDEVICE_NODE dependentNode;
    PLIST_ENTRY listEntry;
    PLIST_ENTRY listHead;
    PNP_DEVNODE_STATE sentinelState;

    PAGED_CODE();

    PNP_ASSERT(RelationLevel == RELATION_LEVEL_DIRECT_DESCENDANT ||
               RelationLevel == RELATION_LEVEL_DEPENDENT);

    PiAssertDependencyRelationsLockHeld(TRUE);
    if (DescendantNode != FALSE) {

        //
        // Device is a descendant of a device being removed or deleted.
        // Therefore, regardless of relation level, it must be deleted.
        //

        PNP_ASSERT(TreeDeletion != FALSE);

    } else if (RelationLevel == RELATION_LEVEL_DEPENDENT) {

        //
        // Top-level dependent nodes are removed, never deleted.
        //

        PNP_ASSERT(TreeDeletion == FALSE);
    }

    if (RelationLevel == RELATION_LEVEL_DIRECT_DESCENDANT || DescendantNode != FALSE) {

        //
        // The target device, descendants of target device, and descendants
        // of dependent devices must have thier device node states checked
        // to see if a sentinel state should be added.
        //

        addSentinelState = PipDeviceRemovalCheckDeviceNodeState(
            DeviceNode,
            TargetDeviceNode,
            TreeDeletion);

    } else {

        //
        // Top-level dependent devices are special case; these will be Surprise
        // Failed only if the device is considered to be started.
        //

        addSentinelState = (BOOLEAN) PipIsDevNodeDNStarted(DeviceNode);
    }

    if (addSentinelState != FALSE) {

        //
        // Give the devnode a sentinel state that will keep the start/enum engine
        // at bay until the removal engine processes the tree.
        //

        sentinelState = (TreeDeletion != FALSE) ?
            DeviceNodeAwaitingQueuedDeletion :
            DeviceNodeAwaitingQueuedRemoval;

        PipSetDevNodeState(DeviceNode, sentinelState, NULL);

        //
        // Call into the child device nodes.
        //

        child = DeviceNode->Child;
        while (child != NULL) {

            //
            // Descendant devices are always marked for deletion.
            //

            PnpRequestDeviceRemovalWorker(TargetDeviceNode,
                                          child,
                                          TRUE,
                                          RelationLevel,
                                          TRUE);

            child = child->Sibling;
        }

        //
        // Call into dependents.
        //

        listHead = PiGetDependentList(DeviceNode->PhysicalDeviceObject);
        listEntry = listHead->Flink;
        while (listEntry != listHead) {
            PiEnumerateDependentListEntry(listEntry, &dependentPdo, &dependencyTypes);
            PNP_ASSERT((dependencyTypes &
                        ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                        PNP_DEPENDENCY_TYPE_START)) == 0);

            listEntry = listEntry->Flink;
            if (dependentPdo == NULL) {
                continue;
            }

            dependentNode = PP_DO_TO_DN(dependentPdo);
            PNP_ASSERT(dependentNode != NULL);

            //
            // Top-level dependents are always marked for removal, not deletion.
            // However, their descendants will be marked for deletion.
            //

            PnpRequestDeviceRemovalWorker(TargetDeviceNode,
                                          dependentNode,
                                          FALSE,
                                          RELATION_LEVEL_DEPENDENT,
                                          FALSE);

        }
    }
}

BOOLEAN
PnpIsBeingRemovedSafely (
    __in PDEVICE_NODE DeviceNode,
    __in DEVICE_RELATION_LEVEL RelationLevel
    )
/*++

Routine Description:

    This function looks at a device with a physical remove queued against it
    and indicates whether it is safe to remove.

Arguments:

    DeviceNode - Supplies a pointer to the device node to examine.

Return Value:

    BOOLEAN - TRUE iff the devnode is safe to be removed.

--*/
{
    PNP_DEVNODE_STATE State;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    switch (RelationLevel) {
        case RELATION_LEVEL_DIRECT_DESCENDANT:
            PNP_ASSERT(DeviceNode->State == DeviceNodeAwaitingQueuedDeletion);
            break;

        case RELATION_LEVEL_DEPENDENT:
            PNP_ASSERT(DeviceNode->State == DeviceNodeAwaitingQueuedDeletion ||
                       DeviceNode->State == DeviceNodeAwaitingQueuedRemoval ||
                       DeviceNode->State == DeviceNodeRemovePendingCloses ||
                       DeviceNode->State == DeviceNodeDeletePendingCloses);

            break;

        case RELATION_LEVEL_REMOVE_EJECT:
        default:
            PNP_ASSERT(FALSE);
            return TRUE;
    }

    if (IopDeviceNodeFlagsToCapabilities(DeviceNode)->SurpriseRemovalOK) {
        return TRUE;
    }

    if (DeviceNode->State == DeviceNodeAwaitingQueuedDeletion ||
        DeviceNode->State == DeviceNodeAwaitingQueuedRemoval) {

        //
        // Ignore the scheduler state and check the previous one instead.
        //

        State = DeviceNode->PreviousState;

    } else {
        State = DeviceNode->State;
    }

    if (State != DeviceNodeStarted &&
        State != DeviceNodeStopped &&
        State != DeviceNodeStartPostWork &&
        State != DeviceNodeRestartCompletion) {

        return TRUE;

    } else {
        return FALSE;
    }
}

BOOLEAN
PipDeviceRemovalCheckDeviceNodeState (
    PDEVICE_NODE DeviceNode,
    PDEVICE_NODE TargetDeviceNode,
    BOOLEAN TreeDeletion
)

/*++

Routine Description:

    Determines if the given device node should be marked for
    AwaitingQueuedRemoval or AwaitingQueuedDeletion, depending on the context.

Arguments:

    DeviceNode - Supplies a pointer to the device node to examine.

    TargetDeviceNode - Supplies the target device of the removal operation.

    TreeDeletion - Supplies TRUE if DeviceNode is for deletion, FALSE if removal.

Return Value:

    BOOLEAN - TRUE if DeviceNode should be given a sentinel state.

--*/

{

    BOOLEAN addSentinelState;

    PAGED_CODE();

#if !DBG

    UNREFERENCED_PARAMETER(TargetDeviceNode);

#endif

    addSentinelState = TRUE;

    switch(DeviceNode->State) {
        case DeviceNodeUninitialized:

            PNP_ASSERT(TreeDeletion != FALSE);
            break;

        case DeviceNodeInitialized:

            //
            // This can happen on a non-descendant node if it fails AddDevice.
            //

            break;

        case DeviceNodeDriversAdded:

            //
            // Happens when a parent stops enumerating a kid who had a
            // resource conflict. This can also happen if AddDevice fails when
            // a lower filter is attached but the service fails.
            //

            break;

        case DeviceNodeResourcesAssigned:

            //
            // Happens when a parent stops enumerating a kid who has been
            // assigned resources but hadn't yet been started.
            //

            PNP_ASSERT(TreeDeletion != FALSE);
            break;

        case DeviceNodeStartPending:

            //
            // Not implemented yet.
            //

            PNP_ASSERT(0);
            break;

        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:

            //
            // These are operational states for taking Added to Started. No
            // descendant should be in this state as the engine currently
            // finishes these before progressing to the next node.
            //
            // Note that DeviceNodeStartPostWork can occur on a legacy added
            // root enumerated devnode. Since the root itself cannot disappear
            // or be removed the below asserts still hold true.


            PNP_ASSERT(TargetDeviceNode == DeviceNode);
            PNP_ASSERT(TreeDeletion == FALSE);

            break;

        case DeviceNodeStarted:
            break;

        case DeviceNodeQueryStopped:

            //
            // Internal rebalance engine state, should never be seen.
            //

            PNP_ASSERT(0);
            break;

        case DeviceNodeStopped:

            //
            // IoInvalidateDeviceState - ResourceRequirementsChanged will
            // trigger a re-enumeration of the entire tree and allow for
            // non-intermediate states to be processed.  This means that QDRs
            // will be sent to all nodes and thus it is possible for a device
            // to be found missing while in the stopped state.
            //

            PNP_ASSERT(TreeDeletion != FALSE);
            break;

        case DeviceNodeRestartCompletion:

            //
            // There are two reasons why this might be the current state. Either
            // the device failed to restart, or during a resource requirements
            // change the parent noticed it missing. If the device failed to
            // restart, then it had better be the target of the removal and not a
            // child. If the parent noticed it missing, then the previous state
            // must have been Started.
            //

            PNP_ASSERT(((TargetDeviceNode == DeviceNode) && (TreeDeletion == FALSE)) ||
                       (DeviceNode->PreviousState == DeviceNodeStarted));

            break;

        case DeviceNodeEnumeratePending:

            //
            // Not implemented yet.
            //

            PNP_ASSERT(0);

            break;

        case DeviceNodeAwaitingQueuedRemoval:

            if (TreeDeletion != FALSE) {

                //
                // Switch QueuedRemoval to QueuedDeletion.
                //

                PipRestoreDevNodeState(DeviceNode);
                PipSetDevNodeState(DeviceNode, DeviceNodeAwaitingQueuedDeletion, NULL);
            }

            //
            // Since this device has been visited previously, do not process
            // its children and dependents again.
            //

            addSentinelState = FALSE;
            break;

        case DeviceNodeAwaitingQueuedDeletion:

            //
            // The device is already marked for deletion.
            //

            addSentinelState = FALSE;
            break;

        case DeviceNodeRemovePendingCloses:
        case DeviceNodeRemoved:

            //
            // An already removed device should not appear in the remove case,
            // since if its ancestor was removed or deleted, this node would
            // have been unlinked from the tree.
            // Therefore, it must be a deletion operation.
            //

            PNP_ASSERT(TreeDeletion != FALSE);
            break;

        case DeviceNodeDeletePendingCloses:
        case DeviceNodeDeleted:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeQueryRemoved:
        case DeviceNodeUnspecified:
        default:

            PNP_ASSERT(0);
            break;
    }

    return addSentinelState;
}


