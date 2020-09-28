/*
Module: TIMER.C


name:USBHUB Io Timer

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002

Abstract:

    This code implements a general purpose DmTimer for use on lower priority yet still critical retry
    and hard-error recovery operations.

    The goal here is to enable these timer objects without allocating memory dynamically since they are used for
    retries when mem allocations fail.  We use pre-allocated structures so we still get the benefit of driver
    verifier.

    The hub uses a single kernel mode Dpc timer and to service a queue of requests.  These Timer requests should be
    < 1000ms and > 100ms.  We currently use them for the pnp boot timeout, port indicators and hardware error 
    recovery.  When th etimer queue is empty the DM timer is diabled.  

    Perf note: Since this is used for error and hardware recovery most of the time the DM timer should not be 
    scheduled.

    Very low priority operations such as the SSP timer should be defered to the IoTimer which fires every second.


Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      NULL
    Unload -->linked to Unload           NULL
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        UsbhTimerPnpStart
    Remove -->linked to PNP REMOVE       UsbhTimerPnpStop

</pre>

Disabled        - removed from enabled list
Busy            - on busy list
Enabled         - on enabled list


before callback
    TimerObj_Disabled           TimerObj_Disabled   ; remove now
    TimerObj_Enabled            TimerObj_Busy
    TimerObj_Busy               x
..................................................................................
after callback
    TimerObj_Disabled           TimerObj_Disabled  ; canceled, remove now
    **TimerObj_Enabled            TimerObj_Enabled   ; re-enabled re-insert
    TimerObj_Busy---retry=0---->TimerObj_Disabled  ; no retry, remove now
    TimerObj_Busy---retry<>0--->TimerObj_Enabled   ; retry, re-insert
    TimerObj_Busy---exp>0------>TimerObj_Enabled
..................................................................................
..................................................................................
Enable
    TimerObj_Disabled           TimerObj_Enabled    ; insert
    TimerObj_Enabled            x
    **TimerObj_Busy             TimerObj_Enabled    ; re-enable
...................................................................................
Disable
    TimerObj_Disabled           TimerObj_Disabled   ; nop
    TimerObj_Enabled            TimerObj_Disabled   ; remove now
    TimerObj_Busy               TimerObj_Disabled   ; remove when finished
...................................................................................

***********************************************************************************

(DmTimerEvent) 0 = not signaled 1= signaled

Started = timer queued or dpc queued or dpc running event(0)
Stopped = timer not scheduled, dpc not queued event (0)
Disabled = timer not scheduled, dpc not queued event (1)

TimerDpc(listempty)

    HubDmTimer_Disabled     0                   HubDmTimer_Disabled     0->1
    HubDmTimer_Started      0                   HubDmTimer_Stopped      0
    HubDmTimer_Stopped      x

TimerDpc(!listempty)
    HubDmTimer_Disabled     0                   HubDmTimer_Disabled     0->1
    HubDmTimer_Started      0                   HubDmTimer_Started      0
    HubDmTimer_Stopped      x

Start
    HubDmTimer_Disabled     x                   HubDmTimer_Disabled     x
    HubDmTimer_Started      0                   HubDmTimer_Started      0
    HubDmTimer_Stopped      0                   HubDmTimer_Started      0


Disable
    HubDmTimer_Disabled     x                   HubDmTimer_Disabled     x
    HubDmTimer_Started      0--->cancel----no-->HubDmTimer_Disabled     (wait)(1)
                             --->cancel----yes->HubDmTimer_Disabled     0->1
    HubDmTimer_Stopped      0                   HubDmTimer_Disabled     0->1

Init
    HubDmTimer_Stopped      ->0
*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "timer.tmh"
#endif

#define HUB_TIMER_INTERVAL  15


PHUB_TIMER_OBJECT
UsbhAllocateTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_ROUTINE TimeoutFunction,
    ULONG Tag
    )
/*++

Routine Description:

    Sets up a dynamic timerObject in the disabled state.

Arguments:

Irql: ANY

Return Value:

    returns STATUS_DEVICE_BUSY if timer already set

--*/
{
    PHUB_TIMER_OBJECT timrObj;

    UsbhAllocatePool_Z(timrObj,
                       NonPagedPool,
                       sizeof(struct _HUB_TIMER_OBJECT));
    if (timrObj) {
        UsbhInitializeTimerObject(HubFdo,
                                  TimeoutFunction,
                                  timrObj,
                                  Tag);
        timrObj->Type = TimerObj_Dynamic;
    }

    return timrObj;
}


VOID
UsbhInitializeTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_ROUTINE TimeoutFunction,
    PHUB_TIMER_OBJECT TimerObject,
    ULONG Tag
    )
/*++

Routine Description:

    Sets up a static timerObject in the disabled state.

Arguments:

Irql: ANY

Return Value:

    returns STATUS_DEVICE_BUSY if timer already set

--*/
{
    LOG(HubFdo, LOG_TMR, 'iniT', TimerObject, TimeoutFunction);
    TimerObject->Sig = SIG_TIMEROBJ;
    TimerObject->TimeoutFunction = TimeoutFunction;
    TimerObject->State = TimerObj_Disabled;
    TimerObject->Type = TimerObj_Static;
    TimerObject->Tag = Tag;

    return ;
}


VOID
UsbhFreeTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT *TimerObject
    )
/*++

Routine Description:

    Frees an allocated timer object.

Arguments:

Irql: ANY

Return Value:

    returns STATUS_DEVICE_BUSY if timer already set

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_TIMER_OBJECT timrObj;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_TMR, 'freT', TimerObject, *TimerObject);
    if (*TimerObject == NULL) {
        return;
    }

    timrObj = *TimerObject;
    // it really should already be disabled
    UsbhDisableTimerObject(HubFdo, timrObj);

    KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);

    *TimerObject = NULL;
    LOG(HubFdo, LOG_TMR, 'frT1', timrObj, 0);
    UsbhFreePool(timrObj);

    KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);

}


#pragma prefast(disable:__WARNING_UNEXPECTED_IRQL_CHANGE  __WARNING_FAILING_TO_RELEASE, "Function releases spinlock, but analysis doesn't seem to know this") 

VOID
UsbhEnableTimerObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    LONG MillisecsToExpire,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    Sets the timerObject event to run at the specified time.

Arguments:

Irql: ANY

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    KIRQL irql;
  
    hubExt = FdoExt(HubFdo);

    if (!TimerObject) {
        return;
    }

    ASSERT_TIMER_OBJECT(HubFdo, TimerObject);

    KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);

    LOG(HubFdo, LOG_TMR, 'enT1', TimerObject, MillisecsToExpire);
    LOG(HubFdo, LOG_TMR, 'enp1', PortNumber, Context);
    LOG(HubFdo, LOG_TMR, 'enp2', Parameter1, Parameter2);

    UsbhEtwLogHubEventWithExtraData(HubFdo, 
                                    &USBHUB_ETW_EVENT_HUB_ENABLE_TIMER,
                                    PortNumber,
                                    &TimerObject->Tag,
                                    sizeof(TimerObject->Tag));


    // set values
    TimerObject->Parameter1 = Parameter1;
    TimerObject->Parameter2 = Parameter2;
    TimerObject->Context = Context;
    TimerObject->PortNumber = PortNumber;
    TimerObject->MillisecsToExpire = MillisecsToExpire;
    TimerObject->ExpireTime = MillisecsToExpire;

    if (TimerObject->State == TimerObj_Disabled) {
        LOG(HubFdo, LOG_TMR, 'enT2', TimerObject,&hubExt->DmTimerListHead);
        // new timer in queue
        TimerObject->State = TimerObj_Enabled;
        InsertTailList(&hubExt->DmTimerListHead, &TimerObject->EnableLink);

        LOG(HubFdo, LOG_TMR, 'dmTT', HubFdo, hubExt->DmTimerState);
        if (hubExt->DmTimerState == HubDmTimer_Stopped) {
            NTSTATUS tmr_nts;

            LOG(HubFdo, LOG_TMR, 'd2>0', HubFdo, hubExt->DmTimerState);
            hubExt->DmTimerState = HubDmTimer_Started;

            KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
            // stopped -> started
            tmr_nts = UsbhSetTimer(HubFdo,
                         HUB_TIMER_INTERVAL,
                         &hubExt->DmKtimer,
                         &hubExt->DmTimerDpc,
                         dTMR,
                         TRUE);

            // timer already queued?
            UsbhAssert(HubFdo, tmr_nts != STATUS_DEVICE_BUSY);
            if (NT_ERROR(tmr_nts)) {
                // low resource sim will hit this path
                TEST_TRAP();
            }     
        } else {
            KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
        }

    } else if (TimerObject->State == TimerObj_Busy) {
        // preempting the timer
        UsbhAssert(HubFdo, hubExt->DmTimerState != HubDmTimer_Stopped);

        LOG(HubFdo, LOG_TMR, 'enT3', TimerObject,&hubExt->DmTimerListHead);
        TimerObject->State = TimerObj_Enabled;
        KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
    } else {
        KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
    }

    return;
}

#pragma prefast(enable:__WARNING_UNEXPECTED_IRQL_CHANGE  __WARNING_FAILING_TO_RELEASE, "Reenable warnings for IRQL changes and spinlock release") 


VOID
UsbhDisableTimerObject(
     PDEVICE_OBJECT HubFdo,
     PHUB_TIMER_OBJECT TimerObject
     )
/*++

Routine Description:

    Disables a pre-allocated timer object. It waits for the timer function to
    finish if it is running.

Arguments:

Irql: PASSIVE

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    if (!TimerObject) {
        return;
    }

    ASSERT_TIMER_OBJECT(HubFdo, TimerObject);

    UsbhEtwLogHubEventWithExtraData(HubFdo, 
                                    &USBHUB_ETW_EVENT_HUB_DISABLE_TIMER,
                                    TimerObject->PortNumber,
                                    &TimerObject->Tag,
                                    sizeof(TimerObject->Tag));

    KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);
    LOG(HubFdo, LOG_TMR, 'dsT1', TimerObject, TimerObject->State);

    if (TimerObject->State == TimerObj_Enabled) {
        LOG(HubFdo, LOG_TMR, 'dsT2', TimerObject, TimerObject->State);

        RemoveEntryList(&TimerObject->EnableLink);
        TimerObject->State = TimerObj_Disabled;
    } else {
        LOG(HubFdo, LOG_TMR, 'dsT3', TimerObject, TimerObject->State);
        // preempting the timer or already disabled
        TimerObject->State = TimerObj_Disabled;
    }

    KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
    return;

}


KDEFERRED_ROUTINE UsbhDmTimerDpc;

VOID
UsbhDmTimerDpc(
    PKDPC Dpc,
    PDEVICE_OBJECT HubFdo,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )

/*++

Routine Description:

    Called every 500 ms by the DmTimer

Arguments:

Irql: DPC

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PLIST_ENTRY le;
    LIST_ENTRY retryListHead;
    PHUB_TIMER_OBJECT tmrObj;
    KIRQL irql;
    BOOLEAN DereferenceTimer=TRUE;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);

    InitializeListHead(&retryListHead);

    while (!IsListEmpty(&hubExt->DmTimerListHead)) {

        le = RemoveHeadList(&hubExt->DmTimerListHead);

        tmrObj = (PHUB_TIMER_OBJECT) CONTAINING_RECORD(
                   le,
                   struct _HUB_TIMER_OBJECT,
                   EnableLink);

        LOG(HubFdo, LOG_TMR, 'dmT1', tmrObj, le);
        ASSERT_TIMER_OBJECT(HubFdo, tmrObj);

        tmrObj->EnableLink.Blink = NULL;
        tmrObj->EnableLink.Flink = NULL;

        UsbhAssert(HubFdo, tmrObj->State != TimerObj_Busy);
        if (tmrObj->State == TimerObj_Enabled) {

            LOG(HubFdo, LOG_TMR, 'dmE1', tmrObj, tmrObj->ExpireTime);
            UsbhAssert(HubFdo, tmrObj->State == TimerObj_Enabled);

            tmrObj->State = TimerObj_Busy;
            InsertTailList(&retryListHead, &tmrObj->EnableLink);

            if (tmrObj->ExpireTime > 0) {
                LOG(HubFdo, LOG_TMR, 'dmE2', tmrObj, tmrObj->ExpireTime);
                // count down to retry using the same interval as used by UsbhSetTimer()
                tmrObj->ExpireTime -= HUB_TIMER_INTERVAL + ((KeQueryTimeIncrement() - 1)/10000);
                tmrObj->Retry = 1;
            } else {
                LOG(HubFdo, LOG_TMR, 'dmE3', tmrObj, 0);

                KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);

                UsbhEtwLogHubEventWithExtraData(HubFdo, 
                                                &USBHUB_ETW_EVENT_HUB_TIMER_CALLBACK,
                                                tmrObj->PortNumber,
                                                &tmrObj->Tag,
                                                sizeof(tmrObj->Tag));

                tmrObj->Retry = tmrObj->TimeoutFunction(tmrObj,
                                                HubFdo,
                                                tmrObj->PortNumber,
                                                tmrObj->Context,
                                                tmrObj->Parameter1,
                                                tmrObj->Parameter2);

                tmrObj->ExpireTime = tmrObj->MillisecsToExpire;
                
                LOG(HubFdo, LOG_TMR, 'dmRR', tmrObj, tmrObj->Retry);
                KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);
                LOG(HubFdo, LOG_TMR, 'dmE4', tmrObj, 0);
            }
        }

    }

    // restore any items that still need to be run or that have
    // been re-activated
    while (!IsListEmpty(&retryListHead)) {

        le = RemoveHeadList(&retryListHead);

        tmrObj = (PHUB_TIMER_OBJECT) CONTAINING_RECORD(
                   le,
                   struct _HUB_TIMER_OBJECT,
                   EnableLink);

        LOG(HubFdo, LOG_TMR, 'dmR1', tmrObj, le);
        ASSERT_TIMER_OBJECT(HubFdo, tmrObj);

        if (tmrObj->State == TimerObj_Busy) {
            LOG(HubFdo, LOG_TMR, 'dmRB', tmrObj, tmrObj->ExpireTime);
            if (tmrObj->Retry) {
                LOG(HubFdo, LOG_TMR, 'dmR2', tmrObj, 0);
                tmrObj->State = TimerObj_Enabled;
                InsertTailList(&hubExt->DmTimerListHead, &tmrObj->EnableLink);
            } else {
                LOG(HubFdo, LOG_TMR, 'dmR3', tmrObj, 0);
                tmrObj->State = TimerObj_Disabled;
            }
        } else if (tmrObj->State == TimerObj_Enabled) {
            LOG(HubFdo, LOG_TMR, 'dmR4', tmrObj, 0);
            // preempted by re-enable
            InsertTailList(&hubExt->DmTimerListHead, &tmrObj->EnableLink);
        }
    }

    if (hubExt->DmTimerState == HubDmTimer_Disabled) {
        // disable function is waiting
        LOG(HubFdo, LOG_TMR, 'd1>1', 0, 0);
        SET_EVENT(&hubExt->DmTimerEvent);

    } else if (IsListEmpty(&hubExt->DmTimerListHead)) {

        // don't re-schedule timer if the list is empty
        LOG(HubFdo, LOG_TMR, 'dmMT', 0, 0);
        // started->stopped

        if (hubExt->DmTimerState != HubDmTimer_Disabled) {
            hubExt->DmTimerState = HubDmTimer_Stopped;
        }
    } else {
        NTSTATUS tmr_nts;

        LOG(HubFdo, LOG_TMR, 'd>TT', 0, hubExt->DmTimerState);
        UsbhAssert(HubFdo, hubExt->DmTimerState == HubDmTimer_Started);

        tmr_nts = UsbhSetTimer(HubFdo,
                           HUB_TIMER_INTERVAL,
                           &hubExt->DmKtimer,
                           &hubExt->DmTimerDpc,
                           dTMR,
                           FALSE);

        UsbhAssert(HubFdo, tmr_nts != STATUS_DEVICE_BUSY);
        if (NT_ERROR(tmr_nts)) {
            // low resource sim will hit this path
            TEST_TRAP();
            hubExt->DmTimerState = HubDmTimer_Disabled;
            SET_EVENT(&hubExt->DmTimerEvent);
        }  else {
            DereferenceTimer = FALSE;
        }   
    }

    LOG(HubFdo, LOG_TMR, 'dmX1', 0, 0);
    KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);

    if (DereferenceTimer) {
        //dTMR
        UsbhReferenceListRemove(HubFdo, &hubExt->DmKtimer);
    }
}


NTSTATUS
UsbhTimerPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Allocs what we need for our DmTimer.

    called in context of PnP Start IRP

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));
    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_TMR, 'dmST', 0, 0);
    InitializeListHead(&hubExt->DmTimerListHead);
    KeInitializeSpinLock(&hubExt->TimerObjSpin);
    INIT_EVENT_NOT_SIGNALED(&hubExt->DmTimerEvent);

    do {
        KeInitializeDpc(&hubExt->DmTimerDpc,
                        UsbhDmTimerDpc,
                        HubFdo);
        KeInitializeTimer(&hubExt->DmKtimer);

        nts = UsbhReferenceListAdd(HubFdo, &hubExt->DmTimerListHead, DMtm);

        if (NT_ERROR(nts)) {
            SET_EVENT(&hubExt->DmTimerEvent);
            break;
        }   
        
        hubExt->FdoFlags.DmTimerInit = 1;
        hubExt->DmTimerState = HubDmTimer_Stopped;
        
    } WHILE (0);
    
    LOG(HubFdo, LOG_TMR, 'Atmr', 0, nts);

    return nts;
}


VOID
UsbhTimerPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Cleans up in context of remove(stop)

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));
    UsbhAssertPassive();
    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_TMR, 'dmSP', HubFdo, 0);

    if (!hubExt->FdoFlags.DmTimerInit) {
        return;
    }

    UsbhDmDisableTimer(HubFdo);

    hubExt->FdoFlags.DmTimerInit = 0;

        //DMtm
    LOG(HubFdo, LOG_TMR, 'dmS1', HubFdo, 0);
    UsbhReferenceListRemove(HubFdo, &hubExt->DmTimerListHead);

    return;
}


VOID
UsbhDmDisableTimer(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Stops general purpose timer function and waits for DPC to complete

Arguments:


--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    BOOLEAN inQ;
    HUB_DMTIMER_STATE oldState;
    KIRQL irql;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->TimerObjSpin, &irql);

    //UsbhAssert(HubFdo, hubExt->DmTimerState != HubDmTimer_Disabled);
    if (hubExt->DmTimerState == HubDmTimer_Disabled) {
        KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
        // low resource will hit this path
        return;        
    }

    oldState = hubExt->DmTimerState;
    hubExt->DmTimerState = HubDmTimer_Disabled;

    LOG(HubFdo, LOG_TMR, 'dmTc', HubFdo, oldState);
    // oldState = hubExt->DmTimerState
    // hubExt->DmTimerState = HubDmTimer_Disabled
    if (oldState == HubDmTimer_Started) {
        //started -> disabled
        LOG(HubFdo, LOG_TMR, 'dmT-', HubFdo, oldState);
        KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);

        inQ = KeCancelTimer(&hubExt->DmKtimer);
        if (inQ) {
            LOG(HubFdo, LOG_TMR, 'd1>1', HubFdo, oldState);
            UsbhReferenceListRemove(HubFdo, &hubExt->DmKtimer);
            SET_EVENT(&hubExt->DmTimerEvent);
        }
    } else {
        KeReleaseSpinLock(&hubExt->TimerObjSpin, irql);
        LOG(HubFdo, LOG_TMR, 'd3>1', HubFdo, oldState);
        SET_EVENT(&hubExt->DmTimerEvent);
    }// stopped -> disabled

    LOG(HubFdo, LOG_TMR, 'dmWt', HubFdo, oldState);
    WAIT_EVENT(&hubExt->DmTimerEvent);

}

