/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiinit.c

Abstract:

    This modules implements the initialization routines for an application
    loaded by EFI firmware.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ---------------------------------------------------------------- Definitions

#define EFI_FILEPATH_SEPARATOR  L'\\'

#if DBG

#define EFI_INIT_DEBUG 0

#endif

// -------------------------------------------------------------------- Globals

UCHAR EfiInitScratch[2048];                // Scratch for input parameters.
UCHAR EfiInitBootOptionBuffer[1024];       // Scratch for BootCurrent handling.

#if EFI_INIT_DEBUG

EFI_DEVICE_PATH *EfiInitDevicePath;
EFI_LOADED_IMAGE *EfiInitLoadedImageInterface;

#endif

// ----------------------------------------------- Internal Function Prototypes

#if EFI_INIT_DEBUG

VOID
EfiDebugInitialize (
    __in struct _EFI_SYSTEM_TABLE *SystemTable
    );

VOID
EfiDebugPrint (
    __in PWSTR String
    );

VOID
EfiDebugPrintBaseAddress (
    __in EFI_LOADED_IMAGE *LoadedImageInterface
    );

#endif

NTSTATUS
EfiInitpAppendPathString  (
    __in_bcount(RemainingSize) WCHAR UNALIGNED *DestinationPath,
    __in ULONG RemainingSize,
    __in_bcount(AppendLength) WCHAR UNALIGNED *AppendPath,
    __in ULONG AppendLength,
    __out PULONG BytesAppended
    );

VOID
EfiInitpCreateApplicationEntry (
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __inout_bcount(EntrySize) PBOOT_APPLICATION_TRANSITION_ENTRY Entry,
    __in ULONG EntrySize,
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath,
    __in EFI_DEVICE_PATH UNALIGNED *FilePath,
    __in_bcount(LoadOptionsSize) PWCHAR LoadOptions,
    __in ULONG LoadOptionsSize,
    __in ULONG Flags,
    __out PULONG ConsumedSize,
    __out PBOOT_ENVIRONMENT_DEVICE *AppDevice
    );

NTSTATUS
EfiInitpConvertEfiDevicePath (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *EfiDevicePath,
    __in BCDE_DATA_TYPE DeviceType,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes
    );

NTSTATUS
EfiInitpConvertEfiFilePath (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *EfiFilePath,
    __in BCDE_DATA_TYPE FilePathType,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes
    );

NTSTATUS
EfiInitpGetBootCurrentLoadOptions (
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __deref_out_bcount(*LoadOptionsSize) PWCHAR *LoadOptions,
    __out PULONG LoadOptionsSize
    );

EFI_DEVICE_PATH UNALIGNED *
EfiInitpGetDeviceNode (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *DevicePath
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiInitpAppendPathString (
    __in_bcount(RemainingSize) WCHAR UNALIGNED *DestinationPath,
    __in ULONG RemainingSize,
    __in_bcount(AppendLength) WCHAR UNALIGNED *AppendPath,
    __in ULONG AppendLength,
    __out PULONG BytesAppended
    )

/*++

Routine Description:

    This routine appends a given path to the destination path. The
    append is done in-place, i.e., the supplied destination path is extended.
    Hence the destination path must provide for the space required to hold
    the append path.

    The destination path is not NULL terminated. The path to be appended
    may or may not be NULL terminated. Further, the length for the path to be
    appended may or may not include the NULL character. The result should be a
    non-NULL terminated concatenated string.

    N.B. The above ambiguity arises due to the lack of clear specifications
         regarding "File Path Media Device Path" in EFI 1.1 or UEFI 2.0.
         Multiple File Path nodes, each describing a portion of the total file
         path, may need to be concatenated together to form the complete path.
         However, the spec is not clear whether each of the parts is
         NULL-terminated or not and also whether the length includes the NULL
         terminator. This leaves scope for each Vendor to implement this in a
         different way.

    The path to be appended may have a leading and/or a trailing path separator
    ('\') character. The result string should be prefixed with a path separator
    and be well formed. For e.g., given two EFI path nodes containing
    "x\" and "\y\", appending them should yield "\x\y".

    N.B. Per UEFI 2.1 (errata) specification, each node can optionally contain
         a path separator at the beginning and/or the end of the string. It
         is the responsibility of the consumer to concatenate multiple nodes
         into a well-formed file path string.

Arguments:

    DestinationPath - Supplies a pointer to the target buffer location where the
        given path is to be appended. The resultant path is not NULL terminated.

    RemainingSize - Supplies the number of bytes available in the target buffer
        for the append operation.

    AppendPath - Supplies a pointer to the path to be append.

    AppendLength - Supplies the size (in bytes) of the path to append.

    BytesAppended - Supplies a pointer to a variable that receives the number
        of bytes appended to target buffer.

Return Value:

    STATUS_SUCCESS if the path was successfully appended.

    STATUS_INVALID_PARAMETER if the path to be appended is invalid.

    STATUS_BUFFER_TOO_SMALL if destination path does not have sufficient space
        to hold the append path.

--*/

{

    WCHAR PathSeparator;
    ULONG Position;
    NTSTATUS Status;
    ULONG TotalLength;

    //
    // Verify that the input path (i.e. the string to append) has a valid
    // length.
    //
    // N.B. NULL strings are treated as valid to accomodate some firmwares that
    //      erroneously provide empty file paths.
    //

    if ((AppendLength % sizeof(WCHAR)) != 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto AppendPathStringEnd;
    }

    //
    // Skip any terminating NULL character present in the input string as the
    // result should be a non-NULL terminated string.
    //

    if (AppendLength >= sizeof(WCHAR)) {
        Position = (AppendLength - sizeof(WCHAR)) / sizeof(WCHAR);
        if (*((PWCHAR)(AppendPath + Position)) == UNICODE_NULL) {
            AppendLength -= sizeof(UNICODE_NULL);
        }
    }

    //
    // Skip any leading path separator ('\') character in the input string.
    // The destination string will be prefixed with a path separator regardless.
    //

    if (AppendLength >= sizeof(WCHAR)) {
        if (*AppendPath == EFI_FILEPATH_SEPARATOR) {
            AppendPath += 1;
            AppendLength -= sizeof(EFI_FILEPATH_SEPARATOR);
        }
    }

    //
    // Skip any trailing path separator ('\') character in the input string.
    //
    // N.B. The path separator will be appended to the destination string on a
    //      subsequent invocation of this routine.
    //

    if (AppendLength >= sizeof(WCHAR)) {
        Position = (AppendLength - sizeof(WCHAR)) / sizeof(WCHAR);
        if (*((PWCHAR)(AppendPath + Position)) == EFI_FILEPATH_SEPARATOR) {
            AppendLength -= sizeof(EFI_FILEPATH_SEPARATOR);
        }
    }

    //
    // Empty strings are treated as valid to accomodate some firmwares that
    // erroneously provide empty file paths.
    //

    if (AppendLength == 0) {
        *BytesAppended = 0;
        Status = STATUS_SUCCESS;
        goto AppendPathStringEnd;
    }

    //
    // Calculate the total number of bytes to be appended including the
    // leading file separator.
    //

    TotalLength = AppendLength + sizeof(EFI_FILEPATH_SEPARATOR);

    //
    // Check whether the destination buffer has adequate space to accomodate
    // the append path.
    //

    if (RemainingSize < TotalLength) {
        Status = STATUS_BUFFER_TOO_SMALL;
        goto AppendPathStringEnd;
    }

    //
    // Append the input string to the destination and prefix it with a path
    // separator.
    //

    PathSeparator = EFI_FILEPATH_SEPARATOR;
    RtlCopyMemory(DestinationPath,
                  &PathSeparator,
                  sizeof(EFI_FILEPATH_SEPARATOR));

    RtlCopyMemory(Add2Ptr(DestinationPath, sizeof(EFI_FILEPATH_SEPARATOR)),
                  AppendPath,
                  AppendLength);

    *BytesAppended = TotalLength;
    Status = STATUS_SUCCESS;

AppendPathStringEnd:
    return Status;
}

PVOID
EfiInitCreateInputParameters (
    __in EFI_HANDLE ImageHandle,
    __in struct _EFI_SYSTEM_TABLE *SystemTable
    )

/*++

Routine Description:

    This routine aids an EFI application in creating the input parameters
    for the boot application.  This routine is expected to be called from
    the EFI entry point of each application.

    N.B. All contents of the application parameter structure are assumed to be
         unaligned by all consumers.  As a result, this routine does not need
         to insert any explicit padding into the parameter structure.

Arguments:

    ImageHandle - Firmware allocated handle for the Boot Manager image.

    SystemTable - A pointer to the EFI system table.

Returns:

    A pointer to a boot application parameter structure for the EFI boot
    application.

    NULL when unsuccessful.

--*/

{

    PVOID Result;

    Result = EfiInitCreateInputParametersEx(ImageHandle, SystemTable, 0);
    return Result;
}

PVOID
EfiInitCreateInputParametersEx (
    __in EFI_HANDLE ImageHandle,
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine aids an EFI application in creating the input parameters
    for the boot application.  This routine is expected to be called from
    the EFI entry point of each application.

    N.B. All contents of the application parameter structure are assumed to be
         unaligned by all consumers.  As a result, this routine does not need
         to insert any explicit padding into the parameter structure.

Arguments:

    ImageHandle - Firmware allocated handle for the Boot Manager image.

    SystemTable - A pointer to the EFI system table.

    Flags - Supplies flags controlling input parameter creation.  This
        parameter can contain one or more of the following flags.

        EFI_INIT_ALLOW_USE_OF_BOOT_CURRENT - Indicates that this routine
            should attempt to infer the application input parameters from the
            BootCurrent environment variable if the loaded image interface
            structure does not contain a valid load options block.

Returns:

    A pointer to a boot application parameter structure for the EFI boot
    application.

    NULL when unsuccessful.

--*/

{

    EFI_PHYSICAL_ADDRESS Address;
    PALLOCATED_MEMORY AllocatedMemory;
    PBOOT_ENVIRONMENT_DEVICE AppDevice;
    ULONG ConsumedSize;
    EFI_DEVICE_PATH *DevicePath;
    EFI_STATUS EfiStatus;
    PBOOT_APPLICATION_TRANSITION_ENTRY Entry;
    ULONG EntrySize;
    PFIRMWARE_PARAMETERS FirmwareParameters;
    PBOOT_APPLICATION_PARAMETERS InputParameters;
    EFI_LOADED_IMAGE *LoadedImageInterface;
    MEMORY_DESCRIPTOR UNALIGNED *MemDesc;
    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArgs;
    ULONG Size;

    SET_FLAGS(BlpApplicationFlags, AF_LOADED_BY_FIRMWARE);

    //
    // Initialize the input parameter structure.
    //

    InputParameters = (PBOOT_APPLICATION_PARAMETERS)&EfiInitScratch[0];
    InputParameters->Header.Signature = BOOT_APPLICATION_PARAMETERS_SIGNATURE;
    InputParameters->Header.Version = BOOT_APPLICATION_PARAMETERS_VERSION;

    //
    // Simple data (not a complex structure referenced from parameters).
    //

    InputParameters->MachineType = BOOT_APPLICATION_MACHINE_TYPE;
    InputParameters->TranslationType = VA_TRANSLATION_TYPE_NONE;

    //
    // Obtain a pointer to the loaded image interface.  This will contain
    // the size and location of the image, the source location of the image
    // and any load options that may be associated with the image.
    //

    EfiStatus = SystemTable->BootServices->HandleProtocol(
                                                 ImageHandle,
                                                 (EFI_GUID*)&EfiLoadedImageProtocol, // const_cast
                                                 &LoadedImageInterface);

    if (EfiStatus != EFI_SUCCESS) {
        return NULL;
    }

#if EFI_INIT_DEBUG

    EfiDebugInitialize(SystemTable);
    EfiDebugPrintBaseAddress(LoadedImageInterface);
    EfiInitLoadedImageInterface = LoadedImageInterface;

#endif

    InputParameters->ImageBase = (UINTN)LoadedImageInterface->ImageBase;
    InputParameters->ImageSize = (ULONG)LoadedImageInterface->ImageSize;

    //
    // Determine the EFI device path for device from which the EFI application
    // was loaded.  This will be used when creating a boot entry for the
    // application.
    //

    EfiStatus = SystemTable->BootServices->HandleProtocol(
                                             LoadedImageInterface->DeviceHandle,
                                             (EFI_GUID*)&EfiDevicePathProtocol,  // const_cast
                                             &DevicePath);

    if (EfiStatus != EFI_SUCCESS) {
        return NULL;
    }

#if EFI_INIT_DEBUG

    EfiInitDevicePath = DevicePath;

#endif

    //
    // On PCAT systems some machines would corrupt page 0x102 during S3 sleep
    // transitions. In Win8 a change was done in Winload to explicitly mark
    // page 0x102 as FirmwarePermanent on all systems, which can cause issues
    // if the page is used for other things (such as page tables) since its
    // usage and marking in the memory map will not match. The change below is
    // a work around to make sure the page is not used in the boot environment.
    //

    Address = (0x102 << PAGE_SHIFT);
    SystemTable->BootServices->AllocatePages(AllocateAddress,
                                             EfiLoaderData,
                                             1,
                                             &Address);

    //
    // Allocated MDL.  Note that unlike other components of the application
    // parameter structure, the MEMORY_DESCRIPTOR structure does not carry an
    // implicit UNALIGNED qualifier.  This routine must use an explicit
    // UNALIGNED memory descriptor pointer as a result.
    //

    Size = sizeof(BOOT_APPLICATION_PARAMETERS);
    InputParameters->AllocatedMemoryOffset = Size;
    AllocatedMemory = Add2Ptr(InputParameters, Size);
    AllocatedMemory->Version = ALLOCATED_MEMORY_VERSION;
    AllocatedMemory->MdlOffset = sizeof(ALLOCATED_MEMORY);
    AllocatedMemory->SizeOfDescriptor = sizeof(MEMORY_DESCRIPTOR);
    AllocatedMemory->NumberOfDescriptors = 1;
    AllocatedMemory->FixedDataOffset = UFIELD_OFFSET(MEMORY_DESCRIPTOR,
                                                    FixedLengthData);

    Size += sizeof(ALLOCATED_MEMORY);
    MemDesc = Add2Ptr(AllocatedMemory, AllocatedMemory->MdlOffset);

    //
    // Create a memory descriptor for the boot application.
    //
    // N.B. We assume that the memory containing the boot application can be
    //      mapped writeback cacheable.  TODO: investigate checking the EFI
    //      memory map to determine the precise set of supported cache
    //      attributes.
    //

    MemDesc->BasePage = (InputParameters->ImageBase >> PAGE_SHIFT);
    MemDesc->PageCount = ALIGN_RANGE_UP(InputParameters->ImageSize, PAGE_SIZE);
    if (MemDesc->PageCount < InputParameters->ImageSize) {
        return NULL;
    }

    MemDesc->PageCount = MemDesc->PageCount >> PAGE_SHIFT;
    MemDesc->Attributes = MEMORY_ATTRIBUTE_CACHE_WRITE_BACK;
    MemDesc->MemoryType = MEMORY_TYPE_BOOT_APPLICATION;

    Size += AllocatedMemory->NumberOfDescriptors * sizeof(MEMORY_DESCRIPTOR);

    //
    // Create a boot entry for the application.
    //

    InputParameters->ApplicationEntryOffset = Size;
    Entry = Add2Ptr(InputParameters, Size);
    EntrySize =
        (ULONG)sizeof(EfiInitScratch) - (ULONG)PtrOffset(InputParameters, Entry);

    EfiInitpCreateApplicationEntry(SystemTable,
                                   Entry,
                                   EntrySize,
                                   DevicePath,
                                   LoadedImageInterface->FilePath,
                                   LoadedImageInterface->LoadOptions,
                                   LoadedImageInterface->LoadOptionsSize,
                                   Flags,
                                   &ConsumedSize,
                                   &AppDevice);

    Size += ConsumedSize;

    //
    // Copy the boot device information from the application device.
    //

    InputParameters->BootDeviceOffset = Size;
    if (AppDevice != NULL) {
        RtlCopyMemory(Add2Ptr(InputParameters, Size),
                      AppDevice,
                      AppDevice->Size);

        Size += AppDevice->Size;

    } else {
        RtlZeroMemory(Add2Ptr(InputParameters, Size),
                      sizeof(BOOT_ENVIRONMENT_DEVICE));

        Size += sizeof(BOOT_ENVIRONMENT_DEVICE);
    }

    //
    // Firmware Parameters.
    //

    InputParameters->FirmwareDataOffset = Size;
    FirmwareParameters = Add2Ptr(InputParameters, Size);
    EfiInitFirmwareParameters(FirmwareParameters,
                              ImageHandle,
                              SystemTable);

    Size += sizeof(FIRMWARE_PARAMETERS);

    //
    // Return Arguments.
    //

    InputParameters->ReturnArgumentOffset = Size;
    ReturnArgs = Add2Ptr(InputParameters, Size);
    ReturnArgs->Version = BOOT_APPLICATION_RETURN_ARGUMENTS_VERSION;
    Size += sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS);

    //
    // Done.
    //

    InputParameters->Header.SizeOfParameters = Size;

    ASSERT(InputParameters->Header.SizeOfParameters <= sizeof(EfiInitScratch));

    return InputParameters;
}

NTSTATUS
EfiInitTranslateDevicePath (
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath,
    __inout_bcount(AvailableBytes) PBOOT_ENVIRONMENT_DEVICE Device,
    __in ULONG AvailableBytes
    )

/*++

Routine Description:

    Attempts to translate an EFI device path into a boot environment
    device.

    This function was written for use before all library services are
    available.  As a result, this function can only handle media device
    paths (partitions, floppys, etc).  In order to determine the device type
    of an arbitrary device, additional services are required.  See the
    block i/o implementation for details on how to do this.

Arguments:

    DevicePath - EFI device path to translate.

    Device - When successful, Device is initialized to describe the device
        in the syntax of the boot environment library.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given device buffer pointer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER when the generated device does not fit in the
        allotted number of bytes.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    ACPI_HID_DEVICE_PATH UNALIGNED *AcpiDevicePath;
    BLOCK_IO_IDENTIFIER UNALIGNED *BlockDevice;
    CDROM_DEVICE_PATH UNALIGNED *CdromNode;
    EFI_DEVICE_PATH UNALIGNED *DevPath;
    HARDDRIVE_DEVICE_PATH UNALIGNED *HardDriveNode;
    MEMMAP_DEVICE_PATH UNALIGNED *MemoryMapNode;
    NTSTATUS Status;

    //
    // Initialize the device identifier for the boot device.
    //

    if (AvailableBytes < sizeof(BOOT_ENVIRONMENT_DEVICE)) {
        return STATUS_INVALID_PARAMETER;
    }

    Device->Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    Status = STATUS_UNSUCCESSFUL;

    //
    // If this device path is rooted on a memory
    // map, then treat it as a RAMDISK.
    //

    if ((DevicePath->Type == HARDWARE_DEVICE_PATH) &&
        (DevicePath->SubType == HW_MEMMAP_DP)) {

        MemoryMapNode = (MEMMAP_DEVICE_PATH UNALIGNED *)DevicePath;
        Device->DeviceType = DEVICE_BLOCK_IO;
        BlockDevice = &Device->u.BlockIo;
        BlockDevice->BlockIoType = BLOCK_IO_RAMDISK;
        BlockDevice->u.RamDisk.ImageBase.QuadPart =
            MemoryMapNode->StartingAddress;

        BlockDevice->u.RamDisk.ImageSize =
            (MemoryMapNode->EndingAddress - MemoryMapNode->StartingAddress);

        BlockDevice->u.RamDisk.ImageOffset = 0;
        return STATUS_SUCCESS;
    }

    //
    // Determine the type of device by looking at the last node on the
    // device path.  This typically is a media device path node, describing
    // the partioning scheme on the device.
    //

    DevPath = EfiInitpGetDeviceNode(DevicePath);
    switch (DevPath->Type) {
    case ACPI_DEVICE_PATH:

        //
        // Check to see if this is a legacy floppy device.
        //

        AcpiDevicePath = (ACPI_HID_DEVICE_PATH UNALIGNED *)DevPath;
        if ((AcpiDevicePath->HID == EISA_PNP_ID(0x604)) ||
            (AcpiDevicePath->HID == EISA_PNP_ID(0x700))) {

            //
            // If this is a legacy floppy device, then the drive number
            // is listed in the UID field.
            //

            Device->DeviceType = DEVICE_BLOCK_IO;
            BlockDevice = &Device->u.BlockIo;
            BlockDevice->BlockIoType = BLOCK_IO_REMOVABLE_DISK;
            BlockDevice->u.RemovableDisk.DriveNumber = AcpiDevicePath->UID;
            Status = STATUS_SUCCESS;
        }

        break;

    case MEDIA_DEVICE_PATH:

        //
        // A Media Device Path describes some abstraction on the  device.
        // Most notably the partitioning scheme or a file path on the device.
        //

        switch (DevPath->SubType) {
        case MEDIA_HARDDRIVE_DP:

            //
            // The EFI Application was booted from a partition on a
            // Harddisk Drive.
            //

            HardDriveNode = (HARDDRIVE_DEVICE_PATH UNALIGNED *)DevPath;
            Device->DeviceType = DEVICE_PARTITION_EX;

            //
            // A problem with booting from the firmware on an EFI system is
            // that the device path does not describe the parent device.
            // The boot library defines a partition as an abstraction ontop
            // of a block i/o device.  Therefore, a partition identifier
            // contains information to describe the parent device.
            // Unfortunately, this information is not available on an EFI
            // system (GPT disk) because the partition identifier is supposed
            // to be unique to the system.  In this case, leave the parent
            // identifier as blank.  Fortunately, the device i/o library was
            // written to handle this exact case.
            //

            BlockDevice = &Device->u.PartitionEx.ParentIdentifier;
            if (HardDriveNode->SignatureType != SIGNATURE_TYPE_MBR) {
                BlockDevice->BlockIoType = BLOCK_IO_HARD_DISK;
            }

            switch (HardDriveNode->SignatureType) {

                //
                // MBR partition case.  Use the deprecated partition
                // identifier format since only the partition number is
                // readily available.  The partition byte offset required by
                // the new format can only be obtained if the block size
                // associated with this partition device is known.  Using the
                // new format is not important since support for EFI MBR boot
                // will be removed before the old partition identifier format
                // is fully deprecated and removed from the product, the only
                // thing that would fully necessitate generation of the new
                // format here.
                //
                // N.B. The partition device is identified only through the
                //      given device path, which may be a short-form device
                //      path understood by the EFI boot manager but not
                //      directly understood by EFI boot services.  The only
                //      reliable way to obtain its block size is to first
                //      enumerate all block I/O handles in the system and
                //      second read the block size from whichever handle has a
                //      media harddrive node in its device path precisely
                //      matching the one supplied by the caller.
                //

            case SIGNATURE_TYPE_MBR:
                Device->DeviceType = DEVICE_PARTITION;
                BlockDevice = &Device->u.Partition.ParentIdentifier;
                BlockDevice->BlockIoType = BLOCK_IO_HARD_DISK;

                //
                // On an MBR disk, the HD device path node does contain
                // the disk signature.
                //

                BlockDevice->u.HardDisk.PartitionType = MBR;
                BlockDevice->u.HardDisk.Mbr.Signature =
                    *((ULONG UNALIGNED *)HardDriveNode->Signature);

                Device->u.Partition.Mbr.PartitionNumber =
                    HardDriveNode->PartitionNumber;

                Status = STATUS_SUCCESS;
                break;

            case SIGNATURE_TYPE_GUID:
                BlockDevice->u.HardDisk.PartitionType = GPT;
                RtlCopyMemory(&Device->u.PartitionEx.Gpt.PartitionSignature,
                              &HardDriveNode->Signature,
                              sizeof(HardDriveNode->Signature));

                Device->Flags |= DEVICE_FLAG_PARENT_SIGNATURE_INVALID;
                Status = STATUS_SUCCESS;
                break;

            default:
                BlockDevice->u.HardDisk.PartitionType = RAW;
                BlockDevice->u.HardDisk.Raw.DiskNumber = 0;
                break;
            }

            break;

        case MEDIA_CDROM_DP:

            //
            // The EFI Application was booted from a CDROM.  The media
            // device path describes the El Torito boot entry number in the
            // boot entry catalog that the application was booted from.
            // This is ignored since the boot environment assumes that all
            // files on a CD are in the one ETFS file system on the CD, and
            // not embedded in the El Torito boot entries.  As a result, the
            // application device is a block device, not a partition.
            //

            CdromNode = (CDROM_DEVICE_PATH UNALIGNED *)DevPath;
            Device->DeviceType = DEVICE_BLOCK_IO;
            BlockDevice = &Device->u.BlockIo;
            BlockDevice->BlockIoType = BLOCK_IO_CDROM;
            BlockDevice->u.Cdrom.CdromNumber = 0;
            Status = STATUS_SUCCESS;
            break;

        default:
            break;
        }

        break;

    case MESSAGING_DEVICE_PATH:

        switch (DevPath->SubType) {
        case MSG_MAC_ADDR_DP:
        case MSG_IPv4_DP:
        case MSG_IPv6_DP:

            //
            // The EFI Application was booted off network.
            //

            Device->DeviceType = DEVICE_UDP;
            Device->u.Udp.HwType = UDP_IDENTIFIER_HW_TYPE_BOOT_DEVICE;
            break;

        default:

            //
            // If we booted off a partition or a CD, the leaf node would have
            // been a MEDIA_DEVICE_PATH.  Instead, the leaf node simply describes
            // the device.  There is no good way to know what type of device
            // this actually is.  But since it is not a CD or partition (aka HD),
            // then assume this is a floppy.
            //

            Device->DeviceType = DEVICE_BLOCK_IO;
            BlockDevice = &Device->u.BlockIo;
            BlockDevice->BlockIoType = BLOCK_IO_REMOVABLE_DISK;
            BlockDevice->u.RemovableDisk.DriveNumber = 0;
            break;
        }

        Status = STATUS_SUCCESS;
        break;

#if !defined(_ARM_) && !defined(_ARM64_)
    case HARDWARE_DEVICE_PATH:

        //
        // Check to see if this is a vmbus device.
        //

        if (EfiVmbusGetChannelInterfaceGuids(
                        (EFI_DEVICE_PATH_PROTOCOL*)DevPath,
                        (PGUID)&Device->u.Vmbus.InterfaceType,
                        (PGUID)&Device->u.Vmbus.InterfaceInstance)) {
            Device->DeviceType = DEVICE_VMBUS;
            Status = STATUS_SUCCESS;
        }
        break;
#endif

    default:
        break;
    }

    return Status;
}

// --------------------------------------------------------- Internal Functions

BOOLEAN
FORCEINLINE
EfiInitpCheckForWindowsOsOptions (
    __in_bcount(LoadOptionsSize) PWCHAR LoadOptions,
    __in ULONG LoadOptionsSize
    )

/*++

Routine Description:

    This function runs basic checks to determine if the given application load
    options resemble a Windows OS options structure.

Arguments:

    LoadOptions - A pointer to the application's option string.

    LoadOptionsSize - The size of the buffer pointed to by LoadOptions.

Return Value:

    TRUE if the given options appear to be Windows OS options, FALSE
    otherwise.

--*/

{

    if ((LoadOptions != NULL)

        &&

        //
        // Make sure the size of the structure is large enough to be a
        // WINDOWS_OS_STRUCTURE, both by LoadOptionsSize says and by the
        // value stored at the Length offset of the structure.
        //

        (sizeof(UNALIGNED_WINDOWS_OS_OPTIONS) <= LoadOptionsSize)

        &&

        (sizeof(UNALIGNED_WINDOWS_OS_OPTIONS) <=
         ((PUNALIGNED_WINDOWS_OS_OPTIONS)LoadOptions)->Length)

        &&

        //
        // A WINDOWS_OS_OPTIONS structure contains a signature which is
        // always initialized to "WINDOWS".
        //

        !strncmp((PCHAR)(&((PUNALIGNED_WINDOWS_OS_OPTIONS)LoadOptions)->Signature[0]),
                 WINDOWS_OS_OPTIONS_SIGNATURE,
                 sizeof(WINDOWS_OS_OPTIONS_SIGNATURE) - 1)) {

        return TRUE;
    }

    return FALSE;
}

NTSTATUS
EfiInitConvertEfiPxeFilePath (
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __in EFI_DEVICE_PATH *EfiDevicePath,
    __in BCDE_DATA_TYPE FilePathType,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes
    )

/*++

Routine Description:

    This routine converts the file path described in the filepath portion
    of a PXE dhcp packet to a path string.

Arguments:

    SystemTable - Supplies a pointer to the EFI system table.

    EfiDevicePath - Supplies a pointer to the EFI device path from which
        PXE client can be queried.

    FilePathType - Supplies the data type to apply to the element.

    Option - Supplies a pointer to the buffer to receive the constructed
        option.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given boot entry option pointer.

Return Value:

    NT status code.

--*/

{

    ULONG DataSize;
    PWCHAR DestinationString;
    EFI_STATUS EfiStatus;
    USHORT NbpPathLength;
    PCHAR NetworkBootProgram;
    EFI_PXE_BASE_CODE *PxeClient;
    EFI_DEVICE_PATH *PxeDevicePath;
    EFI_HANDLE PxeHandle;
    EFI_PXE_BASE_CODE_MODE *PxeMode;
    PBCDE_STRING StringElement;

    //
    // The supplied buffer must be at least large enough to hold the boot
    // option.
    //

    if (AvailableBytes < sizeof(BOOT_ENTRY_OPTION)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Initialize the option.
    //

    DataSize = sizeof(BOOT_ENTRY_OPTION);
    RtlZeroMemory(Option, DataSize);
    Option->Type = FilePathType;
    StringElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
    Option->DataOffset = (ULONG)PtrOffset(Option, StringElement);

    //
    // Get the boot file path from the DHCP proxy offer packet.
    //

    PxeDevicePath = EfiDevicePath;
    EfiStatus = SystemTable->BootServices->LocateDevicePath(
                                                        (EFI_GUID*)&EfiPxeBaseCodeProtocol,  // const_cast
                                                        &PxeDevicePath,
                                                        &PxeHandle);

    if (EfiStatus != EFI_SUCCESS) {
        return STATUS_INVALID_PARAMETER;
    }

    EfiStatus = SystemTable->BootServices->OpenProtocol(
                                                PxeHandle,
                                                (EFI_GUID*)&EfiPxeBaseCodeProtocol, // const_cast
                                                &PxeClient,
                                                EfiImageHandle,
                                                NULL,
                                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EfiStatus != EFI_SUCCESS) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Get the network boot program name from the proxy offer dhcp packet. If
    // the name is not there, then fallback to the dhcp acknowledgment packet
    // since there might not have been a proxy offer. If the name is still not
    // there then just mark the option as invalid and return.
    //

    PxeMode = PxeClient->Mode;
    if (PxeMode->UsingIpv6) {

        //
        // In the case of DHCPv6 the bootfile is encoded in either a URL or
        // vendor-specific WDS option, as opposed to being directly accessible
        // as in the DHCPv4 case.  Since this is not crucial to the boot
        // process, parsing the packet to determine the bootfile is left until
        // the BlNetInitialize stage.
        //

        Option->Invalid = TRUE;
        return STATUS_SUCCESS;

    } else if (PxeMode->ProxyOffer.Dhcpv4.BootpBootFile[0] != '\0') {
        NetworkBootProgram = (PCHAR)&PxeMode->ProxyOffer.Dhcpv4.BootpBootFile;

    } else if (PxeMode->DhcpAck.Dhcpv4.BootpBootFile[0] != '\0') {
        NetworkBootProgram = (PCHAR)&PxeMode->DhcpAck.Dhcpv4.BootpBootFile;

    } else {
        Option->Invalid = TRUE;
        return STATUS_SUCCESS;
    }

    NbpPathLength = (USHORT)strlen(NetworkBootProgram) * sizeof(WCHAR);
    NbpPathLength += sizeof(UNICODE_NULL);
    DataSize += NbpPathLength;
    if (AvailableBytes < DataSize) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If a backslash doesn't exist before the network file path name, then add
    // it now and copy the ansi file path into the unicode string.
    //

    DestinationString = StringElement->String;
    if (*NetworkBootProgram != '\\') {
        DataSize += sizeof(WCHAR);

        if (AvailableBytes < DataSize) {
            return STATUS_INVALID_PARAMETER;
        }

        *DestinationString = L'\\';
        DestinationString += 1;
    }

    BlCopyStringToWcharString(DestinationString, NetworkBootProgram);
    Option->DataSize = DataSize;
    return STATUS_SUCCESS;
}

VOID
EfiInitpCreateApplicationEntry (
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __inout_bcount(EntrySize) PBOOT_APPLICATION_TRANSITION_ENTRY Entry,
    __in ULONG EntrySize,
    __in EFI_DEVICE_PATH UNALIGNED *EfiDevicePath,
    __in EFI_DEVICE_PATH UNALIGNED *EfiFilePath,
    __in_bcount(LoadOptionsSize) PWCHAR LoadOptions,
    __in ULONG LoadOptionsSize,
    __in ULONG Flags,
    __out PULONG ConsumedSize,
    __out PBOOT_ENVIRONMENT_DEVICE *AppDevice
    )

/*++

Routine Description:

    This routine will initialize a boot entry data structure for an EFI
    application.

Arguments:

    SystemTable - Supplies a pointer to the EFI system table.

    Entry - Supplies a pointer to a buffer to initialize with the application
        entry.

    EntrySize - Supplies the size of the entry buffer in bytes.

    EfiDevicePath - Supplies the device path for the application.
        EFI Application.

    EfiFilePath - Supplies the file path for the application.

    LoadOptions - Supplies a pointer to the firmware provided load options.

    LoadOptionsSize - Supplies the size of the load options.

    Flags - Supplies flags controlling application entry creation.  This
        parameter can contain one or more of the following flags.

        EFI_INIT_ALLOW_USE_OF_BOOT_CURRENT - Indicates that this routine
            should attempt to infer the application load options from the
            BootCurrent environment variable if the loaded image interface
            structure does not contain a valid load options block.

    ConsumedSize - Supplies a pointer to a variable that receives the number
        of bytes in the entry buffer consumed by the created application entry.

    AppDevice - Supplies a pointer to a variable that receives a pointer to the
        device identifier for the device from which this application was loaded.
        This will be a pointer to a location within the application entry.

Return Value:

    None.

--*/

{

    PBCDE_DEVICE DeviceElement;
    BOOLEAN IdentifierSpecified;
    GUID Identifier;
    PWCHAR IdentifierString;
    PWCHAR InputOptions;
    ULONG InputOptionsSize;
    PUNALIGNED_FILE_PATH OsFilePath;
    PBOOT_ENTRY_OPTION Option;
    ULONG OptionListOffset;
    PBOOT_ENTRY_OPTION PreviousOption;
    ULONG RemainingSize;
    ULONG Size;
    NTSTATUS Status;
    ULONG TotalSize;
    UNICODE_STRING UnicodeString;
    BOOLEAN UsingWindowsOsOptions;
    WINDOWS_OS_OPTIONS UNALIGNED *WindowsOsOptions;

    *AppDevice = NULL;
    OptionListOffset = 0;
    TotalSize = 0;

    //
    // An EFI Application could have been loaded from the EFI Shell or
    // loaded by the EFI Boot Manager.  The meaning and value of LoadOptions
    // depends on this load technique.  When loaded from the EFI Boot
    // Manager, LoadOptions is a pointer to the OptionalData[] of the
    // NVRAM EFI_LOAD_OPTION.  When loaded from the Shell, this consists
    // of the command line arguments.
    //
    // A boot application that is loaded directly from the EFI boot manager
    // contains a WINDOWS_OS_OPTIONS structure in the OptionalData[] field.
    //
    // If the boot application does not contain a WINDOWS_OS_OPTIONS structure,
    // assume that it is loaded from the command line, and the load options
    // should be processed as a string that represent the arguments to the
    // application.
    //

    UsingWindowsOsOptions = EfiInitpCheckForWindowsOsOptions(LoadOptions,
                                                             LoadOptionsSize);

    if ((UsingWindowsOsOptions == FALSE) &&
        CHECK_FLAG(Flags, EFI_INIT_ALLOW_USE_OF_BOOT_CURRENT)) {

        //
        // The given load options were not formatted as Windows OS options,
        // however the caller requested that BootCurrent be checked for
        // Windows OS options to account for the possibility that the given
        // options were populated incorrectly by the firmware.
        //
        // Query BootCurrent and determine if it points to valid Windows OS
        // options.
        //

        Status = EfiInitpGetBootCurrentLoadOptions(SystemTable,
                                                   &LoadOptions,
                                                   &LoadOptionsSize);

        if (NT_SUCCESS(Status)) {
            UsingWindowsOsOptions = EfiInitpCheckForWindowsOsOptions(
                                        LoadOptions,
                                        LoadOptionsSize);
        }
    }

    //
    // Obtain a pointer to the load options string, either from the Windows
    // OS structure or from the loaded image interface.  Ensure the string is
    // NULL-terminated.
    //

    if (UsingWindowsOsOptions != FALSE) {
        InputOptions = Add2Ptr(LoadOptions,
                               UFIELD_OFFSET(WINDOWS_OS_OPTIONS, OsLoadOptions));

        InputOptionsSize =
            LoadOptionsSize - (ULONG)PtrOffset(LoadOptions, InputOptions);

    } else {
        InputOptions = LoadOptions;
        InputOptionsSize = LoadOptionsSize;
    }

    InputOptionsSize /= sizeof(WCHAR);
    if (InputOptionsSize != 0) {
        if (wcsnlen(InputOptions, InputOptionsSize) == InputOptionsSize) {
            InputOptions[InputOptionsSize - 1] = L'\0';
        }
    }

    //
    // Initialize a boot environment application entry for the EFI application.
    //

    RemainingSize = EntrySize;
    if (RemainingSize < sizeof(BOOT_APPLICATION_TRANSITION_ENTRY)) {
        Status = STATUS_INVALID_PARAMETER;
        goto CreateApplicationEntryEnd;
    }

    RtlZeroMemory(Entry, sizeof(BOOT_APPLICATION_TRANSITION_ENTRY));
    RtlCopyMemory(&Entry->Signature,
                  BOOT_ENTRY_SIGNATURE,
                  sizeof(BOOT_ENTRY_SIGNATURE));

    TotalSize = UFIELD_OFFSET(BOOT_APPLICATION_TRANSITION_ENTRY, InlineOptions);

    //
    // The application entry consists of an application identifier followed by
    // a set of options.  If the current application is BCD aware, it will
    // contain a string of the format /BCDOBJECT={GUID}, in which case the
    // given GUID string will be converted to a GUID and used as the BCD object
    // identifier.  If the BCDOBJECT string is not specified, assume this is
    // a BCD unaware Windows OS loader.
    //

    IdentifierSpecified = FALSE;
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "String in LoadOptions is ok");
    if (LoadOptions != NULL) {
        IdentifierString = wcsstr(InputOptions, L"BCDOBJECT=");
        if (IdentifierString != NULL) {
            IdentifierString =
                Add2Ptr(IdentifierString,
                        sizeof(L"BCDOBJECT=") - sizeof(UNICODE_NULL));

            RtlInitUnicodeString(&UnicodeString, IdentifierString);
            Status = RtlGUIDFromString(&UnicodeString, &Identifier);
            if (NT_SUCCESS(Status)) {
                RtlCopyMemory(&Entry->Identifier, &Identifier, sizeof(GUID));
                IdentifierSpecified = TRUE;
            }
        }

    } else {
        IdentifierString = NULL;
    }

    if (IdentifierSpecified == FALSE) {
        Entry->Attributes |= BOOT_ENTRY_IDENTIFIER_UNSPECIFIED;
    }

    //
    // The options in the application entry are seeded with the application
    // device and file path.
    //

    RemainingSize -= UFIELD_OFFSET(BOOT_APPLICATION_TRANSITION_ENTRY,
                                  InlineOptions);

    PreviousOption = NULL;
    Option = &Entry->InlineOptions;
    OptionListOffset = 0;

    //
    // First the device.
    //
    // N.B. A boot option list requires at least one entry, since the last
    //      option is determined by a NextOptionOffset of zero.  If the
    //      application device can not be translated, create a NULL option.
    //

    Status = EfiInitpConvertEfiDevicePath(EfiDevicePath,
                                          BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                          Option,
                                          RemainingSize);

    if (!NT_SUCCESS(Status)) {
        Option->Invalid = TRUE;
        OptionListOffset = sizeof(BOOT_ENTRY_OPTION);
        goto CreateApplicationEntryEnd;
    }

    DeviceElement = Add2Ptr(Option, Option->DataOffset);
    *AppDevice = &DeviceElement->Device;
    Size = BlGetBootOptionSize(Option);
    OptionListOffset += Size;
    RemainingSize -= Size;
    PreviousOption = Option;
    Option = Add2Ptr(&Entry->InlineOptions, OptionListOffset);

    //
    // Create a second option for the application file path. If the system is
    // booting off UDP (PXE) then get the boot file name from the cached proxy
    // offer or dhcp acknowledgment packet.
    //

    if ((*AppDevice)->DeviceType == DEVICE_UDP) {
        Status = EfiInitConvertEfiPxeFilePath(SystemTable,
                                              (EFI_DEVICE_PATH*)EfiDevicePath,
                                              BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                              Option,
                                              RemainingSize);

    } else {
        Status = EfiInitpConvertEfiFilePath(EfiFilePath,
                                            BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                            Option,
                                            RemainingSize);
    }

    if (!NT_SUCCESS(Status)) {
        goto CreateApplicationEntryEnd;
    }

    PreviousOption->NextOptionOffset =
        (ULONG)PtrOffset(&Entry->InlineOptions, Option);

    Size = BlGetBootOptionSize(Option);
    OptionListOffset += Size;
    RemainingSize -= Size;
    PreviousOption = Option;
    Option = Add2Ptr(Option, Size);

    //
    // If the load options were specified as a WINDOWS_OS_OPTIONS structure,
    // but no mapping was provided to a BCD object, assume the application is
    // a BCD disabled Windows OS loader.  In this case, attempt to create
    // BCD elements for the OS device and OS file path.
    //

    if ((UsingWindowsOsOptions != FALSE) && (IdentifierSpecified == FALSE)) {
        WindowsOsOptions = (WINDOWS_OS_OPTIONS UNALIGNED *)LoadOptions;
        OsFilePath =
            Add2Ptr(WindowsOsOptions, WindowsOsOptions->OsLoadPathOffset);

        if ((OsFilePath->Length > UFIELD_OFFSET(FILE_PATH, FilePath)) &&
            (OsFilePath->Type == FILE_PATH_TYPE_EFI)) {

            //
            // Convert the OS device to a BCD element.
            //

            EfiDevicePath =
                (EFI_DEVICE_PATH UNALIGNED *)&OsFilePath->FilePath[0];

            Status = EfiInitpConvertEfiDevicePath(EfiDevicePath,
                                                  BCDE_OSLOADER_TYPE_OS_DEVICE,
                                                  Option,
                                                  RemainingSize);

            if (!NT_SUCCESS(Status)) {
                goto CreateApplicationEntryEnd;
            }

            PreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(&Entry->InlineOptions, Option);

            Size = BlGetBootOptionSize(Option);
            OptionListOffset += Size;
            PreviousOption = Option;
            Option = Add2Ptr(Option, Size);
            RemainingSize -= Size;

            //
            // Convert the OS path to a BCD element.
            //

            Status = EfiInitpConvertEfiFilePath(EfiDevicePath,
                                                BCDE_OSLOADER_TYPE_SYSTEM_ROOT,
                                                Option,
                                                RemainingSize);

            if (!NT_SUCCESS(Status)) {
                goto CreateApplicationEntryEnd;
            }

            PreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(&Entry->InlineOptions, Option);

            Size = BlGetBootOptionSize(Option);
            OptionListOffset += Size;
            PreviousOption = Option;
            Option = Add2Ptr(Option, Size);
            RemainingSize -= Size;
        }
    }

    //
    // Convert all load options to BCD elements.
    //

    Status = AhCreateLoadOptionsList(InputOptions,
                                     &Entry->InlineOptions,
                                     RemainingSize,
                                     &OptionListOffset,
                                     &PreviousOption,
                                     &Size);

    if (!NT_SUCCESS(Status)) {
        goto CreateApplicationEntryEnd;
    }

    //
    // Return the consumed size and the application device to the caller.
    //

CreateApplicationEntryEnd:
    TotalSize += OptionListOffset;
    *ConsumedSize = TotalSize;
    return;
}

NTSTATUS
EfiInitpConvertEfiDevicePath (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *EfiDevicePath,
    __in BCDE_DATA_TYPE DeviceType,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes
    )

/*++

Routine Description:

    This routine converts an EFI device path into a BCD device element.

Arguments:

    EfiDevicePath - Supplies a pointer to a EFI device path.

    DeviceType - Supplies the data type to apply to the device option.

    Option - Supplies a pointer to a buffer that receives the constructed
        option.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given boot entry option pointer.

Return Value:

    NT status code.

--*/

{

    PBCDE_DEVICE DeviceElement;
    NTSTATUS Status;

    //
    // Translate the EFI device path to a boot environment device.
    //

    if (AvailableBytes < sizeof(BOOT_ENTRY_OPTION)) {
        return STATUS_INVALID_PARAMETER;
    }

    AvailableBytes -= sizeof(BOOT_ENTRY_OPTION);
    RtlZeroMemory(Option, sizeof(BOOT_ENTRY_OPTION));
    DeviceElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
    if (AvailableBytes < UFIELD_OFFSET(BCDE_DEVICE, Device)) {
        return STATUS_INVALID_PARAMETER;
    }

    AvailableBytes -= UFIELD_OFFSET(BCDE_DEVICE, Device);
    Status = EfiInitTranslateDevicePath(EfiDevicePath,
                                        &DeviceElement->Device,
                                        AvailableBytes);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Option->DataOffset = (ULONG)PtrOffset(Option, DeviceElement);
    Option->Type = DeviceType;
    Option->DataSize = UFIELD_OFFSET(BCDE_DEVICE, Device) +
                       DeviceElement->Device.Size;

    return STATUS_SUCCESS;
}

NTSTATUS
EfiInitpConvertEfiFilePath (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *EfiDevicePath,
    __in BCDE_DATA_TYPE FilePathType,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes
    )

/*++

Routine Description:

    This routine converts the file paths described in the filepath portions
    of an EFI device path to a path string.

Arguments:

    EfiDevicePath - Supplies a pointer to the EFI device path from which
        the filepath nodes should be parsed.

    FilePathType - Supplies the data type to apply to the element.

    Option - Supplies a pointer to the buffer to receive the constructed
        option.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given boot entry option pointer.

Return Value:

    NT status code.

--*/

{

    ULONG BytesAppended;
    PWCHAR CurrentBufferPosition;
    EFI_DEVICE_PATH UNALIGNED *CurrentPathNode;
    ULONG DataSize;
    FILEPATH_DEVICE_PATH UNALIGNED *FilePathNode;
    PWCHAR StartingPosition;
    NTSTATUS Status;
    ULONG StringLength;
    PBCDE_STRING StringElement;

    //
    // The supplied buffer must be at least large enough to hold the boot
    // option.
    //

    if (AvailableBytes < sizeof(BOOT_ENTRY_OPTION)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Initialize the option.
    //

    DataSize = sizeof(BOOT_ENTRY_OPTION);
    AvailableBytes -= sizeof(BOOT_ENTRY_OPTION);
    RtlZeroMemory(Option, DataSize);
    Option->Type = FilePathType;
    StringElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
    Option->DataOffset = (ULONG)PtrOffset(Option, StringElement);

    //
    // Copy each file path node into the option.
    //

    CurrentPathNode = EfiDevicePath;
    StartingPosition = &StringElement->String[0];
    CurrentBufferPosition = StartingPosition;
    while (IsDevicePathEndType(CurrentPathNode) == FALSE) {
        if ((CurrentPathNode->Type == MEDIA_DEVICE_PATH) &&
            (CurrentPathNode->SubType == MEDIA_FILEPATH_DP)) {

            //
            // Copy the filepath node to the boot entry.
            //

            Status = RtlULongSub(DevicePathNodeLength(CurrentPathNode),
                                 UFIELD_OFFSET(FILEPATH_DEVICE_PATH, PathName),
                                 &StringLength);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            FilePathNode = (FILEPATH_DEVICE_PATH UNALIGNED *)CurrentPathNode;
            Status = EfiInitpAppendPathString(CurrentBufferPosition,
                                              AvailableBytes,
                                              &FilePathNode->PathName[0],
                                              StringLength,
                                              &BytesAppended);

            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            DataSize += BytesAppended;
            AvailableBytes -= BytesAppended;
            CurrentBufferPosition = Add2Ptr(CurrentBufferPosition, BytesAppended);
        }

        CurrentPathNode = NextDevicePathNode(CurrentPathNode);
    }

    //
    // The file path is not NULL terminated during the append operation.
    // Properly NULL terminate it now.
    //

    if (AvailableBytes < sizeof(UNICODE_NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    *CurrentBufferPosition = L'\0';
    DataSize += sizeof(UNICODE_NULL);

    //
    // In some scenarios the firmware incorrectly supplies a NULL file path.
    // Workaround the firmware issue by setting an empty file path and marking
    // the boot option as invalid. This will effectively prevent the option from
    // being used inside the boot application.
    //

    if (CurrentBufferPosition == StartingPosition) {
        Option->Invalid = TRUE;
    }

    Option->DataSize = DataSize;
    return STATUS_SUCCESS;
}

NTSTATUS
EfiInitpGetBootCurrentLoadOptions (
    __in struct _EFI_SYSTEM_TABLE *SystemTable,
    __deref_out_bcount(*LoadOptionsSize) PWCHAR *LoadOptions,
    __out PULONG LoadOptionsSize
    )

/*++

Routine Description:

    This function extracts the OptionalData field associated with the
    BootCurrent load option.  While the EFI spec dictates that this data must
    be passed to loaded images through the EFI loaded image protocol
    structure, some systems (e.g., HP Integrity servers) do not populate the
    loader image protocol structure correctly.  The old loader codebase
    unconditionally used BootCurrent to extract load options, so this routine
    is required in order to preserve backward compatibility on these machines.

    N.B. The boot library is not initialized, implying that this routine
         cannot use the boot library firmware subsystem and must instead call
         the firmware directly.

Arguments:

    SystemTable - Supplies a pointer to the EFI system table.

    LoadOptions - Supplies a pointer that receives a pointer to the
        OptionalData buffer associated with the BootCurrent load option.

    LoadOptionsSize - Supplies the size in bytes of the returned load options
        buffer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INSUFFICIENT_RESOURCES if the BootCurrent load option overflows the
        statically allocated buffer that this routine uses to operate upon it.

    The appropriate NT status code equivalent of the received EFI failure code
        otherwise.

--*/

{

    UINT32 Attributes;
    UINT16 BootCurrent;
    PEFI_LOAD_OPTION BootOption;
    WCHAR BootOptionName[9];
    UINTN DataSize;
    EFI_STATUS EfiStatus;
    UINTN OptionalDataOffset;
    int Result;
    NTSTATUS Status;
    UINTN StringSize;

    //
    // Read the BootCurrent environment variable.  The EFI spec requires this
    // variable to contain the 16 bit index of the boot option that was
    // selected on the current boot.
    //

    DataSize = sizeof(UINT16);
    BootCurrent = 0;
    EfiStatus = SystemTable->RuntimeServices->GetVariable(L"BootCurrent",
                                                          (EFI_GUID*)&EfiGlobalVariable,  // const_cast
                                                          &Attributes,
                                                          &DataSize,
                                                          &BootCurrent);

    Status = EfiGetNtStatusCode(EfiStatus);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Read the EFI_LOAD_OPTION associated with the current boot.  The boot
    // option index is appended to the string L"Boot" to obtain the name of
    // the current boot option.  The index is formatted in hexadecimal using
    // the uppercase versions of any alphabetical characters.
    //

    Result = swprintf_s(&BootOptionName[0],
                        RTL_NUMBER_OF(BootOptionName),
                        L"Boot%04X",
                        BootCurrent);

    ASSERT(Result == 8);

    DataSize = RTL_NUMBER_OF(EfiInitBootOptionBuffer);
    EfiStatus = SystemTable->RuntimeServices->GetVariable(
                    &BootOptionName[0],
                    (EFI_GUID*)&EfiGlobalVariable,  // const_cast
                    &Attributes,
                    &DataSize,
                    &EfiInitBootOptionBuffer[0]);

    Status = EfiGetNtStatusCode(EfiStatus);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The OptionalData component of the load option begins following a 32-bit
    // attributes field, a 16-bit file path length field, a NULL terminated
    // wide character string, and the actual file path whose length is
    // reflected in the 16-bit field.
    //

    BootOption = (PEFI_LOAD_OPTION)&EfiInitBootOptionBuffer[0];
    StringSize = (wcslen(&BootOption->Description[0]) + 1) * sizeof(WCHAR);
    OptionalDataOffset = sizeof(UINT32)
                         + sizeof(UINT16)
                         + StringSize
                         + BootOption->FilePathLength;

    ASSERT(OptionalDataOffset < DataSize);

    *LoadOptions = (PWCHAR)&EfiInitBootOptionBuffer[OptionalDataOffset];
    *LoadOptionsSize = (ULONG)(DataSize - OptionalDataOffset);
    return STATUS_SUCCESS;
}

EFI_DEVICE_PATH UNALIGNED *
EfiInitpGetDeviceNode (
    __in_xcount(polymorphism) EFI_DEVICE_PATH UNALIGNED *DevicePath
    )

/*++

Routine Description:

    This routine returns the last device path node before a file path node.

Arguments:

    DevicePath - Supplies an EFI device path.

Return Value:

    A pointer to the last device path node.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *CurrentPathNode;

    if (IsDevicePathEndType(DevicePath)) {
        return DevicePath;
    }

    //
    // Walk down the device path, have DevicePath lag by one node.
    //

    CurrentPathNode = NextDevicePathNode(DevicePath);
    while (!IsDevicePathEndType(CurrentPathNode)) {
        if ((CurrentPathNode->Type == MEDIA_DEVICE_PATH) &&
            (CurrentPathNode->SubType == MEDIA_FILEPATH_DP)) {

            break;
        }

        DevicePath = CurrentPathNode;
        CurrentPathNode = NextDevicePathNode(CurrentPathNode);
    }

    return DevicePath;
}

#if EFI_INIT_DEBUG

VOID
EfiDebugInitialize (
    __in struct _EFI_SYSTEM_TABLE *SystemTable
    )

/*++

Routine Description:

    This routine initializes global data structures used for EFI initialization
    debugging.

Arguments:

    SystemTable - Supplies a pointer to the EFI system table.

Returns:

    None.

--*/

{

    EfiConOut = SystemTable->ConOut;
    return;
}

VOID
EfiDebugPrint (
    __in PWSTR String
    )

/*++

Routine Description:

    This routine will print a string to the local console.

Arguments:

    String - Supplies a string to print to the local console.

Returns:

    None.

--*/

{

    EfiConOut->OutputString(EfiConOut, String);
    return;
}

VOID
EfiDebugPrintBaseAddress (
    __in EFI_LOADED_IMAGE *LoadedImageInterface
    )

/*++

Routine Description:

    This routine will print the base address of the application to the local
    console.

Arguments:

    LoadedImageInterface - Supplies a pointer to the loaded image interface
        for the boot application.

Returns:

    None.

--*/

{

    WCHAR DebugMessage[80];

#if _WIN64

    swprintf_s(DebugMessage,
               sizeof(DebugMessage) / sizeof(WCHAR),
               L"Image Base = %08x %08x\r\n",
               (ULONG)((ULONG_PTR)LoadedImageInterface->ImageBase >> 32),
               (ULONG)((ULONG_PTR)LoadedImageInterface->ImageBase));

#else

    swprintf_s(DebugMessage,
               sizeof(DebugMessage) / sizeof(WCHAR),
               L"Image Base = %08x\r\n",
               (ULONG)(ULONG_PTR)LoadedImageInterface->ImageBase);

#endif

    EfiDebugPrint(DebugMessage);
    return;
}

#endif



