/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    xsplit.c

Abstract:

    splits a DMA transfer into multiple smallest transfers
    that the miniport can handle.

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

ULONG
USBPORT_MakeSplitTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSFER_SG_LIST SgList,
    PHCD_TRANSFER_CONTEXT SplitTransfer,
    ULONG MaxTransferLength,
    ULONG MaxPacketLength,
    PULONG Idx,
    PULONG Offset,
    ULONG BytesToMap,
    ULONG LengthMapped
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    ULONG length = 0;
    ULONG maxSplitLen = 0;
    PTRANSFER_SG_LIST splitSgList = NULL;
    PTRANSFER_SG_ENTRY sgEntry = NULL;
    PTRANSFER_SG_ENTRY splitSgEntry = NULL;
    ULONG startOffset;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'splt',
             SplitTransfer,
             *Idx,
             *Offset);

    USBPORT_ASSERT(0 != BytesToMap);
    USBPORT_ASSERT(0 == (MaxTransferLength % MaxPacketLength));

    // init vars
    //
    sgEntry = &SgList->SgEntry[*Idx];

    splitSgList = &SplitTransfer->SgList;
    splitSgList->SgCount = 0;
    splitSgList->MdlVirtualAddress =
        SgList->MdlVirtualAddress + LengthMapped;
    splitSgList->MdlSystemAddress =
        SgList->MdlSystemAddress + LengthMapped;

    startOffset = 0;

    splitSgEntry = &splitSgList->SgEntry[0];
    splitSgEntry->StartOffset = startOffset;

    SplitTransfer->Tp.TransferBufferLength = 0;
    SplitTransfer->Tp.MiniportFlags = MPTX_SPLIT_TRANSFER;
    USBPORT_ASSERT(SplitTransfer->TxType == Tx_SPLIT_CHILD);

    maxSplitLen = MaxTransferLength;

    while (maxSplitLen && BytesToMap) {
        BOOLEAN nextEntry = FALSE;

        if ((sgEntry->Length - *Offset) > maxSplitLen) {
            length = maxSplitLen;
        }
        else {
            length = sgEntry->Length - *Offset;
            nextEntry = TRUE;
        }

        if (length > BytesToMap) {
            length = BytesToMap;
            nextEntry = FALSE;
        }

        splitSgList->SgCount += 1;
        splitSgEntry->LogicalAddress.Hw64 =
            sgEntry->LogicalAddress.Hw64;
        splitSgEntry->LogicalAddress.Hw32 += *Offset;
        splitSgEntry->SystemAddress =
            sgEntry->SystemAddress + *Offset;
        splitSgEntry->Length = length;
        splitSgEntry->StartOffset = startOffset;

        SplitTransfer->Tp.TransferBufferLength += length;

        *Offset += length;
        startOffset += length;
        BytesToMap -= length;

        maxSplitLen -= length;

        splitSgEntry++;

        if (TRUE == nextEntry) {
            (*Idx)++;
            *Offset = 0;
            sgEntry++;
        }
    }

    return BytesToMap;
}


NTSTATUS
USBPORT_Split_iBulkInterruptTransfer(
    PDEVICE_OBJECT HcFdo,
    PXDPC_CONTEXT Xdpc,
    PHCD_ENDPOINT Endpoint,
    PHCD_TRANSFER_CONTEXT Transfer,
    PLIST_ENTRY TransferList,
    ULONG MaxTransferLength
    )
/*++

Routine Description:

    Split a bulk or interrupt transfer

Arguments:

Return Value:

    nt status code

--*/
{
    ULONG maxPacketLength;
    PHCD_TRANSFER_CONTEXT splitTransfer = NULL;
    PTRANSFER_SG_LIST sgList;
    LIST_ENTRY tmpList;
    ULONG idx, i, offset, bytesToMap, lengthMapped;
    ULONG numberOfSplits, splitIdx;
    PLIST_ENTRY listEntry, tail_le;

    sgList = &Transfer->SgList;
    maxPacketLength = EP_MAX_PACKET(Endpoint);
    USBPORT_ASSERT(EP_MAX_TRANSFER(Endpoint) >= maxPacketLength);
    // round to the smallest multiple of max packet size
    MaxTransferLength =
        (MaxTransferLength/maxPacketLength) * maxPacketLength;

    // some notes:

    //
    //
    // The MAXTRANSFER is equal to USB_PAGE_SIZE (4k) and
    // the transfer sg list is broken into usb pages.  In this
    // case we construct a transfer structure for each pair of
    // sg entries we round the length of the first sg entry down
    // to the highest multiple of MAXPACKET so we get a split
    // pattern like this:
    //
    //          {Sg1}{.Sg2.}{.Sg3.}{.Sg4.}     sg entries
    //       |------|------|------|------|     page breaks
    //          |----------------------|       original transfer
    //          <...><>
    //                 <...><>
    //                        <...><><.>
    //          {1    }{2    }{3    }{4}       splits
    //

    // The MAXTRANSFER is less than USB_PAGE_SIZE (4k) and the
    // transfer sg list is broken into usb pages.
    //
    // the pattern will look like this:
    //
    //        {...Sg1......}{......Sg2......}{......Sg3......}
    //    |----------------|----------------|----------------|
    //        |-----------------------------------|
    //        <..>
    //            <..>
    //                <..>
    //                    <><>
    //                        <..>
    //                            <..>
    //                                <..>
    //                                    <><>
    //                                        <..>
    //        {1 }{2 }{3 }{4 }{5 }{6 }{7 }{8 }{9 }

    // cases:
    // case 1 - transfer lies within the sg entry
    // case 2 - transfer overlaps the sg entry


    //
    // The MAXTRANSFER is greater than USB_PAGE_SIZE (4k)
    // ie etc, we currently don't handle this case
    //
    // Note: since the buffer is currently completely mapped
    // and locked it is better to tune the mniport to take the
    // larger transfers if possible

    // allocate the split elements

    // mark the parent transfer as a split
    Transfer->TxType = Tx_SPLIT_PARENT;
    USBPORT_TxSetState(HcFdo, Xdpc, Transfer, Ev_TX_SplitTransfer, TX_Mapped_Split);

    numberOfSplits =
        Transfer->Tp.TransferBufferLength / MaxTransferLength + 1;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'sptC',
                     numberOfSplits,
                     0,
                     0);

    InitializeListHead(&tmpList);

    for (i=0; i<numberOfSplits; i++) {

        ALLOC_POOL_Z(splitTransfer,
                     NonPagedPoolNx,
                     Transfer->TotalLength);

        if (splitTransfer == NULL) {
            goto SplitBulkInterruptTransfer_Fail;
        }

        // Only copy the core HCD_TRANSFER_CONTEXT structure from the
        // parent to the split child.  The split child transfer SgList
        // at the end of the HCD_TRANSFER_CONTEXT structure will be
        // initialized by USBPORT_MakeSplitTransfer().
        //
        RtlCopyMemory(splitTransfer,
                      Transfer,
                      sizeof(HCD_TRANSFER_CONTEXT));

        splitTransfer->TxType = Tx_SPLIT_CHILD;
        splitTransfer->LogIdx=0;

        // child starts at map pending
        splitTransfer->TxState = TX_MapPending;
        USBPORT_TxSetState(HcFdo, NULL, splitTransfer, Ev_TX_SplitTransfer, TX_Mapped);


        splitTransfer->MiniportContext = (PUCHAR) splitTransfer;
        splitTransfer->MiniportContext += splitTransfer->PrivateLength;
        InitializeListHead(&splitTransfer->DoubleBufferListHead);

        // re-init some fields
        USB_ZAP_LINK(&splitTransfer->TxDoneLink);
        USB_ZAP_LINK(&splitTransfer->TxEndpointLink);
        USB_ZAP_LINK(&splitTransfer->SplitLink);
        USB_ZAP_LINK(&splitTransfer->TmpLink);
        USB_ZAP_LINK(&splitTransfer->TxCsqIrpLink);
        USB_ZAP_LINK(&splitTransfer->TxAbortLink);
        USB_ZAP_LINK(&splitTransfer->TxPriorityLink);
        splitTransfer->AbortContext = NULL;

        InsertTailList(&tmpList,
                       &splitTransfer->TmpLink);

    }

    splitIdx = 0;
    idx = 0;
    offset = 0;
    bytesToMap = Transfer->Tp.TransferBufferLength;
    lengthMapped = 0;

    while (bytesToMap) {

        LOGENTRY(HcFdo, LOG_TRANSFER, 'sptM',
                     bytesToMap,
                     offset,
                     idx);

        listEntry = RemoveHeadList(&tmpList);

        splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        listEntry,
                        struct _HCD_TRANSFER_CONTEXT,
                        TmpLink);

        ASSERT_TRANSFER(splitTransfer);
        splitTransfer->SplitIdx = splitIdx++;

        bytesToMap = USBPORT_MakeSplitTransfer(HcFdo,
                                               sgList,
                                               splitTransfer,
                                               MaxTransferLength,
                                               maxPacketLength,
                                               &idx,
                                               &offset,
                                               bytesToMap,
                                               lengthMapped);

        lengthMapped += splitTransfer->Tp.TransferBufferLength;

        InsertTailList(TransferList,
                       &splitTransfer->TmpLink);

        InsertTailList(&Transfer->SplitTransferListHead,
                       &splitTransfer->SplitLink);

    }

    // free extra splits we did not use
    while (!IsListEmpty(&tmpList)) {

        listEntry = RemoveHeadList(&tmpList);

        splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        listEntry,
                        struct _HCD_TRANSFER_CONTEXT,
                        TmpLink);

        ASSERT_TRANSFER(splitTransfer);

        UNSIG(splitTransfer);
        UsbFreePool( splitTransfer);
    }

    tail_le = Transfer->SplitTransferListHead.Blink;
    USBPORT_ASSERT(tail_le != &Transfer->SplitTransferListHead);

    // mark the last element with a special flag
    splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        tail_le,
                        struct _HCD_TRANSFER_CONTEXT,
                        SplitLink);

    splitTransfer->Tp.MiniportFlags |= MPTX_SPLIT_TRANSFER_TAIL;

    return STATUS_SUCCESS;

SplitBulkInterruptTransfer_Fail:

    TEST_TRAP();
    // free the tmp list
    while (!IsListEmpty(&tmpList)) {

        listEntry = RemoveHeadList(&tmpList);

        splitTransfer = (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        listEntry,
                        struct _HCD_TRANSFER_CONTEXT,
                        TmpLink);

        ASSERT_TRANSFER(splitTransfer);

        UNSIG(splitTransfer);
        UsbFreePool( splitTransfer);
    }

    return STATUS_INSUFFICIENT_RESOURCES;

}


NTSTATUS
USBPORT_Split_Transfer(
    __inout PDEVICE_OBJECT FdoDeviceObject,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PLIST_ENTRY TransferList
    )
/*++

Routine Description:

    Splits a transfer into multiple transfers of the proper size
    for the miniport.

    Returns a list of transfer structures that need to be added to
    the active list.  If the transfer does not need to be split the
    list will contain only the original transfer.

--*/
{
    NTSTATUS nts = STATUS_INVALID_PARAMETER;

    InitializeListHead(TransferList);
    InitializeListHead(&Transfer->SplitTransferListHead);
    //Transfer->UsbdStatus = USBD_STATUS_SUCCESS;

    if (Transfer->Tp.TransferBufferLength <= EP_MAX_TRANSFER(Endpoint)) {
        // no split needed
        Transfer->TxType = Tx_NOT_SPLIT;
        InsertTailList(TransferList,
                       &Transfer->TmpLink);
        return STATUS_SUCCESS;
    }

    Transfer->TxType = Tx_SPLIT_PARENT;
    switch(Endpoint->Parameters.TransferType) {
    case Interrupt:
    case Bulk:
        if (EP_MAX_TRANSFER(Endpoint) <= USB_PAGE_SIZE) {
            nts = USBPORT_Split_iBulkInterruptTransfer(FdoDeviceObject,
                                               Xdpc,
                                               Endpoint,
                                               Transfer,
                                               TransferList,
                                               EP_MAX_TRANSFER(Endpoint));
        }
        else {
#define SPLIT_SIZE_FOR_EP_MAX_TFER_GT_4K       512 * 1024
            nts = USBPORT_Split_iBulkInterruptTransfer(FdoDeviceObject,
                                               Xdpc,
                                               Endpoint,
                                               Transfer,
                                               TransferList,
                                               SPLIT_SIZE_FOR_EP_MAX_TFER_GT_4K);
#undef SPLIT_SIZE_FOR_EP_MAX_TFER_GT_4K
        }
        break;
    case Control:
        // we should not hit this code path
        TEST_TRAP();
        nts = STATUS_NOT_SUPPORTED;
        break;
    case Isochronous:
        nts = STATUS_NOT_SUPPORTED;
//        BUGCHECK(USBBUGCODE_INTERNAL_ERROR, 0, 0, 0);
        break;
    }

    return nts;
}


VOID
USBPORT_Split_iCompleteChildTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT SplitChildTransfer,
    __in USBD_STATUS UsbdCompleteCode,
    __in TX_EVENT TxEvent,
    __in TX_STATE TxState
    )
/*++

Routine Description:

    Completes and frees a split child transfer, when all children are complete the parent is completed

--*/
{
    PHCD_TRANSFER_CONTEXT parentTransfer;
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'spc0', SplitChildTransfer,
        SplitChildTransfer->MiniportBytesTransferred, UsbdCompleteCode);
    ASSERT_TRANSFER(SplitChildTransfer);

    SplitChildTransfer->UsbdStatus = UsbdCompleteCode;
    // get the parent
    parentTransfer = SplitChildTransfer->ParentTransfer;

    ASSERT_TRANSFER(parentTransfer);
    USBPORT_ASSERT(SplitChildTransfer->TxType == Tx_SPLIT_CHILD);
    USBPORT_ASSERT(parentTransfer->TxType == Tx_SPLIT_PARENT);

    //
    // If the USBPORT_TXFLAG_MPCANCELED flag was set in the split child
    // transfer by USBPORT_Core_iCancelPriorityTransfer() then copy the
    // USBPORT_TXFLAG_MPCANCELED flag to the parent transfer.
    //
    if (TEST_FLAG(SplitChildTransfer->Flags, USBPORT_TXFLAG_MPCANCELED)) {
        SET_FLAG(parentTransfer->Flags, USBPORT_TXFLAG_MPCANCELED);
    }

    // set state current before checking
    USBPORT_TxSetState(HcFdo, Xdpc, SplitChildTransfer, TxEvent, TxState);
    // error ?
    //
    if (SplitChildTransfer->TxState != TX_Mapped_TerminateSplit) {

        //
        // copy the child data count to the parent transfer
        //
        if (TEST_FLAG(parentTransfer->Flags, USBPORT_TXFLAG_SPLIT_ERROR)) {
            // error should be set
            USBPORT_ASSERT(parentTransfer->UsbdStatus != USBD_STATUS_PENDING);
            USBPORT_ASSERT(parentTransfer->UsbdStatus != USBD_STATUS_SUCCESS);
            // if previous error, no data should be indicated on this child
            if (TEST_FLAG(SplitChildTransfer->Flags, USBPORT_TXFLAG_MPCANCELED) == 0 &&
                TEST_FLAG(SplitChildTransfer->Flags, USBPORT_TXFLAG_MPCOMPLETED) == 0) {
                // no data should be indicated unless this xfer was in the miniport
                USBPORT_ASSERT(SplitChildTransfer->MiniportBytesTransferred == 0);
                SplitChildTransfer->MiniportBytesTransferred = 0;
            }
        } else {
            // we set the parent transfer on the first error encountered/ in case the miniport indicates
            // subsequent splits without indicating the error

            parentTransfer->UsbdStatus = UsbdCompleteCode;
            if (UsbdCompleteCode != USBD_STATUS_SUCCESS) {
                // pick up the last error code reported
                SET_FLAG(parentTransfer->Flags, USBPORT_TXFLAG_SPLIT_ERROR);
            }
        }

        parentTransfer->MiniportBytesTransferred +=
            SplitChildTransfer->MiniportBytesTransferred;

    }

    // unlink child from priority queue here
    USBPORT_Core_iRemovePriorityTransfer(HcFdo, SplitChildTransfer);

    // remove this transfer from the split list
    LOGENTRY(HcFdo, LOG_TRANSFER, 'spc1', parentTransfer, 0, SplitChildTransfer);
    RemoveEntryList(&SplitChildTransfer->SplitLink);

    // flush any triple buffers
    USBPORT_FlushAdapterDBs(HcFdo, SplitChildTransfer);


    // free this child
    UNSIG(SplitChildTransfer);
    UsbFreePool( SplitChildTransfer);
    SplitChildTransfer = NULL;

    // is the transfer complete?
    if (IsListEmpty(&parentTransfer->SplitTransferListHead)) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'spc2', parentTransfer, 0, 0);

        // complete the parent
        USBPORT_Core_iCompleteTransfer(HcFdo,
                                       Xdpc,
                                       parentTransfer,
                                       parentTransfer->UsbdStatus,
                                       Ev_TX_CompleteSplitParent,
                                       TX_HwDone);

    } else {
        // transfer completed so map dpc should be signalled so additiosnl transfers can be called
        // down to the miniport
        USBPORT_Xdpc_iSignal(HcFdo, &devExt->Fdo.UsbMapDpc, NULL);
    }
}


VOID
USBPORT_Split_iCancelParentTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT ParentTransfer
    )
/*++

Routine Description:

    Cancels all children given a split parent

--*/
{
    PHCD_TRANSFER_CONTEXT splitChildTransfer;
    PHCD_ENDPOINT endpoint;
    PLIST_ENTRY le;

    endpoint = USBPORT_EndpointFromHandle(ParentTransfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);
    USBPORT_ASSERT(ParentTransfer->TxType == Tx_SPLIT_PARENT);

    // cancel all the children
    LOGENTRY(HcFdo, LOG_TRANSFER, 'cas0', ParentTransfer, 0, 0);

    le = ParentTransfer->SplitTransferListHead.Flink;

    while(le != &ParentTransfer->SplitTransferListHead) {

         splitChildTransfer =  (PHCD_TRANSFER_CONTEXT) CONTAINING_RECORD(
                        le,
                        struct _HCD_TRANSFER_CONTEXT,
                        SplitLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'cas1', 0, 0, splitChildTransfer);
        ASSERT_TRANSFER(splitChildTransfer);
        USBPORT_ASSERT(splitChildTransfer->TxType == Tx_SPLIT_CHILD);

        // advance before we cancel this child transfer since it may be removed
        le = le->Flink;
        // this will remove the transfer
        USBPORT_Core_iCancelPriorityTransfer(HcFdo, splitChildTransfer);

    }

}



