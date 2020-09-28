/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    conflict.h

Abstract:

    This module implements the handling of hardware resource conflicts.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

#define MIN_CONFLICT_LIST_SIZE  (sizeof(PLUGPLAY_CONTROL_CONFLICT_LIST) - sizeof(PLUGPLAY_CONTROL_CONFLICT_ENTRY) + sizeof(PLUGPLAY_CONTROL_CONFLICT_STRINGS))

NTSTATUS
IopQueryConflictListInternal(
    __in PDEVICE_OBJECT        PhysicalDeviceObject,
    __in_bcount(ResourceListSize) PCM_RESOURCE_LIST  ResourceList,
    __in_range(>=, sizeof(CM_RESOURCE_LIST)) ULONG              ResourceListSize,
    __out_bcount(ConflictListSize) PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in ULONG              ConflictListSize,
    __in ULONG              Flags
    );

NTSTATUS
IopQueryConflictFillConflicts(
    __in_opt PDEVICE_OBJECT              PhysicalDeviceObject,
    __in ULONG                    ConflictCount,
    __in_ecount_opt(ConflictCount) PARBITER_CONFLICT_INFO   ConflictInfoList,
    __out_bcount(ConflictListSize) PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in ULONG                    ConflictListSize,
    __in ULONG                    Flags
    );

NTSTATUS
IopQueryConflictFillString(
    __in PDEVICE_OBJECT   DeviceObject,
    __out_ecount_opt(*Length) PWSTR Buffer,
    __inout PULONG       Length,
    __inout_opt PULONG       Flags
    );

BOOLEAN
IopEliminateBogusConflict(
    __in PDEVICE_OBJECT   PhysicalDeviceObject,
    __in PDEVICE_OBJECT   ConflictDeviceObject
    );

NTSTATUS
IopQueryConflictList(
    __in                            PDEVICE_OBJECT                  PhysicalDeviceObject,
    __in_bcount(ResourceListSize)   PCM_RESOURCE_LIST               ResourceList,
    __in_range(>=, sizeof(CM_RESOURCE_LIST))      ULONG                 ResourceListSize,
    __out_bcount(ConflictListSize)  PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in      ULONG                 ConflictListSize,
    __in      ULONG                 Flags
    );

