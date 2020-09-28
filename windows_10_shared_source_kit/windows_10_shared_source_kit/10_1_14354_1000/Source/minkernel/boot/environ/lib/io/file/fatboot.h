/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    fatboot.h

Abstract:

    Header file for boot environment FAT file system support.

    This header is for boot library internal use only.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include <wchar.h>
#include "fat.h"
#include "lfn.h"
#include "file.h"

// ---------------------------------------------------------------- Definitions

#define FAT_DRIVER_NAME L"fastfat"

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented by this module.  They make up the filesystem function
// table for a FAT partition along with the mount routine.
//

NTSTATUS
FatInitialize (
    VOID
    );

NTSTATUS
FatDestroy (
    VOID
    );

NTSTATUS
FatMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
FatOpen (
    __in PFILE_ENTRY ParentDirectoryEntry,
    __in PWSTR FileName,
    __in OPEN_FLAGS OpenFlags,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
FatClose (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
FatCreate (
    __in DEVICE_ID DeviceId,
    __in PWCHAR OpenPath,
    __inout PFILE_ENTRY FileEntry
    );

NTSTATUS
FatRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
FatWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
FatGetNextFile (
    __in PFILE_ENTRY DirectoryEntry,
    __inout_ecount(*BufferSize) PWSTR FileName,
    __inout PULONG BufferSize
    );

NTSTATUS
FatDelete (
    __in DEVICE_ID DeviceId,
    __in PWCHAR FilePath
    );

NTSTATUS
FatTruncate (
    __in PFILE_ENTRY FileEntry,
    __in LARGE_INTEGER ByteSize
    );

NTSTATUS
FatGetInformation (
    __in PFILE_ENTRY FileEntry,
    __out PFILE_INFORMATION FileInformation
    );

NTSTATUS
FatSetInformation (
    __in PFILE_ENTRY FileEntry,
    __in PFILE_INFORMATION FileInformation
    );

// ------------------------------------------------- Data Structure Definitions

#define FAT_WINDOW_SIZE (512*3)

typedef struct _FAT_DEVICE_CONTEXT {
    BIOS_PARAMETER_BLOCK Bpb;
    BOOLEAN FatWindowDirty;
    ULONG FatWindowOffset;
    UCHAR FatWindow[FAT_WINDOW_SIZE];
} FAT_DEVICE_CONTEXT, *PFAT_DEVICE_CONTEXT;

/*++

FAT Device Context Description:

    This data structure contains FAT file system specific data for a device
    or volume.  The structure includes a copy of the device's BPB as well
    as a buffer to cache a portion of the FileAllocation Table.

Fields:

    Bpb - A copy of the device's bios parameter block.  The bios parameter
          block is a structure located in the first sector of a volume,
          describing the filesystem.

    FatWindow - The File Allocation Table is a per device table.  This buffer
          is used to keep a window of the FAT in memory at all times.  The
          goal is to aid performance by eliminating each file from having
          to read the FAT themselves.

          The cached buffer is 3 * 512 bytes large.  It needs to be a
          multiple of 3 in order fo the size to be a multiple of a
          12-bit, 16-bit and a 32-bit FAT.  LCM(12, 16, 32) = 96 = 3 bytes.

    FatWindowDirty - Indicates if the cached FAT window is dirty (contains
          data in memory that has not yet been written to disk).

    FatWindowOffset - Specifies the starting offset of the FatWindow.

--*/

typedef struct _FAT_FILE_CONTEXT {

    DIRENT              Dirent;
    LBO                 DirentLbo;
    LBO                 FileLbo;

    LBO                 CurrentClusterLbo;
    ULONGLONG           CurrentClusterFileOffset;

} FAT_FILE_CONTEXT, *PFAT_FILE_CONTEXT;

/*++

FAT File Context Description:

    This data structure contains internal FAT data for a particular file.

Fields:

    Dirent - The Dirent for a file.

    DirentLbo - The byte offset for the directory entry.

    FileLbo - The byte offset to the file's first cluster.


    In order to allow a read to start midway through a FAT File Cluster
    chain, a current Lbo is maintained in a addition to to its associating
    file offset.

    CurrentClusterLbo - The byte offset in a device/volume for the current file
        FAT cluster.

    CurrentClusterFileOffset - The file offset associated with the FAT cluster
        with the byte offset, CurrentLbo.

--*/

typedef struct _FAT_FILE {

    FAT_FILE_CONTEXT    FileContext;
    FILE_INFORMATION    Information;
    ULONG               FatDeviceTableId;

} FAT_FILE, *PFAT_FILE;

/*++

FAT File Description:

    This data structure contains the FAT specific information pointed to by
    FILE_ENTRY.FileData.  This structure contains the FILE_INFORMATION
    structure as well as any FAT filesystem specific information.

Fields:

    FileContext - Contains FAT specific information, including a file's
        dirent, FAT offset, etc.

    Information - File Information structure used with BlFileGetInformation
        and BlFileSetInformation.

    FatDeviceTableId - Index into a global table used to keep track of
        device context for multiple FAT volumes.

--*/

//
// Simple enumerated type for describing fat clusters.
//

typedef enum _CLUSTER_TYPE {
    FatClusterAvailable,
    FatClusterReserved,
    FatClusterBad,
    FatClusterLast,
    FatClusterNext
} CLUSTER_TYPE, *PCLUSTER_TYPE;

/*++

Cluster Type Description:

    Enumerated type used to describe FAT cluster.

--*/

// -------------------------------------------------------------------- Globals

//
// There is a Boot Environment Library Table which includes entries containing
// FAT context for a FAT volume.  Each file contains an entry into the table
// for the FAT context that is specific to their FAT volume.  This way, only
// one File Allocation Table is needed per device.  Additionally, this allows
// the table to be cached, eliminating the need to read the table from disk for
// all file operations.
//

PFAT_DEVICE_CONTEXT *FatDeviceTable;
ULONG FatDeviceTableEntries;

//
// To save space in the binary as well as stack space, this global pointer will
// be used to return the filename when it is searched for.  The buffer is
// allocated during FatInitialize and freed during FatDestroy.
//

PWCHAR FatpLongFileName = NULL;

//
// A long file name can be a maximum of 255 characters long. Each one of the
// LFN_DIRENT structures can contain 13 characters.  The calculation of the
// maximum number of LFN_DIRENT entries allowed is a simple one.
//

#define FAT_MAX_LFN_LENGTH  (255)
#define FAT_MAX_LFN_ENTRIES ((FAT_MAX_LFN_LENGTH + 12) / 13)

#define INITIAL_FAT_DEVICE_ENTRIES (2)

// ----------------------------------------------- Internal Function Prototypes

NTSTATUS
FatpSearchForDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __out PDIRENT Dirent,
    __out PLBO DirentLbo
    );

NTSTATUS
FatpGetNextDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __out PDIRENT Dirent,
    __out PLBO DirentLbo
    );

NTSTATUS
FatpGetNextFileNameDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __out PDIRENT Dirent,
    __out_ecount(FAT_MAX_LFN_LENGTH + 1) PWSTR FileName,
    __out PLBO DirentLbo
    );

ULONGLONG
FatpGetDirectorySize (
    __in PFILE_ENTRY DirEntry
    );

LBO
FatpIndexToLbo (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in FAT_ENTRY FatIndex
    );

FAT_ENTRY
FatpLboToIndex (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in LBO ClusterOffset
    );

FAT_ENTRY
FatpFetchFatEntry (
    __in PFILE_ENTRY FileEntry,
    __in FAT_ENTRY FatIndex
    );

NTSTATUS
FatpWindowFlush (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
FatpIoOperation (
    __in PFILE_ENTRY FileEntry,
    __inout_bcount(Size) PUCHAR Buffer,
    __in ULONG Size,
    __out PULONG Count,
    __in BOOLEAN ReadOperation
    );

CLUSTER_TYPE
FatpInterpretClusterType (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in FAT_ENTRY Entry
    );

VOID
FatpResetCurrentClusterPointer (
    __in PFAT_FILE FatFile
    );

VOID
FatpPrintFatWindow (
    __in PFILE_ENTRY FileEntry,
    __in ULONG FatWindowSize
    );

VOID
FatpPrintBiosParameterBlock (
    __in PBIOS_PARAMETER_BLOCK Bpb
    );

VOID
FatpPrint8Dot3 (
    __in PFAT8DOT3 Name8Dot3
    );

VOID
FatpPrintDirectoryEntries (
    __in PFILE_ENTRY DirectoryEntry
    );

NTSTATUS
FatpDeviceTableDestroyEntry (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in ULONG Index
    );

NTSTATUS
FatpCopyPartialLfn (
    __in PDIRENT Dirent,
    __out_ecount(FAT_MAX_LFN_LENGTH + 1) PWCHAR Lfn
    );

// --------------------------------------------------------------------- Macros

#define FatLbo(Bpb) ( (Bpb)->ReservedSectors * (Bpb)->BytesPerSector )

#define Are8Dot3NamesEqual(_x,_y) (                         \
    ((*(_x))[0]==(*(_y))[0]) && ((*(_x))[1]==(*(_y))[1]) && \
    ((*(_x))[2]==(*(_y))[2]) && ((*(_x))[3]==(*(_y))[3]) && \
    ((*(_x))[4]==(*(_y))[4]) && ((*(_x))[5]==(*(_y))[5]) && \
    ((*(_x))[6]==(*(_y))[6]) && ((*(_x))[7]==(*(_y))[7]) && \
    ((*(_x))[8]==(*(_y))[8]) && ((*(_x))[9]==(*(_y))[9]) && \
    ((*(_x))[10]==(*(_y))[10])                              \
)

#define IS_LONGFILENAME_DIRENT(_dir) \
    ((((PDIRENT)(_dir))->Attributes & FAT_DIRENT_ATTR_LFN) ==   \
        FAT_DIRENT_ATTR_LFN)

#define IS_VOLUME_ID_DIRENT(_dir) \
    (((PDIRENT)(_dir))->Attributes & FAT_DIRENT_ATTR_VOLUME_ID)

#define IS_DIRENT_DELETED(_dir) \
    ((((PDIRENT)(_dir))->FileName[0] == FAT_DIRENT_DELETED))

#define IS_DIRENT_NEVER_USED(_dir) \
    ((((PDIRENT)(_dir))->FileName[0] == FAT_DIRENT_NEVER_USED))

#define IS_LAST_LONGFILENAME_DIRENT(_dir) \
    ((((PLFN_DIRENT)(_dir))->Ordinal & FAT_LAST_LONG_ENTRY) ==  \
        FAT_LAST_LONG_ENTRY)

#define GET_LONGFILENAME_ORDINAL(_dir) \
    ((((PLFN_DIRENT)(_dir))->Ordinal) & (FAT_LAST_LONG_ENTRY-1))

#define GET_LONGFILENAME_CHECKSUM(_dir) \
    (((PLFN_DIRENT)(_dir))->Checksum)


