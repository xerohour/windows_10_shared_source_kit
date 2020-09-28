/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    start.h

Abstract:

    This module implements starting\restarting a device.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

NTSTATUS
PipProcessRestartPhase1(
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    );

NTSTATUS
PipProcessRestartPhase2(
    __in PDEVICE_NODE     DeviceNode
    );

NTSTATUS
PipProcessStartPhase1(
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    );

NTSTATUS
PipProcessStartPhase2(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PipProcessStartPhase3(
    __in PDEVICE_NODE DeviceNode,
    __in LOGICAL MarkForReenumeration
    );

NTSTATUS
PiProcessQueryDeviceState(
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
PiCreateDriverSwDevices(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PiDestroyDriverSwDevices(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpStartDeviceNode (
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Restart,
    __in LOGICAL        Synchronous
    );

NTSTATUS
PnpTraceStartDevice (
    __in PDEVICE_NODE DeviceNode,
    __in NTSTATUS Status,
    __in_opt PDRIVER_OBJECT FailingDriver
    );

