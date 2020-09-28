/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    enum.h

Abstract:

    This module implements the bringing up of a PNP device.

Author:

    Santosh Jodh (santoshj) 08-09-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

typedef enum _ENUM_TYPE {
    EnumTypeNone,
    EnumTypeShallow,
    EnumTypeDeep,
    EnumTypeIgnoreUninstalledDevices
} ENUM_TYPE;

typedef struct _ADD_CONTEXT {

    ULONG DriverStartType;
    BOOLEAN LoadDriver;

} ADD_CONTEXT, *PADD_CONTEXT;

typedef struct _START_CONTEXT {

    BOOLEAN NewDevice;
    ADD_CONTEXT AddContext;

} START_CONTEXT, *PSTART_CONTEXT;

NTSTATUS
PipProcessDevNodeTree(
    IN  PDEVICE_NODE        SubtreeRootDeviceNode,
    IN  PPNP_DEVICE_ACTION_ENTRY  Request,
    IN  PADD_CONTEXT        AddContext,
    IN  ENUM_TYPE           EnumType,
    IN  LOGICAL             ReallocateResources,
    IN  LOGICAL             Synchronous,
    IN  LOGICAL             ProcessOnlyIntermediateStates
    );

NTSTATUS
PiProcessAddBootDevices(
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
PpProcessClearProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessClearDeviceProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessHaltDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessRequeryDeviceState(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiRestartDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PipMakeGloballyUniqueId(
    __in        PDEVICE_OBJECT ParentDeviceObject,
    __in_opt    PWSTR          UniqueId,
    __deref_out PWSTR          *GloballyUniqueId
    );

NTSTATUS
PiProcessResourceRequirementsChanged(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessReenumeration(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessSetDeviceProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiProcessStartSystemDevices(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

NTSTATUS
PiConfigureDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    );

VOID
PnpCompleteSystemStartProcess(
    VOID
    );

VOID
PpMarkDeviceStackExtensionFlag(
    __in PDEVICE_OBJECT   DeviceObject,
    __in ULONG            ExtensionFlag,
    __in BOOLEAN          Set
    );

NTSTATUS
PnpCallAddDevice (
    __in PDEVICE_NODE       DeviceNode,
    __in PDRIVER_OBJECT     DriverObject,
    __in PDRIVER_ADD_DEVICE AddDeviceRoutine,
    __in ULONG              ServiceType
    );

extern PHALNUMAQUERYPROXIMITYNODE PnpQueryProximityNode;
