/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    relationsp.h

Abstract:

    Private header for relation list management.

Author:

    Minsang Kim (mikim) August 2013

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Constants for resizing DEVICE_OBJECT_LIST_ENTRY array.
//

#define DEVICE_OBJECT_LIST_INITIAL_SIZE 8
#define DEVICE_OBJECT_LIST_MAX_SIZE ULONG_MAX
#define DEVICE_OBJECT_LIST_GROWTH_RATE 2

//
// Flags for DEVICE_OBJECT_LIST_ENTRY
//

#define DEVICE_OBJECT_LIST_ENTRY_TAGGED  (1UL << 0)
#define DEVICE_OBJECT_LIST_ENTRY_VISITED (1UL << 1)
#define DEVICE_OBJECT_LIST_ENTRY_LEAF    (1UL << 2)
#define DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(x) \
    ((x->Flags & DEVICE_OBJECT_LIST_ENTRY_TAGGED) != 0) ? TRUE : FALSE

#define DEVICE_OBJECT_LIST_ENTRY_TAGGED_STATUS(x) \
    (DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(x)) ? DEVICE_TAGGED : DEVICE_UNTAGGED

typedef struct _DEVICE_OBJECT_LIST_ENTRY {
    PDEVICE_OBJECT DeviceObject;
    DEVICE_RELATION_LEVEL RelationLevel;
    ULONG Ordinal;
    ULONG Flags;
} DEVICE_OBJECT_LIST_ENTRY, *PDEVICE_OBJECT_LIST_ENTRY;

//
// Internal data structure for RELATION_LIST.
//

typedef struct _DEVICE_OBJECT_LIST {
    ULONG Count;
    ULONG MaxCount;
    ULONG TagCount;
    PNP_DEVICE_DELETE_TYPE OperationCode;
    DEVICE_OBJECT_LIST_ENTRY Devices[ANYSIZE_ARRAY];
} DEVICE_OBJECT_LIST, *PDEVICE_OBJECT_LIST;

NTSTATUS
PipDeviceObjectListElementAt (
    __in PDEVICE_OBJECT_LIST List,
    __in ULONG Index,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    );

VOID
PiValidatePowerRelations (
    __inout PDEVICE_NODE DevNode
    );

PDEVICE_OBJECT_LIST
PiAllocateDeviceObjectList (
    __in PNP_DEVICE_DELETE_TYPE OperationCode,
    __in ULONG MaxCount
    );

VOID
PiClearDeviceObjectList (
    __inout PDEVICE_OBJECT_LIST List
    );

VOID
PipFreeDeviceObjectList (
    __in PDEVICE_OBJECT_LIST List
    );

LONG
PipDeviceObjectListIndexOf (
    __in PDEVICE_OBJECT_LIST List,
    __in PDEVICE_OBJECT Target,
    __in ULONG EndIndex,
    __out_opt PDEVICE_OBJECT_LIST_ENTRY *Result
    );

NTSTATUS
PipDeviceObjectListRemove(
    __in PDEVICE_OBJECT_LIST List,
    __in ULONG Index
    );

_Must_inspect_result_
NTSTATUS
PipDeviceObjectListAdd (
    __inout PDEVICE_OBJECT_LIST *List,
    __in PDEVICE_OBJECT Device,
    __in DEVICE_RELATION_LEVEL RelationLevel,
    __in DEVICE_TAGGED_FOR_REMOVAL Tagged
    );

BOOLEAN
PipIsDeviceInDeviceObjectList (
    __in PDEVICE_OBJECT_LIST List,
    __in PDEVICE_OBJECT Target,
    __out_opt PDEVICE_OBJECT_LIST_ENTRY *Result
    );

_Requires_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipSortDeviceObjectList (
    __inout PDEVICE_OBJECT_LIST *List
    );

_Requires_lock_held_(PiDependencyRelationsLock)
NTSTATUS
PipVisitDeviceObjectListEntry (
    __in PDEVICE_OBJECT_LIST_ENTRY Device,
    __in PDEVICE_OBJECT_LIST List,
    __inout PULONG Ordinal
    );

NTSTATUS
PipGrowDeviceObjectList (
    __inout PDEVICE_OBJECT_LIST *List
    );

int
__cdecl
PipSortDevicesByOrdinal (
    __in const void *Arg1,
    __in const void *Arg2
    );
