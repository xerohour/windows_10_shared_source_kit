/*++ 

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    hubutil.h

Abstract:

    inline utility functions.

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

/***********************************/

#ifndef   __HUBUTIL_H__
#define   __HUBUTIL_H__

VOID
__inline
Log(
    PDEVICE_OBJECT HubFdo,
    ULONG lmask,
    ULONG lsig,
    ULONG_PTR linfo1,
    ULONG_PTR linfo2
    )
{
    PDEVICE_EXTENSION_HUB hubExt;
    PDEBUG_LOG llog;
    extern ULONG UsbhLogMask;

    if (!(lmask & UsbhLogMask)) {
        return;
    }        
    
    if ((HubFdo != NULL) &&
        ((hubExt = HubFdo->DeviceExtension) != NULL)) {

        llog = &hubExt->Log;
        UsbhAddLogEntry(llog, (lmask), (lsig), (ULONG_PTR)NULL, (linfo1), (linfo2));
    }
}

BOOLEAN
__inline
IsBitSet(
    PVOID Bitmap,
    ULONG BitNumber
    )
/*++
Routine Description:

    Check if a bit is set given a string of bytes.

Arguments:


Return Value:

 TRUE - if the corresponding bit is set. FALSE - otherwise

-- */
{
    ULONG dwordOffset;
    ULONG bitOffset;
    PUCHAR l = (PUCHAR) Bitmap;

    dwordOffset = BitNumber / 8;
    bitOffset = BitNumber % 8;

    return ((l[dwordOffset] & (1 << bitOffset)) ? TRUE : FALSE);

}


HUB_PDO_HWPNP_STATE
__inline
GET_PDO_HWPNPSTATE(
    PDEVICE_OBJECT Pdo
    )
{
    return PdoExt(Pdo)->CurrentPdoHwPnpState;
}


HUB_PDO_SWPNP_STATE
__inline
GET_PDO_SWPNPSTATE(
    PDEVICE_OBJECT Pdo
    )
{
    return PdoExt(Pdo)->CurrentPdoSwPnpState;
}


HUB_FDO_PNPSTATE
__inline
GET_FDO_PNPSTATE(
    PSTATE_CONTEXT Sc
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    // we should be holding the lock when we read state
    UsbhAssert(Sc->HubFdo, hubExt->CurrentFdoPnpContext == Sc);

    return FdoExt(Sc->HubFdo)->CurrentFdoPnpState;
}


HUB_FDO_POWERSTATE
__inline
GET_FDO_POWER_STATE(
    PSTATE_CONTEXT Sc
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    // we should be holding the lock when we read state
    UsbhAssert(Sc->HubFdo, hubExt->CurrentFdoPwrContext == Sc);

    return FdoExt(Sc->HubFdo)->CurrentFdoPwrState;
}


VOID
__inline
SET_PDO_SWPNPSTATE(
    PDEVICE_OBJECT Pdo,
    HUB_PDO_SWPNP_STATE NewPdoState,
    HUB_PDO_SWPNP_EVENT PdoEvent
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PG_STATE_LOG_ENTRY pdoPnpLog;

    // sets the software pnp state for things like stop and start
    pdoExt = PdoExt(Pdo);

    pdoExt->PnpLogIdx=(pdoExt->PnpLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    pdoPnpLog = &pdoExt->PnpLog[pdoExt->PnpLogIdx];

    pdoPnpLog->Event = PdoEvent;
    pdoPnpLog->CurrentState = pdoExt->CurrentPdoSwPnpState;
    pdoPnpLog->NextState = NewPdoState;

    pdoExt->CurrentPdoSwPnpState = NewPdoState;

}



VOID
__inline
SET_PDO_HWPNPSTATE(
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_STATE NewPdoState,
    HUB_PDO_HWPNP_EVENT PdoEvent
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PG_STATE_LOG_ENTRY pdoLog;

    pdoExt = PdoExt(Pdo);

    pdoExt->PdoLogIdx=(pdoExt->PdoLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    pdoLog = &pdoExt->PdoLog[pdoExt->PdoLogIdx];

    pdoLog->Event = PdoEvent;
    pdoLog->CurrentState = pdoExt->CurrentPdoHwPnpState;
    pdoLog->NextState = NewPdoState;

    pdoExt->CurrentPdoHwPnpState = NewPdoState;

}


HUB_PDO_POWER_STATE
__inline
GET_PDO_POWER_STATE(
    PDEVICE_OBJECT Pdo
    )
{
    return PdoExt(Pdo)->CurrentPdoPowerState;
}


VOID
__inline
UsbhAssertFdoPnpContextLock(
    PSTATE_CONTEXT Sc
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;

    // asserts that the give context holds the FDO/PDO power lock

    // make sure its a context
    ASSERT_HUB_CONTEXT(Sc);

    hubExt = FdoExt(Sc->HubFdo);

    // pnp state ownership?
    UsbhAssert(Sc->HubFdo, Sc->FdoPnpSig == FdoPnpLock_Sig_FDO1);
    UsbhAssert(Sc->HubFdo, hubExt->CurrentFdoPnpContext == Sc);

}


VOID
__inline
UsbhAssertFdoPwrContextLock(
    PSTATE_CONTEXT Sc
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;

    // asserts that the give context holds the FDO/PDO power lock

    // make sure its a context
    ASSERT_HUB_CONTEXT(Sc);

    hubExt = FdoExt(Sc->HubFdo);

    // power state ownership?
    UsbhAssert(Sc->HubFdo, Sc->FdoPwrSig == FdoPwrLock_Sig_FDO2);
    UsbhAssert(Sc->HubFdo, hubExt->CurrentFdoPwrContext == Sc);

}


VOID
__inline
UsbhAssertBusStateContext(
    PSTATE_CONTEXT Sc
    )
/*
Sanity check the state context for Fdo State, validates lock is held
*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    // make sure its a context
    ASSERT_HUB_CONTEXT(Sc);

    hubExt = FdoExt(Sc->HubFdo);

    // bus state ownership?
    UsbhAssert(Sc->HubFdo, Sc->BusSig == SIG_BUS_CONTEXT);
    UsbhAssert(Sc->HubFdo, hubExt->CurrentBusStateContext == Sc);

}


VOID
__inline
UsbhAssertResetStateContext(
    PSTATE_CONTEXT Sc
    )
/*
Sanity check the state context for Reset State, validates lock is held
*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    // make sure its a context
    ASSERT_HUB_CONTEXT(Sc);

    hubExt = FdoExt(Sc->HubFdo);

    // bus state ownership?
    UsbhAssert(Sc->HubFdo, Sc->HardResetSig == HardResetLock_Sig_RSTx);
    UsbhAssert(Sc->HubFdo, hubExt->CurrentResetContext == Sc);

}




#define Usbh_PnpCtxFDO_Pnp_State(HubFdo, FdoEvent) \
    Usbh_FDO_Pnp_State(PNP_CONTEXT((HubFdo)), (FdoEvent))

#define SET_FDO_DX(HubFdo, ps)\
    FdoExt((HubFdo))->Fdo_Dx = (ps)

#define GET_FDO_DX(HubFdo)\
    FdoExt((HubFdo))->Fdo_Dx

#define SET_FDO_SX(HubFdo, ps)\
    FdoExt((HubFdo))->Fdo_Sx = (ps)

#define GET_FDO_SX(HubFdo)\
    FdoExt((HubFdo))->Fdo_Sx


VOID
__inline
UsbhSet_Pdo_Dx(
    PDEVICE_OBJECT Pdo,
    DEVICE_POWER_STATE Dx
    )
{
    POWER_STATE ps;

    ps.DeviceState = Dx;
    PdoExt((Pdo))->Pdo_Dx = Dx;
    PoSetPowerState(Pdo,
                    DevicePowerState,
                    ps);

    // Update the ports busy mask for hub selective suspend
    switch (Dx) {
    case PowerDeviceD0:
        UsbhSshSetPortsBusyState(PdoExt((Pdo))->HubFdo, PdoExt((Pdo))->PortNumber, PORT_BUSY, NO_RESUME_HUB);
        break;
    case PowerDeviceD1:
    case PowerDeviceD2:
    case PowerDeviceD3:
        UsbhSshSetPortsBusyState(PdoExt((Pdo))->HubFdo, PdoExt((Pdo))->PortNumber, PORT_IDLE, NO_RESUME_HUB);
    }
}

#define GET_PDO_DX(Pdo)\
    PdoExt((Pdo))->Pdo_Dx



/*

Connection Status value state map

    CurrentState                NextState
--------------------------------------------------------------------------
    NoDeviceConnected           DeviceEnumerating
                                DeviceConnected
                                DeviceGeneralFailure        ; no PDO
..........................................................................
    DeviceEnumerating           DeviceConnected
                                DeviceFailedEnumeration
                                DeviceEnumerating
                                DeviceCausedOvercurrent
                                DeviceNotEnoughPower
                                DeviceNotEnoughBandwidth
                                DeviceHubNestedTooDeeply
                                DeviceInLegacyHub
...........................................................................
    DeviceConnected             DeviceFailedEnumeration
                                DeviceEnumerating
                                DeviceGeneralFailure,
                                DeviceCausedOvercurrent
                                DeviceNotEnoughPower
                                DeviceNotEnoughBandwidth
                                DeviceHubNestedTooDeeply
                                DeviceInLegacyHub


*/
VOID
__inline
UsbhSetConnectionStatus(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USB_CONNECTION_STATUS ConnectionStatus
    )
{
    USB_CONNECTION_STATUS oldStatus;

    UNREFERENCED_PARAMETER(HubFdo);

    oldStatus = (USB_CONNECTION_STATUS) InterlockedExchange((PLONG)&PortData->ConnectionStatus,
                                    ConnectionStatus);

    if (ConnectionStatus == DeviceConnected) {
        UsbhAssert(HubFdo, oldStatus == NoDeviceConnected ||
                           oldStatus == DeviceEnumerating ||
                           oldStatus == DeviceNotEnoughBandwidth ||
                           oldStatus == DeviceReset);
    }

}


BOOLEAN
__inline
UsbhEventSignaled(
    PKEVENT Event
    )
{
    // returns TRUE if event is signalled

    if (KeReadStateEvent(Event)) {
        // nonzero means signalled
        return TRUE;
    } else {
        return FALSE;
    }
}


VOID
__inline
UsbhSetPdo_FailIo(
    PDEVICE_OBJECT Pdo
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;
    KIRQL irql;

    pdoExt = PdoExt(Pdo);

    // no more io will be processed for this Pdo,
    // start failing as soon as we possibly can
    KeAcquireSpinLock(&pdoExt->IoLock, &irql);
    pdoExt->IoState = Pdo_FailIo;
    KeReleaseSpinLock(&pdoExt->IoLock, irql);

}

VOID
__inline
UsbhSetPdo_AllowIo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;
    KIRQL irql;
    HUB_PDO_HWPNP_STATE hwState;

    pdoExt = PdoExt(Pdo);

    // take the Pdo state lock so the we do't change state while we
    // set the flag

    UsbhAcquirePdoStateLock(HubFdo, PDO_CONTEXT(Pdo), PdoEv_Internal);

    // the only valid states here are Pdo_PnpRefHwPresent, Pdo_PnpRefHwAbsent
    hwState = GET_PDO_HWPNPSTATE(Pdo);
    UsbhAssert(HubFdo, hwState == Pdo_PnpRefHwPresent ||
                       hwState == Pdo_PnpRefHwAbsent);

    if (hwState == Pdo_PnpRefHwPresent) {
        // Allow IO to this PDO
        KeAcquireSpinLock(&pdoExt->IoLock, &irql);
        pdoExt->IoState = Pdo_AllowIo;
        KeReleaseSpinLock(&pdoExt->IoLock, irql);
    }
    UsbhReleasePdoStateLock(HubFdo, PDO_CONTEXT(Pdo));

}


ULONG
__inline
UsbhLogSignalResumeEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_ResumePending
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG signal = 0;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    if (Assert_ResumePending) {
        UsbhAssert(HubFdo, Pd->PortHwPendingState == PortHw_ResumePending);
    }

    LOG(HubFdo, LOG_POWER, 'siRS', Pd->PortHwPendingState, Pd->PortNumber);
    if (Pd->PortHwPendingState == PortHw_ResumePending) {
       // should not be signalled
       LOG(HubFdo, LOG_POWER, 'siRx', Pd->PortHwPendingState, Pd->PortNumber);
       UsbhAssert(HubFdo, !UsbhEventSignaled(&Pd->PortResumeEvent));
       Pd->PortHwPendingState = PortHw_Waiting;
       SET_EVENT(&Pd->PortResumeEvent);
       signal = 1;
       //UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvResume, PcqEv_Signal);
    }
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

    return signal;
}


VOID
__inline
UsbhiSignalResumeEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_ResumePending
    )
{
    ULONG s;

    s = UsbhLogSignalResumeEvent(HubFdo, Pd, Assert_ResumePending);
    if (s) {
        //KeAcquireSpinLock(&PortData->PortChangeListSpin, &irql);
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvResume, PcqEv_Signal);
        //KeReleaseSpinLock(&PortData->PortChangeListSpin, irql);
    }
}


VOID
__inline
UsbhSignalResumeEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_ResumePending
    )
{
    KIRQL irql;
    ULONG s;

    s = UsbhLogSignalResumeEvent(HubFdo, Pd, Assert_ResumePending);
    if (s) {
        KeAcquireSpinLock(&Pd->PortChangeListSpin, &irql);
        Pd->PortChangeListLock = 1;
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvResume, PcqEv_Signal);
        Pd->PortChangeListLock = 0;
        KeReleaseSpinLock(&Pd->PortChangeListSpin, irql);
    }
}



VOID
__inline
UsbhSetResumePending(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvResume, PcqEv_Reset);
    // port suspend is pending
    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    UsbhAssert(HubFdo, ((Pd->PortHwPendingState == PortHw_Waiting) || (Pd->PortHwPendingState == PortHw_ResumePending)));
    Pd->ResumeTimeout = FALSE;
    Pd->PortHwPendingState = PortHw_ResumePending;
    RESET_EVENT(&Pd->PortResumeEvent);
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
}


ULONG
__inline
UsbhLogSignalSuspendEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_SuspendPending
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG signal = 0;
    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    if (Assert_SuspendPending) {
        UsbhAssert(HubFdo, Pd->PortHwPendingState == PortHw_SuspendPending);
    }

    LOG(HubFdo, LOG_POWER, 'siSU', 0, Pd->PortNumber);
    if (Pd->PortHwPendingState == PortHw_SuspendPending) {
       // should not be signalled
       UsbhAssert(HubFdo, !UsbhEventSignaled(&Pd->PortSuspendEvent));
       Pd->PortHwPendingState = PortHw_Waiting;
       SET_EVENT(&Pd->PortSuspendEvent);
       //UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvSuspend, PcqEv_Signal);
       signal = 1;
    }
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

    return signal;
}


VOID
__inline
UsbhSignalSuspendEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_SuspendPending
    )
{
    KIRQL irql;
    ULONG s;

    s = UsbhLogSignalSuspendEvent(HubFdo, Pd, Assert_SuspendPending);
    if (s) {
        KeAcquireSpinLock(&Pd->PortChangeListSpin, &irql);
        Pd->PortChangeListLock = 1;
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvSuspend, PcqEv_Signal);
        Pd->PortChangeListLock = 0;
        KeReleaseSpinLock(&Pd->PortChangeListSpin, irql);
    }
}


VOID
__inline
UsbhiSignalSuspendEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_SuspendPending
    )
{
    ULONG s;

    s = UsbhLogSignalSuspendEvent(HubFdo, Pd, Assert_SuspendPending);
    if (s) {
        //KeAcquireSpinLock(&PortData->PortChangeListSpin, &irql);
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvSuspend, PcqEv_Signal);
        //KeReleaseSpinLock(&PortData->PortChangeListSpin, irql);
    }
}


VOID
__inline
UsbhSetSuspendPending(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

     // port suspend is pending
    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    switch (Pd->PortHwPendingState) {
    case PortHw_SuspendPending:
        // suspend is currently pending. just wait for the current
        // suspend to complete

        KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
        break;

    default:

        UsbhAssert(HubFdo, Pd->PortHwPendingState == PortHw_Waiting);
        Pd->PortHwPendingState = PortHw_SuspendPending;
        RESET_EVENT(&Pd->PortSuspendEvent);

        KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvSuspend, PcqEv_Reset);
        break;
    }

}


VOID
__inline
UsbhSignalPauseEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_POWER, 'siPS', 0, Pd->PortNumber);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    if (Pd->PausePending) {
        // should not be signalled here
        UsbhAssert(HubFdo, !UsbhEventSignaled(&Pd->PortPausedEvent));
        Pd->PausePending = NULL;
        SET_EVENT(&Pd->PortPausedEvent);
    }
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
}


// returns true if the event is reset
BOOLEAN
__inline
UsbhSetPausePending(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    LOG(HubFdo, LOG_POWER, 'pspm', 0, Pd->PortNumber);
    // if PortPausedEvent == signaled
    //     pausePending = TRUE
    if (!UsbhEventSignaled(&Pd->PortPausedEvent)) {
        // pause already pending

        KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
        return FALSE;
    }

    RESET_EVENT(&Pd->PortPausedEvent);
    Pd->PausePending = Pd;
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

    return TRUE;
}


NTSTATUS
__inline
UsbhAllocateTimeoutObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    PPCQ_TIMEOUT_OBJECT *TmoObject
    )
{
    PPCQ_TIMEOUT_OBJECT tmoObj;
    NTSTATUS nts;

    UsbhAllocatePool_Z(tmoObj, NonPagedPool, sizeof(struct _PCQ_TIMEOUT_OBJECT));
    if (tmoObj == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
        *TmoObject = NULL;
        LOG(HubFdo, LOG_BUSF, 'ato!', Pd, 0);
    } else {
        tmoObj->Sig = SIG_TMOOBJ;
        tmoObj->ResetState = Pd->CurrentState;
        *TmoObject = tmoObj;
        nts = STATUS_SUCCESS;
    }

    return nts;
}

/*
It is not quite clear what the 'disconnect' status code for NT is.
-- We use a macro in case we need to change it.
*/
#define NT_DEVICE_DISCONNECTED_STATUS STATUS_NO_SUCH_DEVICE


BOOLEAN
__inline
Usb_Disconnected(
    NTSTATUS NtStatus
    )
{
    if (NtStatus == NT_DEVICE_DISCONNECTED_STATUS ||
        // port driver returs this status and it may be unsafe to
        // change this
        NtStatus == STATUS_DEVICE_NOT_CONNECTED) {
        return TRUE;
    } else {
        return FALSE;
    }
}


PHUB_POWER_CONTEXT
__inline
UsbhAcquirePowerContext(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT DevObject,
    PIRP SystemIrp,
    PIRP DeviceIrp,
    HUB_POWER_REQUEST_TYPE PowerReqType,
    BOOLEAN StoreContext
    )
{
    PHUB_POWER_CONTEXT pc;
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    SYSTEM_POWER_STATE systemState;
    PHGSL lock;

    hubExt = FdoExt(HubFdo);
    UsbhAllocatePool_Z(pc, NonPagedPool, sizeof(struct _HUB_POWER_CONTEXT));

    if (pc == NULL) {
        return pc;
    }

    pc->Sig = SIG_POWER_CTX;
    pc->PowerReqType = PowerReqType;
    pc->DevObject = DevObject;
    pc->HubFdo = HubFdo;

    if (SystemIrp) {
        ioStack = IoGetCurrentIrpStackLocation(SystemIrp);
        systemState = ioStack->Parameters.Power.State.SystemState;
        // Allocate and intialize a power context for the FDO
        pc->SystemState = systemState;
        pc->SystemIrp = SystemIrp;
        UsbhEtwGetActivityId(SystemIrp, &pc->ActivityId);
    }

    if (DeviceIrp) {
        ioStack = IoGetCurrentIrpStackLocation(DeviceIrp);
        deviceState = ioStack->Parameters.Power.State.DeviceState;
        // Allocate and intialize a power context for the FDO
        pc->DeviceState = deviceState;
        pc->DeviceIrp = DeviceIrp;
    }

    pc->Sc = Sc;
    pc->Thread = KeGetCurrentThread();

    lock = &Sc->gLock;
    LockGSL(lock);

    if (StoreContext) {
        UsbhAssert(HubFdo, hubExt->CurrentFdoPwrCtx == NULL);
        hubExt->CurrentFdoPwrCtx = pc;
        pc->ContextStored = TRUE;
    }

    InsertTailList(&HubG.HubPowerCtxListHead, &pc->PwrCtxLink);

    unLockGSL(lock);

    return pc;
}


VOID
__inline
UsbhReleasePowerContext(
    PDEVICE_OBJECT HubFdo,
    PHUB_POWER_CONTEXT PowerCtx
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHGSL lock;

    hubExt = FdoExt(HubFdo);

    lock = &(PowerCtx->Sc)->gLock;

    LockGSL(lock);

    if (PowerCtx->ContextStored) {
        UsbhAssert(HubFdo, hubExt->CurrentFdoPwrCtx == PowerCtx);
        hubExt->CurrentFdoPwrCtx = NULL;
    }

    RemoveEntryList(&PowerCtx->PwrCtxLink);
    UsbhFreePool(PowerCtx);

    unLockGSL(lock);

}


#define REF_HUB(Fdo, Obj, Tag) \
    UsbhReferenceListAdd((Fdo),(Obj),(Tag))

#define DEREF_HUB(Fdo, Obj) \
    UsbhReferenceListRemove((Fdo),(Obj))


ULONG
__inline
UsbhLogSignalDriverResetEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_DriverResetPending
    )
{
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG signal = 0;

    UNREFERENCED_PARAMETER(Assert_DriverResetPending);

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    //if (Assert_DriverResetPending) {
    //    UsbhAssert(HubFdo, Pd->PortHwPendingState == PortHw_ResumePending);
    //}

    LOG(HubFdo, LOG_POWER, 'drRS', Pd->PortHwPendingState, Pd->PortNumber);
    SET_EVENT(&Pd->DriverResetEvent);
    
    //UsbhAssert(HubFdo, Pd->PortHwPendingState == PortHw_Waiting);
    signal = 1;

    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

    return signal;
}


VOID
__inline
Usbh_iSignalDriverResetEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_DriverResetPending
    )
{
    ULONG s;

    s = UsbhLogSignalDriverResetEvent(HubFdo, Pd, Assert_DriverResetPending);
    if (s) {
        //KeAcquireSpinLock(&PortData->PortChangeListSpin, &irql);
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvDriverReset, PcqEv_Signal);
        //KeReleaseSpinLock(&PortData->PortChangeListSpin, irql);
    }
}


VOID
__inline
UsbhSignalDriverResetEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    BOOLEAN Assert_DriverResetPending
    )
{
    KIRQL irql;
    ULONG s;

    LOG(HubFdo, LOG_POWER, 'drR1', Pd, Pd->PortNumber);
    s = UsbhLogSignalDriverResetEvent(HubFdo, Pd, Assert_DriverResetPending);
    if (s) {
        KeAcquireSpinLock(&Pd->PortChangeListSpin, &irql);
        UsbhAssert(HubFdo, Pd->PortChangeListLock == 0);
        Pd->PortChangeListLock = 1;
        UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvDriverReset, PcqEv_Signal);
        Pd->PortChangeListLock = 0;
        KeReleaseSpinLock(&Pd->PortChangeListSpin, irql);
    }
}



VOID
__inline
Usbh_iSetDriverResetPending(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
{
    //hubExt = FdoExt(HubFdo);
    UsbhSetPcqEventStatus(HubFdo, Pd, PcqEvDriverReset, PcqEv_Reset);
    // port suspend is pending
    //KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    UsbhAssert(HubFdo, Pd->PortHwPendingState != PortHw_SuspendPending);
    UsbhAssert(HubFdo, Pd->PortHwPendingState != PortHw_ResetPending);
    RESET_EVENT(&Pd->DriverResetEvent);
    //KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
}


VOID
__inline
Usbh_CheckPortHwPendingStatus(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    BOOLEAN bWait
    )
{
KIRQL rs_irql;
PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);

    switch (PortData->PortHwPendingState) {
       case PortHw_ResumePending:
           KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
           LOG(HubFdo, LOG_POWER, 'cPP1', HubFdo, PortData->PortNumber);

           if (bWait) {
               UsbhWaitEventWithTimeout(HubFdo,
                                        &PortData->PortResumeEvent,
                                        USBHUB_X9F_TIMEOUT,
                                        'cPP1');
           } else {
               UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
           }

           break;

       case PortHw_SuspendPending:
           KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
           LOG(HubFdo, LOG_POWER, 'cPP2', HubFdo, PortData->PortNumber);
           if (bWait) {
               UsbhWaitEventWithTimeout(HubFdo,
                                        &PortData->PortSuspendEvent,
                                        USBHUB_X9F_TIMEOUT,
                                        'cPP2');
           } else {
               UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
           }
           break;

       case PortHw_Waiting:
       default:
           KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
           break;
   }
}


VOID
__forceinline
UsbhSetCompletionRoutine(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PIO_COMPLETION_ROUTINE CompletionRoutine,
    PVOID Context,
    BOOLEAN InvokeOnSuccess,    
    BOOLEAN InvokeOnError,
    BOOLEAN InvokeOnCancel
    )
/*
Routine Description:

    This function is a wrapper for setting completion routine on
    an IRP.It first tries to call IoSetCompletionRoutineEx and if 
    that fails, falls back to using IoSetCompletionRoutine. Refer 
    to the documentation of IoSetCompletionRoutineEx to understand
    why it is better than IoSetCompletionRoutine.  
    
    NOTE: Since this function uses IoSetCompletionRoutineEx, it
    is mandatory that this function is followed by IoCallDriver,
    otherwise memory allocated during the call will be leaked.

Arguments:
    
    Same arguments as IoSetCompletionRoutineEx

Return Value:

    Void

*/
{
    if(!NT_SUCCESS(IoSetCompletionRoutineEx(DeviceObject,
                                            Irp,
                                            CompletionRoutine,
                                            Context,
                                            InvokeOnSuccess,
                                            InvokeOnError,
                                            InvokeOnCancel))) {
        IoSetCompletionRoutine(Irp,
                               CompletionRoutine,
                               Context,
                               InvokeOnSuccess,
                               InvokeOnError,
                               InvokeOnCancel);

    }

}


VOID
_inline
Usbh_OvercurrentRefHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __in PHUB_PORT_DATA PortData,
    __in BOOLEAN Auto
    )
/*
Routine Description:

    Increments the hub busy list so the hub doesn't enter selective suspend
    while it has a port pending overcurrent reset/recovery.

Arguments:

    HubFdo - FDO for the hub.

    PortData - Port data structure for the overcurrent port.

    Auto - Indicates that the port is being auto-recovered vs. the using popup UI.
           
Return Value:

    Void

*/
{
    WAIT_EVENT(&PortData->OvercurrentLock); 
    
    // Only increment hub busy if we don't have an overcurrent reference
    if ((PortData->OvercurrentHubBusyRefCount == 0) && 
        (PortData->OvercurrentPopup == FALSE)) {
        UsbhAssert(HubFdo, PortData->OvercurrentSshHandle == NULL);
        PortData->OvercurrentSshHandle = UsbhIncHubBusy(HubFdo,
                                                        PDOC(PortData),
                                                        PortData,
                                                        UsbhOverCurrentBusy_Tag,
                                                        NO_RESUME_HUB);
    }

    // Determine if the overcurrent reference count should be incremented versus 
    // setting OvercurrentPopup to TRUE.  The need for both a reference count and a 
    // boolean is due to the 2 different ways that overcurrent is handled by the hub driver.
    // If the port has a PDO that has successfully started, the hub driver will fire a WMI
    // event that brings up the USB popup UI.  The port will not be recovered until the user
    // hits the "reset" button.  If the port does not have a started PDO, it will attempt to
    // recover the port automatically.  
    // There is a 1-to-1 correspondence between the number of overcurrent change events that
    // trigger auto-recovery and the number of work-items queued that recover the port.  We
    // need to maintain a reference count of these and keep the hub active until they have all
    // run.  that call UsbhOvercurrentResetWorker(), which recovers the port.  For the popup UI
    // there can be multiple overcurrent change events that fire the WMI event, but only one  
    // request from the UI to reset the port.  Therefore a BOOLEAN is used indicate that the hub
    // should remain active until the user requests the port to reset.
    if (Auto == TRUE) {
        PortData->OvercurrentHubBusyRefCount++;
    } else {
        PortData->OvercurrentPopup = TRUE;
    }

    SET_EVENT(&PortData->OvercurrentLock); 
}


VOID
_inline
Usbh_OvercurrentDerefHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __in PHUB_PORT_DATA PortData,
    __in BOOLEAN Auto
    )
/*
Routine Description:

    Decrements the hub busy list if it has no overcurrent references so it can
    enter selective suspend.

Arguments:

    HubFdo - FDO for the hub.

    PortData - Port data structure for the overcurrent port.

    Auto - Indicates that the port is being auto-recovered vs. the using popup UI.

Return Value:

    Void

*/
{
    WAIT_EVENT(&PortData->OvercurrentLock); 

    if (Auto == TRUE) {
        PortData->OvercurrentHubBusyRefCount--;
    } else {
        PortData->OvercurrentPopup = FALSE;
    }

    if ((PortData->OvercurrentHubBusyRefCount == 0) && 
       (PortData->OvercurrentPopup == FALSE)) {
        UsbhAssert(HubFdo, PortData->OvercurrentSshHandle != NULL);
        UsbhDecHubBusy(HubFdo, PDOC(PortData), PortData->OvercurrentSshHandle);
        PortData->OvercurrentSshHandle = NULL;
    }

    SET_EVENT(&PortData->OvercurrentLock); 
}

#endif
