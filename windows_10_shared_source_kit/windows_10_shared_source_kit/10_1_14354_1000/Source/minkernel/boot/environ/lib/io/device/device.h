/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    device.h

Abstract:

    Header file for boot environment device i/o.

    When device's are opened, an entry is created in a device table.  After
    this, all device i/o calls are directed based on function pointers set
    during the specific devices open routine.

    This header is to be included by all device implementation of i/o (ie.
    console, disk, etc).  It contains definitions for the routines each
    needs to implement and the data structures used in each.

Environment:

    Boot.

Revision History:

--*/

#pragma once

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ------------------------------------------------- Data Structure Definitions

typedef ULONG DEVICE_ATTRIBUTES;
typedef DEVICE_ATTRIBUTES *PDEVICE_ATTRIBUTES;

#define DEVICE_OPEN         (0x01)
#define DEVICE_READ         (0x02)
#define DEVICE_WRITE        (0x04)
#define DEVICE_NO_FVE       (0x08)
#define DEVICE_OPEN_BY_HASH (0x10)
#define DEVICE_NT_DEVICE    (0x20)

// --------------------------------------------------------------------- Macros

#define SET_DEVICE_OPEN(_x)            ((DEVICE_ATTRIBUTES)_x |= DEVICE_OPEN)
#define SET_DEVICE_READ(_x)            ((DEVICE_ATTRIBUTES)_x |= DEVICE_READ)
#define SET_DEVICE_WRITE(_x)           ((DEVICE_ATTRIBUTES)_x |= DEVICE_WRITE)
#define SET_DEVICE_NO_FVE(_x)          ((DEVICE_ATTRIBUTES)_x |= DEVICE_NO_FVE)
#define SET_DEVICE_OPEN_BY_HASH(_x)    ((DEVICE_ATTRIBUTES)_x |= DEVICE_OPEN_BY_HASH)
#define SET_DEVICE_NT_DEVICE(_x)       ((DEVICE_ATTRIBUTES)_x |= DEVICE_NT_DEVICE)

#define CLEAR_DEVICE_OPEN(_x)          ((DEVICE_ATTRIBUTES)_x &= ~DEVICE_OPEN)
#define CLEAR_DEVICE_ATTRIBUTES(_x)    ((DEVICE_ATTRIBUTES)_x = 0)

#define IS_DEVICE_OPENED(_x)           ((DEVICE_ATTRIBUTES)_x & DEVICE_OPEN)
#define IS_DEVICE_READ_SET(_x)         ((DEVICE_ATTRIBUTES)_x & DEVICE_READ)
#define IS_DEVICE_WRITE_SET(_x)        ((DEVICE_ATTRIBUTES)_x & DEVICE_WRITE)
#define IS_DEVICE_NO_FVE_SET(_x)       ((DEVICE_ATTRIBUTES)_x & DEVICE_NO_FVE)
#define IS_DEVICE_OPEN_BY_HASH_SET(_x) ((DEVICE_ATTRIBUTES)_x & DEVICE_OPEN_BY_HASH)
#define IS_DEVICE_NT_DEVICE(_x)        ((DEVICE_ATTRIBUTES)_x & DEVICE_NT_DEVICE)
#define IS_DEVICE_OPENED_FOR_READ(_x)  (IS_DEVICE_OPENED(_x) && \
                                        IS_DEVICE_READ_SET(_x))
#define IS_DEVICE_OPENED_FOR_WRITE(_x) (IS_DEVICE_OPENED(_x) && \
                                        IS_DEVICE_WRITE_SET(_x))

// ------------------------------------------------------------ Data Structures

typedef enum _OVERLAY_STATE {
    OverlayOpened,
    OverlayClosed,
    OverlayError
} OVERLAY_STATE;

typedef struct _OVERLAY_ENTRY {
    LARGE_INTEGER DataSourceId;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    OVERLAY_STATE State;
    DEVICE_ID DeviceId;
} OVERLAY_ENTRY, *POVERLAY_ENTRY;

typedef struct _DEVICE_OVERLAY {
    ULONG OverlayCount;
    POVERLAY_CONFIG OverlayConfig;
    OVERLAY_ENTRY Entry[ANYSIZE_ARRAY];
} DEVICE_OVERLAY, *PDEVICE_OVERLAY;

typedef struct _OVERLAY_INFORMATION {
    DEVICE_ID DeviceId;
    LARGE_INTEGER DataSourceId;
} OVERLAY_INFORMATION, *POVERLAY_INFORMATION;

typedef struct _DEVICE_ENTRY {
    DEVICE_ID DeviceId;
    PDEVICE_OVERLAY Overlay;
    OVERLAY_INFORMATION OverlayInfo;
    DEVICE_ATTRIBUTES Attributes;
    ULONG FilterLevel;
    ULONG ReferenceCount;
    DEVICE_FUNCTION_TABLE FunctionTable;
    PVOID DeviceData;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
} DEVICE_ENTRY, *PDEVICE_ENTRY;

/*++

Device Entry Description:

    A device entry is the internal representation for a device after it has
    been opened.

Fields:

    DeviceId - The index into the global device table.

    Attributes - Attribute flags for the device.  Includes, open, read, write.

    ReferenceCount - A reference count to the number of open instances of the
        device.

    FilterLevel - The Device I/O Library supports device filters by
        allowing a device class to open a device and then forward the open
        request, allowing a second device class to open the device.
        For the Device I/O Library to fully support this functionality, it
        requires an additional level of granularity when describing a device.
        Each device class that opens the same device will be assigned a
        different filter level (with the top level device class assigned
        level 0).

    FunctionTable - Function table for I/O operations on the device.

    DeviceData - A pointer to device specific data.  Device information can
        be conceptualized as three different types and a device class is
        expected to define a data buffer as such.

        - Device Information : Data that would be interesting to the user and
          is obtained/modified using BlDeviceGetInformation and
          BlDeviceSetInformation.

        - Device Class Information : Data structures use in all devices of a
          common device type.  The data structures are internel and are not
          exposed to a user in any mannar.

        - Firmware Data : Firmware specific information and data structures
          for a device, including firmware handles, paths, protocols, etc.

     DeviceIndentifier - A pointer to an allocated buffer which saves the
        device identifier for a device.  N.B. This field must be an allocated
        buffer since it is possible for a registered device class to have
        an identifier larger than the defined union, BOOT_ENVIRONMENT_DEVICE.

--*/

typedef struct _REGISTERED_DEVICE_CLASS {
    LIST_ENTRY ListEntry;
    DEVICE_FUNCTION_TABLE FunctionTable;
} REGISTERED_DEVICE_CLASS, *PREGISTERED_DEVICE_CLASS;

/*++

Registered Device Class Description :

    The Device I/O Manager maintains a list of device classes which have
    been registered with the library.  If registered, the device classes'
    open routine will be called prior to the library attempting to open the
    device using well known device classes.

Fields:

    ListEntry - List entry pointing to the previous and next entries in the
        linked list.

    FunctionTable - Registered device classes' function table.

--*/

// -------------------------------------------------------------------- Globals

extern const DEVICE_FUNCTION_TABLE ConsoleDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE BlockIoDeviceFunctionTable;
extern const DEVICE_FUNCTION_TABLE SerialPortFunctionTable;
extern const DEVICE_FUNCTION_TABLE UdpFunctionTable;
extern const DEVICE_FUNCTION_TABLE VmbusDeviceFunctionTable;

// ----------------------------------------------------------------- Prototypes

//
// Filter class initialization routines.
//

NTSTATUS
FvebInitialize (
    VOID
    );

NTSTATUS
BlockIoPurgeCache (
    VOID
    );

VOID
BlockIoPurgeCacheByDevice (
    __in PDEVICE_ENTRY DeviceEntry
    );

PDEVICE_ENTRY
BlpDeviceGetEntry (
    __in DEVICE_ID DeviceId
    );

NTSTATUS
BlpDeviceFlush (
    __in PDEVICE_ENTRY Device
    );

NTSTATUS
BlpDeviceReset (
    __in PDEVICE_ENTRY Device
    );

