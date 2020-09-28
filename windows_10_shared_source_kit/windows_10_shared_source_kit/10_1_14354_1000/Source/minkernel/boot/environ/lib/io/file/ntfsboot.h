/*++

Copyright (c) 1990-1997  Microsoft Corporation

Module Name:

    ntfsboot.h

Abstract:

    Header file for native boot environment NTFS file system support.

    This header is for boot library internal use only.

Author:

    Jamie Schwartz (jamschw) August 2003

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include "lfs.h"
#include "ntfs.h"
#include <wchar.h>
#include "file.h"
#include <lzx.h>

// ---------------------------------------------------------------- Definitions

#define NTFS_DRIVER_NAME L"ntfs"

//
// Size of FRS hash table. Should be a power of 2 or else the hash function
// breaks.
//

#define FILE_RECORD_HASH_TABLE_SIZE 128

#define NTFS_ATTRIBUTE_RESIDENT       0x80000000
#define NTFS_ATTRIBUTE_COMPRESSED     ATTRIBUTE_FLAG_COMPRESSION_MASK
#define NTFS_ATTRIBUTE_CACHE_READ     0x40000000

#ifndef CONSTANT_UNICODE_STRING
#define CONSTANT_UNICODE_STRING(s)   { sizeof( s ) - sizeof( WCHAR ), sizeof( s ), s }
#endif

//
// Chunk sizes for WIM compressed files.
//

#ifndef FILE_MIN

#define WIM_LZX_CHUNK_SIZE    0x8000
#define WIM_XPRESS_CHUNK_SIZE 0x1000

#endif

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented by this module.  They make up the filesystem function
// table for a NTFS partition.
//

NTSTATUS
NtfsInitialize (
    VOID
    );

NTSTATUS
NtfsDestroy (
    VOID
    );

NTSTATUS
NtfsMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
NtfsOpen (
    __in PFILE_ENTRY ParentDirectoryEntry,
    __in PWSTR FileName,
    __in OPEN_FLAGS OpenFlags,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
NtfsClose (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
NtfsCreate (
    __in DEVICE_ID DeviceId,
    __in PWCHAR OpenPath,
    __inout PFILE_ENTRY FileEntry
    );

NTSTATUS
NtfsRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
NtfsWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
NtfsGetNextFile (
    __in PFILE_ENTRY DirectoryEntry,
    __inout_ecount(*BufferSize) PWSTR FileName,
    __inout PULONG BufferSize
    );

NTSTATUS
NtfsDelete (
    __in DEVICE_ID DeviceId,
    __in PWCHAR FilePath
    );

NTSTATUS
NtfsTruncate (
    __in PFILE_ENTRY FileEntry,
    __in LARGE_INTEGER ByteSize
    );

NTSTATUS
NtfsGetInformation (
    __in PFILE_ENTRY FileEntry,
    __out PFILE_INFORMATION FileInformation
    );

NTSTATUS
NtfsSetInformation (
    __in PFILE_ENTRY FileEntry,
    __in PFILE_INFORMATION FileInformation
    );

NTSTATUS
NtfsGetReparseData (
    __in PFILE_ENTRY FileEntry,
    __out PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    );

NTSTATUS
NtfsPurgeCache (
    VOID
    );

// ------------------------------------------------- Data Structure Definitions

typedef struct _FILE_RECORD_SEGMENT_KEY {
    ULONGLONG FrsNumber;
} FILE_RECORD_SEGMENT_KEY, *PFILE_RECORD_SEGMENT_KEY;

/*++

Cached File Record Segment Description:

    Each mounted NTFS volume maintains a hash table of File Record Segments to
    increase File I/O performance. The FRS number is the key to a hash entry.

Fields:

    FrsNumber - The file record segment number.

--*/

typedef struct _NTFS_ATTRIBUTE_CONTEXT {
    ULONGLONG FileRecordSegment;
    ULONG FileRecordOffset;
    ATTRIBUTE_TYPE_CODE TypeCode;
    ULONGLONG Size;
    ULONG Flags;
    ULONG CompressionUnit;
} NTFS_ATTRIBUTE_CONTEXT, *PNTFS_ATTRIBUTE_CONTEXT;

/*++

Ntfs Attribute Context Description:

    Every file record segment is comprised of attributes.  This data structure
    is used to describe an attribute in a FRS.

Fields:

    FileRecordSegement - The file record segment containing the specified
        attribute.

    FileRecordOffset - Offset within the file record segment to the specified
        attribute's header.

    TypeCode - The type code for the attribute.

    Size - The size of the attribute value.

    Flags - A bitmask field used to describe characteristics of an attribute.
        Possible values include:

        NTFS_ATTRIBUTE_RESIDENT : When set, indicates that an attribute is
            resident in the file record segment described by the attribute
            field, FileRecordSegment;

        NTFS_ATTRIBUTE_COMPRESSED : When set, indicates that an attribute is
            compressed.  The value in this bitmask refers to the compression
            format which is [(Flags & NTFS_ATTRIBUTE_COMPRESSED) + 1].

    CompressionUnit - When an attribute is compressed, this field describes
        the number of bytes in each unit of compression.  The value is
        2^n * BytesPerCluster where n = {1, 2, 3, 4, 5}.

--*/

typedef struct _NTFS_DEVICE_CONTEXT {
    DEVICE_ID DeviceId;
    ULONG ContextIndex;
    BIOS_PARAMETER_BLOCK Bpb;
    LONGLONG NumberSectors;
    LCN MftStartLcn;
    ULONG BytesPerFileRecordSegment;
    ULONG DefaultBytesPerIndexAllocationBuffer;
    NTFS_ATTRIBUTE_CONTEXT MftAttribute;
    HASH_TABLE_ID FileRecordHashTableId;
    ULONG ClusterMask;
    ULONG ClusterShift;
    ULONGLONG TotalClusters;
} NTFS_DEVICE_CONTEXT, *PNTFS_DEVICE_CONTEXT;

/*++

Ntfs Device Context Description:

    This structure contains information pertaining to a NTFS volume,
    including the device's BIOS Parameter Block as well as a window of the
    Master File Table.

Fields:

    DeviceId - Device ID for the device containing a NTFS volume.

    ContextIndex - Index for this entry in NtfsDeviceTable.

    Bpb - A copy of the device's Bios Parameter Block.  The BPB is a structure
        located in the sector of a partition (or device) and describes the
        filesystem on the partition.

    NumberSector - The size of the NTFS volume (in sectors).

    MftStartLcn - The offset to the Master File Table.

    BytesPerFileRecordSegment - The size (in bytes) of a file record segment.

    DefaultBytesPerIndexAllocationBuffer - The default size (in bytes) of
        an Index Allocation Buffer.

    MftAttribute - Attribute context for the Master File Table's $DATA
        attribute.  If the attribute spans multiple records, and the file
        contains multiple $DATA attribute records, this field stores the
        context for the primary (LowestLcn == 0) record.

        N.B. The FileSize field of a primary attribute record contains the
             size of all attributes records of the same type code.  Therefore,
             the FileSize field in the NTFS_ATTRIBUTE_CONTEXT will accurately
             describe the size of all $DATA attributes.

    FileRecordCache - Each device will cache recently read file record
        segments.  For each device, an array (Boot Environment Library Table)
        is used to maintain a cache of FRS.  This is a pointer to the
        table.

    FileRecordCacheEntries - The size of FileRecordCache.

--*/

typedef struct {
    ULONG BytesPerIndexBuffer;
    ULONG FirstIndexEntry;
} NTFS_INDEX_ROOT, *PNTFS_INDEX_ROOT;

/*++

Ntfs Index Root:

    This data structure is used to cache Index Root data for a directory.
    The fields of the structure represent data that is repeatedly required
    from an Index Root Attribute.  The goal is to cache the data and minimize
    the number of actual attribute reads.

Fields:

    BytesPerIndexBuffer - The size (in bytes) of each Index Allocation
        Buffer.

    FirstIndexEntry - Offset to the first Index Entry in the Index Root
        Attribute.

--*/

typedef struct _NTFS_INDEX_ALLOCATION {
    VBO IndexAllocationBuffer;
    PMULTI_SECTOR_HEADER MultiSectorHeader;
    ULONG FirstIndexEntry;
} NTFS_INDEX_ALLOCATION, *PNTFS_INDEX_ALLOCATION;

/*++

Ntfs Index Allocation:

    This data structure is used to cache Index Allocation data for a directory.
    The fields of the structure represent data that is repeatedly required
    from an Index Allocation Buffer Attribute.  The goal is to cache the data
    and minimize the number of actual attribute reads.

Fields:

    IndexAllocationBuffer - Specifies the index allocation buffer that is
        cached by this structure.  The value is 1-based.

        N.B. This value is 1-based to distinguish index buffer 0 from an
             uninitialized value.

    MultiSectorHeader - Pointer ot the Multi Sector Header (include the
        Update Sequence Array) for the current Index Allocation Buffer.

    FirstIndexEntry - Offset to the first Index Entry in the current Index
        Allocation Buffer.

--*/

typedef struct _NTFS_FILE_CONTEXT {
    ULONGLONG BaseFileRecordSegment;
    union {
        struct {
            NTFS_ATTRIBUTE_CONTEXT DataAttribute;
            PULONG ChunkTable;
            ULONG CompressedSize;
            ULONG OriginalSize;
            ULONG CompressedChunkSize;
            ULONG Algorithm;
            BOOLEAN WimCompressed;
        };
        struct {
            NTFS_ATTRIBUTE_CONTEXT IndexRootAttribute;
            NTFS_ATTRIBUTE_CONTEXT IndexAllocationAttribute;
            NTFS_ATTRIBUTE_CONTEXT BitmapAttribute;
            NTFS_INDEX_ROOT IndexRoot;
            NTFS_INDEX_ALLOCATION IndexAllocation;
            ULONGLONG GetNextFileOffset;
            struct {
                ULONG UseGetNextFileOffsetOnOpen : 1;
                ULONG Reserved                   : 31;
            };
        };
    };

} NTFS_FILE_CONTEXT, *PNTFS_FILE_CONTEXT;

/*++

Ntfs File Context Description:

    This data structure contains internal NTFS specific information for a
    file.

Fields:

    BaseFileRecordSegment - Base FRS for a file.

    DataAttribute (File only) - Context for a file's $DATA attribute.  If the
        attribute spans multiple records, and the file contains multiple $DATA
        attribute records, this field stores the context for the primary
        (LowestLcn == 0) record.

        N.B. The FileSize field of a primary attribute record contains the
             size of all attributes records of the same type code.  Therefore,
             the FileSize field in the NTFS_ATTRIBUTE_CONTEXT will accurately
             describe the size of all $DATA attributes.

    The next fields for files are used for WIM compressed files.

    ChunkTable (File only) - Chunk table for WIM compressed files.

    CompressedSize (File only) - Compressed file size (chunk table and chunks).

    OriginalSize (File only) - Uncompressed file size.

    CompressedChunkSize (File only) - Size of the compression chunk.

    Algorithm (File only) - Compression algorithm used.

    WimCompressed (File only) - Boolean indicating that the file is WIM
        compressed.

    IndexRootAttribute (Directory only) - Context for a directory's
        $INDEX_ROOT attribute.

    IndexAllocationAttribute (Directory only) - Context for a directory's
        $INDEX_ALLOCATION attribute.  If the attribute spans multiple records,
        and the file contains multiple $INDEX_ALLOCATION attribute records,
        this field stores the context for the primary (LowestLcn == 0) record.

        N.B. The FileSize field of a primary attribute record contains the
             size of all attributes records of the same type code.  Therefore,
             the FileSize field in the NTFS_ATTRIBUTE_CONTEXT will accurately
             describe the size of all $INDEX_ALLOCATION attributes.

    BitmapAttribute (Directory only) - Context for a directory's $BITMAP
        attribute.

    The next two structures are used to cache information for a directory.
    The structures contain data that is repeatedly required from either
    the Index Root Attribute or the current Index Allocation Buffer.  The
    goal is to cache the data and minimize the number of actual attribute
    reads.

    IndexRoot (Directory only) :

        BytesPerIndexBuffer - The size (in bytes) of each Index Allocation
            Buffer.

        FirstIndexEntry - Offset to the first Index Entry in the Index
            Root Attribute.

    IndexAllocation (Directory only) :

        MultiSectorHeader - Pointer ot the Multi Sector Header (include the
            Update Sequence Array) for the current Index Allocation Buffer.

        FirstIndexEntry - Offset to the first Index Entry in the current
            Index Allocation Buffer.

    GetNextFileOffset (Directory only) - Contains a cached offset to the last
        file returned by the GetNextFile API.

    GetNextFileOffsetOnOpen (Directory only) - Indicates if the get next file
        offset should be used when searching for an index entry on file open.

--*/

typedef struct _NTFS_FILE {
    NTFS_FILE_CONTEXT FileContext;
    FILE_INFORMATION Information;
    WIM_PROVIDER_EXTERNAL_INFO ReparseData;
    ULONG NtfsDeviceTableId;
} NTFS_FILE, *PNTFS_FILE;

/*++

Ntfs File Description:

    This data structure contains the NTFS specific information pointed to
    by FILE_ENTRY.FileData.  This structure contains the FILE_INFORMATION
    structure as well as any NTFS filesystem specific information.

Fields:

    FileContext - Contains NTFS specific information, including a file's
        base FRS and $DATA attribute.

    Information - File Information structure used with BlFileGetInformation
        and BlFileSetInformation.

    NtfsDeviceTableId - Index into a global table used to keep track of
        device context for multiple NTFS volumes.

--*/

typedef union _WOF_REPARSE_DATA {
    REPARSE_DATA_BUFFER ReparseHeader;
    struct {
        UCHAR ReparseHeader[REPARSE_DATA_BUFFER_HEADER_SIZE];
        WOF_EXTERNAL_INFO WofInfo;
        union {
            WIM_PROVIDER_EXTERNAL_INFO WimInfo;
            FILE_PROVIDER_ON_DISK_EXTERNAL_INFO_V0 FileInfo;
        };
    } Data;
} WOF_REPARSE_DATA, *PWOF_REPARSE_DATA;

/*++

Wof Reparse Data Description:

    This data structure is used for accessing reparse data for files handled
    by WOF filter.

Fields:

    ReparseHeader - Standard NTFS header for reparse points.

    WofInfo - Common header for files handled by WOF filter.

    WimInfo - Information for files comming out of the WIM.

    FileInfo - Information for file provider compressed files.

--*/

typedef struct _NTFS_COMPRESSION_CONTEXT {
    PVOID CompressedDataBuffer;
    PCHAR UncompressedChunkBuffer;
    PVOID LZXDecompressContext;
    PVOID LZXWorkspace;
    PVOID XpressWorkspace;
    ULONG CompressedBufferSize;
    BOOLEAN Initialized;
} NTFS_COMPRESSION_CONTEXT, *PNTFS_COMPRESSION_CONTEXT;


/*++

Ntfs Compression Context Description:

    This data structure is used to hold pointers to buffers/workspaces used for
    WIM based compression. Because only one file is going to be decompressed
    at a time this structure is single instance. The buffers are allocated
    lazily when compressed files are encountered.

Fields:

    CompressedDataBuffer - Buffer used for reading compressed data from the
        file.

    UncompressedChunkBuffer - Buffer used to hold uncompressed data.

    LZXDecompressContext - Context used by LZX comrpession engine.
   
    LZXWorkspace - Workspace used by LZX compression engine.

    XpressWorkspace - Workspace used by XPress compression engine.

    CompressedBufferSize - Size of the buffer used for reading compressed data.

    Initialized - Boolean to track if the structure has been initialized.

--*/

// -------------------------------------------------------------------- Globals

//
// The NTFS File System code includes a Table to cache device context for each
// NTFS volume that has been mounted.  Each file contains a reference into the
// table for their specific NTFS volume.
//

PNTFS_DEVICE_CONTEXT *NtfsDeviceTable;
ULONG NtfsDeviceTableEntries;

#define INITIAL_NTFS_DEVICE_ENTRIES (2)

//
// ----------------------------------------------- Internal Function Prototypes
//

NTSTATUS
NtfspDeviceReadAtOffset (
    __in_opt PNTFS_DEVICE_CONTEXT VolumeContext,
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead
    );

NTSTATUS
NtfspDeviceWriteAtOffset (
    __in_opt PNTFS_DEVICE_CONTEXT VolumeContext,
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __in_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesWritten
    );

//
// Local routine prototypes for device and file initialization.
//

NTSTATUS
NtfspInitializeDeviceContext (
    __in DEVICE_ID DeviceId,
    __in PPACKED_BOOT_SECTOR BootSector,
    __deref_out PNTFS_DEVICE_CONTEXT *DeviceContext
    );

NTSTATUS
NtfspInitializeFileContext (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __deref_out PNTFS_FILE *FileContext
    );

NTSTATUS
NtfspInitializeFileEntry (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PWSTR ParentFileName,
    __in PWSTR FileName,
    __in ULONGLONG FrsNumber,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
NtfspGetFileName (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __deref_inout_ecount(*FileNameSize) PWCHAR *FileName,
    __inout PULONG FileNameSize
    );

//
// Local routine for reading and searching for attributes.
//

NTSTATUS
NtfspLookupAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    );

NTSTATUS
NtfspLookupAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    );

NTSTATUS
NtfspLookupAttributeVcn (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __in VCN Vcn,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    );

NTSTATUS
NtfspLookupAttributeVcnEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PVOID FileRecord,
    __in ULONGLONG FileRecordNumber,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __in VCN Vcn,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    );

NTSTATUS
NtfspReadWriteAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    );

NTSTATUS
NtfspReadWriteAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    );

NTSTATUS
NtfspReadWriteResidentAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout PVOID Buffer,
    __in BOOLEAN ReadOperation
    );

NTSTATUS
NtfspReadWriteNonResidentAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    );

NTSTATUS
NtfspReadWriteCompressedAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    );

NTSTATUS
NtfspDetermineLbo (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Vbo,
    __out PLBO Lbo,
    __out PBOOLEAN SparseRun,
    __out PVBO RunLength
    );

//
// Local routines for directory parsing.
//

NTSTATUS
NtfspSearchForFileIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    );

NTSTATUS
NtfspSearchForFileIndexEntryEx (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __in ULONGLONG StartingOffset,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    );

NTSTATUS
NtfspGetNextFileIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    );

NTSTATUS
NtfspGetNextIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    );

NTSTATUS
NtfspIsIndexBufferAllocated (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT BitmapAttribute,
    __in ULONG BitOffset,
    __out PBOOLEAN IsAllocated
    );

NTSTATUS
NtfspResetDirectoryFilePointer (
    __in PFILE_ENTRY DirectoryEntry,
    __in BOOLEAN ResetGetNextFileOffset
    );

NTSTATUS
NtfspReadDirectoryIndexAllocationBuffer (
    __inout PFILE_ENTRY DirectoryEntry,
    __in VBO IndexBuffer
    );

//
// Other Ntfs utility functions.
//

NTSTATUS
NtfspDecodeUsa (
    __in PMULTI_SECTOR_HEADER MultiSectorHeader,
    __inout_bcount(Length) PVOID Buffer,
    __in ULONG Length,
    __in VBO Offset
    );

//
// Local routines for maintaining device cache.
//

NTSTATUS
NtfspDeviceTablePurgeEntry (
    __in PNTFS_DEVICE_CONTEXT NtfsContext,
    __in ULONG Index
    );

NTSTATUS
NtfspDeviceTableDestroyEntry (
    __in PNTFS_DEVICE_CONTEXT NtfsContext,
    __in ULONG Index
    );

//
// Local routine for interacting with File Record Segments in addition to
// maintaining FRS cache.
//

NTSTATUS
NtfspReadAndDecodeFileRecord (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __inout_bcount(BufferSize) PVOID Buffer,
    __in ULONG BufferSize
    );

NTSTATUS
NtfspGetFileRecordSegment (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecordSegmentNumber,
    __deref_out PVOID *FileRecordSegment
    );

BOOLEAN
NtfspFileRecordCompareKey (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    );

ULONG
NtfspFileRecordHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    );

//
// Local routines for handling WIM compression.
//

VOID
NtfspFreeCompressionContext (
    VOID
    );

PNTFS_COMPRESSION_CONTEXT
NtfspGetCompressionContext (
    VOID
    );

NTSTATUS
NtfspInitializeCompressionContext (
    __in ULONG Algorithm
    );

NTSTATUS
NtfspReadAtOffset (
    __in PFILE_ENTRY FileEntry,
    __in ULONG64 FileOffset,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
NtfspWimRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Length,
    __out_opt PULONG Count
    );

NTSTATUS
NtfspWimWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

// --------------------------------------------------------------------- Macros

#define BytesPerCluster(_bpb) ((_bpb)->BytesPerSector * (_bpb)->SectorsPerCluster)

#define ClustersFromBytesTruncate(_ndc, _bytes) ( \
    (_bytes) >> (_ndc)->ClusterShift)

#define ClustersFromBytes(_ndc, _bytes) ( \
    ((_bytes) + (_ndc)->ClusterMask) >> (_ndc)->ClusterShift)

#define FileReferenceToLargeInt(_fr, _li) {         \
    *(_li) = *(PLONGLONG)&(_fr);                    \
    ((PFILE_REFERENCE)(_li))->SequenceNumber = 0;   \
}

#if NTFS_DBG
#define NtfsPrint BlPrintString
#else
#define NtfsPrint
#endif

// ------------------------------------------------------------ Function Macros

#define NtfspReadAttribute(_vc, _ac, _offset, _len, _buf) \
        NtfspReadWriteAttribute(_vc, _ac, _offset, _len, _buf, TRUE)

#define NtfspWriteAttribute(_vc, _ac, _offset, _len, _buf) \
        NtfspReadWriteAttribute(_vc, _ac, _offset, _len, _buf, FALSE)
