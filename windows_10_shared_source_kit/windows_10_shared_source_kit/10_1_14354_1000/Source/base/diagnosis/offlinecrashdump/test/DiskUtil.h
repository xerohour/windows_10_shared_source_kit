/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   diskutil.h

Environment:
   User Mode

Author:
   jinsh 10/3/2010
--*/

#ifndef __DISKUTIL_H__
#define __DISKUTIL_H__
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>

#include <initguid.h>
#include <diskguid.h>
#include <fmifs.h>
#include <mountmgr.h>
#include <ntddvol.h>
#include <LogLib.hpp>
#include <vds.h>

#include <windef.h>




#define STR_GUID_FMT  L"GUID: {%.8x-%.4x-%.4x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x}"

#define GUID_PRINTF_ARG( X )                                \
    (X).Data1,                                              \
    (X).Data2,                                              \
    (X).Data3,                                              \
    (X).Data4[0], (X).Data4[1], (X).Data4[2], (X).Data4[3], \
    (X).Data4[4], (X).Data4[5], (X).Data4[6], (X).Data4[7]

#define IsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(GUID)))

#define SAFE_RELEASE(x) \
    if (x) { \
        x->Release(); \
        x = NULL; \
    }

#define SAFE_FREE(x) \
if (x) { \
    CoTaskMemFree(x); \
    x = NULL; \
}
#define TEST_FLAG(_f, _b) (((_f) & (_b)) != 0)

#define UNREFERENCED(x) (x)

NTSTATUS
OpenDiskHandle(
    IN INT     DiskN,
    IN PHANDLE Handle
    );

LONGLONG
GetDiskSize(
    IN HANDLE DiskHandle
    );

LONG
GetDiskNumber(
    IN HANDLE DiskHandle
    );

NTSTATUS 
ZeroPartitionFirstSector(
    IN HANDLE DiskHandle,
    IN LONGLONG  Offset
    );
ULONG
GetNewPartitionNumber(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    );

NTSTATUS
PrintDiskLayout(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    );

VOID
GetFreeSpaceAtEnd(
    IN LONGLONG DiskSize,
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout,
    OUT LONGLONG* FreeSpace
    );


NTSTATUS
InsertPartitionGpt(
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout,
    IN ULONG PartitionNumber,
    IN LONGLONG DiskLength, 
    IN LONGLONG PartitionLength,
    IN HANDLE DiskHandle
    );

NTSTATUS
SetDiskLayout(
    IN  HANDLE diskHandle,
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    );

HANDLE
OpenVolume(UINT32 DiskIndex,
           ULONG PartIndex);

BOOL ReadRawDumpPartitionToFile( IN WCHAR* FilePath,
                                 IN HANDLE hVolume,
                                 PPARTITION_INFORMATION_EX PartitionEntry);
HANDLE 
CreateGPTRawDumpPartition( );

HRESULT ShrinkLargetPartition(
      __in  ULONG DiskIndex,
      __in LONGLONG ReqdFreeSpace,
      __out ULONGLONG *volumeOffset,
      __out ULONGLONG *freeSpaceStart);

HRESULT
ShrinkVolume(
    IVdsService * VDSService,
    IVdsDisk *Disk,
    __in  ULONGLONG Offset,
    __out ULONGLONG *StartOffset,
    __in  ULONGLONG DesiredShrinkSize
    );

HRESULT DoShrinkVolume(
    IVdsVolume *Volume, 
    ULONGLONG numberOfBytesToShrink);

HRESULT
FindVolume(
    IVdsService * VDSService,
    IVdsDisk *Disk,
    ULONGLONG Offset,
    IVdsVolume** FirstMatchingVolume
    );

HRESULT
GetDiskExtent(
    IVdsService * VDSService,
    __in IVdsVolume *Volume,
    __deref_out_opt VDS_DISK_EXTENT** DiskExtent
    );

HRESULT
GetLastPartition(
    IVdsDisk* Disk,
    ULONGLONG *Offset
    );

HRESULT GetVdsService(
    IVdsService **VdsService);

HRESULT VDSGetDisk(
    __in IVdsService * VdsService,
    __in_ecount(cchDiskName) LPWSTR DiskName,
    __in size_t cchDiskName,
    __deref_out_opt IVdsDisk **FirstMatchingDisk
    );
#endif // __DISKUTIL_H__