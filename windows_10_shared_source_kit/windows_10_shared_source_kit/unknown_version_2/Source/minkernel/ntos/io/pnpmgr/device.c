/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    devnode.c

Abstract:

    This file contains routines to maintain our private device node list.

Author:

    Forrest Foltz (forrestf) 27-Mar-1996

Revision History:

    Modified for nt kernel.

--*/

#include "pnpmgrp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PipAllocateDeviceNode)
#pragma alloc_text(PAGE, PipForAllDeviceNodes)
#pragma alloc_text(PAGE, PipForDeviceNodeSubtree)
#pragma alloc_text(PAGE, PipForAllChildDeviceNodes)
#pragma alloc_text(PAGE, PipForAllDeviceNodesCallback)
#pragma alloc_text(PAGE, IopDestroyDeviceNode)
#pragma alloc_text(PAGE, PpDevNodeLockTree)
#pragma alloc_text(PAGE, PpDevNodeUnlockTree)
#pragma alloc_text(PAGE, PipIsDevNodeRootEnumerated)
#pragma alloc_text(PAGE, PipIsProblemReadonly)
#pragma alloc_text(PAGE, PipSetDevNodeProblem)
#pragma alloc_text(PAGE, PipClearDevNodeProblem)
#pragma alloc_text(PAGE, PipSetDevNodeFlags)
#pragma alloc_text(PAGE, PipClearDevNodeFlags)
#pragma alloc_text(PAGE, PipSetDevNodeUserFlags)
#pragma alloc_text(PAGE, PipClearDevNodeUserFlags)
#pragma alloc_text(PAGE, IopReleaseResources)
#pragma alloc_text(PAGE, IopWriteAllocatedResourcesToRegistry)
#pragma alloc_text(PAGE, IopReleaseDeviceResources)
#pragma alloc_text(PAGE, IopUncacheInterfaceInformation)
#pragma alloc_text(PAGE, PnpAllocateDeviceInstancePath)
#pragma alloc_text(PAGE, PnpCopyDeviceInstancePath)
#pragma alloc_text(PAGE, PnpFreeDeviceInstancePath)
//#pragma alloc_text(NONPAGE, PipAreDriversLoaded)
//#pragma alloc_text(NONPAGE, PipAreDriversLoadedWorker)
//#pragma alloc_text(NONPAGE, PipIsDevNodeDNStarted)
//#pragma alloc_text(NONPAGE, PpDevNodeInsertIntoTree)
//#pragma alloc_text(NONPAGE, PpDevNodeRemoveFromTree)
//#pragma alloc_text(NONPAGE, PipRestoreDevNodeState)
//#pragma alloc_text(NONPAGE, PipSetDevNodeState)

#if DBG

#pragma alloc_text(PAGE, PpDevNodeAssertLockLevel)

#endif // DBG

#endif // ALLOC_PRAGMA


LOGICAL
PipAreDriversLoaded(
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This routine determines whether a devnode should be treated as if it has
    drivers attached to the PDO's stack (ie it's been added.)

Arguments:

    DeviceNode - Device node to examine.

Return Value:

    TRUE if drivers are loaded, FALSE otherwise.

--*/
{

    return PipAreDriversLoadedWorker(
        DeviceNode->State,
        DeviceNode->PreviousState
        );
}

LOGICAL
PipAreDriversLoadedWorker(
    __in PNP_DEVNODE_STATE    CurrentNodeState,
    __in PNP_DEVNODE_STATE    PreviousNodeState
    )
/*++

Routine Description:

    This routine determines whether a devnode should be treated as if it has
    drivers attached to the PDO's stack (ie it's been added.)

Arguments:

    CurrentNodeState - Current state of device node to examine.

    PreviousNodeState - Previous state of device node to examine.

Return Value:

    TRUE if drivers are loaded, FALSE otherwise.

--*/
{

    switch(CurrentNodeState) {

        case DeviceNodeDriversAdded:
        case DeviceNodeResourcesAssigned:
        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:
        case DeviceNodeStarted:
        case DeviceNodeQueryStopped:
        case DeviceNodeStopped:
        case DeviceNodeRestartCompletion:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeQueryRemoved:
        case DeviceNodeRemovePendingCloses:
        case DeviceNodeDeletePendingCloses:
        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeStartPending:
        case DeviceNodeEnumeratePending:
            return TRUE;

        case DeviceNodeAwaitingQueuedDeletion:
            return PipAreDriversLoadedWorker(
                PreviousNodeState,
                DeviceNodeUnspecified
                );

        case DeviceNodeUninitialized:
        case DeviceNodeInitialized:
        case DeviceNodeRemoved:
            return FALSE;

        case DeviceNodeDeleted:
            //
            // This can be seen by user mode because we defer delinking devices
            // from the tree during removal.
            //
            return FALSE;

        case DeviceNodeUnspecified:
        default:
            PNP_ASSERT(0);
            return FALSE;
    }
}

LOGICAL
PipIsDevNodeDNStarted(
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This routine takes a devnode and determines whether the devnode should
    have the user mode DN_STARTED bit set.

Arguments:

    DeviceNode - Device node to examine.

Return Value:

    TRUE if the devnode should be considered started, FALSE otherwise.

--*/
{

    switch (DeviceNode->State) {

        case DeviceNodeStartPending:
        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:
        case DeviceNodeStarted:
        case DeviceNodeQueryStopped:
        case DeviceNodeEnumeratePending:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeStopped:
        case DeviceNodeRestartCompletion:
            return TRUE;

        case DeviceNodeUninitialized:
        case DeviceNodeInitialized:
        case DeviceNodeDriversAdded:
        case DeviceNodeResourcesAssigned:
        case DeviceNodeRemoved:
        case DeviceNodeQueryRemoved:
        case DeviceNodeRemovePendingCloses:
        case DeviceNodeDeletePendingCloses:
        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeAwaitingQueuedDeletion:
            return FALSE;

        case DeviceNodeDeleted:
            //
            // This can be seen by user mode because we defer delinking devices
            // from the tree during removal.
            //
            return FALSE;

        case DeviceNodeUnspecified:
        default:
            PNP_ASSERT(0);
            return FALSE;
    }
}

LOGICAL
PipIsDevNodeRootEnumerated(
    __in PDEVICE_NODE DeviceNode
    )
{
    if (_CmIsRootEnumeratedDevice(PnpGetDeviceInstancePath(DeviceNode)->Buffer)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

VOID
PipClearDevNodeProblem(
    __in PDEVICE_NODE DeviceNode
    )
{
    HPNPOPCTX hPnpOpCtx = NULL;
    ULONG OldProblem = DeviceNode->Problem;
    NTSTATUS OldProblemStatus = DeviceNode->ProblemStatus;

    PAGED_CODE();

    if ((DeviceNode->Flags & DNF_HAS_PROBLEM) ||
        (DeviceNode->Problem != 0)) {

        PiPnpRtlBeginOperation(&hPnpOpCtx); // If this fails, then the events will not be batched.

        PipClearDevNodeFlags(DeviceNode, DNF_HAS_PROBLEM);
        DeviceNode->Problem = 0;
        DeviceNode->ProblemStatus = STATUS_SUCCESS;

        if (!PnpIsDeviceInstancePathEmpty(DeviceNode)) {

            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_PROBLEM);

            if (DeviceNode->ProblemStatus != OldProblemStatus) {
                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_PROBLEM_STATUS);
            }

            PnpTraceClearDevNodeProblem(PnpGetDeviceInstancePath(DeviceNode),
                                        &(DeviceNode->ServiceName),
                                        OldProblem,
                                        OldProblemStatus);

            if ((OldProblem == CM_PROB_DISABLED) &&
                (SeAuditingPlugAndPlayEvents() != FALSE)) {
                PiAuditDeviceEnableDisableAction(PnpGetDeviceInstancePath(DeviceNode),
                                                 OldProblem,
                                                 DeviceNode->Problem,
                                                 TRUE);
            }
        }
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }
}

VOID
PipSetDevNodeProblem(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG        Problem,
    __in NTSTATUS     ProblemStatus
    )
{
    HPNPOPCTX hPnpOpCtx = NULL;
    ULONG OldProblem = DeviceNode->Problem;
    NTSTATUS OldProblemStatus = DeviceNode->ProblemStatus;

    PAGED_CODE();

    PNP_ASSERT((DeviceNode->State != DeviceNodeUninitialized) ||
               (!(DeviceNode->Flags & DNF_ENUMERATED)) ||
               (Problem == CM_PROB_INVALID_DATA) ||
               (Problem == CM_PROB_REGISTRY) ||
               (Problem == CM_PROB_WAITING_ON_DEPENDENCY));

    PNP_ASSERT(DeviceNode->State != DeviceNodeStarted);
    PNP_ASSERT(Problem != 0);

    if ((!(DeviceNode->Flags & DNF_HAS_PROBLEM)) ||
        (DeviceNode->Problem != Problem) ||
        (DeviceNode->ProblemStatus != ProblemStatus)) {

        PiPnpRtlBeginOperation(&hPnpOpCtx); // If this fails, then the events will not be batched.

        PipSetDevNodeFlags(DeviceNode, DNF_HAS_PROBLEM);
        DeviceNode->Problem = Problem;
        DeviceNode->ProblemStatus = ProblemStatus;

        if (!PnpIsDeviceInstancePathEmpty(DeviceNode)) {

            if (DeviceNode->Problem != OldProblem) {
                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_PROBLEM);
            }

            if (DeviceNode->ProblemStatus != OldProblemStatus) {
                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_PROBLEM_STATUS);
            }

            PnpTraceSetDevNodeProblem(PnpGetDeviceInstancePath(DeviceNode),
                                      &(DeviceNode->ServiceName),
                                      DeviceNode->Problem,
                                      DeviceNode->ProblemStatus,
                                      OldProblem,
                                      OldProblemStatus);

            if ((DeviceNode->Problem == CM_PROB_DISABLED) &&
                (SeAuditingPlugAndPlayEvents() != FALSE)) {
                PiAuditDeviceEnableDisableAction(PnpGetDeviceInstancePath(DeviceNode),
                                                 OldProblem,
                                                 DeviceNode->Problem,
                                                 TRUE);
            }
        }
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }
}

VOID
PipSetDevNodeState(
    __in  PDEVICE_NODE        DeviceNode,
    __in  PNP_DEVNODE_STATE   State,
    __out_opt PNP_DEVNODE_STATE   *OldState
    )
/*++

Routine Description:

    This routine sets a devnodes state and optional returns the prior state.
    The prior state is saved and can be restored via PipRestoreDevNodeState.

Arguments:

    DeviceNode - Device node to update state.

    State - State to place devnode in.

    OldState - Optionally receives prior state of devnode.

Return Value:

    None.

--*/
{
    PNP_DEVNODE_STATE previousState;
    LOGICAL previousDriversLoaded = FALSE;
    LOGICAL previousDevNodeStarted = FALSE;
    KIRQL oldIrql;
    BOOLEAN stateChanged = FALSE;

    PNP_ASSERT(KeGetCurrentIrql() <= APC_LEVEL);  // This function in non-paged but should only be called from paged code.

    PNP_ASSERT(State != DeviceNodeQueryStopped || DeviceNode->State == DeviceNodeStarted);

#if DBG

    if ((State == DeviceNodeDeleted) ||
        (State == DeviceNodeDeletePendingCloses)) {
        PNP_ASSERT(!(DeviceNode->Flags & DNF_ENUMERATED));
    }

#endif

    KeAcquireSpinLock(&PnpSpinLock, &oldIrql);

    previousState = DeviceNode->State;
    if (DeviceNode->State != State) {
        previousDriversLoaded = PipAreDriversLoaded(DeviceNode);
        previousDevNodeStarted = PipIsDevNodeDNStarted(DeviceNode);

        //
        // Update the devnode's current and previous state.
        //
        DeviceNode->State = State;
        DeviceNode->PreviousState = previousState;

        //
        // Push prior state onto the history stack.
        //
        DeviceNode->StateHistory[DeviceNode->StateHistoryEntry] = previousState;
        DeviceNode->StateHistoryEntry++;
        DeviceNode->StateHistoryEntry %= STATE_HISTORY_SIZE;

        stateChanged = TRUE;
    }

    KeReleaseSpinLock(&PnpSpinLock, oldIrql);

    if (stateChanged) {

        IopDbgPrint((IOP_INFO_LEVEL,
                     "%wZ: %s => %s\n",
                     PnpGetDeviceInstancePath(DeviceNode),
                     PP_DEVNODESTATE_NAME(previousState),
                     PP_DEVNODESTATE_NAME(State)));

        //
        // Don't generate a notification if no properties of this device have
        // changed.  The device state potentially influences
        // DEVPKEY_Device_DevNodeStatus.  The bits in that value that depend on
        // the device state are:
        //
        // DN_DRIVER_LOADED: determined using PipAreDriversLoaded
        //
        // DN_STARTED: determined using PipIsDevNodeDNStarted
        //
        if ((PnpGetDeviceInstancePath(DeviceNode)->Buffer) &&
            ((PipAreDriversLoaded(DeviceNode) != previousDriversLoaded) ||
             (PipIsDevNodeDNStarted(DeviceNode) != previousDevNodeStarted))) {

            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

            //
            // As an optimization, notifications for derived properties that
            // depend on the device's status are not based on the property
            // change event for the status.  They are based on property change
            // events for the individual bits in the status they care about.
            // Send relevant property change events here.
            //
            if (PipIsDevNodeDNStarted(DeviceNode) != previousDevNodeStarted) {
                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_STARTED);
            }
        }
    }

    if (ARGUMENT_PRESENT(OldState)) {

        *OldState = previousState;
    }

    if (State == DeviceNodeDeleted) {

        PnpRemoveDeviceActionRequests(DeviceNode->PhysicalDeviceObject);
    }
}

VOID
PipRestoreDevNodeState(
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This routine restores a devnodes state to the state pushed by the last
    PipSetDevNodeState call. This function can only be called once for each
    call to PipSetDevNodeState.

Arguments:

    DeviceNode - Device node to restore state.

Return Value:

    None.

--*/
{
    PNP_DEVNODE_STATE previousState, newState;
    LOGICAL previousDriversLoaded = FALSE;
    LOGICAL previousDevNodeStarted = FALSE;
    KIRQL oldIrql;

    PNP_ASSERT(KeGetCurrentIrql() <= APC_LEVEL);  // This function in non-paged but should only be called from paged code.

    PNP_ASSERT((DeviceNode->State == DeviceNodeQueryRemoved) ||
           (DeviceNode->State == DeviceNodeQueryStopped) ||
           (DeviceNode->State == DeviceNodeAwaitingQueuedRemoval) ||
           (DeviceNode->State == DeviceNodeAwaitingQueuedDeletion));

    KeAcquireSpinLock(&PnpSpinLock, &oldIrql);

    previousDriversLoaded = PipAreDriversLoaded(DeviceNode);
    previousDevNodeStarted = PipIsDevNodeDNStarted(DeviceNode);

    //
    // Update the devnode's state.
    //
    previousState = DeviceNode->State;
    newState = DeviceNode->State = DeviceNode->PreviousState;

    //
    // Push the old state onto the history stack.
    //
    DeviceNode->StateHistory[DeviceNode->StateHistoryEntry] = previousState;
    DeviceNode->StateHistoryEntry++;
    DeviceNode->StateHistoryEntry %= STATE_HISTORY_SIZE;

    //
    // Put a sentinel on the stack - restoring twice is a bug.
    //

#if DBG

    DeviceNode->PreviousState = DeviceNodeUnspecified;

#endif

    KeReleaseSpinLock(&PnpSpinLock, oldIrql);

    IopDbgPrint((IOP_INFO_LEVEL,
                 "%wZ: %s => %s\n",
                 PnpGetDeviceInstancePath(DeviceNode),
                 PP_DEVNODESTATE_NAME(previousState),
                 PP_DEVNODESTATE_NAME(newState)));

    //
    // Don't generate a notification if no properties of this device have
    // changed.  The device state potentially influences
    // DEVPKEY_Device_DevNodeStatus.  The bits in that value that depend on the
    // device state are:
    //
    // DN_DRIVER_LOADED: determined using PipAreDriversLoaded
    //
    // DN_STARTED: determined using PipIsDevNodeDNStarted
    //
    if ((PnpGetDeviceInstancePath(DeviceNode)->Buffer) &&
        ((PipAreDriversLoaded(DeviceNode) != previousDriversLoaded) ||
         (PipIsDevNodeDNStarted(DeviceNode) != previousDevNodeStarted))) {

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

        //
        // As an optimization, notifications for derived properties that
        // depend on the device's status are not based on the property
        // change event for the status.  They are based on property change
        // events for the individual bits in the status they care about.
        // Send relevant property change events here.
        //
        if (PipIsDevNodeDNStarted(DeviceNode) != previousDevNodeStarted) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_STARTED);
        }
    }
}

//
// Devnode status is derived from these DNF flags
//

const ULONG PipDevnodeStatusFlags = DNF_HAS_PRIVATE_PROBLEM |
                                    DNF_HAS_PROBLEM |
                                    DNF_DRIVER_BLOCKED |
                                    DNF_LEGACY_DRIVER |
                                    DNF_CHILD_WITH_INVALID_ID;

VOID PipSetDevNodeFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    )
{
    ULONG OldFlags = DeviceNode->Flags;

    PAGED_CODE();

    DeviceNode->Flags |= Flags;

    if (((DeviceNode->Flags ^ OldFlags) & PipDevnodeStatusFlags) &&
        PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

        //
        // As an optimization, notifications for derived properties that
        // depend on the device's status are not based on the property
        // change event for the status.  They are based on property change
        // events for the individual bits in the status they care about.
        // Send relevant property change events here.
        //

        if (((DeviceNode->Flags ^ OldFlags) & DNF_HAS_PRIVATE_PROBLEM)) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_PRIVATE_PROBLEM);
        }
        if (((DeviceNode->Flags ^ OldFlags) & DNF_HAS_PROBLEM)) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_HAS_PROBLEM);
        }
    }
}

VOID PipClearDevNodeFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    )
{
    ULONG OldFlags = DeviceNode->Flags;

    PAGED_CODE();

    DeviceNode->Flags &= ~Flags;

    if (((DeviceNode->Flags ^ OldFlags) & PipDevnodeStatusFlags) &&
        PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

        //
        // As an optimization, notifications for derived properties that
        // depend on the device's status are not based on the property
        // change event for the status.  They are based on property change
        // events for the individual bits in the status they care about.
        // Send relevant property change events here.
        //

        if (((DeviceNode->Flags ^ OldFlags) & DNF_HAS_PRIVATE_PROBLEM)) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_PRIVATE_PROBLEM);
        }
        if (((DeviceNode->Flags ^ OldFlags) & DNF_HAS_PROBLEM)) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_HAS_PROBLEM);
        }
    }
}

//
// Devnode status is derived from these DNUF flags
//

const ULONG PipDevnodeStatusUserFlags = DNUF_WILL_BE_REMOVED |
                                        DNUF_DONT_SHOW_IN_UI |
                                        DNUF_DEVICE_DISCONNECTED |
                                        DNUF_NEED_RESTART;

VOID PipSetDevNodeUserFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    )
{
    ULONG OldUserFlags = DeviceNode->UserFlags;

    PAGED_CODE();

    DeviceNode->UserFlags |= Flags;

    if (((DeviceNode->UserFlags ^ OldUserFlags) & PipDevnodeStatusUserFlags) &&
        PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

        //
        // As an optimization, notifications for derived properties that
        // depend on the device's status are not based on the property
        // change event for the status.  They are based on property change
        // events for the individual bits in the status they care about.
        // Send relevant property change events here.
        //

        if ((DeviceNode->UserFlags ^ OldUserFlags) & DNUF_DEVICE_DISCONNECTED) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_DEVICE_DISCONNECTED);
        }
        if ((DeviceNode->UserFlags ^ OldUserFlags) & DNUF_NEED_RESTART) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_NEED_RESTART);
        }
    }
}

VOID PipClearDevNodeUserFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    )
{
    ULONG OldUserFlags = DeviceNode->UserFlags;

    PAGED_CODE();

    DeviceNode->UserFlags &= ~Flags;

    if (((DeviceNode->UserFlags ^ OldUserFlags) & PipDevnodeStatusUserFlags) &&
        PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);

        //
        // As an optimization, notifications for derived properties that
        // depend on the device's status are not based on the property
        // change event for the status.  They are based on property change
        // events for the individual bits in the status they care about.
        // Send relevant property change events here.
        //

        if ((DeviceNode->UserFlags ^ OldUserFlags) & DNUF_DEVICE_DISCONNECTED) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_DEVICE_DISCONNECTED);
        }
        if ((DeviceNode->UserFlags ^ OldUserFlags) & DNUF_NEED_RESTART) {
            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS_NEED_RESTART);
        }
    }
}

LOGICAL
PipIsProblemReadonly(
    __in  PDEVICE_NODE DeviceNode,
    __in  ULONG Problem
    )
/*++

Routine Description:

    This routine returns TRUE if the specified CM_PROB code cannot be cleared
    by user mode, FALSE otherwise.

Arguments:

    Problem - CM_PROB_...

Return Value:

    TRUE/FALSE.

--*/
{
    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    switch(Problem) {

        case CM_PROB_OUT_OF_MEMORY:

            //
            // Only non-legacy device nodes can be reset. Legacy devices are
            // non resettable due to IoReportResourceUsagePath
            //

            if ((DeviceNode->Flags & DNF_LEGACY_RESOURCE_DEVICENODE) != 0) {
                return TRUE;

            } else {
                return FALSE;
            }

            break;

        case CM_PROB_NORMAL_CONFLICT:
        case CM_PROB_PARTIAL_LOG_CONF:
        case CM_PROB_DEVICE_NOT_THERE:
        case CM_PROB_HARDWARE_DISABLED:
        case CM_PROB_TRANSLATION_FAILED:
        case CM_PROB_NO_SOFTCONFIG:
        case CM_PROB_BIOS_TABLE:
        case CM_PROB_IRQ_TRANSLATION_FAILED:
        case CM_PROB_DUPLICATE_DEVICE:
        case CM_PROB_SYSTEM_SHUTDOWN:
        case CM_PROB_HELD_FOR_EJECT:
        case CM_PROB_REGISTRY_TOO_LARGE:
        case CM_PROB_INVALID_DATA:
        case CM_PROB_SETPROPERTIES_FAILED:
        case CM_PROB_DEVICE_RESET:

            return TRUE;

        case CM_PROB_FAILED_INSTALL:
        case CM_PROB_FAILED_ADD:
        case CM_PROB_FAILED_START:
        case CM_PROB_NOT_CONFIGURED:
        case CM_PROB_NEED_RESTART:
        case CM_PROB_REINSTALL:
        case CM_PROB_REGISTRY:
        case CM_PROB_DISABLED:
        case CM_PROB_DISABLED_SERVICE:
        case CM_PROB_FAILED_DRIVER_ENTRY:
        case CM_PROB_DRIVER_FAILED_PRIOR_UNLOAD:
        case CM_PROB_DRIVER_FAILED_LOAD:
        case CM_PROB_DRIVER_SERVICE_KEY_INVALID:
        case CM_PROB_LEGACY_SERVICE_NO_DEVICES:
        case CM_PROB_HALTED:
        case CM_PROB_FAILED_POST_START:
        case CM_PROB_WILL_BE_REMOVED:
        case CM_PROB_DRIVER_BLOCKED:
        case CM_PROB_WAITING_ON_DEPENDENCY:
        case CM_PROB_UNSIGNED_DRIVER:
        case CM_PROB_USED_BY_DEBUGGER:

            return FALSE;

        case CM_PROB_PHANTOM:

            //
            // Should never see in kernel mode
            //

        case CM_PROB_DEVLOADER_FAILED:
        case CM_PROB_DEVLOADER_NOT_FOUND:
        case CM_PROB_REENUMERATION:
        case CM_PROB_VXDLDR:
        case CM_PROB_NOT_VERIFIED:
        case CM_PROB_LIAR:
        case CM_PROB_FAILED_FILTER:
        case CM_PROB_MOVED:
        case CM_PROB_TOO_EARLY:
        case CM_PROB_NO_VALID_LOG_CONF:
        case CM_PROB_UNKNOWN_RESOURCE:
        case CM_PROB_ENTRY_IS_WRONG_TYPE:
        case CM_PROB_LACKED_ARBITRATOR:
        case CM_PROB_BOOT_CONFIG_CONFLICT:
        case CM_PROB_DEVLOADER_NOT_READY:
        case CM_PROB_CANT_SHARE_IRQ:

            //
            // Win9x specific
            //

        default:
            PNP_ASSERT(0);

            //
            // We return TRUE in this path because that prevents these problems
            // from being set on devnodes (SetDeviceProblem won't allow usage
            // of ReadOnly problems)
            //
            return TRUE;
    }
}

NTSTATUS
PipAllocateDeviceNode(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PDEVICE_NODE *DeviceNode
    )
/*++

Routine Description:

    This function allocates a device node from nonpaged pool and initializes
    the fields which do not require to hold lock to do so.  Since adding
    the device node to pnp mgr's device node tree requires acquiring lock,
    this routine does not add the device node to device node tree.

Arguments:

    PhysicalDeviceObject - Supplies a pointer to its corresponding physical device
        object.

Return Value:

    a pointer to the newly created device node. Null is returned if failed.

--*/
{

    PAGED_CODE();

    *DeviceNode = ExAllocatePoolWithTag(
                    NonPagedPoolNx,
                    sizeof(DEVICE_NODE),
                    IOP_DNOD_TAG
                    );

    if (*DeviceNode == NULL ){

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    InterlockedIncrement((LONG *)&IopNumberDeviceNodes);

    RtlZeroMemory(*DeviceNode, sizeof(DEVICE_NODE));

    (*DeviceNode)->InterfaceType = InterfaceTypeUndefined;
    (*DeviceNode)->BusNumber = (ULONG)-1;
    (*DeviceNode)->ChildInterfaceType = InterfaceTypeUndefined;
    (*DeviceNode)->ChildBusNumber = (ULONG)-1;
    (*DeviceNode)->ChildBusTypeIndex = (USHORT)-1;
    (*DeviceNode)->State = DeviceNodeUninitialized;
    (*DeviceNode)->DisableableDepends = 0;
    (*DeviceNode)->DriverUnloadRetryCount = 0;
    (*DeviceNode)->PreviousParent = NULL;
    (*DeviceNode)->DeletedChildren = 0;
    (*DeviceNode)->NumaNodeIndex = PNP_NUMA_NODE_UNKNOWN;

    PpHotSwapInitRemovalPolicy(*DeviceNode);

    InitializeListHead(&(*DeviceNode)->DeviceArbiterList);
    InitializeListHead(&(*DeviceNode)->DeviceTranslatorList);

    if (PhysicalDeviceObject){

        (*DeviceNode)->PhysicalDeviceObject = PhysicalDeviceObject;
        PhysicalDeviceObject->DeviceObjectExtension->DeviceNode = (PVOID)*DeviceNode;
        PhysicalDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }

    InitializeListHead(&(*DeviceNode)->TargetDeviceNotify);
    InitializeListHead(&(*DeviceNode)->DockInfo.ListEntry);
    InitializeListHead(&(*DeviceNode)->PendedSetInterfaceState);
    InitializeListHead(&(*DeviceNode)->LegacyBusListEntry);
    InitializeListHead(&(*DeviceNode)->Notify.PowerChildren);
    InitializeListHead(&(*DeviceNode)->Notify.PowerParents);

    if (PnpSystemHiveTooLarge) {

        //
        // This flag is currently only used for informational purposes.  Print
        // out a warning to that effect.
        //

        IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                     "The system hive size is nearing its limit.\n"));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PipForAllDeviceNodes(
    __in PENUM_CALLBACK Callback,
    __in_opt PVOID Context
    )
/*++

Routine Description:

    This function walks the device node tree and invokes the caller specified
    'Callback' function for each device node.

    Note, this routine (or its worker routine) traverses the tree in a top
    down manner.

Arguments:

    Callback - Supplies the call back routine for each device node.

    Context - Supplies a parameter/context for the callback function.

Return Value:

    Status returned from Callback, if not successfull then the tree walking stops.

--*/
{
    PAGED_CODE();

    return PipForDeviceNodeSubtree(IopRootDeviceNode, Callback, Context);
}


NTSTATUS
PipForDeviceNodeSubtree(
    __in PDEVICE_NODE     DeviceNode,
    __in PENUM_CALLBACK   Callback,
    __in_opt PVOID            Context
    )
/*++

Routine Description:

    This function walks the device node tree under but not including the passed
    in device node and perform caller specified 'Callback' function for each
    device node.

    Note, this routine (or its worker routine) traverses the tree in a top
    down manner.

Arguments:

    Callback - Supplies the call back routine for each device node.

    Context - Supplies a parameter/context for the callback function.

Return Value:

    Status returned from Callback, if not successfull then the tree walking stops.

--*/
{
    ENUM_CONTEXT enumContext;
    NTSTATUS status;

    PAGED_CODE();

    enumContext.CallersCallback = Callback;
    enumContext.CallersContext = Context;

    //
    // Start with a pointer to the root device node, recursively examine all the
    // children until we the callback function says stop or we've looked at all
    // of them.
    //
    PpDevNodeLockTree(PPL_SIMPLE_READ);

    status = PipForAllChildDeviceNodes(DeviceNode,
                                       PipForAllDeviceNodesCallback,
                                       (PVOID)&enumContext );


    PpDevNodeUnlockTree(PPL_SIMPLE_READ);
    return status;
}


NTSTATUS
PipForAllChildDeviceNodes(
    __in PDEVICE_NODE Parent,
    __in PENUM_CALLBACK Callback,
    __in PVOID Context
    )

/*++

Routine Description:

    This function walks the Parent's device node subtree and perform caller specified
    'Callback' function for each device node under Parent.

    Note, befor calling this rotuine, callers must acquire the enumeration mutex
    of the 'Parent' device node to make sure its children won't go away unless the
    call tells them to.

Arguments:

    Parent - Supplies a pointer to the device node whose subtree is to be walked.

    Callback - Supplies the call back routine for each device node.

    Context - Supplies a parameter/context for the callback function.

Return Value:

    NTSTATUS value.

--*/

{
    PDEVICE_NODE nextChild = Parent->Child;
    PDEVICE_NODE child;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    //
    // Process siblings until we find the end of the sibling list or
    // the Callback() returns FALSE.  Set result = TRUE at the top of
    // the loop so that if there are no siblings we will return TRUE,
    // e.g. Keep Enumerating.
    //
    // Note, we need to find next child before calling Callback function
    // in case the current child is deleted by the Callback function.
    //

    while (nextChild && NT_SUCCESS(status)) {
        child = nextChild;
        nextChild = child->Sibling;
        status = Callback(child, Context);
    }

    return status;
}

NTSTATUS
PipForAllDeviceNodesCallback(
    __in PDEVICE_NODE DeviceNode,
    __in PVOID Context
    )

/*++

Routine Description:

    This function is the worker routine for PipForAllChildDeviceNodes routine.

Arguments:

    DeviceNode - Supplies a pointer to the device node whose subtree is to be walked.

    Context - Supplies a context which contains the caller specified call back
              function and parameter.

Return Value:

    NTSTATUS value.

--*/

{
    PENUM_CONTEXT enumContext;
    NTSTATUS status;

    PAGED_CODE();

    enumContext = (PENUM_CONTEXT)Context;

    //
    // First call the caller's callback for this devnode
    //

    status =
        enumContext->CallersCallback(DeviceNode, enumContext->CallersContext);

    if (NT_SUCCESS(status)) {

        //
        // Now enumerate the children, if any.
        //
        if (DeviceNode->Child) {

            status = PipForAllChildDeviceNodes(
                                        DeviceNode,
                                        PipForAllDeviceNodesCallback,
                                        Context);
        }
    }

    return status;
}
VOID
IopDestroyDeviceNode(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function is invoked by IopDeleteDevice to clean up the device object's
    device node structure.

Arguments:

    DeviceNode - Supplies a pointer to the device node whose subtree is to be walked.

    Context - Supplies a context which contains the caller specified call back
              function and parameter.

Return Value:

    NTSTATUS value.

--*/

{

#if DBG

    PDEVICE_OBJECT dbgDeviceObject;

#endif

    PAGED_CODE();

    if (DeviceNode) {

        PNP_ASSERT((DeviceNode->PoIrpManager.DeviceIrpQueue.CurrentIrp == NULL) &&
               (DeviceNode->PoIrpManager.DeviceIrpQueue.PendingIrpList == NULL));
        PNP_ASSERT((DeviceNode->PoIrpManager.SystemIrpQueue.CurrentIrp == NULL) &&
               (DeviceNode->PoIrpManager.SystemIrpQueue.PendingIrpList == NULL));

        if ((DeviceNode->PhysicalDeviceObject->Flags & DO_BUS_ENUMERATED_DEVICE) &&
            DeviceNode->Parent != NULL)  {

            PNP_SAVE_DEVNODE_TO_TRIAGE_DUMP(DeviceNode);
            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                          PNP_ERR_ACTIVE_PDO_FREED,
                          (ULONG_PTR)DeviceNode->PhysicalDeviceObject,
                          0,
                          0);
        }
        if (DeviceNode->Flags & DNF_LEGACY_RESOURCE_DEVICENODE) {
            //
            // Release the resources this device consumes (the devicenode will
            // get deleted after the release). Basically cleanup after bad
            // (legacy) drivers.
            //
            IopLegacyResourceAllocation(ArbiterRequestUndefined,
                                        PnpDriverObject,
                                        DeviceNode->PhysicalDeviceObject,
                                        NULL,
                                        NULL);
            return;
        }

#if DBG

        //
        // If Only Parent is NOT NULL, most likely the driver forgot to
        // release resources before deleting its FDO.  (The driver previously
        // call legacy assign resource interface.)
        //

        PNP_ASSERT(DeviceNode->Child == NULL &&
               DeviceNode->Sibling == NULL &&
               DeviceNode->LastChild == NULL
               );

        PNP_ASSERT(DeviceNode->DockInfo.SerialNumber == NULL &&
               IsListEmpty(&DeviceNode->DockInfo.ListEntry));

        if (DeviceNode->PhysicalDeviceObject->Flags & DO_BUS_ENUMERATED_DEVICE) {
            PNP_ASSERT (DeviceNode->Parent == 0);
        }

        if (DeviceNode->PreviousResourceList) {
            ExFreePool(DeviceNode->PreviousResourceList);
        }
        if (DeviceNode->PreviousResourceRequirements) {
            ExFreePool(DeviceNode->PreviousResourceRequirements);
        }

        //
        // device should not appear to be not-disableable if/when we get here
        // if either of these two lines PNP_ASSERT, email: jamiehun
        //

        PNP_ASSERT((DeviceNode->UserFlags & DNUF_NOT_DISABLEABLE) == 0);
        PNP_ASSERT(DeviceNode->DisableableDepends == 0);

        if (PnpIsDeviceInstancePathEmpty(DeviceNode) == FALSE) {

            dbgDeviceObject = PnpDeviceObjectFromDeviceInstance(PnpGetDeviceInstancePath(DeviceNode));

            if (dbgDeviceObject) {

                PNP_ASSERT(dbgDeviceObject != DeviceNode->PhysicalDeviceObject);
                ObDereferenceObject(dbgDeviceObject);
            }
        }

#endif
        if (DeviceNode->DuplicatePDO) {
            ObDereferenceObject(DeviceNode->DuplicatePDO);
        }
        if (DeviceNode->ServiceName.Length != 0) {
            ExFreePool(DeviceNode->ServiceName.Buffer);
        }

        PnpFreeDeviceInstancePath(DeviceNode);
        IopAcquireResourceListLock(&PiResourceListLock);
        if (DeviceNode->ResourceRequirements) {
            ExFreePool(DeviceNode->ResourceRequirements);
        }
        IopReleaseResourceListLock(&PiResourceListLock);
        //
        // Dereference all the arbiters and translators on this PDO.
        //
        IopUncacheInterfaceInformation(DeviceNode->PhysicalDeviceObject, FALSE);

        //
        // Release any pended IoSetDeviceInterface structures
        //

        while (!IsListEmpty(&DeviceNode->PendedSetInterfaceState)) {

            PPENDING_SET_INTERFACE_STATE entry;

            entry = (PPENDING_SET_INTERFACE_STATE)RemoveHeadList(&DeviceNode->PendedSetInterfaceState);

            ExFreePool(entry->LinkName.Buffer);

            ExFreePool(entry);
        }

        //
        // Clear the rebalance tracing data-structure.
        //

        if (DeviceNode->RebalanceContext != NULL) {
            ExFreePoolWithTag(DeviceNode->RebalanceContext,
                              PNP_POOLTAG_REBALANCE_TRACE_CONTEXT);
        }

        DeviceNode->PhysicalDeviceObject->DeviceObjectExtension->DeviceNode = NULL;
        ExFreePool(DeviceNode);
        InterlockedDecrement((LONG *)&IopNumberDeviceNodes);
    }
}

VOID
PpDevNodeInsertIntoTree(
    __in PDEVICE_NODE     ParentNode,
    __in PDEVICE_NODE     DeviceNode
    )
/*++

Routine Description:

    This function is called to insert a new devnode into the device tree.

    Note that there are two classes of callers:
        PnP callers
        Legacy callers

    All PnP callers hold the device tree lock. Legacy callers however come in
    with no locks, as they might be brought into being due to a PnP event. To
    deal with the later case, inserts are atomic and legacy callers can never
    remove themselves from the tree.

Arguments:

    ParentNode - Supplies a pointer to the device node's parent

    DeviceNode - Supplies a pointer to the device node which needs to be
                 inserted into the tree.

Return Value:

    None.

--*/
{
    ULONG depth;
    KIRQL oldIrql;

    //
    // Acquire spinlock to deal with legacy/PnP synchronization.
    //
    KeAcquireSpinLock(&PnpSpinLock, &oldIrql);

    //
    // Determine the depth of the devnode.
    //
    depth = ParentNode->Level + 1;
    DeviceNode->Level = depth;

    //
    // Update the maximum depth of the tree.
    //
    if (depth > IopMaxDeviceNodeLevel) {
        IopMaxDeviceNodeLevel = depth;
    }

    //
    // Put this devnode at the end of the parent's list of children. Note that
    // the Child/Sibling fields are really the last things to be updated. This
    // has to be done as walkers of the tree hold no locks that protect the
    // tree from legacy inserts.
    //
    DeviceNode->Parent = ParentNode;
    KeMemoryBarrier();
    if (ParentNode->LastChild) {
        PNP_ASSERT(ParentNode->LastChild->Sibling == NULL);
        ParentNode->LastChild->Sibling = DeviceNode;
        ParentNode->LastChild = DeviceNode;
    } else {
        PNP_ASSERT(ParentNode->Child == NULL);
        ParentNode->Child = ParentNode->LastChild = DeviceNode;
    }

    KeReleaseSpinLock(&PnpSpinLock, oldIrql);
}

BOOLEAN
PpDevNodeRemoveFromTree(
    __in PDEVICE_NODE     DeviceNode
    )
/*++

Routine Description:

    This function removes the device node from the device node tree

Arguments:

    DeviceNode      - Device node to remove

Return Value:

--*/
{

    PLIST_ENTRY     link;
    PDEVICE_NODE    *node;
    KIRQL           oldIrql;
    PPO_RELATION    powerRelation;
    BOOLEAN         bRemoved = FALSE;
    //
    // Acquire spinlock to deal with legacy/PnP synchronization.
    //

    KeAcquireSpinLock(&PnpSpinLock, &oldIrql);

    //
    // Unlink the pointer to this device node.  (If this is the
    // first entry, unlink it from the parents child pointer, else
    // remove it from the sibling list). Note that there are cases
    // where a device node may have already been unlinked.  The most
    // likely scenario is where a device was surprise failed, and then
    // either reported missing or an ancestor was reported missing.
    // In this case, ensure that a the device node is not unlinked again
    // to avoid touch NULL'd pointers.
    //

    if (DeviceNode->Parent != NULL) {
        node = &DeviceNode->Parent->Child;
        while (*node != DeviceNode) {
            node = &(*node)->Sibling;
        }
        *node = DeviceNode->Sibling;

        if (DeviceNode->Parent->Child == NULL) {
            DeviceNode->Parent->LastChild = NULL;
        } else {
            while (*node) {
                node = &(*node)->Sibling;
            }
            DeviceNode->Parent->LastChild = CONTAINING_RECORD(node, DEVICE_NODE, Sibling);
        }

        KeReleaseSpinLock(&PnpSpinLock, oldIrql);

        //
        // Remove this device node from Legacy Bus information table.
        //
        IopRemoveLegacyBusDeviceNode(DeviceNode);

        //
        // Orphan any outstanding device change notifications on these nodes.
        //

        PnpOrphanNotification(DeviceNode);

        bRemoved = TRUE;

    } else {
        KeReleaseSpinLock(&PnpSpinLock, oldIrql);
        IopDbgPrint((IOP_ENUMERATION_WARNING_LEVEL,
                     "PpDevNodeRemoveFromTree: Attempted to unlink previously unlinked devnode 0x%p\n",
                     DeviceNode));
    }

    //
    // Remove all power relations linked to this device node.
    //

    link = DeviceNode->Notify.PowerChildren.Flink;
    while (link != &DeviceNode->Notify.PowerChildren) {
        powerRelation = CONTAINING_RECORD(link, PO_RELATION, ParentLink);
        link = link->Flink;
        RemoveEntryList(&powerRelation->ChildLink);
        RemoveEntryList(&powerRelation->ParentLink);
        ExFreePoolWithTag(powerRelation, IOP_DPWR_TAG);
    }

    link = DeviceNode->Notify.PowerParents.Flink;
    while (link != &DeviceNode->Notify.PowerParents) {
        powerRelation = CONTAINING_RECORD(link, PO_RELATION, ChildLink);
        link = link->Flink;
        RemoveEntryList(&powerRelation->ChildLink);
        RemoveEntryList(&powerRelation->ParentLink);
        ExFreePoolWithTag(powerRelation, IOP_DPWR_TAG);
    }

    //
    // No longer linked
    //

    DeviceNode->Parent    = NULL;
    DeviceNode->Child     = NULL;
    DeviceNode->Sibling   = NULL;
    DeviceNode->LastChild = NULL;

    return bRemoved;
}

BOOLEAN
PpDevNodeLockTree(
    __in  PNP_LOCK_LEVEL  LockLevel
    )
/*++

Routine Description:

    This function acquires the tree lock with the appropriate level of
    restrictions.

Arguments:

    LockLevel:
        PPL_SIMPLE_READ         - Allows simple examination of the tree.

        PPL_TREEOP_ALLOW_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however.

        PPL_TREEOP_ALLOW_READS_NO_WAIT
                                - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however. This
                                  will not block on acquiring the PiEngineLock.
                                  If the engine lock cannot be acquired, this
                                  function will return false

        PPL_TREEOP_BLOCK_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads are also blocked.

        PPL_TREEOP_BLOCK_READS_FROM_ALLOW - Switch to PPL_TREEOP_BLOCK_READS
                                            when already in
                                            PPL_TREEOP_ALLOW_READS. Note that
                                            PpDevNodeUnlockTree must be
                                            subsequently called on both to
                                            release.

Return Value:

    TRUE if all locks were successfully acquired.  FALSE otherwise.

--*/
{
    ULONG refCount, remainingCount;
    BOOLEAN retVal = TRUE;

    PAGED_CODE();

    //
    // Block any attempt to suspend the thread via user mode.
    //
    KeEnterCriticalRegion();

    switch(LockLevel) {

        case PPL_SIMPLE_READ:
            ExAcquireResourceSharedLite(&IopDeviceTreeLock, TRUE);
            break;

        case PPL_TREEOP_ALLOW_READS:
            ExAcquireResourceExclusiveLite(&PiEngineLock, TRUE);
            ExAcquireResourceSharedLite(&IopDeviceTreeLock, TRUE);
            break;

        case PPL_TREEOP_ALLOW_READS_NO_WAIT:
            if (!ExAcquireResourceExclusiveLite(&PiEngineLock, FALSE)) {
                retVal = FALSE;
                goto clean0;
            }
            ExAcquireResourceSharedLite(&IopDeviceTreeLock, TRUE);
            break;

        case PPL_TREEOP_BLOCK_READS:
            ExAcquireResourceExclusiveLite(&PiEngineLock, TRUE);
            ExAcquireResourceExclusiveLite(&IopDeviceTreeLock, TRUE);
            break;

        case PPL_TREEOP_BLOCK_READS_FROM_ALLOW:

            //
            // Drop the tree lock and require exclusive.
            //
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiEngineLock));

            //
            // "Shared" is a subset of exclusive. ExIsResourceAcquiredShared
            // will return nonzero if it's owned exclusive. We flush out that
            // case here.
            //
            PNP_ASSERT(ExIsResourceAcquiredSharedLite(&IopDeviceTreeLock) &&
                   (!ExIsResourceAcquiredExclusiveLite(&IopDeviceTreeLock)));

            //
            // Drop the tree lock entirely.
            //
            refCount = ExIsResourceAcquiredSharedLite(&IopDeviceTreeLock);
            for(remainingCount = refCount; remainingCount; remainingCount--) {

                ExReleaseResourceLite(&IopDeviceTreeLock);
            }

            //
            // Grab it exclusively while keeping the original count.
            //
            for(remainingCount = refCount; remainingCount; remainingCount--) {

                ExAcquireResourceExclusiveLite(&IopDeviceTreeLock, TRUE);
            }
            break;

        default:
            PNP_ASSERT(0);
            break;
    }

  clean0:

    if (!retVal) {
        KeLeaveCriticalRegion();
    }

    return retVal;
}

VOID
PpDevNodeUnlockTree(
    __in  PNP_LOCK_LEVEL  LockLevel
    )
/*++

Routine Description:

    This function releases the tree lock with the appropriate level of
    restrictions.

Arguments:

    LockLevel:
        PPL_SIMPLE_READ         - Allows simple examination of the tree.

        PPL_TREEOP_ALLOW_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however.

        PPL_TREEOP_ALLOW_READS_NO_WAIT
                                - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however.

        PPL_TREEOP_BLOCK_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads are also blocked.

        PPL_TREEOP_BLOCK_READS_FROM_ALLOW - Switch to PPL_TREEOP_BLOCK_READS
                                            when already in
                                            PPL_TREEOP_BLOCK_READS. Note that
                                            PpDevNodeUnlockTree must be
                                            subsequently called on both to
                                            release.
Return Value:

    None.

--*/
{
    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(LockLevel);
    switch(LockLevel) {

        case PPL_SIMPLE_READ:
            ExReleaseResourceLite(&IopDeviceTreeLock);
            break;

        case PPL_TREEOP_ALLOW_READS:
            ExReleaseResourceLite(&IopDeviceTreeLock);
            ExReleaseResourceLite(&PiEngineLock);
            break;

        case PPL_TREEOP_ALLOW_READS_NO_WAIT:
            ExReleaseResourceLite(&IopDeviceTreeLock);
            ExReleaseResourceLite(&PiEngineLock);
            break;

        case PPL_TREEOP_BLOCK_READS:
            ExReleaseResourceLite(&IopDeviceTreeLock);
            ExReleaseResourceLite(&PiEngineLock);
            break;

        case PPL_TREEOP_BLOCK_READS_FROM_ALLOW:
            //
            // The engine lock should still be held here. Now we adjust the
            // tree lock. Go back to allow by converting the exclusive lock to
            // shared. Note that this doesn't chance the acquisition count.
            //
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&IopDeviceTreeLock));
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiEngineLock));
            ExConvertExclusiveToSharedLite(&IopDeviceTreeLock);
            break;

        default:
            PNP_ASSERT(0);
            break;
    }

    KeLeaveCriticalRegion();
}

VOID
IopReleaseResources (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    IopReleaseResources releases resources owned by the device and release
    the memory pool.  We also release the cached resource requirements list.
    If the device is a root enumerated device with BOOT config, we will preallocate
    boot config resources for this device.

    NOTE, this is a routine INTERNAL to this file.  NO one should call this function
    outside of this file.  Outside of this file, IopReleaseDeviceResources should be
    used.

Arguments:

    DeviceNode - Supplies a pointer to the device node.object.  If present, caller wants to

Return Value:

    None.

--*/
{
    PAGED_CODE();

    //
    // Release the resources owned by the device
    //

    PnpReleaseResourcesInternal(DeviceNode);

#if DBG

    if (DeviceNode->PreviousResourceList) {
        ExFreePool(DeviceNode->PreviousResourceList);
        DeviceNode->PreviousResourceList = NULL;
    }
    if (DeviceNode->PreviousResourceRequirements) {
        ExFreePool(DeviceNode->PreviousResourceRequirements);
        DeviceNode->PreviousResourceRequirements = NULL;
    }
#endif

    IopAcquireResourceListLock(&PiResourceListLock);
    if (DeviceNode->ResourceList) {

#if DBG

        if (!NT_SUCCESS(DeviceNode->FailureStatus)) {
            DeviceNode->PreviousResourceList = DeviceNode->ResourceList;
        } else {
            ExFreePool(DeviceNode->ResourceList);
        }

#else

        ExFreePool(DeviceNode->ResourceList);

#endif

        DeviceNode->ResourceList = NULL;
    }
    if (DeviceNode->ResourceListTranslated) {
        ExFreePool(DeviceNode->ResourceListTranslated);
        DeviceNode->ResourceListTranslated = NULL;
    }
    IopReleaseResourceListLock(&PiResourceListLock);

    //
    // If this device is a root enumerated device, preallocate its BOOT resources
    //

    if ((DeviceNode->Flags & (DNF_MADEUP | DNF_DEVICE_GONE)) == DNF_MADEUP) {
        if (DeviceNode->Flags & DNF_HAS_BOOT_CONFIG && DeviceNode->BootResources) {
            IopAllocateBootResourcesInternal(ArbiterRequestPnpEnumerated,
                                            DeviceNode->PhysicalDeviceObject,
                                            DeviceNode->BootResources);
        }
    } else {
        PipClearDevNodeFlags(DeviceNode, DNF_HAS_BOOT_CONFIG | DNF_BOOT_CONFIG_RESERVED);
        if (DeviceNode->BootResources) {
            ExFreePool(DeviceNode->BootResources);
            DeviceNode->BootResources = NULL;
        }
    }
}

NTSTATUS
IopWriteAllocatedResourcesToRegistry (
    __in PDEVICE_NODE DeviceNode,
    __in_bcount_opt(Length) PCM_RESOURCE_LIST CmResourceList,
    __in_opt ULONG Length
    )

/*++

Routine Description:

    This routine writes allocated resources for a device to its control key of device
    instance path key.

Arguments:

    DeviceNode - Supplies a pointer to the device node structure of the device.

    CmResourceList - Supplies a pointer to the device's allocated CM resource list.

    Length - Supplies the length of the CmResourceList.

Return Value:

    The status returned is the final completion status of the operation.

--*/
{
    NTSTATUS status;
    HANDLE handle;
    UNICODE_STRING unicodeName;

    PiLockPnpRegistry(FALSE);

    //
    // Open the Control key of the device instance.
    //

    status = _CmOpenDeviceRegKey(
                 PiPnpRtlCtx,
                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                 CM_KEY_TYPE_DEVICE_CONTROL,
                 0,
                 KEY_ALL_ACCESS,
                 TRUE,
                 &handle,
                 NULL);

    if (NT_SUCCESS(status)) {

        PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_ALLOC_CONFIG);
        if (CmResourceList) {
            status = ZwSetValueKey(
                          handle,
                          &unicodeName,
                          TITLE_INDEX_VALUE,
                          REG_RESOURCE_LIST,
                          CmResourceList,
                          Length
                          );
        } else {
            status = ZwDeleteValueKey(handle, &unicodeName);
        }
        ZwClose(handle);
    }

    PiUnlockPnpRegistry();
    return status;
}

NTSTATUS
IopReleaseDeviceResources (
    __in PDEVICE_NODE DeviceNode,
    __in LOGICAL ReserveResources
    )

/*++

Routine Description:

    This routine releases the resources assigned to a device.

Arguments:

    DeviceNode          - Device whose resources are to be released.

    ReserveResources    - TRUE specifies that the BOOT config needs to be
                          reserved (after re-query).

Return Value:

    Final status code.


--*/
{
    ULONG               cmLength;
    PCM_RESOURCE_LIST   cmResource;
    BOOLEAN             createKey;
    HANDLE              logConfHandle;
    NTSTATUS            status;
    UNICODE_STRING      unicodeName;

    PAGED_CODE();

    if (    !DeviceNode->ResourceList &&
            !(DeviceNode->Flags & DNF_BOOT_CONFIG_RESERVED)) {

        return STATUS_SUCCESS;
    }

    cmLength    = 0;
    cmResource  = NULL;
    createKey   = FALSE;

    //
    // If needed, re-query for BOOT configs. We need to do this BEFORE we
    // release the BOOT config (otherwise ROOT devices cannot report BOOT
    // config).
    //

    if (ReserveResources && !(DeviceNode->Flags & DNF_MADEUP)) {
        //
        // First query for new BOOT config (order important for ROOT devices).
        //
        status = IopQueryDeviceResources(
                    DeviceNode->PhysicalDeviceObject,
                    QUERY_RESOURCE_LIST,
                    &cmResource,
                    &cmLength);
        if (!NT_SUCCESS(status)) {

            cmResource  = NULL;
            cmLength    = 0;
        }
    }
    //
    // Release resources for this device.
    //
    status = IopLegacyResourceAllocation(ArbiterRequestUndefined,
                                         PnpDriverObject,
                                         DeviceNode->PhysicalDeviceObject,
                                         NULL,
                                         NULL);
    if (!NT_SUCCESS(status)) {

        return status;
    }
    //
    // Request reallocation of resources for conflicting devices.
    //
    PnpRequestDeviceAction(NULL, AssignResources, FALSE, 0, NULL, NULL);
    //
    // If needed, re-query and reserve current BOOT config for this device.
    // We always rereserve the boot config (ie DNF_MADEUP root enumerated
    // and IoReportDetected) devices in IopLegacyResourceAllocation.
    //
    if (ReserveResources && !(DeviceNode->Flags & DNF_MADEUP)) {

        PNP_ASSERT(DeviceNode->BootResources == NULL);

        logConfHandle = NULL;

        //
        // Only create the LogConf key if a resource will be set.  If a resource
        // is not going to be set and the LogConf key doesn't exist, then there
        // is no resource there to delete.
        //

        if (cmResource != NULL) {
            createKey = TRUE;
        }

        status = _CmOpenDeviceRegKey(
                     PiPnpRtlCtx,
                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                     CM_KEY_TYPE_DEVICE_LOGCONF,
                     0,
                     KEY_ALL_ACCESS,
                     createKey,
                     &logConfHandle,
                     NULL);

        if (NT_SUCCESS(status)) {

            PNP_ASSERT(logConfHandle != NULL);

            PiWstrToUnicodeString(&unicodeName, REGSTR_VAL_BOOTCONFIG);

            PiLockPnpRegistry(FALSE);

            if (cmResource) {

                ZwSetValueKey(
                    logConfHandle,
                    &unicodeName,
                    TITLE_INDEX_VALUE,
                    REG_RESOURCE_LIST,
                    cmResource,
                    cmLength);
            } else {

                ZwDeleteValueKey(logConfHandle, &unicodeName);
            }

            PiUnlockPnpRegistry();
            ZwClose(logConfHandle);
        }
        //
        // Reserve any remaining BOOT config.
        //
        if (cmResource) {

            PipSetDevNodeFlags(DeviceNode, DNF_HAS_BOOT_CONFIG);
            //
            // This device consumes BOOT resources.  Reserve its boot resources
            //
            (*IopAllocateBootResourcesRoutine)(
                ArbiterRequestPnpEnumerated,
                DeviceNode->PhysicalDeviceObject,
                DeviceNode->BootResources = cmResource);
        }
    }

    return STATUS_SUCCESS;
}

VOID
IopUncacheInterfaceInformation (
    __in PDEVICE_OBJECT DeviceObject,
    __in LOGICAL        AcquireLock
    )

/*++

Routine Description:

    This function removes all the cached translators and arbiters information
    from the device object.

Parameters:

    DeviceObject - Supplies the device object of the device being removed.

    AcquireLock - If TRUE, acquire the resource manager lock. This allows
        PNP to synchronize with the legacy resource allocations.

Return Value:

    None.

--*/

{
    PDEVICE_NODE                    deviceNode;
    PLIST_ENTRY                     listHead;
    PLIST_ENTRY                     nextEntry;
    PLIST_ENTRY                     entry;
    PPI_RESOURCE_TRANSLATOR_ENTRY   translatorEntry;
    PPI_RESOURCE_ARBITER_ENTRY      arbiterEntry;
    PINTERFACE                      interface;

    if (AcquireLock) {

        IopLockResourceManager();
    }

    deviceNode = PP_DO_TO_DN(DeviceObject);
    //
    // Dereference all the arbiters on this PDO.
    //
    listHead    = &deviceNode->DeviceArbiterList;
    nextEntry   = listHead->Flink;
    while (nextEntry != listHead) {

        entry           = nextEntry;
        arbiterEntry    = CONTAINING_RECORD(
                            entry,
                            PI_RESOURCE_ARBITER_ENTRY,
                            DeviceArbiterList);

        interface = (PINTERFACE)arbiterEntry->ArbiterInterface;
        if (interface != NULL) {

            (interface->InterfaceDereference)(interface->Context);
            ExFreePool(interface);
        }
        nextEntry = entry->Flink;
        ExFreePool(entry);
    }
    //
    // Dereference all the translators on this PDO.
    //
    listHead    = &deviceNode->DeviceTranslatorList;
    nextEntry   = listHead->Flink;
    while (nextEntry != listHead) {
        entry           = nextEntry;
        translatorEntry = CONTAINING_RECORD(
                            entry,
                            PI_RESOURCE_TRANSLATOR_ENTRY,
                            DeviceTranslatorList);
        interface = (PINTERFACE)translatorEntry->TranslatorInterface;
        if (interface != NULL) {

            (interface->InterfaceDereference)(interface->Context);
            ExFreePool(interface);
        }
        nextEntry = entry->Flink;
        ExFreePool(entry);
    }
    InitializeListHead(&deviceNode->DeviceArbiterList);
    InitializeListHead(&deviceNode->DeviceTranslatorList);
    deviceNode->NoArbiterMask       = 0;
    deviceNode->QueryArbiterMask    = 0;
    deviceNode->NoTranslatorMask    = 0;
    deviceNode->QueryTranslatorMask = 0;

    if (AcquireLock) {

        IopUnlockResourceManager();
    }
}

NTSTATUS
PnpAllocateDeviceInstancePath (
    __in PDEVICE_NODE   DeviceNode,
    __in ULONG          Size
    )

/*++

Routine Description:

    This function allocates device instance path for the devnode.

Parameters:

    DeviceNode - Supplies a pointer to the device node.

    Length - Supplies the size of the instance path in bytes.

Return Value:

    NTSTATUS.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(DeviceNode->InstancePath.Buffer == NULL);

    DeviceNode->InstancePath.Length = 0;
    DeviceNode->InstancePath.MaximumLength = (USHORT)Size;
    DeviceNode->InstancePath.Buffer = ExAllocatePoolWithTag(
                                        NonPagedPoolNx,
                                        Size,
                                        PNP_POOLTAG_INSTANCE_PATH);
    if (DeviceNode->InstancePath.Buffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

VOID
PnpFreeDeviceInstancePath (
    __in PDEVICE_NODE   DeviceNode
    )

/*++

Routine Description:

    This function frees storage for the device instance path for the devnode.

Parameters:

    DeviceNode - Supplies a pointer to the device node.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    //
    // Acquire the reference table lock here to ensure the instance path
    // is not wacked while its being used by some other thread to look up
    // a device in the table.
    //

    KeAcquireGuardedMutex(&PnpDeviceReferenceTableLock);
    if (DeviceNode->InstancePath.Buffer != NULL) {
        ExFreePoolWithTag(DeviceNode->InstancePath.Buffer, PNP_POOLTAG_INSTANCE_PATH);
    }

    PiWstrToUnicodeString(&DeviceNode->InstancePath, NULL);
    KeReleaseGuardedMutex(&PnpDeviceReferenceTableLock);
}

VOID
PnpCopyDeviceInstancePath (
    __in PDEVICE_NODE       DeviceNode,
    __in PUNICODE_STRING    InstancePath
    )

/*++

Routine Description:

    This function copies device instance path into the devnode.

Parameters:

    DeviceNode - Supplies a pointer to the device node.

    InstancePath - Supplies a pointer to the instance path.

Return Value:

    NTSTATUS.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(InstancePath != NULL);
    PNP_ASSERT(DeviceNode->InstancePath.Buffer != NULL);
    PNP_ASSERT(DeviceNode->InstancePath.Length == 0);
    PNP_ASSERT(DeviceNode->InstancePath.MaximumLength >= InstancePath->Length);

    RtlCopyUnicodeString(&DeviceNode->InstancePath,
                         InstancePath);
}

VOID
PnpCheckForWaitingReportedDevice (
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine checks to see if the given device object belongs to a reported
    device that is waiting to for an FDO to be attached.  If so, an enumeration
    of that device is queued so as to query for updated hardware IDs from the
    updated device stack.

    N.B. LockQueueIoDatabaseLock must already be held by the caller.

Arguments:

    DeviceObject - Supplies a pointer to the device object to check.

Return Value:

    None.

--*/

{

    PDEVICE_NODE deviceNode;

    PNP_ASSERT(DeviceObject != NULL);

    deviceNode = PP_DO_TO_DN(DeviceObject);
    if ((deviceNode != NULL) &&
        ((deviceNode->Flags & DNF_WAITING_FOR_FDO) != 0)) {
        PnpRequestDeviceAction(deviceNode->PhysicalDeviceObject,
                               ReenumerateDeviceOnly,
                               FALSE,
                               (ULONG_PTR)TRUE,
                               NULL,
                               NULL);
    }

    return;
}

LOGICAL
PipIsDevNodeEffectivelyRemoved(
    _In_ PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This routine checks the device node to determine if it is effectively
    removed. A device is considered effectively removed if it has been actually
    removed  or is in the process of being removed (query removed, surprise
    removed, waiting for handles to be closed etc.).

    N.B. This routine assumes the caller holds the PNP device tree to allow
         the device node state to be examined reliably.

Arguments:

    DeviceNode - Device node to examine.

Return Value:

    TRUE if the devnode should be considered effectively removed, FALSE
    otherwise.

--*/
{

    switch (DeviceNode->State) {

        case DeviceNodeRemoved:
        case DeviceNodeQueryRemoved:
        case DeviceNodeRemovePendingCloses:
        case DeviceNodeDeletePendingCloses:
        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeAwaitingQueuedDeletion:
        case DeviceNodeDeleted:
            return TRUE;

        default:
            return FALSE;
    }
}

#if DBG

VOID
PpDevNodeAssertLockLevel(
    __in  PNP_LOCK_LEVEL  LockLevel,
    __in  PCSTR           File,
    __in  ULONG           Line
    )
/*++

Routine Description:

    This asserts the lock is currently held at the appropriate level.

Arguments:

    LockLevel:
        PPL_SIMPLE_READ         - Allows simple examination of the tree.

        PPL_TREEOP_ALLOW_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however.

        PPL_TREEOP_ALLOW_READS_NO_WAIT
                                - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads can go through however.

        PPL_TREEOP_BLOCK_READS  - Called as part of a StartEnum/Remove/Power
                                  operation, blocks other such operations.
                                  Simple reads are also blocked.

        PPL_TREEOP_BLOCK_READS_FROM_ALLOW - Switch to PPL_TREEOP_BLOCK_READS
                                            when already in
                                            PPL_TREEOP_BLOCK_READS. Note that
                                            PpDevNodeUnlockTree must be
                                            subsequently called on both to
                                            release.

    File: Name of c-file asserting the lock is held.

    Line: Line number in above c-file.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER (File);
    UNREFERENCED_PARAMETER (Line);

    switch(LockLevel) {

        case PPL_SIMPLE_READ:
            PNP_ASSERT(ExIsResourceAcquiredSharedLite(&IopDeviceTreeLock));
            break;

        case PPL_TREEOP_ALLOW_READS:
            PNP_ASSERT(ExIsResourceAcquiredSharedLite(&IopDeviceTreeLock));
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiEngineLock));
            break;

        case PPL_TREEOP_ALLOW_READS_NO_WAIT:
            PNP_ASSERT(ExIsResourceAcquiredSharedLite(&IopDeviceTreeLock));
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiEngineLock));
            break;

        case PPL_TREEOP_BLOCK_READS_FROM_ALLOW:
            //
            // This isn't really a lock level, but this assert-o-matic function
            // is called from Unlock, in which case this level means "drop back
            // to PPL_TREEOP_ALLOW_READS *from* PPL_TREEOP_BLOCK_READS." So...
            //
            // Fall through
            //

        case PPL_TREEOP_BLOCK_READS:
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&IopDeviceTreeLock));
            PNP_ASSERT(ExIsResourceAcquiredExclusiveLite(&PiEngineLock));
            break;

        default:
            PNP_ASSERT(0);
            break;
    }
}

#endif // DBG
