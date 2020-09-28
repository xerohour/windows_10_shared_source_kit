/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    remove.h

Abstract:

    This module contains the private (internal) header file for PNP device removal.

Author:

    Santosh Jodh (santoshj) 12-Jul-2004


Revision History:

--*/

#pragma once

//
// Define device removal types.
//

typedef enum _PNP_DEVICE_DELETE_TYPE {
    QueryRemoveDevice,
    CancelRemoveDevice,
    RemoveDevice,
    SurpriseRemoveDevice,
    EjectDevice,
    RemoveFailedDevice,
    RemoveUnstartedFailedDevice,
    MaxDeviceDeleteType
} PNP_DEVICE_DELETE_TYPE, *PPNP_DEVICE_DELETE_TYPE;

typedef enum _PNP_UNLOCK_UNLINK_ACTION {
    UnlinkRemovedDeviceNodes,
    UnlinkAllDeviceNodesPendingClose,
    UnlinkOnlyChildDeviceNodesPendingClose
} PNP_UNLOCK_UNLINK_ACTION, *PPNP_UNLOCK_UNLINK_ACTION;

VOID
PnpRequestDeviceRemoval (
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN      TreeDeletion,
    __in ULONG        Problem,
    __in NTSTATUS     ProblemCode
    );

BOOLEAN
PnpIsBeingRemovedSafely (
    __in PDEVICE_NODE DeviceNode,
    __in DEVICE_RELATION_LEVEL RelationLevel
    );

NTSTATUS
PnpBuildRemovalRelationList (
    __in  PDEVICE_OBJECT            DeviceObject,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __out PNP_VETO_TYPE             *VetoType,
    __out PUNICODE_STRING           VetoName,
    __out PRELATION_LIST            *RelationsList
    );

NTSTATUS
PnpDeleteLockedDeviceNodes (
    __in  PDEVICE_OBJECT            DeviceObject,
    __in  PRELATION_LIST            DeviceRelations,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  BOOLEAN                   ProcessIndirectDescendants,
    __in  ULONG                     Problem,
    __in  NTSTATUS                  ProblemCode,
    __out_opt PNP_VETO_TYPE         *VetoType,
    __out_opt PUNICODE_STRING       VetoName
    );

VOID
PnpUnlinkDeviceRemovalRelations (
    __in    PDEVICE_OBJECT              RemovedDeviceObject,
    __inout PRELATION_LIST              RelationList,
    __in    PNP_UNLOCK_UNLINK_ACTION    UnlinkAction
    );

NTSTATUS
PnpInvalidateRelationsInList (
    __in  PRELATION_LIST            RelationList,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  BOOLEAN                   OnlyIndirectDescendants,
    __in  BOOLEAN                   RestartDevNode
    );

BOOLEAN
PnpQueuePendingEject (
    __in PPENDING_RELATIONS_LIST_ENTRY Entry
    );

WORKER_THREAD_ROUTINE PnpProcessCompletedEject;

VOID
PnpQueuePendingSurpriseRemoval (
    __in PDEVICE_OBJECT DeviceObject,
    __inout PRELATION_LIST *List,
    __in ULONG Problem,
    __in PNP_DEVICE_DELETE_TYPE DeleteType
    );

_Requires_exclusive_lock_held_(IopSurpriseRemoveListLock)
VOID
PnpDequeuePendingSurpriseRemoval (
    _Inout_ PLIST_ENTRY ReadyListHead
    );

NTSTATUS
PnpUnloadAttachedDriver (
    __in PDRIVER_OBJECT DriverObject
    );

NTSTATUS
PnpDeleteLockedDeviceNode (
    __in  PDEVICE_NODE              DeviceNode,
    __in  PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in  ULONG                     Problem,
    __in  NTSTATUS                  ProblemStatus,
    __out_opt PNP_VETO_TYPE         *VetoType,
    __out_opt PUNICODE_STRING       VetoName
    );

NTSTATUS
PnpProcessRelation (
    __in     PDEVICE_NODE           DeviceNode,
    __in     PNP_DEVICE_DELETE_TYPE OperationCode,
    __in     DEVICE_RELATION_LEVEL  RelationLevel,
    __out    PNP_VETO_TYPE          *VetoType,
    __out    PUNICODE_STRING        VetoName,
    __inout  PRELATION_LIST         RelationList
    );

VOID
PnpSurpriseRemoveLockedDeviceNode (
    __in      PDEVICE_NODE     DeviceNode,
    __in      ULONG            Problem,
    __in      NTSTATUS         ProblemStatus
    );

NTSTATUS
PnpQueryRemoveLockedDeviceNode (
    __in  PDEVICE_NODE        DeviceNode,
    __in  ULONG               Problem,
    __out PNP_VETO_TYPE      *VetoType,
    __out PUNICODE_STRING     VetoName
    );

VOID
PnpCancelRemoveLockedDeviceNode (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpRemoveLockedDeviceNode (
    __in     PDEVICE_NODE    DeviceNode,
    __in     ULONG           Problem,
    __in     NTSTATUS        ProblemStatus
    );

_Requires_exclusive_lock_held_(PiDependencyRelationsLock)
VOID
PnpRequestDeviceRemovalWorker (
    __in PDEVICE_NODE TargetDeviceNode,
    __in PDEVICE_NODE DeviceNode,
    __in BOOLEAN TreeDeletion,
    __in DEVICE_RELATION_LEVEL RelationLevel,
    __in BOOLEAN DescendantNode
    );

NTSTATUS
PnpProcessBusRelations (
    __in     PDEVICE_NODE           DeviceNode,
    __in     PNP_DEVICE_DELETE_TYPE OperationCode,
    __in     DEVICE_RELATION_LEVEL  RelationLevel,
    __out    PNP_VETO_TYPE          *VetoType,
    __out    PUNICODE_STRING        VetoName,
    __inout  PRELATION_LIST         RelationList
    );

NTSTATUS
PnpProcessDependencyRelations(
    __in      PDEVICE_NODE              DeviceNode,
    __in      PNP_DEVICE_DELETE_TYPE    OperationCode,
    __in      DEVICE_RELATION_LEVEL     RelationLevel,
    __out     PNP_VETO_TYPE             *VetoType,
    __out     PUNICODE_STRING           VetoName,
    __inout   PRELATION_LIST            RelationList
    );

NTSTATUS
PnpCancelRemoveOnHungDevices (
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_ BOOLEAN ExcludeDeviceRelations,
    _In_ PRELATION_LIST RelationList,
    _Out_ PWSTR *RelatedDeviceInstances
    );