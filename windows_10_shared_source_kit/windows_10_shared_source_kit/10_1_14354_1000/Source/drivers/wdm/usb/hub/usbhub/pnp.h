//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: pnp.c */


//NTSTATUS
//UsbhCallModule(
//    BOOLEAN Start,
//    PHUB_MODULE_DISPATCH Dispatch,
//    PDRIVER_OBJECT DriverObject,
//    PDEVICE_OBJECT HubFdo,
//    HUB_MODULE_FUNCTION ModFunction,
//    PUNICODE_STRING UniRegistryPath
//    );


//NTSTATUS
//UsbhModuleDispatch(
//    BOOLEAN StartFlag,
//    PDRIVER_OBJECT DriverObject,
//    PDEVICE_OBJECT HubFdo,
//    HUB_MODULE_FUNCTION ModFunction,
//    PUNICODE_STRING UniRegistryPath
//    );

DRIVER_INITIALIZE DriverEntry;

VOID
UsbhInitCallbackWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PSTATE_CONTEXT Sc
    );

NTSTATUS
UsbhFdoPnp_QueryStop(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoPnp_QueryRemove(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
Usbh_FDO_WaitPnpAdd(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

NTSTATUS
Usbh_FDO_WaitPnpRemove(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

VOID
UsbhSetFdoPnpState(
    PDEVICE_OBJECT HubFdo,
    HUB_FDO_PNPSTATE NewFdoPnpState,
    HUB_FDO_EVENT FdoPnpEvent
    );

NTSTATUS
Usbh_FDO_WaitPnpStart(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

NTSTATUS
Usbh_FDO_WaitPnpStop(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

NTSTATUS
Usbh_FDO_WaitPnpStop_CB(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

NTSTATUS
UsbhAsyncStartComplete(
    PSTATE_CONTEXT Sc
    );

NTSTATUS
Usbh_FDO_Pnp_State(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

VOID
UsbhAsyncStop(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

NTSTATUS
UsbhHubStart(
    PSTATE_CONTEXT Sc,
    HUB_START_MODE Mode
    );

NTSTATUS
UsbhFdoPnp(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

VOID
UsbhAssignHubNumber(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhReleaseHubNumber(
    PDEVICE_OBJECT HubFdo
    );

NTSTATUS
UsbhPdoPnp(
    PDEVICE_OBJECT HubPdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_PassOnSuccess(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_DoDefault(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoPnp_DoDefault_HubNotInitialized(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_DeviceUsageNotification(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_QueryPnpDeviceState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_SurpriseRemove(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

VOID
UsbhInitGlobal(
     PUNICODE_STRING UniRegistryPath
     );

NTSTATUS
UsbhQueryBusRelations(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_QueryDeviceRelations(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


IO_COMPLETION_ROUTINE UsbhQueryCapsComplete;
/*
NTSTATUS
UsbhQueryCapsComplete(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoPnp_QueryCaps(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


DRIVER_UNLOAD UsbhDriverUnload;
/*
VOID
UsbhDriverUnload(
    PDRIVER_OBJECT DriverObject
    );
*/


DRIVER_ADD_DEVICE UsbhAddDevice;
/*
NTSTATUS
UsbhAddDevice(
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT PhysicalDeviceObject
    );
*/

NTSTATUS
Usbh_FDO_WaitPnpRestart(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    );

#if DBG
#endif

NTSTATUS
UsbhFdoPnp_StartDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoPnp_StopDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoPnp_RemoveDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhMakePdoName(
    PDEVICE_OBJECT HubFdo,
    PUNICODE_STRING PdoNameUnicodeString,
    ULONG Index
    );

__drv_dispatchType(IRP_MJ_SHUTDOWN)
DRIVER_DISPATCH UsbhDeviceShutdown;
/*
NTSTATUS
UsbhDeviceShutdown(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );
*/

IO_COMPLETION_ROUTINE UsbhResetNotificationIrpCompletion;

VOID
UsbhResetNotifyDownstreamHub(
    __in PDEVICE_OBJECT Pdo
);


ULONG
UsbhDriverNotFoundTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );
