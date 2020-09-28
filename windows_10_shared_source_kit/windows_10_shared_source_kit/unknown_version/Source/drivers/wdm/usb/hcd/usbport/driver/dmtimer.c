/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    dmtimer.c

Abstract:

    This module implements our 'deadman' timer DPC.
    This is our general purpose timer we use to deal with
    situations where the controller is not giving us
    interrupts.

    examples:
        root hub polling.
        dead controller detection

Environment:

    kernel mode only

Notes:

Revision History:

    01-01-00 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "dmtimer.tmh"
#endif

VOID
USBPORT_DM_TimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies FdoDeviceObject.

    SystemArgument1 - not used.

    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT      fdoDeviceObject = DeferredContext;
    PDEVICE_EXTENSION   devExt;
    ULONG               mpOptionFlags;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

#if DBG
    {
        LARGE_INTEGER t;

        KeQuerySystemTime(&t);
        LOGENTRY(fdoDeviceObject, LOG_NOISY, 'dmTM', fdoDeviceObject, t.LowPart, t.HighPart);
    }
#endif

#ifdef EN_TEST_CODE
    if (devExt->TestWorkState == t_work_run) {
        devExt->TestWorkState = t_work_run_again;
        USBPORT_QueueTestWorkerItem(fdoDeviceObject);
    }        
#endif //EN_TEST_CODE

    // This runs regardless of hardware or power state
    //
    USBPORT_ProcessNeoStateChangeList(fdoDeviceObject);

    // Skip timer is set when we are in low power
    //
    if (!devExt->Fdo.DmTimerFlags.SkipTimerWork) {

        // USBPORT_ASSERT(!devExt->Fdo.DmTimerFlags.DecodesOff);

        MPf_CheckController(devExt);

        if (devExt->Fdo.Flags.PollController == 1 &&
            devExt->Fdo.Flags.ControllerGone == 0) {

            MPf_PollController(devExt);
        }

        //
        // This was added for Wireless USB, which can return STATUS_BUSY
        // on SubmitTransfer even when there aren't any transfers pending,
        // thus there will never be a transfer completion to cause us to 
        // try to submit the transfer again unless we signal the DPC here
        //

        mpOptionFlags = devExt->Fdo.HcOptionFlags;

        if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_BUSY_WHILE_IDLE)) {

            USBPORT_Xdpc_Signal(fdoDeviceObject, &devExt->Fdo.UsbMapDpc, NULL);

        }

        // Call the ISR worker here in the event that the controller is
        // unable to generate interrupts
        //
        USBPORT_Core_TimeoutAllTransfers(fdoDeviceObject);
    }
}


/*
NAME: USB 2.0 Port Routing

I have copiled all comments regarding USB20 and USB11cc (companion controllers) port routing here.


This code replaces USBPORT_SynchronizeControllersStart.  The purpose of this code is to
synchronize the start of the USB20 (ehanced) controller with one or more 'companion'
USB11cc controllers. The goal is to mminimize the amount of bouncing that USB devices may do between
USB11cc and USB20 controllers due to the port routing feature on the root ports.


Each instance of the USB hub diver for the 'root hub' registers a notification function
(call it rhinit).

When this notification function is invoked it indicates that the hub driver may safely access
the port status registers.

The USB 20 controller has a 'config' flag that controls port routing on shared ports -- it has two
states:
0 - ports not owned by EHCI (USB20) (theoretically owned by the CCs)
1 - ports owned by EHCI(USB20)

The default state is 0 until the EHCI driver loads and sets it..

The problem with the config flag is that it only reflects the state of the hardware.  We introduce
the Usb2_cfg_flag to solve this.

The Usb2_cfg_flag reflects the state of the hardware config flag and the host drivers for EHCI
(USB20) -- the hub and miniport.

The Usb2_cfg_flag is set to 1 only when the hardware cfg flag is 1 and when the ehci driver is loaded
and the usb20 root hub has posted an interrupt transfer.  This indicates that the stack can
effectively transfer control (or route) the ports if a USB 1.1 only device is detected.

The Usb2_cfg_flag is reset to zero whenever the the USB20 root hub is pnp_stopped or removed or
the ehci driver is pnp_stopped or removed.

The rhinit_callback_flush function invokes the rhinit callbacks based on the state of the
Usb2_cfg_flag and the type of controller.


rhinit_callback_flush logic is:


                         USB20      |       USB11cc
------------------+-----------------+---------------------
Usb2_cfg_flag = 1 |  Usb2RootInit   |     UsbAllStart
------------------+-----------------+---------------------
Usb2_cfg_flag = 0 |   Usb2Start     |  Usb2WaitforCfgFlag
------------------+-----------------+---------------------


Usb2RootInit
case 1: Flush in context of root hub interrupt (posting IRQ), start all

Usb2Start
case 2: Normal EHCI start, USB20 callback invoked.  Flush will occur on next transition of
usb2_cfg_flag (when root hub posts irq).

UsbAllStart
case 3: ehci & hub is ready -- all USB11 callbacks are invoked.

Usb2WaitforCfgFlag
case 4: wait for ehci, ehci and/or root hub not ready yet.
        * USB2 ROOTHUB could be disabled
        * EHCI could be disabled
        * USB2 ROOTHUB still starting on another thread or waiting on callback
        * EHCI starting on another thread
        - flush will occur on next transition of usb2_cfg_flag.(when root hub posts irq)

typedef enum _USB2_CFG_STATE {
    Unitialized = 0,
    Usb2RootInit = 1,
    Usb2Start = 2,
    UsbAllStart = 3,
    Usb2WaitforCfgFlag = 4,
    NotUsb2
} USB2_CFG_STATE;


*/


ULONG
ReadUsb2CfgFlag(
    PDEVICE_OBJECT Fdo
    )
/*++

Routine Description:

    returns the state of the Usb2_cfg_flag for this controller.  If USB 2 hardware registers not
    accessible zero is returned.

Arguments:

    Fdo - FDO device object a usb controller may be USB20 or USB11.


Return Value:

    state of the Usb2_cfg_flag.

    value of 1 means both ehci and root hub are initialized.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT usb2Fdo = NULL;
    // default is 0
    ULONG cf = 0;
    USBPORT_PNP_STATE pnpState;

    LOGENTRY(Fdo, LOG_PNP, 'rCF1', 0, 0, Fdo);
#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "Fdo cannot be NULL; see definition of LOGENTRY")
    if (Fdo != NULL) {
        GET_DEVICE_EXT(devExt, Fdo);
        ASSERT_FDOEXT(devExt);

        if (USBPORT_IS_USB20(devExt)) {
            pnpState = USBPORT_GetPnpState(devExt);
            LOGENTRY(Fdo, LOG_PNP, 'rCF2', 0, pnpState, Fdo);
            if (pnpState == PnpStarted) {
                PDEVICE_OBJECT rhPdo;
                PDEVICE_EXTENSION rhDevExt;
                KIRQL irql;
                LOGENTRY(Fdo, LOG_PNP, 'rCF3', 0, 0, Fdo);
                USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
                if (!devExt->Fdo.DmTimerFlags.DecodesOff) {
                    cf = MPf_ReadCfgFlag(devExt);
                }
                USBPORT_RELEASE_DM_LOCK(devExt, irql);
                LOGENTRY(Fdo, LOG_PNP, 'rCF4', cf, 0, Fdo);
                
                // sync with stop by ref'ing the root pdo
                rhPdo = USBPORT_RefRootHubPdo(Fdo, REF_RhPDO_ReadUsb2CfgFlag);
         
                if (cf && rhPdo != NULL) {
                    GET_DEVICE_EXT(rhDevExt, rhPdo);
                    ASSERT_PDOEXT(rhDevExt);
                    LOGENTRY(Fdo, LOG_PNP, 'rCF5', cf, rhPdo, Fdo);
                    // return the state of the root hub cfg flag
                    USBPORT_DerefRootHubPdo(Fdo, rhPdo, REF_RhPDO_ReadUsb2CfgFlag);
                    return rhDevExt->Pdo.RhCfgFlag;
                } else {
                    if (rhPdo) {
                        USBPORT_DerefRootHubPdo(Fdo, rhPdo, REF_RhPDO_ReadUsb2CfgFlag);
                    }                        
                    // treat as not set
                    LOGENTRY(Fdo, LOG_PNP, 'rCF6', cf, 0, Fdo);
                    cf = 0;
                }
            }

        } else if (devExt->Fdo.Flags.IsCompanionController) {

            usb2Fdo = USBPORT_FindUSB2Controller(Fdo);
            LOGENTRY(Fdo, LOG_PNP, 'rCF7', cf, 0, usb2Fdo);
            // will not be found if ehci not loaded yet
            if (usb2Fdo) {
                LOGENTRY(Fdo, LOG_PNP, 'rCFo', cf, usb2Fdo, Fdo);
                cf = ReadUsb2CfgFlag(usb2Fdo);
                ObDereferenceObject(usb2Fdo);
            }
        }

    }

    LOGENTRY(Fdo, LOG_PNP, 'rCFx', cf, 0, Fdo);

    return cf;
}


VOID
USBPORT_FlushRhCallbacks(
    PDEVICE_OBJECT Fdo
    )
/*++

Routine Description:

    This routine invokes the rhcallbacks for all controllers

Arguments:

    Fdo - FDO device object for a usb controller may be USB20 or USB11.


Return Value:

   none

--*/
{
    ULONG usb2cfgFlag;
    PDEVICE_EXTENSION devExt;
    USB2_CFG_STATE cfgState;
    PDEVICE_OBJECT usb2Fdo = NULL;

    GET_DEVICE_EXT(devExt, Fdo);
    ASSERT_FDOEXT(devExt);

    usb2cfgFlag = ReadUsb2CfgFlag(Fdo);

    if (USBPORT_IS_USB20(devExt)) {
        cfgState = usb2cfgFlag == 0 ? Usb2Start : Usb2RootInit;
        usb2Fdo = Fdo;
    } else if (devExt->Fdo.Flags.IsCompanionController) {
        cfgState = usb2cfgFlag == 0 ? Usb2WaitforCfgFlag : UsbAllStart;
        usb2Fdo = USBPORT_FindUSB2Controller(Fdo);
    } else {
        cfgState = NotUsb2;
    }

// This line disables sync for ehci and companions
// cfgState = NotUsb2;

    LOGENTRY(Fdo, LOG_PNP, 'frhS', cfgState, usb2cfgFlag, Fdo);

    devExt->Fdo.CfgState = cfgState;

    switch (cfgState) {

    case Usb2WaitforCfgFlag:
        break;

    case Usb2Start:
        USBPORT_ASSERT(usb2Fdo);
        USBPORT_ASSERT(usb2Fdo == Fdo);
        USBPORT_InvokeRhCallback(usb2Fdo);
        break;

    case Usb2RootInit:
    case UsbAllStart:
        {
        PDEVICE_RELATIONS devR;
        ULONG i;

        USBPORT_ASSERT(usb2Fdo);
        if (usb2Fdo == NULL) {
            break;
        }
        // find companions and do the callbacks
        devR = USBPORT_FindCompanionControllers(usb2Fdo,
                                                TRUE,
                                                TRUE);
        LOGENTRY(Fdo, LOG_PNP, 'frh1', cfgState, usb2Fdo, Fdo);
        for (i=0; devR && i< devR->Count; i++) {

            PDEVICE_OBJECT ccFdo;
            PDEVICE_EXTENSION ccDevExt;

            ccFdo = devR->Objects[i];

            LOGENTRY(Fdo, LOG_PNP, 'frh2', ccFdo, 0, 0);

            GET_DEVICE_EXT(ccDevExt, ccFdo);
            ASSERT_FDOEXT(ccDevExt);

            USBPORT_InvokeRhCallback(ccFdo);
            // this prevents the CCFdos from unloading while we flush the callbacks
            ObDereferenceObject(ccFdo);
        }

        if (devR) {
            UsbFreePool(devR);
        }
        }
        break;

    case NotUsb2:
        USBPORT_InvokeRhCallback(Fdo);
        break;
    }

    // done with the USB2 stack
    // if we are not in the context of the USB2 controller driver instance we must
    // dereference the USB2 FDO here
    LOGENTRY(Fdo, LOG_PNP, 'frh3', 0, usb2Fdo, Fdo);
    if (usb2Fdo != Fdo && usb2Fdo) {
         ObDereferenceObject(usb2Fdo);
    }

    return;
}


VOID
USBPORT_InvokeRhCallback(
    PDEVICE_OBJECT Fdo
    )
/*++

Routine Description:

    Execute the root hub notification callback for a given FDO if there is one.

Arguments:

    Fdo - FDO device object for a usb controller may be USB20 or USB11.

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt, rhDevExt;
    PDEVICE_OBJECT rhPdo;
    PRH_INIT_CALLBACK cb;
    PVOID context;
    KIRQL irql;

    if (Fdo == NULL) {
        return;
    }

    GET_DEVICE_EXT(devExt, Fdo);
    ASSERT_FDOEXT(devExt);

    rhPdo = USBPORT_RefRootHubPdo(Fdo, REF_RhPDO_InvokeRhCallback);
    if (rhPdo == NULL) {
        // root hub does not exist yet
        return;
    }

    GET_DEVICE_EXT(rhDevExt, rhPdo);
    ASSERT_PDOEXT(rhDevExt);

    KeAcquireSpinLock(&devExt->Fdo.HcSyncSpin, &irql);

    LOGENTRY(Fdo, LOG_PNP, 'syCB', rhDevExt, 0, 0);

    context = rhDevExt->Pdo.HubInitContext;
    cb = rhDevExt->Pdo.HubInitCallback;

    rhDevExt->Pdo.HubInitCallback = NULL;
    rhDevExt->Pdo.HubInitContext = NULL;

    KeReleaseSpinLock(&devExt->Fdo.HcSyncSpin, irql);

    
    if (cb == NULL) {
        // callback is NULL , root hub has been stopped or we've already done a
        // callback so we don't need to do anything
        USBPORT_DerefRootHubPdo(Fdo, rhPdo, REF_RhPDO_InvokeRhCallback);
        return;
    }

    // do the callback
    cb(context);

    USBPORT_DerefRootHubPdo(Fdo, rhPdo, REF_RhPDO_InvokeRhCallback);
}

VOID
USBPORT_InitDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    )
/*++

Routine Description:

    Inialize the deadman timer

Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION   devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(devExt->Fdo.DmTimerState == DmTimer_Uninit);

    KeInitializeSpinLock(&devExt->Fdo.DM_TimerSpin);
    KeInitializeTimer(&devExt->Fdo.DM_Timer);
    KeInitializeDpc(&devExt->Fdo.DM_TimerDpc,
                    USBPORT_DM_TimerDpc,
                    FdoDeviceObject);
    KeInitializeDpc(&devExt->Fdo.DM_SimulateTimerDpc,
                    USBPORT_DM_TimerDpc,
                    FdoDeviceObject);
    devExt->Fdo.DmTimerState = DmTimer_Init;

}



VOID
USBPORT_StartDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    )

/*++

Routine Description:

    Inialize and start the timer

Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;
    LONG                dueTime;    

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    dueTime = -1 * MILLISECONDS_TO_100_NS_UNITS(USBPORT_DM_TIMER_INTERVAL);

    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);

    USBPORT_ASSERT((devExt->Fdo.DmTimerState != DmTimer_Enabled) &&
                   (devExt->Fdo.DmTimerState != DmTimer_Suspended));
    
    // We consider the timer a pending request while it is scheduled.
    //
    USBPORT_Core_IncIoRequestCount(FdoDeviceObject,
                                   &devExt->Fdo.UsbIoDmTimer,
                                   devExt,
                                   UsbIoTag_DmTimer,
                                   FALSE);

    devExt->Fdo.DmTimerState = DmTimer_Enabled;

    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    // Start the timer.
    // 
    KeSetCoalescableTimer(&devExt->Fdo.DM_Timer,
                          RtlConvertLongToLargeInteger(dueTime),
                          USBPORT_DM_TIMER_INTERVAL,
                          USBPORT_DM_TIMER_TOLERABLE_DELAY,
                          &devExt->Fdo.DM_TimerDpc);    


}

VOID
USBPORT_SuspendDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    )

/*++

Routine Description:

    Suspends the deadman timer when powering down

Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);

    USBPORT_ASSERT(devExt->Fdo.DmTimerState == DmTimer_Enabled);
    USBPORT_ASSERT(devExt->Fdo.DmTimerFlags.DecodesOff == 1);

    if (devExt->Fdo.DmTimerState != DmTimer_Enabled) {
        USBPORT_RELEASE_DM_LOCK(devExt, irql);
        return;
    }

    devExt->Fdo.DmTimerState = DmTimer_Suspended;
    
    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    KeCancelTimer(&devExt->Fdo.DM_Timer);

    //
    // the DM Timer also is used to flush any unfinished endpoint state changes
    // which would be syncing with the next SOF if interrupts and decodes hadn't
    // been turned off. 
    // Since we are stopping the timer, we need to flush the endpoint state changes
    // here.
    // 
    USBPORT_ProcessNeoStateChangeList(FdoDeviceObject);

}


VOID
USBPORT_ResumeDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    )

/*++

Routine Description:

    Restarts the deadman timer on power up

Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;
    LONG                dueTime;    

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    dueTime = -1 * MILLISECONDS_TO_100_NS_UNITS(USBPORT_DM_TIMER_INTERVAL);

    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);

    USBPORT_ASSERT(devExt->Fdo.DmTimerState == DmTimer_Suspended);

    if (devExt->Fdo.DmTimerState != DmTimer_Suspended) {        
        USBPORT_RELEASE_DM_LOCK(devExt, irql);
        return;
    }

    devExt->Fdo.DmTimerState = DmTimer_Enabled;

    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    // Start the timer.
    // 
    KeSetCoalescableTimer(&devExt->Fdo.DM_Timer,
                          RtlConvertLongToLargeInteger(dueTime),
                          USBPORT_DM_TIMER_INTERVAL,
                          USBPORT_DM_TIMER_TOLERABLE_DELAY,
                          &devExt->Fdo.DM_TimerDpc);    

}


VOID
USBPORT_StopDM_Timer(
    PDEVICE_OBJECT FdoDeviceObject
    )

/*++

Routine Description:

    stop the timer

Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_PASSIVE();
    LOGENTRY(FdoDeviceObject, LOG_MISC, 'klT1', devExt->Fdo.DmTimerState, 0, 0);
    
    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    if ((devExt->Fdo.DmTimerState != DmTimer_Enabled) && 
        (devExt->Fdo.DmTimerState != DmTimer_Suspended)) {

        USBPORT_RELEASE_DM_LOCK(devExt, irql);    
        // The timer was never started so bypass the stop.
        return;
    }

    devExt->Fdo.DmTimerState = DmTimer_DisablePending;

    USBPORT_RELEASE_DM_LOCK(devExt, irql);    


    // if there is a timer in the queue, remove it
    //
    KeCancelTimer(&devExt->Fdo.DM_Timer);

    //
    // Since there is no way to determine if the DPC for a periodic timer is running or
    // will run after cancelling the timer, all the DPCs must be flushed
    // 
    KeFlushQueuedDpcs();

    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'klT2', devExt->Fdo.DmTimerState, 0, 0);

    devExt->Fdo.DmTimerState = DmTimer_Disabled;

    // Dereference now.
    //
    USBPORT_Core_DecIoRequestCount(FdoDeviceObject, &devExt->Fdo.UsbIoDmTimer, TRUE);        

    USBPORT_RELEASE_DM_LOCK(devExt, irql);    

}
