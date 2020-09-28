/*++

Copyright (c) Microsoft Corporation

Module Name:

    util.c

Abstract:

    EFI specific routines for the boot manager.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootmgr.h"

//
// -------------------------------------------------------------------- Globals
//

PWCHAR BootDirectory;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
BmpFwGetApplicationDirectoryPath (
    __out PUNICODE_STRING ApplicationDirectoryPath
    );

NTSTATUS
BmpFwGetFullPath (
    __in_z PWSTR FilePath,
    __deref_out_z PWSTR *FullPath
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BmFwBootToFirmware (
    VOID
    )

/*++

Routine Description:

    This routine reboots to the firmware menus.

Arguments:

    None.

Return Value:

    TRUE if the system can reboot to firmware menus. FALSE otherwise.

--*/

{

    ULONG Attributes;
    ULONGLONG OsIndications;
    UINTN OsIndicationsSize;
    NTSTATUS Status;

    Attributes = 0;
    OsIndications = 0;
    OsIndicationsSize = sizeof(ULONGLONG);

    //
    // Try to get the OsIndications variable in order to preserve any current
    // settings. If it cannot be accessed, return an error.
    //

    Status = EfiGetVariable(EFI_OS_INDICATIONS,
                            &EfiGlobalVariable,
                            &Attributes,
                            &OsIndicationsSize,
                            &OsIndications);

    if ((Status == STATUS_ACCESS_DENIED) ||
        (Status == STATUS_IO_DEVICE_ERROR)) {

        goto BootToFirmwareEnd;
    }

    //
    // If the variable does not exist, initialize values and set the
    // variable. If the attempt to get the variable failed for another reason or
    // it was the wrong size, delete the variable and then set it.
    //

    if (Status == STATUS_NOT_FOUND) {
        OsIndications = 0;
        Attributes = EFI_VARIABLE_NON_VOLATILE;
        Attributes |= EFI_VARIABLE_BOOTSERVICE_ACCESS;
        Attributes |= EFI_VARIABLE_RUNTIME_ACCESS;

    } else if (!NT_SUCCESS(Status) ||
               (OsIndicationsSize != sizeof(ULONGLONG))) {

        Status = EfiSetVariable(EFI_OS_INDICATIONS,
                                &EfiGlobalVariable,
                                0,
                                0,
                                NULL);

        ASSERT(NT_SUCCESS(Status));

        OsIndications = 0;
        Attributes = EFI_VARIABLE_NON_VOLATILE;
        Attributes |= EFI_VARIABLE_BOOTSERVICE_ACCESS;
        Attributes |= EFI_VARIABLE_RUNTIME_ACCESS;
    }

    OsIndications |= EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
    Status = EfiSetVariable(EFI_OS_INDICATIONS,
                            &EfiGlobalVariable,
                            Attributes,
                            OsIndicationsSize,
                            &OsIndications);

    //
    // If the set succeeded, reboot to the firmware menus.
    //

    if (NT_SUCCESS(Status)) {
        BlFwReboot();
    }

BootToFirmwareEnd:
    return Status;
}

BOOLEAN
BmFwBootToFirmwareSupported (
    VOID
    )

/*++

Routine Description:

    This routine determines if the system allows booting to firmware menus.

Arguments:

    None.

Return Value:

    TRUE if the system can reboot to firmware menus. FALSE otherwise.

--*/

{

    ULONG Attributes;
    BOOLEAN BootToFirmwareSupported;
    ULONGLONG OsIndicationsSupported;
    UINTN OsIndicationsSupportedSize;
    NTSTATUS Status;

    //
    // The boot to firmware option is only supported if the
    // OsIndicationsSupported variable exists, is the right size, and the boot
    // to firmware UI bit is set.
    //

    Attributes = 0;
    BootToFirmwareSupported = FALSE;
    OsIndicationsSupported = 0;
    OsIndicationsSupportedSize = sizeof(ULONGLONG);
    Status = EfiGetVariable(EFI_OS_INDICATIONS_SUPPORTED,
                            &EfiGlobalVariable,
                            &Attributes,
                            &OsIndicationsSupportedSize,
                            &OsIndicationsSupported);

    if (!NT_SUCCESS(Status)) {
        goto BootToFirmwareSupportedEnd;
    }

    if (OsIndicationsSupportedSize != sizeof(ULONGLONG)) {
        goto BootToFirmwareSupportedEnd;
    }

    if ((OsIndicationsSupported & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) == 0) {
        goto BootToFirmwareSupportedEnd;
    }

    BootToFirmwareSupported = TRUE;

BootToFirmwareSupportedEnd:
    return BootToFirmwareSupported;
}

NTSTATUS
BmFwGetBcdFilePath (
    __deref_out_z PWCHAR *FilePath,
    __out PBOOLEAN FilePathAllocated
    )

/*++

Routine Description:

    This routine returns the boot configuration data store file path. On UEFI
    systems, the boot manager first looks for a BCD store in its directory. If
    no BCD store exists there, then it uses a hard-coded path.

Arguments:

    FilePath - Supplies a pointer to a variable that receives the BCD file
        path.

    FilePathAllocated - Supplies a pointer to a variable that receives
       indication of whether or not the BCD's file path was allocated by this
       routine.

Return Value:

    File path to BCD store.

--*/

{

    PWCHAR LocalFilePath;
    NTSTATUS Status;

    Status = BmpFwGetFullPath(BM_DATA_STORE_FILE_NAME, &LocalFilePath);
    if (!NT_SUCCESS(Status)) {
        goto GetBcdFilePathEnd;
    }

    *FilePath = LocalFilePath;
    *FilePathAllocated = TRUE;

GetBcdFilePathEnd:
    return Status;
}

PWCHAR
BmFwGetBgfxRootDirectory (
    __out PBOOLEAN RootDirectoryAllocated
    )

/*++

Routine Description:

    This routine returns the path necessary to initialize BGFX. On UEFI systems
    this is the boot directory - that is, the directory that contains the BCD
    store.

Arguments:

    RootDirectoryAllocated - Supplies a pointer that recevies indication of
        whether or not the returned root directory path was allocated.

Return Value:

    BGFX initialization directory.

--*/

{

    PWCHAR BgfxDirectory;
    ULONG BgfxDirectoryLength;
    ULONG BgfxDirectorySize;
    ULONG BootDirectoryLength;
    LONG Index;
    NTSTATUS Status;

    BgfxDirectory = NULL;
    BootDirectoryLength = (ULONG)wcslen(BootDirectory);
    for (Index = (BootDirectoryLength - 1); Index > 0; Index -= 1) {
        if (BootDirectory[Index] == L'\\') {
            break;
        }
    }

    Status = RtlULongAdd(Index, 1, &BgfxDirectoryLength);
    if (!NT_SUCCESS(Status)) {
        goto GetBgfxRootDirectoryEnd;
    }

    Status = RtlULongMult(BgfxDirectoryLength,
                          (ULONG)sizeof(WCHAR),
                          &BgfxDirectorySize);

    if (!NT_SUCCESS(Status)) {
        goto GetBgfxRootDirectoryEnd;
    }

    BgfxDirectory = BlMmAllocateHeap(BgfxDirectorySize);
    if (BgfxDirectory == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBgfxRootDirectoryEnd;
    }

    RtlCopyMemory(BgfxDirectory, BootDirectory, BgfxDirectorySize);
    BgfxDirectory[BgfxDirectoryLength - 1] = L'\0';
    *RootDirectoryAllocated = TRUE;

GetBgfxRootDirectoryEnd:
    if (!NT_SUCCESS(Status)) {
        if (BgfxDirectory != NULL) {
            BlMmFreeHeap(BgfxDirectory);
        }

        *RootDirectoryAllocated = FALSE;
        BgfxDirectory = BM_BGFX_DIRECTORY;
    }

    return BgfxDirectory;
}

PWCHAR
BmFwGetBsdLogPath (
    __out PBOOLEAN FilePathAllocated
    )

/*++

Routine Description:

    This routine returns the boot status data log path. This is the bootstat.dat
    file in the boot directory.

Arguments:

    FilePathAllocated - Supplies a pointer to a variable that receives
       indication of whether or not the BSD's file path was allocated by this
       routine.

Return Value:

    File path to BSD log.

--*/

{

    PWCHAR BsdLogPath;
    NTSTATUS Status;

    Status = BmpFwGetFullPath(BM_BSD_LOG_NAME, &BsdLogPath);
    if (!NT_SUCCESS(Status)) {
        BsdLogPath = NULL;
        goto GetBsdLogPathEnd;
    }

    *FilePathAllocated = TRUE;

GetBsdLogPathEnd:
    return BsdLogPath;
}

PWCHAR
BmFwGetRevocationListPath (
    VOID
    )

/*++

Routine Description:

    This routine returns the secure boot revocation list. This is the boot.stl
    file in the boot directory.

Arguments:

    None.

Return Value:

    File path to revocation list.

--*/

{

    PWCHAR RevocationListPath;
    NTSTATUS Status;

    Status = BmpFwGetFullPath(BM_REVOCATION_LIST_FILE, &RevocationListPath);
    if (!NT_SUCCESS(Status)) {
        RevocationListPath = NULL;
    }

    return RevocationListPath;
}

VOID
BmFwInitializeBootDirectoryPath (
    VOID
    )

/*++

Routine Description:

    This routine initializes the boot manager's directory path. The directory
    path is the parent directory of the BCD store. First, the routine looks for
    a BCD store in the parent directory of the boot manager application. If a
    BCD file exists there, then that parent directory is set to be the boot
    directory. If no BCD store exists there, then the hard-coded path of
    \EFI\Microsoft\Boot is used.

Arguments:

    None.

Return Value:

    None.

--*/

{

    UNICODE_STRING BcdDirectoryPath;
    PWCHAR BcdFilePath;
    ULONG BcdFilePathSize;
    ULONG BcdNameSize;
    DEVICE_ID DeviceId;
    FILE_ID FileId;
    NTSTATUS Status;

    DeviceId = INVALID_DEVICE_ID;
    FileId = INVALID_FILE_ID;
    BcdFilePath = NULL;
    RtlInitUnicodeString(&BcdDirectoryPath, NULL);

    //
    // Assume the BCD store is on the boot device. Nothing uses the BCD store
    // overrides today and no third party user can set them.
    //

    Status = BlDeviceOpen(BlGetBootDevice(), OPEN_READ, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto InitializeBootDirectoryPathEnd;
    }

    //
    // Get the BCD's parent directory. It is the same as the application's
    // parent directory.
    //

    Status = BmpFwGetApplicationDirectoryPath(&BcdDirectoryPath);
    if (!NT_SUCCESS(Status)) {
        goto InitializeBootDirectoryPathEnd;
    }

    //
    // Calculate the size of the BCD file path.
    //

    Status = RtlULongMult((ULONG)wcslen(BM_DATA_STORE_FILE_NAME),
                          (ULONG)sizeof(WCHAR),
                          &BcdNameSize);

    if (!NT_SUCCESS(Status)) {
        goto InitializeBootDirectoryPathEnd;
    }

    Status = RtlULongAdd(BcdDirectoryPath.MaximumLength,
                         BcdNameSize,
                         &BcdFilePathSize);

    if (!NT_SUCCESS(Status)) {
        goto InitializeBootDirectoryPathEnd;
    }

    //
    // Allocate memory for the BCD file path and then copy the contents to that
    // buffer.
    //

    BcdFilePath = BlMmAllocateHeap(BcdFilePathSize);
    if (BcdFilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto InitializeBootDirectoryPathEnd;
    }

    RtlZeroMemory(BcdFilePath, BcdFilePathSize);
    RtlCopyMemory(BcdFilePath,
                  BcdDirectoryPath.Buffer,
                  BcdDirectoryPath.MaximumLength);

    wcscat_s(BcdFilePath,
             BcdFilePathSize / sizeof(WCHAR),
             BM_DATA_STORE_FILE_NAME);

    //
    // Try to open the BCD file. If it does not exist, revert to the fixed file
    // path.
    //

    Status = BlFileOpen(DeviceId, BcdFilePath, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto InitializeBootDirectoryPathEnd;
    }

    BootDirectory = BcdDirectoryPath.Buffer;

InitializeBootDirectoryPathEnd:
    if (!NT_SUCCESS(Status)) {
        if (BcdDirectoryPath.Buffer != NULL) {
            BlMmFreeHeap(BcdDirectoryPath.Buffer);
        }

        BootDirectory = BM_DEFAULT_BOOT_DIRECTORY;
    }

    if (BcdFilePath != NULL) {
        BlMmFreeHeap(BcdFilePath);
    }

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    return;
}

VOID
BmFwMemoryInitialize (
    VOID
    )

/*++

Routine Description:

    This routine performs EFI specific memory initialization operations.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(_X86_) || defined(_AMD64_)

    MEMORY_ADDRESS_RANGE AddressRange;
    ULONG Attributes;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG RangeFlags;
    NTSTATUS Status;

    //
    // The HAL requires 1 page below 1MB to setup the APs. This is allocated
    // from kernel phase 0 reclaimable memory - regions the OS loader marks as
    // free or firmware temporary. On machines where there are no EFI boot
    // services code or data pages (i.e. firmware temporary pages) in the first
    // 1MB, the Vista and Windows 7 HAL was relying on the fact that the
    // Vista/Windows 7 boot manager had happened to allocate its heap in the
    // first 1MB of memory. The boot manager's heap is marked as firmware
    // temporary memory by the OS loader and reclaimed by the HAL. This class of
    // EFI machines were successfully booting by a stroke of good luck.
    //
    // Post Vista/Windows 7, boot library allocation policy changed for EFI
    // systems, disallowing normal allocations to use memory below 1MB. The
    // purpose of this change was to make it easier for the hypervisor loader
    // to find a page below 1MB to reserve for the hypervisor, which requires
    // said page to bring APs online.
    //
    // As this change caused the boot manager to make no reclaimable heap
    // allocations in the first 1MB, it broke the previously discussed class of
    // EFI machines when attempting to boot Vista and Windows 7. The Vista and
    // Windows 7 OS loaders, without the new allocation policy, fill the first
    // 1MB with un-reclaimable allocations, leaving no memory for the HAL to
    // usein kernel phase 0. This causes the Vista and Windows 7 HALs to
    // bugcheck.
    //
    // The fix is to allocate a single page below 1MB that a Vista or Windows 7
    // OS loader will not reclaim, but that a Vista or Windows 7 HAL can
    // reclaim. The memory type for this allocation should have a subtype of
    // MEMORY_APPLICATION_TYPE_UNAVAILABLE.
    //
    // This is a hack. It can only be removed once Vista and Windows 7 have been
    // deprecated.
    //

    Attributes = MEMORY_ATTRIBUTE_ALLOCATION_REALMODE_ACCESSIBLE;
    INITIALIZE_MEMORY_ADDRESS_RANGE(&AddressRange, 0, _1MB);
    RangeFlags = MEMORY_RANGE_PREFER_LOW_ADDRESSES;
    Status = BlMmAllocatePhysicalPagesInRange(&PhysicalAddress,
                                              1,
                                              MEMORY_TYPE_APPLICATION_UNAVAILABLE,
                                              Attributes,
                                              0,
                                              &AddressRange,
                                              RangeFlags);

    //
    // Allocation failure is not necessarily fatal, but provide notification if
    // it does fail.
    //

    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"BmFwMemoryInitialize: Failed to allocate a page below "
                      L"1MB. Status: 0x%08x\r\n",
                      Status);
    }

#endif

    return;
}

//
// ----------------------------------------------------------- Private Routines
//

NTSTATUS
BmpFwGetApplicationDirectoryPath (
    __out PUNICODE_STRING ApplicationDirectoryPath
    )

/*++

Routine Description:

    This routine returns a path to the parent directory of the currently running
    application.

Arguments:

    ApplicationDirectoryPath - Supplies a pointer to a variable that receives
        the path to the application's parent directory.

Return Value:

    NT status code.

--*/

{

    PWCHAR ApplicationPath = NULL;
    ULONG ApplicationPathLength;
    PWCHAR DirectoryPath;
    ULONG DirectoryPathLength;
    ULONG DirectoryPathSize;
    LONG Index;
    NTSTATUS Status;

    RtlInitUnicodeString(ApplicationDirectoryPath, NULL);

    //
    // Get the application's file path from the BCD settings, either from the
    // BCD store or from the UEFI initialization stub.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          &ApplicationPath);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationDirectoryPathEnd;
    }

    //
    // Search backwards through the path to find the file's parent directory.
    //

    ApplicationPathLength = (ULONG)wcslen(ApplicationPath);
    for (Index = (ApplicationPathLength - 1); Index > 0; Index -= 1) {
        if (ApplicationPath[Index] == L'\\') {
            break;
        }
    }

    //
    // Calculate the size of the parent directory path, accounting for the index
    // being off by 1 and a NULL terminator. The final L'\\' is not necessary,
    // so just add one for the NULL terminator.
    //

    Status = RtlULongAdd(Index, 1, &DirectoryPathLength);
    if (!NT_SUCCESS(Status)) {
        goto GetApplicationDirectoryPathEnd;
    }

    Status = RtlULongMult(DirectoryPathLength,
                          (ULONG)sizeof(WCHAR),
                          &DirectoryPathSize);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationDirectoryPathEnd;
    }

    //
    // Allocate memory for the new path, copy the contents and terminate the
    // string with a NULL character.
    //

    DirectoryPath = BlMmAllocateHeap(DirectoryPathSize);
    if (DirectoryPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetApplicationDirectoryPathEnd;
    }

    RtlCopyMemory(DirectoryPath, ApplicationPath, DirectoryPathSize);
    DirectoryPath[DirectoryPathLength - 1] = L'\0';
    RtlInitUnicodeString(ApplicationDirectoryPath, DirectoryPath);

GetApplicationDirectoryPathEnd:
    if (ApplicationPath != NULL) {
        BlMmFreeHeap(ApplicationPath);
    }

    return Status;
}

NTSTATUS
BmpFwGetFullPath (
    __in_z PWSTR FilePath,
    __deref_out_z PWSTR *FullPath
    )

/*++

Routine Description:

    Returns the full path to a file or directory.

Arguments:

    FilePath - Supplies a path starting from system root.

    FullPath - Supplies a pointer to a variable that receives the full path.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    Another error status code if returned from a sub-routine.

--*/

{

    ULONG Length;
    ULONG RequiredBufferSize;
    NTSTATUS Status;

    Status = RtlULongAdd((ULONG)wcslen(BootDirectory), 1, &Length);
    if (!NT_SUCCESS(Status)) {
        goto GetFullPathEnd;
    }

    Status = RtlULongAdd((ULONG)wcslen(FilePath), Length, &Length);
    if (!NT_SUCCESS(Status)) {
        goto GetFullPathEnd;
    }

    Status = RtlULongMult(Length, (ULONG)sizeof(WCHAR), &RequiredBufferSize);
    if (!NT_SUCCESS(Status)) {
        goto GetFullPathEnd;
    }

    *FullPath = BlMmAllocateHeap(RequiredBufferSize);
    if (*FullPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetFullPathEnd;
    }

    wcscpy_s(*FullPath, RequiredBufferSize / sizeof(WCHAR), BootDirectory);
    wcscat_s(*FullPath, RequiredBufferSize / sizeof(WCHAR), FilePath);

GetFullPathEnd:
    return Status;
}

