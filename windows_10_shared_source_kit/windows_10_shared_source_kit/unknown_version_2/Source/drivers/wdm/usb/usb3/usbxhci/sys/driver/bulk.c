
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Bulk.c

Abstract:

    Compared to WIN8, Bulk/Interrupt TransferRing implementation has been separated out from
    other endpoint types.


    ---------------------------------------------------------------------
    Differences in Bulk TransferRing compared to other endpoint types:
    ---------------------------------------------------------------------

    1. Workarounds that don't make sense for Bulk/Int:

    2. Workarounds that only apply to Bulk/Int:
        - SerializeTDBulkInterrupt


    ------------------------------------------------
    Functional changes from the WIN8 implementation:
    ------------------------------------------------

    1. Allow multiple outstanding stages per transfer if PAE=1, and serialize stages for the
       transfer if PAE=0, or hack flag is set.


    ------------
    Open issues:
    ------------


    --------------------------------------------------
    Unnatural hardware behavior:
    --------------------------------------------------


--*/

#include "pch.h"
#include "Bulk.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Bulk_GetConfiguration)
#pragma alloc_text(PAGE, Bulk_Initialize)
#endif

CONST TR_FUNCTION_TABLE BulkFunctionTable = {

    Bulk_Initialize,
    Bulk_Cleanup,
    Bulk_CommonBufferCallback,
    Bulk_EP_Enable,
    Bulk_EP_Disable,
    Bulk_EP_EnableForwardProgress,
    Bulk_EP_AreTransfersPending,
    Bulk_EP_OkToReclaimTransfersOnCancel,
    Bulk_EP_ReclaimTransfers,
    Bulk_EP_IsTransferEventLikelyDuplicate,
    Bulk_EP_TransferEventHandler,
    Bulk_EP_StartMapping,
    Bulk_EP_StopMapping,
    Bulk_EP_IsMappingStoppedOrPaused,
    Bulk_EP_EndpointHalted,
    TR_EmptyFunction,                               // EP_EndpointResetFromClient
    Bulk_EP_ProcessExpectedEventTRBs,
    Bulk_EP_FSEReceived,
    Bulk_EP_WasDoorbellRungSinceMappingStart,
    NULL,                                           // WDF_EvtIoDefault
    Bulk_WdfEvtIoCanceledOnQueue,
    Bulk_WdfEvtIoQueueReadyNotification,
    Bulk_WdfEvtDpcForTransferCompletion

};

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Bulk_GetConfiguration(
    __in
        HANDLE      ControllerHandle,
    __out
        PTR_CONFIG  Config
    )
{
    ULONG trbsPerSegment;

    PAGED_CODE();

    Config->QueueDispatchType = WdfIoQueueDispatchManual;
    Config->ContextSize       = sizeof(BULK_DATA);
    Config->SegmentSize       = SMALL_SEGMENT_SIZE;
    Config->DoubleBufferSize  = SMALL_SEGMENT_SIZE;
    Config->FunctionTable     = &BulkFunctionTable;

    if (Controller_GetDeviceFlags(ControllerHandle).NoLinkTRBInTD) {

        NT_ASSERTMSG("DisableChainedMDLSupport must be set if NoLinkTRBInTD is set",
                     (Controller_GetDeviceFlags(ControllerHandle).DisableChainedMDLSupport == 1));

        //
        // Since one stage corresponds to one TD for Bulk/Interrupt, if the TD is
        // not allowed to cross segments, size of the stage is limited by the number
        // of Normal TRBs that can be programmed in one segment.
        //
        trbsPerSegment = Config->SegmentSize / sizeof(TRB);

        //
        // Subtract two TRBs to deal with page boundaries, plus one for Event Data
        // TRB and one for Link TRB.
        //
        NT_ASSERT(trbsPerSegment > 4);
        trbsPerSegment -= 4;

        Config->MaxStageSize = trbsPerSegment * PAGE_SIZE;
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Bulk_Initialize(
    __in
        HANDLE Context
    )
{
    PBULK_DATA bulkData;

    PAGED_CODE();

    bulkData = (PBULK_DATA)Context;

    InitializeListHead(&bulkData->PendingTransferList);
    InitializeListHead(&bulkData->WaitingForCancelCallbackTransferList);
    InitializeListHead(&bulkData->CompletionTransferList);

    //
    // Table 73: IDT shall not be set ('1') for TRBs on endpoints that define a Max
    // Packet Size < 8 bytes, or on IN endpoints.
    //
    if ((Controller_GetDeviceFlags(bulkData->ControllerHandle).DisableImmediateData) ||
        (Endpoint_GetMaxPacketSize(bulkData->EndpointHandle) < IMMEDIATE_DATA_LENGTH) ||
        (Endpoint_GetDirection(bulkData->EndpointHandle) == EndpointDirection_In)) {

        bulkData->ImmediateDataEnabled = FALSE;

    } else {

        bulkData->ImmediateDataEnabled = TRUE;
    }

    //
    // WIN8 behavior: Not clear why stages are serialized if NoLinkTRBInTD is set.
    //
    if ((Controller_GetDeviceFlags(bulkData->ControllerHandle).SerializeTDBulkInterrupt) ||
        (Controller_GetDeviceFlags(bulkData->ControllerHandle).NoLinkTRBInTD)) {

        bulkData->MaxPendingStages = 1;

    } else {

        bulkData->MaxPendingStages = BULK_MAX_PENDING_STAGES_PER_TRANSFERRING;
    }

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Cleanup(
    __in
        HANDLE Context
    )
{
    PBULK_DATA bulkData;

    bulkData = (PBULK_DATA)Context;

    TR_Lock(bulkData);

    if (Bulk_DoesDriverOwnRequests(bulkData)) {
        LogError(Controller_GetRecorderLog(bulkData->ControllerHandle), TransferRing,
                "%u.%u.%u: ERROR_ASSERT: Driver owned requests not completed",
                UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                Endpoint_GetContextIndex(bulkData->EndpointHandle),
                bulkData->StreamId);
        NT_ASSERTMSG("Driver owned requests not completed", 0);
    }

    TR_Unlock(bulkData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_CommonBufferCallback(
    __in
        HANDLE      Context,
    __in
        NTSTATUS    Status
    )
{
    PBULK_DATA bulkData;

    bulkData = (PBULK_DATA)Context;

    UNREFERENCED_PARAMETER(Status);

    bulkData->CommonBufferAcquireAttemptCount += 1;

    //
    // Unconditionally go back to mapping transfers. We could honor the StopMapping request
    // here, but that would incur an Interlocked operation in the common case of not having
    // received a StopMapping request.
    //
    NT_ASSERT((bulkData->MappingState == MS_Mapping) ||
              (bulkData->MappingState == MS_Stopping));

    Bulk_MapTransfers(bulkData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Bulk_EP_Enable(
    __in
        HANDLE Context
    )
{
    PBULK_DATA  bulkData;
    ULONG       maxBurstPayload;

    bulkData = (PBULK_DATA)Context;

    NT_ASSERT(bulkData->MaxStageSize >= MIN_STAGE_SIZE);

    if (UsbDevice_GetDeviceSpeed(bulkData->UsbDeviceHandle) == UsbSuperSpeed) {

        //
        // N.B: Endpoint_GetMaxPayloadPerServiceOpportunity returns the same value, but it
        //      is not set yet.
        //
        maxBurstPayload = Endpoint_GetMaxPacketSize(bulkData->EndpointHandle) *
                          (Endpoint_GetMaxBurst(bulkData->EndpointHandle) + 1);

        //
        // The max stage size must be multiple of max burst payload. Round it down if it
        // is not a multiple.
        //
        if ((bulkData->MaxStageSize % maxBurstPayload) != 0) {

            bulkData->MaxStageSize -=
                (bulkData->MaxStageSize % maxBurstPayload);

            bulkData->MapRegisterCount =
                ((bulkData->MaxStageSize + (PAGE_SIZE - 1)) / PAGE_SIZE);

            LogInfo(Controller_GetRecorderLog(bulkData->ControllerHandle), TransferRing,
                    "%u.%u.%u: Reducing MaxStageSize to %u",
                    UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(bulkData->EndpointHandle),
                    bulkData->StreamId,
                    bulkData->MaxStageSize);
        }
    }

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_Disable(
    __in
        HANDLE    Context,
    __in
        BOOLEAN   FreeResources
    )
{
    PBULK_DATA    bulkData;
    MAPPING_STATE mappingState;

    UNREFERENCED_PARAMETER(FreeResources);

    bulkData = (PBULK_DATA)Context;

    //
    // There can be a case where the last outstanding transfer is
    // completed back to the client driver on one DPC, and the client
    // driver immediately deconfigures the endpoint. Another DPC runs
    // on a separate processor and processes the completion of this
    // endpoint deconfigure, while the first DPC has not fully unwound
    // and changed the Mapping State from running to paused.
    //
    // Busy wait for the Mapping state to get to Stopped or Paused before
    // completing this disable request. Busy wait because this request
    // can arrive at Dispatch and needs to be completed synchronously.
    //
    for (;;) {

        mappingState = *((volatile MAPPING_STATE*)&bulkData->MappingState);

        if ((mappingState != MS_Mapping) && (mappingState != MS_Stopping)) {
            break;
        }
    }

    if (mappingState != MS_Stopped) {

        //
        // If mapping state is MS_Paused, change it to MS_Stopped so that
        // if the endpoint is subsequently enabled, the mapping state is
        // stopped.
        //
        mappingState = InterlockedExchange((volatile LONG*)&bulkData->MappingState, MS_Stopped);
        NT_ASSERT(mappingState == MS_Paused);
    }

    NT_ASSERT(IsListEmpty(&bulkData->PendingTransferList));
    NT_ASSERT(IsListEmpty(&bulkData->WaitingForCancelCallbackTransferList));
    NT_ASSERT(IsListEmpty(&bulkData->CompletionTransferList));

    NT_ASSERT(Bulk_EP_IsMappingStoppedOrPaused(bulkData) != FALSE);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Bulk_EP_EnableForwardProgress(
    __in
        HANDLE    Context,
    __in
        ULONG     MaxTransferSize
    )
{
    PBULK_DATA  bulkData;
    ULONG       maxBurstPayload;
    ULONG       segmentsRequired;
    NTSTATUS    status;
    ULONG       trbsRequired;
    ULONG       trbsPerBurst;
    ULONG       trbsPerSegment;
    ULONG       usableTrbsPerSegment;

    TRY {

        bulkData = (PBULK_DATA)Context;

        //
        // Reserve CommonBuffer segments required to map a MaxTransferSize transfer.
        //
        trbsRequired = BufferSizeToSpanPages(MaxTransferSize) +     // Normal TRBs.
                       1;                                           // Event Data TRB.

        maxBurstPayload =
            Endpoint_GetMaxPayloadPerServiceOpportunity(bulkData->EndpointHandle);

        trbsPerBurst = BufferSizeToSpanPages(maxBurstPayload) +     // Normal TRBs.
                       1;                                           // Event Data TRB.

        trbsPerSegment = bulkData->SegmentSize / sizeof(TRB);

        usableTrbsPerSegment = ((trbsPerSegment - 1) -
                               ((trbsPerSegment - 1) % trbsPerBurst));

        if (trbsRequired > usableTrbsPerSegment) {
            segmentsRequired = trbsRequired / usableTrbsPerSegment;
            if (trbsRequired % usableTrbsPerSegment) {
                segmentsRequired += 1;
            }
        } else {
            segmentsRequired = 1;
        }

        status = TR_EnsureSegments(bulkData, segmentsRequired, FALSE);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Disable Doublebuffering since it requires allocating CommonBuffer at runtime.
        //
        bulkData->DoubleBufferSize = 0;

        //
        // WIN8 behavior: Not clear why ImmediateData needs to be disabled.
        //
        bulkData->ImmediateDataEnabled = FALSE;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_EP_AreTransfersPending(
    __in
        HANDLE Context
    )
{
    PBULK_DATA  bulkData;
    BOOLEAN     transfersPending;

    bulkData = (PBULK_DATA)Context;

    TR_Lock(bulkData);

    //NT_ASSERTMSG("Revisit", 0);

    if (IsListEmpty(&bulkData->PendingTransferList)) {
        transfersPending = FALSE;
    } else {
        transfersPending = TRUE;
    }

    TR_Unlock(bulkData);

    return transfersPending;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_OkToReclaimTransfersOnCancel(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to go ahead and complete
    transfers that have already been canceled, or get canceled in future.

    State machine ensures that mapping is currently stopped, and that no
    Interrupt Dpc is currently processing completion of transfers.

--*/
{
    PBULK_DATA  bulkData;
    BOOLEAN     queueDpc;

    TRY {

        bulkData = (PBULK_DATA)Context;

        TR_Lock(bulkData);

        if (bulkData->Flags.AtleastOneTransferCanceled) {
            APPEND_LIST(&bulkData->CompletionTransferList, &bulkData->PendingTransferList);
            queueDpc = TRUE;
        } else {
            bulkData->Flags.OkToReclaimTransfersOnCancel = 1;
            queueDpc = FALSE;
        }

        TR_Unlock(bulkData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(bulkData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_ReclaimTransfers(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to complete or requeue all
    driver-owned transfer requests, right away.

    State machine ensures that mapping is currently stopped, and that no
    Interrupt Dpc is currently processing completion of transfers.

--*/
{
    PBULK_DATA bulkData;

    TRY {

        bulkData = (PBULK_DATA)Context;

        TR_Lock(bulkData);

        if (IsListEmpty(&bulkData->PendingTransferList) == FALSE) {

            APPEND_LIST(&bulkData->CompletionTransferList, &bulkData->PendingTransferList);
        }

        bulkData->Flags.ReclaimTransfersAndAcknowledge = 1;

        TR_Unlock(bulkData);

    } FINALLY {

        //
        // Unconditionally queue the Dpc because only the Dpc can know for certain
        // that there are no more driver owned requests. And only then should the
        // reclaim of transfers be acknowledged.
        //
        TR_QueueDpcForTransferCompletion(bulkData);
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_EP_IsTransferEventLikelyDuplicate(
    __in
        HANDLE              Context,
    __in
        PHYSICAL_ADDRESS    DequeuePointer,
    __out
        PBOOLEAN            PointsToNoOpTRB
    )
{
    PBULK_DATA  bulkData;
    BOOLEAN     likelyDuplicate;

    bulkData = (PBULK_DATA)Context;

    TR_Lock(bulkData);

    likelyDuplicate =
        Bulk_IsTransferEventLikelyDuplicate_Internal(bulkData, DequeuePointer, PointsToNoOpTRB);

    TR_Unlock(bulkData);

    return likelyDuplicate;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_EP_TransferEventHandler(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
{
    if (Trb->EventData == 1) {
        return Bulk_ProcessTransferEventWithED1(Trb, Context);
    } else {
        return Bulk_ProcessTransferEventWithED0(Trb, Context);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_StartMapping(
    __in
        HANDLE Context
    )
{
    PBULK_DATA      bulkData;
    MAPPING_STATE   previousState;

    bulkData = (PBULK_DATA)Context;

    InterlockedExchange((volatile LONG*)&bulkData->Flags.AsUlong32, 0);

    bulkData->OutstandingEventCountAfterStop = 0;

    NT_ASSERT(bulkData->PendingStageCount == 0);

    previousState = TR_AttemptStateChange(bulkData, MS_Any, MS_Mapping);

    if (previousState == MS_Stopped) {
        Bulk_MapTransfers(bulkData);
    } else {
        NT_ASSERT(FALSE);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_StopMapping(
    __in
        HANDLE Context
    )
{
    PBULK_DATA      bulkData;
    MAPPING_STATE   previousState;

    bulkData = (PBULK_DATA)Context;

    //
    // Previous state must be Mapping or Paused. Unconditionally change it to
    // Stopping.
    //
    previousState = TR_AttemptStateChange(bulkData, MS_Any, MS_Stopping);

    NT_ASSERT(previousState != MS_Stopped);
    NT_ASSERT(previousState != MS_Stopping);

    //
    // If previous state was Paused, then change to Stopped and acknowledge
    // the stop.
    //
    if (previousState == MS_Paused) {

        previousState = TR_AttemptStateChange(bulkData, MS_Stopping, MS_Stopped);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_EP_IsMappingStoppedOrPaused(
    __in
        HANDLE Context
    )
{
    PBULK_DATA bulkData;

    bulkData = (PBULK_DATA)Context;

    return ((bulkData->MappingState == MS_Paused) ||
            (bulkData->MappingState == MS_Stopped));
}

_Requires_lock_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_ProcessStreamOnEndpointStopOrHalt(
    __in
        PBULK_DATA BulkData
    )
{
    ULONG               bytesTransferred;
    PHYSICAL_ADDRESS    dequeuePointer;
    BOOLEAN             dequeuePointerFound;
    ULONG               skippedStageCount;
    PBULK_STAGE_DATA    stageData;
    PBULK_TRANSFER_DATA transferData;

    NT_ASSERT(BulkData->StreamId != 0);

    dequeuePointer = Endpoint_GetStreamContextDequeuePointer(BulkData->EndpointHandle,
                                                             BulkData->StreamId);

    dequeuePointerFound = Bulk_FindStage(BulkData,
                                         dequeuePointer,
                                         &stageData,
                                         &skippedStageCount);

    NT_ASSERT(dequeuePointerFound);

    if (dequeuePointerFound && stageData) {

        transferData = stageData->TransferData;

        bytesTransferred = Endpoint_GetStreamContextTransferLength(BulkData->EndpointHandle,
                                                                   BulkData->StreamId);

        if (bytesTransferred > stageData->TransferSize) {

            LogWarning(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Invalid bytesTransferred %u",
                       UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(BulkData->EndpointHandle),
                       BulkData->StreamId, bytesTransferred);

            NT_ASSERTMSG("Invalid bytesTransferred", (bytesTransferred <= stageData->TransferSize));
            bytesTransferred = 0;
        }

        transferData->BytesTransferred += bytesTransferred;

        if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
            (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

            RtlCopyMemory(stageData->Buffer,
                          stageData->DoubleBufferData->VirtualAddress,
                          bytesTransferred);
        }

        BulkData->OutstandingEventCountAfterStop = skippedStageCount;

    } else {

        BulkData->OutstandingEventCountAfterStop = BulkData->PendingStageCount;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_EndpointHalted(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the notification from the State Machine that the Endpoint has halted.
    It is needed in case of Streams, where one Stream saw the halted transfer
    completion, but others are not aware.

--*/
{
    PBULK_DATA bulkData;

    TRY {

        bulkData = (PBULK_DATA)Context;

        if (bulkData->StreamId == 0) {
            LEAVE;
        }

        TR_Lock(bulkData);

        //
        // If EndpointHalted is already set, this TransferRing is the one that received the
        // halted completion code. See Bulk_ProcessTransferEventWithHaltedCompletion.
        //
        if (bulkData->Flags.EndpointHalted) {
            TR_Unlock(bulkData);
            LEAVE;
        }

        Bulk_ProcessStreamOnEndpointStopOrHalt(bulkData);

        bulkData->Flags.EndpointHalted = 1;

        TR_Unlock(bulkData);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_ProcessExpectedEventTRBs(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to complete processing of
    outstanding Event TRBs after Stop Endpoint command completion. This is
    needed because in case of multiple interrupters, we may get completion
    of Stop Endpoint command and the Stopped Event, but still have unprocessed
    Event TRBs on a different interrupter.

    N.B: Whether or not there are outstanding Event TRBs to process should
         already be determined before this call.

--*/
{
    PBULK_DATA bulkData;
    BOOLEAN    acknowledge;

    TRY {

        bulkData = (PBULK_DATA)Context;

        TR_Lock(bulkData);

        if (bulkData->OutstandingEventCountAfterStop == 0) {
            acknowledge = TRUE;
        } else {
            bulkData->Flags.AcknowledgeExpectedEventTRBsProcessed = 1;
            acknowledge = FALSE;
        }

        TR_Unlock(bulkData);

    } FINALLY {

        if (acknowledge) {
            Endpoint_TR_ExpectedEventTRBsProcessed(bulkData->EndpointHandle);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_EP_FSEReceived(
    __in
        HANDLE Context
    )
{
    PBULK_DATA bulkData;

    TRY {

        bulkData = (PBULK_DATA)Context;

        if (bulkData->StreamId == 0) {
            LEAVE;
        }

        TR_Lock(bulkData);

        //
        // If EndpointHalted is already set, this TransferRing is the one that received the
        // halted completion code. See Bulk_ProcessTransferEventWithHaltedCompletion.
        //
        if (bulkData->Flags.FSEReceived) {
            TR_Unlock(bulkData);
            LEAVE;
        }

        Bulk_ProcessStreamOnEndpointStopOrHalt(bulkData);

        bulkData->Flags.FSEReceived = 1;

        TR_Unlock(bulkData);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_EP_WasDoorbellRungSinceMappingStart(
    __in
        HANDLE Context
    )
{
    PBULK_DATA bulkData;

    bulkData = (PBULK_DATA)Context;

    return (bulkData->Flags.DoorbellRungSinceMappingStart == 1);
}

VOID
Bulk_WdfEvtIoCanceledOnQueue(
    __in
        WDFQUEUE    WdfQueue,
    __in
        WDFREQUEST  WdfRequest
    )
{
    PBULK_TRANSFER_DATA transferData;
    PBULK_DATA          bulkData;

    bulkData     = (PBULK_DATA)TR_GetContext(WdfQueue);
    transferData = &(GetRequestData(WdfRequest)->BulkTransferData);

    //
    // If this request was not seen by the driver before, initialize the context
    // because the rest of the code expects an initialized request context.
    //
    if (transferData->Initialized == FALSE) {

        Bulk_TransferData_Initialize(bulkData, transferData, WdfRequest);
    }

    transferData->CancelState = TCS_Canceled;

    TR_Lock(bulkData);
    InsertTailList(&bulkData->CompletionTransferList, &transferData->ListEntry);
    TR_Unlock(bulkData);

    TR_QueueDpcForTransferCompletion(bulkData);
}

VOID
Bulk_WdfEvtRequestCancel(
    WDFREQUEST WdfRequest
    )
{
    PBULK_DATA              bulkData;
    BOOLEAN                 queueDpc;
    BOOLEAN                 reportCancel;
    PBULK_TRANSFER_DATA     transferData;

    TRY {

        queueDpc     = FALSE;
        reportCancel = FALSE;

        transferData = &(GetRequestData(WdfRequest)->BulkTransferData);
        bulkData     = transferData->BulkData;

        LogInfo(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                "%u.%u.%u: WdfRequest 0x%p cancel requested",
                UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                Endpoint_GetContextIndex(bulkData->EndpointHandle),
                bulkData->StreamId, WdfRequest);

        TR_Lock(bulkData);

        if (transferData->CancelState == TCS_Cancelable) {

            //
            // WIN8 behavior: When completing canceled requests programmed in hardware,
            // requests that were not canceled get requeued.
            //
            if (bulkData->Flags.OkToReclaimTransfersOnCancel) {
                APPEND_LIST(&bulkData->CompletionTransferList, &bulkData->PendingTransferList);
                queueDpc = TRUE;
            } else {
                bulkData->Flags.AtleastOneTransferCanceled = 1;
                reportCancel = TRUE;
            }
        }

        TR_Unlock(bulkData);

        if (reportCancel) {
            Endpoint_TR_TransferCanceled(bulkData->EndpointHandle);
        }

        TR_Lock(bulkData);

        if (transferData->CancelState == TCS_WaitingForCancelCallback) {

            //
            // When we were about to complete the transfer, WdfRequestUnmarkCancellable failed
            // because the Cancel callback was about to execute. Simply go ahead and complete
            // the transfer, now that the Cancel callback has arrived.
            //
            RemoveEntryList(&transferData->ListEntry);
            InsertTailList(&bulkData->CompletionTransferList, &transferData->ListEntry);

            queueDpc = TRUE;
        }

        transferData->CancelState = TCS_Canceled;

        TR_Unlock(bulkData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(bulkData);
        }
    }
}

VOID
Bulk_WdfEvtIoQueueReadyNotification(
    WDFQUEUE    WdfQueue,
    WDFCONTEXT  WdfContext
    )
/*++

Routine Description:

    This routine gets called when a request arrives on an empty queue, even if the
    driver still owns previously delivered requests from the queue that it has not
    yet completed.

--*/
{
    PBULK_DATA      bulkData;
    MAPPING_STATE   previousState;

    UNREFERENCED_PARAMETER(WdfQueue);

    bulkData = (PBULK_DATA)WdfContext;

    previousState = TR_AttemptStateChange(bulkData, MS_Paused, MS_Mapping);

    if (previousState == MS_Paused) {

        Bulk_MapTransfers(bulkData);

    } else {

        InterlockedExchange(&bulkData->AttemptMapping, 1);

        previousState = TR_AttemptStateChange(bulkData, MS_Paused, MS_Mapping);

        if (previousState == MS_Paused) {
            Bulk_MapTransfers(bulkData);
        }
    }
}

VOID
Bulk_CompleteTransfers(
    __in
        PBULK_DATA  BulkData,
    __in
        PLIST_ENTRY CompletionTransferList
    )
{
    LIST_ENTRY          cancelList;
    LIST_ENTRY          completionList;
    LIST_ENTRY          requeueList;
    PBULK_STAGE_DATA    stageData;
    NTSTATUS            status;
    PBULK_TRANSFER_DATA transferData;

    InitializeListHead(&cancelList);
    InitializeListHead(&completionList);
    InitializeListHead(&requeueList);

    TR_Lock(BulkData);

    while (IsListEmpty(CompletionTransferList) == FALSE) {

        transferData = (PBULK_TRANSFER_DATA)RemoveHeadList(CompletionTransferList);
        InitializeListHead(&transferData->ListEntry);

        //
        // Free all stages of the transfer.
        //
        StageQueue_ForwardScanStart(&transferData->StageQueue);

        stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

        while (stageData != NULL) {

            TR_Unlock(BulkData);
            Bulk_Stage_FreeScatterGatherList(BulkData, stageData);
            TR_Lock(BulkData);

            Bulk_Stage_Release(BulkData, stageData);

            transferData->StagesCompleted += 1;
            BulkData->PendingStageCount   -= 1;

            stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
        }

        if (transferData->CancelState == TCS_Canceled) {
            InsertTailList(&cancelList, &transferData->ListEntry);
            continue;
        }

        NT_ASSERT(transferData->CancelState == TCS_Cancelable);

        status = WdfRequestUnmarkCancelable(transferData->WdfRequest);

        if (!NT_SUCCESS(status)) {

            NT_ASSERT(status == STATUS_CANCELLED);

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: WdfRequest 0x%p WdfRequestUnmarkCancelable returned %!STATUS!",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle),
                    BulkData->StreamId, transferData->WdfRequest, status);

            transferData->CancelState = TCS_WaitingForCancelCallback;
            InsertTailList(&BulkData->WaitingForCancelCallbackTransferList, &transferData->ListEntry);
            continue;
        }

        transferData->CancelState = TCS_NotCancelable;

        //
        // Transfer can be completed here if the Stopped Transfer Event was pointing
        // to its stage and the completion code was stopped short packet, or if all
        // its bytes were transferred. See Bulk_ProcessTransferEventWithStoppedCompletion().
        //
        if ((transferData->TrbCompletionCode == TRB_COMPLETION_CODE_SUCCESS) ||
            (transferData->TrbCompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET)) {

            InsertTailList(&completionList, &transferData->ListEntry);
            continue;
        }

        InsertTailList(&requeueList, &transferData->ListEntry);
    }

    TR_Unlock(BulkData);

    while (IsListEmpty(&cancelList) == FALSE) {

        transferData = (PBULK_TRANSFER_DATA)RemoveHeadList(&cancelList);
        InitializeListHead(&transferData->ListEntry);

        if (BulkData->StreamId != 0) {

            if (transferData->TrbCompletionCode == TRB_COMPLETION_CODE_INVALID) {
                transferData->TrbCompletionCode =
                    Endpoint_StreamsGetCompletionCode(BulkData->EndpointHandle);
            }
        }

        Bulk_Transfer_Complete(BulkData, transferData, USBD_STATUS_CANCELED);
    }

    while (IsListEmpty(&completionList) == FALSE) {

        transferData = (PBULK_TRANSFER_DATA)RemoveHeadList(&completionList);
        InitializeListHead(&transferData->ListEntry);

        Bulk_Transfer_Complete(BulkData, transferData, USBD_STATUS_NOT_SET);
    }

    while (IsListEmpty(&requeueList) == FALSE) {

        //
        // N.B: Requests are removed from the tail of the list. This is because
        //      WdfRequestRequeue adds requests to the head of the queue. So when
        //      we are done, oldest request should be at the head of the queue.
        //
        transferData = (PBULK_TRANSFER_DATA)RemoveTailList(&requeueList);
        InitializeListHead(&transferData->ListEntry);

        NT_ASSERT(transferData->TrbCompletionCode == TRB_COMPLETION_CODE_INVALID);
        NT_ASSERT(transferData->BytesTransferred <= transferData->BytesMapped);
        NT_ASSERT(transferData->WdfRequestStatus == STATUS_PENDING);
        NT_ASSERT(transferData->StagesMapped == transferData->StagesCompleted);

        transferData->BytesMapped = transferData->BytesTransferred;

        if (transferData->DoubleBufferData != NULL) {
            TR_ReleaseDoubleBuffer(BulkData, transferData->DoubleBufferData);
            transferData->DoubleBufferData = NULL;
        }

        if ((transferData->Mdl != NULL) &&
            (transferData->Mdl != transferData->TransferUrb->TransferBufferMDL) &&
            (transferData->Mdl != transferData->BulkData->ForwardProgressMdl)) {

            IoFreeMdl(transferData->Mdl);
            transferData->Mdl = NULL;
        }

        status = WdfRequestRequeue(transferData->WdfRequest);

        NT_ASSERT(NT_SUCCESS(status));

        LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                "%u.%u.%u: Requeued WdfRequest 0x%p %!STATUS!",
                UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                Endpoint_GetContextIndex(BulkData->EndpointHandle),
                BulkData->StreamId, transferData->WdfRequest, status);
    }
}

_Requires_lock_held_(BulkData->Lock)
BOOLEAN
Bulk_DoesDriverOwnRequests(
    __in
        PBULK_DATA BulkData
    )
{
    if (IsListEmpty(&BulkData->PendingTransferList) &&
        IsListEmpty(&BulkData->WaitingForCancelCallbackTransferList) &&
        IsListEmpty(&BulkData->CompletionTransferList)) {

        return FALSE;

    } else {

        return TRUE;
    }
}

VOID
Bulk_WdfEvtDpcForTransferCompletion(
    WDFDPC WdfDpc
    )
/*++

Routine Description:

    This Dpc gets queued to complete transfers in below cases:
        - After the Stop Endpoint process due to cancellation is done.
        - A transfer that was about to be completed got canceled, and a cancel
          callback arrived later, which queued this Dpc for completion.
        - A transfer got canceled on the queue.

--*/
{
    BOOLEAN     acknowledgeTransfersReclaimed;
    PBULK_DATA  bulkData;
    LIST_ENTRY  completionList;
    WDFQUEUE    wdfQueue;

    TRY {

        acknowledgeTransfersReclaimed = FALSE;

        wdfQueue = WdfDpcGetParentObject(WdfDpc);
        bulkData = (PBULK_DATA)TR_GetContext(wdfQueue);

        InitializeListHead(&completionList);

        TR_Lock(bulkData);

        if (bulkData->DpcRunning) {
            TR_Unlock(bulkData);
            LEAVE;
        }

        bulkData->DpcRunning = TRUE;

        do {

            APPEND_LIST(&completionList, &bulkData->CompletionTransferList);

            TR_Unlock(bulkData);

            Bulk_CompleteTransfers(bulkData, &completionList);

            TR_Lock(bulkData);

        } while (IsListEmpty(&bulkData->CompletionTransferList) == FALSE);

        if (bulkData->Flags.ReclaimTransfersAndAcknowledge) {

            //
            // Transfers reclaimed acknowledgement should be sent only after ensuring
            // that the number of 'driver owned' requests is 0.
            //
            if (Bulk_DoesDriverOwnRequests(bulkData) == FALSE) {

                bulkData->Flags.ReclaimTransfersAndAcknowledge = 0;
                acknowledgeTransfersReclaimed = TRUE;
            }
        }

        bulkData->DpcRunning = FALSE;

        TR_Unlock(bulkData);

    } FINALLY {

        if (acknowledgeTransfersReclaimed) {

            LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Message to State Machine: TransfersReclaimed",
                       UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(bulkData->EndpointHandle),
                       bulkData->StreamId);

            TR_TransfersReclaimed(bulkData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
BOOLEAN
Bulk_ShouldRoundTransferSizeToPacketSize(
    __in
        PBULK_DATA  BulkData,
    __in
        ULONG       TransferSize,
    __out
        PULONG      PadTransferBufferSize
    )
/*++

Routine Description:

    This routine is used to determine whether the transfer size of the TD presented to
    the controller should be rounded up to to the packet size. It will return TRUE
    if all of these conditions are met:

        - Device is low speed or full speed.
        - Device is behind a TT hub.
        - Endpoint type is interrupt or bulk.
        - Endpoint direction is In.
        - Transfer length is not an even multiple of the packet size.

--*/
{
    if ((Controller_GetDeviceFlags(BulkData->ControllerHandle).RoundSplitTransferSizeToPacketSize) &&
        (UsbDevice_IsBehindTTHub(BulkData->UsbDeviceHandle)) &&
        (Endpoint_GetDirection(BulkData->EndpointHandle) == EndpointDirection_In) &&
        ((UsbDevice_GetDeviceSpeed(BulkData->UsbDeviceHandle) == UsbLowSpeed) ||
         (UsbDevice_GetDeviceSpeed(BulkData->UsbDeviceHandle) == UsbFullSpeed)) &&
        ((TransferSize % Endpoint_GetMaxPacketSize(BulkData->EndpointHandle)) != 0)) {

        *PadTransferBufferSize =
            (Endpoint_GetMaxPacketSize(BulkData->EndpointHandle) -
             (TransferSize % Endpoint_GetMaxPacketSize(BulkData->EndpointHandle)));

        return TRUE;

    } else {

        *PadTransferBufferSize = 0;

        return FALSE;
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_InsertLinkTrb(
    __in
        PBULK_DATA          BulkData,
    __in
        PBULK_STAGE_DATA    StageData,
    __in
        TRB_STAGE           TrbStage,
    __in
        BOOLEAN             FirstTrb
    )
/*++

Routine Description:

    This function inserts a link TRB into the current segment and advances
    to the next segment.

Returns:

    TRUE  - link TRB was inserted correctly.
    FALSE - no segments are available.

--*/
{
    PBUFFER_DATA    nextSegment;
    TRB             localTrb;

    if (IsListEmpty(&StageData->FreeSegmentList)) {
        if (KdRefreshDebuggerNotPresent() == FALSE) {
            DbgBreakPoint();
        }
        return FALSE;
    }

    nextSegment = (PBUFFER_DATA)RemoveHeadList(&StageData->FreeSegmentList);
    InitializeListHead(&nextSegment->ListEntry);

    TR_InitializeLinkTrb(BulkData, TrbStage, &localTrb, FirstTrb);

    localTrb.Link.RingSegmentPointer = nextSegment->LogicalAddress.QuadPart;
    localTrb.Link.ToggleCycle = 0;

    RtlCopyMemory(&BulkData->CurrentSegment[BulkData->CurrentSegmentIndex],
                  &localTrb,
                  sizeof(TRB));

    TR_Lock(BulkData);

    InsertTailList(&StageData->UsedSegmentList, &BulkData->CurrentSegmentBufferData->ListEntry);

    BulkData->CurrentSegmentBufferData = nextSegment;
    BulkData->CurrentSegment           = (PTRB)nextSegment->VirtualAddress;
    BulkData->CurrentSegmentIndex      = 0;
    StageData->TrbRange.LastSegment    = nextSegment;

    TR_Unlock(BulkData);

    return TRUE;
}

FORCEINLINE
BOOLEAN
Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(
    __in
        PBULK_DATA BulkData
    )
{
    if ((Controller_GetDeviceFlags(BulkData->ControllerHandle).Append8NoOpTRBsToEventDataTrbForSSBulkIn) &&
        (UsbDevice_GetDeviceSpeed(BulkData->UsbDeviceHandle) == UsbSuperSpeed) &&
        (Endpoint_GetType(BulkData->EndpointHandle) == EndpointType_BulkIn)) {

        return TRUE;

    } else {

        return FALSE;
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_MapIntoRing(
    __in
        PBULK_STAGE_DATA StageData
    )
{
    PBULK_DATA          bulkData;
    PBULK_TRANSFER_DATA transferData;
    PTRANSFER_URB       transferUrb;
    TRB_STAGE           trbStage;
    ULONG               currentElement;
    ENDPOINT_DIRECTION  dataDirection;
    PTRB                currentTrb;
    TRB                 localTrb;
    ULONG64             elementAddress;
    ULONG               elementLength;
    ULONG               maxTrbLength;
    PTRB                firstTrb;
    ULONG               tdPacketCount;
    ULONG               lengthMapped;
    ULONG               pendingTrbsPerBurst;
    ULONG               padTransferSize;
    PBUFFER_DATA        padTransferBufferData;
    PTD_TRACKING_DATA   tdTrackingData;
    ULONG               i;

    TRY {

        ICE_START_TRACE();

        transferData = StageData->TransferData;
        bulkData     = transferData->BulkData;
        transferUrb  = transferData->TransferUrb;

        currentElement = 0;

        padTransferBufferData =
            Controller_GetScratchTransferBufferData(bulkData->ControllerHandle);

        Bulk_ShouldRoundTransferSizeToPacketSize(bulkData,
                                                 StageData->TransferSize,
                                                 &padTransferSize);

        //
        // For bulk or interrupt endpoints, the dataDirection indicates the
        // direction of the endpoint. This is used only for determining double
        // buffer behavior.
        //
        if (transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) {

            dataDirection = EndpointDirection_In;

        } else {

            dataDirection = EndpointDirection_Out;
        }

        maxTrbLength = MAX_TRB_LENGTH;

        tdPacketCount = TR_GetPacketCount(bulkData, StageData->TransferSize);

        //
        // Cache the initial TRB for this request. This is used in the case where
        // this request needs to be removed from the transfer ring.
        //
        StageData->UsedTrbs = 0;
        elementAddress = 0;
        elementLength = 0;

        lengthMapped = 0;

        firstTrb = &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex];

        pendingTrbsPerBurst = StageData->TrbsPerBurst;

        trbStage = TrbStage_Normal;

        //
        // If a link TRB cannot be inserted into a TD, then always start at the
        // beginning of the transfer ring segment. This is done by first inserting
        // a link TRB into the current transfer ring segment and then creating
        // the TD for this request in the next transfer ring segment.
        //
        if (Controller_GetDeviceFlags(bulkData->ControllerHandle).NoLinkTRBInTD) {

            Bulk_InsertLinkTrb(bulkData,
                               StageData,
                               trbStage,
                               TRUE);
        }

        while (trbStage != TrbStage_Done) {

            NT_ASSERT(bulkData->CurrentSegmentIndex <= bulkData->MaxSegmentIndex);

            currentTrb = &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex];

            RtlZeroMemory(&localTrb, sizeof(TRB));

            //
            // Determine if there is enough room in this transfer ring to fully map
            // the TD fragment. If there's not enough room, insert a Link TRB and
            // advance to the next transfer ring segment.
            //
            // The last trb in the transfer ring is a link trb that points to the
            // beginning of the transfer ring. Take this into account when determining
            // if we are at the end of the transfer ring.
            //
            if ((pendingTrbsPerBurst + bulkData->CurrentSegmentIndex) >
                (bulkData->MaxSegmentIndex)) {

                NT_ASSERT(Controller_GetDeviceFlags(bulkData->ControllerHandle).NoLinkTRBInTD == 0);

                LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                           "%u.%u.%u: Insert: CurrentSegment[%u] 0x%p Link TRB",
                           UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(bulkData->EndpointHandle),
                           bulkData->StreamId, bulkData->CurrentSegmentIndex,
                           &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex]);

                Bulk_InsertLinkTrb(bulkData,
                                   StageData,
                                   trbStage,
                                   (currentTrb == firstTrb));
                continue;
            }

            if (Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(bulkData)) {

                //
                // Event Data TRB must be followed by 8 consecutive No-Op TRBs. If there
                // is not enough room in the current segment to accomodate one Event Data
                // TRB followed by 8 No-Op TRBs and a Link TRB, insert a Link TRB and move
                // to the next segment.
                //
                if ((trbStage == TrbStage_FinalEventData) &&
                    ((bulkData->CurrentSegmentIndex + 8 + 1) > bulkData->MaxSegmentIndex)) {

                    NT_ASSERT(Controller_GetDeviceFlags(bulkData->ControllerHandle).NoLinkTRBInTD == 0);

                    LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                               "%u.%u.%u: Insert: CurrentSegment[%u] 0x%p Link TRB",
                               UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(bulkData->EndpointHandle),
                               bulkData->StreamId, bulkData->CurrentSegmentIndex,
                               &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex]);

                    Bulk_InsertLinkTrb(bulkData,
                                       StageData,
                                       trbStage,
                                       (currentTrb == firstTrb));
                    continue;
                }
            }

            LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Insert: CurrentSegment[%u] 0x%p StageData->UsedTrbs %u pendingTrbsPerBurst %u",
                       UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(bulkData->EndpointHandle),
                       bulkData->StreamId, bulkData->CurrentSegmentIndex,
                       &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex],
                       StageData->UsedTrbs, pendingTrbsPerBurst);

            switch (trbStage) {

            case TrbStage_Normal:

                localTrb.Normal.Type = TRB_TYPE_NORMAL;

                switch (transferData->TransferMechanism) {

                case TransferMechanism_NoData:

                    localTrb.Normal.TransferLength = 0;
                    trbStage = TrbStage_FinalEventData;
                    break;

                case TransferMechanism_ImmediateData:

                    NT_ASSERT(StageData->DoubleBufferData == NULL);
                    NT_ASSERT(StageData->Buffer != NULL);

                    __analysis_assume(StageData->TransferSize <= IMMEDIATE_DATA_LENGTH);

                    RtlCopyMemory(&localTrb.Normal.DataBufferPointer,
                                  StageData->Buffer,
                                  StageData->TransferSize);

                    localTrb.Normal.ImmediateData = 1;
                    localTrb.Normal.TransferLength = StageData->TransferSize;

                    trbStage = TrbStage_FinalEventData;
                    break;

                case TransferMechanism_DoubleBufferData:

                    if (dataDirection == EndpointDirection_Out) {

                        RtlCopyMemory(StageData->DoubleBufferData->VirtualAddress,
                                      StageData->Buffer,
                                      StageData->TransferSize);
                    }

                    localTrb.Normal.DataBufferPointer =
                        StageData->DoubleBufferData->LogicalAddress.QuadPart;

                    localTrb.Normal.TransferLength = StageData->TransferSize;

                    trbStage = TrbStage_FinalEventData;
                    break;

                case TransferMechanism_Dma:

                    if (elementLength == 0) {

                        elementAddress = StageData->ScatterGatherList->Elements[currentElement].Address.QuadPart;
                        elementLength  = StageData->ScatterGatherList->Elements[currentElement].Length;
                    }

                    if (elementLength <= maxTrbLength) {

                        localTrb.Normal.DataBufferPointer = elementAddress;
                        localTrb.Normal.TransferLength = elementLength;

                        elementLength = 0;
                        currentElement += 1;

                        //
                        // If this is the last entry in the scatter/gather list, then the
                        // next TRB is a status stage TRB. If not, then the next TRB is a
                        // transfer TRB and they need to be chained together.
                        //
                        if (StageData->ScatterGatherList->NumberOfElements == currentElement) {

                            trbStage = TrbStage_FinalEventData;

                        } else {

                            trbStage = TrbStage_Normal;
                        }

                    } else {

                        localTrb.Normal.DataBufferPointer = elementAddress;
                        localTrb.Normal.TransferLength = maxTrbLength;

                        elementAddress += maxTrbLength;
                        elementLength  -= maxTrbLength;

                        trbStage = TrbStage_Normal;
                    }
                    break;
                }

                lengthMapped += localTrb.Normal.TransferLength;
                NT_ASSERT(lengthMapped <= StageData->TransferSize);

                localTrb.Normal.TDSize = TR_CalculateTDSize(bulkData,
                                                            tdPacketCount,
                                                            lengthMapped,
                                                            (trbStage == TrbStage_FinalEventData) ? TRUE : FALSE);

                localTrb.Normal.ChainBit = 1;

                NT_ASSERT(pendingTrbsPerBurst > 0);
                pendingTrbsPerBurst--;
                break;

            case TrbStage_FinalEventData:

#if defined(DBG) && !defined(_WIN64)
                localTrb.EventData.Data  = ((ULONG64)Controller_GetNextEventDataId(bulkData->ControllerHandle) << 32);
                localTrb.EventData.Data |= ((ULONG64)StageData & 0x0FFFFFFFF);
#else
                localTrb.EventData.Data  = (ULONG_PTR)StageData;
#endif

                tdTrackingData = (PTD_TRACKING_DATA)&localTrb.EventData.Data;
                tdTrackingData->EndpointType = Endpoint_GetTransferType(bulkData->EndpointHandle);

                localTrb.EventData.InterruptOnCompletion = 1;
                localTrb.EventData.Type = TRB_TYPE_EVENT_DATA;

                trbStage = TrbStage_Done;
                break;

            case TrbStage_PadTransferBuffer:

                localTrb.Normal.Type = TRB_TYPE_NORMAL;
                localTrb.Normal.TransferLength = padTransferSize;
                localTrb.Normal.DataBufferPointer = padTransferBufferData->LogicalAddress.QuadPart;
                localTrb.Normal.TDSize = 0;
                localTrb.Normal.ChainBit = 1;

                NT_ASSERT(pendingTrbsPerBurst > 0);
                pendingTrbsPerBurst--;
                padTransferSize = 0;

                trbStage = TrbStage_FinalEventData;
                break;
            }

            StageData->UsedTrbs += 1;

            if (trbStage == TrbStage_FinalEventData) {

                if ((padTransferSize != 0) && (padTransferBufferData != NULL)) {

                    localTrb.Normal.TDSize += 1;
                    trbStage = TrbStage_PadTransferBuffer;

                } else {

                    //
                    // If the next state is to build an event data TRB, then set
                    // the ENT bit. This is required by spec:
                    //
                    // 4.12.3 Evaluate Next TRB (ENT)
                    //
                    // Note: System software shall set the ENT flag in the
                    // last Transfer TRB before a terminating Event Data TRB
                    // in a TD on a Stream (Bulk), normal Bulk, Interrupt,
                    // or Control Transfer Ring. This action ensures the
                    // timely execution of an Event Data TRB if the Transfer
                    // Ring is flow controlled.
                    //
                    NT_ASSERT(localTrb.Normal.ChainBit == 1);
                    localTrb.Normal.EvaluateNextTrb = 1;
                    pendingTrbsPerBurst = 1;
                }
            }

            //
            // All of the Trbs for a TD fragment have been completed. Reset the
            // number of pending Trbs.
            //
            if (trbStage == TrbStage_Normal) {

                if (pendingTrbsPerBurst == 0) {

                    pendingTrbsPerBurst = Min(StageData->TrbsPerBurst,
                                              StageData->RequiredTrbs - StageData->UsedTrbs);
                }
            }

            //
            // Only set the cycle bit if the current TRB is not the first TRB. The cycle
            // bit in the first TRB is set last, to prevent premature processing of
            // the transfer.
            //
            if (currentTrb != firstTrb) {
                localTrb.Normal.CycleBit = TR_GetCycleState(bulkData);
            }

            //
            // Set the target event ring for this request. The normal TRB is used for all
            // TRB types. This is safe because the interrupter target value in all of the
            // TRBs is located in the same location within the TRB structure.
            //
            localTrb.Normal.InterrupterTarget = bulkData->InterrupterTarget;

            RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

            bulkData->CurrentSegmentIndex += 1;

            if ((trbStage == TrbStage_Done) && Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(bulkData)) {

                for (i = 0; i < 8; i += 1) {

                    RtlZeroMemory(&localTrb, sizeof(TRB));

                    localTrb.Normal.Type = TRB_TYPE_NO_OP;
                    localTrb.Normal.CycleBit = TR_GetCycleState(bulkData);

                    NT_ASSERT(bulkData->CurrentSegmentIndex < bulkData->MaxSegmentIndex);

                    currentTrb = &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex];

                    RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

                    bulkData->CurrentSegmentIndex += 1;
                }
            }
        }

        //
        // The number of used TRBs must always be lower or equal to the number
        // of required TRBs.
        //
        NT_ASSERT(StageData->UsedTrbs <= StageData->RequiredTrbs);

        StageData->TrbRange.LastSegmentEndIndex = bulkData->CurrentSegmentIndex;

        //
        // Invalidate the CycleBit in the next unused TRB, making no assumptions
        // about the current value of its CycleBit. This is because the TransferRing
        // segment is not reinitialized when reused.
        //
        NT_ASSERT(bulkData->CurrentSegmentIndex <= bulkData->MaxSegmentIndex);

        //
        // Since the segment is not reinitialized when reused, ensure that the next
        // TRB is invalid by clearing its CycleBit. Be extra conservative by zero'ing
        // out the rest of the bits in the TRB.
        //
        currentTrb = &bulkData->CurrentSegment[bulkData->CurrentSegmentIndex];
        RtlZeroMemory(currentTrb, sizeof(TRB));

        TR_Lock(bulkData);

        transferData->StagesMapped  += 1;
        transferData->BytesMapped   += StageData->TransferSize;
        bulkData->PendingStageCount += 1;

        bulkData->Flags.DoorbellRungSinceMappingStart = 1;

        TR_Unlock(bulkData);

        //
        // Set the cycle bit in the first TRB for the transfer to start
        // the transferr of this request.
        //
        Register_ToggleTrbCycleBit(firstTrb);

        UsbDevice_WriteDoorbell(Endpoint_GetUsbDeviceHandle(bulkData->EndpointHandle),
                                Endpoint_GetContextIndex(bulkData->EndpointHandle),
                                bulkData->StreamId);

    } FINALLY {

        ICE_STOP_TRACE();
    }

    return;
}

VOID
Bulk_EvtDmaCallback(
    __in
        PDEVICE_OBJECT          DeviceObject,
    __in
        PIRP                    Irp,
    __in
        PSCATTER_GATHER_LIST    ScatterGatherList,
    __in
        PVOID                   Context
    )
{
    PBULK_DATA          bulkData;
    MAPPING_STATE       previousState;
    PBULK_STAGE_DATA    stageData;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    TRY {

        stageData = (PBULK_STAGE_DATA)Context;
        bulkData  = stageData->TransferData->BulkData;

        stageData->ScatterGatherList = ScatterGatherList;

        Bulk_Stage_MapIntoRing(stageData);

    } FINALLY {

        if (InterlockedXor(&bulkData->ContinueMapping, 1) == 1) {

            previousState = TR_AttemptStateChange(bulkData, MS_Mapping, MS_Mapping);

            if (previousState == MS_Mapping) {
                Bulk_MapTransfers(bulkData);
            }
        }

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_RetrieveNextStage(
    __in
        PBULK_DATA BulkData
    )
/*++

Routine Description:

    Finds the next stage of a transfer to process and saves it in the BulkData->CurrentStage
    on success.

    Reasons for failure:

        1. Limit for outstanding stages per transferring is reached.
        2. The last pending transfer has more stages to program, but has reached the limit of
           outstanding stages per transfer.
        3. There are no new transfers available to program stages for.

    Reasons for success:

        1. A new transfer became available.
        2. The last pending transfer has more stages to program and has not reached the limit
           of outstanding stages per transfer.
        3. There is already a stage to be programmed. This stage was pended previously
           because it was waiting on CommonBuffer.

--*/
{
    BOOLEAN             result;
    NTSTATUS            status;
    PBULK_TRANSFER_DATA transferData;
    WDFREQUEST          wdfRequest;

    TRY {

        status                 = STATUS_SUCCESS;
        BulkData->CurrentStage = NULL;

        TR_Lock(BulkData);

        //
        // Fail if the pending stage limit is reached.
        //
        if (BulkData->PendingStageCount == BulkData->MaxPendingStages) {

            status = STATUS_UNSUCCESSFUL;
            LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Bulk Pending stage limit %u reached, not mapping more stages",
                       UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(BulkData->EndpointHandle),
                       BulkData->StreamId, BulkData->MaxPendingStages);

            TR_Unlock(BulkData);
            LEAVE;
        }

        if (IsListEmpty(&BulkData->PendingTransferList) == FALSE) {

            transferData = (PBULK_TRANSFER_DATA)BulkData->PendingTransferList.Blink;

            //
            // The last pending transfer has more stages to map.
            //
            if (transferData->BytesMapped < transferData->BytesTotal) {

                if (BulkData->CommonBufferAcquireAttemptCount == BULK_MAX_COMMONBUFFER_ACQUIRE_ATTEMPTS) {

                    if (transferData->StagesMapped == transferData->StagesCompleted) {

                        Bulk_Transfer_CompleteCancelable(BulkData,
                                                         transferData,
                                                         USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                         TRUE);

                    } else {

                        transferData->BytesMapped = transferData->BytesTotal;

                        TR_Unlock(BulkData);
                        LEAVE;
                    }

                } else {

                    BulkData->CurrentStage = Bulk_Stage_Acquire(transferData);

                    if (BulkData->CurrentStage != NULL) {

                        LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                                "%u.%u.%u: WdfRequest 0x%p, start mapping next stage 0x%p",
                                UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                                Endpoint_GetContextIndex(BulkData->EndpointHandle),
                                BulkData->StreamId, transferData->WdfRequest, BulkData->CurrentStage);

                    } else {

                        status = STATUS_UNSUCCESSFUL;
                        LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                                   "%u.%u.%u: WdfRequest 0x%p, no free stages available to map next stage",
                                   UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(BulkData->EndpointHandle),
                                   BulkData->StreamId, transferData->WdfRequest);
                    }

                    TR_Unlock(BulkData);
                    LEAVE;
                }
            }
        }

        TR_Unlock(BulkData);

        WHILE (TRUE) {

            status = WdfIoQueueRetrieveNextRequest(BulkData->WdfQueue, &wdfRequest);

            if (!NT_SUCCESS(status)) {

                LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                           "%u.%u.%u: WdfIoQueueRetrieveNextRequest %!STATUS!",
                           UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(BulkData->EndpointHandle),
                           BulkData->StreamId, status);
                LEAVE;
            }

            transferData = &(GetRequestData(wdfRequest)->BulkTransferData);

            Bulk_TransferData_Initialize(BulkData, transferData, wdfRequest);

            Bulk_TransferData_DetermineTransferMechanism(transferData);

            status = Bulk_TransferData_ConfigureBuffer(transferData);

            if (!NT_SUCCESS(status)) {

                LogWarning(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                           "%u.%u.%u: WdfRequest 0x%p, Bulk_TransferData_Initialize failed %!STATUS!",
                           UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(BulkData->EndpointHandle),
                           BulkData->StreamId, wdfRequest, status);

                Bulk_Transfer_Complete(BulkData, transferData, USBD_STATUS_INSUFFICIENT_RESOURCES);
                continue;
            }

            transferData->CancelState = TCS_Cancelable;

            status = WdfRequestMarkCancelableEx(transferData->WdfRequest, Bulk_WdfEvtRequestCancel);

            if (!NT_SUCCESS(status)) {

                NT_ASSERT(status == STATUS_CANCELLED);

                LogWarning(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                           "%u.%u.%u: WdfRequest 0x%p WdfRequestMarkCancelableEx failed %!STATUS!",
                           UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId,
                           transferData->WdfRequest, status);

                transferData->CancelState = TCS_Canceled;

                Bulk_Transfer_Complete(BulkData, transferData, USBD_STATUS_CANCELED);
                continue;
            }

            break;
        }

        BulkData->CurrentStage = Bulk_Stage_Acquire(transferData);

        if (BulkData->PendingStageCount == 0) {

            BulkData->InterrupterTarget =
                Interrupter_GetInterrupterTarget(
                    Controller_GetInterrupterHandle(BulkData->ControllerHandle),
                    transferData->TransferUrb->UrbData.ProcessorNumber);
        }

        LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                "%u.%u.%u: WdfRequest 0x%p, start mapping first stage 0x%p",
                UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                Endpoint_GetContextIndex(BulkData->EndpointHandle),
                BulkData->StreamId, transferData->WdfRequest, BulkData->CurrentStage);

        TR_Lock(BulkData);
        InsertTailList(&BulkData->PendingTransferList, &transferData->ListEntry);
        TR_Unlock(BulkData);

    } FINALLY {

        if (NT_SUCCESS(status)) {
            NT_ASSERT(BulkData->CurrentStage != NULL);
            result = TRUE;
        } else {
            NT_ASSERT(BulkData->CurrentStage == NULL);
            result = FALSE;
        }
    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Bulk_Stage_AcquireMdl(
    __inout
        PBULK_STAGE_DATA StageData
    )
/*++

Routine Description:

    In the general case of multiple stages per transfer and Chained MDLs, pick up
    where we left off by finding the location in the buffer where this stage needs
    to continue the transfer, and allocating a partial MDL, if that location is not
    on an MDL boundary.

--*/
{
    PUCHAR              dataAddress;
    ULONG               dataLength;
    PBULK_DATA          bulkData;
    PMDL                startMdl;
    ULONG               startOffset;
    NTSTATUS            status;
    PBULK_TRANSFER_DATA transferData;

    TRY {

        status       = STATUS_SUCCESS;
        transferData = StageData->TransferData;
        bulkData     = transferData->BulkData;
        startMdl     = transferData->Mdl;
        startOffset  = transferData->BytesMapped;

        while (startOffset >= MmGetMdlByteCount(startMdl)) {

            startOffset -= MmGetMdlByteCount(startMdl);
            startMdl     = startMdl->Next;
        }

        NT_ASSERT(startMdl != NULL);

        //
        // No need to create a partial MDL if start location is on an MDL boundary.
        //
        if (startOffset == 0) {
            StageData->Mdl = startMdl;
            LEAVE;
        }

        dataAddress = (PUCHAR)MmGetMdlVirtualAddress(startMdl) + startOffset;
        dataLength  = MmGetMdlByteCount(startMdl) - startOffset;

        StageData->Mdl = IoAllocateMdl(dataAddress, dataLength, FALSE, FALSE, NULL);

        if (StageData->Mdl == NULL) {

            LogError(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                     "%u.%u.%u: WdfRequest 0x%p, Failed to allocate partial Mdl for stage 0x%p",
                     UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId,
                     transferData->WdfRequest, StageData);

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        StageData->FreeMdl = TRUE;

        IoBuildPartialMdl(startMdl, StageData->Mdl, dataAddress, dataLength);

        //
        // Point the partial MDL to the next MDL in case there are chained-MDLs.
        //
        StageData->Mdl->Next = startMdl->Next;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_DetermineSize(
    __in
        PBULK_DATA          BulkData,
    __in
        PBULK_TRANSFER_DATA TransferData,
    __in
        PBULK_STAGE_DATA    StageData
    )
/*++

Routine Description:

    Determine the number of bytes to map for the supplied stage. Constraints on the
    size of a stage:

    1. Number of bytes left to map for the transfer.
    1. Number of DMA map registers that are available per DMA transaction.

--*/
{
    ULONG               stageSize;
    ULONG               dmaLimit;
    ULONG               stageLimit;
    PDMA_ADAPTER        dmaAdapter;
    DMA_TRANSFER_INFO   dmaTransferInfo;
    PTRANSFER_URB       transferUrb;

    TRY {

        stageSize = TransferData->BytesTotal - TransferData->BytesMapped;

        if (TransferData->TransferMechanism != TransferMechanism_Dma) {
            NT_ASSERT(stageSize <= BulkData->MaxStageSize);
            LEAVE;
        }

        stageLimit = BulkData->MaxStageSize;
        transferUrb = TransferData->TransferUrb;

        dmaAdapter =
            CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(BulkData->ControllerHandle));

        RtlZeroMemory(&dmaTransferInfo, sizeof(DMA_TRANSFER_INFO));
        dmaTransferInfo.Version = DMA_TRANSFER_INFO_VERSION1;

        //
        // Use GetDmaTransferInfo() to determine the actual number of map registers
        // that a given transfer MDL is going to require. If the number of map
        // registers required is less than or equal to the MapRegisterCount, there
        // is no need to break this transfer MDL into more stages.
        //

        (VOID)dmaAdapter->DmaOperations->GetDmaTransferInfo(
            dmaAdapter,
            StageData->Mdl,
            0,
            stageSize,
            ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0),
            &dmaTransferInfo);

        if (dmaTransferInfo.V1.MapRegisterCount > BulkData->MapRegisterCount) {

            dmaLimit = (BulkData->MapRegisterCount - 1) * PAGE_SIZE;
            if (dmaLimit < stageLimit) {
                stageLimit = dmaLimit;
            }
        }

        if (stageSize > stageLimit) {
            stageSize = stageLimit;
        }

    } FINALLY {

        StageData->TransferSize = stageSize;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_EstimateRequiredTrbs(
    __in
        PBULK_STAGE_DATA StageData
    )
/*++

Routine Description:

    Given the size of the stage and transfer mechanism, estimate the number of TRBs
    required to program the transfer.

Side effects:

    Updates StageData->RequiredTrbs and StageData->TrbsPerBurst.

--*/
{
    PBULK_DATA          bulkData;
    ULONG               maxBurstPayload;
    ULONG               padTransferSize;
    PBULK_TRANSFER_DATA transferData;

    transferData = StageData->TransferData;
    bulkData     = transferData->BulkData;

    if ((transferData->TransferMechanism == TransferMechanism_NoData) ||
        (transferData->TransferMechanism == TransferMechanism_ImmediateData) ||
        (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

        //
        // Normal TRB + Event Data TRB
        //
        StageData->RequiredTrbs = 2;
        StageData->TrbsPerBurst = 2;

    } else {

        maxBurstPayload = Endpoint_GetMaxPayloadPerServiceOpportunity(bulkData->EndpointHandle);

        if (maxBurstPayload > StageData->TransferSize) {
            maxBurstPayload = StageData->TransferSize;
        }

        StageData->TrbsPerBurst = BufferSizeToSpanPages(maxBurstPayload) +  // Normal TRBs.
                                  1;                                        // Event Data TRBs.

        if (TR_IsUrbUsingChainedMdl(transferData->TransferUrb) == FALSE) {

            StageData->RequiredTrbs =
                ADDRESS_AND_SIZE_TO_SPAN_PAGES(
                    MmGetMdlVirtualAddress(StageData->Mdl),
                    StageData->TransferSize);

        } else {

            //
            // WIN8 behavior: Some logic seems questionable.
            //
            PMDL    currentMdl;
            ULONG   remainingLength;
            ULONG   currentLength;
            ULONG   fragmentTrbs;
            ULONG   fragmentLength;
            PUCHAR  fragmentBuffer;
            ULONG   previousLength;
            ULONG   previousTrbs;

            currentMdl = StageData->Mdl;
            remainingLength = StageData->TransferSize;

            fragmentTrbs = 0;
            previousTrbs = 0;
            previousLength = 0;

            while (remainingLength != 0) {

                //
                // It is possible to have chained MDLs that exceed the requested
                // number of bytes to transfer. In this case, only calculate the
                // number of required TRBs necessary for the MDLs that will be
                // transferred (those that fall within StageData->TransferSize).
                //
                if (remainingLength >= MmGetMdlByteCount(currentMdl)) {

                    currentLength = MmGetMdlByteCount(currentMdl);
                    remainingLength -= currentLength;

                } else {

                    currentLength = remainingLength;
                    remainingLength = 0;
                }

                //
                // Iterate through each MDL in burst size increments and determine
                // how many TRBs are required by each burst, assuming the worst case
                // that a new TRB is needed at each page boundary. If this value exceeds
                // the maximum number of TRBs available in a transfer ring segment
                // then a failure status is returned.
                //
                fragmentLength = currentLength;

                while (fragmentLength) {

                    fragmentBuffer = ((PUCHAR)MmGetMdlVirtualAddress(currentMdl) +
                                      (currentLength - fragmentLength));

                    //
                    // This MDL exceeds the max burst payload size. Determine how many TRBs
                    // are required for each burst.
                    //
                    if ((fragmentLength + previousLength) >= maxBurstPayload) {

                        fragmentTrbs = ADDRESS_AND_SIZE_TO_SPAN_PAGES(fragmentBuffer,
                                                                      (maxBurstPayload - previousLength));

                        fragmentTrbs += previousTrbs;

                        fragmentLength -= (maxBurstPayload - previousLength);

                        previousLength = 0;
                        previousTrbs = 0;

                    } else {

                        fragmentTrbs = ADDRESS_AND_SIZE_TO_SPAN_PAGES(fragmentBuffer,
                                                                      fragmentLength);

                        //
                        // This fragment may span MDLs, if it's not the last MDL in
                        // the chain. Cache the amount that was processed by this fragment.
                        //
                        previousTrbs = fragmentTrbs;
                        previousLength = fragmentLength;

                        fragmentLength = 0;
                    }

                    StageData->RequiredTrbs += fragmentTrbs;

                    LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                               "%u.%u.%u: fragmentBuffer 0x%p fragmentTrbs %u fragmentLength %u previousTrbs %u previousLength %u StageData->RequiredTrbs %u",
                               UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(bulkData->EndpointHandle),
                               bulkData->StreamId,
                               fragmentBuffer, fragmentTrbs, fragmentLength, previousTrbs,
                               previousLength, StageData->RequiredTrbs);

                    //
                    // If the number of fragment TRBs exceed the number of TRBs that
                    // can fit in a single TRB segment, then the request needs to
                    // be failed.
                    //
                    if (fragmentTrbs >= bulkData->MaxSegmentIndex) {

                        LogWarning(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                                   "%u.%u.%u: Number of fragment TRBs %u exceeds max TRBs per ring %u",
                                   UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(bulkData->EndpointHandle),
                                   bulkData->StreamId,
                                   fragmentTrbs, bulkData->MaxSegmentIndex);
                        //status = STATUS_UNSUCCESSFUL;
                        //LEAVE;
                        NT_ASSERTMSG("Not implemented", 0);
                    }

                    //
                    // This fragment requires more trbs per burst than what was
                    // previously calculated.
                    //
                    if (fragmentTrbs > StageData->TrbsPerBurst) {
                        StageData->TrbsPerBurst = fragmentTrbs;
                        LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                                   "%u.%u.%u: StageData->TrbsPerBurst %u",
                                   UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(bulkData->EndpointHandle),
                                   bulkData->StreamId, StageData->TrbsPerBurst);
                    }
                }

                currentMdl = currentMdl->Next;
            }
        }

        //
        // Event Data TRB.
        //
        StageData->RequiredTrbs += 1;
    }

    if (Bulk_ShouldRoundTransferSizeToPacketSize(bulkData,
                                                 StageData->TransferSize,
                                                 &padTransferSize)) {

        StageData->TrbsPerBurst += 1;
        StageData->RequiredTrbs += 1;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_EstimateRequiredSegments(
    __in
        PBULK_STAGE_DATA    StageData,
    __out
        PULONG              RequiredSegmentCountEstimate
    )
{
    ULONG               availableTrbsInCurrentSegment;
    PBULK_DATA          bulkData;
    ULONG               estimatedSegments;
    PBULK_TRANSFER_DATA transferData;
    ULONG               usableTrbsPerSegment;

    transferData = StageData->TransferData;
    bulkData     = transferData->BulkData;

    if (Controller_GetDeviceFlags(bulkData->ControllerHandle).NoLinkTRBInTD) {

        estimatedSegments = 1;

    } else {

        availableTrbsInCurrentSegment  = bulkData->MaxSegmentIndex - bulkData->CurrentSegmentIndex;
        availableTrbsInCurrentSegment -= (availableTrbsInCurrentSegment % StageData->TrbsPerBurst);

        LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                   "%u.%u.%u: RequiredTrbs %u MaxSegmentIndex %u CurrentSegmentIndex %u availableTrbs %u",
                   UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(bulkData->EndpointHandle),
                   bulkData->StreamId, StageData->RequiredTrbs, bulkData->MaxSegmentIndex,
                   bulkData->CurrentSegmentIndex, availableTrbsInCurrentSegment);

        if (StageData->RequiredTrbs <= availableTrbsInCurrentSegment) {

            estimatedSegments = 0;

        } else {

            usableTrbsPerSegment =
                bulkData->MaxSegmentIndex - (bulkData->MaxSegmentIndex % StageData->TrbsPerBurst);

            estimatedSegments =
                ((StageData->RequiredTrbs - availableTrbsInCurrentSegment) + (usableTrbsPerSegment - 1)) /
                usableTrbsPerSegment;

            LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Current transfer ring is full: trbsPerRingSegment %u ringSegmentsRequired %u",
                       UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(bulkData->EndpointHandle),
                       bulkData->StreamId, usableTrbsPerSegment, estimatedSegments);
        }
    }

    *RequiredSegmentCountEstimate = estimatedSegments;

    if (Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(bulkData)) {

        //
        // If the workaround is set, overallocate a segment for the worst case
        // scenario : the last segment where the TD ends may not have enough TRBs
        // to fit an Event Data TRB + 8 No-Op TRBs + Link TRB. In that case, the
        // Event Data TRB and the 8 No-Op TRBs will have to be moved to the next
        // segment.
        //
        *RequiredSegmentCountEstimate += 1;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
PROCESS_STAGE_RESULT
Bulk_PrepareStage(
    __in
        PBULK_DATA BulkData
    )
/*++

Routine Description:

    Prepares a stage for mapping by setting up the MDL, calculating and acquiring segments,
    and creating ScatterGatherList.

--*/
{
    ULONG                   estimatedRequiredSegments;
    PBULK_STAGE_DATA        stageData;
    NTSTATUS                status;
    PROCESS_STAGE_RESULT    result;
    PBULK_TRANSFER_DATA     transferData;
    PTRANSFER_URB           transferUrb;

    TRY {

        stageData    = BulkData->CurrentStage;
        transferData = stageData->TransferData;
        transferUrb  = transferData->TransferUrb;

        //
        // Ensure that Buffer, DoubleBufferData or Mdl are appropriately initialized.
        //
        switch (transferData->TransferMechanism) {

        case TransferMechanism_ImmediateData:
            stageData->Buffer = transferData->Buffer + transferData->BytesMapped;
            break;

        case TransferMechanism_DoubleBufferData:
            stageData->Buffer = transferData->Buffer + transferData->BytesMapped;
            stageData->DoubleBufferData = transferData->DoubleBufferData;
            break;

        case TransferMechanism_Dma:
            status = Bulk_Stage_AcquireMdl(stageData);

            if (!NT_SUCCESS(status)) {

                TR_Lock(BulkData);

                Bulk_Stage_Release(BulkData, stageData);

                if (transferData->StagesMapped == transferData->StagesCompleted) {

                    Bulk_Transfer_CompleteCancelable(BulkData,
                                                     transferData,
                                                     USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                     TRUE);
                    result = PSR_TransferCompletedPrematurely;

                } else {

                    transferData->BytesMapped = transferData->BytesTotal;
                    result = PSR_ResourceAllocationFailed;
                }

                TR_Unlock(BulkData);
                LEAVE;
            }
            break;

        case TransferMechanism_NoData:
        default:
            break;
        }

        Bulk_Stage_DetermineSize(BulkData, transferData, stageData);

        Bulk_Stage_EstimateRequiredTrbs(stageData);

        Bulk_Stage_EstimateRequiredSegments(stageData, &estimatedRequiredSegments);

        if (estimatedRequiredSegments == 0) {
            result = PSR_Success;
            LEAVE;
        }

        status = TR_AcquireSegments(BulkData, estimatedRequiredSegments, &stageData->FreeSegmentList);

        if (status == STATUS_PENDING) {

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: WdfRequest 0x%p Stage 0x%p Waiting to acquire %u segments",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId,
                    transferData->WdfRequest, stageData, estimatedRequiredSegments);

            TR_Lock(BulkData);

            Bulk_Stage_Release(BulkData, stageData);

            TR_Unlock(BulkData);

            result = PSR_WaitingOnCommonBuffer;

        } else if (!NT_SUCCESS(status)) {

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: WdfRequest 0x%p Stage 0x%p Failed to acquire %u segments",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId,
                    transferData->WdfRequest, stageData, estimatedRequiredSegments);

            TR_Lock(BulkData);

            Bulk_Stage_Release(BulkData, stageData);

            if (transferData->StagesMapped == transferData->StagesCompleted) {

                Bulk_Transfer_CompleteCancelable(BulkData,
                                                 transferData,
                                                 USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                 TRUE);
                result = PSR_TransferCompletedPrematurely;

            } else {

                transferData->BytesMapped = transferData->BytesTotal;
                result = PSR_ResourceAllocationFailed;
            }

            TR_Unlock(BulkData);

        } else {

            BulkData->CommonBufferAcquireAttemptCount = 0;
            result = PSR_Success;
        }

    } FINALLY {

    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
MAP_STAGE_RESULT
Bulk_MapStage(
    __in
        PBULK_DATA BulkData
    )
/*++

Routine Description:


--*/
{
    PDEVICE_OBJECT      deviceObject;
    PDMA_ADAPTER        dmaAdapter;
    MAP_STAGE_RESULT    result;
    KIRQL               savedIrql;
    PBULK_STAGE_DATA    stageData;
    NTSTATUS            status;
    PBULK_TRANSFER_DATA transferData;
    PTRANSFER_URB       transferUrb;

    stageData    = BulkData->CurrentStage;
    transferData = stageData->TransferData;
    transferUrb  = stageData->TransferData->TransferUrb;

    if (transferData->TransferMechanism != TransferMechanism_Dma) {

        Bulk_Stage_MapIntoRing(stageData);

        result = MSR_Success;

    } else {

        dmaAdapter =
            CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(BulkData->ControllerHandle));

        deviceObject =
            WdfDeviceWdmGetDeviceObject(Controller_GetWdfDeviceObject(BulkData->ControllerHandle));

        KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);

        status = dmaAdapter->DmaOperations->GetScatterGatherList(
                    dmaAdapter,
                    deviceObject,
                    stageData->Mdl,
                    MmGetMdlVirtualAddress(stageData->Mdl),
                    stageData->TransferSize,
                    Bulk_EvtDmaCallback,
                    stageData,
                    ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));

        KeLowerIrql(savedIrql);

        if (NT_SUCCESS(status)) {

            if (InterlockedXor(&BulkData->ContinueMapping, 1) == 1) {
                result = MSR_Success;
            } else {
                result = MSR_WaitingOnDmaCallback;
            }

        } else {

            TR_Lock(BulkData);

            Bulk_Stage_Release(BulkData, stageData);

            if (transferData->StagesMapped == transferData->StagesCompleted) {
                Bulk_Transfer_CompleteCancelable(BulkData,
                                                 transferData,
                                                 USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                 TRUE);
                result = MSR_TransferCompletedPrematurely;
            } else {
                result = MSR_GetScatterGatherListFailed;
            }

            TR_Unlock(BulkData);
        }
    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_MappingLoop(
    __in
        PBULK_DATA BulkData
    )
{
    MAPPING_STATE           previousState;
    PROCESS_STAGE_RESULT    processStageResult;
    MAP_STAGE_RESULT        mapStageResult;
    ULONG                   iteration;
    BOOLEAN                 pause;

    pause = FALSE;

    for (iteration = 0; NOTHING; iteration += 1) {

        LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                   "%u.%u.0: Mapping Loop iteration %u",
                   UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(BulkData->EndpointHandle),
                   iteration);

        BulkData->AttemptMapping = 0;

        //
        // Step 1: Find the next stage to map.
        //
        if (Bulk_RetrieveNextStage(BulkData) == FALSE) {
            pause = TRUE;
            break;
        }

        //
        // Step 2: Prepare the stage to map.
        //
        processStageResult = Bulk_PrepareStage(BulkData);

        if (processStageResult == PSR_TransferCompletedPrematurely) {
            continue;
        }

        if (processStageResult == PSR_WaitingOnCommonBuffer) {
            break;
        }

        if (processStageResult == PSR_ResourceAllocationFailed) {
            pause = TRUE;
            break;
        }

        //
        // Step 3: Map the stage.
        //
        mapStageResult = Bulk_MapStage(BulkData);

        if (mapStageResult == MSR_TransferCompletedPrematurely) {
            continue;
        }

        if (mapStageResult == MSR_WaitingOnDmaCallback) {
            break;
        }

        if (mapStageResult == MSR_GetScatterGatherListFailed) {
            pause = TRUE;
            break;
        }

        previousState = TR_AttemptStateChange(BulkData, MS_Mapping, MS_Mapping);

        if (previousState != MS_Mapping) {

            NT_ASSERT(previousState == MS_Stopping);
            break;
        }
    }

    if (pause) {
        previousState = TR_AttemptStateChange(BulkData, MS_Mapping, MS_Paused);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_MapTransfers(
    __in
        PBULK_DATA BulkData
    )
{
    MAPPING_STATE previousState;
    ULONG         iteration;

    for (iteration = 0; NOTHING; iteration += 1) {

        LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                   "%u.%u.0: Outer Mapping Loop iteration %u",
                   UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(BulkData->EndpointHandle),
                   iteration);

        Bulk_MappingLoop(BulkData);

        if (BulkData->MappingState == MS_Paused) {

            if (BulkData->AttemptMapping == 0) {
                break;
            }

            previousState = TR_AttemptStateChange(BulkData, MS_Paused, MS_Mapping);

            if (previousState != MS_Paused) {
                break;
            }

        } else {
            break;
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Initialize(
    __in
        PBULK_TRANSFER_DATA    TransferData,
    __out
        PBULK_STAGE_DATA       StageData
    )
{
    PBULK_DATA bulkData;

    bulkData = TransferData->BulkData;

    NT_ASSERTMSG("Bulk Transfer Stage address should be atleast 8-byte aligned",
                 (((ULONG_PTR)StageData & 0x07) == 0));

    InitializeListHead(&StageData->FreeSegmentList);
    InitializeListHead(&StageData->UsedSegmentList);

    StageData->TransferData                    = TransferData;
    StageData->TransferSize                    = 0;
    StageData->FreeMdl                         = FALSE;
    StageData->Mdl                             = NULL;
    StageData->ScatterGatherList               = NULL;
    StageData->TrbRange.FirstSegment           = bulkData->CurrentSegmentBufferData;
    StageData->TrbRange.FirstSegmentStartIndex = bulkData->CurrentSegmentIndex;
    StageData->TrbRange.LastSegment            = bulkData->CurrentSegmentBufferData;
    StageData->TrbRange.LastSegmentEndIndex    = bulkData->CurrentSegmentIndex;
}

_Requires_lock_not_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_FreeScatterGatherList(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    )
/*++

Routine Description:

    N.B: Requires that the lock be not held because PutScatterGatherList may cause the
         HAL to invoke a different DMA callback of this same Bulk object, which then
         tries to acquire the lock and deadlocks.

--*/
{
    PDMA_ADAPTER    dmaAdapter;
    KIRQL           savedIrql;
    PTRANSFER_URB   transferUrb;

    transferUrb  = StageData->TransferData->TransferUrb;

    if (StageData->ScatterGatherList) {

        dmaAdapter =
            CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(BulkData->ControllerHandle));

        KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);

        dmaAdapter->DmaOperations->PutScatterGatherList(
            dmaAdapter,
            StageData->ScatterGatherList,
            ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));

        KeLowerIrql(savedIrql);

        StageData->ScatterGatherList = NULL;
    }
}

_Requires_lock_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Free(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    )
{
    NT_ASSERT(StageData->ScatterGatherList == NULL);

    if (StageData->FreeMdl) {

        IoFreeMdl(StageData->Mdl);

        StageData->Mdl     = NULL;
        StageData->FreeMdl = FALSE;
    }

    TR_ReleaseSegments(BulkData, &StageData->FreeSegmentList, TRUE);
    TR_ReleaseSegments(BulkData, &StageData->UsedSegmentList, FALSE);
}

__drv_maxIRQL(DISPATCH_LEVEL)
PBULK_STAGE_DATA
Bulk_Stage_Acquire(
    __inout
        PBULK_TRANSFER_DATA TransferData
    )
{
    PBULK_STAGE_DATA stageData;

    stageData = StageQueue_Acquire(&TransferData->StageQueue);

    if (stageData != NULL) {
        Bulk_Stage_Initialize(TransferData, stageData);
    }

    return stageData;
}

_Requires_lock_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Release(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    )
{
    PBULK_TRANSFER_DATA transferData;

    transferData = StageData->TransferData;

    Bulk_Stage_Free(BulkData, StageData);

    StageQueue_Release(&transferData->StageQueue, StageData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_DetermineTransferMechanism(
    __in
        PBULK_TRANSFER_DATA TransferData
    )
{
    PBULK_DATA          bulkData;
    TRANSFER_MECHANISM  transferMechanism;
    PTRANSFER_URB       transferUrb;

    TRY {

        bulkData    = TransferData->BulkData;
        transferUrb = TransferData->TransferUrb;

        if (TransferData->BytesTotal == 0) {

            transferMechanism = TransferMechanism_NoData;
            LEAVE;
        }

        //
        // Table 73: IDT shall not be set ('1') for TRBs on endpoints that define a Max
        // Packet Size < 8 bytes, or on IN endpoints.
        //
        if ((bulkData->ImmediateDataEnabled) &&
            (TransferData->BytesTotal <= IMMEDIATE_DATA_LENGTH)) {

            NT_ASSERT((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0);
            transferMechanism = TransferMechanism_ImmediateData;
            LEAVE;
        }

        if ((TR_IsUrbUsingChainedMdl(transferUrb) == FALSE) &&
            (TransferData->BytesTotal <= bulkData->DoubleBufferSize)) {

            TransferData->DoubleBufferData = TR_AcquireDoubleBuffer(bulkData);
            if (TransferData->DoubleBufferData != NULL) {
                transferMechanism = TransferMechanism_DoubleBufferData;
                LEAVE;
            }
        }

        transferMechanism = TransferMechanism_Dma;

    } FINALLY {

        TransferData->TransferMechanism = transferMechanism;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Bulk_TransferData_ConfigureBuffer(
    __in
        PBULK_TRANSFER_DATA TransferData
    )
/*++

Routine Description:

    Given the Transfer Mechanism, ensures that either the transfer Buffer or the
    MDL are valid.

--*/
{
    PBULK_DATA      bulkData;
    NTSTATUS        status;
    PTRANSFER_URB   transferUrb;

    TRY {

        status      = STATUS_SUCCESS;
        transferUrb = TransferData->TransferUrb;
        bulkData    = TransferData->BulkData;

        switch (TransferData->TransferMechanism) {

        case TransferMechanism_NoData:
            break;

        case TransferMechanism_ImmediateData:
        case TransferMechanism_DoubleBufferData:

            if (transferUrb->TransferBuffer != NULL) {

                TransferData->Buffer = transferUrb->TransferBuffer;

            } else {

                TransferData->Buffer =
                    MmGetSystemAddressForMdlSafe(transferUrb->TransferBufferMDL, NormalPagePriority);

                if (TransferData->Buffer == NULL) {

                    LogError(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                             "%u.%u.%u: MmGetSystemAddressForMdlSafe (TransferBuffer) returned NULL",
                             UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                             Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId);

                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LEAVE;
                }
            }
            break;

        case TransferMechanism_Dma:

            if (transferUrb->TransferBufferMDL != NULL) {

                TransferData->Mdl = transferUrb->TransferBufferMDL;

            } else {

                TransferData->Mdl = IoAllocateMdl(transferUrb->TransferBuffer,
                                                  TransferData->BytesTotal,
                                                  FALSE,
                                                  FALSE,
                                                  NULL);

                if (TransferData->Mdl == NULL) {

                    if ((transferUrb->TransferFlags & USBD_FORWARD_PROGRESS_TRANSFER) &&
                        (bulkData->ForwardProgressMdl != NULL)) {

                        LogVerbose(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                                   "%u.%u.%u: Using ForwardProgressMdl",
                                   UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId);

                        TransferData->Mdl = bulkData->ForwardProgressMdl;

                        MmInitializeMdl(TransferData->Mdl, transferUrb->TransferBuffer, TransferData->BytesTotal);

                    } else {

                        LogError(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                                 "%u.%u.%u: Failed to allocate Mdl",
                                 UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                                 Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId);

                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }
                }

                MmBuildMdlForNonPagedPool(TransferData->Mdl);
            }
            break;

        default:
            break;
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_Initialize(
    __in
        PBULK_DATA              BulkData,
    __in
        PBULK_TRANSFER_DATA     TransferData,
    __in
        WDFREQUEST              WdfRequest
    )
{
    PTRANSFER_URB           transferUrb;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        transferUrb = (PTRANSFER_URB)wdfRequestParams.Parameters.Others.Arg1;

        //
        // During cancellation, requests that are not canceled are put back on the queue.
        // When they get presented to the driver again, the request context does not need
        // to be reinitialized.
        //
        if (TransferData->Initialized) {

            NT_ASSERT(TransferData->CancelState == TCS_NotCancelable);
            NT_ASSERT(TransferData->WdfRequestStatus == STATUS_PENDING);
            NT_ASSERT(IsListEmpty(&TransferData->ListEntry));
            NT_ASSERT(TransferData->StagesMapped == TransferData->StagesCompleted);

            LEAVE;
        }

        //
        // We have not seen this request before. But since UCX may reuse WDFREQUEST objects,
        // manually zero out the request context.
        //
        RtlZeroMemory(TransferData, sizeof(BULK_TRANSFER_DATA));

        InitializeListHead(&TransferData->ListEntry);

        TransferData->Initialized                     = TRUE;
        TransferData->WdfRequest                      = WdfRequest;
        TransferData->TransferUrb                     = transferUrb;
        TransferData->BulkData                        = BulkData;
        TransferData->CancelState                     = TCS_NotCancelable;
        TransferData->TrbCompletionCode               = TRB_COMPLETION_CODE_INVALID;
        TransferData->WdfRequestStatus                = STATUS_PENDING;
        TransferData->TransferMechanism               = TransferMechanism_NoData;
        TransferData->Mdl                             = NULL;
        TransferData->Buffer                          = NULL;
        TransferData->DoubleBufferData                = NULL;
        TransferData->BytesTotal                      = transferUrb->TransferBufferLength;
        TransferData->BytesTransferred                = 0;
        TransferData->BytesMapped                     = 0;
        TransferData->StagesCompleted                 = 0;
        TransferData->StagesMapped                    = 0;

        StageQueue_Initialize(&TransferData->StageQueue,
                              sizeof(BULK_STAGE_DATA),
                              BULK_MAX_PENDING_STAGES_PER_TRANSFER);

        //
        // This is done to maintain COMPAT with the USB 2.0 stack.
        //
        transferUrb->TransferBufferLength = 0;

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_Free(
    __in
        PBULK_TRANSFER_DATA TransferData
    )
{
    PBULK_DATA bulkData;

    bulkData = TransferData->BulkData;

    NT_ASSERT(TransferData->WdfRequestStatus != STATUS_PENDING);

    NT_ASSERT(StageQueue_GetUsedStageCount(&TransferData->StageQueue) == 0);

    if (TransferData->DoubleBufferData != NULL) {
        TR_ReleaseDoubleBuffer(bulkData, TransferData->DoubleBufferData);
        TransferData->DoubleBufferData = NULL;
    }

    if ((TransferData->Mdl != NULL) &&
        (TransferData->Mdl != TransferData->TransferUrb->TransferBufferMDL) &&
        (TransferData->Mdl != TransferData->BulkData->ForwardProgressMdl)) {

        IoFreeMdl(TransferData->Mdl);
        TransferData->Mdl = NULL;
    }

    TransferData->Initialized = FALSE;
}

VOID
Bulk_Transfer_PrepareForCompletion(
    __in
        PBULK_DATA             BulkData,
    __in
        PBULK_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS            UsbdStatus
    )
/*++

Routine Description:

    This routine prepares the transfer for completion and is responsible for:

    - Updating Status, TransferBufferLength for the URB.
    - Updating performance counters.

--*/
{
    PTRANSFER_URB transferUrb;

    transferUrb = TransferData->TransferUrb;

    NT_ASSERT(TransferData->BytesTransferred <= TransferData->BytesTotal);

    transferUrb->TransferBufferLength = TransferData->BytesTransferred;

    transferUrb->Header.Status =
        TR_GetUsbdStatusFromTrbCompletionCode(TransferData->TrbCompletionCode,
                                              UsbdStatus);

    TransferData->WdfRequestStatus =
        TR_GetNtStatusFromUsbdStatus(transferUrb->Header.Status);

    //
    // Update performance counters.
    //
    BulkData->Counter.TransferCount    += 1;
    BulkData->Counter.BytesTransferred += TransferData->BytesTransferred;

    if (!NT_SUCCESS(TransferData->WdfRequestStatus)) {
        BulkData->Counter.FailedTransferCount += 1;
    }

    LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
            "%u.%u.%u: WdfRequest 0x%p  %!STATUS! BytesTransferred %u",
            UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
            Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId,
            TransferData->WdfRequest, TransferData->WdfRequestStatus, TransferData->BytesTransferred);

    Bulk_TransferData_Free(TransferData);

    return;
}

_Requires_lock_not_held_(BulkData->Lock)
VOID
Bulk_Transfer_Complete(
    __in
        PBULK_DATA             BulkData,
    __in
        PBULK_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS            UsbdStatus
    )
{
    NT_ASSERT((TransferData->CancelState == TCS_NotCancelable) ||
              (TransferData->CancelState == TCS_Canceled));

    Bulk_Transfer_PrepareForCompletion(BulkData,
                                       TransferData,
                                       UsbdStatus);

    WdfRequestComplete(TransferData->WdfRequest, TransferData->WdfRequestStatus);
}

_Requires_lock_held_(BulkData->Lock)
VOID
Bulk_Transfer_CompleteCancelable(
    __in
        PBULK_DATA              BulkData,
    __in
        PBULK_TRANSFER_DATA     TransferData,
    __in
        USBD_STATUS             UsbdStatus,
    __in
        BOOLEAN                 CompleteIfCanceled
    )
{
    NTSTATUS status;

    TRY {

        switch (TransferData->CancelState) {

        case TCS_NotCancelable:
            break;

        case TCS_Cancelable:

            RemoveEntryList(&TransferData->ListEntry);

            status = WdfRequestUnmarkCancelable(TransferData->WdfRequest);

            if (!NT_SUCCESS(status)) {

                NT_ASSERT(status == STATUS_CANCELLED);

                LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                        "%u.%u.%u: WdfRequest 0x%p WdfRequestUnmarkCancelable returned %!STATUS!",
                        UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(BulkData->EndpointHandle),
                        BulkData->StreamId, TransferData->WdfRequest, status);

                TransferData->CancelState = TCS_WaitingForCancelCallback;
                InsertTailList(&BulkData->WaitingForCancelCallbackTransferList, &TransferData->ListEntry);
                LEAVE;

            } else {

                TransferData->CancelState = TCS_NotCancelable;
            }
            break;

        case TCS_Canceled:

            if (CompleteIfCanceled) {

                if (UsbdStatus == USBD_STATUS_NOT_SET) {
                    UsbdStatus = USBD_STATUS_CANCELED;
                }
                RemoveEntryList(&TransferData->ListEntry);

            } else {

                LEAVE;
            }
            break;

        default:
            NT_ASSERTMSG("Incorrect Transfer Cancel State", FALSE);
            break;
        }

        TR_Unlock(BulkData);

        Bulk_Transfer_PrepareForCompletion(BulkData,
                                           TransferData,
                                           UsbdStatus);

        WdfRequestComplete(TransferData->WdfRequest, TransferData->WdfRequestStatus);

        TR_Lock(BulkData);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_ValidateStagePointer(
    __in
        PBULK_DATA          BulkData,
    __in
        PBULK_STAGE_DATA    StagePointerFromHardare
    )
{
    PBULK_STAGE_DATA    stageData;
    PBULK_TRANSFER_DATA transferData;
    BOOLEAN             validPointer;

    TRY {

        validPointer = FALSE;

        if (StagePointerFromHardare == NULL) {

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%d.%d.%d: StageData pointer from hardware is NULL",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId);
            LEAVE;
        }

        TR_Lock(BulkData);

        //
        // WIN8 Behavior: StageData pointer is only valid if it is pointing to the oldest
        // stage. WIN8 logic considered it valid if it pointed to any pending stage.
        //
        FOR_ALL_IN_LIST(BULK_TRANSFER_DATA,
                        &BulkData->PendingTransferList,
                        ListEntry,
                        transferData) {

            StageQueue_ForwardScanStart(&transferData->StageQueue);

            stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

            while (stageData != NULL) {

                if (stageData == StagePointerFromHardare) {

                    validPointer = TRUE;
                    TR_Unlock(BulkData);
                    LEAVE;
                }

                stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
            }
        }

        TR_Unlock(BulkData);

    } FINALLY {

    }

    return validPointer;
}

_Requires_lock_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_IsTransferEventLikelyDuplicate_Internal(
    __in
        PBULK_DATA          BulkData,
    __in
        PHYSICAL_ADDRESS    DequeuePointer,
    __out
        PBOOLEAN            PointsToNoOpTRB
    )
/*++

Routine Description:

    Duplicate detection for transfer events that have EventData == 0. Should be called only after
    determining that the event doesn't point to any pending transfer; this does not examine some
    of the pending segments.

Arguments:

    BulkData - Transfer ring whose currently or recently used segments we will check.

    DequeuePointer - Pointer from the Transfer Event TRB.

Return Value:

    TRUE if, by heuristic, we think DequeuePointer is a duplicate Transfer Event TRB (we may have
    seen the same logical address Pointer before)
    FALSE otherwise (we still don't know what to make of the completion)

--*/
{
    PBULK_TRANSFER_DATA transferData;
    PBULK_STAGE_DATA    stageData;
    BOOLEAN             likelyDuplicate;
    PBUFFER_DATA        segment;
    PTRB                trbVA;
    ULONG               trbIndex;

    TRY {

        *PointsToNoOpTRB = FALSE;

        //
        // Heuristic:
        // Optimize for detecting the following case:
        // - The same Transfer Event TRB (or another one for the same transfer) came twice in a row,
        //   and the stage got freed the first time around, and now we're handling the second time;
        //   and
        // - the stage was the oldest pending stage when it was completed; and
        // - the stage spanned no more than 2 ring segments.
        // Then the Dequeue Pointer is pointing to 1 of 2 ring segments, which we will check:
        // (1) The segment most recently freed back to the transfer ring
        // (2a) (if there are pending stages)  The oldest pending stage's first UsedSegmentList segment
        // (2b) (if there are no pending stages) The current enqueue segment
        // The check for control endpoints is different. In that case there is only 1 segment and we
        // check that one.
        //
        likelyDuplicate = FALSE;

        if (!IsListEmpty(&BulkData->SegmentList)) {

            //
            // Check segment most recently freed back to transfer ring.
            //
            segment = (PBUFFER_DATA)BulkData->SegmentList.Blink;

            if (TR_DoesSegmentContainDequeuePointer(segment,
                                                    0,
                                                    ULONG_MAX,
                                                    DequeuePointer.QuadPart)) {
                likelyDuplicate = TRUE;
                LEAVE;
            }
        }

        stageData = NULL;

        //
        // Find the oldest mapped stage on the ring, if any.
        //
        // WIN8 behavior: Not clear why we are walking through the list when all we need is the
        // oldest transfer;
        //
        FOR_ALL_IN_LIST(BULK_TRANSFER_DATA,
                        &BulkData->PendingTransferList,
                        ListEntry,
                        transferData) {

            StageQueue_ForwardScanStart(&transferData->StageQueue);

            stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

            if (stageData != NULL) {
                break;
            }
        }

        if (stageData == NULL) {


            //
            // No pending stage or it is a control endpoint. Check current enqueue segment (or, for
            // control endpoints, the only segment).
            //
            segment = BulkData->CurrentSegmentBufferData;

            if (TR_DoesSegmentContainDequeuePointer(segment,
                                                    0,
                                                    ULONG_MAX,
                                                    DequeuePointer.QuadPart)) {
                likelyDuplicate = TRUE;
                LEAVE;
            }

        } else {

            //
            // There is a pending transfer on the non-control endpoint. Check oldest pending
            // stage's first segment (can also be its only segment).
            //
            segment = stageData->TrbRange.FirstSegment;

            if (TR_DoesSegmentContainDequeuePointer(segment,
                                                    0,
                                                    ULONG_MAX,
                                                    DequeuePointer.QuadPart)) {
                likelyDuplicate = TRUE;
                LEAVE;
            }
        }

    } FINALLY {

        if (likelyDuplicate) {

            trbIndex =
                (ULONG)((DequeuePointer.QuadPart - segment->LogicalAddress.QuadPart) / sizeof(TRB));

            trbVA =
                ((PTRB)segment->VirtualAddress + trbIndex);

            if (trbVA->Normal.Type == TRB_TYPE_NO_OP) {
                *PointsToNoOpTRB = TRUE;
            }
        }
    }

    return likelyDuplicate;
}

_Requires_lock_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Bulk_ValidateED0TrbPointerOnMismatch(
    __in
        PBULK_DATA          BulkData,
    __in
        PTRANSFER_EVENT_TRB Trb
    )
{
    BOOLEAN pointsToNoOpTRB;

    if (Bulk_IsTransferEventLikelyDuplicate_Internal(BulkData, Trb->Pointer, &pointsToNoOpTRB)) {

        //
        // Avoid resetting the controller for known benign case of duplicate ED=0 event.
        //
        if (Controller_GetDeviceFlags(BulkData->ControllerHandle).DropDuplicateEd0TransferEvents) {

            LogWarning(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Dropping invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                       UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(BulkData->EndpointHandle),
                       BulkData->StreamId, Trb->Pointer.QuadPart);

        } else if (pointsToNoOpTRB &&
                   Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(BulkData) &&
                   Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            LogWarning(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: Dropping Stopped Transfer Event pointing to No-Op TRB at 0x%I64x",
                       UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(BulkData->EndpointHandle),
                       BulkData->StreamId, Trb->Pointer.QuadPart);

        } else {

            LogError(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                     "%u.%u.%u: Invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                     UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(BulkData->EndpointHandle),
                     BulkData->StreamId, Trb->Pointer.QuadPart);

            Controller_ReportFatalError(
                BulkData->ControllerHandle,
                Controller_FatalErrorReset,
                USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0_DUPLICATE,
                BulkData->UsbDeviceHandle,
                BulkData->EndpointHandle,
                BulkData);
        }

    } else {

        LogError(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                 "%u.%u.%u: Invalid transfer event TRB.Pointer 0x%I64x",
                 UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                 Endpoint_GetContextIndex(BulkData->EndpointHandle),
                 BulkData->StreamId, Trb->Pointer.QuadPart);

        Controller_ReportFatalError(
            BulkData->ControllerHandle,
            Controller_FatalErrorReset,
            USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0,
            BulkData->UsbDeviceHandle,
            BulkData->EndpointHandle,
            BulkData);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_DoesStageContainDequeuePointer(
    __in
        PBULK_STAGE_DATA    StageData,
    __in
        LONGLONG            DequeuePointer
    )
{
    ULONG           startIndex;
    PTRB_RANGE      trbRange;
    BOOLEAN         result;
    PBUFFER_DATA    segment;

    TRY {

        result     = FALSE;
        trbRange   = &StageData->TrbRange;
        startIndex = trbRange->FirstSegmentStartIndex;

        FOR_ALL_IN_LIST(BUFFER_DATA, &StageData->UsedSegmentList, ListEntry, segment) {

            if (TR_DoesSegmentContainDequeuePointer(segment,
                                                    startIndex,
                                                    ULONG_MAX,
                                                    DequeuePointer)) {

                result = TRUE;
                LEAVE;
            }

            startIndex = 0;
        }

        if (TR_DoesSegmentContainDequeuePointer(trbRange->LastSegment,
                                                startIndex,
                                                trbRange->LastSegmentEndIndex,
                                                DequeuePointer)) {

            result = TRUE;
        }

    } FINALLY {

    }

    return result;
}

__success(return == TRUE)
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_Stage_ProcessSegment(
    __in
        PBULK_STAGE_DATA    StageData,
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in
        PBUFFER_DATA        Segment,
    __in
        ULONG               SegmentStartIndex,
    __inout
        PULONG              BytesTransferred
    )
/*++

Routine Description:

    Scans the supplied transfer ring segment beginning at SegmentStartIndex to find a
    Transfer TRB that matches the Pointer value in the supplied TRB.

Arguments:

    Trb - Transfer Event TRB with ED=0 whose Pointer value will be used to find the TRB.

    Segment - Supplies the segment to scan.

    SegmentStartIndex - Supplies the location in the segment to start the scan from.

    BytesTransferred - If a matching Transfer TRB was found, receives the number of bytes
        transferred for this Bulk TD.

Side effects:

    Updates transferData->PacketsCompleted and StageData->OnePastLastCompletedPacket.

Return value:

    TRUE if a matching Transfer TRB was found.

--*/
{
    BOOLEAN             trbMatch;
    PTRB                firstTrbVA;
    PTRB                lastTrbVA;
    PTRB                currentTrbVA;
    PHYSICAL_ADDRESS    currentTrbLA;
    PBULK_TRANSFER_DATA transferData;
    PBULK_DATA          bulkData;

    TRY {

        trbMatch              = FALSE;
        transferData          = StageData->TransferData;
        bulkData              = transferData->BulkData;

        firstTrbVA            = (PTRB)Segment->VirtualAddress;
        lastTrbVA             = firstTrbVA + ((Segment->Size / sizeof(TRB)) - 1);
        currentTrbVA          = (PTRB)(Segment->VirtualAddress + (SegmentStartIndex * sizeof(TRB)));
        currentTrbLA.QuadPart = Segment->LogicalAddress.QuadPart + (SegmentStartIndex * sizeof(TRB));

        while (currentTrbVA <= lastTrbVA) {

            if (currentTrbLA.QuadPart == Trb->Pointer.QuadPart) {
                trbMatch = TRUE;
            }

            switch (currentTrbVA->Normal.Type) {

            case TRB_TYPE_NORMAL:

                if (trbMatch) {

                    switch (Trb->CompletionCode) {

                    case TRB_COMPLETION_CODE_STOPPED:
                    case TRB_COMPLETION_CODE_DATA_BUFFER_ERROR:
                    case TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR:
                    case TRB_COMPLETION_CODE_STALL_ERROR:
                    case TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR:
                    case TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR:
                        *BytesTransferred +=
                            (currentTrbVA->Normal.TransferLength - Trb->TransferLength);
                        break;

                    case TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET:
                        *BytesTransferred = Trb->TransferLength;
                        break;

                    case TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID:
                    case TRB_COMPLETION_CODE_INVALID_STREAM_TYPE_ERROR:
                    case TRB_COMPLETION_CODE_INVALID_STREAM_ID_ERROR:
                        break;

                    default:
                        NT_ASSERTMSG("Unexpected TRB completion code", 0);
                        break;
                    }

                } else {
                    *BytesTransferred += currentTrbVA->Normal.TransferLength;
                }
                break;

            case TRB_TYPE_EVENT_DATA:
                if (trbMatch == FALSE) {
                    *BytesTransferred = 0;
                }
                break;

            case TRB_TYPE_NO_OP:
                NT_ASSERT(Bulk_ShouldAppend8NoOpTrbsToEventDataTrb(bulkData) != FALSE);
                break;

            case TRB_TYPE_LINK:
                LEAVE;

            default:
                NT_ASSERTMSG("Unexpected TRB Type encountered while scanning TRB segment", 0);
                break;
            }

            if (trbMatch) {
                break;
            }

            currentTrbVA += 1;
            currentTrbLA.QuadPart += sizeof(TRB);
        }

    } FINALLY {

        NT_ASSERT(currentTrbVA <= lastTrbVA);

    }

    return trbMatch;
}

_Requires_lock_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Bulk_Stage_CalculateBytesTransferred(
    __in
        PBULK_DATA          BulkData,
    __in
        PBULK_STAGE_DATA    StageData,
    __in
        PTRANSFER_EVENT_TRB Trb
    )
/*++

Routine Description:

    Locate the Transfer TRB whose logical address is the Pointer value from the
    supplised Transfer Event TRB with ED=0. Calculate the bytes transferred for
    the Bulk TD that contained the matching TRB.

--*/
{
    ULONG           bytesTransferred;
    PBUFFER_DATA    currentSegment;
    ULONG           startIndex;
    BOOLEAN         trbMatch;
    PTRB_RANGE      trbRange;

    UNREFERENCED_PARAMETER(BulkData);

    TRY {

        bytesTransferred = 0;
        trbRange         = &StageData->TrbRange;
        startIndex       = trbRange->FirstSegmentStartIndex;

        FOR_ALL_IN_LIST(BUFFER_DATA,
                        &StageData->UsedSegmentList,
                        ListEntry,
                        currentSegment) {

            trbMatch = Bulk_Stage_ProcessSegment(StageData,
                                                 Trb,
                                                 currentSegment,
                                                 startIndex,
                                                 &bytesTransferred);

            if (trbMatch) {
                LEAVE;
            }

            startIndex = 0;
        }

        //
        // A match was not found for any segments on the UsedSegmentList. Try the last
        // segment (which can also be the only segment).
        //
        trbMatch = Bulk_Stage_ProcessSegment(StageData,
                                             Trb,
                                             trbRange->LastSegment,
                                             startIndex,
                                             &bytesTransferred);

    } FINALLY {

        NT_ASSERT(trbMatch != FALSE);
    }

    return bytesTransferred;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_FindStage(
    __in
        PBULK_DATA          BulkData,
    __in
        PHYSICAL_ADDRESS    TransferTrbLA,
    __out
        PBULK_STAGE_DATA*   StageData,
    __out
        PULONG              StagesSkipped
    )
/*++

Routine Description:

    Side effects:

--*/
{
    BOOLEAN             trbMatch;
    LONGLONG            dequeuePointer;
    LONGLONG            enqueuePointer;
    PBULK_TRANSFER_DATA transferData;
    PBULK_STAGE_DATA    localStageData;
    ULONG               localSkippedStageCount;

    TRY {

        trbMatch               = FALSE;
        localSkippedStageCount = 0;
        localStageData         = NULL;

        dequeuePointer         = TransferTrbLA.QuadPart;
        enqueuePointer         = (BulkData->CurrentSegmentBufferData->LogicalAddress.QuadPart +
                                  (BulkData->CurrentSegmentIndex * sizeof(TRB)));

        if (dequeuePointer == 0) {

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: TransferEventTrb->Pointer is zero",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId);

            NT_ASSERTMSG("TransferEventTrb->Pointer is zero", 0);
            LEAVE;
        }

        if (dequeuePointer == enqueuePointer) {

            LogInfo(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: TransferRing is empty",
                    UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(BulkData->EndpointHandle), BulkData->StreamId);

            localSkippedStageCount = BulkData->PendingStageCount;
            trbMatch = TRUE;
            LEAVE;
        }

        FOR_ALL_IN_LIST(BULK_TRANSFER_DATA,
                        &BulkData->PendingTransferList,
                        ListEntry,
                        transferData) {

            StageQueue_ForwardScanStart(&transferData->StageQueue);

            localStageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

            while (localStageData != NULL) {

                if (Bulk_DoesStageContainDequeuePointer(localStageData, dequeuePointer)) {

                    LogVerbose(Endpoint_GetRecorderLog(BulkData->EndpointHandle), TransferRing,
                               "%u.%u.%u: WdfRequest 0x%p Matching Stage 0x%p found",
                               UsbDevice_GetSlotId(BulkData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(BulkData->EndpointHandle),
                               BulkData->StreamId, transferData->WdfRequest, localStageData);

                    trbMatch = TRUE;
                    LEAVE;
                }

                localSkippedStageCount += 1;
                localStageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
            }
        }

    } FINALLY {

        if (trbMatch) {
            *StageData     = localStageData;
            *StagesSkipped = localSkippedStageCount;
        } else {
            *StageData     = NULL;
            *StagesSkipped = 0;
        }
    }

    return trbMatch;
}

_Requires_lock_not_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Bulk_ProcessTransferEventWithHaltedCompletion(
    __in
        PBULK_DATA          BulkData,
    __in
        PBULK_STAGE_DATA    StageData,
    __in
        ULONG               CompletionCode,
    __in
        ULONG               BytesTransferred
    )
/*++

Routine Description:

    Processes a Transfer Event TRB with a halted completion code. Completes the
    transfer that failed with halt, if any, and notifies the ESM of the halt.

--*/
{
    BOOLEAN             acknowledgeExpectedEventTRBsProcessed;
    PBULK_TRANSFER_DATA transferData;

    TRY {

        acknowledgeExpectedEventTRBsProcessed = FALSE;

        TR_Lock(BulkData);

        BulkData->Flags.EndpointHalted           = 1;
        BulkData->OutstandingEventCountAfterStop = 1;

        TR_Unlock(BulkData);

        //
        // ESM must be notified of the halt before the client driver, because ESM
        // depends on halt notification coming before client reset request.
        //
        Endpoint_TR_HaltedCompletionCodeReceived(BulkData->EndpointHandle,
                                                 CompletionCode,
                                                 TRUE);

        transferData = StageData->TransferData;

        transferData->TrbCompletionCode = CompletionCode;
        transferData->BytesTransferred += BytesTransferred;

        if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
            (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

            RtlCopyMemory(StageData->Buffer,
                          StageData->DoubleBufferData->VirtualAddress,
                          BytesTransferred);
        }

        Bulk_Stage_FreeScatterGatherList(BulkData, StageData);

        TR_Lock(BulkData);

        Bulk_Stage_Release(BulkData, StageData);

        transferData->StagesCompleted += 1;

        NT_ASSERTMSG("Not implemented: Multiple outstanding stages per transfer",
                     (transferData->StagesCompleted == transferData->StagesMapped));

        Bulk_Transfer_CompleteCancelable(BulkData,
                                         transferData,
                                         USBD_STATUS_NOT_SET,
                                         FALSE);

        BulkData->PendingStageCount              -= 1;
        BulkData->OutstandingEventCountAfterStop -= 1;

        if (BulkData->Flags.AcknowledgeExpectedEventTRBsProcessed) {
            acknowledgeExpectedEventTRBsProcessed = TRUE;
        }

        TR_Unlock(BulkData);

    } FINALLY {

        if (acknowledgeExpectedEventTRBsProcessed) {
            Endpoint_TR_ExpectedEventTRBsProcessed(BulkData->EndpointHandle);
        }
    }
}

_Requires_lock_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Bulk_ProcessTransferEventWithStoppedCompletion(
    __in
        PBULK_DATA          BulkData,
    __in_opt
        PBULK_STAGE_DATA    StageData,
    __in
        ULONG               CompletionCode,
    __in
        ULONG               BytesTransferred,
    __in
        ULONG               SkippedStageCount
    )
{
    PBULK_TRANSFER_DATA transferData;

    if (StageData) {

        transferData = StageData->TransferData;

        transferData->BytesTransferred += BytesTransferred;

        if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
            (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

            RtlCopyMemory(StageData->Buffer,
                          StageData->DoubleBufferData->VirtualAddress,
                          BytesTransferred);
        }

        if (transferData->BytesTransferred == transferData->BytesTotal) {
            transferData->TrbCompletionCode = TRB_COMPLETION_CODE_SUCCESS;
        } else if (CompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET) {
            transferData->TrbCompletionCode = TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET;
        }
    }

    BulkData->OutstandingEventCountAfterStop = SkippedStageCount;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_ProcessTransferEventWithED0(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
{
    PBULK_DATA          bulkData;
    ULONG               bytesTransferred;
    BOOLEAN             eventHandled;
    ULONG               skippedStageCount;
    PBULK_STAGE_DATA    stageData;
    BOOLEAN             trbMatchFound;

    TRY {

        __analysis_assume(Context != NULL);

        eventHandled = FALSE;
        bulkData     = (PBULK_DATA)Context;

        LogInfo(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                "%d.%d.%d: TransferEventTrb 0x%p %!TrbCompletionCode! Length %d EventData %d Pointer 0x%I64x",
                Trb->SlotId, Trb->EndpointId, bulkData->StreamId, Trb, (TRB_COMPLETION_CODE)Trb->CompletionCode,
                Trb->TransferLength, Trb->EventData, Trb->Pointer.QuadPart);

        if (Endpoint_StoppedCompletionCode(Trb->CompletionCode) &&
            Endpoint_TR_ShouldDropFSE(bulkData->EndpointHandle)) {

            eventHandled = TRUE;
            LEAVE;
        }

        TR_Lock(bulkData);

        trbMatchFound = Bulk_FindStage(bulkData,
                                       Trb->Pointer,
                                       &stageData,
                                       &skippedStageCount);

        if (trbMatchFound == FALSE) {

            if (bulkData->StreamId == 0) {
                Bulk_ValidateED0TrbPointerOnMismatch(bulkData, Trb);
            }

            TR_Unlock(bulkData);
            LEAVE;
        }

        //
        // StageData may be NULL event if match is found, if the TRB Pointer is at the
        // EnqueuePointer.
        //
        if (stageData) {

            bytesTransferred = Bulk_Stage_CalculateBytesTransferred(bulkData, stageData, Trb);

            if (bytesTransferred > stageData->TransferSize) {

                LogWarning(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                           "%u.%u.%u: BytesTransferred %u greater than expected %u",
                           UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(bulkData->EndpointHandle),
                           bulkData->StreamId, bytesTransferred, stageData->TransferSize);

                NT_ASSERTMSG("Invalid bytesTransferred", 0);
                bytesTransferred = 0;
            }

        } else {

            bytesTransferred = 0;
        }

        if (Endpoint_HaltedCompletionCode(Trb->CompletionCode)) {

            TR_Unlock(bulkData);

            if (stageData) {

                Bulk_ProcessTransferEventWithHaltedCompletion(bulkData,
                                                              stageData,
                                                              Trb->CompletionCode,
                                                              bytesTransferred);

            } else {

                //
                // This is the case of a Transfer Event TRB with halted completion code
                // pointing to one past the last valid TRB on the Transfer Ring. This is
                // not expected to happen but has been observed on several (older)
                // controllers. WIN8 behavior is preserved here by simply dropping the
                // TRB. Note that since the endpoint is halted and we cannot report it
                // back to the client driver because there is no corresponding transfer
                // to complete, the next transfer will probably timeout and client driver
                // will have to cancel it.
                //
            }

            eventHandled = TRUE;

        } else if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            Bulk_ProcessTransferEventWithStoppedCompletion(bulkData,
                                                           stageData,
                                                           Trb->CompletionCode,
                                                           bytesTransferred,
                                                           skippedStageCount);

            LogInfo(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                    "%u.%u.%u: OutstandingEventCountAfterStop %u",
                    UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId,
                    bulkData->OutstandingEventCountAfterStop);

            bulkData->Flags.FSEReceived = 1;

            TR_Unlock(bulkData);

            Endpoint_TR_FSEReceived(bulkData->EndpointHandle);
            eventHandled = TRUE;

        } else {

            NT_ASSERTMSG("Unrecognized completion code for Transfer Event TRB with ED=0", 0);

            TR_Unlock(bulkData);
        }

    } FINALLY {

    }

    return eventHandled;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_ProcessTransferEventWithED1(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
{
    BOOLEAN             acknowledgeExpectedEventTRBsProcessed;
    PBULK_DATA          bulkData;
    ULONG               bytesTransferred;
    PBULK_STAGE_DATA    matchingStage;
    BOOLEAN             eventHandled;
    MAPPING_STATE       previousState;
    ULONG               skippedStageCount;
    PBULK_STAGE_DATA    stageData;
    BOOLEAN             trbMatchFound;
    PBULK_TRANSFER_DATA transferData;
    BOOLEAN             validPointer;
    PHYSICAL_ADDRESS    transferTrbLA;
    ULONG_PTR           eventData;

    TRY {

        acknowledgeExpectedEventTRBsProcessed = FALSE;
        eventHandled                          = FALSE;

#if defined (_WIN64)
        eventData = (ULONG_PTR)(Trb->Pointer.QuadPart);
#else
        eventData = (ULONG_PTR)(Trb->Pointer.LowPart);
#endif
        eventData &= ~0x03;

        stageData = (PBULK_STAGE_DATA)eventData;

        if (Context != NULL) {

            bulkData = (PBULK_DATA)Context;

            validPointer = Bulk_ValidateStagePointer(bulkData, stageData);

            if (validPointer == FALSE) {

                if (bulkData->StreamId == 0) {

                    NT_ASSERTMSG("EventData pointer is not vaild", 0);

                    LogError(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                             "%u.%u.%u: EventData pointer is not valid: 0x%I64x",
                             Trb->SlotId, Trb->EndpointId, bulkData->StreamId, Trb->Pointer.QuadPart);

                    Controller_ReportFatalError(bulkData->ControllerHandle,
                                                Controller_FatalErrorReset,
                                                USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_1,
                                                bulkData->UsbDeviceHandle,
                                                bulkData->EndpointHandle,
                                                bulkData);
                }
                LEAVE;
            }

            transferData = stageData->TransferData;

        } else {

            transferData = stageData->TransferData;
            bulkData     = transferData->BulkData;
        }

        eventHandled = TRUE;

        LogInfo(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                "%u.%u.%u: TransferEventTrb 0x%p %!TrbCompletionCode! Length %u EventData %u Pointer 0x%I64x",
                Trb->SlotId, Trb->EndpointId, bulkData->StreamId, Trb,
                (TRB_COMPLETION_CODE)Trb->CompletionCode, Trb->TransferLength,
                Trb->EventData, Trb->Pointer.QuadPart);

        //
        // Some controllers have been known to reference incorrect Endpoint Id in their
        // Transfer Event TRB. See Windows 8 Bug 430453 for details.
        //
        NT_ASSERTMSG("Transfer Event TRB points to incorrect EndpointId",
                     (Trb->EndpointId == Endpoint_GetContextIndex(bulkData->EndpointHandle)));

        //
        // Some controllers have been known to reference incorrect Slot Id in their
        // Transfer Event TRB. See Windows 8 Bug 431065 for details.
        //
        NT_ASSERTMSG("Transfer Event TRB points to incorrect SlotId",
                     (Trb->SlotId == UsbDevice_GetSlotId(bulkData->UsbDeviceHandle)));

        bytesTransferred = Trb->TransferLength;

        if (bytesTransferred > stageData->TransferSize) {

            LogWarning(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: BytesTransferred %u greater than expected %u",
                       UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(bulkData->EndpointHandle),
                       bulkData->StreamId, bytesTransferred, stageData->TransferSize);

            NT_ASSERTMSG("Invalid bytesTransferred", 0);
            bytesTransferred = 0;
        }

        LogInfo(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                "%u.%u.%u: WdfRequest 0x%p transferData 0x%p stageData 0x%p",
                UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                Endpoint_GetContextIndex(bulkData->EndpointHandle), bulkData->StreamId,
                transferData->WdfRequest, transferData, stageData);

        if (Endpoint_HaltedCompletionCode(Trb->CompletionCode)) {









            LogError(Endpoint_GetRecorderLog(bulkData->EndpointHandle), TransferRing,
                       "%u.%u.%u: HW_COMPLIANCE Endpoint_HaltedCompletionCode %u",
                       UsbDevice_GetSlotId(bulkData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(bulkData->EndpointHandle),
                       bulkData->StreamId, Trb->CompletionCode);

            Bulk_ProcessTransferEventWithHaltedCompletion(bulkData,
                                                          stageData,
                                                          Trb->CompletionCode,
                                                          bytesTransferred);

        } else if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            skippedStageCount = 0;

            transferTrbLA.QuadPart =
                stageData->TrbRange.FirstSegment->LogicalAddress.QuadPart +
                (stageData->TrbRange.FirstSegmentStartIndex * sizeof(TRB));

            TR_Lock(bulkData);

            trbMatchFound = Bulk_FindStage(bulkData,
                                           transferTrbLA,
                                           &matchingStage,
                                           &skippedStageCount);

            if (trbMatchFound && (matchingStage == stageData)) {

                Bulk_ProcessTransferEventWithStoppedCompletion(bulkData,
                                                               stageData,
                                                               Trb->CompletionCode,
                                                               bytesTransferred,
                                                               skippedStageCount);

            } else {
                NT_ASSERT(FALSE);
            }

            bulkData->Flags.FSEReceived = 1;

            TR_Unlock(bulkData);

            Endpoint_TR_FSEReceived(bulkData->EndpointHandle);

        } else {

            transferData->TrbCompletionCode = Trb->CompletionCode;
            transferData->BytesTransferred += bytesTransferred;

            if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
                (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

                RtlCopyMemory(stageData->Buffer,
                              stageData->DoubleBufferData->VirtualAddress,
                              bytesTransferred);
            }

            Bulk_Stage_FreeScatterGatherList(bulkData, stageData);

            TR_Lock(bulkData);

            Bulk_Stage_Release(bulkData, stageData);

            transferData->StagesCompleted += 1;

            if (transferData->TrbCompletionCode == TRB_COMPLETION_CODE_SHORT_PACKET) {

                NT_ASSERTMSG("Not implemented: Multiple outstanding stages per transfer",
                             (transferData->StagesCompleted == transferData->StagesMapped));

                Bulk_Transfer_CompleteCancelable(bulkData,
                                                 transferData,
                                                 USBD_STATUS_NOT_SET,
                                                 FALSE);

            } else {

                if (transferData->BytesMapped == transferData->BytesTotal) {

                    Bulk_Transfer_CompleteCancelable(bulkData,
                                                     transferData,
                                                     USBD_STATUS_NOT_SET,
                                                     FALSE);
                }
            }

            bulkData->PendingStageCount -= 1;

            //
            // OutstandingEventCountAfterStop will be set to a non-zero value if a Stopped
            // Event was processed before this Event.
            //
            if (bulkData->OutstandingEventCountAfterStop != 0) {

                bulkData->OutstandingEventCountAfterStop -= 1;

                if ((bulkData->Flags.AcknowledgeExpectedEventTRBsProcessed) &&
                    (bulkData->OutstandingEventCountAfterStop == 0)) {

                    acknowledgeExpectedEventTRBsProcessed = TRUE;
                }
            }

            TR_Unlock(bulkData);
        }

    } FINALLY {

        if (acknowledgeExpectedEventTRBsProcessed) {
            Endpoint_TR_ExpectedEventTRBsProcessed(bulkData->EndpointHandle);
        }

        //
        // Unconditionally attempt to restart mapping in case it had been paused because
        // maximum number of stages had been mapped.
        //

        InterlockedExchange(&bulkData->AttemptMapping, 1);

        previousState = TR_AttemptStateChange(bulkData, MS_Paused, MS_Mapping);

        if (previousState == MS_Paused) {
            Bulk_MapTransfers(bulkData);
        }
    }

    return eventHandled;
}
