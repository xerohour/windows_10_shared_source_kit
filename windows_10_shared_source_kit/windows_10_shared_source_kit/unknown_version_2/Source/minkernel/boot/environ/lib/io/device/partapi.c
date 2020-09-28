/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    partapi.c

Abstract:

    Implementation of boot library partition i/o api.  These routines make up
    the partition device's function table for partition i/o.

    A partition is defined as a block i/o format on top of a block i/o device.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include "partition.h"
#include <ntdddisk.h>

// -------------------------------------------------------------------- Globals

//
// Function table for Partition I/O.  A partition can be accessed using the
// block i/o interface, so most of the table does not need to be implemented.
//

const DEVICE_FUNCTION_TABLE PartitionDeviceFunctionTable =
{
    (PDEVICE_ENUMERATE_ROUTINE)       PartitionEnumerateDeviceClass,
    (PDEVICE_OPEN_ROUTINE)            PartitionOpen,
    (PDEVICE_CLOSE_ROUTINE)           PartitionClose,
    (PDEVICE_READ_ROUTINE)            NULL,
    (PDEVICE_WRITE_ROUTINE)           NULL,
    (PDEVICE_GET_INFORMATION_ROUTINE) NULL,
    (PDEVICE_SET_INFORMATION_ROUTINE) NULL,
    (PDEVICE_RESET_ROUTINE)           NULL,
    (PDEVICE_FLUSH_ROUTINE)           NULL,
    (PDEVICE_CREATE_ROUTINE)          NULL
};

// ------------------------------------------------------------------ Functions

NTSTATUS
PartitionOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a partition.  Collects partition information.

    Internally, a partition is actually a block i/o device.  They are exposed
    as seperate boot environment devices to avoid confusion.

Arguments:

    Device - The partition to open and initialize.  The partition is identified
             by the block i/o the device it is located as well as the partition
             identifier.

    DeviceEntry - Pointer to the device entry.  Will be filled with all
                  information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if parent Block I/O Type is invalid or if
        partition type can not be determined.
    STATUS_NO_MEMORY if allocation for internal data structure fails.
    STATUS_NO_SUCH_DEVICE if the parent block i/o device can not be found.

--*/

{

    PBLOCK_IO_IDENTIFIER Parent;
    NTSTATUS Status;
    BOOLEAN VirtualPartition;

    //
    // Take a look at the partition's parent device.  Make sure it makes sense.
    //

    if (Device->DeviceType == DEVICE_PARTITION) {
        Parent = &Device->u.Partition.ParentIdentifier;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        Parent = &Device->u.PartitionEx.ParentIdentifier;

    } else {
        return STATUS_INVALID_PARAMETER;
    }

    if (BLOCK_IO_MAX_TYPE <= Parent->BlockIoType) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Check whether the specified partition is a virtual partition (i.e.
    // resides on a virtual device) or physical. Virtual partitions can be
    // opened in a firmware independent manner. For physical partitions,
    // firmware services are required to open the partition.
    //
    // Opening the partition will initialize internal data, device information
    // and function table.
    //

    VirtualPartition = BlDeviceIsVirtualPartitionDevice(Device, NULL);
    if (VirtualPartition == FALSE) {
        Status = PartitionFirmwareOpen(Device, DeviceEntry);

    } else {
        Status = PartpOpenVirtualPartitionDevice(Device, DeviceEntry);
    }

    return Status;
}

NTSTATUS
PartitionClose (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Closes a partition.  This function both performs any firmware
    steps necessary to close a device as well as freeing all memory allocated
    for the device.

Arguments:

    DeviceEntry - Pointer to the device entry.

Return Value:

    STATUS_SUCCESS always.
    STATUS_INVALID_PARAMETER if data structure could not be freed.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    PDISK_INTERNAL_DATA DiskData;
    PBLOCK_IO_INFORMATION Information;
    PDEVICE_ENTRY ParentDevice;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    BOOLEAN VirtualPartition;

    ReturnStatus = STATUS_SUCCESS;

    //
    // Determine whether the partition is physical or virtual.
    //

    BlockIoDevice = (PBLOCK_IO_DEVICE)DeviceEntry->DeviceData;
    Information = &BlockIoDevice->Information;
    if (CHECK_FLAG(Information->DeviceFlags,
                   BLOCK_IO_DEVICE_FLAGS_VIRTUAL_DEVICE) != FALSE) {

        VirtualPartition = TRUE;

    } else {
        VirtualPartition = FALSE;
    }

    //
    // Firmware data only exists for physical partitions and thus need not be
    // released for virtual partitions.
    //

    if (VirtualPartition == FALSE) {
        Status = PartitionFirmwareClose(BlockIoDevice);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    //
    // Close the parent device if one exists for the partition.
    //

    DiskData = &BlockIoDevice->InternalData.u.DiskData;
    ParentDevice = DiskData->ParentDevice;
    if (ParentDevice != NULL) {
        Status = ParentDevice->FunctionTable.Close(ParentDevice);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        if (ParentDevice->DeviceIdentifier != NULL) {
            BlMmFreeHeap(ParentDevice->DeviceIdentifier);
        }

        BlMmFreeHeap(ParentDevice);
        DiskData->ParentDevice = NULL;
    }

    //
    // Free partition patch information if present.
    //

#ifdef ENABLE_PATCHED_PARTITIONS

    PartpReleasePatchData(DiskData);

#endif

    Status = BlockIopFreeAllocations(BlockIoDevice);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    return ReturnStatus;
}

NTSTATUS
PartitionEnumerateDeviceClass (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    Enumerates all the partitions in a system.

    For each device instance in the device class, a device handle is returned.
    The buffer in DeviceIdBuffer is an array of DEVICE_ID's where the size
    field = Number of Device Handles * sizeof(DEVICE_ID).

    If AllocateNewBuffer is TRUE, this routine will allocate sufficient
    memory for all device handles.  The caller is responsible for freeing
    any memory allocated by this routine.

Arguments:

    DeviceClassIdentifier - A boot environment device structure used to
        represent the device class to enumerate (in this case, partition).
        Only Device Class / Sub Class information is referenced.
        (ie. DeviceType, BlockIoType).

        TODO: Should this API allow a caller to specify a disk to enumerate
              the partition's on?

    DeviceIdBuffer - Generic buffer structure for the array of device handles
        for all devices of the requested type.

    AllocateNewBuffer - Boolean value indicating whether the device handle
        buffer should be allocated.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if any allocation fails.

    STATUS_BUFFER_TOO_SMALL if DeviceIdBuffer is too small and
        AllocateNewBuffer is FALSE.

    STATUS_NOT_SUPPORTED if an attempt is being made to enumerate the
        deprecated DEVICE_PARTITION device class.

--*/

{

    PDEVICE_ID CurrDeviceId;
    ULONG DataSize;
    DEVICE_INFORMATION DeviceInformation;
    GENERIC_BUFFER DiskIdArray;
    BOOT_ENVIRONMENT_DEVICE EnumIdentifier;
    PDEVICE_ID FirstFailureId;
    ULONG FirstFailureOffset;
    BOOLEAN ForceFveCheck;
    PDEVICE_ID LastDeviceId;
    PDEVICE_ID LocalIdBuffer;
    GENERIC_BUFFER PartIdArray;
    PBLOCK_IO_IDENTIFIER ParentIdentifier;
    ULONG ParentOffset;
    ULONG RequiredBufferSize;
    NTSTATUS Status;
    PBOOT_ENVIRONMENT_DEVICE BlockDeviceIdentifier;

    //
    // Support has been removed for enumerating the partitions in the system
    // using the deprecated partition identifier layout.
    //

    if (DeviceClassIdentifier->DeviceType != DEVICE_PARTITION_EX) {
        return STATUS_NOT_SUPPORTED;
    }

    ForceFveCheck = (CHECK_FLAG(DeviceClassIdentifier->Flags,
                                DEVICE_FLAG_PARTITION_FORCE_FVE) != FALSE);

    //
    // If the parent identifier for the partition is a hard disk device, then
    // enumerate all firmware enumerable hard disk drives. Otherwise, only
    // partitions resident on the parent device need to be enumerated.
    //

    BlInitGenericBuffer(&DiskIdArray, NULL, 0);
    ParentIdentifier = &DeviceClassIdentifier->u.PartitionEx.ParentIdentifier;
    ParentOffset = (ULONG)PtrOffset(&DeviceClassIdentifier->u.PartitionEx,
                                    ParentIdentifier);

    //
    // If a parent virtual disk device is specified, then open it first. The
    // parent block I/O identifier needs to be converted to a boot environment
    // device for use with the device library.
    //

    LocalIdBuffer = NULL;
    BlockDeviceIdentifier = NULL;
    if (ParentIdentifier->BlockIoType == BLOCK_IO_VIRTUAL_HARD_DISK) {
        BlockDeviceIdentifier = BlMmAllocateHeap(DeviceClassIdentifier->Size);
        if (BlockDeviceIdentifier == NULL) {
            return STATUS_NO_MEMORY;
        }

        RtlZeroMemory(BlockDeviceIdentifier, DeviceClassIdentifier->Size);
        BlockDeviceIdentifier->DeviceType = DEVICE_BLOCK_IO;
        BlockDeviceIdentifier->Size = DeviceClassIdentifier->Size - ParentOffset;
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "ParentIdentifier length is validated")
        RtlCopyMemory(&BlockDeviceIdentifier->u.BlockIo,
                      ParentIdentifier,
                      BlockDeviceIdentifier->Size);

        LocalIdBuffer = BlMmAllocateHeap(sizeof(DEVICE_ID));
        if (LocalIdBuffer == NULL) {
            BlMmFreeHeap(BlockDeviceIdentifier);
            return STATUS_NO_MEMORY;
        }

        BlInitGenericBuffer(&DiskIdArray, LocalIdBuffer, sizeof(DEVICE_ID));

        //
        // Open the parent virtual disk device.
        //

        Status = BlDeviceOpen(BlockDeviceIdentifier, OPEN_READ, LocalIdBuffer);

    } else {
        EnumIdentifier.DeviceType = DEVICE_BLOCK_IO;
        EnumIdentifier.u.BlockIo.BlockIoType = BLOCK_IO_HARD_DISK;
#pragma prefast (suppress : __WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "There is no overlow accessing EnumIdentifier")
        Status = BlDeviceEnumerateDeviceClass(&EnumIdentifier,
                                              &DiskIdArray,
                                              TRUE);
    }

    if (!NT_SUCCESS(Status)) {
        if (LocalIdBuffer != NULL) {
            BlMmFreeHeap(LocalIdBuffer);
        }

        if (BlockDeviceIdentifier != NULL) {
            BlMmFreeHeap(BlockDeviceIdentifier);
        }

        return Status;
    }

    ASSERT((DiskIdArray.Buffer != NULL) && (DiskIdArray.DataSize != 0));

    //
    // Enumerate the partitions of each harddisk, copying a device ID for
    // each into the caller's buffer.  Every partition must be enumerated in
    // order to determine if the caller's buffer is too small.  In this event,
    // remember which device first failed the copy.  If the caller will
    // allow for the buffer to be reallocated, every disk will not have to
    // be reenumerated.
    //

    FirstFailureId = NULL;
    FirstFailureOffset = 0;
    RequiredBufferSize = 0;
    RtlCopyMemory(&PartIdArray, DeviceIdBuffer, sizeof(GENERIC_BUFFER));
    CurrDeviceId = DiskIdArray.Buffer;
    LastDeviceId = Add2Ptr(DiskIdArray.Buffer, DiskIdArray.DataSize);
    while (CurrDeviceId < LastDeviceId) {

        //
        // In enumerating harddisks, BlDeviceEnumerateDeviceClass opened
        // all the harddisks in the system.  This function can enumerate
        // partitions on a GPT or MBR partitioned harddisk.  Skip over all
        // RAW disks.
        //

        Status = BlDeviceGetInformation(*CurrDeviceId, &DeviceInformation);
        if (!NT_SUCCESS(Status)) {
            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            Status = STATUS_SUCCESS;
            continue;
        }

        ASSERT(Status == STATUS_SUCCESS);
        ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);

        if ((DeviceInformation.u.BlockIo.PartitionType != MBR) &&
            (DeviceInformation.u.BlockIo.PartitionType != GPT)) {

            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            continue;
        }

        //
        // Enumerate the partitions on a MBR or GPT partitioned hard disk.
        // Depending on the size of the caller's buffer, this call may only
        // serve to determine the number of valid partitions on the disk.
        //

        if (DeviceInformation.u.BlockIo.PartitionType == MBR) {
            Status = PartpEnumerateMbrPartitions(*CurrDeviceId, &PartIdArray);

        } else if (DeviceInformation.u.BlockIo.PartitionType == GPT) {
            Status = PartpEnumerateGptPartitions(*CurrDeviceId,
                                                 &PartIdArray,
                                                 ForceFveCheck);
        }

        //
        // Enumerating partitions on a MBR disk can fail if the disk
        // has a logical partition and the EFI implementation does not
        // provide the signature of the disk in the partition path.
        // Ignore the disk and move ahead. Also skip over any disks with no
        // media.
        //

#if defined (EFI)

        if (((DeviceInformation.u.BlockIo.PartitionType == MBR) &&
             (Status == STATUS_NOT_FOUND)) || 
            (Status == STATUS_NO_MEDIA)) {

            *CurrDeviceId = INVALID_DEVICE_ID;
            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            Status = STATUS_SUCCESS;
            continue;
        }

#endif

        //
        // Treat any failure to enumerate partitions as fatal except one due to
        // insufficient buffer (which is handled below).
        //

        if ((Status != STATUS_SUCCESS) && (Status != STATUS_BUFFER_TOO_SMALL)) {
            goto EnumerateDeviceClassEnd;
        }

        //
        // Update the offset into caller's buffer to copy valid device
        // handles for enumerated partition devices.  If the caller's buffer
        // is already full, continue to keep track of how much memory is
        // required for every enumerated partition device.
        //

        if (Status == STATUS_SUCCESS) {
            RequiredBufferSize += PartIdArray.DataSize;
            PartIdArray.BufferSize -= PartIdArray.DataSize;
            PartIdArray.Buffer = Add2Ptr(PartIdArray.Buffer,
                                         PartIdArray.DataSize);

        } else /* STATUS_BUFFER_TOO_SMALL */ {
            if (FirstFailureId == NULL) {
                FirstFailureId = CurrDeviceId;
                FirstFailureOffset = RequiredBufferSize;
            }

            RequiredBufferSize += PartIdArray.BufferSize;
            PartIdArray.BufferSize = 0;
            PartIdArray.Buffer = NULL;

        }

        CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
    }

    //
    // If the caller's buffer was sufficiently large, return successfully
    // to the caller.
    //

    if (FirstFailureId == NULL) {
        DeviceIdBuffer->DataSize = RequiredBufferSize;
        return STATUS_SUCCESS;
    }

    ASSERT(DeviceIdBuffer->BufferSize < RequiredBufferSize);

    //
    // If the caller did not request a new buffer to be allocated, then
    // return STATUS_BUFFER_TOO_SMALL.
    //

    if (AllocateNewBuffer == FALSE) {
        DeviceIdBuffer->BufferSize = RequiredBufferSize;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Allocate a sufficiently large buffer.
    //

    PartIdArray.BufferSize = PartIdArray.DataSize = RequiredBufferSize;
    PartIdArray.Buffer = BlMmAllocateHeap(PartIdArray.BufferSize);
    if (PartIdArray.Buffer == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Copy all device id's that were copied into the old buffer.
    //

    if (FirstFailureOffset != 0) {

        ASSERT(DeviceIdBuffer->BufferSize != 0);

        RtlCopyMemory(PartIdArray.Buffer,
                      DeviceIdBuffer->Buffer,
                      FirstFailureOffset);

        BlMmFreeHeap(DeviceIdBuffer->Buffer);
    }

    RtlCopyMemory(DeviceIdBuffer, &PartIdArray, sizeof(GENERIC_BUFFER));

    //
    // Continue populating the caller's buffer with device handles for each
    // partition device on the system.
    //

    PartIdArray.Buffer = Add2Ptr(PartIdArray.Buffer, FirstFailureOffset);
    PartIdArray.BufferSize -= FirstFailureOffset;
    CurrDeviceId = FirstFailureId;
    DataSize = FirstFailureOffset;
    while (CurrDeviceId < LastDeviceId) {

        //
        // In enumerating harddisks, BlDeviceEnumerateDeviceClass opened
        // all the harddisks in the system.  This function can enumerate
        // partitions on a GPT or MBR partitioned harddisk.  Skip over all
        // RAW disks.
        //

        //
        // Ignore disks that failed to enumerate in the last iteration.
        //

#if defined(EFI)

        if (*CurrDeviceId == INVALID_DEVICE_ID) {
            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            continue;
        }

#endif

        Status = BlDeviceGetInformation(*CurrDeviceId, &DeviceInformation);
        if (!NT_SUCCESS(Status)) {
            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            Status = STATUS_SUCCESS;
            continue;
        }

        ASSERT(Status == STATUS_SUCCESS);
        ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);

        if ((DeviceInformation.u.BlockIo.PartitionType != MBR) &&
            (DeviceInformation.u.BlockIo.PartitionType != GPT)) {

            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            continue;
        }

        //
        // Enumerate the partitions on a MBR or GPT partitioned hard disk.
        // Depending on the size of the caller's buffer, this call may only
        // serve to determine the number of valid partitions on the disk.
        //

        if (DeviceInformation.u.BlockIo.PartitionType == MBR) {
            Status = PartpEnumerateMbrPartitions(*CurrDeviceId, &PartIdArray);

        } else if (DeviceInformation.u.BlockIo.PartitionType == GPT) {
            Status = PartpEnumerateGptPartitions(*CurrDeviceId,
                                                 &PartIdArray,
                                                 ForceFveCheck);
        }

        //
        // Skip over MBR disks which failed to enumerate and disks which don't
        // have media.
        //

#if defined (EFI)

        if (((DeviceInformation.u.BlockIo.PartitionType == MBR) &&
             (Status == STATUS_NOT_FOUND)) || 
            (Status == STATUS_NO_MEDIA)) {

            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            Status = STATUS_SUCCESS;
            continue;
        }

#endif

        //
        // If there was a failure enumerating partitions, treat it as fatal.
        //

        if (Status != STATUS_SUCCESS) {
            goto EnumerateDeviceClassEnd;
        }

        DataSize += PartIdArray.DataSize;
        PartIdArray.BufferSize -= PartIdArray.DataSize;
        PartIdArray.Buffer = Add2Ptr(PartIdArray.Buffer, PartIdArray.DataSize);

        CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
    }

    DeviceIdBuffer->DataSize = DataSize;

EnumerateDeviceClassEnd:
    if (DiskIdArray.Buffer != NULL) {
        BlMmFreeHeap(DiskIdArray.Buffer);
    }

    if (BlockDeviceIdentifier != NULL) {
        BlMmFreeHeap(BlockDeviceIdentifier);
    }

    return Status;
}

// --------------------------------------------------------- Internal Functions

//
// Various partition routines.  These can be used from firmware stubs or from
// other routines in this module.
//

NTSTATUS
PartpOpenMbrPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    )

/*++

Routine Description:

    Finds/collects information/opens for the MBR partition described by
    the given device identifier.  The parent device will be read in order to
    obtain the partition table.

    On output the following will have the correct data:

        PartitionEntry->Information
        PartitionEntry->InternalData.StartLbaOffset
        PartitionEntry->FirmwareData

Arguments:

    ParentDeviceEntry - Supplies the device entry for the device where the
        partition resides.

    Device - Supplies the partition to open.  This device must be either a
        DEVICE_PARTITION_EX device, which identifies MBR partitions using
        offsets on disk, or a deprecated DEVICE_PARTITION device, which
        identifiers MBR partitions using partition numbers.

    PartitionEntry - Supplies the device entry buffer for the partition.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND when the partition can not be found.

    STATUS_IO_DEVICE_ERRROR or STATUS_UNEXPECTED_IO_ERROR if error occurs
        while reading from the parent block i/o device.

--*/

{

    PARTP_MBR_TRAVERSAL_CALLBACK Callback;
    PARTP_DEVICE_HANDLE Handle;
    PMBR_PARTITION_TABLE_ENTRY MbrEntry;
    PBLOCK_IO_DEVICE ParentBlockIoEntry;
    PBLOCK_IO_INFORMATION ParentInfo;
    PVOID PartitionContext;
    PBLOCK_IO_INFORMATION PartitionInfo;
    NTSTATUS Status;

    //
    // Load the callback routine corresponding to the supplied partition
    // device structure format.
    //

    if (Device->DeviceType == DEVICE_PARTITION) {
        Callback = PartpMatchDeprecatedMbrPartition;
        PartitionContext = &Device->u.Partition;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        Callback = PartpMatchMbrPartition;
        PartitionContext = &Device->u.PartitionEx;

    } else {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Send the MBR partitions on this disk to the selected callback routine
    // for processing.  If the callback finds a partition matching the
    // partition identifier supplied by the caller, it will capture the
    // associated MBR partition table entry, the 1-based partition number, and
    // the partition offset into the appropriate fields within the partition
    // device structure.
    //
    // N.B. A return code of STATUS_SUCCESS indicates that all partitions were
    //      enumerated without a match generating STATUS_NO_MORE_MATCHES.
    //      This implies that no matching partitions were found.
    //

    Handle.AccessMethod = PARTP_USE_DEVICE_ENTRY;
    Handle.u.DeviceEntry = ParentDeviceEntry;
    Status = PartpTraverseMbrPartitions(&Handle,
                                        Callback,
                                        PartitionContext,
                                        PartitionEntry,
                                        NULL);

    if (Status != STATUS_NO_MORE_MATCHES) {
        if (Status == STATUS_SUCCESS) {
            Status = STATUS_NOT_FOUND;
        }

        return Status;
    }

    //
    // Fill in the rest of the block I/O information structure.
    //

    PartitionInfo = &PartitionEntry->Information;
    ParentBlockIoEntry = (PBLOCK_IO_DEVICE)ParentDeviceEntry->DeviceData;
    ParentInfo = &ParentBlockIoEntry->Information;
    MbrEntry = &PartitionInfo->DeviceInformation.Partition.Mbr.Entry;
    PartitionInfo->LastBlock = MbrEntry->NumberSectors - 1;
    PartitionInfo->Type = BLOCK_IO_PARTITION;
    PartitionInfo->PartitionType = MBR;
    PartitionInfo->BlockSize = ParentInfo->BlockSize;
    PartitionInfo->Alignment = ParentInfo->Alignment;
    PartitionInfo->IoAccessFlags = BLOCK_IO_ACCESS_FLAGS_NONE;
    PartitionInfo->DeviceFlags = BLOCK_IO_DEVICE_FLAGS_MEDIA_PRESENT;

    //
    // Copy firmware information and handles.
    //

    RtlCopyMemory(&PartitionEntry->FirmwareData,
                  &ParentBlockIoEntry->FirmwareData,
                  sizeof(BLOCK_IO_FIRMWARE_DATA));

    return STATUS_SUCCESS;
}

NTSTATUS
PartpMatchDeprecatedMbrPartition (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __in PPARTITION_IDENTIFIER PartitionIdentifier,
    __inout PPARTITION_DEVICE PartitionEntry,
    __in PVOID Context3
    )

/*++

Routine Description:

    This function is called for every MBR partition enumerated on a given
    disk.  Its function is to determine which partition matches the given
    partition identifier.  This routine operates on the deprecated structure
    that identifies MBR partitions using partition numbers rather than
    partition offsets.

Arguments:

    Context - Supplies a description of the partition being enumerated.

    PartitionIdentifier - Supplies an identifier describing the partition to
        match.

    PartitionEntry - Supplies a partition device structure to be updated
        with data describing the matching MBR partition.

    Context3 - Unused.

Return Value:

    STATUS_NO_MORE_MATCHES is returned if a match is made, indicating that
        partition enumeration should terminate.

    STATUS_SUCCESS is returned if a match is not made, indicating that
        partition enumeration should continue.

--*/

{

    PDEVICE_PARTITION_INFORMATION Information;
    ULONG PartitionNumber;

    UNREFERENCED_PARAMETER(Context3);

    //
    // Continue the enumeration if this partition does not match the given
    // partition identifier.
    //

    PartitionNumber = Context->PartitionNumber;
    if (PartitionNumber != PartitionIdentifier->Mbr.PartitionNumber) {
        return STATUS_SUCCESS;
    }

    //
    // The matching partition has been found.  Capture the 1-based number of
    // this partition, its MBR partition table entry, and its sector offset on
    // disk.
    //

    Information = &PartitionEntry->Information.DeviceInformation.Partition;
    Information->Mbr.Number = PartitionNumber;
    RtlCopyMemory(&Information->Mbr.Entry,
                  Context->MbrEntry,
                  sizeof(MBR_PARTITION_TABLE_ENTRY));

    PartitionEntry->InternalData.u.DiskData.StartLbaOffset =
        Context->PartitionOffset / Context->SectorSize;
    Information->PartitionOffset =
        PartitionEntry->InternalData.u.DiskData.StartLbaOffset;

    return STATUS_NO_MORE_MATCHES;
}

NTSTATUS
PartpMatchMbrPartition (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __in PPARTITION_IDENTIFIER_EX PartitionIdentifier,
    __inout PPARTITION_DEVICE PartitionEntry,
    __in PVOID Context3
    )

/*++

Routine Description:

    This function is called for every MBR partition enumerated on a given
    disk.  Its function is to determine which partition matches the given
    partition identifier.  When a match is detected, information on the
    partition is captured into the given partition device structure and the
    enumeration is terminated.

Arguments:

    Context - Supplies a description of the partition being enumerated.

    PartitionIdentifier - Supplies an identifier describing the partition to
        match.

    PartitionEntry - Supplies a partition device structure to be updated
        with data describing the matching MBR partition.

    Context3 - Unused.

Return Value:

    STATUS_NO_MORE_MATCHES is returned if a match is made, indicating that
        partition enumeration should terminate.

    STATUS_SUCCESS is returned if a match is not made, indicating that
        partition enumeration should continue.

--*/

{

    PDISK_INTERNAL_DATA DiskData;
    PDEVICE_PARTITION_INFORMATION Information;
    ULONG64 PartitionOffset;

    UNREFERENCED_PARAMETER(Context3);

    //
    // Continue the enumeration if this partition does not match the given
    // partition identifier.
    //

    PartitionOffset = Context->PartitionOffset;
    if (PartitionOffset != PartitionIdentifier->Mbr.PartitionOffset) {
        return STATUS_SUCCESS;
    }

    //
    // The matching partition has been found.  Capture the 1-based number of
    // this partition, its MBR partition table entry, and its sector offset on
    // disk.
    //

    Information = &PartitionEntry->Information.DeviceInformation.Partition;
    Information->Mbr.Number = Context->PartitionNumber;
    RtlCopyMemory(&Information->Mbr.Entry,
                  Context->MbrEntry,
                  sizeof(MBR_PARTITION_TABLE_ENTRY));

    DiskData = &PartitionEntry->InternalData.u.DiskData;
    DiskData->StartLbaOffset = PartitionOffset / Context->SectorSize;
    Information->PartitionOffset = DiskData->StartLbaOffset;
    return STATUS_NO_MORE_MATCHES;
}

NTSTATUS
PartpOpenGptPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    )

/*++

Routine Description:

    Finds/collects information/opens for the GPT partition described by
    PartitionEntry.  The parent device will be read in order to obtain
    the partition table.

    On output the following will have the correct data:
        PartitionEntry->Information
        PartitionEntry->InternalData.StartLbaOffset
        PartitionEntry->FirmwareData

Arguments:

    ParentDeviceEntry - Supplies the device entry for the device where the
        partition resides.

    Device - Supplies the device identifier describing the partition to open.
        This device may be of type DEVICE_PARTITION_EX or the deprecated
        DEVICE_PARTITION type.

    PartitionEntry - Supplies the device entry buffer for this partition.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a buffer could not be allocated for all partition
        entries.

    STATUS_NOT_FOUND when the partition can not be found.

    STATUS_IO_DEVICE_ERRROR or STATUS_UNEXPECTED_IO_ERROR if error occurs
        while reading from the parent block i/o device.

    STATUS_INVALID_PARAMETER if the GPT header is corrupt or larger than 4GB.

--*/

{

    ULONG BytesRead;
    PDISK_INTERNAL_DATA DiskData;
    PGPT_PARTITION_ENTRY Entry;
    GPT_PARTITION_TABLE GptHeader;
    ULONG Index;
    PBLOCK_IO_INTERNAL_DATA InternalData;
    PBLOCK_IO_DEVICE ParentDevice;
    PBLOCK_IO_INFORMATION ParentInfo;
    PGPT_PARTITION_ENTRY PartitionEntryArray;
    GUID UNALIGNED *PartitionSignature;
    PBLOCK_IO_INFORMATION PartInfo;
    UINTN Size;
    NTSTATUS Status;

    PartitionEntryArray = NULL;
    ParentDevice = (PBLOCK_IO_DEVICE)ParentDeviceEntry->DeviceData;
    PartInfo = &PartitionEntry->Information;
    ParentInfo = &ParentDevice->Information;
    InternalData = &PartitionEntry->InternalData;
    DiskData = &InternalData->u.DiskData;

    //
    // Read the GPT Partition Table from the parent device.
    //

    ParentInfo->LbaOffset = GPT_PARTITION_LBA;
    ParentInfo->BlockOffset = 0;
    Status = ParentDeviceEntry->FunctionTable.Read(ParentDeviceEntry,
                                                   &GptHeader,
                                                   sizeof(GPT_PARTITION_TABLE),
                                                   &BytesRead);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = BlockIoGetGPTTable(ParentDeviceEntry,
                                &GptHeader,
                                &PartitionEntryArray);

    if (!NT_SUCCESS(Status)) {
        goto PartpOpenGptPartitionEnd;
    }

    //
    // Iterate through the GPT Partition Entries searching for the partition
    // GUID specified in the supplied device identifier.
    //

    if (Device->DeviceType == DEVICE_PARTITION) {
        PartitionSignature = &Device->u.Partition.Gpt.PartitionSignature;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        PartitionSignature = &Device->u.PartitionEx.Gpt.PartitionSignature;

    } else {
        Status = STATUS_NOT_SUPPORTED;
        goto PartpOpenGptPartitionEnd;
    }

    for (Index = 0, Entry = PartitionEntryArray;
         (Index < GptHeader.PartitionCount);
         Index += 1, Entry = Add2Ptr(Entry, GptHeader.PartitionEntrySize)) {

        //
        // A GPT Partition is identified by a partition signature.  Check to
        // see if the current entrie's signature matches the one specified by
        // the caller.
        //

        Size = RtlCompareMemory(PartitionSignature,
                                &Entry->Id,
                                sizeof(Entry->Id));

        if (Size == sizeof(Entry->Id)) {

            //
            // If a match was found, save partition information and return
            // successfully.
            //

            //
            // Save GPT Partition Entry.
            //

            PartInfo->DeviceInformation.Partition.Gpt.Number = Index + 1;
            RtlCopyMemory(&PartInfo->DeviceInformation.Partition.Gpt.Entry,
                          Entry,
                          sizeof(GPT_PARTITION_ENTRY));

            //
            // Check if this is a patched partition.
            //

#ifdef ENABLE_PATCHED_PARTITIONS

            Status = PartpGetPatchInformation(GptHeader.PartitionCount,
                                              GptHeader.PartitionEntrySize,
                                              PartitionEntryArray,
                                              ParentInfo->BlockSize,
                                              (LPCGUID)PartitionSignature,
                                              DiskData);

            if (NT_SUCCESS(Status)) {

                NT_ASSERT(DiskData->PatchCount > 0);

                PartInfo->LastBlock =
                    ((DiskData->Patches[DiskData->PatchCount - 1].LogicalOffset +
                      DiskData->Patches[DiskData->PatchCount - 1].Length) /
                     ParentInfo->BlockSize) - 1;

                PartInfo->DeviceInformation.Partition.PartitionOffset =
                    DiskData->Patches[0].PhysicalOffset / ParentInfo->BlockSize;

            } else if (Status != STATUS_NOT_FOUND) {
                goto PartpOpenGptPartitionEnd;

            } else

#endif

            {
                DiskData->StartLbaOffset = Entry->StartingLBA;
                PartInfo->LastBlock = Entry->EndingLBA - Entry->StartingLBA;
                PartInfo->DeviceInformation.Partition.PartitionOffset =
                    Entry->StartingLBA;
            }

            //
            // Create rest of information structure.
            //

            PartInfo->Type = BLOCK_IO_PARTITION;
            PartInfo->DeviceFlags = BLOCK_IO_DEVICE_FLAGS_MEDIA_PRESENT;
            PartInfo->IoAccessFlags = BLOCK_IO_ACCESS_FLAGS_NONE;
            PartInfo->PartitionType = GPT;
            PartInfo->BlockSize = ParentInfo->BlockSize;
            PartInfo->Alignment = ParentInfo->Alignment;

            //
            // Copy firmware information/handles.
            //

            RtlCopyMemory(&PartitionEntry->FirmwareData,
                          &ParentDevice->FirmwareData,
                          sizeof(BLOCK_IO_FIRMWARE_DATA));

            Status = STATUS_SUCCESS;
            goto PartpOpenGptPartitionEnd;
        }
    }

    Status = STATUS_NOT_FOUND;

PartpOpenGptPartitionEnd:
    BlMmFreeHeap(PartitionEntryArray);
    return Status;
}

NTSTATUS
PartpOpenElToritoPartition (
    __in PDEVICE_ENTRY ParentDeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PPARTITION_DEVICE PartitionEntry
    )

/*++

Routine Description:

    Finds/collects information/opens for the ElTorito partition described by
    PartitionEntry.  The parent device will be read in order to obtain
    the partition table.

    On output the following will have the correct data:
        PartitionEntry->Information
        PartitoinEntry->InternalData.StartLbaOffset
        PartitionEntry->FirmwareData

Arguments:

    ParentDeviceEntry - Supplies the device entry for the device where the
        partition resides.

    Device - Supplies the device identifier describing the partition to open.

    PartitionEntry - Supplies the device entry buffer for this partition.

Return Value:

    TBD.

--*/

{

    UNREFERENCED_PARAMETER(ParentDeviceEntry);
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(PartitionEntry);

    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS
PartpCreatePartitionDeviceEntry (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out PDEVICE_ENTRY *DeviceEntry
    )

/*++

Routine Description:

    Creates a device entry for a partition device described by
    PartitionIdentifier.  This includes the allocation of memory for the device
    entry and it's initialization.

Arguments:

    PartitionIdentifier - Unique identifier describing the partition to open,
                          including the parent device that the partition is
                          located.

    DeviceEntry - Address to store the pointer to the device entry object
                  created for the partition.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if the parent device can not be opened.
    STATUS_NO_MEMORY when unable to allocate memory for the device entry.
    STATUS_INVALID_PARAMETER when the partition can not be found.

--*/

{

    NTSTATUS CloseStatus;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    PDISK_INTERNAL_DATA DiskData;
    PBLOCK_IO_DEVICE ParentDevice;
    PBLOCK_IO_IDENTIFIER ParentIdentifier;
    ULONG_PTR ParentOffset;
    PPARTITION_DEVICE PartitionDevice;
    PDEVICE_ENTRY ParentDeviceEntry;
    PDEVICE_ENTRY PartitionEntry;
    NTSTATUS Status;
    BOOLEAN VirtualPartition;

    //
    // Load a pointer to the parent identifier associated with the partition
    // being opened.  This allows easy access to the parent identifier despite
    // this routine supporting two incompatible partition identifier formats.
    //

    VirtualPartition = FALSE;
    if (Device->DeviceType == DEVICE_PARTITION) {
        ParentIdentifier = &Device->u.Partition.ParentIdentifier;
        ParentOffset = PtrOffset(&Device->u.Partition, ParentIdentifier);

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        ParentIdentifier = &Device->u.PartitionEx.ParentIdentifier;
        ParentOffset = PtrOffset(&Device->u.PartitionEx, ParentIdentifier);

    } else {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Initialize parent device entry before passing it to open.
    //

    ParentDeviceEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
    if (ParentDeviceEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(ParentDeviceEntry, sizeof(DEVICE_ENTRY));

    //
    // Build the device identifier for the partition's parent device.  We need
    // to open the parent device to read the partition table and find the
    // user specified partition.
    // Note: We need to allocate this memory, since we do not know if the size
    // of the parent device identifier is larger than our definition.  But we
    // do know that the required size of the identifier is less that the size
    // reported by Device since the parent identifier is embedded in the data.
    // Allocate a buffer of the same size to avoid some computation.  This also
    // allows us to reuse the buffer when creating the device identifier for
    // DeviceEntry.
    //

    DeviceIdentifier = BlMmAllocateHeap(Device->Size);
    if (DeviceIdentifier == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(DeviceIdentifier, Device->Size);

    //
    // Open the parent device.  We need to bypass the BlDevice API in order to
    // completely create the partition's device entry.  We need to get firmware
    // data from the parent's device entry.
    //

    DeviceIdentifier->DeviceType = DEVICE_BLOCK_IO;
    DeviceIdentifier->Size = (ULONG)(Device->Size - ParentOffset);
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "ParentIdentifier length is validated")
    RtlCopyMemory(&DeviceIdentifier->u.BlockIo,
                  ParentIdentifier,
                  DeviceIdentifier->Size);

    Status = BlockIoOpen(DeviceIdentifier, ParentDeviceEntry);
    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(DeviceIdentifier);
        return Status;
    }

    //
    // Allocate a device entry buffer for the partition.  This routine will
    // fill out this structure and return it to the caller.
    //

    PartitionEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
    if (PartitionEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartitionCreateMbrDeviceEntryEnd;
    }

    RtlZeroMemory(PartitionEntry, sizeof(DEVICE_ENTRY));

    //
    // Set the device's identifier to be the buffer allocated for the parent
    // identifier.  This temporary assignment will cause the buffer to be freed
    // if there is an error prior to us setting the real devices identifier.
    //

    PartitionEntry->DeviceIdentifier = DeviceIdentifier;

    //
    // Allocate a Partition Device Structure.
    //

    PartitionDevice = BlMmAllocateHeap(sizeof(PARTITION_DEVICE));
    if (PartitionDevice == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartitionCreateMbrDeviceEntryCleanup;
    }

    PartitionEntry->DeviceData = PartitionDevice;
    RtlZeroMemory(PartitionDevice, sizeof(PARTITION_DEVICE));

    //
    // Initialize internal data structures.
    //
    // Zeroed the memory buffer above, so everything is already intialized to
    // zero.
    //
    // PartitionDevice->Information.LbaOffset       = 0;
    // PartitionDevice->Information.BlockOffset     = 0;
    //

    PartitionDevice->Information.Type = BLOCK_IO_PARTITION;

    //
    // Save the partition identifer.  Reuse the buffer allocated for the
    // parent device identifier.
    //

    PartitionEntry->DeviceIdentifier = DeviceIdentifier;
    RtlCopyMemory(DeviceIdentifier, Device, Device->Size);

    //
    // Check whether partition resides on a virtual block I/O device.
    //

    ParentDevice = (PBLOCK_IO_DEVICE)ParentDeviceEntry->DeviceData;
    if (CHECK_FLAG(ParentDevice->Information.DeviceFlags,
                   BLOCK_IO_DEVICE_FLAGS_VIRTUAL_DEVICE) != FALSE) {

        VirtualPartition = TRUE;
    }

    //
    // Set the function table. Block I/O routines can be used for I/O, but
    // the close routine must be hooked to ensure the device handle is
    // properly closed.
    //

    RtlCopyMemory(&PartitionEntry->FunctionTable,
                  &BlockIoDeviceFunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    PartitionEntry->FunctionTable.Close = PartitionClose;

    //
    // Set the internal data for the partition. Only the parent device is
    // set here. The start LBA offset is set after the partition is located.
    //

    if (VirtualPartition != FALSE) {
        DiskData = &PartitionDevice->InternalData.u.DiskData;
        DiskData->ParentDevice = ParentDeviceEntry;
    }

    //
    // Fork based on partition type to find the actual partition and collect
    // all disk/partition information.
    //

    switch (ParentDevice->Information.PartitionType) {
    case MBR:
        Status = PartpOpenMbrPartition(ParentDeviceEntry,
                                       Device,
                                       PartitionEntry->DeviceData);
        break;

    case GPT:
        Status = PartpOpenGptPartition(ParentDeviceEntry,
                                       Device,
                                       PartitionEntry->DeviceData);
        break;

    case RAW:
        if (ParentIdentifier->BlockIoType == BLOCK_IO_CDROM) {
            Status = PartpOpenElToritoPartition(ParentDeviceEntry,
                                                Device,
                                                PartitionEntry->DeviceData);

        } else {
            Status = STATUS_INVALID_PARAMETER;
        }
    }

    if (!NT_SUCCESS(Status)) {
        goto PartitionCreateMbrDeviceEntryCleanup;
    }

    //
    // For virtual partitions, mark it as such in the device entry. This
    // information will be used when performing IO on the device.
    //
    // N.B. The flag needs to be set after the partition has been opened
    //      otherwise the partition open routines will overwrite this flag.
    //

    if (VirtualPartition != FALSE) {
        SET_FLAGS(PartitionDevice->Information.DeviceFlags,
                  BLOCK_IO_DEVICE_FLAGS_VIRTUAL_DEVICE);
    }

    //
    // Save geometry information from the parent device.
    //

    RtlCopyMemory(&PartitionDevice->Information.DeviceInformation.Disk,
                  &ParentDevice->Information.DeviceInformation.Disk,
                  sizeof(DEVICE_DISK_INFORMATION));

PartitionCreateMbrDeviceEntryCleanup:

    if (!NT_SUCCESS(Status)) {

        //
        // If we experienced any errors, free any memory allocated.
        //

        BlMmFreeHeap(PartitionEntry->DeviceIdentifier);
        BlockIopFreeAllocations((PBLOCK_IO_DEVICE)PartitionEntry->DeviceData);
        goto PartitionCreateMbrDeviceEntryEnd;
    }

    //
    // Set the caller's pointer to the new disk entry.
    //

    *DeviceEntry = PartitionEntry;

PartitionCreateMbrDeviceEntryEnd:
    CloseStatus = STATUS_SUCCESS;
    if (!NT_SUCCESS(Status) || (VirtualPartition == FALSE)) {
        CloseStatus = ParentDeviceEntry->FunctionTable.Close(ParentDeviceEntry);
        BlMmFreeHeap(ParentDeviceEntry);
    }

    return (NT_SUCCESS(Status) ? CloseStatus : Status);
}

NTSTATUS
PartpTraverseMbrPartitions (
    __in PPARTP_DEVICE_HANDLE ParentDeviceHandle,
    __in PARTP_MBR_TRAVERSAL_CALLBACK Callback,
    __in PVOID Context1,
    __in PVOID Context2,
    __in PVOID Context3
    )

/*++

Routine Description:

    Traverses the partitions on an MBR disk.  Information on each enumerated
    partition is passed to the callback function supplied by the caller.

    This routine will walk the partition tables of a MBR partitioned hard
    disk, enumerating the partitions which can be opened using the Boot
    Environment Library.

Arguments:

    ParentDeviceHandle - Supplies the device ID or device entry to use when
        issuing I/O requests to the harddisk being enumerated.

    Callback - Supplies the callback function that should invoked to process
        the partition offset of each partition found on disk.

    Context1, Context2, Context3 - Supplies opaque caller provided context
        that will be forwarded to the callback routine each time it is called.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERRROR or STATUS_UNEXPECTED_IO_ERROR if error occurs
        while reading from the parent block i/o device.

    Any error code returned by the callback routine is passed along as the
        result status of this function.

--*/

{

    ULONG AccessMethod;
    ULONG BytesRead;
    PARTP_MBR_TRAVERSAL_CONTEXT Context;
    ULONG Id;
    DEVICE_INFORMATION Information;
    PBLOCK_IO_DEVICE ParentBlockIoEntry;
    PDEVICE_ENTRY ParentDeviceEntry;
    PBLOCK_IO_INFORMATION ParentDeviceInformation;
    DEVICE_ID ParentDeviceId;
    ULONG64 PartitionSectorOffset;
    ULONG64 PartitionTableOffset;
    MBR_PARTITION_TABLE_ENTRY PartitionTable[MBR_PARTITION_TABLE_ENTRIES];
    UCHAR PartitionType;
    BOOLEAN PrimaryPartitionTable;
    BOOLEAN ReadExtendedPartition;
    ULONG64 SectorSize;
    NTSTATUS Status;
    ULONGLONG VolumeOffset;

    //
    // Initializer that is not functionally required but is needed to allow
    // this code to compile with unintialized variable checking enabled.
    //

    ParentDeviceEntry = NULL;

    //
    // This function will have to seek to the various partition tables
    // on the specified disk.  Populate the information data structure to
    // ensure no garbage data is passed to BlDeviceSetInformation when
    // seeking to the LBA of each table.
    //
    // N.B. Seeks are done through direct field manipulation when doing I/O
    //      through a device entry, making this step unnecessary in that case.
    //

    AccessMethod = ParentDeviceHandle->AccessMethod;
    if (AccessMethod == PARTP_USE_DEVICE_ID) {
        Status = BlDeviceGetInformation(ParentDeviceHandle->u.DeviceId,
                                        &Information);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        ParentDeviceInformation = &Information.u.BlockIo;

    } else if (AccessMethod == PARTP_USE_DEVICE_ENTRY) {
        ParentDeviceEntry = ParentDeviceHandle->u.DeviceEntry;
        ParentBlockIoEntry = (PBLOCK_IO_DEVICE)ParentDeviceEntry->DeviceData;
        ParentDeviceInformation = &ParentBlockIoEntry->Information;

    } else {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Walk the partition tables, invoking the callback routine for each valid
    // partition.
    //

    SectorSize = ParentDeviceInformation->BlockSize;
    Context.SectorSize = (ULONG)SectorSize;
    Context.PartitionNumber = 0;
    PartitionTableOffset = 0;
    VolumeOffset = 0;
    PrimaryPartitionTable = TRUE;
    do {
        ReadExtendedPartition = FALSE;

        //
        // Read the MBR Partition Table using the appropriate access method.
        //

        if (AccessMethod == PARTP_USE_DEVICE_ID) {
            ParentDeviceId = ParentDeviceHandle->u.DeviceId;
            Information.u.BlockIo.LbaOffset = PartitionTableOffset;
            Information.u.BlockIo.BlockOffset = MBR_PARTITION_TABLE_OFFSET;
            Status = BlDeviceSetInformation(ParentDeviceId, &Information);

            ASSERT(Status == STATUS_SUCCESS);

            Status = BlDeviceRead(ParentDeviceId,
                                  PartitionTable,
                                  sizeof(PartitionTable),
                                  &BytesRead);

        } else if (AccessMethod == PARTP_USE_DEVICE_ENTRY) {
            ParentDeviceInformation->LbaOffset = PartitionTableOffset;
            ParentDeviceInformation->BlockOffset = MBR_PARTITION_TABLE_OFFSET;
            Status = ParentDeviceEntry->FunctionTable.Read(
                         ParentDeviceEntry,
                         PartitionTable,
                         sizeof(PartitionTable),
                         &BytesRead);

        } else {
            Status = STATUS_NOT_SUPPORTED;
        }

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Enumerate each valid entry in the current partition table.
        //

        for (Id = 0; Id < MBR_PARTITION_TABLE_ENTRIES; Id += 1) {
            PartitionType = PartitionTable[Id].PartitionType;
            if ((PartitionType == MBR_PARTITION_ENTRY_UNUSED) ||
                IsContainerPartition(PartitionType)) {
                continue;
            }

            //
            // For GPT Protected MBR partitions, the partition number must be
            // incremented to maintain consistency with the operating system's
            // partition numbering scheme.
            //

            if (PartitionType == GPT_PROTECTED_MBR_ENTRY) {
                Context.PartitionNumber += 1;
                continue;
            }

            //
            // This is a valid partition.  Pass it to the supplied callback
            // routine.  Partition offsets for valid partitions are always
            // relative to the start of the containing partition table.
            //
            // N.B. This differs from the semantics applied to extended
            //      partition entries as described below.
            //

            PartitionSectorOffset = PartitionTableOffset +
                                    PartitionTable[Id].StartSector;

            Context.PartitionNumber += 1;
            Context.MbrEntry = &PartitionTable[Id];
            Context.PartitionOffset = PartitionSectorOffset * SectorSize;
            Status = Callback(&Context, Context1, Context2, Context3);
            if (!NT_SUCCESS(Status)) {
                return Status;
            }
        }

        //
        // Enumerate the partitions on an extended partition, if one exists.
        // The starting sector value reported for the first extended partition
        // is relative to the root MBR partition table.  This indicates the
        // location of the first extended partition table.
        //
        // The starting sector value reported for nested extended partitions
        // is always relative to the start of this first extended partition
        // table, termed the volume offset here, even if the extended
        // partition entries appear in nested extended partition tables
        // elsewhere on disk.
        //

        for (Id = 0; Id < MBR_PARTITION_TABLE_ENTRIES; Id++) {
            if (IsContainerPartition(PartitionTable[Id].PartitionType)) {
                PartitionTableOffset = VolumeOffset +
                                       PartitionTable[Id].StartSector;

                if (PrimaryPartitionTable == TRUE) {
                    VolumeOffset = PartitionTable[Id].StartSector;
                }

                ReadExtendedPartition = TRUE;

                //
                // Only one partition can be extended.  Loop back to the top
                // and read the extended partition table.
                //

                break;
            }
        }

        PrimaryPartitionTable = FALSE;

    } while (ReadExtendedPartition == TRUE);

    return STATUS_SUCCESS;
}

NTSTATUS
PartpCountMbrPartitions (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __inout PULONG RunningCounter,
    __in PVOID Context2,
    __in PVOID Context3
    )

/*++

Routine Description:

    This function is called for every MBR partition enumerated on a given
    disk.  Its function is to increment a running counter tracking the total
    number of partitions on this disk.

Arguments:

    Context - Supplies a description of the partition being enumerated.

    RunningCounter - Supplies a pointer to a variable which holds a running
        count of the number of MBR partitions on this disk.  This routine's
        only function is to increment this counter.

    Context2 - Unused.

    Context3 - Unused.

Return Value:

    STATUS_SUCCESS is returned to indicate that partition enumeration should
        continue.

--*/

{

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(Context2);
    UNREFERENCED_PARAMETER(Context3);

    *RunningCounter += 1;
    return STATUS_SUCCESS;
}

NTSTATUS
PartpOpenAllMbrPartitions (
    __in PPARTP_MBR_TRAVERSAL_CONTEXT Context,
    __inout PDEVICE_ID *CurrentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __inout PBOOT_ENVIRONMENT_DEVICE IdentifierTemplate
    )

/*++

Routine Description:

    This function is called for every MBR partition enumerated on a given
    disk.  Its function is to open a handle to each partition, storing the
    generated device ID value in the given collection buffer.

Arguments:

    Context - Supplies a description of the partition being enumerated.

    CurrentDeviceId - Supplies a pointer to a variable containing a pointer to
        the current location in the device ID collection buffer.  The device
        ID for the current partition is stored to this location and the pointer
        is updated to point to the next entry in the device ID buffer.

    DeviceIdBuffer - Supplies the generic buffer being used to collect the
        device ID for each partition on the disk being enumerated.  It is
        the caller's responsibility to ensure that this buffer is large enough
        to accomodate a device ID for each partition on the disk being
        examined.

    IdentifierTemplate - Supplies a partial device identifier of type
        DEVICE_PARTITION_EX that fully describes all properties of this
        partition except for the partition offset.

Return Value:

    STATUS_SUCCESS is returned to indicate that partition enumeration should
        continue if the partition is opened successfully.

    The device open failure code is returned if the device cannot be opened
        in order to indicate that partition enumeration should halt.

--*/

{

    ULONG64 PartitionOffset;
    NTSTATUS Status;

    ASSERT(IdentifierTemplate->DeviceType == DEVICE_PARTITION_EX);

    //
    // Complete the device identifier for this partition and attempt to open
    // it for read access.
    //

    PartitionOffset = Context->PartitionOffset;
    IdentifierTemplate->u.PartitionEx.Mbr.PartitionOffset = PartitionOffset;
    Status = BlDeviceOpen(IdentifierTemplate, OPEN_READ, *CurrentDeviceId);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The partition has been opened and the associated device ID has been
    // stored to the current location in the collection buffer.  Return
    // success after preparing the current device ID pointer and backing
    // buffer to receive the next device ID.
    //

    *CurrentDeviceId = Add2Ptr(*CurrentDeviceId, sizeof(DEVICE_ID));
    DeviceIdBuffer->DataSize += sizeof(DEVICE_ID);
    return STATUS_SUCCESS;
}

NTSTATUS
PartpEnumerateMbrPartitions (
    __in DEVICE_ID ParentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer
    )

/*++

Routine Description:

    Enumerates the partitions on a MBR disk.

    This routine will walk the partition tables of a MBR partitioned hard
    disk, enumerating the partitions which can (and will) be opened using
    the Boot Environment Library.

Arguments:

    ParentDeviceId - Device Id for the harddisk to enumerate.

    DeviceIdBuffer - Generic buffer structure for the array of device handles
        for all devices of the requested type.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_BUFFER_TOO_SMALL if DeviceIdBuffer is too small.

    STATUS_NO_MEMORY if a memory allocation fails.

    STATUS_IO_DEVICE_ERRROR or STATUS_UNEXPECTED_IO_ERROR if error occurs
        while reading from the parent block i/o device.

--*/

{

    BLOCK_IO_TYPE BlockIoType;
    PDEVICE_ID CurrentDeviceId;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    PARTP_DEVICE_HANDLE Handle;
    ULONG IdentifierSize;
    PBOOT_ENVIRONMENT_DEVICE ParentIdentifier;
    ULONG ParentSize;
    ULONG PartitionCount;
    NTSTATUS Status;

    //
    // Walk the partition tables, counting the number of valid partitions.
    //

    PartitionCount = 0;
    Handle.AccessMethod = PARTP_USE_DEVICE_ID;
    Handle.u.DeviceId = ParentDeviceId;
    Status = PartpTraverseMbrPartitions(&Handle,
                                        PartpCountMbrPartitions,
                                        &PartitionCount,
                                        NULL,
                                        NULL);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // If the caller did not supply enough buffer to accomodate all of the
    // partitions on this disk, then return STATUS_BUFFER_TOO_SMALL.
    //

    if (DeviceIdBuffer->BufferSize < (PartitionCount * sizeof(DEVICE_ID))) {
        DeviceIdBuffer->BufferSize = PartitionCount * sizeof(DEVICE_ID);
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // The device ID buffer is large enough to hold an ID for each partition
    // on disk.  Enumerate the partitions again, opening each one and
    // collecting the associated device IDs.
    //
    // The same device identifier structure is used when opening each device;
    // the callback routine simply fills in the appropriate partition offset.
    // To initially create the device structure, obtain the device identifier
    // for the parent and copy it to the partition's identifier.
    //

    Status = BlGetDeviceIdentifier(ParentDeviceId, &ParentIdentifier);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT(ParentIdentifier->DeviceType == DEVICE_BLOCK_IO);

    //
    // Build the partition device identifier. Set the parent identifier for
    // the partition by copying the it from the parent device.
    //
    // N.B. For virtual hard disk devices, the parent identifier is followed
    //      by other device identifiers, which also need to be copied.
    //

    BlockIoType = ParentIdentifier->u.BlockIo.BlockIoType;
    if (BlockIoType != BLOCK_IO_VIRTUAL_HARD_DISK) {
        IdentifierSize = sizeof(BOOT_ENVIRONMENT_DEVICE);
        ParentSize = sizeof(BLOCK_IO_IDENTIFIER);

    } else {
        ParentSize = ParentIdentifier->Size;
        ParentSize -= FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo);
        IdentifierSize = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.PartitionEx) +
            FIELD_OFFSET(PARTITION_IDENTIFIER_EX, ParentIdentifier) +
            ParentSize;
    }

    DeviceIdentifier = BlMmAllocateHeap(IdentifierSize);
    if (DeviceIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto EnumerateMbrPartitionsEnd;
    }

    RtlZeroMemory(DeviceIdentifier, IdentifierSize);
    DeviceIdentifier->DeviceType = DEVICE_PARTITION_EX;
    DeviceIdentifier->Size = IdentifierSize;
    RtlCopyMemory(&DeviceIdentifier->u.PartitionEx.ParentIdentifier,
                  &ParentIdentifier->u.BlockIo,
                  ParentSize);

    //
    // Copy a valid device handle into the caller's buffer for each enumerated
    // partition.
    //

    CurrentDeviceId = DeviceIdBuffer->Buffer;
    DeviceIdBuffer->DataSize = 0;
    Status = PartpTraverseMbrPartitions(&Handle,
                                        PartpOpenAllMbrPartitions,
                                        &CurrentDeviceId,
                                        DeviceIdBuffer,
                                        DeviceIdentifier);

EnumerateMbrPartitionsEnd:
    if (DeviceIdentifier != NULL) {
        BlMmFreeHeap(DeviceIdentifier);
    }

    return Status;
}

NTSTATUS
PartpEnumerateGptPartitions (
    __in DEVICE_ID ParentDeviceId,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN ForceFveCheck
    )

/*++

Routine Description:

    Enumerates the partitions on a GPT disk.

    When the output buffer is sufficiently size, this routine will enumerate
    and open every partition on a GPT disk.

Arguments:

    ParentDeviceId - Device Id for the harddisk to enumerate.

    DeviceIdBuffer - Generic buffer structure for the array of device handles
        for all devices of the requested type.

    ForceFveCheck - Supplies a boolean indicating if the OPEN_NO_FVE should
        be skipped.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_BUFFER_TOO_SMALL if DeviceIdBuffer is too small.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_IO_DEVICE_ERRROR or
    STATUS_UNEXPECTED_IO_ERROR if error occurs while reading from the parent
        block i/o device.
    STATUS_INVALID_PARAMETER if the GPT header is corrupt or larger than 4GB.

--*/

{

    BLOCK_IO_TYPE BlockIoType;
    PDEVICE_ID CurrDeviceId;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    ULONG DeviceOpenFlags;
    PGPT_PARTITION_ENTRY Entry;
    PGPT_PARTITION_ENTRY EntryArray;
    ULONG EntryArraySize;
    GPT_PARTITION_TABLE GptHeader;
    GUID UNALIGNED *GuidId;
    ULONG IdentifierSize;
    ULONG Index;
    DEVICE_INFORMATION Information;
    PDEVICE_ID LastAllocDeviceId;
    GUID NullGuid;
    PBOOT_ENVIRONMENT_DEVICE ParentIdentifier;
    ULONG ParentSize;
    NTSTATUS Status;

    DeviceIdentifier = NULL;

    //
    // Populate the information data structure to ensure no garbage data is
    // passed to BlDeviceSetInformation when seeking to the LBA of each table.
    //

    Status = BlDeviceGetInformation(ParentDeviceId, &Information);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Read the GPT partition table header from the parent device.  The
    // header will state the number of partitions in the disk.
    //

    Information.u.BlockIo.LbaOffset = GPT_PARTITION_LBA;
    Information.u.BlockIo.BlockOffset = 0;
    Status = BlDeviceSetInformation(ParentDeviceId, &Information);

    ASSERT(Status == STATUS_SUCCESS);

    Status = BlDeviceRead(ParentDeviceId,
                          &GptHeader,
                          sizeof(GPT_PARTITION_TABLE),
                          NULL);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The caller's buffer is sufficiently large to store a device ID for
    // each enumerated partition.  In order to open each, the entire partition
    // table must be read into memory.  Ensure that the header's value does
    // not overflow a 32 bit integer.
    //

    Status = RtlULongMult(GptHeader.PartitionEntrySize,
                          GptHeader.PartitionCount,
                          &EntryArraySize);

    if (!NT_SUCCESS(Status)) {
        return STATUS_INVALID_PARAMETER;
    }

    EntryArray = BlMmAllocateHeap(EntryArraySize);
    if (EntryArray == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Read all of the GPT partition table entries into memory.
    //

    Information.u.BlockIo.LbaOffset = GptHeader.PartitionEntryLBA;
    Information.u.BlockIo.BlockOffset = 0;
    Status = BlDeviceSetInformation(ParentDeviceId, &Information);

    ASSERT(Status == STATUS_SUCCESS);

    Status = BlDeviceRead(ParentDeviceId,
                          EntryArray,
                          EntryArraySize,
                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto PartpEnumerateGptPartitionsEnd;
    }

    //
    // Open each partition on the disk.
    //
    // Use the same device structure when opening each device; simply update
    // the partition GUID.  To initially create the device structure, obtain
    // the device identifier for the parent and copy it to the partition's
    // identifier.
    //

    Status = BlGetDeviceIdentifier(ParentDeviceId, &ParentIdentifier);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT(ParentIdentifier->DeviceType == DEVICE_BLOCK_IO);

    BlockIoType = ParentIdentifier->u.BlockIo.BlockIoType;
    if (BlockIoType != BLOCK_IO_VIRTUAL_HARD_DISK) {
        IdentifierSize = sizeof(BOOT_ENVIRONMENT_DEVICE);
        ParentSize = sizeof(BLOCK_IO_IDENTIFIER);

    } else {
        ParentSize = ParentIdentifier->Size;
        ParentSize -= FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo);
        IdentifierSize = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.PartitionEx) +
            FIELD_OFFSET(PARTITION_IDENTIFIER_EX, ParentIdentifier) +
            ParentSize;
    }

    DeviceIdentifier = BlMmAllocateHeap(IdentifierSize);
    if (DeviceIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartpEnumerateGptPartitionsEnd;
    }

    RtlZeroMemory(DeviceIdentifier, IdentifierSize);
    DeviceIdentifier->DeviceType = DEVICE_PARTITION_EX;
    DeviceIdentifier->Size = IdentifierSize;
    RtlCopyMemory(&DeviceIdentifier->u.PartitionEx.ParentIdentifier,
                  &ParentIdentifier->u.BlockIo,
                  ParentSize);

    //
    // Iterate over each entry in the partition table, opening each partition
    // and storing the device handle in the caller's buffer.
    //

    GuidId = &DeviceIdentifier->u.PartitionEx.Gpt.PartitionSignature;
    RtlZeroMemory(&NullGuid, sizeof(GUID));
    DeviceIdBuffer->DataSize = 0;
    CurrDeviceId = DeviceIdBuffer->Buffer;
    LastAllocDeviceId = Add2Ptr(DeviceIdBuffer->Buffer,
                                DeviceIdBuffer->BufferSize);

    for (Index = 0, Entry = EntryArray;
         (Index < GptHeader.PartitionCount);
         Index += 1, Entry = Add2Ptr(Entry, GptHeader.PartitionEntrySize)) {

        //
        // todo: Check with VM guys about the expected value of PartitionCount.
        //       It seems like this value is padded such that PartitionCount *
        //       PartitionEntrySize is modulo BlockSize.  But then you can't
        //       easily determine the actual number of partition entries.
        //       Had the product not been padded to fill a block, we wouldn't
        //       have open each device in order to determine if the caller's
        //       buffer is sufficiently sized.
        //

        if (RtlEqualMemory(&NullGuid, &Entry->Id, sizeof(GUID))) {
            continue;
        }

        RtlCopyMemory(GuidId, &Entry->Id, sizeof(GUID));

        //
        // If the caller's buffer is too small to store the partition's
        // device id, don't even attempt to open the device.  This may cause
        // the caller to allocate a slightly larger buffer than required.
        // The partition won't be read from, so specify OPEN_NO_FVE
        // to indicate that FVE detection is not needed, unless ForceFveCheck
        // is set.
        //

        if (CurrDeviceId < LastAllocDeviceId) {
            DeviceOpenFlags = OPEN_READ;
            if (ForceFveCheck == FALSE) {
                DeviceOpenFlags |= OPEN_NO_FVE;
            }

            Status = BlDeviceOpen(DeviceIdentifier,
                                  DeviceOpenFlags,
                                  CurrDeviceId);

            //
            // Skip over any invalid partition.
            //

            if (!NT_SUCCESS(Status)) {
                Status = STATUS_SUCCESS;  // Don't return error status.
                continue;
            }

            CurrDeviceId = Add2Ptr(CurrDeviceId, sizeof(DEVICE_ID));
            DeviceIdBuffer->DataSize += sizeof(DEVICE_ID);

        } else {
            DeviceIdBuffer->BufferSize += sizeof(DEVICE_ID);
            Status = STATUS_BUFFER_TOO_SMALL;
        }
    }

PartpEnumerateGptPartitionsEnd:
    if (DeviceIdentifier != NULL) {
        BlMmFreeHeap(DeviceIdentifier);
    }

    BlMmFreeHeap(EntryArray);
    return Status;
}

//
// ------------------------------------------------ Virtual Partition Functions
//

NTSTATUS
PartpOpenVirtualPartitionDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    This routine opens and initializes a virtual partition. The parent device
    will be opened to determine the partition type and its offset. On success,
    this routine will intialize the device data and function table within the
    supplied device entry.

Arguments:

    Device - Supplies the partition device to open and initialize.

    DeviceEntry - Supplies a pointer to the device entry to be initialized.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if the parent device can not be opened.
    STATUS_NO_MEMORY when unable to allocate memory for the device entry.
    STATUS_INVALID_PARAMETER when the partition can not be found.

--*/

{

    ULONG Id;
    PDEVICE_ENTRY PartitionEntry;
    NTSTATUS Status;

    //
    // Attempt to open the specified partition. On success, this will return
    // a fully intialized device entry.
    //

    Status = PartpCreatePartitionDeviceEntry(Device, &PartitionEntry);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Save entry in the BlockIoDeviceTable. This will improve performance
    // in cases where a partition is opened multiple times.
    //

    Status = BlTblSetEntry(&BlockIoDeviceTable,
                           &BlockIoDeviceTableEntries,
                           PartitionEntry,
                           &Id,
                           TblDoNotPurgeEntry);

    //
    // On failure, invoke the partition's close routine to get it closed.
    // The close routine is responsible for only releasing the device data
    // buffer. The buffer allocated for device identifier and the device
    // entry need to be freed separately.
    //

    if (!NT_SUCCESS(Status)) {
        PartitionEntry->FunctionTable.Close(PartitionEntry);
        BlMmFreeHeap(PartitionEntry->DeviceIdentifier);
        BlMmFreeHeap(PartitionEntry);
        return Status;
    }

    //
    // Copy the contents of the cached entry to the caller's buffer.
    //

    RtlCopyMemory(DeviceEntry->DeviceData,
                  PartitionEntry->DeviceData,
                  sizeof(PARTITION_DEVICE));

    //
    // Initialize the device function table.
    //
    // N.B. Do not copy the device close routine. The partition entry created
    //      above might have hooked the close routine to clean up all the
    //      resources allocated for the partition (including shared resources)
    //      when it gets closed. If the close routine is copied, all the shared
    //      resources may get cleaned up  when the partition entry being
    //      returned to the caller gets closed, even though the entry in the
    //      block I/O device table still references them. Patch the close
    //      routine to point to the generic block I/O close routine.
    //

    RtlCopyMemory(&DeviceEntry->FunctionTable,
                  &PartitionEntry->FunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    DeviceEntry->FunctionTable.Close = BlockIoClose;
    return STATUS_SUCCESS;
}

#ifdef ENABLE_PATCHED_PARTITIONS

BOOLEAN
PartpMatchPatchPartition (
    __in PGPT_PARTITION_ENTRY Entry,
    __in PWSTR GuidString
    )

/*++

Routine Description:

    This routine checks if supplied partition entry is a patch for a partition
    identified by guid string.

Arguments:

    Entry - Partition entry from the GPT partition table.

    GuidString - Id of the partition to match in the string form.

Return Value:

    Boolean indicating is partition entry is a matching patch.

--*/

{

    //
    // Check if this is a patched partition with a matching name.
    //

    if (IsEqualGUID(&Entry->Type, &PARTITION_PATCH_GUID) == FALSE) {
        return FALSE;
    }

    if (wcsncmp(GuidString,
                (PWSTR)Entry->Name,
                PATCHED_PARTITION_NAME_SIZE) == 0) {

        return TRUE;
    }

    return FALSE;
}

INT
__cdecl
PartpPatchCompare (
    __in const VOID *Left,
    __in const VOID *Right
    )

/*++

Routine Description:

    This routine compares two partition patch entries by logical LBA offset.

Arguments:

    Left - The left entry for comparison.

    Right - The right entry for comparison.

Return Value:

    < 0, 0, or > 0 as standard for comparison routines.

--*/

{

    const PARTITION_PATCH *LeftPatch;
    const PARTITION_PATCH *RightPatch;

    LeftPatch = Left;
    RightPatch = Right;
    if (LeftPatch->LogicalOffset < RightPatch->LogicalOffset) {
        return -1;

    } else if (LeftPatch->LogicalOffset == RightPatch->LogicalOffset) {
        return 0;

    } else {
        return 1;
    }
}

NTSTATUS
PartpGetPatchInformation (
    __in ULONG PartitionCount,
    __in ULONG PartitionEntrySize,
    __in PGPT_PARTITION_ENTRY PartitionEntryArray,
    __in ULONG BlockSize,
    __in LPCGUID PartitionGuid,
    __inout PDISK_INTERNAL_DATA DiskData
    )

/*++

Routine Description:

    This routine will check if partition identified by PartitionGUid is patched,
    and populate patch information if it is.

Arguments:

    GptHeader - Header of the GPT partition table.

    PartitionEntryArray - Entries of the GPT partition table.

    BlockSize - Block size for the device.

    PartitionGuid - Guid of the partition to query.

    DiskData - Receives patch ifnormation, if partition is patched.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND when no matching patch entries where found.

    STATUS_DISK_CORRUPT_ERROR when partition data is malformed.

    STATUS_NO_MEMORY when unable to allocate memory for the patch information.

--*/

{

    PGPT_PARTITION_ENTRY Entry;
    ULONG Index;
    WCHAR PartitionName[PATCHED_PARTITION_NAME_SIZE + 1];
    ULONG PatchArraySize;
    ULONG PatchCount;
    ULONG PatchIndex;
    PPARTITION_PATCH Patches;
    NTSTATUS Status;

    //
    // Convert partition guid to string form for comparison.
    //

    swprintf_s(PartitionName,
               RTL_NUMBER_OF(PartitionName),
               PATCHED_PARTITION_NAME_FORMAT,
               PartitionGuid->Data1,
               PartitionGuid->Data2,
               PartitionGuid->Data3,
               PartitionGuid->Data4[0],
               PartitionGuid->Data4[1],
               PartitionGuid->Data4[2],
               PartitionGuid->Data4[3],
               PartitionGuid->Data4[4],
               PartitionGuid->Data4[5],
               PartitionGuid->Data4[6],
               PartitionGuid->Data4[7]);

    //
    // Count number of patches with matching name.
    //

    PatchCount = 0;
    for (Index = 0, Entry = PartitionEntryArray;
         (Index < PartitionCount);
         Index += 1, Entry = Add2Ptr(Entry, PartitionEntrySize)) {

        if (PartpMatchPatchPartition(Entry, PartitionName) != FALSE) {
            PatchCount += 1;
        }
    }

    //
    // If no patches have been found there is nothing to do.
    //

    if (PatchCount == 0) {
        Status = STATUS_NOT_FOUND;
        goto GetPartitionPatchInfoEnd;
    }

    //
    // If patches are present allocate array to hold patch information.
    //

    PatchArraySize = PatchCount * sizeof(PARTITION_PATCH);
    Patches = BlMmAllocateHeap(PatchArraySize);
    if (Patches == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetPartitionPatchInfoEnd;
    }

    RtlZeroMemory(Patches, PatchArraySize);

    //
    // Populate patch information array.
    //

    for (Index = 0, PatchIndex = 0, Entry = PartitionEntryArray;
         ((Index < PartitionCount) && (PatchIndex < PatchCount));
         Index += 1, Entry = Add2Ptr(Entry, PartitionEntrySize)) {

        if (PartpMatchPatchPartition(Entry, PartitionName) != FALSE) {
            Patches[PatchIndex].LogicalOffset =
                GET_PATCH_PARTITION_LOGICAL_OFFSET(Entry->Attributes);

            Patches[PatchIndex].PhysicalOffset = Entry->StartingLBA * BlockSize;
            Patches[PatchIndex].Length =
                (Entry->EndingLBA + 1 - Entry->StartingLBA) * BlockSize;

            RtlCopyMemory(&Patches[PatchIndex].PartitionGuid, 
                          Entry->Id,
                          sizeof(GUID));

            PatchIndex += 1;
        }
    }

    //
    // Sort patches by logical offset.
    //

    qsort(Patches,
          PatchCount,
          sizeof(PARTITION_PATCH),
          PartpPatchCompare);

    //
    // Validate patch information:
    //
    // 1. Patched partition should have a logical offset of 0.
    // 2. Patched partition should be logically contiguous.
    //

    if (Patches[0].LogicalOffset != 0) {
        BlMmFreeHeap(Patches);
        Status = STATUS_DISK_CORRUPT_ERROR;
        goto GetPartitionPatchInfoEnd;
    }

    for (Index = 1; Index < PatchCount; Index += 1) {
        if (Patches[Index].LogicalOffset !=
            (Patches[Index - 1].LogicalOffset + Patches[Index - 1].Length)) {

            BlMmFreeHeap(Patches);
            Status = STATUS_DISK_CORRUPT_ERROR;
            goto GetPartitionPatchInfoEnd;
        }
    }

    //
    // Populate DiskData with patch information.
    //

    DiskData->Patches = Patches;
    DiskData->PatchCount = PatchCount;
    Status = STATUS_SUCCESS;

GetPartitionPatchInfoEnd:
    return Status;
}

VOID
PartpReleasePatchData (
    __inout PDISK_INTERNAL_DATA DiskData 
    )

/*++

Routine Description:

    This routine closes any devices associated with patch partition and frees
    patch data.

Arguments:

    DiskData - Supplies pointer to the disk information structure.

Return Value:

    None.

--*/

{

    ULONG Index;

    if (DiskData->Patches != NULL) {

        NT_ASSERT(DiskData->PatchCount > 0);

        for (Index = 0; Index < DiskData->PatchCount; Index += 1) {
            if (DiskData->Patches[Index].DeviceEntry != NULL) {
                BlockIoClose(DiskData->Patches[Index].DeviceEntry);
            }
        }

        BlMmFreeHeap(DiskData->Patches);
        DiskData->Patches = NULL;
        DiskData->PatchCount = 0;
    }

    return;
}

#endif

