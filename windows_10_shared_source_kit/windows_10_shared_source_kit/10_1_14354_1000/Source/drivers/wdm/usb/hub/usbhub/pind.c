/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    pind.c

     3-10-2004

Abstract:

   This file contains code to support flashing LEDs (port indicators) on hubs.

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "pind.tmh"
#endif

#define USBFILE usbfile_pind_c

CCHAR usbfile_pind_c[] = "pind.c";


NTSTATUS
UsbhSetPortIndicator(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USHORT Selector
    )
/*++

Routine Description:

    Suspend a given USB port on a hub

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    NTSTATUS nts;
    USHORT nBytes = 0;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace, "Set Indicator> Hub #%d Port #%d\n", hubExt->HubNumber, PortNumber));

    LOG(HubFdo, LOG_PIND, 'pin0', PortNumber,  0);

    // setup packet for Set_Feature Port Suspend
    // 11.24.2.12
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_SET_FEATURE;
    setupPacket.wValue.W = PORT_INDICATOR;
    setupPacket.wIndex.W = PortNumber | Selector;
    setupPacket.wLength = nBytes;
    // spec specifies 0
    UsbhAssert(HubFdo, nBytes == 0);

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    LOG(HubFdo, LOG_HUB, 'sup1', nts,  usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE,  HubFdo, PortNumber, HubErr_SetPortIndicatorFailed,
                          nts, usbdStatus, NULL, 0);
    }

    LOG(HubFdo, LOG_HUB, 'susP', nts,  PortNumber);

    return nts;
}


HUB_PINDICATOR_STATE
UsbhLockPindicator(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData,
    HUB_PINDICATOR_EVENT Event
    )
/*++

Routine Description:

Serializes port indicator transitions.

Arguments:


Irql: PASSIVE

Return Value:

   returns the current indicator state

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(Sc->HubFdo);

    UsbhAssertPassive();

    KeWaitForSingleObject(&hubExt->PindicatorLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

    UsbhAssert(Sc->HubFdo, Sc->PindSig == UNSIG);

    Sc->PindSig = SIG_PIND_CONTEXT;
    Sc->PindEvent = Event;

    // update state history
    PortData->PindLogIdx = (PortData->PindLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    log = &PortData->PindLog[PortData->PindLogIdx];

    log->Event = Event;
    log->CurrentState = PortData->Pindicator_State;

    return  PortData->Pindicator_State;
}


VOID
UsbhUnlockPindicator(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

Release port indicator lock

Arguments:

    Context - ptr to a bus state context -- this will be the context that currently
        holds the lock.
    NewBusState - new state to transition too before releasing the lock.

Irql: PASSIVE

Return Value:

   none

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    UsbhAssert(Sc->HubFdo, Sc->PindSig == SIG_PIND_CONTEXT);
    Sc->PindSig = UNSIG;

    KeReleaseSemaphore(&hubExt->PindicatorLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

}


VOID
UsbhPindicatorWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
 /*++

Routine Description:

    Worker routine for the flashing LEDs.

Arguments:

Return Value:

   none.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    pd = UsbhGetPortData(HubFdo, (USHORT)PortNumber);
    if (pd) {
        Usbh_PIND_Timeout_Action(HubFdo, pd, Context);
    }
}


ULONG
UsbhPindicatorTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    Timer function we use to toggle the LED state -- flash the LEDs on
    an external hub that supports this feature.

Arguments:

Irql: DISPATCH

Return Value:

    nonzero = retry

--*/

{
    NTSTATUS nts;

    nts = UsbhQueueWorkItem(HubFdo,
                             DelayedWorkQueue,
                             UsbhPindicatorWorker,
                             Context,
                             PortNumber,
                             SIG_PINDICATOR_WORKER);

    if (NT_ERROR(nts)) {
        // returning 1 re-queues the timer
        return 1;
    }

    return 0;
}


VOID
UsbhDispatch_PindicatorEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PINDICATOR_EVENT PindicatorEvent,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

Dispatch routine for blinking port indicator event

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", PindicatorEvent));
    LOG(HubFdo, LOG_PIND, 'flEV', Context, PindicatorEvent);
    UsbhAssertPassive();

    switch (PindicatorEvent) {
    case EvPIND_Timeout:
        Usbh_PIND_Timeout_Action(HubFdo, Pd, Context);
        break;

    case EvPIND_Disable:
        Usbh_PIND_Disable_Action(HubFdo, Pd, Context);
        break;

    case EvPIND_SetAuto:
        Usbh_PIND_SetAuto_Action(HubFdo, Pd, Context);
        break;

    case EvPIND_SetBlink:
        Usbh_PIND_SetBlink_Action(HubFdo, Pd, Context);
        break;

    case EvPIND_Enable:
         Usbh_PIND_Enable_Action(HubFdo, Pd, Context);
        break;

    default:
        UsbhAssertFailure(HubFdo, Context);

    }

}


VOID
UsbhInitPortPindicator(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
/*++

Routine Description:

    Initializes port flasher feature.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    LOG(HubFdo, LOG_PIND, 'inFL', 0, PN(Pd));
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(Pd)));

    UsbhInitializeTimerObject(HubFdo,
                              UsbhPindicatorTimer,
                              &Pd->PindicatorTimer,
                              'Pind');


    Pd->Pindicator_State = Pindicator_Disabled;

    // default to .5 sec duty cycle
    Pd->PindicatorDutyCycle = 500;

}


VOID
UsbhSetPindicatorState(
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData,
    HUB_PINDICATOR_STATE NewState
    )
{

    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(Sc->HubFdo);

    UsbhAssertPassive();

    UsbhAssert(Sc->HubFdo, Sc->PindSig != UNSIG);

    log = &PortData->PindLog[PortData->PindLogIdx];

    log->NextState = PortData->Pindicator_State = NewState;

}


VOID
Usbh_PIND_SetBlink_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    start the port flashing

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PINDICATOR_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPindicator(Sc, PortData, EvPIND_SetBlink);
    LOG(HubFdo, LOG_PIND, 'fls1', state, PN(PortData));

    UsbhAssertPassive();

    switch(state) {
    case Pindicator_BlinkOff:
    case Pindicator_BlinkOn:
        // already blinking
        UsbhSetPindicatorState(Sc, PortData,  state);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Auto:
        // start flashing
        UsbhEnableTimerObject(Sc->HubFdo,
                              &PortData->PindicatorTimer,
                              PortData->PindicatorDutyCycle,
                              PortData->PortNumber,
                              Sc,
                              SIG_PINDICATOR_WORKER,
                              0);


        UsbhSetPindicatorState(Sc, PortData,  Pindicator_BlinkOff);

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Off);

        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Disabled:
        UsbhSetPindicatorState(Sc, PortData,  state);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    default:
        UsbhAssertFailure(HubFdo, PortData);
        UsbhUnlockPindicator(Sc, PortData);
    }

}


VOID
Usbh_PIND_Disable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    disable port flasher

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PINDICATOR_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPindicator(Sc, PortData, EvPIND_SetBlink);
    LOG(HubFdo, LOG_PIND, 'fls2', state, PN(PortData));

    UsbhAssertPassive();

    switch(state) {
    case Pindicator_BlinkOff:
    case Pindicator_BlinkOn:

        // return port to auto mode
        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Auto);

        UsbhDisableTimerObject(Sc->HubFdo, &PortData->PindicatorTimer);
        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Disabled);

        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Auto:

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Auto);

        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Disabled);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Disabled:
        // already disabled
        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Disabled);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    default:
        UsbhAssertFailure(HubFdo, PortData);
        UsbhUnlockPindicator(Sc, PortData);
    }

}


VOID
Usbh_PIND_Enable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    enable port flasher support on errors

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PINDICATOR_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPindicator(Sc, PortData, EvPIND_Enable);
    LOG(HubFdo, LOG_PIND, 'fls3', state, PN(PortData));

    UsbhAssertPassive();

    switch(state) {
    case Pindicator_Disabled:
        // flasher should be in auto mode
        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Auto);

        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Auto);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    default:
        UsbhAssertFailure(HubFdo, PortData);
        UsbhUnlockPindicator(Sc, PortData);
    }

}


VOID
Usbh_PIND_SetAuto_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    disable port flasher

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PINDICATOR_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPindicator(Sc, PortData, EvPIND_SetAuto);
    LOG(HubFdo, LOG_PIND, 'fls4', state, PN(PortData));

    UsbhAssertPassive();

    switch(state) {
    case Pindicator_BlinkOff:
    case Pindicator_BlinkOn:

        // turn off timer
        UsbhDisableTimerObject(Sc->HubFdo, &PortData->PindicatorTimer);
        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Auto);

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Auto);

        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Auto:
        // resync state on hardware
        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Auto);

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Auto);

        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Disabled:
        UsbhSetPindicatorState(Sc, PortData,  Pindicator_Disabled);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    default:
        UsbhAssertFailure(HubFdo, PortData);
        UsbhUnlockPindicator(Sc, PortData);
    }

}


VOID
Usbh_PIND_Timeout_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    disable port flasher

Arguments:

    Sc- state context for the port.

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PINDICATOR_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    state = UsbhLockPindicator(Sc, PortData, EvPIND_Timeout);
    LOG(HubFdo, LOG_PIND, 'fls5', state, PN(PortData));

    switch(state) {
    case Pindicator_BlinkOff:

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortData->PindicatorLED);

        // start flashing
        UsbhEnableTimerObject(Sc->HubFdo,
                             &PortData->PindicatorTimer,
                             PortData->PindicatorDutyCycle,
                             PortData->PortNumber,
                             Sc,
                             SIG_PINDICATOR_WORKER,
                             0);

        UsbhSetPindicatorState(Sc, PortData,  Pindicator_BlinkOn);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_BlinkOn:

        UsbhSetPortIndicator(HubFdo, PortData->PortNumber, PortLED_Off);

        // start flashing
        UsbhEnableTimerObject(Sc->HubFdo,
                              &PortData->PindicatorTimer,
                              PortData->PindicatorDutyCycle,
                              PortData->PortNumber,
                              Sc,
                              SIG_PINDICATOR_WORKER,
                              0);

        UsbhSetPindicatorState(Sc, PortData,  Pindicator_BlinkOff);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    case Pindicator_Auto:
    case Pindicator_Disabled:
        // workitem may run after we disable
        UsbhSetPindicatorState(Sc, PortData,  state);
        UsbhUnlockPindicator(Sc, PortData);
        break;

    default:
        UsbhAssertFailure(HubFdo, PortData);
        UsbhUnlockPindicator(Sc, PortData);
    }

}



VOID
UsbhEvPIND_SetAuto(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    set port flasher to 'automatic' mode

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    PHUB_PORT_DATA pd;

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PindicatorEvent(HubFdo, pd, EvPIND_SetAuto, Sc);
    }

}


VOID
UsbhEvPIND_SetBlink(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT Sc,
    USHORT LED_Color,
    ULONG DutyCycle
    )
/*++

Routine Description:

    set port flasher to 'automatic' mode

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    PHUB_PORT_DATA pd;

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        pd->PindicatorDutyCycle = DutyCycle;
        pd->PindicatorLED = LED_Color;
        UsbhDispatch_PindicatorEvent(HubFdo, pd, EvPIND_SetBlink, Sc);
    }

}


VOID
UsbhEnablePortIndicators(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    enable the port indicators if supported for all ports

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT p;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    UsbhAssertPassive();

    if (!hubExt->PortIndicators) {
        return;
    }

    for (p=1; p <= N_PORTS(HubFdo); p++) {

        pd = UsbhGetPortData(HubFdo, p);
        if (pd) {
            UsbhDispatch_PindicatorEvent(HubFdo, pd, EvPIND_Enable, PNP_CONTEXT(HubFdo));
        }

    }

}


VOID
UsbhDisablePortIndicators(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    disable the port indicators if supported for all ports. This is used by power and pnp
    code to stop pindicators from flashing.

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT p;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    UsbhAssertPassive();

    for (p=1; p <= N_PORTS(HubFdo); p++) {

        pd = UsbhGetPortData(HubFdo, p);
        if (pd) {
            UsbhDispatch_PindicatorEvent(HubFdo, pd, EvPIND_Disable, PNP_CONTEXT(HubFdo));
        }

    }

}


