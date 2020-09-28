/*++

Copyright (c) Microsoft Corporation

Module Name:

    eventremove.c

Abstract:

    This module contains handling of device removal events.

Environment:

    Kernel Mode

Author:

    Minsang Kim (mikim) 15-Oct-2015

--*/

#include "pnpmgrp.h"
#include "eventremove.h"

//
// Number of times we check the ref count.
// Total timeout = MAX_ATTEMPTS  * 100ms
//               = 5 seconds
//

#define MAX_ATTEMPTS 50

NTSTATUS
PiEventRemovalCheckOpenHandles (
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount,
    _Out_ PDEVICE_OBJECT *VetoingDevice
    );

#pragma alloc_text(PAGE, PiEventRemovalGetDeleteType)
#pragma alloc_text(PAGE, PiEventQueryRemoveNoFdo)
#pragma alloc_text(PAGE, PiEventRemovalOpenHandleVeto)
#pragma alloc_text(PAGE, PiEventRemovalPostSurpriseRemove)
#pragma alloc_text(PAGE, PiEventRemovalCompleteNotifyKernel)
#pragma alloc_text(PAGE, PiEventRemovalCanceledNotifyKernel)
#pragma alloc_text(PAGE, PiEventQueryRemoveNotifyKernel)
#pragma alloc_text(PAGE, PiEventRemovalCheckOpenHandles)
#pragma alloc_text(PAGE, PiEventRestartRemovalRelations)
#pragma alloc_text(PAGE, PiEventBuildPdoList)
#pragma alloc_text(PAGE, PiEventExcludeDeviceRelations)
#pragma alloc_text(PAGE, PiEventAllocateVetoBuffer)
#pragma alloc_text(PAGE, PiEventQueryRemoveDevices)

PNP_DEVICE_DELETE_TYPE
PiEventRemovalGetDeleteType (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    )

/*++

Routine Description:

    This routine determines what removal operation is needed to be performed
    based on the state of the device node and the requested device event.

Arguments:

    DeviceNode - Supplies a pointer to the device node.

    DeviceEvent - Supplies a pointer to the device removal event.

Return Value:

    Enum value to indiate what removal operation is needed.

--*/

{

    PNP_DEVICE_DELETE_TYPE DeleteType;
    PDEVICE_OBJECT DeviceObject;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    DeviceObject = DeviceNode->PhysicalDeviceObject;
    if (PnpCompareGuid(&DeviceEvent->Data.EventGuid, &GUID_DEVICE_EJECT)) {
        DeleteType = EjectDevice;

    } else if (DeviceEvent->Data.Flags & TDF_KERNEL_INITIATED) {
        if (!(DeviceNode->Flags & DNF_ENUMERATED)) {

            //
            // In most cases the state should be the scheduler state
            // DeviceNodeAwaitingQueuedDeletion, however there is the possibilty
            // that the device was being surprise failed before it was
            // reported missing.  In that case the scheduler state would have
            // been cleared off the stack prior to sending the surprise remove
            // and as such the current state would be DeletePendingCloses.
            //
            // PNP_ASSERT(DeviceNode->State == DeviceNodeAwaitingQueuedDeletion);
            //

            if ((DeviceNode->PreviousState == DeviceNodeStarted) ||
                (DeviceNode->PreviousState == DeviceNodeStopped) ||
                (DeviceNode->PreviousState == DeviceNodeStartPostWork) ||
                (DeviceNode->PreviousState == DeviceNodeRestartCompletion) ||
                (DeviceObject->DeviceObjectExtension->ExtensionFlags &
                 DOE_REMOVE_PENDING)) {

                DeleteType = SurpriseRemoveDevice;

            } else {
                DeleteType = RemoveDevice;
            }

        } else {

            PNP_ASSERT(!(DeviceObject->DeviceObjectExtension->ExtensionFlags &
                DOE_REMOVE_PENDING));

            //
            // In most cases, the devnode state should be
            // DeviceNodeAwaitingQueuedRemoval.  However, it is possible
            // for a parent to queue a surprise remove while the original
            // remove event was waiting in the event queue.  This causes
            // the devnode's state to be changed to
            // DeviceNodeAwaitingQueuedDeletion even though the device is still
            // marked as DNF_ENUMERATED.
            //
            // PNP_ASSERT(DeviceNode->State == DeviceNodeAwaitingQueuedRemoval);

            if ((DeviceNode->PreviousState == DeviceNodeStarted) ||
                (DeviceNode->PreviousState == DeviceNodeStopped) ||
                (DeviceNode->PreviousState == DeviceNodeStartPostWork) ||
                (DeviceNode->PreviousState == DeviceNodeRestartCompletion)) {

                DeleteType = RemoveFailedDevice;

            } else {
                DeleteType = RemoveUnstartedFailedDevice;
            }
        }

    } else {
        DeleteType = QueryRemoveDevice;
    }

    return DeleteType;
}

NTSTATUS
PiEventQueryRemoveNoFdo (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    )

/*++

Routine Description:

    This routine processes a query removal operation for the case where
    no functional device object (FDO) is present on the stack; i.e., no service
    has been loaded.

Arguments:

    DeviceNode - Supplies a pointer to the device node of the target device
        being removed.

    DeviceEvent - Supplies a pointer to the device event.

Return Value:

    NT Status code.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    //
    // The device doesn't have an FDO.
    //

    PNP_ASSERT(PipAreDriversLoaded(DeviceNode) == FALSE);

    Status = STATUS_SUCCESS;
    if ((DeviceNode->State == DeviceNodeInitialized) ||
        (DeviceNode->State == DeviceNodeRemoved)) {

        //
        // The rules are:
        // 1) !TDF_NO_RESTART means clear the devnode and get it ready
        //    as long as the problem is user resettable. Ignore the passed
        //    in problem code (probably either CM_PROB_WILL_BE_REMOVED or
        //    CM_PROB_DEVICE_NOT_THERE), as it means nothing.
        // 2) TDF_NO_RESTART means change the problem code over if you can.
        //    If the problem code is not user resettable, the problem code
        //    won't change.
        //
        // In all cases we try to clear the problem.
        //

        if (PipDoesDevNodeHaveProblem(DeviceNode)) {
            if (!PipIsProblemReadonly(DeviceNode, DeviceNode->Problem)) {
                PipClearDevNodeProblem(DeviceNode);
            }
        }

        if (!PipDoesDevNodeHaveProblem(DeviceNode)) {
            if (!(DeviceEvent->Data.Flags & TDF_NO_RESTART))  {

                //
                // This is a reset attempt. Mark the devnode so that it
                // comes online next enumeration.
                //

                PnpRestartDeviceNode(DeviceNode);

            } else {

                //
                // We're changing or setting problem codes. Note that the
                // device is still in DeviceNodeInitialized or
                // DeviceNodeRemoved.
                //

                PipSetDevNodeProblem(DeviceNode,
                                     DeviceEvent->Argument,
                                     STATUS_SUCCESS);
            }

        } else {

            //
            // The problem is fixed, so the devnode state is immutable
            // as far as user mode is concerned. Here we fail the call
            // if we can't bring the devnode back online. We always succeed
            // the call if it was an attempt to change the code, as the
            // user either wants to prepare the device for ejection (done),
            // or wants to disable it (as good as done.)
            //

            if (!(DeviceEvent->Data.Flags & TDF_NO_RESTART))  {
                Status = STATUS_INVALID_PARAMETER;
            }
        }
    }

    return Status;
}

VOID
PiEventRemovalOpenHandleVeto (
    _In_ ULONG PdoListCount,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ PDEVICE_OBJECT VetoingDevice,
    _Inout_ PENUM_HANDLES_CONTEXT EnumContext,
    _Out_ PPNP_VETO_TYPE VetoType,
    _Inout_ PUNICODE_STRING VetoString
    )

/*++

Routine Description:

    This routine handles the case where query remove notifications and IRPs
    have been sent to devices but file handles on devices are not closed.

    Fill out veto information, and send cancel remove IRPs.

Arguments:

    PdoListCount - Supplies the length of PdoList array.

    PdoList - Supplies an array of PDOs.

    VetoingDevice - Supplies a pointer to a device object of the device that
        has handles open.

    EnumContext - Supplies a pointer to the context object that will be used
        for logging.

    VetoType - Supplies a pointer to receive the veto type.

    VetoString - Supplies a pointer to a unicode string to receive the
        instance path of the vetoing device.

Return Value:

    None.

--*/

{

    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT DeviceObject;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                 "PnpProcessQueryRemoveAndEject: Vetoed due to open handles "
                 "against the device\n"));

    *VetoType = PNP_VetoOutstandingOpen;
    PnpCollectOpenHandles(PdoList, PdoListCount, EnumContext);

    //
    // Since nothing has explicitly vetoed the removal yet, the internal veto
    // string should still be empty.
    //

    PNP_ASSERT(VetoString->Length == 0);

    if (VetoingDevice != NULL) {
        DeviceObject = IoGetDeviceAttachmentBaseRef(VetoingDevice);
        DeviceNode = PP_DO_TO_DN(DeviceObject);
        RtlCopyUnicodeString(VetoString,
                             PnpGetDeviceInstancePath(DeviceNode));

        ObDereferenceObject(DeviceObject);

    } else {
        RtlInitUnicodeString(VetoString, NULL);
    }

    return;
}

VOID
PiEventRemovalPostSurpriseRemove (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _Inout_ PRELATION_LIST *RelationList
    )

/*++

Routine Description:

    This routine post-processes a surprise remove and surprise fail
    events, after a surprise remove IRP has been sent to devices.

    Device nodes are unlinked from the device tree and the devices are
    put into the queue where they will wait for open file handles to be closed.

Arguments:

    DeviceNode - Supplies a pointer to the device node of the target device
        being removed.

    DeviceEvent - Supplies a pointer to the device event.

    DeleteType - Supplies the removal operation being performed.

    RelationList - Supplies a pointer to a pointer to the relation list.
        A NULL may be assigned since the relation list may be freed when
        queued.

Return Value:

    NT Status code.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(DeviceEvent != NULL);
    PNP_ASSERT((DeleteType == RemoveFailedDevice) ||
               (DeleteType == SurpriseRemoveDevice));

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    //
    // Unlock the device relations list.
    //
    // Note there could be a potential race condition here between
    // unlocking the devnodes in the relation list and completing the
    // execution of PnpIsChainDereferenced.  If an enumeration
    // takes places (we've unlocked the devnode) before the eventual remove
    // is sent then problems could occur.
    //
    // This is prevented by the setting of DNF_REMOVE_PENDING_CLOSES when
    // we sent the IRP_MN_SURPRISE_REMOVAL.
    //
    // We do need to do it prior to calling PnpQueuePendingSurpriseRemoval
    // since we lose ownership of the relation list in that call.  Also
    // PnpIsChainDereferenced may cause the relation list to be
    // freed before it returns.
    //
    // If this is a RemoveFailedDevice then we don't want to remove the
    // device node from the tree but we do want to remove children without
    // resources.
    //

    PnpUnlinkDeviceRemovalRelations(DeviceNode->PhysicalDeviceObject,
                                    *RelationList,
                                    DeleteType == SurpriseRemoveDevice ?
                                        UnlinkAllDeviceNodesPendingClose :
                                        UnlinkOnlyChildDeviceNodesPendingClose);

    if (DeviceNode->Parent == NULL) {

        //
        // The target device was unlinked from the tree.
        //

        PNP_ASSERT(DeviceNode->PreviousParent != NULL);
        PNP_ASSERT(((ULONG_PTR) DeviceNode->PreviousParent &
                    PNP_DN_PREVIOUS_PARENT_REFERENCED) == 0);

        //
        // Add a reference on the previous parent.
        //

        DeviceNode->PreviousParent->DeletedChildren += 1;

        //
        // Set a flag to indicate that a reference has been added
        // to the parent.
        //

        DeviceNode->PreviousParent = (PDEVICE_NODE)
            (((ULONG_PTR) DeviceNode->PreviousParent) |
                PNP_DN_PREVIOUS_PARENT_REFERENCED);

    }

    //
    // Add the relation list to a list of pending surprise removals.
    //

    PnpQueuePendingSurpriseRemoval(DeviceNode->PhysicalDeviceObject,
                                   RelationList,
                                   DeviceEvent->Argument,
                                   DeleteType);

    return;
}

VOID
PiEventRemovalCompleteNotifyKernel (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount
    )

/*++

Routine Description:

    This routine sends "remove complete" notifications to kernel mode
    listeners.

Arguments:

    DeleteType - Supplies the removal operation being performed.

    PdoList - Supplies an array of PDOs being removed.

    PdoListCount - Supplies the array size of PdoList.

Return Value:

    None.

--*/

{

    ULONG Index;
    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT DeviceObject;
    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    for (Index = 0; Index < PdoListCount; Index += 1) {
        DeviceObject = PdoList[Index];
        DeviceNode = PP_DO_TO_DN(DeviceObject);
        if ((DeleteType == QueryRemoveDevice) &&
            (DeviceNode != NULL) &&
            ((DeviceNode->StateFlags &
              DNSF_DEVICE_HUNG_DURING_RESET) != 0)) {

            continue;
        }

        Status = PnpNotifyTargetDeviceChange(
                     &GUID_TARGET_DEVICE_REMOVE_COMPLETE,
                     DeviceObject,
                     NULL,
                     NULL);

        PNP_ASSERT(NT_SUCCESS(Status));
    }

    return;
}

VOID
PiEventRemovalCanceledNotifyKernel (
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount
    )

/*++

Routine Description:

    This routine sends "remove cancelled" notifications to kernel mode
    listeners.

Arguments:

    PdoList - Supplies an array of PDOs being removed.

    PdoListCount - Supplies the array size of PdoList.

Return Value:

    None.

--*/

{
    
    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT DeviceObject;
    ULONG Index;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    //
    // Iterate backwards; i.e., reverse order of query remove notifications.
    //

    for (Index = 0; Index < PdoListCount; Index += 1) {
        DeviceObject = PdoList[PdoListCount - 1 - Index];
        DeviceNode = PP_DO_TO_DN(DeviceObject);

        //
        // No need to send cancel query remove notification
        // to devices which are already removed.
        //

        if ((DeviceNode != NULL) &&
            ((DeviceNode->State == DeviceNodeRemoved) ||
             (DeviceNode->State == DeviceNodeInitialized) ||
             (DeviceNode->State == DeviceNodeUninitialized))) {

            continue;
        }

        PnpNotifyTargetDeviceChange(&GUID_TARGET_DEVICE_REMOVE_CANCELLED,
                                    DeviceObject,
                                    NULL,
                                    NULL);
    }

    return;
}

NTSTATUS
PiEventQueryRemoveNotifyKernel (
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount,
    _Out_ PDRIVER_OBJECT *VetoingDriver,
    _Out_ PPNP_VETO_TYPE VetoType,
    _Out_ PUNICODE_STRING VetoString
    )

/*++

Routine Description:

    This routine sends query removal notifications to kernel mode
    listeners.

Arguments:

    DeviceEvent - Supplies a pointer ot the current device event.

    PdoList - Supplies an array of PDOs being removed.

    PdoListCount - Supplies the array size of PdoList.

    VetoingDriver - Supplies a pointer to receive the driver that vetoed
        the query removal.

    VetoType - Supplies a pointer to receive the veto reason type.

    VetoString - Supplies a pointer to receive the name of the vetoing driver.

Return Value:

    NT status code.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    PDEVICE_NODE DeviceNode;
    ULONG Index;
    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    Status = STATUS_SUCCESS;
    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                 "PnpProcessQueryRemoveAndEject: QUERY_REMOVE - notifying kernel-mode\n"));

    for (Index = 0; Index < PdoListCount; Index += 1) {
        DeviceObject = PdoList[Index];

        //
        // If the device has already been removed or is unstarted,
        // then no query remove will be sent and thus no notification
        // is needed.
        //

        DeviceNode = PP_DO_TO_DN(DeviceObject);
        if ((DeviceNode != NULL) &&
            ((DeviceNode->State == DeviceNodeRemoved) ||
             (DeviceNode->State == DeviceNodeInitialized) ||
             (DeviceNode->State == DeviceNodeUninitialized))) {

            continue;
        }

        //
        // The original requestor has already decided to unwind.  No need to actually
        // call down to the driver.
        //

        if (DeviceEvent->Cancel != FALSE) {
            Status = STATUS_CANCELLED;

        } else {
            Status = PnpNotifyTargetDeviceChange(&GUID_TARGET_DEVICE_QUERY_REMOVE,
                                                 DeviceObject,
                                                 NULL,
                                                 VetoingDriver);
        }

        if (!NT_SUCCESS(Status)) {

            //
            // The kernel mode notification was failed. Iterate
            // backwards in the list and send cancel remove
            // notifications.
            //

            IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                         "PnpProcessQueryRemoveAndEject: Vetoed due to driver failing QR notification\n"));

            *VetoType = PNP_VetoDriver;
            if (*VetoingDriver != NULL) {
                RtlCopyUnicodeString(VetoString, &((*VetoingDriver)->DriverName));

            } else {
                RtlInitUnicodeString(VetoString, NULL);
            }

            if (Index > 0) {
                PiEventRemovalCanceledNotifyKernel(PdoList, Index - 1);
            }

            break;
        }
    }

    return Status;
}

NTSTATUS
PiEventRemovalCheckOpenHandles (
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount,
    _Out_ PDEVICE_OBJECT *VetoingDevice
    )

/*++

Routine Description:

    This routine checks for open handles on devices until it times out.

Arguments:

    PdoList - Supplies an array of PDOs to check for open handles.

    PdoListCount - Supplies the array size of PdoList.

    VetoingDevice - Supplies a pointer to receive the vetoing device, if any.

Return Value:

    NT status code.

--*/

{

    ULONG Attempt;
    LARGE_INTEGER Delay = { (ULONG)(-10 * 1000 * 100), -1 }; // 100ms
    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    Status = STATUS_DEVICE_BUSY;

    //
    // Handles closed at non-passive level get closed
    // asynchronously. This would be the case when a driver
    // closes the handle as part of handling the
    // QUERY_REMOVE IRP. Since there is no Ob...() API to flush
    // this asynchronous close queue, we wait for a certain time
    // interval for the queue to empty.
    //

    for (Attempt = 0; Attempt < MAX_ATTEMPTS; Attempt += 1) {
        if (Attempt > 0) {

            //
            // Wait 100ms before checking again.
            //

            KeDelayExecutionThread(KernelMode, FALSE, &Delay);
        }

        if (!PnpIsChainDereferenced(PdoList,
                                    PdoListCount,
                                    TRUE,
                                    FALSE,
                                    VetoingDevice)) {

            //
            // No more references, all handles must be closed.
            //

            Status = STATUS_SUCCESS;
            break;
        }
    }

    return Status;
}

VOID
PiEventRestartRemovalRelations (
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PRELATION_LIST RelationList,
    _In_ PDEVICE_OBJECT TargetDeviceObject
    )

/*++

Routine Description:

    This routine restarts devices in the relation list.

Arguments:

    DeviceEvent - Supplies a pointer to the device event.

    RelationList - Supplies a pointer to the relation list of devices being restarted.

    TargetDeviceObject - Supplies a pointer to the target device being removed.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    PDEVICE_NODE DeviceNode;
    PNP_DEVICE_STATE DeviceState;
    RELATION_LIST_MARKER Marker;
    DEVICE_RELATION_LEVEL RelationLevel;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    //
    // Restart removal relations. Target device being removed
    // is restarted unless specified otherwise.
    //

    IopInitEnumerationMarker(ENUMERATE_FORWARD, &Marker);
    while (IopEnumerateRelations(RelationList,
                                 &Marker,
                                 &DeviceObject,
                                 &RelationLevel,
                                 NULL)) {

        DeviceNode = PP_DO_TO_DN(DeviceObject);


        } else if (DeviceObject == TargetDeviceObject) {

            //
            // Target device is restarted unless specified otherwise.
            //

            if (!(DeviceEvent->Data.Flags & TDF_NO_RESTART)) {
                if ((DeviceNode->State == DeviceNodeRemoved) &&
                    PipIsDevNodeProblem(DeviceNode, CM_PROB_WILL_BE_REMOVED)) {

                    PipClearDevNodeProblem(DeviceNode);
                    PnpRestartDeviceNode(DeviceNode);
                }
            }

        } else {

            //
            // All removal relations without existing problem
            // are restarted.
            //

            if (!PipDoesDevNodeHaveProblem(DeviceNode)) {
                if (DeviceNode->State == DeviceNodeRemoved) {
                    PnpRestartDeviceNode(DeviceNode);
                }
            }
        }
    }

    return;
}

NTSTATUS
PiEventBuildPdoList (
    _In_ PRELATION_LIST RelationList,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _Out_ PULONG PdoListCount,
    _Outptr_result_buffer_(*PdoListCount) PDEVICE_OBJECT **PdoListOut,
    _Out_ PBOOLEAN PossibleProfileChangeInProgress,
    _Out_ PBOOLEAN SubsumingProfileChange
    )

/*++

Routine Description:

    This routine constructs the list of PDO of devices being removed.
    Additionally it inspects dock states to determine profile change
    conditions.

Arguments:

    RelationList - Supplies a pointer to the relation list of devices being removed.

    DeleteType - Supplies the removal operation being performed.

    DeviceEvent - Supplies a pointer to the device removal event.

    PdoListCount - Supplies a pointer to receive the number of PDOs in the list.

    PdoListOut - Supplies a pointer to an array to receive PDOs.

    PossibleProfileChangeInProgress - Supplies a pointer to a boolean to receive
        whether a profile change may be in progress.

    SubsumingProfileChange - Supplies a pointer to a boolean to receive whether
        another profile change is subsumed by this new removal operation.

Return Value:

    NT status code.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    PDEVICE_NODE DeviceNode;
    RELATION_LIST_MARKER Marker;
    PDEVICE_OBJECT *PdoList;
    DEVICE_RELATION_LEVEL RelationLevel;
    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);
    NT_ASSERT(RelationList != NULL);
    NT_ASSERT(DeviceEvent != NULL);
    NT_ASSERT(PdoListCount != NULL);
    NT_ASSERT(PdoListOut != NULL);
    NT_ASSERT(PossibleProfileChangeInProgress != NULL);
    NT_ASSERT(SubsumingProfileChange != NULL);

    Status = STATUS_SUCCESS;
    *PossibleProfileChangeInProgress = FALSE;
    *SubsumingProfileChange = FALSE;

    //
    // Allocate the PdoList array.
    //

    PdoList = (PDEVICE_OBJECT *)PnpAllocateCriticalMemory(
                                    DeleteType,
                                    NonPagedPoolNx,
                                    (IopGetRelationsCount(RelationList) *
                                    sizeof(PDEVICE_OBJECT)),
                                    PNP_POOLTAG_PDO_ARRAY);

    if (PdoList == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto EventBuildPdoListEnd;
    }

    *PdoListCount = 0;
    IopInitEnumerationMarker(ENUMERATE_FORWARD, &Marker);
    while (IopEnumerateRelations(RelationList,
                                 &Marker,
                                 &DeviceObject,
                                 &RelationLevel,
                                 NULL)) {

        //
        // Here is a list of what operations retrieve what relations,
        // who they query, and who/how they notify.
        //
        // Operation                    Relations    Queries   Notifies
        // ---------                    ---------    -------   --------
        // EjectDevice                  Ejection     Everyone  Everyone (Remove)
        // SurpriseRemoveDevice         Ejection     NA        Descendants, Dependents (SurpriseRemove)
        // RemoveDevice                 Ejection     NA        Descendants, Dependents (Remove)
        // RemoveFailedDevice           Removal      NA        Descendants, Dependents (SurpriseRemove)
        // RemoveUnstartedFailedDevice  Removal      NA        Descendants, Dependents (Remove)
        // QueryRemoveDevice            Removal      Everyone  Everyone (Remove)
        //
        //
        // N.B.
        //     We do not send SurpriseRemove's to removal relations.
        // While doing so might seem to be the correct behavior, many
        // drivers do not handle this well. Simply reenumerating the
        // parents of the removal relations works much better. Similarly
        // ejection relations have their parents reenumerated (which
        // makes sense, as they are speculative in nature anyway).
        //
        //      If we get in a case where a *parent* of a dock gets
        // into the RemoveFailedDevice case (ie, failed restart,
        // responded to QueryDeviceState with Removed, etc), then we
        // will be shortly losing the children when we stop the parent.
        // However, we want to eject the dock child, not just remove it
        // as starting and ejecting are symmetric here. Note that
        // currently the only such parent would be the root ACPI devnode.
        //
        //      Ejection relations of a device (eg dock) that has been
        // surprise removed are not notified that they *may* have been
        // pulled (remember, ejection relations are speculative). We
        // will notify only DirectDescendants and queue an enumeration
        // on every parent of the ejection relations.  If they really
        // disappeared, they will get their notification, albeit a bit
        // later than some of the other devices in the tree.
        //

        if ((RelationLevel != RELATION_LEVEL_REMOVE_EJECT) ||
            (DeleteType == EjectDevice) ||
            (DeleteType == QueryRemoveDevice)) {

            DeviceNode = PP_DO_TO_DN(DeviceObject);

            //
            // PnpProcessQueryRemoveAndEject will be called twice for
            // the dock during an eject. Once with EjectDevice, and
            // after the dock is listed as missing once more with
            // RemoveDevice. We don't want to start a profile change
            // for RemoveDevice as we are already in one, and we would
            // deadlock if we tried. We don't start one for QueryRemove
            // either as the dock isn't *physically* going away.
            //

            PNP_ASSERT(DeviceNode->DockInfo.DockStatus != DOCK_ARRIVING);

            if ((DeleteType != RemoveDevice) && (DeleteType != QueryRemoveDevice)) {
                if (DeviceNode->DockInfo.DockStatus == DOCK_QUIESCENT) {
                    *PossibleProfileChangeInProgress = TRUE;

                } else if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {
                    *SubsumingProfileChange = TRUE;
                }
            }

            if ((DeleteType == QueryRemoveDevice) || (DeleteType == EjectDevice)) {
                if (DeviceNode->Flags & DNF_LEGACY_DRIVER) {
                    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                 "PnpProcessQueryRemoveAndEject: "
                                 "Vetoed by legacy driver relation\n"));

                    PnpFinalizeVetoedRemove(DeviceEvent,
                                            PNP_VetoLegacyDevice,
                                            PnpGetDeviceInstancePath(DeviceNode));

                    Status = STATUS_UNSUCCESSFUL;
                    break;
                }

                if (DeviceNode->State == DeviceNodeRemovePendingCloses) {
                    IopDbgPrint((IOP_IOEVENT_INFO_LEVEL,
                                 "PnpProcessQueryRemoveAndEject: "
                                 "Vetoed due to device in DeviceNodeRemovePendingCloses\n"));

                    PnpFinalizeVetoedRemove(DeviceEvent,
                                            PNP_VetoOutstandingOpen,
                                            PnpGetDeviceInstancePath(DeviceNode));

                    Status = STATUS_UNSUCCESSFUL;
                    break;
                }

            }

            PdoList[*PdoListCount] = DeviceObject;
            *PdoListCount += 1;
        }
    }

EventBuildPdoListEnd:
    *PdoListOut = PdoList;
    return Status;
}

BOOLEAN
PiEventExcludeDeviceRelations (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType
    )

/*++

Routine Description:

    This routine determines if device relations (ejection relations, removal
    relations) are excluded in a removal operation.

Arguments:

    DeleteType - Supplies the delete operation.

Return Value:

    TRUE if device relations must be excluded; FALSE otherwise.

--*/

{

    BOOLEAN ExcludeDeviceRelations;

    PAGED_CODE();

    if ((DeleteType == SurpriseRemoveDevice) ||
        (DeleteType == RemoveFailedDevice) ||
        (DeleteType == RemoveUnstartedFailedDevice) ||
        (DeleteType == RemoveDevice)) {

        ExcludeDeviceRelations = TRUE;

    } else {
        ExcludeDeviceRelations = FALSE;
    }

    return ExcludeDeviceRelations;
}

PVOID
PiEventAllocateVetoBuffer (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _Out_ PPNP_VETO_TYPE *VetoTypeOut,
    _Out_ PUNICODE_STRING *VetoStringOut
    )

/*++

Routine Description:

    This routine allocates the veto buffer.

Arguments:

    DeleteType - Supplies the delete operation.

    VetoTypeOut - Supplies a pointer to receive the address (within the buffer)
        that contains veto type information.

    VetoStringOut - Supplies a pointer to receive the address (within the buffer)
        that contains veto string.

Return Value:

    A pointer to the allocated buffer, or NULL. It should be freed after being
    used.

--*/

{

    PVOID VetoBase;
    PWSTR VetoBuffer;
    PUNICODE_STRING VetoString;
    PPNP_VETO_TYPE VetoType;

    PAGED_CODE();

    VetoBase = PnpAllocateCriticalMemory(
        DeleteType,
        PagedPool,
        (MAX_VETO_NAME_LENGTH * sizeof(WCHAR) + // String Buffer
         sizeof(UNICODE_STRING) + // Unicode String
         sizeof(PNP_VETO_TYPE)), // Veto Type
        PNP_POOLTAG_INTERNAL_VETO_BUFFER);

    if (VetoBase == NULL) {
        goto EventAllocateVetoBufferEnd;
    }

    //
    // Preinit the veto information
    //

    VetoType = (PPNP_VETO_TYPE)VetoBase;
    VetoString = (PUNICODE_STRING)(VetoType + 1);
    VetoBuffer = (PWSTR)(VetoString + 1);
    *VetoType = PNP_VetoTypeUnknown;
    VetoString->MaximumLength = MAX_VETO_NAME_LENGTH;
    VetoString->Length = 0;
    VetoString->Buffer = VetoBuffer;

    //
    // Copy output parameters.
    //

    *VetoTypeOut = VetoType;
    *VetoStringOut = VetoString;

EventAllocateVetoBufferEnd:
    return VetoBase;
}

NTSTATUS
PiEventQueryRemoveDevices (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PRELATION_LIST RelationList,
    _In_ ULONG PdoListCount,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _Out_ PNP_VETO_TYPE *VetoType,
    _Out_ PUNICODE_STRING VetoString,
    _Out_ PDEVICE_OBJECT *VetoingDevice
    )

/*++

Routine Description:

    This routine performs a query remove action on the device stack.

    1) Send IRP_MN_QUERY_REMOVE_DEVICE to each device in the relation list.
    2) Check for open handles against all devices in the PDO list.

Arguments:

    DeleteType - Supplies the delete operation.

    DeviceEvent - Supplies a pointer to the device removal event.

    DeviceObject - Supplies a pointer to the target device being removed.

    RelationList - Supplies a pointer to the relation list of devices being restarted.

    PdoListCount - Supplies the length of PdoList array.

    PdoList - Supplies an array of PDOs being removed.

    VetoType - Supplies a pointer to receive the address (within the buffer)
        that contains veto type information.

    VetoString - Supplies a pointer to receive the address (within the buffer)
        that contains veto string.

    VetoingDevice - Supplies a pointer to a device object of the device that
        has handles open.

Return Value:

    NT_SUCCESS indicates that all devices in the list accepted query removal
    and there were no open handles.

    Failure code is returned in all other cases.

--*/

{

    ENUM_HANDLES_CONTEXT EnumContext;
    NTSTATUS Status;

    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    //
    // Send query remove IRPs to the devices in the relation list.
    // The returned status is unsuccessful is any of the devices
    // vetoed the removal; if this is the case, cancel IRPs have
    // been sent out.
    //

    Status = PnpDeleteLockedDeviceNodes(DeviceObject,
                                        RelationList,
                                        QueryRemoveDevice,
                                        TRUE,
                                        DeviceEvent->Argument,
                                        DeviceEvent->ArgumentStatus,
                                        VetoType,
                                        VetoString);

    //
    // Initialize enumeration context for collection
    // of handles against the pdo stack.
    //

    RtlZeroMemory(&EnumContext, sizeof(ENUM_HANDLES_CONTEXT));
    if ((DeleteType == EjectDevice) ||
        (DeleteType == QueryRemoveDevice)) {

        EnumContext.EnableLogging = TRUE;
    }

    EnumContext.TargetDeviceInstance = PnpGetDeviceInstancePath(
                                           PP_DO_TO_DN(DeviceObject));

    EnumContext.HandleCount = 0;
    EnumContext.PhysicalDeviceObject = NULL;
    InitializeListHead(&EnumContext.VetoProcessList);

    //
    // If we have enabled the dumping flag, or the user ran oh.exe, spit all
    // handles on a veto to the debugger.
    //

    EnumContext.DumpHandles = ((NtGlobalFlag & FLG_MAINTAIN_OBJECT_TYPELIST) != 0);
    if (NT_SUCCESS(Status)) {

        //
        // Everyone has been notified and had a chance to close
        // their handles. Since no one has vetoed it yet, let's see
        // if there are any open references.
        //

        Status = PiEventRemovalCheckOpenHandles(PdoList,
                                                PdoListCount,
                                                VetoingDevice);

        if (!NT_SUCCESS(Status)) {

            //
            // There are still open handles on devices in the
            // relation list. This is a failure condition for query
            // removal; record the veto information.
            //

            PiEventRemovalOpenHandleVeto(PdoListCount,
                                         PdoList,
                                         *VetoingDevice,
                                         &EnumContext,
                                         VetoType,
                                         VetoString);

            //
            // Send cancel remove to the target devices.
            //

            PnpDeleteLockedDeviceNodes(DeviceObject,
                                       RelationList,
                                       CancelRemoveDevice,
                                       TRUE,
                                       0,
                                       STATUS_SUCCESS,
                                       NULL,
                                       NULL);

            Status = STATUS_UNSUCCESSFUL;
        }

    } else if (*VetoType == PNP_VetoDevice) {

        //
        // If a device in the list vetoed the query remove IRP,
        // collect handles since it is common for a driver to fail
        // the removal request in this case.
        //

        PnpCollectOpenHandles(PdoList,
                              PdoListCount,
                              &EnumContext);
    }

    //
    // Print out vetoing processes to the event log if they were logged,
    // and subsequently free the list.  If logging wasn't enabled,
    // the list should be empty.
    //

    if (EnumContext.EnableLogging != FALSE) {
        PnpLogVetoInformation(EnumContext.TargetDeviceInstance,
                              &EnumContext.VetoProcessList);

        PnpFreeVetoInformation(&EnumContext.VetoProcessList);
    }

    PNP_ASSERT(IsListEmpty(&EnumContext.VetoProcessList));

    return Status;
}

