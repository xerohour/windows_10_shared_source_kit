/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ramapi.c

Abstract:

    Implementation of a RAM buffer as a block i/o device.

Environment:

    Boot

--*/


// ------------------------------------------------------------------- Includes

#include "block.h"
#include <blimage.h>
#include <bmbuild.h>
#include "sdi.h"
#include <stdio.h>

// ------------------------------------------------- Data Structure Definitions

typedef struct _RAMDISK_DEVICE {
    struct {
        PUNALIGNED_FILE_PATH SdiPath;
        ULONG RamdiskImageOffset;
        ULONGLONG RamdiskImageLength;
        USHORT TftpClientPort;
        ULONG TftpBlockSize;
        ULONG TftpWindowSize;
        BOOLEAN TftpVariableWindowEnabled;
        ULONG MulticastOptions;
    } Options;

    PRAM_DISK_IDENTIFIER Identifier;

    struct {
        PUCHAR SourceFileHash;
        ULONG SourceFileHashSize;
    } IntegrityInfo;

} RAMDISK_DEVICE, *PRAMDISK_DEVICE;

/*++

RAM Disk Device Description:

    Data structure describing a RAM disk created by this module.

Fields:

    Options - Structure containing optional information for a RAM disk device,
        all used at creation time.  Options include :

        SdiPath - Device and file path to a file containing the SDI header
            for a RAM disk device.

        ImageOffset - A conditional option specifying the offset to the
            start of the image where the actual ramdisk image is. This option
            is ignored if a SDI header is present at the base of the image.

        TftpClientPort - Specifies the local port to use when performing a
            TFTP download request for a RAM disk image.

        TftpBlockSize - Specifies the tftp block size to use when downloading
            the ramdisk image.

        TftpWindowSize - Specifies the tftp window size to use when downloading
            the randisk image.

        TftpVariableWindowEnabled - Specifies if the tftp variable window size
            extension is enabled.

        MulticastOptions - Specifies options that control multicast download of
            images for ramdisk.

    Identifier - RAM disk device identifier for a RAM disk device after it has
        been created.  This contains the source file for the ramdisk and the
        loaded image information.

    IntegrityInfo - Structure containing optional integrity information for a
        RAM disk device. This is used to inform FVE of integrity information
        related to the source file. Fields include :

        SourceFileHash - The hash of the source WIM file.

        SourceFileHashSize - The length in bytes of the hash.

--*/


// ---------------------------------------------------------------- Definitions

#define INITIAL_TABLE_ENTRIES  (4)

#define FIND_SDI_PART_BLOB      1
#define FIND_SDI_WIM_BLOB       2

#define RAMDISK_ENABLE_MULTICAST    1
#define RAMDISK_ENABLE_FALLBACK     2

#define IS_RAMDISK_MULTICAST_ENABLED(Ramdisk)   \
            CHECK_FLAG(Ramdisk->Options.MulticastOptions, \
                        RAMDISK_ENABLE_MULTICAST)

#define IS_RAMDISK_FALLBACK_ENABLED(Ramdisk) \
            CHECK_FLAG(Ramdisk->Options.MulticastOptions, \
                RAMDISK_ENABLE_FALLBACK)

// -------------------------------------------------------------------- Globals

PRAMDISK_DEVICE *RdpCreationTable = NULL;
ULONG RdpCreationTableEntries = 0;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
RdDeviceOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
RdDeviceRead (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    );

NTSTATUS
RdDeviceWrite (
    __in PDEVICE_ENTRY DeviceEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesWritten
    );

NTSTATUS
RdDeviceEnumerate (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    );

NTSTATUS
RdDeviceCreate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION Options,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    );

// ------------------------------------------------ Utility Function Prototypes

NTSTATUS
RdpParseOptions (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION Options,
    __out PRAMDISK_DEVICE RamdiskDevice
    );

NTSTATUS
RdpFreeRamdiskDevice (
    __in PRAMDISK_DEVICE RamDiskOptions
    );

NTSTATUS
RdpLoadImage (
    __inout PRAMDISK_DEVICE RamdiskDevice
    );

NTSTATUS
RdpLoadFileToImage (
    __in PUNALIGNED_FILE_PATH FilePath,
    __in PVOID ImageBase,
    __in ULONGLONG ImageSize,
    __in BOOLEAN ShowProgress,
    __in BOOLEAN CalculateFileHash,
    __deref_opt_out_bcount_opt(*ImageHashSize) PUCHAR* ImageHash,
    __out_opt PULONG ImageHashSize
    );

NTSTATUS
RdpCreateDeviceIdentifier (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PRAM_DISK_IDENTIFIER RamdiskIdentifier,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    );

NTSTATUS
RdpUnlockSourceDevice (
    __in PUNALIGNED_FILE_PATH FilePath
    );

ULONGLONG
RdpQueryImageSize (
    __in PUNALIGNED_FILE_PATH FilePath
    );

NTSTATUS
RdpFindRamdiskRange (
    __in PVOID ImageBase,
    __in ULONGLONG ImageSize,
    __in ULONG Flags,
    __out PULONG RamdiskOffset,
    __out PULONGLONG RamdiskLength
    );

VOID
RdpCorrectRamdiskRange (
    __in ULONGLONG ImageSize,
    __inout PULONG RamdiskOffset,
    __inout PULONGLONG RamdiskLength
    );

NTSTATUS
RdpPopulateBlockIoDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PBLOCK_IO_DEVICE BlockIoDevice
    );

NTSTATUS
RdpAddToBlockIoTable (
    __in PBOOT_ENVIRONMENT_DEVICE Device
    );

VOID
RdpUpdateImageLoadingProgress (
    __in PVOID Context,
    __in ULONG PercentComplete
    );

BOOLEAN
RdpIsCompressed (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONGLONG ImageSize
    );

NTSTATUS
RdpDecompressImage (
    __inout_bcount(*ImageSize) PVOID *ImageBase,
    __inout PULONGLONG ImageSize
    );

ULONG
RdpXpressCheckSum (
    __in ULONG PartialSum,
    __in_bcount(Length) PVOID Source,
    __in ULONG Length
    );

// ---------------------------------------------------- Creation Table Routines

NTSTATUS
RdpCreationTableInitialize (
    VOID
    );

NTSTATUS
RdpCreationTableDestroy (
    VOID
    );

BOOLEAN
RdpCreationTableCompare (
    __in PRAMDISK_DEVICE TableEntry,
    __in PRAMDISK_DEVICE RamdiskDevice,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    );

NTSTATUS
RdpCreationTableDestroyEntry (
    __in PRAMDISK_DEVICE TableEntry,
    __in ULONG Index
    );

// -------------------------------------------------------------------- Globals

//
// Function table for a ram block i/o device.
//

const DEVICE_FUNCTION_TABLE RamDiskDeviceFunctionTable =
{
    (PDEVICE_ENUMERATE_ROUTINE)       RdDeviceEnumerate,
    (PDEVICE_OPEN_ROUTINE)            RdDeviceOpen,
    (PDEVICE_CLOSE_ROUTINE)           NULL,
    (PDEVICE_READ_ROUTINE)            RdDeviceRead,
    (PDEVICE_WRITE_ROUTINE)           RdDeviceWrite,
    (PDEVICE_GET_INFORMATION_ROUTINE) NULL,
    (PDEVICE_SET_INFORMATION_ROUTINE) NULL,
    (PDEVICE_RESET_ROUTINE)           NULL,
    (PDEVICE_FLUSH_ROUTINE)           NULL,
    (PDEVICE_CREATE_ROUTINE)          RdDeviceCreate
};

// ------------------------------------------------------------------ Functions

NTSTATUS
RdDeviceOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a RAM disk.  A RAM disk device is assumed to be created at the
    time of an open request.  RAM disks can have many formats, so there is
    no way to validate the device identifier describing the base and size of
    the RAM disk device to open.  Resultantly, the only work operations
    required by this routine is the initialization of the RAM disk's
    device entry.

    N.B.  This routine will only be called if a RAM disk is created by another
          boot application.  RdDeviceCreate will insert an entry into
          the Block I/O Device Table.  Resultantly, any open request by the
          same boot application will be serviced by BlockIoOpen using the
          cached table entry.

Arguments:

    Device - The ramdisk to open as a block i/o device.  A ramdisk is
        identified by its physical base and size.

    DeviceEntry - Pointer to the device entry.  Will be filled with all
        information for the device.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if a memory allocation for an internal data structure
        fails.

--*/

{

    NTSTATUS Status;

    //
    // Populate the Block I/O Device Table with an entry for the specified
    // ramdisk device.  The table contains a single device entry for each
    // Block I/O Device processed by the library.
    //
    // N.B. This step is done first since the remainder of the routine can not
    //      produce an error.
    //

    Status = RdpAddToBlockIoTable(Device);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Initialize the device entry for the specified RAM disk.
    //
    // N.B. BlockIoOpen allocates and initializes all common block I/O
    //      device structures.  This routine is responsible for populating
    //      the fields that are specific to a RAM disk device.
    //

    //
    // Populate the Block I/O Device structure with the base and size of the
    // RAM disk.
    //

    Status = RdpPopulateBlockIoDevice(Device, DeviceEntry->DeviceData);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // The Block I/O Device Function Table can be used for many generic
    // operation (GetInformation, SetInformation, Reset).  Hook all RAM disk
    // specific routines.
    //

    DeviceEntry->FunctionTable.Read = RdDeviceRead;
    DeviceEntry->FunctionTable.Write = RdDeviceWrite;
    return STATUS_SUCCESS;
}

NTSTATUS
RdDeviceRead (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesRead
    )

/*++

Routine Description:

    Read from the RAM disk device.

    All reads are done from the location of the internal block i/o pointer.

Arguments:

    DeviceEntry - Pointer to the device entry.

    Buffer - Pointer to address to write contents of the read operation.

    Size - Number of bytes to read.

    BytesRead - Number of bytes actually read.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a requested block is invalid.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    ULONGLONG Offset;
    PRAMDISK_INTERNAL_DATA RamDiskData;

    BlockIoDevice = (PBLOCK_IO_DEVICE)DeviceEntry->DeviceData;
    Offset = BlockIoDevice->Information.LbaOffset;
    RamDiskData = &BlockIoDevice->InternalData.u.RamDiskData;

    //
    // Ensure that no part of the read request is off the end of the RAM disk.
    //

    if ((BlockIoDevice->Information.LastBlock + 1) < (Offset + Size)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Since the entire RAM disk is mapped virtually into a contiguous memory
    // buffer, the read request is a simple memory copy.
    //

    RtlCopyMemory(Buffer, Add2Ptr(RamDiskData->RamDiskBase, Offset), Size);

    //
    // Update the device pointer and return to the caller.
    //

    BlockIoDevice->Information.LbaOffset += Size;

    if (BytesRead != NULL) {
        *BytesRead = Size;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RdDeviceWrite (
    __in PDEVICE_ENTRY DeviceEntry,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Write to the RAM disk device.

    All writes are done from the location of the internal block i/o pointer.

Arguments:

    DeviceEntry - Pointer to the device entry.

    Buffer - Pointer to a buffer containing the context to write to the device.

    Size - Number of bytes to write.

    BytesWritten - Number of bytes actually written.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a requested ramdisk or block is invalid.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    ULONGLONG Offset;
    PRAMDISK_INTERNAL_DATA RamDiskData;

    BlockIoDevice = (PBLOCK_IO_DEVICE)DeviceEntry->DeviceData;
    Offset = BlockIoDevice->Information.LbaOffset;
    RamDiskData = &BlockIoDevice->InternalData.u.RamDiskData;

    //
    // Ensure that no part of the write request is off the end of the RAM disk.
    //

    if ((BlockIoDevice->Information.LastBlock + 1) < (Offset + Size)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Since the entire RAM disk is mapped virtually into a contiguous memory
    // buffer, the write request is a simply memory copy.
    //

    RtlCopyMemory(Add2Ptr(RamDiskData->RamDiskBase, Offset), Buffer, Size);

    //
    // Update the device pointer and return to the caller.
    //

    BlockIoDevice->Information.LbaOffset += Size;

    if (BytesWritten != NULL) {
        *BytesWritten = Size;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RdDeviceEnumerate (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    )

/*++

Routine Description:

    Enumerates the ramdisk device class.  Ramdisks are added to the block I/O
    table when they are created.  This routine runs through each of the entries
    in the table looking for ramdisk entries, and opens them.  The resulting
    device IDs are returned.

Arguments:

    DeviceClassIdentifier - A boot environment device structure used to
        represent the device class to enumerate.  Only Device Class / Sub
        Class information is referenced.  (ie. DeviceType, BlockIoType).

    DeviceIdBuffer - Generic buffer structure for the array of device handles
        for all devices of the requested type.

    AllocateNewBuffer - Boolean value indicating whether the device handle
        buffer should be allocated.

Return Value:

    STATUS_SUCCESS if the device class is enumerated successfully.
    STATUS_NO_MEMORY if a memory allocation failed.
    STATUS_BUFFER_TOO_SMALL if the generic buffer is not large enough to hold
        the data.
    Another failure status code if a ramdisk device could not be opened.

--*/

{

    ULONG Count;
    PDEVICE_ENTRY DeviceEntry;
    DEVICE_ID DeviceId;
    ULONG Id;
    PBOOT_ENVIRONMENT_DEVICE Identifier;
    NTSTATUS Status;

#if DBG

    ASSERT(DeviceClassIdentifier->DeviceType == DEVICE_BLOCK_IO);
    ASSERT(DeviceClassIdentifier->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK);

#else

    UNREFERENCED_PARAMETER(DeviceClassIdentifier);

#endif

    //
    // Walk the Block I/O Device Table in search of RAM disks.  RAM disks
    // are inserted into the table at either creation time (when created by
    // the calling boot application) or open time (when created by another
    // boot application).  Therefore, all known RAM disk contain an entry
    // in the Block I/O Device Table.
    //

    Count = 0;
    for (Id = 0; Id < BlockIoDeviceTableEntries; Id += 1) {
        DeviceEntry = BlockIoDeviceTable[Id];
        if (DeviceEntry == NULL) {
            continue;
        }

        Identifier = DeviceEntry->DeviceIdentifier;
        if ((Identifier->DeviceType != DEVICE_BLOCK_IO) ||
            (Identifier->u.BlockIo.BlockIoType != BLOCK_IO_RAMDISK)) {

            continue;
        }

        Count += 1;
    }

    //
    // If the caller buffer is not large enough to hold the results, then
    // either allocate a new buffer if requested, or return an error.
    //

    if (DeviceIdBuffer->BufferSize < (Count * sizeof(DEVICE_ID))) {
        if (AllocateNewBuffer == FALSE) {
            DeviceIdBuffer->BufferSize = Count * sizeof(DEVICE_ID);
            return STATUS_BUFFER_TOO_SMALL;
        }

        if (DeviceIdBuffer->BufferSize != 0) {

            ASSERT(DeviceIdBuffer->Buffer != NULL);

            BlMmFreeHeap(DeviceIdBuffer->Buffer);
        }

        DeviceIdBuffer->BufferSize = Count * sizeof(DEVICE_ID);
        DeviceIdBuffer->Buffer = BlMmAllocateHeap(DeviceIdBuffer->BufferSize);
        if (DeviceIdBuffer->Buffer == NULL) {
            return STATUS_NO_MEMORY;
        }
    }

    //
    // The handles returned to the caller must be from the device table and
    // not the cached table.  Walk the Block I/O Device Table once more,
    // opening each RAM disk device, returning to the caller the resulting
    // device ID from the device table.
    //

    Count = 0;
    for (Id = 0; Id < BlockIoDeviceTableEntries; Id += 1) {
        DeviceEntry = BlockIoDeviceTable[Id];
        if (DeviceEntry == NULL) {
            continue;
        }

        Identifier = DeviceEntry->DeviceIdentifier;
        if ((Identifier->DeviceType != DEVICE_BLOCK_IO) ||
            (Identifier->u.BlockIo.BlockIoType != BLOCK_IO_RAMDISK)) {

            continue;
        }

        DeviceId = INVALID_DEVICE_ID;
        Status = BlDeviceOpen(Identifier, OPEN_READ, &DeviceId);

        ASSERT(Status == STATUS_SUCCESS);

        ASSERT(Count < (DeviceIdBuffer->BufferSize / sizeof(DEVICE_ID)));

        __analysis_assume(Count <
                          (DeviceIdBuffer->BufferSize / sizeof(DEVICE_ID)));

        ((PDEVICE_ID)DeviceIdBuffer->Buffer)[Count] = DeviceId;
        Count += 1;
    }

    DeviceIdBuffer->DataSize = Count * sizeof(DEVICE_ID);
    return STATUS_SUCCESS;
}

NTSTATUS
RdDeviceCreate (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION Options,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    )

/*++

Routine Description:

    This routine creates a ramdisk identifier based on the specified options.
    It looks for an existing ramdisk that matches the options, and if one does
    not exist, loads the specified file into memory as a ramdisk.

Arguments:

    Device - Supplies a pointer to a boot environment device that will be
        updated to refer to the ramdisk.

    CreationOptions - Supplies a pointer to an options list that contains
        options describing how the device should be created.

    CreatedDevice - Supplies a pointer to a variable that receives a heap
        allocated buffer containing the device identifier of the created
        device.

Return Value:

    STATUS_SUCCESS if the creation succeeds.
    STATUS_INVALID_PARAMETER if the device identifier and options are not
        sufficient to create the ramdisk.

    Another failure status code if loading the ramdisk fails.

--*/

{

    PRAMDISK_DEVICE CachedTableEntry;
    ULONG CachedTableId;
    PBOOT_ENVIRONMENT_DEVICE Identifier;
    RAMDISK_DEVICE RamdiskDevice;
    NTSTATUS Status;

    //
    // RAM disk creation is only supported via a path to a source file whose
    // contents should be loaded into the ramdisk.  Since the ramdisk identifier
    // is the image base and size, if the identifier is already specified the
    // ramdisk has already been created.  As a result only proceed if the
    // flag indicating the identifier is unspecified is set.
    //

    if ((Device->Flags & DEVICE_FLAG_IDENTIFIER_UNSPECIFIED) == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // RAM disk creation is costly since the size of the device is unbounded.
    // As prevention against creating the same RAM disk multiple times, this
    // module will cache the source paths for each RAM disk created by
    // this function.
    //
    // If this is the first call to this routine, initialize the global
    // cache table.
    //

    if (RdpCreationTable == NULL) {
        Status = RdpCreationTableInitialize();
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    //
    // Parse the options string for ramdisk specifics.
    //

    Status = RdpParseOptions(Device, Options, &RamdiskDevice);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Only initialize the RAM disk if this module has not already loaded
    // a device from the same source.
    //

    CachedTableEntry = BlTblFindEntry(RdpCreationTable,
                                      RdpCreationTableEntries,
                                      &CachedTableId,
                                      RdpCreationTableCompare,
                                      &RamdiskDevice,
                                      NULL,
                                      NULL,
                                      NULL);

    if (CachedTableEntry == NULL) {

        //
        // Create the RAM disk device by loading the specified file into
        // memory.
        //

        Status = RdpLoadImage(&RamdiskDevice);
        if (!NT_SUCCESS(Status)) {
            RdpFreeRamdiskDevice(&RamdiskDevice);
            return Status;
        }

        //
        // After the RAM disk has been loaded into memory, do not consider
        // any failure fatal.  Even when unable to cache the device descriptor,
        // the loaded image is valid as a RAM disk.
        //

        //
        // Allocate a device descriptor for the loaded RAM disk for the
        // device cache.
        //

        CachedTableEntry = BlMmAllocateHeap(sizeof(RAMDISK_DEVICE));
        if (CachedTableEntry != NULL) {

            //
            // Copy the RAM disk device description to the buffer for the
            // cached entry.  Note that this operation copies embedded
            // pointers allocatd by RdpParseOptions.  Ensure that
            // RdpFreeRamdiskDevice is not called for RamdiskDevice in this
            // case.
            //

            RtlCopyMemory(CachedTableEntry,
                          &RamdiskDevice,
                          sizeof(RAMDISK_DEVICE));

            //
            // Save an entry which describes the RAM disk just created.
            //

            BlTblSetEntry(&RdpCreationTable,
                          &RdpCreationTableEntries,
                          CachedTableEntry,
                          &CachedTableId,
                          TblDoNotPurgeEntry);

        }

        //
        // Create a new identifier describing the fully created device.
        //

        Status = RdpCreateDeviceIdentifier(Device,
                                           RamdiskDevice.Identifier,
                                           &Identifier);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        *CreatedDevice = Identifier;

        //
        // Populate the Block I/O Table with an entry for the ramdisk device
        // just created.  The table contains a single device entry for each
        // Block I/O Device processed by the library.  This allows for the
        // cached entries to be used and to avoid the duplication of work.
        //
        // N.B. If BlDeviceOpen is called after BlDeviceCreate, the cached
        //      entry will be used from the Block I/O Device Table and
        //      RdDeviceOpen will NOT be called.
        //

        RdpAddToBlockIoTable(Identifier);

        if (BlFveIsRegisteredForTrustedWimBoot()) {

            //
            // FVE is registered for trusted WIM boots. Notify FVE of the
            // calculated digest for the just-loaded WIM. Errors here are
            // ignored as they are not fatal to execution.
            //

            BlFveRegisterTrustedWimIntegrityInfo(
                            Identifier,
                            RamdiskDevice.IntegrityInfo.SourceFileHash,
                            RamdiskDevice.IntegrityInfo.SourceFileHashSize);
        }

    } else {

        //
        // Only free RamdiskDevice if the cached entry already contains
        // a description of the RAM disk device.
        //

        RdpFreeRamdiskDevice(&RamdiskDevice);

        //
        // Create a new identifier describing the fully created device.
        //

        Status = RdpCreateDeviceIdentifier(Device,
                                           CachedTableEntry->Identifier,
                                           CreatedDevice);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    return STATUS_SUCCESS;
}

// ---------------------------------------------------------- Utility Functions

NTSTATUS
RdpLoadImage (
    __inout PRAMDISK_DEVICE RamdiskDevice
    )

/*++

Routine Description:

    This routine loads a ramdisk image from a source file into memory.  When
    successful, this routine will populate the identifier field of the
    input parameter, RamdiskDevice.

Arguments:

    RamdiskDevice - Supplies a pointer to the internal data structure for
        describing a RAM disk device to load.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if a requested block is invalid.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

--*/

{

    ULONGLONG HeaderSize;
    PRAM_DISK_IDENTIFIER Identifier;
    PVOID ImageBase;
    ULONGLONG ImageSize;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONGLONG RamdiskLength;
    ULONG RamdiskOffset;
    ULONGLONG SourceSize;
    PUCHAR SourceFileHash;
    ULONG SourceFileHashSize;
    NTSTATUS Status;

    ImageBase = NULL;
    PhysicalAddress.QuadPart = 0;
    HeaderSize = 0;
    ImageSize = 0;
    Identifier = RamdiskDevice->Identifier;
    SourceFileHash = NULL;
    SourceFileHashSize = 0;

    //
    // If this is a TFTP ramdisk download, set the client port according to
    // the options. If TFTP port is not specified i.e. 0, then this will
    // reset the tftp port.
    //

    BlTftpSetFixedClientPort(RamdiskDevice->Options.TftpClientPort);

    //
    // If this is a TFTP ramdisk download, set the tftp block size. If block
    // size is not specified i.e. 0, then this will reset it to default.
    //

    BlTftpSetBlockSize(RamdiskDevice->Options.TftpBlockSize);

    //
    // Set the tftp window size for TFTP ramdisk download. If TFPT window size
    // is not specified, this will reset the tftp window size to default.
    //

    BlTftpSetWindowSize(RamdiskDevice->Options.TftpWindowSize);

    //
    // Enable (or disable) the tftp variable window size extension.
    //

    BlTftpSetVariableWindowEnabled(RamdiskDevice->Options.TftpVariableWindowEnabled);

    //
    // Set multicast policy.
    //

    BlMcSetPolicy((BOOLEAN)IS_RAMDISK_MULTICAST_ENABLED(RamdiskDevice),
                  (BOOLEAN)IS_RAMDISK_FALLBACK_ENABLED(RamdiskDevice));

    //
    // The device containing the RAM disk image might be encrypted. Try to
    // unlock it before reading from it. (This will do nothing if the device
    // is not encrypted.)
    //

    Status = RdpUnlockSourceDevice(&Identifier->Source.SourcePath);
    if (!NT_SUCCESS(Status)) {
        goto RdpLoadImageEnd;
    }

    //
    // Determine the size of the RAM disk image.
    //

    SourceSize = RdpQueryImageSize(&Identifier->Source.SourcePath);
    if (SourceSize == 0) {
        Status = STATUS_NO_SUCH_FILE;
        goto RdpLoadImageEnd;
    }

    //
    // A RAM disk can be a raw block I/O device or it can be a partition
    // or "blob" in a SDI.  Supported is the ability for a SDI header to
    // be loaded separate from the source file and loaded into memory in
    // front of a raw RAM disk image.  In this case, the SDI header is
    // specified in the options.  If this option is present, determine the
    // size of the SDI header.  Again, unlock the device if necessary.
    //

    if (RamdiskDevice->Options.SdiPath != NULL) {
        Status = RdpUnlockSourceDevice(RamdiskDevice->Options.SdiPath);
        if (!NT_SUCCESS(Status)) {
            goto RdpLoadImageEnd;
        }

        HeaderSize = RdpQueryImageSize(RamdiskDevice->Options.SdiPath);
        if (HeaderSize == 0) {
            Status = STATUS_NO_SUCH_FILE;
            goto RdpLoadImageEnd;
        }
    }

    //
    // Ensure the SDI header does not produce overflow.
    //

    if ((HeaderSize + SourceSize) < HeaderSize) { // check size overflow
        Status = STATUS_INVALID_PARAMETER;
        goto RdpLoadImageEnd;
    }

    //
    // The total image size is the sum of the sizes of the header file and the
    // source file.
    //

    ImageSize = HeaderSize + SourceSize;

    //
    // Allocate (and map one to one) a memory region large enough for both
    // the SDI header (if specified separate from the RAM disk image) and
    // the RAM disk image.
    //

    Status = BlImgAllocateImageBuffer(&ImageBase,
                                      ImageSize,
                                      MEMORY_TYPE_RAMDISK,
#if !defined(NTKERNEL)
                                      MEMORY_ATTRIBUTE_NONE,
#else
                                      MEMORY_ATTRIBUTE_ALLOCATION_REQUIRE_CONTIGUOUS,
#endif
                                      0,
#if !defined(NTKERNEL)
                                      IMAGE_FLAGS_MAP_ONE_TO_ONE);
#else
                                      IMAGE_FLAGS_NONE);
#endif

    if (!NT_SUCCESS(Status)) {
        goto RdpLoadImageEnd;
    }

    NT_VERIFY(BlMmTranslateVirtualAddress(ImageBase, &PhysicalAddress));

    //
    // Let multicast module know the path to image file. Multicast uses it to
    // create the namespace name.
    //

    Status = BlMcSetImageFilePath(BlGetFilePath(&Identifier->Source.SourcePath));
    if (!NT_SUCCESS(Status)) {
        goto RdpLoadImageEnd;
    }

    //
    // Load the RAM disk into memory.
    //

    if (HeaderSize != 0) {
        Status = RdpLoadFileToImage(RamdiskDevice->Options.SdiPath,
                                    ImageBase,
                                    HeaderSize,
                                    TRUE,
                                    FALSE,
                                    NULL,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RdpLoadImageEnd;
        }
    }

    Status = RdpLoadFileToImage(&Identifier->Source.SourcePath,
                                Add2Ptr(ImageBase, HeaderSize),
                                SourceSize,
                                TRUE,
                                BlFveIsRegisteredForTrustedWimBoot(),
                                &SourceFileHash,
                                &SourceFileHashSize);

    if (!NT_SUCCESS(Status)) {
        goto RdpLoadImageEnd;
    }

    //
    // Search the image for supported RAM disk formats.  If one is not found,
    // then the associated options for ramdisk image offset and length will be
    // used.
    //

    Status = RdpFindRamdiskRange(ImageBase,
                                 ImageSize,
                                 FIND_SDI_WIM_BLOB | FIND_SDI_PART_BLOB,
                                 &RamdiskOffset,
                                 &RamdiskLength);

    if (!NT_SUCCESS(Status)) {

        ASSERT(Status == STATUS_NOT_FOUND);

        RamdiskOffset = RamdiskDevice->Options.RamdiskImageOffset;
        RamdiskLength = RamdiskDevice->Options.RamdiskImageLength;
        RdpCorrectRamdiskRange(ImageSize, &RamdiskOffset, &RamdiskLength);
        Status = STATUS_SUCCESS;
    }

    //
    // Initialize the device identifier for the loaded RAM disk.
    //
    // N.B. The image size member of the RAM disk identifier is the total image
    //      size - including the RAM disk image offset.
    //


    Identifier->ImageBase = PhysicalAddress;
    Identifier->ImageOffset = RamdiskOffset;
    Identifier->ImageSize = RamdiskOffset + RamdiskLength;
    RamdiskDevice->IntegrityInfo.SourceFileHash = SourceFileHash;
    RamdiskDevice->IntegrityInfo.SourceFileHashSize = SourceFileHashSize;
    SourceFileHash = NULL;

RdpLoadImageEnd:
    if (!NT_SUCCESS(Status)) {

        //
        // If we experienced any errors, free all memory allocated for the
        // device.
        //

        if (ImageBase != NULL) {

            NT_ASSERT(PhysicalAddress.QuadPart != 0);

            BlMmUnmapVirtualAddress(ImageBase, ImageSize);
            BlMmFreePhysicalPages(PhysicalAddress);
        }
    }

    if (SourceFileHash != NULL) {
        BlMmFreeHeap(SourceFileHash);
    }

    //
    // Set the image file path to NULL to force disable multicast.
    //

    BlMcSetImageFilePath(NULL);
    return Status;
}

NTSTATUS
RdpUnlockSourceDevice (
    __in PUNALIGNED_FILE_PATH FilePath
    )

/*++

Routine Description:

    This routine attempts to unlock the source device indicated by the
    provided file path.

Arguments:

    FilePath - Supplies a pointer to the path of the ramdisk, which is used
        to extract the source device.

Return Value:

    STATUS_SUCCESS if the device was unlocked, or if it did not need to be
        unlocked.

    STATUS_FVE_LOCKED_VOLUME or other FVE errors if the volume needed to be
        unlocked but could not be.

    STATUS_NO_SUCH_FILE if a device could not be extracted from the supplied
        file path.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE SourceDevice;
    NTSTATUS Status;

    ASSERT(FilePath != NULL);

    SourceDevice = BlGetDevice(FilePath);

    //
    // Make sure a valid device was retrieved.
    //

    if (SourceDevice == NULL) {
        Status = STATUS_NO_SUCH_FILE;
        goto RdpUnlockSourceDeviceEnd;
    }

    //
    // Unlock the device.
    //

    Status = BlFveUnlockSinglePartition(SourceDevice);
    if (!NT_SUCCESS(Status)) {
        goto RdpUnlockSourceDeviceEnd;
    }

RdpUnlockSourceDeviceEnd:
    return Status;
}

ULONGLONG
RdpQueryImageSize (
    __in PUNALIGNED_FILE_PATH FilePath
    )

/*++

Routine Description:

    This routine queries the specified image file size.

Arguments:

    FilePath - Supplies a pointer to the file size of the file to be queried.

Return Value:

    Size of the file, or 0 if the file does not exist or the size could
    not be retrieved.

--*/

{

    OPEN_FLAGS DeviceFlags;
    DEVICE_ID DeviceId;
    FILE_ID FileId;
    FILE_INFORMATION Info;
    PWCHAR LoadFile;
    ULONGLONG Size;
    PBOOT_ENVIRONMENT_DEVICE SourceDevice;
    NTSTATUS Status;

    ASSERT(FilePath != NULL);

    SourceDevice = BlGetDevice(FilePath);
    LoadFile = BlGetFilePath(FilePath);
    DeviceId = INVALID_DEVICE_ID;
    FileId = INVALID_FILE_ID;
    Size = 0;

    //
    // Make sure we got back valid values from BlGetDevice and BlGetFilePath.
    //

    if ((SourceDevice == NULL) || (LoadFile == NULL)) {
        goto RdpQueryImageSizeEnd;
    }

    //
    // The boot device type is a distinguished value that indicates that the
    // boot device is to be opened, regardless of the specific boot device in
    // use on this particular boot.  Convert this sentinel identifier to a
    // real identifier.
    //

    if (SourceDevice->DeviceType == DEVICE_BOOT) {
        SourceDevice = BlGetBootDevice();
    }

    //
    // Open the source file to determine the size of an image.  If the
    // source device is a UDP device, open the device with read and write
    // access since packets must be sent in both directions.
    //

    if (SourceDevice->DeviceType == DEVICE_UDP) {
        DeviceFlags = OPEN_READ_WRITE;

    } else {
        DeviceFlags = OPEN_READ;
    }

    //
    // Open the device that contains the RAM disk image file.
    //

    Status = BlDeviceOpen(SourceDevice, DeviceFlags, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto RdpQueryImageSizeEnd;
    }

    //
    // Attempt to open the requested file.
    //

    Status = BlFileOpen(DeviceId, LoadFile, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto RdpQueryImageSizeEnd;
    }

    //
    // Query the file size.
    //

    Status = BlFileGetInformation(FileId, &Info);
    if (!NT_SUCCESS(Status)) {
        goto RdpQueryImageSizeEnd;
    }

    Size = Info.FileSize;

RdpQueryImageSizeEnd:

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    return Size;
}

NTSTATUS
RdpLoadFileToImage (
    __in PUNALIGNED_FILE_PATH FilePath,
    __in PVOID ImageBase,
    __in ULONGLONG ImageSize,
    __in BOOLEAN ShowProgress,
    __in BOOLEAN CalculateFileHash,
    __deref_opt_out_bcount_opt(*ImageHashSize) PUCHAR* ImageHash,
    __out_opt PULONG ImageHashSize
    )

/*++

Routine Description:

    Loads the specified file to memory located at ImageBase.

Arguments:

    FilePath - Supplies a pointer to a file path to the file to be loaded.

    ImageBase - Supplies the base address of the memory at which the file
        will be loaded.

    ImageSize - Supplies the image memory size.

    ShowProgress - Supplies a boolean indicating whether a progress bar
        should be shown.

    CalculateFileHash - Supplies a boolean indicating whether the file hash
        will be computed for this file.

    ImageHash - Supplies a pointer to a variable which receives the calculated
        file hash for this file. Only set if CalculateFileHash is not FALSE.
        The caller is expected to free the resulting buffer.

    ImageHashSize - Supplies a pointer to a variable which receives the size of
        the file hash for this file. Only set if CalculateFileHash is not
        FALSE.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.
    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.
    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

--*/

{

    DEVICE_ID DeviceId;
    PWCHAR LoadFile;
    size_t LoadFileLen;
    WCHAR ProgressInfo[76];
    size_t ProgressInfoLen;
    WCHAR ServerAddress[PXE_IP_ADDRESS_STRING_BUFFER_LENGTH];
    PBOOT_ENVIRONMENT_DEVICE SourceDevice;
    NTSTATUS Status;
    DEVICE_INFORMATION UdpInfo;

    ASSERT(FilePath != NULL);

    SourceDevice = BlGetDevice(FilePath);
    LoadFile = BlGetFilePath(FilePath);
    LoadFileLen = wcslen(LoadFile);
    ProgressInfoLen = sizeof(ProgressInfo) / sizeof(WCHAR);

    //
    // Make sure we got back valid values from BlGetDevice and BlGetFilePath.
    //

    if ((SourceDevice == NULL) || (LoadFile == NULL)) {
        return STATUS_NO_SUCH_FILE;
    }

    //
    // Zero out optional parameters if they are supplied. These will only be
    // used if CalculateFileHash is not FALSE.
    //

    if (ImageHash != NULL) {
        *ImageHash = NULL;
    }

    if (ImageHashSize != NULL) {
        *ImageHashSize = 0;
    }

    //
    // The boot device type is a distinguished value that indicates that the
    // boot device is to be opened, regardless of the specific boot device in
    // use on this particular boot.  Convert this sentinel identifier to a
    // real identifier.
    //

    if (SourceDevice->DeviceType == DEVICE_BOOT) {
        SourceDevice = BlGetBootDevice();
    }

    //
    // Check if the source device refers to a network device, if so, use TFTP
    // to transfer the image, otherwise, use image loading function to load
    // the image.
    //

    if (SourceDevice->DeviceType == DEVICE_UDP) {

        //
        // Open the UDP device. We need to open it as R/W since we will send
        // and receive UDP packets.
        //

        Status = BlDeviceOpen(SourceDevice, OPEN_READ_WRITE, &DeviceId);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Use TFTP to download the image file.
        //

        if (ShowProgress == TRUE) {

            //
            // TFTP is very slow, therefore we'll add some additional
            // context (IP address) for the progress indicator, so the
            // user can better understand the difference in load time.
            //

            Status = BlDeviceGetInformation(DeviceId, &UdpInfo);
            if (NT_SUCCESS(Status)) {
                BlNetAddressToString(ServerAddress,
                                     sizeof(ServerAddress) / sizeof(WCHAR),
                                     &UdpInfo.u.Udp.RemoteAddress);

                swprintf_s(ProgressInfo,
                           ProgressInfoLen,
                           L"IP: %s, File: ",
                           ServerAddress);

                if ((ProgressInfoLen - wcslen(ProgressInfo)) <= LoadFileLen) {
                    wcsncat_s(ProgressInfo,
                              ProgressInfoLen,
                              LoadFile,
                              ProgressInfoLen - wcslen(ProgressInfo) - 1);

                } else {
                    wcscat_s(ProgressInfo, ProgressInfoLen, LoadFile);
                }

                BlUtlUpdateProgressInfo(ProgressInfo);
            }
        }

        Status = BlNetLoadPreallocatedImageWithProgress(DeviceId,
                                                        LoadFile,
                                                        &ImageBase,
                                                        (PULONG)&ImageSize,
                                                        ShowProgress);

        BlDeviceClose(DeviceId);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    } else {

        //
        // Open the device that contains the RAM disk image file.
        //

        Status = BlDeviceOpen(SourceDevice, OPEN_READ, &DeviceId);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // Load the file into memory.
        //

        if (CalculateFileHash == FALSE) {
            Status = BlImgLoadPreallocatedImageWithProgress(DeviceId,
                                                            LoadFile,
                                                            &ImageBase,
                                                            (PULONG)&ImageSize,
                                                            ShowProgress);
        } else {
            Status = BlImgLoadImageWithProgress2(
                        DeviceId,
                        0,
                        LoadFile,
                        &ImageBase,
                        (PULONG)&ImageSize,
                        MEMORY_ATTRIBUTE_NONE,
                        0,
                        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED |
                         IMAGE_FLAGS_CALCULATE_IMAGE_HASH,
                        TRUE,
                        ImageHash,
                        ImageHashSize);
        }

        BlDeviceClose(DeviceId);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

#ifdef WANT_COMPRESSED_RAMDISK_SUPPORT

    //
    // Decompress the image if it is compressed.
    //

    if (RdpIsCompressed(ImageBase, ImageSize)) {
        Status = RdpDecompressImage(&ImageBase, &ImageSize);
    }

#endif // WANT_COMPRESSED_RAMDISK_SUPPORT

    return Status;
}

NTSTATUS
RdpCreateDeviceIdentifier (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PRAM_DISK_IDENTIFIER RamdiskIdentifier,
    __out PBOOT_ENVIRONMENT_DEVICE *CreatedDevice
    )

/*++

Routine Description:

    This routine creates a device identifier for a newly-created RAM disk.

Arguments:

    Device - Supplies a pointer to the input device provided to device creation.

    RamdiskIdentifier - Supplies a pointer to the RAM disk identifier
        constructed as part of the creation process.

    CreatedDevice - Supplies a pointer to a variable that receives the newly
        allocated identifier.

Return Value:

    STATUS_SUCCESS if the identifier is constructed successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    PUNALIGNED_FILE_PATH FilePath;
    PBOOT_ENVIRONMENT_DEVICE Identifier;
    ULONG RamdiskIdentifierSize;
    ULONG Size;

    //
    // Create a device identifier for the newly constructed RAM disk.
    //

    FilePath = &RamdiskIdentifier->Source.SourcePath;
    RamdiskIdentifierSize =
        FIELD_OFFSET(RAM_DISK_IDENTIFIER, Source.SourcePath) +
        FilePath->Length;

    Size = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo.u.RamDisk) +
           RamdiskIdentifierSize;

    Identifier = BlMmAllocateHeap(Size);
    if (Identifier == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Copy the header of the identifier from the input identifier, and
    // copy the body from the information constructed by this routine.
    //

    RtlCopyMemory(Identifier,
                  Device,
                  FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo.u.RamDisk));

    CLEAR_FLAGS(Identifier->Flags, DEVICE_FLAG_IDENTIFIER_UNSPECIFIED);

    RtlCopyMemory(&Identifier->u.BlockIo.u.RamDisk,
                  RamdiskIdentifier,
                  RamdiskIdentifierSize);

    *CreatedDevice = Identifier;
    return STATUS_SUCCESS;
}

NTSTATUS
RdpAddToBlockIoTable (
    __in PBOOT_ENVIRONMENT_DEVICE Device
    )

/*++

Routine Description:

    This routine creates and initializes a device entry for the Block I/O
    Device Table.

Arguments:

    Device - Supplies a pointer to a device identifier for the ramdisk device
        to add to the Block I/O Device Table.

Return Value:

    STATUS_SUCCESS if the operation completes successfully.
    STATUS_NO_MEMORY if there is a memory allocation failure.
    Another failure status code if adding the entry to the table fails.

--*/

{

    PBLOCK_IO_DEVICE BlockIoDevice;
    PDEVICE_ENTRY DeviceEntry;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    ULONG Id;
    NTSTATUS Status;

    ASSERT(Device->DeviceType == DEVICE_BLOCK_IO);
    ASSERT(Device->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK);

    DeviceEntry = NULL;
    DeviceIdentifier = NULL;
    BlockIoDevice = NULL;

    //
    // Allocate memory for the ramdisk's device entry.
    //

    DeviceEntry = BlMmAllocateHeap(sizeof(DEVICE_ENTRY));
    if (DeviceEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RdpAddToBlockIoTableEnd;
    }

    RtlZeroMemory(DeviceEntry, sizeof(DEVICE_ENTRY));

    //
    // A device entry contains two embedded pointers, one for the device
    // identifier and one containing device information (both internal and
    // external).
    //

    DeviceEntry->DeviceIdentifier = BlMmAllocateHeap(Device->Size);
    if (DeviceEntry->DeviceIdentifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RdpAddToBlockIoTableEnd;
    }

    DeviceEntry->DeviceData = BlMmAllocateHeap(sizeof(BLOCK_IO_DEVICE));
    if (DeviceEntry->DeviceData == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RdpAddToBlockIoTableEnd;
    }

    RtlZeroMemory(DeviceEntry->DeviceData, sizeof(BLOCK_IO_DEVICE));

    //
    // Initialize the device entry.  The attributes and reference count fields
    // have no meaning in the Block I/O Device Table since the entries are
    // solely used to cache device data and not handle information.
    //

    RtlCopyMemory(DeviceEntry->DeviceIdentifier, Device, Device->Size);

    //
    // Initialize the function table.  There are ramdisk specific I/O routines
    // for read and write.  All other routines can be inherited from the
    // Block I/O Device function table.
    //

    RtlCopyMemory(&DeviceEntry->FunctionTable,
                  &BlockIoDeviceFunctionTable,
                  sizeof(DEVICE_FUNCTION_TABLE));

    DeviceEntry->FunctionTable.Read  = RdDeviceRead;
    DeviceEntry->FunctionTable.Write = RdDeviceWrite;

    //
    // Initialize the BlockIoDevice data structure for the ramdisk.
    //

    Status = RdpPopulateBlockIoDevice(Device, DeviceEntry->DeviceData);
    if (!NT_SUCCESS(Status)) {
        goto RdpAddToBlockIoTableEnd;
    }

    //
    // Insert the initialized device entry into the block I/O device table.
    //

    Status = BlTblSetEntry(&BlockIoDeviceTable,
                           &BlockIoDeviceTableEntries,
                           DeviceEntry,
                           &Id,
                           TblDoNotPurgeEntry);

RdpAddToBlockIoTableEnd:

    if (!NT_SUCCESS(Status)) {
        if (DeviceEntry != NULL) {
            if (DeviceEntry->DeviceIdentifier != NULL) {
                BlMmFreeHeap(DeviceEntry->DeviceIdentifier);
            }

            if (DeviceEntry->DeviceData != NULL) {
                BlMmFreeHeap(DeviceEntry->DeviceData);
            }

            BlMmFreeHeap(DeviceEntry);
        }
    }

    return Status;
}

NTSTATUS
RdpPopulateBlockIoDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PBLOCK_IO_DEVICE BlockIoDevice
    )

/*++

Routine Description:

    This routine fills in ramdisk specific information in the Block I/O
    Device structure of a device entry.

Arguments:

    Device - Device identifier for a ramdisk device.

    BlockIoDevice - Supplies a pointer to the Block I/O Device data structure
        to be populated for the specified ramdisk device.

Return Value:

    NT status code.

--*/

{

    PVOID Base;
    PVOID ImageBase;
    ULONG ImageOffset;
    ULONGLONG ImageSize;
    PRAM_DISK_IDENTIFIER RamdiskIdentifier;
    PDEVICE_RAMDISK_INFORMATION RamdiskInformation;
    ULONGLONG RamdiskLength;
    ULONG RamdiskOffset;
    ULONGLONG Size;
    NTSTATUS Status;

    RamdiskIdentifier = &Device->u.BlockIo.u.RamDisk;
    RamdiskInformation = &BlockIoDevice->Information.DeviceInformation.RamDisk;

    //
    // Extract the image base and size and offset from the device identifier.
    // Map the physical address provided to a virtual address before accessing
    // it.
    //

    ImageBase = (PVOID)(ULONG_PTR)RamdiskIdentifier->ImageBase.QuadPart;
    Status = BlMmMapPhysicalAddress(&ImageBase,
                                    RamdiskIdentifier->ImageBase,
                                    RamdiskIdentifier->ImageSize,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    ImageSize = RamdiskIdentifier->ImageSize;
    ImageOffset = RamdiskIdentifier->ImageOffset;

    //
    // Calculate the memory range within the image that will be available as a
    // RAM disk.
    //

    Base = Add2Ptr(ImageBase, ImageOffset);
    Size = ImageSize - ImageOffset;

    //
    // Initialize the block I/O information.
    //

    BlockIoDevice->Information.Type = BLOCK_IO_RAMDISK;
    BlockIoDevice->Information.DeviceFlags = BLOCK_IO_DEVICE_FLAGS_NONE;
    BlockIoDevice->Information.IoAccessFlags = BLOCK_IO_ACCESS_FLAGS_NONE;
    BlockIoDevice->Information.BlockSize = 1;
    BlockIoDevice->Information.Alignment = 1;
    BlockIoDevice->Information.PartitionType = RAW;
    BlockIoDevice->Information.LastBlock = Size - 1;

    //
    // Populate the RAM disk information structure.  This will contain the
    // format of the ramdisk image, as well as the offset and length of the
    // image range available as a ramdisk.
    //
    // N.B. A SDI WIM blob range cannot be used here since the Windows OS RAM
    //      disk driver cannot directly deal with a WIM blob.  A part blob must
    //      be sought.
    //

    Status = RdpFindRamdiskRange(ImageBase,
                                 ImageSize,
                                 FIND_SDI_PART_BLOB,
                                 &RamdiskOffset,
                                 &RamdiskLength);

    if (!NT_SUCCESS(Status)) {

        ASSERT(Status == STATUS_NOT_FOUND);

        RamdiskInformation->ImageType = RAMDISK_IMAGE_TYPE_RAW_DISK;
        RamdiskInformation->RawDisk.ImageOffset = ImageOffset;
        RamdiskInformation->RawDisk.ImageLength = ImageSize - ImageOffset;

    } else {
        RamdiskInformation->ImageType = RAMDISK_IMAGE_TYPE_SDI_PART;
        RamdiskInformation->SdiPart.ImageOffset = RamdiskOffset;
        RamdiskInformation->SdiPart.ImageLength = RamdiskLength;
    }

    BlockIoDevice->InternalData.u.RamDiskData.RamDiskBase = Base;
    return STATUS_SUCCESS;
}

NTSTATUS
RdpParseOptions (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in_opt PBOOT_ENTRY_OPTION Options,
    __out PRAMDISK_DEVICE RamdiskDevice
    )

/*++

Routine Description:

    Parses an options structure into a structure describing load options for
    the ramdisk.

Arguments:

    Device - Supplies a pointer to the device identifier for the ramdisk being
        loaded.  This contains the source path of the ramdisk.

    Options - Supplies a pointer to the options associated with the ramdisk.
        Supported options are:

        BCDE_DEVICE_TYPE_RAMDISK_SDI_DEVICE
        BCDE_DEVICE_TYPE_RAMDISK_SDI_PATH

            Indicates that the boot ramdisk has a SDI header that should be
            downloaded from the specified path.

        BCDE_DEVICE_TYPE_RAMDISK_IMAGE_OFFSET

            Specifies the offset to the image where the actual disk image
            starts.

        BCDE_DEVICE_TYPE_RAMDISK_TFTP_CLIENT_PORT

            Specifies the local UDP port to use when performing TFTP operation.

    RamdiskDevice - Supplies a pointer to a structure describing a RAM disk
        to load.  The data structure contains RAM disk options which get
        read and parsed by this routine.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if the requested device could not be found.

--*/

{

    BOOLEAN BoolValue;
    PUNALIGNED_FILE_PATH FilePath;
    PRAM_DISK_IDENTIFIER Identifier;
    ULONG PathSize;
    PBOOT_ENVIRONMENT_DEVICE SdiDevice;
    PWCHAR SdiPath;
    ULONG Size;
    NTSTATUS Status;
    ULONGLONG Value;

    Status = STATUS_SUCCESS;
    RtlZeroMemory(RamdiskDevice, sizeof(RAMDISK_DEVICE));
    SdiDevice = NULL;
    SdiPath = NULL;

    //
    // Copy the identifier for the ramdisk into the ramdisk device.  This
    // contains the source path of the disk.  Leave the image portions of the
    // identifier blank for now - only fill in the source path.
    //

    FilePath = &Device->u.BlockIo.u.RamDisk.Source.SourcePath;
    MARK_SAFE_ADDITION(FilePath->Length,
                       FIELD_OFFSET(RAM_DISK_IDENTIFIER, Source.SourcePath));

    Size = FIELD_OFFSET(RAM_DISK_IDENTIFIER, Source.SourcePath) +
           FilePath->Length;

    Identifier = BlMmAllocateHeap(Size);
    if (Identifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RdpParseOptionsEnd;
    }

    RtlZeroMemory(Identifier, Size);
    RtlCopyMemory(&Identifier->Source.SourcePath, FilePath, FilePath->Length);
    RamdiskDevice->Identifier = Identifier;

    //
    // A RAM disk requires a SDI header.  If one is not present in the source
    // RAM disk image, one can be specified separately in which case the
    // RAM disk will be loaded contiguously following the SDI header.
    //
    // The SDI header device and path are specified separately.  If the device
    // exists, the path must as well.
    //

    Status = BlGetBootOptionDevice(Options,
                                   BCDE_DEVICE_TYPE_RAMDISK_SDI_DEVICE,
                                   &SdiDevice,
                                   NULL);

    if (NT_SUCCESS(Status)) {
        Status = BlGetBootOptionString(Options,
                                       BCDE_DEVICE_TYPE_RAMDISK_SDI_PATH,
                                       &SdiPath);

        if (!NT_SUCCESS(Status)) {
            goto RdpParseOptionsEnd;
        }

        //
        // Construct a file path containing the device concatenated with the
        // path.
        //

        PathSize = (ULONG)(wcslen(SdiPath) * sizeof(WCHAR)) +
                   sizeof(UNICODE_NULL);

        Size = FIELD_OFFSET(FILE_PATH, FilePath) + SdiDevice->Size + PathSize;
        FilePath = BlMmAllocateHeap(Size);
        if (FilePath == NULL) {
            goto RdpParseOptionsEnd;
        }

        RamdiskDevice->Options.SdiPath = FilePath;
        FilePath->Version = 1;
        FilePath->Type = FILE_PATH_TYPE_BOOT_ENV;
        FilePath->Length = Size;
        RtlCopyMemory(FilePath->FilePath, SdiDevice, SdiDevice->Size);
        RtlCopyMemory(Add2Ptr(FilePath->FilePath, SdiDevice->Size),
                      SdiPath,
                      PathSize);
    }

    //
    // Determine the RAM disk image offset.
    //

    Status = BlGetBootOptionInteger(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_IMAGE_OFFSET,
                                    &Value);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.RamdiskImageOffset = (ULONG)Value;
    }

    //
    // Determine the RAM disk image length.
    //

    Status = BlGetBootOptionInteger(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_IMAGE_LENGTH,
                                    &Value);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.RamdiskImageLength = Value;
    }

    //
    // Determine the UDP port to use when doing TFTP.
    //

    Status = BlGetBootOptionInteger(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_TFTP_CLIENT_PORT,
                                    &Value);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.TftpClientPort = (USHORT)Value;
    }

    //
    // Determine the Tftp block size to use when doing TFTP.
    //

    Status = BlGetBootOptionInteger(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_TFTP_BLOCK_SIZE,
                                    &Value);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.TftpBlockSize = (ULONG)Value;
    }

    //
    // Determine the Tftp window  size to use when doing TFTP.
    //

    Status = BlGetBootOptionInteger(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_TFTP_WINDOW_SIZE,
                                    &Value);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.TftpWindowSize = (ULONG)Value;
    }

    //
    // Determine if tftp variable window size extension is enabled.
    //

    Status = BlGetBootOptionBoolean(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_TFTP_VAR_WINDOW,
                                    &BoolValue);

    if (NT_SUCCESS(Status)) {
        RamdiskDevice->Options.TftpVariableWindowEnabled = BoolValue;
    }

    //
    // Determine if multicast is enabled.
    //

    Status = BlGetBootOptionBoolean(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_MULTICAST_ENABLED,
                                    &BoolValue);

    if (NT_SUCCESS(Status) && (BoolValue != FALSE)) {
        SET_FLAGS(RamdiskDevice->Options.MulticastOptions,
                    RAMDISK_ENABLE_MULTICAST);
    }

    //
    // Determine if client should fallback to tftp if multicast fails.
    //

    Status = BlGetBootOptionBoolean(Options,
                                    BCDE_DEVICE_TYPE_RAMDISK_MULTICAST_TFTP_FALLBACK,
                                    &BoolValue);

    if (NT_SUCCESS(Status)) {
        if (BoolValue != FALSE) {
            SET_FLAGS(RamdiskDevice->Options.MulticastOptions,
                        RAMDISK_ENABLE_FALLBACK);
        }

    } else {
        SET_FLAGS(RamdiskDevice->Options.MulticastOptions,
                    RAMDISK_ENABLE_FALLBACK);
    }

    Status = STATUS_SUCCESS;

RdpParseOptionsEnd:
    if (SdiDevice != NULL) {
        BlMmFreeHeap(SdiDevice);
    }

    if (SdiPath != NULL) {
        BlMmFreeHeap(SdiPath);
    }

    if (!NT_SUCCESS(Status)) {
        RdpFreeRamdiskDevice(RamdiskDevice);
    }

    return Status;
}

NTSTATUS
RdpFindRamdiskRange (
    __in PVOID ImageBase,
    __in ULONGLONG ImageSize,
    __in ULONG Flags,
    __out PULONG RamdiskOffset,
    __out PULONGLONG RamdiskLength
    )

/*++

Routine Description:

    This routine will search the specified image for a ramdisk of the specified
    type.

Arguments:

    ImageBase - Supplies a pointer to the start of the loaded image.

    ImageSize - Supplies the size in bytes of the loaded image.

    Flags - Supplies a ramdisk image type to be searched for.  This can be a
        combination of one or more of the following:

            FIND_SDI_WIM_BLOB - Will will search for a SDI WIM blob.

            FIND_SDI_PART_BLOB - This will search for a SDI PART blob.

        If both flags are specified, and and both types of ramdisk image exist
        in the loaded image, then the WIM blob will be returned.

    RamdiskOffset - Supplies a location in which the offset to the found RAM
        disk range is returned.

    RamdiskLength - Supplies a location in which the length of the found RAM
        disk range is returned.  This length excludes the RAM disk offset.

Return Value:

    STATUS_NOT_FOUND if the specified subimage cannot be found.

    STATUS_SUCCESS otherwise.

--*/

{

    PVOID Base;
    ULONGLONG Length;
    ULONG Offset;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    Length = 0;
    Offset = 0;

    //
    // Determine whether the loaded image is a SDI image.  If it is, then
    // search for any requested SDI RAM disk images.
    //

    if (SdiIsSdiImage(ImageBase, ImageSize) != FALSE) {
        if (CHECK_FLAG(Flags, FIND_SDI_WIM_BLOB)) {
            Base = SdiFindBlob((PSDI_HEADER)ImageBase,
                               SDI_BLOBTYPE_WIM,
                               &Length,
                               NULL);

            if (Base != NULL) {
                Offset = (ULONG)PtrOffset(ImageBase, Base);
                goto FindRamdiskRangeEnd;
            }
        }

        if (CHECK_FLAG(Flags, FIND_SDI_PART_BLOB)) {
            Base = SdiFindBlob((PSDI_HEADER)ImageBase,
                                 SDI_BLOBTYPE_PART,
                                 &Length,
                                 NULL);

            if (Base != NULL) {
                Offset = (ULONG)PtrOffset(ImageBase, Base);
                goto FindRamdiskRangeEnd;
            }
        }
    }

    Status = STATUS_NOT_FOUND;

FindRamdiskRangeEnd:

    //
    // If the requested range has been found, then ensure that it falls
    // completely within the loaded image.  Also, the SDI header may contain
    // zero as the length for the range.  If this is the case, make it the
    // maximum possible.
    //

    if (NT_SUCCESS(Status)) {
        *RamdiskOffset = Offset;
        *RamdiskLength = Length;
        RdpCorrectRamdiskRange(ImageSize, RamdiskOffset, RamdiskLength);
    }

    return Status;
}

VOID
RdpCorrectRamdiskRange (
    __in ULONGLONG ImageSize,
    __inout PULONG RamdiskOffset,
    __inout PULONGLONG RamdiskLength
    )

/*++

Routine Description:

    This function will ensure that the supplied ramdisk range is completely
    contained within the supplied image size.  If it is not, the offset and
    length will be modified so that it is.

Arguments:

    ImageSize - Supplies the loaded image size.

    RamdiskOffset - Supplies the offset of the range to be corrected.

    RamdiskLength - Supplies the the length of the range to be corrected.  This
        value excludes the offset.

Return Value:

    None.

--*/

{

    if (*RamdiskOffset >= ImageSize) {
        *RamdiskOffset = 0;
    }

    if ((*RamdiskLength == 0) ||
        ((*RamdiskOffset + *RamdiskLength) > ImageSize)) {

        *RamdiskLength = ImageSize - *RamdiskOffset;
    }
}


NTSTATUS
RdpFreeRamdiskDevice (
    __in PRAMDISK_DEVICE RamdiskDevice
    )

/*++

Routine Description:

    Frees all memory allocated for a loaded RAM disk.

Arguments:

    DeviceEntry - pointer to a device entry structure.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    ASSERT(RamdiskDevice != NULL);

    if (RamdiskDevice->Identifier != NULL) {
        BlMmFreeHeap(RamdiskDevice->Identifier);
        RamdiskDevice->Identifier = NULL;
    }

    if (RamdiskDevice->Options.SdiPath != NULL) {
        BlMmFreeHeap(RamdiskDevice->Options.SdiPath);
        RamdiskDevice->Options.SdiPath = NULL;
    }

    if (RamdiskDevice->IntegrityInfo.SourceFileHash != NULL) {
        BlMmFreeHeap(RamdiskDevice->IntegrityInfo.SourceFileHash);
        RtlZeroMemory(&RamdiskDevice->IntegrityInfo,
                      sizeof(RamdiskDevice->IntegrityInfo));
    }

    return STATUS_SUCCESS;
}

#ifdef WANT_COMPRESSED_RAMDISK_SUPPORT

// -------------------------------------------------------- Decompress routines

/*

    These routines would be used if we wanted to support loading compressed
    RAM disk files from a source other than TFTP.  TFTP has its own
    implementation that decompresses in place as the RAM disk is downloaded.

*/

BOOLEAN
RdpIsCompressed (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONGLONG ImageSize
    )

/*++

Routine Description:

    Determines if the ram disk image is compressed

Arguments:

    ImageBase - pointer to the ram disk image

    ImageSize - size of the ram disk image

Return Value:

    TRUE if the image is compressed
    FALSE otherwise

--*/

{

    PBMBUILD_COMPRESSED_HEADER CompressedHeader;

    CompressedHeader = (PBMBUILD_COMPRESSED_HEADER)ImageBase;

    //
    // A disk image normally has 0x55 0xAA as the last two bytes
    // in the first block (512 bytes).   A compressed ramdisk
    // has a signature field in the last 4 bytes which does NOT
    // include 0x55 0xAA in its last two bytes.
    //

    if ((ImageSize > sizeof(BMBUILD_COMPRESSED_HEADER)) &&
        (CompressedHeader->Signature == BMBUILD_COMPRESSED_SIGNATURE) &&

        //
        // Note: As only one version and compression format is currently
        // supported, assume the image is not a ramdisk if the header
        // doesn't match the expected values.   In a possible future
        // different versions may be handled.
        //

        (CompressedHeader->Version == BMBUILD_COMPRESSED_VERSION) &&
        (CompressedHeader->CompressionFormat == BMBUILD_COMPRESSED_FMT_XPRESS)) {

        return TRUE;
    }

    return FALSE;
}

NTSTATUS
RdpDecompressImage (
    __inout_bcount(*ImageSize) PVOID *ImageBase,
    __inout PULONGLONG ImageSize
    )

/*++

Routine Description:

    This routine decompresses an image.  It allocates enough contiguous
    memory for the uncompressed image, decompresses the image, deallocates
    the compressed image and updates the image base and size to refer to
    the uncompressed location.

Arguments:

    ImageBase - Supplies a pointer to a variable that receives the base
        of the decompressed image.

    ImageSize - Supplies a pointer to a variable that recieves the size
        of the decompressed image.

Return Value:

    STATUS_SUCCESS if the decompression occurred successfully.
    STATUS_FILE_INVALID if the ramdisk contains a corrupt compression format.
    Another failure status code if there was a memory allocation failure.

--*/

{

    PBMBUILD_COMPRESSED_BLOCK Block;
    ULONG BlockSize;
    ULONG CheckSum;
    PBMBUILD_COMPRESSED_HEADER CompressedHeader;
    ULONG DecodedSize;
    PUCHAR Done;
    PVOID NewImageBase;
    ULONG NewImageSize;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PUCHAR Target;
    ULONG UncompressedSize;

    CheckSum = 0;
    NewImageBase = NULL;
    CompressedHeader = (PBMBUILD_COMPRESSED_HEADER)*ImageBase;

    //
    // It seems rational to expect that the uncompressed size
    // of the image is less than 4GB,... this code will require
    // work if that assumption changes.
    //

    ASSERT((CompressedHeader->UncompressedSize & 0xFFFFFFFF) ==
            CompressedHeader->UncompressedSize);
    ASSERT(CompressedHeader->UncompressedSize >=
           CompressedHeader->BlockSize);

    //
    // Get local copies of the low 32 bits of the uncompressed
    // image size and the block size.
    //

    UncompressedSize = (ULONG)CompressedHeader->UncompressedSize;
    BlockSize = (ULONG)CompressedHeader->BlockSize;

    //
    // Block size is either a multiple of page size or exactly
    // equal to the uncompressed image size in the case that
    // the image is not blocked.
    //

    ASSERT(((BlockSize & (PAGE_SIZE-1)) == 0) ||
            (BlockSize == UncompressedSize));

    //
    // Allocate enough physical pages to hold the entire file
    //

    NewImageSize = (ULONG)ALIGN_RANGE_UP(CompressedHeader->UncompressedSize, PAGE_SIZE);

    ASSERT(NewImageSize >= ALIGN_RANGE_UP(*ImageSize, PAGE_SIZE));

    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       NewImageSize >> PAGE_SHIFT,
                                       MemoryXIPRom,
                                       MEMORY_ATTRIBUTE_NONE,
                                       0);             // no alignment

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Create a one to one mapping for the new image location
    //

    NewImageBase = (PVOID)(UINTN)PhysicalAddress.QuadPart;
    Status = BlMmMapPhysicalAddress(&NewImageBase,
                                    PhysicalAddress,
                                    NewImageSize,
                                    MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

    if (!NT_SUCCESS(Status)) {
        goto RdpDecompressImageEnd;
    }

    //
    // Uncompress the image.
    //

    Target = NewImageBase;
    Block = (PBMBUILD_COMPRESSED_BLOCK)(CompressedHeader + 1);
    Done = Target + UncompressedSize;
    do {
        if ((Block->UncompressedSize < Block->CompressedSize) ||
            ((Block->UncompressedSize != BlockSize) &&
             (Block->UncompressedSize + Target != Done))) {

            //
            // This can't happen, the image is corrupt.
            //

            ASSERT(Block->UncompressedSize < Block->CompressedSize);
            ASSERT((Block->UncompressedSize == BlockSize) ||
                   (Block->UncompressedSize + Target == Done));

            Status = STATUS_FILE_INVALID;
            goto RdpDecompressImageEnd;
        }

        //
        // Checksum the compressed block and compare to the
        // running checksum.  If they don't match, abort.
        //

        CheckSum = RdpXpressCheckSum(CheckSum,
                                     Block + 1,
                                     Block->CompressedSize);

        if (CheckSum != Block->CheckSum) {

            ASSERT(CheckSum == Block->CheckSum);

            Status = STATUS_FILE_INVALID;
            goto RdpDecompressImageEnd;
        }

        //
        // Decompress block to target location.
        //

        if (Block->UncompressedSize == Block->CompressedSize) {

            //
            // This block was not compressed, just copy it.
            //

            RtlCopyMemory(Target, Block+1, Block->CompressedSize);

        } else {


            DecodedSize = XpressDecode(NULL,
                                       Target,
                                       Block->UncompressedSize,
                                       Block->UncompressedSize,
                                       Block+1,
                                       Block->CompressedSize);

            ASSERT(DecodedSize == Block->UncompressedSize);
        }

        //
        // Update the target and source addresses.  The source will
        // be at the next 8 byte boundary beyond the compressed data
        // just processed.
        //

        Target += Block->UncompressedSize;
        Block = (PBMBUILD_COMPRESSED_BLOCK)
                ROUND_UP(((ULONG_PTR)(Block+1) + Block->CompressedSize),
                         BMBUILD_COMPRESSED_BLOCK_PAD);

    } while (Target < Done);

    ASSERT(Target == Done);

    //
    // Free compressed image.
    //

    BlMmFreeHeap(*ImageBase);

    //
    // Return the location of the uncompressed image.
    //

    *ImageBase = NewImageBase;
    *ImageSize = NewImageSize;

RdpDecompressImageEnd:

    if (!NT_SUCCESS(Status)) {
        if (NewImageBase != NULL) {
            BlMmFreeHeap(NewImageBase);
        }
    }

    return Status;
}

ULONG
RdpXpressCheckSum (
    __in ULONG PartialSum,
    __in_bcount(Length) PVOID Source,
    __in ULONG Length
    )

/*++

Routine Description:

    Calculate the 16 bit checksum of the input data.

    It is expected that there are less than 64K bytes (32K USHORTS)
    of data in the source.  Summing two maximal 16 bit numbers can
    result in a carry bit.  By accumulating the partial sum in a
    32 bit integer we can allow the carry bits to accumulate, the
    value of the carry cannot exceed the number of input elements,
    that is, the carry cannot exceed 32K which fits within the
    upper 15 bits of the partial sum.   At the end of summation we
    fold the upper 15 bits (carry) into the partial sum.

    This doesn't work if there is more than 64K bytes of input. However,
    as long as the same checksum routine is used in both the source
    and destination, it really doesn't matter.  In this application
    the input data will never exceed 64K.

    Note:  The source is processed from high address to low address
    so that the last accessed data is in the cache when done.  This
    data is about to be processed by other code.

    Note:  As long as there is less than 64K of input this algorithm
    is the same as tcpxsum().

Arguments:

    PartialSum  Supplies the starting value.  This allows calculation
                of a fragmented data set.
    Source      Is the address if the data to be checksummed.  Source
                is expected to be USHORT aligned.
    Length      Is the length of the data in bytes.

Return Value:

    Returns the 16 bit checksum of the source.

--*/

{

    PUSHORT pusSource;

    ASSERT(Length <= 0x10000);
    ASSERT(((ULONG_PTR)Source & 1) == 0);
    ASSERT((PartialSum & 0xFFFF) == PartialSum);

    //
    // If there is an odd number of bytes, handle the last byte.
    //

    if ((Length & 1) != 0) {
        PartialSum += *((PUCHAR)Source + Length - 1);
        Length--;
    }

    //
    // Handle the rest of the source, in reverse order, as USHORTs.
    //

    pusSource = (PUSHORT)((ULONG_PTR)Source + Length);
    Length = Length / 2;

    //
    // Sum the USHORTs in the source.
    //

    while (Length--) {
        PartialSum += *--pusSource;
    }

    ASSERT((PVOID)pusSource == Source);

    //
    // Fold in any carry.  Because we accumulate the carry, folding it
    // in can again cause a carry which needs to be folded in.
    //
    // Remember that we expect to be called for no more than 64K of
    // data, ie 32K of USHORTs, given the maximum value of a USHORT
    // is 0xFFFF, so the sum of two of them is, maximally, 0x1FFFE.
    // Therefore the upper 16 bits of the PartialSum can grow to
    // 16K - 1, ie 0x7FFF.
    //
    // Adding 0x7FFF to the maximum possible low 16 bits 0xFFFF gives
    // 0x17FFE, and the 1 (carry) needs to be folded in two.
    //
    // This is still considerably cheaper than individualy folding in
    // the carry for every USHORT processed.
    //

    PartialSum = (PartialSum >> 16) + (PartialSum & 0xFFFF);
    ASSERT((PartialSum >> 16) < 2);
    PartialSum = (PartialSum >> 16) + (PartialSum & 0xFFFF);

    return PartialSum;
}

#endif // WANT_COMPRESSED_RAMDISK_SUPPORT

// ---------------------------------------------------- Creation Table Routines

NTSTATUS
RdpCreationTableInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the global data structures used for the RAM disk creation
    table.  RAM disk creation can be a costly operation since the size of the
    device is unbounded.  As prevention against creating the same RAM disk
    twice, this module will cache the source paths for each RAM disk created
    by RdDeviceCreate.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if the allocation for the creation table fails.
    STATUS_NO_MEMORY if the registration of a destruction routine fails.

--*/

{

    NTSTATUS Status;
    ULONG TableSize;

    RdpCreationTableEntries = INITIAL_TABLE_ENTRIES;
    TableSize = RdpCreationTableEntries * sizeof(PRAMDISK_DEVICE);

    //
    // Allocate the initial creation table.
    //

    RdpCreationTable = BlMmAllocateHeap(TableSize);
    if (RdpCreationTable == NULL) {
        return STATUS_NO_MEMORY;
    }

    RtlZeroMemory(RdpCreationTable, TableSize);

    //
    // Register a destroy routine with the I/O manager to provide a mechanism
    // for the global data structures to be reinitialized during destruction.
    // The reinitialization is required for the ability to reinitialize the
    // Boot Environment Library subsequently by the same application.
    //

    Status = BlpIoRegisterDestroyRoutine(RdpCreationTableDestroy);
    return Status;
}

NTSTATUS
RdpCreationTableDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys and reinitializes all global data structures used to cache
    the RAM disk devices created by RdDeviceCreate.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    NTSTATUS Status;

    ASSERT(RdpCreationTable != NULL);
    ASSERT(RdpCreationTableEntries != 0);

    //
    // Free each entry in the global table.
    //

    Status = BlTblMap(RdpCreationTable,
                      RdpCreationTableEntries,
                      RdpCreationTableDestroyEntry,
                      FALSE);

    ASSERT(Status == STATUS_SUCCESS);

    BlMmFreeHeap(RdpCreationTable);

    //
    // Reinitialize the values of the global variables.
    //

    RdpCreationTable = NULL;
    RdpCreationTableEntries = 0;
    return Status;
}

BOOLEAN
RdpCreationTableCompare (
    __in PRAMDISK_DEVICE TableEntry,
    __in PRAMDISK_DEVICE RamdiskDevice,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    )

/*++

Routine Description:

    Determines if the specified entry in the cached creation table refers
    to the same RAM disk device as the specified parameter.

    Two RAM disks are defined to be the same if they have the same source.

Arguments:

    TableEntry - An entry in the global, RdpCreationTable.

    RamdiskOptions - An internal data structure used to describe a loaded
        RAM disk.

Return Value:

    TRUE if TableEntry and RamdiskOptions have the same source.
    FALSE otherwise.

--*/

{

    UNREFERENCED_PARAMETER(Arg2);
    UNREFERENCED_PARAMETER(Arg3);
    UNREFERENCED_PARAMETER(Arg4);

    ASSERT(TableEntry != NULL);
    ASSERT(TableEntry->Identifier != NULL);
    ASSERT(RamdiskDevice != NULL);
    ASSERT(RamdiskDevice->Identifier != NULL);

    //
    // Compare the path to the RAM disk to see if they describe the same
    // source.
    //

    if (TableEntry->Identifier->Source.SourcePath.Length !=
        RamdiskDevice->Identifier->Source.SourcePath.Length) {

        return FALSE;
    }

    if (RtlEqualMemory(
            &TableEntry->Identifier->Source.SourcePath,
            &RamdiskDevice->Identifier->Source.SourcePath,
            TableEntry->Identifier->Source.SourcePath.Length) == FALSE) {

        return FALSE;
    }

    return TRUE;
}

NTSTATUS
RdpCreationTableDestroyEntry (
    __in PRAMDISK_DEVICE TableEntry,
    __in ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the RAM disk creation table.

Arguments:

    TableEntry - Pointer to an entry in the RAM disk creation table.

    Index - Index in the creation table.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    //
    // Free all memory allocated for the current entry.
    //

    RdpFreeRamdiskDevice(TableEntry);
    BlMmFreeHeap(TableEntry);
    RdpCreationTable[Index] = NULL;
    return STATUS_SUCCESS;
}


