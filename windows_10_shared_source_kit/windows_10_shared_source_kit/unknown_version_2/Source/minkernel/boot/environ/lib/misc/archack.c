/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    archack.c

Abstract:

    This module implements routines to translate Arc Paths to Boot Environment
    File Paths and vice versa.  Arc syntax will eventually go away, and this
    module can be removed at that time.

    Arc syntax needs to go away, since a device path can change based on
    firmware enumeration.  Therefore, there is no guarentee that an arc device
    path will be valid across boots.  Because of the need to kill this syntax,
    this module is called arc hack and exposed routines have the
    prefix Ah (arc hack).

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>

//
// ----------------------------------------------- Internal Function Prototypes
//

BOOLEAN
GetArcPathMnemonicKey (
    __in PSTR ArcPath,
    __in PSTR Mnemonic,
    __in PULONG Key,
    __in ULONG Base
    );

NTSTATUS
ArcConvertOptionToSubString (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in PBOOT_ENTRY_OPTION Option,
    __out_ecount_part_opt(BufferLength,*StringLength) PCHAR OptionString,
    __in ULONG BufferLength,
    __out PULONG StringLength
    );

NTSTATUS
ArcConvertStringIntegerToOption(
    __in PWSTR IntegerString,
    __in BCDE_DATA_TYPE Type,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes,
    __out_opt PULONG RequiredBytes
    );

NTSTATUS
ArcConvertStringToOption (
    __in PWSTR String,
    __in BOOLEAN WhitespaceTerminated,
    __in BCDE_DATA_TYPE Type,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes,
    __out_opt PULONG RequiredBytes
    );

NTSTATUS
ArcTranslatePartitionDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    );

NTSTATUS
ArcTranslateBlockIoDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    );

NTSTATUS
ArcTranslateUdpDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    );

VOID
BlpCopyAndSanitizeUnicodeToAnsiString (
    __out_ecount(Length) PCHAR AnsiString,
    __in_ecount(Length) PCWCHAR UnicodeString,
    __in ULONG Length
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
AhGetArcDevice (
    __in DEVICE_ID DeviceId,
    __inout PANSI_STRING ArcDevice,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    This routine will translate a boot environment device into and arc
    device.

    If the buffer (in ArcDevice) is not large enough and AllocateNewBuffer is
    TRUE, a new buffer will be allocated and returned to the caller.
    The caller is responsible for freeing the memory allocated in this routine.

Arguments:

    DeviceId - Handle to an open boot environment device.

    ArcDevice - Ansi string with the translated arc device name.

    AllocateNewBuffer - If TRUE and the specified buffer is too small, then
        this routine will allocate a new buffer large enough for the device
        structure.  If the buffer was too small and AllocateNewBuffer == FALSE,
        the required buffer size will be returned in ArcDevice.Length.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    STATUS_INVALID_PARAMETER if DeviceId is invalid or ArcDevice is NULL.
    STATUS_BUFFER_TOO_SMALL if the specified buffer is too small and
        AllocateNewBuffer is FALSE.

--*/

{

    ANSI_STRING ArcDeviceBuffer;
    BOOLEAN ArcDeviceStringAllocated;
    CHAR Buffer[128];
    DEVICE_INFORMATION Information;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceAh);

    if (ArcDevice == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto AhGetArcDeviceEnd;
    }

    //
    // Determine the device type.
    //

    Status = BlDeviceGetInformation(DeviceId, &Information);
    if (!NT_SUCCESS(Status)) {
        goto AhGetArcDeviceEnd;
    }

    ArcDeviceBuffer.Buffer = Buffer;
    ArcDeviceBuffer.Length = 0;
    ArcDeviceBuffer.MaximumLength = sizeof(Buffer);

    //
    // Fork code based on device type.
    //

    switch (Information.DeviceType) {
    case DEVICE_PARTITION:
    case DEVICE_PARTITION_EX:
        Status = ArcTranslatePartitionDevice(DeviceId,
                                             &Information,
                                             &ArcDeviceBuffer);

        break;

    case DEVICE_BLOCK_IO:
        Status = ArcTranslateBlockIoDevice(DeviceId,
                                           &Information,
                                           &ArcDeviceBuffer);

        break;

    case DEVICE_UDP:
        Status = ArcTranslateUdpDevice(DeviceId,
                                       &Information,
                                       &ArcDeviceBuffer);

        break;

    case DEVICE_VMBUS:

        //
        // For VMBUS Boot, the arc device names we store in the Loader Block
        // must be partitions. If we're attempting to resolve the arcname
        // for a VMBUS device, we're probably doing something wrong...
        //

        ASSERT(FALSE);
        Status = STATUS_NOT_SUPPORTED;
        break;

    default:
        Status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    if (!NT_SUCCESS(Status)) {
        goto AhGetArcDeviceEnd;
    }

    //
    // At this point, the arc device name is stored in the stack variable,
    // Buffer.  Copy it to the caller's buffer.
    //

    if (ArcDevice->MaximumLength < ArcDeviceBuffer.Length + 1) {
        if (AllocateNewBuffer == FALSE) {
            ArcDevice->Length = ArcDeviceBuffer.Length;
            Status = STATUS_BUFFER_TOO_SMALL;
            goto AhGetArcDeviceEnd;
        }

        //
        // Allocate a new buffer.
        //

        if (ArcDevice->MaximumLength != 0) {
            RtlFreeStringRoutine(ArcDevice->Buffer);
        }

        ArcDeviceStringAllocated = BlCreateAnsiString(ArcDevice, Buffer);
        if (ArcDeviceStringAllocated == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto AhGetArcDeviceEnd;
        }

    } else {
        RtlCopyMemory(ArcDevice->Buffer,
                      Buffer,
                      ArcDeviceBuffer.Length);

        ArcDevice->Buffer[ArcDeviceBuffer.Length] = '\0';
        ArcDevice->Length = ArcDeviceBuffer.Length;
    }

    Status = STATUS_SUCCESS;

AhGetArcDeviceEnd:

    BlpInterfaceExit(InterfaceAh);
    return Status;
}

NTSTATUS
AhGetArcPath (
    __in PUNALIGNED_FILE_PATH FilePath,
    __inout PANSI_STRING ArcPath,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    This routine will translate a boot environment file path into an arc path.

    If the buffer (in ArcPath) is not large enough and AllocateNewBuffer is
    TRUE, a new buffer will be allocated and returned to the caller.
    The caller is responsible for freeing the memory allocated in this routine.

Arguments:

    DeviceId - Handle to an open boot environment device.

    ArcDevice - Ansi string with the translated arc device name.

    AllocateNewBuffer - If TRUE and the specified buffer is too small, then
        this routine will allocate a new buffer large enough for the device
        structure.  If the buffer was too small and AllocateNewBuffer == FALSE,
        the required buffer size will be returned in ArcPath.Length.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    STATUS_INVALID_PARAMETER if FilePath is invalid or NULL or ArcPath is NULL.
    STATUS_BUFFER_TOO_SMALL if the specified buffer is too small and
        AllocateNewBuffer is FALSE.

--*/

{

    ANSI_STRING ArcDevice;
    PCHAR Dest;
    DEVICE_ID DeviceId;
    ULONG Length;
    PWCHAR Source;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceAh);
    DeviceId = INVALID_DEVICE_ID;

    if ((FilePath == NULL) || (ArcPath == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AhGetArcPathEnd;
    }

    //
    // Open the device referenced in FilePath.
    //

    Status = BlDeviceOpen(BlGetDevice(FilePath), OPEN_READ, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto AhGetArcPathEnd;
    }

    //
    // Obtain a string for the arc device.
    //

    RtlInitAnsiString(&ArcDevice, NULL);
    Status = AhGetArcDevice(DeviceId, &ArcDevice, TRUE);
    if (!NT_SUCCESS(Status)) {
        goto AhGetArcPathEnd;
    }

    //
    // Copy the full arc path to the caller's buffer.
    // N.B. The file path immediately follows the device identifier.
    //

    Source = BlGetFilePath(FilePath);
    Length = (ULONG)wcslen(Source);
    Status = RtlULongAdd(Length, 1, &Length); // Count '\0'.
    if (!NT_SUCCESS(Status)) {
        goto AhGetArcPathEnd;
    }

    if (ArcPath->MaximumLength < ArcDevice.Length + Length) {
        if (AllocateNewBuffer == FALSE) {
            ArcPath->Length = (USHORT)(ArcDevice.Length + Length);
            BlMmFreeHeap(ArcDevice.Buffer);
            Status = STATUS_BUFFER_TOO_SMALL;
            goto AhGetArcPathEnd;
        }

        //
        // Allocate a new buffer.
        //

        if (ArcPath->MaximumLength != 0) {
            BlMmFreeHeap(ArcPath->Buffer);
        }

        ArcPath->Buffer = BlMmAllocateHeap(ArcDevice.Length + Length);
        if (ArcPath->Buffer == NULL) {
            BlMmFreeHeap(ArcDevice.Buffer);
            Status = STATUS_NO_MEMORY;
            goto AhGetArcPathEnd;
        }

        ArcPath->MaximumLength = (USHORT)(ArcDevice.Length + Length);
    }

    //
    // Copy the arc device name.
    //

    RtlCopyMemory(ArcPath->Buffer, ArcDevice.Buffer, ArcDevice.Length);
    BlMmFreeHeap(ArcDevice.Buffer);
    Dest = Add2Ptr(ArcPath->Buffer, ArcDevice.Length);
    ArcPath->Length = (USHORT)(ArcDevice.Length + Length - 1);  // Remove '\0'.

    //
    // Copy the file path.  This will copy the null character.
    //

    for ( ; (Length != 0); Dest += 1, Source += 1, Length -= 1) {
        *Dest = (CHAR)*Source;
    }

    Status = STATUS_SUCCESS;

AhGetArcPathEnd:

    BlpInterfaceExit(InterfaceAh);
    return Status;
}

NTSTATUS
AhGetBootEnvironmentDevice (
    __in PSTR ArcDevice,
    __in_opt PWSTR Options,
    __inout PGENERIC_BUFFER IdentifierBuffer,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    This routine will translate an ArcDevice string into a Boot Environment
    Device Structure.

    The resulting structure will be copied to IdentifierBuffer.Buffer.  If
    the buffer is not large enough and AllocateNewBuffer is TRUE, a new buffer
    will be allocated and returned to the caller.  The caller is responsible
    for freeing the memory allocated in this routine.

Arguments:

    ArcDevice - Arc device to translate.

    Options - An optional string containing options substrings that describe
        how to interpret the arc device.

    IdentifierBuffer - The translated boot environment device.

    AllocateNewBuffer - If TRUE and the specified buffer is too small, then
        this routine will allocate a new buffer large enough for the device
        structure.  If the buffer was too small and AllocateNewBuffer == FALSE,
        the required buffer size will be returned in IdentifierBuffer.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    STATUS_NO_SUCH_DEVICE if ArcDevice is invalid.
    STATUS_INVALID_PARAMETER if ArcDevice is NULL.
    STATUS_BUFFER_TOO_SMALL if the specified buffer is too small and
        AllocateNewBuffer is FALSE.

--*/

{

    PCHAR ArcPath;
    UINTN ArcPathLength;
    PBLOCK_IO_IDENTIFIER BlockIoDevice;
    WCHAR Character;
    BOOT_ENVIRONMENT_DEVICE Device;
    DEVICE_ID DeviceId;
    GENERIC_BUFFER DeviceIdArray;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    BOOT_ENVIRONMENT_DEVICE EnumDevice;
    DEVICE_ID FileDeviceId;
    BOOLEAN IdentifierAllocated;
    ULONG Key;
    PCHAR NarrowString;
    ULONG Partition;
    PPXE_INFO PxeInfo;
    PFILE_PATH RdFilePath;
    PWCHAR RdPath;
    GENERIC_BUFFER RdPathBuffer;
    ULONG Size;
    NTSTATUS Status;
    PBOOT_ENVIRONMENT_DEVICE TranslatedDevice;
    PWCHAR WimPath;

    BlpInterfaceEnter(InterfaceAh);

    //
    // Initialize local variables.
    //

    ArcPath = NULL;
    DeviceId = INVALID_DEVICE_ID;
    FileDeviceId = INVALID_DEVICE_ID;
    IdentifierAllocated = FALSE;
    BlInitGenericBuffer(&DeviceIdArray, NULL, 0);
    BlInitGenericBuffer(&RdPathBuffer, NULL, 0);

    //
    // Use a local stack copy of a boot environment device when translating
    // ArcDevice.  That way, a buffer is not allocated for the caller when
    // ArcDevice is invalid.
    //

    RtlZeroMemory(&Device, sizeof(BOOT_ENVIRONMENT_DEVICE));
    Device.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    TranslatedDevice = &Device;

    //
    // Validate parameters.
    //

    if ((ArcDevice == NULL) || (IdentifierBuffer == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AhGetBootEnvironmentDeviceEnd;
    }

    //
    // Allocate a new string buffer we can modify, and copy in the
    // ArcDevice.
    //

    ArcPathLength = strlen(ArcDevice) + 1;
    ArcPath = BlMmAllocateHeap(ArcPathLength);
    if (ArcPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AhGetBootEnvironmentDeviceEnd;
    }

    strcpy_s(ArcPath, ArcPathLength, ArcDevice);

    //
    // Convert ArcPath to lowercase, making searches easier.
    //

    _strlwr(ArcPath);

    //
    // ARC device paths have the following syntax:
    //
    // <path> ::= [<adapter>]*<controller><peripheral>[<partition>] where
    //
    // <adapter>    ::= <adapter mnemonic(<key>)
    // <controller> ::= <controller mnemonic>(<key>)
    // <peripheral> ::= <peripheral mnemonic>(<key>)
    // <partition>  ::= <partition mnemonic>(<key>)
    //
    // valid adapter mnemonics : eisa, multi, scsi, ramdisk, net
    // valid controller mnemonics : cdrom, disk
    // valid peripheral mnenomics : rdisk, fdisk
    //

    //
    // A multi path will refer to the int13 drive number with the rdisk key.
    // A signature path refers to the MBR disk signature with the signature
    // key.
    //

    //
    // Fork based on controller.
    //

    if (GetArcPathMnemonicKey(ArcPath, "disk", &Key, 10)) {

        //
        // Fork based on peripheral mnemonic.
        //

        if (GetArcPathMnemonicKey(ArcPath, "signature", &Key, 16)) {

            //
            // ARC path used signature syntax.  This must be a hard disk or a
            // partition on a hard disk.
            //
            // Attempt to find the partition on the disk that the OS
            // installation lies.
            //

            if (GetArcPathMnemonicKey(ArcPath, "partition", &Partition, 10)) {
                Device.DeviceType = DEVICE_PARTITION;
                Device.u.Partition.Mbr.PartitionNumber = Partition;
                BlockIoDevice = &Device.u.Partition.ParentIdentifier;

            } else {
                Device.DeviceType = DEVICE_BLOCK_IO;
                BlockIoDevice = &Device.u.BlockIo;
            }

            BlockIoDevice->BlockIoType = BLOCK_IO_HARD_DISK;
            BlockIoDevice->u.HardDisk.PartitionType = MBR;    // Assumption.
            BlockIoDevice->u.HardDisk.Mbr.Signature = Key;

        } else if (GetArcPathMnemonicKey(ArcPath, "rdisk", &Key, 10)) {

            //
            // Hard disk drive.
            //

            //
            // Proceed like before (signature case).  Only, this time we have
            // to determine the signature.
            //

            //
            // In order to determine the signature, enumerate all firmware
            // disks.  Arc disk numbers are assigned based on enumeration
            // order.  Match the above key with that associated device id
            // array.
            //

            EnumDevice.DeviceType = DEVICE_BLOCK_IO;
            EnumDevice.u.BlockIo.BlockIoType = BLOCK_IO_HARD_DISK;
            Status = BlDeviceEnumerateDeviceClass(&EnumDevice,
                                                  &DeviceIdArray,
                                                  TRUE);  // Allocate Array.

            if (!NT_SUCCESS(Status)) {
                goto AhGetBootEnvironmentDeviceEnd;
            }

            //
            // Ensure that the device exists.
            //

            if (((Key + 1) * sizeof(DEVICE_ID)) > DeviceIdArray.DataSize) {
                Status = STATUS_NO_SUCH_DEVICE;
                goto AhGetBootEnvironmentDeviceEnd;
            }

            //
            // Obtain the device identifier for the disk in question.
            //

            Status = BlGetDeviceIdentifier(
                                ((PDEVICE_ID)DeviceIdArray.Buffer)[Key],
                                &DeviceIdentifier);

            ASSERT(Status == STATUS_SUCCESS);

            //
            // Determine if this is a partition or just a HDD.  If this is
            // a partition, the disk identifier lives in the parent
            // identifier.
            //

            if (GetArcPathMnemonicKey(ArcPath, "partition", &Partition, 10)) {
                Device.DeviceType = DEVICE_PARTITION;
                Device.u.Partition.Mbr.PartitionNumber = Partition;
                BlockIoDevice = &Device.u.Partition.ParentIdentifier;

            } else {
                Device.DeviceType = DEVICE_BLOCK_IO;
                BlockIoDevice = &Device.u.BlockIo;
            }

            //
            // Copy the HDD identifier.
            //

            BlockIoDevice->BlockIoType = BLOCK_IO_HARD_DISK;
            RtlCopyMemory(&BlockIoDevice->u.HardDisk,
                          &DeviceIdentifier->u.BlockIo.u.HardDisk,
                          sizeof(HARD_DISK_IDENTIFIER));

        } else if (GetArcPathMnemonicKey(ArcPath, "fdisk", &Key, 10)) {

            //
            // Floppy disk drive.
            //

            Device.DeviceType = DEVICE_BLOCK_IO;
            Device.u.BlockIo.BlockIoType = BLOCK_IO_REMOVABLE_DISK;
            Device.u.BlockIo.u.RemovableDisk.DriveNumber = Key;

        } else if (GetArcPathMnemonicKey(ArcPath, "cdrom", &Key, 10)) {

            //
            // CDROMs can be designated by either
            // multi(0)cdrom(%u) or multi(0)disk(0)cdrom(%u)
            //

            Device.DeviceType = DEVICE_BLOCK_IO;
            BlockIoDevice = &Device.u.BlockIo;
            BlockIoDevice->BlockIoType = BLOCK_IO_CDROM;
            BlockIoDevice->u.Cdrom.CdromNumber = Key;

        } else {
            Status = STATUS_INVALID_PARAMETER;
            goto AhGetBootEnvironmentDeviceEnd;
        }

    } else if (GetArcPathMnemonicKey(ArcPath, "ramdisk", &Key, 10)) {

        //
        // RAM disk device.  In the case of a ramdisk, the device being referred
        // to by an arc path is dictated by the source of the ramdisk - the
        // pneumonic is not relevant.  Return a boot environment device that
        // contains this source path.  This identifier is not fully specified
        // since it does not contain the physical memory location of the
        // ramdisk - the device must be created for this to be known.
        //

        RdFilePath = NULL;
        RdPath = wcsstr(Options, L"/RDPATH");
        if (RdPath != NULL) {
            RdPath = Add2Ptr(RdPath, sizeof(L"/RDPATH"));

            //
            // The RDPATH option contains the ARC path of the source path.
            // Convert this to a boot environment file path.  The string must
            // be converted to a single-byte character string for consumption
            // by the ARC translation routines.
            //

            Size = 0;
            while ((RdPath[Size] != L' ') && (RdPath[Size] != L'\0')) {
                Size += 1;
            }

            NarrowString = BlMmAllocateHeap((Size + 1) * sizeof(CHAR));
            if (NarrowString == NULL) {
                Status = STATUS_NO_MEMORY;
                goto AhGetBootEnvironmentDeviceEnd;
            }

            Character = RdPath[Size];
            RdPath[Size] = UNICODE_NULL;
            BlCopyWcharStringToString(NarrowString, RdPath);
            RdPath[Size] = Character;
            Status = AhGetFilePath(NarrowString, NULL, &RdPathBuffer, TRUE);
            if (!NT_SUCCESS(Status)) {
                goto AhGetBootEnvironmentDeviceEnd;
            }

            //
            // Allocate a buffer large enough to hold the entire identifier,
            // including source path.
            //

            RdFilePath = RdPathBuffer.Buffer;
            Size = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                u.BlockIo.u.RamDisk.Source.SourcePath);

            Size += RdFilePath->Length;

            TranslatedDevice = BlMmAllocateHeap(Size);
            if (TranslatedDevice == NULL) {
                Status = STATUS_NO_MEMORY;
                goto AhGetBootEnvironmentDeviceEnd;
            }

            RtlZeroMemory(TranslatedDevice, Size);
            IdentifierAllocated = TRUE;

        } else {
            TranslatedDevice = &Device;
            Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
        }

        //
        // Fill in the identifier.
        //

        TranslatedDevice->Size = Size;
        TranslatedDevice->DeviceType = DEVICE_BLOCK_IO;
        TranslatedDevice->Flags = DEVICE_FLAG_IDENTIFIER_UNSPECIFIED;
        BlockIoDevice = &TranslatedDevice->u.BlockIo;
        BlockIoDevice->BlockIoType = BLOCK_IO_RAMDISK;
        if (RdFilePath != NULL) {
            RtlCopyMemory(&BlockIoDevice->u.RamDisk.Source.SourcePath,
                          RdFilePath,
                          RdFilePath->Length);
        }

    } else if (GetArcPathMnemonicKey(ArcPath, "net", &Key, 10)) {

        //
        // Net device.
        //

        Device.DeviceType = DEVICE_UDP;

        //
        // Key 0 specifies the NIC from which boot originated.  The firmware
        // enumeration order can be used to map other keys to physical
        // devices.
        //

        if (Key == 0) {

            //
            // Get the PXE_INFO data. If we are unable to get it, we are not
            // booting from net. If so, net(0) is invalid.
            //

            PxeInfo = BlNetGetPxeInfo();
            if (PxeInfo == NULL) {
                return STATUS_NO_SUCH_DEVICE;
            }

            //
            // Initialize the UDP device identifier.
            //

            Device.u.Udp.HwType = PxeInfo->HardwareType;
            RtlCopyMemory(Device.u.Udp.HwAddress,
                          PxeInfo->HardwareAddress,
                          sizeof(PxeInfo->HardwareAddress));

        } else {

            //
            // TODO: Add the firmware enumeration routine for NIC, and add
            //       utility library function to translate the enumeration
            //       number to MAC address. For now, simple return error.
            //

            return STATUS_NO_SUCH_DEVICE;
        }

    } else if (GetArcPathMnemonicKey(ArcPath, "cdrom", &Key, 10)) {

        //
        // CDROMs can be designated by either
        // multi(0)cdrom(%u) or multi(0)disk(0)cdrom(%u)
        //

        Device.DeviceType = DEVICE_BLOCK_IO;
        BlockIoDevice = &Device.u.BlockIo;
        BlockIoDevice->BlockIoType = BLOCK_IO_CDROM;
        BlockIoDevice->u.Cdrom.CdromNumber = Key;

    } else {
        Status = STATUS_INVALID_PARAMETER;
        goto AhGetBootEnvironmentDeviceEnd;
    }

    //
    // Handle the /WIMSOURCE boot option, which specifies that it is really
    // a file on the device that is being represented by the arc path.
    //

    WimPath = wcsstr(Options, L"/WIMSOURCE");
    if (WimPath != NULL) {

        //
        // The option exists.  Step over the option string and an equals sign.
        // Note that since the string size contains a NULL terminator, this
        // will step over the equals sign in addition to the string.
        //

        WimPath = Add2Ptr(WimPath, sizeof(L"/WIMSOURCE"));

        //
        // Mount the WIM.
        //

        Status = BlDeviceOpen(TranslatedDevice, OPEN_READ, &DeviceId);
        if (!NT_SUCCESS(Status)) {
            goto AhGetBootEnvironmentDeviceEnd;
        }

        Status = BlFileMount(DeviceId, WimPath, &FileDeviceId);
        if (!NT_SUCCESS(Status)) {
            goto AhGetBootEnvironmentDeviceEnd;
        }

        Status = BlGetDeviceIdentifier(FileDeviceId, &DeviceIdentifier);
        if (!NT_SUCCESS(Status)) {
            goto AhGetBootEnvironmentDeviceEnd;
        }

        if (IdentifierAllocated != FALSE) {
            BlMmFreeHeap(TranslatedDevice);
        }

        TranslatedDevice = DeviceIdentifier;
        IdentifierAllocated = FALSE;
    }

    //
    // At this point, we have successfully translated the arc device into
    // a boot environment device structure.  Copy it to the caller's
    // structure.  If required, allocate a new buffer.
    //

    if (IdentifierBuffer->BufferSize < TranslatedDevice->Size) {

        //
        // If the caller did not request a new buffer to be allocated, then
        // return STATUS_BUFFER_TOO_SMALL.
        //

        if (AllocateNewBuffer == FALSE) {
            IdentifierBuffer->BufferSize = TranslatedDevice->Size;
            Status = STATUS_BUFFER_TOO_SMALL;
            goto AhGetBootEnvironmentDeviceEnd;
        }

        //
        // Allocate a larger buffer.
        //

        if (IdentifierBuffer->BufferSize != 0) {
            BlMmFreeHeap(IdentifierBuffer->Buffer);
        }

        IdentifierBuffer->BufferSize = TranslatedDevice->Size;
        IdentifierBuffer->Buffer = BlMmAllocateHeap(TranslatedDevice->Size);
        if (IdentifierBuffer->Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto AhGetBootEnvironmentDeviceEnd;
        }
    }

    IdentifierBuffer->DataSize = TranslatedDevice->Size;
    RtlCopyMemory(IdentifierBuffer->Buffer,
                  TranslatedDevice,
                  TranslatedDevice->Size);

    Status = STATUS_SUCCESS;

AhGetBootEnvironmentDeviceEnd:

    if (FileDeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(FileDeviceId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    if (DeviceIdArray.Buffer != NULL) {
        BlMmFreeHeap(DeviceIdArray.Buffer);
    }

    if (ArcPath != NULL) {
        BlMmFreeHeap(ArcPath);
    }

    if (IdentifierAllocated != FALSE) {
        BlMmFreeHeap(TranslatedDevice);
    }

    if (RdPathBuffer.Buffer != NULL) {
        BlMmFreeHeap(RdPathBuffer.Buffer);
    }

    BlpInterfaceExit(InterfaceAh);
    return Status;
}

NTSTATUS
AhGetFilePath (
    __in PSTR ArcPath,
    __in_opt PWSTR Options,
    __inout PGENERIC_BUFFER FilePathBuffer,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    This routine will translate an Arc Path into a File Path (with file path
    type FILE_PATH_TYPE_BOOT_ENV).  This is the file path type used by
    the boot environment.

    A boot environment file path consists of a Boot Environment Device plus
    the file path off the device.

    The resulting structure will be copied to FilePathBuffer.Buffer.  If
    the buffer is not large enough and AllocateNewBuffer is TRUE, a new buffer
    will be allocated and returned to the caller.  The caller is responsible
    for freeing the memory allocated in this routine.

Arguments:

    ArcPath - Arc path to translate.

    Options - An optional string containing a series of space-separated options
        substrings that describe how to interpret the arc device.

    FilePathBuffer - Contains the translated file path.

    AllocateNewBuffer - If TRUE and the file path buffer is too small, the
        routine will allocate a new buffer that can fit the entire file path.
        If the buffer was too small and AllocateNewBuffer == FALSE, the
        required buffer size will be returned in IdentifierBuffer.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    STATUS_NO_SUCH_DEVICE if the specified device is invalid.
    STATUS_INVALID_PARAMETER if ArcPath is NULL or malformed.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE Device;
    PBOOT_ENVIRONMENT_DEVICE DeviceId;
    PCHAR FilePath;
    PUNALIGNED_FILE_PATH FilePathStructure;
    GENERIC_BUFFER IdentifierBuffer;
    ULONG RequiredSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceAh);

    BlInitGenericBuffer(&IdentifierBuffer, NULL, 0);

    //
    // Validate parameters.
    //

    if ((ArcPath == NULL) || (FilePathBuffer == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AhGetFilePathEnd;
    }

    //
    // Get a pointer to the file path.
    //

    FilePath = strchr(ArcPath, '\\');
    if (FilePath == NULL) {
        Status = STATUS_INVALID_PARAMETER;   // Malformed arc path.
        goto AhGetFilePathEnd;
    }

    //
    // Translate the arc device into a boot environment device.
    //

    Status = AhGetBootEnvironmentDevice(ArcPath,
                                        Options,
                                        &IdentifierBuffer,
                                        TRUE);

    if (!NT_SUCCESS(Status)) {
        goto AhGetFilePathEnd;
    }

    //
    // At this point, we have translated the device and we have a pointer to
    // the file path.  We can create the file path structure.
    //

    RequiredSize = IdentifierBuffer.DataSize +                     // Device.
                   ((ULONG)strlen(FilePath) + 1) * sizeof(WCHAR);  // Filepath.

    RequiredSize += FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath);

    //
    // Check to see if the user's buffer is large enough for the entire
    // file path structure.
    //

    if (FilePathBuffer->BufferSize < RequiredSize) {

        //
        // If the caller did not request a new buffer to be allocated, then
        // return STATUS_BUFFER_TOO_SMALL.
        //

        if (AllocateNewBuffer == FALSE) {
            FilePathBuffer->BufferSize = RequiredSize;
            Status = STATUS_BUFFER_TOO_SMALL;
            goto AhGetFilePathEnd;
        }

        //
        // Allocate a new buffer.
        //

        if (FilePathBuffer->BufferSize != 0) {
            BlMmFreeHeap(FilePathBuffer->Buffer);
        }

        FilePathBuffer->BufferSize = RequiredSize;
        FilePathBuffer->Buffer = BlMmAllocateHeap(RequiredSize);
        if (FilePathBuffer->Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto AhGetFilePathEnd;
        }
    }

    FilePathBuffer->DataSize = RequiredSize;
    RtlZeroMemory(FilePathBuffer->Buffer, FilePathBuffer->DataSize);

    //
    // Initialize the file path structure.
    //

    FilePathStructure = (PUNALIGNED_FILE_PATH)FilePathBuffer->Buffer;
    FilePathStructure->Version = FILE_PATH_VERSION;
    FilePathStructure->Type    = FILE_PATH_TYPE_BOOT_ENV;
    FilePathStructure->Length  = RequiredSize;

    //
    // Copy the device identifier and file path.
    //

    DeviceId = BlGetDevice(FilePathStructure);
    if (DeviceId == NULL) {
        Status = STATUS_NO_SUCH_DEVICE;

    } else {
        Device = (PBOOT_ENVIRONMENT_DEVICE)IdentifierBuffer.Buffer;
        RtlCopyMemory(DeviceId, Device, Device->Size);
        BlCopyStringToWcharString(BlGetFilePath(FilePathStructure), FilePath);
        Status = STATUS_SUCCESS;
    }

AhGetFilePathEnd:
    if (IdentifierBuffer.Buffer != NULL) {
        BlMmFreeHeap(IdentifierBuffer.Buffer);
    }

    BlpInterfaceExit(InterfaceAh);
    return Status;
}

NTSTATUS
AhCreateLoadOptionsString (
    __in_opt PBOOT_ENTRY_OPTION Options,
    __deref_opt_out PSTR *LoadOptionsOut
    )

/*++

Routine Description:

    This routine creates a load options string corresponding to the optional
    parameters for the operating system being loaded.  The string is constructed
    in the format specified in boot.ini on prior versions of Windows.  Each
    option string is delimited by a space, and leading slashes are not inserted
    before the options.

Arguments:

    Options - Supplies the list of options that are to be converted into a load
        options string.

    LoadOptionsOut - Supplies a reference to the location in which a pointer to
        the load options string is returned.  The caller is responsible for
        freeing the memory.

Return Value:

    STATUS_SUCCESS if the options string is created successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    PBOOT_ENTRY_OPTION AssociatedOptions;
    PSTR AssociatedOptionsString;
    PBOOT_ENTRY_OPTION CurrentOption;
    DEVICE_INFORMATION DeviceInformation;
    ULONG Length;
    PCHAR LoadOptions;
    ULONG NextOffset;
    PBOOT_ENVIRONMENT_DEVICE OsDevice;
    DEVICE_ID OsDeviceId;
    BOOLEAN RamDiskBoot;
    PDEVICE_RAMDISK_INFORMATION RamDiskInformation;
    ANSI_STRING RamDiskSourceArcPath = {0};
    ULONG RemainingLength;
    ULONGLONG RamdiskLength;
    CHAR RamdiskLengthString[sizeof("1234567812345678")];
    ULONG RamdiskLengthStringLength;
    ULONGLONG RamdiskOffset;
    CHAR RamdiskOffsetString[sizeof("1234567812345678")];
    ULONG RamdiskOffsetStringLength;
    NTSTATUS Status;
    ULONG StringLength;
    PCHAR SubString;
    ULONG TotalLength;

    AssociatedOptionsString = NULL;
    *LoadOptionsOut = NULL;
    LoadOptions = NULL;
    OsDeviceId = INVALID_DEVICE_ID;
    if (Options == NULL) {
        Status = STATUS_SUCCESS;
        goto CreateLoadOptionsStringEnd;
    }

    //
    // Walk the options structure, determining the size required to convert
    // each to a string.
    //

    TotalLength = 0;
    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(Options, NextOffset);
        if (CurrentOption->Invalid == FALSE) {
            Status = ArcConvertOptionToSubString(Options,
                                                 CurrentOption,
                                                 NULL,
                                                 0,
                                                 &StringLength);

            if (!NT_SUCCESS(Status) && (Status != STATUS_BUFFER_TOO_SMALL)) {
                goto CreateLoadOptionsStringEnd;
            }

            //
            // If this option corresponds to a string, add a space at the end
            // to delimit the substrings.
            //

            if (StringLength != 0) {
                StringLength += 1;
            }

            TotalLength += StringLength;
        }

        NextOffset = CurrentOption->NextOptionOffset;
    } while (NextOffset != 0);

    //
    // Get ramdisk options out of BlDeviceGetInformation if OS device is a
    // ramdisk.  Currently, a ramdisk must have an SDI header on it to be
    // processed by the OS as a valid filesystem.  However, the ramdisk driver
    // is not capable of walking the header to determine the real start of the
    // ramdisk.  These offset and length options must therefore be appended to
    // the OS load options string in order to convey this information to the
    // ramdisk driver.
    //

    Status = BlGetBootOptionDevice(Options,
                                   BCDE_OSLOADER_TYPE_OS_DEVICE,
                                   &OsDevice,
                                   &AssociatedOptions);

    if (!NT_SUCCESS(Status)) {
        OsDevice = NULL;
        AssociatedOptions = NULL;
    }

    RamDiskBoot = FALSE;
    RamDiskInformation = NULL;
    RamdiskOffsetStringLength = 0;
    RamdiskLengthStringLength = 0;
    if ((OsDevice != NULL) &&
        (OsDevice->DeviceType == DEVICE_BLOCK_IO) &&
        (OsDevice->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK)) {

        RamDiskBoot = TRUE;
        Status = BlDeviceOpen(OsDevice, OPEN_READ, &OsDeviceId);
        if (!NT_SUCCESS(Status)) {
            goto CreateLoadOptionsStringEnd;
        }

        Status = BlDeviceGetInformation(OsDeviceId, &DeviceInformation);
        if (!NT_SUCCESS(Status)) {
            goto CreateLoadOptionsStringEnd;
        }

        BlDeviceClose(OsDeviceId);
        OsDeviceId = INVALID_DEVICE_ID;

        RamDiskInformation =
            &DeviceInformation.u.BlockIo.DeviceInformation.RamDisk;

        //
        // Add space for the RDIMAGEOFFSET and RDIMAGELENGTH options.  Note that
        // these computations take the NULL terminator of the option string into
        // account.  The space allocated for this character will actually be
        // used to hold a sub-string delimiting space character.
        //

        if (RamDiskInformation->ImageType == RAMDISK_IMAGE_TYPE_RAW_DISK) {
            RamdiskOffset = RamDiskInformation->RawDisk.ImageOffset;
            RamdiskLength = RamDiskInformation->RawDisk.ImageLength;

        } else if (RamDiskInformation->ImageType ==
                   RAMDISK_IMAGE_TYPE_SDI_PART) {

            RamdiskOffset = RamDiskInformation->SdiPart.ImageOffset;
            RamdiskLength = RamDiskInformation->SdiPart.ImageLength;

        } else {
            Status = STATUS_INVALID_PARAMETER;
            goto CreateLoadOptionsStringEnd;
        }

        _i64toa_s(RamdiskOffset,
                  RamdiskOffsetString,
                  sizeof(RamdiskOffsetString),
                  10);

        RamdiskOffsetStringLength = (ULONG)strlen(RamdiskOffsetString);
        TotalLength += sizeof("RDIMAGEOFFSET=");
        TotalLength += RamdiskOffsetStringLength;
        _i64toa_s(RamdiskLength,
                  RamdiskLengthString,
                  sizeof(RamdiskLengthString),
                  10);

        RamdiskLengthStringLength = (ULONG)strlen(RamdiskLengthString);
        TotalLength += sizeof("RDIMAGELENGTH=");
        TotalLength += RamdiskLengthStringLength;

        //
        // calculate the space required to hold the source path string.
        //

        if (OsDevice->u.BlockIo.u.RamDisk.Source.SourcePath.Length > 0) {
            Status = AhGetArcPath(&OsDevice->u.BlockIo.u.RamDisk.Source.SourcePath,
                                  &RamDiskSourceArcPath,
                                  TRUE);
        }

        if (!NT_SUCCESS(Status)) {
            goto CreateLoadOptionsStringEnd;
        }

        TotalLength += sizeof("RDPATH=");
        TotalLength += RamDiskSourceArcPath.Length;
    }

    //
    // Recall this function with the associated options to convert it into a
    // load options string.
    //

    if (AssociatedOptions != NULL) {
        Status = AhCreateLoadOptionsString(AssociatedOptions,
                                           &AssociatedOptionsString);

        if (!NT_SUCCESS(Status)) {
            goto CreateLoadOptionsStringEnd;
        }

        if (AssociatedOptionsString != NULL) {
            TotalLength += (ULONG)strlen(AssociatedOptionsString) + 1;
            TotalLength += 1;
        }
    }

    if (TotalLength == 0) {
        Status = STATUS_SUCCESS;
        goto CreateLoadOptionsStringEnd;
    }

    //
    // Allocate a buffer to hold the options string.
    //

    LoadOptions = BlMmAllocateHeap(TotalLength);
    if (LoadOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateLoadOptionsStringEnd;
    }

    //
    // Walk the options again, filling in the buffer.  The converted sub-string
    // will not be terminated, and the string length returned does not include
    // any terminator.
    //

    RemainingLength = TotalLength;
    SubString = LoadOptions;
    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(Options, NextOffset);
        if (CurrentOption->Invalid == FALSE) {
            Status = ArcConvertOptionToSubString(Options,
                                                 CurrentOption,
                                                 SubString,
                                                 RemainingLength,
                                                 &StringLength);

            if (!NT_SUCCESS(Status)) {
                goto CreateLoadOptionsStringEnd;
            }

            SubString += StringLength;

            //
            // If this option was converted to a string, add a space to
            // delimit.
            //

            if (StringLength != 0) {
                if (StringLength == RemainingLength) {
                    Status = STATUS_BUFFER_TOO_SMALL;
                    goto CreateLoadOptionsStringEnd;
                }

                *SubString = ' ';
                SubString += 1;
                StringLength += 1;
            }

            RemainingLength -= StringLength;
        }

        NextOffset = CurrentOption->NextOptionOffset;
    } while (NextOffset != 0);

    //
    // Add the ramdisk option strings if required, including the space
    // delimiters.
    //

    if (RamDiskBoot != FALSE) {

        //
        // First the offset string.
        //

        Length = sizeof("RDIMAGEOFFSET=") - 1;
        RtlCopyMemory(SubString, "RDIMAGEOFFSET=", Length);
        SubString += Length;
        RtlCopyMemory(SubString,
                      RamdiskOffsetString,
                      RamdiskOffsetStringLength);

        SubString += RamdiskOffsetStringLength;
        *SubString = ' ';
        SubString += 1;

        //
        // Now the length string.
        //

        Length = sizeof("RDIMAGELENGTH=") - 1;
        RtlCopyMemory(SubString, "RDIMAGELENGTH=", Length);
        SubString += Length;
        RtlCopyMemory(SubString,
                      RamdiskLengthString,
                      RamdiskLengthStringLength);

        SubString += RamdiskLengthStringLength;
        *SubString = ' ';
        SubString += 1;

        //
        // Now the ramdisk source path.
        //

        Length = sizeof("RDPATH=") - 1;
        RtlCopyMemory(SubString, "RDPATH=", Length);
        SubString += Length;
        RtlCopyMemory(SubString,
                      RamDiskSourceArcPath.Buffer,
                      RamDiskSourceArcPath.Length);

        SubString += RamDiskSourceArcPath.Length;
        *SubString = ' ';
        SubString += 1;
    }

    if (AssociatedOptionsString != NULL) {
        Length = (ULONG)strlen(AssociatedOptionsString);
        RtlCopyMemory(SubString, AssociatedOptionsString, Length);
        SubString += Length;
        *SubString = ' ';
        SubString += 1;
    }

    ASSERT(PtrOffset(LoadOptions, SubString) <= TotalLength);

    //
    // Convert the space added after the final option to a terminating NULL.
    //

    SubString -= 1;

    ASSERT(*SubString == L' ');

    *SubString = UNICODE_NULL;

    //
    // Return the string to the caller.
    //

    *LoadOptionsOut = LoadOptions;
    Status = STATUS_SUCCESS;

CreateLoadOptionsStringEnd:
    if (!NT_SUCCESS(Status)) {
        if (LoadOptions != NULL) {
            BlMmFreeHeap(LoadOptions);
        }
    }

    if (OsDeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(OsDeviceId);
    }

    if (RamDiskSourceArcPath.Buffer != NULL) {
        BlMmFreeHeap(RamDiskSourceArcPath.Buffer);
    }

    if (AssociatedOptionsString != NULL) {
        BlMmFreeHeap(AssociatedOptionsString);
    }

    return Status;
}

NTSTATUS
AhCreateLoadOptionsList (
    __inout PWSTR OptionString,
    __inout_bcount_opt(OptionListSize + *Offset)
        PBOOT_ENTRY_OPTION OptionListBase,
    __in ULONG OptionListSize,
    __inout_opt PULONG Offset,
    __inout_opt PBOOT_ENTRY_OPTION *PreviousOption,
    __out PULONG RequiredSize
    )

/*++

Routine Description:

    This routine converts an OS load options string to the corresponding boot
    option list.  Only those options sub-strings that must be processed by the
    boot environment are converted.  So that the remaining sub-strings can be
    passed into the OS, the entire options string is stored in a string-format
    boot option.

    N.B.  This routine may not access any library allocation functions, as it
          may run prior to the library memory subsystem being initialized.

Arguments:

    OptionString - Supplies the options string to convert.

    OptionListBase - Supplies a pointer to the base of a the option list
        structure.

    Offset - Supplies an optional pointer to a variable that specifies the
        option list offset.  On input, this variable specifies the current
        offset in the option list.  On output, the variable contains the
        updated offset.

    PreviousOption - Supplies an optional pointer to a variable that describes
        the boot option preceeding the current offset.

    OptionListSize - Supplies the number of bytes of available buffer.

    RequiredSize - Supplies the required size for OptionListBase buffer.

Return Value:

    NT status code.

--*/

{

    ULONG AvailableBytes;
    PBOOT_ENTRY_OPTION LocalPreviousOption;
    ULONG LocalRequiredSize;
    PBOOT_ENTRY_OPTION Option;
    BOOLEAN PopulatedBuffer;
    ULONG Size;
    NTSTATUS Status;
    PWCHAR String;
    ULONG TotalOffset;

#if !defined(MISC_MIN)

    PBCDE_BOOLEAN BooleanElement;
    BCDE_CONFIG_ACCESS_POLICY ConfigAccessPolicy;
    BOOLEAN Found;
    BCDE_GRAPHICS_RESOLUTION_TYPE GraphicsResolution;
    BCDE_HYPERVISOR_IOMMU_POLICY HypervisorIommuPolicy;
    BCDE_HYPERVISOR_LAUNCH_TYPE HypervisorLaunchType;
    PBCDE_INTEGER IntegerElement;
    BOOLEAN MaxGroupFlag;
    ULONGLONG MaxMegabyteValue;
    BCDE_MSI_POLICY MsiPolicy;
    BCDE_NX_POLICY NxPolicy;
    BCDE_PAE_POLICY PaePolicy;
    BCDE_PCI_EXPRESS_POLICY PciExpressPolicy;
    ULONG Port;
    BCDE_TSC_SYNC_POLICY TscSyncPolicy;
    BCDE_VSM_LAUNCH_TYPE VsmLaunchType;
    BCDE_X2APIC_POLICY X2ApicPolicy;

#endif

    if (ARGUMENT_PRESENT(OptionString) == FALSE) {
        return STATUS_SUCCESS;
    }

    if (ARGUMENT_PRESENT(OptionListBase) == FALSE) {
        OptionListSize = 0;
    }

    if (ARGUMENT_PRESENT(Offset)) {
        TotalOffset = *Offset;

    } else {
        TotalOffset = 0;
    }

    __assume_bound(TotalOffset);
    if (ARGUMENT_PRESENT(PreviousOption)) {
        LocalPreviousOption = *PreviousOption;

        __analysis_assume(LocalPreviousOption > OptionListBase);
        __analysis_assume(LocalPreviousOption + LocalPreviousOption->DataSize <=
                          OptionListBase + TotalOffset);
    } else {
        LocalPreviousOption = NULL;
    }

    AvailableBytes = OptionListSize;
    Option = Add2Ptr(OptionListBase, TotalOffset);
    PopulatedBuffer = TRUE;
    *RequiredSize = 0;

    //
    // Uppercase the load options string so all comparisons can be made against
    // uppercased version of the string.
    //

    _wcsupr(OptionString);

    //
    // Filtering against Secure Boot Policy is not needed here since future
    // reads of the options created will be filtered. Filtering is only needed
    // when converting back from a boot option to a string in
    // ArcConvertOptionToSubString.
    //

#if !defined(MISC_MIN)

    //
    // Handle MAXMEM.  Note that advancing the string past the sizeof L"MAXMEM"
    // also advances past the equals sign, since the sizeof operator includes
    // space for the NULL.
    //

    if ((String = wcsstr(OptionString, L"MAXMEM=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"MAXMEM"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_LIBRARY_TYPE_TRUNCATE_PHYSICAL_MEMORY,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            MaxMegabyteValue = MAXULONGLONG / _1MB;
            IntegerElement = Add2Ptr(Option, Option->DataOffset);
            if (IntegerElement->Value <= MaxMegabyteValue) {
                Size = BlGetBootOptionSize(Option);
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }
        }
    }

    //
    // Handle AVOIDLOWMEMORY.  Note that advancing the string past the sizeof
    // L"AVOIDLOWMEMORY" also advances past the equals sign, since the sizeof
    // operator includes space for the NULL.
    //

    if ((String = wcsstr(OptionString, L"AVOIDLOWMEMORY=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"AVOIDLOWMEMORY"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_LIBRARY_TYPE_AVOID_LOW_PHYSICAL_MEMORY,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            MaxMegabyteValue = MAXULONGLONG / _1MB;
            IntegerElement = Add2Ptr(Option, Option->DataOffset);
            if (IntegerElement->Value <= MaxMegabyteValue) {
                Size = BlGetBootOptionSize(Option);
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }
        }
    }

    //
    // Handle timeout.  Note that advancing the string past the sizeof
    // L"timeout" also advances past the equals sign, since the sizeof operator
    // includes space for the NULL.
    //

    if ((String = wcsstr(OptionString, L"TIMEOUT=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"TIMEOUT"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_BOOTMGR_TYPE_TIMEOUT,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle Kernel path.
    //

    if ((String = wcsstr(OptionString, L"KERNEL=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"KERNEL"));
        Status = ArcConvertStringToOption(String,
                                          TRUE,
                                          BCDE_OSLOADER_TYPE_KERNEL_PATH,
                                          Option,
                                          AvailableBytes,
                                          &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle HAL path.
    //

    if ((String = wcsstr(OptionString, L"HAL=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"HAL"));
        Status = ArcConvertStringToOption(String,
                                          TRUE,
                                          BCDE_OSLOADER_TYPE_HAL_PATH,
                                          Option,
                                          AvailableBytes,
                                          &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle NX policy.
    //

    Found = TRUE;
    NxPolicy = 0;
    if (wcsstr(OptionString, L"NOEXECUTE=OPTIN") != NULL) {
        NxPolicy = NxPolicyOptIn;

    } else if (wcsstr(OptionString, L"NOEXECUTE=OPTOUT") != NULL) {
        NxPolicy = NxPolicyOptOut;

    } else if (wcsstr(OptionString, L"NOEXECUTE=ALWAYSON") != NULL) {
        NxPolicy = NxPolicyAlwaysOn;

    } else if (wcsstr(OptionString, L"NOEXECUTE=ALWAYSOFF") != NULL) {
        NxPolicy = NxPolicyAlwaysOff;

    } else if (wcsstr(OptionString, L"NOEXECUTE") != NULL) {
        NxPolicy = NxPolicyOptIn;

    } else if (wcsstr(OptionString, L"EXECUTE") != NULL) {
        NxPolicy = NxPolicyAlwaysOff;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_NX_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = NxPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle PAE policy.
    //

    Found = TRUE;
    PaePolicy = PaePolicyDefault;
    if (wcsstr(OptionString, L"NOPAE") != NULL) {
        PaePolicy = PaePolicyForceDisable;

    } else if (wcsstr(OptionString, L"PAE") != NULL) {
        PaePolicy = PaePolicyForceEnable;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_PAE_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = PaePolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle Debugger options.  DEBUG is a substring of NODEBUG, and so if
    // NODEBUG exists, don't perform the search for DEBUG.
    //

    if (wcsstr(OptionString, L"NODEBUG")) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = FALSE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }

    } else if (wcsstr(OptionString, L"DEBUG") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    if (wcsstr(OptionString, L"BOOTDEBUG") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    if ((String = wcsstr(OptionString, L"DEBUGPORT")) != NULL) {
        String = Add2Ptr(String, sizeof(L"DEBUGPORT"));
        Port = 0;
        if (wcsstr(String, L"COM1") != NULL) {
            Port = 1;

        } else if (wcsstr(String, L"COM2") != NULL) {
            Port = 2;

        } else if (wcsstr(String, L"COM3") != NULL) {
            Port = 3;

        } else if (wcsstr(String, L"COM4") != NULL) {
            Port = 4;
        }

        if (Port != 0) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_PORT_NUMBER;
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = Port;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }

            //
            // Create an element stating the debugger is serial.
            //

            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_TYPE;
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = DebuggerSerial;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }

        } else if (wcsstr(String, L"1394") != NULL) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_TYPE;
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = Debugger1394;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }

        } else if (wcsstr(String, L"USB") != NULL) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_TYPE;
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = DebuggerUsb;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }

         } else if (wcsstr(String, L"NET") != NULL) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_TYPE;
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = DebuggerNet;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }

         } else if (wcsstr(String, L"LOCAL") != NULL) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                Option->Type = BCDE_LIBRARY_TYPE_DEBUGGER_TYPE;
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = DebuggerLocal;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }
        }
    }

    if ((String = wcsstr(OptionString, L"BAUDRATE")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"BAUDRATE"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_LIBRARY_TYPE_DEBUGGER_BAUDRATE,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle 1394 debug channel number.
    //

    if ((String = wcsstr(OptionString, L"CHANNEL=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"CHANNEL"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_LIBRARY_TYPE_DEBUGGER_1394_CHANNEL,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle USB debug target name.
    //

    if ((String = wcsstr(OptionString, L"TARGETNAME=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"TARGETNAME"));
        Status = ArcConvertStringToOption(
                     String,
                     TRUE,
                     BCDE_LIBRARY_TYPE_DEBUGGER_USB_TARGETNAME,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle EMS options.
    //

    if ((String = wcsstr(OptionString, L"REDIRECT=")) != NULL) {
        String = Add2Ptr(String, sizeof(L"REDIRECT"));
        Port = 0;
        if (wcsstr(String, L"COM1") != NULL) {
            Port = 1;

        } else if (wcsstr(String, L"COM2") != NULL) {
            Port = 2;

        } else if (wcsstr(String, L"COM3") != NULL) {
            Port = 3;

        } else if (wcsstr(String, L"COM4") != NULL) {
            Port = 4;
        }

        if (Port != 0) {
            if (LocalPreviousOption != NULL) {
                LocalPreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionListBase, Option);
            }

            Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
            *RequiredSize += Size;
            if (AvailableBytes < Size) {
                PopulatedBuffer = FALSE;
            }

            if (PopulatedBuffer != FALSE) {
                RtlZeroMemory(Option, Size);
                IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
                Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
                Option->Type = BCDE_LIBRARY_TYPE_EMS_PORT_NUMBER;
                Option->DataSize = sizeof(BCDE_INTEGER);
                IntegerElement->Value = Port;
                TotalOffset += Size;
                AvailableBytes -= Size;
                LocalPreviousOption = Option;
                Option = Add2Ptr(Option, Size);
            }
        }

        //
        // The existence of REDIRECT= implies both that boot environment and
        // kernel redirection is enabled.  Create an option for each.
        //

        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_LIBRARY_TYPE_EMS_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }

        //
        // Create the second option for the OS.
        //

        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_KERNEL_EMS_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    if ((String = wcsstr(OptionString, L"REDIRECTBAUDRATE=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"REDIRECTBAUDRATE"));
        Status = ArcConvertStringIntegerToOption(String,
                                                 BCDE_LIBRARY_TYPE_EMS_BAUDRATE,
                                                 Option,
                                                 AvailableBytes,
                                                 &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle /3GB and /USERVA case. When /USERVA is used with /3GB, it requests
    // a specific usermode virtual address space size, in megabytes (between
    // 2GB and 3GB).  If not specified, use the /3GB default value.
    //

    if ((String = wcsstr(OptionString, L"3GB")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        if ((String = wcsstr(OptionString, L"USERVA=")) != NULL) {
            String = Add2Ptr(String, sizeof(L"USERVA"));

        } else {
            String = L"3072";
        }

        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_OSLOADER_TYPE_INCREASE_USER_VA,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle integrity check options.
    //

    if ((wcsstr(OptionString, L"DISABLE_INTEGRITY_CHECKS") != NULL) ||
        (wcsstr(OptionString, L"NOINTEGRITYCHECKS") != NULL)) {

        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_LIBRARY_TYPE_DISABLE_INTEGRITY_CHECKS;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle the ELAM driver switch.
    //

    if (wcsstr(OptionString, L"DISABLEELAMDRIVERS") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_DISABLE_ELAM_DRIVERS;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle the test signing switch.
    //

    if (wcsstr(OptionString, L"TESTSIGNING") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_LIBRARY_TYPE_ALLOW_PRERELEASE_SIGNATURES;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle PCI configuration space access policy.
    //

    Found = TRUE;
    ConfigAccessPolicy = 0;
    if (wcsstr(OptionString, L"CONFIGACCESSPOLICY=DISALLOWMMCONFIG") != NULL) {
        ConfigAccessPolicy = ConfigAccessPolicyDisallowMmConfig;

    } else if (wcsstr(OptionString, L"CONFIGACCESSPOLICY=DEFAULT") != NULL) {
        ConfigAccessPolicy = ConfigAccessPolicyDefault;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_LIBRARY_TYPE_CONFIG_ACCESS_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = ConfigAccessPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle MSI policy.
    //

    Found = TRUE;
    MsiPolicy = 0;
    if (wcsstr(OptionString, L"MSIPOLICY=DEFAULT") != NULL) {
        MsiPolicy = MsiPolicyDefault;

    } else if (wcsstr(OptionString, L"MSIPOLICY=FORCEDISABLE") != NULL) {
        MsiPolicy = MsiPolicyForceDisable;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_MSI_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = MsiPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle PCI express policy.
    //

    Found = TRUE;
    PciExpressPolicy = 0;
    if (wcsstr(OptionString, L"PCIEXPRESSPOLICY=DEFAULT") != NULL) {
        PciExpressPolicy = PciExpressPolicyDefault;

    } else if (wcsstr(OptionString, L"PCIEXPRESSPOLICY=FORCEDISABLE") != NULL) {
        PciExpressPolicy = PciExpressPolicyForceDisable;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_PCI_EXPRESS_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = PciExpressPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle graphics resolution.
    //

    if (wcsstr(OptionString, L"GRAPHICSRESOLUTION=1024X768") != NULL) {
        GraphicsResolution = GraphicsResolution1024x768;
        Found = TRUE;

    } else if (wcsstr(OptionString, L"GRAPHICSRESOLUTION=800X600") != NULL) {
        GraphicsResolution = GraphicsResolution800x600;
        Found = TRUE;

    } else if (wcsstr(OptionString, L"GRAPHICSRESOLUTION=1024x600") != NULL) {
        GraphicsResolution = GraphicsResolution1024x600;
        Found = TRUE;

    } else {
        GraphicsResolution = 0; // Satify compiler.
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_LIBRARY_TYPE_GRAPHICS_RESOLUTION;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = GraphicsResolution;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle NUMA node group assignment option.
    // MAXGROUP=ON and MAXGROUP alone are the same, both indicating the option
    // value is TRUE. MAXGROUP=OFF indicates the option value is FALSE.
    //

    if (wcsstr(OptionString, L"MAXGROUP=OFF") != NULL) {
        Found = TRUE;
        MaxGroupFlag = FALSE;

    } else if (wcsstr(OptionString, L"MAXGROUP") != NULL)  {
        Found = TRUE;
        MaxGroupFlag = TRUE;

    } else {
        Found = FALSE;
        MaxGroupFlag = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_MAXIMIZE_GROUPS_CREATED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = MaxGroupFlag;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle group size option.
    //

    if ((String = wcsstr(OptionString, L"GROUPSIZE=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"GROUPSIZE"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_OSLOADER_TYPE_GROUP_SIZE,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle force group awareness option.
    //

    if (wcsstr(OptionString, L"FORCEGROUPAWARE") != NULL)  {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_FORCE_GROUP_AWARENESS;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle APIC mode option.
    //

    if (wcsstr(OptionString, L"USELEGACYAPICMODE") != NULL)  {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_USE_LEGACY_APIC_MODE;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle TSC Synchronization Policy.
    //

    Found = TRUE;
    TscSyncPolicy = TscSyncPolicyDefault;
    if (wcsstr(OptionString, L"TSCSYNCPOLICY=LEGACY") != NULL) {
        TscSyncPolicy = TscSyncPolicyLegacy;

    } else if (wcsstr(OptionString, L"TSCSYNCPOLICY=ENHANCED") != NULL) {
        TscSyncPolicy = TscSyncPolicyEnhanced;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_TSC_SYNC_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = TscSyncPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle X2APIC policy.
    //

    Found = TRUE;
    X2ApicPolicy = X2APICPolicyDefault;
    if (wcsstr(OptionString, L"X2APICPOLICY=DEFAULT") != NULL) {
        X2ApicPolicy = X2APICPolicyDefault;

    } else if (wcsstr(OptionString, L"X2APICPOLICY=DISABLE") != NULL) {
        X2ApicPolicy = X2APICPolicyDisable;

    } else if (wcsstr(OptionString, L"X2APICPOLICY=ENABLE") != NULL) {
        X2ApicPolicy = X2APICPolicyEnable;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_X2APIC_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = X2ApicPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle debugger bus parameters.
    //

    if ((String = wcsstr(OptionString, L"BUSPARAMS=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"BUSPARAMS"));
        Status = ArcConvertStringToOption(
                     String,
                     TRUE,
                     BCDE_LIBRARY_TYPE_DEBUGGER_BUS_PARAMETERS,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle FORCELEGACYPLATFORM option.
    //

    if (wcsstr(OptionString, L"FORCELEGACYPLATFORM") != NULL)  {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_FORCE_LEGACY_PLATFORM;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor launch setting.
    //

    Found = TRUE;
    HypervisorLaunchType = 0;
    if (wcsstr(OptionString, L"HYPERVISORLAUNCHTYPE=AUTO") != NULL) {
        HypervisorLaunchType = HypervisorLaunchAuto;

    } else if (wcsstr(OptionString, L"HYPERVISORLAUNCHTYPE=OFF") != NULL) {
        HypervisorLaunchType = HypervisorLaunchOff;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_LAUNCH_TYPE;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = HypervisorLaunchType;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor debugger enabled setting.
    //

    if (wcsstr(OptionString, L"HYPERVISORDBG") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor SLAT disable option.
    //

    if (wcsstr(OptionString, L"HYPERVISORDISABLESLAT") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_SLAT_DISABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor large VTLB option.
    //

    if (wcsstr(OptionString, L"HYPERVISORUSELARGEVTLB") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_USE_LARGE_VTLB;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor logical processor count setting.
    //

    if ((String = wcsstr(OptionString, L"HYPERVISORNUMPROC=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"HYPERVISORNUMPROC"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_NUM_PROC,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor root virtual processor count setting.
    //

    if ((String = wcsstr(OptionString, L"HYPERVISORROOTPROC=")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"HYPERVISORROOTPROC"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_ROOT_PROC,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor root virtual processors per node setting.
    //

    if ((String = wcsstr(OptionString, L"HYPERVISORROOTPROCPERNODE=")) !=
        NULL) {

        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = Add2Ptr(String, sizeof(L"HYPERVISORROOTPROCPERNODE"));
        Status = ArcConvertStringIntegerToOption(
                     String,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_ROOT_PROC_PER_NODE,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        *RequiredSize += LocalRequiredSize;
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor IOMMU policy option.
    //

    Found = TRUE;
    HypervisorIommuPolicy = 0;
    if (wcsstr(OptionString, L"HYPERVISORIOMMUPOLICY=DEFAULT") != NULL) {
        HypervisorIommuPolicy = HypervisorIommuPolicyDefault;

    } else if (wcsstr(OptionString, L"HYPERVISORIOMMUPOLICY=ENABLE") != NULL) {
        HypervisorIommuPolicy = HypervisorIommuPolicyEnable;

    } else if (wcsstr(OptionString, L"HYPERVISORIOMMUPOLICY=DISABLE") != NULL) {
        HypervisorIommuPolicy = HypervisorIommuPolicyDisable;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_IOMMU_POLICY;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = HypervisorIommuPolicy;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle hypervisor full APIC virtualization enable option.
    //

    if (wcsstr(OptionString, L"HYPERVISORUSEVAPIC") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_HYPERVISOR_USE_VAPIC;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle VSM launch setting.
    //

    Found = TRUE;
    VsmLaunchType = 0;
    if (wcsstr(OptionString, L"VSMLAUNCHTYPE=AUTO") != NULL) {
        VsmLaunchType = VsmLaunchAuto;

    } else if (wcsstr(OptionString, L"VSMLAUNCHTYPE=OFF") != NULL) {
        VsmLaunchType = VsmLaunchOff;

    } else {
        Found = FALSE;
    }

    if (Found != FALSE) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
            Option->Type = BCDE_OSLOADER_TYPE_VSM_LAUNCH_TYPE;
            Option->DataSize = sizeof(BCDE_INTEGER);
            IntegerElement->Value = VsmLaunchType;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle remote event logging options.
    //

    if (wcsstr(OptionString, L"NOEVENT")) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = FALSE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }

    } else if (wcsstr(OptionString, L"EVENT") != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_BOOLEAN);
        *RequiredSize += Size;
        if (AvailableBytes < Size) {
            PopulatedBuffer = FALSE;
        }

        if (PopulatedBuffer != FALSE) {
            RtlZeroMemory(Option, Size);
            BooleanElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
            Option->DataOffset = (ULONG)PtrOffset(Option, BooleanElement);
            Option->Type = BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED;
            Option->DataSize = sizeof(BCDE_BOOLEAN);
            BooleanElement->Value = TRUE;
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    //
    // Handle recovery bcd option.
    //

    if ((String = wcsstr(OptionString, L"RECOVERYBCD")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        Status = ArcConvertStringToOption(
                     DEFAULT_EFI_RECOVERY_DIRECTORY L"BCD",
                     TRUE,
                     BCDE_BOOTMGR_TYPE_BCD_FILEPATH,
                     Option,
                     AvailableBytes,
                     &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

#else // !MISC_MIN

    //
    // Handle boot parameter string options.  Boot parameter string options
    // inclue /CMDCONS and /ROLLBACK.
    //

    if ((String = wcsstr(OptionString, L"CMDCONS")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = L"cmdcons";
        Status = ArcConvertStringToOption(String,
                                          TRUE,
                                          BCDE_LEGACY_LOADER_TYPE_BPB_STRING,
                                          Option,
                                          AvailableBytes,
                                          &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

    if ((String = wcsstr(OptionString, L"ROLLBACK")) != NULL) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        String = L"undo";
        Status = ArcConvertStringToOption(String,
                                          TRUE,
                                          BCDE_LEGACY_LOADER_TYPE_BPB_STRING,
                                          Option,
                                          AvailableBytes,
                                          &LocalRequiredSize);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_BUFFER_TOO_SMALL) {
                PopulatedBuffer = FALSE;

            } else {
                return Status;
            }
        }

        *RequiredSize += LocalRequiredSize;

        if (PopulatedBuffer != FALSE) {
            Size = BlGetBootOptionSize(Option);
            TotalOffset += Size;
            AvailableBytes -= Size;
            LocalPreviousOption = Option;
            Option = Add2Ptr(Option, Size);
        }
    }

#endif // MISC_MIN

    //
    // Finally, an option containing the entire string.
    //

    Status = ArcConvertStringToOption(
                 OptionString,
                 FALSE,
                 BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING,
                 Option,
                 AvailableBytes,
                 &LocalRequiredSize);

    if (!NT_SUCCESS(Status)) {
        if (Status == STATUS_BUFFER_TOO_SMALL) {
            PopulatedBuffer = FALSE;

        } else {
            return Status;
        }
    }

    *RequiredSize += LocalRequiredSize;

    if (NT_SUCCESS(Status)) {
        if (LocalPreviousOption != NULL) {
            LocalPreviousOption->NextOptionOffset =
                (ULONG)PtrOffset(OptionListBase, Option);
        }

        LocalPreviousOption = Option;
        Size = BlGetBootOptionSize(Option);
        TotalOffset += Size;
    }

    if (PopulatedBuffer != FALSE) {
        if (ARGUMENT_PRESENT(Offset)) {
            *Offset = TotalOffset;
        }

        if (ARGUMENT_PRESENT(PreviousOption)) {
            *PreviousOption = LocalPreviousOption;
        }

        Status =  STATUS_SUCCESS;
    }

    return Status;
}

//
// ------------------------------------------------------------ Local Functions
//

BOOLEAN
GetArcPathMnemonicKey (
    __in PSTR ArcPath,
    __in PSTR Mnemonic,
    __in PULONG Key,
    __in ULONG Base
    )

/*++

Routine Description:

    This routine looks for the given Mnemonic in ArcPath.  If Mnemonic is a
    component of the path, then it converts the key value to an integer wich
    is returned in Key.

Arguments:

    ArcPath  - Pointer to a string that contains an ARC pathname.

    Mnemonic - Pointer to a string that contains a ARC Mnemonic

    Key      - Pointer to a ULONG where the Key value is stored.

    Base     - The number scale Key was written with (8, 10, 16, etc).


Return Value:

    TRUE   if mnemonic is found in path and a valid key is converted.
    FALSE  otherwise.

--*/

{

    WCHAR Digits[9];
    ULONG Id;
    CHAR String[16];
    PCHAR Tmp;

    //
    // Construct a string of the form ")mnemonic(".
    //

    String[0]=')';
    for(Id = 1; (*Mnemonic != '\0') && (Id < sizeof(String) - 2); Id += 1) {
        String[Id] = * Mnemonic++;
    }
    String[Id++]='(';
    String[Id]='\0';

    //
    // Attempt to find the mnemonic in the specified arcpath.  Allow for both
    // the case where the Mnemonic is the start of the path.
    //

    Tmp = strstr(ArcPath, &String[1]);

    if (Tmp != ArcPath) {
        Tmp = strstr(ArcPath, String);
    } else {
        Id -= 1;
    }

    if (Tmp == NULL) {
        return FALSE;
    }

    //
    // Convert the value in between parentheses to integer
    //

    Tmp += Id;
    for (Id = 0; Id < RTL_NUMBER_OF(Digits) - 1; Id += 1) {
        if (*Tmp == ')') {
            Digits[Id] = '\0';
            break;
        }
        Digits[Id] = *Tmp++;
    }

    Digits[Id]='\0';
    *Key = wcstoul(Digits, NULL, Base);

    return TRUE;
}

NTSTATUS
ArcTranslatePartitionDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    )

/*++

Routine Description:

    This routine will translate the file path a boot environment partition
    device into an arc device path.

Arguments:

    DeviceId - Handle for partition device.

    Information - Supplies a pointer to the device information for the device
        with the given ID.

    ArcDeviceBuffer - Buffer to return arc device path name for the partition
        device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    Other error possible if unable to open parent device.

--*/

{

    DEVICE_TYPE DeviceType;
    GUID UNALIGNED *Guid;
    ULONG Id;
    PBLOCK_IO_IDENTIFIER Parent;
    DEVICE_ID ParentDeviceId;
    PBOOT_ENVIRONMENT_DEVICE ParentIdentifier;
    ULONG ParentIdentifierSize;
    ULONG_PTR ParentOffset;
    ANSI_STRING ParentPath;
    ULONG ParentSize;
    PBOOT_ENVIRONMENT_DEVICE PartitionIdentifier;
    NTSTATUS Status;

#if EFI

    PDEVICE_ID CurrDeviceId;
    GUID UNALIGNED *DeviceGuid;
    GENERIC_BUFFER DeviceIdBuffer;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    DEVICE_INFORMATION DeviceInformation;
    BOOT_ENVIRONMENT_DEVICE EnumDevice;
    PDEVICE_ID EndDeviceId;

#endif

    ASSERT(Information->u.BlockIo.Type == BLOCK_IO_PARTITION);

    ParentIdentifier = NULL;

    //
    // Get a pointer to the device identifier for this partition.
    // It is needed in order to get the partition number as well as
    // determining the arc name for the parent device.
    //

    Status = BlGetDeviceIdentifier(DeviceId, &PartitionIdentifier);
    ASSERT(Status == STATUS_SUCCESS);
    if (Status != STATUS_SUCCESS) {
        goto TranslatePartitionDeviceEnd;
    }

    //
    // Based on the type of partition device supplied by the caller, load a
    // pointer to the parent identifier and compute its offset within the
    // device identifier structure.
    //

    DeviceType = PartitionIdentifier->DeviceType;
    if (DeviceType == DEVICE_PARTITION) {
        Parent = &PartitionIdentifier->u.Partition.ParentIdentifier;
        Guid = &PartitionIdentifier->u.Partition.Gpt.PartitionSignature;
        ParentOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                    u.Partition.ParentIdentifier);

    } else if (DeviceType == DEVICE_PARTITION_EX) {
        Parent = &PartitionIdentifier->u.PartitionEx.ParentIdentifier;
        Guid = &PartitionIdentifier->u.PartitionEx.Gpt.PartitionSignature;
        ParentOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                    u.PartitionEx.ParentIdentifier);

    } else {
        Status = STATUS_NOT_SUPPORTED;
        goto TranslatePartitionDeviceEnd;
    }

    //
    // Open the parent device in order to optain a device id.  The device id
    // is required to translate the parent device into an arc path.
    //

    ParentSize = (ULONG)(PartitionIdentifier->Size - ParentOffset);
    ParentIdentifierSize =
        FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo) +
        max(sizeof(BLOCK_IO_IDENTIFIER), ParentSize);

    ParentIdentifier = BlMmAllocateHeap(ParentIdentifierSize);
    if (ParentIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto TranslatePartitionDeviceEnd;
    }

    RtlZeroMemory(ParentIdentifier, ParentIdentifierSize);
    ParentIdentifier->DeviceType = DEVICE_BLOCK_IO;
    ParentIdentifier->Size = ParentIdentifierSize;
    RtlCopyMemory(&ParentIdentifier->u.BlockIo, Parent, ParentSize);

#if EFI

    //
    // The EFI device namespace does not describe a parent device when
    // describing a GPT partition.  Since the partition identifier is assumed
    // to be unique, this is not required.  Unfortunetly, arcpaths do not
    // describe GPT partition identifiers.  This becomes problematic when
    // booting from a GPT partition on an EFI system.  In this case, a device
    // identifier must be constructed for the boot device.  Since no parent
    // information is known, it can not be included in the device identifier.
    // In this case, we must enumerate all partitions and map a partition to
    // both a partition number and a parent device.
    //
    // N.B. Partition enumeration includes parent device information.
    //

    if (CHECK_FLAG(PartitionIdentifier->Flags,
                   DEVICE_FLAG_PARENT_SIGNATURE_INVALID) != FALSE) {

        ASSERT(Information->u.BlockIo.PartitionType == GPT);

        RtlZeroMemory(&EnumDevice, sizeof(BOOT_ENVIRONMENT_DEVICE));
        EnumDevice.DeviceType = DEVICE_PARTITION_EX;
        EnumDevice.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
        DeviceIdBuffer.BufferSize = 0;
        Status = BlDeviceEnumerateDeviceClass(&EnumDevice,
                                              &DeviceIdBuffer,
                                              TRUE);

        if (!NT_SUCCESS(Status)) {
            goto TranslatePartitionDeviceEnd;
        }

        ASSERT((DeviceIdBuffer.Buffer != NULL) &&
               (DeviceIdBuffer.DataSize != 0));

        //
        // Walk the list of enumerated partitions, search for a GPT partition
        // with a matching partition signature.
        //
        // N.B. Initialization code has already loaded a pointer to the
        //      partition GUID embedded in the given partition identifier
        //

        CurrDeviceId = DeviceIdBuffer.Buffer;
        EndDeviceId = Add2Ptr(DeviceIdBuffer.Buffer, DeviceIdBuffer.DataSize);
        while (CurrDeviceId < EndDeviceId) {
            Status = BlGetDeviceIdentifier(*CurrDeviceId, &DeviceIdentifier);

            ASSERT(Status == STATUS_SUCCESS);
            ASSERT(DeviceIdentifier->DeviceType == DEVICE_PARTITION_EX);

            Status = BlDeviceGetInformation(*CurrDeviceId, &DeviceInformation);

            ASSERT(Status == STATUS_SUCCESS);
            ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);
            ASSERT(DeviceInformation.u.BlockIo.Type == BLOCK_IO_PARTITION);

            //
            // If the current device is a GPT partition, compare the partition
            // signature with the one we wish to translate.
            //

            DeviceGuid =
                &DeviceIdentifier->u.PartitionEx.Gpt.PartitionSignature;

            if ((DeviceInformation.u.BlockIo.PartitionType == GPT) &&
                (RtlEqualMemory(Guid, DeviceGuid, sizeof(GUID)))) {

                //
                // When a match is found, save the parent device identifier
                // so that the parent device can be opened.
                //

                RtlCopyMemory(&ParentIdentifier->u.BlockIo,
                              &DeviceIdentifier->u.PartitionEx.ParentIdentifier,
                              sizeof(BLOCK_IO_IDENTIFIER));

                break;
            }

            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
        }

        ASSERT(CurrDeviceId != EndDeviceId);
    }

#endif

    //
    // Open the parent device to obtain a device id.  The device id is required
    // to translate the parent device into an arch path.
    //

    Status = BlDeviceOpen(ParentIdentifier, OPEN_READ, &ParentDeviceId);
    if (!NT_SUCCESS(Status)) {
        goto TranslatePartitionDeviceEnd;
    }

    //
    // Get the arc name for the parent device.
    //

    RtlInitAnsiString(&ParentPath, NULL);
    Status = AhGetArcDevice(ParentDeviceId, &ParentPath, TRUE);
    if (!NT_SUCCESS(Status)) {
        goto TranslatePartitionDeviceEnd;
    }

    //
    // The partition number is added to the arc path.  The information data
    // structure contains this value for all disks.
    //

    if (Information->u.BlockIo.PartitionType == GPT) {
        Id = Information->u.BlockIo.DeviceInformation.Partition.Gpt.Number;

    } else {
        Id = Information->u.BlockIo.DeviceInformation.Partition.Mbr.Number;
    }

    sprintf_s(ArcDeviceBuffer->Buffer,
              ArcDeviceBuffer->MaximumLength - 1,
              "%spartition(%d)",
              ParentPath.Buffer,
              Id);

    BlMmFreeHeap(ParentPath.Buffer);
    ArcDeviceBuffer->Buffer[ArcDeviceBuffer->MaximumLength - 1] = '\0';
    ArcDeviceBuffer->Length = (USHORT)strlen(ArcDeviceBuffer->Buffer);

    //
    // Arc device name is currently in ArcDeviceBuffer.
    //

    Status = STATUS_SUCCESS;

TranslatePartitionDeviceEnd:
    if (ParentIdentifier != NULL) {
        BlMmFreeHeap(ParentIdentifier);
    }

    return Status;
}

NTSTATUS
ArcTranslateBlockIoDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    )

/*++

Routine Description:

    This routine will translate the file path a boot environment block i/o
    device into an arc device path.

Arguments:

    DeviceId - Handle for block i/o device.

    Information - Supplies a pointer to the device information for the device
        with the given ID.

    ArcDeviceBuffer - Buffer to return arc device path name for the partition
        device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY is memory allocation fails.
    STATUS_NO_SUCH_DEVICE if the device cannot be accessed.

--*/

{

    GENERIC_BUFFER DeviceHandleBuffer;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    BOOT_ENVIRONMENT_DEVICE EnumerationIdentifier;
    PDEVICE_ID HandleArray;
    ULONG HandleCount;
    PBOOT_ENVIRONMENT_DEVICE HandleIdentifier;
    ULONG Instance;
    DEVICE_ID ParentId;
    PBOOT_ENVIRONMENT_DEVICE ParentIdentifier;
    PUNALIGNED_FILE_PATH SourcePath;
    NTSTATUS Status;

    if (Information->u.BlockIo.Type == BLOCK_IO_PARTITION) {
        return ArcTranslatePartitionDevice(DeviceId,
                                           Information,
                                           ArcDeviceBuffer);
    }

    //
    // The ARC path for a device that resides inside a file is the
    // ARC path of the parent device that the file is on.  Recurse to
    // get this ARC path.
    //

    if (Information->u.BlockIo.Type == BLOCK_IO_FILE) {
        Status = BlGetDeviceIdentifier(DeviceId, &HandleIdentifier);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        SourcePath = &HandleIdentifier->u.BlockIo.u.File.SourcePath;
        ParentIdentifier = BlGetDevice(SourcePath);
        if (ParentIdentifier != NULL) {
            Status = BlDeviceOpen(ParentIdentifier, OPEN_READ, &ParentId);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

        } else {
            return STATUS_NO_SUCH_DEVICE;
        }

        Status = AhGetArcDevice(ParentId, ArcDeviceBuffer, FALSE);
        BlDeviceClose(ParentId);
        return Status;
    }

    ASSERT((Information->u.BlockIo.Type != BLOCK_IO_PARTITION) &&
           (Information->u.BlockIo.Type != BLOCK_IO_FILE));

    //
    // There's only ever one instance of a VHD.
    //

    if (Information->u.BlockIo.Type == BLOCK_IO_VIRTUAL_HARD_DISK) {
        sprintf_s(ArcDeviceBuffer->Buffer,
                  ArcDeviceBuffer->MaximumLength - 1,
                  "multi(0)disk(0)vdisk(0)");

        ArcDeviceBuffer->Length = (USHORT)strlen(ArcDeviceBuffer->Buffer);
        return STATUS_SUCCESS;
    }

    //
    // Enumerate all block i/o devices of this type.  Create a device
    // identifier with the same device class / sub class as the device
    // the caller provided.
    //

    RtlZeroMemory(&EnumerationIdentifier, sizeof(BOOT_ENVIRONMENT_DEVICE));
    EnumerationIdentifier.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    EnumerationIdentifier.DeviceType = DEVICE_BLOCK_IO;
    EnumerationIdentifier.u.BlockIo.BlockIoType = Information->u.BlockIo.Type;

    //
    // Get a device handle for every device of this type.
    //

    DeviceHandleBuffer.BufferSize = 0;
    Status = BlDeviceEnumerateDeviceClass(&EnumerationIdentifier,
                                          &DeviceHandleBuffer,
                                          TRUE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The above array is returned in the order the firmware enumerated
    // the devices.  Arc syntax utilizes the order of firmware
    // enumeration.  Assign the arc number based on the instance number
    // in this array.  Compare devices in the array to the input device
    // based on their device identifiers rather than their device IDs,
    // since devices may have multiple device IDs associated with them
    // based on attributes the device was opened with.
    //

    HandleCount = DeviceHandleBuffer.DataSize / sizeof(DEVICE_ID);
    HandleArray = DeviceHandleBuffer.Buffer;
    Status = BlGetDeviceIdentifier(DeviceId, &DeviceIdentifier);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    for (Instance = 0; Instance < HandleCount; Instance += 1) {
        Status = BlGetDeviceIdentifier(HandleArray[Instance],
                                       &HandleIdentifier);

        ASSERT(NT_SUCCESS(Status));

        if ((HandleIdentifier->Size == DeviceIdentifier->Size)  &&
            RtlEqualMemory(HandleIdentifier,
                           DeviceIdentifier,
                           DeviceIdentifier->Size)) {

            break;
        }
    }

    ASSERT(Instance < HandleCount);
    BlMmFreeHeap(DeviceHandleBuffer.Buffer);  // Free handle array.

    switch (Information->u.BlockIo.Type) {
    case BLOCK_IO_HARD_DISK:
        sprintf_s(ArcDeviceBuffer->Buffer,
                  ArcDeviceBuffer->MaximumLength - 1,
                  "multi(0)disk(0)rdisk(%d)",
                  Instance);
        break;

    case BLOCK_IO_REMOVABLE_DISK:
        sprintf_s(ArcDeviceBuffer->Buffer,
                  ArcDeviceBuffer->MaximumLength - 1,
                  "multi(0)disk(0)fdisk(%d)",
                  Instance);
        break;

    case BLOCK_IO_CDROM:
        sprintf_s(ArcDeviceBuffer->Buffer,
                  ArcDeviceBuffer->MaximumLength - 1,
                  "multi(0)disk(0)cdrom(%d)",
                  Instance);
        break;

    case BLOCK_IO_RAMDISK:
        sprintf_s(ArcDeviceBuffer->Buffer,
                  ArcDeviceBuffer->MaximumLength - 1,
                  "ramdisk(%d)",
                  Instance);
        break;

    default:

            //
            // Unknown device type.
            //

        return STATUS_NOT_IMPLEMENTED;
    }

    //
    // Arc device name currently in Buffer.
    //

    ArcDeviceBuffer->Length = (USHORT)strlen(ArcDeviceBuffer->Buffer);
    return STATUS_SUCCESS;
}

NTSTATUS
ArcTranslateUdpDevice (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION Information,
    __out PANSI_STRING ArcDeviceBuffer
    )

/*++

Routine Description:

    This routine will translate the file path to a boot environment UDP device
    into an arc device path.

Arguments:

    DeviceId - Supplies a handle to the UDP device.

    Information - Supplies a pointer to the device information for the UDP
        device with the given ID.

    ArcDeviceBuffer - Supplies a buffer in which to return the arc device path
        name for the specified UDP device.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    UNREFERENCED_PARAMETER(DeviceId);
    UNREFERENCED_PARAMETER(Information);

    ASSERT(Information->DeviceType == DEVICE_UDP);

    strcpy_s(ArcDeviceBuffer->Buffer,
             ArcDeviceBuffer->MaximumLength - 1,
             "net(0)");

    ArcDeviceBuffer->Length = (USHORT)strlen(ArcDeviceBuffer->Buffer);
    return STATUS_SUCCESS;
}

NTSTATUS
ArcConvertOptionToSubString (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in PBOOT_ENTRY_OPTION Option,
    __out_ecount_part_opt(BufferLength,*StringLength) PCHAR OptionString,
    __in ULONG BufferLength,
    __out PULONG StringLength
    )

/*++

Routine Description:

    This routine converts a boot option to the corresponding string to be
    included in the OS load options string passed to the kernel.  The load
    options string is in legacy boot.ini format.  If there is insufficient
    space in the destination buffer for the converted string, the size required
    for the string is still returned.

Arguments:

    OptionList - Supplies a pointer to the full boot option list.

    Option - Supplies a pointer to the boot option to be converted.

    OptionString - Supplies an optional pointer to a buffer that receives the
        converted string.

    BufferLength - Supplies the length, in characters, of the destination
        buffer.

    StringLength - Supplies a pointer to a variable that receives the length,
        in characters, of the converted string.

Return Value:

    STATUS_SUCCESS if the conversion succeeded.

    STATUS_BUFFER_TOO_SMALL if there was insufficient space in the destination
        buffer for the converted string.

--*/

{

    PBCDE_BOOLEAN BooleanOption;
    BOOLEAN BooleanOptionValue;
    BCDE_CONFIG_ACCESS_POLICY ConfigAccessPolicy;
    PVOID Data;
    BOOLEAN DebugEquals;
    BOOLEAN DebuggerEnabled;
    BCDE_DEBUGGER_START_POLICY DebuggerPolicy;
    BCDE_DEBUGGER_TYPE DebuggerType;
    BCDE_FIRST_MEGABYTE_POLICY FirstMegabytePolicy;
    BCDE_HYPERVISOR_DEBUGGER_TYPE HypervisorDebuggerType;
    BCDE_HYPERVISOR_IOMMU_POLICY HypervisorIommuPolicy;
    BCDE_HYPERVISOR_LAUNCH_TYPE HypervisorLaunchType;
    BOOLEAN IgnoreUsermodeExceptions;
    ULONGLONG Integer;
    PBCDE_INTEGER IntegerOption;
    ULONGLONG IntegerOptionValue;
    CHAR IntString[sizeof("XXXXXXXXXXXXXXXXXXXXX")];
    BCDE_MSI_POLICY MsiPolicy;
    ULONG NameLength;
    BCDE_NX_POLICY NxPolicy;
    BCDE_PAE_POLICY PaePolicy;
    BCDE_PCI_EXPRESS_POLICY PciExpressPolicy;
    BOOLEAN EventLoggingEnabled;
    ULONG RequiredSize;
    BCDE_SAFEBOOT_TYPE SafeBootType;
    NTSTATUS Status;
    PCHAR String;
    PBCDE_STRING StringOption;
    PCWSTR StringOptionValue;
    ULONG StringOptionValueLength;
    BCDE_TSC_SYNC_POLICY TscSyncPolicy;
    BOOLEAN UseAlternateShell;
    ULONG ValueLength;
    PCSTR ValueString;
    BCDE_VSM_LAUNCH_TYPE VsmLaunchType;
    BCDE_X2APIC_POLICY X2ApicPolicy;

    ASSERT(Option->Invalid == FALSE);

    BooleanOptionValue = FALSE;
    IntegerOptionValue = 0;
    StringOptionValue = NULL;
    StringOptionValueLength = 0;

    Data = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));

    //
    // Pre-read the value of the option, and check with the callback
    // if it should have a different value.
    //

    switch (GET_BCDE_DATA_FORMAT(Option->Type))
    {
    case BCDE_FORMAT_BOOLEAN:
        BooleanOption = (PBCDE_BOOLEAN)Data;
        BooleanOptionValue = BooleanOption->Value;

        Status = BlpBootOptionCallbackBoolean(STATUS_SUCCESS,
                                                 BCD_CALLBACK_OPERATION_GET,
                                                 BlGetApplicationIdentifier(),
                                                 Option->Type,
                                                 &BooleanOptionValue);
        break;

    case BCDE_FORMAT_INTEGER:
        IntegerOption = (PBCDE_INTEGER)Data;
        IntegerOptionValue = IntegerOption->Value;

        Status = BlpBootOptionCallbackULongLong(STATUS_SUCCESS,
                                                   BCD_CALLBACK_OPERATION_GET,
                                                   BlGetApplicationIdentifier(),
                                                   Option->Type,
                                                   &IntegerOptionValue);
        break;

    case BCDE_FORMAT_STRING:
        StringOption = (PBCDE_STRING)Data;
        StringOptionValue = StringOption->String;
        StringOptionValueLength = (ULONG)wcslen(StringOptionValue);

        Status = BlpBootOptionCallbackString(STATUS_SUCCESS,
                                                BCD_CALLBACK_OPERATION_GET,
                                                BlGetApplicationIdentifier(),
                                                Option->Type,
                                                StringOptionValue,
                                                StringOptionValueLength,
                                                &StringOptionValue,
                                                &StringOptionValueLength);
        break;

    default:
        //
        // Add BDCE_FORMAT_DEVICE and other format types if this function is
        // extended to handle other types.
        //

        Status = STATUS_SUCCESS;
        break;
    }

    if (!NT_SUCCESS(Status)) {
        *StringLength = 0;

        //
        // If the filter reports that the item is blocked, just return a
        // length of zero and report success.
        //

        if (Status == STATUS_NOT_FOUND) {
            Status = STATUS_SUCCESS;
        }

        goto ConvertOptionToSubStringEnd;
    }

    //
    // Convert the option based on its type.  Convert all strings in upper case
    // and without leading slashes.
    //

    switch (Option->Type) {

        //
        // WinPE boot case.  This boolean option, if enabled, corresponds to
        // the /MININT switch.
        //

    case BCDE_OSLOADER_TYPE_WINPE:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/MININT") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/MININT", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // No low memory case.  This boolean option, if enabled, corresponds to
        // the /NOLOWMEMORY switch.
        //

    case BCDE_OSLOADER_TYPE_NO_LOW_MEMORY:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NOLOWMEM") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NOLOWMEM", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // No access bit replacement case.  This boolean option, if enabled,
        // corresponds to the /NOACCESSBITREPLACEMENT switch.
        //

    case BCDE_OSLOADER_TYPE_NO_ACCESS_BIT_REPLACEMENT:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NOACCESSBITREPLACEMENT") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NOACCESSBITREPLACEMENT", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // VGA driver enable case.  This boolean option, if enabled, corresponds
        // to the /BASEVIDEO switch.
        //

    case BCDE_OSLOADER_TYPE_USE_VGA_DRIVER:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/BASEVIDEO") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/BASEVIDEO", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Prevent display during OS boot case.  This boolean option, if
        // enabled, corresponds to the /NOGUIBOOT and /BOOTLOGO switches.
        //

    case BCDE_OSLOADER_TYPE_DISABLE_BOOT_DISPLAY:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NOGUIBOOT /BOOTLOGO") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NOGUIBOOT /BOOTLOGO", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Expose MAXIMUM_PROCESSORS outside the kernel.
        // This boolean option, if enabled, corresponds
        // to the /MAXPROC switch.
        //

    case BCDE_OSLOADER_TYPE_MAXIMUM_PROCESSORS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/MAXPROC") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/MAXPROC", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Disable VESA BIOS calls case.  This boolean option, if enabled,
        // corresponds to the /NOVESA switch.
        //

    case BCDE_OSLOADER_TYPE_DISABLE_VESA_BIOS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NOVESA") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NOVESA", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Disable VGA modes case.  This boolean option, if enabled,
        // corresponds to the /NOVGA switch.
        //

    case BCDE_OSLOADER_TYPE_DISABLE_VGA_MODE:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NOVGA") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NOVGA", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Use physical APIC destination mode case.  This boolean option, if
        // enabled, corresponds to the /USEPHYSICALAPIC switch.
        //

    case BCDE_OSLOADER_TYPE_USE_PHYSICAL_DESTINATION:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/USEPHYSICALAPIC") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/USEPHYSICALAPIC", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Restrict system to boot processor case.  This boolean option, if
        // enabled, corresponds to the /ONECPU switch.
        //

    case BCDE_OSLOADER_TYPE_BOOT_PROCESSOR_ONLY:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/ONECPU") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/ONECPU", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Restrict system to BIOS-configured PCI resources case.  This boolean
        // option, if enabled, corresponds to the /PCILOCK switch.
        //

    case BCDE_OSLOADER_TYPE_USE_FIRMWARE_PCI_SETTINGS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/PCILOCK") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/PCILOCK", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Create a system initialization log case.  This boolean option, if
        // enabled, corresponds to the /BOOTLOG switch.
        //

    case BCDE_OSLOADER_TYPE_LOG_INITIALIZATION:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/BOOTLOG") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/BOOTLOG", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Break into the debugger during HAL init case.  This boolean option,
        // if enabled, corresponds to the /BOOTLOG switch.
        //

    case BCDE_OSLOADER_TYPE_DEBUGGER_HAL_BREAKPOINT:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/BREAK") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/BREAK", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Force platform clock source case.  This boolean option,
        // if enabled, corresponds to the /USEPLATFORMCLOCK switch.
        //

    case BCDE_OSLOADER_TYPE_USE_PLATFORM_CLOCK:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/USEPLATFORMCLOCK") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/USEPLATFORMCLOCK", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // NX Policy case.  This option specifies the NX policy in use on the
        // system.  This corresponds to the /NOEXECUTE= and /EXECUTE switches,
        // depending on the active policy.
        //

    case BCDE_OSLOADER_TYPE_NX_POLICY:
        NxPolicy = (BCDE_NX_POLICY)IntegerOptionValue;
        if (NxPolicy == NxPolicyOptIn) {
            String = "/NOEXECUTE=OPTIN";

        } else if (NxPolicy == NxPolicyOptOut) {
            String = "/NOEXECUTE=OPTOUT";

        } else if (NxPolicy == NxPolicyAlwaysOff) {
            String = "/EXECUTE /NOEXECUTE=ALWAYSOFF";

        } else if (NxPolicy == NxPolicyAlwaysOn) {
            String = "/NOEXECUTE=ALWAYSON";

        } else {
            String = NULL;
        }

        if (String != NULL) {
            RequiredSize = (ULONG)strlen(String);
            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString, String, RequiredSize);
            }

            *StringLength = RequiredSize;

        } else {
            *StringLength = 0;
        }

        break;

        //
        // PAE Policy case.  The option specifies the PAE policy in use on the
        // system.  This corresponds to the /PAE or /NOPAE switches, depending
        // on the active policy.  A policy of "default" indicates that no
        // switches are required.
        //

    case BCDE_OSLOADER_TYPE_PAE_POLICY:
        PaePolicy = (BCDE_PAE_POLICY)IntegerOptionValue;
        if (PaePolicy == PaePolicyDefault) {
            String = NULL;

        } else if (PaePolicy == PaePolicyForceEnable) {
            String = "/PAE";

        } else if (PaePolicy == PaePolicyForceDisable) {
            String = "/NOPAE";

        } else {
            String = NULL;
        }

        if (String != NULL) {
            RequiredSize = (ULONG)strlen(String);
            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString, String, RequiredSize);
            }

            *StringLength = RequiredSize;

        } else {
            *StringLength = 0;
        }

        break;

        //
        // Truncate physical memory case.  This option specifies the largest
        // physical address, in megabytes, to allow to be used by the OS.  This
        // corresponds to the /MAXMEM= switch, with the megabyte value specified
        // as a decimal integer after the equals sign.
        //

    case BCDE_LIBRARY_TYPE_TRUNCATE_PHYSICAL_MEMORY:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/MAXMEM=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/MAXMEM=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Avoid low memory case. This option specifies the minimum physical
        // address the OS loader should use during initialization. The megabyte
        // value is specified as a decimal integer after the equals sign.
        //

    case BCDE_LIBRARY_TYPE_AVOID_LOW_PHYSICAL_MEMORY:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/AVOIDLOWMEMORY=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/AVOIDLOWMEMORY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Cluster mode addressing case.  This option specifies the number of
        // processors that are to be included in a single APIC cluster.  This
        // corresponds to the /MAXPROCSPERCLUSTER= switch, with the number of
        // processors specified as a decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_CLUSTERMODE_ADDRESSING:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/MAXPROCSPERCLUSTER=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/MAXPROCSPERCLUSTER=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Restrict APIC cluster case.  This option specifies the largest APIC
        // cluster number to be used on the system.  This corresponds to the
        // /MAXAPICCLUSTER= switch, with the number of clusters specified as a
        // decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_RESTRICT_APIC_CLUSTER:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/MAXAPICCLUSTER=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/MAXAPICCLUSTER=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Use legacy APIC mode case.  This boolean option, if
        // enabled, corresponds to the /USELEGACYAPICMODE switch.
        //

    case BCDE_OSLOADER_TYPE_USE_LEGACY_APIC_MODE:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/USELEGACYAPICMODE") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/USELEGACYAPICMODE", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // TSC Synchronization policy case.  This is an integer policy.
        //

    case BCDE_OSLOADER_TYPE_TSC_SYNC_POLICY:
        TscSyncPolicy = (BCDE_TSC_SYNC_POLICY)IntegerOptionValue;
        switch (TscSyncPolicy) {
        case TscSyncPolicyLegacy:
            ValueString = "LEGACY";
            break;

        case TscSyncPolicyEnhanced:
            ValueString = "ENHANCED";
            break;

        case TscSyncPolicyDefault:
        default:
            ValueString = NULL;
            break;

        }

        if (ValueString == NULL) {
            *StringLength = 0;
            break;
        }

        NameLength = sizeof("/TSCSYNCPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/TSCSYNCPOLICY=",
                          NameLength);

            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // X2APIC policy case.  This is an integer policy.
        //

    case BCDE_OSLOADER_TYPE_X2APIC_POLICY:
        X2ApicPolicy = (BCDE_X2APIC_POLICY)IntegerOptionValue;
        switch (X2ApicPolicy) {
        case X2APICPolicyDisable:
            ValueString = "DISABLE";
            break;

        case X2APICPolicyEnable:
            ValueString = "ENABLE";
            break;

        case X2APICPolicyDefault:
        default:
            ValueString = "DEFAULT";
            break;
        }

        NameLength = sizeof("/X2APICPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/X2APICPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Number of processors case.  This option specifies the maximum number
        // of processors to utilize on the system.  This corresponds to the
        // /NUMPROC= switch, with the number of processors specified as a
        // decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_NUMBER_OF_PROCESSORS:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/NUMPROC=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/NUMPROC=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Maximizing the number of groups created case. This boolean option
        // indicates the manner in which NUMA nodes are assigned to processor
        // groups. Possible values are On and Off.
        //

    case BCDE_OSLOADER_TYPE_MAXIMIZE_GROUPS_CREATED:
        if (BooleanOptionValue != FALSE) {
            String = "/MAXGROUP=ON";

        } else {
            String = "/MAXGROUP=OFF";
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Force processor group awareness case.  This boolean option, if
        // enabled, corresponds to the /FORCEGROUPAWARE switch.
        //

    case BCDE_OSLOADER_TYPE_FORCE_GROUP_AWARENESS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/FORCEGROUPAWARE") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/FORCEGROUPAWARE", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Group size case.  This option specifies the size of all processor
        // groups. This corresponds to the /GROUPSIZE= switch, with the group
        // size specified as a decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_GROUP_SIZE:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/GROUPSIZE=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/GROUPSIZE=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Processor configuration flags case.  This option specifies the
        // configuration flags for the processor, used to specify specific
        // errata workarounds in the processor.  This corresponds to the
        // /CONFIGFLAGS= switch, with the configuration flag mask specified as
        // a decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_PROCESSOR_CONFIGURATION_FLAGS:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/CONFIGFLAG=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/CONFIGFLAG=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Remove memory from use case.  This option specifies an amount of
        // memory, in megabytes, to remove from use by the OS.  This corresponds
        // to the /BURNMEMORY= switch, with the amount of memory to burn
        // specified as a decimal number after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_REMOVE_MEMORY:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/BURNMEMORY=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/BURNMEMORY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Increase user-mode VA space case.  This option specifies an amount
        // of virtual address space, in megabytes, that is to be used for
        // user-mode.  The /3GB switch must be added to indicate to the OS that
        // a user VA space override is present.  With this switch in place, the
        // /USERVA= switch is used to communicate the exact amount of VA space
        // to assign.
        //

    case BCDE_OSLOADER_TYPE_INCREASE_USER_VA:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/3GB /USERVA=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/3GB /USERVA=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Performance data buffer memory case.  This option specifies a size
        // in megabytes of a buffer to be allocated early in boot to be used for
        // storing performance related information.
        //

    case BCDE_OSLOADER_TYPE_PERFORMANCE_DATA_MEMORY:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/PERFMEM=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/PERFMEM=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Kernel binary path case.  This option specifies a system32-relative
        // path to the kernel in use on the system, overriding the default
        // kernel name.  This corresponds to the /KERNEL= switch.
        //

    case BCDE_OSLOADER_TYPE_KERNEL_PATH:
        NameLength = sizeof("/KERNEL=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/KERNEL=", NameLength);

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the KERNEL= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // HAL binary path case.  This option specifies a system32-relative
        // path to the HAL in use on the system, overriding the default HAL
        // name.  This corresponds to the /HAL= switch.
        //

    case BCDE_OSLOADER_TYPE_HAL_PATH:
        NameLength = sizeof("/HAL=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HAL=", NameLength);

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the KERNEL= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Safe boot case.  This option specifies that the system is to boot
        // in safe mode.  The option data specifies the type of safe boot in
        // use.  This corresponds to the /SAFEBOOT: switch, with the type of
        // safe boot in a string after the colon.
        //

    case BCDE_OSLOADER_TYPE_SAFEBOOT:
        SafeBootType = (BCDE_SAFEBOOT_TYPE)IntegerOptionValue;
        if (SafeBootType == SafemodeMinimal) {

            //
            // Minimal safe boot is the normal safe boot mode.  A related option
            // is whether a command-prompt-only shell is to be used for safe
            // mode.  Alter the safe boot mode string based on the presence of
            // this option.
            //

            Status = BlGetBootOptionBoolean(
                         OptionList,
                         BCDE_OSLOADER_TYPE_SAFEBOOT_ALTERNATE_SHELL,
                         &UseAlternateShell);

            if (NT_SUCCESS(Status) && (UseAlternateShell != FALSE)) {
                String = "MINIMAL(ALTERNATESHELL)";

            } else {
                String = "MINIMAL";
            }

        } else if (SafeBootType == SafemodeNetwork) {
            String = "NETWORK";

        } else if (SafeBootType == SafemodeDsRepair) {
            String = "DSREPAIR";

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        NameLength = sizeof("/SAFEBOOT:") - 1;
        ValueLength = (ULONG)strlen(String);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/SAFEBOOT:", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          String,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Boot debugger enabled case.  This boolean option, if enabled,
        // corresponds to the /BOOTDEBUG switch.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_ENABLED:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/BOOTDEBUG") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/BOOTDEBUG", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Kernel debugger enabled case.  This boolean option, if enabled,
        // corresponds to the /DEBUG switch.
        //

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED:
        if (BooleanOptionValue == FALSE) {
            RequiredSize = sizeof("/NODEBUG") - 1;

            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString, "/NODEBUG", RequiredSize);
            }

            *StringLength = RequiredSize;
            break;
        }

        //
        // Obtain the BCD elements that correspond to the /DEBUG switch
        // arguments.  Use them to calculate how long the buffer should be.
        //

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_LIBRARY_TYPE_DEBUGGER_START_POLICY,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            DebuggerPolicy = DebuggerStartPolicyActive;

        } else {
            DebuggerPolicy = Integer;
        }

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_LIBRARY_TYPE_DEBUGGER_IGNORE_USERMODE_EXCEPTIONS,
                     &IgnoreUsermodeExceptions);

        if (!NT_SUCCESS(Status)) {
            IgnoreUsermodeExceptions = FALSE;
        }

        RequiredSize = sizeof("/DEBUG") - 1;
        if (DebuggerPolicy == DebuggerStartPolicyAutoEnable) {
            RequiredSize += sizeof("=AUTOENABLE") - 1;

        } else if (DebuggerPolicy == DebuggerStartPolicyDisable) {
            RequiredSize += sizeof("=DISABLE") - 1;
        }

        if (IgnoreUsermodeExceptions != FALSE) {
            RequiredSize += sizeof("=NOUMEX") - 1;
        }

        if (RequiredSize <= BufferLength) {
            DebugEquals = FALSE;
            RtlCopyMemory(OptionString, "/DEBUG", sizeof("/DEBUG") - 1);

            //
            // Keep track of the end of the option string.  This is where any
            // continuation of the option will be placed.
            //

            String = Add2Ptr(OptionString, sizeof("/DEBUG") - 1);
            if (DebuggerPolicy == DebuggerStartPolicyAutoEnable) {
                RtlCopyMemory(String, "=AUTOENABLE", sizeof("=AUTOENABLE") - 1);
                String = Add2Ptr(String, sizeof("=AUTOENABLE") - 1);
                DebugEquals = TRUE;

            } else if (DebuggerPolicy == DebuggerStartPolicyDisable) {
                RtlCopyMemory(String, "=DISABLE", sizeof("=DISABLE") - 1);
                String = Add2Ptr(String, sizeof("=DISABLE") - 1);
                DebugEquals = TRUE;
            }

            if (IgnoreUsermodeExceptions != FALSE) {
                if (DebugEquals == FALSE) {
                    RtlCopyMemory(String, "=NOUMEX", sizeof("=NOUMEX") - 1);
                    String = Add2Ptr(String, sizeof("=NOUMEX") - 1);
                    DebugEquals = TRUE;

                } else {
                    RtlCopyMemory(String, ",NOUMEX", sizeof(",NOUMEX") - 1);
                    String = Add2Ptr(String, sizeof(",NOUMEX") - 1);
                }
            }
        }

        *StringLength = RequiredSize;
        break;

        //
        // Kernel debugger type case.  This option indicates the type of
        // debugging in use on the system: Serial, 1394, USB, NET, or Local.
        // For serial debugging this option does not generate a load option
        // string.  Serial debugging is the default, so no type specifier is
        // necessary.  Otherwise, this option corresponds to the /DEBUGPORT=
        // switch, with the type string after the equals sign.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_TYPE:

        //
        // If there's a kernel debugger type override, then simply just defer
        // to that.
        //

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE:        
        DebuggerType = (BCDE_DEBUGGER_TYPE)IntegerOptionValue;

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(OptionList,
                                            BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                            &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) ||
                (EventLoggingEnabled == FALSE) ||
                (DebuggerType != DebuggerNet)) {

                *StringLength = 0;
                break;
            }
        }
        
        if (DebuggerType == Debugger1394) {
            String = "/DEBUGPORT=1394";

        } else if (DebuggerType == DebuggerUsb) {
            String = "/DEBUGPORT=USB";

        } else if (DebuggerType == DebuggerNet) {
            String = "/DEBUGPORT=NET";

        } else if (DebuggerType == DebuggerLocal) {
            String = "/DEBUGPORT=LOCAL";

        } else if (DebuggerType == DebuggerSerial) {
            *StringLength = 0;
            break;

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Serial debugger port number case.  If serial debugging is in use,
        // this option corresponds to the /DEBUGPORT=COMx switch, with the
        // com port number replacing the x in the string.  If serial debugging
        // is not in use, this option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_PORT_NUMBER:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionInteger(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_PORT_NUMBER,
                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_PORT_NUMBER:

        //
        // Ensure that kernel debugging is both enabled, and using the serial
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerSerial) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/DEBUGPORT=COM") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/DEBUGPORT=COM", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Serial debugger baudrate case.  If serial debugging is in use,
        // this option corresponds to the /BAUDRATE= switch, with the
        // baudrate specified as a decimal number after the equals sign.  If
        // serial debugging is not in use, this option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_BAUDRATE:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionInteger(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_BAUDRATE,
                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_BAUDRATE:

        //
        // Ensure that kernel debugging is both enabled, and using the serial
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerSerial) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/BAUDRATE=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/BAUDRATE=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Kernel debugging 1394 channel case.  If 1394 kernel debugging is
        // enabled, this option corresponds to the /CHANNEL= switch, with
        // the 1394 channel to be used for debugging specified as a decimal
        // number after the equals sign.  If 1394 debugging is not in use, this
        // option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_1394_CHANNEL:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionInteger(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_1394_CHANNEL,
                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_1394_CHANNEL:

        //
        // Ensure that kernel debugging is both enabled, and using the 1394
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != Debugger1394) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/CHANNEL=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/CHANNEL=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Kernel debugging USB target name case.  If USB kernel debugging is
        // enabled, this option corresponds to the /targetname= switch, with
        // the usb target name to be used for debugging specified as a string
        // after the equals sign.  If USB debugging is not in use, this
        // option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_USB_TARGETNAME:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionString(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_USB_TARGETNAME,
                        (PWSTR*)&StringOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_USB_TARGETNAME:

        //
        // Ensure that kernel debugging is both enabled, and using the USB
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerUsb) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        NameLength = sizeof("/TARGETNAME=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/TARGETNAME=", NameLength);

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the TARGETNAME= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Net debugger IP address case.  If network debugging is in use,
        // this option corresponds to the /HOST_IP= switch, with the host IP
        // address specified as a sequence of 4 decimal numbers separated by
        // "." after the equals sign.  If network debugging is not in use, this
        // option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_NET_HOST_IP:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionInteger(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_HOST_IP,
                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_HOST_IP:

        //
        // Ensure that kernel debugging is both enabled, and using the network
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                        &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(OptionList,
                                            BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                            &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) || (EventLoggingEnabled == FALSE)) {
                *StringLength = 0;
                break;
            }
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerNet) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s((UCHAR)(IntegerOptionValue >> 24),
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HOST_IP=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HOST_IP=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;

        _i64toa_s((UCHAR)(IntegerOptionValue >> 16),
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof(".") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((*StringLength + NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(Add2Ptr(OptionString, *StringLength), ".", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, (*StringLength + NameLength)),
                          IntString,
                          ValueLength);
        }

        *StringLength += NameLength + ValueLength;

        _i64toa_s((UCHAR)(IntegerOptionValue >> 8),
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof(".") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((*StringLength + NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(Add2Ptr(OptionString, *StringLength), ".", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, (*StringLength + NameLength)),
                          IntString,
                          ValueLength);
        }

        *StringLength += NameLength + ValueLength;

        _i64toa_s((UCHAR)IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof(".") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((*StringLength + NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(Add2Ptr(OptionString, *StringLength), ".", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, (*StringLength + NameLength)),
                          IntString,
                          ValueLength);
        }

        *StringLength += NameLength + ValueLength;

        break;

        //
        // Net debugger port case.  If network debugging is in use,
        // this option corresponds to the /HOST_PORT= switch, with the
        // port specified as a decimal number after the equals sign.  If
        // network debugging is not in use, this option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_NET_PORT:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionInteger(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_HOST_PORT,
                        &IntegerOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_HOST_PORT:

        //
        // Ensure that kernel debugging is both enabled, and using the network
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                        &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(OptionList,
                                            BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                            &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) || (EventLoggingEnabled == FALSE)) {
                *StringLength = 0;
                break;
            }
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerNet) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HOST_PORT=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HOST_PORT=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Net debugger encryption key case.  If network debugging is in use,
        // this option corresponds to the /ENCRYPTION_KEY= switch.  If
        // network debugging is not in use, this option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_NET_KEY:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionString(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_KEY,
                        (PWSTR*)&StringOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_KEY:

        //
        // Ensure that kernel debugging is both enabled, and using the network
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                        &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(OptionList,
                                            BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                            &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) || (EventLoggingEnabled == FALSE)) {
                *StringLength = 0;
                break;
            }
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerNet) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        NameLength = sizeof("/ENCRYPTION_KEY=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/ENCRYPTION_KEY=", NameLength);

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the ENCRYPTION_KEY= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Net debugger DHCP case.  If network debugging is in use,
        // this option corresponds to the /NO_DHCP switch.  If
        // network debugging is not in use, this option is ignored.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_NET_DHCP:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionBoolean(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_DHCP,
                        &BooleanOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_NET_DHCP:

        //
        // Ensure that kernel debugging is both enabled, and using the network
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                                        &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(OptionList,
                                            BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                                            &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) || (EventLoggingEnabled == FALSE)) {
                *StringLength = 0;
                break;
            }
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType != DebuggerNet) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.  Only if DHCP is set false do we add the NODHCP
        // string to the loader block boot parameters.
        //

        if (BooleanOptionValue != FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/NO_DHCP") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/NO_DHCP", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Kernel redirection enabled case.  This boolean option, if enabled,
        // corresponds to the /REDIRECT switch.  Redirection options, including
        // com port and baudrate, are not specified in the load options string
        // since historically these have been specified in the global section
        // of boot.ini and were therefore not included in the OS load options
        // string.
        //

    case BCDE_OSLOADER_TYPE_KERNEL_EMS_ENABLED:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/REDIRECT") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/REDIRECT", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Load options string case.  This string option contains a legacy
        // load options string in boot.ini format.  This may be appended to the
        // boot entry by the boot manager if boot.ini was used to enumerate the
        // boot entries, to avoid converting this string into boot options only
        // to be converted back again.  It may also be set by software that
        // must set a boot.ini switch that is unknown to this routine for
        // consumption by a non-BCD-aware piece of code like a 3rd party driver.
        //
        // In the case where boot.ini is used to enumerate boot entries, some
        // of the boot.ini switches are converted to boot options because they
        // must be processed by the boot library or this application.  In this
        // case the OS load options string created by this routine will contain
        // duplication, but this should not cause an issue.
        //

    case BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING:
        ValueLength = StringOptionValueLength;
        if (ValueLength <= BufferLength) {

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the KERNEL= switch.
            //

            BlpCopyAndSanitizeUnicodeToAnsiString(OptionString,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = ValueLength;
        break;

        //
        // FVE KeyRing case.  This is used to pass a /FVEBOOT option to the
        // FVEVOL.SYS driver so that it knows where to find the keyring.
        //
        // FUTURE-2006/05/30-JamieHun FVEVOL should use BCD data directly.
        //     At a future time when drivers can access BCD data directly to
        //     retrieve settings, or if the method of communicating the keyring
        //     address is otherwise changed, this method of communicating the
        //     keyring address can and should be removed.
        //

    case BCDE_LIBRARY_TYPE_FVE_KEYRING_ADDRESS:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/FVEBOOT=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/FVEBOOT=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);

        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Graphics resolution case.
        //

    case BCDE_LIBRARY_TYPE_GRAPHICS_RESOLUTION:
        if (IntegerOptionValue == GraphicsResolution1024x768) {
            RequiredSize = sizeof("/GRAPHICSRESOLUTION=1024x768") - 1;
            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString,
                              "/GRAPHICSRESOLUTION=1024x768",
                              RequiredSize);
            }

        } else if (IntegerOptionValue == GraphicsResolution800x600) {
            RequiredSize = sizeof("/GRAPHICSRESOLUTION=800x600") - 1;
            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString,
                              "/GRAPHICSRESOLUTION=800x600",
                              RequiredSize);
            }

        } else if (IntegerOptionValue == GraphicsResolution1024x600) {
            RequiredSize = sizeof("/GRAPHICSRESOLUTION=1024x600") - 1;
            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString,
                              "/GRAPHICSRESOLUTION=1024x600",
                              RequiredSize);
            }

        } else {
            RequiredSize = 0;
        }

        *StringLength = RequiredSize;
        break;

        //
        // Integrity check case.  This boolean option, if enabled, corresponds
        // to the /DISABLE_INTEGRITY_CHECKS switch.
        //

    case BCDE_LIBRARY_TYPE_DISABLE_INTEGRITY_CHECKS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/DISABLE_INTEGRITY_CHECKS") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString,
                            "/DISABLE_INTEGRITY_CHECKS",
                            RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Disable ELAM case. This boolean option, if enabled, corresponds
        // to the /DISABLEELAMDRIVER switch.
        //

    case BCDE_OSLOADER_TYPE_DISABLE_ELAM_DRIVERS:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/DISABLEELAMDRIVERS") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString,
                            "/DISABLEELAMDRIVERS",
                            RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Code integrity test mode case.  This boolean option, if enabled,
        // allows test pre-release test certificate roots.
        //

    case BCDE_LIBRARY_TYPE_ALLOW_PRERELEASE_SIGNATURES:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/TESTSIGNING") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/TESTSIGNING", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Code integrity flight signing case.  This boolean option, if enabled,
        // allows flight signing certificate.
        //

    case BCDE_LIBRARY_TYPE_ALLOW_FLIGHT_SIGNATURES:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/FLIGHTSIGNING") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/FLIGHTSIGNING", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // CD export case.  This boolean option, if enabled, causes the kernel
        // to export the ramdisk as a CDROM.
        //

    case BCDE_DEVICE_TYPE_RAMDISK_EXPORT_AS_CD:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/RDEXPORTASCD") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString,
                          "/RDEXPORTASCD",
                          RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

    case BCDE_LIBRARY_TYPE_FIRST_MEGABYTE_POLICY:
        FirstMegabytePolicy = (BCDE_FIRST_MEGABYTE_POLICY)IntegerOptionValue;

        //
        // Determine the string that will be passed to the kernel. If the value
        // is not valid, then the default is "use none".  This is the default
        // behaviour if this element doesn't appear at all.
        //

        switch (FirstMegabytePolicy) {
        case FirstMegabytePolicyUseNone:
            ValueString = "USENONE";
            break;

        case FirstMegabytePolicyUseAll:
            ValueString = "USEALL";
            break;

        case FirstMegabytePolicyUsePrivate:
            ValueString = "USEPRIVATE";
            break;

        default:
            ValueString = "USENONE";
            break;
        }

        NameLength = sizeof("/FIRSTMEGABYTEPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/FIRSTMEGABYTEPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // PCI configuration access policy case.  This is an integer policy.
        //

    case BCDE_LIBRARY_TYPE_CONFIG_ACCESS_POLICY:
        ConfigAccessPolicy = (BCDE_CONFIG_ACCESS_POLICY)IntegerOptionValue;
        switch (ConfigAccessPolicy) {
        case ConfigAccessPolicyDisallowMmConfig:
            ValueString = "DISALLOWMMCONFIG";
            break;

        case ConfigAccessPolicyDefault:
        default:
            ValueString = "DEFAULT";
            break;
        }

        NameLength = sizeof("/CONFIGACCESSPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/CONFIGACCESSPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // MSI policy case.  This is an integer policy.
        //

    case BCDE_OSLOADER_TYPE_MSI_POLICY:
        MsiPolicy = (BCDE_MSI_POLICY)IntegerOptionValue;
        switch (MsiPolicy) {
        case MsiPolicyForceDisable:
            ValueString = "FORCEDISABLE";
            break;

        case MsiPolicyDefault:
        default:
            ValueString = "DEFAULT";
            break;
        }

        NameLength = sizeof("/MSIPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/MSIPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // PCI express policy case.  This is an integer policy.
        //

    case BCDE_OSLOADER_TYPE_PCI_EXPRESS_POLICY:
        PciExpressPolicy = (BCDE_PCI_EXPRESS_POLICY)IntegerOptionValue;
        switch (PciExpressPolicy) {
        case PciExpressPolicyForceDisable:
            ValueString = "FORCEDISABLE";
            break;

        case PciExpressPolicyDefault:
        default:
            ValueString = "DEFAULT";
            break;
        }

        NameLength = sizeof("/PCIEXPRESSPOLICY=") - 1;
        ValueLength = (ULONG)strlen(ValueString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/PCIEXPRESSPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          ValueString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Hypervisor launch type case.  This is an integer option and
        // indicates the manner in which hypervisor will be launched. Possible
        // values are Off and Auto.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_LAUNCH_TYPE:
        HypervisorLaunchType = (BCDE_HYPERVISOR_LAUNCH_TYPE)IntegerOptionValue;
        if (HypervisorLaunchType == HypervisorLaunchOff) {
            String = "/HYPERVISORLAUNCHTYPE=OFF";

        } else if (HypervisorLaunchType == HypervisorLaunchAuto) {
            String = "/HYPERVISORLAUNCHTYPE=AUTO";

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor debugger enabled case.  This boolean option, if enabled,
        // corresponds to the /HYPERVISORDBG switch.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_ENABLED:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/HYPERVISORDBG") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORDBG", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor debugger type case.  This is an integer option and
        // indicates the type of hypervisor debugger: Serial, 1394, or Net.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_TYPE:
        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        HypervisorDebuggerType = (BCDE_HYPERVISOR_DEBUGGER_TYPE)IntegerOptionValue;
        if (HypervisorDebuggerType == HypervisorDebuggerSerial) {
            String = "/HYPERVISORDBGTYPE=SERIAL";

        } else if (HypervisorDebuggerType == HypervisorDebugger1394) {
            String = "/HYPERVISORDBGTYPE=1394";

        } else if (HypervisorDebuggerType == HypervisorDebuggerNet) {
            String = "/HYPERVISORDBGTYPE=NET";

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor debugger port number for serial debugging. This option
        // corresponds to the /HYPERVISORDBGPORT=COMx switch, with the
        // com port number replacing the x in the string.  If serial debugging
        // is not in use, this option is ignored.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_PORT_NUMBER:

        //
        // Ensure that hypervisor debugging is both enabled and using the serial
        // debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(
                     OptionList,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_TYPE,
                     &Integer);

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        HypervisorDebuggerType = (BCDE_HYPERVISOR_DEBUGGER_TYPE)Integer;
        if (HypervisorDebuggerType != HypervisorDebuggerSerial) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HYPERVISORDBGPORT=COM") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORDBGPORT=COM", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

    case BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_BUSPARAMS:

        //
        // Ensure that hypervisor debugging is both enabled and not using the
        // serial debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            *StringLength = 0;
            break;
        }

        Status = BlGetBootOptionInteger(
                     OptionList,
                     BCDE_OSLOADER_TYPE_HYPERVISOR_DEBUGGER_TYPE,
                     &Integer);

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        HypervisorDebuggerType = (BCDE_HYPERVISOR_DEBUGGER_TYPE)Integer;
        if (HypervisorDebuggerType == HypervisorDebuggerSerial) {
            *StringLength = 0;
            break;
        }

        //
        // Convert the option.
        //

        NameLength = sizeof("/HYPERVISORDBGDEVICE=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORDBGDEVICE=", NameLength);

            //
            // Convert the wide-character string down to a single byte character
            // string appended to the /HYPERVISORDBGDEVICE= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Hypervisor SLAT (nested paging) disabled case.  This boolean option,
        // if enabled, corresponds to the /HYPERVISORDISABLESLAT switch.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_SLAT_DISABLED:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/HYPERVISORDISABLESLAT") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORDISABLESLAT", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor use large vTLB case.  This boolean option,
        // if enabled, corresponds to the /HYPERVISORUSELARGEVTLB switch.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_USE_LARGE_VTLB:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/HYPERVISORUSELARGEVTLB") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORUSELARGEVTLB", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor number of processors case.  This option specifies the
        // maximum number of logical processors to utilize by the hypervisor.
        // This corresponds to the /HYPERVISORNUMPROC= switch, with the number
        // of logical processors specified as a decimal number after the equals
        // sign.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_NUM_PROC:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HYPERVISORNUMPROC=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORNUMPROC=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Number of root virtual processors case.  This option specifies the
        // maximum number of root virtual processors to be created and started,
        // without constraining the number of LPs used by the Hypervisor.  This
        // corresponds to the /HYPERVISORROOTPROC= switch, with the number of
        // root virtual processors specified as a decimal number after the
        // equals sign.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_ROOT_PROC:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HYPERVISORROOTPROC=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORROOTPROC=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Number of root virtual processors per NUMA node case.  This option
        // specifies the number of virtual processors to be started in
        // the root partition on each NUMA node.
        // This corresponds to the /HYPERVISORROOTPROCPERNODE= switch, with
        // the number of root virtual processors specified as a decimal number
        // after the equals sign.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_ROOT_PROC_PER_NODE:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/HYPERVISORROOTPROCPERNODE=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORROOTPROCPERNODE=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Hypervisor IOMMU policy. The options have the following behavior -
        // Default - Enabled unless an error condition is encountered during
        //      initialization.
        // Enable - Enabled unless a fatal error condition is encountered
        //      during initialization. Some errors can be ignored with potential
        //      loss of functionality and are not considered fatal under this
        //      setting.
        // Disable - Always disabled.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_IOMMU_POLICY:
        HypervisorIommuPolicy = (BCDE_HYPERVISOR_IOMMU_POLICY)IntegerOptionValue;
        if (HypervisorIommuPolicy == HypervisorIommuPolicyDefault) {
            String = "/HYPERVISORIOMMUPOLICY=DEFAULT";

        } else if (HypervisorIommuPolicy == HypervisorIommuPolicyEnable) {
            String = "/HYPERVISORIOMMUPOLICY=ENABLE";

        } else if (HypervisorIommuPolicy == HypervisorIommuPolicyDisable) {
            String = "/HYPERVISORIOMMUPOLICY=DISABLE";

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Hypervisor full APIC virtualization enabled case.  This boolean
        // option, if enabled, corresponds to the /HYPERVISORUSEVAPIC
        // switch.
        //

    case BCDE_OSLOADER_TYPE_HYPERVISOR_USE_VAPIC:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/HYPERVISORUSEVAPIC") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/HYPERVISORUSEVAPIC", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // VSM launch type case.  This is an integer option and indicates the
        // manner in which VSM will be launched. Possible values are Off and
        // Auto.
        //

    case BCDE_OSLOADER_TYPE_VSM_LAUNCH_TYPE:
        VsmLaunchType = (BCDE_VSM_LAUNCH_TYPE)IntegerOptionValue;
        if (VsmLaunchType == VsmLaunchOff) {
            String = "/VSMLAUNCHTYPE=OFF";

        } else if (VsmLaunchType == VsmLaunchAuto) {
            String = "/VSMLAUNCHTYPE=AUTO";

        } else {
            return STATUS_INVALID_PARAMETER;
        }

        RequiredSize = (ULONG)strlen(String);
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, String, RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // XSave Policy case.  This is an integer option and indicates the
        // policy to be used.
        //

    case BCDE_OSLOADER_TYPE_XSAVEPOLICY:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/XSAVEPOLICY=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEPOLICY=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 0 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE0:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE0=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE0=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 1 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE1:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE1=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE1=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 2 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE2:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE2=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE2=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 3 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE3:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE3=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE3=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 4 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE4:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE4=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE4=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 5 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE5:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE5=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE5=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 6 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE6:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE6=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE6=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave add feature 7 case.  This is an integer option and indicates
        // the feature to be added as well as its associated information.
        //

    case BCDE_OSLOADER_TYPE_XSAVEADDFEATURE7:
        _ui64toa_s(IntegerOptionValue,
                   IntString,
                   sizeof(IntString),
                   10);

        NameLength = sizeof("/XSAVEADDFEATURE7=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEADDFEATURE7=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave remove feature case.  This is an integer option and indicates
        // a mask of which features to remove.
        //

    case BCDE_OSLOADER_TYPE_XSAVEREMOVEFEATURE:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/XSAVEREMOVEFEATURE=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEREMOVEFEATURE=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave remove feature case.  This is an integer option and indicates
        // a mask of which features to remove.
        //

    case BCDE_OSLOADER_TYPE_XSAVEPROCESSORSMASK:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/XSAVEPROCESSORSMASK=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEPROCESSORSMASK=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // XSave remove feature case.  This is an integer option and indicates
        // whether or not XSave should be disabled on this system.
        //

    case BCDE_OSLOADER_TYPE_XSAVEDISABLE:
        _i64toa_s(IntegerOptionValue,
                  IntString,
                  sizeof(IntString),
                  10);

        NameLength = sizeof("/XSAVEDISABLE=") - 1;
        ValueLength = (ULONG)strlen(IntString);
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/XSAVEDISABLE=", NameLength);
            RtlCopyMemory(Add2Ptr(OptionString, NameLength),
                          IntString,
                          ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

        //
        // Debugger bus parameters case.
        //

    case BCDE_LIBRARY_TYPE_DEBUGGER_BUS_PARAMETERS:

        //
        // If there's a kernel debugger override, then simply just defer to
        // that.
        //

        Status = BlGetBootOptionString(
                        OptionList,
                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_BUS_PARAMETERS,
                        (PWSTR*)&StringOptionValue);

        if (NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

    case BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_BUS_PARAMETERS:

        //
        // Ensure that kernel debugging is both enabled, and not using the
        // serial debugging type, before converting the option.
        //

        Status = BlGetBootOptionBoolean(
                     OptionList,
                     BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_ENABLED,
                     &DebuggerEnabled);

        if (!NT_SUCCESS(Status) || (DebuggerEnabled == FALSE)) {
            Status = BlGetBootOptionBoolean(
                         OptionList,
                         BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED,
                         &EventLoggingEnabled);

            if (!NT_SUCCESS(Status) || (EventLoggingEnabled == FALSE)) {
                *StringLength = 0;
                break;
            }
        }

        Status = BlGetBootOptionInteger(OptionList,
                                        BCDE_OSLOADER_TYPE_KERNEL_DEBUGGER_TYPE,
                                        &Integer);

        if (!NT_SUCCESS(Status)) {
            Status = BlGetBootOptionInteger(
                            OptionList,
                            BCDE_LIBRARY_TYPE_DEBUGGER_TYPE,
                            &Integer);
        }

        if (!NT_SUCCESS(Status)) {
            *StringLength = 0;
            break;
        }

        DebuggerType = (BCDE_DEBUGGER_TYPE)Integer;
        if (DebuggerType == DebuggerSerial) {
            *StringLength = 0;
            break;
        }

        NameLength = sizeof("/BUSPARAMS=") - 1;
        ValueLength = StringOptionValueLength;
        if ((NameLength + ValueLength) <= BufferLength) {
            RtlCopyMemory(OptionString, "/BUSPARAMS=", NameLength);

            //
            // Convert the wide-character string down to a single byte
            // character string appended to the BUSPARAMS= switch.
            //

            String = Add2Ptr(OptionString, NameLength);
            BlpCopyAndSanitizeUnicodeToAnsiString(String,
                                                   StringOptionValue,
                                                   ValueLength);
        }

        *StringLength = NameLength + ValueLength;
        break;

    case BCDE_OSLOADER_TYPE_FORCE_LEGACY_PLATFORM:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/FORCELEGACYPLATFORM") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/FORCELEGACYPLATFORM", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Use a platform clock source.  This boolean option,
        // if enabled, corresponds to the /USEPLATFORMTICK switch.
        //

    case BCDE_OSLOADER_TYPE_USE_PLATFORM_TICK:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/USEPLATFORMTICK") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/USEPLATFORMTICK", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Disable dynamic timer tick.  This boolean option,
        // if enabled, corresponds to the /DISABLEDYNAMICTICK switch.
        //

    case BCDE_OSLOADER_TYPE_DISABLE_DYNAMIC_TICK:
        if (BooleanOptionValue == FALSE) {
            *StringLength = 0;
            break;
        }

        RequiredSize = sizeof("/DISABLEDYNAMICTICK") - 1;
        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/DISABLEDYNAMICTICK", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

        //
        // Remote event logging enabled case.  This boolean option, if enabled,
        // corresponds to the /EVENT switch.
        //

    case BCDE_OSLOADER_TYPE_EVENT_LOGGING_ENABLED:
        if (BooleanOptionValue == FALSE) {
            RequiredSize = sizeof("/NOEVENT") - 1;

            if (RequiredSize <= BufferLength) {
                RtlCopyMemory(OptionString, "/NOEVENT", RequiredSize);
            }

            *StringLength = RequiredSize;
            break;
        }

        RequiredSize = sizeof("/EVENT") - 1;

        if (RequiredSize <= BufferLength) {
            RtlCopyMemory(OptionString, "/EVENT", RequiredSize);
        }

        *StringLength = RequiredSize;
        break;

    default:
        *StringLength = 0;
        break;
    }

ConvertOptionToSubStringEnd:
    if (*StringLength > BufferLength) {
        return STATUS_BUFFER_TOO_SMALL;

    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS
ArcConvertStringIntegerToOption(
    __in PWSTR IntegerString,
    __in BCDE_DATA_TYPE Type,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes,
    __out_opt PULONG RequiredBytes
    )

/*++

Routine Description:

    This routine converts a string containing an integer into a boot option with
    the given type that references the integer.

Arguments:

    IntegerString - Supplies a pointer to the string to convert.

    Type - Supplies the data type to apply to the option.

    Option - Supplies a pointer to a buffer that receives the constructed
        option.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given boot entry option pointer.

    RequiredBytes - Supplies an optional number of bytes required to contain the
        the option.

Return Value:

    STATUS_BUFFER_TOO_SMALL if the buffer is too small.

    STATUS_SUCCESS if the routine succeeded.

--*/

{

    ULONG Index;
    PBCDE_INTEGER IntegerElement;
    ULONG Size;
    ULONGLONG Value;

    Size = sizeof(BOOT_ENTRY_OPTION) + sizeof(BCDE_INTEGER);
    if (ARGUMENT_PRESENT(RequiredBytes)) {
        *RequiredBytes = Size;
    }

    if (AvailableBytes < Size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // First convert the integer string to an integer.
    //

    Value = 0;
    Index = 0;
    while (IntegerString[Index] != UNICODE_NULL) {

        //
        // Terminate early if a non-integer value is found in the option
        // string.
        //

        if ((IntegerString[Index] < L'0') || (IntegerString[Index] > L'9')) {
            break;
        }

        Value *= 10;
        Value += (IntegerString[Index] - '0');
        Index += 1;
    }

    //
    // Fill in the option.
    //

    RtlZeroMemory(Option, Size);
    Option->Type = Type;
    IntegerElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
    Option->DataOffset = (ULONG)PtrOffset(Option, IntegerElement);
    Option->DataSize = sizeof(BCDE_INTEGER);
    IntegerElement->Value = Value;
    return STATUS_SUCCESS;
}

NTSTATUS
ArcConvertStringToOption (
    __in PWSTR String,
    __in BOOLEAN WhitespaceTerminated,
    __in BCDE_DATA_TYPE Type,
    __inout_bcount(AvailableBytes) PBOOT_ENTRY_OPTION Option,
    __in ULONG AvailableBytes,
    __out_opt PULONG RequiredBytes
    )

/*++

Routine Description:

    This routine converts a string into a boot option with the given type that
    references the string.

Arguments:

    String - Supplies a pointer to the string to convert.

    WhitespaceTerminated - Supplies a boolean value indicating whether the input
        string is to be terminated at the first space character.  If false, the
        input string is terminated at NULL.

    Type - Supplies the data type to apply to the option.

    Option - Supplies a pointer to a buffer that receives the constructed
        option.

    AvailableBytes - Supplies the number of bytes of available buffer that
        follow the given boot entry option pointer.

    RequiredBytes - Supplies an optional number of bytes required to contain the
        option.

Return Value:

    STATUS_BUFFER_TOO_SMALL if the buffer is too small.

    STATUS_SUCCESS if the routine succeeded.

--*/

{

    ULONG DataSize;
    ULONG Length;
    ULONG Size;
    NTSTATUS Status;
    PBCDE_STRING StringElement;

    //
    // Determine the length of the string.
    //

    if (WhitespaceTerminated != FALSE) {
        Length = 0;
        while ((String[Length] != L' ') && (String[Length] != L'\0')) {
            Length += 1;
        }

    } else {
        Length = (ULONG)wcslen(String);
    }

    //
    // Reserve space for the option header, description and for the string.
    //

    Status = RtlULongMult(Length, sizeof(WCHAR), &DataSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongAdd(DataSize, sizeof(UNICODE_NULL), &DataSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Size = sizeof(BOOT_ENTRY_OPTION);
    Status = RtlULongAdd(Size, FIELD_OFFSET(BCDE_STRING, String), &Size);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = RtlULongAdd(Size, DataSize, &Size);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (ARGUMENT_PRESENT(RequiredBytes)) {
        *RequiredBytes = Size;
    }

    if (AvailableBytes < Size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Fill in the option.
    //

    RtlZeroMemory(Option, Size);
    Option->Type = Type;
    StringElement = Add2Ptr(Option, sizeof(BOOT_ENTRY_OPTION));
    Option->DataOffset = (ULONG)PtrOffset(Option, StringElement);
    Option->DataSize = DataSize;
    RtlCopyMemory(StringElement->String,
                  String,
                  DataSize - sizeof(UNICODE_NULL));

    StringElement->String[Length] = L'\0';
    return STATUS_SUCCESS;
}

VOID
BlpCopyAndSanitizeUnicodeToAnsiString (
    __out_ecount(Length) PCHAR AnsiString,
    __in_ecount(Length) PCWCHAR UnicodeString,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine converts a string from UNICODE to ANSI form. Any non ANSI
    characters (values above 0xFF) are converted to a sentinel '?' value.

Arguments:

    AnsiString - Receives ANSI string.

    UnicodeString - Supplies unicode string to convert.

    Length - Supplies number of characters in the string.

Return Value:

    None.

--*/

{

    WCHAR Character;
    ULONG Index;

    for (Index = 0; Index < Length; Index += 1) {
        Character = UnicodeString[Index];
        if ((Character & 0xFF00) != 0) {
            Character = L'?';
        }

        AnsiString[Index] = (UCHAR)Character;
    }

    return;
}

