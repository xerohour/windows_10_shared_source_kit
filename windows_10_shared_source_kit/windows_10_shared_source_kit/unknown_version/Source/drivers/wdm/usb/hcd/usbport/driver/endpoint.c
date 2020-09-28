/*++

Copyright (c) Microsoft Corporation

Module Name:

    endpoint.c

Abstract:

    Contains new endpoint support routines.


Environment:

    kernel mode only

Notes:

    Margin = 120

Revision History:

    9-8-04


--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "endpoint.tmh"
#endif


VOID
USBPORT_iAssertEndpoint(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Debug only, sanity checks an endpoint.

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);

#if DBG
    // this routine (or caller) should hold global ep list lock
    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);

    USBPORT_ASSERT(Endpoint->Sig == SIG_ENDPOINT);

    if (Endpoint->PendingAbortContext) {
        ASSERT_ABORT_CONTEXT(Endpoint->PendingAbortContext);
    }

#endif
}


_Requires_lock_not_held_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_Acquires_lock_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_raises_(DISPATCH_LEVEL)
VOID
USBPORT_AcquireEpListLock(
    _Inout_ PDEVICE_OBJECT HcFdo,
    _Out_ __deref _IRQL_saves_ PKIRQL Irql
    )
/*++

Routine Description:

    Acquires Global Endpoint list Lock

Arguments:

    HcFdo - Host controller.

    Endpoint - (ref'ed)endpoint whose lock needs to be acquired.


Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.EndpointListSpin, Irql);

    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 0);
    devExt->Fdo.EpListLockState = 1;
}


_Requires_lock_held_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_Releases_lock_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
USBPORT_ReleaseEpListLock(
    _Inout_ PDEVICE_OBJECT HcFdo,
    _In_ _IRQL_restores_ KIRQL Irql
    )
/*++

Routine Description:

    Releases Global Endpoint list Lock

Arguments:

    HcFdo - Host controller.

    Endpoint - (ref'ed)endpoint whose lock needs to be acquired.


Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);
    devExt->Fdo.EpListLockState = 0;
    KeReleaseSpinLock(&devExt->Fdo.EndpointListSpin, Irql);
}


/*
Port/Miniport EndpointHardware States:


ACTIVE/ PAUSE, REMOVE toggle the endpoint in and out of the schedule
** PAUSE was created to expoit the OHCI skip bit, for UHCI & EHCI PAUSE == REMOVE.
OPEN/ CLOSE alloacte and free the endpoint.

This list show the global endpoint superstates along with the corresponding hardware substate

The host controllers only maintain internal hardware state of the endpoints.  Due to the way USB controllers are
designed some harware state transitions take a descrete amount of time.  The port driver manages this with a set
of global endpoint 'superstates' that include the transitions in the hardware.  This way the miniport does not have
to handle the hardware synchronization.  (see MINIPORT_SetEndpointState in usbhcdi.h)

Example:

<USBPORT>
GEp_Active--(remove queue head, req bell)------>GEp_PausePending--(bell)--->GEp_Paused
<MINIPORT>
ENDPOINT_ACTIVE-------(MP_SetEpState)------------------------------------>ENDPOINT_PAUSE





Endpoint Superstate         Miniport Hardware State

GEp_Init                    ENDPOINT_CLOSED
GEp_Opened                  ENDPOINT_PAUSE
GEp_ClosePending            ENDPOINT_TRANSITION,ENDPOINT_REMOVE
GEp_Closed                  ENDPOINT_CLOSED
GEp_PausePending            ENDPOINT_TRANSITION,ENDPOINT_PAUSE
GEp_Paused                  ENDPOINT_PAUSE
GEp_Active                  ENDPOINT_ACTIVE



//geOP
Ev_gEp_Open
-endpoint is created and put on the GlobalEndpointList
--------------------------------------------------------------------------------------------------------------
//geCL
Ev_gEp_Close
-endpoint is closed, removed from the GlobalEndpointList.  A closed endpoint cannot be referenced.
--------------------------------------------------------------------------------------------------------------
//geRM
Ev_gEp_Remove
--------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------

USB Queue head synchronization routines.

** This is a modification from earlier versions of the port driver.
** Hardware synchronization is now left to the miniport.

Endpoints are added to the state change list when they need to be synchronized with the controller harwdare schedule.
The initial request is passed to the miniport by issueing a MP_SetEndpointStateEx.  The miniport is polled on each
hardware interrupt until it indicates the status has changed.  This allows the ehci controller driver to make better
use of hardware synch prmatives.

For USB 1.1 controller compatibility all endpoints transition on the next SOF -- this is done automatically by usbport.


Not all endpoint changes require an interrupt (hardware sync), by only waiting on those that need it we are able to
decrease the latency
associated with openaing an isoch endpoint.


+----------------------+---------------+----------------+----------------+----------------+---------------+
     current/Next      | ENDPOINT_IDLE |ENDPOINT_PAUSE  |ENDPOINT_ACTIVE |ENDPOINT_REMOVE |ENDPOINT_CLOSED|
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_IDLE      |      x        |       tbd      |        tbd     |      tbd       |      tbd      |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_PAUSE     |      i        |       x        |        i       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_ACTIVE    |      x        |      sync      |        x       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_REMOVE    |      x        |       x        |                |       x        |      i        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
   *ENDPOINT_CLOSED    |      x        |       x        |        i       |       i        |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+

*ENDPOINT_CLOSED is initial state
x       = invalid
sync    = hw sync(ie next sof)
i       = immediate


  //geOP 1
    Ev_gEp_Open = 1,
    //geCL 2
    Ev_gEp_ReqRemove,
    //geRM 3
    Ev_gEp_Close,
    // 4
    Ev_gEp_ReqActive,
    // 5
    Ev_gEp_ReqPause,
    // 6
    Ev_gEp_HwSync,
    // 7
    Ev_gEp_Reset,
    // 8
    // special states for handling iocancelirp
    // endpoint needs to be paused for a canceled transfer, it is resumed after canceled transfers are flushed
    // off the hardware
    Ev_gEp_ReqPauseCancel,
    // 9
    Ev_gEp_ReqActiveCancel,
    // 10
    // indicates a forced hardware sync on power management
    Ev_gEp_PM_HwSync,
    // 11
    // request a resync, activate on enpoint following suspend
    Ev_gEp_PM_ReqResume,
    // 12
    Ev_gEp_PM_ReqSuspend,
    // 13
    // replaces the 'Zapped' guard (flag)
    Ev_gEp_Zap,
    // 14
    Ev_gEp_PollEndpoint,
    // 15
    Ev_gEp_AbortTransfer,
    // 16
    Ev_gEp_SubmitTransfer,
    // 17
    Ev_gEp_Clone,



  current/ev            Open                Close                   ReqRemove
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                GEp_Opened(PAUSE)   GEp_Closed              GEp_Removed
GEp_RemovePending       x                   GEp_RemovePending*      GEp_RemovePending(TRANSITION)
GEp_Closed              x                   x                       x
GEp_Active              x                   x                       GEp_RemovePending(TRANSITION)
GEp_Paused              x                   x                       GEp_RemovePending(TRANSITION)
GEp_PausePending        x                   x                       x
GEp_Removed             x                   GEp_Closed              GEp_Removed
GEp_PausePendingCancel  x                   x
GEp_Free                x                   x
GEp_Unknown             x                   x
GEp_Zapped              x                   x
=================================================================================================================

  current/ev            HwSync              ReqActive               ReqPause
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                x                   x                       x
GEp_RemovePending       GEp_Removed         x                       x
GEp_Closed              x                   x                       x
GEp_Active              x                   GEp_Active              GEp_PausePending(TRANSITION)
GEp_Paused              x                   GEp_Active(ACTIVE)      GEp_Paused(PAUSE)
GEp_PausePending        GEp_Paused(PAUSE)   x                       GEp_PausePending(TRANSITION)
GEp_Removed             x                   x                       x
GEp_PausePendingCancel
GEp_Free
GEp_Unknown
GEp_Zapped
=================================================================================================================

  current/ev            Reset               ReqPauseCancel           ReqActiveCancel
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                x
GEp_RemovePending       x
GEp_Closed              x
GEp_Active              x
GEp_Paused              x
GEp_PausePending        x
GEp_Removed             x
GEp_PausePendingCancel  x
GEp_Free                x
GEp_Unknown             x
GEp_Zapped              x
=================================================================================================================


  current/ev            PM_HwSync           PM_ReqResume            PM_ReqSuspend
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                x
GEp_RemovePending       x
GEp_Closed              x
GEp_Active              x
GEp_Paused              x
GEp_PausePending        x
GEp_Removed             x
GEp_PausePendingCancel  x
GEp_Free                x
GEp_Unknown             x
GEp_Zapped              x
=================================================================================================================


  current/ev            Zap                 PollEndpoint            AbortTransfer
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                GEp_Zapped(CLOSED)
GEp_RemovePending       GEp_Zapped(CLOSED)
GEp_Closed              GEp_Zapped(CLOSED)
GEp_Active              GEp_Zapped(CLOSED)
GEp_Paused              GEp_Zapped(CLOSED)
GEp_PausePending        GEp_Zapped(CLOSED)
GEp_Removed             GEp_Zapped(CLOSED)
GEp_PausePendingCancel  GEp_Zapped(CLOSED)
GEp_Free                GEp_Zapped(CLOSED)
GEp_Unknown             GEp_Zapped(CLOSED)
GEp_Zapped              GEp_Zapped
=================================================================================================================


  current/ev            SubmitTransfer      Clone
+-----------------------------------------------------------------------------------------------------------------

GEp_Init                x
GEp_RemovePending       x
GEp_Closed              x
GEp_Active              x
GEp_Paused              x
GEp_PausePending        x
GEp_Removed             x
GEp_PausePendingCancel  x
GEp_Free                x
GEp_Unknown             x
GEp_Zapped              x
=================================================================================================================

*/

HC_GLOBAL_EP_STATE
USBPORT_SetGlobalEndpointStateAndWait(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event,
    __in MP_ENDPOINT_STATE TransitionState
    )
/*++

Routine Description:

    Sets the 'global' state of the endpoint based on the event code passed in.

    This version waits
--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    HC_GLOBAL_EP_STATE geps;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'sep1', 0, Endpoint, Event);
    ASSERT_ENDPOINT(Endpoint);
    USBPORT_AcquireEpListLock(HcFdo, &irql);

    geps = USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Event);

    USBPORT_ReleaseEpListLock(HcFdo, irql);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'sep2', 0, Endpoint, Event);
    ASSERT_PASSIVE();
    USBPORT_WaitEpStateEvent(HcFdo, Endpoint, TransitionState);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'sep4', 0, Endpoint, 0);

    return geps;

}


HC_GLOBAL_EP_STATE
USBPORT_SetGlobalEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event
    )
/*++

Routine Description:

    Sets the 'global' state of the endpoint based on the event code passed in.



--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    HC_GLOBAL_EP_STATE geps;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'sep1', 0, Endpoint, Event);
    ASSERT_ENDPOINT(Endpoint);
    USBPORT_AcquireEpListLock(HcFdo, &irql);

    geps = USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Event);

    USBPORT_ReleaseEpListLock(HcFdo, irql);

    return geps;

}


HC_GLOBAL_EP_STATE
USBPORT_iSetGlobalEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event
    )
/*++


--*/
{
    return USBPORT_iSetGlobalEndpointStateTx(HcFdo, Endpoint, Event, NULL, NULL, NULL);
}


HC_GLOBAL_EP_STATE
USBPORT_iSetGlobalEndpointStateTx(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer,
    __out_opt USB_MINIPORT_STATUS *MpStatus,
    __inout_opt PULONG BytesTransferred
    )
/*++

Routine Description:

    Sets the 'global' state of the endpoint based on the event code passed in.

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE geps;
    GEP_STATE_LOG_ENTRY *newLog;
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'seI1', 0, Endpoint, Event);
    USBPORT_iAssertEndpoint(HcFdo, Endpoint);
    // this routine should hold global ep list lock
    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);

    geps = Endpoint->GepState;

    Endpoint->GepLogIdx=(Endpoint->GepLogIdx+1) % NUM_GEP_STATE_LOG_ENTRIES;
    newLog = &Endpoint->GepLog[Endpoint->GepLogIdx];

    newLog->Event = Event;
    newLog->CurrentState = geps;


    switch(Event) {
    case Ev_gEp_PollEndpoint:
    case Ev_gEp_SubmitTransfer:
    case Ev_gEp_AbortTransfer:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI8', Endpoint->CurrentEpState, Endpoint, Endpoint->GepState);
        switch (Endpoint->GepState) {
        case GEp_Closed:
        case GEp_Init:
        case GEp_Unknown:
            // invalid states
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;

        case GEp_Zapped:
            // bypass the miniport for zapped endpoints
            mps = USBMP_STATUS_SUCCESS;
            break;

        default:
            // access the miniport
            if (Event == Ev_gEp_PollEndpoint) {
                // poll the miniport for completed transfers
                mps = USBMP_STATUS_SUCCESS;
                MPf_PollEndpoint(devExt, Endpoint);
            } else if (Event == Ev_gEp_SubmitTransfer && Transfer) {
                mps = MPx_SubmitTransfer(devExt, Endpoint, Transfer);
            } else if (Event == Ev_gEp_AbortTransfer && Transfer) {
                mps = USBMP_STATUS_SUCCESS;
                MPf_AbortTransfer(devExt, Endpoint, Transfer, BytesTransferred);
            } else {
                USBPORT_ASSERTMSG("Unexpected Endpoint Event",FALSE);
            }
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Reset:
        // Reset (clone) opertions occur only on enpoints that are
        // 'ZAPPED' (not in the miniport). This event returns the
        // endpoint to the same state just after init.
        USBPORT_ASSERT(Endpoint->GepState == GEp_Zapped);
        geps = Endpoint->GepState = GEp_Paused;
        Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
        Endpoint->EpFlags.DeviceGone = 0;
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Reopen:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI2', 0, Endpoint, Endpoint->GepState);
        switch (Endpoint->GepState) {

        case GEp_Zapped:
            //re-open

            geps = Endpoint->GepState = GEp_Paused;
            Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
            break;

        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Open:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI2', 0, Endpoint, Endpoint->GepState);
        switch (Endpoint->GepState) {
        case GEp_Init:
            InsertTailList(&devExt->Fdo.GlobalEndpointList, &Endpoint->GlobalLink);
            geps = Endpoint->GepState = GEp_Paused;
            Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
            // should call this in to miniport
            break;

        //case GEp_Removed:
        //    //re-open

        //    geps = Endpoint->GepState = GEp_Paused;
        //    Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
        //    break;

        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

//    case Ev_gEp_PM_ReqSuspend:
//
//        Endpoint->EpFlags.HcSusupended = 1;
//        geps = Endpoint->GepState = GEp_Unknown;
//
//        newLog->NextState = geps;
//        newLog->i.HwEpState = Endpoint->CurrentEpState;
//        break;

    case Ev_gEp_PM_ReqResume:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI3', Endpoint->CurrentEpState, Endpoint, Endpoint->GepState);
        switch (geps) {
        case GEp_Unknown:
            //Endpoint->EpFlags.HcSusupended = 0;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'seI4', 0, Endpoint, Endpoint->GepState);
            // on resume, curent state may be undefined
            Endpoint->CurrentEpState = MPf_GetEndpointState(devExt, Endpoint);
            switch(Endpoint->CurrentEpState) {
            case ENDPOINT_REMOVE:
                geps = Endpoint->GepState = GEp_Removed;
                break;
            case ENDPOINT_PAUSE:
                geps = Endpoint->GepState = GEp_Paused;
                break;
            case ENDPOINT_ACTIVE:
                geps = Endpoint->GepState = GEp_Active;
                break;
            default:
                USBPORT_ASSERTMSG("Unexpected Endpoint->CurrentEpState",FALSE);
            }
            // set the endpoint state to Active, RUN
            MPf_SetEndpointStatus(devExt, Endpoint, ENDPOINT_STATUS_RUN);
            break;

        case GEp_Zapped:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'se5x', Endpoint->CurrentEpState, Endpoint, Endpoint->GepState);

            geps = Endpoint->GepState;
            break;

        default:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'seI5', Endpoint->CurrentEpState, Endpoint, Endpoint->GepState);

            geps = Endpoint->GepState;

#if DBG
            // sanity check that we are in sync with the miniport
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
             // set the endpoint state to Active, RUN
            MPf_SetEndpointStatus(devExt, Endpoint, ENDPOINT_STATUS_RUN);
        }
        // set the endpoint state to Active, RUN
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;


    case Ev_gEp_ReqActiveCancel:
    case Ev_gEp_ReqActive:

        geps = Endpoint->GepState;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI6', 0, Endpoint, geps);
        switch (geps) {
        case GEp_Active:
#if DBG
            // sanity check that we are in sync with the miniport
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
            // already active
            break;

        case GEp_Paused:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'seI7', 0, Endpoint, geps);
            // activate the endpoint
            //if (Endpoint->EpFlags.HcSusupended) {
            //    TEST_TRAP();
            //    geps = Endpoint->GepState;
            //} else {
            geps = Endpoint->GepState = USBPORT_SetEp_ACTIVE(HcFdo, Endpoint);

            // push transfers to the miniport
            USBPORT_Core_iEndpointActive(HcFdo, Endpoint);
            break;

        case GEp_PausedClearTTNeeded:
            geps = Endpoint->GepState = GEp_ActiveClearTTNeeded;
            break;
        case GEp_PausedClearTTPending:
            geps = Endpoint->GepState = GEp_ActiveClearTTPending;
            break;
        case GEp_ActiveClearTTNeeded:
        case GEp_ActiveClearTTPending:
            // NOP
            break;

        case GEp_Zapped:
        case GEp_Removed:
        case GEp_RemovePending:
            LOGENTRY(HcFdo, LOG_TRANSFER, 'seI8', 0, Endpoint, geps);
            // ignore attempts to transition to active if remove or remove pending
            break;

        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_ReqPauseCancel:

    case Ev_gEp_ReqPause:

        geps = Endpoint->GepState;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'seI9', 0, Endpoint, geps);
        switch (geps) {
        case GEp_Paused:
#if DBG
            // sanity check that we are in sync with the miniport
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
            //already paused, (should not see this but we handle it in any case)
            if (Event == Ev_gEp_ReqPauseCancel) {
                USBPORT_Core_iEndpointPaused(HcFdo, Endpoint);
            }
            break;

        case GEp_PausedClearTTNeeded:
        case GEp_PausedClearTTPending:
            // NOP
            break;
        case GEp_ActiveClearTTNeeded:
            geps = Endpoint->GepState = GEp_PausedClearTTNeeded;
            break;
        case GEp_ActiveClearTTPending:
            geps = Endpoint->GepState = GEp_PausedClearTTPending;
            break;

        case GEp_Active:
            geps = Endpoint->GepState = USBPORT_SetEp_PAUSE(HcFdo, Event, Endpoint);
            break;

        case GEp_PausePendingCancel:
            geps = Endpoint->GepState = GEp_PausePending;
            break;

        case GEp_PausePending:
            // pause is already pending, this most likely means two pause requests are comming from different threads
            geps = Endpoint->GepState = GEp_PausePending;
            break;

        case GEp_Zapped:
            geps = Endpoint->GepState;
            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
            USBPORT_ASSERT(Endpoint->CurrentEpState == ENDPOINT_CLOSED);
            break;

        case GEp_Removed:
            // cannot pause a removed endpoint
            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
            geps = Endpoint->GepState;
            break;

        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_PM_HwSync:
    case Ev_gEp_HwSync:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se10', 0, Endpoint, Endpoint->GepState);
        switch (Endpoint->GepState) {
        // transition complete
        case GEp_PausePendingCancel:

            if (Event == Ev_gEp_PM_HwSync) {
                geps = Endpoint->GepState = GEp_Unknown;
            } else {
                geps = Endpoint->GepState = GEp_Paused;
            }

            // flush transfer from the miniport
            Endpoint->CurrentEpState = Endpoint->PendingEpState;

            newLog->NextState = geps;
            newLog->i.HwEpState = Endpoint->CurrentEpState;

            if (MpStatus) {
                *MpStatus = mps;
            }

#if DBG
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
            USBPORT_Core_iEndpointPaused(HcFdo, Endpoint);



            // (go_active) automatically return to active state
            LOGENTRY(HcFdo, LOG_TRANSFER, 'se15', 0, Endpoint, Endpoint->GepState);
            geps = Endpoint->GepState = USBPORT_iSetGlobalEndpointState(HcFdo, Endpoint, Ev_gEp_ReqActiveCancel);

            // exit here since USBPORT_Core_iEndpointPaused will re-enter
            return geps;

        // transition complete
        case GEp_PausePending:
            if (Event == Ev_gEp_PM_HwSync) {
                geps = Endpoint->GepState = GEp_Unknown;
            } else {
                geps = Endpoint->GepState = GEp_Paused;
            }

            Endpoint->CurrentEpState = Endpoint->PendingEpState;
            // flush transfer from the miniport

            newLog->NextState = geps;
            newLog->i.HwEpState = Endpoint->CurrentEpState;

            if (MpStatus) {
                *MpStatus = mps;
            }

#if DBG
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
            USBPORT_Core_iEndpointPaused(HcFdo, Endpoint);

            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);

            geps = Endpoint->GepState;

            // exit here since USBPORT_Core_iEndpointPaused will re-enter
            return geps;

        case GEp_RemovePending:
            // transition complete
            if (Event == Ev_gEp_PM_HwSync) {
                geps = Endpoint->GepState = GEp_Unknown;
            } else {
                geps = Endpoint->GepState = GEp_Removed;
            }
            Endpoint->CurrentEpState = Endpoint->PendingEpState;

            newLog->NextState = geps;
            newLog->i.HwEpState = Endpoint->CurrentEpState;

            if (MpStatus) {
                *MpStatus = mps;
            }

#if DBG
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif
            // flush transfers
            USBPORT_Core_iEndpointPaused(HcFdo, Endpoint);

            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);

            geps = Endpoint->GepState;

            // exit here since USBPORT_Core_iEndpointPaused will re-enter
            return geps;

        case GEp_Zapped:
        default:
            break;
        }

        if (Event == Ev_gEp_PM_HwSync) {
            LOGENTRY(HcFdo, LOG_TRANSFER, 'se11', 0, Endpoint, Endpoint->GepState);
            geps = Endpoint->GepState = GEp_Unknown;
            //Endpoint->EpFlags.HcSusupended = 1;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_ReqRemove:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se12', 0, Endpoint, Endpoint->GepState);
        // this locks out refs to the endpoint
        switch (Endpoint->GepState) {
        case GEp_PausePendingCancel:
        case GEp_Paused:
        case GEp_Active:
            geps = Endpoint->GepState = USBPORT_SetEp_REMOVE(HcFdo, Endpoint);
            break;

        case GEp_Zapped:
            // endpoint is already closed in the miniport
            geps = Endpoint->GepState;
            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);
            USBPORT_ASSERT(Endpoint->CurrentEpState == ENDPOINT_CLOSED);
            break;

        default:
            geps = Endpoint->GepState;
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Close:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se13', 0, Endpoint, Endpoint->GepState);
        switch (Endpoint->GepState) {
        case GEp_Removed:
            RemoveEntryList(&Endpoint->GlobalLink);
            InitializeListHead(&Endpoint->GlobalLink);

            Endpoint->GepState = USBPORT_SetEp_CLOSED(HcFdo, Endpoint);
            break;

        case GEp_Init:
            // endpoint is not on the HW or on global list
            TEST_TRAP();
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;
            Endpoint->GepState = GEp_Closed;
            break;

        case GEp_Zapped:
            RemoveEntryList(&Endpoint->GlobalLink);
            InitializeListHead(&Endpoint->GlobalLink);

            // endpoint is already closed in the miniport
            Endpoint->GepState = GEp_Closed;
            USBPORT_ASSERT(Endpoint->CurrentEpState == ENDPOINT_CLOSED);
            break;

        case GEp_Unknown:
            if (Endpoint->GlobalLink.Flink != NULL &&
                Endpoint->GlobalLink.Blink != NULL) {
                //
                // Unlink the endpoint from the Global List to prevent a bugcheck
                // later.
                //
                RemoveEntryList(&Endpoint->GlobalLink);
                InitializeListHead(&Endpoint->GlobalLink);
            }
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);

        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);
        }

        newLog->NextState = geps = GEp_Free;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Poke:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se18', 0, Endpoint, Endpoint->GepState);

        // zapping an endpoint implies a close the endpoint should not be referenced by the
        // miniport once it has been zapped.
        geps = Endpoint->GepState;

        switch (geps) {
        case GEp_Zapped:

#if DBG
            Endpoint->PendingEpState = Endpoint->CurrentEpState;
            USBPORT_ASSERT(Endpoint->PendingEpState == ENDPOINT_CLOSED);
#endif
            break;

        case GEp_Removed:

#if DBG
            // sanity check that we are in sync with the miniport
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif

            // notify miniport
            MPf_CloseEndpoint(devExt, Endpoint);
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;

            //indicate endpoint structure will be re-initialized
            MPf_DbgFreeEndpoint(devExt, Endpoint);
            // zero miniport data

            USBPORT_ASSERT(Endpoint->MiniportEndpointDataSize == REGISTRATION_PACKET(devExt).EndpointDataSize);
            RtlZeroMemory(&Endpoint->MiniportEndpointData[0], Endpoint->MiniportEndpointDataSize);

            // this is the same as zapping the endpoint. we canot re-open it once it is freed.
            geps = GEp_Zapped;
            break;

        default:

            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState",FALSE);

        }


        newLog->NextState = Endpoint->GepState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Zap:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se16', 0, Endpoint, Endpoint->GepState);

        // zapping an endpoint implies a close the endpoint should not be referenced by the
        // miniport once it has been zapped.
        geps = Endpoint->GepState;

        switch (geps) {
        case GEp_Zapped:
            // already zapped is a nop
            break;

        case GEp_Init:
            // endpoint was not opened
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;
            break;

        default:
            // controller harwdare should be stopped here, REMOVE should happen
            // immediatly
            MPf_SetEndpointState(devExt, Endpoint, ENDPOINT_REMOVE);
            Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_REMOVE;
#if DBG
            // sanity check that we are in sync with the miniport
            USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif

            // notify miniport
            MPf_CloseEndpoint(devExt, Endpoint);
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;

            // miniport no longer has access to the endpoint
            MPf_DbgFreeEndpoint(devExt, Endpoint);
            // zero miniport data

            USBPORT_ASSERT(Endpoint->MiniportEndpointDataSize == REGISTRATION_PACKET(devExt).EndpointDataSize);
            //RtlZeroMemory(&Endpoint->MiniportEndpointData[0], Endpoint->MiniportEndpointDataSize);

            // If an endpoint close operation is blocked waiting for the
            // Ev_gEp_ReqPause or Ev_gEp_ReqRemove to complete it can
            // unblock and complete now as the miniport can no longer
            // access the endpoint after the Ev_gEp_Zap.
            //
            if (geps == GEp_PausePending) {
                USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
            } else if (geps == GEp_RemovePending) {
                USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);
            }
        }

        geps = Endpoint->GepState = GEp_Zapped;
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Clone:

        LOGENTRY(HcFdo, LOG_TRANSFER, 'se17', 0, Endpoint, Endpoint->GepState);

        geps = Endpoint->GepState;

        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_Rebalance:
        // currently this just logs the rebalance operation realtive to the
        // endpoint state
        LOGENTRY(HcFdo, LOG_TRANSFER, 'se18', 0, Endpoint, Endpoint->GepState);

        geps = Endpoint->GepState;

        switch (geps) {
        case GEp_Zapped:
            break;
        default:
            MPx_RebalanceEndpoint(devExt, Endpoint);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_ClearTTNeeded:
        geps = Endpoint->GepState;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'se21', 0, Endpoint, geps);
        switch (geps) {
        case GEp_Paused:
            geps = Endpoint->GepState = GEp_PausedClearTTNeeded;
            break;
        case GEp_PausedClearTTNeeded:
        case GEp_PausedClearTTPending:
        case GEp_ActiveClearTTNeeded:
        case GEp_ActiveClearTTPending:
            // NOP
            break;
        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState", FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_ClearTTIssued:
        geps = Endpoint->GepState;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'se22', 0, Endpoint, geps);
        switch (geps) {
        case GEp_PausedClearTTNeeded:
            geps = Endpoint->GepState = GEp_PausedClearTTPending;
            break;
        case GEp_ActiveClearTTNeeded:
            geps = Endpoint->GepState = GEp_ActiveClearTTPending;
            break;
        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState", FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    case Ev_gEp_ClearTTComplete:
        geps = Endpoint->GepState;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'se23', 0, Endpoint, geps);
        switch (geps) {
        case GEp_PausedClearTTPending:
            geps = Endpoint->GepState = GEp_Paused;
            break;
        case GEp_ActiveClearTTPending:
            geps = Endpoint->GepState = USBPORT_SetEp_ACTIVE(HcFdo, Endpoint);
            USBPORT_Core_iEndpointActive(HcFdo, Endpoint);
            break;
        case GEp_Paused:
            // Reached due to USBPORT_CloseEndpoint() calling
            // USBPORT_ClearTtBufferAndWait() after pausing the
            // endpoint.
            // NOP
            break;
        default:
            USBPORT_ASSERTMSG("Unexpected Endpoint->GepState", FALSE);
            break;
        }
        newLog->NextState = geps;
        newLog->i.HwEpState = Endpoint->CurrentEpState;
        break;

    default:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'seXX', 0, Endpoint, Endpoint->GepState);
        USBPORT_ASSERTMSG("Unexpected Endpoint Event",FALSE);
        break;
    }

    //


    if (MpStatus) {
        *MpStatus = mps;
    }
    return geps;
}


NTSTATUS
USBPORT_OpenEndpointRefList(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Sets up the endpoint reference list for a given endpoint.

Arguments:


Return Value:

    none

--*/
{
    NTSTATUS nts;

    InitializeListHead(&Endpoint->EpRefListHead);
    KeInitializeEvent(&Endpoint->EpClosedEvent, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

    // keep one treference to ourselves
    nts = USBPORT_ReferenceEndpoint(HcFdo, Endpoint, &Endpoint->OpenEpRefHandle, Endpoint, EPREF_OPEN_TAG);

    return nts;
}


VOID
USBPORT_CloseEndpointRefList(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    removes the initial ref on the endpoint from its creation and waits for any outstanding
    refs to be removed.

Arguments:


Return Value:

    none

--*/
{

    USBPORT_DereferenceEndpoint(HcFdo, Endpoint->OpenEpRefHandle);

    KeWaitForSingleObject(&Endpoint->EpClosedEvent,
                           Suspended,
                           KernelMode,
                           FALSE,
                           NULL);


}


PHCD_ENDPOINT
USBPORT_EndpointFromHandle(
    PUSBOBJ_REF EpRefHandle
    )
/*++

Routine Description:

    Returns a pointer to the endpoint give a valid reference handle.

Arguments:

    EpRefHandle - enpoint refence handle allocated with USBPORT_ReferenceEndpoint.

Return Value:

   none

--*/
{

    NT_ASSERT(EpRefHandle != USB_ZERO_BW_ENDP_HANDLE);

    ASSERT_EPHANDLE(EpRefHandle);

    ASSERT_ENDPOINT(EpRefHandle->Endpoint);

    return EpRefHandle->Endpoint;

}


NTSTATUS
USBPORT_iReferenceEndpointObject(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF EpRefHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    References the endpoint object structure for internal use by the port driver. This routine takes a handle that
    is already allocated or embedded in another structure.

Arguments:

    Endpoint - endpoint to reference

    EpRefHandle - pointer to ep ref handle allocated from non-paged pool.

Return Value:

    STATUS_SUCCES if the endpoint could be refenced and handle created otherwise error.

--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS nts;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    if (Endpoint == NULL) {
        // if NULL passed in fail the request
        return STATUS_INVALID_PARAMETER;
    }


    USBPORT_iAssertEndpoint(HcFdo, Endpoint);

    do {
        //USBPORT_ASSERT(DeviceHandle->DhState != Devh_Deleted);

        if (Endpoint->GepState == GEp_RemovePending ||
            Endpoint->GepState == GEp_Removed ||
            Endpoint->GepState == GEp_Closed) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        if(EpRefHandle != &Endpoint->EpRefFP) {
            EpRefHandle->Endpoint = Endpoint;
            EpRefHandle->Sig = SIG_EPREF;
            EpRefHandle->Object = Object;
            EpRefHandle->Tag = Tag;
            EpRefHandle->Thread = KeGetCurrentThread();
            LOGENTRY(HcFdo, LOG_MISC, 'epr+', Endpoint, EpRefHandle, Object);

            InsertTailList(&Endpoint->EpRefListHead,  &EpRefHandle->UsbRefLink);
        } else {
            // Allocation fails use the resource-less ref count
            LOGENTRY(HcFdo, LOG_MISC, 'epR+', Endpoint, NULL, Object);
            Endpoint->EpRefCountNoRes++;
        }

        nts = STATUS_SUCCESS;

    } WHILE (0);


    return nts;
}


NTSTATUS
USBPORT_iReferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF *EpRefHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    References the endpoint object structure for internal use by the port driver. This returns a handle corresponding
    to this specific reference on the endpoint. Structures that hold pointers to endpoints should also hold a reference
    handle and should use this handle to access the endpoint by calling USBPORT_EndpointFromHandle. The endpoint will
    not be freed until all references are removed.

Arguments:

    Endpoint - endpoint to reference

    EpRefHandle - pointer filled in with ep ref handle allocated from non-paged pool.

Return Value:

    STATUS_SUCCES if the endpoint could be refenced and handle created otherwise error.

--*/
{
    PUSBOBJ_REF epRefObj;
    PDEVICE_EXTENSION devExt;
    NTSTATUS nts;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    if (EpRefHandle == NULL) {
        // if NULL passed in fail the request
        return STATUS_INVALID_PARAMETER;
    }

    *EpRefHandle = NULL;

    if (Endpoint == NULL) {
        // if NULL passed in fail the request
        return STATUS_INVALID_PARAMETER;
    }

    epRefObj = NULL;

#ifndef USB_FORCE_MEMORY_STRESS
    ALLOC_POOL_Z(epRefObj, NonPagedPoolNx, sizeof(struct _USBOBJ_REF));
#endif

    // Allocation failed, use the backup ref obj
    if (epRefObj == NULL) {
        epRefObj = &Endpoint->EpRefFP;
    }

    nts = USBPORT_iReferenceEndpointObject(HcFdo, Endpoint, epRefObj, Object, Tag);

    if (NT_SUCCESS(nts)) {
        *EpRefHandle = epRefObj;
    } else if (epRefObj != &Endpoint->EpRefFP) {
        NT_ASSERT(epRefObj);
        UsbFreePool( epRefObj);
    }

    return nts;

}


NTSTATUS
USBPORT_ReferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF *EpRefHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    see USBPORT_ReferenceEndpoint, this is used when the Global endpoint list lock is not yet acquired.

Arguments:

    Endpoint - endpoint to reference

    EpRefHandle - pointer filled in with ep ref handle allocated from non-paged pool.

Return Value:

    STATUS_SUCCES if the endpoint could be refenced and handle created otherwise error.

--*/
{
    KIRQL irql;
    NTSTATUS nts;

    USBPORT_AcquireEpListLock(HcFdo, &irql);
    nts = USBPORT_iReferenceEndpoint(HcFdo, Endpoint, EpRefHandle, Object, Tag);
    USBPORT_ReleaseEpListLock(HcFdo, irql);

    return nts;

}

NTSTATUS
USBPORT_ReferenceAllEndpoints(
    PDEVICE_OBJECT  FdoDeviceObject,
    ULONG           Tag,
    PLIST_ENTRY     ListHead
    )
/*++

Routine Description:

    This routine take a reference on each endpoint for a host controller
    so that the endpoints do not get deleted while the references are
    outstanding.

Arguments:

    FdoDeviceObject - Host controller FDO for which all endpoints should
    be referenced.

    Tag - Signature that IDs these references.

    ListHead - The list of the USBOBJ_REF_EX references is returned in
    this caller supplied list head.  The references on this list should
    be individually released by calling USBPORT_DereferenceEndpoint().

Return Value:

    Error if the endoints cannot all be referenced.  ListHead is not
    necessarily empty in that case.

--*/
{
    PDEVICE_EXTENSION   devExt;
    PKTHREAD            thread;
    KIRQL               irql;
    PLIST_ENTRY         listEntry;
    PHCD_ENDPOINT       endpoint;
    PUSBOBJ_REF_EX      epRefObjEx;
    NTSTATUS            nts;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Initialize the caller's reference list as an empty list.
    //
    InitializeListHead(ListHead);

    // STATUS_SUCCESS will be returned unless a reference pool
    // allocation fails.
    //
    nts = STATUS_SUCCESS;

    // Store the current thread in the endpoint references to match the
    // behavior of other endpoint references.
    //
    thread = KeGetCurrentThread();

    // Acquire the host controller endpoint list spinlock while walking
    // the list of endpoints.
    //
    USBPORT_AcquireEpListLock(FdoDeviceObject, &irql);

    listEntry = devExt->Fdo.GlobalEndpointList.Flink;

    while (listEntry != &devExt->Fdo.GlobalEndpointList) {

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
            listEntry,
            struct _HCD_ENDPOINT,
            GlobalLink);

        // Only reference endpoints in the same valid states that
        // USBPORT_iReferenceEndpointObject() allows.
        //
        if (!(endpoint->GepState == GEp_RemovePending ||
              endpoint->GepState == GEp_Removed ||
              endpoint->GepState == GEp_Closed)) {

            // Allocate a USBOBJ_REF_EX to contain the reference
            //
            ALLOC_POOL_Z(epRefObjEx,
                         NonPagedPoolNx,
                         sizeof(struct _USBOBJ_REF_EX));

            if (!epRefObjEx) {
                nts = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            // Initialize the reference
            //
            epRefObjEx->Endpoint = endpoint;
            epRefObjEx->Sig = SIG_EPREF;
            epRefObjEx->Object = NULL;  // Don't have a use for this here.
            epRefObjEx->Tag = Tag;
            epRefObjEx->Thread = thread;

            // Link the reference onto the endpoint reference list.
            //
            InsertTailList(&endpoint->EpRefListHead,  &epRefObjEx->UsbRefLink);

            // Link the reference onto the callers list.
            //
            InsertTailList(ListHead, &epRefObjEx->ExRefLink);
        }

        // Advance to the next endpoint on the list.
        //
        listEntry = endpoint->GlobalLink.Flink;
    }

    // Done walking the list of endpoints, release the endpoint list
    // spinlock now.
    //
    USBPORT_ReleaseEpListLock(FdoDeviceObject, irql);

    return nts;
}

VOID
USBPORT_iDereferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PUSBOBJ_REF EpRefHandle
    )
/*++

Routine Description:

    Dereferences an endpoint based and releases the EpRefHandle structure. Caller must free the handle
    if it was allocated from non-paged pool.

Arguments:

    EpRefHandle - ref handle allocated with a previous call to USBPORT_ReferenceEndpoint

Return Value:

    none.

--*/
{
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);


    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);

    endpoint = EpRefHandle->Endpoint;
    USBPORT_iAssertEndpoint(HcFdo, endpoint);

    if (EpRefHandle == &endpoint->EpRefFP) {
        USBPORT_ASSERT(endpoint->EpRefCountNoRes);
        if (endpoint->EpRefCountNoRes) {
            endpoint->EpRefCountNoRes--;
        }
    } else {
        RemoveEntryList(&EpRefHandle->UsbRefLink);
        USB_ZAP_LINK(&EpRefHandle->UsbRefLink);

        UsbFreePool( EpRefHandle);
    }

    if (IsListEmpty(&endpoint->EpRefListHead) &&
        !endpoint->EpRefCountNoRes) {
        KeSetEvent(&endpoint->EpClosedEvent, IO_NO_INCREMENT, FALSE);
    }
}


VOID
USBPORT_DereferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PUSBOBJ_REF EpRefHandle
    )
/*++

Routine Description:

    Dereferences an endpoint based and releases the EpRefHandle structure. This routine frees the refhandle
    structure.

Arguments:

Return Value:

    none.

--*/
{
    KIRQL irql;

    USBPORT_AcquireEpListLock(HcFdo, &irql);
    USBPORT_iDereferenceEndpoint(HcFdo, EpRefHandle);
    USBPORT_ReleaseEpListLock(HcFdo, irql);

}



/*

USB Queue head synchronization routines.

** This is a modification from earlier versions of the port driver.
** Hardware synchronization is now left to the miniport.

Endpoints are added to the state change list when they need to be synchronized with the controller harwdare schedule.
The initial request is passed to the miniport by issueing a MP_SetEndpointStateEx.  The miniport is polled on each
hardware interrupt until it indicates the status has changed.  This allows the ehci controller driver to make better
use of hardware synch prmatives.

For USB 1.1 controller compatibility all endpoints transition on the next SOF -- this is done automatically by usbport.


Not all endpoint changes require an interrupt (hardware sync), by only waiting on those that need it we are able to
decrease the latency
associated with openaing an isoch endpoint.


+----------------------+---------------+----------------+----------------+----------------+---------------+
     current/Next      | ENDPOINT_IDLE |ENDPOINT_PAUSE  |ENDPOINT_ACTIVE |ENDPOINT_REMOVE |ENDPOINT_CLOSED|
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_IDLE      |      x        |                |                |                |               |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_PAUSE     |      i        |       x        |        i       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_ACTIVE    |      x        |      sync      |        x       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_REMOVE    |      x        |       x        |        x       |       x        |      i        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_CLOSED    |      x        |       x        |        x       |       i        |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+

*ENDPOINT_CLOSED is initial state
x       = invalid
sync    = hw sync(ie next sof)
i       = immediate


*/


HC_GLOBAL_EP_STATE
USBPORT_SetEp_ACTIVE(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Transition to active state, active endpoints are already in the schedule but may be disabled.

    valid transitions

    ENDPOINT_PAUSE->ENDPOINT_ACTIVE

Arguments:

Return Value:

    Global Endpoint State.

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE gepState;

    ASSERT_ENDPOINT(Endpoint);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    gepState = Endpoint->GepState;

    switch (Endpoint->CurrentEpState) {

    case ENDPOINT_PAUSE:
        if (!Endpoint->EpFlags.RootHub) {

            // The endpoint shouldn't be activated again until all of
            // the canceled transfers waiting for a CLEAR_TT_BUFFER
            // request completion have been completed.
            //
            USBPORT_ASSERT(IsListEmpty(&Endpoint->TxCanceledPendingClearTTCompletionListHead));
            USBPORT_ASSERT(IsListEmpty(&Endpoint->TxDonePendingClearTTCompletionListHead));

            MPf_SetEndpointState(devExt, Endpoint, ENDPOINT_ACTIVE);
        }
        Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_ACTIVE;
        gepState = Endpoint->GepState = GEp_Active;
        break;

    // these are invalid
    case ENDPOINT_REMOVE:
    case ENDPOINT_CLOSED:
    case ENDPOINT_TRANSITION:
    default:
        USBPORT_ASSERTMSG("Unexpected Endpoint->CurrentEpState",FALSE);
        break;
    }

    return gepState;
}


HC_GLOBAL_EP_STATE
USBPORT_SetEp_PAUSE(
    __inout PDEVICE_OBJECT HcFdo,
    __in HC_GLOBAL_EP_EVENT Event,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:




Arguments:

Return Value:

    Global Endpoint State.

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE gepState;

    ASSERT_ENDPOINT(Endpoint);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ResetEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);

    gepState = Endpoint->GepState;

    USBPORT_ASSERT(gepState != GEp_Zapped);

    switch(Endpoint->CurrentEpState) {
    case ENDPOINT_ACTIVE:

        if (Endpoint->EpFlags.RootHub) {
            Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
            gepState = Endpoint->GepState = GEp_Paused;
        } else {
            // remove from hw access
            MPf_SetEndpointState(devExt, Endpoint, ENDPOINT_PAUSE);

            Endpoint->StateChangeFrame = MPf_Get32BitFrameNumber(devExt);

            Endpoint->CurrentEpState = ENDPOINT_TRANSITION;
            Endpoint->PendingEpState = ENDPOINT_PAUSE;

            USBPORT_iEndpointInsertStateList(HcFdo, Endpoint);

//            if (Endpoint->EpFlags.HwSyncOnSOF) {
                MPf_InterruptNextSOF(devExt);
//            }

            if (Event == Ev_gEp_ReqPauseCancel) {
                gepState = Endpoint->GepState = GEp_PausePendingCancel;
            } else {
                gepState = Endpoint->GepState = GEp_PausePending;
            }
        }
        break;

    case ENDPOINT_PAUSE:

        Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_PAUSE;
        USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
        gepState = Endpoint->GepState = GEp_Paused;
        break;

    // these are invalid
    case ENDPOINT_IDLE:
    case ENDPOINT_REMOVE:
    case ENDPOINT_CLOSED:
    case ENDPOINT_TRANSITION:
        USBPORT_ASSERTMSG("Unexpected Endpoint->CurrentEpState",FALSE);
        USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_PAUSE);
        break;
    }

    return gepState;
}


HC_GLOBAL_EP_STATE
USBPORT_SetEp_REMOVE(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    remove endpoint from schedule

Arguments:

Return Value:

    Global Endpoint State.

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE gepState;

    ASSERT_ENDPOINT(Endpoint);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ResetEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);

    gepState = Endpoint->GepState;

    USBPORT_ASSERT(gepState != GEp_Zapped);

    switch(Endpoint->CurrentEpState) {
    case ENDPOINT_ACTIVE:
    case ENDPOINT_IDLE:
    case ENDPOINT_PAUSE:
        // remove from schedule
        // remove from hw access
        if (Endpoint->EpFlags.RootHub) {
            Endpoint->CurrentEpState = Endpoint->PendingEpState = ENDPOINT_REMOVE;
            // endpoint is not in hardware so we can go immedaitly to close
            gepState = Endpoint->GepState = GEp_Removed;
            USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);
        } else {

            // miniports must be able to remove the endpoint even if the hardware is suspended
            MPf_SetEndpointState(devExt, Endpoint, ENDPOINT_REMOVE);

            Endpoint->StateChangeFrame = MPf_Get32BitFrameNumber(devExt);

            Endpoint->CurrentEpState = ENDPOINT_TRANSITION;
            Endpoint->PendingEpState = ENDPOINT_REMOVE;

            USBPORT_iEndpointInsertStateList(HcFdo, Endpoint);

//            if (Endpoint->EpFlags.HwSyncOnSOF) {
                MPf_InterruptNextSOF(devExt);
//            }
            gepState = Endpoint->GepState = GEp_RemovePending;
        }
        break;

    // these are invalid
    case ENDPOINT_REMOVE:
    case ENDPOINT_CLOSED:
    case ENDPOINT_TRANSITION:
        USBPORT_SignalEpStateEvent(HcFdo, Endpoint, ENDPOINT_REMOVE);
        USBPORT_ASSERTMSG("Unexpected Endpoint->CurrentEpState",FALSE);
        break;
    }

    return gepState;
}


HC_GLOBAL_EP_STATE
USBPORT_SetEp_CLOSED(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

valid transitions:
    ENDPOINT_REMOVE->ENDPOINT_CLOSED

Arguments:

Return Value:

   Global Endpoint State.

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE gepState;

    ASSERT_ENDPOINT(Endpoint);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    gepState = Endpoint->GepState;

    USBPORT_ASSERT(gepState != GEp_Zapped);

    switch(Endpoint->CurrentEpState) {
    case ENDPOINT_REMOVE:
        if (Endpoint->EpFlags.RootHub) {
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;
        } else {
            // notify miniport
            MPf_CloseEndpoint(devExt, Endpoint);
            Endpoint->PendingEpState = Endpoint->CurrentEpState = ENDPOINT_CLOSED;
        }
        break;

    case ENDPOINT_ACTIVE:
    case ENDPOINT_IDLE:
    case ENDPOINT_PAUSE:
    case ENDPOINT_CLOSED:
    case ENDPOINT_TRANSITION:
        USBPORT_ASSERTMSG("Unexpected Endpoint->CurrentEpState",FALSE);
        break;
    }

    gepState = Endpoint->GepState = GEp_Closed;

    return gepState;
}


VOID
USBPORT_ProcessNeoStateChangeList(
    PDEVICE_OBJECT HcFdo
    )

/*++

Routine Description:

    This routine walks the list of endpoints with pending state chnage requests and determines if hardware sync
    is complete.  If complete the endpoint state is moved and the endpoint is removed from the list.

    This routine can be re-entered since it holds the eplistlock while it processes the list.  This means it can be
    called from other locations such as USBPORT_Core_FlushController.

Arguments:


Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY listEntry;
    ULONG cf, timerIncrement;
    HC_GLOBAL_EP_EVENT sync_event;
    LARGE_INTEGER tc_begin, tc_end;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_AcquireEpListLock(HcFdo, &irql);
    timerIncrement = KeQueryTimeIncrement();
    KeQueryTickCount(&tc_begin);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'chg0', tc_begin.HighPart, tc_begin.LowPart, timerIncrement);

    // fetch the current 32 bit frame number
    cf = MPf_Get32BitFrameNumber(devExt);

    // check the state list for any endpoints that have changed state

    listEntry = devExt->Fdo.EpNeoStateChangeList.Flink;
    LOGENTRY(HcFdo, LOG_TRANSFER, 'nes+', listEntry, &(devExt->Fdo.EpNeoStateChangeList), cf);

    while (listEntry != &(devExt->Fdo.EpNeoStateChangeList)) {

        BOOLEAN epReady;
        PHCD_ENDPOINT endpoint;

        sync_event = Ev_gEp_HwSync;

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                        listEntry,
                        struct _HCD_ENDPOINT,
                        StateLink);

        ASSERT_ENDPOINT(endpoint);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'nes2', listEntry, endpoint, 0);

        LOGENTRY(HcFdo, LOG_MISC, 'chgS', endpoint, cf, endpoint->StateChangeFrame);

        // check if hw synch is complete?

        do {
            epReady = FALSE;

            // zapped endpoints are not on the hardware so the can be immediatly transitioned without
            // a signal from the miniport.
            if (endpoint->GepState == GEp_Zapped) {
                epReady = TRUE;
                break;
            }


            if (endpoint->EpFlags.PollForHwSync) {

                if (MPf_CheckHwSync(devExt, endpoint)) {
                    epReady = TRUE;
                    break;
                }

//                if (cf > endpoint->StateChangeFrame &&
//                   (cf - endpoint->StateChangeFrame) > 1000) {
//#if DBG
//                    DbgPrint(" USB (USBPORT) Watchdog Timeout, !usbep %p\n", endpoint);
//                    NT_ASSERT(FALSE);
//#endif
//                }
                // request interrupt to check later
                // let miniport handle this
                //MPf_InterruptNextSOF(devExt);
            }

            if (endpoint->EpFlags.HwSyncOnSOF) {

                MP_ENDPOINT_STATUS eps;

                // legacy mode syncs with the next SOF, we look for the frame counter to advance.
                if ((endpoint->StateChangeFrame == 0xFFFFFFFF) ||
                    (GREATER_THAN(cf, endpoint->StateChangeFrame))) {
                    LOGENTRY(HcFdo, LOG_TRANSFER, 'nes4', cf, endpoint, endpoint->StateChangeFrame);
                    epReady = TRUE;
                    break;
                }

                eps = MPf_GetEndpointStatus(devExt, endpoint);
                if (eps == ENDPOINT_STATUS_ERROR_HC_HALTED) {
                    LOGENTRY(HcFdo, LOG_TRANSFER, 'nes5', 0, endpoint, eps);
                    // treat endpoint as ready if hc is not running
                    epReady = TRUE;
                    break;
                }
            }

            // allow transition if HC in low power or removed
            if (devExt->Fdo.PowerFlags.Off ||
                devExt->Fdo.PowerFlags.Suspended) {
                LOGENTRY(HcFdo, LOG_TRANSFER, 'nes6', 0, endpoint, 0);

                epReady = TRUE;
                sync_event = Ev_gEp_PM_HwSync;
                break;
            }



        } WHILE (0);

        if (epReady) {
            // advance to next entry
            listEntry = listEntry->Flink;
            LOGENTRY(HcFdo, LOG_TRANSFER, 'nes3', listEntry, endpoint, 0);

            // drop the ref on the endpoint
            USBPORT_iEndpointRemoveStateList(HcFdo, endpoint);

            USBPORT_iSetGlobalEndpointState(HcFdo, endpoint, sync_event);

        } else {
            listEntry = listEntry->Flink;
        }

    }

    LOGENTRY(HcFdo, LOG_TRANSFER, 'nes-', 0, 0, 0);
    KeQueryTickCount(&tc_end);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'chgZ', tc_end.HighPart, tc_end.LowPart, 0);

    USBPORT_ReleaseEpListLock(HcFdo, irql);

}


VOID
USBPORT_WaitEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    )

/*++

Routine Description:


Arguments:


Return Value:

    None.

--*/

{
    PKEVENT kev = NULL;


    ASSERT_ENDPOINT(Endpoint);

    switch(State) {
    case ENDPOINT_PAUSE:
        kev = &Endpoint->GepEventPause;
        break;
    case ENDPOINT_REMOVE:
        kev = &Endpoint->GepEventRemove;
        break;
    default:
        USBPORT_ASSERTMSG("Unexpected Endpoint State",FALSE);
    }
    if (kev) {
        Endpoint->EpCurrentThread = KeGetCurrentThread();

        KeWaitForSingleObject(kev,
                                Executive,
                                KernelMode,
                                FALSE,
                                NULL);
    }

}


VOID
USBPORT_SignalEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    )

/*++

Routine Description:


--*/

{
    PKEVENT kev = NULL;

    ASSERT_ENDPOINT(Endpoint);

    switch(State) {
    case ENDPOINT_PAUSE:
        kev = &Endpoint->GepEventPause;
        break;

    case ENDPOINT_REMOVE:
        kev = &Endpoint->GepEventRemove;
        break;
    default:
        USBPORT_ASSERTMSG("Unexpected Endpoint State",FALSE);
    }

    if (kev) {
        KeSetEvent(kev, IO_NO_INCREMENT, FALSE);
    }
}


VOID
USBPORT_ResetEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    )

/*++

Routine Description:

    Reset the endpoint state change event.


--*/

{
    PKEVENT kev = NULL;

    ASSERT_ENDPOINT(Endpoint);

    switch(State) {
    case ENDPOINT_PAUSE:
        kev = &Endpoint->GepEventPause;
        break;

    case ENDPOINT_REMOVE:
        kev = &Endpoint->GepEventRemove;
        break;

    default:
        USBPORT_ASSERTMSG("Unexpected Endpoint State",FALSE);
    }

    if (kev) {
        KeResetEvent(kev);
    }
}


MP_ENDPOINT_STATE
USBPORT_GetMpEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Return the hardware state for an endpoint. Ep List lock should be held.

Arguments:

Return Value:

    None.

--*/
{
    MP_ENDPOINT_STATE epState;
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt;

    ASSERT_ENDPOINT(Endpoint);

    fdoDeviceObject = Endpoint->FdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(Endpoint->EpFlags.RootHub == 0);
    epState = Endpoint->CurrentEpState;
#if DBG
    USBPORT_CheckMpEnpointState(HcFdo, Endpoint);
#endif

    return epState;
}


NTSTATUS
USBPORT_iEndpointInsertStateList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

   Inserts an Endpoint on the Attention list, EpListLock should be held. If the endpoint is already on the list an
   error is returned.

Arguments:

    AcquireEpListLock - TRUE indicates the lock is to be acquired in this function, FALSE indicates it is currently
    held.

Return Value:

    STATUS_SUCCESS if inserted. Error if we could not ref the endpoint

--*/
{
    NTSTATUS ref_nts;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);


    // this routine should hold global ep list lock
    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);

    if (Endpoint->StateLink.Flink != NULL &&
        Endpoint->StateLink.Blink != NULL) {
        // already on list

        // double insert, this means a state change is already pending.  The caller will have to handle this case and
        // if necessary wait for the current change to occurr and then retry.

        // if we are on list then we should still be holding the ref
        USBPORT_ASSERT(Endpoint->EpStateHandle != NULL);
        ref_nts = STATUS_UNSUCCESSFUL;

    } else {

        // not on list, insert
        USBPORT_ASSERT(Endpoint->EpStateHandle == NULL);

        ref_nts = USBPORT_iReferenceEndpoint(HcFdo, Endpoint,
                    &Endpoint->EpStateHandle, Endpoint, EPREF_STATE_TAG);


        if (NT_SUCCESS(ref_nts)) {
            LOGENTRY(HcFdo, LOG_TRANSFER, 'sta+', Endpoint, &devExt->Fdo.EpNeoStateChangeList, 0);
            InsertTailList(&devExt->Fdo.EpNeoStateChangeList, &Endpoint->StateLink);
            USBPORT_ASSERT(Endpoint->EpStateHandle != NULL);
        }
    }


    return ref_nts;
}


VOID
USBPORT_iEndpointRemoveStateList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

   Removes an Endpoint on the Attention list, EpListLock should be held.

Arguments:

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSBOBJ_REF eph;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);


    // this routine should hold global ep list lock
    USBPORT_ASSERT(devExt->Fdo.EpListLockState == 1);

    USBPORT_ASSERT(Endpoint->StateLink.Flink != NULL);
    USBPORT_ASSERT(Endpoint->StateLink.Blink != NULL);
    USBPORT_ASSERT(Endpoint->EpStateHandle != NULL);

    RemoveEntryList(&Endpoint->StateLink);
    Endpoint->StateLink.Flink = NULL;
    Endpoint->StateLink.Blink = NULL;
    // drop the ref for the attention list
    LOGENTRY(HcFdo, LOG_TRANSFER, 'sta-', Endpoint, Endpoint->EpStateHandle, 0);
    eph = Endpoint->EpStateHandle;
    Endpoint->EpStateHandle = NULL;

    // endpoint may be freed here
    USBPORT_iDereferenceEndpoint(HcFdo, eph);

    return;
}


MP_ENDPOINT_STATE
USBPORT_CheckMpEnpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

   Sanity check the hardware state of the endpoint against the miniport

Arguments:

Return Value:

   current ep state from the miniport.

--*/
{
    PDEVICE_EXTENSION devExt;
    MP_ENDPOINT_STATE mps;
    HC_GLOBAL_EP_STATE gepState;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    gepState = Endpoint->GepState;
    USBPORT_ASSERT(gepState != GEp_Zapped);

    mps = MPf_GetEndpointState(devExt, Endpoint);
    LOGENTRY(HcFdo, LOG_TRANSFER, 'ceps', Endpoint, Endpoint->CurrentEpState, mps);

    if (mps != ENDPOINT_ERROR) {
        USBPORT_ASSERT(Endpoint->CurrentEpState == mps);
    }

    return mps;
}
