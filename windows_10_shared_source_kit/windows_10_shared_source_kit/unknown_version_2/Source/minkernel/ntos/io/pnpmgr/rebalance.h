/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    rebalance.h

Abstract:

    This module implements the hardware resource rebalance.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

NTSTATUS
PnpRebalance (
    __in PDEVICE_NODE RebalanceNode,
    __in_ecount_opt(RequestTableCount) PPNP_RESOURCE_REQUEST RequestTable,
    __in_opt ULONG RequestTableCount,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    );

VOID
PnpFindRebalanceCandidates (
    __in ULONG Phase,
    __in PDEVICE_NODE DeviceNode,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout OUT PULONG RebalanceTableCount,
    __in BOOLEAN RebalanceDueToDynamicPartitioning,
    __inout PPNP_REBALANCE_TRACE_CONTEXT TraceContext
    );

NTSTATUS
PnpQueryRebalance (
    __in PDEVICE_NODE Subtree,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout PULONG RebalanceTableCount,
    __out_opt PDEVICE_NODE *VetoNode,
    __out_opt PPNP_DEVNODE_QUERY_REBALANCE_VETO_REASON VetoReason,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    );

NTSTATUS
PnpQueryRebalanceWorker (
    __in PDEVICE_NODE DeviceNode,
    __inout_ecount(IopNumberDeviceNodes) PPNP_RESOURCE_REQUEST RebalanceTable,
    __inout PULONG RebalanceTableCount,
    __out_opt PDEVICE_NODE *VetoNode,
    __out_opt PPNP_DEVNODE_QUERY_REBALANCE_VETO_REASON VetoReason,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    );

NTSTATUS
PnpQueryStopDeviceNode (
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN RebalanceDueToDynamicPartitioning
    );

VOID
PnpCancelStopDeviceNode (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpStopDeviceSubtree (
    __in PDEVICE_NODE Subtree
    );

VOID
PnpStopDeviceNode (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpCancelStopDeviceSubtree (
    __in PDEVICE_NODE Subtree
    );

NTSTATUS
PnpProcessRebalance (
    __in PPNP_DEVICE_ACTION_ENTRY Request
    );

extern LOGICAL PnpDisableRebalance;

