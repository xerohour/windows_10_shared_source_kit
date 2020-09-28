/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    wimboot.c

Abstract:

    Implementation of the boot library's WIM file system filter support.
    The routines in this module make up the function table for a WIM
    file entry.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#pragma warning(disable : 4200)     // Allow 0 length array's
#include <xpress.h>
#include <bootlib.h>
#include "wimboot.h"

// -------------------------------------------------------------------- Globals

//
// Function table for a WIM filter.
//

const FILESYSTEM_FUNCTION_TABLE WimFunctionTable = {
    (PFILESYSTEM_OPEN_ROUTINE)             WimOpen,
    (PFILESYSTEM_CLOSE_ROUTINE)            WimClose,
    (PFILESYSTEM_READ_ROUTINE)             WimRead,
    (PFILESYSTEM_WRITE_ROUTINE)            WimWrite,
    (PFILESYSTEM_GET_NEXT_FILE_ROUTINE)    WimGetNextFile,
    (PFILESYSTEM_GET_INFORMATION_ROUTINE)  WimGetInformation,
    (PFILESYSTEM_SET_INFORMATION_ROUTINE)  WimSetInformation,
    (PFILESYSTEM_GET_REPARSE_DATA_ROUTINE) NULL
};

//
// Routines required to register this WIM file system filter implementation.
//

const FILESYSTEM_REGISTER_FUNCTION_TABLE WimRegisterFunctionTable = {
    (PFILESYSTEM_INITIALIZE_ROUTINE)      WimInitialize,
    (PFILESYSTEM_DESTROY_ROUTINE)         WimDestroy,
    (PFILESYSTEM_MOUNT_ROUTINE)           WimMount,
    (PFILESYSTEM_PURGE_CACHE_ROUTINE)     NULL
};

//
//  List of WIM structure contexts
//

PWIM_STRUCTURE_CONTEXT *WimDeviceTable;
ULONG WimDeviceTableEntries;
UCHAR NullHash[HASH_SIZE];
ULONG WimFixupErrorCount;

#define INITIAL_WIM_DEVICE_ENTRIES (2)
#define DIRENTRY_LENGTH_OFFSET (FIELD_OFFSET(DIRENTRY,liLength) + \
    sizeof(((LPDIRENTRY)0)->liLength))

#define STREAMENTRY_LENGTH_OFFSET (FIELD_OFFSET(STREAMENTRY,liLength) + \
    sizeof(((LPSTREAMENTRY)0)->liLength))

//
// Hash string is of the following format: 
// {xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx}
//

#define HASH_STRING_SIZE (5 * 8 + 2 + 4)

#ifdef FILE_MIN

#define WimpIntegrityDeviceReadAtOffset(_Integrity, _DeviceId, _ByteOffset, _Size, _Buffer, _BytesRead) \
    BlDeviceReadAtOffset(_DeviceId, _ByteOffset, _Size, _Buffer, _BytesRead)

#endif

// ------------------------------------------------------------------ Functions

NTSTATUS
WimDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys the WIM filter

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    Other Error if error occured destroying any device entry.

--*/

{

    NTSTATUS Status;

    //
    // Destroy the WIM Device Table.
    //

    Status = BlTblMap(WimDeviceTable,
                      WimDeviceTableEntries,
                      WimpDeviceTableDestroyEntry,
                      FALSE);

    //
    // Free the memory allocated for the table.
    //

    BlMmFreeHeap(WimDeviceTable);
    return Status;
}

// ------------------------------------------------------------------ Functions

NTSTATUS
WimInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the WIM filter

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if the device table could not be allocated.

--*/

{

    //
    // Initialize the Wim Device Table.  This table keeps track of Wim Device
    // Context for each Wim Volume.
    //

    WimDeviceTableEntries = INITIAL_WIM_DEVICE_ENTRIES;
    WimDeviceTable = BlMmAllocateHeap(WimDeviceTableEntries *
                                      sizeof(PWIM_STRUCTURE_CONTEXT));

    if (WimDeviceTable == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(WimDeviceTable,
                  WimDeviceTableEntries * sizeof(PWIM_STRUCTURE_CONTEXT));

    return STATUS_SUCCESS;
}

NTSTATUS
WimMount (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Attempts to mount the WIM file system filter on a block i/o device.
    Provided a block i/o device (DeviceId), this routine will attempt to
    to mount a WIM file system filter on top of a file system on the device.

Arguments:

    DeviceId - Device Id for a Block I/O Device.

    FilterLevel - Indicates the filter location of the mount request (zero
        indicates this is the first mount request on the device).

    FileEntry - Pointer to address to store an allocated FILE_ENTRY structure
        if a Wim file detected on the Block I/O Device referenced by
        DeviceId.  FileEntry is only allocated if a Wim file is
        detected.

Return Value:

    STATUS_SUCCESS when successful (and the file system has been mounted on
        the device).
    STATUS_UNSUCCESSFUL if the device does not have a WIM filesystem.
    STATUS_NO_MEMORY if there was an error allocating memory for any internal
        data structures.

--*/

{

    PFILE_ENTRY AllocatedFileEntry;
    LPDIRENTRY Dirent;
    ULONG Id;
    NTSTATUS Status;
    PWIM_STRUCTURE_CONTEXT WimContext;
    PWIM_FILE WimFile;

    UNREFERENCED_PARAMETER(FilterLevel);

    AllocatedFileEntry = NULL;
    WimContext = NULL;
    Dirent = NULL;

    //
    // Attempt to attach a Wim filter to the underlying device
    //

    Status = WimpAttachWim(DeviceId, &WimContext);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Attempt to add the Wim device structure to the WimDeviceTable which is
    // used to store the structure for each device that has a Wim filter.
    //

    Status = BlTblSetEntry(&WimDeviceTable,
                           &WimDeviceTableEntries,
                           WimContext,
                           &Id,
                           TblDoNotPurgeEntry);

    if (!NT_SUCCESS(Status)) {
        goto WimMountEnd;
    }

    //
    // If we get here, we want to use a Wim filter.  Allocate a buffer
    // for the root FILE_ENTRY.
    //

    AllocatedFileEntry = BlMmAllocateHeap(sizeof(FILE_ENTRY));
    if (AllocatedFileEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimMountEnd;
    }

    //
    // We need to initialize the file entry for the caller.  This should be
    // fairly easy, since we are creating a file entry for the root directory
    // on the device described by WimContext.
    //

    RtlZeroMemory(AllocatedFileEntry, sizeof(FILE_ENTRY));

    //
    // Allocate a buffer for the filepath.
    //

    AllocatedFileEntry->FilePath = BlMmAllocateHeap(sizeof(L"\\"));
    if (AllocatedFileEntry->FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimMountEnd;
    }

    RtlCopyMemory(AllocatedFileEntry->FilePath, L"\\", sizeof(L"\\"));

    //
    // The device id stored in a file entry described the device the file
    // system exists.  It is used when performing device reads for the
    // filesystem.
    //

    AllocatedFileEntry->DeviceId = DeviceId;

    //
    // Set the function table.
    //

    RtlCopyMemory(&AllocatedFileEntry->FunctionTable,
                  &WimFunctionTable,
                  sizeof(FILESYSTEM_FUNCTION_TABLE));

    //
    // Allocate a buffer for the file data.
    //

    WimFile = BlMmAllocateHeap(sizeof(WIM_FILE));
    if (WimFile == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimMountEnd;
    }

    RtlZeroMemory(WimFile, sizeof(WIM_FILE));
    AllocatedFileEntry->FileData = WimFile;

    //
    // Initialize Wim File structure.  It is composed of three fields, the Wim
    // specific file information, the user file information and the id to the
    // WimContext structure.
    //

    WimFile->WimDeviceTableId = Id;
    SET_FLAGS(WimFile->Information.Flags, FILE_FLAGS_IS_DIRECTORY);
    SET_FILE_DIRECTORY(AllocatedFileEntry->Attributes);

    //
    // The root directory does not have a DIRENTRY.  Each file's dirent is an
    // entry in the parent directory's file content.  Since the root
    // directory does not have a parent, there is no such entry for it.
    // Create a fake DIRENTRY for it.
    //

    Dirent = BlMmAllocateHeap(sizeof(DIRENTRY));
    if (Dirent == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimMountEnd;
    }

    //
    // Initialize the fake DIRENTRY for the root.  The root of the WIM
    // context contains nothing but a sub directory, which itself is
    // the real root.
    //

    RtlZeroMemory(Dirent, sizeof(DIRENTRY));

    if (WimContext->OpenByHash == FALSE) {
        Dirent->pSubdir = WimGetDirEntrySubDir(WimContext, WimContext->Root);
        // flag that the entry is fixed up
        SetFlag(Dirent->liUnused2.HighPart, WIM_ENTRY_FIXED);
    }

    SetFlag(Dirent->dwAttributes, FILE_ATTRIBUTE_DIRECTORY);

    //
    // Initialize FileContext.
    //

    WimFile->FileContext.FileEntry = Dirent;

    //
    // Wim directories are linked lists and tracking the size is not useful.
    //

    WimFile->Information.FileSize = 0;

    //
    // Return to caller with the new file entry for the mounted file.
    //

    *FileEntry = AllocatedFileEntry;

WimMountEnd:

    //
    // If we were encountered any errors, free allocated memory before
    // returning to caller.
    //

    if (!NT_SUCCESS(Status)) {
        if (WimContext != NULL) {
            WimpDestroyContext(WimContext)  ;
        }

        if (AllocatedFileEntry != NULL) {
            if (AllocatedFileEntry->FilePath != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FilePath);
            }

            if (AllocatedFileEntry->FileData != NULL) {
                BlMmFreeHeap(AllocatedFileEntry->FileData);
            }

            BlMmFreeHeap(AllocatedFileEntry);
        }

        if (Dirent != NULL) {
            BlMmFreeHeap(Dirent);
        }
    }

    return Status;
}

NTSTATUS
WimOpen (
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

    todo: [jamschw] - Determine underyling FS and store in information
          structure.

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
    WCHAR Buffer[HASH_STRING_SIZE + 1];
    LPDIRENTRY Dirent;
    PUCHAR FileHash;
    PWCHAR FilePath;
    SIZE_T FilePathLength;
    PWIM_FILE ParentWimFile;
    NTSTATUS Status;
    ULONG WimDeviceTableId;
    PWIM_FILE WimFile;
    PWIM_STRUCTURE_CONTEXT WimContext;

    //
    // Initialize local variables.
    //

    AllocatedFileEntry = NULL;
    ParentWimFile = (PWIM_FILE)ParentDirectoryEntry->FileData;
    WimContext = (PWIM_STRUCTURE_CONTEXT)WimDeviceTable[ParentWimFile->WimDeviceTableId];
    FileHash = NULL;
    Dirent = NULL;

    //
    // Attempt to find the dirent for FileName in the parent directory. If the
    // file is opened by file has, construct string representation of the hash
    // which will be used as file name.
    //

    if (!IS_OPEN_BY_HASH_SET(OpenFlags)) {
        
        //
        // Fail open call, since there is no metadata loaded for the image.
        //

        if (WimContext->OpenByHash != FALSE) {
            return STATUS_NOT_SUPPORTED;
        }

        Status = WimpSearchForDirent(ParentDirectoryEntry, FileName, &Dirent);
        if (!NT_SUCCESS(Status)) {
            goto WimOpenEnd;
        }

    } else {
        FileHash = (PUCHAR)FileName;
        swprintf_s(Buffer,
                   sizeof(Buffer) / sizeof(WCHAR),
                   L"{%08x-%08x-%08x-%08x-%08x}",
                   *(PULONG)&FileHash[0],                    
                   *(PULONG)&FileHash[4],
                   *(PULONG)&FileHash[8],
                   *(PULONG)&FileHash[12],
                   *(PULONG)&FileHash[16]);

        FileName = (PWSTR)Buffer; 
    }

    //
    // If we are here, we have found a dirent for the requested file.  Allocate
    // a file entry for the file and collect any information that is possible.
    //

    AllocatedFileEntry = BlMmAllocateHeap(sizeof(FILE_ENTRY));
    if (AllocatedFileEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimOpenEnd;
    }

    RtlZeroMemory(AllocatedFileEntry, sizeof(FILE_ENTRY));

    //
    // Do all allocations up front.  We additionally need to allocate a buffer
    // for the file path string and we need to allocate a WIM_FILE structure.
    //

    FilePathLength = wcslen(ParentDirectoryEntry->FilePath);
    Status = RtlSIZETAdd(FilePathLength, wcslen(FileName), &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto WimOpenEnd;
    }

    //
    // + 2 = '\\' + '\0';
    //

    Status = RtlSIZETAdd(FilePathLength, 2, &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto WimOpenEnd;
    }

    Status = RtlSIZETMult(FilePathLength, sizeof(WCHAR), &AllocationSize);
    if (!NT_SUCCESS(Status)) {
        goto WimOpenEnd;
    }

    FilePath = BlMmAllocateHeap(AllocationSize);
    if (FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimOpenEnd;
    }

    WimFile = BlMmAllocateHeap(sizeof(WIM_FILE));
    if (WimFile == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimOpenEnd;
    }

    RtlZeroMemory(WimFile, sizeof(WIM_FILE));

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
               (ParentDirectoryEntry->FilePath[1] == L'\0') ? L"%ls%ls" : L"%ls\\%ls",
               ParentDirectoryEntry->FilePath,
               FileName);
    
    AllocatedFileEntry->FilePath = FilePath;

    //
    // Copy the Wim function table to the file entry.
    //

    RtlCopyMemory(&AllocatedFileEntry->FunctionTable,
                  &WimFunctionTable,
                  sizeof(FILESYSTEM_FUNCTION_TABLE));

    //
    // Initialize the file information (WIM_FILE) structure.  Much of the
    // file's information can be determined from the dirent.  In the case of a
    // directory, its size must be determined by walking its cluster chain,
    // looking for the EOC (End of Clusters).  This operation is done last, as
    // it requires an initialized File Entry structure since the procedure will
    // require reads of the File Allocation Table.
    //

    //
    // Initialize the Wim Context structure.
    //

    WimFile->FileContext.FileEntry = Dirent;

    if (IS_OPEN_BY_HASH_SET(OpenFlags)) {
        WimFile->FileContext.Resource =
            WimpFindStreamResource(WimContext, FileHash);

        if (WimFile->FileContext.Resource == NULL) {
            Status = STATUS_NO_SUCH_FILE;
            goto WimOpenEnd;
        }

    } else if (BooleanFlagOn(Dirent->dwAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
        WimFile->FileContext.Resource = NULL;

    } else if (Dirent->wStreams) {

        //
        //  This file has streams, for the boot extension we just allow
        //  opening the default stream, as that should be more than
        //  enough for the boot case.
        //

        if (Dirent->pStream->pResource == NULL) {
            Dirent->pStream->pResource = WimpFindStreamResource(
                                                WimContext,
                                                Dirent->pStream->bHash);
        }

        WimFile->FileContext.Resource = Dirent->pStream->pResource;

    } else {
        if (Dirent->pResource == NULL) {
            Dirent->pResource = WimpFindStreamResource(
                                                WimContext,
                                                Dirent->bHash);
        }

        WimFile->FileContext.Resource = Dirent->pResource;
    }

    //
    // Only files smaller than 4GB are supported.
    //

    if (((Dirent->dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
        (WimFile->FileContext.Resource != NULL) &&
        (WimFile->FileContext.Resource->liOriginalSize.HighPart > 0)) {

        DBGPRINT(DBG_INIT, DBG_ERROR, ("WimOpen failed: file > 4GB limit.\n"));

        Status = STATUS_NO_SUCH_FILE;
        goto WimOpenEnd;
    }

    //
    // The WimDeviceTableId is the same as the parent directory's.
    //

    WimDeviceTableId = ParentWimFile->WimDeviceTableId;
    WimFile->WimDeviceTableId = WimDeviceTableId;

    //
    // Done constructing the File Data structure (minus the FileSize).
    //

    AllocatedFileEntry->FileData = WimFile;

    //
    // Determine the File Size and anything else dependent on the file being a
    // directory.
    //

    if (!IS_OPEN_BY_HASH_SET(OpenFlags) &&
        BooleanFlagOn(Dirent->dwAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

        SET_FLAGS(WimFile->Information.Flags, FILE_FLAGS_IS_DIRECTORY);
        SET_FILE_DIRECTORY(AllocatedFileEntry->Attributes);

        //
        // Wim directories are linked lists and tracking the size is not useful
        //

        WimFile->Information.FileSize = 0;

    } else {
        WimFile->Information.FileSize = WimFile->FileContext.Resource ?
            WimFile->FileContext.Resource->liOriginalSize.QuadPart :
            0;
    }

    //
    // Return the allocated and initialized file entry to the caller.
    //

    *FileEntry = AllocatedFileEntry;

WimOpenEnd:

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
WimClose (
    __in PFILE_ENTRY FileEntry
    )

/*++

Routine Description:

    Closes a file.  All that is required is to free all memory allocated.

Arguments:

    FileEntry - File entry for file to be closed.

Return Value:

    STATUS_SUCCESS

--*/

{
    PWIM_FILE FileInfo;

    FileInfo = (PWIM_FILE)FileEntry->FileData;

    //
    // Free memory allocated for the file.
    //

    BlMmFreeHeap(FileEntry->FileData);
    BlMmFreeHeap(FileEntry->FilePath);

    if (FileInfo->FileContext.ChunkTable != NULL) {
        BlMmFreeHeap(FileInfo->FileContext.ChunkTable);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
WimRead (
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
    // Call into helper routine to do actual work.
    //

    Status = WimpRead(FileEntry, Buffer, Size, &ReadCount);
    //
    // Count gets updated regardless of transfer failure or success
    //

    if (Count != NULL) {
        *Count = ReadCount;
    }

    return Status;
}

NTSTATUS
WimWrite (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

{

    UNREFERENCED_PARAMETER( FileEntry);
    UNREFERENCED_PARAMETER( Buffer);
    UNREFERENCED_PARAMETER( Size);
    UNREFERENCED_PARAMETER( Count);

    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS
WimGetNextFile (
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

    LPDIRENTRY CachedGetNextFileEntry;
    LPDIRENTRY Dirent;
    ULONG FileNameLength;
    NTSTATUS Status;
    PWIM_FILE WimFile;
    PWIM_STRUCTURE_CONTEXT WimContext;

    WimFile = (PWIM_FILE)DirectoryEntry->FileData;

    WimContext = (PWIM_STRUCTURE_CONTEXT)WimDeviceTable[WimFile->WimDeviceTableId];

    //
    // Find the dirent for FileName.  In doing so, DirectoryEntry's file
    // pointer will be at the next dirent.  This can be further optimized
    // to check the dirent at the current file pointer - sizeof(DIRENT) to
    // see if it is for FileName.  The assumption here is that WimpGetNextFile
    // will be called consecutively without other I/O operations in between.
    //
    // If FileName == NULL or FileName[0] == L'\0', then the caller is
    // initiating the search.
    //

    if ((FileName == NULL) || (FileName[0] == L'\0')) {
        Dirent = WimGetDirEntrySubDir(WimContext, WimFile->FileContext.FileEntry);
        if (Dirent == NULL) {
            return STATUS_NOT_FOUND;
        }

    } else {

        CachedGetNextFileEntry = WimFile->FileContext.CachedGetNextFileEntry;

        //
        // See if the file name is the same as the entry we returned last
        //

        FileNameLength = (ULONG)wcslen(FileName);
        if ((CachedGetNextFileEntry != NULL) &&
            (CachedGetNextFileEntry->wFileNameLength == (FileNameLength * sizeof(WCHAR))) &&
            WimpCompareFileNames(CachedGetNextFileEntry->FileName,
                                    FileName,
                                    (USHORT)FileNameLength)) {

            //
            // We can use the cached entry and avoid rescanning the entire directory.
            //

            Dirent = CachedGetNextFileEntry;

        } else {

            //
            // Scan from the start of the directory for the FileName
            //

            Status = WimpSearchForDirent(DirectoryEntry, FileName, &Dirent);
            if (!NT_SUCCESS(Status)) {
                return STATUS_INVALID_PARAMETER;
            }
        }

        //
        // Move to the next dirent.  It will be the one just after FileName.
        //

        Dirent = WimGetDirEntryNext(WimContext, Dirent);

        if (Dirent == NULL) {
            return STATUS_NOT_FOUND;
        }
    }

    //
    // If the buffer is too small, return the required size.
    //

    if (*BufferSize < (ULONG)Dirent->wFileNameLength / sizeof(WCHAR) + 1) {
        *BufferSize = Dirent->wFileNameLength / sizeof(WCHAR) + 1;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Copy the filename to the caller's buffer.
    //

    RtlCopyMemory(FileName,
                  Dirent->FileName,
                  Dirent->wFileNameLength);

    FileName[Dirent->wFileNameLength / sizeof(WCHAR)] = L'\0';
    WimFile->FileContext.CachedGetNextFileEntry = Dirent;
    return STATUS_SUCCESS;
}

NTSTATUS
WimGetInformation (
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

    PWIM_FILE FileInfo;

    FileInfo = (PWIM_FILE)FileEntry->FileData;

    //
    // Copy FileEntry->FileData.Information to the caller's buffer.
    //

    RtlCopyMemory(FileInformation,
                  &FileInfo->Information,
                  sizeof(FILE_INFORMATION));

    return STATUS_SUCCESS;
}

NTSTATUS
WimSetInformation (
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

    PWIM_FILE FileInfo;
    FILE_INFORMATION FileInformationCopy;

    FileInfo = (PWIM_FILE)FileEntry->FileData;

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

// --------------------------------------------------------- Internal Functions

//
// WIM Device Table Entry Routines.
//

NTSTATUS
WimpDeviceTableDestroyEntry (
    PWIM_STRUCTURE_CONTEXT WimContext,
    ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the WIM device table.  This includes freeing the
    memory allocated for the entry as well as zeroing the entry in the device
    table.

Arguments:

    WimContext - Entry in the WIM device table to destroy.

    Index - Index in the device table that points to WimContext.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    //
    // Free the entry's memory.
    //

    BlMmFreeHeap(WimContext);
    WimDeviceTable[Index] = NULL;
    return STATUS_SUCCESS;
}

NTSTATUS
WimpAttachWim (
    __in DEVICE_ID DeviceId,
    __out PWIM_STRUCTURE_CONTEXT * WimContext
    )

/*++

Routine Description:

    Opens the Wim file if it exists, and builds up the Wim context

Arguments:

    DeviceId - Device Id for a Block I/O Device

    FilterLevel - Level of the Wim filter

Return Value:

    A Wim context if a Wim is found and successfully initialized

--*/

{

    ULONG Count;
    BOOLEAN OpenByHash;
    NTSTATUS Status;
    WIMHEADER_PACKED WimHeader;
    PWIM_INTEGRITY_CONTEXT WimIntegrity;

    WimIntegrity = NULL;

    *WimContext = NULL;

    //
    // Before any data is read from the WIM, check if integrity is required.
    //

#ifndef FILE_MIN

    Status = WimpIntegrityCreateContext(DeviceId, &WimIntegrity);
    if (!NT_SUCCESS(Status)) {
        goto WimpAttachWimEnd;
    }

#endif


    //
    // Read the Wim header
    //

    Status = WimpIntegrityDeviceReadAtOffset(WimIntegrity,
                                             DeviceId,
                                             0,
                                             sizeof(WimHeader),
                                             &WimHeader,
                                             &Count);

    if (!NT_SUCCESS(Status)) {
        goto WimpAttachWimEnd;
    }

    //
    // Determine if the files in the wim will only be accessed by resource hash.
    //

    OpenByHash = BlDeviceIsOpenByHash(DeviceId);

    //
    // Is it a valid Wim header?
    //

    Status = WimpValidateWim(&WimHeader, OpenByHash);
    if (!NT_SUCCESS(Status)) {
        goto WimpAttachWimEnd;
    }

    //
    // Allocate the Wim context for this device
    //

    *WimContext = WimpAllocateContext(&WimHeader, OpenByHash);
    if (*WimContext == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimpAttachWimEnd;
    }

    //
    // Fill out the Wim context
    //

    (*WimContext)->DeviceId = DeviceId;
    (*WimContext)->Integrity = WimIntegrity;
    WimIntegrity = NULL;

    Status = WimpSetupContext(*WimContext, &WimHeader);
    if (!NT_SUCCESS(Status)) {
        goto WimpAttachWimEnd;
    }

WimpAttachWimEnd:

    //
    // Clean up on failure and don't attach.
    //

    if (!NT_SUCCESS(Status)) {
        if (*WimContext != NULL) {
            WimpDestroyContext(*WimContext);
            *WimContext = NULL;
        }

#ifndef FILE_MIN

        if (WimIntegrity != NULL) {
            WimpIntegrityDestroyContext(WimIntegrity);
        }

#endif

    }

    return Status;
}

NTSTATUS
WimpDestroyContext (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    )

/*++

Routine Description:

  Frees the WimContext.

Arguments:

  WimContext - Context of WIM.

Return Value:

  STATUS_SUCCESS

--*/

{

    if (WimContext != NULL) {
        WimpDestroyDecompressionWorkspace(WimContext);

#ifndef FILE_MIN

        if (WimContext->Integrity != NULL) {
            WimpIntegrityDestroyContext(WimContext->Integrity);
        }

#endif

        BlMmFreeHeap(WimContext);
    }

    return STATUS_SUCCESS;
}

PVOID
XPRESS_CALL
WimpAllocateXpress(
    __in PVOID Context,
    __in int Size
    )
/*++

Routine Description:

    Allocation routine for XPress decompression workspace.

Arguments:

    Context - Context provided by xpress is not used.
    Size - Size of the allocation buffer.

Return Value:

    Pointer to buffer, if successful, NULL otherwise.

--*/
{
    UNREFERENCED_PARAMETER(Context);

    return BlMmAllocateHeap(Size);
}

VOID
XPRESS_CALL
WimpFreeXpress(
    __in PVOID Context,
    __in PVOID Address
    )
/*++

Routine Description:

    Allocation routine for XPress decompression workspace.

Arguments:

    Context - Context provided by xpress is not used.
    Address - Pointer to the buffer that needs to be released.

Return Value:

    Pointer to buffer, if successful, NULL otherwise.

--*/
{
    UNREFERENCED_PARAMETER(Context);

    BlMmFreeHeap(Address);
}

NTSTATUS
WimpDestroyDecompressionWorkspace (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    )

/*++

Routine Description:

    Free the decompression workspace.

Arguments:

    WimContext - Context of WIM.

Return Value:

    STATUS_SUCCESS

--*/

{

    if (WimContext->Workspace != NULL) {

        if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_XPRESS) &&
            WimContext->DecompressContext) {

            XpressDecodeClose(WimContext->DecompressContext, NULL, WimpFreeXpress);
            WimContext->DecompressContext = NULL;
        }

        BlMmFreeHeap(WimContext->Workspace);
        WimContext->Workspace = NULL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
WimpValidateWim (
    __in LPWIMHEADER_PACKED WimHeader,
    __in BOOLEAN OpenByHash
    )

/*++

Routine Description:

    Validates a WIM, given its header.

Arguments:

    WimHeader - Header of the WIM

    OpenByHash - Supplies boolean value indicating that files will only be
        opened by resource hash.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_UNSUCCESSFUL if it is not a valid Wim header.
    STATUS_NOT_SUPPORTED opening by hash in the min version of the library.
    STATUS_WIM_NOT_BOOTABLE if the Wim file is not makred as bootable.

--*/

{

    CHAR ImageTag[] = IMAGE_TAG;
    LARGE_INTEGER Size4GB;

    Size4GB.QuadPart = (LONGLONG)4 * 1024 * 1024 * 1024;
    if (RtlCompareMemory(WimHeader->ImageTag, ImageTag, sizeof(WimHeader->ImageTag)) != sizeof(WimHeader->ImageTag)) {

        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_ERROR,
                 ("Invalid WIM tag '%s'\n", WimHeader->ImageTag));
        
        return STATUS_UNSUCCESSFUL;

    } else if (WimHeader->cbSize != sizeof(WIMHEADER_V1_PACKED)) {

        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_ERROR,
                 ("Invalid WIM wimHeader size %d\n", WimHeader->cbSize));

        return STATUS_UNSUCCESSFUL;
    } else if (WimHeader->dwVersion != IMGVER_CUR_DWORD) {
        
        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_ERROR,
                 ("Invalid WIM version %d\n", WimHeader->dwVersion));

        return STATUS_UNSUCCESSFUL;
    }

    //
    // Opening by hash (WIMBOOT) is not supported in the minimal version which
    // does not implement integrity checks.
    //

    if (OpenByHash == FALSE) {
        if (WimHeader->dwBootIndex == 0) {

            DBGPRINT(DBG_MOUNTUNMOUNT,
                     DBG_ERROR,
                     ("WIM not marked as bootable\n"));

            return STATUS_WIM_NOT_BOOTABLE;

        } else if ((WimHeader->rhBootMetadata.liOriginalSize.QuadPart > Size4GB.QuadPart) ||
                   (WimHeader->rhBootMetadata.liOriginalSize.QuadPart == 0)) {

            DBGPRINT(DBG_MOUNTUNMOUNT,
                     DBG_ERROR,
                     ("Invalid WIM boot metadata size %x\n",
                      WimHeader->rhBootMetadata.liOriginalSize.QuadPart));
            
            return STATUS_UNSUCCESSFUL;

        } else if (WimHeader->rhBootMetadata.Base.liOffset.QuadPart == 0)    {
            
            DBGPRINT(DBG_MOUNTUNMOUNT,
                     DBG_ERROR,
                     ("Invalid WIM boot metadata offset = %x\n",
                      WimHeader->rhBootMetadata.Base.liOffset.QuadPart));

            return STATUS_UNSUCCESSFUL;
        }
    } 

#ifdef FILE_MIN

    else {
        return STATUS_NOT_SUPPORTED;
    }

#endif

    return STATUS_SUCCESS;
}

PWIM_STRUCTURE_CONTEXT
WimpAllocateContext (
    __in LPWIMHEADER_PACKED WimHeader,
    __in BOOLEAN OpenByHash
    )

/*++

Routine Description:

    Allocates the WIM_STRUCTURE_CONTEXT

Arguments:

    WimHeader - Wim header.

    OpenByHash - Supplies boolean value indicating that files will only be
        opened by resource hash.

Return Value:

    PWIM_STRUCTURE_CONTEXT if successful
    NULL otherwise

--*/

{

    ULONG AllocationSize;
    ULONG BootMetaDataLength;
    ULONG OffsetTableLength;
    NTSTATUS Status;
    PWIM_STRUCTURE_CONTEXT WimContext;

    //
    // Resources larger than 4GB are not supported.
    //

    if ((WimHeader->rhBootMetadata.liOriginalSize.HighPart) ||
        (WimHeader->rhOffsetTable.liOriginalSize.HighPart)) {

        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_ERROR,
                 ("Cannot handle BootMetadata or OffsetTable > 4GB.\n"));

        return NULL;
    }

    if (OpenByHash == FALSE) {
        BootMetaDataLength = WimHeader->rhBootMetadata.liOriginalSize.LowPart;

    } else {
        BootMetaDataLength = 0;
    }

    OffsetTableLength = WimHeader->rhOffsetTable.liOriginalSize.LowPart;

    //
    // Check if 8-bit aligning overflows the resource size.
    //

    if ((QuadAlign(BootMetaDataLength) < BootMetaDataLength) ||
        (QuadAlign(OffsetTableLength) < OffsetTableLength)) {

        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_ERROR,
                 "8-bit aligning BootMetaDataLength and/or OffsetTableLength"
                     "causes integer overflow, and cannot be handled.\n");

        return NULL;
    }

    //
    // Calculate context allocation size.
    //

    Status = RtlULongAdd(QuadAlign(sizeof(WIM_STRUCTURE_CONTEXT)),
                         QuadAlign(OffsetTableLength),
                         &AllocationSize);

    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    Status = RtlULongAdd(AllocationSize,
                         QuadAlign(BootMetaDataLength),
                         &AllocationSize);

    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    //
    // The buffer we get from BlMmAllocateHeap may not be quad aligned
    // on x86 systems, and QuadAlignPtr will not give the same
    // result as QuadAlign when calculating the location of
    // WimContext->BootMetaData.  Therefore, we request an extra quadword
    // worth of bytes from the allocation so that we can use QuadAlignPtr.
    //

    Status = RtlULongAdd(AllocationSize, QuadAlign(1), &AllocationSize);
    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    //
    // Allocate the context.
    //

    WimContext = BlMmAllocateHeap(AllocationSize);
    if (!WimContext) {
        return NULL;
    }

    //
    // Initialize the context.
    //

    RtlZeroMemory(WimContext, sizeof(WIM_STRUCTURE_CONTEXT));
    WimContext->OpenByHash = OpenByHash;

    //
    // Skip over structure context.
    //

    WimContext->OffsetTable = Add2Ptr(WimContext,
                                      QuadAlign(sizeof(WIM_STRUCTURE_CONTEXT)));

    //
    // Calculate the last byte of the offset table. The offset table isn't
    // linked therefore we need to know when we've walked to the end.
    //

    WimContext->LastResourceBYTE = Add2Ptr(WimContext->OffsetTable,
                                           OffsetTableLength);

    //
    // Skip over the resource.
    //

    if (BootMetaDataLength > 0) {
        WimContext->BootMetaData = QuadAlignPtr(WimContext->LastResourceBYTE);
        WimContext->BootMetaDataLength = BootMetaDataLength;
    }

    WimContext->CompressedChunkSize = WimHeader->dwCompressionSize;
    WimContext->WimFlags = WimHeader->dwFlags;
    return WimContext;
}

NTSTATUS
WimpSetupContext (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPWIMHEADER_PACKED WimHeader
    )

/*++

Routine Description:

    Initializes the WIM_STRUCTURE_CONTEXT

Arguments:

    WimContext - Context of WIM.
    WimHeader - Wim header.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_FILE_INVALID if the Wim is corrupt or of the wrong format

--*/

{

    NTSTATUS Status;

    //
    // Setup the decompression workspace.
    //

    Status = WimpInitializeDecompressionWorkspace(WimContext);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Read the offset/resource table.
    //

    Status = WimpReadResource(WimContext,
                              &WimHeader->rhOffsetTable,
                              WimContext->OffsetTable);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Don't need to load boot metadata when the device is in the open by hash
    // mode.
    //

    if (WimContext->OpenByHash != FALSE) {
        goto WimpSetupContextEnd;
    }


    //
    // Read the directory/BootMetaData.
    //

    Status = WimpReadResource(WimContext,
                              &WimHeader->rhBootMetadata,
                              WimContext->BootMetaData);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Fixup root directory pointer in the boot medatada.
    //

    Status = WimpFixupRoot(WimContext);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

WimpSetupContextEnd:
    return Status;
}

NTSTATUS
WimpInitializeDecompressionWorkspace (
    __in PWIM_STRUCTURE_CONTEXT WimContext
    )

/*++

Routine Description:

    This routine initializes the decompression workspace, if the target
    WIM uses compression. It also initializes the pages lookaside lists.

Arguments:

    WimContext - Wim overlay to be initialized


Return Value:

    STATUS_SUCCESS if successful
    STATUS_NO_MEMORY if the decompression buffer could not be allocated.

--*/

{

    ULONG AllocationSize;
    ULONG CompressBufferWorkSpaceSize;
    ULONG DecompressBufferWorkSpaceSize;
    NTSTATUS NtStatus;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Is the Wim compressed?
    //

    if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESSION)) {

        //
        // Fail cleanly in the presence of a zero compressed chunk size
        // instead of hitting divide-by-zero faults later on in decompression.
        //

        if (WimContext->CompressedChunkSize == 0) {
            return STATUS_DISK_CORRUPT_ERROR;
        }

        if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_LZNT1)) {

            //
            // Wim uses LZNT compression.
            //

            NtStatus = RtlGetCompressionWorkSpaceSize(
                            COMPRESSION_FORMAT_LZNT1,
                            &CompressBufferWorkSpaceSize,
                            &DecompressBufferWorkSpaceSize);

            //
            // ISSUE-2003/10/13-SaadSyed: Why are we doing this?
            //

            if (!NT_SUCCESS(NtStatus) ||
                (DecompressBufferWorkSpaceSize < WIM_MINIMUM_DECOMPRESS_WORKSPACE)) {

                DecompressBufferWorkSpaceSize = WIM_MINIMUM_DECOMPRESS_WORKSPACE;
            }

        } else if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_NEW_XPRESS)) {

            NtStatus = RtlGetCompressionWorkSpaceSize(
                            COMPRESSION_FORMAT_XPRESS,
                            &CompressBufferWorkSpaceSize,
                            &DecompressBufferWorkSpaceSize);

            //
            // ISSUE-2003/10/13-SaadSyed: Why are we doing this?
            //

            if (!NT_SUCCESS(NtStatus) ||
                (DecompressBufferWorkSpaceSize < WIM_MINIMUM_DECOMPRESS_WORKSPACE)) {

                DecompressBufferWorkSpaceSize = WIM_MINIMUM_DECOMPRESS_WORKSPACE;
            }

        } else if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_NEW_XPRESS_HUFF)) {

            NtStatus = RtlGetCompressionWorkSpaceSize(
                            COMPRESSION_FORMAT_XPRESS_HUFF,
                            &CompressBufferWorkSpaceSize,
                            &DecompressBufferWorkSpaceSize);

            //
            // ISSUE-2003/10/13-SaadSyed: Why are we doing this?
            //

            if (!NT_SUCCESS(NtStatus) ||
                (DecompressBufferWorkSpaceSize < WIM_MINIMUM_DECOMPRESS_WORKSPACE)) {

                DecompressBufferWorkSpaceSize = WIM_MINIMUM_DECOMPRESS_WORKSPACE;
            }

        } else if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_LZX)) {

            //
            // Wim is compressed using LZX.
            //

            DecompressBufferWorkSpaceSize = LZX_GetDecodeBufferSize(
                                                WimContext->CompressedChunkSize);


        } else if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_XPRESS)) {

            //
            // Wim is compressed using XPress, initialize the context.
            //

            WimContext->DecompressContext = XpressDecodeCreate(NULL,
                                                               WimpAllocateXpress);

            if (NULL == WimContext->DecompressContext) {

                //
                // Failed to allocate the Xpress context.
                //

                Status = STATUS_INSUFFICIENT_RESOURCES;

                goto WimpInitializeDecompressionWorkspaceEnd;
            }

            //
            // Unlike LZX, Xpress doesn't decompress into its own workspace
            // before copying it to the output buffer. Therefore allocate extra
            // workspace to use as decompression buffer.
            //

            DecompressBufferWorkSpaceSize = WimContext->CompressedChunkSize;
        } else {

            //
            // Wim is not of known compression type.
            //

            Status = STATUS_UNSUPPORTED_COMPRESSION;
            goto WimpInitializeDecompressionWorkspaceEnd;
        }

        DBGPRINT(DBG_MOUNTUNMOUNT,
                 DBG_VERBOSE,
                 ("Decompression workspace = %d.\n",
                  DecompressBufferWorkSpaceSize));

        WimContext->CompressedBufferSize = max( WimContext->CompressedChunkSize + WIM_INTEGRITY_BLOCK_LENGTH, 0x80000 );

        //
        // With the workspace buffer we allocate a decompression buffer
        // Our Workspace is layed out as:
        //    decompression buffer (the compressed stream) +
        //      uncompressed chunk +
        //      decompressor workspace
        //

        Status = RtlULongAdd(WimContext->CompressedBufferSize,
                             WimContext->CompressedChunkSize,
                             &AllocationSize);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        Status = RtlULongAdd(DecompressBufferWorkSpaceSize,
                             AllocationSize,
                             &AllocationSize);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }


        WimContext->Workspace = BlMmAllocateHeap(AllocationSize);
        if (WimContext->Workspace == NULL) {
            return  STATUS_NO_MEMORY;
        }

        WimContext->UncompressedChunkBuffer = Add2Ptr(WimContext->Workspace, WimContext->CompressedBufferSize);
        WimContext->DecompressorWorkspace = Add2Ptr(WimContext->UncompressedChunkBuffer, WimContext->CompressedChunkSize);

        //
        // Size reserved for decompressor.
        //

        WimContext->DecompressWorkSpaceSize = DecompressBufferWorkSpaceSize;
        if (FlagOn(WimContext->WimFlags, FLAG_HEADER_COMPRESS_LZX)) {

            //
            // Initialize LZX decompressor with the portion of
            // workspace after the decompression buffer.
            //

            WimContext->DecompressContext =
                LZX_DecodeInit(WimContext->DecompressorWorkspace,
                               WimContext->DecompressWorkSpaceSize,
                               WimContext->CompressedChunkSize);
        }
    }

WimpInitializeDecompressionWorkspaceEnd:

    return Status;
}

NTSTATUS
WimpReadResource (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPRESHDR_DISK_SHORT Resource,
    __in PVOID Buffer
    )

/*++

Routine Description:

    Reads bytes from a resource into buffer.

Arguments:

    WimContext - Context of the WIM
    Resource - Pointer to resource
    Buffer - Output buffer

Return Value:

    STATUS_SUCCESS if successfull

--*/

{

    ULONG BytesRead;
    ULONG Chunk;
    ULONG Chunks;
    ULONG *ChunkTable;
    ULONG Length;
    UCHAR *Next;
    ULONG_PTR NextLocation;
    NTSTATUS Status;
    ULONGLONG Offset;
    ULONG Total;
    ULONG UncompressedSize;

    ChunkTable = NULL;
    Status = STATUS_SUCCESS;
    Offset = Resource->Base.liOffset.QuadPart;

    //
    // Is the resource compressed?
    //

    if (Resource->Base.bFlags & RESHDR_FLAG_COMPRESSED) {

        Chunks = ((Resource->liOriginalSize.LowPart
                   + WimContext->CompressedChunkSize
                   - 1)
                  / WimContext->CompressedChunkSize)
                 - 1;

        //
        // Convert the file based Chunk table into a memory based Chunk table.
        //

        Status = WimpBuildChunkTable(WimContext, Resource, Chunks, &ChunkTable);
        if (!NT_SUCCESS( Status )) {
            goto WimpReadResourceEnd;
        }

        //
        //  Advance to after the chunk table.
        //

        Status = RtlULongLongAdd(Offset, ChunkTable[0], &Offset);
        if (!NT_SUCCESS(Status)) {
            goto WimpReadResourceEnd;
        }

        Total = Resource->liOriginalSize.LowPart;

        //
        // Destination for first uncompressed output.
        //

        Next = (UCHAR*)Buffer;
        Chunk = 0;
        while (Total != 0) {
#pragma prefast(suppress: __WARNING_INCORRECT_VALIDATION, "False positive 26014, prefast thinks we can walk beyond the ChunkTable, but the ChunkTable's size is always one larger than liOriginalSize needs, and we're looping up to that value.  See annotation in WimpBuildChunkTable")
            Length = ChunkTable[Chunk + 1] - ChunkTable[Chunk];
            Chunk++;
            UncompressedSize = WimContext->CompressedChunkSize;
            if (UncompressedSize > Total) {
                UncompressedSize = Total;
            }

            if (Length > UncompressedSize) {
                Status = STATUS_FILE_INVALID;
                goto WimpReadResourceEnd;
            }

            if (Length == 0) {

                //
                // Handle a zero filled Chunk.
                //

                RtlZeroMemory(Next, UncompressedSize);

            } else if (Length == UncompressedSize) {

                //
                // Handle Chunk that is not compressed.
                //

                Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                         WimContext->DeviceId,
                                                         Offset,
                                                         Length,
                                                         Next,
                                                         &BytesRead);

                if (!NT_SUCCESS(Status)) {
                    goto WimpReadResourceEnd;
                }

            } else {

                //
                // Read a compressed Chunk into buffer.
                //

                Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                         WimContext->DeviceId,
                                                         Offset,
                                                         Length,
                                                         WimContext->Workspace,
                                                         &BytesRead);

                if (!NT_SUCCESS(Status)) {
                    goto WimpReadResourceEnd;
                }

                Status = WimpDecompress(WimContext,
                                        Next,
                                        UncompressedSize,
                                        WimContext->Workspace,
                                        Length,
                                        &UncompressedSize);

                if ( !NT_SUCCESS(Status) ) {
                    goto WimpReadResourceEnd;
                }
            }

            //
            // Update the starting offset.
            //

            Status = RtlULongLongAdd(Offset, Length, &Offset);
            if (!NT_SUCCESS(Status)) {
                goto WimpReadResourceEnd;
            }

            //
            // Bump the destination pointer.
            //

            Status = RtlULongPtrAdd((ULONG_PTR)Next,
                                    UncompressedSize,
                                    &NextLocation);

            if (!NT_SUCCESS(Status)) {
                goto WimpReadResourceEnd;
            }

            Next = (UCHAR *)NextLocation;

            //
            // Decrement the number of bytes left.
            //

            Total -= UncompressedSize;
        }

    } else {

        //
        // Resource is not compressed.
        //

        Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                 WimContext->DeviceId,
                                                 Offset,
                                                 Resource->liOriginalSize.LowPart,
                                                 Buffer,
                                                 &BytesRead);

        if (!NT_SUCCESS(Status)) {
            goto WimpReadResourceEnd;
        }
    }

WimpReadResourceEnd:

    if (ChunkTable) {
        BlMmFreeHeap(ChunkTable);
    }

    return Status;
}

NTSTATUS
WimpBuildChunkTable (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPRESHDR_DISK_SHORT Resource,
    __in ULONG Chunks,
    __deref_out_ecount(Chunks + 2) PULONG * ChunkTable
    )

/*++

Routine Description:

    Converts the file chunk table into a memory chunk table

    File chunk table:
         offset to chunk 2, offset to chunk 3, ..., offset to chunk n

    Memory chunk table:
         offset to chunk 1, offset to chunk 2, offset to chunk 3, ...,
            offset to chunk n, offset to byte after end of chunk n

    This routine assumes that ChunkTable has 2 extra entries more than "Chunks"

Arguments:

    Resource
    Chunks
    ChunkTable

Return Value:

    none

--*/

{

    ULONG i;
    ULONG length;
    ULONG *p1;
    ULONG *p2;
    NTSTATUS Status;
    ULONG Length;
    PULONG ChunkTableLocal = NULL;

    //
    // We only have to deal with ULONG Chunks, because we fail to accept
    // any WIMs where the resource table or boot directory are greater than
    // 4GB in WimpOpenWimFile.
    //

    Status = RtlULongMult(Chunks, sizeof(ULONG), &Length);
    if (!NT_SUCCESS(Status)) {
        goto WimpBuildChunkTableEnd;
    }

    //
    // Length = # of bytes of ChunkTable in the file, WimpBuildChunkTable
    // needs 2 more entries.
    //

    Status = RtlULongAdd(Length, 2 * sizeof(ULONG), &Length);
    if (!NT_SUCCESS(Status)) {
        goto WimpBuildChunkTableEnd;
    }

    //
    // Allocate a temp buffer.
    //

    ChunkTableLocal = BlMmAllocateHeap(Length);
    if (ChunkTableLocal == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WimpBuildChunkTableEnd;
    }

    if (Chunks != 0) {

        ULONG BytesRead;

        //
        // Subtract the extra entries back off so that we Read the
        // correct amount.
        //

        Length -= (2 * sizeof(ULONG));
        Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                 WimContext->DeviceId,
                                                 Resource->Base.liOffset.QuadPart,
                                                 Length,
                                                 ChunkTableLocal,
                                                 &BytesRead);

        if (!NT_SUCCESS(Status)) {
            goto WimpBuildChunkTableEnd;
        }
    }

    length = Chunks * sizeof(ULONG);  // length of chunk table in file
    p2 = ChunkTableLocal + Chunks + 1;

    //
    // last entry is offset beyond end of compressed data.
    //

    *p2 = (ULONG)SIZEMASK(Resource->Base.ullSize);
    p2--;
    if (Chunks != 0) {
        p1 = ChunkTableLocal + Chunks - 1;
        for (i = 0; i < Chunks; i++) {
            *p2 = length + *p1;
            p1--;
            p2--;
        }
    }

    //
    // first entry is offset to immediately after chunk table in file.
    //

    *p2 = length;

WimpBuildChunkTableEnd:

    if (NT_SUCCESS( Status )) {

        *ChunkTable = ChunkTableLocal;
    } else {
        if (ChunkTableLocal != NULL) {
            BlMmFreeHeap(ChunkTableLocal);
        }
    }

    return Status;
}

NTSTATUS
WimpFixupRoot (
    __inout PWIM_STRUCTURE_CONTEXT WimContext
    )
{

    ULONG_PTR EndOfSecurityData;
    ULONG Length;
    SECURITYBLOCK_DISK *SecurityBlock;
    NTSTATUS Status;

    SecurityBlock = (SECURITYBLOCK_DISK *)WimContext->BootMetaData;

    DBGPRINT(DBG_DIRECTORY,
             DBG_INFO,
             ("SecurityBlock=%p\n",
              SecurityBlock));

    if (WimContext->BootMetaDataLength < sizeof(SECURITYBLOCK_DISK)) {
        return STATUS_FILE_INVALID;
    }

    Length = SecurityBlock->dwTotalLength;
    if (Length == 0) {
        Length = sizeof(SECURITYBLOCK_DISK);
    }
    
    if (Length < sizeof(SECURITYBLOCK_DISK)) {
        return STATUS_FILE_INVALID;
    }

    //
    // Simply skip past the security table since we don't use it
    // during boot.
    //

    Status = RtlULongPtrAdd((ULONG_PTR)WimContext->BootMetaData,
                            (ULONG_PTR)Length,
                            &EndOfSecurityData);

    if (!NT_SUCCESS(Status)) {
        return STATUS_FILE_INVALID;
    }

    //
    // Return pointer to first byte after the table and security descriptors.
    // This is actually a pointer to the Root DIRENTRY. Not the security
    // table itself.
    //

    WimContext->Root = (LPDIRENTRY)QuadAlignPtr(EndOfSecurityData);
    return STATUS_SUCCESS;
}

NTSTATUS
WimpFixupStreams (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in DWORD Count,
    __in LPSTREAMENTRY Stream,
    __out LPDIRENTRY *NextDirEntry
    )

/*++

Routine Description:


Arguments:


Return Value:


--*/

{

    PUCHAR EndOfLengthFieldPointer;
    LPSTREAMENTRY LastStreamEntry;
    PUCHAR MetadataLimit;
    LPSTREAMENTRY NextStreamEntry;
    NTSTATUS Status;

    LastStreamEntry = NULL;
    MetadataLimit = WimContext->BootMetaData + WimContext->BootMetaDataLength;

    while (Count != 0) {
        if (Stream == NULL) {
            return STATUS_FILE_INVALID;
        }

        //
        // Check if the length field of the first stream is within boot metadata
        // before reading it.
        //

        Status = RtlULongPtrAdd((ULONG_PTR)Stream,
                                STREAMENTRY_LENGTH_OFFSET,
                                &(ULONG_PTR)EndOfLengthFieldPointer);

        if ((!NT_SUCCESS(Status)) ||
            (EndOfLengthFieldPointer >= MetadataLimit)) {

            ASSERT (FALSE);
            DBGPRINT(DBG_DIRECTORY,
                     DBG_ERROR,
                     ("Stream %p does not fit in BootMetaData.\n",
                      Stream));

            return STATUS_FILE_INVALID;
        }

        if (Stream->liLength.LowPart < sizeof(STREAMENTRY)) {
            DBGPRINT(DBG_DIRECTORY,
                      DBG_ERROR,
                      ("STREAMENTRY length is invalid for %p.\n", Stream));

            return STATUS_FILE_INVALID;
        }

        //
        // Make sure this stream entry falls entirely within the boot metadata
        // block before accessing it.
        //

        Status = RtlULongPtrAdd((ULONG_PTR)Stream,
                                (ULONG_PTR)Stream->liLength.QuadPart,
                                &(ULONG_PTR)NextStreamEntry);

        if ((!NT_SUCCESS(Status)) ||
            ((PUCHAR)NextStreamEntry > MetadataLimit)) {

            DBGPRINT(DBG_DIRECTORY,
                     DBG_ERROR,
                     ("STREAMENTRY %p does not fit in BootMetaData.\n",
                      Stream));

            return STATUS_FILE_INVALID;
        }

        if (!IsQuadAligned(NextStreamEntry)) {
            DBGPRINT(DBG_DIRECTORY,
                      DBG_ERROR,
                      ("Unaligned Next STREAMENTRY %p->pNext=%p.\n",
                       Stream,
                       Stream->pNext));

            return STATUS_FILE_INVALID;
        }

        Stream->pNext = NextStreamEntry;

#if 0
        //
        // Disabling this code to save processing time.  We don't need any stream
        // support for boot files, so don't attempt to find resources for any files
        // that we happen to fixup that happen to have stream resources.
        // richp 2/11/2013
        //

        ResourceHash = Stream->bHash;

        //
        // Lookup the stream resource by hash.
        //

        if (!IsNullHash(ResourceHash, HASH_SIZE)) {
            Stream->pResource = WimpFindStreamResource(WimContext,
                                                       ResourceHash);

            //
            // Resource not found, hard error.
            //

            if (Stream->pResource == NULL) {
                DBGPRINT(DBG_DIRECTORY,
                          DBG_ERROR,
                          ("Resource not found for stream %p.\n",
                           Stream));

                return STATUS_FILE_INVALID;
            }

        } else {
            Stream->pResource = NULL;
        }
#endif

        LastStreamEntry = Stream;
        Stream = NextStreamEntry;
        Count--;
    }

    if (LastStreamEntry != NULL) {

        //
        // Terminate list of streams.
        //

        LastStreamEntry->pNext = NULL;
    }

    *NextDirEntry = (LPDIRENTRY)Stream;
    return STATUS_SUCCESS;
}

LPRESHDR_DISK
WimpFindStreamResource (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in_bcount(HASH_SIZE) PUCHAR ResourceHash
    )

/*++

Routine Description:


Arguments:


Return Value:


--*/

{

    LPRESHDR_DISK Resource;

    if (IsNullHash(ResourceHash, HASH_SIZE)) {
        return NULL;
    }

    Resource = WimContext->OffsetTable;

    while ((Resource != NULL) &&
           ((RtlCompareMemory(Resource->bHash, ResourceHash, HASH_SIZE) != HASH_SIZE) ||
            (Resource->Base.bFlags & RESHDR_FLAG_FREE))) {

        Resource++;
        if ((PVOID)Resource >= WimContext->LastResourceBYTE) {
            Resource = NULL;
        }
    }

    if (Resource != NULL) {

        DBGPRINT(DBG_DIRECTORY,
                 DBG_PAINFUL,
                 ("Found resource %p for id %d\n",
                  Resource,
                  *ResourceHash));

    } else {

        DBGPRINT(DBG_DIRECTORY,
                 DBG_ERROR,
                 ("Resource not found for id %d.\n",
                  *ResourceHash
                  ));
    }

    return Resource;
}

void
FixupDirEntry(
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPDIRENTRY DirEntry
    )
{
    NTSTATUS Status;
    PUCHAR EndOfLengthFieldPointer;
    PUCHAR MetadataLimit = WimContext->BootMetaData + WimContext->BootMetaDataLength;
    LPDIRENTRY NextEntry;

    if (FlagOn(DirEntry->liUnused2.HighPart, WIM_ENTRY_FIXED)) {
        return;
    }

    ASSERT(IsQuadAligned(DirEntry));

    //
    // Check if the length field of the directory is within boot metadata
    // before reading it.
    //

    Status = RtlULongPtrAdd((ULONG_PTR)DirEntry,
                            DIRENTRY_LENGTH_OFFSET,
                            &(ULONG_PTR)EndOfLengthFieldPointer);


    ASSERT(NT_SUCCESS(Status) && EndOfLengthFieldPointer <= MetadataLimit);

    if ((!NT_SUCCESS(Status)) ||
        (EndOfLengthFieldPointer > MetadataLimit)) {

        goto FixupFail;
    }

    if (DirEntry->liLength.QuadPart == 0) {
        goto FixupFail;
    }

    //
    // The length of DIRENTRY should be >= sizeof(DIRENTRY), but it
    // shouldn't exceed 64KB, let alone 4.2GB.
    //

    ASSERT(DirEntry->liLength.HighPart == 0);
    ASSERT(DirEntry->liLength.LowPart < 0x10000);
    ASSERT(DirEntry->liLength.LowPart >= sizeof(DIRENTRY));

    if ((DirEntry->liLength.HighPart != 0) ||
        (DirEntry->liLength.LowPart >= 0x10000) ||
        (DirEntry->liLength.LowPart < sizeof(DIRENTRY))) {

        goto FixupFail;
    }

    //
    // Make sure the entire directory entry falls entirely within the boot
    // metadata block before accessing it.
    //

    Status = RtlULongPtrAdd((ULONG_PTR)DirEntry,
                            (ULONG_PTR)DirEntry->liLength.QuadPart,
                            &(ULONG_PTR)NextEntry);

    ASSERT(NT_SUCCESS(Status) && (PUCHAR)NextEntry <= MetadataLimit);

    if ((!NT_SUCCESS(Status)) ||
        ((PUCHAR)NextEntry > MetadataLimit)) {

        goto FixupFail;
    }

    ASSERT(IsQuadAligned(NextEntry));

    if (!DirEntry->wStreams) {

        DirEntry->pStream = NULL;
        DirEntry->pNext = NextEntry;

    } else {

        //
        // This file has multiple streams, steam entries following
        // immediately after.
        //

        DirEntry->pStream = (STREAMENTRY *)NextEntry;

        //
        // Fix up stream entries.
        //

        Status = WimpFixupStreams(WimContext,
                                    DirEntry->wStreams,
                                    DirEntry->pStream,
                                    &DirEntry->pNext);

        if (!NT_SUCCESS(Status)) {

            ASSERT(FALSE);

            goto FixupFail;
        }
    }

    //
    // If next entry is zero length, then end of directory
    //

    if (DirEntry->pNext->liLength.QuadPart == 0) {
        DirEntry->pNext = NULL;
    }

    if (BooleanFlagOn(DirEntry->dwAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

        if (DirEntry->liSubdirOffset.LowPart == 0) {
            DirEntry->pSubdir = NULL;

        } else {

            //
            // Check if the length field of the subdirectory is within
            // BootMetaData before reading it.
            //

            Status = RtlULongPtrAdd((ULONG_PTR)WimContext->BootMetaData,
                                    (ULONG_PTR)DirEntry->liSubdirOffset.LowPart,
                                    &(ULONG_PTR)NextEntry);

            if ((!NT_SUCCESS(Status)) ||
                ((PUCHAR)NextEntry >= MetadataLimit)) {

                ASSERT (FALSE);

                goto FixupFail;
            }

            Status = RtlULongPtrAdd((ULONG_PTR)NextEntry,
                                    DIRENTRY_LENGTH_OFFSET,
                                    &(ULONG_PTR)EndOfLengthFieldPointer);

            if ((!NT_SUCCESS(Status)) ||
                ((PUCHAR)EndOfLengthFieldPointer > MetadataLimit)) {

                ASSERT (FALSE);

                goto FixupFail;
            }

            if ((DirEntry->liSubdirOffset.LowPart == 0) ||
                (NextEntry->liLength.QuadPart == 0)) {

                //
                // The subdirectory is empty, we set pSubDir to NULL.
                // If we let this pass down a NULL DIRENTRY is linked to
                // pSubDir. During query we cannot determine if its a NULL
                // DIRENTRY or if its the last DIRENTRY as both have
                // pNext==NULL. Therefore we mark pSubDir=NULL.
                //

                DirEntry->pSubdir = NULL;

            } else {
                DirEntry->pSubdir = NextEntry;
            }
        }

    }

    SetFlag(DirEntry->liUnused2.HighPart, WIM_ENTRY_FIXED);
    return;

FixupFail:
    DirEntry->pNext = NULL;
    DirEntry->pSubdir = NULL;
    SetFlag(DirEntry->liUnused2.HighPart, WIM_ENTRY_FIXED);
    WimFixupErrorCount += 1;
    return;
}

LPDIRENTRY
WimGetDirEntryNext (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPDIRENTRY Dirent
    )
{

    //
    // Ensure that entry has been fixed up.
    //

    FixupDirEntry(WimContext, Dirent);
    return Dirent->pNext;
}

LPDIRENTRY
WimGetDirEntrySubDir (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __in LPDIRENTRY Dirent
    )
{

    //
    // Ensure that entry has been fixed up.
    //

    FixupDirEntry(WimContext, Dirent);
    return Dirent->pSubdir;
}

NTSTATUS
WimpSearchForDirent (
    __in PFILE_ENTRY DirectoryEntry,
    __in PWSTR FileName,
    __out LPDIRENTRY *Dirent
    )

/*++

Routine Description:

    Searches the directory described by DirectoryEntry in search of a dirent
    for the file with the name, FileName.  If found, this routine will return
    the file offset to the direct as well as the dirent to the caller.

    This routine assumes that DirectoryEntry is indeed a directory.

Arguments:

    DirectoryEntry - File Entry structure for the directory to be searched.

    FileName - The file to look for.

    Dirent - An address to store the dirent for the filename, if found.

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

    LPDIRENTRY DirEntry;
    ULONG FileNameLength;
    PWIM_FILE WimFile;
    PWIM_STRUCTURE_CONTEXT WimContext;

    *Dirent = NULL;

    //
    // Initialize locals.
    //

    WimFile = DirectoryEntry->FileData;
    WimContext = (PWIM_STRUCTURE_CONTEXT)WimDeviceTable[WimFile->WimDeviceTableId];
    DirEntry = WimGetDirEntrySubDir(WimContext, WimFile->FileContext.FileEntry);
    FileNameLength = (ULONG)wcslen(FileName);

    //
    // Walk the directory looking for a match for FileName.
    //

    while (DirEntry) {
        if (DirEntry->wFileNameLength == FileNameLength *  sizeof(WCHAR)) {

            //
            // Case insensitive compare.
            //

            if (WimpCompareFileNames(DirEntry->FileName,
                                     FileName,
                                     (USHORT)FileNameLength)) {

                *Dirent = DirEntry;
                return STATUS_SUCCESS;
            }
        }

        DirEntry = WimGetDirEntryNext(WimContext, DirEntry);
    }

    //
    // File / directory was not found in this parent.
    //

    return STATUS_NO_SUCH_FILE;
}

BOOLEAN
WimpCompareFileNames (
    __in_ecount(Length) PWCHAR Name1,
    __in_ecount(Length) PWCHAR Name2,
    __in USHORT Length
    )

/*++

Routine Description:

    Performs a case insensitive compare of two wchar strings of equal length.

Arguments:

    Name1  - file name 1

    Name2  - file name 2

    Length - Maximum length to compare

Return Value:

    TRUE if it is a match, FALSE otherwise

--*/

{

    while (Length) {
        if ((*Name1 != *Name2) && (ToUpper(*Name1) != ToUpper(*Name2))) {
            return FALSE;
        }

        Name1++;
        Name2++;
        Length--;
    }

    return TRUE;
}

//
//  BlockAlign(): Aligns P on the next V boundary.
//  BlockAlignTruncate(): Aligns P on the prev V boundary.
//

#define BlockAlign(P,V) ((ASSERT( V != 0)), (((P)) + (V-1) & (0-(V))))
#define BlockAlignTruncate(P,V) ((P) & (0-(V)))

NTSTATUS
WimpRead (
    __in PFILE_ENTRY FileEntry,
    __out_bcount(Length) VOID *Buffer,
    __in ULONG Length,
    __out ULONG *Count
    )

/*++

Routine Description:

    This routine reads data from the specified file.

Arguments:

    FileId - Supplies the file table index.

    Buffer - Supplies a pointer to the buffer that receives the data
        read.

    Length - Supplies the number of bytes that are to be read.

    Transfer - Supplies a pointer to a variable that receives the number
        of bytes actually transfered.

Return Value:

    STATUS_SUCCESS is returned if the read operation is successful. Otherwise,
    an unsuccessful status is returned that describes the reason for failure.

--*/

{

    ULONG BytesRead;
    PWIM_FILE_CONTEXT FileContext;
    LPRESHDR_DISK Resource;
    LARGE_INTEGER start;
    NTSTATUS Status;
    ULONG uncompressedSize;
    PWIM_STRUCTURE_CONTEXT WimContext;
    PWIM_FILE WimFile;

    Status = STATUS_SUCCESS;
    WimFile = (PWIM_FILE)FileEntry->FileData;
    FileContext = (PWIM_FILE_CONTEXT)&WimFile->FileContext;
    WimContext = (PWIM_STRUCTURE_CONTEXT)WimDeviceTable[WimFile->WimDeviceTableId];
    Resource = FileContext->Resource;
    *Count = 0;

    DBGPRINT(DBG_READWRITE,
             DBG_PAINFUL,
             ("FileId=%d, Length=%d Buffer=%p.\n",
              FileEntry->FileId,
              Length,
              Buffer));

    if (Resource == NULL) {
        return STATUS_SUCCESS;
    }

    if (Resource->Base.bFlags & RESHDR_FLAG_COMPRESSED) {

        //
        // Read from compressed resource.
        //

        PVOID WorkSpace, WorkSpaceExtra, Next;
        ULONG ByteOffset, chunks, chunk, lastchunk, length, fragoffset;

        //
        // Read for actual decompression read operation.
        //

        WorkSpace = WimContext->Workspace;
        WorkSpaceExtra = WimContext->DecompressorWorkspace;

        chunks = ((Resource->liOriginalSize.LowPart
                   + WimContext->CompressedChunkSize - 1)
                  / WimContext->CompressedChunkSize)
                 - 1;

        if (FileContext->ChunkTable == NULL) {

            //
            // Convert the file based Chunk table into a memory based Chunk table.
            // We're also creating a dummy chunk table if there is none on the disk.
            //
    
            Status = WimpBuildChunkTable(WimContext, (LPRESHDR_DISK_SHORT)Resource, chunks, &FileContext->ChunkTable);
            if (!NT_SUCCESS( Status )) {
                goto WimpReadDone;
            }
        }

        ByteOffset = (ULONG)WimFile->Information.FileOffset;
        chunk = ByteOffset / WimContext->CompressedChunkSize;
        fragoffset = ByteOffset % WimContext->CompressedChunkSize;

        if (ByteOffset + Length > Resource->liOriginalSize.LowPart) {
            Length = Resource->liOriginalSize.LowPart - ByteOffset;
        }

        Next = Buffer;
        while (NT_SUCCESS(Status) && Length) {

            ULONG ThisCompressedReadLength = 0;
            PVOID CompressedDataBuffer = WorkSpace;
            ULONG Padding;

            //
            // Start is first byte after chunkTable + offset to start of chunk.
            // We round down our read to ensure alignment with integrity checking
            // so we don't need to triple buffer.
            //

            start.QuadPart = FileContext->ChunkTable[chunk] + Resource->Base.liOffset.QuadPart;
            Padding = (start.LowPart & (WIM_INTEGRITY_BLOCK_LENGTH-1));
            if (Padding) {
                start.QuadPart -= Padding;
            }

            //
            // Now walk forward counting the maximum number of chunks we can read.
            // While doing this:
            //   ThisCompressedReadLength - the size of the compressed read we're up to.
            //   length - the amount of uncompressed data that we'd get.
            //   lastchunk - the chunk immediately following the read.
            //

            lastchunk = chunk;
            length = 0;

            while(TRUE) {

                lastchunk++;
                length += WimContext->CompressedChunkSize;
                ThisCompressedReadLength = ALIGN_RANGE_UP(FileContext->ChunkTable[lastchunk] - FileContext->ChunkTable[chunk] + Padding, WIM_INTEGRITY_BLOCK_LENGTH);

                //
                //  Whoopsie, we went to far.  Back up.
                //

                if (ThisCompressedReadLength > WimContext->CompressedBufferSize) {

                    length -= WimContext->CompressedChunkSize;
                    lastchunk--;
                    ThisCompressedReadLength = ALIGN_RANGE_UP(FileContext->ChunkTable[lastchunk] - FileContext->ChunkTable[chunk] + Padding, WIM_INTEGRITY_BLOCK_LENGTH);
                    break;
                }

                //
                //  We have sufficient to satisfy the original request,
                //  so we can stop.
                //

                if (length - fragoffset >= Length) {
                    break;
                }
            }

            if (ThisCompressedReadLength > WimContext->CompressedBufferSize) {

                ASSERT (FALSE);
                Status = STATUS_FILE_INVALID;
                goto WimpReadDone;
            }

            if (ThisCompressedReadLength > 0) {

                //
                // Read the data.  We double buffer assuming that chunks may be
                // compressed and require decompression.
                //
    
                Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                         WimContext->DeviceId,
                                                         start.QuadPart,
                                                         ThisCompressedReadLength,
                                                         CompressedDataBuffer,
                                                         &BytesRead);
    
                if (!NT_SUCCESS(Status)) {
                    goto WimpReadDone;
                }

                //
                //  Because we rounded down to integrity alignment to prevent
                //  triple buffering, we have to start decompressing from
                //  where the compressed data really is.
                //

                CompressedDataBuffer = Add2Ptr(CompressedDataBuffer, Padding);
            }

            while (chunk < lastchunk) {

                length = FileContext->ChunkTable[chunk + 1] - FileContext->ChunkTable[chunk];

                uncompressedSize = WimContext->CompressedChunkSize - fragoffset;
                if (uncompressedSize > Length) {
                    uncompressedSize = Length;
                }


                if (length == 0) {

                    //
                    // handle a zero filled chunk.
                    //

#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "False positive 26015, prefast thinks uncompressedSize can be greater than Length, but we just bounded it 10 lines ago")
                    RtlZeroMemory(Next, uncompressedSize);

                } else if ((length == WimContext->CompressedChunkSize) ||
                           ((chunk == chunks) &&
                            (length == (ULONG)(Resource->liOriginalSize.QuadPart
                                               % WimContext->CompressedChunkSize)))) {

                    RtlCopyMemory( Next, Add2Ptr( CompressedDataBuffer, fragoffset ), uncompressedSize );
    
                } else {
    
                    if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_LZNT1) {
                        Status = RtlDecompressFragment(COMPRESSION_FORMAT_LZNT1,
                                                       Next,
                                                       uncompressedSize,
                                                       CompressedDataBuffer,
                                                       length,
                                                       fragoffset,
                                                       &uncompressedSize,
                                                       WorkSpaceExtra);
    
                        if (!NT_SUCCESS(Status)) {
                            goto WimpReadDone;
                        }
    
                    } else if ((WimContext->WimFlags & FLAG_HEADER_COMPRESS_LZX) ||
                               (WimContext->WimFlags & FLAG_HEADER_COMPRESS_XPRESS) ||
                               (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS) ||
                               (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS_HUFF)) {
    
                        PCHAR DecompDestination;
                        ULONG desiredsize = uncompressedSize;
                        ULONG fullchunksize = WimContext->CompressedChunkSize; // full uncompressed size
    
                        if (chunk == chunks) {
    
                            //
                            // Adjust uncompressed size of last chunk.
                            //
    
                            fullchunksize = (ULONG)(Resource->liOriginalSize.QuadPart
                                                    - (LONGLONG)(chunk * WimContext->CompressedChunkSize));
                        }
    
                        if (fragoffset == 0 && uncompressedSize == fullchunksize) {
    
                            //
                            // We can decompress the whole chunk directly to user buffer.
                            //
    
                            DecompDestination = Next;
    
                        } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_LZX) {
    
                            //
                            // We need to decompress the whole chunk, we can do it over the top
                            // of the read input (WorkSpace), because LZX decompresses into its
                            // work buffer and copies result into output buffer at the end.
                            //
    
                            uncompressedSize = fullchunksize;
                            DecompDestination = WimContext->UncompressedChunkBuffer;
    
                        } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_XPRESS) {
    
                            //
                            // Decompress the whole chunk in the extra work space we
                            // allocated for Xpress.
                            //
    
                            uncompressedSize = fullchunksize;
                            DecompDestination = WimContext->UncompressedChunkBuffer;
    
                        } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS) {
    
                            //
                            // Decompress the whole chunk in the extra work space we
                            // allocated for Xpress.
                            //
    
                            uncompressedSize = fullchunksize;
                            DecompDestination = WimContext->UncompressedChunkBuffer;
    
                        } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS_HUFF) {
    
                            //
                            // Decompress the whole chunk in the extra work space we
                            // allocated for Xpress.
                            //
    
                            uncompressedSize = fullchunksize;
                            DecompDestination = WimContext->UncompressedChunkBuffer;
    
                        } else {
    
                            ASSERT(FALSE);
    
                            DecompDestination = NULL;
                        }
    
                        Status = WimpDecompress(WimContext,
                                                DecompDestination,
                                                uncompressedSize,
                                                CompressedDataBuffer,
                                                length,
                                                &uncompressedSize);
    
                        if (!NT_SUCCESS(Status)) {
                            goto WimpReadDone;
                        }
    
                        if (DecompDestination != Next) {
    
                            //
                            //  Now copy needed portion from WorkSpace to user buffer
                            //
    
                            uncompressedSize = desiredsize;
                            RtlCopyMemory(Next, DecompDestination + fragoffset, uncompressedSize);
                        }
                    }
                }

                DBGPRINT(DBG_READWRITE,
                         DBG_PAINFUL,
                         ("chunk @ %x, %d - %d\r\n",
                          start.LowPart,
                          read,
                          uncompressedSize ));
    
                CompressedDataBuffer = Add2Ptr(CompressedDataBuffer, length);
                Next = (UCHAR *)Next + uncompressedSize;
                Length -= uncompressedSize;
                ByteOffset += uncompressedSize;
                *Count += uncompressedSize;
                chunk++;
                fragoffset = 0;
            }
        }

    } else {

        //
        // Read from non-compressed resource.
        //

        start.QuadPart = Resource->Base.liOffset.QuadPart
                            + WimFile->Information.FileOffset;

        if (WimFile->Information.FileOffset >= Resource->liOriginalSize.LowPart) {
            Length = 0;

        } else if ((WimFile->Information.FileOffset + Length) > Resource->liOriginalSize.LowPart) {
            Length = (ULONG)(Resource->liOriginalSize.LowPart - WimFile->Information.FileOffset);
        }

        if (Length) {
            Status = WimpIntegrityDeviceReadAtOffset(WimContext->Integrity,
                                                     WimContext->DeviceId,
                                                     start.QuadPart,
                                                     Length,
                                                     Buffer,
                                                     &BytesRead);

            if (!NT_SUCCESS(Status)) {
                goto WimpReadDone;
            }

            *Count = BytesRead;
        }
    }

WimpReadDone:

    WimFile->Information.FileOffset += *Count;

    DBGPRINT(DBG_READWRITE,
             DBG_PAINFUL,
             ("    ->%d, %x\r\n",
              Length,
              Status));

    return Status;
}

NTSTATUS
WimpDecompress (
    __in PWIM_STRUCTURE_CONTEXT WimContext,
    __out_bcount(TargetBufferLength) PVOID TargetBuffer,
    __in ULONG TargetBufferLength,
    __in_bcount(SourceBufferLength) PVOID SourceBuffer,
    __in ULONG SourceBufferLength,
    __out PULONG TargetSize
    )

/*++

Routine Description:

    Decompress a block from the SourceBuffer to the TargetBuffer

Arguments:

    WimContext -

    TargetBuffer - pointer to the destination buffer for the uncompressed data

    TargetBufferLength - size of the destination buffer

    SourceBuffer - pointer to the source buffer containing the compressed data

    SourceBufferLength - number of bytes of compressed data

    TargetSize - pointer to a variable that returns the count of bytes after decompression


Return Value:

    STATUS_SUCCESS is returned if the operation is successful. Otherwise,
    an unsuccessful status is returned that describes the reason for failure.

--*/

{

    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_LZNT1) {

        Status = RtlDecompressBuffer(COMPRESSION_FORMAT_LZNT1,
                                     TargetBuffer,
                                     TargetBufferLength,
                                     SourceBuffer,
                                     SourceBufferLength,
                                     TargetSize);

    } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS) {

        Status = RtlDecompressBufferEx(COMPRESSION_FORMAT_XPRESS,
                                       TargetBuffer,
                                       TargetBufferLength,
                                       SourceBuffer,
                                       SourceBufferLength,
                                       TargetSize,
                                       WimContext->DecompressorWorkspace);

    } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_NEW_XPRESS_HUFF) {

        Status = RtlDecompressBufferEx(COMPRESSION_FORMAT_XPRESS_HUFF,
                                       TargetBuffer,
                                       TargetBufferLength,
                                       SourceBuffer,
                                       SourceBufferLength,
                                       TargetSize,
                                       WimContext->DecompressorWorkspace);

    } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_LZX) {

        int Result;

        Result = LZX_Decode(WimContext->DecompressContext,
                            TargetBufferLength,
                            SourceBuffer,
                            SourceBufferLength,
                            TargetBuffer,
                            TargetBufferLength,
                            (LONG *)TargetSize);

        //
        // Reset the compression history.
        //

        LZX_DecodeNewGroup(WimContext->DecompressContext);

        if (Result) {

            Status = STATUS_FILE_INVALID;
        }
    } else if (WimContext->WimFlags & FLAG_HEADER_COMPRESS_XPRESS) {

        *TargetSize = XpressDecode(WimContext->DecompressContext,
                                   TargetBuffer,
                                   TargetBufferLength,
                                   TargetBufferLength,
                                   SourceBuffer,
                                   SourceBufferLength);

        if (*TargetSize != TargetBufferLength) {

            Status = STATUS_FILE_INVALID;
        }
    }

    return Status;
}

NTSTATUS
WimpAddDirectoryListEntry (
    __in PLIST_ENTRY ListHead,
    __in LPDIRENTRY Directory
    )

/*++

Routine Description:

    This routine adds an entry for the given directory to the list.

Arguments:

    ListHead - Supplies a pointer to the head of the list.

    Directory - Supplies a pointer to the directory to be added.

Return Value:

    NT status code.

--*/

{

    PWIM_DIRECTORY_LIST_ENTRY Entry;
    NTSTATUS Status;

    Entry = BlMmAllocateHeap(sizeof(WIM_DIRECTORY_LIST_ENTRY));
    if (Entry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateDirectoryListEntryEnd;
    }

    RtlZeroMemory(Entry, sizeof(WIM_DIRECTORY_LIST_ENTRY));
    Entry->Directory = Directory;
    InsertTailList(ListHead, &Entry->NextEntry);
    Status = STATUS_SUCCESS;

CreateDirectoryListEntryEnd:
    return Status;
}

VOID
WimpCleanupDirectoryList (
    __in PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This routine frees up all entries in the given directory list.

Arguments:

    ListHead - Supplies a pointer to the head of the list.

Return Value:

    None.

--*/

{

    PWIM_DIRECTORY_LIST_ENTRY DirectoryEntry;
    PLIST_ENTRY Entry;

    //
    // Walk through the list and free up all entries.
    //

    while (IsListEmpty(ListHead) == FALSE) {
        Entry = RemoveHeadList(ListHead);
        DirectoryEntry = CONTAINING_RECORD(Entry,
                                           WIM_DIRECTORY_LIST_ENTRY,
                                           NextEntry);

        BlMmFreeHeap(DirectoryEntry);
    }

    return;
}

