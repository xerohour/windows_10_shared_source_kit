//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: hub.c */


NTSTATUS
UsbhSetTimer(
    PDEVICE_OBJECT HubFdo,
    ULONG MsTime,
    PKTIMER Timer,
    PKDPC Dpc,
    ULONG Tag,
    BOOLEAN ReferenceTimer
    );


ULONG
UsbhRevSig(
    ULONG Sig
    );


VOID
UsbhWait(
    PDEVICE_OBJECT HubFdo,
    ULONG Milliseconds
    );

VOID
UsbhRawWait(
    ULONG Milliseconds
    );


NTSTATUS
UsbhTrapBadIrp(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

#if DBG
ULONG
_cdecl
UsbhKdPrintX(
    __in ULONG Level,
    PCSTR Format,
    ...
    );

#endif

PHUB_PORT_DATA
UsbhGetPortData(
    PDEVICE_OBJECT HubFdo,
    USHORT Port
    );

NTSTATUS
UsbhWaitEventWithTimeout(
    __in PDEVICE_OBJECT HubFdo,
    __inout PKEVENT Event,
    __in LONG Milliseconds,
    __in ULONG LogSig
    );

USHORT
UsbhNumberOfPorts(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhDisconnectPdoFromHub(
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhConnectPdoToHub(
    PDEVICE_OBJECT Pdo,
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhSyncSendCommand(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PUSHORT BufferLength,
    ULONG MillisecondTimeout,
    USBD_STATUS *ReturnUsbdStatus
    );


__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
DRIVER_DISPATCH UsbhGenDispatch;
/*
NTSTATUS
UsbhGenDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );
*/


NTSTATUS
UsbhFdoSuccess(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoDeviceControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhSyncSendDeviceIoctl(
    PDEVICE_OBJECT HubFdo,
    ULONG Ioctl,
    PVOID IoBuffer,
    ULONG IoBufferLength,
    BOOLEAN Internal
    );


NTSTATUS
UsbhSyncSendInternalIoctl(
    PDEVICE_OBJECT HubFdo,
    ULONG Ioctl,
    PVOID Argument1,
    PVOID Argument2
    );


NTSTATUS
UsbhFdoReturnHubCount(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhGetControllerName(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_NAME HubNameBuffer,
    ULONG HubNameBufferLength
    );


NTSTATUS
UsbhFdoReturnControllerName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhGetDeviceBusInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_BUS_NOTIFICATION UserBuffer,
    ULONG UserBufferLength
    );


NTSTATUS
UsbhFdoReturnDeviceBusInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


VOID
UsbhOutOfBandwidthTimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    );


VOID
UsbhCancelOutOfBandwidthTimer(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


VOID
UsbhSetOutOfBandwidthTimer(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus
    );


ULONG
UsbhCalculateInterfaceBandwidth(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSBD_INTERFACE_INFORMATION UsbInterface
    );

IO_COMPLETION_ROUTINE UsbhSelectConfigOrInterfaceComplete;
/*
NTSTATUS
UsbhSelectConfigOrInterfaceComplete(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFdoSelectInterfacePdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoGetMsDescriptorPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoValidateConnectionPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoSelectConfigurationPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoUrbPdoFilter(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoCheckUpstreamConnectionState(
    PDEVICE_OBJECT HubFdo,
    PULONG IoctlPortStatus
    );


NTSTATUS
UsbhFdoReturnPortStatus(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    BOOLEAN ExApi
    );

NTSTATUS
UsbhReinitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );

NTSTATUS
UsbhFdoResetPdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoEnablePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoCyclePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnParentHubInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhGetHubPdoName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_HUB_NAME NameBuffer,
    ULONG NameBufferLength,
    PULONG LengthReturned
    );


NTSTATUS
UsbhFdoReturnHubName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnRootHubPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhGetRootHubPdo(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhLogStartFailure(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS FailStatus,
    USB_CONNECTION_STATUS ConnectStatus,
    ULONG Line,
    PCSTR SzFile,
    PCSTR SzDebugText
    );


NTSTATUS
UsbhGetHubCount(
    PDEVICE_OBJECT HubFdo,
    ULONG *Count
    );


NTSTATUS
UsbhFdoChainIrp(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoSystemControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );


IO_COMPLETION_ROUTINE UsbhDeferIrpCompletion;
/*
NTSTATUS
UsbhDeferIrpCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    );
*/


NTSTATUS
UsbhFlushPortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USBD_STATUS *UsbdStatus
    );


NTSTATUS
UsbhSetPortPower(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


NTSTATUS
UsbhSuspendPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


NTSTATUS
UsbhGetDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSHORT BufferLengthBytes,
    PVOID DescriptorBuffer,
    UCHAR DescriptorType,
    UCHAR DescriptorIndex,
    BOOLEAN ClassDescriptor
    );


NTSTATUS
UsbhGetStatus(
    PDEVICE_OBJECT HubFdo,
    PUSHORT StatusBits
    );


NTSTATUS
UsbhGetHubConfiguratonDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_CONFIGURATION_DESCRIPTOR *ConfigurationDescriptor
    );


NTSTATUS
UsbhSyncPowerOnPorts(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhGetHubClassDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_DESCRIPTOR HubDescriptor
    );


NTSTATUS
UsbhGetHubDeviceInformation(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


BOOLEAN
UsbhIsHighSpeedCapable(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhConfigureUsbHub(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhInitializeTtHub(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


ULONG
UsbhGetMaxPowerPerPort(
    PDEVICE_OBJECT HubFdo
    );


NTSTATUS
UsbhGetExtendedHubInformation(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhUninitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhInitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


PHUB_WORKITEM
UsbhAllocWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig,
    BOOLEAN ForwardProgress
    );


VOID
UsbhHubWorker(
    PDEVICE_OBJECT HubFdo,
    PVOID Context,
    PIO_WORKITEM IoWorkItem
    );


NTSTATUS
UsbhQueueWorkItem(
    PDEVICE_OBJECT HubFdo,
    WORK_QUEUE_TYPE QueueType,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    );


NTSTATUS
UsbhQueueWorkItemEx(
    PDEVICE_OBJECT HubFdo,
    WORK_QUEUE_TYPE QueueType,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig,
    PHUB_WORKITEM HubWi
    );


ULONG
UsbhQueueCriticalWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    );

ULONG
UsbhQueueDelayedWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    );


#ifdef _WIN64
#endif

PHUB_WORKITEM
UsbhFindPendingWorkItem(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    ULONG Sig
    );

NTSTATUS
UsbhFdoSuspendPdoPort(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );

HUB_FDO_PNPSTATE
UsbhAcquireFdoPnpLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_FDO_EVENT FdoEvent,
    ULONG Tag,
    ULONG LogIt
    );

VOID
UsbhInitStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    STATE_CONTEXT_TYPE CtxType,
    PVOID ParentObject
    );

PSTATE_CONTEXT
UsbhAllocateStateCtx(
    PDEVICE_OBJECT HubFdo,
    ULONG Tag
    );

VOID
UsbhAssertStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    ULONG Flags
    );

VOID
UsbhRefStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    ULONG Tag
    );


VOID
UsbhDerefStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    ULONG Tag
    );

PHUB_PORT_DATA
UsbhGetPortDataFromContext(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );

VOID
UsbhReleaseFdoPwrLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );

HUB_FDO_POWERSTATE
UsbhAcquireFdoPwrLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_FDO_EVENT FdoEvent,
    ULONG Tag
    );

NTSTATUS
UsbhFdoReturnTtDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhSetHubRemoteWake(
    PDEVICE_OBJECT HubFdo,
    BOOLEAN EnableUsbWake
    );

NTSTATUS
UsbhFdoInternalDeviceControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    );

NTSTATUS
UsbhQueryTopologyAddress(
    PDEVICE_OBJECT HubFdo
    );

__declspec(noreturn) void
UsbhTrapFatalTimeout_x9f(
    __in_opt PDEVICE_OBJECT HubFdo,
    __in USBHUB_FATAL_TIMEOUT_CODE TimeoutCode,
    __in_opt PVOID TimeoutContext
    );

NTSTATUS
UsbhWaitEventWithTimeoutEx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PKEVENT Event,
    __in LONG Milliseconds,
    __in ULONG LogSig,
    __in USBHUB_FATAL_TIMEOUT_CODE TimeoutCode,
    __in_opt PHUB_PORT_DATA pd
    );


VOID
USBHUB_FreeBugCheck(
    PKBUGCHECK_REASON_CALLBACK_RECORD BugCheckRecord
);

PKBUGCHECK_REASON_CALLBACK_RECORD
USBHUB_InitBugCheck(
    PKBUGCHECK_REASON_CALLBACK_ROUTINE BugCheckCallback
);

VOID
USBHUB_TriggerCallBacks (
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

extern PKBUGCHECK_REASON_CALLBACK_RECORD USBHUB_BugCheckPortArray;
extern PKBUGCHECK_REASON_CALLBACK_RECORD USBHUB_BugCheckHubExt;

#define USBHUB_InitAllBugChecks()\
    NT_ASSERT(USBHUB_BugCheckPortArray == NULL);\
    USBHUB_BugCheckPortArray = USBHUB_InitBugCheck(USBHUB_BugCheckSavePortArrayData);\
    NT_ASSERT(USBHUB_BugCheckHubExt == NULL);\
    USBHUB_BugCheckHubExt = USBHUB_InitBugCheck(USBHUB_BugCheckSaveHubExtData);\
    USBHUB_CurrentFdo = NULL;

#define USBHUB_FreeAllBugChecks()\
    USBHUB_FreeBugCheck(USBHUB_BugCheckPortArray);\
    USBHUB_FreeBugCheck(USBHUB_BugCheckHubExt);

VOID
USBHUB_BugCheckSavePortArrayData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    );

VOID
USBHUB_BugCheckSaveHubExtData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    );

extern PDEVICE_OBJECT USBHUB_CurrentFdo;
