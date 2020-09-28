/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    lock.c

     1-10-2004

Abstract:

   Contains the locking functions and descriptions for locks we use in the hub driver.


Author:

    jdunn

Environment:

    Kernel mode

Revision History:



Notes:

FDO Locks:
Lock List (Priority):

level indicates lock hierarchy.

Type                level      Name                         Notes

(sem)               80          FdoPnpStateLock             Instanced per hub
(sem)               70          HardResetLock               Instanced per hub
(sem)               60          BusStateLock                Instanced per hub
(sem)               50          PindicatorLock              Instanced per hub
(sem)               40          SSH_Lock                    Instanced per hub
(sem)               30          SSH_PortsBusyLock           Instanced per hub

(spin)              8           FdoPwrStateLock             Instanced per hub
(spin)              4           PortChangeListSpin          Instanced per port
(spin)              2           PdoStateLock (HGSL)         Global for all hubs

(spin)              1           DevHandle                   Instanced per hub

(spin - queued)     1           HubFdoOmniListSpin          Global for all hubs


Misc FDO Internal Locks:
(These are exclusive -- i.e. you should never need more than one of them at a time)

Type                level      Name                         Notes

(spin - queued)     1          ExceptionListSpin            Instanced per hub
(spin)              1          WorkItemListSpin             Instanced per hub
(spin)              1          TimerObjSpin                 Instanced per hub
(spin)              1          PortResetSpin                Instanced per hub
(spin)              1          BusyListSpin                 Instanced per hub
(spin)              1          SyncDeviceResetSpin          Instanced per hub

PDO Locks:

(spin)              1           IoLock                      Instanced per PDO
(spin)              1           WakeIrpSpin                 Instanced per PDO
(spin)              1           IdleIrpSpin                 Instanced per PDO
(spin)              1           IdleIrpStateSpin            Instanced per PDO



External Locks:

(sem)               1000        BusEnumLock                  Global for each bus


----------------------------------------------------------------------------------------------
LockName:
    <Description>
        -Fields protected
        lock routines
----------------------------------------------------------------------------------------------
BusEnumLock:
    External lock owned by USBPORT, used to control ownership of address zero.  The bus enum lock
    is shared for all hubs on the same bus -- it can potentially be held for long periods of time
    so it is the highest level (lowest priority) and should be acquired first.
        -USB Address 0


..............................................................................................
FdoPnpStateLock:
    Used to manage pnp states for the hub FDO and the PDOs.
        -(FDO)CurrentFdoPnpState
        UsbhAcquireFdoPnpLock
        UsbhReleaseFdoPnpLock

HardResetLock
    Used for the ESD (hard reset) module
        -(FDO) HardResetState
        UsbhAcquireResetLock
        UsbhReleaseResetLock

SSH_Lock
    Used for Hub Selective Suspend
        -(FDO) SSH_State
        UsbhLockSSH
        UsbhUnlockSSH

BusStateLock:
    Used to manage the bus states for the enumeration engine.
        -(FDO)CurrentBusState
        UsbhAcquireBusStateLock
        UsbhReleaseBusStateLock

FdoPwrStateLock
    Used to manage the power states for the hub FDO and PDOs.
        -(FDO)CurrentFdoPwrState
        -(PDO)CurrentPdoPwrState
        UsbhAcquireFdoPwrLock
        UsbhReleaseFdoPwrLock

PortChangeListSpin
    Controls access to the per port change queue.
        -(PORT_DATA)Pcq_State
        -(PORT_DATA)PortChangeList
        UsbhLockPcqWithTag
        UsbhLockPcq
        UsbhUnlockPcq

PdoStateLock
    Used to manage pnp HARDWARE states for all hub PDOs -- this indicates the devices physical
    presence on the bus which may or may not correspond to what Pnp thinks it is.
        -(PDO)CurrentPdoHwPnpState
        -(GLOBAL)HubOmniListHead
        -(PDO)LatchListHead
        UsbhAcquirePdoStateLock
        UsbhReleasePdoStateLock

PindicatorLock

...............................................................................................

ExceptionListSpin:
    Protects the exception record list.
        -(FDO)ExceptionList

WorkItemListSpin:
    Protects the list of currently pending workitems.
        -(FDO)WorkItemListHead

TimerObjSpin:
    Protects the internal timer queue and timer DPC.
        -(FDO)DmTimerListHead

PortResetSpin:
    synchronizes the reset timeout DPC and the enum engine.  Since only one reset can be pending at
    a time this is instanced per hub.



...............................................................................................

IoLock:
    Protects list of 'pending IO' for a PDO. Since is some cases we hook traffic passing through the
    PDO (e.g. bandwidth allocation) we use this lock and list to keep track of it so we don't delete
    the PDO while it is pending.
        -(PDO)PdoFlags.FailIo
        -(PDO)IoList
        UsbhDecPdoIoCount
        UsbhIncPdoIoCount

WakeIrpSpin:
    Used with the CANCEL SAFE QUEUE for wait wake irps posted to PDOs.
        -(PDO)WakeIrp
        UsbhAcquirePdoWakeIrpLock
        UsbhReleasePdoWakeIrpLock

IdleIrpSpin:
    Used with the CANCEL SAFE QUEUE for 'USB Idle' requests sent to the PDOs.
        -(PDO)IdleIrp
        UsbhAcquirePdoIdleIrpLock
        UsbhReleasePdoIdleIrpLock

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "lock.tmh"
#endif

CCHAR usbfile_lock_c[] = "lock.c";

#define USBFILE usbfile_lock_c


/*
State Context Notes

    The state context is used to track threads of execution within the hub driver.
    It is used primarily as a debug mechanism to detect lock contention and other deadlock
    scenarios.

    We create contexts for:

    Global -- stored in HubGlobals, created one time.
    HubFdo -- this hubs FDO, created for each hub.
    Pdo -- Pdos associated with devices, created for each device.
    PortData -- port data struct created for each hub.

*/


VOID
LockGSL(PHGSL Lock)
{
    extern KSPIN_LOCK PdoStateLock;
    // spot recursion errors
   
    KeAcquireSpinLock(&HubG.PdoStateLock, &Lock->Irql);
    UsbhAssert(NULL, !Lock->Locked);
    Lock->Locked = 1;
    UsbhAssert(NULL, HubG.CurrentPdoPnpContext == NULL);
    HubG.CurrentPdoPnpContext = &HubG.GlobalHubContext;
}


VOID
unLockGSL(PHGSL Lock)
{
    extern KSPIN_LOCK PdoStateLock;

    UsbhAssert(NULL, Lock->Locked);
    Lock->Locked = 0;
    UsbhAssert(NULL, HubG.CurrentPdoPnpContext == &HubG.GlobalHubContext);
    HubG.CurrentPdoPnpContext = NULL;
    KeReleaseSpinLock(&HubG.PdoStateLock, Lock->Irql);
}



VOID
UsbhInitStateCtx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    STATE_CONTEXT_TYPE CtxType,
    PVOID Parent
    )
/*++


Routine Description:

    Toplevel init of state context. We assume it is zeroed out
    and the ScFlags are set.

Arguments:


Return Value:

--*/
{
    StateContext->CtxSig = SIG_HUB_CONTEXT;
    StateContext->Parent = Parent;
    StateContext->Type = CtxType;
    StateContext->HubFdo = HubFdo;
    StateContext->PindSig = UNSIG;
    StateContext->HardResetSig = UNSIG;
    StateContext->FdoPnpSig = UNSIG;
    StateContext->FdoPwrSig = UNSIG;
    StateContext->BusSig = UNSIG;
    StateContext->PdoSig = UNSIG;
    StateContext->PcSig = UNSIG;
    StateContext->SSH_Sig = UNSIG;

}

/*
    PDO PNP Lock
*/

VOID
UsbhAcquirePdoStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_PDO_HWPNP_EVENT PdoEvent
    )
/*++

Routine Description:

PdoPnp uses the global hub lock

Arguments:

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    PHGSL lock;

    lock = &StateContext->gLock;

    KeAcquireSpinLock(&HubG.PdoStateLock, &lock->Irql);
    lock->Locked = 1;

    UsbhAssert(HubFdo, StateContext->PdoSig == UNSIG);
    UsbhAssert(HubFdo, HubG.CurrentPdoPnpContext == NULL);

    StateContext->PdoSig = PdoPnpLock_Sig_PDOx;
    StateContext->PdoEvent = PdoEvent;
    HubG.CurrentPdoPnpContext = StateContext;
    StateContext->ScThread = KeGetCurrentThread();

}


VOID
UsbhReleasePdoStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

Acquires the global hub lock in the context of a Pdo Event

Arguments:

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{

    PHGSL lock;

    lock = &StateContext->gLock;

    UsbhAssert(HubFdo, lock->Locked);
    lock->Locked = 0;

    UsbhAssert(HubFdo, StateContext->PdoSig == PdoPnpLock_Sig_PDOx);
    UsbhAssert(HubFdo, HubG.CurrentPdoPnpContext == StateContext);
    HubG.CurrentPdoPnpContext = NULL;
    StateContext->PdoSig = UNSIG;

    KeReleaseSpinLock(&HubG.PdoStateLock, lock->Irql);

}




HUB_FDO_PNPSTATE
UsbhAcquireFdoPnpLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_FDO_EVENT FdoEvent,
    ULONG Tag,
    ULONG LogIt
    )
/*++


Routine Description:

    The FDO state lock is used to synchronize pnp and power requests. The system actually
    serializes most of the key irps associated with pnp but not for power.  This lock is used
    to protect the initialization code from power irps and visa versa.

    We also use it to synchronize pnp with any other request that might interfere with pnp.

    It is instanced for each hub should be used to protect the FDO pnp and power
    state machines from attack during FDO pnp and power events.


Arguments:

    FdoEvent - event associated with this acquisition.

    Tag - optional tag stored in the event log

    LogIt - flag nonzero updates the state transition log. This is used by routines
        that the lock but don't transition the state.  Nice for debugging.

Return Value:

    returns a ptr to the context for convenience

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc = StateContext;
    PG_STATE_LOG_ENTRY fdoLog;

    hubExt = FdoExt(HubFdo);

    KeWaitForSingleObject(&hubExt->FdoPnpStateLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

    // this indicates a double acquire because its the same context
    UsbhAssert(HubFdo, sc->FdoPnpSig == UNSIG);
    // we should not be holding the lock
    UsbhAssert(HubFdo, hubExt->CurrentFdoPnpContext == NULL);

    sc->FdoPnpState = hubExt->CurrentFdoPnpState;
    sc->FdoPnpEvent = FdoEvent;
    sc->FdoPnpSig = FdoPnpLock_Sig_FDO1;
    sc->ScThread = KeGetCurrentThread();

    hubExt->CurrentFdoPnpContext = sc;

    if (LogIt) {

        hubExt->FdoLogIdx=(hubExt->FdoLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
        fdoLog = &hubExt->FdoLog[hubExt->FdoLogIdx];

        fdoLog->Event = FdoEvent;
        fdoLog->CurrentState = hubExt->CurrentFdoPnpState;
        fdoLog->NextState = hubExt->CurrentFdoPnpState;
        fdoLog->Tag = Tag;
    }

    return hubExt->CurrentFdoPnpState;
}


VOID
UsbhReleaseFdoPnpLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++


Routine Description:


Arguments:


Return Value:

    returns a ptr to the context for convenience

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc = StateContext;

    hubExt = FdoExt(HubFdo);

    // this would indicate possible race condition
    UsbhAssertFdoPnpContextLock(StateContext);


    sc->FdoPnpSig = UNSIG;
    hubExt->CurrentFdoPnpContext = NULL;

    //irql = hubExt->FdoStateLockIrql;
    KeReleaseSemaphore(&hubExt->FdoPnpStateLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);


}


HUB_FDO_POWERSTATE
UsbhAcquireFdoPwrLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_FDO_EVENT FdoEvent,
    ULONG Tag
    )
/*++


Routine Description:


    Acquires the FDO/PDO power lock. We use this to serialize the power state
    machine in the hub.  The power messages are serialized for each device object
    but not always between the PDOs and FDO.

Arguments:


Return Value:

    returns the current FDO power state.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc = StateContext;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->FdoPwrStateLock, &irql);
    // this indicates a double acquire because its the same context
    UsbhAssert(HubFdo, sc->FdoPwrSig == UNSIG);
    // we should not be holding the lock
    UsbhAssert(HubFdo, hubExt->CurrentFdoPwrContext != StateContext);

    hubExt->FdoPwrLockIrql = irql;
    sc->FdoPwrState = hubExt->CurrentFdoPwrState;
    sc->FdoPwrEvent = FdoEvent;
    sc->FdoPwrSig = FdoPwrLock_Sig_FDO2;
    sc->FdoPwrTag = Tag;
    sc->ScThread = KeGetCurrentThread();
    UsbhAssert(HubFdo, hubExt->CurrentFdoPwrContext == NULL);

    hubExt->CurrentFdoPwrContext = sc;

    return hubExt->CurrentFdoPwrState;
}


VOID
UsbhReleaseFdoPwrLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++


Routine Description:


Arguments:


Return Value:

    returns a ptr to the context for convenience

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc = StateContext;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    // this would indicate possible race condition
    UsbhAssertFdoPwrContextLock(StateContext);

    sc->FdoPwrSig = UNSIG;
    hubExt->CurrentFdoPwrContext = NULL;

    irql = hubExt->FdoPwrLockIrql;

    KeReleaseSpinLock(&hubExt->FdoPwrStateLock, irql);


}


HARD_RESET_STATE
UsbhAcquireResetLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

Serializes Hubreset state transitions.

Arguments:

    Context - state context, this will be the context that currently holds the lock.

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    KeWaitForSingleObject(&hubExt->HardResetLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

    UsbhAssert(HubFdo, StateContext->HardResetSig == UNSIG);

    StateContext->HardResetSig = HardResetLock_Sig_RSTx;
    StateContext->HardResetEvent = HardResetEvent;
    StateContext->ScThread = KeGetCurrentThread();
    rs = StateContext->HardResetState = hubExt->HardResetState;

    UsbhAssert(HubFdo, rs);
    UsbhAssert(HubFdo, hubExt->CurrentResetContext == NULL);
    hubExt->CurrentResetContext = StateContext;

    // update state history
    hubExt->ResetLogIdx=(hubExt->ResetLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    log = &hubExt->ResetLog[hubExt->ResetLogIdx];

    log->Event = HardResetEvent;
    log->CurrentState = hubExt->HardResetState;

    return rs;
}


HUB_BUS_STATE
UsbhReleaseResetLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HARD_RESET_STATE NewResetState
    )
/*++

Routine Description:

Serializes Bus state transitions. Releases the bus state lock

Arguments:

    Context - ptr to a bus state context -- this will be the context that currently
        holds the lock.

    NewResetState - new state to transition to before releasing the lock.

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(HubFdo);

    rs = StateContext->HardResetState = hubExt->HardResetState = NewResetState;

    StateContext->HardResetSig = UNSIG;

    hubExt->CurrentResetContext = NULL;

    //hubExt->BusLogIdx=(hubExt->BusLogIdx) % G_NUM_STATE_LOG_ENTRIES;
    log = &hubExt->ResetLog[hubExt->ResetLogIdx];

    log->NextState = NewResetState;

    KeReleaseSemaphore(&hubExt->HardResetLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

    return rs;
}


