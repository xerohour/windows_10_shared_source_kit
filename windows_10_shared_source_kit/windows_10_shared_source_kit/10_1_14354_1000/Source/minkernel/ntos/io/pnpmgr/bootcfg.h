/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    bootcfg.h

Abstract:

    This module implements the handling of boot configurations.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

typedef struct _IOP_RESERVED_RESOURCES_RECORD IOP_RESERVED_RESOURCES_RECORD, *PIOP_RESERVED_RESOURCES_RECORD;

struct _IOP_RESERVED_RESOURCES_RECORD {
    PIOP_RESERVED_RESOURCES_RECORD  Next;
    PDEVICE_OBJECT                  DeviceObject;
    PCM_RESOURCE_LIST               ReservedResources;
};

//
// BOOT allocation related declarations.
//

typedef
NTSTATUS
(*PIO_ALLOCATE_BOOT_RESOURCES_ROUTINE) (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopAllocateBootResources (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopReportBootResources (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopAllocateLegacyBootResources (
    __in INTERFACE_TYPE   InterfaceType,
    __in ULONG            BusNumber
    );

NTSTATUS
IopBootAllocation (
    __in PREQ_LIST ReqList
    );

NTSTATUS
IopAllocateBootResourcesInternal (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    );

PCM_RESOURCE_LIST
IopCreateCmResourceList(
    __in PCM_RESOURCE_LIST ResourceList,
    __in INTERFACE_TYPE InterfaceType,
    __in ULONG   BusNumber,
    __out PCM_RESOURCE_LIST *RemainingList
    );

PCM_RESOURCE_LIST
IopCombineCmResourceList(
    __in PCM_RESOURCE_LIST ResourceListA,
    __in PCM_RESOURCE_LIST ResourceListB
    );

LOGICAL
PnpNeedToReleaseBootResources (
    __in PDEVICE_NODE       DeviceNode,
    __in PCM_RESOURCE_LIST  AllocatedResources,
    __out PLOGICAL          HasMoved
    );

VOID
IopReleaseFilteredBootResources(
    __in PPNP_RESOURCE_REQUEST AssignTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST AssignTableEnd
    );

LOGICAL
PnpIsRangeWithin (
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Inner,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Outer
    );

extern PIO_ALLOCATE_BOOT_RESOURCES_ROUTINE IopAllocateBootResourcesRoutine;

