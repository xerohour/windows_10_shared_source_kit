/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    device.c

Abstract:

    Implementation of boot library device i/o api.

    When device's are opened, an entry is created in a device table.  After
    this, all device i/o calls are directed based on function pointers set
    during the specific devices open routine.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "device.h"

// -------------------------------------------------------------------- Globals

//
// There is a global pointer to the device table.  The device table is simply
// an array of pointers to device entries.  It is dynamic is size and therefore
// the library does have a limit on number of devices.
//

PDEVICE_ENTRY *DmDeviceTable;
ULONG DmTableEntries;

//
// Global list of registered device classes.  Each registered device class
// has a chance to open a device prior to the Device I/O Manager's attempt to
// call the appropriate device open routine.
//

LIST_ENTRY DmRegisteredDevices;

//
// Global variable for keeping track of the total IO performed on all devices.
//

DEVICE_IO_INFORMATION DmDeviceIoInformation;

// ---------------------------------------------------------------- Definitions

#define INITIAL_DEVICE_ENTRIES (8)

// ----------------------------------------------------------------- Prototypes

NTSTATUS
BlpDeviceSetOverlayInformation (
    _In_ DEVICE_ID DeviceId,
    _In_ DEVICE_ID OverlayedDeviceId,
    _In_ PLARGE_INTEGER DataSourceId
    );

NTSTATUS
BlpValidateOverlayConfig (
    __in_bcount(OverlayConfigSize) POVERLAY_CONFIG OverlayConfig,
    __in ULONG OverlayConfigSize
    );

NTSTATUS
DeviceTableDestroyEntry (
    PDEVICE_ENTRY DeviceEntry,
    ULONG Index
    );

NTSTATUS
DeviceTablePurge (
    __in PDEVICE_ENTRY DeviceEntry
    );

BOOLEAN
DeviceTableCompare (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel,
    __in_opt PVOID Arg4
    );

NTSTATUS
DmOpenRegisteredDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    );

// ------------------------------------------------------------------ Functions

BOOLEAN
BlDeviceIsVirtualPartitionDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out_opt PBOOT_ENVIRONMENT_DEVICE *VhdDevice
    )

/*++

Routine Description:

    This routine checks whether the specified partition is physical or virtual.

Arguments:

    Device - Supplies the partition device to check.

    VhdDevice - Optionally receives underlying VHD device.

Return Value:

    TRUE if the partition is virtual. FALSE otherwise.

--*/

{

    ULONG Offset;
    PBLOCK_IO_IDENTIFIER ParentIdentifier;
    BOOLEAN VirtualPartition;

    //
    // A partition is virtual if it resides on a virtual block IO resides.
    // Obtain the parent identifier for the partition and inspect its type.
    //

    ParentIdentifier = NULL;
    VirtualPartition = FALSE;
    if (Device->DeviceType == DEVICE_PARTITION) {
        ParentIdentifier = &Device->u.Partition.ParentIdentifier;

    } else if (Device->DeviceType == DEVICE_PARTITION_EX) {
        ParentIdentifier = &Device->u.PartitionEx.ParentIdentifier;
    }

    if ((ParentIdentifier != NULL) &&
        (ParentIdentifier->BlockIoType == BLOCK_IO_VIRTUAL_HARD_DISK)) {

        VirtualPartition = TRUE;

        //
        // Return a pointer to the underlying VHD device, if requested.
        //

        if (ARGUMENT_PRESENT(VhdDevice)) {
            Offset = RTL_SIZEOF_THROUGH_FIELD(BLOCK_IO_IDENTIFIER,
                                              u.VirtualHardDisk);

            *VhdDevice = (PBOOT_ENVIRONMENT_DEVICE)Add2Ptr(ParentIdentifier, Offset);
        }
    }

    return VirtualPartition;
}

NTSTATUS
BlDeviceGetVhdFileDevice (
    __in PBOOT_ENVIRONMENT_DEVICE VhdDevice,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *VhdFileDevice
    )

/*++

Routine Description:

    This routine will retrieve device identifier for the device hosting the VHD
    file.

Arguments:

    VhdDevice - Supplies VHD device identifier.

    VhdFileDevice - Receives device identifier for the device hosting the VHD
        file.

Return Value:

    NTSTATUS.

--*/

{

    PUNALIGNED_FILE_PATH FilePath;

    if ((VhdDevice->DeviceType != DEVICE_BLOCK_IO) ||
        (VhdDevice->u.BlockIo.BlockIoType != BLOCK_IO_FILE)) {

        return STATUS_BAD_DEVICE_TYPE;
    }

    FilePath = &VhdDevice->u.BlockIo.u.File.SourcePath;

    if (FilePath->Type != FILE_PATH_TYPE_BOOT_ENV) {
        return STATUS_BAD_DEVICE_TYPE;
    }

    if (FilePath->Length <
        (FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath) +
         FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, Pad))) {

        return STATUS_BAD_DEVICE_TYPE;
    }

    *VhdFileDevice = (PBOOT_ENVIRONMENT_DEVICE)FilePath->FilePath;
    return STATUS_SUCCESS;
}

NTSTATUS
BlDeviceOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in OPEN_FLAGS OpenFlags,
    __out PDEVICE_ID DeviceId
    )

/*++

Routine Description:

    This function opens a device for use.  It will call the appropriate
    device routine call (based on Device->DeviceType) to collect all the
    required information on the device.  This will be then added to the
    device table.

    If the device is already open and the OpenFlags flags do not conflict,
    the same device id may be returned and the reference count to the device
    increased.

Arguments:

    Device - Contains the device type and any required information to
        correctly identify the device.

    OpenFlags - The flags to open the device with.

    DeviceId - Pointer to store the index to the device's entry in the device
        table.

Return Value:

    STATUS_SUCCESS when successful.  DeviceId will also contain the device's
        device id.

    STATUS_INVALID_PARAMETER if Device or DeviceId is NULL if Device is
        invalid or if OpenFlags is zero.

    STATUS_NO_MEMORY if allocation for internal data structure fails.

    Other failing error status codes are possible based on the requested
        device's implementation.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);
    Status = BlpDeviceOpen(Device, OpenFlags, 0, DeviceId);
    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

VOID
BlpDeviceCloseOverlay (
    __inout PDEVICE_ENTRY Device
    )
{

    POVERLAY_ENTRY Entry;
    ULONG Index;
    PDEVICE_OVERLAY Overlay;

    Overlay = Device->Overlay;
    for (Index = 0; Index < Overlay->OverlayCount; Index += 1) {
        Entry = &Overlay->Entry[Index];
        if (Entry->State == OverlayOpened) {
            Entry->State = OverlayClosed;
            BlDeviceClose(Entry->DeviceId);
            Entry->DeviceId = INVALID_DEVICE_ID;
        }
    }

    return;
}

VOID
BlpDeviceDestoryOverlay (
    __inout PDEVICE_ENTRY Device
    )
{

    BlMmFreeHeap(Device->Overlay->OverlayConfig);
    BlMmFreeHeap(Device->Overlay);
    Device->Overlay = NULL;
    return;
}

VOID
BlpDeviceInitializeOverlay (
    __inout PDEVICE_ENTRY Device
    )
{

    ULONG BytesRead;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    PDEVICE_OVERLAY Overlay;
    ULONG OverlayCount;
    POVERLAY_CONFIG OverlayConfig;
    POVERLAY_INFO OverlayInfo;
    NTSTATUS Status;
    ULONG Size;
    ULONG Index;

    BlpInterfaceEnter(InterfaceDevice);

    Overlay = NULL;
    FileId = INVALID_FILE_ID;
    OverlayConfig = NULL;

    NT_ASSERT(Device->Overlay == NULL);

    //
    // Open overlay configuration file for the volume.
    //
    // N.B. Configuration file being a reparse poing is not supported, as it can
    //      result in infinite recursion.
    //

    Status = BlFileOpen(Device->DeviceId,
                        OVERLAY_CONFIG_FILE,
                        OPEN_READ | OPEN_NO_REPARSE,
                        &FileId);

    if (!NT_SUCCESS(Status)) {
        Status = BlFileOpen(Device->DeviceId,
                            NEW_OVERLAY_CONFIG_FILE,
                            OPEN_READ | OPEN_NO_REPARSE,
                            &FileId);

        if (!NT_SUCCESS(Status)) {
            goto DeviceInitializeOverlayEnd;
        }
    }


    //
    // Query file size.
    //

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto DeviceInitializeOverlayEnd;
    }

    if ((FileInfo.FileSize < FIELD_OFFSET(OVERLAY_CONFIG, OverlayInfo)) ||
        (FileInfo.FileSize > MAX_OVERLAY_CONFIG_SIZE)) {

        goto DeviceInitializeOverlayEnd;
    }

    Size = (ULONG)FileInfo.FileSize;
    OverlayConfig = BlMmAllocateHeap(Size);
    if (OverlayConfig == NULL) {
        goto DeviceInitializeOverlayEnd;
    }

    //
    // Read overlay configuration into memory.
    //

    Status = BlFileReadEx(FileId,
                          OverlayConfig,
                          Size,
                          &BytesRead,
                          FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto DeviceInitializeOverlayEnd;
    }

    if (BytesRead != Size) {
        goto DeviceInitializeOverlayEnd;
    }

    //
    // Validate overlay configuration.
    //

    Status = BlpValidateOverlayConfig(OverlayConfig, Size);
    if (!NT_SUCCESS(Status)) {
        goto DeviceInitializeOverlayEnd;
    }

    //
    // Associate overlay configuration with the device.
    //

    OverlayCount = OverlayConfig->OverlayCount;
    Overlay = BlMmAllocateHeap(FIELD_OFFSET(DEVICE_OVERLAY, Entry) +
                                    sizeof(OVERLAY_ENTRY) * OverlayCount);

    if (Overlay == NULL) {
        goto DeviceInitializeOverlayEnd;
    }

    Device->Overlay = Overlay;
    Overlay->OverlayConfig = OverlayConfig;
    Overlay->OverlayCount = OverlayCount;
    OverlayInfo = &OverlayConfig->OverlayInfo[0];
    for (Index = 0; Index < OverlayCount; Index += 1) {
        DeviceIdentifier = (PBOOT_ENVIRONMENT_DEVICE)
            Add2Ptr(OverlayConfig, OverlayInfo->DeviceOffset);
        Overlay->Entry[Index].DataSourceId = OverlayInfo->DataSource;
        Overlay->Entry[Index].State = OverlayClosed;
        Overlay->Entry[Index].DeviceId = INVALID_DEVICE_ID;
        Overlay->Entry[Index].DeviceIdentifier = DeviceIdentifier;
        OverlayInfo = Add2Ptr(OverlayInfo, OverlayConfig->OverlayInfoSize);
    }

    Overlay = NULL;
    OverlayConfig = NULL;

DeviceInitializeOverlayEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (OverlayConfig != NULL) {
        BlMmFreeHeap(OverlayConfig);
    }

    if (Overlay != NULL) {
        BlMmFreeHeap(Overlay);
    }

    BlpInterfaceExit(InterfaceDevice);
    return;
}

DEVICE_ID
BlDeviceGetOverlay (
    __in DEVICE_ID DeviceId,
    __in LARGE_INTEGER DataSource
    )
{

    PDEVICE_ENTRY Device;
    POVERLAY_ENTRY Entry;
    DEVICE_ID OverlayDeviceId;
    NTSTATUS Status;
    ULONG Index;

    OverlayDeviceId = INVALID_DEVICE_ID;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Make sure this is a valid device id.
    //

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
         (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        goto DeviceGetOverlayEnd;
    }

    //
    // Initialize overlay configuration.
    //

    Device = DmDeviceTable[DeviceId];
    if (Device->Overlay == NULL) {
        BlpDeviceInitializeOverlay(Device);
        if (Device->Overlay == NULL) {
            goto DeviceGetOverlayEnd;
        }
    }

    //
    // Try to find a matching overlay device, and open it if required.
    //

    for (Index = 0; Index < Device->Overlay->OverlayCount; Index += 1) {
        Entry = &Device->Overlay->Entry[Index];
        if (Entry->DataSourceId.QuadPart == DataSource.QuadPart) {
            if (Entry->State == OverlayClosed) {
                Status = BlDeviceOpen(Entry->DeviceIdentifier,
                                      OPEN_READ | OPEN_BY_HASH,
                                      &Entry->DeviceId);

                if (NT_SUCCESS(Status)) {
                    Entry->State = OverlayOpened;
                    BlpDeviceSetOverlayInformation(Entry->DeviceId,
                                                   DeviceId,
                                                   &DataSource);

                } else {
                    Entry->State = OverlayError;
                    goto DeviceGetOverlayEnd;
                }
            }

            OverlayDeviceId = Entry->DeviceId;
            break;
        }
    }

DeviceGetOverlayEnd:
    BlpInterfaceExit(InterfaceDevice);
    return OverlayDeviceId;
}

BOOLEAN
BlDeviceIsOpenByHash (
    __in DEVICE_ID DeviceId
    )
{

    PDEVICE_ENTRY Device;

    Device = BlpDeviceGetEntry(DeviceId);

    NT_ASSERT(Device != NULL);

    return IS_DEVICE_OPEN_BY_HASH_SET(Device->Attributes);
}

NTSTATUS
BlDeviceClose (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    Decrements the reference count to the device.  If zero, the device's is
    deemed 'closed'.

    Do a lazy removal of device entries from the device table.  The
    initialization code of a device may prove costly.  With the lazy removal
    of the device entries, we would save time when a device is opened again
    after being closed.

Arguments:

    DeviceId - The id for the device to be closed.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or if the device is already
        closed.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Make sure this is a valid device id.
    //

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
         (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Close an instance of the device.
        //

        DmDeviceTable[DeviceId]->ReferenceCount -= 1;

        //
        // If there are no more instances of the device open, close the device.
        //

        if (DmDeviceTable[DeviceId]->ReferenceCount == 0) {

            //
            // Close any overlays associated with the device.
            //

            if (DmDeviceTable[DeviceId]->Overlay != NULL) {
                BlpDeviceCloseOverlay(DmDeviceTable[DeviceId]);
            }

            CLEAR_DEVICE_OPEN(DmDeviceTable[DeviceId]->Attributes);

            if (DmDeviceTable[DeviceId]->DeviceIdentifier->DeviceType == DEVICE_VMBUS) {

                //
                // When closing a vmbus channel, we must call the Device's close
                // routine.  This is because the Vmbus protocol first performs a
                // protocol negotiation when a new channel is opened.
                //

                DeviceTableDestroyEntry(DmDeviceTable[DeviceId], DeviceId);
            }
        }

        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDevicePurgeAllCaches (
    VOID
    )

/*++

Routine Description:

    This routine purges all device caches, freeing their associated
    resources.

Arguments:

    None.

Return Value:

    Always returns STATUS_SUCCESS.

--*/

{

    BlpInterfaceEnter(InterfaceDevice);
    BlockIoPurgeCache();
    BlpInterfaceExit(InterfaceDevice);
    return STATUS_SUCCESS;
}

NTSTATUS
BlDeviceRead (
    __in DEVICE_ID DeviceId,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    )

/*++

Routine Description:

    Reads up to Size bytes from the Device specified.  The contents of the read
    are copied to Buffer.

Arguments:

    DeviceId - The identifier for the device to be read.

    Buffer - Buffer to copy read contents.

    Size - The number of bytes to be read.

    Count - If present, the number of bytes transferred to Buffer will be
        written to the address BytesRead points to.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the device ID is invalid.

    STATUS_INVALID_PARAMETER if the device is not opened with read access.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    ULONG DeviceBytesRead;
    PDEVICE_READ_ROUTINE ReadRoutine;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((Buffer == NULL) ||
        (DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED_FOR_READ(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Call the device class specific READ routine.
        //

        ReadRoutine = DmDeviceTable[DeviceId]->FunctionTable.Read;
        Status = ReadRoutine(DmDeviceTable[DeviceId],
                             Buffer,
                             Size,
                             &DeviceBytesRead);

        if (DmDeviceTable[DeviceId]->FilterLevel == 0) {
            DmDeviceIoInformation.TotalBytesRead += DeviceBytesRead;
        }

        if (ARGUMENT_PRESENT(BytesRead) != FALSE) {
            *BytesRead = DeviceBytesRead;
        }
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceWrite (
    __in DEVICE_ID DeviceId,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Attempts to writes Size bytes from Buffer to the Device specified.

Arguments:

    DeviceId - The identifier for the device to write to.

    Buffer - Pointer to buffer to write to device.

    Size - The number of bytes to write.

    Count - If present, the number of bytes transferred from Buffer to the
        device will be written to the address Count points to.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the device ID is invalid.

    STATUS_INVALID_PARAMETER if the device is not opened with write access.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    ULONG DeviceBytesWritten;
    NTSTATUS Status;
    PDEVICE_WRITE_ROUTINE WriteRoutine;

    BlpInterfaceEnter(InterfaceDevice);

    if ((Buffer == NULL) ||
        (DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED_FOR_WRITE(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        WriteRoutine = DmDeviceTable[DeviceId]->FunctionTable.Write;
        Status = WriteRoutine(DmDeviceTable[DeviceId],
                              Buffer,
                              Size,
                              &DeviceBytesWritten);

        if (DmDeviceTable[DeviceId]->FilterLevel == 0) {
            DmDeviceIoInformation.TotalBytesWritten += DeviceBytesWritten;
        }

        if (ARGUMENT_PRESENT(BytesWritten) != FALSE) {
            *BytesWritten = DeviceBytesWritten;
        }
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceGetInformation (
    __in DEVICE_ID DeviceId,
    __out PDEVICE_INFORMATION DeviceInformation
    )

/*++

Routine Description:

    Attempts to get device information for the device specified by DeviceId.

Arguments:

    DeviceId - The identifier for the device to get information for.

    DeviceInformation - Pointer to buffer to copy device information.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the DeviceInformation is NULL.

    STATUS_INVALID_PARAMETER if the device ID is invalid.

    STATUS_INVALID_PARAMETER if the device is not currently opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    DEVICE_TYPE DeviceType;
    PDEVICE_GET_INFORMATION_ROUTINE GetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DeviceInformation == NULL) ||
        (DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        DeviceType = DmDeviceTable[DeviceId]->DeviceIdentifier->DeviceType;
        DeviceInformation->DeviceType = DeviceType;
        GetInformation = DmDeviceTable[DeviceId]->FunctionTable.GetInformation;
        Status = GetInformation(DmDeviceTable[DeviceId], DeviceInformation);
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceSetInformation (
    __in DEVICE_ID DeviceId,
    __in PDEVICE_INFORMATION DeviceInformation
    )

/*++

Routine Description:

    Attempts to set device information for the device specified by DeviceId.
    Not all information is configurable (some is read only), and the way a
    device responds to non-configurable in DeviceInformation changing is
    left up to the implementation.

    It is recommended to populate a DEVICE_INFORMATION structure with non-
    configurable data by calling BlDeviceGetInformation.

Arguments:

    DeviceId - The identifier for the device to get information for.

    DeviceInformation - Pointer to buffer of device settings.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the DeviceInformation is NULL.

    STATUS_INVALID_PARAMETER if the device ID is invalid.

    STATUS_INVALID_PARAMETER if the device is not currently opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    PDEVICE_SET_INFORMATION_ROUTINE SetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DeviceInformation == NULL) ||
        (DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        SetInformation = DmDeviceTable[DeviceId]->FunctionTable.SetInformation;
        Status = SetInformation(DmDeviceTable[DeviceId], DeviceInformation);
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceGetIoInformation (
    __out PDEVICE_IO_INFORMATION DeviceIoInformation
    )

/*++

Routine Description:

    This routine retrieves information about device IO.

Arguments:

    DeviceIoInformation - Supplies the pointer to the Device IO Information
        structure to be filled out.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the DeviceIoInformation is NULL.

--*/

{

    if (DeviceIoInformation == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    memcpy(DeviceIoInformation,
           &DmDeviceIoInformation,
           sizeof(DEVICE_IO_INFORMATION));

    return STATUS_SUCCESS;
}


NTSTATUS
BlDeviceReset (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    Resets a device.  The meaning of a device reset will vary from device to
    device.  The idea stems from the idea that initialization is the collection
    of device information and allocation of device data structures.  The device
    can be reset a number of times without the need to reopen or reinitialize
    the device.

Arguments:

    DeviceId - The identifier for the device to get information for.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or if the device is not
        opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {
        Status = BlpDeviceReset(DmDeviceTable[DeviceId]);
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceFlush (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    Flushes a device.

Arguments:

    DeviceId - The identifier for the device.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or if the device is not
        opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {
        Status = BlpDeviceFlush(DmDeviceTable[DeviceId]);
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceEnumerateDeviceClass (
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

    N.B. Handles returned for a device are the result of opening the device
         with read-only access rights.  Opening a device read-write would
         cause a new handle to be create with each open.  This would be
         costly if the devices were enumerated multiple times and the original
         handles were never closed.

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

    STATUS_INVALID_PARAMETER if the device class identifier is NULL.

    STATUS_INVALID_PARAMETER if the device ID buffer is NULL.

    STATUS_NO_MEMORY if any allocation fails.

    STATUS_BUFFER_TOO_SMALL if DeviceIdBuffer is too small and
        AllocateNewBuffer is FALSE.

--*/

{

    PDEVICE_ENUMERATE_ROUTINE EnumerateRoutine;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DeviceClassIdentifier == NULL) || (DeviceIdBuffer == NULL)) {
        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Call the device type's enumerate routine.  Note that enumerates are
        // not allowed on the deprecated DEVICE_PARTITION device type.
        //

        switch (DeviceClassIdentifier->DeviceType) {
        case DEVICE_BLOCK_IO:
        case DEVICE_PARTITION_EX:
            EnumerateRoutine = BlockIoDeviceFunctionTable.Enumerate;
            break;

        case DEVICE_SERIAL_PORT:
            EnumerateRoutine = SerialPortFunctionTable.Enumerate;
            break;

        case DEVICE_UDP:
            EnumerateRoutine = UdpFunctionTable.Enumerate;
            break;

#if !defined(DEVICE_MIN) && !defined(ARM) && !defined(ARM64)
        case DEVICE_VMBUS:
            EnumerateRoutine = VmbusDeviceFunctionTable.Enumerate;
            break;
#endif

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
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlGetDeviceIdentifier (
    __in DEVICE_ID DeviceId,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *DeviceIdentifier
    )

/*++

Routine Description:

    Returns the device identifier stored in the device entry referenced
    by DeviceId.

    TODO:  Get rid of this function.  It only exists for AhGetArcDevice
           so a parition get get a device id for it's parent device.

Arguments:

    DeviceId - Handle for an opened device (obtained from BlDeviceOpen).

    DeviceIdentifier - On output, contains a pointer to the device identifier
        for a particular device.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or DeviceIdentifier is
        NULL.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
        (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes)) ||
        (DeviceIdentifier == NULL)) {

        Status = STATUS_INVALID_PARAMETER;

    } else {
        *DeviceIdentifier = DmDeviceTable[DeviceId]->DeviceIdentifier;
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceCreate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION CreationOptions,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    )

/*++

Routine Description:

    This routine handles creation of a device.  It simply calls
    out to the type-specific creation routine.  This is an optional
    routine, so if the type-specific routine does not exist then this
    call just returns.

Arguments:

    Device - Supplies a pointer to a boot environment device describing
        the device type to create.  Only the device type in this structure
        is guaranteed to be valid.

    CreationOptions - Supplies a pointer to an options list that contains
        options describing how the device should be created.

    CreatedDevice - Supplies a pointer to a variable that receives a heap
        allocated buffer containing the device identifier of the created
        device.

--*/

{

    PDEVICE_CREATE_ROUTINE CreateRoutine;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if (Device == NULL) {
        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Call the device type's creation routine.
        //

        switch (Device->DeviceType) {
        case DEVICE_BLOCK_IO:
        case DEVICE_PARTITION:
        case DEVICE_PARTITION_EX:
            CreateRoutine = BlockIoDeviceFunctionTable.Create;
            break;

        case DEVICE_SERIAL_PORT:
            CreateRoutine = SerialPortFunctionTable.Create;
            break;

        default:
            CreateRoutine = NULL;
            break;
        }

        if (CreateRoutine == NULL) {
            Status = STATUS_NOT_SUPPORTED;

        } else {
            Status = CreateRoutine(Device, CreationOptions, CreatedDevice);
        }
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

// -------------------------------------------------- Block I/O Device Wrappers

NTSTATUS
BlDeviceReadAtOffsetEx (
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead,
    __in ULONG Flags
    )

/*++

Routine Description:

    Wrapper for device reads.  Provided a ByteOffset, this routine will set the
    LBA + Block Offset for the read based on the device's block size.
    This routine simplies the device reads required by each file system.

Arguments:

    DeviceId - Device id for the Block I/O Device the request is for.

    ByteOffset - The offset on the device for the read operation to start.

    Size - The number of bytes to read.

    Buffer - Buffer to store contents of the read operation.

    BytesRead - Pointer to address to store the number of bytes actually read.

    Flags - Supplies the flags controlling behavior of the device read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if a requested block is invalid.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    DEVICE_INFORMATION DeviceInformation;
    PBLOCK_IO_INFORMATION Information;
    ULONG SavedBlockOffset;
    ULONGLONG SavedLbaOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Get information prior to setting device offset.  This operation serves
    // two purposes, it provides the Block Size of the device and it
    // sets all the non-configurable information fields to their actual value.
    //

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceReadAtOffsetEnd;
    }

    //
    // Update the device pointer.
    //

    Information = &DeviceInformation.u.BlockIo;
    SavedLbaOffset = Information->LbaOffset;
    SavedBlockOffset = Information->BlockOffset;
    Information->LbaOffset = ByteOffset / Information->BlockSize;
    Information->BlockOffset = (ULONG)(ByteOffset % Information->BlockSize);
    Status = BlDeviceSetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceReadAtOffsetEnd;
    }

    //
    // Read Size bytes from the Block I/O Device.
    //

    Status = BlDeviceRead(DeviceId, Buffer, Size, BytesRead);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceReadAtOffsetEnd;
    }

    //
    // Restore original offset values if requested.
    //

    if (CHECK_FLAG(Flags, DEVICE_FLAGS_NO_OFFSET_UPDATE)) {
        Information->LbaOffset = SavedLbaOffset;
        Information->BlockOffset = SavedBlockOffset;
        BlDeviceSetInformation(DeviceId, &DeviceInformation);
    }

BlDeviceReadAtOffsetEnd:
    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlDeviceWriteAtOffset (
    __in DEVICE_ID DeviceId,
    __in ULONGLONG ByteOffset,
    __in ULONG Size,
    __in_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Wrapper for device writes.  Provided a ByteOffset, this routine will set
    the LBA + Block Offset for the write based on the device's block size.
    This routine simplies the device writes required by each file system.

Arguments:

    DeviceId - Device id for the Block I/O Device the request is for.

    ByteOffset - The offset on the device for the write operation to start.

    Size - The number of bytes to write.

    Buffer - Buffer with contents to write to the block i/o device.

    BytesWritten - Pointer to address to store the number of bytes actually
        written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if a requested block is invalid.

    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.

    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    DEVICE_INFORMATION DeviceInformation;
    PBLOCK_IO_INFORMATION Information;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Get information prior to setting device offset.  This operation serves
    // two purposes, it provides the Block Size of the device and it
    // sets all the non-configurable information fields to their actual value.
    //

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceWriteAtOffsetEnd;
    }

    //
    // Update the device pointer.
    //

    Information = &DeviceInformation.u.BlockIo;
    Information->LbaOffset = ByteOffset / Information->BlockSize;
    Information->BlockOffset = (ULONG)(ByteOffset % Information->BlockSize);
    Status = BlDeviceSetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceWriteAtOffsetEnd;
    }

    //
    // Write Size bytes to the Block I/O Device.
    //

    Status = BlDeviceWrite(DeviceId, Buffer, Size, BytesWritten);
    if (!NT_SUCCESS(Status)) {
        goto BlDeviceWriteAtOffsetEnd;
    }

BlDeviceWriteAtOffsetEnd:

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

// ---------------------------------------------------------------- Private API

NTSTATUS
BlpDeviceFlush (
    __in PDEVICE_ENTRY Device
    )

/*++

Routine Description:

    Flushes a device.

Arguments:

    Device - Supplies a device entry for the device to flush.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or if the device is not
        opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    PDEVICE_FLUSH_ROUTINE FlushRoutine;
    NTSTATUS Status;

    FlushRoutine = Device->FunctionTable.Flush;
    if (FlushRoutine == NULL) {
        Status = STATUS_SUCCESS;

    } else {
        Status = FlushRoutine(Device);
    }

    return Status;
}

NTSTATUS
BlpDeviceReset (
    __in PDEVICE_ENTRY Device
    )

/*++

Routine Description:

    Resets a device.  The meaning of a device reset will vary from device to
    device.  The idea stems from the idea that initialization is the collection
    of device information and allocation of device data structures.  The device
    can be reset a number of times without the need to reopen or reinitialize
    the device.

Arguments:

    Device - Supplies a device entry for the device to reset.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid or if the device is not
        opened.

    Other error status codes are possible depending on the device
        implementation.

--*/

{

    PDEVICE_RESET_ROUTINE ResetRoutine;
    NTSTATUS Status;

    ResetRoutine = Device->FunctionTable.Reset;
    if (ResetRoutine == NULL) {
        Status = STATUS_SUCCESS;

    } else {
        Status = ResetRoutine(Device);
    }

    return Status;
}

BOOLEAN
BlpDeviceCompare (
    __in PBOOT_ENVIRONMENT_DEVICE Device1,
    __in PBOOT_ENVIRONMENT_DEVICE Device2
    )

/*++

Routine Description:

    This routine determines if two device identifiers describe the same device.

Arguments:

    Device1 - Supplies a device identifier for comparison.

    Device2 - Supplies a device identifier for comparison.

Return Value:

    TRUE if the two device identifiers describe the same device.

    FALSE otherwise.

--*/

{

    UINTN CompareSize;
    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceDevice);
    ReturnValue = FALSE;
    if ((Device1 == NULL) || (Device2 == NULL)) {
        goto DeviceCompareEnd;
    }

    if (Device1->DeviceType != Device2->DeviceType) {
        goto DeviceCompareEnd;
    }

    //
    // Perform a byte comparison of the two device identifiers.  Since either
    // input identifier can differ in size from the library's definition,
    // the compare must be done on the smaller of the two.
    //

    CompareSize = min(Device1->Size, Device2->Size);
    if (CompareSize < FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u)) {
        goto DeviceCompareEnd;
    }

    CompareSize -= FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u);
    if (RtlEqualMemory(&Device1->u, &Device2->u, CompareSize) == FALSE) {
        goto DeviceCompareEnd;
    }

    ReturnValue = TRUE;

DeviceCompareEnd:
    BlpInterfaceExit(InterfaceDevice);
    return ReturnValue;
}

NTSTATUS
BlpDeviceDestroy (
    VOID
    )

/*++

Routine Description:

    Purges the device entries in the device table.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    Other error status codes are possible depending on device implementation
        of Close.

--*/

{

    PREGISTERED_DEVICE_CLASS DeviceClass;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Free all device caches.
    //

    BlDevicePurgeAllCaches();

    //
    // Destroy the device table and all entries in it.
    //

    Status = BlTblMap(DmDeviceTable,
                      DmTableEntries,
                      DeviceTableDestroyEntry,
                      FALSE);

    BlMmFreeHeap(DmDeviceTable);

    //
    // Destroy the list of registered device classes.
    //

    NextEntry = DmRegisteredDevices.Flink;
    while (NextEntry != &DmRegisteredDevices) {
        DeviceClass = CONTAINING_RECORD(NextEntry,
                                        REGISTERED_DEVICE_CLASS,
                                        ListEntry);

        NextEntry = NextEntry->Flink;
        BlMmFreeHeap(DeviceClass);
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlpDeviceInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the data structures used for device i/o.

Arguments:

    none

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for DmDeviceTable.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Initialize list of registered device classes.
    //

    InitializeListHead(&DmRegisteredDevices);

    //
    // Initialize globals
    //

    DmDeviceIoInformation.TotalBytesRead = 0;
    DmDeviceIoInformation.TotalBytesWritten = 0;

    //
    // Initial Device Table.
    //

    DmTableEntries = INITIAL_DEVICE_ENTRIES;
    DmDeviceTable = BlMmAllocateHeap(DmTableEntries * sizeof(PDEVICE_ENTRY));
    if (DmDeviceTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto DeviceInitializeEnd;
    }

    RtlZeroMemory(DmDeviceTable, DmTableEntries * sizeof(PDEVICE_ENTRY));

    //
    // Initialize the FVE module.  Encrypted volume support is implemented
    // as a filter on Block I/O Partitions.
    //

    Status = FvebInitialize();
    if (!NT_SUCCESS(Status)) {
        goto DeviceInitializeEnd;
    }

DeviceInitializeEnd:
    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlpDeviceRegisterDeviceClass (
    __in const DEVICE_FUNCTION_TABLE* FunctionTable,
    __in ULONG Flags
    )

/*++

Routine Description:

    Registers a device class with the Device I/O Manager.  The Device I/O
    Manager maintains a list of registered device classes.  During a device
    open request, registered device classes will be given an opportunity to
    open a specified device prior to the Device I/O Manager attempting to
    open the device using well known device classes.

Arguments:

    FunctionTable - The function table for the specified device class.

    Flags - Registration flags (DEVCLASS_FLAGS_*).  Valid flags include,

        DEVCLASS_FLAGS_INSERT_AT_HEAD : Indicates that a device class's open
            routine should be called before all others during device open.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for internal data structure.

--*/

{

    PREGISTERED_DEVICE_CLASS DeviceClass;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    DeviceClass = BlMmAllocateHeap(sizeof(REGISTERED_DEVICE_CLASS));
    if (DeviceClass == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpDeviceRegisterDeviceClassEnd;
    }

    RtlCopyMemory(&DeviceClass->FunctionTable,
                  FunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    if (CHECK_FLAG(Flags, DEVCLASS_FLAGS_INSERT_AT_HEAD)) {
        InsertHeadList(&DmRegisteredDevices, &DeviceClass->ListEntry);

    } else {
        InsertTailList(&DmRegisteredDevices, &DeviceClass->ListEntry);
    }

    Status = STATUS_SUCCESS;

BlpDeviceRegisterDeviceClassEnd:

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlpDeviceOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __out PDEVICE_ID DeviceId
    )

/*++

Routine Description:

    This function will open the specified device.  This routine will attempt
    to use an existing internel device entry from the global device table.
    If one an adequate device entry is not already present, this function
    will attempt to open the device by first calling all registered device
    classes, followed by a call to the appropriate device routine open
    (based on Device->DeviceType).  The open process will generate and
    initialize an internal device entry for the specified device.  The
    entry will be added to the global device table.

    If the device is already open and the OpenFlags flags do not conflict,
    the same device id may be returned and the reference count to the device
    increased.

Arguments:

    Device - Contains the device type and any required information to
        correctly identify the device.

    OpenFlags - The flags to open the device with.

    FilterLevel - Specifies the location of the device class to open on the
        stack.

    DeviceId - Pointer to store the index to the device's entry in the device
        table.

Return Value:

    STATUS_SUCCESS when successful.  DeviceId will also contain the device's
        device id.

    STATUS_INVALID_PARAMETER if Device or DeviceId is NULL if Device is
        invalid or if OpenFlags is zero.

    STATUS_NO_MEMORY if allocation for internal data structure fails.

    Other failing error status codes are possible based on the requested
        device's implementation.

--*/

{

    PDEVICE_ENTRY DeviceEntry;
    PBOOT_ENVIRONMENT_DEVICE LocateDevice = NULL;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);
    DeviceEntry = NULL;

    //
    // Basic parameter checks.
    //
    // N.B. DeviceType is not checked since DeviceIoRegisterDeviceClass allows
    //      for the registration of a potentially undefined device type.
    //

    if ((Device == NULL) || (DeviceId == NULL) || (Device->Size == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlpDeviceOpenEnd;
    }

    if (!IS_OPEN_READ_SET(OpenFlags) && !IS_OPEN_WRITE_SET(OpenFlags)) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlpDeviceOpenEnd;
    }

    //
    // The boot device type is a distinguished value that indicates that the
    // boot device is to be opened, regardless of the specific boot device in
    // use on this particular boot.  Convert this sentinel identifier to a
    // real identifier.
    //

    if (Device->DeviceType == DEVICE_BOOT) {
        Device = BlGetBootDevice();
    }

    //
    // For a locate device, resolve it to its target device. If successful,
    // reset the input device identifier to be the resolved device. This will
    // cause the target device to be opened.
    //

    if (Device->DeviceType == DEVICE_LOCATE) {
        Status = BlpDeviceResolveLocate(Device, &LocateDevice);
        if (!NT_SUCCESS(Status)) {
            goto BlpDeviceOpenEnd;
        }

        Device = LocateDevice;
    }

    //
    // If the device identifier isn't fully populated, it doesn't provide
    // enough information to be opened.  This could occur if the device
    // needs to be created through BlDeviceCreate first.  Return a special
    // status code so that this error can be differentiated from other
    // errors if the caller wishes.
    //

    if (CHECK_FLAG(Device->Flags, DEVICE_FLAG_IDENTIFIER_UNSPECIFIED)) {
        Status = STATUS_DEVICE_NOT_READY;
        goto BlpDeviceOpenEnd;
    }

    //
    // Check to see if the device has already been opened with similar
    // attributes.
    //

    DeviceEntry = BlTblFindEntry(DmDeviceTable,
                                 DmTableEntries,
                                 DeviceId,
                                 DeviceTableCompare,
                                 Device,
                                 &OpenFlags,
                                 &FilterLevel,
                                 NULL);

    if (DeviceEntry == NULL) {

        //
        // Allocate memory for a new device entry.
        //

        DeviceEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
        if (DeviceEntry == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlpDeviceOpenEnd;
        }

        //
        // Initialize the device entry with device independent data.
        //

        RtlZeroMemory(DeviceEntry, sizeof(DEVICE_ENTRY));
        DeviceEntry->ReferenceCount = 1;
        DeviceEntry->FilterLevel = FilterLevel;
        DeviceEntry->OverlayInfo.DeviceId = INVALID_DEVICE_ID;

        //
        // Open every device R+W as a performance optimization to cut
        // down on mount IO.
        //
        // N.B. This is safe to do in the single-threaded boot environmet
        //      where all applications are under MS control.
        //

        SET_DEVICE_OPEN(DeviceEntry->Attributes);
        SET_DEVICE_READ(DeviceEntry->Attributes);
        SET_DEVICE_WRITE(DeviceEntry->Attributes);
        if (IS_OPEN_NO_FVE_SET(OpenFlags)) {
            SET_DEVICE_NO_FVE(DeviceEntry->Attributes);
        }

        if (IS_OPEN_BY_HASH_SET(OpenFlags)) {
            SET_DEVICE_OPEN_BY_HASH(DeviceEntry->Attributes);
        }

        //
        // Save a copy of the device identifier in the device entry.
        //
        // N.B. The device identifier buffer may get updated or even
        //      reallocated in cases where the device identifier does not
        //      have accurate information (e.g. LOCATE case). Hence the
        //      identifier buffer address should not be cached within this
        //      routine.
        //

        DeviceEntry->DeviceIdentifier = BlMmAllocateHeap(Device->Size);
        if (DeviceEntry->DeviceIdentifier == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlpDeviceOpenEnd;
        }

        RtlCopyMemory(DeviceEntry->DeviceIdentifier, Device, Device->Size);

        //
        // The Device I/O Library has knowledge of most device types.  For
        // extensibility, the library provides the ability to register
        // device classes.  This could be for new device types or device
        // filters.  Walk the list of registered devices in attempt to open
        // a device using an alternate open routine.  Only when this operation
        // is unsuccessful, attempt to open the device using the well-known
        // open routine for the specified device type.
        //

        Status = DmOpenRegisteredDevice(Device, DeviceEntry);
        if (!NT_SUCCESS(Status)) {
            switch (Device->DeviceType) {
            case DEVICE_BLOCK_IO:
            case DEVICE_PARTITION:
            case DEVICE_PARTITION_EX:
                Status = BlockIoDeviceFunctionTable.Open(Device, DeviceEntry);
                break;

            case DEVICE_SERIAL_PORT:
                Status = SerialPortFunctionTable.Open(Device, DeviceEntry);
                break;

            case DEVICE_UDP:
                Status = UdpFunctionTable.Open(Device, DeviceEntry);
                break;

#if !defined(DEVICE_MIN) && !defined(ARM) && !defined(ARM64)
            case DEVICE_VMBUS:
                Status = VmbusDeviceFunctionTable.Open(Device, DeviceEntry);
                break;
#endif

            default:
                Status = STATUS_NOT_IMPLEMENTED;
                break;
            }

            if (!NT_SUCCESS(Status)) {
                goto BlpDeviceOpenEnd;
            }
        }

        //
        // Find open spot in device table.
        //

        Status = BlTblSetEntry(&DmDeviceTable,
                               &DmTableEntries,
                               DeviceEntry,
                               DeviceId,
                               DeviceTablePurge);

        if (!NT_SUCCESS(Status)) {
            goto BlpDeviceOpenEnd;
        }

        DeviceEntry->DeviceId = *DeviceId;

    } else {

        //
        // If the device was already opened, then increment the reference count
        // and use the same device entry.
        //

        *DeviceId = DeviceEntry->DeviceId;
        DeviceEntry->ReferenceCount += 1;

        //
        // Set the device as being open.  We may be reusing the device, so it
        // is essential that we mark this bit.
        //

        SET_DEVICE_OPEN(DeviceEntry->Attributes);
        Status = STATUS_SUCCESS;
    }

BlpDeviceOpenEnd:
    if (!NT_SUCCESS(Status)) {
        if (DeviceEntry != NULL) {
            if (DeviceEntry->DeviceIdentifier != NULL) {
                BlMmFreeHeap(DeviceEntry->DeviceIdentifier);
            }

            BlMmFreeHeap(DeviceEntry);
        }
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlpDeviceIncrementReference (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    Increments the reference on a device.

Arguments:

    DeviceId - Handle to a device instance.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DmTableEntries <= DeviceId) || (DmDeviceTable[DeviceId] == NULL)) {
        Status = STATUS_INVALID_PARAMETER;

    } else {

        ASSERT(DmDeviceTable[DeviceId]->ReferenceCount != 0);

        DmDeviceTable[DeviceId]->ReferenceCount += 1;
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

NTSTATUS
BlpDeviceDecrementReference (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    Decrements the reference on a device.

Arguments:

    DeviceId - Handle to a device instance.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DeviceId is invalid.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    if ((DmTableEntries <= DeviceId) || (DmDeviceTable[DeviceId] == NULL)) {
        Status = STATUS_INVALID_PARAMETER;

    } else {

        ASSERT(DmDeviceTable[DeviceId]->ReferenceCount != 0);

        DmDeviceTable[DeviceId]->ReferenceCount -= 1;
        if (DmDeviceTable[DeviceId]->ReferenceCount == 0) {
            CLEAR_DEVICE_OPEN(DmDeviceTable[DeviceId]->Attributes);

            if (DmDeviceTable[DeviceId]->DeviceIdentifier->DeviceType == DEVICE_VMBUS) {

                //
                // When closing a vmbus channel, we must call the Device's close
                // routine.  This is because the Vmbus protocol first performs a
                // protocol negotiation when a new channel is opened.
                //

                DeviceTableDestroyEntry(DmDeviceTable[DeviceId], DeviceId);
            }
        }

        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

BOOLEAN
BlpDeviceTestAccessMode (
    __in DEVICE_ID DeviceId,
    __in OPEN_FLAGS AccessFlags
    )

/*++

Routine Description:

    This function tests the supplied access mode flags for compatibility with
    the supplied device.

Arguments:

    DeviceId - Supplies the identifier of the device to test for compatibility.

    AccessFlags - Supplies flags describing the access mode being tested.

Return Value:

    TRUE is returned if the supplied access mode is compatible with the access
    mode of the supplied device.  FALSE is returned otherwise.

--*/

{

    PDEVICE_ENTRY DeviceEntry;
    BOOLEAN Result;

    BlpInterfaceEnter(InterfaceDevice);
    if ((DmTableEntries <= DeviceId) || (DmDeviceTable[DeviceId] == NULL)) {
        Result = FALSE;
        goto BlpDeviceTestAccessModeEnd;
    }

    //
    // Determine whether all requested access rights are available on the
    // supplied device.
    //

    DeviceEntry = DmDeviceTable[DeviceId];
    if (((IS_OPEN_READ_SET(AccessFlags) != FALSE) &&
         (IS_DEVICE_OPENED_FOR_READ(DeviceEntry->Attributes) == FALSE)) ||
        ((IS_OPEN_WRITE_SET(AccessFlags) != FALSE) &&
         (IS_DEVICE_OPENED_FOR_WRITE(DeviceEntry->Attributes) == FALSE))) {

        Result = FALSE;

    } else {
        Result = TRUE;
    }

BlpDeviceTestAccessModeEnd:
    BlpInterfaceExit(InterfaceDevice);
    return Result;
}

PDEVICE_ENTRY
BlpDeviceGetEntry (
    __in DEVICE_ID DeviceId
    )

/*++

Routine Description:

    This function returns a device entry for the corresponding device ID.

Arguments:

    DeviceId - Supplies the identifier of the device to test for compatibility.

Return Value:

    PDEVICE_ENTRY.

--*/
{
    if (DmTableEntries < DeviceId) {
        return NULL;
    }

    return DmDeviceTable[DeviceId];
}

BOOLEAN
BlpValidateBootEnvironmentDevice (
    __in_xcount(polymorphism) PBOOT_ENVIRONMENT_DEVICE Device,
    __in ULONG DeviceSize
    )

{

    if (DeviceSize < RTL_SIZEOF_THROUGH_FIELD(BOOT_ENVIRONMENT_DEVICE, Size)) {
        return FALSE;
    }

    if (Device->Size < DeviceSize) {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
BlpValidateRange (
    __in PVOID ConfigBase,
    __in SIZE_T ConfigSize,
    __in PVOID Pointer,
    __in SIZE_T Size
    )
{

    NT_ASSERT(ConfigSize <= MAXULONG);

    if ((Size != 0) &&
        (ConfigBase <= Pointer) &&
        (((ULONG_PTR)Pointer + Size) >= (ULONG_PTR)Pointer) &&
        (((ULONG_PTR)ConfigBase + ConfigSize) >= (ULONG_PTR)ConfigBase) &&
        (((ULONG_PTR)Pointer + Size) <= ((ULONG_PTR)ConfigBase + ConfigSize))) {

        return TRUE;
    }

    return FALSE;
}

NTSTATUS
BlpValidateOverlayConfig (
    __in_bcount(OverlayConfigSize) POVERLAY_CONFIG OverlayConfig,
    __in ULONG OverlayConfigSize
    )
{

    NTSTATUS Status;
    ULONG OverlayCount;
    POVERLAY_INFO OverlayInfo;
    ULONG Index;
    PBOOT_ENVIRONMENT_DEVICE Device;
    ULONG DeviceSize;
    SIZE_T Size;

    NT_ASSERT(OverlayConfigSize >= FIELD_OFFSET(OVERLAY_CONFIG, OverlayInfo));

    //
    // Verify configuration file signature and version.
    //

    if (OverlayConfig->Signature != OVERLAY_CONFIG_SIGNATURE) {
        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto ValidateOverlayConfigEnd;
    }

    if (OverlayConfig->Version != OVERLAY_CONFIG_CURRENT_VERSION) {
        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto ValidateOverlayConfigEnd;
    }

    //
    // Check for minimum allowed OverlayInfo size.
    //

    if (OverlayConfig->OverlayInfoSize < sizeof(OVERLAY_INFO_V0)) {
        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto ValidateOverlayConfigEnd;
    }

    OverlayCount = OverlayConfig->OverlayCount;
    if (OverlayCount > 0) {

        if (OverlayCount > MAX_OVERLAY_COUNT) {
            Status = STATUS_CORRUPT_SYSTEM_FILE;
            goto ValidateOverlayConfigEnd;
        }

        Status = RtlSIZETMult(OverlayCount, OverlayConfig->OverlayInfoSize, &Size);
        if (!NT_SUCCESS(Status)) {
            Status = STATUS_CORRUPT_SYSTEM_FILE;
            goto ValidateOverlayConfigEnd;
        }

        if (BlpValidateRange(OverlayConfig,
                              OverlayConfigSize,
                              &OverlayConfig->OverlayInfo[0],
                              Size) == FALSE) {

            Status = STATUS_CORRUPT_SYSTEM_FILE;
            goto ValidateOverlayConfigEnd;
        }

        //
        // Validate OverlayDevice entries.
        //

        OverlayInfo = &OverlayConfig->OverlayInfo[0];
        for (Index = 0; Index < OverlayCount; Index += 1) {
            Device = (PBOOT_ENVIRONMENT_DEVICE)Add2Ptr(OverlayConfig, OverlayInfo->DeviceOffset);
            DeviceSize = OverlayInfo->DeviceSize;

            if (BlpValidateRange(OverlayConfig,
                                 OverlayConfigSize,
                                 Device,
                                 DeviceSize) == FALSE) {

                Status = STATUS_CORRUPT_SYSTEM_FILE;
                goto ValidateOverlayConfigEnd;
            }

            if (BlpValidateBootEnvironmentDevice(Device, DeviceSize) == FALSE) {
                Status = STATUS_CORRUPT_SYSTEM_FILE;
                goto ValidateOverlayConfigEnd;
            }

            OverlayInfo = Add2Ptr(OverlayInfo, OverlayConfig->OverlayInfoSize);
        }
    }

    Status = STATUS_SUCCESS;

ValidateOverlayConfigEnd:
     return Status;
}

// ----------------------------------------------------- Device Table Functions

NTSTATUS
DeviceTableDestroyEntry (
    PDEVICE_ENTRY DeviceEntry,
    ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the device table by calling the device's close routine
    and freeing it's memory.

Arguments:

    DeviceEntry - Entry in the device table.

    Index - Index in the device table for DeviceEntry.

Return Value:

    STATUS_SUCCESS when successful.

    Other status codes are possible depending on device implementation of
        Close.

--*/

{

    NTSTATUS Status;

    //
    // Close the device.
    //

    Status = DeviceEntry->FunctionTable.Close(DmDeviceTable[Index]);

    //
    // Free the entry's memory.  Note: the device manager is responsible for
    // allocating and freeing device identifier.
    //

    BlMmFreeHeap(DeviceEntry->DeviceIdentifier);
    if (DeviceEntry->Overlay != NULL) {
        BlpDeviceDestoryOverlay(DeviceEntry);
    }

    //
    // Purge any cache entries associated with the device going away.
    //

    BlockIoPurgeCacheByDevice(DeviceEntry);
    BlMmFreeHeap(DeviceEntry);
    DmDeviceTable[Index] = NULL;
    return Status;
}

NTSTATUS
DeviceTablePurge (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    For the BlDevice Api's, we do a lazy disposal of entries in the device
    table.  We do not purge their entries from the table until there is no
    longer any room for new devices.

    We make the assumption that the most costly operation is to open a device
    and initialize it.  We do a lazy purge of entries, because it may be
    possible to reuse an entry for a device that has been closed, avoiding
    the cost of collecting all the information for the device.

Arguments:

    DeviceEntry - Pointer to a device entry.

Return Value:

    STATUS_UNSUCCESSFUL when device is opened.

    STATUS_SUCCESS when a closed device can successfully be closed.

    Other errors possible depending on device implementation of Close.

--*/

{

    //
    // If the device is closed, allow for the deletion of the the entry.
    // Make sure to officially close the device by calling the device's
    // close routine.
    //

    if (!IS_DEVICE_OPENED(DeviceEntry->Attributes)) {
        return DeviceTableDestroyEntry(DeviceEntry, DeviceEntry->DeviceId);
    }

    return STATUS_UNSUCCESSFUL;
}

BOOLEAN
DeviceTableCompare (
    __in PDEVICE_ENTRY DeviceEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel,
    __in_opt PVOID Arg4
    )

/*++

Routine Description:

    This routine attempts to find an entry in the device table that can be used
    for the specified device with the prescribed attributes.

Arguments:

    DeviceEntry - Supplies a device entry in the device table.

    Device - Supplies an identifier for the device to be opened.

    OpenFlags - Supplies the flags to open the device with.

    FilterLevel - Supplies the filter level for the the device.

    Arg4 - Ignored.

Return Value:

    TRUE, if the device is opened with flags that are supported for multiple
        consumers.

    FALSE, otherwise.

--*/

{

    UNREFERENCED_PARAMETER(Arg4);

    //
    // Return unsuccessfully if the entry describes a different device, or if
    // the entry describes the same device but at a different filter level.
    //

    if ((BlpDeviceCompare(DeviceEntry->DeviceIdentifier, Device) == FALSE) ||
        (DeviceEntry->FilterLevel != *FilterLevel)) {

        return FALSE;
    }

    //
    // Maintain a usage policy which allows "readers" and "writers" to
    // share devices.
    //
    // N.B. IS_DEVICE_READ_SET / IS_DEVICE_WRITE_SET check to see if the
    //      device was initialized with Read or Write access.  Even if the
    //      device is now closed, if the device entry is to be reused, it must
    //      have been initialized with the requested access rights.
    //
    // IS_OPEN_NO_FVE_SET must be checked because reads from an FVE device that
    // was opened without FVE access will return encrypted data directly from
    // the BlockIo layer. Writes will have similar problems.
    //
    // OPEN_BY_HASH will load a subset of metadata and is not compatible
    // with regular device opens.
    //

    if ((IS_OPEN_READ_SET(*OpenFlags) &&
         !IS_DEVICE_READ_SET(DeviceEntry->Attributes)) ||
        (IS_OPEN_WRITE_SET(*OpenFlags) &&
         !IS_DEVICE_WRITE_SET(DeviceEntry->Attributes)) ||
        (!IS_OPEN_NO_FVE_SET(*OpenFlags) &&
         IS_OPEN_NO_FVE_SET(DeviceEntry->Attributes)) ||
        (IS_OPEN_NO_FVE_SET(*OpenFlags) &&
         !IS_OPEN_NO_FVE_SET(DeviceEntry->Attributes))||
        (!IS_OPEN_BY_HASH_SET(*OpenFlags) &&
         (IS_DEVICE_OPEN_BY_HASH_SET(DeviceEntry->Attributes)))) {

        return FALSE;
    }

    return TRUE;
}

NTSTATUS
DmOpenRegisteredDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Attempts to open the specified device as one of the registered device
    classes.

Arguments:

    Device - The device to open.

    DeviceEntry - Pointer to internal device data structure.  When a device
        is successfully opened, the data structure will be initialized for
        the appropriate device class.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    PREGISTERED_DEVICE_CLASS DeviceClass;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    //
    // Walk the list of registered device classes, attempting to open the
    // specified device as each.
    //

    NextEntry = DmRegisteredDevices.Flink;
    while (NextEntry != &DmRegisteredDevices) {
        DeviceClass = CONTAINING_RECORD(NextEntry,
                                        REGISTERED_DEVICE_CLASS,
                                        ListEntry);

        Status = DeviceClass->FunctionTable.Open(Device, DeviceEntry);
        if (Status == STATUS_SUCCESS) {
            return Status;
        }

        NextEntry = NextEntry->Flink;
    }

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS
BlpDeviceSetOverlayInformation (
    _In_ DEVICE_ID DeviceId,
    _In_ DEVICE_ID OverlayedDeviceId,
    _In_ PLARGE_INTEGER DataSourceId
    )

/*++

Routine Description:

    This routine returns the id for the device that the supplied device is
    overlaying.

Arguments:

    DeviceId - Supplies the id of the overlay device.

Return Value:

    DEVICE_ID of the overlayed device if successful.

    INVALID_DEVICE_ID if the device does not overlay another device.

--*/

{

    PDEVICE_ENTRY Device;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Make sure this is a valid device id.
    //

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
         (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto DeviceGetOverlayInfoEnd;
    }

    Device = DmDeviceTable[DeviceId];
    Device->OverlayInfo.DeviceId = OverlayedDeviceId;
    Device->OverlayInfo.DataSourceId = *DataSourceId;
    Status = STATUS_SUCCESS;

DeviceGetOverlayInfoEnd:
    BlpInterfaceExit(InterfaceDevice);
    return Status;
}

DEVICE_ID
BlDeviceGetOverlayInformation (
    _In_ DEVICE_ID DeviceId,
    _Out_ PLARGE_INTEGER DataSource
    )

/*++

Routine Description:

    This routine returns the id for the device that the supplied device is
    overlaying.

Arguments:

    DeviceId - Supplies the id of the overlay device.

Return Value:

    DEVICE_ID of the overlayed device if successful.

    INVALID_DEVICE_ID if the device does not overlay another device.

--*/

{

    PDEVICE_ENTRY Device;
    DEVICE_ID OverlayedDeviceId;

    OverlayedDeviceId = INVALID_DEVICE_ID;

    BlpInterfaceEnter(InterfaceDevice);

    //
    // Make sure this is a valid device id.
    //

    if ((DmTableEntries <= DeviceId) ||
        (DmDeviceTable[DeviceId] == NULL) ||
         (!IS_DEVICE_OPENED(DmDeviceTable[DeviceId]->Attributes))) {

        goto DeviceGetOverlayInfoEnd;
    }

    //
    // OverlayEntry may be an array > 1 but they all should be overlaying the
    // same original device and so we can just use the first.
    //

    Device = DmDeviceTable[DeviceId];
    OverlayedDeviceId = Device->OverlayInfo.DeviceId;
    if (OverlayedDeviceId != INVALID_DEVICE_ID) {
        *DataSource = Device->OverlayInfo.DataSourceId;
    }


DeviceGetOverlayInfoEnd:
    BlpInterfaceExit(InterfaceDevice);
    return OverlayedDeviceId;
}


