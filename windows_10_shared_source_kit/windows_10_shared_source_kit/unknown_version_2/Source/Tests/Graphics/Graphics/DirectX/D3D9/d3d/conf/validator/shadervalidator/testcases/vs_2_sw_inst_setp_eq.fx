INIPORT_AbortTransfer

VOID
MINIPORT_AbortTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_CONTEXT TransferContext,
    PULONG BytesTransferred
    );


Description:

    Abort a specific transfer that has been started, this will only be called if the endpoint is in the ENDPOINT_PAUSED
    state.  This call is NOT fail-able and the HW must have no reference to the transfer on return.  The miniport does
    not indicate completion on transfer by calling any USBPORT services.

    *There is one exception added near for Vista RTM. We will peremit a call to USBPORT_SVC_CompleteTransfer as long as
    its NOT the current transfer being aborted.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TxContext - Hc Transfer Context for a mpped transfer.

BytesTransferred - pointer filled in with the count of bytes successfully transferred before the endpoint was paused.

Returns:

    none.

*/
VOID
MPf_AbortTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PULONG BytesTransferred
    )
{
    KIRQL irql;

    do {


        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_AbortTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_AbortTransfer == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->CurrentAbortTransfer == NULL);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        HcFdoExt->CurrentAbortTransfer = Transfer;
        MPF(HcFdoExt).MINIPORT_AbortTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               Transfer->MiniportContext,
                               BytesTransferred);

        HcFdoExt->CurrentAbortTransfer = NULL;
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_SubmitTransfer

USB_MINIPORT_STATUS
MINIPORT_SubmitTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_PARAMETERS TransferParameters,
    PTRANSFER_CONTEXT TransferContext,
    PTRANSFER_SG_LIST TransferSGList
    );


Description:

    Request the miniport to program a USB async transfer (bulk, interrupt or control) to the hardware. If no hw
    resources are avaiable to program the transfer then the miniport should return USBMP_STATUS_BUSY and the request
    will be tried later. If the transfer is successfully queued to the HW then the miniport returns
    USBMP_STATUS_SUCCESS.  At the time this routine is called the transfer buffer has been mapped
    (ie no need to call IoMapTransfer).

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TransferParameters - USB parameters for this transfer.

TxContext - Hc Transfer Context for a mpped transfer.

TransferSGList - contains a list of physical addresses for transfer buffer.

Returns:

    USBMP_STATUS_BUSY or USBMP_STATUS_SUCCESS.

*/
USB_MINIPORT_STATUS
MPf_SubmitTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    KIRQL irql;
    USB_MINIPORT_STATUS mps;

    do {

        mps = USBMP_STATUS_FAILURE;

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SubmitTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_SubmitTransfer == NULL) {
            break;
        }

        USBPORT_ASSERT(Endpoint->GepState != GEp_Zapped);
        if (Endpoint->GepState == GEp_Zapped) {
            // miniport cannot reference zapped enpoint
            mps = USBMP_STATUS_FAILURE;
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_SubmitTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               &Transfer->Tp,
                               Transfer->MiniportContext,
                               &Transfer->SgList);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mps;
}

/*

Routine: MINIPORT_SubmitIsoTransfer

USB_MINIPORT_STATUS
MINIPORT_SubmitIsoTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_PARAMETERS TransferParameters,
    PTRANSFER_CONTEXT TransferContext,
    PMINIPORT_ISO_TRANSFER IsochTransfer
    );


Description:

    Request the miniport to program a USB isochronous transferto the hardware.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TransferParameters - USB parameters for this transfer.

TxContext - Hc Transfer Context for a mpped transfer.

IsochTransfer - contains a list of isoch packets and physical addresses for transfer.

Returns:

    USBMP_STATUS_BUSY or USBMP_STATUS_SUCCESS.

*/
USB_MINIPORT_STATUS
MPf_SubmitIsochTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    KIRQL irql;
    USB_MINIPORT_STATUS mps;

    do {

        mps = USBMP_STATUS_FAILURE;

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               &Transfer->Tp,
                               Transfer->MiniportContext,
                               Transfer->IsoTransfer);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_CreateDeviceData

VOID
MINIPORT_CreateDeviceData(
    PDEVICE_DATA HcContext
    );


Description:

    Notify miniport of pnp ADD.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.


*/
VOID
MPf_CreateDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        //USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_CreateDeviceData != NULL);

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_CreateDeviceData == NULL) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_CreateDeviceData(HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);


}


/*
Routine: MINIPORT_DeleteDeviceData

VOID
MINIPORT_DeleteDeviceData(
    PDEVICE_DATA HcContext
    );


Description:

    Notify miniport of pnp REMOVE.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.


*/
VOID
MPf_DeleteDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_DeleteDeviceData == NULL) {
            break;
        }


        MPF(HcFdoExt).MINIPORT_DeleteDeviceData(HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);


}


/*

Routine: MINIPORT_OpenDevHandle

VOID
MINIPORT_OpenDevHandle(
    PDEVICE_DATA HcContext,
    PDEVH_CONTEXT DevhContext
    );


Description:

    Notify miniport of USB function arrival.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

DevhContext - USB function driver context

*/
VOID
MPf_OpenDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    )
{
    do {


        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_OpenDevHandle == NULL) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_OpenDevHandle(HcFdoExt->Fdo.MiniportDeviceData,
                                             DevH->MiniportContext);


    } WHILE (0);


}

/*

Routine: MINIPORT_CloseDevHandle

VOID
MINIPORT_CloseDevHandle(
    PDEVICE_DATA HcContext,
    PDEVH_CONTEXT DevhContext
    );


Description:

    Notify miniport of USB function removal.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

DevhContext - USB function driver context

*/
VOID
MPf_CloseDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    )
{
    do {


        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_CloseDevHandle == NULL) {
            break;
        }


        MPF(HcFdoExt).MINIPORT_CloseDevHandle(HcFdoExt->Fdo.MiniportDeviceData,
                                             DevH->MiniportContext);


    } WHILE (0);


}

/*

Routine: MINIPORT_RebalanceEndpoint

VOID
MINIPORT_RebalanceEndpoint(
    PDEVICE_DATA HcContext,
    PHCD_ENDPOINT EpContext
    );


Description:

    Rebalance a USB 2.0 endpoint. This routine is used to notify the host controller that tha an endpoints
    schedule parameters have changed

Core_Lock - yes.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - miniport context for endpoint

*/
VOID
MPf_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql;

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_RebalanceEndpoint == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_RebalanceEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                 &Endpoint->Parameters,
                                                 &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);



    } WHILE (0);


}


/**********************************************************************************************************************
    These routines dispatches to the appropriate function (USBX or legacy) based on miniport flags
***********************************************************************************************************************/

VOID
MPx_QueryEndpointRequirements(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __out   PENDPOINT_REQUIREMENTS Requirements
    )
{
    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        // newer version with perf tweaks
        //MP_UsbxQueryEndpointRequirements(HcFdoExt, Endpoint, Requirements);
    } else {
        MPf_QueryEndpointRequirements(HcFdoExt, Endpoint, Requirements);
    }
}


USB_MINIPORT_STATUS
MPx_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        mps = MPf_UsbxOpenEndpoint(HcFdoExt, Endpoint, Endpoint->Tt, UsbdStatus);
    } else {
        mps = MPf_OpenEndpoint(HcFdoExt, Endpoint, UsbdStatus);
    }

    // if endpoint open succeeds the miniport has a reference to the endpoint that must be closed/freed
    // we keep track of this with a single flag.

    if (mps == USBMP_STATUS_SUCCESS) {
        Endpoint->MpOpenRef = TRUE;
    }

    return mps;
}


USB_MINIPORT_STATUS
MPx_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_SUCCESS;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        //mps = MPf_UsbxRebalanceEndpoint(HcFdoExt, Endpoint, Endpoint->Tt, UsbdStatus);
        mps = USBMP_STATUS_NOT_SUPPORTED;
    } else {
        MPf_RebalanceEndpoint(HcFdoExt, Endpoint);
    }

    return mps;
}


USB_MINIPORT_STATUS
MPx_PokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        mps = MPf_UsbxPokeEndpoint(HcFdoExt, Endpoint, Tt);
    } else {
        mps = MPf_PokeEndpoint(HcFdoExt, Endpoint);
    }

    return mps;
}


VOID
MPx_InitializeHsb(
    VOID
    )
{
#ifdef USB2_INTERNAL
    USB2LIB_InitializeLibEx(&USB2LIB_HcContextSize,
                            &USB2LIB_EndpointContextSize,
                            &USB2LIB_TtContextSize);
#endif
}


ULONG
MPx_MpRevision(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    ULONG rev;

    rev = HcFdoExt->Fdo.MiniportDriver->HciVersion;

    return rev;
}


ULONG
MPx_HsbTtContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbTtContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_TtContextSize;
#endif
    }

    return n;
}

ULONG
MPx_HsbControllerContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_HcContextSize;
#endif
    }

    return n;
}


ULONG
MPx_HsbEndpointContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbEpContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_EndpointContextSize;
#endif
    }

    return n;

}



USB_MINIPORT_STATUS
MPx_SubmitTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
//        mps = MPf_UsbxSubmitTransfer(HcFdoExt, Endpoint, Transfer);
        mps = USBMP_STATUS_NOT_SUPPORTED;
    } else {

        if (Endpoint->LastTransferSequenceNumberAccepted != 0) {
            //
            // Assert that the sequence number of the transfer that is
            // currently being submitted to the miniport is greater than
            // the sequence number of the last transfer accepted by the
            // miniport.
            //
            USBPORT_ASSERT(Transfer->Tp.SequenceNumber -
                           Endpoint->LastTransferSequenceNumberAccepted <
                           MAXLONG);
        }

        if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_ISO)) {
            mps = MPf_SubmitIsochTransfer(HcFdoExt, Endpoint, Transfer);
        } else {
            mps = MPf_SubmitTransfer(HcFdoExt, Endpoint, Transfer);
        }

        if (mps == USBMP_STATUS_SUCCESS) {
            //
            // Remember the sequence number of the last transfer
            // accepted by the miniport.
            //
            Endpoint->LastTransferSequenceNumberAccepted =
                Transfer->Tp.SequenceNumber;
        }
    }

    return mps;
}

/*
    Initializes an instance of a high speed endpoint for HS bugdeter.
*/
NTSTATUS
MPx_InitializeHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR TransactionTranslator
    )
{
    NTSTATUS nts;
    ULONG nBytes;

    Endpoint->HsbEpContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return STATUS_SUCCESS;
    }

    nBytes = MPx_HsbEndpointContextSize(HcFdoExt);

    if (nBytes) {
        nts = UsbAllocPool_Z(&Endpoint->HsbEpContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {

        if (USBPORT_IS_USBX(HcFdoExt)) {

            USB_MINIPORT_STATUS mps;

            TEST_TRAP();
            // callout to miniport
            if (NT_SUCCESS(nts)) {
                mps = MPf_UsbxInitHsbEndpoint(HcFdoExt, Endpoint, TransactionTranslator);

                nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
            }
        } else {

#ifdef USB2_INTERNAL

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif
        }
    }
    return nts;
}



/*
    Initializes an instance of a high speed TT for HS bugdeter.
*/
NTSTATUS
MPx_InitializeHsbTt(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR TransactionTranslator
    )
{
    NTSTATUS nts;
    ULONG nBytes;

    TransactionTranslator->HsbTtContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return STATUS_SUCCESS;
    }

    nBytes = MPx_HsbTtContextSize(HcFdoExt);

    if (nBytes) {
        nts = UsbAllocPool_Z(&TransactionTranslator->HsbTtContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {
        if (USBPORT_IS_USBX(HcFdoExt)) {
            USB_MINIPORT_STATUS mps;

            TEST_TRAP();
            mps = MPf_UsbxInitHsbTransactionTranslator(HcFdoExt, TransactionTranslator);

            nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
        } else {

#ifdef USB2_INTERNAL
            USB2LIB_InitTt(HcFdoExt->Fdo.HsbControllerContextPtr,  TransactionTranslator->HsbTtContextPtr);

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif

        }
    }
    return nts;
}

/*
    Initializes an instance of a high speed USB controller from Add Device.
*/
NTSTATUS
MPx_InitializeHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    NTSTATUS nts;
    ULONG flags;
    USB_MINIPORT_STATUS mps;
    ULONG nBytes;

//#define USB_MINIPORT_OPT_USB20                0x00000010
//#define USB_MINIPORT_OPT_USBX                 0x00000400

    HcFdoExt->Fdo.HsbControllerContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        // only allowed for USB20
        return STATUS_SUCCESS;
    }

    flags = MPF(HcFdoExt).OptionFlags & 0x00000410;

    // allocate this block dynamically
    nBytes = MPx_HsbControllerContextSize(HcFdoExt);
    if (nBytes) {
        nts = UsbAllocPool_Z(&HcFdoExt->Fdo.HsbControllerContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {
        switch(flags) {
        // USB2 only
        case 0x00000010:
#ifdef USB2_INTERNAL

            USB2LIB_InitController(HcFdoExt->Fdo.HsbControllerContextPtr);

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif
            break;

        // USBX + USB2
        case 0x00000410:

            mps = MPf_UsbxInitHsbController(HcFdoExt);

            nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
            break;

        default:
            nts = STATUS_INVALID_PARAMETER;

        }
    }
    return nts;
}

ULONG
MPx_QueryTtBandwidthData(
    PDEVICE_EXTENSION HcFdoExt,
    PTRANSACTION_TRANSLATOR Tt,
    ULONG Frame
    )
{
/*
    Returns consumed bandwidth for frame on a TT
*/

    ULONG bw = 0;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return 0;
    }

    if (USBPORT_IS_USBX(HcFdoExt)) {
        bw = MPf_UsbxQueryTtBandwidthData(HcFdoExt, Tt, Frame);
    } else {
#ifdef USB2_INTERNAL
        bw = USB2LIB_GetTt_time_used(Tt->HsbTtContextPtr, Frame);
#endif
    }


    return bw;
}


ULONG
MPx_QueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    )
{
/*
    Returns consumed bandwidth for frame, microframe on a high speed controller
*/

    ULONG bw = 0;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return 0;
    }

   if (USBPORT_IS_USBX(HcFdoExt)) {
        bw = MPf_UsbxQueryBandwidthData(HcFdoExt, Frame, Microframe);
    } else {
#ifdef USB2_INTERNAL
        bw = USB2LIB_Get_bus_time(HcFdoExt->Fdo.HsbControllerContextPtr, Frame, Microframe);
#endif
    }


    return bw;
}


ULONG
MPx_QueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
/*
    Returns consumed bandwidth for a given endpoint
*/

    ULONG bw = 0;
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    // USB11
    case 0x00000008:
        // internal (legacy mode)
        bw = USBPORT_CalculateUsbBandwidthEp(HcFdoExt->HcFdoDeviceObject, Endpoint);
        break;
    // USBX
    case 0x00000400:
    // USBX, USB20
    case 0x00000410:
    // USBX, USB11
    case 0x00000408:
        // query thru miniport
        bw = MPf_UsbxQueryEpBandwidthData(HcFdoExt, Endpoint->Tt, Endpoint);
        break;

    // USB20
    case 0x00000010:

#ifdef USB2_INTERNAL
        // query thru old budgeter code
        bw = USB2LIB_Get_Ep_bus_time(Endpoint->HsbEpContextPtr);
#endif
        break;

    default:
        bw = 0;
    }


    return bw;
}

BOOLEAN
MPx_AllocateBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    BOOLEAN gotBw;
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    case 0x00000000:

    //USB11
    case 0x00000008:

        // Legacy USB 1.1 bw allocation
        Endpoint->Parameters.Bandwidth =
            USBPORT_CalculateUsbBandwidthEp(HcFdoExt->HcFdoDeviceObject, Endpoint);

        // caclualte the best schedule position
        gotBw = USBPORT_AllocateBandwidthUSB11(HcFdoExt->HcFdoDeviceObject, Endpoint);
        if (gotBw) {
            Endpoint->EpFlags.USB11_Bandwidth = 1;
        }
        break;

    // USBX (wireless case?)
    case 0x00000400:
    // USB11 USBX (wireless case?)
    case 0x00000408:
    // USBX USB20
    case 0x00000410:
        {
        // USBX interface allocates thru miniport
        USB_MINIPORT_STATUS mpStatus;
        PTRANSACTION_TRANSLATOR tt;

        tt = Endpoint->Tt;
        if (tt) {
            ASSERT_TT(tt);
        }
        // call the miniport and attempt to allocate the necessary bus time for this endoint
        mpStatus = MPf_UsbxAllocateBandwidth(HcFdoExt, Endpoint, tt);

        if (mpStatus == USBMP_STATUS_SUCCESS) {
            gotBw = TRUE;
            Endpoint->EpFlags.USBX_Bandwidth = 1;
        } else {
            gotBw = FALSE;
        }
        }
        break;

    //USB20 - use old budgeter code
    case  0x00000010:
        gotBw = FALSE;
#ifdef USB2_INTERNAL
        {
        NTSTATUS nts;

        nts = USBPORT_AllocateBandwidthUSB20(HcFdoExt->HcFdoDeviceObject, Endpoint);
        if (NT_SUCCESS(nts)) {

            gotBw = TRUE;
        }

        }
#endif
        break;

    default:
        gotBw = FALSE;

    }

    return gotBw;
}


VOID
MPx_FreeBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    case 0x00000000:

    //USB11
    case 0x00000008:
        if (Endpoint->EpFlags.USB11_Bandwidth) {

            // caclualte the best schedule position
            USBPORT_FreeBandwidthUSB11(HcFdoExt->HcFdoDeviceObject, Endpoint);
        }

        break;

     //USBX (wireless case?)
    case 0x00000400:
    // USB11 USBX (wireless case?)
    case 0x00000408:
    // USBX USB20
    case 0x00000410:
        {
        // USBX interface frees thru miniport
        PTRANSACTION_TRANSLATOR tt;

        tt = Endpoint->Tt;
        if (tt) {
            ASSERT_TT(tt);
        }

        MPf_UsbxFreeBandwidth(HcFdoExt, Endpoint, tt);
        }
        break;

    //USB20 - use old budgeter code
    case  0x00000010:

#ifdef USB2_INTERNAL
        USBPORT_FreeBandwidthUSB20(HcFdoExt->HcFdoDeviceObject, Endpoint);
#endif
        break;

    default:
        break;
    }

    return ;
}

NTSTATUS
MPf_GetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __inout LPGUID ContainerId
    )
{
    USB_MINIPORT_STATUS usbMpStatus;


    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_RH_GetContainerIdForPort == NULL) ){
            return STATUS_NOT_IMPLEMENTED;
    }

    usbMpStatus = MPF(HcFdoExt).MINIPORT_RH_GetContainerIdForPort(HcFdoExt->Fdo.MiniportDeviceData,
                                                    PortNumber,
                                                    ContainerId);

    return ((usbMpStatus == USBMP_STATUS_SUCCESS) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);
}

VOID
MPf_SetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __in LPGUID ContainerId
    )
{
    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_RH_SetContainerIdForPort == NULL) ){
            return;
    }

    MPF(HcFdoExt).MINIPORT_RH_SetContainerIdForPort(HcFdoExt->Fdo.MiniportDeviceData,
                                                    PortNumber,
                                                    ContainerId);
}


NTSTATUS
MPf_GetHcCapabilities(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{
    MINIPORT_HC_CAPABILITIES_V2 hcCapabilitiesV2;
    USB_MINIPORT_STATUS         usbMpStatus;

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_GetHcCapabilities == NULL) ){
            return STATUS_NOT_IMPLEMENTED;
    }

    hcCapabilitiesV2.Size = sizeof(MINIPORT_HC_CAPABILITIES_V2);

    usbMpStatus = MPF(HcFdoExt).MINIPORT_GetHcCapabilities(
        HcFdoExt->Fdo.MiniportDeviceData,
        (PMINIPORT_HC_CAPABILITIES)&hcCapabilitiesV2);

    if (usbMpStatus == USBMP_STATUS_SUCCESS) {
        if (hcCapabilitiesV2.Size >= sizeof(MINIPORT_HC_CAPABILITIES_V1) &&
            hcCapabilitiesV2.Version >= MINIPORT_HC_CAPABILITIES_VERSION1) {
            HcFdoExt->Fdo.Bit64AddressingCapability =
                hcCapabilitiesV2.Bit64AddressingCapability ? 1 : 0;
        }

        if (hcCapabilitiesV2.Size >= sizeof(MINIPORT_HC_CAPABILITIES_V2) &&
            hcCapabilitiesV2.Version >= MINIPORT_HC_CAPABILITIES_VERSION2) {
            HcFdoExt->Fdo.RootHubTTCapable =
                hcCapabilitiesV2.IntegratedTTSupported;
        } else {
            HcFdoExt->Fdo.RootHubTTCapable = FALSE;
        }



        return STATUS_SUCCESS;

    } else {

        HcFdoExt->Fdo.Bit64AddressingCapability = 0;
        HcFdoExt->Fdo.RootHubTTCapable = FALSE;

        return STATUS_UNSUCCESSFUL;
    }
}


USHORT
MPf_GetDebugPortNumber(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_GetDebugPortNumber == NULL) ){
            return 0;
    }

    return MPF(HcFdoExt).MINIPORT_GetDebugPortNumber(
        HcFdoExt->Fdo.MiniportDeviceData);
}


BOOLEAN
MPf_CheckInvalidateRootHub(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_CheckInvalidateRootHub == NULL) ){
            return FALSE;
    }

    return MPF(HcFdoExt).MINIPORT_CheckInvalidateRootHub(
        HcFdoExt->Fdo.MiniportDeviceData);
}


VOID
MPf_SetClearPortResumeTime(
    __inout PDEVICE_EXTENSION   HcFdoExt,
    __in USHORT PortNumber,
    __in BOOLEAN SetTime,
    __in UCHAR ResumeTime
    )
{
    KIRQL irql;

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_SetPortResumeTime == NULL) ){
            return;
    }

    USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

    MPF(HcFdoExt).MINIPORT_SetPortResumeTime(HcFdoExt->Fdo.MiniportDeviceData,
                                             PortNumber,
                                             SetTime,
                                             ResumeTime);

    USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
}

/*********************************************************************************************************************/
/* USBPORT service documentation                                                                                     */
/*********************************************************************************************************************/

/*

Routine: (service) USBPORTSVC_RequestAsyncCallbackEx

NTSTATUS
USBPORTSVC_RequestAsyncCallbackEx(
    PDEVICE_DATA DeviceData,
    ULONG MilliSecondInterval,
    PVOID Context,
    ULONG ContextLength,
    PMPFN_MINIPORT_CALLBACK Callback,
    PVOID *CbHandle,
    BOOLEAN RootHubPortFunction
    )

Description:

    Allows miniport to request an async callback when the millisecond interval specified has elapsed.

    The context field is copied to an allocated structure and the miniport is called back with the copy so it
    is safe for the miniport to use a stack variable as context.

    The callback is serialized with the core function lock.

Irql - DISPATCH_LEVEL

Parameters:

MilliSecondInterval - minimum (ms) time that must elapse before callback is invoked.

Context - miniport specified context for callback.

ContextLength - length in bytes of miniport context.

CbHandle - optional, pointer filled in with handle that can be used to cancel the callback dpc

RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* or an async timer callback


Returns:

     NT error code.


*/


/*

Routine: (service) USBPORTSVC_CancelAsyncCallback

BOOLEAN
USBPORTSVC_CancelAsyncCallback(
    PDEVICE_DATA DeviceData,
    PVOID CbHandle,
    BOOLEAN RootHubPortFunction
    )

Description:

    Allows miniport to cancel a previously requested callback.


Irql - DISPATCH_LEVEL

Parameters:

CbHandle - handle returned from a call to USBPORTSVC_RequestAsyncCallbackEx

RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* or an async timer callback


Returns:

     TRUE if callback was canceled.


*/

#include "Test_Include.fx"

int Test_Count = 449;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_sw_inst_setp_eq";

VS_CRITERIA VS_001_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_sw : setp_eq source reg combination v0, v1 is allowed";
string VS_001 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"setp_eq p0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_sw : setp_eq source reg combination v0, r0 is allowed";
string VS_002 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"setp_eq p0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_sw : setp_eq source reg combination v0, c0 is allowed";
string VS_003 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_sw : setp_eq source reg combination r0, v0 is allowed";
string VS_004 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"setp_eq p0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_sw : setp_eq source reg combination r0, r1 is allowed";
string VS_005 = 
	"vs_2_sw "
	"mov r0, c0 "
	"mov r1, c0 "
	"setp_eq p0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_sw : setp_eq source reg combination r0, c0 is allowed";
string VS_006 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"setp_eq p0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_sw : setp_eq source reg combination c0, v0 is allowed";
string VS_007 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_sw : setp_eq source reg combination c0, r0 is allowed";
string VS_008 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"setp_eq p0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_sw : setp_eq source reg combination c0, c1 is allowed";
string VS_009 = 
	"vs_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"setp_eq p0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_sw : setp_eq source reg combination -v0, -c0 is allowed";
string VS_010 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, -v0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_sw : setp_eq source reg combination -v0, c0 is allowed";
string VS_011 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, -v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_2_sw : setp_eq source reg combination v0, -c0 is allowed";
string VS_012 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_2_sw : setp_eq source reg combination v0, c0 is allowed";
string VS_013 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_2_sw : setp_eq source reg combination v0.x, c0 is allowed";
string VS_014 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.x, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_2_sw : setp_eq source reg combination v0.y, c0 is allowed";
string VS_015 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.y, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_2_sw : setp_eq source reg combination v0.z, c0 is allowed";
string VS_016 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.z, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_2_sw : setp_eq source reg combination v0.w, c0 is allowed";
string VS_017 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.w, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_2_sw : setp_eq source reg combination v0.yzxw, c0 is allowed";
string VS_018 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.yzxw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_2_sw : setp_eq source reg combination v0.zxyw, c0 is allowed";
string VS_019 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.zxyw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_sw : setp_eq source reg combination v0.wzyx, c0 is allowed";
string VS_020 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.wzyx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_sw : setp_eq source reg combination v0.wyxz, c0 is allowed";
string VS_021 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.wyxz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_sw : setp_eq source reg combination v0.xzyw, c0 is allowed";
string VS_022 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.xzyw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_sw : setp_eq source reg combination v0.xywz, c0 is allowed";
string VS_023 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.xywz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_sw : setp_eq source reg combination v0.zyx, c0 is allowed";
string VS_024 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.zyx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_sw : setp_eq source reg combination v0.xzy, c0 is allowed";
string VS_025 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.xzy, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_sw : setp_eq source reg combination v0.ywx, c0 is allowed";
string VS_026 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.ywx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_sw : setp_eq source reg combination v0.yx, c0 is allowed";
string VS_027 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.yx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_sw : setp_eq source reg combination v0.wz, c0 is allowed";
string VS_028 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.wz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_sw : setp_eq source reg combination v0.zy, c0 is allowed";
string VS_029 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0.zy, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.x is allowed";
string VS_030 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.y is allowed";
string VS_031 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.z is allowed";
string VS_032 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.w is allowed";
string VS_033 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.yzxw is allowed";
string VS_034 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.zxyw is allowed";
string VS_035 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.wzyx is allowed";
string VS_036 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.wyxz is allowed";
string VS_037 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.xzyw is allowed";
string VS_038 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.xywz is allowed";
string VS_039 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.zyx is allowed";
string VS_040 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.xzy is allowed";
string VS_041 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.ywx is allowed";
string VS_042 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.yx is allowed";
string VS_043 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.wz is allowed";
string VS_044 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_sw : setp_eq source reg combination v0, c0.zy is allowed";
string VS_045 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"setp_eq p0, v0, c0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_sw : setp_eq source reg combination a0, a0 is NOT allowed";
string VS_046 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_sw : setp_eq source reg combination a0, b0 is NOT allowed";
string VS_047 = 
	"vs_2_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_sw : setp_eq source reg combination a0, i0 is NOT allowed";
string VS_048 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_sw : setp_eq source reg combination a0, aL is NOT allowed";
string VS_049 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"setp_eq p0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_2_sw : setp_eq source reg combination a0, p0 is NOT allowed";
string VS_050 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_sw : setp_eq source reg combination a0, oPos is NOT allowed";
string VS_051 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_sw : setp_eq source reg combination a0, oFog is NOT allowed";
string VS_052 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_sw : setp_eq source reg combination a0, oPts is NOT allowed";
string VS_053 = 
	"vs_2_sw "
	"dcl ts "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_sw : setp_eq source reg combination a0, oD0 is NOT allowed";
string VS_054 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_sw : setp_eq source reg combination a0, oT0 is NOT allowed";
string VS_055 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_sw : setp_eq source reg combination b0, a0 is NOT allowed";
string VS_056 = 
	"vs_2_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"setp_eq p0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_sw : setp_eq source reg combination b0, b0 is NOT allowed";
string VS_057 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_sw : setp_eq source reg combination b0, i0 is NOT allowed";
string VS_058 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"setp_eq p0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_sw : setp_eq source reg combination b0, aL is NOT allowed";
string VS_059 = 
	"vs_2_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"setp_eq p0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_2_sw : setp_eq source reg combination b0, p0 is NOT allowed";
string VS_060 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_sw : setp_eq source reg combination b0, oPos is NOT allowed";
string VS_061 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_sw : setp_eq source reg combination b0, oFog is NOT allowed";
string VS_062 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_sw : setp_eq source reg combination b0, oPts is NOT allowed";
string VS_063 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl ts "
	"setp_eq p0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_sw : setp_eq source reg combination b0, oD0 is NOT allowed";
string VS_064 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_sw : setp_eq source reg combination b0, oT0 is NOT allowed";
string VS_065 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_sw : setp_eq source reg combination i0, a0 is NOT allowed";
string VS_066 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"setp_eq p0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_sw : setp_eq source reg combination i0, b0 is NOT allowed";
string VS_067 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_sw : setp_eq source reg combination i0, i0 is NOT allowed";
string VS_068 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_sw : setp_eq source reg combination i0, aL is NOT allowed";
string VS_069 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"setp_eq p0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_2_sw : setp_eq source reg combination i0, p0 is NOT allowed";
string VS_070 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_sw : setp_eq source reg combination i0, oPos is NOT allowed";
string VS_071 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_sw : setp_eq source reg combination i0, oFog is NOT allowed";
string VS_072 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_sw : setp_eq source reg combination i0, oPts is NOT allowed";
string VS_073 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"setp_eq p0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_sw : setp_eq source reg combination i0, oD0 is NOT allowed";
string VS_074 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_sw : setp_eq source reg combination i0, oT0 is NOT allowed";
string VS_075 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_sw : setp_eq source reg combination aL, a0 is NOT allowed";
string VS_076 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"setp_eq p0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_sw : setp_eq source reg combination aL, b0 is NOT allowed";
string VS_077 = 
	"vs_2_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_sw : setp_eq source reg combination aL, i0 is NOT allowed";
string VS_078 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_sw : setp_eq source reg combination aL, aL is NOT allowed";
string VS_079 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_2_sw : setp_eq source reg combination aL, p0 is NOT allowed";
string VS_080 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_sw : setp_eq source reg combination aL, oPos is NOT allowed";
string VS_081 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_sw : setp_eq source reg combination aL, oFog is NOT allowed";
string VS_082 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_sw : setp_eq source reg combination aL, oPts is NOT allowed";
string VS_083 = 
	"vs_2_sw "
	"dcl ts "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_sw : setp_eq source reg combination aL, oD0 is NOT allowed";
string VS_084 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_sw : setp_eq source reg combination aL, oT0 is NOT allowed";
string VS_085 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_2_sw : setp_eq source reg combination p0, a0 is NOT allowed";
string VS_086 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_2_sw : setp_eq source reg combination p0, b0 is NOT allowed";
string VS_087 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_2_sw : setp_eq source reg combination p0, i0 is NOT allowed";
string VS_088 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_2_sw : setp_eq source reg combination p0, aL is NOT allowed";
string VS_089 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_sw : setp_eq source reg combination p0, p0 is NOT allowed";
string VS_090 = 
	"vs_2_sw "
	"setp_eq p0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_sw : setp_eq source reg combination p0, oPos is NOT allowed";
string VS_091 = 
	"vs_2_sw "
	"setp_eq p0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_sw : setp_eq source reg combination p0, oFog is NOT allowed";
string VS_092 = 
	"vs_2_sw "
	"setp_eq p0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_sw : setp_eq source reg combination p0, oPts is NOT allowed";
string VS_093 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_sw : setp_eq source reg combination p0, oD0 is NOT allowed";
string VS_094 = 
	"vs_2_sw "
	"setp_eq p0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_sw : setp_eq source reg combination p0, oT0 is NOT allowed";
string VS_095 = 
	"vs_2_sw "
	"setp_eq p0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_sw : setp_eq source reg combination oPos, a0 is NOT allowed";
string VS_096 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_sw : setp_eq source reg combination oPos, b0 is NOT allowed";
string VS_097 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_sw : setp_eq source reg combination oPos, i0 is NOT allowed";
string VS_098 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_sw : setp_eq source reg combination oPos, aL is NOT allowed";
string VS_099 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_sw : setp_eq source reg combination oPos, p0 is NOT allowed";
string VS_100 = 
	"vs_2_sw "
	"setp_eq p0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_sw : setp_eq source reg combination oPos, oPos is NOT allowed";
string VS_101 = 
	"vs_2_sw "
	"setp_eq p0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_sw : setp_eq source reg combination oPos, oFog is NOT allowed";
string VS_102 = 
	"vs_2_sw "
	"setp_eq p0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_sw : setp_eq source reg combination oPos, oPts is NOT allowed";
string VS_103 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_sw : setp_eq source reg combination oPos, oD0 is NOT allowed";
string VS_104 = 
	"vs_2_sw "
	"setp_eq p0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_sw : setp_eq source reg combination oPos, oT0 is NOT allowed";
string VS_105 = 
	"vs_2_sw "
	"setp_eq p0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_sw : setp_eq source reg combination oFog, a0 is NOT allowed";
string VS_106 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_sw : setp_eq source reg combination oFog, b0 is NOT allowed";
string VS_107 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_sw : setp_eq source reg combination oFog, i0 is NOT allowed";
string VS_108 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_sw : setp_eq source reg combination oFog, aL is NOT allowed";
string VS_109 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_2_sw : setp_eq source reg combination oFog, p0 is NOT allowed";
string VS_110 = 
	"vs_2_sw "
	"setp_eq p0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_sw : setp_eq source reg combination oFog, oPos is NOT allowed";
string VS_111 = 
	"vs_2_sw "
	"setp_eq p0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_sw : setp_eq source reg combination oFog, oFog is NOT allowed";
string VS_112 = 
	"vs_2_sw "
	"setp_eq p0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_sw : setp_eq source reg combination oFog, oPts is NOT allowed";
string VS_113 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_sw : setp_eq source reg combination oFog, oD0 is NOT allowed";
string VS_114 = 
	"vs_2_sw "
	"setp_eq p0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_sw : setp_eq source reg combination oFog, oT0 is NOT allowed";
string VS_115 = 
	"vs_2_sw "
	"setp_eq p0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_sw : setp_eq source reg combination oPts, a0 is NOT allowed";
string VS_116 = 
	"vs_2_sw "
	"dcl ts "
	"mova a0.x, c0.x "
	"setp_eq p0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_sw : setp_eq source reg combination oPts, b0 is NOT allowed";
string VS_117 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl ts "
	"setp_eq p0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_sw : setp_eq source reg combination oPts, i0 is NOT allowed";
string VS_118 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"setp_eq p0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_2_sw : setp_eq source reg combination oPts, aL is NOT allowed";
string VS_119 = 
	"vs_2_sw "
	"dcl ts "
	"mova aL.x, c0.x "
	"setp_eq p0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_2_sw : setp_eq source reg combination oPts, p0 is NOT allowed";
string VS_120 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_sw : setp_eq source reg combination oPts, oPos is NOT allowed";
string VS_121 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_sw : setp_eq source reg combination oPts, oFog is NOT allowed";
string VS_122 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_sw : setp_eq source reg combination oPts, oPts is NOT allowed";
string VS_123 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_sw : setp_eq source reg combination oPts, oD0 is NOT allowed";
string VS_124 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_2_sw : setp_eq source reg combination oPts, oT0 is NOT allowed";
string VS_125 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_2_sw : setp_eq source reg combination oD0, a0 is NOT allowed";
string VS_126 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_2_sw : setp_eq source reg combination oD0, b0 is NOT allowed";
string VS_127 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_2_sw : setp_eq source reg combination oD0, i0 is NOT allowed";
string VS_128 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_2_sw : setp_eq source reg combination oD0, aL is NOT allowed";
string VS_129 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_2_sw : setp_eq source reg combination oD0, p0 is NOT allowed";
string VS_130 = 
	"vs_2_sw "
	"setp_eq p0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_2_sw : setp_eq source reg combination oD0, oPos is NOT allowed";
string VS_131 = 
	"vs_2_sw "
	"setp_eq p0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_2_sw : setp_eq source reg combination oD0, oFog is NOT allowed";
string VS_132 = 
	"vs_2_sw "
	"setp_eq p0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_2_sw : setp_eq source reg combination oD0, oPts is NOT allowed";
string VS_133 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_2_sw : setp_eq source reg combination oD0, oD0 is NOT allowed";
string VS_134 = 
	"vs_2_sw "
	"setp_eq p0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_sw : setp_eq source reg combination oD0, oT0 is NOT allowed";
string VS_135 = 
	"vs_2_sw "
	"setp_eq p0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_sw : setp_eq source reg combination oT0, a0 is NOT allowed";
string VS_136 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"setp_eq p0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_sw : setp_eq source reg combination oT0, b0 is NOT allowed";
string VS_137 = 
	"vs_2_sw "
	"defb b0, true "
	"setp_eq p0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_sw : setp_eq source reg combination oT0, i0 is NOT allowed";
string VS_138 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"setp_eq p0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_2_sw : setp_eq source reg combination oT0, aL is NOT allowed";
string VS_139 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"setp_eq p0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_2_sw : setp_eq source reg combination oT0, p0 is NOT allowed";
string VS_140 = 
	"vs_2_sw "
	"setp_eq p0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_sw : setp_eq source reg combination oT0, oPos is NOT allowed";
string VS_141 = 
	"vs_2_sw "
	"setp_eq p0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_sw : setp_eq source reg combination oT0, oFog is NOT allowed";
string VS_142 = 
	"vs_2_sw "
	"setp_eq p0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_sw : setp_eq source reg combination oT0, oPts is NOT allowed";
string VS_143 = 
	"vs_2_sw "
	"dcl ts "
	"setp_eq p0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_sw : setp_eq source reg combination oT0, oD0 is NOT allowed";
string VS_144 = 
	"vs_2_sw "
	"setp_eq p0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_sw : setp_eq source reg combination oT0, oT0 is NOT allowed";
string VS_145 = 
	"vs_2_sw "
	"setp_eq p0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_sw : setp_eq dest p0 is allowed";
string VS_146 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_sw : setp_eq dest v# is NOT allowed";
string VS_147 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq v0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_sw : setp_eq dest r# is NOT allowed";
string VS_148 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_2_sw : setp_eq dest c# is NOT allowed";
string VS_149 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq c1, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_sw : setp_eq dest a0 is NOT allowed";
string VS_150 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq a0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_sw : setp_eq dest b# is NOT allowed";
string VS_151 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq b0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_sw : setp_eq dest i# is NOT allowed";
string VS_152 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq i0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_sw : setp_eq dest aL is NOT allowed";
string VS_153 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq aL, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_sw : setp_eq dest oPos is NOT allowed";
string VS_154 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq oPos, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_sw : setp_eq dest oD# is NOT allowed";
string VS_155 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq oD0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_2_sw : setp_eq dest oT# is NOT allowed";
string VS_156 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq oT0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_sw : setp_eq write mask .x is allowed";
string VS_157 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.x, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_sw : setp_eq write mask .y is allowed";
string VS_158 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.y, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_2_sw : setp_eq write mask .z is allowed";
string VS_159 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.z, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_sw : setp_eq write mask .w is allowed";
string VS_160 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.w, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_sw : setp_eq write mask .xy is allowed";
string VS_161 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_sw : setp_eq write mask .yz is allowed";
string VS_162 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.yz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_sw : setp_eq write mask .zw is allowed";
string VS_163 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.zw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_sw : setp_eq write mask .xz is allowed";
string VS_164 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_sw : setp_eq write mask .xw is allowed";
string VS_165 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_sw : setp_eq write mask .yw is allowed";
string VS_166 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.yw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_sw : setp_eq write mask .xyz is allowed";
string VS_167 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xyz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_2_sw : setp_eq write mask .yzw is allowed";
string VS_168 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.yzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_2_sw : setp_eq write mask .xzw is allowed";
string VS_169 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_2_sw : setp_eq write mask .xyzw is allowed";
string VS_170 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.xyzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_2_sw : setp_eq write mask .yx is NOT allowed";
string VS_171 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.yx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_2_sw : setp_eq write mask .zx is NOT allowed";
string VS_172 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.zx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_2_sw : setp_eq write mask .zy is NOT allowed";
string VS_173 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.zy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_2_sw : setp_eq write mask .wx is NOT allowed";
string VS_174 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_2_sw : setp_eq write mask .wz is NOT allowed";
string VS_175 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_2_sw : setp_eq write mask .wy is NOT allowed";
string VS_176 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_2_sw : setp_eq write mask .zyx is NOT allowed";
string VS_177 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.zyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_2_sw : setp_eq write mask .wzy is NOT allowed";
string VS_178 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wzy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_2_sw : setp_eq write mask .wzx is NOT allowed";
string VS_179 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wzx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_2_sw : setp_eq write mask .wyx is NOT allowed";
string VS_180 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_2_sw : setp_eq write mask .yxw is NOT allowed";
string VS_181 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.yxw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_2_sw : setp_eq write mask .wzyx is NOT allowed";
string VS_182 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.wzyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_2_sw : setp_eq write mask .zxwy is NOT allowed";
string VS_183 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"setp_eq p0.zxwy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if b0 block";
string VS_184 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_185_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if b0 block";
string VS_185 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_186_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if b0 block";
string VS_186 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_187_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if b0 block";
string VS_187 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_188_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if b0 block";
string VS_188 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_189_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if b0 nop else block";
string VS_189 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_190_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if b0 nop else block";
string VS_190 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_191_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if b0 nop else block";
string VS_191 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_192_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if b0 nop else block";
string VS_192 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_193_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if b0 nop else block";
string VS_193 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_194_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_2_sw : setp_eq is allowed in a 1 level loop aL, i0 block";
string VS_194 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	setp_eq p0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_195_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_2_sw : setp_eq is allowed in a 2 level loop aL, i0 block";
string VS_195 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_196_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_2_sw : setp_eq is allowed in a 3 level loop aL, i0 block";
string VS_196 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			setp_eq p0, v0, v1 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_197_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_2_sw : setp_eq is allowed in a 4 level loop aL, i0 block";
string VS_197 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				setp_eq p0, v0, v1 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_198_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_2_sw : setp_eq is NOT allowed in a 5 level loop aL, i0 block";
string VS_198 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					setp_eq p0, v0, v1 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_199_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_2_sw : setp_eq is allowed in a 1 level rep i0 block";
string VS_199 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	setp_eq p0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_200_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_2_sw : setp_eq is allowed in a 2 level rep i0 block";
string VS_200 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_201_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_2_sw : setp_eq is allowed in a 3 level rep i0 block";
string VS_201 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			setp_eq p0, v0, v1 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_202_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_2_sw : setp_eq is allowed in a 4 level rep i0 block";
string VS_202 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				setp_eq p0, v0, v1 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_203_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_2_sw : setp_eq is NOT allowed in a 5 level rep i0 block";
string VS_203 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					setp_eq p0, v0, v1 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_204_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if_ne c0.x, c0.y block";
string VS_204 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_205_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if_ne c0.x, c0.y block";
string VS_205 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_206_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if_ne c0.x, c0.y block";
string VS_206 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_207_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if_ne c0.x, c0.y block";
string VS_207 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																if_ne c0.x, c0.y "
	"																	if_ne c0.x, c0.y "
	"																		if_ne c0.x, c0.y "
	"																			if_ne c0.x, c0.y "
	"																				if_ne c0.x, c0.y "
	"																					if_ne c0.x, c0.y "
	"																						if_ne c0.x, c0.y "
	"																							if_ne c0.x, c0.y "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_208_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if_ne c0.x, c0.y block";
string VS_208 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																if_ne c0.x, c0.y "
	"																	if_ne c0.x, c0.y "
	"																		if_ne c0.x, c0.y "
	"																			if_ne c0.x, c0.y "
	"																				if_ne c0.x, c0.y "
	"																					if_ne c0.x, c0.y "
	"																						if_ne c0.x, c0.y "
	"																							if_ne c0.x, c0.y "
	"																								if_ne c0.x, c0.y "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_209_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if_ne c0.x, c0.y nop else block";
string VS_209 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_210_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if_ne c0.x, c0.y nop else block";
string VS_210 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_211_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if_ne c0.x, c0.y nop else block";
string VS_211 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_212_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if_ne c0.x, c0.y nop else block";
string VS_212 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																if_ne c0.x, c0.y nop else "
	"																	if_ne c0.x, c0.y nop else "
	"																		if_ne c0.x, c0.y nop else "
	"																			if_ne c0.x, c0.y nop else "
	"																				if_ne c0.x, c0.y nop else "
	"																					if_ne c0.x, c0.y nop else "
	"																						if_ne c0.x, c0.y nop else "
	"																							if_ne c0.x, c0.y nop else "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_213_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if_ne c0.x, c0.y nop else block";
string VS_213 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																if_ne c0.x, c0.y nop else "
	"																	if_ne c0.x, c0.y nop else "
	"																		if_ne c0.x, c0.y nop else "
	"																			if_ne c0.x, c0.y nop else "
	"																				if_ne c0.x, c0.y nop else "
	"																					if_ne c0.x, c0.y nop else "
	"																						if_ne c0.x, c0.y nop else "
	"																							if_ne c0.x, c0.y nop else "
	"																								if_ne c0.x, c0.y nop else "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_214_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if p0.x block";
string VS_214 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_215_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if p0.x block";
string VS_215 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_216_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if p0.x block";
string VS_216 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_217_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if p0.x block";
string VS_217 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_218_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if p0.x block";
string VS_218 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_219_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_2_sw : setp_eq is allowed in a 1 level if p0.x nop else block";
string VS_219 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x nop else "
	"	setp_eq p0, v0, v1 "
	"endif ";

VS_CRITERIA VS_220_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_2_sw : setp_eq is allowed in a 8 level if p0.x nop else block";
string VS_220 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								setp_eq p0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_221_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_2_sw : setp_eq is allowed in a 16 level if p0.x nop else block";
string VS_221 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																setp_eq p0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_222_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_2_sw : setp_eq is allowed in a 24 level if p0.x nop else block";
string VS_222 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								setp_eq p0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_223_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level if p0.x nop else block";
string VS_223 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									setp_eq p0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_224_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_2_sw : setp_eq is allowed in a 1 level loop aL, i0 break block";
string VS_224 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 break "
	"	setp_eq p0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_225_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level loop aL, i0 break block";
string VS_225 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								setp_eq p0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_226_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level loop aL, i0 break block";
string VS_226 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																setp_eq p0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_227_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level loop aL, i0 break block";
string VS_227 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								setp_eq p0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_228_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level loop aL, i0 break block";
string VS_228 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									setp_eq p0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_229_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_2_sw : setp_eq is allowed in a 1 level rep i0 break block";
string VS_229 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break "
	"	setp_eq p0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_230_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level rep i0 break block";
string VS_230 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								setp_eq p0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_231_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level rep i0 break block";
string VS_231 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																setp_eq p0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_232_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level rep i0 break block";
string VS_232 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								setp_eq p0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_233_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level rep i0 break block";
string VS_233 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									setp_eq p0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_234_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_2_sw : setp_eq is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_234 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 breakp p0.x "
	"	setp_eq p0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_235_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_235 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								setp_eq p0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_236_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_236 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																setp_eq p0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_237_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_237 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								setp_eq p0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_238_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_238 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									setp_eq p0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_239_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_2_sw : setp_eq is allowed in a 1 level rep i0 breakp p0.x block";
string VS_239 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 breakp p0.x "
	"	setp_eq p0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_240_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_240 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								setp_eq p0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_241_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_241 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																setp_eq p0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_242_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_242 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								setp_eq p0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_243_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_243 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									setp_eq p0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_244_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_2_sw : setp_eq is allowed in a 1 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_244 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	setp_eq p0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_245_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_245 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								setp_eq p0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_246_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_246 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																setp_eq p0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_247_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_247 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								setp_eq p0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_248_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_248 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								loop aL, i0 break_gt c0.x, c0.y "
	"																									setp_eq p0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_249_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_2_sw : setp_eq is allowed in a 1 level rep i0 break_gt c0.x, c0.y block";
string VS_249 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break_gt c0.x, c0.y "
	"	setp_eq p0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_250_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_2_sw : setp_eq is NOT allowed in a 8 level rep i0 break_gt c0.x, c0.y block";
string VS_250 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								setp_eq p0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_251_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_2_sw : setp_eq is NOT allowed in a 16 level rep i0 break_gt c0.x, c0.y block";
string VS_251 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																setp_eq p0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_252_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_2_sw : setp_eq is NOT allowed in a 24 level rep i0 break_gt c0.x, c0.y block";
string VS_252 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								setp_eq p0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_253_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_2_sw : setp_eq is NOT allowed in a 25 level rep i0 break_gt c0.x, c0.y block";
string VS_253 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								rep i0 break_gt c0.x, c0.y "
	"																									setp_eq p0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_254_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if b0 block";
string VS_254 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_255_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if b0 nop else block";
string VS_255 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_256_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and loop aL, i0 block";
string VS_256 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_257_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and rep i0 block";
string VS_257 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_258_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if_ne c0.x, c0.y block";
string VS_258 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_259_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if_ne c0.x, c0.y nop else block";
string VS_259 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_260_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if p0.x block";
string VS_260 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_261_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and if p0.x nop else block";
string VS_261 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_262_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and loop aL, i0 break block";
string VS_262 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_263_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and rep i0 break block";
string VS_263 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_264_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_264 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_265_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_265 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_depth v1 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_266_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and loop aL, i0 break_eq c0.x, c0.y block";
string VS_266 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_267_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_2_sw : setp_eq is allowed in a if b0 and rep i0 break_eq c0.x, c0.y block";
string VS_267 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_268_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if b0 block";
string VS_268 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_269_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if b0 nop else block";
string VS_269 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_270_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and loop aL, i0 block";
string VS_270 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_271_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and rep i0 block";
string VS_271 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_272_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if_ne c0.x, c0.y block";
string VS_272 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_273_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if_ne c0.x, c0.y nop else block";
string VS_273 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_274_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if p0.x block";
string VS_274 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_275_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and if p0.x nop else block";
string VS_275 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_276_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_276 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_277_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and rep i0 break block";
string VS_277 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_278_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_278 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_279_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_279 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_280_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_280 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_281_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_2_sw : setp_eq is allowed in a if b0 nop else and rep i0 break_eq c0.x, c0.y block";
string VS_281 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"if b0 nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_282_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if b0 block";
string VS_282 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_283_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if b0 nop else block";
string VS_283 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_284_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_284 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_285_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and rep i0 block";
string VS_285 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_286_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if_ne c0.x, c0.y block";
string VS_286 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_287_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if_ne c0.x, c0.y nop else block";
string VS_287 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_288_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if p0.x block";
string VS_288 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_289_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_289 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_290_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_290 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_291_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and rep i0 break block";
string VS_291 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_292_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_292 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_293_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_293 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_294_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and loop aL, i0 break_eq c0.x, c0.y block";
string VS_294 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_295_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_2_sw : setp_eq is allowed in a loop aL, i0 and rep i0 break_eq c0.x, c0.y block";
string VS_295 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"loop aL, i0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_296_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if b0 block";
string VS_296 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_297_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if b0 nop else block";
string VS_297 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_298_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and loop aL, i0 block";
string VS_298 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_299_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and rep i0 block";
string VS_299 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_300_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if_ne c0.x, c0.y block";
string VS_300 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_301_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if_ne c0.x, c0.y nop else block";
string VS_301 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_302_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if p0.x block";
string VS_302 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_303_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and if p0.x nop else block";
string VS_303 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_304_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and loop aL, i0 break block";
string VS_304 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_305_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and rep i0 break block";
string VS_305 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_306_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_306 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_307_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_307 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_308_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and loop aL, i0 break_lt c0.x, c0.y block";
string VS_308 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_309_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_2_sw : setp_eq is allowed in a rep i0 and rep i0 break_lt c0.x, c0.y block";
string VS_309 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_310_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if b0 block";
string VS_310 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_311_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if b0 nop else block";
string VS_311 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_312_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and loop aL, i0 block";
string VS_312 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_313_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and rep i0 block";
string VS_313 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_314_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if_ne c0.x, c0.y block";
string VS_314 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_315_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if_ne c0.x, c0.y nop else block";
string VS_315 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_316_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if p0.x block";
string VS_316 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_317_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and if p0.x nop else block";
string VS_317 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_318_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and loop aL, i0 break block";
string VS_318 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_319_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and rep i0 break block";
string VS_319 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_320_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_320 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_321_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and rep i0 breakp p0.x block";
string VS_321 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_322_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string VS_322 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_323_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string VS_323 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_324_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if b0 block";
string VS_324 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_325_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if b0 nop else block";
string VS_325 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_fog v1 "
	"if_ne c0.x, c0.y nop else "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_326_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 block";
string VS_326 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_327_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and rep i0 block";
string VS_327 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_328_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if_ne c0.x, c0.y block";
string VS_328 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_329_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if_ne c0.x, c0.y nop else block";
string VS_329 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_330_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if p0.x block";
string VS_330 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_331_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and if p0.x nop else block";
string VS_331 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_332_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 break block";
string VS_332 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_333_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and rep i0 break block";
string VS_333 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_334_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_334_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_334 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_335_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_335 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_336_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_336 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_337_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_2_sw : setp_eq is allowed in a if_ne c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string VS_337 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_338_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if b0 block";
string VS_338 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_339_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_339_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if b0 nop else block";
string VS_339 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_340_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and loop aL, i0 block";
string VS_340 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_341_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and rep i0 block";
string VS_341 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_342_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if_ne c0.x, c0.y block";
string VS_342 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_343_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if_ne c0.x, c0.y nop else block";
string VS_343 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_344_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_344_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if p0.x block";
string VS_344 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_345_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_345_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and if p0.x nop else block";
string VS_345 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_346_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_346_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and loop aL, i0 break block";
string VS_346 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_347_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_347_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and rep i0 break block";
string VS_347 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_348_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_348_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_348 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_349_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_349_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_349 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_350_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_350_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_350 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_351_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_351_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string VS_351 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_352_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_352_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if b0 block";
string VS_352 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x nop else "
	"	if b0 "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_353_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_353_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if b0 nop else block";
string VS_353 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_354_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_354_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_354 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_355_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_355_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and rep i0 block";
string VS_355 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	rep i0 "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_356_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_356_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if_ne c0.x, c0.y block";
string VS_356 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if_ne c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_357_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_357_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if_ne c0.x, c0.y nop else block";
string VS_357 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if_ne c0.x, c0.y nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_358_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_358_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if p0.x block";
string VS_358 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_359_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_359_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_359 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		setp_eq p0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_360_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_360_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_360 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_361_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_361_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and rep i0 break block";
string VS_361 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	rep i0 break "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_362_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_362_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_362 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_363_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_363_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_363 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_364_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_364_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_364 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_365_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_365_Desc = "vs_2_sw : setp_eq is allowed in a if p0.x nop else and rep i0 break_ge c0.x, c0.y block";
string VS_365 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		setp_eq p0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_366_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_366_Desc = "vs_2_sw : setp_eq is allowed in a lo