/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubxfer.h

Abstract:

    Control and interrupt transfers for the hub.

--*/

#pragma once


typedef struct _CLEAR_TT_BUFFER_CONTEXT {
    CONTROL_REQUEST     ControlRequest;
    PHUB_FDO_CONTEXT    HubFdoContext;
    UCXENDPOINT         UcxEndpoint;
} CLEAR_TT_BUFFER_CONTEXT, *PCLEAR_TT_BUFFER_CONTEXT;

typedef union _USB_HUB_CLEAR_TT_BUFFER_VALUE {  
    USHORT  AsUShort;
    struct {  
        USHORT   EndpointNumber:4;     // 0 - 3
        USHORT   DeviceAddress:7;      // 4 - 10          
        USHORT   EndpointType:2;       // 11 - 12          
        USHORT   Reserved_MBZ:2;       // 13-14    
        USHORT   Direction_1forIN:1;   // 15
    };  
} USB_HUB_CLEAR_TT_BUFFER_VALUE, *PUSB_HUB_CLEAR_TT_BUFFER_VALUE;  

VOID
HUBHTX_ClearTTBuffer(
    __in
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext,
    __in
        UCXENDPOINT        UcxEndpoint,
    __in
        ULONG              EndpointNumber,
    __in
        ULONG              TTHubPortNumber
    );

VOID
HUBHTX_NoPingResponse(
    __in
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext
    );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBHTX_GetPortStatusForBootDevice (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __out
        PUSB_PORT_STATUS_AND_CHANGE     PortStatusChange
    );
