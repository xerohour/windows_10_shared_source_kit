/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    disk.c

Abstract: 
    
    Implementation of disk device i/o routines using EFI.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "disk.h"

// ------------------------------------------------------------------ Functions

NTSTATUS
DiskFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a disk device and collects information, such as disk geometry for the
    device. 
    
Arguments:

    Device - The Disk device to open and initialize.
             
    DeviceEntry - Pointer to the device entry.  Will be filled with all 
                  information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE when the requested device could not be found.
    STATUS_NO_MEMORY if allocation fails for an internal data structure.
    
--*/

{

    NTSTATUS Status;

    //
    // On EFI machines, it is easiest to process all block i/o devices at once.
    // They all share a common interface and can be obtained by querying for
    // devices that support the block i/o protocol.
    //

    Status = BlockIoFirmwareOpen(Device, DeviceEntry->DeviceData);
    return Status;
}

NTSTATUS
DiskFirmwareClose (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    Closes an EFI disk device.  
     
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

    PDISK_INTERNAL_DATA DiskData;

    //
    // Release partition table, if allocated.
    //

#ifdef ENABLE_PATCHED_PARTITIONS

    DiskData = &BlockIoDevice->InternalData.u.DiskData;
    if (DiskData->GptData.PartitionEntryArray != NULL) {
        BlMmFreeHeap(DiskData->GptData.PartitionEntryArray);
        DiskData->GptData.PartitionEntryArray = NULL;
    }

#endif

    return EfiCloseProtocol(BlockIoDevice->FirmwareData.Handle,
                            &EfiBlockIoProtocol,
                            BlockIoDevice->FirmwareData.BlockIoInterface);
}


