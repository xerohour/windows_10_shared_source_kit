//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: timer.c */


PHUB_TIMER_OBJECT
UsbhAllocateTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_ROUTINE TimeoutFunction,
    ULONG Tag
    );


VOID
UsbhInitializeTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_ROUTINE TimeoutFunction,
    PHUB_TIMER_OBJECT TimerObject,
    ULONG Tag
    );


VOID
UsbhFreeTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT *TimerObject
    );


VOID
UsbhEnableTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    LONG MillisecsToExpire,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    );


VOID
UsbhDisableTimerObject(
     PDEVICE_OBJECT HubFdo,
     PHUB_TIMER_OBJECT TimerObject
     );


NTSTATUS
UsbhTimerPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhTimerPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhDmTimer(
    PDEVICE_OBJECT HubFdo,
    PVOID Context
    );


VOID
UsbhDmStartTimer(
    PDEVICE_OBJECT HubFdo
    );


VOID
UsbhDmDisableTimer(
    PDEVICE_OBJECT HubFdo
    );


