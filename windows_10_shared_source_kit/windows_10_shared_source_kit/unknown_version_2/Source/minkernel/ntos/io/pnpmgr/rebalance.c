/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    rebalance.c

Abstract:

    This module implements the hardware resource rebalance.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

LOGICAL PnpDisableRebalance = FALSE;
#define PNP_DHP_REBALANCE_POLICY_OPT_OUT 1
#define PNP_DHP_REBALANCE_POLICY_OPT_IN 2

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, PnpRebalance)
#pragma alloc_text(PAGE, PnpFindRebalanceCandidates)
#pragma alloc_text(PAGE, PnpQueryRebalance)
#pragma alloc_text(PAGE, PnpQueryRebalanceWorker)
#pragma alloc_text(PAGE, PnpQueryStopDeviceNode)
#pragma alloc_text(PAGE, PnpCancelStopDeviceNode)
#pragma alloc_text(PAGE, PnpStopDeviceSubtree)
#pragma alloc_text(PAGE, PnpStopDeviceNode)
#pragma alloc_text(PAGE, PnpCancelStopDeviceSubtree)

#endif // ALLOC_PRAGMA

BOOLEAN
PiCollapseRebalanceRequests(
    PLIST_ENTRY ListHead
    );

BOOLEAN
PiRebalanceOptOut (
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpRebalance (
    __in PDEVICE_NODE RebalanceNode,
    __in_ecount_opt(RequestTableCount) PPNP_RESOURCE_REQUEST RequestTable,
    __in_opt ULONG RequestTableCount,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    )

/*++

Routine Description:

    This function attempts to rebalance resources in the system.

Parameters:

    RebalanceNode - Pointer to the device that triggered rebalance.

    RequestTable - Pointer to the original resource requests that could not be satisfied.

    RequestTableCount - Number of entries in the request table.

    RebalanceDueToDynamicPartitioning - TRUE if the rebalance is due to
        a dynamic partitioning event. In this case rebalance should
        rebalance as many devices beneath the given node irrespective
        of whether the entire tree starting from the node can participate.
        If this is FALSE, the requesting device (and its entire subordinate
        tree) must participate.

Return Value:

    Rebalance status.

--*/

{
    NTSTATUS                status;
    PPNP_RESOURCE_REQUEST   rebalTable;
    ULONG                   rebalPhase, rebalCandidateCount, oldRebalCandidateCount, i;
    LIST_ENTRY              activeArbiterList;
    LOGICAL                 found;
    PNP_REBALANCE_TRACE_CONTEXT traceContext;

    PAGED_CODE();

    if (RebalanceNode->RebalanceContext != NULL) {
        RtlZeroMemory(RebalanceNode->RebalanceContext,
                     (FIELD_OFFSET(PNP_REBALANCE_TRACE_CONTEXT, ConflictContext)));
    }

    PnpTraceRebalanceInitializeContext(&traceContext);
    traceContext.RebalanceDueToDynamicPartitioning =
        RebalanceDueToDynamicPartitioning;

    PnpDiagnosticTraceObject(&KMPnPEvt_Rebalance_Start,
                             PnpGetDeviceInstancePath(RebalanceNode));

    PNP_ASSERT(RequestTableCount <= 1);
    if (PnpDisableRebalance != FALSE) {
        IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                     "PnppRebalance: Rebalance disabled via policy!\n"));

        traceContext.Failure[0] = RebalanceFailureDisabled;
        status = STATUS_NOT_SUPPORTED;
        goto PnpRebalanceEnd;
    }

    //
    // Assume best case scenario of all devices in the system participating and allocate
    // table large enough to hold all the requests.
    //

    rebalTable = (PPNP_RESOURCE_REQUEST)ExAllocatePoolWithTag(PagedPool,
                                                              IopNumberDeviceNodes * sizeof(PNP_RESOURCE_REQUEST),
                                                              PNP_POOLTAG_REBALANCE_REQUEST_TABLE);

    if (rebalTable == NULL) {
        IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                     "PnpRebalance: Could not allocate memory to collect rebalance candidates\n"));

        traceContext.Failure[0] = RebalanceFailureNoMemory;
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto PnpRebalanceEnd;
    }

    //
    // Copy the original table at the beginning of the table. Devices that participate in
    // rebalance will get added after the original table entries.
    //

    if (RequestTableCount != 0) {
        RtlCopyMemory(rebalTable,
                      RequestTable,
                      RequestTableCount * sizeof(PNP_RESOURCE_REQUEST));
    }

    //
    // Attempt rebalance in two phases.
    //
    // Phase 1 attempts to do an optimize rebalance by only stopping the smallest subtree
    // containing the device that triggered rebalance. We hope this is the most common
    // case.
    //
    // Phase 2 falls back to a full tree wide rebalance. Hopefully we dont have to do this
    // often.
    //

    status = STATUS_SUCCESS;
    rebalCandidateCount = 0;
    for (rebalPhase = 0; rebalPhase < 2; rebalPhase++) {
        traceContext.RebalancePhase += 1;

        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                     "PnpRebalance: PHASE = %d\n", rebalPhase));

        oldRebalCandidateCount = rebalCandidateCount;

        //
        // Find more rebalance candidates.
        //

        PnpFindRebalanceCandidates(rebalPhase,
                                   RebalanceNode,
                                   &rebalTable[RequestTableCount],
                                   &rebalCandidateCount,
                                   RebalanceDueToDynamicPartitioning,
                                   &traceContext);

        //
        // If we are reallocating a device's resources, it has to succeed
        // query_stop and report that its requirements changed.
        //

        if ((RequestTableCount == 0) &&
            (RebalanceDueToDynamicPartitioning == FALSE)) {

            //
            // The table contains only those devices that report any resource
            // requirement.
            //

            found = FALSE;
            for (i = 0; i < rebalCandidateCount; i++) {
                if (rebalTable[i].PhysicalDevice ==
                    RebalanceNode->PhysicalDeviceObject) {

                    found = TRUE;
                    break;
                }
            }

            if (found == FALSE) {
                status = STATUS_UNSUCCESSFUL;
                if (RebalanceNode->State != DeviceNodeQueryStopped) {
                    IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                                 "PnpRebalance: Device %wZ wanted to change its resources but did not succeed query stop!\n",
                                 PnpGetDeviceInstancePath(RebalanceNode)));

                    traceContext.Failure[rebalPhase] = RebalanceFailureQueryStopUnexpectedVeto;

                } else {
                    IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                                 "PnpRebalance: Device %wZ wanted to change its resources but did not report any requirements!\n",
                                 PnpGetDeviceInstancePath(RebalanceNode)));

                    traceContext.Failure[rebalPhase] = RebalanceFailureNoRequirements;
                }

                break;
            }
        }

        //
        // If no more rebalance candidates found, rebalance has failed and we are done.
        //

        if (oldRebalCandidateCount != rebalCandidateCount) {

            //
            // Find the best working configuration.
            //

            status = PnpFindBestConfiguration(rebalTable,
                                              RequestTableCount + rebalCandidateCount,
                                              &activeArbiterList);

            //
            // If a best working configuration is found, rebalance is successful and we are done.
            //

            if (NT_SUCCESS(status)) {
                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "PnpRebalance: Rebalance successful\n"));

                break;

            } else {
                if (traceContext.Failure[rebalPhase] == RebalanceFailureNone) {
                    traceContext.Failure[rebalPhase] = RebalanceFailureNoConfiguration;
                }
            }

        } else {
            IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                         "PnpRebalance: No new rebalance candidates found (previous = %d), rebalance failed\n", oldRebalCandidateCount));

            if (traceContext.Failure[rebalPhase] == RebalanceFailureNone) {
                    traceContext.Failure[rebalPhase] = RebalanceFailureNoCandidates;
            }

            //
            // Allow MCFG conflict errors to persist.
            //

            if (status != STATUS_BAD_MCFG_TABLE) {
                status = STATUS_UNSUCCESSFUL;
            }
        }
    }

    //
    // Do post rebalance processing based on whether it was successful or not
    //

    if (NT_SUCCESS(status)) {

        //
        // Stop everyone who participated in rebalance. Note that we dont
        // attempt to optimize stopping only those whose resources changed.
        //

        PnpStopDeviceSubtree(IopRootDeviceNode);
        IopCommitConfiguration(&activeArbiterList);

        //
        // Build resource lists for the table.  Only update the devnode state
        // for the rebalance participants.
        //

        if (RequestTableCount != 0) {
            PnpBuildCmResourceLists(&rebalTable[0],
                                    &rebalTable[RequestTableCount],
                                    FALSE);
        }

        PnpBuildCmResourceLists(&rebalTable[RequestTableCount],
                                &rebalTable[RequestTableCount + rebalCandidateCount],
                                TRUE);

        //
        // Copy back into the original table.
        //

        if (RequestTableCount != 0) {
            RtlCopyMemory(RequestTable,
                          rebalTable,
                          RequestTableCount * sizeof(PNP_RESOURCE_REQUEST));
        }

    } else {

        //
        // Send cancels since rebalance failed!
        //

        PnpCancelStopDeviceSubtree(IopRootDeviceNode);
    }

    PnpFreeResourceRequirementsForAssignTable(
        &rebalTable[RequestTableCount],
        &rebalTable[RequestTableCount + rebalCandidateCount]);

    ExFreePoolWithTag(rebalTable, PNP_POOLTAG_REBALANCE_REQUEST_TABLE);

PnpRebalanceEnd:
    PnpTraceRebalanceResult(&traceContext);
    
    //
    // Record the rebalance result.
    //

    if (!NT_SUCCESS(status)) {
        PnpLogDeviceRebalanceResult(RebalanceNode, &traceContext);
    } else {
        if (RebalanceNode->RebalanceContext != NULL) {
            ExFreePoolWithTag(RebalanceNode->RebalanceContext,
                              PNP_POOLTAG_REBALANCE_TRACE_CONTEXT);

            RebalanceNode->RebalanceContext = NULL;
        }
    }

    PnpDiagnosticTraceObjectWithStatus(&KMPnPEvt_Rebalance_Stop,
                                       PnpGetDeviceInstancePath(RebalanceNode),
                                       status);

    return status;
}

VOID
PnpFindRebalanceCandidates (
    __in ULONG Phase,
    __in PDEVICE_NODE DeviceNode,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout OUT PULONG RebalanceTableCount,
    __in BOOLEAN RebalanceDueToDynamicPartitioning,
    __inout PPNP_REBALANCE_TRACE_CONTEXT TraceContext
    )

/*++

Routine Description:

    This functions finds devices that can be used in rebalance.

    For phase 0, it attempts to find the smallest subtree whose resource requirements have
    changed. We assume that this would be the parent of the highest bridge whose resource
    requirements changed to accomodate the new device.

    For phase 1, it walks the entire device tree and attempts to query stop all devices.

Parameters:

    Phase - Rebalance phase.

    DeviceNode - Pointer to the device that triggered rebalance.

    RebalanceTable - Pointer to the request table that gets updated with new rebalance
        candidates.

    RebalanceTableCount - On input, it contains the number of entries in the table. It gets
        updated as neccessary.

    RebalanceDueToDynamicPartitioning - TRUE if the rebalance operation is a result of
        a dynamic partitioning event.

    TraceContext - Supplies a pointer to the tracing context.

Return Value:

    None.

--*/

{
    PDEVICE_NODE    node;
    NTSTATUS        status;

    PAGED_CODE();

    if (Phase == 0 && DeviceNode) {
        TraceContext->TriggerRoot = DeviceNode;
        if (DeviceNode->State == DeviceNodeStarted) {

            //
            // The requirements for this device have changed.
            //
            node = DeviceNode;
            TraceContext->Reason[Phase] = RebalanceReasonRequirementsChanged;

        } else {

            //
            // This is a new device that triggered rebalance.
            //
            node = DeviceNode->Parent;
            TraceContext->Reason[Phase] = RebalanceReasonNewDevice;
        }

        //
        // Walk up from the DeviceNode.
        //
        status = STATUS_SUCCESS;
        while (node) {

            status = PnpQueryRebalance(node,
                                       RebalanceTable,
                                       RebalanceTableCount,
                                       &(TraceContext->VetoNode[Phase]),
                                       &(TraceContext->VetoQueryRebalanceReason[Phase]),
                                       RebalanceDueToDynamicPartitioning);

            if (status != STATUS_RESOURCE_REQUIREMENTS_CHANGED) {

                break;
            }

            node = node->Parent;
        }

        if (node) {

            if (NT_SUCCESS(status)) {
                TraceContext->SubtreeIncludesRoot = TRUE;
                IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                             "IopFindRebalanceCandidates: Using subtree at %wZ during PHASE=%d of REBALANCE\n",
                             PnpGetDeviceInstancePath(node),
                             Phase));
            } else {
                TraceContext->SubtreeIncludesRoot = FALSE;
                IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                             "IopFindRebalanceCandidates: Using subtree below %wZ during PHASE=%d of REBALANCE\n",
                             PnpGetDeviceInstancePath(node),
                             Phase));
            }

            TraceContext->SubtreeRoot = node;
        }

    } else {

        //
        // Walk the entire device tree.
        //
        PnpQueryRebalance(IopRootDeviceNode,
                          RebalanceTable,
                          RebalanceTableCount,
                          &(TraceContext->VetoNode[Phase]),
                          &(TraceContext->VetoQueryRebalanceReason[Phase]),
                          RebalanceDueToDynamicPartitioning);

        TraceContext->SubtreeRoot = IopRootDeviceNode;
        TraceContext->SubtreeIncludesRoot = TRUE;
    }

    TraceContext->DeviceCount = *RebalanceTableCount;
}

NTSTATUS
PnpQueryRebalance (
    __in PDEVICE_NODE Subtree,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout PULONG RebalanceTableCount,
    __out_opt PDEVICE_NODE *VetoNode,
    __out_opt PPNP_DEVNODE_QUERY_REBALANCE_VETO_REASON VetoReason,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    )

/*++

Routine Description:

    This function finds rebalance candidates by walking the device tree and
    sending IRP_MN_QUERY_STOP_DEVICE bottom up. Devices that succeed this IRP
    participate in rebalance. If a device fails this IRP, his parent does not
    get queried and cannot participate. However, its siblings can still
    participate if they succeed the IRP.

Parameters:

    Subtree - Pointer to the root of the subtree to be queried for rebalance.

    RebalanceTable      - DeviceNode gets added to the end of this table if it
        can be used for rebalance.

    RebalanceTableCount - On input, it contains the number of rebalance
        candidates already found. It gets updated as neccessary.

    RebalanceDueToDynamicPartitioning - TRUE if the rebalance is due to a
        dynamic paritioning event.

    VetoNode - An auxiliary pointer for recording the node that veto'ed rebalance.

    VetoReason - An auxiliary pointer for recording the reason of veto.

Return Value:

    Status indicating whether the root of the subtree is a rebalance
        candidate or not.

--*/

{
    NTSTATUS status, subtreeStatus;
    PDEVICE_NODE node;

    PAGED_CODE();

    subtreeStatus = STATUS_SUCCESS;

    //
    // First query all children.
    //
    for (node = Subtree->Child; node; node = node->Sibling) {

        //
        // Recurse on children.
        //
        status = PnpQueryRebalance(node,
                                   RebalanceTable,
                                   RebalanceTableCount,
                                   VetoNode,
                                   VetoReason,
                                   RebalanceDueToDynamicPartitioning);

        //
        // Remember if any child fails the query.
        //
        if (!NT_SUCCESS(status)) {

            subtreeStatus = STATUS_UNSUCCESSFUL;
        }
    }

    //
    // Query the root iff the entire subtree succeeded the query.
    //
    if (NT_SUCCESS(subtreeStatus)) {

        subtreeStatus =
            PnpQueryRebalanceWorker(Subtree,
                                    RebalanceTable,
                                    RebalanceTableCount,
                                    VetoNode,
                                    VetoReason,
                                    RebalanceDueToDynamicPartitioning);
    }

    //
    // Return the query status for the subtree.
    //
    return subtreeStatus;
}

NTSTATUS
PnpQueryRebalanceWorker (
    __in PDEVICE_NODE DeviceNode,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout PULONG RebalanceTableCount,
    __out_opt PDEVICE_NODE *VetoNode,
    __out_opt PPNP_DEVNODE_QUERY_REBALANCE_VETO_REASON VetoReason,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    )

/*++

Routine Description:

    This routine determines if the passed in device node can be used for
        rebalance.

Parameters:

    DeviceNode          - Device to be queried.

    RebalanceTable      - DeviceNode gets added to the end of this table if it
        can be used for rebalance.

    RebalanceTableCount - On input, it contains the number of rebalance
        candidates already found. It gets updated as neccessary.

    RebalanceDueToDynamicPartitioning - TRUE if this rebalance was due to a
        dynamic partitioning event.

    VetoNode - An auxiliary pointer for recording the node that veto'ed rebalance.

    VetoReason - An auxiliary pointer for recording the reason of veto.

Return Value:

    Status indicating whether the devnode is a rebalance candidate or not.

--*/

{
    NTSTATUS status;
    ULONG deviceCount;

    PAGED_CODE();

    status = STATUS_SUCCESS;

    if (DeviceNode->State == DeviceNodeStarted &&
        !(DeviceNode->Flags & DNF_LEGACY_DRIVER)) {

        status = PnpQueryStopDeviceNode(DeviceNode,
                                        RebalanceDueToDynamicPartitioning);

        if (NT_SUCCESS(status)) {

            IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                         "IopQueryStopDeviceNode: %wZ participates in REBALANCE\n",
                         PnpGetDeviceInstancePath(DeviceNode)));

            if (status == STATUS_RESOURCE_REQUIREMENTS_CHANGED) {

                IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                             "IopQueryStopDeviceNode: %wZ reported requirements changed during REBALANCE\n",
                             PnpGetDeviceInstancePath(DeviceNode)));

                //
                // Remember that we need to requery requirements for this device before rebalance.
                //
                PipSetDevNodeFlags(DeviceNode, DNF_RESOURCE_REQUIREMENTS_CHANGED);
            }

            PnpInitializeResourceRequest(&RebalanceTable[*RebalanceTableCount],
                                         DeviceNode->PhysicalDeviceObject,
                                         NULL,
                                         ArbiterRequestPnpEnumerated);

            PnpGetResourceRequirementsForAssignTable(&RebalanceTable[*RebalanceTableCount],
                                                     &RebalanceTable[*RebalanceTableCount + 1],
                                                     &deviceCount);
            if (NT_SUCCESS(RebalanceTable[*RebalanceTableCount].Status)) {

                //
                // Record resource consuming rebalance candidates only.
                //
                if (!(RebalanceTable[*RebalanceTableCount].Flags & IOP_ASSIGN_IGNORE)) {

                    (*RebalanceTableCount)++;
                }

            } else {

                //
                // If something went wrong while getting the new requirements of a device, it cannot participate in rebalance.
                // Return failure so none of its parents will be queried.
                //
                status = RebalanceTable[*RebalanceTableCount].Status;

                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "IopQueryStopDeviceNode: Failed to get requirements for %wZ, cannot participate in REBALANCE (status=%08x)\n",
                             PnpGetDeviceInstancePath(DeviceNode),
                             status));

                PnpCancelStopDeviceNode(DeviceNode);
                if (ARGUMENT_PRESENT(VetoReason) != FALSE) {
                    *VetoReason = DeviceFailedGetNewResourceRequirement;
                }
            }

        } else {

            IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                         "IopQueryStopDeviceNode: %wZ does not participate in REBALANCE (status=%08x)\n",
                         PnpGetDeviceInstancePath(DeviceNode),
                         status));

            PnpCancelStopDeviceNode(DeviceNode);
            if (ARGUMENT_PRESENT(VetoReason) != FALSE) {
                *VetoReason = DeviceQueryStopFailed;
            }
        }

    } else if (DeviceNode->State != DeviceNodeUninitialized &&
               DeviceNode->State != DeviceNodeInitialized &&
               DeviceNode->State != DeviceNodeDriversAdded &&
               DeviceNode->State != DeviceNodeQueryStopped &&
               DeviceNode->State != DeviceNodeRemovePendingCloses &&
               DeviceNode->State != DeviceNodeRemoved &&
               !(DeviceNode->Flags & DNF_NEEDS_REBALANCE)) {
        //
        // These cause the parent to not participate in rebalance.
        //
        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                     "IopQueryStopDeviceNode: %wZ forces parent to not participate in REBALANCE\n",
                     PnpGetDeviceInstancePath(DeviceNode)));

        status = STATUS_UNSUCCESSFUL;
        if (ARGUMENT_PRESENT(VetoReason) != FALSE) {
            *VetoReason = DeviceInUnexpectedState;
        }

    } else {

        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                     "IopQueryStopDeviceNode: %wZ cannot participate in REBALANCE\n",
                     PnpGetDeviceInstancePath(DeviceNode)));

        if (ARGUMENT_PRESENT(VetoReason) != FALSE) {
            *VetoReason = DeviceNotSupportQueryRebalance;
        }
    }

    if (!NT_SUCCESS(status)) {

        //
        // On failure, return the information about the failing device for better
        // diagnosis.
        //

        if (ARGUMENT_PRESENT(VetoNode) != FALSE) {
            *VetoNode = DeviceNode;
        }

    } else {
        if (ARGUMENT_PRESENT(VetoNode) != FALSE) {
            *VetoNode = NULL;
        }

        if (ARGUMENT_PRESENT(VetoReason) != FALSE) {
            *VetoReason = DeviceQueryRebalanceSucceeded;
        }
    }

    return status;
}

NTSTATUS
PnpQueryStopDeviceNode (
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    )

/*++

Routine Description:

    This routine sends IRP_MN_QUERY_STOP_DEVICE and updates the devnode state accordingly.

Parameters:

    DeviceNode - Device to be queried.

    RebalanceDueToDynamicPartitioning - TRUE if the query stop for this device is
        due to a rebalance triggered by a dynamic partitioning event;

Return Value:

    Status from the handling of the IRP.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_UNSUCCESSFUL;
    if (DeviceNode->State == DeviceNodeStarted) {
        if ((RebalanceDueToDynamicPartitioning != FALSE) &&
            (PiRebalanceOptOut(DeviceNode) != FALSE)) {

            //
            // This device has opted out of rebalance.
            //

            status = STATUS_UNSUCCESSFUL;
            PnpTraceQueryStopFailure(DeviceNode, status, TRUE);
            return status;
        }

        if (PnpFindMountableDevice(DeviceNode->PhysicalDeviceObject)) {
            PnpLockMountableDevice(DeviceNode->PhysicalDeviceObject);
            PipSetDevNodeFlags(DeviceNode, DNF_LOCKED_FOR_REBALANCE);
        }

        //
        // Call into PO to activate the device prior to starting a rebalance.
        // 

        PNP_ASSERT((DeviceNode->StateFlags & DNSF_POFX_QUERY_STOP) == 0);

        PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
        DeviceNode->StateFlags |= DNSF_POFX_QUERY_STOP;
        status = IopQueryReconfiguration(IRP_MN_QUERY_STOP_DEVICE,
                                         DeviceNode->PhysicalDeviceObject);

        PipSetDevNodeState(DeviceNode, DeviceNodeQueryStopped, NULL);
        if (!NT_SUCCESS(status)) {
            PnpTraceQueryStopFailure(DeviceNode, status, FALSE);
        }
    }

    return status;
}

VOID
PnpCancelStopDeviceNode (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine sends IRP_MN_CANCEL_STOP_DEVICE and updates the devnode state appropriately.

Parameters:

    DeviceNode          - DeviceNode that needs to be targetted.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    if (DeviceNode->State == DeviceNodeQueryStopped) {

        //
        // Call into PO to undo the last activation on this device.
        // 

        if ((DeviceNode->StateFlags & DNSF_POFX_QUERY_STOP) == 0) {
            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                          PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE,
                          (ULONG_PTR) DeviceNode,
                          DNSF_POFX_QUERY_STOP,
                          0);

        }

        PoFxIdleDevice(DeviceNode->PhysicalDeviceObject);
        DeviceNode->StateFlags &= ~DNSF_POFX_QUERY_STOP;
        IopQueryReconfiguration(IRP_MN_CANCEL_STOP_DEVICE,
                                DeviceNode->PhysicalDeviceObject);

        PipRestoreDevNodeState(DeviceNode);
        if (DeviceNode->Flags & DNF_LOCKED_FOR_REBALANCE) {
            PnpUnlockMountableDevice(DeviceNode->PhysicalDeviceObject);
            PipClearDevNodeFlags(DeviceNode, DNF_LOCKED_FOR_REBALANCE);
        }
    }

    return;
}

VOID
PnpStopDeviceSubtree (
    __in PDEVICE_NODE Subtree
    )

/*++

Routine Description:

    This routine sends IRP_MN_STOP_DEVICE to an entire subtree, child first.

Parameters:

    Subtree - Root of the subtree to be walked.

Return Value:

    None.

--*/

{
    PDEVICE_NODE node;

    PAGED_CODE();

    //
    // First stop all children.
    //
    for (node = Subtree->Child; node; node = node->Sibling) {

        //
        // Recurse on children.
        //
        PnpStopDeviceSubtree(node);
    }

    //
    // Stop the root of the subtree.
    //
    PnpStopDeviceNode(Subtree);
}

VOID
PnpStopDeviceNode (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine sends IRP_MN_STOP_DEVICE to the devnode and updates it state appropriately.

Parameters:

    DeviceNode - Devnode to be stopped.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    if (DeviceNode->State == DeviceNodeQueryStopped) {

        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                     "IopStopDeviceNode: Stopping %wZ during REBALANCE\n",
                     PnpGetDeviceInstancePath(DeviceNode)));

        IopQueryReconfiguration(IRP_MN_STOP_DEVICE,
                                DeviceNode->PhysicalDeviceObject);

        PipSetDevNodeState(DeviceNode, DeviceNodeStopped, NULL);
    }

    return;
}

VOID
PnpCancelStopDeviceSubtree (
    __in PDEVICE_NODE Subtree
    )

/*++

Routine Description:

    This routine sends IRP_MN_CANCEL_STOP_DEVICE to an entire subtree, parent first.

Parameters:

    Subtree - Root of the subtree to be walked.

Return Value:

    None.

--*/

{
    PDEVICE_NODE node;

    PAGED_CODE();

    //
    // First send cancel to the root of the subtree.
    //
    PnpCancelStopDeviceNode(Subtree);

    //
    // Send cancels to all children.
    //
    for (node = Subtree->Child; node; node = node->Sibling) {

        //
        // Recurse on children.
        //
        PnpCancelStopDeviceSubtree(node);
    }

    return;
}

NTSTATUS
PnpProcessRebalance (
    __in PPNP_DEVICE_ACTION_ENTRY Request
    )

/*++

Description

    This routine processes a resource rebalance request.

Parameters

    Request - Pointer to the rebalance request.

Return

    NTSTATUS from rebalance operation.

--*/

{
    BOOLEAN rebalanceDueToDynamicPartitioning;
    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    ADD_CONTEXT addContext;


    if (Request->DeviceObject) {
        deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    } else {
        deviceNode = IopRootDeviceNode;
    }

    rebalanceDueToDynamicPartitioning = (BOOLEAN)Request->RequestArgument;

    if (PipIsDevNodeDeleted(deviceNode)) {

        //
        // Drop the ref since the caller is not going to.
        //

        ObDereferenceObject(Request->DeviceObject);
        return STATUS_DELETE_PENDING;
    }

    //
    // Collapse rebalance requests in the device action queue.
    //

    if ((Request != NULL) &&
        (Request->ReorderingBarrier == FALSE) &&
        (rebalanceDueToDynamicPartitioning != FALSE)) {

        if (PiCollapseRebalanceRequests(&Request->ListEntry)) {
            deviceNode = IopRootDeviceNode;
        }
    }

    IopLockResourceManager();

    status = PnpRebalance(deviceNode, NULL, 0, rebalanceDueToDynamicPartitioning);

    IopUnlockResourceManager();

    if (NT_SUCCESS(status)) {

        addContext.DriverStartType = SERVICE_DEMAND_START;
        addContext.LoadDriver = PnPBootDriversInitialized;

        ObReferenceObject(IopRootDeviceNode->PhysicalDeviceObject);
        status = PipProcessDevNodeTree(IopRootDeviceNode,
                                       Request,
                                       &addContext,
                                       EnumTypeNone,
                                       FALSE,    // ReallocateResources
                                       TRUE,     // Synchronous
                                       FALSE);    // ProcessOnlyIntermediateStates
    }

    return status;
}

BOOLEAN
PiCollapseRebalanceRequests(
    PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This function collapses rebalance requests in the device action queue.

Parameters:

    ListHead - The collapsed requests get added to the end of this list.

ReturnValue:

    None.

--*/

{
    KIRQL oldIrql;
    PPNP_DEVICE_ACTION_ENTRY  request;
    PLIST_ENTRY entry, next, last;

    PAGED_CODE();

    ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
    last = ListHead->Blink;

    //
    // Walk the device action queue list and build the list of collapsed
    // requests.
    //

    for (entry = PnpEnumerationRequestList.Flink;
         entry != &PnpEnumerationRequestList;
         entry = next) {

        next = entry->Flink;
        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);

        //
        // If a reorder barrier is encountered, no requests after this point
        // can be considered so stop collapsing here.
        //

        if (request->ReorderingBarrier) {
            break;
        }

        if (request->RequestType == Rebalance) {

            //
            // The boolean passed in as the request argument specifies whether
            // this is a rebalance triggered by a dynamic partitioning action.
            // In such cases its useful to collapse rebalance requests.
            // If the boolen is not set, skip this request and continue through
            // the action queue.
            //

            if (((BOOLEAN)request->RequestArgument) == FALSE) {
                continue;
            }

            IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                         "PiCollapseRebalanceRequests: Collapsing rebalance request for device object 0x%p\n",
                         request->DeviceObject));

            RemoveEntryList(entry);
            InsertTailList(ListHead, entry);
        }
    }

    ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    entry = last->Flink;
    while (entry != ListHead) {
        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);
        ObDereferenceObject(request->DeviceObject);
        request->DeviceObject = NULL;
        entry = entry->Flink;
    }

    return (last != ListHead->Blink)? TRUE : FALSE;
}

BOOLEAN
PiRebalanceOptOut (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine checks the dynamic hardware partitoning rebalance policy for
    the given device to decide whether the device should participate in
    rebalances following a hot add event. If the policy is specified in the
    device's instance key, this value will be given preference.  If the value
    does not exist, is invalid, or specifies no preference, the decision is then
    based on the rebalance policy specified for the class key.

Parameters:

    DeviceNode - Supplies a pointer to the device node for this device.

ReturnValue:

    TRUE if this device should be opted out of rebalance.  FALSE otherwise.

--*/

{
    WCHAR classGuid[GUID_STRING_LEN];
    ULONG classGuidLength;
    DEVPROP_BOOLEAN optOut;
    BOOLEAN rebalanceOptOut;
    ULONG rebalancePolicy = 0;
    ULONG size = 0;
    NTSTATUS status;
    DEVPROPTYPE type = DEVPROP_TYPE_EMPTY;

    PAGED_CODE();

    optOut = FALSE;
    rebalanceOptOut = FALSE;

    //
    // First attempt to find the custom property to opt out of rebalance on DHP
    // events.  If unset, invalid, or indicating no preference, revert to
    // deciding based on the class rebalance property.
    //

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                   PNP_OBJECT_TYPE_DEVICE,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_Device_DHP_Rebalance_Policy,
                                   &type,
                                   (PUCHAR)&rebalancePolicy,
                                   sizeof(rebalancePolicy),
                                   &size,
                                   0);

    if (NT_SUCCESS(status) &&
        (type == DEVPROP_TYPE_UINT32)){

        if (rebalancePolicy == PNP_DHP_REBALANCE_POLICY_OPT_OUT) {
            rebalanceOptOut = TRUE;
            goto exit;
        }

        if (rebalancePolicy == PNP_DHP_REBALANCE_POLICY_OPT_IN) {
            goto exit;
        }
    }

    //
    // No device preference or invalid value found.  Reverting to the class
    // rebalance property.
    //

    classGuidLength = 0;
    status = IoGetDeviceProperty(DeviceNode->PhysicalDeviceObject,
                                 DevicePropertyClassGuid,
                                 sizeof(classGuid),
                                 classGuid,
                                 &classGuidLength);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   classGuid,
                                   PNP_OBJECT_TYPE_DEVICE_INSTALLER_CLASS,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_DeviceClass_DHPRebalanceOptOut,
                                   &type,
                                   (PUCHAR)&optOut,
                                   sizeof(optOut),
                                   &size,
                                   0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    if ((type == DEVPROP_TYPE_BOOLEAN) && (size == sizeof(optOut))) {

        if (optOut == DEVPROP_TRUE) {
            rebalanceOptOut = TRUE;
        }
    }

exit:

    return rebalanceOptOut;
}
