//
//    Copyright (C) Microsoft.  All rights reserved.
//

VOID
USBPORT_XdpcInitialize(
    __inout PDEVICE_OBJECT HcFdo
    );


XDPC_STATE
USBPORT_Xdpc_iSetState(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT Xdpc,
     __in XDPC_EVENT Event,
     __in XDPC_STATE State
     );

VOID
USBPORT_Xdpc_iSignal(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
     );


VOID
USBPORT_Xdpc_Signal(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
    );


VOID
USBPORT_Xdpc_InitDpc(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc,
    __in PKDEFERRED_ROUTINE XdpcWorker,
    __in PXDPC_WORKER_ROUTINE DpcWorker,
    __in ULONG TimeLimit_ms
    );


VOID
USBPORT_Xdpc_Disable(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
     );

VOID
USBPORT_Xdpc_Enable(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT Xdpc
     );

BOOLEAN
USBPORT_Xdpc_iTimeExpired(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    );

VOID
USBPORT_Xdpc_iBegin(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
     );


VOID
USBPORT_Xdpc_End(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    );


KDEFERRED_ROUTINE USBPORT_Xdpc_Worker_MapDpc;
KDEFERRED_ROUTINE USBPORT_Xdpc_Worker_HcIntDpc;
KDEFERRED_ROUTINE USBPORT_Xdpc_Worker_DoneDpc;
KDEFERRED_ROUTINE USBPORT_Xdpc_Worker_IocDpc;

VOID
USBPORT_XdpcDisableAll(
    __inout PDEVICE_OBJECT HcFdo
    );


VOID
USBPORT_XdpcEnableAll(
    __inout PDEVICE_OBJECT HcFdo
    );


BOOLEAN
USBPORT_Xdpc_InsertQueueDpc(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal
    );
