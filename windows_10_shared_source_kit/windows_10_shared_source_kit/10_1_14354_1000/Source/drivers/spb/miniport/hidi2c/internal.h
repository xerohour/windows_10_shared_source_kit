//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    internal.h

Abstract:

    This module contains the internal declarations for
    the HIDI2C driver

Author:

    Arvind Aiyar March 2011

Environment:

    kernel

--*/

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

//
// Common includes.
//
#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntintsafe.h>

#include "wdf.h"
#include <wdm.h>

#include <initguid.h>
#include <devpkey.h>

//
// HID definitions
//
#include <hidport.h>

//
// SPB definitions
//
#include "spb.h"

//
// Resource and descriptor definitions.
//
#define RESHUB_USE_HELPER_ROUTINES
#include "reshub.h"

//
// ACPI IOCTL definitions.
//
#include <acpiioct.h>

//
// Telemetry
//
#include <telemetry.h>

//
// Common typedefs and defines 
//

typedef BYTE* PBYTE;

#define countof(x) (sizeof(x) / sizeof(x[0]))

#define WdfDeleteObjectSafe(o)  \
    if (o != NULL) { WdfObjectDelete(o); }

//
// ACPI Device settings
//
typedef struct _ACPI_DEVICE_SETTINGS
{
    // ACPI "_DSM" HID Descriptor Address
    USHORT  HidDescriptorAddress;

} ACPI_DEVICE_SETTINGS, *PACPI_DEVICE_SETTINGS;

#include "pshpack1.h"

//
// HID Device Settings (HID has a different notion of a "HID Descriptor")
//
typedef struct _HID_DEVICE_SETTINGS
{
    // Length of the HID Descriptor
    USHORT HidDescriptorLength;

    // BCD Version
    USHORT  BcdVersion;

    // Length of the Report Descriptor
    USHORT  ReportDescriptorLength;

    // Report Descriptor Register address
    USHORT  ReportDescriptorAddress;

    // Input Register Address
    USHORT  InputRegisterAddress;

    // Input Register Maximum Length
    USHORT  InputReportMaxLength;

    // Output Register Address
    USHORT  OutputRegisterAddress;

    // Output Register Maximum Length
    USHORT  OutputReportMaxLength;

    // Command Register Address
    USHORT  CommandRegisterAddress;

    // Data Register Address
    USHORT  DataRegisterAddress;

    // Vendor Id
    USHORT VendorId;
    
    // Device Id
    USHORT ProductId;

    //Revision Id
    USHORT VersionId;

    // Reserved Bits
    ULONG  Reserved;

} HID_DEVICE_SETTINGS, *PHID_DEVICE_SETTINGS;

#include "poppack.h"

//
// Registry settings
//
typedef struct _REGISTRY_DEVICE_SETTINGS
{
    BOOLEAN CompactWriteWriteSequences;

} REGISTRY_DEVICE_SETTINGS, *PREGISTRY_DEVICE_SETTINGS;

//
// Device context
//

typedef struct _DEVICE_CONTEXT
{
    // Handle to the WDF device.
    WDFDEVICE                   FxDevice;

    // Locks
    WDFSPINLOCK                 InProgressLock;

    // Interrupts, DPCs and workitems
    WDFINTERRUPT                ReadInterrupt;
    volatile BOOLEAN            ProcessInterrupts;
    BOOLEAN                     DpcRerun;
    BOOLEAN                     DpcInProgess;

    // Queues 
    WDFQUEUE                    IoctlQueue;
    WDFQUEUE                    ReportQueue;
    WDFQUEUE                    CompletionQueue;
    WDFQUEUE                    IdleQueue;

    // SPB specific settings
    WDFIOTARGET                 SpbIoTarget;
    LARGE_INTEGER               SpbConnectionId;
    WDFREQUEST                  SpbRequest;

    // ACPI specific settings
    ACPI_DEVICE_SETTINGS        AcpiSettings;

    // HID specific settings
    HID_DEVICE_SETTINGS         HidSettings;
    PBYTE                       pHidInputReport;
    LIST_ENTRY                  InputReportListHead;
    WDFSPINLOCK                 InputReportListLock;
    ULONG                       InputReportListCount;

    // Flags
    BOOLEAN                     HostInitiatedResetActive;

    // Device Reset Notification members
    WDFSPINLOCK                 DeviceResetNotificationSpinLock;
    WDFREQUEST                  DeviceResetNotificationRequest;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;


//
// Request context
//
typedef struct _REQUEST_CONTEXT
{    
    // Handle to the WDF device
    WDFDEVICE FxDevice;

    // Handle to the WDFMemory object
    WDFMEMORY FxMemory;

} REQUEST_CONTEXT, *PREQUEST_CONTEXT;


//
// Memory context
// 
typedef struct _MEMORY_CONTEXT
{    
    // linked list entry that holds an input report
    LIST_ENTRY InputReportListEntry;

} MEMORY_CONTEXT, *PMEMORY_CONTEXT;


//
// Workitem context
// 
typedef struct _WORKITEM_CONTEXT
{    
    // Handle to a WDF device object
    WDFDEVICE FxDevice;

    // Handle to a WDF request object
    WDFREQUEST FxRequest;

} WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

//
// Inline function to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT,GetDeviceContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT,GetRequestContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(MEMORY_CONTEXT,GetMemoryContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT,GetWorkItemContext);

// Command Op-Codes

typedef enum HID_COMMAND
{
    CmdMin          = 0,
    CmdReservedLow  = CmdMin,
    CmdReset        = 1,
    CmdGetReport    = 2,
    CmdSetReport    = 3,
    CmdGetIdle      = 4,
    CmdSetIdle      = 5,
    CmdGetProtocol  = 6,
    CmdSetProtocol  = 7,
    CmdSetPower     = 8,
    CmdReservedHigh = 9,
    CmdMax          = CmdReservedHigh
} HID_COMMAND, *PHID_COMMAND;

// Report Types

typedef enum HID_REPORT_TYPE
{
    ReportTypeMin       = 0,
    ReportTypeReserved  = ReportTypeMin,
    ReportTypeInput     = 1,
    ReportTypeOutput    = 2,
    ReportTypeFeature   = 3,
    ReportTypeMax       = ReportTypeFeature
} HID_REPORT_TYPE, *PHID_REPORT_TYPE;

// Power States

typedef enum HID_POWER_STATE
{
    PowerStateMin       = 0,
    PowerStateOn        = PowerStateMin,
    PowerStateSleep     = 1,
    PowerStateMax       = PowerStateSleep
} HID_POWER_STATE, *PHID_POWER_STATE;

//
// ETW generated header
//
#include "hidi2c.Events.h"

#include "trace.h"
#include "driver.h"
#include "device.h"
#include "queue.h"
#include "acpi.h"
#include "bus.h"
#include "hid.h"
#include "interrupt.h"
#include "power.h"
#include "registry.h"
#include "hidmsg.h"

//
// Request specific defines
//

//
// HID specific defines
//
const USHORT HID_DESCRIPTOR_SIZE_V1 = 0x1E;
const USHORT HID_DESCRIPTOR_BCD_VERSION = 0x0100;
const USHORT HID_DESCRIPTOR_LENGTH_FIELD_SIZE = 2;
const USHORT HID_REPORT_LENGTH_FIELD_SIZE = 2;
const USHORT HID_REPORT_ID_OPTIMAL = 15;

const PWSTR MANUFACTURER_ID = L"Microsoft";
const PWSTR PRODUCT_ID      = L"HIDI2C Device";
const PWSTR SERIAL_NUMBER   = L"9999";

//
// Registers 
//
const USHORT REG_SIZE = 2;

//
// ACPI defines/constants
//

// {3CDFF6F7-4267-4555-AD05-B30A3D8938DE}
DEFINE_GUID(ACPI_DSM_GUID_HIDI2C, 
0x3CDFF6F7, 0x4267, 0x4555, 0xAD, 0x05, 0xB3, 0x0A, 0x3D, 0x89, 0x38, 0xDE);

#define ACPI_DSM_REVISION               1
#define ACPI_DSM_QUERY_FUNCTION         0
#define ACPI_DSM_HIDI2C_FUNCTION        1
#define ACPI_DSM_INPUT_ARGUMENTS_COUNT  4

#define EXPECTED_IOCTL_ACPI_GET_DEVICE_INFORMATION_STRING_LENGTH 100

//
// Tag used for pool allocations.
//

#define HIDI2C_POOL_TAG 'IdiH'

#define HIDI2C_ALLOCATE_POOL(Type, Size) \
    ExAllocatePoolWithTag((Type), (Size), HIDI2C_POOL_TAG)

#define HIDI2C_FREE_POOL(Buffer) \
    ExFreePoolWithTag((Buffer), HIDI2C_POOL_TAG)

#endif