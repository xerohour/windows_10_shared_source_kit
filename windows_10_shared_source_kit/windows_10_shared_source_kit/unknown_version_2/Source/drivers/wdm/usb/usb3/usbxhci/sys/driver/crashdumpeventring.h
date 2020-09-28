/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpeventring.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#define CRASHDUMP_EVENTRING_POLL_WAIT_TIME      200     // 200us
#define CRASHDUMP_EVENTRING_SIZE                (63 * sizeof(TRB))

typedef struct _CRASHDUMP_EVENTRING_DATA {

    HANDLE                      DumperHandle;
    HANDLE                      RegisterHandle;

    PINTERRUPTER_REGISTER_SET   InterrupterRegister;

    CRASHDUMP_BUFFER_DATA       EventRingTableBuffer;
    CRASHDUMP_BUFFER_DATA       EventRingSegmentBuffer;

    PTRB                        TRBArray;

    ULONG                       UsedEventRingSegments;

    ULONG                       TRBsPerSegment;

    ULONG                       CycleState;

    ULONG                       DequeueIndex;
    ULONG                       DequeueSegment;

    PBUFFER_DATA                CurrentBufferData;

    LIST_ENTRY                  BufferDataList;

} CRASHDUMP_EVENTRING_DATA, *PCRASHDUMP_EVENTRING_DATA;

typedef
NTSTATUS
DUMP_EVENTRING_CALLBACK(
    __inout
        PVOID   Context,
    __in
        PTRB    Trb,
    __out
        BOOLEAN *Continue
    );

typedef DUMP_EVENTRING_CALLBACK *PDUMP_EVENTRING_CALLBACK;

//
// Public functions.
//
__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_EventRing_InitializeForDump(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      DumperHandle
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_EventRing_InitializeForHibernate(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      DumperHandle
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForDump(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForHibernate(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      PrimaryInterrupterData
    );


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
    );

//
// Private functions.
//
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_UpdateDequeuePointer(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_EventRing_FreeDumpData(
    __inout
        PCRASHDUMP_EVENTRING_DATA   EventRingData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Crashdump_EventRing_PrepareForSaveState(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        HANDLE                      PrimaryInterrupterData
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_EventRing_AsyncPoll(
    __in
        PCRASHDUMP_EVENTRING_DATA   EventRingData,
    __in
        PURB                        *BulkUrb
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
Crashdump_EventRing_SetCompletionStatusInUrb(
    __in
        PTRB   Trb,
    __inout
        PURB   *TransferUrb
    )
/*++

Routine Description:

    This function set the USBD status in the transfer URB based on the xHCI completion
    code or the passed in status and returns the corresponding NTSTATUS value.

--*/
{
    NTSTATUS        status;
    PTRANSFER_URB   transferUrb;

    //
    // Update the number of bytes transferred in the URB.
    //
    transferUrb = (PTRANSFER_URB)*TransferUrb;

    //
    // In failure case let this field remain 0
    //
    transferUrb->TransferBufferLength = 0;

    switch (Trb->TransferEvent.CompletionCode) {

    case TRB_COMPLETION_CODE_SUCCESS:

        if (Trb->TransferEvent.EventData == 1) {
            transferUrb->TransferBufferLength = Trb->TransferEvent.TransferLength;
            transferUrb->Header.Status = USBD_STATUS_SUCCESS;
            status = STATUS_SUCCESS;
        } else {
            transferUrb->Header.Status = USBD_STATUS_XACT_ERROR;
            status = STATUS_UNSUCCESSFUL;
        }
        break;

    case TRB_COMPLETION_CODE_DATA_BUFFER_ERROR:

        transferUrb->Header.Status = USBD_STATUS_DATA_BUFFER_ERROR;
        status = STATUS_UNSUCCESSFUL;
        break;

    case TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR:

        transferUrb->Header.Status = USBD_STATUS_BABBLE_DETECTED;
        status = STATUS_UNSUCCESSFUL;
        break;

    case TRB_COMPLETION_CODE_STALL_ERROR:

        transferUrb->Header.Status = USBD_STATUS_STALL_PID;
        status = STATUS_UNSUCCESSFUL;
        break;

    case TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR:
    case TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR:

        transferUrb->Header.Status = USBD_STATUS_XACT_ERROR;
        status = STATUS_UNSUCCESSFUL;
        break;

    case TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR:

        transferUrb->Header.Status = USBD_STATUS_ISO_TD_ERROR;
        status = STATUS_UNSUCCESSFUL;
        break;

    case TRB_COMPLETION_CODE_SHORT_PACKET:

        transferUrb->TransferBufferLength = Trb->TransferEvent.TransferLength;
        transferUrb->Header.Status = USBD_STATUS_SUCCESS;
        status = STATUS_SUCCESS;
        break;

    default:
        transferUrb->Header.Status = USBD_STATUS_INTERNAL_HC_ERROR;
        status = STATUS_UNSUCCESSFUL;
        break;
    }

    CrashdumpLogInfo("URB Status %u: URB Transfer Length %u, TRB Completion Code: %u, status =0x%X",
                     transferUrb->Header.Status,
                     transferUrb->TransferBufferLength,
                     Trb->TransferEvent.CompletionCode,
                     status);

    return status;
}

