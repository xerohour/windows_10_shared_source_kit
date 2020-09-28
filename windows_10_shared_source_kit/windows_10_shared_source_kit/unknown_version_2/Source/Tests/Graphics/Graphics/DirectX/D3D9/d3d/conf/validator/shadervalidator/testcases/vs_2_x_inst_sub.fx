against.

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

#include "Test_Include.fx"

int Test_Count = 449;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_x_inst_sub";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_x : sub source reg combination v0, r0 is allowed";
string VS_001 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"sub r0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_x : sub source reg combination v0, c0 is allowed";
string VS_002 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_x : sub source reg combination r0, v0 is allowed";
string VS_003 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"sub r0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_x : sub source reg combination r0, r1 is allowed";
string VS_004 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"sub r0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_x : sub source reg combination r0, c0 is allowed";
string VS_005 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"sub r0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_x : sub source reg combination c0, v0 is allowed";
string VS_006 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_x : sub source reg combination c0, r0 is allowed";
string VS_007 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"sub r0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_x : sub source reg combination -c0, -v0 is allowed";
string VS_008 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, -c0, -v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_x : sub source reg combination -c0, v0 is allowed";
string VS_009 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, -c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_x : sub source reg combination c0, -v0 is allowed";
string VS_010 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, c0, -v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_x : sub source reg combination c0, v0 is allowed";
string VS_011 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_2_x : sub source reg combination c0.x, v0 is allowed";
string VS_012 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"sub r0, c0.x, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_2_x : sub source reg combination c0.y, v0 is allowed";
string VS_013 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.y, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_2_x : sub source reg combination c0.z, v0 is allowed";
string VS_014 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.z, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_2_x : sub source reg combination c0.w, v0 is allowed";
string VS_015 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.w, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_2_x : sub source reg combination c0.yzxw, v0 is allowed";
string VS_016 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.yzxw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_2_x : sub source reg combination c0.zxyw, v0 is allowed";
string VS_017 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.zxyw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_2_x : sub source reg combination c0.wzyx, v0 is allowed";
string VS_018 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.wzyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_2_x : sub source reg combination c0.wyxz, v0 is allowed";
string VS_019 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.wyxz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_x : sub source reg combination c0.xzyw, v0 is allowed";
string VS_020 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.xzyw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_x : sub source reg combination c0.xywz, v0 is allowed";
string VS_021 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.xywz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_x : sub source reg combination c0.zyx, v0 is allowed";
string VS_022 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.zyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_x : sub source reg combination c0.xzy, v0 is allowed";
string VS_023 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.xzy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_x : sub source reg combination c0.ywx, v0 is allowed";
string VS_024 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.ywx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_x : sub source reg combination c0.yx, v0 is allowed";
string VS_025 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.yx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_x : sub source reg combination c0.wz, v0 is allowed";
string VS_026 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.wz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_x : sub source reg combination c0.zy, v0 is allowed";
string VS_027 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0.zy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_x : sub source reg combination c0, v0.x is allowed";
string VS_028 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_x : sub source reg combination c0, v0.y is allowed";
string VS_029 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_x : sub source reg combination c0, v0.z is allowed";
string VS_030 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_x : sub source reg combination c0, v0.w is allowed";
string VS_031 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_x : sub source reg combination c0, v0.yzxw is allowed";
string VS_032 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_x : sub source reg combination c0, v0.zxyw is allowed";
string VS_033 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_x : sub source reg combination c0, v0.wzyx is allowed";
string VS_034 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_x : sub source reg combination c0, v0.wyxz is allowed";
string VS_035 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_x : sub source reg combination c0, v0.xzyw is allowed";
string VS_036 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_x : sub source reg combination c0, v0.xywz is allowed";
string VS_037 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_x : sub source reg combination c0, v0.zyx is allowed";
string VS_038 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_x : sub source reg combination c0, v0.xzy is allowed";
string VS_039 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_x : sub source reg combination c0, v0.ywx is allowed";
string VS_040 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_x : sub source reg combination c0, v0.yx is allowed";
string VS_041 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_x : sub source reg combination c0, v0.wz is allowed";
string VS_042 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_x : sub source reg combination c0, v0.zy is allowed";
string VS_043 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"sub r0, c0, v0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_x : sub source reg combination v0, v1 is NOT allowed";
string VS_044 = 
	"vs_2_x "
	"dcl_color v0 "
	"dcl_fog v1 "
	"sub r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_x : sub source reg combination c0, c1 is NOT allowed";
string VS_045 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"sub r0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_x : sub source reg combination a0, a0 is NOT allowed";
string VS_046 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_x : sub source reg combination a0, b0 is NOT allowed";
string VS_047 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"sub r0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_x : sub source reg combination a0, i0 is NOT allowed";
string VS_048 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"sub r0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_x : sub source reg combination a0, aL is NOT allowed";
string VS_049 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"sub r0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_050_Desc = "vs_2_x : sub source reg combination a0, p0 is NOT allowed";
string VS_050 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_x : sub source reg combination a0, oPos is NOT allowed";
string VS_051 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_x : sub source reg combination a0, oFog is NOT allowed";
string VS_052 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_x : sub source reg combination a0, oPts is NOT allowed";
string VS_053 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"sub r0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_x : sub source reg combination a0, oD0 is NOT allowed";
string VS_054 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_x : sub source reg combination a0, oT0 is NOT allowed";
string VS_055 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_x : sub source reg combination b0, a0 is NOT allowed";
string VS_056 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"sub r0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_x : sub source reg combination b0, b0 is NOT allowed";
string VS_057 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_x : sub source reg combination b0, i0 is NOT allowed";
string VS_058 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"sub r0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_x : sub source reg combination b0, aL is NOT allowed";
string VS_059 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"sub r0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_060_Desc = "vs_2_x : sub source reg combination b0, p0 is NOT allowed";
string VS_060 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_x : sub source reg combination b0, oPos is NOT allowed";
string VS_061 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_x : sub source reg combination b0, oFog is NOT allowed";
string VS_062 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_x : sub source reg combination b0, oPts is NOT allowed";
string VS_063 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"sub r0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_x : sub source reg combination b0, oD0 is NOT allowed";
string VS_064 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_x : sub source reg combination b0, oT0 is NOT allowed";
string VS_065 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_x : sub source reg combination i0, a0 is NOT allowed";
string VS_066 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"sub r0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_x : sub source reg combination i0, b0 is NOT allowed";
string VS_067 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_x : sub source reg combination i0, i0 is NOT allowed";
string VS_068 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_x : sub source reg combination i0, aL is NOT allowed";
string VS_069 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"sub r0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_070_Desc = "vs_2_x : sub source reg combination i0, p0 is NOT allowed";
string VS_070 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_x : sub source reg combination i0, oPos is NOT allowed";
string VS_071 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_x : sub source reg combination i0, oFog is NOT allowed";
string VS_072 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_x : sub source reg combination i0, oPts is NOT allowed";
string VS_073 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"sub r0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_x : sub source reg combination i0, oD0 is NOT allowed";
string VS_074 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_x : sub source reg combination i0, oT0 is NOT allowed";
string VS_075 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_x : sub source reg combination aL, a0 is NOT allowed";
string VS_076 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"sub r0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_x : sub source reg combination aL, b0 is NOT allowed";
string VS_077 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"sub r0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_x : sub source reg combination aL, i0 is NOT allowed";
string VS_078 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"sub r0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_x : sub source reg combination aL, aL is NOT allowed";
string VS_079 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_080_Desc = "vs_2_x : sub source reg combination aL, p0 is NOT allowed";
string VS_080 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_x : sub source reg combination aL, oPos is NOT allowed";
string VS_081 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_x : sub source reg combination aL, oFog is NOT allowed";
string VS_082 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_x : sub source reg combination aL, oPts is NOT allowed";
string VS_083 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"sub r0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_x : sub source reg combination aL, oD0 is NOT allowed";
string VS_084 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_x : sub source reg combination aL, oT0 is NOT allowed";
string VS_085 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_086_Desc = "vs_2_x : sub source reg combination p0, a0 is NOT allowed";
string VS_086 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_087_Desc = "vs_2_x : sub source reg combination p0, b0 is NOT allowed";
string VS_087 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_088_Desc = "vs_2_x : sub source reg combination p0, i0 is NOT allowed";
string VS_088 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_089_Desc = "vs_2_x : sub source reg combination p0, aL is NOT allowed";
string VS_089 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_090_Desc = "vs_2_x : sub source reg combination p0, p0 is NOT allowed";
string VS_090 = 
	"vs_2_x "
	"sub r0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_091_Desc = "vs_2_x : sub source reg combination p0, oPos is NOT allowed";
string VS_091 = 
	"vs_2_x "
	"sub r0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_092_Desc = "vs_2_x : sub source reg combination p0, oFog is NOT allowed";
string VS_092 = 
	"vs_2_x "
	"sub r0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_093_Desc = "vs_2_x : sub source reg combination p0, oPts is NOT allowed";
string VS_093 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_094_Desc = "vs_2_x : sub source reg combination p0, oD0 is NOT allowed";
string VS_094 = 
	"vs_2_x "
	"sub r0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_095_Desc = "vs_2_x : sub source reg combination p0, oT0 is NOT allowed";
string VS_095 = 
	"vs_2_x "
	"sub r0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_x : sub source reg combination oPos, a0 is NOT allowed";
string VS_096 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_x : sub source reg combination oPos, b0 is NOT allowed";
string VS_097 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_x : sub source reg combination oPos, i0 is NOT allowed";
string VS_098 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_x : sub source reg combination oPos, aL is NOT allowed";
string VS_099 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_100_Desc = "vs_2_x : sub source reg combination oPos, p0 is NOT allowed";
string VS_100 = 
	"vs_2_x "
	"sub r0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_x : sub source reg combination oPos, oPos is NOT allowed";
string VS_101 = 
	"vs_2_x "
	"sub r0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_x : sub source reg combination oPos, oFog is NOT allowed";
string VS_102 = 
	"vs_2_x "
	"sub r0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_x : sub source reg combination oPos, oPts is NOT allowed";
string VS_103 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_x : sub source reg combination oPos, oD0 is NOT allowed";
string VS_104 = 
	"vs_2_x "
	"sub r0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_x : sub source reg combination oPos, oT0 is NOT allowed";
string VS_105 = 
	"vs_2_x "
	"sub r0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_x : sub source reg combination oFog, a0 is NOT allowed";
string VS_106 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_x : sub source reg combination oFog, b0 is NOT allowed";
string VS_107 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_x : sub source reg combination oFog, i0 is NOT allowed";
string VS_108 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_x : sub source reg combination oFog, aL is NOT allowed";
string VS_109 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_110_Desc = "vs_2_x : sub source reg combination oFog, p0 is NOT allowed";
string VS_110 = 
	"vs_2_x "
	"sub r0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_x : sub source reg combination oFog, oPos is NOT allowed";
string VS_111 = 
	"vs_2_x "
	"sub r0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_x : sub source reg combination oFog, oFog is NOT allowed";
string VS_112 = 
	"vs_2_x "
	"sub r0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_x : sub source reg combination oFog, oPts is NOT allowed";
string VS_113 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_x : sub source reg combination oFog, oD0 is NOT allowed";
string VS_114 = 
	"vs_2_x "
	"sub r0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_x : sub source reg combination oFog, oT0 is NOT allowed";
string VS_115 = 
	"vs_2_x "
	"sub r0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_x : sub source reg combination oPts, a0 is NOT allowed";
string VS_116 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"sub r0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_x : sub source reg combination oPts, b0 is NOT allowed";
string VS_117 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"sub r0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_x : sub source reg combination oPts, i0 is NOT allowed";
string VS_118 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"sub r0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_2_x : sub source reg combination oPts, aL is NOT allowed";
string VS_119 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"sub r0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_120_Desc = "vs_2_x : sub source reg combination oPts, p0 is NOT allowed";
string VS_120 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_x : sub source reg combination oPts, oPos is NOT allowed";
string VS_121 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_x : sub source reg combination oPts, oFog is NOT allowed";
string VS_122 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_x : sub source reg combination oPts, oPts is NOT allowed";
string VS_123 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_x : sub source reg combination oPts, oD0 is NOT allowed";
string VS_124 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_2_x : sub source reg combination oPts, oT0 is NOT allowed";
string VS_125 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_2_x : sub source reg combination oD0, a0 is NOT allowed";
string VS_126 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_2_x : sub source reg combination oD0, b0 is NOT allowed";
string VS_127 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_2_x : sub source reg combination oD0, i0 is NOT allowed";
string VS_128 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_2_x : sub source reg combination oD0, aL is NOT allowed";
string VS_129 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_130_Desc = "vs_2_x : sub source reg combination oD0, p0 is NOT allowed";
string VS_130 = 
	"vs_2_x "
	"sub r0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_2_x : sub source reg combination oD0, oPos is NOT allowed";
string VS_131 = 
	"vs_2_x "
	"sub r0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_2_x : sub source reg combination oD0, oFog is NOT allowed";
string VS_132 = 
	"vs_2_x "
	"sub r0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_2_x : sub source reg combination oD0, oPts is NOT allowed";
string VS_133 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_2_x : sub source reg combination oD0, oD0 is NOT allowed";
string VS_134 = 
	"vs_2_x "
	"sub r0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_x : sub source reg combination oD0, oT0 is NOT allowed";
string VS_135 = 
	"vs_2_x "
	"sub r0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_x : sub source reg combination oT0, a0 is NOT allowed";
string VS_136 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"sub r0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_x : sub source reg combination oT0, b0 is NOT allowed";
string VS_137 = 
	"vs_2_x "
	"defb b0, true "
	"sub r0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_x : sub source reg combination oT0, i0 is NOT allowed";
string VS_138 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"sub r0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_2_x : sub source reg combination oT0, aL is NOT allowed";
string VS_139 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"sub r0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_140_Desc = "vs_2_x : sub source reg combination oT0, p0 is NOT allowed";
string VS_140 = 
	"vs_2_x "
	"sub r0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_x : sub source reg combination oT0, oPos is NOT allowed";
string VS_141 = 
	"vs_2_x "
	"sub r0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_x : sub source reg combination oT0, oFog is NOT allowed";
string VS_142 = 
	"vs_2_x "
	"sub r0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_x : sub source reg combination oT0, oPts is NOT allowed";
string VS_143 = 
	"vs_2_x "
	"dcl ts "
	"sub r0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_x : sub source reg combination oT0, oD0 is NOT allowed";
string VS_144 = 
	"vs_2_x "
	"sub r0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_x : sub source reg combination oT0, oT0 is NOT allowed";
string VS_145 = 
	"vs_2_x "
	"sub r0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_x : sub dest r# is allowed";
string VS_146 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_x : sub dest oPos is allowed";
string VS_147 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub oPos, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_x : sub dest oD# is allowed";
string VS_148 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub oD0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_2_x : sub dest oT# is allowed";
string VS_149 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub oT0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_x : sub dest v# is NOT allowed";
string VS_150 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub v0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_x : sub dest c# is NOT allowed";
string VS_151 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub c1, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_x : sub dest a0 is NOT allowed";
string VS_152 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub a0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_x : sub dest b# is NOT allowed";
string VS_153 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub b0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_x : sub dest i# is NOT allowed";
string VS_154 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub i0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_x : sub dest aL is NOT allowed";
string VS_155 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub aL, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_156_Desc = "vs_2_x : sub dest p0 is NOT allowed";
string VS_156 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub p0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_x : sub write mask .x is allowed";
string VS_157 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.x, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_x : sub write mask .y is allowed";
string VS_158 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.y, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_2_x : sub write mask .z is allowed";
string VS_159 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.z, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_x : sub write mask .w is allowed";
string VS_160 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.w, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_x : sub write mask .xy is allowed";
string VS_161 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_x : sub write mask .yz is allowed";
string VS_162 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.yz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_x : sub write mask .zw is allowed";
string VS_163 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.zw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_x : sub write mask .xz is allowed";
string VS_164 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_x : sub write mask .xw is allowed";
string VS_165 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_x : sub write mask .yw is allowed";
string VS_166 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.yw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_x : sub write mask .xyz is allowed";
string VS_167 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xyz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_2_x : sub write mask .yzw is allowed";
string VS_168 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.yzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_2_x : sub write mask .xzw is allowed";
string VS_169 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_2_x : sub write mask .xyzw is allowed";
string VS_170 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.xyzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_2_x : sub write mask .yx is NOT allowed";
string VS_171 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.yx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_2_x : sub write mask .zx is NOT allowed";
string VS_172 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.zx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_2_x : sub write mask .zy is NOT allowed";
string VS_173 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.zy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_2_x : sub write mask .wx is NOT allowed";
string VS_174 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_2_x : sub write mask .wz is NOT allowed";
string VS_175 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_2_x : sub write mask .wy is NOT allowed";
string VS_176 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_2_x : sub write mask .zyx is NOT allowed";
string VS_177 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.zyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_2_x : sub write mask .wzy is NOT allowed";
string VS_178 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wzy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_2_x : sub write mask .wzx is NOT allowed";
string VS_179 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wzx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_2_x : sub write mask .wyx is NOT allowed";
string VS_180 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_2_x : sub write mask .yxw is NOT allowed";
string VS_181 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.yxw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_2_x : sub write mask .wzyx is NOT allowed";
string VS_182 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.wzyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_2_x : sub write mask .zxwy is NOT allowed";
string VS_183 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"sub r0.zxwy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_2_x : sub is allowed in a 1 level if b0 block";
string VS_184 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_185_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_2_x : sub is allowed in a 8 level if b0 block";
string VS_185 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_186_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_2_x : sub is allowed in a 16 level if b0 block";
string VS_186 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_187_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_2_x : sub is NOT allowed in a 24 level if b0 block";
string VS_187 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_188_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_2_x : sub is NOT allowed in a 25 level if b0 block";
string VS_188 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_189_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_2_x : sub is allowed in a 1 level if b0 nop else block";
string VS_189 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_190_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_2_x : sub is allowed in a 8 level if b0 nop else block";
string VS_190 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_191_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_2_x : sub is NOT allowed in a 16 level if b0 nop else block";
string VS_191 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_192_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_2_x : sub is NOT allowed in a 24 level if b0 nop else block";
string VS_192 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_193_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_2_x : sub is NOT allowed in a 25 level if b0 nop else block";
string VS_193 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_194_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_2_x : sub is allowed in a 1 level loop aL, i0 block";
string VS_194 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	sub r0, v0, r0 "
	"endloop ";

VS_CRITERIA VS_195_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_2_x : sub is allowed in a 2 level loop aL, i0 block";
string VS_195 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_196_Criteria = { true, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_2_x : sub is allowed in a 3 level loop aL, i0 block";
string VS_196 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			sub r0, v0, r0 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_197_Criteria = { true, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_2_x : sub is allowed in a 4 level loop aL, i0 block";
string VS_197 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				sub r0, v0, r0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_198_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_2_x : sub is NOT allowed in a 5 level loop aL, i0 block";
string VS_198 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					sub r0, v0, r0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_199_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_2_x : sub is allowed in a 1 level rep i0 block";
string VS_199 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	sub r0, v0, r0 "
	"endrep ";

VS_CRITERIA VS_200_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_2_x : sub is allowed in a 2 level rep i0 block";
string VS_200 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_201_Criteria = { true, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_2_x : sub is allowed in a 3 level rep i0 block";
string VS_201 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			sub r0, v0, r0 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_202_Criteria = { true, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_2_x : sub is allowed in a 4 level rep i0 block";
string VS_202 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				sub r0, v0, r0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_203_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_2_x : sub is NOT allowed in a 5 level rep i0 block";
string VS_203 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					sub r0, v0, r0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_204_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_2_x : sub is allowed in a 1 level if_gt c0.x, c0.y block";
string VS_204 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_205_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_2_x : sub is allowed in a 8 level if_gt c0.x, c0.y block";
string VS_205 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_206_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_2_x : sub is allowed in a 16 level if_gt c0.x, c0.y block";
string VS_206 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_207_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_2_x : sub is allowed in a 24 level if_gt c0.x, c0.y block";
string VS_207 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_208_Criteria = { false, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_2_x : sub is NOT allowed in a 25 level if_gt c0.x, c0.y block";
string VS_208 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								if_gt c0.x, c0.y "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_209_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_2_x : sub is allowed in a 1 level if_gt c0.x, c0.y nop else block";
string VS_209 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_210_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_2_x : sub is allowed in a 8 level if_gt c0.x, c0.y nop else block";
string VS_210 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_211_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_2_x : sub is allowed in a 16 level if_gt c0.x, c0.y nop else block";
string VS_211 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_212_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_2_x : sub is allowed in a 24 level if_gt c0.x, c0.y nop else block";
string VS_212 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_213_Criteria = { false, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_2_x : sub is NOT allowed in a 25 level if_gt c0.x, c0.y nop else block";
string VS_213 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								if_gt c0.x, c0.y nop else "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_214_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_214_Desc = "vs_2_x : sub is allowed in a 1 level if p0.x block";
string VS_214 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_215_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_215_Desc = "vs_2_x : sub is allowed in a 8 level if p0.x block";
string VS_215 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_216_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_216_Desc = "vs_2_x : sub is allowed in a 16 level if p0.x block";
string VS_216 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_217_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_217_Desc = "vs_2_x : sub is allowed in a 24 level if p0.x block";
string VS_217 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_218_Criteria = { false, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_218_Desc = "vs_2_x : sub is NOT allowed in a 25 level if p0.x block";
string VS_218 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_219_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_219_Desc = "vs_2_x : sub is allowed in a 1 level if p0.x nop else block";
string VS_219 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	sub r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_220_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_220_Desc = "vs_2_x : sub is allowed in a 8 level if p0.x nop else block";
string VS_220 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								sub r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_221_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_221_Desc = "vs_2_x : sub is allowed in a 16 level if p0.x nop else block";
string VS_221 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																sub r0, v0, r0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_222_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_222_Desc = "vs_2_x : sub is allowed in a 24 level if p0.x nop else block";
string VS_222 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								sub r0, v0, r0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_223_Criteria = { false, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_223_Desc = "vs_2_x : sub is NOT allowed in a 25 level if p0.x nop else block";
string VS_223 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									sub r0, v0, r0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_224_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_2_x : sub is allowed in a 1 level loop aL, i0 break block";
string VS_224 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	sub r0, v0, r0 "
	"endloop ";

VS_CRITERIA VS_225_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_2_x : sub is NOT allowed in a 8 level loop aL, i0 break block";
string VS_225 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								sub r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_226_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_2_x : sub is NOT allowed in a 16 level loop aL, i0 break block";
string VS_226 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																sub r0, v0, r0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_227_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_2_x : sub is NOT allowed in a 24 level loop aL, i0 break block";
string VS_227 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								sub r0, v0, r0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_228_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_2_x : sub is NOT allowed in a 25 level loop aL, i0 break block";
string VS_228 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									sub r0, v0, r0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_229_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_2_x : sub is allowed in a 1 level rep i0 break block";
string VS_229 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	sub r0, v0, r0 "
	"endrep ";

VS_CRITERIA VS_230_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_2_x : sub is NOT allowed in a 8 level rep i0 break block";
string VS_230 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								sub r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_231_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_2_x : sub is NOT allowed in a 16 level rep i0 break block";
string VS_231 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																sub r0, v0, r0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_232_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_2_x : sub is NOT allowed in a 24 level rep i0 break block";
string VS_232 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								sub r0, v0, r0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_233_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_2_x : sub is NOT allowed in a 25 level rep i0 break block";
string VS_233 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									sub r0, v0, r0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_234_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_234_Desc = "vs_2_x : sub is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_234 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	sub r0, v0, r0 "
	"endloop ";

VS_CRITERIA VS_235_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string VS_235_Desc = "vs_2_x : sub is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_235 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								sub r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_236_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string VS_236_Desc = "vs_2_x : sub is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_236 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																sub r0, v0, r0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_237_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string VS_237_Desc = "vs_2_x : sub is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_237 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								sub r0, v0, r0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_238_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string VS_238_Desc = "vs_2_x : sub is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_238 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									sub r0, v0, r0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_239_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_239_Desc = "vs_2_x : sub is allowed in a 1 level rep i0 breakp p0.x block";
string VS_239 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	sub r0, v0, r0 "
	"endrep ";

VS_CRITERIA VS_240_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string VS_240_Desc = "vs_2_x : sub is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_240 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								sub r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_241_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string VS_241_Desc = "vs_2_x : sub is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_241 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																sub r0, v0, r0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_242_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string VS_242_Desc = "vs_2_x : sub is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_242 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								sub r0, v0, r0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_243_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string VS_243_Desc = "vs_2_x : sub is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_243 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									sub r0, v0, r0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_244_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_2_x : sub is allowed in a 1 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_244 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	sub r0, v0, r0 "
	"endloop ";

VS_CRITERIA VS_245_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_2_x : sub is NOT allowed in a 8 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_245 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								sub r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_246_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_2_x : sub is NOT allowed in a 16 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_246 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																sub r0, v0, r0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_247_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_2_x : sub is NOT allowed in a 24 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_247 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								sub r0, v0, r0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_248_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_2_x : sub is NOT allowed in a 25 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_248 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								loop aL, i0 break_gt c0.x, c0.y "
	"																									sub r0, v0, r0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_249_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_2_x : sub is allowed in a 1 level rep i0 break_gt c0.x, c0.y block";
string VS_249 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	sub r0, v0, r0 "
	"endrep ";

VS_CRITERIA VS_250_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_2_x : sub is NOT allowed in a 8 level rep i0 break_gt c0.x, c0.y block";
string VS_250 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								sub r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_251_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_2_x : sub is NOT allowed in a 16 level rep i0 break_gt c0.x, c0.y block";
string VS_251 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																sub r0, v0, r0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_252_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_2_x : sub is NOT allowed in a 24 level rep i0 break_gt c0.x, c0.y block";
string VS_252 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								sub r0, v0, r0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_253_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_2_x : sub is NOT allowed in a 25 level rep i0 break_gt c0.x, c0.y block";
string VS_253 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								rep i0 break_gt c0.x, c0.y "
	"																									sub r0, v0, r0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_254_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_2_x : sub is allowed in a if b0 and if b0 block";
string VS_254 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_255_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_2_x : sub is allowed in a if b0 and if b0 nop else block";
string VS_255 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_256_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_2_x : sub is allowed in a if b0 and loop aL, i0 block";
string VS_256 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_257_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_2_x : sub is allowed in a if b0 and rep i0 block";
string VS_257 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_258_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_2_x : sub is allowed in a if b0 and if_ge c0.x, c0.y block";
string VS_258 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_259_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_2_x : sub is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string VS_259 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_260_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_260_Desc = "vs_2_x : sub is allowed in a if b0 and if p0.x block";
string VS_260 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_261_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_261_Desc = "vs_2_x : sub is allowed in a if b0 and if p0.x nop else block";
string VS_261 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_262_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_2_x : sub is allowed in a if b0 and loop aL, i0 break block";
string VS_262 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_263_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_2_x : sub is allowed in a if b0 and rep i0 break block";
string VS_263 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_264_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_264_Desc = "vs_2_x : sub is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_264 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_265_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_265_Desc = "vs_2_x : sub is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_265 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_266_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_2_x : sub is allowed in a if b0 and loop aL, i0 break_ge c0.x, c0.y block";
string VS_266 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_267_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_2_x : sub is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string VS_267 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_268_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if b0 block";
string VS_268 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_269_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if b0 nop else block";
string VS_269 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_270_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_2_x : sub is allowed in a if b0 nop else and loop aL, i0 block";
string VS_270 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_271_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_2_x : sub is allowed in a if b0 nop else and rep i0 block";
string VS_271 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_272_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string VS_272 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_273_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string VS_273 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_274_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_274_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if p0.x block";
string VS_274 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_275_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_275_Desc = "vs_2_x : sub is allowed in a if b0 nop else and if p0.x nop else block";
string VS_275 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_276_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_2_x : sub is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_276 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_277_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_2_x : sub is allowed in a if b0 nop else and rep i0 break block";
string VS_277 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_278_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_278_Desc = "vs_2_x : sub is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_278 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_279_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_279_Desc = "vs_2_x : sub is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_279 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_280_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_2_x : sub is allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_280 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_281_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_2_x : sub is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string VS_281 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_282_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if b0 block";
string VS_282 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_283_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if b0 nop else block";
string VS_283 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_284_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_284 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_285_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and rep i0 block";
string VS_285 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_286_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string VS_286 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_287_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string VS_287 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_288_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_288_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if p0.x block";
string VS_288 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_289_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_289_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_289 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_290_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_290 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_291_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and rep i0 break block";
string VS_291 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_292_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_292_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_292 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_293_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_293_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_293 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_294_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and loop aL, i0 break_lt c0.x, c0.y block";
string VS_294 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_295_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_2_x : sub is allowed in a loop aL, i0 and rep i0 break_lt c0.x, c0.y block";
string VS_295 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_296_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_2_x : sub is allowed in a rep i0 and if b0 block";
string VS_296 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_297_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_2_x : sub is allowed in a rep i0 and if b0 nop else block";
string VS_297 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_298_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_2_x : sub is allowed in a rep i0 and loop aL, i0 block";
string VS_298 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_299_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_2_x : sub is allowed in a rep i0 and rep i0 block";
string VS_299 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_300_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_2_x : sub is allowed in a rep i0 and if_lt c0.x, c0.y block";
string VS_300 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_301_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_2_x : sub is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string VS_301 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_302_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_302_Desc = "vs_2_x : sub is allowed in a rep i0 and if p0.x block";
string VS_302 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_303_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_303_Desc = "vs_2_x : sub is allowed in a rep i0 and if p0.x nop else block";
string VS_303 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_304_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_2_x : sub is allowed in a rep i0 and loop aL, i0 break block";
string VS_304 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_305_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_2_x : sub is allowed in a rep i0 and rep i0 break block";
string VS_305 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_306_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_306_Desc = "vs_2_x : sub is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_306 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_307_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_307_Desc = "vs_2_x : sub is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_307 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_308_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_2_x : sub is allowed in a rep i0 and loop aL, i0 break_lt c0.x, c0.y block";
string VS_308 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_309_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_2_x : sub is allowed in a rep i0 and rep i0 break_lt c0.x, c0.y block";
string VS_309 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_310_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if b0 block";
string VS_310 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_311_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if b0 nop else block";
string VS_311 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_312_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and loop aL, i0 block";
string VS_312 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_313_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and rep i0 block";
string VS_313 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_314_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_314 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_315_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_315 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_316_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_316_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if p0.x block";
string VS_316 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_317_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_317_Desc = "vs_2_x : sub is allowed in a if_lt c0.x, c0.y and if p0.x nop else block";
string VS_317 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_318_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and loop aL, i0 break block";
string VS_318 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_319_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and rep i0 break block";
string VS_319 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_320_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_320_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_320 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_321_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_321_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and rep i0 breakp p0.x block";
string VS_321 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_322_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_322 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_323_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_323 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_324_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if b0 block";
string VS_324 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_325_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if b0 nop else block";
string VS_325 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_326_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and loop aL, i0 block";
string VS_326 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_327_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and rep i0 block";
string VS_327 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_328_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if_le c0.x, c0.y block";
string VS_328 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_329_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if_le c0.x, c0.y nop else block";
string VS_329 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if_le c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_330_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_330_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if p0.x block";
string VS_330 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_331_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_331_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and if p0.x nop else block";
string VS_331 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_332_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and loop aL, i0 break block";
string VS_332 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_333_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and rep i0 break block";
string VS_333 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_334_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_334_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_334 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_335_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_335_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_335 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_336_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_336 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_337_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_2_x : sub is allowed in a if_le c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string VS_337 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_338_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_338_Desc = "vs_2_x : sub is allowed in a if p0.x and if b0 block";
string VS_338 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_339_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_339_Desc = "vs_2_x : sub is allowed in a if p0.x and if b0 nop else block";
string VS_339 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_340_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_340_Desc = "vs_2_x : sub is allowed in a if p0.x and loop aL, i0 block";
string VS_340 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_341_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_341_Desc = "vs_2_x : sub is allowed in a if p0.x and rep i0 block";
string VS_341 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_342_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_342_Desc = "vs_2_x : sub is allowed in a if p0.x and if_le c0.x, c0.y block";
string VS_342 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_343_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_343_Desc = "vs_2_x : sub is allowed in a if p0.x and if_le c0.x, c0.y nop else block";
string VS_343 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if_le c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_344_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_344_Desc = "vs_2_x : sub is allowed in a if p0.x and if p0.x block";
string VS_344 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_345_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_345_Desc = "vs_2_x : sub is allowed in a if p0.x and if p0.x nop else block";
string VS_345 = 
	"vs_2_x "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_346_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_346_Desc = "vs_2_x : sub is allowed in a if p0.x and loop aL, i0 break block";
string VS_346 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_347_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_347_Desc = "vs_2_x : sub is allowed in a if p0.x and rep i0 break block";
string VS_347 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_348_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_348_Desc = "vs_2_x : sub is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_348 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_349_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_349_Desc = "vs_2_x : sub is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_349 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_350_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_350_Desc = "vs_2_x : sub is allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_350 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_351_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_351_Desc = "vs_2_x : sub is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string VS_351 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_352_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_352_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if b0 block";
string VS_352 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_353_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_353_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if b0 nop else block";
string VS_353 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_354_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_354_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_354 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_355_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_355_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and rep i0 block";
string VS_355 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_356_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_356_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if_eq c0.x, c0.y block";
string VS_356 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_357_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_357_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if_eq c0.x, c0.y nop else block";
string VS_357 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_358_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_358_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if p0.x block";
string VS_358 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_359_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_359_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_359 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_360_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_360_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_360 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_361_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_361_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and rep i0 break block";
string VS_361 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_362_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_362_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_362 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_363_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_363_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_363 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_364_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_364_Desc = "vs_2_x : sub is allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_364 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_365_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_365_Desc = "vs_2_x :