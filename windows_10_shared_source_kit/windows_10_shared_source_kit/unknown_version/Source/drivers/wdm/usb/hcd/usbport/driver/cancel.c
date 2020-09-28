/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    cancel.c

Abstract:

    (revised 2005) contains cancel safe queue routines.

Environment:

    kernel mode only

Notes:

    Margin = 120

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    05-01-05 : changed to cancel safe queues

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "cancel.tmh"
#endif


VOID
USBPORT_TxInitialize(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

   Initialize the transfer safe queue used to track pending transfer irps.  This queue is used only for Irps.

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    //
    // Transfer request Cancel Safe Queue
    //
    InitializeListHead(&devExt->TxCsqIrpListHead);

    //
    // Abort Pipe request Cancel Safe Queue
    //
    InitializeListHead(&devExt->AxCsqIrpListHead);

    //
    // Transfer requeust Done Queue
    //
    InitializeListHead(&devExt->Fdo.TxDoneListHead);

    // intialze wait event for transfer drain
    KeInitializeEvent(&devExt->Fdo.TxDoneListEmpty_Event,
                      NotificationEvent,
                      USBD_EVENT_STATE_SIGNALED);

    //
    // Transfer request Priority Queue
    //
    InitializeListHead(&devExt->Fdo.TxPriorityListHead);

    // intialze wait event for transfer drain
    KeInitializeEvent(&devExt->Fdo.TxPriorityListEmpty_Event,
                      NotificationEvent,
                      USBD_EVENT_STATE_SIGNALED);

    devExt->Fdo.TxPriorityListAsyncCount = 0;
    devExt->Fdo.TxPriorityListPeriodicCount = 0;

    //
    // Transfer request Cancel List
    //
    InitializeListHead(&devExt->Fdo.TxCancelIrpListHead);

    KeInitializeEvent(&devExt->Fdo.TxCancelIrpListEmpty_Event,
                      NotificationEvent,
                      USBD_EVENT_STATE_SIGNALED);



    IoCsqInitializeEx(&devExt->TxCsq,
                USBPORT_TxCsqInsertIrpEx,            //CsqInsertIrpEx,
                USBPORT_TxCsqRemoveIrp,              //CsqRemoveIrp,
                USBPORT_TxCsqPeekNextIrp,            //CsqPeekNextIrp,
                USBPORT_TxCsqAcquireLock,            //CsqAcquireLock,
                USBPORT_TxCsqReleaseLock,            //CsqReleaseLock,
                USBPORT_TxCsqCompleteCanceledIrp     //CsqCompleteCanceledIrp
                );

    // Abort List
    InitializeListHead(&devExt->Fdo.AxPendingListHead);
    InitializeListHead(&devExt->Fdo.AxDoneListHead);
    InitializeListHead(&devExt->Fdo.AxCanceledListHead);

    IoCsqInitializeEx(&devExt->AxCsq,
                USBPORT_AxCsqInsertIrpEx,            //CsqInsertIrpEx,
                USBPORT_AxCsqRemoveIrp,              //CsqRemoveIrp,
                USBPORT_AxCsqPeekNextIrp,            //CsqPeekNextIrp,
                USBPORT_AxCsqAcquireLock,            //CsqAcquireLock,
                USBPORT_AxCsqReleaseLock,            //CsqReleaseLock,
                USBPORT_AxCsqCompleteCanceledIrp     //CsqCompleteCanceledIrp
                );
}


VOID
USBPORT_TxSetState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in TX_EVENT Event,
    __in TX_STATE NewState
    )
/*++

Routine Description:

   Logs a given transfer state transition

Arguments:

    see DDK

Return Value:

--*/
{
    TX_STATE_LOG_ENTRY *newLog;
    PDEVICE_EXTENSION devExt;
    TX_STATE PreviousState;
 
    if (Xdpc) {
        USBPORT_ASSERT(Xdpc->TxLockState);
    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'tSt0', Transfer, Event, NewState);

    devExt = GetHcFdoExt(HcFdo);
    PreviousState = Transfer->TxState;
 
    //
    // Account for entry into and exit out of TX_InQueue/TX_Mapped
    // which are the only transfer states that need the Map DPC to
    // be signalled
    //
    // Note that the global transfer lock is always held before 
    // entering this function so we should be fine with not using
    // interlocked operations for maintaining the TxNeedsMapDpcCount 
    // counter.
    //
 
    if ((PreviousState == TX_InQueue) || (PreviousState == TX_Mapped)) {
        if ((NewState != TX_InQueue) && (NewState != TX_Mapped)) {
            USBPORT_ASSERT(devExt->Fdo.TxNeedsMapDpcCount>0);
            devExt->Fdo.TxNeedsMapDpcCount--;
        } 
    } else if ((NewState == TX_InQueue) || (NewState == TX_Mapped)) {
        devExt->Fdo.TxNeedsMapDpcCount++; 
    }
    
    Transfer->LogIdx=(Transfer->LogIdx+1) % NUM_TX_STATE_LOG_ENTRIES;
    newLog = &Transfer->Log[Transfer->LogIdx];

    newLog->Event = Event;
    newLog->CurrentState = Transfer->TxState;
    Transfer->TxState = newLog->NextState = NewState;
}


NTSTATUS
USBPORT_TxCsqInsertIrpEx(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID InsertContext
    )
/*++

Routine Description:

    PIO_CSQ_INSERT_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'TxI0', Irp, InsertContext,  0);

    transfer = (PHCD_TRANSFER_CONTEXT) InsertContext;
    ASSERT_TRANSFER(transfer);

    USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);
    InsertTailList(&devExt->TxCsqIrpListHead, &transfer->TxCsqIrpLink);
    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

    transfer->IrpState = Irp_TxQueued_Pending; // (Priority Queue, Cancel Safe Queue)

    // lock is held
    USBPORT_Core_iInsertPriorityTransfer(devExt->HcFdoDeviceObject, transfer);

    // alwyas inserted
    return STATUS_SUCCESS;
}


PHCD_TRANSFER_CONTEXT
USBPORT_TxCsqFindIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    )
/*++

Routine Description:

    Find the transfer associated with an Irp in the TxCsq.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'TxF0', Irp, 0,  0);

    // remove transfer from Irp queue only
    le = devExt->TxCsqIrpListHead.Flink;

    while (le != &devExt->TxCsqIrpListHead) {
        transfer =  CONTAINING_RECORD(le,
                                      HCD_TRANSFER_CONTEXT,
                                      TxCsqIrpLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'TxF1', Irp, transfer,  transfer->Irp );

        if (transfer->Irp == Irp) {
            break;
        }
        le = le->Flink;
        transfer = NULL;
    }

    return transfer;
}


VOID
USBPORT_TxCsqRemoveIrp(
    PIO_CSQ Csq,
    PIRP    Irp
    )
/*++

Routine Description:

    PIO_CSQ_REMOVE_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION       devExt;
    PDEVICE_OBJECT          hcFdo;
    PHCD_TRANSFER_CONTEXT   transfer;


    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR0', Irp, Irp->Cancel, 0);

    //
    // Search the devExt->TxCsqIrpListHead Cancel Safe Queue for the
    // Transfer that is associated with this Irp.  The associated
    // Transfer should always be found on the Cancel Safe Queue.
    //
    transfer = USBPORT_TxCsqFindIrp(devExt->HcFdoDeviceObject, Irp);

    if (transfer) {

        if (transfer->IrpState == Irp_IoDone_Pending) {

            //
            // (Done Queue, Cancel Safe Queue)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR1', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is on both the Done Queue and the
            // Cancel Safe Queue then this routine could have been
            // called either within the context of a client driver call
            // to IoCancelIrp() or within the context of
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() calling
            // IoCsqRemoveIrp().
            //
            // In either case USBPORT_Core_iIrpCsqCompleteDoneTransfer()
            // has not completed execution since the Transfer/Irp is
            // still on the Done Queue.
            //

            //
            // This is the hot completion path for a Transfer/Irp which
            // was not canceled by the client driver.
            //

            //
            // Place the Transfer/Irp on the Cancel List to be handled
            // by USBPORT_TxCsqCompleteCanceledIrp().  The actual
            // insertion happens in the common code path below.
            //
            // Or, in the case where this routine is executing within
            // the context of the IoCsqRemoveIrp() call from
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() the Irp will
            // be returned by IoCsqRemoveIrp() and
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() will then know
            // that USBPORT_TxCsqCompleteCanceledIrp() will not be
            // called for this Transfer/Irp.  In that case when
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() continues
            // execution it will remove the Transfer/Irp from both the
            // Cancel List and the Done Queue and complete the
            // Transfer/Irp.
            //
            transfer->IrpState = Irp_IoDone_Canceled; // (Done Queue, Cancel List)

        } else if (transfer->IrpState == Irp_TxQueued_Pending) {

            //
            // (Priority Queue, Cancel Safe Queue)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR2', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is still on the Priority Queue this
            // routine must have been called within the context of a
            // client driver call to IoCancelIrp() and not within the
            // context of USBPORT_Core_iIrpCsqCompleteDoneTransfer()
            // calling IoCsqRemoveIrp().
            //
            USBPORT_ASSERT(Irp->Cancel == TRUE);

            //
            // Place the Transfer/Irp on the Cancel List to be handled
            // by USBPORT_TxCsqCompleteCanceledIrp().  The actual
            // insertion happens in the common code path below.
            //
            transfer->IrpState = Irp_TxQueued_Canceled; // (Priority Queue, Cancel List)

        } else if (transfer->IrpState == Irp_Pending) {

            //
            // (Cancel Safe Queue)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR3', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is not on the Priority Queue nor on
            // the Done Queue then this routine must have been called
            // within the context of a client driver call to
            // IoCancelIrp() and not within the context of
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() calling
            // IoCsqRemoveIrp().
            //
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() would have
            // called IoCsqRemoveIrp(), but that call to
            // IoCsqRemoveIrp() must have returned NULL due to the
            // Transfer/Irp being canceled.
            //
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() must have
            // removed the Transfer/Irp from the Done Queue and
            // completed execution without completing the Transfer/Irp.
            //
            USBPORT_ASSERT(Irp->Cancel == TRUE);

            //
            // Place the Transfer/Irp on the Cancel List to be handled
            // by USBPORT_TxCsqCompleteCanceledIrp().  The actual
            // insertion happens in the common code path below.
            //
            transfer->IrpState = Irp_Canceled; // (Cancel List)

        } else {

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR4', Irp, transfer->IrpState, transfer);

            USBPORT_ASSERTMSG("Unexpected transfer->IrpState", FALSE);
        }

        //
        // In all cases remove the Transfer/Irp from the
        // devExt->TxCsqIrpListHead Cancel Safe Queue.
        //
        RemoveEntryList(&transfer->TxCsqIrpLink);
        USB_ZAP_LINK(&transfer->TxCsqIrpLink);

        //
        // In all cases insert the Transfer/Irp into the
        // devExt->Fdo.TxCancelIrpListHead Cancel List to either be
        // removed and handled by USBPORT_TxCsqCompleteCanceledIrp()
        // when it will subsequently execute, or to be removed by
        // USBPORT_Core_iIrpCsqCompleteDoneTransfer() when
        // USBPORT_TxCsqCompleteCanceledIrp() will not execute.
        //
        USB_ASSERT_UNLINKED(&transfer->TxCancelIrpLink);
        InsertTailList(&devExt->Fdo.TxCancelIrpListHead, &transfer->TxCancelIrpLink);

    } else {

        LOGENTRY(hcFdo, LOG_TRANSFER, 'TXR5', Irp, Irp->Cancel, 0);

        USBPORT_ASSERTMSG("Transfer not found on Cancel Safe Queue!", FALSE);
    }
}


PIRP
USBPORT_TxCsqPeekNextIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    )
/*++

Routine Description:

    PIO_CSQ_PEEK_NEXT_IRP

    Reukes for peeNext the way I understand them -- these are not documented in
    the DDK text.

    if Irp == NULL start from list head
    else start peeking from the Irp following the Irp passed in.

    When we encounter an irp matching the context we return the irp

    if peekcontext == NULL then we return the next irp

    The DDK sample checks the FileObject in the irpstack even though a
    PeekContext is never specified.

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY le;
    PIRP nextIrp = NULL;
    PHCD_TRANSFER_CONTEXT transfer;
    PDEVICE_OBJECT hcFdo;

    USBPORT_ASSERT(PeekContext == NULL);
    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'TxP0', Irp, PeekContext, 0);

    if (Irp == NULL) {
        // start at the head
        le = devExt->TxCsqIrpListHead.Flink;
    } else {
        transfer = USBPORT_TxCsqFindIrp(hcFdo, Irp);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'TxP1', Irp, transfer, 0);

        // start at the next irp in the queue
        if (transfer) {
            le = transfer->TxCsqIrpLink.Flink;
        } else {
            le = NULL;
        }
    }

    while (le && le != &devExt->TxCsqIrpListHead) {

        transfer =  CONTAINING_RECORD(le,
                                      HCD_TRANSFER_CONTEXT,
                                      TxCsqIrpLink);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'TxP2', Irp, transfer, 0);

        nextIrp = transfer->Irp;
        LOGENTRY(hcFdo, LOG_TRANSFER, 'TxP3', Irp, transfer, nextIrp);

        USBPORT_ASSERT(nextIrp);

        if (PeekContext == NULL) {
            break;
        }

        le = le->Flink;
    }

    LOGENTRY(hcFdo, LOG_TRANSFER, 'TxP4', nextIrp, 0, le);

    return nextIrp;
}


__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxCsqAcquireLock(
    PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_ACQUIRE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);

    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'txL+', 0, hcFdo, 0);
    *Irql = USBPORT_TxAcquireLock(hcFdo, NULL);
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxCsqReleaseLock(
    PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_RELEASE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'txL-', 0, hcFdo, 0);
    USBPORT_TxReleaseLock(hcFdo, NULL, Irql);
}


VOID
USBPORT_TxCsqCompleteCanceledIrp(
    PIO_CSQ Csq,
    PIRP    Irp
    )
/*++

Routine Description:

    PIO_CSQ_COMPLETE_CANCELED_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{

    PDEVICE_EXTENSION       devExt;
    PDEVICE_OBJECT          hcFdo;
    KIRQL                   irql;
    PHCD_TRANSFER_CONTEXT   transfer;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               TxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC0', Irp, 0, 0);

    // Log the Irp cancellation.
    // template: tid_USBPORT_IRP
    //
    USBPORT_EtwWriteHcData1(hcFdo,
                            &USBPORT_ETW_EVENT_IRP_CANCEL,
                            NULL,

                            &Irp,
                            sizeof(PIRP));

    irql = USBPORT_TxAcquireLock(hcFdo, NULL);

    //
    // Search the devExt->TxCancelIrpListHead Cancel List for the
    // Transfer that is associated with this Irp.  The associated
    // Transfer should always be found on the Cancel List.
    //
    transfer = USBPORT_Core_iFindCanceledTransferIrp(hcFdo, Irp);

    if (transfer) {

        //
        // The Cancel Safe Queue and the Cancel List are mutually
        // exclusive.
        //
        USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);

        //
        // This flag has no purpose other than a debugging aid
        // indicating that the client driver called IoCancelIrp() for
        // this Transfer/Irp.
        //
        SET_FLAG(transfer->Flags, USBPORT_TXFLAG_IO_CANCEL);

        if (transfer->IrpState == Irp_TxQueued_Canceled) {

            //
            // (Priority Queue, Cancel List)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC1', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is still on the Priority Queue it
            // must have been canceled by a client driver call to
            // IoCancelIrp() before it was completed by the miniport
            // or even queued in the miniport.
            //
#if DBG
            USBPORT_ASSERT(USBPORT_Core_iFindPriorityTransferIrp(hcFdo, Irp) == transfer);
#endif
            USB_ASSERT_UNLINKED(&transfer->TxDoneLink);

            //
            // Remove the Transfer/Irp from the Cancel List, leaving it
            // only on the Priority Queue.
            //
            RemoveEntryList(&transfer->TxCancelIrpLink);
            USB_ZAP_LINK(&transfer->TxCancelIrpLink);

            transfer->IrpState = Irp_TxQueued; // (Priority Queue)

            //
            // Initiate the actual Transfer/Irp cancelation process.
            // The Transer/Irp will eventually be moved from the
            // Priority Queue to the Done Queue by
            // USBPORT_Core_UsbDoneDpc_Worker(), then removed from the
            // Done Queue by USBPORT_Core_UsbIocDpc_Worker() and
            // completed by USBPORT_Core_iIrpCsqCompleteDoneTransfer().
            //
            USBPORT_Core_iCancelPriorityTransfer(hcFdo, transfer);

            USBPORT_TxReleaseLock(hcFdo, NULL, irql);

        } else if (transfer->IrpState == Irp_IoDone_Canceled) {

            //
            // (Done Queue, Cancel List)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC2', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is on the Done Queue then
            // USBPORT_Core_UsbDoneDpc_Worker() must have started the
            // completion process for the Transfer/Irp either before the
            // client driver called IoCancelIrp() and
            // USBPORT_TxCsqRemoveIrp() ran, or after he client driver
            // called IoCancelIrp() and USBPORT_TxCsqRemoveIrp() ran but
            // before this routine ran and initiated the actual
            // Transfer/Irp cancelation process in the
            // Irp_TxQueued_Canceled state.
            //
            // In either case there is no need to initate the
            // cancelation process now.
            //
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() will complete
            // the Transfer/Irp when it is found in the Irp_IoDone state
            // after calling IoCsqRemoveIrp().
            //
            USB_ASSERT_UNLINKED(&transfer->TxPriorityLink);

            //
            // Remove the Transfer/Irp from the Cancel List, leaving it
            // only on the Done Queue.
            //
            RemoveEntryList(&transfer->TxCancelIrpLink);
            USB_ZAP_LINK(&transfer->TxCancelIrpLink);

            transfer->IrpState = Irp_IoDone; // (Done Queue)

            USBPORT_TxReleaseLock(hcFdo, NULL, irql);

        } else if (transfer->IrpState == Irp_Canceled) {

            //
            // (Cancel List)
            //

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC3', Irp, transfer->Urb, transfer);

            //
            // If the Transfer/Irp is not on the Priority Queue nor on
            // the Done Queue then USBPORT_Core_UsbDoneDpc_Worker() must
            // have started the completion process for the Transfer/Irp
            // and USBPORT_Core_iIrpCsqCompleteDoneTransfer() must have
            // run to completion before USBPORT_TxCsqRemoveIrp() ran
            // due to a client driver call to IoCancelIrp().
            //
            // USBPORT_Core_iIrpCsqCompleteDoneTransfer() would not have
            // completed the Transfer/Irp when it was found in the
            // Irp_IoDone_Pending state after calling IoCsqRemoveIrp(),
            // leaving it to this routine to complete the Transfer/Irp.
            //
            USB_ASSERT_UNLINKED(&transfer->TxPriorityLink);
            USB_ASSERT_UNLINKED(&transfer->TxDoneLink);

            //
            // Remove the Transfer/Irp from the Cancel List, leaving it
            // now on no lists.
            //
            RemoveEntryList(&transfer->TxCancelIrpLink);
            USB_ZAP_LINK(&transfer->TxCancelIrpLink);

            transfer->IrpState = Irp_Complete; // (none)

            //
            // This routine will release the TX Lock before it returns.
            //
            USBPORT_Core_iCompleteDoneTransfer(hcFdo, Irp, transfer, NULL, irql);

            USBPORT_Core_FlushCompletedAborts(hcFdo, NULL);

        } else {

            LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC4', Irp, transfer->IrpState, transfer);

            USBPORT_ASSERTMSG("Unexpected transfer->IrpState", FALSE);

            USBPORT_TxReleaseLock(hcFdo, NULL, irql);
        }

    } else {

        LOGENTRY(hcFdo, LOG_TRANSFER, 'TXC5', Irp, 0, 0);

        USBPORT_ASSERTMSG("Transfer not found on Cancel List!", FALSE);

        USBPORT_TxReleaseLock(hcFdo, NULL, irql);
    }
}


VOID
USBPORT_TxQueueTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PIRP Irp
    )
/*++

Routine Description:

    Queues a transfer to the global priority transfer queue.

Arguments:

    Endpoint - endpoit referenced by this transfer.

    Transfer - pointer to transfer to queue.

    Irp - (optional) pointer to irp.

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS nts;

    devExt = GetHcFdoExt(HcFdo);

    if (Irp) {
        nts = IoCsqInsertIrpEx(&devExt->TxCsq, Irp, &Transfer->CsqContext, Transfer);
        // success means insterted in the queue and must be removed
        USBPORT_ASSERT(NT_SUCCESS(nts));
    } else {
        // irpless transfer we queue the transfer directly
        Transfer->IrpState = Irp_TxQueued; // (Priority Queue)
        USBPORT_Core_LockInsertPriorityTransfer(HcFdo, Transfer);
    }
}


__drv_savesIRQL
__drv_raisesIRQL(DISPATCH_LEVEL)
KIRQL
USBPORT_TxAcquireLock(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc
    )
/*++

Routine Description:

    Acquires the Tx spinlock

    *this is currently the same lock used for the global endpoint list

--*/
{
    KIRQL irql;

    USBPORT_AcquireEpListLock(HcFdo, &irql);

    if (Xdpc) {
        Xdpc->TxLockState = 1;
    }

    return irql;
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxReleaseLock(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    Releases the Tx spinlock

    *this is currently the same lock used for the global endpoint list and Xdpc

--*/
{
    if (Xdpc) {
        Xdpc->TxLockState = 0;
    }

    USBPORT_ReleaseEpListLock(HcFdo, Irql);
}

/*
    Transfer safe queue for abort pipe request functons.


    The USB URB_FUNCTION_ABORT_PIPE function aborts (cancels) all transfers currently queued for a given endpoint/pipe.
    This code handles both internal(flush functions for pnp) and external(URB_FUNCTION_ABORT_PIPE) abort requests

*/



BOOLEAN
USBPORT_AxQueueAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PUSBD_DEVICE_HANDLE DevHandle,
    __inout_opt PURB Urb,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT AbortEvent
    )
/*++

Routine Description:

    Queues a usb abort-pipe request for an endpoint.

Arguments:

    Endpoint - endpoint referenced by this transfer.  Endpoint should be ref'ed before calling this routine.

    UrbEpHandle - handle associated with the irp/urb request

    DevHandle - (optional) - device handle referenced by the request.

    URB - (optional) pointer to abort request.

    Irp - (optional) pointer to irp.

    AbortEvent - Event signalled when this request completes


Return Value:

    TRUE if request was queued for async completion.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortContext;
    BOOLEAN requestQueued = FALSE;

    devExt = GetHcFdoExt(HcFdo);

    abortContext = USBPORT_Core_AllocateAbortRequest(HcFdo, Endpoint, DevHandle, Urb, Irp, AbortEvent);

    if (abortContext) {
        if (Irp) {
            NTSTATUS nts;

            // URB function
            USBPORT_ASSERT(Urb != NULL);
            nts = IoCsqInsertIrpEx(&devExt->AxCsq, Irp, &abortContext->CsqContext, abortContext);
            // success means pended
            if (NT_SUCCESS(nts)) {
                requestQueued = TRUE;
            } else {
                // irp was not queued
                abortContext->Irp = NULL;
                abortContext->Urb = NULL;
                abortContext->DeviceHandle = NULL;
                // set the error in the URB
                SET_USBD_ERROR(abortContext->Urb, USBD_STATUS_DEVICE_GONE);
                abortContext->DeviceHandle = NULL;
                // free context, don't complete IRP
                USBPORT_ASSERT(NT_ERROR(nts));
                USBPORT_Core_CompleteAbortRequest(devExt->HcFdoDeviceObject, abortContext, nts);
            }
        } else {
            KIRQL irql;

            // internal abort request
            USBPORT_ASSERT(Urb == NULL);
            USBPORT_ASSERT(Irp == NULL);
            USBPORT_ASSERT(DevHandle == NULL);

            irql = USBPORT_TxAcquireLock(HcFdo, NULL);

            requestQueued = USBPORT_Core_iAbortEndpoint(HcFdo, abortContext);

            USBPORT_TxReleaseLock(HcFdo, NULL, irql);

             // success means pended
            if (!requestQueued) {
                // request was not queued, free it
                USBPORT_Core_CompleteAbortRequest(HcFdo, abortContext, STATUS_UNSUCCESSFUL);
            }

        }

    }

    return requestQueued;
}


NTSTATUS
USBPORT_AxCsqInsertIrpEx(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID InsertContext
    )
/*++

Routine Description:

    PIO_CSQ_INSERT_IRP

    see DDK

Arguments:

    InsertContext URB_CONTEXT

    see DDK

Return Value:

--*/
{

    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortCtx;
    NTSTATUS nts;
    BOOLEAN requestQueued;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'AxI0', Irp, InsertContext,  0);

    abortCtx = (PUSB_ABORT_PIPE_CONTEXT) InsertContext;
    ASSERT_ABORT_CONTEXT(abortCtx);

    // lock is held
    requestQueued = USBPORT_Core_iAbortEndpoint(devExt->HcFdoDeviceObject, abortCtx);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'AxI1', Irp, abortCtx, requestQueued);

    if (requestQueued) {
        // request was pended
        LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'AxI2', Irp, abortCtx,  0);

        // should still be valid
        ASSERT_ABORT_CONTEXT(abortCtx);

        // Insert USB_ABORT_PIPE_CONTEXT AxCsqIrpLink on to the
        // DEVICE_EXTENSION AxCsqIrpListHead cancel safe queue list.
        //
        InsertTailList(&devExt->AxCsqIrpListHead, &abortCtx->AxCsqIrpLink);

        // Take the USBPORT_AxCsqRemoveIrpRef reference on this
        // USB_ABORT_PIPE_CONTEXT that will be released by
        // USBPORT_AxCsqRemoveIrp().
        //
        abortCtx->USBPORT_AxCsqRemoveIrpRef = 1;

        IoMarkIrpPending(Irp);

        nts = STATUS_SUCCESS;
    } else {
        nts = STATUS_UNSUCCESSFUL;
    }

    return nts;
}


PUSB_ABORT_PIPE_CONTEXT
USBPORT_AxFindIrpCsq(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    )
/*++

Routine Description:

    Find the USB_ABORT_PIPE_CONTEXT associated with an Irp in the
    AxCsq, i.e. on the DEVICE_EXTENSION AxCsqIrpListHead list.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortCtx = NULL;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'AxF0', Irp, 0,  0);

    // Walk the DEVICE_EXTENSION AxCsqIrpListHead list looking for a
    // USB_ABORT_PIPE_CONTEXT Irp which matches the target Irp.
    //
    le = devExt->AxCsqIrpListHead.Flink;

    while (le != &devExt->AxCsqIrpListHead) {
        abortCtx =  CONTAINING_RECORD(le,
                                      USB_ABORT_PIPE_CONTEXT,
                                      AxCsqIrpLink);

        ASSERT_ABORT_CONTEXT(abortCtx);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'AxF1', Irp, abortCtx,  abortCtx->Irp );

        if (abortCtx->Irp == Irp) {
            break;
        }
        le = le->Flink;
        abortCtx = NULL;
    }

    return abortCtx;
}


PUSB_ABORT_PIPE_CONTEXT
USBPORT_AxFindIrpCanceled(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    )
/*++

Routine Description:

    Find the USB_ABORT_PIPE_CONTEXT associated with a canceled Irp in
    the DEVICE_EXTENSION Fdo.AxCanceledListHead list.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortCtx = NULL;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'AxC>', Irp, 0,  0);

    // Walk the DEVICE_EXTENSION Fdo.AxCanceledListHead list looking
    // for a USB_ABORT_PIPE_CONTEXT Irp which matches the target Irp.
    //
    le = devExt->Fdo.AxCanceledListHead.Flink;

    while (le != &devExt->Fdo.AxCanceledListHead) {
        abortCtx =  CONTAINING_RECORD(le,
                                      USB_ABORT_PIPE_CONTEXT,
                                      AxCanceledLink);

        ASSERT_ABORT_CONTEXT(abortCtx);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'AxC#', Irp, abortCtx,  abortCtx->Irp );

        if (abortCtx->Irp == Irp) {
            break;
        }
        le = le->Flink;
        abortCtx = NULL;
    }

    return abortCtx;
}


VOID
USBPORT_AxCsqRemoveIrp(
    PIO_CSQ Csq,
    PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_REMOVE_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortContext;
    PDEVICE_OBJECT hcFdo;
    //
    // NOTE:
    //
    // There is an USBPORT_TxAcquireLock() and USBPORT_TxReleaseLock()
    // pair in effect around the entire execution of this routine.
    //

    // Get the DEVICE_EXTENSION in which the AxCsq is contained.
    //
    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR0', Irp, 0,  0 );

    // The AxCsqIrpLink for the USB_ABORT_PIPE_CONTEXT for this Irp must
    // currently be inserted in the DEVICE_EXTENSION AxCsqIrpListHead
    // cancel safe queue as this is the only routine which removes the
    // USB_ABORT_PIPE_CONTEXT AxCsqIrpLink from the DEVICE_EXTENSION
    // AxCsqIrpListHead list, and this routine cannot be called more
    // than once for the same Irp.
    //
    // Find the USB_ABORT_PIPE_CONTEXT for this Irp.
    //
    abortContext = USBPORT_AxFindIrpCsq(devExt->HcFdoDeviceObject, Irp);

    LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR1', Irp, 0,  abortContext);
    USBPORT_ASSERT(abortContext != NULL);
    ASSERT_ABORT_CONTEXT(abortContext);

#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "abortContext cannot be NULL; see ASSERT_ABORT_CONTEXT")
    if (abortContext) {

        // Remove USB_ABORT_PIPE_CONTEXT AxCsqIrpLink from
        // DEVICE_EXTENSION AxCsqIrpListHead cancel safe queue.
        //
        LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR2', Irp, 0,  abortContext);
        USBPORT_AssertListEntryLog(hcFdo, &abortContext->AxCsqIrpLink);

        RemoveEntryList(&abortContext->AxCsqIrpLink);
        USB_ZAP_LINK(&abortContext->AxCsqIrpLink);

        // Release the USB_ABORT_PIPE_CONTEXT USBPORT_AxCsqRemoveIrpRef
        // reference.
        //
        abortContext->USBPORT_AxCsqRemoveIrpRef = 0;

        if (USBPORT_IsLinked(&abortContext->AxPendingLink)) {

            PHCD_ENDPOINT abrEp;

            LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR3', Irp, 0,  abortContext);

            // Case 1:
            //
            // The USB_ABORT_PIPE_CONTEXT AxPendingLink is currently on
            // the DEVICE_EXTENSION Fdo.AxPendingListHead list.
            //
            // There may still be HCD_TRANSFER_CONTEXT TxAbortLink
            // links on the USB_ABORT_PIPE_CONTEXT TxListHead list.
            //
            // USBPORT_Core_iCheckAbortList() has not removed the
            // USB_ABORT_PIPE_CONTEXT AxPendingLink from the
            // DEVICE_EXTENSION Fdo.AxPendingListHead list and inserted
            // the USB_ABORT_PIPE_CONTEXT AxDoneLink on to the
            // DEVICE_EXTENSION Fdo.AxDoneListHead list.  At this point
            // it never will.
            //

            // Remove the USB_ABORT_PIPE_CONTEXT AxPendingLink from the
            // DEVICE_EXTENSION Fdo.AxPendingListHead list.
            //
            USBPORT_AssertListEntryLog(hcFdo, &abortContext->AxPendingLink);
            RemoveEntryList(&abortContext->AxPendingLink);
            USB_ZAP_LINK(&abortContext->AxPendingLink);

            USB_ASSERT_UNLINKED(&abortContext->AxDoneLink);

            // Remove any HCD_TRANSFER_CONTEXT TxAbortLink links which
            // are currently on the USB_ABORT_PIPE_CONTEXT TxListHead
            // list.
            //
            while (!IsListEmpty(&abortContext->TxListHead)) {
                PLIST_ENTRY le;
                PHCD_TRANSFER_CONTEXT t;

                le = RemoveHeadList(&abortContext->TxListHead);

                t = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxAbortLink);

                USBPORT_ASSERT(t->TxType != Tx_SPLIT_CHILD);
                LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR4', t, 0, abortContext);

                t->AbortContext = NULL;
                USB_ZAP_LINK(&t->TxAbortLink);
            }

            abrEp = USBPORT_EndpointFromHandle(abortContext->EpRefHandle);
            LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR5', abrEp, 0, abortContext);
            USBPORT_iAssertEndpoint(hcFdo, abrEp);
            USBPORT_ASSERT(abortContext->Endpoint == abrEp);
            // allow another abort
            abrEp->PendingAbortContext = NULL;

            // Insert the USB_ABORT_PIPE_CONTEXT AxCanceledLink on to
            // the DEVICE_EXTENSION Fdo.AxCanceledListHead list where it
            // will be found and completed by
            // USBPORT_AxCsqCompleteCanceledIrp().
            //
            USB_ASSERT_UNLINKED(&abortContext->AxCanceledLink);
            InsertTailList(&devExt->Fdo.AxCanceledListHead,
                           &abortContext->AxCanceledLink);

        } else if (USBPORT_IsLinked(&abortContext->AxDoneLink)) {

            LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR6', Irp, 0,  abortContext);

            // Case 2:
            //
            // The USB_ABORT_PIPE_CONTEXT AxDoneLink is currently on
            // the DEVICE_EXTENSION Fdo.AxDoneListHead list.
            //
            // There should be no HCD_TRANSFER_CONTEXT TxAbortLink
            // links on the USB_ABORT_PIPE_CONTEXT TxListHead list.
            //
            // USBPORT_Core_FlushCompletedAborts() has not removed the
            // USB_ABORT_PIPE_CONTEXT AxDoneLink from the
            // DEVICE_EXTENSION Fdo.AxDoneListHead list.  At this point
            // it never will.
            //

            // Remove the USB_ABORT_PIPE_CONTEXT AxDoneLink from the
            // DEVICE_EXTENSION Fdo.AxDoneListHead list.
            //
            USBPORT_AssertListEntryLog(hcFdo, &abortContext->AxDoneLink);
            RemoveEntryList(&abortContext->AxDoneLink);
            USB_ZAP_LINK(&abortContext->AxDoneLink);

            USBPORT_ASSERT(IsListEmpty(&abortContext->TxListHead));

            // Insert the USB_ABORT_PIPE_CONTEXT AxCanceledLink on to
            // the DEVICE_EXTENSION Fdo.AxCanceledListHead list where it
            // will be found and completed by
            // USBPORT_AxCsqCompleteCanceledIrp().
            //
            USB_ASSERT_UNLINKED(&abortContext->AxCanceledLink);
            InsertTailList(&devExt->Fdo.AxCanceledListHead,
                           &abortContext->AxCanceledLink);

        } else {

            // Case 3:
            //
            // The USB_ABORT_PIPE_CONTEXT AxPendingLink is not currently
            // on the DEVICE_EXTENSION Fdo.AxPendingListHead list and
            // the USB_ABORT_PIPE_CONTEXT AxDoneLink is not currently on
            // the DEVICE_EXTENSION Fdo.AxDoneListHead list.
            //
            // USBPORT_Core_iCheckAbortList() has removed the
            // USB_ABORT_PIPE_CONTEXT AxPendingLink from the
            // DEVICE_EXTENSION Fdo.AxPendingListHead list, and
            // USBPORT_Core_FlushCompletedAborts() has removed the
            // USB_ABORT_PIPE_CONTEXT AxDoneLink from the
            // DEVICE_EXTENSION Fdo.AxDoneListHead list.
            //
            // Case 3a:
            //
            // USBPORT_Core_FlushCompletedAborts() may currently be
            // holding the USBPORT_Core_FlushCompletedAbortsRef
            // reference on the USB_ABORT_PIPE_CONTEXT, in which case
            // the USB_ABORT_PIPE_CONTEXT AxCanceledLink cannot be
            // inserted on the DEVICE_EXTENSION Fdo.AxCanceledListHead
            // list for completion by USBPORT_AxCsqCompleteCanceledIrp().
            //
            // Case 3b:
            //
            // Or USBPORT_Core_FlushCompletedAborts() may have released
            // the USBPORT_Core_FlushCompletedAbortsRef
            // reference on the USB_ABORT_PIPE_CONTEXT, in which case
            // the USB_ABORT_PIPE_CONTEXT AxCanceledLink can be
            // inserted on the DEVICE_EXTENSION Fdo.AxCanceledListHead
            // list for completion by USBPORT_AxCsqCompleteCanceledIrp().

            USBPORT_ASSERT(IsListEmpty(&abortContext->TxListHead));

            if (abortContext->USBPORT_Core_FlushCompletedAbortsRef == 0) {

                LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR7', Irp, 0,  abortContext);

                // Case 3b:
                //
                // USBPORT_Core_FlushCompletedAborts() will never
                // access the USB_ABORT_PIPE_CONTEXT again.
                //

                // Insert the USB_ABORT_PIPE_CONTEXT AxCanceledLink on
                // to the DEVICE_EXTENSION Fdo.AxCanceledListHead list
                // where it will be found and completed by
                // USBPORT_AxCsqCompleteCanceledIrp().
                //
                USB_ASSERT_UNLINKED(&abortContext->AxCanceledLink);
                InsertTailList(&devExt->Fdo.AxCanceledListHead,
                               &abortContext->AxCanceledLink);

            } else {

                LOGENTRY(hcFdo, LOG_TRANSFER, 'AxR8', Irp, 0,  abortContext);

                // Case 3a:
                //
                // USBPORT_Core_FlushCompletedAborts() will access the
                // USB_ABORT_PIPE_CONTEXT again and find that the
                // USB_ABORT_PIPE_CONTEXT USBPORT_AxCsqRemoveIrpRef
                // reference has been released so it will complete the
                // USB_ABORT_PIPE_CONTEXT Irp and free the
                // USB_ABORT_PIPE_CONTEXT.

            }
        }
    }
}


PIRP
USBPORT_AxCsqPeekNextIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    )
/*++

Routine Description:

    PIO_CSQ_PEEK_NEXT_IRP

    Reukes for peeNext the way I understand them -- these are not documented in
    the DDK text.

    if Irp == NULL start from list head
    else start peeking from the Irp following the Irp passed in.

    When we encounter an irp matching the context we return the irp

    if peekcontext == NULL then we return the next irp

    The DDK sample checks the FileObject in the irpstack even though a
    PeekContext is never specified.

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY le;
    PIRP nextIrp = NULL;
    PUSB_ABORT_PIPE_CONTEXT abortCtx;
    PDEVICE_OBJECT hcFdo;

    USBPORT_ASSERT(PeekContext == NULL);
    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'AxP0', Irp, PeekContext, 0);

    if (Irp == NULL) {
        // start at the head
        le = devExt->AxCsqIrpListHead.Flink;
    } else {
       abortCtx = USBPORT_AxFindIrpCsq(hcFdo, Irp);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'AxP1', Irp, abortCtx, 0);

        // start at the next irp in the queue
        if (abortCtx) {
            le = abortCtx->AxCsqIrpLink.Flink;
        } else {
            le = NULL;
        }
    }

    while (le && le != &devExt->AxCsqIrpListHead) {

        abortCtx =  CONTAINING_RECORD(le,
                                      USB_ABORT_PIPE_CONTEXT,
                                      AxCsqIrpLink);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'AxP2', Irp, abortCtx, 0);

        USBPORT_ASSERT(abortCtx->Irp != NULL);

        nextIrp = abortCtx->Irp;
        LOGENTRY(hcFdo, LOG_TRANSFER, 'AxP3', Irp, abortCtx, nextIrp);

        USBPORT_ASSERT(nextIrp);

        if (PeekContext == NULL) {
            break;
        }

        le = le->Flink;
    }

    LOGENTRY(hcFdo, LOG_TRANSFER, 'AxP4', nextIrp, 0, le);

    return nextIrp;
}


__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_AxCsqAcquireLock(
    PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_ACQUIRE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);

    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'axL+', 0, hcFdo, 0);
    *Irql = USBPORT_TxAcquireLock(hcFdo, NULL);
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_AxCsqReleaseLock(
    PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_RELEASE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'axL-', 0, hcFdo, 0);
    USBPORT_TxReleaseLock(hcFdo, NULL, Irql);
}


VOID
USBPORT_AxCsqCompleteCanceledIrp(
    PIO_CSQ Csq,
    PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_COMPLETE_CANCELED_IRP

    see DDK


Arguments:
    see DDK

Return Value:

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PDEVICE_OBJECT hcFdo;
    PUSB_ABORT_PIPE_CONTEXT abortContext ;

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               AxCsq);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'axCA', Irp, 0, 0);

    irql = USBPORT_TxAcquireLock(hcFdo, NULL);

    // See if USBPORT_AxCsqRemoveIrp() inserted the
    // USB_ABORT_PIPE_CONTEXT AxCanceledLink on the DEVICE_EXTENSION
    // Fdo.AxCanceledListHead list.
    //
    abortContext = USBPORT_AxFindIrpCanceled(hcFdo, Irp);

    if (abortContext) {
        LOGENTRY(hcFdo, LOG_TRANSFER, 'axC1', Irp, 0, abortContext);

        // USBPORT_AxCsqRemoveIrp() did insert the
        // USB_ABORT_PIPE_CONTEXT AxCanceledLink on the DEVICE_EXTENSION
        // Fdo.AxCanceledListHead list.
        //
        // The USB_ABORT_PIPE_CONTEXT Irp can be completed and the
        // USB_ABORT_PIPE_CONTEXT can be freed at this point
        //
        USB_ASSERT_UNLINKED(&abortContext->AxCsqIrpLink);
        USB_ASSERT_UNLINKED(&abortContext->AxPendingLink);
        USB_ASSERT_UNLINKED(&abortContext->AxDoneLink);
        USBPORT_ASSERT(IsListEmpty(&abortContext->TxListHead));

        // Remove the USB_ABORT_PIPE_CONTEXT AxCanceledLink from the
        // DEVICE_EXTENSION Fdo.AxCanceledListHead list.
        //
        USBPORT_AssertListEntryLog(hcFdo, &abortContext->AxCanceledLink);
        RemoveEntryList(&abortContext->AxCanceledLink);
        USB_ZAP_LINK(&abortContext->AxCanceledLink);

        USBPORT_TxReleaseLock(hcFdo, NULL, irql);

        USBPORT_Core_CompleteAbortRequest(hcFdo, abortContext, STATUS_CANCELLED);

    } else {
        LOGENTRY(hcFdo, LOG_TRANSFER, 'axC2', Irp, 0, 0);

        // USBPORT_AxCsqRemoveIrp() did not insert the
        // USB_ABORT_PIPE_CONTEXT AxCanceledLink on the DEVICE_EXTENSION
        // Fdo.AxCanceledListHead list.
        //
        // The USB_ABORT_PIPE_CONTEXT Irp will be completed and the
        // USB_ABORT_PIPE_CONTEXT will be freed by
        // USBPORT_Core_FlushCompletedAborts().
        //

        USBPORT_TxReleaseLock(hcFdo, NULL, irql);
    }
}
