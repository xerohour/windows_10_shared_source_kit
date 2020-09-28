/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    Wmilog.h

Abstract:

    This module contains Wmi loging support

Author:

    Jason Kace (jkace)

Environment:

    NT Kernel Model Driver only

Revision History:

    6/2004: jkace   Initial Version

--*/

#ifndef _WMILOG_H_
#define _WMILOG_H_



//
// Globals
//

extern ULONG       UsbportWmiTraceEnable;
extern ULONG       UsbportWmiTraceLevel;
extern ULONG       UsbportWmiTraceFlags;
extern TRACEHANDLE UsbportWmiLoggerHandle;


#define USBPORT_WMI_EVENT(lvl, flg, x) \
{\
    if(UsbportWmiTraceEnable &&   \
       lvl <= UsbportWmiTraceLevel && \
       (flg & UsbportWmiTraceFlags) ){ \
        USBPORT_WmiLogEvent x; \
    } \
}

#define USBPORT_WMI_DATA(lvl, flg, x) \
{\
    if(UsbportWmiTraceEnable &&   \
       lvl <= UsbportWmiTraceLevel && \
       (flg & UsbportWmiTraceFlags) ){ \
        USBPORT_WmiLogData x; \
    } \
}

//
// Structures
//

typedef struct _WMI_LOG_DATA{
    EVENT_TRACE_HEADER  Header;
    MOF_FIELD           Data;
} WMI_LOG_DATA, *PWMI_LOG_DATA;

typedef struct _USBPORT_WMI_DATA{
    ULONG   EventType;
    PVOID   DataPtr;
    ULONG   Data;
    ULONG   BufferLength;
    UCHAR   Buffer[1024];
} USBPORT_WMI_DATA, *PUSBPORT_WMI_DATA;

//
// Event Levels
//

#define USBPORT_WMI_BASIC 1
#define USBPORT_WMI_VERBOSE 2

//
// Event Types
//

//USBPORT_TRANSFER_URB
#define USBPORT_EVENT_TYPE_URB_DISPATCH     128
#define USBPORT_EVENT_TYPE_URB_COMPLETE     129

// USBPORT_TRANSFER_SCHEDULE
#define USBPORT_EVENT_TYPE_MINIPORT_SCHEDULED    130
#define USBPORT_EVENT_TYPE_MINIPORT_DONE         131

// USBPORT_SYSTEM_POWER_RESUME
#define USBPORT_EVENT_TYPE_S0_DISPATCH      132
#define USBPORT_EVENT_TYPE_RESUME_COMPLETE  133

// USBPORT_URB_LOG
#define USBPORT_EVENT_TYPE_URB_LOG          134

// USBPORT_DISPATCH_ERROR
#define USBPORT_EVENT_TYPE_DISPATCH_ERROR   135

// USBPORT_DPC_TIME
#define USBPORT_EVENT_TYPE_DPC_START        136
#define USBPORT_EVENT_TYPE_DPC_END          137


//
// GUID_USB_TRANSFER_TRACING enable flags
//

#define USBPORT_TRANSFER_URB            0x0001
#define USBPORT_TRANSFER_SCHEDULE       0x0002

#define USBPORT_DISPATCH_ERROR          0x0010
#define USBPORT_URB_LOG                 0x0020


//
// GUID_USB_PERFORMANCE_TRACING enable flags
//

#define USBPORT_DPC_TIME                0x0100
#define USBPORT_SYSTEM_POWER_RESUME     0x0200


//
// Function Prototypes
//

NTSTATUS
USBPORT_WmiDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath,
    PUSBPORT_MINIPORT_DRIVER MiniportDriver
    );

VOID
USBPORT_WmiUnload(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING MiniportRegistryPath
    );

VOID
USBPORT_WmiRegisterLog(
    IN  PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_WmiUnRegisterLog(
    IN  PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_WmiLogEvent(
    IN UCHAR    EventType,
    IN GUID     LogGUID,
    IN PVOID    Data1,
    IN ULONG    Data2,
    IN PUCHAR   Format,
    IN ...
    );

NTSTATUS
USBPORT_WmiLogData(
    IN UCHAR    EventType,
    IN GUID     LogGUID,
    IN PVOID    Data1,
    IN ULONG    Data2,
    IN PVOID    Buffer,
    IN ULONG    BufferLength
    );

NTSTATUS
USBPORT_DispatchSystemControl(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_QueryWmiRegInfo(
    IN PDEVICE_OBJECT FdoDeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING *RegistryPath,
    OUT PUNICODE_STRING MofResourceName,
    OUT PDEVICE_OBJECT *Pdo
    );

NTSTATUS
USBPORT_InitializeWmilibContext(
    IN PWMILIB_CONTEXT WmilibContext
    );

NTSTATUS
USBPORT_FunctionControl(
  IN PDEVICE_OBJECT FdoDeviceObject,
  IN PIRP Irp,
  IN ULONG GuidIndex,
  IN WMIENABLEDISABLECONTROL Function,
  IN BOOLEAN Enable
  );

NTSTATUS
USBPORT_GetControllerNodeInfo(PDEVICE_OBJECT FdoDeviceObject,
                              __in_bcount(BufferLength) PVOID Buffer,
                              ULONG BufferLength,
                              PULONG LengthOfDataReturned);

NTSTATUS
USBPORT_QueryWmiDataBlock(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG InstanceCount,
    PULONG InstanceLengthArray,
    ULONG OutBufferSize,
    PUCHAR Buffer
    );

NTSTATUS
USBPORT_GetControllerPerformanceInfo(PDEVICE_OBJECT FdoDeviceObject,
                                     __in_bcount(BufferLength) PVOID Buffer,
                                     ULONG BufferLength,
                                     PULONG LengthOfDataReturned);

#endif // _WMILOG_H_
