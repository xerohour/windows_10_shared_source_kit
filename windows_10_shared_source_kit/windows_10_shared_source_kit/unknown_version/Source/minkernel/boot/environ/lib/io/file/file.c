/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    file.c

Abstract:

    Implementation of boot library file i/o api.

    When file's are opened, an entry is created in a file table.  On each file,
    a mount file system routine is called.  This allows for a file system to be
    mounted on a file.  To reduce the overhead of a mount call on each file,
    the file table will not purge file entries when their respective files are
    closed.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include "file.h"

// -------------------------------------------------------------------- Globals

//
// There is a global pointer to the file table and the number of entries in the
// table.  The file table makes use of the boot environment library's generic
// table api.
//

PFILE_ENTRY *FileTable;
ULONG FileEntries;

#define INITIAL_FILE_ENTRIES (16)
#define CAPPED_FILE_ENTRIES  (512)

//
// Global list of registered file systems.  Each file system must register a
// mount routine.
//

LIST_ENTRY RegisteredFileSystems;

// ----------------------------------------------- Internal Function Prototypes

NTSTATUS
BlpFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __out PFILE_ID FileId,
    __out_opt PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    );

NTSTATUS
FileInformationCheck (
    __in PFILE_INFORMATION FileInformation,
    __in BOOLEAN IsWriteOperation,
    __in_opt PULONG InputSize,
    __out_opt PULONG ByteCountRequested,
    __out_opt PULONG OutputAdjustedSize
    );

NTSTATUS
FileTableDestroyEntry (
    __in PFILE_ENTRY FileEntry,
    __in ULONG Index
    );

BOOLEAN
FileTableCompareWithSameAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    );

BOOLEAN
FileTableCompareWithSubsetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    );

NTSTATUS
FileTablePurgeEntry (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
FileIoOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __in PTABLE_COMPARE_ENTRY CompareRoutine,
    __deref_opt_out PFILE_ENTRY *ReturnFileEntry
    );

NTSTATUS
FileIoMountFileSystem (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
FileIoDestroyRegisteredFileSystems (
    VOID
    );

VOID
FileIoSetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in OPEN_FLAGS OpenFlags
    );

PWCHAR
FileIoCopyParentDirectoryPath (
    __in PWSTR FilePath
    );

PWCHAR
FileIoCopyFileName (
    __in PWSTR FilePath
    );

NTSTATUS
FileIoPrint (
    __in PFILE_ENTRY FileEntry,
    __in FILE_ID FileId
    );

// ------------------------------------------------------------------ Functions

//
// Implementation of Boot Environment File I/O API.
//

NTSTATUS
BlFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __out PFILE_ID FileId
    )

/*++

Routine Description:

    This function opens a file.  In order to open the file, a filesystem must
    be mounted on the file's block i/o device and the file system's function
    table must be associated with the newly opened file.

    In order to 'open' a file, the parent directory must be 'opened' in order
    for a file system to determine the existence of a file and location on the
    block i/o disk of the file.

    If the file is already open and the OpenFlags flags do not conflict, the
    same file descriptor id may be returned and the reference count to the
    file increased.

Arguments:

    DeviceId - Supplies the handle for the containing the file to open.

    OpenPath - Supplies the path to the file to open.

    OpenFlags - Supplies a bitmask containing flags to use when opening the
        file.

    FileId - Supplies a pointer to a variable that receives a handle to file.

Return Value:

    STATUS_SUCCESS when successful.  (FileId will also contain the file's
        handle).

    STATUS_INVALID_PARAMETER if the specified device is invalid (Invalid handle
        or a handle to a non-block i/o device).

    STATUS_INVALID_PARAMETER if the specified file path is invalid (NULL or
        does not start with a '\').

    STATUS_NO_MEMORY if an allocation fails for an internal data structure.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on the underlying
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist.

--*/

{

    DEVICE_ID OverlayDeviceId;
    WIM_PROVIDER_EXTERNAL_INFO ReparseData;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);
    Status = BlpFileOpen(DeviceId, OpenPath, OpenFlags, 0, FileId, &ReparseData);
    if(Status == STATUS_REPARSE) {
        BlFileClose(*FileId);
        Status = STATUS_NOT_FOUND;

        //
        // If the file is a reparse point, try to open the actual file, unless
        // the caller explicity requested to not follow reparse points.
        //

        if (!IS_OPEN_NO_REPARSE_SET(OpenFlags)) {
            OverlayDeviceId = BlDeviceGetOverlay(DeviceId, ReparseData.DataSourceId);
            if (OverlayDeviceId != INVALID_DEVICE_ID) {
                Status = BlpFileOpen(OverlayDeviceId,
                                     (PWSTR)ReparseData.ResourceHash,
                                     OpenFlags | OPEN_BY_HASH,
                                     0,
                                     FileId,
                                     NULL);
            }
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileClose (
    __in FILE_ID FileId
    )

/*++

Routine Description:

    Closes a device by decrementing the reference count on the file handle.

    N.B. This routine does not purge the file's entry in the global file table.
         Entries remain resident and are only purged when required.  This allows
         a the file entry to be reused if the file is ever reopened.

Arguments:

    FileId - Supplies the handle to the file to close.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid or if the file is
        already closed.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[4];
    BOOLEAN IsCatalog;
    BOOLEAN IsDirectory;
    ULONG PathSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Make sure this is a valid file id.
    //

    if ((FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Close an instance of the file.
        //

        FileTable[FileId]->ReferenceCount -= 1;

        //
        // If there are no more instances of the file open, clear the open flag
        // and log the ETW event.
        //

        if (FileTable[FileId]->ReferenceCount == 0) {
            CLEAR_FILE_OPEN(FileTable[FileId]->Attributes);
            IsDirectory =
                IS_FILE_DIRECTORY_SET(FileTable[FileId]->Attributes) != FALSE;

            IsCatalog =
                IS_FILE_CATALOG_SET(FileTable[FileId]->Attributes) != FALSE;

            if ((!IsDirectory) && (!IsCatalog)) {

                EventDataDescCreate(&EventDataDescriptor[0],
                                    (PVOID)&(FileTable[FileId]->DeviceId),
                                    sizeof(DEVICE_ID));

                PathSize = (ULONG)((wcslen(FileTable[FileId]->FilePath) + 1) *
                                   sizeof(WCHAR));

                EventDataDescCreate(&EventDataDescriptor[1],
                                    (PVOID)FileTable[FileId]->FilePath,
                                    PathSize);

                EventDataDescCreate(&EventDataDescriptor[2],
                                    (PVOID)&(FileTable[FileId]->BytesRead),
                                    sizeof(ULONGLONG));

                EventDataDescCreate(&EventDataDescriptor[3],
                                    (PVOID)&(FileTable[FileId]->BytesWritten),
                                    sizeof(ULONGLONG));

                BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                              &BOOT_FILE_ACCESS,
                              NULL,
                              4,
                              EventDataDescriptor);

                FileTable[FileId]->BytesRead = 0;
                FileTable[FileId]->BytesWritten = 0;
            }
        }

        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileReadEx (
    __in FILE_ID FileId,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine reads from a file at the offset specified by the internal
    file pointer. In the case where the requested number of bytes exceeds the
    remaining bytes in the file (from the file pointer to the end), behavior
    depends upon the presence of the Count output parameter. If present, the
    actual remaining bytes are returned and the call completes successfully.
    If not present, an error is returned. In all cases, a successful read
    operation will advance the file pointer by the number of bytes read.

    N.B. File contents may be cached and a file read request does not
         necessarily result in a device read.

Arguments:

    FileId - Supplies a handle to a file.

    Buffer - Supplies a buffer to receive the contents of the read operation.

    Size - Supplies the number of bytes to read.

    Count - Supplies an optional pointer to a variable that receives the number
        of bytes read. The presence of this argument may affect the behavior
        of this routine.

    Flags - Supplies the flags indicating how the file should be read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file information reveals that the
        current file offset exceeds the file's size.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with read access.

    STATUS_INVALID_PARAMETER if the size requested would cause an overflow
        when added to the current file offset.

    STATUS_INVALID_PARAMETER if the number of bytes to be read crosses
        the end of the file and the "Count" parameter is not specified.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    DEVICE_INFORMATION DeviceInformation = {0};
    NTSTATUS DeviceStatus;
    FILE_INFORMATION FileInformation;
    ULONG OriginalIoAccessFlags;
    PFILESYSTEM_READ_ROUTINE ReadRoutine;
    BOOLEAN RestoreIoAccessFlags;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    RestoreIoAccessFlags = FALSE;

    //
    // Initialize OriginalIoAccessFlags to satisfy the compiler.
    //

    OriginalIoAccessFlags = 0;

    //
    // Ensure that the file handle is valid and the file was opened for read.
    //

    if ((Buffer == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED_FOR_READ(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileReadExEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlFileReadExEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    Status = FileInformationCheck(&FileInformation,
                                  FALSE,
                                  &AdjustedSize,
                                  Count,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileReadExEnd;
    }

    //
    // Specifically handle a request of zero bytes, avoiding passing such a
    // request to the filesystem stub.  Forward all other requests to the
    // stub.
    //

    if (AdjustedSize == 0) {
        Status = STATUS_SUCCESS;
        if (ARGUMENT_PRESENT(Count) != FALSE) {
            *Count = 0;
        }

    } else {

        //
        // If this is a block IO or partition device, and the read request
        // is for cache-enabled or prefetch-enabled, then modify the device IO
        // access flags for this read when necessary, however, only assert if
        // this task fails as the read can still succeed.
        //

        if (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE) ||
            CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH)) {

            DeviceStatus = BlDeviceGetInformation(FileTable[FileId]->DeviceId,
                                                  &DeviceInformation);

            ASSERT(DeviceStatus == STATUS_SUCCESS);

            if (((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
                 (DeviceInformation.DeviceType == DEVICE_PARTITION)) &&
                ((!CHECK_FLAG(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_CACHE_READ) &&
                  (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE))) ||
                 (!CHECK_FLAG(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_DEVICE_PREFETCH) &&
                  (CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH))))) {

                RestoreIoAccessFlags = TRUE;
                OriginalIoAccessFlags =
                    DeviceInformation.u.BlockIo.IoAccessFlags;

                if (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE)) {
                    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);
                }

                if (CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH)) {
                    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_DEVICE_PREFETCH);
                }

                DeviceStatus =
                    BlDeviceSetInformation(FileTable[FileId]->DeviceId,
                                           &DeviceInformation);

                ASSERT(DeviceStatus == STATUS_SUCCESS);
            }
        }

        ReadRoutine = FileTable[FileId]->FunctionTable.Read;
        Status = ReadRoutine(FileTable[FileId], Buffer, AdjustedSize, Count);

        //
        //  N.B. FileInformation has been updated by a successful read, so the
        //       local copy will be out of synch.
        //

        if (!NT_SUCCESS(Status)) {
            goto BlFileReadExLogRead;
        }
    }

    FileTable[FileId]->BytesRead += AdjustedSize;

BlFileReadExLogRead:

    //
    // Log this read operation.
    //

    BlpFileLogAccess(FileTable[FileId]->DeviceId,
                     FileTable[FileId]->FilePath,
                     FileInformation.FileOffset,
                     AdjustedSize,
                     (NT_SUCCESS(Status) ? BlLogReadAccess :
                                           BlLogReadAccessFailure));

    //
    // Restore the device IO access attributes if necessary.
    //

BlFileReadExEnd:
    if (RestoreIoAccessFlags != FALSE) {
        DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
        DeviceStatus = BlDeviceSetInformation(FileTable[FileId]->DeviceId,
                                              &DeviceInformation);

        ASSERT(DeviceStatus == STATUS_SUCCESS);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileWrite (
    __in FILE_ID FileId,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    This routine writes to a file at the offset specified by the internal
    file pointer.  A write operation will advance the file pointer by the
    number of bytes written.

    N.B. A boot environment filesystem implementation can only support write
         back.  File growth would require interaction with the Operating
         System driver to allow for sector allocation.

    N.B. Not all filesystems will support this routine and may return
         STATUS_NOT_IMPLEMENTED.

Arguments:

    FileId - Supplies a handle to a file.

    Buffer - Supplies a buffer to write to a file.

    Size - Supplies the number of bytes to read to a file.

    Count - Supplies an optional pointer to a variable that receives the number
        of bytes written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with write access.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION FileInformation;
    NTSTATUS Status;
    PFILESYSTEM_WRITE_ROUTINE WriteRoutine;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Ensure that the file handle is valid and the file was opened for write.
    //

    if ((Buffer == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED_FOR_WRITE(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileWriteEnd;
    }

    //
    // Ensure the write operation will not go off the end of the file.
    //

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    Status = FileInformationCheck(&FileInformation,
                                  TRUE,
                                  &AdjustedSize,
                                  Count,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteEnd;
    }

    //
    // Specifically handle a request of zero bytes, avoiding passing such a
    // request to the filesystem stub.  Forward all other requests to the
    // stub.
    //

    if (AdjustedSize == 0) {
        Status = STATUS_SUCCESS;

    } else {
        WriteRoutine = FileTable[FileId]->FunctionTable.Write;

        //
        //  N.B. The file information has been updated by a successful write, so the
        //       local copy may be out of synch.
        //

        Status =  WriteRoutine(FileTable[FileId], Buffer, AdjustedSize, Count);
        if (!NT_SUCCESS(Status)) {
            goto BlFileWriteLogWrite;
        }
    }

    FileTable[FileId]->BytesWritten += AdjustedSize;

BlFileWriteLogWrite:

    //
    // Log the write operation.
    //

    BlpFileLogAccess(FileTable[FileId]->DeviceId,
                     FileTable[FileId]->FilePath,
                     FileInformation.FileOffset,
                     AdjustedSize,
                     (NT_SUCCESS(Status) ? BlLogWriteAccess :
                                           BlLogWriteAccessFailure));

BlFileWriteEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileGetNextFile (
    __in FILE_ID DirectoryId,
    __deref_inout_ecount(*BufferSize) PWCHAR *FileName,
    __inout PULONG BufferSize,
    __in BOOLEAN AllocateBuffer
    )

/*++

Routine Description:

    This routine enumerates the file entries in a directory.

    This routine will return the next file entry in a directory.  Each call
    requires the file name returned by the previous call and the output is the
    next file name.

    To initiate the search, the input file name pointer must point to NULL or
    to a unicode string whose first character is the NULL character.
    (ie. *FileName == NULL || *FileName[0] == L'\0').

    A search is ended with a return value of STATUS_NOT_FOUND, indicating the
    next filename was not found.

Arguments:

    DirectoryId - Supplies a handle to a directory.

    FileName - Supplies a pointer to an unicode string containing the last
        file name returned by this API.  On output, the string contains the
        next file in the directory.

    BufferSize - Supplies a pointer to a variable that contains the size, in
        WCHARs, of the buffer pointed to by FileName.  If
        STATUS_BUFFER_TOO_SMALL is returned by the API, on output, this
        variable contains the required buffer size for the filename.

    AllocateBuffer - Supplies a boolean indicating whether a new buffer should
        be allocated if the supplied file name buffer is too small.

        N.B. If this boolean is set to true and the API allocates a new buffer,
             the current buffer is freed.  This value should not be set to true
             if the existing buffer is a stack buffer or an invalid pointer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND to terminate the enumeration.

    STATUS_BUFFER_TOO_SMALL if the file name buffer is too small.

    STATUS_NOT_A_DIRECTORY if the specified file handle is not a directory.

    STATUS_INVALID_PARAMETER if the specified file handle is invalid or if
        BufferSize is NULL, FileName is NULL.

--*/

{

    PFILESYSTEM_GET_NEXT_FILE_ROUTINE GetNextFile;
    PWCHAR LocalFileName;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Fail if the specified handle is invalid, if the specified handle does
    // not refer to a directory or if BufferSize or FileName are NULL.
    //

    if ((FileEntries <= DirectoryId) ||
        (FileTable[DirectoryId] == NULL) ||
        (BufferSize == NULL) ||
        (FileName == NULL) ||
        (!IS_FILE_OPENED(FileTable[DirectoryId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileGetNextFileEnd;
    }

    if (!IS_FILE_DIRECTORY_SET(FileTable[DirectoryId]->Attributes)) {
        Status = STATUS_NOT_A_DIRECTORY;
        goto BlFileGetNextFileEnd;
    }

    //
    // Use the filesystem stub to determine the next filename.
    //

    GetNextFile = FileTable[DirectoryId]->FunctionTable.GetNextFile;
    Status = GetNextFile(FileTable[DirectoryId], *FileName, BufferSize);

    //
    // If the user's buffer was too small and the caller requested a new buffer,
    // allocate a new one and try the filesystem GetNextFile stub again.
    //

    if ((Status == STATUS_BUFFER_TOO_SMALL) && (AllocateBuffer != FALSE)) {
        LocalFileName = BlMmAllocateHeap(*BufferSize * sizeof(WCHAR));
        if (LocalFileName == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlFileGetNextFileEnd;
        }

        //
        // Copy the current file name into the newly allocated buffer and call
        // into the filesystem stub again.
        //

        if (*FileName != NULL) {
            wcscpy_s(LocalFileName, *BufferSize, *FileName);
            BlMmFreeHeap(*FileName);

        } else {
            LocalFileName[0] = L'\0';
        }

        *FileName = LocalFileName;
        Status = GetNextFile(FileTable[DirectoryId], *FileName, BufferSize);
    }

BlFileGetNextFileEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileGetInformation (
    __in FILE_ID FileId,
    __out PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    This routine queries for file information.

Arguments:

    FileId - Supplies a handle to a file.

    FileInformation - Supplies a pointer to a buffer that is populated with
        file information.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid.

    STATUS_INVALID_PARAMETER if the file information pointer is NULL.

    Other status codes are possible based on the implementation of the
        underlying filesystem.

--*/

{

    PFILESYSTEM_GET_INFORMATION_ROUTINE GetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Parameter checking.  Ensure FileId is valid and that the file is
    // currently opened.
    //

    if ((FileInformation == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Perform the GetInformation operation by calling the filesystem stub.
        //

        GetInformation = FileTable[FileId]->FunctionTable.GetInformation;
        Status = GetInformation(FileTable[FileId], FileInformation);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileSetInformation (
    __in FILE_ID FileId,
    __in PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    Sets file information.

    N.B. Not all file information is configurable (some read-only) and there is
         no specification to the mannar in which the filesystem must handle
         configurable data being modified.  It is recommended to always call
         BlFileGetInformation prior to calling BlFileSetInformation to ensure
         non-configurable fields are properly set.

Arguments:

    FileId - Supplies a handle to a file.

    FileInformation - Supplies the file information to set.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid.

    STATUS_INVALID_PARAMETER if the file information pointer is NULL.

    Other status codes are possible based on the implementation of the
        underlying filesystem.

--*/

{

    PFILESYSTEM_SET_INFORMATION_ROUTINE SetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Parameter checking.  Ensure that FileId is valid and the file is
    // currently opened.
    //

    if ((FileInformation == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Perform SetInformation operation by calling the file system stub.
        //

        SetInformation = FileTable[FileId]->FunctionTable.SetInformation;
        Status = SetInformation(FileTable[FileId], FileInformation);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileMount (
    __in DEVICE_ID ParentDeviceId,
    __in PWSTR FileName,
    __out PDEVICE_ID DeviceId
    )

/*++

Routine Description:

    Attempts to mount a file system on a file treated as a block i/o device.

Arguments:

    ParentDeviceId - Supplies the handle to the device containing the file
        to mount.

    FileName - Supplies the path to the file to mount.

    DeviceId - Supplies a pointer to a variable that receives the handle to
        the file as a block i/o device.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the specified file name is NULL.

    STATUS_INVALID_PARAMETER if the specified device handle is invalid.

    STATUS_NO_SUCH_FILE if the specified file does not exist.

    STATUS_UNSUCCESSFUL if a file system could not be mounted on the specified
        file.

--*/

{

    PVOID Buffer;
    PBOOT_ENVIRONMENT_DEVICE Device;
    FILE_ID FileId;
    SIZE_T FileNameSize;
    PUNALIGNED_FILE_PATH FilePath;
    SIZE_T FilePathLength;
    UINTN FilePathOffset;
    FILE_ID RootDirectoryId;
    PBOOT_ENVIRONMENT_DEVICE ParentDevice;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    FileId = INVALID_FILE_ID;

    //
    // Simple parameter checking.
    //

    if (DeviceId == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlFileMountEnd;
    }

    *DeviceId = INVALID_DEVICE_ID;
    if (FileName == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlFileMountEnd;
    }

    //
    // Attempt to open the specified file on the parent device.
    //

    Status = BlFileOpen(ParentDeviceId, FileName, OPEN_READ_WRITE, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // Get the device identifier for the parent device.  This is necessary
    // to construct the source path for the file for opening it as a device.
    //

    Status = BlGetDeviceIdentifier(ParentDeviceId, &ParentDevice);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // We were successful in opening the file.  Construct the device identifer
    // for a file as a block I/O device.  The identifier contains a full source
    // path to the file to open it as a device.  This includes the source
    // device identifier and the file name.
    //

    FilePathOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u) +
                     FIELD_OFFSET(BLOCK_IO_IDENTIFIER, u);

    FilePathLength = FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath);
    Status = RtlSIZETAdd(FilePathLength, ParentDevice->Size, &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETMult(wcslen(FileName), sizeof(WCHAR), &FileNameSize);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETAdd(FileNameSize, sizeof(UNICODE_NULL), &FileNameSize);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETAdd(FilePathLength, FileNameSize, &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    MARK_SAFE_ADDITION(FilePathOffset, FilePathLength);
    Device = BlMmAllocateHeap(FilePathOffset + FilePathLength);
    if (Device == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlFileMountEnd;
    }

    Device->DeviceType = DEVICE_BLOCK_IO;
    Device->Size = (ULONG)(FilePathOffset + FilePathLength);
    Device->u.BlockIo.BlockIoType = BLOCK_IO_FILE;

    //
    // Construct the file path identifier for the file to be mounted.
    //

    FilePath = &Device->u.BlockIo.u.File.SourcePath;

    ASSERT(FilePathOffset == PtrOffset(Device, FilePath));

    FilePath->Version = FILE_PATH_VERSION;
    FilePath->Type = FILE_PATH_TYPE_BOOT_ENV;
    FilePath->Length = (ULONG)FilePathLength;
    Buffer = &FilePath->FilePath[0];
    RtlCopyMemory(Buffer, ParentDevice, ParentDevice->Size);
    Buffer = Add2Ptr(Buffer, ParentDevice->Size);
    RtlCopyMemory(Buffer, FileName, FileNameSize);

    //
    // Open the file as a block I/O device.
    //

    Status = BlDeviceOpen(Device, OPEN_READ_WRITE, DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // Attempt to see if a file system can be detected for the block i/o file
    // by opening the root directory.
    //

    Status = BlFileOpen(*DeviceId,
                        L"\\",
                        (OPEN_READ | OPEN_DIRECTORY),
                        &RootDirectoryId);

    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // A file system was detected on the block i/o file device.  Close the
    // root directory entry and successfully return to the caller.
    //

    BlFileClose(RootDirectoryId);

BlFileMountEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileId != INVALID_FILE_ID) {
            BlFileClose(FileId);
        }

        if ((DeviceId != NULL) && (*DeviceId != INVALID_DEVICE_ID)) {
            BlDeviceClose(*DeviceId);
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileReadAtOffsetEx (
    __in FILE_ID FileId,
    __in ULONG Size,
    __in ULONGLONG ByteOffset,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead,
    __in ULONG Flags
    )

/*++

Routine Description:

    Reads from a file at the specified offset.

Arguments:

    FileId - Supplies a handle to a file.

    Size - Supplies the number of bytes to read.

    ByteOffset - Supplies the file offset to start the read operation.

    Buffer - Supplies a buffer to receive the contents of the read operation.

    BytesRead - Supplies an optional pointer to a variable that receives the
        number of bytes read.

    Flags - Supplies the flags indicating how the file should be read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with read access.

    STATUS_INVALID_PARAMETER if the requested offset is invalid.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION Information;
    ULONGLONG OriginalFileOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Get the current file offset and other file information.
    //

    Status = BlFileGetInformation(FileId, &Information);
    if (!NT_SUCCESS(Status)) {
        goto BlFileReadAtOffsetEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    OriginalFileOffset = Information.FileOffset;
    Information.FileOffset = ByteOffset;
    Status = FileInformationCheck(&Information,
                                  FALSE,
                                  &AdjustedSize,
                                  BytesRead,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileReadAtOffsetEnd;
    }

    //
    // If the modified file pointer differs from its original value,
    // the file information needs to be updated.
    //

    if (Information.FileOffset != OriginalFileOffset) {
        Status = BlFileSetInformation(FileId, &Information);
        if (!NT_SUCCESS(Status)) {
            goto BlFileReadAtOffsetEnd;
        }
    }

    //
    // Perform the read request on the specified file.
    //

    Status = BlFileReadEx(FileId, Buffer, AdjustedSize, BytesRead, Flags);
    if (!NT_SUCCESS(Status)) {

        //
        // If the read failed, need to restore the file offset back
        // to its original value.
        //
        // N.B. Ignore the status of the Set and return the original error.
        //

        if (Information.FileOffset != OriginalFileOffset) {
            Information.FileOffset = OriginalFileOffset;
            BlFileSetInformation(FileId, &Information);
        }

        goto BlFileReadAtOffsetEnd;
    }

BlFileReadAtOffsetEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileWriteAtOffset (
    __in FILE_ID FileId,
    __in ULONG Size,
    __in ULONGLONG ByteOffset,
    __in_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Writes to a file at the specified offset.

Arguments:

    FileId - Supplies a handle to a file.

    Size - Supplies the number of bytes to read to a file.

    ByteOffset - Supplies the file offset to start the write operation.

    Buffer - Supplies a buffer to write to a file.

    BytesWritten - Supplies an optional pointer to a variable that receives the
        number of bytes written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with write access.

    STATUS_INVALID_PARAMETER if the specified offset is invalid.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION Information;
    ULONGLONG OriginalFileOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Get the current file offset and other file information.
    //

    Status = BlFileGetInformation(FileId, &Information);
    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteAtOffsetEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    OriginalFileOffset = Information.FileOffset;
    Information.FileOffset = ByteOffset;
    Status = FileInformationCheck(&Information,
                                  TRUE,
                                  &AdjustedSize,
                                  BytesWritten,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteAtOffsetEnd;
    }

    //
    // If the updated file pointer differs from its original value,
    // the file information needs to be updated.
    //

    if (Information.FileOffset != OriginalFileOffset) {
        Status = BlFileSetInformation(FileId, &Information);
        if (!NT_SUCCESS(Status)) {
            goto BlFileWriteAtOffsetEnd;
        }
    }

    //
    // Perform the write request on the specified file.
    //

    Status = BlFileWrite(FileId, Buffer, AdjustedSize, BytesWritten);
    if (!NT_SUCCESS(Status)) {

        //
        // If the write failed, restore the file offset to its original value.
        //
        // N.B. Ignore the status of the Set operation and return the original error.
        //

        if (Information.FileOffset != OriginalFileOffset) {
            Information.FileOffset = OriginalFileOffset;
            BlFileSetInformation(FileId, &Information);
        }

        goto BlFileWriteAtOffsetEnd;
    }

BlFileWriteAtOffsetEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

// --------------------------------------------------------------- Private APIs

NTSTATUS
BlpFileInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes data structures used for file i/o (i.e. the file table).

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for internal data structure.

    Other failing error status code possible based on the implementation of the
        registering filesystem.

--*/

{

    const PCFILESYSTEM_REGISTER_FUNCTION_TABLE* FsItem;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Initialize the global file table.
    //

    FileEntries = INITIAL_FILE_ENTRIES;
    FileTable = BlMmAllocateHeap(FileEntries * sizeof(PFILE_ENTRY));
    if (FileTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpFileInitializeEnd;
    }

    Status = STATUS_SUCCESS;
    RtlZeroMemory(FileTable, FileEntries * sizeof(PFILE_ENTRY));
    InitializeListHead(&RegisteredFileSystems);

    //
    // Register the statically implemented file systems.
    //

    for (FsItem = &FsTable[0]; NULL != *FsItem; FsItem += 1) {
        Status = BlpFileRegisterFileSystem((*FsItem)->InitializeRoutine,
                                           (*FsItem)->DestroyRoutine,
                                           (*FsItem)->MountRoutine,
                                           (*FsItem)->PurgeCacheRoutine,
                                           FS_FLAGS_INSERT_AT_TAIL);

        if (!NT_SUCCESS(Status)) {
            goto BlpFileInitializeEnd;
        }
    }


BlpFileInitializeEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileTable != NULL) {
            BlpFileDestroy();
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlpFileDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys all data structures allocated for the file i/o manager.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    Failing error status codes are determined based on the implementation of
        the underlying filesystems (Close + Destroy routines).

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);
    ReturnStatus = STATUS_SUCCESS;

    //
    // Purge each file entry that is in the file table.
    //

    Status = BlTblMap(FileTable, FileEntries, FileTableDestroyEntry, FALSE);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Free memory allocated for the global file table. Set the number of
    // file entries to zero to prevent file APIs from referencing the
    // file table after its been freed.
    //

    BlMmFreeHeap(FileTable);
    FileTable = NULL;
    FileEntries = 0;

    //
    // Free the global registered file system list.
    //

    Status = FileIoDestroyRegisteredFileSystems();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlpInterfaceExit(InterfaceFile);
    return ReturnStatus;
}

NTSTATUS
BlpFileRegisterFileSystem (
    __in PFILESYSTEM_INITIALIZE_ROUTINE InitializeRoutine,
    __in PFILESYSTEM_DESTROY_ROUTINE DestroyRoutine,
    __in PFILESYSTEM_MOUNT_ROUTINE MountRoutine,
    __in_opt PFILESYSTEM_PURGE_CACHE_ROUTINE PurgeCacheRoutine,
    __in ULONG Flags
    )

/*++

Routine Description:

    Registers a filesystem with the file i/o manager.  This module maintains
    a list of registered filesystems.  Each registered filesystem contains
    an initialize routine a destroy routine and a mount routine.  The
    initialize routine is called prior to inserting the filesystem into the
    registered list.  The mount routine is called when attempting to mount a
    filesystem to a file.  Lastly, the destroy routine is called from
    from BlpFileDestroy, giving a each filesystem a chance to destroy data
    structures before the FiloIo subsystem is destroyed.

Arguments:

    InitializeRoutine - Supplies an initialization routine for a file system
        implementation to be called by this routine.

    DestroyRoutine - Supplies a destruction routine for a file system
        implementation to be called during the destruction of the file i/o
        manager.

    MountRoutine - Supplies a mount routine for a file system implementation.

    PurgeCacheRoutine - Supplies a routine that purges all file caches.

    Flags - Registration flags (FS_FLAGS_*).  Valid flags include,

        FS_FLAGS_INSERT_AT_HEAD : Indicates that the filesystem should be
            inserted at the head of the registered list.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for internal data structure.

    Other failing error status code possible based on the implementation of the
        registering filesystem.

--*/

{

    PREGISTERED_FILESYSTEM FileSystem;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Allocate memory for the registered filesystem.
    //

    FileSystem = BlMmAllocateHeap(sizeof(REGISTERED_FILESYSTEM));
    if (FileSystem == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpFileRegisterFileSystemEnd;
    }

    //
    // Give the boot envirionment filesystem implementation a chance to
    // initialize any data structures it has.
    //

    Status = InitializeRoutine();
    if (!NT_SUCCESS(Status)) {
        goto BlpFileRegisterFileSystemEnd;
    }

    //
    // Save the three filesystem routine pointers and insert them into a list
    // with the other registered filesystems.
    //

    FileSystem->FunctionTable.InitializeRoutine = InitializeRoutine;
    FileSystem->FunctionTable.DestroyRoutine = DestroyRoutine;
    FileSystem->FunctionTable.MountRoutine = MountRoutine;
    FileSystem->FunctionTable.PurgeCacheRoutine = PurgeCacheRoutine;
    if (CHECK_FLAG(Flags, FS_FLAGS_INSERT_AT_HEAD)) {
        InsertHeadList(&RegisteredFileSystems, &FileSystem->ListEntry);

    } else {
        InsertTailList(&RegisteredFileSystems, &FileSystem->ListEntry);
    }

BlpFileRegisterFileSystemEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileSystem != NULL) {
            BlMmFreeHeap(FileSystem);
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlpFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __out PFILE_ID FileId,
    __out_opt PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    )

/*++

Routine Description:

    This function opens a file.  In order to open the file, a filesystem must
    be mounted on the file's block i/o device and the file system's function
    table must be associated with the newly opened file.

    In order to 'open' a file, the parent directory must be 'opened' in order
    for a file system to determine the existence of a file and location on the
    block i/o disk of the file.

    If the file is already open and the OpenFlags flags do not conflict, the
    same file descriptor id may be returned and the reference count to the
    file increased.

Arguments:

    DeviceId - Supplies the handle for the containing the file to open.

    OpenPath - Supplies the path to the file to open.

    OpenFlags - Supplies a bitmask containing flags to use when opening the
        file.

    FilterLevel - Supplies the file system filter level on the underlying block
        i/o device.

    FileId - Supplies a pointer to a variable that receives a handle to file.

Return Value:

    STATUS_SUCCESS when successful.  (FileId will also contain the file's
        handle).

    STATUS_REPARSE when file is a reparse point, and actual data should be
        extracted from a backing file in a WIM.

    STATUS_INVALID_PARAMETER if the specified device is invalid (Invalid handle
        or a handle to a non-block i/o device).

    STATUS_INVALID_PARAMETER if the specified file path is invalid (NULL or
        does not start with a '\').

    STATUS_NO_MEMORY if an allocation fails for an internal data structure.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on the underlying
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist.

--*/

{

    DEVICE_INFORMATION Information;
    PFILE_ENTRY FileEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Basic parameter checking.
    //

    if ((OpenPath == NULL) ||
        (!IS_OPEN_BY_HASH_SET(OpenFlags) && (OpenPath[0] != L'\\')) ||
        (FileId == NULL) ||
        (!IS_OPEN_READ_SET(OpenFlags) && !IS_OPEN_WRITE_SET(OpenFlags))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlpFileOpenEnd;
    }

    //
    // Make sure DeviceId is a valid ID and that it refers to a Block I/O
    // Device, a UDP device, or a VMBus device.
    //

    Status = BlDeviceGetInformation(DeviceId, &Information);
    if (!NT_SUCCESS(Status) ||
        ((Information.DeviceType != DEVICE_BLOCK_IO) &&
         (Information.DeviceType != DEVICE_PARTITION) &&
         (Information.DeviceType != DEVICE_UDP) &&
         (Information.DeviceType != DEVICE_VMBUS))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlpFileOpenEnd;
    }

    //
    // Open the requested file.
    //

    Status = FileIoOpen(DeviceId,
                        OpenPath,
                        OpenFlags,
                        FilterLevel,
                        FileTableCompareWithSameAttributes,
                        &FileEntry);

    if (!NT_SUCCESS(Status)) {
        goto BlpFileOpenEnd;
    }

    *FileId = FileEntry->FileId;
    if (IS_FILE_REPARSE_SET(FileEntry->Attributes) != FALSE) {

        NT_ASSERT(IS_OPEN_BY_HASH_SET(OpenFlags) == FALSE);

        if (ARGUMENT_PRESENT(ReparseData)) {

            NT_ASSERT(FileEntry->FunctionTable.GetReparseData != NULL);

            NT_VERIFY(
                NT_SUCCESS(
                    FileEntry->FunctionTable.GetReparseData(FileEntry,
                                                            ReparseData)));
        }

        Status = STATUS_REPARSE;
    }

BlpFileOpenEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

// ------------------------------------------------------- File Table Functions

NTSTATUS
FileTableDestroyEntry (
    __in PFILE_ENTRY FileEntry,
    __in ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the file table by calling the entry's close routine
    and freeing memory used by the entry.

Arguments:

    FileEntry - Entry in the file table.

    Index - Index in the file table for FileEntry.

Return Value:

    STATUS_SUCCESS when successful.
    Failing status code determined by the underlying file system
    implementation.

--*/

{

    NTSTATUS Status;

    ASSERT(FileEntry != NULL);

    //
    // Decrement the reference count on the underlying device.  The converse
    // was done when the entry was inserted in the table to ensure a device
    // would not be closed while an entry existed in the file table.
    //

    Status = BlpDeviceDecrementReference(FileEntry->DeviceId);

    ASSERT(Status == STATUS_SUCCESS);

    //
    // Close the File.
    //

    Status = FileEntry->FunctionTable.Close(FileEntry);

    //
    // Free the entry's memory.
    //

    BlMmFreeHeap(FileEntry);
    FileTable[Index] = NULL;
    return Status;
}

BOOLEAN
FileTableCompareWithSameAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    )

/*++

Routine Description:

    This routine checks to see if an existing entry in the file table
    (FileEntry) is an instance of a previous open request for the same file.
    This routine will only return positively (TRUE) if the OpenFlags of the
    current request and the existing entry are the same.

Arguments:

    FileEntry - Entry in the file table to compare with the other parameters.

    DeviceId - Device Id for the file to be compared against.

    FilePath - Filepath for the file to be compared against.

    OpenFlags - The open flags for the file to be compared against.

    FilterLevel - The filter level for the filesystem the entry should be
        compared against.

Return Value:

    TRUE, if FileEntry is an entry in the file table for an equivalent file to
    that described by (DeviceId, FilePath, OpenFlags).
    FALSE, otherwise.

--*/

{

    FILE_ATTRIBUTES Attributes;

    //
    // Check to see if the FileEntry was opened on the same device as that
    // passed as a parameter.
    //

    if (FileEntry->DeviceId != *DeviceId) {
        return FALSE;
    }

    //
    // Check to see if the filepath's match.
    //

    if (_wcsicmp(FileEntry->FilePath, FilePath) != 0) {
        return FALSE;
    }

    //
    // Check to see if the filter level (filesystem) match.
    //

    if (FileEntry->FilterLevel != *FilterLevel) {
        return FALSE;
    }

    //
    // Check to see if the read/write flags are the same.
    //

    Attributes = FileEntry->Attributes;
    if ((IS_OPEN_READ_SET(*OpenFlags) && !IS_FILE_READ_SET(Attributes)) ||
        (!IS_OPEN_READ_SET(*OpenFlags) && IS_FILE_READ_SET(Attributes)) ||
        (IS_OPEN_WRITE_SET(*OpenFlags) && !IS_FILE_WRITE_SET(Attributes)) ||
        (!IS_OPEN_WRITE_SET(*OpenFlags) && IS_FILE_WRITE_SET(Attributes)) ) {

        return FALSE;
    }

    //
    // Check to see if the directory flag matches directory attribute.
    //

    if ((IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         !IS_FILE_DIRECTORY_SET(Attributes)) ||
        (!IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         IS_FILE_DIRECTORY_SET(Attributes)) ) {

        return FALSE;
    }


    return TRUE;
}

BOOLEAN
FileTableCompareWithSubsetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    )

/*++

Routine Description:

    This routine checks to see if an existing entry in the file table
    (FileEntry) is an instance of a previous open request for the same file.
    This routine will only return positively (TRUE) if the OpenFlags of the
    current request is a subset of those for the existing entry (FileEntry).

Arguments:

    FileEntry - Entry in the file table to compare with the other parameters.

    DeviceId - Device Id for the file to be compared against.

    FilePath - Filepath for the file to be compared against.

    OpenFlags - The open flags for the file to be compared against.

    FilterLevel - The filter level for the filesystem the entry should be
        compared against.


Return Value:

    TRUE, if FileEntry is an entry in the file table for an equivalent file to
    that described by (DeviceId, FilePath, OpenFlags).
    FALSE, otherwise.

--*/

{

    FILE_ATTRIBUTES Attributes;

    //
    // Check to see if the FileEntry was opened on the same device as that
    // passed as a parameter.
    //

    if (FileEntry->DeviceId != *DeviceId) {
        return FALSE;
    }

    //
    // Check to see if the filepath's match.
    //

    if (_wcsicmp(FileEntry->FilePath, FilePath) != 0) {
        return FALSE;
    }

    //
    // Check to see if the filter level (filesystem) match.
    //

    if (FileEntry->FilterLevel != *FilterLevel) {
        return FALSE;
    }

    //
    // Check to see if the read/write flags are a subset of those in FileEntry.
    //

    Attributes = FileEntry->Attributes;
    if ((IS_OPEN_READ_SET(*OpenFlags) && !IS_FILE_READ_SET(Attributes)) ||
        (IS_OPEN_WRITE_SET(*OpenFlags) && !IS_FILE_WRITE_SET(Attributes))) {

        return FALSE;
    }

    //
    // Check to see if the directory flag matches directory attribute.
    //

    if ((IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         !IS_FILE_DIRECTORY_SET(Attributes)) ||
        (!IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         IS_FILE_DIRECTORY_SET(Attributes)) ) {

        return FALSE;
    }

    return TRUE;
}

NTSTATUS
FileTablePurgeEntry (
    __in PFILE_ENTRY FileEntry
    )

/*++

Routine Description:

    The File Table is purged of entries to a file with no open handles.
    However, a directory entry will not be purged from the table.  The
    directory entry is kept in table for perfomance reasons.  If multiple files
    are read from the same directory, we want to avoid having to open their
    parent directory twice.  This becomes more important when dealing with
    files deap in a directory structure.

Arguments:

    DeviceEntry - Pointer to a device entry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_UNSUCCESSFUL if the specified file is currently opened or is a
        directory.
    Other failing error status codes are possible depending on the
    implementation of the underlying file system.

--*/

{

    //
    // Purge a file entry only if it has no open handles.  Never purge a
    // directory entry.  Never purge the root directory, because that
    // will trigger a remount.
    //

    if (!IS_FILE_OPENED(FileEntry->Attributes) &&
        (!IS_FILE_DIRECTORY_SET(FileEntry->Attributes) ||
         (CAPPED_FILE_ENTRIES <= FileEntries)) &&
         wcscmp(FileEntry->FilePath, L"\\") != 0) {

        return FileTableDestroyEntry(FileEntry, FileEntry->FileId);
    }

    return STATUS_UNSUCCESSFUL;
}

// ------------------------------------------------- File I/O Library Functions

NTSTATUS
FileIoOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __in PTABLE_COMPARE_ENTRY CompareRoutine,
    __deref_opt_out PFILE_ENTRY *ReturnFileEntry
    )

/*++

Routine Description:

    Internal File I/O routine to open a file.  This routine will attempt to
    use an existing FileEntry from the file table (using CompareRoutine when
    calling BlTblFindEntry).  If an existing entry is not found, then
    this routine will open the file by either Mounting a File System on the
    Block I/O Device (if OpenPath is the root directory) or by opening the
    parent directory and using the File System's open routine to read the
    parent directory and open the file described by OpenPath.

Arguments:

    DeviceId - The Device Id for the file's parent block i/o device.

    OpenPath - The path on the device for the file to be opened.

    OpenFlags - The flags the file should be opened with.

    FilterLevel - Specifies the filesystem on the parent device the request
        should be made.

    CompareRoutine - The File Table Comparison Routine to use when determine
        if an existing FileEntry can be used.

    ReturnFileEntry - Pointer to FileEntry buffer for the newly opened file.
        (Set on output when Open operation is successful).  OPTIONAL

Return Value:

    STATUS_SUCCESS when successsful.  FileEntry will point to the File Entry
        for the file described by DeviceId, OpenPath, OpenFlags.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_ACCESS_DENIED if the supplied device does not carry one or more of
        the file access rights requested in the open flags.

--*/

{

    NTSTATUS CloseStatus;
    PFILE_ENTRY FileEntry;
    FILE_ID FileId;
    PWCHAR FileName;
    PFILE_ENTRY ParentDirectoryEntry;
    PWCHAR ParentDirectoryPath;
    BOOLEAN ReparsePoint;
    BOOLEAN Result;
    NTSTATUS Status;

    ParentDirectoryEntry = NULL;
    ParentDirectoryPath = NULL;
    FileName = NULL;
    Status = STATUS_SUCCESS;
    ReparsePoint = FALSE;

    //
    // All access rights requested in the open flags must be available on the
    // underlying device.  Exit if the supplied device does not carry one or
    // more of the requested access rights.
    //
    // N.B. Read access to the underlying device is required regardless of the
    //      access mode requested by the caller since opening an existing file
    //      on disk involves accesses to the filesystem metadata.
    //

    Result = BlpDeviceTestAccessMode(DeviceId, OpenFlags | OPEN_READ);
    if (Result == FALSE) {
        Status = STATUS_ACCESS_DENIED;
        goto FileOpenEnd;
    }

    //
    // Check to see if there is already a file opened with compatible
    // file attributes.
    //

    FileEntry = BlTblFindEntry(FileTable,
                               FileEntries,
                               &FileId,
                               CompareRoutine,
                               &DeviceId,
                               OpenPath,
                               &OpenFlags,
                               &FilterLevel);

    //
    // If we found a compatible File Entry, we can return that pointer to the
    // caller.  Otherwise, we will have to actually go and do the work to open
    // the file.
    //

    if (FileEntry == NULL) {

        //
        // If we are trying to open the root directory of a device, then
        // attempt to detect what File System exists on the device.
        //
        // Otherwise, attempt to open the parent directory in order to
        // inherit a file system function table.
        //

        if ((OpenPath[0] == L'\\') && (OpenPath[1] == L'\0')) {

            //
            // Attempt to mount a file system on the parent block i/o device.
            //

            Status = FileIoMountFileSystem(DeviceId, FilterLevel, &FileEntry);
            if (!NT_SUCCESS(Status)) {
                goto FileOpenEnd;
            }

        } else {

            //
            // Allocate a buffer for the parent directory's path.
            //

            if (!IS_OPEN_BY_HASH_SET(OpenFlags)) {
                ParentDirectoryPath = FileIoCopyParentDirectoryPath(OpenPath);
                if (ParentDirectoryPath == NULL) {
                    Status = STATUS_NO_MEMORY;
                    goto FileOpenEnd;
                }

            } else {
                ParentDirectoryPath = L"\\";
            }

            //
            // Attempt to open the parent directory.  The parent directory only
            // needs to be opened for read.
            //

            Status = FileIoOpen(DeviceId,
                                ParentDirectoryPath,
                                (OPEN_READ | OPEN_DIRECTORY),
                                FilterLevel,
                                FileTableCompareWithSubsetAttributes,
                                &ParentDirectoryEntry);

            if (!NT_SUCCESS(Status)) {
                goto FileOpenEnd;
            }

            //
            // Allocate a buffer for the relative filename for the file being
            // opened.  (The file system open routine requires the File Entry
            // for the parent directory and the relative file name for the file
            // being opened.
            //

            if (!IS_OPEN_BY_HASH_SET(OpenFlags)) {
                FileName = FileIoCopyFileName(OpenPath);
                if (FileName == NULL) {
                    Status = STATUS_NO_MEMORY;
                    goto FileOpenEnd;
                }

            } else {
                FileName = OpenPath;
            }

            //
            // Call the file system Open Routine with both the Parent Directory
            // and the relative filename.
            //

            Status = ParentDirectoryEntry->FunctionTable.Open(
                ParentDirectoryEntry,
                FileName,
                OpenFlags,
                &FileEntry);

            if (!NT_SUCCESS(Status)) {
                goto FileOpenEnd;
            }
        }

        //
        // Record if the file is a reparse point.
        //

        if (Status == STATUS_REPARSE) {
            ReparsePoint = TRUE;
        }

        //
        // Set the filter level.
        //

        FileEntry->FilterLevel = FilterLevel;

        //
        // A File Entry has been created for the file.  Set the attribute flags
        // for the file.
        //

        FileIoSetAttributes(FileEntry, OpenFlags);

        //
        // Add the File Entry to the FileTable.
        //

        Status = BlTblSetEntry(&FileTable,
                               &FileEntries,
                               FileEntry,
                               &FileId,
                               FileTablePurgeEntry);

        if (!NT_SUCCESS(Status)) {
            CloseStatus = FileEntry->FunctionTable.Close(FileEntry);
            ASSERT(CloseStatus == STATUS_SUCCESS);
            goto FileOpenEnd;
        }

        //
        // Increment the reference count on the underlying device in effort to
        // prevent a device closing when there exists an entry in the file
        // table with a valid handle to the device.
        //

        Status = BlpDeviceIncrementReference(DeviceId);

        ASSERT(Status == STATUS_SUCCESS);

        FileEntry->FileId = FileId;
    }

    //
    // At this point, a file entry has been created and is present in the File
    // Table.  Increase the reference count for the file and ensure the open
    // attribute is set.
    //

    FileEntry->ReferenceCount += 1;
    if (FileEntry->ReferenceCount == 1) {
        FileEntry->BytesRead = 0;
        FileEntry->BytesWritten = 0;
    }

    SET_FILE_OPEN(FileEntry->Attributes);
    if (IS_OPEN_CATALOG_SET(OpenFlags) != FALSE) {
        SET_FILE_CATALOG(FileEntry->Attributes);
    }

    if (ReparsePoint != FALSE) {
        SET_FILE_REPARSE(FileEntry->Attributes);
    }

    if (ReturnFileEntry != NULL) {
        *ReturnFileEntry = FileEntry;
    }

FileOpenEnd:

    //
    // Close the File Entry created for the Parent Directory.
    //

    if (ParentDirectoryEntry != NULL) {
        BlFileClose(ParentDirectoryEntry->FileId);
    }

    if (!IS_OPEN_BY_HASH_SET(OpenFlags)) {

        //
        // Free the memory allocaed for the Parent Directory's File Path.  It was
        // only needed for the pFileOpen call.  The associated File System Mount
        // Routine or Open Routine will allocate a new buffer for the File Path
        // when creating a File Entry for the file.
        //

        if (ParentDirectoryPath != NULL) {
            BlMmFreeHeap(ParentDirectoryPath);
        }

        //
        // Free the memory allocated for the relative file name of the file being
        // opened.  It was only needed for the filesystem Open routine, which will
        // allocate a buffer for the full filepath based on the relative filename
        // and the parent directory.
        //

        if (FileName != NULL) {
            BlMmFreeHeap(FileName);
        }
    }

    return Status;
}

NTSTATUS
FileIoMountFileSystem (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    )

/*++

Routine Description:

    Attempts to mount a file system on a block i/o device.  There is a global
    linked list of registered file systems.  This routine will walk the list,
    calling the registered mount routine for each, returning after the first
    successful return value.  In this case, the file system mount code will
    have allocated and initialized a File Entry for the root directory of the
    mounted file system.

Arguments:

    DeviceId - The device id for the Block I/O Device to mount.

    FilterLevel - Indicates the filter location of the mount request (zero
        indicates this is the first mount request on the device).

    FileEntry - If a file system successfully detects its presence on the block
        i/o device, it will allocate and initialize a File Entry for the root
        directory of the file system.  FileEntry is a pointer to store the
        address of the allocated File Entry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_UNSUCCESSFUL if a filesystem can not be mounted on the specified
        block i/o device.

--*/

{

    PREGISTERED_FILESYSTEM FileSystem;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    //
    // Iterate through the list of registered filesystems, attempting to mount
    // each on the file.  Stop once one's mount routine returns successfully.
    //

    ReturnStatus = STATUS_UNSUCCESSFUL;
    if (IsListEmpty(&RegisteredFileSystems)) {

        NT_ASSERTMSG("FileSystem list is empty", FALSE);

        goto MountFileSystemEnd;
    }

    NextEntry = RegisteredFileSystems.Flink;
    do {

        FileSystem = CONTAINING_RECORD(NextEntry,
                                       REGISTERED_FILESYSTEM,
                                       ListEntry);

        Status = FileSystem->FunctionTable.MountRoutine(DeviceId,
                                                        FilterLevel,
                                                        FileEntry);

        if (Status != STATUS_UNSUCCESSFUL) {
            ReturnStatus = Status;
        }

        NextEntry = NextEntry->Flink;
    } while (!NT_SUCCESS(Status) && (NextEntry != &RegisteredFileSystems));

MountFileSystemEnd:
    return ReturnStatus;
}

NTSTATUS
BlFilePurgeAllCaches (
    VOID
    )

/*++

Routine Description:

    This routine purges all file record segment caches, freeing their associated
    resources.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL if a filesystem can not have its cache purged.

--*/

{

    PREGISTERED_FILESYSTEM FileSystem;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    //
    // Call through each filesystem that is mounted and purge its cache if
    // possible.
    //

    ReturnStatus = STATUS_SUCCESS;
    NextEntry = RegisteredFileSystems.Flink;
    while (NextEntry != &RegisteredFileSystems) {
        FileSystem = CONTAINING_RECORD(NextEntry,
                                       REGISTERED_FILESYSTEM,
                                       ListEntry);

        if (FileSystem->FunctionTable.PurgeCacheRoutine != NULL) {
            Status = FileSystem->FunctionTable.PurgeCacheRoutine();

            //
            // Save the last error that occurred.
            //

            if (!NT_SUCCESS(Status)) {
                ReturnStatus = Status;
            }
        }

        NextEntry = NextEntry->Flink;
    }

    return ReturnStatus;
}

NTSTATUS
FileIoDestroyRegisteredFileSystems (
    VOID
    )

/*++

Routine Description:

    This routine walks the list of registered filesytems, calling the destroy
    routine for each.  If there is an error, this routine will continue to call
    the remaining destroy routines.  The last error status code received will
    be the one returned.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    Other failing error status code possible based on the implementation of the
        registering filesystem.

--*/

{

    PREGISTERED_FILESYSTEM FileSystem;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    PLIST_ENTRY TempEntry;

    ReturnStatus = STATUS_SUCCESS;

    //
    // Walk the list of registered filesytems calling the destory routine for
    // each.
    //

    NextEntry = RegisteredFileSystems.Flink;
    while (NextEntry != &RegisteredFileSystems) {
        FileSystem = CONTAINING_RECORD(NextEntry,
                                       REGISTERED_FILESYSTEM,
                                       ListEntry);

        Status = FileSystem->FunctionTable.DestroyRoutine();

        //
        // Save the last error that occurred.
        //

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        //
        // Remove entry from the list.
        //

        TempEntry = NextEntry;
        NextEntry = NextEntry->Flink;
        RemoveEntryList(TempEntry);

        //
        // Free allocated buffer for the registered file system.
        //

        BlMmFreeHeap(FileSystem);
    }

    return ReturnStatus;
}

VOID
FileIoSetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in OPEN_FLAGS OpenFlags
    )

/*++

Routine Description:

    Sets a FileEntry's file attributes based on the flags set in OpenFlags.

Arguments:

    FileEntry - Pointer to file entry structure for an opened file.

    OpenFlags - Flags that are being used to open the file specified by
        FileEntry.

Return Value:

    None.

--*/

{

    if (IS_OPEN_READ_SET(OpenFlags)) {
        SET_FILE_READ(FileEntry->Attributes);
    }

    if (IS_OPEN_WRITE_SET(OpenFlags)) {
        SET_FILE_WRITE(FileEntry->Attributes);
    }
}

PWCHAR
FileIoCopyParentDirectoryPath (
    __in PWSTR FilePath
    )

/*++

Routine Description:

    Allocates a buffer and copies FilePath's parent directory to the buffer.
    The caller is responsible for freeing memory allocated by this routine.

    Note: NULL is returned if the buffer can not be allocated.  A string with
    the first character the null character is returned if there is no parent
    directory (ie. FilePath == '\').

Arguments:

    FilePath - The Full directory path to a file.

Return Value:

    An allocated buffer with the parent directory file path.
    NULL if the buffer could not be allocated.
    NULL if the specified file path is improperly formatted.

--*/

{

    PWCHAR Last;
    PWCHAR ParentDirPath;
    SIZE_T ParentDirPathSize;
    NTSTATUS Status;

    //
    // Allocate a memory buffer for the parent directory name.  It can be no
    // lareger than the string length for FilePath.  Allocate a buffer that is
    // this large.  It may waste a few bytes, but since the memory can be
    // freed, it is not a big deal.
    //

    Status = RtlSIZETMult(wcslen(FilePath), sizeof(WCHAR), &ParentDirPathSize);
    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    Status = RtlSIZETAdd(ParentDirPathSize,
                         sizeof(UNICODE_NULL),
                         &ParentDirPathSize);

    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    ParentDirPath = BlMmAllocateHeap(ParentDirPathSize);
    if (ParentDirPath == NULL) {
        return NULL;
    }

    //
    // Copy the entire FilePath into ParentDirPath.  Therefore, no
    // manipulations need to be done to FilePath in order to file the parent
    // directory name.
    //

    wcsncpy_s(ParentDirPath,
              (ParentDirPathSize / sizeof(WCHAR)),
              FilePath,
              ((ParentDirPathSize / sizeof(WCHAR)) - 1));

    //
    // Get a pointer to the last occurrence of '\'.  This represents the
    // current file.  If this is replaced with the null character, the parent
    // directory remains.  If the string is improperly formatted (no occurrence
    // of '\', return with an error.
    //

    Last = wcsrchr(ParentDirPath, L'\\');
    if (Last == NULL) {
        BlMmFreeHeap(ParentDirPath);
        return NULL;
    }

    //
    // If the last occurrence of '\' was for the root directory, increment the
    // Last pointer, so that the resulting parent directory is the root
    // directory.
    //

    if (Last == ParentDirPath) {
        Last += 1;
    }

    //
    // Replace the last occurrence of '\' with the null character, leaving the
    // parent directory.
    //

    *Last = L'\0';
    return ParentDirPath;
}

PWCHAR
FileIoCopyFileName (
    __in PWSTR FilePath
    )

/*++

Routine Description:

    Allocates a buffer and copies FilePath's relative filename into a buffer.
    The caller is responsible for freeing memory allocated by this routine.

    The filepath passed into this routine is a full NT style directory path
    (ie. \foo\bar\foobar.c).  This routine allocates a buffer for the filename
    at the base of the directory structure (foobar.c).

    Note: NULL is returned if the buffer can not be allocated.

Arguments:

    FilePath - The Full directory path to a file.

Return Value:

    An allocated buffer with the relative filename.
    NULL if the buffer could not be allocated.
    NULL if the file path is improperly formatted.

--*/

{

    PWCHAR Last;
    PWCHAR FileName;
    SIZE_T LastSize;
    NTSTATUS Status;

    //
    // Get a pointer to the last occurrence of '\'.  This represents the
    // current file.
    //

    Last = wcsrchr(FilePath, L'\\');
    if (Last == NULL) {
        return NULL;
    }

    //
    // Allocate a memory buffer for the relative filename.
    //

    Status = RtlSIZETMult(wcslen(Last), sizeof(WCHAR), &LastSize);
    if (!NT_SUCCESS(Status)) {
        return NULL;
    }

    FileName = BlMmAllocateHeap(LastSize);
    if (FileName == NULL) {
        return NULL;
    }

    //
    // Copy the entire string starting at Last + 1 (Last points to the last
    // occurrence of the character '\'.
    //

    wcscpy_s(FileName, (LastSize / sizeof(WCHAR)), Last + 1);
    return FileName;
}

NTSTATUS
FileIoPrint (
    __in PFILE_ENTRY FileEntry,
    __in FILE_ID FileId
    )

/*++

Routine Description:

    For Debug Purposes.

    Prints out a File Entry.

Arguments:

    FileEntry - Pointer to a file entry structure.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    FILE_INFORMATION Information;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(FileId);

    BlDisplayPrintString(L"FileEntry for Device %d Level %d File %s\r\n",
                         FileEntry->DeviceId,
                         FileEntry->FilterLevel,
                         FileEntry->FilePath);

    BlDisplayPrintString(L"FileId: %d\r\n", FileEntry->FileId);
    BlDisplayPrintString(L"Attributes: ");
    if (IS_FILE_OPENED(FileEntry->Attributes)) {
        BlDisplayPrintString(L"Open ");
    }

    if (IS_FILE_READ_SET(FileEntry->Attributes)) {
        BlDisplayPrintString(L"Read ");
    }

    if (IS_FILE_WRITE_SET(FileEntry->Attributes)) {
        BlDisplayPrintString(L"Write ");
    }

    if (IS_FILE_DIRECTORY_SET(FileEntry->Attributes)) {
        BlDisplayPrintString(L"Directory ");
    }

    BlDisplayPrintString(L"\r\n");
    BlDisplayPrintString(L"ReferenceCount: %d\r\n", FileEntry->ReferenceCount);
    Status = BlFileGetInformation(FileEntry->FileId, &Information);
    if (!NT_SUCCESS(Status)) {
        BlDisplayPrintString(L"BlFileGetInformation failed with status 0x%x\r\n",
                             Status);

    } else {
        BlDisplayPrintString(L"Information.Flags: %08x\r\n",
                             Information.Flags);
        BlDisplayPrintString(L"Information.FileSize: 0x%08x %08x\r\n",
                             (ULONG)(Information.FileSize >> 32),
                             (ULONG)(Information.FileSize));
        BlDisplayPrintString(L"Information.FileOffset: 0x%08x %08x\r\n",
                             (ULONG)(Information.FileOffset >> 32),
                             (ULONG)(Information.FileOffset));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FileInformationCheck (
    __in PFILE_INFORMATION FileInformation,
    __in BOOLEAN IsWriteOperation,
    __in_opt PULONG InputSize,
    __out_opt PULONG ByteCountRequested,
    __out_opt PULONG OutputAdjustedSize
    )

/*++

Routine Description:

    This function performs a consistency check on fields of the provided
    file information block that are used when performing read and write
    operations. It can optionally calculate an adjusted size that takes
    into consideration the current file offset and the requested read or
    write size. For write operations, it ensures that the new FileSize
    will not cause an overflow, resulting in invalid file context.

Arguments:

    FileInformation - Supplies the address of a FILE_INFORMATION structure
        that the caller would like to have validated for a read or write.

    IsWriteOperation - Supplies an indication that this check is being
        performed as part of a write operation, as opposed to a read.

    InputSize - Supplies the optional address of the size of the read or write
        operation that the caller would like validated, and possibly
        adjusted to consider end-of-file.

    ByteCountRequested - Supplies the optional address where the caller WOULD
        RETURN the actual size of a successful subsequent read or write
        operation, if it is present (passed by the caller). This routine
        senses ByteCountRequested, but does not use it to return a value.
        The caller of this routine is expected to return any appropriate
        value upon completion of the read or write operation.

    OutputAdjustedSize - Supplies the optional address where the size that
        is adjusted for the end-of-file can be returned to the caller.
        Note that while ByteCountRequested reflects an argument passed TO
        THE CALLER, this argument is passed BY THE CALLER.

    N.B.    The presence of the InputSize and ByteCountRequested arguments
            will affect the behavior of this function, as follows:

        InputSize   OutputSize-
         Supplied    Present            Expected Behavior
        ---------   ---------   -----------------------------------------
           No          No       - FileInformation is checked.
           No          Yes      - FileInformation is checked.
                                - OutputAdjustedSize returns the maximum number
                                  of bytes that can be read or written.
           Yes         No       - FileInformation is checked.
                                - If a read crosses FileSize, an error.
                                - If a write would overflow FileSize, an
                                  error.
           Yes         Yes      - FileInformation is checked.
                                - If a read crosses FileSize, OutputAdjustedSize
                                  returns the lesser of (1) InputSize, and
                                  (2) the number of bytes between FileOffset
                                  and FileSize.
                                - If a write would overflow FileSize, an
                                  error, otherwise OutputAdjustedSize returns the
                                  InputSize.

    N.B.    While OutputAdjustedSize is modified, ByteCountRequested is not.

Return Value:

    STATUS_SUCCESS when all checks and calculations succeed.

    STATUS_INVALID_PARAMETER when the file information's current offset and
        the file's size are not consistent.

    STATUS_INVALID_PARAMETER when, for a read operation, the amount of data
        being read exceeds the file's size, and the caller did not supply the
        output size output argument. (The caller would not be able to
        determine when the routine returned less data than was requested.)

    STATUS_INVALID_PARAMETER when, for a write operation, the amount of data
        being written would cause the file's size to overflow.

--*/

{

    ULONG AdjustedSize;
    ULONG Size;
    NTSTATUS Status;

    //
    // Ensure that the overflow detection logic is correct.
    //

    ASSERT(sizeof(FileInformation->FileOffset) == sizeof(FileInformation->FileSize));
    ASSERT(sizeof(AdjustedSize) <= sizeof(FileInformation->FileSize));
    ASSERT(sizeof(Size) <= sizeof(FileInformation->FileSize));

    AdjustedSize = 0;

    //
    // Perform FileInformation validation:
    //      - That the current position in the file is proper with respect to its size.
    //

    if (FileInformation->FileOffset > FileInformation->FileSize) {
        Status = STATUS_INVALID_PARAMETER;
        goto FileInformationCheckEnd;
    }

    //
    // Determine how much data is present from the current position to the
    // end of the file, keeping the result within the limits of the 32-bit
    // size argument.
    //

    if ((FileInformation->FileSize - FileInformation->FileOffset) > (ULONGLONG)ULONG_MAX) {
        AdjustedSize = ULONG_MAX;

    } else {
        AdjustedSize = (ULONG)(FileInformation->FileSize - FileInformation->FileOffset);
    }

    //
    // If the operation size argument is supplied, perform additional size checking
    // and necessary adjustment.
    //
    //  N.B. The file's size is known to be greater or equal to current position.
    //

    if (ARGUMENT_PRESENT(InputSize) != FALSE) {
        Size = *InputSize;
        if (AdjustedSize < Size) {
            if (ARGUMENT_PRESENT(ByteCountRequested) == FALSE) {

                //
                // The size to read or write exceeds what is remaining to the end of
                // the file and the caller can't be returned a proper size to use.
                // Return an error, recommending that the caller doesn't perform the
                // operation. The adjusted size is irrelevant since the caller's caller
                // did not supply the necessary output argument.
                //

                Status = STATUS_INVALID_PARAMETER;
                goto FileInformationCheckEnd;
            }

            //
            // The library does not support file extension. Any attempts to
            // write past the end-of-file will be rejected.
            //

            if (IsWriteOperation != FALSE) {
                Status = STATUS_INVALID_PARAMETER;
                goto FileInformationCheckEnd;
            }

            //
            // Caller can return a size that is smaller than the requested
            // size. Keep and use the adjusted size.
            //

        } else {

            //
            // Caller requested less than what remains in the file. Use
            // the requested size instead.
            //

            AdjustedSize = Size;
        }
    }

    Status = STATUS_SUCCESS;

FileInformationCheckEnd:
    if (ARGUMENT_PRESENT(OutputAdjustedSize) != FALSE) {
        *OutputAdjustedSize = AdjustedSize;
    }

    return Status;
}

#ifdef NTKERNEL

VOID
BlCloseOpenFiles (
    VOID
    )
{

    BL_PAGED_CODE();

    BlpInterfaceEnter(InterfaceFile);
    BlTblMap(FileTable, FileEntries, FileTableDestroyEntry, FALSE);
    BlpInterfaceExit(InterfaceFile);
    return;
}

#endif

