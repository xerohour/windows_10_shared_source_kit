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

#pragma once

PNP_DEVICE_DELETE_TYPE
PiEventRemovalGetDeleteType (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    );

NTSTATUS
PiEventQueryRemoveNoFdo (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    );

VOID
PiEventRemovalOpenHandleVeto (
    _In_ ULONG PdoListCount,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ PDEVICE_OBJECT VetoingDevice,
    _Inout_ PENUM_HANDLES_CONTEXT EnumContext,
    _Out_ PPNP_VETO_TYPE VetoType,
    _Inout_ PUNICODE_STRING VetoString
    );

VOID
PiEventRemovalPostSurpriseRemove (
    _In_ PDEVICE_NODE DeviceNode,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _Inout_ PRELATION_LIST *RelationList
    );

VOID
PiEventRemovalCompleteNotifyKernel (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount
    );

VOID
PiEventRemovalCanceledNotifyKernel (
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount
    );

NTSTATUS
PiEventQueryRemoveNotifyKernel(
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_count_(PdoListCount) PDEVICE_OBJECT *PdoList,
    _In_ ULONG PdoListCount,
    _Out_ PDRIVER_OBJECT *VetoingDriver,
    _Out_ PPNP_VETO_TYPE VetoType,
    _Out_ PUNICODE_STRING VetoString
    );

VOID
PiEventRestartRemovalRelations(
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _In_ PRELATION_LIST RelationList,
    _In_ PDEVICE_OBJECT TargetDeviceObject
    );

NTSTATUS
PiEventBuildPdoList (
    _In_ PRELATION_LIST RelationList,
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _In_ PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    _Out_ PULONG PdoListCount,
    _Outptr_result_buffer_(*PdoListCount) PDEVICE_OBJECT **PdoListOut,
    _Out_ PBOOLEAN PossibleProfileChangeInProgress,
    _Out_ PBOOLEAN SubsumingProfileChange
    );

BOOLEAN
PiEventExcludeDeviceRelations (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType
    );

PVOID
PiEventAllocateVetoBuffer (
    _In_ PNP_DEVICE_DELETE_TYPE DeleteType,
    _Out_ PPNP_VETO_TYPE *VetoTypeOut,
    _Out_ PUNICODE_STRING *VetoStringOut
    );

NTSTATUS
PiEventQueryRemoveDevices (
    _In_ PNP_DEVICE_DELETE_TYPE deleteType,
    _In_ PPNP_DEVICE_EVENT_ENTRY deviceEvent,
    _In_ PDEVICE_OBJECT deviceObject,
    _In_ PRELATION_LIST relationsList,
    _In_ ULONG pdoListCount,
    _In_count_(pdoListCount) PDEVICE_OBJECT *pdoList,
    _Out_ PNP_VETO_TYPE *internalVetoType,
    _Out_ PUNICODE_STRING internalVetoString,
    _Out_ PDEVICE_OBJECT *vetoingDevice
    );
