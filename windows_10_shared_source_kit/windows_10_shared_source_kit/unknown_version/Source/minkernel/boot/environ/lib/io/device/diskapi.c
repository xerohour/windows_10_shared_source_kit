/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    diskapi.c

Abstract:

    Implementation of boot library disk i/o api.  These routines make up the
    disk device's function table for disk i/o.  The actual block i/o is
    firmware dependent and exists in firmware stubs.

Environment:

    boot

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include "disk.h"

// -------------------------------------------------------------------- Globals

//
// Function table for Disk I/O.  Exported to the device api's for setting
// up device entries.  For disk i/o, we actually don't have to hook any device
// routines.  The block i/o routines will be sufficient.  The only specific
// routines needed are firmware routines to open and close the disks.
// DiskOpen will use the firmware to open the disk, and BlockIoTableDestroy
// will call the firmware stub to do any firmware specific tasks to close the
// device.
//

const DEVICE_FUNCTION_TABLE DiskDeviceFunctionTable =
{
    (PDEVICE_ENUMERATE_ROUTINE)       DiskEnumerateDeviceClass,
    (PDEVICE_OPEN_ROUTINE)            DiskOpen,
    (PDEVICE_CLOSE_ROUTINE)           DiskClose,
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
DiskOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a Disk device.  Collects disk information, such as disk geometry,
    for the device.

Arguments:

    Device - The Disk device to open and initialize.

    DeviceEntry - Pointer to the device entry.  Will be filled with all
                  information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if the requested device could not be found.
    STATUS_NO_MEMORY if allocation for internal data structure fails.

--*/

{

    NTSTATUS Status;

    //
    // Open device using the firmware.  This routine will also initialize
    // internal data and device information.
    //

    Status = DiskFirmwareOpen(Device, DeviceEntry);
    return Status;
}

NTSTATUS
DiskClose (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Closes a disk device.  This function both performs any firmware
    steps necessary to close a device as well as freeing all memory allocated
    for the device.

Arguments:

    DeviceEntry - Pointer to the device entry.

Return Value:

    STATUS_SUCCESS always.
    STATUS_INVALID_PARAMETER if data structure could not be freed.
    Other errors are possible depending on the implementation of the close
        routine for a firmware.

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = STATUS_SUCCESS;

    Status = DiskFirmwareClose(DeviceEntry->DeviceData);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlockIopFreeAllocations(DeviceEntry->DeviceData);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    return ReturnStatus;
}

NTSTATUS
DiskEnumerateDeviceClass (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    Enumerates a specified device class.  If the device identifier specifies
    a sub-class (like in the case of a block i/o device), the subclass is
    enumerated.

    For each device instance in the device class, a device handle is returned.
    The buffer in DeviceIdBuffer is an array of DEVICE_ID's where the size
    field = Number of Device Handles * sizeof(DEVICE_ID).

    If AllocateNewBuffer is TRUE, this routine will allocate sufficient
    memory for all device handles.  The caller is responsible for freeing
    any memory allocated by this routine.

Arguments:

    DeviceClassIdentifier - A boot environment device structure used to
        represent the device class to enumerate.  Only Device Class / Sub
        Class information is referenced.  (ie. DeviceType, BlockIoType).

    DeviceIdBuffer - Generic buffer structure for the array of device handles
        for all devices of the requested type.

    AllocateNewBuffer - Boolean value indicating whether the device handle
        buffer should be allocated.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if any allocation fails.
    STATUS_BUFFER_TOO_SMALL if DeviceIdBuffer is too small and
        AllocateNewBuffer is FALSE.

--*/

{

    BOOT_ENVIRONMENT_DEVICE Device;
    DEVICE_ID DeviceId;
    BLOCK_IO_TYPE EnumerateType;
    ULONG Id;
    PBOOT_ENVIRONMENT_DEVICE Identifier;
    ULONG NumberOfDevices;
    NTSTATUS Status;

    EnumerateType = DeviceClassIdentifier->u.BlockIo.BlockIoType;

    //
    // This routine leverages both the use of a the Block I/O Device Table
    // to cache devices opened via the firmware as well as the manner in
    // which devices are enumerated by the firmware.
    // This routine successfully enumerates devices in which a list of
    // firmware enumerated devices are processed until a match is found.
    // Using this fact, it is possible to process all firmware enumerated
    // devices by supplying an identifier for a device that does not exist.
    // Each processed device will be cached in the Block I/O Device Table
    // and can be returned to the caller.
    //

    RtlZeroMemory(&Device, sizeof(BOOT_ENVIRONMENT_DEVICE));
    Device.DeviceType = DEVICE_BLOCK_IO;
    Device.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    Device.u.BlockIo.BlockIoType = EnumerateType;

    //
    // Attempt to open an arbitrary device with the device class type to
    // enumerate.
    //

    Device.u.BlockIo.u.Cdrom.CdromNumber = 0;

    do {

        //
        // The block i/o device type was already specified.  Use the Cdrom
        // identifier just because it's identifier is a ULONG.  Cycle through
        // values until we get STATUS_NO_SUCH_DEVICE.  This indicates that
        // every firmware enumerated device of this particular device type
        // has been processed.  Therefore, there is an entry in the Block I/O
        // Device Table for each of these processed devices.
        //

        Device.u.BlockIo.u.Cdrom.CdromNumber -= 1;
        Status = BlDeviceOpen(&Device, OPEN_READ, &DeviceId);

    } while ( Status != STATUS_NO_SUCH_DEVICE );

    //
    // At this point, there is an entry for each device (of the specified
    // device class) in the block i/o device table.  Walk the table to
    // determine the number of devices.  Use this value to determine if the
    // specified buffer is large enough to return an array of handles (one
    // for each device).
    //

    for (Id = 0, NumberOfDevices = 0; Id < BlockIoDeviceTableEntries; Id += 1) {
        if (BlockIoDeviceTable[Id] != NULL) {
            Identifier = BlockIoDeviceTable[Id]->DeviceIdentifier;

            if ((Identifier->DeviceType == DEVICE_BLOCK_IO)  &&
                (Identifier->u.BlockIo.BlockIoType == EnumerateType)) {

                NumberOfDevices += 1;
            }
        }
    }

    if (DeviceIdBuffer->BufferSize < NumberOfDevices * sizeof(DEVICE_ID)) {

        //
        // If the caller did not request a new buffer to be allocated, then
        // return STATUS_BUFFER_TOO_SMALL.
        //

        if (AllocateNewBuffer == FALSE) {
            DeviceIdBuffer->BufferSize = NumberOfDevices * sizeof(DEVICE_ID);
            return STATUS_BUFFER_TOO_SMALL;
        }

        //
        // Allocate a larger buffer.
        //

        if (DeviceIdBuffer->BufferSize != 0) {
            BlMmFreeHeap(DeviceIdBuffer->Buffer);
        }

        DeviceIdBuffer->BufferSize = NumberOfDevices * sizeof(DEVICE_ID);
        DeviceIdBuffer->Buffer = BlMmAllocateHeap(DeviceIdBuffer->BufferSize);
        if (DeviceIdBuffer->Buffer == NULL) {
            return STATUS_NO_MEMORY;
        }
    }

    //
    // Return a list of device's with the specified device class by creating
    // an array of device handles for each device.  The handles returned
    // need to be from the device table, not the cached block i/o device table.
    // Therefore, we must open each device to force an entry to be inserted
    // in the device table.
    //

    for (Id = 0, NumberOfDevices = 0; Id < BlockIoDeviceTableEntries; Id += 1) {
        if (BlockIoDeviceTable[Id] != NULL) {
            Identifier = BlockIoDeviceTable[Id]->DeviceIdentifier;

            if ((Identifier->DeviceType == DEVICE_BLOCK_IO)  &&
                (Identifier->u.BlockIo.BlockIoType == EnumerateType)) {

                Status = BlDeviceOpen(Identifier, OPEN_READ, &DeviceId);

                ASSERT(Status == STATUS_SUCCESS);

                ((PDEVICE_ID)DeviceIdBuffer->Buffer)[NumberOfDevices] = DeviceId;
                NumberOfDevices += 1;
            }
        }
    }

    DeviceIdBuffer->DataSize = NumberOfDevices * sizeof(DEVICE_ID);
    return STATUS_SUCCESS;
}

