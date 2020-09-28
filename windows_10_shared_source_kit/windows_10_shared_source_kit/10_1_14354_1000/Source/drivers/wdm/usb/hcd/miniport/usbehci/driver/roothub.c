/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

   roothub.c

Abstract:

   miniport root hub

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    1-1-00 : created, jdunn

implements the following miniport functions:

MINIPORT_RH_GetStatus
MINIPORT_RH_GetPortStatus
MINIPORT_RH_GethubStatus

MINIPORT_RH_SetFeaturePortReset
MINIPORT_RH_SetFeaturePortSuspend
MINIPORT_RH_SetFeaturePortPower

MINIPORT_RH_ClearFeaturePortEnable
MINIPORT_RH_ClearFeaturePortSuspend
MINIPORT_RH_ClearFeaturePortPower

MINIPORT_RH_ClearFeaturePortConnectChange
MINIPORT_RH_ClearFeaturePortResetChange
MINIPORT_RH_ClearFeaturePortEnableChange
MINIPORT_RH_ClearFeaturePortSuspendChange
MINIPORT_RH_ClearFeaturePortOvercurrentChange


--*/

#include "common.h"

#include "usbpriv.h"
















































































































FORCEINLINE
VOID
EHCI_RH_GetPortSpeed(
    __inout PDEVICE_DATA DeviceData,
    __in    USHORT PortNumber,
    __in    PORTSC PortRegister,
    __inout PRH_PORT_STATUS PortStatus
    )
{

    PHOSTPC hostPcx;
    HOSTPC  pcxValue;

    switch (DeviceData->IntegratedTTSupport) {



















    case IntegratedTTNotSupported:
    default:
        // the high speed bit is not valid until after we issue a reset so we don't report
        // high speed unless the enable bit is also set
        if (PortRegister.PortConnect &&
            PortRegister.PortEnable) {
                PortStatus->HighSpeed = 1;
                PortStatus->LowSpeed = 0;
        }
        break;

    }



}

VOID
EHCI_RH_GetRootHubData(
    __in PDEVICE_DATA DeviceData,
    __out PROOTHUB_DATA HubData
    )
/*++
    return info about the root hub
--*/
{
    HubData->NumberOfPorts =
        DeviceData->NumberOfPorts;

    if (DeviceData->PortPowerControl == 1) {
        HubData->HubCharacteristics.PowerSwitchType =
            USBPORT_RH_POWER_SWITCH_PORT;
    } else {
        HubData->HubCharacteristics.PowerSwitchType =
            USBPORT_RH_POWER_SWITCH_GANG;
    }

    HubData->HubCharacteristics.Reserved = 0;
    HubData->HubCharacteristics.OverCurrentProtection = 0;
    HubData->HubCharacteristics.CompoundDevice = 0;

    HubData->PowerOnToPowerGood = 2;
    // this value is the current consumed by the hub
    // brains, for the embeded hub this doesn't make
    // much sense.
    //
    // so we report zero
    HubData->HubControlCurrent = 0;
    HubData->pad = 0;

    LOGENTRY(DeviceData, G, '_hub', HubData->NumberOfPorts,
        DeviceData->PortPowerControl, 0);

}


USB_MINIPORT_STATUS
EHCI_RH_SetWakeConnect(
     __inout PDEVICE_DATA DeviceData,
     ULONG Enable
    )
/*++
    return info about the root hub
--*/
{

    PHC_OPERATIONAL_REGISTER hcOp;
    ULONG pn;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    LOGENTRY(DeviceData, G, '_srw', Enable, DeviceData, 0);

    // we set the bit for all ports
    for (pn = 0; pn < DeviceData->NumberOfPorts; pn++) {
        PORTSC port;

        LOGENTRY(DeviceData, G, '_srp', pn+1, 0, 0);

        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[pn].ul);
        MASK_CHANGE_BITS(port);
        port.WakeOnConnect = Enable;
        port.WakeOnDisconnect = Enable;

        WRITE_REGISTER_ULONG(&hcOp->PortRegister[pn].ul, port.ul);
    }

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_GetStatus(
    __inout PDEVICE_DATA DeviceData,
    __out PUSHORT Status
    )
/*++
    get the device status
--*/
{
    // the root hub is self powered
    *Status = USB_GETSTATUS_SELF_POWERED;

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortEnable (
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    )
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    EHCI_KdPrint((DeviceData, 1
        , "port[%d] disable (1) %x\n", PortNumber, port.ul));

    port.PortEnable = 0;
    MASK_CHANGE_BITS(port);

    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                         port.ul);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortPower (
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    )
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    // turn power off

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

    port.PortPower = 0;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
        port.ul);

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_RH_PortResumeComplete(
    __inout PDEVICE_DATA DeviceData,
    PVOID Context
    )
/*++

Routine Description:

    This routine completes a port resume operation.

    This USBPORTSVC_RequestAsyncCallbackEx() callback routine is always
    called with the USBPORT Fdo.MpRootHubFunctionSpin spinlock held.

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    PEHCI_PORT_EVENT_CONTEXT portResumeContext = Context;
    USHORT portNumber;
    NTSTATUS nts;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    portNumber = portResumeContext->PortNumber;

    EHCI_ASSERT_RUN_BIT(DeviceData);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
    LOGENTRY(DeviceData, G, '_pRS', port.ul,
        DeviceData->PortSuspendChange, portNumber);

EHCI_KdPrint((DeviceData, 1, "port[%d] resume (1) %x\n", portNumber, port.ul));


    // writing a 0 stops resume sequence
    MASK_CHANGE_BITS(port);
    port.ForcePortResume = 0;
    port.PortSuspend = 0;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul,
        port.ul);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
    EHCI_KdPrint((DeviceData, 1, "port[%d] resume (2) %x\n", portNumber, port.ul));

#ifdef _ARM_
























#endif

    if ((DeviceData->ResumeTimeRefCount[portNumber-1] != 0) &&
        (portResumeContext->ResumeTime < 50) &&
        (port.PortSuspend == 1)) {

        UCHAR retryTime;

        // The port status still shows that the port is suspended.  This may be due
        // to the port still transistioning to high-speed operation, which per the 
        // EHCI spec can take up to 2 milliseconds.  It could also be that the HC
        // is timing resume signaling in HW, which is typical on ARM platforms.  
        // For cases where the device has specified a minimum resume signaling time
        // via the MS OS 2.0 descriptor, we will reschedule this timer callback in 2ms
        // increments until either the port suspend bit is clear or 50ms has elapsed, 
        // which is the legacy resume signaling time for the EHCI driver.

        if (portResumeContext->Retry == FALSE) {
            retryTime = 2;
        } else {
            retryTime = 15;
        }

        portResumeContext->ResumeTime += retryTime;
        portResumeContext->Retry = TRUE;

        nts = USBPORT_REQUEST_ASYNC_CALLBACK_EX(DeviceData,
                                                retryTime, // callback in ms,
                                                portResumeContext,
                                                sizeof(struct _EHCI_PORT_EVENT_CONTEXT),
                                                EHCI_RH_PortResumeComplete,
                                                NULL,
                                                TRUE);

        if (NT_SUCCESS(nts)) {
            return;
        }
    }

    // In the cases where EHCI_InterruptDpcEx() detected resume signaling
    // and timer callback was queued to call EHCI_RH_PortResumeComplete()
    // but at the same time the driver was preparing to enter an Sx state,
    // resume signaling shall be ignored here.
    //
    // The resume signaling is not expected to be ignored when the timer
    // callback to EHCI_RH_PortResumeComplete() was queued by
    // EHCI_RH_ClearFeaturePortSuspend().

    CLEAR_BIT(DeviceData->PortInResume, portNumber-1);

    if (FALSE == RegistrationPacket.USBPORTSVC_IgnoreResumeSignaling(DeviceData)) {
        // indicate a change to suspend state ie resume complete
        SET_BIT(DeviceData->PortSuspendChange, portNumber-1);
        CLEAR_BIT(DeviceData->PortSuspended, portNumber-1);

        // signalling the root hub here will allow a faster response to the change
        USBPORT_INVALIDATE_ROOTHUB(DeviceData);
    }

    // allow idle
    if (portResumeContext->AsyncIdleRef) {
        EHCI_DerefAsyncIdle(DeviceData, portResumeContext->AsyncIdleRef);
        portResumeContext->AsyncIdleRef = NULL;
    }

}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortSuspend (
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    )
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    EHCI_PORT_EVENT_CONTEXT portResumeContext;
    USB_MINIPORT_STATUS mps = USBMP_STATUS_SUCCESS;
    UCHAR resumeTime;
    NTSTATUS nts;

    // resume the port
    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    // mask off CC chirping on this port
    SET_BIT(DeviceData->PortPMChirp, PortNumber-1);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

    LOGENTRY(DeviceData, G, '_Prs', PortNumber, port.ul, 0);

    SET_BIT(DeviceData->PortInResume, PortNumber-1);
    // writing a 1 generates resume signalling
    port.ForcePortResume = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
        port.ul);


    // time it
    resumeTime = 50;
    
    if (DeviceData->ResumeTimeRefCount[PortNumber-1] != 0) {
        resumeTime = DeviceData->ResumeSignalTime[PortNumber-1];
    }

    portResumeContext.ResumeTime = resumeTime;
    portResumeContext.PortNumber = PortNumber;
    portResumeContext.Retry = FALSE;
    portResumeContext.AsyncIdleRef = EHCI_RefAsyncIdle(DeviceData, RH_PortResumeComplete, NULL);





    nts = USBPORT_REQUEST_ASYNC_CALLBACK_EX(DeviceData,
                                      resumeTime, // callback in ms,
                                      &portResumeContext,
                                      sizeof(struct _EHCI_PORT_EVENT_CONTEXT),
                                      EHCI_RH_PortResumeComplete,
                                      NULL,
                                      TRUE);

    LOGENTRY(DeviceData, G, '_Pr2', PortNumber, port.ul, nts);

    // trying to track down resume hang with this
    EHCI_ASSERT(DeviceData, NT_SUCCESS(nts));
    if (NT_ERROR(nts)) {
        if (portResumeContext.AsyncIdleRef) {
            EHCI_DerefAsyncIdle(DeviceData, portResumeContext.AsyncIdleRef);
        }
        mps = USBMP_STATUS_FAILURE;
    }

    return mps;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortSuspendChange (
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    )
{
    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    CLEAR_BIT(DeviceData->PortSuspendChange, PortNumber-1);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortOvercurrentChange (
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    )
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    EHCI_KdPrint((DeviceData, 1,
                  "'EHCI_RH_ClearFeatureOvercurrentChange\n"));

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

    MASK_CHANGE_BITS(port);
    port.OvercurrentChange = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
        port.ul);

    return USBMP_STATUS_SUCCESS;
}

USB_MINIPORT_STATUS
EHCI_RH_GetPortStatus(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber,
    __out PRH_PORT_STATUS portStatus
    )
/*++
    get the status of a partuclar port
--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        portStatus->ul = 0;
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        portStatus->ul = 0;

        portStatus->PowerOn = 1;

        return USBMP_STATUS_SUCCESS;
    }

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    portStatus->ul = 0;
    LOGENTRY(DeviceData, G, '_Pp1', PortNumber, port.ul, 0);

// NEOHUB
// removed old hub driver hacks
#if 0
    // low speed detect, if low speed then do an immediate
    // handoff to the CC
    // This field is only valid if enable status is 0 and
    // connect status is 1

    if ((port.LineStatus == 1) &&
         port.PortOwnedByCC == 0 &&
         port.PortSuspend == 0 &&
         port.PortEnable == 0 &&
         port.PortConnect == 1 ) {

        EHCI_KdPrint((DeviceData, 1, "'low speed device detected %d (skip)\n", PortNumber));
        TEST_TRAP(DeviceData);
        // low speed device detected
        port.PortOwnedByCC = 1;
        WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                             port.ul);

        // reread port status after handing to the HC
        //
        // Must reread port status after handing control to CC otherwise a
        // bogus connect status is reported to the hub driver.  For the new
        // hub this will result in it trying to reset the port.
        // I think this may be what causes the mysterious device-not-recognized
        // popup you see plugging in 1.1 devices to the 2.0 root.
        //
        // You get the popup but nothing appears to be wrong.
        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
        LOGENTRY(DeviceData, G, '_rr1', PortNumber, port.ul, 0);

        return USBMP_STATUS_SUCCESS;

    }
#endif

    // map the bits to the port status structure

    portStatus->Connected =
        port.PortConnect;
    portStatus->Enabled =
        port.PortEnable;
    portStatus->Suspended =
        port.PortSuspend;
    portStatus->OverCurrent =
        port.OvercurrentActive;
    portStatus->Reset =
        port.PortReset;
    portStatus->PowerOn =
        port.PortPower;
    portStatus->OwnedByCC =
        port.PortOwnedByCC;

    EHCI_RH_GetPortSpeed(DeviceData, PortNumber, port, portStatus);

#if 0
    else {
        // report high speed when no device connected
        // this should work around a bug in the usbhub
        // driver -- the hub driver does not refresh the
        // port status register if the first reset attempt
        // fails.
        portStatus->HighSpeed = 1;
    }
#endif

    //
    // Some controllers automatically time the resume signalling and
    // when complete, will automatically clear the ForcePortResume
    // This may all happen before we have had a chance to read the port status.
    // So, we will check if we think the port was suspended, but it is now
    // not suspended, and set the PortSuspendChange bit appropriately
    //
    if (!portStatus->Suspended && !port.ForcePortResume) {
        if (TEST_BIT(DeviceData->PortSuspended, PortNumber-1)) {
            SET_BIT(DeviceData->PortSuspendChange, PortNumber - 1);
            CLEAR_BIT(DeviceData->PortSuspended, PortNumber - 1);
        }

    }

    // chirping support allows us to use the
    // port change status bit
    if (port.PortConnectChange == 1) {
        SET_BIT(DeviceData->PortConnectChange, PortNumber-1);
    }

    portStatus->EnableChange =
        port.PortEnableChange;
    portStatus->OverCurrentChange =
        port.OvercurrentChange;

    // these change bits must be emulated
    if (TEST_BIT(DeviceData->PortResetChange, PortNumber-1)) {
        portStatus->ResetChange = 1;
    }

    if (TEST_BIT(DeviceData->PortConnectChange, PortNumber-1)) {
        portStatus->ConnectChange = 1;
    }

    if (TEST_BIT(DeviceData->PortSuspendChange, PortNumber-1)) {
        portStatus->SuspendChange = 1;
    }

    LOGENTRY(DeviceData, G, '_gps',
        PortNumber, portStatus->ul, port.ul);

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_RH_FinishReset(
    __inout PDEVICE_DATA DeviceData,
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine completes a port reset operation after an appropriate
    delay after EHCI_RH_PortResetComplete() clears the the Port Reset
    bit.

    This USBPORTSVC_RequestAsyncCallbackEx() callback routine is always
    called with the USBPORT Fdo.MpRootHubFunctionSpin spinlock held.

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    PEHCI_PORT_EVENT_CONTEXT portResetContext = Context;
    USHORT portNumber;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    portNumber = portResetContext->PortNumber;

    LOGENTRY(DeviceData, G, '_fnr', portNumber, Context, 0);

    do {
        if (portNumber > EHCI_MAX_PORT) {
            // port number bogus
            break;
        }

        DeviceData->PortResetCb[portNumber-1] = NULL;
        DeviceData->PortResetCbRef[portNumber-1] = NULL;

        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
        EHCI_KdPrint((DeviceData, 1,
            "port[%d] reset (4) %x\n", portNumber, port.ul));

        if (port.ul == 0xFFFFFFFF) {
            // just bail if hardware disappears
            break;
        }

        // at this point we will know if this is a high speed
        // device -- if it is not then we need to hand the port
        // to the CC

        // port enable of zero means we have a full or low speed
        // device (ie not chirping).
#if DBG
        if (port.PortConnect == 0) {
            EHCI_KdPrint((DeviceData, 1, "HS device dropped\n"));
        }
#endif
        if (port.PortEnable == 0 &&
            port.PortConnect == 1 &&
            port.PortConnectChange == 0 &&
            DeviceData->CompanionControllerHandoffEnabled) {

            // do the handoff
            port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
            port.PortOwnedByCC = 1;
            WRITE_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul,
                                 port.ul);

            // always indicate the change, the neohub driver can handle the case where the device
            // drops off the bus.
            SET_BIT(DeviceData->PortResetChange, portNumber-1);
            USBPORT_INVALIDATE_ROOTHUB(DeviceData);
        } else {
            // we have a USB 2.0 device, indicate the reset change
            // NOTE if the device dropped off the bus (NEC USB 2 hub or
            // user removed it) we still indicate a reset change on high
            // speed
            SET_BIT(DeviceData->PortResetChange, portNumber-1);
            USBPORT_INVALIDATE_ROOTHUB(DeviceData);
        }

        CLEAR_BIT(DeviceData->PortPMChirp, portNumber-1);
    } WHILE (0);

    if (portResetContext->AsyncIdleRef) {
        EHCI_DerefAsyncIdle(DeviceData, portResetContext->AsyncIdleRef);
        portResetContext->AsyncIdleRef = NULL;
    }
}


VOID
EHCI_RH_PortResetComplete(
    __inout PDEVICE_DATA DeviceData,
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine clears the Port Reset bit in the port status register
    after an appropriate delay after EHCI_RH_SetFeaturePortReset() set
    the the Port Reset bit.

    It is required that resets from root ports have a duration of at
    least 50 ms (TDRSTR).

    This USBPORTSVC_RequestAsyncCallbackEx() callback routine is always
    called with the USBPORT Fdo.MpRootHubFunctionSpin spinlock held.

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    PEHCI_PORT_EVENT_CONTEXT portResetContext = Context;
    USHORT portNumber;
    ULONG microsecs;
    ULONG num_tries = 0;
    USBCMD cmd;
    PEHCI_IDLE_REF_CONTEXT oldRef;
    CONFIGFLAG configFlag;
    ULONG hw_error = 0;
    NTSTATUS nts;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    portNumber = portResetContext->PortNumber;

    oldRef = portResetContext->AsyncIdleRef;
    portResetContext->AsyncIdleRef = NULL;

    LOGENTRY(DeviceData, G, '_prC', Context, 0, 0);

    if (portNumber > EHCI_MAX_PORT) {

        // port number bogus
        if (oldRef) {
            EHCI_DerefAsyncIdle(DeviceData, oldRef);
        }

        LOGENTRY(DeviceData, G, '_pr1', Context, 0, 0);
        return;
    }

    DeviceData->PortResetCb[portNumber-1] = NULL;
    DeviceData->PortResetCbRef[portNumber-1] = NULL;

EHCI_RH_PortResetComplete_Retry:

    microsecs = 0;
    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
    if (num_tries < 10) {
        // skip logging after 10 tries
        LOGENTRY(DeviceData, G, '_pr2', port.ul, DeviceData->PortResetChange, portNumber);
    }

    EHCI_KdPrint((DeviceData, 1, "port[%d] reset (1) %x\n", portNumber, port.ul));

    // writing a 0 stops reset
    MASK_CHANGE_BITS(port);
    port.PortReset = 0;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul,
                         port.ul);
    
    // wait for reset to go low -- this should be on the order of microseconds
    do {






        microsecs+=20;
        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);
        EHCI_KdPrint((DeviceData, 1, "port[%d] reset (2) %x\n", portNumber, port.ul));

        if (microsecs > USBEHCI_MAX_PORT_RESET_CLEAR_TIME) {
            // > 1 microframe (125 us) has passed, retry
            EHCI_KdPrint((DeviceData, 1,
                "port[%d] reset (timeout) %x\n", portNumber, port.ul));
            num_tries++;

            // arbitrary max number of retries
            if (num_tries > USBEHCI_MAX_PORT_RESET_CLEAR_RETRIES) {
                hw_error = 1;
                break;
            }
            goto EHCI_RH_PortResetComplete_Retry;
        }

        // bail if HW is gone
        if (port.ul == EHCI_HARDWARE_GONE) {
            hw_error = 1;
            break;
        }

        // should not be resetting if owned by CC
        //
        // if the config flag is reset then we no longer own the ports so don't try to clear
        // the reset -- some controllers won't let us clear it. Instead we bail here.
        configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

        if (num_tries < 10) {
            // skip logging after 10 tries
            LOGENTRY(DeviceData, G, '_pr3', port.ul, configFlag.ul , portNumber);
        }

        if (configFlag.ul == 0) {
            hw_error = 1;
            break;
        }

        if (!DeviceData->StopHcForPortOperations) {
            cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
            if (cmd.HostControllerRun == 0) {
                // run bit off so this won't complete
                hw_error = 1;
                break;
            }
        }

    } while (port.PortReset == 1);











    EHCI_KdPrint((DeviceData, 1, "port[%d] reset (3) %x\n", portNumber, port.ul));

    // skip this if we pickup an error
    if (hw_error) {
        // perform some cleanup
        CLEAR_BIT(DeviceData->PortPMChirp, portNumber-1);
    } else {

        portResetContext->AsyncIdleRef = EHCI_RefAsyncIdle(DeviceData, RH_FinishReset, NULL);
        DeviceData->PortResetCbRef[portNumber-1] = portResetContext->AsyncIdleRef;

        nts = USBPORT_REQUEST_ASYNC_CALLBACK_EX(DeviceData,
                                                50, // callback in 10 ms,
                                                portResetContext,
                                                sizeof(struct _EHCI_PORT_EVENT_CONTEXT),
                                                EHCI_RH_FinishReset,
                                                &DeviceData->PortResetCb[portNumber-1],
                                                TRUE);

        LOGENTRY(DeviceData, G, '_pr4', portNumber, port.ul, nts);

        EHCI_ASSERT(DeviceData, NT_SUCCESS(nts));

        if (NT_ERROR(nts)) {
            if (portResetContext->AsyncIdleRef) {
                EHCI_DerefAsyncIdle(DeviceData, portResetContext->AsyncIdleRef);
                portResetContext->AsyncIdleRef = NULL;
                DeviceData->PortResetCbRef[portNumber-1] = NULL;
            }
        }
    }

    //
    // drop the original ref
    //

    if (oldRef) {
        EHCI_DerefAsyncIdle(DeviceData, oldRef);
    }

}

USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortReset(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++
    Put a port in reset
--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    EHCI_PORT_EVENT_CONTEXT portResetContext;
    USB_MINIPORT_STATUS mps = USBMP_STATUS_SUCCESS;
    NTSTATUS nts;
    USBCMD cmd;
    ULONG resetTimeout;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber > EHCI_MAX_PORT) {
        return USBMP_STATUS_FAILURE;
    }

    if (DeviceData->PortResetCb[PortNumber-1] != NULL) {
        BOOLEAN canceled;
        // cancel any reset that may be in progress
        canceled = USBPORT_CANCEL_ASYNC_CALLBACK(DeviceData, DeviceData->PortResetCb[PortNumber-1], TRUE);

        if (canceled) {
            DeviceData->PortResetCb[PortNumber-1] = NULL;

            // Free the Async Idle Ref now since the callback will not
            // execute and free it.
            //

            if (DeviceData->PortResetCbRef[PortNumber-1] != NULL) {
                EHCI_DerefAsyncIdle(DeviceData, DeviceData->PortResetCbRef[PortNumber-1]);
                DeviceData->PortResetCbRef[PortNumber-1] = NULL;
            }
        } else {
            // if we could not cancel the reset callback dpc is in progress, fail the request
            return USBMP_STATUS_FAILURE;
        }
    }

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    LOGENTRY(DeviceData, G, '_spr', port.ul,
        0, PortNumber);

    // low speed device detect
    // EHCI spec specifies the linestatus is 01 if this is a low speed device and we
    // are supposed to bypass the high speed reset/chirp.  There is at least one low speed
    // device that requires this behavior.

    // For NEOHUB this detection has been moved to when we issue the reset. This is the more
    // appropriate place for this detection and neohub can handle the case where the device drops
    // during reset cleanly.

    if ((port.LineStatus == 1) &&
         port.PortOwnedByCC == 0 &&
         port.PortSuspend == 0 &&
         port.PortEnable == 0 &&
         port.PortConnect == 1 &&
         port.PortConnectChange == 0 &&
         DeviceData->LowSpeedHandOffEnabled &&
         DeviceData->CompanionControllerHandoffEnabled) {

        EHCI_KdPrint((DeviceData, 1, "'low speed device detected %d\n", PortNumber));

        // low speed device detected
        port.PortOwnedByCC = 1;
        WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                             port.ul);

        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
        LOGENTRY(DeviceData, G, '_rr1', PortNumber, port.ul, 0);

        return USBMP_STATUS_SUCCESS;
    }

    // before initiating a reset make sure we have a device connected, if we do
    // not, skip this and let the request time out.
    // if we attempt a reset while the owned by CC bit is set this will cause
    // problems even though we don't own the port.
    if (port.PortOwnedByCC || port.PortConnect == 0) {
        LOGENTRY(DeviceData, G, '_mtp', port.ul, 0, PortNumber);
        return USBMP_STATUS_SUCCESS;
    }

    // mask off CC chirping on this port
    SET_BIT(DeviceData->PortPMChirp, PortNumber-1);

    // should not be driving reset while suspended
    EHCI_ASSERT(DeviceData, port.PortSuspend == 0);
    EHCI_ASSERT(DeviceData, port.ForcePortResume == 0);

    // do a normal reset sequence
    LOGENTRY(DeviceData, G, '_res', port.ul, 0, PortNumber);










    MASK_CHANGE_BITS(port);
    port.PortEnable = 0;
    port.PortReset = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul, port.ul);








    // schedule a callback
    portResetContext.PortNumber = PortNumber;
    // note that usbport calls us back with a copy of this
    // structure not the pointer to the original structure

    resetTimeout = 50;









    } else {
        // Do not reference the async idle list (and turn on the async schedule) 
        // if the controller is not running.
        portResetContext.AsyncIdleRef = EHCI_RefAsyncIdle(DeviceData, RH_PortResetComplete, NULL);
        DeviceData->PortResetCbRef[PortNumber-1] = portResetContext.AsyncIdleRef;
    }

    nts = USBPORT_REQUEST_ASYNC_CALLBACK_EX(DeviceData,
                                            resetTimeout, // callback in x ms,
                                            &portResetContext,
                                            sizeof(struct _EHCI_PORT_EVENT_CONTEXT),
                                            EHCI_RH_PortResetComplete,
                                            &DeviceData->PortResetCb[PortNumber-1],
                                            TRUE);

    LOGENTRY(DeviceData, G, '_sr2', PortNumber, port.ul, nts);

    EHCI_ASSERT(DeviceData, NT_SUCCESS(nts));

    if (NT_ERROR(nts)) {
        if (portResetContext.AsyncIdleRef) {
            EHCI_DerefAsyncIdle(DeviceData, portResetContext.AsyncIdleRef);
            portResetContext.AsyncIdleRef = NULL;
            DeviceData->PortResetCbRef[PortNumber-1] = NULL;
        }











        mps = USBMP_STATUS_FAILURE;
    }

    return mps;
}


USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortSuspend(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++
    Put a port in suspend
--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    USBCMD cmd;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber) {
        return USBMP_STATUS_SUCCESS;
    }










    // NOTE:
    // there should be no transactions in progress at the
    // time we suspend the port.
    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    LOGENTRY(DeviceData, G, '_sps', port.ul,
        0, PortNumber);

    SET_BIT(DeviceData->PortSuspended, PortNumber - 1);

    // writing a 1 suspends the port
    MASK_CHANGE_BITS(port);
    port.PortSuspend = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,  port.ul);
    // wiat 1 microframe for current transaction to finish
    KeStallExecutionProcessor(125);

    if (DeviceData->StopHcForPortOperations) {
        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        cmd.HostControllerRun = 1;
        WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);
    }

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortPower(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++
--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    LOGENTRY(DeviceData, G, '_spp', port.ul,
        0, PortNumber);

    // writing a 1 turns on power
    MASK_CHANGE_BITS(port);
    port.PortPower = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
        port.ul);
    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortEnable(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++
--*/
{
    // do nothing, independent enable not supported

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortConnectChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    //
    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    LOGENTRY(DeviceData, G, '_pcc', port.ul,
        0, PortNumber);

    // writing a 1 zeros the change bit
    if (port.PortConnectChange == 1) {
        // mask off other change bits
        MASK_CHANGE_BITS(port);
        port.PortConnectChange = 1;

        WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
            port.ul);
    }

    CLEAR_BIT(DeviceData->PortConnectChange, PortNumber-1);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortEnableChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    LOGENTRY(DeviceData, G, '_cpe', PortNumber, 0, 0);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    MASK_CHANGE_BITS(port);
    port.PortEnableChange = 1;

    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul, port.ul);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_GetHubStatus(
    __inout PDEVICE_DATA DeviceData,
    __out PRH_HUB_STATUS HubStatus
    )
/*++
--*/
{
    // Nothing interesting for the root hub to report
    //
    HubStatus->ul = 0;

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortResetChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
/*++

--*/
{
    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    CLEAR_BIT(DeviceData->PortResetChange, PortNumber-1);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_UsbprivRootPortStatus(
    __inout PDEVICE_DATA DeviceData,
    ULONG ParameterLength,
    __inout PVOID Parameters
    )
{

    PUSBPRIV_ROOTPORT_STATUS portStatusParams;
    PHC_OPERATIONAL_REGISTER hcOp;
    PRH_PORT_STATUS portStatus;
    PORTSC port;
    USHORT portNumber;

    if (ParameterLength < sizeof(USBPRIV_ROOTPORT_STATUS))
    {
        return (USBMP_STATUS_FAILURE);
    }

    //
    // Read the port status for this port from the registers
    //

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    portStatusParams = (PUSBPRIV_ROOTPORT_STATUS) Parameters;

    portNumber = portStatusParams->PortNumber;
    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);

    LOGENTRY(DeviceData, G, '_Up1', portNumber, port.ul, 0);

    //
    // Check to see if the port is resuming.  If so, clear the bit and
    //  reenable the port.
    //

    if (port.ForcePortResume)
    {
        //
        // Clear the port resume
        //

        USBPORT_WAIT(DeviceData, 50);

        MASK_CHANGE_BITS(port);
        port.ForcePortResume = 0;
        port.PortSuspend = 0;
        WRITE_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul, port.ul);

        //
        // Reread the port status
        //

        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[portNumber-1].ul);

        SET_BIT(DeviceData->PortSuspendChange, portNumber-1);

        LOGENTRY(DeviceData, G, '_Up2', portNumber, port.ul, 0);
    }

    //
    // Map the current port information to the port status
    //

    portStatus = &portStatusParams->PortStatus;

    portStatus->ul = 0;
    portStatus->Connected =
        port.PortConnect;
    portStatus->Enabled =
        port.PortEnable;
    portStatus->Suspended =
        port.PortSuspend;
    portStatus->OverCurrent =
        port.OvercurrentActive;
    portStatus->Reset =
        port.PortReset;
    portStatus->PowerOn =
        port.PortPower;
    portStatus->OwnedByCC =
        port.PortOwnedByCC;

    if (portStatus->Connected == 1) {
        portStatus->HighSpeed = 1;
        portStatus->LowSpeed = 0;
    } else {
        // report high speed when no device connected
        // this should work around a bug in the usbhub
        // driver -- the hub driver does not refresh the
        // port status register if the first reset attempt
        // fails.
        portStatus->HighSpeed = 1;
    }


    //
    // Some controllers automatically time the resume signalling and
    // when complete, will automatically clear the ForcePortResume
    // This may all happen before we have had a chance to read the port status.
    // So, we will check if we think the port was suspended, but it is now
    // not suspended, and set the PortSuspendChange bit appropriately
    //
    if (!portStatus->Suspended && !port.ForcePortResume) {
        if (TEST_BIT(DeviceData->PortSuspended, portNumber-1)) {
            SET_BIT(DeviceData->PortSuspendChange, portNumber - 1);
            CLEAR_BIT(DeviceData->PortSuspended, portNumber - 1);
        }
    }

    portStatus->ConnectChange =
        port.PortConnectChange;
    portStatus->EnableChange =
        port.PortEnableChange;
    portStatus->OverCurrentChange =
        port.OvercurrentChange;

    // these change bits must be emulated
    if (TEST_BIT(DeviceData->PortResetChange, portNumber-1)) {
        portStatus->ResetChange = 1;
    }

    if (TEST_BIT(DeviceData->PortConnectChange, portNumber-1)) {
        portStatus->ConnectChange = 1;
    }

    if (TEST_BIT(DeviceData->PortSuspendChange, portNumber-1)) {
        portStatus->SuspendChange = 1;
    }

    LOGENTRY(DeviceData, G, '_Ups',
        portNumber, portStatus->ul, port.ul);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_RH_ChirpRootPort(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    )
{
    PHC_OPERATIONAL_REGISTER hcOp;
    PORTSC port;
    ULONG microsecs;
    ULONG num_tries = 0;
    CONFIGFLAG configFlag;
    USBCMD cmd;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return USBMP_STATUS_SUCCESS;
    }

    if (PortNumber == DeviceData->DebugPortNumber)
    {
        return USBMP_STATUS_SUCCESS;
    }

    EHCI_ASSERT_RUN_BIT(DeviceData);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
    LOGENTRY(DeviceData, G, '_chr', port.ul,
        0, PortNumber);

    EHCI_KdPrint((DeviceData, 1,
        ">port[%d] chirp %x\n", PortNumber, port.ul));

    if (TEST_BIT(DeviceData->PortPMChirp, PortNumber-1)) {
        // skip the chirp if we have already done this
        EHCI_KdPrint((DeviceData, 1,
            "<skip port chirp[%d] %x\n", PortNumber, port.ul));
        return USBMP_STATUS_SUCCESS;
    }

    if (!port.PortPower) {
        // skip if not powered, this will cause us to
        // bypass the chirp if the controller has not initialized
        // such as in the case of BOOT
        EHCI_KdPrint((DeviceData, 1,
            "<skip port chirp[%d] %x, no power\n", PortNumber, port.ul));
        return USBMP_STATUS_SUCCESS;
    }

    // port is connect and not enabled and not owned by CC
    // therefore we should probably chirp it
    if (port.PortConnect &&
        !port.PortEnable &&
        !port.PortOwnedByCC) {

        //TEST_TRAP(DeviceData);
        // quick check for handoff of LS devices
        if ((port.LineStatus == 1) &&
             port.PortOwnedByCC == 0 &&
             port.PortSuspend == 0 &&
             port.PortEnable == 0 &&
             port.PortConnect == 1 &&
             DeviceData->CompanionControllerHandoffEnabled) {
            
            // low speed device detected
            port.PortOwnedByCC = 1;
            WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                                 port.ul);
           
            EHCI_KdPrint((DeviceData, 1,
                        ">port chirp[%d] %x, ->cc(1)\n", PortNumber,
                        port.ul));
            return USBMP_STATUS_SUCCESS;
        }

        // do a normal reset sequence
        LOGENTRY(DeviceData, G, '_rss', port.ul, 0, PortNumber);

        // should not be driving reset while suspended
        EHCI_ASSERT(DeviceData, port.PortSuspend == 0);
        EHCI_ASSERT(DeviceData, port.ForcePortResume == 0);

        // set reset and clear connect change
        port.PortEnable = 0;
        port.PortReset = 1;
        WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul, port.ul);

        USBPORT_WAIT(DeviceData, 10);

EHCI_RH_ChirpRootPort_Retry:

        microsecs = 0;
        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

        // writing a 0 stops reset
        MASK_CHANGE_BITS(port);
        port.PortReset = 0;
        WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul, port.ul);

        //
        // Wait for the port reset to clear.  This should be on the
        // order of microseconds.
        //
        WHILE_TRUE {

            //
            // Wait at least 20 microseconds before checking to see if
            // the host controller has cleared the port reset.
            //
            KeStallExecutionProcessor(20);
            microsecs += 20;

            port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

            if (port.PortReset == 0) {
                //
                // The port reset is now clear and the wait is now done.
                //
                break;
            }

            if (port.ul == EHCI_HARDWARE_GONE) {
                //
                // The hardware appears to be gone so give up.
                //
                break;
            }

            configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

            if (configFlag.ul == 0) {
                //
                // If the config flag is clear then we no longer own the
                // ports so do not try to clear the port reset.  Some
                // controllers will not clear the port reset in this
                // case.
                //
                break;
            }

            cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

            if (cmd.HostControllerRun == 0) {
                //
                // The host controller may hold port reset asserted to
                // a one if the controller is not running.
                //
                break;
            }

            if (microsecs > USBEHCI_MAX_PORT_RESET_CLEAR_TIME) {
                //
                // The port reset did not clear within the maximum wait
                // time.  Try again to clear the port reset unless the
                // maximum number of retries has been reached.
                //
                num_tries++;

                if (num_tries > USBEHCI_MAX_PORT_RESET_CLEAR_RETRIES) {
                    break;
                }
                goto EHCI_RH_ChirpRootPort_Retry;
            }
        }

        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);

        if (port.PortEnable == 0) {

            if (DeviceData->CompanionControllerHandoffEnabled) {
                // do the handoff
                port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
                port.PortOwnedByCC = 1;
                WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                                     port.ul);
                EHCI_KdPrint((DeviceData, 1,
                              "<port chirp[%d] %x, ->cc(2)\n", PortNumber,
                              port.ul));
            }
            
        } else {
            // clear the enable bit so the device does not listen
            // on address 0
            port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul);
            port.PortEnable = 0;
            MASK_CHANGE_BITS(port);

            // allow some SOFs before we disable



            USBPORT_WAIT(DeviceData, 10);
            SET_BIT(DeviceData->PortPMChirp, PortNumber-1);
            WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNumber-1].ul,
                                 port.ul);

            EHCI_KdPrint((DeviceData,   1, "<chirp port[%d] disable %x\n",
                PortNumber, port.ul));

        }
    } else {
         EHCI_KdPrint((DeviceData, 1,
            "<no port chirp[%d] %x\n", PortNumber, port.ul));
    }

    return USBMP_STATUS_SUCCESS;
}


USHORT
EHCI_GetDebugPortNumber(
    __in PDEVICE_DATA DeviceData
    )
{
    return DeviceData->DebugPortNumberUI;
}


BOOLEAN
EHCI_CheckInvalidateRootHub(
    __in PDEVICE_DATA DeviceData
    )
{
    BOOLEAN InvalidateRootHub;

    InvalidateRootHub = DeviceData->InvalidateRootHub;
    DeviceData->InvalidateRootHub = FALSE;

    return InvalidateRootHub;
}


VOID
EHCI_SetPortResumeTime(
    __in PDEVICE_DATA DeviceData,
    __in USHORT PortNumber,
    __in BOOLEAN SetTime,
    __in UCHAR ResumeTime
    )
{
    PLONG refCount;
    
    if (PortNumber > EHCI_MAX_PORT) {
        return;
    }
    
    refCount = &DeviceData->ResumeTimeRefCount[PortNumber - 1];
    
    EHCI_ASSERT(DeviceData, *refCount >= 0);

    if (SetTime == TRUE) {

        (*refCount)++;
        
        // In cases where multiple downstream devices have set a minimum
        // resume time, use the slowest time for the root port.  Note that
        // the resume time set by the hub driver in these cases should be 
        // 20ms for non-root attached devices.
        if ((((*refCount) > 1) && 
             (DeviceData->ResumeSignalTime[PortNumber - 1] < ResumeTime)) ||
            ((*refCount) == 1)) {

            DeviceData->ResumeSignalTime[PortNumber - 1] = ResumeTime;
        }
    } else if ((*refCount) > 0) {
        (*refCount)--;
    }
}
