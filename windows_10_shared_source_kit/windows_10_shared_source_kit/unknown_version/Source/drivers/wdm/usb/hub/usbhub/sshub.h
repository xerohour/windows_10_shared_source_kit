//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: sshub.c */

#ifdef HUB_WPP
#endif

__drv_maxIRQL(PASSIVE_LEVEL)
SSH_HUB_STATE
UsbhLockSSH(
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
	SSH_EVENT Event
    );

typedef enum _RESUME_HUB_FLAG {
    NO_RESUME_HUB = 0,
    RESUME_HUB
} RESUME_HUB_FLAG;

__drv_maxIRQL(PASSIVE_LEVEL)
PSSH_BUSY_HANDLE
UsbhIncHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in PVOID Object,
    __in ULONG Tag,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhDecHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __inout_opt PSSH_BUSY_HANDLE BusyHandle
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhFreeBusyList(
    __in PDEVICE_OBJECT HubFdo
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhUnlockSSH(
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
	SSH_HUB_STATE NewState
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_Event(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
	SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubActive(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSsh_CheckHubIdle(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingSuspend(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingResumeAfterSuspend(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubSuspended(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingResume(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubFailedResume(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubSSH_AddDeviceInit(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhHubSSH_PnpStart(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubSSH_PnpStop(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshDisabled(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnabled(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnterSx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshExitSx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshPortsBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshPortsIdle(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshSetPortsBusyState(
    __in PDEVICE_OBJECT HubFdo,
    __in USHORT PortNumber,
    __in __drv_strictTypeMatch(__drv_typeConst)
    PORT_BUSY_STATE PortBusyState,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    );

VOID
UsbhSShResumeWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );

VOID
UsbhHubSSH_Worker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );

ULONG
UsbhHubSSH_Timer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshPropagateResume(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in PDEVICE_OBJECT Pdo
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshResumePort(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __inout PHUB_PORT_DATA Pd
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshSuspendHub(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshResumeDownstream(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in BOOLEAN ResumeBus
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshResumeUpstream(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_BusIf_SuspendHub(
    __in PDEVICE_OBJECT HubFdo
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_BusIf_ResumeHub(
    __in PDEVICE_OBJECT HubFdo
    );

VOID
UsbhRegisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhUnregisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnableDisable(
    __in PDEVICE_OBJECT HubFdo,
    __in SSH_ENABLE_STATUS Enable
    );



NTSTATUS
UsbhSetHubIdle(
    PDEVICE_OBJECT HubFdo
    );

NTSTATUS
Usbh_SSH_SuspendHub(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

VOID
Usbh_SSH_ResumeHub(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

VOID
UsbhSSH_EnableDisable(
    PDEVICE_OBJECT HubFdo,
    BOOLEAN Enable
    );


VOID
UsbhArmHubWakeOnConnect(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhDisarmHubWakeOnConnect(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhUnregisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhRegisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    );

#if DBG
#define UsbhAssertSshLocked(HubFdo) UsbhAssert(HubFdo, !(KeReadStateSemaphore(&FdoExt(HubFdo)->SSH_Lock)))
#else
#define UsbhAssertSshLocked(HubFdo)
#endif


