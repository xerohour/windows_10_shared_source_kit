/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    async.h

Abstract:

    This module contains the support for asynchronous PNP IRPs. Currently, we only
    support asynchronous handling of IRP_MN_START_DEVICE and
    IRP_MN_QUERY_DEVICE_RELATIONS-BusRelations.

Author:

    Santosh Jodh (santoshj) 19-Feb-2003

Environment:

    Kernel mode


Revision History:

--*/

#pragma once

//
// These global flags control PNP asynchronous behavior.
//

#define PNP_ASYNC_START_ENABLED 0x00000001
#define PNP_ASYNC_ENUM_ENABLED  0x00000002
#define PNP_ASYNC_TREE_WIDE     0x80000000


typedef struct _PNP_DEVICE_COMPLETION_REQUEST PNP_DEVICE_COMPLETION_REQUEST, *PPNP_DEVICE_COMPLETION_REQUEST;

struct _PNP_DEVICE_COMPLETION_REQUEST {
    LIST_ENTRY                  ListEntry;
    PDEVICE_NODE                DeviceNode;
    PVOID                       Context;
    PNP_DEVNODE_STATE           CompletionState;
    LOGICAL                     IrpPended;
    NTSTATUS                    Status;
    PVOID                       Information;
    __volatile LONG             ReferenceCount;
};

TRIAGE_SIZE_CHECK(PNP_DEVICE_COMPLETION_REQUEST);
TRIAGE_FIELD_CHECK(PNP_DEVICE_COMPLETION_REQUEST, ListEntry);
TRIAGE_FIELD_CHECK(PNP_DEVICE_COMPLETION_REQUEST, DeviceNode);

PPNP_DEVICE_COMPLETION_REQUEST
PnpDeviceCompletionRequestCreate (
    __in PDEVICE_NODE         DeviceNode,
    __in PNP_DEVNODE_STATE    CompletionState,
    __in_opt PVOID                Context
    );

VOID
PnpDeviceCompletionRequestDestroy (
    __inout PPNP_DEVICE_COMPLETION_REQUEST Entry
    );

NTSTATUS
PnpDeviceCompletionProcessCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_REQUEST CompletedRequest
    );

NTSTATUS
PnpDeviceCompletionProcessCompletedRequests (
    __in LOGICAL                      All,
    __in LOGICAL                      Wait,
    __out LOGICAL                     *Completed
    );

extern PNP_DEVICE_COMPLETION_QUEUE PnpDeviceCompletionQueue;
extern ULONG PnpAsyncOptions;
