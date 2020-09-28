/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    partition.c

Abstract:

    Implementation of partition device i/o routines using EFI.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "efiblock.h"
#include "partition.h"

// ----------------------------------------------------------------- Prototypes

BOOLEAN
PartitionFwpIsMatch (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath,
    __in EFI_HANDLE Handle
    );

NTSTATUS
PartitionFwpCreateDeviceEntry (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_HANDLE Handle,
    __deref_out PDEVICE_ENTRY *DeviceEntry
    );

NTSTATUS
PartitionFwpGetPartitionInformation (
    __inout PPARTITION_DEVICE PartitionDevice,
    __in PBLOCK_IO_IDENTIFIER ParentIdentifier
    );

#ifdef ENABLE_PATCHED_PARTITIONS

NTSTATUS
PartitionFwpGetPatchInformation (
    __inout PPARTITION_DEVICE PartitionDevice,
    __in PBLOCK_IO_IDENTIFIER ParentIdentifier
    );

#endif

// ------------------------------------------------------------------ Functions

NTSTATUS
PartitionFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens (and initializes) partition using EFI block i/o protocol.

    Each partition on a block i/o device will also contain a block i/o
    interface.  This interface will be used and therefore a partition shares
    all the same routines as a block i/o device (minus the Open routine).

    Despite partitions having a block i/o device interface, their open routine
    must be specialized.  Each device contains a device path, with partitions
    containing a special device path node describing the partition.
    Unfortunetly, there is no information describing the parent device.
    In fact, a hard disk open routine must read from the device to determine
    its disk signature since this is not derivable from an EFI device path.

    N.B. A partition is treated seperately from a standard block i/o
         device to allow the open/detection code to inherit parent device
         information from the caller.  This is particularly useful when an
         application is loaded from the firmware.  In this case, there is no
         easy way to determine the parent device signature.  However, it is
         not required, since on a GPT disk requires each partition to have
         a unique partition signature.  Therefore, the partition signature is
         sufficient and the complexity of determining the disk signature can
         be avoided.

Arguments:

    Device - The partition to open and initialize.

    DeviceEntry - Pointer to the device entry.  Will be filled with all
                  information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE when the requested device could not be found.
    STATUS_NO_MEMORY if allocation fails for an internal data structure.

--*/

{

    EFI_HANDLE *BlockIoHandleArray;
    EFI_DEVICE_PATH *DevicePath;
    BOOLEAN FoundMatch;
    DEVICE_ID Id;
    ULONG Index;
    UINTN NumberDevices;
    PDEVICE_ENTRY PartitionEntry;
    NTSTATUS Status;

    //
    // Get the list of handles to devices that support the Block I/O protocol.
    //

    Status = BlockIoEfiGetBlockIoHandleArray(&BlockIoHandleArray,
                                             &NumberDevices);

    if (!NT_SUCCESS(Status)) {
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Find the handle which describes the partition requested by the caller.
    // Every Block I/O Device is required to support the device path protocol.
    // The interface for this protocol contains a binary device path describing
    // the location of the device.  All partitions end with a node of type
    // MEDIA_DEVICE_PATH.  This fact can be used to filter out other device
    // handles.  A media device path node contains information describing the
    // partition and can be used to determine if the current handle is for
    // the requested partition device.
    //

    for (Index = 0, FoundMatch = FALSE;
         (Index < NumberDevices) && (FoundMatch == FALSE);
         Index += 1) {

        Status = EfiOpenProtocol(BlockIoHandleArray[Index],
                                 &EfiDevicePathProtocol,
                                 &DevicePath);

        if (!NT_SUCCESS(Status)) {
            ASSERT(Status == STATUS_NOT_SUPPORTED);
            continue;
        }

        FoundMatch = PartitionFwpIsMatch(Device,
                                         DevicePath,
                                         BlockIoHandleArray[Index]);

        Status = EfiCloseProtocol(BlockIoHandleArray[Index],
                                  &EfiDevicePathProtocol,
                                  DevicePath);

        ASSERT(Status == STATUS_SUCCESS);
    }

    //
    // If an EFI device was found for the specified device, allocate and
    // initialize a device entry structure for the device.
    //

    if (FoundMatch != FALSE) {
        Status = PartitionFwpCreateDeviceEntry(Device,
                                               BlockIoHandleArray[Index - 1],
                                               &PartitionEntry);

        if (!NT_SUCCESS(Status)) {
            goto PartitionFirmwareOpenEnd;
        }

        //
        // Cache the device entry structure in the global Block I/O Device
        // Table.
        //

        Status = BlTblSetEntry(&BlockIoDeviceTable,
                               &BlockIoDeviceTableEntries,
                               PartitionEntry,
                               &Id,
                               TblDoNotPurgeEntry);

        if (!NT_SUCCESS(Status)) {
            BlMmFreeHeap(PartitionEntry->DeviceIdentifier);
            BlMmFreeHeap(PartitionEntry->DeviceData);
            BlMmFreeHeap(PartitionEntry);
            goto PartitionFirmwareOpenEnd;
        }

        //
        // Copy the contents of the cached entry to the caller's buffer.
        //

        RtlCopyMemory(DeviceEntry->DeviceData,
                      PartitionEntry->DeviceData,
                      sizeof(PARTITION_DEVICE));

    } else {
        Status = STATUS_NOT_FOUND;
    }

PartitionFirmwareOpenEnd:

    BlMmFreeHeap(BlockIoHandleArray);
    return Status;
}

NTSTATUS
PartitionFirmwareClose (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    Closes an EFI partition device.

    On EFI 1.10 systems this is to call CloseProtocol to notify the EFI handle
    database that there the handle is no longer being used.

Arguments:

    DeviceEntry - Block I/O device structure.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if the stored firmware handle is no longer valid.
    STATUS_NOT_FOUND if the firmware handle no longer supports the block i/o
        protocol.

    Both error cases result in some sort of internal error (Handle is
    corrupted).

--*/

{

    return EfiCloseProtocol(BlockIoDevice->FirmwareData.Handle,
                            &EfiBlockIoProtocol,
                            BlockIoDevice->FirmwareData.BlockIoInterface);
}

// ------------------------------------------------------------ Local Functions

NTSTATUS
PartitionFwpFlushOrReset (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONG Operation
    )

/*++

Routine Description:

    Sends flush or reset command to the physical device through EFI Block I/O
    interface. If partition is made up of patches, the command is forwared
    to the underlying devices for each of the patches.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

    Operation - Identifies operation to be performed.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing the
        requested operation.
    STATUS_NO_MEDIA when there is no media in the device.

--*/

{

    EFI_BLOCK_IO *BlockIoInterface;
    PDISK_INTERNAL_DATA DiskData;
    NTSTATUS OperationStatus;
    ULONG Index;
    PPARTITION_DEVICE PartitionDevice;
    NTSTATUS Status;

    NT_ASSERT((Operation == BLOCK_IO_RESET) ||
              (Operation == BLOCK_IO_FLUSH));

    PartitionDevice = DeviceEntry->DeviceData;

#ifdef ENABLE_PATCHED_PARTITIONS

    DiskData = &PartitionDevice->InternalData.u.DiskData;
    if (DiskData->PatchCount > 0) {
        Status = STATUS_SUCCESS;
        for (Index = 0; Index < DiskData->PatchCount; Index += 1) {
            if (DiskData->Patches[Index].DeviceEntry != NULL) {
                if (Operation == BLOCK_IO_FLUSH) {
                    OperationStatus = BlpDeviceFlush(DiskData->Patches[Index].DeviceEntry);

                } else {
                    OperationStatus = BlpDeviceReset(DiskData->Patches[Index].DeviceEntry);
                }

                if ((Status == STATUS_SUCCESS) && 
                    !NT_SUCCESS(OperationStatus)) {
                    
                    Status = OperationStatus;
                }
            }
        }

    } else 

#endif

    {
        BlockIoInterface = PartitionDevice->FirmwareData.BlockIoInterface;
        if (Operation == BLOCK_IO_FLUSH) {
            Status = EfiBlockIoFlushBlocks(BlockIoInterface);
            
        } else {
            Status = EfiBlockIoReset(BlockIoInterface, TRUE);
        }
    }

    return Status;
}

NTSTATUS
PartitionFwpReset (
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
    return PartitionFwpFlushOrReset(DeviceEntry, BLOCK_IO_RESET);
}

NTSTATUS
PartitionFwpFlush (
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

    return PartitionFwpFlushOrReset(DeviceEntry, BLOCK_IO_FLUSH);
}

NTSTATUS
PartitionFwpRetrievePartitionOffset (
    __in EFI_HANDLE Handle,
    __in HARDDRIVE_DEVICE_PATH UNALIGNED *HardDriveNode,
    __out PULONG64 PartitionOffset
    )

/*++

Routine Description:

    This function determines the byte offset of the partition associated with
    the given handle within its enclosing disk.

Arguments:

    Handle - Supplies a handle to the partition device to query.

    HardDriveNode - Supplies the hard drive component of the device path of
        this partition device.

    PartitionOffset - Supplies a pointer to a variable that, on success,
        receives the offset in bytes of the given partition device within
        its enclosing disk.

Return Value:

    STATUS_SUCCESS is returned if the partition offset is computed
        successfully.

    Translated EFI error codes are returned if any EFI library calls fail
        during the computation.

--*/

{

    EFI_BLOCK_IO *BlockIoInterface;
    NTSTATUS Status;

    //
    // The hard drive device path node contains the logical block
    // address of this partition within the containing disk.  In
    // order to compare this against the byte offset in the given
    // partition identifier, the block size for this device must
    // be loaded using the block I/O protocol.
    //

    Status = EfiOpenProtocol(Handle,
                             &EfiBlockIoProtocol,
                             &BlockIoInterface);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    *PartitionOffset = HardDriveNode->PartitionStart *
                       BlockIoInterface->Media->BlockSize;

    Status = EfiCloseProtocol(Handle,
                              &EfiBlockIoProtocol,
                              BlockIoInterface);

    ASSERT(Status == STATUS_SUCCESS);

    return STATUS_SUCCESS;
}

BOOLEAN
PartitionFwpIsMatch (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath,
    __in EFI_HANDLE Handle
    )

/*++

Routine Description:

    Determines if an EFI device path describes the caller specified boot
    environment device.  If a match is found, a positive (non-zero) value
    is returned.

Arguments:

    Device - Supplies the device to compare an EFI device path against.

    DevicePath - Supplies an EFI device path describing the location of a
        device.

    Handle - Supplies the EFI handle associated with the supplied device path.

Return Value:

    TRUE if the EFI device path describes the specified device.

    FALSE otherwise.

--*/

{

    HARDDRIVE_DEVICE_PATH UNALIGNED *HardDriveNode;
    HARDDRIVE_DEVICE_PATH UNALIGNED *FirstHardDriveNode;
    EFI_DEVICE_PATH UNALIGNED *LeafNode;
    PBLOCK_IO_IDENTIFIER Parent;
    ULONG64 PartitionByteOffset;
    ULONG UNALIGNED *PartitionNumber;
    ULONG64 UNALIGNED *PartitionOffset;
    GUID UNALIGNED *PartitionSignature;
    ULONG Signature;
    NTSTATUS Status;
    BOOLEAN UsingDeprecatedDefinition;

    //
    // Initializers that are not functionally required but are needed to allow
    // this code to compile with unintialized variable checking enabled.
    //

    PartitionNumber = NULL;
    PartitionOffset = NULL;

    //
    // This routine can only handle partition devices.  Therefore, the last
    // node must be of type, MEDIA_DEVICE_PATH.
    //

    LeafNode = EfiGetLeafNode(DevicePath);
    if (LeafNode->Type != MEDIA_DEVICE_PATH) {
        return FALSE;
    }

    //
    // Two incompatible partition identifier formats are supported by this
    // routine.  Bypass the incompatible structure layouts by loading pointers
    // directly referencing the relevant internal fields.
    //

    if (Device->DeviceType == DEVICE_PARTITION) {
        UsingDeprecatedDefinition = TRUE;
        PartitionSignature = &Device->u.Partition.Gpt.PartitionSignature;
        PartitionNumber = &Device->u.Partition.Mbr.PartitionNumber;
        Parent = &Device->u.Partition.ParentIdentifier;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        UsingDeprecatedDefinition = FALSE;
        PartitionSignature = &Device->u.PartitionEx.Gpt.PartitionSignature;
        PartitionOffset = &Device->u.PartitionEx.Mbr.PartitionOffset;
        Parent = &Device->u.PartitionEx.ParentIdentifier;

    } else {
        return FALSE;
    }

    //
    // A Media Device Path Node can either describe a harddisk or a CD
    // partition.  These map directly into the boot environment library's
    // definition of a partition.
    //

    if ((LeafNode->SubType == MEDIA_HARDDRIVE_DP) &&
        (Parent->BlockIoType == BLOCK_IO_HARD_DISK)) {

        HardDriveNode = (HARDDRIVE_DEVICE_PATH UNALIGNED *)LeafNode;
        switch (HardDriveNode->SignatureType) {
        case SIGNATURE_TYPE_MBR:
            Signature = *((ULONG UNALIGNED *)HardDriveNode->Signature);

            //
            // Many EFI implementations return zero or garbage as the signature
            // for the extended MBR partition - it is necessary to match the
            // signature from the first hard drive node in the device path
            // (i.e. the primary MBR partition table node).
            //

            if ((Device->DeviceType == DEVICE_PARTITION_EX) ||
                (Device->DeviceType == DEVICE_PARTITION)) {

                FirstHardDriveNode = EfiGetFirstHardDriveNode(DevicePath);
                if (FirstHardDriveNode != NULL) {
                    Signature =
                        *((ULONG UNALIGNED *)FirstHardDriveNode->Signature);
                }
            }

            if ((Parent->u.HardDisk.PartitionType != MBR) ||
                (Parent->u.HardDisk.Mbr.Signature != Signature)) {

                return FALSE;
            }

            //
            // Determine whether this MBR partition matches the given
            // identifier.  The deprecated identifier definition uses the
            // partition number for this purpose, the new identifier
            // definition uses the byte offset of the partition within the
            // disk.
            //

            if (UsingDeprecatedDefinition != FALSE) {
                if (*PartitionNumber != HardDriveNode->PartitionNumber) {
                    return FALSE;
                }

            } else {

                //
                // Determine whether the byte offset associated with this
                // partition device matches the given device identifier.
                //

                Status = PartitionFwpRetrievePartitionOffset(
                             Handle,
                             HardDriveNode,
                             &PartitionByteOffset);

                if (!NT_SUCCESS(Status)) {
                    return FALSE;
                }

                if (*PartitionOffset != PartitionByteOffset) {
                    return FALSE;
                }
            }

            return TRUE;

        case SIGNATURE_TYPE_GUID:
            if ((Parent->u.HardDisk.PartitionType != GPT) ||
                RtlCompareMemory(PartitionSignature,
                                 &HardDriveNode->Signature,
                                 sizeof(GUID)) != sizeof(GUID)) {

                return FALSE;
            }

            return TRUE;
        }
    }

    return FALSE;
}

NTSTATUS
PartitionFwpCreateDeviceEntry (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_HANDLE Handle,
    __deref_out PDEVICE_ENTRY *DeviceEntry
    )

/*++

Routine Description:

    Allocates and initializes a device entry structure for an EFI partition
    device.

Arguments:

    Device - Supplies the boot environment device structure describing the
        partition device.

    Handle - Supplies the EFI handle for the partition.

    DeviceEntry - Supplies a pointer to a variable that receives the device
        entry object created for the partition.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY when unable to allocate memory for the device entry.

--*/

{

    PBLOCK_IO_IDENTIFIER Parent;
    PPARTITION_DEVICE PartitionDevice;
    PDEVICE_ENTRY PartitionEntry;
    NTSTATUS Status;

    //
    // Allocate all the necessary memory for a device entry.
    //

    PartitionEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
    if (PartitionEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    RtlZeroMemory(PartitionEntry, sizeof(DEVICE_ENTRY));

    PartitionEntry->DeviceData = BlMmAllocateHeap(sizeof(PARTITION_DEVICE));
    if (PartitionEntry->DeviceData == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    PartitionEntry->DeviceIdentifier = BlMmAllocateHeap(Device->Size);
    if (PartitionEntry->DeviceIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    //
    // An EFI partition device can use the Block I/O APIs for all I/O
    // routines.  Close should still be hooked to ensure a handle gets properly
    // closed.
    //

    RtlCopyMemory(&PartitionEntry->FunctionTable,
                  &BlockIoDeviceFunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    PartitionEntry->FunctionTable.Close = PartitionClose;

    //
    // Override 'Reset' and 'Flush' methods of the block device with those
    // available through the EFI protocol.
    //

    PartitionEntry->FunctionTable.Reset = PartitionFwpReset;
    PartitionEntry->FunctionTable.Flush = PartitionFwpFlush;

    //
    // Copy the device identifier into the allocated buffer.  It is assumed
    // that the specified device identifier is accurate and no detection is
    // required at this point.
    //

    RtlCopyMemory(PartitionEntry->DeviceIdentifier, Device, Device->Size);

    //
    // Initialize the partition device structure.  This is the internal
    // structure used by the Block I/O APIs.  All information can be derived
    // from the input parameters.
    //

    PartitionDevice = PartitionEntry->DeviceData;
    RtlZeroMemory(PartitionDevice, sizeof(PARTITION_DEVICE));

    //
    // A partition device is divided into three internal structures, one
    // containing internal data, one the common device information and the
    // last containing firmware specific information.  Data from each can
    // be derived from any number of the input parameters.  The device handle
    // is required to obtain the Block I/O Interface which is requird for the
    // firmware data structure.  But the Block I/O Interface is used to
    // populate the majority information structure.  Lastly, partition type
    // is derived from the device identifier.
    //

    PartitionDevice->FirmwareData.Handle = Handle;
    PartitionDevice->Information.Type = BLOCK_IO_PARTITION;

    Status = BlockIoEfiGetBlockIoInformation(PartitionDevice);
    if (!NT_SUCCESS(Status)) {
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    //
    // Load the appropriate parent identifier pointer based upon the supplied
    // partition identifier type.
    //

    if (Device->DeviceType == DEVICE_PARTITION) {
        Parent = &Device->u.Partition.ParentIdentifier;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        Parent = &Device->u.PartitionEx.ParentIdentifier;

    } else {
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    if (Parent->BlockIoType == BLOCK_IO_HARD_DISK) {
        PartitionDevice->Information.PartitionType =
            Parent->u.HardDisk.PartitionType;

    } else {
        PartitionDevice->Information.PartitionType = RAW;
    }

    //
    // Non-EFI Boot Application's store a copy of the a partition's entry in
    // the partition table (MBR or GPT) into the device's information data
    // structure.  Since the detection of an EFI partition is not done by
    // reading the partition table, this step is not done.  However, on GPT
    // disk, the partition number should be saved to allow the translation
    // from a Boot Environment Device structure to an ARC device.
    //

    Status = PartitionFwpGetPartitionInformation(PartitionDevice, Parent);
    if (!NT_SUCCESS(Status)) {
        goto PartitionFwpCreateDeviceEntryEnd;
    }

    //
    // Return the allocated device entry to the caller.
    //

    *DeviceEntry = PartitionEntry;

PartitionFwpCreateDeviceEntryEnd:

    if (!NT_SUCCESS(Status)) {
        if (PartitionEntry != NULL) {
            if (PartitionEntry->DeviceData != NULL) {
                BlMmFreeHeap(PartitionEntry->DeviceData);
            }

            if (PartitionEntry->DeviceIdentifier != NULL) {
                BlMmFreeHeap(PartitionEntry->DeviceIdentifier);
            }

            BlMmFreeHeap(PartitionEntry);
        }
    }

    return Status;
}

NTSTATUS
PartitionFwpGetPartitionInformation (
    __inout PPARTITION_DEVICE PartitionDevice,
    __inout PBLOCK_IO_IDENTIFIER ParentIdentifier
    )

/*++

Routine Description:

    Populates the partition number and partition offset fields for a partition.

    N.B. The partition number is part of the deprecated DEVICE_PARTITION
         identifier structure.  Since the partition number was available
         directly in the identifier, this routine previously only needed to
         run for GPT partitions.  This routine now runs for MBR partitions in
         order to make the partition number available since identifiers now
         contain no partition number and use partition offsets instead.

Arguments:

    PartitionDevice - Supplies the partition device structure to update.


    ParentIdentifier - Supplies block io identifier for the parent device
        where the partition resides.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    PDEVICE_PARTITION_INFORMATION DeviceInformation;
    EFI_DEVICE_PATH UNALIGNED *DevicePath;
    HARDDRIVE_DEVICE_PATH UNALIGNED *HdNode;
    ULONG Number;
    PBLOCK_IO_INFORMATION PartInfo;
    ULONG64 PartitionOffset;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    PartitionOffset = 0;

    //
    // Obtain the EFI Device Path for the partition device.  The leaf node
    // should be a Hard Drive node which describes the partition.
    //
    // N.B. The device path has already been obtained once for the partition.
    //      Therefore, this request must succeed.
    //
    
    PartInfo = &PartitionDevice->Information;
    NT_VERIFY(
        NT_SUCCESS(
            EfiOpenProtocol(PartitionDevice->FirmwareData.Handle,
                            &EfiDevicePathProtocol,
                            &DevicePath)));

    HdNode = (HARDDRIVE_DEVICE_PATH UNALIGNED *)EfiGetLeafNode(DevicePath);
    Number = HdNode->PartitionNumber;

    //
    // Retrieve partition offset and store it in DeviceInformation.
    //

    NT_VERIFY(
        NT_SUCCESS(
            PartitionFwpRetrievePartitionOffset(
                PartitionDevice->FirmwareData.Handle,
                HdNode,
                &PartitionOffset)));

    ASSERT(PartInfo->BlockSize != 0);

    DeviceInformation = &PartInfo->DeviceInformation.Partition;
    DeviceInformation->PartitionOffset = PartitionOffset / PartInfo->BlockSize;
    if (PartInfo->PartitionType == MBR) {
        DeviceInformation->Mbr.Number = Number;

    } else if (PartInfo->PartitionType == GPT) {
        DeviceInformation->Gpt.Number = Number;

        //
        // Record partition signature, and check if this is a patched partition.
        //

#ifdef ENABLE_PATCHED_PARTITIONS

        RtlCopyMemory(DeviceInformation->Gpt.Entry.Id,
                      HdNode->Signature,
                      sizeof(GUID));

        Status = PartitionFwpGetPatchInformation(PartitionDevice, 
                                                 ParentIdentifier);

#endif

    }

    NT_VERIFY(
        NT_SUCCESS(
            EfiCloseProtocol(PartitionDevice->FirmwareData.Handle,
                             &EfiDevicePathProtocol,
                             DevicePath)));

    return Status;
}

#ifdef ENABLE_PATCHED_PARTITIONS

NTSTATUS
PartitionFwpGetPatchInformation (
    __inout PPARTITION_DEVICE PartitionDevice,
    __in PBLOCK_IO_IDENTIFIER ParentIdentifier
    )

/*++

Routine Description:

    This routine checks for patched partitions, and populates patch information
    if required.

Arguments:

    PartitionDevice - Supplies the partition device structure to update.

    ParentIdentifier - Supplies block I/O identifier for the disk where the
        partition resides.

Return Value:

    NTSTATUS.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    BOOT_ENVIRONMENT_DEVICE Device;
    PDISK_INTERNAL_DATA DiskData;
    ULONG Index;
    PDISK_INTERNAL_DATA ParentDiskData;
    DEVICE_ENTRY ParentDevice;
    PBLOCK_IO_INFORMATION PartInfo;
    LPGUID PartitionGuid;
    NTSTATUS Status;

    //
    // Only hard disk devices can have patches.
    //

    if (ParentIdentifier->BlockIoType != BLOCK_IO_HARD_DISK) {
        return STATUS_SUCCESS;;
    }

    //
    // Open disk device containing the partition.
    //
    // N.B. If open fails assume that there are no patches
    //      and return success status code.
    //

    RtlZeroMemory(&ParentDevice, sizeof(DEVICE_ENTRY));
    RtlZeroMemory(&Device, sizeof(Device));
    Device.DeviceType = DEVICE_BLOCK_IO;
    Device.Size = sizeof(Device);
    RtlCopyMemory(&Device.u.BlockIo, 
                  ParentIdentifier,
                  sizeof(BLOCK_IO_IDENTIFIER));

    Status = BlockIoOpen(&Device, &ParentDevice);
    if (!NT_SUCCESS(Status)) {
        return STATUS_SUCCESS;
    }

    //
    // Check if partition contains patches.
    //

    BlockIoDevice = (PBLOCK_IO_DEVICE)ParentDevice.DeviceData;
    ParentDiskData = &BlockIoDevice->InternalData.u.DiskData;
    DiskData = &PartitionDevice->InternalData.u.DiskData;
    PartInfo = &PartitionDevice->Information;

    PartitionGuid = 
        (LPGUID)PartInfo->DeviceInformation.Partition.Gpt.Entry.Id;

    Status = PartpGetPatchInformation(ParentDiskData->GptData.PartitionCount,
                                      ParentDiskData->GptData.PartitionEntrySize,
                                      ParentDiskData->GptData.PartitionEntryArray,
                                      PartInfo->BlockSize,
                                      PartitionGuid,
                                      DiskData);

    BlockIoClose(&ParentDevice);

    //
    // If no patches where found, this is not a patched partition and there is
    // nothing to do. If getting patch information failed, return failure
    // status, othervice fix-up device information and open a device for each
    // patch partition.
    //

    if (Status == STATUS_NOT_FOUND) {
        Status = STATUS_SUCCESS;
        goto FwpGetPatchInformationEnd;

    } else if (!NT_SUCCESS(Status)) {
        goto FwpGetPatchInformationEnd;
    }

    NT_ASSERT(DiskData->PatchCount > 0);

    //
    // Recompute last block LBA.
    //

    PartInfo->LastBlock =
        ((DiskData->Patches[DiskData->PatchCount - 1].LogicalOffset +
          DiskData->Patches[DiskData->PatchCount - 1].Length) /
         PartInfo->BlockSize) - 1;

    //
    // Resolve patches to corresponding partition devices.
    //

    RtlZeroMemory(&Device, sizeof(Device));
    Device.Size = sizeof(Device);
    Device.DeviceType = DEVICE_PARTITION_EX; 
    PartitionGuid = &Device.u.PartitionEx.Gpt.PartitionSignature;
    RtlCopyMemory(&Device.u.PartitionEx.ParentIdentifier, 
                  ParentIdentifier,
                  sizeof(BLOCK_IO_IDENTIFIER));

    for (Index = 0; Index < DiskData->PatchCount; Index += 1) {
        DiskData->Patches[Index].PhysicalOffset = 0;
        *PartitionGuid = DiskData->Patches[Index].PartitionGuid;
        Status = BlockIoOpen(&Device, 
                             &DiskData->Patches[Index].PatchDevice);

        if (!NT_SUCCESS(Status)) {
            goto FwpGetPatchInformationEnd;
        }

        DiskData->Patches[Index].DeviceEntry = 
            &DiskData->Patches[Index].PatchDevice;
    }

    Status = STATUS_SUCCESS;

FwpGetPatchInformationEnd:
    if (!NT_SUCCESS(Status)) {
        PartpReleasePatchData(DiskData);
    }

    return Status;
}

#endif

