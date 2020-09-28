/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    fatboot.c

Abstract:

    Implementation of the boot library's native FAT file system support.
    The routines in this module make up the function table for a for
    a FAT file entry.

Environment:

    Boot

--*/

// -------------------------------------------------------------------- Pragmas

//
// Disable warnings for nonstandard extensions. Used in fat.h
//

#pragma warning(disable:4214)

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include "fatboot.h"

// -------------------------------------------------------------------- Globals

//
// Function table for a FAT partition.
//

const FILESYSTEM_FUNCTION_TABLE FatFunctionTable = {
    (PFILESYSTEM_OPEN_ROUTINE)             FatOpen,
    (PFILESYSTEM_CLOSE_ROUTINE)            FatClose,
    (PFILESYSTEM_READ_ROUTINE)             FatRead,
    (PFILESYSTEM_WRITE_ROUTINE)            FatWrite,
    (PFILESYSTEM_GET_NEXT_FILE_ROUTINE)    FatGetNextFile,
    (PFILESYSTEM_GET_INFORMATION_ROUTINE)  FatGetInformation,
    (PFILESYSTEM_SET_INFORMATION_ROUTINE)  FatSetInformation,
    (PFILESYSTEM_GET_REPARSE_DATA_ROUTINE) NULL
};

//
// Routines required to register this FAT filesystem implementation.
//

const FILESYSTEM_REGISTER_FUNCTION_TABLE FatRegisterFunctionTable = {
    (PFILESYSTEM_INITIALIZE_ROUTINE)      FatInitialize,
    (PFILESYSTEM_DESTROY_ROUTINE)         FatDestroy,
    (PFILESYSTEM_MOUNT_ROUTINE)           FatMount,
    (PFILESYSTEM_PURGE_CACHE_ROUTINE)     NULL
};

// ------------------------------------------------------------------ Functions

NTSTATUS
FatInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the FAT Device Table used to keep track of the File Allocation
    Table and the bios parameter block for each FAT volume.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if the device table could not be allocated.

--*/

{

    //
    // Initialize the Fat Device Table.  This table keeps track of Fat Device
    // Context for each FAT Volume.
    //

    FatDeviceTableEntries = INITIAL_FAT_DEVICE_ENTRIES;
    FatDeviceTable = BlMmAllocateHeap(FatDeviceTableEntries *
                                      sizeof(PFAT_DEVICE_CONTEXT));

    if (FatDeviceTable == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(FatDeviceTable,
                  FatDeviceTableEntries * sizeof(PFAT_DEVICE_CONTEXT));

    FatpLongFileName = BlMmAllocateHeap((FAT_MAX_LFN_LENGTH + 1) *
                                        sizeof(WCHAR));

    if (FatpLongFileName == NULL) {
        return STATUS_NO_MEMORY;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FatDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys the FAT Device Table used to keep track of the File Allocation
    Table and the bios parameter block for each FAT volume.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    Other Error if error occured destroying any device entry.

--*/

{

    NTSTATUS Status;

    //
    // Destroy the Fat Device Table.
    //

    Status = BlTblMap(FatDeviceTable,
                      FatDeviceTableEntries,
                      FatpDeviceTableDestroyEntry,
                      FALSE);

    //
    // Free the memory allocated during initialization.
    //

    BlMmFreeHeap(FatDeviceTable);
    BlMmFreeHeap(FatpLongFileName);

    return Status;
}

NTSTATUS
FatMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Attempts to mount FAT filesystem on a block i/o device.  Provided a block
    i/o device (DeviceId), this routine will attempt to detect a FAT file
    system on the device.  If present, this routine will allocate and
    initialize a File Entry for the root directory of the file system.

Arguments:

    DeviceId - Device Id for a Block I/O Device.

    FilterLevel - (unused) - Indicates the filter location of the mount request
        (zero indicates this is the first mount request on the device).

    FileEntry - Pointer to address to store an allocated FILE_ENTRY structure
        if a FAT file system if detected on the Block I/O Device referenced by
        DeviceId.  FileEntry is only allocated if the a FAT filesystem is
        detected.

Return Value:

    STATUS_SUCCESS when successful (and the file system has been mounted on
        the device).
    STATUS_UNSUCCESSFUL if the device does not have a FAT filesystem.
    STATUS_NO_MEMORY if there was an error allocating memory for any internal
        data structures.

--*/

{

    PFILE_ENTRY AllocatedFileEntry;
    PACKED_BOOT_SECTOR BootSector;
    BIOS_PARAMETER_BLOCK Bpb;
    DEVICE_INFORMATION DeviceInformation;
    PFAT_DEVICE_CONTEXT FatContext;
    PFAT_FILE File;
    ULONG Id;
    BOOLEAN IsFat32;
    ULONG OriginalIoAccessFlags;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER( FilterLevel);

    AllocatedFileEntry = NULL;

    //
    // In attempt to minimize the number of physical reads performed while
    // mounting a filesystem, cache the read of the boot sector.
    //

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);

    ASSERT((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION) ||
           (DeviceInformation.DeviceType == DEVICE_PARTITION_EX));

    OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

    Status = BlDeviceSetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);

    //
    // A FAT Partition is identifiable by it's bios parameter block.
    // The BPB is located on the first sector of the volume or Block I/O Device
    // that contains the FAT filesystem.
    //
    // Note: the FAT32 Specification, v1.03 was used when determining the
    // structure and validity of the the bios parameter block.
    //

    //
    // Attempt to read the bios parameter block for the device.
    //

    Status = BlDeviceReadAtOffset(DeviceId,
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

    FatUnpackBios(&Bpb, &BootSector.PackedBpb);

    //
    // Check to see if the BPB is for a FAT volume.
    //

    //
    // The first 3 bytes of the boot sector contain the jump instruction to
    // the bootcode.  The byte sequence can be 0xEB 0x?? 0x90 or 0xE9 0x?? 0x??
    // where ?? indicates any 8-bit value.
    //

    if (((BootSector.Jump[0] != 0xEB) && (BootSector.Jump[0] != 0xE9)) ||
        ((BootSector.Jump[0] == 0xEB) && (BootSector.Jump[2] != 0x90))) {

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
    // The number of sectors per allocation unit must be a power of two.
    //

    if ((Bpb.SectorsPerCluster & (Bpb.SectorsPerCluster - 1)) != 0) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // The number of reserved sectors must not be zero. For Fat12, Fat16
    // volumes, this value should never be anything other than 1.
    //

    if (Bpb.ReservedSectors == 0) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // The number of FAT data structures of the volume must be at least one.
    //

    if (Bpb.Fats == 0) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Bpb.Sectors is the old 16 bit total count of sectors on the volume. This
    // field can be zero (and ignored), but if it is, the Bpb.LargeSectors must
    // be non-zero. FAT32 requires the use of only the 32 bit value.
    //

    IsFat32 = (BOOLEAN)IsBpbFat32(&Bpb);
    if (((Bpb.Sectors == 0) && (Bpb.LargeSectors == 0)) ||
        ((IsFat32 != FALSE) && (Bpb.Sectors != 0))) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // The legal values for the media field are [0xf0, 0xf8, 0xf9, 0xfa, 0xfb,
    // 0xfc, 0xfd, 0xfe, 0xff]. The standard for fixed media is 0xf8. For
    // removable media it is 0xf0.
    //

    if ((Bpb.Media != 0xF0) && (Bpb.Media != 0xF8) && (Bpb.Media != 0xF9) &&
        (Bpb.Media != 0xFA) && (Bpb.Media != 0xFB) && (Bpb.Media != 0xFC) &&
        (Bpb.Media != 0xFD) && (Bpb.Media != 0xFE) && (Bpb.Media != 0xFF)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // On FAT32 volumes, the number of root entries (the number of 32 byte
    // directory entries in the root directory) must be set to zero.
    //

    if ((IsFat32 != FALSE) && (Bpb.RootEntries != 0)) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Allocate a buffer for the device specific internal data structure. This
    // will contain the bios parameter block and a window of the file
    // allocation table. This is unique for each device/volume with a FAT
    // filesystem.
    //

    FatContext = BlMmAllocateHeap(sizeof(FAT_DEVICE_CONTEXT));
    if (FatContext == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Initialize the FAT device structure.
    //

    RtlZeroMemory(FatContext, sizeof(FAT_DEVICE_CONTEXT));
    RtlCopyMemory(&FatContext->Bpb, &Bpb, sizeof(BIOS_PARAMETER_BLOCK));

    //
    // Attempt to add the FAT device structure to the FatDeviceTable which is
    // used to store the structure for each device that has a FAT filesystem.
    //

    Status = BlTblSetEntry(&FatDeviceTable,
                           &FatDeviceTableEntries,
                           FatContext,
                           &Id,
                           TblDoNotPurgeEntry);

    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(FatContext);
        return Status;
    }

    //
    // A FAT volume was successfully found; allocate a buffer for the newly
    // opened file's FILE_ENTRY.
    //

    AllocatedFileEntry = BlMmAllocateHeap(sizeof(FILE_ENTRY));
    if (AllocatedFileEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Initialize the file entry for the caller. This should be fairly easy,
    // since it is creating a file entry for the root directory on the device
    // described by FatContext.
    //

    RtlZeroMemory(AllocatedFileEntry, sizeof(FILE_ENTRY));

    //
    // Allocate a buffer for the filepath.
    //

    AllocatedFileEntry->FilePath = BlMmAllocateHeap(sizeof(L"\\"));
    if (AllocatedFileEntry->FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FatMountEnd;
    }

    wcsncpy_s(AllocatedFileEntry->FilePath,
              sizeof(L"\\") / sizeof(WCHAR),
              L"\\",
              (sizeof(L"\\") / sizeof(WCHAR)) - 1);

    //
    // The device id stored in a file entry describes the device the file
    // system exists on. It is used when performing device reads for the
    // filesystem.
    //

    AllocatedFileEntry->DeviceId = DeviceId;

    //
    // Set the function table.
    //

    RtlCopyMemory(&AllocatedFileEntry->FunctionTable,
                  &FatFunctionTable,
                  sizeof(FILESYSTEM_FUNCTION_TABLE));

    //
    // Allocate a buffer for the file data.
    //

    File = BlMmAllocateHeap(sizeof(FAT_FILE));
    if (File == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FatMountEnd;
    }

    RtlZeroMemory(File, sizeof(FAT_FILE));
    AllocatedFileEntry->FileData = File;

    //
    // Initialize Fat File structure.  It is composed of three fields, the FAT
    // specific file information, the user file information and the id to the
    // FatContext structure.
    //

    File->FatDeviceTableId = Id;

    //
    // The root directory does not have a Dirent.  Each file's Dirent is a 32
    // bit entry in the parent directory's file content.  Since the root
    // directory does not have a parent, there is no such entry for it.
    //

    File->FileContext.Dirent.Attributes = FAT_DIRENT_ATTR_DIRECTORY;
    SET_FLAGS(File->Information.Flags, FILE_FLAGS_IS_DIRECTORY);
    SET_FILE_DIRECTORY(AllocatedFileEntry->Attributes);
    File->Information.FileSystemName = FAT_DRIVER_NAME;

    //
    // Read part of the File Allocation Table into memory. Do this before
    // determining the Directory Size, since a FAT32 filesystem will require
    // walking the File Allocation Table in order to determine its size.
    //

    OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

    Status = BlDeviceSetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);

    Status = BlDeviceReadAtOffset(DeviceId,
                                  FatLbo(&Bpb),
                                  FAT_WINDOW_SIZE,
                                  FatContext->FatWindow,
                                  NULL);

    DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
    BlDeviceSetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto FatMountEnd;
    }

    //
    // The root directory sectors are different for FAT12/16 vs. FAT32.
    // On FAT32, the root directory can span multiple clusters.
    //

    if (IsFat32 != FALSE) {
        File->FileContext.FileLbo = FatpIndexToLbo(FatContext,
                                                   Bpb.RootDirFirstCluster);

        //
        // The BPB does not indicate the size of the Root Directory.  Just like
        // any other directory, the size is determined by walking the cluster
        // chain. The end of the directory is signified by the EOC.
        //

        File->Information.FileSize = FatpGetDirectorySize(AllocatedFileEntry);

    } else {
        File->FileContext.FileLbo = FatRootDirectoryLbo(&Bpb);
        File->Information.FileSize = FatRootDirectorySize(&Bpb);
    }

    //
    // Maintain a file pointer within the FAT cluster chain for a file.
    //

    FatpResetCurrentClusterPointer(File);

    //
    // Return to caller with the new file entry for the mounted file.
    //

    *FileEntry = AllocatedFileEntry;

FatMountEnd:

    //
    // If we were encountered any errors, free allocated memory before
    // returning to caller.
    //

    if (!NT_SUCCESS(Status)) {
        if (AllocatedFileEntry != NULL) {
            if (AllocatedFileEntry->FilePath != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FilePath);
            }

            if (AllocatedFileEntry->FileData != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FileData);
            }

            BlMmFreeHeap(AllocatedFileEntry);
        }
    }

    return Status;
}

NTSTATUS
FatOpen (
    __in PFILE_ENTRY ParentDirectoryEntry,
    __in PWSTR FileName,
    __in OPEN_FLAGS OpenFlags,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Opens the file, FileName, in the directory described by
    ParentDirectoryEntry.  If successful, this routine will allocate a
    FILE_ENTRY structure for the newly opened file.

    To open a file, we have to ensure that the file exists.  This routine walks
    the directory entries in the parent directory, looking for the dirent for
    FileName.

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

    PFILE_ENTRY AllocatedFileEntry;
    SIZE_T AllocationSize;
    DIRENT Dirent;
    LBO DirentOffset;
    ULONG FatDeviceTableId;
    PFAT_FILE FatFile;
    PWCHAR FilePath;
    SIZE_T FilePathLength;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(OpenFlags);

    //
    // Initialize local variables.
    //

    AllocatedFileEntry = NULL;

    //
    // Attempt to find the dirent for FileName in the parent directory.
    //

    Status = FatpSearchForDirent(ParentDirectoryEntry,
                                 FileName,
                                 &Dirent,
                                 &DirentOffset);

    if (!NT_SUCCESS(Status)) {
        goto FatOpenEnd;
    }

    //
    // If we are here, we have found a dirent for the requested file.  Allocate
    // a file entry for the file and collect any information that is possible.
    //

    AllocatedFileEntry = BlMmAllocateHeap(sizeof(FILE_ENTRY));
    if (AllocatedFileEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FatOpenEnd;
    }

    RtlZeroMemory(AllocatedFileEntry, sizeof(FILE_ENTRY));

    //
    // Do all allocations up front.  We additionally need to allocate a buffer
    // for the file path string and we need to allocate a FAT_FILE structure.
    //

    FilePathLength = wcslen(ParentDirectoryEntry->FilePath) +
                     wcslen(FileName) + 2;   // + 2 = '\\' + '\0';

    Status = RtlSIZETMult(FilePathLength, sizeof(WCHAR), &AllocationSize);
    if (!NT_SUCCESS(Status)) {
        goto FatOpenEnd;
    }

    FilePath = BlMmAllocateHeap(AllocationSize);
    if (FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FatOpenEnd;
    }

    FatFile = BlMmAllocateHeap(sizeof(FAT_FILE));
    if (FatFile == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FatOpenEnd;
    }

    RtlZeroMemory(FatFile, sizeof(FAT_FILE));

    //
    // Initialize the file entry.  The device id can be copied from the parent
    // directory.
    //

    AllocatedFileEntry->DeviceId = ParentDirectoryEntry->DeviceId;

    //
    // Construct the filepath.  The resulting string should be the following:
    // ParentDirectory's FullPath \ FileName
    //

    swprintf_s(FilePath,
               FilePathLength,
               (ParentDirectoryEntry->FilePath[1] == L'\0') ? L"%ls%ls" :
                                                              L"%ls\\%ls",
               ParentDirectoryEntry->FilePath,
               FileName);

    AllocatedFileEntry->FilePath = FilePath;

    //
    // Copy the FAT function table to the file entry.
    //

    RtlCopyMemory(&AllocatedFileEntry->FunctionTable,
                  &FatFunctionTable,
                  sizeof(FILESYSTEM_FUNCTION_TABLE));

    //
    // Initialize the file information (FAT_FILE) structure.  Much of the
    // file's information can be determined from the dirent.  In the case of a
    // directory, its size must be determined by walking its cluster chain,
    // looking for the EOC (End of Clusters).  This operation is done last, as
    // it requires an initialized File Entry structure since the procedure will
    // require reads of the File Allocation Table.
    //

    //
    // Initialize the Fat Context structure.
    //

    RtlCopyMemory(&FatFile->FileContext.Dirent, &Dirent, sizeof(DIRENT));
    FatFile->FileContext.DirentLbo = DirentOffset;

    //
    // The FatDeviceTableId is the same as the parent directory's.
    //

    FatDeviceTableId = ((PFAT_FILE)
                        (ParentDirectoryEntry->FileData))->FatDeviceTableId;

    FatFile->FatDeviceTableId = FatDeviceTableId;

    //
    // Determine the offset to the file.
    //

    FatFile->FileContext.FileLbo = FatpIndexToLbo(
                                       FatDeviceTable[FatDeviceTableId],
                                       (Dirent.FirstClusterOfFileHi << 16) |
                                       Dirent.FirstClusterOfFile);

    //
    // Maintain a file pointer within the FAT cluster chain for a file.
    //

    FatpResetCurrentClusterPointer(FatFile);

    //
    // Done constructing the File Data structure (minus the FileSize).
    //

    AllocatedFileEntry->FileData = FatFile;

    //
    // Determine the File Size and anything else dependent on the file being a
    // directory.
    //

    if (Dirent.Attributes & FAT_DIRENT_ATTR_DIRECTORY) {
        SET_FLAGS(FatFile->Information.Flags, FILE_FLAGS_IS_DIRECTORY);
        SET_FILE_DIRECTORY(AllocatedFileEntry->Attributes);
        FatFile->Information.FileSize = FatpGetDirectorySize(
                                            AllocatedFileEntry);

    } else {
        FatFile->Information.FileSize = Dirent.FileSize;
    }

    FatFile->Information.FileSystemName = FAT_DRIVER_NAME;

    //
    // Return the allocated and initialized file entry to the caller.
    //

    *FileEntry = AllocatedFileEntry;

FatOpenEnd:

    //
    // If we were encountered any errors, free allocated memory before
    // returning to caller.
    //

    if (!NT_SUCCESS(Status)) {
        if (AllocatedFileEntry != NULL) {
            if (AllocatedFileEntry->FilePath != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FilePath);
            }

            if (AllocatedFileEntry->FileData != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FileData);
            }

            BlMmFreeHeap(AllocatedFileEntry);
        }
    }

    return Status;
}

NTSTATUS
FatClose (
    __in PFILE_ENTRY FileEntry
    )

/*++

Routine Description:

    Closes a file.  All that is required is to ensure the File Allocaiton Table
    window that is in memory is flushed (if dirty) and all memory allocated for
    the file is freed.

Arguments:

    FileEntry - File entry for file to be closed.

Return Value:

    STATUS_SUCCESS when successful.

    Error status if unable to flush FAT window:
    STATUS_INVALID_PARAMETER if a FAT offset is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PFAT_DEVICE_CONTEXT FatContext;
    PFAT_FILE FileInfo;
    NTSTATUS Status;

    FileInfo = (PFAT_FILE)FileEntry->FileData;
    FatContext = FatDeviceTable[FileInfo->FatDeviceTableId];
    Status = STATUS_SUCCESS;

    //
    // Flush the file allocation table window if it is marked dirty.
    //

    if (FatContext->FatWindowDirty != FALSE) {
        Status = FatpWindowFlush(FileEntry);
    }

    //
    // Free memory allocated for the file.
    //

    BlMmFreeHeap(FileEntry->FileData);
    BlMmFreeHeap(FileEntry->FilePath);
    return Status;
}

NTSTATUS
FatRead (
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
    STATUS_INVALID_PARAMETER if a read offset is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONG ReadCount;
    NTSTATUS Status;

    //
    // Call into a helper routine to do the actual work provided this read
    // targets a file and not a directory.
    //

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        return STATUS_FILE_IS_A_DIRECTORY;
    }

    Status = FatpIoOperation(FileEntry, Buffer, Size, &ReadCount, TRUE);
    if (Count != NULL) {
        *Count = ReadCount;
    }

    return Status;
}

NTSTATUS
FatWrite (
    __in PFILE_ENTRY FileEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    Writes Size bytes starting at the offset indicated in the file information
    structure : FileEntry->FileData.Infomation.FileOffset.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for the write operation.

    Size - The number of bytes to write.

    Count - Address to store the number of bytes actually written.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a write offset is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    NTSTATUS Status;
    ULONG WriteCount;

    //
    // Call into a helper routine to do the actual work provided this write
    // targets a file and not a directory.
    //

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        return STATUS_FILE_IS_A_DIRECTORY;
    }

    Status = FatpIoOperation(FileEntry, Buffer, Size, &WriteCount, FALSE);
    if (Count != NULL) {
        *Count = WriteCount;
    }

    return Status;
}

NTSTATUS
FatGetNextFile (
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

    DIRENT Dirent;
    LBO DirentOffset;
    PFAT_FILE DirInfo;
    ULONG Size;
    NTSTATUS Status;

    //
    // Find the dirent for FileName.  In doing so, DirectoryEntry's file
    // pointer will be at the next dirent.  This can be further optimized
    // to check the dirent at the current file pointer - sizeof(DIRENT) to
    // see if it is for FileName.  The assumption here is that FatGetNextFile
    // will be called consecutively without other I/O operations in between.
    //
    // If FileName == NULL or FileName[0] == L'\0', then the caller is
    // initiating the search.  In that case, just set the File Pointer to the
    // begininning of the file.
    //

    if (FileName == NULL || FileName[0] == L'\0') {
        DirInfo = (PFAT_FILE)DirectoryEntry->FileData;
        DirInfo->Information.FileOffset = 0;
        FatpResetCurrentClusterPointer(DirInfo);

    } else {
        Status = FatpSearchForDirent(DirectoryEntry,
                                     FileName,
                                     &Dirent,
                                     &DirentOffset);

        if (!NT_SUCCESS(Status)) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    //
    // Read the next dirent.  It will be the one just after FileName.
    //

    Status = FatpGetNextFileNameDirent(DirectoryEntry,
                                       &Dirent,
                                       FatpLongFileName,
                                       &DirentOffset);

    if (!NT_SUCCESS(Status)) {
        return STATUS_NOT_FOUND;
    }

    Size = (ULONG)wcslen(FatpLongFileName) + 1;

    //
    // If the buffer is too small, return the required size.
    //

    if (*BufferSize < Size) {
        *BufferSize = Size;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Copy the filename to the caller's buffer.
    //

    wcscpy_s(FileName, *BufferSize, FatpLongFileName);
    return STATUS_SUCCESS;
}

NTSTATUS
FatGetInformation (
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

    PFAT_FILE FileInfo;

    FileInfo = (PFAT_FILE)FileEntry->FileData;

    //
    // Copy FileEntry->FileData.Information to the caller's buffer.
    //

    RtlCopyMemory(FileInformation,
                  &FileInfo->Information,
                  sizeof(FILE_INFORMATION));

    return STATUS_SUCCESS;
}

NTSTATUS
FatSetInformation (
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

    STATUS_INVALID_PARAMETER if the requested file offset is off the file, or
        if information fields other than the file offset are being set.

--*/

{

    PFAT_FILE FileInfo;
    FILE_INFORMATION FileInformationCopy;

    FileInfo = (PFAT_FILE)FileEntry->FileData;

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
        FatpResetCurrentClusterPointer(FileInfo);
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

// --------------------------------------------------------- Internal Functions

NTSTATUS
FatpSearchForDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __out PDIRENT Dirent,
    __out PLBO DirentLbo
    )

/*++

Routine Description:

    Searches the directory described by DirectoryEntry in search of a dirent
    for the file with the name, FileName.  If found, this routine will return
    the file offset to the direct as well as the dirent to the caller.

    This routine assumes that DirectoryEntry is indeed a directory.

    todo: FatpGet8Dot3Name can not predict the 8Dot3 name used if FileName has
    more than 8.3 characters.  May want to look at all dirents for a file to
    see if the filename's match.

Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    FileName - The file to look for.

    Dirent - An address to store the dirent for the filename, if found.

    DirentLbo - An address to store the file offset to the dirent for the
        file, if found.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if a file with the name FileName is not found in the
        searching directory.

    Other error if a device read fails:
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PFAT_FILE DirInfo;
    NTSTATUS Status;

    DirInfo = (PFAT_FILE)DirectoryEntry->FileData;

    ASSERT(CHECK_FLAG(DirInfo->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    //
    // A directory is a file whose data is a series of 32-bit dirents.  Walk
    // through the directory, searching for a dirent whose filename matches the
    // user requested file.
    //

    DirInfo->Information.FileOffset = 0;
    FatpResetCurrentClusterPointer(DirInfo);
    Status = FatpGetNextFileNameDirent(DirectoryEntry,
                                       Dirent,
                                       FatpLongFileName,
                                       DirentLbo);

    while (NT_SUCCESS(Status)) {

        //
        // If there is a long filename, compare that first.  If there is no
        // match here, then compare the short filename.
        //

        if (_wcsicmp(FatpLongFileName, FileName) == 0) {
            return STATUS_SUCCESS;
        }

        Status = FatpGetNextFileNameDirent(DirectoryEntry,
                                           Dirent,
                                           FatpLongFileName,
                                           DirentLbo);
    }

    return Status;
}

NTSTATUS
FatpGetNextDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __out PDIRENT Dirent,
    __out PLBO DirentLbo
    )

/*++

Routine Description:

    Returns the dirent at the current file offset for the directory described
    bye DirectoryEntry.  If the current file offset is not aligned on a dirent
    boundary, the file offset will be rounded down.

    This routine assumes that DirectoryEntry is indeed a directory.

Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    Dirent - An address to store the dirent, if it exists.

    DirentLbo - An address to store the file offset to the dirent for the
        file, if it exists.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if file offset is at the EOF or if the next dirent is
        invalid.

    Other error if a device read fails:
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PFAT_FILE DirInfo;
    ULONG ReadCount;
    ULONG64 RoundedFileSize;
    NTSTATUS Status;

    DirInfo = (PFAT_FILE)DirectoryEntry->FileData;

    ASSERT(CHECK_FLAG(DirInfo->Information.Flags, FILE_FLAGS_IS_DIRECTORY));

    //
    // If the current file pointer is not on a dirent boundary, round the file
    // pointer down to the nearest dirent boundary.
    //

    DirInfo->Information.FileOffset =
        ROUND_DOWN(DirInfo->Information.FileOffset, sizeof(DIRENT));

    //
    // The dirent at the rounded file offset can extend past the end of the
    // file if this is a directory that runs all the way to the end of its
    // cluster chain without containing any terminator dirents.  Return a file
    // not found status in this case.
    //

    RoundedFileSize = ROUND_DOWN(DirInfo->Information.FileSize, sizeof(DIRENT));
    if (DirInfo->Information.FileOffset >= RoundedFileSize) {
        return STATUS_NO_SUCH_FILE;
    }

    *DirentLbo = DirInfo->Information.FileOffset;

    //
    // Read the next dirent from the directory.
    //
    // N.B. BlFileRead cannot be used here since it only allows access to file
    //      data and not directory data.
    //

    Status = FatpIoOperation(DirectoryEntry,
                             (PUCHAR)Dirent,
                             sizeof(DIRENT),
                             &ReadCount,
                             TRUE);

    //
    // If we were successful, check to see if this was the last dirent.
    //

    if ((Status == STATUS_SUCCESS) &&
        (Dirent->FileName[0] == FAT_DIRENT_NEVER_USED)) {

        Status = STATUS_NO_SUCH_FILE;
    }

    return Status;
}

NTSTATUS
FatpGetNextFileNameDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __out PDIRENT Dirent,
    __out_ecount(FAT_MAX_LFN_LENGTH + 1) PWSTR FileName,
    __out PLBO DirentLbo
    )

/*++

Routine Description:

    Returns the next file dirent, using the current file pointer as the
    starting offset when searching for the next file dirent.

    A file dirent is defined as a dirent for a file or directory.  This does
    not include a dirent for a long file name or a dirent for the volume
    identifier.


Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    Dirent - An address to store the dirent, if it exists.

    Lfn - A buffer into which the long file name (if present) will be copied.

    DirentLbo - An address to store the file offset to the dirent for the
        file, if it exists.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_FILE if file offset is at the EOF or if the next dirent is
        invalid.

    Other error if a device read fails:
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    UCHAR ActualChecksum;
    ULONG CharacterCount;
    UCHAR Checksum;
    ULONG ChecksumFlags;
    PWSTR Destination;
    UCHAR LastOrdinal;
    BOOLEAN LfnInProgress;
    UCHAR Ordinal;
    PCHAR Source;
    NTSTATUS Status;

    LfnInProgress = FALSE;
    Checksum = 0;
    LastOrdinal = 0;

    //
    // This loop only terminates in two cases:
    //
    // 1. FatpGetNextDirent has failed, meaning that there are no more directory
    //    entries.  The status code will be a failure code.
    //
    // 2. The next short entry is found, in which case the status code is set to
    //    STATUS_SUCCESS.
    //
    // Invalid directory entries are ignored.
    //

    do {
        Status = FatpGetNextDirent(DirectoryEntry, Dirent, DirentLbo);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        //
        // If a deleted entry is encountered, orphan any partially constructed
        // long file names.
        //

        if (IS_DIRENT_DELETED(Dirent)) {
            LfnInProgress = FALSE;
            continue;
        }

        //
        // If the dirent is a long filename dirent, then copy out the proper
        // characters.  Long filenames are chained to the short filename dirent
        // structure.  They are in reverse order, and appear before the
        // corresponding short entry.  The LFN dirent records must be in order,
        // and there cannot be more than 20 of then describing one long
        // filename.  The checksum field in the LFN records must agree, and must
        // be equal to the checksum of the filename in the corresponding short
        // dirent.  If any of these rules are broken, then the partially
        // constructed LFN dirent records must be ignored.
        //

        if (IS_LONGFILENAME_DIRENT(Dirent)) {
            if (IS_LAST_LONGFILENAME_DIRENT(Dirent)) {
                Checksum = GET_LONGFILENAME_CHECKSUM(Dirent);
                LastOrdinal = GET_LONGFILENAME_ORDINAL(Dirent);
                LfnInProgress = TRUE;

            } else {
                if (LfnInProgress == FALSE) {
                    continue;
                }

                Ordinal = GET_LONGFILENAME_ORDINAL(Dirent);

                //
                // The ordinals must be in order.
                //

                if (Ordinal != (LastOrdinal - 1)) {
                    LfnInProgress = FALSE;
                    continue;
                }

                //
                // The checksum fields must match in all entries for the same
                // long filename.
                //

                if (Checksum != GET_LONGFILENAME_CHECKSUM(Dirent)) {
                    LfnInProgress = FALSE;
                    continue;
                }

                LastOrdinal = Ordinal;
            }

            //
            // Copy the partial long filename into the user-supplied buffer.
            //

            Status = FatpCopyPartialLfn(Dirent, FileName);
            if (!NT_SUCCESS(Status)) {
                LfnInProgress = FALSE;
            }

            continue;
        }

        //
        // If a volume label entry is encountered, orphan any partially
        // constructed long file names.
        //

        if (IS_VOLUME_ID_DIRENT(Dirent)) {
            LfnInProgress = FALSE;
            continue;
        }

        //
        // At this point, the dirent can only be a regular short entry. Decide
        // whether a valid Lfn has been found.  If there is no long filename,
        // then copy the short filename into the filename buffer.
        //

        ChecksumFlags = CHECKSUM_BYTE_ARRAY | CHECKSUM_ROTATE_RIGHT;
        ActualChecksum = (UCHAR)BlUtlCheckSum(0,
                                              &Dirent->FileName,
                                              11,
                                              ChecksumFlags);

        if ((LfnInProgress == FALSE) || (LastOrdinal != 1) ||
            (ActualChecksum != Checksum)) {

            Source = (PCHAR)&Dirent->FileName[0];
            Destination = FileName;
            for (CharacterCount = 0; CharacterCount < 8; CharacterCount += 1) {
                if (*Source == ' ') {
                    break;
                }

                *Destination = (WCHAR)*Source;
                Destination += 1;
                Source += 1;
            }

            Source = (PCHAR)&Dirent->FileName[8];
            if (*Source != ' ') {
                *Destination = L'.';
                Destination += 1;
            }

            for (CharacterCount = 0; CharacterCount < 3; CharacterCount += 1) {
                if (*Source == ' ') {
                    break;
                }

                *Destination = (WCHAR)*Source;
                Destination += 1;
                Source += 1;
            }

            *Destination = UNICODE_NULL;

            //
            // If the first character of the short filename is the biased
            // version of 0xe5, then change the filename to report the correct
            // first character.
            //

            if (*FileName == FAT_DIRENT_REALLY_0E5) {
                *FileName = 0xe5;
            }
        }

        Status = STATUS_SUCCESS;
        break;

    } while (TRUE);

    return Status;
}

ULONGLONG
FatpGetDirectorySize (
    __in PFILE_ENTRY DirEntry
    )

/*++

Routine Description:

    Determines the Directory Size by reading each dirent, looking for the entry
    whose first character is FAT_DIRENT_NEVER_USED.

Arguments:

    DirEntry - File Entry for the directory.

Return Value:

    The size of the Directory.

--*/

{

    DIRENT Dirent;
    PFAT_FILE DirInfo;
    ULONGLONG DirSize;
    ULONG ReadCount;
    NTSTATUS Status;

    DirInfo = (PFAT_FILE)DirEntry->FileData;

    ASSERT(CHECK_FLAG(DirInfo->Information.Flags, FILE_FLAGS_IS_DIRECTORY));
    ASSERT(DirInfo->FatDeviceTableId < FatDeviceTableEntries);

    //
    // Walk each Cluster looking for the last Dirent.
    //
    // N.B. BlFileRead can not be used since FileSize has not yet been
    //      initialized (which is precisely what this routine is doing).
    //
    // N.B. FatRead can not be used since it only allows access to files and
    //      not directories.
    //

    FatpResetCurrentClusterPointer(DirInfo);
    for (DirSize = 0, Dirent.FileName[0] = (UCHAR)~FAT_DIRENT_NEVER_USED;
         (Dirent.FileName[0] != FAT_DIRENT_NEVER_USED);
         DirSize += sizeof(DIRENT)) {

        Status = FatpIoOperation(DirEntry,
                                 (PUCHAR)&Dirent,
                                 sizeof(Dirent),
                                 &ReadCount,
                                 TRUE);

        if (!NT_SUCCESS(Status)) {
            break;
        }
    }

    FatpResetCurrentClusterPointer(DirInfo);

    //
    // Return the calculated directory size.
    //

    return DirSize;
}

LBO
FatpIndexToLbo (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in FAT_ENTRY FatIndex
    )

/*++

Routine Description:

    This routine will determine the Logical Byte Offset of the cluster index
    in the FAT with FatIndex.

    Since the File Allocation Table has an entry for each cluster (starting
    with cluster 2) to describe the Data portion of the disk/device, the
    LBO can be determined by multiplying the cluster index by the number
    of bytes per sector.  Additionally, the first two clusters and any
    reserved sectors will need to be taken into account.

Arguments:

    FatContext - Fat Context for the current device.

    FatIndex - Cluster index in the File Allocation Table.

Return Value:

    The logical byte offset to the cluster described by FatIndex.

--*/

{

    //
    // Index 0 + 1 are reserved.  FatFileAreaLbo is the byte offset to the
    // first data cluster.
    //

    LBO ByteOffset;
    LBO Mask;

    //
    // Properly handle any overflow.
    //

    switch (FatIndexBitSize(&FatContext->Bpb)) {
    case 12:
        Mask = ((LBO)0x1000 * FatBytesPerCluster(&FatContext->Bpb)) - 1;
        break;

    case 32:
        Mask = ((LBO)0x100000000 * FatBytesPerCluster(&FatContext->Bpb)) - 1;
        break;

    default:
        Mask = ((LBO)0x10000 * FatBytesPerCluster(&FatContext->Bpb)) - 1;
        break;
    }

    ByteOffset = ((FatIndex-2) * (LBO)FatBytesPerCluster(&FatContext->Bpb)) +
                 FatFileAreaLbo(&FatContext->Bpb);

    return (ByteOffset & Mask);
}

FAT_ENTRY
FatpLboToIndex (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in LBO ClusterOffset
    )

/*++

Routine Description:

    The inverse of FatpIndexToLbo.  Given a byte offset to a Fat Entry,
    this routine will return the index into the cluster index in the
    File Allocation Table.

    Since the File Allocation Table has an entry for each cluster (starting
    with cluster 2) to describe the Data portion of the disk/device, the
    LBO can be determined by multiplying the cluster index by the number
    of bytes per sector.  Additionally, the first two clusters and any
    reserved sectors will need to be taken into account.

Arguments:

    FatContext - Fat Context for the current device.

    ClusterOffset - Byte offset to a cluster.

Return Value:

    The FAT index for the specified cluster.

--*/

{

    FAT_ENTRY FatEntry;

    FatEntry = (FAT_ENTRY)(((ClusterOffset - FatFileAreaLbo(&FatContext->Bpb))
                            / FatBytesPerCluster(&FatContext->Bpb)) + 2);

    //
    // Determine the size of the FAT Entry to mask off high bits.
    //

    switch (FatIndexBitSize(&FatContext->Bpb)) {
    case 12:
        FatEntry &= 0x0fff;          // A FAT entry is 12 bits (1.5 bytes).
        break;

    case 32:
        FatEntry &= 0xffffffff;      // A FAT entry is 32 bits (4 bytes).
        break;

    default:
        FatEntry &= 0xffff;          // A FAT entry is 16 bits (2 bytes).
        break;
    }

    return FatEntry;
}

FAT_ENTRY
FatpFetchFatEntry (
    __in PFILE_ENTRY FileEntry,
    __in FAT_ENTRY FatIndex
    )

/*++

Routine Description:

    This routine will return the value stored in the File Allocation Table at
    index, FatIndex.  It is equivalent to the following,

        x = FAT[FatIndex];

Arguments:

    FileEntry - FileEntry for the caller of this routine.  The file entry is
        required in order to get a pointer to the device's FAT context
        structure and also for the device/file id of the parent device in the
        case a differenct window of the File Allocation Table needs to be read.

Return Value:

    The value stored at in the file allocation table at index, FatIndex.

--*/

{

    CLUSTER_TYPE ClusterType;
    DEVICE_INFORMATION DeviceInformation;
    PFAT_DEVICE_CONTEXT FatContext;
    FAT_ENTRY FatEntry;
    PFAT_FILE FileInfo;
    BOOLEAN IsFat12;
    ULONG Offset;
    ULONG OriginalIoAccessFlags;
    ULONG Size;
    NTSTATUS Status;

    FileInfo = (PFAT_FILE)FileEntry->FileData;
    FatContext = FatDeviceTable[FileInfo->FatDeviceTableId];

    //
    // Look at the cluster type of the current FAT entry.  If the cluster is
    // reserved, this is likely the root directory.  In this case, just return
    // the next reserved FAT index.
    //

    ClusterType = FatpInterpretClusterType(FatContext, FatIndex);
    if (ClusterType == FatClusterReserved) {
        return FatIndex + 1;
    }

    //
    // Determine the offset in the File Allocation Table, provided the size of
    // a FAT entry.
    //

    switch (FatIndexBitSize(&FatContext->Bpb)) {
    case 12:
        IsFat12 = TRUE;
        Offset = (FatIndex * 3) / 2;  // A FAT entry is 12 bits (1.5 bytes).
        Size = 2; // 2 bytes must be available in a window.
        break;

    case 32:
        IsFat12 = FALSE;
        Offset = FatIndex * 4;        // A FAT entry is 32 bits (4 bytes).
        Size = 4; // 4 bytes must be available in a window.
        break;

    default:
        IsFat12 = FALSE;
        Offset = FatIndex * 2;        // A FAT entry is 16 bits (2 bytes).
        Size = 2; // 2 bytes must be available in a window.
        break;
    }

    //
    // Only a fraction of the File Allocation Table is kept in memory.  Check
    // to see if FAT[FatIndex] is in memory.  If not, read the table starting
    // at Offset.
    //

    if ((Offset < FatContext->FatWindowOffset) ||
        (FatContext->FatWindowOffset + FAT_WINDOW_SIZE - Size <= Offset)) {

        //
        // If the FAT window in memory is dirty, write it back to disk before
        // reading the new window into memory.
        //

        if (FatContext->FatWindowDirty != FALSE) {
            Status = FatpWindowFlush(FileEntry);
            if (!NT_SUCCESS(Status)) {
                return 0;
            }
        }

        //
        // Read the new window of the file allocation table into memory.
        //

        Status = BlDeviceGetInformation(FileEntry->DeviceId,
                                        &DeviceInformation);

        ASSERT(Status == STATUS_SUCCESS);

        OriginalIoAccessFlags = DeviceInformation.u.BlockIo.IoAccessFlags;
        SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                  BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

        Status = BlDeviceSetInformation(FileEntry->DeviceId,
                                        &DeviceInformation);

        ASSERT(Status == STATUS_SUCCESS);

        FatContext->FatWindowOffset = Offset;
        Status = BlDeviceReadAtOffset(FileEntry->DeviceId,
                                      FatLbo(&FatContext->Bpb) + Offset,
                                      FAT_WINDOW_SIZE,
                                      FatContext->FatWindow,
                                      NULL);

        DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
        BlDeviceSetInformation(FileEntry->DeviceId, &DeviceInformation);
        if (!NT_SUCCESS(Status)) {
            return 0;
        }
    }

    //
    // At this point, the window of the file allocation table that is in memory
    // contains FatIndex.
    //

    if (IsBpbFat32(&FatContext->Bpb)) {
        CopyUchar4(&FatEntry,
                   &FatContext->FatWindow[Offset - FatContext->FatWindowOffset]);

    } else {
        FatEntry = 0;
        CopyUchar2(&FatEntry,
                   &FatContext->FatWindow[Offset - FatContext->FatWindowOffset]);

        //
        // If this is FAT12, mask off the high 4 bits of the FatEntry (since we
        // copied 16 bits).  If the FatIndex is odd, then the FatEntry also
        // needs to be right shifted by 4.
        //

        if (IsFat12 != FALSE) {
            if ((FatIndex % 2) == 1) {
                FatEntry = FatEntry >> 4;
            }

            FatEntry &= 0xFFF;
        }
    }

    return FatEntry;
}

NTSTATUS
FatpWindowFlush (
    __in PFILE_ENTRY FileEntry
    )

/*++

Routine Description:

    Writes the portion of the file allocation table that is in memory to disk.

Arguments:

    FileEntry - Entry in file table for caller making the request.

Return Value:

    STATUS_SUCCESS when successful.

    Error from disk read if error is encountered:
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    PFAT_DEVICE_CONTEXT FatContext;
    PFAT_FILE FileInfo;
    NTSTATUS Status;

    FileInfo = (PFAT_FILE)FileEntry->FileData;
    FatContext = FatDeviceTable[FileInfo->FatDeviceTableId];

    //
    // Write the File Allocation Table to disk.
    //

    Status = BlDeviceWriteAtOffset(FileEntry->DeviceId,
                                   FatLbo(&FatContext->Bpb) +
                                   FatContext->FatWindowOffset,
                                   FAT_WINDOW_SIZE,
                                   FatContext->FatWindow,
                                   NULL);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    FatContext->FatWindowDirty = FALSE;
    return STATUS_SUCCESS;
}

NTSTATUS
FatpIoOperation (
    __in PFILE_ENTRY FileEntry,
    __inout_bcount(Size) PUCHAR Buffer,
    __in ULONG Size,
    __out PULONG Count,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

    A helper function for FatRead and FatWrite.  This routine will walk the
    File Allocation Table entries for a given file in order to perform an
    I/O operation.

    The walk begins at the FAT entry described by the file's current Lbo.
    If the entire file chain should be walked, the file's
    FileContext->CurrentClusterLbo should equal FileContext->FileLbo.

Arguments:

    FileEntry - File entry structure for the caller.

    Buffer - Transfer buffer for I/O operation.

    Size - The number of bytes to transfer.

    Count - A pointer to an address write the number of bytes actually
        transfered.

    ReadOperation - Boolean value indicating if the I/O operation is a read.
        If not, the I/O operation is assumed to be a write.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    ULONG BytesPerCluster;
    LBO ClusterLbo;
    CLUSTER_TYPE ClusterType;
    PFAT_DEVICE_CONTEXT DeviceContext;
    DEVICE_INFORMATION DeviceInformation;
    ULONG64 Difference;
    FAT_ENTRY FatEntry;
    PFAT_FILE FatFile;
    PFAT_FILE_CONTEXT FileContext;
    ULONGLONG FileEnd;
    ULONGLONG FilePointer;
    LBO FirstClusterLbo;
    LBO LastClusterLbo;
    ULONG OperationCount;
    ULONGLONG OperationOffset;
    ULONG OperationSize;
    ULONG OriginalIoAccessFlags;
    ULONGLONG RelativeClusterStart;
    ULONGLONG RequestEnd;
    ULONGLONG RequestStart;
    NTSTATUS Status;

    FatFile = (PFAT_FILE)FileEntry->FileData;
    DeviceContext = FatDeviceTable[FatFile->FatDeviceTableId];
    FileContext = &FatFile->FileContext;
    OperationCount = 0;
    Status = STATUS_UNSUCCESSFUL;

    //
    // A FAT32 root directory is just like any other directory.  It is of
    // variable size and is a cluster chain, just like any other directory.
    // However, FAT12 and FAT16 have just one region for the root directory.
    // Fork the code if this is a root directory for FAT12 or FAT16.
    //

    if (!IsBpbFat32(&DeviceContext->Bpb) &&
        (FileEntry->FilePath[1] == L'\0')) {

        if (FatFile->Information.FileOffset + Size <=
            FatFile->Information.FileSize) {

            OperationOffset =
                FileContext->FileLbo + FatFile->Information.FileOffset;

            //
            // Perform I/O operation on Root Directory.
            //

            if (ReadOperation != FALSE) {
                OriginalIoAccessFlags = 0;
                if (CHECK_FLAG(FatFile->Information.Flags,
                               FILE_FLAGS_IS_DIRECTORY) != FALSE) {

                    Status = BlDeviceGetInformation(FileEntry->DeviceId,
                                                    &DeviceInformation);

                    OriginalIoAccessFlags =
                        DeviceInformation.u.BlockIo.IoAccessFlags;

                    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

                    Status = BlDeviceSetInformation(FileEntry->DeviceId,
                                                    &DeviceInformation);

                    ASSERT(Status == STATUS_SUCCESS);

                }

                Status = BlDeviceReadAtOffset(FileEntry->DeviceId,
                                              OperationOffset,
                                              Size,
                                              Buffer,
                                              NULL);

                if (CHECK_FLAG(FatFile->Information.Flags,
                               FILE_FLAGS_IS_DIRECTORY) != FALSE) {

                    DeviceInformation.u.BlockIo.IoAccessFlags =
                        OriginalIoAccessFlags;

                    BlDeviceSetInformation(FileEntry->DeviceId,
                                           &DeviceInformation);
                }

            } else {
                Status = BlDeviceWriteAtOffset(FileEntry->DeviceId,
                                               OperationOffset,
                                               Size,
                                               Buffer,
                                               NULL);

            }
        }

        if (Status == STATUS_SUCCESS) {
            OperationCount = Size;
            FatFile->Information.FileOffset += Size;
        }

    } else {

        //
        // Initialize some local variables.
        //

        BytesPerCluster = FatBytesPerCluster(&DeviceContext->Bpb);
        FirstClusterLbo = FileContext->CurrentClusterLbo;
        OperationOffset = 0;
        OperationSize = 0;
        OperationCount = 0;
        FilePointer = FileContext->CurrentClusterFileOffset;
        FileEnd = FatFile->Information.FileSize;
        RequestStart = FatFile->Information.FileOffset;
        RequestEnd = RequestStart + Size;

        //
        // This case should be handled by a caller.
        //

        ASSERT(FilePointer <= RequestStart);

        //
        // Whether this is a FAT32 root directory or any other directory,
        // walk the File Allocation Table entries for the file.  If the
        // current file offset lies within the current cluster, perform the
        // I/O operation on as much of the desired range as possible.
        //

        //
        // Determine the initialize FAT entry and the associated offset
        // into the volume.
        //

        FatEntry = FatpLboToIndex(DeviceContext,
                                  FileContext->CurrentClusterLbo);

        ClusterType = FatpInterpretClusterType(DeviceContext, FatEntry);
        ClusterLbo = FatpIndexToLbo(DeviceContext, FatEntry);
        while ((ClusterType != FatClusterAvailable) &&
               (ClusterType != FatClusterBad) &&
               (ClusterType != FatClusterLast) &&
               (OperationCount < Size)) {

            //
            // Determine if any of the requested region overlaps with the
            // current cluster.
            //

            RelativeClusterStart = FilePointer;
            if ((FilePointer < RequestEnd) &&
                (RequestStart < FilePointer + BytesPerCluster)) {

                //
                // If this is the first cluster that contains part of the
                // requested region, set the starting offset of the read /
                // write operation.
                //

                if (FilePointer <= RequestStart) {
                    OperationOffset = ClusterLbo + RequestStart - FilePointer;
                    RelativeClusterStart = RequestStart;
                }

                //
                // Adjust the end LBO for the read / write operation.
                //

                OperationSize += (ULONG)(min(RequestEnd,
                                             FilePointer + BytesPerCluster) -
                                         RelativeClusterStart);

            }

            ASSERT(OperationSize + OperationCount <= Size);

            //
            // Determine the next cluster in the file.
            //

            LastClusterLbo = ClusterLbo;
            FatEntry = FatpFetchFatEntry(FileEntry, FatEntry);
            ClusterLbo = FatpIndexToLbo(DeviceContext, FatEntry);
            ClusterType = FatpInterpretClusterType(DeviceContext, FatEntry);

            //
            // If the next cluster is not adjacent to the current cluster
            // or if the end of file or end of request were hit, perform the
            // bundled I/O operation.
            //

            if (((ClusterType == FatClusterLast) ||
                 (ClusterLbo != LastClusterLbo + BytesPerCluster) ||
                 (Size <= OperationSize + OperationCount)) &&
                (OperationSize != 0)) {

                if (ReadOperation != FALSE) {
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "This has been checked above");

                    OriginalIoAccessFlags = 0;
                    if (CHECK_FLAG(FatFile->Information.Flags,
                                   FILE_FLAGS_IS_DIRECTORY) != FALSE) {

                        Status = BlDeviceGetInformation(FileEntry->DeviceId,
                                                        &DeviceInformation);

                        OriginalIoAccessFlags =
                            DeviceInformation.u.BlockIo.IoAccessFlags;

                        SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                                  BLOCK_IO_ACCESS_FLAGS_CACHE_READ);

                        Status = BlDeviceSetInformation(FileEntry->DeviceId,
                                                        &DeviceInformation);

                        ASSERT(Status == STATUS_SUCCESS);

                    }

                    //
                    // Cache if directory.
                    //

                    Status = BlDeviceReadAtOffset(FileEntry->DeviceId,
                                                  OperationOffset,
                                                  OperationSize,
                                                  Buffer,
                                                  NULL);

                    if (CHECK_FLAG(FatFile->Information.Flags,
                                   FILE_FLAGS_IS_DIRECTORY) != FALSE) {

                        DeviceInformation.u.BlockIo.IoAccessFlags =
                            OriginalIoAccessFlags;

                        BlDeviceSetInformation(FileEntry->DeviceId,
                                               &DeviceInformation);
                    }

                } else {
                    Status = BlDeviceWriteAtOffset(FileEntry->DeviceId,
                                                   OperationOffset,
                                                   OperationSize,
                                                   Buffer,
                                                   NULL);

                }

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                //
                // Update pointers.
                //

                Buffer = Add2Ptr(Buffer, OperationSize);
                FatFile->Information.FileOffset += OperationSize;
                RequestStart += OperationSize;

                //
                // Update the current FAT cluster pointer.
                //

                if (((OperationOffset + OperationSize) - FirstClusterLbo) &
                    (BytesPerCluster - 1)) {

                    //
                    // The read operation did not end on a cluster boundary.
                    // Round down the offset to the start of the last cluster
                    // read.
                    //

                    Difference =
                        OperationOffset + OperationSize - FirstClusterLbo;

                    FileContext->CurrentClusterLbo +=
                        ALIGN_RANGE_DOWN(Difference, BytesPerCluster);

                } else {

                    //
                    // The read operation ended on a cluster boundary.  The
                    // next read should start with the next cluster.
                    //

                    FileContext->CurrentClusterLbo = ClusterLbo;
                    FirstClusterLbo = ClusterLbo;
                }

                //
                // Round the current file position down to a cluster boundary
                // to find the current cluster offset.
                //

                FileContext->CurrentClusterFileOffset =
                    ALIGN_RANGE_DOWN(FatFile->Information.FileOffset,
                                     BytesPerCluster);

                //
                // Update local values.
                //

                OperationCount += OperationSize;
                OperationOffset = 0;
                OperationSize = 0;

                //
                // Exit the read / write loop if the end of file or end
                // of operation were hit.
                //

                if ((ClusterType == FatClusterLast) ||
                    (Size <= OperationSize + OperationCount)) {

                    break;
                }
            }

            FilePointer += BytesPerCluster;
            ASSERT(CHECK_FLAG(FatFile->Information.Flags,
                              FILE_FLAGS_IS_DIRECTORY) ||
                   (FilePointer <= FileEnd));

        }
    }

    //
    // We are done.  Return to the caller.  If we succeeded, the status code
    // was set as we broke out of the above loop.
    //

    if (Count != NULL) {
        *Count = OperationCount;
    }

    return Status;
}

CLUSTER_TYPE
FatpInterpretClusterType (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in FAT_ENTRY Entry
    )

/*++

Routine Description:

    This procedure tells the caller how to interpret a fat table entry.  It
    will indicate if the fat cluster is available, reserved, bad, the last in
    a cluster chain or another fat index.

Arguments:

    FatContext - Pointer to the Fat device context for the device the FAT is
        for.

    Entry - File Allocation Table entry to examine.

Return Value:

    The type of cluster Entry refers to.

--*/

{

    //
    // The special FAT values are similar for each FAT type, only their values
    // will vary based on the size of the FAT.
    //

    switch (FatIndexBitSize(&FatContext->Bpb)) {

    //
    // FAT12
    //

    case 12:
        Entry &= 0x0fff;
        if (Entry == 0x000) {
            return FatClusterAvailable;

        } else if (Entry >= 0xff0 && Entry <= 0xff6) {
            return FatClusterReserved;

        } else if (Entry == 0xff7) {
            return FatClusterBad;

        } else if (Entry >= 0xff8 && Entry <= 0xfff) {
            return FatClusterLast;

        } else {
            return FatClusterNext;
        }

    //
    // FAT32
    //

    case 32:
        Entry &= 0x0fffffff;
        if (Entry == 0x0000) {
            return FatClusterAvailable;

        } else if (Entry == 0x0ffffff7) {
            return FatClusterBad;

        } else if (Entry >= 0x0ffffff8 && Entry <= 0x0fffffff) {
            return FatClusterLast;

        } else {
            return FatClusterNext;
        }

    //
    // FAT16
    //

    default:
        Entry &= 0xffff;
        if (Entry == 0x0000) {
            return FatClusterAvailable;

        } else if (Entry >= 0xfff0 && Entry <= 0xfff6) {
            return FatClusterReserved;

        } else if (Entry == 0xfff7) {
            return FatClusterBad;

        } else if (Entry >= 0xfff8 && Entry <= 0xffff) {
            return FatClusterLast;

        } else {
            return FatClusterNext;
        }
    }
}

VOID
FatpResetCurrentClusterPointer (
    __in PFAT_FILE FatFile
    )

/*++

Routine Description:

    This routine will reset a file's internal data which maintains record
    of the current FAT cluster pointer for a file.  This internal data is used
    for performance when performing file I/O, eliminating the need to
    walk the entire FAT cluster chain for every I/O operation.

Arguments:

    FatFile - Internal data structure for recording file information.

Return Value:

    none.

--*/

{

    FatFile->FileContext.CurrentClusterLbo = FatFile->FileContext.FileLbo;
    FatFile->FileContext.CurrentClusterFileOffset = 0;
    return;
}

VOID
FatpPrintFatWindow (
    __in PFILE_ENTRY FileEntry,
    __in ULONG FatWindowSize
    )

/*++

Routine Description:

    A debug routine to print the entries of the file allocation table that
    are currently in memory.

Arguments:

    FileEntry - File entry of the caller.

Return Value:

    none.

--*/

{

    PFAT_DEVICE_CONTEXT FatContext;
    FAT_ENTRY FatEntry;
    PFAT_FILE FatFile;
    FAT_ENTRY FirstEntry;
    FAT_ENTRY Id;
    FAT_ENTRY LastEntry;

    FatFile = (PFAT_FILE)FileEntry->FileData;
    FatContext = FatDeviceTable[FatFile->FatDeviceTableId];
    switch (FatIndexBitSize(&FatContext->Bpb)) {
    case 12:
        FirstEntry = (FatContext->FatWindowOffset * 2) / 3;
        LastEntry = FirstEntry + ((FatWindowSize * 2) / 3);
        break;

    case 32:
        FirstEntry = FatContext->FatWindowOffset / 4;
        LastEntry = FirstEntry + (FatWindowSize / 4);
        break;

    default:
        FirstEntry = FatContext->FatWindowOffset / 2;
        LastEntry = FirstEntry + (FatWindowSize / 2);
        break;
    }

    BlDisplayPrintString(L"FAT Window First Entry: %x\r\n", FirstEntry);
    for (Id = FirstEntry; Id < LastEntry; Id += 1) {
        FatEntry = FatpFetchFatEntry(FileEntry, Id);
        BlDisplayPrintString(L"FAT[%x] = %x\r\n", Id, FatEntry);
    }

    return;
}

VOID
FatpPrintBiosParameterBlock (
    __in PBIOS_PARAMETER_BLOCK Bpb
    )

/*++

Routine Description:

    A debug routine to print the contents of a bios parameter block.

Arguments:

    Bpb - The bios parameter block to print.

Return Value:

    none.

--*/

{

    BlDisplayPrintString(L"BytesPerSector : %d\r\n", Bpb->BytesPerSector);
    BlDisplayPrintString(L"SectorsPerCluster : %d\r\n", Bpb->SectorsPerCluster);
    BlDisplayPrintString(L"ReservedSectors : %d\r\n", Bpb->ReservedSectors);
    BlDisplayPrintString(L"Fats : %d\r\n", Bpb->Fats);
    BlDisplayPrintString(L"RootEntries : %d\r\n", Bpb->RootEntries);
    BlDisplayPrintString(L"Sectors : %d\r\n", Bpb->Sectors);
    BlDisplayPrintString(L"Media : %d\r\n", Bpb->Sectors);
    BlDisplayPrintString(L"SectorsPerFat : %d\r\n", Bpb->SectorsPerFat);
    BlDisplayPrintString(L"SectorsPerTrack : %d\r\n", Bpb->SectorsPerTrack);
    BlDisplayPrintString(L"LargeSectors : %d\r\n", Bpb->LargeSectors);

#if 0

    BlDisplayPrintString(L"LargeSectorsPerFat : %d\r\n",
                         Bpb->LargeSectorsPerFat);

    BlDisplayPrintString(L"RootDirFirstCluster : %d\r\n",
                         Bpb->RootDirFirstCluster);

#endif

    return;
}

VOID
FatpPrint8Dot3 (
    __in PFAT8DOT3 Name8Dot3
    )

/*++

Routine Description:

    A debug routine to print an 8 dot 3 filename.

Arguments:

    Name8Dot3 - Pointer to FAT8DOT3 structure.

Return Value:

    none.

--*/

{

    BlDisplayPrintString(L"%x %x %x %x %x %x %x %x %x %x %x\r\n",
                         (*Name8Dot3)[0],
                         (*Name8Dot3)[1],
                         (*Name8Dot3)[2],
                         (*Name8Dot3)[3],
                         (*Name8Dot3)[4],
                         (*Name8Dot3)[5],
                         (*Name8Dot3)[6],
                         (*Name8Dot3)[7],
                         (*Name8Dot3)[8],
                         (*Name8Dot3)[9],
                         (*Name8Dot3)[10]);

    return;
}

VOID
FatpPrintDirectoryEntries (
    __in PFILE_ENTRY DirectoryEntry
    )

/*++

Routine Description:

    A debug routine to print the 8 dot 3 dirent filename's for each dirent
    present.

Arguments:

    DirectoryEntry - FileEntry for the directory.

Return Value:

    none.

--*/

{

    PFAT_FILE DirectoryInformation;
    DIRENT Dirent;
    LBO DirentOffset;
    NTSTATUS Status;

    DirectoryInformation = (PFAT_FILE)DirectoryEntry->FileData;
    ASSERT(CHECK_FLAG(DirectoryInformation->Information.Flags,
                      FILE_FLAGS_IS_DIRECTORY));

    BlDisplayPrintString(L"Dirent entries for %ls\r\n",
                         DirectoryEntry->FilePath);

    //
    // Reset the file offset, and start reading each DIRENT from the start of
    // the directory.
    //

    DirectoryInformation->Information.FileOffset = 0;
    FatpResetCurrentClusterPointer(DirectoryInformation);

    Status = FatpGetNextDirent(DirectoryEntry, &Dirent, &DirentOffset);
    while (NT_SUCCESS(Status)) {

        //
        // Print Dirent's Filename.
        //

        FatpPrint8Dot3(&Dirent.FileName);
        BlDisplayPrintString(L"Attributes: 0x%x\r\n", Dirent.Attributes);

        //
        // Get next dirent.
        //

        Status = FatpGetNextDirent(DirectoryEntry, &Dirent, &DirentOffset);
    }

    //
    // Reset file pointer again..
    //

    DirectoryInformation->Information.FileOffset = 0;
    FatpResetCurrentClusterPointer(DirectoryInformation);
}

//
// FAT Device Table Entry Routines.
//

NTSTATUS
FatpDeviceTableDestroyEntry (
    __in PFAT_DEVICE_CONTEXT FatContext,
    __in ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the fat device table.  This includes freeing the
    memory allocated for the entry as well as zeroing the entry in the device
    table.

Arguments:

    FatContext - Entry in the FAT device table to destroy.

    Index - Index in the device table that points to FatContext.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    //
    // Free the entry's memory.
    //

    BlMmFreeHeap(FatContext);
    FatDeviceTable[Index] = NULL;
    return STATUS_SUCCESS;
}

NTSTATUS
FatpCopyPartialLfn (
    __in PDIRENT Dirent,
    __out_ecount(FAT_MAX_LFN_LENGTH + 1) PWCHAR Lfn
    )

/*++

Routine Description:

    This routine will copy the long file name portion stored in the supplied
    dirent.  The function assumes that this is actually a LFN_DIRENT structure.

Arguments:

    Dirent - Supplies the Dirent structure whose LFN component will be copied
        into the provided buffer.

    Lfn - Supplies the string buffer into which the LFN component will be
        copied.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    PWCHAR Index;
    PLFN_DIRENT LfnDirent;
    ULONG Ordinal;
    NTSTATUS Status;
    BOOLEAN Terminate;

    Terminate = FALSE;
    Ordinal = GET_LONGFILENAME_ORDINAL(Dirent);
    Index = Lfn + ((Ordinal - 1) * 13);

    //
    // Ordinal must be non-zero, and cannot be higher than the number required
    // to represent the maximum length of a long filename.
    //

    if ((Ordinal == 0) || (Ordinal > FAT_MAX_LFN_ENTRIES)) {
        Status = STATUS_INVALID_PARAMETER;
        goto CopyPartialLfnEnd;
    }

    //
    // Copy the pieces of the long filename into a temporary location, ensuring
    // that the string is correctly terminated when assembly is complete.
    //

    LfnDirent = (PLFN_DIRENT)Dirent;
    if (Ordinal == FAT_MAX_LFN_ENTRIES) {

        //
        // Only eight characters can be specified in the 20th ordinal.
        //

        RtlCopyMemory(&Index[0], LfnDirent->Name1, 10);
        RtlCopyMemory(&Index[5], LfnDirent->Name2, 6);
        Index[8] = UNICODE_NULL;

    } else {
        RtlCopyMemory(&Index[0], LfnDirent->Name1, 10);
        RtlCopyMemory(&Index[5], LfnDirent->Name2, 12);
        RtlCopyMemory(&Index[11], LfnDirent->Name3, 4);
        if (IS_LAST_LONGFILENAME_DIRENT(Dirent)) {
            Index[13] = UNICODE_NULL;
        }
    }

    Status = STATUS_SUCCESS;

CopyPartialLfnEnd:
    return Status;
}


