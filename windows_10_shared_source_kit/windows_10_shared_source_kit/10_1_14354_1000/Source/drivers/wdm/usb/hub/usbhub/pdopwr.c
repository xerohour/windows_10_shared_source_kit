/*
Module: PDOPWR.C


name:USB Wdm Power management

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002

Abstract:


    Provides power management for usbhub.sys PDOs

Hub Device States:

    Power states for the hub are well defined and map to USB Vbus states.

    D0 - on, can receive SOFs;              Vbus_On
    D1 - not used (treated same as D2)
    D2 - USB suspend, wake-up possible       Vbus_On, VbusInterrupted
    D3 - OFF device loses power             Vbus_Off

USB vbus Power States:

    Possible power states for USB when entering system sleep states. Any given S or D
    state will map to one of these three.

    - Vbus_On
    - Vbus_Off
    - Vbus_Interrupted


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


/**************************************************************************************************

Power request handling (Device PDO)

---------------------------------------------------------------------------------------------------
QUERY_POWER_DX:

    IRP_MN_QUERY_POWER
....................................................................................................
        WinNT5.1(XP)(SP2)
            IRP_MN_QUERY_POWER (unspecified)
            IRP_MN_QUERY_POWER (D0)
            IRP_MN_QUERY_POWER (D1)
            IRP_MN_QUERY_POWER (D2)
            IRP_MN_QUERY_POWER (D3)
                always succeeds (STATUS_SUCCESS)
.....................................................................................................
        WinNT6(neohub)
            IRP_MN_QUERY_POWER (unspecified)
            IRP_MN_QUERY_POWER (D0)
            IRP_MN_QUERY_POWER (D1)
            IRP_MN_QUERY_POWER (D2)
            IRP_MN_QUERY_POWER (D3)
                always succeeds (STATUS_SUCCESS)


-----------------------------------------------------------------------------------------------------
QUERY_POWER_SX:

    IRP_MN_QUERY_POWER
....................................................................................................
        WinNT5.1(XPSP2)
            IRP_MN_QUERY_POWER (unspecified)
            IRP_MN_QUERY_POWER (S0)
            IRP_MN_QUERY_POWER (S1)
            IRP_MN_QUERY_POWER (S2)
            IRP_MN_QUERY_POWER (S3)
                fails (STATUS_UNSUCCESSFUL) if Pdo armed for wake (WW posted) AND sys_state
                maps to D3
            IRP_MN_QUERY_POWER (S4)(hiber)
            IRP_MN_QUERY_POWER (S5)
                succeeds regardless of wake irp status

.....................................................................................................
        WinNT6(neohub)
            IRP_MN_QUERY_POWER (unspecified)
            IRP_MN_QUERY_POWER (S0)
            IRP_MN_QUERY_POWER (S1)
            IRP_MN_QUERY_POWER (S2)
            IRP_MN_QUERY_POWER (S3)
            IRP_MN_QUERY_POWER (S4)(hiber)
            IRP_MN_QUERY_POWER (S5)
                succeeds regardless of wake irp status

-----------------------------------------------------------------------------------------------------
SET_POWER_DX

    IRP_MN_SET_POWER
.....................................................................................................
         WinNT5.1(XPSP2)
            Completes with STATUS_SUCCESS if already marked in this power state

            IRP_MN_SET_POWER (D0)
            IRP_MN_SET_POWER (D1)
                Suspends the port
            IRP_MN_SET_POWER (D2)
                Suspends the port
            IRP_MN_SET_POWER (D3)
                Suspends the port, completes WW irps with
                    STATUS_POWER_STATE_INVALID
            IRP_MN_SET_POWER (unspecified)
                Completes Irp with STATUS_INVALID_PARAMETER

.....................................................................................................
         WinNT6(neohub)
            Completes with STATUS_SUCCESS.
            Completes STATUS_POWER_STATE_INVALID if the parent hub is not in a valid power
            state to handle this request (indicates some type of function driver error).

            IRP_MN_SET_POWER (D0)
            IRP_MN_SET_POWER (D1)
                Suspends the port
            IRP_MN_SET_POWER (D2)
                Suspends the port
            IRP_MN_SET_POWER (D3)
                 Suspends the port, completes WW irps with
                    STATUS_POWER_STATE_INVALID
            IRP_MN_SET_POWER (unspecified)

-----------------------------------------------------------------------------------------------------
SET_POWER_SX

    IRP_MN_SET_POWER
.....................................................................................................
         WinNT5.1(XPSP2)
            Always completes with STATUS_SUCCESS

            IRP_MN_SET_POWER (unspecified)
            IRP_MN_SET_POWER (S0)
            IRP_MN_SET_POWER (S1)
            IRP_MN_SET_POWER (S2)
            IRP_MN_SET_POWER (S3)
            IRP_MN_SET_POWER (S4) (hiber)
            IRP_MN_SET_POWER (S5)


.....................................................................................................
         WinNT6(neohub)
            Always completes with STATUS_SUCCESS

            IRP_MN_SET_POWER (unspecified)
            IRP_MN_SET_POWER (S0)
            IRP_MN_SET_POWER (S1)
            IRP_MN_SET_POWER (S2)
            IRP_MN_SET_POWER (S3)
            IRP_MN_SET_POWER (S4) (hiber)
            IRP_MN_SET_POWER (S5)


-----------------------------------------------------------------------------------------------------
WAIT_WAKE

    IRP_MN_WAIT_WAKE


.....................................................................................................
         WinNT5.1(XPSP2)
possible error codes on ww irp complete
STATUS_INVALID_DEVICE_STATE ; if hub is stopping or not in D0
STATUS_NOT_SUPPORTED        ; if device does not support remote wakeup
STATUS_DEVICE_BUSY          ; if ww irp is already pending
STATUS_UNSUCCESSFUL         ; if query power for S1,S2,S3 and these map to D3
                              (system doesn't support wake from USB)
STATUS_CANCELLED            ; if hub is removed
STATUS_POWER_STATE_INVALID  ; if IRP_MN_SET_POWER(D3)




.....................................................................................................
        WinNT6
possible error codes on ww irp complete
STATUS_NOT_SUPPORTED        ; if device (hardware) does not support remote wakeup
STATUS_DEVICE_BUSY          ; if ww irp is already pending
STATUS_POWER_STATE_INVALID  ; if IRP_MN_SET_POWER(D3)
STATUS_CANCELLED            ; if device is removed or surprise_removed






*****************************************************************************************************/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "pdopwr.tmh"
#endif

CCHAR usbfile_pdopwr_c[] = "pdopwr.c";

#define USBFILE usbfile_pdopwr_c

#define pdopwr_c_tag      __LINE__


//
// PDO Power Handlers
//

const HUB_POWER_DISPATCH HubPdoPowerDispatch[HUB_POWER_MAX_MN] = {
    IRP_MN_WAIT_WAKE,                   UsbhPdoPower_WaitWake,
    IRP_MN_POWER_SEQUENCE,              UsbhPdoPower_Sequence,
    IRP_MN_SET_POWER,                   UsbhPdoPower_SetPower,
    IRP_MN_QUERY_POWER,                 UsbhPdoPower_QueryPower
};


VOID
UsbhStartThisPowerIrp_Pdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    ULONG Tag
    )
/*++


Routine Description:

    References the power irp held by the PDO so we can track it. This function
    is useful for debugging power deadlocks.

Arguments:

    Irp - Power Irp to attach to the hubs device extension

Return Value:

    none -- always succeeds

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION ioStack;
    ULONG idx;

    pdoExt = PdoExt(Pdo);

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

    LOG(HubFdo, LOG_POWER, 'PDN3', ioStack, Irp);

    UsbhAssert(HubFdo, pdoExt->CurrentPowerIrp[idx] == NULL);
    // we are supposed to call this on the current stack location
    UsbhAssert(HubFdo, pdoExt->CurrentPowerIrpStack[idx] == NULL);

    // ref
    pdoExt->CurrentPowerIrp[idx] = Irp;
    pdoExt->CurrentPowerIrpStack[idx] = ioStack;
    pdoExt->CurrentPowerIrpTag[idx] = Tag;
}


VOID
UsbhPoStartNextPowerIrp_Pdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    ULONG Tag
    )
/*++


Routine Description:

    Removes a reference to the power irp held by the PDO and calls
    PoStartNextPowerIrp.

Arguments:

    Irp - Power Irp to attach to the hubs ref list

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION ioStack;
    ULONG idx;

    pdoExt = PdoExt(Pdo);

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

    LOG(HubFdo, LOG_POWER, 'PDN1', pdoExt->CurrentPowerIrp[idx], Irp);
    LOG(HubFdo, LOG_POWER, 'PDN2', pdoExt->CurrentPowerIrpStack[idx], ioStack);

    UsbhAssert(HubFdo, pdoExt->CurrentPowerIrp[idx] == Irp);
    // we are supposed to call this on the current stack location
    UsbhAssert(HubFdo, pdoExt->CurrentPowerIrpStack[idx] == ioStack);

    // deref
    pdoExt->CurrentPowerIrp[idx] = NULL;
    pdoExt->CurrentPowerIrpStack[idx] = NULL;
    pdoExt->CurrentPowerIrpTag[idx] = Tag;

    // release the lock
    PoStartNextPowerIrp(Irp);

}


VOID
UsbhSetPdoPowerState(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    ULONG Data,
    HUB_PDO_POWER_STATE NewPdoPowerState,
    HUB_PDO_POWER_EVENT PdoPowerEvent
    )
/*++


Routine Description:

    Sets the current power state for the hub generated Pdo.

Arguments:

    Sc - thread context this action is taking place in.

    Pdo - Pdo whose state is changing

    Data - optional ULONG data element to include in the log.

    NewPdoPowerState - state we are transitioning in to.

    PdoPowerEvent - event causing transition.

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    PG_STATE_LOG_ENTRY pwrLog;

    pdoExt = PdoExt(Pdo);

    UsbhAssertFdoPwrContextLock(Sc);

    pdoExt->PwrLogIdx=(pdoExt->PwrLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    pwrLog = &pdoExt->PwrLog[pdoExt->PwrLogIdx];

    pwrLog->Event = PdoPowerEvent;
    pwrLog->CurrentState = pdoExt->CurrentPdoPowerState;
    pwrLog->NextState = NewPdoPowerState;

    pdoExt->CurrentPdoPowerState = NewPdoPowerState;

}


NTSTATUS
UsbhPdoPowerWorkerInternal(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch worker routine for Power Irps to Pdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG idx;
    UCHAR pow_func, pow_subfunc;
    HUB_PDO_HWPNP_STATE curState;

    curState = GET_PDO_HWPNPSTATE(Pdo);

    if (curState == Pdo_Deleted) {
        Irp->IoStatus.Status = STATUS_DEVICE_REMOVED;
        IoCompleteRequest(Irp,IO_NO_INCREMENT);
        return STATUS_DEVICE_REMOVED;
    }

    pdoExt = PdoExt(Pdo);

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

    idx = InterlockedIncrement((PLONG)&pdoExt->PowerFuncHistoryIdx);
    idx = idx % HUB_PP_HISTORY_LEN;
    pdoExt->PowerFuncHistory[idx] = pow_func;
    pdoExt->PowerSubFuncHistory[idx] = pow_subfunc;
    pdoExt->CurrentPowerFunc = (ULONG) pow_func;

    if (ioStack->MinorFunction <= HUB_POWER_MAX_MN) {
        nts = HubPdoPowerDispatch[ioStack->MinorFunction].PowerHandler(
                Pdo,
                Irp);
    } else {
        // this is no indication that PoStartNextPowerIrp should be
        // called for anything but SetPower and QueryPower
        nts = UsbhPdoPower_DoDefault(Pdo, Irp, FALSE);
    }

    return nts;
}


VOID
UsbhPdoPowerWorker(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch worker routine for Power Irps to Pdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    UsbhPdoPowerWorkerInternal(Pdo, Irp);
}


NTSTATUS
UsbhPdoPower(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch routine for Power Irps to Pdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO  pdoExt;

#ifdef USB_FORCE_POWER_AT_DISPATCH
    KIRQL irql;
    BOOLEAN irqlRaised = FALSE;
    if (!(Pdo->Flags & DO_POWER_PAGABLE)) {
        KeRaiseIrql(DISPATCH_LEVEL, &irql);
        irqlRaised = TRUE;
    }
#endif 

    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
        nts = UsbhPdoPowerWorkerInternal(Pdo, Irp);
    } else {
        pdoExt = PdoExt(Pdo);

        nts = Usbh_HubDeferIrpProcessing(pdoExt->HubFdo,
                                         Pdo, 
                                         UsbhPdoPowerWorker, 
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


NTSTATUS
UsbhPdoPower_DoDefault(
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    BOOLEAN CallPoStartNextPowerIrp
    )
/*++

Routine Description:

    Default handler for Power irp mn functions to PDO, completes irp
    with status unchanged after calling PoStartNextPowerIrp

Arguments:

Irql:

Return Value:

    nt status

--*/
{
    NTSTATUS nts;

    nts = Irp->IoStatus.Status;

    if (CallPoStartNextPowerIrp) {
        UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);
    }
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return nts;
}


BOOLEAN
UsbhPdoArmedForWake(
    PDEVICE_OBJECT Pdo
    )
/*++
Routine Description:

   Checks the armed status of a PDO.  A pdo is armed for wake if a
   IRP_MN_WAIT_WAKE irp has been submitted by the client driver.
   The device itself may not actually be armed yet.

   The PDO is armed as long as we are processing a wake irp somewhere in our
   driver -- WakeIrpPending > -1.

Arguments:

Irql: ANY

Return Value:

    TRUE if the PDO is armed for wake
--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    return pdoExt->WakeIrpPending > -1 ? TRUE : FALSE;

}


NTSTATUS
UsbhPdoPower_Sequence(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Completes a PDO power irp with STATUS_SUCESS

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    NTSTATUS nts;

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'sPOW', 0);
    if (NT_ERROR(nts)) {
        UsbhCompleteIrp(Irp, nts);
        return nts;
    }

    nts = STATUS_SUCCESS;

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


NTSTATUS
UsbhPdoPower_QueryPower(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Completes a PDO power irp with STATUS_SUCESS

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);

    UsbhStartThisPowerIrp_Pdo(pdoExt->HubFdo, Pdo, Irp, pdopwr_c_tag);

    switch (ioStack->Parameters.Power.Type) {
    case SystemPowerState:
        DbgTrace((UsbhDebugTrace, "Port %d PDO Query.SystemState %!SYSPWR!\n",
            pdoExt->PortNumber,(LONG)ioStack->Parameters.Power.State.SystemState));
        break;
    case DevicePowerState:
        DbgTrace((UsbhDebugTrace, "Port %d PDO Query.DeviceState %!DEVPWR! \n",
            pdoExt->PortNumber,(LONG)ioStack->Parameters.Power.State.DeviceState));
        break;
    }

    UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);
    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhPdoPower_SetPower(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Handles IRP_MN_SET_POWER for a HUB PDO.

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pdoExt = PdoExt(Pdo);

    UsbhStartThisPowerIrp_Pdo(pdoExt->HubFdo, Pdo, Irp, pdopwr_c_tag);

    UsbhAssert(NULL, pdoExt->HubFdo != NULL);
    hubExt = FdoExt(pdoExt->HubFdo);
    LOG(pdoExt->HubFdo, LOG_POWER, 'pdSP', Pdo, Irp);

    switch (ioStack->Parameters.Power.Type) {
    case SystemPowerState:
        return UsbhPdoSystemPowerState(pdoExt->HubFdo, Pdo, Irp);
    case DevicePowerState:
        return UsbhPdoDevicePowerState(pdoExt->HubFdo, Pdo, Irp);
    }

    nts = STATUS_NOT_SUPPORTED;

    UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoSetD0_Finish(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

    Completes the deferred D0 request and transitions to the appropriate state.

Arguments:

Irql: PASSIVE

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    USBD_STATUS usbdStatus;
    HUB_PDO_POWER_STATE pdoPowerState;
    NTSTATUS nts;

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(Sc->HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    deviceState = ioStack->Parameters.Power.State.DeviceState;

    UsbhAcquireFdoPwrLock(Sc->HubFdo, Sc, Ev_PDO_POWER, PdoSetD0_Finish_Tag);
    pdoPowerState = GET_PDO_POWER_STATE(Pdo);

    // we are still in the context of the devices 'current d-irp' so it
    // the pdo state should be Pdo_WaitD0 or Pdo WaitDx

    // The hub is in D0 now so we can talk to the device, however, the
    // device may not be physically present anymore.

    // verify the hubs state -- it should be D0
    UsbhAssert(Sc->HubFdo, GET_FDO_DX(Sc->HubFdo) == PowerDeviceD0);

    UsbhSetPdoPowerState( Sc,  Pdo, 0, Pdo_D0, PdoPwrEv_Hub_D0);
    UsbhAssert(Sc->HubFdo, deviceState == PowerDeviceD0);

    LOG(Sc->HubFdo, LOG_POWER, 'pWKc', Sc, Pdo);

    // set pdo to same vbus as the parent hub lost power on resume
    pdoExt->VbusState = hubExt->HubVbusState;
    
    UsbhReleaseFdoPwrLock(Sc->HubFdo, Sc);

    // disable remote wake on the device if it was enabled.
    // This behavior is identical to the original hub driver.
    if (pdoExt->PdoFlags.UsbWakeupSupported) {
        nts = UsbhDisableDeviceForWake(Sc->HubFdo, Pdo, &usbdStatus);

        if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
            // failure here could lead to some strange behavior of the device
            // so we make note of this

            HUB_EXCEPTION(USBFILE,  Sc->HubFdo, pdoExt->PortNumber,
                          PowerErr_FailedWakeDisable,
                          nts, usbdStatus, NULL, 0);
        }
    }

    DbgTrace((UsbhDebugTrace, "Port %d PDO Power.DeviceState D0\n",
                           pdoExt->PortNumber));

    UsbhSet_Pdo_Dx(Pdo, deviceState);

    DbgTrace((UsbhDebugTrace, ">CPT Port %d PDO Power.DeviceState D0\n", pdoExt->PortNumber));

    return STATUS_SUCCESS;

}



VOID
UsbhPdoWaitForD3Reconnect(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++
Routine Description:


Arguments:

Irql: PASSIVE

Return Value:

     VOID

 --*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    NTSTATUS status;
    BOOLEAN deviceWasDisconnected;
    USBD_STATUS usbdStatus;
    USB_HUB_PORT_STATE ps;

    deviceWasDisconnected = FALSE;
    status = STATUS_SUCCESS;

    pdoExt = PdoExt(Pdo);

    do {
        if (pdoExt->PdoFlags.D3ColdWaitForReconnect == 1) {
            //
            // Wait for the D3Cold device to reconnect.  Timeout after default of 1 second or other value 
            // specified in registry.
            //
            deviceWasDisconnected = TRUE;
            status = UsbhWaitEventWithTimeout(HubFdo, 
                                              &pdoExt->D3ColdReconnectEvent,
                                              pdoExt->D3ColdReconnectTimeout,
                                              'dc3D');
        }

        //
        // Port change queue will stop consuming connect changes once we clear InD3Cold
        //
        WAIT_EVENT(&pdoExt->D3ColdLock);
        pdoExt->PdoFlags.InD3Cold = 0;
        SET_EVENT(&pdoExt->D3ColdLock);
    
        LOG(HubFdo, LOG_POWER, 'D3Cr', status, pdoExt->PortNumber);
                
        //
        // Check device state in case connect change was lost before exiting D3Cold
        //
        if (deviceWasDisconnected == FALSE) {
            status = UsbhQueryPortState(HubFdo,
                                        pdoExt->PortNumber,
                                        &ps,
                                        &usbdStatus);

            if (NT_SUCCESS(status)) {
                
                if ((ps.Status.Enable == 0) && 
                    (ps.Status.Connect == 1)) {
                    LOG(HubFdo, LOG_POWER, 'D3Cl', ps.Status.us, pdoExt->PortNumber);
                    deviceWasDisconnected = TRUE;
                
                } else if (ps.Status.Connect == 0) {
                    LOG(HubFdo, LOG_POWER, 'D3Cg', ps.Status.us, pdoExt->PortNumber);
                    status = STATUS_UNSUCCESSFUL;
                }
            }
        }
    
        if (NT_SUCCESS(status) && (deviceWasDisconnected == TRUE)) {

            //
            // D3Cold device was disconnected.  Wait for 100ms debounce interval
            //
            UsbhWait(HubFdo, 100);

            //
            // Make sure the device is still connected.  
            //
            status = UsbhQueryPortState(HubFdo,
                                        pdoExt->PortNumber,
                                        &ps,
                                        &usbdStatus);

            LOG(HubFdo, LOG_POWER, 'D3Cs', status, ps.Status.us);

            if (NT_SUCCESS(status) && (ps.Status.Connect == 1)) {
                    //
                    // Reset the D3Cold device
                    //
                    status = UsbhSyncResetDeviceInternal(HubFdo, PDO_CONTEXT(Pdo), Pdo);
            } else {
                status = STATUS_UNSUCCESSFUL;
            }
        } else if (NT_SUCCESS(status) && (deviceWasDisconnected == FALSE)) {
            UsbhSyncResumeDeviceInternal(HubFdo, PDO_CONTEXT(Pdo), Pdo);
        }

    } WHILE(0);

    if (!NT_SUCCESS(status)) {
        //
        // D3Cold device never re-connected or failed to re-enumerate. Cycle port to insure the device is either
        // re-enumerated or removed.
        //
        UsbhQueueSoftConnectChange(HubFdo, pdoExt->PortNumber, PDO_CONTEXT(Pdo), FALSE);
    }

    UsbhSet_Pdo_Dx(Pdo, PowerDeviceD0);
}



VOID
UsbhPdoSetD0(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PHUB_POWER_CONTEXT PowerCtx
    )
/*++
Routine Description:

    Deferred request.
    Sets a PDO to the D0 state, this routine runs in the context of
    a worker thread.

    Since all the work necessary to resume or restore the device is done by the
    hub there is nothing to do in the case of D0 other than to wait.

Arguments:

Irql: PASSIVE

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    USBD_STATUS usbdStatus;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;
    HUB_PDO_POWER_STATE pdoPowerState;
    PDEVICE_OBJECT pdo;
    KIRQL oldIrql;

    UsbhAssert(HubFdo, PowerCtx->Sig == SIG_POWER_CTX);
    pdo = PowerCtx->DevObject;
    pdoExt = PdoExt(pdo);
    hubExt = FdoExt(pdoExt->HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(PowerCtx->DeviceIrp);
    deviceState = ioStack->Parameters.Power.State.DeviceState;

    UsbhAssert(HubFdo, deviceState == PowerDeviceD0);
    UsbhAssertPassive();
    LOG(HubFdo, LOG_POWER, 'PDD0', PowerCtx, pdo);

    /*
    Client drivers may request D0 at any time. This leads a number of cases depending on
    the hubs own power state. Some of these cases must be considered invalid.


                    Case 1 | Case 2 | Case 3 | Case 4
    -----------------------+--------+--------+--------
    Hub S state     S0     | S0     | not S0 | not S0
    Hub D state     D0     | not D0 | D0     | not D0


    Case 1:
    Hub in running state
    This is a nop - we complete the request with success immediately.

    Case 2:
    Hub in S0 not in D0 - powering up.
    The hub is transitioning to D0 as part of power up from S1 or S3, we wait
    for the hub to reach D0 before completing the request.

    Case 3,4:
    Hub not in S0 (D0 or not in D0) - hub is powering down
    The hub is in or will transition to D2 or D3 because the system is entering
    S1 or S3.  Client drivers should have received their S irps and should not
    be putting themselves in D0 at this time. This would be a bug in the client
    driver. Best thing we can do here to mange this case is to fail the power
    irp request.



    */

    sc = PDO_CONTEXT(pdo);

    // hub needs to wake and remain out of selective suspend until PDO is back in D0.
    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoSetD0ResumeRequested_Tag);
    UsbhSetPdoPowerState( sc,  pdo, 0, Pdo_Dx_ResumeRequested, PdoPwrEv_Hub_D0);
    UsbhReleaseFdoPwrLock(HubFdo, sc);

    PowerCtx->Bh = UsbhIncHubBusy(HubFdo, sc, pdo, UsbhBusyPdoSetD0_Tag, RESUME_HUB);

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoSetD0_Tag);
    pdoPowerState = GET_PDO_POWER_STATE(PowerCtx->DevObject);

    switch (GET_FDO_POWER_STATE(sc)) {
    case Fdo_PowerFailureDisconnected:

        // if we are disconnected then it follows that our child is disconnected.
        // WDM permits us to return an error in this case.
        nts = NT_DEVICE_DISCONNECTED_STATUS;
        DbgTrace((UsbhDebugTrace, ">CPT w/ ERROR(DISCONNECTED) Port %d PDO Power.DeviceState D0 %!STATUS!\n",
                        pdoExt->PortNumber, nts));
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        break;

    case FdoWaitSxIoComplete_D0:
    case FdoWaitSx_WaitDx:
    case FdoWaitSx_WaitDxPoComplete:
    case FdoSx_Dx:
    case Fdo_PowerFailure:

        UsbhSetPdoPowerState( sc,  pdo, 0, pdoPowerState, PdoPwrEv_Hub_NOT_D0);
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        // case 3
        // treat this as an invalid request because we are powering down or have finished
        // powering down

        nts = STATUS_POWER_STATE_INVALID;
        DbgTrace((UsbhDebugTrace, ">CPT w/ ERROR Port %d PDO Power.DeviceState D0 %!STATUS!\n",
                        pdoExt->PortNumber, nts));
        
        break;

    case FdoS0_D0:
    case FdoSx_D0:
        // case 1

        // These checks are necessary if the client driver issues multple SetD0 requests 
        switch (pdoPowerState) {
        case Pdo_D0_ResumePending:
            
            // already resuming from another worker thread
            UsbhSetPdoPowerState( sc,  pdo, 0, Pdo_D0_ResumePending, PdoPwrEv_Double_D0);
            UsbhReleaseFdoPwrLock(HubFdo, sc);

            // block this thread
            UsbhKdPrint((1, "<PDO %p double D0)(2)>\n", pdo));
            LOG(HubFdo, LOG_POWER, 'w2D0', 0, pdoExt->PortNumber);
            WAIT_EVENT(&pdoExt->DeviceD0Event);
            break;
            
        case Pdo_D0:
            UsbhSetPdoPowerState( sc,  pdo, 0, Pdo_D0, PdoPwrEv_Double_D0);
            UsbhReleaseFdoPwrLock(HubFdo, sc);

            // block this thread
            UsbhKdPrint((1, "<PDO %p double D0)(3)>\n", pdo));
            LOG(HubFdo, LOG_POWER, 'w3D0', 0, pdoExt->PortNumber);
            break;

        default:                    
        
            UsbhSetPdoPowerState( sc,  pdo, 0, Pdo_D0_ResumePending, PdoPwrEv_Hub_D0);
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            // device wants to go to D0
            UsbhAssert(HubFdo, (GET_PDO_DX(pdo) != PowerDeviceD0) ||
                               (GET_PDO_HWPNPSTATE(pdo) != Pdo_PnpRefHwPresent)); 

            //
            // Check if a D3Cold device was disconnected
            //
            if (pdoExt->PdoFlags.InD3Cold == 1) {
                UsbhPdoWaitForD3Reconnect(HubFdo, pdo);
            } else {
                UsbhSyncResumeDeviceInternal(HubFdo, sc, pdo);

                DbgTrace((UsbhDebugTrace, "(Hub in S0) Port %d PDO Power.DeviceState D0\n",
                                       pdoExt->PortNumber));

                UsbhSet_Pdo_Dx(pdo, PowerDeviceD0);
            }

            // pdo is now in D0
            UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoSetD0_Tag);
            pdoPowerState = GET_PDO_POWER_STATE(PowerCtx->DevObject);
            
            // PDO should either be pending resume, or may already be resumed if it has been soft
            // removed concurrently with the set D0 request
            UsbhAssert(HubFdo, (pdoPowerState == Pdo_D0_ResumePending) || (pdoPowerState == Pdo_D0) );

            // PDO should either be pending resume, or may already be resumed if it has been soft
            // removed concurrently with the set D0 request
            UsbhAssert(HubFdo, (pdoPowerState == Pdo_D0_ResumePending) || (pdoPowerState == Pdo_D0) );

            // sync vbus state to that of hub so the the device can recover
            pdoExt->VbusState = hubExt->HubVbusState;
            
            UsbhSetPdoPowerState( sc,  pdo, 0, Pdo_D0, PdoPwrEv_Hub_D0);
            
            SET_EVENT(&pdoExt->DeviceD0Event);
            UsbhReleaseFdoPwrLock(HubFdo, sc);

            // complete idle irps with STATUS_SUCCESS
            UsbhCompletePdoIdleIrp(HubFdo, pdo, STATUS_SUCCESS);

            // implied clear idle on return to D0
            UsbhClearPdoIdleReady(HubFdo, pdo, PowerCtx);
            
            // disable remote wake on the device if it was enabled.
            // This behavior is identical to the original hub driver.
            if (pdoExt->PdoFlags.UsbWakeupSupported) {
                nts = UsbhDisableDeviceForWake(sc->HubFdo, pdo, &usbdStatus);

                if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
                    // failure here could lead to some strange behavior of the device
                    // so we make note of this

                    HUB_EXCEPTION(USBFILE,  sc->HubFdo, pdoExt->PortNumber,
                                  PowerErr_FailedWakeDisable,
                                  nts, usbdStatus, NULL, 0);
                }
            }
        }    
        nts = STATUS_SUCCESS;
        break;

    case Fdo_WaitD0:
    case Fdo_WaitD0Worker:
    case Fdo_WaitD0PoComplete:

        UsbhSetPdoPowerState(sc,  pdo, 0, Pdo_WaitD0, PdoPwrEv_SET_D0_wWait);
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        // case 2
        // this is where we wait for the hub to finish power up
        LOG(HubFdo, LOG_POWER, 'PuD0', PowerCtx, pdo);
        
        // implied clear idle on return to D0
        UsbhClearPdoIdleReady(HubFdo, pdo, PowerCtx);


        // we cannot complete the D0 request until the parent hub has
        // reached D0.
        // We are likely in the context of the system power Irp here -- that
        // is the client may be holding this irp until we power up.
        LOG(HubFdo, LOG_POWER, 'whD0', 0, PortNumber);
        
        
        //
        // If the PendingD0 list is enabled by the FDO, it indicates that 
        // the FDO still hasn't come back to D0 - So just add ourself
        // to the list and return. 
        // When the FDO is ready it will queue a  worker routine 
        // for us to do the rest of the processing.
        //
        
        KeAcquireSpinLock(&hubExt->LockPdoD0List, &oldIrql );

        if ( hubExt->PendingPdoD0ListEnabled ) {         

            /*
            * Add our context to a list.
            */                                      

            InsertTailList( &hubExt->PendingPdoD0List,
                            &PowerCtx->PendingD0ListEntry );
            KeReleaseSpinLock(&hubExt->LockPdoD0List, oldIrql );

            return;
        }

        KeReleaseSpinLock(&hubExt->LockPdoD0List, oldIrql );

        
        UsbhPdoSetD0_Finish(sc, pdo, PowerCtx->DeviceIrp);
        nts = STATUS_SUCCESS;
        break;

    default:
        UsbhSetPdoPowerState( sc,  pdo, 0, pdoPowerState, PdoPwrEv_SET_D0_Error);
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        // we missed something
        UsbhAssertFailure(HubFdo, PowerCtx);
        nts = STATUS_POWER_STATE_INVALID;
        break;
    }

    KeQuerySystemTime(&pdoExt->Stat_Pdo_SetD0_CompleteAt);

    UsbhDecHubBusy(HubFdo, sc, PowerCtx->Bh);    

    PowerCtx->Bh = NULL;

    UsbhEtwLogDevicePowerEvent(pdoExt, PowerCtx->DeviceIrp, &USBHUB_ETW_EVENT_DEVICE_POWER_SET_D0_COMPLETE, ioStack->Parameters.Power.State, nts);
    UsbhCompleteIrp(PowerCtx->DeviceIrp, nts);
    UsbhDecPdoIoCount(pdo, PowerCtx->DeviceIrp);

    UsbhReleasePowerContext(HubFdo, PowerCtx);
}



__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhPdoUnblockPendedD0IrpWI(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    )
/*++

Routine Description:

    This is the worker item routine that is queued by 
    the FDO when it comes back to D0 so that the PDO
    can complete the processing of its own D0 IRP.


Irql: PASSIVE

Arguments:

    Context is Hub Power context

Return Value:

    none

--*/
{
    
    PHUB_POWER_CONTEXT powerCtx = Context;
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;

    LOG(HubFdo, LOG_POWER, 'upD0', powerCtx, PortNumber);

    UsbhAssert(HubFdo, powerCtx->Sig == SIG_POWER_CTX);

    pdo = powerCtx->DevObject; 
    pdoExt = PdoExt(pdo);                     
    ioStack = IoGetCurrentIrpStackLocation(powerCtx->DeviceIrp);


    UsbhPdoSetD0_Finish(powerCtx->Sc, pdo , powerCtx->DeviceIrp);
    
    KeQuerySystemTime(&pdoExt->Stat_Pdo_SetD0_CompleteAt);

    UsbhDecHubBusy(HubFdo, powerCtx->Sc, powerCtx->Bh);    

    powerCtx->Bh = NULL;

    UsbhEtwLogDevicePowerEvent(pdoExt, powerCtx->DeviceIrp, &USBHUB_ETW_EVENT_DEVICE_POWER_SET_D0_COMPLETE, 
                               ioStack->Parameters.Power.State, STATUS_SUCCESS);

    UsbhCompleteIrp(powerCtx->DeviceIrp, STATUS_SUCCESS);

    UsbhDecPdoIoCount(pdo, powerCtx->DeviceIrp);

    UsbhReleasePowerContext(HubFdo, powerCtx);
    
}


NTSTATUS
UsbhSyncSuspendPdoPort(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    BOOLEAN FlushWake
    )
/*++

Routine Description:

    Given a port with an attached PDO, suspend the associated USB device.

    This is a synchronous function that runs at passive level.

Irql: PASSIVE

Arguments:

    Pdo - a latched PDO

    FlushWake - Arm the device for remote wake if WW IRP is pending

Return Value:

    This function returns an error if the device currently attached is
    different from the one associated with the PDO or if we cannot suspend the
    device.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts = STATUS_SUCCESS, qp_nts;
    USBD_STATUS usbdStatus;
    USB_HUB_PORT_STATE ps;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    DbgTrace((UsbhDebugTrace, "SyncSuspend USB device (PDO) %p\n", Pdo));

    pdoExt = PdoExt(Pdo);

    UsbhAssert(Sc->HubFdo, pdoExt->PdoFlags.EnumerationFailed == 0);

    // check the current port status to see if the device is still present
    // this is an optimization since the device could still be disconnected after
    // check.

    qp_nts = UsbhQueryPortState(Sc->HubFdo,
                               pdoExt->PortNumber,
                               &ps,
                               &usbdStatus);

    if (NT_SUCCESS(qp_nts) &&
        ps.Status.Connect) {

        // Historical Note:
        // Wake support must be enabled dynamically, we cannot do it when
        // the ww irp is submitted because of USB core spec requirements.
        // The core spec requires us to not allow a device to generate
        // resume signalling while in reset -- the only way to guarantee this
        // is to only arm the device just before port suspend.
        if (pdoExt->PdoFlags.UsbWakeupSupported &&
            UsbhPdoArmedForWake(Pdo) &&
            !FlushWake && 
            !pdoExt->PdoFlags.DeviceIsHub) {

            nts = UsbhEnableDeviceForWake(Sc->HubFdo, Pdo, &usbdStatus);

            if (NT_ERROR(nts)) {
                // failure here could lead to some strange behavior of the
                // device so we make note of this

                HUB_EXCEPTION(USBFILE,  Sc->HubFdo, pdoExt->PortNumber,
                              PowerErr_FailedWakeEnable,
                              nts, usbdStatus, NULL, 0);
            }
        }

        Usbh__TestPoint__Ulong(Sc->HubFdo, TstPt_SelctiveSuspend, nts, pdoExt->PortNumber);

        nts = UsbhPCE_Suspend(Sc->HubFdo, Sc, pdoExt->PortNumber);

        Usbh__TestPoint__Ulong(Sc->HubFdo, TstPt_SelctiveSuspendComplete, nts, pdoExt->PortNumber);

    } else {
        // device not connected (or we are not connected) in either case
        // we skip the suspend operation

        HUB_EXCEPTION(USBFILE,  Sc->HubFdo, pdoExt->PortNumber,
                              PowerErr_DeviceDisconnectOnSuspend,
                              qp_nts, usbdStatus, NULL, 0);

        // return an error so ssh can abort
        nts = STATUS_UNSUCCESSFUL;
    }

    return nts;
}


NTSTATUS
UsbhPdoSetDx(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

    Places a PDO in a low power state at the request of the function driver.

    This can happen at any time and may or may not be in response to a system suspend.

    FdoPowerLock is acquired by caller and released here

Arguments:

Irql: PASSIVE

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_OBJECT hubFdo;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    SYSTEM_POWER_STATE effectiveSystemState;
    NTSTATUS nts;
    HUB_PDO_POWER_STATE pdoPowerState;
    BOOLEAN flushwake;

    pdoExt = PdoExt(Pdo);
    UsbhAssert(NULL, Sc == PDO_CONTEXT(Pdo));
    hubFdo = Sc->HubFdo;
    hubExt = FdoExt(hubFdo);
    flushwake = FALSE;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    deviceState = ioStack->Parameters.Power.State.DeviceState;
    effectiveSystemState = ioStack->Parameters.Power.SystemPowerStateContext.EffectiveSystemState;

    UsbhAssert(hubFdo, deviceState != PowerDeviceD0);
    UsbhAssertFdoPwrContextLock(Sc);

    LOG(hubFdo, LOG_POWER, 'PDDx', Irp, Pdo);

    pdoExt->PdoFlags.ResetOnLastResume = 0;


    /*
    Client drivers may request Dx at any time. This leads a number of cases depending on
    the hubs own power state. Some of these cases must be considered invalid.


                    Case 1 | Case 2 | Case 3 | Case 4
    -----------------------+--------+--------+--------
    Hub S state     S0     | S0     | not S0 | not S0
    Hub D state     D0     | not D0 | D0     | not D0


    Case 1:
            hub is running state
            STATUS_POWER_STATE_INVALID ; device is not powered up or hw failure

    Case 2:
            hub is powering up.

    Case 3:
            hub is powering down.
            STATUS_POWER_STATE_INVALID

    Case 4:
            hub is already powered down.
            STATUS_POWER_STATE_INVALID

    */

    switch (GET_FDO_POWER_STATE(Sc)) {
    case FdoSx_Dx:
        // case 4 - hub is powered down

    case FdoWaitSxIoComplete_D0:
    case FdoWaitSx_WaitDx:
    case FdoWaitSx_WaitDxPoComplete:
    case FdoSx_D0:
    case Fdo_PowerFailure:

        // case 3 - hub is powering down
        // treat this as an invalid request because we are powering down or have finished
        // powering down
        nts = STATUS_POWER_STATE_INVALID;

        DbgTrace((UsbhDebugTrace, "#%d UsbhPdoSetDx  - hub is powering down\n",
            pdoExt->PortNumber));

        UsbhSetPdoPowerState( Sc,  Pdo, 0, Pdo_Dx, PdoPwrEv_SET_DX_Error);
        UsbhReleaseFdoPwrLock(hubFdo, Sc);
        TEST_TRAP();
        break;

    case FdoS0_D0:

        // case 1, hub is running, OK to power down, check the pdo

        pdoPowerState = GET_PDO_POWER_STATE(Pdo);

        // take action based on the pdo state, I'm not sure how many of these are truly
        // legitimate but the represent all theoretical possibilities.

        switch(pdoPowerState) {
        case Pdo_D0:
            // pdo is in D0, set to D2 or D3 and suspend the port

            UsbhSetPdoPowerState( Sc,  Pdo, 0, Pdo_Dx, PdoPwrEv_SET_DX);
            // we must complete wake irps with errors if we enter a 'sleep state deeper that device wake'
            // for usb this is constant of PowerDeviceD3
            flushwake  = deviceState == PowerDeviceD3 ? TRUE : FALSE;
            UsbhReleaseFdoPwrLock(hubFdo, Sc);

            // Abort any outstanding transfer requests for the device
            // before putting the device into a low power state.
            //
            UsbhPdoAbortAllDevicePipes(hubFdo, Pdo);

            // attempt to suspend the port
            // queue a change to the bus engine requesting a suspend, since the bus engine is running
            // we don't want to access the port directly.

            // If the machine is being hibernated and the PDO is on the hibernate 
            // path we will need to keep the port powered on or the hibernate file
            // will not be generated.
            
            if (!pdoExt->HibernateCount || 
                effectiveSystemState != PowerSystemHibernate) {
                nts = UsbhSyncSuspendPdoPort(Sc, Pdo, flushwake);
            }            

            //jd validate port suspend state?

            //HUBEX

            //ignore any error here -- we will attempt to recover on resume.
            if (deviceState == PowerDeviceD3) {
                // for D3 the device should expect to be off even though we suspend the port so we don't treat this as 
                // an error.
                pdoExt->VbusState = Vbus_Off;
            } else {
                pdoExt->VbusState = Vbus_On;
            }                
            nts = STATUS_SUCCESS;

            // transitioning to Dx state (low power) implies idle_ready
            UsbhSetPdoIdleReady(hubFdo, Pdo, Irp);
            break;

        case Pdo_WaitD0Worker:
        case Pdo_WaitD0:
        case Pdo_PowerFailure:
            // pdo is powering down before it finishes powering up. This is probably
            // some kind of race condition in the function driver.
            // Since it is better to be in D0 than D2 we will just fail this D-state request.

            UsbhSetPdoPowerState( Sc,  Pdo, 0, pdoPowerState, PdoPwrEv_SET_DX_Error);
            flushwake = TRUE;
            UsbhReleaseFdoPwrLock(hubFdo, Sc);
            nts = STATUS_POWER_STATE_INVALID;
            break;

        case Pdo_Dx:
        case Pdo_WaitDx:
            // we might see this if the device wants to go from D2 to D3 or visa versa.
            // I don't think it is illegal, just update the target state, flush wakes
            // depending on new state
            // NOTE: Dvc300 and Dvc320 selectively suspend and will hit this path


            if (deviceState == PowerDeviceD3) {
                flushwake = TRUE;
                pdoExt->VbusState = Vbus_Off;
            } else {
                // device will not expect to lose power.  If it does this
                // value will be set to Vbus_Interrupted and the enable status will be
                // reported as clear if the function driver queries port status.
                pdoExt->VbusState = Vbus_On;
                flushwake = FALSE;
            }

            UsbhSetPdoPowerState( Sc,  Pdo, 0, pdoPowerState, PdoPwrEv_SET_DX);
            UsbhReleaseFdoPwrLock(hubFdo, Sc);

            // set ilde ready state
            UsbhSetPdoIdleReady(hubFdo, Pdo, Irp);
            nts = STATUS_SUCCESS;
            break;

        default:

            flushwake  = TRUE;
            UsbhReleaseFdoPwrLock(hubFdo, Sc);
            // did we miss something?
            TEST_TRAP();
            nts = STATUS_POWER_STATE_INVALID;
        }

        break;

    case Fdo_WaitD0:
    case Fdo_WaitD0Worker:
    case Fdo_WaitD0PoComplete:

        // case 2 hub is powering up

        // for some reason we get this request before the hub is powered up.
        // we set the pdo to a Pdo_WaitDx so we can suspend it when the hub has finished.
        UsbhSetPdoPowerState( Sc,  Pdo, 0, Pdo_WaitDx, PdoPwrEv_SET_DX);
        LOG(hubFdo, LOG_POWER, 'PuDx', Irp, Pdo);
        UsbhReleaseFdoPwrLock(hubFdo, Sc);

        TEST_TRAP();
        DbgTrace((UsbhDebugTrace, ">CPT Port %d PDO Power.DeviceState Dx [wait for hub]\n",
                            pdoExt->PortNumber));

        nts = STATUS_SUCCESS;
        break;

    default:
        UsbhReleaseFdoPwrLock(hubFdo, Sc);
        // we missed something
        UsbhAssertFailure(hubFdo, Irp);
        nts = STATUS_UNSUCCESSFUL;
        break;
    }


    // based on the new power state deal with the WAIT_WAKE irps.

    if (flushwake) {

        UsbhCompletePdoWakeIrp(hubFdo, pdoExt->Pdo, STATUS_POWER_STATE_INVALID);

        // by our definition D3 is OFF for USB -- we don't have D3 hot vs D3
        // cold.  If the PDO is enabled for wake this is illegal and a bug
        // in the client.
        //UsbhVerifierAssert(hubFdo, Pdo, USBvfe_WakeEnabledInD3);

        if (pdoExt->PdoFlags.AllowIdleIrpInD3 == 0) {
            UsbhCompletePdoIdleIrp(hubFdo, pdoExt->Pdo, STATUS_POWER_STATE_INVALID);
        }
    }

    // caller completes the irp

    return nts;
}



NTSTATUS
UsbhPdoSystemPowerState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

    Handles IRP_MN_SET_POWER, systempower for a HUB PDO.

    We don't take the io lock on PDOs that are soft removed since we need to
    handle these messages.

    Irp Args from wdm.h:

    struct {
           ULONG SystemContext;
           POWER_STATE_TYPE POINTER_ALIGNMENT Type;  0 = SystemPower, 1 = DevicePower
           POWER_STATE POINTER_ALIGNMENT State;     1 = PowerSystemWorking
           POWER_ACTION POINTER_ALIGNMENT ShutdownType;
   } Power;


Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    PIO_STACK_LOCATION ioStack;
    SYSTEM_POWER_STATE systemState;
    PSTATE_CONTEXT sc;
    HUB_PDO_POWER_STATE pdoPowerState;

    pdoExt = PdoExt(Pdo);

    sc = PDO_CONTEXT(Pdo);

    LOG(HubFdo, LOG_POWER, 'pdPs', Pdo, Irp);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    systemState = ioStack->Parameters.Power.State.SystemState;

    // if there is an SS idle request still pending complete it now with status
    // canceled.

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoSystemPowerState_Tag);
    pdoPowerState = GET_PDO_POWER_STATE(Pdo);
    // this doesn't change state but it records the event
    if (systemState == PowerSystemWorking) {
        UsbhSetPdoPowerState(sc,  Pdo, 0, pdoPowerState, PdoPwrEv_SET_S0);
    } else {
        UsbhSetPdoPowerState(sc,  Pdo, 0, pdoPowerState, PdoPwrEv_SET_SX);
    }

    pdoExt->Pdo_Sx = systemState;
    UsbhReleaseFdoPwrLock(HubFdo, sc);

    DbgTrace((UsbhDebugTrace, ">CPT Port %d PDO Power.SystemState %d\n",
                        pdoExt->PortNumber, systemState));

    UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);
    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhSet_D0_wQueued_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Handles IRP_MN_SET_POWER, devicepower for a HUB PDO.

   This function pends the request and schedules the actual SET_D0 action for
   the PDO on a worker thread.

   For D1-D3 it completes the action immediately.

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PHUB_POWER_CONTEXT pwrContext;

    // this routine queues a workitem to handle the irp
    // we must not access the irp if we are successful since it can complete
    // any time past this point.

    do {
        pwrContext = UsbhAcquirePowerContext(HubFdo,
                                             Sc,
                                             Pdo,
                                             NULL,
                                             Irp,
                                             Pdo_SetPower,
                                             FALSE);

        if (pwrContext == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break; //while
        }

        nts = UsbhFdoScheduleDeferredPowerRequest(HubFdo,
                                                  UsbhPdoSetD0,
                                                  pwrContext,
                                                  Irp);

        if (NT_ERROR(nts)) {
            // request could not be scheduled, complete the power irp
            // with an error
            UsbhReleasePowerContext(HubFdo, pwrContext);
            break; //while
        }

    } WHILE (0);

    return nts;
}


NTSTATUS
UsbhPdoDevicePowerState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Handles IRP_MN_SET_POWER, devicepower for a HUB PDO.

   This function pends the request and schedules the actual SET_D0 action for
   the PDO on a worker thread.

   For D1-D3 it completes the action immediately.

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    DEVICE_POWER_STATE deviceState;
    PSTATE_CONTEXT sc;
    HUB_PDO_POWER_STATE pdoPowerState;

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);

    sc = PDO_CONTEXT(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    deviceState = ioStack->Parameters.Power.State.DeviceState;

    LOG(HubFdo, LOG_POWER, 'pdPd', Pdo, Irp);
    // take the io lock while we process the D request, we should only get D
    // requests if an FDO is loaded for this PDO and we only want to handle
    // them if we are not removed.
    nts = UsbhIncPdoIoCount(Pdo, Irp, 'pPOW', 0);
    if (NT_ERROR(nts)) {
        if (pdoExt->IsBootDevice) {
            UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoDevicePowerState_Tag);

            switch (deviceState) {
                case PowerDeviceD0:
                    UsbhSetPdoPowerState( sc,  Pdo, 0, Pdo_D0, PdoPwrEv_SET_D0);
                    UsbhReleaseFdoPwrLock(HubFdo, sc);

                    UsbhClearPdoIdleReady(HubFdo, Pdo, Irp);
                    break;

                default:
                    UsbhSetPdoPowerState( sc,  Pdo, 0, Pdo_Dx, PdoPwrEv_SET_DX);
                    UsbhReleaseFdoPwrLock(HubFdo, sc);

                    UsbhSetPdoIdleReady(HubFdo, Pdo, Irp);
                    break;
            }
 
            UsbhSet_Pdo_Dx(Pdo, deviceState);
        }

        LOG(HubFdo, LOG_POWER, 'prf!', nts, Irp);

        // the rule of thumb here is to return status success for D requests even
        // if the device is gone  -- this permits drivers to get into D0 before
        // they have to hand;e the remove.  Since the hardware is physically gone
        // we don't need to do anything else here.

        nts = STATUS_SUCCESS;
        KeQuerySystemTime(&pdoExt->Stat_Pdo_SetD0_CompleteAt);
        
        UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);
        UsbhCompleteIrp(Irp, nts);
        return nts;
    }

    // check the state of the PDO, both PnP and Power states are relevant.
    UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_Internal);

    if (GET_PDO_HWPNPSTATE(Pdo) != Pdo_PnpRefHwPresent) {
        // pdo is removed or soon will be.
        // WDM allows us to fail the request in this special case
        TEST_TRAP();
        nts = NT_DEVICE_DISCONNECTED_STATUS;
        UsbhReleasePdoStateLock(HubFdo, sc);

        goto devpower_exit;
    }
    UsbhReleasePdoStateLock(HubFdo, sc);

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoDevicePowerState_Tag);
    pdoPowerState = GET_PDO_POWER_STATE(Pdo);

    nts = Irp->IoStatus.Status;

    switch(deviceState) {

    case PowerDeviceD0:

        UsbhEtwLogDevicePowerEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_POWER_SET_D0_DISPATCH, ioStack->Parameters.Power.State, STATUS_SUCCESS);

        KeQuerySystemTime(&pdoExt->Stat_Pdo_SetD0_StartAt);
         
        // PdoPwrEv_SET_D0
        switch (pdoPowerState) {
        case Pdo_D0:
            // already in D0.
            UsbhSetPdoPowerState(sc,  Pdo, 0, pdoPowerState, PdoPwrEv_SET_D0);
            
            UsbhReleaseFdoPwrLock(HubFdo, sc);

            nts = STATUS_SUCCESS;
            break;

        case Pdo_D0_ResumePending:
            // special case for the Double D0 irp:  
            // race conditions in the function driver could cause us to see two D0 irps in a row.  If this occurs
            // then we must block the second irp untilthe port is fully resumed and the device can be accessed.
            
            UsbhSetPdoPowerState(sc,  Pdo, 0, pdoPowerState, PdoPwrEv_Double_D0);
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            
            UsbhKdPrint((1, "<PDO %p double D0)>\n", Pdo));
            LOG(HubFdo, LOG_POWER, 'wdD0', 0, pdoExt->PortNumber);
            WAIT_EVENT(&pdoExt->DeviceD0Event);
            nts = STATUS_SUCCESS;
            break;
            
        default:
            // check the state of the hub
            switch (GET_FDO_POWER_STATE(sc)) {
                // hub is powering up
            case FdoS0_D0:
            case FdoSx_D0:
            case Fdo_WaitD0:
            case Fdo_WaitD0Worker:
            case Fdo_WaitD0PoComplete:

                nts = UsbhSet_D0_wQueued_Action(HubFdo, sc, Pdo, Irp);

                if (NT_SUCCESS(nts)) {
                    RESET_EVENT(&pdoExt->DeviceD0Event);
                    UsbhSetPdoPowerState(sc,  Pdo, 0, Pdo_WaitD0Worker, PdoPwrEv_SET_D0_wQueued);

                    UsbhReleaseFdoPwrLock(HubFdo, sc);

                    // pend the D irp here.

                    // note that we hold an io ref on the PDO which will keep
                    // the hub devobj around until we finish
                    return STATUS_PENDING;
                }
                // fall thru to error case if we cannot queue the worker

            case Fdo_PowerFailure:
            case Fdo_PowerFailureDisconnected:

                // hub has failed power so we cannot really power up.  The device will be removed soon
                // so we just complete with success so the power up threads can finish normally.
                // If the hub has been removed the function driver will see DEVICE_GONE or similar status.
                UsbhSetPdoPowerState(sc,  Pdo, 0, Pdo_PowerFailure, PdoPwrEv_SET_D0_Error);

                // we are allowed to return an error for the disconnected case
                if (GET_FDO_POWER_STATE(sc) == Fdo_PowerFailureDisconnected) {
                    nts = NT_DEVICE_DISCONNECTED_STATUS;
                } else {
                    // other errors are completed with success -- the function driver will handle the error after
                    // reaching D0
                    nts = STATUS_SUCCESS;
                }

                UsbhReleaseFdoPwrLock(HubFdo, sc);
                break;

            default:
                // hub is powering down but pdo wants to power up.

                //!ERROR_PATH_NOTE!
                //
                // Hub is powering down but client is powering up. Its not clear if we can really handle this case,
                // we complete  the D irp with success and mark the PDO such that we can deal with it on resume.

                // I seen this with the hid driver and failing the request seems to allow for recovery
                // so I'm leaving this as our error path.
                UsbhSetPdoPowerState(sc,  Pdo, 0, Pdo_PowerFailure, PdoPwrEv_SET_D0_Error);

                UsbhReleaseFdoPwrLock(HubFdo, sc);

                nts = STATUS_SUCCESS;
            }
        }
        break;

    case PowerDeviceD3:
    case PowerDeviceD2:
    case PowerDeviceD1:        
        UsbhEtwLogDevicePowerEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_POWER_SET_DX_DISPATCH, ioStack->Parameters.Power.State, STATUS_SUCCESS);

        if (GET_PDO_DX(Pdo) != deviceState) {
            if (pdoExt->DeviceHandleState == PdoDevH_Valid) {

                if (ioStack->Parameters.Power.ShutdownType == PowerActionNone) {
                    
                    if (!pdoExt->DeviceSelectiveSuspended) {
                
    		            pdoExt->DeviceSelectiveSuspended = 1;

                        // Update Registry that this device has tried to selective Suspend.				
            	        UsbhQueueWorkItem(HubFdo,
                                          DelayedWorkQueue,
                                          UsbhSetPdoSelectiveSuspendedKey,
                                          Pdo,
                                          0,
                                          SIG_SSH_WORKER);
                    }

                    if ((deviceState == PowerDeviceD3) &&
                        (PdoExt(Pdo)->PortAttributes.DeviceNotRemovable == 1) &&
                        (pdoExt->PdoFlags.D3ColdEnabled == 1) && 
                        (pdoExt->PdoFlags.D3ColdSupportedInAcpi == 1)) {
                        
                        SET_EVENT(&pdoExt->D3ColdReconnectEvent);
                        pdoExt->PdoFlags.D3ColdWaitForReconnect = 0;
                        pdoExt->PdoFlags.InD3Cold = 1;
                    }
                }                                               		  

                nts = UsbhPdoSetDx(sc, Pdo, Irp);

            } else {
                // PDO is gone, so fail the Dx request
                nts = STATUS_DEVICE_REMOVED;
                UsbhReleaseFdoPwrLock(HubFdo, sc);
            }
        } else {
            // if we are already in this state just complete with success
            nts = STATUS_SUCCESS;   
            
            UsbhReleaseFdoPwrLock(HubFdo, sc);
        }
        break;

    default:
        nts = STATUS_INVALID_PARAMETER;
        UsbhReleaseFdoPwrLock(HubFdo, sc);            
        UsbhAssertFailure(HubFdo, Irp);
    }

devpower_exit:

    if (Usb_Disconnected(nts)) {
        // disconnect this device so the client driver receives the correct
        // status codes on any transactions.
        UsbhPCE_BusDisconnect(HubFdo, sc, pdoExt->PortNumber);
    }

    DbgTrace((UsbhDebugTrace, ">CPT Port %d PDO Power.DeviceState %d\n",
                        pdoExt->PortNumber, deviceState));

    UsbhSet_Pdo_Dx(Pdo, ioStack->Parameters.Power.State.DeviceState);
    KeQuerySystemTime(&pdoExt->Stat_Pdo_SetD0_CompleteAt);
            
    UsbhPoStartNextPowerIrp_Pdo(PdoExt(Pdo)->HubFdo, Pdo, Irp, pdopwr_c_tag);

    UsbhEtwLogDevicePowerEvent(pdoExt,
                               Irp,
                               deviceState == PowerDeviceD0 ? 
                                   &USBHUB_ETW_EVENT_DEVICE_POWER_SET_D0_COMPLETE : 
                                   &USBHUB_ETW_EVENT_DEVICE_POWER_SET_DX_COMPLETE,
                               ioStack->Parameters.Power.State, 
                               STATUS_SUCCESS);

    UsbhCompleteIrp(Irp, nts);

    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


NTSTATUS
UsbhPdoPower_WaitWake(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:

   Handles IRP_MN_WAIT_WAKE for a HUB PDO.

Arguments:

Irql: ANY

Return Value:

     ntstatus

 --*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    LONG wakeIrpPending;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(pdoExt->HubFdo);

    UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_DISPATCH, 0);

    wakeIrpPending = InterlockedIncrement(&pdoExt->WakeIrpPending);

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'wwPD', 0);
    if (NT_ERROR(nts)) {
        UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_COMPLETE, nts);
        UsbhCompleteIrp(Irp, nts);
        InterlockedDecrement(&pdoExt->WakeIrpPending);
        return nts;
    }

    if (!pdoExt->PdoFlags.UsbWakeupSupported) {
        // this device does not support wake, therefore it cannot be armed
        // for wake
        nts = STATUS_NOT_SUPPORTED;

        //USBVERIFIER
        UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_COMPLETE, nts);
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);
        InterlockedDecrement(&pdoExt->WakeIrpPending);
        return nts;
    }

    // see if we already have one pending
    // non-zero means we do
    if (wakeIrpPending) {
        // according to DDK we are supposed to return this error if the wake
        // irp is already pending.
        nts = STATUS_DEVICE_BUSY;

        UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_COMPLETE, nts);
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);
        InterlockedDecrement(&pdoExt->WakeIrpPending);
        return nts;
    }

    // insert the wake irp in our 'cancel safe' queue, these services
    // automatically install a cancel function for us.

    IoMarkIrpPending(Irp);
    LOG(pdoExt->HubFdo, LOG_POWER, 'wwII', 0, Irp);
    IoCsqInsertIrp(&pdoExt->WakeIrpCsq, Irp, NULL);

    return STATUS_PENDING;
}


/*
    Use cancel kernel 'safe' queue to handle the wake request irp queueing and
    cancelation.
*/

BOOLEAN
UsbhCompletePdoWakeIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS NtStatus
    )
/*++
Description:

    Complete any pending wake irp we have as appropriate.

Arguments:

    Pdo - a latched pdo

    NtStatus - status code to complete the irp with.

Return:

    BOOLEAN true if a wake irp was completed for this PDO

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIRP wakeIrp;
    BOOLEAN irpCompleted = FALSE;

    pdoExt = PdoExt(Pdo);

    // dequeue the pending wake irp

    wakeIrp = IoCsqRemoveNextIrp(&pdoExt->WakeIrpCsq, NULL);

    if (wakeIrp) {
        // complete the wake irp with success
        DbgTrace((UsbhDebugTrace, "Port #%d Hub #%d PDO (%p) IRP(%p) **complete wake** 0x%08.8x\n",
            pdoExt->PortNumber, FdoExt(HubFdo)->HubNumber, Pdo, wakeIrp, NtStatus));
        UsbhKdPrint((1, "Port #%d Hub #%d PDO (%p) IRP(%p) **complete wake** 0x%08.8x\n",
            pdoExt->PortNumber, FdoExt(HubFdo)->HubNumber, Pdo, wakeIrp, NtStatus));

        LOG(HubFdo, LOG_POWER, 'wwCP', NtStatus, wakeIrp);

        // Propagate the system wake source information if the hub was marked
        // as a system wake source.

        if ((NT_SUCCESS(NtStatus)) &&
            (FdoExt(HubFdo)->FdoFlags.SystemWakeSource)) {

            PoSetSystemWake(wakeIrp);
        }

        UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_COMPLETE, NtStatus);
        UsbhCompleteIrp(wakeIrp, NtStatus);
        UsbhDecPdoIoCount(pdoExt->Pdo, wakeIrp);
        irpCompleted = TRUE;
    }

    // reset resume detection flags
    pdoExt->PdoFlags.ResumeSignal = 0;

    return irpCompleted;
}


VOID
UsbhInsertPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_INSERT_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    LOG(pdoExt->HubFdo, LOG_POWER, 'wwQ+', 0, Irp);
    // our queue only has one entry
    InsertTailList(&pdoExt->WakeIrpList,
                   &Irp->Tail.Overlay.ListEntry);
}


VOID
UsbhRemovePdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_REMOVE_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    LOG(pdoExt->HubFdo, LOG_POWER, 'wwQ-', 0, Irp);
    // our queue only has one entry
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);

    // allow another wake irp, at this point we are no longer 'armed'
    InterlockedDecrement(&pdoExt->WakeIrpPending);
}


PIRP
UsbhPeekNextPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    )
/*++

Routine Description:

    PIO_CSQ_PEEK_NEXT_IRP

    see DDK

    We don't allow >1 wake irp but be handle it as a list in
    the event there is some window where two irps somehow
    manage to get queued.


Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PLIST_ENTRY le;
    PIRP nextIrp = NULL;
    PIO_STACK_LOCATION irpStack;

    UsbhAssert(NULL, PeekContext == NULL);
    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    if (Irp == NULL) {
        // start at the head
        le = pdoExt->WakeIrpList.Flink;
    } else {
        // start at the next irp in the queue
        le = Irp->Tail.Overlay.ListEntry.Flink;
    }

   
    while (le != &pdoExt->WakeIrpList) {

        // a value of NULL would mean the list is corrupted
        UsbhFatalAssert(pdoExt->HubFdo,le!=NULL, Csq);

        nextIrp = (PIRP) CONTAINING_RECORD(le,
                                struct _IRP,
                                Tail.Overlay.ListEntry);

        UsbhFatalAssert(pdoExt->HubFdo,nextIrp != NULL, Csq);
        irpStack = IoGetCurrentIrpStackLocation(nextIrp);

        if (PeekContext == NULL) {
            break;
        }

// This check is unecessary since we don't use a PeekContext
//        if (irpStack->FileObject == (PFILE_OBJECT) PeekContext) {
//            break;
//        }

        le = le->Flink;
    }

    return nextIrp;
}


VOID
UsbhAcquirePdoWakeIrpLock(
    PIO_CSQ Csq,
    PKIRQL  Irql
    )
/*++

Routine Description:

    PIO_CSQ_ACQUIRE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);
    LOG(pdoExt->HubFdo, LOG_POWER, 'wwL+', 0, 0);

    KeAcquireSpinLock(&pdoExt->WakeIrpSpin, Irql);

}


VOID
UsbhReleasePdoWakeIrpLock(
    PIO_CSQ Csq,
    KIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_RELEASE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);
    LOG(pdoExt->HubFdo, LOG_POWER, 'wwL-', 0, 0);
    KeReleaseSpinLock(&pdoExt->WakeIrpSpin, Irql);
}


VOID
UsbhCompleteCanceledPdoWakeIrp(
    PIO_CSQ Csq,
    PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_COMPLETE_CANCELED_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION_PDO,
                               WakeIrpCsq);
    ASSERT_PDOEXT(pdoExt);
    LOG(pdoExt->HubFdo, LOG_POWER, 'wwCA', Irp, 0);

    // this particular wake irp has already been dequeued.
    DbgTrace((UsbhDebugTrace, "Port #%d PDO (%p) IRP(%p) **complete wake** STATUS_CANCELLED\n",
            pdoExt->PortNumber, pdoExt->Pdo, Irp));
    UsbhKdPrint((1, "Port #%d PDO (%p) IRP(%p) **complete wake** STATUS_CANCELLED\n",
            pdoExt->PortNumber, pdoExt->Pdo, Irp));

    UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_WAIT_WAKE_COMPLETE, STATUS_CANCELLED);
    UsbhCompleteIrp(Irp, STATUS_CANCELLED);

    UsbhDecPdoIoCount(pdoExt->Pdo, Irp);

}

