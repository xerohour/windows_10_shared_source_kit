/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    wimboot.h

Abstract:

    This module defines globally used procedure and data structures used by WIM boot extension.

--*/

#pragma once

// ------------------------------------------------------------------- Includes

//#include <wchar.h>
#include <minwindef.h>
#include <sha.h>
#include <file.h>
#include <lzx.h>
#include <wimfile.h>
#include <wimmd.h>
#include <imgver.h>
#include <wimintegrity.h>

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented by this module.  They make up the filesystem function
// table for a Wim along with the mount routine.
//

NTSTATUS
WimInitialize (
    VOID
    );

NTSTATUS
WimDestroy (
    VOID
    );

NTSTATUS
WimMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
WimOpen (
    __in PFILE_ENTRY ParentDirectoryEntry,
    __in PWSTR FileName,
     __in OPEN_FLAGS OpenFlags,
   __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
WimClose (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
WimCreate (
    __in DEVICE_ID DeviceId,
    __in PWCHAR OpenPath,
    __inout PFILE_ENTRY FileEntry
    );

NTSTATUS
WimRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
WimWrite (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    );

NTSTATUS
WimGetNextFile (
    __in PFILE_ENTRY DirectoryEntry,
    __inout_ecount(*BufferSize) PWSTR FileName,
    __inout PULONG BufferSize
    );

NTSTATUS
WimDelete (
    __in DEVICE_ID DeviceId,
    __in PWCHAR FilePath
    );

NTSTATUS
WimTruncate (
    __in PFILE_ENTRY FileEntry,
    __in LARGE_INTEGER ByteSize
    );

NTSTATUS
WimGetInformation (
    __in PFILE_ENTRY FileEntry,
    __out PFILE_INFORMATION FileInformation
    );

NTSTATUS
WimSetInformation (
    __in PFILE_ENTRY FileEntry,
    __in PFILE_INFORMATION FileInformation
    );

// ------------------------------------------------------------------ Constants

#define WIM_SOURCE_FILENAME L"\\boot.wim"
#define WIM_ALTERNATE_SOURCE_FILENAME L"\\sources\\install.wim"
#define WIM_CONFIG_FILENAME L"\\wim.ini"
#define WIM_ALTERNATE_CONFIG_FILENAME L"\\boot\\wim.ini"

#define WIM_MINIMUM_DECOMPRESS_WORKSPACE        4096

#define WIM_PARTIAL_CHUNK_TABLE_SIZE            4

// --------------------------------------------------------------------- Macros

//
//  Handy macros for doing pointer arithmetic
//

//  #define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))
//  #define PtrOffset(B,O) ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))


//
//  Miscellaneous support macros.
//
//      ULONG_PTR
//      WordAlign (
//          __in ULONG_PTR Pointer
//          );
//
//      ULONG_PTR
//      LongAlign (
//          __in ULONG_PTR Pointer
//          );
//
//      ULONG_PTR
//      QuadAlign (
//          __in ULONG_PTR Pointer
//          );
//
//      UCHAR
//      CopyUchar1 (
//          __in PUCHAR Destination,
//          __in PUCHAR Source
//          );
//
//      UCHAR
//      CopyUchar2 (
//          __in PUSHORT Destination,
//          __in PUCHAR Source
//          );
//
//      UCHAR
//      CopyUchar4 (
//          __in PULONG Destination,
//          __in PUCHAR Source
//          );
//
//      PVOID
//      Add2Ptr (
//          __in PVOID Pointer,
//          __in ULONG Increment
//          );
//
//      ULONG
//      PtrOffset (
//          __in PVOID BasePtr,
//          __in PVOID OffsetPtr
//          );
//

#define WordAlignPtr(P) (             \
    (PVOID)((((ULONG_PTR)(P)) + 1) & (-2)) \
)

#define LongAlignPtr(P) (             \
    (PVOID)((((ULONG_PTR)(P)) + 3) & (-4)) \
)

#define QuadAlignPtr(P) (             \
    (PVOID)((((ULONG_PTR)(P)) + 7) & (-8)) \
)

#define WordAlign(P) (             \
    ((((P)) + 1) & (-2)) \
)

#define LongAlign(P) (             \
    ((((P)) + 3) & (-4)) \
)

#define QuadAlign(P) (             \
    ((((P)) + 7) & (-8)) \
)

#define IsWordAligned(P)    ((ULONG_PTR)(P) == WordAlign( (ULONG_PTR)(P) ))

#define IsLongAligned(P)    ((ULONG_PTR)(P) == LongAlign( (ULONG_PTR)(P) ))

#define IsQuadAligned(P)    ((ULONG_PTR)(P) == QuadAlign( (ULONG_PTR)(P) ))

#ifndef FlagOn
#define FlagOn(F,SF) ( \
    (((F) & (SF)))     \
    )

#ifndef BooleanFlagOn
#define BooleanFlagOn(F,SF) (    \
    (BOOLEAN)(((F) & (SF)) != 0) \
    )
#endif

#ifndef SetFlag
#define SetFlag(_F,_SF)       ((_F) |= (_SF))
#endif

#ifndef ClearFlag
#define ClearFlag(_F,_SF)     ((_F) &= ~(_SF))
#endif

#endif

#define WIM_ENTRY_FIXED 0x80000000

//
//  Debug Macros
//
#ifdef DBG

#ifndef DBG_HEADER
#define DBG_HEADER  "BOOTWIM"
#endif

#define DBGPRINT(_component, _level, _fmt)

//  #define DBGPRINT( _component, _level, _fmt) {                                      \
//      if ( ((DebugComponents & (_component)) != 0) && ((_level) <= DebugLevel) ) {    \
//          BlPrintString( "%s : %s :", DBG_HEADER, __FUNCTION__);  \
//          BlPrintString _fmt ;                                \
//      }                                                          \
//  }

#define DBG_NONE        0x00000000
#define DBG_INIT        0x00000001
#define DBG_READWRITE   0x00000002
#define DBG_CALLBACKS 0x00000004
#define DBG_DIRECTORY 0x00000008
#define DBG_MOUNTUNMOUNT 0x00000010

#define DBG_ERROR       0x00000001
#define DBG_NOTIFY      0x00000002
#define DBG_WARN        0x00000003
#define DBG_INFO        0x00000004
#define DBG_VERBOSE     0x00000005
#define DBG_PAINFUL     0x00000006
#define DBG_ALL         0x7FFFFFFF

#define DEFAULT_BREAK_ON_ENTRY FALSE
#define DEFAULT_DEBUG_LEVEL DBG_VERBOSE
#define DEFAULT_DEBUG_COMPONENTS DBG_ALL

extern ULONG DebugComponents;
extern ULONG DebugLevel;

#else

#define DBGPRINT(_component, _level, _fmt)

#endif

#define IsNullHash(Hash, HashSize)  \
            ((Hash != NULL) && \
             ((HashSize) == HASH_SIZE) && \
             (RtlEqualMemory(Hash, NullHash, HASH_SIZE) != 0))


// ------------------------------------------------- Data Structure Definitions

#define WimTypeCode 'FMIW'

//
// WIM file context structure maintains the open file context
//

typedef struct _WIM_FILE_CONTEXT {

    //
    //  DIRENTRY of the opened file.
    //

    LPDIRENTRY FileEntry;

    //
    //  Resource of the file entry we're interested in, incase of multiple
    //  streams, this points to the correct stream.
    //

    LPRESHDR_DISK Resource;

    //
    // File entry for the most recent file returned by WimGetNextFile
    //

    LPDIRENTRY CachedGetNextFileEntry;

    //
    //  Allocated chunk table for the file
    //

    PULONG ChunkTable;

} WIM_FILE_CONTEXT, *PWIM_FILE_CONTEXT;

typedef struct _WIM_FILE {

    WIM_FILE_CONTEXT FileContext;

    FILE_INFORMATION Information;

    //
    // There is a global device table use to keep track of the Wim Device
    // Context for each Wim filtered Volume.  Each file contains an index into the
    // table for their specific volume.
    //

    ULONG            WimDeviceTableId;   // Index into the WimDeviceTable.

} WIM_FILE, *PWIM_FILE;

//
// WIM integrity context.
//

typedef struct _WIM_INTEGRITY_CONTEXT {

    //
    // Handle to the WIM integrity file.
    //

    FILE_ID IntegrityFileId;

    //
    // Offset to the hashes in the integrity file.
    //

    ULONG OffsetToHashes;

    //
    // Size of the WIM.
    //

    ULONGLONG WimFileSize;

    //
    // Bitmap of verified blocks.
    //

    RTL_BITMAP VerifiedBlockBitmap;

    //
    // Buffer to cache hashes from the integrity file.
    //

    PVOID HashCache;

    //
    // First and last indices of block hashes that are present in the cache.
    //

    ULONG HashCacheBlockStart;
    ULONG HashCacheBlockEnd;

} WIM_INTEGRITY_CONTEXT, *PWIM_INTEGRITY_CONTEXT;

//
//  WIM volume structure context maintains the WIM data
//

typedef struct _WIM_STRUCTURE_CONTEXT {

    //
    // Device Id of the wim file device
    //

    DEVICE_ID DeviceId;

    //
    // Track if the files will only be opened by resource hash, in which case
    // boot metadata is not required.
    //

    BOOLEAN OpenByHash;

    //
    //  Wim Block Size, from the header.
    //

    ULONG CompressedChunkSize;      // *

    //
    //  Copy of flags in the WIM header. Overloaded for own use.
    //

    ULONG WimFlags;         // *

    //
    //  Size of the compressed buffer that we can use for compressed reads
    //

    ULONG CompressedBufferSize;

    //
    //  Pointer to decompression workspace
    //

    __field_ecount(CompressedBufferSize+CompressedChunkSize+DecompressWorkSpaceSize) PVOID Workspace;

    //
    //  Convenience pointer into the above buffer where the compression engine
    //  can operate.
    //

    PVOID DecompressorWorkspace;

    //
    //  Space for a single uncompressed chunk in case we need to decompress the
    //  chunk somewhere and copy a portion back for the caller.
    //

    PVOID UncompressedChunkBuffer;

    //
    //  Decompression workspace size
    //

    ULONG DecompressWorkSpaceSize;

    //
    //  Decompression Context
    //

    PVOID DecompressContext;

    //
    //  Pointer to resource directory
    //

    LPRESHDR_DISK OffsetTable;  // *

    //
    //  Pointer used to determine that we've scanned the OffsetTable
    //

    PVOID LastResourceBYTE;     // *

    //
    //  Pointer to the directory block
    //

    PUCHAR BootMetaData;        // *

    //
    //  Size in bytes of the directory block
    //

    ULONG BootMetaDataLength;

    //
    //  Pointer to start of Root directory
    //

    LPDIRENTRY Root;

    //
    //  Current Byte Offset
    //

    LARGE_INTEGER ByteOffset;

    //
    //  Length of the WIM
    //

    LARGE_INTEGER WimLength;

    //
    //  Base Address of the WIM
    //

    LPWIMHEADER_PACKED WimBaseAddress;

    //
    // Pointer to an integrity context
    //

    PWIM_INTEGRITY_CONTEXT Integrity;

} WIM_STRUCTURE_CONTEXT, *PWIM_STRUCTURE_CONTEXT;

//
// Define a list of directories to be processed during fixup stage.
//

typedef struct _WIM_DIRECTORY_LIST_ENTRY {
    LIST_ENTRY NextEntry;
    LPDIRENTRY Directory;
} WIM_DIRECTORY_LIST_ENTRY, *PWIM_DIRECTORY_LIST_ENTRY;

// -------------------------------------------------------------------- Globals

// ----------------------------------------------- Internal Function Prototypes

NTSTATUS
WimpDeviceTableDestroyEntry (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in ULONG Index
    );

NTSTATUS
WimpAttachWim (
    __in DEVICE_ID DeviceId,
    __out PWIM_STRUCTURE_CONTEXT * WimContext
    );

NTSTATUS
WimpDestroyContext (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    );

NTSTATUS
WimpDestroyDecompressionWorkspace (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    );

NTSTATUS
WimpValidateWim (
    __in LPWIMHEADER_PACKED WimHeader,
    __in BOOLEAN OpenByHash
    );

PWIM_STRUCTURE_CONTEXT
WimpAllocateContext (
    __in LPWIMHEADER_PACKED WimHeader,
    __in BOOLEAN OpenByHash
    );

NTSTATUS
WimpSetupContext (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPWIMHEADER_PACKED WimHeader
    );

NTSTATUS
WimpInitializeDecompressionWorkspace (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    );

NTSTATUS
WimpReadResource (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPRESHDR_DISK_SHORT Resource,
    __in PVOID Buffer
    );

NTSTATUS
WimpBuildChunkTable (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPRESHDR_DISK_SHORT Resource,
    __in ULONG Chunks,
    __deref_out_ecount(Chunks + 2) PULONG * ChunkTable
    );

LPDIRENTRY
WimGetDirEntryNext (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPDIRENTRY Dirent
    );

LPDIRENTRY
WimGetDirEntrySubDir (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPDIRENTRY Dirent
    );

NTSTATUS
WimpFixupRoot (
    __inout PWIM_STRUCTURE_CONTEXT WimContext
    );

NTSTATUS
WimpFixupStreams (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in DWORD Count,
    __in LPSTREAMENTRY Stream,
    __out LPDIRENTRY *NextDirEntry
    );

LPRESHDR_DISK
WimpFindStreamResource (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in_bcount(HASH_SIZE) PUCHAR Hash
    );

NTSTATUS
WimpSearchForDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __out LPDIRENTRY *Dirent
    );

BOOLEAN
WimpCompareFileNames (
    __in_ecount(Length) PWCHAR Name1,
    __in_ecount(Length) PWCHAR Name2,
    __in USHORT Length
    );

NTSTATUS
WimpRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Length) VOID *Buffer,
    __in ULONG Length,
    __out ULONG *Count
    );

NTSTATUS
WimpDecompress (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __out_bcount(TargetBufferLength) PVOID TargetBuffer,
    __in ULONG TargetBufferLength,
    __in_bcount(SourceBufferLength) PVOID SourceBuffer,
    __in ULONG SourceBufferLength,
    __out PULONG TargetSize
    );

NTSTATUS
WimpAddDirectoryListEntry (
    __in PLIST_ENTRY ListHead,
    __in LPDIRENTRY Directory
    );

VOID
WimpCleanupDirectoryList (
    __in PLIST_ENTRY ListHead
    );

NTSTATUS
WimpIntegrityCreateContext (
    _In_ DEVICE_ID WimDeviceId,
    _Out_ PWIM_INTEGRITY_CONTEXT *Integrity
    );

VOID
WimpIntegrityDestroyContext (
    _Inout_ PWIM_INTEGRITY_CONTEXT Integrity
    );

_Success_(return != FALSE)
BOOLEAN
WimpIntegrityCalculateBlockIndices (
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_ ULONG ReadSize,
    _Out_ PULONG BlockStart,
    _Out_ PULONG BlockCount,
    _Out_opt_ PULONG BlockEnd
    );

VOID
WimpIntegrityReportFailure (
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONG FailingBlock
    );

BOOLEAN
WimpIntegrityAreBlocksValidated (
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_ ULONG ReadSize
    );

VOID
WimpIntegrityGetAlignedSizes (
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG ReadOffset,
    _In_ ULONG ReadSize,
    _In_ ULONG Alignment,
    _Out_ PULONGLONG AlignedOffset,
    _Out_ PULONG AlignedReadSize,
    _Out_ PULONG Padding
    );

NTSTATUS
WimpIntegrityValidateBlocks (
    _Inout_opt_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize
    );

NTSTATUS
WimpIntegrityLoadHashesInCache (
    _Inout_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONG BlockStart
    );

NTSTATUS
WimpIntegrityDeviceReadAtOffset (
    _Inout_opt_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ DEVICE_ID DeviceId,
    _In_ ULONGLONG ByteOffset,
    _In_ ULONG Size,
    _Out_writes_bytes_(Size) PVOID Buffer,
    _Out_opt_ PULONG BytesRead
    );

NTSTATUS
WimpIntegrityBuildContext (
    _In_ ULONGLONG WimFileSize,
    _In_ FILE_ID IntegrityFileId,
    _In_ ULONG OffsetToHashes,
    _Outptr_ PWIM_INTEGRITY_CONTEXT *Integrity
    );

NTSTATUS
WimpIntegrityValidateIntegrityFile (
    _In_ FILE_ID IntegrityFileId,
    _In_ DEVICE_ID WimDeviceId,
    _In_ ULONGLONG WimFileSize,
    _Out_ PBOOLEAN IntegrityValid,
    _Out_ PBOOLEAN IntegrityReady,
    _Out_ PULONG OffsetToHashes
    );

NTSTATUS
WimpIntegrityOpenIntegrityFile (
    _In_ DEVICE_ID WimDeviceId,
    _Out_ PFILE_ID IntegrityFileId,
    _Out_ PDEVICE_ID OverlayedDeviceId
    );

