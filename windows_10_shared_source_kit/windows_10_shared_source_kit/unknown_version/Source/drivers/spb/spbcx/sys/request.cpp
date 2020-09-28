/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    request.cpp

Abstract:

    This module contains the code for SPB requests

Environment:

    kernel-mode only

Revision History:

--*/

#include "internal.h"
#include "request.tmh"


NTSTATUS
CScxRequest::_CreateAndInitialize(
    __in  PCScxTarget        Target,
    __in  WDFREQUEST         FxRequest,
    __in  SCX_REQUEST_FLAGS  RequestFlags,
    __out PCScxRequest      *Request
    )
/*++

  Routine Description:

    This is the factory method for CScxRequest, which lives as a context
    attached to any request sent to an SPB target.  This creates the object
    and then initializes it based on the supplied request object.

  Arguments:

    Target - the target which the request has been sent to

    FxRequest - the WDFREQUEST the CScxRequest is attached to

    RequestFlags - special case flags for the request

    Request - a location to store a pointer to the new object

  Return Value:

    status

--*/
{
    PVOID context;
    PCScxRequest request = NULL;
    GUID activityId;
    bool tracingActive;

    NTSTATUS status;

    //
    // Write a transfer event from the underlying IRP to our request.
    //

    tracingActive = _WriteStartEvent(FxRequest,
                                     Target->GetController()->GetFxObject(),
                                     Target->GetFxObject(),
                                     Target->GetConnectionTag(),
                                     Target->GetScopeTag(),
                                     &activityId);

    //
    // If the client driver wants a context on each request then allocate
    // that.
    //

    if (Target->GetController()->GetDriverRequestAttributes() != NULL)
    {
        status = WdfObjectAllocateContext(
                    FxRequest,
                    Target->GetController()->GetDriverRequestAttributes(),
                    NULL
                    );

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_REQUEST,
                         "Failure adding driver request context to WDFREQUEST "
                         "%p (target ConnectionTag %S) - %!STATUS!",
                         FxRequest,
                         Target->GetConnectionTag(),
                         status);

            goto exit;
        }
    }

    //
    // Construct the request object in the appropriate context
    //

    {
        context = (PVOID) GetRequestFromFxRequest(FxRequest);

        request = new (context) CScxRequest(Target->GetController(),
                                            Target,
                                            FxRequest,
                                            RequestFlags,
                                            (tracingActive ? &activityId :
                                                             NULL));

        if (request == NULL)
        {
            NT_ASSERTMSG("Request object context shouldn't be NULL", request != NULL);

            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error constructing request object context - %!STATUS!",
                status);
            goto exit;
        }
    }

    //
    // Parse the FxRequest and initialize the new type
    //

    status = request->CaptureFxRequest();

    if (!NT_SUCCESS(status))
    {
        //
        // Just return on error.  The called function logs the error and the
        // caller will complete the request.
        //

        goto exit;
    }

    //
    // Success.
    //

    *Request = request;

exit:

    if (!NT_SUCCESS(status))
    {
        //
        // End the event if we aren't going to be able to process the request.
        //

        if (tracingActive)
        {
            _WriteStopEvent(&activityId, request);
        }
    }

    return status;
}

SPB_REQUEST_TYPE
CScxRequest::GetRequestType(
    __in PWDF_REQUEST_PARAMETERS RequestParameters
    )
{
    SPB_REQUEST_TYPE type = SpbRequestTypeUndefined;

    //
    // Initialize the request type and save the request parameters.
    //

    switch (RequestParameters->Type)
    {
        case WdfRequestTypeRead:
        {
            type = SpbRequestTypeRead;
            break;
        }

        case WdfRequestTypeWrite:
        {
            type = SpbRequestTypeWrite;
            break;
        }

        case WdfRequestTypeDeviceControl:
        case WdfRequestTypeDeviceControlInternal:
        {
            switch(RequestParameters->Parameters.DeviceIoControl.IoControlCode)
            {
                case IOCTL_SPB_LOCK_CONTROLLER:
                {
                    type = SpbRequestTypeLockController;
                    break;
                }

                case IOCTL_SPB_UNLOCK_CONTROLLER:
                {
                    type = SpbRequestTypeUnlockController;
                    break;
                }
                case IOCTL_SPB_LOCK_CONNECTION:
                {
                    type = SpbRequestTypeLockConnection;
                    break;
                }

                case IOCTL_SPB_UNLOCK_CONNECTION:
                {
                    type = SpbRequestTypeUnlockConnection;
                    break;
                }

                case IOCTL_SPB_EXECUTE_SEQUENCE:
                {
                    type = SpbRequestTypeSequence;
                    break;
                }

                default:
                {
                    type = SpbRequestTypeOther;
                    break;
                }
            }
        }
    }

    return type;
}

NTSTATUS
CScxRequest::CaptureFxRequest(
    VOID
    )
/*++

  Routine Description:

    This method captures parameters from the FxRequest into the
    request instance.  If called in caller's context this will
    include capturing any user-mode I/O buffers which a sequence
    IOCTL may contain.

  Parameters:

    None

  Return Value:

    status

--*/
{
    SPB_REQUEST_TYPE type = SpbRequestTypeUndefined;

    WDF_REQUEST_PARAMETERS fxParams;

    PIRP irp = WdfRequestWdmGetIrp(GetFxObject());

    SPB_TRANSFER_LIST_ENTRY defaultTd = {};

    //
    // Initialize the request type and save the request parameters.
    //

    WDF_REQUEST_PARAMETERS_INIT(&fxParams);

    WdfRequestGetParameters(GetFxObject(), &fxParams);

    switch (fxParams.Type)
    {
        case WdfRequestTypeRead:
        {
            type = SpbRequestTypeRead;

            if (irp->MdlAddress != NULL)
            {
                defaultTd = SPB_TRANSFER_LIST_ENTRY_INIT_MDL(
                                SpbTransferDirectionFromDevice,
                                0,
                                irp->MdlAddress
                                );
            }
            else
            {
                defaultTd = SPB_TRANSFER_LIST_ENTRY_INIT_NON_PAGED(
                                SpbTransferDirectionFromDevice,
                                0,
                                irp->AssociatedIrp.SystemBuffer,
                                (ULONG) fxParams.Parameters.Read.Length
                                );
            }

            break;
        }

        case WdfRequestTypeWrite:
        {
            type = SpbRequestTypeWrite;

            if (irp->MdlAddress != NULL)
            {
                defaultTd = SPB_TRANSFER_LIST_ENTRY_INIT_MDL(
                                SpbTransferDirectionToDevice,
                                0,
                                irp->MdlAddress
                                );
            }
            else
            {
                defaultTd = SPB_TRANSFER_LIST_ENTRY_INIT_NON_PAGED(
                                SpbTransferDirectionToDevice,
                                0,
                                irp->AssociatedIrp.SystemBuffer,
                                (ULONG) fxParams.Parameters.Write.Length
                                );
            }

            break;
        }

        case WdfRequestTypeDeviceControl:
        case WdfRequestTypeDeviceControlInternal:
        {
            switch(fxParams.Parameters.DeviceIoControl.IoControlCode)
            {
                case IOCTL_SPB_LOCK_CONTROLLER:
                {
                    type = SpbRequestTypeLockController;
                    break;
                }

                case IOCTL_SPB_UNLOCK_CONTROLLER:
                {
                    type = SpbRequestTypeUnlockController;
                    break;
                }
                case IOCTL_SPB_LOCK_CONNECTION:
                {
                    type = SpbRequestTypeLockConnection;
                    break;
                }

                case IOCTL_SPB_UNLOCK_CONNECTION:
                {
                    type = SpbRequestTypeUnlockConnection;
                    break;
                }

                case IOCTL_SPB_EXECUTE_SEQUENCE:
                {
                    type = SpbRequestTypeSequence;
                    break;
                }

                default:
                {
                    if (m_Controller->DriverSupportsOtherIo())
                    {
                        type = SpbRequestTypeOther;
                    }
                    break;
                }
            }
        }
    }

    //
    // If we didn't determine an SPB I/O type then tell the caller to fail
    // the request.
    //

    if (type == SpbRequestTypeUndefined)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Request %p to controller %p, target %p (ConnectionTag %S) is of "
            "unsupported request type %!WDF_REQUEST_TYPE! - %!STATUS!",
            GetFxObject(),
            GetController()->GetFxObject(),
            GetTarget()->GetFxObject(),
            GetTarget()->GetConnectionTag(),
            fxParams.Type,
            STATUS_INVALID_DEVICE_REQUEST
            );

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // If the driver doesn't support lock and unlock then tell the caller to fail
    // the request.
    //

    if ((type == SpbRequestTypeLockController) ||
        (type == SpbRequestTypeUnlockController))
    {
        if (GetController()->DriverSupportsLockUnlock() == false)
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Request %p to controller %p, target %p (ConnectionTag %S) is of "
                "unsupported request type %!WDF_REQUEST_TYPE! - %!STATUS!",
                GetFxObject(),
                GetController()->GetFxObject(),
                GetTarget()->GetFxObject(),
                GetTarget()->GetConnectionTag(),
                fxParams.Type,
                STATUS_NOT_SUPPORTED
                );

            return STATUS_NOT_SUPPORTED;
        }
    }

    //
    // Capture the remaining request parameters (if needed)
    //

    if ((type == SpbRequestTypeSequence) ||
        ((type == SpbRequestTypeOther) &&
            ((GetRequestFlags() & ScxRequestFlagsUseIoOtherTransferList) > 0)))
    {
        NTSTATUS status;

        status = CaptureTransferListIoctl();

        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }
    else if ((type == SpbRequestTypeRead) ||
             (type == SpbRequestTypeWrite))
    {
#if DBG
        NTSTATUS status;

        status =
#endif
#pragma prefast(suppress:__WARNING_MUST_USE, "TD capture will not fail for read or write request")
        CaptureTransferList(&defaultTd, 1, KernelMode);

        NT_ASSERTMSG("Capture of a TD should never fail for read or write",
                     NT_SUCCESS(status));
    }

    //
    // Save the request type in the request.
    //

    m_RequestType = type;

    TraceMessage(
        TRACE_LEVEL_VERBOSE,
        TRACE_FLAG_REQUEST,
        "Request %p to controller %p, target %p (ConnectionTag %S) is of "
        "type %!SPB_REQUEST_TYPE!",
            GetFxObject(),
            GetController()->GetFxObject(),
            GetTarget()->GetFxObject(),
            GetTarget()->GetConnectionTag(),
            type
            );

    return STATUS_SUCCESS;
}

NTSTATUS
CScxRequest::CaptureTransferListIoctl(
    VOID
    )
/*++

  Routine Description:

    This routine captures the additional buffers associated with an
    I/O control formatted with an SPB transfer list and fills in the
    request's transfer descriptor list.

  Arguments:

    None

  Return Value:

    status

--*/
{
    WDF_REQUEST_PARAMETERS fxParams;
    PSPB_TRANSFER_LIST list;
    size_t listCb;

    NTSTATUS status;

    //
    // Verify output buffer length is 0.
    //

    WDF_REQUEST_PARAMETERS_INIT(&fxParams);
    WdfRequestGetParameters(GetFxObject(), &fxParams);

    if (fxParams.Parameters.DeviceIoControl.OutputBufferLength != 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error handling transfer list IOCTL request %p: "
            "expected output buffer length to be 0 - %!STATUS!",
            GetFxObject(),
            status
            );
        goto exit;
    }

    //
    // Fetch the input buffer for this request.
    //

    status = WdfRequestRetrieveInputBuffer(
                GetFxObject(),
                sizeof(SPB_TRANSFER_LIST),
                (PVOID *) &list,
                &listCb
                );

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error handling transfer list IOCTL request %p: "
            "unable to retrieve input buffer - %!STATUS!",
            GetFxObject(),
            status
            );
        goto exit;
    }

    //
    // Validate the packet structure.
    //

    if (list->Size != sizeof(SPB_TRANSFER_LIST))
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error validating transfer list IOCTL request %p: "
            "Size is %d, not sizeof(SPB_PACKET) (%d) - %!STATUS!",
            GetFxObject(),
            list->Size,
            sizeof(SPB_TRANSFER_LIST),
            status
            );
        goto exit;
    }

    //
    // Verify that the flags are 0
    //

    if (list->Reserved != 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error validating transfer list IOCTL request %p: "
            "Reserved is %d, not 0 - %!STATUS!",
            GetFxObject(),
            list->Reserved,
            status
            );
        goto exit;
    }

    //
    // Transfer count must be 1 <= x <= (number of TD's that can fit
    // in the input buffer)
    //

    if (list->TransferCount < 1)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error validating transfer list IOCTL request %p: "
            "TransferCount is < 1 - %!STATUS!",
            GetFxObject(),
            status
            );
        goto exit;
    }

    ULONG maxTransferCount = (ULONG)
            ((listCb - FIELD_OFFSET(SPB_TRANSFER_LIST, Transfers)) /
             sizeof(SPB_TRANSFER_LIST_ENTRY));

    if (list->TransferCount > maxTransferCount)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error validating transfer list IOCTL request %p: "
            "TransferCount is too large (%d, max is %d) - %!STATUS!",
            GetFxObject(),
            list->TransferCount,
            maxTransferCount,
            status
            );
        goto exit;
    }

    KPROCESSOR_MODE previousMode = WdfRequestGetRequestorMode(GetFxObject());

    status = CaptureTransferList(
                list->Transfers,
                list->TransferCount,
                previousMode
                );

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

exit:

    return status;
}

NTSTATUS
CScxRequest::CaptureTransferList(
    __in_ecount(DescriptorsCe) SPB_TRANSFER_LIST_ENTRY Descriptors[],
    __in                       ULONG                   DescriptorsCe,
    __in                       KPROCESSOR_MODE         CaptureMode
    )
/*++

  Routine Description:

    This routine captures a set of transfer descriptors and converts them into
    an array of SCX_TRANSFER_DESCRIPTORS stored in the request.  If the number
    of descriptors is small enough, this routine will use the preallocated
    descriptor array in the request.

  Arguments:

    Descriptors - the array of descriptors from the request.  This is in
                  kernel memory.

    DescriptorsCe - the number of descriptors.

    CaptureMode - indicates whether any secondary buffers in the the TD's
                  should be treated as user-mode or kernel-mode buffers.

  Return Value:

    status

--*/
{
    NTSTATUS status;

    //
    // Set the transfer count and, if there's more than one TD, allocate an
    // array of them.
    //

    if (DescriptorsCe > SCX_PREALLOCATED_TD_COUNT)
    {
        ULONG transferDescriptorsCb;

        status = RtlULongMult(sizeof(SCX_TRANSFER_DESCRIPTOR),
                              DescriptorsCe,
                              &transferDescriptorsCb);

        if (!NT_SUCCESS(status))
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "arithmetic overflow allocating transfer descriptors %d - %!STATUS!",
                GetFxObject(),
                DescriptorsCe,
                status
                );
            goto exit;
        }

        m_TransferDescriptors =
            (PSCX_TRANSFER_DESCRIPTOR) ExAllocatePoolWithTag(
                                            NonPagedPoolNx,
                                            transferDescriptorsCb,
                                            SCX_POOL_TAG
                                            );

        if (m_TransferDescriptors == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error handling IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "could not allocate %d transfer descriptors - %!STATUS!",
                GetFxObject(),
                DescriptorsCe,
                status
                );
            goto exit;
        }

        //
        // Note: zeroing is a little inefficient, but it makes for an easier
        //       time in cleanup figuring out how many valid TD's there are.
        //       Tracking this with a count of valid TD's would be more
        //       efficient.
        //
        RtlZeroMemory(m_TransferDescriptors, transferDescriptorsCb);
    }
    else
    {
        m_TransferDescriptors = m_PreallocatedTDs;
    }

    //
    // Validate and capture each transfer descriptor.
    //

    for (ULONG i = 0; i < DescriptorsCe; i += 1)
    {
        status = CaptureTransferListEntry(
                    &(Descriptors[i]),
                    CaptureMode,
                    i,
                    &m_TotalCb,
                    &(m_TransferDescriptors[i])
                    );

        if (!NT_SUCCESS(status))
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "failure to capture transfer descriptor %d - %!STATUS!",
                GetFxObject(),
                i,
                status
                );
            goto exit;
        }

        m_TransferCount += 1;
    }

    //
    // Success
    //

    status = STATUS_SUCCESS;

exit:

    return status;
}


NTSTATUS
CScxRequest::CaptureTransferListEntry(
    __in    PSPB_TRANSFER_LIST_ENTRY  Source,
    __in    KPROCESSOR_MODE           CaptureMode,
    __in    ULONG                     Index,
    __inout size_t                   *TotalCb,
    __out   SCX_TRANSFER_DESCRIPTOR  *Destination
    )
/*++

  Routine Description:

    This routine captures the source transfer descriptor, validates it and
    uses it to initialize the destination transfer descriptor.

  Arguments:

    Source - the source TD

    CaptureMode - the mode (user/kernel) we should use for capture.  Validation
                  in a kernel-mode capture is minimal

    Index - the index of this TD for the request.  Used for debug messages.

    TotalCb - a location to accumulate the total length of this transfer

    Destination - the destination TD

  Return Value:

    status

--*/
{
#if DBG
    KPROCESSOR_MODE previousMode = WdfRequestGetRequestorMode(GetFxObject());
#endif

    ULONG bufferCb = 0;

    NTSTATUS status = STATUS_SUCCESS;

    //
    // Validate the transfer descriptor from the client
    //

    status = ValidateClientTransferListEntry(Source, Index);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    if (CaptureMode == UserMode)
    {
        //
        // Don't allow non-paged or MDL transfers if the client is user-mode
        // (as it is in this validate path)
        //

        if ((Source->Buffer.Format == SpbTransferBufferFormatSimpleNonPaged) ||
            (Source->Buffer.Format == SpbTransferBufferFormatMdl))
        {
            status = STATUS_INVALID_PARAMETER;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "transfer descriptor %d BufferFormat value "
                "(%!SPB_TRANSFER_BUFFER_FORMAT!) is only for use by "
                "kernel-mode clients - %!STATUS!",
                GetFxObject(),
                Index,
                Source->Buffer.Format,
                status
                );
            goto exit;
        }
    }

    //
    // Determine how big of an MDL we need.  If the required MDL is small
    // enough then use the preallocated one in the TD.
    //

    if ((Source->Buffer.Format == SpbTransferBufferFormatSimple) ||
        (Source->Buffer.Format == SpbTransferBufferFormatSimpleNonPaged))
    {
        SPB_TRANSFER_BUFFER_LIST_ENTRY entry = {Source->Buffer.Simple.Buffer,
                                                Source->Buffer.Simple.BufferCb};
        SCX_TRANSFER_DESCRIPTOR_FLAGS flags;

        status = BuildMdlForBufferEntry(
                    &entry,
                    CaptureMode,
                    Source->Direction,
                    &(Destination->PreallocatedMdl.Mdl),
                    SCX_PREALLOCATED_REQUEST_PAGE_COUNT,
                    &(Destination->Mdl),
                    &flags
                    );

        Destination->Flags |= flags;
    }
    else if (Source->Buffer.Format == SpbTransferBufferFormatList)
    {
        SCX_TRANSFER_DESCRIPTOR_FLAGS flags;

        status = BuildMdlForBufferList(
                    Source->Buffer.BufferList.List,
                    Source->Buffer.BufferList.ListCe,
                    CaptureMode,
                    Source->Direction,
                    &(Destination->Mdl),
                    &flags
                    );

        Destination->Flags |= flags;
    }
    else
    {
        NT_ASSERTMSG("Should be a kernel-mode request at this point",
                     (previousMode == KernelMode));

        //
        // Caller provided an MDL.  Blindly use it.
        //

        Destination->Mdl = (PMDL) Source->Buffer.Mdl;

        Destination->Flags |= ScxTransferDescriptorFlagsMdlFromCaller;
    }

    //
    // Bail out if an error occurred getting the MDLs
    //

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    //
    // Compute the total transfer length from the MDLs.
    //

    for(PMDL mdl = Destination->Mdl; mdl != NULL; mdl = mdl->Next)
    {
        bufferCb += MmGetMdlByteCount(mdl);
    }

    //
    // Now add the total length back in.
    //

    status = RtlSizeTAdd(*TotalCb, bufferCb, TotalCb);

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Transfer length overflow processing SPB PACKET IOCTL "
            "request %p on transfer descriptor %d (length %d) - "
            "%!STATUS!",
            GetFxObject(),
            Index,
            bufferCb,
            status
            );
        goto exit2;
    }

    //
    // Copy the TD
    //

    Destination->InitializeFromClientTd(Source, bufferCb);

exit2:

    if (!NT_SUCCESS(status))
    {
        //
        // MDL failed during locking, so no need to unlock.
        //

        CleanupTransferDescriptor(Destination);
    }

exit:

    return status;
}

NTSTATUS
CScxRequest::ValidateClientTransferListEntry(
    __in PSPB_TRANSFER_LIST_ENTRY Source,
    __in ULONG                    Index
    )
{
    NTSTATUS status;

    //
    // Validate transfer direction
    //

    if ((Source->Direction <= SpbTransferDirectionNone) ||
        (Source->Direction >= SpbTransferDirectionMax))
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_REQUEST,
            "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
            "transfer descriptor %d Direction value "
            "(%!SPB_TRANSFER_DIRECTION!) is invalid - %!STATUS!",
            GetFxObject(),
            Index,
            Source->Direction,
            status
            );
        goto exit;
    }

    //
    // Check for a valid buffer format & non-null buffer.
    //

    switch(Source->Buffer.Format)
    {
        case SpbTransferBufferFormatSimple:
        case SpbTransferBufferFormatSimpleNonPaged:
        {
            if ((Source->Buffer.Simple.Buffer == NULL) ||
                (Source->Buffer.Simple.BufferCb == 0))
            {
                status = STATUS_INVALID_PARAMETER;
                TraceMessage(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_REQUEST,
                    "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                    "transfer descriptor %d NULL or zero length buffer not "
                    "allowed - %!STATUS!",
                    GetFxObject(),
                    Index,
                    status
                    );
                goto exit;
            }
            break;
        }

        case SpbTransferBufferFormatMdl:
        {
            if (Source->Buffer.Mdl == NULL)
            {
                status = STATUS_INVALID_PARAMETER;
                TraceMessage(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_REQUEST,
                    "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                    "transfer descriptor %d NULL DDL not allowed - "
                    "%!STATUS!",
                    GetFxObject(),
                    Index,
                    status
                    );
                goto exit;
            }
            break;
        }

        case SpbTransferBufferFormatList:
        {
            if ((Source->Buffer.BufferList.List == NULL) ||
                (Source->Buffer.BufferList.ListCe == 0))
            {
                status = STATUS_INVALID_PARAMETER;
                TraceMessage(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_REQUEST,
                    "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                    "transfer descriptor %d NULL or 0 length list not allowed "
                    "- %!STATUS!",
                    GetFxObject(),
                    Index,
                    status
                    );
                goto exit;
            }
            break;
        }

        case SpbTransferBufferFormatMax:
        case SpbTransferBufferFormatInvalid:
        default:
        {
            status = STATUS_INVALID_PARAMETER;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "transfer descriptor %d BufferFormat value "
                "(%!SPB_TRANSFER_BUFFER_FORMAT!) is invalid - %!STATUS!",
                GetFxObject(),
                Index,
                Source->Buffer.Format,
                status
                );
            goto exit;
        }
    }

    //
    // Success.
    //

    status = STATUS_SUCCESS;

exit:

    return status;
}

VOID
CScxRequest::CompleteRequest(
    __in     NTSTATUS   Status,
    __in_opt ULONG_PTR *Information
    )
{
    if (ARGUMENT_PRESENT(Information))
    {
        WdfRequestCompleteWithInformation(GetFxObject(), Status, *Information);

    }
    else
    {
        WdfRequestComplete(GetFxObject(), Status);
    }
}

VOID
CScxRequest::GetParameters(
    __inout SPB_REQUEST_PARAMETERS *Parameters
    )
/*++

  Routine Description:

    This routine retrieves the parameters for the specified request.

  Arguments:

    Parameters - a location to store the parameters.  the size member
                 should be properly initialized by the caller.

  Return Value:

    none

--*/
{
    Parameters->Type = m_RequestType;

    Parameters->Position = m_Position;

    Parameters->PreviousTransferDirection = m_LastDirection;

    Parameters->Length = m_TotalCb;

    Parameters->SequenceTransferCount = m_TransferCount;

}

VOID
CScxRequest::OnCleanup(
    VOID
    )
/*++

  Routine Description:

    This method is invoked when an SPBREQUEST object is cleaned up during the
    process of i/o completion.  It is responsible for restarting the target
    queue if necessary.

    TODO: To handle sequence decomposition this either needs to be replaced
          with an internal completion routine (which would allow us to reuse
          the request) or sequence should be decomposed into driver-created
          requests handed directly to the controller driver.

    TODO: For handling failure on lock operations, this needs to be replaced
          in a similar manner.  Currently this routine assumes lock operations
          succeed

  Arguments:

    None

  Return Value:

    None

--*/
{
    //
    // If the request hasn't yet been constructed, we have nothing to do.
    //

    if (IsConstructed() == false)
    {
        return;
    }

    //
    // If the request is the active request for the target, then notify the
    // target it is being completed.
    //

    if (m_IsTargetActiveRequest == true)
    {
        m_Target->OnActiveRequestCompletion(this);
    }

    //
    // Write an end event for this request.
    //

    _WriteStopEvent(&m_ActivityId, this);
}

CScxRequest::~CScxRequest(
    VOID
    )
{
    if (m_TransferCount > 0)
    {
        CleanupTransferDescriptors();
    }

    if ((m_TransferDescriptors != m_PreallocatedTDs) &&
        (m_TransferDescriptors != NULL))
    {
        ExFreePool(m_TransferDescriptors);
        m_TransferDescriptors = NULL;
    }
}

VOID
CScxRequest::CleanupTransferDescriptors(
    VOID
    )
/*++

  Routine Description:

    This routine cleans up any transfer descriptors associated with this
    request

  Arguments:

    None

  Return Value:

    None

--*/
{
    //
    // Kernel-mode requests just clone the MDLs or buffers from the client
    // There is nothing extra to free.
    //

    for(ULONG i = 0; i < m_TransferCount; i += 1)
    {
        CleanupTransferDescriptor(&(m_TransferDescriptors[i]));
    }
}

VOID
CScxRequest::CleanupTransferDescriptor(
    __inout PSCX_TRANSFER_DESCRIPTOR Desc
    )
/*++

  Routine Description:

    This routine cleans up a specific transfer descriptor.  This includes
    cleanup of the MDL.

  Arguments:

    Desc - the descriptor to be cleaned up

  Return Value:

    None

--*/
{
    EventWrite_SPBCX_IO_DESC_DIRECTION(&m_ActivityId, (UCHAR)Desc->SpbDescriptor.Direction);

    //
    // Clean up the MDL if need be.
    //

    if ((Desc->Flags & ScxTransferDescriptorFlagsMdlFromCaller) == 0)
    {
        CleanupMdlChain(Desc->Mdl, Desc->Flags);
    }

    Desc->Flags &= ~(ScxTransferDescriptorFlagsMdlAllocated |
                     ScxTransferDescriptorFlagsMdlLocked |
                     ScxTransferDescriptorFlagsMdlFromCaller);

    //
    // Poison the MDL pointer.
    //

    Desc->Mdl = NULL;
}

VOID
CScxRequest::CleanupMdlChain(
    __in_opt PMDL                          MdlChain,
    __in     SCX_TRANSFER_DESCRIPTOR_FLAGS Flags
    )
/*++

  Routine Description:

    Helper routine to unlock and free an MDL chain.  This routine can be
    safely called with a NULL MDL chain.

  Arguments:

    MdlChain - the chain to clean up

    Flags - indicates whether the caller has locked pages on the chain,
            and whether the caller has allocated an MDL or is using the
            preallocated MDL

  Return Value:

    None

--*/
{
    //
    // For each chunk of the MDL.
    //

    while (MdlChain != NULL)
    {
        PMDL mdl = MdlChain;

        //
        // Advance the chain.
        //

        MdlChain = mdl->Next;

        //
        // Unlock pages if necessary.
        //

        if ((Flags & ScxTransferDescriptorFlagsMdlLocked) != 0)
        {
            MmUnlockPages(mdl);
        }

        //
        // Free the MDL if necessary.
        //

        if ((Flags & ScxTransferDescriptorFlagsMdlAllocated) != 0)
        {
            IoFreeMdl(mdl);
        }
        else
        {
            NT_ASSERTMSG("Don't expect an MDL chain to use the preallocated \
                         MDL",
                         (MdlChain == NULL));
            MmPrepareMdlForReuse(mdl);
        }
    }
}

VOID
CScxRequest::GetTransferParameters(
    __in      ULONG                    Index,
    __out_opt SPB_TRANSFER_DESCRIPTOR *TransferDescriptor,
    __out_opt PMDL                    *TransferBuffer
    )
/*++

  Routine Description:

    This routine returns the specified transfer descriptor for the
    request and (optionally) the MDL for the transfer buffer.

  Arguments:

    Index - the index of the TD.  The caller is responsible for ensuring
            the index is valid for the request.

    TransferDescriptor - A location to store the transfer descriptor if the
                         caller requests it.

    TransferBuffer - a location to store the MDL for the transfer descriptor
                     if the caller requests it

  Return Value:

    None

--*/
{
    NT_ASSERTMSG("Invalid transfer descriptor index",
                 (Index < GetTransferCount()));

    PSCX_TRANSFER_DESCRIPTOR td = &(m_TransferDescriptors[Index]);

    if (ARGUMENT_PRESENT(TransferDescriptor))
    {
        td->CopyToDriverTd(TransferDescriptor);
    }

    if (ARGUMENT_PRESENT(TransferBuffer))
    {
        *TransferBuffer = td->Mdl;
    }
}


NTSTATUS
CScxRequest::BuildMdlForBufferEntry(
    __in        PSPB_TRANSFER_BUFFER_LIST_ENTRY  Source,
    __in        KPROCESSOR_MODE                  CaptureMode,
    __in        SPB_TRANSFER_DIRECTION           Direction,
    __inout_opt PMDL                             PreallocatedMdl,
    __in_opt    ULONG                            PreallocatedMdlPfnCount,
    __out       PMDL                            *Mdl,
    __out       SCX_TRANSFER_DESCRIPTOR_FLAGS   *Flags
    )
/*++

  Routine Description:

    This routine builds an MDL for a specific buffer-list entry.

  Arguments:

    Source - the source entry.  This structure must be in kernel memory.

    CaptureMode - are the buffers from user-mode or kernel-mode?

    Direction - the direction of the transfer ... used for probing the source
                buffer

    PreallocatedMdl - an pre-allocated MDL this routine can use if the buffer
                      is small enough

    PreallocatedMdlPfnCount - the number of entries in the PFN array for the
                              preallocated MDL.  The buffer cannot span more
                              than this many pages to use the preallocated
                              MDL

    Mdl - a location to store the built MDL.

    Flags - a location to store information about what the routine did

  Return Value:

    Status

--*/
{
    PMDL mdl;
    SCX_TRANSFER_DESCRIPTOR_FLAGS flags = (SCX_TRANSFER_DESCRIPTOR_FLAGS) 0;

    NTSTATUS status;

    //
    // Determine whether the reqeust will fit in the preallocated
    // MDL (if one was supplied.
    //

    if ((ARGUMENT_PRESENT(PreallocatedMdl)) &&
        (ADDRESS_AND_SIZE_TO_SPAN_PAGES(Source->Buffer, Source->BufferCb) <=
         PreallocatedMdlPfnCount))
    {
        mdl = PreallocatedMdl;
#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "IoAllocateMdl allocates memory, which is exactly what this path is trying to avoid")
        MmCreateMdl(mdl,
                    Source->Buffer,
                    Source->BufferCb);
    }
    else
    {
        //
        // Allocate the MDL.  Charge the client process for quota when
        // capturing for a user-mode request.
        //

        mdl = IoAllocateMdl(Source->Buffer,
                            Source->BufferCb,
                            false,
                            (CaptureMode == UserMode),
                            NULL);

        if (mdl == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "could not allocate MDL for transfer buffer - "
                "%!STATUS!",
                GetFxObject(),
                status
                );
            goto exit;
        }

        flags |= ScxTransferDescriptorFlagsMdlAllocated;
    }

    //
    // If the caller has a paged buffer then we need to probe and
    // lock it.  Otherwise we can initialize it as if it were from non-paged
    // pool
    //

    if (CaptureMode == UserMode)
    {
        bool mapped = false;

        __try
        {
            MmProbeAndLockPages(
                mdl,
                UserMode,
                ((Direction == SpbTransferDirectionFromDevice) ?
                 IoWriteAccess :
                 IoReadAccess)
                );
            mapped = true;
            status = STATUS_SUCCESS;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
        }

        if (mapped == false)
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "error probing transfer buffer (addr %p) - %!STATUS!",
                GetFxObject(),
                Source->Buffer,
                status
                );
            goto exit;
        }

        //
        // Record that we locked this MDL into memory.
        //

        flags |= ScxTransferDescriptorFlagsMdlLocked;
    }
    else
    {
        MmBuildMdlForNonPagedPool(mdl);
    }

    //
    // Success.
    //

    status = STATUS_SUCCESS;

exit:

    if (NT_SUCCESS(status))
    {
        *Mdl = mdl;
        *Flags = flags;
    }
    else if ((flags & ScxTransferDescriptorFlagsMdlAllocated) != 0)
    {
        IoFreeMdl(mdl);
    }

    return status;
}

NTSTATUS
CScxRequest::BuildMdlForBufferList(
    __in_ecount(ListCe) SPB_TRANSFER_BUFFER_LIST_ENTRY  List[],
    __in                ULONG                           ListCe,
    __in                KPROCESSOR_MODE                 CaptureMode,
    __in                SPB_TRANSFER_DIRECTION          Direction,
    __out               PMDL                           *Mdl,
    __out               SCX_TRANSFER_DESCRIPTOR_FLAGS  *Flags
    )
/*++

  Routine Description:

    This routine builds a chained MDL for a buffer list.

  Arguments:

    List - the list of buffers.  If CaptureMode is kernel-mode this must
           reside in kernel-memory.  If CaptureMode is user-mode this must
           reside in user-memory.

    ListCe - the number of entries in the list

    CaptureMode - the mode in which to capture/probe the buffers

    Direction - the direction of transfer (used when probing buffers)

    Mdl - a location to store the first MDL in the chain

    Flags - a location to store flags about the actions this routine
            has taken

  Return Value:

    status

--*/
{
    PMDL mdlChain = NULL;
    SCX_TRANSFER_DESCRIPTOR_FLAGS accumulatedFlags =
        (SCX_TRANSFER_DESCRIPTOR_FLAGS) 0;

    NTSTATUS status;

    NT_ASSERTMSG("ListCe must not be zero", (ListCe > 0));
    PREFAST_ASSUME(ListCe > 0);

    //
    // Probe the buffer list if necessary to make sure it's in valid
    // address space.
    //

    if (CaptureMode == UserMode)
    {
        SIZE_T bufferSize;

        status = RtlSIZETMult(
                     ListCe,
                     sizeof(SPB_TRANSFER_BUFFER_LIST_ENTRY),
                     &bufferSize
                     );

        if (!NT_SUCCESS(status))
        {
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error processing SPBREQUEST %p: Buffer size calculation overflow "
                "(length %d) - %!STATUS!",
                GetSpbObject(),
                ListCe,
                status);

            goto exit;
        }

        __try
        {
            ProbeForRead(List, bufferSize, 1);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();

            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error processing SPBREQUEST %p: Invalid buffer list address "
                "%p (length %d) - %!STATUS!",
                GetSpbObject(),
                List,
                ListCe,
                status
                );

            goto exit;
        }
    }

    PMDL *nextMdl = &mdlChain;

    for (ULONG i = 0; i < ListCe; i += 1)
    {
        SPB_TRANSFER_BUFFER_LIST_ENTRY tmp;
        SCX_TRANSFER_DESCRIPTOR_FLAGS flags;

        if (CaptureMode == UserMode)
        {
            //
            // Capture the buffer address and length from user-mode, but
            // beware of the list being invalidated.
            //

            __try
            {
                tmp = List[i];
            }
#pragma prefast(suppress:__WARNING_UNREACHABLE_CODE, "List is in user-mode. Access may throw an exception.")
            __except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();

                TraceMessage(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_REQUEST,
                    "Exception reading buffer list %p entry %d - "
                    "%!STATUS!",
                    List,
                    i,
                    status
                    );

                goto exit2;
            }
        }
        else
        {
            tmp = List[i];
        }

        //
        // Check for a non-null buffer.
        //

        if ((tmp.Buffer == NULL) ||
            (tmp.BufferCb == 0))
        {
            status = STATUS_INVALID_PARAMETER;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REQUEST,
                "Error validating IOCTL_SPB_EXECUTE_SEQUENCE request %p: "
                "buffer list entry %d NULL or zero length buffer not "
                "allowed - %!STATUS!",
                GetFxObject(),
                i,
                status
                );
            goto exit2;
        }

        //
        // Build an MDL for this entry and add it to the MDL chain
        //

        status = BuildMdlForBufferEntry(
                    &tmp,
                    CaptureMode,
                    Direction,
                    NULL,
                    0,
                    nextMdl,
                    &flags
                    );

        //
        // If an error occurs just break out of the loop.
        //

        if (!NT_SUCCESS(status))
        {
            goto exit2;
        }

        accumulatedFlags |= flags;

        //
        // Store the next MDL (if there is one) in the Next field of the
        // one we just generated.
        //

        nextMdl = &((*nextMdl)->Next);
    }

    //
    // Success
    //

    status = STATUS_SUCCESS;

exit2:

    if (NT_SUCCESS(status))
    {
        *Mdl = mdlChain;
        *Flags = accumulatedFlags;
    }
    else
    {
        CleanupMdlChain(mdlChain, accumulatedFlags);
    }

exit:
    return status;
}

_Success_(return)
bool
CScxRequest::_WriteStartEvent(
    __in  WDFREQUEST    FxRequest,
    __in  WDFDEVICE     FxDevice,
    __in  WDFFILEOBJECT FxFile,
    __in  PCWSTR        TargetName,
    __in  PCWSTR        ScopeTag,
    __out GUID         *ActivityId
    )
/*++

  Routine Description:

    This routine creates an activity ID for the current request and logs
    a start event if ETW tracing is enabled.

  Arguments:

    FxRequest - the request to create an activity ID for

    FxFile - denotes client (in case of multiple clients)

    ActivityId - a location to store the activity ID

  Return Value:

    true - tracing is enabled and the activity ID is valid.  In this case
           the caller is responsible for ensuring a stop event is written.

    false - tracing is not enabled.  The activity ID returned is not valid
            and no stop event is required.

--*/
{
    WDF_REQUEST_PARAMETERS fxParams;

    NTSTATUS status;

    if (ActivityId == NULL)
    {
        return FALSE;
    }

    //
    // If tracing is not enabled then return false.
    //

    if (MCGEN_ENABLE_CHECK(SPB_PROVIDER_Context,
                           EVENT_SPBCX_IO_RECEIVE) == false)
    {
        return false;
    }

    //
    // Get the IRP parameters so we can report the request type.
    //

    WDF_REQUEST_PARAMETERS_INIT(&fxParams);
    WdfRequestGetParameters(FxRequest, &fxParams);

    //
    // Generate the event.  This code is copied from the generated ETW wrappers
    //

    PVOID Controller = FxDevice;
    PVOID Request = FxRequest;
    PVOID File = FxFile;
    SPB_REQUEST_TYPE RequestType = CScxRequest::GetRequestType(&fxParams);
    ULONG Length = (ULONG) fxParams.Parameters.Read.Length;

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_pzzppcq];

    EventDataDescCreate(&EventData[0], &Controller, sizeof(PVOID));

    EventDataDescCreate(&EventData[1],
                        (TargetName != NULL) ? TargetName : L"NULL",
                        (TargetName != NULL) ? (ULONG)((wcslen(TargetName) + 1) * sizeof(WCHAR)) : (ULONG)sizeof(L"NULL"));

    EventDataDescCreate(&EventData[2],
                        (ScopeTag != NULL) ? ScopeTag : L"NULL",
                        (ScopeTag != NULL) ? (ULONG)((wcslen(ScopeTag) + 1) * sizeof(WCHAR)) : (ULONG)sizeof(L"NULL"));

    EventDataDescCreate(&EventData[3], &File, sizeof(PVOID));

    EventDataDescCreate(&EventData[4], &Request, sizeof(PVOID));

    EventDataDescCreate(&EventData[5], &RequestType, sizeof(const char));

    EventDataDescCreate(&EventData[6], &Length, sizeof(const unsigned int));

    //
    // See if we can get an activity ID from the IRP.
    //

    status = IoGetActivityIdIrp(WdfRequestWdmGetIrp(FxRequest),
                                ActivityId);

    //
    // If not found generate a new activity ID for this request.
    //

    if (!NT_SUCCESS(status))
    {
        EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID, ActivityId);
    }

    //
    // Write out the start event
    //

    EtwWrite(Microsoft_Windows_SPB_ClassExtensionHandle,
             &EVENT_SPBCX_IO_RECEIVE,
             ActivityId,
             ARGUMENT_COUNT_pzzppcq,
             EventData);

    return true;
}

VOID
CScxRequest::_WriteStopEvent(
    __in_opt PGUID        ActivityId,
    __in_opt PCScxRequest Request
    )
{
    //
    // Return if there provider isn't enabled.
    //

    if (MCGEN_ENABLE_CHECK(SPB_PROVIDER_Context,
                           EVENT_SPBCX_IO_END) == false)
    {
        return;
    }

    //
    // if we got an scx request object then clear the flags which
    // have it log a stop event during cleanup.
    //

    if (Request != NULL)
    {
        if (Request->m_LogStopEvent == false)
        {
            return;
        }
        Request->m_LogStopEvent = false;

        if (ARGUMENT_PRESENT(ActivityId) == false)
        {
            ActivityId = &(Request->m_ActivityId);
        }
    }

    EventWrite_SPBCX_IO_END(ActivityId);
}

VOID
CScxRequest::WritePayloadEvents(
    VOID
    )
/*++

  Routine Description:

    This routine logs a set of ETW events that describe the entire payload
    in this request, i.e., each transfer descriptor, and each buffer in the
    transfer descriptor. If the request is a type of "other" I/O that SpbCx
    does not recognize, the payload is not logged.

    N.B. Care must be taken to ensure that this method is externally synchronized.

--*/
{
    if (!IsConstructed())
    {
        return;
    }

    //
    // Log only if the start event was fired, which means an activity ID is available,
    // and the start payload event is enabled, which should mean that the other payload events
    // are enabled as well.
    //

    PGUID activityId = GetActivityId();

    if ((activityId == NULL) ||
        (!EventEnabledEVENT_SPBCX_IO_PAYLOAD_START()))
    {
        return;
    }

    if (!IsTransferListIo())
    {
        //
        // This is "other" I/O that SpbCx does not understand. We cannot make any assumptions
        // about the data in the request. So skip this.
        //

        return;
    }

    ULONG transferCount = GetTransferCount();

    EventWrite_SPBCX_IO_PAYLOAD_START(activityId, GetTotalCb(), transferCount);

    for (ULONG transferIndex = 0; transferIndex < transferCount; ++transferIndex)
    {
        PSCX_TRANSFER_DESCRIPTOR currentTd = &(m_TransferDescriptors[transferIndex]);

        EventWrite_SPBCX_IO_PAYLOAD_TD_START(activityId,
                                             transferIndex,
                                             currentTd->SpbDescriptor.Direction,
                                             currentTd->SpbDescriptor.TransferLength);

        ULONG bufferIndex = 0;
        for (PMDL currentMdl = currentTd->Mdl;
             currentMdl != NULL;
             ++bufferIndex, (currentMdl = currentMdl->Next))
        {
            //
            // The system address we may reserve here will be released when we clean up the MDLs in
            // CleanupTransferDescriptors()
            //

            PVOID systemAddress = MmGetSystemAddressForMdlSafe(currentMdl,
                                                               NormalPagePriority | MdlMappingNoExecute);

            if (systemAddress == NULL)
            {
                TraceMessage(
                    TRACE_LEVEL_WARNING,
                    TRACE_FLAG_REQUEST,
                    "Could not reserve system address for MDL."
                    );
            }

            EventWrite_SPBCX_IO_PAYLOAD_TD_BUFFER(
                activityId,
                bufferIndex,
                ((systemAddress != NULL) ? MmGetMdlByteCount(currentMdl) : 0),
                (PUCHAR)systemAddress);
        }

        EventWrite_SPBCX_IO_PAYLOAD_TD_STOP(activityId);
    }

    EventWrite_SPBCX_IO_PAYLOAD_STOP(activityId);
}
