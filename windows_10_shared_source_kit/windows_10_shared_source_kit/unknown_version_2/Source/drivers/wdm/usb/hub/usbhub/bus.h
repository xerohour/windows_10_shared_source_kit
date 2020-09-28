//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: bus.c */

#ifdef HUB_WPP
#endif

VOID
UsbhBusIoInvalidateDeviceRelations(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhNotifyPnpOfFailure_Action(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS StartStatus
    );


NTSTATUS
UsbhFinishStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


ULONG
UsbhInitRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


VOID
Usbh_BusPnpFreeResources_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


ULONG
UsbHubhInitTimeoutTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


VOID
UsbhInitCallback(
    PSTATE_CONTEXT StateContext
    );




NTSTATUS
UsbhBusPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusAddInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhBusAdd(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


ULONG
UsbhResetIrqPipeRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


ULONG
UsbhIsrRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


IO_COMPLETION_ROUTINE UsbhHubIsr;
/*
NTSTATUS
UsbhHubIsr(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    );
*/


VOID
UsbhPostInterrupt(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhResetIrqPipe(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhQueryChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_HW_PORT_CHANGE PortChange,
    USBD_STATUS *ReturnUsbdStatus
    );


NTSTATUS
UsbhQueryPortState(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_STATE PortState,
    USBD_STATUS *ReturnUsbdStatus
    );


NTSTATUS
UsbhQueryHubState(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_STATE HubState,
    USBD_STATUS *ReturnUsbdStatus
    );


VOID
UsbhResetPortTimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    );


VOID
UsbhCancelResetTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    ULONG Control
    );

BOOLEAN
UsbhCancelQueuedTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );

#if DBG
#endif

VOID
UsbhDisablePort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );


NTSTATUS
UsbhResumePort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USBD_STATUS *UsbdStatus
    );


VOID
UsbhClearTt(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USHORT DeviceAddress
    );


VOID
UsbhResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPCQ_TIMEOUT_OBJECT TimeoutObject
    );


NTSTATUS
UsbhHubAckPortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_HW_PORT_CHANGE PortChange,
    USBD_STATUS *UsbdStatus
    );


NTSTATUS
UsbhHubAckHubChange(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_STATE HubChange,
    USBD_STATUS *UsbdStatus
    );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubResetIrqPipeWorker(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    );


VOID
UsbhHubIsrWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


VOID
UsbhIncrementHubIsrWorkerReference(
    __in PDEVICE_OBJECT HubFdo
    );


VOID
UsbhDecrementHubIsrWorkerReference(
    __in PDEVICE_OBJECT HubFdo
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhDecrementHubIsrWorkerReferenceAndWait(
    __in PDEVICE_OBJECT HubFdo
    );


__drv_requiresIRQL(PASSIVE_LEVEL)
VOID 
UsbhWaitWhileProcessingGlobalHubIsrList(
    __in PDEVICE_OBJECT HubFdo,
    __in PKEVENT WaitEvent
    );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubProcessIsr(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PSTATE_CONTEXT sc
    );


PUSB_DEVICE_HANDLE
UsbhGetDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PVOID Object
    );


UCHAR
UsbhGetPortStatusBits(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PUSB_HUB_PORT_STATE PortState,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhChangeRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


NTSTATUS
UsbhGetUsbDeviceFlags(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhSetupDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *ReturnUsbdStatus
    );


VOID
UsbhBusConnectPdo(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );


VOID
Usbh_BusPnpStop_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_BusRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_PdoSurpriseRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_PdoRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_Disconnect_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_EVENT PdoHwPnpEvent,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_ErrorDisconnect_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    );


VOID
Usbh_BusPause_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );


BOOLEAN
UsbhOnPdoPresentList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


BOOLEAN
UsbhOnPdoRemovedList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


BOOLEAN
UsbhOnPdoAbsentList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


BOOLEAN
UsbhFindListPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PLIST_ENTRY ListHead,
    PHGSL Lock
    );


VOID
UsbhDeregisterPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_STATE PdoState,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhSoftDisconnectPdo(
    PDEVICE_OBJECT HubFdo,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhAssertBusLock(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


VOID
UsbhAcquireEnumBusLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    );

VOID
UsbhReleaseEnumBusLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    );

VOID
UsbhReleaseEnumBusLockEx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber,
    BOOLEAN AssertLockOwned
    );

NTSTATUS
UsbhCreateDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USHORT PortStatus,
    USHORT PortNumber
    );


NTSTATUS
UsbhInitializeDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData
    );


VOID
UsbhSignalSyncDeviceReset(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    NTSTATUS NtStatus
    );

VOID
UsbhSignalSyncDeviceResetPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS NtStatus
    );

VOID
Usbh_iSignalSyncDeviceReset(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    NTSTATUS NtStatus
    );

NTSTATUS
UsbhSyncResetDeviceInternal(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhCycleDevicePort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    );

NTSTATUS
UsbhWaitForPortResume(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA pd,
    PSTATE_CONTEXT StateContext
    );

NTSTATUS
UsbhPortResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA Pd
    );

NTSTATUS
UsbhSyncResumePort(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    );

KIPI_BROADCAST_WORKER UsbhExecuteFreezeSystemRoutine;

VOID
UsbhWaitForBootDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG ConnectBit,
    ULONG ConnectChangeBit
    );

NTSTATUS
UsbhGetBillboardInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR BillboardDescriptor
    );

