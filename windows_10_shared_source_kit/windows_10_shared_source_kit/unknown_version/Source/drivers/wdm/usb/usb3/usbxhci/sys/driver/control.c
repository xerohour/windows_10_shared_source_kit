
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Control.c

Abstract:

    Compared to WIN8, Control TransferRing implementation has been separated out from other
    endpoint types.

    ---------------------------------------------------------------------
    Differences in Control TransferRing compared to other endpoint types:
    ---------------------------------------------------------------------

    1. Endpoint has to be reset right away if a halt is received:
       If a transfer fails and stalls the endpoint, we have to clear the halt by resetting the
       endpoint, before completing the transfer. This is in contrast to other endpoint types,
       where the transfer is failed and then we wait on the client driver to cancel outstanding
       transfers and request an endpoint reset.

    2. Only one stage per transfer:
       In theory, a Control transfer may have to be broken up into multiple stages if the
       transfer size is greater than the number of map registers.  But the WIN8 implementation
       did not handle this, and since this is an edge case, we are just going to detect this
       case and fail the transfer if it is too big to fit in one stage (which is better than
       the WIN8 implementation where the behavior was undefined).

    3. Only one outstanding transfer at a time. This implies some simplifications:
       - Ring comprises a single circular segment. This eliminates the need to calculate the
         number of TRBs required for a transfer, and then allocating the required segments, and
         waiting on any allocation that needs to be done at PASSIVE.
       - Transfers don't have to be requed.
       - Less synchronization needed between mapping and transfer completion code.

    4. No Chained MDL support. That is, there is no URB_FUNCTION_CONTROL_TRANSFER_USING_CHAINED_MDL
       defined.


    ------------------------------------------------
    Functional changes from the WIN8 implementation:
    ------------------------------------------------

    1. Sequential queue instead of manual. Since Control transfers are processed and completed
       one at a time.

    2. If reset fails, we don't complete the transfer. Instead, we wait for the purge to come
       in, in preparation for controller reset. This is similar to how cancel/stop failure works.

    3. If client driver issues an endpoint reset, and the Control endpoint happens to be halted,
       the endpoint object will start the reset process. But the new Control TransferRing object
       will ignore the result of that (i.e. TR_ClearTrbProcessed callback is not implemented).








    --------------------------------------------------
    Unnatural hardware behavior that is worked around:
    --------------------------------------------------

    1. A Set Address Command failure could have halted the control endpoint. A subsequent
       Control transfer would timeout and trigger cancellation. The resulting stop endpoint
       command would fail with Context State Error because the endpoint is halted. So this
       becomes a case of the endpoint getting halted without the Control transfer actually
       failing with a halted completion code. The State machine handles this by handling the
       Context State error completion of the Stop Endpoint Command, then waiting for a halted
       completion code, finally timing out and resetting the endpoint.






--*/

#include "pch.h"
#include "Control.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Control_GetConfiguration)
#pragma alloc_text(PAGE, Control_Initialize)
#endif

CONST TR_FUNCTION_TABLE ControlFunctionTable = {

    Control_Initialize,
    Control_Cleanup,
    NULL,                                           // TR_CommonBufferCallback
    Control_EP_Enable,
    Control_EP_Disable,
    Control_EP_EnableForwardProgress,
    Control_EP_AreTransfersPending,
    Control_EP_OkToReclaimTransfersOnCancel,
    Control_EP_ReclaimTransfers,
    NULL,                                           // EP_IsTransferEventLikelyDuplicate
    Control_EP_TransferEventHandler,
    Control_EP_StartMapping,
    Control_EP_StopMapping,
    Control_EP_IsMappingStoppedOrPaused,
    TR_EmptyFunction,                               // EP_EndpointHalted
    TR_EmptyFunction,                               // EP_EndpointResetFromClient
    Control_EP_ProcessExpectedEventTRBs,
    TR_EmptyFunction,                               // EP_FSEReceived
    Control_EP_WasDoorbellRungSinceMappingStart,
    Control_WdfEvtIoDefault,
    Control_WdfEvtIoCanceledOnQueue,
    NULL,                                           // WDF_EvtIoQueueReadyNotification
    Control_WdfEvtDpcForTransferCompletion

};

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Control_GetConfiguration(
    __in
        HANDLE      ControllerHandle,
    __out
        PTR_CONFIG  Config
    )
{
    PAGED_CODE();

    Config->QueueDispatchType = WdfIoQueueDispatchSequential;
    Config->ContextSize       = sizeof(CONTROL_DATA);
    Config->DoubleBufferSize  = SMALL_SEGMENT_SIZE;
    Config->FunctionTable     = &ControlFunctionTable;

    if (Controller_GetDeviceFlags(ControllerHandle).NoLinkTRBInTD) {
        Config->SegmentSize = LARGE_SEGMENT_SIZE;
    } else {
        Config->SegmentSize = SMALL_SEGMENT_SIZE;
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Control_Initialize(
    __in
        HANDLE Context
    )
{
    NTSTATUS                status;
    PCONTROL_DATA           controlData;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDF_DPC_CONFIG          wdfDpcConfig;
    WDF_TIMER_CONFIG        wdfTimerConfig;

    PAGED_CODE();

    TRY {

        controlData = (PCONTROL_DATA)Context;

        InitializeListHead(&controlData->CanceledOnQueueTransferList);

        //
        // Table 73: IDT shall not be set ('1') for TRBs on endpoints that define a Max
        // Packet Size < 8 bytes, or on IN endpoints.
        //
        if ((Controller_GetDeviceFlags(controlData->ControllerHandle).DisableImmediateData) ||
            (Endpoint_GetMaxPacketSize(controlData->EndpointHandle) < IMMEDIATE_DATA_LENGTH)) {

            controlData->ImmediateDataEnabled = FALSE;

        } else {

            controlData->ImmediateDataEnabled = TRUE;
        }

        //
        // Timer for Control Transfer timeout.
        //
        WDF_TIMER_CONFIG_INIT(&wdfTimerConfig, Control_WdfEvtTimerForTransferTimeout);

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = controlData->WdfQueue;

        status = WdfTimerCreate(&wdfTimerConfig, &wdfAttributes, &controlData->WdfTimerForTransferTimeout);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(controlData->ControllerHandle), TransferRing,
                     "%u.%u.0: WdfTimerCreate (WdfTimerForTransferTimeout) Failed %!STATUS!",
                     UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(controlData->EndpointHandle), status);
            LEAVE;
        }

        //
        // DPC used for completion of cancelled on queue transfers.
        //
        WDF_DPC_CONFIG_INIT(&wdfDpcConfig, Control_WdfDpcForCanceledOnQueueTransferCompletion);

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = controlData->WdfQueue;

        status = WdfDpcCreate(&wdfDpcConfig, &wdfAttributes, &controlData->WdfDpcForCanceledOnQueueTransferCompletion);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(controlData->ControllerHandle), TransferRing,
                     "%u.%u.0: WdfDpcCreate Failed %!STATUS!",
                     UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(controlData->EndpointHandle), status);
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_Cleanup(
    __in
        HANDLE Context
    )
{
    PCONTROL_DATA controlData;

    controlData = (PCONTROL_DATA)Context;

    if (controlData->TransferData != NULL) {
        NT_ASSERTMSG("Failed to complete control transfer", (controlData->TransferData == NULL));
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Control_EP_Enable(
    __in
        HANDLE Context
    )
{
    UNREFERENCED_PARAMETER(Context);

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_Disable(
    __in
        HANDLE    Context,
    __in
        BOOLEAN   FreeResources
    )
{
    PCONTROL_DATA   controlData;
    MAPPING_STATE   mappingState;

    UNREFERENCED_PARAMETER(FreeResources);

    controlData = (PCONTROL_DATA)Context;

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

        mappingState = *((volatile MAPPING_STATE*)&controlData->MappingState);

        if ((mappingState != MS_Mapping) && (mappingState != MS_Stopping)) {
            break;
        }
    }

    TR_Lock(controlData);
    if (mappingState != MS_Stopped) {

        //
        // If mapping state is MS_Paused, change it to MS_Stopped so that
        // if the endpoint is subsequently enabled, the mapping state is
        // stopped.
        //
        NT_ASSERT(mappingState == MS_Paused);
        controlData->MappingState = MS_Stopped;
    }
    TR_Unlock(controlData);

    NT_ASSERTMSG("Failed to complete control transfer", (controlData->TransferData == NULL));

    NT_ASSERT(IsListEmpty(&controlData->CanceledOnQueueTransferList));

    NT_ASSERT(Control_EP_IsMappingStoppedOrPaused(controlData) != FALSE);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Control_EP_EnableForwardProgress(
    __in
        HANDLE    Context,
    __in
        ULONG     MaxTransferSize
    )
{
    PCONTROL_DATA   controlData;
    ULONG           segmentsRequired;
    NTSTATUS        status;
    ULONG           trbsPerSegment;
    ULONG           trbsRequired;

    TRY {

        controlData = (PCONTROL_DATA)Context;

        //
        // Control endpoint is hardcoded to have one segment. This logic is
        // only here to catch unintentional changes in future.
        //
        trbsRequired = 1 +                                      // Setup Stage TRB.
                       BufferSizeToSpanPages(MaxTransferSize) + // Data Stage Normal TRBs.
                       1 +                                      // Data Stage Event Data TRB.
                       1 +                                      // Status Stage TRB.
                       1 +                                      // Event Data TRB.
                       1;                                       // Link TRB.

        trbsPerSegment   = controlData->SegmentSize / sizeof(TRB);
        segmentsRequired = 1;

        NT_ASSERT(trbsRequired <= (segmentsRequired * trbsPerSegment));

        if (trbsRequired > (segmentsRequired * trbsPerSegment)) {
            NT_ASSERT(FALSE);
        }

        //
        // Disable Doublebuffering since it requires allocating CommonBuffer at runtime.
        //
        controlData->DoubleBufferSize = 0;

        //
        // WIN8 behavior: Not clear why ImmediateData needs to be disabled.
        //
        controlData->ImmediateDataEnabled = FALSE;

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_EP_AreTransfersPending(
    __in
        HANDLE Context
    )
{
    PCONTROL_DATA           controlData;
    PCONTROL_TRANSFER_DATA  transferData;

    controlData = (PCONTROL_DATA)Context;

    TR_Lock(controlData);
    transferData = controlData->TransferData;
    TR_Unlock(controlData);

    return (transferData != NULL);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_OkToReclaimTransfersOnCancel(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to go ahead and complete
    transfers that have already been canceled, or get canceled/timeout in
    future.

    State machine ensures that mapping is currently stopped, and that no
    Interrupt Dpc is currently processing completion of transfers.

--*/
{
    PCONTROL_DATA           controlData;
    BOOLEAN                 queueDpc;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        queueDpc    = FALSE;
        controlData = (PCONTROL_DATA)Context;

        TR_Lock(controlData);

        transferData = controlData->TransferData;

        if (transferData) {

            if (transferData->CancelState == TCS_Canceled) {
                queueDpc = TRUE;
            }
        }

        //
        // Unconditionally set this bit, because once the current transfer is
        // completed, new transfers may get delivered to EvtIoDefault, which
        // we will have to hang on to until they are canceled.
        //
        controlData->Flags.OkToReclaimTransfersOnCancel = 1;

        TR_Unlock(controlData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(controlData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_ReclaimTransfers(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to complete or requeue all
    driver-owned transfer requests. In case of Control TransferRing, the only
    pending transfer, if it exists, should be either canceled or timed out.
    So we just go ahead and complete it.

    State machine ensures that mapping is currently stopped, and that no
    Interrupt Dpc is currently processing completion of transfers.

--*/
{
    PCONTROL_DATA           controlData;
    BOOLEAN                 queueDpc;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        queueDpc    = FALSE;
        controlData = (PCONTROL_DATA)Context;

        TR_Lock(controlData);

        transferData = controlData->TransferData;

        if (transferData) {

            queueDpc = TRUE;
            controlData->Flags.ReclaimTransfersAndAcknowledge = 1;
        }

        TR_Unlock(controlData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(controlData);
        } else {
            TR_TransfersReclaimed(controlData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_StartMapping(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine that it is ok to start
    mapping transfers when EvtIoDefault gets called. If EvtIoDefault was
    already called in the past, TransferData will be set. In that case,
    start mapping that transfer right away.

--*/
{
    PCONTROL_DATA           controlData;
    BOOLEAN                 processTransfer;
    BOOLEAN                 queueDpc;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        queueDpc        = FALSE;
        processTransfer = FALSE;
        controlData     = (PCONTROL_DATA)Context;

        TR_Lock(controlData);

        controlData->Flags.AsUlong32                = 0;
        controlData->OutstandingEventCountAfterStop = 0;

        NT_ASSERT(controlData->MappingState == MS_Stopped);

        transferData = controlData->TransferData;

        //
        // EvtIoDefault call has no arrived yet, i.e. there are no transfers to map.
        //
        if (transferData == NULL) {

            controlData->MappingState = MS_Paused;

            TR_Unlock(controlData);
            LEAVE;
        }

        //
        // We have a transfer to map, but it got canceled before this StartMapping call.
        // If we don't check and complete it here, it may never get completed because
        // we think we informed the ESM about the cancel, but that was before the
        // StartMapping call, in which case ESM would have dropped that event.
        //
        if (transferData->CancelState == TCS_Canceled) {

            controlData->MappingState = MS_Paused;
            queueDpc = TRUE;

            TR_Unlock(controlData);
            LEAVE;
        }

        controlData->MappingState = MS_Mapping;
        processTransfer = TRUE;

        TR_Unlock(controlData);

    } FINALLY {

        if (processTransfer) {
            Control_MapTransfer(controlData);
        }

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(controlData);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_StopMapping(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    This is the directive from the State Machine to stop mapping transfers.
    If mapping is paused, stop of mapping is acknowledged right away. Otherwise,
    the DMA mapping code will transition to stopped state and acknowledge the
    stop after it is done mapping.

--*/
{
    BOOLEAN         acknowledgeStop;
    PCONTROL_DATA   controlData;

    TRY {

        controlData = (PCONTROL_DATA)Context;

        TR_Lock(controlData);

        if (controlData->MappingState == MS_Mapping) {
            controlData->MappingState = MS_Stopping;
            acknowledgeStop = FALSE;
        } else {
            NT_ASSERT(controlData->MappingState == MS_Paused);
            controlData->MappingState = MS_Stopped;
            acknowledgeStop = TRUE;
        }

        TR_Unlock(controlData);

    } FINALLY {

        if (acknowledgeStop) {
            Endpoint_TR_MappingStopped(controlData->EndpointHandle);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_EP_IsMappingStoppedOrPaused(
    __in
        HANDLE Context
    )
{
    PCONTROL_DATA controlData;

    controlData = (PCONTROL_DATA)Context;

    return ((controlData->MappingState == MS_Paused) ||
            (controlData->MappingState == MS_Stopped));
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_EP_ProcessExpectedEventTRBs(
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

--*/
{
    PCONTROL_DATA controlData;
    BOOLEAN       acknowledgeExpectedEventTRBsProcessed;

    TRY {

        controlData = (PCONTROL_DATA)Context;

        TR_Lock(controlData);

        //
        // When the FSE was received, OutstandingEventCountAfterStop would have
        // been set to a non-zero value if there were outstanding Event TRBs. Then,
        // it would have gotten decremented to 0 if all Event TRBs got processed.
        //
        if (controlData->OutstandingEventCountAfterStop == 0) {
            acknowledgeExpectedEventTRBsProcessed = TRUE;
        } else {
            acknowledgeExpectedEventTRBsProcessed = FALSE;
            controlData->Flags.AcknowledgeExpectedEventTRBsProcessed = 1;
        }

        TR_Unlock(controlData);

    } FINALLY {

        if (acknowledgeExpectedEventTRBsProcessed) {
            Endpoint_TR_ExpectedEventTRBsProcessed(controlData->EndpointHandle);
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_EP_WasDoorbellRungSinceMappingStart(
    __in
        HANDLE Context
    )
/*++

Routine Description:

    Returns the value of DoorbellRungSinceMappingStart. Right before the mapping
    was started, this bit was cleared to 0. Then, it gets set every time the
    doorbell is rung.

    N.B: This function is expected to get called only when mapping is stopped.

--*/
{
    PCONTROL_DATA   controlData;
    ULONG           doorbellRungSinceMappingStart;

    controlData = (PCONTROL_DATA)Context;

    NT_ASSERT(controlData->MappingState == MS_Stopped);

    TR_Lock(controlData);

    doorbellRungSinceMappingStart = controlData->Flags.DoorbellRungSinceMappingStart;

    TR_Unlock(controlData);

    return (doorbellRungSinceMappingStart == 1);
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_FindTrbMatch(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in
        PBUFFER_DATA        Segment,
    __in
        ULONG               StartIndex,
    __in
        ULONG               EndIndex,
    __inout
        PULONG              SkippedTDCount,
    __inout
        PULONG              BytesTransferred
    )
/*++

Routine Description:

    Given a range of TRBs, finds the TRB that the Transfer Event TRB points to.

    Side effects:
    - Updates the number of TDs skipped between the StartIndex and the matching TRB.
    - Calculates the bytes transferred for the TD that has the matching TRB.

--*/
{
    BOOLEAN             trbMatch;
    PTRB                lastTrbVA;
    PTRB                currentTrbVA;
    PHYSICAL_ADDRESS    currentTrbLA;

    TRY {

        trbMatch              = FALSE;
        currentTrbVA          = (PTRB)(Segment->VirtualAddress + (StartIndex * sizeof(TRB)));
        lastTrbVA             = (PTRB)(Segment->VirtualAddress + (EndIndex * sizeof(TRB)));
        currentTrbLA.QuadPart = Segment->LogicalAddress.QuadPart + (StartIndex * sizeof(TRB));

        while ((currentTrbVA <= lastTrbVA) && (trbMatch == FALSE)) {

            if (currentTrbLA.QuadPart == Trb->Pointer.QuadPart) {
                trbMatch = TRUE;
            }

            switch (currentTrbVA->Normal.Type) {

            case TRB_TYPE_NORMAL:
            case TRB_TYPE_DATA_STAGE:

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
                    *SkippedTDCount  += 1;
                }
                break;

            case TRB_TYPE_SETUP_STAGE:
            case TRB_TYPE_STATUS_STAGE:
            case TRB_TYPE_LINK:
                break;

            case TRB_TYPE_RESERVED:
                NT_ASSERT(currentTrbVA == lastTrbVA);
                break;

            default:
                NT_ASSERTMSG("Unexpected TRB Type encountered while scanning Control TRB segment", 0);
                break;
            }

            currentTrbVA += 1;
            currentTrbLA.QuadPart += sizeof(TRB);
        }

    } FINALLY {

    }

    return trbMatch;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Control_ValidateED0TrbPointerOnMismatch(
    __in
        PCONTROL_DATA       ControlData,
    __in
        PTRANSFER_EVENT_TRB Trb
    )
/*++

Routine Description:

    This routine gets called on failure to find a TRB matching the Pointer in the Transfer
    Event TRB, in the range of valid TRBs on the Control TransferRing segment. It checks
    whether the supplied Transfer Event TRB points to somewhere in the Control TransferRing
    segment and takes appropriate actions.

--*/
{
    if (IsDequeuePointerInSegment(Trb->Pointer, ControlData->CurrentSegmentBufferData)) {

        //
        // Avoid resetting the controller for known benign case of duplicate ED=0 event.
        //
        if (Controller_GetDeviceFlags(ControlData->ControllerHandle).DropDuplicateEd0TransferEvents) {

            LogWarning(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                       "%u.%u.0: Dropping invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                       UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(ControlData->EndpointHandle), Trb->Pointer.QuadPart);

        } else {

            LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                     "%u.%u.0: Invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                     UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(ControlData->EndpointHandle), Trb->Pointer.QuadPart);

            Controller_ReportFatalError(
                ControlData->ControllerHandle,
                Controller_FatalErrorReset,
                USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0_DUPLICATE,
                ControlData->UsbDeviceHandle,
                ControlData->EndpointHandle,
                ControlData);
        }

    } else {

        LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                 "%u.%u.0: Invalid transfer event TRB.Pointer 0x%I64x",
                 UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                 Endpoint_GetContextIndex(ControlData->EndpointHandle), Trb->Pointer.QuadPart);

        Controller_ReportFatalError(
            ControlData->ControllerHandle,
            Controller_FatalErrorReset,
            USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0,
            ControlData->UsbDeviceHandle,
            ControlData->EndpointHandle,
            ControlData);
    }
}

_Requires_lock_held_(ControlData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_ProcessTransferEventPointer(
    __in
        PCONTROL_DATA           ControlData,
    __in
        PCONTROL_TRANSFER_DATA  TransferData,
    __in
        PTRANSFER_EVENT_TRB     Trb,
    __out
        PULONG                  SkippedTDCount,
    __out
        PULONG                  BytesTransferred
    )
/*++

Routine Description:

    Locate the TRB on the Transfer Ring pointed to by the supplied Transfer Event
    TRB with ED=0.

Arguments:

    Trb - Supplies the Transfer Event TRB with ED=0.

    SkippedTDCount - Receives the count of number of TDs encountered on the Transfer
        Ring before the TD containing the matching TRB. This count includes TDs that
        may have been completed.

    BytesTransferred - Count of bytes transferred for the TD that the matching TRB
        belongs to.

Return Value:

    TRUE if a matching TRB is found on the Transfer Ring. i.e. the supplied Transfer
    Event TRB with ED=0 is pointing to a valid TRB on the Transfer Ring.

--*/
{
    LONGLONG                dequeuePointer;
    ULONG                   endIndex;
    LONGLONG                enqueuePointer;
    ULONG                   localBytesTransferred;
    ULONG                   localSkippedTDCount;
    ULONG                   startIndex;
    PTRB_RANGE              trbRange;
    BOOLEAN                 trbMatch;

    TRY {

        trbMatch              = FALSE;
        localSkippedTDCount   = 0;
        localBytesTransferred = 0;

        dequeuePointer = Trb->Pointer.QuadPart;
        enqueuePointer = (ControlData->CurrentSegmentBufferData->LogicalAddress.QuadPart +
                          (ControlData->CurrentSegmentIndex * sizeof(TRB)));

        if (dequeuePointer == 0) {

            LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                    "%u.%u.0: TransferEventTrb->Pointer is zero",
                    UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(ControlData->EndpointHandle));

            NT_ASSERTMSG("TransferEventTrb->Pointer is zero", 0);
            LEAVE;
        }

        if (dequeuePointer == enqueuePointer) {

            LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                    "%u.%u.0: TransferRing is empty",
                    UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(ControlData->EndpointHandle));

            localSkippedTDCount = TransferData->TDCount;
            trbMatch = TRUE;
            LEAVE;
        }

        trbRange   = &TransferData->TrbRange;
        startIndex = trbRange->FirstSegmentStartIndex;

        //
        // Since this is a single sement circular Transfer Ring, the first index can
        // be greater than the last index.
        //
        if (trbRange->FirstSegmentStartIndex > trbRange->LastSegmentEndIndex) {
            endIndex = ControlData->MaxSegmentIndex;
        } else {
            endIndex = trbRange->LastSegmentEndIndex;
        }

        trbMatch = Control_FindTrbMatch(Trb,
                                        trbRange->FirstSegment,
                                        startIndex,
                                        endIndex,
                                        &localSkippedTDCount,
                                        &localBytesTransferred);

        //
        // A TD wrapped around the single segment circular ring.
        //
        if ((trbMatch == FALSE) &&
            (trbRange->FirstSegmentStartIndex > trbRange->LastSegmentEndIndex)) {

            startIndex = 0;
            endIndex   = trbRange->LastSegmentEndIndex;

            trbMatch = Control_FindTrbMatch(Trb,
                                            trbRange->FirstSegment,
                                            startIndex,
                                            endIndex,
                                            &localSkippedTDCount,
                                            &localBytesTransferred);
        }

    } FINALLY {

        *SkippedTDCount   = localSkippedTDCount;
        *BytesTransferred = localBytesTransferred;
    }

    return trbMatch;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_ProcessTransferEventWithED0(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
/*++

Routine Description:

    Processes a Transfer Event TRB with ED=0 by first locating the TRB that the
    Event points to and calculating the bytes transferred. Takes appropriate
    actions based on whether the completion code of the Event is halted or stopped.

--*/
{
    ULONG                   bytesTransferred;
    PCONTROL_DATA           controlData;
    BOOLEAN                 eventHandled;
    BOOLEAN                 matchFound;
    ULONG                   skippedTDCount;
    ULONG                   totalExpectedEventCount;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        eventHandled            = FALSE;
        skippedTDCount          = 0;
        totalExpectedEventCount = 0;
        bytesTransferred        = 0;
        controlData             = (PCONTROL_DATA)Context;

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: TransferEventTrb 0x%p %!TrbCompletionCode! Length %u EventData %u Pointer 0x%I64x",
                Trb->SlotId, Trb->EndpointId, Trb, (TRB_COMPLETION_CODE)Trb->CompletionCode,
                Trb->TransferLength, Trb->EventData, Trb->Pointer.QuadPart);

        if (Endpoint_StoppedCompletionCode(Trb->CompletionCode) &&
            Endpoint_TR_ShouldDropFSE(controlData->EndpointHandle)) {

            eventHandled = TRUE;
            LEAVE;
        }

        TR_Lock(controlData);

        transferData = controlData->TransferData;











        if (transferData == NULL) {
            TR_Unlock(controlData);
            LEAVE;
        }

        matchFound = Control_ProcessTransferEventPointer(controlData,
                                                         transferData,
                                                         Trb,
                                                         &skippedTDCount,
                                                         &bytesTransferred);

        //
        // Here, we may drop the event or reset the controller.
        //
        if (matchFound == FALSE) {

            TR_Unlock(controlData);

            Control_ValidateED0TrbPointerOnMismatch(controlData, Trb);
            LEAVE;
        }

        if (bytesTransferred > transferData->BytesTotal) {

            LogWarning(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                       "%u.%u.0: BytesTransferred %u greater than expected %u",
                       UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(controlData->EndpointHandle),
                       bytesTransferred, transferData->BytesTotal);

            NT_ASSERTMSG("Invalid bytesTransferred", 0);
            bytesTransferred = 0;
        }

        //
        // bytesTransferred != 0 implies that Event TRB is pointing to somewhere within
        // the Data Stage TD. Take care not to overwrite transferData->BytesTransferred
        // to 0 if Event TRB is pointing to Status Stage TRB, in which case
        // bytesTransferred == 0.
        //
        if (bytesTransferred != 0) {

            transferData->BytesTransferred = bytesTransferred;

            if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
                (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

                RtlCopyMemory(transferData->Buffer,
                              transferData->DoubleBufferData->VirtualAddress,
                              bytesTransferred);
            }
        }

        if (Endpoint_HaltedCompletionCode(Trb->CompletionCode)) {

            transferData->TrbCompletionCode = Trb->CompletionCode;

            TR_Unlock(controlData);

            Endpoint_TR_HaltedCompletionCodeReceived(controlData->EndpointHandle,
                                                     Trb->CompletionCode,
                                                     TRUE);
            eventHandled = TRUE;

        } else if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            if (transferData->BytesTransferred == transferData->BytesTotal) {
                transferData->TrbCompletionCode = TRB_COMPLETION_CODE_SUCCESS;
            } else if (Trb->CompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET) {
                transferData->TrbCompletionCode = TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET;
            }

            transferData->ReceivedEventCount += 1;

            //
            // skippedTDCount is the number of whole TDs for this transfer before the TRB
            // that this Transfer Event is pointing to. Since we expect to receive completion
            // for each of them, totalExpectedEventCount is the sum of skippedTDCount, plus
            // 1 to account for the current Transfer Event.
            //
            totalExpectedEventCount = skippedTDCount + 1;

            //
            // Calculate how many more Transfer Event TRBs remain to be processed before we
            // can reclaim canceled transfers.
            //
            NT_ASSERT(totalExpectedEventCount >= transferData->ReceivedEventCount);

            controlData->OutstandingEventCountAfterStop =
                totalExpectedEventCount - transferData->ReceivedEventCount;

            TR_Unlock(controlData);

            Endpoint_TR_FSEReceived(controlData->EndpointHandle);
            eventHandled = TRUE;

        } else {

            NT_ASSERTMSG("Unrecognized TRB completion code for TRB with ED=0", 0);

            TR_Unlock(controlData);
        }

    } FINALLY {

    }

    return eventHandled;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_ProcessTransferEventWithED1(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
/*++

Routine Description:

    Processes a Transfer Event TRB with ED=1. Keeps track of how many TDs have been
    processed for the transfer (a Control transfer can have 1 or 2 TDs) and completes
    the transfer when all pending TDs are processed.

Arguments:

    Trb - Supplies the Transfer Event TRB with ED=1.

    Context - If non-NULL, implies that the Pointer value in the supplied Trb is not
        to be trusted. The Trb pointer value should be validated, and if not valid,
        the controller should be reset.

Return Value:

--*/
{
    ULONG                   bytesTransferred;
    PCONTROL_DATA           controlData;
    PCONTROL_TRACKING_DATA  controlTrackingData;
    BOOLEAN                 eventHandled;
    BOOLEAN                 acknowledgeExpectedEventTRBsProcessed;
    BOOLEAN                 statusStageCompletion;
    PCONTROL_TRANSFER_DATA  transferData;
    ULONG_PTR               eventData;

    TRY {

        eventHandled = TRUE;
        acknowledgeExpectedEventTRBsProcessed = FALSE;

#if defined (_WIN64)
        eventData = (ULONG_PTR)(Trb->Pointer.QuadPart);
#else
        eventData = (ULONG_PTR)(Trb->Pointer.LowPart);
#endif

        eventData &= ~0x07;

        controlTrackingData = (PCONTROL_TRACKING_DATA)&Trb->Pointer.QuadPart;

        if (controlTrackingData->Stage == 1) {
            statusStageCompletion = TRUE;
        } else {
            statusStageCompletion = FALSE;
        }

        transferData = (PCONTROL_TRANSFER_DATA)eventData;

        if (Context != NULL) {

            controlData = (PCONTROL_DATA)Context;

            if (transferData != controlData->TransferData) {

                LogError(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                         "%u.%u.0: EventData pointer is not valid: 0x%I64x, 0x%p, 0x%p",
                         Trb->SlotId, Trb->EndpointId, Trb->Pointer.QuadPart, transferData, controlData->TransferData);

                Controller_ReportFatalError(controlData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_1,
                                            controlData->UsbDeviceHandle,
                                            controlData->EndpointHandle,
                                            controlData);

                eventHandled = FALSE;
                LEAVE;
            }

        } else {

            controlData = transferData->ControlData;
            if(transferData != controlData->TransferData) {
                
                LogError(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                         "%u.%u.0: ERROR_ASSERT: EventData pointer is not valid (null Context): 0x%I64x, 0x%p, 0x%p",
                         Trb->SlotId, Trb->EndpointId, Trb->Pointer.QuadPart, transferData, controlData->TransferData);

                NT_ASSERTMSG("EventData pointer not valid (null Context)", FALSE);
            }
        }

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: TransferEventTrb 0x%p %!TrbCompletionCode! Length %u EventData %u Pointer 0x%I64x",
                Trb->SlotId, Trb->EndpointId, Trb, (TRB_COMPLETION_CODE)Trb->CompletionCode,
                Trb->TransferLength, Trb->EventData, Trb->Pointer.QuadPart);

        //
        // Some controllers have been known to reference incorrect Endpoint Id in their
        // Transfer Event TRB. See Windows 8 Bug 430453 for details.
        //
        if(Trb->EndpointId != Endpoint_GetContextIndex(controlData->EndpointHandle)) {
            NT_ASSERTMSG("Transfer Event TRB points to incorrect EndpointId", FALSE);
            LogError(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                         "%u.%u.0: ERROR_ASSERT: Transfer Event TRB points to incorrect EndpointId: ", Trb->SlotId, Trb->EndpointId);
        }

        //
        // Some controllers have been known to reference incorrect Slot Id in their
        // Transfer Event TRB. See Windows 8 Bug 431065 for details.
        //
        if(Trb->SlotId != UsbDevice_GetSlotId(controlData->UsbDeviceHandle)) {
            LogError(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                         "%u.%u.0: ERROR_ASSERT: Transfer Event TRB points to incorrect SlotId: ", Trb->SlotId, Trb->EndpointId);

            NT_ASSERTMSG("Transfer Event TRB points to incorrect SlotId", FALSE);
        }
        
        bytesTransferred = Trb->TransferLength;

        if (bytesTransferred > transferData->BytesTotal) {

            LogWarning(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                       "%u.%u.0: ERROR_ASSERT: BytesTransferred %u greater than expected %u",
                       UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(controlData->EndpointHandle),
                       bytesTransferred, transferData->BytesTotal);

            NT_ASSERTMSG("Invalid bytesTransferred", 0);
            bytesTransferred = 0;
        }

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p transferData 0x%p",
                UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(controlData->EndpointHandle),
                transferData->WdfRequest, transferData);

        //
        // BytesTransferred for the transfer comes from Data Stage TRB completion.
        //
        if (statusStageCompletion == FALSE) {

            transferData->BytesTransferred = bytesTransferred;

            if ((transferData->TransferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) &&
                (transferData->TransferMechanism == TransferMechanism_DoubleBufferData)) {

                RtlCopyMemory(transferData->Buffer,
                              transferData->DoubleBufferData->VirtualAddress,
                              bytesTransferred);
            }
        }

        if (Endpoint_HaltedCompletionCode(Trb->CompletionCode)) {








            transferData->TrbCompletionCode = Trb->CompletionCode;

            Endpoint_TR_HaltedCompletionCodeReceived(controlData->EndpointHandle,
                                                     Trb->CompletionCode,
                                                     TRUE);

        } else if (Endpoint_StoppedCompletionCode(Trb->CompletionCode)) {

            TR_Lock(controlData);

            transferData->ReceivedEventCount += 1;

            //
            // Note: Calculation of OutstandingEventCountAfterStop works only for the
            // case of single outstanding transfer at a time. To handle multiple outstanding
            // Control Transfers, totalExpectedEventCount has to be calculated as in the
            // ED=0 case.
            //
            if (statusStageCompletion) {

                if (transferData->BytesTransferred == transferData->BytesTotal) {
                    transferData->TrbCompletionCode = TRB_COMPLETION_CODE_SUCCESS;
                } else if (Trb->CompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET) {
                    transferData->TrbCompletionCode = TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET;
                }
                NT_ASSERT(transferData->TDCount >= transferData->ReceivedEventCount);
                controlData->OutstandingEventCountAfterStop =
                    transferData->TDCount - transferData->ReceivedEventCount;

            } else {

                controlData->OutstandingEventCountAfterStop = 0;
            }

            TR_Unlock(controlData);

            Endpoint_TR_FSEReceived(controlData->EndpointHandle);

        } else {

            TR_Lock(controlData);

            transferData->ReceivedEventCount += 1;

            if (statusStageCompletion) {

                //
                // Result of transfer completion comes from Status Stage TRB completion.
                //
                transferData->TrbCompletionCode = Trb->CompletionCode;

                //
                // At this point, transfer is already canceled or timedout. It will be
                // completed after the Endpoint Stop processing is complete.
                //
                if ((transferData->CancelState != TCS_Canceled) &&
                    (transferData->TimeoutState != TTS_TimedOut)) {

                    Control_Transfer_CompleteCancelable(controlData);
                }
            }

            //
            // OutstandingEventCountAfterStop will be set to a non-zero value if a Stopped Event
            // was processed before this Event.
            //
            if (controlData->OutstandingEventCountAfterStop != 0) {

                controlData->OutstandingEventCountAfterStop -= 1;

                if ((controlData->OutstandingEventCountAfterStop == 0) &&
                    (controlData->Flags.AcknowledgeExpectedEventTRBsProcessed)) {

                    acknowledgeExpectedEventTRBsProcessed = TRUE;
                }
            }

            TR_Unlock(controlData);
        }

    } FINALLY {

        if (acknowledgeExpectedEventTRBsProcessed) {
            Endpoint_TR_ExpectedEventTRBsProcessed(controlData->EndpointHandle);
        }
    }

    return eventHandled;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Control_EP_TransferEventHandler(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    )
{
    if (Trb->EventData == 1) {
        return Control_ProcessTransferEventWithED1(Trb, Context);
    } else {
        return Control_ProcessTransferEventWithED0(Trb, Context);
    }
}

VOID
Control_TransferData_Initialize(
    __in
        PCONTROL_DATA           ControlData,
    __in
        WDFREQUEST              WdfRequest,
    __in
        PTRANSFER_URB           TransferUrb,
    __out
        PCONTROL_TRANSFER_DATA  TransferData
    )
{
    RtlZeroMemory(TransferData, sizeof(CONTROL_TRANSFER_DATA));

    InitializeListHead(&TransferData->ListEntry);

    TransferData->Initialized                     = TRUE;
    TransferData->WdfRequest                      = WdfRequest;
    TransferData->TransferUrb                     = TransferUrb;
    TransferData->ControlData                     = ControlData;
    TransferData->TransferMechanism               = TransferMechanism_NoData;
    TransferData->Mdl                             = NULL;
    TransferData->Buffer                          = NULL;
    TransferData->DoubleBufferData                = NULL;
    TransferData->ScatterGatherList               = NULL;
    TransferData->BytesTotal                      = TransferUrb->TransferBufferLength;
    TransferData->BytesTransferred                = 0;
    TransferData->CancelState                     = TCS_NotCancelable;
    TransferData->TimeoutState                    = TTS_NoTimeout;
    TransferData->Status                          = STATUS_PENDING;
    TransferData->TrbCompletionCode               = TRB_COMPLETION_CODE_INVALID;

    TransferData->TrbRange.FirstSegment           = ControlData->CurrentSegmentBufferData;
    TransferData->TrbRange.FirstSegmentStartIndex = ControlData->CurrentSegmentIndex;
    TransferData->TrbRange.LastSegment            = ControlData->CurrentSegmentBufferData;
    TransferData->TrbRange.LastSegmentEndIndex    = ControlData->CurrentSegmentIndex;

    //
    // This is done to maintain COMPAT with the USB 2.0 stack.
    //
    TransferUrb->TransferBufferLength = 0;
}

VOID
Control_TransferData_Free(
    __in
        PCONTROL_DATA           ControlData,
    __in
        PCONTROL_TRANSFER_DATA  TransferData
    )
{
    PDMA_ADAPTER    dmaAdapter;
    KIRQL           savedIrql;
    PTRANSFER_URB   transferUrb;

    transferUrb = TransferData->TransferUrb;

    switch (TransferData->TransferMechanism) {

    case TransferMechanism_NoData:
        break;

    case TransferMechanism_ImmediateData:
        break;

    case TransferMechanism_DoubleBufferData:

        TR_ReleaseDoubleBuffer(ControlData, TransferData->DoubleBufferData);
        TransferData->DoubleBufferData = NULL;
        break;

    case TransferMechanism_Dma:

        if (TransferData->ScatterGatherList != NULL) {

            dmaAdapter =
                CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(ControlData->ControllerHandle));

            KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);
            dmaAdapter->DmaOperations->PutScatterGatherList(
                dmaAdapter,
                TransferData->ScatterGatherList,
                ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));
            KeLowerIrql(savedIrql);

            TransferData->ScatterGatherList = NULL;
        }

        if ((TransferData->Mdl != NULL) &&
            (TransferData->Mdl != transferUrb->TransferBufferMDL) &&
            (TransferData->Mdl != ControlData->ForwardProgressMdl)) {

            IoFreeMdl(TransferData->Mdl);
            TransferData->Mdl = NULL;
        }
        break;

    default:
        break;
    }

    TransferData->Initialized = FALSE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_Transfer_DetermineTransferMechanism(
    __in
        PCONTROL_DATA ControlData
    )
{
    PCONTROL_TRANSFER_DATA  transferData;
    TRANSFER_MECHANISM      transferMechanism;
    PTRANSFER_URB           transferUrb;

    TRY {

        transferData = ControlData->TransferData;
        transferUrb  = transferData->TransferUrb;

        if (transferData->BytesTotal == 0) {

            transferMechanism = TransferMechanism_NoData;
            LEAVE;
        }

        //
        // Table 73: IDT shall not be set ('1') for TRBs on endpoints that define a Max
        // Packet Size < 8 bytes, or on IN endpoints.
        //
        if ((ControlData->ImmediateDataEnabled) &&
            (transferData->BytesTotal <= IMMEDIATE_DATA_LENGTH) &&
            ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0)) {

            transferMechanism = TransferMechanism_ImmediateData;
            LEAVE;
        }

        if ((TR_IsUrbUsingChainedMdl(transferUrb) == FALSE) &&
            (transferData->BytesTotal <= ControlData->DoubleBufferSize)) {

            transferData->DoubleBufferData = TR_AcquireDoubleBuffer(ControlData);
            if (transferData->DoubleBufferData != NULL) {
                transferMechanism = TransferMechanism_DoubleBufferData;
                LEAVE;
            }
        }

        transferMechanism = TransferMechanism_Dma;

    } FINALLY {

        transferData->TransferMechanism = transferMechanism;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Control_Transfer_ConfigureBuffer(
    __in
        PCONTROL_DATA ControlData
    )
/*++

Routine Description:

    Given the Transfer Mechanism, ensures that either the transfer Buffer or the
    MDL are valid.

--*/
{
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;
    PTRANSFER_URB           transferUrb;

    TRY {

        status       = STATUS_SUCCESS;
        transferData = ControlData->TransferData;
        transferUrb  = transferData->TransferUrb;

        switch (transferData->TransferMechanism) {

        case TransferMechanism_NoData:
            break;

        case TransferMechanism_ImmediateData:
        case TransferMechanism_DoubleBufferData:

            if (transferUrb->TransferBuffer != NULL) {

                transferData->Buffer = transferUrb->TransferBuffer;

            } else {

                transferData->Buffer =
                    MmGetSystemAddressForMdlSafe(transferUrb->TransferBufferMDL, NormalPagePriority);

                if (transferData->Buffer == NULL) {

                    LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                             "%u.%u.0: MmGetSystemAddressForMdlSafe (TransferBuffer) returned NULL",
                             UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                             Endpoint_GetContextIndex(ControlData->EndpointHandle));

                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LEAVE;
                }
            }
            break;

        case TransferMechanism_Dma:

            if (transferUrb->TransferBufferMDL != NULL) {

                transferData->Mdl = transferUrb->TransferBufferMDL;

            } else {

                transferData->Mdl = IoAllocateMdl(transferUrb->TransferBuffer,
                                                  transferData->BytesTotal,
                                                  FALSE,
                                                  FALSE,
                                                  NULL);

                if (transferData->Mdl == NULL) {

                    if ((transferUrb->TransferFlags & USBD_FORWARD_PROGRESS_TRANSFER) &&
                        (ControlData->ForwardProgressMdl != NULL)) {

                        LogVerbose(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                                   "%u.%u.0: Using ForwardProgressMdl",
                                   UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(ControlData->EndpointHandle));

                        transferData->Mdl = ControlData->ForwardProgressMdl;

                        MmInitializeMdl(transferData->Mdl, transferUrb->TransferBuffer, transferData->BytesTotal);

                    } else {

                        LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                                 "%u.%u.0: Failed to allocate Mdl",
                                 UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                                 Endpoint_GetContextIndex(ControlData->EndpointHandle));

                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }
                }

                MmBuildMdlForNonPagedPool(transferData->Mdl);
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
NTSTATUS
Control_Transfer_ValidateBuffer(
    __in
        PCONTROL_DATA ControlData
    )
{
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        transferData = ControlData->TransferData;

        if (transferData->TransferMechanism == TransferMechanism_NoData) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        if (transferData->BytesTotal > MAX_CONTROL_TRANSFER_LENGTH) {

            LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                     "%u.%u.0: WdfRequest 0x%p transfer size 0x%X is larger than supported size 0x%X",
                     UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(ControlData->EndpointHandle),
                     transferData->WdfRequest, transferData->BytesTotal, MAX_CONTROL_TRANSFER_LENGTH);

            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        //
        // WIN8 implementation had undefined behavior if a Control Transfer could not be
        // fit into one DMA transaction. Here we explicitly fail this case.
        //
        if (transferData->BytesTotal > ControlData->MaxStageSize) {

            LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                     "%u.%u.0: WdfRequest 0x%p transfer size 0x%X is larger than the size 0x%X supported by DMA engine",
                     UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(ControlData->EndpointHandle),
                     transferData->WdfRequest, transferData->BytesTotal, ControlData->MaxStageSize);

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        if ((transferData->BytesTotal == ControlData->MaxStageSize) &&
            (IsPageAligned(transferData->Buffer) == FALSE)) {

            LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                     "%u.%u.0: WdfRequest 0x%p Non-page aligned transfer of size 0x%X cannot be fit into the DMA transaction",
                     UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(ControlData->EndpointHandle),
                     transferData->WdfRequest, transferData->BytesTotal);

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        if ((transferData->TransferMechanism == TransferMechanism_Dma) &&
            (transferData->Mdl->Next != NULL)) {

            LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                     "%u.%u.0: WdfRequest 0x%p Chained MDL not supported for Control Transfers",
                     UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(ControlData->EndpointHandle),
                     transferData->WdfRequest);

            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_InitializeLinkTrb(
    __in
        PCONTROL_DATA   ControlData,
    __in
        TRB_STAGE       TrbStage,
    __in
        PTRB            LinkTrb,
    __in
        BOOLEAN         FirstTrb
    )
/*++

Routine Description:

    Initialize the Link TRB for a Control TransferRing which is a single
    segment circular ring.

    Also toggle the TransferRing CycleState variable and update the
    CurrentSegmentIndex.

--*/
{
    TR_InitializeLinkTrb(ControlData, TrbStage, LinkTrb, FirstTrb);

    //
    // Since this is a single segment circular ring, RingSegmentPointer points
    // back to the beginning of the same segment.
    //
    LinkTrb->Link.RingSegmentPointer =
        ControlData->CurrentSegmentBufferData->LogicalAddress.QuadPart;

    //
    // Set the ToggleCycle bit since this is a single segment circular ring.
    //
    LinkTrb->Link.ToggleCycle = 1;

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_Transfer_MapIntoRing(
    __in
        PCONTROL_DATA ControlData
    )
{
    BOOLEAN                 acknowledgeStop;
    PCONTROL_TRANSFER_DATA  transferData;
    PTRANSFER_URB           transferUrb;
    TRB_STAGE               trbStage;
    PCONTROL_TRACKING_DATA  controlTrackingData;
    ULONG                   currentElement;
    ENDPOINT_DIRECTION      dataDirection;
    PTRB                    currentTrb;
    TRB                     localTrb;
    ULONG64                 elementAddress;
    ULONG                   elementLength;
    ULONG                   maxTrbLength;
    PTRB                    firstTrb;
    ULONG                   packetCount;
    ULONG                   lengthMapped;
    BOOLEAN                 setReserved1LowestBitInNormalTrbs;
    ULONG                   TDCount;

    TRY {

        ICE_START_TRACE();

        TDCount        = 0;
        transferData   = ControlData->TransferData;
        transferUrb    = transferData->TransferUrb;

        if (transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) {
            dataDirection = EndpointDirection_In;
        } else {
            dataDirection = EndpointDirection_Out;
        }

        //
        // TRBs in the Data Stage TD require TD Size to be set. packetCount will be
        // used in calculating TD Size for these TRBs.
        //
        packetCount = TR_GetPacketCount(ControlData, transferData->BytesTotal);

        //
        // Cache the initial TRB for this request. This is used in the case where
        // this request needs to be removed from the transfer ring.
        //
        currentElement  = 0;
        elementAddress  = 0;
        elementLength   = 0;
        lengthMapped    = 0;
        trbStage        = TrbStage_Setup;
        firstTrb        = &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex];

        setReserved1LowestBitInNormalTrbs = FALSE;

        //
        // Check if a link TRB can be inserted into a TD. For a control endpoint,
        // this is done by inserting a link TRB into the current transfer ring
        // segment pointing back to the beginning of the same transfer ring segment.
        //
        // For a control endpoint, special care needs to be taken that the link
        // TRB is not overwritten by the new TD. This could happen if the
        // previous TD is smaller than the current TD. To avoid this case, a
        // transfer ring segment size must be large enough to hold the largest
        // supported control transfer.
        //
        // The logic used is to assume this TD is the largest supported, which
        // requires a total of 21 TRBs:
        //
        //  01 - Setup TRB
        //  17 - Data Stage/Normal TRBs - allows for 1 64kb transfer MDL made of physically
        //           non-contiguous pages (control transfers don't support chained MDLs)
        //  01 - Event Data TRB for Data Stage
        //  01 - Status Stage TRB
        //  01 - Event Data TRB for Status Stage
        //
        //  21 - Total number of TRBs required for worst case control transfer length.
        //
        if (Controller_GetDeviceFlags(ControlData->ControllerHandle).NoLinkTRBInTD &&
            ((ControlData->CurrentSegmentIndex + MAX_NUMBER_OF_TRBS_FOR_CONTROL_TD) >
             ControlData->MaxSegmentIndex)) {

            //
            // This is the minimum size transfer ring that can be used to support
            // NoLinkTRBInTD workaround. It is the sum of two worst case control
            // transfers plus a link TRB.
            //
            NT_ASSERT(ControlData->MaxSegmentIndex >= (MAX_NUMBER_OF_TRBS_FOR_CONTROL_TD * 2));

            //
            // Program a Link TRB at the current index. Ensure that its Cycle Bit is invalid.
            // It will be made valid after the entire TD is programmed.
            //
            Control_InitializeLinkTrb(ControlData, trbStage, &localTrb, TRUE);

            currentTrb = &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex];
            RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

            //
            // Toggle the internal cycle state. This must be done after the Link TRB
            // is built, otherwise the Link TRB will contain an invalid cycle bit value.
            //
            TR_ToggleCycleState(ControlData);

            ControlData->CurrentSegmentIndex = 0;
        }

        //
        // Control transfers contain a Setup, optional Data, and Status packet. Stage through
        // the different types of packets required, starting with the Setup packet.
        //
        while (trbStage != TrbStage_Done) {

            NT_ASSERT(ControlData->CurrentSegmentIndex <= ControlData->MaxSegmentIndex);

            currentTrb = &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex];

            //
            // The last trb in the transfer ring is a link trb that points to the
            // beginning of the transfer ring. Take this into account when determining
            // if we are at the end of the transfer ring.
            //
            if ((ControlData->CurrentSegmentIndex + 1) > ControlData->MaxSegmentIndex) {

                NT_ASSERT(Controller_GetDeviceFlags(ControlData->ControllerHandle).NoLinkTRBInTD == 0);

                LogVerbose(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                           "%u.%u.0: Insert: CurrentSegment[%u] 0x%p Link TRB CycleState %u",
                           UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(ControlData->EndpointHandle),
                           ControlData->CurrentSegmentIndex,
                           &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex],
                           TR_GetCycleState(ControlData));

                Control_InitializeLinkTrb(ControlData, trbStage, &localTrb, (currentTrb == firstTrb)); 

                RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

                //
                // Toggle the internal cycle state. This must be done after the Link TRB
                // is built, otherwise the Link TRB will contain an invalid cycle bit value.
                //
                TR_ToggleCycleState(ControlData);

                ControlData->CurrentSegmentIndex = 0;
                continue;
            }

            LogVerbose(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                       "%u.%u.0: Insert: CurrentSegment[%u] 0x%p CycleState %u",
                       UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(ControlData->EndpointHandle),
                       ControlData->CurrentSegmentIndex,
                       &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex],
                       TR_GetCycleState(ControlData));

            RtlZeroMemory(&localTrb, sizeof(TRB));

            switch (trbStage) {

            case TrbStage_Setup:

                localTrb.SetupStage.Type           = TRB_TYPE_SETUP_STAGE;
                localTrb.SetupStage.ImmediateData  = 1;
                localTrb.SetupStage.TransferLength = 8;

                RtlCopyMemory(&localTrb,
                              &transferUrb->Control.SetupPacket[0],
                              sizeof(transferUrb->Control.SetupPacket));

                if (transferData->BytesTotal == 0) {

                    localTrb.SetupStage.TransferType = SETUP_STAGE_TRANSFER_TYPE_NO_DATA_STAGE;
                    trbStage = TrbStage_Status;

                } else {

                    if (dataDirection == EndpointDirection_In) {
                        localTrb.SetupStage.TransferType = SETUP_STAGE_TRANSFER_TYPE_IN_DATA_STAGE;
                    } else {
                        localTrb.SetupStage.TransferType = SETUP_STAGE_TRANSFER_TYPE_OUT_DATA_STAGE;
                    }
                    trbStage = TrbStage_Data;
                }
                break;

            //
            // Excluding the direction, the Data and Normal TRBs are identical. This
            // code references the Normal TRB for mapping the data.
            //
            case TrbStage_Data:
            case TrbStage_Normal:

                if (trbStage == TrbStage_Data) {
                    localTrb.DataStage.Type      = TRB_TYPE_DATA_STAGE;
                    localTrb.DataStage.Direction = dataDirection;
                } else {
                    localTrb.Normal.Type         = TRB_TYPE_NORMAL;
                }

                //
                // Immediate data can only be used on out endpoints.
                //
                switch (transferData->TransferMechanism) {

                case TransferMechanism_NoData:

                    NT_ASSERTMSG("There should not be a Data stage if TransferLength is 0", 0);
                    LEAVE;

                case TransferMechanism_ImmediateData:

                    NT_ASSERT(dataDirection == EndpointDirection_Out);
                    NT_ASSERT(transferData->Buffer != NULL);
                    NT_ASSERT(transferData->BytesTotal <= IMMEDIATE_DATA_LENGTH);

                    __analysis_assume(transferData->BytesTotal <= IMMEDIATE_DATA_LENGTH);

                    RtlCopyMemory(&localTrb.Normal.DataBufferPointer,
                                  transferData->Buffer,
                                  transferData->BytesTotal);

                    localTrb.Normal.ImmediateData  = 1;
                    localTrb.Normal.TransferLength = transferData->BytesTotal;

                    trbStage = TrbStage_EventData;
                    break;

                case TransferMechanism_DoubleBufferData:

                    NT_ASSERT(transferData->ScatterGatherList == NULL);
                    NT_ASSERT(transferData->DoubleBufferData != NULL);
                    NT_ASSERT(transferData->Buffer != NULL);

                    //
                    // Currently we don't transfer more than 512 bytes using double buffering,
                    // therefore we don't need to apply the DataStageTrbMax512 hack in this path.
                    // Get the compiler to complain if this changes.
                    //
                    { C_ASSERT(SMALL_SEGMENT_SIZE <= 512); }

                    if (dataDirection == EndpointDirection_Out) {

                        RtlCopyMemory(transferData->DoubleBufferData->VirtualAddress,
                                      transferData->Buffer,
                                      transferData->BytesTotal);
                    }

                    localTrb.Normal.DataBufferPointer = transferData->DoubleBufferData->LogicalAddress.QuadPart;
                    localTrb.Normal.TransferLength    = transferData->BytesTotal;

                    trbStage = TrbStage_EventData;
                    break;

                case TransferMechanism_Dma:

                    NT_ASSERT(transferData->ScatterGatherList != NULL);

                    if (elementLength == 0) {

                        elementAddress = transferData->ScatterGatherList->Elements[currentElement].Address.QuadPart;
                        elementLength  = transferData->ScatterGatherList->Elements[currentElement].Length;
                    }

                    maxTrbLength = MAX_TRB_LENGTH;

                    //
                    // Determine whether this is a Data Stage TRB whose length needs to be shortened
                    // due to hack flag.
                    //
                    if ((trbStage == TrbStage_Data) &&
                        (elementLength > 512) &&
                        Controller_GetDeviceFlags(ControlData->ControllerHandle).DataStageTrbMax512) {

                        //
                        // Shorten this TRB to 512 bytes.
                        //
                        maxTrbLength = 512;










                        if (dataDirection == EndpointDirection_In) {

                            setReserved1LowestBitInNormalTrbs = TRUE;
                        }
                    }

                    //
                    // DataStageTrbMax512 affects certain normal TRBs, as described
                    // above.
                    //
                    if ((trbStage == TrbStage_Normal) && (setReserved1LowestBitInNormalTrbs != FALSE)) {
                        localTrb.Normal.Reserved1 |= 0x1;
                    }

                    if (elementLength <= maxTrbLength) {

                        localTrb.Normal.DataBufferPointer = elementAddress;
                        localTrb.Normal.TransferLength    = elementLength;

                        elementLength   = 0;
                        currentElement += 1;

                        //
                        // If this is the only entry in the scatter/gather list, then the
                        // next TRB is an event data TRB. If not, then the next TRB is a
                        // transfer TRB. In either case, they need to be chained together.
                        //
                        if (currentElement == transferData->ScatterGatherList->NumberOfElements) {
                            trbStage = TrbStage_EventData;
                        } else {
                            trbStage = TrbStage_Normal;
                        }

                    } else {

                        localTrb.Normal.DataBufferPointer = elementAddress;
                        localTrb.Normal.TransferLength    = maxTrbLength;

                        elementAddress += maxTrbLength;
                        elementLength  -= maxTrbLength;

                        trbStage = TrbStage_Normal;
                    }
                    break;
                }

                lengthMapped += localTrb.Normal.TransferLength;
                NT_ASSERT(lengthMapped <= transferData->BytesTotal);

                localTrb.Normal.TDSize = TR_CalculateTDSize(ControlData,
                                                            packetCount,
                                                            lengthMapped,
                                                            (trbStage == TrbStage_EventData));

                localTrb.Normal.ChainBit = 1;
                break;

            case TrbStage_Status:

                localTrb.StatusStage.Type = TRB_TYPE_STATUS_STAGE;

                if (dataDirection == EndpointDirection_In) {
                    localTrb.StatusStage.Direction = EndpointDirection_Out;
                } else {
                    localTrb.StatusStage.Direction = EndpointDirection_In;
                }

                localTrb.StatusStage.ChainBit = 1;

                trbStage = TrbStage_FinalEventData;
                break;

            case TrbStage_EventData:
            case TrbStage_FinalEventData:

                localTrb.EventData.Type                  = TRB_TYPE_EVENT_DATA;
                localTrb.EventData.InterruptOnCompletion = 1;

#if defined(DBG) && !defined(_WIN64)
                localTrb.EventData.Data  = ((ULONG64)Controller_GetNextEventDataId(ControlData->ControllerHandle) << 32);
                localTrb.EventData.Data |= ((ULONG64)transferData & 0x0FFFFFFFF);
#else
                localTrb.EventData.Data  = (ULONG_PTR)transferData;
#endif

                controlTrackingData = (PCONTROL_TRACKING_DATA)&localTrb.EventData.Data;
                controlTrackingData->EndpointType = USB_ENDPOINT_TYPE_CONTROL;

                TDCount += 1;

                if (trbStage == TrbStage_FinalEventData) {
                    controlTrackingData->Stage = 1;
                    trbStage = TrbStage_Done;
                } else {
                    trbStage = TrbStage_Status;
                }
                break;
            }

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
            if ((trbStage == TrbStage_EventData) || (trbStage == TrbStage_FinalEventData)) {
                NT_ASSERT(localTrb.Normal.ChainBit == 1);
                localTrb.Normal.EvaluateNextTrb = 1;
            }

            //
            // If this is the first TRB, keep it inactive until the rest of the TD is formed.
            //
            if (currentTrb == firstTrb) {
                localTrb.Normal.CycleBit = INVERT(TR_GetCycleState(ControlData));
            } else {
                localTrb.Normal.CycleBit = TR_GetCycleState(ControlData);
            }

            //
            // Set the target event ring for this request. The normal TRB is used for all
            // TRB types. This is safe because the interrupter target value in all of the
            // TRBs is located in the same location within the TRB structure.
            //
            localTrb.Normal.InterrupterTarget = ControlData->InterrupterTarget;

            RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

            ControlData->CurrentSegmentIndex += 1;
        }

        transferData->TrbRange.LastSegmentEndIndex = ControlData->CurrentSegmentIndex;

        //
        // Invalidate the CycleBit in the next unused TRB, making no assumptions
        // about the current value of its CycleBit. This is because the TransferRing
        // segment is not reinitialized when reused.
        //
        NT_ASSERT(ControlData->CurrentSegmentIndex <= ControlData->MaxSegmentIndex);

        //
        // Since the segment is not reinitialized when reused, ensure that the next
        // TRB is invalid by clearing its CycleBit. Be extra conservative by zero'ing
        // out the rest of the bits in the TRB.
        //
        RtlZeroMemory(&localTrb, sizeof(TRB));
        localTrb.Normal.CycleBit = INVERT(TR_GetCycleState(ControlData));

        currentTrb = &ControlData->CurrentSegment[ControlData->CurrentSegmentIndex];
        RtlCopyMemory(currentTrb, &localTrb, sizeof(TRB));

        //
        // Update the TDCount BEFORE making the TRBs visible to the controller.
        //
        TR_Lock(ControlData);

        transferData->TDCount = TDCount;

        ControlData->Flags.DoorbellRungSinceMappingStart = 1;

        if (ControlData->MappingState == MS_Mapping) {
            ControlData->MappingState = MS_Paused;
            acknowledgeStop = FALSE;
        } else {
            NT_ASSERT(ControlData->MappingState == MS_Stopping);
            ControlData->MappingState = MS_Stopped;
            acknowledgeStop = TRUE;
        }

        TR_Unlock(ControlData);

        LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                "%u.%u.0: Programmed WdfRequest 0x%p transferData 0x%p",
                UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(ControlData->EndpointHandle),
                transferData->WdfRequest, transferData);

        //
        // Commit the first TRB using the local TRB copy. Cycle Bit should
        // be made valid only after copying the entire TRB.
        //
        Register_ToggleTrbCycleBit(firstTrb);

        UsbDevice_WriteDoorbell(Endpoint_GetUsbDeviceHandle(ControlData->EndpointHandle),
                                Endpoint_GetContextIndex(ControlData->EndpointHandle),
                                ControlData->StreamId);

        if (acknowledgeStop) {
            Endpoint_TR_MappingStopped(ControlData->EndpointHandle);
        }

    } FINALLY {

        ICE_STOP_TRACE();
    }

    return;
}

VOID
Control_EvtDmaCallback(
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
    PCONTROL_DATA ControlData;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    ControlData = (PCONTROL_DATA)Context;

    ControlData->TransferData->ScatterGatherList = ScatterGatherList;

    Control_Transfer_MapIntoRing(ControlData);

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Control_Transfer_Map(
    __in
        PCONTROL_DATA ControlData
    )
{
    PDEVICE_OBJECT          deviceObject;
    PDMA_ADAPTER            dmaAdapter;
    KIRQL                   savedIrql;
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;
    PTRANSFER_URB           transferUrb;
    BOOLEAN                 timerAlreadyInQueue;

    TRY {

        status       = STATUS_SUCCESS;
        transferData = ControlData->TransferData;
        transferUrb  = transferData->TransferUrb;

        if ((transferUrb->Header.Function == URB_FUNCTION_CONTROL_TRANSFER_EX) &&
            (transferUrb->Timeout != 0)) {

            transferData->TimeoutState = TTS_TimeoutSet;

            timerAlreadyInQueue = WdfTimerStart(ControlData->WdfTimerForTransferTimeout,
                                                WDF_REL_TIMEOUT_IN_MS(transferUrb->Timeout));

            if (timerAlreadyInQueue) {
                NT_ASSERT(FALSE);
            }
        }

        ControlData->InterrupterTarget =
            Interrupter_GetInterrupterTarget(
                Controller_GetInterrupterHandle(ControlData->ControllerHandle),
                transferUrb->UrbData.ProcessorNumber);

        if (transferData->TransferMechanism != TransferMechanism_Dma) {

            Control_Transfer_MapIntoRing(ControlData);

        } else {

            dmaAdapter =
                CommonBuffer_GetDmaAdapter(Controller_GetCommonBufferHandle(ControlData->ControllerHandle));

            deviceObject =
                WdfDeviceWdmGetDeviceObject(Controller_GetWdfDeviceObject(ControlData->ControllerHandle));

            KeRaiseIrql(DISPATCH_LEVEL, &savedIrql);

            status = dmaAdapter->DmaOperations->GetScatterGatherList(
                        dmaAdapter,
                        deviceObject,
                        transferData->Mdl,
                        MmGetMdlVirtualAddress(transferData->Mdl),
                        transferData->BytesTotal,
                        Control_EvtDmaCallback,
                        ControlData,
                        ((transferUrb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) == 0));
        
            KeLowerIrql(savedIrql);

            if (!NT_SUCCESS(status)) {

                LogError(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                         "%u.%u.0: GetScatterGatherList Failed %!STATUS!",
                         UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(ControlData->EndpointHandle), status);
                LEAVE;
            }
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_MapTransfer(
    __in
        PCONTROL_DATA ControlData
    )
{
    BOOLEAN                 acknowledgeStop;
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        Control_Transfer_DetermineTransferMechanism(ControlData);

        status = Control_Transfer_ConfigureBuffer(ControlData);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = Control_Transfer_ValidateBuffer(ControlData);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = Control_Transfer_Map(ControlData);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            TR_Lock(ControlData);

            transferData = ControlData->TransferData;
            transferData->Status = status;

            if (ControlData->MappingState == MS_Mapping) {
                ControlData->MappingState = MS_Paused;
                acknowledgeStop = FALSE;
                Control_Transfer_CompleteCancelable(ControlData);
            } else {
                NT_ASSERT(ControlData->MappingState == MS_Stopping);
                ControlData->MappingState = MS_Stopped;
                acknowledgeStop = TRUE;
            }

            TR_Unlock(ControlData);

            if (acknowledgeStop) {
                Endpoint_TR_MappingStopped(ControlData->EndpointHandle);
            }
        }
    }
}

VOID
Control_WdfEvtIoDefault(
    WDFQUEUE      WdfQueue,
    WDFREQUEST    WdfRequest
    )
/*++

Routine Description:

    This is the WDF callback that delivers a WDFREQUEST from client. Since the
    Control TransferRing Queue is configured as sequential, this callback can
    only arrive when the number of driver owned requests is 0.

    This routine will start mapping the request unless MappingState is Stopped.
    If MappingState is Stopped, request pointer is saved, and it will be mapped
    when the StartMapping callback arrives from ESM.

--*/
{
    PCONTROL_DATA           controlData;
    BOOLEAN                 processTransfer;
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;
    PTRANSFER_URB           transferUrb;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    TRY {

        processTransfer = FALSE;

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        transferUrb  = (PTRANSFER_URB)wdfRequestParams.Parameters.Others.Arg1;
        controlData  = (PCONTROL_DATA)TR_GetContext(WdfQueue);
        transferData = &(GetRequestData(WdfRequest)->ControlTransferData);

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p Bytes %u transferData 0x%p",
                UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(controlData->EndpointHandle),
                WdfRequest, transferUrb->TransferBufferLength, transferData);

        Control_TransferData_Initialize(controlData, WdfRequest, transferUrb, transferData);

        TR_Lock(controlData);

        NT_ASSERT(controlData->TransferData == NULL);
        controlData->TransferData = transferData;

        //
        // Note: Since only one control transfer is currently programmed into the
        // hardware at a time, these variables are tracked in the transfer. If we
        // were to program multiple transfers at a time, these variables would have
        // to be tracked at the TransferRing object level, and initialized in the
        // EP_StartMapping callback.
        //
        transferData->TDCount            = 0;
        transferData->ReceivedEventCount = 0;

        //
        // Mark the request as Cancelable upfront. This is needed for the following
        // case: Mapping is stopped. ESM directed to complete canceled transfers (eg.
        // because Controller got surprise removed). If the transfer is not already
        // marked cancelable, we may never get the Cancel callback, and so we may
        // never complete it.
        //
        transferData->CancelState = TCS_Cancelable;

        status = WdfRequestMarkCancelableEx(transferData->WdfRequest, Control_WdfEvtRequestCancel);

        if (!NT_SUCCESS(status)) {

            LogWarning(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                       "%u.%u.0: WdfRequestMarkCancelableEx returned %!STATUS!",
                       UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                       Endpoint_GetContextIndex(controlData->EndpointHandle), status);

            NT_ASSERT(status == STATUS_CANCELLED);

            transferData->CancelState = TCS_Canceled;

            Control_Transfer_CompleteCancelable(controlData);

            TR_Unlock(controlData);
            LEAVE;
        }

        if (controlData->MappingState == MS_Paused) {

            controlData->MappingState = MS_Mapping;
            processTransfer = TRUE;

        } else {

            //
            // EvtIoDefault callback came in before the State Machine gave its Ok to
            // start mapping. Save the TransferData pointer and leave. Transfer will
            // get processed when the State Machine says start mapping.
            //
            NT_ASSERT(controlData->MappingState == MS_Stopped);
        }

        TR_Unlock(controlData);

    } FINALLY {

        if (processTransfer) {
            Control_MapTransfer(controlData);
        }
    }

    return;
}

VOID
Control_WdfEvtIoCanceledOnQueue(
    WDFQUEUE    WdfQueue,
    WDFREQUEST  WdfRequest
    )
{
    PCONTROL_DATA           controlData;
    PCONTROL_TRANSFER_DATA  transferData;
    PTRANSFER_URB           transferUrb;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        controlData  = (PCONTROL_DATA)TR_GetContext(WdfQueue);
        transferData = &(GetRequestData(WdfRequest)->ControlTransferData);
        transferUrb  = (PTRANSFER_URB)wdfRequestParams.Parameters.Others.Arg1;

        NT_ASSERT(transferData->Initialized == FALSE);

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p Canceled on Queue",
                UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(controlData->EndpointHandle), WdfRequest);

        Control_TransferData_Initialize(controlData, WdfRequest, transferUrb, transferData);

        transferData->CancelState = TCS_Canceled;

        TR_Lock(controlData);

        InsertTailList(&controlData->CanceledOnQueueTransferList, &transferData->ListEntry);

        TR_Unlock(controlData);

    } FINALLY {

        WdfDpcEnqueue(controlData->WdfDpcForCanceledOnQueueTransferCompletion);
    }
}

VOID
Control_WdfEvtTimerForTransferTimeout(
    WDFTIMER WdfTimer
    )
/*++

Routine Description:

    This is the Timer that fires after the timeout for Control Transfers that do have
    a timeout.

--*/
{
    WDFQUEUE                wdfQueue;
    PCONTROL_DATA           controlData;
    BOOLEAN                 queueDpc;
    BOOLEAN                 reportCancel;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        queueDpc     = FALSE;
        reportCancel = FALSE;
        wdfQueue     = WdfTimerGetParentObject(WdfTimer);
        controlData  = (PCONTROL_DATA)TR_GetContext(wdfQueue);
        transferData = controlData->TransferData;

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p timed out",
                UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(controlData->EndpointHandle), transferData->WdfRequest);

        TR_Lock(controlData);

        if (transferData->TimeoutState == TTS_TimeoutSet) {

            if (controlData->Flags.OkToReclaimTransfersOnCancel) {
                queueDpc = TRUE;
            } else {
                reportCancel = TRUE;
            }
        }

        TR_Unlock(controlData);

        if (reportCancel) {
            Endpoint_TR_TransferCanceled(controlData->EndpointHandle);
        }

        TR_Lock(controlData);

        //
        // WdfTimerStop failed during completion of the transfer because the Timer was not
        // in the Dpc queue (i.e. about to execute). Now that the Timer callback has arrived,
        // continue with completion of the transfer.
        //
        if (transferData->TimeoutState == TTS_WaitingForTimerCallback) {

            if (reportCancel) {

                //
                // If Cancel is reported to ESM, we cannot complete the transfer until
                // the state machine OK's it. This is because as soon as we complete one
                // transfer, we may receive another one through EvtIoDefault, and if the
                // ESM OK's completion of transfer after that point, we may prematurely
                // complete the new transfer.
                //
                if ((controlData->Flags.ReclaimTransfersAndAcknowledge) ||
                    (controlData->Flags.OkToReclaimTransfersOnCancel)) {
                    queueDpc = TRUE;
                }

            } else {

                queueDpc = TRUE;
            }
        }

        //
        // Updating the TimeoutState last keeps someone else from completing this transfer
        // while this function is touching the transfer data structure.
        //
        transferData->TimeoutState = TTS_TimedOut;

        TR_Unlock(controlData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(controlData);
        }
    }
}

VOID
Control_WdfEvtRequestCancel(
    WDFREQUEST  WdfRequest
    )
{
    PCONTROL_DATA           controlData;
    BOOLEAN                 queueDpc;
    BOOLEAN                 reportCancel;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        queueDpc     = FALSE;
        reportCancel = FALSE;
        transferData = &(GetRequestData(WdfRequest)->ControlTransferData);
        controlData  = transferData->ControlData;

        LogInfo(Endpoint_GetRecorderLog(controlData->EndpointHandle), TransferRing,
                "%u.%u.0: WdfRequest 0x%p cancel requested",
                UsbDevice_GetSlotId(controlData->UsbDeviceHandle),
                Endpoint_GetContextIndex(controlData->EndpointHandle), WdfRequest);

        TR_Lock(controlData);

        if (transferData->CancelState == TCS_Cancelable) {

            if (controlData->Flags.OkToReclaimTransfersOnCancel) {
                queueDpc = TRUE;
            } else {
                reportCancel = TRUE;
            }
        }

        TR_Unlock(controlData);

        if (reportCancel) {
            Endpoint_TR_TransferCanceled(controlData->EndpointHandle);
        }

        TR_Lock(controlData);

        //
        // WdfRequestUnmarkCancelable failed during completion of the transfer because
        // the Cancel callback was about to execute. Now that the Cancel callback has
        // arrived, continue with completion of the transfer.
        //
        if (transferData->CancelState == TCS_WaitingForCancelCallback) {

            if (reportCancel) {

                //
                // If Cancel is reported to ESM, we cannot complete the transfer until
                // the state machine OK's it. This is because as soon as we complete one
                // transfer, we may receive another one through EvtIoDefault, and if the
                // ESM OK's completion of transfer after that point, we may prematurely
                // complete the new transfer.
                //
                if ((controlData->Flags.ReclaimTransfersAndAcknowledge) ||
                    (controlData->Flags.OkToReclaimTransfersOnCancel)) {
                    queueDpc = TRUE;
                }

            } else {

                queueDpc = TRUE;
            }
        }

        //
        // Updating the CancelState last keeps someone else from completing this transfer
        // while this function is touching the transfer data structure.
        //
        transferData->CancelState = TCS_Canceled;

        TR_Unlock(controlData);

    } FINALLY {

        if (queueDpc) {
            TR_QueueDpcForTransferCompletion(controlData);
        }
    }
}

VOID
Control_WdfEvtDpcForTransferCompletion(
    WDFDPC WdfDpc
    )
/*++

Routine Description:

    This DPC completes a request that was found to be canceled right before
    completion. This DPC is queued when the cancel callback for the request
    arrives, to avoid completing the request synchronously.

--*/
{
    BOOLEAN         acknowledgeTransfersReclaimed;
    PCONTROL_DATA   controlData;
    NTSTATUS        status;
    WDFQUEUE        wdfQueue;

    acknowledgeTransfersReclaimed = FALSE;
    wdfQueue                      = WdfDpcGetParentObject(WdfDpc);
    controlData                   = (PCONTROL_DATA)TR_GetContext(wdfQueue);

    TR_Lock(controlData);

    if (controlData->TransferData) {

        //
        // Transfer completion can fail if a Cancel or Timer callback is just
        // about to execute as we try to complete the transfer. In that case,
        // the Transfer will get completed when the Cancel or Timer callback
        // arrives.
        //
        status = Control_Transfer_CompleteCancelable(controlData);

        if (NT_SUCCESS(status)) {

            if (controlData->Flags.ReclaimTransfersAndAcknowledge) {
                acknowledgeTransfersReclaimed = TRUE;
            }
        }
    }

    TR_Unlock(controlData);

    if (acknowledgeTransfersReclaimed) {
        TR_TransfersReclaimed(controlData);
    }

    return;
}

VOID
Control_WdfDpcForCanceledOnQueueTransferCompletion(
    WDFDPC WdfDpc
    )
/*++

Routine Description:

    Completes requests that got canceled on queue. Requests canceled on queue
    must be completed from Dpc for compatibility reasons.

--*/
{
    LIST_ENTRY              completionList;
    PCONTROL_DATA           controlData;
    PCONTROL_TRANSFER_DATA  transferData;
    WDFQUEUE                wdfQueue;

    TRY {

        wdfQueue    = WdfDpcGetParentObject(WdfDpc);
        controlData = (PCONTROL_DATA)TR_GetContext(wdfQueue);

        InitializeListHead(&completionList);

        TR_Lock(controlData);

        if (controlData->DpcRunning) {
            TR_Unlock(controlData);
            LEAVE;
        }

        controlData->DpcRunning = TRUE;

        while (IsListEmpty(&controlData->CanceledOnQueueTransferList) == FALSE) {

            transferData =
                (PCONTROL_TRANSFER_DATA)RemoveHeadList(&controlData->CanceledOnQueueTransferList);

            InitializeListHead(&transferData->ListEntry);

            Control_Transfer_Complete(controlData, transferData);
        }

        controlData->DpcRunning = FALSE;

        TR_Unlock(controlData);

    } FINALLY {

    }

    return;
}

_Requires_lock_held_(ControlData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Control_Transfer_Complete(
    __in
        PCONTROL_DATA           ControlData,
    __in
        PCONTROL_TRANSFER_DATA  TransferData
    )
{
    ULONG           bytesTransferred;
    PTRANSFER_URB   transferUrb;
    NTSTATUS        wdfRequestCompletionStatus;

    TR_Unlock(ControlData);

    transferUrb = TransferData->TransferUrb;

    if (TransferData->TrbCompletionCode == TRB_COMPLETION_CODE_INVALID) {

        //
        // This is the case when transfer is being completed due to cancel or timeout.
        //
        if ((TransferData->CancelState == TCS_Canceled) || (TransferData->TimeoutState == TTS_TimedOut)) {
            TransferData->Status = STATUS_CANCELLED;
        }

        transferUrb->Header.Status =
            NTSTATUS_To_USBD_STATUS(TransferData->Status);

    } else {

        transferUrb->Header.Status =
            TR_GetUsbdStatusFromTrbCompletionCode(TransferData->TrbCompletionCode,
                                                  USBD_STATUS_NOT_SET);

        TransferData->Status =
            TR_GetNtStatusFromUsbdStatus(transferUrb->Header.Status);
    }

    wdfRequestCompletionStatus        = TransferData->Status;
    bytesTransferred                  = TransferData->BytesTransferred;
    transferUrb->TransferBufferLength = bytesTransferred;

    LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
            "%u.%u.0: WdfRequest 0x%p completed with %!STATUS! BytesTransferred %u BytesTotal %u",
            UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
            Endpoint_GetContextIndex(ControlData->EndpointHandle), TransferData->WdfRequest,
            wdfRequestCompletionStatus, TransferData->BytesTotal, bytesTransferred);

    Control_TransferData_Free(ControlData, TransferData);

    WdfRequestComplete(TransferData->WdfRequest, wdfRequestCompletionStatus);

    TR_Lock(ControlData);

    //
    // Update performance counters.
    //
    ControlData->Counter.TransferCount    += 1;
    ControlData->Counter.BytesTransferred += bytesTransferred;

    if (!NT_SUCCESS(wdfRequestCompletionStatus)) {
        ControlData->Counter.FailedTransferCount += 1;
    }
}

_Requires_lock_held_(ControlData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Control_Transfer_CompleteCancelable(
    __in
        PCONTROL_DATA ControlData
    )
{
    NTSTATUS                status;
    PCONTROL_TRANSFER_DATA  transferData;

    TRY {

        NT_ASSERTMSG("Request not getting completed at DISPATCH", (KeGetCurrentIrql() == DISPATCH_LEVEL));

        transferData = ControlData->TransferData;

        if (transferData->CancelState == TCS_Cancelable) {

            status = WdfRequestUnmarkCancelable(transferData->WdfRequest);

            if (!NT_SUCCESS(status)) {

                NT_ASSERT(status == STATUS_CANCELLED);

                transferData->CancelState = TCS_WaitingForCancelCallback;

                LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfRequestUnmarkCancelable returned %!STATUS!",
                        UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(ControlData->EndpointHandle), status);
                LEAVE;
            }

            transferData->CancelState = TCS_NotCancelable;
        }

        if (transferData->TimeoutState == TTS_TimeoutSet) {

            if (WdfTimerStop(ControlData->WdfTimerForTransferTimeout, FALSE) == FALSE) {

                //
                // N.B: STATUS_TIMEOUT should not be returned because it is not a failure code.
                //
                status = STATUS_CANCELLED;

                transferData->TimeoutState = TTS_WaitingForTimerCallback;

                LogInfo(Endpoint_GetRecorderLog(ControlData->EndpointHandle), TransferRing,
                        "%u.%u.0: WdfTimerStop returned FALSE",
                        UsbDevice_GetSlotId(ControlData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(ControlData->EndpointHandle));
                LEAVE;
            }

            transferData->TimeoutState = TTS_NoTimeout;
        }

        ControlData->TransferData = NULL;

        Control_Transfer_Complete(ControlData, transferData);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}
