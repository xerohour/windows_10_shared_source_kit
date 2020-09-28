//
//    Copyright (C) Microsoft.  All rights reserved.
//

VOID
USBPORT_Ev_Rh_IntrEp_Close(
    __in PDEVICE_OBJECT HcFdo
    );

NTSTATUS
USBPORT_Ev_Rh_IntrEp_Open(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT IntrEp
    );

VOID
USBPORT_Ev_Rh_IntrEp_Invalidate(
    __in PDEVICE_OBJECT HcFdo
    );

KDEFERRED_ROUTINE USBPORT_Ev_Rh_IntrEp_Dpc;

VOID
USBPORT_Rh_Initialize(
    __in PDEVICE_OBJECT HcFdo
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Rh_TransferFromIrp(
    __in PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    );

VOID
USBPORT_RH_NeoQueueAsyncTransfer(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORT_Rh_CtrlEp_Worker(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

RHSTATUS
USBPORT_RootHub_Endpoint1_Peek(
    __in PDEVICE_OBJECT HcFdo,
    __in PHCD_ENDPOINT Endpoint,
    __inout PVOID IntPipeDataBuffer,
    __inout PULONG IntPipeDataBufferLength
    );

VOID
USBPORT_Rh_IntrEp_Worker(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_RhIntrEp_Insert_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp
    );

VOID
USBPORT_RhIntrEp_Remove_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp
    );

PIRP
USBPORT_RhIntrEp_PeekNext_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp,
    __inout_opt PVOID PeekContext
    );

__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_RhIntrEp_Acquire_TxqLock(
    __in PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    );
    
VOID
USBPORT_RhIntrEp_Release_TxqLock(
    __in PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    );

VOID
USBPORT_RhIntrEp_CompleteCanceled_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp
    );

NTSTATUS
USBPORT_HardResetRootPdo(
    __in PDEVICE_OBJECT HcFdo
    );

PDEVICE_OBJECT
USBPORT_RefRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in LONG RefOwnerTag
    );

VOID
USBPORT_DerefRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in PDEVICE_OBJECT RhPdo,
    __in LONG RefOwnerTag
    );

PDEVICE_OBJECT
USBPORT_SetRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in_opt PDEVICE_OBJECT NewRhPdoValue
    );

PDEVICE_EXTENSION
USBPORT_GetRootHubPdoExt(
    __in PDEVICE_OBJECT HcFdo
    );

RHSTATUS
USBPORT_RootHub_AsyncCommand(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    __in PUCHAR Buffer,
    __in ULONG BufferLength,
    __in PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORTSVC_Rh_Complete(
    __in PDEVICE_DATA DeviceData,
    __in PTRANSFER_PARAMETERS TransferParameters,
    __in USB_MINIPORT_STATUS  MpStatus,
    __in ULONG                BytesReturned
    );

USB_MINIPORT_STATUS
USBPORT_GetRootHubPortStatus(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __out PRH_PORT_STATUS PortStatus
    );

VOID
USBPORT_HcDisconnectRootHubPorts(
    __in PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_HcReconnectRootHubPorts(
    __in PDEVICE_OBJECT FdoDeviceObject
    );
