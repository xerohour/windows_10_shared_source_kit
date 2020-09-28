/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpendpoint.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_Endpoint_Initialize(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        HANDLE                      DumperHandle,
    __in
        HANDLE                      UsbDeviceHandle,
    __in
        ULONG                       EndpointDCI,
    __in
        ULONG                       MaxTransferSize,
    __in
        PVOID                       DeviceContext
    )
{
    NTSTATUS        status;
    ULONG           segmentSize;
    PSTREAM_CONTEXT streamContextArray;
    ULONG           trbsPerSegment;

    TRY {

        EndpointData->DumperHandle = DumperHandle;
        EndpointData->RegisterHandle = Crashdump_GetRegisterHandle(EndpointData->DumperHandle);
        EndpointData->CommandHandle = Crashdump_GetCommandHandle(DumperHandle);
        EndpointData->UsbDeviceHandle = UsbDeviceHandle;
        EndpointData->EndpointDCI = EndpointDCI;

        EndpointData->TransferTag = 1;
        EndpointData->CurrentSegmentIndex = 0;

        Crashdump_Endpoint_SetOutputEndpointContext(EndpointData, DeviceContext);
        Crashdump_Endpoint_SetEndpointContext(EndpointData, DeviceContext);

        //
        // Calculate worst case TRBs per transfer, which is the same as TRBs per
        // segment, since only one transfer is programmed into a segment at once.
        //
        if (EndpointData->EndpointDCI == 1) {

            //
            // Default Control Endpoint.
            //
            trbsPerSegment = 1 +                                        // Setup Stage TRB.
                             1 +                                        // Data Stage TRB.
                          // 1 +                                        // Event Data TRB.
                             1 +                                        // Status Stage TRB.
                             1 +                                        // Event Data TRB.
                             1;                                         // Link TRB.

        } else {

            //
            // Other than the Default Control Endpoint, we only support Bulk Endpoints.
            //
            trbsPerSegment = BufferSizeToSpanPages(MaxTransferSize) +   // Normal TRBs.
                             1 +                                        // Event Data TRB.
                             1;                                         // Link TRB.
        }

        //
        // Acquire buffers for two Transfer Ring segments.
        //
        segmentSize = trbsPerSegment * sizeof(TRB);

        status = Crashdump_CommonBufferAcquire(EndpointData->DumperHandle,
                                               segmentSize,
                                               &EndpointData->TrbSegment[0]);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = Crashdump_CommonBufferAcquire(EndpointData->DumperHandle,
                                               segmentSize,
                                               &EndpointData->TrbSegment[1]);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (EndpointData->EndpointContext.MaxPrimaryStreams > 0) {

            EndpointData->StreamsCapable = TRUE;

            status = Crashdump_CommonBufferAcquire(EndpointData->DumperHandle,
                                                   CRASHDUMP_MIN_STREAM_CONTEXT_ARRAY_SIZE,
                                                   &EndpointData->StreamContextArray);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            streamContextArray = (PSTREAM_CONTEXT)EndpointData->StreamContextArray.VirtualAddress;

            streamContextArray[1].DequeuePointer = EndpointData->TrbSegment[0].LogicalAddress.QuadPart;
            streamContextArray[1].StreamContextType = STREAM_CONTEXT_TYPE_PRIMARY_TR;
            streamContextArray[1].DequeueCycleState = Crashdump_Endpoint_GetDequeueCycleState(EndpointData);
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_PrepareForHibernate(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
/*++

Routine Description:

    This routine will set up a circular Transfer Ring of two segments. Each
    segment will be big enough to hold a single transfer, and segments will
    be used alternately for each transfer. The last valid TRB in a segment
    will be a Link TRB that points to the first TRB of the alternate segment.
    The xHC will be informed to always look for CycleState=1 as there is no
    need to toggle the cycle bit with the two segment design.

Reference:

    4.9.2       - Transfer Ring Management.

--*/
{
    NTSTATUS        status;

    TRY {

        CrashdumpLogInfo("Crashdump_Endpoint_PrepareForHibernate: begin");

        RtlZeroMemory(EndpointData->TrbSegment[0].VirtualAddress,
                      EndpointData->TrbSegment[0].Size);

        RtlZeroMemory(EndpointData->TrbSegment[1].VirtualAddress,
                      EndpointData->TrbSegment[1].Size);


        EndpointData->TransferTag = 1;
        EndpointData->CurrentSegmentIndex = 0;

        //
        // Stop the endpoint before sending SetDequeuePointer command.
        //
        status = Crashdump_Endpoint_StopEndpoint(EndpointData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Endpoint_StopEndpoint failed with error 0x%X",status);
            LEAVE;
        }

        status = Crashdump_Endpoint_SetDequeuePointer(EndpointData);

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Endpoint_PrepareForHibernate: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendControlTransfer(
    __in
        PCRASHDUMP_ENDPOINT_DATA        EndpointData,
    __in
        PUSB_DEFAULT_PIPE_SETUP_PACKET  SetupPacket,
    __in
        ULONG                           DataBufferLength,
    __in_opt
        PPHYSICAL_ADDRESS               DataBuffer
    )
/*++

Routine Description:

    This routine will schedule a Control transfer by setting up a Setup Stage
    TRB, optional Data Stage TRB, and a Status Stage TRB. One Event Data TRB
    each will follow the Status and Data Stage TRBs to generate corresponding
    events on the Event Ring.

Reference:

    3.2.9       - Control Transfers.
    4.11.2.2    - Setup Stage, Data Stage, and Stagus Stage TRBs.
    4.11.5.2    - Event Data TRB.

--*/
{
    NTSTATUS            status;
    ULONG               index;
    PTRB                trb;
    PTRB                trbArray;
    PPHYSICAL_ADDRESS   alternateSegmentLA;
    ENDPOINT_DIRECTION  dataDirection;

    CrashdumpLogInfo("Crashdump_Endpoint_SendControlTransfer: begin");

    if (SetupPacket->bmRequestType.Dir == BMREQUEST_HOST_TO_DEVICE) {
         dataDirection = EndpointDirection_Out;
    } else {
         dataDirection = EndpointDirection_In;
    }

    if (EndpointData->CurrentSegmentIndex == 0) {
        trbArray = (PTRB)EndpointData->TrbSegment[0].VirtualAddress;
        alternateSegmentLA = &EndpointData->TrbSegment[1].LogicalAddress;
    } else {
        trbArray = (PTRB)EndpointData->TrbSegment[1].VirtualAddress;
        alternateSegmentLA = &EndpointData->TrbSegment[0].LogicalAddress;
    }

    index = 0;

    //
    // Setup TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));
    RtlCopyMemory(trb, SetupPacket, sizeof(USB_DEFAULT_PIPE_SETUP_PACKET));

    trb->SetupStage.TransferLength = sizeof(USB_DEFAULT_PIPE_SETUP_PACKET);
    trb->SetupStage.ImmediateData = 1;
    trb->SetupStage.Type = TRB_TYPE_SETUP_STAGE;

    //
    // Data Stage TRB.
    //
    if (DataBufferLength > 0) {

        NT_ASSERT(DataBufferLength <= PAGE_SIZE);

        trb = &trbArray[index++];

        RtlZeroMemory(trb, sizeof(TRB));

        trb->DataStage.Type = TRB_TYPE_DATA_STAGE;
        trb->DataStage.Direction = dataDirection;
        trb->DataStage.DataBufferPointer = DataBuffer->QuadPart;
        trb->DataStage.TransferLength = DataBufferLength;
        trb->DataStage.CycleBit = 1;

        /*
        //
        // Run-time stack adds an Event Data TRB here, but we will just
        // rely on successful completion of the Status Stage.
        //
        trb->DataStage.ChainBit = 1;
        trb->DataStage.EvaluateNextTrb = 1;

        trb = &trbArray[index++];

        RtlZeroMemory(trb, sizeof(TRB));

        trb->EventData.Type = TRB_TYPE_EVENT_DATA;
        trb->EventData.Data = EndpointData->TransferTag++;
        trb->EventData.InterrupterTarget = 0;
        trb->EventData.InterruptOnCompletion = 1;
        trb->EventData.CycleBit = 1;
        */
    }

    //
    // Status TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));

    trb->StatusStage.Type = TRB_TYPE_STATUS_STAGE;
    trb->StatusStage.ChainBit = 1;
    trb->StatusStage.CycleBit = 1;
    trb->StatusStage.Direction = (dataDirection == EndpointDirection_Out) ?
                                 EndpointDirection_In :
                                 EndpointDirection_Out;

    //
    // Event Data TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));

    trb->EventData.Type = TRB_TYPE_EVENT_DATA;
    trb->EventData.Data = EndpointData->TransferTag;
    trb->EventData.InterrupterTarget = 0;
    trb->EventData.InterruptOnCompletion = 1;
    trb->EventData.CycleBit = 1;

    //
    // Link TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));

    trb->Link.Type = TRB_TYPE_LINK;
    trb->Link.RingSegmentPointer = alternateSegmentLA->QuadPart;
    trb->Link.ToggleCycle = 0;
    trb->Link.CycleBit = 1;

    //
    // Switch to alternate segment for next transfer.
    //
    TOGGLE(EndpointData->CurrentSegmentIndex);

    //
    // Commit the CycleBit last.
    //
    Register_ToggleTrbCycleBit(&trbArray[0]);

    status = Crashdump_Endpoint_SendTransferAndPollForCompletion(EndpointData, 20000);

    trbArray[0].TransferEvent.CycleBit = 0;

    CrashdumpLogInfo("Crashdump_Endpoint_SendControlTransfer: end 0x%X", status);

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendBulkTransfer(
    __in
        PCRASHDUMP_ENDPOINT_DATA        EndpointData,
    __in
        PURB_BULK_OR_INTERRUPT_TRANSFER BulkUrb,
    __in
        PPHYSICAL_ADDRESS               TransferPAArray,
    __in
        BOOLEAN                         IsTransferAsync
    )
/*++

Routine Description:

    This routine will schedule a bulk transfer by setting up one or more Normal
    TRBs, followed by an Event Data TRB to generate a completion event on the
    Event Ring.

Reference:

    3.2.10      - Bulk and Interrupt Transfers.
    4.11.2.1    - Normal TRB.
    4.11.5.2    - Event Data TRB.

--*/
{
    NTSTATUS            status;
    ULONG               index;
    ULONG               thisTransferLength;
    ULONG               remainingBufferLength;
    PTRB                trb;
    PTRB                trbArray;
    PPHYSICAL_ADDRESS   alternateSegmentLA;

    CrashdumpLogInfo("Crashdump_Endpoint_SendBulkTransfer: begin: length: 0x%X TransferPA: 0x%I64X",
                     BulkUrb->TransferBufferLength,
                     TransferPAArray->QuadPart);

    if (EndpointData->CurrentSegmentIndex == 0) {
        trbArray = (PTRB)EndpointData->TrbSegment[0].VirtualAddress;
        alternateSegmentLA = &EndpointData->TrbSegment[1].LogicalAddress;
    } else {
        trbArray = (PTRB)EndpointData->TrbSegment[1].VirtualAddress;
        alternateSegmentLA = &EndpointData->TrbSegment[0].LogicalAddress;
    }

    index = 0;
    remainingBufferLength = BulkUrb->TransferBufferLength;
    EndpointData->LastUrb = (PURB)BulkUrb;

    while (remainingBufferLength > 0) {

        if (IsPageAligned(TransferPAArray->QuadPart)) {
            thisTransferLength = Min(remainingBufferLength, PAGE_SIZE);
        } else {
            thisTransferLength = Min(remainingBufferLength, PageBytesLeft(TransferPAArray->QuadPart));
        }

        trb = &trbArray[index];

        RtlZeroMemory(trb, sizeof(TRB));

        trb->Normal.Type = TRB_TYPE_NORMAL;
        trb->Normal.ChainBit = 1;
        trb->Normal.DataBufferPointer = TransferPAArray->QuadPart;
        trb->Normal.TransferLength = thisTransferLength;
        trb->Normal.CycleBit = ((index == 0) ? 0 : 1);

        CrashdumpLogInfo("Normal TRB %u: PA: 0x%I64X, length: 0x%X",
                         index,
                         TransferPAArray->QuadPart,
                         thisTransferLength);

        index++;
        TransferPAArray += 1;
        remainingBufferLength -= thisTransferLength;
    }

    //
    // 4.12.3 - Last Normal TRB should have ENT set if it is followed by
    // an Event Data TRB.
    //
    trbArray[index - 1].Normal.EvaluateNextTrb = 1;

    //
    // Event Data TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));

    trb->EventData.Type = TRB_TYPE_EVENT_DATA;
    trb->EventData.Data = EndpointData->TransferTag;
    trb->EventData.InterrupterTarget = 0;
    trb->EventData.InterruptOnCompletion = 1;
    trb->EventData.CycleBit = 1;

    //
    // Link TRB.
    //
    trb = &trbArray[index++];

    RtlZeroMemory(trb, sizeof(TRB));

    trb->Link.Type = TRB_TYPE_LINK;
    trb->Link.RingSegmentPointer = alternateSegmentLA->QuadPart;
    trb->Link.ToggleCycle = 0;
    trb->Link.CycleBit = 1;

    //
    // Switch to alternate segment for next transfer.
    //
    TOGGLE(EndpointData->CurrentSegmentIndex);

    //
    // Commit the CycleBit last.
    //
    Register_ToggleTrbCycleBit(&trbArray[0]);

    if (IsTransferAsync) {
        Crashdump_UsbDevice_WriteDoorBell(EndpointData->UsbDeviceHandle,
                                          EndpointData->EndpointDCI,
                                          Crashdump_Endpoint_GetStreamId(EndpointData));
        status = STATUS_PENDING;
    } else {
        status = Crashdump_Endpoint_SendTransferAndPollForCompletion(EndpointData, 20000);
        trbArray[0].TransferEvent.CycleBit = 0;
    }

    CrashdumpLogInfo("Crashdump_Endpoint_SendBulkTransfer: end 0x%X", status);

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendTransferAndPollForCompletion(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        ULONG                       RetryCount
    )
{
    NTSTATUS    status;

    //
    // Ring the doorbell.
    //
    Crashdump_UsbDevice_WriteDoorBell(EndpointData->UsbDeviceHandle,
                                      EndpointData->EndpointDCI,
                                      Crashdump_Endpoint_GetStreamId(EndpointData));

    //
    // Zero out the last transfer event, so we can validate that the
    // callback function received a new Event Data TRB.
    //
    RtlZeroMemory(&EndpointData->LastTransferEvent, sizeof(TRB));

    //
    // Poll for completion.
    //
    status = Crashdump_EventRing_Poll(Crashdump_GetEventRingHandle(EndpointData->DumperHandle),
                                      TRB_TYPE_TRANSFER_EVENT,
                                      RetryCount,
                                      EndpointData,
                                      Crashdump_Endpoint_EventRingCallback);

    if (NT_SUCCESS(status)) {

        if (EndpointData->LastTransferEvent.EventData != 1) {

            CrashdumpLogError("Did not receive an Event Data Trb");
            status = STATUS_UNSUCCESSFUL;

        } else {

            if (EndpointData->LastTransferEvent.Pointer.QuadPart != EndpointData->TransferTag) {

                CrashdumpLogError("Event Data Trb received is not the one we are looking for");
                status = STATUS_UNSUCCESSFUL;

            } else {

                CrashdumpLogInfo("Received the correct Event Data Trb.");
            }
        }
    }

    Crashdump_Endpoint_IncrementTransferTag(EndpointData);

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_EventRingCallback(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        PTRB                        Trb,
    __out
        BOOLEAN                     *Continue
    )
{
    NTSTATUS status;
    TRY {
        *Continue = TRUE;

        if (Trb->TransferEvent.CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {
            CrashdumpLogError("Event Data Trb was not successful Error=%x", Trb->TransferEvent.CompletionCode);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (IsEventDataTrb(Trb)) {

            if (Trb->TransferEvent.Pointer.QuadPart == EndpointData->TransferTag) {

                RtlCopyMemory(&EndpointData->LastTransferEvent, Trb, sizeof(TRB));
                *Continue = FALSE;
            } else {
                CrashdumpLogError("Event Data Trb was not the one we were looking for.");
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }
        }
        
        status = STATUS_SUCCESS;
    } FINALLY {
    }
    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_StopEndpoint(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
/*++

Routine Description:

    This routine will bring the specified Endpoint to the Stopped state.

Reference:

    4.6.9       - Stop Endpoint.
    6.4.3.8     - Stop Endpoint Command TRB.

--*/
{
    NTSTATUS    status;
    TRB         trb;

    TRY {

        CrashdumpLogInfo("Crashdump_Endpoint_StopEndpoint: begin");

        //
        // Check if endpoint needs to be stopped.
        //
        switch (Crashdump_Endpoint_GetEndpointState(EndpointData)) {

            case ENDPOINT_STATE_STOPPED:
                CrashdumpLogInfo("Endpoint %u is currently stopped", EndpointData->EndpointDCI);
                status = STATUS_SUCCESS;
                LEAVE;

            case ENDPOINT_STATE_RUNNING:
                CrashdumpLogInfo("Endpoint %u running, stopping now", EndpointData->EndpointDCI);
                break;

            default:
                CrashdumpLogError("Endpoint %u is in unknown state", EndpointData->EndpointDCI);
                status = STATUS_ADAPTER_HARDWARE_ERROR;
                LEAVE;
        }

        //
        // Send a stop endpoint command.
        //
        RtlZeroMemory(&trb, sizeof(TRB));

        trb.StopEndpointCommand.Type = TRB_TYPE_STOP_ENDPOINT_COMMAND;
        trb.StopEndpointCommand.EndpointId = EndpointData->EndpointDCI;
        trb.StopEndpointCommand.SlotId = Crashdump_UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

        status = Crashdump_Command_SendCommand(EndpointData->CommandHandle, &trb, NULL);

        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Endpoint %u: stop endpoint command completion failure", EndpointData->EndpointDCI);
            LEAVE;
        }

        //
        // Verify that the endpoint is stopped.
        //
        if (Crashdump_Endpoint_GetEndpointState(EndpointData) != ENDPOINT_STATE_STOPPED) {

            CrashdumpLogError("Endpoint %u: endpoint still not stopped", EndpointData->EndpointDCI);
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Endpoint_StopEndpoint: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SetDequeuePointer(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
/*++

Routine Description:

    This routine will program the dequeue pointer for the Transfer Ring.

Reference:

    4.6.10      - Set TR Dequeue Pointer.
    6.4.3.9     - Set TR Dequeue Pointer Command TRB.

--*/
{
    NTSTATUS        status;
    TRB             trb;
    PSTREAM_CONTEXT streamContextArray;

    TRY {

        CrashdumpLogInfo("Crashdump_Endpoint_SetDequeuePointer: begin");

        NT_ASSERTMSG("SetDequeuePointer for streams not implemented", EndpointData->StreamsCapable == FALSE);

        RtlZeroMemory(&trb, sizeof(TRB));

        trb.SetDequeuePointerCommand.Type = TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND;
        trb.SetDequeuePointerCommand.EndpointId = EndpointData->EndpointDCI;
        trb.SetDequeuePointerCommand.SlotId = Crashdump_UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

        //
        // If the endpoint is streams capable, current implementation will
        // always use only 1 stream.
        //
        if (EndpointData->StreamsCapable) {
            
            streamContextArray = (PSTREAM_CONTEXT)EndpointData->StreamContextArray.VirtualAddress;
            trb.SetDequeuePointerCommand.DequeuePointer = streamContextArray[1].DequeuePointer;
            trb.SetDequeuePointerCommand.StreamId = 1;
            
        } else {
        
            trb.SetDequeuePointerCommand.DequeuePointer = Crashdump_Endpoint_GetStreamOrTransferRingLA(EndpointData);
            trb.SetDequeuePointerCommand.DequeueCycleState = Crashdump_Endpoint_GetDequeueCycleState(EndpointData);

        }

        status = Crashdump_Command_SendCommand(EndpointData->CommandHandle, &trb, NULL);

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Endpoint_SetDequeuePointer: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_Cleanup(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
/*++

Routine Description:

    This routine will stop all the endpoints

Reference:


--*/
{
    NTSTATUS        status;

    TRY {

        CrashdumpLogInfo("Crashdump_Endpoint_Cleanup: begin");

        EndpointData->TransferTag = 1;
        EndpointData->CurrentSegmentIndex = 0;

        //
        // Stop the endpoint
        //
        status = Crashdump_Endpoint_StopEndpoint(EndpointData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Endpoint_Cleanup: end 0x%X", status);
    }

    return status;
}
