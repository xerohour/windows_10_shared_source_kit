//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: fdopwr.c */

#ifdef HUB_WPP
#endif

NTSTATUS
UsbhFdoPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


BOOLEAN
UsbhCanSuspend(
    PDEVICE_OBJECT HubFdo,
    SYSTEM_POWER_STATE SystemState
    );


VOID
UsbhArmHubForWakeDetect(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhDisarmHubForWakeDetect(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhFdoPower_DoDefault(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    BOOLEAN CallPoStartNextPowerIrp
    );


NTSTATUS
UsbhFdoPower_PowerSequence(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPower_DoDefault(
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    BOOLEAN CallPoStartNextPowerIrp
    );


NTSTATUS
UsbhFdoPower_QueryPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoSetPowerD0_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PIRP Irp
    );

NTSTATUS
UsbhFdoQueryDevicePowerDxPoCompletion(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    );


IO_COMPLETION_ROUTINE UsbhFdoQuerySystemPowerSxIoCompletion;
/*
NTSTATUS
UsbhFdoQuerySystemPowerSxIoCompletion(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoQuerySystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoQueryDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


IO_COMPLETION_ROUTINE UsbhFdoWaitWake_IoCompletion;
/*
NTSTATUS
UsbhFdoWaitWake_IoCompletion(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoWakePoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    );


NTSTATUS
UsbhFdoPower_WaitWake(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPower_SetPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoScheduleDeferredPowerRequest(
    PDEVICE_OBJECT HubFdo,
    PHUB_WORKITEM_ROUTINE Handler,
    PHUB_POWER_CONTEXT PowerCtx,
    PIRP DeviceIrp
    );

NTSTATUS
UsbhFdoD0IoComplete_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    );


NTSTATUS
UsbhFdoDxPoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    );


NTSTATUS
UsbhFdoD0PoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    );


NTSTATUS
UsbhFdoSetPowerSx_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemPowerIrp,
    PHUB_POWER_CONTEXT PowerCtx
    );


IO_COMPLETION_ROUTINE UsbhFdoS0IoComplete_Action;
/*
NTSTATUS
UsbhFdoS0IoComplete_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemIrp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoSetPowerS0_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PHUB_POWER_CONTEXT PowerCtx
    );


IO_COMPLETION_ROUTINE UsbhFdoSxIoComplete_Action;
/*
NTSTATUS
UsbhFdoSxIoComplete_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemIrp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoSystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoColdStartCheckPort(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PHUB_PORT_DATA PortData
    );


NTSTATUS
UsbhFdoColdStartPdo(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData
    );


NTSTATUS
UsbhFdoSetD0Cold(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    BOOLEAN Reset
    );


NTSTATUS
UsbhFdoSetD0Warm(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhCompletePdoWakeIrps(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhCheckHubPowerStatus(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhFdoD0Worker_Action(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PHUB_POWER_CONTEXT PowerCtx
    );


NTSTATUS
UsbhFdoSetPowerDx_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_POWER_CONTEXT PowerCtx,
    PIRP Irp
    );

VOID UsbhFdoUnblockAllPendedPdoD0Irps( 
    __inout PDEVICE_OBJECT HubFdo
    );

