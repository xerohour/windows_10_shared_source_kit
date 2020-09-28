//
//    Copyright (c) Microsoft.  All rights reserved.
//
#pragma once

#include <wmistr.h>
#include <evntrace.h>

//
// PortCls logging globals
//
extern ULONG		PortClsWmiLogEnable;
extern ULONG		PortClsWmiLogLevel;
extern ULONG		PortClsWmiLogFlags;
extern TRACEHANDLE	PortClsWmiLoggerHandle;

//
// PortCls logging structures
//
typedef struct _WMI_LOG_DATA
{
	EVENT_TRACE_HEADER	Header;
	MOF_FIELD			Data;
} WMI_LOG_DATA, *PWMI_LOG_DATA;

typedef struct _PORTCLS_WMI_DATA
{
    ULONGLONG EventType;
    ULONGLONG Data1;
    ULONGLONG Data2;
    ULONGLONG Data3;
    ULONGLONG Data4;
    ULONGLONG Data5;
} PORTCLS_WMI_DATA, *PPORTCLS_WMI_DATA;

typedef struct _PC_WMI_PWMILIB_CONTEXT
{
  WMILIB_CONTEXT         PcWmiLibContext;
  PWMILIB_CONTEXT        MiniPortWmiLibContext;
  ULONG                  MiniPortControlGuidIndex;
  BOOL                   SameGUIDUsed;
} PC_WMI_PWMILIB_CONTEXT, *PPC_WMI_PWMILIB_CONTEXT;

//
// PortCls logging event levels
//
#define PORTCLS_WMI_BASIC	1
#define PORTCLS_WMI_VERBOSE	2

//
// PortCls logging event types
//

// Pin State Events
#define PC_ETYPE_PIN_STOP		0x01
#define PC_ETYPE_PIN_ACQUIRE	0x02
#define PC_ETYPE_PIN_PAUSE		0x03
#define PC_ETYPE_PIN_RUN		0x04
#define PC_ETYPE_PIN_CREATED	0x05
#define PC_ETYPE_PIN_CLOSED		0x06

// Irp Processing Events
#define PC_ETYPE_IRP_RECEIVED	        0x10
#define PC_ETYPE_IRP_COMPLETED	        0x11
#define PC_ETYPE_IRP_CANCELLED	        0x12
#define PC_ETYPE_IRP_MAPPING_ACQUIRED	0x13
#define PC_ETYPE_IRP_MAPPING_RELEASED	0x14
#define PC_ETYPE_IRP_WAVECYC_DPC_ENTRY  0x15
#define PC_ETYPE_IRP_WAVECYC_DPC_EXIT   0x16
#define PC_ETYPE_IRP_WAVECYC_GLITCH     0x17


// Power State Events
#define PC_ETYPE_PWR_D0			        0x20
#define PC_ETYPE_PWR_D1			        0x21
#define PC_ETYPE_PWR_D2			        0x22
#define PC_ETYPE_PWR_D3			        0x23

// Position Events
#define PC_ETYPE_POS_PIN        0x30
#define PC_ETYPE_POS_DEVICE		0x31

// SubDevice Events
#define PC_ETYPE_SUBDEV_REGISTER	0x40
#define PC_ETYPE_SUBDEV_UNREGISTER	0x41

//ServiceGroup Events
#define PC_ETYPE_SC_DPC_CALLED	0x50
#define PC_ETYPE_SC_DPC_DONE	0x51

//Miniport audio engine events

#define PC_ETYPE_MINIPORT_MINIPORT_DEFINED          0x60
#define PC_ETYPE_MINIPORT_BUFFER_COMPLETE_EVENT     0x61
#define PC_ETYPE_MINIPORT_PIN_STATE                 0x62
#define PC_ETYPE_MINIPORT_GET_STREAM_POS            0x63
#define PC_ETYPE_MINIPORT_SET_WAVERT_WRITE_POS      0x64
#define PC_ETYPE_MINIPORT_GET_PRESENTATION_POS      0x65
#define PC_ETYPE_MINIPORT_PROGRAM_DMA               0x66
#define PC_ETYPE_MINIPORT_GLITCH_REPORT             0x67
#define PC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED      0x68
#define PC_ETYPE_MINIPORT_PROCESSING_MODE           0x69
#define PC_ETYPE_MINIPORT_FX_CLSID                  0x6a

// Power Policy Events
#define PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION        0x70
#define PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE  0x71

// Pin Types for PC_ETYPE_PIN_CREATED
#define PC_PINTYPE_WAVECYC		0x01
#define PC_PINTYPE_WAVEPCI		0x02
#define PC_PINTYPE_WAVERT		0x03

//
// PortCls logging function prototypes
//
NTSTATUS 
PcWmiLogEvent(
    IN UCHAR    Level,
    IN UCHAR	EventType,
    IN GUID		LogGUID,
    IN ULONGLONG pvData1,
    IN ULONGLONG pvData2,
    IN ULONGLONG ulData3,
    IN ULONGLONG ulData4,
    IN ULONGLONG pvData5);

NTSTATUS
InternalPcWmiRegisterProvider (IN PDEVICE_OBJECT DeviceObject);

NTSTATUS
InternalPcWmiUnregisterProvider (IN PDEVICE_OBJECT DeviceObject);

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH PcWmiSystemControl;

WMI_QUERY_REGINFO_CALLBACK PcQueryWmiRegInfo;

WMI_QUERY_DATABLOCK_CALLBACK PcQueryWmiDataBlock;

WMI_FUNCTION_CONTROL_CALLBACK PcWmiFunctionControl;

WMI_SET_DATABLOCK_CALLBACK PcSetWmiDataBlock;

WMI_SET_DATAITEM_CALLBACK PcSetWmiDataItem;

WMI_EXECUTE_METHOD_CALLBACK PcExecuteWmiMethod;

