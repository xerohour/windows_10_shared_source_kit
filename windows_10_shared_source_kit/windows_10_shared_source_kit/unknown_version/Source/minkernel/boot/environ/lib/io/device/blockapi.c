/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    blockapi.c

Abstract:

    Implementation of boot library block i/o api.  These routines may fork off
    into other devices that support block i/o.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "block.h"

// -------------------------------------------------------------------- Globals

//
// Function table for Block I/O.  Exported to the device api's for setting
// up device entries.
//

const DEVICE_FUNCTION_TABLE BlockIoDeviceFunctionTable =
{
    (PDEVICE_ENUMERATE_ROUTINE)       BlockIoEnumerateDeviceClass,
    (PDEVICE_OPEN_ROUTINE)            BlockIoOpen,
    (PDEVICE_CLOSE_ROUTINE)           BlockIoClose,
    (PDEVICE_READ_ROUTINE)            BlockIoReadUsingCache,
    (PDEVICE_WRITE_ROUTINE)           BlockIoWrite,
    (PDEVICE_GET_INFORMATION_ROUTINE) BlockIoGetInformation,
    (PDEVICE_SET_INFORMATION_ROUTINE) BlockIoSetInformation,
    (PDEVICE_RESET_ROUTINE)           NULL,
    (PDEVICE_FLUSH_ROUTINE)           NULL,
    (PDEVICE_CREATE_ROUTINE)          BlockIoCreate
};

BOOLEAN BlockIoInitialized;

NTSTATUS
BlockIoOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a Block I/O device by calling the open routine for the block i/o
    subtype.  This routine may additionally hook routines in the device's
    function table.

Arguments:

    Device - The Disk device to open and initialize.

    DeviceEntry - Pointer to the device entry.  Will be filled with all
                  information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if unsuccessful in allocating buffer for Block I/O
        Device or any other internal data structures.
    STATUS_INVALID_PARAMETER if Device describes an invalid Block I/O
        Device type.
    STATUS_NOT_FOUND if the requested Device was not found.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    ULONG Id;
    NTSTATUS Status;
    PDEVICE_ENTRY TableDeviceEntry;

    //
    // Initialize the module on the first BlockIoOpen call.
    //

    if (BlockIoInitialized == FALSE) {
        Status = BlockIopInitialize();
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    //
    // Copy the Block I/O Function Table into the Device Entry.
    //

    RtlCopyMemory(&DeviceEntry->FunctionTable,
                  &BlockIoDeviceFunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    //
    // Allocate the Block I/O Device Structure.
    //

    BlockIoDevice = BlMmAllocateHeap(sizeof(BLOCK_IO_DEVICE));
    if (BlockIoDevice == NULL) {
        return STATUS_NO_MEMORY;
    }

    DeviceEntry->DeviceData = BlockIoDevice;

    //
    // Attempt to find a match in the block i/o device entry table.
    //

    TableDeviceEntry = BlTblFindEntry(BlockIoDeviceTable,
                                      BlockIoDeviceTableEntries,
                                      &Id,
                                      BlockIoDeviceTableCompare,
                                      Device,
                                      NULL,
                                      NULL,
                                      NULL);

    if (TableDeviceEntry != NULL) {

        //
        // If we found an entry, copy it's block i/o device structure and
        // function table into the device manager's entry.  Do not copy the
        // entire device entry because some of the data in the device entry
        // pertains to the table it is located.
        //

        RtlCopyMemory(BlockIoDevice,
                      TableDeviceEntry->DeviceData,
                      sizeof(BLOCK_IO_DEVICE));

        //
        // Do not copy the device close routine.  The device table could have
        // multiple entries for a device in addition to the entry found in the
        // block i/o device table.  A device class implementation could be
        // using a firmware handle and calling the device specific close
        // routine more than once has indeterminate behavior.  Use the
        // convention that entries in the block I/O device table can call
        // a device specific close routine.  Entries in the device table
        // should call BlockIoClose to simply free memory allocated for the
        // device entry.
        //
        // N.B. This problem extends past block I/O devices.  Currently there
        //      is no way to avoid closing a console device or a UART multiple
        //      times.  A real solution is required.
        //

        RtlCopyMemory(&DeviceEntry->FunctionTable,
                      &TableDeviceEntry->FunctionTable,
                      sizeof(DEVICE_FUNCTION_TABLE));

        DeviceEntry->FunctionTable.Close = BlockIoDeviceFunctionTable.Close;
        Status = STATUS_SUCCESS;

    } else {

        //
        // If we did not find the entry, call the Open routine for the device
        // type.
        //

        //
        // Initialize the newly allocated Block I/O Device Entry.
        //

        RtlZeroMemory(BlockIoDevice, sizeof(BLOCK_IO_DEVICE));
        switch (Device->DeviceType) {
        case DEVICE_BLOCK_IO:
            switch (Device->u.BlockIo.BlockIoType) {
            case BLOCK_IO_HARD_DISK:
            case BLOCK_IO_REMOVABLE_DISK:
            case BLOCK_IO_CDROM:
                Status = DiskDeviceFunctionTable.Open(Device, DeviceEntry);
                break;

            case BLOCK_IO_FILE:
                Status = FileDeviceFunctionTable.Open(Device, DeviceEntry);
                break;

            case BLOCK_IO_RAMDISK:
                Status = RamDiskDeviceFunctionTable.Open(Device, DeviceEntry);
                break;

            case BLOCK_IO_VIRTUAL_HARD_DISK:
                Status = VirtualDiskDeviceFunctionTable.Open(Device,
                                                             DeviceEntry);

                break;

            default:
                Status = STATUS_INVALID_PARAMETER;
                break;
            }

            break;

        case DEVICE_PARTITION:
        case DEVICE_PARTITION_EX:
            Status = PartitionDeviceFunctionTable.Open(Device, DeviceEntry);
            break;

        default:
            Status = STATUS_INVALID_PARAMETER;
            break;
        }
    }

    if (!NT_SUCCESS(Status)) {
        BlockIopFreeAllocations(BlockIoDevice);
    }

    return Status;
}

NTSTATUS
BlockIoClose (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Closes a Block I/O Device.

    Nothing to do for a generic device except free internal variables.  If more
    specific steps need to be taken, this routine should be hooked.

Arguments:

    DeviceEntry - Pointer to the device entry.

Return Value:

    STATUS_SUCCESS when successful.
    An error status is not expected since it implies an error closing an
        existing handle or an error cleaning up internal allocations.

--*/

{

    NTSTATUS Status;

    //
    // If this is the device the last prefetch or partial block read,
    // invalidate the cached buffer.
    //

    if (BlockIopPrefetchDevice == DeviceEntry->DeviceData) {
        BlockIopPrefetchDevice = NULL;
    }

    if (BlockIopPartialBlockDevice == DeviceEntry->DeviceData) {
        BlockIopPartialBlockDevice = NULL;
    }

    //
    // Free any memory allocations made for the device.
    //

    Status = BlockIopFreeAllocations(DeviceEntry->DeviceData);
    return Status;
}

NTSTATUS
BlockIoGetInformation (
    __in PDEVICE_ENTRY DeviceEntry,
    __out PDEVICE_INFORMATION DeviceInformation
    )

/*++

Routine Description:

    Returns the block i/o device information structure to the caller in the
    memory provided by DeviceInformation.  This is quite simple since the
    block i/o device structure is setup in a mannar that isolates all the
    information that pertains to a caller.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

    DeviceInformation - Pointer to buffer to copy device information.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;

    BlockIoDevice = (PBLOCK_IO_DEVICE)DeviceEntry->DeviceData;
    RtlCopyMemory(&DeviceInformation->u.BlockIo,
                  &BlockIoDevice->Information,
                  sizeof(BLOCK_IO_INFORMATION));

    DeviceInformation->DeviceType = DEVICE_BLOCK_IO;
    return STATUS_SUCCESS;
}

NTSTATUS
BlockIoSetInformation (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PDEVICE_INFORMATION DeviceInformation
    )

/*++

Routine Description:

    Uses the settings in DeviceInformation to set certain parameters for the
    device.  Not all the parameters in the block i/o device information will
    be configurable, and those will be ignored.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

    DeviceInformation - Pointer to buffer of block i/o device settings.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if the device offset specified is invalid.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    ULONGLONG CurrentDeviceAddress;
    PBLOCK_IO_INFORMATION CurrentInfo;
    ULONGLONG HighestDeviceAddress;

    BlockIoDevice = (PBLOCK_IO_DEVICE) DeviceEntry->DeviceData;
    CurrentInfo = &BlockIoDevice->Information;

    //
    // The only configurable data is the device offset (LbaOffset,
    // BlockOffset).  Ignore everything else.
    //
    // N.B. When the entire device has been read/written, the current
    //      position remains at the byte immediately following the
    //      highest byte on the device. The highest address here is
    //      the address of the highest byte that can be read or written.
    //      Any value above that is past the end of the device.
    //

    CurrentDeviceAddress = (DeviceInformation->u.BlockIo.LbaOffset *
                            CurrentInfo->BlockSize) +
                            DeviceInformation->u.BlockIo.BlockOffset;

    HighestDeviceAddress = (CurrentInfo->LastBlock *
                            CurrentInfo->BlockSize) +
                            CurrentInfo->BlockSize - 1;

    if (CurrentDeviceAddress > HighestDeviceAddress) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Ensure that the current position is written in the standard format,
    // where the Lba offset is full blocks and block offset is less than the
    // block size. Cannot update the caller's information because it is not
    // expected to be changed here.
    //

    CurrentInfo->LbaOffset = CurrentDeviceAddress / CurrentInfo->BlockSize;
    CurrentInfo->BlockOffset = CurrentDeviceAddress % CurrentInfo->BlockSize;
    CurrentInfo->IoAccessFlags = DeviceInformation->u.BlockIo.IoAccessFlags;
    return STATUS_SUCCESS;
}

NTSTATUS
BlockIoEnumerateDeviceClass (
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
    STATUS_INVALID_PARAMETER if an invalid device class was specified.

--*/

{

    PDEVICE_ENUMERATE_ROUTINE EnumerateRoutine;
    NTSTATUS Status;

    //
    // Call the appropriate enumerate routine.  Note that enumerates are not
    // allowed on the deprecated DEVICE_PARTITION device type.
    //

    switch (DeviceClassIdentifier->DeviceType) {
    case DEVICE_BLOCK_IO:
        switch (DeviceClassIdentifier->u.BlockIo.BlockIoType) {
        case BLOCK_IO_HARD_DISK:
        case BLOCK_IO_REMOVABLE_DISK:
        case BLOCK_IO_CDROM:
            EnumerateRoutine = DiskDeviceFunctionTable.Enumerate;
            break;

        case BLOCK_IO_FILE:
            EnumerateRoutine = FileDeviceFunctionTable.Enumerate;
            break;

        case BLOCK_IO_RAMDISK:
            EnumerateRoutine = RamDiskDeviceFunctionTable.Enumerate;
            break;

        case BLOCK_IO_VIRTUAL_HARD_DISK:
            EnumerateRoutine = VirtualDiskDeviceFunctionTable.Enumerate;
            break;

        default:
            return STATUS_INVALID_PARAMETER;
        }

        break;

    case DEVICE_PARTITION_EX:
        EnumerateRoutine = PartitionDeviceFunctionTable.Enumerate;
        break;

    default:
        EnumerateRoutine = NULL;
        break;
    }

    if (EnumerateRoutine != NULL) {
        Status = EnumerateRoutine(DeviceClassIdentifier,
                                  DeviceIdBuffer,
                                  AllocateNewBuffer);

    } else {
        Status = STATUS_NOT_IMPLEMENTED;
    }

    return Status;
}

NTSTATUS
BlockIoCreate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION CreationOptions,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    )

/*++

Routine Description:

    This routine handles creation of a block I/O device.  It simply calls
    out to the sub-type-specific creation routine.  This is an optional
    routine, so if the sub-type-specific routine does not exist then this
    call just returns.

Arguments:

    Device - Supplies a pointer to a boot environment device describing
        the device type to create.  Only the device and block I/O type
        in this structure are guaranteed to be valid.

    CreationOptions - Supplies a pointer to an options list that contains
        options describing how the device should be created.

    CreatedDevice - Supplies a pointer to a variable that receives a heap
        allocated buffer containing the device identifier of the created
        device.

--*/

{

    PDEVICE_CREATE_ROUTINE CreateRoutine;

    switch (Device->DeviceType) {
    case DEVICE_BLOCK_IO:
        switch (Device->u.BlockIo.BlockIoType) {
        case BLOCK_IO_HARD_DISK:
        case BLOCK_IO_REMOVABLE_DISK:
        case BLOCK_IO_CDROM:
            CreateRoutine = DiskDeviceFunctionTable.Create;
            break;

        case BLOCK_IO_FILE:
            CreateRoutine = FileDeviceFunctionTable.Create;
            break;

        case BLOCK_IO_RAMDISK:
            CreateRoutine = RamDiskDeviceFunctionTable.Create;
            break;

        case BLOCK_IO_VIRTUAL_HARD_DISK:
            CreateRoutine = VirtualDiskDeviceFunctionTable.Create;
            break;

        default:
            CreateRoutine = NULL;
            break;
        }

        break;

    case DEVICE_PARTITION:
    case DEVICE_PARTITION_EX:
        CreateRoutine = PartitionDeviceFunctionTable.Create;
        break;

    default:
        CreateRoutine = NULL;
        break;
    }

    if (CreateRoutine != NULL) {
        return CreateRoutine(Device, CreationOptions, CreatedDevice);

    } else {
        return STATUS_NOT_IMPLEMENTED;
    }
}

NTSTATUS
BlockIopInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the Block I/O Module.  In particular, this routine initializes
    global data structures required by the module, initializes the firmware
    block I/O implementation and initializes the block cache.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.

--*/

{

    NTSTATUS Status;

    //
    // Block I/O devices can be cached in a global block i/o device table
    // by each device's open routine.  The purpose for this is to prevent
    // duplication of work when opening another device of the same type.
    // The firmware enumeration of devices is firmware dependent.  To open
    // a device, one might have to do the work of opening many devices to
    // find the device the caller wished to open.  The Block I/O Device
    // Table will solve this problem.
    //

    Status = BlockIoDeviceTableInitialize();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

#if !defined(NTKERNEL)

    //
    // Initalize the global state for this module.
    //

    BlockIoFirmwareRemovableDiskCount = 0;
    BlockIoFirmwareRawDiskCount = 0;
    BlockIoFirmwareCdromCount = 0;
    BlockIopAlignedBuffer = NULL;
    BlockIopAlignedBufferSize = 0;
    BlockIopPartialBlockBuffer = NULL;
    BlockIopPartialBlockBufferSize = 0;
    BlockIopPrefetchBuffer = NULL;
    BlockIopReadBlockBuffer = NULL;
    BlockIopReadBlockBufferSize = 0;

    //
    // Allocate memory for a device prefetch.  This functionality is only
    // used conditionally at the request of the caller.
    //

    Status = BlMmAllocatePages(&BlockIopPrefetchBuffer,
                               BLOCK_DEVICE_PREFETCH_SIZE >> PAGE_SHIFT,
                               MEMORY_TYPE_DEVICE_DATA,
                               MEMORY_ATTRIBUTE_NONE,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto BlockIopInitializeEnd;
    }

    //
    // Initialize the block cache.  The cache provides API callers the
    // ability to request certain blocks to be cached.
    //

    Status = BcInitialize();
    if (!NT_SUCCESS(Status)) {
        goto BlockIopInitializeEnd;
    }

    //
    // Provide the firmware implementation a chance to initialize and
    // data structures it requires.
    //

    Status = BlockIoFirmwareInitialize();
    if (!NT_SUCCESS(Status)) {
        goto BlockIopInitializeEnd;
    }

    //
    // And mark the module as initialized, registering a routine that
    // will mark it uninitialized if the library is destroyed.
    //

    Status = BlpIoRegisterDestroyRoutine(BlockIopDestroy);
    if (!NT_SUCCESS(Status)) {
        goto BlockIopInitializeEnd;
    }

#endif

    BlockIoInitialized = TRUE;

#if !defined(NTKERNEL)

BlockIopInitializeEnd:

    if (!NT_SUCCESS(Status)) {
        if (BlockIopPrefetchBuffer != NULL) {
            BlMmFreePages(BlockIopPrefetchBuffer);
        }
    }

#endif

    return Status;

}

NTSTATUS
BlockIopFreeAllocations (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    Frees up any memory allocated for a Block I/O Device structure.  This
    includes the Block I/O Device structure itself.

Arguments:

    BlockIoDevice - Pointer to a Block I/O Device Structure.  A Block I/O
        Device structure is allocated by BlockIoOpen.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    if (BlockIoDevice != NULL) {
        BlMmFreeHeap(BlockIoDevice);
    }

    return STATUS_SUCCESS;
}



