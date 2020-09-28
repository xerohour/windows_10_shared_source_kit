//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: pdopwr.c */

#ifdef HUB_WPP
#endif

BOOLEAN
UsbhPdoArmedForWake(
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhPdoPower_Sequence(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPower_QueryPower(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPower_SetPower(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPower(
    PDEVICE_OBJECT HubPdo,
    PIRP Irp
    );

VOID
UsbhSetPdoPowerState(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    ULONG Data,
    HUB_PDO_POWER_STATE NewPdoPowerState,
    HUB_PDO_POWER_EVENT PdoPowerEvent
    );

NTSTATUS
UsbhSyncResumeDeviceInternal(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhPdoSetD0(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PHUB_POWER_CONTEXT PowerCtx
    );

VOID
UsbhPoStartNextPowerIrp_Pdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    ULONG Tag
    );

NTSTATUS
UsbhSyncSuspendPdoPort(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    BOOLEAN FlushWake
    );


NTSTATUS
UsbhPdoSetDx(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoSystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPower_WaitWake(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


VOID
UsbhInsertPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );


VOID
UsbhRemovePdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );


PIRP
UsbhPeekNextPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    );


VOID
UsbhAcquirePdoWakeIrpLock(
    PIO_CSQ Csq,
    PKIRQL  Irql
    );


VOID
UsbhReleasePdoWakeIrpLock(
    PIO_CSQ Csq,
    KIRQL Irql
    );

BOOLEAN
UsbhCompletePdoWakeIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS NtStatus
    );

VOID
UsbhCompleteCanceledPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhPdoUnblockPendedD0IrpWI(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    );

