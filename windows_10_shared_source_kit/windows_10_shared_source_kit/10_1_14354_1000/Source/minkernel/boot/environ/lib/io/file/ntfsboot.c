/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ntfsboot.c

Abstract:

    Implementation of the boot library's native NTFS file system support.
    The routines in this module make up the function table for a for
    a NTFS file entry.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "ntfsboot.h"

// ---------------------------------------------------------------- Definitions

#define WOF_DATA_STREAM L"WofCompressedData"

// -------------------------------------------------------------------- Globals

//
// Function table for a NTFS partition.
//

const FILESYSTEM_FUNCTION_TABLE NtfsFunctionTable = {
    (PFILESYSTEM_OPEN_ROUTINE)             NtfsOpen,
    (PFILESYSTEM_CLOSE_ROUTINE)            NtfsClose,
    (PFILESYSTEM_READ_ROUTINE)             NtfsRead,
    (PFILESYSTEM_WRITE_ROUTINE)            NtfsWrite,
    (PFILESYSTEM_GET_NEXT_FILE_ROUTINE)    NtfsGetNextFile,
    (PFILESYSTEM_GET_INFORMATION_ROUTINE)  NtfsGetInformation,
    (PFILESYSTEM_SET_INFORMATION_ROUTINE)  NtfsSetInformation,
    (PFILESYSTEM_GET_REPARSE_DATA_ROUTINE) NtfsGetReparseData
};

//
// Routines required to register this Ntfs filesystem implementation.
//

const FILESYSTEM_REGISTER_FUNCTION_TABLE NtfsRegisterFunctionTable = {
    (PFILESYSTEM_INITIALIZE_ROUTINE)      NtfsInitialize,
    (PFILESYSTEM_DESTROY_ROUTINE)         NtfsDestroy,
    (PFILESYSTEM_MOUNT_ROUTINE)           NtfsMount,
    (PFILESYSTEM_PURGE_CACHE_ROUTINE)     NtfsPurgeCache
};

const UNICODE_STRING NtfsEmptyName = CONSTANT_UNICODE_STRING(L"");

#ifndef FILE_MIN

const UNICODE_STRING NtfsWofDataStream = CONSTANT_UNICODE_STRING(WOF_DATA_STREAM);

NTFS_COMPRESSION_CONTEXT NtfspCompressionContext;

#endif

// ------------------------------------------------------------------ Functions

NTSTATUS
NtfsInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the NTFS Device Table used to cache NTFS volume information for
    each mounted device.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if the device table could not be allocated.

--*/

{

    //
    // Initialize the Ntfs Device Table.  This table keeps track of Ntfs Device
    // Context for each NTFS Volume.
    //

    NtfsDeviceTableEntries = INITIAL_NTFS_DEVICE_ENTRIES;
    NtfsDeviceTable = BlMmAllocateHeap(NtfsDeviceTableEntries *
                                       sizeof(PNTFS_DEVICE_CONTEXT));

    if (NtfsDeviceTable == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(NtfsDeviceTable,
                  NtfsDeviceTableEntries * sizeof(PNTFS_DEVICE_CONTEXT));

    return STATUS_SUCCESS;
}

NTSTATUS
NtfsDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys the NTFS Device Table used to cache NTFS volume information for
    each NTFS mounted block i/o devicek.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    Other Error if error occured destroying any device entry.

--*/

{

    NTSTATUS Status;

    //
    // Destroy the Ntfs Device Table by mapping the deallocation routine for
    // each table entry.
    //

    Status = BlTblMap(NtfsDeviceTable,
                      NtfsDeviceTableEntries,
                      NtfspDeviceTableDestroyEntry,
                      FALSE);

    //
    // Free the memory allocated for the table.
    //

    BlMmFreeHeap(NtfsDeviceTable);

    //
    // Free compression context if previously allocated.
    //

#ifndef FILE_MIN

    NtfspFreeCompressionContext();

#endif

    return Status;
}

NTSTATUS
NtfsMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Attempts to mount a NTFS filesystem on a block i/o device.  Provided a
    block i/o device (DeviceId), this routine will attempt to detect a NTFS
    volume by reading the Bios Parameter Block for the device.

    If a NTFS volume is detected, this routine will allocate and initialize a
    file entry for the root directory of the NTFS volume.

Arguments:

    DeviceId - Device Id for a Block I/O Device.

    FilterLevel - (unused) - Indicates the filter location of the mount request
        (zero indicates this is the first mount request on the device).

    FileEntry - Pointer to address to store an allocated FILE_ENTRY structure.
        The data structure is allocated if this routine detects the Block I/O
        Device is a NTFS volume.  In that case, the allocated file entry is
        initialized for the root directory of the volume.

Return Value:

    STATUS_SUCCESS when successful (and the file system has been mounted on
        the device).
    STATUS_UNSUCCESSFUL if a NTFS filesystem is not present on the device.
    STATUS_NO_MEMORY if there was an error allocating memory for any internal
        data structures.

--*/

{

    BIOS_PARAMETER_BLOCK Bpb;
    PACKED_BOOT_SECTOR BootSector;
    DEVICE_INFORMATION DeviceInformation;
    ULONG OriginalIoAccessFlags;
    NTSTATUS Status;
    PNTFS_DEVICE_CONTEXT VolumeContext;

    UNREFERENCED_PARAMETER( FilterLevel);

    VolumeContext = NULL;

    //
    // In attempt to minimize the number of physical reads performed while
    // mounting a filesystem, cache the read of the boot sector.
    //

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION_EX));

    DeviceInformation.u.BlockIo.LbaOffset = 0;
    OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

    Status = BlDeviceSetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);

    //
    // A NTFS volume starts with a boot sector at LBN 0.  There is a duplicate
    // boot sector at LBN=(number of sectors on partition -1) and legacy
    // volumes had a backup boot sector at LBN=(number of sectors on partition
    // divided by 2).  The boot sector provides a standard Bios Parameter
    // Block which includes both partition and filesystem information.  The
    // Bios Parameter Block (BPB) can be used to determine if the filesystem
    // on the volume is NTFS.  This code only reads the primary boot sector,
    // but can be extended to the backup boot sector as well.
    //

    //
    // Attempt to read the bios parameter block for the device.
    //

    Status = NtfspDeviceReadAtOffset(NULL,
                                     DeviceId,
                                     0,
                                     sizeof(BootSector),
                                     &BootSector,
                                     NULL);

    //
    // Restore the original I/O access flags before checking the return
    // status.  BlDeviceGetInformation should be called again to preserve the
    // seek pointer after the attribute read.  However, this module only uses
    // BlDeviceReadAtOffset and no caller should assume the value of the seek
    // position on return.
    //

    DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
    BlDeviceSetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Unpack the bios parameter block from the contents read.
    //

    NtfsUnpackBios(&Bpb, &BootSector.PackedBpb);

    //
    // Check to see if the contents read as the BPB are actually a BPB and if
    // so, if the contents specify a NTFS volume.
    //

    //
    // A NTFS BPB will contain the ASCII characters "NTFS" in the OEM field.
    //

    if (RtlEqualMemory(&BootSector.Oem[0], "NTFS    ", 8) == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Calculate the checksum for the Bios Parameter Block.  The checksum is
    // defined to be the additive sum of the ULONGs up to the Checksum field.
    //
    // The validation of the checksum is commented out in NtfsIsBootSectorNtfs
    // even though it has been documented in the documentation I have read.
    // For right now, do not actually check the checksum.  If the detection
    // code changes in NtfsIsBootSectorNtfs, this routine needs to be changed
    // as well.
    //

    //
    //  for (l = (PULONG)BootSector; l < (PULONG)(&BootSector.Checksum); l++) {
    //      Checksum += *l
    //  }
    //
    //  if (Bootsector.Checksum != Checksum) {
    //      return STATUS_UNSUCCESSFUL;
    //  }
    //

    //
    // Ensure the BPB does not describe a FAT volume.  All fields that pertain
    // to a FAT volume must be zero.
    //

    if ((Bpb.ReservedSectors != 0) ||
        (Bpb.Fats            != 0) ||
        (Bpb.RootEntries     != 0) ||
        (Bpb.Sectors         != 0) ||
        (Bpb.SectorsPerFat   != 0) ||
        (Bpb.LargeSectors    != 0)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The valid bytes per sector values include [512, 1024, 2048, 4096].
    //

    if ((Bpb.BytesPerSector != 512) &&
        (Bpb.BytesPerSector != 1024) &&
        (Bpb.BytesPerSector != 2048) &&
        (Bpb.BytesPerSector != 4096)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The number of sectors per allocation unit must be a power of two with
    // the max value of 0x80.
    //

    if (((Bpb.SectorsPerCluster & (Bpb.SectorsPerCluster - 1)) != 0) ||
        (0x80 < Bpb.SectorsPerCluster) ) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The legal values for the media field are [0xf0, 0xf8, 0xf9, 0xfa, 0xfb,
    // 0xfc, 0xfd, 0xfe, 0xff].  The standard for fixed media is 0xf8.  For
    // removable media it is 0xf0.
    //

    if ((Bpb.Media != 0xF0) && (Bpb.Media != 0xF8) && (Bpb.Media != 0xF9) &&
        (Bpb.Media != 0xFA) && (Bpb.Media != 0xFB) && (Bpb.Media != 0xFC) &&
        (Bpb.Media != 0xFD) && (Bpb.Media != 0xFE) && (Bpb.Media != 0xFF)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The ClustersPerFileRecordSegment indicates the size of a file record
    // segment.  This value must be a power of two.
    //
    // File Record Segment Size = ClustersPerRecordSegment * BytesPerCluster.
    //
    // ClustersPerFileRecordSegment can be a negative number.  If so, it
    // indicates the negative bit shift count of the File Record Segment Size.
    // Since a sector is at least 512 bytes, the valid shift count is [9, 31].
    //
    // File Record Segment Size = 1 << (-1 * ClustersPerRecordSegment).
    //

    if (BootSector.ClustersPerFileRecordSegment < 0) {
        if ((BootSector.ClustersPerFileRecordSegment > -9) ||
            (BootSector.ClustersPerFileRecordSegment < -31)) {

            return STATUS_UNSUCCESSFUL;
        }

    } else if ((BootSector.ClustersPerFileRecordSegment &
                (BootSector.ClustersPerFileRecordSegment - 1)) != 0) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The same is true for the DefaultClustersPerIndexAllocationBuffer.  The
    // valid values are [-9,-31] and a positive number that is a power of two.
    //

    if (BootSector.DefaultClustersPerIndexAllocationBuffer < 0) {
        if ((BootSector.DefaultClustersPerIndexAllocationBuffer > -9) ||
            (BootSector.DefaultClustersPerIndexAllocationBuffer < -31)) {

            return STATUS_UNSUCCESSFUL;
        }

    } else if ((BootSector.DefaultClustersPerIndexAllocationBuffer &
                (BootSector.DefaultClustersPerIndexAllocationBuffer - 1)) != 0) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // Ensure that some value was provided for the location of the MFT, backup
    // MFT and the size of the partition.
    //

    if ((BootSector.NumberSectors == 0) ||
        (BootSector.MftStartLcn == 0) ||
        (BootSector.Mft2StartLcn == 0)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The Bios Parameter Block has been verified and the device described by
    // DeviceId appears to be a NTFS volume.
    //

    //
    // Initialize context structure for the NTFS Volume.
    //

    Status = NtfspInitializeDeviceContext(DeviceId,
                                          &BootSector,
                                          &VolumeContext);

    if (!NT_SUCCESS(Status)) {
        goto NtfsMountEnd;
    }

    //
    // Initialize a File Entry structure for the root directory.
    //

    Status = NtfspInitializeFileEntry(VolumeContext,
                                      L"",
                                      L"",
                                      ROOT_FILE_NAME_INDEX_NUMBER,
                                      FileEntry);

    if (!NT_SUCCESS(Status)) {
        goto NtfsMountEnd;
    }

NtfsMountEnd:

    //
    // Cleanup from any errors that occurred during the initialization of the
    // the root directory.
    //

    if (!NT_SUCCESS(Status)) {
        if (VolumeContext != NULL) {
            NtfspDeviceTableDestroyEntry(VolumeContext,
                                         VolumeContext->ContextIndex);
        }
    }

    return Status;
}

NTSTATUS
NtfsOpen (
    __in PFILE_ENTRY ParentDirectoryEntry,
    __in PWSTR FileName,
    __in OPEN_FLAGS OpenFlags,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Opens the specified file in the directory described by
    ParentDirectoryEntry.  When successful, this routine will allocated a
    File Entry structure for the newly opened file.

Arguments:

    ParentDirectoryEntry - The File Entry for the parent directory of the file
        to be opened.

    FileName - The name of the file to be opened.  This is just the filename
        and is NOT a full path name.

    OpenFlags - Open flags, indicating if the open is for read, write, and/or
        for a directory.

    FileEntry - Pointer to address to store an allocated FILE_ENTRY structure
        if the file could be opened.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if the file does not exist in the parent directory.
    STATUS_NO_MEMORY if there was an arror allocating memory for any internal
        data structures.

--*/

{

    ULONGLONG BaseFrsNumber;
    PNTFS_FILE DirContext;
    PFILE_RECORD_SEGMENT_HEADER FrsHeader;
    ULONGLONG FrsNumber;
    PINDEX_ENTRY IndexEntry;
    ULONG IndexEntrySize;
    ULONGLONG StartingOffset;
    NTSTATUS Status;
    PNTFS_DEVICE_CONTEXT VolumeContext;

    UNREFERENCED_PARAMETER(OpenFlags);

    DirContext = (PNTFS_FILE)ParentDirectoryEntry->FileData;
    VolumeContext = NtfsDeviceTable[DirContext->NtfsDeviceTableId];
    IndexEntry = NULL;
    IndexEntrySize = 0;

    //
    // Find the index entry for the specified file.  When found, create a
    // file entry for the opened file.
    //
    // If the cached get next file offset is valid, attempt to first find the
    // file starting at the cached offset.  If this fails, perform the search
    // starting at the beginning of the directory.
    //

    Status = STATUS_NOT_FOUND;
    if ((DirContext->FileContext.GetNextFileOffset != 0) &&
        (DirContext->FileContext.UseGetNextFileOffsetOnOpen != 0)) {

        StartingOffset = DirContext->FileContext.GetNextFileOffset;
        Status = NtfspSearchForFileIndexEntryEx(ParentDirectoryEntry,
                                                FileName,
                                                StartingOffset,
                                                &IndexEntry,
                                                &IndexEntrySize);

        if (!NT_SUCCESS(Status)) {
            DirContext->FileContext.UseGetNextFileOffsetOnOpen = 0;
        }
    }

    if (!NT_SUCCESS(Status)) {
        Status = NtfspSearchForFileIndexEntry(ParentDirectoryEntry,
                                              FileName,
                                              &IndexEntry,
                                              &IndexEntrySize);
    }

    if (!NT_SUCCESS(Status)) {
        goto NtfsOpenEnd;;
    }

    //
    // Determine the File Record Segment that is referenced by the Index
    // Entry for the file.  This is contained in the Index Entry header.
    //

    FileReferenceToLargeInt(IndexEntry->FileReference, &BaseFrsNumber);

    //
    // Determine the Base File Record Segment of the file.  This could be the
    // value just read, but to ensure we must read the on-disk header for the
    // file record segment.
    //

    for (FrsNumber = BaseFrsNumber;
         (Status == STATUS_SUCCESS) && (BaseFrsNumber != 0);
         /* Nothing */ ) {

        //
        // Update the File Record Number.  On completion of this loop,
        // FrsNumber number should contain the Base File Record Segment number
        // for the file.
        //

        FrsNumber = BaseFrsNumber;

        //
        // Get the contents of the File Record Segment to determine if this is
        // the base file record segment for the file.
        //

        Status = NtfspGetFileRecordSegment(VolumeContext,
                                           FrsNumber,
                                           &FrsHeader);

        if (!NT_SUCCESS(Status)) {
            continue;                      // Break out of loop.
        }

        //
        // Determine if this File Record Segment refers to another File Record
        // Segment.  If not, this value is zero.
        //

        FileReferenceToLargeInt(FrsHeader->BaseFileRecordSegment,
                                &BaseFrsNumber);
    }

    //
    // Allocate an initiate a File Entry structure for the file being opened.
    //

    Status = NtfspInitializeFileEntry(VolumeContext,
                                      ParentDirectoryEntry->FilePath,
                                      FileName,
                                      FrsNumber,
                                      FileEntry);

NtfsOpenEnd:

    if (IndexEntry != NULL) {
        BlMmFreeHeap(IndexEntry);
    }

    return Status;
}

NTSTATUS
NtfsClose (
    __in PFILE_ENTRY FileEntry
    )

/*++

Routine Description:

    Closes a file.  Free all memory allocated for the file.

Arguments:

    FileEntry - File entry for file to be closed.

Return Value:

    STATUS_SUCCESS when successful.
    Error otherwise.

--*/

{

    PNTFS_FILE NtfsFile;

    NtfsFile = (PNTFS_FILE)FileEntry->FileData;
    BlMmFreeHeap(FileEntry->FilePath);

    //
    // Free chunk table for WIM compressed files.
    //

#ifndef FILE_MIN

    if ((IS_FILE_DIRECTORY_SET(FileEntry->Attributes) == FALSE) &&
        (NtfsFile->FileContext.WimCompressed != FALSE)) {

        if (NtfsFile->FileContext.ChunkTable != NULL) {
            BlMmFreeHeap(NtfsFile->FileContext.ChunkTable);
        }
    }

#endif

    BlMmFreeHeap(NtfsFile);
    return STATUS_SUCCESS;
}

NTSTATUS
NtfsRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Reads Size bytes starting at the offset indicated in the file information
    structure : FileEntry->FileData.Infomation.FileOffset.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the read operation.

    Size - The number of bytes to read.

    Count - Address to store the number of bytes actually read.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_FILE_IS_A_DIRECTORY if the file specified is a directory.

    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PNTFS_FILE_CONTEXT FileContext;
    PNTFS_FILE FileData;
    NTSTATUS Status;

    FileData = (PNTFS_FILE)FileEntry->FileData;
    FileContext = &FileData->FileContext;

    //
    // Do not allow any reads from a directory.  If a user wishes to manipulate
    // a directory, they should use GetNextFile, CreateFile, etc.
    //
    // A read from a file is simply reading the $DATA attribute at the
    // specified offset.
    //

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        return STATUS_FILE_IS_A_DIRECTORY;
    }

    Status = NtfspReadAttribute(NtfsDeviceTable[FileData->NtfsDeviceTableId],
                                &FileContext->DataAttribute,
                                FileData->Information.FileOffset,
                                Size,
                                Buffer);

    if (!NT_SUCCESS(Status)) {
        goto NtfsReadEnd;
    }

    //
    // Update file pointer.
    //

    FileData->Information.FileOffset += Size;
    if (ARGUMENT_PRESENT(Count)) {
        *Count = Size;
    }

NtfsReadEnd:
    return Status;
}

NTSTATUS
NtfsWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Writes Size bytes starting at the offset indicated in the file information
    structure : FileEntry->FileData.Infomation.FileOffset.

    N.B. This routine only contains file write back support.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the write operation.

    Size - The number of bytes to write.

    Count - Address to store the number of bytes actually written.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_FILE_IS_A_DIRECTORY if the file specified is a directory.

    STATUS_INVALID_PARAMETER if write request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PNTFS_FILE_CONTEXT FileContext;
    PNTFS_FILE FileData;
    NTSTATUS Status;

    FileData = (PNTFS_FILE)FileEntry->FileData;
    FileContext = &FileData->FileContext;

    //
    // Do not allow any writes to a directory.  This function only supports
    // FILE WRITE BACK.
    //
    // Write back support includes writing the specified buffer to the
    // correct offset of the $DATA attribute.
    //

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        return STATUS_FILE_IS_A_DIRECTORY;

    } else {
        Status = NtfspWriteAttribute(NtfsDeviceTable[FileData->NtfsDeviceTableId],
                                     &FileContext->DataAttribute,
                                     FileData->Information.FileOffset,
                                     Size,
                                     Buffer);

        if (!NT_SUCCESS(Status)) {
            goto NtfsWriteEnd;
        }
    }

    //
    // Update file pointer.
    //

    FileData->Information.FileOffset += Size;
    if (Count != NULL) {
        *Count = Size;
    }

NtfsWriteEnd:
    return Status;
}

NTSTATUS
NtfsGetNextFile (
    __in PFILE_ENTRY DirectoryEntry,
    __inout_ecount(*BufferSize) PWSTR FileName,
    __inout PULONG BufferSize
    )

/*++

Routine Description:

    This routine returns the filename for the next directory entry in the
    directory described by DirectoryEntry.

    Each call requires the file name return by the previous call and on output
    the next file name is returned.

    To initiate the search, FileName must point to a NULL buffer or a buffer
    whose first Unicode Character is the NULL Character.
    (ie. FileName == NULL || FileName[0] == L'\0').

    A search is ended with a return value of STATUS_NOT_FOUND, indicating the
    next filename was not found.

Arguments:

    DirectoryEntry - FILE_ENTRY for the directory being parsed.

    FileName - On input, the last filename returned by this function.  On
        output, returns the NULL-terminated Unicode string for the next file
        name in the directory.

    BufferSize - The size (in number of WCHARs) of the FileName buffer.  If the
        buffer is too small (return value of STATUS_BUFFER_TOO_SMALL), the
        return value of BufferSize is the required size (in WCHARs) for the
        filename buffer.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_BUFFER_TOO_SMALL if the size specified by BufferSize is smaller then
        the length (including the null character) of the next filename.
    STATUS_NOT_FOUND when the next filename can not be found.
    STATUS_INVALID_PARAMETER if FileName is invalid.  All other parameters are
        validated by caller.

--*/

{

    PFILE_NAME FileNameEntry;
    ULONG FileNameLength;
    PINDEX_ENTRY IndexEntry;
    ULONG IndexEntrySize;
    ULONGLONG NextFileOffset;
    PNTFS_FILE NtfsData;
    ULONGLONG StartingOffset;
    NTSTATUS Status;

    IndexEntry = NULL;
    IndexEntrySize = 0;
    NtfsData = DirectoryEntry->FileData;

    //
    // Find the Index Entry for the file specified.  The header for the entry
    // will contain a reference to the file's base file record.
    //
    // If a filename was not specified, start the search at the start of the
    // index entry list.
    //

    if ((FileName == NULL) || (FileName[0] == L'\0')) {
        Status = NtfspResetDirectoryFilePointer(DirectoryEntry, TRUE);
        if (!NT_SUCCESS(Status)) {
            goto NtfsGetNextFileEnd;
        }

    } else {
        StartingOffset = NtfsData->FileContext.GetNextFileOffset;
        Status = NtfspSearchForFileIndexEntryEx(DirectoryEntry,
                                                FileName,
                                                StartingOffset,
                                                &IndexEntry,
                                                &IndexEntrySize);

        if (!NT_SUCCESS(Status)) {
            Status = STATUS_INVALID_PARAMETER;
            goto NtfsGetNextFileEnd;
        }
    }

    //
    // N.B. The current file offset value refers to the offset of the index
    //      entry immediately after the index entry just read.
    //
    // Save the current file offset to optimize the next get next file call
    // and read the next file name in the directory.
    //
    // If this is the last file in the directory, reset the cached offset.
    //

    NextFileOffset = NtfsData->Information.FileOffset;
    Status = NtfspGetNextFileIndexEntry(DirectoryEntry,
                                        &IndexEntry,
                                        &IndexEntrySize);

    if (!NT_SUCCESS(Status)) {
        NtfsData->FileContext.GetNextFileOffset = 0;
        Status = STATUS_NOT_FOUND;
        goto NtfsGetNextFileEnd;
    }

    //
    // An Index Entry will have a FILE_NAME structure following it.  The above
    // routine returns the entire Index Entry, which includes the File Name
    // Entry.  This structure contains the next file name (the name we need to
    // return to the caller).
    //

    FileNameEntry = Add2Ptr(IndexEntry, sizeof(INDEX_ENTRY));
    FileNameLength = (ULONG)FileNameEntry->FileNameLength;
    if (*BufferSize < FileNameLength + 1) {
        *BufferSize = FileNameLength + 1;
        Status = STATUS_BUFFER_TOO_SMALL;
        goto NtfsGetNextFileEnd;
    }

    RtlCopyMemory(FileName,
                  &FileNameEntry->FileName[0],
                  FileNameLength * 2);

    FileName[FileNameLength] = L'\0';

    //
    // Save the file offset of the last file's index entry to optimize the next
    // get next file call.
    //

    NtfsData->FileContext.GetNextFileOffset = NextFileOffset;
    NtfsData->FileContext.UseGetNextFileOffsetOnOpen = 1;

NtfsGetNextFileEnd:

    //
    // We don't care about the Index Entry that was returned.  Free the
    // allocated buffer.
    //

    if (IndexEntry) {
        BlMmFreeHeap(IndexEntry);
    }

    return Status;
}

NTSTATUS
NtfsGetInformation (
    __in PFILE_ENTRY FileEntry,
    __out PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    Copies FileEntry->FileData.Information to the caller's buffer.

Arguments:

    FileEntry - Pointer to the file entry (stored in the FileTable) for the
                file.

    FileInformation - Pointer to buffer to copy file information.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    PNTFS_FILE FileInfo;

    FileInfo = (PNTFS_FILE)FileEntry->FileData;

    //
    // Copy FileEntry->FileData.Information to the caller's buffer.
    //

    RtlCopyMemory(FileInformation,
                  &FileInfo->Information,
                  sizeof(FILE_INFORMATION));

    return STATUS_SUCCESS;
}

NTSTATUS
NtfsSetInformation (
    __in PFILE_ENTRY FileEntry,
    __in PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    Uses the values in FileInformation to set user configurable settings for
    the file.

    The only user configurable setting is FileOffset.  This routine will ignore
    all other settings, returning STATUS_SUCCESS if FileOffset is less than the
    size of the file.

Arguments:

    FileEntry - Pointer to the file entry (stored in the FileTable) for the
                file.

    FileInformation - Pointer to buffer to read file information.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_FILE_IS_A_DIRECTORY if the file is a directory.

    STATUS_INVALID_PARAMETER if the requested file offset is off the file, or
        if information fields other than the file offset are being set.

--*/

{

    PNTFS_FILE FileInfo;
    FILE_INFORMATION FileInformationCopy;

    FileInfo = (PNTFS_FILE)FileEntry->FileData;
    if (CHECK_FLAG(FileInfo->Information.Flags, FILE_FLAGS_IS_DIRECTORY)) {
        return STATUS_FILE_IS_A_DIRECTORY;
    }

    //
    // The file offset is the only configurable setting.  Validate that the
    // user is not attempting to set any other settings.
    //

    RtlCopyMemory(&FileInformationCopy,
                  FileInformation,
                  sizeof(FILE_INFORMATION));

    FileInformationCopy.FileOffset = FileInfo->Information.FileOffset;
    if (RtlEqualMemory(&FileInformationCopy,
                       &FileInfo->Information,
                       sizeof(FILE_INFORMATION)) == FALSE) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Save the user configured setting for the file pointer.
    //

    if (FileInformation->FileOffset < FileInfo->Information.FileSize) {
        FileInfo->Information.FileOffset = FileInformation->FileOffset;
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
NtfsGetReparseData (
    __in PFILE_ENTRY FileEntry,
    __out PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    )

/*++

Routine Description:

    Copies FileEntry->FileData.Information to the caller's buffer.

Arguments:

    FileEntry - Pointer to the file entry (stored in the FileTable) for the
                file.

    FileInformation - Pointer to buffer to copy file information.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    PNTFS_FILE FileInfo;

    FileInfo = (PNTFS_FILE)FileEntry->FileData;

    //
    // Copy FileEntry->FileData.ReparseData to the caller's buffer.
    //

    RtlCopyMemory(ReparseData,
                  &FileInfo->ReparseData,
                  sizeof(WIM_PROVIDER_EXTERNAL_INFO));

    return STATUS_SUCCESS;
}

NTSTATUS
NtfsPurgeCache (
    VOID
    )

/*++

Routine Description:

    This routine purges all file record segment caches, freeing any associated
    resources.

Arguments:

    None.

Return Value:

    Always returns STATUS_SUCCESS.

--*/

{

    //
    // Purge each file record segment in the device table by mapping the
    // purge routine for each table entry.
    //

    BlTblMap(NtfsDeviceTable,
             NtfsDeviceTableEntries,
             NtfspDeviceTablePurgeEntry,
             FALSE);

    return STATUS_SUCCESS;
}

//
// --------------------------------------------------------- Internal Functions
//

//
// Local routines to help initialize / destroy NTFS data structures.
//

NTSTATUS
NtfspInitializeDeviceContext (
    __in DEVICE_ID DeviceId,
    __in PPACKED_BOOT_SECTOR BootSector,
    __deref_out PNTFS_DEVICE_CONTEXT *DeviceContext
    )

/*++

Routine Description:

    Allocates and initializes a NTFS Device Context structure for a NTFS
    volume.

Arguments:

    DeviceId - The Volume's device id. (Will be a Block I/O Device).

    BootSector - Validated BootSector for the NTFS volume.

    DeviceContext - On output, contains an allocated Ntfs Device Context
        structure for the volume.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation failed for any internal data structures.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if the $DATA attribute was not found.

--*/

{

    PBIOS_PARAMETER_BLOCK Bpb;
    LOGICAL ContextInserted;
    PNTFS_DEVICE_CONTEXT DevpContext;
    ULONG Index;
    NTSTATUS Status;

    ContextInserted = FALSE;

    //
    // Allocate a buffer for the device context.  In this structure we will
    // save the Bios Parameter Block, maintain a File Record Segment cache,
    // and any other volume level data.
    //

    DevpContext = BlMmAllocateHeap(sizeof(NTFS_DEVICE_CONTEXT));
    if (DevpContext == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Initialize NTFS Device Context for this volume.
    //

    RtlZeroMemory(DevpContext, sizeof(NTFS_DEVICE_CONTEXT));

    DevpContext->DeviceId = DeviceId;

    //
    // Save Bios Parameter Block for the volume.
    //

    NtfsUnpackBios(&DevpContext->Bpb, &BootSector->PackedBpb);

    //
    // Save other NTFS volume information from the boot sector.
    //

    DevpContext->NumberSectors = BootSector->NumberSectors;
    DevpContext->MftStartLcn = BootSector->MftStartLcn;
    DevpContext->BytesPerFileRecordSegment =
        (BootSector->ClustersPerFileRecordSegment < 0) ?
        1 << (-1 * BootSector->ClustersPerFileRecordSegment) :
        BootSector->ClustersPerFileRecordSegment *
        BytesPerCluster(&DevpContext->Bpb);

    DevpContext->DefaultBytesPerIndexAllocationBuffer =
        (BootSector->DefaultClustersPerIndexAllocationBuffer < 0) ?
        1 << (-1 * BootSector->DefaultClustersPerIndexAllocationBuffer) :
        BootSector->DefaultClustersPerIndexAllocationBuffer *
        BytesPerCluster(&DevpContext->Bpb);

    //
    // Compute the total number of clusters for the volume.
    //

    Bpb = &DevpContext->Bpb;
    for (Index = BytesPerCluster(Bpb); Index > 1; Index = Index / 2) {
        DevpContext->ClusterShift += 1;
    }

    DevpContext->ClusterMask = BytesPerCluster(Bpb) - 1;
    DevpContext->TotalClusters =
        ClustersFromBytesTruncate(
            DevpContext,
            (BootSector->NumberSectors * Bpb->BytesPerSector));

    //
    // Initialize File Record Segment cache.
    //

    Status = BlHtCreate(FILE_RECORD_HASH_TABLE_SIZE,
                        NtfspFileRecordHashFunction,
                        NtfspFileRecordCompareKey,
                        &DevpContext->FileRecordHashTableId);

    if (!NT_SUCCESS(Status)) {
        DevpContext->FileRecordHashTableId = INVALID_HASH_TABLE_ID;
        goto NtfspInitializeDeviceContextEnd;
    }

    //
    // Add the newly allocated and initialized volume context to the global
    // list.
    //
    // N.B. This must be done prior to any I/O operations.  If a file
    //      record segment was read prior to setting up the device table,
    //      cached file record segments would contain invalid device IDs.
    //      Besides having a meaningless cached FRS entry, an invalid device
    //      ID could also lead to an invalid address to be dereferenced if
    //      the device table is ever referenced using the device ID in a
    //      cached entry.
    //

    Status = BlTblSetEntry(&NtfsDeviceTable,
                           &NtfsDeviceTableEntries,
                           DevpContext,
                           &DevpContext->ContextIndex,
                           TblDoNotPurgeEntry);

    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeDeviceContextEnd;
    }

    ContextInserted = TRUE;

    //
    // Keep a resident handle of the MFT $DATA attribute.  The MFT is a
    // virtually contiguous File Record Table.  Each Index Entry will refer
    // back into the MFT with it's file record number.  The MFT is needed in
    // order to map the File Record to the actual File Record on disk.
    //

    Status = NtfspLookupAttribute(DevpContext,
                                  MASTER_FILE_TABLE_NUMBER,
                                  $DATA,
                                  &DevpContext->MftAttribute);

    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeDeviceContextEnd;
    }

    //
    // Set return value.
    //

    *DeviceContext = DevpContext;

NtfspInitializeDeviceContextEnd:

    //
    // Cleanup in the case of any errors.
    //

    if (!NT_SUCCESS(Status)) {
        if (DevpContext != NULL) {
            if (ContextInserted) {
                NtfspDeviceTableDestroyEntry(DevpContext,
                                             DevpContext->ContextIndex);

            } else {
                if (DevpContext->FileRecordHashTableId != INVALID_HASH_TABLE_ID) {
                    BlHtDestroy(DevpContext->FileRecordHashTableId);
                }

                BlMmFreeHeap(DevpContext);
            }

        }
    }

    return Status;
}

NTSTATUS
NtfspInitializeFileContext (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __deref_out PNTFS_FILE *FileContext
    )

/*++

Routine Description:

    Allocates and initialized the internal data structure for a ntfs file.  The
    data structure contains three types of information, generic information
    common to all file systems, ntfs specific context and a referenced to
    the Ntfs device.

    On successful return, the data structure will have been allocated and
    initialized.

Arguments:

    VolumeContext - Contextural information for the NTFS device the file is
        located.

    FrsNumber - The Base File Record Segment for the file.

    FileContext - On return, contains a pointer to File Data for the file
        referenced by FrsNumber.


Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if file does not contain a $DATA attribute.

--*/

{

    PNTFS_FILE File;
    PFILE_RECORD_SEGMENT_HEADER Frs;
    INDEX_ROOT IndexRoot;
    PNTFS_INDEX_ROOT IndexRootCache;
    NTSTATUS Status;

    //
    // Allocate a buffer for the file context.  This structure contains
    // internal data structures for maintaining the file.
    //

    File = BlMmAllocateHeap(sizeof(NTFS_FILE));
    if (File == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(File, sizeof(NTFS_FILE));

    //
    // Initialize the NTFS File structure.  The data structure is composed of
    // three fields, the NTFS specific information, the generic common (across
    // all file systems) information and the reference to the volume context
    // in the NtfsDeviceTable.
    //

    File->NtfsDeviceTableId = VolumeContext->ContextIndex;
    File->FileContext.BaseFileRecordSegment = FrsNumber;

    //
    // A directory is defined as having a File Name Attribute.  Read the file
    // record segment to determine if the file name attribute flag is set.
    //

    Status = NtfspGetFileRecordSegment(VolumeContext,
                                       FrsNumber,
                                       &Frs);

    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeFileContextEnd;
    }

    if (CHECK_FLAG(Frs->Flags, FILE_FILE_NAME_INDEX_PRESENT)) {
        SET_FLAGS(File->Information.Flags, FILE_FLAGS_IS_DIRECTORY);
    }

    //
    // Save a pointer to a global constant string, "NTFS".  This allows
    // an OS loader to query the filesystem type and know what driver to
    // load.
    //

    File->Information.FileSystemName = NTFS_DRIVER_NAME;

    //
    // If the file is a directory, an index root attribute exists and contains
    // directory entries.  If the one attribute is too small, additional index
    // allocations are made and stored in an index allocation attribute.  The
    // allocated FRSs are specified via a bitmap attribute.
    //
    // All other files will contain a data attribute which contains file
    // content.
    //

    if (CHECK_FLAG(File->Information.Flags, FILE_FLAGS_IS_DIRECTORY) != 0) {
        Status = NtfspLookupAttribute(VolumeContext,
                                      FrsNumber,
                                      $INDEX_ROOT,
                                      &File->FileContext.IndexRootAttribute);

        if (!NT_SUCCESS(Status)) {
            goto NtfspInitializeFileContextEnd;
        }

        //
        // The index allocation and bitmap attributes are not required.  It
        // is not considered a fatal failure if either attribute lookup
        // fails.
        //

        NtfspLookupAttribute(VolumeContext,
                             FrsNumber,
                             $INDEX_ALLOCATION,
                             &File->FileContext.IndexAllocationAttribute);

        NtfspLookupAttribute(VolumeContext,
                             FrsNumber,
                             $BITMAP,
                             &File->FileContext.BitmapAttribute);

        //
        // In attempt to minimize the number of reads performed on the physical
        // device, attempt to cache all directory meta-data.
        //

        SET_FLAGS(File->FileContext.IndexRootAttribute.Flags,
                  NTFS_ATTRIBUTE_CACHE_READ);

        SET_FLAGS(File->FileContext.IndexAllocationAttribute.Flags,
                  NTFS_ATTRIBUTE_CACHE_READ);

        SET_FLAGS(File->FileContext.BitmapAttribute.Flags,
                  NTFS_ATTRIBUTE_CACHE_READ);

        //
        // Consider the size of both the index root and index allocation to
        // be the size of a directory.  This value is only consumed by this
        // module.
        //

        File->Information.FileSize = File->FileContext.IndexRootAttribute.Size +
            File->FileContext.IndexAllocationAttribute.Size;

        //
        // Read the Index Root Attribute, caching the offset to the first
        // Index Entry and the size of each Index Allocation Buffer.
        //

        Status = NtfspReadAttribute(VolumeContext,
                                    &File->FileContext.IndexRootAttribute,
                                    0,
                                    sizeof(INDEX_ROOT),
                                    &IndexRoot);

        if (!NT_SUCCESS(Status)) {
            goto NtfspInitializeFileContextEnd;
        }

        IndexRootCache = &File->FileContext.IndexRoot;
        IndexRootCache->FirstIndexEntry = IndexRoot.IndexHeader.FirstIndexEntry;
        IndexRootCache->BytesPerIndexBuffer = IndexRoot.BytesPerIndexBuffer;

    } else {
        Status = NtfspLookupAttribute(VolumeContext,
                                      FrsNumber,
                                      $DATA,
                                      &File->FileContext.DataAttribute);

        if (!NT_SUCCESS(Status)) {
            goto NtfspInitializeFileContextEnd;
        }

        File->Information.FileSize = File->FileContext.DataAttribute.Size;

        //
        // Now check if the file has a reparse point and that reparse point
        // indicates it should be serviced by the WIM.
        //

        {
            NTFS_ATTRIBUTE_CONTEXT ReparseAttribute;
            WOF_REPARSE_DATA Reparse;

            Status = NtfspLookupAttribute(VolumeContext,
                                          FrsNumber,
                                          $REPARSE_POINT,
                                          &ReparseAttribute);

            //
            // If we don't have a reparse point, give the caller the
            // successful data open.  We know this wasn't meant for
            // the WIM.
            //

            if (!NT_SUCCESS(Status)) {
                Status = STATUS_SUCCESS;
                goto NtfspInitializeFileContextEnd;
            }

            //
            // It's possible that this will fail because we don't have
            // enough data in the attribute.  In that case we'll
            // assume it's not for us.
            //

            Status = NtfspReadAttribute(VolumeContext,
                                        &ReparseAttribute,
                                        0,
                                        RTL_SIZEOF_THROUGH_FIELD(WOF_REPARSE_DATA, Data.WofInfo),
                                        &Reparse.Data);

            if (!NT_SUCCESS(Status)) {
                Status = STATUS_SUCCESS;
                goto NtfspInitializeFileContextEnd;
            }

            //
            // Check for WOF reparse points which are handled by redirecting to
            // the file in the WIM image or performing on the fly decompression
            // of the data from the alternate stream.
            //

            if (Reparse.ReparseHeader.ReparseTag == IO_REPARSE_TAG_WOF) {

#ifndef FILE_MIN

                if (Reparse.Data.WofInfo.Version != WOF_CURRENT_VERSION) {
                    Status = STATUS_IO_REPARSE_TAG_NOT_HANDLED;
                    goto NtfspInitializeFileContextEnd;
                }

                if (Reparse.Data.WofInfo.Provider == WOF_PROVIDER_WIM) {
                    Status = NtfspReadAttribute(VolumeContext,
                                                &ReparseAttribute,
                                                FIELD_OFFSET(WOF_REPARSE_DATA, Data.WimInfo),
                                                sizeof(Reparse.Data.WimInfo),
                                                &Reparse.Data.WimInfo);

                    if (NT_SUCCESS(Status) &&
                        ((Reparse.Data.WimInfo.Version == WIM_PROVIDER_CURRENT_VERSION) ||
                         (Reparse.Data.WimInfo.Version == WIM_PROVIDER_EXTENDED_VERSION))) {

                        RtlCopyMemory(&File->ReparseData,
                                      &Reparse.Data.WimInfo,
                                      sizeof(WIM_PROVIDER_EXTERNAL_INFO));

                        Status = STATUS_REPARSE;

                    } else {
                        Status = STATUS_IO_REPARSE_DATA_INVALID;
                    }

                    goto NtfspInitializeFileContextEnd;

                } else if (Reparse.Data.WofInfo.Provider == WOF_PROVIDER_FILE) {
                    Status = NtfspReadAttribute(VolumeContext,
                                                &ReparseAttribute,
                                                FIELD_OFFSET(WOF_REPARSE_DATA, Data.FileInfo),
                                                sizeof(Reparse.Data.FileInfo),
                                                &Reparse.Data.FileInfo);

                    if (NT_SUCCESS(Status) &&
                        (Reparse.Data.FileInfo.Version == FILE_PROVIDER_SINGLE_FILE) &&
                        ((File->FileContext.Algorithm == FILE_PROVIDER_COMPRESSION_XPRESS4K) ||
                         (File->FileContext.Algorithm == FILE_PROVIDER_COMPRESSION_LZX))) {

                        Status = NtfspLookupAttributeEx(VolumeContext,
                                                        FrsNumber,
                                                        $DATA,
                                                        &NtfsWofDataStream,
                                                        &File->FileContext.DataAttribute);

                        if (!NT_SUCCESS(Status)) {
                            goto NtfspInitializeFileContextEnd;
                        }

                        //
                        // Boot environment does not support compressed files 4GB or
                        // greater in size.
                        //

                        if ((File->Information.FileSize >= _4GB) ||
                            (File->FileContext.DataAttribute.Size >= _4GB)) {

                            return STATUS_NOT_SUPPORTED;
                        }

                        File->FileContext.WimCompressed = TRUE;
                        File->FileContext.OriginalSize = (ULONG)File->Information.FileSize;
                        File->FileContext.CompressedSize = (ULONG)File->FileContext.DataAttribute.Size;
                        File->FileContext.Algorithm = Reparse.Data.FileInfo.Algorithm;
                        if (File->FileContext.Algorithm == FILE_PROVIDER_COMPRESSION_XPRESS4K) {
                            File->FileContext.CompressedChunkSize = WIM_XPRESS_CHUNK_SIZE;

                        } else {

                            NT_ASSERT(File->FileContext.Algorithm == FILE_PROVIDER_COMPRESSION_LZX);

                            File->FileContext.CompressedChunkSize = WIM_LZX_CHUNK_SIZE;
                        }

                        //
                        // Initialize compression context.
                        //

                        Status = NtfspInitializeCompressionContext(File->FileContext.Algorithm);
                        if (!NT_SUCCESS(Status)) {
                            goto NtfspInitializeFileContextEnd;
                        }

                    } else {
                        Status = STATUS_IO_REPARSE_DATA_INVALID;
                    }

                } else

#endif

                {
                    Status = STATUS_NOT_SUPPORTED;
                    goto NtfspInitializeFileContextEnd;
                }
            }
        }
    }

NtfspInitializeFileContextEnd:

    if (!NT_SUCCESS(Status)) {
        if (File != NULL) {
            BlMmFreeHeap(File);
        }

    } else {

        //
        // Set return value.
        //

        *FileContext = File;
    }

    return Status;
}

NTSTATUS
NtfspInitializeFileEntry (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PWSTR ParentFileName,
    __in PWSTR FileName,
    __in ULONGLONG FrsNumber,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Allocates and initializes a File Entry for the given file.  This includes
    initializing the NTFS specific File Context.

    On successful return, the data structure will have been allocated and
    initialized.

Arguments:

    VolumeContext - Contextural information for the NTFS device the file is
        located.

    ParentFileName - Filename for the parent directory.

    FileName - Filename that is being opened.

    FrsNumber - The Base File Record Segment for the file.

    FileEntry - On return, contains a pointer to a File Entry structure for the
        file referenced by FrsNumber and FileName.


Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if file does not contain a $DATA attribute.

--*/

{

    PFILE_ENTRY AllocatedFileEntry;
    PNTFS_FILE FileContext;
    ULONG FullFilePathSize;
    PWCHAR FullFilePath;
    ULONG ParentFileNameLength;
    NTSTATUS Status;
    PWCHAR StringFormat;

    AllocatedFileEntry = NULL;
    FileContext = NULL;

    //
    // Allocated a File Entry structure for the file being opened.
    //

    AllocatedFileEntry = BlMmAllocateHeap(sizeof(FILE_ENTRY));
    if (AllocatedFileEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeFileEntryEnd;
    }

    RtlZeroMemory(AllocatedFileEntry, sizeof(FILE_ENTRY));

    //
    // Allocate a buffer for the full filepath strored in the allocated
    // FILE_ENTRY.
    //

    ParentFileNameLength = (ULONG)wcslen(ParentFileName);
    FullFilePathSize = (ULONG)wcslen(FileName);
    Status = RtlULongAdd(FullFilePathSize,
                         ParentFileNameLength,
                         &FullFilePathSize);

    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeFileEntryEnd;
    }

    Status = RtlULongAdd(FullFilePathSize, 2, &FullFilePathSize);
    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeFileEntryEnd;
    }

    Status = RtlULongMult(FullFilePathSize, sizeof(WCHAR), &FullFilePathSize);
    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeFileEntryEnd;
    }

    FullFilePath = BlMmAllocateHeap(FullFilePathSize);
    if (FullFilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeFileEntryEnd;
    }

    //
    // Construct the full filepath.  Take care not to get into a situation
    // where the filepath contains adjacent backslashes.  The easiest case for
    // this is when opening a file in the root directory.
    //

    if ((ParentFileNameLength != 0) &&
        (ParentFileName[ParentFileNameLength - 1] == L'\\')) {

        StringFormat = L"%ls%ls";

    } else {
        StringFormat = L"%ls\\%ls";
    }

    swprintf_s(FullFilePath,
               (FullFilePathSize / sizeof(WCHAR)),
               StringFormat,
               ParentFileName,
               FileName);

    AllocatedFileEntry->FilePath = FullFilePath;

    //
    // Initialize file context for the file being opened.
    //

    Status = NtfspInitializeFileContext(VolumeContext,
                                        FrsNumber,
                                        &FileContext);

    if (!NT_SUCCESS(Status)) {
        goto NtfspInitializeFileEntryEnd;
    }

    //
    // Finish initializing File Entry.
    //

    AllocatedFileEntry->DeviceId = VolumeContext->DeviceId;
    AllocatedFileEntry->FileData = FileContext;

    //
    // Set the function table.
    //

    RtlCopyMemory(&AllocatedFileEntry->FunctionTable,
                  &NtfsFunctionTable,
                  sizeof(FILESYSTEM_FUNCTION_TABLE));

    //
    // If the file is WIM compressed, replace read/write routines with
    // appropriate versions which can handle compression.
    //

    if (CHECK_FLAG(FileContext->Information.Flags, FILE_FLAGS_IS_DIRECTORY)) {
        SET_FILE_DIRECTORY(AllocatedFileEntry->Attributes);

    }

#ifndef FILE_MIN

    else if (FileContext->FileContext.WimCompressed != FALSE) {
        AllocatedFileEntry->FunctionTable.Read = NtfspWimRead;
        AllocatedFileEntry->FunctionTable.Write = NtfspWimWrite;
    }

#endif

    //
    // Return Allocated File Entry to the caller.
    //

    *FileEntry = AllocatedFileEntry;

NtfspInitializeFileEntryEnd:

    if (!NT_SUCCESS(Status)) {
        if (AllocatedFileEntry != NULL) {
            if (AllocatedFileEntry->FilePath != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FilePath);
            }

            BlMmFreeHeap(AllocatedFileEntry);
        }

        if (FileContext != NULL) {
            BlMmFreeHeap(FileContext);
        }
    }

    return Status;
}

NTSTATUS
NtfspGetFileName (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __deref_inout_ecount(*FileNameLength) PWCHAR *FileName,
    __inout PULONG FileNameLength
    )

/*++

Routine Description:

    Provided a File Record Segment, this routine will search for the the
    $FILE_NAME attribute which is used to provide a file's name.  If the file
    has multiple file names, then it contains both a short (8.3) and long
    file name.  The long filename has precedence and will be found first.
    Therefore, this routine will always return a file's long file name.

Arguments:

    VolumeContext - Device context for a NTFS volume.

    FrsNumber - The File Record Number to query.

    FileName - On input, a pointer to a buffer for the FileName.  If the buffer
        is too small, a new buffer will be allocated and returned on output.

    FileNameLength - The length (in # of WCHARs) of the buffer allocated for
        FileName.  On return, this value will be updated if a new buffer was
        allocated for FileName.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when allocation is unsuccessful.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND when $FILE_NAME attibute does not exist.

--*/

{

    NTFS_ATTRIBUTE_CONTEXT AttributeContext;
    FILE_NAME FileNameHeader;
    NTSTATUS Status;

    //
    // Obtain a handle to the the $FILE_NAME attribute for the given file.
    // If the File Record contains multiple file name attributes, then it
    // contains an 8.3 name.  The first attribute is always the longname.
    // Therefore this routine will always find the longfilename for a FRS.
    //

    Status = NtfspLookupAttribute(VolumeContext,
                                  FrsNumber,
                                  $FILE_NAME,
                                  &AttributeContext);

    //
    // $FILE_NAME attribute does not exist for the specified File Record.
    //

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = NtfspReadAttribute(VolumeContext,
                                &AttributeContext,
                                0,
                                sizeof(FILE_NAME),
                                &FileNameHeader);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // We have read the File Name header.  This provides us with the exact size
    // of the File Name.  If the caller did not provide a large enough buffer
    // for the entire buffer, allocate a new buffer.
    //

    if (*FileNameLength < (ULONG)(FileNameHeader.FileNameLength + 1)) {
        if ((*FileName != NULL) && (*FileNameLength != 0)) {
            BlMmFreeHeap(*FileName);
        }

        *FileNameLength = FileNameHeader.FileNameLength + 1;
        *FileName = BlMmAllocateHeap((*FileNameLength) * sizeof(WCHAR));
        if (*FileName == NULL) {
            return STATUS_NO_MEMORY;
        }
    }

    //
    // Read the File Name for the requested File Record.
    //

    Status = NtfspReadAttribute(VolumeContext,
                                &AttributeContext,
                                FIELD_OFFSET(FILE_NAME, FileName),
                                FileNameHeader.FileNameLength * sizeof(WCHAR),
                                *FileName);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Zero-terminate filename buffer.
    //

    (*FileName)[FileNameHeader.FileNameLength] = L'\0';
    return STATUS_SUCCESS;
}

//
// Local routines for parsing a directory file record.
//

NTSTATUS
NtfspSearchForFileIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    )

/*++

Routine Description:

    This routine searches a directory for the specified file's index entry.
    When an entry is found, this routine will return an allocated copy of the
    index entry to the caller.

Arguments:

    DirectoryEntry - Supplies a file entry for the directory to search.

    FileName - Supplies a file name.

    IndexEntry - Supplies a pointer to a variable that receives the index
        entry for the requested file.  If the buffer is too small, this routine
        will allocate a new buffer that is large enough to receive the index
        entry.

        N.B. The caller is responsible for freeing this buffer.

    IndexEntrySize - Supplies a pointer to a variable containing the size of
        the index entry buffer.  If this routine allocates a new index entry
        buffer, this variable will receive the size of the allocated buffer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_SUCH_FILE if an index entry is not found for the specified file.

    Other errors from inability to find index root or index entries include:

    STATUS_INVALID_PARAMETER if the offset of a read request of an attribute
        is invalid.

    A non-resident attribute can also have the following error status codes.

    STATUS_INVALID_PARAMETER if a requested block is invalid.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PNTFS_FILE NtfsData;
    ULONGLONG StartingOffset;
    NTSTATUS Status;

    Status = NtfspResetDirectoryFilePointer(DirectoryEntry, FALSE);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    NtfsData = DirectoryEntry->FileData;
    StartingOffset = NtfsData->Information.FileOffset;
    Status = NtfspSearchForFileIndexEntryEx(DirectoryEntry,
                                            FileName,
                                            StartingOffset,
                                            IndexEntry,
                                            IndexEntrySize);

    return Status;
}

NTSTATUS
NtfspSearchForFileIndexEntryEx (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __in ULONGLONG StartingOffset,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    )

/*++

Routine Description:

    This routine searches a directory for the specified file's index entry.
    When an entry is found, this routine will return an allocated copy of the
    index entry to the caller.

Arguments:

    DirectoryEntry - Supplies a file entry for the directory to search.

    FileName - Supplies a file name.

    StartingOffset - Supplies the directory offset to start the search.

    IndexEntry - Supplies a pointer to a variable that receives the index
        entry for the requested file.  If the buffer is too small, this routine
        will allocate a new buffer that is large enough to receive the index
        entry.

        N.B. The caller is responsible for freeing this buffer.

    IndexEntrySize - Supplies a pointer to a variable containing the size of
        the index entry buffer.  If this routine allocates a new index entry
        buffer, this variable will receive the size of the allocated buffer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_SUCH_FILE if an index entry is not found for the specified file.

    Other errors from inability to find index root or index entries include:

    STATUS_INVALID_PARAMETER if the offset of a read request of an attribute
        is invalid.

    A non-resident attribute can also have the following error status codes.

    STATUS_INVALID_PARAMETER if a requested block is invalid.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PNTFS_FILE DirData;
    PWCHAR EntryFileName;
    PFILE_NAME FileNameEntry;
    UCHAR FileNameSize;
    ULONG Id;
    PNTFS_INDEX_ALLOCATION IndexAllocationCache;
    VBO IndexBuffer;
    VBO IndexBufferVbo;
    PNTFS_INDEX_ROOT IndexRoot;
    PNTFS_ATTRIBUTE_CONTEXT IndexRootAttribute;
    NTSTATUS Status;

    DirData = (PNTFS_FILE)DirectoryEntry->FileData;

    ASSERT(CHECK_FLAG(DirData->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    DirData->Information.FileOffset = StartingOffset;

    //
    // Update the index allocation cache if the starting offset is in the
    // index allocation attribute and the current cached buffer is invalid.
    //

    IndexRootAttribute = &DirData->FileContext.IndexRootAttribute;
    IndexRoot = &DirData->FileContext.IndexRoot;
    if (StartingOffset >= IndexRootAttribute->Size) {
        IndexBufferVbo = StartingOffset - IndexRootAttribute->Size;
        IndexBufferVbo = ALIGN_RANGE_DOWN(IndexBufferVbo,
                                          IndexRoot->BytesPerIndexBuffer);

        IndexBuffer = IndexBufferVbo / IndexRoot->BytesPerIndexBuffer;
        IndexAllocationCache = &DirData->FileContext.IndexAllocation;
        if (IndexAllocationCache->IndexAllocationBuffer != IndexBuffer + 1) {
            Status = NtfspReadDirectoryIndexAllocationBuffer(DirectoryEntry,
                                                             IndexBuffer);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }
        }
    }

    //
    // Read directory index entries until the entry for the specified file is
    // found or the search is exhausted.
    //

    FileNameSize = (UCHAR)wcslen(FileName);
    FileName = _wcsupr(FileName);            // make compare case insensitive.
    Status = NtfspGetNextFileIndexEntry(DirectoryEntry,
                                        IndexEntry,
                                        IndexEntrySize);

    while (Status == STATUS_SUCCESS) {

        //
        // Check the current Index Entry and determine if it represents
        // the requested file.
        //

        FileNameEntry = Add2Ptr(*IndexEntry, sizeof(INDEX_ENTRY));
        if (FileNameSize == FileNameEntry->FileNameLength) {

            //
            // The filename stored in the Index Entry is not null terminated.
            // To create a case insensitive search, manually capitalize the
            // string.
            //

            EntryFileName = &FileNameEntry->FileName[0];
            for (Id = 0; Id < FileNameSize; Id += 1) {
                EntryFileName[Id] = ToUpperW(EntryFileName[Id]);
            }

            //
            // If a match is found with the caller's request filename,
            // return successfully.
            //

            if (RtlEqualMemory(FileName,
                               &FileNameEntry->FileName[0],
                               FileNameSize * 2) != FALSE) {

                return STATUS_SUCCESS;
            }
        }

        //
        // Get next Index Entry.
        //

        Status = NtfspGetNextFileIndexEntry(DirectoryEntry,
                                            IndexEntry,
                                            IndexEntrySize);
    }

    return STATUS_NO_SUCH_FILE;
}

NTSTATUS
NtfspGetNextFileIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    )

/*++

Routine Description:

    Searches the specified directory for the next Index Entry that describes
    a File.  If found, this routine will return a buffer with the contents
    of the Index Entry.

    This routine assumes DirectoryEntry describes a directory.

Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    IndexEntry - On successful return, contains a pointer with an *allocated*
        buffer containing the Index Entry for the requested file.  The caller
        is responsible for freeing the buffer.  On input, *IndexEntry can be
        NULL if *IndexEntrySize equals zero.

    IndexEntrySize - On input, contains the size of the pointer pointed to by
        IndexEntry.  On output, contains the size.  The size could have changed
        if a new buffer was allocated for IndexEntry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if a file with the name FileName is not found in the
        searching directory.

    Other errors from inability to find index root or index entries include:
    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PNTFS_FILE DirData;
    PFILE_NAME FileNameEntry;
    ULONGLONG FileNumber;
    ULONG NameFlags;
    NTSTATUS Status;

    DirData = (PNTFS_FILE)DirectoryEntry->FileData;

    ASSERT(CHECK_FLAG(DirData->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    //
    // Continue to read Index Entries until one is found describing FileName.
    //

    Status = NtfspGetNextIndexEntry(DirectoryEntry,
                                    IndexEntry,
                                    IndexEntrySize);

    while (Status == STATUS_SUCCESS) {
        FileNameEntry = Add2Ptr(*IndexEntry, sizeof(INDEX_ENTRY));

        //
        // Don't bother with the following checks unless the index entry's
        // size accounts for an entire filename structure.
        //

        if (*IndexEntrySize >= (sizeof(FILE_NAME) + sizeof(INDEX_ENTRY))) {

            //
            // System files all have well known file numbers and are all under
            // LAST_SYSTEM_FILE_NUMBER in value.  The first user file number is
            // FIRST_USER_FILE_NUMBER.  Values between LAST_SYSTEM_FILE_NUMBER
            // and FIRST_USER_FILE_NUMBER are reserved.  Ignore all system and
            // reserved files.
            //
            // Additionally, ignore all index entries used for internal meta
            // data.  For example, an entry exists to mark the end of an index
            // allocation buffer.
            //
            // Lastly, do not enumerate entries that only have a short-file
            // name.  If FILE_NAME.Flags has FILE_NAME_NTFS set, the file
            // contains a long file name.  If FILE_NAME.Flags has
            // FILE_NAME_DOS set, the file contains a short file name.  A file
            // can have either one of these flags set, or both (a file can
            // contain multiple $FILE_NAME attributes). It is additionally
            // valid for FILE_NAME.Flags to be zero.  This indicates that
            // NtfsDisable8dot3NameCreation was set to TRUE and the file only
            // has a long file name.
            //

            NameFlags = FileNameEntry->Flags;
            FileReferenceToLargeInt((*IndexEntry)->FileReference, &FileNumber);
            if ((!CHECK_FLAG((*IndexEntry)->Flags, INDEX_ENTRY_END)) &&
                (FIRST_USER_FILE_NUMBER <= FileNumber) &&
                ((NameFlags == 0) || CHECK_FLAG(NameFlags, FILE_NAME_NTFS))) {

                return STATUS_SUCCESS;
            }
        }

        //
        // Get next Index Entry.
        //

        Status = NtfspGetNextIndexEntry(DirectoryEntry,
                                        IndexEntry,
                                        IndexEntrySize);
    }

    return STATUS_NO_SUCH_FILE;
}

NTSTATUS
NtfspGetNextIndexEntry (
    __in PFILE_ENTRY DirectoryEntry,
    __deref_out_bcount_opt(*IndexEntrySize) PINDEX_ENTRY *IndexEntry,
    __inout PULONG IndexEntrySize
    )

/*++

Routine Description:

    Returns the next Index Entry for the specified Directory.  A Directory will
    contain Index Entries for each file, referencing their base file record
    segment.  This routine will return the next Index Entry, and update the
    current file pointer to point to the next Index Entry.

    This routine assumes DirectoryEntry describes a directory.

Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    IndexEntry - On input, contains a pointer to a buffer.  (*IndexEntry can be
        NULL).  On successful return, contains a pointer to the next Index
        Entry.  If the input buffer was too small, a new buffer will be
        allocated (and the previous buffer will be freed).

    IndexEntrySize - On input contains the size of IndexEntry.  On output,
        contains the size of IndexEntry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if a file with the name FileName is not found in the
        searching directory.
    STATUS_UNEXPECTED_IO_ERROR if a corrupted data structure is read from
        the directory.

    Other errors from inability to find index root or index entries include:
    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONGLONG AlignedIndexBuffer;
    PNTFS_ATTRIBUTE_CONTEXT BitmapAttribute;
    PNTFS_FILE DirData;
    PNTFS_ATTRIBUTE_CONTEXT IndexAllocationAttribute;
    PNTFS_INDEX_ALLOCATION IndexAllocationCache;
    VBO IndexBuffer;
    VBO IndexBufferVbo;
    VBO IndexBufferHeaderVbo;
    INDEX_ENTRY IndexEntryHeader;
    PNTFS_ATTRIBUTE_CONTEXT IndexRootAttribute;
    PNTFS_INDEX_ROOT IndexRootCache;
    VBO IndexRootVbo;
    BOOLEAN IsAllocated;
    NTSTATUS Status;
    PNTFS_DEVICE_CONTEXT VolumeContext;

    //
    // Initialize local variables.
    //

    DirData = (PNTFS_FILE)DirectoryEntry->FileData;
    VolumeContext = NtfsDeviceTable[DirData->NtfsDeviceTableId];

    ASSERT(CHECK_FLAG(DirData->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    IndexRootAttribute = &DirData->FileContext.IndexRootAttribute;
    IndexRootCache = &DirData->FileContext.IndexRoot;

    IndexAllocationAttribute = &DirData->FileContext.IndexAllocationAttribute;
    IndexAllocationCache = &DirData->FileContext.IndexAllocation;

    //
    // Return if the current file pointer is at the end of the file.
    //

    if (DirData->Information.FileOffset == DirData->Information.FileSize) {
        return STATUS_END_OF_FILE;
    }

    //
    // A directory consists of index entries that are keys referencing
    // File Record Segments for each entry.  These entries can be files that
    // form the directory.  To search for an index entry, we must read the
    // directory comparing the filenames of each.  The Index Entries are
    // scattered through runs in the Index Root and in the Index Allocation
    // Buffers of the Index Allocation Attribute.  Together, they form a
    // b-tree, but they can also be searched linearly.  This routine reads the
    // Index Entries linearly.
    //

    //
    // Determine the current offset within the directory (split between the
    // index root and index allocation attributes).
    //

    if (DirData->Information.FileOffset < IndexRootAttribute->Size) {
        IndexRootVbo = DirData->Information.FileOffset;
        IndexBufferVbo = 0;
        IndexBufferHeaderVbo = 0;

    } else {
        IndexRootVbo = IndexRootAttribute->Size;
        IndexBufferVbo = DirData->Information.FileOffset - IndexRootVbo;
        IndexBufferHeaderVbo =
            ALIGN_RANGE_DOWN(IndexBufferVbo,
                             IndexRootCache->BytesPerIndexBuffer);

        ASSERT(IndexAllocationCache->MultiSectorHeader != NULL);
    }

    //
    // Read the next Index Entry based on the current offset.  Earlier, we
    // determined the current offset both into the Index Root and Index
    // Allocation.  If the Index Allocation Offset is set, then this read
    // should be from the Index Allocation Offset.  Otherwise it should be
    // made from the Index Root.
    //
    // Initially read just the header to determine the size of the buffer that
    // is required to read the entire Index Entry.
    //

    if (DirData->Information.FileOffset < IndexRootAttribute->Size) {

        //
        // Read Index Entry from Index Root.
        //

        Status = NtfspReadAttribute(VolumeContext,
                                    IndexRootAttribute,
                                    IndexRootVbo,
                                    sizeof(INDEX_ENTRY),
                                    &IndexEntryHeader);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    } else {

        //
        // Read Index Entry from Index Allocation.
        //

        Status = NtfspReadAttribute(VolumeContext,
                                    IndexAllocationAttribute,
                                    IndexBufferVbo,
                                    sizeof(INDEX_ENTRY),
                                    &IndexEntryHeader);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // If required, decode the Index Entry's header (this only needs to
        // be done if it straddles a Stride boundary.
        //

        Status = NtfspDecodeUsa(IndexAllocationCache->MultiSectorHeader,
                                &IndexEntryHeader,
                                sizeof(INDEX_ENTRY),
                                IndexBufferVbo - IndexBufferHeaderVbo);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    }

    //
    // If the ondisk structure contains an erroneous size, there is either an
    // external bug (firmware or library) or the disk is corrupt.
    //

    if (IndexEntryHeader.Length < sizeof(INDEX_ENTRY)) {
        return STATUS_UNEXPECTED_IO_ERROR;
    }

    //
    // If a new buffer needs to be allocated to store the entire Index Entry,
    // do so now.  Otherwise, reuse the entry.  If a new buffer is allocated,
    // free the existing one.
    //

    if (*IndexEntrySize < IndexEntryHeader.Length) {
        if (*IndexEntry != NULL) {
            BlMmFreeHeap(*IndexEntry);
        }

        *IndexEntry = BlMmAllocateHeap(IndexEntryHeader.Length);
        *IndexEntrySize = IndexEntryHeader.Length;
        if (*IndexEntry == NULL) {
            return STATUS_NO_MEMORY;
        }
    }

    //
    // Read the entire Index Entry into memory.
    //

    if (DirData->Information.FileOffset < IndexRootAttribute->Size) {

        //
        // Read Index Entry from Index Root.
        //

        Status = NtfspReadAttribute(VolumeContext,
                                    IndexRootAttribute,
                                    IndexRootVbo,
                                    IndexEntryHeader.Length,
                                    *IndexEntry);

    } else {

        //
        // Read Index Entry from Index Allocation.
        //

        Status = NtfspReadAttribute(VolumeContext,
                                    IndexAllocationAttribute,
                                    IndexBufferVbo,
                                    IndexEntryHeader.Length,
                                    *IndexEntry);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // If required, decode the Index Entry's header (this only needs to
        // be done if it straddles a Stride boundary.
        //

        Status = NtfspDecodeUsa(IndexAllocationCache->MultiSectorHeader,
                                *IndexEntry,
                                IndexEntryHeader.Length,
                                IndexBufferVbo - IndexBufferHeaderVbo);
    }

    //
    // An error here would be rather unexpected since the header claimed the
    // Index Entry was the requested size.  Return a more noticable error in
    // this case.
    //

    if (!NT_SUCCESS(Status)) {
        return STATUS_UNEXPECTED_IO_ERROR;
    }

    //
    // Currently, we have read an entire Index Entry into memory.  This entry
    // will be returned to the caller.  Before returning, we must update the
    // file pointer.  For a directory, the file pointer simply marks the
    // location of the next Index Entry.  Therefore, we must determine the next
    // entry, even if this Index Entry was for an intermediate node.
    //

    //
    // An End Entry marks the end of an Index Allocation.  A Node entry marks
    // a pointer into an Index Allocation Buffer.
    //
    // If this entry is a leaf end (end of an index buffer), we must read the
    // next Index Allocation Buffer entry to determine the new file pointer.
    // Additionally, multiple Index Allocation Buffers may need to be walked
    // since the one pointed to may not be in use (not set in BitMap
    // Attribute).
    //

    if (CHECK_FLAG(IndexEntryHeader.Flags, INDEX_ENTRY_END)) {
        do {

            //
            // Determine the offset to the next Index Allocation Buffer.  If we
            // just finished reading the Index Root, we will read the first
            // Index Allocation Buffer.
            //

            AlignedIndexBuffer =
                ALIGN_RANGE_UP(IndexBufferVbo,
                               IndexRootCache->BytesPerIndexBuffer);

            if (AlignedIndexBuffer < (ULONGLONG)IndexBufferVbo) {
                Status = STATUS_INTEGER_OVERFLOW;
                break;
            }

            IndexBufferVbo = (VBO)AlignedIndexBuffer;
            IndexBuffer = IndexBufferVbo / IndexRootCache->BytesPerIndexBuffer;

            //
            // Read the next Index Allocation Buffer header, caching the
            // Multi-Sector Header, Update Sequence Array and offset to the
            // first Index Entry.
            //

            Status = NtfspReadDirectoryIndexAllocationBuffer(DirectoryEntry,
                                                             IndexBuffer);

            //
            // NtfspReadAttribute will return STATUS_INVALID_PARAMETER if the
            // request is off the end of the attribute data.  If the next
            // Index Buffer is off the end of the Index Allocaion Attribute,
            // ensure that we return successfully since we did find a valid
            // index entry.
            //

            if (!NT_SUCCESS(Status)) {
                if (Status == STATUS_INVALID_PARAMETER) {
                    Status = STATUS_SUCCESS;
                    break;
                }

                return Status;
            }

            //
            // Update the current offset to point to the first Index Entry of
            // this Index Allocation Buffer.
            //

            IndexBufferVbo += IndexAllocationCache->FirstIndexEntry;
            IndexBufferVbo += FIELD_OFFSET(INDEX_ALLOCATION_BUFFER,IndexHeader);

            //
            // Determine if this Index Buffer is in use.
            //

            BitmapAttribute = &DirData->FileContext.BitmapAttribute;
            Status = NtfspIsIndexBufferAllocated(VolumeContext,
                                                 BitmapAttribute,
                                                 (ULONG)IndexBuffer,
                                                 &IsAllocated);

            //
            // We shouldn't expect an error from reading the Bitmap Attribute.
            // There is a one to one mapping between the Bitmap Attribute and
            // the Index Allocation Attribute.
            //

            if (!NT_SUCCESS(Status)) {
                return STATUS_UNEXPECTED_IO_ERROR;
            }

            //
            // If the Index Allocation Buffer is not allocated, then we need to
            // walk past this Index Allocation Buffer.
            //

        } while (IsAllocated == FALSE);

        //
        // Set the directory's file pointer to represent the start of the next
        // index allocation.
        //

        DirData->Information.FileOffset = IndexRootAttribute->Size +
                                          IndexBufferVbo;

    } else {

        //
        // In all other cases, we can simply update the file pointer by the
        // number of bytes read for the Index Entry.
        //

        DirData->Information.FileOffset += IndexEntryHeader.Length;
    }

    return Status;
}

NTSTATUS
NtfspIsIndexBufferAllocated (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT BitmapAttribute,
    __in ULONG BitOffset,
    __out PBOOLEAN IsAllocated
    )

/*++

Routine Description:

    Determines if the specified index allocation buffer is in use.  If it is
    in use, the associated bit offset would be set.

Arguments:

    VolumeContext - Device context for the NTFS Volume.

    BitmapAttribute - Bitmap Attribute to read.

    BitOffset - Offset in bitmap to read.  The bit offset refers to the queried
        index allocation buffer.  An Index Allocation attribute is partitioned
        into buffers of IndexRoot.BytesPerIndexBuffer size.  BitOffset refers
        the IndexBuffer number.

    IsAllocated - On return, set to TRUE if allocation buffer is allocated.

Return Value:

    STATUS_SUCCESS when successful.

    Other Error if error occurs reading BitmapAttribute (including):
    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONG BitIndex;
    ULONG ByteIndex;
    UCHAR LocalByte;
    NTSTATUS Status;

    //
    // This routine will only read the one byte which contains the one bit at
    // BitOffset.
    //

    ByteIndex = BitOffset / 8;
    BitIndex = BitOffset % 8;

    //
    // Read the single byte containing the single bit we are interested in.
    //

    Status = NtfspReadAttribute(VolumeContext,
                                BitmapAttribute,
                                ByteIndex,
                                1,
                                &LocalByte);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set return value.  Perform a simple bit check to determine if the
    // queried bit offset is set.
    //

    *IsAllocated = (CHECK_FLAG(LocalByte, 1 << BitIndex) ? TRUE : FALSE);
    return STATUS_SUCCESS;
}

NTSTATUS
NtfspResetDirectoryFilePointer (
    __in PFILE_ENTRY DirectoryEntry,
    __in BOOLEAN ResetGetNextFileOffset
    )

/*++

Routine Description:

    This routine will reset a directory's file offset to the first index entry.
    It will additionally reset the cached get next file offset when specified
    by the caller.

Arguments:

    DirectoryEntry - Supplies a file entry for a directory.

    ResetGetNextFileOffset - Supplies a boolean indicating if the cached get
        next file offset value should be reset.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    PNTFS_FILE DirData;
    PNTFS_INDEX_ROOT IndexRoot;
    ULONGLONG StartingOffset;

    DirData = (PNTFS_FILE)DirectoryEntry->FileData;

    ASSERT(CHECK_FLAG(DirData->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    IndexRoot = &DirData->FileContext.IndexRoot;
    StartingOffset =
        FIELD_OFFSET(INDEX_ROOT, IndexHeader) + IndexRoot->FirstIndexEntry;

    DirData->Information.FileOffset = StartingOffset;
    if (ResetGetNextFileOffset != FALSE) {
        DirData->FileContext.GetNextFileOffset = StartingOffset;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
NtfspReadDirectoryIndexAllocationBuffer (
    __inout PFILE_ENTRY DirectoryEntry,
    __in VBO IndexBuffer
    )

/*++

Routine Description:

    This routine will read the header for the specified Index Allocation
    Buffer.  When successful, the Multi Sector Header and Update Sequence
    Array will be cached, in addition to the offset to the first Index
    Entry in the buffer.

Arguments:

    DirectoryEntry - File entry for the Directory.

    IndexBuffer - The Index Allocation Buffer to read.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_NO_MEMORY when an allocation for the multi-sector header fails.

    Error if unsuccessful reading Index Allocation Attribute:
    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONG BufferSize;
    PNTFS_FILE DirData;
    INDEX_ALLOCATION_BUFFER IndexAllocation;
    PNTFS_ATTRIBUTE_CONTEXT IndexAllocationAttribute;
    PNTFS_INDEX_ALLOCATION IndexAllocationCache;
    VBO IndexBufferVbo;
    PNTFS_INDEX_ROOT IndexRootCache;
    PMULTI_SECTOR_HEADER MultiSectorHeader;
    NTSTATUS Status;
    PNTFS_DEVICE_CONTEXT VolumeContext;

    DirData = (PNTFS_FILE)DirectoryEntry->FileData;
    VolumeContext = NtfsDeviceTable[DirData->NtfsDeviceTableId];

    //
    // Index Allocation Buffers are virtually contiguous.  Therefore, the
    // virtual offset to the specified buffer is just a multiple of a
    // single buffer's size.
    //

    IndexRootCache = &DirData->FileContext.IndexRoot;
    IndexBufferVbo = IndexRootCache->BytesPerIndexBuffer * IndexBuffer;

    //
    // Read the header for the specified Index Allocation Buffer.  The
    // Update Sequence Array immediately follows the header, but is dynamic
    // in size.  Therefore, the header must first be read to know the size
    // of the array.
    //

    IndexAllocationAttribute = &DirData->FileContext.IndexAllocationAttribute;
    Status = NtfspReadAttribute(VolumeContext,
                                IndexAllocationAttribute,
                                IndexBufferVbo,
                                sizeof(INDEX_ALLOCATION_BUFFER),
                                &IndexAllocation);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Allocate a buffer for both the header and the Update Sequence Array.
    // Do not use the currently cached buffer.  In the event of a failure,
    // this routine must not invalidate the currently cached data.
    //

    BufferSize = IndexAllocation.MultiSectorHeader.UpdateSequenceArraySize;
    BufferSize *= sizeof(USHORT);  // UsaSize is an element count.
    BufferSize += sizeof(INDEX_ALLOCATION_BUFFER);
    MultiSectorHeader = BlMmAllocateHeap(BufferSize);
    if (MultiSectorHeader == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Read the Index Allocation Buffer header + Update Sequence Array into
    // the buffer for cache.
    //

    Status = NtfspReadAttribute(VolumeContext,
                                IndexAllocationAttribute,
                                IndexBufferVbo,
                                BufferSize,
                                MultiSectorHeader);

    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(MultiSectorHeader);
        return Status;
    }

    //
    // Save the multi-sector header, update sequence array and offset to the
    // first index entry.  Describe the index buffer as 1-relative instead of
    // 0 relative to distinguish buffer 0 from an uninitialized version of the
    // structure.
    //

    IndexAllocationCache = &DirData->FileContext.IndexAllocation;
    IndexAllocationCache->IndexAllocationBuffer = IndexBuffer + 1;
    IndexAllocationCache->FirstIndexEntry =
        IndexAllocation.IndexHeader.FirstIndexEntry;

    if (IndexAllocationCache->MultiSectorHeader != NULL) {
        BlMmFreeHeap(IndexAllocationCache->MultiSectorHeader);
    }

    IndexAllocationCache->MultiSectorHeader = MultiSectorHeader;
    return STATUS_SUCCESS;
}

//
// Local routines for looking up and reading an attribute.
//

NTSTATUS
NtfspLookupAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    )

/*++

Routine Description:

    This routine will read a File Record Segment into memory and look for the
    requested attribute.  The attribute can be either resident or non-resident.

    If the attribute is found, AttributeContext will be initialized with the
    values for the requested attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    FileRecord - File Record Number for the request.

    TypeCode - The type of attribute to lookup.

    AttributeContext - Pointer to a Ntfs Attribute Context structure.  On
        successful return, this data structure will be populated based on
        the Attribute Header.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_NO_MEMORY when a buffer can not be allocated to read the File Record
        Segment into memory.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if an attribute with type, TypeCode is not found.

--*/

{

    return NtfspLookupAttributeEx(VolumeContext,
                                  FileRecord,
                                  TypeCode,
                                  &NtfsEmptyName,
                                  AttributeContext);
}

NTSTATUS
NtfspLookupAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    )

/*++

Routine Description:

    This routine will read a File Record Segment into memory and look for the
    requested attribute.  The attribute can be either resident or non-resident.

    If the attribute is found, AttributeContext will be initialized with the
    values for the requested attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    FileRecord - File Record Number for the request.

    TypeCode - The type of attribute to lookup.

    AttributeName - Name for the $DATA stream.

    AttributeContext - Pointer to a Ntfs Attribute Context structure.  On
        successful return, this data structure will be populated based on
        the Attribute Header.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_NO_MEMORY when a buffer can not be allocated to read the File Record
        Segment into memory.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if an attribute with type, TypeCode is not found.

--*/

{

    return NtfspLookupAttributeVcn(VolumeContext,
                                   FileRecord,
                                   TypeCode,
                                   AttributeName,
                                   0,
                                   AttributeContext);
}

NTSTATUS
NtfspLookupAttributeVcn (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecord,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __in VCN Vcn,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    )

/*++

Routine Description:

    This routine will read a File Record Segment into memory and look for the
    requested attribute.  The attribute can be either resident or
    non-resident.  If the attribute is non-resident, this routine will
    look for the attribute record containing the specified cluster.

    If the attribute is found, AttributeContext will be initialized with the
    values for the requested attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    FileRecord - File Record Number for the request.

    TypeCode - The type of attribute to lookup.

    AttributeName - Name for the $DATA stream.

    Vcn - The cluster number of the attribute to find.  An attribute may
        span multiple attribute records with each containing a window of the
        entire attribute.

    AttributeContext - Pointer to a Ntfs Attribute Context structure.  On
        successful return, this data structure will be populated based on
        the Attribute Header.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_NO_MEMORY when a buffer can not be allocated to read the File Record
        Segment into memory.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if an attribute with type, TypeCode is not found.

--*/

{

    PVOID Frs;
    NTSTATUS Status;

    //
    // Get the requested File Record Segment.
    //

    Status = NtfspGetFileRecordSegment(VolumeContext,
                                       FileRecord,
                                       &Frs);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Call the helper routine that will actually walk the File Record Segment
    // looking for the desired attribute.
    //

    Status = NtfspLookupAttributeVcnEx(VolumeContext,
                                       Frs,
                                       FileRecord,
                                       TypeCode,
                                       AttributeName,
                                       Vcn,
                                       AttributeContext);

    return Status;
}

NTSTATUS
NtfspLookupAttributeVcnEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PVOID FileRecord,
    __in ULONGLONG FileRecordNumber,
    __in ATTRIBUTE_TYPE_CODE TypeCode,
    __in PCUNICODE_STRING AttributeName,
    __in VCN Vcn,
    __out PNTFS_ATTRIBUTE_CONTEXT AttributeContext
    )

/*++

Routine Description:

    Provided a File Record Segment, this routine will look for the requested
    attribute.  The attribute can be either resident or non-resident.  If the
    attribute is non-resident, this routine will recurse on the Attribute List
    by calling NtfspLookupAttribute to additionally read the chained File
    Record Segment.

    If the attribute is found, AttributeContext will be initialized with the
    values for the requested attribute.

    N.B. The ondisk attribute record header does not contain populated fields
         for the entire header of a non-zero VCN attribute record.  If a
         non-zero VCN header is requested, only the following fields will be
         populated in the output parameter AttributeContext.
            - FileRecordSegment
            - FileRecordOffset
            - TypeCode
            - NTFS_ATTRIBUTE_RESIDENT flag for resident attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    FileRecord - File Record Segment for the request.

    FileRecordNumber - The FRS number for the above buffer.

    TypeCode - The type of attribute to lookup.

    AttributeName - Name for the $DATA stream.

    Vcn - The cluster number of the attribute to find.  An attribute may
        span multiple attribute records with each containing a window of the
        entire attribute.

    AttributeContext - Pointer to a Ntfs Attribute Context structure.  On
        successful return, this data structure will be populated based on
        the Attribute Header.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_DEVICE_DATA_ERROR if there is an error reading the File Record
        Segment into memory.
    STATUS_NOT_FOUND if an attribute with type, TypeCode is not found.

--*/

{

    ULONG AccessEndOffset;
    NTFS_ATTRIBUTE_CONTEXT AttributeList;
    ATTRIBUTE_LIST_ENTRY AttributeListEntry;
    WCHAR AttributeNameBuffer[RTL_NUMBER_OF(WOF_DATA_STREAM) - 1];
    ULONG CompressionFormat;
    ULONG CompressionUnit;
    BOOLEAN FoundAttribute;
    PFILE_RECORD_SEGMENT_HEADER Frs;
    PVOID FrsMax;
    PATTRIBUTE_RECORD_HEADER Header;
    ULONG Id;
    ULONGLONG ListFileRecordNumber;
    ULONG ListOffset;
    PATTRIBUTE_RECORD_HEADER PreviousHeader;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(VolumeContext);

    //
    // Verify that the buffer is big enough to hold attribute name.
    //
    // N.B. WOF data stream is the only named data stream used in the boot
    //      environment.
    //

    NT_ASSERT_ASSUME(AttributeName->Length <= sizeof(AttributeNameBuffer));

    Frs = (PFILE_RECORD_SEGMENT_HEADER)FileRecord;
    RtlZeroMemory(&AttributeList, sizeof(NTFS_ATTRIBUTE_CONTEXT));

    ListFileRecordNumber = 0; // Satisfy compiler.

    //
    // Walk the attributes of the File Record Segment, looking for the
    // requested attribute.
    //

    Header = NtfsFirstAttribute(Frs);
    FrsMax = Add2Ptr(Frs, VolumeContext->BytesPerFileRecordSegment);
    do {

        //
        // The attribute header must be dword-aligned.
        //

        if ((UINT_PTR)Header & 0x3) {
            BlStatusPrint(L"NTFS: Unaligned file attribute header, "
                          L"Header=0x%08p, FRS=0x%08p\n",
                          Header,
                          Frs);

            break;
        }

        //
        // Before dereferencing any field, ensure that it is contained within
        // the provided file record segment.  If not, the file record segment
        // is likely corrupt and the search should be terminated.
        //

        AccessEndOffset = FIELD_OFFSET(ATTRIBUTE_RECORD_HEADER, RecordLength);
        if (Add2Ptr(Header, AccessEndOffset) > FrsMax) {
            BlStatusPrint(L"NTFS: Attribute type not contained in file record "
                          L"segment (0x%08p)!\n",
                          Frs);

            break;
        }

        //
        // An attribute list is sorted by TYPE_CODE.  Terminate the search when
        // appropriate.
        //

        if ((Header->TypeCode == $END) ||
            ((TypeCode < Header->TypeCode) &&
            (AttributeList.TypeCode == $ATTRIBUTE_LIST))) {

            break;
        }

        //
        // Ensure that the entire attribute is accessible.  The header's size
        // depends on whether the attribute is resident or non-resident.
        //

        AccessEndOffset = FIELD_OFFSET(ATTRIBUTE_RECORD_HEADER, Form);
        if (Add2Ptr(Header, AccessEndOffset) > FrsMax) {
            BlStatusPrint(L"NTFS: Attribute header not contained within "
                          L"file record segment, Header=0x%08p, FRS=0x%08p\n",
                          Header,
                          Frs);

            break;
        }

        if (Header->FormCode == NONRESIDENT_FORM) {
            AccessEndOffset += sizeof(Header->Form.Nonresident);

        } else {
            AccessEndOffset += sizeof(Header->Form.Resident);
        }

        if (Add2Ptr(Header, AccessEndOffset) > FrsMax) {
            BlStatusPrint(L"NTFS: Attribute header not contained within "
                          L"file record segment, Header=0x%08p, FRS=0x%08p\n",
                          Header,
                          Frs);

            break;
        }

        //
        // Check to see if the current attribute is the one requested by the
        // caller.  If the user requested the $DATA attribute, make sure the
        // attribute is unnamed.  Additionally, if the attribute is non-
        // resident, find the attribute record that describes the requested
        // VCN.
        //

        if (((Header->TypeCode == TypeCode) &&
             ((TypeCode != $DATA) ||
              ((AttributeName->Length / sizeof(WCHAR) == Header->NameLength) &&
               (_wcsnicmp(AttributeName->Buffer,
                          (PWSTR)Add2Ptr(Header, Header->NameOffset),
                          Header->NameLength) == 0))))

                 &&

            ((Header->FormCode != NONRESIDENT_FORM) ||
             ((Header->Form.Nonresident.LowestVcn <= Vcn) &&
              (Vcn <= Header->Form.Nonresident.HighestVcn)))) {

            //
            // Populate the caller's attribute context buffer using the
            // attribute record header.  If a non-zero VCN header was
            // requested, only populate the fields guaranteed to be valid
            // (FRS number, FRS offset, type code, form code).
            //

            AttributeContext->FileRecordSegment = FileRecordNumber;
            AttributeContext->FileRecordOffset = (ULONG)PtrOffset(Frs, Header);
            AttributeContext->TypeCode = Header->TypeCode;
            AttributeContext->Flags = 0;
            if (Header->FormCode == RESIDENT_FORM) {
                SET_FLAGS(AttributeContext->Flags, NTFS_ATTRIBUTE_RESIDENT);
                AttributeContext->Size = Header->Form.Resident.ValueLength;

            } else if (Vcn == 0) {
                AttributeContext->Size = Header->Form.Nonresident.FileSize;

                //
                // Non-resident attributes can be compressed.  The lowest byte
                // of the attribute's flags field indicates the compression
                // algorithm.  The algorithm is the value stored in this low
                // byte.  Therefore a value of 0xFF is invalid.
                //

                CompressionFormat = (Header->Flags & NTFS_ATTRIBUTE_COMPRESSED);

                ASSERT(CompressionFormat != NTFS_ATTRIBUTE_COMPRESSED);

                if (CompressionFormat != 0) {
                    SET_FLAGS(AttributeContext->Flags, CompressionFormat);

                    //
                    // The unit of compression is a power of two times the
                    // size of a cluster.  The valid units include 2^n where
                    // n = {1, 2, 3, 4, 5}.
                    //

                    ASSERT((Header->Form.Nonresident.CompressionUnit > 0) &&
                           (Header->Form.Nonresident.CompressionUnit <= 5));

                    CompressionUnit = BytesPerCluster(&VolumeContext->Bpb);
                    for (Id = 0; Id < Header->Form.Nonresident.CompressionUnit;
                         Id += 1) {

                        CompressionUnit *= 2;
                    }

                    AttributeContext->CompressionUnit = CompressionUnit;
                }
            }

            return STATUS_SUCCESS;
        }

        //
        // Check to see if the current attribute describes an attribute list.
        // If so, save a pointer to the list.  In the event we do not find the
        // desired attribute in the local File Record Segment, we can recurse
        // on non-resident attributes.
        //

        if (Header->TypeCode == $ATTRIBUTE_LIST) {
            AttributeList.FileRecordSegment = FileRecordNumber;
            AttributeList.FileRecordOffset= (ULONG)PtrOffset(Frs, Header);
            AttributeList.TypeCode = Header->TypeCode;
            AttributeList.Flags = 0;
            if (Header->FormCode == RESIDENT_FORM) {
                SET_FLAGS(AttributeList.Flags, NTFS_ATTRIBUTE_RESIDENT);
                AttributeList.Size = Header->Form.Resident.ValueLength;

            } else {
                AttributeList.Size = Header->Form.Nonresident.FileSize;

                //
                // Non-resident attributes can be compressed.  The lowest byte
                // of the attribute's flags field indicates the compression
                // algorithm.  The algorithm is the value stored in this low
                // byte + 1.  Therefore a value of 0xFF is invalid.
                //

                CompressionFormat = (Header->Flags & NTFS_ATTRIBUTE_COMPRESSED);

                ASSERT(CompressionFormat != NTFS_ATTRIBUTE_COMPRESSED);

                if (CompressionFormat != 0) {
                    SET_FLAGS(AttributeList.Flags, CompressionFormat);

                    //
                    // The unit of compression is a power of two times the
                    // size of a cluster.  The valid units include 2^n where
                    // n = {1, 2, 3, 4, 5}.
                    //

                    ASSERT((Header->Form.Nonresident.CompressionUnit > 0) &&
                           (Header->Form.Nonresident.CompressionUnit <= 5));

                    CompressionUnit = BytesPerCluster(&VolumeContext->Bpb);
                    for (Id = 0; Id < Header->Form.Nonresident.CompressionUnit;
                         Id += 1) {

                        CompressionUnit *= 2;
                    }

                    AttributeList.CompressionUnit = CompressionUnit;
                }
            }
        }

        PreviousHeader = Header;
        Header = NtfsGetNextRecord(Header);

        //
        // A valid attribute record header should have a non-zero length.
        //

        if (Header == PreviousHeader) {
            return STATUS_FILE_CORRUPT_ERROR;
        }

    } while (TRUE);

    //
    // If the attribute was not found resident in the File Record Segment, walk
    // the attribute list searching for a match.
    //

    if (AttributeList.TypeCode == $ATTRIBUTE_LIST) {
        for (ListOffset = 0, FoundAttribute = FALSE;
             ListOffset < AttributeList.Size;
             ListOffset += AttributeListEntry.RecordLength) {

            Status = NtfspReadAttribute(VolumeContext,
                                        &AttributeList,
                                        ListOffset,
                                        sizeof(ATTRIBUTE_LIST_ENTRY),
                                        &AttributeListEntry);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            //
            // A valid attribute list entry should have a non-zero length.
            //

            if (AttributeListEntry.RecordLength == 0) {
                return STATUS_FILE_CORRUPT_ERROR;
            }

            //
            // An attribute list is sorted by TYPE_CODE.  Terminate the
            // search when appropriate.
            //

            if (TypeCode < AttributeListEntry.AttributeTypeCode) {
                break;
            }

            //
            // Check to see if we found the requested attribute. For $DATA
            // streams ensure that the name is a match.
            //

            if ((AttributeListEntry.AttributeTypeCode == TypeCode) &&
                ((TypeCode != $DATA) ||
                 (AttributeName->Length / sizeof(WCHAR) == AttributeListEntry.AttributeNameLength))) {

                //
                // Read and compare attribute name for named attributes of type $DATA.
                //

                if ((TypeCode == $DATA) &&
                    (AttributeName->Length > 0)) {

                    //
                    // Verify that attribute name is within the record.
                    //

                    if ((AttributeListEntry.AttributeNameOffset <
                         FIELD_OFFSET(ATTRIBUTE_LIST_ENTRY, AttributeName)) ||
                        ((AttributeListEntry.AttributeNameOffset +
                          (AttributeListEntry.AttributeNameLength * sizeof(WCHAR)) >
                          AttributeListEntry.RecordLength))) {

                        return STATUS_FILE_CORRUPT_ERROR;
                    }

                    Status = NtfspReadAttribute(VolumeContext,
                                                &AttributeList,
                                                ListOffset + AttributeListEntry.AttributeNameOffset,
                                                AttributeName->Length,
                                                AttributeNameBuffer);

                    if (!NT_SUCCESS(Status)) {
                        return Status;
                    }

                    if (_wcsnicmp(AttributeName->Buffer,
                                  AttributeNameBuffer,
                                  AttributeListEntry.AttributeNameLength) != 0) {

                        continue;
                    }
                }

                //
                // Ensure that if the attribute spans multiple segments, the
                // record containing the specified VNC is read and returned.
                //
                // Always set FoundAttribute to TRUE.  Most attributes will
                // not span multiple segments.  Ensure that FoundAttribute
                // and ListFileRecordNumber are properly set in this scenario.
                //

                FoundAttribute = TRUE;
                if (Vcn < AttributeListEntry.LowestVcn) {
                    break;
                }

                FileReferenceToLargeInt(AttributeListEntry.SegmentReference,
                                        &ListFileRecordNumber);
            }
        }

        //
        // When the specified attribute was found, read and return the
        // attribute record from the specified file record segment.
        //

        if (FoundAttribute != FALSE) {
            Status = NtfspLookupAttributeVcn(VolumeContext,
                                             ListFileRecordNumber,
                                             TypeCode,
                                             AttributeName,
                                             Vcn,
                                             AttributeContext);

            return Status;
        }
    }

    return STATUS_NOT_FOUND;
}

NTSTATUS
NtfspReadWriteAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    Depending on the value of ReadOperation, this routine will either
    read from or write to the specified attribute and the specified offset.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    AttributeContext - Attribute that should be read/written.

    Offset - The offset within the attribute value the i/o operation should
        start.

    Length - The number of bytes that should be read/written.

    Buffer - I/O buffer.

    ReadOperation - Boolean value indicating whether to read from the
        specified attribute or write back to the attribute.


Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_INVALID_PARAMETER if request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    DEVICE_INFORMATION DeviceInformation;
    PVOID Frs;
    ULONG OriginalIoAccessFlags;
    NTSTATUS Status;

    OriginalIoAccessFlags = 0; // Satisfy compiler.

    if ((VBO)AttributeContext->Size < Offset + Length) {
        return STATUS_END_OF_FILE;
    }

    //
    // Get the File Record Segment for the requested attribute.
    //

    Status = NtfspGetFileRecordSegment(VolumeContext,
                                       AttributeContext->FileRecordSegment,
                                       &Frs);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // In attempt to minimize the number of physical reads performed while
    // reading metadata, attempt to cache all non-data attributes.
    //

    if ((CHECK_FLAG(AttributeContext->Flags, NTFS_ATTRIBUTE_CACHE_READ)) &&
        (ReadOperation != FALSE)) {

        Status = BlDeviceGetInformation(VolumeContext->DeviceId,
                                        &DeviceInformation);

        ASSERT(Status == STATUS_SUCCESS);
        ASSERT((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
               (DeviceInformation.DeviceType == DEVICE_PARTITION) ||
               (DeviceInformation.DeviceType == DEVICE_PARTITION_EX));

        OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
        SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                  BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

        Status = BlDeviceSetInformation(VolumeContext->DeviceId,
                                        &DeviceInformation);

        ASSERT(Status == STATUS_SUCCESS);
    }

    //
    // Call the helper routine that will actually perform the i/o
    // operation.
    //

    Status = NtfspReadWriteAttributeEx(VolumeContext,
                                       AttributeContext,
                                       Frs,
                                       Offset,
                                       Length,
                                       Buffer,
                                       ReadOperation);

    if ((CHECK_FLAG(AttributeContext->Flags, NTFS_ATTRIBUTE_CACHE_READ)) &&
        (ReadOperation != FALSE)) {

        //
        // Restore the original I/O access flags.  BlDeviceGetInformation
        // should be called again to preserve the seek pointer after the
        // attribute read.  However, this module only uses BlDeviceReadAtOffset
        // and no caller should assume the value of the seek position on
        // return.
        //

        DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
        BlDeviceSetInformation(VolumeContext->DeviceId, &DeviceInformation);
    }

    return Status;
}

NTSTATUS
NtfspReadWriteAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    Depending on the value of ReadOperation, this routine will either
    read from the specified attribute or write back to it.  The attribute
    can either be resident or non-resident.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    AttributeContext - Attribute that should be read / written.

    FileRecordSegment - Pointer to the File Record Segment that the requested
        attribute lives.

    Offset - The offset within the attribute value to start the i/o operation.

    Length - The number of bytes to perform i/o operation.

    Buffer - I/O transfer buffer.

    ReadOperation - Boolean value indicating whether to read from the
        specified attribute or write back.


Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_INVALID_PARAMETER if request walks off attribute value.

    A non-resident attribute can also have the following error status codes:
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.


--*/

{

    NTSTATUS Status;

    //
    // Fork request based on whether the attribute is resident or not.
    //

    if (CHECK_FLAG(AttributeContext->Flags, NTFS_ATTRIBUTE_RESIDENT)) {
        Status = NtfspReadWriteResidentAttributeEx(VolumeContext,
                                                   AttributeContext,
                                                   FileRecordSegment,
                                                   Offset,
                                                   Length,
                                                   Buffer,
                                                   ReadOperation);

    } else {
        Status = NtfspReadWriteNonResidentAttributeEx(VolumeContext,
                                                      AttributeContext,
                                                      FileRecordSegment,
                                                      Offset,
                                                      Length,
                                                      Buffer,
                                                      ReadOperation);
    }

    return Status;
}

NTSTATUS
NtfspReadWriteResidentAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout PVOID Buffer,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    This routine will read the value of a resident attribute.

    Note: This routine does not support writing to a resident attribute.
          In order to do so, we would need to pass around cached copies of
          the FRS, and mark them dirty when written to.  This would require
          changing a few functions.  This may not be difficult, but the
          old loader code did not support this.  So we currently have parity
          with it.
    TODO: write support for resident attributes.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    AttributeContext - Attribute that should be read / written, including the
        offset of the resident attribute within the FRS.

    FileRecordSegment - Pointer to the File Record Segment that the requested
        attribute lives.

    Offset - The offset within the attribute value the i/o operation should
        start.

    Length - The number of bytes on which the i/o operation should be
        performed.

    Buffer - I/O transfer buffer.

    ReadOperation - Boolean value indicating whether to read from the
        specified attribute or write back.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_NOT_IMPLEMENTED for any write request.
    STATUS_INVALID_PARAMETER if the i/o operation will go off the end of the
        attribute value.

--*/

{

    PATTRIBUTE_RECORD_HEADER AttributeHeader;

    UNREFERENCED_PARAMETER( VolumeContext);

    //
    // We do not currently support writing to a resident attribute.  Add this
    // support later.  To do so, we would need to mark the FRS as dirty and
    // write it to disk when purging from cache.
    //

    if (ReadOperation == FALSE) {
        return STATUS_NOT_IMPLEMENTED;
    }

    //
    // Ensure the operation does not walk off the end of the attribute value.
    //

    if (AttributeContext->Size < (ULONGLONG)Offset + Length) {
        return STATUS_INVALID_PARAMETER;
    }

    AttributeHeader = Add2Ptr(FileRecordSegment,
                              AttributeContext->FileRecordOffset);

    //
    // Copy the requested data into the caller's buffer.
    //

    RtlCopyMemory(Buffer,
                  Add2Ptr(NtfsGetValue(AttributeHeader), Offset),
                  Length);

    return STATUS_SUCCESS;
}

NTSTATUS
NtfspReadWriteNonResidentAttributeEx (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    Depending on the value of ReadOperation, this routine will either read
    from or write to a non-resident attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    AttributeContext - Attribute that should be read / written.

    FileRecordSegment - Pointer to the File Record Segment that the requested
        attribute lives.

    Offset - The offset within the attribute value to start i/o operation.

    Length - The number of bytes for the i/o operation.

    Buffer - I/O transfer buffer.

    ReadOperation - Boolean value indicating whether to read from the
        specified attribute or write back.


Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_INVALID_PARAMETER if read request walks off attribute value.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_NOT_SUPPORTED if a write request was made to a cached attribute.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.


--*/

{

    ULONG Bytes;
    ULONG CompressionFormat;
    LBO Lbo;
    ULONG OperationLength;
    VBO RunLength;
    BOOLEAN SparseRun;
    NTSTATUS Status;
    ULONG TotalBytes;

    //
    // Non-resident attributes are scattered over logical runs on the
    // disk.  Each non-resident attribute contains a mapping pair array
    // which maps virtual offsets to their logical location on disk.
    // To read the attribute, each logical run must be walked and the
    // appropriate logical disk sectors must be read into memory.
    //

    CompressionFormat = AttributeContext->Flags & NTFS_ATTRIBUTE_COMPRESSED;
    if (CompressionFormat != 0) {
        Status = NtfspReadWriteCompressedAttribute(VolumeContext,
                                                   AttributeContext,
                                                   FileRecordSegment,
                                                   Offset,
                                                   Length,
                                                   Buffer,
                                                   ReadOperation);

        return Status;
    }

    //
    // Continue to read the requested attribute, mapping the relative attribute
    // offset to the offset in the NTFS volume.
    //

    TotalBytes = 0;
    while (TotalBytes < Length) {

        //
        // Determing mapping of the relative offset to the actual location in
        // the volume.
        //

        Status = NtfspDetermineLbo(VolumeContext,
                                   AttributeContext,
                                   FileRecordSegment,
                                   Offset + TotalBytes,
                                   &Lbo,
                                   &SparseRun,
                                   &RunLength);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // A sparse or compressed run is interpretted as zeroes.  Writing to
        // such a run is invalid and requires OS support to grow the attribute.
        // Physically read or write non-sparse runs from the underlying volume.
        //
        // N.B. Operation length is always 32 bits or less by definition of the
        //      boot library.  The file library only supports 32 bit i/o
        //      lengths.
        //

        ASSERT(min(RunLength, Length - TotalBytes) < 0x100000000);

        OperationLength = (ULONG)min(RunLength, Length - TotalBytes);
        if (ReadOperation == FALSE) {
            if (SparseRun != FALSE) {
                Status = STATUS_NOT_SUPPORTED;
                Bytes = 0;

            } else {
                Status = NtfspDeviceWriteAtOffset(VolumeContext,
                                                  VolumeContext->DeviceId,
                                                  Lbo,
                                                  OperationLength,
                                                  Add2Ptr(Buffer, TotalBytes),
                                                  &Bytes);
            }

        } else {
            if (SparseRun != FALSE) {
                RtlZeroMemory(Add2Ptr(Buffer, TotalBytes), OperationLength);
                Bytes = OperationLength;

            } else {
                Status = NtfspDeviceReadAtOffset(VolumeContext,
                                                 VolumeContext->DeviceId,
                                                 Lbo,
                                                 OperationLength,
                                                 Add2Ptr(Buffer, TotalBytes),
                                                 &Bytes);
            }
        }

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        TotalBytes += Bytes;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
NtfspReadWriteCompressedAttribute (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Offset,
    __in ULONG Length,
    __inout_bcount(Length) PVOID Buffer,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    This routine will read the specified virtual range in a compressed
    non-resident attribute.

Arguments:

    VolumeContext - Pointer to filesystem contextural information for the NTFS
        volume.

    AttributeContext - Attribute that should be read / written.

    FileRecordSegment - Pointer to the File Record Segment that the requested
        attribute lives.

    Offset - The offset within the attribute value to start i/o operation.

    Length - The number of bytes for the i/o operation.

    Buffer - I/O transfer buffer.

    ReadOperation - Boolean value indicating whether to read from the
        specified attribute or write back.

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_INVALID_PARAMETER if read request walks off attribute value.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_NOT_SUPPORTED if a write request was made to a cached attribute.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.


--*/

{

    ULONG Bytes;
    PVOID CompressedBuffer;
    ULONG CompressionFormat;
    ULONG CompressionUnit;
    VBO CompressedBufferOffset;
    PVOID DecompressedBuffer;
    ULONG DeltaOffset;
    LBO Lbo;
    VBO RunLength;
    BOOLEAN SparseRun;
    NTSTATUS Status;
    ULONG TotalBytes;
    PCHAR TransferBuffer;
    ULONG UnitOffset;

    //
    // Writes to a compressed file are not supported.
    //

    if (ReadOperation == FALSE) {
        return STATUS_NOT_SUPPORTED;
    }

    Status = STATUS_SUCCESS;
    CompressedBuffer = NULL;
    DecompressedBuffer = NULL;

    //
    // Non-resident attributes are scattered over logical runs on the
    // disk.  Each non-resident attribute contains a mapping pair array
    // which maps virtual offsets to their logical location on disk.
    // To read the attribute, each logical run must be walked and the
    // appropriate logical disk sectors must be read into memory.
    //

    //
    // TODO: PERF: Cache decompressed buffers.
    //

    CompressionFormat = AttributeContext->Flags & NTFS_ATTRIBUTE_COMPRESSED;

    //
    // The old loader assumed the compression format was always
    // COMPERSSION_FORMAT_LZNT1.  Add an assert to see if this is true.
    //

    ASSERT((CompressionFormat + 1) == COMPRESSION_FORMAT_LZNT1);

    //
    // Allocate a single buffer to store every compressed buffer unit.
    // Since the caller isn't guarenteed to make a read/write request
    // that is at least as large as a compression unit, a decompression
    // buffer is additionally required.
    //

    CompressionUnit = AttributeContext->CompressionUnit;
    CompressedBuffer = BlMmAllocateHeap(CompressionUnit);
    if (CompressedBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspReadCompressedAttributeEnd;
    }

    DecompressedBuffer = BlMmAllocateHeap(CompressionUnit);
    if (DecompressedBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspReadCompressedAttributeEnd;
    }

    //
    // Continue to read the requested attribute one compression unit at a
    // time.  When each compressed unit has been read, decompress its data.
    //

    TotalBytes = 0;
    while (TotalBytes < Length) {

        //
        // Attempt to read the next compression unit.
        // N.B. This may span multiple logical runs.
        //

        CompressedBufferOffset = ALIGN_RANGE_DOWN(Offset, CompressionUnit);
        for (UnitOffset = 0;
             UnitOffset < CompressionUnit;
             UnitOffset += Bytes) {

            //
            // Determing mapping of the relative offset to the actual location
            // in the volume.  A compressed buffer is terminated with a sparse
            // run in the vbo to lbo mapping.
            //

            Status = NtfspDetermineLbo(VolumeContext,
                                       AttributeContext,
                                       FileRecordSegment,
                                       CompressedBufferOffset + UnitOffset,
                                       &Lbo,
                                       &SparseRun,
                                       &RunLength);

            if (!NT_SUCCESS(Status)) {
                goto NtfspReadCompressedAttributeEnd;
            }

            if (SparseRun != FALSE) {
                break;
            }

            //
            // Read the current logical run.  Be careful not to read past
            // the current compression unit.
            //
            //
            // N.B. The length of a compression buffer is always 32 bits or
            //      less due to the definition of RtlDecompressBuffer.
            //

            ASSERT(min(RunLength, CompressionUnit - UnitOffset) < 0x100000000);

            Bytes = (ULONG)min(RunLength, CompressionUnit - UnitOffset);
            Status = NtfspDeviceReadAtOffset(VolumeContext,
                                             VolumeContext->DeviceId,
                                             Lbo,
                                             Bytes,
                                             Add2Ptr(CompressedBuffer, UnitOffset),
                                             &Bytes);

            if (!NT_SUCCESS(Status)) {
                goto NtfspReadCompressedAttributeEnd;
            }
        }

        //
        // The entire compression buffer has been read into memory.
        // There are three possibilities:
        //
        // 1. There isn't any disk information for the compression unit.
        //    In this case, the entire decompressed buffer is zero.
        //
        // 2. The compressed buffer is exactly one compression unit in
        //    size.  This indicates that the contents are not actually
        //    compressed and can be treated as uncompressed data.
        //
        // 3. The compressed data is between 0 and CompressionUnit bytes
        //    in length.  In this case, the buffer must be decompressed
        //

        if (UnitOffset == 0) {
            TransferBuffer = CompressedBuffer;
            Bytes = 0;

        } else if (UnitOffset == CompressionUnit) {
            TransferBuffer = CompressedBuffer;
            Bytes = CompressionUnit;

        } else {
            Status = RtlDecompressBuffer((USHORT)(CompressionFormat + 1),
                                         DecompressedBuffer,
                                         CompressionUnit,
                                         CompressedBuffer,
                                         UnitOffset,
                                         &Bytes);

            if (!NT_SUCCESS(Status)) {
                goto NtfspReadCompressedAttributeEnd;
            }

            TransferBuffer = DecompressedBuffer;
        }

        //
        // Zero all slack bytes at the end of the compressed buffer.
        //

        RtlZeroMemory(Add2Ptr(TransferBuffer, Bytes), CompressionUnit - Bytes);

        //
        // Copy data from the decompressed buffer to the caller's buffer.
        // Take care not to overrun or underrun the caller's buffer.
        //

        DeltaOffset = (ULONG)(Offset - CompressedBufferOffset);
        Bytes = min(Length - TotalBytes, CompressionUnit - DeltaOffset);
        RtlCopyMemory(Add2Ptr(Buffer, TotalBytes),
                      Add2Ptr(TransferBuffer, DeltaOffset),
                      Bytes);

        //
        // Update internal state pointers.
        //

        TotalBytes += Bytes;
        Offset += Bytes;
    }

NtfspReadCompressedAttributeEnd:

    if (CompressedBuffer != NULL) {
        BlMmFreeHeap(CompressedBuffer);
    }

    if (DecompressedBuffer != NULL) {
        BlMmFreeHeap(DecompressedBuffer);
    }

    return Status;
}

NTSTATUS
NtfspDetermineLbo (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in PNTFS_ATTRIBUTE_CONTEXT AttributeContext,
    __in PVOID FileRecordSegment,
    __in VBO Vbo,
    __out PLBO Lbo,
    __out PBOOLEAN SparseRun,
    __out PVBO RunLength
    )

/*++

Routine Description:

    This routine will decode the retrieval information stored after a
    Non-Resident attribute header in order to determine the LBO for the
    relative offset provided.  After each non-resident attribute header, there
    exists a mapping pairs array which provides a mapping from VBO to LBO for
    each cluster run of the attribute.

    The mapping could (probably should) be cached to avoid repeated processing
    of the mapping pairs array.

Arguments:

    VolumeContext - Supplies a NTFS volume.

    AttributeContext - Supplies the description of the attribute to read.

    FileRecordSegment - Supplies the file record segment containing the
        attribute header.

    Vbo - Supplies a byte offset within the attribute.

    Lbo - Supplies a pointer to a variable that receives the byte offset
        relative to the NTFS volume containing the data at the specified vbo.

    SparseRun - Supplies a pointer to a variable that receives a boolean
        indicating if the supplied offset is part of a sparse run.  In this case
        the returned LBO is the last LBO.

    RunLength - Supplies a pointer to a variable that receives the number of
        bytes (after the vbo) that are part of the same logical run.

Return Value:

    STATUS_SUCCESS when operation is successful.

    STATUS_INVALID_PARAMETER otherwise.

--*/

{

    PVOID AlternateFrs = NULL;
    ULONGLONG AlternateFrsNumber;
    NTFS_ATTRIBUTE_CONTEXT AlternateContext;
    PATTRIBUTE_RECORD_HEADER AttributeHeader;
    UNICODE_STRING AttributeName;
    PVOID BaseFrs;
    ULONGLONG BaseFrsNumber;
    ULONG BytesPerCluster;
    PCHAR Ch;
    LONGLONG Change;
    LBO CurrentLcn;
    VBO CurrentVcn;
    PFILE_RECORD_SEGMENT_HEADER FrsHeader;
    VBO HighestVbo;
    ULONG LboBytes;
    VBO LowestVbo;
    VBO NextVcn;
    NTSTATUS Status;
    ULONG VboBytes;

    BytesPerCluster = BytesPerCluster(&VolumeContext->Bpb);

    //
    // Get a pointer to the Attribute's Header.  In the same FRS, typically
    // directly after the header will be the Mapping Pairs Array which maps
    // each logical cluster used by the attribute to it's relative offset.
    //

    AttributeHeader = Add2Ptr(FileRecordSegment,
                              AttributeContext->FileRecordOffset);

    ASSERT(AttributeHeader->FormCode == NONRESIDENT_FORM);

    //
    // An attribute may span multiple segments.  When this is the case, there
    // exists multiple attribute record headers and each describe the virtual
    // range described their Mapping Pairs Array.  If the provided
    // attribute record does not map the requested VBO, obtain a pointer to
    // the attribute record that does.
    //

    LowestVbo = AttributeHeader->Form.Nonresident.LowestVcn * BytesPerCluster;
    HighestVbo = ((AttributeHeader->Form.Nonresident.HighestVcn + 1) *
                  BytesPerCluster);

    if ((Vbo < LowestVbo) || (HighestVbo <= Vbo)) {

        //
        // Lookup the requested VBO.  In order to do so, the appropriate
        // attribute record must be found.  This requires looking up the
        // attribute on the base file record segment (which contains the
        // attribute list for the file).
        //
        // Do not read the base file record segment if it is the FRS provided
        // by the caller.  In this scenario, use the caller's buffer.
        //

        FrsHeader = ((PFILE_RECORD_SEGMENT_HEADER)FileRecordSegment);
        if (*((PULONGLONG)&FrsHeader->BaseFileRecordSegment) != 0) {
            FileReferenceToLargeInt(FrsHeader->BaseFileRecordSegment,
                                    &BaseFrsNumber);

            Status = NtfspGetFileRecordSegment(VolumeContext,
                                               BaseFrsNumber,
                                               &BaseFrs);

            if (!NT_SUCCESS(Status)) {
                goto NtfspDetermineLboEnd;
            }

        } else {
            BaseFrs = FileRecordSegment;
            BaseFrsNumber = AttributeContext->FileRecordSegment;
        }

        //
        // Get attribute name, if any.
        //

        AttributeName = NtfsEmptyName;
        if (AttributeHeader->NameLength > 0) {
            AttributeName.Buffer = 
                (PWSTR)Add2Ptr(AttributeHeader, AttributeHeader->NameOffset);

            AttributeName.Length = AttributeHeader->NameLength * sizeof(WCHAR);
            AttributeName.MaximumLength = AttributeName.Length;
        }

        //
        // Lookup the attribute record which contains the mapping pairs
        // information for the requested VBO.
        //

        Status = NtfspLookupAttributeVcnEx(VolumeContext,
                                           BaseFrs,
                                           BaseFrsNumber,
                                           AttributeHeader->TypeCode,
                                           &AttributeName,
                                           Vbo / BytesPerCluster,
                                           &AlternateContext);

        if (!NT_SUCCESS(Status)) {
            goto NtfspDetermineLboEnd;
        }

        //
        // Read in the File Record Segment containing the desired attribute
        // record.  This must succeed since it was read when populating
        // AlternateContext.
        //

        if (AlternateContext.FileRecordSegment != BaseFrsNumber) {
            AlternateFrsNumber = AlternateContext.FileRecordSegment;
            Status = NtfspGetFileRecordSegment(VolumeContext,
                                               AlternateFrsNumber,
                                               &AlternateFrs);

            ASSERT(Status == STATUS_SUCCESS);

        } else {
            AlternateFrs = BaseFrs;
        }

        //
        // Save a pointer to the attribute header.
        //

        AttributeHeader = Add2Ptr(AlternateFrs,
                                  AlternateContext.FileRecordOffset);
    }

    Ch = Add2Ptr(AttributeHeader,
                 AttributeHeader->Form.Nonresident.MappingPairsOffset);

    NextVcn = AttributeHeader->Form.Nonresident.LowestVcn;

    //
    // The mapping pairs array is a zero termianted byte stream.  The contents
    // of the byte stream are compressed and need to be interpreted as follows:
    //
    //     count byte = v + (l * 16)
    //
    //     where v = number of changed low-order Vcn bytes.
    //           l = number of changed low-order Lcn bytes.
    //
    // Following the count byte, the next v bytes are to be interpreted as the
    // relative change in Vcn bytes.  The next l bytes are interpreted as the
    // relative change in Lcn bytes.  The values need to be sign extented, as
    // the resulting change may be negative (in the case of the Lcn).
    //

    CurrentLcn = 0;
    while (*Ch != 0) {
        CurrentVcn = NextVcn;

        //
        // Extract the Vcn and Lcn change-bytes. These values are limited by
        // the size of a cluster (8 bytes).
        //

        VboBytes = *Ch & 0x0f;
        LboBytes = *Ch++ >> 4;
        if ((VboBytes > sizeof(Change)) || (LboBytes > sizeof(Change))) {
            Status = STATUS_INVALID_PARAMETER;
            goto NtfspDetermineLboEnd;
        }

        //
        // Determine the change in the Vcn.
        //

        //
        // Check to see if the most significant bit is set.  Ntfs is little
        // endian, so look at the last byte in the VboBytes.  It does not
        // make sense for a Vcn change to be negative, so fail in this case.
        //

        if (*((PCHAR)Add2Ptr(Ch, VboBytes - 1)) & 0x80) {
            Status = STATUS_INVALID_PARAMETER;
            goto NtfspDetermineLboEnd;
        }

        Change = 0;
        RtlMoveMemory(&Change, Ch, VboBytes);
        NextVcn += Change;

        Ch = Add2Ptr(Ch, VboBytes);

        //
        // There isn't necessarily a change in the Lcn.  The file could be
        // sparse or compressed, and in both cases this could lead to only a
        // Vcn change.
        //

        if (LboBytes != 0) {

            //
            // Determine the change in Lcn.
            // N.B. A LCN change can be negative.  Check the most significant
            //      bit to determine if the change is negative.
            //

            if (*((PCHAR)Add2Ptr(Ch, LboBytes - 1)) & 0x80) {
                Change = -1;

            } else {
                Change = 0;
            }

            RtlMoveMemory(&Change, Ch, LboBytes);
            CurrentLcn += Change;
            Ch = Add2Ptr(Ch, LboBytes);
        }

        //
        // Check to see if the requested offset has been processed.
        //

        if (Vbo < (NextVcn * BytesPerCluster)) {
            if (LboBytes == 0) {
                *SparseRun = TRUE;

            } else {
                *SparseRun = FALSE;
            }

            *Lbo = CurrentLcn * BytesPerCluster;
            *Lbo += (Vbo - (CurrentVcn * BytesPerCluster));
            *RunLength = (NextVcn * BytesPerCluster) - Vbo;
            Status = STATUS_SUCCESS;
            goto NtfspDetermineLboEnd;
        }
    }

    Status = STATUS_INVALID_PARAMETER;

NtfspDetermineLboEnd:
    return Status;
}

NTSTATUS
NtfspDecodeUsa (
    __in PMULTI_SECTOR_HEADER MultiSectorHeader,
    __inout_bcount(Length) PVOID Buffer,
    __in ULONG Length,
    __in VBO Offset
    )

/*++

Routine Description:

    File Records and other Allocations that may span multiple sectors contain
    a Multi-Sector header and Update Sequence Array to provide detection of
    incomplete multi-sector transfers for devices.  The last two bytes of each
    Stride (512 bytes) are set to a constant (Update Sequence Array[0]).  When
    reading the entire record from disk, if the protected 2 bytes from the end
    of each stride do not match the constant, there was an error writing the
    entire record to disk.

    This routine decodes a window of a multi-sector record.  If the buffer
    contains any of the protected USHORTS at the end of a stride, then they
    will be decoded using the Update Sequence Array.

Arguments:

    MultiSectorHeader - On-disk Multi Sector Header for the record being
        decoded.

    Buffer - Supplies the buffer to be decoded.

    Length - Length of the Buffer.

    Offset - The Offset in the record (VBO between Buffer and the MultiSector
        Header).

Return Value:

    STATUS_SUCCESS when operation is successful.
    STATUS_INVALID_PARAMETER if part of the buffer is after the last stride.
    STATUS_UNSUCCESSFUL if the Buffer is not properly encode (error writing
        sector to disk).

--*/

{

    ULONG Id;
    PUSHORT ProtectedUshort;
    VBO ProtectedUshortOffset;
    PUSHORT UpdateSequenceArray;
    ULONG UsaSize;               // Number of values in USA.

    //
    // Initialize local variables.
    //

    UpdateSequenceArray = Add2Ptr(MultiSectorHeader,
                                  MultiSectorHeader->UpdateSequenceArrayOffset);
    UsaSize = MultiSectorHeader->UpdateSequenceArraySize;

    //
    // For every entry in the usa, we need to computer the address of the
    // protected ushort.  The protected ushort must be equal to the current
    // sequence number (i.e., the number at UsaOffset[0]).  If the two are
    // equal, then replace the protected ushort with the saved ushort in the
    // usa.
    //

    for (Id = 1; Id < UsaSize; Id += 1) {
        ProtectedUshortOffset = (SEQUENCE_NUMBER_STRIDE * Id) - sizeof(USHORT);

        //
        // If the current stride falls within the Buffer's window, decode the
        // protected ushort.
        //

        MARK_SAFE_ADDITION(ProtectedUshortOffset, (VBO)sizeof(USHORT));
        if ((Offset <= ProtectedUshortOffset) &&
            (ProtectedUshortOffset + (VBO)sizeof(USHORT) <= Offset + Length)) {

            ProtectedUshort = Add2Ptr(Buffer, ProtectedUshortOffset - Offset);
            if (*ProtectedUshort != UpdateSequenceArray[0]) {
                return STATUS_UNSUCCESSFUL;
            }

            *ProtectedUshort = UpdateSequenceArray[Id];

        }
    }

    //
    // Return successfully to the caller.  The Buffer has been decoded in
    // place.
    //

    return STATUS_SUCCESS;
}

//
// Ntfs Device Table Entry Routines.
//

NTSTATUS
NtfspDeviceTablePurgeEntry (
    __in PNTFS_DEVICE_CONTEXT NtfsContext,
    __in ULONG Index
    )

/*++

Routine Description:

    This routine destroys the file record segment hash table for the device.

Arguments:

    NtfsContext - Supplies and entry in the Ntfs Device Table to purge.

    Index - Supplies an index in the device table that points to NtfsContext.

Return Value:

    STATUS_SUCCESS when successful.
    Failing error status is possible depending on success of destroying a
        file entry.

--*/

{

    HASH_DATA HashData;
    HASH_KEY HashKey;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Index);

    //
    // Purge the file record cache.
    //

    Status = STATUS_SUCCESS;
    while (TRUE) {
        Status = BlHtGetNext(NtfsContext->FileRecordHashTableId,
                             NULL,
                             &HashKey,
                             &HashData);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        BlHtDelete(NtfsContext->FileRecordHashTableId, &HashKey);

        //
        // Only the hash key value needs to be freed. The hash data value is
        // contiguous on the end of the key value.
        //

        BlMmFreeHeap(HashKey.Value);
    }

    return Status;
}

NTSTATUS
NtfspDeviceTableDestroyEntry (
    __in PNTFS_DEVICE_CONTEXT NtfsContext,
    __in ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the Ntfs Device Table.  This includes freeing the
    memory allocated for the entry as well as zeroing the entry in the device
    table.

Arguments:

    NtfsContext - Entry in the Ntfs Device Table to destroy.

    Index - Index in the device table that points to NtfsContext.

Return Value:

    STATUS_SUCCESS when successful.
    Failing error status is possible depending on success of destroying a
        file entry.

--*/

{

    HASH_DATA HashData;
    HASH_KEY HashKey;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Free the entry's memory.
    //

    while (TRUE) {
        Status = BlHtGetNext(NtfsContext->FileRecordHashTableId,
                             NULL,
                             &HashKey,
                             &HashData);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        BlHtDelete(NtfsContext->FileRecordHashTableId, &HashKey);

        //
        // Only the hash key value needs to be freed. The hash data value is
        // contiguous on the end of the key value.
        //

        BlMmFreeHeap(HashKey.Value);
    }

    BlHtDestroy(NtfsContext->FileRecordHashTableId);
    BlMmFreeHeap(NtfsContext);
    NtfsDeviceTable[Index] = NULL;
    return Status;
}

//
// Ntfs File Record Segment Routines.
//

NTSTATUS
NtfspReadAndDecodeFileRecord (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FrsNumber,
    __inout_bcount(BufferSize) PVOID Buffer,
    __in ULONG BufferSize
    )

/*++

Routine Description:

    This function will read a File Record Segment from a NTFS filesystem.  The
    file record segment specified is read into a pre-allocated buffer.

Arguments:

    VolumeContext - The NTFS mounted device the read operation is to be
        performed on.

    FrsNumber - The File Record Segment to read.  All File Records are
        referenced by number.  (The MFT is FR #0, MFT2 is FR #1, etc).

    Buffer - Buffer to copy the contents of the read.

    BufferSize - The size of the Buffer.

Return Value:

    STATUS_SUCCESS when successful.

    Other Error if Device Read operation fails (including):
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/


{

    DEVICE_INFORMATION DeviceInformation;
    VBO FrsOffset;
    LBO MftOffset;
    ULONG OriginalIoAccessFlags;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(BufferSize);

    ASSERT(BufferSize >= VolumeContext->BytesPerFileRecordSegment);

    __analysis_assume(BufferSize >= VolumeContext->BytesPerFileRecordSegment);

    Status = BlDeviceGetInformation(VolumeContext->DeviceId,
                                    &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION_EX));

    OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

    Status = BlDeviceSetInformation(VolumeContext->DeviceId,
                                    &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);

    //
    // Determine the byte offset to the File Record Segment requested.  The MFT
    // is a virtually contiguous collection of File Records.  Therefore, the
    // byte offset of a File Record is the VBO offset of the MFT plus the
    // offset of the File Record in the MFT.
    //
    // If the request is to read the MFT itself, read directory from disk.
    //

    if (FrsNumber == MASTER_FILE_TABLE_NUMBER) {
        MftOffset = VolumeContext->MftStartLcn *
                    BytesPerCluster(&VolumeContext->Bpb);

        //
        // Read the MFT FRS from the volume.
        //

        Status = NtfspDeviceReadAtOffset(
                     VolumeContext,
                     VolumeContext->DeviceId,
                     MftOffset,
                     VolumeContext->BytesPerFileRecordSegment,
                     Buffer,
                     NULL);

    } else {
        FrsOffset = FrsNumber * VolumeContext->BytesPerFileRecordSegment;
        Status = NtfspReadAttribute(VolumeContext,
                                    &VolumeContext->MftAttribute,
                                    FrsOffset,
                                    VolumeContext->BytesPerFileRecordSegment,
                                    Buffer);
    }

    DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
    BlDeviceSetInformation(VolumeContext->DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Decode the buffer to normal format.
    //

    Status = NtfspDecodeUsa(Buffer,
                            Buffer,
                            VolumeContext->BytesPerFileRecordSegment,
                            0);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
NtfspGetFileRecordSegment (
    __in PNTFS_DEVICE_CONTEXT VolumeContext,
    __in ULONGLONG FileRecordSegmentNumber,
    __deref_out PVOID *FileRecordSegment
    )

/*++

Routine Description:

    This function reads a File Record Segment and returns it to the caller.
    The FRS could be cached, in which case the cached entry will be used.
    Otherwise, the FRS will be read from the volume and then cached.

Arguments:

    VolumeContext - The NTFS Volume the File Record Segment should be read
        from.

    FileRecordSegmentNumber - The File Record Segment to read.

    FileRecordSegment - On return, contains a pointer to a buffer containing
        the requested File Record Segment.

Return Value:

    STATUS_SUCCCESS when operation is successful.
    STATUS_NO_MEMORY when unable to allocate memory for FRS or any other
        internal buffer.
    Other error if device read or insertion into cache table fails.

--*/

{

    PVOID Buffer;
    ULONG BytesPerFileRecordSegment;
    PVOID CachedFileRecordSegment;
    PHASH_DATA HashData;
    HASH_KEY HashKey;
    FILE_RECORD_SEGMENT_KEY LookupCacheKey;
    ULONG RequiredSize;
    NTSTATUS Status;
    PFILE_RECORD_SEGMENT_KEY StoreCacheKey;

    //
    // Look in the device's cache to see if the FRS has already been read.
    //

    LookupCacheKey.FrsNumber = FileRecordSegmentNumber;
    HashKey.Size = sizeof(FILE_RECORD_SEGMENT_KEY);
    HashKey.Value = &LookupCacheKey;
    HashKey.Flags = HASH_KEY_FLAGS_NONE;
    Status = BlHtLookup(VolumeContext->FileRecordHashTableId,
                        &HashKey,
                        &HashData);

    if (!NT_SUCCESS(Status)) {

        //
        // If the File Record Segment is not in the device's cache, then read
        // the entire segment and then cache the data.
        //

        BytesPerFileRecordSegment = VolumeContext->BytesPerFileRecordSegment;
        Status = RtlULongAdd(BytesPerFileRecordSegment,
                             (ULONG)sizeof(FILE_RECORD_SEGMENT_KEY),
                             &RequiredSize);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Buffer = BlMmAllocateHeap(RequiredSize);
        if (Buffer == NULL) {
            return STATUS_NO_MEMORY;
        }

        CachedFileRecordSegment = Add2Ptr(Buffer,
                                          sizeof(FILE_RECORD_SEGMENT_KEY));

        //
        // Read the File Record Segment from the device.
        //

        Status = NtfspReadAndDecodeFileRecord(VolumeContext,
                                              FileRecordSegmentNumber,
                                              CachedFileRecordSegment,
                                              BytesPerFileRecordSegment);

        if (!NT_SUCCESS(Status)) {
            BlMmFreeHeap(Buffer);
            return Status;
        }

        //
        // Initialize the hash key to store. The value needs to be allocated.
        //

        StoreCacheKey = (PFILE_RECORD_SEGMENT_KEY)Buffer;
        StoreCacheKey->FrsNumber = FileRecordSegmentNumber;
        HashKey.Value = StoreCacheKey;

        ASSERT(HashKey.Size == sizeof(FILE_RECORD_SEGMENT_KEY));
        ASSERT(HashKey.Flags == HASH_KEY_FLAGS_NONE);

        //
        // Insert the data into the hash table.
        //

        Status = BlHtStore(VolumeContext->FileRecordHashTableId,
                           &HashKey,
                           CachedFileRecordSegment,
                           BytesPerFileRecordSegment);

        if (!NT_SUCCESS(Status)) {
            BlMmFreeHeap(Buffer);
            return Status;
        }

    } else {

        ASSERT(HashData->Size == VolumeContext->BytesPerFileRecordSegment);

        CachedFileRecordSegment = HashData->Value;
    }

    //
    // Return the File Record Segment.
    //

    *FileRecordSegment = CachedFileRecordSegment;
    return STATUS_SUCCESS;
}

NTSTATUS
NtfspDeviceReadAtOffset (
    __in_opt PNTFS_DEVICE_CONTEXT VolumeContext,
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead
    )

/*++

Routine Description:

    Wrapper for device reads.  Provided a ByteOffset, this routine will
    verify the offset is contained within the NTFS volume prior to calling
    into the actual device library to perform the operation.

Arguments:

    VolumeContext - The NTFS mounted device the read operation is to be
        performed on.

    DeviceId - The identifier for the device to be read.

    ByteOffset - The offset on the device for the read operation to start.

    Size - The number of bytes to read.

    Buffer - Buffer to store contents of the read operation.

    BytesRead - Pointer to address to store the number of bytes actually read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL if the write is not wholly contained within the
        NTFS volume.

    STATUS_INVALID_PARAMETER if a requested block is invalid, or if the
        Size argument is 0.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONGLONG LastByteOffset;

    if (Size == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Make sure the read does not extend past the volume end.
    //

    LastByteOffset = ByteOffset + Size - 1;

    //
    // The minimum size for an NTFS volume is 1MB.  In absence of
    // any volume context, limit the read to a 1MB volume.
    //

    if (ARGUMENT_PRESENT(VolumeContext) == FALSE) {
        if (LastByteOffset >= (1024 * 1024)) {
            return STATUS_UNSUCCESSFUL;
        }

    } else if (ClustersFromBytes(VolumeContext, LastByteOffset) >
               VolumeContext->TotalClusters) {

        return STATUS_UNSUCCESSFUL;
    }

    return BlDeviceReadAtOffset(DeviceId,
                                ByteOffset,
                                Size,
                                Buffer,
                                BytesRead);
}

NTSTATUS
NtfspDeviceWriteAtOffset (
    __in_opt PNTFS_DEVICE_CONTEXT VolumeContext,
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __in_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Wrapper for device writes.  Provided a ByteOffset, this routine will
    verify the offset is contained within the NTFS volume prior to calling
    into the actual device library to perform the operation.

Arguments:

    VolumeContext - The NTFS mounted device the write operation is to be
        performed on.

    DeviceId - The identifier for the device to be read.

    ByteOffset - The offset on the device for the write operation to start.

    Size - The number of bytes to write.

    Buffer - Buffer with contents to write to the block i/o device.

    BytesWritten - Pointer to address to store the number of bytes actually
        written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL if the write is not wholly contained within the
        NTFS volume.

    STATUS_INVALID_PARAMETER if a requested block is invalid, or if the
        Size argument is 0.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONGLONG LastByteOffset;

    if (Size == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Make sure the write does not extend past the volume end.
    //

    LastByteOffset = ByteOffset + Size - 1;

    //
    // The minimum size for an NTFS volume is 1MB.  In absence of
    // any volume context, limit the write to a 1MB volume.
    //

    if (ARGUMENT_PRESENT(VolumeContext) == FALSE) {
        if (LastByteOffset >= (1024 * 1024)) {
            return STATUS_UNSUCCESSFUL;
        }

    } else if (ClustersFromBytes(VolumeContext, LastByteOffset) >
               VolumeContext->TotalClusters) {

        return STATUS_UNSUCCESSFUL;
    }

    return BlDeviceWriteAtOffset(DeviceId,
                                 ByteOffset,
                                 Size,
                                 Buffer,
                                 BytesWritten);
}

BOOLEAN
NtfspFileRecordCompareKey (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    )

/*++

Routine Description:

    This routine compares two file record segment hash table keys.

Arguments:

    Key1 - Supplies a pointer to a hash key.

    Key2 - Supplies a pointer to a second hash key.

Return Value:

    TRUE is the keys are identical. FALSE otherwise.

--*/

{

    PFILE_RECORD_SEGMENT_KEY FrsKey1;
    PFILE_RECORD_SEGMENT_KEY FrsKey2;

    ASSERT(Key1 != NULL);
    ASSERT(Key2 != NULL);
    ASSERT(Key1->Size == Key2->Size);
    ASSERT(Key1->Flags == Key2->Flags);

    FrsKey1 = (PFILE_RECORD_SEGMENT_KEY)Key1->Value;
    FrsKey2 = (PFILE_RECORD_SEGMENT_KEY)Key2->Value;
    if (FrsKey1->FrsNumber != FrsKey2->FrsNumber) {
        return FALSE;
    }

    return TRUE;
}

ULONG
NtfspFileRecordHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    )

/*++

Routine Description:

    This routine is an optimized hash function for the FRS cache.

Arguments:

    Key - Supplies the key identifying the object to be hashed.

    HashTableSize - Supplies the number of buckets in the hash table.

--*/

{

    ULONG Sum;
    PUCHAR UcharPointer;

    UNREFERENCED_PARAMETER(HashTableSize);

    ASSERT(Key != NULL);
    ASSERT(Key->Size == sizeof(FILE_RECORD_SEGMENT_KEY));
    ASSERT(Key->Value != NULL);
    ASSERT(HashTableSize == FILE_RECORD_HASH_TABLE_SIZE);

    UcharPointer = Key->Value;
    Sum = UcharPointer[0];
    Sum += UcharPointer[1];
    Sum += UcharPointer[2];
    Sum += UcharPointer[3];
    Sum += UcharPointer[4];
    Sum += UcharPointer[5];
    Sum += UcharPointer[6];
    Sum += UcharPointer[7];
    return (Sum & (FILE_RECORD_HASH_TABLE_SIZE - 1));
}

#ifndef FILE_MIN

VOID
NtfspTraceBuildChunkTableFailure (
    __in PFILE_ENTRY FileEntry,
    __in NTSTATUS Status
    )


/*++

Routine Description:

    This route will log an ETW event for a failure to construct chunk table for
    WIM compressed file.

Arguments:

    FileEntry - Entry for the compressed file.

    Status - Failure status code.

Return Value:

    None.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    ULONG PathSize;

    PathSize = (ULONG)((wcslen(FileEntry->FilePath) + 1) *
                       sizeof(WCHAR));

    EventDataDescCreate(&EventDataDescriptor[0],
                        &Status,
                        sizeof(Status));

    EventDataDescCreate(&EventDataDescriptor[1],
                        FileEntry->FilePath,
                        PathSize);

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_BUILD_CHUNK_TABLE_FAILURE,
                  NULL,
                  2,
                  EventDataDescriptor);

    return;
}

NTSTATUS
NtfspBuildChunkTable (
    __in PFILE_ENTRY FileEntry,
    __in ULONG CompressedFileSize,
    __in ULONG CompressedChunkSize,
    __in ULONG Chunks,
    __deref_out_ecount(Chunks + 2) PULONG * ChunkTable
    )

/*++

Routine Description:

    Converts the file chunk table into a memory chunk table. Chunk numbering is
    one based. For computing chunk size the following is used:

    Size(chunk x) = Table[x] - Table[x-1]

    Chunk size should never exceed compression unit size.

    File chunk table:
         offset to chunk 2, offset to chunk 3, ..., offset to chunk n

    Memory chunk table:
         offset to chunk 1, offset to chunk 2, offset to chunk 3, ...,
            offset to chunk n, offset to byte after end of chunk n

    This routine assumes that ChunkTable has 2 extra entries more than "Chunks"

Arguments:

    FileEntry - Entry for the compressed file.

    CompressedFileSize - Compressed file size.

    CompressedChunkSize - Chunk size used during compression.

    Chunks - Number of chunks in the file.

    ChunkTable - Receives constructed chunk table for the file.

Return Value:

    STATUS_SUCCESS when successful. Error status if unable to construct chunk
    table.

--*/

{

    PULONG ChunkTableLocal;
    ULONG CurrentOffset;
    ULONG NextOffset;
    ULONG Length;
    ULONG Index;
    ULONG *p1;
    ULONG *p2;
    NTSTATUS Status;
    ULONG TableLength;

    ChunkTableLocal = NULL;

    //
    // Only files less then 4GB in size are supported, so the routine only needs
    // to deal with ULONG chunks.
    //

    Status = RtlULongMult(Chunks, sizeof(ULONG), &Length);
    if (!NT_SUCCESS(Status)) {
        goto NtfspBuildChunkTableEnd;
    }

    //
    // Length = # of bytes of ChunkTable in the file, NtfspBuildChunkTable
    // needs 2 more entries.
    //

    Status = RtlULongAdd(Length, 2 * sizeof(ULONG), &Length);
    if (!NT_SUCCESS(Status)) {
        goto NtfspBuildChunkTableEnd;
    }

    //
    // Allocate a temp buffer.
    //

    ChunkTableLocal = BlMmAllocateHeap(Length);
    if (ChunkTableLocal == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspBuildChunkTableEnd;
    }

    //
    // Read chunk table from the file, subtracting extra entries.
    //

    if (Chunks != 0) {
        Length -= (2 * sizeof(ULONG));
        Status = NtfspReadAtOffset(FileEntry,
                                   0,
                                   ChunkTableLocal,
                                   Length,
                                   NULL);

        if (!NT_SUCCESS(Status)) {
            goto NtfspBuildChunkTableEnd;
        }
    }

    //
    // Length of chunk table in the file.
    //

    TableLength = Chunks * sizeof(ULONG);
    p2 = ChunkTableLocal + Chunks + 1;

    //
    // Last entry is offset beyond end of compressed data.
    //

    NextOffset = (ULONG)CompressedFileSize;
    *p2 = NextOffset;
    p2--;
    if (Chunks != 0) {
        p1 = ChunkTableLocal + Chunks - 1;
        for (Index = 0; Index < Chunks; Index += 1) {
            CurrentOffset = TableLength + *p1;
            if ((CurrentOffset > NextOffset) ||
                ((NextOffset - CurrentOffset) > CompressedChunkSize)) {

                Status = STATUS_FILE_CORRUPT_ERROR;
                goto NtfspBuildChunkTableEnd;
            }

            *p2 = CurrentOffset;
            p1--;
            p2--;
            NextOffset = CurrentOffset;
        }
    }

    //
    // First entry is offset to immediately after chunk table in file.
    //

    CurrentOffset = TableLength;
    if ((CurrentOffset > NextOffset) ||
        ((NextOffset - CurrentOffset) > CompressedChunkSize)) {

        Status = STATUS_FILE_CORRUPT_ERROR;
        goto NtfspBuildChunkTableEnd;
    }

    *p2 = CurrentOffset;
    *ChunkTable = ChunkTableLocal;

    NT_ASSERT(NT_SUCCESS(Status));

NtfspBuildChunkTableEnd:
    if (!NT_SUCCESS(Status)) {
        NtfspTraceBuildChunkTableFailure(FileEntry, Status);
        if (ChunkTableLocal != NULL) {
            BlMmFreeHeap(ChunkTableLocal);
        }
    }

    return Status;
}

NTSTATUS
NtfspWimRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Reads Size bytes starting at the offset indicated in the file information
    structure : FileEntry->FileData.Infomation.FileOffset and updates the
    FileOffset.

    N.B. This version of the read routine is used for reading from WIM
         compressed files.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the read operation.

    Size - The number of bytes to read.

    Count - Address to store the number of bytes actually read.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_FILE_IS_A_DIRECTORY if the file specified is a directory.

    STATUS_INVALID_PARAMETER if read request walks off attribute value.

    A non-resident attribute can also have the following error status codes.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONG ByteOffset;
    ULONG BytesRead;
    ULONG ChunkCount;
    ULONG Chunk;
    PNTFS_COMPRESSION_CONTEXT CompressionContext;
    PVOID CompressedDataBuffer;
    PCHAR DecompDestination;
    ULONG DesiredSize;
    PNTFS_FILE_CONTEXT FileContext;
    ULONG FragmentOffset;
    ULONG FullChunkSize;
    ULONG LastChunk;
    ULONG Length;
    PVOID Next;
    PNTFS_FILE NtfsFile;
    LARGE_INTEGER Start;
    NTSTATUS Status;
    ULONG ThisCompressedReadLength;
    ULONG UncompressedSize;

    NtfsFile = (PNTFS_FILE)FileEntry->FileData;
    FileContext = &NtfsFile->FileContext;

    //
    // Do not allow any reads from a directory.  If a user wishes to manipulate
    // a directory, they should use GetNextFile, CreateFile, etc.
    //

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        return STATUS_FILE_IS_A_DIRECTORY;
    }

    Status = STATUS_SUCCESS;
    BytesRead = 0;

    //
    // Read for actual decompression read operation.
    //

    ChunkCount = ((FileContext->OriginalSize
                   + FileContext->CompressedChunkSize - 1)
                  / FileContext->CompressedChunkSize) - 1;

    //
    // Convert the file based Chunk table into a memory based Chunk table.
    //

    if (FileContext->ChunkTable == NULL) {
        Status = NtfspBuildChunkTable(FileEntry,
                                      (ULONG)FileContext->CompressedSize,
                                      FileContext->CompressedChunkSize,
                                      ChunkCount,
                                      &FileContext->ChunkTable);

        if (!NT_SUCCESS(Status)) {
            goto NtfspReadWimEnd;
        }
    }

    CompressionContext = NtfspGetCompressionContext();
    ByteOffset = (ULONG)NtfsFile->Information.FileOffset;
    Chunk = ByteOffset / FileContext->CompressedChunkSize;
    FragmentOffset = ByteOffset % FileContext->CompressedChunkSize;

    //
    // Adjust read size if trying to read past the end of the file.
    //

    if (ByteOffset + Size > FileContext->OriginalSize) {
        Size = (ULONG)FileContext->OriginalSize - ByteOffset;
    }

    Next = Buffer;
    while (NT_SUCCESS(Status) && Size) {
        ThisCompressedReadLength = 0;
        CompressedDataBuffer = CompressionContext->CompressedDataBuffer;

        //
        // Start is first byte after ChunkTable + offset to start of chunk.
        //

        Start.QuadPart = FileContext->ChunkTable[Chunk];

        //
        // Now walk forward counting the maximum number of chunks we can read.
        // While doing this:
        //   ThisCompressedReadLength - the size of the compressed read we're up to.
        //   Length - the amount of uncompressed data that we'd get.
        //   LastChunk - the chunk immediately following the read.
        //

        LastChunk = Chunk;
        Length = 0;
        for (;;) {
            LastChunk += 1;
            Length += FileContext->CompressedChunkSize;
            ThisCompressedReadLength = FileContext->ChunkTable[LastChunk] - FileContext->ChunkTable[Chunk];

            //
            //  Back up if compressed buffer size is exceeded.
            //

            if (ThisCompressedReadLength > CompressionContext->CompressedBufferSize) {
                Length -= FileContext->CompressedChunkSize;
                LastChunk -= 1;
                ThisCompressedReadLength = FileContext->ChunkTable[LastChunk] - FileContext->ChunkTable[Chunk];
                break;
            }

            //
            //  We have sufficient to satisfy the original request,
            //  so we can stop.
            //

            if (Length - FragmentOffset >= Size) {
                break;
            }
        }

        if (ThisCompressedReadLength > CompressionContext->CompressedBufferSize) {

            NT_ASSERT(FALSE);

            Status = STATUS_FILE_INVALID;
            goto NtfspReadWimEnd;
        }

        //
        // Read compressed data into temporary buffer.
        //

        if (ThisCompressedReadLength > 0) {
            Status = NtfspReadAtOffset(FileEntry,
                                       Start.QuadPart,
                                       CompressedDataBuffer,
                                       ThisCompressedReadLength,
                                       NULL);

            if (!NT_SUCCESS(Status)) {
                goto NtfspReadWimEnd;
            }
        }

        //
        // Decompress the chunks.
        //

        while (Chunk < LastChunk) {
            Length = FileContext->ChunkTable[Chunk + 1] - FileContext->ChunkTable[Chunk];
            UncompressedSize = FileContext->CompressedChunkSize - FragmentOffset;
            if (UncompressedSize > Size) {
                UncompressedSize = Size;
            }

            if (Length == 0) {

                //
                // Handle a zero filled chunk.
                //

#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "False positive 26015, prefast thinks uncompressedSize can be greater than Length, but we just bounded it 10 lines ago")
                RtlZeroMemory(Next, UncompressedSize);

            } else if ((Length == FileContext->CompressedChunkSize) ||
                       ((Chunk == ChunkCount) &&
                        (Length == (ULONG)(FileContext->OriginalSize
                                           % FileContext->CompressedChunkSize)))) {

                RtlCopyMemory(Next, Add2Ptr(CompressedDataBuffer, FragmentOffset), UncompressedSize);

            } else {
                DesiredSize = UncompressedSize;

                //
                // Adjust uncompressed size of last chunk.
                //

                FullChunkSize = FileContext->CompressedChunkSize;
                if (Chunk == ChunkCount) {
                    FullChunkSize = (ULONG)(FileContext->OriginalSize
                                            - (LONGLONG)(Chunk * FileContext->CompressedChunkSize));
                }

                //
                // Check if entire chunk can be decompressed directly to user
                // buffer.
                //

                if ((FragmentOffset == 0) && (UncompressedSize == FullChunkSize)) {
                    DecompDestination = Next;

                } else {

                    //
                    // We need to decompress the whole chunk, we can do it over the top
                    // of the read input (WorkSpace), because LZX decompresses into its
                    // work buffer and copies result into output buffer at the end.
                    //

                    UncompressedSize = FullChunkSize;
                    DecompDestination = CompressionContext->UncompressedChunkBuffer;
                }

                if (FileContext->Algorithm == FILE_PROVIDER_COMPRESSION_LZX) {
                    if (LZX_Decode(CompressionContext->LZXDecompressContext,
                                   UncompressedSize,
                                   CompressedDataBuffer,
                                   Length,
                                   (PBYTE)DecompDestination,
                                   UncompressedSize,
                                   (PLONG)&UncompressedSize)) {

                        Status = STATUS_FILE_INVALID;
                    }

                    //
                    // Reset the compression history.
                    //

                    LZX_DecodeNewGroup(CompressionContext->LZXDecompressContext);

                } else {

                    NT_ASSERT(FileContext->Algorithm == FILE_PROVIDER_COMPRESSION_XPRESS4K);

                    Status = RtlDecompressBufferEx(COMPRESSION_FORMAT_XPRESS_HUFF,
                                                   (PUCHAR)DecompDestination,
                                                   UncompressedSize,
                                                   CompressedDataBuffer,
                                                   Length,
                                                   &UncompressedSize,
                                                   CompressionContext->XpressWorkspace);
                }

                if (!NT_SUCCESS(Status)) {
                    goto NtfspReadWimEnd;
                }

                //
                //  Now copy needed portion from WorkSpace to user buffer.
                //

                if (DecompDestination != Next) {
                    UncompressedSize = DesiredSize;
                    RtlCopyMemory(Next, DecompDestination + FragmentOffset, UncompressedSize);
                }
            }

            CompressedDataBuffer = Add2Ptr(CompressedDataBuffer, Length);
            Next = Add2Ptr(Next, UncompressedSize);
            Size -= UncompressedSize;
            ByteOffset += UncompressedSize;
            BytesRead += UncompressedSize;
            Chunk += 1;
            FragmentOffset = 0;
        }
    }

    NT_ASSERT(Status == STATUS_SUCCESS);

    NtfsFile->Information.FileOffset += BytesRead;
    if (ARGUMENT_PRESENT(Count)) {
        *Count = BytesRead;
    }

NtfspReadWimEnd:
    return Status;
}

NTSTATUS
NtfspReadAtOffset (
    __in PFILE_ENTRY FileEntry,
    __in ULONG64 FileOffset,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Reads Size bytes starting at the specified offset.

    N.B. File offset will not be updated.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the read operation.

    Size - The number of bytes to read.

    Count - Address to store the number of bytes actually read.

Return Value:

    STATUS_SUCCESS when successful or appropriate status code.

--*/

{

    PNTFS_FILE_CONTEXT FileContext;
    PNTFS_FILE FileData;
    NTSTATUS Status;

    FileData = (PNTFS_FILE)FileEntry->FileData;
    FileContext = &FileData->FileContext;

    NT_ASSERT(IS_FILE_DIRECTORY_SET(FileEntry->Attributes) == FALSE);

    Status = NtfspReadAttribute(NtfsDeviceTable[FileData->NtfsDeviceTableId],
                                &FileContext->DataAttribute,
                                FileOffset,
                                Size,
                                Buffer);

    if (!NT_SUCCESS(Status)) {
        goto NtfspReadAtOffsetEnd;
    }

    if (Count != NULL) {
        *Count = Size;
    }

NtfspReadAtOffsetEnd:
    return Status;
}

NTSTATUS
NtfspWimWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Write routine for WIM compressed files. Writing to such files is not
    supported.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the write operation.

    Size - The number of bytes to write.

    Count - Address to store the number of bytes actually written.

Return Value:

    STATUS_NOT_SUPPORTED

--*/

{

    UNREFERENCED_PARAMETER(FileEntry);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Count);

    NT_ASSERT(FALSE);

    return STATUS_NOT_SUPPORTED;
}

NTSTATUS
NtfspInitializeCompressionContext (
    __in ULONG Algorithm
    )

/*++

Routine Description:

    This routine will initialize compression context for dealing with WIM
    compressed files.

Arguments:

    Algorithm - Compression algorithm used.

Return Value:

    NTSTATUS.

--*/

{

    ULONG ChunkSize;
    ULONG LZXWorkSpaceSize;
    NTSTATUS Status;
    ULONG Temp;
    ULONG XpressWorkSpaceSize;

    UNREFERENCED_PARAMETER(Algorithm);

    if (NtfspCompressionContext.Initialized != FALSE) {
        return STATUS_SUCCESS;
    }

    NT_VERIFY(
        NT_SUCCESS(
            RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_XPRESS_HUFF,
                                           &XpressWorkSpaceSize,
                                           &Temp)));

    LZXWorkSpaceSize = LZX_GetDecodeBufferSize(WIM_LZX_CHUNK_SIZE);
    ChunkSize = max(WIM_LZX_CHUNK_SIZE, WIM_XPRESS_CHUNK_SIZE);

    //
    // Compressed data buffer is used as temporary storage for reading
    // compressed data. It is chosen to be 2X uncompressed chunk size.
    // which allows for reading two or more chunks at a time.
    //

    NtfspCompressionContext.CompressedBufferSize = ChunkSize * 2;
    NtfspCompressionContext.CompressedDataBuffer =
        BlMmAllocateHeap(NtfspCompressionContext.CompressedBufferSize);

    if (NtfspCompressionContext.CompressedDataBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeCompressionContextEnd;
    }

    NtfspCompressionContext.UncompressedChunkBuffer = BlMmAllocateHeap(ChunkSize);
    if (NtfspCompressionContext.UncompressedChunkBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeCompressionContextEnd;
    }

    NtfspCompressionContext.XpressWorkspace = BlMmAllocateHeap(XpressWorkSpaceSize);
    if (NtfspCompressionContext.XpressWorkspace == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeCompressionContextEnd;
    }

    NtfspCompressionContext.LZXWorkspace = BlMmAllocateHeap(LZXWorkSpaceSize);
    if (NtfspCompressionContext.LZXWorkspace == NULL) {
        Status = STATUS_NO_MEMORY;
        goto NtfspInitializeCompressionContextEnd;
    }

    NtfspCompressionContext.LZXDecompressContext =
        LZX_DecodeInit(NtfspCompressionContext.LZXWorkspace,
                       LZXWorkSpaceSize,
                       WIM_LZX_CHUNK_SIZE);

    NtfspCompressionContext.Initialized = TRUE;
    Status = STATUS_SUCCESS;

NtfspInitializeCompressionContextEnd:
    if (!NT_SUCCESS(Status)) {
        NtfspFreeCompressionContext();
    }

    return Status;
}

VOID
NtfspFreeCompressionContext (
    VOID
    )

/*++

Routine Description:

    This routine is used to free previously allocated compression context.

Arguments:

    None.

Return Value:

    None.

--*/

{

    if (NtfspCompressionContext.CompressedDataBuffer != NULL) {
        BlMmFreeHeap(NtfspCompressionContext.CompressedDataBuffer);
        NtfspCompressionContext.CompressedDataBuffer = NULL;
    }

    if (NtfspCompressionContext.UncompressedChunkBuffer != NULL) {
        BlMmFreeHeap(NtfspCompressionContext.UncompressedChunkBuffer);
        NtfspCompressionContext.UncompressedChunkBuffer = NULL;
    }

    if (NtfspCompressionContext.LZXWorkspace != NULL) {
        BlMmFreeHeap(NtfspCompressionContext.LZXWorkspace);
        NtfspCompressionContext.LZXWorkspace = NULL;
    }

    if (NtfspCompressionContext.XpressWorkspace != NULL) {
        BlMmFreeHeap(NtfspCompressionContext.XpressWorkspace);
        NtfspCompressionContext.XpressWorkspace = NULL;
    }

    NtfspCompressionContext.Initialized = FALSE;
    return;
}

PNTFS_COMPRESSION_CONTEXT
NtfspGetCompressionContext (
    VOID
    )

/*++

Routine Description:

    This routine retrieves compression context used for decompressing WIM
    compressed files.

Arguments:

    None.

Return Value:

    Pointer to the compression context structure.

--*/

{

    return &NtfspCompressionContext;
}

#endif

