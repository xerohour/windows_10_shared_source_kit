
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Isoch.c

Abstract:

    Compared to WIN8, Isoch TransferRing implementation has been separated out from other
    endpoint types.


    ---------------------------------------------------------------------
    Differences in Isoch TransferRing compared to other endpoint types:
    ---------------------------------------------------------------------

    1. No zero length transfers, No ImmediateData, No DoubleBuffering.

    2. No Forward Progress support.

    3. No halt handling needed.

    4. Workarounds that don't make sense for Isoch:
        - NoLinkTRBInTD (because TDs never cross segments).
        - DisableImmediateData
        - SerializeTDBulkInterrupt
        - DataStageTrbMax512

    5. Workarounds that only apply to Isoch:
        - DisableBEI
        - ExpectLackOfMseEvents
        - PreventSchedulingLateIsochTd
        - PreventStandaloneLinkTDForFullSpeedIsoch

    6. During cancellation, all transfers that are not canceled, get completed instead of
       getting requeued. This implies a simplification:
        - Every request retrieved from the WDFQUEUE is fresh, not a requeued one.

    7. A given stage will have more than one TD, as opposed to Bulk/Int where there is
       a 1:1 relationship between stage and TD.


    ------------------------------------------------
    Functional changes from the WIN8 implementation:
    ------------------------------------------------

    1. Support simultaneous multiple outstanding stages for a single transfer.

    2. Removed ISOCH_LOG, never utilized it in two years.

    3. Old code was (incorrectly) not setting any status other than USBD_STATUS_SUCCESS
       or USBD_STATUS_ISOCH_REQUEST_FAILED in the URB header. New code sets other status
       like USBD_STATUS_BAD_START_FRAME, USBD_STATUS_INSUFFICIENT_RESOURCES, etc.

    4. Replaced TRANSFER_STATE with TRANSFER_CANCEL_STATE. i.e. The only state tracking for
       transfers is for cancellation, not things like PendingResources, IsochLate, etc.
       which are handled by the TransferRing state machine now.

    5. Completion of canceled transfers is done on a different DPC than the interrupt DPC,
       which keeps the interrupt Dpc routine from doing unbounded amount of work in the
       processing of a single Event TRB.


    ------------
    Open issues:
    ------------

    1. If the number of segments is underestimated, stage needs to rollback in a way that
       it still ends on a frame boundary.


    --------------------------------------------------
    Unnatural hardware behavior:
    --------------------------------------------------








--*/

#include "pch.h"
#include "Isoch.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Isoch_GetConfiguration)
#pragma alloc_text(PAGE, Isoch_Initialize)
#endif

#define ABS(x) ( (0 < (x)) ? (x) : (0 - (x)))

CONST TR_FUNCTION_TABLE IsochFunctionTable = {

    Isoch_Initialize,
    Isoch_Cleanup,
    Isoch_CommonBufferCallback,
    Isoch_EP_Enable,
    Isoch_EP_Disable,
    NULL,                                           // EP_EnableForwardProgress
    Isoch_EP_AreTransfersPending,
    Isoch_EP_OkToReclaimTransfersOnCancel,
    Isoch_EP_ReclaimTransfers,
    NULL,                                           // EP_IsTransferEventLikelyDuplicate
    Isoch_EP_TransferEventHandler,
    Isoch_EP_StartMapping,
    Isoch_EP_StopMapping,
    Isoch_EP_IsMappingStoppedOrPaused,
    NULL,                                           // EP_EndpointHalted
    Isoch_EP_EndpointResetFromClient,
    Isoch_EP_ProcessExpectedEventTRBs,
    TR_EmptyFunction,                               // EP_FSEReceived
    Isoch_EP_WasDoorbellRungSinceMappingStart,
    NULL,                                           // WDF_EvtIoDefault
    Isoch_WdfEvtIoCanceledOnQueue,
    Isoch_WdfEvtIoQueueReadyNotification,
    Isoch_WdfEvtDpcForTransferCompletion

};

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Isoch_GetConfiguration(
    __in
        HANDLE      ControllerHandle,
    __out
        PTR_CONFIG  Config
    )
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(ControllerHandle);

    Config->QueueDispatchType = WdfIoQueueDispatchManual;
    Config->ContextSize       = sizeof(ISOCH_DATA);
    Config->DoubleBufferSize  = 0;
    Config->SegmentSize       = SMALL_SEGMENT_SIZE;
    Config->FunctionTable     = &IsochFunctionTable;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Isoch_Initialize(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA            isochData;
#ifndef DISABLE_RING_EMPTY_TIMER
    WDF_OBJECT_ATTRIBUTES  wdfAttributes;
    WDF_TIMER_CONFIG       wdfTimerConfig;
    NTSTATUS               status;
#endif

    PAGED_CODE();

    isochData = (PISOCH_DATA)Context;

    TRY {

        InitializeListHead(&isochData->PendingTransferList);
        InitializeListHead(&isochData->WaitingForCancelCallbackTransferList);
        InitializeListHead(&isochData->CompletionTransferList);

        isochData->PacketsPerFrame = Isoch_CalculatePacketsPerFrame(isochData);

#ifndef DISABLE_RING_EMPTY_TIMER
        WDF_TIMER_CONFIG_INIT(&wdfTimerConfig, Isoch_WdfEvtRingEmptyTimer);

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = isochData->WdfQueue;

        status = WdfTimerCreate(&wdfTimerConfig, &wdfAttributes, &isochData->WdfRingEmptyTimer);

        if (!NT_SUCCESS(status)) {

            LogError(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                     "WdfTimerCreate Failed %!STATUS!", status);
            LEAVE;
        }
#endif
        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

#ifndef DISABLE_RING_EMPTY_TIMER
VOID
Isoch_WdfEvtRingEmptyTimer(
    _In_
        WDFTIMER Timer
    )
{
    WDFQUEUE  wdfQueue;
    PTR_DATA  trData;

    TRY {

        wdfQueue = WdfTimerGetParentObject(Timer);
        trData = GetTRData(wdfQueue);

        LogInfo(Endpoint_GetRecorderLog(trData->EndpointHandle), TransferRing,
                "Isoch_WdfEvtRingEmptyTimer is executed");

        if (trData->MappingState != MS_WaitingForTransferRingEmptyEvent) {
            LEAVE;
        }

#if DBG
        NT_ASSERTMSG("Isoch: stuck while in MS_WaitingForTransferRingEmptyEvent state", 0);
#else
        if (KdRefreshDebuggerNotPresent() == FALSE) {

            DbgBreakPoint();
        }
#endif

    } FINALLY {

    }
}
#endif


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_Cleanup(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    TR_Lock(isochData);

    if (Isoch_DoesDriverOwnRequests(isochData)) {
        NT_ASSERTMSG("Driver owned requests not completed", 0);
    }

    TR_Unlock(isochData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_CommonBufferCallback(
    __in
        HANDLE      Context,
    __in
        NTSTATUS    Status
    )
{
    PISOCH_DATA isochData;

    UNREFERENCED_PARAMETER(Status);

    isochData = (PISOCH_DATA)Context;

    //
    // Unconditionally go back to mapping transfers. We could honor the StopMapping request
    // here, but that would incur an Interlocked operation in the common case of not having
    // received a StopMapping request.
    //
    isochData->CommonBufferAcquireAttemptCount = 0;

    NT_ASSERT((isochData->MappingState == MS_Mapping) ||
              (isochData->MappingState == MS_Stopping));

    LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
            "CommonBufferCallback : Current state is %!MappingState!",
            isochData->MappingState);

    Isoch_MapTransfers(isochData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Isoch_EP_Enable(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
            "%u.%u.0: Setting IsochPure on TransferRing enable",
            UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
            Endpoint_GetContextIndex(isochData->EndpointHandle));

    isochData->IsochPure = TRUE;

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_Disable(
    __in
        HANDLE    Context,
    __in
        BOOLEAN   FreeResources
    )
{
    PISOCH_DATA isochData;
    MAPPING_STATE mappingState;

    UNREFERENCED_PARAMETER(FreeResources);

    isochData = (PISOCH_DATA)Context;

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
    for ( ; ; ) {

        mappingState = *((volatile MAPPING_STATE*)&isochData->MappingState);

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
        mappingState = InterlockedExchange((volatile LONG*)&isochData->MappingState, MS_Stopped);
        NT_ASSERT(mappingState == MS_Paused);
    }

    NT_ASSERT(IsListEmpty(&isochData->PendingTransferList));
    NT_ASSERT(IsListEmpty(&isochData->WaitingForCancelCallbackTransferList));
    NT_ASSERT(IsListEmpty(&isochData->CompletionTransferList));

    NT_ASSERT(Isoch_EP_IsMappingStoppedOrPaused(isochData) != FALSE);
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_EP_AreTransfersPending(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;
    BOOLEAN     transfersPending;

    isochData = (PISOCH_DATA)Context;

    TR_Lock(isochData);

    if (IsListEmpty(&isochData->PendingTransferList)) {
        transfersPending = FALSE;
    } else {
        transfersPending = TRUE;
    }

    TR_Unlock(isochData);

    return transfersPending;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_OkToReclaimTransfersOnCancel(
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
    PISOCH_DATA isochData;
    BOOLEAN     queueDpc;

    TRY {

        isochData = (PISOCH_DATA)Context;

        TR_Lock(isochData);

        if (isochData->Flags.AtleastOneTransferCanceled) {
            APPEND_LIST(&isochData->CompletionTransferList, &isochData->PendingTransferList);
            queueDpc = TRUE;
        } else {
            isochData->Flags.OkToReclaimTransfersOnCancel = 1;
            queueDpc = FALSE;
        }

        TR_Unlock(isochData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(isochData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_ReclaimTransfers(
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
    PISOCH_DATA isochData;
    BOOLEAN     queueDpc;

    TRY {

        queueDpc  = FALSE;
        isochData = (PISOCH_DATA)Context;

        TR_Lock(isochData);

        isochData->Flags.ReclaimTransfersAndAcknowledge = 1;

        if (IsListEmpty(&isochData->PendingTransferList) == FALSE) {

            APPEND_LIST(&isochData->CompletionTransferList, &isochData->PendingTransferList);
        }

        TR_Unlock(isochData);

    } FINALLY {

        TR_QueueDpcForTransferCompletion(isochData);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_EndpointResetFromClient(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    An Endpoint Reset request from client causes a new Isoch Stream to get
    started with the next transfer. That is, the next Isoch Transfer with
    the ASAP bit set will start on a fresh frame number.

--*/
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
            "%u.%u.0: Setting IsochPure on client endpoint reset request",
            UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
            Endpoint_GetContextIndex(isochData->EndpointHandle));

    isochData->IsochPure = TRUE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_ProcessExpectedEventTRBs(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    Endpoint_TR_ExpectedEventTRBsProcessed(isochData->EndpointHandle);
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_EP_WasDoorbellRungSinceMappingStart(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    return (isochData->Flags.DoorbellRungSinceMappingStart == 1);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_Initialize(
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __out
        PISOCH_STAGE_DATA       StageData
    )
{
    PISOCH_DATA isochData;

    isochData = TransferData->IsochData;

    NT_ASSERTMSG("Isoch Transfer Stage address should be atleast 8-byte aligned",
                 (((ULONG_PTR)StageData & 0x07) == 0));

    InitializeListHead(&StageData->FreeSegmentList);
    InitializeListHead(&StageData->UsedSegmentList);

    //
    // N.B: FirstPacket and LastPacket are initially set to INVALID values to protect
    // the stage from being prematurely seen as valid by the completion code which
    // tries to scan through stages from newest to oldest. ULONG_MAX - 1 is used
    // instead of ULONG_MAX to prevent wraparound in comparisons like
    // (FirstPacket + 1) or (LastPacket + 1).
    //
    StageData->TransferData                    = TransferData;
    StageData->FirstPacket                     = ULONG_MAX - 1;
    StageData->LastPacket                      = ULONG_MAX - 1;
    StageData->OnePastLastCompletedPacket      = 1;
    StageData->TransferSize                    = 0;
    StageData->FreeMdl                         = FALSE;
    StageData->Mdl                             = NULL;
    StageData->ScatterGatherList               = NULL;
    StageData->TrbRange.FirstSegment           = isochData->CurrentSegmentBufferData;
    StageData->TrbRange.FirstSegmentStartIndex = isochData->CurrentSegmentIndex;
    StageData->TrbRange.LastSegment            = isochData->CurrentSegmentBufferData;
    StageData->TrbRange.LastSegmentEndIndex    = isochData->CurrentSegmentIndex;
    StageData->TrbIterator.Segment             = isochData->CurrentSegmentBufferData;
    StageData->TrbIterator.SegmentIndex        = isochData->CurrentSegmentIndex;
}

_Requires_lock_not_held_(IsochData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_FreeScatterGatherList(
    __in
        PISOCH_DATA         IsochData,
    __inout
        PISOCH_STAGE_DATA   StageData
    )
/*++

Routine Description:

    N.B: Requires that the lock be not held because PutScatterGatherList may cause the
         HAL to invoke a different DMA callback of this same Isoch object, which then
         tries to acquire the lock and deadlocks.

--*/
{
    PDMA_ADAPTER    dmaAdapter;
    KIRQL           savedIrql;
    PTRANSFER_URB   transferUrb;

    transferUrb = StageData->TransferData->TransferUrb;

    if (StageData->ScatterGatherList) {

        dmaAdapter =
            CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(IsochData->ControllerHandle));

        KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);

        dmaAdapter->DmaOperations->PutScatterGatherList(
            dmaAdapter,
            StageData->ScatterGatherList,
            ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));

        KeLowerIrql(savedIrql);

        StageData->ScatterGatherList = NULL;
    }
}

_Requires_lock_held_(IsochData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_Free(
    __in
        PISOCH_DATA         IsochData,
    __inout
        PISOCH_STAGE_DATA   StageData
    )
{
    NT_ASSERT(StageData->ScatterGatherList == NULL);

    if (StageData->FreeMdl) {

        IoFreeMdl(StageData->Mdl);

        StageData->Mdl     = NULL;
        StageData->FreeMdl = FALSE;
    }

    TR_ReleaseSegments(IsochData, &StageData->FreeSegmentList, TRUE);
    TR_ReleaseSegments(IsochData, &StageData->UsedSegmentList, FALSE);
}

__drv_maxIRQL(DISPATCH_LEVEL)
PISOCH_STAGE_DATA
Isoch_Stage_Acquire(
    __inout
        PISOCH_TRANSFER_DATA TransferData
    )
{
    PISOCH_STAGE_DATA stageData;

    stageData = StageQueue_Acquire(&TransferData->StageQueue);

    if (stageData != NULL) {
        Isoch_Stage_Initialize(TransferData, stageData);
    }

    return stageData;
}

_Requires_lock_held_(IsochData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_Release(
    __in
        PISOCH_DATA         IsochData,
    __inout
        PISOCH_STAGE_DATA   StageData
    )
{
    PISOCH_TRANSFER_DATA transferData;

    transferData = StageData->TransferData;

    Isoch_Stage_Free(IsochData, StageData);

    StageQueue_Release(&transferData->StageQueue, StageData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Isoch_TransferData_Initialize(
    __in
        PISOCH_DATA             IsochData,
    __in
        WDFREQUEST              WdfRequest,
    __out
        PISOCH_TRANSFER_DATA*   IsochTransferData
    )
{
    NTSTATUS                status;
    PISOCH_TRANSFER_DATA    transferData;
    PTRANSFER_URB           transferUrb;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    TRY {

        status = STATUS_SUCCESS;

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        transferUrb  = (PTRANSFER_URB)wdfRequestParams.Parameters.Others.Arg1;
        transferData = &(GetRequestData(WdfRequest)->IsochTransferData);

        NT_ASSERTMSG("Isoch URB must end on a Frame boundary",
                     ((transferUrb->Isoch.NumberOfPackets % IsochData->PacketsPerFrame) == 0));

        //
        // WDFREQUEST context needs to be zero'd out manually because WDFREQUEST
        // objects can get reused by UCX.
        //
        RtlZeroMemory(transferData, sizeof(ISOCH_TRANSFER_DATA));

        InitializeListHead(&transferData->ListEntry);

        transferData->Initialized      = TRUE;
        transferData->WdfRequest       = WdfRequest;
        transferData->TransferUrb      = transferUrb;
        transferData->IsochData        = IsochData;
        transferData->WdfRequestStatus = STATUS_PENDING;
        transferData->CancelState      = TCS_NotCancelable;
        transferData->Mdl              = NULL;
        transferData->BytesTotal       = transferUrb->TransferBufferLength;
        transferData->BytesTransferred = 0;
        transferData->BytesMapped      = 0;
        transferData->BytesSkipped     = 0;
        transferData->PacketsTotal     = transferUrb->Isoch.NumberOfPackets;
        transferData->PacketsCompleted = 0;
        transferData->PacketsMapped    = 0;
        transferData->PacketsSkipped   = 0;
        transferData->StagesCompleted  = 0;
        transferData->StagesMapped     = 0;

        StageQueue_Initialize(&transferData->StageQueue,
                              sizeof(ISOCH_STAGE_DATA),
                              ISOCH_MAX_PENDING_STAGES_PER_TRANSFER);

        if (transferUrb->TransferBufferMDL) {

            transferData->Mdl = transferUrb->TransferBufferMDL;

        } else {

            transferData->Mdl = IoAllocateMdl(transferUrb->TransferBuffer,
                                              transferData->BytesTotal,
                                              FALSE,
                                              FALSE,
                                              NULL);

            if (transferData->Mdl == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                LogError(Controller_GetRecorderLog(IsochData->ControllerHandle), TransferRing,
                         "%u.%u.0: WdfRequest 0x%p, IoAllocateMdl failed",
                         UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(IsochData->EndpointHandle), WdfRequest);
                LEAVE;
            }

            MmBuildMdlForNonPagedPool(transferData->Mdl);
        }

    } FINALLY {

        *IsochTransferData = transferData;
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_TransferData_Free(
    __in
        PISOCH_TRANSFER_DATA TransferData
    )
{
    NT_ASSERT(TransferData->WdfRequestStatus != STATUS_PENDING);

    NT_ASSERT(StageQueue_GetUsedStageCount(&TransferData->StageQueue) == 0);

    if ((TransferData->Mdl != NULL) &&
        (TransferData->Mdl != TransferData->TransferUrb->TransferBufferMDL)) {

        IoFreeMdl(TransferData->Mdl);
        TransferData->Mdl = NULL;
    }

    TransferData->Initialized = FALSE;
}

VOID
Isoch_Transfer_PrepareForCompletion(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS             UrbPacketStatus,
    __in
        USBD_STATUS             UrbHeaderStatus
    )
/*++

Routine Description:

    This routine prepares the transfer for completion and is responsible for:

    - Updating Status, ErrorCount and TransferBufferLength for the URB.
    - Setting status for packets that don't have it set.
    - Updating performance counters.

--*/
{
    ULONG                       i;
    ULONG                       packetsSuccessful;
    PTRANSFER_URB               transferUrb;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;

    packetsSuccessful = 0;
    transferUrb      = TransferData->TransferUrb;
    isochPacketArray = transferUrb->Isoch.IsochPacket;

    transferUrb->TransferBufferLength = TransferData->BytesTransferred;

    for (i = 0; i < TransferData->PacketsTotal; i += 1) {

        if (UrbPacketStatus != USBD_STATUS_NOT_SET) {
            isochPacketArray[i].Status = UrbPacketStatus;
        } else {
            if (isochPacketArray[i].Status == USBD_STATUS_NOT_SET) {
                isochPacketArray[i].Status = USBD_STATUS_ISO_NOT_ACCESSED_BY_HW;
            }
        }

        if (isochPacketArray[i].Status != USBD_STATUS_SUCCESS) {
            transferUrb->Isoch.ErrorCount += 1;
            IsochData->Counter.IsochTDFailCount += 1;
        } else {
            packetsSuccessful += 1;
        }

        IsochData->Counter.IsochTDCount += 1;
    }

    //
    // If this is an isoch transfer and all of the isoch packets completed with an error,
    // then set the global status in the urb. This mimics the USB 2.0 stack behavior. If
    // some of the packets have completed with no error, then complete the URB with success.
    //
    if (UrbHeaderStatus != USBD_STATUS_NOT_SET) {
        transferUrb->Header.Status = UrbHeaderStatus;
    } else {
        if (transferUrb->Isoch.ErrorCount == TransferData->PacketsTotal) {
            transferUrb->Header.Status = USBD_STATUS_ISOCH_REQUEST_FAILED;
        } else {
            transferUrb->Header.Status = USBD_STATUS_SUCCESS;
        }
    }

    TransferData->WdfRequestStatus =
        TR_GetNtStatusFromUsbdStatus(transferUrb->Header.Status);

    //
    // Update performance counters.
    //
    IsochData->Counter.TransferCount    += 1;
    IsochData->Counter.BytesTransferred += TransferData->BytesTransferred;

    if (!NT_SUCCESS(TransferData->WdfRequestStatus)) {
        IsochData->Counter.FailedTransferCount += 1;
    }

#ifdef DBG
    /*
    if (transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) {

        ULONG totalPacketLength;

        totalPacketLength = 0;

        for (i = 0; i < TransferData->PacketsTotal; i += 1) {
            totalPacketLength += isochPacketArray[i].Length;
        }

        NT_ASSERT(totalPacketLength == transferUrb->TransferBufferLength);
    }
    */





















    /*
    // This is for checking if the controller waited for a wraparound of the MFINDEX
    // register before executing the TD. But it is not perfect, and can get hit if
    // system is broken into the debugger and then let go. So keeping it off unless
    // testing manually.
    //
    if ((UrbHeaderStatus == USBD_STATUS_NOT_SET) &&
        NT_SUCCESS(TransferData->WdfRequestStatus) &&
        (TransferData->CancelState != TCS_Canceled)) {

        ULONG xhciFrameNumber;

        xhciFrameNumber =
            Controller_GetFrameNumber(IsochData->ControllerHandle, MICROFRAME_INCREMENT_DEFAULT);

        NT_ASSERTMSG("Controller executed Isoch TD after frame number wraparound",
                     (((LONG)(xhciFrameNumber - transferUrb->Isoch.StartFrame) < 2048)));
    }
    */
#endif

    LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
            "%u.%u.0: WdfRequest 0x%p  %!STATUS! StartFrame 0x%X Packets (Successful, Total) = (%u, %u), BytesTransferred %u",
            UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
            Endpoint_GetContextIndex(IsochData->EndpointHandle), TransferData->WdfRequest,
            TransferData->WdfRequestStatus, transferUrb->Isoch.StartFrame,
            packetsSuccessful, TransferData->PacketsTotal, TransferData->BytesTransferred);

    Isoch_TransferData_Free(TransferData);

    return;
}

_Requires_lock_not_held_(IsochData->Lock)
VOID
Isoch_Transfer_Complete(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS             UrbPacketStatus,
    __in
        USBD_STATUS             UrbHeaderStatus,
    __in
        BOOLEAN                 CompleteAsync
    )
{
    NT_ASSERT((TransferData->CancelState == TCS_NotCancelable) ||
              (TransferData->CancelState == TCS_Canceled));

    Isoch_Transfer_PrepareForCompletion(IsochData,
                                        TransferData,
                                        UrbPacketStatus,
                                        UrbHeaderStatus);

    if (CompleteAsync) {

        TR_Lock(IsochData);
        InsertTailList(&IsochData->CompletionTransferList, &TransferData->ListEntry);
        TR_Unlock(IsochData);

        TR_QueueDpcForTransferCompletion(IsochData);

    } else {

        WdfRequestComplete(TransferData->WdfRequest, TransferData->WdfRequestStatus);
    }
}

_Requires_lock_held_(IsochData->Lock)
VOID
Isoch_Transfer_CompleteCancelable(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS             UrbPacketStatus,
    __in
        USBD_STATUS             UrbHeaderStatus,
    __in
        BOOLEAN                 CompleteIfCanceled,
    __in
        BOOLEAN                 CompleteAsync
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

                LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequest 0x%p WdfRequestUnmarkCancelable returned %!STATUS!",
                        UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(IsochData->EndpointHandle),
                        TransferData->WdfRequest, status);

                TransferData->CancelState = TCS_WaitingForCancelCallback;
                InsertTailList(&IsochData->WaitingForCancelCallbackTransferList, &TransferData->ListEntry);
                LEAVE;

            } else {

                TransferData->CancelState = TCS_NotCancelable;
            }
            break;

        case TCS_Canceled:

            if (CompleteIfCanceled) {

                if (UrbHeaderStatus == USBD_STATUS_NOT_SET) {
                    UrbHeaderStatus = USBD_STATUS_CANCELED;
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

        TR_Unlock(IsochData);

        Isoch_Transfer_PrepareForCompletion(IsochData,
                                            TransferData,
                                            UrbPacketStatus,
                                            UrbHeaderStatus);

        if (CompleteAsync) {

            TR_Lock(IsochData);
            InsertTailList(&IsochData->CompletionTransferList, &TransferData->ListEntry);
            TR_Unlock(IsochData);

            TR_QueueDpcForTransferCompletion(IsochData);

        } else {

            WdfRequestComplete(TransferData->WdfRequest, TransferData->WdfRequestStatus);
        }

        TR_Lock(IsochData);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
PISOCH_STAGE_DATA
Isoch_Stage_Find(
    __in
        PISOCH_DATA             IsochData,
    __in
        PTRANSFER_EVENT_TRB     Trb,
    __in
        PISOCH_TRACKING_DATA    IsochTrackingData
    )
/*++

Routine Description:

    Locates the transfer and then the stage that the Transfer Event TRB is meant for
    by using the FrameNumber and IsochPacketIndex in the IsochTrackingData.

    Side effects: Updates stageData->OnePastLastCompletedPacket and
    transferData->PacketsCompleted.

--*/
{
    ULONG                   currentPacket;
    PISOCH_STAGE_DATA       stageData;
    PISOCH_TRANSFER_DATA    transferData;
    PTRANSFER_URB           transferUrb;
    BOOLEAN                 urbFound;
    BOOLEAN                 stageFound;

    TRY {

        urbFound      = FALSE;
        stageFound    = FALSE;
        stageData     = NULL;
        transferUrb   = NULL;
        currentPacket = (ULONG)IsochTrackingData->IsochPacketIndex;

        FOR_ALL_IN_LIST(ISOCH_TRANSFER_DATA,
                        &IsochData->PendingTransferList,
                        ListEntry,
                        transferData) {

            LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                       "%u.%u.0: WdfRequest 0x%p",
                       Trb->SlotId, Trb->EndpointId, transferData->WdfRequest);

            transferUrb = transferData->TransferUrb;

            if (transferUrb->Isoch.StartFrame == IsochTrackingData->FrameNumber) {
                urbFound = TRUE;
                break;
            }
        }

        //
        // A pending Transfer corresponding to the TRB may not be found. See comment in
        // Isoch_ProcessTransferEventWithED1().
        //
        if (urbFound == FALSE) {

            LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: Isoch URB request not found TrackingData: FrameNumber 0x%x Packet %u",
                    Trb->SlotId, Trb->EndpointId, (ULONG)IsochTrackingData->FrameNumber,
                    currentPacket);
            LEAVE;
        }

        NT_ASSERT(currentPacket < transferData->PacketsTotal);

        LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                   "%u.%u.0: Urb.StartFrame 0x%x Tracking: FrameNumber 0x%x IsochPacketIndex %u",
                   Trb->SlotId, Trb->EndpointId, transferData->TransferUrb->Isoch.StartFrame,
                   (ULONG)IsochTrackingData->FrameNumber, currentPacket);

        StageQueue_ForwardScanStart(&transferData->StageQueue);

        stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

        while (stageData != NULL) {

            if (currentPacket <= stageData->LastPacket) {

                if ((currentPacket == 0) || (currentPacket > (stageData->OnePastLastCompletedPacket - 1))) {

                    LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                               "%u.%u.0: Updating OnePastLastCompletedPacket: old value: %u, new value: %u",
                               Trb->SlotId, Trb->EndpointId, stageData->OnePastLastCompletedPacket,
                               (currentPacket + 1));

                    transferData->PacketsCompleted        = currentPacket + 1;
                    stageData->OnePastLastCompletedPacket = currentPacket + 1;

                } else {

                    //
                    // This can happen if an ED=0 event was received and we are now processing an
                    // ED=1 event.
                    //
                    NT_ASSERT((ULONG)currentPacket == (stageData->OnePastLastCompletedPacket - 1));
                }

                stageFound = TRUE;
                break;
            }

            stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
        }

        if (stageFound == FALSE) {

            NT_ASSERTMSG("Staging Data for Isoch URB request not found!", 0);

            LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                     "%u.%u.0: Matching stage for WdfRequest 0x%p not found! Tracking: FrameNumber 0x%x IsochPacketIndex %u",
                     Trb->SlotId, Trb->EndpointId, transferData->WdfRequest,
                     (ULONG)IsochTrackingData->FrameNumber, (ULONG)currentPacket);

            stageData = NULL;
        }

    } FINALLY {

    }

    return stageData;
}

_Requires_lock_not_held_(IsochData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Isoch_CompleteStaleTransfers(
    __in
        PISOCH_DATA             IsochData,
    __in_opt
        PISOCH_TRANSFER_DATA    TransferData
    )
/*++

Routine Description:

    This function is called to complete stale isoch requests. These are requests that
    are located in the pending transfer list prior to the transfer data passed into
    this function.

--*/
{
    LIST_ENTRY              completionList;
    PLIST_ENTRY             current;
    PISOCH_TRANSFER_DATA    currentTransferData;
    PLIST_ENTRY             head;
    PLIST_ENTRY             next;
    PISOCH_STAGE_DATA       stageData;
    ULONG                   stagesCompleted;

    InitializeListHead(&completionList);

    TR_Lock(IsochData);

    head    = &IsochData->PendingTransferList;
    current = head->Flink;

    while ((current != head) && (current != &TransferData->ListEntry)) {

        currentTransferData = (PISOCH_TRANSFER_DATA)current;
        next = current->Flink;

        LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                "%u.%u.0: Stale WdfRequest 0x%p StartFrame 0x%x",
                UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                Endpoint_GetContextIndex(IsochData->EndpointHandle),
                currentTransferData->WdfRequest, currentTransferData->TransferUrb->Isoch.StartFrame);

        RemoveEntryList(&currentTransferData->ListEntry);
        InsertTailList(&completionList, &currentTransferData->ListEntry);

        current = next;
    }

    TR_Unlock(IsochData);

    while (IsListEmpty(&completionList) == FALSE) {

        stagesCompleted = 0;

        currentTransferData = (PISOCH_TRANSFER_DATA)RemoveHeadList(&completionList);
        InitializeListHead(&currentTransferData->ListEntry);

        StageQueue_ForwardScanStart(&currentTransferData->StageQueue);

        stageData = StageQueue_ForwardScanGetNextStage(&currentTransferData->StageQueue);

        while (stageData != NULL) {

            Isoch_Stage_FreeScatterGatherList(IsochData, stageData);

            TR_Lock(IsochData);
            Isoch_Stage_Release(IsochData, stageData);
            TR_Unlock(IsochData);

            stagesCompleted += 1;

            stageData = StageQueue_ForwardScanGetNextStage(&currentTransferData->StageQueue);
        }

        TR_Lock(IsochData);

        Isoch_Transfer_CompleteCancelable(IsochData,
                                          currentTransferData,
                                          USBD_STATUS_NOT_SET,
                                          USBD_STATUS_NOT_SET,
                                          TRUE,
                                          FALSE);

        IsochData->PendingStageCount -= stagesCompleted;
        NT_ASSERT(IsochData->PendingStageCount <= ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING);

        TR_Unlock(IsochData);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_CompleteTD(
    __in
        PISOCH_STAGE_DATA   StageData,
    __in
        ULONG               CompletionCode,
    __in
        ULONG               BytesTransferred,
    __in
        BOOLEAN             CompleteStageIfAllPacketsComplete,
    __out
        PBOOLEAN            StageCompleted,
    __out
        PULONG              PendingStageCount
    )
/*++

Routine Description:

    This function handles completion of one Isoch packet. As a side effect, it also
    completes stale isoch packets of this stage and completes this stage if this is
    the last packet for the stage. It also completes the transfer if this is the last
    stage for the transfer.

--*/
{
    ULONG                       currentPacket;
    LONG                        i;
    PISOCH_DATA                 isochData;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;
    PISOCH_TRANSFER_DATA        transferData;
    PTRANSFER_URB               transferUrb;

    TRY {

        *PendingStageCount = 0;
        *StageCompleted    = FALSE;
        transferData       = StageData->TransferData;
        transferUrb        = transferData->TransferUrb;
        isochData          = transferData->IsochData;
        isochPacketArray   = transferUrb->Isoch.IsochPacket;
        currentPacket      = StageData->OnePastLastCompletedPacket - 1;

        //
        // For this stage, complete packets that were skipped due to no MSE event.
        // Compat: USBD_STATUS_ISO_TD_ERROR is the status set for missed microframes.
        //
        if ((currentPacket > 0) &&
            (isochPacketArray[currentPacket - 1].Status == USBD_STATUS_NOT_SET)) {

            for (i = currentPacket - 1; i >= 0; i -= 1) {

                if (isochPacketArray[i].Status != USBD_STATUS_NOT_SET) {
                    break;
                }
                LogError(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                    "%u.%u.0: WdfRequest 0x%p Stage 0x%p : Setting status USBD_STATUS_ISO_TD_ERROR due to missed microframes skipped due to no MSE event",
                    UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(isochData->EndpointHandle), transferData->WdfRequest,
                    StageData);

                isochPacketArray[i].Status = USBD_STATUS_ISO_TD_ERROR;
            }
        }

        //
        // The status, length etc. for this packet could have been updated when an ED=0
        // event was received. Don't do it again when a subsequent ED=1 event is received.
        //
        if (isochPacketArray[currentPacket].Status == USBD_STATUS_NOT_SET) {

            if ((Endpoint_StoppedCompletionCode(CompletionCode) == FALSE) ||
                (BytesTransferred != 0)) {

                isochPacketArray[currentPacket].Status =
                    TR_GetUsbdStatusFromTrbCompletionCode(CompletionCode, USBD_STATUS_NOT_SET);

                transferData->BytesTransferred += BytesTransferred;

                //
                // Compat: Only set the length for Isoch In.
                //
                if (Endpoint_GetType(isochData->EndpointHandle) == EndpointType_IsochIn) {
                    isochPacketArray[currentPacket].Length = BytesTransferred;
                }
            }

        } else {

            /*
            //
            // The Trb CompletionCode and Length in the ED=1 event should match with that
            // in the prior ED=0 event, but that has not been the case due to controller
            // bugs. That is why we rely on updating the Status and Length for the packet
            // when we receive the ED=0 event, and only use the ED=1 event for considering
            // the TD complete. Ideally, we should be able to rely on the Trb CompletionCode
            // and Length in the ED=1 event, and drop the Ed=0 event.
            //
            if (!Endpoint_StoppedCompletionCode(CompletionCode)) {

                NT_ASSERT(isochPacketArray[currentPacket].Status ==
                          TR_GetUsbdStatusFromTrbCompletionCode(CompletionCode, USBD_STATUS_NOT_SET);

                NT_ASSERT(isochPacketArray[currentPacket].Length == BytesTransferred);
            }
            */
        }

        //
        // Note: LastPacket indicates the last Td that corresponds to this Stage. However
        // it is possible that this routine gets called before the actual value of
        // LastPacket is determined by the Isoch_Stage_MapIntoRing function the Td is already
        // complete. Since LastPacket is initialized to PacketsTotal - 1, thus it is possible that
        // the stage gets completed even though following check fails. If that happens this stage
        // would be completed by Isoch_Transfer_CompleteStaleStages when a future Td completes.
        //
        if (currentPacket != StageData->LastPacket) {
            LEAVE;
        }

        if (CompleteStageIfAllPacketsComplete) {

            LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                    "%u.%u.0: WdfRequest 0x%p Stage 0x%p : Finished all %u packets",
                    UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(isochData->EndpointHandle), transferData->WdfRequest,
                    StageData, (StageData->LastPacket - StageData->FirstPacket + 1));

            Isoch_Stage_FreeScatterGatherList(isochData, StageData);

            TR_Lock(isochData);

            Isoch_Stage_Release(isochData, StageData);

            transferData->StagesCompleted += 1;

            if (transferData->PacketsCompleted == transferData->PacketsTotal) {

                Isoch_Transfer_CompleteCancelable(isochData,
                                                  transferData,
                                                  USBD_STATUS_NOT_SET,
                                                  USBD_STATUS_NOT_SET,
                                                  FALSE,
                                                  FALSE);
            }

            isochData->PendingStageCount -= 1;
            NT_ASSERT(isochData->PendingStageCount <= ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING);

            *PendingStageCount = isochData->PendingStageCount;
            TR_Unlock(isochData);

            *StageCompleted = TRUE;

        } else {

            LogVerbose(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                       "%u.%u.0: WdfRequest 0x%p Stage 0x%p : Finished all %u packets. "
                       "Waiting for EventData Transfer Trb to complete Urb",
                       UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(isochData->EndpointHandle), transferData->WdfRequest,
                       StageData, (StageData->LastPacket - StageData->FirstPacket + 1));
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Isoch_Transfer_CompleteStaleStages(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        PISOCH_STAGE_DATA       CurrentStageData
    )
/*++

Routine Description:

    Due to the reasons mentioned below an Isoch Stage may not be completed when the last TD for that
    stage completes:

    - Last TD for the stage encountered a Missed Service Interval and XHCI did not generage a
      transfer event data trb
    - At the time the last TD was completed and processed, the
      TransferRing_Isoch_InsertStageIntoRing was still running and had not determined the
      value for MaxIsochPacketIndex

    Thus on completion of a future td (or some other conditions <TO BE FILLED LATER>), this routine
    would complete such a stale isochronous stage

--*/
{
    ULONG                       pendingStageCount;
    BOOLEAN                     stageCompleted;
    PISOCH_STAGE_DATA           stageData;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;

    UNREFERENCED_PARAMETER(IsochData);

    isochPacketArray = TransferData->TransferUrb->Isoch.IsochPacket;

    UNREFERENCED_PARAMETER(isochPacketArray);

    StageQueue_ForwardScanStart(&TransferData->StageQueue);

    stageData = StageQueue_ForwardScanGetNextStage(&TransferData->StageQueue);

    while (stageData != NULL) {

        if (stageData == CurrentStageData) {
            NT_ASSERT((stageData->LastPacket + 1) >= TransferData->PacketsCompleted);
            break;
        }

        NT_ASSERTMSG("Found older completed stage on the StageQueue",
                     (isochPacketArray[stageData->LastPacket].Status == USBD_STATUS_NOT_SET));

        //
        // Simulate as if we recieved an MSE on the last IsochPacketIndex for this stage.
        //
        stageData->OnePastLastCompletedPacket = stageData->LastPacket + 1;

        Isoch_Stage_CompleteTD(stageData,
                               TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR,
                               0,
                               TRUE,
                               &stageCompleted,
                               &pendingStageCount);

        stageData = StageQueue_ForwardScanGetNextStage(&TransferData->StageQueue);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_ProcessTransferEventWithED1(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in
        HANDLE              Context
    )
{
    ULONG                   pendingStageCount;
    ULONG                   bytesTransferred;
    BOOLEAN                 eventHandled;
    PISOCH_DATA             isochData;
    PISOCH_TRACKING_DATA    isochTrackingData;
    MAPPING_STATE           previousState;
    BOOLEAN                 stageCompleted;
    PISOCH_STAGE_DATA       stageData;
    PISOCH_TRANSFER_DATA    transferData;

    TRY {

        pendingStageCount = 0;
        stageCompleted    = FALSE;
        eventHandled      = FALSE;
        isochData         = (PISOCH_DATA)Context;
        isochTrackingData = (PISOCH_TRACKING_DATA)&Trb->Pointer.QuadPart;

        LogVerbose(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                   "%u.%u.0: %!TrbCompletionCode! Length %u EventData %u Pointer 0x%I64x",
                   Trb->SlotId, Trb->EndpointId, (TRB_COMPLETION_CODE)Trb->CompletionCode,
                   Trb->TransferLength, Trb->EventData, Trb->Pointer.QuadPart);

        if (isochTrackingData->EndpointType != USB_ENDPOINT_TYPE_ISOCHRONOUS) {

            LogError(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                     "%u.%u.0: EventData pointer not valid: 0x%I64X",
                     Trb->SlotId, Trb->EndpointId, Trb->Pointer.QuadPart);

            Controller_ReportFatalError(isochData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_1,
                                        isochData->UsbDeviceHandle,
                                        isochData->EndpointHandle,
                                        isochData);
            LEAVE;
        }

        TR_Lock(isochData);

        if (isochData->Flags.FSEReceived) {
            TR_Unlock(isochData);
            LEAVE;
        }

        stageData = Isoch_Stage_Find(isochData, Trb, isochTrackingData);

        if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            //
            // The Completion Code for a Transfer Event TRB with ED=1 should be the result
            // of processing the Transfer TRB before the Event Data TRB. If the controller
            // is stopped on an Event Data TRB, it should generate a separate event with
            // ED=0 and Stopped completion code. But some controllers have been observed to
            // deliver a Transfer Event TRB with ED=1 and Stopped completion code (FL1100).
            //
            isochData->Flags.FSEReceived = 1;

            TR_Unlock(isochData);

            transferData = stageData->TransferData;

            bytesTransferred = Trb->TransferLength;

            //
            // Complete the isoch TD, which may also complete the request if this is the last
            // TD in the URB.
            //
            Isoch_Stage_CompleteTD(stageData,
                                   Trb->CompletionCode,
                                   bytesTransferred,
                                   FALSE,
                                   &stageCompleted,
                                   &pendingStageCount);

            Endpoint_TR_FSEReceived(isochData->EndpointHandle);

        } else {

            TR_Unlock(isochData);

            //
            // For Isoch, during cancellation, after Stop Endpoint command completion, after
            // we receive an FSE, we do not wait for outstanding TRBs on Secondary Event Rings
            // to finish processing before completing all outstanding stages. This is because
            // there is no guarantee that all TRBs for all outstanding TDs will be present on
            // the secondary Event Rings, due to the possibility of missed MSE events. After
            // such a cancellation, it is possible that we end up processing those stale TRBs
            // on Secondary Event Rings, in which case, we will not find a corresponding stage.
            // So we have to drop the Event if a corresponding stage is not found, although it
            // could happen due to a controller bug.
            //
            if (stageData == NULL) {
                LEAVE;
            }

            transferData = stageData->TransferData;

            bytesTransferred = Trb->TransferLength;

            //
            // Before completing the TD/request that received the transfer event TRB, complete
            // any prior (stale) requests that did not receive a transfer event TRB.
            //
            Isoch_CompleteStaleTransfers(isochData, transferData);

            //
            // Also complete any stale stages corresponding to the current transfer.
            //
            Isoch_Transfer_CompleteStaleStages(isochData, transferData, stageData);

            //
            // Complete the isoch TD, which may also complete the request if this is the last
            // TD in the URB.
            //
            Isoch_Stage_CompleteTD(stageData,
                                   Trb->CompletionCode,
                                   bytesTransferred,
                                   TRUE,
                                   &stageCompleted,
                                   &pendingStageCount);
        }

    } FINALLY {

        if (stageCompleted) {

            //
            // Attempt to restart mapping in case it had been paused because maximum number
            // of stages had been mapped.
            //
            InterlockedExchange(&isochData->AttemptMapping, 1);
            
            previousState = TR_AttemptStateChange(isochData, MS_Paused, MS_Mapping);

            if (previousState == MS_Paused) {
                Isoch_MapTransfers(isochData);
            } else if (pendingStageCount == 0) {

                //
                // Attempt to restart mapping to account for a case when RingEmptyEvent
                // arrives before all pending stages are completed
                //
                previousState =
                    TR_AttemptStateChange(isochData, MS_WaitingForTransferRingEmptyEvent, MS_Mapping);

#ifndef DISABLE_RING_EMPTY_TIMER
                if (previousState == MS_WaitingForTransferRingEmptyEvent) {
                    WdfTimerStop(isochData->WdfRingEmptyTimer, FALSE);
                }
#endif

                if (previousState == MS_WaitingForTransferRingEmptyEvent) {
                    Isoch_MapTransfers(isochData);
                }
            }
        }
    }

    return eventHandled;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_DoesStageContainDequeuePointer(
    __in
        PISOCH_STAGE_DATA   StageData,
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

FORCEINLINE
BOOLEAN
Isoch_PreventStandaloneLinkTD(
    __in
        PISOCH_DATA IsochData
    )
{
    if ((Controller_GetDeviceFlags(IsochData->ControllerHandle).PreventStandaloneLinkTDForFullSpeedIsoch) &&
        (UsbDevice_GetDeviceSpeed(IsochData->UsbDeviceHandle) == UsbFullSpeed) &&
        (UsbDevice_IsBehindTTHub(IsochData->UsbDeviceHandle))) {

        return TRUE;

    } else {

        return FALSE;
    }
}

__success(return == TRUE)
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_ProcessSegment(
    __in
        PISOCH_DATA         IsochData,
    __in
        PISOCH_STAGE_DATA   StageData,
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in
        PBUFFER_DATA        Segment,
    __in
        ULONG               SegmentStartIndex,
    __out
        PULONG              SegmentMatchIndex,
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

    SegmentMatchIndex - Receives the location in the segment where the scan stopped.

    BytesTransferred - If a matching Transfer TRB was found, receives the number of bytes
        transferred for this Isoch TD.

Side effects:

    Updates transferData->PacketsCompleted and StageData->OnePastLastCompletedPacket.

Return value:

    TRUE if a matching Transfer TRB was found.

--*/
{
    ULONG                   currentPacket;
    BOOLEAN                 trbMatch;
    PTRB                    firstTrbVA;
    PTRB                    lastTrbVA;
    PTRB                    currentTrbVA;
    PTRB                    forwardScanTrbVA;
    PHYSICAL_ADDRESS        currentTrbLA;
    PISOCH_TRACKING_DATA    isochTrackingData;
    PISOCH_TRANSFER_DATA    transferData;

    TRY {

        trbMatch              = FALSE;
        transferData          = StageData->TransferData;

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
            case TRB_TYPE_ISOCH:

                if (trbMatch) {

                    switch (Trb->CompletionCode) {

                    case TRB_COMPLETION_CODE_STOPPED:
                    case TRB_COMPLETION_CODE_DATA_BUFFER_ERROR:
                    case TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR:
                    case TRB_COMPLETION_CODE_ISOCH_BUFFER_OVERRUN:
                    case TRB_COMPLETION_CODE_STALL_ERROR:
                    case TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR:
                    case TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR:
                        if(currentTrbVA->Normal.TransferLength < Trb->TransferLength) {
                            LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                  "%u.%u.0: ERROR_ASSERT: currentTrbVA->Normal.TransferLength (%u) is < Trb->TransferLength (%u), TRB completion code: %!TrbCompletionCode!",
                                   UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(IsochData->EndpointHandle),
                                   currentTrbVA->Normal.TransferLength,
                                   Trb->TransferLength,
                                   Trb->CompletionCode);

                            NT_ASSERT(FALSE);
                        }
                        *BytesTransferred +=
                            (currentTrbVA->Normal.TransferLength - Trb->TransferLength);
                        break;

                    case TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET:
                        *BytesTransferred = Trb->TransferLength;
                        break;

                    case TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID:
                    case TRB_COMPLETION_CODE_NO_PING_RESPONSE_ERROR:
                        break;

                    case TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR:
                        if (Controller_IsMSELengthUsable(IsochData->ControllerHandle)) {
                            if(currentTrbVA->Normal.TransferLength < Trb->TransferLength) {
                                LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                      "%u.%u.0: ERROR_ASSERT: currentTrbVA->Normal.TransferLength (%u) is < Trb->TransferLength (%u), TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR and IsMSELengthUsable",
                                       UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                       Endpoint_GetContextIndex(IsochData->EndpointHandle),
                                       currentTrbVA->Normal.TransferLength,
                                       Trb->TransferLength);

                                NT_ASSERT(FALSE);
                            }
                            *BytesTransferred +=
                                (currentTrbVA->Normal.TransferLength - Trb->TransferLength);
                        }
                        break;

                    default:
                        LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                              "%u.%u.0: ERROR_ASSERT: TRB_TYPE_ISOCH has unexpected TRB completion code: %!TrbCompletionCode!",
                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(IsochData->EndpointHandle),
                               Trb->CompletionCode);
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

            case TRB_TYPE_LINK:
                LEAVE;

            default:
                LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                               "%u.%u.0: ERROR_ASSERT: Unexpected TRB Type encountered while scanning TRB segment: %!TrbType!",
                                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                               Endpoint_GetContextIndex(IsochData->EndpointHandle),
                                               currentTrbVA->Normal.Type);
                NT_ASSERTMSG("Unexpected TRB Type encountered while scanning TRB segment", 0);
                break;
            }

            if (trbMatch) {
                break;
            }

            currentTrbVA += 1;
            currentTrbLA.QuadPart += sizeof(TRB);
        }

        if (trbMatch) {

            //
            // Scan through to the end of this TD looking for the terminating Event Data TRB
            // to find out what packet in the Urb this is.
            //
            forwardScanTrbVA = currentTrbVA;

            while (forwardScanTrbVA < lastTrbVA) {

                switch (forwardScanTrbVA->Normal.Type) {

                case TRB_TYPE_EVENT_DATA:

                    isochTrackingData = (PISOCH_TRACKING_DATA)&forwardScanTrbVA->EventData.Data;
                    currentPacket     = (ULONG)isochTrackingData->IsochPacketIndex;

                    LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                               "%u.%u.0: Updating OnePastLastCompletedPacket: old value: %u, new value: %u",
                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(IsochData->EndpointHandle),
                               StageData->OnePastLastCompletedPacket, (currentPacket + 1));

                    transferData->PacketsCompleted        = currentPacket + 1;
                    StageData->OnePastLastCompletedPacket = currentPacket + 1;
                    LEAVE;

                case TRB_TYPE_NORMAL:
                case TRB_TYPE_ISOCH:
                    break;

                //
                // Isoch TD is not expected to cross segments unless the
                // PreventStandaloneLinkTDForFullSpeedIsoch flag is set.
                //
                case TRB_TYPE_LINK:
                    NT_ASSERTMSG("Isoch TD incorrectly crossing ring segment",
                                 (Isoch_PreventStandaloneLinkTD(IsochData) != FALSE));
                    LEAVE;

                default:
                    LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                       "%u.%u.0: ERROR_ASSERT: Unexpected TRB Type encountered while scanning end of TRB segment: %!TrbType!",
                       UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(IsochData->EndpointHandle),
                       forwardScanTrbVA->Normal.Type);

                    NT_ASSERTMSG("Unexpected TRB Type encountered while scanning end of TRB segment", 0);
                    break;
                }

                forwardScanTrbVA += 1;
            }
        }

    } FINALLY {

        NT_ASSERT(currentTrbVA <= lastTrbVA);

        if (trbMatch) {
            NT_ASSERT(currentTrbVA >= firstTrbVA);
            (*SegmentMatchIndex) = (ULONG)(currentTrbVA - firstTrbVA);
        }
    }

    return trbMatch;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_CalculateBytesTransferred(
    __in
        PISOCH_DATA         IsochData,
    __in
        PISOCH_STAGE_DATA   StageData,
    __in
        PTRANSFER_EVENT_TRB Trb
    )
/*++

Routine Description:

    Locate the Transfer TRB whose logical address is the Pointer value from the
    supplised Transfer Event TRB with ED=0. Calculate the bytes transferred for
    the Isoch TD that contained the matching TRB.

Side effects:

    Updates StageData->TrbIterator to the location where the scanning of the
    transfer ring segment should start, the next time a Transfer Event TRB with
    ED=0 is received. TrbIterator is an optimization taking advantage of the
    fact that the next Transfer Event TRB with ED=0 will not point to a Transfer
    TRB older than the one that this Transfer Event TRB is pointing to. So the
    scan can safely pick up where it left off.

--*/
{
    ULONG           bytesTransferred;
    PBUFFER_DATA    currentSegment;
    ULONG           startIndex;
    ULONG           matchIndex;
    BOOLEAN         trbMatch;
    PTRB_RANGE      trbRange;
    PTRB_ITERATOR   trbIterator;
    PLIST_ENTRY     head;
    PLIST_ENTRY     next;

    TRY {

        bytesTransferred = 0;
        trbRange         = &StageData->TrbRange;
        trbIterator      = &StageData->TrbIterator;
        startIndex       = trbIterator->SegmentIndex;

        //
        // If the current segment is not the last segment, then it is one of the
        // segments on the UsedSegmentList. Start from the current segment and scan
        // through all remaining segments on the UsedSegmentList until a match is found.
        //
        if (trbIterator->Segment != trbRange->LastSegment) {

            head = &StageData->UsedSegmentList;
            next = &trbIterator->Segment->ListEntry;

            while (next != head) {

                currentSegment = CONTAINING_RECORD(next, BUFFER_DATA, ListEntry);

                trbMatch = Isoch_ProcessSegment(IsochData,
                                                StageData,
                                                Trb,
                                                currentSegment,
                                                startIndex,
                                                &matchIndex,
                                                &bytesTransferred);

                if (trbMatch) {
                    LEAVE;
                }

                startIndex = 0;

                next = next->Flink;
            }
        }

        //
        // A match was not found for any segments on the UsedSegmentList. Try the last
        // segment (which can also be the only segment).
        //
        currentSegment = trbRange->LastSegment;

        trbMatch = Isoch_ProcessSegment(IsochData,
                                        StageData,
                                        Trb,
                                        currentSegment,
                                        startIndex,
                                        &matchIndex,
                                        &bytesTransferred);

    } FINALLY {

        NT_ASSERT(trbMatch != FALSE);

        if (trbMatch) {

            trbIterator->Segment      = currentSegment;
            trbIterator->SegmentIndex = matchIndex;
        }
    }

    return bytesTransferred;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_FindTrbMatch(
    __in
        PISOCH_DATA         IsochData,
    __in
        PTRANSFER_EVENT_TRB Trb,
    __out
        PISOCH_STAGE_DATA*  StageData,
    __out
        PULONG              StagesSkipped,
    __out
        PULONG              BytesTransferred
    )
/*++

Routine Description:

    Side effects:

--*/
{
    BOOLEAN                 trbMatch;
    LONGLONG                enqueuePointer;
    LONGLONG                dequeuePointer;
    PISOCH_TRANSFER_DATA    transferData;
    ULONG                   localBytesTransferred;
    PISOCH_STAGE_DATA       localStageData;
    ULONG                   localSkippedStageCount;

    TRY {

        trbMatch               = FALSE;
        localBytesTransferred  = 0;
        localSkippedStageCount = 0;
        localStageData         = NULL;

        dequeuePointer         = Trb->Pointer.QuadPart;
        enqueuePointer         = (IsochData->CurrentSegmentBufferData->LogicalAddress.QuadPart +
                                  (IsochData->CurrentSegmentIndex * sizeof(TRB)));

        if (dequeuePointer == 0) {

            LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: ERROR_ASSERT: TransferEventTrb->Pointer is zero",
                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(IsochData->EndpointHandle));

            NT_ASSERTMSG("TransferEventTrb->Pointer is zero", 0);
            LEAVE;
        }

        if (dequeuePointer == enqueuePointer) {

            LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: TransferRing is empty",
                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(IsochData->EndpointHandle));

            localSkippedStageCount = IsochData->PendingStageCount;
            trbMatch = TRUE;
            LEAVE;
        }

        FOR_ALL_IN_LIST(ISOCH_TRANSFER_DATA,
                        &IsochData->PendingTransferList,
                        ListEntry,
                        transferData) {

            StageQueue_ForwardScanStart(&transferData->StageQueue);

            localStageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

            while (localStageData != NULL) {

                if (Isoch_DoesStageContainDequeuePointer(localStageData, dequeuePointer)) {

                    LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                               "%u.%u.0: WdfRequest 0x%p Matching Stage 0x%p found",
                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(IsochData->EndpointHandle),
                               transferData->WdfRequest, localStageData);

                    localBytesTransferred = Isoch_CalculateBytesTransferred(IsochData, localStageData, Trb);
                    trbMatch = TRUE;
                    LEAVE;
                }

                localSkippedStageCount += 1;
                localStageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
            }
        }

    } FINALLY {

        if (trbMatch) {
            *StageData        = localStageData;
            *StagesSkipped    = localSkippedStageCount;
            *BytesTransferred = localBytesTransferred;
        } else {
            *StageData        = NULL;
            *StagesSkipped    = 0;
            *BytesTransferred = 0;
        }
    }

    return trbMatch;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
Isoch_ProcessTransferRingEmptyEvent(
    __in
        PISOCH_DATA         IsochData,
    __in
        PTRANSFER_EVENT_TRB Trb
    )
/*++

Routine Description:

    An Underrun/Overrun event is considered a TransferRingEmpty event. On receiving
    a TransferRingEmpty event, one of two things are done:

    1. If the ExpectLackOfMseEvents flag is set, then it is possible that the xHC
       missed service interval for one or more Isoch TDs right before it reached
       the end of valid TRBs and posted a TransferRingEmpty event. If that happens,
       one or more transfers may not have completed due to missed MSE events. Detect
       that case and reclaim the stale transfers.
    2. If we were waiting on a TransferRingEmpty event, then restart mapping of
       transfers.

--*/
{
    MAPPING_STATE   previousState;
    BOOLEAN         simulateCancel;
    ULONG           xhciFrameNumber;

    TRY {

        simulateCancel = FALSE;

        xhciFrameNumber =
            Controller_GetFrameNumber(IsochData->ControllerHandle, MICROFRAME_INCREMENT_DEFAULT);

        LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                "%u.%u.0: xhciFrame 0x%x CompletionCode = %!TrbCompletionCode!",
                UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                Endpoint_GetContextIndex(IsochData->EndpointHandle),
                xhciFrameNumber, (TRB_COMPLETION_CODE)Trb->CompletionCode);

        TR_Lock(IsochData);

        IsochData->Counter.UnderrunOverrunCount += 1;
        IsochData->Flags.TransferRingEmptyEventExpected = 0;

        if (IsochData->PendingStageCount == 0) {
            IsochData->Flags.FrameNumberOfLastTDInScheduleValid = 0;
        }

        if (Controller_GetDeviceFlags(IsochData->ControllerHandle).ExpectLackOfMseEvents) {

            if (IsochData->PendingStageCount == 0) {
                LEAVE;
            }

            if (IsochData->Flags.FrameNumberOfLastTDInScheduleValid == 0) {
                LEAVE;
            }

            if (((LONG)(xhciFrameNumber - IsochData->FrameNumberOfLastTDInSchedule) < 0)) {

                //
                // There will be another underrun/overrun event since we would have rung the
                // doorbell after the last TD got programmed.
                //
                LEAVE;
            }

            //
            // There are stale transfers programmed on the TransferRing. Mimic the cancel
            // logic to stop the endpoint and reclaim all the transfers.
            //
            // N.B: It is not OK to just go and complete the transfers right here, because
            // this code is executing in the context of the underrun/overrun event which is
            // on the primary event ring, while there could be code running on a secondary
            // event ring processing completion of Transfer Event TRBs.
            //
            LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: Running cancellation to complete stale transfers after underrun/overrun",
                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(IsochData->EndpointHandle));

            simulateCancel = TRUE;
        }

    } FINALLY {

        TR_Unlock(IsochData);

        if (simulateCancel) {

            Endpoint_TR_TransferCanceled(IsochData->EndpointHandle);

        } else {

            InterlockedExchange(&IsochData->TransferRingEmptyEventReceived, 1);

            previousState =
                TR_AttemptStateChange(IsochData, MS_WaitingForTransferRingEmptyEvent, MS_Mapping);

#ifndef DISABLE_RING_EMPTY_TIMER
            if (previousState == MS_WaitingForTransferRingEmptyEvent) {
                WdfTimerStop(IsochData->WdfRingEmptyTimer, FALSE);
            }
#endif

            if (previousState == MS_WaitingForTransferRingEmptyEvent) {
                Isoch_MapTransfers(IsochData);
            }
        }
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_ProcessTransferEventWithED0(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in
        HANDLE              Context
    )
{
    ULONG                   pendingStageCount;
    ULONG                   bytesTransferred;
    BOOLEAN                 eventHandled;
    PISOCH_DATA             isochData;
    ULONG                   skippedStages;
    BOOLEAN                 stageCompleted;
    PISOCH_STAGE_DATA       stageData;
    PISOCH_TRANSFER_DATA    transferData;
    BOOLEAN                 trbMatchFound;

    TRY {

        eventHandled = FALSE;
        isochData    = (PISOCH_DATA)Context;

        LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                "%u.%u.0: %!TrbCompletionCode! Length %u EventData %u Pointer 0x%I64x",
                Trb->SlotId, Trb->EndpointId, (TRB_COMPLETION_CODE)Trb->CompletionCode,
                Trb->TransferLength, Trb->EventData, Trb->Pointer.QuadPart);

        switch (Trb->CompletionCode) {

        case TRB_COMPLETION_CODE_RING_OVERRUN:
        case TRB_COMPLETION_CODE_RING_UNDERRUN:

            Isoch_ProcessTransferRingEmptyEvent(isochData, Trb);
            LEAVE;

        case TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR:

            isochData->Counter.MissedServiceErrorCount += 1;

















            break;

        case TRB_COMPLETION_CODE_NO_PING_RESPONSE_ERROR:

            LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                    "%u.%u.0: CompletionCode = %!TrbCompletionCode! Pointer = 0x%I64x",
                    UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(isochData->EndpointHandle),
                    Trb->CompletionCode, Trb->Pointer.QuadPart);

            UcxEndpointNoPingResponseError(Endpoint_GetUcxEndpoint(isochData->EndpointHandle));

            //
            // 4.23.5.2.1 : The TRB Pointer field of No Ping Response Error Transfer Event may be 0.
            // If the TRB Pointer = 0, then the TRB Transfer Length field shall be invalid.
            //
            if (Trb->Pointer.QuadPart == 0) {
                LEAVE;
            }

            break;

        case TRB_COMPLETION_CODE_STOPPED:
        case TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID:
        case TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET:
        case TRB_COMPLETION_CODE_DATA_BUFFER_ERROR:
        case TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR:
        case TRB_COMPLETION_CODE_ISOCH_BUFFER_OVERRUN:
        case TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR:
        case TRB_COMPLETION_CODE_STALL_ERROR:
        case TRB_COMPLETION_CODE_EVENT_LOST_ERROR:
        case TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR:
            break;

        default:
            NT_ASSERTMSG("Unexpected TRB completion code", 0);
            break;
        }

        TR_Lock(isochData);

        if (isochData->Flags.FSEReceived) {
            TR_Unlock(isochData);
            LEAVE;
        }

        trbMatchFound = Isoch_FindTrbMatch(isochData,
                                           Trb,
                                           &stageData,
                                           &skippedStages,
                                           &bytesTransferred);

        if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            //
            // trbMatchFound maybe false at this point in the following case:
            //
            // PreventStandaloneLinkTDForFullSpeedIsoch workaround is applied, so every stage ends with
            // a Link TRB. Before the FSE is received, all outstanding stages have completed.
            // FSE is pointing to the Link TRB of the last stage. In this case, we won't find a
            // matching stage because they have all completed, and the pointer in the FSE will
            // also not be equal to the EnqueuePointer.
            //
            // In any case, the FSE should not be ignored.
            //
            isochData->Flags.FSEReceived = 1;

            TR_Unlock(isochData);

            if (stageData) {

                transferData = stageData->TransferData;

                Isoch_Stage_CompleteTD(stageData,
                                       Trb->CompletionCode,
                                       bytesTransferred,
                                       FALSE,
                                       &stageCompleted,
                                       &pendingStageCount);
            }

            Endpoint_TR_FSEReceived(isochData->EndpointHandle);

        } else {

            TR_Unlock(isochData);

            //
            // See comments in Isoch_ProcessTransferEventWithED1 for when a Stage is not found.
            //
            if (stageData == NULL) {
                LEAVE;
            }

            transferData = stageData->TransferData;

            //
            // Before completing the TD/request that received the transfer event TRB, complete
            // any prior (stale) requests that did not receive a transfer event TRB.
            //
            Isoch_CompleteStaleTransfers(isochData, transferData);

            //
            // Also complete any stale stages corresponding to the current transfer.
            //
            Isoch_Transfer_CompleteStaleStages(isochData, transferData, stageData);

            //
            // Complete the isoch TD, which may also complete the request if this is the last
            // TD in the URB.
            //
            Isoch_Stage_CompleteTD(stageData,
                                   Trb->CompletionCode,
                                   bytesTransferred,
                                   FALSE,
                                   &stageCompleted,
                                   &pendingStageCount);
        }

        eventHandled = TRUE;

    } FINALLY {

    }

    return eventHandled;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_EP_TransferEventHandler(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
{
    __analysis_assume(Context != NULL);

    if (Trb->EventData == 1) {
        return Isoch_ProcessTransferEventWithED1(Trb, Context);
    } else {
        return Isoch_ProcessTransferEventWithED0(Trb, Context);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_StartMapping(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is a command from the Endpoint object to start the mapping of transfers.
    This command is expected

    - the first time that the endpoint is enabled.
    - after the transfer cancellation process is completed.

--*/
{
    PISOCH_DATA     isochData;
    MAPPING_STATE   previousState;

    isochData = (PISOCH_DATA)Context;

    InterlockedExchange((volatile LONG*)&isochData->Flags.AsUlong32, 0);

    isochData->PendingStageCount = 0;

    previousState = TR_AttemptStateChange(isochData, MS_Any, MS_Mapping);

    if (previousState == MS_Stopped) {
        Isoch_MapTransfers(isochData);
    } else {
        NT_ASSERT(FALSE);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_EP_StopMapping(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is a command from the Endpoint object to stop the mapping of transfers.
    Mapping may get stopped asynchronously later and will be acknowledged by
    sending the MappingStopped message. This command is expected

    - when a driver-owned request gets canceled.
    - when the endpoint is about to be disabled.

--*/
{
    PISOCH_DATA     isochData;
    MAPPING_STATE   previousState;

    isochData = (PISOCH_DATA)Context;

    previousState = TR_AttemptStateChange(isochData, MS_Any, MS_Stopping);

    NT_ASSERT(previousState != MS_Stopped);
    NT_ASSERT(previousState != MS_Stopping);

#ifndef DISABLE_RING_EMPTY_TIMER
    if (previousState == MS_WaitingForTransferRingEmptyEvent) {
        WdfTimerStop(isochData->WdfRingEmptyTimer, FALSE);
    }
#endif

    if ((previousState == MS_Paused) || (previousState == MS_WaitingForTransferRingEmptyEvent)) {

        previousState = TR_AttemptStateChange(isochData, MS_Stopping, MS_Stopped);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_EP_IsMappingStoppedOrPaused(
    __in
        HANDLE Context
    )
{
    PISOCH_DATA isochData;

    isochData = (PISOCH_DATA)Context;

    return ((isochData->MappingState == MS_Paused) ||
            (isochData->MappingState == MS_Stopped));
}

VOID
Isoch_WdfEvtIoCanceledOnQueue(
    WDFQUEUE    WdfQueue,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine gets called when a request that is on the WDFQUEUE gets canceled.
    In general, the driver might have seen this request before, if the driver requeues
    the request under some conditions. In case of Isoch TransferRing, requests are
    never requeued by the driver. Which means that every time we get this callback,
    it is for a request that we have not seen before.

    The reason we implement this callback is that default WDF behavior is to complete
    the canceled WDFREQUEST synchronously, which is not Ok for USB URBs for compatibility
    reasons. This callback will ensure that the request gets completed from a Dpc.

--*/
{
    ULONG                   i;
    PISOCH_DATA             isochData;
    PISOCH_TRANSFER_DATA    transferData;
    PTRANSFER_URB           transferUrb;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    isochData    = (PISOCH_DATA)TR_GetContext(WdfQueue);
    transferData = &(GetRequestData(WdfRequest)->IsochTransferData);

    NT_ASSERT(transferData->Initialized == FALSE);

    WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
    WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

    transferUrb = (PTRANSFER_URB)wdfRequestParams.Parameters.Others.Arg1;

    transferUrb->Header.Status        = USBD_STATUS_CANCELED;
    transferUrb->TransferBufferLength = 0;
    transferUrb->Isoch.ErrorCount     = 0;

    for (i = 0; i < transferUrb->Isoch.NumberOfPackets; i += 1) {
        transferUrb->Isoch.IsochPacket[i].Status = USBD_STATUS_NOT_SET;
    }

    RtlZeroMemory(transferData, sizeof(ISOCH_TRANSFER_DATA));
    InitializeListHead(&transferData->ListEntry);

    transferData->CancelState      = TCS_Canceled;
    transferData->WdfRequest       = WdfRequest;
    transferData->WdfRequestStatus = STATUS_CANCELLED;

    TR_Lock(isochData);
    InsertTailList(&isochData->CompletionTransferList, &transferData->ListEntry);
    TR_Unlock(isochData);

    TR_QueueDpcForTransferCompletion(isochData);
}

VOID
Isoch_WdfEvtIoQueueReadyNotification(
    WDFQUEUE    WdfQueue,
    WDFCONTEXT  WdfContext
    )
/*++

Routine Description:

    This routine gets called when the number of waiting requests on the WDFQUEUE
    goes form 0 to non-zero. Waiting requests exclude driver-owned requests.

    This routine will start the mapping of transfers if the mapping state is
    currently paused.

--*/
{
    PISOCH_DATA     isochData;
    MAPPING_STATE   previousState;

    UNREFERENCED_PARAMETER(WdfQueue);

    isochData = (PISOCH_DATA)WdfContext;

    previousState = TR_AttemptStateChange(isochData, MS_Paused, MS_Mapping);

    if (previousState == MS_Paused) {

        Isoch_MapTransfers(isochData);

    } else {

        InterlockedExchange(&isochData->AttemptMapping, 1);

        previousState = TR_AttemptStateChange(isochData, MS_Paused, MS_Mapping);

        if (previousState == MS_Paused) {
            Isoch_MapTransfers(isochData);
        }
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Isoch_CompleteTransfers(
    __in
        PISOCH_DATA IsochData,
    __in
        PLIST_ENTRY CompletionList
    )
{
    ULONG                   stagesCompleted;
    PISOCH_STAGE_DATA       stageData;
    PISOCH_TRANSFER_DATA    transferData;

    while (IsListEmpty(CompletionList) == FALSE) {

        transferData = (PISOCH_TRANSFER_DATA)RemoveHeadList(CompletionList);
        InitializeListHead(&transferData->ListEntry);

        //
        // If the WdfRequestStatus is set, transfer was already prepared for completion,
        // and then queued for asynchronous completion. Just complete the request here.
        //
        if (transferData->WdfRequestStatus != STATUS_PENDING) {
            WdfRequestComplete(transferData->WdfRequest, transferData->WdfRequestStatus);
            continue;
        }

        //
        // Free all stages of the transfer.
        //
        stagesCompleted = 0;

        StageQueue_ForwardScanStart(&transferData->StageQueue);

        stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);

        while (stageData != NULL) {

            Isoch_Stage_FreeScatterGatherList(IsochData, stageData);

            TR_Lock(IsochData);
            Isoch_Stage_Release(IsochData, stageData);
            TR_Unlock(IsochData);

            stagesCompleted += 1;
            transferData->StagesCompleted += 1;

            stageData = StageQueue_ForwardScanGetNextStage(&transferData->StageQueue);
        }

        TR_Lock(IsochData);

        Isoch_Transfer_CompleteCancelable(IsochData,
                                          transferData,
                                          USBD_STATUS_NOT_SET,
                                          USBD_STATUS_CANCELED,
                                          TRUE,
                                          FALSE);

        IsochData->PendingStageCount -= stagesCompleted;

        NT_ASSERT(IsochData->PendingStageCount <= ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING);

        TR_Unlock(IsochData);
    }
}

_Requires_lock_held_(IsochData->Lock)
BOOLEAN
Isoch_DoesDriverOwnRequests(
    __in
        PISOCH_DATA IsochData
    )
{
    if (IsListEmpty(&IsochData->PendingTransferList) &&
        IsListEmpty(&IsochData->WaitingForCancelCallbackTransferList) &&
        IsListEmpty(&IsochData->CompletionTransferList)) {

        return FALSE;

    } else {

        return TRUE;
    }
}

VOID
Isoch_WdfEvtDpcForTransferCompletion(
    WDFDPC WdfDpc
    )
{
    BOOLEAN     acknowledgeTransfersReclaimed;
    PISOCH_DATA isochData;
    LIST_ENTRY  completionList;
    WDFQUEUE    wdfQueue;

    TRY {

        acknowledgeTransfersReclaimed = FALSE;

        wdfQueue = WdfDpcGetParentObject(WdfDpc);
        isochData = (PISOCH_DATA)TR_GetContext(wdfQueue);

        InitializeListHead(&completionList);

        TR_Lock(isochData);

        if (isochData->DpcRunning) {
            TR_Unlock(isochData);
            LEAVE;
        }

        isochData->DpcRunning = TRUE;

        do {

            APPEND_LIST(&completionList, &isochData->CompletionTransferList);

            TR_Unlock(isochData);

            Isoch_CompleteTransfers(isochData, &completionList);

            TR_Lock(isochData);

        } while (IsListEmpty(&isochData->CompletionTransferList) == FALSE);

        if (isochData->Flags.ReclaimTransfersAndAcknowledge) {

            //
            // Transfers reclaimed acknowledgement should be sent only after ensuring
            // that the number of 'driver owned' requests is 0.
            //
            if (Isoch_DoesDriverOwnRequests(isochData) == FALSE) {

                isochData->Flags.ReclaimTransfersAndAcknowledge = 0;
                acknowledgeTransfersReclaimed = TRUE;
            }
        }

        isochData->DpcRunning = FALSE;

        TR_Unlock(isochData);

    } FINALLY {

        if (acknowledgeTransfersReclaimed) {

            LogVerbose(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                       "%u.%u.0: Message to State Machine: TransfersReclaimed",
                       UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(isochData->EndpointHandle));

            TR_TransfersReclaimed(isochData);
        }
    }

}

VOID
Isoch_TransferUrb_MarkLatePackets(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData
    )
/*++

Routine Description:

    Side effects: Updates BytesSkipped and PacketsSkipped fields in TransferData.

--*/
{
    ULONG                       currentFrame;
    ULONG                       frameCount;
    ULONG                       firstFrame;
    ULONG                       firstPacket;
    ULONG                       i;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;
    ULONG                       packetCount;
    ULONG                       packetIndex;
    PTRANSFER_URB               transferUrb;
    ULONG                       xhciFrame;

    transferUrb =
        TransferData->TransferUrb;

    isochPacketArray =
        transferUrb->Isoch.IsochPacket;

    firstPacket =
        TransferData->PacketsMapped + TransferData->PacketsSkipped;

    firstFrame =
        transferUrb->Isoch.StartFrame + Isoch_GetFrameCount(IsochData, firstPacket);

    xhciFrame =
        Controller_GetFrameNumber(IsochData->ControllerHandle, MICROFRAME_INCREMENT_PREVENT_LATE_TD);

    packetCount =
        TransferData->PacketsTotal - firstPacket;

    frameCount =
        Isoch_GetFrameCount(IsochData, packetCount);

    NT_ASSERTMSG("Stage must start on a Frame boundary",
                 ((firstPacket % IsochData->PacketsPerFrame) == 0));

    //
    // Determine if each packet has a start frame that is greater than the current
    // bus frame count. If not, then mark the packet as late. If all of the packets
    // are marked late, then the request is completed with a failure. The start
    // frame must be a few frames before the current frame count in order to provide
    // time to map the request into the controller.
    //
    // Note that this code was taken from the USB2 stack. It does not support
    // wrapping of the frame count and it does not increment the URB error
    // count when setting a non-success usbd status in the isoch packet.
    //
    for (currentFrame = firstFrame, packetIndex = firstPacket;
         currentFrame < (firstFrame + frameCount);
         currentFrame += 1) {

        if (currentFrame > xhciFrame) {
            break;
        }

        for (i = 0; i < IsochData->PacketsPerFrame; i += 1) {

            NT_ASSERT(isochPacketArray[packetIndex].Status == USBD_STATUS_NOT_SET);

            isochPacketArray[packetIndex].Status = USBD_STATUS_ISO_NOT_ACCESSED_LATE;

            TransferData->BytesSkipped   += Isoch_GetPacketLength(TransferData, packetIndex);
            TransferData->PacketsSkipped += 1;
            packetIndex                  += 1;
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Isoch_TransferUrb_Initialize(
    __inout
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData
    )
/*++

Routine Description:

    Initialize fields in the Urb like TransferBufferLength, default status in each of
    the packets etc. Also determine the StartFrame if the ASAP flag was specified in
    the URB.

    Side effects: Updates RingDoorbellOnFirstTD and NextTransferStartFrame fields in
    IsochData.

--*/
{
    ULONG                   i;
    ULONG                   maxPacketCount;
    NTSTATUS                status;
    PTRANSFER_URB           transferUrb;
    ULONG                   xhciFrame;

    TRY {

        status      = STATUS_SUCCESS;
        transferUrb = TransferData->TransferUrb;

        //
        // Update if needed, and validate the StartFrame in the Urb.
        //
        if (transferUrb->TransferFlags & USBD_START_ISO_TRANSFER_ASAP) {

            xhciFrame = Controller_GetFrameNumber(IsochData->ControllerHandle, MICROFRAME_INCREMENT_DEFAULT);

            if (IsochData->IsochPure) {

                IsochData->RingDoorbellOnFirstTD  = TRUE;
                transferUrb->Isoch.StartFrame     = xhciFrame + ISOCH_TRANSFER_DELAY;
                IsochData->NextTransferStartFrame = transferUrb->Isoch.StartFrame;

            } else {

                transferUrb->Isoch.StartFrame = IsochData->NextTransferStartFrame;

                //
                // Check to see if this request is out of range. If so, treat it as
                // the first request received. This is done to mimic the behavior
                // in the USB2 stack.
                //
                if ((UsbDevice_GetDeviceSpeed(IsochData->UsbDeviceHandle) == UsbHighSpeed) ||
                    (UsbDevice_GetDeviceSpeed(IsochData->UsbDeviceHandle) == UsbSuperSpeed)) {
                    maxPacketCount = 1024;
                } else {
                    maxPacketCount = 255;
                }

                //
                // Note that this code doesn't take into account a wrap around of
                // the frame number.
                //
                if ((xhciFrame > transferUrb->Isoch.StartFrame) &&
                    ((xhciFrame - transferUrb->Isoch.StartFrame) > maxPacketCount)) {

                    IsochData->RingDoorbellOnFirstTD  = TRUE;
                    transferUrb->Isoch.StartFrame     = xhciFrame + ISOCH_TRANSFER_DELAY;
                    IsochData->NextTransferStartFrame = transferUrb->Isoch.StartFrame;
                }
            }

        } else {

            //
            // COMPAT: Folloiwng line has been added due to compat with the 2.0 stack.
            //
            IsochData->NextTransferStartFrame = transferUrb->Isoch.StartFrame;
        }

        IsochData->NextTransferStartFrame +=
            Isoch_GetFrameCount(IsochData, transferUrb->Isoch.NumberOfPackets);

        //
        // This is done to maintain COMPAT with the USB 2.0 stack.
        //
        transferUrb->TransferBufferLength = 0;
        transferUrb->Isoch.ErrorCount     = 0;

        for (i = 0; i < transferUrb->Isoch.NumberOfPackets; i += 1) {
            transferUrb->Isoch.IsochPacket[i].Status = USBD_STATUS_NOT_SET;
        }

        xhciFrame = Controller_GetFrameNumber(IsochData->ControllerHandle, MICROFRAME_INCREMENT_PREVENT_LATE_TD);

        //
        // If the start frame is not within the frame range, then mark each isoch packet
        // as not accessed and complete the transfer with a failure. This behavior is
        // taken from the USB2 stack.
        //
        if (ABS((LONG)(transferUrb->Isoch.StartFrame - xhciFrame)) > USBD_ISO_START_FRAME_RANGE) {

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        IsochData->IsochPure = FALSE;

    } FINALLY {

        LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p packets %u, xhciFrame 0x%X, StartFrame 0x%X, NextStartFrame 0x%X",
                UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                Endpoint_GetContextIndex(IsochData->EndpointHandle), TransferData->WdfRequest,
                transferUrb->Isoch.NumberOfPackets, xhciFrame, transferUrb->Isoch.StartFrame,
                IsochData->NextTransferStartFrame);
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Isoch_Stage_AcquireMdl(
    __inout
        PISOCH_STAGE_DATA StageData
    )
/*++

Routine Description:

    In the general case of multiple stages per transfer and Chained MDLs, pick up
    where we left off by finding the location in the buffer where this stage needs
    to continue the transfer, and allocating a partial MDL, if that location is not
    on an MDL boundary.

--*/
{
    PUCHAR                  dataAddress;
    ULONG                   dataLength;
    PISOCH_DATA             isochData;
    PMDL                    startMdl;
    ULONG                   startOffset;
    NTSTATUS                status;
    PISOCH_TRANSFER_DATA    transferData;

    TRY {

        status       = STATUS_SUCCESS;
        transferData = StageData->TransferData;
        isochData    = transferData->IsochData;
        startMdl     = transferData->Mdl;
        startOffset  = transferData->BytesMapped + transferData->BytesSkipped;

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

            LogError(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                     "%u.%u.0: WdfRequest 0x%p, Failed to allocate partial Mdl for stage 0x%p",
                     UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(isochData->EndpointHandle),
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

VOID
Isoch_Stage_DetermineSize(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        PISOCH_STAGE_DATA       StageData
    )
/*++

Routine Description:

    Determine the number of bytes to map for the supplied stage. Constraints on the
    size of a stage:

    1. Number of bytes left to map for the transfer.
    1. Number of DMA map registers that are available per DMA transaction.
    2. Stage must end on a packet boundary.
    3. Stage must end on a frame boundary.

    Side effects: Updates StageData->LastPacket in addition to StageData->TransferSize.

--*/
{
    ULONG               currentFrameLength;
    ULONG               cumulativeFrameLength;
    ULONG               frameIndex;
    ULONG               i;
    ULONG               packetIndex;
    ULONG               stageSize;
    ULONG               dmaLimit;
    ULONG               stageLimit;
    ULONG               lastPacket;
    PDMA_ADAPTER        dmaAdapter;
    DMA_TRANSFER_INFO   dmaTransferInfo;
    PTRANSFER_URB       transferUrb;

    TRY {

        NT_ASSERTMSG("Stage must start on a Frame boundary",
                     ((StageData->FirstPacket % IsochData->PacketsPerFrame) == 0));

        stageSize = (TransferData->BytesTotal -
                     (TransferData->BytesMapped + TransferData->BytesSkipped));

        stageLimit = IsochData->MaxStageSize;
        transferUrb = TransferData->TransferUrb;

        dmaAdapter =
            CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(IsochData->ControllerHandle));

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

        //
        // Limit the max stage size if the transfer requires more map registers than
        // what is available.
        //

        if (dmaTransferInfo.V1.MapRegisterCount > IsochData->MapRegisterCount) {

            dmaLimit = (IsochData->MapRegisterCount - 1) * PAGE_SIZE;
            if (dmaLimit < stageLimit) {
                stageLimit = dmaLimit;
            }
        }

        //
        // Find a stage size that is smaller than the MaxStageSize but also makes
        // the stage end on a frame boundary.
        //

        if (stageSize <= stageLimit) {

            lastPacket = TransferData->PacketsTotal - 1;
            LEAVE;

        }

        stageSize = stageLimit;

        //
        // TODO: Change to more efficient lookup than going packet by packet.
        //

        cumulativeFrameLength = 0;
        packetIndex           = StageData->FirstPacket;
        frameIndex            = StageData->FirstPacket / IsochData->PacketsPerFrame;

        WHILE (TRUE) {

            currentFrameLength = 0;

            for (i = 0; i < IsochData->PacketsPerFrame; i += 1) {

                currentFrameLength += Isoch_GetPacketLength(TransferData, packetIndex);
                packetIndex += 1;
            }

            if ((cumulativeFrameLength + currentFrameLength) > stageSize) {
                stageSize  = cumulativeFrameLength;
                lastPacket = (frameIndex * IsochData->PacketsPerFrame) - 1;
                break;
            }

            cumulativeFrameLength += currentFrameLength;
            frameIndex += 1;
        }

        NT_ASSERT(packetIndex <= TransferData->PacketsTotal);

    } FINALLY {

        NT_ASSERTMSG("Stage must end on a Frame boundary",
                     (((lastPacket + 1) % IsochData->PacketsPerFrame) == 0));

        StageData->LastPacket   = lastPacket;
        StageData->TransferSize = stageSize;
    }

    return;
}

ULONG
Isoch_Stage_EstimateRequiredSegments(
    __in
        PISOCH_DATA         IsochData,
    __in
        PISOCH_STAGE_DATA   StageData
    )
/*++

Routine Description:

    Returns an estimate of the number of extra segments needed to map the supplied
    stage of the transfer after accounting for the TRBs remaining in the current
    segment. This is only an estimate, as figuring out the correct number is expensive
    and difficult to do correctly because the following things have to be taken into
    account:

    1. Buffer can be in the form of a Chained-MDL with no restriction on the size or
       each MDL.
    2. Each Isoch Packet can be of a different length and can span multiple MDLs.
    3. Last few TRBs in a segment may be wasted to hold the guarantee that an Isoch
       TD does not cross segments, by just starting the Isoch TD on the next segment.

    Caller is responsible for handling the case where less segments are allocated
    than required.

--*/
{
    ULONG availableTrbsInCurrentSegment;
    ULONG estimatedTrbs;
    ULONG estimatedTrbsPerTD;
    ULONG estimatedSegments;
    ULONG packets;
    ULONG usableTrbsPerSegment;

    estimatedTrbsPerTD = 1;

    estimatedTrbsPerTD += BufferSizeToSpanPages(
                              Endpoint_GetMaxPayloadPerServiceOpportunity(
                                  IsochData->EndpointHandle));

    //
    // Overestimate the number of TRBs per TD in case we may have to extend this
    // Isoch TD to the next segment by inserting a dummy Normal TRB.
    //
    if (Isoch_PreventStandaloneLinkTD(IsochData)) {
        estimatedTrbsPerTD += 1;
    }

    //
    // Add a couple of TRBs per TD for Chained MDL case (WIN8 logic).
    //
    if (MmGetMdlByteCount(StageData->Mdl) < StageData->TransferSize) {
        estimatedTrbsPerTD += 2;
    }

    IsochData->EstimatedTrbsPerTD = estimatedTrbsPerTD;

    packets       = StageData->LastPacket - StageData->FirstPacket + 1;
    estimatedTrbs = estimatedTrbsPerTD * packets;

    availableTrbsInCurrentSegment  = IsochData->MaxSegmentIndex - IsochData->CurrentSegmentIndex;
    availableTrbsInCurrentSegment -= (availableTrbsInCurrentSegment % estimatedTrbsPerTD);

    if (estimatedTrbs <= availableTrbsInCurrentSegment) {

        estimatedSegments = 0;

    } else {

        estimatedTrbs -= availableTrbsInCurrentSegment;

        usableTrbsPerSegment =
            IsochData->MaxSegmentIndex - (IsochData->MaxSegmentIndex % estimatedTrbsPerTD);

        estimatedSegments = (estimatedTrbs + (usableTrbsPerSegment - 1)) / usableTrbsPerSegment;
    }

    //
    // After all TDs for this stage are programmed, programming for the next stage may
    // have to start by inserting a Link TD if there are not enough TRBs in the current
    // segment. To prevent that case of inserting a standalone Link TD, the last TD of
    // every stage will be extended to the next segment. For this reason, acquire an
    // extra segment.
    //
    if (Isoch_PreventStandaloneLinkTD(IsochData)) {
        estimatedSegments += 1;
    }

    LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
               "%u.%u.0: Stage 0x%p EstimatedSegments %u estimatedTrbsPerTD %u packets %u",
               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
               Endpoint_GetContextIndex(IsochData->EndpointHandle),
               StageData, estimatedSegments, estimatedTrbsPerTD, packets);

    return estimatedSegments;
}

__drv_maxIRQL(DISPATCH_LEVEL)
MAP_STAGE_RESULT
Isoch_MapStage(
    __in
        PISOCH_DATA IsochData
    )
/*++

Routine Description:


--*/
{
    PDEVICE_OBJECT          deviceObject;
    PDMA_ADAPTER            dmaAdapter;
    MAP_STAGE_RESULT        result;
    KIRQL                   savedIrql;
    PISOCH_STAGE_DATA       stageData;
    NTSTATUS                status;
    PISOCH_TRANSFER_DATA    transferData;
    PTRANSFER_URB           transferUrb;

    stageData    = IsochData->CurrentStage;
    transferData = stageData->TransferData;
    transferUrb  = stageData->TransferData->TransferUrb;

    IsochData->LateDetected = 0;

    dmaAdapter =
        CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(IsochData->ControllerHandle));

    deviceObject =
        WdfDeviceWdmGetDeviceObject(Controller_GetWdfDeviceObject(IsochData->ControllerHandle));

    KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);

    status = dmaAdapter->DmaOperations->GetScatterGatherList(
                dmaAdapter,
                deviceObject,
                stageData->Mdl,
                MmGetMdlVirtualAddress(stageData->Mdl),
                stageData->TransferSize,
                Isoch_EvtDmaCallback,
                stageData,
                ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));

    KeLowerIrql(savedIrql);

    if (NT_SUCCESS(status)) {

        if (InterlockedXor(&IsochData->ContinueMapping, 1) == 1) {

            if (IsochData->LateDetected == 0) {
                result = MSR_Success;
            } else {
                result = MSR_LateDetected;
            }

        } else {

            result = MSR_WaitingOnDmaCallback;
        }

    } else {

        TR_Lock(IsochData);

        Isoch_Stage_Release(IsochData, stageData);

        if (transferData->StagesCompleted == transferData->StagesMapped) {

            Isoch_Transfer_CompleteCancelable(IsochData,
                                              transferData,
                                              USBD_STATUS_NOT_SET,
                                              USBD_STATUS_INSUFFICIENT_RESOURCES,
                                              FALSE,
                                              FALSE);

        } else {

            transferData->PacketsSkipped +=
                (transferData->PacketsTotal -
                 (transferData->PacketsMapped + transferData->PacketsSkipped));
        }

        TR_Unlock(IsochData);

        result = MSR_GetScatterGatherListFailed;
    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
PROCESS_STAGE_RESULT
Isoch_PrepareStage(
    __in
        PISOCH_DATA IsochData
    )
/*++

Routine Description:

    Prepares a stage for mapping by setting up the MDL, calculating and acquiring segments,
    and creating ScatterGatherList.

--*/
{
    ULONG                   estimatedRequiredSegments;
    PISOCH_STAGE_DATA       stageData;
    ULONG                   startFrame;
    NTSTATUS                status;
    PROCESS_STAGE_RESULT    result;
    PISOCH_TRANSFER_DATA    transferData;
    PTRANSFER_URB           transferUrb;

    TRY {

        stageData    = IsochData->CurrentStage;
        transferData = stageData->TransferData;
        transferUrb  = transferData->TransferUrb;

        stageData->FirstPacket = transferData->PacketsMapped + transferData->PacketsSkipped;

        startFrame = transferUrb->Isoch.StartFrame +
                     Isoch_GetFrameCount(IsochData, stageData->FirstPacket);

        if (!Register_GetContiguousFrameIDCapability(Controller_GetRegisterHandle(IsochData->ControllerHandle)) &&
            IsochData->Flags.FrameNumberOfLastTDInScheduleValid &&
            (startFrame != (IsochData->FrameNumberOfLastTDInSchedule + 1))) {

            if (IsochData->Flags.TransferRingEmptyEventExpected || (IsochData->PendingStageCount != 0)) {

                LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequest 0x%p Stage 0x%p Gap Detected. Stage StartFrame 0x%x, FrameOfLastMappedTD 0x%x",
                        UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(IsochData->EndpointHandle),
                        transferData->WdfRequest, stageData, startFrame, IsochData->FrameNumberOfLastTDInSchedule);

                TR_Lock(IsochData);
                Isoch_Stage_Release(IsochData, stageData);
                TR_Unlock(IsochData);

                result = PSR_GapDetected;
                LEAVE;
            }
        }

        status = Isoch_Stage_AcquireMdl(stageData);

        if (!NT_SUCCESS(status)) {

            TR_Lock(IsochData);

            Isoch_Stage_Release(IsochData, stageData);

            if (transferData->StagesCompleted == transferData->StagesMapped) {

                Isoch_Transfer_CompleteCancelable(IsochData,
                                                  transferData,
                                                  USBD_STATUS_NOT_SET,
                                                  USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                  FALSE,
                                                  FALSE);

                result = PSR_TransferCompletedPrematurely;

            } else {

                transferData->PacketsSkipped +=
                    (transferData->PacketsTotal -
                     (transferData->PacketsMapped + transferData->PacketsSkipped));

                result = PSR_ResourceAllocationFailed;
            }

            TR_Unlock(IsochData);
            LEAVE;
        }

        Isoch_Stage_DetermineSize(IsochData, transferData, stageData);

        estimatedRequiredSegments =
            Isoch_Stage_EstimateRequiredSegments(IsochData, stageData);

        if (estimatedRequiredSegments == 0) {
            result = PSR_Success;
            LEAVE;
        }

        status = TR_AcquireSegments(IsochData, estimatedRequiredSegments, &stageData->FreeSegmentList);

        if (status == STATUS_PENDING) {

            LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: WdfRequest 0x%p Stage 0x%p Waiting to acquire %u segments",
                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(IsochData->EndpointHandle),
                    transferData->WdfRequest, stageData, estimatedRequiredSegments);

            TR_Lock(IsochData);

            Isoch_Stage_Release(IsochData, stageData);

            TR_Unlock(IsochData);

            result = PSR_WaitingOnCommonBuffer;

        } else if (!NT_SUCCESS(status)) {

            LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                    "%u.%u.0: WdfRequest 0x%p Stage 0x%p Failed to acquire %u segments",
                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(IsochData->EndpointHandle),
                    transferData->WdfRequest, stageData, estimatedRequiredSegments);

            TR_Lock(IsochData);

            Isoch_Stage_Release(IsochData, stageData);

            if (transferData->StagesCompleted == transferData->StagesMapped) {

                Isoch_Transfer_CompleteCancelable(IsochData,
                                                  transferData,
                                                  USBD_STATUS_NOT_SET,
                                                  USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                  FALSE,
                                                  FALSE);

                result = PSR_TransferCompletedPrematurely;

            } else {

                transferData->PacketsSkipped +=
                    (transferData->PacketsTotal -
                     (transferData->PacketsMapped + transferData->PacketsSkipped));

                result = PSR_ResourceAllocationFailed;
            }

            TR_Unlock(IsochData);

        } else {

            IsochData->CommonBufferAcquireAttemptCount = 0;
            result = PSR_Success;
        }

    } FINALLY {

    }

    return result;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_InsertLinkTrb(
    __in
        PISOCH_DATA         IsochData,
    __in
        PISOCH_STAGE_DATA   StageData,
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
        return FALSE;
    }

    nextSegment = (PBUFFER_DATA)RemoveHeadList(&StageData->FreeSegmentList);
    InitializeListHead(&nextSegment->ListEntry);

    TR_InitializeLinkTrb(IsochData, TrbStage, &localTrb, FirstTrb);

    localTrb.Link.RingSegmentPointer = nextSegment->LogicalAddress.QuadPart;
    localTrb.Link.ToggleCycle = 0;

    RtlCopyMemory(&IsochData->CurrentSegment[IsochData->CurrentSegmentIndex],
                  &localTrb,
                  sizeof(TRB));

    TR_Lock(IsochData);

    InsertTailList(&StageData->UsedSegmentList, &IsochData->CurrentSegmentBufferData->ListEntry);

    IsochData->CurrentSegmentBufferData = nextSegment;
    IsochData->CurrentSegment           = (PTRB)nextSegment->VirtualAddress;
    IsochData->CurrentSegmentIndex      = 0;
    StageData->TrbRange.LastSegment     = nextSegment;

    TR_Unlock(IsochData);

    return TRUE;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Isoch_Stage_MapIntoRing(
    __in
        PISOCH_STAGE_DATA StageData
    )
/*++

Routine Description:

    This function inserts a stage of a transfer, which can be the entire transfer,
    into the transfer ring.

    N.B: There will never be a frame number gap between the Isoch TDs that comprise
         a stage.

--*/
{
    UCHAR                  beiValue;
    ULONG                  currentElement;
    PTRB                   currentTrb;
    ULONG64                elementAddress;
    ULONG                  elementLength;
    ULONG                  firstElementOffsetForThisTd;
    ULONG                  firstElemetForThisTd;
    ULONG                  firstRingIndexForThisTd;
    PTRB                   firstTrbInCurrentTd;
    ULONG                  frameNumber;
    PISOCH_DATA            isochData;
    ULONG                  packetLength;
    PISOCH_TRACKING_DATA   isochTrackingData;
    ULONG                  lengthMapped;
    BOOLEAN                linkInserted;
    TRB                    localTrb;
    ULONG                  maxPacketSizePerInterval;
    ULONG                  packetIndex;
    PSCATTER_GATHER_LIST   scatterGatherList;
    ULONG                  stagingDataUsedTrbsBeforeThisTd;
    ULONG                  tdPacketCount;
    PISOCH_TRANSFER_DATA   transferData;
    PTRANSFER_URB          transferUrb;
    TRB_STAGE              trbStage;
    ULONG                  usedTrbs;
    ULONG                  xhciFrameNumber;
    BOOLEAN                forceLinkTrb;
    BOOLEAN                extendedTBC;
    BOOLEAN                extendedTBCIsoch;
    HANDLE                 registerHandle;

    TRY {

        ICE_START_TRACE();

        transferData     = StageData->TransferData;
        isochData        = transferData->IsochData;
        transferUrb      = transferData->TransferUrb;
        tdPacketCount    = 0;
        forceLinkTrb     = FALSE;

        if (Controller_GetDeviceFlags(isochData->ControllerHandle).DisableBEI) {
            beiValue = 0;
        } else {
            beiValue = 1;
        }

        packetIndex = StageData->FirstPacket;

        usedTrbs = 0;

        currentElement = 0;
        scatterGatherList = StageData->ScatterGatherList;
        elementAddress = scatterGatherList->Elements[currentElement].Address.QuadPart;
        elementLength = scatterGatherList->Elements[currentElement].Length;

        lengthMapped = 0;
        frameNumber = 0;

        firstElemetForThisTd = currentElement;
        firstElementOffsetForThisTd = (ULONG) (elementAddress - scatterGatherList->Elements[currentElement].Address.QuadPart);
        firstRingIndexForThisTd = isochData->CurrentSegmentIndex;
        stagingDataUsedTrbsBeforeThisTd = 0;

        maxPacketSizePerInterval =
            Endpoint_GetMaxPayloadPerServiceOpportunity(isochData->EndpointHandle);

        packetLength = Isoch_GetPacketLength(transferData, packetIndex);

        if (Controller_GetDeviceFlags(isochData->ControllerHandle).PreventSchedulingLateIsochTd) {

            frameNumber = Iosch_GetPacketFrameNumber(isochData,
                                                     transferUrb->Isoch.StartFrame,
                                                     packetIndex);

            xhciFrameNumber = Controller_GetFrameNumber(isochData->ControllerHandle,
                                                        MICROFRAME_INCREMENT_PREVENT_LATE_TD);


            if (((LONG)(xhciFrameNumber - frameNumber)) >= 0) {

                LogError(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequest 0x%p Stage 0x%p is late. xhciFrame 0x%X, packet 0x%u, packetFrame 0x%X",
                        UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(isochData->EndpointHandle), transferData->WdfRequest,
                        StageData, xhciFrameNumber, packetIndex, frameNumber);

                Isoch_Stage_FreeScatterGatherList(isochData, StageData);

                TR_Lock(isochData);

                Isoch_Stage_Release(isochData, StageData);

                isochData->Flags.TransferRingEmptyEventExpected = 1;

                TR_Unlock(isochData);

                UsbDevice_WriteDoorbell(Endpoint_GetUsbDeviceHandle(isochData->EndpointHandle),
                                        Endpoint_GetContextIndex(isochData->EndpointHandle),
                                        isochData->StreamId);

                isochData->LateDetected = 1;
                LEAVE;
            }
        }

        //
        // firstTrbInCurrentTd points to the TRB whose CycleBit is set after the Isoch TD is
        // mapped. Normally, it will point to an Isoch TRB, which is the first TRB
        // in an Isoch TD. But it can point to a Link TRB, if the next Isoch TD is
        // started on a new segment.
        //
        firstTrbInCurrentTd = NULL;

        trbStage = TrbStage_Isoch;

        if (isochData->CurrentSegmentIndex >= isochData->MaxSegmentIndex) {

            NT_ASSERT(isochData->CurrentSegmentIndex == isochData->MaxSegmentIndex);
            //
            // It seems that that last stage consumed all entries of the current Ring.
            // Immediately insert a link Trb.
            //
            firstTrbInCurrentTd = &isochData->CurrentSegment[isochData->CurrentSegmentIndex];
            linkInserted = Isoch_InsertLinkTrb(isochData, StageData, trbStage, TRUE);

            //
            // We have to have atleast one ring segment available this routine is called.
            //
            NT_ASSERT(linkInserted);
            UNREFERENCED_PARAMETER(linkInserted);

            //
            // Now we have a new transfer ring segment
            //
            firstRingIndexForThisTd = 0;
        }

        registerHandle = Controller_GetRegisterHandle(isochData->ControllerHandle);
        extendedTBC = ((Register_GetExtendedTBC(registerHandle) != 0) &&
                       (Register_GetLargeESITPayload(registerHandle) != 0));

        while (trbStage != TrbStage_Done) {

            NT_ASSERT(isochData->CurrentSegmentIndex <= isochData->MaxSegmentIndex);

            currentTrb = &isochData->CurrentSegment[isochData->CurrentSegmentIndex];
            RtlZeroMemory(&localTrb, sizeof(TRB));

            if (firstTrbInCurrentTd == NULL) {
                NT_ASSERT(trbStage == TrbStage_Isoch);
                firstTrbInCurrentTd = currentTrb;
            }

            //LogVerbose(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
            //           "%u.%u.%u: Insert: CurrentSegment[%u] 0x%p Stage[%u] usedTrbs %u",
            //           UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
            //           Endpoint_GetContextIndex(isochData->EndpointHandle),
            //           isochData->StreamId,
            //           isochData->CurrentSegmentIndex,
            //           &isochData->CurrentSegment[isochData->CurrentSegmentIndex],
            //           trbStage,
            //           usedTrbs);

            extendedTBCIsoch = FALSE;

            switch (trbStage) {

            //
            // Excluding the direction, the Data and Normal TRBs are identical. This
            // code references the Normal TRB for mapping the data.
            //
            case TrbStage_Isoch:
            case TrbStage_Normal:

                if (trbStage == TrbStage_Isoch) {

                    localTrb.Isoch.Type = TRB_TYPE_ISOCH;

                    frameNumber = Iosch_GetPacketFrameNumber(isochData,
                                                             transferUrb->Isoch.StartFrame,
                                                             packetIndex);

                    localTrb.Isoch.FrameId = (frameNumber & ISOCH_MAX_FRAME_MASK);

                    //
                    // Extended TBC resues TDSize to place the actual transfer burst count.
                    //
                    if (extendedTBC) {
                        localTrb.Isoch.TDSize = Isoch_GetTBC(isochData, packetLength);
                    } else {
                        localTrb.Isoch.TransferBurstCount = Isoch_GetTBC(isochData, packetLength);
                    }
                    extendedTBCIsoch = extendedTBC;

                    localTrb.Isoch.TransferLastBurstPacketCount = Isoch_GetTLBPC(isochData, packetLength);

                    tdPacketCount = TR_GetPacketCount(isochData, packetLength);

                } else {

                    localTrb.Normal.Type = TRB_TYPE_NORMAL;
                }

                if (elementLength <= packetLength) {

                    localTrb.Normal.DataBufferPointer = elementAddress;
                    localTrb.Normal.TransferLength = elementLength;

                    packetLength -= elementLength;

                    //
                    // Fetch the next element
                    //
                    currentElement += 1;

                    if (currentElement < scatterGatherList->NumberOfElements) {
                        elementAddress = scatterGatherList->Elements[currentElement].Address.QuadPart;
                        elementLength  = scatterGatherList->Elements[currentElement].Length;
                    } else {
                        elementAddress = 0;
                        elementLength  = 0;
                        NT_ASSERT(packetLength == 0);
                    }

                    //
                    // If the length mapped in this TD is greater than what is supported,
                    // set it to the max packet size allowed for the isoch TD.
                    //
                    if ((lengthMapped + localTrb.Normal.TransferLength) > maxPacketSizePerInterval) {

                        localTrb.Normal.TransferLength = maxPacketSizePerInterval - lengthMapped;

                        //
                        // we need to advance beyond the current location in scatter gather list.
                        // This is done because we had to prematurely terminate a TD and we need to make
                        // sure that the buffer associated with the TD is also skipped.
                        //
                        while (packetLength) {

                            if (elementLength <= packetLength) {

                                packetLength -= elementLength;

                                //
                                // Fetch the next element
                                //
                                currentElement += 1;
                                if (currentElement >= scatterGatherList->NumberOfElements) {
                                    elementAddress = 0;
                                    elementLength = 0;
                                    break;
                                }
                                elementAddress = scatterGatherList->Elements[currentElement].Address.QuadPart;
                                elementLength  = scatterGatherList->Elements[currentElement].Length;

                            } else {
                                elementAddress += packetLength;
                                elementLength -= packetLength;
                                packetLength = 0;
                            }
                        }
                    }

                    if (packetLength == 0) {

                        if (packetIndex == StageData->LastPacket) {

                            trbStage = TrbStage_FinalEventData;

                        } else {

                            //
                            // N.B: Mapping must continue until all the packets for the stage are mapped. That
                            // is, even if the ScatterGatherList becomes empty, continue mapping the reset of
                            // the packets. Those packets should be of zero length.
                            //
                            trbStage = TrbStage_EventData;
                        }

                    } else {

                        trbStage = TrbStage_Normal;
                    }

                } else {

                    localTrb.Normal.DataBufferPointer = elementAddress;
                    localTrb.Normal.TransferLength = packetLength;

                    elementAddress += packetLength;
                    elementLength  -= packetLength;

                    //
                    // If the length mapped in this TD is greater than what is supported,
                    // set it to the max packet size allowed for the isoch TD.
                    //
                    if ((lengthMapped + localTrb.Normal.TransferLength) > maxPacketSizePerInterval) {

                        localTrb.Normal.TransferLength = maxPacketSizePerInterval - lengthMapped;
                    }

                    trbStage = TrbStage_EventData;
                }

                lengthMapped += localTrb.Normal.TransferLength;

                //
                // Extended TBC Isoch reuses TDSize for transfer burst count, see above.
                //
                if (!extendedTBCIsoch) {
                    localTrb.Normal.TDSize =
                        TR_CalculateTDSize(isochData,
                                           tdPacketCount,
                                           lengthMapped,
                                           (trbStage == TrbStage_EventData ||
                                           trbStage == TrbStage_FinalEventData) ? TRUE : FALSE);
                }

                localTrb.Normal.ChainBit = 1;

                if (Isoch_PreventStandaloneLinkTD(isochData)) {

                    //
                    // Extend this TD to the next segment if we think that the next TD
                    // might not fit into the rest of the TRBs of current segment.
                    //
                    if (trbStage == TrbStage_EventData) {

                        if ((isochData->MaxSegmentIndex - (isochData->CurrentSegmentIndex + 1)) <
                            isochData->EstimatedTrbsPerTD) {

                            trbStage = TrbStage_DummyNormal;
                        }
                    }

                    //
                    // Always extend last TD of the stage to the next segment.
                    //
                    if (trbStage == TrbStage_FinalEventData) {
                        trbStage = TrbStage_FinalDummyNormal;
                    }
                }

                if ((trbStage == TrbStage_EventData) || (trbStage == TrbStage_FinalEventData)) {

                    localTrb.Normal.EvaluateNextTrb = 1;
                }

                break;

            case TrbStage_EventData:
            case TrbStage_FinalEventData:

                isochTrackingData = (PISOCH_TRACKING_DATA)&localTrb.EventData.Data;
                isochTrackingData->EndpointType     = USB_ENDPOINT_TYPE_ISOCHRONOUS;
                isochTrackingData->IsochPacketIndex = packetIndex;
                isochTrackingData->FrameNumber      = transferUrb->Isoch.StartFrame;

                localTrb.EventData.InterruptOnCompletion = 1;
                localTrb.EventData.Type = TRB_TYPE_EVENT_DATA;

                packetIndex += 1;

                //
                // There are cases where a malformed URB is received, where each
                // isoch packet in the URB is greater than the max packet size
                // allowed for the EP. To handle this condition, in addition to
                // checking if this is the final event data, also check to see
                // if there are additional isoch packets to be processed.
                //
                if ((trbStage == TrbStage_FinalEventData) ||
                    (packetIndex == transferData->PacketsTotal)) {        // TODO: shady logic.

                    trbStage = TrbStage_Done;

                } else {

                    if (packetIndex & MAX_ISOCH_IOC_INTERVAL_MASK) {
                        localTrb.EventData.BlockEventInterrupt = beiValue;
                    }

                    lengthMapped = 0;

                    packetLength = Isoch_GetPacketLength(transferData, packetIndex);

                    trbStage = TrbStage_Isoch;
                }

                break;

            case TrbStage_DummyNormal:

                localTrb.Normal.Type              = TRB_TYPE_NORMAL;
                localTrb.Normal.DataBufferPointer = 0;
                localTrb.Normal.TransferLength    = 0;
                localTrb.Normal.TDSize            = 0;
                localTrb.Normal.ChainBit          = 1;
                localTrb.Normal.EvaluateNextTrb   = 1;

                forceLinkTrb = TRUE;
                trbStage = TrbStage_EventData;
                break;

            case TrbStage_FinalDummyNormal:

                localTrb.Normal.Type              = TRB_TYPE_NORMAL;
                localTrb.Normal.DataBufferPointer = 0;
                localTrb.Normal.TransferLength    = 0;
                localTrb.Normal.TDSize            = 0;
                localTrb.Normal.ChainBit          = 1;
                localTrb.Normal.EvaluateNextTrb   = 1;

                forceLinkTrb = TRUE;
                trbStage = TrbStage_FinalEventData;
                break;
            }

            //
            // Only set the cycle bit if the current TRB is not the first TRB. The cycle
            // bit in the first TRB is set last, to prevent premature processing of
            // the transfer.
            //
            if (currentTrb != firstTrbInCurrentTd) {
                localTrb.Normal.CycleBit = TR_GetCycleState(isochData);
            }

            //
            // Set the target event ring for this request. The normal TRB is used for all
            // TRB types. This is safe because the interrupter target value in all of the
            // TRBs is located in the same location within the TRB structure.
            //
            localTrb.Normal.InterrupterTarget = isochData->InterrupterTarget;

            RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

            usedTrbs += 1;

            isochData->CurrentSegmentIndex += 1;
            NT_ASSERT(isochData->CurrentSegmentIndex <= isochData->MaxSegmentIndex);

            if (forceLinkTrb) {

                //
                // Prematurely insert a Link TD after programming this stage. This is to
                // workaround an issue where the controller cannot handle a Link TD pointing
                // to an Isoch TRB with a non-consecutive frame number. By inserting the Link TD now,
                // we avoid that case, because if the next Isoch TD was going to introduce
                // a gap in the stream, we will force an underrun/overrun condition first,
                // before programming the next Isoch TD. In that case, controller will see
                // the Link TD and then see no more TRBs, causing it to generate an underrun/
                // overrun event. It will only see the Isoch TRB with non-consecutive frame
                // number once software programs it after the underrun/overrun and rings the
                // doorbell.
                //
                // An optimization that can be done is to only insert the Link TD if the
                // first Isoch TD of the next stage will not fit in the rest of the TRBs of
                // the current segment.
                //
                linkInserted = Isoch_InsertLinkTrb(isochData,
                                                   StageData,
                                                   TrbStage_Normal,
                                                   FALSE);

                NT_ASSERT(linkInserted);

                forceLinkTrb = FALSE;
            }

            if ((trbStage == TrbStage_Done) || (trbStage == TrbStage_Isoch)) {

                StageData->TrbRange.LastSegmentEndIndex = isochData->CurrentSegmentIndex;

                //
                // Invalidate the CycleBit in the next unused TRB, making no assumptions
                // about the current value of its CycleBit. This is because the TransferRing
                // segment is not reinitialized when reused.
                //
                NT_ASSERT(isochData->CurrentSegmentIndex <= isochData->MaxSegmentIndex);

                //
                // Since the segment is not reinitialized when reused, ensure that the next
                // TRB is invalid by clearing its CycleBit. Be extra conservative by zero'ing
                // out the rest of the bits in the TRB due to this observation:
                //
                // Controller posted an underrun event with Pointer=0 and ED=1, presumably
                // by copying the ED bit from the invalid TRB that it happened to be pointing
                // to.
                //
                currentTrb = &isochData->CurrentSegment[isochData->CurrentSegmentIndex];
                RtlZeroMemory(currentTrb, sizeof(TRB));

                if ((trbStage == TrbStage_Done) || (isochData->RingDoorbellOnFirstTD)) {

                    TR_Lock(isochData);

                    if (trbStage == TrbStage_Done) {

                        NT_ASSERT((packetIndex - 1) == StageData->LastPacket);

                        transferData->PacketsMapped   = packetIndex - transferData->PacketsSkipped;
                        transferData->StagesMapped   += 1;
                        transferData->BytesMapped    += Isoch_GetPacketSpanLength(transferData, StageData->FirstPacket, StageData->LastPacket);
                        isochData->PendingStageCount += 1;

                        //
                        // Cache the next frame number. This value is used to determine if there are
                        // frame number gaps between two isoch URBs.
                        //
                        isochData->FrameNumberOfLastTDInSchedule            = frameNumber;
                        isochData->Flags.FrameNumberOfLastTDInScheduleValid = 1;
                    }

                    if ((trbStage == TrbStage_Done) || (isochData->RingDoorbellOnFirstTD)) {

                        isochData->Flags.TransferRingEmptyEventExpected = 1;
                        isochData->Flags.DoorbellRungSinceMappingStart  = 1;
                    }

                    TR_Unlock(isochData);
                }

                //
                // Set the cycle bit in the first TRB for the TD to start the transfer of this request.
                //
                // N.B: Do not touch transfer related data structures after making the last Isoch TD
                //      visible to the controller.
                //
                Register_ToggleTrbCycleBit(firstTrbInCurrentTd);
                firstTrbInCurrentTd = NULL;

                //
                // If this is the first isoch TD programmed into the endpoint, then
                // ring the doorbell. This happens either on the first URB request
                // or after an underrun/overrun event is processed.
                //
                if (isochData->RingDoorbellOnFirstTD) {

                    isochData->RingDoorbellOnFirstTD = FALSE;

                    UsbDevice_WriteDoorbell(Endpoint_GetUsbDeviceHandle(isochData->EndpointHandle),
                                            Endpoint_GetContextIndex(isochData->EndpointHandle),
                                            isochData->StreamId);
                }

                if (trbStage != TrbStage_Done) {

                    //
                    // Previous TD Was completed fully. Update variables needed for TRB rollback,
                    // partial stage
                    //
                    firstElemetForThisTd = currentElement;
                    if (currentElement < scatterGatherList->NumberOfElements) {
                        firstElementOffsetForThisTd =
                            (ULONG)(elementAddress - scatterGatherList->Elements[currentElement].Address.QuadPart);
                    }
                    firstRingIndexForThisTd = isochData->CurrentSegmentIndex;
                    stagingDataUsedTrbsBeforeThisTd = usedTrbs;
                }
            }

            if ((isochData->CurrentSegmentIndex == isochData->MaxSegmentIndex) &&
                (trbStage != TrbStage_Done)) {


                if (trbStage != TrbStage_Isoch) {

                    //
                    // The CurrentSegmentIndex cannot accept any more TRBs, other than a LinkTrb.
                    // If the trbStage is not TrbStage_Isoch, it means that we are in the
                    // middle of mapping a TD, we need to instead rollback and insert a
                    // link trb and then try to remap this td.
                    //

                    if (firstRingIndexForThisTd == 0) {
                        //
                        // This TD Requires too many TDs. This would happen only if client driver
                        // sent a too framanted buffer. An IsochTd cannot have a link trb in it.
                        // At present we cannot handle this case.
                        //
                        NT_ASSERTMSG("Client Buffer Too Fragmented, cannot map, skipping TD", FALSE);

                        //
                        // It is very unlikely that for Win8, we hit this code path.
                        // Thus for Win8, we are not going to provide a solution for this case.
                        // TODO: To keep things moving, maybe skip this Td. Win9Consider?
                        //
                    }

                    //
                    // Rollback to the point where we started to map this currect Td.
                    //
                    isochData->CurrentSegmentIndex = firstRingIndexForThisTd;
                    currentElement = firstElemetForThisTd;
                    elementAddress = scatterGatherList->Elements[currentElement].Address.QuadPart;
                    elementLength = scatterGatherList->Elements[currentElement].Length;
                    NT_ASSERT(firstElementOffsetForThisTd < elementLength);
                    elementLength -= firstElementOffsetForThisTd;
                    elementAddress += firstElementOffsetForThisTd;
                    usedTrbs = stagingDataUsedTrbsBeforeThisTd;
                    lengthMapped = 0;
                    packetLength = Isoch_GetPacketLength(transferData, packetIndex);

                    trbStage = TrbStage_Isoch;
                }

                //
                // Try to insert a link td now.
                //
                firstTrbInCurrentTd = &isochData->CurrentSegment[isochData->CurrentSegmentIndex];
                linkInserted = Isoch_InsertLinkTrb(isochData,
                                                   StageData,
                                                   trbStage,
                                                   TRUE);

                if (linkInserted) {

                    //
                    // Since a LinkTd has been inserted, the current ring segment got updated
                    // We dont add LinkTds in the middle of isoch transfers anyway.
                    //
                    firstRingIndexForThisTd = 0;

                } else {

                    NT_ASSERTMSG("Not implemented", 0);
                    //
                    // End this stage and record that we need another
                    // stage for this Isoch transfer.
                    //
                    //transferData->PendingPartialStage = TRUE;
                    //transferData->PendingPartialStagingData.Size = StageData->TransferSize - lengthMapped;
                    //transferData->PendingPartialStagingData.scatterGatherList = scatterGatherList;
                    //StageData->scatterGatherListShared = TRUE;
                    //transferData->PendingPartialStagingData.TransferMdl = StageData->TransferMdl;
                    //transferData->PendingPartialStagingData.FreeMdl = StageData->FreeMdl;
                    //StageData->FreeMdl = FALSE;
                    //transferData->PendingPartialStagingData.SGStartElement = currentElement;
                    //transferData->PendingPartialStagingData.SGStartElementOffset = firstElementOffsetForThisTd;
                    //transferData->PendingPartialStagingData.packetIndex = packetIndex;

                    //transferData->StageTotalCount += 1;
                    //trbStage = TrbStage_Done;
                    continue;
                }
            }
        }

        UsbDevice_WriteDoorbell(Endpoint_GetUsbDeviceHandle(isochData->EndpointHandle),
                                Endpoint_GetContextIndex(isochData->EndpointHandle),
                                isochData->StreamId);

    } FINALLY {

        ICE_STOP_TRACE();
    }

    return;
}

VOID
Isoch_EvtDmaCallback(
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
    PISOCH_DATA         isochData;
    MAPPING_STATE       previousState;
    PISOCH_STAGE_DATA   stageData;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    TRY {

        stageData = (PISOCH_STAGE_DATA)Context;
        isochData = stageData->TransferData->IsochData;

        stageData->ScatterGatherList = ScatterGatherList;

        Isoch_Stage_MapIntoRing(stageData);

    } FINALLY {

        if (InterlockedXor(&isochData->ContinueMapping, 1) == 1) {

            if (isochData->LateDetected == 0) {

                previousState = TR_AttemptStateChange(isochData, MS_Mapping, MS_Mapping);

                if (previousState == MS_Mapping) {
                    Isoch_MapTransfers(isochData);
                }

            } else {

                previousState = TR_AttemptStateChange(isochData, MS_Mapping, MS_WaitingForTransferRingEmptyEvent);

#ifndef DISABLE_RING_EMPTY_TIMER
                if (previousState == MS_Mapping) {
                    WdfTimerStart(isochData->WdfRingEmptyTimer, ISOCH_RING_EMPTY_TIMEOUT);
                }
#endif
            }
        }
    }

    return;
}

VOID
Isoch_WdfEvtRequestCancel(
    WDFREQUEST WdfRequest
    )
{
    PISOCH_DATA             isochData;
    BOOLEAN                 queueDpc;
    BOOLEAN                 reportCancel;
    PISOCH_TRANSFER_DATA    transferData;

    TRY {

        queueDpc     = FALSE;
        reportCancel = FALSE;

        transferData = &(GetRequestData(WdfRequest)->IsochTransferData);
        isochData    = transferData->IsochData;

        LogInfo(Endpoint_GetRecorderLog(isochData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p cancel requested",
                UsbDevice_GetSlotId(isochData->UsbDeviceHandle),
                Endpoint_GetContextIndex(isochData->EndpointHandle), WdfRequest);

        TR_Lock(isochData);

        if (transferData->CancelState == TCS_Cancelable) {

            if (isochData->Flags.OkToReclaimTransfersOnCancel) {
                APPEND_LIST(&isochData->CompletionTransferList, &isochData->PendingTransferList);
                queueDpc = TRUE;
            } else {
                isochData->Flags.AtleastOneTransferCanceled = 1;
                reportCancel = TRUE;
            }
        }

        TR_Unlock(isochData);

        if (reportCancel) {
            Endpoint_TR_TransferCanceled(isochData->EndpointHandle);
        }

        TR_Lock(isochData);

        if (transferData->CancelState == TCS_WaitingForCancelCallback) {

            RemoveEntryList(&transferData->ListEntry);
            InsertTailList(&isochData->CompletionTransferList, &transferData->ListEntry);

            queueDpc = TRUE;
        }

        transferData->CancelState = TCS_Canceled;

        TR_Unlock(isochData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(isochData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Isoch_RetrieveNextStage(
    __in
        PISOCH_DATA IsochData
    )
/*++

Routine Description:

    Finds the next stage of a transfer to process and saves it in the IsochData->CurrentStage
    on success.

    Reasons for failure:

        1. Limit for outstanding stages per transferring is reached.
        2. The last pending transfer has more stages to program, but has reached the limit of
           outstanding stages per transfer.
        3. There are no new transfers available to program stages for.

    Reasons for success:

        1. There is already a stage to be programmed. This stage was pended previously due to
           reasons like waiting for TransferRing empty event, waiting for CommonBuffer etc.
        2. The last pending transfer has more stages to program and has not reached the limit
           of outstanding stages per transfer.
        3. A new transfer became available.

--*/
{
    BOOLEAN                 result;
    NTSTATUS                status;
    PISOCH_TRANSFER_DATA    transferData;
    WDFREQUEST              wdfRequest;

    TRY {

        status                  = STATUS_UNSUCCESSFUL;
        IsochData->CurrentStage = NULL;

        TR_Lock(IsochData);

        if (IsochData->PendingStageCount == ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING) {

            LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                       "%u.%u.0: Pending stage limit %u reached, not mapping more stages",
                       UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(IsochData->EndpointHandle),
                       ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING);

            TR_Unlock(IsochData);
            LEAVE;
        }

        //
        // Last transfer on the PendingTransferList may have more stages to map.
        //
        if (IsListEmpty(&IsochData->PendingTransferList) == FALSE) {

            transferData = (PISOCH_TRANSFER_DATA)IsochData->PendingTransferList.Blink;

            //
            // For a given stage, CommonBuffer acquisition is attempted multiple times on failure.
            // After the limit is reached, the stage is considered done, and if there are no more
            // outstanding stages, the transfer is completed.
            //
            if (IsochData->CommonBufferAcquireAttemptCount == ISOCH_MAX_COMMONBUFFER_ACQUIRE_ATTEMPTS) {

                if (transferData->StagesCompleted == transferData->StagesMapped) {

                    LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                               "%u.%u.0: Status USBD_STATUS_INSUFFICIENT_RESOURCES due to ISOCH_MAX_COMMONBUFFER_ACQUIRE_ATTEMPTS",
                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(IsochData->EndpointHandle));

                    Isoch_Transfer_CompleteCancelable(IsochData,
                                                      transferData,
                                                      USBD_STATUS_NOT_SET,
                                                      USBD_STATUS_INSUFFICIENT_RESOURCES,
                                                      FALSE,
                                                      FALSE);

                } else {

                    transferData->PacketsSkipped +=
                        (transferData->PacketsTotal -
                         (transferData->PacketsMapped + transferData->PacketsSkipped));

                    TR_Unlock(IsochData);
                    LEAVE;
                }

            } else {

                //
                // Check if the transfer has more packets (stages) to map.
                //
                if ((transferData->PacketsMapped + transferData->PacketsSkipped) < transferData->PacketsTotal) {

                    //
                    // When the transfer is first received, the StartFrame is determined and packets are
                    // marked late. Subsequently, every time a stage is programmed, we scan through the
                    // packets that are left to map, and mark them late if needed.
                    //
                    Isoch_TransferUrb_MarkLatePackets(IsochData, transferData);

                    if (transferData->TransferUrb->Isoch.IsochPacket[transferData->PacketsTotal - 1].Status ==
                        USBD_STATUS_ISO_NOT_ACCESSED_LATE) {

                        if (transferData->StagesCompleted == transferData->StagesMapped) {

                            //
                            // COMPAT: The 2.0 Stack does not return any error status in this case.
                            // Clear of the error status from the IsochPackets.
                            //
                            LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                       "%u.%u.0: USBD_STATUS_ISO_NOT_ACCESSED_LATE: Calling Isoch_Transfer_CompleteCancelable with STATUS_SUCCESS",
                                       UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                       Endpoint_GetContextIndex(IsochData->EndpointHandle));

                            Isoch_Transfer_CompleteCancelable(IsochData,
                                                              transferData,
                                                              USBD_STATUS_SUCCESS,
                                                              USBD_STATUS_SUCCESS,
                                                              TRUE,
                                                              TRUE);

                        } else {

                            transferData->PacketsSkipped +=
                                (transferData->PacketsTotal -
                                 (transferData->PacketsMapped + transferData->PacketsSkipped));

                            TR_Unlock(IsochData);
                            LEAVE;
                        }

                    } else {

                        IsochData->CurrentStage = Isoch_Stage_Acquire(transferData);

                        if (IsochData->CurrentStage != NULL) {

                            LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                    "%u.%u.0: WdfRequest 0x%p, start mapping next stage 0x%p",
                                    UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                    Endpoint_GetContextIndex(IsochData->EndpointHandle),
                                    transferData->WdfRequest, IsochData->CurrentStage);

                            status = STATUS_SUCCESS;

                        } else {

                            //
                            // There are more stages to program for this transfer, but no free stage
                            // data structures available at the moment.
                            //
                            LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                                       "%u.%u.0: WdfRequest 0x%p, no free stages currently available to map next stage",
                                       UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                                       Endpoint_GetContextIndex(IsochData->EndpointHandle), transferData->WdfRequest);
                        }

                        TR_Unlock(IsochData);
                        LEAVE;
                    }
                }
            }
        }

        TR_Unlock(IsochData);

        WHILE (TRUE) {

            status = WdfIoQueueRetrieveNextRequest(IsochData->WdfQueue, &wdfRequest);

            if (!NT_SUCCESS(status)) {

                LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                           "%u.%u.0: WdfIoQueueRetrieveNextRequest %!STATUS!",
                           UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(IsochData->EndpointHandle), status);

                if (IsochData->PendingStageCount == 0) {

                    LogWarning(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                               "%u.%u.0: Possible starvation of Isoch stream - no outstanding URBs",
                               UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(IsochData->EndpointHandle));
                }

                LEAVE;
            }

            status = Isoch_TransferData_Initialize(IsochData, wdfRequest, &transferData);

            if (!NT_SUCCESS(status)) {

                LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                         "%u.%u.0: WdfRequest 0x%p, Isoch_TransferData_Initialize failed %!STATUS!",
                         UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(IsochData->EndpointHandle), wdfRequest, status);

                Isoch_Transfer_Complete(IsochData,
                                        transferData,
                                        USBD_STATUS_NOT_SET,
                                        USBD_STATUS_INSUFFICIENT_RESOURCES,
                                        FALSE);
                continue;
            }

            status = Isoch_TransferUrb_Initialize(IsochData, transferData);

            if (!NT_SUCCESS(status)) {

                LogError(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequest 0x%p, StartFrame is out of frame range. status %!STATUS!",
                        UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(IsochData->EndpointHandle), wdfRequest, status);

                Isoch_Transfer_Complete(IsochData,
                                        transferData,
                                        USBD_STATUS_ISO_NOT_ACCESSED_LATE,
                                        USBD_STATUS_BAD_START_FRAME,
                                        FALSE);
                continue;
            }

            Isoch_TransferUrb_MarkLatePackets(IsochData, transferData);

            NT_ASSERT(transferData->PacketsTotal >= 1);
            if (transferData->TransferUrb->Isoch.IsochPacket[transferData->PacketsTotal - 1].Status ==
                USBD_STATUS_ISO_NOT_ACCESSED_LATE) {

                //
                // COMPAT: The 2.0 Stack does not return any error status in this case.
                // Clear of the error status from the IsochPackets.
                LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                           "%u.%u.0: USBD_STATUS_ISO_NOT_ACCESSED_LATE: Calling Isoch_Transfer_Complete with STATUS_SUCCESS",
                           UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(IsochData->EndpointHandle));

                //
                Isoch_Transfer_Complete(IsochData,
                                        transferData,
                                        USBD_STATUS_SUCCESS,
                                        USBD_STATUS_SUCCESS,
                                        TRUE);
                continue;
            }

            if (transferData->BytesTotal == 0) {

                //
                // COMPAT: 2.0 Stack will fail the Isoch request asynchronously if the
                // TransferBufferLength specified is 0.
                //
                LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequest 0x%p, Failing 0-length Isoch Transfer.",
                        UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(IsochData->EndpointHandle), wdfRequest);

                Isoch_Transfer_Complete(IsochData,
                                        transferData,
                                        USBD_STATUS_NOT_SET,
                                        USBD_STATUS_NOT_SET,
                                        TRUE);
                continue;
            }

            transferData->CancelState = TCS_Cancelable;

            status = WdfRequestMarkCancelableEx(transferData->WdfRequest, Isoch_WdfEvtRequestCancel);

            if (!NT_SUCCESS(status)) {

                LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                        "%u.%u.0: USBD_STATUS_CANCELED: WdfRequest 0x%p WdfRequestMarkCancelableEx failed %!STATUS!",
                        UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(IsochData->EndpointHandle),
                        transferData->WdfRequest, status);

                transferData->CancelState = TCS_Canceled;

                Isoch_Transfer_Complete(IsochData,
                                        transferData,
                                        USBD_STATUS_NOT_SET,
                                        USBD_STATUS_CANCELED,
                                        FALSE);

                continue;
            }

            break;
        }

        IsochData->CurrentStage = Isoch_Stage_Acquire(transferData);

        //
        // Refresh the InterrupterTarget if there are no pending transfers in hardware.
        //
        if (IsochData->PendingStageCount == 0) {

            IsochData->InterrupterTarget =
                Interrupter_GetInterrupterTarget(
                    Controller_GetInterrupterHandle(IsochData->ControllerHandle),
                    transferData->TransferUrb->UrbData.ProcessorNumber);
        }

        LogInfo(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p, start mapping first stage 0x%p",
                UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                Endpoint_GetContextIndex(IsochData->EndpointHandle),
                transferData->WdfRequest, IsochData->CurrentStage);

        TR_Lock(IsochData);
        InsertTailList(&IsochData->PendingTransferList, &transferData->ListEntry);
        TR_Unlock(IsochData);


    } FINALLY {

        if (NT_SUCCESS(status)) {
            NT_ASSERT(IsochData->CurrentStage != NULL);
            result = TRUE;
        } else {
            NT_ASSERT(IsochData->CurrentStage == NULL);
            result = FALSE;
        }
    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_MappingLoop(
    __in
        PISOCH_DATA IsochData
    )
{
    MAP_STAGE_RESULT        mapStageResult;
    MAPPING_STATE           previousState;
    PROCESS_STAGE_RESULT    processStageResult;
    ULONG                   iteration;

    for (iteration = 0; NOTHING; iteration += 1) {

        LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                   "%u.%u.0: Mapping Loop iteration %u",
                   UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(IsochData->EndpointHandle),
                   iteration);

        IsochData->AttemptMapping                 = 0;
        IsochData->TransferRingEmptyEventReceived = 0;

        //
        // Step 1: Find the next stage to map.
        //
        if (Isoch_RetrieveNextStage(IsochData) == FALSE) {

            previousState = TR_AttemptStateChange(IsochData, MS_Mapping, MS_Paused);
            break;
        }

        //
        // Step 2: Prepare the stage to map.
        //
        processStageResult = Isoch_PrepareStage(IsochData);

        if (processStageResult == PSR_TransferCompletedPrematurely) {
            continue;
        }

        if (processStageResult == PSR_WaitingOnCommonBuffer) {
            break;
        }

        if (processStageResult == PSR_GapDetected) {

            previousState = TR_AttemptStateChange(IsochData, MS_Mapping, MS_WaitingForTransferRingEmptyEvent);
#ifndef DISABLE_RING_EMPTY_TIMER
            if (previousState == MS_Mapping) {
                WdfTimerStart(IsochData->WdfRingEmptyTimer, ISOCH_RING_EMPTY_TIMEOUT);
            }
#endif
            break;
        }

        if (processStageResult == PSR_ResourceAllocationFailed) {

            previousState = TR_AttemptStateChange(IsochData, MS_Mapping, MS_Paused);
            break;
        }

        //
        // Step 3: Map the stage.
        //
        mapStageResult = Isoch_MapStage(IsochData);

        if (mapStageResult == MSR_WaitingOnDmaCallback) {
            break;
        }

        if (mapStageResult == MSR_LateDetected) {

            previousState = TR_AttemptStateChange(IsochData, MS_Mapping, MS_WaitingForTransferRingEmptyEvent);
#ifndef DISABLE_RING_EMPTY_TIMER
            if (previousState == MS_Mapping) {
                WdfTimerStart(IsochData->WdfRingEmptyTimer, ISOCH_RING_EMPTY_TIMEOUT);
            }
#endif
            break;
        }

        previousState = TR_AttemptStateChange(IsochData, MS_Mapping, MS_Mapping);

        if (previousState != MS_Mapping) {

            NT_ASSERT(previousState == MS_Stopping);
            break;
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_MapTransfers(
    __in
        PISOCH_DATA IsochData
    )
{
    MAPPING_STATE previousState;
    ULONG         iteration;

    for (iteration = 0; NOTHING; iteration += 1) {

        LogVerbose(Endpoint_GetRecorderLog(IsochData->EndpointHandle), TransferRing,
                   "%u.%u.0: Outer Mapping Loop iteration %u",
                   UsbDevice_GetSlotId(IsochData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(IsochData->EndpointHandle),
                   iteration);

        Isoch_MappingLoop(IsochData);

        if (IsochData->MappingState == MS_Paused) {

            if (IsochData->AttemptMapping == 0) {
                break;
            }

            previousState = TR_AttemptStateChange(IsochData, MS_Paused, MS_Mapping);

            if (previousState != MS_Paused) {
                break;
            }

        } else if (IsochData->MappingState == MS_WaitingForTransferRingEmptyEvent) {

            if (IsochData->TransferRingEmptyEventReceived == 0) {
                break;
            }

            previousState = TR_AttemptStateChange(IsochData, MS_WaitingForTransferRingEmptyEvent, MS_Mapping);

#ifndef DISABLE_RING_EMPTY_TIMER
            if (previousState == MS_WaitingForTransferRingEmptyEvent) {
                WdfTimerStop(IsochData->WdfRingEmptyTimer, FALSE);
            }
#endif

            if (previousState != MS_WaitingForTransferRingEmptyEvent) {
                break;
            }

        } else {
            break;
        }
    }
}

ULONG
Isoch_CalculatePacketsPerFrame(
    __in
        PISOCH_DATA IsochData
    )
{
    ULONG packetsPerFrame;
    ULONG periodIn125usecs;

    switch (UsbDevice_GetDeviceSpeed(IsochData->UsbDeviceHandle)) {

    case UsbFullSpeed:

        //
        // Full Speed devices have a period of 1 ms.
        //
        packetsPerFrame = 1;
        break;

    case UsbHighSpeed:
    case UsbSuperSpeed:

        periodIn125usecs = 1 << (Endpoint_GetInterval(IsochData->EndpointHandle) - 1);

        if (periodIn125usecs > 8) {
            periodIn125usecs = 8;
        }

        packetsPerFrame = 8 / periodIn125usecs;
        break;

    case UsbLowSpeed:
    default:
        NT_ASSERT(FALSE);
        packetsPerFrame = 1;
        break;
    }

    return packetsPerFrame;
}

ULONG
Isoch_GetFrameCount(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       PacketCount
    )
{
    return ((PacketCount + (IsochData->PacketsPerFrame - 1)) / IsochData->PacketsPerFrame);
}

ULONG
Iosch_GetPacketFrameNumber(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       StartFrame,
    __in
        ULONG       PacketIndex
    )
{
    return (StartFrame + (PacketIndex / IsochData->PacketsPerFrame));
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTDPC(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       TDLength
    )
/*++

Routine Description:

    This function returns the Transfer Descriptor Packet Count. This value is defined
    in xHCI 1.0 spec, section 4.14.1:

        The Transfer Descriptor Packet Count (TDPC) is the number of packets
        required to move all the data defined by a TD. Note that a partial or
        a zero-length packet increments this count by 1.

--*/
{
    ULONG tdpc;
    ULONG maxPacketSize;

    maxPacketSize = Endpoint_GetMaxPacketSize(IsochData->EndpointHandle);
    tdpc          = (TDLength + (maxPacketSize - 1)) / maxPacketSize;

    if (tdpc == 0) {
        tdpc = 1;
    }

    return tdpc;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTBC(
    __in
        PISOCH_DATA         IsochData,
    __in
        ULONG               TDLength
    )
/*++

Routine Description:

    Returns the Transfer Burst Count (TBC). This calculation is taken from the
    xHCI 1.0 spec, section 4.11.2.3:

        TBC = ROUNDUP ( TDPC / ( Max Burst Size + 1 ) ) - 1

--*/
{
    ULONG tdpc;
    ULONG tbc;
    ULONG maxBurstSize;

    maxBurstSize = Endpoint_GetMaxBurstSize(IsochData->EndpointHandle);
    tdpc         = Isoch_GetTDPC(IsochData, TDLength);
    tbc          = ((tdpc + maxBurstSize) / (maxBurstSize + 1)) - 1;

    return tbc;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTLBPC(
    __in
        PISOCH_DATA         IsochData,
    __in
        ULONG               TDLength
    )
/*++

Routine Description:

    Returns the Transfer Last Burst Packet Count (TLBPC). This calculation is taken
    from the xHCI 1.0 spec, section 4.11.2.3:

        IsochBurstResiduePackets = TDPC MODULUS ( Max Burst Size + 1 )

        TLBPC = IF ( IsochBurstResiduePackets == 0 )
                    THEN Max Burst Size - 1
                    ELSE IsochBurstResiduePackets - 1

--*/
{
    ULONG isochBurstResiduePackets;
    ULONG maxBurstSize;

    maxBurstSize = Endpoint_GetMaxBurstSize(IsochData->EndpointHandle);

    isochBurstResiduePackets = Isoch_GetTDPC(IsochData, TDLength) % (maxBurstSize + 1);

    if (isochBurstResiduePackets == 0) {
        return maxBurstSize;
    } else {
        return isochBurstResiduePackets -1;
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetPacketLength(
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        ULONG                   PacketIndex
    )
/*++

Routine Description:

    Returns the length of the buffer addressed by the Isoch Packet with the
    supplied PacketIndex.

--*/
{
    ULONG                       packetLength;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;

    isochPacketArray = TransferData->TransferUrb->Isoch.IsochPacket;

    if (PacketIndex == (TransferData->PacketsTotal - 1)) {
        packetLength = TransferData->BytesTotal - isochPacketArray[PacketIndex].Offset;
    } else {
        packetLength = isochPacketArray[PacketIndex+1].Offset - isochPacketArray[PacketIndex].Offset;
    }

    return packetLength;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetPacketSpanLength(
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        ULONG                   FirstPacket,
    __in
        ULONG                   LastPacket
    )
/*++

Routine Description:

    Returns the length of the buffer addressed by the supplied First and Last
    Isoch Packets, inclusive.

--*/
{
    ULONG                       packetSpanLength;
    PUSBD_ISO_PACKET_DESCRIPTOR isochPacketArray;

    isochPacketArray = TransferData->TransferUrb->Isoch.IsochPacket;

    if (LastPacket == (TransferData->PacketsTotal - 1)) {

        packetSpanLength =
            TransferData->BytesTotal - isochPacketArray[FirstPacket].Offset;

    } else {

        packetSpanLength =
            isochPacketArray[LastPacket + 1].Offset - isochPacketArray[FirstPacket].Offset;
    }

    return packetSpanLength;
}
