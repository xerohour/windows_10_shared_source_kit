//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    queue.cpp

Abstract:

    This file contains the queue entry points and callbacks.

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "queue.tmh"

VOID
OnInternalDeviceIoControl(
    _In_  WDFQUEUE      FxQueue,
    _In_  WDFREQUEST    FxRequest,
    _In_  size_t        OutputBufferLength,
    _In_  size_t        InputBufferLength,
    _In_  ULONG         IoControlCode    
    )
/*++

Routine Description:

    This event is called when the framework receives 
    IRP_MJ_INTERNAL DEVICE_CONTROL requests from the system.

    ======================================================================================================
    IOCTL                                     IO TYPE    ORIGIN Buffer Location          Original Buffer?
    ==================================        =======    ====== ================         ===================
    IOCTL_HID_GET_DEVICE_DESCRIPTOR           NEITHER    KM     Irp->UserBuffer          Y
    IOCTL_HID_GET_REPORT_DESCRIPTOR           NEITHER    KM     Irp->UserBuffer          Y
    IOCTL_HID_READ_REPORT                     NEITHER    KM     Irp->UserBuffer          Y
    IOCTL_HID_WRITE_REPORT                    NEITHER    KM     Irp->UserBuffer          NO (HID_XFER_PACKET)
    IOCTL_HID_GET_STRING                      NEITHER    KM     Type3InputBuffer(in)     Y
                                                                Irp->UserBuffer (out)    
    IOCTL_HID_ACTIVATE_DEVICE                 NEITHER    KM     Type3InputBuffer         Y
    IOCTL_HID_DEACTIVATE_DEVICE               NEITHER    KM     Type3InputBuffer         Y
    IOCTL_HID_GET_DEVICE_ATTRIBUTES           NEITHER    KM     Irp->UserBuffer          Y
    IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST  NEITHER    KM     Type3InputBuffer         Y (PHID_SUBMIT_IDLE_NOTIFICATION_INFO)
    IOCTL_HID_SET_FEATURE                     IN_DIRECT  UM/KM  Irp->UserBuffer (in)     NO (HID_XFER_PACKET)
    IOCTL_HID_SET_OUTPUT_REPORT (>=XP)        IN_DIRECT  UM/KM  Irp->UserBuffer (in)     NO (HID_XFER_PACKET)
    IOCTL_HID_GET_FEATURE                     OUT_DIRECT UM/KM  Irp->UserBuffer (in/out) NO (HID_XFER_PACKET)
    IOCTL_HID_GET_INPUT_REPORT (>=XP)         OUT_DIRECT UM/KM  Irp->UserBuffer (in/out) NO (HID_XFER_PACKET)              
    IOCTL_GET_PHYSICAL_DESCRIPTOR             OUT_DIRECT UM/KM  Irp->UserBuffer          Y     
    IOCTL_HID_GET_INDEXED_STRING (~)          OUT_DIRECT UM/KM  Type3InputBuffer(in)     Y           
                                                                Irp->MdlAddress (out)    Y
    IOCTL_HID_DEVICERESET_NOTIFICATION        NEITHER    UM/KM  unused                   n/a

Arguments:

    Queue -     Handle to the framework queue object that is associated
                with the I/O request.
    Request -   Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.
Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    NTSTATUS    status;
    BOOLEAN     requestPending = FALSE;

    WDFDEVICE device = WdfIoQueueGetDevice(FxQueue);
    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(device);

    TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_FLAG_IOCTL,
            "Received %s, Queue:0x%p, Request:0x%p\n",
            _DbgInternalIoctlString(IoControlCode),
            FxQueue, 
            FxRequest
            );

    //
    // Note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //
    
    switch (IoControlCode) 
    {
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        //
        // Retrieves the device's HID descriptor.
        //
        status = HidGetDeviceDescriptor(
                            pDeviceContext,
                            FxRequest);
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        //
        // Obtains the report descriptor for the HID device.
        //
        status = HidGetReportDescriptor(
                            pDeviceContext,
                            FxRequest);
        break;

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        //
        //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
        //
        status = HidGetDeviceAttributes(
                            pDeviceContext,
                            FxRequest);
        break;

    case IOCTL_HID_GET_STRING:
        //
        // Requests that the HID minidriver retrieve a human-readable string
        // for either the manufacturer ID, the product ID, or the serial number
        status = HidGetString(
                        pDeviceContext, 
                        FxRequest);
        break;

    case IOCTL_HID_READ_REPORT:

        //
        // Returns a report from the device into a class driver-supplied buffer.
        // Request gets completed if there is cached data or fwded to the manual queue
        // to be completed when the device interrupts to inform of new data.
        //
        status = HidReadReport(
                        pDeviceContext, 
                        FxRequest,
                        &requestPending);
        break;

    case IOCTL_HID_WRITE_REPORT:
        //
        // sends a HID class output report to a top-level collection of a HID
        // class device.
        //
        status = HidWriteReport(
                        pDeviceContext, 
                        FxRequest);
        break;
        
    case IOCTL_HID_GET_INPUT_REPORT:
        //
        // returns a HID class input report associated with a top-level
        // collection of a HID class device.
        //     
        status = HidGetReport(
                        pDeviceContext,
                        FxRequest,
                        ReportTypeInput);
        break;
        
    case IOCTL_HID_GET_FEATURE:
        //
        // returns a feature report associated with a top-level collection
        //
        status = HidGetReport(
                        pDeviceContext,
                        FxRequest,
                        ReportTypeFeature);
        break;

    case IOCTL_HID_SET_OUTPUT_REPORT:
        //
        // sends a HID class output report to a top-level collection of a HID
        // class device.
        //
        status = HidSetReport(
                        pDeviceContext,
                        FxRequest,
                        ReportTypeOutput);
        break;

    case IOCTL_HID_SET_FEATURE:
        //
        // This sends a HID class feature report to a top-level collection of
        // a HID class device.
        //
        status = HidSetReport(
                        pDeviceContext,
                        FxRequest,
                        ReportTypeFeature);
        break;

    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
        //
        // Hidclass sends this IOCTL to notify miniports that it wants
        // them to go into idle
        status = HidSendIdleNotification(
                        pDeviceContext,
                        FxRequest,
                        &requestPending);
        break;
   
    case IOCTL_HID_DEVICERESET_NOTIFICATION:
        //
        // HIDCLASS sends this IOCTL to wait for the device-initiated reset event.
        // We only allow one Device Reset Notification request at a time.
        //
        WdfSpinLockAcquire(pDeviceContext->DeviceResetNotificationSpinLock);

        if (pDeviceContext->DeviceResetNotificationRequest != NULL)
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            requestPending = FALSE;

            TraceEvents(
                TRACE_LEVEL_WARNING,
                TRACE_FLAG_IOCTL,
                "We have an active %s request:0x%p from queue:0x%p already. Fail the new one:0x%p with %!STATUS!.\n",
                _DbgInternalIoctlString(IoControlCode),
                pDeviceContext->DeviceResetNotificationRequest,
                FxQueue,
                FxRequest,
                status
                );
        }
        else
        {
            //
            // Try to make it cancelable
            //
            status = WdfRequestMarkCancelableEx(FxRequest, OnDeviceResetNotificationRequestCancel);

            if (NT_SUCCESS(status))
            {
                //
                // Successfully marked it cancelable. Pend the request then.
                //
                pDeviceContext->DeviceResetNotificationRequest = FxRequest;
                status = STATUS_PENDING;    // just to satisfy the compilier
                requestPending = TRUE;

                TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_IOCTL,
                    "Pending the new %s request:0x%p from queue:0x%p.\n",
                    _DbgInternalIoctlString(IoControlCode),
                    FxRequest,
                    FxQueue
                    );
            }
            else
            {
                //
                // It's already cancelled. Our EvtRequestCancel won't be invoked.
                // We have to complete the request with STATUS_CANCELLED here.
                //
                NT_ASSERT(status == STATUS_CANCELLED);
                status = STATUS_CANCELLED;    // just to satisfy the compilier
                requestPending = FALSE;

                TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_IOCTL,
                    "The new %s request:0x%p from queue:0x%p is already cancelled. Completing it with %!STATUS!.\n",
                    _DbgInternalIoctlString(IoControlCode),
                    FxRequest,
                    FxQueue,
                    status
                    );
            }
        }

        WdfSpinLockRelease(pDeviceContext->DeviceResetNotificationSpinLock);
        break;

    default:
        status = STATUS_NOT_SUPPORTED;
        break;
    }
    
    //
    // Complete the request if it was not forwarded
    //
    if (requestPending == FALSE)
    {
        WdfRequestComplete(FxRequest, status);
    }

    return;
}


VOID
OnIoStop(
    _In_  WDFQUEUE      FxQueue,
    _In_  WDFREQUEST    FxRequest,
    _In_  ULONG         ActionFlags
    )
/*++
Routine Description:

    OnIoStop is called by the framework when the device leaves the D0 working state
    for every I/O request that this driver has not completed, including
    requests that the driver owns and those that it has forwarded to an
    I/O target.

Arguments:

    FxQueue - Handle to the framework queue object that is associated with the I/O request.

    FxRequest - Handle to a framework request object.

    ActionFlags - WDF_REQUEST_STOP_ACTION_FLAGS specifying reason that the callback function is being called 

Return Value:

    None

--*/
{
    WDF_REQUEST_PARAMETERS fxParams;
    WDF_REQUEST_PARAMETERS_INIT(&fxParams);
        
    // Get the request parameters
    WdfRequestGetParameters(FxRequest, &fxParams);

    if (fxParams.Type == WdfRequestTypeDeviceControlInternal)
    {
        switch (fxParams.Parameters.DeviceIoControl.IoControlCode)
        {
        //
        // When we invoke the HID idle notification callback inside the workitem, the idle IRP is
        // yet to be enqueued into the Idle Queued. Since hidclass will queue a Dx IRP from the callback
        // and our default queue is power managed, the Dx IRP progression is blocked because of the in-flight
        // idle IRP. In order to avoid blocking the Dx IRP, we acknowledge the request. Since 
        // we anyway will queue the IRP into the Idle Queue immediately after, this is safe. 
        //
        case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
            //
            // The framework is stopping the I/O queue because the device is leaving its working (D0) state.
            //
            if (ActionFlags & WdfRequestStopActionSuspend)
            {
                //
                // Acknowledge the request
                //
                WdfRequestStopAcknowledge(FxRequest, FALSE);

                TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_IOCTL,
                    "Acknowledged stop on idle Request:0x%p on Queue:0x%p\n",
                    FxRequest,
                    FxQueue
                    );
            }
            break;

        //
        // Device Reset Notification is normally pended by the HIDI2C driver.
        // We need to complete it only when the device is being removed. For
        // device power state changes, we will keep it pended, since HID clients 
        // are not interested in these changes.
        //
        case IOCTL_HID_DEVICERESET_NOTIFICATION:
            if (ActionFlags & WdfRequestStopActionPurge)
            {
                //
                // The framework is stopping it because the device is being removed.
                // So we complete it, if it's not cancelled yet.
                //
                PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(FxQueue));
                BOOLEAN         completeRequest = FALSE;
                NTSTATUS        status;

                WdfSpinLockAcquire(pDeviceContext->DeviceResetNotificationSpinLock);

                status = WdfRequestUnmarkCancelable(FxRequest);
                if (NT_SUCCESS(status))
                {
                    //
                    // EvtRequestCancel won't be called. We complete it here.
                    //
                    completeRequest = TRUE;

                    NT_ASSERT(pDeviceContext->DeviceResetNotificationRequest == FxRequest);
                    if (pDeviceContext->DeviceResetNotificationRequest == FxRequest)
                    {
                        pDeviceContext->DeviceResetNotificationRequest = NULL;
                    }
                }
                else
                {
                    NT_ASSERT(status == STATUS_CANCELLED);

                    //
                    // EvtRequestCancel will be called. Leave it as it is.
                    //
                    TraceEvents(
                        TRACE_LEVEL_INFORMATION,
                        TRACE_FLAG_IOCTL,
                        "Device Reset Notification request:0x%p on Queue:0x%p is no longer cancelable. Leave it as it is.\n",
                        FxRequest,
                        FxQueue
                        );
                }

                WdfSpinLockRelease(pDeviceContext->DeviceResetNotificationSpinLock);

                if (completeRequest)
                {
                    //
                    // Complete the pending Device Reset Notification with STATUS_CANCELLED
                    //
                    status = STATUS_CANCELLED;

                    TraceEvents(
                        TRACE_LEVEL_INFORMATION,
                        TRACE_FLAG_IOCTL,
                        "Completing the Device Reset Notification request:0x%p on Queue:0x%p with %!STATUS!\n",
                        FxRequest,
                        FxQueue,
                        status
                        );

                    WdfRequestComplete(FxRequest, status);
                }
            }
            else 
            {
                //
                // The framework is stopping it because the device is leaving D0.
                // Keep it pending.
                //
                NT_ASSERT(ActionFlags & WdfRequestStopActionSuspend);

                TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_IOCTL,
                    "Acknowledging stop on Device Reset Notification request:0x%p on Queue:0x%p\n",
                    FxRequest,
                    FxQueue
                    );

                WdfRequestStopAcknowledge(FxRequest, FALSE);
            }
            break;

        default:
            //
            // Leave other requests as they are. 
            //
            NOTHING;
        }
    }
}

VOID
OnDeviceResetNotificationRequestCancel(
    _In_  WDFREQUEST FxRequest
    )
/*++
Routine Description:

    OnDeviceResetNotificationRequestCancel is the EvtRequestCancel routine for
    Device Reset Notification requests. It's called by the framework when HIDCLASS 
    is cancelling the Device Reset Notification request that is pending in HIDI2C 
    driver. It basically completes the pending request with STATUS_CANCELLED.

Arguments:

    FxRequest - Handle to a framework request object being cancelled.

Return Value:

    None

--*/
{
    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(FxRequest)));
    NTSTATUS        status = STATUS_CANCELLED;

    //
    // Hold the spinlock to sync up with other threads who
    // reads and then writes to DeviceResetNotificationRequest won't run
    // into a problem while holding the same spinlock.
    //
    WdfSpinLockAcquire(pDeviceContext->DeviceResetNotificationSpinLock);

    //
    // It may have been cleared by either of the following. So we just clear
    // the matched pDeviceContext->DeviceResetNotificationRequest.
    //
    // - ISR (when Device Initiated Reset occurs)
    // - OnIoStop (when device is being removed or leaving D0)
    // 
    if (pDeviceContext->DeviceResetNotificationRequest == FxRequest)
    {
        pDeviceContext->DeviceResetNotificationRequest = NULL;
    }

    WdfSpinLockRelease(pDeviceContext->DeviceResetNotificationSpinLock);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_FLAG_IOCTL,
        "Completing Device Reset Notification request:0x%p with %!STATUS!.\n",
        FxRequest,
        status
        );

    //
    // Simply complete the request here.
    //
    WdfRequestComplete(FxRequest, status);
}

PCHAR
_DbgInternalIoctlString(
    _In_ ULONG        IoControlCode
    )
/*++

Routine Description:

    Returns Ioctl string helpful for debugging

Arguments:

    IoControlCode - IO Control code

Return Value:

    Name String

--*/
{
    switch (IoControlCode)
    {
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";
    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        return "IOCTL_HID_GET_REPORT_DESCRIPTOR";
    case IOCTL_HID_READ_REPORT:
        return "IOCTL_HID_READ_REPORT";
    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";
    case IOCTL_HID_WRITE_REPORT:
        return "IOCTL_HID_WRITE_REPORT";
    case IOCTL_HID_SET_FEATURE:
        return "IOCTL_HID_SET_FEATURE";
    case IOCTL_HID_GET_FEATURE:
        return "IOCTL_HID_GET_FEATURE";
    case IOCTL_HID_GET_STRING:
        return "IOCTL_HID_GET_STRING";
    case IOCTL_HID_GET_INPUT_REPORT:
        return "IOCTL_HID_GET_INPUT_REPORT";
    case IOCTL_HID_SET_OUTPUT_REPORT:
        return "IOCTL_HID_SET_OUTPUT_REPORT";
    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
        return "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST";
    case IOCTL_HID_DEVICERESET_NOTIFICATION:
        return "IOCTL_HID_DEVICERESET_NOTIFICATION";
    default:
        return "Unknown IOCTL";
    }
}
