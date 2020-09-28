/*
Module: FDOPWR.C


name:USB Wdm Power management

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002

Abstract:


    Provides power management for usbhub.sys FDO

Low Power (Sx) States:
    S1-S5 the hub will synchronously handle the message
    completing the S state after the D state request is complete

PowerSystemWorking (S0):
    The hub driver will complete the S irp immediately and defer the
    actual resume processing. The processing will be held if one of the
    hubs PDOs requests D0 for backward compatibility.


Hub Device States:

    Power states for the hub are well defined and map to USB Vbus states.

    D0 - on, can receive SOFs;              Vbus_On
    D1 - not used (treated same as D2)
    D2 - USB suspend, wake-up possible      Vbus_On, Vbus_Interrupted
    D3 - OFF device loses power             Vbus_Off

USB Vbus Power States:

    Possible power states for USB when entering system sleep states. Any given S or D
    state will map to one of these three.

    - Vbus_On
    - Vbus_Off
    - Vbus_Interrupted


USB Remote Wake-up:

The following section describes USB suspend/resume relative to the WDM power model.

Hub Functions:

    UsbdHubArmBusForWake        BUS_ARM         ; bus enabled to detect resume
    UsbdHubDisarmBusForWake     BUS_DISARM

**notation
These functions are called independently by each hub instance. Each call arms/disarms this
specific hub on a global list.  If any hub on this list is armed the bus is also armed.

    UsbhArmHubForWakeDetect     HUB_ARM   ; hub enabled to detect resume
                                          ; and propagate resume
                                          ;>HubWakeState == HUB_WAKESTATE_ARMED
    UsbhDisarmHubForWakeDetect  HUB_DISARM


    UsbhEnableWakeOnConnect     HWC_ARM
    UsbhDisableWakeOnConnect    HWC_DISARM
**notation
These functions transmit the USB wake enable/disable command to the hub.

USB_Device - refers to any usb device EXCEPT a USB hub in this context.
WW_Irp - WDM power Irp, IRP_MN_WAIT_WAKE
suspend_state - hardware state of the port == USB_Suspend

WDM expects a tree structure, unfortunately USB power management (as defined in the USB specification)
is not really a tree structure so we must abstract this for the operating system and function drivers.

.....................................................................................................
USB_Device:

DEVICE_ARM/DEVICE_DISARM - 'Arming' a USB_Device or USB_Hub for wake means enabling it to
GENERATE Usb resume signalling.

1. A USB_Device driver may 'arm' itself for wake by posting a WW_Irp to its PDO. This irp
will complete with STATUS_SUCCESS if resume signaling is detected on the devices upstream port.
(Note that the upstream port must be in USB suspend_state for resume signalling to be detected.)

2. A USB_Device driver may attempt to suspend its upstream port by requesting a 'D2' power irp. System
software will do its best to accommodate this request within the limits of the hardware servicing
the port. If the USB_Device driver has submitted a WW_Irp ('arming' the device) the device will
be sent the necessary commands to enable it to generate resume signalling just before the port is
placed in USB_Suspend state.

*Currently a USB_Device driver can not detect when or if its upstream port has been truly suspended
creating a window where the device hardware may not be able to notify its driver.  This makes
moving in and out of D0 an unreliable means of power managing device power in some cases.  We attempt
to fix some of this in neohub by failing the D2 request if the port can not be suspended.
**jd - we may need an addition API to 'poll' the port suspend state status for the function drivers.


.......................................................................................................
USB_Hub:

HUB_ARM/HUB_DISARM - By definition all USB hubs are always 'armed' for wake in the WDM sense (meaning WW irp
is posted unless the bus is in Vbus_Off (no power).
The WW_Irp posted by the hub driver is used only to track the path or wake-up for WDM. HUB_ARM
indicates a WW_Irp is pending for the hub.

BUS_ARM/BUS_DISARM - Indicates the bus ability to detect resume signalling.  For System States S1->S3
this requires programming the controller and posting a WW_Irp to PCI for the host controller.  BUS_ARM
means the bus is enabled to detect resume. The bus is ARMed if any usb device or hub is enabled to wake
the system.

BUS_ARM state:

Y=BUS_ARM
N=BUS_DISARM

                DEVICE_ARM      DEVICE_DISARM

HWC_ARM            Y                Y

HWC_DISARM         Y                N

This result of this table is effectively OR'ed for each instance of the hub on the bus.


HWC_ARM/HWC_DISARM - Indicates the hubs ability to GENERATE resume signalling on certain events (like
attaching a device).  The USB specification requires that this feature be independent of propagating
and/or detecting USB resume.  A hub is HWC_ARM if it will GENERATE resume on its own, in order to
generate resume a standard wake_enable command must be transmitted to the hub.  This setting corresponds
to the UI checkbox in the property page for the hub:

    'allow this device to bring the system out of standby'


.........................................................................................................




<pre>

HUB FDO Power State Machine
-----------------------


HubPowerState

HubVbusState    - the actual state of the bus

These identify the value for Sx or Dx, the meaning of these is based on
the value of HubPowerState.
e.g. if Dx= D3 and Sx= S1 then FdoWaitSx_WaitDx would mean WaitS1_WaitD3
or waiting to transition to S1 and D3.

FdoDx           - the current D state mapped to Dx
FdoSx           - the current S state mapped to Sx


States:

FdoS0_D0 - hub FD0 in D0 and S0 waiting for system suspend

FdoWaitSxIoComplete_D0 - waiting on system sx (suspend) irp completion

FdoWaitSx_WaitDx - sxIocomplete called, waiting to go to Dx

FdoWaitSx_WaitDxPoComplete - waiting for PoCallback for Dx irp.

FdoSx_Dx - in low system power state.

FdoSx_D0 - received D0 request before the system power Sx irp.

FdoWaitS0IoComplete_D0 - waiting for systemworking when hub already in D0

FdoWaitS0IoComplete_Dx - waiting for systemworking, hub not in D0 yet.

Fdo_WaitD0 - S0 irp completed, waiting to go to D0.

Fdo_WaitD0Worker - waiting for async D0 worker to run, OS thinks hub is in D0 but we haven't
                    fully repowered yet.

Fdo_WaitD0PoComplete - waiting for D0 irp to finish.

Fdo_PowerFailure - error occurred during a power event.

Fdo_PowerFailureDisconnect - hub disconnected during power


...........................................................................................
FDO power entry points

    UsbhFdoPower
        UsbhFdoPower_WaitWake
        UsbhFdoPower_PowerSequence
        UsbhFdoPower_QueryPower
        UsbhFdoPower_SetPower
            UsbhFdoSystemPowerState(HubFdo, Irp);
            UsbhFdoDevicePowerState(HubFdo, Irp);


-------------------------------------------------------------------------------------------
...........................................................................................
>> Call Stack:
>> Power Down Sequence, Set Power Sx

                                                                S-state(Fdo)    D-state(Fdo)
UsbhFdoPower_SetPower(SxIrp)                                    S0              D0
    UsbhFdoSystemPowerState(SxIrp);                             S0              D0
        UsbhFdoSetPowerSx_Action(SxIrp) [STATUS_PENDING]        S0              D0
            UsbhFdoSxIoComplete_Action(SxIrp)                   Sx              D0
            PoRequestPowerIrp(DxIrp)                            Sx              D0
                UsbhFdoSetPowerDx_Action(DxIrp)                 Sx              D0
                    UsbhFdoDxPoComplete_Action(DxIrp)           Sx              Dx
                    IoCompleteRequest(SxIrp)                    Sx              Dx



------------------------------------------------------------------------------------------
..........................................................................................
>> Call Stack
>> Power up sequence, Set Power S0

                                                                S-state(Fdo)    D-state(Fdo)
UsbhFdoPower_SetPower(S0Irp)                                    Sx              Dx
    UsbhFdoSystemPowerState(S0Irp);                             Sx              Dx
        UsbhFdoSetPowerS0_Action(S0Irp) [STATUS_PENDING]        Sx              Dx
            UsbhFdoS0IoComplete_Action(S0Irp)                   S0              Dx
            PoRequestPowerIrp(D0Irp)                            S0              Dx
                UsbhFdoSetPowerD0_Action(D0Irp)                 S0              Dx
                    UsbhFdoD0PoComplete_Action(D0Irp)           S0              D0
                    IoCompleteRequest(S0Irp)                    S0              D0
                        UsbhFdoScheduleDeferredPowerRequest     S0              D0
                            UsbhFdoD0Worker_Action              S0              D0


------------------------------------------------------------------------------------------
..........................................................................................


</pre>


Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      NULL
    Unload -->linked to Unload           NULL
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        NULL
    Remove -->linked to PNP REMOVE       NULL

</pre>

*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "fdopwr.tmh"
#endif

CCHAR usbfile_fdopwr_c[] = "fdopwr.c";

#define USBFILE usbfile_fdopwr_c

#define fdopwr_c_tag      __LINE__

//
// FDO Power Handlers
//
//  IRP_MN_WAIT_WAKE = 0
//  IRP_MN_POWER_SEQUENCE = 1
//  IRP_MN_SET_POWER = 2
//  IRP_MN_QUERY_POWER = 3
//
//
//  ULONG SystemContext;
//  POWER_STATE_TYPE POINTER_ALIGNMENT Type; SystemPower = 0, DevicePower = 1
//  POWER_STATE POINTER_ALIGNMENT State;
//  POWER_ACTION POINTER_ALIGNMENT ShutdownType;
///
//
//

const HUB_POWER_DISPATCH HubFdoPowerDispatch[HUB_POWER_MAX_MN] = {
    IRP_MN_WAIT_WAKE,                   UsbhFdoPower_WaitWake,
    IRP_MN_POWER_SEQUENCE,              UsbhFdoPower_PowerSequence,
    IRP_MN_SET_POWER,                   UsbhFdoPower_SetPower,
    IRP_MN_QUERY_POWER,                 UsbhFdoPower_QueryPower
};


VOID
UsbhStartThisPowerIrp_Fdo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG Tag
    )
/*++


Routine Description:

    References the power irp held by the FDO so we can track it.

Arguments:

    Irp - Power Irp to attach to the hubs device extension

Return Value:

    none -- always succeeds

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    ULONG idx;

    hubExt = FdoExt(HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    //idx=0 is system irp, idx=1 is device irp
    idx = ioStack->Parameters.Power.Type;
    UsbhAssert(HubFdo, idx<2);

    if (idx>1) {
        return;
    }

    // power irps are serialized, the serialization object is the power
    // irp itself. We are responsible for releasing this lock by calling
    // PoStartNextPowerIrp.  This routine sets a currently held power
    // irp and performs some debug checking.

    LOG(HubFdo, LOG_POWER, 'PON3', ioStack, Irp);

    UsbhAssert(HubFdo, hubExt->CurrentPowerIrp[idx] == NULL);
    // we are supposed to call this on the current stack location
    UsbhAssert(HubFdo, hubExt->CurrentPowerIrpStack[idx] == NULL);

    // ref
    hubExt->CurrentPowerIrp[idx] = Irp;
    hubExt->CurrentPowerIrpStack[idx] = ioStack;
    hubExt->CurrentPowerIrpTag[idx] = Tag;
}


VOID
UsbhPoStartNextPowerIrp_Fdo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG Tag
    )
/*++


Routine Description:

    Removes a reference to the power irp held by the FDO and calls
    PoStartNextPowerIrp.

Arguments:

    Irp - Power Irp to attach to the hubs ref list

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    ULONG idx;

    hubExt = FdoExt(HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    //idx=0 is system irp, idx=1 is device irp
    idx = ioStack->Parameters.Power.Type;
    UsbhAssert(HubFdo, idx<2);

    if (idx>1) {
        return;
    }

    // power irps are serialized, the serialization object is the power
    // irp itself. We are responsible for releasing this lock by calling
    // PoStartNextPowerIrp.  This routine releases a currently held power
    // irp and performs some debug checking.

    LOG(HubFdo, LOG_POWER, 'PON1', hubExt->CurrentPowerIrp[idx], Irp);
    LOG(HubFdo, LOG_POWER, 'PON2', hubExt->CurrentPowerIrpStack[idx], ioStack);

    UsbhAssert(HubFdo, hubExt->CurrentPowerIrp[idx] == Irp);
    // we are supposed to call this on the current stack location
    UsbhAssert(HubFdo, hubExt->CurrentPowerIrpStack[idx] == ioStack);

    // deref
    hubExt->CurrentPowerIrp[idx] = NULL;
    hubExt->CurrentPowerIrpStack[idx] = NULL;
    hubExt->CurrentPowerIrpTag[idx] = Tag;

    // release the lock
    PoStartNextPowerIrp(Irp);

}



VOID
UsbhSetFdoPowerState(
    PDEVICE_OBJECT HubFdo,
    ULONG Data,
    HUB_FDO_POWERSTATE NewFdoPowerState,
    HUB_FDO_EVENT FdoPowerEvent
    )
/*++


Routine Description:

    Sets the current power state for the hub Fdo

Arguments:

    Data - optional ULONG data element to include in the log

    NewFdoPowerState - state we are transitioning in to.

    FdoPowerEvent - event causing transition

Return Value:

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PG_STATE_LOG_ENTRY fdoLog;

    hubExt = FdoExt(HubFdo);

    hubExt->PwrLogIdx=(hubExt->PwrLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    fdoLog = &hubExt->PwrLog[hubExt->PwrLogIdx];

    fdoLog->Event = FdoPowerEvent;
    fdoLog->CurrentState = hubExt->CurrentFdoPwrState;
    fdoLog->NextState = NewFdoPowerState;
    fdoLog->Tag = Data;

    hubExt->CurrentFdoPwrState = NewFdoPowerState;

}



NTSTATUS
UsbhFdoPowerWorkerInternal(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    Dispatch worker routine for Power Irps to Fdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG idx;
    UCHAR pow_func, pow_subfunc;

    hubExt = FdoExt(HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pow_func = ioStack->MinorFunction;
    pow_subfunc = 0xff;

    switch(pow_func) {
    /*
       //ioStack->Parameters.Power
       // see wdm.h
       ULONG SystemContext;
       // 0=system 1= device
       POWER_STATE_TYPE Type;
       // depends on type
       // 0 = undefined, 1= D1, etc...
       POWER_STATE State;
       POWER_ACTION ShutdownType;
       // ----------------
       // bit 8; 1= device 0 = system
       //
    */
    case IRP_MN_SET_POWER:
    case IRP_MN_QUERY_POWER:

       if (ioStack->Parameters.Power.Type == DevicePowerState) {
           pow_subfunc = (UCHAR) ioStack->Parameters.Power.State.DeviceState;
           pow_subfunc |= 0x80;
       } else {
           pow_subfunc = (UCHAR) ioStack->Parameters.Power.State.SystemState;
       }
       break;
    case IRP_MN_WAIT_WAKE:
       pow_subfunc = ioStack->Parameters.WaitWake.PowerState;
       break;
    }

    idx = InterlockedIncrement((PLONG)&hubExt->PowerFuncHistoryIdx);
    idx = idx % HUB_PP_HISTORY_LEN;
    hubExt->PowerFuncHistory[idx] = pow_func;
    hubExt->PowerSubFuncHistory[idx] = pow_subfunc;
    hubExt->CurrentPowerFunc = (ULONG) pow_func;

    LOG(HubFdo, LOG_POWER, 'fdoP', 0, Irp);

    if (ioStack->MinorFunction < HUB_POWER_MAX_MN) {
        nts = HubFdoPowerDispatch[ioStack->MinorFunction].PowerHandler(
                HubFdo,
                Irp);
    } else {
        // this is where we handle the famous IRP_MN_BOGUS irp and anything
        // else that is weird

        // There is no indication in the DDK that PoStartNextPowerIrp is needed
        // here.
        //UsbhStartThisPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

        nts = UsbhFdoPower_DoDefault(HubFdo, Irp, FALSE);
    }

    return nts;
}


VOID
UsbhFdoPowerWorker(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch worker routine for Power Irps to Fdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    UsbhFdoPowerWorkerInternal(HubFdo, Irp);
}


NTSTATUS
UsbhFdoPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch routine for Power Irps to Fdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;    
    
#ifdef USB_FORCE_POWER_AT_DISPATCH
    KIRQL irql;
    BOOLEAN irqlRaised = FALSE;
    if (!(HubFdo->Flags & DO_POWER_PAGABLE)) {
        KeRaiseIrql(DISPATCH_LEVEL, &irql);
        irqlRaised = TRUE;
    }
#endif 

    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
        nts = UsbhFdoPowerWorkerInternal(HubFdo, Irp);
    } else {

        nts = Usbh_HubDeferIrpProcessing(HubFdo, 
                                         HubFdo,
                                         UsbhFdoPowerWorker, 
                                         Irp);

        if (!NT_SUCCESS(nts)) {
            UsbhCompleteIrp(Irp, nts);            
        } else {        
            nts = STATUS_PENDING;
        }
    }

#ifdef USB_FORCE_POWER_AT_DISPATCH
    if (irqlRaised) {
        KeLowerIrql(irql);
    }
#endif

    return nts;    
}



VOID
UsbdHubArmBusForWake(
    PDEVICE_OBJECT HubFdo
    )
/*++
Routine Description:

    Arms the actual bus for wake (resume signalling) through a bus interface
    function.

    Arming the USB bus for wake requires hardware set up on the host controller.



Arguments:

Irql: PASSIVE

Return Value:

    not failable
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    if (!bus->SetBusSystemWakeMode) {
        UsbhAssertMsg(HubFdo, "SetBusSystemWakeMode function should not be NULL", FALSE);
    } else {
        UsbhKdPrint((1, "Hub #%d FDO (%p) *arm bus**\n", hubExt->HubNumber, HubFdo));

        DbgTrace((UsbhDebugTrace,"ARM bus for wake\n"));
        bus->SetBusSystemWakeMode(bus->BusContext, 1);
        hubExt->FdoFlags.BUS_ARM = 1;
    }
}



VOID
UsbdHubDisarmBusForWake(
    PDEVICE_OBJECT HubFdo
    )
/*++
Routine Description:

    Disarms the actual bus for wake (resume signalling) through a bus interface
    function.

    Arming the USB bus for wake requires hardware set up on the host controller.
    The net result will be a WAIT_WAKE irp posted to PCI for the host
    controller.

Arguments:

Irql: PASSIVE

Return Value:

    not failable
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    if (!bus->SetBusSystemWakeMode) {
        UsbhAssertMsg(HubFdo, "SetBusSystemWakeMode function should not be NULL", FALSE);
    } else {
        UsbhKdPrint((1, "Hub #%d FDO (%p) **disarm bus**\n", hubExt->HubNumber, HubFdo));

        DbgTrace((UsbhDebugTrace, "DISARM bus for wake\n"));
        bus->SetBusSystemWakeMode(bus->BusContext, 0);
        hubExt->FdoFlags.BUS_ARM = 0;
    }
}


BOOLEAN
UsbhCanSuspend(
    PDEVICE_OBJECT HubFdo,
    SYSTEM_POWER_STATE SystemState
    )
/*++

Routine Description:

    Returns true if the hub can suspend in a given system state.

Arguments:

Return Value:

    TRUE if we can go to D2 (USB Suspend)

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, SystemState >= 0 && SystemState < PowerSystemMaximum);

    // check our devcaps if the system state maps to D2 return TRUE
    if (hubExt->DeviceCapabilities.DeviceState[SystemState] == PowerDeviceD2) {
        return TRUE;
    }

    return FALSE;
}


VOID
UsbhArmHubWakeOnConnect(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

Arguments:

Irql:  PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    // hub is allowed to wake on system connect/disconnect, this posts the WW irp
    // so we can detect the source of resume.

    if (hubExt->FdoFlags.HWC_ARM == 0) {
        UsbhArmHubForWakeDetect(HubFdo);

        // we need to transmit the remote_wake en/disable here
        nts = UsbhSetHubRemoteWake(HubFdo, TRUE);
        if (NT_SUCCESS(nts)) {
            hubExt->FdoFlags.HWC_ARM = 1;
        }
        UsbhKdPrint((1, "Hub #%d FDO (%p) **arm WOC**\n", hubExt->HubNumber, HubFdo));
    }

}


VOID
UsbhDisarmHubWakeOnConnect(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

Arguments:

Irql:  PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    UsbhDisarmHubForWakeDetect(HubFdo);
    if (hubExt->FdoFlags.HWC_ARM == 1) {

        // we need to transmit the remote_wake en/disable here.
        nts = UsbhSetHubRemoteWake(HubFdo, FALSE);

        if (NT_SUCCESS(nts)) {
            hubExt->FdoFlags.HWC_ARM = 0;
        }
        UsbhKdPrint((1, "Hub #%d FDO (%p) **disarm WOC**\n", hubExt->HubNumber, HubFdo));
    }

}


VOID
UsbhArmHubForWakeDetect(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Arms a USB hub so that it can generate resume signaling.  Two cases for
    this.  We do this by posting a IRP_MN_WAIT_WAKE irp to our PDO.


    We use Adrian Oneys synchronous mechanism to arm and disarm.

    When we return from this function the hub should be 'armed'.

Arguments:

Irql:  PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    POWER_STATE powerState;
    NTSTATUS nts;
    HUB_WAKE_STATE oldWakeState;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    LOG(HubFdo, LOG_POWER, 'hWW+', 0, 0);

    UsbhKdPrint((1, "Hub #%d FDO (%p) **arm WW** (S%d)\n", hubExt->HubNumber, HubFdo,
        hubExt->DeviceCapabilities.SystemWake-1));

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_WAIT_WAKE_START, 0, STATUS_SUCCESS);

    WHILE_TRUE {
        oldWakeState = InterlockedCompareExchange((PLONG)&hubExt->HubWakeState,
                                                  HUB_WAKESTATE_WAITING,
                                                  HUB_WAKESTATE_DISARMED);

        LOG(HubFdo, LOG_POWER, 'hWWx', 0, oldWakeState);

        if (oldWakeState == HUB_WAKESTATE_DISARMED) {
            break;
        }

        if ((oldWakeState == HUB_WAKESTATE_ARMED) ||
            (oldWakeState == HUB_WAKESTATE_WAITING)) {
            // The device is already arming
            return;
        }

        // wait for previous wake irp to finish
        UsbhDisarmHubForWakeDetect(HubFdo);
    }

    // wait for previous wake irp to finish
    WAIT_EVENT(&hubExt->PendingWakeIrpEvent);

    // begin arming sequence

    LOG(HubFdo, LOG_POWER, 'hWWp', 0, 0);

    RESET_EVENT(&hubExt->PendingWakeIrpEvent);

    // According to the NTDDK this should be systemwake
    powerState.DeviceState = hubExt->DeviceCapabilities.SystemWake;

    // send the wake irp to our PDO, since it is not our
    // responsibility to free the irp we don't keep track
    // of it
    //
    // NOTE: This request will result in a WAIT_WAKE irp being sent to the HUB
    // FDO.
    nts = PoRequestPowerIrp(HubFdo,
                             IRP_MN_WAIT_WAKE,
                             powerState,
                             UsbhFdoWakePoComplete_Action,
                             NULL,
                             NULL);

    if (nts != STATUS_PENDING) {

        LOG(HubFdo, LOG_POWER, 'WAKp', nts, 0);

        // arming has failed
        hubExt->HubWakeState = HUB_WAKESTATE_DISARMED;
        SET_EVENT(&hubExt->PendingWakeIrpEvent);

        return;
    }

    LOG(HubFdo, LOG_POWER, 'hARM', 0, 0);
    return;
}


VOID
UsbhDisarmHubForWakeDetect(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    This function synchronously disarms the hub for wake.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_WAKE_STATE oldWakeState;

    LOG(HubFdo, LOG_POWER, 'hWW-', 0, 0);

    hubExt = FdoExt(HubFdo);

    // Check if the hub wake state was ever initialized
    if (hubExt->HubWakeState == HUB_WAKESTATE_UNINITIALIZED) {
        return;
    }

    UsbhKdPrint((1, "Hub #%d FDO (%p) **disarm WW**\n", hubExt->HubNumber, HubFdo));

    // Go from WAKESTATE_WAITING to WAKESTATE_WAITING_CANCELLED, or
    //         WAKESTATE_ARMED to WAKESTATE_ARMING_CANCELLED, or
    // stay in WAKESTATE_DISARMED or WAKESTATE_COMPLETING
    oldWakeState = _InterlockedOr( (PLONG)&hubExt->HubWakeState, 1 );
    //oldWakeState = RtlInterlockedSetBits((PULONG)&devExt->Fdo.HcWakeState, 1 );

    if (oldWakeState == HUB_WAKESTATE_ARMED) {

       IoCancelIrp(hubExt->PendingWakeIrp);

       //
       // Now that we’ve cancelled the IRP, try to give back ownership
       // to the completion routine by restoring the WAKESTATE_ARMED state
       //
       oldWakeState = InterlockedCompareExchange(
            (PLONG) &hubExt->HubWakeState,
            HUB_WAKESTATE_ARMED,
            HUB_WAKESTATE_ARMING_CANCELLED);

       if (oldWakeState == HUB_WAKESTATE_COMPLETING) {
           //
           // We didn’t give back control of IRP in time, so we own it now.
           //
           // this will cause tp PoCompletion routine to run
           IoCompleteRequest(hubExt->PendingWakeIrp, IO_NO_INCREMENT);
       }
    }

}


VOID
UsbhFdoPower_PowerFailureEntry(
    PDEVICE_OBJECT HubFdo,
    HUB_FDO_EVENT FdoPowerEvent,
    NTSTATUS Nts,
    PHUB_POWER_CONTEXT PowerCtx
    )
/*++

Routine Description:

    Called on power failure, can sometimes be caused by a removal
    during power management but, we log these anyway just in case there
    is some pathology associated with the removal.

    This routine also performs the necessary cleanup

Arguments:

Irql:

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);


    HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_FdoPowerFailure,
                         Nts, 0, &FdoPowerEvent, sizeof(FdoPowerEvent));

    hubExt->PowerFailureStatus = Nts;

    if (Usb_Disconnected(Nts)) {
        UsbhSetFdoPowerState(HubFdo, Nts, Fdo_PowerFailureDisconnected, FdoPowerEvent);
    } else {
        UsbhSetFdoPowerState(HubFdo, Nts, Fdo_PowerFailure, FdoPowerEvent);
    }

    LOG(HubFdo, LOG_POWER, 'sPW!', Nts, 0);
    
    //
    // Unblock any PDOs that might be waiting for the parent to go to D0
    //
    UsbhFdoUnblockAllPendedPdoD0Irps(HubFdo);

    // release locks and context
    UsbhReleaseFdoPwrLock(HubFdo, PowerCtx->Sc);
    // poke Pnp
    UsbhBusIoInvalidateDeviceRelations(HubFdo, PowerCtx->Sc);
    UsbhReleasePowerContext(HubFdo, PowerCtx);

    // cancel any pending ww irps
    UsbhDisarmHubForWakeDetect(HubFdo);


}


NTSTATUS
UsbhFdoPower_DoDefault(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    BOOLEAN CallPoStartNextPowerIrp
    )
/*++

Routine Description:

    Default handler for Power irp mn functions to FDO, passes irp on
    status unchanged after calling PoStartNextPowerIrp

Arguments:

Irql:

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (CallPoStartNextPowerIrp) {
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    }

    IoSkipCurrentIrpStackLocation(Irp);

    return PoCallDriver(hubExt->TopOfStackDeviceObject,
                        Irp);
}


NTSTATUS
UsbhFdoPower_PowerSequence(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handles power sequence irp

Arguments:

Irql:

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    // The DDK explicitly indicates that PoStartNextPowerIrp should
    // not be called here

    IoCopyCurrentIrpStackLocationToNext(Irp);(Irp);

    return PoCallDriver(hubExt->TopOfStackDeviceObject,
                        Irp);
}


NTSTATUS
UsbhFdoPower_QueryPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++
Routine Description:

    QUERY_POWER
    Handler for IRP_MN_QUERY_POWER

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/

{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    UsbhAssertPassive();

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_POWER);
    hubExt = FdoExt(HubFdo);

    UsbhStartThisPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

    switch (ioStack->Parameters.Power.Type) {
    case SystemPowerState:
        return UsbhFdoQuerySystemPowerState(HubFdo, Irp);
    case DevicePowerState:
        return UsbhFdoQueryDevicePowerState(HubFdo, Irp);
    }

    // unknown power type requested, just pass on

    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);
    nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                       Irp);

    return nts;
}


NTSTATUS
UsbhFdoQueryDevicePowerDxPoCompletion(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    )
/*++
Routine Description:

    QUERY_POWER
    Completion Handler for IRP_MN_QUERY_POWER D-state irp

    Completes the associated system state irp with the same status as the D
    state irp.

Arguments:

Irql: DPC

Return Value:

    nt status
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PIRP systemPowerIrp = Context;

    hubExt = FdoExt(HubFdo);

    nts = systemPowerIrp->IoStatus.Status = IoStatus->Status;

    LOG(HubFdo, LOG_POWER, 'qdxC', nts, systemPowerIrp);

    // complete the associated system power irp
    UsbhPoStartNextPowerIrp_Fdo(HubFdo, systemPowerIrp, fdopwr_c_tag);
    IoCompleteRequest(systemPowerIrp, IO_NO_INCREMENT);

    IoReleaseRemoveLock(&hubExt->RemoveLock, systemPowerIrp);

    return nts;
}


NTSTATUS
UsbhFdoQuerySystemPowerSxIoCompletion(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    )
/*++

  QUERY_POWER
  Handles SystemPowerState 'QUERY' completion, this is where we request
  the QUERY D state IRP.

*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    POWER_STATE powerState;
    SYSTEM_POWER_STATE systemState;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    hubExt = FdoExt(HubFdo);

    systemState = ioStack->Parameters.Power.State.SystemState;

    LOG(HubFdo, LOG_POWER, 'qSxs', systemState, Irp);

    if (UsbhCanSuspend(HubFdo, systemState)) {
        powerState.DeviceState = PowerDeviceD2;
    } else {
        powerState.DeviceState = PowerDeviceD3;
    }

    nts = PoRequestPowerIrp(HubFdo,
                            IRP_MN_QUERY_POWER,
                            powerState,
                            UsbhFdoQueryDevicePowerDxPoCompletion,
                            Irp,
                            NULL);

    if (nts != STATUS_PENDING) {

        // query power completed synchronously

        Irp->IoStatus.Status = nts;

        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        return nts;
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
UsbhFdoQuerySystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++
Routine Description:

    QUERY_POWER
    Handle system power state irp for FDO.

    The appropriate D state irp is requested for this S irp from the S irp
    completion routine. The S irp itself is not completed until the D irp
    completes.

    flow:
    Q-s-state -->Q-s-state-complete-->Q-D-state-->Q-D-state-complete
        -->s-state-complete

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    hubExt = FdoExt(HubFdo);

    // take the remove lock until we are done with this 'S-IRP'

    nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

    if (NT_ERROR(nts)) {
        Irp->IoStatus.Status = nts;
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return nts;
    }

    IoMarkIrpPending(Irp);
    IoCopyCurrentIrpStackLocationToNext(Irp);

    UsbhSetCompletionRoutine(HubFdo,
                             Irp,
                             UsbhFdoQuerySystemPowerSxIoCompletion,
                             HubFdo,
                             TRUE,
                             TRUE,
                             TRUE);

    PoCallDriver(hubExt->TopOfStackDeviceObject,
                 Irp);

    return STATUS_PENDING;
}


NTSTATUS
UsbhFdoQueryDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++
Routine Description:

   Handles IRP_MN_QUERY_POWER DevicePowerState for a hub.

   Note: By definition the driver must be able to handle any random set_power
   request following the QUERY so there is no point performing any action here
   other than to pass the request on to our PDO.

   We take action when the actual SET_POWER is sent.

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_POWER, 'Qdps', 0, Irp);

    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_POWER);
    UsbhAssert(HubFdo, ioStack->Parameters.Power.Type == DevicePowerState);

    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);
    nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                       Irp);

    return nts;
}


NTSTATUS
UsbhFdoWaitWake_IoCompletion(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    )
/*++
Routine Description:

    IoCompletion routine for WAIT_WAKE IRPS sent to the FDO

Arguments:

Irql: DPC

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_WAKE_STATE oldWakeState;
    NTSTATUS status;

    LOG(HubFdo, LOG_POWER, 'fWWi', 0, Irp);

    DbgTrace((UsbhDebugTrace, ">IOC Hub FDO WW\n"));
    hubExt = FdoExt(HubFdo);

    // Advance the state to completing
    oldWakeState = InterlockedExchange((PLONG) &hubExt->HubWakeState,
                                        HUB_WAKESTATE_COMPLETING );

    if (oldWakeState == HUB_WAKESTATE_ARMED) {
        // Normal case, IoCancelIrp isn’t being called. Note that we already
        // marked the IRP pending in our dispatch routine
        //
        // return success so that the irp completes.

        hubExt->FdoFlags.SystemWakeSource = (PoGetSystemWake(Irp) ? 1 : 0);
        status = STATUS_SUCCESS;
    } else {
        UsbhAssert(HubFdo, oldWakeState == HUB_WAKESTATE_ARMING_CANCELLED);
        // IoCancelIrp is being called RIGHT NOW. The disarm code will try
        // to put back the WAKESTATE_ARMED state. It will then see our
        // WAKESTATE_COMPLETED value, and complete the IRP itself!

        status = STATUS_MORE_PROCESSING_REQUIRED;
    }

    UsbhEtwLogHubIrpEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_WAIT_WAKE_COMPLETE, 0, status);

    // release the remove lock now since as far as the FDO stack is concerned
    // this irp is done
    IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

    return status;
}


NTSTATUS
UsbhFdoWakePoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

    WAIT_WAKE (PO) completion for USB hub FDO.

Arguments:

Irql: DPC

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PSTATE_CONTEXT sc;
    NTSTATUS nts;
    HUB_FDO_POWERSTATE pwr_state;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, ">POC Hub FDO WW\n"));

    // since we may be reentered on our own thread we assume the global context for this
    // request.
    sc = PNP_CONTEXT(HubFdo);

    //
    // Zero already freed IRP pointer (not necessary, but nice when debugging)
    //
    hubExt->PendingWakeIrp = NULL;

    //
    // Restore state (old state will have been completing)
    //
    hubExt->HubWakeState = HUB_WAKESTATE_DISARMED;


    // We wish to avoid multiple requests to D0 so we check here to see if
    // a request is pending in the power manager.
    //
    // The above two variables are not part of the power state machine so we don't try
    // to protect them with the lock.  The wake-up, arm-disarm mechanism is a separate function
    // The FDO power module is currently only interested in the fact the IRP completed.
    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_WAKE_POCOMPLETE, WakePoComplete_Action_Tag1);

    nts = IoStatus->Status;

    if (NT_SUCCESS(nts)) {

        //
        // success indicates this hub is either the source of USB resume
        // signalling or in the path of USB resume signalling.
        //
        // Only one possibility here:
        // Resume signaling to wake the system originated upstream of this hub
        //
        // for this case we don't request a D0 we let the system send it
        //
        LOG(HubFdo, LOG_POWER, 'hWAK', 0, 0);

        // indicate hub is in the path or is the source of resume
        hubExt->FdoFlags.ResumeSignalling = 1;
        DbgTrace((UsbhDebugTrace, ">POC Hub FDO **WW**\n"));
        UsbhKdPrint((1, "Hub #%d FDO (%p) **detected wake (IRP)**\n", hubExt->HubNumber, HubFdo));


        switch(GET_FDO_POWER_STATE(sc)) {
        // system is suspended and we caught a wake irp
//jd
#if 0
        case FdoSx_Dx:
            {
            POWER_STATE ps;

            ps.DeviceState = PowerDeviceD0;

            DbgTrace((UsbhDebugTrace, ">POR Hub FDO PoRequestD0 %p\n", HubFdo));
            LOG(HubFdo, LOG_POWER, 'PrD0', 0, 0);

            UsbhSetFdoPowerState(HubFdo, nts, Fdo_WaitD0, Ev_WAKE_POCOMPLETE);

            UsbhReleaseFdoPwrLock(HubFdo, sc);

            nts = PoRequestPowerIrp(HubFdo,
                                    IRP_MN_SET_POWER,
                                    ps,
                                    UsbhFdoD0PoComplete_Action,
                                    sc,
                                    NULL);

            UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_TEST_ERROR, WakePoComplete_Action_Tag2);

            if (NT_ERROR(nts) &&  GET_FDO_POWER_STATE(sc) == Fdo_WaitD0) {
                // !ERROR PATH NOTE! the assumption here is that the request is not pending if an
                // error is returned.

                LOG(HubFdo, LOG_POWER, 'sdER', 0, nts);
                // DX remains unchanged
                UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_POREQUEST_ERROR, nts);
            }
            SET_EVENT(&hubExt->PendingWakeIrpEvent);

            LOG(HubFdo, LOG_POWER, 'PRD0', IoStatus->Status, nts);
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            // return original status of the wake
            return IoStatus->Status;
            }
#endif
        // system is suspended and we caught a wake irp
        case FdoSx_Dx:
            break;
        // system and hub are in the 'working' state, this is the ss type case
        case FdoS0_D0:
            nts = UsbhQueueWorkItem(HubFdo,
                                    DelayedWorkQueue,
                                    UsbhSShResumeWorker,
                                    SSH_API_CONTEXT(HubFdo),
                                    0,
                                    SIG_SSH_WORKER);
            break;
        // we are currently powering down
        case FdoWaitSxIoComplete_D0:
        case FdoWaitSx_WaitDx:
        case FdoWaitSx_WaitDxPoComplete:

            TEST_TRAP();
            break;

        // we are already powering up
        case Fdo_WaitD0:
        case Fdo_WaitD0Worker:
        case Fdo_WaitD0PoComplete:
            break;
        }

        SET_EVENT(&hubExt->PendingWakeIrpEvent);

        pwr_state = GET_FDO_POWER_STATE(sc);
        UsbhSetFdoPowerState(HubFdo, nts, pwr_state, Ev_WAKE_POCOMPLETE);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

    } else {

        SET_EVENT(&hubExt->PendingWakeIrpEvent);

        pwr_state = GET_FDO_POWER_STATE(sc);
        UsbhSetFdoPowerState(HubFdo, nts, pwr_state, Ev_WAKE_POCOMPLETE_ERROR);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

    // return status here will indicate that this FDO is in the path or source of the wake-up.
    }

    return IoStatus->Status;

}


NTSTATUS
UsbhFdoPower_WaitWake(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    Process the wake irp for the hub FDO

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    HUB_WAKE_STATE oldWakeState;

    // we armed ourselves for wake-up
    LOG(HubFdo, LOG_POWER, 'fWW+', 0, Irp);
    hubExt = FdoExt(HubFdo);

    UsbhEtwLogHubIrpEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_WAIT_WAKE_DISPATCH, 0, STATUS_SUCCESS);

    // take the remove lock while we process the WAIT_WAKE request
    nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

    if (NT_ERROR(nts)) {
        // device is removed
        Irp->IoStatus.Status = nts;
        // no PoStartNextPowerIrp for WAIT_WAKE

        LOG(HubFdo, LOG_POWER, 'fWWr', nts, Irp);
        UsbhEtwLogHubIrpEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_WAIT_WAKE_COMPLETE, 0, nts);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return nts;
    }

    // track the irp sent down the stack
    hubExt->PendingWakeIrp = Irp;

    // Advance the state if the armed if we are to proceed
    oldWakeState = InterlockedCompareExchange( (PLONG) &hubExt->HubWakeState,
                                               HUB_WAKESTATE_ARMED,
                                               HUB_WAKESTATE_WAITING );

    if (oldWakeState == HUB_WAKESTATE_WAITING_CANCELLED) {
        // We got disarmed, finish up and complete the IRP
        hubExt->HubWakeState = HUB_WAKESTATE_COMPLETING;

        LOG(HubFdo, LOG_POWER, 'fWW1', 0, Irp);

        UsbhEtwLogHubIrpEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_WAIT_WAKE_COMPLETE, 0, STATUS_CANCELLED);

        Irp->IoStatus.Status = STATUS_CANCELLED;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        return STATUS_CANCELLED;
    }


    // We went from WAITING to ARMED. Set a completion routine and forward
    // the IRP. Note that our completion routine might complete the IRP
    // asynchronously, so we mark the IRP pending
    IoMarkIrpPending(Irp);
    IoCopyCurrentIrpStackLocationToNext(Irp);
    UsbhSetCompletionRoutine(HubFdo,
                             Irp,
                             UsbhFdoWaitWake_IoCompletion,
                             NULL,
                             TRUE,
                             TRUE,
                             TRUE);

    LOG(HubFdo, LOG_POWER, 'fWW2', 0, Irp);

    // hub is armed, reset resume detect flag

    hubExt->FdoFlags.ResumeSignalling = 0;

    PoCallDriver(hubExt->TopOfStackDeviceObject,
                 Irp);

    return STATUS_PENDING;
}


NTSTATUS
UsbhFdoPower_SetPower(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++
Routine Description:

    SET_POWER
    Handler for IRP_MN_SET_POWER  irp

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_SET_POWER);

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_POWER, 'Pset', 0, Irp);
    UsbhStartThisPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

    switch (ioStack->Parameters.Power.Type) {
    case SystemPowerState:
        return UsbhFdoSystemPowerState(HubFdo, Irp);
    case DevicePowerState:
        return UsbhFdoDevicePowerState(HubFdo, Irp);
    }

    // unknown power, just pass on
    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);
    return PoCallDriver(hubExt->TopOfStackDeviceObject,
                        Irp);
}


NTSTATUS
UsbhFdoScheduleDeferredPowerRequest(
    PDEVICE_OBJECT HubFdo,
    PHUB_WORKITEM_ROUTINE Handler,
    PHUB_POWER_CONTEXT PowerCtx,
    PIRP DeviceIrp
    )
/*++
Routine Description:

    Schedules a deferred request on a worker thread. We use this to execute
    power code outside the context of the OS power engine.

Arguments:

Irql: ANY

Return Value:

    nt status, failure if we can't queue the workitem

--*/
{
    //PHUB_DEFERRED_POWER_REQUEST_DATA wiData;
    NTSTATUS nts;
    PHUB_WORKITEM hubWi;

    
    hubWi = NULL;

    // Currently we use high priority workitems.
    LOG(HubFdo, LOG_POWER, 'sdp1', DeviceIrp, PowerCtx);

    if (DeviceIrp) {

        hubWi = UsbhAllocWorkItem(HubFdo,
                                  Handler,
                                  PowerCtx,
                                  0,
                                  SIG_POWER_WI,
                                  FdoExt(HubFdo)->IsBootDevice);

        if (hubWi) {
            // the irp is effectively pended here, release the lock on the
            // the power engine associated with this irp.
            IoMarkIrpPending(DeviceIrp);
            UsbhAssert(HubFdo, PowerCtx->PowerReqType == Pdo_SetPower);
            UsbhAssert(HubFdo, PowerCtx->DevObject != NULL);

            // use the fdo_tag since it indicates the file
            UsbhPoStartNextPowerIrp_Pdo(HubFdo, PowerCtx->DevObject, DeviceIrp, fdopwr_c_tag);
        } else {
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    nts = UsbhQueueWorkItemEx(HubFdo,
                              CriticalWorkQueue,
                              Handler,
                              PowerCtx,
                              0,
                              SIG_POWER_WI,
                              hubWi);

    return nts;
}


NTSTATUS
UsbhFdoDxPoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

    ->FdoWaitSx_WaitDxPoComplete
        FdoSxDx->

    Handles DevicePowerState D1, D2, D3 completion

    This code runs for system power cases only.

Arguments:

    Context = PowerCtx, contains state context and current irp
            SystemPowerIrp .

Irql: DPC

Return Value:
    nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;
    PIRP systemPowerIrp;
    PIO_STACK_LOCATION ioStack;
    PSTATE_CONTEXT sc;
    PHUB_POWER_CONTEXT powerCtx = Context;

    UsbhAssert(HubFdo, PowerState.DeviceState != PowerDeviceD0);
    UsbhAssert(HubFdo, powerCtx->Sig == SIG_POWER_CTX);

    systemPowerIrp = powerCtx->SystemIrp;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_POWER, 'dxPC', Context, MinorFunction);
    DbgTrace((UsbhDebugTrace, ">POC Hub FDO Power.DeviceState %d \n",
            PowerState.DeviceState));

    // we are always in the global hub context for fdo system power
    sc = powerCtx->Sc;
    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_DX_POCOMPLETE, DxPoComplete_Action_Tag);
    UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoWaitSx_WaitDxPoComplete);

    // complete the system power irp now
    // We are now in the appropriate D state D1, D2 or D3

    nts = systemPowerIrp->IoStatus.Status = IoStatus->Status;
    ioStack = IoGetCurrentIrpStackLocation(systemPowerIrp);

    //
    // Enable the D0 List for PDOs so that while processing D0 IRPs , 
    // the PDOs can wait in that list till the parent comes back to D0.
    // Note that we don't need to hold the lock because we won't get a
    // D0 IRP while processing Dx IRP as we haven't asked Power Manager 
    // to start the next IRP yet.
    //
    hubExt->PendingPdoD0ListEnabled = TRUE;

    // set current DX, SX state set in SxIoComplete
    SET_FDO_DX(HubFdo, PowerState.DeviceState);

    UsbhSetFdoPowerState(HubFdo, 0, FdoSx_Dx, Ev_DX_POCOMPLETE);
    UsbhReleaseFdoPwrLock(HubFdo, sc);

    // we stalled the system Irp until we could reach the appropriate D state,
    // Now complete it

    // bugbug_power
    // should we map a D0 failure here to a failure of the system irp

    UsbhEtwLogHubPowerEvent(HubFdo, systemPowerIrp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_DX_COMPLETE, PowerState, nts);

    // D and S irps finished, we are done with power
    UsbhReleasePowerContext(HubFdo, powerCtx);
    UsbhPoStartNextPowerIrp_Fdo(HubFdo, systemPowerIrp, fdopwr_c_tag);
    IoCompleteRequest(systemPowerIrp, IO_NO_INCREMENT);

    KeQuerySystemTime(&hubExt->Stat_SyncResumeCompleteAt);

    IoReleaseRemoveLock(&hubExt->RemoveLock, systemPowerIrp);

    // status returned here doesn't really matter because the kernel doesn't
    // check it.  We return the IoStatus passed to us.

    return nts;
}


NTSTATUS
UsbhFdoD0PoComplete_Action(
    PDEVICE_OBJECT       HubFdo,
    UCHAR                MinorFunction,
    POWER_STATE          PowerState,
    PVOID                Context,
    PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

    ->FdoS0_WaitD0PoComplete
        FdoS0_D0 ->

    ->FdoSx_WaitD0PoComplete
        FdoSx_D0 ->

        Hub_PowerFailure->

    Handles DevicePowerState D0 PoCompletion

    Occurs when the hub finally reaches D0, ie the D0 irp has been completed
    and the hub has been powered and all child devices have been restored or
    resumed.

Arguments:

    Context -- StateContext

Irql: DPC

Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc;
    HUB_FDO_POWERSTATE curState;
    PHUB_POWER_CONTEXT powerCtx = Context;
    PIRP systemIrp;
    BOOLEAN workItemQueued;

    // Hub Fdo
    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, PowerState.DeviceState == PowerDeviceD0);
    UsbhAssert(HubFdo, powerCtx->Sig == SIG_POWER_CTX);

    LOG(HubFdo, LOG_POWER, 'd0po', Context, IoStatus->Status);
    DbgTrace((UsbhDebugTrace, ">POC Hub FDO Power.DeviceState D0 \n"));

    workItemQueued = FALSE;

    // keep same status as D0 irp
    nts = IoStatus->Status;

    sc = powerCtx->Sc;
    systemIrp = powerCtx->SystemIrp;

    LOG(HubFdo, LOG_POWER, 'dpo1', Context, sc);
    ASSERT_HUB_CONTEXT(sc);
    UsbhAssert(HubFdo, &hubExt->SyncPnpStateCtx == sc);

    curState = UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_D0_POCOMPLETE, D0PoComplete_Action_Tag1);
    KeQuerySystemTime(&hubExt->Stat_AsyncResumeStartAt);
    // the hub is actually in D0 now, the PDO has been set and the FDO has
    // restored the state.
    do {
        NTSTATUS qw_nts;

        if (NT_ERROR(nts)) {

            UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_D0_POCOMPLETE_ERROR, nts, powerCtx);
            // D and S irps finished, we are done with power
            break;
        }

        UsbhSetFdoPowerState(HubFdo, GET_FDO_SX(HubFdo), Fdo_WaitD0Worker, Ev_D0_POCOMPLETE);
        SET_FDO_DX(HubFdo, PowerDeviceD0);

        UsbhReleaseFdoPwrLock(HubFdo, sc);

        //UsbhFdoSetD0
        qw_nts = UsbhFdoScheduleDeferredPowerRequest(HubFdo,
                                                        UsbhFdoD0Worker_Action,
                                                        powerCtx,
                                                        NULL);

        if (NT_ERROR(qw_nts)) {
            KeQuerySystemTime(&hubExt->Stat_AsyncResumeCompleteAt);

            curState = UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_D0_POCOMPLETE_QW_ERROR,
                        D0PoComplete_Action_Tag2);

            // no longer waiting on worker
            TEST_TRAP();
            UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_D0_POCOMPLETE_QW_ERROR, qw_nts, powerCtx);

            // D and S irps finished, we are done with power
            // power failure cal throws exception
            break;
        } else {
            workItemQueued = TRUE;
        }

    } WHILE (0);

    // if the worker is successfully queued then the power context may already be freed.

    // complete the pended S irp if fast resume is disabled.
    // os has us in D0 but we may not be finished yet, we release both D and S irps now
    // regardless

    UsbhEtwLogHubPowerEvent(HubFdo, NULL, &powerCtx->ActivityId, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COMPLETE, PowerState, nts);

    if (HubG.GlobalDisableFastResume) {
        UsbhAssert(HubFdo, systemIrp);
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, systemIrp, fdopwr_c_tag);
        IoCompleteRequest(systemIrp, IO_NO_INCREMENT);
        IoReleaseRemoveLock(&hubExt->RemoveLock, systemIrp);
    }

    KeQuerySystemTime(&hubExt->Stat_SyncResumeCompleteAt);

    if (workItemQueued == FALSE) {
        IoReleaseRemoveLock(&hubExt->RemoveLock, powerCtx);
    }

    return nts;
}


NTSTATUS
UsbhFdoSetPowerSx_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemPowerIrp,
    PHUB_POWER_CONTEXT PowerCtx
    )
/*++
Routine Description:

    entry state:
    ->FdoS0_D0
    exit state:
       FdoWaitSxIoComplete_D0->


   Handles IRP_MN_SET_POWER SystemPowerState for a hub for low power state.
   This function prepares the hub for low power from the S0_D0 or 'working state'.

   At this point our child devices should have already received the system
   state notification and requested the appropriate D-states -- but you never know.

Arguments:

Irql: PASSIVE

Return Value:

    not failable
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    SYSTEM_POWER_STATE systemState;
    PSTATE_CONTEXT sc;

    ioStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    systemState = ioStack->Parameters.Power.State.SystemState;
    hubExt = FdoExt(HubFdo);
    sc = PowerCtx->Sc;

    LOG(HubFdo, LOG_POWER, 'Plow', 0, SystemPowerIrp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_SET_POWER);
    UsbhAssert(HubFdo, ioStack->Parameters.Power.Type == SystemPowerState);

    UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoS0_D0);
    UsbhAssert(HubFdo, PowerCtx->Sig == SIG_POWER_CTX);
    UsbhAssert(HubFdo, PowerCtx->SystemState == systemState);
    UsbhAssert(HubFdo, PowerCtx->SystemIrp == SystemPowerIrp);

    UsbhSetFdoPowerState(HubFdo, systemState, FdoWaitSxIoComplete_D0, Ev_SET_POWER_SX);

    UsbhReleaseFdoPwrLock(HubFdo, sc);

    // turn off selective suspend for duration of system power
    UsbhSshEnterSx(HubFdo, sc);

    // stall system irp with an iocompletion routine,
    // no send the S irp down.

    IoMarkIrpPending(SystemPowerIrp);
    IoCopyCurrentIrpStackLocationToNext(SystemPowerIrp);

    UsbhSetCompletionRoutine(HubFdo,
                             SystemPowerIrp,
                             UsbhFdoSxIoComplete_Action,
                             PowerCtx,
                             TRUE,
                             TRUE,
                             TRUE);

    PoCallDriver(hubExt->TopOfStackDeviceObject,
                 SystemPowerIrp);

    LOG(HubFdo, LOG_POWER, 'plw>', 0, 0);

    return STATUS_PENDING;
}


NTSTATUS
UsbhFdoS0IoComplete_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemIrp,
    PVOID Context
    )
/*++

Routine Description:

    ->FdoWaitS0IoComplete_Dx
        FdoS0_WaitD0IoComplete ->
        Fdo_PowerFailure ->

    Handles SystemPowerState completion, this is where we
    request the D state IRP.

    This routine handles PowerSystemWorking

Arguments:

Irql: DPC

Return Value:
    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    SYSTEM_POWER_STATE systemState;
    PSTATE_CONTEXT sc;
    NTSTATUS nts;
    PHUB_POWER_CONTEXT powerCtx;

    ioStack = IoGetCurrentIrpStackLocation(SystemIrp);
    hubExt = FdoExt(HubFdo);

    powerCtx = Context;
    UsbhAssert(HubFdo, powerCtx->Sig == SIG_POWER_CTX);
    UsbhAssert(HubFdo, SystemIrp == powerCtx->SystemIrp);

    systemState = ioStack->Parameters.Power.State.SystemState;
    LOG(HubFdo, LOG_POWER, 's0IC', SystemIrp, SystemIrp->IoStatus.Status);
    DbgTrace((UsbhDebugTrace, ">IOC Hub FDO Power.SystemState S0 \n"));

    sc = powerCtx->Sc;
    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_S0_IOCOMPLETE, S0IoComplete_Action_Tag1);

    UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoWaitS0IoComplete_Dx);
    UsbhAssert(HubFdo, GET_FDO_DX(HubFdo) != PowerDeviceD0);

    nts = SystemIrp->IoStatus.Status;

    do {
        POWER_STATE ps;

        SET_FDO_SX(HubFdo, PowerSystemWorking);

        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_POWER, 'PD0!', 0, nts);
            UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_S0_IOCOMPLETE_ERROR, nts, powerCtx);

            // system irps are not really supposed to fail, if we get a failure we still
            // assume we are in system working.
            break;
        }

        ps.DeviceState = PowerDeviceD0;
        powerCtx->DeviceState = PowerDeviceD0;

        DbgTrace((UsbhDebugTrace, ">POR Hub FDO PoRequestD0 %p\n", HubFdo));
        LOG(HubFdo, LOG_POWER, 'PrD0', 0, nts);

        UsbhSetFdoPowerState(HubFdo, 0, Fdo_WaitD0, Ev_S0_IOCOMPLETE);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        UsbhEtwLogHubPowerEvent(HubFdo, SystemIrp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_START, ps, STATUS_SUCCESS);

        nts = IoAcquireRemoveLock(&hubExt->RemoveLock, powerCtx);

        if (!NT_SUCCESS(nts)) {
            nts = STATUS_SUCCESS;
            UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_S0_IOCOMPLETE_ERROR, nts, powerCtx);
            break;
        }

        nts = PoRequestPowerIrp(HubFdo,
                                IRP_MN_SET_POWER,
                                ps,
                                UsbhFdoD0PoComplete_Action,
                                powerCtx,
                                NULL);

        LOG(HubFdo, LOG_POWER, 'P1D0', 0, nts);
        UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_S0_IOCOMPLETE, S0IoComplete_Action_Tag2);

        if (NT_SUCCESS(nts) && 
            (!HubG.GlobalDisableFastResume)) {
            // Fast Resume - complete the S0 IRP while D0 IRP is still pending            
            LOG(HubFdo, LOG_POWER, 'CmpS', SystemIrp, nts);
            nts = STATUS_SUCCESS;
            UsbhReleaseFdoPwrLock(HubFdo, sc);
        } 
        else if (nts == STATUS_PENDING) {
            // fast resume disabled
            LOG(HubFdo, LOG_POWER, 'PndS', SystemIrp, nts);

            UsbhReleaseFdoPwrLock(HubFdo, sc);
            // pend the S-Irp until D irp completes
            return STATUS_MORE_PROCESSING_REQUIRED;

        } else {
            nts = STATUS_SUCCESS;
            // DX remains unchanged
            UsbhAssert(HubFdo, nts != STATUS_MORE_PROCESSING_REQUIRED);
            UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_POREQUEST_ERROR, nts, powerCtx);
            IoReleaseRemoveLock(&hubExt->RemoveLock, powerCtx);
        }

    } WHILE (0);

    UsbhAssert(HubFdo, nts != STATUS_MORE_PROCESSING_REQUIRED);

    UsbhPoStartNextPowerIrp_Fdo(HubFdo, SystemIrp, fdopwr_c_tag);

    IoReleaseRemoveLock(&hubExt->RemoveLock, SystemIrp);

    LOG(HubFdo, LOG_POWER, 'PrRT', SystemIrp, nts);

    return nts;

}


NTSTATUS
UsbhFdoSetPowerS0_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemPowerIrp,
    PHUB_POWER_CONTEXT PowerCtx
    )
/*++
Routine Description:

   ->FdoSx_Dx

    FdoWaitS0IoComplete_Dx->

   Handles IRP_MN_SET_POWER SystemPowerState for a hub for the working state
   when the hub is not in D0

Arguments:

Irql: PASSIVE

Return Value:

    STATUS_PENDING

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    SYSTEM_POWER_STATE systemState;
    PSTATE_CONTEXT sc;

    ioStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    systemState = ioStack->Parameters.Power.State.SystemState;

    UsbhAssert(HubFdo, PowerCtx->Sig == SIG_POWER_CTX);
    sc = PowerCtx->Sc;

    UsbhAssert(HubFdo, ioStack->Parameters.Power.State.SystemState ==
                    PowerSystemWorking);

    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_POWER, 'Pwrk', 0, SystemPowerIrp);

    UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoSx_Dx);
    UsbhAssert(HubFdo, GET_FDO_DX(HubFdo) != PowerDeviceD0);
    UsbhAssert(HubFdo, systemState == PowerCtx->SystemState);

    // defer the set D0 request to a later time or when one of our
    // PDOs goes to D0 we don't pend this request so that the power
    // manager can keep running.

    // request a deferred D0
    LOG(HubFdo, LOG_POWER, 'Pend', 0, SystemPowerIrp);

    UsbhSetFdoPowerState(HubFdo, 0, FdoWaitS0IoComplete_Dx, Ev_SET_POWER_S0);

    UsbhReleaseFdoPwrLock(HubFdo, sc);


    IoMarkIrpPending(SystemPowerIrp);
    IoCopyCurrentIrpStackLocationToNext(SystemPowerIrp);
    UsbhSetCompletionRoutine(HubFdo,
                             SystemPowerIrp,
                             UsbhFdoS0IoComplete_Action,
                             PowerCtx,
                             TRUE,
                             TRUE,
                             TRUE);

    // call down the system state SystemPowerIrp, we return to D0 later
    PoCallDriver(hubExt->TopOfStackDeviceObject,
                 SystemPowerIrp);

    return STATUS_PENDING;

}

BOOLEAN
UsbhIsHybridSleepState(
    PIO_STACK_LOCATION ioStack,
    PSYSTEM_POWER_STATE RealSystemState
    )
/*++
Routine Description:

    Determine if this is Hybrid suspend or not.  If this is Hybrid Suspend,
    return real System State.

Arguments:

    Pointer to ioStack

    Pointer to System Power State

Return Value:

    TRUE,  if this is Hybrid Suspend.
    FALSE, if this is regular Suspend.

--*/
{
    SYSTEM_POWER_STATE requestedSystemState;
    SYSTEM_POWER_STATE_CONTEXT sysPowerCtx;
    BOOLEAN isHybridSleep = FALSE;

    sysPowerCtx = ioStack->Parameters.Power.SystemPowerStateContext;
    requestedSystemState = ioStack->Parameters.Power.State.SystemState;
    if ((ULONG)requestedSystemState != sysPowerCtx.TargetSystemState) {

        // this is a fast s4 transition, return the true sleep state
        *RealSystemState = sysPowerCtx.TargetSystemState;
        isHybridSleep = TRUE;
    }

    return isHybridSleep;
}


NTSTATUS
UsbhFdoSxIoComplete_Action(
    PDEVICE_OBJECT HubFdo,
    PIRP SystemIrp,
    PVOID Context
    )
/*++

Routine Description:

    entry state:
    ->FdoWaitSxIoComplete_D0

    exit state:
    FdoWaitSx_WaitDx ->
    Fdo_PowerFailure->

    Handles SystemPowerState completion, this is where we request the D state IRP for the
    hub.

    This routine handles all cases except PowerSystemWorking

Arguments:

Irql: DPC

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    POWER_STATE powerState;
    SYSTEM_POWER_STATE systemState, oldSystemState, realSystemState;
    PSTATE_CONTEXT sc;
    PHUB_POWER_CONTEXT powerCtx = Context;

    UsbhAssert(HubFdo, powerCtx->SystemIrp == SystemIrp);
    ioStack = IoGetCurrentIrpStackLocation(SystemIrp);
    hubExt = FdoExt(HubFdo);

    if (UsbhIsHybridSleepState(ioStack, &realSystemState)) {
       systemState = realSystemState;
    } else {
       systemState = ioStack->Parameters.Power.State.SystemState;
    }

    UsbhAssert(HubFdo, powerCtx->SystemState == ioStack->Parameters.Power.State.SystemState);
    LOG(HubFdo, LOG_POWER, 'sxIC', Context, SystemIrp);

    sc = powerCtx->Sc;
    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_SX_IOCOMPLETE, SxIoComplete_Action_Tag1);

    UsbhAssert(HubFdo,  GET_FDO_POWER_STATE(sc) == FdoWaitSxIoComplete_D0);

    // set the power status vbus flag based on what we expect to happen
    // to the bus.
    //
    // S1, S3 we expect Vbus_On unless the system cannot support
    // wake-up from those states
    // S4 and deeper we expect to Vbus_Off unless the system can wake
    // from these states -- if they can wake we expect vbus_powerInterrupted

    hubExt->HubVbusState = Vbus_Off;

    if (UsbhCanSuspend(HubFdo, systemState)) {
        powerState.DeviceState = PowerDeviceD2;
        hubExt->HubVbusState = Vbus_On;
    } else {
        powerState.DeviceState = PowerDeviceD3;
        hubExt->HubVbusState = Vbus_Off;
    }

    // system states > S3 that support wake are considered Vbus_interrupted
    // since the system will have to reboot
    // This is hibernate and shutdown
    if (systemState > PowerSystemSleeping3 &&
        hubExt->HubVbusState == Vbus_On) {
        hubExt->HubVbusState = Vbus_Interrupted;
    }

    UsbhSetFdoPowerState(HubFdo, 0, FdoWaitSx_WaitDx, Ev_SX_IOCOMPLETE);
    powerCtx->VbusState = hubExt->HubVbusState;
    powerCtx->DeviceState = powerState.DeviceState;

    oldSystemState = GET_FDO_SX(HubFdo);
    SET_FDO_SX(HubFdo, systemState);
    UsbhReleaseFdoPwrLock(HubFdo, sc);

    // DDK says ignore current state.
    // always request the power irp, lower driver has to handle
    // this.
    LOG(HubFdo, LOG_POWER, 'sxRQ', powerState.DeviceState, SystemIrp);
    DbgTrace((UsbhDebugTrace, ">IOC Hub FDO Power.SystemState %d \n", systemState));

    UsbhEtwLogHubPowerEvent(HubFdo, SystemIrp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_DX_START, powerState, STATUS_SUCCESS);

    nts = PoRequestPowerIrp(HubFdo,
                             IRP_MN_SET_POWER,
                             powerState,
                             UsbhFdoDxPoComplete_Action,
                             powerCtx,
                             NULL);

    if (nts != STATUS_PENDING) {

        // probably a memory allocation failure
        LOG(HubFdo, LOG_POWER, 'sxER', nts, SystemIrp);

        // set the status to the returned status and allow the irp to complete
        SystemIrp->IoStatus.Status = nts;

        UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_SX_IOCOMPLETE_ERROR, SxIoComplete_Action_Tag2);
        // !ERROR_PATH_NOTE!
        // this leaves us in some undefined state we are guessing that we should remain in
        // whatever state we were in before the request failed.

        // on error restore original system state.
        SET_FDO_SX(HubFdo, oldSystemState);

        UsbhFdoPower_PowerFailureEntry(HubFdo, Ev_SX_IOCOMPLETE_ERROR, nts, powerCtx);
        // we are done with power
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, SystemIrp, fdopwr_c_tag);

        UsbhEtwLogHubPowerEvent(HubFdo, SystemIrp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_DX_COMPLETE, powerState, nts);

        IoReleaseRemoveLock(&hubExt->RemoveLock, SystemIrp);

        // don't return errors for power -- we have to deal with this ourselves.
        return STATUS_SUCCESS;

    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
UsbhFdoSystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++
Routine Description:

    Handles IRP_MN_SET_POWER SystemPowerState for a hub.

    Handles both power up and power down.

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    SYSTEM_POWER_STATE systemState;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;
    PHUB_POWER_CONTEXT powerCtx;
    POWER_STATE powerState;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    
    powerState = ioStack->Parameters.Power.State;
    systemState = ioStack->Parameters.Power.State.SystemState;

    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_POWER, 'Psys', 0, Irp);

    nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

    UsbhEtwLogHubPowerEvent(HubFdo, 
                            Irp,
                            NULL,
                            systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_DISPATCH : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_DISPATCH,
                            powerState,
                            STATUS_SUCCESS);

    if (NT_ERROR(nts)) {
        // device is removed
        Irp->IoStatus.Status = nts;

        LOG(HubFdo, LOG_POWER, '1sys', nts, Irp);
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

        UsbhEtwLogHubPowerEvent(HubFdo, 
                                Irp,
                                NULL,
                                systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_COMPLETE : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_COMPLETE,
                                powerState,
                                nts);

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return nts;
    }

    sc = PNP_CONTEXT(HubFdo);
    UsbhAcquireFdoPnpLock(HubFdo, sc, Ev_SYSTEM_POWER, SystemPowerState_Tag0, 1);

    if (GET_FDO_PNPSTATE(sc) == FDO_WaitPnpStop_CB)  {
        UsbhReleaseFdoPnpLock(HubFdo, sc);
        
        // A system power event has occured before the async start callback from
        // USBPORT has occurred, and before the callback's timeout has expired.
        // Try to cancel the timeout and the async callback.
        
        UsbhDisableTimerObject(HubFdo, hubExt->HubInitTimeoutTimer);
        
        nts = Usbh_HubRootHubInitNotification(HubFdo, NULL, NULL);

        if (nts == STATUS_CANCELLED) {
            // USBPORT never executed the callback.  Complete the async start here.
            Usbh_FDO_Pnp_State(sc, Ev_RH_CALLBACK);            
        } else {
            // USBPORT has started execution of the callback.  Wait for it to complete.
            WAIT_EVENT(&hubExt->Evnt_FdoAsyncStart);
        }

        UsbhAcquireFdoPnpLock(HubFdo, sc, Ev_SYSTEM_POWER, SystemPowerState_Tag3, 1);
    }

    // STARTED?
    if (GET_FDO_PNPSTATE(sc) != FDO_WaitPnpStop) {
        GUID activityId;
        NTSTATUS idStatus;

        UsbhReleaseFdoPnpLock(HubFdo, sc);

        LOG(HubFdo, LOG_POWER, '2sys', nts, Irp);
        // record the nop
        UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_SYSTEM_POWER, SystemPowerState_Tag1);

        TEST_TRAP();
        if (systemState == PowerSystemWorking) {
            UsbhSetFdoPowerState(HubFdo, 0, FdoS0_D0, Ev_SET_POWER_S0_NoPM);
        } else {
            UsbhSetFdoPowerState(HubFdo, 0, FdoSx_Dx, Ev_SET_POWER_SX_NoPM);
        }

        UsbhReleaseFdoPwrLock(HubFdo, sc);

        idStatus = UsbhEtwGetActivityId(Irp, &activityId);

        // hub hardware not running so there is nothing to power manage yet
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                           Irp);

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);
        UsbhEtwLogHubPowerEvent(HubFdo, 
                                NULL,
                                NT_SUCCESS(idStatus) ? &activityId : NULL,
                                systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_COMPLETE : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_COMPLETE,
                                powerState,
                                nts);
        return nts;
    }

    UsbhReleaseFdoPnpLock(HubFdo, sc);

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_SYSTEM_POWER, SystemPowerState_Tag2);

    //handle case of entering another system state after being disconnected.
    if (GET_FDO_POWER_STATE(sc) == Fdo_PowerFailureDisconnected) {
        GUID activityId;
        NTSTATUS idStatus;

        if (systemState == PowerSystemWorking) {
            UsbhSetFdoPowerState(HubFdo, systemState, Fdo_PowerFailureDisconnected, Ev_SET_POWER_S0);
        } else {
            UsbhSetFdoPowerState(HubFdo, systemState, Fdo_PowerFailureDisconnected, Ev_SET_POWER_SX);
        }
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        idStatus = UsbhEtwGetActivityId(Irp, &activityId);

        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoSkipCurrentIrpStackLocation(Irp);
        nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                           Irp);

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        UsbhEtwLogHubPowerEvent(HubFdo,
                                NULL,
                                NT_SUCCESS(idStatus) ? &activityId : NULL,
                                systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_COMPLETE : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_COMPLETE,
                                powerState,
                                nts);
        return nts;
    }

    if (systemState == PowerSystemWorking &&
        GET_FDO_SX(HubFdo) == PowerSystemWorking) {

        GUID activityId;
        NTSTATUS idStatus;

        UsbhReleaseFdoPwrLock(HubFdo, sc);
        // re-asserting system working, someone failed the query Sx.
        // TEST #2010

        idStatus = UsbhEtwGetActivityId(Irp, &activityId);

        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoSkipCurrentIrpStackLocation(Irp);
        nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                           Irp);

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        UsbhEtwLogHubPowerEvent(HubFdo, 
                                NULL,
                                NT_SUCCESS(idStatus) ? &activityId : NULL,
                                systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_COMPLETE : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_COMPLETE,
                                powerState,
                                nts);
        return nts;
    }

    // acquire the context here , we release it when we think we (and the power manager)
    // are done with power up or power down.
    powerCtx = UsbhAcquirePowerContext(HubFdo,
                                       sc,
                                       HubFdo,
                                       Irp,
                                       NULL,
                                       Fdo_SetPower,
                                       systemState!=PowerSystemWorking);
    if (powerCtx == NULL) {
        // device is removed
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        nts = Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;

        LOG(HubFdo, LOG_POWER, '1sys', nts, Irp);
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

        UsbhEtwLogHubPowerEvent(HubFdo, 
                                Irp,
                                NULL,
                                systemState==PowerSystemWorking ? &USBHUB_ETW_EVENT_HUB_POWER_SET_S0_COMPLETE : &USBHUB_ETW_EVENT_HUB_POWER_SET_SX_COMPLETE,
                                powerState,
                                nts);

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        return nts;
    }

    switch(systemState) {

    case PowerSystemSleeping1:
    case PowerSystemSleeping2:
    case PowerSystemSleeping3:
    case PowerSystemHibernate:
    case PowerSystemShutdown:

        DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.SystemState %d [current = %d]\n",
                                    systemState,
                                    GET_FDO_DX(HubFdo)));

        return UsbhFdoSetPowerSx_Action(HubFdo, Irp, powerCtx);

    case PowerSystemWorking:
        {
        HUB_FDO_POWERSTATE newHubPwrState;

        KeQuerySystemTime(&hubExt->Stat_SyncResumeStartAt);
        // defer the set D0 request to a later time or when one of our
        // PDOs goes to D0 we don't pend this request so that the power
        // manager can keep running.
        DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.SystemState S0 [current = %d]\n",
                                    GET_FDO_DX(HubFdo)));

        // internal transition by default
        newHubPwrState = GET_FDO_POWER_STATE(sc);
        switch(GET_FDO_POWER_STATE(sc)) {
        case FdoSx_Dx:
            // this is the normal power up sequence
            return UsbhFdoSetPowerS0_Action(HubFdo, Irp, powerCtx);

        // this is the codepath hit if we get the system message while the D0 power up
        // is already in progress
        case FdoSx_D0:
            // already in d0
            UsbhAssert(HubFdo, GET_FDO_DX(HubFdo) == PowerDeviceD0);
            UsbhAssert(HubFdo, GET_FDO_SX(HubFdo) != PowerSystemWorking);
            newHubPwrState = FdoS0_D0;
            break;

        // already powering up
        case Fdo_WaitD0:
        case Fdo_WaitD0Worker:
        case Fdo_WaitD0PoComplete:
            break;

        // if we are in some other state -- perhaps due to pnp errors
        // then fall thru and complete the request

        default:
            UsbhAssertFailure(HubFdo, Irp);
            break;
        }

        SET_FDO_SX(HubFdo, PowerSystemWorking);
        UsbhSetFdoPowerState(HubFdo, 0, newHubPwrState, Ev_SET_POWER_S0_Internal);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        }
    }

    // this general case, we just pass on the irp

    // done with the power irp
    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);
    nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                       Irp);

    IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

    return nts;
}


NTSTATUS
UsbhFdoDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:
    ->FdoS0_WaitD0
        FdoS0_WaitD0IoComplete->

    ->FdoSx_WaitD0
        FdoSx_WaitD0IoComplete->

    Handles IRP_MN_SET_POWER DevicePowerState for a hub.

    Our devices(child PDOs) should have already received SET_POWER.

    This routine handles both SS and system power cases.

Arguments:

Irql: PASSIVE

Return Value:

    nt status
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;
    POWER_STATE powerState;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_POWER, 'Pdev', 0, Irp);

    powerState = ioStack->Parameters.Power.State;
    deviceState = ioStack->Parameters.Power.State.DeviceState;

    if (deviceState != PowerDeviceD0) {
        nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

        if (NT_ERROR(nts)) {
            Irp->IoStatus.Status = nts;
            UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);

            LOG(HubFdo, LOG_POWER, 'Pdv!', nts, Irp);
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return nts;
        }
    }

    sc = PNP_CONTEXT(HubFdo);

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_DEVICE_POWER, DevicePowerState_Tag);

    switch(deviceState) {
    case PowerDeviceD0:
        LOG(HubFdo, LOG_POWER, 'P_D0', 0, Irp);
        DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.DeviceState D0\n"));
        UsbhEtwLogHubPowerEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_DISPATCH, powerState, STATUS_SUCCESS);

        UsbhSetFdoPowerState(HubFdo, 0, Fdo_WaitD0PoComplete, Ev_SET_POWER_D0);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        nts = UsbhFdoSetPowerD0_Action(HubFdo, sc, Irp);
        break;

    //
    // D1 is not used but we have the same behavior.
    // 2 reasons: backward compatibility and set D irps
    // are non-failable according to DDK.
    // we assert in the checked build since it may be a bug
    //
    case PowerDeviceD1:
    case PowerDeviceD2:

        UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoWaitSx_WaitDx);
        LOG(HubFdo, LOG_POWER, 'P_Dx', deviceState, Irp);
        UsbhEtwLogHubPowerEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_DX_DISPATCH, powerState, STATUS_SUCCESS);
        DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.DeviceState %d\n",
            deviceState));
        UsbhAssert(HubFdo, deviceState != PowerDeviceD1);

        UsbhSetFdoPowerState(HubFdo, Vbus_On, FdoWaitSx_WaitDxPoComplete, Ev_SET_POWER_D2);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        nts = UsbhFdoSetPowerDx_Action(HubFdo, hubExt->CurrentFdoPwrCtx, Irp);
        break;

    case PowerDeviceD3:

        UsbhAssert(HubFdo, GET_FDO_POWER_STATE(sc) == FdoWaitSx_WaitDx);
        LOG(HubFdo, LOG_POWER, 'P_Dx', deviceState, Irp);
        UsbhEtwLogHubPowerEvent(HubFdo, Irp, NULL, &USBHUB_ETW_EVENT_HUB_POWER_SET_DX_DISPATCH, powerState, STATUS_SUCCESS);
        DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.DeviceState %d\n",
            deviceState));

        UsbhSetFdoPowerState(HubFdo, Vbus_Off, FdoWaitSx_WaitDxPoComplete, Ev_SET_POWER_D3);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        nts = UsbhFdoSetPowerDx_Action(HubFdo, hubExt->CurrentFdoPwrCtx, Irp);
        break;

    default:

        UsbhReleaseFdoPwrLock(HubFdo, sc);
        // default is to just pass on
        UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                           Irp);

    }

    return nts;
}


NTSTATUS
UsbhFdoColdStartCheckPort(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PHUB_PORT_DATA PortData
    )
/*++
Description:

   Handles the case where a device was added during suspend/hibernate

Arguments:

Return:

    Status of the operation
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    USB_HUB_PORT_STATE ps;
    USBD_STATUS usbdStatus;

    hubExt = FdoExt(HubFdo);

    // case 1:
    // device was present before hibernate occurred
    nts = UsbhQueryPortState(HubFdo,
                             PortData->PortNumber,
                             &ps,
                             &usbdStatus);

    LOG(HubFdo, LOG_POWER, 'cdC1', PortData->PortNumber, nts);

    if (NT_ERROR(nts)) {

        LOG(HubFdo, LOG_POWER, 'cdC2', 0, nts);
        if (Usb_Disconnected(nts)) {
            // the hub has been removed, just complete the cold
            // start sequence
            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_HUB_DISCONNECT, 0, nts);
            return STATUS_SUCCESS;
        } else {

            // we don't expect errors here so log an exception and
            // return an error -- the Cold-Start will fail.
            HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                      PowerErr_GetPortStatusFailed,
                      nts, usbdStatus, NULL, 0);
        }
        return nts;
    }

    if (ps.Status.Connect && !ps.Change.ConnectChange) {
        // queue a fake connect change for this port

        LOG(HubFdo, LOG_POWER, 'cdC3', 0, nts);
        //UsbhAssert(HubFdo, !ps.Status.Enable);

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_PORT_CONNECT, PortData->PortNumber, nts);

        // Flag this port as needing a soft connect change
        PortData->NeedsSoftConnectChange = TRUE;
    }

    return nts;

}


NTSTATUS
UsbhFdoColdStartPdo(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData
    )
/*++
Description:

    Handles the case where the PDO was present before the power event (eg
    hibernate).

Arguments:

    Pdo - latched PDO.  the bus engine won't disconnect the PDO but PnP might try to

Return:

    Status of the operation
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    NTSTATUS nts;
    USB_HUB_PORT_STATE ps;
    USBD_STATUS usbdStatus;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    // case 1:
    // device was present before hibernate occurred
    nts = UsbhQueryPortState(HubFdo,
                             PortData->PortNumber,
                             &ps,
                             &usbdStatus);

    LOG(HubFdo, LOG_POWER, 'cdP1', PortData->PortNumber, nts);

    if (NT_ERROR(nts)) {
        LOG(HubFdo, LOG_POWER, 'cl2E', 0, nts);
        if (Usb_Disconnected(nts)) {
            // the hub has been removed, just complete the cold
            // start sequence
            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_HUB_DISCONNECT, 0, nts);
            return STATUS_SUCCESS;
        } else {

            // we don't expect errors here so log an exception and
            // return an error -- the Cold-Start will fail.
            HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                      PowerErr_GetPortStatusFailed,
                      nts, usbdStatus, NULL, 0);
        }
        return nts;
    } else {
    
        // Need to check to see if the boot/paging/hiber/crash device's connect status bit is set before proceeding
        //
        if (pdoExt->IsBootDevice) {
            // This means that a upstream hub in the boot path was removed 
            // was connected back with a downstream hub or boot device missing
            if (ps.Status.Connect == 0) {
                // Need to keep reading port status until the Connect bit is set
                //
                DbgTrace((UsbhPwBootTrace,"Boot device missing from HubFdo=#%p %!FUNC!\n", HubFdo));
                LOG(HubFdo, LOG_POWER, 'SRBM', 0, HubFdo);

                // Set SQM surprise removal count flag to indicate that the boot device has been unplugged
                //
                UsbhWaitForBootDevice(HubFdo , 
                                      Pdo, 
                                      HUB_RESET_NEED_BIT_SET,
                                      HUB_RESET_DO_NOT_NEED_BIT);

                // The device is back, lets get the port state again
                nts = UsbhQueryPortState(HubFdo,
                                        PortData->PortNumber,
                                        &ps,
                                        &usbdStatus);

                if (NT_ERROR(nts)) {
                    LOG(HubFdo, LOG_POWER, 'cl3E', 0, nts);
                    if (Usb_Disconnected(nts)) {
                        // the hub has been removed, just complete the cold
                        // start sequence
                        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_HUB_DISCONNECT, 0, nts);
                        return STATUS_SUCCESS;
                    } else {
                        // we don't expect errors here so log an exception and
                        // return an error -- the Cold-Start will fail.
                        HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                              PowerErr_GetPortStatusFailed,
                              nts, usbdStatus, NULL, 0);
                    }
                    return nts;
                }

            }
        }
    }

    // see if the device is still connected and is the same device

    LOG(HubFdo, LOG_POWER, 'cd11', ps.Status.us, ps.Change.us);
    if (ps.Status.Connect) {
        // case 1:
        // device still connected

        if (ps.Change.ConnectChange) {
            PDEVICE_EXTENSION_PDO pdoExt;

            // device still connected and change asserted,
            // flush this change
            LOG(HubFdo, LOG_POWER, 'cdP2', PortData->PortNumber, 0);
            pdoExt = PdoExt(Pdo);

            nts = UsbhFlushPortChange(HubFdo, PortData->PortNumber,
                    &usbdStatus);

            if ((NT_SUCCESS(nts)) && (pdoExt->PdoFlags.PortDisabled == 0)) {
                // mark this device for a reset when put it in D0
                pdoExt->PdoFlags.NeedsReset = 1;
                // this indicates function driver did not expect to lose power
                if (pdoExt->VbusState == Vbus_On) {
                    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_PORT_LOST_POWER, PortData->PortNumber, nts);

                    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                          PowerErr_DeviceUnexpectedlyLostPower,
                          nts, usbdStatus, NULL, 0);
                    pdoExt->VbusState = hubExt->HubVbusState;

                }
            } else {
                if (Usb_Disconnected(nts)) {
                    // the hub has been removed, just complete the cold start
                    // sequence
                    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_HUB_DISCONNECT, 0, nts);
                    return STATUS_SUCCESS;
                } else {
                    // we don't expect errors here so log an exception and
                    // return an error
                    // Cold-Start will fail.
                    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                          PowerErr_FlushPortChangeFailed,
                          nts, usbdStatus, NULL, 0);
                }
            }
        }else if (pdoExt->PdoFlags.PortDisabled == 0) {
            // no change asserted for this port, just mark the
            // device for reset.
            LOG(HubFdo, LOG_POWER, 'cdP3', PortData->PortNumber, 0);
            PdoExt(Pdo)->PdoFlags.NeedsReset = 1;
            nts = STATUS_SUCCESS;
        }


    } else {

        LOG(HubFdo, LOG_POWER, 'cdP4', PortData->PortNumber, 0);
        // case 2:
        // device was present before hibernate but is no longer present
        if (!ps.Change.ConnectChange) {

            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_PORT_DISCONNECT, PortData->PortNumber, nts);

            // since devices sometimes fail to appear on resume even though they are present
            // we log an error here to detect this.  Some devices take longer than the spec minimum
            // to signal the proper attach.
            HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, PowerErr_DeviceDisconnectOnResume,
                          0, 0, NULL, 0);


            // queue an artificial connect change if one is not be currently
            // asserted so that the device will be removed when the enumeration
            // engine is started.
            UsbhAssert(HubFdo, !ps.Status.Enable);
            UsbhQueueSoftConnectChange(HubFdo, PortData->PortNumber, Sc, FALSE);

            LOG(HubFdo, LOG_POWER, 'cdP5', PortData->PortNumber, nts);

        } else {
            nts = STATUS_SUCCESS;
        }
    }

    return nts;
}


NTSTATUS
UsbhFdoSetD0Cold(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    BOOLEAN Reset
    )
/*++
Description:

    Perform a cold SetD0 on the hub.  This will only occur in the case of power
    management resume where the hub has lost power for one reason or another.

    The 'cold' resume Sequence looks like this

    BS_BusSuspend
        |
        |----BE_RequestBusReset,PCE_BusReset(port)
        +
    BS_BusSuspend
        |
        |----BE_RequestBusResume, PCE_Resume(port), PCE_Enable(port)
        |
    BS_BusRun


Arguments:

Return:

    ntstatus not success if an error is encountered resuming the hub

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT port;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    // The D0 irp has been completed by our parent
    //
    // Perform a cold restart of the hub. The hub has been restored
    // at this point so configuration, interface and pipehandles
    // should still be valid.
    LOG(HubFdo, LOG_POWER, 'cold', 0, 0);
    DbgTrace((UsbhDebugTrace, "Cold start hub\n"));

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_START, 0, STATUS_SUCCESS);

    if (Reset) {
        nts = UsbhResetHubUpstreamPort(HubFdo);

        if (NT_ERROR(nts)){
           if (!Usb_Disconnected(nts)) {
                // the hub has failed to respond, note this
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                          PowerErr_PostResumeResetFailed,
                          nts, 0, NULL, 0);
            }
            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_COMPLETE, 0, nts);
            return nts;
        }
    }

    // not like a 1394 bus reset or is it?
    // this will reset our queues to the pause state.
    UsbhSyncBusReset(HubFdo, Sc);

    // The hub may have lost power due to some hardware condition like
    // overcurrent or in the case of the root the controller may have reset
    // effectively killing power to the root hub.
    // if this was an unexpected power loss

    // re-power the ports
    nts = UsbhSyncPowerOnPorts(HubFdo);

    if (NT_ERROR(nts)) {
        if (!Usb_Disconnected(nts)) {
            // the hub has failed to respond, note this
            HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                          PowerErr_PowerPortsFailed,
                          nts, 0, NULL, 0);
        }
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_COMPLETE, 0, nts);
        return nts;
    }

    // core spec requires a 100ms time after power for devices to signal the
    // attach.
    UsbhWait(HubFdo, 100);

    // all ports successfully powered, and we have waited the required
    // time for it to reach 'power good' and see the attached devices.

    //
    // Step 1:
    // Walk the ports and see if we have any devices previously attached
    // devices. If so flush out any change indications for these ports.
    //

    for (port=1; port <= N_PORTS(HubFdo); port++) {

        PHUB_PORT_DATA pd;
        PDEVICE_OBJECT pdo;

        pd = UsbhGetPortData(HubFdo, port);
        //UsbhAssert(HubFdo, pd);

        if (pd) {

            pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_Cold_Tag);
            LOG(HubFdo, LOG_POWER, 'cld1', pd->PortNumber, pdo);

            if (pdo) {
                nts = UsbhFdoColdStartPdo(HubFdo, Sc, pdo, pd);
                if (NT_ERROR(nts)) {
                    // stop on first error
                    UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold_Tag);
                    break;
                }
                UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold_Tag);
            } else {
                // no device was present before suspend, see if one was
                // attached
                nts = UsbhFdoColdStartCheckPort(HubFdo, Sc, pd);
                if (NT_ERROR(nts)) {
                    // stop on first error
                    break;
                }
            }
        }
    }

    //
    // Step 2:
    // Change indications for all devices that persisted across the power event
    // have been flushed.  Any remaining changes queued are for devices that
    // were actually removed or newly attached.
    //

    // restart the enumeration engine.
    UsbhSyncBusResume(HubFdo, Sc);


    // step 3:
    // Restore (re-enumerate) any devices that are marked for reset. This is to
    // handle the case where the device was swapped, we will need to queue
    // changes for devices that switched identities.
    // We need to re-enumerate here in order to detect the device change.
    //
    
    //
    // If we are in the boot path we must first reset/restore the boot device and notify it
    // that it has been reset.  Otherwise we can take page-faults in the reset/enum
    // path that block because the boot device is not accessible.
    //
    if (hubExt->IsBootDevice) {
        for (port=1; port <= N_PORTS(HubFdo); port++) {
            PDEVICE_OBJECT pdo;
            PDEVICE_EXTENSION_PDO pdoExt;

            pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_Cold2_Tag);
            if (pdo) {
                pdoExt = PdoExt(pdo);

                if (pdoExt->IsBootDevice) {
                    nts = UsbhSyncResetDeviceInternal(HubFdo, Sc, pdo);

                    if (NT_SUCCESS(nts)) {
                        UsbhResetNotifyCompletion(Sc->HubFdo);
                    } else {
                        UsbhQueueSoftConnectChange(HubFdo, port, Sc, TRUE);
                        pdoExt->PdoFlags.NeedsReset = 0;
                        
                        //
                        // No point in resetting/enumerating other devices if the boot
                        // device is not accessible.  
                        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_COMPLETE, 0, nts);
                        UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold2_Tag);

                        return nts;
                    }
                }
                UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold2_Tag);
            }
        }
    }


    for (port=1; port <= N_PORTS(HubFdo); port++) {

        PDEVICE_OBJECT pdo;
        PDEVICE_EXTENSION_PDO pdoExt;

        // since the change engine is running now we need to latch
        // on the PDO in order to restore the device.
        pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_Cold2_Tag);
        LOG(HubFdo, LOG_POWER, 'ren1', port, pdo);

        if (pdo) {
            pdoExt = PdoExt(pdo);
            if (pdoExt->IsBootDevice) {
                UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold2_Tag);
                continue;
            }
            if (pdoExt->PdoFlags.NeedsReset) {
                // device was lost during suspend, we will need to
                // restore it
                LOG(HubFdo, LOG_POWER, 'PDRS', 0, 0);
                // initiate our own reset for this device
                UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_PORT_RESET, pdoExt->PortNumber, STATUS_SUCCESS);
                nts = UsbhSyncResetDeviceInternal(HubFdo, Sc, pdo);
                if (NT_ERROR(nts)) {
                    // queue a connect change for this port, this is either
                    // not the same device or we were unable to
                    // re-enumerate it.

                    LOG(HubFdo, LOG_POWER, 'ren2', 0, nts);

                    // queue the change message to the bus engine and disable the
                    // hub port.
                    UsbhQueueSoftConnectChange(HubFdo, port, Sc, TRUE);
                    pdoExt->PdoFlags.NeedsReset = 0;

                    // it is not necessarily an error for the device to be different,
                    // log this as an exception in case the hw failed.
                    HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                          PowerErr_PostHiberRestoreFailed,
                          nts, 0, NULL, 0);

                    nts = STATUS_SUCCESS;

                } else {
                    pdoExt->PdoFlags.NeedsReset = 0;

                }
            }
            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Cold2_Tag);
        } else {
            PHUB_PORT_DATA pd;
            
            // check if we need to simulate a connect change for this port to trigger enumeration
            pd = UsbhGetPortData(HubFdo, port);
            if (pd->NeedsSoftConnectChange) {
                pd->NeedsSoftConnectChange = FALSE;
                UsbhQueueSoftConnectChange(HubFdo, port, Sc, FALSE);
            }
        }
    }

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_COLD_COMPLETE, 0, nts);

    return nts;
}




NTSTATUS
UsbhFdoSetD0Warm(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++
Description:

    Perform a Warm SetD0 on the hub. This assumes the hub has not lost power.
    We simply resume our downstream ports.

    The hub is functional at this point. The D0 Irp has been completed for our
    PDO.

    If a port indicates resume signalling we complete the wake-irp with status
    success.  Otherwise we complete any pending wake irps with status cancelled.

    In the case of SS there may be pending idle notification Irps -- if so we
    complete these with STATUS_SUCCESS.

    **NOTE Hardware reset(ESD) for the hub is disabled during this function.

Arguments:

Return:

    ntstatus not success if an error is encountered resuming the hub

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT port;
    PHUB_PORT_DATA pd;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    ULONG ps;
    USB_HUB_PORT_STATE portState;

    hubExt = FdoExt(HubFdo);

    // assume success
    nts = STATUS_SUCCESS;
    // note that the bus engine is not running yet.  We handle all changes and
    // communications synchronously at this point.

    // loop through the ports resuming them and flag any that indicate resume
    // signalling
    LOG(HubFdo, LOG_POWER, 'warm', 0, 0);
    DbgTrace((UsbhDebugTrace, "Warm start hub\n"));
    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_WARM_START, 0, nts);

    for (port=1; port <= N_PORTS(HubFdo); port++) {
        pd = UsbhGetPortData(HubFdo, port);
        if (pd) {
            PDEVICE_EXTENSION_PDO pdoExt;

            nts = UsbhQueryPortState(HubFdo,
                                     port,
                                     &portState,
                                     &usbdStatus);


            if (NT_SUCCESS(nts)) {
                PDEVICE_OBJECT pdo;
    
                pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_WarmReset_Tag);

                if (pdo) {
                    pdoExt = PdoExt(pdo);

                    if (portState.Status.Connect && portState.Change.ConnectChange) {

                        // device may have dropped off the bus or been swapped during suspend.
                        // flush this change and let the driver reset code deal with it.
                        LOG(HubFdo, LOG_POWER, 'wrfl', port, 0);

                        nts = UsbhFlushPortChange(HubFdo, port, &usbdStatus);
                        if (NT_SUCCESS(nts) && (pdoExt->PdoFlags.PortDisabled == 0)) {
                            pdoExt->PdoFlags.NeedsReset = 1; 
                        }

                    } else if (portState.Status.Connect && !portState.Status.Enable && (pdoExt->PdoFlags.PortDisabled == 0)) {
                        
                        // device lost power during suspend; needs to be reset.
                        pdoExt->PdoFlags.NeedsReset = 1;
                        LOG(HubFdo, LOG_POWER, 'wrf2', port, 0);

                    } else if (!portState.Change.ConnectChange && !portState.Status.Connect) {
                        
                        // Device disconnected during suspend without a connect change.  
                        // Port needs to be reenumerated.
                        pd->PD_Flags.Reenumerate = 1;
                        LOG(HubFdo, LOG_POWER, 'wrf3', port, 0);
                    }

                    UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_WarmReset_Tag); 

                } else if (!portState.Change.ConnectChange && portState.Status.Connect) {

                    // Device connected during suspend without a connect change, or enumeration was
                    // cancelled due to system suspend.  Port needs to be reenumerated.
                    pd->PD_Flags.Reenumerate = 1;
                    LOG(HubFdo, LOG_POWER, 'wrf4', port, 0);
                }
            }
        }
    }

    // Enable the bus engine now so it can begin handling change indications
    // for devices that may have disconnected and for ports that need to be
    // resumed.

    LOG(HubFdo, LOG_POWER, 'bsRM', 0, 0);

    // this will resume the hub enum engine and resume the ports.
    // This will call PCE_Resume on all the ports, reset the resume event and timeout flags.
    UsbhSyncBusResume(HubFdo, StateContext);

    do {
        // wait for all our ports to complete resume
        for (port=1; port <= N_PORTS(HubFdo); port++) {

            pd = UsbhGetPortData(HubFdo, port);
            //UsbhAssert(HubFdo, pd);

            if (pd) {
                nts = UsbhWaitForPortResume(HubFdo, pd, StateContext);
    
                if (NT_ERROR(nts)) {
                    // Test #2004

                    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_WARM_PORT_RESUME_ERROR, pd->PortNumber, nts);
                    // if we cannot recover the hub may be dead, keep the error for this
                    // port failure and fail the warm-start.
                    break; // exit for
                }
            }
        }

        if (NT_ERROR(nts)) {
            break;
        }

        // 11.6.3.7
        // resume has completed if necessary,
        // wait the prescribed 10ms before allowing traffic to the device
        UsbhWait(HubFdo, 10);

        // re-validate the upstream connection state, this is where we detect hub
        // hardware failure that is NOT the result of an un-plug. If the upstream
        // state is valid and the hardware failed we faill back to a cold start.
        //
        // if the hub was cycled during resume then we will report disconnected.
        //

        nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

        if (NT_ERROR(nts)) {
            break;
        }

        for (port=1; port <= N_PORTS(HubFdo); port++) {
            pd = UsbhGetPortData(HubFdo, port);

            if (pd) {
                PDEVICE_OBJECT pdo;
                PDEVICE_EXTENSION_PDO pdoExt;
                NTSTATUS ntStatus = STATUS_SUCCESS;

                if (pd->PD_Flags.Reenumerate == 1) {
                    LOG(HubFdo, LOG_POWER, 'wrsc', port, 0);
                    pd->PD_Flags.Reenumerate = 0;
                    UsbhQueueSoftConnectChange(HubFdo, port, StateContext, TRUE);
                    continue;
                } 

                pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_WarmReset_Tag);

                if (pdo) {
                    pdoExt = PdoExt(pdo);

                    if ( (pdoExt->DeviceFlags.ResetOnResume) || (pdoExt->PdoFlags.NeedsReset) ) {
                        
                        pdoExt->PdoFlags.NeedsReset = 0;
                        pdoExt->PdoFlags.ResetOnLastResume = 1;
                        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_WARM_RESET_ON_RESUME, PdoExt(pdo)->PortNumber, STATUS_SUCCESS);
                        ntStatus = UsbhSyncResetDeviceInternal(HubFdo, PDO_CONTEXT(pdo), pdo);

                        if (pdoExt->PdoFlags.DeviceIsHub) {
                            // Notify the child hub that it lost power, so it can be cold started when it 
                            // receives the set D0 power request
                            UsbhSendSynchronousUsbIoctlRequest(HubFdo,
                                pdo,
                                IOCTL_INTERNAL_USB_NOTIFY_HUB_PWR_LOSS);
                        }
                    }
                    UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_WarmReset_Tag); 
                    if (NT_ERROR(ntStatus)) {
                        // queue a connect change for this port, this is either
                        // not the same device or we were unable to
                        // re-enumerate it.

                        LOG(HubFdo, LOG_POWER, 'WmRS', 0, ntStatus);
    
                        // queue the change message to the bus engine and disable the
                        // hub port.
                        UsbhQueueSoftConnectChange(HubFdo, port, StateContext, TRUE);

                        // it is not necessarily an error for the device to be different,
                        // log this as an exception in case the hw failed.
                        HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                            PowerErr_ResumeOnResetFailed,
                            ntStatus, 0, NULL, 0);

                    }
                }
            }
        }
        
        if (hubExt->FdoFlags.HubHardResetPending) {
            // some failure occurred resulting in a reset.

            // clear the flag and return an error
            // we will drop back to a cold start
            hubExt->FdoFlags.HubHardResetPending = 0;

            HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_HardwareFailureOnWarmStart,
                                          0, 0, NULL, 0);

            nts = STATUS_UNSUCCESSFUL;
            break;
        }

    } WHILE (0);

    // re-enable ESD
    UsbhResumeHardReset(HubFdo, StateContext);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_SET_D0_WARM_COMPLETE, 0, nts);
    return nts;
}


VOID
UsbhCompletePdoWakeIrps(
    PDEVICE_OBJECT HubFdo
    )
/*++
Description:

     Complete any pending wake irps we have as appropriate.  We only complete
     with STATUS_SUCCESS if we detect resume signalling on the port.

Arguments:

Return:

    none

--*/
{
    USHORT port;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_OBJECT pdo;
    ULONG resumeCount = 0;

    for (port=1; port <= N_PORTS(HubFdo); port++) {

        pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_WakeIrpCpt_Tag);

        if (pdo) {
            pdoExt = PdoExt(pdo);

            // complete if resume detected
            if (pdoExt->PdoFlags.ResumeSignal) {
                BOOLEAN wake_completed;

                wake_completed = UsbhCompletePdoWakeIrp(HubFdo, pdo, STATUS_SUCCESS);

                if (wake_completed) {
                    // complete the wake irp with success

                    // note that only one port should have signalled resume, since we
                    // if we find >1 it may be a hardware or HCD bug so we log an exception
                    // we still complete all resumed ports wake irps with success.
                    //
                    resumeCount++;
                    if (resumeCount>1) {
                        HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                                      PowerErr_MultiplePortsSignalledResume,
                                      0, 0, NULL, 0);
                    }
                }
            }
            // reset resume detection flags
            pdoExt->PdoFlags.ResumeSignal = 0;

            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_WakeIrpCpt_Tag);
        }
    }

    FdoExt(HubFdo)->FdoFlags.SystemWakeSource = 0;
}


NTSTATUS
UsbhCheckHubPowerStatus(
    PDEVICE_OBJECT HubFdo
    )
/*++
Description:

     This is the heart of our bad system power management recovery code. After
     a power event we attempt to determine the state of the hub relative to
     what we expect.

     If the hub has lost power and we did not expect it to we return an error
     code that will result in the hub doing a 'cold start' all devices will be
     re-enumerated.  A hardware error will also be logged for tracking
     purposes.

     We also use the vbus state of the hub to determine if power was lost,
     Vbus_Off and Vbus_Interrupted both require a cold start.


Arguments:

Return:

    nt status code, NT_DEVICE_DISCONNECTED_STATUS otherwise the return code from
    transactions to the hub.


--*/
{

    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    ULONG ps = 0;
    USB_HUB_DESCRIPTOR hd;

    hubExt = FdoExt(HubFdo);

    // first check the status of our port to see if we were removed during
    // suspend or hibernate.

    nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

    // process connected status
    do {
        if (NT_SUCCESS(nts)) {
            // still connected and enabled

            if (hubExt->HubVbusState == Vbus_Off ||
                hubExt->HubVbusState == Vbus_Interrupted) {
                // if we expected a power loss just return status of our connection
                LOG(HubFdo, LOG_POWER, '0PWR', 0, nts);

                // not an error, no need to check more hub will reset anyway
                break;
            }

            // fall thru and test the hub, regardless of what port status says
        }

        // disconnected?
        if (Usb_Disconnected(nts)) {

            // if disconnected status then port status is reliable

            if ((ps & USBD_PORT_CONNECTED) == 0) {

                // device was removed, return the DEVICE_NOT_CONNECTED status
                LOG(HubFdo, LOG_POWER, 'PWRr', 0, nts);

                UsbhAssert(HubFdo, Usb_Disconnected(nts));
                break;

            } else {
                // still connected, the device dropped on/off the bus AFTER resume
                // started. Disconnect status is returned for this case and PNP removes
                // and reenumerates the devices.

                LOG(HubFdo, LOG_POWER, 'PW3r', 0, nts);

                HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_DeviceCycledPostResume,
                                      nts, 0, NULL, 0);
                UsbhAssert(HubFdo, Usb_Disconnected(nts));
                break;
            }

        }

        // if some other type of error we cannot rely on port status
        // if disconnect status device dropped on and off the bus, the error
        // will trigger recovery.
        //
        if (NT_ERROR(nts)) {

            // an error getting status, probably not connected anymore,
            // this will trigger a reset unless the error is
            // NT_DEVICE_DISCONNECTED_STATUS
            LOG(HubFdo, LOG_POWER, 'PWR!', 0, nts);

            // log this case
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_ErrorOnUpstreamStatus,
                                      nts, 0, NULL, 0);
            break;
        }

        // at this point we still assume the device is physically present
        // to keep it simple we look at port status (enable bit) only if we got
        // status success.

        UsbhAssert(HubFdo, hubExt->HubVbusState == Vbus_On);

        // check the enable status on our port, if not enabled then we lost power
        if (NT_SUCCESS(nts) && !(ps & USBD_PORT_ENABLED)) {
            LOG(HubFdo, LOG_POWER, 'ePWR', 0, STATUS_UNSUCCESSFUL);
            nts = STATUS_UNSUCCESSFUL;

            HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_UpstreamPortDisabled,
                                      nts, 0, NULL, 0);
            break;
        }

        // we think the hub is powered and it seems to be connected, fetch a
        // descriptor and return the status of this operation.
        // If this operation fails then the hub has probably lost power.
        // In some cases this is legitimate (S4 or S5) but others it is not
        // (S3 or S1)
        // In cases like S4 and S5 HubLostPower should be preset to one so we
        // should not get here
        nts = UsbhGetHubClassDescriptor(HubFdo, &hd);
        LOG(HubFdo, LOG_POWER, 'cPWR', 0, nts);

        // since the device may be unplugged at any time we still check for the
        // disconnect status
        if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, PowerErr_ResumeFailedHubClassDs,
                                      nts, 0, NULL, 0);
            break;
        }

        // if the power cycled on us then the hub may actually be ok and respond to the
        // descriptor even though it is configured (spec says it shouldn't but they
        // have to because of a Windows bug). Get the current config -- if its still set
        // then the hub is probably OK.




        // any additional checks to see if the hub is on-line go here


    } WHILE (0);


    LOG(HubFdo, LOG_POWER, 'Perr', 0, nts);

    return nts;
}


VOID
UsbhFdoD0Worker_Action(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PHUB_POWER_CONTEXT PowerCtx
    )
/*++
Description:
     ->Fdo_WaitD0Worker
        Fdo_WaitD0PoComplete->

     Handles DevicePowerState D0 message for a hub.

     We must handle this message after the completion at which time the device
     will be in D0 and we can talk to it.

     Two types of Set D0. Cold Start and Warm Start

     Cold Start: The hub lost power. In the case we need to re-initialize the
     hub hardware and restore the children.  We will not be able to determine
     the source of the wake -- complete all pending wake irps with error.

     Warm Start: The hub did not lose power and seems to be functioning, check
     the status of the child ports.

     In the case of Selective Suspend this will always be a warm start.

Arguments:

     PowerCtx - structure describing this power event

Return: nt status code


--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;
    HUB_FDO_EVENT fdoEvent;
    HUB_FDO_POWERSTATE curState;

    // reset is TRUE if this is an unexpected power loss ie the port is
    // disabled
    BOOLEAN reset = FALSE;

    // disarm wake irp until we complete transition to D0
    UsbhDisarmHubForWakeDetect(HubFdo);

    nts = Usbh_HubWaitAsyncPowerUp(HubFdo);

    // ioStack = IoGetCurrentIrpStackLocation(irp);

    hubExt = FdoExt(HubFdo);
    sc = PNP_CONTEXT(HubFdo);
    sc->ScThread = KeGetCurrentThread();

    LOG(HubFdo, LOG_POWER, 'PsD0', sc, nts);
//PATH1 failure before we check
//UsbhKdPrint((1, "PATH1 hub#%d\n", hubExt->HubNumber));
//TEST_TRAP();
    // attempt to dynamically detect power loss
    if (!Usb_Disconnected(nts)) {
        nts = UsbhCheckHubPowerStatus(HubFdo);
    }

    // see if we were removed, if so just complete the D irp
    // and wait for PnP.

    if (!Usb_Disconnected(nts)) {

        if (NT_ERROR(nts)) {

            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_D0_WORKER_HUB_FAIL_AFTER_SUSPEND, 0, nts);

            hubExt->HubVbusState = Vbus_Interrupted;
            // initiate a reset before cold start if we get any type of error
            reset = TRUE;
            // this indicates some kind of hardware problem on the hub or in
            // the system.  The devices will recover but the system will have
            // reduced functionality.

            if (hubExt->FdoFlags.HubHardResetPending) {
                // Hub may have failed going into suspend.  Make sure reset pending flag is cleared, 
                // otherwise it can force a cold start on every resume.
                hubExt->FdoFlags.HubHardResetPending = 0;
            }
            
            if (hubExt->FdoFlags.HubIsRoot) {
                // if this is the root hub then the controller failed to meet
                // our hardware requirements for Power Management (see power
                // profiles).  We generate a different exception here so we can
                // detect these problem controllers.
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                                      PowerErr_RootHubFailAfterSuspend,
                                      nts, 0, NULL, 0);
            } else {
                // indicates a general problem with this hub.
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                                      PowerErr_HubFailAfterSuspend,
                                      nts, 0, NULL, 0);
            }

            LOG(HubFdo, LOG_POWER, 'PER0', 0, nts);
        }
//PATH2 failure after we check
UsbhKdPrint((1, "PATH2 hub#%d\n", hubExt->HubNumber));

        // take action based on if the hub has power
        if (hubExt->HubVbusState == Vbus_Off ||
            hubExt->HubVbusState == Vbus_Interrupted) {

            LOG(HubFdo, LOG_POWER, 'Vcld', 0, hubExt->HubVbusState );

            fdoEvent = Ev_D0_WORKER_Cold;
            nts = UsbhFdoSetD0Cold(HubFdo, sc, reset);

            // note that we only enable ESD after we are fully powered
            // we have to re-enable if we want the exception to initiate recovery
            UsbhResumeHardReset(HubFdo, sc);

            if (NT_ERROR(nts) &&
                !Usb_Disconnected(nts)) {
                // failure here will require full reset of the hub
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                                      PowerErr_ColdStartFailed_Reset,
                                      nts, 0, NULL, 0);
            }


        } else {
            // vbus remained on

            LOG(HubFdo, LOG_POWER, 'Vwrm', 0, hubExt->HubVbusState );
            fdoEvent = Ev_D0_WORKER_Warm;

            nts = UsbhFdoSetD0Warm(HubFdo, sc);

            if (NT_ERROR(nts) &&
                !Usb_Disconnected(nts)) {
                // failure here will require full reset of the hub
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                                      PowerErr_WarmStartFailed_Reset,
                                      nts, 0, NULL, 0);
            }
        }

//PATH3 failure after warm start
//UsbhKdPrint((1, "PATH3 hub#%d\n", hubExt->HubNumber));
//TEST_TRAP();


        // complete the D0 IRP we previously pended
        LOG(HubFdo, LOG_POWER, 'cpD0', 0, 0);

        curState = UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_D0_WORKER, D0Worker_Action_Tag1);
        UsbhAssert(HubFdo,  GET_FDO_POWER_STATE(sc) == Fdo_WaitD0Worker);

        // regardless of success or failure we complete the D0 request
        // FdoSx and FdoDx should be set as appropriate.
        // we always succeed the power irp
        UsbhAssert(HubFdo, GET_FDO_DX(HubFdo) == PowerDeviceD0);

        // see if the PoCompletion has run, if it has the we are now in S?_D0.
        if (curState == Fdo_WaitD0Worker) {
            if (GET_FDO_SX(HubFdo) == PowerSystemWorking) {
                UsbhSetFdoPowerState(HubFdo, GET_FDO_SX(HubFdo), FdoS0_D0, Ev_D0_WORKER_Complete);
            } else {
                UsbhSetFdoPowerState(HubFdo, GET_FDO_SX(HubFdo), FdoSx_D0, Ev_D0_WORKER_Complete);
            }
        }

        // unlink this power ctx from the global list
        UsbhReleasePowerContext(HubFdo, PowerCtx);

        UsbhReleaseFdoPwrLock(HubFdo, sc);

    } else {
        // we were disconnected during power

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_POWER_D0_WORKER_HUB_DISCONNECT_AFTER_SUSPEND, 0, nts);

        curState = UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_D0_WORKER, D0Worker_Action_Tag2);

        UsbhAssert(HubFdo, GET_FDO_DX(HubFdo) == PowerDeviceD0);

        UsbhSetFdoPowerState(HubFdo, nts, Fdo_PowerFailureDisconnected, Ev_DISCONNECTED);

        // unlink this power ctx from the global list
        UsbhReleasePowerContext(HubFdo, PowerCtx);

        UsbhReleaseFdoPwrLock(HubFdo, sc);

        // drop all devices, we are disconnected, the bus engine was never started
        UsbhSyncBusDisconnect(HubFdo, sc);
    }

    // complete the D0 IRP we previously pended
    LOG(HubFdo, LOG_POWER, 'cpD0', 0, 0);

    // hub is finished powering up, unblock anyone waiting, if we got an error then
    // we still consider ourselves finished.
    LOG(HubFdo, LOG_POWER, 'sPWe', nts, 0);
    KeQuerySystemTime(&hubExt->Stat_AsyncResumeCompleteAt);

    
    //
    // Unblock any PDOs that are waiting for the parent to go to D0
    //
    UsbhFdoUnblockAllPendedPdoD0Irps(HubFdo);

    // Hub is back in DO, so allow hub selective suspend to occur.
    UsbhSshExitSx(HubFdo, sc);
   
    // complete wait_wake irps as necessary, we do this here after we complete
    // the power irp because the client driver may reenter this function
    // setting the device to D0 and we don't want to block it.

    if (NT_SUCCESS(nts)) {
        UsbhCompletePdoWakeIrps(HubFdo);
    }

    IoReleaseRemoveLock(&hubExt->RemoveLock, PowerCtx);
}


VOID UsbhFdoUnblockAllPendedPdoD0Irps( 
    __inout PDEVICE_OBJECT HubFdo
    )
/*++
Description:
    Walk through the list of child PDOs that are waiting for the
    parent FDO to come back to D0 and unblock them.

Arguments:

    HubFdo - Parent Hub FDO

Return: void


--*/
{

    
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    KIRQL oldIrql;
    LIST_ENTRY tmpListHead;
    PLIST_ENTRY listEntry;
    PHUB_POWER_CONTEXT powerCtx = NULL;
    NTSTATUS ntStatus;

    InitializeListHead(&tmpListHead);

    LOG(HubFdo, LOG_POWER, 'uaD0', 0 , 0);

    KeAcquireSpinLock(&hubExt->LockPdoD0List, &oldIrql );
    
    //
    // No more entries allowed to be added in the pendingD0List
    //
    hubExt->PendingPdoD0ListEnabled = FALSE;

    //
    // Pull out all the entries from the list, and store them in a temporary list
    //
    while ( ! IsListEmpty(&hubExt->PendingPdoD0List)) {
        InsertTailList( &tmpListHead,
                        RemoveHeadList( &hubExt->PendingPdoD0List ) );
    }

    KeReleaseSpinLock(&hubExt->LockPdoD0List, oldIrql );

    //
    // Walk though each element of the list and Unblock the Pending D0 Operation. 
    // Queue a separate work item for each of them so that they can do their 
    // wake operation in parallel and not affect wake times for other devices.
    //
    while ( ! IsListEmpty(&tmpListHead)) {
        listEntry = RemoveHeadList( &tmpListHead );
        InitializeListHead(listEntry);
        powerCtx = CONTAINING_RECORD( listEntry,
                                      HUB_POWER_CONTEXT,
                                      PendingD0ListEntry );

        ntStatus = UsbhQueueWorkItem(HubFdo,
                                     CriticalWorkQueue,
                                     UsbhPdoUnblockPendedD0IrpWI,
                                     powerCtx,
                                     powerCtx->Sc->PortNumber,
                                     SIG_UNBLOCKD0_WORKITEM);

        if (NT_ERROR(ntStatus)) {
            //
            // If we can't allocate work item, we will fall back to directly 
            // unblocking the pending operation. This is a rare error condition, 
            // so it is ok to be not efficient
            //
            UsbhPdoUnblockPendedD0IrpWI(HubFdo,
                                        powerCtx->Sc->PortNumber,
                                        powerCtx);

        }
        

        
    }
}




NTSTATUS
UsbhFdoSetPowerDx_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_POWER_CONTEXT PowerCtx,
    PIRP Irp
    )
/*++

Routine Description:

    ->FdoWaitSx_WaitDx
        FdoWaitSx_WaitDxPoComplete->

    When transitioning the above states we are in the process of suspending the ports.

    Handles D1,D2 and D3 DevicePowerState for a hub.

    The suspend sequence for the Bus usually looks like this:

    BS_BusRun -> BS_BusPause -> BS->Suspend if vbus is On
    and
    BS_BusRun -> BS_BusPause  if vbus is off

Arguments:

Irql: PASSIVE

Return Value:

   status from PoCallDriver

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    DEVICE_POWER_STATE deviceState;
    USHORT port;
    ULONG arm_bus = 0;
    PSTATE_CONTEXT sc;
    ULONG allowWakeOnConnect = 0;
    ULONG pdoCount = 0;
    BOOLEAN bootDeviceArmForWake = FALSE;

    // Classic policy was to <wakeSystemOnConnect> if a device is connected and also armed
    // for wake.  This generates confusion because <wakeSystemOnConnect> is a side effect
    // of arming a device.
    //
    // Setting a hub to <wakeSystemOnConnect> is not required to allow a connected device
    // to wake, however, some hubs may require this.
    //
    // Hence, default policy dictated by hardware is that all hubs are set to
    // <wakeSystemOnConnect> and this can be overridden as appropriate.
    //

    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_POWER, 'PsDx', PowerCtx, Irp);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, PowerCtx->Sig == SIG_POWER_CTX);
    sc = PowerCtx->Sc;
    PowerCtx->DeviceIrp = Irp;

    // implied with any transition to low power, this will allow any reset in progress to
    // finish.
    UsbhPauseHardReset(HubFdo, sc);

    // suspend the enum engine for all ports
    UsbhSyncBusPause(HubFdo, sc, PauseReason_SystemPower);

    //
    // cancel our pending requests and other activity before going to low power
    //

    // Do pre-suspend work specific to power management.

    // Arm the bus and the hub for wake as appropriate.
    deviceState = ioStack->Parameters.Power.State.DeviceState;
    PowerCtx->DeviceState = deviceState;

    switch(deviceState) {

    case PowerDeviceD2:

        // set the power status flag based on what we expect to happen
        // D2 always indicates power is on.

        hubExt->HubVbusState = PowerCtx->VbusState; //Vbus_On;

        // will pick up system power cases that report wake capapbility but interrupt vbus (like hibernate)
        //if (hubExt->HubVbusState != Vbus_On) {
        //    TEST_TRAP();
        //}
        LOG(HubFdo, LOG_POWER, 'Pld2', 0, Irp);

        // D2 = wake-up possible on USB

        // determine if we should wake on connect
        switch (HubG.GlobalWakeOnConnectUI) {
        case HUB_WAKE_C_Never:
        case HUB_WAKE_C_WinXP_Compatible:
            allowWakeOnConnect = 0;
            break;

        case HUB_WAKE_C_Always:
            allowWakeOnConnect = 1;
            break;

        case HUB_WAKE_C_UserControl:
            // use user defined setting
            if (hubExt->FdoFlags.AllowWakeOnConnect) {
                allowWakeOnConnect = 1;
            } else {
                allowWakeOnConnect = 0;
            }
            break;
        }

        // see if we need to arm the bus because of a device.
        // If any devices are armed for wake then we need to arm the bus.

        for (port=1; port <= N_PORTS(HubFdo); port++) {
            PDEVICE_OBJECT pdo;

            pdo = UsbhLatchPdo(HubFdo, port, Irp, PdoLatch_FdoSetD2);
            if(pdo) {
                pdoCount++;
                if ((PdoExt(pdo))->IsBootDevice) {
                    bootDeviceArmForWake = TRUE;
                }
                if (UsbhPdoArmedForWake(pdo)) {
                    UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatch_FdoSetD2);
                    arm_bus = 1;
                    // xp compat mode -- arm the hub if the child is also armed
                    if (HubG.GlobalWakeOnConnectUI == HUB_WAKE_C_WinXP_Compatible) {
                        allowWakeOnConnect = 1;
                    }
                } else {
                    UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatch_FdoSetD2);
                }
            }
        }

        // If the boot device is attached to this hub, we can arm it for wake if
        // 1. The hub is a root hub or
        // 2. There is no other device on this hub
        if (bootDeviceArmForWake) {
            if (hubExt->FdoFlags.HubIsRoot || pdoCount == 1) {
                allowWakeOnConnect = 1;
            }
        }
        // flag value is set at init time
        if (allowWakeOnConnect) {

            UsbhArmHubWakeOnConnect(HubFdo);

            // arm the host controller too.
            arm_bus = 1;

        } else {
            // Hub is not allowed to wake the system (on its own).

            UsbhDisarmHubWakeOnConnect(HubFdo);

        }

        if (arm_bus) {

            //HUB_ARM, BUS_ARM
            //hub may or may not be armed to wake.

            // this posts the ww irp
            UsbhArmHubForWakeDetect(HubFdo);

            UsbdHubArmBusForWake(HubFdo);

        } else {

            // if the bus isn't armed we don't need WW irps
            // HUB_DISARM, HWC_DISARM, BUS_DISARM

            // this posts the ww irp
            UsbhDisarmHubForWakeDetect(HubFdo);

            UsbdHubDisarmBusForWake(HubFdo);
        }
        break;

    case PowerDeviceD3:

        // set the power status flag based on what we expect to happen
        // D3 always indicates a power loss

        hubExt->HubVbusState = Vbus_Off;

        LOG(HubFdo, LOG_POWER, 'Pld3', 0, Irp);

        UsbhDisarmHubWakeOnConnect(HubFdo);

        // wake-up not possible from D3, disarm the hub and bus
        UsbhDisarmHubForWakeDetect(HubFdo);

        UsbdHubDisarmBusForWake(HubFdo);

        // At this point no devices should be armed for wake and no drivers
        // should have pending idle irps
        // *Any PDOs armed for wake will have had their wake irps completed
        // with STATUS_INVALID_DEVICE_STATE (device wake for all USB devices is
        // D2).
        for (port=1; port <= N_PORTS(HubFdo); port++) {
            PDEVICE_OBJECT pdo;

            pdo = UsbhLatchPdo(HubFdo, port, Irp, PdoLatch_FdoSetD3_Tag);
            if(pdo) {
                PDEVICE_EXTENSION_PDO pdoExt;
                BOOLEAN wake_completed;

                pdoExt = PdoExt(pdo);

                //UsbhAssert(HubFdo, !UsbhPdoIsIdle(HubFdo, pdo));

                //jd
                // what should we do with idle irp?

                wake_completed = UsbhCompletePdoWakeIrp(HubFdo, pdo, STATUS_INVALID_DEVICE_STATE);
                if (wake_completed) {
                    // complete the wake irp with error
                    DbgTrace((UsbhDebugTrace,
                        "Port %d PDO (%p) **signal wake** STATUS_INVALID_DEVICE_STATE\n",
                        pdoExt->PortNumber, pdo));
                }
                UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatch_FdoSetD3_Tag);
            }

        }
        break;

    }

    // This will suspend the hub and suspend the ports if they are not already suspended.
    // 'TRUE' selective suspend requires that the port be suspended when the client requests
    // the Dx irp.
    // Classic behavior for the hub was to suspend the ports only at the time the hub
    // itself is suspended.

    UsbhSyncBusSuspend(HubFdo, sc);

    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);
    nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                       Irp);

    IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

    return nts;
}


NTSTATUS
UsbhFdoSetPowerD0_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PIRP Irp
    )
/*++

Routine Description:

    ->Fdo_WaitD0
        FdoWaitS0_WaitD0IoComplete->

    When transitioning the above states we are in the process of suspending the ports.

    Handles D0 DevicePowerState for a hub.

Arguments:

Irql: PASSIVE

Return Value:

   status from PoCallDriver

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace, ">IRP Hub FDO Power.DeviceState D0\n"));

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_POWER, 'p_D0', StateContext, Irp);

    UsbhPoStartNextPowerIrp_Fdo(HubFdo, Irp, fdopwr_c_tag);
    IoSkipCurrentIrpStackLocation(Irp);

    // we are not in D0 until the D0 irp completes

    nts = PoCallDriver(hubExt->TopOfStackDeviceObject,
                       Irp);

    return nts;
}


