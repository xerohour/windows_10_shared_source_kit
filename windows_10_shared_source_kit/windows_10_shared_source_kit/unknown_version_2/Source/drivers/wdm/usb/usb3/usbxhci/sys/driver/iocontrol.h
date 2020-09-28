/*++
Copyright (c) Microsoft Corporation

Module Name:

    iocontrol.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

typedef
VOID
(*PXHCI_IOCTL_FUNCTION) (
    __in
        HANDLE      ObjectHandle,
    __in
        WDFREQUEST  WdfRequest
    );

typedef struct _IOCONTROL_DATA {

    WDFQUEUE                    WdfQueue;
    HANDLE                      ControllerHandle;

} IOCONTROL_DATA, *PIOCONTROL_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(IOCONTROL_DATA, GetIoControlData)

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
IoControl_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL IoControl_WdfEvtIoDeviceControl;

