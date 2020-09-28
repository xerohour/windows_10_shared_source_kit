/*++

Module Name:

    power.cpp

Abstract:

    This file contains the declarations for Power specific callbacks
    and function definitions

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "power.tmh"

VOID
PowerIdleIrpWorkitem(
            _In_  WDFWORKITEM IdleWorkItem
            )
/*++

Routine Description:
 
    This is a workitem routine that HidSendIdleNotification queues when 
    handling the HIDClass's idle notification IRP, so the idle callback can be made in
    a different thread context, instead of the Idle Irp's dispatch call.

Arguments:

    IdleWorkItem    -   Handle to a WDF workitem object

Return Value:

    VOID

--*/
{
    FuncEntry(TRACE_FLAG_POWER);
    
    NTSTATUS status;

    PWORKITEM_CONTEXT pWorkItemContext = GetWorkItemContext(IdleWorkItem);
    NT_ASSERT(pWorkItemContext != NULL);

    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(pWorkItemContext->FxDevice);
    NT_ASSERT(pDeviceContext != NULL);

    //
    // Get the idle callback info from the workitem context
    //
    PHID_SUBMIT_IDLE_NOTIFICATION_CALLBACK_INFO idleCallbackInfo = _HidGetIdleCallbackInfo(pWorkItemContext->FxRequest);

    //
    // idleCallbackInfo is validated already, so invoke idle callback
    //
    idleCallbackInfo->IdleCallback(idleCallbackInfo->IdleContext);

    //
    // Park this request in our IdleQueue and mark it as pending
    // This way if the IRP was cancelled, WDF will cancel it for us
    //
    status = WdfRequestForwardToIoQueue(
                            pWorkItemContext->FxRequest,
                            pDeviceContext->IdleQueue);

    if (!NT_SUCCESS(status))
    {
        //
        // IdleQueue is a manual-dispatch, non-power-managed queue. This should
        // *never* fail.
        //

        NT_ASSERTMSG("WdfRequestForwardToIoQueue to IdleQueue failed!", FALSE);

        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_POWER,
                "WdfRequestForwardToIoQueue failed forwarding idle notification Request:0x%p to IdleQueue:0x%p "
                "status:%!STATUS!",
                pWorkItemContext->FxRequest,
                pDeviceContext->IdleQueue,
                status);

        //
        // Complete the request if we couldnt forward to the Idle Queue
        //
        WdfRequestComplete(pWorkItemContext->FxRequest, status);
    }
    else
    {
        TraceEvents(
                TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_POWER,
                "Forwarded idle notification Request:0x%p to IdleQueue:0x%p "
                "status:%!STATUS!",
                pWorkItemContext->FxRequest,
                pDeviceContext->IdleQueue,
                status);
    }
    
    if (!NT_SUCCESS(status))
    {
        TraceLoggingPowerDevice(
            status, 
            pDeviceContext->FxDevice);
    }
    //
    // Delete the workitem since we're done with it
    //
    WdfObjectDelete(IdleWorkItem);

    FuncExit(TRACE_FLAG_POWER);
    return;
}


VOID
PowerIdleIrpCompletion(
            _In_ PDEVICE_CONTEXT    FxDeviceContext
            )
/*++

Routine Description:
 
    This is invoked when we enter D0. 
    We simply complete the Idle Irp in case it already hasnt bein cancelled.

Arguments:

    FxDeviceContext -  Pointer to Device Context for the device

Return Value:

    

--*/
{
    FuncEntry(TRACE_FLAG_POWER);
    NTSTATUS status = STATUS_SUCCESS;

    {
        //
        // Lets try to retrieve the Idle IRP from the Idle queue
        //
        WDFREQUEST request = NULL;
        status = WdfIoQueueRetrieveNextRequest(
                                FxDeviceContext->IdleQueue,
                                &request);

        //
        // We did not find the Idle IRP, maybe it was cancelled
        // 
        if (!NT_SUCCESS(status) || (request == NULL))
        {
            TraceEvents(
                    TRACE_LEVEL_WARNING,
                    TRACE_FLAG_POWER,
                    "WdfIoQueueRetrieveNextRequest failed to find idle notification request in IdleQueue:0x%p "
                    "status:%!STATUS!",
                    FxDeviceContext->IdleQueue,
                    status);
        }
        else
        {
            //
            // Complete the Idle IRP
            //
            WdfRequestComplete(request, status);

            TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_POWER,
                    "Completed idle notification Request:0x%p from IdleQueue:0x%p "
                    "status:%!STATUS!",
                    request,
                    FxDeviceContext->IdleQueue,
                    status);
        }
    }

    if (!NT_SUCCESS(status))
    { 
        TraceLoggingPowerDevice(
            status, 
            FxDeviceContext->FxDevice);
    }
    FuncExit(TRACE_FLAG_POWER);
    return;
}