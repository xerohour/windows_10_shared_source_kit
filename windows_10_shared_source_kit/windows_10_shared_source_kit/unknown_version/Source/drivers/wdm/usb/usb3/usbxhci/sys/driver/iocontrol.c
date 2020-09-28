/*++
Copyright (c) Microsoft Corporation

Module Name:

    iocontrol.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "iocontrol.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IoControl_Create)
#endif

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
    )
/*++

Routine Description:

    This function creates a default queue where all requests targetted to the
    controller's FDO are routed.

--*/
{
    NTSTATUS                status;
    WDF_IO_QUEUE_CONFIG     wdfIoQueueConfig;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDFQUEUE                wdfQueue;
    PIOCONTROL_DATA         ioControlData;

    UNREFERENCED_PARAMETER(ControllerHandle);

    PAGED_CODE();

    TRY {

        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&wdfIoQueueConfig, WdfIoQueueDispatchSequential);
        wdfIoQueueConfig.EvtIoDeviceControl = IoControl_WdfEvtIoDeviceControl;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, IOCONTROL_DATA);

        status = WdfIoQueueCreate(WdfDevice,
                                  &wdfIoQueueConfig,
                                  &wdfAttributes,
                                  &wdfQueue);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(ControllerHandle), IoControl,
                     "WdfIoQueueCreate Failed %!STATUS!", status);
            LEAVE;
        }

        ioControlData = GetIoControlData(wdfQueue);

        ioControlData->WdfQueue = wdfQueue;
        ioControlData->ControllerHandle = ControllerHandle;

        LogInfo(Controller_GetRecorderLog(ControllerHandle), IoControl,
                "wdfQueue 0x%p", wdfQueue);

        //
        // Return iocontrol data as the handle to the iocontrol object.
        //
        *ObjectHandle = (HANDLE)ioControlData;

    } FINALLY {

    }

    return status;
}

VOID
IoControl_WdfEvtIoDeviceControl(
    WDFQUEUE    WdfQueue,
    WDFREQUEST  WdfRequest,
    size_t      OutputBufferLength,
    size_t      InputBufferLength,
    ULONG       IoControlCode
    )
/*++

Routine Description:

    This function is called when a request is received on the default queue.

--*/
{
    PIOCONTROL_DATA ioControlData;
    KPROCESSOR_MODE requestorMode;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    TRY {

        ioControlData = GetIoControlData(WdfQueue);

        if (UcxIoDeviceControl(WdfIoQueueGetDevice(WdfQueue),
                               WdfRequest,
                               OutputBufferLength,
                               InputBufferLength,
                               IoControlCode)) {
            LogVerbose(Controller_GetRecorderLog(ioControlData->ControllerHandle), IoControl,
                       "IoControlCode 0x%x was handled by UCX", IoControlCode);
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(ioControlData->ControllerHandle), IoControl,
                "WdfRequest 0x%p IoControlCode 0x%x", WdfRequest, IoControlCode);

        requestorMode = WdfRequestGetRequestorMode(WdfRequest);

        if (requestorMode != UserMode) {

            LogWarning(Controller_GetRecorderLog(ioControlData->ControllerHandle), IoControl,
                       "Invalid RequestorMode %d", requestorMode);
            WdfRequestComplete(WdfRequest, STATUS_INVALID_PARAMETER);
            LEAVE;
        }

        switch (IoControlCode) {

        default:
            LogError(Controller_GetRecorderLog(ioControlData->ControllerHandle), IoControl,
                       "Unsupported IoControlCode 0x%x", IoControlCode);
            WdfRequestComplete(WdfRequest, STATUS_INVALID_DEVICE_REQUEST);
            break;
        }

    } FINALLY {

    }

    return;
}

