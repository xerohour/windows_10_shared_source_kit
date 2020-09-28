/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    action.h

Abstract:

    This module implements the PNP device actions.

Author:

    Santosh Jodh (santoshj) 08-09-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// PNP device actions.
//

typedef enum _PNP_DEVICE_ACTION_REQUEST {
    AssignResources,
    ClearDeviceProblem,
    ClearProblem,
    ClearEjectProblem,
    HaltDevice,
    QueryPowerRelations,
    Rebalance,
    ReenumerateBootDevices,
    ReenumerateDeviceOnly,
    ReenumerateDeviceTree,
    ReenumerateRootDevices,
    RequeryDeviceState,
    ResetDevice,
    ResourceRequirementsChanged,
    RestartEnumeration,
    SetDeviceProblem,
    StartDevice,
    StartSystemDevicesPass0,
    StartSystemDevicesPass1,
    NotifyTransportRelationsChange,
    NotifyEjectionRelationsChange,
    ConfigureDevice,
    ConfigureDeviceClass,
    ConfigureDeviceExtensions,
    ConfigureDeviceReset,

} PNP_DEVICE_ACTION_REQUEST;

struct _PNP_DEVICE_ACTION_ENTRY {
    LIST_ENTRY                  ListEntry;
    PDEVICE_OBJECT              DeviceObject;
    PNP_DEVICE_ACTION_REQUEST   RequestType;
    BOOLEAN                     ReorderingBarrier;
    ULONG_PTR                   RequestArgument;
    PKEVENT                     CompletionEvent;
    PNTSTATUS                   CompletionStatus;
    GUID                        ActivityId;

};

NTSTATUS
PnpRequestDeviceAction (
    __in_opt    PDEVICE_OBJECT DeviceObject,
    __in        PNP_DEVICE_ACTION_REQUEST RequestType,
    __in        BOOLEAN ReorderingBarrier,
    __in        ULONG_PTR Argument,
    __in_opt    PKEVENT CompletionEvent,
    __out_opt   PNTSTATUS CompletionStatus
    );

VOID
PnpRemoveDeviceActionRequests (
    __in PDEVICE_OBJECT DeviceObject
    );

VOID
PnpLockDeviceActionQueue (
    VOID
    );

VOID
PnpUnlockDeviceActionQueue (
    VOID
    );

VOID
PnpDeviceActionWorker (
    __in_opt PVOID Context
    );

NTSTATUS
PnpInitializeDeviceActions (
    VOID
    );

typedef enum _PNP_ACTION_QUEUE_OP {
    PNP_ACTION_QUEUE,
    PNP_ACTION_START,
    PNP_ACTION_STOP

} PNP_ACTION_QUEUE_OP;

VOID
PnpLogActionQueueEvent(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEVICE_ACTION_REQUEST RequestType,
    __in PNP_ACTION_QUEUE_OP Op
    );

//
// This is the head of the list of queued device actions.
//

extern LIST_ENTRY PnpEnumerationRequestList;
