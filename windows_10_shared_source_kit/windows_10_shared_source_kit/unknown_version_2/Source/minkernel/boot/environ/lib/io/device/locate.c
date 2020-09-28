/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    locate.c

Abstract:

    This module implements support for resolving locate devices to their
    target devices. All resolved devices are cached in a global table to
    improve performance when the same locate device is opened multiple times.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "device.h"

//
// -------------------------------------------------------------------- Defines
//

//
// The number of initial entries in the locate table.
//

#define INITIAL_TABLE_ENTRIES (8)

//
// ---------------------------------------------------------------------- Types
//

//
// Each entry within the locate table contains the source locate device and
// its target.
//

typedef struct _LOCATE_MAPPING_ENTRY {
    PBOOT_ENVIRONMENT_DEVICE Source;
    PBOOT_ENVIRONMENT_DEVICE Target;
} LOCATE_MAPPING_ENTRY, *PLOCATE_MAPPING_ENTRY;

//
// -------------------------------------------------------------------- Globals
//

//
// Define a global cache to hold the result of locate mappings. The table
// is dynamic is size and therefore the library does not have a limit on number
// of devices.
//

BOOLEAN DmLocateMappingInitialized = FALSE;
PLOCATE_MAPPING_ENTRY *DmLocateMappingTable = NULL;
ULONG DmLocateTableEntries = 0;

//
// ----------------------------------------------------------------- Prototypes
//

BOOLEAN
LocateMappingCompare (
    __in PLOCATE_MAPPING_ENTRY MappingEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    );

NTSTATUS
LocateMappingDestroy (
    VOID
    );

NTSTATUS
LocateMappingDestroyEntry (
    __in PLOCATE_MAPPING_ENTRY MappingEntry,
    __in ULONG Index
    );

NTSTATUS
LocateMappingFind (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *TargetDevice
    );

NTSTATUS
LocateMappingInitialize (
    VOID
    );

NTSTATUS
LocateMappingInsert (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PBOOT_ENVIRONMENT_DEVICE TargetDevice
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BlpDeviceResolveLocate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *TargetDevice
    )

/*++

Routine Description:

    This routine will resolve a locate device to its target device.

    N.B. This routine only supports resolution of string-type locate devices.
         Other locate devices should be converted to this type prior to
         calling this routine.

Arguments:

    Device - Supplies the locate device identifier to be resolved.

    TargetDevice - Supplies a pointer to a variable that receives the target
        device.

Return Value:

    NT status code.

--*/

{

    PDEVICE_ID DeviceId;
    GENERIC_BUFFER DeviceIdBuffer;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    FILE_ID FileId;
    ULONG Index;
    LOCATE_IDENTIFIER UNALIGNED *LocateIdentifier;
    PBOOT_ENVIRONMENT_DEVICE LocateParent;
    PWCHAR LocatePath;
    ULONG NumberOfDevices;
    PBOOT_ENVIRONMENT_DEVICE PartitionIdentifier;
    ULONG PartitionIdentifierSize;
    ULONG PathSize;
    NTSTATUS Status;
    DEVICE_ID TargetId;

    BlInitGenericBuffer(&DeviceIdBuffer, NULL, 0);
    LocatePath = NULL;
    PartitionIdentifier = NULL;

    //
    // Check to see if the input device was already resolved to its target, and
    // if so, return the cached result.
    //

    Status = LocateMappingFind(Device, TargetDevice);
    if (NT_SUCCESS(Status)) {
        goto ResolveLocateEnd;
    }

    //
    // Only string type LOCATE devices can be resolved to their target
    // devices by the device library. All other locate devices must be
    // converted prior to calling this routine.
    //

    LocateIdentifier = NULL;
    if ((Device->DeviceType != DEVICE_LOCATE) ||
        (Device->u.Locate.Type != LOCATE_TYPE_STRING)) {

        Status = STATUS_NOT_SUPPORTED;
        goto ResolveLocateEnd;

    } else {
        LocateIdentifier = &Device->u.Locate;
    }

    //
    // If a parent device exists for the locate device, then only partitions
    // existent on that device should be enumerated. Otherwise, all partitions
    // on the machine need to be enumerated. To restrict partition enumeration
    // only on a certain device, the parent identifier needs to be set as that
    // device within the enumeration (partition) device.
    //
    // Determine the size of the partition enumeration identifier including
    // the size needed for the parent.
    //

    if (LocateIdentifier->ParentOffset == 0) {
        LocateParent = NULL;
        PartitionIdentifierSize = sizeof(BOOT_ENVIRONMENT_DEVICE);

    } else {
        LocateParent = Add2Ptr(Device, LocateIdentifier->ParentOffset);
        PartitionIdentifierSize = VHD_PARTITION_EX_FILE_OFFSET;
        PartitionIdentifierSize -= sizeof(VIRTUAL_HARD_DISK_IDENTIFIER);
        PartitionIdentifierSize += LocateParent->Size;
        PartitionIdentifierSize -= FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                                u.BlockIo);
    }

    //
    // Allocate and intialize the partition identifier.
    //
    // N.B. Zeroing out the partition identifier sets the parent identifier
    //      to BLOCK_IO_DEVICE and BLOCK_IO_HARD_DISK.
    //

    PartitionIdentifier = BlMmAllocateHeap(PartitionIdentifierSize);
    if (PartitionIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ResolveLocateEnd;
    }

    RtlZeroMemory(PartitionIdentifier, PartitionIdentifierSize);
    PartitionIdentifier->DeviceType = DEVICE_PARTITION_EX;
    PartitionIdentifier->Size = PartitionIdentifierSize;

    //
    // Copy the locate identifier's parent device as the parent identifier
    // for the partition. This will cause only partitions residing on the
    // parent device to be enumerated.
    //

    if (LocateParent != NULL) {
        RtlCopyMemory(&PartitionIdentifier->u.PartitionEx.ParentIdentifier,
                      &LocateParent->u.BlockIo,
                      LocateParent->Size -
                        FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo));

    }

    //
    // Enumerate partitions on the parent device. If the parent device is
    // BLOCK_IO_HARD_DISK, then all the partitions on the machine will be
    // enumerated. Otherwise, only partitions on the parent device will be
    // enumerated.
    //

    Status = BlDeviceEnumerateDeviceClass(PartitionIdentifier,
                                          &DeviceIdBuffer,
                                          TRUE);

    if (!NT_SUCCESS(Status) || (DeviceIdBuffer.Buffer == NULL)) {
        goto ResolveLocateEnd;
    }

    //
    // Set the file path to be used to resolve the partition device. The file
    // path may be unaligned, align it by copying into a new (aligned) buffer.
    //

    PathSize = Device->Size - FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.Locate)
        - FIELD_OFFSET(LOCATE_IDENTIFIER, Path);

    LocatePath = BlMmAllocateHeap(PathSize);
    if (LocatePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ResolveLocateEnd;
    }

    RtlCopyMemory(LocatePath, LocateIdentifier->Path, PathSize);

    //
    // Now all the partitions are open. Search through them all looking for
    // the partition that contains the specified file path.
    //

    DeviceId = (PDEVICE_ID)DeviceIdBuffer.Buffer;
    NumberOfDevices = DeviceIdBuffer.DataSize / sizeof(DEVICE_ID);
    Status = STATUS_UNSUCCESSFUL;
    TargetId = INVALID_DEVICE_ID;
    for (Index = 0; Index < NumberOfDevices; Index += 1) {
        Status = BlFileOpen(DeviceId[Index], LocatePath, OPEN_READ, &FileId);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        BlFileClose(FileId);

        //
        // The file was found on this partition, return the matching partition
        // device to the caller.
        //

        TargetId = DeviceId[Index];
        Status = STATUS_SUCCESS;
        break;
    }

    //
    // If the device could be successfully located, return the device
    // identifier for the target device. Otherwise, fallback to the boot device.
    //

    if (NT_SUCCESS(Status)) {
        Status = BlGetDeviceIdentifier(TargetId, &DeviceIdentifier);
        if (!NT_SUCCESS(Status)) {
            goto ResolveLocateEnd;
        }

        *TargetDevice = DeviceIdentifier;

    } else {
        *TargetDevice = BlGetBootDevice();
        Status = STATUS_SUCCESS;
    }

    //
    // If the device was successfully resolved, cache the result. Do not
    // treat failure to cache the result as fatal.
    //

    if (NT_SUCCESS(Status)) {
        LocateMappingInsert(Device, *TargetDevice);
    }

ResolveLocateEnd:
    if (DeviceIdBuffer.Buffer != NULL) {
        BlMmFreeHeap(DeviceIdBuffer.Buffer);
    }

    if (LocatePath != NULL) {
        BlMmFreeHeap(LocatePath);
    }

    if (PartitionIdentifier != NULL) {
        BlMmFreeHeap(PartitionIdentifier);
    }

    return Status;
}

//
// --------------------------------------------------------- Internal Functions
//

BOOLEAN
LocateMappingCompare (
    __in PLOCATE_MAPPING_ENTRY MappingEntry,
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    )

/*++

Routine Description:

    This routine determines if the given mapping entry contains the target
    device for the specified locate device.

Arguments:

    MappingEntry - Supplies an entry in the locate table.

    Device - Supplies a device identifier to be matched.

Return Value:

    TRUE if the locate entry is for the specified device. FALSE otherwise.

--*/

{

    BOOLEAN Result;

    UNREFERENCED_PARAMETER(Arg2);
    UNREFERENCED_PARAMETER(Arg3);
    UNREFERENCED_PARAMETER(Arg4);

    //
    // For a successful match, the identifiers must match exactly.
    //

    Result = FALSE;
    if ((MappingEntry != NULL) &&
        (MappingEntry->Source->Size == Device->Size)) {

        Result = RtlEqualMemory(MappingEntry->Source,
                                Device,
                                Device->Size);
    }

    return Result;
}

NTSTATUS
LocateMappingDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys all cached locate mappings. This is called by
    the IO manager during library destruction.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    //
    // Destroy each cached entry in the locate table.
    //

    Status = BlTblMap(DmLocateMappingTable,
                      DmLocateTableEntries,
                      LocateMappingDestroyEntry,
                      FALSE);

    //
    // Free the memory used for the table itself.
    //

    BlMmFreeHeap(DmLocateMappingTable);
    DmLocateMappingTable = NULL;
    DmLocateTableEntries = 0;
    return Status;
}

NTSTATUS
LocateMappingDestroyEntry (
    __in PLOCATE_MAPPING_ENTRY MappingEntry,
    __in ULONG Index
    )

/*++

Routine Description:

    This routine destroys an entry within the locate mapping table.

Arguments:

    DeviceEntry - Pointer to the device's entry in the device table.

    Index - Index in the block i/o device table.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    //
    // Free the memory allocated for storing the locate mapping and clear the
    // entry in the table.
    //

    if (MappingEntry != NULL) {
        BlMmFreeHeap(MappingEntry);
    }

    DmLocateMappingTable[Index] = NULL;
    return STATUS_SUCCESS;
}

NTSTATUS
LocateMappingFind (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __deref_out PBOOT_ENVIRONMENT_DEVICE *TargetDevice
    )

/*++

Routine Description:

    This routine checks whether a mapping exists for the given device in
    the global mapping table.

Arguments:

    Device - Supplies the locate device identifier to be resolved.

    TargetDevice - Supplies a pointer to a variable that receives the target
        device.

Return Value:

    STATUS_SUCCESS if the mapping exists. STATUS_NOT_FOUND otherwise.

--*/

{

    PLOCATE_MAPPING_ENTRY LocateMapping;
    NTSTATUS Status;
    ULONG UnusedId;

    //
    // If the locate table hasn't yet been intialized, then the mapping cannot
    // exist.
    //

    Status = STATUS_NOT_FOUND;
    if (DmLocateMappingInitialized == FALSE) {
        goto LocateMappingFindEnd;
    }

    //
    // Look up the locate mapping table for a matching device.
    //

    LocateMapping = BlTblFindEntry(DmLocateMappingTable,
                                   DmLocateTableEntries,
                                   &UnusedId,
                                   LocateMappingCompare,
                                   Device,
                                   NULL,
                                   NULL,
                                   NULL);

    if (LocateMapping != NULL) {
        *TargetDevice = LocateMapping->Target;
        Status = STATUS_SUCCESS;
    }

LocateMappingFindEnd:
    return Status;
}

NTSTATUS
LocateMappingInitialize (
    VOID
    )

/*++

Routine Description:

    This routine allocates and initializes the global locate mappings table.
    It registers a destroy routine with the IO manager to get called during
    library destruction.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful. STATUS_NO_MEMORY otherwise.

--*/

{

    ULONG Size;
    NTSTATUS Status;

    //
    // Allocate buffer for the locate mappings table.
    //

    DmLocateTableEntries = INITIAL_TABLE_ENTRIES;
    Size = DmLocateTableEntries * sizeof(PLOCATE_MAPPING_ENTRY);
    DmLocateMappingTable = BlMmAllocateHeap(Size);
    if (DmLocateMappingTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LocateMappingInitializeEnd;
    }

    RtlZeroMemory(DmLocateMappingTable, Size);

    //
    // Register a destroy routine with the I/O manager to allow the memory
    // allocated for the table to be freed upon destruction.
    //

    Status = BlpIoRegisterDestroyRoutine(LocateMappingDestroy);
    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(DmLocateMappingTable);
        goto LocateMappingInitializeEnd;
    }

    DmLocateMappingInitialized = TRUE;

LocateMappingInitializeEnd:
    return Status;
}

NTSTATUS
LocateMappingInsert (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PBOOT_ENVIRONMENT_DEVICE TargetDevice
    )

/*++

Routine Description:

    This routine will insert the given locate mapping into the locate table.
    If the locate table is not yet intialized, this routine will invoke the
    initialization routine.

Arguments:

    Device - Supplies the locate device identifier to be resolved.

    TargetDevice - Supplies a pointer to a variable that receives the target
        device.

Return Value:

    NT status code.

--*/

{

    PLOCATE_MAPPING_ENTRY MappingEntry;
    PBOOT_ENVIRONMENT_DEVICE LocateIdentifier;
    ULONG Size;
    NTSTATUS Status;
    PBOOT_ENVIRONMENT_DEVICE TargetIdentifier;
    ULONG UnusedId;

    MappingEntry = NULL;

    //
    // Initialize the locate mapping table on the first insertion.
    //

    if (DmLocateMappingInitialized == FALSE) {
        Status = LocateMappingInitialize();
        if (!NT_SUCCESS(Status)) {
            goto LocateMappingInsertEnd;
        }
    }

    Size = sizeof(LOCATE_MAPPING_ENTRY) + Device->Size + TargetDevice->Size;
    MappingEntry = BlMmAllocateHeap(Size);
    if (MappingEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LocateMappingInsertEnd;
    }

    //
    // The device identifier may be freed when the caller closes the device.
    // Create a copy of the input device identifiers.
    //

    LocateIdentifier = Add2Ptr(MappingEntry, sizeof(LOCATE_MAPPING_ENTRY));
    TargetIdentifier = Add2Ptr(LocateIdentifier, Device->Size);
    RtlCopyMemory(LocateIdentifier, Device, Device->Size);
    RtlCopyMemory(TargetIdentifier, TargetDevice, TargetDevice->Size);

    //
    // Initialize the mapping entry.
    //

    MappingEntry->Source = LocateIdentifier;
    MappingEntry->Target = TargetIdentifier;

    //
    // Find an open spot in locate table and insert the mapping.
    //

    Status = BlTblSetEntry(&DmLocateMappingTable,
                           &DmLocateTableEntries,
                           MappingEntry,
                           &UnusedId,
                           TblDoNotPurgeEntry);

LocateMappingInsertEnd:
    if (!NT_SUCCESS(Status)) {
        if (MappingEntry != NULL) {
            BlMmFreeHeap(MappingEntry);
        }
    }

    return Status;
}


