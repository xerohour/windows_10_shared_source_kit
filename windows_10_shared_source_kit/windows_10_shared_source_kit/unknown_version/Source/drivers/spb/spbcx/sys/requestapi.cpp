/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    requestAPI.cpp

Abstract:

    This module contains stubbed-out implementations of each of the 
    SPBCx DDI functions.

Environment:

    kernel-mode only

Revision History:

--*/

#include "internal.h"

extern "C" 
{
#include "requestAPI.tmh"
}

__drv_maxIRQL(DISPATCH_LEVEL)
SPBTARGET
SPBEXPORT(SpbRequestGetTarget)(
    __in PSPB_DRIVER_GLOBALS DriverGlobals,
    __in SPBREQUEST          SpbRequest
    )
{
    PCScxRequest request;
    SPBTARGET target = NULL;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(SpbRequest)))
    {
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object",
            SpbRequest
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }
    
    target = request->GetTarget()->GetSpbObject();

exit:

    return target;
}

__drv_maxIRQL(DISPATCH_LEVEL)
WDFDEVICE
SPBEXPORT(SpbRequestGetController)(
    __in PSPB_DRIVER_GLOBALS DriverGlobals,
    __in SPBREQUEST          SpbRequest
    )
{
    PCScxRequest request;
    WDFDEVICE device = NULL;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(SpbRequest)))
    {
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object",
            SpbRequest
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    device = request->GetController()->GetFxObject();

exit:

    return device;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
SPBEXPORT(SpbRequestGetParameters)(
    __in  PSPB_DRIVER_GLOBALS      DriverGlobals,
    __in  SPBREQUEST               SpbRequest,
    __out SPB_REQUEST_PARAMETERS * Parameters
    )
{
    PCScxRequest request;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(SpbRequest)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }
    
#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "Parameters is initialized by caller")
    if (!NT_SUCCESS(ScxVerifyNotNull(Parameters)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object",
            SpbRequest
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate the request parameters size field is valid
    //

    if (Parameters->Size != sizeof(SPB_REQUEST_PARAMETERS))
    {
#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "Parameters is initialized by caller")
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Parameters %p Size %d incorrect, expected %d", 
            Parameters,
            Parameters->Size,
            sizeof(SPB_REQUEST_PARAMETERS)
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    request->GetParameters(Parameters);

exit:

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
SPBEXPORT(SpbRequestGetTransferParameters)(
    __in      PSPB_DRIVER_GLOBALS       DriverGlobals,
    __in      SPBREQUEST                SpbRequest,
    __in      ULONG                     Index,
    __out_opt SPB_TRANSFER_DESCRIPTOR * TransferDescriptor,
    __out_opt PMDL                    * TransferBuffer
    )
{
    PCScxRequest request;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(SpbRequest)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object",
            SpbRequest
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate the TransferDescriptor size field is valid
    //
    
#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "TransferDescriptor is initialized by caller")
    if ((TransferDescriptor != NULL) && 
        (TransferDescriptor->Size != sizeof(SPB_TRANSFER_DESCRIPTOR)))
    {
#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "TransferDescriptor is initialized by caller")
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "TransferDescriptor %p Size %d incorrect, expected %d", 
            TransferDescriptor,
            TransferDescriptor->Size,
            sizeof(SPB_TRANSFER_DESCRIPTOR)
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate transfer index
    //

    if (Index >= request->GetTransferCount())
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Transfer index %d is invalid", 
            Index
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    request->GetTransferParameters(Index,
                                   TransferDescriptor,
                                   TransferBuffer);

exit:

    return ;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
SPBEXPORT(SpbRequestComplete)(
    __in PSPB_DRIVER_GLOBALS DriverGlobals,
    __in SPBREQUEST SpbRequest,
    __in NTSTATUS   CompletionStatus
    )
{
    PCScxRequest request;
    SPB_REQUEST_TYPE spbIoType;
    ULONG_PTR information;
    size_t length;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(SpbRequest)))
    {
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object",
            SpbRequest
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    spbIoType = request->GetSpbIoType();

    if ((spbIoType == SpbRequestTypeLockController) ||
        (spbIoType == SpbRequestTypeUnlockController))
    {
        if (CompletionStatus != STATUS_SUCCESS)
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_APIERROR,
                "SpbRequest %p failed with %!STATUS!. %!SPB_REQUEST_TYPE! "
                "should always succeed",
                SpbRequest,
                CompletionStatus,
                spbIoType
                );
            WdfVerifierDbgBreakPoint();
        }
    }

    //
    // Log info about the request and complete it.
    //

    information = WdfRequestGetInformation(SpbRequest);
    length = request->GetTotalCb();

    BOOLEAN incompleteIo = FALSE;

    if (request->IsTransferListIo())
    {
        //
        // Check to see if the request was completed successfully
        // but with fewer bytes than expected.
        //

        if (NT_SUCCESS(CompletionStatus) && (information < length))
        {
            TraceMessage(
                TRACE_LEVEL_WARNING,
                TRACE_FLAG_REQUEST,
                "SPBREQUEST %p completed with %Iu out of %Iu expected bytes", 
                SpbRequest,
                information,
                length
                );

            incompleteIo = TRUE;
        }
    }

    request->WritePayloadEvents();

    SPBTARGET target = NULL;

    //
    // If SpbRequestComplete is being called in the controller driver's
    // InCallerContextCallback, this request may not have been contructed, i.e., the
    // context will be zeroed out. Handle that case gracefully. Trace messages and ETW events
    // will have target as NULL.
    //

    if (request->IsConstructed())
    {
        target = request->GetTarget()->GetSpbObject();

        request->GetTarget()->LogIoCompletionStatus(
            CompletionStatus,
            incompleteIo);
    }

    if (NT_SUCCESS(CompletionStatus))
    {
        TraceMessage(
            TRACE_LEVEL_INFORMATION,
            TRACE_FLAG_REQUEST,
            "SPBREQUEST %p for SPBTARGET %p completed with %!STATUS!", 
            SpbRequest,
            target,
            CompletionStatus
            );
    }
    else
    {
        TraceMessage(
            TRACE_LEVEL_WARNING,
            TRACE_FLAG_REQUEST,
            "SPBREQUEST %p for SPBTARGET %p completed with %!STATUS!", 
            SpbRequest,
            target,
            CompletionStatus
            );
    }

    EventWrite_SPBCX_IO_COMPLETE(
        request->GetActivityId(), 
        request->GetFxObject(), 
        CompletionStatus, 
        information);

    WdfRequestComplete(SpbRequest, CompletionStatus);

exit:

    return;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
SPBEXPORT(SpbRequestCaptureIoOtherTransferList)(
    __in PSPB_DRIVER_GLOBALS DriverGlobals,
    __in SPBREQUEST SpbRequest
    )
{
    PCScxRequest request;
    NTSTATUS status;

    //
    // Validate globals
    //

    status = ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals));

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    //
    // Validate parameters
    //

    status = ScxVerifyNotNull(SpbRequest);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    //
    // Verify that we are at the correct IRQL. This DDI must be
    // called from PASSIVE_LEVEL for user mode requests and less
    // than or equal to DISPATCH_LEVEL for kernel mode requests.
    //

    KPROCESSOR_MODE captureMode = WdfRequestGetRequestorMode(SpbRequest);
    KIRQL maxIrql = (captureMode == UserMode) ? PASSIVE_LEVEL : DISPATCH_LEVEL;
    
    status = ScxVerifyIrqlLessThanOrEqual(maxIrql);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    //
    // Validate that we were handed an actual SPB request object and that
    // the request context has not already been constructed.
    //

    request = GetRequestFromFxRequest(SpbRequest);

    if (request == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is not a real SPB request object - %!STATUS!",
            SpbRequest,
            status
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    if (request->IsConstructed() == true)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p has already been constructed, its transfer"
            "list cannot be captured - %!STATUS!",
            SpbRequest,
            status
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    //
    // Validate that the SPB request IOCTL code is not
    // one already handled internally.
    //

    WDF_REQUEST_PARAMETERS fxParams;
    WDF_REQUEST_PARAMETERS_INIT(&fxParams);

    WdfRequestGetParameters(SpbRequest, &fxParams);

    if ((fxParams.Type != WdfRequestTypeDeviceControl) &&
        (fxParams.Type != WdfRequestTypeDeviceControlInternal))
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "SpbRequest %p is of unsupported request type %!WDF_REQUEST_TYPE! "
            " - %!STATUS!",
            SpbRequest,
            fxParams.Type,
            status
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    switch(fxParams.Parameters.DeviceIoControl.IoControlCode)
    {
        case IOCTL_SPB_LOCK_CONTROLLER:
        case IOCTL_SPB_UNLOCK_CONTROLLER:
        case IOCTL_SPB_LOCK_CONNECTION:
        case IOCTL_SPB_UNLOCK_CONNECTION:
        case IOCTL_SPB_EXECUTE_SEQUENCE:
        {
            status = STATUS_INVALID_PARAMETER;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_APIERROR,
                "Transfer list for SpbRequest %p cannot be captured for "
                "internally supported SPB IOCTLs - %!STATUS!",
                SpbRequest,
                status
                );
            WdfVerifierDbgBreakPoint();
            goto exit;
        }

        default:
        {
            //
            // Allow all other IOCTLs
            //

            break;
        }
    }

    //
    // Create the request
    //

    PCScxTarget target = GetTargetFromFxFile(
                            WdfRequestGetFileObject(SpbRequest)
                            );

    status = CScxRequest::_CreateAndInitialize(
                            target,
                            SpbRequest,
                            ScxRequestFlagsUseIoOtherTransferList,
                            &request
                            );

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_ERROR, 
            TRACE_FLAG_APIERROR,
            "Unable to allocate context for request %p to target "
            "%p - %!STATUS!", 
            SpbRequest,
            target->GetFxObject(),
            status
            );
        goto exit;
    }

exit:

    return status;
}

