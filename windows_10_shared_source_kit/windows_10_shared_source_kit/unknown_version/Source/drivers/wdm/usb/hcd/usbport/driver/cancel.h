//
//    Copyright (C) Microsoft.  All rights reserved.
//
VOID
USBPORT_TxInitialize(
    __inout PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_TxSetState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in TX_EVENT Event,
    __in TX_STATE NewState
    );

NTSTATUS
USBPORT_TxCsqInsertIrpEx(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID InsertContext
    );

VOID
USBPORT_TxCsqRemoveIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

PIRP
USBPORT_TxCsqPeekNextIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    );

__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxCsqAcquireLock(
    PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxCsqReleaseLock(
    PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    );

VOID
USBPORT_TxCsqCompleteCanceledIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

PHCD_TRANSFER_CONTEXT
USBPORT_TxCsqFindIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    );

VOID
USBPORT_TxValidateTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PIRP Irp
    );

VOID
USBPORT_TxQueueTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PIRP Irp
    );

__drv_savesIRQL
__drv_raisesIRQL(DISPATCH_LEVEL)
KIRQL
USBPORT_TxAcquireLock(
    __inout PDEVICE_OBJECT      HcFdo,
    __inout_opt PXDPC_CONTEXT   Xdpc
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_TxReleaseLock(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

BOOLEAN
USBPORT_AxQueueAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PUSBD_DEVICE_HANDLE DevHandle,
    __inout_opt PURB Urb,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT AbortEvent
    );

NTSTATUS
USBPORT_AxCsqInsertIrpEx(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID InsertContext
    );

PUSB_ABORT_PIPE_CONTEXT
USBPORT_AxFindIrpCsq(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    );

PUSB_ABORT_PIPE_CONTEXT
USBPORT_AxFindIrpCanceled(
    __inout PDEVICE_OBJECT HcFdo,
    __in PIRP Irp
    );

VOID
USBPORT_AxCsqRemoveIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

PIRP
USBPORT_AxCsqPeekNextIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    );

__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_AxCsqAcquireLock(
    PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_AxCsqReleaseLock(
    PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    );

VOID
USBPORT_AxCsqCompleteCanceledIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindCanceledTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    );
