/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    disk.h

Abstract:

    Header file for disk i/o implementation.

Environment:

    Boot

--*/

#pragma once

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include "device.h"
#include "block.h"

// ----------------------------------------------------------------- Prototypes

//
// Routines implemented in this module.  They are used to patch some of the
// block i/o routines in the block i/o function table.
//

NTSTATUS
DiskOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __out PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
DiskClose (
    __in PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
DiskEnumerateDeviceClass (
    __in PBOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier,
    __inout PGENERIC_BUFFER DeviceIdBuffer,
    __in BOOLEAN AllocateNewBuffer
    );

// -------------------------------------------------------- Firmware Prototypes

NTSTATUS
DiskFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    );

NTSTATUS
DiskFirmwareClose (
    __in PBLOCK_IO_DEVICE BlockIoDevice
    );

