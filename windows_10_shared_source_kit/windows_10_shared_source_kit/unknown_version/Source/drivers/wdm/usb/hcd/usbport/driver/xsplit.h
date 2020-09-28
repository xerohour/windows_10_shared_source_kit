//
//    Copyright (C) Microsoft.  All rights reserved.
//


VOID
USBPORT_Split_iCompleteChildTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT SplitTransfer,
    __in USBD_STATUS UsbdCompleteCode,
    __in TX_EVENT TxEvent,
    __in TX_STATE TxState
    );

NTSTATUS
USBPORT_Split_Transfer(
    __inout PDEVICE_OBJECT FdoDeviceObject,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PLIST_ENTRY TransferList
    );    

VOID
USBPORT_Split_iCancelParentTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT ParentTransfer
    );