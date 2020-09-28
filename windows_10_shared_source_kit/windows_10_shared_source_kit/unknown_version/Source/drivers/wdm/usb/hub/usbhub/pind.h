//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: pind.c */

#ifdef HUB_WPP
#endif

VOID
UsbhPindicatorWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


ULONG
UsbhPindicatorTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


VOID
UsbhDispatch_PindicatorEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PINDICATOR_EVENT PindicatorEvent,
    PSTATE_CONTEXT Context
    );


VOID
UsbhInitPortPindicator(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    );


VOID
UsbhSetPindicatorState(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData,
    HUB_PINDICATOR_STATE NewState
    );


HUB_PINDICATOR_STATE
UsbhLockPindicator(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData,
    HUB_PINDICATOR_EVENT Event
    );


VOID
UsbhUnlockPindicator(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData
    );


VOID
Usbh_PIND_SetBlink_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    );


VOID
Usbh_PIND_Disable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    );


VOID
Usbh_PIND_Enable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    );


VOID
Usbh_PIND_SetAuto_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    );


VOID
Usbh_PIND_Timeout_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhEvPIND_SetAuto(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT Sc
    );


VOID
UsbhEvPIND_SetBlink(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT Sc,
    USHORT LED_Color,
    ULONG DutyCycle
    );

VOID
UsbhEnablePortIndicators(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhDisablePortIndicators(
    PDEVICE_OBJECT HubFdo
    );

