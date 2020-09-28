//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: pchange.c */

#ifdef HUB_WPP
#endif

VOID
UsbhUnlockPcq(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    KIRQL Irql
    );

VOID
UsbhUnlockPcqWithTag(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    KIRQL Irql,
    ULONG Tag
    );

HUB_PCQ_STATE
UsbhLockPcq(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_EVENT PcqEvent,
    KIRQL *Irql
    );

HUB_PCQ_STATE
UsbhLockPcqWithTag(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_EVENT PcqEvent,
    KIRQL *Irql,
    ULONG Tag
    );

VOID
UsbhSetPcqState(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_STATE NewState
    );

VOID
UsbhPCE_ResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );

VOID
Usbh_PCE_ResumeTimeout_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
UsbhDispatch_PortChangeQueueEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );

PVOID
UsbhDispatch_PortChangeQueueEventEx(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject,
    PVOID Parameter
    );


VOID
UsbhDispatch_PortChangeQueueNullEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_HardwareWake_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
UsbhPCE_ResumeDone(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );

VOID
Usbh_PCE_Disable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_ResumeDone_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
Usbh_PCE_SoftDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_Resume_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_Enable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_BusDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_Suspend_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    PNTSTATUS Parameter
    );


VOID
Usbh_PCE_BusReset_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_QueueChange_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );


VOID
Usbh_PCE_wRun_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );


VOID
Usbh_PCE_HW_Stop_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

PVOID
Usbh_PCE_wChange_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

PVOID
Usbh_PCE_Check_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    PULONG Parameter
    );

VOID
UsbhFreePortChangeQueueTimeoutObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPCQ_TIMEOUT_OBJECT TmoObj,
    BOOLEAN SignalCompletionEvent
    );

VOID
Usbh_PCE_psPAUSED_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
Usbh_PCE_psSUSPEND_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
Usbh_PCE_wChangeERROR_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );

VOID
Usbh_PCE_SD_Resume_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
Usbh_PCE_wDone_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
UsbhSetPcqEventStatus(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PCQEV_FIELD EventField,
    PCQEV_STATUS EventStatus
    );


NTSTATUS
UsbhInsertQueuePortChangeObject(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_EVENT PortEvent,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );


NTSTATUS
UsbhInsertQueuePortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_EVENT PortEvent
    );


VOID
UsbhInsertPauseChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


VOID
UsbhInitPortChangeList(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    );


PPORT_CHANGE_LOG_ENTRY
Usbh_PcLogEntry(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PVOID Pdo,
    ULONG PortEventUlong,
    ULONG FrameInserted,
    USHORT HwPortStatus,
    UCHAR PortStatusBits,
    UCHAR FlushFlag
    );


VOID
UsbhFlushPortChangeQueue(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );


VOID
UsbhPCE_Enable(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );


VOID
UsbhPCE_Resume(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );


VOID
UsbhPCE_HardwareWake(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );


VOID
UsbhPCE_BusReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );


VOID
UsbhPCE_BusDisconnect(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );


VOID
UsbhPCE_Disable(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    );

VOID
UsbhPCE_SD_Resume(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhPCE_psSUSPEND(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhPCE_psPAUSED(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );

ULONG
UsbhPCE_Check(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );

VOID
UsbhPCE_HW_Stop(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    );

VOID
UsbhPCE_QueueChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhPCE_QueueChangeObject(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT Sc,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );


VOID
UsbhQueuePauseChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhPCE_SoftDisconnect(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhQueueSoftConnectChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext,
    BOOLEAN DisablePort
    );


NTSTATUS
UsbhPCE_Suspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    );

VOID
UsbhPCE_wRun(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PHUB_PORT_DATA Pd
    );

VOID
UsbhRemoveQueuedSuspend(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );

ULONG
UsbhHubSyncSuspendPortEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData
    );


ULONG
UsbhHubDispatchPortEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPORT_CHANGE_CONTEXT PcData,
    PUSB_HUB_PORT_STATE PortState
    );


VOID
UsbhHubRunPortChangeQueue(
    PDEVICE_OBJECT   HubFdo,
    PHUB_PORT_DATA PortData,
    PVOID Context,
    PKIRQL Irql
    );

VOID
Usbh_PCE_Close_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    );

VOID
UsbhPCE_Close(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    );

VOID
UsbhHubProcessChangeWorker(
    PDEVICE_OBJECT   HubFdo,
    PVOID Context
    );

VOID
Usbh_PCE_QueueDriverReset_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    );

VOID
UsbhPCE_QueueDriverReset(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhFreePortChangeQueueObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPORT_CHANGE_CONTEXT PcData,
    BOOLEAN SignalCompletionEvent
    );

VOID
UsbhHubQueueProcessChangeWorker(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );

VOID
UsbhHubReferenceProcessChangeWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );

VOID
UsbhHubDereferenceProcessChangeWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );

VOID
UsbhHubDereferenceProcessChangeWorkItemAndWait(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );
