//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: ssdev.c */

#ifdef HUB_WPP
#endif

VOID
UsbhClearPdoIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object
    );

VOID
UsbhSetPdoIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object
    );


VOID
UsbhInsertPdoIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );


VOID
UsbhRemovePdoIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );


PIRP
UsbhPeekNextPdoIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    );


VOID
UsbhAcquirePdoIdleIrpLock(
    PIO_CSQ Csq,
    PKIRQL  Irql
    );


VOID
UsbhReleasePdoIdleIrpLock(
    PIO_CSQ Csq,
    KIRQL Irql
    );


VOID
UsbhCompleteCanceledPdoIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );


ULONG
UsbhSS_WakeIrpRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


ULONG
UsbhIdleIrpRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


VOID
UsbhSS_PdoWakeWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


VOID
UsbhPdoIdleCC_Worker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


NTSTATUS
UsbhIdleIrp_ReleaseIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    NTSTATUS NtStatus
    );


NTSTATUS
UsbhIdleIrp_NoIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    );


NTSTATUS
UsbhIdleIrp_WaitWorker(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    );


NTSTATUS
UsbhIdleIrp_CB_Complete(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    );


NTSTATUS
UsbhIdleIrp_CB_Canceled(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    );


NTSTATUS
UsbhIdleIrp_CB_Pending(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    );


NTSTATUS
UsbhIdleIrp_Event(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    NTSTATUS CompleteStatus
    );


USB_IDLE_IRP_STATE
UsbhGetIdleIrpState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PKIRQL Irql
    );


VOID
UsbhSetIdleIrpState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE NewState,
    KIRQL Irql
    );


VOID
UsbhCompletePdoIdleIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS CompleteStatus
    );


VOID
UsbhSS_SignalPdoWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

