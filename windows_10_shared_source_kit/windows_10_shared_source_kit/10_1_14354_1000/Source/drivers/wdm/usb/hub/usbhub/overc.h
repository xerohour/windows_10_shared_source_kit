//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: overc.c */

#ifdef HUB_WPP
#endif

VOID
UsbhQueueOvercurrentReset(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhAutoOvercurrentResetWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


VOID
UsbhOvercurrentResetWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PSTATE_CONTEXT Sc,
    BOOLEAN AutoRecover
    );


VOID
UsbhDriverOvercurrentResetWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    );


VOID
UsbhQueueDriverOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhDeviceOvercurrentPopup(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );
