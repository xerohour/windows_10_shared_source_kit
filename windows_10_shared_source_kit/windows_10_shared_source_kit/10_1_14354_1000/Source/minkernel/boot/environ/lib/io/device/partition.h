/*++

Copyright (c) 1990-1997  Microsoft Corporation

Module Name:

    partition.h

Abstract:

    Header file for partition i/o implementation.

Author:

    Jamie Schwartz (jamschw) May 2003

Environment:

    Boot.

Revision History:

--*/

#pragma once

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include "device.h"
#include "block.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------- Definitions

//
// For patched partitions logical byte offset is stached in Attributes field in
// the partition table entry.
//

#define GET_PATCH_PARTITION_LOGICAL_OFFSET(_b)      (((_b) >> 8) << 8)

//
// Size of the patched partition name in characters not including terminating
// null.
//

#define PATCHED_PARTITION_NAME_SIZE (RTL_GUID_STRING_SIZE - 3)

//
// Format string for the patched partition name.
//

#define PATCHED_PARTITION_NAME_FORMAT L"%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x"

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented in this module.  They are used to patch some of the
// block i/o routines in the block i/o function table.
//

NTSTATUS
PartitionOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
PartitionClose (
    __in PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
PartitionEnumerateDeviceClass (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    );

// -------------------------------------------------------- Firmware Prototypes

NTSTATUS
PartitionFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
PartitionFirmwareClose (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    );

// -------------------------------------------------------- Internal Structures

typedef struct _PARTP_MBR_TRAVERSAL_CONTEXT {
    ULONG PartitionNumber;
    PMBR_PARTITION_TABLE_ENTRY MbrEntry;
    ULONG64 PartitionOffset;
    ULONG SectorSize;
} PARTP_MBR_TRAVERSAL_CONTEXT, *PPARTP_MBR_TRAVERSAL_CONTEXT;

/*++

MBR disk traversal context:

    This structure encapsulates the MBR partition information required by the
    callback routines invoked from the generic MBR partition traversal code.

Fields:

    PartitionNumber - Supplies the 1-based number of this partition within
        the disk.

    MbrEntry - Supplies a pointer to the MBR partition table entry describing
        this partition.

    PartitionOffset - Supplies the offset, in bytes, of this partition within
        the enclosing disk.

    SectorSize - Supplies the sector size, in bytes, associated with the
        enclosing disk.

--*/

#define PARTP_USE_DEVICE_ID 1
#define PARTP_USE_DEVICE_ENTRY 2

typedef struct _PARTP_DEVICE_HANDLE {
    ULONG AccessMethod;
    union {
        DEVICE_ID DeviceId;
        PDEVICE_ENTRY DeviceEntry;
    } u;
} PARTP_DEVICE_HANDLE, *PPARTP_DEVICE_HANDLE;

/*++

Device access handle:

    This structure describes a device, indicating whether it should be accessed
    through a standard device ID or be accessed directly through its device
    table entry.

--*/

// -------------------------------------------------------- Internal Prototypes

NTSTATUS
PartpCreatePartitionDeviceEntry (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out PDEVICE_ENTRY *DeviceEntry
    );

NTSTATUS
PartpOpenMbrPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    );

NTSTATUS
PartpOpenGptPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    );

NTSTATUS
PartpOpenElToritoPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    );

NTSTATUS
PartpEnumerateMbrPartitions (
    __in DEVICE_ID ParentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer
    );

NTSTATUS
PartpEnumerateGptPartitions (
    __in DEVICE_ID ParentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN ForceFveCheck
    );

typedef
NTSTATUS
(*PARTP_MBR_TRAVERSAL_CALLBACK) (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __in PVOID Context1,
    __in PVOID Context2,
    __in PVOID Context3
    );

NTSTATUS
PartpTraverseMbrPartitions (
    __in PPARTP_DEVICE_HANDLE ParentDeviceHandle,
    __in PARTP_MBR_TRAVERSAL_CALLBACK Callback,
    __in PVOID Context1,
    __in PVOID Context2,
    __in PVOID Context3
    );

NTSTATUS
PartpMatchMbrPartition (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __in PPARTITION_IDENTIFIER_EX PartitionIdentifier,
    __inout PPARTITION_DEVICE PartitionEntry,
    __in PVOID Context3
    );

NTSTATUS
PartpMatchDeprecatedMbrPartition (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __in PPARTITION_IDENTIFIER PartitionIdentifier,
    __inout PPARTITION_DEVICE PartitionEntry,
    __in PVOID Context3
    );

NTSTATUS
PartpCountMbrPartitions (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __inout PULONG RunningCounter,
    __in PVOID Context2,
    __in PVOID Context3
    );

NTSTATUS
PartpOpenAllMbrPartitions (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __inout PDEVICE_ID *CurrentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __inout PBOOT_ENVIRONMENT_DEVICE IdentifierTemplate
    );

NTSTATUS
PartpOpenVirtualPartitionDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    );

#ifdef ENABLE_PATCHED_PARTITIONS

NTSTATUS
PartpGetPatchInformation (
    __in ULONG PartitionCount,
    __in ULONG PartitionEntrySize,
    __in PGPT_PARTITION_ENTRY PartitionEntryArray,
    __in ULONG BlockSize,
    __in LPCGUID PartitionGuid,
    __inout PDISK_INTERNAL_DATA DiskData
    );

VOID
PartpReleasePatchData (
    __inout PDISK_INTERNAL_DATA DiskData
    );

#endif

#ifdef __cplusplus
}
#endif
