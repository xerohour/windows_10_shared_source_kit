/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    block.c

Abstract:

    Implementation of block i/o routines using EFI.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "efiblock.h"
#include "disk.h"

// -------------------------------------------------------------------- Globals

//
// Block I/O devices are enumerated using EFI Boot Services.  A service exists
// to query for an array of device handles for devices supporting the block i/o
// protocol.  Using each handle, it is possible to obtain an block i/o
// interface for each device.  This interface will contain device information.
//
// We will need to hash the EFI handle's for the devices already processed.
// The list of handles that support the block i/o protocol can change at
// runtime with the addition/removal of devices and media.  Hashing the handles
// will provide a lookup for processed handles to minimize duplicated work.
//

HASH_TABLE_ID HashTableId;

// ------------------------------------------------------------------ Functions

NTSTATUS
BlockIoFirmwareInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the global data structures used by this modude, in particular
    a global hash table used to cache the device handles processed by
    BlockIoFirmwareOpen.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if allocation for hash table fails.

--*/

{

    NTSTATUS Status;

    //
    // Initialize a hash table for caching device handles processed by this
    // routine.
    //

    Status = BlHtCreate(29,   // Number of buckets in hash table.
                        BlockIoEfiHashFunction,
                        NULL,
                        &HashTableId);

    return Status;
}

NTSTATUS
BlockIoFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    Open a disk device and collects information, such as disk geometry for the
    device.

    Since block i/o devices are enumerated through the firmware, we will have
    to iterate through the devices in the order they were enumerated by EFI
    until we find the caller's device.  To prevent the duplication of work,
    cache each device that we process in the block i/o device entry table.

    Using EFI all Block I/O device handles can be obtained with one single
    query.  Therefore, all block i/o device implementations should call this
    routine in their firmware open routine.

Arguments:

    Device - Device to open.

    BlockIoDevice - Pointer to buffer to store block i/o information for the
                    device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE when the requested device could not be found.
    STATUS_NO_MEMORY if allocation fails for an internal data structure.

--*/

{

    EFI_HANDLE *BlockIoHandleArray;
    PDEVICE_ENTRY DeviceEntry;
    BOOLEAN FoundMatch;
    HASH_KEY HashKey;
    ULONG Id;
    ULONG Index;
    UINTN NumberDevices;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    DeviceEntry = NULL;   // Quiet compiler.

    //
    // Get the list of handles to devices that support the Block I/O protocol.
    //

    Status = BlockIoEfiGetBlockIoHandleArray(&BlockIoHandleArray,
                                             &NumberDevices);

    if (!NT_SUCCESS(Status)) {
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Loop on the array of device handles searching for our device.  Cache the
    // block i/o information for each handle processed, adding it to the device
    // table.
    //

    ReturnStatus = STATUS_NO_SUCH_DEVICE;
    FoundMatch = FALSE;

    //
    // Block I/O Handles are hashed to avoid the duplication of work.
    //

    HashKey.Size = sizeof(EFI_HANDLE);
    HashKey.Flags = HASH_KEY_FLAGS_VALUE;
    for (Index = 0;
         (Index < NumberDevices) && (FoundMatch == FALSE);
         Index += 1) {

        //
        // Check to see if the EFI Block I/O device has already been processed.
        // If it has, then the caller (BlockIoOpen) would have already checked
        // the entry when looking at the Block I/O Device Table.  Only create
        // a new entry for devices that have not yet been processed.
        //

        HashKey.Value = BlockIoHandleArray[Index];
        Status = BlHtLookup(HashTableId, &HashKey, NULL);
        if (!NT_SUCCESS(Status)) {

            //
            // Hash the EFI handle to avoid processing this handle again.  The
            // data field does not matter too much, since we are using the
            // hash able as a quick lookup as to whether a handle has been
            // processed.
            //
            // N.B. The EFI Handle is hashed regardless of whether we could
            //      construct a device entry.  The assumption is that if
            //      we fail to create a device entry once for a handle, we
            //      will continue to fail to create the entry.  No reason to
            //      duplicate all that work.
            //

            Status = BlHtStore(HashTableId,
                               &HashKey,
                               BlockIoHandleArray[Index],
                               sizeof(EFI_HANDLE));

            if (!NT_SUCCESS(Status)) {
                ReturnStatus = Status;
                break;
            }

            //
            // Create a device entry for the current device.  This will be
            // added to the Block I/O Device Table.
            //

            Status = BlockIoEfiCreateDeviceEntry(&DeviceEntry,
                                                 BlockIoHandleArray[Index]);

            //
            // If there was any error processing the device, skip to the next
            // handle.
            //

            if (!NT_SUCCESS(Status)) {
                continue;
            }

            //
            // Cache the device entry entry in the DiskTable.
            //

            Status = BlTblSetEntry(&BlockIoDeviceTable,
                                   &BlockIoDeviceTableEntries,
                                   DeviceEntry,
                                   &Id,
                                   TblDoNotPurgeEntry);

            if (!NT_SUCCESS(Status)) {
                ReturnStatus = Status;

                //
                // Remove the entry from the hash table, since the error
                // was not a result of processing the handle, but rather
                // adding the entry to our internal cache table.
                //

                Status = BlHtDelete(HashTableId, &HashKey);

                ASSERT(Status == STATUS_SUCCESS);

                BlockIopFreeAllocations(DeviceEntry->DeviceData);
                BlMmFreeHeap(DeviceEntry->DeviceIdentifier);
                BlMmFreeHeap(DeviceEntry);
                break;
            }

            //
            // Check to see if the device we just collected information for
            // was the one the caller wishes to open.
            //

            FoundMatch = BlockIoDeviceTableCompare(DeviceEntry,
                                                   Device,
                                                   NULL,
                                                   NULL,
                                                   NULL);
        }
    }

    //
    // If we found the caller's device, copy the information from the disk
    // table to the device entry.
    //

    if (FoundMatch != FALSE) {
        RtlCopyMemory(BlockIoDevice,
                      DeviceEntry->DeviceData,
                      sizeof(BLOCK_IO_DEVICE));

        ReturnStatus = STATUS_SUCCESS;

    }

    BlMmFreeHeap(BlockIoHandleArray);
    return ReturnStatus;
}

NTSTATUS
BlockIoFirmwareRead (
    __in PBLOCK_IO_DEVICE BlockIoDevice,
    __out_ecount(NumberBlocks * BlockIoDevice->Information.BlockSize) PUCHAR Buffer,
    __in ULONGLONG BlockNumber,
    __in ULONGLONG NumberBlocks
    )

/*++

Routine Description:

    Reads NumberBlocks starting at offset BlockNumber and stores the contents
    in Buffer.

    The EFI Block I/O interface was saved in the firmware data structure of the
    block i/o device and is used when reading from the device.

Arguments:

    BlockIoDevice - Block I/O device structure.

    Buffer - Pointer to address to copy read contents.

    BlockNumber - Absolute LBA to start the read.

    NumberBlocks - The number of blocks to read.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing the
        requested operation.
    STATUS_INVALID_PARAMETER if the requested block number is invalid.

--*/

{

    ULONG Attempt;
    EFI_BLOCK_IO *BlockIoInterface;
    NTSTATUS Status;

    //
    // Attempt to read the request blocks via the device's interface.
    // We may try one additional time if the media has changed.
    //

    for (Status = STATUS_SUCCESS, Attempt = 0;
         (Attempt < 2) && NT_SUCCESS(Status);
         Attempt += 1) {

        BlockIoInterface = BlockIoDevice->FirmwareData.BlockIoInterface;
        Status = EfiBlockIoReadBlocks(BlockIoInterface,
                                      BlockNumber,
                                      (UINTN)NumberBlocks,
                                      Buffer);

        if (Status != STATUS_MEDIA_CHANGED) {
            break;
        }

        //
        // The media has changed.  Get the new pointer to the device's
        // interface.
        //
        // First, call EfiCloseProtocol to unmap the current interface.
        //

        EfiCloseProtocol(BlockIoDevice->FirmwareData.Handle,
                         &EfiBlockIoProtocol,
                         &BlockIoDevice->FirmwareData.BlockIoInterface);

        Status = EfiOpenProtocol(
                         BlockIoDevice->FirmwareData.Handle,
                         &EfiBlockIoProtocol,
                         &BlockIoDevice->FirmwareData.BlockIoInterface);
    }

    return Status;
}

NTSTATUS
BlockIoFirmwareWrite (
    __in PBLOCK_IO_DEVICE BlockIoDevice,
    __in_ecount(NumberBlocks * BlockIoDevice->Information.BlockSize) PUCHAR Buffer,
    __in ULONGLONG BlockNumber,
    __in ULONGLONG NumberBlocks
    )

/*++

Routine Description:

    Writes NumberBlocks from Buffer to the device, starting at offset
    BlockNumber.

    The EFI Block I/O interface was saved in the firmware data structure of the
    block i/o device and is used when writing to the device.

Arguments:

    BlockIoDevice - Block I/O device structure.

    Buffer - Pointer to address with write contents.

    BlockNumber - Absolute LBA to start the write.

    NumberBlocks - The number of blocks to write.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing the
        requested operation.
    STATUS_INVALID_PARAMETER if the requested block number is invalid.

--*/

{

    ULONG Attempt;
    EFI_BLOCK_IO *BlockIoInterface;
    NTSTATUS Status;

    //
    // Attempt to write the request blocks via the device's interface.  We may
    // try one additional time if the media has changed.
    //

    for (Status = STATUS_SUCCESS, Attempt = 0;
         (Attempt < 2) && NT_SUCCESS(Status);
         Attempt += 1) {

        BlockIoInterface = BlockIoDevice->FirmwareData.BlockIoInterface;
        Status = EfiBlockIoWriteBlocks(BlockIoInterface,
                                       BlockNumber,
                                       (UINTN)NumberBlocks,
                                       Buffer);

        if (Status != STATUS_MEDIA_CHANGED) {
            break;
        }

        //
        // The media has changed.  Get the new pointer to the device's
        // interface.
        //
        // First, call EfiCloseProtocol to unmap the current interface.
        //

        EfiCloseProtocol(BlockIoDevice->FirmwareData.Handle,
                         &EfiBlockIoProtocol,
                         &BlockIoDevice->FirmwareData.BlockIoInterface);

        Status = EfiOpenProtocol(BlockIoDevice->FirmwareData.Handle,
                                 &EfiBlockIoProtocol,
                                 &BlockIoDevice->FirmwareData.BlockIoInterface);

    }

    return Status;
}

// --------------------------------------------------------- Internal Functions

NTSTATUS
BlockIoEfiReset (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Resets the device.

    The EFI Block I/O interface was saved in the firmware data structure of the
    block i/o device and is used resetting the disk.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing the
        requested operation.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    EFI_BLOCK_IO *BlockIoInterface;
    NTSTATUS Status;

    BlockIoDevice = DeviceEntry->DeviceData;
    BlockIoInterface = BlockIoDevice->FirmwareData.BlockIoInterface;
    Status = EfiBlockIoReset(BlockIoInterface, TRUE);
    return Status;
}

NTSTATUS
BlockIoEfiFlush (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Flushes all modified data to the physical block device.

    The EFI Block I/O interface was saved in the firmware data structure of the
    block i/o device and is used resetting the disk.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing the
        requested operation.
    STATUS_NO_MEDIA when there is no media in the device.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    EFI_BLOCK_IO *BlockIoInterface;
    NTSTATUS Status;

    BlockIoDevice = DeviceEntry->DeviceData;
    BlockIoInterface = BlockIoDevice->FirmwareData.BlockIoInterface;
    Status = EfiBlockIoFlushBlocks(BlockIoInterface);
    return Status;
}

NTSTATUS
BlockIoEfiCompareDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_HANDLE Handle
    )

/*++

Routine Description:

    This routine compares a boot environment device to an EFI block IO device
    handle to determine if they represent the same hardware device.

Arguments:

    Device - Supplies the device to compare against.

    Handle - Supplies a handle to a block IO device to compare.

Return Value:

    NT status code.

--*/

{

    PBLOCK_IO_IDENTIFIER CandidateBlockIo;
    PDEVICE_ENTRY CandidateDevice;
    PBOOT_ENVIRONMENT_DEVICE CandidateIdentifier;
    NTSTATUS Status;
    PBLOCK_IO_IDENTIFIER TargetBlockIo;

    CandidateDevice = NULL;

    //
    // Validate input parameters.
    //

    if (Device == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto CompareDeviceEnd;
    }

    //
    // Get the block IO identifier for the boot environment device for
    // comparison.
    //

    switch (Device->DeviceType) {
        case DEVICE_BLOCK_IO:
            TargetBlockIo = &Device->u.BlockIo;
            break;

        case DEVICE_PARTITION:
        case DEVICE_PARTITION_EX:
            TargetBlockIo = &Device->u.Partition.ParentIdentifier;
            break;

        default:
            Status = STATUS_INVALID_PARAMETER;
            goto CompareDeviceEnd;
    }

    //
    // Create a device entry for the EFI block IO device.
    //

    Status = BlockIoEfiCreateDeviceEntry(&CandidateDevice, Handle);
    if (!NT_SUCCESS(Status)) {
        goto CompareDeviceEnd;
    }

    //
    // Extract the block IO identifier from the new device entry, then
    // compare it against the boot environment device.
    //

    Status = STATUS_UNSUCCESSFUL;
    CandidateIdentifier = CandidateDevice->DeviceIdentifier;
    switch (CandidateIdentifier->DeviceType) {
        case DEVICE_BLOCK_IO:
            CandidateBlockIo = &CandidateIdentifier->u.BlockIo;
            break;

        case DEVICE_PARTITION:
        case DEVICE_PARTITION_EX:
            CandidateBlockIo =
                &CandidateIdentifier->u.Partition.ParentIdentifier;

            break;

        default:
            goto CompareDeviceEnd;
    }

    if (CandidateBlockIo->BlockIoType != TargetBlockIo->BlockIoType) {
        goto CompareDeviceEnd;
    }

    switch (CandidateBlockIo->BlockIoType) {
        case BLOCK_IO_REMOVABLE_DISK:
            if (RtlCompareMemory(&CandidateBlockIo->u.RemovableDisk,
                                 &TargetBlockIo->u.RemovableDisk,
                                 sizeof(REMOVABLE_DISK_IDENTIFIER)) ==
                sizeof(REMOVABLE_DISK_IDENTIFIER)) {

                Status = STATUS_SUCCESS;
            }

            break;

        case BLOCK_IO_CDROM:
            if (RtlCompareMemory(&CandidateBlockIo->u.Cdrom,
                                 &TargetBlockIo->u.Cdrom,
                                 sizeof(CDROM_IDENTIFIER)) ==
                sizeof(CDROM_IDENTIFIER)) {

                Status = STATUS_SUCCESS;
            }

            break;

        case BLOCK_IO_RAMDISK:
            if (RtlCompareMemory(&CandidateBlockIo->u.RamDisk,
                                 &TargetBlockIo->u.RamDisk,
                                 sizeof(RAM_DISK_IDENTIFIER)) ==
                sizeof(RAM_DISK_IDENTIFIER)) {

                Status = STATUS_SUCCESS;
            }

            break;

        case BLOCK_IO_FILE:
            if (RtlCompareMemory(&CandidateBlockIo->u.File,
                                 &TargetBlockIo->u.File,
                                 sizeof(FILE_IDENTIFIER)) ==
                sizeof(FILE_IDENTIFIER)) {

                Status = STATUS_SUCCESS;
            }

            break;

        case BLOCK_IO_HARD_DISK:
            if (RtlCompareMemory(&CandidateBlockIo->u.HardDisk,
                                 &TargetBlockIo->u.HardDisk,
                                 sizeof(HARD_DISK_IDENTIFIER)) ==
                sizeof(HARD_DISK_IDENTIFIER)) {

                Status = STATUS_SUCCESS;
            }

            break;

        default:
            break;
    }

CompareDeviceEnd:
    if (CandidateDevice != NULL) {
        BlMmFreeHeap(CandidateDevice->DeviceIdentifier);
        BlockIopFreeAllocations((PBLOCK_IO_DEVICE)CandidateDevice->DeviceData);
        BlMmFreeHeap(CandidateDevice);
    }

    return Status;
}

NTSTATUS
BlockIoEfiCreateDeviceEntry (
    __deref_out PDEVICE_ENTRY *DeviceEntry,
    __in EFI_HANDLE Handle
    )

/*++

Routine Description:

    Provided an EFI handle to a Block I/O Device, this routine attempts to
    build a device entry.

    N.B. Although EFI treats partitions as Block I/O Device's and each will
         support the Block I/O Protocol, this routine will not successfully
         initialize a device entry for a partition device.  The partition code
         in partition.c contains specialized code for the partition case.

Arguments:

    DeviceEntry - Pointer to store the address of the device entry allocated by
    this routine.

    Handle - EFI Handle to a block i/o device.

Return Value:

    STATUS_SUCCESS: when successful + DeviceEntry pointing to a device entry
        buffer allocated by this routine.
    STATUS_NO_MEMORY: when the DeviceEntry buffer can not be allocated by this
        routine.
    STATUS_INVALID_PARAMETER: when there is an error detected (ie. Handle does
        not support the protocols we require).

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    PDEVICE_ENTRY DiskEntry;
    NTSTATUS Status;

    //
    // Allocate a device entry for the for the block i/o cache table..
    //

    DiskEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
    if (DiskEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(DiskEntry, sizeof(DEVICE_ENTRY));

    //
    // Allocate device identifier for DiskEntry.
    //

    DeviceIdentifier = BlMmAllocateHeap(sizeof(BOOT_ENVIRONMENT_DEVICE));
    if (DeviceIdentifier == NULL) {
        Status =  STATUS_NO_MEMORY;
        goto BlockIoEfiCreateDeviceEntryDone;
    }

    RtlZeroMemory(DeviceIdentifier, sizeof(BOOT_ENVIRONMENT_DEVICE));

    //
    // Allocate a Block I/O Device.
    //

    BlockIoDevice = BlMmAllocateHeap(sizeof(BLOCK_IO_DEVICE));
    if (BlockIoDevice == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlockIoEfiCreateDeviceEntryDone;
    }

    RtlZeroMemory(BlockIoDevice, sizeof(BLOCK_IO_DEVICE));
    DiskEntry->DeviceData = BlockIoDevice;

    //
    // Note: BlockIoEfiGetDeviceInformation will set
    // DeviceIdentifier->DeviceType.
    //

    DeviceIdentifier->Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    DiskEntry->DeviceIdentifier = DeviceIdentifier;

    //
    // Initialize some internal data.  The code that collects information for
    // the device may need to read/write to the device in order to determine
    // all required information.  Initialize the internal data to ensure any
    // device operations will be successful.
    //
    // Zeroed the memory above, everything is initialized to 0.
    //
    // BlockIoDevice->Information.LbaOffset       = 0;
    // BlockIoDevice->Information.BlockOffset     = 0;
    //

    //
    // Set the function table.  Default all function pointers to the block i/o
    // table.
    //

    RtlCopyMemory(&DiskEntry->FunctionTable,
                  &BlockIoDeviceFunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    //
    // Override 'Reset' and 'Flush' methods of the block device with those
    // available through the EFI protocol.
    //

    DiskEntry->FunctionTable.Reset = BlockIoEfiReset;
    DiskEntry->FunctionTable.Flush = BlockIoEfiFlush;
    
    //
    // Hook close routine to close EFI handle and release partition table if
    // allocated.
    //

    DiskEntry->FunctionTable.Close = DiskClose;

    //
    // Save the EFI handle.  If the media is removed, or if the interface
    // pointer changes for any reason, we need to be able to get the new
    // interface for the device.
    //

    BlockIoDevice->FirmwareData.Handle = Handle;

    //
    // Collect the Block I/O information for the device.
    //

    Status = BlockIoEfiGetBlockIoInformation(BlockIoDevice);
    if (!NT_SUCCESS(Status)) {
        goto BlockIoEfiCreateDeviceEntryDone;
    }

    //
    // Collect other device information.  This includes the block i/o type and
    // partitioning information.
    //

    Status = BlockIoEfiGetDeviceInformation(DiskEntry);
    if (!NT_SUCCESS(Status)) {
        goto BlockIoEfiCreateDeviceEntryDone;
    }

    //
    // Set caller's pointer do the new DiskEntry.
    //

    *DeviceEntry = DiskEntry;

BlockIoEfiCreateDeviceEntryDone:

    if (!NT_SUCCESS(Status)) {

        //
        // If an error was found, free all allocated memory.
        //

        if (DiskEntry->DeviceIdentifier) {
            BlMmFreeHeap(DiskEntry->DeviceIdentifier);
        }

        BlockIopFreeAllocations((PBLOCK_IO_DEVICE)DiskEntry->DeviceData);
        BlMmFreeHeap(DiskEntry);
    }

    return Status;
}

NTSTATUS
BlockIoEfiGetBlockIoInformation (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    Fills out any fields in a Block I/O Device structure which can be
    obtained from an EFI Block I/O Interface.

    This informatin includes; block size, alignment requirements and even
    the Block I/O interface itself.

Arguments:

    BlockIoDevice - Block I/O device structure.

Return Value:

    STATUS_SUCCESS: when successful.  BlockIoDevice will have additional data
    fields filled.
    STATUS_INVALID_PARAMETER if the firmware handle is invalid.
    STATUS_NOT_FOUND if the firmware handle specified in BlockIoDevice does not
        support the EFI Block I/O Protocol.

--*/

{

    PBLOCK_IO_FIRMWARE_DATA Firmware;
    EFI_BLOCK_IO_MEDIA *FirmwareInfo;
    PBLOCK_IO_INFORMATION Info;
    NTSTATUS Status;

    Firmware = &BlockIoDevice->FirmwareData;
    Info = &BlockIoDevice->Information;

    //
    // Attempt to get the EFI Block I/O interface for the device.
    //

    Status = EfiOpenProtocol(Firmware->Handle,
                             &EfiBlockIoProtocol,
                             &Firmware->BlockIoInterface);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Initialize the block i/o information structure using the firmware
    // description.
    //

    FirmwareInfo = Firmware->BlockIoInterface->Media;

    //
    // Ignore removable media devices with no media.
    //

    if ((FirmwareInfo->RemovableMedia != FALSE) &&
        (FirmwareInfo->MediaPresent == FALSE)) {

        return STATUS_INVALID_PARAMETER;
    }

    Info->DeviceFlags = BLOCK_IO_DEVICE_FLAGS_NONE;
    if (FirmwareInfo->RemovableMedia != FALSE) {
        SET_FLAGS(Info->DeviceFlags, BLOCK_IO_DEVICE_FLAGS_REMOVABLE_MEDIA);
    }

    if (FirmwareInfo->MediaPresent != FALSE) {
        SET_FLAGS(Info->DeviceFlags, BLOCK_IO_DEVICE_FLAGS_MEDIA_PRESENT);
    }

    Info->IoAccessFlags = BLOCK_IO_ACCESS_FLAGS_NONE;
    Info->BlockSize = FirmwareInfo->BlockSize;

    //
    // Sanitize the last block field provided by the firmware.  If the value
    // is set to zero, fail to enumerate the device.  If the value is greater
    // than MAX_LAST_BLOCK, truncate the value.
    //

    if (FirmwareInfo->LastBlock == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    if (FirmwareInfo->LastBlock > MAX_LAST_BLOCK) {
        Info->LastBlock = MAX_LAST_BLOCK;

    } else {
        Info->LastBlock = FirmwareInfo->LastBlock;
    }

    //
    // TODO: Remove this hack.
    //
    // A hack was added to BlockIoFirmwareRead in order to get around a
    // buggy EFI implementation where a transfer buffer could not cross a
    // 64KB boundary.  This hack requires transfer buffers to be block
    // aligned.
    //

    if (FirmwareInfo->IoAlign < FirmwareInfo->BlockSize) {
        Info->Alignment = FirmwareInfo->BlockSize;

    } else {
        Info->Alignment = FirmwareInfo->IoAlign;
    }

    //
    // As per the EFI specification, IoAlign must be a power of two.
    //

    ASSERT((Info->Alignment & (Info->Alignment - 1)) == 0);

    return STATUS_SUCCESS;
}

NTSTATUS
BlockIoEfiGetDeviceInformation (
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Initializes any fields in a device entry that can be gathered from the EFI
    device path for a block I/O device.

    Most notably, this routine will infer the device type and create a device
    identifier for the device.

    N.B. This routine will only operate on non-partition devices.  Partitions
         are handled seprately in partition.c.

Arguments:

    DeviceEntry - Supplies a pointer to a device entry.

Return Value:

    STATUS_SUCCESS: when successful.  BlockIoDevice will have additional data
        fields filled.
    STATUS_INVALID_PARAMETER if the EFI Handle specified in DeviceEntry is not
        valid or if the Device's EFI device path is unexpected or not
        supported.
    STATUS_NOT_FOUND if the EFI Handle specified in DeviceEntry does not
        support the EFI Device Path Protocol.
    STATUS_NOT_SUPPORTED if the device type can not be determined for the
        device.

--*/

{

    ACPI_HID_DEVICE_PATH UNALIGNED *AcpiDevicePath;
    PBLOCK_IO_DEVICE BlockIoDevice;
    PBLOCK_IO_IDENTIFIER BlockIoIdentifier;
    CDROM_DEVICE_PATH UNALIGNED *CdromNode;
    PBOOT_ENVIRONMENT_DEVICE Device;
    PDEVICE_DISK_INFORMATION DiskInformation;
    BOOLEAN FoundDevice;
    HARDDRIVE_DEVICE_PATH UNALIGNED *HardDriveNode;
    EFI_DEVICE_PATH UNALIGNED *LeafNode;
    ULONG Pass;
    EFI_PROTOCOL_INTERFACE Protocol[2];
    NTSTATUS Status;

    FoundDevice = FALSE;

    //
    // Get pointers to device specific structures within the device entry.
    //

    Device = DeviceEntry->DeviceIdentifier;
    BlockIoIdentifier = &Device->u.BlockIo;
    BlockIoDevice = (PBLOCK_IO_DEVICE)DeviceEntry->DeviceData;

    //
    // Every device contains an EFI device path which describes the location
    // of a device.  Partitions (and media) are additionally called devices
    // and contain a device path.  The media and partition device path node
    // provides the requisite information to determine the type of device.
    // For physical devices, the device path describes the physical location of
    // the device (SCSI PUN/LUN, ATAPI IDE controller X, etc).  From the
    // media or partition information, it can be determined if a block I/O
    // device is a HDD, CDROM, Floppy, etc.
    //
    // This fact is leveraged to make a two passes through the namespace.
    // The first is for the device specified by a device entry.  A second pass
    // is used to find the media device (or partition) ontop of the device.
    // When found, the media device path node will describe the type of device.
    //

    //
    // Initialize the protocol interface array.  Index 0 is for the current
    // device.  If needed, index 1 will contain the handle and device path
    // interface for a child device.
    //

    Protocol[0].Handle = BlockIoDevice->FirmwareData.Handle;
    Protocol[1].Handle = 0;

    //
    // Get the EFI device path for the device.
    //

    Status = EfiOpenProtocol(Protocol[0].Handle,
                             &EfiDevicePathProtocol,
                             &Protocol[0].Interface);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    for (Pass = 0;
         ((Pass < 2) && (FoundDevice == FALSE) && (Protocol[Pass].Handle != 0));
         Pass += 1) {

        //
        // Get a pointer to the last node in the device path.  This should
        // either be a node describing the physical location of the device
        // (typically a messaging device node) or the partitioning scheme of
        // the device (media device node).
        //

        LeafNode = EfiGetLeafNode(Protocol[Pass].Interface);
        switch (LeafNode->Type) {
        case ACPI_DEVICE_PATH:

            //
            // Check to see if this is a legacy floppy device.
            //

            AcpiDevicePath = (ACPI_HID_DEVICE_PATH UNALIGNED *)LeafNode;
            if ((AcpiDevicePath->HID == EISA_PNP_ID(0x604)) ||
                (AcpiDevicePath->HID == EISA_PNP_ID(0x700))) {

                //
                // If this is a legacy floppy device, then the drive number
                // is listed in the UID field.
                //

                Device->DeviceType = DEVICE_BLOCK_IO;
                BlockIoDevice->Information.Type = BLOCK_IO_REMOVABLE_DISK;
                BlockIoIdentifier->BlockIoType = BLOCK_IO_REMOVABLE_DISK;
                BlockIoIdentifier->u.RemovableDisk.DriveNumber =
                    AcpiDevicePath->UID;

                FoundDevice = TRUE;
            }

            break;

        case MEDIA_DEVICE_PATH:

            //
            // A media device path describes some abstraction on the device.
            // Most notably the partitioning scheme or a file path for the
            // device.
            //

            //
            // Ignore all partition devices, unless they are being explicitly
            // used to determine device type.
            //

            if (Pass == 0) {
                break;
            }

            switch (LeafNode->SubType) {
            case MEDIA_HARDDRIVE_DP:

                //
                // The EFI application was booted from a partition on a
                // harddisk drive.
                //

                HardDriveNode = (HARDDRIVE_DEVICE_PATH UNALIGNED *)LeafNode;
                BlockIoIdentifier->BlockIoType = BLOCK_IO_HARD_DISK;
                DiskInformation =
                    &BlockIoDevice->Information.DeviceInformation.Disk;

                switch (HardDriveNode->SignatureType) {
                case SIGNATURE_TYPE_MBR:
                    BlockIoDevice->Information.PartitionType = MBR;
                    BlockIoIdentifier->u.HardDisk.PartitionType = MBR;
                    BlockIoIdentifier->u.HardDisk.Mbr.Signature =
                        *((ULONG UNALIGNED *)HardDriveNode->Signature);

                    DiskInformation->Mbr.Signature =
                        BlockIoIdentifier->u.HardDisk.Mbr.Signature;

                    FoundDevice = TRUE;
                    break;

                case SIGNATURE_TYPE_GUID:
                    BlockIoDevice->Information.PartitionType = GPT;
                    BlockIoIdentifier->u.HardDisk.PartitionType = GPT;

                    //
                    // N.B. The GUID signature listed in the hard drive device
                    //      path node is for the partition.  To determine
                    //      the signature for the disk, the GPT partition
                    //      table must be read.
                    //

                    Status =
                        BlockIoGetGPTDiskData(
                            DeviceEntry,
                            &BlockIoIdentifier->u.HardDisk.Gpt.Signature,
                            &BlockIoDevice->InternalData.u.DiskData.GptData);

                    if (NT_SUCCESS(Status)) {
                        RtlCopyMemory(
                            &DiskInformation->Gpt.Signature,
                            &BlockIoIdentifier->u.HardDisk.Gpt.Signature,
                            sizeof(GUID));

                        FoundDevice = TRUE;
                    }

                    break;

                default:
                    BlockIoDevice->Information.PartitionType = RAW;
                    BlockIoIdentifier->u.HardDisk.PartitionType = RAW;
                    BlockIoIdentifier->u.HardDisk.Raw.DiskNumber =
                        BlockIoFirmwareRawDiskCount;

                    BlockIoFirmwareRawDiskCount += 1;

                    //
                    // Does this case ever happen?  Is it possible to have
                    // a media device node for a raw disk?
                    //

                    ASSERT(FALSE);

                    FoundDevice = TRUE;
                    break;
                }

                break;

            case MEDIA_CDROM_DP:

                //
                // The EFI application was booted from a CDROM.
                //

                CdromNode = (CDROM_DEVICE_PATH UNALIGNED *)LeafNode;
                BlockIoDevice->Information.PartitionType = RAW;
                BlockIoDevice->Information.Type = BLOCK_IO_CDROM;

                //
                // TODO: CDROM number should not equal zero.  It should be
                //       determined by the parent (messanger device node)'s
                //       number.
                //

                BlockIoIdentifier->BlockIoType = BLOCK_IO_CDROM;
                BlockIoIdentifier->u.Cdrom.CdromNumber = 0;
                FoundDevice = TRUE;
                break;

            default:
                break;
            }

        default:
            break;
        }

        //
        // If a device type could not be determined, attempt to find a device
        // describing media or a partition on top of the current device.
        //
        // Media or partition information can be overlayed on top of any
        // device path supporting the block I/O protocol (commonly messaging
        // device paths but sometimes other device paths as well).  It is not
        // expected that additional media or partition information will
        // overlay media or ACPI device paths.
        //

        if ((FoundDevice == FALSE) &&
            (LeafNode->Type != ACPI_DEVICE_PATH) &&
            (LeafNode->Type != MEDIA_DEVICE_PATH) &&
            (Pass == 0)) {

            Status = BlockIoEfiGetChildHandle(&Protocol[Pass],
                                              &Protocol[Pass + 1]);

            //
            // If a media or partition device could not be obtained, then the
            // device is either an unpartitioned harddisk or some kind of
            // removable disk.
            //

            if (!NT_SUCCESS(Status)) {

                ASSERT(Protocol[Pass + 1].Handle == 0);

                FoundDevice = TRUE;
                Device->DeviceType = DEVICE_BLOCK_IO;
                if (CHECK_FLAG(BlockIoDevice->Information.DeviceFlags,
                               BLOCK_IO_DEVICE_FLAGS_REMOVABLE_MEDIA) != FALSE) {

                    BlockIoDevice->Information.Type = BLOCK_IO_REMOVABLE_DISK;
                    BlockIoIdentifier->BlockIoType = BLOCK_IO_REMOVABLE_DISK;
                    BlockIoIdentifier->u.RemovableDisk.DriveNumber =
                        BlockIoFirmwareRemovableDiskCount;

                    BlockIoFirmwareRemovableDiskCount += 1;

                } else {
                    BlockIoDevice->Information.Type = BLOCK_IO_HARD_DISK;
                    BlockIoIdentifier->BlockIoType = BLOCK_IO_HARD_DISK;
                    BlockIoIdentifier->u.HardDisk.PartitionType = RAW;
                    BlockIoDevice->Information.PartitionType = RAW;
                    BlockIoIdentifier->u.HardDisk.Raw.DiskNumber =
                        BlockIoFirmwareRawDiskCount;

                    BlockIoFirmwareRawDiskCount += 1;
                }
            }
        }
    }

    //
    // Release reference to each device path opened.
    //

    while (Pass--) {
        Status = EfiCloseProtocol(Protocol[Pass].Handle,
                                  &EfiDevicePathProtocol,
                                  Protocol[Pass].Interface);

        ASSERT(Status == STATUS_SUCCESS);
    }

    if (FoundDevice == FALSE) {
        return STATUS_NOT_SUPPORTED;

    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS
BlockIoEfiGetParentHandle (
    __in PEFI_PROTOCOL_INTERFACE ProtocolInterface,
    __out PEFI_PROTOCOL_INTERFACE ParentProtocolInterface
    )

/*++

Routine Description:

    This routine attempts to find a device that supports the Block I/O
    Protocol, whose device path indicates the device is a parent of a
    specified device.

    Given two device paths, one is defined as a parent device if its device
    path is a subset of the other.  Examples include disk drive containing
    partitions, or a cdrom containing media.

Arguments:

    ProtocolInterface - Pointer to an EFI_PROTOCOL_INTERFACE structure
        containing a device handle and a pointer to the device's EFI device
        path.

    ParentProtocolInterface - Pointer to a memory buffer which will be
        populated with a device handle and the device's EFI device path,
        when a parent device is found.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if a child does not exist.
    STATUS_NOT_FOUND if no devices support the EFI Block I/O Protocol.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *DevicePath;
    EFI_DEVICE_PATH UNALIGNED *Dp;
    EFI_HANDLE *HandleArray;
    ULONG Index;
    UINTN NumberDevices;
    NTSTATUS Status;

    //
    // Get list of block i/o device handles.
    //

    Status = BlockIoEfiGetBlockIoHandleArray(&HandleArray, &NumberDevices);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Iterate through the list of handles looking for a handle whose device
    // path is a superset of the ParentDevicePath.
    //

    for (Index = 0; Index < NumberDevices; Index += 1) {

        //
        // Do not open the device path protocol twice for the same device.
        //

        if (HandleArray[Index] != ProtocolInterface->Handle) {
            Status = EfiOpenProtocol(HandleArray[Index],
                                     &EfiDevicePathProtocol,
                                     &DevicePath);

            if (!NT_SUCCESS(Status)) {
                continue;
            }

            //
            // Determine if the current handle represents a parent device.
            //

            Dp = EfiIsDevicePathParent(
                            (EFI_DEVICE_PATH *)ProtocolInterface->Interface,
                            DevicePath);

            if (Dp == DevicePath) {

                //
                // The current handle is for a parent device.  Return the
                // handle and the device path to the caller.
                //

                ParentProtocolInterface->Handle = HandleArray[Index];
                ParentProtocolInterface->Interface = DevicePath;

                BlMmFreeHeap(HandleArray);
                return STATUS_SUCCESS;
            }

            //
            // Close the protocol on the current handle.
            //

            Status = EfiCloseProtocol(HandleArray[Index],
                                      &EfiDevicePathProtocol,
                                      DevicePath);

            ASSERT(Status == STATUS_SUCCESS);
        }
    }

    BlMmFreeHeap(HandleArray);
    return STATUS_NO_SUCH_DEVICE;
}

NTSTATUS
BlockIoEfiGetChildHandle (
    __in PEFI_PROTOCOL_INTERFACE ProtocolInterface,
    __out PEFI_PROTOCOL_INTERFACE ChildProtocolInterface
    )

/*++

Routine Description:

    This routine attempts to find a device that supports the Block I/O
    Protocol, whose device path indicates the device is a child of a
    specified device.

    Given two device paths, one is defined as a child device if its device
    path is a superset of the other.  Examples include partitions on a
    device, media in a CDROM, etc.

Arguments:

    ProtocolInterface - Pointer to an EFI_PROTOCOL_INTERFACE structure
        containing a device handle and a pointer to the device's EFI device
        path.

    ChildProtocolInterface - Pointer to a memory buffer which will be
        populated with a device handle and the device's EFI device path,
        when a child device is found.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if a child does not exist.
    STATUS_NOT_FOUND if no devices support the EFI Block I/O Protocol.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *DevicePath;
    EFI_DEVICE_PATH UNALIGNED *Dp;
    EFI_HANDLE *HandleArray;
    ULONG Index;
    UINTN NumberDevices;
    NTSTATUS Status;

    //
    // Get a list of handles to devices that support the block i/o protocol.
    // Iterate through the list looking for a handle to a child device.
    //

    Status = BlockIoEfiGetBlockIoHandleArray(&HandleArray, &NumberDevices);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Iterate through the list of handles looking for a handle whose device
    // path is a superset of the specified device's.
    //

    for (Index = 0; Index < NumberDevices; Index += 1) {

        //
        // Do not open the device path protocol twice for the same device.
        //

        if (HandleArray[Index] != ProtocolInterface->Handle) {
            Status = EfiOpenProtocol(HandleArray[Index],
                                     &EfiDevicePathProtocol,
                                     &DevicePath);

            if (!NT_SUCCESS(Status)) {
                continue;
            }

            //
            // Determine if the current handle represents a child device.
            //

            Dp = EfiIsDevicePathParent(
                            (EFI_DEVICE_PATH *)ProtocolInterface->Interface,
                            DevicePath);

            if (Dp == ProtocolInterface->Interface) {

                //
                // The current handle is for a child device.  Return the
                // handle and the device path to the caller.
                //

                ChildProtocolInterface->Handle = HandleArray[Index];
                ChildProtocolInterface->Interface = DevicePath;
                BlMmFreeHeap(HandleArray);
                return STATUS_SUCCESS;
            }

            //
            // Close the protocol on the current handle.
            //

            Status = EfiCloseProtocol(HandleArray[Index],
                                      &EfiDevicePathProtocol,
                                      DevicePath);

            ASSERT(Status == STATUS_SUCCESS);
        }
    }

    BlMmFreeHeap(HandleArray);
    return STATUS_NO_SUCH_DEVICE;
}

NTSTATUS
BlockIoEfiGetBlockIoHandleArray (
    __deref_out_ecount(*NumberDevices) EFI_HANDLE **HandleArray,
    __out UINTN *NumberDevices
    )

/*++

Routine Description:

    Gets a list of device handles from the firmware for all devices that
    support block i/o.

Arguments:

    HandleArray - Pointer to address to store an array

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND if there are no handles that support the EFI Block I/O
        Protocol.

--*/

{

    NTSTATUS Status;

    //
    // Get the array of handles to all block i/o devices.
    //

    *NumberDevices = 0;
    *HandleArray = NULL;
    Status = EfiLocateHandleBuffer(ByProtocol,
                                   &EfiBlockIoProtocol,
                                   NULL,
                                   NumberDevices,
                                   HandleArray);

    return Status;
}

ULONG
BlockIoEfiHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    )

/*++

Routine Description:

    This routine implements the hash algorithm for the EFI Block I/O
    hash table.  A key in this table is just an EFI_HANDLE which is a
    pointer to a collection of interfaces.  A pointer is UINTN byte
    aligned which is either 4 or 8 bytes on current processors.  This routine
    devices the pointer by 8 and returns the result modulo HashTableSize.

Arguments:

    Key - Identifying key for object to be inserted in hash table.

    HashTableSize - Number of buckets in the hash table.

Return Value:

    A value between zero and HashTableSize - 1.

--*/

{

    ASSERT(Key != NULL);
    ASSERT(Key->Size == sizeof(UINTN));
    ASSERT(CHECK_FLAG(Key->Flags, HASH_KEY_FLAGS_VALUE));

    return ((ULONG)(((UINTN)Key->Value >> 3) % HashTableSize));
}

