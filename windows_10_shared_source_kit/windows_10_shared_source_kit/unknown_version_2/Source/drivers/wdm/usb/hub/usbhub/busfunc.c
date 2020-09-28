
/*++

Copyright (c) Microsoft Corporation. All rights reserved.


Module Name:

    busfunc.c

     11-16-2003

Abstract:

    contains bus state handler routines for ProccessChangeWorker






Module:


Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "busfunc.tmh"
#endif

CCHAR usbfile_busfunc_c[] = "busfunc.c";

#define USBFILE usbfile_busfunc_c



/*
Handler routine:


ULONG HubPortEventHandler(PDEVICE_OBJECT HubFdo,
                          PHUB_PORT_DATA PortData,
                          PDEVICE_OBJECT Pdo
                          );

Arguments:
    Pdo - Pdo is latched

*/



VOID
UsbhSetEnumFailData(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo,
    PENUM_FAIL_DATA EnumFailData
    )
/*++

Routine Description:

    Sets up the data record we store with a device 'enumeration' failure event
    that is reported to the user.

Irql: PASSIVE

Arguments:

    PortData - Port we are working with.

    Pdo - pdo we are working with -- this PDO is latched by the enum engine.

Return Value:

    none
--*/
{

    EnumFailData->DeltaTm = PortData->TmResetEnd - PortData->TmResetStart;
    EnumFailData->RetryCount = PortData->EnumRetries;
    EnumFailData->PortNumber = PortData->PortNumber;

}


USB_CONNECTION_STATUS
UsbhSetEnumerationFailed(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo,
    HUB_EXCEPTION_CLASS ExClass,
    PENUM_FAIL_DATA EnumFailData,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus,
    PCSTR SzFile,
    ULONG Line
    )
/*++

Routine Description:

   Sets enumeration failure status for the PDO.  The sets flags an other fields
   appropriately for a failed to enumerate device.

   In this context, failed to enumerate means we were unable to initialize the device
   sufficiently enough to load a driver.

   This is a helper function for the bus state routines so we don't have duplicate code
   in each path that reports an enum failure.

Irql: PASSIVE

Arguments:
    PortData - Port we are working with.



    Pdo - pdo we are working with -- this PDO is latched by the enum engine, this routine
            releases the latch.

Return Value:

    Connection Status
--*/
{
    USB_CONNECTION_STATUS connectionStatus;
    PSTATE_CONTEXT sc;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    HUB_PDO_HWPNP_STATE pdoState;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    sc = PortData->CurrentSc;
    // default to current status;
    connectionStatus = PortData->ConnectionStatus;

    UsbhDisablePort(HubFdo, PortData);

    // release the lock only if we hold it
    UsbhReleaseEnumBusLockEx(HubFdo, PortData->CurrentSc, PortData->PortNumber, FALSE);

    // generate exception and popup message unless this is a driver
    // initialed reset

    UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_RegisterPdo);
    pdoState = GET_PDO_HWPNPSTATE(Pdo);
    LOG(HubFdo, LOG_BUSF, 'enu!', Pdo, pdoState);

    UsbhAssert(HubFdo, PortData != NULL);

    // see if we are re-registering -- PDO is known to pnp
    if (pdoState == Pdo_PnpRefHwPresent && PortData) {

        NTSTATUS syncResetNts = STATUS_SUCCESS;


        UsbhAssert(HubFdo, PortData->PnpPdo == Pdo);
        syncResetNts = STATUS_UNSUCCESSFUL;

        UsbhReleasePdoStateLock(HubFdo, sc);

        // allow PDO to be deleted if necessary
        BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

        UsbhSignalSyncDeviceReset(HubFdo, Pdo, PortData, syncResetNts);

    } else {
        
        UsbhReleasePdoStateLock(HubFdo, sc);

        // PDO should not be marked 'present'
        // this flag also indicates a popup has been generated
        pdoExt->PdoFlags.EnumerationFailed = 1;

        UsbhBuildUnknownIds(HubFdo, Pdo);    

        pdoExt->EnumFailedBh =
            UsbhIncHubBusy(HubFdo, PortData->CurrentSc, HubFdo, UsbhBusyEnumFailed_Tag, RESUME_HUB);

        UsbhException(HubFdo, PortData->PortNumber, ExClass,
                      EnumFailData,
                      sizeof(struct _ENUM_FAIL_DATA),
                      NtStatus,
                      UsbdStatus,
                      SzFile,
                      Line,
                      FALSE);
#if DBG
        if (HubG.GlobalDebugBreakOnEnumFailure> 0) {
            // replicant failed to disappear, we may want to see why
            // it is so persistent.
            NT_ASSERTMSG("Enumeration Failure",FALSE);
        }
#endif


        // dump any serial number we may have managed to get hold of
        UsbhFreeID(&(pdoExt)->SerialNumberId);
        pdoExt->PdoFlags.SerialNumber = 0;

        // allow PDO to be deleted if necessary
        BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

        pdoExt->PdoPnpDeviceState |= PNP_DEVICE_FAILED;
        
        // report pdo to pnp for theerror case
        UsbhBusConnectPdo(HubFdo, PortData);

        connectionStatus = DeviceFailedEnumeration;
    }

    UsbhSetConnectionStatus(HubFdo, PortData, connectionStatus);

    return connectionStatus;
}


VOID
UsbhDropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Helper function, disconnects a device that has dropped off the bus.

    entry:
    Buslock is held...
    Pdo is latched..

Irql: PASSIVE

Arguments:

    PortData - port we are working with

Return Value:

    none
--*/
{
    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d UsbhDropDevice", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pDRP', PortData, PN(PortData));

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PN(PortData));


    // we should not return to wait_connect with a reset timeout pending
    // otherwise we will ignore if the device suddenly reappears
    UsbhAssert(HubFdo, PortData->ResetTimerPending == NULL);
    UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_Waiting);

    // signal pd event only in this case
    UsbhSignalDriverResetEvent(HubFdo, PortData, FALSE);

    // device fails enumeration
    UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PN(PortData));

    // allow PDO to be deleted
    BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

    Usbh_Disconnect_PdoEvent(HubFdo, Pdo, PdoEv_Disconnect, PortData->CurrentSc);

}



ULONG
UsbhPortIgnoreChange(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB device connection to a port

    Essentially a nop.  This is a legitimate case -- the device simply
    disappeared between the time we where notified and when we got around
    to processing the change.

    case

    PS_WAIT_CONNECT
        CONNECT_CHANGE
            ceosp
            00000
            // in this case we have a stale reset change -- queued before the connect
            // this is harmless since the enable bit is clear.
            10000

    PS_WAIT_CONNECT
        ENABLE_CHANGE
            ceosp
            00000

    PS_WAIT_CONNECT
        RESET_CHANGE
            ceosp
            00000

    PS_WAIT_CONNECT
        TIMEOUT_CHANGE
            ceosp
            00000


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    //test#25
    DbgTrace((UsbhDebugTrace, "#%d UsbhPortIgnoreChange", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pDRP', PortData, 0);

    // nothing to do here but wait for another connect

    return PS_WAIT_CONNECT;
}


ULONG
UsbhChangeTrap(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:



--*/
{
    NT_ASSERTMSG("UsbhChangeTrap", FALSE);
    return PS_WAIT_CONNECT;
}



ULONG
UsbhPortWaitForResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

   Handles Error cases on port resume path. Remoans in WIAT_CONNECT state until
   the resume event times out.


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d UsbhPortWaitForResumeTimeout", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRTO', PortData, 0);

    // nothing to do here but wait for another connect

    return PS_WAIT_CONNECT;
}


ULONG
UsbhPortDisconnect(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB device disconnection to a port

    We immediately remove the device handle so that the bus resources associated
    with the device are freed up.  The removal of the PDO cones later.

    case

    PS_WAIT_CONNECT
        CONNECT_CHANGE
            ceosp
            00001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    //Test #44, #53, #54, #70, #71, #72, #73, #74, #75, #76, #77, #78, #79
    // #80
    DbgTrace((UsbhDebugTrace,"#%d UsbhPortDisconnect", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pDIS', PortData, PortData->PortNumber);

    UsbhAssert(HubFdo, Pdo);
    pdoExt = PdoExt(Pdo);

    // delete the device handle and mark the pdo as removed.
    // note that a bad device handle(NULL) will cause devices request to
    // complete with DEVICE_GONE

    // removing the device handle immediately frees any resources
    // this device may have had -- such as bandwidth. DisconnectPdo may not
    // be able to delete right away.

    UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH5', TRUE);

    LOG(HubFdo, LOG_BUSF, 'dHD3', Pdo, 0);

    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
    UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);

    //
    // Cleanup our UDX/virtual machine registry keys.  This must
    // be called at passive level.  We must call this here since we
    // want to delete our keys before our PDO is actually removed.
    // Otherwise the remove may be delayed and the device could be
    // re-enumerated.  We only want to delete on true disconnect,
    // not cycle port
    //
    if(!pdoExt->PdoFlags.CyclePortPending){
        UsbhDeleteUxdSettings(HubFdo,
                              Pdo,
                              UXD_EVENT_DISCONNECT);
    }

    // allow PDO to be deleted
    BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

    Usbh_Disconnect_PdoEvent(HubFdo, Pdo, PdoEv_Disconnect, PortData->CurrentSc);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhPortConnect(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB device connection to a port

    case

    PS_WAIT_CONNECT
        CONNECT_CHANGE
            ceosp
            10000


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    NTSTATUS nts;
    PPCQ_TIMEOUT_OBJECT tmoObj = NULL;
    ULONGLONG StartTime, CurrentTime, debounceWait, LastTime, ConnectCount=0; 
    PSSH_BUSY_HANDLE bh;

    UsbhAssertPassive();
    //test #41, #42, #43, #44
    ASSERT_PORT_DATA(HubFdo, PortData);
    PortData->EnumRetries = 0;
    PortData->PD_Flags.ul = 0;
    // we are connecting so there should be no PDO
    UsbhAssert(HubFdo, Pdo == NULL);

    DbgTrace((UsbhDebugTrace, "#%d UsbhPortConnect", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pCON', PortData, 0);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_START, PN(PortData), STATUS_SUCCESS);
    
    // reset the device connection status
    UsbhSetConnectionStatus(HubFdo, PortData, NoDeviceConnected);

    bh = UsbhIncHubBusy(HubFdo, PDC(PortData), HubFdo, UsbhBusyPortConnect_Tag, RESUME_HUB);

    // pre-allocate the PDO before we take the enum lock
    nts = UsbhCreatePdo(HubFdo, PortData);

    if (bh) {
        UsbhDecHubBusy(HubFdo, PDC(PortData), bh);
    }

    if (!NT_SUCCESS(nts)) {
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_CREATE_PDO_FAILURE, PN(PortData), nts);
        
        // device fails enumeration
        HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, CreatePdoFailure_Popup,
                      nts, 0xFFFFFFFF, NULL, 0);

        // since we don't have a Pdo we cannot report it to
        // PnP
        UsbhSetConnectionStatus(HubFdo, PortData, DeviceGeneralFailure);

        return PS_WAIT_CONNECT;
    }

    // take the bus lock
    UsbhAcquireEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

    // 100 ms for port stabilization (debounce interval)
    // 7.1.7.3
    // specification indicates that 'system software' ie us has wait 100ms
    // after the port connect as a debounce interval before driving reset.
    // if the connection drops during this time the 'debounce' timer is reset.
    //
    // if the connection cycles before this point another connect change
    // will be queued behind this one and we will see the following  state
    // change:
    //
    //  PS_ENUMERATE_1
    //      CONNECT_CHANGE
    //          ceosp
    //          10001  0x11
    //


    // Begin Debounce
    //reset the debounce counter
    InterlockedExchange(&PortData->ConnectCount, 0);
    debounceWait = 0;
    
    CurrentTime = LastTime = StartTime = KeQueryInterruptTime();

    do {
        if ((CurrentTime - StartTime) > MILLISECONDS_TO_100_NS_UNITS(200)) {

            // When the system is under stress the wait may actually be several hundred
            // milliseconds. In this case we allow another 100ms debounce.  
            if (ConnectCount > 1) {
                // timeout on the debounce, return to wait connect, we will service
                // one on the queued connect changes to hand a restart.
    
                UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);
    
                UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_DEBOUNCE_FAILURE, PN(PortData), STATUS_UNSUCCESSFUL);

                Usbh_ErrorDisconnect_PdoEvent(HubFdo,  (PDEVICE_OBJECT) PortData->PnpPdo, PDC(PortData));
                
                LOG(HubFdo, LOG_BUSF, 'DBto', PortData, PN(PortData));
                DbgTrace((UsbhDebugTrace, "#%d debounce timeout", PN(PortData)));

                return PS_WAIT_CONNECT;
            }
        }

        UsbhWait(HubFdo, 20);
        
        CurrentTime = KeQueryInterruptTime();
        debounceWait += (CurrentTime - LastTime);
        LastTime = CurrentTime;

        if (InterlockedExchange(&PortData->ConnectCount, 0)) {
            // connect change occurred, reset counter
            ConnectCount++;
            debounceWait = 0;
            continue;
        }

    } while (debounceWait < MILLISECONDS_TO_100_NS_UNITS(100));

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_DEBOUNCE_COMPLETE, PN(PortData), STATUS_SUCCESS);

    nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

    // we have at least 100ms with a stable connection, attempt reset

    if (NT_SUCCESS(nts)) {

        UsbhResetPort(HubFdo, PortData, tmoObj);
        UsbhSetConnectionStatus(HubFdo, PortData, DeviceEnumerating);

        return PS_ENUMERATE_1;

    } else {
        if (tmoObj != NULL) {
            UsbhFreePool(tmoObj);
        }
        UsbhDisablePort(HubFdo, PortData);
        UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

        Usbh_ErrorDisconnect_PdoEvent(HubFdo,  (PDEVICE_OBJECT) PortData->PnpPdo, PDC(PortData));

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_CREATE_PDO_FAILURE, PN(PortData), nts);
        
        // device fails enumeration
        HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, CreatePdoFailure_Popup,
                      nts, 0xFFFFFFFF, NULL, 0);

        // since we don't have a Pdo we cannot report it to
        // PnP
        UsbhSetConnectionStatus(HubFdo, PortData, DeviceGeneralFailure);

        return PS_WAIT_CONNECT;
    }

}


ULONG
UsbhReset1Timeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle Timeout of usb port reset waiting to enumerate

    Bus lock is held

    case

    PS_ENUMERATE1
        TIMEOUT_CHANGE
            ceosp
            10001

    * we have the PDO and the device is still connected

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    ENUM_FAIL_DATA efd;
    NTSTATUS nts;
    PPCQ_TIMEOUT_OBJECT tmoObj = NULL;

    UsbhAssertPassive();
    //test #41, #42, #43

    DbgTrace((UsbhDebugTrace, "#%d UsbhReset1Timeout", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRST', PortData, PortData->EnumRetries);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssert(HubFdo, PortData->ResetTimerPending == NULL);
    UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_Waiting);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    PortData->TmResetEnd = Usbh_UsbdReadFrameCounter(HubFdo);
    UsbhSetEnumFailData(HubFdo, PortData, Pdo, &efd);

    // allocate another context for a retry
    nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

    if (PortData->EnumRetries < USBH_MAX_ENUM_RETRIES && NT_SUCCESS(nts)) {
        PortData->EnumRetries++;

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RETRY, PN(PortData), STATUS_UNSUCCESSFUL);

        // our old friend reset timeout -- use exception to track some
        // data on these

        HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, PnpErr_Reset1Failure,
                             nts, 0xFFFFFFFF, &efd,
                             sizeof(struct _ENUM_FAIL_DATA));

        UsbhResetPort(HubFdo, PortData, tmoObj);

        return PS_ENUMERATE_1;
    } else {
        if (tmoObj != NULL) {
            UsbhFreePool(tmoObj);
        }

        PdoExt(Pdo)->EnumFailureMsgId = USBENUM_PORT_RESET_FAILURE;

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_TIMEOUT, PN(PortData), STATUS_UNSUCCESSFUL);

        // device fails enumeration
        UsbhSetEnumerationFailed(HubFdo, PortData, Pdo,
                Reset1Failure_Popup, &efd, nts, 0xFFFFFFFF,
                USBFILE, __LINE__);

        return PS_WAIT_CONNECT;
    }
}


ULONG
UsbhReset1DebounceError(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    case(s)

    PS_ENUMERATE1
        CONNECT_CHANGE
            ceosp
            11001

    This occurs when a connect change is queued after the debounce interval but
    before the reset is initiated.  This can indicate a potentially unstable
    connection so we generate a hardware exception for this case.  However we
    continue with attempt to enumerate the device.

    We ack the change but continue to wait for the reset.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();
    // Pnpdtest stress will hit this path

    DbgTrace((UsbhDebugTrace,"#%d UsbhReset1DebounceError", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pBNe', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);


    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, DebounceFailure,
                         0xFFFFFFFF, 0xFFFFFFFF, NULL, 0);

    return PS_ENUMERATE_1;
}


ULONG
UsbhReset1Debounce(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    case(s)

    PS_ENUMERATE1
        CONNECT_CHANGE
            ceosp
            10001

    This occurs when a connect change is queued during the debounce interval.
    We ack the change but continue to wait for the reset.

    If debounce is disabled then we treat this like a cycle.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    USB_HUB_PORT_EVENT pc;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d UsbhReset1Debounce", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pBNC', PortData, 0);

    DbgTrace((UsbhDebugTrace, "Debouncing port %d\n", PortData->PortNumber));
    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    // if this is a driver initiated reset then we won't debounce -- we will just fail
    if (PortData->PD_Flags.NoDebounce) {
        //Test #100

        // fail and return to WAIT_CONNECT -- treat just like a cycle

        LOG(HubFdo, LOG_BUSF, 'noDB', PortData, PortData->PortNumber);
        UsbhCancelResetTimeout(HubFdo, PortData, CANCEL_RESET_IN_PROGRESS);

        // DriverResetEvent must be signaled
        UsbhSignalSyncDeviceReset(HubFdo, Pdo, PortData, STATUS_UNSUCCESSFUL);

                // since enable bit is clear this should be ok
        UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

        // first perform the 'disconnect' of the current device
        UsbhPortDisconnect(HubFdo, PortData, Pdo);

        // pdo is lost at this point, process as if a new connect
        // the pdo is unlatched and may be deleted

        // queue an artificial change for the new 'connect'
        pc.ul = 0;
        pc.ConnectChange = 1;

        UsbhPCE_QueueChange(HubFdo, PortData->PortNumber, pc, PortData->CurrentSc);

        return PS_WAIT_CONNECT;

    } else {
        // timeout will handle retry
        //
        //#100, #101
        return PS_ENUMERATE_1;
    }
}


ULONG
UsbhReset1DropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle Timeout of usb port reset waiting to enumerate

    Bus lock is held

    * in this case the timeout has fired before any connect change
    * notification

    PS_ENUMERATE1
        CONNECT_CHANGE
            ceosp
            00001

    * in the case we were notified of the disconnect before the timeout
    * fired

    PS_ENUMERATE1
        RESET_CHANGE
            ceosp
            00001
            00101
            11011

   * in the case we were notified of the reset completing but the device was
   * dropped just after

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();
    //test #41, #42, #43

    DbgTrace((UsbhDebugTrace, "#%d UsbhReset1DropDevice", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRSd', PortData, 0);

    // any reset that was in progress should have been aborted by hw
    // but in the case of software emulated root hubs the HC miniport may
    // not realize this so it us safer to treat the reset as 'in-progress'
    UsbhCancelResetTimeout(HubFdo, PortData, CANCEL_RESET_IN_PROGRESS);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_FAILURE, PN(PortData), STATUS_UNSUCCESSFUL);

    UsbhDropDevice(HubFdo, PortData, Pdo);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhReset1DropDeviceTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle Timeout of usb port reset waiting to enumerate

    Bus lock is held

    * we have a PDO but the device dropped off the bus during the
    * reset, this can happen if the device is high speed.

    case(s)

    PS_ENUMERATE1
        TIMEOUT_CHANGE
            ceosp
            00001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();
    //test #46

    DbgTrace((UsbhDebugTrace, "#%d UsbhReset1DropDevice", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRto', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_TIMEOUT, PN(PortData), STATUS_UNSUCCESSFUL);

    UsbhDropDevice(HubFdo, PortData, Pdo);

    return PS_WAIT_CONNECT;
}



ULONG
UsbhHardErrorReset1BadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine handles completely bogus cases where the hub has probably lost its
    brains.
    The test code (initfail 36) simulates this by ignoring the reset change ack
    and continually asserting a reset change. Eventually we see enable bits set while
    in wait connect.

    This routine halts all processing in the hub pnp engine until the hub can be reset.

    PS_ENUMERATE1
        RESET_CHANGE
            ceosp
            01001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    //DbgTrace((UsbhDebugTrace,"%!FUNC!\n", PortData));
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pBDE', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhCancelResetTimeout(HubFdo, PortData, USB_RESET_COMPLETED);

    // if we hold the bus lock release it since no more enumeration will occur after this
    // point
    if (hubExt->UsbEnumLockPort == PortData->PortNumber) {
        UsbhAssertBusLock(HubFdo, PortData->PortNumber);

        UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);
    }

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_BAD_ENABLE, PN(PortData), STATUS_UNSUCCESSFUL);

    // remain in current state and wait for reset

    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_HardError_Reset,
                          0xFFFFFFFF, 0xFFFFFFFF, PortData,
                          sizeof(struct _HUB_PORT_DATA));

    return PS_HARDERROR;
}


ULONG
UsbhHardErrorInvalidData(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine handles completely bogus cases where the hub has probably lost its
    brains.
    The test code #60 simulates this by ignoring the reset change ack
    and continually asserting a reset change. Eventually we see enable bits set while
    in wait connect.
    I think this is a pretty degenerate case so I handle it with a separate function that
    does not try to cancel a pending reset. The test code is trying to fake us out here
    in PS_WAIT_CONNECT no reset should be pending.

    This routine halts all processing in the hub pnp engine until the hub can be reset.

    PS_WAIT_CONNECT
        RESET_CHANGE
            ceosp
            11001
            10001


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();
    //Test #55, #60, #64

    hubExt = FdoExt(HubFdo);

    //DbgTrace((UsbhDebugTrace,"%!FUNC!\n", PortData));
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pINV', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    // we do not cancel the timeout since none should be pending in PS_WAIT_CONNECT


    // if we hold the bus lock release it since no more enumeration will occur after this point
    UsbhReleaseEnumBusLockEx(HubFdo, PortData->CurrentSc, PortData->PortNumber, FALSE);

    // remain in current state and wait for reset

    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_HardError_Reset,
                          0xFFFFFFFF, 0xFFFFFFFF, PortData,
                          sizeof(struct _HUB_PORT_DATA));

    return PS_HARDERROR;
}


ULONG
UsbhReset1TimeoutBadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:



    PS_ENUMERATE1
        TIMEOUT_CHANGE
            ceosp
            11001

    The reset has timed out but the port status is enabled.  This is a
    hardware error of some kind -- it means the hub never indicated a reset
    change for the port (reset time is <<<= (much less) than the timeout.

    It's not clear if this problem happens in real life but we do simulate it
    in our component test so we have code to handle it.

    We treat this the same as a regular timeout except we disable the port
    before retrying.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();

    // Test #49

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRbe', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);


    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_Reset1BadEnable,
                        0xFFFFFFFF, 0xFFFFFFFF, NULL, 0);
    //
    //
    UsbhDisablePort(HubFdo, PortData);

    return UsbhReset1Timeout(HubFdo, PortData, Pdo);
}


ULONG
UsbhReset2TimeoutBadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:



    PS_ENUMERATE2
        TIMEOUT_CHANGE
            ceosp
            11001

   Same as reset 1 timeout case but this is on the second reset.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRbe', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);


    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_Reset2BadEnable,
                        0xFFFFFFFF, 0xFFFFFFFF, NULL, 0);
    //
    //
    UsbhDisablePort(HubFdo, PortData);

    return UsbhReset2Timeout(HubFdo, PortData, Pdo);
}



ULONG
UsbhReset2DropDeviceTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle Timeout of usb port reset waiting to enumerate

    Bus lock is held

    * we have a PDO but the device dropped off the bus during the
    * second reset.

    case(s)

    PS_ENUMERATE2
        TIMEOUT_CHANGE
            ceosp
            00001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    UsbhAssertPassive();
TEST_TRAP();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRd2', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);

    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_TIMEOUT, PN(PortData), STATUS_UNSUCCESSFUL);

    UsbhDropDevice(HubFdo, PortData, Pdo);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhReset2Timeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle Timeout of usb port reset waiting to enumerate (reset 2)

    Bus lock is held

    case(s)

    PS_ENUMERATE2
        TIMEOUT_CHANGE
            ceosp
            10001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    ENUM_FAIL_DATA efd;
    PPCQ_TIMEOUT_OBJECT tmoObj = NULL;
    NTSTATUS nts;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRR2', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    PortData->TmResetEnd = Usbh_UsbdReadFrameCounter(HubFdo);
    UsbhSetEnumFailData(HubFdo, PortData, Pdo, &efd);

    // allocate another context for a retry
    nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

    if (PortData->EnumRetries < USBH_MAX_ENUM_RETRIES && NT_SUCCESS(nts)) {

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RETRY, PN(PortData), STATUS_UNSUCCESSFUL);

        PortData->EnumRetries++;

        // initiate a reset from the enumerate_1 state
        // we need to dump the current dev handle
        UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH9', TRUE);

        UsbhResetPort(HubFdo, PortData, tmoObj);
        return PS_ENUMERATE_1;

    } else {
        if (tmoObj != NULL) {
            UsbhFreePool(tmoObj);
        }

        PdoExt(Pdo)->EnumFailureMsgId = USBENUM_PORT_RESET_FAILURE;
        
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_TIMEOUT, PN(PortData), STATUS_UNSUCCESSFUL);

        // device fails enumeration
        UsbhSetEnumerationFailed(HubFdo, PortData, Pdo,
                Reset2Failure_Popup, &efd, 0xFFFFFFFF, 0xFFFFFFFF,
                USBFILE, __LINE__);

        return PS_WAIT_CONNECT;
    }
}


ULONG
UsbhReset1Complete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    reset complete, get the device descriptor then attempt a second reset

    Bus lock is held

    case

    PS_ENUMERATE1
        RESET_CHANGE
            ceosp
            11001

    * device is connected and port is enabled

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    NTSTATUS nts;
    ENUM_FAIL_DATA efd;
    PPCQ_TIMEOUT_OBJECT tmoObj = NULL;

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pR1c', PortData, 0);

    UsbhAssertPassive();
    // test #44
    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    // reset should not be in progress at this point
    UsbhCancelResetTimeout(HubFdo, PortData, USB_RESET_COMPLETED);
    //UsbhAssert(HubFdo, PortData->PD_Flags.ResetInProgress == 0);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET1_COMPLETE, PN(PortData), STATUS_SUCCESS);

    PortData->TmResetEnd = Usbh_UsbdReadFrameCounter(HubFdo);
    UsbhSetEnumFailData(HubFdo, PortData, Pdo, &efd);

    // Wait 10 ms after reset according to section 7.1.4.3 of the
    // USB specification.
    // 
    // In Windows 7/8 without high resolution timers, a 10ms timer can expire in 10 + 16 = 26ms (which can take
    // upto 32 ms in reality) as 16 additional milliseconds were added to avoid early expiration of timers.
    //
    // In Windows 8.1 the USB stack moved to using the high resolution timer for these delays, so the actual delay
    // was reduced to the spec required 10ms and subsequently to 15ms as it was found that some devices will
    // occasionally fail the first request after reset if the delay after the first reset is only 10ms.
    //
    // There is now evidence that some devices require more time to initialize/stabilize after USB reset to respond
    // to the device descriptor requests (works for 32 ms, fails for 15ms). Hence this value is being changed to
    // 32ms for Windows 10 (Redstone 1 release).
    //
    // When the enumeration is being retried, use an extended delay of 100ms in case it allows some devices to
    // initialize properly and be able to respond to control transfers.
    //
    if (PortData->EnumRetries) {

        UsbhWait(HubFdo, 100);

    } else {

        UsbhWait(HubFdo, 32);
    }
    
    nts = UsbhCreateDevice(HubFdo,
                           Pdo,
                           PortData->CurrentPortStatus.us,
                           PortData->PortNumber);

    if (NT_SUCCESS(nts)) {
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_ENUM_CREATE_DEVICE_COMPLETE, PN(PortData), STATUS_SUCCESS);

        //
        // Skip the second reset for high-speed devices, if we aren't retrying enumeration and not running WinPE.
        //
        if ((PortData->CurrentPortStatus.HighSpeed) && 
            (PortData->EnumRetries == 0) &&
            (HubG.GlobalDisableFastEnumeration == 0) &&
            (FdoExt(HubFdo)->FdoFlags.WinPE == 0)) {

            return UsbhReset2Complete(HubFdo, 
                                      PortData,
                                      Pdo);
        }

        nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

        if (NT_SUCCESS(nts)) {
            // second reset after getting the device descriptor
            UsbhResetPort(HubFdo, PortData, tmoObj);
            return PS_ENUMERATE_2;
        }
    }

    if (tmoObj == NULL) {
        nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);
    }

    // failed first attempt to create device
    if (PortData->EnumRetries < USBH_MAX_ENUM_RETRIES && tmoObj != NULL) {

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RETRY, PN(PortData), nts);

        // it is tradition to give the device this time to recover
        UsbhWait(HubFdo, 500);
        PortData->EnumRetries++;

        // reset the TT for address 0
        UsbhClearTt(HubFdo, PortData, 0);
        UsbhResetPort(HubFdo, PortData, tmoObj);

        return PS_ENUMERATE_1;
    } else {

        if (tmoObj != NULL) {
            UsbhFreePool(tmoObj);
        }

        UsbhSetEnumerationFailed(HubFdo, PortData, Pdo,
                    CreateDeviceFailure_Popup, &efd, nts, 0xFFFFFFFF,
                    USBFILE, __LINE__);

    }

    return PS_WAIT_CONNECT;

}


ULONG
UsbhReset2CycleDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    device dropped off and reattached to the bus during the second reset

    Bus lock is held

    case

    PS_ENUMERATE2
        CONNECT_CHANGE
            ceosp
            10001


    device has dropped off the bus.

    This could be caused by a bad or worn out connector or the device may have
    just been removed and reattached during the reset or just before the reset.

    In any case this indicates an unstable connection -- the reset may have completed
    but the enable status was lost.

    Since the device a device still shows connected we process this disconnect reconnect
    or 'cycle'. We don't debounce here since we already performed this ritual on initial
    connect.


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    //test #101
    UsbhCancelResetTimeout(HubFdo, PortData, CANCEL_RESET_IN_PROGRESS);
    //
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);
    UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

    return UsbhPortCycle(HubFdo, PortData, Pdo);
}


ULONG
UsbhReset2ErrorWaitForTimeoutDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    device dropped off/on the bus during the second reset

    Bus lock is held

    case

    PS_ENUMERATE2

        ENABLE_CHANGE
            ceosp
            10001

        RESET_CHANGE
            ceosp
            ** device dropped during the reset.
            10001

    For these cases we wait for the next change event to take action -- the reset will
    eventually timeout.


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'e2ER', PortData, 0);

    UsbhAssertPassive();

    return PS_ENUMERATE_2;
}

ULONG
UsbhReset2DropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    device dropped off the bus during the second reset

    Bus lock is held

    case

    PS_ENUMERATE2
        CONNECT_CHANGE
            ceosp
            00001

        ENABLE_CHANGE
            ceosp
            00001

        RESET_CHANGE
            ceosp
            ** device dropped during the reset
            00001



    device has dropped off the bus.

    This could be caused by a bad or worn out connector or the device may have
    just been removed during the reset.


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'e2DR', PortData, 0);

    UsbhAssertPassive();
    //#97

    // if device dropped any reset in progress should have stopped
    UsbhCancelResetTimeout(HubFdo, PortData, USB_RESET_COMPLETED);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET_FAILURE, PN(PortData), STATUS_UNSUCCESSFUL);
    
    UsbhDropDevice(HubFdo, PortData, Pdo);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhReset1EnIgnore(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Reset1 Enable change ignore: device dropped etc

    Bus lock is held

    case

    PS_ENUMERATE1

        ENABLE_CHANGE
            ceosp
            00001

    device has dropped off the bus and an enable change is reported.  The
    device may have dropped due to babble.

    We ignore the change an wait for the reset to timeout or a connect change
    to be reported.

            10001

    Enable change occurred and enable bit is set. Either hub has indicated the
    enable change before reporting the enable bit as set or the device
    dropped prior the reset change message.  In either case we pass the
    message on to the next link in the chain. If it is the first case the reset
    change will follow and we'll see the enable bit, if the second case the
    reset will timeout and we will retry.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{

    DbgTrace((UsbhDebugTrace, "#%d UsbhReset1EnIgnore", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'e1DR', PortData, 0);
    //Test #65

    return PS_ENUMERATE_1;
}


ULONG
UsbhReset2Complete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    second reset complete, assign a USB address

    Bus lock is held

    case

    PS_ENUMERATE2
        RESET_CHANGE
            ceosp
            11001

    * device is connected and port is enabled

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    ULONG ps;
    HUB_EXCEPTION_CLASS exClass;
    ENUM_FAIL_DATA efd;
    PDEVICE_EXTENSION_HUB hubExt;

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pR2c', PortData, 0);

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();
    // Test #44, #53, #60, #71, #76, #77, #78, #79, #80
    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssert(HubFdo, Pdo);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhCancelResetTimeout(HubFdo, PortData, USB_RESET_COMPLETED);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RESET2_COMPLETE, PN(PortData), STATUS_SUCCESS);

    PortData->TmResetEnd = Usbh_UsbdReadFrameCounter(HubFdo);
    UsbhSetEnumFailData(HubFdo, PortData, Pdo, &efd);

    LOG(HubFdo, LOG_BUSF, 'en21', PortData, PortData->EnumRetries);
    if (PortData->EnumRetries) {
        // if we retried then wait an additional 100 ms.  This is one
        // of those hacks that "we don't do".
        UsbhWait(HubFdo, 100);
    } else {
        // otherwise
        // Wait is 10 ms after reset according to section 7.1.4.3 of the USB
        // specification before sending the next command.
        //
        // We wait 15ms to allow for some devices that fail if only allowed 10ms.
        UsbhWait(HubFdo, 15);
    }

    // set error class to setup or init
    exClass = InitializeDeviceFailure_Popup;
    nts = UsbhInitializeDevice(HubFdo,
                               Pdo,
                               PortData);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_INITIALIZE_DEVICE_COMPLETE, PN(PortData), STATUS_SUCCESS);

    LOG(HubFdo, LOG_BUSF, 'en22', PortData->PortNumber, nts);
    if (NT_SUCCESS(nts)) {
        ULONG dupCheckCount = 0;
        BOOLEAN skipDuplicateDetection = FALSE;

        exClass = SetupDeviceFailure_Popup;
        // device now has an address
        UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

        // don't unlatch -- we just want to peek at the PDO state
        // BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);
        UsbhAcquirePdoStateLock(HubFdo, PortData->CurrentSc, PdoEv_Internal);
        if (Pdo && GET_PDO_HWPNPSTATE(Pdo) == Pdo_PnpRefHwPresent) {
            // skip setup if the PDO is already registered, just use the
            // information we had from the original enumeration.
            // *this happens during driver initiated reset
            LOG(HubFdo, LOG_BUSF, 'stDk', PortData, PortData->PortNumber);
            nts = STATUS_SUCCESS;
    
            // No need to look for a duplicate PDO if this is a driver
            // initiated reset.
            skipDuplicateDetection = TRUE;

            UsbhReleasePdoStateLock(HubFdo, PortData->CurrentSc);
        } else {
            UsbhReleasePdoStateLock(HubFdo, PortData->CurrentSc);
            nts = UsbhSetupDevice(HubFdo, Pdo, &usbdStatus);
            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_SETUP_DEVICE_COMPLETE, PN(PortData), nts);
            LOG(HubFdo, LOG_BUSF, 'en23', PortData->PortNumber, nts);
        }

        /* Replicant spin loop */
        do {

            USBD_CHILD_STATUS childStatus;
            PDEVICE_EXTENSION_PDO pdoExt;
            KIRQL irql;

            if (NT_SUCCESS(nts)) {
                nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);
                LOG(HubFdo, LOG_BUSF, 'en24', PortData->PortNumber, nts);
            }

            LOG(HubFdo, LOG_BUSF, 'stDn', PortData, PortData->PortNumber);
            if (NT_ERROR(nts) && Usb_Disconnected(nts)) {

                // hub is disconnected -- go directly to pause state
                // pnpdtest will hit this path
                LOG(HubFdo, LOG_BUSF, 'en2D', PortData->PortNumber, nts);
                // need to delete this PDO

                BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

                UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_HUB_DISCONNECT, PN(PortData), nts);
                
                Usbh_Disconnect_PdoEvent(HubFdo, Pdo, PdoEv_Disconnect, PortData->CurrentSc);

                return PS_HARDERROR;
            }

            if (NT_ERROR(nts)) {

                LOG(HubFdo, LOG_BUSF, 'enER', PortData->PortNumber, nts);
                UsbhDisablePort(HubFdo, PortData);
                // exception is generated if we don't retry
                //TEST #77, #78

                // exit loop, retry
                break;
            }

            pdoExt = PdoExt(Pdo);

            // Replicants are only interesting if they have unique serial numbers
            // or are new PDOs
            if ((!pdoExt->PdoFlags.SerialNumber) || (skipDuplicateDetection == TRUE)){
                break;
            }
            
            // check for a duplicate serial number or replicants.
            // we need to determine if its a true duplicate or some kind of PnP race condition.
            dupCheckCount++;
            
            childStatus = USBD_CHILD_STATUS_FAILURE;

            KeAcquireSpinLock(&pdoExt->GlobalListStateLock, &irql);
            
            // insert into the global list if we are allowed to
            if (pdoExt->GlobalListState == PdoNotInGlobalList) {
                childStatus = UsbhInsertDeviceIntoGlobalList(HubFdo, Pdo);
            
                if (childStatus == USBD_CHILD_STATUS_INSERTED) {
                    pdoExt->GlobalListState = PdoInGlobalList;
                }
            } 

            KeReleaseSpinLock(&pdoExt->GlobalListStateLock, irql);

            if (childStatus == USBD_CHILD_STATUS_DUPLICATE_FOUND) {

                UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_DUPE_SERIAL_NUMBER, PN(PortData), STATUS_UNSUCCESSFUL);

                // true duplicate serial number, in the case we throw an exception to
                // note it then act as if the device has no serial number.
                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, DuplicateSerialNumber,
                                nts, 0, pdoExt->SerialNumberId.Buffer,
                                pdoExt->SerialNumberId.LengthInBytes);

                // dump the serial number on this instance
                UsbhFreeID(&pdoExt->SerialNumberId);
                pdoExt->PdoFlags.SerialNumber = 0;
                // exit loop
                break;

            }

            if (childStatus == USBD_CHILD_STATUS_DUPLICATE_PENDING_REMOVAL) {

                ULONG q_stopped;

                //
                // we must stall reporting this PDO
                // We have detected a possible replicant. The replicant will be deleted soon so
                // we want to delay the arrival of the new PDO to avoid conflicts with PnP.
                // This is not common but does happen under certain error or stress conditions.
                // We also want to detect if the hub is removed during this process and bail
                // out.


                UsbhKdPrint((1, ">Warning: Replicant detected PDO %p port #%d\n",
                    Pdo, PortData->PortNumber));


                //state = UsbhLockPcq(HubFdo, PortData, PCE_wBusy, &irql);
                //if (state == Pcq_Stop_Wait) {
                //    // attempt an early exit if the queue state changes,
                //    // report an error so that this enumeration is aborted
                //    // and retried
                //    nts = STATUS_UNSUCCESSFUL;
                //    PortData->EnumRetries = USBH_MAX_ENUM_RETRIES;
                //    UsbhUnlockPcq(HubFdo, PortData, irql);
                //    break;
                //}
                //UsbhUnlockPcq(HubFdo, PortData, irql);

                q_stopped = UsbhPCE_Check(HubFdo, PortData, PortData->CurrentSc);
                if (q_stopped) {
                    nts = STATUS_UNSUCCESSFUL;
                    PortData->EnumRetries = USBH_MAX_ENUM_RETRIES;
                    break;
                }

                UsbhWait(HubFdo, HubG.GlobalReplicantWaitTime);
                if (dupCheckCount > HubG.GlobalMaxReplicantRetry) {

                    // if we exceed max tries we break the wait and retry enumeration
                    nts = STATUS_UNSUCCESSFUL;

                    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_REPLICANT_TIMEOUT, PN(PortData), STATUS_UNSUCCESSFUL);

                    HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, ReplicantDetected,
                                nts, 0, NULL, 0);

                    PdoExt(Pdo)->EnumFailureMsgId = USBENUM_DUPLICATE_INSTANCE;
                    
                    UsbhKdPrint((1, "Replicant failed to disappear %p\n", Pdo));
#if DBG
                    if (HubG.GlobalDebugBreakOnReplicant> 0) {
                        // replicant failed to disappear, we may want to see why
                        // it is so persistent.
                        NT_ASSERTMSG("Replicant failed to disappear", FALSE);
                    }
#endif
                    // exit loop, retry

                    break;
                }
    
            } else if (childStatus == USBD_CHILD_STATUS_FAILURE) {
                // Failed to add device to list for other than a duplicate device detection
                // Retry enumeration
                nts = STATUS_UNSUCCESSFUL;
                break;

            } else {
                //  Successfully added the device to the global list
                UsbhAssert(HubFdo, PortData->ConnectionStatus >= DeviceConnected);

                // exit loop
                // connect the pdo...
                break;
            }
        } WHILE (1);
    }

    if (NT_ERROR(nts)) {

        PPCQ_TIMEOUT_OBJECT tmoObj = NULL;

        LOG(HubFdo, LOG_BUSF, 'sef1', PortData->PortNumber, nts);

        nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

        if (PortData->EnumRetries < USBH_MAX_ENUM_RETRIES && NT_SUCCESS(nts)) {

            LOG(HubFdo, LOG_BUSF, 'sef2', PortData->EnumRetries, nts);

            UsbhDisablePort(HubFdo, PortData);
            PortData->EnumRetries++;
            // failed attempt to init the device but within grace period.
            // recycle reset again and try to setup the device

            // if we released the bus lock we need to reacquire before driving reset
            UsbhAcquireEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

            // dump the current device handle if we have one

            UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDHa', TRUE);

            // unlatch the pdo before driving reset
            BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_RETRY, PN(PortData), nts);

            UsbhResetPort(HubFdo, PortData, tmoObj);

            // take us back thru reset 1
            ps = PS_ENUMERATE_1;
            
        } else {
            if (tmoObj != NULL) {
                UsbhFreePool(tmoObj);
            }

            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_COMPLETE, PN(PortData), nts);

            // failed attempt to init/setup the device
            UsbhSetEnumerationFailed(HubFdo, PortData, Pdo,
                    exClass, &efd, nts, 0xFFFFFFFF,
                    USBFILE, __LINE__);

            ps = PS_WAIT_CONNECT;
        }
    } else {
        PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);

        // If no ContainerID in firmware, create one using the serial number
        if (!pdoExt->ContainerIDValid &&  pdoExt->PdoFlags.SerialNumber && !pdoExt->PortAttributes.DeviceNotRemovable) {
            UsbhBuildContainerID(HubFdo, Pdo);
        }
    
        // If we have a valid Container ID, inform the miniport if it's on the root hub
        if ((pdoExt->ContainerIDValid) && (hubExt->FdoFlags.HubIsRoot)) {
            UsbhBusIf_SetContainerIdForPort(HubFdo,
                                            pdoExt->PortNumber,
                                            &pdoExt->ContainerID );
        }

        // If no ContainerID in firmware and no serial number exists,
        // see if the miniport can give us one
        if ((!pdoExt->ContainerIDValid) && (hubExt->FdoFlags.HubIsRoot)) {
            nts = UsbhBusIf_GetContainerIdForPort(HubFdo,
                                                  pdoExt->PortNumber,
                                                  &pdoExt->ContainerID );

            if NT_SUCCESS(nts) {
                pdoExt->ContainerIDValid = TRUE;
            }
            nts = STATUS_SUCCESS;
        }
        
        // unlatch the pdo before we acquire the lock and change state to connected
        BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_COMPLETE, PN(PortData), nts);

        PdoExt(Pdo)->EnumFailureMsgId = 0;

        UsbhBusConnectPdo(HubFdo, PortData);

        ps = PS_WAIT_CONNECT;
    }
    return ps;

}


ULONG
UsbhReset1CompleteNotEnabled(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    reset complete but port is not enabled, this is the same as a
    failure

    Bus lock is held

    case

    PS_ENUMERATE1
        RESET_CHANGE
            ceosp
            10001

    * device is connected reset finished but port is not in the enabled state

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRne', PortData, 0);
    //Test #54, #65

    // cancel any pending timeout, reset change means the hub portion is complete
    // UsbhCancelResetTimeout(HubFdo, PortData, USB_RESET_COMPLETED);

    //return UsbhReset1Timeout(HubFdo, PortData, Pdo);

    // allow the reset to timeout on its own here,
    // this gives the device more breathing room before then next reset attempt
    // in the event the port itself is not quite stable.
    //
    return PS_ENUMERATE_1;

}


ULONG
UsbhReset1CompleteOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    reset complete but port is not enabled due to overcurrent, this is the same as a
    failure

    Bus lock is held

    case

    PS_ENUMERATE1
        RESET_CHANGE
            ceosp
            10101   (0x15)

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRne', PortData, 0);
    //Test #143

    TEST_TRAP();
    // allow the reset to timeout on its own here,
    // this gives the device more breathing room before then next reset attempt
    // in the event the port itself is not quite stable.
    //
    return PS_ENUMERATE_1;

}


ULONG
UsbhPortCycle(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB device cycle on a port.

    Enable bit clear and connect change asserted, the device was unplugged and
    re-plugged rapidly (Percy Test) or briefly dropped off the bus.

    This also occurs if the client driver or user mode app cycles the port.

    case

    PS_WAIT_CONNECT
        CONNECT_CHANGE
            ceosp
            10001


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    //#93
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pCYC', PortData, Pdo);

    // should not break the boot device
    if (pdoExt->IsBootDevice) {
        //
        // If no hub reset is in progress, this is a spurious connect change
        // for the boot device.  We will have to trigger a downstream hub reset
        // to recover from this.
        //
        if (hubExt->HardResetState == HReset_WaitReady) {
            if (pdoExt->PendingResetNotificationRequestIrp != NULL) {
                UsbhCompleteIrp(pdoExt->PendingResetNotificationRequestIrp,STATUS_SUCCESS);
            }
        }
            
        return PS_WAIT_CONNECT;
    }


    //
    // Our UXD handling needs to know that this disconnect is part of a cycle, not
    // a real disconnect
    //
    pdoExt->PdoFlags.CyclePortPending = 1;

    // first perform the 'disconnect' of the current device
    UsbhPortDisconnect(HubFdo, PortData, Pdo);

    // pdo is lost at this point, process as if a new connect
    // the pdo is unlatched and may be deleted

    // queue an artificial change for the new 'connect'

    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
    UsbhQueueSoftConnectChange(HubFdo, PortData->PortNumber, PortData->CurrentSc, FALSE);

    // now perform 'connect' of the new device
    return PS_WAIT_CONNECT;

}


ULONG
UsbhPortRecycle(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB device cycle on a port.

    Enable bit clear and connect change asserted, the device was unplugged and
    re-plugged rapidly (Percy Test) or briefly dropped off the bus.

    This also occurs if the client driver or user mode app cycles the port.

    case

    PS_WAIT_CONNECT
        RESET_CHANGE
            ceosp
            11000


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{


    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRYC', PortData, Pdo);


    //UsbhPortDisconnect(HubFdo, PortData, Pdo);

    // pdo is lost at this point, process as if a new connect
    // the pdo is unlatched and may be deleted

    // queue an artificial change for the new 'connect'

    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
    UsbhQueueSoftConnectChange(HubFdo, PortData->PortNumber, PortData->CurrentSc, TRUE);

    UsbhReleaseEnumBusLockEx(HubFdo, PortData->CurrentSc, PortData->PortNumber, FALSE);

    // now perform 'connect' of the new device
    return PS_WAIT_CONNECT;

}



ULONG
UsbhPortResumeComplete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB Suspend Change (resume) on an active port.

    case

    PS_WAIT_CONNECT
        SUSPEND_CHANGE
            ceosp
            11001


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG resumeDelay;

    resumeDelay = 10;

    pdoExt = PdoExt(Pdo);

    if (pdoExt->MsOs20Info.Flags.MinResumeTime == 1) {
        resumeDelay = pdoExt->MsOs20Info.MinResumeTimeDescriptor->bResumeRecoveryTime;
    }

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRSM', PortData, 0);

    // 7.1.7.7
    // Wait 10ms after resume signaling has completed
    // to allow the device to recover
    UsbhWait(HubFdo, resumeDelay);

    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);

    UsbhSS_SignalPdoWake(HubFdo, Pdo);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhPortResumeFailed(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle USB Suspend Change (resume failure) on an active port.

    case

    PS_WAIT_CONNECT
        SUSPEND_CHANGE
            ceosp
            11011


    If suspend bit is still set this indicates resume failed for some reason.



Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pRS!', PortData, 0);

    HUB_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber, SsErr_FailedResumeOnPort,
                              0, 0, NULL, 0);

    // 7.1.7.7
    // Wait 10ms after resume signaling has completed
    // to allow the device to recover, even if the resume failed
    UsbhWait(HubFdo, 10);

    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);

    UsbhSS_SignalPdoWake(HubFdo, Pdo);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhPortDisable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle spontaneous disable of a port

    case

    PS_WAIT_CONNECT
        ENABLE_CHANGE
            ceosp
            10001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    // wait for disconnect
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'pDSB', PortData, 0);
    //#65

    UsbhAssert(HubFdo, PortData->ConnectionStatus != NoDeviceConnected);

    return PS_WAIT_CONNECT;
}


ULONG
UsbhDriverResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Artificial condition to generate a device reset.  When reset completes
    the SyncDeviceResetEvent will be signaled.

    In this case we are using a PDO that is already registered but we go
    through the exact same enumeration code path.

    Handle spontaneous reset request

    case

    PS_WAIT_CONNECT
        DRIVER_RESET_CHANGE
            ceosp
            10001
            11001

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    NTSTATUS nts;
    ULONG ps;
    PPCQ_TIMEOUT_OBJECT tmoObj = NULL;

    LOG(HubFdo, LOG_BUSF, 'pRSD', PortData, 0);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    //Test #94, #95, #97, #98, #99, #100

    // signal any waiting resume thread if the driver manages to queue a reset
    // we must do this since initialiting the reset will kill any resume in
    // progress.
    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);

    nts = UsbhAllocateTimeoutObject(HubFdo, PortData, &tmoObj);

    if (NT_SUCCESS(nts)) {

        // RE-INIT enum retry counters for this next 'enumeration'
        PortData->EnumRetries = 0;
        PortData->PD_Flags.NoDebounce = 1;
        if (Pdo) {
            // clear the enum failure flag in the event a previous reset failed
            PdoExt(Pdo)->PdoFlags.EnumerationFailed = 0;
            UsbhDecHubBusy(HubFdo, PortData->CurrentSc, PdoExt(Pdo)->EnumFailedBh);
            PdoExt(Pdo)->EnumFailedBh = NULL;
        }
         // take the bus lock
        UsbhAcquireEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

        // clear any stale data in the TT buffer if we can
        //UsbhResetTt(HubFdo, PortData);

        // issue the port reset
        UsbhResetPort(HubFdo, PortData, tmoObj);

        UsbhSetConnectionStatus(HubFdo, PortData, DeviceReset);



        // wait for reset to complete or timeout
        ps = PS_ENUMERATE_1;
    } else {
        // cannot reset, report the error
        ps = PS_WAIT_CONNECT;

        UsbhSignalSyncDeviceReset(HubFdo, Pdo, PortData, nts);
    }


    return ps;
}


ULONG
UsbhFailDriverResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_ENUMERATE1
    PS_ENUMERATE2
    PS_WAIT_CONNECT

    if we see a driver reset change in an invalid state fail it and remain in current state.


Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    LOG(HubFdo, LOG_BUSF, 'pRSD', PortData, 0);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));

    // TEST NOTE
    // this path may be hit running a conmbination of SS mice and cyclelseep

    // release any pending reset
    UsbhSignalSyncDeviceReset(HubFdo, Pdo, PortData, NT_DEVICE_DISCONNECTED_STATUS);

    // remain in current state
    return PortData->CurrentState;
}


ULONG
UsbhCancelEnumeration(
    PDEVICE_OBJECT HubFdo,
    ULONG State,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_ENUMERATE1
    PS_ENUMERATE2

    kills an enumeration in progress.  Pdos that are not ref'ed are disconnected.

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'cxEN', PortData, 0);

    switch (State) {
    case PS_ENUMERATE_1:
    case PS_ENUMERATE_2:
        break;
    default:
        // skip if not enumerating
        return State;
    }

    hubExt = FdoExt(HubFdo);
    // cancel the reset that is in progress, this will also disable the port
    UsbhCancelResetTimeout(HubFdo, PortData, CANCEL_RESET_IN_PROGRESS);

    ASSERT_PORT_DATA(HubFdo, PortData);
    UsbhAssertBusLock(HubFdo, PortData->PortNumber);

    UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

    // allow PDO to be deleted
    BUS_UNLATCH_PDO(HubFdo, Pdo, PortData);

    // disconnect this PDO.  we will deal with the device when we spin up.
    // if the PDO is not ref'ed by PNP disconnect it
    // this will only disconnect orphans or enums in-progress

    //UsbhDispatch_PdoPnpEvent(HubFdo, Pdo, PdoEv_BusPause);
    Usbh_BusPause_PdoEvent(HubFdo, Pdo, PortData, PortData->CurrentSc);

    // we should not return with a reset timeout pending
    // otherwise we will ignore if the device suddenly reappears
    UsbhAssert(HubFdo, PortData->ResetTimerPending == NULL);
    UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_Waiting);

    return PS_PAUSED;

}


ULONG
UsbhWcCancelEnumeration(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_WAIT_CONNECT

    kill the enumeration in wait connect state iff the PDO has not been
    reported to pnp yet

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    HUB_PDO_HWPNP_STATE pdoHwState;
    PSTATE_CONTEXT sc;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'cxEN', PortData, 0);

    ASSERT_PORT_DATA(HubFdo, PortData);

    UsbhAssert(HubFdo, PortData->CurrentState == PS_WAIT_CONNECT);


    if (Pdo) {
        sc = PortData->CurrentSc;

        // check the current hardware state -- if we haven't enumerated yet then
        // we need to cancel enumeration instead.
        UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_PortSuspendAbortEnum);

        pdoHwState = GET_PDO_HWPNPSTATE(Pdo);
        switch(pdoHwState) {
            // device disconnected while we were initializing the PDO
            // return it to the created state for when we resume.

        case Pdo_HwPresent:

            // allow PDO to be deleted
            UsbhBusUnlatchPdo(HubFdo, Pdo, PortData, NULL, &sc->gLock);
            UsbhDeletePdo(HubFdo, Pdo, pdoHwState ,PdoEv_PortSuspendAbortEnum, sc);
            break;

        default:
            // otherwise just go to pause
            UsbhReleasePdoStateLock(HubFdo, PortData->CurrentSc);
        }
    }

    return PS_PAUSED;

}


ULONG
UsbhHandleOvercurrent(
    PDEVICE_OBJECT HubFdo,
    ULONG State,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_*

    Handles a port level overcurrent indication for all states.
    State indicates the current state.

    This event is generated when a port overcurrent condition is signalled
    by the hub.  The condition may or may not be persistent, the routine
    is called if any change is indicated.

    ** overcurrent is a hardware condition triggered by the hubs internal
       circuitry, it is the same thing as tripping a circuit breaker.  It
       should not be confused with a device that requests a configuration
       that is failed by software for insufficient power -- such requests
       do not effect the hardware (device or hub) in any way.



Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG next_state = PS_PAUSED;
    USB_HUB_HW_PORT_STATUS ps;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    ps.us = PortData->CurrentPortStatus.us;
    // try to determine if this is true overcurrent
    do {
        if (ps.Power && hubExt->FdoFlags.PowerOnOverCurrent == 0) {
            // port still has power

            if (ps.Overcurrent == 0) {

                //TEST #136
                // must be false or intermittent condition, since the port
                // is powered there is nothing to do.
                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_OvercurrentFalse,
                                     0, 0, PortData, sizeof(struct _HUB_PORT_DATA));


            } else {
                //TEST #137
                // port is powered and overcurrent is indicated, it is not clear if this is legal
                // or what it means, normally hubs clear the power bit.

                // The XP version of the hub driver ignores this condition so we continue
                // this behavior to be compatible.

                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_OvercurrentIgnored,
                                     0, 0, PortData, sizeof(struct _HUB_PORT_DATA));

            }

            next_state = State;
            break;

        } else {
            // port power was shut off.
            //TEST #138, #139

            // the power has been shut off, this is possibly a real overcurrent
            // in the event this is just a surge we will try to re-enable power.
            PortData->OvercurrentCounter++;

            if (Pdo &&
                GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpStop) {

                // Keep the hub active until the user requests the port be reset via the popup UI
                Usbh_OvercurrentRefHubBusy(HubFdo, PortData, FALSE);

                // if overcurrent occurs after a driver is loaded then we invoke UI or
                // allow the driver to try to handle it.

                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_DriverOvercurrent_Popup,
                                                0, 0, PortData, sizeof(struct _HUB_PORT_DATA));

                UsbhSetConnectionStatus(HubFdo, PortData, DeviceCausedOvercurrent);

                // returning paused flushes all remaining changes in the queue
                next_state = PS_PAUSED;
                break;
            }

            // if the overcurrent has happened during enumeration or there is no device
            // driver then this will result in a connect change that can be processed normally...
            // i.e. a sudden disconnect/reconnect of the device.

            // cancel any enumeration in progress
            next_state = UsbhCancelEnumeration(HubFdo, State, PortData, Pdo);

            if (PortData->OvercurrentCounter < 3) {

                // Keep the hub active until auto-recovery is complete
                Usbh_OvercurrentRefHubBusy(HubFdo, PortData, TRUE);

                // attempt to re-enable power
               
                // this will invoke UsbhQueueOvercurrentReset
                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_Overcurrent_AutoClear,
                                   0, 0, PortData, sizeof(struct _HUB_PORT_DATA));

                next_state = PS_PAUSED;
                break;
            }

            TEST_TRAP();

            //TEST #139
            // several attempts to auto-recover failed, we now request
            // user intervention in the Windows XP fashion.

            LOG(HubFdo, LOG_BUSF, 'pOVR', PortData, 0);
            HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, HubErr_OvercurrentClearFailed,
                                        0, 0, PortData, sizeof(struct _HUB_PORT_DATA));

            UsbhSetConnectionStatus(HubFdo, PortData, DeviceCausedOvercurrent);

            // returning paused flushes all remaining changes in the queue
            next_state = PS_PAUSED;

            Usbh_OvercurrentRefHubBusy(HubFdo, PortData, FALSE);

            // generate the UI
            HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber, DeviceOvercurrent_Popup,
                                            0, 0, PortData, sizeof(struct _HUB_PORT_DATA));

            break;
        }

    } WHILE (0);

    return next_state;

}

VOID
UsbhSusTrap(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    ULONG ChangeBits,
    UCHAR PortStatusBits,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Trap unhandled port events

Irql: PASSIVE

Arguments:

Return Value:

    none


--*/
{
    UsbhKdPrint((0, ">Invalid Change Case: SUSPENDED\n"));
    UsbhKdPrint((0, ">ChangeBits: %08.8x PortStatusBits x%02.2x\n",
        ChangeBits, PortStatusBits));
    UsbhKdPrint((0, ">PortData: %p \n", PortData));
    UsbhKdPrint((0, ">Pdo: %p \n", Pdo));
    UsbhAssertFailure(HubFdo, PortData);
}


ULONG
UsbhHandleSuspend(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_SUSPENDED

    Handle state changes for PS_SUSPEND

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    // default is to remain in the current state
    ULONG nextState = PS_SUSPENDED;

    UsbhAssertPassive();

    LOG(HubFdo, LOG_BUSF, 'SUSP', PortData, Event.ul);
    DbgTrace((UsbhDebugTrace, "#%d SUSPEND portStatusBits status_bits:0x%1.1x change:0x%4.4x\n",
      PN(PortData),
      PortStatusBits,
      Event.ul));

    switch (Event.ul) {
    case REQUEST_RESUME_BIT:

        nextState =

        UsbhResumeSuspendedPort(HubFdo,
                            Event,
                            PortStatusBits,
                            PortData,
                            Pdo);

        break;

    case OVERCURRENT_CHANGE_BIT:

        TEST_TRAP();
        nextState = UsbhHandleOvercurrent(HubFdo,
                             PS_SUSPENDED,
                             PortStatusBits,
                             PortData,
                             Pdo);
        break;

    case REQUEST_SUSPEND_BIT:
        // already suspended?
        break;

    case CONNECT_CHANGE_BIT:
        // device disconnected
        TEST_TRAP();
        nextState = UsbhPortDisconnect(HubFdo, PortData, Pdo);
        break;

    default:
        // default behavior is to dump the change
        DbgTrace((UsbhDebugTrace, "suspend flush change\n"));
        LOG(HubFdo, LOG_BUSF, 'susF', PortData->CurrentPortStatus.us,
              PortStatusBits);
        nextState = PS_PAUSED;
    }


    return nextState;

}



ULONG
UsbhResumeSuspendedPort(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    PS_SUSPENDED

    resumes a suspended port if necessary

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    USB_PORT_STATUS_BITS psBits;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'ssRM', PortData, PortStatusBits);
    psBits.uc = PortStatusBits;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Event.RequestPortResume);

    do {
        // *(Pdo may be null if the device was partially enumerated durting suspend)
        // see if this is the source of the resume
        if (psBits.Enable && psBits.Connect &&
            !(psBits.Suspend) && Pdo != NULL) {

            // looks like this device already signalled resume
            PdoExt(Pdo)->PdoFlags.ResumeSignal = 1;
        }

        if (psBits.Suspend && psBits.Connect) {
            // we need to resume
            // transmit a resume request on the bus if the port is suspended
            nts = UsbhResumePort(HubFdo, PortData, &usbdStatus);

            if (NT_SUCCESS(nts)) {
                LOG(HubFdo, LOG_POWER, 'wtRS', 0, PortData->PortNumber);
                // wait for the hubs resume change
                break;
            }

            LOG(HubFdo, LOG_POWER, 'PRE0', nts, PortData->PortNumber);
            if (!Usb_Disconnected(nts)) {
                // TEST #2003
                HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                      PowerErr_SetPortResumeFailed,
                      nts, usbdStatus, NULL, 0);
            }
        }

        // if we have an error we need to signal that resume is complete for
        // this port anyway.

        // not that we do not assert the resume pending state since the resume may have already timed
        // out.  This can happen if you run a wimpy machine with DMA verifier enabled -- it slows things
        // down enough to trigger these codepaths.
        UsbhSignalResumeEvent(HubFdo, PortData, FALSE);

    } WHILE (0);

    //
    // return to wait connect to handle the completion
    return PS_WAIT_CONNECT;

}


ULONG
UsbhRequestPortSuspend(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

     ceosp
     00000

   // PS_*
    //  REQUEST_SUSPEND
    //      ****

    //this is only valid in one case
    //PS_WAIT_CONNECT
    //  DRIVER_SUSPEND_CHANGE
    //      11001
    //

    // all other cases this request is a nop.
    // in the above case we issue the port suspend and signal the event

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_PORT_STATUS_BITS psBits;
    ULONG nextState = PS_WAIT_CONNECT;
    HUB_PDO_HWPNP_STATE pdoHwState;
    PSTATE_CONTEXT sc;

    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC!", PN(PortData)));
    LOG(HubFdo, LOG_BUSF, 'ssSP', PortData, 0);
    psBits.uc = PortStatusBits;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->CurrentState == PS_WAIT_CONNECT);

    if (Pdo) {
        // check the current hardware state -- if we haven't enumerated yet then
        // we need to cancel enumeration instead.
        sc = PortData->CurrentSc;
        UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_PortSuspendAbortEnum);

        pdoHwState = GET_PDO_HWPNPSTATE(Pdo);
        switch(pdoHwState) {
            // device disconnected while we were initializing the PDO
            // return it to the created state for when we resume.

        case Pdo_HwPresent:
            TEST_TRAP();
            UsbhBusUnlatchPdo(HubFdo, Pdo, PortData, NULL, &sc->gLock);

            UsbhDeletePdo(HubFdo, Pdo, pdoHwState ,PdoEv_PortSuspendAbortEnum, PortData->CurrentSc);
            break;

        default:
            UsbhReleasePdoStateLock(HubFdo, PortData->CurrentSc);
        }


        DbgTrace((UsbhDebugTrace, "#%d Req Suspend Change \n", PN(PortData)));
        if (PortStatusBits == 0x19) {

    //        ceosp
    //        11001
    //          connect, enabled, PDO

            UsbhSuspendPort(HubFdo, PN(PortData));

            nextState = PS_SUSPENDED;
        } else if (PortStatusBits == 0x1b) {
            // 1208006 Race condition in usbhub!UsbhSyncResumePort if resume occurs
            // before suspend is complete

            nextState = PS_SUSPENDED;
        }

    } else {
        // no pdo return to wait_connect, signal any waiting suspend operation
        nextState = PS_WAIT_CONNECT;
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
    }
    // signaled when in suspend operation completes
    //UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);

    return nextState;

}


VOID
UsbhBusUnlatchPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    PVOID Object,
    PHGSL Glock
    )
/*++

Routine Description:

    Releases the 'bus latch' on a PDO

    This is the inverse of

    UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoBusLatch_Tag);

Irql: PASSIVE

Arguments:

Return Value:

    next state

--*/
{
    HGSL lock;

    UsbhAssert(HubFdo, PortData->PdoLatched);
    UsbhAssert(HubFdo, Pdo);

    LOG(HubFdo, LOG_BUSF, 'ulP1', Pdo, PortData);

    if (Glock == NULL) {
        Glock = &lock;

        Glock->Locked = 0;
        LockGSL(Glock);

        PortData->PdoLatched = 0;
        LOG(HubFdo, LOG_BUSF, 'ulP2', Pdo, PortData);

        UsbhDerefPdo(HubFdo, Pdo, Object, PdoBusLatch_Tag, Glock);

        unLockGSL(Glock);
        Glock = NULL;
    } else {
        UsbhAssert(HubFdo, Glock->Locked);
        LOG(HubFdo, LOG_BUSF, 'ulP3', Pdo, PortData);
        PortData->PdoLatched = 0;

        UsbhDerefPdo(HubFdo, Pdo, Object, PdoBusLatch_Tag, Glock);
    }

}


