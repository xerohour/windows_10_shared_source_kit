/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    block.h

Abstract:

    Header file for firmware block i/o stubs.

Environment:

    Boot

--*/

#pragma once

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------- Definitions

#define BLOCK_IO_READ  0
#define BLOCK_IO_WRITE 1
#define BLOCK_IO_FLUSH 2
#define BLOCK_IO_RESET 3

//
// Define block I/O performance enhancement sizes.  The definitions are based
// on firmware type to allow for definitions to be geared specifically to
// address firmware limitations.
//
// The max value for a last block is defined to be 0xFFFFFFFFFFE and
// is used if the real value can not be queried from a firmware implementation.
//
// N.B. This value supports a last block offset of MAX_ULONG64 when the block
//      size is 1MB.
//

#define MAX_LAST_BLOCK ((ULONGLONG)0xFFFFFFFFFFE)

// ------------------------------------------- Block I/O Device Data Structures

typedef struct _RAMDISK_INTERNAL_DATA {
    PUCHAR RamDiskBase;
} RAMDISK_INTERNAL_DATA, *PRAMDISK_INTERNAL_DATA;

/*++

Internal Data for a Ramdisk Device:

    This structure contains internal data for a ramdisk.

Fields:

    TODO: troysh fill this out.

    RamdiskBase -

--*/

typedef struct _PARTITION_PATCH {
    GUID PartitionGuid;
    PDEVICE_ENTRY DeviceEntry;
    ULONGLONG  LogicalOffset;
    ULONGLONG  PhysicalOffset;
    ULONGLONG  Length;

#ifdef EFI

    DEVICE_ENTRY PatchDevice;
    
#endif

} PARTITION_PATCH, *PPARTITION_PATCH;

/*++

Internal Data for Partition Patch:

    This structure is used to describe a patch in patched partition.

Fields:

    PartitionGuid - Id of the patch partition.

    DeviceEntry - Optinal device corresponding to the patch.

    LogicalOffset - Logical byte offset of the patch.

    PhysicalOffset - Physical location of the patch.

    Length - Length of the patch in bytes.

    PatchDevice - On EFI systems local storage for the device entry associated
        with the patch.

--*/

typedef struct _GPT_TABLE_DATA {
    PGPT_PARTITION_ENTRY PartitionEntryArray;
    ULONG PartitionEntrySize;
    ULONG PartitionCount;   
} GPT_TABLE_DATA, *PGPT_TABLE_DATA;

/*++

Internal Data for a GPT Disk:

    This structure contains partition table for GPT disks used on EFI machines.

Fields:

    PartitionEntryArray - Pointer to the partition table.

    PartitionEntrySize - Size of the partition entry in the table.

    PartitionCount - Number of entries in the table.

--*/


typedef struct _DISK_INTERNAL_DATA {
    ULONGLONG StartLbaOffset;
    PVOID ParentDevice;

#ifdef ENABLE_PATCHED_PARTITIONS

    GPT_TABLE_DATA GptData;
    PPARTITION_PATCH Patches;
    ULONG PatchCount;

#endif

} DISK_INTERNAL_DATA, *PDISK_INTERNAL_DATA;

/*++

Internal Data for a Disk Device:

    This structure contains internal data for a disk (or partition) device.

Fields:

    StartLbaOffset - The LBA offset of the device.  All operations are
        performed at LBA + StartLbaOffset.

    ParentDevice - Pointer to the parent device entry. This is used in cases
        where a partition resides on virtual device.

    GptData - Partition table saved away for GPT disks on EFI systems.

    Patches - Pointer to array of PARTITION_PATCH structures describing patches
        making up the partition.

    PatchCount - Number of patches in the partition.

--*/

typedef struct _VHD_FILE_DATA {
    struct _VHD_FILE_DATA *Parent;
    UCHAR Id[16];
    ULARGE_INTEGER OriginalSize;
    ULARGE_INTEGER CurrentSize;
    ULARGE_INTEGER DiskSize;
    ULARGE_INTEGER DataOffset;
    ULARGE_INTEGER BatOffset;
    ULARGE_INTEGER LastByte;
    PULONG Bat;
    FILE_ID FileId;
    ULONG BlockSize;
    ULONG NumberBlocks;
    ULONG SectorsInBlock;
    ULONG SectorsBitmapSize;
    ULONG Type;
    UCHAR ParentId[16];
    PVOID IoContextBlock;
    ULONG SectorSize;
    BOOLEAN IsLeaf;
} VHD_FILE_DATA, *PVHD_FILE_DATA;

/*++

Internal Data for a Virtual hard disk Device:

    This structure contains internal data for a VHD file.

Fields:

    Parent - VHD file information for the parent VHD. This field is used only
        in case of differencing disks.

    Id - The unique GUID for the VHD file.

    DiskSize - Size of the virtual disk.

    DataOffset - Offset to the sparse disk header for dynamic and differencing
        VHDs.

    BatOffset - Byte offset to the starting location of the BAT.

    LastByte - Offset of the last valid byte within the VHD.

    Bat - Pointer to a buffer containing the BAT table.

    FileId - The file id for the VHD file (used in case of differencing VHDs).

    BlockSize - Number of bytes within a VHD block.

    SectorBitmapSize - Size of the sector bitmap buffer.

    Type - VHD disk type.

    ParentId - The unique GUID for the parent VHD. This field is used for
        differencing VHDs only.

    IoContext - Buffer to hold the status of an IO request as the request
        travels through the VHD chain (when blocks are read from the parent
        VHD). This field is used for differencing VHDs only.

    SectorSize - Number of bytes per sector for the virtual disk.

--*/

typedef struct _FILE_DEVICE_INTERNAL_DATA {
    DEVICE_ID DeviceId;
    FILE_ID FileId;
} FILE_DEVICE_INTERNAL_DATA, *PFILE_DEVICE_INTERNAL_DATA;

/*++

Internal Data for a File Device:

    This structure contains internal data for a file device.

Fields:

    DeviceId - The device ID for the device on which the file resides.

    FileId - The file ID for the file being used as a device.

--*/

typedef struct _VIRTUAL_DISK_PARSER VIRTUAL_DISK_PARSER;
typedef const VIRTUAL_DISK_PARSER *PCVIRTUAL_DISK_PARSER;

typedef struct _VIRTUAL_DISK_INTERNAL_DATA {
    DEVICE_ID DeviceId;
    FILE_ID FileId;
    PVOID VhdFileData;
    PVOID ParentDevice;
    PCVIRTUAL_DISK_PARSER Parser;
} VIRTUAL_DISK_INTERNAL_DATA, *PVIRTUAL_DISK_INTERNAL_DATA;

/*++

Internal Data for a File Device:

    This structure contains internal data for a virtual hard disk device.

Fields:

    DeviceId - The device ID for the device on which the backing file resides.

    FileId - The file ID for the backing VHD file.

    VhdFileData - Pointer to a buffer containing information about the VHD file.

    ParentDevice - Pointer to the device entry for the parent device.

    Parser - Pointer to the parser to use to manipulate the VHD.

--*/

typedef struct _BLOCK_IO_INTERNAL_DATA {

    union {
        RAMDISK_INTERNAL_DATA RamDiskData;
        DISK_INTERNAL_DATA    DiskData;
        FILE_DEVICE_INTERNAL_DATA FileData;
        VIRTUAL_DISK_INTERNAL_DATA VhdData;
    } u;

} BLOCK_IO_INTERNAL_DATA, *PBLOCK_IO_INTERNAL_DATA;

/*++

Block I/O Internal Data Description:

    This structure contains internal data for a specific Block I/O device
    class.

Fields:

    u - Union of internal data structures for each device type.

--*/

typedef struct _BLOCK_IO_FIRMWARE_DATA {

#if WIN_APP

    HANDLE Handle;

#elif EFI

    EFI_HANDLE    Handle;
    EFI_BLOCK_IO *BlockIoInterface;

#elif PCAT

    ULONG DriveNumber;

#else

    UCHAR Placeholder;

#endif

} BLOCK_IO_FIRMWARE_DATA, *PBLOCK_IO_FIRMWARE_DATA;

/*++

Block I/O Firmware Data Description:

    Structure containing all firmware data for a Block I/O Device.

Fields:

    WINDOWS CONSOLE APPLICATION:

    Handle - A Win32 Console App will open a Block I/O Device as a file.  All
        interaction with the device is done via this handle.


    EFI:

    Handle - On an EFI platform, an EFI handle is obtained when querying for
        Block I/O Devices.

    BlockIoInterface - Interface obtained via the handle for the particular
        device.


    PCAT:

    DriveNumber - int 13h drive number.

--*/

typedef struct _BLOCK_IO_DEVICE {
    BLOCK_IO_INTERNAL_DATA InternalData;
    BLOCK_IO_INFORMATION Information;
    BLOCK_IO_FIRMWARE_DATA FirmwareData;
} BLOCK_IO_DEVICE, *PBLOCK_IO_DEVICE;

/*++

Block I/O Device Description:

    Data for a Block I/O Device can be partitioned into three categories;
    Internal data used by the Device I/O Library, External data/information
    that can be queried using the Boot Environment Library and firmware
    data used to perform actual device operations.  A Block I/O Device
    contains structures for all three types of data.

Fields:

    InternalData - Data and information consumed by the Device I/O Library
        or by device specific stubs.

    Information - Data that can be queried by the Boot Environment Library
        (BlDeviceGetInformation or BlDeviceSetInformation).

    FirmwareData - Firmware specific data structures.

--*/

//
// A partition is a Block I/O format on a Block I/O Device.  The same data
// structure and routines can be used to do partition I/O as used for other
// block I/O.
//

typedef  BLOCK_IO_DEVICE  PARTITION_DEVICE;
typedef PBLOCK_IO_DEVICE PPARTITION_DEVICE;

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented in this module.  These make up the block i/o device
// function table.
//

NTSTATUS
BlockIoClose (
    __in PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
BlockIoCreate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION CreationOptions,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    );

NTSTATUS
BlockIoEnumerateDeviceClass (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    );

NTSTATUS
BlockIoGetInformation (
    __in PDEVICE_ENTRY DeviceEntry,
    __out PDEVICE_INFORMATION DeviceInformation
    );

NTSTATUS
BlockIoAddDevice (
    __in PDEVICE_ENTRY DeviceEntry,
    __out PDEVICE_ID DeviceId
    );

VOID
BlockIoInitializeFunctionTable (
    __in PDEVICE_ENTRY DeviceEntry
    );


NTSTATUS
BlockIopInitialize (
    VOID
    );

NTSTATUS
BlockIoOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
BlockIoRead (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    );

NTSTATUS
BlockIoReadUsingCache (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    );

NTSTATUS
BlockIoReadVirtualDevice (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    );

NTSTATUS
BlockIoSetInformation (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PDEVICE_INFORMATION DeviceInformation
    );

NTSTATUS
BlockIoWrite (
    __in PDEVICE_ENTRY DeviceEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesWritten
    );

NTSTATUS
BlockIoWriteVirtualDevice (
    __in PDEVICE_ENTRY DeviceEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesWritten
    );

NTSTATUS
BlockIopReadPhysicalDevice (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    );

NTSTATUS
BlockIopOperation (
    __in PDEVICE_ENTRY DeviceEntry,
    __inout PVOID Buffer,
    __in ULONG NumBlocks,
    __in ULONG OperationType
    );

//
// Block cache routines.
//

NTSTATUS
BcCacheRead (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONGLONG DeviceLbo,
    __deref_out_bcount(BLOCK_CACHE_READ_SIZE) PVOID *CacheBuffer
    );

VOID
BcFlushRangeInCache (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONGLONG DeviceLbo,
    __in ULONG Length
    );

NTSTATUS
BcInitialize (
    VOID
    );

NTSTATUS
BcPurge (
    VOID
    );

NTSTATUS
BcReadFromCache (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Length) PVOID Buffer,
    __in ULONGLONG DeviceLbo,
    __in ULONG Length
    );

// -------------------------------------------------------- Firmware Prototypes

NTSTATUS
BlockIoFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PBLOCK_IO_DEVICE BlockIoDevice
    );

NTSTATUS
BlockIoFirmwareRead (
    __in PBLOCK_IO_DEVICE BlockIoDevice,
    __out_ecount(NumberBlocks * BlockIoDevice->Information.BlockSize) PUCHAR Buffer,
    __in ULONGLONG BlockNumber,
    __in ULONGLONG NumberBlocks
    );

NTSTATUS
BlockIoFirmwareWrite (
    __in PBLOCK_IO_DEVICE BlockIoDevice,
    __in_ecount(NumberBlocks * BlockIoDevice->Information.BlockSize) PUCHAR Buffer,
    __in ULONGLONG BlockNumber,
    __in ULONGLONG NumberBlocks
    );

NTSTATUS
BlockIoFirmwareInitialize (
    VOID
    );

// --------------------------------------------------------- Utility Prototypes

VOID
BlockIopDevicePrint (
    __in PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
BlockIopFreeAllocations (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    );

#ifdef ENABLE_PATCHED_PARTITIONS

typedef
NTSTATUS
(*PDEVICE_READ_WRITE_CALLBACK) (
    __in PDEVICE_ENTRY DeviceEntry,
    __inout_bcount(Size) PVOID Buffer,
    __in ULONG64 ByteOffset,
    __in ULONG Size,
    __in ULONG OperationType,
    __out_opt PULONG BytesTransferred
    );

NTSTATUS
BlockIopSplitIo (
    __in PDISK_INTERNAL_DATA DiskData,
    __in PDEVICE_READ_WRITE_CALLBACK Callback,
    __in PDEVICE_ENTRY DeviceEntry,
    __inout_bcount(Size) PVOID Buffer,
    __in ULONG64 Offset,
    __in ULONG Size,
    __in ULONG OperationType,
    __out_opt PULONG BytesTransferred
    );

#endif

// ----------------------------------------------------- Block I/O Device Table

NTSTATUS
BlockIoDeviceTableInitialize (
    VOID
    );

NTSTATUS
BlockIoDeviceTableDestroy (
    VOID
    );

VOID
BlockIoDeviceTablePrint (
    VOID
    );

BOOLEAN
BlockIoDeviceTableCompare (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    );

NTSTATUS
BlockIoDeviceTableDestroyEntry (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONG Index
    );

NTSTATUS
BlockIoDeviceTablePrintEntry (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONG Index
    );

NTSTATUS
BlockIopDestroy (
    VOID
    );

extern PDEVICE_ENTRY *BlockIoDeviceTable;
extern ULONG BlockIoDeviceTableEntries;
extern BOOLEAN BlockIoInitialized;
extern PVOID BlockIopPrefetchBuffer;
extern PVOID BlockIopPartialBlockDevice;
extern PVOID BlockIopPrefetchDevice;
extern PUCHAR BlockIopAlignedBuffer;
extern ULONG BlockIopAlignedBufferSize;
extern PVOID BlockIopPartialBlockBuffer;
extern ULONG BlockIopPartialBlockBufferSize;
extern PVOID BlockIopReadBlockBuffer;
extern ULONG BlockIopReadBlockBufferSize;

#define IsBlockIoDeviceTableUnInitialized() \
        ((BlockIoDeviceTableEntries == 0) ? TRUE : FALSE)

// ------------------------------------------------------ Partition definitions

//
// GPT Partition Header.  Located on two different sectors.  The header is
// always located at LBA GPT_PARTITION_LBA.  Additionally, the header is
// duplicated at the LBA specified in the header field, AlternateLBA.
//

typedef struct _GPT_PARTITION_TABLE {
    UCHAR       Signature[8];
    ULONG       Revision;
    ULONG       HeaderSize;
    ULONG       HeaderCRC;
    ULONG       Reserved;
    unsigned __int64 MyLBA;
    unsigned __int64 AlternateLBA;
    unsigned __int64 FirstUsableLBA;
    unsigned __int64 LastUsableLBA;
    UCHAR       DiskGuid[16];
    unsigned __int64 PartitionEntryLBA;
    ULONG       PartitionCount;
    ULONG       PartitionEntrySize;
    ULONG       PartitionEntryArrayCRC;
    UCHAR       ReservedEnd[1];    // will extend till block size
} GPT_PARTITION_TABLE, *PGPT_PARTITION_TABLE;

// ----------------------------------------------------------- Partition macros

#define GPT_HEADER_SIGNATURE  "EFI PART"
#define GPT_PARTITION_LBA     EFI_PARTITION_LBA

#define MBR_UNIQUE_SIGNATURE_OFFSET 0x1B8
#define MBR_REQUIRED_SIGNATURE      0xAA55
#define MBR_PARTITION_TABLE_OFFSET  0x1BE
#define MBR_PARTITION_TABLE_ENTRIES 0x04
#define MBR_PARTITION_ENTRY_UNUSED  0x00
#define GPT_PROTECTED_MBR_ENTRY     0xEE

// ------------------------------------------------------- Partition prototypes

NTSTATUS
BlockIoGetDiskType (
    __in PDEVICE_ENTRY DeviceEntry,
    __out PULONG DiskType
    );

NTSTATUS
BlockIoGetGPTDiskData (
    __in PDEVICE_ENTRY DeviceEntry,
    __out GUID UNALIGNED *DiskSignature,
    __out_opt PGPT_TABLE_DATA GptData
    );

NTSTATUS
BlockIoGetGPTDiskSignature (
    __in PDEVICE_ENTRY DeviceEntry,
    __out GUID UNALIGNED *DiskSignature
    );

NTSTATUS
BlockIoGetMbrDiskSignature (
    __in PDEVICE_ENTRY DeviceEntry,
    __out ULONG UNALIGNED *DiskSignature
    );

NTSTATUS
BlockIoGetGPTTable (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PGPT_PARTITION_TABLE GptHeader,
    __out PGPT_PARTITION_ENTRY *PartitionTable
    );

// ---------------------------------------------------------------------- Other

//
// Counters for block i/o device types that can only be identified by the order
// they were enumerated by the firmware.
//

extern ULONG BlockIoFirmwareRemovableDiskCount;
extern ULONG BlockIoFirmwareRawDiskCount;
extern ULONG BlockIoFirmwareCdromCount;

extern const DEVICE_FUNCTION_TABLE DiskDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE PartitionDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE FileDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE RamDiskDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE VirtualDiskDeviceFunctionTable;

extern const VIRTUAL_DISK_PARSER Vhd1Parser;
extern const VIRTUAL_DISK_PARSER Vhd2Parser;

#ifdef __cplusplus
}
#endif

