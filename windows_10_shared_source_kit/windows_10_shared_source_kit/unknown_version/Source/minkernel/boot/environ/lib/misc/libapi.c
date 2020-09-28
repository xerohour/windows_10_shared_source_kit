/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    libapi.c

Abstract:

    This module implements misc. API for the boot environment library.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>
#include <image.h>

// -------------------------------------------------------------------- Globals

static BOOLEAN BlpLibraryParametersInitialized;

//
// Input parameters for the application.
//

PBOOT_APPLICATION_PARAMETERS BlpApplicationParameters;
BOOT_APPLICATION_ENTRY BlpApplicationEntry;
PBOOT_ENVIRONMENT_DEVICE BlpBootDevice;
PWSTR BlpApplicationBaseDirectory;
LIBRARY_PARAMETERS BlpLibraryParameters;

//
// Global scratch buffer to be used by any library component.
//

UCHAR BlScratchBuffer[GLOBAL_SCRATCH_BUFFER_SIZE];

//
// Bitmasks containing platform and application specific flags.
//

ULONG BlPlatformFlags = FW_FLAGS | ARCH_FLAGS;
ULONG BlpApplicationFlags;

//
// Boot option callbacks and callback cookie.
//

static PCBOOT_OPTION_CALLBACKS BlpBootOptionCallbacks;
static PVOID BlpBootOptionCallbackCookie;

//--------------------------------------------------------------------- Defines

#define LOGICAL_XOR(_a,_b) (((_a) && !(_b)) || (!(_a) && (_b)))

//
// Set the SymCrypt environment for the boot library
//

BOOTLIB_SYMCRYPT_ENVIRONMENT

//------------------------------------------------------------------ Prototypes

NTSTATUS
InitializeLibrary (
    __in PBOOT_APPLICATION_PARAMETERS ApplicationParameters,
    __in PLIBRARY_PARAMETERS LibraryParameters
    );

NTSTATUS
ReinitializeLibrary (
    __in PLIBRARY_PARAMETERS LibraryParameters
    );

// ------------------------------------------------------------------ Functions

PBOOT_ENVIRONMENT_DEVICE
BlGetBootDevice (
    VOID
    )

/*++

Routine Description:

    Returns a pointer to a device identifier for the boot device (the
    device from which the boot manager was loaded).

Arguments:

    None.

Return Value:

    Device identifier for the boot device.

--*/

{

    BlpInterfaceEnter(InterfaceMisc);
    BlpInterfaceExit(InterfaceMisc);
    return BlpBootDevice;
}

ULONG
BlGetApplicationAttributes (
    VOID
    )

/*++

Routine Description:

    This routine returns the attributes associated with the application.

Arguments:

    None.

Return Value:

    The attributes associated with the application.

--*/

{

    BlpInterfaceEnter(InterfaceMisc);
    BlpInterfaceExit(InterfaceMisc);
    return BlpApplicationEntry.Attributes;
}

GUID *
BlGetApplicationIdentifier (
    VOID
    )

/*++

Routine Description:

    This routine returns the identifier associated with the application.

Arguments:

    None.

Return Value:

    The identifier associated with the application.

--*/

{

    GUID *Identifier;

    BlpInterfaceEnter(InterfaceMisc);
    if (CHECK_FLAG(BlpApplicationEntry.Attributes,
                   BOOT_ENTRY_IDENTIFIER_UNSPECIFIED)) {

        Identifier = NULL;

    } else {
        Identifier = &BlpApplicationEntry.Identifier;
    }

    BlpInterfaceExit(InterfaceMisc);
    return Identifier;
}

NTSTATUS
BlGetApplicationImageName (
    __deref_opt_out PWSTR *ApplicationBaseDirectory,
    __deref_opt_out PWSTR *ApplicationImageName
    )

/*++

Routine Description:

    Returns the base directory and image name for the currently running
        application.

Arguments:

    ApplicationBaseDirectory - On output, optionally contains a pointer to the
        directory containing the boot application.

    ApplicationImageName - On output, optionally contains a pointer to the image
        name of the boot application.

Return Value:

    NT_STATUS code.

--*/

{

    USHORT AllocationSize;
    PWSTR ApplicationPath;
    PWSTR BaseDirectory;
    ULONG BaseDirectorySize;
    PVOID ImageBase;
    PWSTR ImageName;
    ULONG ImageNameSize;
    ULONG ImageSize;
    STRING ModuleName;
    PWSTR p;
    NTSTATUS Status;

    ApplicationPath = NULL;
    BaseDirectory = NULL;
    ImageName = NULL;
    RtlZeroMemory(&ModuleName, sizeof(ModuleName));

    //
    // If the application path is specified, it contains sufficient information
    // to derive the base directory and the image name.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          &ApplicationPath);

    //
    // The base directory may be formed by searching backwards for a '\'.  The
    // image name follows the '\'.
    //

    if (NT_SUCCESS(Status)) {
        p = wcsrchr(ApplicationPath, L'\\');
        if (p == NULL) {
            Status = STATUS_INVALID_PARAMETER;
            goto GetApplicationImageNameExit;
        }

        *p = L'\0';
        Status = RtlULongMult((ULONG)wcslen(ApplicationPath),
                              sizeof(WCHAR),
                              &BaseDirectorySize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        Status = RtlULongAdd(BaseDirectorySize,
                             sizeof(UNICODE_NULL),
                             &BaseDirectorySize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        BaseDirectory = BlMmAllocateHeap(BaseDirectorySize);
        if (BaseDirectory == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        wcscpy_s(BaseDirectory,
                 (BaseDirectorySize / sizeof(WCHAR)),
                 ApplicationPath);

        *p = L'\\';
        p += 1;

        Status = RtlULongMult((ULONG)wcslen(p),
                              sizeof(WCHAR),
                              &ImageNameSize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        Status = RtlULongAdd(ImageNameSize,
                             sizeof(UNICODE_NULL),
                             &ImageNameSize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        ImageName = BlMmAllocateHeap(ImageNameSize);
        if (ImageName == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        wcscpy_s(ImageName,
                 (ImageNameSize / sizeof(WCHAR)),
                 p);

    } else {

        //
        // Application path is not specified for this boot application.  The
        // base directory may be retreived from the BlpApplicationBaseDirectory
        // global parameter and the image name can be extracted directly out of
        // the PE image.
        //

        if (BlpApplicationBaseDirectory == NULL) {
            Status = STATUS_INVALID_PARAMETER;
            goto GetApplicationImageNameExit;
        }

        Status = RtlULongMult((ULONG)wcslen(BlpApplicationBaseDirectory),
                              sizeof(WCHAR),
                              &BaseDirectorySize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        Status = RtlULongAdd(BaseDirectorySize,
                             sizeof(UNICODE_NULL),
                             &BaseDirectorySize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        BaseDirectory = BlMmAllocateHeap(BaseDirectorySize);
        if (BaseDirectory == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        wcscpy_s(BaseDirectory,
                 (BaseDirectorySize / sizeof(WCHAR)),
                 BlpApplicationBaseDirectory);

        Status = BlGetApplicationBaseAndSize(&ImageBase, &ImageSize);
        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        Status = BlImgGetModuleName(ImageBase, ImageSize, &ModuleName);
        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        Status = RtlUShortMult(ModuleName.Length,
                               sizeof(WCHAR),
                               &AllocationSize);

        if (!NT_SUCCESS(Status)) {
            goto GetApplicationImageNameExit;
        }

        ImageName = BlMmAllocateHeap(AllocationSize);
        if (ImageName == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        BlCopyStringToWcharString(ImageName, ModuleName.Buffer);
    }

    Status = RtlULongMult((ULONG)wcslen(BaseDirectory),
                          sizeof(WCHAR),
                          &BaseDirectorySize);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationImageNameExit;
    }

    Status = RtlULongAdd(BaseDirectorySize,
                         sizeof(UNICODE_NULL),
                         &BaseDirectorySize);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationImageNameExit;
    }

    Status = RtlULongMult((ULONG)wcslen(ImageName),
                          sizeof(WCHAR),
                          &ImageNameSize);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationImageNameExit;
    }

    Status = RtlULongAdd(ImageNameSize,
                         sizeof(UNICODE_NULL),
                         &ImageNameSize);

    if (!NT_SUCCESS(Status)) {
        goto GetApplicationImageNameExit;
    }

    if (ARGUMENT_PRESENT(ApplicationBaseDirectory)) {
        *ApplicationBaseDirectory = BlMmAllocateHeap(BaseDirectorySize);
        if (*ApplicationBaseDirectory == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        wcscpy_s(*ApplicationBaseDirectory,
                 (BaseDirectorySize / sizeof(WCHAR)),
                 BaseDirectory);
    }

    if (ARGUMENT_PRESENT(ApplicationImageName)) {
        *ApplicationImageName = BlMmAllocateHeap(ImageNameSize);
        if (*ApplicationImageName == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetApplicationImageNameExit;
        }

        wcscpy_s(*ApplicationImageName,
                 (ImageNameSize / sizeof(WCHAR)),
                 ImageName);

    }

    Status = STATUS_SUCCESS;

GetApplicationImageNameExit:
    if (ApplicationPath != NULL) {
        BlMmFreeHeap(ApplicationPath);
    }

    if (BaseDirectory != NULL) {
        BlMmFreeHeap(BaseDirectory);
    }

    if (ImageName != NULL) {
        BlMmFreeHeap(ImageName);
    }

    if (ModuleName.Buffer != NULL) {
        BlMmFreeHeap(ModuleName.Buffer);
    }

    return Status;
}

PBOOT_APPLICATION_ENTRY
BlGetApplicationEntry (
    VOID
    )

/*++

Routine Description:

    This routine returns the current application entry.  This pointer is only
    valid until the next call to modify the application entry.

Arguments:

    None.

Return Value:

    The application entry.

--*/
{

    BlpInterfaceEnter(InterfaceMisc);
    BlpInterfaceExit(InterfaceMisc);
    return &BlpApplicationEntry;
}

ULONG
BlGetApplicationFlags (
    VOID
    )

/*++

Routine Description:

    This routine returns the current application flags.

Arguments:

    None.

Return Value:

    The application flags.

--*/
{

    BlpInterfaceEnter(InterfaceMisc);
    BlpInterfaceExit(InterfaceMisc);
    return BlpApplicationFlags;
}

PBOOT_APPLICATION_PARAMETERS
BlGetApplicationParameters (
    VOID
    )

/*++

Routine Description:

    This routine returns the current application parameters.

Arguments:

    None.

Return Value:

    The application parameters.

--*/
{

    BlpInterfaceEnter(InterfaceMisc);
    BlpInterfaceExit(InterfaceMisc);
    return BlpApplicationParameters;
}

PBOOT_APPLICATION_RETURN_ARGUMENTS
BlGetApplicationReturnArguments (
    VOID
    )

/*++

Routine Description:

    This routine returns a pointer to the boot application's return
    arguments structure.

Arguments:

    None

Return Value:

    A pointer to the application's return arguments.

--*/

{

    return Add2Ptr(BlpApplicationParameters,
                   BlpApplicationParameters->ReturnArgumentOffset);
}

NTSTATUS
BlGetApplicationBaseAndSize (
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize
    )

/*++

Routine Description:

    Returns the base address of the executing boot application.

Arguments:

    ImageBase - On output, contains a pointer to the base address of the
        executing boot application.

    ImageSize - On output, contains the size (in bytes) of the executing boot
        application.

Return Value:

    STATUS_INVALID_PARAMETER if either ImageBase or ImageSize are NULL.
    STATUS_SUCCESS otherwise.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);
    if ((ImageBase == NULL) || (ImageSize == NULL)) {
        Status = STATUS_INVALID_PARAMETER;

    } else {

        *ImageBase = (PVOID)(UINTN)BlpApplicationParameters->ImageBase;
        *ImageSize = BlpApplicationParameters->ImageSize;
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

PBOOT_ENVIRONMENT_DEVICE
BlGetDevice (
    __in PUNALIGNED_FILE_PATH FilePath
    )

/*++

Routine Description:

    Returns a pointer to a device identifier for the specified file path.

Arguments:

    FilePath - pointer to the file path structure

Return Value:

    A pointer to the device identifier for the specified file path.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE Device;

    BlpInterfaceEnter(InterfaceMisc);
    if ((FilePath != NULL) &&
        (FilePath->Length > UFIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath))) {

        Device = (PBOOT_ENVIRONMENT_DEVICE)&FilePath->FilePath[0];

    } else {
        Device = NULL;
    }

    BlpInterfaceExit(InterfaceMisc);
    return Device;
}

PWCHAR
BlGetFilePath (
    __in PUNALIGNED_FILE_PATH FilePath
    )

/*++

Routine Description:

    Returns a pointer to the file path for the specified file path structure

Arguments:

     FilePath - pointer to the file path structure

Return Value:

    the file path portion of the specified file path structure.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE Device;
    PWCHAR StringFilePath;

    BlpInterfaceEnter(InterfaceMisc);
    Device = BlGetDevice(FilePath);
    if (Device != NULL) {
        StringFilePath = Add2Ptr(Device, Device->Size);

    } else {
        StringFilePath = NULL;
    }

    BlpInterfaceExit(InterfaceMisc);
    return StringFilePath;
}

VOID
BlDestroyBootEntry (
    __inout PBOOT_APPLICATION_ENTRY BootEntry
    )

/*++

Routine Description:

    This routine frees the memory associated with a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to free.

Return Value:

    None.

--*/

{

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED)) {
        BlMmFreeHeap(BootEntry->Options);
    }

    BlMmFreeHeap(BootEntry);
    return;
}

LIBRARY_PARAMETERS
BlGetCurrentLibraryParameters (
    VOID
    )

/*++

Routine Description:

    This routine returns the current library parameters.  The output of this
    routine is designed to be passed to BlInitializeLibrary by a boot
    application.

Arguments:

    None.

Return Value:

    The current library parameters.

--*/

{

    LIBRARY_PARAMETERS Parameters;

    BlpInterfaceEnter(InterfaceMisc);

    ASSERT(BlpLibraryParametersInitialized != FALSE);

    RtlCopyMemory(&Parameters,
                  &BlpLibraryParameters,
                  sizeof(LIBRARY_PARAMETERS));

    BlpInterfaceExit(InterfaceMisc);
    return Parameters;
}

LIBRARY_PARAMETERS
BlGetDefaultLibraryParameters (
    VOID
    )

/*++

Routine Description:

    This routine returns the default library parameters.  The output of this
    routine is designed to be passed to BlInitializeLibrary by a boot
    application that does not explicitly provide overridden parameters .

Arguments:

    None.

Return Value:

    The default library parameters.

--*/

{

    LIBRARY_PARAMETERS Parameters;

    BlpInterfaceEnter(InterfaceMisc);

#if defined(WIN_APP)

    Parameters.Flags = LF_BOOT_OPTIONS_INITIALIZED;

#else

    Parameters.Flags = LF_BOOT_OPTIONS_INITIALIZED | LF_ZERO_SENSITIVE_DATA;

#endif

    Parameters.TranslationType = VA_TRANSLATION_TYPE_DEFAULT;
    Parameters.MinimumPageAllocation = PAGE_ALLOCATION_MINIMUM_COUNT;
    Parameters.MinimumHeapSize = HEAP_MINIMUM_SIZE;
    Parameters.HeapAllocationAttributes = MEMORY_ATTRIBUTE_NONE;
    Parameters.ApplicationBaseDirectory = NULL;
    Parameters.NumberOfDynamicDescriptors = DEFAULT_DYNAMIC_DESCRIPTOR_COUNT;
    Parameters.FontDirectory = NULL;
    BlpInterfaceExit(InterfaceMisc);
    return Parameters;
}

VOID
BlSetLibraryFlags (
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine sets the library parameter flags to the specified value.

Arguments:

    Flags - Supplies a bitmask containing library flags.

Return Value:

    The current library parameters.

--*/

{

    BlpInterfaceEnter(InterfaceMisc);

    ASSERT(BlpLibraryParametersInitialized != FALSE);

    BlpLibraryParameters.Flags = Flags;
    BlpInterfaceExit(InterfaceMisc);
    return;
}

NTSTATUS
BlInitializeLibrary (
    __in PBOOT_APPLICATION_PARAMETERS ApplicationParameters,
    __in PLIBRARY_PARAMETERS LibraryParameters
    )

/*++

Routine Description:

    This routine will initialize the boot environment library for a boot
    application.

Arguments:

    ApplicationParameters - Supplies the input parameters for a boot environment
        application.

    LibraryParameters - Supplies the library input parameters.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a memory allocation fails.

    STATUS_INVALID_PARAMETER if an input parameter is invalid.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // If requested, reinitialize the library, otherwise initialize the
    // library.
    //

    if (CHECK_FLAG(LibraryParameters->Flags, LF_REINITIALIZE)) {
        Status = ReinitializeLibrary(LibraryParameters);

    } else {
        Status = InitializeLibrary(ApplicationParameters, LibraryParameters);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;

}

NTSTATUS
BlDestroyLibrary (
    VOID
    )

/*++

Routine Description:

    This routine will destroy data structures utilized by the boot environment
    application and will return the execution environment to the original
    operating state.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    ERROR otherwise (If more than one registered destroy routine fails, the
        error returned will be the first failure status code received).

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);
    ReturnStatus = STATUS_SUCCESS;

    //
    // Notify all registered components regarding the start of library
    // destruction.
    //

    BlEnNotifyEvent(EVENT_LIBRARY_DESTRUCTION_START, NULL);

    //
    // Tear down all different subsytems in sequence.
    //

    Status = BlpTimeDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Destroy the logger and SI subsystems before the memory and persistent
    // data subsystems are destroyed.
    //

    Status = BlLogDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpSiDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpPdDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlSecureBootDestroy();

    //
    // Perform application phase of MM destruction prior to internal library
    // destruction.
    //

    Status = BlpMmDestroy(0);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Destroy each library subsystem in reverse dependency order.
    //

    BlCryptoDestroy();

    Status = BlpResourceDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpDisplayDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    ImgpDestroy();
    BlpBdDestroy();
    BlpPltDestroy();
    BlNetDestroy();
    Status = BlpIoDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlLdrDestroy();
    Status = BlpEnDestroy(0);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpTpmDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpFwDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlpArchDestroy();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Free the application options, if necessary.
    //

    if (CHECK_FLAG(BlpApplicationEntry.Attributes,
                   BOOT_ENTRY_OPTIONS_ALLOCATED)) {

        BlMmFreeHeap(BlpApplicationEntry.Options);
    }

    //
    // Lastly, destroy the memory subsystem and destroy the memory context of
    // the calling boot application.
    //

    Status = BlpMmDestroy(1);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlpInterfaceExit(InterfaceMisc);
    return ReturnStatus;
}

// ------------------------------------------------------ Boot Option Functions

NTSTATUS
BlGetBootOption (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PBOOT_ENTRY_OPTION *Option
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Option - Supplies a pointer to a variable that receives the option.  The
        caller is responsible for freeing this memory.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_NO_MEMORY if there was a memory allocation failure.

    Another error code if the option could not be found.

--*/

{

    PBOOT_ENTRY_OPTION LocalOption;
    ULONG Size;
    PBOOT_ENTRY_OPTION StoredOption;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Retrieve the option.
    //

    StoredOption = BcdUtilGetBootOption(OptionList, Type);
    if (StoredOption == NULL) {
        return STATUS_NOT_FOUND;
    }

    Size = BlGetBootOptionSize(StoredOption);
    LocalOption = BlMmAllocateHeap(Size);
    if (LocalOption == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlCopyMemory(LocalOption, StoredOption, Size);
    LocalOption->NextOptionOffset = 0;
    *Option = LocalOption;
    return STATUS_SUCCESS;
}

BOOLEAN
BlBootOptionExists (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type
    )

/*++

Routine Description:

    This routine determines whether an option with the given type is present
    in the specified option list.  It does not provide any data about the
    option, and is not an indication that an option is "enabled."  It simply
    records the option's presence in the list.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option type to find in the list.

Return Value:

    TRUE if the option is found in the list.

    FALSE otherwise.

--*/

{

    PBOOT_ENTRY_OPTION Option;

    BlpInterfaceEnter(InterfaceMisc);

    Option = BcdUtilGetBootOption(OptionList, Type);
    BlpInterfaceExit(InterfaceMisc);
    if (Option != NULL) {
        return TRUE;

    } else {
        return FALSE;
    }
}

NTSTATUS
BlInitGetBootOptionBoolean (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PBOOLEAN Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a boolean.

    N.B. This routine can be called prior to library initialization and thus
         must only interact with pure data processing routines with no
         connection to the initialization state of the library.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the boolean.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    BOOLEAN LocalValue;
    NTSTATUS Status;

    Status = BcdUtilGetBootOptionBoolean(OptionList, Type, &LocalValue);
    if (Status == STATUS_INVALID_PARAMETER) {
        goto  GetBootOptionBooleanEnd;
    }

    //
    // Call a registered callback.
    //

    Status = BlpBootOptionCallbackBoolean(Status,
                                          BCD_CALLBACK_OPERATION_GET,
                                          BlGetApplicationIdentifier(),
                                          Type,
                                          &LocalValue);

    if (NT_SUCCESS(Status)) {
        *Value = LocalValue;
    }

GetBootOptionBooleanEnd:
    return Status;
}

BOOLEAN
BlInMemorySettingAllowed (
    __in PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Setting
    )

/*++

Routine Description:

    This routine determines if the supplied setting exists in the list of
    allowed in-memory BCD settings that can be passed to the supplied boot
    application.

Arguments:

    BootEntry - Supplies a pointer to the boot application entry whose list of
        allowed in-memory settings is to be searched.

    Settings - Supplies the setting to be searched for.

Return Value:

    TRUE if the setting is allowed. FALSE otherwise.

--*/

{

    ULONGLONG UNALIGNED *AllowedInMemorySettings;
    ULONGLONG Index;
    ULONGLONG NumberOfSettings;
    NTSTATUS Status;

    Status = BlpGetBootOptionIntegerList(
                 BootEntry->Options,
                 BCDE_LIBRARY_TYPE_ALLOWED_IN_MEMORY_SETTINGS,
                 (PULONGLONG *)&AllowedInMemorySettings,
                 &NumberOfSettings,
                 TRUE);

    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    for (Index = 0; Index < NumberOfSettings; Index += 1) {
        if (AllowedInMemorySettings[Index] == Setting) {
            return TRUE;
        }
    }

    return FALSE;
}

NTSTATUS
BlGetBootOptionBoolean (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PBOOLEAN Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a boolean.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the boolean.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);
    Status = BlInitGetBootOptionBoolean(OptionList, Type, Value);
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlInitGetBootOptionInteger (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PULONGLONG Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of an integer.

    N.B. This routine can be called prior to library initialization and thus
         must only interact with pure data processing routines with no
         connection to the initialization state of the library.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the integer.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    NTSTATUS Status;

    Status = BcdUtilGetBootOptionInteger(OptionList, Type, Value);
    if (Status == STATUS_INVALID_PARAMETER) {
        goto GetBootOptionIntegerEnd;
    }

    //
    // Call a registered callback.
    //

    Status = BlpBootOptionCallbackULongLong(Status,
                                            BCD_CALLBACK_OPERATION_GET,
                                            BlGetApplicationIdentifier(),
                                            Type,
                                            Value);

GetBootOptionIntegerEnd:
    return Status;
}

NTSTATUS
BlGetBootOptionInteger (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PULONGLONG Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of an integer.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the integer.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);
    Status = BlInitGetBootOptionInteger(OptionList, Type, Value);
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlGetBootOptionIntegerList (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out PULONGLONG *Value,
    __out PULONGLONG Count
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of an integer list.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the integer list.

    Count - Supplies a pointer to a variable that receives the number of
        integers in the list.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    return BlpGetBootOptionIntegerList(OptionList, Type, Value, Count, FALSE);
}

NTSTATUS
BlpGetBootOptionIntegerList (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __deref_out_ecount(*Count) PULONGLONG *Value,
    __out PULONGLONG Count,
    __in BOOLEAN ReturnInternalBuffer
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of an integer list.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the integer list.

    Count - Supplies a pointer to a variable that receives the number of
        integers in the list.

    ReturnInternalBuffer - Supplies a boolean that indicates if the return
        buffer should be allocated from heap, or simply return a pointer to
        the library's internal memory.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    PULONGLONG IntegerList;
    PBCDE_INTEGER_LIST IntegerListOption;
    PBOOT_ENTRY_OPTION Option;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Make sure the option type being requested has the correct data format.
    //

    if (GET_BCDE_DATA_FORMAT(Type) != BCDE_FORMAT_INTEGER_LIST) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetBootOptionIntegerListEnd;
    }

    //
    // Retrieve the option.
    //

    Option = BcdUtilGetBootOption(OptionList, Type);
    if (Option == NULL) {
        Status = STATUS_NOT_FOUND;
        goto GetBootOptionIntegerListEnd;
    }

    //
    // The option data returned is part of the boot entry's option list.
    // Allocate a buffer to copy the option data into.
    //

    IntegerListOption = (PBCDE_INTEGER_LIST)Add2Ptr(Option,
                                                    Option->DataOffset);

    if (ReturnInternalBuffer == TRUE) {
        *Value = (UNALIGNED PULONGLONG)IntegerListOption->Value;
        *Count = Option->DataSize / sizeof(ULONGLONG);
        Status = STATUS_SUCCESS;
        goto GetBootOptionIntegerListEnd;
    }

    IntegerList = BlMmAllocateHeap(Option->DataSize);
    if (IntegerList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBootOptionIntegerListEnd;
    }

    RtlCopyMemory(IntegerList, IntegerListOption->Value, Option->DataSize);
    *Value = IntegerList;
    *Count = Option->DataSize / sizeof(ULONGLONG);
    Status = STATUS_SUCCESS;

GetBootOptionIntegerListEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlGetBootOptionString (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __deref_out PWSTR *Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a NULL terminated wide character string.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the string.  The
        string will be NULL-terminated.  The caller is responsible for freeing
        this memory.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    ULONG AllocationSize;
    PWSTR CopiedString;
    NTSTATUS Status;
    UNICODE_STRING String;
    PCWSTR ValueString;
    ULONG ValueStringLength;

    BlpInterfaceEnter(InterfaceMisc);

    RtlInitUnicodeString(&String, NULL);
    Status = BcdUtilGetBootOptionString(OptionList, Type, &String);
    if (Status == STATUS_INVALID_PARAMETER) {
        goto GetBootOptionStringEnd;
    }

    ValueString = String.Buffer;
    ValueStringLength = String.Length / sizeof(WCHAR);

    //
    // Call a registered callback.
    //

    Status = BlpBootOptionCallbackString(Status,
                                         BCD_CALLBACK_OPERATION_GET,
                                         BlGetApplicationIdentifier(),
                                         Type,
                                         ValueString,
                                         ValueStringLength,
                                         &ValueString,
                                         &ValueStringLength);

    if (!NT_SUCCESS(Status)) {
        goto GetBootOptionStringEnd;
    }

    //
    // The option data returned is part of the boot entry's option list.
    // Allocate a buffer to copy the option data into.
    //

    Status = RtlULongAdd(ValueStringLength, 1, &AllocationSize);
    if (!NT_SUCCESS(Status)) {
        goto GetBootOptionStringEnd;
    }

    Status = RtlULongMult(AllocationSize, sizeof(WCHAR), &AllocationSize);
    if (!NT_SUCCESS(Status)) {
        goto GetBootOptionStringEnd;
    }

    CopiedString = (PWSTR)BlMmAllocateHeap(AllocationSize);
    if (CopiedString == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBootOptionStringEnd;
    }

    RtlCopyMemory(CopiedString, ValueString, AllocationSize - sizeof(WCHAR));
    CopiedString[ValueStringLength] = UNICODE_NULL;
    *Value = (PWSTR)CopiedString;
    Status = STATUS_SUCCESS;

GetBootOptionStringEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlInitGetBootOptionGuid (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out GUID *Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a GUID.

    N.B. This routine can be called prior to library initialization and thus
         must only interact with pure data processing routines with no
         connection to the initialization state of the library.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the GUID.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    PBCDE_OBJECT GuidOption;
    PBOOT_ENTRY_OPTION Option;
    NTSTATUS Status;

    //
    // Make sure the option type being requested has the correct data format.
    //

    if (GET_BCDE_DATA_FORMAT(Type) != BCDE_FORMAT_OBJECT) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetBootOptionGuidEnd;
    }

    //
    // Retrieve the option.
    //

    Option = BcdUtilGetBootOption(OptionList, Type);
    if (Option == NULL) {
        Status = STATUS_NOT_FOUND;
        goto GetBootOptionGuidEnd;
    }

    GuidOption = (PBCDE_OBJECT)Add2Ptr(Option, Option->DataOffset);
    *Value = GuidOption->Object;
    Status = STATUS_SUCCESS;

GetBootOptionGuidEnd:
    return Status;
}

NTSTATUS
BlGetBootOptionGuid (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __out GUID *Value
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a GUID.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the GUID.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);
    Status = BlInitGetBootOptionGuid(OptionList, Type, Value);
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlGetBootOptionGuidList (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __deref_out_bcount(*Count) GUID **Value,
    __out PULONG Count
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of an array of GUIDs.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Value - Supplies a pointer to a variable that receives the array of GUIDs.
        The caller is responsible for freeing this memory.

    Count - Supplies a pointer to a variable that receives the number of GUIDs
        in the option array.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the correct
        data format.

    Another error code if the option could not be found.

--*/

{

    GUID *GuidList;
    PBCDE_OBJECT_LIST GuidListOption;
    PBOOT_ENTRY_OPTION Option;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Make sure the option type being requested has the correct data format.
    //

    if (GET_BCDE_DATA_FORMAT(Type) != BCDE_FORMAT_OBJECT_LIST) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetBootOptionGuidListEnd;
    }

    //
    // Retrieve the option.
    //

    Option = BcdUtilGetBootOption(OptionList, Type);
    if (Option == NULL) {
        Status = STATUS_NOT_FOUND;
        goto GetBootOptionGuidListEnd;
    }

    //
    // The option data returned is part of the boot entry's option list.
    // Allocate a buffer to copy the option data into.
    //

    GuidListOption = (PBCDE_OBJECT_LIST)Add2Ptr(Option, Option->DataOffset);
    GuidList = BlMmAllocateHeap(Option->DataSize);
    if (GuidList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBootOptionGuidListEnd;
    }

    RtlCopyMemory(GuidList, GuidListOption->ObjectList, Option->DataSize);
    *Value = GuidList;
    *Count = Option->DataSize / sizeof(GUID);
    Status = STATUS_SUCCESS;

GetBootOptionGuidListEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}


NTSTATUS
BlGetBootOptionDevice (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *Device,
    __deref_opt_out PBOOT_ENTRY_OPTION *AssociatedOptions
    )

/*++

Routine Description:

    This routine retrieves the requested boot option from a boot entry, and
    returns the data in the form of a boot environment device.

Arguments:

    OptionList - Supplies the options list containing the option to search.

    Type - Supplies the option to be retrieved.

    Device - Supplies a pointer to a variable that receives the option data.
        The caller is responsible for freeing this memory.

    AdditionalOptions - Supplies an optional pointer to a variable that
        receives the options associated with the device.  The caller is
        responsible for freeing this memory.

Return Value:

    STATUS_SUCCESS if the option was found successfully.

    STATUS_INVALID_PARAMETER if the requested option is not of the "device"
        data format.

    Another error code if the option could not be found.

--*/

{

    PBCDE_DEVICE DeviceOption;
    PBOOT_ENTRY_OPTION DeviceOptionList;
    PBOOT_ENVIRONMENT_DEVICE FilteredDevice;
    PBOOT_ENTRY_OPTION FilteredDeviceOptions;
    PBOOT_ENTRY_OPTION *FilteredDeviceOptionsPointer;
    PBOOT_ENVIRONMENT_DEVICE LocalDevice;
    PBOOT_ENTRY_OPTION LocalDeviceOptions;
    PBOOT_ENTRY_OPTION Option;
    ULONG RequiredSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    FilteredDeviceOptionsPointer = NULL;
    LocalDevice = NULL;
    LocalDeviceOptions = NULL;

    //
    // Make sure the option type being requested has the correct data format.
    //

    if (GET_BCDE_DATA_FORMAT(Type) != BCDE_FORMAT_DEVICE) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetBootOptionDeviceEnd;
    }

    //
    // Retrieve the option.
    //

    Option = BcdUtilGetBootOption(OptionList, Type);
    if (Option == NULL) {
        Status = STATUS_NOT_FOUND;
        goto GetBootOptionDeviceFilter;
    }

    DeviceOption = (PBCDE_DEVICE)Add2Ptr(Option, Option->DataOffset);

    //
    // The option data returned is part of the boot entry's option list.
    // Allocate a buffer to copy the option data into.
    //

    LocalDevice = BlMmAllocateHeap(DeviceOption->Device.Size);
    if (LocalDevice == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBootOptionDeviceEnd;
    }

    RtlCopyMemory(LocalDevice,
                  &DeviceOption->Device,
                  DeviceOption->Device.Size);

    //
    // If the caller requested the device options, allocate a buffer to hold
    // these.
    //

    if (ARGUMENT_PRESENT(AssociatedOptions) != FALSE) {
        if (Option->AssociatedOptionsOffset != 0) {
            DeviceOptionList = Add2Ptr(Option, Option->AssociatedOptionsOffset);
            RequiredSize = BlGetBootOptionListSize(DeviceOptionList);
            LocalDeviceOptions = BlMmAllocateHeap(RequiredSize);
            if (LocalDeviceOptions == NULL) {
                Status = STATUS_NO_MEMORY;
                goto GetBootOptionDeviceEnd;
            }

            RtlCopyMemory(LocalDeviceOptions, DeviceOptionList, RequiredSize);
        }

        FilteredDeviceOptionsPointer = &FilteredDeviceOptions;
    }

    Status = STATUS_SUCCESS;

GetBootOptionDeviceFilter:
    //
    // Call a registered callback.
    //

    FilteredDevice = LocalDevice;
    FilteredDeviceOptions = LocalDeviceOptions;

    Status = BlpBootOptionCallbackDevice(Status,
                                         BCD_CALLBACK_OPERATION_GET,
                                         BlGetApplicationIdentifier(),
                                         Type,
                                         &FilteredDevice,
                                         FilteredDeviceOptionsPointer);
    if (!NT_SUCCESS(Status)) {
        goto GetBootOptionDeviceEnd;
    }

    //
    // If the filter returned a different device/options, swap them over.
    //

    if (FilteredDevice != LocalDevice)
    {
        if (LocalDevice != NULL) {
            BlMmFreeHeap(LocalDevice);
        }

        LocalDevice = FilteredDevice;
    }

    if (FilteredDeviceOptions != LocalDeviceOptions)
    {
        if (LocalDeviceOptions != NULL) {
            BlMmFreeHeap(LocalDeviceOptions);
        }

        LocalDeviceOptions = FilteredDeviceOptions;
    }

    if (ARGUMENT_PRESENT(AssociatedOptions) != FALSE) {
        *AssociatedOptions = LocalDeviceOptions;
        LocalDeviceOptions = NULL;
    }

    *Device = LocalDevice;
    LocalDevice = NULL;

GetBootOptionDeviceEnd:
    if (LocalDeviceOptions != NULL) {
        BlMmFreeHeap(LocalDeviceOptions);
    }

    if (LocalDevice != NULL) {
        BlMmFreeHeap(LocalDevice);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

ULONG
BlGetBootOptionSize (
    __in PBOOT_ENTRY_OPTION Option
    )

/*++

Routine Description:

    This routine returns the size required to hold a copy of the specified
    option.

Arguments:

    Option - Supplies a pointer to the option for which the size is to
        be determined.

Return Value:

    The size required to hold a copy of the specified option.

--*/

{

    ULONG BufferSize;

    //
    // Allow space for the option, description and data.  If there is no data,
    // this must be an invalid option, and assume it also has no description.
    //

    if (Option->DataOffset != 0) {
        BufferSize = Option->DataOffset + Option->DataSize;

    } else {

        ASSERT(Option->Invalid != FALSE);

        BufferSize = sizeof(BOOT_ENTRY_OPTION);
    }


    //
    // If there is an associated option list, recursively determine its size
    // and add it to the total.
    //

    if (Option->AssociatedOptionsOffset != 0) {
        BufferSize += BlGetBootOptionListSize(
                          Add2Ptr(Option, Option->AssociatedOptionsOffset));
    }

    return BufferSize;
}

ULONG
BlGetBootOptionListSize (
    __in PBOOT_ENTRY_OPTION Options
    )

/*++

Routine Description:

    This routine returns the size required to hold a copy of the specified
    options list.

Arguments:

    Options - Supplies a pointer to the options list for which the size is to
        be determined.

Return Value:

    The size required to hold a copy of the specified options list

--*/

{

    ULONG BufferSize;
    PBOOT_ENTRY_OPTION CurrentOption;
    ULONG NextOffset;
    ULONG OptionSize;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Loop through each of the boot options, computing the size of each.
    //

    BufferSize = 0;
    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(Options, NextOffset);
        OptionSize = BlGetBootOptionSize(CurrentOption);
        BufferSize += OptionSize;

        if (CurrentOption->NextOptionOffset != 0) {

            ASSERT(CurrentOption->NextOptionOffset >= (NextOffset + OptionSize));
        }

        //
        // Move on to the next offset.
        //

        NextOffset = CurrentOption->NextOptionOffset;
    } while (NextOffset != 0);

    BlpInterfaceExit(InterfaceMisc);

    return BufferSize;
}

NTSTATUS
BlCopyBootOptions (
    __in PBOOT_ENTRY_OPTION OptionList,
    __deref_out PBOOT_ENTRY_OPTION *CopiedOptions
    )

/*++

Routine Description:

    This routine copies the boot options out of the specified boot entry into
    a heap-allocated buffer.

Arguments:

    OptionList - Supplies the options list to copy.

    CopiedOptions - Supplies a pointer to a variable that receives the copied
        options.  The caller is responsible for freeing this memory.

Return Value:

    STATUS_SUCCESS if the copy succeeded.

    STATUS_INVALID_PARAMETER if a boot entry is not provided.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    PBOOT_ENTRY_OPTION NewOptions;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    *CopiedOptions = NULL;
    NewOptions = NULL;

    //
    // Allocate a buffer large enough to hold the options.
    //

    Size = BlGetBootOptionListSize(OptionList);
    NewOptions = BlMmAllocateHeap(Size);
    if (NewOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CopyBootOptionsEnd;
    }

    //
    // Fill in the new buffer with a copy of the original.
    //

    RtlCopyMemory(NewOptions, OptionList, Size);
    *CopiedOptions = NewOptions;
    Status = STATUS_SUCCESS;

CopyBootOptionsEnd:
    if (!NT_SUCCESS(Status)) {
        if (NewOptions != NULL) {
            BlMmFreeHeap(NewOptions);
        }
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlRemoveBootOptions (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in PBOOT_ENTRY_OPTION DeleteOptions
    )

/*++

    This routine removes boot options from a boot entry's option list.

Arguments:

    BootEntry - Supplies a pointer to the boot entry for which the options
        are to be deleted.

    DeleteOptions - Supplies a pointer to the option values to delete.

Return Value:

    STATUS_SUCCESS if the delete operation succeeded.

    STATUS_INVALID_PARAMETER if a required parameter is missing.

--*/

{

    PBOOT_ENTRY_OPTION CurrentOption;
    ULONG NextOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // A boot entry and delete options are both required parameters.
    //

    if ((BootEntry == NULL) || (DeleteOptions == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto RemoveBootOptionsEnd;
    }

    //
    // Loop through the delete options, and for each option, if its type
    // matches any of the boot entry's options, remove it.
    //

    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(DeleteOptions, NextOffset);
        NextOffset = CurrentOption->NextOptionOffset;
        BlRemoveBootOption(BootEntry->Options, CurrentOption->Type);
    } while (NextOffset != 0);

    Status = STATUS_SUCCESS;

RemoveBootOptionsEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

VOID
BlUpdateBootOptionInteger (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __in ULONGLONG Value
    )

/*++

Routine Description:

    This routine updates a boot option in the given list. Since a boot option
    list is a contiguous block of memory, update it by writing to the data
    offset of an entry already present in the list. This allows updates to
    occur without reallocating the list.

Arguments:

    OptionList - Supplies the options list containing the option to update.

    Type - Supplies the data type of the option to be updated.

    Value - Supplies an integer value to write to the option list.

Return Value:

    None.

--*/

{

    PBCDE_INTEGER IntegerData;
    PBOOT_ENTRY_OPTION Option;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Retrieve the first instance of this boot option and update it.
    //

    Option = BcdUtilGetBootOption(OptionList, Type);
    if (Option != NULL) {

        ASSERT(Option->DataSize == sizeof(BCDE_INTEGER));

        IntegerData = Add2Ptr(Option, Option->DataOffset);
        IntegerData->Value = Value;
    }

    BlpInterfaceExit(InterfaceMisc);
    return;
}

NTSTATUS
BlUpdateBootOptions (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in PBOOT_ENTRY_OPTION UpdateOptions
    )

/*++

    This routine updates a boot entries options, or appends to them in the
    cases where an option does not already exist.

Arguments:

    BootEntry - Supplies a pointer to the boot entry for which the options
        are to be updated.

    UpdateOptions - Supplies a pointer to the option values to use for the
        update.

Return Value:

    STATUS_SUCCESS if the update succeeded.

    STATUS_INVALID_PARAMETER if a required parameter is missing.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    PBOOT_ENTRY_OPTION CurrentOption;
    PBOOT_ENTRY_OPTION OptionsCache;
    ULONG OriginalAttributes;
    PBOOT_ENTRY_OPTION OriginalOptions;
    ULONG NextOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    OptionsCache = NULL;
    OriginalAttributes = ~(ULONG)BOOT_ENTRY_OPTIONS_ALLOCATED;
    OriginalOptions = NULL;

    //
    // A boot entry and update options are both required parameters.
    //

    if ((BootEntry == NULL) || (UpdateOptions == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto UpdateBootOptionsEnd;
    }

    //
    // Cache a copy of the current boot options to manipulate.
    //

    Status = BlCopyBootOptions(BootEntry->Options, &OptionsCache);
    if (!NT_SUCCESS(Status)) {
        goto UpdateBootOptionsEnd;
    }

    //
    // Loop through the options cache, and for each option, if its type
    // matches any of the update options, remove it.
    //

    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(UpdateOptions, NextOffset);
        NextOffset = CurrentOption->NextOptionOffset;
        BlRemoveBootOption(OptionsCache, CurrentOption->Type);
    } while (NextOffset != 0);

    //
    // Replace the boot entry's options with the invalidated options, but
    // save the original options to free or restore later.
    //

    OriginalOptions = BootEntry->Options;
    OriginalAttributes = BootEntry->Attributes;
    BootEntry->Options = OptionsCache;
    CLEAR_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED);
    SET_FLAGS(BootEntry->Attributes,
              BOOT_ENTRY_OPTIONS_ALLOCATED_EXTERNALLY);

    //
    // Append the update options to the boot entry.
    //

    Status = BlAppendBootOptions(BootEntry, UpdateOptions);
    if (!NT_SUCCESS(Status)) {
        BootEntry->Options = OriginalOptions;
        BootEntry->Attributes = OriginalAttributes;
        OriginalOptions = NULL;
    }

UpdateBootOptionsEnd:
    if ((OriginalOptions != NULL) &&
        (CHECK_FLAG(OriginalAttributes,
                    BOOT_ENTRY_OPTIONS_ALLOCATED) != FALSE)) {

        BlMmFreeHeap(OriginalOptions);
    }

    if (OptionsCache != NULL) {
        BlMmFreeHeap(OptionsCache);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlAppendBootOptions (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in PBOOT_ENTRY_OPTION AppendedOptions
    )

/*++

Routine Description:

    This routine appends boot options to those stored in the specified boot
    entry.

Arguments:

    BootEntry - Supplies a pointer to the boot entry for which the options are
        to be appended.

    AppendedOptions - Supplies a pointer to the options to be appended.

Return Value:

    STATUS_SUCCESS if the append succeeded.

    STATUS_INVALID_PARAMETER if a boot entry is not provided.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    ULONG AppendedSize;
    PBOOT_ENTRY_OPTION CurrentOption;
    PBOOT_ENTRY_OPTION NewOptions;
    ULONG NextOffset;
    PBOOT_ENTRY_OPTION OriginalOptions;
    ULONG OriginalSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    OriginalOptions = BootEntry->Options;

    //
    // Determine the size of both the original and new options, and allocate
    // a buffer large enough to hold both.
    //

    OriginalSize = BlGetBootOptionListSize(OriginalOptions);
    AppendedSize = BlGetBootOptionListSize(AppendedOptions);
    MARK_SAFE_ADDITION(OriginalSize, AppendedSize);
    NewOptions = BlMmAllocateHeap(OriginalSize + AppendedSize);
    if (NewOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AppendBootOptionsEnd;
    }

    //
    // Copy the original options, and then append the new options.
    //

    RtlCopyMemory(NewOptions, OriginalOptions, OriginalSize);
    RtlCopyMemory(Add2Ptr(NewOptions, OriginalSize),
                  AppendedOptions,
                  AppendedSize);

    //
    // The last option in the original options must now point to the beginning
    // of the appended options, and the offsets of each appended option must be
    // fixed up to be relative to the beginning of the entire options list,
    // rather than to the appended options list.
    //
    // First find the last option in the original list.
    //

    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(NewOptions, NextOffset);
        NextOffset = CurrentOption->NextOptionOffset;
    } while (NextOffset != 0);

    //
    // Now add the original list size to the offset for each element until the
    // end of the appended list.
    //

    do {
        MARK_SAFE_ADDITION(CurrentOption->NextOptionOffset, OriginalSize);
        CurrentOption->NextOptionOffset += OriginalSize;
        CurrentOption = Add2Ptr(NewOptions, CurrentOption->NextOptionOffset);
    } while (CurrentOption->NextOptionOffset != 0);

    //
    // Set the new options into the boot entry.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED)) {
        BlMmFreeHeap(BootEntry->Options);
    }

    BootEntry->Options = NewOptions;
    SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED);
    CLEAR_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED_EXTERNALLY);
    Status = STATUS_SUCCESS;

AppendBootOptionsEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlAppendBootOptionBoolean (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in BOOLEAN Value
    )

/*++

Routine Description:

    This routine appends a boolean boot option to the options for a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Value - Supplies the boolean value to be set into the boot option.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{

    PBCDE_BOOLEAN BooleanData;
    PBOOT_ENTRY_OPTION Option;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Allocate a boot option structure large enough to hold a boolean data
    // element.
    //

    Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
    Option = BlMmAllocateHeap(Size);
    if (Option == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AppendBootOptionBooleanEnd;
    }

    //
    // Populate the option with the supplied data and type.
    //

    RtlZeroMemory(Option, Size);
    Option->Type = Type;
    Option->DataSize = sizeof(BCDE_BOOLEAN);
    Option->DataOffset = sizeof(BOOT_ENTRY_OPTION);
    BooleanData = Add2Ptr(Option, Option->DataOffset);
    BooleanData->Value = Value;

    //
    // Append this option to the application list.
    //

    Status = BlAppendBootOptions(BootEntry, Option);

AppendBootOptionBooleanEnd:
    if (Option != NULL) {
        BlMmFreeHeap(Option);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlpAppendBootOptionInteger (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in ULONGLONG Value,
    __in BOOT_OPTION_FVE_HASH_STATE FveHashState
    )

/*++

Routine Description:

    This routine appends an integer boot option to the options for a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Value - Supplies the integer value to be set into the boot option.

    FveHashState - Supplies the method by which FVE should consider this option
         for inclusion in a hash of boot options.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{

    PBCDE_INTEGER IntegerData;
    PBOOT_ENTRY_OPTION Option;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Allocate a boot option structure large enough to hold a boolean data
    // element.
    //

    Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
    Option = BlMmAllocateHeap(Size);
    if (Option == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AppendBootOptionIntegerEnd;
    }

    //
    // Populate the option with the supplied data and type.
    //

    RtlZeroMemory(Option, Size);
    Option->Type = Type;
    Option->FveHashState = FveHashState;
    Option->DataSize = sizeof(BCDE_INTEGER);
    Option->DataOffset = sizeof(BOOT_ENTRY_OPTION);
    IntegerData = Add2Ptr(Option, Option->DataOffset);
    IntegerData->Value = Value;

    //
    // Append this option to the application list.
    //

    Status = BlAppendBootOptions(BootEntry, Option);

AppendBootOptionIntegerEnd:
    if (Option != NULL) {
        BlMmFreeHeap(Option);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlAppendBootOptionInteger (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in ULONGLONG Value
    )

/*++

Routine Description:

    This routine appends an integer boot option to the options for a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Value - Supplies the integer value to be set into the boot option.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{
    return BlpAppendBootOptionInteger(BootEntry, Type, Value, BootOptionFveHashStateDefault);
}

NTSTATUS
BlAppendBootOptionIntegerButDoNotIncludeInFveHashUseWithExtremeCaution (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in ULONGLONG Value
    )

/*++

Routine Description:

    This routine appends an integer boot option to the options for a boot entry.
    However the option is added such that FVE will not consider it when hashing
    the boot options. This must be used with extreme caution.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Value - Supplies the integer value to be set into the boot option.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{
    return BlpAppendBootOptionInteger(BootEntry, Type, Value, BootOptionFveHashStateNever);
}

NTSTATUS
BlAppendBootOptionString (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in PCWSTR Value
    )

/*++

Routine Description:

    This routine appends an string boot option to the options for a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Value - Supplies the string value to be set into the boot option.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{

    ULONG DataSize;
    PBOOT_ENTRY_OPTION Option;
    ULONG OptionSize;
    NTSTATUS Status;
    PWSTR StringData;

    BlpInterfaceEnter(InterfaceMisc);

    ASSERT(GET_BCDE_DATA_FORMAT(Type) == BCDE_FORMAT_STRING);

    Option = NULL;

    //
    // Allocate a boot option structure large enough to hold the string data
    // element.
    //

    Status = RtlULongMult((ULONG)wcslen(Value), sizeof(WCHAR), &DataSize);
    if (!NT_SUCCESS(Status)) {
        goto AppendBootOptionStringEnd;
    }

    Status = RtlULongAdd(DataSize, sizeof(UNICODE_NULL), &DataSize);
    if (!NT_SUCCESS(Status)) {
        goto AppendBootOptionStringEnd;
    }

    OptionSize = sizeof(BOOT_ENTRY_OPTION) + DataSize;
    Option = BlMmAllocateHeap(OptionSize);
    if (Option == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AppendBootOptionStringEnd;
    }

    //
    // Populate the option with the supplied data and type.
    //

    RtlZeroMemory(Option, OptionSize);
    Option->Type = Type;
    Option->DataSize = DataSize;
    Option->DataOffset = sizeof(BOOT_ENTRY_OPTION);
    StringData = Add2Ptr(Option, Option->DataOffset);
    wcscpy_s(StringData, (DataSize / sizeof(WCHAR)),  Value);

    //
    // Append this option to the application's list.
    //

    Status = BlAppendBootOptions(BootEntry, Option);

AppendBootOptionStringEnd:
    if (Option != NULL) {
        BlMmFreeHeap(Option);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
BlAppendBootOptionDevice (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in BCDE_DATA_TYPE Type,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION AssociatedOptions
    )

/*++

Routine Description:

    This routine appends a device boot option to the options for a boot entry.

Arguments:

    BootEntry - Supplies the boot entry to append the option to.

    Type - Supplies the element type describing the option being appended.

    Device - Supplies the device identifier that is the data for the option.

    AssociatedOptions - Supplies an optional options list that is to be
        associated with the device.

Return Value:

    STATUS_SUCCESS if the append succeeds.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if returned from a sub-routine.

--*/

{

    PBCDE_DEVICE DeviceData;
    PBOOT_ENTRY_OPTION Option;
    ULONG OptionListSize;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    Option = NULL;

    //
    // Allocate a boot option structure large enough to hold a boolean data
    // element.
    //

    Size = sizeof(BOOT_ENTRY_OPTION) +
           FIELD_OFFSET(BCDE_DEVICE, Device) +
           Device->Size;

    OptionListSize = 0;
    if (ARGUMENT_PRESENT(AssociatedOptions) != FALSE) {
        OptionListSize = BlGetBootOptionListSize(AssociatedOptions);
        Size += OptionListSize;
    }

    Option = BlMmAllocateHeap(Size);
    if (Option == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AppendBootOptionDeviceEnd;
    }

    //
    // Populate the option with the supplied data and type.
    //

    RtlZeroMemory(Option, Size);
    Option->Type = Type;
    Option->DataSize = FIELD_OFFSET(BCDE_DEVICE, Device) + Device->Size;
    Option->DataOffset = sizeof(BOOT_ENTRY_OPTION);
    DeviceData = Add2Ptr(Option, Option->DataOffset);
    RtlCopyMemory(&DeviceData->Device, Device, Device->Size);

    //
    // Add the associated options, if they are provided.
    //

    if (ARGUMENT_PRESENT(AssociatedOptions) != FALSE) {
        Option->AssociatedOptionsOffset = Option->DataOffset + Option->DataSize;
        RtlCopyMemory(Add2Ptr(Option, Option->AssociatedOptionsOffset),
                      AssociatedOptions,
                      OptionListSize);
    }

    //
    // Append this option to the application list.
    //

    Status = BlAppendBootOptions(BootEntry, Option);

AppendBootOptionDeviceEnd:
    if (Option != NULL) {
        BlMmFreeHeap(Option);
    }

    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

VOID
BlpRemoveBootOption (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type,
    __in BOOT_OPTION_FVE_HASH_STATE FveHashState
    )

/*++

Routine Description:

    This routine removes a boot option from the given list.  Since a boot option
    list is a contiguous block of memory, invalidate it by marking the option
    as invalid without removing the option from the linked list.  This allows
    removals to occur without reallocating the list.

Arguments:

    OptionList - Supplies the options list containing the option to remove.

    Type - Supplies the data type of the option to be removed.

    FveHashState - Supplies the method by which FVE should consider this option
         for inclusion in a hash of boot options.

Return Value:

    None.

--*/

{

    PBOOT_ENTRY_OPTION Option;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Retrieve the boot option and invalidate it.  Repeat the process until
    // the option no longer exists.
    //

    do {
        Option = BcdUtilGetBootOption(OptionList, Type);
        if (Option != NULL) {
            Option->Invalid = TRUE;
            Option->FveHashState = FveHashState;
        }

    } while (Option != NULL);

    BlpInterfaceExit(InterfaceMisc);
    return;
}

VOID
BlRemoveBootOption (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type
    )

/*++

Routine Description:

    This routine removes a boot option from the given list.  Since a boot option
    list is a contiguous block of memory, invalidate it by marking the option
    as invalid without removing the option from the linked list.  This allows
    removals to occur without reallocating the list.

Arguments:

    OptionList - Supplies the options list containing the option to remove.

    Type - Supplies the data type of the option to be removed.

Return Value:

    None.

--*/

{
    BlpRemoveBootOption(OptionList, Type, BootOptionFveHashStateDefault);
}

VOID
BlRemoveBootOptionButLeaveInFveHashUseWithExtremeCaution (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in BCDE_DATA_TYPE Type
    )

/*++

Routine Description:

    This routine removes a boot option from the given list.  Since a boot option
    list is a contiguous block of memory, invalidate it by marking the option
    as invalid without removing the option from the linked list.  This allows
    removals to occur without reallocating the list.

    However the option is removed such that FVE will still consider it when
    hashing the boot options. This must be used with extreme caution.

Arguments:

    OptionList - Supplies the options list containing the option to remove.

    Type - Supplies the data type of the option to be removed.

Return Value:

    None.

--*/

{
    BlpRemoveBootOption(OptionList, Type, BootOptionFveHashStateAlways);
}

NTSTATUS
BlReplaceBootOptions (
    __inout PBOOT_APPLICATION_ENTRY BootEntry,
    __in_opt PBOOT_ENTRY_OPTION Options
    )

/*++

Routine Description:

    This routine replaces a boot entry's options.

Arguments:

    BootEntry - Supplies the boot entry for which the options are to be
        replaced.

    Options - Supplies a pointer to the replacement options.

Return Value:

    STATUS_SUCCESS if the replacement succeeded.

    STATUS_INVALID_PARAMETER if a boot entry is not provided.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    ULONG FlagsToClear;
    PBOOT_ENTRY_OPTION NewOptions;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMisc);

    //
    // Make sure replacement options are provided.
    //

    if (ARGUMENT_PRESENT(Options) == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        goto ReplaceBootOptionsEnd;
    }

    //
    // Clear the current options.
    //

    if (CHECK_FLAG(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED)) {
        BlMmFreeHeap(BootEntry->Options);
    }

    FlagsToClear = (BOOT_ENTRY_OPTIONS_ALLOCATED |
                    BOOT_ENTRY_OPTIONS_ALLOCATED_EXTERNALLY);

    CLEAR_FLAGS(BootEntry->Attributes, FlagsToClear);
    BootEntry->Options = NULL;

    //
    // Create a copy of the supplied options structure.
    //

    Size = BlGetBootOptionListSize(Options);
    NewOptions = BlMmAllocateHeap(Size);
    if (NewOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ReplaceBootOptionsEnd;
    }

    RtlCopyMemory(NewOptions, Options, Size);

    //
    // Set this copy into the boot entry.
    //

    BootEntry->Options = NewOptions;
    SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED);
    Status = STATUS_SUCCESS;

ReplaceBootOptionsEnd:
    BlpInterfaceExit(InterfaceMisc);
    return Status;
}

NTSTATUS
InitializeLibrary (
    __in PBOOT_APPLICATION_PARAMETERS ApplicationParameters,
    __in PLIBRARY_PARAMETERS LibraryParameters
    )

/*++

Routine Description:

    This routine will initialize the boot environment library for a boot
    application.

Arguments:

    ApplicationParameters - Supplies the input parameters for a boot environment
        application.

    LibraryParameters - Supplies the library input parameters.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a memory allocation fails.

    STATUS_INVALID_PARAMETER if an input parameter is invalid.

--*/

{

    PALLOCATED_MEMORY AllocatedMemory;
    PBOOT_APPLICATION_TRANSITION_ENTRY ApplicationEntry;
    ULONG Attributes;
    PFIRMWARE_PARAMETERS FirmwareParameters;
    ULONG RequiredSize;
    NTSTATUS Status;

    //
    // Validate the input parameters.
    //

    RequiredSize = sizeof(BOOT_APPLICATION_PARAMETERS) +
                   sizeof(ALLOCATED_MEMORY) +
                   sizeof(BOOT_APPLICATION_TRANSITION_ENTRY) +
                   sizeof(FIRMWARE_PARAMETERS) +
                   sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS);

    if ((ApplicationParameters == NULL) ||
        (ApplicationParameters->Header.Signature !=
         BOOT_APPLICATION_PARAMETERS_SIGNATURE) ||
        (ApplicationParameters->Header.SizeOfParameters < RequiredSize)) {

        Status = STATUS_INVALID_PARAMETER;
        goto CleanupFromInvalidParametersFailure;
    }

    //
    // Get a pointer to all internal data.
    //

    AllocatedMemory = Add2Ptr(ApplicationParameters,
                              ApplicationParameters->AllocatedMemoryOffset);

    FirmwareParameters = Add2Ptr(ApplicationParameters,
                                 ApplicationParameters->FirmwareDataOffset);

    ApplicationEntry = Add2Ptr(ApplicationParameters,
                               ApplicationParameters->ApplicationEntryOffset);

    //
    // Globally save the boot device.
    //

    BlpBootDevice = Add2Ptr(ApplicationParameters,
                            ApplicationParameters->BootDeviceOffset);

    BlpApplicationBaseDirectory = LibraryParameters->ApplicationBaseDirectory;

    //
    // Perform Phase 0 firmware library initialization.
    //

    Status = BlpFwInitialize(0, FirmwareParameters);
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromFwPhase0Failure;
    }

    //
    // Check for a signature at the beginning of the application entry.  If
    // this does not exist, the entry is in an incompatible format.  Return
    // a status indicating the 9th application parameter is invalid, which will
    // be used by the calling application to detect this mismatch.
    //

    if (strcmp(ApplicationEntry->Signature, BOOT_ENTRY_SIGNATURE) != 0) {
        Status = STATUS_INVALID_PARAMETER_9;
        goto CleanupFromInvalidParametersFailure;
    }

    //
    // Save the input parameters.
    //

    BlpApplicationParameters = ApplicationParameters;
    RtlCopyMemory(&BlpLibraryParameters,
                  LibraryParameters,
                  sizeof(LIBRARY_PARAMETERS));

    RtlCopyMemory(&BlpApplicationEntry.Identifier,
                  &ApplicationEntry->Identifier,
                  sizeof(GUID));

    BlpApplicationEntry.Options = &ApplicationEntry->InlineOptions;
    Attributes = ApplicationEntry->Attributes;
    if (CHECK_FLAG(Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED)) {
        CLEAR_FLAGS(Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED);
        SET_FLAGS(Attributes, BOOT_ENTRY_OPTIONS_ALLOCATED_EXTERNALLY);
    }

    BlpApplicationEntry.Attributes = Attributes;
    BlpLibraryParametersInitialized = TRUE;

    //
    // Perform architecture specific initialization required for MM
    // initialization.
    //

    Status = BlpArchInitialize(0);
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromArch0Failure;
    }

    //
    // Initialize the library's memory manager.
    //

    Status = BlpMmInitialize(AllocatedMemory,
                             ApplicationParameters->TranslationType,
                             LibraryParameters);

    if (!NT_SUCCESS(Status)) {
        goto CleanupFromMmFailure;
    }

    //
    // Perform outstanding firmware library initialization that requires MM
    // services.
    //

    Status = BlpFwInitialize(1, FirmwareParameters);
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromFwPhase1Failure;
    }

    //
    // Initialize the time subsystem.  On EFI this requires MM services.
    //

    Status = BlpTimeInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromTimeFailure;
    }

    //
    // Perform outstanding architecture initialization.
    //

    Status = BlpArchInitialize(1);
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromArch1Failure;
    }

    //
    // Initialize the boot logger. Ignore the status as its not critical for
    // the functioning of the syste, but do the initialization as early as
    // possible to be able to record initialization errors.
    //

    BlpLogInitialize();

    //
    // Initialize the crypto modules.
    //

    BlCryptoInitialize();

    //
    // Initialize TPM services.  Ignore the status as the TPM is not critical
    // for typical functionality of the system.  Initialization (if it succeeds)
    // must occur before the I/O subsytem and the debugger are initialized.
    //

    BlpTpmInitialize();

    //
    // Initialize the event notification subsystem. Components within the IO
    // subsystem may register for event notification, so this must occur
    // prior to IO initialization.
    //

    Status = BlpEnInitalize(0);
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromEnFailure;
    }

    //
    // Initialize loader support.
    //

    Status = BlLdrInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromLdrFailure;
    }

    //
    // Initialize the I/O subsystem.
    //

    Status = BlpIoInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromIoFailure;
    }

    //
    // Initialize the network.
    //

    Status = BlNetInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromNetFailure;
    }

    //
    // Initialize various utility functions.
    //

    Status = BlUtlInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromUtlFailure;
    }

    //
    // Initialize the platform library.  A failure here is not fatal since
    // platform hardware access is only used for the debugger and the
    // hypervisor, the initialization of which is not fatal.
    //

    BlpPltInitialize();

    //
    // Initialize the Secure Boot policy for the current boot app.
    //

    Status = BlSecureBootInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromSecureBootFailure;
    }

    //
    // Perform Phase 0 initialization of the System Integrity boot services.
    // Phase 0 initialization cannot fail (and there is no reasonable recourse
    // if it does), so ignore the return value.
    //
    // N.B. Phase 0 initialization must occur prior to debugger initialization,
    //      because reporting enabling of the debugger requires these minimal
    //      services.
    //

    BlpSiInitialize(0);

    //
    // Initialize the Boot Debugger.  Do not consider it a fatal failure if
    // debugger initialization fails.
    //

    BlBdInitialize();

    //
    // Initalize the display.  Do not consider it a fatal failure if display
    // can not be initialized.  A local console device may not be present.
    //

    if (!CHECK_FLAG(LibraryParameters->Flags, LF_NO_DISPLAY_INITIALIZATION)) {
        BlpDisplayInitialize(LibraryParameters->Flags);
    }

    //
    // Initialize persistent data support.
    //

    Status = BlpPdInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromPdFailure;
    }

    //
    // Perform Phase 1 initialization of the System Integrity boot services.
    // Failure is not fatal; the library will take necessary steps to ensure
    // that security is maintained.
    //
    // N.B. Phase 1 initialization must occur after Persistent Data
    //      initialization, since the PD library is used to pass SI data
    //      between boot applications.
    //

    BlpSiInitialize(1);

    //
    // Initialize the resource subsystem.
    //
    // N.B. Resource initialization must occur after display initialization to
    //      ensure graphics capabilities can be queried when determining the
    //      resource file to utilize.
    //

    Status = BlpResourceInitialize();
    if (!NT_SUCCESS(Status)) {
        goto CleanupFromResourceFailure;
    }

    BlpLibraryParameters.Flags |= LF_INITIALIZED;
    Status = STATUS_SUCCESS;
    goto InitializeLibraryEnd;

    //
    // In the event of an initialization failure, destroy the library in
    // reverse dependency order.
    //

CleanupFromResourceFailure:
    BlpResourceDestroy();

    //
    // This instance of BlpSiDestroy is for fully-initialized SI system.
    // It needs to execute before Pd sub-system is destroyed in order to
    // have a chance to save measured boot entries if any.
    //

    BlpSiDestroy();

    BlpPdDestroy();

CleanupFromPdFailure:
    if (!CHECK_FLAG(LibraryParameters->Flags, LF_NO_DISPLAY_INITIALIZATION)) {
        BlpDisplayDestroy();
    }

    //
    // This instance of BlpSiDestroy is for Phase 0-only initialized SI system.
    // It is safe to call it even when an earlier call to BlpSiDestroy has been
    // made above.
    //

    BlpSiDestroy();

    BlSecureBootDestroy();
    BlpBdDestroy();

CleanupFromSecureBootFailure:
    BlpMmDestroy(0);
    BlpPltDestroy();

CleanupFromUtlFailure:
    BlNetDestroy();

CleanupFromNetFailure:
    BlpIoDestroy();

CleanupFromLdrFailure:
    BlLdrDestroy();

CleanupFromIoFailure:
    BlpEnDestroy(0);

CleanupFromEnFailure:
    BlpArchDestroy();

CleanupFromArch1Failure:
CleanupFromTimeFailure:
CleanupFromFwPhase1Failure:
    BlpMmDestroy(1);

CleanupFromMmFailure:
CleanupFromArch0Failure:
CleanupFromFwPhase0Failure:
CleanupFromInvalidParametersFailure:
InitializeLibraryEnd:
    return Status;
}

NTSTATUS
ReinitializeLibrary (
    __in PLIBRARY_PARAMETERS LibraryParameters
    )

/*++

Routine Description:

    This routine reinitializes the boot library.

Arguments:

    LibraryParameters - Supplies the library input parameters.

Return Value:

    None.

--*/

{

    ULONG Flags;

    //
    // Re-save the input parameters in case anything has changed.
    //

    Flags = LibraryParameters->Flags;
    RtlCopyMemory(&BlpLibraryParameters,
                  LibraryParameters,
                  sizeof(LIBRARY_PARAMETERS));

    ASSERT((Flags & LF_REINITIALIZE) == LF_REINITIALIZE);

    //
    // If the boot options are initialized, then reinitialize subsystems which
    // have work to do in that case.
    //

    if (CHECK_FLAG(Flags, LF_BOOT_OPTIONS_INITIALIZED)) {

        //
        // Reinitialize the System Integrity library. It does not actually
        // matter which phase of initialization is requested, since the library
        // treats reinitialization specially.
        //

        BlpSiInitialize(1);

        //
        // Attempt to re-initialize the debugger.  If the new options include
        // an option to enable the debugger, this will cause the debugger to
        // connect.
        //

        BlBdInitialize();

        //
        // Remove any bad memory from the system memory map.
        //

        BlMmRemoveBadMemory();

        //
        // Reinitialize the memory manager to obey any memory constraints that
        // may have been loaded.
        //

        BlpMmInitializeConstraints();

        //
        // Reinitialize the display.  The new options may include EMS data which
        // will allow a remote console to be registered successfully, as well as
        // locale information to allow fonts to be initialized successfully.
        //

        BlpDisplayInitialize(Flags);

        //
        // To properly initialize the MUI resource package requires that the
        // application options (specifically the preferredlocale) be
        // initialized.  Now that the application boot options are valid, the
        // resource package can be reinitialized.  Failure of this routine is
        // not fatal.
        //
        // N.B. Resource initialization must occur after display initialization
        //      to ensure graphics capabilities can be queried when determining
        //      the resource file to utilize.
        //

        BlpResourceInitialize();
    }

    return STATUS_SUCCESS;
}

VOID
BlSetBootOptionCallbacks (
    __in_opt PCBOOT_OPTION_CALLBACKS Callbacks,
    __in_opt PVOID Cookie
    )

/*++

Routine Description:

    This routine sets a new set, or deletes an existing set, of boot option
    callbacks. When setting callbacks, not all need to be defined. If a
    particular callback is not needed, null may be set as the value.

Arguments:

    Callbacks - Supplies a structure containing the callbacks to use. This
         should be null to delete the existing set of callbacks.

    Cookie - Supplies a cookie to pass to the callbacks upon each call. This
         should be null when deleting an existing set of callbacks.

Return Value:

    None.

--*/

{
    BlpBootOptionCallbacks = Callbacks;
    BlpBootOptionCallbackCookie = Cookie;
}

NTSTATUS
BlpBootOptionCallbackBoolean (
    __in NTSTATUS BootOptionStatus,
    __in BCD_CALLBACK_OPERATION Operation,
    __in_opt PGUID Object,
    __in BCDE_DATA_TYPE Type,
    __inout PBOOLEAN CurrentValue
    )

/*++

Routine Description:

    This routine calls a registered boot option boolean callback with
    the value of the current boot option (if any) so that the callback can
    override the value or block the action by returning an error.

Arguments:

    BootOptionStatus - Supplies the current status code, typically
         STATUS_SUCCESS or STATUS_NOT_FOUND.

    Operation - Supplies the type of operation being performed on the boot
         option. Get, Set or Delete.

    Object - Supplies the boot object being operated on.

    Type - Supplies the BCD element type being operated on.

    CurrentValue - Supplies a pointer to a variable that on input contains the
         current value of the boot option and upon return contains the updated
         value.

Return Value:

    STATUS_SUCCESS if the callback returned a value.

    The value of BootOptionStatus if there was no registered callback or the
         callback did not update the value.

    Another error code if the callback detected malformed data in its rules
         processing.

--*/

{
    NTSTATUS Status = BootOptionStatus;

    if (BlpBootOptionCallbacks != NULL &&
        BlpBootOptionCallbacks->Boolean != NULL) {
            Status = BlpBootOptionCallbacks->Boolean(
                            BlpBootOptionCallbackCookie,
                            Status,
                            Operation,
                            Object,
                            Type,
                            CurrentValue);
    }

    return Status;
}

NTSTATUS
BlpBootOptionCallbackULongLong (
    __in NTSTATUS BootOptionStatus,
    __in BCD_CALLBACK_OPERATION Operation,
    __in_opt PGUID Object,
    __in BCDE_DATA_TYPE Type,
    __inout PULONGLONG CurrentValue
    )

/*++

Routine Description:

    This routine calls a registered boot option 64-bit integer callback with
    the value of the current boot option (if any) so that the callback can
    override the value or block the action by returning an error.

Arguments:

    BootOptionStatus - Supplies the current status code, typically
         STATUS_SUCCESS or STATUS_NOT_FOUND.

    Operation - Supplies the type of operation being performed on the boot
         option. Get, Set or Delete.

    Object - Supplies the boot object being operated on.

    Type - Supplies the BCD element type being operated on.

    CurrentValue - Supplies a pointer to a variable that on input contains the
         current value of the boot option and upon return contains the updated
         value.

Return Value:

    STATUS_SUCCESS if the callback returned a value.

    The value of BootOptionStatus if there was no registered callback or the
         callback did not update the value.

    Another error code if the callback detected malformed data in its rules
         processing.

--*/

{
    NTSTATUS Status = BootOptionStatus;

    if (BlpBootOptionCallbacks != NULL &&
        BlpBootOptionCallbacks->ULongLong != NULL) {
            Status = BlpBootOptionCallbacks->ULongLong(
                BlpBootOptionCallbackCookie,
                Status,
                Operation,
                Object,
                Type,
                CurrentValue);
    }

    return Status;
}

NTSTATUS
BlpBootOptionCallbackString (
    __in NTSTATUS BootOptionStatus,
    __in BCD_CALLBACK_OPERATION Operation,
    __in_opt PGUID Object,
    __in BCDE_DATA_TYPE Type,
    __in_ecount_opt(StringLength) PCWSTR String,
    __in ULONG StringLength,
    __deref_out_ecount(*ReturnedStringLength) PCWSTR *ReturnedString,
    __out PULONG ReturnedStringLength
    )

/*++

Routine Description:

    This routine calls a registered boot option string callback with the value
    of the current boot option (if any) so that the callback can override the
    value or block the action by returning an error.

Arguments:

    BootOptionStatus - Supplies the current status code, typically
         STATUS_SUCCESS or STATUS_NOT_FOUND.

    Operation - Supplies the type of operation being performed on the boot
         option. Get, Set or Delete.

    Object - Supplies the boot object being operated on.

    Type - Supplies the BCD element type being operated on.

    String - Supplies the current string value of the boot option.

    StringLength - Supplies the character length of the string.

    ReturnedString - Supplies a pointer to a variable to receive the string
         value. The string will be NULL-terminated. If there is no registered
         callback, or the callback does not update the string, the value will
         be the same as String.

    ReturnedStringLength - Supplies a pointer to a variable to receive the
         string length in characters. If there is no registered callback, or
         the callback does not update the string, the value will be the same as
         StringLength.

Return Value:

    STATUS_SUCCESS if the callback returned a string.

    The value of BootOptionStatus if there was no registered callback or the
         callback did not update the string.

    Another error code if the callback detected malformed data in its rules
         processing.

--*/

{
    NTSTATUS Status = BootOptionStatus;

    if (BlpBootOptionCallbacks != NULL &&
        BlpBootOptionCallbacks->String != NULL)
    {
        Status = BlpBootOptionCallbacks->String(
                        BlpBootOptionCallbackCookie,
                        Status,
                        Operation,
                        Object,
                        Type,
                        String,
                        StringLength,
                        ReturnedString,
                        ReturnedStringLength);
    }
    else
    {
        *ReturnedString = String;
        *ReturnedStringLength = StringLength;
    }

    return Status;
}

NTSTATUS
BlpBootOptionCallbackDevice(
    __in NTSTATUS BootOptionStatus,
    __in BCD_CALLBACK_OPERATION Operation,
    __in_opt PGUID Object,
    __in BCDE_DATA_TYPE Type,
    __deref_opt_inout PBOOT_ENVIRONMENT_DEVICE *CurrentDevice,
    __deref_opt_inout_opt PBOOT_ENTRY_OPTION *CurrentDeviceOptions
    )

/*++

Routine Description:

    This routine calls a registered boot option device callback with the value
    of the current boot option (if any) so that the callback can override the
    value or block the action by returning an error.

Arguments:

    Cookie - Supplies an opaque pointer that was registered to be passed back
         to the callbacks when they were registered.

    BootOptionStatus - Supplies the current status code, typically
         STATUS_SUCCESS or STATUS_NOT_FOUND.

    Operation - Supplies the type of operation being performed on the boot
         option. Get, Set or Delete.

    Object - Supplies the boot object being operated on.

    Type - Supplies the BCD element type being operated on.

    CurrentDevice - Supplies a pointer to a variable that on input contains a
         pointer to the current device boot option and upon return contains a
         pointer to the updated (or same) device boot option.

    CurrentDeviceOptions - Supplies a pointer to a variable that on input contains a
         pointer to the current device boot options and upon return contains a
         pointer to the updated (or same) device boot options.

Return Value:

    STATUS_SUCCESS if the callback returned a value.

    The value of BootOptionStatus if there was no registered callback or the
         callback did not update the value.

    Another error code if the callback detected malformed data in its rules
         processing.

--*/
{
    NTSTATUS Status = BootOptionStatus;

    if (BlpBootOptionCallbacks != NULL &&
        BlpBootOptionCallbacks->Device != NULL)
    {
        Status = BlpBootOptionCallbacks->Device(
            BlpBootOptionCallbackCookie,
            Status,
            Operation,
            Object,
            Type,
            CurrentDevice,
            CurrentDeviceOptions);
    }

    return Status;
}

extern
FORCEINLINE
VOID
BlpCriticalInterfaceEnter (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Announces the start of a time sensitive or critical region.

Arguments:

    Name - Supplies the API or API class which is starting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_PERFORMANCE_MONITOR)

    BlpPmCriticalInterfaceEnter(Name);

#endif

#if defined(BOOTENV_MEMORY_CHECK)

    BlpMcCriticalInterfaceEnter(Name);

#endif

}

extern
FORCEINLINE
VOID
BlpCriticalInterfaceExit (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Announces the end of a time sensitive or critical region.

Arguments:

    Name - Supplies the API or API class which is ending.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_PERFORMANCE_MONITOR)

    BlpPmCriticalInterfaceExit(Name);

#endif

#if defined(BOOTENV_MEMORY_CHECK)

    BlpMcCriticalInterfaceExit(Name);

#endif

}

extern
FORCEINLINE
VOID
BlpInterfaceEnter (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Announces the start of a Boot Environment Library API or API class.

    The action of this routine is dependent on C_DEFINES.  Actions include :

        BOOTENV_PERFORMANCE_MOINTOR : Logs a performance counter value
            associated with the entrance of the specified API.  The time
            accrued by the announcing API's caller will be saved accordingly.

        BOOTENV_MEMORY_CHECK : Performs memory validation on internal memory
            management data structures.

Arguments:

    Name - Supplies the API or API class which is starting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_PERFORMANCE_MONITOR)

    BlpPmInterfaceEnter(Name);

#endif

#if defined(BOOTENV_MEMORY_CHECK)

    BlpMcInterfaceEnter(Name);

#endif

#if defined(BOOTENV_PERF_POLLING)

    BlpArchInterfaceEnter(Name);

#endif

}

extern
FORCEINLINE
VOID
BlpInterfaceExit (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Announces the end of a Boot Environment Library API or API class.

    The action of this routine is dependent on C_DEFINES.  Actions include :

        BOOTENV_PERFORMANCE_MONITOR : Saves the time accrued by the exiting
            API.  Resumes the performance analysis of the exiting API's caller.

        BOOTENV_MEMORY_CHECK : Performs memory validation on internal memory
            management data structures.

Arguments:

    Name - Supplies The API or API class which is ending.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

#if defined(BOOTENV_PERFORMANCE_MONITOR)

    BlpPmInterfaceExit(Name);

#endif

#if defined(BOOTENV_MEMORY_CHECK)

    BlpMcInterfaceExit(Name);

#endif

}

