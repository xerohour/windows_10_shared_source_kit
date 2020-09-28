/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    bus.c

     3-10-2002

Abstract:

    Handles hardware reset for a USB hub

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
#include "reset.tmh"
#endif

CCHAR usbfile_reset_c[] = "reset.c";

#define USBFILE usbfile_reset_c

#define BOOT_DEVICE_MAX_CONCURRENT_RESET 20

/*
    The purpose of reset is to recover a hub that is a malfunctioning state,
    the process is the same as that use by all usb device drivers -- reset
    signalling is initiated on the upstream port and the device is reconfigured.

    This feature is more commonly referred to as 'ESD' recovery because it is
    used to recover the hardware during electro-static discharge testing by OEMs.


    Hard reset state machine:


STATES:

    HReset_Disabled - Hard reset is disabled, the hub can not be reset from this state
    HReset_WaitReady - Waiting for a reset request.
    HReset_WaitDone - waiting to finish reset.
    HReset_Queued - Reset Request is queued.

    HReset_ResetStop - The hub requires a reset to function. We enter this state if ESD is
                    disabled, the hub will remain in this state until unloaded or reset
                    by other means.


EVENTS:

    HRE_Enable - triggered when the hub is initialized and can be reset.
    HRE_Disable - triggered when we want to turn-off resets.
    HRE_RequestReset - sent when a hardware error indicates we need to be reset.
    HRE_UserRequestReset - sent when a see the reset IOCLTL.
    HRE_wRun - sent when the reset workitem runs.
    HRE_wResetSuccess - sent when reset succeeds.
    HRE_wResetFailed - sendt when reset fails.
    HRE_wNoESD - sent if reset is requested but ESD is disabled.


Reset State Table
----------------------------------------------------------------------------------------------------------
State               Events                      Next State          Actions

HReset_Disabled
                    HRE_Disable                 HReset_Disabled
                    HRE_RequestReset            HReset_Disabled(HubHardResetPending=1)
                    HRE_Enable                  HReset_WaitReady

..........................................................................................................
HReset_WaitReady
                    HRE_Disable                 HReset_Disabled
                    HRE_RequestReset            HReset_Queued
                    HRE_UserRequestReset        HReset_WaitReady
...........................................................................................................
HReset_Queued
                    HRE_Disable                 HReset_WaitDone
                    HRE_wRun                    HReset_WaitReady
                                                HReset_ResetStop
                    HRE_wNoESD                  HReset_ResetStop
                    HRE_RequestReset            HReset_Queued
                    HRE_UserRequestReset        HReset_WaitReady
............................................................................................................
HReset_ResetStop
                    HRE_UserRequestReset        HReset_Queued
...........................................................................................................
HReset_WaitDone     HRE_wNoESD                  HReset_Disabled
                    HRE_wRun                    HReset_Disabled
                    HRE_RequestReset            HReset_WaitDone
                    HRE_UserRequestReset        HReset_WaitDone

...........................................................................................................


*/

VOID
UsbhHardResetAddDeviceInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    initializes the 'ESD' state machine

    This must start before the bus state machine is started since the the bus enum engine
    may attempt to initiate a reset.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    // initial state is disabled state

    INIT_EVENT_SIGNALED(&hubExt->HardResetDoneEvent);
    KeInitializeSemaphore(&hubExt->HardResetLock, 1, 1);
    hubExt->HardResetState = HReset_Disabled;
}


NTSTATUS
UsbhResetHubUpstreamPort(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    This functions sends a reset - re-enumerate request to our PDO, we use this
    for hardware error recovery.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace, "Reset Hub upstream port\n"));

    LOG(HubFdo, LOG_HUB, 'RST>', 0, 0);

    nts = UsbhSyncSendInternalIoctl(HubFdo,
                              IOCTL_INTERNAL_USB_RESET_PORT,
                              NULL,
                              NULL);

    LOG(HubFdo, LOG_HUB, 'RST<', nts, 0);

    return nts;
}


HARD_RESET_STATE
UsbhGetContextResetState(
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
    UsbhAssertResetStateContext(Context);

    return Context->HardResetState;

}


HARD_RESET_STATE
Usbh_HRS_Disabled(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_Disabled state

   Reset is currently off-line

Arguments:

Return Value:

    next Reset state
--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_Disabled);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'rsDS', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {

    case HRE_Disable:
    case HRE_Pause:
    case HRE_Resume:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Disabled);
        break;
        
    case HRE_UserRequestReset:
    case HRE_RequestReset:
        // hard reset is pending, this flag tells us we need to
        // reset if we re-enable the hub
        hubExt->FdoFlags.HubHardResetPending=1;

        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Disabled);
        break;

    case HRE_Enable:
        if (hubExt->FdoFlags.HubHardResetPending) {
            TEST_TRAP();
            // a reset request was made while reset functionality was
            // disabled.  This means the hardware is in an undefined state
            // and may not function.
        }
        // initial transition puts us in the ready for reset state.
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_WaitReady);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);
    }

    return rs;
}

HARD_RESET_STATE
Usbh_HRS_Paused(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_Paused state

   Reset is currently paused for low Dx state

Arguments:

Return Value:

    next Reset state
--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_Paused);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'rsPS', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {

    case HRE_Disable:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Disabled);
        break;
        
    case HRE_UserRequestReset:
    case HRE_RequestReset:
        // hard reset is pending, this flag tells us we need to
        // reset if we re-enable the hub
        hubExt->FdoFlags.HubHardResetPending=1;

        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Paused);
        break;

    case HRE_Pause:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Paused);
        break;

    case HRE_Enable:
    case HRE_Resume:
        if (hubExt->FdoFlags.HubHardResetPending) {
            // a reset request was made while reset functionality was paused.
            // Queue the hard reset now
            RESET_EVENT(&hubExt->HardResetDoneEvent);

            IoQueueWorkItem(hubExt->HardResetWorkItem,
                            UsbhHardResetWorker,
                            DelayedWorkQueue,
                            NULL);

            rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Queued);
        } else {
            // initial transition puts us in the ready for reset state.
            rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_WaitReady);
        }
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);
    }

    return rs;
}
HARD_RESET_STATE
Usbh_HRS_WaitReady(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_WaitReady state

   Hub can be reset

Arguments:

Return Value:

    next Reset state
--*/

{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_WaitReady);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'bsIN', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {
    case HRE_UserRequestReset:      
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_WaitReady);
        break;
        
    case HRE_Disable:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Disabled);
        break;
        
    case HRE_Pause:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Paused);
        break;

    case HRE_RequestReset:
        RESET_EVENT(&hubExt->HardResetDoneEvent);

        // This is the first call to reset the hub, lets notify all 
        // downstream hubs to start failing I/O's 
        UsbhResetNotifyDownstreamHub(Sc->HubFdo);

        IoQueueWorkItem(hubExt->HardResetWorkItem,
                        UsbhHardResetWorker,
                        DelayedWorkQueue,
                        NULL);

        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Queued);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return rs;
}


HARD_RESET_STATE
Usbh_HRS_ResetStop(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_ResetStop state

   ESD is disabled so the hub just waits to unload.

Arguments:

Return Value:

    next Reset state
--*/

{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_ResetStop);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'bsIN', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {

    case HRE_Enable:
    case HRE_Disable:
    case HRE_Resume:
    case HRE_Pause:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_ResetStop);
        break;

    case HRE_UserRequestReset:
        RESET_EVENT(&hubExt->HardResetDoneEvent);

        IoQueueWorkItem(hubExt->HardResetWorkItem,
                        UsbhHardResetWorker,
                        DelayedWorkQueue,
                        NULL);

        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Queued);
        break;

    case HRE_RequestReset:

        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_ResetStop);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return rs;
}


HARD_RESET_STATE
Usbh_HRS_Queued(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_Queued state

   Reset request is queued.

Arguments:

Return Value:

    next Reset state
--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_Queued);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'rsQU', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {
    case HRE_Disable:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_WaitDone);
        WAIT_EVENT(&hubExt->HardResetDoneEvent);
        break;

    case HRE_Pause:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_PausedWaitDone);
        WAIT_EVENT(&hubExt->HardResetDoneEvent);
        break;

    case HRE_wRun:
        UsbhEtwLogHubEvent(Sc->HubFdo, &USBHUB_ETW_EVENT_HUB_HARD_RESET_START, 0, STATUS_SUCCESS);

        // perform the actual reset
        nts = UsbhHardReset_Action(Sc->HubFdo, Sc);

        // If the hub is in the boot path, and reset requests occurred while the hub reset code
        // was running, reset the hub again as we can't be sure the boot device was recovered
        if (hubExt->IsBootDevice) {
            // If another reset was queued after UsbhHardReset_Action was called
            // and was in progress, we need to reset again
            while (InterlockedExchange(&hubExt->HubResetRequestCount, 0) != 0) {
                UsbhResetNotifyDownstreamHub(Sc->HubFdo);
                nts = UsbhHardReset_Action(Sc->HubFdo, Sc);
                UsbhIncrementConcurrentResetCounter(Sc->HubFdo);
            }
        }

        UsbhEtwLogHubEvent(Sc->HubFdo, &USBHUB_ETW_EVENT_HUB_HARD_RESET_COMPLETE, 0, nts);
        if (NT_SUCCESS(nts) || hubExt->IsBootDevice) {

            // Queue a work item to update hard reset count. We should avoid touching the registry, which
            // can cause a page fault, in this code path.
            nts = UsbhQueueWorkItem(Sc->HubFdo,
                    DelayedWorkQueue,
                    UsbhUpdateRegHubHardResetCount,
                    NULL,
                    0,
                    SIG_RESET_WORKITEM);

            if(NT_ERROR(nts)) {
                // This is not a fatal error, just log debug trace
                DbgTrace((UsbhDebugTrace, "Failed to queue UpdateRegHubHardResetCount work item - %!STATUS!\n", nts));
            }

            rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_WaitReady);
            // A small window where UsbhHardReset_Action returned and while the above
            // 4 statements were executed another reset got queued we need to reset again
            if (hubExt->IsBootDevice && hubExt->HubResetRequestCount != 0) {

                UsbhResetNotifyDownstreamHub(Sc->HubFdo);
                UsbhHardReset_Action(Sc->HubFdo, Sc);
                UsbhIncrementConcurrentResetCounter(Sc->HubFdo);
            }
        } else {
            rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_ResetStop);
        }
        break;

    case HRE_wNoESD:

        UsbhNoESD_Action(Sc->HubFdo, Sc);
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_ResetStop);

        // perform the actual reset
        break;

    case HRE_UserRequestReset:
    case HRE_RequestReset:
        // we already have one queued
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Queued);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return rs;
}


HARD_RESET_STATE
Usbh_HRS_PausedWaitDone(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_PausedWaitDone state

   Reset request is queued.

Arguments:

Return Value:

    next Reset state
--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_PausedWaitDone);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'rsWP', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {
    case HRE_wNoESD:
    case HRE_wRun:
       
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Paused);
        SET_EVENT(&hubExt->HardResetDoneEvent);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return rs;
}

HARD_RESET_STATE
Usbh_HRS_WaitDone(
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

   Handles HReset_Queued state

   Reset request is queued.

Arguments:

Return Value:

    next Reset state
--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(Sc->HubFdo);
    rs = UsbhGetContextResetState(Sc);

    UsbhAssert(Sc->HubFdo, rs == HReset_WaitDone);
    UsbhAssertPassive();

    LOG(Sc->HubFdo, LOG_POWER, 'rsQU', Sc, HardResetEvent);
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    switch(HardResetEvent) {
    case HRE_wNoESD:
    case HRE_wRun:
       
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, HReset_Disabled);
        SET_EVENT(&hubExt->HardResetDoneEvent);
        break;

    default:
        rs = UsbhReleaseResetLock(Sc->HubFdo, Sc, rs);
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return rs;
}


HARD_RESET_STATE
UsbhDispatch_HardResetEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    HARD_RESET_EVENT HardResetEvent
    )
/*++

Routine Description:

    Dispatcher for hard reset events, this routine dispatches by state. The hard reset routine itself runs in the 
    context of its own system work_queue_item, the check to see if we need to do a reset may also be in the context 
    of a work_queue_item item.  If a reset is currently queued we exit so that we don't block a worker thread.

Arguments:

    Sc - context that originated the event.

    HardResetEvent - event to execute.

Return Value:

    new state

--*/
{
    HARD_RESET_STATE rs;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL hrs_irql;
    
    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();
    UsbhAssert(HubFdo, HardResetEvent);

    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", HardResetEvent));

    //
    // special case check to for a pending hardware reset.  Since the hubs hardware error reset code is run from a work 
    // queue item we don't want to block unless we are really going to attempt a hard-reset.
    //
    // we use the workitem list spinlock bacause it is used for a similar function and won't conflict with the lock priority
    // 
    KeAcquireSpinLock(&hubExt->WorkItemListSpin, &hrs_irql);
    
    rs = hubExt->HardResetState;
    LOG(HubFdo, LOG_POWER, 'RSr1', rs, HardResetEvent);

    switch(HardResetEvent) {
    case HRE_UserRequestReset:
    case HRE_RequestReset: 
        if (rs == HReset_Queued) {
            // The HubResetRequestCount will keep track of how many resets were
            // queued while the first one was running
            if (hubExt->IsBootDevice) {
                InterlockedIncrement(&hubExt->HubResetRequestCount);
                UsbhIncrementConcurrentResetCounter(HubFdo);
            }
            LOG(HubFdo, LOG_POWER, 'RSr2', rs, HardResetEvent);
            // Reset is either queued or running. In these cases return and don't queue another reset
            KeReleaseSpinLock(&hubExt->WorkItemListSpin, hrs_irql);
            break;                
        }            
        
    default:
        // any other case fall through to the normal path
        LOG(HubFdo, LOG_POWER, 'RSr3', rs, HardResetEvent);
        
        KeReleaseSpinLock(&hubExt->WorkItemListSpin, hrs_irql);
    
        LOG(HubFdo, LOG_POWER, 'rs++', Sc, HardResetEvent);
        rs = UsbhAcquireResetLock(HubFdo, Sc, HardResetEvent);
        LOG(HubFdo, LOG_POWER, 'rs--', Sc, rs);

        switch (rs) {
        case HReset_Disabled:
            return Usbh_HRS_Disabled(Sc, HardResetEvent);
        case HReset_WaitReady:
            return Usbh_HRS_WaitReady(Sc, HardResetEvent);
        case HReset_Queued:
            return Usbh_HRS_Queued(Sc, HardResetEvent);
        case HReset_ResetStop:
            return Usbh_HRS_ResetStop(Sc, HardResetEvent);
        case HReset_WaitDone:
            return Usbh_HRS_WaitDone(Sc, HardResetEvent);
        case HReset_Paused:
            return Usbh_HRS_Paused(Sc, HardResetEvent);
        case HReset_PausedWaitDone:
            return Usbh_HRS_PausedWaitDone(Sc, HardResetEvent);

        default:
            UsbhAssertFailure(HubFdo, Sc);
            UsbhReleaseResetLock(HubFdo, Sc, rs);
        }
        
    }
    
    return rs;

}



VOID
UsbhEnableHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Enables ESD recovery.  This allows the Bus State machine to generate hub reset
    requests that reset the hub.

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    LOG(HubFdo, LOG_HUB, 'RST+', 0, 0);
    UsbhDispatch_HardResetEvent(HubFdo, Sc, HRE_Enable);

}


VOID
UsbhDisableHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Waits for any pending reset to finish, this will wait for the workitem
    currently queued to finish and no new requests will be scheduled after reset
    is disabled.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{

    LOG(HubFdo, LOG_HUB, 'RST-', 0, 0);
    UsbhDispatch_HardResetEvent(HubFdo, Sc, HRE_Disable);

}

VOID
UsbhPauseHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Waits for any pending reset to finish, this will wait for the workitem
    currently queued to finish and no new requests will be scheduled after reset
    is paused.  

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{

    LOG(HubFdo, LOG_HUB, 'RSTp', 0, 0);
    UsbhDispatch_HardResetEvent(HubFdo, Sc, HRE_Pause);

}


VOID
UsbhResumeHardReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Resumes ESD recovery.  This allows the Bus State machine to generate hub reset
    requests that reset the hub.

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    LOG(HubFdo, LOG_HUB, 'RSTr', 0, 0);
    UsbhDispatch_HardResetEvent(HubFdo, Sc, HRE_Resume);

}


NTSTATUS
UsbhNoESD_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Processes the hard reset request when ESD is disabled. In this case we need
    to pause the bus and drop all child devices.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    // suspend the enum engine for all ports
    UsbhSyncBusPause(HubFdo, Sc, PauseReason_HardReset);

    // dump any devices that were connected their state is undefined
    // anyway.
    UsbhSyncBusDisconnect(HubFdo, Sc);

    return STATUS_SUCCESS;

}


NTSTATUS
UsbhHardReset_Action(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Processes the hard reset request.

    During a hard reset the hardware is dead but pnp isn't.  We report all devices
    as absent that re-enumerate them if we can recover the hub.

    This code performs the actual reset from the workitem or other context.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    ULONG backoff = 20;
    ULONG ps;
    PSSH_BUSY_HANDLE bh;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    UsbhAssert(HubFdo, hubExt->FdoFlags.EsdEnabled);

    LOG(HubFdo, LOG_BUS, 'Hrs2', 0, HubFdo);

    // Prevent hub selective suspend from running while we are resetting the hub
    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhBusyHardReset_Tag, NO_RESUME_HUB);

    if (hubExt->IsBootDevice) {
        WAIT_EVENT(&hubExt->SSH_ResumeEvent);
    }
    
    do {

        // Zero out the HubResetRequestCount so that if another reset happens
        // while we here UsbhDispatch_HardResetEvent can increment this counter
        // to non zero and cause another reset
        if (hubExt->IsBootDevice) {
            InterlockedExchange(&hubExt->HubResetRequestCount, 0);
        }

        // validate upstream connection state before we retry
        nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);
        if (Usb_Disconnected(nts)) {
            LOG(HubFdo, LOG_BUS, 'Rdsc', nts, ps);
            // disconnect and cyclesleep will exercise this path
            break;
        }

        // suspend the enum engine for all ports
        UsbhSyncBusPause(HubFdo, Sc, PauseReason_HardReset);

        // dump any devices that were connected their state is undefined
        // anyway.
        UsbhSyncBusDisconnect(HubFdo, Sc);

        // now that we are stopped, clear the reset circuit.
        hubExt->FdoFlags.HubHardResetPending = 0;

        // See if hub is activesee if this isn the context of global power (like shutdown).  SSP is disabled 
        // during this process so if we are in the context of a power event then stop 
        // any hardware recovery
        nts = Usbh_SSH_Event(HubFdo, SSH_Event_HardReset, Sc);
        if (NT_ERROR(nts)) {
            break;
        }            

        // It turns out that this is the exact same thing as a a cold start for pnp
        // so we use the same function.
        // The only difference is that all the PDOs were dummped before we try to
        // restart -- so in theory it should be easier.
        nts = UsbhFdoSetD0Cold(HubFdo, Sc, TRUE);
        
        // were we able to complete the re-start?

        if (NT_SUCCESS(nts)) {
            // reset counter on first success
            hubExt->HardErrorRetryCount = 0;
            break;
        }

        // restoring the hub failed for some reason, note the failure of reset
        // begin backoff, retry.
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_HardResetFailed,
                                  nts, 0, &backoff, sizeof(ULONG));

        if (nts == STATUS_NOT_SUPPORTED) {
            // for some reason reset port is not supported, this may be the nature of the usbport
            // driver we are loaded on...
            // skip the retry in this case (we will enter error park mode).
            break;
        }

        LOG(HubFdo, LOG_BUS, 'Hrs3', nts, HubFdo);

        // we'll need some type of back-off retry logic I think
        // for now lets try a wait 10 ms and retry.
        hubExt->HardErrorRetryCount++;
        hubExt->FdoFlags.HubHardResetPending = 1;

        UsbhWait(HubFdo, backoff);
        // backoff retry 20, 40, 80, ....
        backoff*=2;
        // three strikes and we are out
    }while (hubExt->HardErrorRetryCount < HubG.GlobalMaxHardResetRetries);

    // returning an error will send in the NoEsd path ie park mode.

    if (NT_ERROR(nts)) {        
        if (!Usb_Disconnected(nts)) {
            // This will generally occur if there is a bug in the hubs internal state machines.  It could also occur if the 
            // hub hardware is hopelessly broken. The hub shuts down in 'Park Mode' and is unresponsive, jason refers to 
            // this as 'the ports are dead'.
            // 
            //For now we treat this condition as a bug, other options include  
            // 1. queue some sort of deadman recovery process at a delayed interval (like 5-10 seconds)
            // 2. Bugcheck.
            // 3. IoInvalidateDeviceState cause hub to unload/reload or !out in devman
            //
            //TEST_TRAP();        
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_HubParked, nts, 0, 0, 0);
        }
        if (hubExt->IsBootDevice) {
            // We have reached a dead end as hub reset fails and the access to 
            // the boot device is lost permanently. Bugcheck is the only option.        
            __pragma(warning(suppress: 28159));
            KeBugCheckEx(BUGCODE_USB_DRIVER, 
                         USBBUGCODE_BOOT_DEVICE_RESET_FAILED,
                         USBHUB_TRAP_FATAL,
                         2,
                         (ULONG_PTR) NULL);
        }            
    }        
    
    // Let the hub suspend
    UsbhDecHubBusy(HubFdo, Sc, bh);

    return nts;
}


VOID
UsbhHardResetWorker(
    PDEVICE_OBJECT HubFdo,
    PVOID Context
    )
/*++

Routine Description:

    RE_wRun, REwDone

    Processes the reset request.

    transition to the 'Queued' state

Irql: PASSIVE

Arguments:

    Context - state context that queued the reset

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc;

    hubExt = FdoExt(HubFdo);
    // the worker runs in its own context, this routine is called from the 
    // workitem that performs the reset.
    
    sc = RESET_CONTEXT(HubFdo);
    ASSERT_HUB_CONTEXT(sc);

    LOG(HubFdo, LOG_BUS, 'resW', sc, 0);

    UsbhAssertPassive();

    if (hubExt->FdoFlags.EsdEnabled) {

        UsbhDispatch_HardResetEvent(HubFdo, sc, HRE_wRun);

    } else {
        // ESD disabled -- we disconnect and enter ResetStop

        UsbhDispatch_HardResetEvent(HubFdo, sc, HRE_wNoESD);
    }
}


VOID
UsbhQueueHardReset(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
 /*++

Routine Description:

    Start the hardware reset process, this potentially queues the reset workitem.
    If a reset is in progress it just exits.

Arguments:

    Exr - optional pointer to the exception record
    ( we could use this to tweak reset behavior if we needed to )

Return Value:

    ntStatus
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    HARD_RESET_STATE rs = HRE_RequestReset;

    hubExt = FdoExt(HubFdo);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_HARD_RESET_QUEUE, 0, STATUS_SUCCESS);

    if (Exr != NULL && Exr->Class == HubErr_User_Reset && Exr->Data != NULL) {
        // Exception data if present, is a BOOLEAN indicating if user is an Admin
        // If user is not an Admin, then dispatch a HRE_UserRequestReset

        if(!(*(PBOOLEAN) Exr->Data))
        {
            // This is a reset request issued by a non-admin process.
            // It has no effect if the hub is working properly.

            rs = HRE_UserRequestReset;
        }
    }

    // exception handlers are executed from workitems so we use relevent context
    // to queue the request. 

    UsbhDispatch_HardResetEvent(HubFdo, RESET_CONTEXT(HubFdo), rs);
}

VOID
UsbhResetNotifyCompletion(
    __in PDEVICE_OBJECT HubFdo
)
/*++

Routine Description:

    This routine is used in the boot path to allow IO's to resume after
    a successful hub reset

Arguments:
    HubFdo - Fdo for the hub whose downstream devices are to be informed

Return Value:

--*/
{
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG pn;
    PHUB_PORT_DATA pd;

    UsbhClearConcurrentResetCounter(HubFdo);

    // We need to go through every Pdo attached to this hub
    // Find the boot pdo and allow IO to it
    for (pn = 1; pn <= N_PORTS(HubFdo); pn++) {
    
        // BUGBUG: This should ideally be converted into UsbhLatchPdo
        // but this function is sometimes called with the GSL
        // taken. That woould cause a hang.
        pd = UsbhGetPortData(HubFdo, (USHORT)pn);
        pdo = pd->PnpPdo;
        if (pdo == NULL) {
            continue;
        }
    
        // We have a valid pdo here. Is this a boot device pdo?
        pdoExt = PdoExt(pdo);
        if (pdoExt->IsBootDevice) {
            if (pdoExt->PendingResetPortAsyncIrp != NULL) {
                DbgTrace((UsbhPwBootTrace,"Allowing I/O for Device HubFdo=#%p %!FUNC!\n", HubFdo));
                LOG(HubFdo, LOG_BUS, 'RNC1', 0, HubFdo);
                UsbhSetPdo_AllowIo(HubFdo,pdo);
                UsbhCompleteIrp(pdoExt->PendingResetPortAsyncIrp, STATUS_SUCCESS);
                pdoExt->PendingResetPortAsyncIrp = NULL;
            } else if (pdoExt->PdoFlags.DeviceIsHub) {
                DbgTrace((UsbhPwBootTrace,"Allowing I/O for Hub HubFdo=#%p %!FUNC!\n", HubFdo));
                LOG(HubFdo, LOG_BUS, 'RNC2', 0, HubFdo);
                UsbhSetPdo_AllowIo(HubFdo,pdo);
                UsbhCompleteIrp(pdoExt->PendingResetNotificationRequestIrp,STATUS_SUCCESS);
            } else {
                // In the sleep and hibernate scenarios, a reset is triggered
                // We do not want to let the ResetCompleteWithoutFailingIO be
                // set without FailIo being set.
                if (pdoExt->IoState == Pdo_FailIo) {
                    pdoExt->ResetCompleteWithoutFailingIO = TRUE;
                }
            }

        }
    }
    
}

VOID
UsbhIncrementConcurrentResetCounter(
    __in PDEVICE_OBJECT HubFdo
    )
{
    
    PDEVICE_EXTENSION_HUB hubExt;
    
    hubExt = FdoExt(HubFdo);

    hubExt->HubConcurrentResetCount++;

    if (hubExt->HubConcurrentResetCount > BOOT_DEVICE_MAX_CONCURRENT_RESET) {
        __pragma(warning(suppress: 28159));
        KeBugCheckEx(BUGCODE_USB_DRIVER, 
                     USBBUGCODE_BOOT_DEVICE_RESET_FAILED,
                     USBHUB_TRAP_FATAL,
                     1,
                     (ULONG_PTR) NULL);
    }
}

VOID
UsbhClearConcurrentResetCounter(
    __in PDEVICE_OBJECT HubFdo
    )
{
    PDEVICE_EXTENSION_HUB hubExt;
    
    hubExt = FdoExt(HubFdo);
    
    hubExt->HubConcurrentResetCount = 0;
}

