//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: lock.c */

#ifdef HUB_WPP
#endif

VOID
UsbhInitStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    STATE_CONTEXT_TYPE CtxType,
    PVOID Parent
    );


VOID
UsbhAcquirePdoStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_PDO_HWPNP_EVENT PdoEvent
    );


VOID
UsbhReleasePdoStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


HUB_FDO_PNPSTATE
UsbhAcquireFdoPnpLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_FDO_EVENT FdoPnpEvent,
    ULONG Tag,
    ULONG LogIt
    );


VOID
UsbhReleaseFdoPnpLock(
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


VOID
UsbhReleaseFdoPwrLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


HARD_RESET_STATE
UsbhAcquireResetLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HARD_RESET_EVENT HardResetEvent
    );


HUB_BUS_STATE
UsbhReleaseResetLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HARD_RESET_STATE NewResetState
    );
