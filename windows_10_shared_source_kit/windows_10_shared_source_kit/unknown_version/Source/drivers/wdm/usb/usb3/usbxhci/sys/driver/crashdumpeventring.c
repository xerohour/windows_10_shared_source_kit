/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpeventring.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_EventRing_InitializeForHibernate(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      DumperHandle
    )
{
    TRY {

        EventRingData->DumperHandle = DumperHandle;
        EventRingData->RegisterHandle = Crashdump_GetRegisterHandle(DumperHandle);

        EventRingData->InterrupterRegister =
            Crashdump_Register_GetPrimaryInterrupterRegister(EventRingData->RegisterHandle);

    } FINALLY {

    }
}

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_EventRing_InitializeForDump(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      DumperHandle
    )
{
    NTSTATUS                status;

    TRY {

        EventRingData->DumperHandle = DumperHandle;
        EventRingData->RegisterHandle = Crashdump_GetRegisterHandle(DumperHandle);

        EventRingData->InterrupterRegister =
            Crashdump_Register_GetPrimaryInterrupterRegister(EventRingData->RegisterHandle);

        //
        // N.B: Allocating 63 TRB event ring may seem conservatively large,
        //      but it is done to handle a burst of unsolicited events.
        //
        status = Crashdump_CommonBufferAcquire(EventRingData->DumperHandle,
                                               CRASHDUMP_EVENTRING_SIZE,
                                               &EventRingData->EventRingSegmentBuffer);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = Crashdump_CommonBufferAcquire(EventRingData->DumperHandle,
                                               sizeof(EVENT_RING_SEGMENT_TABLE),
                                               &EventRingData->EventRingTableBuffer);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        EventRingData->CurrentBufferData = (PBUFFER_DATA) ExAllocatePoolWithTag(NonPagedPool,
                                                                                sizeof(BUFFER_DATA),
                                                                                CrashdumpAllocationTag);

        if (EventRingData->CurrentBufferData == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }


    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForDump(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    )
/*++

Routine Description:

    N.B: We do not touch the IMAN and IMOD registers.

--*/
{
    ERSTSZ                      eventRingSegmentSize;
    PEVENT_RING_SEGMENT_TABLE   eventRingSegmentTable;

    InitializeListHead(&EventRingData->BufferDataList);
    
    EventRingData->TRBArray = (PTRB)EventRingData->EventRingSegmentBuffer.VirtualAddress;

    EventRingData->CurrentBufferData->VirtualAddress = EventRingData->EventRingSegmentBuffer.VirtualAddress;
    EventRingData->CurrentBufferData->LogicalAddress = EventRingData->EventRingSegmentBuffer.LogicalAddress;
    EventRingData->CurrentBufferData->Size= EventRingData->EventRingSegmentBuffer.Size;
    InsertTailList(&EventRingData->BufferDataList, &EventRingData->CurrentBufferData->ListEntry);
    
    EventRingData->TRBsPerSegment = EventRingData->EventRingSegmentBuffer.Size / sizeof(TRB);

    EventRingData->CycleState = 1;

    EventRingData->DequeueIndex = 0;

    EventRingData->UsedEventRingSegments = 1;

    EventRingData->DequeueSegment = 0;


    RtlZeroMemory(EventRingData->EventRingTableBuffer.VirtualAddress,
                  EventRingData->EventRingTableBuffer.Size);

    RtlZeroMemory(EventRingData->EventRingSegmentBuffer.VirtualAddress,
                  EventRingData->EventRingSegmentBuffer.Size);

    //
    // 5.5.2.3.1 Event Ring Segment Table Size Register (ERSTSZ)
    //
    eventRingSegmentSize.AsUlong32 = 0;
    eventRingSegmentSize.Size = 1;

    Register_WriteUlong(
        &EventRingData->InterrupterRegister->EventRingSegmentTableSizeRegister.AsUlong32,
        eventRingSegmentSize.AsUlong32);

    Crashdump_EventRing_UpdateDequeuePointer(EventRingData);

    //
    // Initialize ERST of one segment.
    //
    eventRingSegmentTable =
        (PEVENT_RING_SEGMENT_TABLE)EventRingData->EventRingTableBuffer.VirtualAddress;

    eventRingSegmentTable[0].BaseAddress = EventRingData->EventRingSegmentBuffer.LogicalAddress.QuadPart;
    eventRingSegmentTable[0].SegmentSize = EventRingData->TRBsPerSegment;

    //
    // 5.5.2.3.2 Event Ring Segment Table Base Address Register (ERSTBA)
    //
    Register_WriteUlong64(
        &EventRingData->InterrupterRegister->EventRingSegmentTableBaseAddress.QuadPart,
        EventRingData->EventRingTableBuffer.LogicalAddress.QuadPart,
        (Crashdump_Register_GetDeviceFlags(EventRingData->RegisterHandle).Use32BitRegisterAccess == 1));

    CrashdumpLogInfo("Installed new Event Ring");
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForHibernate(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      PrimaryInterrupterData
    )
/*++

Routine Description:

    Copying the relevant fields from the Interrupter

--*/
{
    HANDLE         interrupterData;
    PBUFFER_DATA   currentBuffer;
    ULONG          i;

    //
    // We would be fine using just the first interrupter data. 
    //
    interrupterData = Interrupter_GetInterrupterData(PrimaryInterrupterData, 0);

    EventRingData->TRBArray = Interrupter_GetCurrentRingSegment(interrupterData);

    EventRingData->UsedEventRingSegments = Interrupter_GetUsedEventRingSegments(interrupterData);

    EventRingData->TRBsPerSegment = Interrupter_GetTRBsPerSegment(interrupterData);

    EventRingData->CycleState = Interrupter_GetCycleState(interrupterData);

    EventRingData->DequeueIndex = Interrupter_GetDequeueIndex(interrupterData);

    EventRingData->DequeueSegment = Interrupter_GetDequeueSegment(interrupterData);

    EventRingData->CurrentBufferData = Interrupter_GetCurrentBufferData(interrupterData);

    EventRingData->BufferDataList = Interrupter_GetBufferDataList(interrupterData);

    //
    // Lets mark the runtime stacks event ring for resume from hibernate.
    // This buffer is not really required to resume from hibernate, but Po 
    // checks any buffer used during hibernate when verifier is on.
    //
    currentBuffer = (PBUFFER_DATA)EventRingData->BufferDataList.Flink;
    for (i=0; i<EventRingData->UsedEventRingSegments; i++) {
        Crashdump_BufferMarkForHibernate(currentBuffer, sizeof(BUFFER_DATA));
        Crashdump_BufferMarkForHibernate(currentBuffer->VirtualAddress, currentBuffer->Size);
        currentBuffer = (PBUFFER_DATA)currentBuffer->ListEntry.Flink;
    }

    CrashdumpLogInfo("Got the Event Ring for hibernate");
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_EventRing_Poll(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        ULONG                       MatchTrbType,
    __in
        ULONG                       RetryCount,
    __in
        PVOID                       CallbackContext,
    __in
        PDUMP_EVENTRING_CALLBACK    Callback
    )
/*++

Routine Description:

    This routine will poll for the Event Ring to become non-empty.
    Then it will drain each event and invoke the Callback function.
    It will keep draining the events until the Callback returns FALSE,
    or a timeout is hit in polling for the Event Ring to become non-empty.

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    ULONG       count;
    ULONG       eventsDrained;
    PTRB        currentTrb;
    BOOLEAN     continueDraining;

    TRY {

        CrashdumpLogInfo("Crashdump_EventRing_Poll: begin");

        NT_ASSERT(RetryCount > 0);

        eventsDrained = 0;
        continueDraining = TRUE;

        do {

            //
            // Poll until there is atleast one event on the ring.
            //
            count = 0;
            currentTrb = &EventRingData->TRBArray[EventRingData->DequeueIndex];

            while ((count < RetryCount) &&
                   (currentTrb->TransferEvent.CycleBit != EventRingData->CycleState)) {

                KeStallExecutionProcessor(CRASHDUMP_EVENTRING_POLL_WAIT_TIME);
                count += 1;
            }

            if (count == RetryCount) {
                CrashdumpLogError("No more events available after %u retries, %u ms",
                                  count,
                                  (count * CRASHDUMP_EVENTRING_POLL_WAIT_TIME) / 1000);
                status = STATUS_IO_TIMEOUT;
                LEAVE;
            }

            CrashdumpLogInfo("Events available after %u retries, %u ms",
                             count,
                             ((count * CRASHDUMP_EVENTRING_POLL_WAIT_TIME) / 1000));

            //
            // Drain all events on the ring.
            //
            while ((continueDraining != FALSE) &&
                   (currentTrb->TransferEvent.CycleBit == EventRingData->CycleState)) {

                CrashdumpLogInfo("EventRing: CS: %u, EI: %03u, ET: %u, CC: %u, Len: %u, Addr: 0x%I64X",
                                 EventRingData->CycleState,
                                 EventRingData->DequeueIndex,
                                 currentTrb->TransferEvent.Type,
                                 currentTrb->TransferEvent.CompletionCode,
                                 currentTrb->TransferEvent.TransferLength,
                                 (IsEventDataTrb(currentTrb) ? 0 : currentTrb->TransferEvent.Pointer.QuadPart));

                if (TrbTypeMatch(currentTrb, MatchTrbType)) {

                    status = Callback(CallbackContext, currentTrb, &continueDraining);

                    //
                    // Even for a failure we need to advance the Dequeue Index
                    //
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogInfo("Callback failed with ntStatus 0x%x, ContinueDraining %u",
                                         status,
                                         continueDraining);
                    }

                } else if (TrbTypeMatch(currentTrb, TRB_TYPE_PORT_STATUS_CHANGE_EVENT)) {

                    PORTSC portSC;
                    ULONG  portNumber;

                    portNumber = currentTrb->PortStatusChangeEvent.PortId;
                    Crashdump_Register_GetPortSC(EventRingData->RegisterHandle, portNumber, &portSC);

                    CrashdumpLogInfo("Ignoring port status change event for port %u, CCS: %u, CSC: %u",
                                     portNumber,
                                     portSC.CurrentConnectStatus,
                                     portSC.ConnectStatusChange);

                } else {

                    // 
                    // If the TRB we received was not of the type we expected, there is nothing 
                    // we can do about it. We would just skip this and move on to the next one.
                    //
                    CrashdumpLogWarning("Unsolicited event encountered: ET: %u, CC: %u",
                                        currentTrb->TransferEvent.Type,
                                        currentTrb->TransferEvent.CompletionCode);
                }

                //
                // The following code is making sure that the event ring is processed 
                // in the same way as the runtime stack. So that when we resume from S3
                // the event ring can be reused by the runtime stack
                //
                EventRingData->DequeueIndex++;

                if (EventRingData->DequeueIndex == EventRingData->TRBsPerSegment) {

                    EventRingData->DequeueIndex = 0;

                    EventRingData->DequeueSegment++;

                    if (EventRingData->DequeueSegment == EventRingData->UsedEventRingSegments) {

                        TOGGLE(EventRingData->CycleState);

                        EventRingData->DequeueSegment = 0;

                        //
                        // Set the current buffer data to point to the first entry in the
                        // buffer data list (this is the first event ring segment).
                        //
                        EventRingData->CurrentBufferData =
                            (PBUFFER_DATA)EventRingData->BufferDataList.Flink;

                    } else {

                        //
                        // Set the current buffer data to the next entry in the buffer
                        // data list.
                        //
                        EventRingData->CurrentBufferData =
                            (PBUFFER_DATA)EventRingData->CurrentBufferData->ListEntry.Flink;
                    }

                    //
                    // Set the TRB Array to reflect the current buffer data.
                    //
                    EventRingData->TRBArray =
                        (PTRB)EventRingData->CurrentBufferData->VirtualAddress;
                }

                eventsDrained += 1;
                currentTrb = &EventRingData->TRBArray[EventRingData->DequeueIndex];
                
                //
                // Failure is ignored above to advance the Dequeue index. This is the time to exit
                //
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }

            //
            // Clear the EHB and inform the xHC where we are at.
            //
            Crashdump_EventRing_UpdateDequeuePointer(EventRingData);

        } while (continueDraining != FALSE);

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_EventRing_Poll: end 0x%X, events drained %u", status, eventsDrained);
    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_UpdateDequeuePointer(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    )
/*++

Routine Description:

    This routine will update the Event Ring Dequeue Pointer (ERSP) and clear
    the EventHandlerBusy (EHB) bit by writing a 1 to it.

Reference:

    5.5.2.3.3   - Event Ring Dequeue Pointer Register (ERDP)

--*/
{
    ERDP    eventRingDequeuePointer;

    eventRingDequeuePointer.AsUlong64 = EventRingData->EventRingSegmentBuffer.LogicalAddress.QuadPart;
    eventRingDequeuePointer.AsUlong64 += (sizeof(TRB) * EventRingData->DequeueIndex);

    eventRingDequeuePointer.DequeueErstSegmentIndex = 0;
    eventRingDequeuePointer.EventHandlerBusy = 1;

    Register_WriteUlong64(
        &EventRingData->InterrupterRegister->EventRingDequeuePointerRegister.AsUlong64,
        eventRingDequeuePointer.AsUlong64,
        (Crashdump_Register_GetDeviceFlags(EventRingData->RegisterHandle).Use32BitRegisterAccess == 1));
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_EventRing_FreeDumpData(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    )
{
    if (EventRingData->CurrentBufferData != NULL) {
        ExFreePool(EventRingData->CurrentBufferData);
        EventRingData->CurrentBufferData = NULL;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForSaveState(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      PrimaryInterrupterData
    )
/*++

Routine Description:

    Reinitializing the Interrupter, this routine is only called for Hybrid Sleep
    when the system is still in S3

--*/
{
    
    HANDLE                      interrupterData;

    //
    // The event ring is shared between crashdump stack and runtime stack
    // Since we have modified the event ring, we need to update the runtime
    // stacks variables to indicate the current position of its variables
    //
    interrupterData = Interrupter_GetInterrupterData(PrimaryInterrupterData, 0);

    Interrupter_SetCurrentRingSegment(interrupterData, EventRingData->TRBArray);

    Interrupter_SetUsedEventRingSegments(interrupterData, EventRingData->UsedEventRingSegments);

    Interrupter_SetTRBsPerSegment(interrupterData, EventRingData->TRBsPerSegment);

    Interrupter_SetCycleState(interrupterData, EventRingData->CycleState);

    Interrupter_SetDequeueIndex(interrupterData, EventRingData->DequeueIndex);

    Interrupter_SetDequeueSegment(interrupterData, EventRingData->DequeueSegment);

    Interrupter_SetCurrentBufferData(interrupterData, EventRingData->CurrentBufferData);

    Interrupter_SetBufferDataList(interrupterData, EventRingData->BufferDataList);

}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_EventRing_AsyncPoll(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        PURB                        *BulkUrb
    )
/*++

Routine Description:

    This routine will check if there is an event TRB at the start of
    the event ring. If no event trb is found it will return STATUS_PENDING
    If event trb is found it will check for the status and return that

--*/
{
    NTSTATUS    status = STATUS_PENDING;
    PTRB        currentTrb;
    HANDLE      targetDevice;
    HANDLE      targetEndpoint;

    TRY {

        CrashdumpLogInfo("Crashdump_EventRing_Poll: begin");

        currentTrb = &EventRingData->TRBArray[EventRingData->DequeueIndex];

        if (currentTrb->TransferEvent.CycleBit != EventRingData->CycleState) {
            LEAVE;
        }

        CrashdumpLogInfo("EventRing: CS: %u, EI: %03u, ET: %u, CC: %u, Len: %u, Addr: 0x%I64X",
                         EventRingData->CycleState,
                         EventRingData->DequeueIndex,
                         currentTrb->TransferEvent.Type,
                         currentTrb->TransferEvent.CompletionCode,
                         currentTrb->TransferEvent.TransferLength,
                         (IsEventDataTrb(currentTrb) ? 0 : currentTrb->TransferEvent.Pointer.QuadPart));

        if (TrbTypeMatch(currentTrb, TRB_TYPE_TRANSFER_EVENT)) {

            //
            // For failure cases we will not get the TransferTag, but we need to
            // inform dump miniport about which transfer failed. We need to look
            // the endpoint up and pull the TransferTag from it
            //
            targetDevice = Crashdump_GetTargetDevice(EventRingData->DumperHandle);
            targetEndpoint = Crashdump_UsbDevice_GetEndpointHandle(targetDevice, currentTrb->TransferEvent.EndpointId);
            *BulkUrb = (PVOID)Crashdump_Endpoint_GetLastURB(targetEndpoint);

            Crashdump_Endpoint_DisableLastTransfer(targetEndpoint);
            
            Crashdump_Endpoint_IncrementTransferTag(targetEndpoint);

            status = Crashdump_EventRing_SetCompletionStatusInUrb(currentTrb, BulkUrb);


        } else if (TrbTypeMatch(currentTrb, TRB_TYPE_PORT_STATUS_CHANGE_EVENT)) {

            PORTSC portSC;
            ULONG  portNumber;

            portNumber = currentTrb->PortStatusChangeEvent.PortId;
            Crashdump_Register_GetPortSC(EventRingData->RegisterHandle, portNumber, &portSC);

            CrashdumpLogInfo("Ignoring port status change event for port %u, CCS: %u, CSC: %u",
                             portNumber,
                             portSC.CurrentConnectStatus,
                             portSC.ConnectStatusChange);
        } else {

            // 
            // If the TRB we received was not of the type we expected, there is nothing 
            // we can do about it. We would just skip this and move on to the next one.
            //
            CrashdumpLogWarning("Unsolicited event encountered: ET: %u, CC: %u",
                                currentTrb->TransferEvent.Type,
                                currentTrb->TransferEvent.CompletionCode);
        }

        //
        // The following code is making sure that the event ring is processed 
        // in the same way as the runtime stack. So that when we resume from S3
        // the event ring can be reused by the runtime stack
        //
        EventRingData->DequeueIndex++;

        if (EventRingData->DequeueIndex == EventRingData->TRBsPerSegment) {

            EventRingData->DequeueIndex = 0;

            EventRingData->DequeueSegment++;

            if (EventRingData->DequeueSegment == EventRingData->UsedEventRingSegments) {

                TOGGLE(EventRingData->CycleState);

                EventRingData->DequeueSegment = 0;

                //
                // Set the current buffer data to point to the first entry in the
                // buffer data list (this is the first event ring segment).
                //
                EventRingData->CurrentBufferData =
                    (PBUFFER_DATA)EventRingData->BufferDataList.Flink;

            } else {

                //
                // Set the current buffer data to the next entry in the buffer
                // data list.
                //
                EventRingData->CurrentBufferData =
                    (PBUFFER_DATA)EventRingData->CurrentBufferData->ListEntry.Flink;
            }

            //
            // Set the TRB Array to reflect the current buffer data.
            //
            EventRingData->TRBArray =
                (PTRB)EventRingData->CurrentBufferData->VirtualAddress;
        }
        
        //
        // Clear the EHB and inform the xHC where we are at.
        //
        Crashdump_EventRing_UpdateDequeuePointer(EventRingData);


    } FINALLY {

        CrashdumpLogInfo("Crashdump_EventRing_Poll: end 0x%X", status);
    }

    return status;
}


