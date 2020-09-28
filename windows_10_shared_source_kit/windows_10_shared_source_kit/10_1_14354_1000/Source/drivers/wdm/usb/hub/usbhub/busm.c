/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    busm.c

     2-4-2004

Abstract:

    Provides bus driver (bus state) functionality for the usbhub.

    The bus state machine is an abstraction that lets us unify pnp and power. Since these
    systems are unsynchronized in the operating system anyway merging them at this common
    point allows us to share a lot of code between pnp and power.  In fact the code paths
    are the same for hibernate as they are for stop.


Module:



Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

/*
Bus State machine

/*
    Bus states for the enumeration engine -- these states are instanced once per hub.

    Actions:
        BusWaitCB_Action
        BusPnpStop_Action
        BusResume_Action
        BusPause_Action
        BusDisconnect_Action
        BusSuspend_Action

BE_BusAdd:
    Occurs when the bus receives a BE_BusPnpStop message. PnpStop and IRP_MN_STOP_DEVICE are not always the
    same thing, however, the 'device' presence is not changed by this message.

BE_BusRemove:
    Sent when the PDO receives an IRP_MN_SURPRISE_REMOVAL message.

BE_BusPnpStop
    Initial state transition when a PDO is created.

BE_BusPnpAsyncStart
    Occurs when a PDO is reported to pnp.

BE_RequestDisconnect
    We send this to 'drop' devices from the bus if we need to reset or if we get an error on power
    management.

BE_RequestBusPause
    Called by power and pnp code to idle the bus.



Bus State Table
----------------------------------------------------------------------------------------------------------
State               Events                      Next State          Actions

BS_BusInit
                    BE_BusAdd                   BS_BusStop

..........................................................................................................
BS_BusStop
                    BE_BusRemove                BS_BusRemove
                    BE_BusPnpStop               BS_BusStop
                    BE_BusPnpAsyncStart         BS_BusPause
                    BE_BusPnpStart              BS_BusRun           BusResume_Action
                    BE_BusSuspend               BS_BusStop
                    BE_RequestBusResume         BS_BusStop
                    BE_BusPnpStartError         BS_BusPause
                    BE_RequestBusReset
//                    BE_RequestDisconnect        BS_BusStop          UsbhBusDisconnect_Action
...........................................................................................................
BS_BusRun
                    BE_BusPnpStop               BS_BusStop          BusPause_Action
                    BE_BusSuspend               BS_BusSuspend       BusPause_Action, BusSuspend_Action
                    BE_RequestBusPause          BS_BusPause         BusPause_Action
............................................................................................................
BS_BusPause
                    BE_BusPnpStart              BS_BusRun           BusResume_Action
                    BE_RequestDisconnect        BS_BusPause         UsbhBusDisconnect_Action
                    BE_RequestBusReset          BS_BusPause
                    BE_RequestBusPause          BS_BusPause
                    BE_BusPnpStop               BS_BusStop          BusPnpStop_Action
                    BE_BusSuspend               BS_BusSuspend       BusSuspend_Action
                    BE_RequestBusResume         BS_BusRun
                    BE_BusPnpStartError         BS_BusPause
...........................................................................................................
BS_BusSuspend
                    BE_RequestBusReset          BS_BusSuspend       BusReset_Action, BusSuspend_Action
                    BE_RequestBusPause          BS_BusPause
                    BE_BusPnpStop               BS_BusStop          BusPnpStopAction
                    BE_RequestBusResume         BS_BusRun
                    BE_RequestDisconnect        BS_BusSuspend       UsbhBusDisconnect_Action
...........................................................................................................
BS_BusRemove

...........................................................................................................

*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "busm.tmh"
#endif

CCHAR usbfile_busm_c[] = "busm.c";

#define USBFILE usbfile_busm_c


HUB_BUS_STATE
UsbhAcquireBusStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
/*++

Routine Description:

Serializes Bus state transitions.

Arguments:

    Context ptr to a bus state context -- this will be the context that currently holds the lock.

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    KeWaitForSingleObject(&hubExt->BusStateLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

    UsbhAssert(HubFdo, StateContext->BusSig == UNSIG);

    StateContext->BusSig = SIG_BUS_CONTEXT;
    StateContext->BusEvent = BusEvent;
    StateContext->ScThread = KeGetCurrentThread();
    bs = StateContext->BusState = hubExt->CurrentBusState;

    UsbhAssert(HubFdo, bs);
    UsbhAssert(HubFdo, hubExt->CurrentBusStateContext == NULL);
    hubExt->CurrentBusStateContext = StateContext;

    // update state history
    hubExt->BusLogIdx=(hubExt->BusLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    log = &hubExt->BusLog[hubExt->BusLogIdx];

    log->Event = BusEvent;
    log->CurrentState = hubExt->CurrentBusState;

    return bs;
}


HUB_BUS_STATE
UsbhReleaseBusStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_STATE NewBusState
    )
/*++

Routine Description:

Serializes Bus state transitions. Releases the bus state lock/

Arguments:

    Context - ptr to a bus state context -- this will be the context that currently
        holds the lock.
    NewBusState - new state to transition too before releasing the lock.

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(HubFdo);

    //
    // Take no action if we are already in the target bus state
    //
    if (hubExt->CurrentBusState != NewBusState) {
    
        StateContext->BusState = hubExt->CurrentBusState = NewBusState;

        switch(NewBusState) {
        case BS_BusRun:
            /* entry actions  BS_Run*/
            UsbhBusResume_Action(HubFdo, StateContext);
            break;

        case BS_BusSuspend:
            /* entry actions  BS_Suspend*/
            UsbhBusSuspend_Action(HubFdo, StateContext);
            break;
        }
    }

    StateContext->BusSig = UNSIG;

    hubExt->CurrentBusStateContext = NULL;

    //hubExt->BusLogIdx=(hubExt->BusLogIdx) % G_NUM_STATE_LOG_ENTRIES;
    log = &hubExt->BusLog[hubExt->BusLogIdx];

    log->NextState = NewBusState;

    KeReleaseSemaphore(&hubExt->BusStateLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

    return NewBusState;
}


HUB_BUS_STATE
UsbhGetContextBusState(
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

Arguments:

Irql: PASSIVE

Return Value:

   returns the current bus state

--*/

{
    UsbhAssertBusStateContext(Context);

    return Context->BusState;

}


HUB_BUS_STATE
Usbh_BS_BusInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{

    HUB_BUS_STATE bs;

    bs = UsbhGetContextBusState(StateContext);

    UsbhAssert(HubFdo, bs == BS_BusInit);
    UsbhAssertPassive();

    LOG(HubFdo, LOG_BUSM, 'bsIN', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    switch(BusEvent) {

    case BE_BusAdd:
       // initial transition, int start status to failure in case the async start worker
       // fails

       bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
       break;

    default:
       bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
       UsbhAssertFailure(HubFdo, StateContext);
    }

    return bs;
}


HUB_BUS_STATE
Usbh_BS_BusStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    bs = UsbhGetContextBusState(StateContext);
    UsbhAssert(HubFdo, bs == BS_BusStop);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_BUSM, 'bsST', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    switch(BusEvent) {

    case BE_BusPnpStop:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

    case BE_BusPnpAsyncStart:

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        break;

    case BE_BusPnpStart:
         TEST_TRAP();
        /* transition actions */
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRun);
        break;

    case BE_BusPnpStartError:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        break;

    case BE_BusRemove:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRemove);
        break;

    case BE_BusSuspend:
        TEST_TRAP();
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        break;
    
    //Vista bug#1620619
    case BE_RequestBusPause:
    case BE_RequestBusResume:
    //Win7 bug#2000089
    case BE_RequestDisconnect:
        TEST_TRAP();
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        break;

    case BE_BusDeviceRemove:
        // this is sent in conjunction with a remove message
        /* transition actions */
        UsbhBusDeviceRemove_Action(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

    default:
        // these events are invalid in this state
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        UsbhAssertFailure(HubFdo, StateContext);

    }

    return bs;
}


HUB_BUS_STATE
Usbh_BS_BusRun(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{

    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    bs = UsbhGetContextBusState(StateContext);
    UsbhAssert(HubFdo, bs == BS_BusRun);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_BUSM, 'bsRN', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    // Pause reason is only valid for pause requests
    if (BusEvent != BE_RequestBusPause) {
        StateContext->PauseReason = PauseReason_None;
    }
    
    switch(BusEvent) {
    case BE_RequestBusPause:
    
        nts = UsbhBusPause_Action(HubFdo, StateContext);

        if (NT_ERROR(nts)) {           
            
            NT_ASSERT(StateContext->PauseReason == PauseReason_SelectiveSuspend);

            // Pause failed for hub selective suspend, return to BusRun state
            bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRun);
        } else {
            bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        }
        break;

    case BE_BusPnpStop:

        nts = UsbhBusPause_Action(HubFdo, StateContext);

        /* transition actions */
        UsbhBusPnpStop_Action(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

     case BE_BusSuspend:

        nts = UsbhBusPause_Action(HubFdo, StateContext);

        // lockout selective suspend
        UsbhSshBusBusy(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusSuspend);
        break;

    default:
       // these events are invalid in this state
       bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
       UsbhAssertFailure(HubFdo, StateContext);

    }

    return bs;

}


HUB_BUS_STATE
Usbh_BS_BusPause(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    bs = UsbhGetContextBusState(StateContext);
    UsbhAssert(HubFdo, bs == BS_BusPause);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_BUSM, 'bsPS', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    switch(BusEvent) {

    case BE_BusPnpStart:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRun);
        break;

    case BE_RequestDisconnect:
        UsbhBusDisconnect_Action(HubFdo, StateContext);
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        break;

    case BE_RequestBusReset:
    case BE_RequestBusPause:
        // nop
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        break;

    case BE_BusPnpStop:

        /* transition actions */
        UsbhBusPnpStop_Action(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

    case BE_BusDeviceRemove:

        /* transition actions */
        UsbhBusDeviceRemove_Action(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

    case BE_BusSuspend:

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusSuspend);
        break;

    case BE_RequestBusResume:

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRun);
        break;

    case BE_BusPnpStartError:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        break;

    default:
        // these events are invalid in this state
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        UsbhAssertFailure(HubFdo, StateContext);
    }

    return bs;
}


HUB_BUS_STATE
Usbh_BS_BusSuspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    bs = UsbhGetContextBusState(StateContext);
    UsbhAssert(HubFdo, bs == BS_BusSuspend);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_BUSM, 'bsPS', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    switch(BusEvent) {

    case BE_RequestBusReset:

        UsbhBusReset_Action(HubFdo, StateContext);
        // reset but stay in suspend
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusSuspend);

        break;

    case BE_RequestBusPause:

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusPause);
        break;

    case BE_BusPnpStop:

        /* transition actions */
        UsbhBusPnpStop_Action(HubFdo, StateContext);

        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusStop);
        break;

    case BE_RequestBusResume:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusRun);
        break;

    case BE_RequestDisconnect:
        UsbhBusDisconnect_Action(HubFdo, StateContext);
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, BS_BusSuspend);
        break;

    default:
        // these events are invalid in this state
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
        UsbhAssertFailure(HubFdo, StateContext);
    }

    return bs;
}


HUB_BUS_STATE
Usbh_BS_BusRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    bs = UsbhGetContextBusState(StateContext);
    UsbhAssert(HubFdo, bs == BS_BusRemove);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_BUSM, 'bsPS', StateContext, BusEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));

    switch(BusEvent) {

    // Hub work-items for the FDO can execute after hub removal and
    // try to initiate bus state transitions.  These are benign events, so
    // they are ingored, aside from being logged in the state machine history.  
    default:
        bs = UsbhReleaseBusStateLock(HubFdo, StateContext, bs);
    }

    return bs;
}


HUB_BUS_STATE
UsbhDispatch_BusEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    HUB_BUS_EVENT BusEvent
    )
/*++

Routine Description:

Dispatcher routine for bus state transitions.

The bus state machine inherits behavior from the FdoPnp state machine.  The following FdoPnp
functions generate hub bus events:

PnpStart()      BE_BusPnpAsyncStart
PnpStop()       BE_BusPnpStop
PnpAdd()        BE_BusAdd
PnpUnload()     BE_Remove


-Exit actions are executed before the state changes. exit actions are global to the state.
-Entry actions are executed when exiting the previous state after the state var is changed
 and before the lock is released.
-Transition Actions are executed before the state is changed while the lock is held. Transition
 actions are specific to the state transition.


Arguments:

    Context ptr to a bus state context -- this will be the context that currently holds the lock.

Irql: PASSIVE

Return Value:

   returns the new bus state

--*/

{
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();
    UsbhAssert(HubFdo, BusEvent);

   // this can be called in the pnp context or the context for one of our
   // workers (eg reset or overcurrent recovery)

    if (BusEvent == BE_BusPnpStop) {
        // lock out selective suspend before acquiring the bus state lock
        UsbhSshBusBusy(HubFdo, StateContext);
    }

    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", BusEvent));
    LOG(HubFdo, LOG_BUSM, 'bs++', StateContext, BusEvent);
    bs = UsbhAcquireBusStateLock(HubFdo, StateContext, BusEvent);
    LOG(HubFdo, LOG_BUSM, 'bs--', StateContext, bs);

    switch (bs) {
    case BS_BusInit:
        return Usbh_BS_BusInit(HubFdo, StateContext, BusEvent);

    case BS_BusStop:
        return Usbh_BS_BusStop(HubFdo, StateContext, BusEvent);

    case BS_BusRun:
        return Usbh_BS_BusRun(HubFdo, StateContext, BusEvent);

    case BS_BusPause:
        return Usbh_BS_BusPause(HubFdo, StateContext, BusEvent);

    case BS_BusSuspend:
        return Usbh_BS_BusSuspend(HubFdo, StateContext, BusEvent);

    case BS_BusRemove:
        return Usbh_BS_BusRemove(HubFdo, StateContext, BusEvent);

    default:
        UsbhAssertFailure(HubFdo, StateContext);
        UsbhReleaseBusStateLock(HubFdo, StateContext, bs);

    }

    return bs;
}


VOID
UsbhBusResume_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Resumes bus engine from the paused state.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT port;
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'res>', 0, 0);

    hubExt->IrqErrorCount = 0;
    RESET_EVENT(&hubExt->IrqDoneEvent);

    hubExt->IrqCancelFlag = 0;

    // enable change processing on each port
    for (port = 1; port <= N_PORTS(HubFdo); port++) {

        UsbhPCE_Resume(HubFdo, Sc, port);
        UsbhPCE_Enable(HubFdo, Sc, port);

        // sync with the "Driver Not Found" timer to insure we can disable the port
        // if needed after the port change state machine is resumed.
        pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatch_Resume_Tag);
        if (pdo != NULL) {
            pdoExt = PdoExt(pdo);
            WAIT_EVENT(&pdoExt->DriverNotFoundLock);
            pdoExt->PdoFlags.PortPaused = 0;
            SET_EVENT(&pdoExt->DriverNotFoundLock);
            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_Resume_Tag);
        }

    }

    

    // this MUST happen after enable since the change engine will ignore
    // any events that occur while the port is disabled.
    UsbhPostInterrupt(HubFdo);

    // allow selective suspend
    UsbhSshBusIdle(HubFdo, Sc);
}


VOID
UsbhBusDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Disconnects all Pdo from the bus and notifies pnp. This is only
    valid from the pause state



Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT port;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'res>', 0, 0);

    // enable change processing on each port
    for (port = 1; port <= N_PORTS(HubFdo); port++) {
        UsbhPCE_BusDisconnect(HubFdo, StateContext, port);
    }

}


VOID
UsbhBusReset_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Resets the bus engine and restores all queues to pause state.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT port;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'res>', 0, 0);

    // enable change processing on each port
    for (port = 1; port <= N_PORTS(HubFdo); port++) {
        UsbhPCE_BusReset(HubFdo, StateContext, port);
    }

}


VOID
UsbhBusDeviceRemove_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Action function associated with pnp removal.
   This is where we cleanup PDOs

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/

{
    // drop all Pdos
    Usbh_BusRemove_PdoEvent(Sc->HubFdo, Sc);

}


VOID
UsbhBusPnpStop_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

   Action function associated with stop.

   This code actually stops the bus engine.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;
    USHORT pn;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'BstA', 0, 0);

    UsbhDisablePortIndicators(HubFdo);

    // no APIs while we are stopping,
    // let any that are running finish
    UsbhAssertPassive();

    // enum engine should already be spun down but we will still be in BS_BusRun
    LOG(HubFdo, LOG_BUSM, 'PAU1', 0, 0);

    // after spinning down disable all downstream ports on this hub.  This is
    // so that we are in a consistent state the event of a re-start.  in the
    // start and re-start case the hub generates 'artificial' connect changes
    // in order to stimulate enumeration of the devices.
    // **note that we don't wait for the ack of the disable (thru a disable
    // change) because the engine is paused. We must deal with that case on restart.
    pd = hubExt->PortData;
    for (pn=1; pn<=N_PORTS(HubFdo); pn++) {
        USB_HUB_PORT_STATE ps;

        UsbhPCE_HW_Stop(HubFdo, pn, StateContext);

        UsbhAssert(HubFdo, pd->Sig == SIG_HUBPORTDATA);
        nts = UsbhQueryPortState(HubFdo, pn, &ps, NULL);
        LOG(HubFdo, LOG_BUSM, 'stpD', pd, ps.Status.us);
        if (NT_SUCCESS(nts) && ps.Status.Enable) {
            // try to disable the port
            UsbhDisablePort(HubFdo, pd);
        }


        // dispatch the cleanup message to the port change queues.  This allows us to validate they are empty.
        UsbhPCE_Close(HubFdo, pd, StateContext);
        pd++;
    }

    // now we loop thru port data structures and remove any PDOs that are in
    // a semi-initialized state as a result of the bus engine spinning down.
    // These are PDOs that Pnp does not know about and will not remove
    // via normal means.

    Usbh_BusPnpStop_PdoEvent(HubFdo, StateContext);

    // Wait for any stray ISR work-items to complete
    UsbhDecrementHubIsrWorkerReferenceAndWait(HubFdo);

    UsbhFreeTimerObject(HubFdo, &hubExt->IsrRetryTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->ResetIrqPipeRetryTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->HubInitTimeoutTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->HubInitRetryTimer);

    IoFreeIrp(hubExt->IrqIrp);
    hubExt->IrqIrp = NULL;

    UsbhFreePool(hubExt->IrqUrb);

    UsbhFreePool(hubExt->IrqBuffer);

    // defer the free of portData until PnpRemove or a re-start, this information is useful to
    // have in the event of a hang on remove.
    //*UsbhFreePool(hubExt->PortData);

}


NTSTATUS
UsbhBusPause_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    stops the hub driver change state machine but does not free any resources
    so it can be restarted again

    this is a synchronous function, enumeration state machine is stopped
    on return.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT pn;
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'pau1', 0, 0);
    UsbhAssertPassive();

    if (InterlockedDecrement(&hubExt->IrqCancelFlag) == 0) {
        // irq is pending, cancel it
        LOG(HubFdo, LOG_BUSM, 'caIQ', hubExt->IrqIrp, 0);
        IoCancelIrp(hubExt->IrqIrp);
    }

    // irq done event is signaled when our transfer completes after a cancel
    // or failure. That is -- it is signaled in any case where we don't repost
    // the IRQ irp thru the worker.
    //
    // it also indicates that there is no ISR work item pending. The completion
    // routine is the only thing that can queue the worker and the worker and
    // UsbhBusResume are the only routines that should post the interrupt
    // transfer.
    // If IrqCancelFlag transitions 0->-1 the worker will not re-post the
    // interrupt.
    //
    // Note: killing the irq does not prevent changes from being queued and processed
    // since not all events are generated by the Hub interrupts.
    WAIT_EVENT(&hubExt->IrqDoneEvent);

    // If we are selectively suspending the hub, we need to check and see if there
    // has been any activity for the hub.  If so we will need to transition back to
    // the BS_BusRun state rather than pausing.
    if ((StateContext->PauseReason == PauseReason_SelectiveSuspend) && !NT_SUCCESS(UsbhSsh_CheckHubIdle(HubFdo, StateContext))) {
        // Hub is no longer idle; repost the status notification interrupt
        hubExt->IrqCancelFlag = 0;
        RESET_EVENT(&hubExt->IrqDoneEvent);
        UsbhPostInterrupt(HubFdo);
        return STATUS_UNSUCCESSFUL;
    }

    // we are now in the bs_busrun state waiting to pause,
    // wait for each port state change machine to drain their change queues.
    for (pn=1; pn<=N_PORTS(HubFdo); pn++) {

        PHUB_PORT_DATA PortData = UsbhGetPortData(HubFdo, pn);

        //
        // There may be a thread waiting on an event.  Signal them based on
        // PortHwPendingState.
        //
        LOG(HubFdo, LOG_BUSM, 'pauW', 0, pn);

        if (PortData != NULL) {

            LOG(HubFdo, LOG_BUSM, 'pau2', PortData->PortHwPendingState, pn);

//            Usbh_CheckPortHwPendingStatus(HubFdo, PortData);
        }

        // Lock out Resume/Enable events until port is paused, only for hub hard resets
        if (StateContext->PauseReason == PauseReason_HardReset) {
            WAIT_EVENT(&PortData->EnableDisableLock);
        }
        
        
        // sync with the "Driver Not Found" timer to insure we don't disable the port
        // after the port change state machine is disabled.
        pdo = UsbhLatchPdo(HubFdo, pn, NULL, PdoLatch_Pause_Tag);
        if (pdo != NULL) {
            pdoExt = PdoExt(pdo);
            WAIT_EVENT(&pdoExt->DriverNotFoundLock);
            pdoExt->PdoFlags.PortPaused = 1;
            SET_EVENT(&pdoExt->DriverNotFoundLock);
            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_Pause_Tag);
        }
        
        // queue the 'pause' event. Any changes encountered after this event are ignored.
        // this allows us to cleanly exit the state machine.
        // the pause message will cause any operation (like reset) to be canceled if it is
        // in progress.
        UsbhQueuePauseChange(HubFdo, pn, StateContext);
        // wait for any running workitem code to finish and the queues to drain
        UsbhPCE_Disable(HubFdo, pn, StateContext);
        
        if (StateContext->PauseReason == PauseReason_HardReset) {
            // Allow Resume/Enable events
            SET_EVENT(&PortData->EnableDisableLock);
        }
    }
    LOG(HubFdo, LOG_BUSM, 'pau>', 0, 0);

    return STATUS_SUCCESS;
}


VOID
UsbhBusSuspend_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Action handler, puts the hub in UsbSuspend.

    Suspends all ports that are not already suspended.  Clients should have already put
    the device in the appropriate state but some devices may not have drivers loaded.

    This code is necessary for devices that may not have a function driver loaded or
    the hub cannot support 'selective' suspend of the ports.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    USHORT port;
    USB_HUB_PORT_STATE ps;
    PDEVICE_OBJECT pdo;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'Bsus', 0, 0);

    if (hubExt->HubVbusState == Vbus_Off) {
        // if we lose Vbus don't bother with
        // suspending the ports.
        return;
    }

    for (port=1; port <= N_PORTS(HubFdo); port++) {

        nts = UsbhQueryPortState(HubFdo,
                         port,
                         &ps,
                         &usbdStatus);

        if (NT_SUCCESS(nts)) {
            // this is a nop if the port is not enabled
            
            if (ps.Status.Enable && ps.Status.Connect &&                
                !ps.Status.Suspend) {

                // if the port is left open due to a hibernate file being in path, 
                // keep it that way
                pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatch_FdoSetD3_Tag);
                if (!pdo || !PdoExt(pdo)->HibernateCount) {
                    // suspend downstream ports that have devices connected.
                    // since the bus engine is not running we issue this call directly
                    //
                    // port should be in PCQ_Stop if its not already suspended.
                    UsbhPCE_Suspend(HubFdo, Context, port);
                    //UsbhSuspendPort(HubFdo, port);
                }
                if (pdo) {
                    UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_FdoSetD3_Tag);
                }
            }

        } else if (!Usb_Disconnected(nts)) {

            // some type of error trying to suspend
            HUB_EXCEPTION(USBFILE,  HubFdo, port,
                                  PowerErr_FailedGetStatusOnSuspend,
                                  nts, usbdStatus, NULL, 0);

            // treat this as Vbus_Interrupted so the hub driver won't
            // try to resume
            hubExt->HubVbusState = Vbus_Interrupted;
        }

    }

}


HUB_BUS_STATE
UsbhSyncBusPause(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    BS_PAUSE_REASON PauseReason
    )
/*++

Routine Description:

    Queues a request top the USB change 'bus state machine' but does not free any
    resources so that the hub can be restarted again.

    This is a synchronous function, enumeration state machine is stopped
    on return.  Action is only taken if the bus is in BS_BusRun  -- otherwise this operation
    is a nop.

    This function is used by power and reset code to start stop the change
    engine -- no structures are freed.

Arguments:

Irql: PASSIVE

Return Value:

   TRUE if the Bus transitioned from BS_BusRun to BS_BusPause, FALSE otherwise.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'PAR>', PauseReason, StateContext);

    StateContext->PauseReason = PauseReason;

    bs = UsbhDispatch_BusEvent(HubFdo, StateContext, BE_RequestBusPause);

    LOG(HubFdo, LOG_BUSM, 'PAR<', 0, bs);

    return bs;
}


HUB_BUS_STATE
UsbhSyncBusResume(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Queues a request top the USB change 'bus state machine' to restart the hub.

    This is a synchronous function, enumeration state machine is BS_Pause or BS_Run on
    return.

    This function is used by power and reset code to start stop the change
    engine.

Arguments:

Irql: PASSIVE

Return Value:

   bus state transitioned to.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'PAR>', 0, 0);

    bs = UsbhDispatch_BusEvent(HubFdo, Context, BE_RequestBusResume);

    LOG(HubFdo, LOG_BUSM, 'PAR<', 0, bs);

    return bs;
}


HUB_BUS_STATE
UsbhSyncBusReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    This function is used by the power code to reset the queues during a cold start.

Arguments:

Irql: PASSIVE

Return Value:

   bus state transitioned to.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'PAR>', 0, 0);

    bs = UsbhDispatch_BusEvent(HubFdo, Context, BE_RequestBusReset);

    LOG(HubFdo, LOG_BUSM, 'PAR<', 0, bs);

    return bs;
}


HUB_BUS_STATE
UsbhSyncBusDisconnect(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    This function is used by the reset code to drop all devices before a hard reset.

Arguments:

Irql: PASSIVE

Return Value:

   bus state transitioned to.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'BDS>', 0, 0);

    bs = UsbhDispatch_BusEvent(HubFdo, Context, BE_RequestDisconnect);

    LOG(HubFdo, LOG_BUSM, 'BDS<', 0, bs);

    return bs;
}


HUB_BUS_STATE
UsbhSyncBusSuspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

   Issues the BE_BusSuspend Event to the bus engine.  This will suspend all ports that
   have devices and are not already suspended.

Arguments:

Irql: PASSIVE

Return Value:

   Bus state transitioned to.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUSM, 'BSP>', 0, 0);

    bs = UsbhDispatch_BusEvent(HubFdo, Context, BE_BusSuspend);

    LOG(HubFdo, LOG_BUSM, 'BSP<', 0, bs);

    return bs;
}


VOID
UsbhSshBusBusy(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Bus set hub SSH busy.  Prevents us from setting the hub device handle to idle.

   We only allow one 'busy handle' outstanding for this purpose.  If one was already present
   then we free the new one.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE oldBh, bh;

    hubExt = FdoExt(HubFdo);

    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhBusyBUS_Tag, RESUME_HUB);
    // oldBh = hubExt->BusBusyHandle
    // if (hubExt->BusBusyHandle == NULL)
    //     hubExt->BusBusyHandle = bh;


    oldBh = InterlockedCompareExchangePointer(&hubExt->BusBusyHandle,
                                               bh,
                                               NULL);

    LOG(HubFdo, LOG_BUSM, 'Bsy+', oldBh, bh);
    if (oldBh != NULL) {
        LOG(HubFdo, LOG_BUSM, 'Bs2-', oldBh, bh);
        UsbhDecHubBusy(HubFdo, Sc, bh);
    }

    return;
}


VOID
UsbhSshBusIdle(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Bus clear hub SSH busy.  Allows the hub to indicate 'idle ready'.

Arguments:

Irql: PASSIVE

Return Value:



--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE bh;

    hubExt = FdoExt(HubFdo);

    bh = InterlockedExchangePointer(&hubExt->BusBusyHandle, NULL);

    LOG(HubFdo, LOG_BUSM, 'Bsy-', 0, bh);
    UsbhDecHubBusy(HubFdo, Sc, bh);

    return;
}

