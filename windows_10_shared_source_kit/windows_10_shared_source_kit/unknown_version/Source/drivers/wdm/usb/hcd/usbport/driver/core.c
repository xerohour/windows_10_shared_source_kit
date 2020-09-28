/*++

Copyright (c) 2004 Microsoft Corporation

Module Name:

    core.c

Abstract:

    Core endpoint transfer code for the port driver.

Environment:

    kernel mode only

Notes:

    Margin=120

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    05-01-05 : priority transfer queue

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "core.tmh"
#endif

#ifdef USB_FORCE_MEMORY_STRESS
ULONG NumberOfResourceAllocations = 0;
#endif

BOOLEAN g_DMA_KdPrint = FALSE;

/*

USB_XDPC <Custom Transfer DPCs in USBPORT>

There are three DPCs that deal with transfer queueing:

USB_INT_DPC    - queued by a hardware interrupt or similar event. This routine polls the miniport endpoints for
completed transfers.

USB_DONE_DPC   - queued when a transfer that must be completed is detected by USB_INT_DPC, USB_MAP_DPC, Dispatch or
            Cancel handler.  This DPC populates the lookaside list used by USB_IO_DPC.

USB_MAP_DPC    - queued when a transfer is submitted by a function driver, this DPC calls IoMapTransfer (or equivalent)
and submits transfers to the miniport for DMA dransfer on the bus.

There is a fourth DPC wich handles only the completion of the irps thru th ekernel.

USB_IO_DPC    - dpc that completes the tranfers from a lookaside list.  This routine calls
IoCompleteRequest.
*/

VOID
USBPORT_AssertListEntry(PLIST_ENTRY Le)
{
#if DBG
    PLIST_ENTRY fl, bl;
#endif

    // assert that the link is not already unlinked
    USBPORT_ASSERT(Le->Flink != NULL && Le->Blink != NULL);

#if DBG
    // attempt to accesss the links to sanity check the list
    fl = Le->Flink;
    bl = Le->Blink;

    USBPORT_ASSERT(fl->Blink == Le);
    USBPORT_ASSERT(bl->Flink == Le);
#endif
}

VOID
USBPORT_AssertListEntryLog(PDEVICE_OBJECT HcFdo, PLIST_ENTRY Le)
{
    LOGENTRY(HcFdo, LOG_TRANSFER, 'lele', 0, 0, Le);
    USBPORT_AssertListEntry(Le);
}


BOOLEAN
USBPORT_IsLinked(PLIST_ENTRY Le)
{
    BOOLEAN linked;
    // assert that the link is unlinked
    linked = ((Le->Flink != NULL) && (Le->Blink != NULL));

    if (!linked) {
        USBPORT_ASSERT(Le->Flink == NULL);
        USBPORT_ASSERT(Le->Blink == NULL);
    }

    return linked;
}


VOID
USB_ZAP_LINK(PLIST_ENTRY Le)
{
    // point the entry at NULL
    Le->Flink = NULL;
    Le->Blink = NULL;
}

#if DBG

BOOLEAN
USBPORT_IsListEntryOnListHead(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListEntry
    )
{
    PLIST_ENTRY le;

    le = ListHead->Flink;

    while (le != ListHead) {

        if (le == ListEntry) {
            return TRUE;
        }

        le = le->Flink;
    }

    return FALSE;
}

#endif

PDEVICE_EXTENSION
GetHcFdoExt(
    __inout PDEVICE_OBJECT DevObj
    )
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, DevObj);
    ASSERT_FDOEXT(devExt);

    return devExt;
}

FORCEINLINE
BOOLEAN
USBPORT_iNeedMapDpc(
    __inout PDEVICE_OBJECT HcFdo
    )
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);
    return (devExt->Fdo.TxNeedsMapDpcCount > 0);
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbIocDpc_Worker(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT UsbIocDpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

 * Completes transfers on the global done queue.

 * USBPORT main worker DPC/XDPC: 4 of 4
 *
 * The main job of the USBPORT_Core_UsbIocDpc_Worker() is to remove done
 * transfers from the host controller FDO TxDoneListHead list and call
 * USBPORT_Core_iIrpCsqCompleteDoneTransfer() which will call
 * USBPORT_Core_iCompleteDoneTransfer() to actually call
 * IoCompleteRequest() to complete the transfer Irp/URB back to the
 * client driver and then free the HCD_TRANSFER_CONTEXT.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;
    PHCD_ENDPOINT endpoint;
    NTSTATUS status;

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_iBegin(HcFdo, UsbIocDpc);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Ioc0', 0, 0, UsbIocDpc);

    while (!IsListEmpty(&devExt->Fdo.TxDoneListHead)) {

        le = RemoveHeadList(&devExt->Fdo.TxDoneListHead);

        transfer = CONTAINING_RECORD(le,
                                     HCD_TRANSFER_CONTEXT,
                                     TxDoneLink);

        USB_ZAP_LINK(&transfer->TxDoneLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Ioc1', 0, transfer->IrpState, transfer);

        ASSERT_TRANSFER(transfer);

        // Check to see if this transfer was canceled/aborted and
        // cannot be completed yet until a corresponding
        // CLEAR_TT_BUFFER request to the upstream TT hub completes. If
        // so, do not call USBPORT_Core_iIrpCsqCompleteDoneTransfer()
        // for the final transfer completion yet.
        //
        if (USBPORT_IsLinked(&transfer->TxClearTTCompletionListEntry)) {

            endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
            USBPORT_iAssertEndpoint(HcFdo, endpoint);

            //
            // Move the transfer from the TxCanceledPendingClearTTCompletionListHead
            // to the TxDonePendingClearTTCompletionListHead.
            //
            RemoveEntryList(&transfer->TxClearTTCompletionListEntry);

            InsertTailList(&endpoint->TxDonePendingClearTTCompletionListHead,
                           &transfer->TxClearTTCompletionListEntry);

            // For any remaining transfers on the
            // TxCanceledPendingClearTTCompletionListHead if the
            // transfer is on the TxDoneListHead, remove the transfer
            // from the TxDoneListHead, and then move the transfer from
            // the CanceledPendingClearTTCompletionListHead to the
            // DonePendingClearTTCompletionListHead.
            //
            le = endpoint->TxCanceledPendingClearTTCompletionListHead.Flink;

            while (le != &endpoint->TxCanceledPendingClearTTCompletionListHead) {

                transfer = CONTAINING_RECORD(le,
                                             HCD_TRANSFER_CONTEXT,
                                             TxClearTTCompletionListEntry);

                if (USBPORT_IsLinked(&transfer->TxDoneLink)) {

                    RemoveEntryList(&transfer->TxDoneLink);
                    USB_ZAP_LINK(&transfer->TxDoneLink);

                    le = le->Flink;

                    RemoveEntryList(&transfer->TxClearTTCompletionListEntry);

                    InsertTailList(&endpoint->TxDonePendingClearTTCompletionListHead,
                                   &transfer->TxClearTTCompletionListEntry);

                } else {

                    break;
                }
            }

            // Issue the CLEAR_TT_BUFFER request if it has not been
            // issued yet.
            //
            if ((endpoint->GepState == GEp_PausedClearTTNeeded) ||
                (endpoint->GepState == GEp_ActiveClearTTNeeded)) {

                USBPORT_iSetGlobalEndpointState(HcFdo, endpoint, Ev_gEp_ClearTTIssued);

                // The TX Lock cannot be held while issuing the
                // CLEAR_TT_BUFER request.
                //
                USBPORT_TxReleaseLock(HcFdo, UsbIocDpc, Irql);

                status = USBPORT_ClearTtBuffer(HcFdo,
                                               (PUSBD_DEVICE_HANDLE)transfer->UrbDeviceHandle,
                                               endpoint,
                                               NULL);

                // Acquire the TX Lock again before continuing.
                //
                Irql = USBPORT_TxAcquireLock(HcFdo, UsbIocDpc);

                if (status != STATUS_PENDING) {

                    // This is not the expected code path.  Normally
                    // the CLEAR_TT_BUFFER request will complete
                    // asynchronously if successful.
                    //
                    USBPORT_Core_iClearTTBufferComplete(HcFdo, endpoint, FALSE);
                }
            }

        } else {

            // complete the transfer to the function driver
            // <this routine drops the lock>
            USBPORT_Core_iIrpCsqCompleteDoneTransfer(HcFdo, transfer, UsbIocDpc, Irql);

            Irql = USBPORT_TxAcquireLock(HcFdo, UsbIocDpc);
        }

        if (USBPORT_Xdpc_iTimeExpired(HcFdo, UsbIocDpc)) {

            // schedule USBHCINT dpc to run again since we didn't finish
            USBPORT_Xdpc_iSignal(HcFdo, UsbIocDpc, NULL);

            USBPORT_TxReleaseLock(HcFdo, UsbIocDpc, Irql);

            USBPORT_Xdpc_End(HcFdo, UsbIocDpc);
            return;
        }
    }

    USBPORT_ASSERT(IsListEmpty(&devExt->Fdo.TxDoneListHead));
    KeSetEvent(&devExt->Fdo.TxDoneListEmpty_Event, IO_NO_INCREMENT, FALSE);

    USBPORT_TxReleaseLock(HcFdo, UsbIocDpc, Irql);

    // since we completed transfers attempt to flush any outstanding abort-pipe requests
    USBPORT_Core_FlushCompletedAborts(HcFdo, UsbIocDpc);

    USBPORT_Xdpc_End(HcFdo, UsbIocDpc);
    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iClearTTBufferComplete(
    __inout PDEVICE_OBJECT  HcFdo,
    __inout PHCD_ENDPOINT   Endpoint,
    __in    BOOLEAN         SignalIocDpc
    )
/*++

Routine Description:

    This routine handles the completion of a CLEAR_TT_BUFFER request.
    It allows the final completion of transfer requests that were
    blocked from completing until the CLEAR_TT_BUFFER request completed.
    It also allows the endpoint to transition out of the
    GEp_PausedClearTTPending or the GEp_ActiveClearTTPending state, if
    that is the state that it is currently in.

    This routine is always called with the TX Lock / Fdo.EndpointListSpin held

--*/
{
    PDEVICE_EXTENSION       devExt;
    PLIST_ENTRY             le;
    PHCD_TRANSFER_CONTEXT   transfer;
    BOOLEAN                 txDoneListUpdated;

    devExt = GetHcFdoExt(HcFdo);

    // Remove all of the transfers from the endpoint
    // TxCanceledPendingClearTTCompletionListHead. These transfers have
    // not been processed by USBPORT_Core_UsbIocDpc_Worker() yet.
    // Removing them from the TxCanceledPendingClearTTCompletionListHead
    // will allow USBPORT_Core_UsbIocDpc_Worker() to complete them
    // normally without first moving them to the
    // TxDonePendingClearTTCompletionListHead.
    //
    while (!IsListEmpty(&Endpoint->TxCanceledPendingClearTTCompletionListHead)) {

        le = RemoveHeadList(&Endpoint->TxCanceledPendingClearTTCompletionListHead);

        transfer = CONTAINING_RECORD(le,
                                     HCD_TRANSFER_CONTEXT,
                                     TxClearTTCompletionListEntry);

        USB_ZAP_LINK(&transfer->TxClearTTCompletionListEntry);
    }

    // Remove all of the transfers from the endpoint
    // TxDonePendingClearTTCompletionListHead and add
    // them back to the TxDoneListHead again.
    //
    txDoneListUpdated = FALSE;

    while (!IsListEmpty(&Endpoint->TxDonePendingClearTTCompletionListHead)) {

        le = RemoveHeadList(&Endpoint->TxDonePendingClearTTCompletionListHead);

        transfer = CONTAINING_RECORD(le,
                                     HCD_TRANSFER_CONTEXT,
                                     TxClearTTCompletionListEntry);

        USB_ZAP_LINK(&transfer->TxClearTTCompletionListEntry);

        USB_ASSERT_UNLINKED(&transfer->TxDoneLink);

        InsertTailList(&devExt->Fdo.TxDoneListHead,
                       &transfer->TxDoneLink);

        txDoneListUpdated = TRUE;
    }

    // Issue the Ev_gEp_ClearTTComplete event so that the
    // endpoint can transition out of the GEp_PausedClearTTPending
    // or GEp_ActiveClearTTPending state, if that is the state that
    // it is currently in.
    //
    USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Ev_gEp_ClearTTComplete);

    if (txDoneListUpdated && SignalIocDpc) {

        // Signal USBPORT_Core_UsbIocDpc_Worker() to run to process the
        // updated TxDoneListHead.
        //
        USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbIocDpc, NULL);
    }
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbDoneDpc_Worker(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT UsbDoneDpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

 * Scans the priority queue for completed transfers and ready them for
 * completion back to the function drivers.

 * USBPORT main worker DPC/XDPC: 3 of 4
 *
 * USBPORT_Core_UsbDoneDpc_Worker() basically walks the
 * FDO_EXTENSION->TxPriorityListHead list of all transfers and look for
 * transfers in the TX_HwDone state and removes them from the
 * TxPriorityListHead list and inserts them on the TxDoneListHead list.
 * Then it queues a call to the final main USBPORT DPC worker
 * USBPORT_Core_UsbIocDpc_Worker().

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PHCD_ENDPOINT endpoint;

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_iBegin(HcFdo, UsbDoneDpc);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne0', 0, 0, 0);

    transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbDoneDpc);

    while (transfer) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne1', 0, 0, transfer);

        endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne2', transfer, 0, endpoint);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne3', transfer, 0, transfer->TxState);

        switch (transfer->TxState) {
        case TX_Canceled:
            // transfer not mapped or queued to harwdare yet

            LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne4', transfer, transfer->IrpState, transfer->TxState);

            // remove from priority queue
            USBPORT_Core_iRemovePriorityTransfer(HcFdo, transfer);

            USBPORT_TxSetState(HcFdo, UsbDoneDpc, transfer, Ev_TX_Done, TX_IoComplete);
            switch (transfer->IrpState) {

            case Irp_TxQueued_Pending:

                //
                // (Priority Queue, Cancel Safe Queue)
                //
                USB_ASSERT_UNLINKED(&transfer->TxCancelIrpLink);
                transfer->IrpState = Irp_IoDone_Pending; // (Done Queue, Cancel Safe Queue)
                break;

            case Irp_TxQueued:

                //
                // (Priority Queue)
                //
                USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);
                USB_ASSERT_UNLINKED(&transfer->TxCancelIrpLink);
                transfer->IrpState = Irp_IoDone; // (Done Queue)
                break;

            case Irp_TxQueued_Canceled:

                //
                // (Priority Queue, Cancel List)
                //
                USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);
                transfer->IrpState = Irp_IoDone_Canceled; // (Done Queue, Cancel List)
                break;

            default:
                USBPORT_ASSERTMSG("Unexpected transfer->IrpState", FALSE);
                break;
            }

            //
            // Insert the Transer/Irp on the Done Queue
            //
            USB_ASSERT_UNLINKED(&transfer->TxDoneLink);
            InsertTailList(&devExt->Fdo.TxDoneListHead, &transfer->TxDoneLink);
            KeResetEvent(&devExt->Fdo.TxDoneListEmpty_Event);

            USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbIocDpc, NULL);
            break;

        case TX_Mapped_Canceled:
        case TX_HwDone:
        case TX_Error:
        case TX_Mapped_Error:
             LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne5', transfer, transfer->IrpState, transfer->TxState);

            // maxreq only counst against mapped transfers so we skip the split children
            if (transfer->TxType == Tx_SPLIT_CHILD) {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne6', transfer, 0, 0);

                // remove from priority queue
                TEST_TRAP();
                USBPORT_Core_iRemovePriorityTransfer(HcFdo, transfer);
                USBPORT_TxSetState(HcFdo, UsbDoneDpc, transfer, Ev_TX_Done, TX_Split_c_IoComplete);
                transfer->IrpState = Irp_Reserved;

            } else {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne7', transfer, 0, 0);

                //maxreq--
                USBPORT_Core_iDecEpHwPendingTransferCount(HcFdo, endpoint, transfer);

                // remove from priority queue
                USBPORT_Core_iRemovePriorityTransfer(HcFdo, transfer);

                // mark it complete and insert to lookaside list
                LOGENTRY(HcFdo, LOG_TRANSFER, 'Dne8', transfer, transfer->IrpState, transfer->TxState);

                USBPORT_TxSetState(HcFdo, UsbDoneDpc, transfer, Ev_TX_Done, TX_Mapped_IoComplete);

                switch (transfer->IrpState) {

                case Irp_TxQueued_Canceled:

                    //
                    // (Priority Queue, Cancel List)
                    //
                    USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);
                    transfer->IrpState = Irp_IoDone_Canceled; // (Done Queue, Cancel List)
                    break;

                case Irp_TxQueued_Pending:

                    //
                    // (Priority Queue, Cancel Safe Queue)
                    //
                    USB_ASSERT_UNLINKED(&transfer->TxCancelIrpLink);
                    transfer->IrpState = Irp_IoDone_Pending; // (Done Queue, Cancel Safe Queue)
                    break;

                case Irp_TxQueued:

                    //
                    // (Priority Queue)
                    //
                    USB_ASSERT_UNLINKED(&transfer->TxCsqIrpLink);
                    USB_ASSERT_UNLINKED(&transfer->TxCancelIrpLink);
                    transfer->IrpState = Irp_IoDone; // (Done Queue)
                    break;

                default:
                    USBPORT_ASSERTMSG("Unexpected transfer->IrpState", FALSE);
                }

                // insert on DoneList
                USB_ASSERT_UNLINKED(&transfer->TxDoneLink);
                InsertTailList(&devExt->Fdo.TxDoneListHead, &transfer->TxDoneLink);
                KeResetEvent(&devExt->Fdo.TxDoneListEmpty_Event);

                USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbIocDpc, NULL);

                // indicate another transfer can be mapped for this endpoint
                if (USBPORT_iNeedMapDpc(HcFdo)) {
                    USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, transfer);
                }
            }
            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbDoneDpc);
            break;

        default:

            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbDoneDpc);
            break;

        }

        if (USBPORT_Xdpc_iTimeExpired(HcFdo, UsbDoneDpc)) {

            // schedule USBDONE dpc to run again since we didn't finish
            USBPORT_Xdpc_iSignal(HcFdo, UsbDoneDpc, NULL);
            break;
        }
    }

    USBPORT_TxReleaseLock(HcFdo, UsbDoneDpc, Irql);
    USBPORT_Xdpc_End(HcFdo, UsbDoneDpc);
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbMapDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbMapDpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

 * USBPORT main worker DPC/XDPC: 1 of 4
 *
 * The main job of USBPORT_Core_UsbMapDpc_Worker() is to walk the
 * FDO_EXTENSION->TxPriorityListHead list of all transfers and look for
 * transfers in the initial TX_InQueue state.  For such transfers it
 * calls USBPORT_Core_iMapTransfer() which is where
 * AdapterObject->DmaOperations->GetScatterGatherList() is called to
 * build the DMA S/G list for the transfer.  The
 * USBPORT_Core_ProcessScatterGatherList() callback from
 * GetScatterGatherList() is where the OS S/G list is translated into
 * the USBPORT / miniport interface version of the S/G list.  After
 * USBPORT_Core_ProcessScatterGatherList() builds the miniport interface
 * version of the S/G list it can call
 * USBPORT_Core_iSubmitTransferToMiniport() to have the miniport
 * translate the request into TDs for the endpoint and queue the request
 * on the host controller hardware.
 *
 * To prevent out of order transfer conditions
 * USBPORT_Core_ProcessScatterGatherList() cannot call
 * USBPORT_Core_iSubmitTransferToMiniport() if there are other
 * previously mapped transfers for the same endpoint for which
 * USBPORT_Core_iSubmitTransferToMiniport() has returned
 * USBMP_STATUS_BUSY.  Any such transfers are resubmitted again to the
 * miniport when USBPORT_Core_UsbMapDpc_Worker() processes transfers on
 * the FDO_EXTENSION->TxPriorityListHead list which are in the TX_Mapped
 * state.

--*/
{
    PDEVICE_EXTENSION       devExt;
    PHCD_TRANSFER_CONTEXT   transfer;

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_iBegin(HcFdo, UsbMapDpc);

    // The TxMapBusy flag prevents the map Dpc from running while a map opertion (put-scatter-gather callback) is in
    // progress it is cleared from the put-scatter-gather callback or on error
    if (devExt->Fdo.TxMapBusy) {
        //
        // Already busy mapping a transfer.  In all code paths where
        // the TxMapBusy state is reset the UsbMapDpc will be signaled
        // to run again so there is no need or value in signaling the
        // UsbMapDpc to run again here.  Doing so would just cause a
        // UsbMapDpc execution loop.
        //

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Map!', 0, 0, 0);

        USBPORT_TxReleaseLock(HcFdo, UsbMapDpc, Irql);

        USBPORT_Xdpc_End(HcFdo, UsbMapDpc);
        return;
    }

    transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbMapDpc);

    while (transfer) {
        PHCD_ENDPOINT endpoint;

        endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Map0', 0, endpoint, transfer);

        switch (transfer->TxState ) {
        case TX_InQueue:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'Map4', 0, endpoint, transfer);

            //maxreq++;
            USBPORT_Core_iIncEpHwPendingTransferCount(HcFdo, endpoint, transfer);

            USBPORT_TxSetState(HcFdo, UsbMapDpc, transfer, Ev_TX_MapTransfer, TX_MapPending);

            // dpc is rescheduled when iomap maps the transfer
            devExt->Fdo.TxMapBusy = 1;

            USBPORT_Core_iMapTransfer(HcFdo, UsbMapDpc, transfer, endpoint, Irql);
            return;

        case TX_Mapped:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'Map5', 0, endpoint, transfer);
            // miniport was busy last time, attempt to calldown to miniport now

            // A TX_Mapped transfer should be on the TxMappedNotInMiniportListHead
            //
            USBPORT_ASSERT(!IsListEmpty(&endpoint->TxMappedNotInMiniportListHead));

            USBPORT_DBG_ASSERT_ISONLIST(&endpoint->TxMappedNotInMiniportListHead,
                                        &transfer->TxMappedNotInMiniportListEntry);

            // A transfer for an endpoint should not not be submitted to
            // the miniport unless it is the first transfer on the
            // TxMappedNotInMiniportListHead list to prevent out of
            // order transfer submission to the miniport.
            //
            if (transfer == CONTAINING_RECORD(endpoint->TxMappedNotInMiniportListHead.Flink,
                                              HCD_TRANSFER_CONTEXT,
                                              TxMappedNotInMiniportListEntry)) {

                USBPORT_Core_iSubmitTransferToMiniport(HcFdo, UsbMapDpc, transfer, TRUE);

            }

            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbMapDpc);
            break;

        default:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'Map6', 0, endpoint, transfer);
            // not interesting to us
            transfer =  USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbMapDpc);
            break;

        }

        if (USBPORT_Xdpc_iTimeExpired(HcFdo, UsbMapDpc)) {

            // schedule USBMAP dpc to run again since we didn't finish
            if (USBPORT_iNeedMapDpc(HcFdo)) {
                USBPORT_Xdpc_iSignal(HcFdo, UsbMapDpc, NULL);
            }
            break;
        }

    }

    USBPORT_ASSERT(devExt->Fdo.TxMapBusy == 0);
    USBPORT_TxReleaseLock(HcFdo, UsbMapDpc, Irql);
    USBPORT_Xdpc_End(HcFdo, UsbMapDpc);
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbHcIntDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbHcIntDpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

 * USBPORT main worker DPC/XDPC: 2 of 4
 *
 * After transfers are queued on the hardware in the miniport they are
 * globally handed by USBPORT_IsrDpc() queuing a call to one of the four
 * main worker DPCs in USBPORT, USBPORT_Core_UsbHcIntDpc_Worker().
 *
 * The main job of USBPORT_Core_UsbHcIntDpc_Worker() is to walk the
 * FDO_EXTENSION->TxPriorityListHead list of all transfers and look for
 * transfers in the TX_Mapped_inMp state (indicating that miniport
 * accepted the transfer when USBPORT_Core_iSubmitTransferToMiniport()
 * was called) and call MPf_PollEndpoint().
 *
 * This is one area where things sub-optimal in the current USB
 * implementation.  Anytime any interrupt for any reason is acknowledged
 * by a host controller USBPORT_Core_UsbHcIntDpc_Worker() will execute
 * for that controller and poll all endpoints on that controller which
 * have active transfers

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    ULONG mpOptionFlags = 0;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'hci0', 0, 0, 0);

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_iBegin(HcFdo, UsbHcIntDpc);

    transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbHcIntDpc);

    while (transfer) {
        PHCD_ENDPOINT endpoint;

        LOGENTRY(HcFdo, LOG_TRANSFER, 'hci2', 0, 0, 0);

        switch (transfer->TxState) {
        case TX_Mapped_TerminateSplit:
             USBPORT_Split_iCompleteChildTransfer(HcFdo,
                                                  UsbHcIntDpc,
                                                  transfer,
                                                  USBD_STATUS_CANCELED,
                                                  Ev_TX_CompleteTerminateSplit,
                                                  TX_HwDone);

            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbHcIntDpc);
            break;

        case TX_Mapped_inMp:

            // transfer references the endpoint, check the endpoint.  Since we poll only by endpoint transfers will
            // still be completed in the correct order. We use the transfer list as a 'hint' for which endpoints
            // to poll.

            endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
            USBPORT_iAssertEndpoint(HcFdo, endpoint);


            LOGENTRY(HcFdo, LOG_TRANSFER, 'hci4', endpoint, 0, 0);

            // We are walking a list of transfers, that may have
            // multiple transfers for the same endpoint.
            // We want to poll all the endpoint that have transfers
            // in state TX_Mapped_inMp.
            // Note that there may be multiple transfers for the same endpoint
            // that are in the TX_Mapped_inMp state. In addition this DPC
            // may run more than once for 1 hardware interrupt dpc
            // (USBPORT_IsrDpc). (Due to USBPORT_Xdpc_iTimeExpired logic below)

            // However we only want to poll an endpoint only once per hardware
            // interrupt. Thus we leverage devExt->Fdo.UsbHcIntDpcGenerationCount
            // to idenfify which which endpoints have been polled for this
            // Interrupt
            if (endpoint->UsbHcIntDpcGenerationCount != devExt->Fdo.UsbHcIntDpcGenerationCount) {

                endpoint->UsbHcIntDpcGenerationCount = devExt->Fdo.UsbHcIntDpcGenerationCount;

                // this may re-enter at USBPORTSVC_CompleteTransfer(),
                // transfer will transition to HwDone
                //MPf_PollEndpoint(devExt, endpoint);
                // poll operations will now show up in the endpoint state log
                USBPORT_iSetGlobalEndpointState(HcFdo, endpoint, Ev_gEp_PollEndpoint);
            }

            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbHcIntDpc);
            break;

        case TX_Mapped_inMp_Canceled:

            LOGENTRY(HcFdo, LOG_TRANSFER, 'hci5', 0, 0, 0);
            USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);

            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbHcIntDpc);
            break;

        case TX_Mapped:

            //
            // This was added for Wireless USB, which can return STATUS_BUSY
            // on SubmitTransfer even when there aren't any transfers pending,
            // thus there will never be a transfer completion to cause us to
            // try to submit the transfer again unless we signal the DPC here
            //

            mpOptionFlags = devExt->Fdo.HcOptionFlags;

            if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_BUSY_WHILE_IDLE)) {
                if (USBPORT_iNeedMapDpc(HcFdo)) {
                   USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, NULL);
                }
            }

            __fallthrough;

        default:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'hci6', 0, 0, 0);
            transfer = USBPORT_Core_iGetNextPriorityTransfer(HcFdo, UsbHcIntDpc);
            break;

        }


        if (USBPORT_Xdpc_iTimeExpired(HcFdo, UsbHcIntDpc)) {
            LOGENTRY(HcFdo, LOG_TRANSFER, 'hci7', 0, 0, 0);
            // schedule USBHCINT dpc to run again since we didn't finish
            USBPORT_Xdpc_iSignal(HcFdo, UsbHcIntDpc, NULL);

            USBPORT_TxReleaseLock(HcFdo, UsbHcIntDpc, Irql);
            USBPORT_Xdpc_End(HcFdo, UsbHcIntDpc);
            return;
        }
    }

    USBPORT_TxReleaseLock(HcFdo, UsbHcIntDpc, Irql);
    USBPORT_Xdpc_End(HcFdo, UsbHcIntDpc);
    return;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_UnlinkTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PHCD_TRANSFER_CONTEXT CompletedTransfer
    )
/*++

Routine Description:

    Disassociates a transfer structure from a URB.  This routine also
    validates that the URB was not corrupted while pending.

Arguments:

    CompletedTransfer - transfer that we are about to free.

Return Value:

    Transfer structure that was previously assocaited with input URB.

--*/
{
    PHCD_TRANSFER_CONTEXT transfer, tmpTx;

    UNREFERENCED_PARAMETER(tmpTx);

    USBPORT_ASSERT(TEST_FLAG(Urb->Hdr.UsbdFlags, USBPORT_TRANSFER_ALLOCATED));

    transfer = Urb->pd.HcdTransferContext;
    Urb->pd.HcdTransferContext = USB_BAD_PTR;
    LOGENTRY(HcFdo, LOG_TRANSFER, 'ULtr', transfer, 0, CompletedTransfer);

    // These should match
    USBPORT_ASSERT(CompletedTransfer == transfer);

    // check for list corruption
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxCsqIrpLink);
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxDoneLink);
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxPriorityLink);
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxCancelIrpLink);
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxMappedNotInMiniportListEntry);
    USB_ASSERT_UNLINKED(&CompletedTransfer->TxClearTTCompletionListEntry);

#if DBG
    // scan iternal lists for this transfer
    tmpTx = USBPORT_Core_CheckTransferQueues(HcFdo, CompletedTransfer);
    USBPORT_ASSERT(tmpTx == NULL);
#endif

    return transfer;
}


USBD_STATUS
USBPORT_Core_AllocTransferEx(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PUSBD_DEVICE_HANDLE DeviceHandle,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT CompleteEvent,
    __in ULONG MillisecTimeout,
    __in_opt PURB_CALLBACK_ROUTINE UrbCallbackFunc,
    __in_opt PVOID UrbCallbackContext
    )
/*++

Routine Description:

    Allocate and initialize a transfer context.

Arguments:

    HcFdo - pointer to a device object

    Urb - a transfer request

    Irp - pointer to an I/O Request Packet
     (optional)

    CompleteEvent - event to signal on completion
     (optional)

    MillisecondTimeout - 0 indicates no timeout

Return Value:

    USBD status code

--*/
{
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus;
    USBD_STATUS usbdStatus;
    PUSBD_PIPE_HANDLE_I pipeHandle;
    ULONG   sgCount;
    ULONG   numScatterGatherLists;
    PUCHAR  currentVa;
    ULONG   privateLength, sgListSize, isoListSize;
    ULONG   sgListSize2;
    ULONG sgSizeNeeded;
    PHCD_ENDPOINT endpoint;

    PUCHAR  pch;
    ULONG   i;
    PDMA_ADAPTER adapterObject;
    PCALCULATE_SCATTER_GATHER_LIST_SIZE  calcSGListSize;
    PVOID   sgBuffer;
    ULONG   sgBufferLen;
    BOOLEAN doubleBuffered = FALSE;
    BOOLEAN physicallyContiguousBufferRequired;
    PUCHAR  origBuffer = NULL;
    PVOID   doubleBuffer = NULL;
    PMDL    doubleBufferMdl = NULL;
    ULONG   packet;
    ULONG   firstPacketOfNextTd;
    ULONG   firstPacketOfCurrTd=0;
    ULONG   numPacketsPerTd=0;
    ULONG   firstPacketOfNextTdOffset;
    ULONG   tdLength;
    ULONG   lengthRemaining;
    ULONG   transferBufferLength;
    ULONG   offset;
    LIST_ENTRY isoDoubleBufferListHead;
    BOOLEAN doubleBufferThisTd = FALSE;
    PUSBPORT_DB_HANDLE dbHandle;
    PUSBPORT_DB_HANDLE dbHandleNext;
    PHYSICAL_ADDRESS maxPhysicalAddress;

    maxPhysicalAddress.QuadPart = 0xFFFFFFFFFFFFFFFF;

    // allocate a transfer context and initialize it

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(Urb != NULL);

    usbdStatus = USBD_STATUS_SUCCESS;
    pipeHandle = Urb->UsbdPipeHandle;
    ASSERT_PIPE_HANDLE(pipeHandle);
    endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);
    ASSERT_ENDPOINT(endpoint);
    adapterObject = devExt->Fdo.AdapterObject;
    calcSGListSize = adapterObject->DmaOperations->CalculateScatterGatherList;
    sgBuffer = NULL;
    sgBufferLen = 0;

    InitializeListHead(&isoDoubleBufferListHead);

    USBPORT_ASSERT(!TEST_FLAG(Urb->Hdr.UsbdFlags, USBPORT_TRANSFER_ALLOCATED));

    LOGENTRY(HcFdo, LOG_TRANSFER, 'urbi',
             Urb->TransferBuffer,
             Urb->TransferBufferLength,
             Urb->TransferFlags);

    do {

        //
        // NVIDIA Arm platform has a bug, due to which the a scatter gather DMA operation would
        // corrupt the system for a transfer that matches the following criteria:
        //    *  a transfer buffer address is not 32 bytes and
        //       + its spans non-contiguous physical pages.
        // Windows 8 Bug: 79552
        //













































































        // see how much space we will need for the sg list
        if (Urb->TransferBufferLength &&
            devExt->Fdo.NumberOfMapRegisters != 0) {

            if (doubleBuffered) {
                currentVa = MmGetMdlVirtualAddress(doubleBufferMdl);
                NT_ASSERT(endpoint->Parameters.TransferType != Isochronous);
            } else {
                currentVa = MmGetMdlVirtualAddress(Urb->TransferBufferMDL);
            }


            sgCount = USBPORT_ADDRESS_AND_SIZE_TO_SPAN_PAGES_4K(
                          currentVa, Urb->TransferBufferLength);

            // Compute how many SCATTER_GATHER_ENTRY entries are necessary.
            // One entry is needed for each call to GetScatterGatherList()
            // that is made for the transfer in USBPORT_FlushMapTransferList().
            //
            // Each call to GetScatterGatherList() can span a region of the
            // total transfer buffer that is no larger than can be mapped
            // by the number of map registers in the DMA adapter object.
            //
            numScatterGatherLists = 0;

            lengthRemaining = Urb->TransferBufferLength;



































            while (lengthRemaining) {
                ULONG   sgLen;

                // Consume a region of the transfer buffer up to N page
                // boundaries forward, where N is the number of map
                // registers in the DMA adapter object.
                //
                transferBufferLength = (devExt->Fdo.NumberOfMapRegisters << PAGE_SHIFT)
                                       - BYTE_OFFSET(currentVa);

                // But no further than the actual remaining length of the
                // transfer buffer.
                //
                if (transferBufferLength > lengthRemaining) {
                    transferBufferLength = lengthRemaining;
                }










































































































































                if (!NT_SUCCESS((*calcSGListSize)(
                        adapterObject,
                        NULL,
                        currentVa,
                        transferBufferLength,
                        &sgLen,
                        NULL))) {
                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    break;
                }

                sgBufferLen += sgLen;

                numScatterGatherLists++;

                currentVa += transferBufferLength;

                lengthRemaining -= transferBufferLength;
            }
        } else {
            // zero length transfer
            currentVa = NULL;
            sgCount = 0;
            numScatterGatherLists = 0;
        }

        if (usbdStatus != USBD_STATUS_SUCCESS) {
            break;
        }

        // sizeof <transfer> + <sgList>
        sgListSize = sizeof(HCD_TRANSFER_CONTEXT) +
                     sizeof(TRANSFER_SG_ENTRY)*sgCount;

        // sizeof transfer->ScatterGatherEntries
        //
        sgListSize2 = sizeof(SCATTER_GATHER_ENTRY) * numScatterGatherLists;

        // if this is an iso transfer we need to alloc the
        // packet structure as well
        if (Urb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER) {
            isoListSize =
                sizeof(MINIPORT_ISO_TRANSFER) +
                sizeof(MINIPORT_ISO_PACKET)*Urb->u.Isoch.NumberOfPackets;
        } else {
            isoListSize = 0;
        }

        privateLength = sgListSize + sgListSize2 + isoListSize;

        LOGENTRY(HcFdo, LOG_TRANSFER, 'TRcs',
                 REGISTRATION_PACKET(devExt).TransferContextSize,
                 privateLength,
                 sgCount);

        // Now allocate the dma resource
        //
        if (sgBufferLen) {
#ifdef USB_FORCE_MEMORY_STRESS
            if (!FP_FAIL_RESOURCE_ALLOCATION(devExt, FALSE)) {
#endif

            sgBuffer = UsbTryAllocPoolFromNonPagedLookaside_Z(sgBufferLen);

#ifdef USB_FORCE_MEMORY_STRESS
            }
#endif

            if (!sgBuffer) {
                sgBuffer = FPAllocSgBuf(devExt,
                                        DeviceHandle,
                                        endpoint,
                                        Urb->TransferFlags,
                                        Urb->TransferBufferLength,
                                        &sgBufferLen);
            }

            if (!sgBuffer) {
                usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
        }


        if (!NT_SUCCESS(RtlULongAdd(privateLength,
                          REGISTRATION_PACKET(devExt).TransferContextSize,
                          &sgSizeNeeded))) {
            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            break;
        }

#ifdef USB_FORCE_MEMORY_STRESS
        if (!FP_FAIL_RESOURCE_ALLOCATION(devExt, FALSE)) {
#endif

        transfer = UsbTryAllocPoolFromNonPagedLookaside_Z(sgSizeNeeded);

#ifdef USB_FORCE_MEMORY_STRESS
        }
#endif

        if (transfer == NULL) {
            transfer = FPAllocTransferContext(devExt,
                                              DeviceHandle,
                                              endpoint,
                                              Urb->TransferFlags,
                                              sgSizeNeeded);
        }

        if (transfer == NULL) {
            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'noTR',
                     sgSizeNeeded, endpoint->FpContext, 0);
            break;
        }

        LOGENTRY(HcFdo, LOG_TRANSFER, 'alTR', transfer, Urb, Irp);

        // init the transfer context
        transfer->Sig = SIG_TRANSFER;
        transfer->Flags = 0;
        transfer->MillisecondTimeout = MillisecTimeout;
        transfer->Irp = Irp;
        transfer->Urb = Urb;
        transfer->TxState = TX_Undefined;
        transfer->UrbDeviceHandle = DeviceHandle;
        transfer->CompleteEvent = CompleteEvent;
        //point to the master transfer for a set
        transfer->ParentTransfer = transfer;
        transfer->UsbdStatus = 0xFFFFFFFF;

        USB_ZAP_LINK(&transfer->TxDoneLink);
        USB_ZAP_LINK(&transfer->TxEndpointLink);
        USB_ZAP_LINK(&transfer->TxMappedNotInMiniportListEntry);
        USB_ZAP_LINK(&transfer->SplitLink);
        USB_ZAP_LINK(&transfer->TmpLink);
        USB_ZAP_LINK(&transfer->TxCsqIrpLink);
        USB_ZAP_LINK(&transfer->TxAbortLink);
        USB_ZAP_LINK(&transfer->TxPriorityLink);
        USB_ZAP_LINK(&transfer->TxCancelIrpLink);
        USB_ZAP_LINK(&transfer->TxClearTTCompletionListEntry);

        //transfer->Endpoint = pipeHandle->Endpoint;

        ntStatus = USBPORT_ReferenceEndpoint(HcFdo,
                                             endpoint,
                                             &transfer->EpRefHandle,
                                             transfer,
                                             EPREF_TRANSFER_TAG);
        if (NT_ERROR(ntStatus)) {
            usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(ntStatus);
            break;
        }

        transfer->MiniportContext = (PUCHAR) transfer;
        transfer->MiniportContext += privateLength;
        transfer->PrivateLength = privateLength;

        InitializeListHead(&transfer->DoubleBufferListHead);
        InitializeListHead(&transfer->IsoTegraDoubleBufferListHead);
        TRANSFER_LIST(&transfer->IsoTegraDoubleBufferListHead, &isoDoubleBufferListHead);

        transfer->NumScatterGatherLists = numScatterGatherLists;
        transfer->NumScatterGatherListsRemaining = numScatterGatherLists;

        // The array of SCATTER_GATHER_ENTRY entries is located
        // immediately after the main HCD_TRANSFER_CONTEXT structure
        // and the TRANSFER_SG_LIST and array of TRANSFER_SG_ENTRY
        // entires it contains.
        //
        pch = (PUCHAR) transfer;
        pch += sgListSize;

        if (sgListSize2) {
            transfer->ScatterGatherEntries = (PSCATTER_GATHER_ENTRY) pch;
            USBPORT_ASSERT(transfer->NumScatterGatherLists != 0);
        }

        // The MINIPORT_ISO_TRANSFER and array of MINIPORT_ISO_PACKET
        // it contains is located immediately after the array of
        // SCATTER_GATHER_ENTRY entries.
        //
        pch += sgListSize2;

        if (isoListSize) {
            transfer->IsoTransfer = (PMINIPORT_ISO_TRANSFER) pch;
            transfer->IsoPeriod =
                endpoint->Parameters.Period;
        } else {
            transfer->IsoTransfer = NULL;
            transfer->IsoPeriod = 0;
        }

        transfer->TotalLength = privateLength +
            REGISTRATION_PACKET(devExt).TransferContextSize;

        transfer->SgList.SgCount = 0;

        // we don't know the direction yet
        transfer->Direction = NotSet;

        if (DeviceHandle == NULL) {
            // no oca data available for internal function
            transfer->DeviceVID = 0xFFFF;
            transfer->DevicePID = 0xFFFF;
            for (i=0; i<USB_DRIVER_NAME_LEN; i++) {
                transfer->DriverName[i] = '?';
            }
        } else {
            // no oca data available for internal function
            transfer->DeviceVID = DeviceHandle->DeviceDescriptor.idVendor;
            transfer->DevicePID = DeviceHandle->DeviceDescriptor.idProduct;
            for (i=0; i<USB_DRIVER_NAME_LEN; i++) {
                transfer->DriverName[i] = DeviceHandle->DriverName[i];
            }
        }

        if (isoListSize) {
            SET_FLAG(transfer->Flags, USBPORT_TXFLAG_ISO);
        }

        if (UrbCallbackFunc) {
            // mark it so we call USBPORT_SendCommandExComplete when it is done
            SET_FLAG(transfer->Flags, USBPORT_TXFLAG_URB_CALLBACK);
            transfer->UrbCallbackFunc = UrbCallbackFunc;
            transfer->UrbCallbackContext = UrbCallbackContext;
        }

        SET_FLAG(Urb->Hdr.UsbdFlags, USBPORT_TRANSFER_ALLOCATED);

        if (doubleBuffered) {
            transfer->DoubleBufferDueToTegraDmaBug = TRUE;
            transfer->OrigBuffer = origBuffer;
            transfer->DoubleBuffer = doubleBuffer;
            transfer->DoubleBufferMdl = doubleBufferMdl;

            doubleBuffer = NULL;
            doubleBufferMdl = NULL;
        }

        transfer->SGBuffer = sgBuffer;
        transfer->SGBufferLength = sgBufferLen;
        sgBuffer = NULL;

        Urb->pd.HcdTransferContext = transfer;
        transfer = NULL;
        Urb->pd.UrbSig = URB_SIG;

        usbdStatus = USBD_STATUS_SUCCESS;
    } WHILE(0);

    if (sgBuffer && !FPFreeSgBuf(devExt,
                                 DeviceHandle,
                                 endpoint,
                                 sgBuffer)) {
        UsbFreeToLookaside(sgBuffer);
    }

    if (transfer && !FPFreeTransferContext(devExt,
                                           DeviceHandle,
                                           endpoint,
                                           transfer)) {
        UsbFreeToLookaside(transfer);
    }

    //
    // Cleanup isoDoubleBufferListHead
    //
    FOR_ALL_IN_LIST_SAFE(USBPORT_DB_HANDLE,
                         &isoDoubleBufferListHead,
                         DbLink,
                         dbHandle,
                         dbHandleNext) {

        RemoveEntryList(&dbHandle->DbLink);

        USBPORT_FreeDbHandleForIsoOnTegraFinal(dbHandle);

    }


    if (doubleBufferMdl) {
        IoFreeMdl(doubleBufferMdl);
    }

    if (doubleBuffer) {
        UsbFreePool(doubleBuffer);
    }

    return usbdStatus;
}


USBD_STATUS
USBPORT_Core_AllocTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PUSBD_DEVICE_HANDLE DeviceHandle,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT CompleteEvent,
    __in ULONG MillisecTimeout
    )
{
    return USBPORT_Core_AllocTransferEx(HcFdo, Urb, DeviceHandle, Irp, CompleteEvent, MillisecTimeout, NULL, NULL);
}


VOID
USBPORT_Core_QueueTransferUrb(
    __inout PTRANSFER_URB Urb
    )
/*++

Routine Description:

    Queues a transfer, either internal (no irp) or external (irp).  This routime will insert the irp into the cancel
    safe queue. All transfers are completed asynchronously.

--*/
{
    PHCD_TRANSFER_CONTEXT transfer;
    PDEVICE_OBJECT hcFdo;
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;
    MP_ENDPOINT_STATUS epStatus;
    PUSBD_DEVICE_HANDLE deviceHandle;

    // on entry the urb is not cancelable ie
    // no cancel routine
    transfer = Urb->pd.HcdTransferContext;
    ASSERT_TRANSFER(transfer);

    if (Urb->Hdr.Function == URB_FUNCTION_CONTROL_TRANSFER_EX) {
        // set the specified timeout for this transaction
        transfer->MillisecondTimeout= Urb->Timeout;
    } else if (TEST_FLAG(Urb->TransferFlags, USBD_DEFAULT_PIPE_TRANSFER)) {
        // to maintain backward compatibility munge the urb function
        // code for control transfers that use the default pipe, just like
        // usbd did.
        Urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
    }

    endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);

    hcFdo = endpoint->FdoDeviceObject;

    LOGENTRY(hcFdo, LOG_TRANSFER, 'quTR', transfer, endpoint, Urb);

    GET_DEVICE_EXT(devExt, hcFdo);
    ASSERT_FDOEXT(devExt);

    // epstatus not used by root hubs
    if (endpoint->EpFlags.RootHub) {
        epStatus = ENDPOINT_STATUS_RUN;
    } else {

        endpoint->EpFlags.Pure = 0;
        // this will sync up the ep status with the vakue we store in the endpoint
        MPf_GetEndpointStatus(devExt, endpoint);
    }
    // copy the transfer parameters from the URB
    // to our structure
    transfer->Tp.TransferBufferLength =
        Urb->TransferBufferLength;
    transfer->Tp.TransferFlags =
        Urb->TransferFlags;

    if (transfer->DoubleBufferDueToTegraDmaBug) {
        transfer->TransferBufferMdl =
            transfer->DoubleBufferMdl;
        transfer->OrigBufferMdl =
            Urb->TransferBufferMDL;
    } else {
        transfer->TransferBufferMdl =
            Urb->TransferBufferMDL;
    }

    transfer->Tp.MiniportFlags = 0;

    if (TEST_FLAG(Urb->Hdr.UsbdFlags, USBPORT_REQUEST_MDL_ALLOCATED)) {
        SET_FLAG(transfer->Flags, USBPORT_TXFLAG_MDL_ALLOCATED);
    }

    if (endpoint->Parameters.TransferType == Control) {
        RtlCopyMemory(&transfer->Tp.SetupPacket[0],
                      &Urb->u.SetupPacket[0],
                      8);
    }


    // we should know the direction and type by now
    transfer->EpTransferType = endpoint->Parameters.TransferType;

    if (Urb->TransferFlags & USBD_TRANSFER_DIRECTION_IN) {
        transfer->Direction = ReadData;
    } else {
        transfer->Direction = WriteData;
    }


    // set URB bytes transferred to zero bytes transferred
    // when this urb completes this value should contain
    // actual bytes transferred -- this will ensure we return
    // zero in the event of a cancel
    Urb->TransferBufferLength = 0;

    // Historical Note:
    // The UHCD driver failed requests to endpoints that were halted
    // we need to preserve this behavior because transfer queued to a
    // halted endpoint will not complete unless the endpoint is resumed
    // or canceled. Some clients (HIDUSB) rely on this behavior when
    // canceling requests as part of an unplug event.
// bugbug the miniports need to be fixed to correctly reflect the
// ep status (USBUHCI)
//    if (epStatus == ENDPOINT_STATUS_HALT) {
//        TEST_TRAP();
//    }

    GET_DEVICE_HANDLE(deviceHandle, Urb);
    ASSERT_DEVICE_HANDLE(deviceHandle);

    if (endpoint->EpFlags.RootHub) {
        // transfer currently references the endpoint, this will queue a
        // DPC to process the transfer or complete it synchronously
        USBPORT_RH_NeoQueueAsyncTransfer(hcFdo,
                                         endpoint,
                                         transfer->Irp,
                                         transfer);
    } else {
        USBPORT_TxQueueTransfer(hcFdo,
                                endpoint,
                                transfer,
                                transfer->Irp);
    }
}


VOID
USBPORT_Core_iInsertPriorityTransferDirect(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT *Transfer,
    __inout PLIST_ENTRY InsertPoint
    )
/*++

Routine Description:

    This routines performs the actual insertion of the transfer on the
    Priority Queue (i.e. devExt->Fdo.TxPriorityListHead) and updates the
    xdpc next pointers.

    It is the responsibility of the caller to set the transfer IrpState
    to indicate that the transfer is now on the Priority Queue.

    This routine is used by both the Irp path and the internal path that
    inserts splits.

    If all transfers are currently being aborted on the endpoint then
    this routine will cancel the transfer after inserting it on the
    Priority Queue.  In that case the Transfer pointer parameter will be
    set to NULL to indicate that the transfer should no longer be
    refereneced by the caller.

 --*/
{
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT insertTransfer;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Inp0', InsertPoint, 0, *Transfer);

    if (InsertPoint == &devExt->Fdo.TxPriorityListHead) {
        insertTransfer = NULL;
    } else {
        insertTransfer =  CONTAINING_RECORD(InsertPoint,
                                      HCD_TRANSFER_CONTEXT,
                                      TxPriorityLink);
    }

    le = devExt->Fdo.XdpcListHead.Flink;

    while (le != &devExt->Fdo.XdpcListHead) {
        PXDPC_CONTEXT xdpc;

        xdpc = (PXDPC_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _XDPC_CONTEXT,
                        XdpcLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Inp1', le, 0, xdpc);
        ASSERT_XDPC(xdpc);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Inp2', xdpc->NextTransfer, *Transfer, xdpc);

        //is it current on this xdpc?
        if (xdpc->CurrentTransfer &&
            xdpc->CurrentTransfer == insertTransfer) {
            // update next

            TEST_TRAP();
            xdpc->NextTransfer = *Transfer;
        }

        le = le->Flink;
    }


    // New transfers are queued to the tail and removed from the head
    InsertTailList(InsertPoint,  &(*Transfer)->TxPriorityLink);

    // Update the host controller Async and Periodic transfer counts.
    // Notify the miniport if the count increments from 0 -> 1.
    //
    switch ((*Transfer)->EpTransferType)
    {
        case Bulk:
        case Control:
            if (++devExt->Fdo.TxPriorityListAsyncCount == 1)
            {
                MPf_NotifyTransferQueueState(devExt,
                                             devExt->Fdo.TxPriorityListAsyncCount,
                                             devExt->Fdo.TxPriorityListPeriodicCount);
            }
            break;

        case Interrupt:
        case Isochronous:
            if (++devExt->Fdo.TxPriorityListPeriodicCount == 1)
            {
                MPf_NotifyTransferQueueState(devExt,
                                             devExt->Fdo.TxPriorityListAsyncCount,
                                             devExt->Fdo.TxPriorityListPeriodicCount);
            }
            break;
    }

    // Now that the transfer is finally queued, check for a race
    // condition with USBPORT_AbortAllTransfers() or USBPORT_ClosePipe()
    // and cancel the transfer if all transfers on the endpoint are
    // being aborted.
    //
    endpoint = USBPORT_EndpointFromHandle((*Transfer)->EpRefHandle);

    if (endpoint->EpFlags.DeviceGone || endpoint->EpFlags.PipeClosed || devExt->Fdo.DmTimerFlags.FailUrbs) {
        USBPORT_Core_iCancelPriorityTransfer(HcFdo, *Transfer);
        *Transfer = NULL;
    }

    //** currently only the map dpc can be interrupted while walking the list so we update the xdpc transfer
    //** pointers from the map routine
}


VOID
USBPORT_Core_iInsertPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Context: DRIVER (IoInsertCsq)

    Queues a transfer to the global priority list and signals the USB_MAP DPC to run.

 --*/
{
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY insertPoint;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Inq0', 0, 0, Transfer);

    ASSERT_TRANSFER(Transfer);
    USB_ASSERT_UNLINKED(&Transfer->TxPriorityLink);

    endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);

    USBPORT_ASSERT(Transfer->TxState == TX_Undefined);
    USBPORT_TxSetState(HcFdo, NULL, Transfer, Ev_TX_Icsq, TX_InQueue);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Inq1', 0, endpoint, Transfer);

    USBPORT_ASSERT(Transfer->TxType != Tx_SPLIT_CHILD);
    // assign a sequence number
    // ** all spilt children should have the same seq number as their parent
    Transfer->ParentIdx =
        Transfer->Tp.SequenceNumber = InterlockedIncrement(&devExt->Fdo.NextTransferSequenceNumber);
    Transfer->UsbdStatus = USBD_STATUS_PENDING;
    SET_USBD_ERROR(Transfer->Urb, Transfer->UsbdStatus);

    insertPoint = &devExt->Fdo.TxPriorityListHead;

    USBPORT_Core_iInsertPriorityTransferDirect(HcFdo, &Transfer, insertPoint);
    KeResetEvent(&devExt->Fdo.TxPriorityListEmpty_Event);

    // signal the USBMAP dpc to map this transfer and post it to the miniport.
    USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, Transfer);
}


VOID
USBPORT_Core_LockInsertPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Context: DRIVER (IoInsertCsq)

    Queues a transfer to the global priority list and signals the USB_MAP DPC to run.

 --*/
{
    KIRQL irql;

    irql = USBPORT_TxAcquireLock(HcFdo, NULL);

    USBPORT_Core_iInsertPriorityTransfer(HcFdo, Transfer);

    USBPORT_TxReleaseLock(HcFdo, NULL, irql);

}

ULONG USBPORT_TxTrapMask = 0;

#define TX_TRAP                        0x00000001
#define TX_TRAP_ZERO_LENGTH            0x00000002
#define TX_TRAP_IRP                    0x00000004
#define TX_TRAP_NTSTATUS_SUCESS        0x00000008
#define TX_TRAP_USBDSTATUS_SUCCESS     0x00000010



VOID
USBPORT_Core_iTrap_TransferComplete(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in LONG NtStatus
    )
/*++

Routine Description:

This routine can be used to break on specifi transfer conditions when debugging:

example:


to break on success with zero bytes
NTSTATUS = SUCCESS, urb = SUCCESS, Irp, zero length transfer set the mask to:  x1f.

to break on error with zero bytes
NTSTATUS = !SUCCESS, urb = !SUCCESS, Irp, zero length transfer set the mask to x07



--*/
{
    ULONG mask = TX_TRAP;

    if (USBPORT_TxTrapMask == 0) {
        return;
    }
    // break if zero length
    if (Transfer->MiniportBytesTransferred == 0) {
        mask |= TX_TRAP_ZERO_LENGTH;
    }

    if (Transfer->UsbdStatus == USBD_STATUS_SUCCESS) {
        mask |= TX_TRAP_USBDSTATUS_SUCCESS;
    }

    if (Transfer->Irp) {
        mask |= TX_TRAP_IRP;
    }

    if (NT_SUCCESS(NtStatus)) {
        mask |= TX_TRAP_NTSTATUS_SUCESS;
    }

    if (mask == USBPORT_TxTrapMask) {
        DbgPrint("USBPORT Transfer trapped mask = %x, transfer: !usbtx %p\n", mask, Transfer);
        DbgPrint("\tHcFdo %p\n", HcFdo);
        NT_ASSERTMSG("mask cannot be USBPORT_TxTrapMask", FALSE);
    }
}


VOID
USBPORT_Core_iCompleteTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in USBD_STATUS UsbdCompleteCode,
    __in TX_EVENT TxEvent,
    __in TX_STATE TxState
    )
/*++

Routine Description:

   Mark a transfer done and signal the USBDONE dpc to run

Arguments:

    Xdpc - current Dpc context (may be NULL)

    UsbdCompleteCode - USB completion status, mapped to NTSTATUS code for the irp.

    TxEvent - event to be logged with the completion

    TxState -

    This routine is always called with Fdo.EndpointListSpin held
--*/
{
    PDEVICE_EXTENSION   devExt;
    PHCD_ENDPOINT       endpoint;

    devExt = GetHcFdoExt(HcFdo);

    ASSERT_TRANSFER(Transfer);

    if (Transfer->TxType == Tx_SPLIT_CHILD) {

        USBPORT_Split_iCompleteChildTransfer(HcFdo,
                                             Xdpc,
                                             Transfer,
                                             UsbdCompleteCode,
                                             TxEvent,
                                             TxState);
    } else {

        endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);

        USBPORT_iAssertEndpoint(HcFdo, endpoint);

        USBPORT_TxSetState(HcFdo, Xdpc, Transfer, TxEvent, TxState);

        // irp & urb error set when transfer is completed to client
        Transfer->UsbdStatus = UsbdCompleteCode;

        LOGENTRY(HcFdo, LOG_TRANSFER, 'cpt0', UsbdCompleteCode, endpoint, Transfer);

        // signal the done transfer Dpc to run (USBDONE)
        USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbDoneDpc, Transfer);

        if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_MPCANCELED)) {

            // Since the transfer was aborted/canceled while actively
            // queued in the miniport hardware it may be necessary to
            // issue a CLEAR_TT_BUFFER request to the upstream TT hub
            // to clear any partial transfer state from the TT buffer.
            //
            if (endpoint->EpFlags.ClearTtBufferOnCancel) {

                USB_ASSERT_UNLINKED(&Transfer->TxClearTTCompletionListEntry);

                //
                // Add this transfer to the list of transfers on this
                // endpoint which have been canceled or aborted while
                // actively queued in the miniport and then completed
                // from a hardware point of view, but which are waiting
                // for the completion of a CLEAR_TT_BUFFER request
                // before the transfers are finally completed from a
                // software point of view.
                //
                InsertTailList(&endpoint->TxCanceledPendingClearTTCompletionListHead,
                               &Transfer->TxClearTTCompletionListEntry);

                //
                // Setting the endpoint state to Ev_gEp_ClearTTNeeded
                // will prevent the endpoint from becoming active again
                // until after the CLEAR_TT_BUFFER request has
                // completed.
                //
                USBPORT_iSetGlobalEndpointState(HcFdo, endpoint, Ev_gEp_ClearTTNeeded);
            }
        }
    }
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iCompleteDoneTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    All transfer completions come thru here -- this is where  we actually comlete the irp.

    We assume all fields are set in the URB for completion except the status.


--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT endpoint;
    PKEVENT event;
    NTSTATUS ntStatus;
    KIRQL oldIrql, statIrql;
    ULONG i;
    ULONG numScatterGather;
    PUSBD_ISO_PACKET_DESCRIPTOR usbdPak;
    ULONG flushLength = 0;
    PUSBD_DEVICE_HANDLE devH;
    PTRANSFER_URB urb;
    PDMA_ADAPTER adapterObject = NULL;
    BOOLEAN writeData = FALSE;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    ULONG flags;
    ULONG miniportBytesTransferred;
    PUSBOBJ_REF epRefHandle;
    PURB_CALLBACK_ROUTINE urbCallback;
    PVOID urbCallbackCtx;
    ETW_EXECUTION_METRICS execMetrics;
    PUCHAR currentVa = NULL;
    INIT_EXEC_METRICS(execMetrics);

    urb = Transfer->Urb;
    usbIoRequest = NULL;
    numScatterGather = 0;

    ASSERT_TRANSFER_URB(urb);
    USBPORT_ASSERT(urb->pd.HcdTransferContext == Transfer);
    //Transfer = urb->pd.HcdTransferContext;

    USBPORT_ASSERT(Transfer->UsbdStatus != USBD_STATUS_PENDING);
    SET_USBD_ERROR(Transfer->Urb, Transfer->UsbdStatus);

    // we could extract this from the URB but this is safer
    devH = (PUSBD_DEVICE_HANDLE) Transfer->UrbDeviceHandle;

    // make sure we have the correct Transfer
    USBPORT_ASSERT(Transfer->Urb == urb);
    USBPORT_ASSERT(Transfer->TxType != Tx_SPLIT_CHILD);

    endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);
    USBPORT_iAssertEndpoint(HcFdo, endpoint);

    USBPORT_TxSetState(HcFdo, Xdpc, Transfer, Ev_TX_Free, TX_Free);
    //check if this transfer is attached to an abort-pipe request and update
    USBPORT_Core_iCheckAbortList(HcFdo, Xdpc, Transfer);

    USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);

    event = Transfer->CompleteEvent;
    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo,
             LOG_TRANSFER,
             'cptU',
             urb,
             Transfer->UsbdStatus,
             Transfer);

    urb->TransferBufferLength = Transfer->MiniportBytesTransferred;

    // This sets urb->UrbHeader.Status, AKA urb->Hdr.Status
    ntStatus = SET_USBD_ERROR(urb, Transfer->UsbdStatus);

    // for debugging purposes
    USBPORT_Core_iTrap_TransferComplete(HcFdo, Transfer, ntStatus);

    if (!NT_SUCCESS(ntStatus)){
        //
        // Increment our perf error counters
        //
        devH->PerformanceData.TransferErrors++;
        devExt->Fdo.PerformanceData.TransferErrors++;
    }

    // bytes transferred is set in the URB based on bytes received
    // or sent, update our counters before completion
    KeAcquireSpinLock(&devExt->Fdo.StatCounterSpin, &statIrql);
    switch(endpoint->Parameters.TransferType) {
    case Bulk:
        devExt->Fdo.PerformanceData.StatBulkDataBytes += Transfer->MiniportBytesTransferred;
        devExt->Fdo.PerformanceData.StatNumberBulkUrbs++;
        devH->PerformanceData.StatBulkDataBytes += Transfer->MiniportBytesTransferred;
        devH->PerformanceData.StatNumberBulkUrbs++;
        flushLength = Transfer->MiniportBytesTransferred;
        break;
    case Control:
        devExt->Fdo.PerformanceData.StatControlDataBytes += Transfer->MiniportBytesTransferred;
        devExt->Fdo.PerformanceData.StatNumberControlUrbs++;
        devH->PerformanceData.StatControlDataBytes += Transfer->MiniportBytesTransferred;
        devH->PerformanceData.StatNumberControlUrbs++;
        flushLength = Transfer->MiniportBytesTransferred;
        break;
    case Isochronous:
        devExt->Fdo.PerformanceData.StatIsoDataBytes += Transfer->MiniportBytesTransferred;
        devExt->Fdo.PerformanceData.StatNumberIsoUrbs++;
        devH->PerformanceData.StatIsoDataBytes += Transfer->MiniportBytesTransferred;
        devH->PerformanceData.StatNumberIsoUrbs++;
        flushLength = 0;
        for (i = 0; i < urb->u.Isoch.NumberOfPackets; i++) {
            usbdPak = &urb->u.Isoch.IsoPacket[i];
            if (usbdPak->Length != 0) {
                flushLength = usbdPak->Offset + usbdPak->Length;
            }
            if (usbdPak->Status != USBD_STATUS_SUCCESS){
                //
                // Late or error packets
                //
                devH->PerformanceData.DroppedIsoPackets++;
                devExt->Fdo.PerformanceData.DroppedIsoPackets++;
            }
        }
        break;
    case Interrupt:
        devExt->Fdo.PerformanceData.StatInterruptDataBytes += Transfer->MiniportBytesTransferred;
        devExt->Fdo.PerformanceData.StatNumberInterruptUrbs++;
        devH->PerformanceData.StatInterruptDataBytes += Transfer->MiniportBytesTransferred;
        devH->PerformanceData.StatNumberInterruptUrbs++;
        flushLength = Transfer->MiniportBytesTransferred;
        break;
    }
    KeReleaseSpinLock(&devExt->Fdo.StatCounterSpin, statIrql);

    // if we have an irp remove it from our internal lists
    LOGENTRY(HcFdo,
             LOG_TRANSFER,
             'CptX',
             Irp,
             Transfer->UsbdStatus,
             ntStatus);


    // Make a note of the current transfer so that we can put some
    // information about this transfer and its client driver in the
    // minidump in the event that we crash while attempting to complete
    // their IRP.
    //
    USBPORT_Core_RecordOcaTransferData(HcFdo, Transfer);


    // Free any DMA resources associated with this Transfer
    //
    if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_MAPPED)) {
        PPUT_SCATTER_GATHER_LIST    putSGList;
        PSCATTER_GATHER_ENTRY       scatterGatherEntry;

        USBPORT_ASSERT(Transfer->Direction != NotSet);

        writeData = Transfer->Direction == WriteData ? TRUE : FALSE;

        currentVa = MmGetMdlVirtualAddress(Transfer->TransferBufferMdl);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'dmF>', currentVa, flushLength, writeData);

        // Walk the array of SCATTER_GATHER_ENTRY entries that were
        // each the result of a call to GetScatterGatherList() and call
        // PutScatterGatherList() on each to free them.
        //
        // This takes the place of calling IoFlushAdapterBuffers() and
        // IoFreeMapRegisters() when GetScatterGatherList() is called
        // instead of IoAllocateAdapterChannel() and IoMapTransfer().
        //

        adapterObject = devExt->Fdo.AdapterObject;

        putSGList = adapterObject->DmaOperations->PutScatterGatherList;

        KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);
        //
        // PutScatterGatherList() must be called at DISPATCH_LEVEL

        for (i = 0, scatterGatherEntry = &Transfer->ScatterGatherEntries[0];
             i < Transfer->NumScatterGatherLists;
             i++, scatterGatherEntry++) {

            LOGENTRY(HcFdo, LOG_TRANSFER, 'dmF1', scatterGatherEntry, adapterObject, i);
            USBPORT_ASSERT(scatterGatherEntry->Sig == SIG_HAL_SG_LIST);
            USBPORT_ASSERT(scatterGatherEntry->Transfer == Transfer);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'dmF2', scatterGatherEntry->ScatterGatherList, 0, 0);

            if (scatterGatherEntry->ScatterGatherList) {

                ETW_MEASURE_TIME_START(execMetrics);

                (*putSGList)(adapterObject,
                             scatterGatherEntry->ScatterGatherList,
                             writeData);

                ETW_MEASURE_TIME_STOP(execMetrics);

                // OS will free this struct after we call the flush routine
                scatterGatherEntry->ScatterGatherList = NULL;
                scatterGatherEntry->Sig = SIG_FREE;

                numScatterGather++;

                if (EtwLoggingEnabled && EtwPerfLoggingEnabled) {

                    //
                    // Log DMA performance metrics
                    //
                    (VOID) USBPORT_EtwWriteDmaExecutionTime(
                            HcFdo,
                            devH,
                            endpoint,
                            &execMetrics,
                            &USBPORT_ETW_EVENT_PUT_SGLIST_EXECUTION_TIME,
                            NULL,
                            &adapterObject,
                            sizeof(PDMA_ADAPTER *),
                            &numScatterGather,
                            sizeof(ULONG),
                            &writeData,
                            sizeof(BOOLEAN),
                            &flushLength,
                            sizeof(ULONG),
                            &currentVa,
                            sizeof(PUCHAR *),
                            ETW_PUT_SG_LIST_TAG
                            );
                }
            }
        }

        // PutScatterGatherList() must be called at DISPATCH_LEVEL
        //
        KeLowerIrql(oldIrql);

        USBPORT_FlushAdapterDBs(HcFdo, Transfer);

        LOGENTRY(HcFdo, LOG_TRANSFER,
                 'dmaF', Transfer, Transfer->NumScatterGatherLists, 0);
    }

    //
    // Log transfer completion information
    //
    if (EtwLoggingEnabled) {

        GUID    activityIdGuid;
        LPGUID  activityId;

        if (Irp && NT_SUCCESS(USBPORT_GetActivityIdIrp(Irp, &activityIdGuid))) {
            activityId = &activityIdGuid;
        } else {
            activityId = NULL;
        }

        switch (urb->Hdr.Function) {

            case URB_FUNCTION_CONTROL_TRANSFER:
            case URB_FUNCTION_CONTROL_TRANSFER_EX:

                if (Transfer->MiniportBytesTransferred > 0 &&
                    Transfer->MiniportBytesTransferred < MAXUSHORT &&
                    Transfer->Direction == ReadData) {

                    PCEVENT_DESCRIPTOR  eventDescriptor;
                    USHORT              bytesTransferred;
                    PVOID               mappedSystemVa;

                    // The system VA would not have been mapped if double buffering
                    // was not required. Lets call MmGetSystemAddressForMdlSafe here
                    // it takes care of the case where SystemAddress is mapped and
                    // also if it was not mapped.
                    //
                    mappedSystemVa = MmGetSystemAddressForMdlSafe(Transfer->TransferBufferMdl,
                                                                  NormalPagePriority  | MdlMappingNoExecute);

                    bytesTransferred = 0;

                    // If the Mdl was correctly mapped then get the bytes to be
                    // transferred
                    //
                    if (mappedSystemVa != NULL) {
                        bytesTransferred = (USHORT)Transfer->MiniportBytesTransferred;
                    }

                    if (Irp) {

                        // Client driver submitted Irp / URB with
                        // ReadData data buffer.
                        //
                        if (urb->Hdr.Function == URB_FUNCTION_CONTROL_TRANSFER) {
                            eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_DATA;
                        } else {
                            eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_EX_DATA;
                        }

                        USBPORT_EtwWriteHcDeviceEndpointData5(
                            HcFdo,
                            devH,
                            endpoint,
                            eventDescriptor,
                            activityId,

                            &Irp,
                            sizeof(PIRP),

                            &urb,
                            sizeof(PURB),

                            urb,
                            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),

                            &bytesTransferred,
                            sizeof(USHORT),

                            mappedSystemVa,
                            bytesTransferred);

                    }  else {

                        // Internally submitted URB with ReadData data
                        // buffer.
                        //
                        eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_INTERNAL_URB_FUNCTION_CONTROL_TRANSFER_DATA;

                        USBPORT_EtwWriteHcDeviceEndpointData4(
                            HcFdo,
                            devH,
                            endpoint,
                            eventDescriptor,
                            NULL,

                            &urb,
                            sizeof(PURB),

                            urb,
                            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),

                            &bytesTransferred,
                            sizeof(USHORT),

                            mappedSystemVa,
                            bytesTransferred);
                    }

                } else {

                    PCEVENT_DESCRIPTOR  eventDescriptor;

                    if (Irp) {

                        // Client driver submitted Irp / URB with no
                        // ReadData data buffer.
                        //
                        if (urb->Hdr.Function == URB_FUNCTION_CONTROL_TRANSFER) {
                            eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER;
                        } else {
                            eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_EX;
                        }

                        USBPORT_EtwWriteHcDeviceEndpointData3(
                            HcFdo,
                            devH,
                            endpoint,
                            eventDescriptor,
                            activityId,

                            &Irp,
                            sizeof(PIRP),

                            &urb,
                            sizeof(PURB),

                            urb,
                            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                    } else {
                        // Internally submitted URB with no ReadData
                        // data buffer.
                        //
                        eventDescriptor = &USBPORT_ETW_EVENT_COMPLETE_INTERNAL_URB_FUNCTION_CONTROL_TRANSFER;

                        USBPORT_EtwWriteHcDeviceEndpointData2(
                            HcFdo,
                            devH,
                            endpoint,
                            eventDescriptor,
                            NULL,

                            &urb,
                            sizeof(PURB),

                            urb,
                            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
                    }
                }
                break;

            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:

                if (Transfer->MiniportBytesTransferred > 0 &&
                      Transfer->Direction == ReadData) {

                    USHORT  bytesTransferred;
                    PVOID   mappedSystemVa;

                    bytesTransferred = 0;

                    // The system VA would not have been mapped if double buffering
                    // was not required. Lets call MmGetSystemAddressForMdlSafe here
                    // it takes care of the case where SystemAddress is mapped and
                    // also if it was not mapped.
                    //
                    mappedSystemVa = MmGetSystemAddressForMdlSafe(Transfer->TransferBufferMdl,
                                                                  NormalPagePriority | MdlMappingNoExecute);

                    // If the Mdl was correctly mapped then calculate the bytes
                    // to be transferred
                    //
                    if (mappedSystemVa != NULL) {
                        // Only log a portion of start of the transfer
                        // buffer.
                        //
                        bytesTransferred = (Transfer->MiniportBytesTransferred < 32) ?
                                           (USHORT)Transfer->MiniportBytesTransferred : 32;
                    }

                    USBPORT_EtwWriteHcDeviceEndpointData5(
                        HcFdo,
                        devH,
                        endpoint,
                        &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_DATA,
                        activityId,

                        &Irp,
                        sizeof(PIRP),

                        &urb,
                        sizeof(PURB),

                        urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),

                        &bytesTransferred,
                        sizeof(USHORT),

                        mappedSystemVa,
                        bytesTransferred);

                } else {

                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        HcFdo,
                        devH,
                        endpoint,
                        &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER,
                        activityId,

                        &Irp,
                        sizeof(PIRP),

                        &urb,
                        sizeof(PURB),

                        urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));
                }
                break;

            case URB_FUNCTION_ISOCH_TRANSFER:
            {
                USHORT  packetCount;

                packetCount = (USHORT)urb->u.Isoch.NumberOfPackets;

                USBPORT_EtwWriteHcDeviceEndpointData5(
                    HcFdo,
                    devH,
                    endpoint,
                    &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &urb,
                    sizeof(PURB),

                    &packetCount,
                    sizeof(USHORT),

                    urb,
                    sizeof(struct _URB_ISOCH_TRANSFER) -
                    sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),

                    &urb->u.Isoch.IsoPacket[0],
                    sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR) * packetCount);

                break;
            }
        }
    }

    if (Transfer->DoubleBufferDueToTegraDmaBug) {
        USBPORT_Core_RevertDoubleBuffer(Transfer, endpoint);
    }







    if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_MDL_ALLOCATED)) {
        // if the flag is not set then see if the mdl came from
        // the controller's internal mdl
        if (!FPFreeMdl(devExt,
                       (PUSBD_DEVICE_HANDLE)Transfer->UrbDeviceHandle,
                       endpoint,
                       Transfer->TransferBufferMdl)) {
            IoFreeMdl(Transfer->TransferBufferMdl);
            ((PTRANSFER_URB)urb)->TransferBufferMDL = NULL;
        }
    }

    // Release scatter gather buffer
    if (Transfer->SGBuffer &&
        !FPFreeSgBuf(devExt, devH,endpoint, Transfer->SGBuffer)) {
        UsbFreeToLookaside(Transfer->SGBuffer);
    }

    // Save a copy of Flags, miniportBytesTransferred, EpRefHandle,
    // urbCallBackFunc, and urbCallbackContxt as we might release
    // Transfer earlier if the emergency Transfer was used to perform
    // this IO request.
    flags = Transfer->Flags;
    miniportBytesTransferred = Transfer->MiniportBytesTransferred;
    epRefHandle = Transfer->EpRefHandle;
    urbCallback = Transfer->UrbCallbackFunc;
    urbCallbackCtx = Transfer->UrbCallbackContext;

    USBPORT_Core_UnlinkTransfer(HcFdo, urb, Transfer);

    if (Irp) {
        PDEVICE_OBJECT pdoDeviceObject;

        // if there is an irp then there is aref on the rh pdo
        pdoDeviceObject = devExt->Fdo.RhPdoPtr;

        usbIoRequest = USBPORT_Core_GetIoRequestFromObject(pdoDeviceObject, Irp);
        ASSERT_USB_IOREQUEST(usbIoRequest);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'irC2',  Irp,  usbIoRequest, 0);

        Irp->IoStatus.Status      = ntStatus;
        Irp->IoStatus.Information = 0;

        USBPORT_ASSERT(ntStatus != STATUS_INSUFFICIENT_RESOURCES ||
                       !TEST_FLAG(urb->TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER));

        LOGENTRY(HcFdo,
                 LOG_TRANSFER,
                 'irpC',
                 Irp,
                 ntStatus,
                 urb);

        // Release the emergency transfer before completing the current IRP
        // so it can be reused immediately by the next IRP
        if (FPFreeTransferContext(devExt, devH, endpoint, Transfer)) {
            Transfer = NULL;
        }

        USBPORT_DereferenceDeviceHandle(HcFdo,
                                        devH,
                                        urb,
                                        DEVH_URB_TAG,
                                        TRUE);

        //LOGENTRY(HcFdo, LOG_TRANSFER, 'irql', 0, 0, KeGetCurrentIrql());
        KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

        INIT_EXEC_METRICS(execMetrics);

        ETW_MEASURE_TIME_START(execMetrics);

        IoCompleteRequest(Irp,
                          IO_NO_INCREMENT);

        ETW_STOP_AND_LOG_TIME(HcFdo, execMetrics, IOCDPC_IO_COMPLETE_REQUEST);

        KeLowerIrql(oldIrql);

        USBPORT_Core_RecordOcaTransferData(HcFdo, NULL);
        // deref the PDO device object since this is what the
        // 'irp' was passed to
        USBPORT_Core_FreeIoRequest(usbIoRequest);

    } else {
        // Release the emergency transfer context used by the Irp-less transfer
        if (FPFreeTransferContext(devExt, devH, endpoint, Transfer)) {
            Transfer = NULL;
        }
    }

    // if we have an event associated with this Transfer signal it

    if (event) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'sgEV', event, 0, 0);

        KeSetEvent(event,
                   1,
                   FALSE);

    }

    // this is an internal async request
    if (TEST_FLAG(flags, USBPORT_TXFLAG_URB_CALLBACK)) {

        urbCallback(HcFdo, urb, urbCallbackCtx, ntStatus);

        USBPORT_DereferenceDeviceHandle(HcFdo, devH, urb, DEVH_SEND_CMDEX_TAG, TRUE);

        UsbFreePool(urb);

    }
    // release the reference on the endpoint
    USBPORT_DereferenceEndpoint(HcFdo, epRefHandle);

    // free the Transfer now that we are done with it
    LOGENTRY(HcFdo, LOG_TRANSFER, 'freT', Transfer, miniportBytesTransferred, 0);

    if (Transfer) {
        UNSIG(Transfer);
        UsbFreeToLookaside(Transfer);
    }
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iIrpCsqCompleteDoneTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

 *
 * This routine completes a single transfer that was on the global done
 * queue, i.e. the devExt->Fdo.TxDoneListHead.
 *
 * The actual completion of the transfer happens if and when this
 * routine calls USBPORT_Core_iCompleteDoneTransfer().
 *
 * In the cases where this routine does not call
 * USBPORT_Core_iCompleteDoneTransfer() it will be called by
 * USBPORT_TxCsqCompleteCanceledIrp()
 *
 * This routine is called only by USBPORT_Core_UsbIocDpc_Worker() after
 * a transfer has been removed from the devExt->Fdo.TxDoneListHead.
 *
 * The Transfer->IrpState has not yet been updated to reflect that fact
 * that the transfer has been removed from the devExt->Fdo.TxDoneListHead.
 * The Transfer->IrpState will be updated here to reflect that.
 *
 * The transfer was previously removed from the
 * devExt->Fdo.TxPriorityListHead (Priority Queue) by
 * USBPORT_Core_UsbDoneDpc_Worker() before it was inserted on the
 * devExt->Fdo.TxDoneListHead (Done Queue).  TxPriorityListHead and
 * TxDoneListHead are mutually exclusive.
 *
 * If the transfer is associated with an Irp it may be on the
 * devExt->TxCsqIrpListHead (Cancel Safe Queue) if the Irp was not
 * canceled, in which case this routine will need to call
 * IoCsqRemoveIrp() and synchronize with the possible cancelation of the
 * Irp before completing the transfer.
 *
 * If the transfer is associated with an Irp it may be on the
 * devExt->TxCancelIrpListHead (Cancel List) if the Irp was
 * canceled, in which case this routine will need to call
 *
 * The global TX Lock is held on entry to this routine.
 *
 * The global TX Lock is released on exit from this routine, either by
 * explicitly releasing the TX Lock, or by calling
 * USBPORT_Core_iCompleteDoneTransfer() which releases the TX Lock.
 *

--*/
{
    PDEVICE_EXTENSION   devExt;
    PTRANSFER_URB       urb;
    PHCD_ENDPOINT       endpoint;
    PIRP                irp;

    devExt = GetHcFdoExt(HcFdo);

    urb = Transfer->Urb;
    ASSERT_TRANSFER_URB(urb);
    USBPORT_ASSERT(urb->pd.HcdTransferContext == Transfer);

    SET_USBD_ERROR(Transfer->Urb, Transfer->UsbdStatus);

    USBPORT_ASSERT(Transfer->TxType != Tx_SPLIT_CHILD);

    endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);
    USBPORT_iAssertEndpoint(HcFdo, endpoint);

    USB_ASSERT_UNLINKED(&Transfer->TxPriorityLink);
    USB_ASSERT_UNLINKED(&Transfer->TxDoneLink);

    if (Transfer->Irp) {

        PIO_CSQ_IRP_CONTEXT irpCsqContext;

        if (Transfer->IrpState == Irp_IoDone_Pending) {

            //
            // (Done Queue, Cancel Safe Queue)
            //

            irpCsqContext = &Transfer->CsqContext;

            LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ1', Transfer, Transfer->Irp, urb);

            //
            // The TX Lock must be released before attempting to remove
            // the Transfer/Irp from the TxCsqIrpListHead as that needs
            // to acquire the TX Lock.
            //
            // USBPORT_TxCsqRemoveIrp() and then
            // USBPORT_TxCsqCompleteCanceledIrp() could run as soon as
            // the TX Lock is released if the Irp is canceled.
            //
            // If USBPORT_TxCsqCompleteCanceledIrp() does run it will
            // not complete the Transfer/Irp since the IrpState still
            // indicates that it is on the Done Queue.  So the
            // CsqContext will remain valid even after the TX Lock is
            // released.
            //
            USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);

            //
            // Attempt to remove the Transfer/Irp from the
            // TxCsqIrpListHead.
            //
            // Note that if the Irp has been canceled calling
            // IoCsqRemoveIrp() here will not result in
            // USBPORT_TxCsqRemoveIrp() being called here, but another
            // call to USBPORT_TxCsqRemoveIrp() and then
            // USBPORT_TxCsqCompleteCanceledIrp() might currently be in
            // progress or might have already completed execution.
            //
            irp = IoCsqRemoveIrp(&devExt->TxCsq, irpCsqContext);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ2', Transfer, irp, urb);

            //
            // Acquire the TX Lock again to exclude simultaneous
            // execution with USBPORT_TxCsqRemoveIrp() or
            // USBPORT_TxCsqCompleteCanceledIrp() before determining how
            // to preceed.
            //
            Irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

            if (irp != NULL) {

                LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ3', Transfer, Transfer->Irp, urb);

                //
                // The call to IoCsqRemoveIrp() returned the Irp which
                // indicates that it was not canceled and at this point
                // it can no longer be canceled.
                //
                // The call to IoCsqRemoveIrp() must have resulted in
                // USBPORT_TxCsqRemoveIrp() being called and running to
                // completion to remove the Transfer/Irp from the
                // TxCsqIrpListHead.
                //
                // USBPORT_TxCsqRemoveIrp() must have inserted the
                // Transfer/Irp on the TxCancelIrpListHead as it does
                // in all cases, but in this case
                // USBPORT_TxCsqCompleteCanceledIrp() will never run to
                // remove the Transfer/Irp from the TxCancelIrpListHead
                // and complete it.
                //

                //
                // This is the normal completion path for a
                // Transfer/Irp which was not canceled by the client
                // driver.
                //

                USBPORT_ASSERT(Transfer->IrpState == Irp_IoDone_Canceled);

                //
                // (Done Queue, Cancel List)
                //

                USBPORT_ASSERT(irp == Transfer->Irp);
                USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);

                //
                // Remove Transfer/Irp from the TxCancelIrpListHead
                // since USBPORT_TxCsqCompleteCanceledIrp() will never
                // run for this Transfer/Irp.
                //
#if DBG
                USBPORT_ASSERT(USBPORT_Core_iFindCanceledTransferIrp(HcFdo, irp) == Transfer);
#endif
                RemoveEntryList(&Transfer->TxCancelIrpLink);
                USB_ZAP_LINK(&Transfer->TxCancelIrpLink);

                Transfer->IrpState = Irp_Complete; // (None)

                //
                // Complete the Transfer/Irp. The TX Lock will be
                // released by USBPORT_Core_iCompleteDoneTransfer().
                //
                USBPORT_Core_iCompleteDoneTransfer(HcFdo, Transfer->Irp, Transfer, Xdpc, Irql);

            } else {

                //
                // The Irp was canceled and the call to IoCsqRemoveIrp()
                // here did not result in USBPORT_TxCsqRemoveIrp() being
                // called from here to remove the Irp from the
                // TxCsqIrpListHead.
                //
                // In this case USBPORT_TxCsqRemoveIrp() and then
                // USBPORT_TxCsqCompleteCanceledIrp() will be called
                // due to the client driver calling IoCancelIrp().
                //
                // USBPORT_TxCsqRemoveIrp() and then
                // USBPORT_TxCsqCompleteCanceledIrp() might have run to
                // completion, or they might still be yet to run.
                //
                if (Transfer->IrpState == Irp_IoDone) {

                    //
                    // (Done Queue)
                    //

                    LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ4', Transfer, Transfer->Irp, urb);

                    //
                    // USBPORT_TxCsqRemoveIrp() and then
                    // USBPORT_TxCsqCompleteCanceledIrp() must have both
                    // run to completion and removed the Irp from the
                    // TxCsqIrpListHead and then the
                    // TxCancelIrpListHead.
                    //
                    // USBPORT_TxCsqCompleteCanceledIrp() would not have
                    // completed the Transfer/Irp so it can be completed
                    // now here.
                    //
                    Transfer->IrpState = Irp_Complete; // (None)

                    USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);
                    USB_ASSERT_UNLINKED(&Transfer->TxCancelIrpLink);

                    //
                    // Complete the Transfer/Irp. The TX Lock will be
                    // released by USBPORT_Core_iCompleteDoneTransfer().
                    //
                    USBPORT_Core_iCompleteDoneTransfer(HcFdo, Transfer->Irp, Transfer, Xdpc, Irql);

                } else if (Transfer->IrpState == Irp_IoDone_Canceled) {

                    //
                    // (Done Queue, Cancel List)
                    //

                    LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ5', Transfer, Transfer->Irp, urb);

                    //
                    // USBPORT_TxCsqRemoveIrp() must have run to
                    // completion and removed the Transfer/Irp from the
                    // TxCsqIrpListHead and then inserted it on the
                    // TxCancelIrpListHead.
                    //
                    // USBPORT_TxCsqCompleteCanceledIrp() has not yet
                    // run.  When it does it will remove the
                    // Transfer/Irp from the TxCancelIrpListHead and
                    // complete it so do not complete it here.
                    //
                    Transfer->IrpState = Irp_Canceled; // (Cancel List)

                    USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);

                    USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);

                } else if (Transfer->IrpState == Irp_IoDone_Pending) {

                    //
                    // (Done Queue, Cancel Safe Queue)
                    //

                    LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ6', Transfer, Transfer->Irp, urb);

                    //
                    // USBPORT_TxCsqRemoveIrp() and then
                    // USBPORT_TxCsqCompleteCanceledIrp() must still
                    // both be yet to run.
                    //
                    // When USBPORT_TxCsqCompleteCanceledIrp() runs it
                    // will complete the Transfer/Irp so it is not
                    // completed now here.
                    //
                    Transfer->IrpState = Irp_Pending; // (Cancel Safe Queue)

                    USB_ASSERT_UNLINKED(&Transfer->TxCancelIrpLink);

                    USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);

                } else {

                    LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ7', Transfer, Transfer->Irp, Transfer->IrpState);

                    USBPORT_ASSERTMSG("Unexpected Transfer->IrpState", FALSE);

                    USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);
                }
            }

        } else if (Transfer->IrpState == Irp_IoDone_Canceled) {

            //
            // (Done Queue, Cancel List)
            //

            LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ8', Transfer, Transfer->Irp, urb);

            //
            // The Transfer/Irp is no longer on the Cancel Safe Queue so
            // there is no need to call IoCsqRemoveIrp() here.
            //
            // USBPORT_TxCsqRemoveIrp() must have run to completion and
            // removed the Transfer/Irp from the TxCsqIrpListHead and
            // then inserted it on the TxCancelIrpListHead.
            //
            // USBPORT_TxCsqCompleteCanceledIrp() has not yet run.  When
            // it does it will remove the Transfer/Irp from the
            // TxCancelIrpListHead and complete it so do not complete it
            // here.
            //
            Transfer->IrpState = Irp_Canceled; // (Cancel List)

            USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);

            USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);

        } else if (Transfer->IrpState == Irp_IoDone) {

            //
            // (Done Queue)
            //

            LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQ9', Transfer, Transfer->Irp, urb);

            //
            // The Transfer/Irp is no longer on the Cancel Safe Queue so
            // there is no need to call IoCsqRemoveIrp() here.
            //
            // USBPORT_TxCsqRemoveIrp() and then
            // USBPORT_TxCsqCompleteCanceledIrp() must have both run to
            // completion and removed the Irp from the TxCsqIrpListHead
            // and then the TxCancelIrpListHead.
            //
            // USBPORT_TxCsqCompleteCanceledIrp() would not have
            // completed the Transfer/Irp so it can be completed now
            // here.
            //
            Transfer->IrpState = Irp_Complete; // (None)

            USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);
            USB_ASSERT_UNLINKED(&Transfer->TxCancelIrpLink);

            //
            // Complete the Transfer/Irp. The TX Lock will be released
            // by USBPORT_Core_iCompleteDoneTransfer().
            //
            USBPORT_Core_iCompleteDoneTransfer(HcFdo, Transfer->Irp, Transfer, Xdpc, Irql);

        } else {

            LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQX', Transfer, Transfer->Irp, Transfer->IrpState);

            USBPORT_ASSERTMSG("Unexpected Transfer->IrpState", FALSE);

            USBPORT_TxReleaseLock(HcFdo, Xdpc, Irql);
        }

    } else {

        //
        // No Irp Transfer.  The Transfer never could have been queued
        // on the TxCsqIrpListHead and there is no need to synchronize
        // with USBPORT_TxCsqRemoveIrp().  Just complete the Transfer
        // here.
        //
        LOGENTRY(HcFdo, LOG_TRANSFER, 'CPQZ', Transfer, urb, Transfer->IrpState);

        USBPORT_ASSERT(Transfer->IrpState == Irp_IoDone); // (Done Queue)

        Transfer->IrpState = Irp_Complete; // (None)

        USB_ASSERT_UNLINKED(&Transfer->TxCsqIrpLink);
        USB_ASSERT_UNLINKED(&Transfer->TxCancelIrpLink);

        //
        // Complete the Transfer. The TX Lock will be released by
        // USBPORT_Core_iCompleteDoneTransfer().
        //
        USBPORT_Core_iCompleteDoneTransfer(HcFdo, NULL, Transfer, Xdpc, Irql);
    }
}


typedef enum _USB_MP_STATUS_TYPE {

    mp_success = 0,
    mp_busy,
    mp_error,

} USB_MP_STATUS_TYPE;

VOID
USBPORT_Core_ProcessScatterGatherList(
    __in PDEVICE_OBJECT         HcFdo,
    __in PIRP                   Irp,
    __in PSCATTER_GATHER_LIST   ScatterGatherList,
    __in PVOID                  Context
    )
/*++

Routine Description:

    This is a callback for the GetScatterGatherList() calls that are made in USBPORT_Core_UsbMapDpc_Worker().

    It turns the lists of system defined SCATTER_GATHER_ELEMENT entries into a list of USBPORT defined
    TRANSFER_SG_ENTRY entries.

    All error paths drop the refernce to the scatter gather callback.  This is because we cannot return an error --
    we make an assumption that if an error is returned our routines are never called.

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION       devExt;
    PSCATTER_GATHER_ENTRY   scatterGatherEntry;     // USBPORT defined struct
    PSCATTER_GATHER_ELEMENT scatterGatherElement;   // System  defined struct
    PHCD_TRANSFER_CONTEXT   transfer;
    PHCD_ENDPOINT           endpoint;
    PTRANSFER_SG_LIST       sgList;                 // USBPORT defined struct
    PTRANSFER_SG_ENTRY      sgEntry;                // USBPORT defined struct
    PUCHAR                  currentVa;
    PHYSICAL_ADDRESS        logicalAddress;
    PHYSICAL_ADDRESS        logicalSave;
    ULONG                   startOffset;
    ULONG                   lengthRemaining;
    ULONG                   lengthThisPage;
    ULONG                   i, j;
    LIST_ENTRY              splitTxListHead;
    LIST_ENTRY              errorTxListHead;
    NTSTATUS                split_nts;
    PLIST_ENTRY             txInsertList;
    KIRQL                   irql;
    USB_MP_STATUS_TYPE      mp_status;
    BOOLEAN                 submCompleteOnError;
    BOOLEAN                 doubleBufferSupported;
    ETW_EXECUTION_METRICS   execMetrics;

    ETW_MEASURE_TIME_START(execMetrics);

    devExt = GetHcFdoExt(HcFdo);

    if (devExt->Fdo.HcOptionFlags & USB_MINIPORT_OPT_NO_DOUBLE_BUFFER) {
        doubleBufferSupported = FALSE;
    } else {
        doubleBufferSupported = TRUE;
    }

    // aquire lock in the context of the callback.
    //** Transfer may have been canceled, we must check for this
    irql = USBPORT_TxAcquireLock(HcFdo, NULL);

    // The context for this callback is a pointer to a
    // SCATTER_GATHER_ENTRY that is in the list of entries hanging off
    // of a HCD_TRANSFER_CONTEXT.  Each SCATTER_GATHER_ENTRY contains
    // a back pointer to the HCD_TRANSFER_CONTEXT, plus a pointer where
    // the current SCATTER_GATHER_LIST pointer can be stored.
    //
    scatterGatherEntry = (PSCATTER_GATHER_ENTRY)Context;

    scatterGatherEntry->ScatterGatherList = ScatterGatherList;

    transfer = scatterGatherEntry->Transfer;
    ASSERT_TRANSFER(transfer);

    switch(transfer->TxState) {
    case TX_MapPending_Canceled:
    case TX_MapPending:

        break;
    default:
        USBPORT_ASSERTMSG( "Unexpected transfer->IrpState", FALSE);
    }

    // If this transfer requires multiple calls to
    // GetScatterGatherList() then do not do anything else yet unless
    // all of the GetScatterGatherList() callbacks have been called.
    //
    if (InterlockedDecrement(&transfer->NumScatterGatherListsRemaining) > 0) {
        USBPORT_TxReleaseLock(HcFdo, NULL, irql);
        ETW_STOP_AND_LOG_TIME(HcFdo, execMetrics, PROCESS_SG_LIST_CALLBACK);
        return;
    }

    endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
    USBPORT_iAssertEndpoint(HcFdo, endpoint);



    LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA0', 0, 0, irql);

    currentVa = MmGetMdlVirtualAddress(transfer->TransferBufferMdl);

    USBPORT_ASSERT(!(transfer->Flags & USBPORT_TXFLAG_MAPPED));

    sgList = &transfer->SgList;
    sgList->SgFlags = 0;
    sgList->SgCount = 0;
    sgList->MdlVirtualAddress = currentVa;

    sgEntry = &sgList->SgEntry[0];

    // Attempt to map a system address for the MDL in case the miniport needs to double buffer.

    if (doubleBufferSupported) {
        sgList->MdlSystemAddress = MmGetSystemAddressForMdlSafe(transfer->TransferBufferMdl,
                                                                NormalPagePriority | MdlMappingNoExecute);
    }

    do {
        PHCD_TRANSFER_CONTEXT   splitTransfer;

        if (sgList->MdlSystemAddress == NULL &&
            doubleBufferSupported) {

            USBPORT_ASSERT(!TEST_FLAG(transfer->Urb->TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER));

            // low memory condition

            TEST_TRAP();
            LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA1', 0, 0, transfer);

            USBPORT_Core_iCompleteTransfer(HcFdo,
                                          NULL,
                                          transfer,
                                          USBD_STATUS_INSUFFICIENT_RESOURCES,
                                          Ev_TX_IoMapError,
                                          TX_Error);

            // transfer pointer may become invalid after the lock is released
            transfer = NULL;

            // end while
            break;
        }

        LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA2', sgList, transfer, transfer->Tp.TransferBufferLength);

        logicalSave.QuadPart = 0;

        startOffset = 0;

        // The outer loop is over the list of SCATTER_GATHER_LIST entries,
        // each of which is the result of a GetScatterGatherList() call.
        //
        for (i = 0, scatterGatherEntry = &transfer->ScatterGatherEntries[0];
             i < transfer->NumScatterGatherLists;
             i++, scatterGatherEntry++) {
            // The inner loop is over the array of SCATTER_GATHER_ELEMENT
            // entries in a single SCATTER_GATHER_LIST entry.
            //

            for (j = 0, scatterGatherElement = &scatterGatherEntry->ScatterGatherList->Elements[0];
                 j < scatterGatherEntry->ScatterGatherList->NumberOfElements;
                 j++, scatterGatherElement++) {
                lengthRemaining = scatterGatherElement->Length;

                logicalAddress  = scatterGatherElement->Address;

                // Check for special case where the MDL entries all map to
                // the same physical page.
                //
                if (logicalSave.QuadPart == logicalAddress.QuadPart) {
                    SET_FLAG(sgList->SgFlags, USBMP_SGFLAG_SINGLE_PHYSICAL_PAGE);

                    LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA3', 0, logicalAddress.LowPart, logicalSave.LowPart);

                }
                logicalSave.QuadPart = logicalAddress.QuadPart;

                // Innermost loop consumes the entire length of the current
                // SCATTER_GATHER_ELEMENT, one page at a time.
                //
                while (lengthRemaining) {
                    // Consume up to the next page (USB 4K page, not CPU
                    // page) boundary, but no more than the length that is
                    // actually remaining.
                    //
                    lengthThisPage = USB_PAGE_SIZE -
                                     (logicalAddress.LowPart & (USB_PAGE_SIZE - 1));

                    if (lengthThisPage > lengthRemaining) {
                        lengthThisPage = lengthRemaining;
                    }

                    // Populate the current TRANSFER_SG_ENTRY32 and advance
                    // to the next one.
                    //

                    sgEntry->LogicalAddress.Hw64 = logicalAddress;

                    sgEntry->Length = lengthThisPage;

                    sgEntry->StartOffset = startOffset;

                    sgEntry++;

                    sgList->SgCount++;

                    // Update the outer and inner loop variables
                    //
                    startOffset += lengthThisPage;

                    lengthRemaining -= lengthThisPage;

                    logicalAddress.LowPart += lengthThisPage;

                }
            }
        }

        USBPORT_ASSERT(startOffset == transfer->Tp.TransferBufferLength);








        if (endpoint->Parameters.DeviceSpeed == HighSpeed) {
            SET_FLAG(transfer->Flags, USBPORT_TXFLAG_HIGHSPEED);
        }

        // mapping complete
        SET_FLAG(transfer->Flags, USBPORT_TXFLAG_MAPPED);

        if (transfer->TxState == TX_MapPending_Canceled) {

            USBPORT_Core_iCompleteTransfer(HcFdo,
                                           NULL,
                                           transfer,
                                           USBD_STATUS_CANCELED,
                                           Ev_TX_MapTransferCanceled,
                                           TX_Mapped_Canceled);

            // transfer pointer may become invalid after the lock is released
            // note: ref is dropped when the transfer is completed
            transfer = NULL;

            // end while
            break;
        }

        //
        // If this is an iso transfer we need to set up the iso data structures as well.
        //
        if (TEST_FLAG(transfer->Flags, USBPORT_TXFLAG_ISO)) {
            // Enable this code path after USBPORT_InitializeIsoTransfer()
            // has been updated to return a USBD_STATUS result.
            //
            USBD_STATUS usbdStatus;

            usbdStatus = USBPORT_InitializeIsoTransfer(HcFdo,
                                                       transfer->Urb,
                                                       transfer);

            if (usbdStatus != USBD_STATUS_SUCCESS) {

                LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA4', 0, 0, transfer);

                USBPORT_Core_iCompleteTransfer(HcFdo,
                                               NULL,
                                               transfer,
                                               usbdStatus,
                                               Ev_TX_Iomap_Isoch_Error,
                                               TX_Mapped_Error);

                // transfer pointer may become invalid after the lock is released
                // note: ref is dropped when the transfer is completed
                transfer = NULL;
                // end while
                break;
            }
        }

        //
        // Transfer is mapped, perform the split operation if necessary, split transfers are mapped as a group
        //
        // this opertaion will generate the split child transfers, these don't count against MaxReq
        //


        split_nts = USBPORT_Split_Transfer(HcFdo,
                                           NULL,
                                           endpoint,
                                           transfer,
                                           &splitTxListHead);

        if (NT_ERROR(split_nts)) {
            USBD_STATUS usbdStatus;

            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA5', 0, split_nts, transfer);

            USBPORT_Core_iCompleteTransfer(HcFdo,
                                          NULL,
                                          transfer,
                                          usbdStatus,
                                          Ev_TX_IomapSplitError,
                                          TX_Mapped_Error);

            // transfer pointer may become invalid after the lock is released
            // note: ref is dropped when the transfer is completed

            transfer = NULL;
            // end while
            break;
        }

        LOGENTRY(HcFdo, LOG_TRANSFER, 'DMA6', 0, 0, transfer);
        // Transfer is now mapped and split, calldown to the miniport.
        //
        // ** if the transfer was not split the split list will contain only the original transfer
        // ** if the transfer was split the the list contains only split children
        splitTransfer = NULL;
        // use parent next link as head pointer
        txInsertList = transfer->TxPriorityLink.Flink;

        // check the state of the endpoint, don't call down unless we are 'active'
        mp_status = mp_success;

        InitializeListHead(&errorTxListHead);

        if (transfer->TxType == Tx_SPLIT_PARENT) {
            // bypass completion if this is a split transfer -- split transfers must be completed after the child
            // transfers are added to the priority queue.
            submCompleteOnError = FALSE;
        } else {
            // original behavior for USBPORT_Core_iSubmitTransferToMiniport is default.
            submCompleteOnError = TRUE;
        }

        while (!IsListEmpty(&splitTxListHead)) {

            PLIST_ENTRY listEntry;
            ULONG count = 0;
            USB_MINIPORT_STATUS mps;


            listEntry = RemoveHeadList(&splitTxListHead);

            splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                            listEntry,
                            struct _HCD_TRANSFER_CONTEXT,
                            TmpLink);

            count++;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'DMs2', splitTransfer, endpoint, count);

            // calldown all transfers to miniport on first map attempt
            switch(mp_status) {
            case mp_busy:
                // indicate transfer is mapped, we skip the rest of the children as soon as one comes back busy
                USBPORT_TxSetState(HcFdo, NULL, splitTransfer, Ev_TX_MapMpSubmitBusy, TX_Mapped);

                // The transfer is mapped but not yet in the miniport.
                // Add the transfer to the TxMappedNotInMiniportListHead.
                //
                USB_ASSERT_UNLINKED(&splitTransfer->TxMappedNotInMiniportListEntry);

                InsertTailList(&endpoint->TxMappedNotInMiniportListHead,
                               &splitTransfer->TxMappedNotInMiniportListEntry);

                break;

            case mp_error:
                // mark as mapped with error
                USBPORT_TxSetState(HcFdo, NULL, splitTransfer, Ev_TX_MapMpSubmitError, TX_Mapped_Error);
                break;

            case mp_success:
            default:

                // post until miniport reports busy or eror
                LOGENTRY(HcFdo, LOG_TRANSFER, 'DMs3', splitTransfer, endpoint, count);

                // Only submit this transfer to the miniport if there
                // are no other already mapped transfers on the
                // TxMappedNotInMiniportListHead list to prevent out of
                // order transfer submission to the miniport.
                //
                if (IsListEmpty(&endpoint->TxMappedNotInMiniportListHead)) {

                    // Submit the mapped transfer to the miniport.
                    //
                    mps = USBPORT_Core_iSubmitTransferToMiniport(
                        HcFdo,
                        NULL,
                        splitTransfer,
                        submCompleteOnError);

                } else {

                    mps = USBMP_STATUS_BUSY;

                    // Indicate that the transfer is mapped but not in
                    // the miniport.
                    //
                    USBPORT_TxSetState(HcFdo, NULL, splitTransfer, Ev_TX_MapMpSubmitBusy, TX_Mapped);

                    // The transfer is mapped but not yet in the miniport.
                    // Add the transfer to the TxMappedNotInMiniportListHead.
                    //
                    USB_ASSERT_UNLINKED(&splitTransfer->TxMappedNotInMiniportListEntry);

                    InsertTailList(&endpoint->TxMappedNotInMiniportListHead,
                                   &splitTransfer->TxMappedNotInMiniportListEntry);
                }

                switch(mps) {
                case USBMP_STATUS_BUSY:
                    mp_status = mp_busy;
                    break;
                case USBMP_STATUS_SUCCESS:
                    mp_status = mp_success;
                    break;
                case USBMP_STATUS_FAILURE:
                default:
                    mp_status = mp_error;
                    break;
                }
            }

            if (splitTransfer != transfer) {
                USBPORT_ASSERT(submCompleteOnError == FALSE);
                // The split transfers must be inserted in the global priority list after their parent
                // They must have the same Tp.SequenceNumber as the parent. The sequence Number is generated at
                // insertion time.
                //
                // the completion functions expect the transfer to be on the priorty list if we have errors we will
                // have to complete the transfer later
                LOGENTRY(HcFdo, LOG_TRANSFER, 'DMs4', splitTransfer, transfer, count);
                USBPORT_ASSERT(splitTransfer->ParentTransfer == transfer);
                USBPORT_ASSERT(splitTransfer->ParentTransfer->Tp.SequenceNumber == transfer->Tp.SequenceNumber);
                USBPORT_Core_iInsertPriorityTransferDirect(HcFdo, &splitTransfer, txInsertList);

                if (splitTransfer && mp_status == mp_error) {
                    InsertTailList(&errorTxListHead, &splitTransfer->TmpLink);
                }
                //TEST_TRAP();
            }

        }

        // now complete the transfers with errors

        while (!IsListEmpty(&errorTxListHead)) {

            PLIST_ENTRY listEntry;
            USBD_STATUS usbdStatus;

            listEntry = RemoveHeadList(&errorTxListHead);

            splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                            listEntry,
                            struct _HCD_TRANSFER_CONTEXT,
                            TmpLink);


            usbdStatus = USBPORT_MiniportStatus_TO_USBDStatus(USBMP_STATUS_FAILURE);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'DMs5', 0, usbdStatus, splitTransfer);
            // return error to caller -- in this case caller we will have to complete the transfers here
            USBPORT_Core_iCompleteTransfer(HcFdo,
                                          NULL,
                                          splitTransfer,
                                          usbdStatus,
                                          Ev_TX_MapMpSubmitErrorComplete,
                                          TX_Mapped_Error);


        }

    }     WHILE (0);

    // Allow more dma operations now.
    //
    // this occurs here beacuse this routine and the map Dpc calls transfers down to the miniport these must be
    // serialized to  prevent transfers from being submitted out-of-order.
    USBPORT_ASSERT(devExt->Fdo.TxMapBusy);

    // signal the map dpc to run again
    if (USBPORT_iNeedMapDpc(HcFdo)) {
        USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, NULL);
    }

    // drop the callback ref
    USBPORT_Core_DecIoRequestCount(HcFdo, &devExt->Fdo.UsbIoDmaCallback, TRUE);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'DMAx', 0, 0, 0);
    devExt->Fdo.TxMapBusy = 0;

    USBPORT_TxReleaseLock(HcFdo, NULL, irql);
    ETW_STOP_AND_LOG_TIME(HcFdo, execMetrics, PROCESS_SG_LIST_CALLBACK);
}

PUSBPORT_DB_HANDLE
USBPORT_FindTegraIsoDoubleBuffer(
    __in PHCD_TRANSFER_CONTEXT Transfer,
    __in PUCHAR                CurrentVa,
    __in ULONG                 Length,
    //
    // Index where the search should begin.
    //
    __in PLIST_ENTRY          *Index

    )
{
    PUSBPORT_DB_HANDLE dbHandle;

    if (*Index == NULL) {
        *Index = &Transfer->IsoTegraDoubleBufferListHead;
    }

    for(dbHandle = CONTAINING_RECORD((*Index)->Flink, USBPORT_DB_HANDLE, DbLink);
        &Transfer->IsoTegraDoubleBufferListHead != &dbHandle->DbLink;
        dbHandle = CONTAINING_RECORD(dbHandle->DbLink.Flink, USBPORT_DB_HANDLE, DbLink)
        )
    {
        if (dbHandle->DbSystemAddress == CurrentVa) {
            NT_ASSERT(dbHandle->DbLength >= Length);
            *Index = &dbHandle->DbLink;
            return dbHandle;
        }
    }

    return NULL;

}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iMapTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbMapDpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PHCD_ENDPOINT Endpoint,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    Attempt to map a DMA transfer for submission to miniport thru the kernel.

--*/
{
    PDEVICE_EXTENSION           devExt;
    PTRANSFER_URB               urb;
    PUCHAR                      currentVa;
    ULONG                       lengthRemaining;
    ULONGLONG                   offset = 0;
    ULONG                       transferBufferLength;
    PBUILD_SCATTER_GATHER_LIST  buildSGList;
    PBUILD_SCATTER_GATHER_LIST_EX  buildSGListEx = NULL;
    PINITIALIZE_DMA_TRANSFER_CONTEXT initDmaTransferContext = NULL;
    PSCATTER_GATHER_ENTRY       scatterGatherEntry;
    NTSTATUS                    nts;
    PDMA_ADAPTER                adapterObject;
    PCALCULATE_SCATTER_GATHER_LIST_SIZE  calcSGListSize;
    PUCHAR                      sgBuf;
    ULONG                       sgLen;
    BOOLEAN                     doubleBufferSupported;
    ETW_EXECUTION_METRICS       execMetrics;
    BOOLEAN                     writeData = FALSE;
    PUCHAR                      transferVa;
    PMDL                        transferMdl;
    PUSBPORT_DB_HANDLE          dbHandle;
    ULONG                       nextPacket;
    ULONG                       lastPacket=0;
    ULONG                       numPacketsPerTd=0;
    ULONG                       nextPacketOffset;
    ULONG                       packetLength;
    PLIST_ENTRY                 index = NULL;

    INIT_EXEC_METRICS(execMetrics);

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'map0',Endpoint ,0 , Transfer);
    ASSERT_TRANSFER(Transfer);
    USBPORT_iAssertEndpoint(HcFdo, Endpoint);

    if (devExt->Fdo.HcOptionFlags & USB_MINIPORT_OPT_NO_DOUBLE_BUFFER) {
        doubleBufferSupported = FALSE;
    } else {
        doubleBufferSupported = TRUE;
    }

    // since Iomaptransfer must be serialized we can use a static ref
    USBPORT_Core_IncIoRequestCount(HcFdo, &devExt->Fdo.UsbIoDmaCallback, Transfer, UsbIoTag_IoMap, FALSE);

    if (Transfer->Tp.TransferBufferLength == 0 ||
        !Endpoint->EpFlags.MapTransfers) {

        PTRANSFER_SG_LIST       sgList;

        // transfers that don't require mapping are still considered 'mapped'. They are distingushed by the
        // USBPORT_TXFLAG_MAPPED flag on the Transfer context.  These still count as mapped transfers whne caclulating
        // pending Transfer limits.  We will need to fill in our SGList so the miniport can use the buffers if needed
        //

        LOGENTRY(HcFdo, LOG_TRANSFER, 'mapZ', Endpoint ,0 , Transfer);

        sgList = &Transfer->SgList;

        if (Transfer->TransferBufferMdl &&
           Transfer->Tp.TransferBufferLength != 0){
            currentVa = MmGetMdlVirtualAddress(Transfer->TransferBufferMdl);

            if (doubleBufferSupported) {
                sgList->MdlSystemAddress = MmGetSystemAddressForMdlSafe(Transfer->TransferBufferMdl,
                                                                        NormalPagePriority | MdlMappingNoExecute);
            }
            sgList->MdlVirtualAddress = currentVa;
        }
        else{
            sgList->MdlSystemAddress = NULL;
            sgList->MdlVirtualAddress = NULL;
        }

        sgList->SgFlags = 0;
        sgList->SgCount = 0;

        USBPORT_TxSetState(HcFdo, UsbMapDpc, Transfer, Ev_TX_Iomap_ZeroByte, TX_Mapped);

        // Only submit this transfer to the miniport if there
        // are no other already mapped transfers on the
        // TxMappedNotInMiniportListHead list to prevent out of
        // order transfer submission to the miniport.
        //
        if (IsListEmpty(&Endpoint->TxMappedNotInMiniportListHead)) {

            // Submit the mapped transfer to the miniport.
            //
            USBPORT_Core_iSubmitTransferToMiniport(HcFdo, UsbMapDpc, Transfer, TRUE);

        } else {

            // Indicate that the transfer is mapped but not in
            // the miniport.
            //
            USBPORT_TxSetState(HcFdo, UsbMapDpc, Transfer, Ev_TX_MapMpSubmitBusy, TX_Mapped);

            // The transfer is mapped but not yet in the miniport.
            // Add the transfer to the TxMappedNotInMiniportListHead.
            //
            USB_ASSERT_UNLINKED(&Transfer->TxMappedNotInMiniportListEntry);

            InsertTailList(&Endpoint->TxMappedNotInMiniportListHead,
                           &Transfer->TxMappedNotInMiniportListEntry);
        }


        devExt->Fdo.TxMapBusy = 0;
        USBPORT_Core_DecIoRequestCount(HcFdo, &devExt->Fdo.UsbIoDmaCallback, TRUE);

        // schedule USBMAP dpc to run again
        if (USBPORT_iNeedMapDpc(HcFdo)) {
            USBPORT_Xdpc_iSignal(HcFdo, UsbMapDpc, NULL);
        }

        USBPORT_TxReleaseLock(HcFdo, UsbMapDpc, Irql);

        USBPORT_Xdpc_End(HcFdo, UsbMapDpc);
        return;
    }

    //** The MapDpc will not run again until USBPORT_Xdpc_End is called
    //**

    // Transfer list may be updated after lock is released
    USBPORT_TxReleaseLock(HcFdo, UsbMapDpc, Irql);

    urb = Transfer->Urb;
    ASSERT_TRANSFER_URB(urb);

    // We have a Transfer, try to map it...

    // We should not be mapping zero length xfers, assert this
    USBPORT_ASSERT(Transfer->Tp.TransferBufferLength != 0);

    // we should know the direction
    USBPORT_ASSERT(Transfer->Direction != NotSet);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'map1', Transfer, 0, urb);

    scatterGatherEntry = &Transfer->ScatterGatherEntries[0];

    lengthRemaining = Transfer->Tp.TransferBufferLength;
    LOGENTRY(HcFdo, LOG_TRANSFER, 'map2', lengthRemaining, scatterGatherEntry, 0);

    USBPORT_ASSERT(lengthRemaining);

    currentVa = MmGetMdlVirtualAddress(Transfer->TransferBufferMdl);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'map3', lengthRemaining, scatterGatherEntry, currentVa);

    adapterObject = devExt->Fdo.AdapterObject;

    // BuildScatterGatherList() must be called at DISPATCH_LEVEL
    //
    calcSGListSize = adapterObject->DmaOperations->CalculateScatterGatherList;
    buildSGList = adapterObject->DmaOperations->BuildScatterGatherList;

    nts = STATUS_SUCCESS;

    sgBuf = (PUCHAR)Transfer->SGBuffer;






























    while (lengthRemaining) {

        // Each call to GetScatterGatherList() can span a region of the
        // total Transfer buffer that is no larger than can be mapped by
        // the number of map registers in the DMA adapter object.
        //
        // Consume a region of the Transfer buffer up to N page
        // boundaries forward, where N is the number of map registers in
        // the DMA adapter object.
        //
        transferBufferLength = (devExt->Fdo.NumberOfMapRegisters << PAGE_SHIFT) - BYTE_OFFSET(currentVa);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'map4', lengthRemaining, transferBufferLength, scatterGatherEntry);

        // But no further than the actual remaining length of the
        // Transfer buffer.
        //
        if (transferBufferLength > lengthRemaining) {
            transferBufferLength = lengthRemaining;
        }

        transferVa = currentVa;
        transferMdl = Transfer->TransferBufferMdl;



















































        // The current SCATTER_GATHER_ENTRY entry will be the context
        // for the USBPORT_ProcessScatterGatherList() callback.  Store
        // a back pointer to the Transfer in the SCATTER_GATHER_ENTRY
        // entry so that the USBPORT_ProcessScatterGatherList()
        // callback can find the Transfer.
        //
        scatterGatherEntry->Transfer = Transfer;
        scatterGatherEntry->Sig = SIG_HAL_SG_LIST;
        scatterGatherEntry->ScatterGatherList = NULL;

        nts = (*calcSGListSize)(adapterObject,
                                NULL,
                                transferVa,
                                transferBufferLength,
                                &sgLen,
                                NULL);

        if (NT_SUCCESS(nts)) {

            // Now make the actual GetScatterGatherList() call with the
            // current region of the Transfer buffer.
            //

            ETW_MEASURE_TIME_START(execMetrics);

            writeData = Transfer->Direction == WriteData ? TRUE : FALSE;



































            ETW_MEASURE_TIME_STOP(execMetrics);

            if (EtwPerfLoggingEnabled)
            {
                //
                // Log performance metrics for BUILD SG list
                //

                PUSBD_DEVICE_HANDLE devH = NULL;
                PHCD_ENDPOINT endpoint = NULL;

                devH = (PUSBD_DEVICE_HANDLE) Transfer->UrbDeviceHandle;
                endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);

                (VOID) USBPORT_EtwWriteDmaExecutionTime(
                        HcFdo,
                        devH,
                        endpoint,
                        &execMetrics,
                        &USBPORT_ETW_EVENT_BUILD_SGLIST_EXECUTION_TIME,
                        NULL,
                        &adapterObject,
                        sizeof(PDMA_ADAPTER *),
                        &transferBufferLength,
                        sizeof(ULONG),
                        &writeData,
                        sizeof(BOOLEAN),
                        &lengthRemaining,
                        sizeof(ULONG),
                        &transferVa,
                        sizeof(PUCHAR *),
                        ETW_BUILD_SG_LIST_TAG
                        );
            }
        }

        USBPORT_ASSERT(nts != STATUS_INSUFFICIENT_RESOURCES &&
                       nts != STATUS_BUFFER_TOO_SMALL);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'map5', nts, 0, 0);

        if (!NT_SUCCESS(nts)) {
            // complete the Transfer with an error, not sure what else is needed here to unwind though.

            LOGENTRY(HcFdo, LOG_TRANSFER, 'map6', scatterGatherEntry, 0, 0);

            USBPORT_ASSERT(scatterGatherEntry->Sig == SIG_HAL_SG_LIST);

            scatterGatherEntry->Sig = SIG_FREE;
            scatterGatherEntry->Transfer = NULL;
            TEST_TRAP();

            Irql = USBPORT_TxAcquireLock(HcFdo, UsbMapDpc);

            // verify that USBPORT_Core_ProcessScatterGatherList was never called
            switch(Transfer->TxState) {
            case TX_MapPending_Canceled:
            case TX_MapPending:

                break;
            default:
                USBPORT_ASSERTMSG("Unexpected transfer->IrpState",FALSE);
            }

            devExt->Fdo.TxMapBusy = 0;
            USBPORT_Core_DecIoRequestCount(HcFdo, &devExt->Fdo.UsbIoDmaCallback, TRUE);

            // ** assume the Transfer is not mappped on this type of error
            USBPORT_Core_iCompleteTransfer(HcFdo,
                                          NULL,
                                          Transfer,
                                          USBD_STATUS_INSUFFICIENT_RESOURCES,
                                          Ev_TX_IoMapError,
                                          TX_Error);

            USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbDoneDpc, Transfer);

            // signal map Dpc to run again
            if (USBPORT_iNeedMapDpc(HcFdo)) {
                USBPORT_Xdpc_iSignal(HcFdo, UsbMapDpc, NULL);
            }

            USBPORT_TxReleaseLock(HcFdo, UsbMapDpc, Irql);

            // allow another map attempt
            USBPORT_Xdpc_End(HcFdo, UsbMapDpc);
            return;
        }

        // Advance to the next SCATTER_GATHER_ENTRY entry and the next
        // region of the total Transfer buffer.
        //
        scatterGatherEntry++;

        currentVa += transferBufferLength;

        lengthRemaining -= transferBufferLength;

        sgBuf += sgLen;
    }

    // Transfer structure and URB may be gone at this point
    LOGENTRY(HcFdo, LOG_TRANSFER, 'mapF', 0, 0, nts);

    USBPORT_Xdpc_End(HcFdo, UsbMapDpc);
}


USB_MINIPORT_STATUS
USBPORT_Core_iSubmitTransferToMiniport(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in BOOLEAN CompleteOnError
    )
/*++

Routine Description:

    TRUE if the miniport indicated busy status.

--*/
{
    USB_MINIPORT_STATUS mps;
    PHCD_ENDPOINT endpoint;
    MP_ENDPOINT_STATUS epStatus;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'sub0', Xdpc, 0, Transfer);
    if (Xdpc) {
        USBPORT_ASSERT(Xdpc->TxLockState);
    }

    endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);

    // not clear what this check might break, bypass for now
    // epStatus = MPf_GetEndpointStatus(devExt, endpoint);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'sub1', Xdpc, endpoint->GepState, Transfer);
    epStatus = ENDPOINT_STATUS_RUN;
    switch(endpoint->GepState) {
    case GEp_Active:

        if (epStatus != ENDPOINT_STATUS_HALT) {
        // attempt to submit Transfer to the miniport
        //mps = MPx_SubmitTransfer(devExt, endpoint, Transfer);
            USBPORT_iSetGlobalEndpointStateTx(HcFdo, endpoint, Ev_gEp_SubmitTransfer, Transfer, &mps, NULL);
        } else {
            mps = USBMP_STATUS_BUSY;
        }
        break;

    case GEp_RemovePending:
    case GEp_Removed:
    case GEp_Zapped:
    case GEp_Closed:
        // endpoint is closed
        mps = USBMP_STATUS_FAILURE;
        break;

    default:
        // return busy so that transfers remianed queued while the endpoint is paused
        LOGENTRY(HcFdo, LOG_TRANSFER, 'sub2', Xdpc, 0, Transfer);
        mps = USBMP_STATUS_BUSY;
        break;
    }

    switch (mps) {
    case USBMP_STATUS_SUCCESS:
        USBPORT_TxSetState(HcFdo, Xdpc, Transfer, Ev_TX_MpSubmitSuccess, TX_Mapped_inMp);

        if (IsListEmpty(&endpoint->TxMappedNotInMiniportListHead)) {

            USB_ASSERT_UNLINKED(&Transfer->TxMappedNotInMiniportListEntry);

        } else {

            // The transfer must have been on the
            // TxMappedNotInMiniportListHead as the head of the list.
            //
            // Remove the transfer from the TxMappedNotInMiniportListHead.
            //
            USBPORT_ASSERT(Transfer == CONTAINING_RECORD(
                endpoint->TxMappedNotInMiniportListHead.Flink,
                HCD_TRANSFER_CONTEXT,
                TxMappedNotInMiniportListEntry));

            RemoveEntryList(&Transfer->TxMappedNotInMiniportListEntry);

            USB_ZAP_LINK(&Transfer->TxMappedNotInMiniportListEntry);
        }

        USBPORT_WMI_EVENT(1,
                      USBPORT_TRANSFER_SCHEDULE,
                      (USBPORT_EVENT_TYPE_MINIPORT_SCHEDULED,
                       GUID_USB_TRANSFER_TRACING,
                       Transfer->Urb,
                       Transfer->Flags,
                       "Scheduling URB %p: devaddr=%d, func=%d, endpoint=%d",
                       Transfer->Urb,
                       endpoint->Parameters.DeviceAddress,
                       Transfer->Urb->Hdr.Function,
                       endpoint->Parameters.EndpointAddress));

        // miniport took it -- set the timeout
        // note: MillisecTimeout will set the timout to 'disabled' (no timeout)
        LOGENTRY(HcFdo, LOG_TRANSFER, 'subt', Transfer,  Transfer->MillisecondTimeout, 0);
        USBPORT_Core_InitTimeout(HcFdo, &Transfer->Timeout, Transfer, Transfer->MillisecondTimeout);
        break;

    case USBMP_STATUS_BUSY:
        USBPORT_TxSetState(HcFdo, Xdpc, Transfer, Ev_TX_MpSubmitBusy, TX_Mapped);

        if (IsListEmpty(&endpoint->TxMappedNotInMiniportListHead)) {

            // The transfer is mapped but not yet in the miniport.
            // Add the transfer to the TxMappedNotInMiniportListHead.
            //
            USB_ASSERT_UNLINKED(&Transfer->TxMappedNotInMiniportListEntry);

            InsertTailList(&endpoint->TxMappedNotInMiniportListHead,
                           &Transfer->TxMappedNotInMiniportListEntry);
        } else {

            // The transfer must have already been on the
            // TxMappedNotInMiniportListHead as the head of the list.
            //
            USBPORT_ASSERT(Transfer == CONTAINING_RECORD(
                endpoint->TxMappedNotInMiniportListHead.Flink,
                HCD_TRANSFER_CONTEXT,
                TxMappedNotInMiniportListEntry));
        }

        break;

    // other error completes the Transfer
    default:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'sube', mps, 0, Transfer);

        if (IsListEmpty(&endpoint->TxMappedNotInMiniportListHead)) {

            USB_ASSERT_UNLINKED(&Transfer->TxMappedNotInMiniportListEntry);

        } else {

            // The transfer must have been on the
            // TxMappedNotInMiniportListHead as the head of the list.
            //
            // Remove the transfer from the TxMappedNotInMiniportListHead.
            //
            USBPORT_ASSERT(Transfer == CONTAINING_RECORD(
                endpoint->TxMappedNotInMiniportListHead.Flink,
                HCD_TRANSFER_CONTEXT,
                TxMappedNotInMiniportListEntry));

            RemoveEntryList(&Transfer->TxMappedNotInMiniportListEntry);

            USB_ZAP_LINK(&Transfer->TxMappedNotInMiniportListEntry);
        }

        if (CompleteOnError) {
            USBD_STATUS usbdStatus;
            usbdStatus = USBPORT_MiniportStatus_TO_USBDStatus(mps);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'subE', mps, usbdStatus, Transfer);
            // return error to caller -- in this case caller will have to complete the transfer
            USBPORT_Core_iCompleteTransfer(HcFdo,
                                          Xdpc,
                                          Transfer,
                                          usbdStatus,
                                          Ev_TX_MpSubmitError,
                                          TX_Mapped_Error);
        } else {
            // set the error so we know its not in theminiport
            USBPORT_TxSetState(HcFdo, Xdpc, Transfer, Ev_TX_MpSubmitErrorNC, TX_Mapped_Error);
        }

        break;
    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'subx', Xdpc, mps, Transfer);

    return mps;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_CheckTransferQueues(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT FindTransfer
    )
/*++

Routine Description:

Checks all queues for the specified transfer

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT tx = NULL;
    PLIST_ENTRY le;
    KIRQL irql;

    devExt = GetHcFdoExt(HcFdo);

    irql = USBPORT_TxAcquireLock(HcFdo, NULL);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'ftt0', 0, 0, FindTransfer);

    // check prority queue
    le = devExt->Fdo.TxPriorityListHead.Flink;
    while (le != &devExt->Fdo.TxPriorityListHead && tx == NULL) {

        tx = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'ftt2', le, tx, FindTransfer);
        ASSERT_TRANSFER(tx);

        if (tx == FindTransfer) {
            break;
        }
        le = le->Flink;
        tx = NULL;

    }

    // check irp queue
    le = devExt->TxCsqIrpListHead.Flink;
    while (le != &devExt->TxCsqIrpListHead && tx == NULL) {

        tx = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxCsqIrpLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'ftt3', le, tx, FindTransfer);
        ASSERT_TRANSFER(tx);

        if (tx == FindTransfer) {
            break;
        }
        le = le->Flink;
        tx = NULL;

    }

    // check done queue
    le = devExt->Fdo.TxDoneListHead.Flink;
    while (le != &devExt->Fdo.TxDoneListHead && tx == NULL) {

        tx = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxDoneLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'ftt4', le, tx, FindTransfer);
        ASSERT_TRANSFER(tx);

        if (tx == FindTransfer) {
            break;
        }
        le = le->Flink;
        tx = NULL;

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'ftt5', 0, tx, FindTransfer);
    USBPORT_TxReleaseLock(HcFdo, NULL, irql);

    return tx;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindPriorityTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    )
/*++

Routine Description:

Returns the transfer in the priority queue matching the given irp. Returns NULL if the Irp is not found.  This
is part io the IoCancel path.

This routine returns the parent if the transfer is a split.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PLIST_ENTRY le;

    // walk the priority list
    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fnd0', 0, 0, Irp);

    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'fnd2', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        if (transfer->Irp && transfer->Irp == Irp &&
            transfer->TxType != Tx_SPLIT_CHILD) {

            break;
        }
        le = le->Flink;
        transfer = NULL;

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fnd3', 0, Irp, transfer);
    return transfer;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindCanceledTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    )
/*++

Routine Description:

Returns the transfer in the cancel queue matching the given irp. Returns NULL if the Irp is not found.  This
is part io the IoCancel path.

This routine returns the parent if the transfer is a split.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PLIST_ENTRY le;

    // walk the priority list
    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fnc0', 0, 0, Irp);

    le = devExt->Fdo.TxCancelIrpListHead.Flink;

    while (le != &devExt->Fdo.TxCancelIrpListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxCancelIrpLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'fnc2', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        if (transfer->Irp && transfer->Irp == Irp &&
            transfer->TxType != Tx_SPLIT_CHILD) {

            break;
        }
        le = le->Flink;
        transfer = NULL;

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fnc3', 0, Irp, transfer);
    return transfer;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindIoDoneTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    )
/*++

Routine Description:

Returns the transfer in the io done queue matching the given irp. Returns NULL if the Irp is not found.  This
is part io the IoCancel path.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PLIST_ENTRY le;

    // walk the priority list
    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fdd0', 0, 0, Irp);

    le = devExt->Fdo.TxDoneListHead.Flink;

    while (le != &devExt->Fdo.TxDoneListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxDoneLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'fdd2', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        if (transfer->Irp && transfer->Irp == Irp) {
            break;
        }
        le = le->Flink;
        transfer = NULL;

    }

    return transfer;
}


VOID
USBPORT_Core_iCancelPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

Cancels a specific transfer in the priority queue.

This routine is always called with Fdo.EndpointListSpin held

--*/
{
    PHCD_ENDPOINT endpoint;
    ULONG bytesTransferred;
    HC_GLOBAL_EP_STATE geps;

    // Transfer references the endpoint
    endpoint = USBPORT_EndpointFromHandle(Transfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);

    switch (Transfer->TxState) {
    case TX_Mapped_Split:
        // cancel parent automatically cancels all children
        // this call will re-enter this function for each child.
        USBPORT_Split_iCancelParentTransfer(HcFdo, Transfer);

        if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_MPCANCELED) &&
            endpoint->EpFlags.ClearTtBufferOnCancel) {

            //
            // If canceling a split parent transfer caused any of the
            // split child transfers to be canceled while actively
            // queued in the miniport, and if the endpoint needs
            // CLEAR_TT_BUFFER requests, then after all of the split
            // child tranfers were completed USBPORT_Core_iCompleteTransfer()
            // should have inserted this split parent transfer into the
            // TxCanceledPendingClearTTCompletionListHead.
            //
            USBPORT_ASSERT(USBPORT_IsLinked(&Transfer->TxClearTTCompletionListEntry));
        }

        break;

    // Transfer in hardware
    case TX_Mapped_inMp_Canceled:
    case TX_Mapped_inMp:

        geps = endpoint->GepState;

        switch (geps) {
        case GEp_Zapped:
        case GEp_Removed:
        case GEp_Paused:
        case GEp_PausedClearTTNeeded:
        case GEp_PausedClearTTPending:
        case GEp_ActiveClearTTNeeded:
        case GEp_ActiveClearTTPending:

            //
            // Now that the endpoint has been removed from hardware
            // access in the miniport as indicated by the current
            // endpoint state it is now safe to tell the miniport to
            // abort/cancel the transfer out of the miniport.
            //
            // NOTE: This is the only code path in USBPORT where the
            // "Ev_gEp_AbortTransfer" event is issued, which will in
            // turn trigger the only code path in USBPORT where
            // USBPORT_iSetGlobalEndpointStateTx() calls
            // MPf_AbortTransfer().  This is also the only code path
            // where the USBPORT_TXFLAG_MPCANCELED flag is set, other
            // than USBPORT_Split_iCompleteChildTransfer() which copies
            // the USBPORT_TXFLAG_MPCANCELED flag from a split child
            // tranfser to the parent.
            //
            bytesTransferred = 0;

            //
            // ABORT THE TRANSFER OUT OF THE MINIPORT.
            //
            SET_FLAG(Transfer->Flags, USBPORT_TXFLAG_MPCANCELED);
            //MPf_AbortTransfer(devExt, endpoint, Transfer, &bytesTransferred);
            USBPORT_iSetGlobalEndpointStateTx(HcFdo, endpoint, Ev_gEp_AbortTransfer, Transfer, NULL, &bytesTransferred);

            //
            // Make sure we indicate any data that was actually
            // transferred prior to the abort.
            //
            if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_ISO)) {
                USBPORT_FlushIsoTransfer(HcFdo,
                                         endpoint,
                                         &Transfer->Tp,
                                         Transfer->IsoTransfer);
            } else {
                Transfer->MiniportBytesTransferred = bytesTransferred;
            }
            LOGENTRY(HcFdo, LOG_TRANSFER, 'can0', endpoint, Transfer, bytesTransferred);

            //
            // Mark the transfer as done (in this case TX_Mapped_Canceled)
            // and signal the USBPORT_Core_UsbDoneDpc_Worker() to run.
            // When that eventually runs it will move the transfer from
            // the TxPriorityListHead to the TxDoneListHead.
            //
            // The Fdo.EndpointListSpin (TX Lock) held remains held
            // during this call.
            //
            USBPORT_Core_iCompleteTransfer(HcFdo,
                                           NULL,
                                           Transfer,
                                           USBD_STATUS_CANCELED,
                                           Ev_TX_CancelPriorityTransfer_Paused,
                                           TX_Mapped_Canceled);
            break;

        case GEp_Active:
            // endpoint needs to pause so we can abort
            USBPORT_TxSetState(HcFdo, NULL, Transfer, Ev_TX_CancelPriorityTransfer, TX_Mapped_inMp_Canceled);

            // pause the endpoint
            USBPORT_iSetGlobalEndpointState(HcFdo, endpoint, Ev_gEp_ReqPauseCancel);

            break;

        case GEp_RemovePending:
        case GEp_PausePendingCancel:
        case GEp_PausePending:
            // will be handled when the endpoint finishes transition
            USBPORT_TxSetState(HcFdo, NULL, Transfer, Ev_TX_CancelPriorityTransfer, TX_Mapped_inMp_Canceled);
            break;

        default:
            USBPORT_ASSERTMSG("Unexpected Transfer->TxState",FALSE);

        }
        break;

    // Transfer just in queue
    case TX_InQueue:

        USBPORT_Core_iCompleteTransfer(HcFdo,
                                       NULL,
                                       Transfer,
                                       USBD_STATUS_CANCELED,
                                       Ev_TX_CancelPriorityTransfer,
                                       TX_Canceled);

        break;

    // mapped but not in HW
    case TX_Mapped:

        // A TX_Mapped transfer should be on the TxMappedNotInMiniportListHead
        //
        USBPORT_ASSERT(!IsListEmpty(&endpoint->TxMappedNotInMiniportListHead));

        USBPORT_DBG_ASSERT_ISONLIST(&endpoint->TxMappedNotInMiniportListHead,
                                    &Transfer->TxMappedNotInMiniportListEntry);

        // Remove the transfer from the TxMappedNotInMiniportListHead.
        //
        RemoveEntryList(&Transfer->TxMappedNotInMiniportListEntry);

        USB_ZAP_LINK(&Transfer->TxMappedNotInMiniportListEntry);

        USBPORT_Core_iCompleteTransfer(HcFdo,
                                       NULL,
                                       Transfer,
                                       USBD_STATUS_CANCELED,
                                       Ev_TX_CancelPriorityTransfer,
                                       TX_Mapped_Canceled);

        break;

    // not quite mappped
    case TX_MapPending:
        USBPORT_TxSetState(HcFdo, NULL, Transfer, Ev_TX_CancelPriorityTransfer, TX_MapPending_Canceled);
        break;

    case TX_Mapped_IoComplete:
        // already due to complete (override status?)
        break;

    default:
        TEST_TRAP();
        break;

    }
}


VOID
USBPORT_Core_iRemovePriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

Removes a transfer from the priority queue and updates the context next pointers for all xdpcs.

--*/
{
    PLIST_ENTRY le;
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);
    // update next pointers for all Dpcs
    // ** this only needs to be done if a dpc releases the lock while traversing the list otherwise
    // the transfer should not be found

    le = devExt->Fdo.XdpcListHead.Flink;

    while (le != &devExt->Fdo.XdpcListHead) {
        PXDPC_CONTEXT xdpc;

        xdpc = (PXDPC_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _XDPC_CONTEXT,
                        XdpcLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'tRM0', le, 0, xdpc);
        ASSERT_XDPC(xdpc);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'tRM2', xdpc->NextTransfer, Transfer, xdpc);

        //is it current on this xdpc?
        if (xdpc->NextTransfer &&
            xdpc->NextTransfer == Transfer) {
            // advance to next
            PHCD_TRANSFER_CONTEXT nextNextTransfer;
            PLIST_ENTRY txLe;

            //TEST_TRAP();
            txLe = Transfer->TxPriorityLink.Flink;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'tRM3', txLe, 0, xdpc);

            if (txLe == &devExt->Fdo.TxPriorityListHead) {
                // end of list
                LOGENTRY(HcFdo, LOG_TRANSFER, 'tRM4', 0, 0, xdpc);
                xdpc->NextTransfer = NULL;
            } else {
                nextNextTransfer = CONTAINING_RECORD(txLe,
                                      HCD_TRANSFER_CONTEXT,
                                      TxPriorityLink);
                LOGENTRY(HcFdo, LOG_TRANSFER, 'tRM5', nextNextTransfer, 0, xdpc);
                ASSERT_TRANSFER(nextNextTransfer);

                xdpc->NextTransfer = nextNextTransfer;
            }
        }

        le = le->Flink;
    }

    RemoveEntryList(&Transfer->TxPriorityLink);
    USB_ZAP_LINK(&Transfer->TxPriorityLink);

    // should not be on any endpoint list
    USB_ASSERT_UNLINKED(&Transfer->TxEndpointLink);

    // Update the host controller Async and Periodic transfer counts.
    // Notify the miniport if the count decrements from 1 -> 0.
    //
    switch (Transfer->EpTransferType)
    {
        case Bulk:
        case Control:
            USBPORT_ASSERT(devExt->Fdo.TxPriorityListAsyncCount > 0);

            if (--devExt->Fdo.TxPriorityListAsyncCount == 0)
            {
                MPf_NotifyTransferQueueState(devExt,
                                             devExt->Fdo.TxPriorityListAsyncCount,
                                             devExt->Fdo.TxPriorityListPeriodicCount);
            }
            break;

        case Interrupt:
        case Isochronous:
            USBPORT_ASSERT(devExt->Fdo.TxPriorityListPeriodicCount > 0);

            if (--devExt->Fdo.TxPriorityListPeriodicCount == 0)
            {
                MPf_NotifyTransferQueueState(devExt,
                                             devExt->Fdo.TxPriorityListAsyncCount,
                                             devExt->Fdo.TxPriorityListPeriodicCount);
            }
            break;
    }

    if (IsListEmpty(&devExt->Fdo.TxPriorityListHead)) {
        KeSetEvent(&devExt->Fdo.TxPriorityListEmpty_Event, IO_NO_INCREMENT, FALSE);
    }
}


PHCD_TRANSFER_CONTEXT
USBPORT_Core_iGetNextPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    )
/*++

Routine Description:

Returns the next transfer in the priority queue based on current context

--*/
{
    PHCD_TRANSFER_CONTEXT nextTransfer = NULL;
    PLIST_ENTRY le;
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'gNX0', 0, 0, Xdpc);
    USBPORT_ASSERT(Xdpc->TxLockState);

    if (Xdpc->NextTransfer) {
        nextTransfer = Xdpc->NextTransfer;

        LOGENTRY(HcFdo, LOG_TRANSFER, 'gNX1', nextTransfer, 0, Xdpc);
        le = nextTransfer->TxPriorityLink.Flink;
        if (le == &devExt->Fdo.TxPriorityListHead) {
            // end of list
            LOGENTRY(HcFdo, LOG_TRANSFER, 'gNX2', le, 0, Xdpc);
            Xdpc->NextTransfer = NULL;
        } else {
            PHCD_TRANSFER_CONTEXT nextNextTransfer;

            nextNextTransfer =  CONTAINING_RECORD(le,
                                                  HCD_TRANSFER_CONTEXT,
                                                  TxPriorityLink);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'gNX3', nextNextTransfer, 0, Xdpc);
            ASSERT_TRANSFER(nextNextTransfer);

            Xdpc->NextTransfer = nextNextTransfer;
        }
    }

    // for debugging
    Xdpc->CurrentTransfer = nextTransfer;

    return nextTransfer;
}


VOID
USBPORTSVC_InvalidateEndpoint(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    Called by miniport to indicate a particular endpoint needs attention.  This is now a NOP.

--*/
{
    PDEVICE_EXTENSION devExt;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
}


VOID
USBPORTSVC_CompleteTransfer(
    PDEVICE_DATA DeviceData,
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    USBD_STATUS UsbdStatus,
    ULONG BytesTransferred
    )
/*++

Routine Description:

 * Service called by the miniport to complete an async transfer.

 * ** Must be called in the context of PollEndpoint

 * When called in the context of PollEndpoint the TxLock is acquired.

 * When the miniport endpoint is polled it checks to see if any TDs for
 * the endpoint have completed and processes completed TDs.  If the last
 * outstanding TD for a transfer has completed it calls
 * USBPORTSVC_CompleteTransfer().
 *
 * USBPORTSVC_CompleteTransfer() basically just calls
 * USBPORT_Core_iCompleteTransfer() to mark the transfer in the
 * TX_HwDone state and then queues a call to one of the four main
 * USBPORT DPC workers, USBPORT_Core_UsbDoneDpc_Worker().

Return Value:

    None.

--*/
{
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PDEVICE_OBJECT hcFdo;
    PTRANSFER_URB urb;
    PXDPC_CONTEXT xdpc;


    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    // sould be in the context of the usbx dpc wich polls the endpoint
    xdpc = &devExt->Fdo.UsbHcIntDpc;
    hcFdo = devExt->HcFdoDeviceObject;

    // spew for xfers
    DbgTrace((UsbpDebugTrace, "'--- xfer length %x (Complete)\n",  BytesTransferred));

    LOGENTRY(hcFdo, LOG_TRANSFER, 'cmpT', BytesTransferred,
        UsbdStatus, TransferParameters);

    TRANSFER_FROM_TPARAMETERS(transfer, TransferParameters);
    ASSERT_TRANSFER(transfer);

    if (devExt->CurrentAbortTransfer != transfer &&
        devExt->CurrentAbortTransfer != NULL) {
        // not current means we are being called in the context of AbortTransfer
        // we will allow this
        xdpc = NULL;
    }

    SET_FLAG(transfer->Flags, USBPORT_TXFLAG_MPCOMPLETED);

    urb = transfer->Urb;
    LOGENTRY(hcFdo, LOG_TRANSFER, 'cmpU', devExt->CurrentAbortTransfer, transfer, urb);
    ASSERT_TRANSFER_URB(urb);

    endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
    transfer->MiniportBytesTransferred = BytesTransferred;

    // check for short split, if it is a short mark all transfers not called down yet

    if (transfer->TxType == Tx_SPLIT_CHILD &&
        BytesTransferred < transfer->Tp.TransferBufferLength) {

        PLIST_ENTRY le;
        PHCD_TRANSFER_CONTEXT tmpTransfer;
        PHCD_TRANSFER_CONTEXT parentTransfer;

        // get the parent
        parentTransfer = transfer->ParentTransfer;

        // start at next transfer and release all children
        le = transfer->SplitLink.Flink;

        while (le != &parentTransfer->SplitTransferListHead) {

            //TEST_TRAP();

            tmpTransfer =  (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        SplitLink);

            LOGENTRY(hcFdo, LOG_TRANSFER, 'cmp0', tmpTransfer, 0, 0);

            ASSERT_TRANSFER(tmpTransfer);
            USBPORT_ASSERT(tmpTransfer->TxType == Tx_SPLIT_CHILD);

            le = tmpTransfer->SplitLink.Flink;

            if (tmpTransfer->TxState != TX_Mapped_inMp &&
                tmpTransfer->TxState != TX_Mapped_inMp_Canceled) {

                // free this child unless its in the miniport
                // ** the miniport will complete all transfers for a split in the event of a short packet.
                // ** it uses the Tp.SequenceNumber to

                if (tmpTransfer->TxState == TX_Mapped) {

                    // A TX_Mapped transfer should be on the TxMappedNotInMiniportListHead
                    //
                    USBPORT_ASSERT(!IsListEmpty(&endpoint->TxMappedNotInMiniportListHead));

                    USBPORT_DBG_ASSERT_ISONLIST(&endpoint->TxMappedNotInMiniportListHead,
                                                &tmpTransfer->TxMappedNotInMiniportListEntry);

                    // Remove the transfer from the TxMappedNotInMiniportListHead.
                    //
                    RemoveEntryList(&tmpTransfer->TxMappedNotInMiniportListEntry);

                    USB_ZAP_LINK(&tmpTransfer->TxMappedNotInMiniportListEntry);
                }

                USBPORT_Core_iCompleteTransfer(hcFdo,
                                      xdpc,
                                      tmpTransfer,
                                      UsbdStatus,
                                      Ev_TX_TerminateSplit,
                                      TX_Mapped_TerminateSplit);
            }
        } /* while */

    }

    USBPORT_WMI_EVENT(1,
                      USBPORT_TRANSFER_SCHEDULE,
                      (USBPORT_EVENT_TYPE_MINIPORT_DONE,
                       GUID_USB_TRANSFER_TRACING,
                       transfer->Urb,
                       BytesTransferred,
                       "Transfer completed by miniport URB %p: devaddr=%d, func=%d, endpoint=%d",
                       transfer->Urb,
                       endpoint->Parameters.DeviceAddress,
                       transfer->Urb->Hdr.Function,
                       endpoint->Parameters.EndpointAddress));


    // schedule usb_done dpc to run
    USBPORT_Core_iCompleteTransfer(hcFdo,
                                  xdpc,
                                  transfer,
                                  UsbdStatus,
                                  Ev_TX_MiniportComplete,
                                  TX_HwDone);
}


VOID
USBPORT_Core_iEndpointPaused(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Process the ENDPOINT_PAUSE state transition. This routine is called when the endpoint transitions from
    ACTIVE to PAUSE.

    EpcList/Tx lock is alread acquired here
--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT txEp;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Epp0', Endpoint, Endpoint->GepState, 0);

    // The expected endpoint state on entry is either GEp_Paused or
    // GEp_Removed.  If it is GEp_Removed then no
    // TX_Mapped_inMp_Canceled transfers are expected to be found.
    //
    USBPORT_ASSERT((Endpoint->GepState == GEp_Paused) ||
                   (Endpoint->GepState == GEp_Removed));

    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        ASSERT_TRANSFER(transfer);
        le = le->Flink;

        txEp = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
        ASSERT_ENDPOINT(txEp);

        if (Endpoint != txEp) {
            // skip if transfer is not for this endpoint
            continue;
        }

        switch (transfer->TxState) {
        case TX_Mapped_inMp_Canceled:

            // The endpoint state may change as a result of calling
            // USBPORT_Core_iCancelPriorityTransfer().  The endpoint
            // state is now expected to be either GEp_Paused, or one of
            // the ClearTTNeeded or ClearTTPending states.
            //
            USBPORT_ASSERT((Endpoint->GepState == GEp_Paused)               ||
                           (Endpoint->GepState == GEp_PausedClearTTNeeded)  ||
                           (Endpoint->GepState == GEp_PausedClearTTPending) ||
                           (Endpoint->GepState == GEp_ActiveClearTTNeeded)  ||
                           (Endpoint->GepState == GEp_ActiveClearTTPending));

            if (TEST_FLAG(transfer->Flags, USBPORT_TXFLAG_ISO)) {
                // cancel iso only if time has expired for this transfer

                ULONG cf, lastFrame;
                PTRANSFER_URB urb;

                urb = transfer->Urb;
                ASSERT_TRANSFER_URB(urb);
                // iso transfer in the miniport, we need to let the time elapse before doing an abort.
                //
                // of the trabsfer in the miniport.  The reason for this is the uhci and ehci controllers do not
                // have queue heads that can be paused for isoch so the hardware can still access the tds.
                lastFrame = urb->u.Isoch.StartFrame + urb->u.Isoch.NumberOfPackets;

                LOGENTRY(HcFdo, LOG_TRANSFER, 'Epp1', transfer, urb, lastFrame);

                // get the current frame
                cf = MPf_Get32BitFrameNumber(devExt);

                LOGENTRY(HcFdo, LOG_TRANSFER, 'Epp2', transfer, cf, lastFrame);

                if (devExt->Fdo.Flags.ControllerGone || (GREATER_THAN(cf, (lastFrame + 1)))) {
                    LOGENTRY(HcFdo, LOG_TRANSFER, 'Epp3', transfer, cf, 0);
                    USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);
                }

            } else {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'Epp4', transfer, 0, 0);

                // async transfer, cancel now
                USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);
            }
            break;

        default:
            break;

        }
    }
}


VOID
USBPORT_Core_iEndpointActive(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Process the ENDPOINT_ACTIVE state transition. This routine is called when the endpoint transitions to ACTIVE. The
    transfer list is scanned, if any mapped transfers (waiting) are detected then the MapDpc routine is signalled to
    run.

    EpcList/Tx lock is already acquired here
--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT txEp;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'EppA', 0, 0, 0);


    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        ASSERT_TRANSFER(transfer);
        le = le->Flink;

        txEp = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
        USBPORT_iAssertEndpoint(HcFdo, txEp);

        if (Endpoint != txEp) {
            // skip if transfer is not for this endpoint
            continue;
        }


        if (transfer->TxState == TX_Mapped) {
            USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, NULL);
            // only one signal needed
            break;
        }
    }
}


VOID
USBPORT_Core_IoCompleteIrp(
    __inout PDEVICE_OBJECT DeviceObject,
    __inout PIRP Irp,
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __in LONG NtStatus,
    __in ULONG_PTR Information
    )
/*++

Routine Description:

    Completes an I/O Request


--*/
{
    DbgTrace((UsbpDebugTrace, "'USBPORT_CompleteIrp status = %x\n", NtStatus));

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = Information;
//  LOGENTRY(FdoDeviceObject, LOG_MISC, 'irpC', Irp, DeviceObject, Urb);

    // free the iorequest first since the irp may be re-used
    if (UsbIoRequest) {
        USBPORT_Core_UnlinkAndFreeIoRequest(UsbIoRequest, Irp);
    }

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return;
}


VOID
USBPORT_Core_InitTimeout(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_TIMEOUT Timeout,
    __in_opt PVOID OwnerObject,
    __in ULONG MillisecondTimeout
    )
/*++

    Initialize USB timeout object.  These structures are used to compute transfer and dpc timeouts in tick counts.

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG timerIncrement;

    DbgTrace((UsbpDebugTrace, "USBPORT_Core_InitTimeoutObject\n"));

    Timeout->Sig = SIG_USB_TIMEOUT;
    Timeout->HcFdo = HcFdo;
    // backpointer for debugging
    Timeout->OwnerObject = OwnerObject;

    if (MillisecondTimeout == 0) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'tmoZ', MillisecondTimeout, OwnerObject, Timeout);
        Timeout->TmoState = Tmo_Disabled;
        return;
    }
    // note the begin time
    timerIncrement = KeQueryTimeIncrement();
    KeQueryTickCount(&Timeout->TmoBeginTickCount);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'tmo0', MillisecondTimeout, OwnerObject, Timeout);

    //
    // cycles = 100NS_units
    // tick = interval timer ticks
    //
    // tickcount = current count of interval timer ticks (KeQueryTickCount)
    // cycles_p_tick = cycles/tick (KeQueryTimeIncrement)
    // ms_timeout = timeout in milliseconds
    // cycle_timeout = timeout in cycles (ms*10000)
    // tick_timeout = timeout in ticks (cycle_timeout/cycles_p_tick)
    //

    do {
        LARGE_INTEGER ms_timeout, cycles_p_tick, tick_timeout, cycle_timeout;


        ms_timeout = RtlConvertUlongToLargeInteger(MillisecondTimeout);
        cycles_p_tick = RtlConvertUlongToLargeInteger(timerIncrement);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'tmo1', cycles_p_tick.HighPart, cycles_p_tick.LowPart, Timeout);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'tmo2', ms_timeout.HighPart, ms_timeout.LowPart, Timeout);

        // convert ms timeout to cycles (100 ns units)
        // cycle_timeout = ms_timeout * 10000
        cycle_timeout.QuadPart = ms_timeout.QuadPart * 10000;

        if (cycle_timeout.QuadPart < cycles_p_tick.QuadPart) {
            // set to smallest interval
            cycle_timeout.QuadPart = cycles_p_tick.QuadPart;
        }
        LOGENTRY(HcFdo, LOG_TRANSFER, 'tmo3', cycle_timeout.HighPart, cycle_timeout.LowPart, Timeout);


        // tick_timeout = cycle_timeout / cycles_p_tick
        tick_timeout.QuadPart = cycle_timeout.QuadPart / cycles_p_tick.QuadPart;

        LOGENTRY(HcFdo, LOG_TRANSFER, 'tmo4', tick_timeout.HighPart, tick_timeout.LowPart, Timeout);

        Timeout->MillisecondTimeout = MillisecondTimeout;
        Timeout->TmoTimeoutTickCount.QuadPart = tick_timeout.QuadPart;

        //TmoExpireTickCount = BeginTickCount + tick_timeout;
        Timeout->TmoExpireTickCount.QuadPart = Timeout->TmoBeginTickCount.QuadPart + tick_timeout.QuadPart;

    } WHILE (0);

    if (NT_SUCCESS(nts)) {
        Timeout->TmoState = Tmo_Enabled;
    }
    return;
}


BOOLEAN
USBPORT_Core_CheckTimeout(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_TIMEOUT Timeout
    )
/*++

    Checks the input timeout object and returns true if the specified amount of time has elapsed since the call to
    USBPORT_Core_InitTimeoutObject.

    If the time has expired the state is set to disabled so that it is not re-used.

--*/
{
    BOOLEAN timeout = FALSE;
    LARGE_INTEGER now;

    DbgTrace((UsbpDebugTrace, "USBPORT_Core_CheckTimeoutObject\n"));

    ASSERT_USB_TIMEOUT(Timeout);
    KeQueryTickCount(&now);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'tmoK', now.HighPart, now.LowPart, Timeout);


    do {
        if (Timeout->TmoState == Tmo_Disabled) {
            break;
        }

        if (Timeout->TmoState == Tmo_DisabledExpired) {
            break;
        }

        if (now.QuadPart > Timeout->TmoExpireTickCount.QuadPart) {
            timeout = TRUE;
            Timeout->TmoState = Tmo_DisabledExpired;
            TEST_TRAP();
        }

    } WHILE (0);

    return timeout;
}


LONG
USBPORT_Core_iGetIoRequestCount(
    __inout PDEVICE_OBJECT DevObj
    )
/*++

Routine Description:

    Returns the current IoRequestCount by counting the items in the list.  This is used to sanity check the count.

--*/
{
    PDEVICE_EXTENSION ext;
    LONG ioCount;

    ext = DevObj->DeviceExtension;
#if DBG
    {
        PUSB_IOREQUEST_CONTEXT ior = NULL;
        PLIST_ENTRY le;

        // count requests
        // 0 items, counter = -1
        // 1 item, counter = 0
        // ...

        ioCount = -1;
        le = ext->IoRequestListHead.Flink;

        while (le != &ext->IoRequestListHead) {
            ior =  CONTAINING_RECORD(le,
                                      USB_IOREQUEST_CONTEXT,
                                      IoRequestLink);

            ASSERT_USB_IOREQUEST(ior);

            le = le->Flink;
            ioCount++;
        }

        USBPORT_ASSERT(ioCount + ext->IoRequestCountNoRes == ext->IoRequestCount);
    }
#else
    ioCount = ext->IoRequestCount;
#endif

    return ioCount;
}


LONG
USBPORT_Core_GetIoRequestCount(
    __inout PDEVICE_OBJECT DevObj
    )
/*++

Routine Description:

    Returns the current IoRequestCount by counting the items in the list.  This is used to sanity check the count.

--*/
{
    PDEVICE_EXTENSION ext;
    KIRQL irql;
    LONG ioCount;

    ext = DevObj->DeviceExtension;

    KeAcquireSpinLock(&ext->IoRequestSpin, &irql);
    ioCount = USBPORT_Core_iGetIoRequestCount(DevObj);
    KeReleaseSpinLock(&ext->IoRequestSpin, irql);

    return ioCount;
}


LONG
USBPORT_Core_IncIoRequestCount(
    __inout PDEVICE_OBJECT DevObj,
    __inout PUSB_IOREQUEST_CONTEXT UsbIoContext,
    __inout PVOID Object,
    __in ULONG Tag,
    __in BOOLEAN FreeContext
    )
/*++

Routine Description:

    Increment outstanding IO count on the given device object.  This routine provides remlock type behavior for both
    the FDO and Root Hub PDO.  This routine combines the Irp Tables and the IO request counter into a single ref list.

    FreeContext - will free the context structure when it is removed from the list with a call to
        USBPORT_Core_DecIoRequestCount.

    Returns the current value of the iorequest counter.

--*/
{
    PDEVICE_EXTENSION ext;
    KIRQL irql;
    LONG ioc, removeEvState;

    ext = DevObj->DeviceExtension;

    KeAcquireSpinLock(&ext->IoRequestSpin, &irql);

#if DBG
    {
        PUSB_IOREQUEST_CONTEXT ior = NULL;
        PLIST_ENTRY le;
        // in debug build scan the list for the request

        le = ext->IoRequestListHead.Flink;

        while (le != &ext->IoRequestListHead) {
            ior =  CONTAINING_RECORD(le,
                                      USB_IOREQUEST_CONTEXT,
                                      IoRequestLink);

            ASSERT_USB_IOREQUEST(ior);
            if (UsbIoContext == ior) {
                break;
            }
            le = le->Flink;
            ior = NULL;
        }

        USBPORT_ASSERT(ior == NULL);
    }
#endif


    UsbIoContext->Sig = SIG_USB_IOREQUEST;
    USBPORT_ASSERT(!UsbIoContext->DevObj || UsbIoContext->DevObj == DevObj);
    UsbIoContext->DevObj = DevObj;

    if (UsbIoContext != &ext->IoRequestContext) {
        UsbIoContext->Tag = Tag;
        UsbIoContext->FreeContext = FreeContext;
        UsbIoContext->OriginalObject =
            UsbIoContext->Object = Object;
        UsbIoContext->ObjUnlinked = FALSE;
        InsertTailList(&ext->IoRequestListHead, &UsbIoContext->IoRequestLink);
    } else {
        UsbIoContext->FreeContext = FALSE;
        UsbIoContext->ObjUnlinked = TRUE;
        ext->IoRequestCountNoRes++;
    }

    ext->IoRequestCount++;

    if (Tag == UsbIoTag_MpAsyncTimer) {
        //
        // If this IoRequest is an AsyncCallback request then also
        // increment the AsyncCallback request count.
        //
        ext->IoRequestAsyncCallbackCount++;

        if (ext->IoRequestAsyncCallbackCount == 0) {
            //
            // If IoRequestAsyncCallbackCount went from -1 (initial
            // condition) to 0 then reset IoRequestAsyncCallbackEvent.
            //
            KeResetEvent(&ext->IoRequestAsyncCallbackEvent);
        }
    }

    // sanity check the ref counter
    ioc = USBPORT_Core_iGetIoRequestCount(DevObj);

    removeEvState = KeReadStateEvent(&ext->IoRequestRemoveEvent);
    //stopEvState = KeReadStateEvent(&ext->IoRequestStopEvent);
    if (ioc > 0) {
        // events should not be signalled
        USBPORT_ASSERT(removeEvState == 0);
        //USBPORT_ASSERT(stopEvState == 0);
    }
    KeReleaseSpinLock(&ext->IoRequestSpin, irql);

    return ext->IoRequestCount;
}


PUSB_IOREQUEST_CONTEXT
USBPORT_Core_AllocIoRequest(
    __inout PDEVICE_OBJECT DevObj,
    __in_opt PVOID Object,
    __inout ULONG Tag
    )
/*++

Routine Description:

    Allocates a context and increments the pending Io Count

    if Owner object is NULL the iorequest is owned by itself.

--*/
{
    NTSTATUS nts;
    PUSB_IOREQUEST_CONTEXT ior;
    PVOID ownerObject;

#ifdef USB_FORCE_MEMORY_STRESS
    nts = STATUS_INSUFFICIENT_RESOURCES;
    ior = NULL;
#else
    nts = UsbAllocPool_Z(&ior, NonPagedPoolNx, sizeof(struct _USB_IOREQUEST_CONTEXT));
#endif

    if (nts == STATUS_INSUFFICIENT_RESOURCES && ior == NULL) {
        // use the reserved io request context object
        PDEVICE_EXTENSION ext;
        ext = DevObj->DeviceExtension;
        ior = &ext->IoRequestContext;
        nts = STATUS_SUCCESS;
    }

    if (NT_ERROR(nts) || ior == NULL) {
        if (ior != NULL) {
            UsbFreePool(ior);
            ior = NULL;
        }
        return NULL;
    }

    if (Object == NULL) {
        ownerObject = ior;
    } else {
        ownerObject = Object;
    }

    USBPORT_Core_IncIoRequestCount(DevObj, ior, ownerObject, Tag, TRUE);

    return ior;
}


VOID
USBPORT_Core_FreeIoRequest(
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Frees a pending Io context on a device object


--*/
{
    PDEVICE_OBJECT devObj;

    if (UsbIoRequest == NULL) {
        return;
    }

    ASSERT_USB_IOREQUEST(UsbIoRequest);
    devObj = UsbIoRequest->DevObj;

    USBPORT_Core_DecIoRequestCount(devObj, UsbIoRequest, FALSE);
}

/*++

Routine Description:

    dissassociates then frees a pending Io context on a device object.


--*/
VOID
USBPORT_Core_UnlinkAndFreeIoRequest(
    __inout PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __inout PVOID Object
    )
{
    PUSB_IOREQUEST_CONTEXT ior;

    ASSERT_USB_IOREQUEST(UsbIoRequest);

    ior = USBPORT_Core_GetIoRequestFromObject(UsbIoRequest->DevObj, Object);
    USBPORT_ASSERT(ior == UsbIoRequest);
    USBPORT_Core_FreeIoRequest(UsbIoRequest);
}


PUSB_IOREQUEST_CONTEXT
USBPORT_Core_GetIoRequestFromObject(
    __inout PDEVICE_OBJECT DevObj,
    __inout PVOID UsbIoObject
    )
/*++

Routine Description:

    Locates an object in the iorequest queue and returns a pointer to the request so the ref can
    be released.

    NOTE: We use two routines so that we can free the ref after completing the associated Irp request. This routine
    can will only return the ref one time.  The first time it is called the request object is marked so it will not
    be returned again.


    ior = USBPORT_Core_GetIoRequestFromObject(Irp)
    ...
    IoCompleteRequest(Irp)
    ...
    USBPORT_Core_FreeIoRequest(ior)

    ** This should only be used to assist the release of the IoRequest (ie the iorequest has an
        implied refcount of 1)
--*/
{
    PDEVICE_EXTENSION ext;
    KIRQL irql;
    PUSB_IOREQUEST_CONTEXT ior = NULL;
    PLIST_ENTRY le;

    ext = DevObj->DeviceExtension;

    USBPORT_ASSERT(UsbIoObject);

    KeAcquireSpinLock(&ext->IoRequestSpin, &irql);

    // scan the list for the request

    le = ext->IoRequestListHead.Flink;

    while (le != &ext->IoRequestListHead) {
        ior =  CONTAINING_RECORD(le,
                                  USB_IOREQUEST_CONTEXT,
                                  IoRequestLink);

        ASSERT_USB_IOREQUEST(ior);
        if (ior->Object == UsbIoObject &&
            ior->Object != NULL) {

            USBPORT_ASSERT(ior->ObjUnlinked == FALSE);
            USBPORT_ASSERT(ior->OriginalObject == UsbIoObject);
            ior->Object = NULL;
            ior->ObjUnlinked = TRUE;
            break;
        }
        le = le->Flink;
        ior = NULL;
    }

    if (!ior && ext->IoRequestCountNoRes) {
        // if we didn't find the ior and we have a non-zero
        // resource-less reference count to the device, return
        // ext->IoRequestContext
        ior = &ext->IoRequestContext;
    }

    USBPORT_ASSERT(ior);

    KeReleaseSpinLock(&ext->IoRequestSpin, irql);

    return ior;
}


LONG
USBPORT_Core_DecIoRequestCount(
    __inout PDEVICE_OBJECT DevObj,
    __inout PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __in BOOLEAN InternalRequest
    )
/*++

Routine Description:

    Increment outstanding IO count on the given device object.  This routine provides remlock type behavior for both
    the FDO and Root Hub PDO.

    InternalRequest - indicates an internal request that is unlinked when the ref is dropped
--*/
{
    PDEVICE_EXTENSION ext;
    KIRQL irql;
    PKEVENT removeEvent = NULL;
    LONG ioc;

    ASSERT_USB_IOREQUEST(UsbIoRequest);
    // freed in the wrong context
    USBPORT_ASSERT(DevObj);
    USBPORT_ASSERT(UsbIoRequest->DevObj);
    USBPORT_ASSERT(UsbIoRequest->DevObj == DevObj);

    ext = DevObj->DeviceExtension;

    KeAcquireSpinLock(&ext->IoRequestSpin, &irql);

#if DBG
    if (UsbIoRequest != &ext->IoRequestContext) {
        PUSB_IOREQUEST_CONTEXT ior = NULL;
        PLIST_ENTRY le;

        // in debug build scan the list for the request
        USBPORT_AssertListEntry(&UsbIoRequest->IoRequestLink);

        le = ext->IoRequestListHead.Flink;

        while (le != &ext->IoRequestListHead) {
            ior =  CONTAINING_RECORD(le,
                                      USB_IOREQUEST_CONTEXT,
                                      IoRequestLink);

            ASSERT_USB_IOREQUEST(ior);
            if (UsbIoRequest == ior) {
                break;
            }
            le = le->Flink;
            ior = NULL;
        }

        USBPORT_ASSERT(ior);
    }
#endif

    if (UsbIoRequest != &ext->IoRequestContext) {
        RemoveEntryList(&UsbIoRequest->IoRequestLink);
        USB_ZAP_LINK(&UsbIoRequest->IoRequestLink);
        if (InternalRequest) {
            USBPORT_ASSERT(UsbIoRequest->ObjUnlinked == FALSE);
            UsbIoRequest->Object = NULL;
            UsbIoRequest->ObjUnlinked = TRUE;
        } else {
            USBPORT_ASSERT(UsbIoRequest->Object == NULL);
        }
    } else {
        ext->IoRequestCountNoRes--;
    }

    // after a successful pnp start there are two entries on the list:
    // for FDO
    //      UsbIoTag_PnpAdd
    //      UsbIoTag_PnpStart
    //
    // for PDO
    //      UsbIoTag_RhCreate
    //


    ext->IoRequestCount--;

    if (UsbIoRequest->Tag == UsbIoTag_MpAsyncTimer) {
        //
        // If this IoRequest is an AsyncCallback request then also
        // decrement the AsyncCallback request count.
        //
        ext->IoRequestAsyncCallbackCount--;

        if (ext->IoRequestAsyncCallbackCount == -1) {
            //
            // If IoRequestAsyncCallbackCount went from 0 to -1 (initial
            // condition) then set IoRequestAsyncCallbackEvent.
            //
            KeSetEvent(&ext->IoRequestAsyncCallbackEvent, 1, FALSE);
        }
    }

    // sanity check the ref counter
    ioc = USBPORT_Core_iGetIoRequestCount(DevObj);

    // 0 indicates - stop?
    if (ext->IoRequestCount == 0) {
        USBPORT_ASSERT(ioc == 0);
        KeSetEvent(&ext->IoRequestStopEvent, 1, FALSE);
    }

    // -1 indicates - remove
    if (IsListEmpty(&ext->IoRequestListHead)) {
        // list empty signals remove
        // note: we used to just check the counter
        USBPORT_ASSERT(ext->IoRequestCount == -1);
        removeEvent = &ext->IoRequestRemoveEvent;
    }

    if (UsbIoRequest->FreeContext) {
        UNSIG(UsbIoRequest);
        ExFreePool(UsbIoRequest);
    } else if (UsbIoRequest != &ext->IoRequestContext) {
        UNSIG(UsbIoRequest);
        // zero it out
        RtlZeroMemory(UsbIoRequest, sizeof(struct _USB_IOREQUEST_CONTEXT));
    }

    ioc = ext->IoRequestCount;

    KeReleaseSpinLock(&ext->IoRequestSpin, irql);

    if (removeEvent) {
        // device object may be deleted here
        KeSetEvent(removeEvent, 1, FALSE);
    }

    return ioc;
}


NTSTATUS
USBPORT_Core_InitIoRequestCount(
    __inout PDEVICE_OBJECT DevObj
    )
/*++

Routine Description:

    Increment outstanding IO count on the given device object.  This routine provides remlock type behavior for both
    the FDO and Root Hub PDO.


--*/
{
    PDEVICE_EXTENSION ext;

    ext = DevObj->DeviceExtension;

    ext->IoRequestCount = -1;
    ext->IoRequestAsyncCallbackCount = -1;

    KeInitializeSpinLock(&ext->IoRequestSpin);
    InitializeListHead(&ext->IoRequestListHead);

    KeInitializeEvent(&ext->IoRequestStopEvent,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    KeInitializeEvent(&ext->IoRequestRemoveEvent,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    KeInitializeEvent(&ext->IoRequestAsyncCallbackEvent,
                      NotificationEvent,
                      USBD_EVENT_STATE_SIGNALED);

    return STATUS_SUCCESS;
}


VOID
USBPORT_Core_WaitPendingTransfers(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Waits for outstanding transfers to flush off the hardware. It checks the status of the Priority queue as well
    as the done queue.

    *This routine should be called when the hc is in a state where it cannot receive any more transfers (
    eg suspended or removed).



--*/
{
    PDEVICE_EXTENSION devExt;
    PVOID eventObjects[2];

    devExt = GetHcFdoExt(HcFdo);

    ASSERT_PASSIVE();

    eventObjects[0] = &devExt->Fdo.TxDoneListEmpty_Event;
    eventObjects[1] = &devExt->Fdo.TxPriorityListEmpty_Event;

    // wait for queues to drain

    KeWaitForMultipleObjects(2, eventObjects, WaitAll, Executive, KernelMode, FALSE, NULL, NULL);
}


ULONG
USBPORT_Core_GetEndpointTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Counts the number of entries on the global transfer list associated with a given endpoint.

    Returns the outstanding transfer io count for an endpoint.

    This routine will scan both the done queue and the priority queue.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;
    PHCD_ENDPOINT txEp;
    ULONG txCount = 0;
    KIRQL irql;

    // walk the priority list
    devExt = GetHcFdoExt(HcFdo);

    irql = USBPORT_TxAcquireLock(HcFdo, NULL);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'epc0', 0, 0, Endpoint);

    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'epc2', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        le = le->Flink;
        txEp = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
        if (txEp == Endpoint) {
            txCount++;
        }
    }

    le = devExt->Fdo.TxDoneListHead.Flink;

    while (le != &devExt->Fdo.TxDoneListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxDoneLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'epc3', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        le = le->Flink;
        txEp = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
        if (txEp == Endpoint) {
            txCount++;
        }

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'epc4', 0, 0, txCount);
    USBPORT_TxReleaseLock(HcFdo, NULL, irql);

    return txCount;
}


ULONG
USBPORT_Core_iGetEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Counts the number of entries on the endpoints pending list.  The is the number of transfers currently queued to
    hardware for a given endpoint.



--*/
{
    PLIST_ENTRY le;
    ULONG txCount = 0;
    PHCD_TRANSFER_CONTEXT transfer;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'ehc0', txCount, 0, Endpoint);

    le = Endpoint->HwPendingListHead.Flink;

    while (le != &Endpoint->HwPendingListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxEndpointLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'ehc1', txCount, transfer, Endpoint);

        ASSERT_TRANSFER(transfer);
        le = le->Flink;
        txCount++;
    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'ehc3', txCount, 0, Endpoint);

    return txCount;
}


VOID
USBPORT_Core_iIncEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Insert the transfer request on the pending request list, this transfer now counts againts maxtransfer for the
    endpoint.

--*/
{
    USBPORT_iAssertEndpoint(HcFdo, Endpoint);
    ASSERT_TRANSFER(Transfer);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'incE', Transfer, 0, Endpoint);
    USB_ASSERT_UNLINKED(&Transfer->TxEndpointLink);
    InsertTailList(&Endpoint->HwPendingListHead, &Transfer->TxEndpointLink);
}


VOID
USBPORT_Core_iDecEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Remove the transfer request from the enpoint pending request list, this transfer no longer counts against
    maxtransfer.

--*/
{
    LOGENTRY(HcFdo, LOG_TRANSFER, 'decE', Transfer, 0, Endpoint);
    USBPORT_iAssertEndpoint(HcFdo, Endpoint);
    ASSERT_TRANSFER(Transfer);

    USBPORT_AssertListEntryLog(HcFdo, &Transfer->TxEndpointLink);
    RemoveEntryList(&Transfer->TxEndpointLink);
    USB_ZAP_LINK(&Transfer->TxEndpointLink);
}


BOOLEAN
USBPORT_Core_iAbortEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_ABORT_PIPE_CONTEXT AbortContext
    )
/*++

Routine Description:

    Abort all transfers currently queued to an endpoint.

    All aborted transfers are insrted on the 'abort queue' for the request.  When all these transfers complete
    the assocaited abort request is completed.  This creates pseudo-synchronous behavior for abort pipe (abort
    completes after all current transfers complete). *This is not a behvaior change, all versions of
    the USB stack attempted this type of behavior.

    Notes:
    - We only permit one abort request pending at a time.
    - Transfers may be queued by the function driver after this routine is run and they will not be aborted.


Returns:

    TRUE - if the request was successfuly queued.

    STATUS_DEVICE_BUSY - if an abort request is already pending for this endpoint.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT txEp, abrEp;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;
    BOOLEAN requestQueued = TRUE;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'abi0', 0, 0, AbortContext);
    ASSERT_ABORT_CONTEXT(AbortContext);

    devExt = GetHcFdoExt(HcFdo);

    abrEp = USBPORT_EndpointFromHandle(AbortContext->EpRefHandle);
    USBPORT_iAssertEndpoint(HcFdo, abrEp);

    // walk the global transfer queue and abort all transfers for this endpoint
    le = devExt->Fdo.TxPriorityListHead.Flink;

    // populate request list with aborted trransfers
    while (le != &devExt->Fdo.TxPriorityListHead) {

        LOGENTRY(HcFdo, LOG_TRANSFER, 'abi1', le, abrEp->PendingAbortContext, 0);

        if (abrEp->PendingAbortContext) {
            requestQueued = FALSE;
            break;
        }

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);


        ASSERT_TRANSFER(transfer);
        txEp = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'abi2', transfer, txEp, 0);
        ASSERT_ENDPOINT(txEp);

        // note: split children are not added to the abort list since they will complete when
        // the parent is canceled.

        if (txEp == abrEp &&
            transfer->TxType != Tx_SPLIT_CHILD) {

            LOGENTRY(HcFdo, LOG_TRANSFER, 'abi3', AbortContext, transfer, 0);
            USB_ASSERT_UNLINKED(&transfer->TxAbortLink);
            InsertTailList(&AbortContext->TxListHead, &transfer->TxAbortLink);
            transfer->AbortContext = AbortContext;
            USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);
        }

        le = le->Flink;

    }

    // we pend the request even if no transfers are present
    if (requestQueued) {
        abrEp->PendingAbortContext = AbortContext;

        if (IsListEmpty(&AbortContext->TxListHead)) {

            // no transfers, insert directly to Done list
            LOGENTRY(HcFdo, LOG_TRANSFER, 'abi4', AbortContext, 0, 0);
            // keep track of all outstanding abort-pipe requests
            USB_ASSERT_UNLINKED(&AbortContext->AxDoneLink);
            USB_ASSERT_UNLINKED(&AbortContext->AxPendingLink);
            InsertTailList(&devExt->Fdo.AxDoneListHead, &AbortContext->AxDoneLink);
            USB_ZAP_LINK(&AbortContext->AxPendingLink);

            abrEp->PendingAbortContext = NULL;

            USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbIocDpc, NULL);

        } else {

            LOGENTRY(HcFdo, LOG_TRANSFER, 'abi6', AbortContext, 0, 0);
            // keep track of all outstanding abort-pipe requests
            USB_ASSERT_UNLINKED(&AbortContext->AxPendingLink);
            USB_ASSERT_UNLINKED(&AbortContext->AxDoneLink);
            InsertTailList(&devExt->Fdo.AxPendingListHead, &AbortContext->AxPendingLink);


        }

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'abi5', 0, 0, requestQueued);

    return requestQueued;
}


VOID
USBPORT_Core_iCheckAbortList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Scans the abort-pipe requests and removes this transfer if it attached to any request.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortContext;
    PLIST_ENTRY le;
    PHCD_ENDPOINT abrEp;

    ASSERT_TRANSFER(Transfer);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'cka0', 0, 0, Transfer);

    devExt = GetHcFdoExt(HcFdo);

    le = devExt->Fdo.AxPendingListHead.Flink;

    while (le != &devExt->Fdo.AxPendingListHead) {

        LOGENTRY(HcFdo, LOG_TRANSFER, 'cka1', le, 0, 0);

        abortContext = (PUSB_ABORT_PIPE_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _USB_ABORT_PIPE_CONTEXT,
                        AxPendingLink);

        ASSERT_ABORT_CONTEXT(abortContext);
        abrEp = USBPORT_EndpointFromHandle(abortContext->EpRefHandle);
        ASSERT_ENDPOINT(abrEp);

        if (Transfer->AbortContext == abortContext) {
            // remove it
            USBPORT_AssertListEntryLog(HcFdo, &Transfer->TxAbortLink);
            RemoveEntryList(&Transfer->TxAbortLink);
            USB_ZAP_LINK(&Transfer->TxAbortLink);

            Transfer->AbortContext = NULL;

            if (IsListEmpty(&abortContext->TxListHead)) {

                // all transfers are complete
                LOGENTRY(HcFdo, LOG_TRANSFER, 'cka2', abortContext, 0, 0);
                RemoveEntryList(&abortContext->AxPendingLink);
                USB_ZAP_LINK(&abortContext->AxPendingLink);

                // insert on done list
                USB_ASSERT_UNLINKED(&abortContext->AxDoneLink);
                InsertTailList(&devExt->Fdo.AxDoneListHead, &abortContext->AxDoneLink);

                abrEp->PendingAbortContext = NULL;

            }

            return;
        }

        LOGENTRY(HcFdo, LOG_TRANSFER, 'cka3', Transfer, 0, 0);
        // next entry
        le = le->Flink;

    }
}


VOID
USBPORT_Core_FlushCompletedAborts(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc
    )
/*++

Routine Description:

    Scans the list of pending abort requests and completes any that are done.


--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortContext;
    PLIST_ENTRY le;
    KIRQL irql;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'fla0', 0, 0, 0);

    devExt = GetHcFdoExt(HcFdo);

    irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

    while (!IsListEmpty(&devExt->Fdo.AxDoneListHead)) {
        PIRP irp;

        le = RemoveHeadList(&devExt->Fdo.AxDoneListHead);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'fla1', le, 0, 0);

        abortContext = (PUSB_ABORT_PIPE_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _USB_ABORT_PIPE_CONTEXT,
                        AxDoneLink);

        ASSERT_ABORT_CONTEXT(abortContext);
        USB_ZAP_LINK(&abortContext->AxDoneLink);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'fla2', abortContext, 0, 0);

        // sanity check
        USBPORT_ASSERT(IsListEmpty(&abortContext->TxListHead));
        USB_ASSERT_UNLINKED(&abortContext->AxPendingLink);

        // As soon as the lock is released USBPORT_AxCsqRemoveIrp() is
        // free to run if the USB_ABORT_PIPE_CONTEXT Irp has been
        // canceled, but it cannot be allowed to free the
        // USB_ABORT_PIPE_CONTEXT when it runs until after the
        // USB_ABORT_PIPE_CONTEXT CsqContext is used for the call to
        // IoCsqRemoveIrp().
        //
        // Take the USBPORT_Core_FlushCompletedAbortsRef reference on
        // the USB_ABORT_PIPE_CONTEXT to prevent
        // USBPORT_AxCsqRemoveIrp() from freeing the
        // USB_ABORT_PIPE_CONTEXT in the case that it runs to completion
        // before IoCsqRemoveIrp() is called.
        //
        abortContext->USBPORT_Core_FlushCompletedAbortsRef = 1;

        // Release the lock before calling IoCsqRemoveIrp() as that
        // will attempt to acquire the lock.
        //
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);

        if (abortContext->Irp == NULL)
        {
            // This case occurs when USBPORT_Core_SyncAbortEndpoint()
            // called USBPORT_AxQueueAbortRequest() without an Irp and
            // USBPORT_AxQueueAbortRequest() never called
            // IoCsqInsertIrpEx().
            //

            irp = NULL;
        }
        else
        {
            //
            // Save a pointer to the Abort Context in case a bugcheck
            // occurs while attemping to remove the USB_ABORT_PIPE_CONTEXT
            // Irp from the abort request cancel safe queue.
            //
            USBPORT_SetMiniDumpData(HcFdo, sizeof(*abortContext), (PUCHAR)abortContext);

            // Attempt to remove the USB_ABORT_PIPE_CONTEXT Irp from the
            // DEVICE_EXTENSION AxCsqIrpListHead list.  Note that if
            // USB_ABORT_PIPE_CONTEXT Irp has been canceled calling
            // IoCsqRemoveIrp() will not result in USBPORT_AxCsqRemoveIrp()
            // being called here, but another call to USBPORT_AxCsqRemoveIrp()
            // might currently be in progress or might have already
            // completed execution.
            //
            irp = IoCsqRemoveIrp(&devExt->AxCsq, &abortContext->CsqContext);

            //
            // Clear the pointer to the Abort Context.  If code is
            // executing at this point then a bugcheck did not occur
            // while attemping to remove the USB_ABORT_PIPE_CONTEXT Irp
            // from the abort request cancel safe queue.
            //
            USBPORT_ClearMiniDumpData(HcFdo);
        }

        if ((abortContext->Irp == NULL) || (irp != NULL))
        {
            LOGENTRY(HcFdo, LOG_TRANSFER, 'fla3', abortContext, irp, abortContext->Irp);

            // Either:

            // (1) IoCsqInsertIrpEx() was never called for this
            // USB_ABORT_PIPE_CONTEXT.
            //
            // or,
            //
            // (2) The USB_ABORT_PIPE_CONTEXT Irp was not canceled and
            // the call to IoCsqRemoveIrp() resulted in
            // USBPORT_AxCsqRemoveIrp() being called to remove the
            // USB_ABORT_PIPE_CONTEXT from the DEVICE_EXTENSION
            // AxCsqIrpListHead list.
            //
            // The USB_ABORT_PIPE_CONTEXT Irp can be completed and the
            // USB_ABORT_PIPE_CONTEXT can be freed at this point without
            // any further synchronization concerns.
            //
            USBPORT_Core_CompleteAbortRequest(HcFdo, abortContext, STATUS_SUCCESS);

            // Acquire the lock again before continuing the while loop.
            //
            irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);
        }
        else
        {
            // The USB_ABORT_PIPE_CONTEXT Irp was canceled and the
            // call to IoCsqRemoveIrp() did not result in
            // USBPORT_AxCsqRemoveIrp() being called to remove the
            // USB_ABORT_PIPE_CONTEXT from the DEVICE_EXTENSION
            // AxCsqIrpListHead list.
            //
            irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

            // At this point the USBPORT_Core_FlushCompletedAbortsRef
            // reference on the USB_ABORT_PIPE_CONTEXT can be released.
            //
            abortContext->USBPORT_Core_FlushCompletedAbortsRef = 0;

            if (abortContext->USBPORT_AxCsqRemoveIrpRef == 0)
            {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'fla4', abortContext, 0, 0);

                // If USBPORT_AxCsqRemoveIrp() has run to completion and
                // released the USBPORT_AxCsqRemoveIrpRef reference on
                // the USB_ABORT_PIPE_CONTEXT then the
                // USB_ABORT_PIPE_CONTEXT Irp can be completed and the
                // USB_ABORT_PIPE_CONTEXT can be freed at this point
                // without any further synchronization concerns.
                //
                USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);

                USBPORT_Core_CompleteAbortRequest(HcFdo, abortContext, STATUS_SUCCESS);

                // Acquire the lock again before continuing the while loop.
                //
                irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);
            }
            else
            {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'fla5', abortContext, 0, 0);

                // Otherwise when USBPORT_AxCsqRemoveIrp() executes it
                // will see that the USBPORT_Core_FlushCompletedAbortsRef
                // reference on the USB_ABORT_PIPE_CONTEXT has been
                // released and it will complete the
                // USB_ABORT_PIPE_CONTEXT Irp and free the
                // USB_ABORT_PIPE_CONTEXT.
                //
            }
        }
    }

    USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
}


VOID
USBPORT_Core_CompleteAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __in    PUSB_ABORT_PIPE_CONTEXT AbortContext,
    __in    LONG NtStatus
    )
/*++

Routine Description:

    Complete an abort-pipe request

--*/
{
    PDEVICE_EXTENSION   devExt;
    PHCD_ENDPOINT       abrEp;
    PIRP                irp = NULL;

    devExt = GetHcFdoExt(HcFdo);
    ASSERT_ABORT_CONTEXT(AbortContext);

    abrEp = USBPORT_EndpointFromHandle(AbortContext->EpRefHandle);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'cab3', abrEp, 0, AbortContext);
    ASSERT_ENDPOINT(abrEp);

    if (AbortContext->Irp) {
        irp = AbortContext->Irp;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'cab0', NtStatus, irp, AbortContext);
        USBPORT_ASSERT(AbortContext->Urb);
        USBPORT_ASSERT(AbortContext->DeviceHandle);

        irp->IoStatus.Status = NtStatus;
        irp->IoStatus.Information = 0;
    }

    // if we have an irp we should also have a URB.
    // set urb status code here
    if (AbortContext->Urb) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'cab1', NtStatus, irp, AbortContext);

        USBPORT_ASSERT(AbortContext->Irp);
        USBPORT_ASSERT(AbortContext->DeviceHandle);

        if (NT_SUCCESS(NtStatus)) {
            SET_USBD_ERROR(AbortContext->Urb, USBD_STATUS_SUCCESS);
        } else {
            SET_USBD_ERROR(AbortContext->Urb, USBD_STATUS_DEVICE_GONE);
        }

        USBPORT_EtwWriteHcDeviceEndpointData3(
            HcFdo,
            AbortContext->DeviceHandle,
            abrEp,
            &USBPORT_ETW_EVENT_COMPLETE_URB_FUNCTION_ABORT_PIPE,
            NULL,

            &AbortContext->Irp,
            sizeof(PIRP),

            &AbortContext->Urb,
            sizeof(PURB),

            AbortContext->Urb,
            sizeof(struct _URB_PIPE_REQUEST));
    }

    if (AbortContext->DeviceHandle) {
        USBPORT_ASSERT(AbortContext->Irp);
        USBPORT_ASSERT(AbortContext->Urb);

         // an irp based request will have a ref on the devhandle as well
        USBPORT_DereferenceDeviceHandle(HcFdo, AbortContext->DeviceHandle, AbortContext->Urb, DEVH_URB_TAG, TRUE);
    }

    if (AbortContext->Event) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'cab2', 0, 0, AbortContext);
        KeSetEvent(AbortContext->Event, IO_NO_INCREMENT, FALSE);
        AbortContext->Event = NULL;
    }

    USBPORT_DereferenceEndpoint(HcFdo, AbortContext->EpRefHandle);

    if (irp) {
        KIRQL  oldIrql;
        PUSB_IOREQUEST_CONTEXT usbIoRequest;
        PDEVICE_OBJECT pdoDeviceObject;

        // if there is an irp then there is a ref on the rh pdo
        pdoDeviceObject = devExt->Fdo.RhPdoPtr;

        usbIoRequest = USBPORT_Core_GetIoRequestFromObject(pdoDeviceObject, irp);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'cab4', 0, 0, usbIoRequest);
        ASSERT_USB_IOREQUEST(usbIoRequest);

        //
        // Save a pointer to the Abort Context in case a bugcheck occurs
        // while attemping to complete the USB_ABORT_PIPE_CONTEXT Irp
        // back to the client driver.
        //
        USBPORT_SetMiniDumpData(HcFdo, sizeof(*AbortContext), (PUCHAR)AbortContext);

        KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

        //
        // Now complete the USB_ABORT_PIPE_CONTEXT Irp back to the
        // client driver.
        //
        IoCompleteRequest(irp,
                          IO_NO_INCREMENT);

        KeLowerIrql(oldIrql);

        //
        // Clear the pointer to the Abort Context.  If code is executing
        // at this point then a bugcheck did not occur while attemping
        // to complete the USB_ABORT_PIPE_CONTEXT Irp back to the
        // client driver.
        //
        USBPORT_ClearMiniDumpData(HcFdo);

        // release the PDO
        USBPORT_Core_FreeIoRequest(usbIoRequest);
    }

    //
    // Done with the USB_ABORT_PIPE_CONTEXT now.  Free it.
    //
    UsbFreePool(AbortContext);
    AbortContext = NULL;
}


PUSB_ABORT_PIPE_CONTEXT
USBPORT_Core_AllocateAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PUSBD_DEVICE_HANDLE DevHandle,
    __inout_opt PURB Urb,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT AbortEvent
    )
/*++

Routine Description:

    Allocates an bort request structure for use with abort-endpoint.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_ABORT_PIPE_CONTEXT abortContext;
    NTSTATUS nts;
    PUSBOBJ_REF eph;
    ULONG i;

    devExt = GetHcFdoExt(HcFdo);

    do {
        nts = UsbAllocPool_Z(&abortContext, NonPagedPoolNx, sizeof(struct _USB_ABORT_PIPE_CONTEXT));

        if (abortContext == NULL) {
            break;
        }

        if (NT_ERROR(nts)) {
            UsbFreePool( abortContext);
            abortContext = NULL;
            break;
        }

        nts = USBPORT_ReferenceEndpoint(HcFdo, Endpoint, &eph, abortContext, EPREF_SYNC_ABORT_CTX_TAG);

        if (NT_ERROR(nts)) {
            UsbFreePool( abortContext);
            abortContext = NULL;
            break;
        }

        abortContext->EpRefHandle = eph;
        abortContext->Endpoint = Endpoint;
        abortContext->Irp = Irp;
        abortContext->Urb = Urb;
        abortContext->DeviceHandle = DevHandle;
        abortContext->Event = AbortEvent;
        abortContext->Sig = SIG_ABORT_PIPE_CONTEXT;

        InitializeListHead(&abortContext->TxListHead);
        USB_ZAP_LINK(&abortContext->AxCsqIrpLink);
        USB_ZAP_LINK(&abortContext->AxPendingLink);
        USB_ZAP_LINK(&abortContext->AxDoneLink);
        USB_ZAP_LINK(&abortContext->AxCanceledLink);

        abortContext->Thread = KeGetCurrentThread();

        abortContext->USBPORT_AxCsqRemoveIrpRef = 0;
        abortContext->USBPORT_Core_FlushCompletedAbortsRef = 0;

        // Fields for minidump debugging.
        //
        abortContext->PciVendorId = devExt->Fdo.PciVendorId;
        abortContext->PciDeviceId = devExt->Fdo.PciDeviceId;

        if (DevHandle == NULL) {

            abortContext->DeviceVID = 0xFFFF;
            abortContext->DevicePID = 0xFFFF;

            for (i = 0; i < USB_DRIVER_NAME_LEN; i++) {
                abortContext->DriverName[i] = '?';
            }

        } else {

            abortContext->DeviceVID = DevHandle->DeviceDescriptor.idVendor;
            abortContext->DevicePID = DevHandle->DeviceDescriptor.idProduct;

            for (i = 0; i < USB_DRIVER_NAME_LEN; i++) {
                abortContext->DriverName[i] = DevHandle->DriverName[i];
            }
        }

    } WHILE (0);

    return abortContext;
}


VOID
USBPORT_Core_SyncAbortEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Synchronously aborts all requests for an endpoint.

    NOTE: Endpoint should be ref'ed before calling this routine.

--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS nts;
    KEVENT abortEvent;
    PUSBOBJ_REF epRefHandle;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, HcFdo);

    if (!TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {
        //
        // After USBPORT_StopDevice() is called nothing else can
        // complete, including Abort requests because
        // USBPORT_StopDevice() disables the UsbIocDpc which needs to
        // run to call USBPORT_Core_FlushCompletedAborts() to complete
        // Abort requests.
        //
        return;
    }

    KeInitializeEvent(&abortEvent,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    nts = USBPORT_ReferenceEndpoint(HcFdo, Endpoint, &epRefHandle,
                    &abortEvent, EPREF_SYNC_ABORT_PIPE_TAG);


    if (NT_SUCCESS(nts)) {
        BOOLEAN requestQueued;
        // internal, only refs the endpoint
        requestQueued = USBPORT_AxQueueAbortRequest(HcFdo, Endpoint, NULL, NULL, NULL, &abortEvent);

        if (requestQueued) {
            // event will be signalled when req completes
            KeWaitForSingleObject(&abortEvent,
                           Suspended,
                           KernelMode,
                           FALSE,
                           NULL);

        }
        USBPORT_DereferenceEndpoint(HcFdo, epRefHandle);
    }
}


VOID
USBPORT_Core_DetectActiveUrb(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp,
    __inout PURB Urb
    )
/*++

Routine Description:

    This routine is used to detect a common error case we have seen with
    USB function drivers.  The error occurs when the same URB pointer is
    attached to two different IRPs.  If detected it means that the
    transfer URB may have been corrupted or reused by the function
    driver while in our pending queue.

    We scan the transfer list for the given URB.  If detected we raise
    USB bugcheck 0xFE.  This problem is the cause for a number of USB
    related OCA crashes. This case is detected by WinXP, in Win7 this
    routine will raise the bugcheck earlier with function driver still
    on the call stack.

    <TBD - note>
    It may be possible to attempt to error-complete the transfer URB/irp
    without accessing it along with the current request.  It is not
    clear what effect this may have.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer = NULL;
    PLIST_ENTRY le;
    KIRQL irql;

    // Scan the priority queue for this IRP and URB
    devExt = GetHcFdoExt(HcFdo);

    irql = USBPORT_TxAcquireLock(HcFdo, NULL);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'Bfe0', 0, Urb, Irp);

    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Bfe2', le, transfer->Irp, transfer);
        ASSERT_TRANSFER(transfer);

        if (transfer->Irp && transfer->Irp == Irp) {
            //
            // This is the Irp double submit case, the Irp is currently
            // pending.  The system will bugcheck no matter what.  If we
            // do it here at least the caller is still on the stack.
            //
            #pragma prefast(suppress: 28159, "Fatal client driver corruption bug requires a bugcheck")
            BUGCHECK_FDO(HcFdo,
                         USBBUGCODE_IRP_DOUBLE_SUBMIT,  // BugCode
                         (ULONG_PTR) Irp,               // BugCheckParameter2
                         (ULONG_PTR) Urb,               // BugCheckParameter3
                         (ULONG_PTR) transfer,          // BugCheckParameter4
                         transfer->TotalLength,         // DataLength
                         (PUCHAR) transfer);            // DataBuffer

        } else if (transfer->Urb && (PVOID)transfer->Urb == (PVOID)Urb) {
            //
            // This is the case where the function driver recycled the
            // Urb before we completed the request that it was attached
            // to.  The system will bugcheck no matter what.  If we do
            // it here at least the caller is still on the stack.
            //
            #pragma prefast(suppress: 28159, "Fatal client driver corruption bug requires a bugcheck")
            BUGCHECK_FDO(HcFdo,
                         USBBUGCODE_BAD_URB,            // BugCode
                         (ULONG_PTR) transfer->Irp,     // BugCheckParameter2
                         (ULONG_PTR) Irp,               // BugCheckParameter3
                         (ULONG_PTR) Urb,               // BugCheckParameter4
                         transfer->TotalLength,         // DataLength
                         (PUCHAR) transfer);            // DataBuffer
        }

        le = le->Flink;
        transfer = NULL;
    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'bfe4', 0, Irp, transfer);
    USBPORT_TxReleaseLock(HcFdo, NULL, irql);

    return;
}

typedef VOID
    (__stdcall *PENDPOINT_WORKER_ROUTINE) (
        __inout PDEVICE_OBJECT,
        __inout PHCD_ENDPOINT
    );


VOID
USBPORT_Core_WalkEndpointList(
    __inout PDEVICE_OBJECT HcFdo,
    __in PENDPOINT_WORKER_ROUTINE EpWorker
    )
/*++

Routine Description:

    This routine will walk the global list of endpoints and and invoke the callbak function passed in.  The callback
    function is used to tweak flags on the endpoint.

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY listEntry;
    PHCD_ENDPOINT endpoint;
    LARGE_INTEGER tc_begin, tc_end, tc;
    ULONG timerIncrement;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_AcquireEpListLock(HcFdo, &irql);

    // log the tick count for this routine in case the k-watchdog fires
    timerIncrement = KeQueryTimeIncrement();
    KeQueryTickCount(&tc_begin);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'epw0', tc_begin.HighPart, tc_begin.LowPart, timerIncrement);

    listEntry = devExt->Fdo.GlobalEndpointList.Flink;

    while (listEntry != &(devExt->Fdo.GlobalEndpointList)) {

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                listEntry,
                struct _HCD_ENDPOINT,
                GlobalLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'epw1', endpoint, EpWorker, 0);
        ASSERT_ENDPOINT(endpoint);

        EpWorker(HcFdo, endpoint);

        KeQueryTickCount(&tc);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'epw2', endpoint, tc.HighPart, tc.LowPart);


        listEntry = endpoint->GlobalLink.Flink;

    }

    KeQueryTickCount(&tc_end);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'epw3', tc_end.HighPart, tc_end.LowPart, 0);
    USBPORT_ReleaseEpListLock(HcFdo, irql);
}


VOID
USBPORT_Core_iZapEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    if (!Endpoint->EpFlags.RootHub) {

         USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Ev_gEp_Zap);

    }
}


VOID
USBPORT_Core_ZapAllEndpoints(
    __inout PDEVICE_OBJECT HcFdo
    )

/*++

Routine Description:

    Resets the endpoints in preparation for controller reset.

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'zapE', 0, 0, 0);
    // attempt to halt the controller first
    MPf_HaltController(devExt);

    USBPORT_Core_WalkEndpointList(HcFdo, USBPORT_Core_iZapEndpoint);
}

/*++

Routine Description:

    This routine will will re-sync the enpoint state with the miniport following sys power management.


--*/
VOID
USBPORT_Core_iResumeEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    if (!Endpoint->EpFlags.RootHub &&
        !Endpoint->EpFlags.DeviceGone) {

        USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Ev_gEp_PM_ReqResume);

        USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Ev_gEp_ReqActive);
    }
}


VOID
USBPORT_Core_ResumeAllEndpoints(
    PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    This routine will walk the global list of endpoints and clear the HC suspend flag allowing the endoints to
    transition hardware state thru the miniport again.  This routine is called on return from low power.

--*/
{
    LOGENTRY(HcFdo, LOG_TRANSFER, 'rsmE', 0, 0, 0);
    USBPORT_Core_WalkEndpointList(HcFdo, USBPORT_Core_iResumeEndpoint);
}

VOID
USBPORT_Core_TimeoutAllTransfers(
    PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Called from USBPORT_DM_IoTimerDpc().  Processes timeouts for all
    transfers currently on the TxPriorityListHead.

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PLIST_ENTRY le;
    BOOLEAN expired;
    BOOLEAN controllerGone = FALSE;
    ULONG asyncCount = 0;
    ULONG periodicCount = 0;
    KIRQL irql;

    irql = USBPORT_TxAcquireLock(HcFdo, NULL);
    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Tmt0', 0, 0, 0);

    if (devExt->Fdo.Flags.ControllerGone) {
        controllerGone = TRUE;
    }

    le = devExt->Fdo.TxPriorityListHead.Flink;

    while (le != &devExt->Fdo.TxPriorityListHead) {

        transfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'Tmt2', le, 0, transfer);
        ASSERT_TRANSFER(transfer);

        // update counters
        if (transfer->EpTransferType == Bulk ||
            transfer->EpTransferType == Control) {
            asyncCount++;
        }

        if (transfer->EpTransferType == Interrupt ||
            transfer->EpTransferType == Isochronous) {
            periodicCount++;
        }

        if (!TEST_FLAG(transfer->Flags, USBPORT_TXFLAG_TIMEOUT)) {

            PHCD_ENDPOINT endpoint;

            endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);

            switch (transfer->TxState) {
                case TX_Mapped_inMp:
                    expired = USBPORT_Core_CheckTimeout(HcFdo, &transfer->Timeout);
                    if ((expired || controllerGone || endpoint->EpFlags.HcReset) &&
                        transfer->TxType != Tx_SPLIT_CHILD) {
                        SET_FLAG(transfer->Flags, USBPORT_TXFLAG_TIMEOUT);
                        USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);

                        LOGENTRY(HcFdo, LOG_TRANSFER, 'Tmt3', &transfer->Timeout, 0, transfer);
                    }
                    break;

                case TX_Mapped:
                case TX_Mapped_Split:
                    if ((controllerGone || endpoint->EpFlags.HcReset) &&
                        transfer->TxType != Tx_SPLIT_CHILD) {
                        SET_FLAG(transfer->Flags, USBPORT_TXFLAG_TIMEOUT);
                        USBPORT_Core_iCancelPriorityTransfer(HcFdo, transfer);

                        LOGENTRY(HcFdo, LOG_TRANSFER, 'Tmt4', 0, 0, transfer);
                    }
                    break;

                default:
                    break;

            }
        }

        // USBPORT_Core_iCancelPriorityTransfer() may cause subsequent
        // transfers to be removed from the TxPriorityListHead, but it
        // should not cause the current transfer to complete all the way
        // to the point of being freed and removed from the
        // TxPriorityListHead.
        //
        // USBPORT_Core_iRemovePriorityTransfer() is only called from
        // USBPORT_Core_UsbDoneDpc_Worker() (which cannot execute while
        // the TxLock is held here) and by USBPORT_Split_iCompleteChildTransfer().
        //
        // Split children are always inserted into the
        // TxPriorityListHead after their parent and if the parent
        // transfer is in the TX_Mapped_Split state and it is canceled
        // all of its split children should be automatically canceled
        // and no longer in the TX_Mapped or TX_Mapped_inMp state after
        // the parent transfer is canceled.
        //
        // So the current transfer list entry should always still be
        // valid here even if the transfer was canceled.
        //
        le = le->Flink;
    }

    USBPORT_ASSERT(asyncCount == devExt->Fdo.TxPriorityListAsyncCount);
    USBPORT_ASSERT(periodicCount == devExt->Fdo.TxPriorityListPeriodicCount);

    USBPORT_TxReleaseLock(HcFdo, NULL, irql);
}


VOID
USBPORT_Core_RecordOcaTransferData(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Record some data on we can use for crash analysis in a minidump. If NULL is passed in for the Transfer the current
    data pointer is released

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    if (Transfer) {

        //HC info
        Transfer->HcFlavor = devExt->Fdo.HcFlavor;
        Transfer->PciVendorId = devExt->Fdo.PciVendorId;
        Transfer->PciDeviceId = devExt->Fdo.PciDeviceId;
        Transfer->PciRevisionId = devExt->Fdo.PciRevisionId;

        USBPORT_SetMiniDumpData(HcFdo, Transfer->TotalLength, (PUCHAR)Transfer);

    } else {
        USBPORT_ClearMiniDumpData(HcFdo);
    }
}


VOID
USBPORT_Core_FlushController(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Flush all active tranfers off the hardware before going in to low power or removing the controller.

    This routine marks all active transfers as aborted and completes them.  All the endpoints are advanced to the paused
    state with the SUSPEND guard flag set -- this prevents the endpoints from being activated after the controller has
    been placed in suspend.

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY listEntry;
    PHCD_ENDPOINT endpoint;
    ULONG count;
    LIST_ENTRY pauseAndAbortEpList;
    PUSBOBJ_REF eph;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    // build a temporary list of referenced endpoints
    USBPORT_AcquireEpListLock(HcFdo, &irql);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf1', 0, 0, 0);

    InitializeListHead(&pauseAndAbortEpList);

    listEntry = devExt->Fdo.GlobalEndpointList.Flink;
    LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf>', 0, listEntry, &(devExt->Fdo.GlobalEndpointList));

    while (listEntry != &(devExt->Fdo.GlobalEndpointList)) {
        NTSTATUS ref_nts;

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                listEntry,
                struct _HCD_ENDPOINT,
                GlobalLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf2', endpoint, 0, 0);
        USBPORT_iAssertEndpoint(HcFdo, endpoint);

        USBPORT_ASSERT(endpoint->FlushHcLink.Flink == NULL);
        USBPORT_ASSERT(endpoint->FlushHcLink.Blink == NULL);

        // Reference the endpoint before adding it to the pause and
        // abort endpoint list.
        //
        ref_nts = USBPORT_iReferenceEndpoint(HcFdo, endpoint, &endpoint->EpFlushHcHandle,
                                             endpoint, EPREF_FLUSH_HC_TAG);

        if (NT_SUCCESS(ref_nts)) {

            if (endpoint->EpFlags.RootHub) {

                // Skip pausing and aborting root hub endpoints.  Just
                // release the reference now without adding the endpoint
                // to the pause and abort endpoint list.
                //
                eph =  endpoint->EpFlushHcHandle;
                endpoint->EpFlushHcHandle = NULL;
                USBPORT_iDereferenceEndpoint(HcFdo, eph);

            } else {

                // Aborted transfers on full-speed and low-speed device
                // async endpoints downstream of high-speed TT hubs are
                // blocked from completing until the completion of a
                // Clear TT Buffer request to the high-speed TT hub
                // control endpoint.
                //
                // Add high-speed device endpoints to the tail of the
                // pause and abort endpoint list and add full-speed and
                // low-speed device endpoints to the head of the list to
                // ensure that high-speed TT hub control endpoints are
                // not paused before the completion of Clear TT Buffer
                // requests issued as a result of aborting transfers on
                // full-speed and low-speed device async endpoint.
                //
                if (endpoint->Parameters.DeviceSpeed == HighSpeed) {
                    InsertTailList(&pauseAndAbortEpList, &endpoint->FlushHcLink);
                } else {
                    InsertHeadList(&pauseAndAbortEpList, &endpoint->FlushHcLink);
                }
            }
        }

        listEntry = endpoint->GlobalLink.Flink;
    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf<', 0, 0, 0);

    USBPORT_ReleaseEpListLock(HcFdo, irql);

    // pause endpoints and complete any active transfers
    count = 0;
    while (!IsListEmpty(&pauseAndAbortEpList)) {
        HC_GLOBAL_EP_STATE geps;

        USBPORT_AcquireEpListLock(HcFdo, &irql);
        listEntry = RemoveHeadList(&pauseAndAbortEpList);
        USBPORT_ReleaseEpListLock(HcFdo, irql);

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                listEntry,
                struct _HCD_ENDPOINT,
                FlushHcLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf3', endpoint, 0, 0);
        ASSERT_ENDPOINT(endpoint);

        // pause this endpoint and wait
        geps = USBPORT_SetGlobalEndpointStateAndWait(HcFdo, endpoint, Ev_gEp_ReqPause, ENDPOINT_PAUSE);

        if (geps != GEp_Removed) {
            // skip this if the endpoint is already removed
            // mark any active transfers as aborted and attempt to complete them
            USBPORT_Core_SyncAbortEndpoint(HcFdo, endpoint);

            LOGENTRY(HcFdo, LOG_TRANSFER, 'hcf4', endpoint, 0, 0);
        }

        USBPORT_AcquireEpListLock(HcFdo, &irql);

        endpoint->FlushHcLink.Flink = endpoint->FlushHcLink.Blink = NULL;
        eph =  endpoint->EpFlushHcHandle;
        endpoint->EpFlushHcHandle = NULL;
        USBPORT_iDereferenceEndpoint(HcFdo, eph);

        USBPORT_ReleaseEpListLock(HcFdo, irql);
    }
}
