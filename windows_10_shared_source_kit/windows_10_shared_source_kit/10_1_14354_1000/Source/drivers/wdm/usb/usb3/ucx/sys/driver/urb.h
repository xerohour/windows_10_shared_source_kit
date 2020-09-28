//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

#define USBPORT_EtwWriteHcData3(a, b, c, d, e, f, g, h, i)
#define USBPORT_EtwWriteHcDeviceData3(a, b, c, d, e, f, g, h, i, j)
#define USBPORT_EtwWriteHcDeviceEndpointData3(a, b, c, d, e, f, g, h, i, j, k)
#define LOGENTRY(a, b, c, d, e, f)
#define USBPORT_ASSERT NT_ASSERT

#define MAXIMUM_CONTROL_TRANSFER_SIZE 0xFFFF

struct _UCX_URB_DATA {
    ULONG ProcessorNumber;
#ifdef WIN64
    ULONG Padding;
#endif
    PVOID Reserved7[7];
};

typedef struct _TRANSFER_URB {

    struct _URB_HEADER Hdr;

    PVOID UsbdPipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;
    union {
        ULONG Timeout;              // no Linked Urbs
        PVOID ReservedMBNull;       // fields
    };
    struct _UCX_URB_DATA UrbData;   // fields for HCD use

    union {
        struct {
            ULONG StartFrame;
            ULONG NumberOfPackets;
            ULONG ErrorCount;
            USBD_ISO_PACKET_DESCRIPTOR IsoPacket[1];
        } Isoch;
        UCHAR SetupPacket[8];
    } u;

} TRANSFER_URB, *PTRANSFER_URB;

IO_COMPLETION_ROUTINE Urb_LegacyTransferUrbCompletion;

FORCEINLINE
VOID
SET_TRANSFER_DIRECTION_IN(
    PULONG TransferFlags
    )
/*++

Routine Description:

    Sets the USBD_TRANSFER_DIRECTION_IN in the transfer flags

Arguments:

    TransferFlags - Pointer to the Transfer Flags

Return Value:

    VOID
--*/
{
    (*TransferFlags) |= USBD_TRANSFER_DIRECTION_IN;
}

FORCEINLINE
VOID
SET_TRANSFER_DIRECTION_OUT(
    PULONG TransferFlags
    )
/*++

Routine Description:

    Sets the USBD_TRANSFER_DIRECTION_OUT in the transfer flags

Arguments:

    TransferFlags - Pointer to the Transfer Flags

Return Value:

    VOID
--*/
{
    (*TransferFlags) &= ~USBD_TRANSFER_DIRECTION_IN;
}

FORCEINLINE
NTSTATUS
UcxForwardIrpToQueueWithCompletion(
    __in
        WDFDEVICE               RHPdo,
    __in
        PIRP                    Irp,
    __in
        PIO_COMPLETION_ROUTINE  CompletionRoutine,
    __in_opt
        PVOID                   Context,
    __in
        WDFQUEUE                WdfQueue
    )
{
    NTSTATUS                status;

    TRY {

        IoMarkIrpPending(Irp);

        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutineSmart(WdfDeviceWdmGetDeviceObject(RHPdo),
                                    Irp, CompletionRoutine, Context,
                                    TRUE, TRUE, TRUE);

        //
        // It is OK to not check the return value since we are using our own stack location.
        //
        #pragma warning (suppress:6031) //  Return value ignored
        WdfDeviceWdmDispatchIrpToIoQueue(
            RHPdo,
            Irp,
            WdfQueue,
            WDF_DISPATCH_IRP_TO_IO_QUEUE_PREPROCESSED_IRP);

       status = STATUS_PENDING;

    } FINALLY {

    }
    return status;
}

FORCEINLINE
NTSTATUS
UcxForwardIrpToQueueWithCompletionUsingRequest(
    __in
        WDFDEVICE               RHPdo,
    __in
        PIRP                    Irp,
    __in
        WDFREQUEST              Request,
    __in
        PIO_COMPLETION_ROUTINE  CompletionRoutine,
    __in_opt
        PVOID                   Context,
    __in
        WDFQUEUE                WdfQueue
    )
{
    NTSTATUS                    status;
    WDF_REQUEST_FORWARD_OPTIONS forwardOptions;

    TRY {

        IoMarkIrpPending(Irp);

        IoCopyCurrentIrpStackLocationToNext(Irp);

        IoSetCompletionRoutineSmart(WdfDeviceWdmGetDeviceObject(RHPdo),
                                    Irp, CompletionRoutine, Context,
                                    TRUE, TRUE, TRUE);

        //
        // Update current stack pointer.
        //
        IoSetNextIrpStackLocation(Irp);

        WDF_REQUEST_FORWARD_OPTIONS_INIT(&forwardOptions);

        status = WdfRequestForwardToParentDeviceIoQueue(
            Request,
            WdfQueue,
            &forwardOptions);

        if (!NT_SUCCESS(status)) {
            if (status == STATUS_WDF_BUSY) {
                //
                // KMDF converts STATUS_WDF_BUSY to a warning status STATUS_DEVICE_BUSY.
                // However, we really want to return an error status in this case.
                //
                // Reasons for the target queue to be purged include: the controller may in reset or
                // the client driver may be concurrently resetting the pipe (client driver error).
                // There is not currently a need for clients to distinguish these reasons.
                //
                status = STATUS_NO_SUCH_DEVICE;
            }
            WdfRequestComplete(Request, status);
        }

        //
        // For compat still return status Pending from this routine.
        //
        status = STATUS_PENDING;

    } FINALLY {

    }
    return status;
}


