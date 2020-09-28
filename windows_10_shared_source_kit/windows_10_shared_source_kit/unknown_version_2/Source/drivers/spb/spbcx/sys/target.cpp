/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    target.cpp

Abstract:

    This module contains the code for managing targets.

Environment:

    kernel-mode only

Revision History:

--*/

#include "internal.h"

#include "target.tmh"

const SPB_TRANSFER_DIRECTION CScxTarget::s_TypeToDirectionMap[] = {
    SpbTransferDirectionNone,       // SpbRequestTypeUndefined = 0,
    SpbTransferDirectionFromDevice, // SpbRequestTypeRead,
    SpbTransferDirectionToDevice,   // SpbRequestTypeWrite,
    SpbTransferDirectionNone,       // SpbRequestTypeSequence,
    SpbTransferDirectionNone,       // SpbRequestTypeLockController,
    SpbTransferDirectionNone,       // SpbRequestTypeUnlockController,
    SpbTransferDirectionNone,       // SpbRequestTypeLockConnection,
    SpbTransferDirectionNone,       // SpbRequestTypeUnlockConnection,
    SpbTransferDirectionNone,       // SpbRequestTypeOther
};

C_ASSERT(countof(CScxTarget::s_TypeToDirectionMap) == SpbRequestTypeMax);

const LPCWSTR CScxTarget::s_AcceptableScopeTags[] = {
    L"",
    L"ACPI",
};

#define MAX_TARGET_CONNECTION_ID_CCH 100

NTSTATUS
CScxTarget::_CreateAndInitialize(
    __in  PCScxController Controller,
    __in  WDFFILEOBJECT   FxFile,
    __in  PLARGE_INTEGER  ConnectionId,
    __in  LPWSTR          ScopeTag,
    __in  bool            UserModeClient,
    __in_opt PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER SpbConnection,
    __out PCScxTarget    *Target
    )
/*++

  Routine Description:

    This is the factory method for the target class.  This routine will
    create a new target object and return the object to the caller.

    The target is created within a context attached to the supplied FxFile
    object.  The caller should not delete the target - it will self destruct
    when the WDFFILEOBJECT is destroyed.

    The caller is expected to later call AttachTargetToController()
    in order to link the target into the controller's target list.

  Arguments:

    Controller - the controller this target is a child of

    FxFile - the file this target is associated with.

    ConnectionId - the ConnectionID for the target as supplied by the Resource Hub

    ScopeTag - the scope tag for the target as supplied in the file handle

    UserModeClient - is the client in user-mode or kernel-mode

    SpbConnection - is the SPB connection buffer (which contains the ACPI
        SPB descriptor). Note about buffer ownership:
        - On success, the supplied SPB connection buffer will be owned by this
          target. The caller should relinquish ownership (and thus should not
          free it). It will be freed when the target is destroyed.
       - On failure, the caller will continue to retain buffer ownership and
         will be responsible for freeing it.

    Target - a location to store the newly created target.

  Return Value:

    status

--*/
{
    PVOID context;
    PCScxTarget target = NULL;

    NTSTATUS status;

    //
    // If the driver wants a context then allocate it.
    //

    if (Controller->GetDriverTargetAttributes() != NULL)
    {
        status = WdfObjectAllocateContext(
                    FxFile,
                    Controller->GetDriverTargetAttributes(),
                    NULL
                    );

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_TARGET,
                         "Failure adding driver's target context to "
                         "WDFFILEOBJECT %p -"
                         "%!STATUS!",
                         FxFile,
                         status);
            goto exit;
        }
    }

    //
    // Retrieve the target context from the file object.
    //

    context = (PVOID) GetTargetFromFxFile(FxFile);

    target = new (context) CScxTarget(Controller,
                                      FxFile,
                                      UserModeClient);

    //
    // Store the ConnectionID
    //

    {
        //
        // Store the connection ID
        //

        target->m_ConnectionId = *ConnectionId;

        //
        // Create a string to store the connection ID
        //

        PWCHAR ConnectionTagBuffer;

#pragma prefast(suppress:__WARNING_MEMORY_LEAK, "ConnectionTagBuffer is freed below or in the descructor")
        ConnectionTagBuffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPoolNx,
                                                            RESOURCE_HUB_FILE_SIZE,
                                                            SCX_POOL_TAG
                                                            );
        if (ConnectionTagBuffer == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_TARGET,
                         "Failure allocating buffer for connection ID string - "
                         "%!STATUS!",
                         status);
            goto ConnectionTagExit;
        }

        status = RESOURCE_HUB_ID_TO_FILE_NAME(target->m_ConnectionId.LowPart,
                                              target->m_ConnectionId.HighPart,
                                              ConnectionTagBuffer);

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_TARGET,
                         "Failure stringing ConnectionID %llx - "
                         "%!STATUS!",
                         target->m_ConnectionId.QuadPart,
                         status);
            goto ConnectionTagExit;
        }

ConnectionTagExit:

        if (!NT_SUCCESS(status))
        {
            if (ConnectionTagBuffer != NULL)
            {
                ExFreePoolWithTag(ConnectionTagBuffer, SCX_POOL_TAG);
                ConnectionTagBuffer = NULL;
            }
            goto exit;
        }
        else
        {
            target->m_ConnectionTag = ConnectionTagBuffer;
        }

    }

    //
    // Create a string to store the scope tag
    //

    {
        LPWSTR ScopeTagBuffer;
        size_t ScopeTagSize = (wcslen(ScopeTag) * sizeof(WCHAR)) +
            sizeof(UNICODE_NULL);

#pragma prefast(suppress:__WARNING_MEMORY_LEAK, "ScopeTagBuffer is freed below or in the descructor")
        ScopeTagBuffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPoolNx,
                                                       ScopeTagSize,
                                                       SCX_POOL_TAG
                                                       );
        if (ScopeTagBuffer == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(TRACE_LEVEL_ERROR,
                            TRACE_FLAG_TARGET,
                            "Failure allocating buffer for scope tag - "
                            "%!STATUS!",
                            status);
            goto ScopeTagExit;
        }

        status = RtlStringCbCopyW(ScopeTagBuffer, ScopeTagSize, ScopeTag);

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_ERROR,
                            TRACE_FLAG_TARGET,
                            "Failure copying scope tag %ws - "
                            "%!STATUS!",
                            ScopeTag,
                            status);
            goto ScopeTagExit;
        }

    ScopeTagExit:

        if (!NT_SUCCESS(status))
        {
            if (ScopeTagBuffer != NULL)
            {
                ExFreePoolWithTag(ScopeTagBuffer, SCX_POOL_TAG);
                ScopeTagBuffer = NULL;
            }
            goto exit;
        }
        else
        {
            target->m_ScopeTag = ScopeTagBuffer;
        }
    }

    //
    // Set the target connection properties. If the caller supplied a
    // connection buffer, then use it. Otherwise, query it from the resource
    // hub.
    //

    {
        PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER OutputBuffer;

        if (ARGUMENT_PRESENT(SpbConnection) == FALSE)
        {
            status = CScxTarget::QueryDescriptorFromResourceHub(
                        Controller,
                        target->m_ConnectionId,
                        &OutputBuffer);
        }
        else
        {
            OutputBuffer = SpbConnection;
            status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(status))
        {
            goto exit;
        }
        else
        {

            NT_ASSERT(OutputBuffer->PropertiesLength >=
                          sizeof(PNP_SERIAL_BUS_DESCRIPTOR));

            NT_ASSERT(OutputBuffer->PropertiesLength >=
                        ((PPNP_SERIAL_BUS_DESCRIPTOR)
                            (OutputBuffer->ConnectionProperties))->Length);

            target->m_ConnectionProperties = OutputBuffer;
        }
    }

    //
    // Create the target queue.
    //

    {
        WDF_IO_QUEUE_CONFIG queueConfig;
        WDF_OBJECT_ATTRIBUTES queueAttributes;

        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchSequential);
        queueConfig.EvtIoDefault = _OnTargetIoDefault;
        queueConfig.PowerManaged = Controller->DriverPowerManaged();
        queueConfig.AllowZeroLengthRequests = false;
        queueConfig.Driver = WdfGetDriver();

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes,
                                                PCScxTarget);
        queueAttributes.ParentObject = FxFile;

        status = WdfIoQueueCreate(Controller->GetFxDevice(),
                                  &queueConfig,
                                  &queueAttributes,
                                  &(target->m_TargetQueue));

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_TARGET,
                         "Failure creating target queue for WDFFILEOBJECT %p "
                         "(connectionId %S) - %!STATUS!",
                         FxFile,
                         target->GetConnectionTag(),
                         status);
            goto exit;
        }

        *(_GetTargetFromFxQueue(target->m_TargetQueue)) = target;
    }

    //
    // On failure, release the SPB connection buffer ownership back to the
    // caller.
    //

exit:

    if (NT_SUCCESS(status))
    {
        TraceMessage(TRACE_LEVEL_VERBOSE,
                     TRACE_FLAG_TARGET,
                     "Initialized target context for WDFFILEOBJECT %p (connectionId %S)",
                     FxFile,
                     target->GetConnectionTag());
        *Target = target;
    }
    else if ((target != NULL) &&
             (target->m_ConnectionProperties == SpbConnection))
    {
        target->m_ConnectionProperties = NULL;
    }

    return status;
}

C_ASSERT(countof(SPB_TARGET_NAME_PREFIX) > 2);

NTSTATUS
CScxTarget::_ParseFileObjectName(
    __in  WDFFILEOBJECT   FxFile,
    __out PLARGE_INTEGER  ConnectionId,
    __out LPWSTR         *ScopeTag
    )
/*++

  Routine Description:

    This method checks the file object name to see if it's an SPB tag and, if
    so, returns the ConnectionID.  The returned value is a pointer into the
    original file name - the caller is responsible for copying it if needed.

  Arguments:

    FxFile - the file object being parsed

    Connection - a location to store the pointer to the connection ID.

    ScopeTag - a location to store the pointer to the scope tag

  Return Value:

    STATUS_NO_SUCH_FILE - there is no file or namespace

    STATUS_OBJECT_NAME_INVALID - the filename format is recognized, but either
                                 the scope tag is missing or invalid

    STATUS_SUCCESS - the file is in the SPB namespace and is a valid ConnectionID

--*/
{
    PUNICODE_STRING fileName;

    NTSTATUS status;

    //
    // Get the name of the file object.
    //

    fileName = WdfFileObjectGetFileName(FxFile);

    //
    // Check if the create is for a non-SPB object (indicated by no trailing
    // name) we just mark it e.g. it could be the Resource Hub trying
    // to open
    //

    if ((fileName == NULL) || (fileName->Length == 0))
    {
        status = STATUS_NO_SUCH_FILE;
        goto exit;
    }

    //
    // Retrieve the ConnectionID from the File Name
    //

    LARGE_INTEGER id;
    UNICODE_STRING tmp;
    LPWSTR tag;

    RtlCopyMemory(&tmp, fileName, sizeof(tmp));

    //
    // Ignore the leading "\\"
    //

    while (tmp.Length > 0 &&
           tmp.Buffer[0] == L'\\') {

        ++tmp.Buffer;
        tmp.Length -= sizeof(WCHAR);
        tmp.MaximumLength -= sizeof(WCHAR);
    }

    status = RESOURCE_HUB_ID_FROM_FILE_NAME_WITH_SUBPATH(tmp.Buffer,
                                                         &id,
                                                         &tag);

    if (!NT_SUCCESS(status))
    {
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_TARGET,
                     "Failed to retrieve resource hub ID from filename %wZ "
                     "for WDFFILEOBJECT %p - %!STATUS!.",
                     &tmp,
                     FxFile,
                     status);
        goto exit;
    }

    //
    // Does the filename contain an acceptable tag?
    //

    BOOLEAN tagFound = FALSE;

    if (tag == NULL)
    {
        status = STATUS_OBJECT_NAME_INVALID;
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_TARGET,
                     "Missing required scope tag for connectionID %llx for "
                     "for WDFFILEOBJECT %p - %!STATUS!.",
                     id.QuadPart,
                     FxFile,
                     status);
        goto exit;
    }

    for (ULONG i = 0; i < countof(s_AcceptableScopeTags); i++)
    {
        if (wcscmp(tag, s_AcceptableScopeTags[i]) == 0)
        {
            tagFound = TRUE;
            break;
        }
    }

    if (tagFound == FALSE)
    {
        status = STATUS_OBJECT_NAME_INVALID;
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_TARGET,
                     "Invalid tag %ws for connectionID %llx for "
                     "for WDFFILEOBJECT %p - %!STATUS!.",
                     tag,
                     id.QuadPart,
                     FxFile,
                     status);
        goto exit;
    }

    //
    // Return the ConnectionID and scope tag.
    //

    *ConnectionId = id;
    *ScopeTag = tag;

exit:
    return status;
}


NTSTATUS
CScxTarget::OnCreate(
    VOID
    )
/*++

  Routine Description:

    This method is invoked by the controller after successfully creating a
    new target object in response to a CREATE request.  It invokes any
    registered I/O controller callbacks and returns the result.

    If this fails the controller should unlink the target and fail the
    CREATE operation.

  Arguments:

    None

  Return Value:

    Status

--*/
{
    //
    // All communications with the client driver should be routed
    // through the controller's Invoke methods.
    //

    return m_Controller->InvokeSpbTargetConnect(this);
}

VOID
CScxTarget::OnCleanup(
    VOID
    )
/*++

  Routine Description:

    This method is invoked by the controller when a CLEANUP request is
    received for an open target.  If the target was opened by a kernel-mode
    client then this does nothing.  If it was opened by a user-mode client
    then it purges the target queue, which blocks any new I/O requests from
    coming in and cancels the pending ones.

    If the target currently has the controller locked that lock will be
    released during CLOSE handling.

  Arguments:

    None

  Return Value:

    None

--*/
{
    //
    // If the client was kernel-mode then do nothing.
    //

    if (m_HasUserModeClient == false)
    {
        TraceMessage(TRACE_LEVEL_INFORMATION,
                     TRACE_FLAG_TARGET,
                     "Ignoring cleanup for target %p (ConnectionTag %S) opened by "
                     "kernel-mode client",
                     GetSpbObject(),
                     GetConnectionTag());
        return;
    }

    TraceMessage(TRACE_LEVEL_INFORMATION,
                 TRACE_FLAG_TARGET,
                 "Received cleanup for target %p (ConnectionTag %S) - purging "
                 "target I/O queue.  Target is%s exclusive owner."
                 "Active request is %p",
                 GetSpbObject(),
                 GetConnectionTag(),
                 m_IsExclusiveControllerOwner ? " " : " not",
                 m_ActiveRequest ? m_ActiveRequest->GetFxObject() : NULL);


    //
    // Purge the target queue.  We do this asynchronously.  The CLOSE
    // won't come in until all active I/O has completed.
    //

    WdfIoQueuePurge(m_TargetQueue, NULL, NULL);

    //
    // Cancel the active I/O operations
    //

    if (m_ActiveRequest != NULL)
    {
        //
        // TODO: Synchronize with I/O completion and cancel any
        //       active request.
        //
    }

    return;
}


VOID
CScxTarget::OnClose(
    VOID
    )
/*++

  Routine Description:

    The controller invokes this method when receiving a CLOSE request for the
    open target.  This method releases the controller (if the target has it
    locked for exclusive access) and notifies the controller driver of the
    disconnect.

    The controller must call this method before removing the target from the
    list of active targets.

  Arguments:

    None

  Return Value:

    None

--*/
{
    TraceMessage(TRACE_LEVEL_INFORMATION,
                 TRACE_FLAG_TARGET,
                 "Received close for target %p (ConnectionTag %S).  "
                 "Target is %s exclusive owner.",
                 GetSpbObject(),
                 GetConnectionTag(),
                 m_IsExclusiveControllerOwner ? "" : "not");

    //
    // If the client is a kernel-mode client then close the I/O queue before
    // we process the close operation.
    //

    if (m_HasUserModeClient == false)
    {
        //
        // Purge the target queue.  We do this asynchronously.  The CLOSE
        // won't come in until all active I/O has completed.
        //

        TraceMessage(TRACE_LEVEL_INFORMATION,
                     TRACE_FLAG_TARGET,
                     "Purging queue for SPBTARGET %p",
                     GetSpbObject());

        WdfIoQueuePurgeSynchronously(m_TargetQueue);
    }

    if (m_IsExclusiveControllerOwner)
    {
        NTSTATUS status;

        TraceMessage(TRACE_LEVEL_INFORMATION,
                     TRACE_FLAG_TARGET,
                     "SPBTARGET %p is still locked - issuing unlock request",
                     GetSpbObject());

        //
        // HACK: Issue an UNLOCK IOCTL to the target while the close is
        //       pending.
        //
        //       This is a blocking call.
        //

        status = SendIoctlToSelf(IOCTL_SPB_UNLOCK_CONTROLLER);

#if DBG

        NT_ASSERTMSG("Failure to unlock target during close.  No remedy for \
                     this until WDF allows us to create a request to forward \
                     to ourself",
                     NT_SUCCESS(status));

#else

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_INFORMATION,
                         TRACE_FLAG_TARGET,
                         "Attempt to unlock SPBTARGET %p during IRP_MJ_CLOSE "
                         "handling has failed - %!STATUS!."
                         "  This is presently an unrecoverable error",
                         GetSpbObject(),
                         status);

#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "bugcheck will be removed in M2 when dependent WDF feature work is complete")
            KeBugCheckEx((ULONG) STATUS_INSUFFICIENT_RESOURCES,
                         (ULONG_PTR) this->m_Controller->GetFxObject(),
                         (ULONG_PTR) this->GetFxObject(),
                         (ULONG) status,
                         0);
        }
#endif

    }

    if (m_IsExclusiveConnectionOwner)
    {
        NTSTATUS status;

        TraceMessage(TRACE_LEVEL_INFORMATION,
                     TRACE_FLAG_TARGET,
                     "SPBTARGET %p is still locked - issuing unlock connection request",
                     GetSpbObject());

        //
        // HACK: Issue an UNLOCK_CONNECTION IOCTL to the target while the close is
        //       pending.
        //
        //       This is a blocking call.
        //

        status = SendIoctlToSelf(IOCTL_SPB_UNLOCK_CONNECTION);

#if DBG

        NT_ASSERTMSG("Failure to unlock connection during close.  No remedy for \
                     this until WDF allows us to create a request to forward \
                     to ourself",
                     NT_SUCCESS(status));

#else

        if (!NT_SUCCESS(status))
        {
            TraceMessage(TRACE_LEVEL_INFORMATION,
                         TRACE_FLAG_TARGET,
                         "Attempt to unlock SPBTARGET %p during IRP_MJ_CLOSE "
                         "handling has failed - %!STATUS!."
                         "  This is presently an unrecoverable error",
                         GetSpbObject(),
                         status);

#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "bugcheck will be removed in M2 when dependent WDF feature work is complete")
            KeBugCheckEx((ULONG) STATUS_INSUFFICIENT_RESOURCES,
                         (ULONG_PTR) this->m_Controller->GetFxObject(),
                         (ULONG_PTR) this->GetFxObject(),
                         (ULONG) status,
                         0);
        }
#endif

    }

    //
    // Invoke the controller driver's target release callback
    //

    m_Controller->InvokeSpbTargetDisconnect(this);

    return;
}

VOID
CScxTarget::GetConnectionParameters(
    __inout SPB_CONNECTION_PARAMETERS *Parameters
    )
/*++

  Routine Description:

    This method retrieves the connection parameters for the target.

    The caller is responsible for verifying/ensuring that the Parameters
    structure has the appropriate size set in it.

  Arguments:

    Parameters - a location to store the connection parameters

  Return Value:

    None

--*/
{
    NT_ASSERTMSG("Caller did not set/validate correct size in Parameters",
                 (Parameters->Size >= sizeof(SPB_CONNECTION_PARAMETERS)));

    Parameters->Size = sizeof(SPB_CONNECTION_PARAMETERS);
    Parameters->ConnectionTag = GetConnectionTag();
    Parameters->ConnectionParameters = m_ConnectionProperties;

    return;
}

CScxTarget::~CScxTarget(
    VOID
    )
{
    if (m_ConnectionTag != NULL)
    {
        ExFreePoolWithTag(m_ConnectionTag, SCX_POOL_TAG);
        m_ConnectionTag = NULL;
    }

    if (m_ScopeTag != NULL)
    {
        ExFreePoolWithTag(m_ScopeTag, SCX_POOL_TAG);
        m_ScopeTag = NULL;
    }

    if ((m_ConnectionProperties != NULL))
    {
        ExFreePoolWithTag(m_ConnectionProperties,SCX_POOL_TAG);
        m_ConnectionProperties = NULL;
    }
}


VOID
CScxTarget::_OnTargetIoDefault(
    __in WDFQUEUE   FxQueue,
    __in WDFREQUEST FxRequest
    )
/*++

  Routine Description:

    This is the I/O handler for the target queue.

  Arguments:

    FxQueue - the queue the request was forwarded to

    FxRequest - the fx request.  The corresponding CScxRequest
                context may or not have been setup by the in-caller-context
                callback.

  Return Value:

    None

--*/
{
    PCScxTarget target = GetTargetFromTargetQueue(FxQueue);

    NT_ASSERTMSG("Target queue does not match target",
                 (FxQueue == target->m_TargetQueue));

    target->ProcessTargetRequest(FxRequest, true);
}

VOID
CScxTarget::ProcessTargetRequest(
    __in WDFREQUEST FxRequest,
    __in bool       StopTargetQueue
    )
/*++

  Routine Description:

    This routine takes an FxRequest and runs it through the target dispatch
    logic.  This includes stopping the target queue (if needed), setting the
    active request, and forwarding the request on to either the controller
    queue, the controller's dispatch routine or processing it as an unlock
    operation.

  Arguments:

    FxRequest - the WDF request to be processed.

    StopTargetQueue - set to true if this is a top level request that should
                      be synchronized with other requests.  False if this is
                      a subordinate request that can run under the auspices
                      of the current active request, or one sent during a time
                      when the queue is stopped (specifically when handling
                      a close)

  Return Value:

    none

--*/
{
    PCScxRequest request = GetRequestFromFxRequest(FxRequest);
    PCScxController controller = GetController();

    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

    //
    // See if the request has a constructed CScxRequest attached from
    // running through the in-caller-context callback.
    //
    // If not then construct the object.
    //

    NT_ASSERTMSG("request is in a pre-allocated context and should never be \
                 NULL",
                 (request != NULL));

    if (request->IsConstructed() == false)
    {
        status = CScxRequest::_CreateAndInitialize(
                                this,
                                FxRequest,
                                ScxRequestFlagsNone,
                                &request
                                );

        if (!NT_SUCCESS(status))
        {
            goto exit;
        }
    }

    //
    // Log that the request reached target dispatch.
    //

    EventWrite_SPBCX_IO_DISPATCH_TO_TARGET(request->GetActivityId());

    //
    // We should never have an active request when processing a new I/O
    // request.  The queue was either stopped (in the case of normal
    // operation) and not restarted until AR was cleared on the completion
    // of the last request, or the queue has been purged due to a cleanup
    // and the gate is closed such that no new requests can enter the queue.
    //

    NT_ASSERTMSG("Target already has active request",
                 (m_ActiveRequest == NULL));

    //
    // Stop the target queue so that the next I/O will not be issued.
    // Mark this as the active request for this target so that we know
    // to restart the target once it completes (this replaces restarting
    // the queue on an error in the middle of this routine)
    //
    // Note: we don't need to wait for the queue to be stopped.  In fact
    //       we can't since it won't stop until this request is completed.
    //
    // Note: We don't stop the queue in the case of an unlock due to a
    //       close operation, because the queue was already purged.  We
    //       do restart the queue on completion, but at that point we're
    //       being closed and there are no open handles, so we shouldn't
    //       get another I/O operation.
    //
    // TODO: This is where we will set the internal completion routine if
    //       that feature comes to light in WDF.
    //

    if (StopTargetQueue == true)
    {
        WdfIoQueueStop(m_TargetQueue, NULL, NULL);
    }

    m_ActiveRequest = request;
    request->SetIsTargetActiveRequest(true);

    TraceMessage(
        TRACE_LEVEL_INFORMATION,
        TRACE_FLAG_TARGET,
        "SPBTARGET %p queue%sfrozen while SPBREQUEST %p is active",
        GetSpbObject(),
        (StopTargetQueue ? " " : " not "),
        request->GetSpbObject()
        );

    //
    // Check the I/O type and process the request accordingly.
    //

    SPB_REQUEST_TYPE RequestType = request->GetSpbIoType();

    switch(RequestType)
    {
        case SpbRequestTypeLockController:
        {
            //
            // Don't allow a new lock while the target has exclusive access.
            //

            if (m_IsExclusiveControllerOwner == true)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p is already locked.  Failing lock "
                             "request %p",
                             GetSpbObject(),
                             request->GetSpbObject());

                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Lock is always a single request
            //

            request->SetPosition(SpbRequestSequencePositionSingle);

            //
            // Let the lock flow down to the controller queue.
            //

            status = STATUS_PENDING;
            break;
        }

        case SpbRequestTypeUnlockController:
        {
            //
            // Only allow unlock if the target has the controller locked.
            //

            if (m_IsExclusiveControllerOwner == false)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p is not locked.  Failing unlock "
                             "request %p",
                             GetSpbObject(),
                             request->GetSpbObject());
                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Unlock is always a single request
            //

            request->SetPosition(SpbRequestSequencePositionSingle);

            //
            // Clear the target's exclusive ownership of the bus.
            //

            m_IsExclusiveControllerOwner = false;

            //
            // Unlock the controller.
            //

            controller->BeginUnlock(request);

            //
            // Abort any subsequent processing.  ProcessUnlock will kick off
            // a sequence that completes the unlock request.
            //

            status = STATUS_PENDING;
            goto exit;
        }

        case SpbRequestTypeLockConnection:
        {
            //
            // Don't allow a connection lock while the target already has
            // exclusive access to the bus.
            //

            if (m_IsExclusiveControllerOwner == true)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p already has controller locked. "
                             "Failing lock request %p",
                             GetSpbObject(),
                             request->GetSpbObject());

                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Don't allow a connection lock while the target already
            // has exclusive access to the connection.
            //

            if (m_IsExclusiveConnectionOwner == true)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p already has connection locked. "
                             "Failing lock request %p",
                             GetSpbObject(),
                             request->GetSpbObject());

                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Lock is always a single request
            //

            request->SetPosition(SpbRequestSequencePositionSingle);

            //
            // Let the lock flow down to the controller queue.
            //

            status = STATUS_PENDING;
            break;
        }

        case SpbRequestTypeUnlockConnection:
        {
            //
            // Don't allow a connection unlock while the target has exclusive
            // access to the bus.
            //

            if (m_IsExclusiveControllerOwner == true)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p is already controller locked. "
                             "Failing lock request %p",
                             GetSpbObject(),
                             request->GetSpbObject());

                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Only allow unlock if the target has the connection locked.
            //

            if (m_IsExclusiveConnectionOwner == false)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p is not locked.  Failing unlock "
                             "request %p",
                             GetSpbObject(),
                             request->GetSpbObject());
                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Unlock is always a single request
            //

            request->SetPosition(SpbRequestSequencePositionSingle);

            //
            // Clear the target's exclusive ownership of the connection.
            //

            m_IsExclusiveConnectionOwner = false;

            //
            // Unlock the controller.
            //

            controller->BeginUnlock(request);

            //
            // Abort any subsequent processing.  ProcessUnlock will kick off
            // a sequence that completes the unlock request.
            //

            status = STATUS_PENDING;
            goto exit;
        }

        case SpbRequestTypeSequence:
        {
            //
            // Don't allow a sequence when target is locked.
            //

            if (m_IsExclusiveControllerOwner == true)
            {
                TraceMessage(TRACE_LEVEL_ERROR,
                             TRACE_FLAG_TARGET,
                             "SPBTARGET %p is locked. Failing sequence "
                             "request %p",
                             GetSpbObject(),
                             request->GetSpbObject());

                status = STATUS_INVALID_DEVICE_STATE;
                break;
            }

            //
            // Sequences are always single.
            //

            request->SetPosition(SpbRequestSequencePositionSingle);

            //
            // Update the locked I/O count anyway.
            //

            m_LockedIoCount += 1;

            //
            // Update the IO stats.
            //

            m_IoStats.TotalIoCount += 1;

            //
            // Run this through the general I/O path.
            //

            status = STATUS_PENDING;
            break;
        }

        case SpbRequestTypeRead:
        case SpbRequestTypeWrite:
        case SpbRequestTypeOther:
        {
            //
            // Set the position of this request
            //

            if (m_IsExclusiveControllerOwner == false)
            {
                request->SetPosition(SpbRequestSequencePositionSingle);
            }
            else if (m_LockedIoCount == 0)
            {
                request->SetPosition(SpbRequestSequencePositionFirst);
            }
            else
            {
                request->SetPosition(SpbRequestSequencePositionContinue);
            }

            //
            // Update the locked I/O count.
            //

            m_LockedIoCount += 1;

            //
            // Update the IO stats.
            //

            m_IoStats.TotalIoCount += 1;

            //
            // Run through the general I/O path.
            //

            status = STATUS_PENDING;
            break;
        }
    }

    //
    // If the status of the request is not pending then bail out now.
    //

    if (status != STATUS_PENDING)
    {
        goto exit;
    }

    //
    // Set the last transfer direction of the request & update the value
    // for the next request.
    //

    request->SetLastDirection(m_LastDirection);
    m_LastDirection = s_TypeToDirectionMap[RequestType];

    //
    // Forward the request to the controller or the controller queue
    //

    if ((m_IsExclusiveControllerOwner == true) ||
        (m_IsExclusiveConnectionOwner == true))
    {
        controller->ProcessRequest(request);
    }
    else
    {
        controller->ForwardRequestToControllerQueue(request);
    }

exit:

    if (status != STATUS_PENDING)
    {
        WdfRequestComplete(FxRequest, status);
    }
}

VOID
CScxTarget::ForwardFxRequestToTargetQueue(
    __in WDFREQUEST FxRequest
    )
/*++

  Routine Description:

    This routine forwards the specified request to the target's queue.
    On failure the request is completed.

  Arguments:

    FxRequest - the request being forwarded

  Return Value:

    None

--*/
{
    NTSTATUS status;

    status = WdfRequestForwardToIoQueue(FxRequest, m_TargetQueue);

    if (!NT_SUCCESS(status))
    {
        //
        // Failed to forward request to target queue,
        // just complete it.
        //

        WdfRequestComplete(
            FxRequest,
            ((status == STATUS_WDF_BUSY) ? STATUS_INVALID_DEVICE_STATE :
                                            status)
            );
    }

    return;
}

NTSTATUS
CScxTarget::ForwardIrpToTargetQueue(
    __in PIRP Irp
    )
/*++

  Routine Description:

    This routine forwards the specified IRP to the target's queue.

  Arguments:

    Irp - the request being forwarded

  Return Value:

    NTSTATUS indicating success or failure

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION stackLocation = IoGetCurrentIrpStackLocation(Irp);
    ULONG forwardIrpFlags = WDF_DISPATCH_IRP_TO_IO_QUEUE_NO_FLAGS;

    if (stackLocation == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_TARGET,
            "SPBTARGET %p - unable to retrieve stack location from IRP %p - "
            "%!STATUS!",
            GetSpbObject(),
            Irp,
            status
            );
        goto exit;
    }

    if ((stackLocation->MajorFunction == IRP_MJ_DEVICE_CONTROL) ||
        (stackLocation->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL))
    {
        forwardIrpFlags =
            WDF_DISPATCH_IRP_TO_IO_QUEUE_INVOKE_INCALLERCTX_CALLBACK;
    }

    status = WdfDeviceWdmDispatchIrpToIoQueue(
        m_Controller->GetFxObject(),
        Irp,
        m_TargetQueue,
        forwardIrpFlags);

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_TARGET,
            "SPBTARGET %p - unable to dispatch IRP %p to target queue - "
            "%!STATUS!",
            GetSpbObject(),
            Irp,
            status
            );
        goto exit;
    }

exit:

    return status;
}

VOID
CScxTarget::OnActiveRequestCompletion(
    __in PCScxRequest Request
    )
/*++

  Routine Description:

    This method is invoked during request completion to restart the
    target queue and clear the current active request (not in that order).
    If necessary it also restarts idle detection (enabling the controller
    to power down again).

    NOTE: This routine cannot currently check the status of the completed
          request.  That's an issue for lock or decomposed sequence,
          operations where we need to know the outcome of the I/O

          For now this routine assumes that lock operations were completed
          successfully.

  Arguments:

    Request - the request which completed

  Return Value:

    None

--*/
{
    SPB_REQUEST_TYPE spbIoType = Request->GetSpbIoType();

    NT_ASSERTMSG("Active request mismatch", (m_ActiveRequest == Request));

    //
    // TODO: Handle failure of lock operations.
    //

    TraceMessage(
        TRACE_LEVEL_VERBOSE,
        TRACE_FLAG_TARGET,
        "SPBTARGET %p active request %p completed",
        GetSpbObject(),
        Request->GetSpbObject()
        );

    //
    // Continue post processing for an unlock request if necessary.
    //

    if (Request->GetPostProcessUnlockOnCompletion() == true)
    {
        //
        // If target is no longer the exclusive controller owner,
        // restart idle detection on the controller.  This releases the
        // power reference taken for the lock which went through the
        // controller queue.
        //

        if (spbIoType == SpbRequestTypeUnlockController)
        {
            if (Request->GetController()->DriverPowerManaged() == WdfTrue)
            {
                TraceMessage(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_TARGET,
                    "SPBTARGET %p no longer exlusive controller owner - "
                    "releasing power reference",
                    GetSpbObject()
                    );

                WdfDeviceResumeIdle(Request->GetController()->GetFxObject());
            }
        }

        //
        // If this is a successful unlock request and target is no longer
        // the exclusive connection or controller owner, restart the controller
        // queue.
        //

        if ((spbIoType == SpbRequestTypeUnlockConnection) ||
            ((spbIoType == SpbRequestTypeUnlockController) &&
             (m_IsExclusiveConnectionOwner == false)))
        {
            TraceMessage(
                TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_TARGET,
                "SPBTARGET %p no longer exclusive %s owner - restarting queue",
                GetSpbObject(),
                (spbIoType == SpbRequestTypeUnlockConnection) ? "connection" : "controller"
                );

            Request->GetController()->EndUnlock();
        }
    }

    m_ActiveRequest = NULL;
    WdfIoQueueStart(m_TargetQueue);
}

NTSTATUS
CScxTarget::SendIoctlToSelf(
    __in ULONG IoControlCode
    )
/*++

  Routine Description:

    This routine issues an IRP to the target's file object and then
    waits synchronously for its completion.

  Arguments:

    IoControlCode - IOCTL to send

  Return Value:

    STATUS_INSUFFICIENT_RESOURCES if the IRP cannot be allocated

    error status from WDF if the ioctl cannot be delivered

    otherwise success

--*/
{
    PIRP irp;
    KEVENT event;

    PDEVICE_OBJECT wdmDevice;
    PFILE_OBJECT wdmFile;

    PIO_STACK_LOCATION irpStack;

    BOOLEAN completeIrp;
    IO_STATUS_BLOCK ioStatusBlock;

    WDF_OBJECT_ATTRIBUTES attributes;
    WDFREQUEST request = NULL;
    WDF_REQUEST_REUSE_PARAMS params;

    NTSTATUS status;
    NTSTATUS tempStatus;

    KeInitializeEvent(&event, NotificationEvent, false);

    wdmDevice = WdfDeviceWdmGetDeviceObject(m_Controller->GetFxObject());
    wdmFile   = WdfFileObjectWdmGetFileObject(GetFxObject());

    //
    // Allocate the IRP.
    //

    completeIrp = FALSE;

    RtlZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    irp = IoBuildDeviceIoControlRequest(
                IoControlCode,
                wdmDevice,
                NULL,
                0,
                NULL,
                0,
                TRUE,
                &event,
                &ioStatusBlock
                );

    if (irp == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;

        TraceMessage(
            TRACE_LEVEL_FATAL,
            TRACE_FLAG_TARGET,
            "SPBTARGET %p - unable to allocate IOCTL IRP - %!STATUS!",
            GetSpbObject(),
            status
            );
        goto exit;
    }

    completeIrp = TRUE;

    //
    // Format the next stack location.
    //

    irpStack = IoGetNextIrpStackLocation(irp);

    irpStack->FileObject = wdmFile;

    IoSetNextIrpStackLocation(irp);

    //
    // Create the request andd attach the IRP.
    //

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
        &attributes,
        CScxRequest);
    attributes.ParentObject = m_Controller->GetFxObject();

    CScxRequest::_SetObjectAttributes(&attributes);

    status = WdfRequestCreate(&attributes, NULL, &request);

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_FATAL,
            TRACE_FLAG_TARGET,
            "SPBTARGET %p - unable to create IOCTL request - "
            "%!STATUS!",
            GetSpbObject(),
            status
            );
        goto exit;
    }

    WDF_REQUEST_REUSE_PARAMS_INIT(
        &params,
        WDF_REQUEST_REUSE_SET_NEW_IRP | WDF_REQUEST_REUSE_MUST_COMPLETE,
        STATUS_SUCCESS);

    params.NewIrp = irp;

    status = WdfRequestReuse(request, &params);

    if (!NT_SUCCESS(status))
    {
        TraceMessage(
            TRACE_LEVEL_FATAL,
            TRACE_FLAG_TARGET,
            "SPBTARGET %p - unable to reuse IOCTL request - "
            "%!STATUS!",
            GetSpbObject(),
            status
            );
        goto exit;
    }

    completeIrp = FALSE;

    //
    // Send the IRP to ourself.
    //

    ProcessTargetRequest(request, false);

    //
    // Wait for the IRP to complete.
    //

    KeWaitForSingleObject(&event,
                          Executive,
                          KernelMode,
                          false,
                          NULL);

    status = ioStatusBlock.Status;

exit:

    if (request != NULL)
    {
        //
        // Clean up the request.
        //

        WDF_REQUEST_REUSE_PARAMS_INIT(
            &params,
            WDF_REQUEST_REUSE_SET_NEW_IRP,
            STATUS_SUCCESS);

        tempStatus = WdfRequestReuse(request, &params);

        if (!NT_SUCCESS(tempStatus))
        {
            NT_ASSERTMSG("WdfRequestReuse should not fail", NT_SUCCESS(tempStatus));

            TraceMessage(
                TRACE_LEVEL_WARNING,
                TRACE_FLAG_TARGET,
                "SPBTARGET %p - WdfRequestReuse failed when "
                "clearing IRP from request - %!STATUS!",
                GetSpbObject(),
                status
                );
        }

        WdfObjectDelete(request);
    }

    if (completeIrp == TRUE)
    {
        //
        // Free the IRP if not done already.
        //

        IoCompleteRequest(irp, 0);
    }

    return status;
}

NTSTATUS
CScxTarget::QueryDescriptorFromResourceHub(
    _In_ PCScxController Controller,
    _In_ LARGE_INTEGER connectionId,
    _Out_ PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER *connectionProperties
    )
/*++

  Routine Description:

    This routine queries the BIOS SPB descriptor for the given connection ID
    from Resource Hub.

  Arguments:

    Controller - the controller object associated with the request.

    connectionId - Connection ID to query.

    connectionProperties - A pointer that receives the BIOS descriptor buffer.

  Return Value:

    STATUS_INSUFFICIENT_RESOURCES if the IRP cannot be allocated

    error status from WDF if the ioctl cannot be delivered

    otherwise success

--*/
{

#define INITIAL_ACPI_STRING_SIZE (64)

    ULONG_PTR BytesReturned;
    NTSTATUS status;

    RH_QUERY_CONNECTION_PROPERTIES_INPUT_BUFFER InputBuffer;
    PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER OutputBuffer;

    WDF_MEMORY_DESCRIPTOR InputMemoryDescriptor;
    WDF_MEMORY_DESCRIPTOR OutputMemoryDescriptor;

    OutputBuffer = NULL;

    //
    // Set up a WDF memory descriptor for the input parameters.
    //

    RtlZeroMemory(&InputBuffer, sizeof(InputBuffer));
    InputBuffer.Version = RH_QUERY_CONNECTION_PROPERTIES_INPUT_VERSION;
    InputBuffer.QueryType = ConnectionIdType;
    RtlCopyMemory(&InputBuffer.u.ConnectionId,
                  (PVOID) (&connectionId),
                  sizeof(InputBuffer.u.ConnectionId));

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&InputMemoryDescriptor,
                                      &InputBuffer,
                                      sizeof(InputBuffer));

    ULONG Length;

    Length = FIELD_OFFSET(RH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER,
                          ConnectionProperties);
    Length += sizeof(PNP_SERIAL_BUS_DESCRIPTOR) +
              INITIAL_ACPI_STRING_SIZE;

    do {
#pragma prefast(suppress:__WARNING_ALIASED_MEMORY_LEAK, "OutputBuffer is freed below or in the descructor")
        OutputBuffer = (PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER) ExAllocatePoolWithTag(NonPagedPoolNx,
                                                                                             Length,
                                                                                             SCX_POOL_TAG
                                                                                             );
        if (OutputBuffer == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceMessage(TRACE_LEVEL_ERROR,
                         TRACE_FLAG_TARGET,
                         "Failure querying target connection properties - %!STATUS!",
                         status);

            goto TargetConnectionExit;
        }

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&OutputMemoryDescriptor,
                                          OutputBuffer,
                                          Length);

        //
        // Send down the IOCTL to query the connection properties
        //

        status = WdfIoTargetSendIoctlSynchronously(
                     Controller->GetResourceHubTarget(),
                     NULL,
                     IOCTL_RH_QUERY_CONNECTION_PROPERTIES,
                     &InputMemoryDescriptor,
                     &OutputMemoryDescriptor,
                     NULL,
                     &BytesReturned);

        if (status == STATUS_BUFFER_TOO_SMALL)
        {
            Length = OutputBuffer->PropertiesLength +
                     FIELD_OFFSET(RH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER,
                                  ConnectionProperties);

            ExFreePoolWithTag(OutputBuffer,SCX_POOL_TAG);
            OutputBuffer = NULL;
        }

    } while (status == STATUS_BUFFER_TOO_SMALL);

    //
    // Reject SPB descriptors that do not meet the minimum length requirements.
    //

    if (NT_SUCCESS(status) &&
        (OutputBuffer->PropertiesLength < sizeof(PNP_SERIAL_BUS_DESCRIPTOR)))
    {
        status = STATUS_ACPI_INVALID_DATA;
    }

TargetConnectionExit:

    if (!NT_SUCCESS(status))
    {
        if (OutputBuffer != NULL)
        {
            ExFreePoolWithTag(OutputBuffer,SCX_POOL_TAG);
            OutputBuffer = NULL;
        }
    }
    else
    {
        *connectionProperties = OutputBuffer;
    }

    return status;
}
