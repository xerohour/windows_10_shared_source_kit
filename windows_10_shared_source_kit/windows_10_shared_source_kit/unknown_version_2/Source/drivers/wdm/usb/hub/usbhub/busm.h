//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: busm.c */

#ifdef HUB_WPP
#endif

HUB_BUS_STATE
UsbhAcquireBusStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
UsbhReleaseBusStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_STATE NewBusState
    );


HUB_BUS_STATE
UsbhGetContextBusState(
    PSTATE_CONTEXT Context
    );


HUB_BUS_STATE
Usbh_BS_BusInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
Usbh_BS_BusStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
Usbh_BS_BusRun(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
Usbh_BS_BusPause(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
Usbh_BS_BusSuspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );

HUB_BUS_STATE
Usbh_BS_BusRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


HUB_BUS_STATE
UsbhDispatch_BusEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    );


VOID
UsbhBusResume_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhBusDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusReset_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusPnpStop_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhBusPause_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusSuspend_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    );


HUB_BUS_STATE
UsbhSyncBusPause(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    BS_PAUSE_REASON PauseReason
    );


HUB_BUS_STATE
UsbhSyncBusResume(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    );


HUB_BUS_STATE
UsbhSyncBusReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    );


HUB_BUS_STATE
UsbhSyncBusDisconnect(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    );

VOID
UsbhBusDeviceRemove_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

HUB_BUS_STATE
UsbhSyncBusSuspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    );

VOID
UsbhSshBusBusy(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );

VOID
UsbhSshBusIdle(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    );