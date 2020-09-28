//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: reset.c */

#ifdef HUB_WPP
#endif

NTSTATUS
UsbhResetHubUpstreamPort(
    PDEVICE_OBJECT HubFdo
    );


HARD_RESET_STATE
Usbh_HRS_Disabled(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    );


HARD_RESET_STATE
Usbh_HRS_Paused(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    );


HARD_RESET_STATE
Usbh_HRS_WaitReady(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    );


HARD_RESET_STATE
Usbh_HRS_Queued(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    );


HARD_RESET_STATE
UsbhDispatch_HardResetEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    );


VOID
UsbhEnableHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhDisableHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhPauseHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhResumeHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


NTSTATUS
UsbhHardReset_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

NTSTATUS
UsbhNoESD_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

VOID
UsbhHardResetAddDeviceInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhHardResetWorker(
    PDEVICE_OBJECT HubFdo,
    PVOID Context
    );


VOID
UsbhQueueHardReset(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );

VOID
UsbhResetNotifyCompletion(
    __in PDEVICE_OBJECT HubFdo  
    );

VOID
UsbhIncrementConcurrentResetCounter(
    __in PDEVICE_OBJECT HubFdo
    );

VOID
UsbhClearConcurrentResetCounter(
    __in PDEVICE_OBJECT HubFdo
    );

